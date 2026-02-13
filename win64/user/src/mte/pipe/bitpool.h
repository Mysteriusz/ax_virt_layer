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
){ simd_imax_store_256(to, *(simd_imax*)from); }
static void _bitpool_bucket_func_64byte(
	void *from,
	void *to,
	u32 size
){ simd_imax_store_512(to, from); }
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
typedef struct _bitpool_prior_range{ _align(16)
	u32		bit_n; // Count of bits in range
	u32		bit_i; // Index of the first bit
	const u32	bit_disp; // Disposition per lowest priority 
} bitpool_prior_range;
typedef struct _bitpool_desc{
 	/* 
	 	Active bucket count per priority range.
		0 -> real;
		1 -> high;
		2 -> med;
		3 -> low;
	*/
	_Atomic(u32)		presence[4];
 	/*
		Base address of the bucket array
		(count and bucket size depends on [bucket_count] and [bucket_size])
	*/
	u8			*bucket_base;
	u32			bucket_count; // Count of buckets (multiplication of 64)
	u32			bucket_size; // Bucket size in bytes (multiplication of 16)
	bitpool_prior_range 	ranges[4];
 	// TODO: If more functions then make an anonymous struct???
	bitpool_bucket_func	qload;
	sync_map_desc		smap_desc; // sync map signaling each bucket emptiness
} bitpool_desc;

/*
 	Since presence is atomic it should be fetched using atomic operations.

	TODO: Fetch each presence atomic, then create 128bit vector and compare.
*/
#define bitpool_presence_any(b_p) \
	(simd_cmpz_128(simd_load_128((b_p)->presence)))

#define BITPOOL_BUCKET_AVX128 0x10 // simd_imax_store_128
#define BITPOOL_BUCKET_AVX256 0x20 // simd_imax_store_256
#define BITPOOL_BUCKET_AVX512 0x40 // simd_imax_store_512

static u8 _bitpool_prior_to_presence(
	_in bitpool_desc		*bitpool,
	_in enum mte_prior		prior
){
	asrt(bitpool != nullptr);

	switch(prior){
	case PRIOR_MIN:
	case PRIOR_LOW:
		return 3;
	case PRIOR_MOD:
	case PRIOR_MED:
		return 2;
	case PRIOR_HIGH:
	case PRIOR_VERY_HIGH:
		return 1;
	case PRIOR_REAL:
	case PRIOR_MAX:
		return 0;
	default:
		asrt(0);
	}
}

/*
 	Convert [bitpool] bit index to [bitpool->presence] index 
*/
static u8 _bitpool_index_to_presence(
	_in bitpool_desc	*bitpool,
	_in u32			index
){
	asrt(bitpool != nullptr);
	asrt(bitpool->bucket_count > index);

	for (u8 i = 0; i < 4; i++){
		if (bitpool->ranges[i].bit_n > 0
		&& index >= bitpool->ranges[i].bit_i){
			return i;
		}
	}

	// No fallback (bitpool and index are completely unaligned)
	asrt(0);
}

/*
	Convert [bitpool] bit index to bucket pointer
*/
_inline_force void *_bitpool_index_to_bucket(
	_in bitpool_desc			*bitpool,
	_in u32					index
){
	asrt(index < bitpool->bucket_count);
	return offp(bitpool->bucket_base, bitpool->bucket_size * index);
}

typedef u8 	bitpool_perc_desc[4];

#define bitpool_perc_desc_sum(b_p) \
	(b_p[0] + b_p[1] + b_p[2] + b_p[3])

#define BITPOOL_PERC_DEFAULT \
	(bitpool_perc_desc){ \
		[0] /*real_perc*/ = 10, \
		[1] /*high_perc*/ = 15, \
		[2] /*med_perc*/ = 25, \
		[3] /*low_perc*/ = 50, \
	}
#define BITPOOL_PERC_OFFLOAD \
	(bitpool_perc_desc){ \
		[1] /*high_perc*/ = 10, \
		[2] /*med_perc*/ = 20, \
		[3] /*low_perc*/ = 70, \
	}

/*
	Create bitpool
*/
axres bitpool_create(
	_in const u32				bucket_count, 
	_in const u32				bucket_size, 
	_in bitpool_perc_desc 			perc,
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
	_in bitpool_perc_desc 			perc,
	_in_out bitpool_prior_range 		buf[4]
);

/*
 	Blocking bitpool load to priority range
*/
struct bitpool_prior_load_res{ _align(8)
	axres		code;
	u32		index;
} bitpool_prior_load(
	_in bitpool_desc	*bitpool,
	_in void		*bucket,
	_in enum mte_prior  	prior
);

/*
 	Bitpool unload from index of priority
*/
void bitpool_prior_unload(
	_in bitpool_desc	*bitpool,
	_in u32			index
);

/*
 	Unload based on dispostitions per range
*/
struct bitpool_prior_unload_disp_res{ _align(8)
	bool 		succ;
	u8		presence_i; // Index of the unloaded presence (priority which took the disposition)
	u8		bucket_i; // Index of the unloaded bucket
} bitpool_prior_unload_disp(
	_in bitpool_desc	*bitpool,
	_in_out u32 		disp_presence[4]
);

#endif // !defined(MTE_BITPOOL_INT)

