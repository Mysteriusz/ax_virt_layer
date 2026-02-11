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
#include <pthread.h>

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

#include "mte/ir/ir.h"

#include "sync_map.h"

/*
 	Vrow processing thread.
	Signals the [vrow->smap] bitmap.
*/
typedef struct _vrow_thread{ _align(64)
	struct _vrow_thread_stack {
		ir_context		*ir;
		struct _vrow_desc 	*vrow;
		u8 			curr_bank;
	} stack;
	pthread_t 		pthread;
} vrow_thread;

/*
 	Any code using vrow should atomically (vrow_is_closed)
	check it`s close state before accessing to avoid use after free.
*/
typedef struct _vrow_desc{
	u8			base[0x100]; // 4 Cache lines
	/*
		Signaling bitmap (described at the beggining)
	*/
	sync_map_ref		smap; // 1 Cache line
	/*
		Processing thread for the vrow.
	*/
	struct _vrow_thread	thread; // 1 Cache line
	/*
	 	Bank thread states.

		(AFAFAFAF)
	 	2 status bits per bank (4 banks):
			AF -> is_active | is_filled

		A -> Thread active in this bank
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
	_in ir_context		*ir,
	_in_out vrow_thread	*th
);

void *vrow_thread_main(
	struct _vrow_thread_stack *stack
);

/*
	Payload can be abstracted tho it has to be polymorhable with structure below
*/
typedef struct _vrow_payload{ _align(16)
	/*
	 	[CONTROL BLOCK]

	 	1 byte payload state
		1 byte payload priority
		14 bytes abstraction defined
	*/
	u8			state;
	enum mte_prior		priority;
	u8			control[0xe];
	/*
	 	[DATA BLOCK]

		3 * 16 bytes data block
	*/
	u8			data[0x30]; // 48 bytes payload
} vrow_payload;

#define VROW_BANK_SIZE 0x40
#define VROW_STATE_EMPTY 0

// Check if bank index (bi) is marked as filled [F]
#define vrow_is_filled(v_p, bi) \
	(atomic_load_explicit(&(v_p)->states, memory_order_acquire) & (0b01 << (bi << 1)))
// Switch bank index (bi) filled [F] state
#define vrow_fill_switch(v_p, bi) \
	(atomic_fetch_xor_explicit(&(v_p)->states, (0b01 << (bi << 1)), memory_order_release))

// Check if bank index (bi) is marked as active [A]
#define vrow_is_active(v_p, bi) \
	(atomic_load_explicit(&(v_p)->states, memory_order_acquire) & (0b10 << (bi << 1)))
// Switch bank index (bi) active [A] state
#define vrow_active_switch(v_p, bi) \
	(atomic_fetch_xor_explicit(&(v_p)->states, (0b10 << (bi << 1)), memory_order_release))

#define vrow_is_closed(v_p) \
	(atomic_load_explicit(&(v_p)->closed, memory_order_acquire))
#define vrow_close(v_p) \
	(atomic_store_explicit(&(v_p)->closed, true, memory_order_release))

/*
	Flush entire vrow structure.

	MAY be dangerous if used on vrow with a thread!
*/
inline void _vrow_force_flush(
	_in vrow_desc		*vrow
){
	if (vrow == nullptr){
		return;
	}

	simd_imax_zero_512(vrow->base);
	simd_imax_zero_512(offp(vrow->base, 64));
	simd_imax_zero_512(offp(vrow->base, 128));
	simd_imax_zero_512(offp(vrow->base, 192));

	// Force all threads and payload states to default.
	atomic_store_explicit(&vrow->states, VROW_STATE_EMPTY, memory_order_release);
}

axres vrow_create(
	_in ir_context		*ir,
	_in_opt	u32		smap_i,
	_in_opt sync_map_desc	*smap,
	_out vrow_desc		**buf
);

void vrow_delete(
	_in vrow_desc		*vrow
);

bool vrow_link_sync(
	_in vrow_desc 		*vrow,
	_in sync_map_desc	*map
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

