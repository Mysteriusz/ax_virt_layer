#if !defined(MTE_BITPOOL_INT)
#define MTE_BITPOOL_INT

#include <ax_type.h>
#include <ax_error.h>
#include <ax_memory.h>

#include "sync_map.h"

typedef struct _bitpool_prior_range{
	u32		bit_n;
	u32		from;
} bitpool_prior_range;
typedef struct _bitpool_desc{
	u32			capacity; // Best if capacity is multiplier of 64
	sync_map_desc		smap;
	struct bitpool_range_desc{
		bitpool_prior_range	real;
		bitpool_prior_range	high;
		bitpool_prior_range	med;
		bitpool_prior_range	low;
		u32			real_count;
		u32			high_count;
		u32			med_count;
		u32			low_count;
	} ranges;
} bitpool_desc;

struct bitpool_desc_perc{
	_in const u8		real_perc;
	_in const u8		high_perc;
	_in const u8		med_perc;
	_in const u8		low_perc;
};
#define bitpool_desc_perc_sum(bp) \
	((bp).real_perc + (bp).high_perc + (bp).med_perc + (bp).low_perc)

axres bitpool_create(
	_in const u32				capacity, 
	_in struct bitpool_desc_perc 		perc,
	_out bitpool_desc			**buf
);
void bitpool_delete(
	_in bitpool_desc	*bitpool
);

axres bitpool_range_populate(
	_in sync_map_desc			*smap,
	_in struct bitpool_desc_perc 		perc,
	_in_out struct bitpool_range_desc 	*buf
);

#endif // !defined(MTE_BITPOOL_INT)

