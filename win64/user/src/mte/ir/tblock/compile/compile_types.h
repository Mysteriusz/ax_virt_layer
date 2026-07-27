#if !defined(MTE_COMPILE_STATE_INT)
#define MTE_COMPILE_STATE_INT

#include <ax_type.h>

typedef u16 comp_reg_liveness;

typedef struct _comp_reg_assoc{
	bool 	used;
	bool 	spill;
	u8	id; // Host architecture-specific register/spill id/index
} comp_reg_assoc;

typedef enum _comp_reg_state : u8{
	REG_FREE = 0,
	REG_OCCUPIED = 1,
} comp_reg_state;


#endif // !defined(MTE_COMPILE_STATE_INT)

