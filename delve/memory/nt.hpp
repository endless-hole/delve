#pragma once

#pragma warning (disable: 4214) // nonstandard extension used : bit field types other than int
#pragma warning (disable: 4201) // nonstandard extension used : nameless struct / union
#include <ntdef.h>

typedef struct _EX_PUSH_LOCK
{
    union
    {
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned __int64 Locked : 1; /* bit position: 0 */
            /* 0x0000 */ unsigned __int64 Waiting : 1; /* bit position: 1 */
            /* 0x0000 */ unsigned __int64 Waking : 1; /* bit position: 2 */
            /* 0x0000 */ unsigned __int64 MultipleShared : 1; /* bit position: 3 */
            /* 0x0000 */ unsigned __int64 Shared : 60; /* bit position: 4 */
        }; /* bitfield */
        /* 0x0000 */ unsigned __int64 Value;
        /* 0x0000 */ void* Ptr;
    }; /* size: 0x0008 */
} __EX_PUSH_LOCK; /* size: 0x0008 */

typedef struct _OBJECT_DIRECTORY_ENTRY
{
    /* 0x0000 */ struct _OBJECT_DIRECTORY_ENTRY* ChainLink;
    /* 0x0008 */ void* Object;
    /* 0x0010 */ unsigned long HashValue;
    /* 0x0014 */ long __PADDING__[ 1 ];
} OBJECT_DIRECTORY_ENTRY, * POBJECT_DIRECTORY_ENTRY; /* size: 0x0018 */

typedef struct _OBJECT_DIRECTORY
{
    /* 0x0000 */ struct _OBJECT_DIRECTORY_ENTRY* HashBuckets[ 37 ];
    /* 0x0128 */ struct _EX_PUSH_LOCK Lock;
    /* 0x0130 */ struct _DEVICE_MAP* DeviceMap;
    /* 0x0138 */ struct _OBJECT_DIRECTORY* ShadowDirectory;
    /* 0x0140 */ void* NamespaceEntry;
    /* 0x0148 */ void* SessionObject;
    /* 0x0150 */ unsigned long Flags;
    /* 0x0154 */ unsigned long SessionId;
} OBJECT_DIRECTORY, * POBJECT_DIRECTORY; /* size: 0x0158 */

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    PVOID Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    CHAR FullPathName[ 0x0100 ];
}RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    struct _RTL_PROCESS_MODULE_INFORMATION Modules[ ANYSIZE_ARRAY ];
}RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

// ----------------------------------------------------------------------------------------------------- // --

typedef struct _KAFFINITY_EX
{
    /* 0x0000 */ unsigned short Count;
    /* 0x0002 */ unsigned short Size;
    /* 0x0004 */ unsigned long Reserved;
    /* 0x0008 */ unsigned __int64 Bitmap[ 20 ];
} KAFFINITY_EX, * PKAFFINITY_EX; /* size: 0x00a8 */

typedef union _KEXECUTE_OPTIONS
{
    union
    {
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned char ExecuteDisable : 1; /* bit position: 0 */
            /* 0x0000 */ unsigned char ExecuteEnable : 1; /* bit position: 1 */
            /* 0x0000 */ unsigned char DisableThunkEmulation : 1; /* bit position: 2 */
            /* 0x0000 */ unsigned char Permanent : 1; /* bit position: 3 */
            /* 0x0000 */ unsigned char ExecuteDispatchEnable : 1; /* bit position: 4 */
            /* 0x0000 */ unsigned char ImageDispatchEnable : 1; /* bit position: 5 */
            /* 0x0000 */ unsigned char DisableExceptionChainValidation : 1; /* bit position: 6 */
            /* 0x0000 */ unsigned char Spare : 1; /* bit position: 7 */
        }; /* bitfield */
        /* 0x0000 */ volatile unsigned char ExecuteOptions;
        /* 0x0000 */ unsigned char ExecuteOptionsNV;
    }; /* size: 0x0001 */
} KEXECUTE_OPTIONS, * PKEXECUTE_OPTIONS; /* size: 0x0001 */

typedef union _KSTACK_COUNT
{
    union
    {
        /* 0x0000 */ long Value;
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned long State : 3; /* bit position: 0 */
            /* 0x0000 */ unsigned long StackCount : 29; /* bit position: 3 */
        }; /* bitfield */
    }; /* size: 0x0004 */
} KSTACK_COUNT, * PKSTACK_COUNT; /* size: 0x0004 */

typedef struct _SE_AUDIT_PROCESS_CREATION_INFO
{
    /* 0x0000 */ struct _OBJECT_NAME_INFORMATION* ImageFileName;
} SE_AUDIT_PROCESS_CREATION_INFO, * PSE_AUDIT_PROCESS_CREATION_INFO; /* size: 0x0008 */

typedef struct _ALPC_PROCESS_CONTEXT
{
    /* 0x0000 */ struct _EX_PUSH_LOCK Lock;
    /* 0x0008 */ struct _LIST_ENTRY ViewListHead;
    /* 0x0018 */ volatile unsigned __int64 PagedPoolQuotaCache;
} ALPC_PROCESS_CONTEXT, * PALPC_PROCESS_CONTEXT; /* size: 0x0020 */

typedef struct _MMSUPPORT_FLAGS
{
    union
    {
        struct
        {
            struct /* bitfield */
            {
                /* 0x0000 */ unsigned char WorkingSetType : 3; /* bit position: 0 */
                /* 0x0000 */ unsigned char Reserved0 : 3; /* bit position: 3 */
                /* 0x0000 */ unsigned char MaximumWorkingSetHard : 1; /* bit position: 6 */
                /* 0x0000 */ unsigned char MinimumWorkingSetHard : 1; /* bit position: 7 */
            }; /* bitfield */
            struct /* bitfield */
            {
                /* 0x0001 */ unsigned char SessionMaster : 1; /* bit position: 0 */
                /* 0x0001 */ unsigned char TrimmerState : 2; /* bit position: 1 */
                /* 0x0001 */ unsigned char Reserved : 1; /* bit position: 3 */
                /* 0x0001 */ unsigned char PageStealers : 4; /* bit position: 4 */
            }; /* bitfield */
        }; /* size: 0x0002 */
        /* 0x0000 */ unsigned short u1;
    }; /* size: 0x0002 */
    /* 0x0002 */ unsigned char MemoryPriority;
    union
    {
        struct /* bitfield */
        {
            /* 0x0003 */ unsigned char WsleDeleted : 1; /* bit position: 0 */
            /* 0x0003 */ unsigned char SvmEnabled : 1; /* bit position: 1 */
            /* 0x0003 */ unsigned char ForceAge : 1; /* bit position: 2 */
            /* 0x0003 */ unsigned char ForceTrim : 1; /* bit position: 3 */
            /* 0x0003 */ unsigned char NewMaximum : 1; /* bit position: 4 */
            /* 0x0003 */ unsigned char CommitReleaseState : 2; /* bit position: 5 */
        }; /* bitfield */
        /* 0x0003 */ unsigned char u2;
    }; /* size: 0x0001 */
} MMSUPPORT_FLAGS, * PMMSUPPORT_FLAGS; /* size: 0x0004 */

typedef struct _MMSUPPORT_INSTANCE
{
    /* 0x0000 */ unsigned long NextPageColor;
    /* 0x0004 */ unsigned long PageFaultCount;
    /* 0x0008 */ unsigned __int64 TrimmedPageCount;
    /* 0x0010 */ struct _MMWSL_INSTANCE* VmWorkingSetList;
    /* 0x0018 */ struct _LIST_ENTRY WorkingSetExpansionLinks;
    /* 0x0028 */ unsigned __int64 AgeDistribution[ 8 ];
    /* 0x0068 */ struct _KGATE* ExitOutswapGate;
    /* 0x0070 */ unsigned __int64 MinimumWorkingSetSize;
    /* 0x0078 */ unsigned __int64 WorkingSetLeafSize;
    /* 0x0080 */ unsigned __int64 WorkingSetLeafPrivateSize;
    /* 0x0088 */ unsigned __int64 WorkingSetSize;
    /* 0x0090 */ unsigned __int64 WorkingSetPrivateSize;
    /* 0x0098 */ unsigned __int64 MaximumWorkingSetSize;
    /* 0x00a0 */ unsigned __int64 PeakWorkingSetSize;
    /* 0x00a8 */ unsigned long HardFaultCount;
    /* 0x00ac */ unsigned short LastTrimStamp;
    /* 0x00ae */ unsigned short PartitionId;
    /* 0x00b0 */ unsigned __int64 SelfmapLock;
    /* 0x00b8 */ struct _MMSUPPORT_FLAGS Flags;
    /* 0x00bc */ long __PADDING__[ 1 ];
} MMSUPPORT_INSTANCE, * PMMSUPPORT_INSTANCE; /* size: 0x00c0 */

typedef struct _MMSUPPORT_SHARED
{
    /* 0x0000 */ volatile long WorkingSetLock;
    /* 0x0004 */ long GoodCitizenWaiting;
    /* 0x0008 */ unsigned __int64 ReleasedCommitDebt;
    /* 0x0010 */ unsigned __int64 ResetPagesRepurposedCount;
    /* 0x0018 */ void* WsSwapSupport;
    /* 0x0020 */ void* CommitReleaseContext;
    /* 0x0028 */ void* AccessLog;
    /* 0x0030 */ volatile unsigned __int64 ChargedWslePages;
    /* 0x0038 */ unsigned __int64 ActualWslePages;
    /* 0x0040 */ unsigned __int64 WorkingSetCoreLock;
    /* 0x0048 */ void* ShadowMapping;
    /* 0x0050 */ long __PADDING__[ 12 ];
} MMSUPPORT_SHARED, * PMMSUPPORT_SHARED; /* size: 0x0080 */

typedef struct _MMSUPPORT_FULL
{
    /* 0x0000 */ struct _MMSUPPORT_INSTANCE Instance;
    /* 0x00c0 */ struct _MMSUPPORT_SHARED Shared;
} MMSUPPORT_FULL, * PMMSUPPORT_FULL; /* size: 0x0140 */

typedef struct _EX_FAST_REF
{
    union
    {
        /* 0x0000 */ void* Object;
        /* 0x0000 */ unsigned __int64 RefCnt : 4; /* bit position: 0 */
        /* 0x0000 */ unsigned __int64 Value;
    }; /* size: 0x0008 */
} EX_FAST_REF, * PEX_FAST_REF; /* size: 0x0008 */

typedef struct _RTL_AVL_TREE
{
    /* 0x0000 */ struct _RTL_BALANCED_NODE* Root;
} RTL_AVL_TREE, * PRTL_AVL_TREE; /* size: 0x0008 */

typedef struct _PS_PROTECTION
{
    union
    {
        /* 0x0000 */ unsigned char Level;
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned char Type : 3; /* bit position: 0 */
            /* 0x0000 */ unsigned char Audit : 1; /* bit position: 3 */
            /* 0x0000 */ unsigned char Signer : 4; /* bit position: 4 */
        }; /* bitfield */
    }; /* size: 0x0001 */
} PS_PROTECTION, * PPS_PROTECTION; /* size: 0x0001 */

typedef union _PS_INTERLOCKED_TIMER_DELAY_VALUES
{
    union
    {
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned __int64 DelayMs : 30; /* bit position: 0 */
            /* 0x0000 */ unsigned __int64 CoalescingWindowMs : 30; /* bit position: 30 */
            /* 0x0000 */ unsigned __int64 Reserved : 1; /* bit position: 60 */
            /* 0x0000 */ unsigned __int64 NewTimerWheel : 1; /* bit position: 61 */
            /* 0x0000 */ unsigned __int64 Retry : 1; /* bit position: 62 */
            /* 0x0000 */ unsigned __int64 Locked : 1; /* bit position: 63 */
        }; /* bitfield */
        /* 0x0000 */ unsigned __int64 All;
    }; /* size: 0x0008 */
} PS_INTERLOCKED_TIMER_DELAY_VALUES, * PPS_INTERLOCKED_TIMER_DELAY_VALUES; /* size: 0x0008 */

typedef struct _JOBOBJECT_WAKE_FILTER
{
    /* 0x0000 */ unsigned long HighEdgeFilter;
    /* 0x0004 */ unsigned long LowEdgeFilter;
} JOBOBJECT_WAKE_FILTER, * PJOBOBJECT_WAKE_FILTER; /* size: 0x0008 */

typedef struct _PS_PROCESS_WAKE_INFORMATION
{
    /* 0x0000 */ unsigned __int64 NotificationChannel;
    /* 0x0008 */ unsigned long WakeCounters[ 7 ];
    /* 0x0024 */ struct _JOBOBJECT_WAKE_FILTER WakeFilter;
    /* 0x002c */ unsigned long NoWakeCounter;
} PS_PROCESS_WAKE_INFORMATION, * PPS_PROCESS_WAKE_INFORMATION; /* size: 0x0030 */



// ----------------------------------------------------------------------------------------------------- // --

typedef struct _LDR_DATA_TABLE_ENTRY
{
    /* 0x0000 */ struct _LIST_ENTRY InLoadOrderLinks;
    /* 0x0010 */ struct _LIST_ENTRY InMemoryOrderLinks;
    /* 0x0020 */ struct _LIST_ENTRY InInitializationOrderLinks;
    /* 0x0030 */ void* DllBase;
    /* 0x0038 */ void* EntryPoint;
    /* 0x0040 */ unsigned long SizeOfImage;
    /* 0x0044 */ long Padding_1;
    /* 0x0048 */ struct _UNICODE_STRING FullDllName;
    /* 0x0058 */ struct _UNICODE_STRING BaseDllName;
    union
    {
        /* 0x0068 */ unsigned char FlagGroup[ 4 ];
        /* 0x0068 */ unsigned long Flags;
        struct /* bitfield */
        {
            /* 0x0068 */ unsigned long PackagedBinary : 1; /* bit position: 0 */
            /* 0x0068 */ unsigned long MarkedForRemoval : 1; /* bit position: 1 */
            /* 0x0068 */ unsigned long ImageDll : 1; /* bit position: 2 */
            /* 0x0068 */ unsigned long LoadNotificationsSent : 1; /* bit position: 3 */
            /* 0x0068 */ unsigned long TelemetryEntryProcessed : 1; /* bit position: 4 */
            /* 0x0068 */ unsigned long ProcessStaticImport : 1; /* bit position: 5 */
            /* 0x0068 */ unsigned long InLegacyLists : 1; /* bit position: 6 */
            /* 0x0068 */ unsigned long InIndexes : 1; /* bit position: 7 */
            /* 0x0068 */ unsigned long ShimDll : 1; /* bit position: 8 */
            /* 0x0068 */ unsigned long InExceptionTable : 1; /* bit position: 9 */
            /* 0x0068 */ unsigned long ReservedFlags1 : 2; /* bit position: 10 */
            /* 0x0068 */ unsigned long LoadInProgress : 1; /* bit position: 12 */
            /* 0x0068 */ unsigned long LoadConfigProcessed : 1; /* bit position: 13 */
            /* 0x0068 */ unsigned long EntryProcessed : 1; /* bit position: 14 */
            /* 0x0068 */ unsigned long ProtectDelayLoad : 1; /* bit position: 15 */
            /* 0x0068 */ unsigned long ReservedFlags3 : 2; /* bit position: 16 */
            /* 0x0068 */ unsigned long DontCallForThreads : 1; /* bit position: 18 */
            /* 0x0068 */ unsigned long ProcessAttachCalled : 1; /* bit position: 19 */
            /* 0x0068 */ unsigned long ProcessAttachFailed : 1; /* bit position: 20 */
            /* 0x0068 */ unsigned long CorDeferredValidate : 1; /* bit position: 21 */
            /* 0x0068 */ unsigned long CorImage : 1; /* bit position: 22 */
            /* 0x0068 */ unsigned long DontRelocate : 1; /* bit position: 23 */
            /* 0x0068 */ unsigned long CorILOnly : 1; /* bit position: 24 */
            /* 0x0068 */ unsigned long ChpeImage : 1; /* bit position: 25 */
            /* 0x0068 */ unsigned long ReservedFlags5 : 2; /* bit position: 26 */
            /* 0x0068 */ unsigned long Redirected : 1; /* bit position: 28 */
            /* 0x0068 */ unsigned long ReservedFlags6 : 2; /* bit position: 29 */
            /* 0x0068 */ unsigned long CompatDatabaseProcessed : 1; /* bit position: 31 */
        }; /* bitfield */
    }; /* size: 0x0004 */
    /* 0x006c */ unsigned short ObsoleteLoadCount;
    /* 0x006e */ unsigned short TlsIndex;
    /* 0x0070 */ struct _LIST_ENTRY HashLinks;
    /* 0x0080 */ unsigned long TimeDateStamp;
    /* 0x0084 */ long Padding_2;
    /* 0x0088 */ struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
    /* 0x0090 */ void* Lock;
    /* 0x0098 */ struct _LDR_DDAG_NODE* DdagNode;
    /* 0x00a0 */ struct _LIST_ENTRY NodeModuleLink;
    /* 0x00b0 */ struct _LDRP_LOAD_CONTEXT* LoadContext;
    /* 0x00b8 */ void* ParentDllBase;
    /* 0x00c0 */ void* SwitchBackContext;
    /* 0x00c8 */ struct _RTL_BALANCED_NODE BaseAddressIndexNode;
    /* 0x00e0 */ struct _RTL_BALANCED_NODE MappingInfoIndexNode;
    /* 0x00f8 */ unsigned __int64 OriginalBase;
    /* 0x0100 */ union _LARGE_INTEGER LoadTime;
    /* 0x0108 */ unsigned long BaseNameHashValue;
    /* 0x010c */ enum _LDR_DLL_LOAD_REASON LoadReason;
    /* 0x0110 */ unsigned long ImplicitPathOptions;
    /* 0x0114 */ unsigned long ReferenceCount;
    /* 0x0118 */ unsigned long DependentLoadFlags;
    /* 0x011c */ unsigned char SigningLevel;
    /* 0x011d */ char __PADDING__[ 3 ];
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY; /* size: 0x0120 */

// ----------------------------------------------------------------------------------------------------- // --

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#define IMAGE_SIZEOF_SHORT_NAME              8
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[ 4 ];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[ 10 ];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER, * PIMAGE_DOS_HEADER;

typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[ IMAGE_SIZEOF_SHORT_NAME ];
    union {
        DWORD   PhysicalAddress;
        DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, * PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD        Magic;
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
    ULONGLONG   ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    DWORD       LoaderFlags;
    DWORD       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[ IMAGE_NUMBEROF_DIRECTORY_ENTRIES ];
} IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, * PIMAGE_NT_HEADERS64;

typedef IMAGE_NT_HEADERS64                  IMAGE_NT_HEADERS;

typedef struct _STOR_SCSI_IDENTITY
{
    char Space[ 0x8 ]; // +0x008 SerialNumber     : _STRING
    STRING SerialNumber;
} STOR_SCSI_IDENTITY, * PSTOR_SCSI_IDENTITY;

typedef struct _TELEMETRY_UNIT_EXTENSION
{
    /*
        +0x000 Flags            : <anonymous-tag>
        +0x000 DeviceHealthEventsLogged : Pos 0, 1 Bit
        +0x000 FailedFirstSMARTCommand : Pos 1, 1 Bit
        +0x000 FailedFirstDeviceStatisticsLogCommand : Pos 2, 1 Bit
        +0x000 FailedFirstNvmeCloudSSDCommand : Pos 3, 1 Bit
        +0x000 SmartPredictFailure : Pos 4, 1 Bit
        +0x000 Reserved         : Pos 5, 27 Bits
     */
    int SmartMask;
} TELEMETRY_UNIT_EXTENSION, * PTELEMETRY_UNIT_EXTENSION;

// lkd> dt storport!_RAID_UNIT_EXTENSION -b
typedef struct _RAID_UNIT_EXTENSION
{
    union
    {
        struct
        {
            char Space[ 0x68 ]; // +0x068 Identity         : _STOR_SCSI_IDENTITY
            STOR_SCSI_IDENTITY Identity;
        } _Identity;

        struct
        {
            char Space[ 0x7c8 ]; // +0x7c8 TelemetryExtension : _TELEMETRY_UNIT_EXTENSION
            TELEMETRY_UNIT_EXTENSION Telemetry;
        } _Smart;
    };
} RAID_UNIT_EXTENSION, * PRAID_UNIT_EXTENSION;

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))