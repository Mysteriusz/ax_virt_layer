#include "drv_func.h"

NTSTATUS AxDispatchClose(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
){
	NT_ASSERT(device_object != nullptr);
	NT_ASSERT(irp != nullptr);

	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	return STATUS_SUCCESS;
}

