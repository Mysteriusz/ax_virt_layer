#if !defined(MTE_COMPILE_STATE_INT)
#define MTE_COMPILE_STATE_INT

#include <ax_type.h>

typedef u16 asm_reg_liveness;

typedef struct _asm_reg_assoc{
	bool 	used;
	bool 	spill;
	u8	id; // Host architecture-specific register/spill id/index
} asm_reg_assoc;

typedef enum _asm_reg_state : u8{
	REG_FREE = 0,
	REG_OCCUPIED = 1,
} asm_reg_state;


#endif // !defined(MTE_COMPILE_STATE_INT)

