#if !defined(MTE_COMPILE_STATE_INT)
#define MTE_COMPILE_STATE_INT

#include <ax_type.h>

typedef u8 asm_reg_liveness;

typedef struct _asm_reg_assoc{
	/*
		Bit format follows:
			- 000000US
			(U) -> Used;
			(S) -> Spilled;
	*/
	u8 	flags;
	u16	id; // Host architecture-specific register/spill id/index
} asm_reg_assoc;

inline static bool _ir_reg_assoc_used(asm_reg_assoc assoc){
	return !!(assoc.flags & BIT(1));
}
inline static bool _ir_reg_assoc_spilled(asm_reg_assoc assoc){
	return !!(assoc.flags & BIT(0));
}
inline static u8 _ir_reg_assoc_flags(bool used, bool spill){
	return (used << 1) | (spill); 
}

typedef enum _asm_reg_state : u8{
	REG_FREE = 0, // Register is unused
	REG_OCCUPIED = 1,
} asm_reg_state;

#endif // !defined(MTE_COMPILE_STATE_INT)

