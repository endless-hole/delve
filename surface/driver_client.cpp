#include "driver_client.h"
#include "WinSystemInfoQuery.h"

#include "../Libs/str.h"

#pragma comment(lib, "Ws2_32")

Client::Client() { this->init(); }

Client::~Client() { this->quit(); }

void Client::init() { WSAStartup( MAKEWORD( 2, 2 ), &_wsa_data ); }

void Client::quit() { WSACleanup(); }

SOCKET Client::setup_socket()
{
    SOCKADDR_IN address{};

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl( server_ip );
    address.sin_port        = htons( server_port );

    const auto connection = socket( AF_INET, SOCK_STREAM, 0 );

    if( connection == INVALID_SOCKET )
        return INVALID_SOCKET;

    if( connect( connection, ( SOCKADDR* )&address, sizeof( address ) ) == SOCKET_ERROR )
    {
        closesocket( connection );
        return INVALID_SOCKET;
    }

    return connection;
}

void Client::disconnect_socket( const SOCKET connection ) { closesocket( connection ); }

bool Client::send_packet( const SOCKET conn, const Packet& packet, uint64_t& out )
{
    Packet return_packet{};

    if( send( conn, ( const char* )&packet, sizeof( Packet ), 0 ) == SOCKET_ERROR ) { return false; }

    const auto result = recv( conn, ( char* )&return_packet, sizeof( Packet ), 0 );

    if( result < sizeof( PacketHeader )
        || return_packet.header.magic != packet_magic
        || return_packet.header.type != PacketType::packet_completed ) { return false; }

    out = return_packet.data.completed.result;
}

Driver::Driver( std::shared_ptr< Client > client, SOCKET connection )
{
    _client     = client;
    _connection = connection;
}

bool Driver::set_target( const wchar_t* process_name )
{
    NTSTATUS                        status;
    SystemProcessInformationExQuery query;

    if( !NT_SUCCESS( status = query.exec() ) ) { return false; }

    auto pid = query.get_proc_id( process_name );

    _pid = pid;

    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type  = PacketType::packet_set_target;

    auto& data = packet.data.set_target;

    data.tgt_process_id = ( ulong_t )pid;
    data.own_process_id = GetCurrentProcessId();

    str_ucpy( data.process_name, (wchar_t*)process_name );

    uint64_t result = 0;

    if( _client->send_packet( _connection, packet, result ) )
        return NT_SUCCESS( result );

    return false;
}

uint64_t Driver::get_peb()
{
    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type = PacketType::packet_get_peb;

    auto& data = packet.data.get_peb;

    data.dummy = 1;

    uint64_t result = 0;

    _client->send_packet( _connection, packet, result );

    return result;
}

uint64_t Driver::alloc( const size_t size, const ulong_t protect )
{
    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type  = PacketType::packet_alloc_memory;

    auto& data   = packet.data.alloc_memory;
    data.size    = size;
    data.protect = protect;

    uint64_t result = 0;

    _client->send_packet( _connection, packet, result );

    return result;
}

uint64_t Driver::protect( const uint64_t address, const size_t size, const ulong_t protect )
{
    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type  = PacketType::packet_protect_memory;

    auto& data   = packet.data.protect_memory;
    data.address = address;
    data.size    = size;
    data.protect = protect;

    uint64_t result = 0;

    _client->send_packet( _connection, packet, result );

    return result;
}

uint64_t Driver::free( const uint64_t address )
{
    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type  = PacketType::packet_free_memory;

    auto& data   = packet.data.free_memory;
    data.address = address;

    uint64_t result = 0;

    _client->send_packet( _connection, packet, result );

    return result;
}

uint64_t Driver::read_memory( uint64_t address, void* buffer, size_t size )
{
    return this->copy_memory( address, ( uint64_t )buffer, size, MemoryMode::read );
}

uint64_t Driver::write_memory( uint64_t address, void* buffer, size_t size )
{
    return this->copy_memory( address, ( uint64_t )buffer, size, MemoryMode::write );
}

uint64_t Driver::copy_memory
(
    const uint64_t   src_address,
    const uint64_t   dest_address,
    const size_t     size,
    const MemoryMode mode
)
{
    Packet packet{};

    packet.header.magic = packet_magic;
    packet.header.type  = PacketType::packet_copy_memory;

    auto& data        = packet.data.copy_memory;
    data.src_address  = src_address;
    data.dest_address = dest_address;
    data.size         = size;
    data.mode         = mode;

    uint64_t result = 0;

    _client->send_packet( _connection, packet, result );

    return result;
}
