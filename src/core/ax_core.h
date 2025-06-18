#pragma once

#include <ntddk.h>
#include <wdf.h>

typedef NTSTATUS AXSTATUS;

const GUID AXGUID;
DEFINE_GUID(AXGUID,
    0x4681fed4, 0x9757, 0x4ea8, 0xaa, 0x1b, 0xf6, 0x1d, 0x25, 0x26, 0x69, 0x43);

NTSTATUS DriverAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit);
