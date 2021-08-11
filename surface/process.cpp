#include "process.h"
#include "../Libs/proc.h"


Process::Process( const wchar_t* proc_name )
{
    auto client = std::make_shared< Client >();

    const auto connection = client->setup_socket();

    _driver = std::make_shared< Driver >( client, connection );

    _driver->set_target( proc_name );

    _pid = _driver->get_target_pid();

    _handle = nullptr;
}

bool_t Process::open_handle( const ulong_t flags )
{
    handle_t handle = proc_open(_pid, flags);

    if(!handle)
        return false;

    _handle = handle;
    return true;
}

void Process::close_handle() const
{
    if(_handle)
        proc_close(_handle);
}

void* Process::alloc( const size_t size, const ulong_t flags ) const
{
    const uint64_t ret = _driver->alloc( size, flags );

    if(!ret)
        return nullptr;

    return (void*)ret;
}

bool_t Process::free( void* ptr ) const
{
    const uint64_t ret = _driver->free( (uint64_t)ptr );

    if(!ret)
        return false;

    return true;
}

bool_t Process::protect( void* addr, const size_t size, const ulong_t flags ) const
{
    return _driver->protect( (uint64_t)addr, size, flags );
}

bool_t Process::write_memory( void* addr, void* buffer, const size_t size ) const
{
    const uint64_t ret = _driver->write_memory( reinterpret_cast< uint64_t >( addr ), buffer, size );

    if(NT_SUCCESS( ret ))
        return true;

    return false;
}

bool_t Process::read_memory( void* addr, void* buffer, const size_t size ) const
{
    const uint64_t ret = _driver->read_memory( reinterpret_cast< uint64_t >( addr ), buffer, size );

    if(NT_SUCCESS( ret ))
        return true;

    return false;
}

void Process::read_pages( void* addr, void* buffer, size_t size ) const
{
    size_t total_bytes = 0;

    while (total_bytes < size)
	{
        _driver->read_memory( (uint64_t)addr + total_bytes, (uint8_t*)buffer + total_bytes, 0x1000 );

        total_bytes += 0x1000;
    }
}

uint64_t Process::get_peb() const
{
    return _driver->get_peb();
}

void* Process::get_module( char* name, size_t* size_of_image, ulong_t proc_type ) const
{
    if(!_handle)
        return nullptr;

    return module_get_base(_handle, name, size_of_image, proc_type);
}

void* Process::get_proc_address( char* mod_name, char* func_name, ulong_t ord, ulong_t code_type ) const
{
    if(!_handle)
        return nullptr;

    return remote_get_proc_address(_handle, mod_name, func_name, ord, code_type);
}