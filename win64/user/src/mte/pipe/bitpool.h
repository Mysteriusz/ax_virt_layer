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
		return _bitpool_bucket_func_anybyte;
	}
	return nullptr;
}

/*
 	Bitpool core definitions
*/
typedef struct _bitpool_prior_range{
	u32		bit_n;
	u32		bit_i;
} bitpool_prior_range;
typedef struct _bitpool_desc{
	u32			bucket_count; // Best if capacity is multiplier of 64
	u32			bucket_size; // Best if capacity is multiplier of 64
	sync_map_desc		smap_desc;
	bitpool_bucket_func	qload;
	struct bitpool_range_desc{
		bitpool_prior_range	real;
		bitpool_prior_range	high;
		bitpool_prior_range	med;
		bitpool_prior_range	low;
	} ranges;
	u8			*base;
} bitpool_desc;

#define BITPOOL_BUCKET_AVX128 0x10 // simd_imax_store_128
#define BITPOOL_BUCKET_AVX256 0x20 // simd_imax_store_256
#define BITPOOL_BUCKET_AVX512 0x40 // simd_imax_store_512

enum bitpool_prior{
	BITPOOL_PRIOR_REAL,
	BITPOOL_PRIOR_HIGH,
	BITPOOL_PRIOR_MED,
	BITPOOL_PRIOR_LOW,
};
static bitpool_prior_range *_bitpool_range_from_prior(
	_in struct bitpool_range_desc	*desc,
	_in enum bitpool_prior		prior
){
	if (desc == nullptr){
		return nullptr;
	}

	switch(prior){
	case BITPOOL_PRIOR_REAL:
		return &desc->real;
	case BITPOOL_PRIOR_HIGH:
		return &desc->high;
	case BITPOOL_PRIOR_MED:
		return &desc->med;
	case BITPOOL_PRIOR_LOW:
		return &desc->low;
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
bool bitpool_prior_load(
	_in bitpool_desc	*bitpool,
	_in void		*bucket,
	_in enum bitpool_prior  prior
);

#endif // !defined(MTE_BITPOOL_INT)

