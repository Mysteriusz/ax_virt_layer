#if !defined(AX_SERVICE_CONTROL_INT)
#define AX_SERVICE_CONTROL_INT

#include "ax_utility.h"
#include "ax_control.h"
#include "service_event_control.h"

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

#endif // !defined(AX_SERVICE_CONTROL_INT)

