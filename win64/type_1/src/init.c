#include "drv_func.h"

DRIVER_UNLOAD DriverUnload;
DRIVER_INITIALIZE DriverEntry;

struct _DEVICE_OBJECT *ioctl_device = nullptr;

struct _UNICODE_STRING ioctl_device_name = {0};
struct _UNICODE_STRING ioctl_dos_name = {0};

NTSTATUS DriverEntry(
  	struct _DRIVER_OBJECT  	*driver_object,
  	struct _UNICODE_STRING 	*registry_path
){
	if (1){
		return STATUS_NOT_IMPLEMENTED;
	}

	NTSTATUS status = STATUS_SUCCESS;

	DbgPrint("AX_VIRT_LAYER STARTING\n");

	RtlInitUnicodeString(&ioctl_device_name, u"\\Device\\AX_VIRT_LAYER");
	RtlInitUnicodeString(&ioctl_dos_name, u"\\DosDevices\\AX_VIRT_LAYER");

	status = IoCreateDevice(
		driver_object,
		0,
		&ioctl_device_name,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		false,
		&ioctl_device);
	if (!NT_SUCCESS(status)){
		DbgPrint("AX_VIRT_LAYER Device fail!\n");
		DbgPrint("%lu\n", status);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	status = IoCreateSymbolicLink(&ioctl_dos_name, &ioctl_device_name);
	if (!NT_SUCCESS(status)){
		DbgPrint("AX_VIRT_LAYER Symbolic link fail!\n");
		DbgPrint("%lu\n", status);
		IoDeleteDevice(ioctl_device);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrint("AX_VIRT_LAYER Device created!\n");

	driver_object->DriverUnload = DriverUnload;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = AxDispatchDeviceControl;
	driver_object->MajorFunction[IRP_MJ_CREATE] = AxDispatchCreate;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = AxDispatchClose;

	return STATUS_SUCCESS;
}

_Use_decl_annotations_
VOID DriverUnload(
	struct _DRIVER_OBJECT	*DriverObject
){
	IoDeleteDevice(ioctl_device);
	IoDeleteSymbolicLink(&ioctl_dos_name);
	DbgPrint("AX_VIRT_LAYER UNLOAD\n");
}

