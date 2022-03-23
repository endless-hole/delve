#pragma warning(disable : 4201)
#pragma warning(disable : 4214)



#include <ntddk.h>

#include "process/process.h"

#include "log.h"


extern void NTAPI init_server(void*);

NTSTATUS entrypoint(uint64_t driver_base)
{
    KeEnterGuardedRegion();

    delve::process::driver_base = driver_base;

    PWORK_QUEUE_ITEM work_item = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));

    ExInitializeWorkItem(work_item, init_server, work_item);

    ExQueueWorkItem(work_item, DelayedWorkQueue);

    KeLeaveGuardedRegion();

    return 1337;
}