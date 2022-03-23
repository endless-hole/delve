#include "server_shared.h"
#include "socket.h"
#include "log.h"
#include "process/process.h"

extern uint64_t handle_incoming_packet( const Packet& packet );
extern bool     complete_request( SOCKET client_connection, uint64_t result );


static SOCKET create_listen_socket()
{
    SOCKADDR_IN address{};

    address.sin_family = AF_INET;
    address.sin_port = htons( server_port );

    const auto listen_socket = socket_listen( AF_INET, SOCK_STREAM, 0 );
    if( listen_socket == INVALID_SOCKET )
    {
        log_error( "Failed to create listen socket." );
        return INVALID_SOCKET;
    }

    if( bind( listen_socket, ( SOCKADDR* )&address, sizeof( address ) ) == SOCKET_ERROR )
    {
        log_error( "Failed to bind socket." );

        closesocket( listen_socket );
        return INVALID_SOCKET;
    }

    if( listen( listen_socket, 10 ) == SOCKET_ERROR )
    {
        log_error( "Failed to set socket mode to listening.\n" );

        closesocket( listen_socket );
        return INVALID_SOCKET;
    }

    return listen_socket;
}

// Connection handling thread.
static void NTAPI connection_thread( void* connection_socket )
{
    const auto client_connection = SOCKET( ULONG_PTR( connection_socket ) );
    log_debug( "New connection.\n" );

    Packet packet{};
    while( true )
    {
        const auto result = recv( client_connection, ( void* )&packet, sizeof( packet ), 0 );

        if( result <= 0 )
        {
            log_error( "failed recv! %X\n", result );
            break;
        }

        if( result < sizeof( PacketHeader ) )
            continue;

        if( packet.header.magic != packet_magic )
            continue;

        const auto packet_result = handle_incoming_packet( packet );
        if( !complete_request( client_connection, packet_result ) )
        {
            log_error( "failed complete_request!\n" );
            break;
        }

        if( delve::process::kill_server )
        {
            log_debug( "kill server called!\n" );
            break;
        }
    }

    log_debug( "Connection closed.\n" );
    closesocket( client_connection );
}

void unload_driver()
{
    ExFreePool( ( void* )delve::process::driver_base );
}


// Main server thread.
void NTAPI init_server( void* )
{
    auto status = KsInitialize();
    if( !NT_SUCCESS( status ) )
    {
        log_error( "Failed to initialize KSOCKET. Status code: %X.\n", status );
        return;
    }

    const auto listen_socket = create_listen_socket();
    if( listen_socket == INVALID_SOCKET )
    {
        log_error( "Failed to initialize listening socket.\n" );

        KsDestroy();
        return;
    }

    log_debug( "Listening on port %d.\n", server_port );


    sockaddr  socket_addr{};
    socklen_t socket_length{};

    const auto client_connection = accept( listen_socket, &socket_addr, &socket_length );
    if( client_connection == INVALID_SOCKET )
    {
        log_error( "Failed to accept client connection.\n" );
        return;
    }

    connection_thread( ( void* )client_connection );

    log_debug( "Closing down server\n" );

    closesocket( listen_socket );

	/*KeEnterGuardedRegion();

	PWORK_QUEUE_ITEM work_item = ( PWORK_QUEUE_ITEM )ExAllocatePool( NonPagedPool, sizeof( WORK_QUEUE_ITEM ) );

	ExInitializeWorkItem( work_item, (PWORKER_THREAD_ROUTINE)unload_driver, work_item );

	ExQueueWorkItem( work_item, DelayedWorkQueue );

	KeLeaveGuardedRegion();*/

    
}