#include "vad_routines.h"


#include "../process/process.h"
#include "memory.hpp"
#include "pattern.hpp"

#define page_offset( x ) ( x << 12 )


bool view_process_page_pte( _EPROCESS* eproc, uintptr_t address )
{
	log_debug( "\n\n================================================\n\n" );

	using get_virtual_for_physical_fn = uintptr_t ( NTAPI* )( uintptr_t );

	static get_virtual_for_physical_fn get_virtual_for_physical = nullptr;

	if( get_virtual_for_physical == nullptr )
	{
		if( delve::process::ntoskrnl == nullptr )
		{
			delve::process::ntoskrnl = util::memory::get_module_base( "ntoskrnl.exe" );
		}

		if( delve::process::ntoskrnl == nullptr )
		{
			log_error( "failed to find ntoskrnl.exe\n" );
			return false;
		}

		auto fn_sig = util::pattern::find_image( delve::process::ntoskrnl, ( char* )"48 8B C1 48 C1 E8 0C 48 8D 14 40" );

		if( fn_sig == 0 )
		{
			log_error( "failed to find MmGetVirtualForPhysical\n" );
			return false;
		}
		else
		{
			log_success( "found MmGetVirtualForPhysicalL 0x%p\n", fn_sig );
		}

		get_virtual_for_physical = ( get_virtual_for_physical_fn )fn_sig;
	}


	if( !eproc )
		return false;

	uintptr_t dir_base = eproc->Pcb.DirectoryTableBase;

	if( dir_base == 0 )
		dir_base = eproc->Pcb.UserDirectoryTableBase;

	if( dir_base == 0 )
		return false;

	virt_addr_t addr_t{ (void*)address };
	cr3_t cr3{ dir_base };

	log_debug( "EPROCESS:           0x%p\n", eproc );
	log_debug( "DirectoryTableBase: 0x%p\n", dir_base );
	log_debug( "Virtual Address:    0x%p\n", address );

	pml4e_t* pml4e = ( pml4e_t* )get_virtual_for_physical( page_offset( cr3.dirbase ) ) + addr_t.pml4_index;


	if( MmIsAddressValid( pml4e ) == false )
	{
		log_error( "pml4e: 0x%p\n", pml4e );
		log_error( "pml4e->value: 0x%p\n", pml4e->value );
		log_error( "could not find pml4e address for 0x%p\n", address );
		return false;
	}
	else
	{
		log_success( "found pml4e for address: 0x%p\n", address );
	}

	pdpte_t* pdpte = ( pdpte_t* )get_virtual_for_physical( page_offset( pml4e->pfn ) ) + addr_t.pdpt_index;

	if( MmIsAddressValid( pdpte ) == false )
	{
		log_error( "pdpte: 0x%p\n", pdpte );
		log_error( "could not find pdpte address for 0x%p\n", address );
		return false;
	}
	else
	{
		log_success( "found pdpte for address: 0x%p\n", address );
	}

	if( pdpte->page_size )
	{
		log_success( "pdpte is a large page\n" );
		return true;
	}

	pde_t* pde = ( pde_t* )get_virtual_for_physical( page_offset( pdpte->pfn ) ) + addr_t.pd_index;

	if( MmIsAddressValid( pde ) == false )
	{
		log_error( "pde: 0x%p\n", pde );
		log_error( "could not find pde address for 0x%p\n", address );
		return false;
	}
	else
	{
		log_success( "found pde for address: 0x%p\n", address );
	}

	if( pde->page_size )
	{
		log_success( "pde is a large page\n" );
		return true;
	}

	pte_t* pte = ( pte_t* )get_virtual_for_physical( page_offset( pde->pfn ) ) + addr_t.pt_index;

	if( MmIsAddressValid( pte ) == false )
	{
		log_error( "pte: 0x%p\n", pte );
		log_error( "could not find pte address for 0x%p\n", address );
		return false;
	}
	else
	{
		log_success( "found pte for address: 0x%p\n", address );
	}

	pte->nx = 0;

	return true;
}

_MMPTE* get_pte_for_va( uintptr_t address )
{
	using get_pte_fn = _MMPTE * ( NTAPI* )( uintptr_t );

	static get_pte_fn get_pte = nullptr;
	static get_pte_fn get_pde = nullptr;

	if( get_pte == nullptr )
	{
		if( delve::process::ntoskrnl == nullptr )
		{
			delve::process::ntoskrnl = util::memory::get_module_base( "ntoskrnl.exe" );
		}

		if( delve::process::ntoskrnl == nullptr )
		{
			log_error( "failed to find ntoskrnl.exe\n" );
			return nullptr;
		}

		auto pte_fn = util::pattern::find_image_relative( delve::process::ntoskrnl, ( char* )"E8 ? ? ? ? 44 3B CA", 1 );

		if( pte_fn == 0 )
		{
			log_error( "failed to find MiGetPteAddress\n" );
			return nullptr;
		}

		log_success( "found MiGetPteAddress: 0x%p", pte_fn );

		get_pte = ( get_pte_fn )pte_fn;
	}

	if( get_pde == nullptr )
	{
		if( delve::process::ntoskrnl == nullptr )
		{
			delve::process::ntoskrnl = util::memory::get_module_base( "ntoskrnl.exe" );
		}

		if( delve::process::ntoskrnl == nullptr )
		{
			log_error( "failed to find ntoskrnl.exe\n" );
			return nullptr;
		}

		auto pde_fn = util::pattern::find_image_relative( delve::process::ntoskrnl, ( char* )"E8 ? ? ? ? 49 3B C0 75 04", 1 );

		if( pde_fn == 0 )
		{
			log_error( "failed to find MiGetPdeAddress\n" );
			return nullptr;
		}

		log_success( "found MiGetPdeAddress: 0x%p", pde_fn );

		get_pde = ( get_pte_fn )pde_fn;
	}

	auto pde = get_pde( address );

	log_debug( "Found PDE: %llX\n", pde );

	if( pde->u.Hard.LargePage )
		return pde;

	pde = get_pte( address );

	log_debug( "Found PTE: %llX\n", pde );

	return pde;
}

_MMVAD* find_vad( uintptr_t address )
{
	using get_vad_fn = _MMVAD* ( NTAPI* )( uintptr_t );

	static get_vad_fn get_vad = nullptr;

	if( get_vad == nullptr )
	{
		if( delve::process::ntoskrnl == nullptr )
		{
			delve::process::ntoskrnl = util::memory::get_module_base( "ntoskrnl.exe" );
		}

		if(delve::process::ntoskrnl == nullptr )
		{
			log_error( "failed to find ntoskrnl.exe\n" );
			return nullptr;
		}

		auto vad_fn = util::pattern::find_image(delve::process::ntoskrnl, ( char* )"65 48 8B 04 25 ? ? ? ? 4C 8B C1 4C 8B 88" );

		if( vad_fn == 0 )
		{
			log_error( "failed to find MiLocateAddress\n" );
			return nullptr;
		}

		log_success( "found MiLocateAddress: 0x%p\n", vad_fn );

		get_vad = ( get_vad_fn )vad_fn;
	}

	auto ret = get_vad( address );

	return ret;
}

bool spoof_vad
(
	_EPROCESS* eproc,
	uintptr_t address,
	size_t size,
	ulong_t protection
)
{
	UNREFERENCED_PARAMETER( protection );

	const auto vad = find_vad( address );

	if( vad == nullptr )
	{
		log_error( "failed to find VAD\n" );
	}
	else
	{
		log_success( "found VAD: 0x%p\n", vad );
		log_success( "  VAD StartingVpn: 0x%p\n", vad->Core.StartingVpn );
		log_success( "  VAD EndingVpn:   0x%p\n", vad->Core.EndingVpn );
	}

	auto addr_page_aligned = ( uintptr_t )PAGE_ALIGN( address );

	log_debug( "addr: 0x%p\n", addr_page_aligned );
	log_debug( "size: 0x%p\n", addr_page_aligned + size );

	for( uintptr_t addr = addr_page_aligned; addr < addr_page_aligned + size; addr += PAGE_SIZE )
	{
		view_process_page_pte( eproc, addr );
	}

	return true;
}