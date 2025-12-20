#include <ax_type.h>

enum mips32_type{
	R,
	I,
	J,
};
enum mips32_reg{
	R0, // Zero (0)
	R1, // Reserved for asm (at)
	R2, R3, // Results (v0-v1)
	R4, R5, R6, R7, // Arguments (a0-a3)
	R8, R9, R10, R11, R12, R13, R14, R15, // Temporaries (t0-t7)
	R16, R17, R18, R19, R20, R21, R22, R23, // Saved (s0-s7)
	R24, R25, // Not saved (t8-t9)
	R26, R27, // OS reserved (k0-k1)
	R28, // Global pointer (gp)
	R29, // Stack pointer (sp)
	R30, // Frame pointer (fp/s8)
	R31 // Return address (ra)
};

/*	
 	MIPS32 instruction types
*/
typedef u8 mips32_opcode;
#define MIPS32_OPCODE_INVALID (mips32_opcode)0b111111

struct mips32_mte_raw_instr_r{
	mips32_opcode		funct : 6; // Function opcode (main opcode not used for r-type on mips32)
	u32			shamt : 5; // Shift amount
	u32			rd : 5; // Destination register 3 (of)
	u32			rt : 5; // Source register 2 (from)
	u32			rs : 5; // Source register 1 (to)
};
struct mips32_mte_raw_instr_i{
	u32		rs : 5;
	u32		rt : 5;
	u32		immd : 16;
};
struct mips32_mte_raw_instr_j{
	u32		addr : 26;
};
struct mips32_mte_raw_instr{
	mips32_opcode			opcode : 6;
	u32 				data : 26;
};

mips32_opcode mips32_opcode_lookup(
	_in register const c8 *str,
	_in register u64 len
);

struct mips32_opcode_entry{
	const c8 			*mnem;
	const mips32_opcode 		code : 6;
};
static struct mips32_opcode_entry _mips32_opcode_table[] = {
      {(c8*)""}, {(c8*)""},

      {(c8*)"sw",	0b101011},
      {(c8*)"sra",	0b000011},

      {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"lw",	0b100011},
      {(c8*)"srl",	0b000010},
      {(c8*)"srav",	0b000111},

      {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"sll",	0b000000},
      {(c8*)"srlv",	0b000110},

      {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"div",	0b011010},
      {(c8*)"sllv",	0b000100},

      {(c8*)""}, {(c8*)""},

      {(c8*)"sh",	0b101001},
      {(c8*)"add",	0b100000},
      {(c8*)"addi",	0b001000},
      {(c8*)"addiu",	0b001001},

      {(c8*)""},

      {(c8*)"lh",	0b100001},
      {(c8*)"lhi",	0b011001},
      {(c8*)"divu",	0b011011},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"addu",	0b100001},
      
      {(c8*)""}, {(c8*)""},

      {(c8*)"se",	0b101000},
      {(c8*)"lhu",	0b100101},
      {(c8*)"sub",	0b100010},
      
      {(c8*)""}, {(c8*)""},

      {(c8*)"le",	0b100000},
      {(c8*)"slt",	0b101010},
      {(c8*)"slti",	0b001010},
      {(c8*)"sltiu",	0b001001},

      {(c8*)""},

      {(c8*)"or",	0b100101},
      {(c8*)"ori",	0b001101},
      {(c8*)"trap",	0b011010},
      {(c8*)"subu",	0b100011},

      {(c8*)""}, {(c8*)""},

      {(c8*)"leu",	0b100100},
      {(c8*)"sltu",	0b101001},

      {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"llo",	0b011000},
      {(c8*)"mult",	0b011000},
      {(c8*)"multu",	0b011001},
      {(c8*)"j",	0b000010},
      {(c8*)"jr",	0b001000},

      {(c8*)""},

      {(c8*)"mthi",	0b010001},
      {(c8*)"bgtz",	0b000111},

      {(c8*)""}, {(c8*)""},

      {(c8*)"jal",	0b000011},
      {(c8*)"jalr",	0b001001},
      {(c8*)"blez",	0b000110},

      {(c8*)""}, {(c8*)""},

      {(c8*)"and",	0b100100},
      {(c8*)"andi",	0b001100},

      {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"xor",	0b100110},
      {(c8*)"xori",	0b001110},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"mfhi",	0b010000},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"beq",	0b000100},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"mtlo",	0b010011},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},
      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"nor",	0b100111},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"mflo",	0b010010},

      {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""}, {(c8*)""},

      {(c8*)"bne",	0b000101}
};

