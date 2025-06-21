#include "ax_core.h"

AXSTATUS ParseCommand(
	_In_ PCHAR commandString,
	_Out_ PAX_COMMAND* command
);

static PCHAR AXTOKENS[4] = {"QUERY", "FETCH", "REQUEST", "SET"};

static CHAR AXTOKENS_BREAK[1] = {')'};
static CHAR AXCOMMAND_BREAK[2] = {';', '\0'};

#define AXTOKEN_BREAK_CHECK(val) (RtlCompareMemory(val, &AXTOKENS_BREAK[0], sizeof(CHAR)))
#define AXCOMMAND_BREAK_CHECK(val) (RtlCompareMemory(val, &AXCOMMAND_BREAK[0], sizeof(CHAR)) || RtlCompareMemory(val, &AXCOMMAND_BREAK[1], sizeof(CHAR)))

AXSTATUS ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_ PAX_TOKEN token
);