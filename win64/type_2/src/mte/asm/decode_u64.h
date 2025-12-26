#if !defined(MTE_DECODE_U64_INT)
#define MTE_DECODE_U64_INT

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

typedef struct _mte_u64_instr{
	u32 			len;
	u64 			*ptr;
	u64 			*org;
} mte_u64_instr;

static mte_u64_instr _str_to_u64(
	_in const char 	*str,
	_in u64 	len
){
	u32 buf_len = (len / sizeof(u64)) 
		+ ((len % sizeof(u64)) == 0 ? 0 : 1); // If not divisible by sizeof(u64)
	u64 *buf = axmalloc((buf_len + 1) * sizeof(u64));
	memcpy(buf, str, len);

	return (mte_u64_instr){.org = buf, .ptr = buf, .len = buf_len};
}

#define U64_BIT_LOW_MASK 	0x0101010101010101ULL
#define U64_BIT_HIGH_MASK 	0x8080808080808080ULL
#define U64_HAS_BIT(b)		(((b) - U64_BIT_LOW_MASK) & ~(b) & U64_BIT_HIGH_MASK)

_inline_force void _u64_byte_search(
	_in u8					byte,
	_in_out register mte_u64_instr *const 	instr 
){
	__builtin_prefetch(instr->ptr + 1);

	const register u64 mask = U64_BIT_LOW_MASK * byte;
	register u64 *ptr = instr->ptr;
	register u64 res = 0;
	do{
		if (*ptr == 0) return;

		res = U64_HAS_BIT(*ptr ^ mask);
		if (res){
			instr->ptr = (u64*)(((c8*)ptr) + (__builtin_ctzll(res) >> 3));
			return;
		}
		ptr++;
	} while(1);
}
_inline_force void _u64_byte_skip(
	_in u8					byte,
	_in_out register mte_u64_instr *const 	instr 
){
	__builtin_prefetch(instr->ptr + 1);

	const register u64 mask = U64_BIT_LOW_MASK * byte;
	register u64 *ptr = instr->ptr;
	register u64 res = 0;
	do{
		if (*ptr == 0) return;

		res = U64_HAS_BIT(*ptr ^ mask) ^ U64_BIT_HIGH_MASK;
		if (res){
			instr->ptr = (u64*)(((c8*)ptr) + (__builtin_ctzll(res) >> 3));
			return;
		}
		ptr++;
	} while(1);
}
_inline_force u64 _u64_qlen(
	_in register const u64 val
){
	return 8 - (__builtin_clzll(val) >> 3);
}
_inline_force u64 _u64_slen(
	_in register const u64 *ptr
){
	const register u64 mask = U64_BIT_LOW_MASK * 0ULL;
	register u64 len = 0;
	register u64 res = 0;
	do{
		if (*ptr == 0) return len;

		res = U64_HAS_BIT(*ptr ^ mask);
		if (res){
			return len + (__builtin_ctzll(res) >> 3);
		}
		ptr++;
	} while(1);
}

#endif // !defined(MTE_DECODE_U64_INT)
