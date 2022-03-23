#include "mapper.h"


#include "../Libs/misc.h"
#include "../Libs/mm.h"
#include "../Libs/pe.h"
#include "../Libs/str.h"
#include "log.h"

#define PE_HDR_SIZE 0x1000
#define PAGE_SIZE 0x1000

PIMAGE_OPTIONAL_HEADER64 mmap::pe64_get_optional_header( void* mod_base )
{
    PIMAGE_NT_HEADERS64 nt;
    PIMAGE_DOS_HEADER   dos;

    /* get the headers */
    dos = ( PIMAGE_DOS_HEADER )mod_base;
    nt  = ( PIMAGE_NT_HEADERS64 )( ( uint8_t* )dos + dos->e_lfanew );

    return &nt->OptionalHeader;
}

void* mmap::alloc_remote_module_memory( Process* proc, void* mod_base )
{
    uint64_t image_size = pe64_get_optional_header( mod_base )->SizeOfImage;

    return proc->alloc( image_size, PAGE_READWRITE );
}

bool_t mmap::map_remote_module( Process* proc, void* in_base, void** out_base, void** out_entrypoint )
{
    uint8_t* rnd_buf = ( uint8_t* )mm_halloc( PE_HDR_SIZE );

    uint8_t* curr_addr;
    void*    remote = nullptr;

    /* initialize the random buffer */
    fill_with_random( rnd_buf, PE_HDR_SIZE );

    /* get the needed info based off PE type */
    size_t   size_of_img = pe64_get_optional_header( in_base )->SizeOfImage;
    uint64_t sec_align   = pe64_get_optional_header( in_base )->SectionAlignment;
    uint64_t entry_point = pe64_get_optional_header( in_base )->AddressOfEntryPoint;

    if( entry_point == 0 )
    {
        LOG( "Failed to find entry point" );
        return false;
    }
        

    if( remote == nullptr )
    {
        remote = proc->alloc( size_of_img, PAGE_READWRITE );

        if( remote == nullptr )
        {
            LOG( "Failed to alloc memory" );
            mm_hfree( rnd_buf );
            return false;
        }
    }

    proc->protect( remote, size_of_img, PAGE_READWRITE );

    /* zero the remote memory */
    void* zero_buffer = mm_halloc( size_of_img );
    mm_set( zero_buffer, 0, size_of_img );
    auto ret = proc->write_memory( remote, zero_buffer, size_of_img );
    mm_hfree( zero_buffer );

    if( !ret )
        return false;

    /* fix the imported function addresses */
    ulong_t i = 0;

    while( true )
    {
        if( pe_imports_fix( proc, in_base, true ) )
        {
            /* success */
            break;
        }

        /* after x attempts exit */
        if( i > 20 )
        {
            LOG( "Failed to fix imports" );
            mm_hfree( rnd_buf );
            //proc->free( remote );
            return false;
        }

        sleep( 50 );
        i++;
    }

    /* fix the relocations */
    pe_relocate( in_base, remote, true );

    /* map the PE/PE+ */
    curr_addr = ( uint8_t* )remote;

    proc->write_memory( curr_addr, rnd_buf, PE_HDR_SIZE );
    proc->protect( curr_addr, PE_HDR_SIZE, PAGE_READONLY );

    curr_addr += PE_HDR_SIZE;

    /* map the sections */
    PIMAGE_SECTION_HEADER sec_hdr = pe_get_section_header( in_base, 0, NULL );

    LOG( "Looping through sections" );

    for( ulong_t i = 1; sec_hdr != NULL; i++ )
    {
        /* calculate the aligned section size */
        ulong_t sec_size = roundup( sec_hdr->Misc.VirtualSize, sec_align );
        ulong_t prot_flags;

        if( sec_hdr->Characteristics & IMAGE_SCN_MEM_READ )
        {
	        prot_flags = PAGE_READONLY;
        }

        if( sec_hdr->Characteristics & IMAGE_SCN_MEM_WRITE )
        {
	        prot_flags = PAGE_READWRITE;
        }

        if( sec_hdr->Characteristics & IMAGE_SCN_MEM_EXECUTE )
        {
	        prot_flags = PAGE_EXECUTE_READWRITE;
        }

        if( sec_hdr->Characteristics & IMAGE_SCN_MEM_WRITE &&
            sec_hdr->Characteristics & IMAGE_SCN_MEM_EXECUTE )
        {
            prot_flags = PAGE_EXECUTE_READWRITE;
        }

        /* replace the content with junk */
        if( str_astri( ( char* )sec_hdr->Name, ( char* )"rsrc" ) != NULL 
            || str_astri( ( char* )sec_hdr->Name, ( char* )"reloc" ) != NULL 
            || str_astri( ( char* )sec_hdr->Name, ( char* )"crt" ) != NULL
            || str_astri( ( char* )sec_hdr->Name, ( char* )"retplne" ) != NULL )
        {
            fill_with_random( ( uint8_t* )in_base + sec_hdr->PointerToRawData, sec_hdr->SizeOfRawData );
        }

        proc->write_memory( curr_addr, ( uint8_t* )in_base + sec_hdr->PointerToRawData, sec_hdr->SizeOfRawData );

        if( prot_flags == PAGE_EXECUTE_READWRITE )
        {
            proc->vad_spoof( curr_addr, sec_size, 0 );
        }
        else if( prot_flags != PAGE_READWRITE )
        {
            proc->protect( curr_addr, sec_size, prot_flags );
        }

        /* update the current address pointer */
        curr_addr += sec_size;

        /* get the next section */
        sec_hdr = pe_get_section_header( in_base, i, NULL );
    }

    *out_base = remote;

    if( ( uint8_t* )remote + entry_point == remote )
    {
        *out_entrypoint = NULL;
    }
    else
    { 
        *out_entrypoint = ( uint8_t* )remote + entry_point;
    }

    mm_hfree( rnd_buf );

    return true;
}

bool_t mmap::pe_imports_fix( Process* proc, void* mod_base, bool_t file_aligned )
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
        ( pe_get_magic( mod_base ) == IMAGE_NT_OPTIONAL_HDR64_MAGIC ? 64 : 32 );

    /* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
    if( pe_get_magic( mod_base ) == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
    {
        /* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
        imp_offs = pe64_get_optional_header( mod_base )->DataDirectory[ 1 ].VirtualAddress;
        imp_size = pe64_get_optional_header( mod_base )->DataDirectory[ 1 ].Size;
    }
    else
    {
        /* IMAGE_DIRECTORY_ENTRY_IMPORT = 1 */
        imp_offs = pe32_get_optional_header( mod_base )->DataDirectory[ 1 ].VirtualAddress;
        imp_size = pe32_get_optional_header( mod_base )->DataDirectory[ 1 ].Size;
    }

    if( !imp_offs && !imp_size )
    {
        /* no imports */
        LOG("No Imports");
        return true;
    }

    /* convert to file aligned offset */
    if( file_aligned ) { imp_offs = pe_rva_to_raw( mod_base, imp_offs ); }

    imp_dir = ( PIMAGE_IMPORT_DESCRIPTOR )( ( uint8_t* )mod_base + imp_offs );

    /* loop every imported module */
    while( imp_dir->FirstThunk )
    {
        /* address to module name */
        offset = imp_dir->Name;
        offset = ( file_aligned ? pe_rva_to_raw( mod_base, offset ) : offset );

        dll_name = ( char* )mod_base + offset;

        /* if the remote module is not loaded, fail */
        if( get_module_base( proc, dll_name, 0, code_type ) == NULL )
        {
            LOG("Failed to find:", dll_name);
            return false;
        }

        /* get the first thunk pointer */
        offset = imp_dir->FirstThunk;
        offset = ( file_aligned ? pe_rva_to_raw( mod_base, offset ) : offset );

        first_thunk = ( uint8_t* )mod_base + offset;

        /* get the original first thunk pointer */
        offset = imp_dir->OriginalFirstThunk;
        offset = ( file_aligned ? pe_rva_to_raw( mod_base, offset ) : offset );

        first_thunk_org = ( uint8_t* )mod_base + offset;

        if( pe_get_magic( mod_base ) == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
        {
            PIMAGE_THUNK_DATA64 itd_first = ( PIMAGE_THUNK_DATA64 )first_thunk;
            PIMAGE_THUNK_DATA64 itd_org   = ( PIMAGE_THUNK_DATA64 )first_thunk_org;
            PIMAGE_THUNK_DATA64 itd;

            /* assign correct thunk */
            itd = ( imp_dir->OriginalFirstThunk != 0 ? itd_org : itd_first );

            /* loop every imported function */
            while( itd->u1.Function )
            {
                void* func_addr;

                if( ( itd->u1.Ordinal & IMAGE_ORDINAL_FLAG64 ) != IMAGE_ORDINAL_FLAG64 )
                {
                    /* get the import name */
                    offset = ( ulong_t )itd->u1.AddressOfData;
                    offset = ( file_aligned ? pe_rva_to_raw( mod_base, offset ) : offset );

                    PIMAGE_IMPORT_BY_NAME ibn = ( PIMAGE_IMPORT_BY_NAME )( ( uint8_t* )mod_base + offset );

                    /* func find addr */
                    func_addr = remote_get_proc_address( proc, dll_name, ibn->Name, 0, 64 );

                    if( func_addr == NULL )
                    {
                        LOG("Failed to find:", ibn->Name, "From:", dll_name);
                        return false;
                    }
                }
                else
                {
                    ulong_t ord = ( ( ulong_t )itd->u1.Ordinal & 0xffff );

                    /* func find addr */
                    func_addr = remote_get_proc_address( proc, dll_name, NULL, ord, 64 );

                    if( func_addr == NULL )
                    {
                        LOG("Failed to find ord:", ord, "From:", dll_name);
                        return false;
                    }
                }

                /* fix the function pointer */
                itd_first->u1.Function = ( uint64_t )func_addr;

                itd++;
                itd_first++;
            }
        }
        else
        {
            PIMAGE_THUNK_DATA32 itd_first = ( PIMAGE_THUNK_DATA32 )first_thunk;
            PIMAGE_THUNK_DATA32 itd_org   = ( PIMAGE_THUNK_DATA32 )first_thunk_org;
            PIMAGE_THUNK_DATA32 itd;

            /* assign correct thunk */
            itd = ( imp_dir->OriginalFirstThunk != 0 ? itd_org : itd_first );

            /* loop every imported function */
            while( itd->u1.Function )
            {
                void* func_addr;

                if( ( itd->u1.Ordinal & IMAGE_ORDINAL_FLAG32 ) != IMAGE_ORDINAL_FLAG32 )
                {
                    /* get the import name */
                    offset = ( ulong_t )itd->u1.AddressOfData;
                    offset = ( file_aligned ? pe_rva_to_raw( mod_base, offset ) : offset );

                    PIMAGE_IMPORT_BY_NAME ibn = ( PIMAGE_IMPORT_BY_NAME )( ( uint8_t* )mod_base + offset );

                    /* func find addr */
                    func_addr = remote_get_proc_address( proc, dll_name, ibn->Name, 0, 32 );

                    if( func_addr == NULL )
                    {
                        LOG("Failed to find:", ibn->Name, "From:", dll_name);
                        return false;
                    }
                }
                else
                {
                    ulong_t ord = ( ( ulong_t )itd->u1.Ordinal & 0xffff );

                    /* func find addr */
                    func_addr = remote_get_proc_address( proc, dll_name, NULL, ord, 32 );

                    if( func_addr == NULL ) 
                    {
                        LOG("Failed to find ord:", ord, "From:", dll_name);
                        return false;
                    }
                }

                /* fix the function pointer */
                itd_first->u1.Function = ( uint32_t )func_addr;

                itd++;
                itd_first++;
            }
        }

        imp_dir++;
    }

    return true;
}

void* mmap::get_module_base( Process* proc, char* mod_name, size_t* image_size_out, ulong_t proc_type )
{
    void* dll_base;

    uint8_t* peb         = NULL;
    uint8_t* ldr_data    = NULL;
    uint8_t* first_entry = NULL;
    uint8_t* curr_entry  = NULL;
    uint8_t* mod         = NULL;

    size_t ptr_size;
    size_t offset = 0;

    /* buffer for the module name */
    char   tmp[MAX_PATH];
    void*  buff;
    size_t buff_len;

    /* if no module name is specified, return 0 */
    if( mod_name == NULL ) { return 0; }

    /* will contain LDR_DATA_TABLE_ENTRY 64/32 */
    uint8_t ldr_data_table[150];
    uint8_t ldr_data_table_size;

    /* zero out the ldr data table */
    mm_set( ldr_data_table, 0, sizeof( ldr_data_table ) );

    if( proc_type == 64 )
    {
        /* 64-bit process */
        ptr_size            = sizeof( uint64_t );
        offset              = offsetof( PEB64, Ldr );
        ldr_data_table_size = sizeof( LDR_DATA_TABLE_ENTRY64 );

        /* get the native peb */
        peb = (uint8_t*)proc->get_peb();
    }
    else if( proc_type == 32 )
    {
        /* 32-bit process */
        ptr_size            = sizeof( uint32_t );
        offset              = offsetof( PEB32, Ldr );
        ldr_data_table_size = sizeof( LDR_DATA_TABLE_ENTRY32 );

        /* get the wow64 peb */
        peb = (uint8_t*)proc->get_peb();
    }
    else { return nullptr; }

    /* read the PEB_LDR_DATA */
    if( !proc->read_memory( peb + offset, &ldr_data, ptr_size ) )
    {
        LOG("failed to read PEB_LDR_DATA");
        return nullptr;
    }

    offset = ( proc_type == 64
                   ? offsetof( PEB_LDR_DATA64, InMemoryOrderModuleList )
                   : offsetof( PEB_LDR_DATA32, InMemoryOrderModuleList ) );

    /* read the InMemoryOrderModuleList */
    if( !proc->read_memory( ldr_data + offset, &first_entry, ptr_size )  )
    {
        LOG("failed to read InMemoryOrderModuleList");
        return nullptr;
    }

    /* loop through the linked list */
    curr_entry = first_entry;

    do
    {
        mod = ( proc_type == 64
                    ? ( uint8_t* )CONTAINING_RECORD( curr_entry, LDR_DATA_TABLE_ENTRY64, InMemoryOrderLinks )
                    : ( uint8_t* )CONTAINING_RECORD( curr_entry, LDR_DATA_TABLE_ENTRY32, InMemoryOrderLinks ) );

        /* read the LDR_DATA_TABLE_ENTRY */
        if( !proc->read_memory( mod, ldr_data_table, ldr_data_table_size ) )
        {
            LOG("failed to read LDR_DATA_TABLE_ENTRY");
            return nullptr;
        }

        /* get BaseDllName info */
        buff = ( proc_type == 64
                     ? ( void* )( ( PLDR_DATA_TABLE_ENTRY64 )ldr_data_table )->BaseDllName.Buffer
                     : ( void* )( ( PLDR_DATA_TABLE_ENTRY32 )ldr_data_table )->BaseDllName.Buffer );

        buff_len = ( proc_type == 64
                         ? ( size_t )( ( PLDR_DATA_TABLE_ENTRY64 )ldr_data_table )->BaseDllName.MaximumLength
                         : ( size_t )( ( PLDR_DATA_TABLE_ENTRY32 )ldr_data_table )->BaseDllName.MaximumLength );

        /* get dll base */
        dll_base = ( proc_type == 64
                         ? ( void* )( ( PLDR_DATA_TABLE_ENTRY64 )ldr_data_table )->DllBase
                         : ( void* )( ( PLDR_DATA_TABLE_ENTRY32 )ldr_data_table )->DllBase );

        /* only check modules with a name and valid base */
        if( buff != NULL && dll_base != NULL )
        {
            bool_t  match = false;
            wchar_t base_dll_name[MAX_PATH];
            proc->read_memory( buff, base_dll_name, buff_len );
            str_utoa( tmp, base_dll_name );
            match = !str_acmpi( tmp, mod_name );

            if( proc_type == 64 )
            {
                PLDR_DATA_TABLE_ENTRY64 p =
                    ( PLDR_DATA_TABLE_ENTRY64 )ldr_data_table;

                if( match )
                {
                    if( image_size_out ) { *image_size_out = p->SizeOfImage; }

                    return ( void* )p->DllBase;
                }

                offset = offsetof( LIST_ENTRY64, Flink );
            }
            else
            {
                PLDR_DATA_TABLE_ENTRY32 p =
                    ( PLDR_DATA_TABLE_ENTRY32 )ldr_data_table;

                if( match )
                {
                    if( image_size_out ) { *image_size_out = p->SizeOfImage; }

                    return ( void* )p->DllBase;
                }

                offset = offsetof( LIST_ENTRY32, Flink );
            }
        }

        /* get the next entry */
        if( !proc->read_memory( curr_entry + offset, &curr_entry, ptr_size ) )
        {
            LOG("failed to read next entry");
            return nullptr;
        }
    }
    while( curr_entry != first_entry );

    return NULL;
}

void* mmap::remote_get_module( Process* proc, char* mod_name, void** remote_module_base, ulong_t proc_type )
{
    void*  result;
    size_t size_of_image;
    void*  dll_base;

    /* get the module's info */
    if( ( dll_base = get_module_base( proc, mod_name, &size_of_image, proc_type ) ) == NULL ) { return NULL; }

    /* allocate enough space locally */
    result = proc_vmalloc
    (
        NtCurrentProcess(),
        size_of_image,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    if( result == NULL ) { return NULL; }

    /* read the module */
    proc->read_pages( dll_base, result, size_of_image );

    if( remote_module_base ) { *remote_module_base = dll_base; }

    return result;
}

void* mmap::remote_get_proc_address( Process* proc, char* mod_name, char* func_name, ulong_t ord, ulong_t code_type )
{
    PIMAGE_EXPORT_DIRECTORY exp_dir;

    void* remote_mod_base;

    ushort_t* ord_table;
    ulong_t*  func_table;
    ulong_t*  name_table;

    ulong_t exp_size;
    ulong_t exp_offs;

    void* result = NULL;

    /* get the module info */
    void* mod_base = remote_get_module( proc, mod_name, &remote_mod_base, code_type );

    if( mod_base == NULL ) { return NULL; }

    /* get the export directory */
    if( pe_get_magic( mod_base ) == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
    {
        /* IMAGE_DIRECTORY_ENTRY_EXPORT = 0 */
        exp_offs = pe64_get_optional_header( mod_base )->DataDirectory[ 0 ].VirtualAddress;
        exp_size = pe64_get_optional_header( mod_base )->DataDirectory[ 0 ].Size;
    }
    else
    {
        /* IMAGE_DIRECTORY_ENTRY_EXPORT = 0 */
        exp_offs = pe32_get_optional_header( mod_base )->DataDirectory[ 0 ].VirtualAddress;
        exp_size = pe32_get_optional_header( mod_base )->DataDirectory[ 0 ].Size;
    }

    exp_dir = ( PIMAGE_EXPORT_DIRECTORY )( ( uint8_t* )mod_base + exp_offs );

    /* get the tables pointers */
    ord_table  = ( ushort_t* )( ( uint8_t* )mod_base + exp_dir->AddressOfNameOrdinals );
    func_table = ( ulong_t* )( ( uint8_t* )mod_base + exp_dir->AddressOfFunctions );
    name_table = ( ulong_t* )( ( uint8_t* )mod_base + exp_dir->AddressOfNames );

    if( func_name == NULL )
    {
        /* import by ordinal */
        ulong_t ord_base = exp_dir->Base;

        if( ord < ord_base ||
            ord > ord_base + exp_dir->NumberOfFunctions ) { return NULL; }

        result = ( void* )( ( uint8_t* )mod_base + func_table[ ord - ord_base ] );
    }
    else
    {
        /* import by name */
        for( ulong_t i = 0; i < exp_dir->NumberOfNames; i++ )
        {
            char* tmp = ( char* )( ( uint8_t* )mod_base + name_table[ i ] );

            if( !str_acmpi( tmp, func_name ) )
            {
                result = ( void* )( ( uint8_t* )mod_base + func_table[ ord_table[ i ] ] );
                break;
            }
        }
    }

    /* check if the function is forwarded */
    if( ( uint8_t* )result >= ( uint8_t* )exp_dir &&
        ( uint8_t* )result < ( uint8_t* )exp_dir + exp_size )
    {
        char tmp_func_name[MAX_PATH];
        char tmp_mod_name[MAX_PATH];

        char* p = str_achr( ( char* )result, '.' );

        /* copy the string */
        str_acpyn( ( char* )tmp_mod_name, ( char* )result, p - ( char* )result );
        str_acat( ( char* )tmp_mod_name, ( char* )".dll" );
        str_acpy( ( char* )tmp_func_name, p + 1 );

        /* recursive call */
        result = remote_get_proc_address( proc, tmp_mod_name, ( char* )tmp_func_name, ord, code_type );
    }
    else
    {
        /* output */
        result = ( void* )( ( uint8_t* )result - ( uint8_t* )mod_base +
            ( uint8_t* )remote_mod_base );
    }

    /* free the memory */
    proc_vmfree( NtCurrentProcess(), mod_base );

    LOG("Got Import:", func_name, "From:", mod_name, "Address:", result);

    return result;
}
