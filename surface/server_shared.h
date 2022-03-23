#pragma once

#include "../Libs/types.h"

static uint32_t packet_magic = 0x88429401;
static uint32_t server_ip = 0x7F000001;
static uint16_t server_port = 45982;

static uint64_t is_alive = 0x356874;

enum class PacketType
{
    packet_copy_memory = 41,
    packet_alloc_memory = 39,
    packet_free_memory = 69,
    packet_protect_memory = 72,
    packet_set_target = 12,
    packet_get_peb = 24,
    packet_completed = 5,
    packet_is_alive = 0,
    packet_vad_spoof = 82,
    packet_kill = 49
};

struct PacketSetTarget
{
    ulong_t tgt_process_id;
    ulong_t own_process_id;
    wchar_t process_name[ 50 ];
};

enum class MemoryMode
{
    read = 46,
    write = 20
};

struct PacketCopyMemory
{
    uint64_t   src_address;
    uint64_t   dest_address;
    size_t     size;
    MemoryMode mode;
};

struct PacketAllocMemory
{
    size_t  size;
    ulong_t protect;
};

struct PacketProtectMemory
{
    uint64_t address;
    size_t   size;
    ulong_t  protect;
};

struct PacketVadSpoof
{
    uint64_t address;
    size_t   size;
    ulong_t  protect;
};

struct PacketFreeMemory
{
    uint64_t address;
};

struct PacketGetPeb
{
    uint64_t dummy;
};

struct PacketIsAlive
{
    uint64_t check;
};

struct PackedCompleted
{
    uint64_t result;
};

struct PacketKill
{
    uint8_t kill;
};

struct PacketHeader
{
    uint32_t   magic;
    PacketType type;
};

struct Packet
{
    PacketHeader header;

    union
    {
        PacketCopyMemory    copy_memory;
        PacketAllocMemory   alloc_memory;
        PacketFreeMemory    free_memory;
        PacketProtectMemory protect_memory;
        PacketSetTarget     set_target;
        PacketGetPeb        get_peb;
        PacketIsAlive       is_alive;
        PackedCompleted     completed;
        PacketKill          kill;
        PacketVadSpoof      vad_spoof;
    }                       data;
};