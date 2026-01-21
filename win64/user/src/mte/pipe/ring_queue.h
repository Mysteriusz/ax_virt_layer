#if !defined(MTE_RING_QUEUE_INT)
#define MTE_RING_QUEUE_INT

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

 _align(64)
typedef struct _rqueue_desc{
	u8		base[0xff];
	_Atomic bool 	r_states[4]; // 4 regions
} rqueue_desc;

typedef struct _rqueue_payload{
	u8	control[0x10]; // 16 bytes control block
	u8	data[0x30]; // 48 bytes payload
} rqueue_payload;

#define RQUEUE_REGION_SIZE 0x40 // 64 bytes for 256 byte region (4 regions per queue)

axres init_rqueue(
	_in rqueue_desc		*rqueue
);

volatile bool rqueue_region_push(
	_in rqueue_desc		*rqueue,
	_in u8			region_i,
	_in rqueue_payload	payload
);

#endif // !defined(MTE_RING_QUEUE_INT)

