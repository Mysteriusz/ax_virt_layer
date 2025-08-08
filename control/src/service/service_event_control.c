#include "service_event_control.h"
#include "service_control.h"

HANDLE start_event_handle = NULL;
HANDLE stop_event_handle = NULL;

void service_intialize_events(
	void
){
	start_event_handle = CreateEventW(NULL, 1, 0, NULL);
	stop_event_handle = CreateEventW(NULL, 1, 0, NULL);

	return;
}

bool service_call_event(
	AX_IN uint32_t			event_id
){
	bool fail = true;

	switch(event_id){
	case START_EVENT_ID:
		fail = false;

		SetEvent(start_event_handle);
		break;
	case STOP_EVENT_ID:
		fail = false;

		SetEvent(stop_event_handle);
		break;
	default:
		return true;
	}

	return fail;
}

bool service_start_sequence(
	void
){
	service_status.dwServiceType = AX_CONTROL_SERVICE_TYPE;
	service_control_report(SERVICE_START_PENDING, 0);

	// Call start event and check result
	if (service_call_event(START_EVENT_ID)){
		return true;
	}

	__debugbreak();
	service_status.dwControlsAccepted = AX_CONTROL_SERVICE_CALLS;

	service_control_report(SERVICE_RUNNING, 0);

	return false;
}
bool service_stop_sequence(
	void
){
	service_control_report(SERVICE_STOP_PENDING, 0);

	// Call start event and check result
	if (service_call_event(STOP_EVENT_ID)){
		return true;
	}

	service_control_report(SERVICE_STOPPED, 0);

	return false;
}

