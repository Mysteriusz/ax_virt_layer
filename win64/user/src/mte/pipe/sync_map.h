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

#include <ax_type.h>
#include <stdatomic.h>

/*
	Bitmap pointer
*/
typedef _Atomic(u64)* 		sync_map;

/*
	Unspecified thread-safe and lock-free bitmap
*/
typedef struct _sync_map_desc{ _align(64)
	sync_map	map; // Map pointer
	u32		index; // Index of the owner
	u32		size; // Size in bytes of the map
	u8		meta[8];
} sync_map_desc;

#define sync_map_sigi(md_p, i, bi) \
	(atomic_fetch_xor_explicit(&md_p[i], (1ULL << bi), memory_order_release))

#endif // !defined(MTE_SYNC_MAP_INT)

