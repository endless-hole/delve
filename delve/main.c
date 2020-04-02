#include <ntddk.h>
#include "defs/cpuid.h"

void log_debug( const char* fmt, ... ){
    va_list args;
    va_start( args, fmt );

    vDbgPrintExWithPrefix( "[*] ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args );

    va_end( args );
}

int VmHasCpuidSupport( void ){
    union __cpuid_t cpuid = { 0 };
    __cpuid( cpuid.cpu_info, 1 );

    return cpuid.feature_ecx.virtual_machine_extensions;
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