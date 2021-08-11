#ifndef MISC_H_
#define MISC_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void sleep(ulong_t ms);

void fill_with_random(uint8_t* buffer, size_t size);

uintptr_t find_pattern(char* pattern);

uintptr_t find_pattern_rel32(char* pattern, int32_t offset_offset);

uintptr_t find_pattern_rel32_ex(uintptr_t start, uintptr_t end,
    char* pattern, int32_t offset_offset);
	
uintptr_t find_pattern_ex(uintptr_t start, uintptr_t end, char* pattern);

uintptr_t find_pattern_raw(uintptr_t start, uintptr_t end, 
	uint8_t* pattern_data, size_t pattern_size);
	
uintptr_t find_string_ref(char* str);

uintptr_t find_string_ref_ex(uintptr_t start, uintptr_t end, char* str);

uintptr_t rel32_to_abs64(uintptr_t src, int32_t offset_offset);

#ifdef __cplusplus
	}
#endif

#endif