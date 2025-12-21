#if !defined(MTE_MIPS32_OPCODE_TABLE_INT)
#define MTE_MIPS32_OPCODE_TABLE_INT

#include "mte/asm/mips/mips32.h"

/*
	MIPS32-specific lookup tables provided by GNU gpref
*/
struct mips32_opcode_entry{
	const char 			*mnem;
	const u8 			len;
	const mips32_opcode 		code : 6;
};
static struct mips32_opcode_entry _mips32_opcode_table[] = {
	{""}, {""},

	{"sw",   	2, 0b101011},
	{"sra",   	3, 0b000011},

	{""}, {""}, {""},

	{"lw",   	2, 0b100011},
	{"srl",   	3, 0b000010},
	{"srav",   	4, 0b000111},

	{""}, {""}, {""},

	{"sll",   	3, 0b000000},
	{"srlv",   	4, 0b000110},

	{""}, {""}, {""},

	{"div",   	3, 0b011010},
	{"sllv",   	4, 0b000100},

	{""}, {""},

	{"sh",   	2, 0b101001},
	{"add",   	3, 0b100000},
	{"addi",   	4, 0b001000},
	{"addiu",   	5, 0b001001},

	{""},

	{"lh",   	2, 0b100001},
	{"lhi",   	3, 0b011001},
	{"divu",   	4, 0b011011},

	{""}, {""}, {""}, {""},

	{"addu",   	4, 0b100001},
      
	{""}, {""},

	{"se",   	2, 0b101000},
	{"lhu",   	3, 0b100101},
	{"sub",   	3, 0b100010},
      
	{""}, {""},

	{"le",   	2, 0b100000},
	{"slt",   	3, 0b101010},
	{"slti",   	4, 0b001010},
	{"sltiu",   	5, 0b001001},

	{""},

	{"or",   	2, 0b100101},
	{"ori",   	3, 0b001101},
	{"trap",   	4, 0b011010},
	{"subu",   	4, 0b100011},

	{""}, {""},

	{"leu",   	3, 0b100100},
	{"sltu",   	4, 0b101001},

	{""}, {""}, {""},

	{"llo",   	3, 0b011000},
	{"mult",   	4, 0b011000},
	{"multu",   	5, 0b011001},
	{"j",   	1, 0b000010},
	{"jr",   	2, 0b001000},

	{""},

	{"mthi",   	4, 0b010001},
	{"bgtz",   	4, 0b000111},

	{""}, {""},

	{"jal",   	3, 0b000011},
	{"jalr",   	4, 0b001001},
	{"blez",   	4, 0b000110},

	{""}, {""},

	{"and",   	3, 0b100100},
	{"andi",   	4, 0b001100},

	{""}, {""}, {""},

	{"xor",   	3, 0b100110},
	{"xori",   	4, 0b001110},

	{""}, {""}, {""}, {""},

	{"mfhi",   	4, 0b010000},

	{""}, {""}, {""}, {""},

	{"beq",   	3, 0b000100},

	{""}, {""}, {""}, {""},

	{"mtlo",   	4, 0b010011},

	{""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
	{""}, {""}, {""}, {""},

	{"nor",   	3, 0b100111},

	{""}, {""}, {""}, {""}, {""},

	{"mflo",   	4, 0b010010},

	{""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},

	{"bne",   	3, 0b000101}
};

struct mips32_opcode_funct_entry{
	const char		*mnem;
	const ir_opcode		ir_code;
};
// 63 entries (maximum value of funct field 0b111111)
static struct mips32_opcode_funct_entry _mips32_opcode_funct_table[63] = {
	[0b001000] = {"addi", ADD_I32_IMMD}, 
};

#endif // !defined(MTE_MIPS32_OPCODE_TABLE_INT)

