#include "drv_func.h"
#include <intrin.h>

NTSTATUS AxDispatchCreate(
	struct _DEVICE_OBJECT 	*device_object,
	struct _IRP 		*irp
){
	NT_ASSERT(device_object != nullptr);
	NT_ASSERT(irp != nullptr);

	axres res = AX_SUCC;
	axres_s *res_s = (axres_s*)&res;

	// Verify process executable
	/*res = verify_proc(IoGetRequestorProcess(irp));
	if (AX_ERR(res | AX_META_NTSTATUS)){
		return res_s->meta_err;
	}*/

    int *cpu_info = axmalloc(4); // EAX, EBX, ECX, EDX
    __cpuid(cpu_info, 1);
    if (cpu_info[2] & (1 << 5)) {
        io_str(u"VMX supported\n");
    } else {
        io_str(u"VMX NOT supported\n");
    }
    axfree(cpu_info);

	//void *t = axmalloc(1024);
	//io_i64(__vmx_vmptrld(t));
	//axfree(t);

	IoCompleteRequest(
		irp,
		IO_NO_INCREMENT
	);

	return res_s->meta_err;
}

