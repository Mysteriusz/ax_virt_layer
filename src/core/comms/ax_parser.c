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

	AXSTATUS status = STATUS_SUCCESS;
	*command = NULL;

	PAX_COMMAND temp = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_COMMAND), 'CXA', NULL, 0);
	if (temp == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	UINT32 si = 0; // STRING INDEX
	UINT32 sbi = 0; // SUB COMMAND INDEX
	do {
		DbgPrint("SBI: %u\n", sbi);
		// Read subcommand from current STRING INDEX
		status = ReadSubcommand(commandString, si, &si, &temp->subCommands[sbi]);
		if (NT_ERROR(status)) {
			DbgBreakPoint();
		}

		sbi++;
	} while (!AXSYNTAX_CHECK(&commandString[si], 0x04));

	*command = temp;
	__analysis_assume(*command != NULL);

	return status;
}
AXSTATUS 
FreeCommand(
	_In_ PAX_COMMAND command
) {
	if (command == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	for (UINT32 i = 0; i < AXMAX_SUBCOMMANDS; i++) {
		FreeSubcommand(command->subCommands[i]);
	}

	ExFreePool(command);

	return STATUS_SUCCESS;
}

AXSTATUS
_Success_(!NT_ERROR(return))
_Post_satisfies_(*subcommand != NULL)
ReadSubcommand(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_Out_opt_ PUINT32 eindex,
	_Outptr_ PAX_SUBCOMMAND* subcommand
) {
	if (commandString == NULL || subcommand == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	*subcommand = NULL;

	PAX_SUBCOMMAND temp = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_SUBCOMMAND), 'CXA', NULL, 0);
	if (temp == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	__analysis_assume(*subcommand != NULL);

	UINT32 si = index;
	SkipIgnored(commandString, si, 0x02, &si);

	UINT32 ti = 0;
	PCHAR curr = &commandString[si];
	while (!AXSYNTAX_CHECK(curr, 0x02 | 0x04)) {
		PAX_TOKEN token = NULL;
		AXSTATUS status = ReadToken(commandString, si, 0x01, &si, &token);
		if (NT_ERROR(status) && status != STATUS_ABANDONED) {
			FreeSubcommand(temp);
			return status;
		}

		if (status != STATUS_ABANDONED) {
			temp->tokenCount++;
			temp->tokens[ti++] = token;
			DbgPrint("TOKEN: %s SINDEX: %u\n", token->buffer, si);
		}
		curr = &commandString[si];
	}
	temp->length = si - index;

	*subcommand = temp;
	if (eindex != NULL) {
		*eindex = si;
	}

	return STATUS_SUCCESS;
}
AXSTATUS
FreeSubcommand(
	_In_ PAX_SUBCOMMAND subcommand
) {
	for (UINT32 i = 0; i < AXMAX_TOKENS; i++) {
		FreeToken(subcommand->tokens[i]);
	}

	ExFreePool(subcommand);

	return STATUS_SUCCESS;
}

AXSTATUS
_Success_(!NT_ERROR(return))
_When_(return == STATUS_SUCCESS, _Post_satisfies_(*token != NULL))
ReadToken(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_In_ UINT32 mask,
	_Out_opt_ PUINT32 eindex,
	_Outptr_ PAX_TOKEN* token
) {
	if (commandString == NULL || token == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	*token = NULL;
	UINT32 si = index;
	SkipIgnored(commandString, index, mask, &si);

	PAX_TOKEN temp = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(AX_TOKEN), 'KTXA', NULL, 0);
	if (temp == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	__analysis_assume(*token != NULL);

	PCHAR curr = &commandString[si];
	while (!AXSYNTAX_CHECK(curr, mask | ~mask)) {
		temp->length++;
		curr++;
	}

	if (eindex != NULL) {
		*eindex = si + temp->length;
	}

	if (temp->length == 0) {
		FreeToken(temp);
		return STATUS_ABANDONED;
	}

	temp->buffer = ExAllocatePool3(POOL_FLAG_PAGED, (temp->length + 1) * sizeof(CHAR), 'RTS', NULL, 0);
	if (temp->buffer == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(temp->buffer, &commandString[si], temp->length);
	temp->buffer[temp->length] = '\0';

	*token = temp;

	return STATUS_SUCCESS;
}

AXSTATUS 
FreeToken(
	_In_ PAX_TOKEN token
) {
	if (token != NULL) {
		if (token->buffer != NULL) {
			ExFreePool(token->buffer);
		}
		ExFreePool(token);
	}

	return STATUS_SUCCESS;
}

AXSTATUS
SkipIgnored(
	_In_ PCHAR commandString,
	_In_ UINT32 index,
	_In_ UINT32 mask,
	_Out_ PUINT32 eindex
) {
	if (commandString == NULL || eindex == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	*eindex = index;

	PCHAR curr = &commandString[index];
	while (AXSYNTAX_CHECK(curr, mask)) {
		(*eindex)++;
		curr++;
	}
	return STATUS_SUCCESS;
}
