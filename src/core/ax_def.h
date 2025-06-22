/*

	Description:

	Definition file for AX_VIRTUALIZATION_DRIVER

*/

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
//                     AX Command token
// ========================================================
typedef struct _AX_TOKEN {
	UINT32 len;
	PCHAR buffer;
} AX_TOKEN, *PAX_TOKEN;

// ========================================================
//                 AX Tokenized subcommand
// ========================================================
#define AXMAX_TOKENS 20
typedef struct _AX_SUBCOMMAND {
	UINT32 tokenCount;
	PAX_TOKEN tokens[AXMAX_TOKENS];
} AX_SUBCOMMAND, *PAX_SUBCOMMAND;

// ========================================================
//                    AX Parsed command
// ========================================================
#define AXMAX_SUBCOMMANDS 10
typedef struct _AX_COMMAND {
	STRING action;
	UINT32 subCommandCount;
	PAX_SUBCOMMAND subCommands[AXMAX_SUBCOMMANDS];
} AX_COMMAND, *PAX_COMMAND;

// ========================================================
//                 AX Machine memory root
// ========================================================
typedef struct _AX_MACHINE_ROOT {
	UINT32 version;
} AX_MACHINE_ROOT, *PAX_MACHINE_ROOT;

// ========================================================
//             AX Machine build configuration
// ========================================================
typedef struct _AX_MACHINE_CONFIG {
	UINT32 cpuCount;
} AX_MACHINE_CONFIG, *PAX_MACHINE_CONFIG;

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
