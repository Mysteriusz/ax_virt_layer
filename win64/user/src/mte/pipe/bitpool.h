/*
	|==============[MTE_SYNC_MAP_INT]==============|
	To initialize:
	1) call [bitpool_create] for [struct _bitpool_desc]

	Bit set when:
	1) Bucket at index is full

	Bit unset when:
	1) Bucket at index is empty
*/
#if !defined(MTE_BITPOOL_INT)
#define MTE_BITPOOL_INT

#include <ax_type.h>
#include <ax_error.h>
#include <ax_memory.h>

#include "mte/core.h"

#include "sync_map.h"

/*
 	Bucket quick load functions
*/
typedef void (*bitpool_bucket_func)(void *from, void *to, u32 size);
static void _bitpool_bucket_func_16byte(
	void *from,
	void *to,
	u32 size
){ /*simd_imax_store_128(from, to);*/ }
static void _bitpool_bucket_func_32byte(
	void *from,
	void *to,
	u32 size
){ simd_imax_store_256(from, *(simd_imax*)to); }
static void _bitpool_bucket_func_64byte(
	void *from,
	void *to,
	u32 size
){ simd_imax_store_512(from, to); }
static void _bitpool_bucket_func_anybyte(
	void *from,
	void *to,
	u32 size
){ memcpy(to, from, size); }
static bitpool_bucket_func _bitpool_bucket_func(
	_in u32 		bucket_size
){
	switch(bucket_size){
	case 16:
		return _bitpool_bucket_func_16byte;
	case 32:
		return _bitpool_bucket_func_32byte;
	case 64:
		return _bitpool_bucket_func_64byte;
	default:
		return _bitpool_bucket_func_anybyte; // Slowest (unaligned/too big for SIMD/register copy)
	}
	return nullptr;
}

/*
 	Bitpool core definitions
*/

/*
 	Bit range of a bitpool
*/
typedef struct _bitpool_prior_range{
	u32		bit_n; // Count of bits in range
	u32		bit_i; // Index of the first bit
} bitpool_prior_range;
typedef struct _bitpool_desc{
 	/*
		Base address of the bucket array
		(count and bucket size depends on [bucket_count] and [bucket_size])
	*/
	u8			*bucket_base;
	u32			bucket_count; // Count of buckets (multiplication of 64)
	u32			bucket_size; // Bucket size in bytes (multiplication of 16)
	struct bitpool_range_desc{
		bitpool_prior_range	low;
		bitpool_prior_range	med;
		bitpool_prior_range	high;
		bitpool_prior_range	real;
	} ranges;
 	/* 
	 	Active bucket count per priority range.
		0 -> real;
		1 -> high;
		2 -> med;
		3 -> low;
	*/
	_Atomic(u32)			presence[4];
 	// TODO: If more functions then make an anonymous struct???
	bitpool_bucket_func	qload;
	sync_map_desc		smap_desc; // sync map signaling each bucket emptiness
} bitpool_desc;

#define bitpool_presence_any(b_p) \
	(simd_cmpz_128(simd_load_128((b_p)->presence)))

#define BITPOOL_BUCKET_AVX128 0x10 // simd_imax_store_128
#define BITPOOL_BUCKET_AVX256 0x20 // simd_imax_store_256
#define BITPOOL_BUCKET_AVX512 0x40 // simd_imax_store_512

static bitpool_prior_range *_bitpool_range_from_prior(
	_in struct bitpool_range_desc	*desc,
	_in enum mte_prior		prior
){
	if (desc == nullptr){
		return nullptr;
	}

	switch(prior){
	case PRIOR_MIN:
	case PRIOR_LOW:
		return &desc->low;
	case PRIOR_MOD:
	case PRIOR_MED:
		return &desc->med;
	case PRIOR_HIGH:
	case PRIOR_VERY_HIGH:
		return &desc->high;
	case PRIOR_REAL:
	case PRIOR_MAX:
		return &desc->real;
	default:
		return nullptr;
	}
}

struct bitpool_perc_desc{
	_in const u8		real_perc;
	_in const u8		high_perc;
	_in const u8		med_perc;
	_in const u8		low_perc;
};
#define bitpool_perc_desc_sum(bp) \
	((bp).real_perc + (bp).high_perc + (bp).med_perc + (bp).low_perc)

#define BITPOOL_PERC_DEFAULT \
	(struct bitpool_perc_desc){ \
		.real_perc = 10, \
		.high_perc = 15, \
		.med_perc = 25, \
		.low_perc = 50, \
	}
#define BITPOOL_PERC_OFFLOAD \
	(struct bitpool_perc_desc){ \
		.high_perc = 10, \
		.med_perc = 20, \
		.low_perc = 70, \
	}

/*
	Create bitpool
*/
axres bitpool_create(
	_in const u32				bucket_count, 
	_in const u32				bucket_size, 
	_in struct bitpool_perc_desc 		perc,
	_out bitpool_desc			**buf
);

/*
	Delete and dispose the bitpool
*/
void bitpool_delete(
	_in bitpool_desc	*bitpool
);

/*
 	Calculate ranges for the sync_map given percentage descriptor
*/
axres bitpool_range_populate(
	_in sync_map_desc			*smap_desc,
	_in struct bitpool_perc_desc 		perc,
	_in_out struct bitpool_range_desc 	*buf
);

/*
 	Blocking bitpool load to priority range
*/
struct bitpool_prior_load_res{
	axres		code;
	u32		index;
} bitpool_prior_load(
	_in bitpool_desc	*bitpool,
	_in void		*bucket,
	_in enum mte_prior  	prior
);

_inline_force void *_bitpool_index_to_bucket(
	_in bitpool_desc			*bitpool,
	_in u32					index
){
	asrt(index < bitpool->bucket_count);
	return offp(bitpool->bucket_base, bitpool->bucket_size * index);
}

/*
 	Convert bitpool anonymous index to [bitpool->presence] index 
*/
_inline_force u8 _bitpool_index_to_presence(
	_in bitpool_desc	*bitpool,
	_in u32			index
){
	asrt(bitpool != nullptr);
	asrt(bitpool->bucket_count <= index);

	if (bitpool->ranges.real.bit_n > 0
	&& index >= bitpool->ranges.real.bit_i){
		return 0;
	}
	if (bitpool->ranges.high.bit_n > 0
	&& index >= bitpool->ranges.high.bit_i){
		return 1;
	}
	if (bitpool->ranges.med.bit_n > 0
	&& index >= bitpool->ranges.med.bit_i){
		return 2;
	}
	if (bitpool->ranges.low.bit_n > 0
	&& index >= bitpool->ranges.low.bit_i){
		return 3;
	}

	// No fallback (bitpool and index are completely unaligned)
	asrt(0);
}

/*
 	Bitpool unload from index of priority
*/
void bitpool_prior_unload(
	_in bitpool_desc	*bitpool,
	_in u32			index
);

#endif // !defined(MTE_BITPOOL_INT)

