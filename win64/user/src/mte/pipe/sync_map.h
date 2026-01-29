/*
 	Any interface including this map should describe flow of it`s usage.

	Example explenation:

	|==============[MTE_SYNC_MAP_INT]==============|
	To initialize:
	1) call [foo] for [struct foo]

	Bit set when:
	1)
	2)

	Bit unset when:
	1)
	2)
*/
#if !defined(MTE_SYNC_MAP_INT)
#define MTE_SYNC_MAP_INT

#include <stdatomic.h>

#include <ax_type.h>
#include <ax_error.h>
#include <ax_memory.h>

/*
	Bitmap pointer
*/
typedef _Atomic(u64)* 		sync_map;

/*
	Unspecified thread-safe and lock-free bitmap descriptor
*/
typedef struct _sync_map_desc{ _align(64)
	sync_map	map; // Map pointer
	u32		index; // Index of the owner
	u32		size; // Size in bytes of the map
	const bool 	owner;
	u8		meta[7];
} sync_map_desc;

// Unset map descriptor (md_p) at map index (i) and bit modulo (bi)
#define sync_map_sigoff(md_p, i, bi) \
	(atomic_fetch_and_explicit(&md_p[i], ~(1ULL << bi), memory_order_release))

// Set map descriptor (md_p) at map index (i) and bit modulo (bi)
#define sync_map_sigon(md_p, i, bi) \
	(atomic_fetch_or_explicit(&md_p[i], (1ULL << bi), memory_order_release))

// Signal (switch) map descriptor (md_p) at map index (i) and bit modulo (bi)
#define sync_map_sig(md_p, i, bi) \
	(atomic_fetch_xor_explicit(&md_p[i], (1ULL << bi), memory_order_release))

axres sync_map_init(
	_in u32 		size,
	_in_out sync_map_desc	*buf
);
void sync_map_disp(
	_in sync_map_desc	*smap
);

axres sync_map_copy(
	_in sync_map_desc	*smap,
	_in_out sync_map_desc	*buf
);

#endif // !defined(MTE_SYNC_MAP_INT)

