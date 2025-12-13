#include "drv_func.h"

NTSTATUS AxDispatchDeviceControl(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
){
	if (device_object == nullptr){
		return STATUS_INVALID_PARAMETER_1;
	}
	if (irp == nullptr){
		return STATUS_INVALID_PARAMETER_2;
	}

	IO_STACK_LOCATION *irp_stack = IoGetCurrentIrpStackLocation(irp);
	ULONG ctl_code = irp_stack->Parameters.DeviceIoControl.IoControlCode;

	DbgPrint("IOCTL Code received\n");
	DbgPrint("%lu\n", ctl_code);

	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	return STATUS_SUCCESS;
}

