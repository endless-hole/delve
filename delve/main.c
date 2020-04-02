#pragma warning(disable : 4201)
#pragma warning(disable : 4214)

#include <ntddk.h>
#include <stdarg.h>

#include "arch/cpuid.h"
#include "arch/vcpu.h"
#include "arch/cr.h"
#include "arch/msr.h"

void log_debug( const char* fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    vDbgPrintExWithPrefix( "[*] ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args );

    va_end( args );
}

int VmHasCpuidSupport()
{
    union __cpuid_t cpuid = { 0 };
    __cpuid( cpuid.cpu_info, 1 );

    return cpuid.feature_ecx.virtual_machine_extensions;
}

int enable_vmx_operation()
{
    union __cr4_t cr4 = { 0 };
    union __ia32_feature_control_msr_t feature_msr = { 0 };


    cr4.control = __readcr4();
    cr4.bits.vmx_enable = 1;
    __writecr4( cr4.control );

    feature_msr.control = __readmsr( IA32_FEATURE_CONTROL );

    if( feature_msr.bits.lock == 0 )
    {
        feature_msr.bits.vmxon_outside_smx = 1;
        feature_msr.bits.lock = 1;

        __writemsr( IA32_FEATURE_CONTROL, feature_msr.control );

        return TRUE;
    }

    return FALSE;
}

NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
    )
{
    UNREFERENCED_PARAMETER( DriverObject );
    UNREFERENCED_PARAMETER( RegistryPath );

    DbgPrint( "[*]%s - output test from delve.\n", __FUNCTION__ );

    return STATUS_SUCCESS;
}