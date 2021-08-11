#pragma once
#include <ntddk.h>
#include "../stdint.h"

namespace delve
{
    namespace process
    {
        uint32_t  tgt_pid;
        PEPROCESS tgt_process;
        uintptr_t tgt_base_address;

        uint32_t  own_pid;
        PEPROCESS own_process;

        bool target_set = false;
    }
}
