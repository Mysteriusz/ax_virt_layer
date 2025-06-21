/*

	Description:

	Token parser for AX Project command reader.
	Works by managing internally token allocations which allows easy command reading.

	Warnings:

	Use carefully since it manages memory allocations internally.
	Method usage recommendation map:

		ReadToken       --->	 FreeToken
		ParseCommand    --->	 FreeCommand

*/

#ifndef AX_DRIVER_PARSER
#define AX_DRIVER_PARSER

#include "ax_core.h"

AXSTATUS ReadCommand(
	_In_ PCHAR commandString,
	_Out_ PAX_COMMAND* command
);
AXSTATUS FreeCommand(
	_In_ PAX_COMMAND command
);

static PCHAR AXTOKENS[4] = {"QUERY", "FETCH", "REQUEST", "SET"};

static CHAR AXTOKENS_BREAK[2] = {')', ' '};
static CHAR AXCOMMAND_BREAK[2] = {';', '\0'};

#define AXTOKEN_BREAK_CHECK(val) (RtlCompareMemory(val, &AXTOKENS_BREAK[0], sizeof(CHAR)) || RtlCompareMemory(val, &AXTOKENS_BREAK[1], sizeof(CHAR)))
#define AXCOMMAND_BREAK_CHECK(val) (RtlCompareMemory(val, &AXCOMMAND_BREAK[0], sizeof(CHAR)) || RtlCompareMemory(val, &AXCOMMAND_BREAK[1], sizeof(CHAR)))

AXSTATUS ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_ PAX_TOKEN* token
);
AXSTATUS FreeToken(
	_In_ PAX_TOKEN token
);

#endif
