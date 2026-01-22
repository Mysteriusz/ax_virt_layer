/*
 	Micro Translation Engine core definitions.
*/

#if !defined(MTE_CORE_INT)
#define MTE_CORE_INT

#include <ax_error.h>

#include "mips/mips32.h"
#include "intel/intel64.h"
#include "mte/asm/decode_u64.h"

#include "arch.h"

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
	|| instr->val.ptr == nullptr){
		return true;
	}
	return false;
}

#endif // !defined(MTE_CORE_INT)

