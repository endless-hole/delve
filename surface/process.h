#pragma once
#include <memory>
#include "driver_client.h"
#include "..\Libs\types.h"

class Process
{
public:
    Process( const wchar_t* proc_name );

    bool_t open_handle( ulong_t flags );
    void   close_handle() const;

    void*  alloc( size_t size, ulong_t flags ) const;
    bool_t free( void* ptr ) const;
    bool_t protect( void* addr, size_t size, ulong_t flags ) const;
    bool_t vad_spoof( void* addr, size_t size, ulong_t protect ) const;
    bool_t write_memory( void* addr, void* buffer, size_t size ) const;
    bool_t read_memory( void* addr, void* buffer, size_t size ) const;
    void   read_pages( void* addr, void* buffer, size_t size ) const;
    uint64_t get_peb() const;
    void*  get_module( char* name, size_t* size_of_image, ulong_t proc_type ) const;
    void*  get_proc_address( char* mod_name, char* func_name, ulong_t ord, ulong_t code_type ) const;

    void kill_server();

    handle_t get_handle() const { return _handle; }
    handle_t get_pid() const { return _pid; }
    uint64_t get_dir_base() const { return _dir_base; }

    template< typename T >
    T read( uint64_t address )
    {
        T temp;
        if( !read_memory( address, &temp, sizeof( T ) ) );
        return 0;

        return temp;
    }

    template< typename T >
    bool_t write( uint64_t address, T value )
    {
        return !!write_memory( address, &value, sizeof( T ) );
    }

private:
    handle_t                  _handle;
    std::shared_ptr< Driver > _driver;
    handle_t                  _pid;
    uint64_t                  _dir_base;
    SOCKET                    _socket;
};
