#pragma once

#include <ntddk.h>
#include <initguid.h>

typedef NTSTATUS AXSTATUS;

// {E8563CF8-FAC3-46E4-A540-0ADE8B69580F}
DEFINE_GUID(AXGUID,
    0xe8563cf8, 0xfac3, 0x46e4, 0xa5, 0x40, 0xa, 0xde, 0x8b, 0x69, 0x58, 0xf);

#define AXNAME_K L"AX_VIRTUALIZATION_DRIVER"
#define AXNAME_U L"AX VIRTUALIZATION LAYER DRIVER"

#define AXPATH_K L"\\Device\\AX_VIRTUALIZATION_DEVICE"
#define AXPATH_U L"\\DosDevices\\AX_VIRTUALIZATION_DEVICE"

AXSTATUS AXDriverInit(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
AXSTATUS AXDriverUnload(PDRIVER_OBJECT DriverObject);

AXSTATUS AXEmulator_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
AXSTATUS AXEmulator_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
AXSTATUS AXEmulator_Control(PDEVICE_OBJECT DeviceObject, PIRP Irp);
