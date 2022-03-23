#pragma once
#include <stdint.h>

namespace util
{
	namespace pattern
	{
		uintptr_t find( uintptr_t start, size_t size, char* pattern );
		uintptr_t find_image( void* image, char* pattern );
		uintptr_t find_relative( uintptr_t start, size_t size, char* pattern, int32_t offset );
		uintptr_t find_image_relative( void* image, char* pattern, int32_t offset );
		uintptr_t relative_to_absolute( uintptr_t src, int32_t offset );
	}
}
