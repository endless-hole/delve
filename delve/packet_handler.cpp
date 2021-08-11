#include "ntifs.h"
#include "server_shared.h"
#include "socket.h"
#include "imports.h"
#include "log.h"
#include "process/process.h"

#pragma warning(disable : 4505)

//=================================================================================================================

uint64_t RDrvGetModuleEntry
(
    PEPROCESS      Process,
    UNICODE_STRING module_name
)
{
    if( !Process ) return ( uint64_t )STATUS_INVALID_PARAMETER_1;
    PPEB peb = PsGetProcessPeb( Process );

    if( !peb )
    {
        log_error( "Error pPeb not found\n" );
        return 0;
    }
    KAPC_STATE state;
    KeStackAttachProcess( Process, &state );
    PPEB_LDR_DATA ldr = peb->Ldr;

    if( !ldr )
    {
        log_error( "Error pLdr not found\n" );
        KeUnstackDetachProcess( &state );
        return 0; // failed
    }

    for( PLIST_ENTRY listEntry = ( PLIST_ENTRY )ldr->ModuleListLoadOrder.Flink;
         listEntry != &ldr->ModuleListLoadOrder;
         listEntry = ( PLIST_ENTRY )listEntry->Flink )
    {
        PLDR_DATA_TABLE_ENTRY ldrEntry = CONTAINING_RECORD( listEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList );
        if( RtlCompareUnicodeString( &ldrEntry->BaseDllName, &module_name, TRUE ) == 0 )
        {
            ULONG64 baseAddr = ( ULONG64 )ldrEntry->DllBase;
            KeUnstackDetachProcess( &state );
            return baseAddr;
        }
    }

    log_error( "Error exiting funcion nothing was found found\n" );
    KeUnstackDetachProcess( &state );

    return 0;
}

//=================================================================================================================

static uint64_t handle_set_target( const PacketSetTarget& packet )
{
    PEPROCESS tgt_process = nullptr;
    PEPROCESS own_process = nullptr;
    NTSTATUS  status      = -1;

    ulong_t tgt_pid = packet.tgt_process_id;
    ulong_t own_pid = packet.own_process_id;

    status = PsLookupProcessByProcessId( HANDLE( tgt_pid ), &tgt_process );

    if( !NT_SUCCESS( status ) )
    {
        log_error( "failed to find target pid\n" );
        return status;
    }


    status = PsLookupProcessByProcessId( HANDLE( own_pid ), &own_process );

    if( !NT_SUCCESS( status ) )
    {
        log_error( "failed to find source pid\n" );
        return status;
    }

    UNICODE_STRING module_name;
    RtlInitUnicodeString( &module_name, packet.process_name );
    const auto tgt_base_address = RDrvGetModuleEntry( tgt_process, module_name );

    if( !tgt_base_address )
        return status;

    delve::process::tgt_process      = tgt_process;
    delve::process::tgt_pid          = tgt_pid;
    delve::process::tgt_base_address = tgt_base_address;
    delve::process::own_process      = own_process;
    delve::process::own_pid          = own_pid;

    delve::process::target_set = true;

    log_debug( "target set to:          %ls\n", packet.process_name );
    log_debug( "target pid:             %016llx\n", tgt_pid );
    log_debug( "target base address:    %016llx\n", tgt_base_address );

    return status;
}

//=================================================================================================================

static uint64_t handle_copy_memory( const PacketCopyMemory& packet )
{
    if( !delve::process::target_set )
        return 0;

    SIZE_T   return_size = 0;
    NTSTATUS status      = -1;

    log_debug( "memory mode: %d\n", packet.mode );

    if( packet.mode == MemoryMode::read )
    {
        log_debug( "reading address %016llx\n", packet.src_address );

        status = MmCopyVirtualMemory
        (
            delve::process::tgt_process,
            ( void* )packet.src_address,
            delve::process::own_process,
            ( void* )packet.dest_address,
            packet.size,
            UserMode,
            &return_size
        );
    }
    else if( packet.mode == MemoryMode::write )
    {
        log_debug( "writing address %016llx\n", packet.src_address );

        status = MmCopyVirtualMemory
        (
            delve::process::own_process,
            ( void* )packet.dest_address,
            delve::process::tgt_process,
            ( void* )packet.src_address,
            packet.size,
            UserMode,
            &return_size
        );
    }

    log_debug( "status %016llx\n", status );

    return uint64_t( status );
}

//=================================================================================================================

static uint64_t handle_alloc_memory( const PacketAllocMemory& packet )
{
    void*    address = 0;
    size_t   size    = packet.size;
    ulong_t  protect = packet.protect;

    if( !delve::process::target_set )
        return 0;

    KeAttachProcess( delve::process::tgt_process );

    ZwAllocateVirtualMemory
    (
        NtCurrentProcess(),
        &address,
        0,
        &size,
        MEM_COMMIT,
        protect
    );

    KeDetachProcess();

    log_debug( "allocated memory %016llx\n", address );

    return ( uint64_t )address;
}

//=================================================================================================================

static uint64_t handle_protect_memory( const PacketProtectMemory& packet )
{
    void*    address = ( void* )packet.address;
    size_t   size    = packet.size;
    ulong_t  protect = packet.protect;
    ulong_t  old_protect = 0;

    NTSTATUS status = -1;

    if( !address || !size || !delve::process::target_set )
        return status;

    KeAttachProcess( delve::process::tgt_process );

    status = ZwProtectVirtualMemory
    (
        NtCurrentProcess(),
        &address,
        &size,
        protect,
        &old_protect
    );

    KeDetachProcess();

    log_debug( "protecting memory %016llx\n", address );

    return old_protect;
}

//=================================================================================================================

static uint64_t handle_free_memory( const PacketFreeMemory& packet )
{
    uint64_t address = packet.address;

    NTSTATUS status = -1;

    if( !address || !delve::process::target_set )
        return status;

    KeAttachProcess( delve::process::tgt_process );
    status = ZwFreeVirtualMemory( delve::process::tgt_process, ( void** )&address, nullptr, MEM_RELEASE );
    KeDetachProcess();

    log_debug( "freeing memory: %016llx\n", address );

    return status;
}

//=================================================================================================================

static uint64_t handle_get_peb( const PacketGetPeb& packet )
{
    uint64_t dummy = packet.dummy;

    if(dummy != 1)
        return 0;

    if(!delve::process::tgt_process)
    {
        log_error( "EPROCESS not set\n" );
        return 0;
    }

    PPEB peb = PsGetProcessPeb( delve::process::tgt_process );

    if( !peb )
    {
        log_error( "PEB not found\n" );
        return 0;
    }

    log_debug( "returning PEB: %016llx\n", peb );

    return (uint64_t)peb;
}

static uint64_t handle_get_is_alive( const PacketIsAlive& packet )
{
    uint64_t check = packet.check;

    if( check == is_alive )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//=================================================================================================================

// Send completion packet.
bool complete_request( const SOCKET client_connection, const uint64_t result )
{
    Packet packet{};

    packet.header.magic          = packet_magic;
    packet.header.type           = PacketType::packet_completed;
    packet.data.completed.result = result;

    return send( client_connection, &packet, sizeof( packet ), 0 ) != SOCKET_ERROR;
}

//=================================================================================================================

uint64_t handle_incoming_packet( const Packet& packet )
{
    switch( packet.header.type )
    {
    case PacketType::packet_copy_memory:
        return handle_copy_memory( packet.data.copy_memory );

    case PacketType::packet_alloc_memory:
        return handle_alloc_memory( packet.data.alloc_memory );

    case PacketType::packet_free_memory:
        return handle_free_memory( packet.data.free_memory );

    case PacketType::packet_protect_memory:
        return handle_protect_memory( packet.data.protect_memory );

    case PacketType::packet_set_target:
        return handle_set_target( packet.data.set_target );

    case PacketType::packet_get_peb:
        return handle_get_peb( packet.data.get_peb );

    case PacketType::packet_is_alive:
        return handle_get_is_alive( packet.data.is_alive );

    default:
        break;
    }

    return uint64_t( STATUS_NOT_IMPLEMENTED );
}

//=================================================================================================================
