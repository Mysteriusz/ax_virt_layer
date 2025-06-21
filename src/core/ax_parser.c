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

#include "ax_parser.h"

AXSTATUS ParseCommand(
	_In_ PCHAR commandString,
	_Out_ PAX_COMMAND* command
) {
	if (commandString == NULL || command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	AXSTATUS status;
	*command = NULL;

	AX_TOKEN token;
	status = ReadToken(commandString, 0, &token);
	ASSERT(NT_ERROR(status) == 0);

	return STATUS_SUCCESS;
}

AXSTATUS ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_ PAX_TOKEN token
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

	token->len = (len + 1) * sizeof(CHAR);
	token->buffer = ExAllocatePool3(POOL_FLAG_PAGED, token->len, 'SXA', NULL, 0);
	ASSERT(token->buffer != NULL);

	RtlCopyMemory(token->buffer, &commandString[index], token->len);
	token->buffer[len] = '\0';

	DbgPrint(token->buffer);

	return STATUS_SUCCESS;
}
AXSTATUS FreeToken(
	_In_ PAX_TOKEN token
) {
	UNREFERENCED_PARAMETER(token);

	return STATUS_SUCCESS;
}
