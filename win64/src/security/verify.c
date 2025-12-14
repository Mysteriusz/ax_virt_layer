#include "verify.h"

_ntstatus_axres
axres verify_proc(
	struct _KPROCESS	*proc
){
	axres res = AX_SUCC;
	axres_s *res_s = (axres_s*)&res;
	res_s->meta.ntstatus = true;

	if (proc == nullptr){
		res_s->meta_err = STATUS_INVALID_PARAMETER_1;
		return res;
	}

	struct _UNICODE_STRING *proc_name = axmalloc(sizeof(struct _UNICODE_STRING));
	res_s->meta_err = SeLocateProcessImageName(proc, &proc_name);
	if (AX_ERR(res)){
		return res;
	}

	return res;
}

