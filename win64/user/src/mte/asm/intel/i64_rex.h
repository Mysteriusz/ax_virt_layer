#if !defined(MTE_I64_REX_INT)
#define MTE_I64_REX_INT

#include <ax_type.h>

#include "intel/instr/i64_opcode.h"

_inline_force u8 _i64_rex_resolve(
	_in struct i64_opcode_meta	meta
){
	// Check if opcode has REX byte
	if (__builtin_expect(meta.flags & REX, false)){
		return 0;
	}

	

	return 1;
}

#endif // !defined(MTE_I64_REX_INT)

