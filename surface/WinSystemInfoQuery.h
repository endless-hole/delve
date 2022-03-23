#pragma once

#include "..\Libs\proc.h"
#include <vector>
#include <string>

#pragma comment(lib, "Libs64")
#pragma comment(lib, "ntdll64")

template <class T>
class WinMemoryInfoQuery
{
private:
	bool_t initialized;
	std::vector<uint8_t> buffer;
	MEMORY_INFORMATION_CLASS mi;
public:
	WinMemoryInfoQuery(MEMORY_INFORMATION_CLASS mem_info_class ):
		initialized(false), mi(mem_info_class)
	{}

	NTSTATUS exec( HANDLE proc, uintptr_t base_address)
	{
		NTSTATUS status;
		size_t return_size;

		ulong_t buffer_size = ( ulong_t )max( buffer.size(), 0x1000 );

		do
		{
			buffer.reserve( buffer_size );

			status = NtQueryVirtualMemory( proc, (void*)base_address, mi,
				buffer.data(), buffer_size, &return_size );

			if( status == STATUS_INFO_LENGTH_MISMATCH )
			{
				buffer_size = return_size;
			}

		} while( status == STATUS_INFO_LENGTH_MISMATCH );

		initialized = NT_SUCCESS( status );

		return status;
	}

	T* get()
	{
		return initialized ? ( T* )buffer.data() : nullptr;
	}
};

class BasicMemoryInformation :
	public WinMemoryInfoQuery<MEMORY_BASIC_INFORMATION>
{
public:
	BasicMemoryInformation() :
		WinMemoryInfoQuery<MEMORY_BASIC_INFORMATION>( MEMORY_INFORMATION_CLASS::MemoryBasicInformation )
	{
	}

	void print_info( )
	{
		auto buffer = get();

		LOG( "BaseAddress:       ", std::hex, buffer->BaseAddress );
		LOG( "AllocationBase:    ", std::hex, buffer->AllocationBase );
		LOG( "AllocationProtect: ", std::hex, buffer->AllocationProtect );
		LOG( "PartitionId:       ", std::hex, buffer->PartitionId );
		LOG( "RegionSize:        ", std::hex, buffer->RegionSize );
		LOG( "State:             ", std::hex, buffer->State );
		LOG( "Protect:           ", std::hex, buffer->Protect );
		LOG( "Type:              ", std::hex, buffer->Type );
	}
};


template <class T>
class WinSystemInfoQuery
{
private:
	bool_t initialized;
	std::vector<uint8_t> buffer;
	SYSTEM_INFORMATION_CLASS si;

public:
	WinSystemInfoQuery(SYSTEM_INFORMATION_CLASS sys_info_class) :
		initialized(false), si(sys_info_class) 
	{ }

	NTSTATUS exec()
	{
		NTSTATUS status;
        ULONG return_size;

		ulong_t buffer_size = (ulong_t)max(buffer.size(), 0x1000);

		do
		{
			buffer.reserve(buffer_size);

			status = NtQuerySystemInformation(si, 
				buffer.data(), buffer_size, &return_size);

			if (status == STATUS_INFO_LENGTH_MISMATCH)
			{
				buffer_size = return_size;
			}

		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		initialized = NT_SUCCESS(status);

		return status;
	}

	T* get()
	{
		return initialized ? (T *)buffer.data() : nullptr;
	}
};

class SystemModuleInformationQuery : 
	public WinSystemInfoQuery<RTL_PROCESS_MODULES>
{
public:
	SystemModuleInformationQuery() :
		WinSystemInfoQuery<RTL_PROCESS_MODULES> 
		(SystemModuleInformation) 
	{ }

	bool find_module (
		const std::string& name,
		RTL_PROCESS_MODULE_INFORMATION& info_out)
	{
		auto buffer = get();

		for (uint32_t i = 0; i < buffer->NumberOfModules; i++)
		{
			RTL_PROCESS_MODULE_INFORMATION* info = &buffer->Modules[i];
			UCHAR* file_name = info->FullPathName + info->OffsetToFileName;

			if (name == (char *)file_name)
			{
				info_out = *info;
				return true;
			}
		}

		return false;
	}
};

class SystemPageFileInfomationQuery :
    public WinSystemInfoQuery<SYSTEM_PAGEFILE_INFORMATION>
{
public:
    SystemPageFileInfomationQuery() :
        WinSystemInfoQuery<SYSTEM_PAGEFILE_INFORMATION>
        (SystemPageFileInformation)
    { }

    void output()
    {
        auto buffer = get();
    }
};

class SystemProcessInformationExQuery :
    public WinSystemInfoQuery<SYSTEM_PROCESS_INFORMATION>
{
public:
    SystemProcessInformationExQuery() :
        WinSystemInfoQuery<SYSTEM_PROCESS_INFORMATION>
        (SystemProcessInformation)
    { }

    handle_t get_proc_id(const std::wstring& name)
    {
        auto buffer = get();

        while(buffer->NextEntryOffset)
        {
            if(buffer->ImageName.Buffer == NULL)
            {
                buffer = (PSYSTEM_PROCESS_INFORMATION)(
                    (BYTE*)buffer + buffer->NextEntryOffset);

                continue;
            }

            if(name == buffer->ImageName.Buffer)
            {
                return buffer->UniqueProcessId;
            }

            buffer = (PSYSTEM_PROCESS_INFORMATION)(
                (BYTE*)buffer + buffer->NextEntryOffset);
        }
        
        return nullptr;
    }
};