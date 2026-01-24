/*
 	gperf generated hash function for table of _mips32_opcode_map
*/

#include <ax_memory.h>

#include "mte/asm/decode_u64.h"

#include "mips32_op.h"

/*#define TOTAL_KEYWORDS 142
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 621*/
/* maximum key range = 123, duplicates = 0 */

_inline_force static u32 _op_hash(
	_in register u64 	mnem
){
	static u16 asso_values[] ={
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622,   0,   0, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622,   5,   0,   0, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622,   0,  15,  90,
		80,  65,   5,  10, 120,  45, 190,   0,   0,  25,
		134, 200,   5,   0, 115,  10,   0,   0, 185,  50,
		45,  40,  10,   0, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622, 622, 622, 622,
		622, 622, 622, 622, 622, 622, 622
    	};
	register u32 hval = _u64_qlen(mnem);
	
	switch (hval){
	default:
		hval += asso_values[(mnem >> 32) & 0xff];
	/*FALLTHROUGH*/
	case 4:
		hval += asso_values[(mnem >> 24) & 0xff];
	/*FALLTHROUGH*/
	case 3:
		hval += asso_values[((mnem >> 16) & 0xff) + 1];
	/*FALLTHROUGH*/
	case 2:
		hval += asso_values[(mnem >> 8) & 0xff];
	/*FALLTHROUGH*/
	case 1:
		hval += asso_values[mnem & 0xff];
		break;
	}

	return hval;
}

_inline_force static const struct mips32_op_byte_info _mips32_op_lookup(
	_in u64 mnem // Mnemonic 
){
	register u32 key = _op_hash(mnem) + 1;
	if (key < 622){
		if (mnem == _mips32_op_byte_table[key].mnem_u64){
			return _mips32_op_byte_table[key];
		}
	}
	return (struct mips32_op_byte_info){0};
}

