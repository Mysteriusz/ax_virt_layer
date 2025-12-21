/*
 	MIPS32 assembler
*/

#include "mte/core.h"
#include "mte/ir.h"

/*
 	Currently achives speeds around
		- ~1100ns for 1GHZ cpu
		- ~440ns for 2.5GHZ cpu
		- ~250ns for 4.4GHZ cpu
*/
axres mips32_byte_to_raw(
	_in mte_byte_instr			*instr,
	_out mte_raw_instr			*buf
);

/*
	R-type instruction has 4 steps
	Shifts are based on struct mte_raw_instr_mips32
	By default all operations are performed and stored as LITTLE-ENDIAN

	1) funct (Ex: add, addi, xor) shift = 0
	TODO: 2) shamt (Ex:) shift = 6
	2) rd (Ex: $t2, $t7) shift = 11
	3) rs (Ex: $t1, $t3) shift = 21
	4) rt (Ex: $t0, $t3) shift = 16
*/

static u32 _mips32_shift_r[4] = {0, 11, 21, 16}; 
static u32 _mips32_mask_r[4] = {0x3f, 0x1f, 0x1f, 0x1f}; 
u8 mips32_eval_r(
	_in const c8	*restrict 	lb_char,
	_in const c8	*restrict 	rb_char,
	_in u8 				i
);

