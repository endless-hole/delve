/*******************************************************************************
	HACK THE PLANETSIDE
	Libs - pe.h
*******************************************************************************/
#ifndef _PE_H_
#define _PE_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "types.h"
#include "proc.h"
#include "ntdll\ntdll.h"

/****************************************************************************

	exported functions -- generic

****************************************************************************/
void* remote_va_from_exp_name(void* mod_base, 
	void* remote_base, char* exp_name);

ulong_t pe_get_magic(void* mod_base);

void* pe_get_proc_address(void* mod_base, 
	char* func_name, ulong_t ord, bool_t file_aligned);

void pe_relocate(void* mod_base, void* new_base, bool_t file_aligned);

PIMAGE_FILE_HEADER pe_get_file_header(void* mod_base);

PIMAGE_SECTION_HEADER pe_get_section_header(void* mod_base, 
	ulong_t i, ulong_t* num_of_sec);

uint32_t pe_rva_to_raw(void* mod_base, ulong_t rva);

uint32_t pe_raw_to_rva(void* mod_base, ulong_t raw);

PIMAGE_SECTION_HEADER pe_get_section_exec(void* mod_base);

PIMAGE_SECTION_HEADER pe_add_section(void* mod_base, char* name, ulong_t size);

bool_t pe_imports_fix(handle_t proc_handle, 
	void* mod_base, bool_t file_aligned);

/****************************************************************************

	exported functions -- PE32

****************************************************************************/

PIMAGE_OPTIONAL_HEADER32 pe32_get_optional_header(void* mod_base);

uint32_t pe32_get_entry_point(void* mod_base);

uint32_t pe32_get_image_base(void* mod_base);

/****************************************************************************

	exported functions -- PE32+ 

****************************************************************************/

PIMAGE_OPTIONAL_HEADER64 pe64_get_optional_header(void* mod_base);

uint32_t pe64_get_entry_point(void* mod_base);

uint64_t pe64_get_image_base(void* mod_base);

uint32_t pe64_get_image_size(void* mod_base);

void* proc_get_image_base(void);

#ifdef __cplusplus
	}
#endif

#endif //_PE_H_