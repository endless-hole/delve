/*******************************************************************************
	HACK THE PLANETSIDE
	vtable.h
*******************************************************************************/
#ifndef VTABLE_HOOK_H_
#define VTABLE_HOOK_H_

#include "..\Libs\types.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct  
{
	void*** obj;
	void** curr;
	void** old;
	void** base_alloc;
	ulong_t size;

} vtable_context_t;

void vtable_create(vtable_context_t* ctx);

void vtable_swap(vtable_context_t* ctx);

void vtable_remove(vtable_context_t* ctx);

void vtable_detour(vtable_context_t* ctx, void* dest, void** old, ulong_t i);

void vtable_detour_ex(void*** obj, void* dest, void** old, ulong_t i);

void vtable_free(vtable_context_t* ctx);

#ifdef __cplusplus
	}
#endif

#endif // VTABLE_HOOK_H_