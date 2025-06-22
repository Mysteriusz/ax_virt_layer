#include "ax_parser.h"

AXSTATUS 
_Success_(!NT_ERROR(return))
_Post_satisfies_(*command != NULL)
ReadCommand(
	_In_ PCHAR commandString,
	_Outptr_ PAX_COMMAND* command
) {
	if (commandString == NULL || command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}
	
	*command = NULL;
	__analysis_assume(*command != NULL);

	AXSTATUS status = STATUS_SUCCESS;
	PAX_COMMAND temp = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_COMMAND), 'CXA', NULL, 0);
	if (temp == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	temp->subCommands = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_SUBCOMMAND), 'CSXA', NULL, 0);
	if (temp->subCommands == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	UINT32 si = 0;
	UINT32 ti = 0;
	while (!AXCOMMAND_BREAK_CHECK(&commandString[si])){
		PAX_TOKEN token = NULL;
		status = ReadToken(commandString, si, &token);
		if (NT_ERROR(status)) {
			FreeCommand(temp);
			return status;
		}

		DbgPrint(token->buffer);

		temp->subCommands[0].tokens[ti] = token;
		temp->subCommands[0].tokenCount++;
		
		si += token->len;
		ti++;
	}

	*command = temp;

	return STATUS_SUCCESS;
}
AXSTATUS 
FreeCommand(
	_In_ PAX_COMMAND command
) {
	if (command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	if (command->subCommands != NULL) {
		for (UINT32 i = 0; i < command->subCommands[0].tokenCount; i++) {
			FreeToken(command->subCommands[0].tokens[i]);
		}
		ExFreePool(command->subCommands);
	}

	ExFreePool(command);

	return STATUS_SUCCESS;
}

AXSTATUS 
_Success_(!NT_ERROR(return))
_Post_satisfies_(*token != NULL)
ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Outptr_ PAX_TOKEN* token
) {
	if (token == NULL || commandString == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	*token = NULL;

	UINT32 len = 0;
	PCHAR curr = &commandString[index];
	while (!AXCOMMAND_BREAK_CHECK(curr) && !AXTOKEN_BREAK_CHECK(curr)) {
		len++;
		curr++;
	}

	PAX_TOKEN temp = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_TOKEN), 'KTXA', NULL, 0);
	if (temp == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	temp->len = (len + 1) * sizeof(CHAR);
	temp->buffer = ExAllocatePool3(POOL_FLAG_PAGED, temp->len, 'TSXA', NULL, 0);
	if (temp->buffer == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(temp->buffer, &commandString[index], temp->len);
	temp->buffer[len] = '\0';

	*token = temp;

	return STATUS_SUCCESS;
}
AXSTATUS 
FreeToken(
	_In_ PAX_TOKEN token
) {
	ExFreePool(token->buffer);
	ExFreePool(token);

	return STATUS_SUCCESS;
}
