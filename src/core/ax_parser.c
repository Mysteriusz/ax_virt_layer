#include "ax_parser.h"

AXSTATUS ReadCommand(
	_In_ PCHAR commandString,
	_Out_ PAX_COMMAND* command
) {
	if (commandString == NULL || command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	AXSTATUS status = STATUS_SUCCESS;
	*command = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_COMMAND), 'CXA', NULL, 0);
	if (*command == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	(*command)->subCommands = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_SUBCOMMAND), 'CSXA', NULL, 0);
	if ((*command)->subCommands == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory((*command)->subCommands, sizeof(AX_SUBCOMMAND));

	UINT32 si = 0;
	UINT32 ti = 0;
	while (!AXCOMMAND_BREAK_CHECK(&commandString[si])){
		PAX_TOKEN token;
		status = ReadToken(commandString, si, &token);
		if (NT_ERROR(status)) {
			FreeCommand(*command);
			return status;
		}

		(*command)->subCommands[0].tokens[ti] = token;
		(*command)->subCommands[0].tokenCount++;

		DbgPrint("TOKEN: ");
		DbgPrint(token->buffer);
		DbgPrint("\n");

		si += token->len;
		ti++;
	}

	return status;
}
AXSTATUS FreeCommand(
	_In_ PAX_COMMAND command
) {
	if (command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	if (command->subCommands != NULL) {
		for (UINT32 i = 0; i < command->subCommands[0].tokenCount; i++) {
			FreeToken(command->subCommands[0].tokens[i]);
		}
	}

	ExFreePool(command);

	return STATUS_SUCCESS;
}

AXSTATUS ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_ PAX_TOKEN* token
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

	*token = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_TOKEN), 'KTXA', NULL, 0);
	if (*token == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	(*token)->len = (len + 1) * sizeof(CHAR);
	(*token)->buffer = ExAllocatePool3(POOL_FLAG_PAGED, (*token)->len, 'TSXA', NULL, 0);
	ASSERT((*token)->buffer != NULL);

	RtlCopyMemory((*token)->buffer, &commandString[index], (*token)->len);
	(*token)->buffer[len] = '\0';

	return STATUS_SUCCESS;
}
AXSTATUS FreeToken(
	_In_ PAX_TOKEN token
) {
	ExFreePool(token->buffer);
	ExFreePool(token);

	return STATUS_SUCCESS;
}
