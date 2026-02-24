#include "cpu_access.h"

struct cpu_reg_id cpu_access_width(
	_in cpu_state 	*state,
	_in u8		width
){
	struct cpu_reg_id buf = {0};
	for (u8 i = 0; i < state->gprs.reg_count; i++){
		if (state->gprs.root[i].width == width){
			buf = state->gprs.root[i];
			break;
		}
	}
	return (struct cpu_reg_id){0};
}

