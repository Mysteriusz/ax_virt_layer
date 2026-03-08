#if !defined(MTE_IR_REG_INT)
#define MTE_IR_REG_INT

#include <ax_type.h>

enum ir_reg_role : u8{
	IR_REG_RETURN 		= 0x01, // Ex: rax
	IR_REG_TEMP 		= 0x02, // Ex: rcx
	IR_REG_LONG_TEMP 	= 0x04, // Ex: r11
	IR_REG_STACK		= 0x08, // Ex: rsi
};

#endif // !defined(MTE_IR_REG_INT)

