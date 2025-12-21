/*
 	gperf generated hash function for table of _mips32_opcode_map
*/

#include <ax_memory.h>

#include "mte/asm/mips/mips32_opcode_table.h"

#define TOTAL_KEYWORDS 50
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 5
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 124
/* maximum key range = 123, duplicates = 0 */

static u32 hash(
	_in register const c8 	*str,
	_in register u64 	len
){
	static unsigned char asso_values[] ={
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125,   0,  26, 125,
		10,  35,  55,   0,  20,   0,  60, 125,   5,   5,
		60,  45,  10,  25,   0,   0,  35,  10,   5,   0,
		30, 125,   0, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
		125, 125, 125, 125, 125, 125
	};
	register unsigned int hval = len;
	
	switch (hval){
	default:
		hval += asso_values[(unsigned char)str[3]];
	/*FALLTHROUGH*/
	case 3:
		hval += asso_values[(unsigned char)str[2]];
	/*FALLTHROUGH*/
	case 2:
		hval += asso_values[(unsigned char)str[1]];
	/*FALLTHROUGH*/
	case 1:
		hval += asso_values[(unsigned char)str[0]];
		break;
	}
	return hval;
}

mips32_opcode mips32_opcode_lookup(
	_in register const c8 *str,
	_in register u64 len
){
	if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH){
		register unsigned int key = hash(str, len);
		
		if (key <= MAX_HASH_VALUE){
			register const c8 *s = _mips32_opcode_table[key].mnem;
			
			if (*str == *s 
			&& !_sfmemcmp_fast(str + 1, s + 1, _mips32_opcode_table[key].len - 1)){
				return _mips32_opcode_table[key].code;
			}
		}
	}
	return MIPS32_OPCODE_INVALID;
}

