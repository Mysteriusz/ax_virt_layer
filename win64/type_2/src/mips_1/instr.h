#include "reg.h"

/*
 	IMPORTANT!

	All instructions are to be strictly zero initialized!
*/

typedef struct _instr_r{
	u32 opcode : 6;
	u32 rs : 5;
	u32 rt : 5;
	u32 rd : 5;
	u32 shamt : 5;
	u32 funct : 6;
} instr_r;

typedef struct _instr_i{
	u32 opcode : 6;
	u32 rs : 5;
	u32 rt : 5;
	u32 immd : 16;
} instr_i;

typedef struct _instr_j{
	u32 opcode : 6;
	u32 addr : 26;
} instr_j;

