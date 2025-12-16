#include <ax_utility.h>
#include "verify.h"

/*
 	Base PDRIVER_DISPATCH definition.
*/
NTSTATUS AxDispatchDeviceControl(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
);

NTSTATUS AxDispatchCreate(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
);
NTSTATUS AxDispatchClose(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
);

