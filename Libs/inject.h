/*******************************************************************************
	HACK THE PLANETSIDE
	Libs - inject.h
	Library for stealthy manual mapping (x64 only)
*******************************************************************************/
#ifndef inject_h__
#define inject_h__

#include "types.h"

typedef struct
{
	char* base_module_name;
	char* module_name;
	char* func_name;

	void* old_func_addr;
	void* old_func_ptr;
	void* dest;

} iat_context_t;

typedef struct
{
	union
	{
		char* base_module_name;
		uint64_t dummy_a;
	};

	union
	{
		char* module_name;
		uint64_t dummy_b;
	};

	union
	{
		char* func_name;
		uint64_t dummy_c;
	};

	uint32_t old_func_addr;
	uint32_t old_func_ptr;
	uint32_t dest;

} iat_context32_t;

void* alloc_remote_module_memory (
	handle_t proc_handle,
	void* module
);

bool_t remote_iat_hook_init (
	iat_context_t* ctx,
	handle_t proc_handle
);

bool_t remote_iat_hook32_init(
	iat_context_t* ctx,
	handle_t proc_handle
);

bool_t map_remote_module (
	handle_t proc_handle,
	void* module,
	void* remote_mem_optional,
	void** out_base, 
	void** out_entry_point
);

bool_t map_remote_module32 (
	handle_t proc_handle,
	void* module,
	void* remote_mem_optional,
	void** out_base,
	void** out_entry_point
);

bool_t remote_iat_hook_install (
	iat_context_t* ctx,
	handle_t proc_handle
);

bool_t remote_iat_hook32_install (
	iat_context_t* ctx,
	handle_t proc_handle
);

bool_t remote_iat_hook_init_32(
	iat_context32_t* ctx, 
	handle_t proc_handle);

bool_t remote_iat_hook_install_32(
	iat_context32_t* ctx, 
	handle_t proc_handle
);

#endif // inject_h__