#include "machine.h"

AXSTATUS AXCreateMachine(
	_In_opt_ PAX_MACHINE_CONFIG config,
	_Out_ PAX_MACHINE_ROOT* root
) {
	if (root == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	UNREFERENCED_PARAMETER(config);

	POOL_EXTENDED_PARAMETER prm;
	prm.Type = PoolExtendedParameterPriority;
	prm.Priority = NormalPoolPriority;

	*root = ExAllocatePool3(POOL_FLAG_PAGED, sizeof(PAX_MACHINE_ROOT), 'XAMH', &prm, 1);
	
	ASSERT(*root != NULL);

	PAX_MACHINE_ROOT temp = *root;
	temp->version = AXVER_0;

	return STATUS_SUCCESS;
}

