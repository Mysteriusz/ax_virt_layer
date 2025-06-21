#pragma once

#include "ax_def.h"
#include "machine/machine.h"

#define AXNAME_K L"AX_VIRTUALIZATION_DRIVER"
#define AXNAME_U L"AX VIRTUALIZATION LAYER DRIVER"

#define AXPATH_K L"\\Device\\AX_VIRTUALIZATION_DEVICE"
#define AXPATH_U L"\\DosDevices\\AX_VIRTUALIZATION_DEVICE"

AXSTATUS AXDriverInit(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID AXDriverUnload(PDRIVER_OBJECT DriverObject);

AXSTATUS AXEmulator_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
AXSTATUS AXEmulator_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
AXSTATUS AXEmulator_Control(PDEVICE_OBJECT DeviceObject, PIRP Irp);
