#include "intel64_qtables.h"

/*
 	For INTEL64_INSTR interface.
*/
u8 MOD_DLEN_LOOKUP[4] = {[0 ... 3] = 0};

/*
 	For INTEL64_IMMD_INT interface.
*/

u8 *L2I_MASK_LOOKUP[8] = {[0 ... 7] = L2I_MASK_DEFAULT};
u64 *L2M_MASK_LOOKUP[8] = {[0 ... 7] = L2M_MASK_DEFAULT};
u8 IMMD_MASK_LEN_TABLE[4] = {0};
u8 IMMD_SUBMASK_LEN_TABLE[4] = {0};

/*
 	Quick table loaders
*/

void intel64_load_qtables(){
	L2I_MASK_LOOKUP[0b111] = (u8*)immd_tables->l2_0f_mask; // 0fH & 7H = 111B
	L2I_MASK_LOOKUP[0b110] = (u8*)immd_tables->l2_66_mask; // 66H & 7H = 110B
	L2I_MASK_LOOKUP[0b001] = (u8*)immd_tables->l2_0f_mask; // f3H & 7H = 001B
	L2I_MASK_LOOKUP[0b010] = (u8*)immd_tables->l2_0f_mask; // f2H & 7H = 010B

	L2M_MASK_LOOKUP[0b111] = (u64*)modrm_tables->l2_0f_mask; // 0fH & 7H = 111B
	L2M_MASK_LOOKUP[0b110] = (u64*)modrm_tables->l2_66_mask; // 66H & 7H = 110B
	L2M_MASK_LOOKUP[0b001] = (u64*)modrm_tables->l2_0f_mask; // f3H & 7H = 001B
	L2M_MASK_LOOKUP[0b010] = (u64*)modrm_tables->l2_0f_mask; // f2H & 7H = 010B

	MOD_DLEN_LOOKUP[0b00] = 4;
	MOD_DLEN_LOOKUP[0b01] = 1;
	MOD_DLEN_LOOKUP[0b10] = 4;
	MOD_DLEN_LOOKUP[0b11] = 0;

	IMMD_MASK_LEN_TABLE[0b00] = 0; // No immediate
	IMMD_MASK_LEN_TABLE[0b01] = 1;
	IMMD_MASK_LEN_TABLE[0b10] = 4;
	IMMD_MASK_LEN_TABLE[0b11] = 0; // Submask should be accessed
	IMMD_SUBMASK_LEN_TABLE[0b00] = 0; // No immediate
	IMMD_SUBMASK_LEN_TABLE[0b01] = 2;
	IMMD_SUBMASK_LEN_TABLE[0b10] = 4;
	IMMD_SUBMASK_LEN_TABLE[0b11] = 8; 
}

