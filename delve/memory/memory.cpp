#include "memory.hpp"
#include "../log.h"
#include "nt.hpp"

#define SystemModuleInformation 0x0B
EXTERN_C __kernel_entry NTSTATUS ZwQuerySystemInformation( IN ULONG SystemInformationClass, OUT VOID* SystemInformation, IN ULONG SystemInformationLength, OUT ULONG* ReturnLength );


void* util::memory::get_module_list()
{
	ULONG size = 0;

	ZwQuerySystemInformation( SystemModuleInformation, nullptr, NULL, &size );

	size += ( 10 * 1024 );

	void* module_list = ExAllocatePool( NonPagedPool, size );

	NTSTATUS status = ZwQuerySystemInformation( SystemModuleInformation, module_list, size, &size );

	if( NT_SUCCESS( status ) == false )
	{
		log_error( "failed to query system module list with error code: 0x%lX\n", status );
		if( module_list ) ExFreePool( module_list );
		return nullptr;
	}

	if( module_list == nullptr )
	{
		log_error( "module list is empty\n" );
		return nullptr;
	}

	return module_list;
}

void* util::memory::get_module_base( const char* name )
{
	void* ret_address = 0;

	void* module_list = get_module_list();

	auto modules = ( RTL_PROCESS_MODULES* )module_list;

	for( int i = 0; i < modules->NumberOfModules; i++ )
	{
		auto module = &modules->Modules[ i ];

		if( strstr( module->FullPathName, "dump_" ) )
			continue;

		if( strstr( module->FullPathName, name ) )
		{
			ret_address = module->ImageBase;
			log_debug( "found module %s : 0x%llX\n", module->FullPathName, ret_address );
			break;
		}
	}

	ExFreePool( module_list );
	return ret_address;
}
