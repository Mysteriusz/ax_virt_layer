#include "ax_parser.h"

AXSTATUS ParseCommand(
	_In_ PCHAR commandString,
	_Out_ PAX_COMMAND* command
) {
	if (commandString == NULL || command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	*command = NULL;

	PCHAR token = NULL;
	ReadToken(commandString, 0, &token);
	DbgPrint(token);

	return STATUS_SUCCESS;
}

AXSTATUS ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_ PCHAR* token
) {
	if (token == NULL || commandString == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	UINT32 len = 0;
	PCHAR curr = &commandString[index];
	while (!AXCOMMAND_BREAK_CHECK(curr) && !AXTOKEN_BREAK_CHECK(curr)) {
		len++;
		curr++;
	}

	*token = ExAllocatePool3(POOL_FLAG_PAGED, (len + 1) * sizeof(CHAR), 'SXA', NULL, 0);
	ASSERT(*token != NULL);

	RtlCopyMemory(*token, &commandString[index], len);
	(*token)[len] = '\0';

	return STATUS_SUCCESS;
}
