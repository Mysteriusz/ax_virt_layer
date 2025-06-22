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

	UINT32 si = 0; // STRING INDEX
	UINT32 sbi = 0; // SUBCOMMAND INDEX
	do {
		// Break the command parsing on any arugment overflow.
		if (sbi == AXMAX_SUBCOMMANDS) {
			FreeCommand(temp);
			return STATUS_STACK_OVERFLOW;
		}

		temp->subCommands[sbi] = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_SUBCOMMAND), 'CSXA', NULL, 0);
		if (temp->subCommands[sbi] == NULL) {
			FreeCommand(temp);
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		temp->subCommandCount++;

		UINT32 ti = 0; // TOKEN INDEX
		while (!AXSUBCOMMAND_BREAK_CHECK(&commandString[si])) {
			// Break the command parsing on any arugment overflow.
			if (ti == AXMAX_TOKENS) {
				FreeCommand(temp);
				return STATUS_STACK_OVERFLOW;
			}

			if (AXCOMMAND_BREAK_CHECK(&commandString[si])) {
				break;
			}
			
			PAX_TOKEN token = NULL;
			status = ReadToken(commandString, si, &token);
			if (NT_ERROR(status)) {
				FreeCommand(temp);
				return status;
			}

			temp->subCommands[sbi]->tokens[ti] = token;
			temp->subCommands[sbi]->tokenCount++;
			DbgPrint(token->buffer);
			DbgPrint("\n");

			si += token->len;
			ti++;
		}
		sbi++;

	} while (!AXCOMMAND_BREAK_CHECK(&commandString[si]));

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

	PAX_SUBCOMMAND sc = NULL;
	// Free all subcommands and their tokens.
	for (UINT32 si = 0; si < command->subCommandCount; si++) {
		sc = command->subCommands[si];
		if (sc == NULL) continue;

		for (UINT32 ti = 0; ti < sc->tokenCount; ti++) {
			FreeToken(sc->tokens[ti]);
		}

		ExFreePool(sc);
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
