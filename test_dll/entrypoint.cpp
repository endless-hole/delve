
#include "../Libs/cnsl.h"
#include "../Libs/proc.h"

#pragma comment(lib, "ntdll64.lib")
#pragma comment(lib, "Libs64.lib")

typedef struct
{
	KernelCallbackProc* cb_entry_pointer;
	KernelCallbackProc cb_entry_original;
}
ldr_data_t;

#pragma data_seg(push, ".ldr")
__declspec(dllexport, allocate(".ldr")) ldr_data_t ldr_data;
#pragma data_seg(pop)

void unhook_KernelCallbackTable(void)
{
	ulong_t old_protect = proc_vmprotect(NtCurrentProcess(), 
		ldr_data.cb_entry_pointer, 8, PAGE_EXECUTE_READWRITE); 

	*ldr_data.cb_entry_pointer = ldr_data.cb_entry_original;

	proc_vmprotect(NtCurrentProcess(), 
		ldr_data.cb_entry_pointer, 8, old_protect);
}

NTSTATUS NTAPI entrypoint_onKernelCallback(PVOID dummy)
{
#ifndef SUPPRESS_DBG_MSG
	cnsl_alloc();
#endif

	dbg_msg((char*)"mapped dll entered entrypoint without new thread.");

	unhook_KernelCallbackTable();

	return ldr_data.cb_entry_original(dummy);
}