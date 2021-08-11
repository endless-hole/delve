#pragma warning(disable : 4201)
#pragma warning(disable : 4214)

#include <ntddk.h>

#include "log.h"

extern void NTAPI init_server(void*);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    KeEnterGuardedRegion();

    UNREFERENCED_PARAMETER( DriverObject );
    UNREFERENCED_PARAMETER( RegistryPath );

    PWORK_QUEUE_ITEM work_item = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));

    ExInitializeWorkItem(work_item, init_server, work_item);

    ExQueueWorkItem(work_item, DelayedWorkQueue);

    KeLeaveGuardedRegion();

    return 1337;
}