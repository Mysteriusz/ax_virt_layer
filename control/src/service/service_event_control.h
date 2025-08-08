#if !defined(AX_SERVICE_EVENT_INT)
#define AX_SERVICE_EVENT_INT

#include "ax_utility.h"

/*
 
   	Service event handles

*/

#define START_EVENT_ID 			0x00000000
extern HANDLE start_event_handle;
#define STOP_EVENT_ID 			0x00000001
extern HANDLE stop_event_handle;

void service_intialize_events(
	void
);

/*
 	All actions that are to take place on specific events
	HAVE to be included in this function
*/
bool service_call_event(
	AX_IN uint32_t			event_id
);

bool service_start_sequence(
	void
);
bool service_stop_sequence(
	void
);

#endif // !defined(AX_SERVICE_EVENT_INT)

