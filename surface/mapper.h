#pragma once

#include "process.h"

namespace mmap
{
    PIMAGE_OPTIONAL_HEADER64 pe64_get_optional_header( void* mod_base );

    void* alloc_remote_module_memory( Process* proc, void* mod_base );

    bool_t map_remote_module( Process* proc, void* in_base, void** out_base, void** out_entrypoint );

    bool_t pe_imports_fix( Process* proc, void* mod_base, bool_t file_aligned );

    void* get_module_base( Process* proc, char* mod_name, size_t* image_size_out, ulong_t proc_type );

    void* remote_get_module( Process* proc, char* mod_name, void** remote_module_base, ulong_t proc_type );

    void* remote_get_proc_address( Process* proc, char* mod_name, char* func_name, ulong_t ord, ulong_t code_type );
}
