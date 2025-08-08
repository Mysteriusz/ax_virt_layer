#include "ax_utility.h"
#include "ax_control.h"

extern SERVICE_STATUS_HANDLE service_status_handle;
extern SERVICE_STATUS service_status;

void service_control_report(
	uint32_t state,
	uint32_t exit_code
);

uint32_t service_control_handler(
	uint32_t control,
	uint32_t event_type,
	void* event_data,
	void* context
);

extern HANDLE start_event_handle;
void service_start_sequence(
	void
);

extern HANDLE stop_event_handle;
void service_stop_sequence(
	void
);

