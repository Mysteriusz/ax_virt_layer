#include "service_control.h"

#if !defined(AX_UM)
#error "This file can only be used in USER MODE"
#endif

#if defined(AX_WINDOWS) 

void WINAPI service_entry(
	uint32_t 			arg_count,
	wchar_t**			args
){
	service_status_handle = RegisterServiceCtrlHandlerExW(AX_CONTROL_SERVICE_NAME, (LPHANDLER_FUNCTION_EX)service_control_handler, NULL);
	if (service_status_handle == NULL){
		return;
	}

	start_event_handle = CreateEventW(NULL, 1, 0, NULL);
	stop_event_handle = CreateEventW(NULL, 1, 0, NULL);

	service_start_sequence();

	WaitForSingleObject(stop_event_handle, INFINITE);
	__debugbreak();
	return;
}

int wmain(){
	SERVICE_TABLE_ENTRYW service_table[] = {
        	{ 
			AX_CONTROL_SERVICE_NAME, 
			(LPSERVICE_MAIN_FUNCTIONW)service_entry
		}
    	};
    	StartServiceCtrlDispatcherW(service_table);
    	return NO_ERROR;
}

#endif

