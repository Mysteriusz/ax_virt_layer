#if !defined(MTE_VROW_INT)
#define MTE_VROW_INT

#include <stdatomic.h>

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

typedef struct _vrow_desc{ _align(8)
	u8			base[0x100];
	/*
		(LFLFLFLF)
	 	2 status bits per bank (4 banks):
			LF -> is_locked | is_filled

		L -> Processing thread not available
		F -> Payload present in bank
	*/
	 _Atomic u8	 	states; // 2 status bits per bank (4 banks)
} vrow_desc;

/*
 	MEMORY LAYOUT:
	
	TODO: describe block internals

	[16 bytes control block]
	[16 byte data block]
	[16 byte data block]
	[16 byte data block] 
*/
typedef struct _vrow_payload{ _align(16)
	u8	control[0x10]; // 16 bytes control block
	u8	data[0x30]; // 48 bytes payload
} vrow_payload;

#define VROW_BANK_SIZE 0x40

/*
 	Find first non-filled bank and return it as a mask

	Example:
	For state mask of: 0b01010001
	Output would be: 0b00000100 (non-filled bank would be set to 0b01)

*/
#define vrow_is_any(s_ptr) \
	(~(atomic_load_explicit(s_ptr, memory_order_acquire) & 0b01010101))

// Check if bank index (bi) is marked as filled
#define vrow_is_filled(s_ptr, bi) \
	(atomic_load_explicit(s_ptr, memory_order_acquire) & (0b01 << (bi << 1)))
// Switch bank index (bi) filled state
#define vrow_fill_switch(s_ptr, bi) \
	(atomic_fetch_xor_explicit(s_ptr, (0b01 << (bi << 1)), memory_order_release))

// Check if bank index (bi) is marked as locked
#define vrow_is_locked(s_ptr, bi) \
	(atomic_load_explicit(s_ptr, memory_order_acquire) & (0b10 << (bi << 1)))
// Switch bank index (bi) locked state
#define vrow_lock_switch(s_ptr, bi) \
	(atomic_fetch_xor_explicit(s_ptr, (0b10 << (bi << 1)), memory_order_release))

axres init_vrow(
	_out vrow_desc		**buf
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

