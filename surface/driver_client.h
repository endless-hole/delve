#pragma once
#include <memory>
#include <Windows.h>
#include "server_shared.h"

class Client
{
public:
    Client();
    ~Client();

private:
    void init();
    void quit();

public:
    SOCKET setup_socket();
    void   disconnect_socket( const SOCKET connection );

    bool send_packet( const SOCKET, const Packet& packet, uint64_t& out );

private:
    WSADATA _wsa_data;
};

class Driver
{
public:
    Driver( std::shared_ptr<Client> client, SOCKET connection );

    bool set_target( const wchar_t* process_name );

    uint64_t get_peb();

    uint64_t alloc( size_t size, ulong_t protect );
    uint64_t protect( uint64_t address, size_t size, ulong_t protect );
    uint64_t free( uint64_t address );

    uint64_t read_memory( uint64_t address, void* buffer, size_t size );
    uint64_t write_memory( uint64_t address, void* buffer, size_t size );

    handle_t get_target_pid() const { return _pid; }

private:
    uint64_t copy_memory( uint64_t src_address, uint64_t dest_address, size_t size, MemoryMode mode );

    std::shared_ptr< Client >  _client;
    SOCKET   _connection;
    handle_t _pid;
};
