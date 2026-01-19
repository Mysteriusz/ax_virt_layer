/*
 	Quick tables interface for x86_64 architecture interfaces
*/

#if !defined(INTEL64_QTABLES_INT)
#define INTEL64_QTABLES_INT

#include <ax_type.h>
#include "mte/asm/intel/tables/intel64_modrm.h"
#include "mte/asm/intel/tables/intel64_immd.h"

/*
 	For INTEL64_IMMD_INT interface.
*/

// Act as default array for [struct immd_tables_root] members.
//
// 3 bit legacy byte to immediate table (for _intel64_immd_check)
_unused static u8 L2I_MASK_DEFAULT[256] = {0};
extern u8 *L2I_MASK_LOOKUP[8]; 

// Act as default array for [struct modrm_tables_root] members.
//
// 3 bit legacy byte to modrm table (for _intel64_modrm_check)
_unused static u64 L2M_MASK_DEFAULT[8] = {0};
extern u64 *L2M_MASK_LOOKUP[8]; 

// Mask to immediate length table
extern u8 IMMD_MASK_LEN_TABLE[4];
#define MASK_TO_LEN(m) 		(IMMD_MASK_LEN_TABLE[(m) & 0x3])

// Submask to immediate length table
extern u8 IMMD_SUBMASK_LEN_TABLE[4];
#define SUBMASK_TO_LEN(sm) 	(((sm) & 1) ? 1 : IMMD_SUBMASK_LEN_TABLE[(((sm) >> 1) & 0x3)])

/*
 	For INTEL64_INSTR_INT interface.
*/
extern u8 MOD_DLEN_LOOKUP[4]; // 2 bit modrm.mod field to displacement (for _intel64_disp_len)

/*
 	-----------------------------------IMPORTANT!-----------------------------------
		Load tables with their data before instruction decoding.
*/
void intel64_load_qtables();

#endif // !defined(INTEL64_QTABLES_INT)

