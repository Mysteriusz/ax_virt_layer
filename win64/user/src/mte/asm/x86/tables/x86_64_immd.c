#include "mte/asm/x86/tables/x86_64_immd.h"

_align(64) const struct immd_tables_root immd_tables = (struct immd_tables_root){
.l0_mask = {
    0x0900090009000900ULL, // 0x00-0x1F
    0x0900090009000900ULL, // 0x20-0x3F
    0x0000000000000000ULL, // 0x40-0x5f
    0x0000000001680000ULL, // 0x60-0x7f
    0x0000000000000049ULL, // 0x80-0x9f
    0xFFFF555500090000ULL, // 0xa0-0xbf
    0x0000000004339035ULL, // 0xc0-0xdf
    0x0000F00000005500ULL, // 0xe0-0xff
},
.l0_submask = {
	// From ((v) - b0)
	[0x08 ... 0x0f] = SUBMASK_IMMD_64,
	[0x12] 		= SUBMASK_IMMD_16,
	[0x1a] 		= SUBMASK_IMMD_16,
	[0x46] 		= SUBMASK_IMMD_MOD_0 | SUBMASK_IMMD_8,
	[0x47] 		= SUBMASK_IMMD_MOD_0 | SUBMASK_IMMD_32,
},
};

