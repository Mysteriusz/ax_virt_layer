/*
 	Quick tables interface for mips32 architecture interfaces
*/

#if !defined(MTE_MIPS32_QTABLES_INT)
#define MTE_MIPS32_QTABLES_INT

#include <ax_type.h>

#include "mte/ir/ir_op.h"

#include "mips32_op.h"
#include "mips32_reg.h"

/*
 	For MTE_MIPS32_IR_INT interface.
*/

// Maps 8 bit mips32 opcode to IR opcode
extern ir_opcode MIPS32_IR_TABLE[0xff];

void mips32_load_qtables(
	void
);

#endif // !defined(MTE_MIPS32_QTABLES_INT)

