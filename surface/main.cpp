#include "../Libs/pe.h"
#include "../Libs/proc.h"
#include "log.h"
#include "process.h"
#include "utils.h"
#include "mapper.h"

typedef struct
{
	KernelCallbackProc* cb_entry_pointer;
	KernelCallbackProc cb_entry_original;
}
ldr_data_t;

static void hook_kct_init(Process* proc, ldr_data_t* ldr_data)
{
    KERNEL_CALLBACK_TABLE_64* remote_kct;
	void* remote_kct_proc;

    uint8_t* remote_peb = ( uint8_t* )proc->get_peb();

    while(true)
    {
        if(!proc->read_memory( remote_peb + offsetof(PEB64, KernelCallbackTable), &remote_kct, 8 ))
            continue;

        if(proc->read_memory( &remote_kct->fnDWORD, &remote_kct_proc, 8 ))
            break;

        Sleep(5);
    }

    ldr_data->cb_entry_pointer = &remote_kct->fnDWORD;
    ldr_data->cb_entry_original = (KernelCallbackProc)remote_kct_proc;
}

static void hook_kct(Process* proc, void* dest, ldr_data_t* ldr_data)
{
    ulong_t old_protect;

    old_protect = proc->protect( ldr_data->cb_entry_pointer, 8, PAGE_EXECUTE_READWRITE );

    proc->write_memory( ldr_data->cb_entry_pointer, &dest, 8 );

    old_protect = proc->protect( ldr_data->cb_entry_pointer, 8, old_protect );
}

int main()
{
    
    auto proc = new Process(L"EscapeFromTarkov.exe");

    auto dll = get_dll_by_file(L"test_dll.dll");

    void* mapped_module_base;
	void* mapped_module_entrypoint;
    void* remote_mod_base;

    ldr_data_t* ldr_data = (ldr_data_t*)pe_get_proc_address(dll, (char*)"ldr_data", 0, true);

    hook_kct_init( proc, ldr_data );

    if(mmap::map_remote_module( proc, dll, &mapped_module_base, &mapped_module_entrypoint ))
    {
        LOG("mapped_module_base: ", mapped_module_base);
        LOG("mapped_module_entrypoint: ", mapped_module_entrypoint);

        hook_kct( proc, mapped_module_entrypoint, ldr_data );
    }
    else
    {
        printf("failed to map!");
    }

    return true;
}
