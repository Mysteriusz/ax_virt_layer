#include <ax_memory.h>
#include "mte/arch/mips/mips32.h"

#define TOTAL_KEYWORDS 65
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 5
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 228
/* maximum key range = 227, duplicates = 0 */

static u32 hash(
	_in register const c8 	*str,
	_in register u64 	len
){
	static unsigned char asso_values[] ={
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
	register unsigned int hval = len;
	
	switch (hval){
	default:
		hval += asso_values[str[2]+4];
	/*FALLTHROUGH*/
	case 2:
		hval += asso_values[str[1]+1];
		break;
	}
	return hval;
}

enum mips32_reg mips32_reg_lookup(
	_in register const c8 	*str,
	_in register u64 	len
){
	if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH){
		register unsigned int key = hash (str, len);
		
		if (key <= MAX_HASH_VALUE){
			register const c8 *s = _mips32_reg_table[key].name;
		
			if (*str == *s 
			&& !_sfmemcmp_fast(str + 1, s + 1, _mips32_reg_table[key].len - 1)){
				return _mips32_reg_table[key].value;
			}
		}
	}
	return 0;
}

