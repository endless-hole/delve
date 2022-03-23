#pragma once

namespace util
{
	namespace memory
	{
		void* get_module_list();
		void* get_module_base( const char* name );
	}
}