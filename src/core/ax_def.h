#ifndef AX_DRIVER_ENV
#define AX_DRIVER_ENV

#include <ntddk.h>
#include <initguid.h>

#define AXVER_0 0x00000000

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
//                    AX Parser command
// ========================================================
typedef struct _AX_SUBCOMMAND {
	UINT32 argsCount;
	STRING* args;
} AX_SUBCOMMAND, *PAX_SUBCOMMAND;
typedef struct _AX_COMMAND {
	STRING action;
	PAX_SUBCOMMAND subCommands;
} AX_COMMAND, *PAX_COMMAND;

// ========================================================
//                 AX Machine memory root
// ========================================================
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
