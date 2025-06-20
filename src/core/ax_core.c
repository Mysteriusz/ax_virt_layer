#include "ax_core.h"

static UNICODE_STRING AXPATH_K_STRING = RTL_CONSTANT_STRING(AXPATH_K);
static UNICODE_STRING AXPATH_U_STRING = RTL_CONSTANT_STRING(AXPATH_U);
static UNICODE_STRING AXNAME_K_STRING = RTL_CONSTANT_STRING(AXNAME_K);
static UNICODE_STRING AXNAME_U_STRING = RTL_CONSTANT_STRING(AXNAME_U);

AXSTATUS AXDriverInit(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath){
    DbgPrint("Virtualization layer initializing!\n");
    
    NTSTATUS status = STATUS_SUCCESS;
    DriverObject->DriverUnload = AXDriverUnload;
    
    PDEVICE_OBJECT device = NULL;
    status = IoCreateDevice(DriverObject, 0, &AXPATH_K_STRING, FILE_DEVICE_UNKNOWN, 0, FALSE, &device);
    if (NT_ERROR(status) || device == NULL) {
        DbgPrint("AX_ERR: 0x%08X\n", status);
        return status;
    }
    
    status = IoCreateSymbolicLink(&AXPATH_U_STRING, &AXPATH_K_STRING);
    if (NT_ERROR(status)) {
        DbgPrint("AX_ERR: 0x%08X\n", status);
        return status;
    }

    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->MajorFunction[IRP_MJ_CLOSE] = AXEmulator_Close;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = AXEmulator_Create;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = AXEmulator_Control;

    return status;
}

AXSTATUS AXDriverUnload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    return STATUS_SUCCESS;
}

AXSTATUS AXEmulator_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    UNREFERENCED_PARAMETER(DeviceObject);
    return STATUS_SUCCESS;
}

AXSTATUS AXEmulator_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    UNREFERENCED_PARAMETER(DeviceObject);
    return STATUS_SUCCESS;
}

AXSTATUS AXEmulator_Control(PDEVICE_OBJECT DeviceObject, PIRP Irp){
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
    UNREFERENCED_PARAMETER(DeviceObject);

    switch (code) {
    case AX_IOCC_MACHINE:
        DbgPrint("IOCTL VMX received\n");
        AXCreateMachine(NULL, NULL);
        break;
    case AX_IOCC_DEBUG:
        DbgPrint("IOCTL DEBUG received\n");
        DbgBreakPoint();
        break;
    default:
        DbgPrint("IOCTL UNK received\n");
        DbgBreakPoint();
        break;
    }

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Irp->IoStatus.Status;
}
