/*
 	Micro Translation Engine core definitions.
*/

#if !defined(MTE_CORE_INT)
#define MTE_CORE_INT

#include <ax_error.h>

#include "mte/asm/decode_u64.h"

#include "arch.h"

typedef struct _mte_raw_instr{
	enum mte_arch	arch;
	/*
	 	Payload
	*/
	u8 		payload[60];
} mte_raw_instr;

#endif // !defined(MTE_CORE_INT)

