#if !defined(MTE_VROW_INT)
#define MTE_VROW_INT

#include <stdatomic.h>

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

/*
 	Any code using vrow should atomically (vrow_is_closed)
	check it`s close state before accessing to avoid use after free.
*/
typedef struct _vrow_desc{ _align(8)
	u8			base[0x100];
	/*
	 	Bank thread states.

		(LFLFLFLF)
	 	2 status bits per bank (4 banks):
			LF -> is_locked | is_filled

		L -> Processing thread not available
		F -> Payload present in bank
	*/
	 _Atomic u8	 	states;
	/*
	 	On-Close usage states.

		(AAAAAAAC)

		A -> Empty action bit (1 if used) ('allocate' using vrow_alloc_action(vrow))
		C -> Close requested
	*/
	 _Atomic u8		close;
} vrow_desc;

/*
 	MEMORY LAYOUT:
	
	TODO: describe block internals

	[16 bytes control block]
	[3 * 16 byte data block]
*/
typedef struct _vrow_payload{ _align(16)
	u8	control[0x10]; // 16 bytes control block
	u8	data[0x30]; // 48 bytes payload
} vrow_payload;

#define VROW_BANK_SIZE 0x40
#define VROW_STATE_EMPTY 0b10101010
#define VROW_CLOSE_EMPTY 0b00000001

/*
 	Find first non-filled bank and return it as a mask

	Example:
	For state mask of: 0b01010001
	Output would be: 0b00000100 (non-filled bank would be set to 0b01)

*/
#define vrow_is_any(s_ptr) \
	(~(atomic_load_explicit(s_ptr, memory_order_acquire) & 0b01010101))

// Check if bank index (bi) is marked as filled [F]
#define vrow_is_filled(vrow_ptr, bi) \
	(atomic_load_explicit(&(vrow_ptr)->states, memory_order_acquire) & (0b01 << (bi << 1)))
// Switch bank index (bi) filled [F] state
#define vrow_fill_switch(vrow_ptr, bi) \
	(atomic_fetch_xor_explicit(&(vrow_ptr)->states, (0b01 << (bi << 1)), memory_order_release))

// Check if bank index (bi) is marked as locked [L]
#define vrow_is_locked(vrow_ptr, bi) \
	(atomic_load_explicit(&(vrow_ptr)->states, memory_order_acquire) & (0b10 << (bi << 1)))
// Switch bank index (bi) locked [L] state
#define vrow_lock_switch(vrow_ptr, bi) \
	(atomic_fetch_xor_explicit(&(vrow_ptr)->states, (0b10 << (bi << 1)), memory_order_release))

// Switch action bit at index (bi) (1-8)
#define vrow_action_switch(vrow_ptr, bi) \
	(atomic_fetch_xor_explicit(&(vrow_ptr)->states, (1 << bi), memory_order_release))

// Check [C] bit of close was issued
#define vrow_is_closed(vrow_ptr) \
	(atomic_load_explicit(&(vrow_ptr)->close, memory_order_acquire) & 1)
// Switch [C] bit of the close
#define vrow_close(vrow_ptr) \
	(atomic_fetch_or_explicit(&(vrow_ptr)->close, 1, memory_order_release))

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

/*
 	Non-blocking safe action bit allocation.

	Return index of the allocation (1-8)
*/
_inline_force u8 vrow_alloc_action(
	_in vrow_desc		*vrow
){
	/*
	 	0xfe -> ~VROW_CLOSE_EMPTY (0b11111110)
	*/
	u8 i = 0;
	u8 close = 0;
	while(1){
		// Load close for masking
		close = atomic_load_explicit(&vrow->close, memory_order_acquire);
		// No empty action bits or closed
		if (!(~close & 0xfe) || vrow_is_closed(vrow)){
			return 0;
		}

		// Calculate index
		i = __builtin_ctzl(~(close & 0xfe) & 0xfe);

		// Validate close with the initialy loaded one
		if (atomic_compare_exchange_weak(
			&vrow->close, &close,
			close ^ (1 << i))
		){
			break;
		}
	}
	return i;
}

axres vrow_create(
	_out vrow_desc		**buf
);

void vrow_delete(
	_in vrow_desc		*vrow
);

/*
 	Blocking payload copy to first available bank
*/
volatile bool vrow_load(
	_in vrow_desc		*vrow,
	_in vrow_payload	payload
);

/*
 	Blocking payload copy to indexed bank
*/
volatile bool vrow_bank_load(
	_in vrow_desc		*vrow,
	_in u8			bank_i,
	_in vrow_payload	payload
);

/*
 	Non-blocking unload of indexed bank
*/
volatile bool vrow_bank_unload(
	_in vrow_desc		*vrow,
	_in u8			bank_i	
);

#endif // !defined(MTE_VROW_INT)

