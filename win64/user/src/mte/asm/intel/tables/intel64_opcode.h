#include <ax_type.h>

/*
   	Reference used:
		- http://ref.x86asm.net/coder64.html
 	
 	How are opcode enums structured?
	As expected first word is the opcode mnemonic itself.
	Then each word describes one operator.

	Structure follows: (ignore [ and ])
		[dest/source prefix][max bit width]

	The only exception for the structure is a register.
	For example if word is AL or AX

	Table of dest/source:
		R -> register
		M -> memory
		IMM -> immidiate

	Table of max bit width:
		Without explicit prefix it`s automatically RM
		8 -> register/memory 8 bits
		16 -> register/memory 16 bits
		32 -> register/memory 8/16/32 bits
		64 -> register/memory 8/16/32/64 bits
*/
enum intel64_opcode{
	ADD_8_R8 	= 0x00, 	// ADD	r/m8		r8
	ADD_64_R64 	= 0x01, 	// ADD	r/m16/32/64	r16/32/64
	ADD_R8_8 	= 0x02, 	// ADD	r8		r/m8
	ADD_R64_64 	= 0x03,		// ADD	r16/32/64	r/m16/32/64
	ADD_AL_IMM8 	= 0x04, 	// ADD	AL		imm8
	ADD_AX_IMM32 	= 0x05, 	// ADD	rAX		imm16/32
};

