#include <ax_type.h>

enum mips32_funct{
	MIPS32_FUNCT_ADD = (1 << 6), // 100000
	MIPS32_FUNCT_SUB = (1 << 6) | (1 << 2), // 100010
	MIPS32_FUNCT_AND = (1 << 6) | (1 << 3), // 100100
	MIPS32_FUNCT_OR = (1 << 6) | (1 << 3) | (1 << 1), // 100101
	MIPS32_FUNCT_SLT = (1 << 6) | (1 << 4) | (1 << 2), // 101010
};

/*	
 	MIPS32 instruction types
*/

union mte_raw_instr_mips32_r{
	u32			rs : 5; // Source register 1 (to)
	u32			rt : 5; // Source register 2 (from)
	u32			rd : 5; // Destination register 3 (of)
	u32			shamt : 5; // Shift amount
	enum mips32_funct	funct : 6; // Function identifier
};
union mte_raw_instr_mips32_i{
	u32		rs : 5;
	u32		rt : 5;
	u32		immd : 16;
};
union mte_raw_instr_mips32_j{
	u32		addr;
};
union mte_raw_instr_mips32{
	u32				opcode : 6;
	union mte_raw_instr_mips32_r 	r;
	union mte_raw_instr_mips32_i 	i;
	union mte_raw_instr_mips32_j 	j;
};

