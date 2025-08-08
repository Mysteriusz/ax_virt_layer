#include "service_control.h"

#if !defined(AX_UM)
#error "This applcation can only be compiled for USER-MODE"
#endif
#if !defined(AX_WINDOWS) || defined(AX_LINUX)
#error "This applcation can only be compiled for Windows"
#endif

void WINAPI service_entry(
	uint32_t 			arg_count,
	wchar_t**			args
){
	service_status_handle = RegisterServiceCtrlHandlerExW(AX_CONTROL_SERVICE_NAME, (LPHANDLER_FUNCTION_EX)service_control_handler, NULL);
	if (service_status_handle == NULL){
		return;
	}

	service_intialize_events();

	service_start_sequence();

	WaitForSingleObject(stop_event_handle, INFINITE);
	service_stop_sequence();
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

