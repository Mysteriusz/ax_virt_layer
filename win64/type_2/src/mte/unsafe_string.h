#if !defined(MTE_UNSAFE_STRING_INT)
#define MTE_UNSAFE_STRING_INT

#include <ax_type.h>

_inline_force static const c8 *_unsafe_skip_until(
	_in const c8 	*text,
	_in c8		byte
){
	while(*text != byte && *text != '\0'){
		text++;
	}
	return text;
}
_inline_force static const c8 *_unsafe_skip_until_not(
	_in const c8 	*text,
	_in c8		byte
){
	while(*text == byte && *text != '\0'){
		text++;
	}
	return text;
}

#endif // !defined(MTE_UNSAFE_STRING_INT)

