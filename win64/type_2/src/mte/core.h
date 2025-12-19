/*
 	Micro Translation Engine core definitions.
*/

#include <ax_error.h>
#include <mte/mips32.h>

/*
 	Update points:
		mte/conv.h/mte_byte_to_raw
*/
enum mte_arch : u32{
	mte_arch_min,
	amd64 = 412 ^ (1UL << 31), // ("amd64" ascii sum) ^ (0x80000000)
	mips32 = 542 ^ (1UL << 31), // ("mips32" ascii sum) ^ (0x80000000)
	mte_arch_max
};

typedef struct _mte_raw_instr{
	enum mte_arch			arch;
	/*
	 	Payload
	*/
	union mte_raw_instr_mips32 	mips32;
} mte_raw_instr;

typedef struct _mte_byte_instr{
	enum mte_arch	arch;
	c8 		*buf;
	u32 		len;
} mte_byte_instr;

typedef struct _mte_context{
	enum mte_arch 		org; // Initial architecture (from)
	enum mte_arch 		exp; // Expected architecture (to)
	mte_raw_instr 		*curr_instr;
	mte_raw_instr 		*first_instr;
} mte_context;

