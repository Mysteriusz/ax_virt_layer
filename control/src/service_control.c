#include "service_control.h"

SERVICE_STATUS_HANDLE service_status_handle = NULL;
SERVICE_STATUS service_status = {0};

HANDLE stop_event_handle = NULL;
HANDLE start_event_handle = NULL;

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

void service_start_sequence(
	void
){
	service_status.dwServiceType = AX_CONTROL_SERVICE_TYPE;

	service_control_report(SERVICE_START_PENDING, 0);

	service_status.dwControlsAccepted = AX_CONTROL_SERVICE_CALLS;
	SetEvent(start_event_handle);	

	service_control_report(SERVICE_RUNNING, 0);

	return;
}
void service_stop_sequence(
	void
){
	service_status.dwServiceType = AX_CONTROL_SERVICE_TYPE;

	service_control_report(SERVICE_STOP_PENDING, 0);

	SetEvent(stop_event_handle);	

	service_control_report(SERVICE_STOPPED, 0);

	return;
}

