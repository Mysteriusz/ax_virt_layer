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
	u32		size; // Size in bytes of the map
} sync_map_desc;

/*
	Map descriptor reference for member
*/
typedef struct _sync_map_ref{ _align(64)
	sync_map	map; // Map pointer
	u32		quad_index; // Quad index of this member
	u32		byte_index; // Index of this member
	u32		bit_index; // Bit index of this member
	u32		size; // Size in bytes of the map
} sync_map_ref;

// Unset map reference (mr_p)
#define sync_map_sigoff(mr_p) \
	(atomic_fetch_and_explicit(&((mr_p)->map[(mr_p)->quad_index]), ~(1ULL << (mr_p)->bit_index), memory_order_release))

// Set map reference (mr_p)
#define sync_map_sigon(mr_p) \
	(atomic_fetch_or_explicit(&((mr_p)->map[(mr_p)->quad_index]), (1ULL << (mr_p)->bit_index), memory_order_release))

// Signal (switch) in map reference (mr_p)
#define sync_map_sig(mr_p) \
	(atomic_fetch_xor_explicit(&((mr_p)->map[(mr_p)->quad_index]), (1ULL << (mr_p)->bit_index), memory_order_release))

// Check signal in map reference (mr_p)
#define sync_map_issig(mr_p) \
	(atomic_load_explicit(&((mr_p)->map[(mr_p)->quad_index]), memory_order_acquire) & (1ULL << (mr_p)->bit_index))

axres sync_map_init(
	_in u32 		size,
	_in_out sync_map_desc	*buf
);
void sync_map_disp(
	_in sync_map_desc	*smap
);

#define sync_map_ind2byte(i) \
	((i) / sizeof(u64))
#define sync_map_ind2bit(i) \
	((i) % 64)
#define sync_map_ind2quad(i) \
	((i) / 64)

axres sync_map_ref_init(
	_in sync_map_desc	*smap,
	_in u32			index, // Member index for the refernece
	_in_out sync_map_ref	*buf
);

/*
 	Sign first non-signaled bit if found
*/
bool sync_map_sig_first(
	_in u32			from,
	_in sync_map_desc	*smap
);

#endif // !defined(MTE_SYNC_MAP_INT)

