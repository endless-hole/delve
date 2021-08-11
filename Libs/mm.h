#ifndef MM_H_
#define MM_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t mm_rdtsc(void);
uint32_t mm_getrn(uint32_t* seed);

void* mm_halloc(size_t s);
void mm_hfree(void* m);
void* mm_hrealloc(void* m, size_t s);

void mm_cpy(void* dst, void* src, size_t size);
void mm_set(void* dst, uint32_t data, size_t size);

bool_t mm_validptr(void* mem);
bool_t mm_validcodeptr(void* mem);

#ifdef __cplusplus
}
#endif

#endif