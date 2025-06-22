/*

	Description:

	Token parser for AX Project command reader.
	Works by managing internally token allocations which allows easy command reading.

	Warnings:

	Use carefully since it manages memory allocations internally.
	Method usage recommendation map:

		ReadToken       --->	 FreeToken
		ReadCommand     --->	 FreeCommand

*/

#ifndef AX_DRIVER_PARSER
#define AX_DRIVER_PARSER

#include "ax_core.h"

/*

Routine Description:
	Parses command string to PAX_COMMAND structure and it`s tokens.

Arguments:
	commandString - Null-terminated input string to parse.
	command       - Receives a pointer to the parsed command.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
_Success_(!NT_ERROR(return))
_Post_satisfies_(*command != NULL)
ReadCommand(
	_In_ PCHAR commandString,
	_Outptr_ PAX_COMMAND* command
);

/*

Routine Description:
	Free`s command structure and all it`s tokens.

Arguments:
	command - Previously allocated command structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS 
FreeCommand(
	_In_ PAX_COMMAND command
);

#define AXTOKEN_COUNT 4
static PCHAR AXTOKENS[AXTOKEN_COUNT] = {"QUERY", "FETCH", "REQUEST", "SET"};

#define AXTOKEN_BREAK_COUNT 2
static CHAR AXTOKEN_BREAK[AXTOKEN_BREAK_COUNT] = {')', ' '};

#define AXSUBCOMMAND_BREAK_COUNT 1
static CHAR AXSUBCOMMAND_BREAK[AXSUBCOMMAND_BREAK_COUNT] = {'|'};

#define AXCOMMAND_BREAK_COUNT 2
static CHAR AXCOMMAND_BREAK[AXCOMMAND_BREAK_COUNT] = {';', '\0'};

static BOOLEAN AXTOKEN_BREAK_CHECK(PCHAR val) {
	for (int i = 0; i < AXTOKEN_BREAK_COUNT; i++) {
		if (*val == AXTOKEN_BREAK[i]) return TRUE;
	}
	return FALSE;
}
static BOOLEAN AXSUBCOMMAND_BREAK_CHECK(PCHAR val) {
	for (int i = 0; i < AXSUBCOMMAND_BREAK_COUNT; i++) {
		if (*val == AXSUBCOMMAND_BREAK[i]) return TRUE;
	}
	return FALSE;
}
static BOOLEAN AXCOMMAND_BREAK_CHECK(PCHAR val) {
	for (int i = 0; i < AXCOMMAND_BREAK_COUNT; i++) {
		if (*val == AXCOMMAND_BREAK[i]) return TRUE;
	}
	return FALSE;
}

/*

Routine Description:
	Reads and allocated a token from index of provided command string.

Arguments:
	commandString - Command string to read from.
	index		  - Starting index of the command string.
	token		  - Pointer to newly parsed token structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
_Success_(!NT_ERROR(return))
_Post_satisfies_(*token != NULL)
ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Outptr_ PAX_TOKEN* token
);

/*

Routine Description:
	Free`s token structure.

Arguments:
	token - Previously allocated token structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS 
FreeToken(
	_In_ PAX_TOKEN token
);

#endif
