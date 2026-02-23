#if !defined(MTE_I64_REX_INT)
#define MTE_I64_REX_INT

#include <ax_type.h>

#include "mte/cpu.h"

#include "intel/instr/i64_opcode.h"

_inline_force u8 _i64_rex_resolve(
	_in i64_opcode_desc		desc,
	_in_opt cpu_state 		*cpu
){
	// Check if opcode has REX byte
	if (__builtin_expect(desc.flags & REX, false)){
		return 0;
	}
	unref(cpu);

	return 1;
}

#endif // !defined(MTE_I64_REX_INT)

