#include "drv_func.h"

NTSTATUS AxDispatchCreate(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
){
	NT_ASSERT(device_object != nullptr);
	NT_ASSERT(irp != nullptr);

	axres res = AX_SUCC;
	axres_s *res_s = (axres_s*)&res;
	res_s->meta.ntstatus = true;

	res = verify_proc(IoGetRequestorProcess(irp));
	irp->IoStatus.Status = res_s->meta_err;

	if (AX_ERR(res)){
		return res_s->meta_err;
	}

	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	return res_s->meta_err;
}

