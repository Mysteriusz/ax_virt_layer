#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/core.h"

#define IR_VER		"MTE_IR_0.01"

typedef struct _ir_raw_instr{
	u32 	opid;
	u8	reg[0xff];
	u8 	reg_count; // Count of required registers
} ir_raw_instr;

typedef struct _ir_context{
	const c8 		ver[14]; // Ex: MTE_IR_0.01\0, MTE_IR_123.45\0
	enum mte_arch 		org;
	struct{
		void (*block)(_in struct _ir_context*);
		void (*unblock)(_in struct _ir_context*);
		axres (*queue)(
			_in struct _ir_context*,
			_in mte_raw_instr*,
			_out ir_raw_instr**);
	} call;
	_Atomic u32 		queue_count;
	_Atomic u32 		thread_count;
	_Atomic bool 		blocked;
} ir_context;

axres ir_create(
	_in enum mte_arch 	org,
	_out ir_context 	**buf
);
axres ir_close(
	_in ir_context 		*ir
);

void ir_block(
	_in struct _ir_context	*ir
);
void ir_unblock(
	_in struct _ir_context	*ir
);

axres ir_queue(
	_in struct _ir_context	*ir,
	_in mte_raw_instr 	*instr,
	_out ir_raw_instr 	**buf
);

