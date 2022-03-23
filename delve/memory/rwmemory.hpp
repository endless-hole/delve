#pragma once
#include <ntifs.h>
#include <intrin.h>
#include "../stdint.h"

namespace rw
{
	typedef struct _EThread
	{
		typedef struct _KAPC_STATE
		{
			struct _LIST_ENTRY ApcListHead[ 2 ];
		} KAPC_STATE, * PKAPC_STATE, * PRKAPC_STATE;

		char pad_0x00[ 0x98 ]; // KTHREAD->apc_state // Windows 10 | 2004 - 20H1
		KAPC_STATE ApcState;

	} EThread, * PEThread;
	PEThread curr_thread;

	template < typename Type >
	Type Read( Type address, uint32_t offset ) noexcept
	{

		if( address == Type( 0 ) )
		{
			return Type( 0 );
		}

		return *reinterpret_cast< Type* >( uintptr_t( address ) + offset );
	}

	uint64_t get_dirbase( PEPROCESS proc )
	{
		return Read<uint64_t>( uint64_t( proc ), 0x28 );
	}

	void attach_proc( PEPROCESS proc, PRKAPC_STATE apc_state )
	{
		curr_thread = PEThread( KeGetCurrentThread() );
		InitializeListHead( &curr_thread->ApcState.ApcListHead[ KernelMode ] );
		InitializeListHead( &curr_thread->ApcState.ApcListHead[ UserMode ] );

		auto dir_table_base = get_dirbase( proc );
		if( ( dir_table_base & 2 ) != 0 )
			dir_table_base = dir_table_base | 0x8000000000000000u;

		__writegsqword( 0x9000u, dir_table_base );
		__writecr3( dir_table_base );

		auto value = __readcr4();
		if( ( value & 0x20080 ) != 0 )
		{
			__writecr4( value ^ 0x80 );
			__writecr4( value );
		}
		else
		{
			value = __readcr3();
			__writecr3( value );
		}
	}

	int detatch_proc( PRKAPC_STATE apc_state )
	{
		int j = 0;
		for( int i = 0; i < 100; i++ )
			j++;

		RemoveHeadList( &curr_thread->ApcState.ApcListHead[ KernelMode ] );
		RemoveHeadList( &curr_thread->ApcState.ApcListHead[ UserMode ] );

		return j;
	}

	NTSTATUS rw_virtual_memory(
		PEPROCESS proc,
		PVOID dst,
		PVOID src,
		SIZE_T size )
	{
		NTSTATUS status = STATUS_SUCCESS;
		KAPC_STATE apc_state;
		PHYSICAL_ADDRESS src_phys_addr;
		PVOID mapped_io_space;
		PVOID mapped_kva;
		PMDL mdl;
		BOOLEAN use_src_as_user_va;

		if( NT_SUCCESS( status ) && proc )
		{
			use_src_as_user_va = src <= MmHighestUserAddress ? TRUE : FALSE;

			// 2. Get the physical address corresponding to the user virtual memory
			src_phys_addr = MmGetPhysicalAddress( use_src_as_user_va == TRUE ? src : dst );

			if( !src_phys_addr.QuadPart )
			{
				return STATUS_INVALID_ADDRESS;
			}

			// 4. Map an IO space for MDL
			mapped_io_space = MmMapIoSpace( src_phys_addr, size, MmNonCached );
			if( !mapped_io_space )
			{
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			// 5. Allocate MDL
			mdl = IoAllocateMdl( mapped_io_space, ( ULONG )size, FALSE, FALSE, NULL );
			if( !mdl )
			{
				MmUnmapIoSpace( mapped_io_space, size );
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			// 6. Build MDL for non-paged pool
			MmBuildMdlForNonPagedPool( mdl );

			// 7. Map to the KVA
			mapped_kva = MmMapLockedPagesSpecifyCache(
				mdl,
				KernelMode,
				MmNonCached,
				NULL,
				FALSE,
				NormalPagePriority );

			if( !mapped_kva )
			{
				MmUnmapIoSpace( mapped_io_space, size );
				IoFreeMdl( mdl );
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			// 8. copy memory
			memcpy(
				use_src_as_user_va == TRUE ? dst : mapped_kva,
				use_src_as_user_va == TRUE ? mapped_kva : dst,
				size );

			MmUnmapIoSpace( mapped_io_space, size );
			MmUnmapLockedPages( mapped_kva, mdl );
			IoFreeMdl( mdl );
		}

		return status;
	}

	NTSTATUS ReadProcessMemory( PEPROCESS proc, PVOID addr, PVOID buffer, SIZE_T size )
	{
		KAPC_STATE apc_state;
		NTSTATUS ntStatus = STATUS_SUCCESS;

		if( proc )
		{
			attach_proc( proc, &apc_state );
			ntStatus = rw_virtual_memory( proc, buffer, addr, size );
			detatch_proc( &apc_state );
		}

		return ntStatus;
	}

	NTSTATUS WriteProcessMemory( PEPROCESS proc, PVOID addr, PVOID buffer, SIZE_T size )
	{
		KAPC_STATE apc_state;
		NTSTATUS ntStatus = STATUS_SUCCESS;

		if( proc )
		{
			attach_proc( proc, &apc_state );
			ntStatus = rw_virtual_memory( proc, addr, buffer, size );
			detatch_proc( &apc_state );
		}

		return ntStatus;
	}
}