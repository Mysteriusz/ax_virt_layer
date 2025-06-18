#include "core/ax_core.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    NTSTATUS status = DriverInit(DriverObject, RegistryPath);
    return status;
}
