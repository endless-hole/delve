#include "pattern.hpp"
#include "nt.hpp"
#include "../log.h"

#define in_range(x,a,b)  (x >= a && x <= b) 
#define get_bits( x )    (in_range((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (in_range(x,'0','9') ? x - '0' : 0))
#define get_byte( x )    (get_bits(x[0]) << 4 | get_bits(x[1]))

#define section_cmp( sect, name ) ( memcmp( sect->Name, name, ( sizeof( name ) - 1 ) ) == 0 )

uintptr_t util::pattern::find( uintptr_t start, size_t size, char* pattern )
{
	uintptr_t match = 0;
	uintptr_t end = start + size;
	char* pat = pattern;

	for( uintptr_t cur = start; cur < end; ++cur )
	{
		if( !*pat )
		{
			return match;
		}

		if( *( uint8_t* )pat == '\?' || *( uint8_t* )cur == get_byte( pat ) )
		{
			if( !match ) match = cur;
			if( !pat[ 2 ] )return match;
			if( *( uint16_t* )pat == '\?\?' || *( uint8_t* )pat != '\?' ) pat += 3;
			else pat += 2;
		}
		else
		{
			pat = pattern;
			match = 0;
		}
	}

	return 0;
}

uintptr_t util::pattern::find_image( void* image, char* pattern )
{
	uintptr_t match = 0;

	auto dos_header = ( PIMAGE_DOS_HEADER )image;
	auto nt_headers = ( PIMAGE_NT_HEADERS )( ( uintptr_t )image + dos_header->e_lfanew );

	auto* sections = IMAGE_FIRST_SECTION( nt_headers );

	for( int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++ )
	{
		auto* section = &sections[ i ];

		if( section_cmp( section, "PAGE" ) || section_cmp( section, ".text" ) )
		{
			match = find(
				( uintptr_t )image + section->PointerToRawData, section->Misc.VirtualSize, pattern );

			if( match ) break;
		}
	}

	return match;
}

uintptr_t util::pattern::find_relative( uintptr_t start, size_t size, char* pattern, int32_t offset )
{
	uintptr_t data = find( start, size, pattern );

	if( data == 0 )
		return 0;

	return relative_to_absolute( data, offset );
}

uintptr_t util::pattern::find_image_relative( void* image, char* pattern, int32_t offset )
{
	uintptr_t match = 0;

	auto dos_header = ( PIMAGE_DOS_HEADER )image;
	auto nt_headers = ( PIMAGE_NT_HEADERS )( ( uintptr_t )image + dos_header->e_lfanew );

	auto* sections = IMAGE_FIRST_SECTION( nt_headers );

	for( int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++ )
	{
		auto* section = &sections[ i ];

		if( section_cmp( section, "PAGE" ) || section_cmp( section, ".text" ) )
		{
			match = find_relative(
				( uintptr_t )image + section->PointerToRawData, section->Misc.VirtualSize, pattern, offset );

			if( match ) break;
		}
	}

	return match;
}

uintptr_t util::pattern::relative_to_absolute( uintptr_t src, int32_t offset )
{
	if( src == 0 ) return 0;

	return src + offset + sizeof( int32_t ) + *( int32_t* )( src + offset );
}