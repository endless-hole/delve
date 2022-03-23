#pragma once

#include <stdint.h>

namespace delve
{
    namespace process
    {
        static uint32_t  tgt_pid;
        static PEPROCESS tgt_process;
        static uintptr_t tgt_base_address;

        static uint32_t  own_pid;
        static PEPROCESS own_process;

        static uint64_t  driver_base;

        static void* ntoskrnl = nullptr;

        static bool target_set = false;

        static bool kill_server = false;

    }
}
