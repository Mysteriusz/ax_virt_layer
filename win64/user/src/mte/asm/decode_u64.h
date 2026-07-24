#if !defined(MTE_DECODE_U64_INT)
#define MTE_DECODE_U64_INT

#include <ax_type.h>
#include <ax_memory.h>
#include <ax_error.h>

typedef struct _mte_u64_instr{
	u32 			len;
	u64 			*ptr;
	u64 *const 		org;
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

// Returns character 0x80 at byte where (b) character is located
#define U64_HAS_BYTE(b)		(((b) - U64_BIT_LOW_MASK) & ~(b) & U64_BIT_HIGH_MASK) 

/*
 	True if found;
	False if not found;
*/
_inline_force bool _u64_byte_search(
	_in u8					byte,
	_in_out register mte_u64_instr *const 	instr 
){
	__builtin_prefetch(instr->ptr + 1);

	const register u64 mask = U64_BIT_LOW_MASK * byte;
	register u64 *ptr = instr->ptr;
	register u64 res = 0;
	do{
		if (*ptr == 0) return false;

		res = U64_HAS_BYTE(*ptr ^ mask);
		if (res){
			instr->ptr = (u64*)(((c8*)ptr) + (__builtin_ctzll(res) >> 3));
			return true;
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

		res = U64_HAS_BYTE(*ptr ^ mask) ^ U64_BIT_HIGH_MASK;
		if (res){
			instr->ptr = (u64*)(((c8*)ptr) + (__builtin_ctzll(res) >> 3));
			return;
		}
		ptr++;
	} while(1);
}
/*
	Distance between two addressess without 0x20 character.
*/
_inline_force u64 _u64_real_dist(
	_in register const u64 *a,
	_in register const u64 *b
){
	__builtin_prefetch(a + 1);

	const register u64 mask = U64_BIT_LOW_MASK * 0x20;
	register u64 count = 0;
	do{
		if (*a == 0) return 0;

		// Count bits hit by the mask
		count += __builtin_popcountll(
			U64_HAS_BYTE((*a & n_mask(addr_diff(a, b))) ^ mask));
		if (a < b){
			return addr_diff(a, b) - count;
		}
		a++;
	} while(1);
}

_inline_force void _u64_move(
	_in u64					n,
	_in_out register mte_u64_instr *const 	instr 
){
	if (*instr->ptr == 0) return;
	instr->ptr = (u64*)(
		((c8*)instr->ptr) + clamp(
			n, (u64)addr_diff(instr->ptr, instr->org + instr->len)
		);
}

// STRING LENGTH IN QUADWORD (U64)
_inline_force u64 _u64_qlen(
	_in register const u64 val
){
	return 8 - (__builtin_clzll(val) >> 3);
}

// STRING LENGTH
_inline_force u64 _u64_slen(
	_in register const u64 *ptr
){
	const register u64 mask = U64_BIT_LOW_MASK * 0ULL;
	register u64 len = 0;
	register u64 res = 0;
	do{
		if (*ptr == 0) return len;

		res = U64_HAS_BYTE(*ptr ^ mask);
		if (res){
			return len + (__builtin_ctzll(res) >> 3);
		}
		ptr++;
	} while(1);
}

#endif // !defined(MTE_DECODE_U64_INT)
