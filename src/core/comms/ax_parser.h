/*

	Description:

	Token parser for AX Project command reader.
	Works by managing internally token allocations which allows easy command reading.

	Warnings:

	Use carefully since it manages memory allocations internally.
	Method usage recommendation map:

		ReadCommand     --->	 FreeCommand
		ReadSubcommand  --->	 FreeSubcommand
		ReadToken       --->	 FreeToken

*/

#ifndef AX_DRIVER_PARSER
#define AX_DRIVER_PARSER

#include "../ax_core.h"

/*

Routine Description:
	Parses commandString to PAX_COMMAND structure and it`s subcommands.

Arguments:
	commandString - Null-terminated input string to parse.
	command       - Receives a pointer to the parsed command.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
_Success_(!NT_ERROR(return))
_When_(return == STATUS_SUCCESS, _Post_satisfies_(*command != NULL))
ReadCommand(
	_In_ PCHAR commandString,
	_Outptr_ PAX_COMMAND* command
);

/*

Routine Description:
	Free`s command structure and all it`s subcommands.

Arguments:
	command - Previously allocated command structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS 
FreeCommand(
	_In_ PAX_COMMAND command
);


/*

Routine Description:
	Free`s subcommand structure and all it`s tokens.

Arguments:
	commandString - Previously allocated command structure.
	index         - Starting index of the commandString.
	eindex        - Next non-masked index of commandString.
	subcommand    - Receives a pointer to the parsed subcommand.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
_Success_(!NT_ERROR(return))
_Post_satisfies_(*subcommand != NULL)
ReadSubcommand(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_opt_ PUINT32 eindex,
	_Outptr_ PAX_SUBCOMMAND* subcommand
);


/*

Routine Description:
	Free`s subcommand structure and all it`s tokens.

Arguments:
	subcommand - Previously allocated subcommand structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
FreeSubcommand(
	_In_ PAX_SUBCOMMAND subcommand
);


#define AXTOKEN_COUNT 4
static PCHAR AXTOKENS[AXTOKEN_COUNT] = {"QUERY", "FETCH", "REQUEST", "SET"};

/*

	Comamnd structrue examples

	Ex 0:

	QUERY (TOKEN1 TOKEN2) : FETCH (TOKEN1 TOKEN2);
	^	  ^       ^         ^      ^      ^	     ^
	SCmd0 T0	  T1        SCmd1  T0     T1     CMD_END
	
	Ex 1:

	Ends with null-terminated string

	QUERY TOKEN1 TOKEN2 | FETCH TOKEN1 TOKEN2\0
	^	  ^      ^        ^     ^      ^	  ^
	SCmd0 T0	 T1       SCmd1 T0     T1     CMD_END

*/

#define AXTOKEN_SYNTAX_COUNT 3
static CHAR AXTOKEN_SYNTAX[AXTOKEN_SYNTAX_COUNT] = {'(', ')', ' '};

#define AXSUBCOMMAND_SYNTAX_COUNT 3
static CHAR AXSUBCOMMAND_SYNTAX[AXSUBCOMMAND_SYNTAX_COUNT] = {':', '|'};

#define AXCOMMAND_SYNTAX_COUNT 2
static CHAR AXCOMMAND_SYNTAX[AXCOMMAND_SYNTAX_COUNT] = {';', '\0'};

static BOOLEAN AXTOKEN_CHECK(PCHAR val) {
	for (int i = 0; i < AXTOKEN_SYNTAX_COUNT; i++) {
		if (*val == AXTOKEN_SYNTAX[i]) return TRUE;
	}
	return FALSE;
}
static BOOLEAN AXSUBCOMMAND_CHECK(PCHAR val) {
	for (int i = 0; i < AXSUBCOMMAND_SYNTAX_COUNT; i++) {
		if (*val == AXSUBCOMMAND_SYNTAX[i]) return TRUE;
	}
	return FALSE;
}
static BOOLEAN AXCOMMAND_CHECK(PCHAR val) {
	for (int i = 0; i < AXCOMMAND_SYNTAX_COUNT; i++) {
		if (*val == AXCOMMAND_SYNTAX[i]) return TRUE;
	}
	return FALSE;
}

#define AXSYNTAX_CHECK_TOKEN 0x01
#define AXSYNTAX_CHECK_SUBCOMMAND 0x02
#define AXSYNTAX_CHECK_COMMAND 0x04

static BOOLEAN AXSYNTAX_CHECK(PCHAR val, UINT32 mask) {
	return 
		(AXTOKEN_CHECK(val) && (mask & AXSYNTAX_CHECK_TOKEN)) ||
		(AXSUBCOMMAND_CHECK(val) && (mask & AXSYNTAX_CHECK_SUBCOMMAND)) ||
		(AXCOMMAND_CHECK(val) && (mask & AXSYNTAX_CHECK_COMMAND));
}

/*

Routine Description:
	Reads and allocated a token from index of provided commandString.

Arguments:
	commandString - Command string to read from.
	index		  - Starting index of the commandString.
	mask		  - Mask of the syntax checks.
	eindex        - Next non-masked index of commandString.
	token		  - Pointer to newly parsed token structure.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
_Success_(!NT_ERROR(return))
_When_(return == STATUS_SUCCESS, _Post_satisfies_(*token != NULL))
ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_In_ UINT32 mask,
	_Out_opt_ PUINT32 eindex,
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


/*

Routine Description:
	Skips ignored characters based on provided mask.

Arguments:
	commandString - Previously allocated command structure.
	index		  - Index from which to start syntax checking.
	mask          - Mask of the syntax checks.
	eindex        - Next non-masked index of commandString.

Return Value:
	AXSTATUS indicating success or failure.

*/
AXSTATUS
SkipIgnored(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_In_ UINT32 mask,
	_Out_ PUINT32 eindex
);

#endif
