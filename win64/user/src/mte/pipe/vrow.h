/*	
	|==============[MTE_SYNC_MAP_INT]==============|
	To initialize:
	1) call [vrow_link_sync] for [struct _vrow_desc]

	Bit set when:
	1) Payload processing starts

	Bit unset when:
	1) Payload processing ends
*/

#if !defined(MTE_VROW_INT)
#define MTE_VROW_INT

#include <stdatomic.h>

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>
#include <pthread.h>

#include "sync_map.h"

/*
 	Vrow processing thread.
	Signals the [vrow->smap] bitmap.
*/
typedef struct _vrow_thread{
	struct _vrow_thread_stack {
		struct _vrow_desc 	*vrow;
		u8 			curr_bank;
	} stack;
	pthread_t 		pthread;
} vrow_thread;

/*
 	Any code using vrow should atomically (vrow_is_closed)
	check it`s close state before accessing to avoid use after free.
*/
typedef struct _vrow_desc{ _align(8)
	u8			base[0x100];
	/*
		Signaling bitmap (described at the beggining)
	*/
	sync_map_desc		smap;
	/*
		Processing thread for the vrow.
	*/
	struct _vrow_thread	thread;
	/*
	 	Bank thread states.

		(LFLFLFLF)
	 	2 status bits per bank (4 banks):
			LF -> is_locked | is_filled

		L -> Processing enabled in this bank
		F -> Payload present in this bank
	*/
	 _Atomic u8	 	states;
	/*
	 	Close request holder.
		On any close vrow thread will exit and set states to empty asap.
	*/
	 _Atomic bool 		closed;
} vrow_desc;

axres vrow_thread_init(
	_in vrow_desc		*vrow,
	_in_out vrow_thread	*th
);

void *vrow_thread_main(
	struct _vrow_thread_stack *stack
);

/*
 	MEMORY LAYOUT:
	
	TODO: describe block internals

	[16 bytes control block]
	[3 * 16 byte data block]
*/
typedef struct _vrow_payload{ _align(16)
	u8	vrow_level; // 1 byte vrow level + 15 bytes control block
	u8	control[0xf];
	u8	data[0x30]; // 48 bytes payload
} vrow_payload;

#define VROW_BANK_SIZE 0x40
#define VROW_STATE_EMPTY 0b10101010

// Check if bank index (bi) is marked as filled [F]
#define vrow_is_filled(v_p, bi) \
	(atomic_load_explicit(&(v_p)->states, memory_order_acquire) & (0b01 << (bi << 1)))
// Switch bank index (bi) filled [F] state
#define vrow_fill_switch(v_p, bi) \
	(atomic_fetch_xor_explicit(&(v_p)->states, (0b01 << (bi << 1)), memory_order_release))

// Check if bank index (bi) is marked as locked [L]
#define vrow_is_locked(v_p, bi) \
	(atomic_load_explicit(&(v_p)->states, memory_order_acquire) & (0b10 << (bi << 1)))
// Switch bank index (bi) locked [L] state
#define vrow_lock_switch(v_p, bi) \
	(atomic_fetch_xor_explicit(&(v_p)->states, (0b10 << (bi << 1)), memory_order_release))

#define vrow_is_closed(v_p) \
	(atomic_load_explicit(&(v_p)->closed, memory_order_acquire))
#define vrow_close(v_p) \
	(atomic_store_explicit(&(v_p)->closed, true, memory_order_release))

/*
   	Blocking safe thread bit allocation.

	Return index of the allocation (0-3)
*/
_inline_force u8 vrow_alloc_thread(
	_in vrow_desc		*vrow
){
	/*
	 	0x50 -> ~VROW_STATE_EMPTY (0b01010101)
	*/

	u8 state = 0;
	u8 i = 0;
	while(1){
		// Load state for masking
		state = atomic_load_explicit(&vrow->states, memory_order_acquire);
		// If state has no empty banks
		if ((state & 0x50) == 0x50 || vrow_is_closed(vrow)){
			_mm_pause();
			continue;
		}

		// Calculate index
		i = __builtin_ctzl(~(state & 0x50) & 0x50) >> 1;

		// Validate states with the initialy loaded one
		if (atomic_compare_exchange_weak(
			&vrow->states, &state,
			state ^ (0b01 << (i << 1)))
		){
			break;
		}
	}
	return i;
}

axres vrow_create(
	_in_opt sync_map_desc	*smap,
	_out vrow_desc		**buf
);

void vrow_delete(
	_in vrow_desc		*vrow
);

bool vrow_link_sync(
	_in vrow_desc 		*vrow,
	_in sync_map_desc	map
);

/*
 	Blocking load to indexed bank.
*/
bool vrow_bank_load(
	_in vrow_desc		*vrow,
	_in u8			bank_i,
	_in vrow_payload	payload
);

#endif // !defined(MTE_VROW_INT)

