#ifndef AX_DRIVER_ENV
#define AX_DRIVER_ENV

#include <ntddk.h>
#include <initguid.h>

/*
*
*
* ------------------- AX TYPE DEFINITIONS -------------------
*
*
*/

// ========================================================
//					  AX Command status
// ========================================================
typedef NTSTATUS AXSTATUS;

// ========================================================
//                 AX Machine memory root
// ========================================================
typedef HANDLE AXMACHINE, *PAXMACHINE;
typedef struct _AXMACHINE_ROOT {
	UINT32 version;
} *PAXMACHINE_ROOT, AXMACHINE_ROOT;

typedef struct _AXMACHINE_CONFIG {
	UINT32 cpuCount;
} *PAXMACHINE_CONFIG, AXMACHINE_CONFIG;

/*
*
*
* ------------------- AX I/O CONTROL CODES -------------------
*
*
*/

// ========================================================
//                 AX I/O Machine control
// ========================================================
#define AX_IOCC_MACHINE (ULONG)(0x00000001)

// ========================================================
//          AX I/O Host machine break interrupt
// ========================================================
#define AX_IOCC_DEBUG (ULONG)(0xFFFFFFFF)

#endif
