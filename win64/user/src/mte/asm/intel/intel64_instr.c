#include "mte/asm/intel/intel64_instr.h"

/*
 	Quick tables
*/

// 3 bit legacy byte to immidiate table 
u8 *L2I_MASK_LOOKUP[8] = {0};
u64 *L2M_MASK_LOOKUP[8] = {0};

