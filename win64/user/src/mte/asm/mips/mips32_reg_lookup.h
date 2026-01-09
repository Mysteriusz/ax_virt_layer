/*
 	gperf generated hash function for table of _mips32_reg_map
*/

#include <ax_memory.h>

#include "mte/asm/mips/mips32_reg_table.h"

/*#define TOTAL_KEYWORDS 65
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 5
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 228*/
/* maximum key range = 227, duplicates = 0 */

_inline_force static u32 _reg_hash(
	_in register u64 	mnem
){
	static u8 asso_values[] ={
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229,  10,
		5,   0,  25,  55, 105,  85,  22,   2, 125,  42,
		0,  62, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 120, 229,
		229,  80, 229,  77,  72,  35, 229, 229, 125, 229,
		229, 229, 229, 229, 229,  80,  10,  15,  20, 229,
		35, 229, 229,  35, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
		229, 229, 229, 229, 229, 229, 229, 229, 229, 229
	};
	register u32 hval = _u64_qlen(mnem);
	
	switch (hval){
	default:
		hval += asso_values[((mnem >> 16) & 0xff) + 4];
	/*FALLTHROUGH*/
	case 2:
		hval += asso_values[((mnem >> 8) & 0xff) + 1];
		break;
	}
	return hval;
}

_inline_force static const struct mips32_reg_info _mips32_reg_lookup(
	_in u64 val // Register syntax
){
	register u32 key = _reg_hash(val);
	if (key <= 228){
		if (val == _mips32_reg_table[key].name_u64){
			return _mips32_reg_table[key];
		}
	}
	return (struct mips32_reg_info){0};
}

