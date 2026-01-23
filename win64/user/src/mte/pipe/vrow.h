#if !defined(MTE_VROW_INT)
#define MTE_VROW_INT

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

typedef struct _vrow_desc{ _align(8)
	u8			base[0x100];
	 _Atomic u8	 	states; // 2 status bits per bank (4 banks)
	 _Atomic u8		lock;
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

axres init_vrow(
	_in vrow_desc		*vrow
);

/*
 	Load payload depending on it`s control bank
*/
volatile bool vrow_load(
	_in vrow_desc		*vrow,
	_in vrow_payload	payload
);

/*
 	Force load payload to indexed bank
*/
volatile bool vrow_bank_load(
	_in vrow_desc		*vrow,
	_in u8			bank_i,
	_in vrow_payload	payload
);

/*
 	Force unload payload from indexed bank
*/
volatile bool vrow_bank_unload(
	_in vrow_desc		*vrow,
	_in u8			bank_i	
);

#endif // !defined(MTE_VROW_INT)

