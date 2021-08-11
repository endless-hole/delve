#include "pe.h"
#include <intrin.h>

void* remote_va_from_exp_name(void* mod_base, 
	void* remote_base, char* exp_name)
{
	void* file_addr = pe_get_proc_address(mod_base, exp_name, 0, true);

	if(!file_addr)
	{
		return NULL;
	}

	(uint8_t *)file_addr -= (uintptr_t)mod_base;

	return (uint8_t *)remote_base + pe_raw_to_rva(mod_base, (ulong_t)file_addr);
}

PIMAGE_FILE_HEADER
pe_get_file_header(void* mod_base)
{
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)mod_base;

	/* get the export directory */
	if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		PIMAGE_NT_HEADERS64 nt;

		nt = (PIMAGE_NT_HEADERS64)((uint8_t *)dos + dos->e_lfanew);

		return &nt->FileHeader;
	}
	else
	{
		PIMAGE_NT_HEADERS32 nt;

		nt = (PIMAGE_NT_HEADERS32)((uint8_t *)dos + dos->e_lfanew);

		return &nt->FileHeader;
	}
}

PIMAGE_OPTIONAL_HEADER32
pe32_get_optional_header(void* mod_base)
{
	PIMAGE_NT_HEADERS32 nt;
	PIMAGE_DOS_HEADER dos;

	/* get the headers */
	dos	= (PIMAGE_DOS_HEADER)mod_base;
	nt = (PIMAGE_NT_HEADERS32)((uint8_t *)dos + dos->e_lfanew);

	return &nt->OptionalHeader;
}

uint32_t
pe32_get_entry_point(void* mod_base)
{
	return pe32_get_optional_header(mod_base)->AddressOfEntryPoint;
}

uint32_t
pe32_get_image_base(void* mod_base)
{
	return pe32_get_optional_header(mod_base)->ImageBase;
}

uint64_t
pe64_get_image_base(void* mod_base)
{
	return pe64_get_optional_header(mod_base)->ImageBase;
}

uint32_t
pe64_get_image_size(void* mod_base)
{
	return pe64_get_optional_header(mod_base)->SizeOfImage;
}

uint32_t
pe64_get_entry_point(void* mod_base)
{
	return pe64_get_optional_header(mod_base)->AddressOfEntryPoint;
}

void* proc_get_image_base(void)
{
	return NtCurrentPeb()->ImageBaseAddress;
}

uint32_t
pe_rva_to_raw(void* mod_base, ulong_t rva)
{
#if 0
	ulong_t num_of_sec;

	PIMAGE_SECTION_HEADER first_hdr;

	/* get the first section header */
	if ((first_hdr = pe_get_section_header(mod_base, 0, &num_of_sec)) == NULL)
	{
		return 0;
	}

	PIMAGE_SECTION_HEADER curr_hdr = first_hdr;

	/* calculate the raw address */
	for (ulong_t i = 0; i < num_of_sec; i++)
	{
		PIMAGE_SECTION_HEADER tmp_hdr = first_hdr;
		PIMAGE_SECTION_HEADER nxt_hdr = NULL;
		size_t sectionSize = curr_hdr->Misc.VirtualSize;

		for(ulong_t i = 0; i < num_of_sec; i++)
		{
			if(tmp_hdr->VirtualAddress > curr_hdr->VirtualAddress)
			{
				if( nxt_hdr == NULL || 
					tmp_hdr->VirtualAddress < nxt_hdr->VirtualAddress )
				{
					nxt_hdr = tmp_hdr;
					sectionSize = nxt_hdr->VirtualAddress - 
						curr_hdr->VirtualAddress;
				}
			}

			tmp_hdr++;
		}

		if ( nxt_hdr == NULL ||
			((curr_hdr->VirtualAddress <= rva) &&
			(curr_hdr->VirtualAddress + sectionSize > rva)) )
		{
			rva -= curr_hdr->VirtualAddress;
			rva += curr_hdr->PointerToRawData;
			return rva;
		}

		curr_hdr++;
	}

	return 0;
#else
	ulong_t num_of_sec;

	PIMAGE_SECTION_HEADER sec_hdr;

	/* get the first section header */
	if ((sec_hdr = pe_get_section_header(mod_base, 0, &num_of_sec)) == NULL)
	{
		return 0;
	}

	/* calculate the raw address */
	for (ulong_t i = 0; i < num_of_sec; i++)
	{
		if ((sec_hdr->VirtualAddress <= rva) &&
			(sec_hdr->VirtualAddress + sec_hdr->Misc.VirtualSize) > rva)
		{
			rva -= sec_hdr->VirtualAddress;
			rva += sec_hdr->PointerToRawData;

			return rva;
		}

		sec_hdr++;
	}

	return 0;
#endif
}

uint32_t
pe_raw_to_rva(void* mod_base, ulong_t raw)
{
	ulong_t num_of_sec;

	PIMAGE_SECTION_HEADER sec_hdr;

	/* get the first section header */
	if ((sec_hdr = pe_get_section_header(mod_base, 0, &num_of_sec)) == NULL)
	{
		return 0;
	}

	/* calculate the raw address */
	for (ulong_t i = 0; i < num_of_sec; i++)
	{
		if ((sec_hdr->PointerToRawData <= raw) &&
			(sec_hdr->PointerToRawData + sec_hdr->SizeOfRawData) >= raw)
		{
			raw -= sec_hdr->PointerToRawData;
			raw += sec_hdr->VirtualAddress;

			return raw;
		}

		sec_hdr++;
	}

	return 0;
}

PIMAGE_SECTION_HEADER
pe_get_section_header(void* mod_base, ulong_t i, ulong_t* num_of_sec)
{
	PIMAGE_DOS_HEADER dos;
	PIMAGE_SECTION_HEADER sec_hdr;
	PIMAGE_FILE_HEADER file_hdr;

	/* get the headers */
	dos = (PIMAGE_DOS_HEADER)mod_base;

	/* get the file header */
	file_hdr = pe_get_file_header(mod_base);

	/* get the number of sections */
	if (i >= file_hdr->NumberOfSections)
	{
		return NULL;
	}

	if (num_of_sec != NULL)
	{
		*num_of_sec = file_hdr->NumberOfSections;
	}

	/* get the first section header */
	if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		sec_hdr = (PIMAGE_SECTION_HEADER)((uint8_t *)dos + dos->e_lfanew +
			offsetof(IMAGE_NT_HEADERS64, OptionalHeader) +
			file_hdr->SizeOfOptionalHeader);
	}
	else
	{
		sec_hdr = (PIMAGE_SECTION_HEADER)((uint8_t *)dos + dos->e_lfanew +
			offsetof(IMAGE_NT_HEADERS32, OptionalHeader) +
			file_hdr->SizeOfOptionalHeader);
	}

	return (sec_hdr + i);
}

ulong_t pe_get_magic(void* mod_base)
{
	PIMAGE_NT_HEADERS nt;
	PIMAGE_DOS_HEADER dos;

	/* get the headers */
	dos = (PIMAGE_DOS_HEADER)mod_base;
	nt = (PIMAGE_NT_HEADERS)((uint8_t *)dos + dos->e_lfanew);

	return nt->OptionalHeader.Magic;
}

void*
pe_get_proc_address(void* mod_base, 
	char* func_name, ulong_t ord, bool_t file_aligned)
{
	PIMAGE_EXPORT_DIRECTORY exp_dir;

	ushort_t* ord_table;
	ulong_t* func_table;
	ulong_t* name_table;

	ulong_t exp_size;
	ulong_t exp_offs;

	void* result = NULL;

	/* IMAGE_DIRECTORY_ENTRY_EXPORT = 0 */
	if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		/* IMAGE_DIRECTORY_ENTRY_EXPORT = 0 */
		exp_offs = pe64_get_optional_header(mod_base)->DataDirectory[0].VirtualAddress;
		exp_size = pe64_get_optional_header(mod_base)->DataDirectory[0].Size;
	}
	else
	{
		/* IMAGE_DIRECTORY_ENTRY_EXPORT = 0 */
		exp_offs = pe32_get_optional_header(mod_base)->DataDirectory[0].VirtualAddress;
		exp_size = pe32_get_optional_header(mod_base)->DataDirectory[0].Size;
	}

	/* convert to file aligned offset */
	if (file_aligned)
	{
		exp_offs = pe_rva_to_raw(mod_base, exp_offs);
	}

	exp_dir = (PIMAGE_EXPORT_DIRECTORY)((uint8_t *)mod_base + exp_offs);

	if (file_aligned)
	{
		ord_table = (ushort_t *)((uint8_t *)mod_base +
			pe_rva_to_raw(mod_base, exp_dir->AddressOfNameOrdinals));

		func_table = (ulong_t *)((uint8_t *)mod_base +
			pe_rva_to_raw(mod_base, exp_dir->AddressOfFunctions));

		name_table = (ulong_t *)((uint8_t *)mod_base +
			pe_rva_to_raw(mod_base, exp_dir->AddressOfNames));
	}
	else
	{
		ord_table = (ushort_t *)((uint8_t *)mod_base +
			exp_dir->AddressOfNameOrdinals);

		func_table = (ulong_t *)((uint8_t *)mod_base +
			exp_dir->AddressOfFunctions);

		name_table = (ulong_t *)((uint8_t *)mod_base +
			exp_dir->AddressOfNames);
	}

	if (func_name == NULL)
	{
		/* import by ordinal */
		ulong_t ord_base = exp_dir->Base;

		if (ord < ord_base ||
			ord > ord_base + exp_dir->NumberOfFunctions)
		{
			return NULL;
		}

		if (file_aligned)
		{
			result = (uint8_t *)mod_base +
				pe_rva_to_raw(mod_base, func_table[ord - ord_base]);
		}
		else
		{
			result = (void *)((uint8_t *)mod_base + func_table[ord - ord_base]);
		}
	}
	else
	{
		/* import by name */
		for (ulong_t i = 0; i < exp_dir->NumberOfNames; i++)
		{
			char* tmp;

			if (file_aligned)
			{
				tmp = (uint8_t *)mod_base +
					pe_rva_to_raw(mod_base, name_table[i]);
			}
			else
			{
				tmp = (char *)((uint8_t *)mod_base + name_table[i]);
			}

			if (!str_acmpi(tmp, func_name))
			{
				if (file_aligned)
				{
					result = (uint8_t *)mod_base +
						pe_rva_to_raw(mod_base, func_table[ord_table[i]]);
				}
				else
				{
					result = (void *)((uint8_t *)mod_base +
						func_table[ord_table[i]]);
				}

				break;
			}
		}
	}

	return result;
}

void
pe_relocate(void* mod_base, void* new_base, bool_t file_aligned)
{
	PIMAGE_BASE_RELOCATION reloc_dir;

	uint8_t* reloc_ptr;
	ulong_t reloc_size;
	ulong_t reloc_offs;
	ulong_t reloc_count;

	ulong_t bytes_read = 0;
	ulong_t offset;

	/* IMAGE_DIRECTORY_ENTRY_BASERELOC = 5 */
	if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		/* IMAGE_DIRECTORY_ENTRY_BASERELOC = 5 */
		reloc_offs = pe64_get_optional_header(mod_base)->DataDirectory[5].VirtualAddress;
		reloc_size = pe64_get_optional_header(mod_base)->DataDirectory[5].Size;
	}
	else
	{
		/* IMAGE_DIRECTORY_ENTRY_BASERELOC = 5 */
		reloc_offs = pe32_get_optional_header(mod_base)->DataDirectory[5].VirtualAddress;
		reloc_size = pe32_get_optional_header(mod_base)->DataDirectory[5].Size;
	}

	/* convert to file aligned offset */
	if (file_aligned)
	{
		reloc_offs = pe_rva_to_raw(mod_base, reloc_offs);
	}

	reloc_dir = (PIMAGE_BASE_RELOCATION)((uint8_t *)mod_base + reloc_offs);

	/* loop through the relocation blocks */
	while (bytes_read < reloc_size)
	{
		reloc_count = reloc_dir->SizeOfBlock;
		reloc_count -= sizeof(IMAGE_BASE_RELOCATION);
		reloc_count /= sizeof(uint16_t);

		uint16_t* tmp = (uint16_t *)((uint8_t *)reloc_dir + sizeof(IMAGE_BASE_RELOCATION));

		for (ulong_t i = 0; i < reloc_count; i++)
		{
			offset = (tmp[i] & 0x0fff) + reloc_dir->VirtualAddress;

			/* convert to file aligned offset */
			if (file_aligned)
			{
				offset = pe_rva_to_raw(mod_base, offset);
			}

			/* get the pointer to the relocation */
			reloc_ptr = (uint8_t *)mod_base + offset;

			//
			// MSB 4 bits: type of relocation
			// IMAGE_REL_BASED_DIR64	-> 64b
			// IMAGE_REL_BASED_HIGHLOW	-> 32b
			// IMAGE_REL_BASED_ABSOLUTE -> only used for padding
			//
			if ((tmp[i] >> 12) == IMAGE_REL_BASED_DIR64)
			{
				// dbg_msg("64, reloc[%03i]: %p\n", i, reloc_ptr);

				uint64_t tmp_ptr = *(uint64_t *)reloc_ptr;

				tmp_ptr += (uint64_t)new_base;
				tmp_ptr -= pe64_get_image_base(mod_base);

				*(uint64_t *)reloc_ptr = tmp_ptr;
			}
			else if ((tmp[i] >> 12) == IMAGE_REL_BASED_HIGHLOW)
			{
				// dbg_msg("32, reloc[%03i]: %p\n", i, reloc_ptr);

				uint32_t tmp_ptr = *(uint32_t *)reloc_ptr;

				tmp_ptr += (uint32_t)new_base;
				tmp_ptr -= pe32_get_image_base(mod_base);

				*(uint32_t *)reloc_ptr = tmp_ptr;
			}
			else
			{
				/* wtf */
			}
		}

		/* update bytes read */
		bytes_read += reloc_dir->SizeOfBlock;

		/* next relocation block */
		reloc_dir = (PIMAGE_BASE_RELOCATION)
			((uint8_t *)reloc_dir + reloc_dir->SizeOfBlock);
	}
}

bool_t
pe_imports_fix(handle_t proc_handle,
	void* mod_base, bool_t file_aligned)
{
	PIMAGE_IMPORT_DESCRIPTOR imp_dir;

	ulong_t imp_size;
	ulong_t imp_offs;

	ulong_t offset;

	uint8_t* first_thunk;
	uint8_t* first_thunk_org;

	char* dll_name;

	/* get code type */
	ulong_t code_type = 
		(pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC ? 64 : 32);

	/* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
	if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		/* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
		imp_offs = pe64_get_optional_header(mod_base)->DataDirectory[1].VirtualAddress;
		imp_size = pe64_get_optional_header(mod_base)->DataDirectory[1].Size;
	}
	else
	{
		/* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
		imp_offs = pe32_get_optional_header(mod_base)->DataDirectory[1].VirtualAddress;
		imp_size = pe32_get_optional_header(mod_base)->DataDirectory[1].Size;
	}

	if (!imp_offs && !imp_size)
	{
		/* no imports */
		return true;
	}

	/* convert to file aligned offset */
	if (file_aligned)
	{
		imp_offs = pe_rva_to_raw(mod_base, imp_offs);
	}

	imp_dir = (PIMAGE_IMPORT_DESCRIPTOR)((uint8_t *)mod_base + imp_offs);

	/* loop every imported module */
	while (imp_dir->FirstThunk)
	{
		/* address to module name */
		offset = imp_dir->Name;
		offset = (file_aligned ? pe_rva_to_raw(mod_base, offset) : offset);

		dll_name = (char *)mod_base + offset;

		/* if the remote module is not loaded, fail */
		if (module_get_base(proc_handle, dll_name, 0, code_type) == NULL)
		{
			return false;
		}

		/* get the first thunk pointer */
		offset = imp_dir->FirstThunk;
		offset = (file_aligned ? pe_rva_to_raw(mod_base, offset) : offset);

		first_thunk = (uint8_t *)mod_base + offset;

		/* get the original first thunk pointer */
		offset = imp_dir->OriginalFirstThunk;
		offset = (file_aligned ? pe_rva_to_raw(mod_base, offset) : offset);

		first_thunk_org = (uint8_t *)mod_base + offset;

		/* PE/PE+ specific */
		if (pe_get_magic(mod_base) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			PIMAGE_THUNK_DATA64 itd_first = (PIMAGE_THUNK_DATA64)first_thunk;
			PIMAGE_THUNK_DATA64 itd_org = (PIMAGE_THUNK_DATA64)first_thunk_org;
			PIMAGE_THUNK_DATA64 itd;

			/* assign correct thunk */
			itd = (imp_dir->OriginalFirstThunk != 0 ? itd_org : itd_first);

			/* loop every imported function */
			while (itd->u1.Function)
			{
				void* func_addr;

				if ((itd->u1.Ordinal & IMAGE_ORDINAL_FLAG64) != IMAGE_ORDINAL_FLAG64)
				{
					/* get the import name */
					offset = (ulong_t)itd->u1.AddressOfData;
					offset = (file_aligned ? pe_rva_to_raw(mod_base, offset) : offset);

					PIMAGE_IMPORT_BY_NAME ibn = (PIMAGE_IMPORT_BY_NAME)
						((uint8_t *)mod_base + offset);

					/* func find addr */
					func_addr = remote_get_proc_address(proc_handle, dll_name,
						ibn->Name, 0, 64);

					if (func_addr == NULL)
					{
						return false;
					}
				}
				else
				{
					ulong_t ord = ((ulong_t)itd->u1.Ordinal & 0xffff);

					/* func find addr */
					func_addr = remote_get_proc_address(proc_handle, dll_name,
						NULL, ord, 64);

					if (func_addr == NULL)
					{
						return false;
					}
				}

				/* fix the function pointer */
				itd_first->u1.Function = (uint64_t)func_addr;

				itd++;
				itd_first++;
			}
		}
		else
		{
			PIMAGE_THUNK_DATA32 itd_first = (PIMAGE_THUNK_DATA32)first_thunk;
			PIMAGE_THUNK_DATA32 itd_org = (PIMAGE_THUNK_DATA32)first_thunk_org;
			PIMAGE_THUNK_DATA32 itd;

			/* assign correct thunk */
			itd = (imp_dir->OriginalFirstThunk != 0 ? itd_org : itd_first);

			/* loop every imported function */
			while (itd->u1.Function)
			{
				void* func_addr;

				if ((itd->u1.Ordinal & IMAGE_ORDINAL_FLAG32) != IMAGE_ORDINAL_FLAG32)
				{
					/* get the import name */
					offset = (ulong_t)itd->u1.AddressOfData;
					offset = (file_aligned ? pe_rva_to_raw(mod_base, offset) : offset);

					PIMAGE_IMPORT_BY_NAME ibn = (PIMAGE_IMPORT_BY_NAME)
						((uint8_t *)mod_base + offset);

					/* func find addr */
					func_addr = remote_get_proc_address(proc_handle, dll_name,
						ibn->Name, 0, 32);

					if (func_addr == NULL)
					{
						return false;
					}
				}
				else
				{
					ulong_t ord = ((ulong_t)itd->u1.Ordinal & 0xffff);

					/* func find addr */
					func_addr = remote_get_proc_address(proc_handle, dll_name,
						NULL, ord, 32);

					if (func_addr == NULL)
					{
						return false;
					}
				}

				/* fix the function pointer */
				itd_first->u1.Function = (uint32_t)func_addr;

				itd++;
				itd_first++;
			}
		}

		imp_dir++;
	}

	return true;
}