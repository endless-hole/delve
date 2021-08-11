/*******************************************************************************
	HACK THE PLANETSIDE
	Libs - inject.c
	Library for stealthy manual mapping (x86/x64)
*******************************************************************************/
#include "types.h"
#include "inject.h"
#include "proc.h"
#include "pe.h"
#include "mm.h"
#include "str.h"

#define PE_HDR_SIZE 0x1000
#define PAGE_SIZE 0x1000

PIMAGE_OPTIONAL_HEADER64
pe64_get_optional_header(void* mod_base)
{
	PIMAGE_NT_HEADERS64 nt;
	PIMAGE_DOS_HEADER dos;

	/* get the headers */
	dos = (PIMAGE_DOS_HEADER)mod_base;
	nt = (PIMAGE_NT_HEADERS64)((uint8_t *)dos + dos->e_lfanew);

	return &nt->OptionalHeader;
}

void* alloc_remote_module_memory (handle_t proc_handle, void* module)
{
	uint64_t size_of_img = pe64_get_optional_header(module)->SizeOfImage;

	return proc_vmalloc(proc_handle, size_of_img, MEM_COMMIT,
		PAGE_READWRITE);
}

bool_t map_remote_module(
	handle_t proc_handle,
	void* module,
	void* remote_mem_optional,
	void** out_base, 
	void** out_entry_point)
{
	uint8_t* rnd_buff = mm_halloc(PE_HDR_SIZE);

	uint8_t* curr_addr;

	uint64_t size_of_img;
	uint64_t entry_point;
	uint64_t sec_align;

	void* remote_base_addr = remote_mem_optional;

	/* initialize the random buffer */
	fill_with_random(rnd_buff, PE_HDR_SIZE);

	/* get the needed info based off PE type */
	size_of_img = pe64_get_optional_header(module)->SizeOfImage;
	sec_align = pe64_get_optional_header(module)->SectionAlignment;
	entry_point = (uint64_t)pe64_get_entry_point(module);

	/* allocate the space for the module */
	if(remote_base_addr == NULL)
	{
		remote_base_addr = proc_vmalloc(proc_handle, size_of_img, MEM_COMMIT,
			PAGE_READWRITE);

		if (remote_base_addr == NULL)
		{
			mm_hfree(rnd_buff);
			return false;
		}
	}

	/* zero the remote memory */
	void* zero_buffer = mm_halloc(size_of_img);
	mm_set(zero_buffer, 0, size_of_img);
	proc_vmwrite(proc_handle, remote_base_addr, zero_buffer, size_of_img);
	mm_hfree(zero_buffer);

	/* fix the imported function addresses */
	ulong_t i = 0;

	while (true)
	{
		if (pe_imports_fix(proc_handle, module, true))
		{
			/* success */
			break;
		}

		/* after x attempts exit */
		if (i > 20)
		{
			mm_hfree(rnd_buff);
			proc_vmfree(proc_handle, remote_base_addr);
			return false;
		}

		sleep(50);
		i++;
	}

	/* fix the relocations */
	pe_relocate(module, remote_base_addr, true);

	/* map the PE/PE+ */
	curr_addr = (uint8_t *)remote_base_addr;

	proc_vmwrite(proc_handle, curr_addr, rnd_buff, PE_HDR_SIZE);
	proc_vmprotect(proc_handle, curr_addr, PE_HDR_SIZE, PAGE_READONLY);

	curr_addr += PE_HDR_SIZE;

	/* map the sections */
	PIMAGE_SECTION_HEADER sec_hdr = pe_get_section_header(module, 0, NULL);

	for (ulong_t i = 1; sec_hdr != NULL; i++)
	{
		/* calculate the aligned section size */
		ulong_t sec_size = roundup(sec_hdr->Misc.VirtualSize, sec_align);
		ulong_t prot_flags;

		if (sec_hdr->Characteristics & IMAGE_SCN_MEM_READ)
		{
			prot_flags = PAGE_READONLY;
		}

		if (sec_hdr->Characteristics & IMAGE_SCN_MEM_WRITE)
		{
			prot_flags = PAGE_READWRITE;
		}

		if (sec_hdr->Characteristics & IMAGE_SCN_MEM_EXECUTE)
		{
			prot_flags = PAGE_EXECUTE_READ;
		}

		if (sec_hdr->Characteristics & IMAGE_SCN_MEM_WRITE &&
			sec_hdr->Characteristics & IMAGE_SCN_MEM_EXECUTE)
		{
			prot_flags = PAGE_EXECUTE_READWRITE;
		}

		/* replace the content with junk */
		if (str_astri(sec_hdr->Name, "rsrc") != NULL ||
			str_astri(sec_hdr->Name, "reloc") != NULL)
		{
			fill_with_random((uint8_t *)module + sec_hdr->PointerToRawData,
				sec_hdr->SizeOfRawData);
		}

		/* write some junk */
#if WHAT_DID_I_MEAN_BY_THIS
		for (ulong_t j = 0; j < sec_size; j += PE_HDR_SIZE)
		{
			proc_vmwrite(proc_handle, curr_addr + j, rnd_buff, PE_HDR_SIZE);
		}
#endif

		/* write the section to the address */
		proc_vmwrite(proc_handle, curr_addr,
			(uint8_t *)module + sec_hdr->PointerToRawData,
			sec_hdr->SizeOfRawData);

		/* change the protection of the memory page */
		proc_vmprotect(proc_handle, curr_addr, sec_size, prot_flags);

		/* update the current address pointer */
		curr_addr += sec_size;

		/* get the next section */
		sec_hdr = pe_get_section_header(module, i, NULL);
	}

	*out_base = remote_base_addr;

	if((uint8_t *)remote_base_addr + entry_point == remote_base_addr)
	{
		*out_entry_point = NULL;
	}
	else
	{
		*out_entry_point = (uint8_t *)remote_base_addr + entry_point;
	}

	mm_hfree(rnd_buff);

	return true;
}

bool_t remote_iat_hook_init(iat_context_t* ctx, handle_t proc_handle)
{
	PIMAGE_NT_HEADERS nt;
	PIMAGE_DOS_HEADER dos;
	PIMAGE_IMPORT_DESCRIPTOR imp_desc;
	PIMAGE_THUNK_DATA thunk_data;
	PIMAGE_THUNK_DATA first_thunk;
	PIMAGE_THUNK_DATA orig_first_thunk;
	PIMAGE_IMPORT_BY_NAME imp_name;
	int64_t delta;

	/* custom PEB modules linked list walking */
	size_t mod_size;

	void* mod_base_remote = module_get_base(proc_handle, 
		ctx->base_module_name, &mod_size, 64);
	
	/* check if the module base is valid (and that it was found) */
	if(mod_base_remote == NULL)
	{
		return false;
	}

	void* mod_base = proc_vmalloc(NtCurrentProcess(), 
		mod_size, MEM_COMMIT, PAGE_READWRITE);
	proc_vmreadpages(proc_handle, mod_base_remote, mod_base, mod_size);
	dos = (PIMAGE_DOS_HEADER)mod_base;
	nt = (PIMAGE_NT_HEADERS)((uint8_t *)dos + dos->e_lfanew);

	/* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
	imp_desc = (PIMAGE_IMPORT_DESCRIPTOR)((uint8_t *)mod_base +
		nt->OptionalHeader.DataDirectory[1].VirtualAddress);

	/* find the module name */
	while (imp_desc->Name)
	{
		if (!str_acmpi((char *)((uint8_t *)mod_base + imp_desc->Name),
			ctx->module_name))
		{
			break;
		}

		imp_desc++;
	}

	/* setup the thunks */
	first_thunk = (PIMAGE_THUNK_DATA)((uint8_t *)mod_base +
		imp_desc->FirstThunk);

	orig_first_thunk = (PIMAGE_THUNK_DATA)((uint8_t *)mod_base +
		imp_desc->OriginalFirstThunk);

	thunk_data = (imp_desc->OriginalFirstThunk == 0 ? first_thunk :
		orig_first_thunk);

	/* loop through the imported functions */
	while (thunk_data->u1.Function)
	{
		/* check if the import is by name and not by ordinal */
		if ((thunk_data->u1.Ordinal & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG)
		{
			imp_name = (PIMAGE_IMPORT_BY_NAME)((uint8_t *)mod_base +
				thunk_data->u1.AddressOfData);

			if (!str_acmpi((char *)imp_name->Name, ctx->func_name))
			{
				uint64_t remote_ptr = (uint64_t)&first_thunk->u1.Function;

				remote_ptr -= (uint64_t)mod_base;
				remote_ptr += (uint64_t)mod_base_remote;

				ctx->old_func_addr = (void *)first_thunk->u1.Function;
				ctx->old_func_ptr = (void *)remote_ptr;

				proc_vmfree(NtCurrentProcess(), mod_base);

				return true;
			}
		}

		first_thunk++;
		thunk_data++;
	}

	proc_vmfree(NtCurrentProcess(), mod_base);

	return false;
}

bool_t remote_iat_hook_init_32(iat_context32_t* ctx, handle_t proc_handle)
{
	PIMAGE_NT_HEADERS32 nt;
	PIMAGE_DOS_HEADER dos;
	PIMAGE_IMPORT_DESCRIPTOR imp_desc;
	PIMAGE_THUNK_DATA32 thunk_data;
	PIMAGE_THUNK_DATA32 first_thunk;
	PIMAGE_THUNK_DATA32 orig_first_thunk;
	PIMAGE_IMPORT_BY_NAME imp_name;

	/* custom PEB modules linked list walking */
	size_t mod_size;

	void* mod_base_remote = module_get_base(proc_handle,
		ctx->base_module_name, &mod_size, 32);

	/* check if the module base is valid (and that it was found) */
	if (mod_base_remote == NULL)
	{
		return false;
	}

	void* mod_base = proc_vmalloc(NtCurrentProcess(),
		mod_size, MEM_COMMIT, PAGE_READWRITE);

	size_t read_size = proc_vmreadpages(proc_handle, 
		mod_base_remote, mod_base, mod_size);

	if(!read_size)
	{
		return false;
	}

	dos = (PIMAGE_DOS_HEADER)mod_base;
	nt = (PIMAGE_NT_HEADERS32)((uint8_t *)dos + dos->e_lfanew);

	/* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
	imp_desc = (PIMAGE_IMPORT_DESCRIPTOR)((uint8_t *)mod_base +
		nt->OptionalHeader.DataDirectory[1].VirtualAddress);

	/* find the module name */
	while (imp_desc->Name)
	{
		if (!str_acmpi((char *)((uint8_t *)mod_base + imp_desc->Name),
			ctx->module_name))
		{
			break;
		}

		imp_desc++;
	}

	if(imp_desc->Name == 0)
	{
		return false;
	}

	/* setup the thunks */
	first_thunk = (PIMAGE_THUNK_DATA32)((uint8_t *)mod_base +
		imp_desc->FirstThunk);

	orig_first_thunk = (PIMAGE_THUNK_DATA32)((uint8_t *)mod_base +
		imp_desc->OriginalFirstThunk);

	thunk_data = (imp_desc->OriginalFirstThunk == 0 ? first_thunk :
		orig_first_thunk);

	/* loop through the imported functions */
	while (thunk_data->u1.Function)
	{
		/* check if the import is by name and not by ordinal */
		if ((thunk_data->u1.Ordinal & IMAGE_ORDINAL_FLAG32) != IMAGE_ORDINAL_FLAG32)
		{
			imp_name = (PIMAGE_IMPORT_BY_NAME)((uint8_t *)mod_base +
				thunk_data->u1.AddressOfData);

			if (!str_acmpi((char *)imp_name->Name, ctx->func_name))
			{
				uint64_t remote_ptr = (uint64_t)&first_thunk->u1.Function;

				remote_ptr -= (uint64_t)mod_base;
				remote_ptr += (uint64_t)mod_base_remote;

				ctx->old_func_addr = (uint32_t)first_thunk->u1.Function;
				ctx->old_func_ptr = (uint32_t)remote_ptr;

				proc_vmfree(NtCurrentProcess(), mod_base);

				return true;
			}
		}

		first_thunk++;
		thunk_data++;
	}

	proc_vmfree(NtCurrentProcess(), mod_base);

	return false;
}

bool_t remote_iat_hook_install(iat_context_t* ctx, handle_t proc_handle)
{
	bool_t result;

	ulong_t old_prot = proc_vmprotect(proc_handle, 
		ctx->old_func_ptr, sizeof(ctx->old_func_ptr), PAGE_EXECUTE_READWRITE);

	result = (proc_vmwrite(proc_handle,	
		ctx->old_func_ptr, &ctx->dest, sizeof(ctx->dest)) == sizeof(ctx->dest));

	proc_vmprotect(proc_handle,
		ctx->old_func_ptr, sizeof(ctx->old_func_ptr), old_prot);

	return result;
}

bool_t remote_iat_hook_install_32(iat_context32_t* ctx, handle_t proc_handle)
{
	bool_t result;

	ulong_t old_prot = proc_vmprotect(proc_handle,
		(void *)ctx->old_func_ptr, sizeof(ctx->old_func_ptr), PAGE_EXECUTE_READWRITE);

	result = (proc_vmwrite(proc_handle,
		(void *)ctx->old_func_ptr, &ctx->dest, sizeof(ctx->dest)) == sizeof(ctx->dest));

	proc_vmprotect(proc_handle,
		(void *)ctx->old_func_ptr, sizeof(ctx->old_func_ptr), old_prot);

	return result;
}