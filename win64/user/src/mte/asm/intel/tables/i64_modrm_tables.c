#include "intel/emitter/i64_modrm.h"

_inline_avert void i64_prefetch_modrm(
	void
){
	_mm_prefetch(&MODRM_TABLES, _MM_HINT_T0);
	_mm_prefetch(offp(&MODRM_TABLES, 64), _MM_HINT_T0);
	_mm_prefetch(offp(&MODRM_TABLES, 128), _MM_HINT_T0);
};

const struct modrm_tables_root MODRM_TABLES = (struct modrm_tables_root){
	.l0_mask = {
	    0x0f0f0f0f0f0f0f0fULL, // 0x00-0x3F
	    0x0000000000000a0cULL, // 0x40-0x7F
	    0x000000000000ffffULL, // 0x80-0xBF
	    0xc3c30f0f0000f3cfULL  // 0xC0-0xFF
	},
	.l1_mask = {
	    0xff80fe0001ff000fULL, // 0x00-0x3F
	    0xc0000000fffffbffULL, // 0x40-0x7F (bits 64-127)
	    0xfffffffffffffe38ULL, // 0x80-0xBF
	    0x7fffefbe7fbf003eULL  // 0xC0-0xFF
	},
	
	.l2_0f_mask = {
	    0x0000000000008000ULL, // 0x00-0x3F
	    0x0000000000000000ULL, // 0x40-0x7F
	    0x000000000000031eULL, // 0x80-0xBF
	    0x0303000000000003ULL  // 0xC0-0xFF
	},
	
	.l2_66_mask = {
	    0xff00000000fffe00ULL, // 0x00-0x3F
	    0xf07fffff0003ffffULL, // 0x40-0x7F
	    0x0000000000000000ULL, // 0x80-0xBF
	    0x7ffffffffffffdf4ULL  // 0xC0-0xFF
	},
	
	.l2_f2_mask = {
	    0x0000340000000700ULL, // 0x00-0x3F
	    0xf0010000f7020000ULL, // 0x40-0x7F
	    0x0000000000000000ULL, // 0x80-0xBF
	    0x0001004000000004ULL  // 0xC0-0xFF
	},
	.l2_f3_mask = {
	    0x0000340000400700ULL, // 0x00-0x3F
	    0xc0800000ff0e0000ULL, // 0x40-0x7F
	    0x0100000000000000ULL, // 0x80-0xBF
	    0x0000004000800084ULL  // 0xC0-0xFF
	},

};

