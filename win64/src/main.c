#include "ax_utility.h"

NTSTATUS DriverEntry(
  	PDRIVER_OBJECT  	DriverObject,
  	PUNICODE_STRING 	RegistryPath
){
	DbgPrint("AX_VIRT_LAYER STARTING\n");
	return STATUS_SUCCESS;
}

