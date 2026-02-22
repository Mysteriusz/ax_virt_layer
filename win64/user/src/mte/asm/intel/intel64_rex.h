#if !defined(MTE_INTEL64_REX_INT)
#define MTE_INTEL64_REX_INT

#include <ax_type.h>

#include "intel/instr/intel64_opcode.h"

_inline_force u8 _intel64_rex_resolve(
	_in struct intel64_opcode_meta	meta
){
	// Check if opcode has REX byte
	if (__builtin_expect(meta.flags & REX, false)){
		return 0;
	}

	

	return 1;
}

#endif // !defined(MTE_INTEL64_REX_INT)

