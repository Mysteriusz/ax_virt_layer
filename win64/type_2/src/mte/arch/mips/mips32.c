#include "mips32.h"

mips32_opcode mips32_opcode_lookup(
	_in const c8	*mnem,
	_in u8		len
){
	u32 k = (len * 5 + mnem[0]) 
		% (sizeof(_mips32_opcode_map) / sizeof(struct mips32_opcode_entry));
	if (memcmp(mnem, _mips32_opcode_map[k].mnem, len) == 0){
		return _mips32_opcode_map[k].code;
	}
	return MIPS32_OPCODE_INVALID;
}

