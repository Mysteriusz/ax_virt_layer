#include "core/ax_core.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    NTSTATUS status = STATUS_SUCCESS;
    status = AXDriverInit(DriverObject, RegistryPath);
    if (NT_ERROR(status)) {
        DbgPrint("Virtualization layer initialization failed!\n");
        return status;
    }

    return status;
}
