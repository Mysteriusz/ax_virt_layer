#include "service_control.h"

SERVICE_STATUS_HANDLE service_status_handle = NULL;
SERVICE_STATUS service_status = {0};

void service_control_report(
	uint32_t state,
	uint32_t exit_code
){
	if (service_status_handle == NULL){
		return;
	}

	service_status.dwCurrentState = state;
	service_status.dwWin32ExitCode = exit_code;

	SetServiceStatus(service_status_handle, &service_status);

	return;
}

uint32_t service_control_handler(
	uint32_t control,
	uint32_t event_type,
	void* event_data,
	void* context
){
	switch (control){
	case SERVICE_CONTROL_STOP:
		service_stop_sequence();
		break;
	default:
		return NO_ERROR;
	}

	return NO_ERROR;
}

