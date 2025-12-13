#include "drv_func.h"

NTSTATUS AxDispatchClose(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
){
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	return STATUS_SUCCESS;
}

