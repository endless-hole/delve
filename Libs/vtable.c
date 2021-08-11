/*******************************************************************************
	HACK THE PLANETSIDE
	vtable.c
*******************************************************************************/
#include "..\Libs\types.h"
#include "..\Libs\mm.h"
#include "vtable.h"

#define PAGE_SIZE 0x1000

#if defined(_WIN64)
#define PROC_TYPE 64
#define MAX_USER_SPACE 0x7ffffffffff
#else
#define PROC_TYPE 32
#define MAX_USER_SPACE 0x7fffffff
#endif

void vtable_create(vtable_context_t* ctx)
{
	ulong_t i = 0;

	/* save the original vtable pointer */
	ctx->old = *ctx->obj;
	ctx->curr = NULL;
	ctx->base_alloc = NULL;

	/* count the virtual functions */
	while (true)
	{
		if (!mm_validcodeptr(ctx->old[i]))
		{
			break;
		}

		i++;
	}

	ctx->size = i;

	if (ctx->size > 0)
	{
		ulong_t delta = 0;

		/* alloc 10 pointers more */
		ctx->base_alloc = (void **)mm_halloc((i + delta) * sizeof(void *));
		ctx->curr = ctx->base_alloc + delta;

		/* copy the old vtable */
		mm_cpy(ctx->base_alloc, ctx->old - delta, (i + delta) * sizeof(void *));
	}
}

void
vtable_swap(vtable_context_t* ctx)
{
	/* sanity check */
	if (ctx->curr != NULL)
	{
		/* replace the vtable pointer */
		*ctx->obj = ctx->curr;
	}
}

void
vtable_remove(vtable_context_t* ctx)
{
	/* replace vtable with the original vtable */
	*ctx->obj = ctx->old;
	vtable_free(ctx);
}

void
vtable_free(vtable_context_t* ctx)
{
	if(ctx->curr != NULL)
	{
		mm_hfree(ctx->base_alloc);
        ctx->base_alloc = NULL;
        ctx->curr = NULL;
	}
}

void
vtable_detour(vtable_context_t* ctx, void* dest, void** old, ulong_t i)
{
	/* sanity check */
	if (ctx->curr == NULL || i >= ctx->size)
	{
		*old = NULL;
		return;
	}

	*old = ctx->old[i];
	ctx->curr[i] = dest;
}