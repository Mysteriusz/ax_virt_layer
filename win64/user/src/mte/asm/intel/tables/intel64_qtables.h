/*
 	Quick tables interface for intel64 architecture interfaces
*/

#if !defined(MTE_INTEL64_QTABLES_INT)
#define MTE_INTEL64_QTABLES_INT

#include <ax_type.h>

/*
 	For MTE_INTEL64_IMMD_INT interface.
*/

// Act as default array for [struct immd_tables_root] members.
//
// 3 bit legacy byte to immediate table (for _intel64_immd_check)
_unused static u64 L2I_MASK_DEFAULT[8] = {0};
extern u64 *L2I_MASK_LOOKUP[8]; 

_unused static u8 L2I_SUBMASK_DEFAULT[8] = {0};
extern u8 *L2I_SUBMASK_LOOKUP[8];

// Act as default array for [struct modrm_tables_root] members.
//
// 3 bit legacy byte to modrm table (for _intel64_modrm_check)
_unused static u64 L2M_MASK_DEFAULT[8] = {0};
extern u64 *L2M_MASK_LOOKUP[8]; 

// Mask to immediate length table
extern u8 IMMD_MASK_LEN_TABLE[4];
// Convert mask from [L2I_MASK_LOOKUP] 
// to length of an opcode
#define MASK_TO_LEN(m) 		(IMMD_MASK_LEN_TABLE[(m) & 0x3])

// Submask to immediate length table
extern u8 IMMD_SUBMASK_LEN_TABLE[4];
// Convert submaskmask from [L2I_SUBMASK_LOOKUP]
// to length an opcode
#define SUBMASK_TO_LEN(sm) 	(((sm) & 1) ? 1 : IMMD_SUBMASK_LEN_TABLE[(((sm) >> 1) & 0x3)])

/*
 	For MTE_INTEL64_INSTR_INT interface.
*/
extern u8 LEG_PREF_LOOKUP[0xff]; // (for _intel64_get_legacy)
extern u8 MOD_DLEN_LOOKUP[4]; // 2 bit modrm.mod field to displacement (for _intel64_disp_len)
extern bool LEG_PREF_EXT_LOOKUP[0xff];

/*
 	-----------------------------------IMPORTANT!-----------------------------------
		Load tables with their data before instruction decoding.
*/
_inline_avert void intel64_load_qtables(
	void
);

#endif // !defined(MTE_INTEL64_QTABLES_INT)

