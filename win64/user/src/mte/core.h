/*
 	Micro Translation Engine core definitions.
*/

#if !defined(MTE_CORE_INT)
#define MTE_CORE_INT

#include <ax_error.h>

#include "mips/mips32.h"
#include "intel/intel64.h"

#include "mte/asm/decode_u64.h"

#define enum_bound(e, t) \
	((e) <= t##_min || (e) >= t##_max)

enum mte_arch : u32{
	mte_arch_min,
	amd64 = 412 ^ (1UL << 31), // ("amd64" ascii sum) ^ (0x80000000)
	mips32 = 542 ^ (1UL << 31), // ("mips32" ascii sum) ^ (0x80000000)
	mte_arch_max
};
enum mte_syn : u32{
	mte_syn_min,
	intel = 1,
	atnt = 2,
	mte_syn_max
};

typedef struct _mte_raw_instr{
	enum mte_arch			arch;
	/*
	 	Payload
	*/
	union {
		mips32_mte_raw_instr 	mips32;
		intel64_mte_raw_instr 	intel64;
	};
} mte_raw_instr;

typedef struct _mte_byte_instr{
	enum mte_syn		syn;
	enum mte_arch		arch;
	mte_u64_instr 		val;
} mte_byte_instr;

_inline_force static bool mte_byte_instr_inv(
	_in mte_byte_instr 	*instr
){
	if (instr == nullptr
	|| instr->val.org == nullptr
	|| instr->val.ptr == nullptr
	|| enum_bound(instr->syn, mte_syn)
	|| enum_bound(instr->arch, mte_arch)){
		return true;
	}
	return false;
}

typedef struct _mte_context{
	enum mte_arch 		org; // Initial architecture (from)
	enum mte_arch 		exp; // Expected architecture (to)
	mte_raw_instr 		*curr_instr;
	mte_raw_instr 		*first_instr;
} mte_context;

#endif // !defined(MTE_CORE_INT)

