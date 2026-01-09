#include "verify.h"
#include <ax_parser.h>

_ntstatus_axres
axres verify_proc(
	struct _KPROCESS	*proc
){
	axres res = AX_SUCC;
	axres_s *res_s = (axres_s*)&res;

	if (proc == nullptr){
		res_s->meta_err = STATUS_INVALID_PARAMETER_1;
		return res;
	}

	/*
	 	Acquire process image name 
		Example:
			\Device\HarddiskVolume3\Users\my_user\Desktop\ax_emulator.exe
	*/
	struct _UNICODE_STRING *proc_name = axmalloc(sizeof(struct _UNICODE_STRING));
	res_s->meta_err = SeLocateProcessImageName(proc, &proc_name);
	if (AX_ERR(res | AX_META_NTSTATUS)){
		axfree(proc_name);
		return res;
	}

	// Open caller file
	io_file *proc_file = nullptr;
	res = io_fo(
		u"\\??\\C:\\update\\test.txt",
		IO_FILE_R,
		IO_FILE_ENC,
		&proc_file);
	axfree(proc_name);

	if (AX_ERR(res | AX_META_NTSTATUS)){
		return res;
	}

	c16 *t = axmalloc(100);
	res = io_fr(proc_file,
		100,
		t,
		nullptr);
	if (AX_ERR(res | AX_META_NTSTATUS)){
		axfree(t);
		io_fc(proc_file);
		return res;
	}
	io_str(t);

	axfree(t);
	io_fc(proc_file);

	return AX_SUCC;
}

