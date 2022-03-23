#pragma once
#include <cstdint>
#include <ntdef.h>


#pragma pack(push, 1)

typedef union _virt_addr_t
{
    PVOID value;
    struct
    {
        ULONG64 offset : 12;
        ULONG64 pt_index : 9;
        ULONG64 pd_index : 9;
        ULONG64 pdpt_index : 9;
        ULONG64 pml4_index : 9;
        ULONG64 reserved : 16;
    };
} virt_addr_t, * pvirt_addr_t;
static_assert( sizeof( virt_addr_t ) == sizeof( PVOID ), "Size mismatch, only 64-bit supported." );

typedef union _pml4e_t
{
    ULONG64 value;
    struct
    {
        ULONG64 present : 1;          // Must be 1, region invalid if 0.
        ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
        ULONG64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
        ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PDPT.
        ULONG64 page_cache : 1; // Determines the memory type used to access PDPT.
        ULONG64 accessed : 1;         // If 0, this entry has not been used for translation.
        ULONG64 Ignored1 : 1;
        ULONG64 page_size : 1;         // Must be 0 for PML4E.
        ULONG64 Ignored2 : 4;
        ULONG64 pfn : 36; // The page frame number of the PDPT of this PML4E.
        ULONG64 Reserved : 4;
        ULONG64 Ignored3 : 11;
        ULONG64 nx : 1; // If 1, instruction fetches not allowed.
    };
} pml4e_t, * ppml4e_t;

typedef union _pdpte_t
{
    ULONG64 value;
    struct
    {
        ULONG64 present : 1;          // Must be 1, region invalid if 0.
        ULONG64 rw : 1;        // If 0, writes not allowed.
        ULONG64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
        ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PD.
        ULONG64 page_cache : 1; // Determines the memory type used to access PD.
        ULONG64 accessed : 1;         // If 0, this entry has not been used for translation.
        ULONG64 Ignored1 : 1;
        ULONG64 page_size : 1;         // If 1, this entry maps a 1GB page.
        ULONG64 Ignored2 : 4;
        ULONG64 pfn : 36; // The page frame number of the PD of this PDPTE.
        ULONG64 Reserved : 4;
        ULONG64 Ignored3 : 11;
        ULONG64 nx : 1; // If 1, instruction fetches not allowed.
    };
} pdpte_t, * ppdpte_t;

typedef union _pde_t
{
    ULONG64 value;
    struct
    {
        ULONG64 present : 1;          // Must be 1, region invalid if 0.
        ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
        ULONG64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
        ULONG64 PageWriteThrough : 1; // Determines the memory type used to access PT.
        ULONG64 page_cache : 1; // Determines the memory type used to access PT.
        ULONG64 Accessed : 1;         // If 0, this entry has not been used for translation.
        ULONG64 Ignored1 : 1;
        ULONG64 page_size : 1; // If 1, this entry maps a 2MB page.
        ULONG64 Ignored2 : 4;
        ULONG64 pfn : 36; // The page frame number of the PT of this PDE.
        ULONG64 Reserved : 4;
        ULONG64 Ignored3 : 11;
        ULONG64 nx : 1; // If 1, instruction fetches not allowed.
    };
} pde_t, * ppde_t;

typedef union _pte_t
{
    ULONG64 value;
    struct
    {
        ULONG64 present : 1;          // Must be 1, region invalid if 0.
        ULONG64 ReadWrite : 1;        // If 0, writes not allowed.
        ULONG64 user_supervisor : 1;   // If 0, user-mode accesses not allowed.
        ULONG64 PageWriteThrough : 1; // Determines the memory type used to access the memory.
        ULONG64 page_cache : 1; // Determines the memory type used to access the memory.
        ULONG64 accessed : 1;         // If 0, this entry has not been used for translation.
        ULONG64 Dirty : 1;            // If 0, the memory backing this page has not been written to.
        ULONG64 PageAccessType : 1;   // Determines the memory type used to access the memory.
        ULONG64 Global : 1;           // If 1 and the PGE bit of CR4 is set, translations are global.
        ULONG64 Ignored2 : 3;
        ULONG64 pfn : 36; // The page frame number of the backing physical page.
        ULONG64 Reserved : 4;
        ULONG64 Ignored3 : 7;
        ULONG64 ProtectionKey : 4;  // If the PKE bit of CR4 is set, determines the protection key.
        ULONG64 nx : 1; // If 1, instruction fetches not allowed.
    };
} pte_t, * ppte_t;

typedef union _cr3_t
{
    ULONG64 flags;
    struct
    {
        ULONG64 reserved1 : 3;
        ULONG64 page_level_write_through : 1;
        ULONG64 page_level_cache_disable : 1;
        ULONG64 reserved2 : 7;
        ULONG64 dirbase : 36;
        ULONG64 reserved3 : 16;
    };
} cr3_t;


#pragma pack(pop)

typedef enum _tdMMVAD_TYPE {
    VadNone = 0,
    VadDevicePhysicalMemory = 1,
    VadImageMap = 2,
    VadAwe = 3,
    VadWriteWatch = 4,
    VadLargePages = 5,
    VadRotatePhysical = 6,
    VadLargePageSection = 7
} _MMVAD_TYPE;

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MM_SHARED_VAD_FLAGS
//0x4 bytes (sizeof)
struct _MM_SHARED_VAD_FLAGS
{
    ULONG Lock : 1;                                                         //0x0
    ULONG LockContended : 1;                                                //0x0
    ULONG DeleteInProgress : 1;                                             //0x0
    ULONG NoChange : 1;                                                     //0x0
    ULONG VadType : 3;                                                      //0x0
    ULONG Protection : 5;                                                   //0x0
    ULONG PreferredNode : 7;                                                //0x0
    ULONG PageSize : 2;                                                     //0x0
    ULONG PrivateMemoryAlwaysClear : 1;                                     //0x0
    ULONG PrivateFixup : 1;                                                 //0x0
    ULONG HotPatchState : 2;                                                //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MM_GRAPHICS_VAD_FLAGS
//0x4 bytes (sizeof)
struct _MM_GRAPHICS_VAD_FLAGS
{
    ULONG Lock : 1;                                                         //0x0
    ULONG LockContended : 1;                                                //0x0
    ULONG DeleteInProgress : 1;                                             //0x0
    ULONG NoChange : 1;                                                     //0x0
    ULONG VadType : 3;                                                      //0x0
    ULONG Protection : 5;                                                   //0x0
    ULONG PreferredNode : 7;                                                //0x0
    ULONG PageSize : 2;                                                     //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                       //0x0
    ULONG WriteWatch : 1;                                                   //0x0
    ULONG FixedLargePageSize : 1;                                           //0x0
    ULONG ZeroFillPagesOptional : 1;                                        //0x0
    ULONG GraphicsAlwaysSet : 1;                                            //0x0
    ULONG GraphicsUseCoherentBus : 1;                                       //0x0
    ULONG GraphicsNoCache : 1;                                              //0x0
    ULONG GraphicsPageProtection : 3;                                       //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MM_PRIVATE_VAD_FLAGS
//0x4 bytes (sizeof)
struct _MM_PRIVATE_VAD_FLAGS
{
    ULONG Lock : 1;                                                         //0x0
    ULONG LockContended : 1;                                                //0x0
    ULONG DeleteInProgress : 1;                                             //0x0
    ULONG NoChange : 1;                                                     //0x0
    ULONG VadType : 3;                                                      //0x0
    ULONG Protection : 5;                                                   //0x0
    ULONG PreferredNode : 7;                                                //0x0
    ULONG PageSize : 2;                                                     //0x0
    ULONG PrivateMemoryAlwaysSet : 1;                                       //0x0
    ULONG WriteWatch : 1;                                                   //0x0
    ULONG FixedLargePageSize : 1;                                           //0x0
    ULONG ZeroFillPagesOptional : 1;                                        //0x0
    ULONG Graphics : 1;                                                     //0x0
    ULONG Enclave : 1;                                                      //0x0
    ULONG ShadowStack : 1;                                                  //0x0
    ULONG PhysicalMemoryPfnsReferenced : 1;                                 //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MMVAD_FLAGS
//0x4 bytes (sizeof)
struct _MMVAD_FLAGS
{
    ULONG Lock : 1;                                                         //0x0
    ULONG LockContended : 1;                                                //0x0
    ULONG DeleteInProgress : 1;                                             //0x0
    ULONG NoChange : 1;                                                     //0x0
    ULONG VadType : 3;                                                      //0x0
    ULONG Protection : 5;                                                   //0x0
    ULONG PreferredNode : 7;                                                //0x0
    ULONG PageSize : 2;                                                     //0x0
    ULONG PrivateMemory : 1;                                                //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_EX_PUSH_LOCK
//0x8 bytes (sizeof)
struct _EX_PUSH_LOCK
{
    union
    {
        struct
        {
            ULONGLONG Locked : 1;                                           //0x0
            ULONGLONG Waiting : 1;                                          //0x0
            ULONGLONG Waking : 1;                                           //0x0
            ULONGLONG MultipleShared : 1;                                   //0x0
            ULONGLONG Shared : 60;                                          //0x0
        };
        ULONGLONG Value;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MMVAD_FLAGS1
//0x4 bytes (sizeof)
struct _MMVAD_FLAGS1
{
    ULONG CommitCharge : 31;                                                //0x0
    ULONG MemCommit : 1;                                                    //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MMVAD_SHORT
//0x40 bytes (sizeof)
struct _MMVAD_SHORT
{
    union
    {
        struct
        {
            struct _MMVAD_SHORT* NextVad;                                   //0x0
            void* ExtraCreateInfo;                                          //0x8
        };
        struct _RTL_BALANCED_NODE VadNode;                                  //0x0
    };
    ULONG StartingVpn;                                                      //0x18
    ULONG EndingVpn;                                                        //0x1c
    UCHAR StartingVpnHigh;                                                  //0x20
    UCHAR EndingVpnHigh;                                                    //0x21
    UCHAR CommitChargeHigh;                                                 //0x22
    UCHAR SpareNT64VadUChar;                                                //0x23
    LONG ReferenceCount;                                                    //0x24
    struct _EX_PUSH_LOCK PushLock;                                          //0x28
    union
    {
        ULONG LongFlags;                                                    //0x30
        struct _MMVAD_FLAGS VadFlags;                                       //0x30
        struct _MM_PRIVATE_VAD_FLAGS PrivateVadFlags;                       //0x30
        struct _MM_GRAPHICS_VAD_FLAGS GraphicsVadFlags;                     //0x30
        struct _MM_SHARED_VAD_FLAGS SharedVadFlags;                         //0x30
        volatile ULONG VolatileVadLong;                                     //0x30
    } u;                                                                    //0x30
    union
    {
        ULONG LongFlags1;                                                   //0x34
        struct _MMVAD_FLAGS1 VadFlags1;                                     //0x34
    } u1;                                                                   //0x34
    union
    {
        ULONGLONG EventListULongPtr;                                        //0x38
        UCHAR StartingVpnHigher : 4;                                        //0x38
    } u5;                                                                   //0x38
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MMVAD_FLAGS2
//0x4 bytes (sizeof)
struct _MMVAD_FLAGS2
{
    ULONG FileOffset : 24;                                                  //0x0
    ULONG Large : 1;                                                        //0x0
    ULONG TrimBehind : 1;                                                   //0x0
    ULONG Inherit : 1;                                                      //0x0
    ULONG NoValidationNeeded : 1;                                           //0x0
    ULONG PrivateDemandZero : 1;                                            //0x0
    ULONG Spare : 3;                                                        //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MI_VAD_SEQUENTIAL_INFO
//0x8 bytes (sizeof)
struct _MI_VAD_SEQUENTIAL_INFO
{
    ULONGLONG Length : 12;                                                  //0x0
    ULONGLONG Vpn : 52;                                                     //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_RTL_AVL_TREE
//0x8 bytes (sizeof)
struct _RTL_AVL_TREE
{
    struct _RTL_BALANCED_NODE* Root;                                        //0x0
    void * NodeHint;
    unsigned __int64 NumberGenericTableElements; 
};

//0x4 bytes (sizeof)
struct _MMSUBSECTION_FLAGS
{
    USHORT SubsectionAccessed : 1;                                            //0x0
    USHORT Protection : 5;                                                    //0x0
    USHORT StartingSector4132 : 10;                                           //0x0
    USHORT SubsectionStatic : 1;                                              //0x2
    USHORT GlobalMemory : 1;                                                  //0x2
    USHORT Spare : 1;                                                         //0x2
    USHORT OnDereferenceList : 1;                                             //0x2
    USHORT SectorEndOffset : 12;                                              //0x2
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MI_SUBSECTION_ENTRY1
//0x4 bytes (sizeof)
struct _MI_SUBSECTION_ENTRY1
{
    ULONG CrossPartitionReferences : 30;                                      //0x0
    ULONG SubsectionMappedLarge : 2;                                          //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_SUBSECTION
//0x38 bytes (sizeof)
struct _SUBSECTION
{
    struct _CONTROL_AREA* ControlArea;                                      //0x0
    struct _MMPTE* SubsectionBase;                                          //0x8
    struct _SUBSECTION* NextSubsection;                                     //0x10
    union
    {
        struct _RTL_AVL_TREE GlobalPerSessionHead;                          //0x18
        struct _MI_CONTROL_AREA_WAIT_BLOCK* CreationWaitList;               //0x18
        struct _MI_PER_SESSION_PROTOS* SessionDriverProtos;                 //0x18
    };
    union
    {
        ULONG LongFlags;                                                    //0x20
        struct _MMSUBSECTION_FLAGS SubsectionFlags;                         //0x20
    } u;                                                                    //0x20
    ULONG StartingSector;                                                   //0x24
    ULONG NumberOfFullSectors;                                              //0x28
    ULONG PtesInSubsection;                                                 //0x2c
    union
    {
        struct _MI_SUBSECTION_ENTRY1 e1;                                    //0x30
        ULONG EntireField;                                                  //0x30
    } u1;                                                                   //0x30
    ULONG UnusedPtes : 30;                                                    //0x34
    ULONG ExtentQueryNeeded : 1;                                              //0x34
    ULONG DirtyPages : 1;                                                     //0x34
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_HARDWARE
//0x8 bytes (sizeof)
struct _MMPTE_HARDWARE
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG Dirty1 : 1;                                                     //0x0
    ULONGLONG Owner : 1;                                                      //0x0
    ULONGLONG WriteThrough : 1;                                               //0x0
    ULONGLONG CacheDisable : 1;                                               //0x0
    ULONGLONG Accessed : 1;                                                   //0x0
    ULONGLONG Dirty : 1;                                                      //0x0
    ULONGLONG LargePage : 1;                                                  //0x0
    ULONGLONG Global : 1;                                                     //0x0
    ULONGLONG CopyOnWrite : 1;                                                //0x0
    ULONGLONG Unused : 1;                                                     //0x0
    ULONGLONG Write : 1;                                                      //0x0
    ULONGLONG PageFrameNumber : 36;                                           //0x0
    ULONGLONG ReservedForHardware : 4;                                        //0x0
    ULONGLONG ReservedForSoftware : 4;                                        //0x0
    ULONGLONG WsleAge : 4;                                                    //0x0
    ULONGLONG WsleProtection : 3;                                             //0x0
    ULONGLONG NoExecute : 1;                                                  //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_PROTOTYPE
//0x8 bytes (sizeof)
struct _MMPTE_PROTOTYPE
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG DemandFillProto : 1;                                            //0x0
    ULONGLONG HiberVerifyConverted : 1;                                       //0x0
    ULONGLONG ReadOnly : 1;                                                   //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG Combined : 1;                                                   //0x0
    ULONGLONG Unused1 : 4;                                                    //0x0
    LONGLONG ProtoAddress : 48;                                               //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_SOFTWARE
//0x8 bytes (sizeof)
struct _MMPTE_SOFTWARE
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG PageFileReserved : 1;                                           //0x0
    ULONGLONG PageFileAllocated : 1;                                          //0x0
    ULONGLONG ColdPage : 1;                                                   //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG Transition : 1;                                                 //0x0
    ULONGLONG PageFileLow : 4;                                                //0x0
    ULONGLONG UsedPageTableEntries : 10;                                      //0x0
    ULONGLONG ShadowStack : 1;                                                //0x0
    ULONGLONG Unused : 5;                                                     //0x0
    ULONGLONG PageFileHigh : 32;                                              //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_TIMESTAMP
//0x8 bytes (sizeof)
struct _MMPTE_TIMESTAMP
{
    ULONGLONG MustBeZero : 1;                                                 //0x0
    ULONGLONG Unused : 3;                                                     //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG Transition : 1;                                                 //0x0
    ULONGLONG PageFileLow : 4;                                                //0x0
    ULONGLONG Reserved : 16;                                                  //0x0
    ULONGLONG GlobalTimeStamp : 32;                                           //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_TRANSITION
//0x8 bytes (sizeof)
struct _MMPTE_TRANSITION
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG Write : 1;                                                      //0x0
    ULONGLONG Spare : 1;                                                      //0x0
    ULONGLONG IoTracker : 1;                                                  //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG Transition : 1;                                                 //0x0
    ULONGLONG PageFrameNumber : 36;                                           //0x0
    ULONGLONG Unused : 16;                                                    //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_SUBSECTION
//0x8 bytes (sizeof)
struct _MMPTE_SUBSECTION
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG Unused0 : 3;                                                    //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG ColdPage : 1;                                                   //0x0
    ULONGLONG Unused1 : 3;                                                    //0x0
    ULONGLONG ExecutePrivilege : 1;                                           //0x0
    LONGLONG SubsectionAddress : 48;                                          //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE_LIST
//0x8 bytes (sizeof)
struct _MMPTE_LIST
{
    ULONGLONG Valid : 1;                                                      //0x0
    ULONGLONG OneEntry : 1;                                                   //0x0
    ULONGLONG filler0 : 2;                                                    //0x0
    ULONGLONG SwizzleBit : 1;                                                 //0x0
    ULONGLONG Protection : 5;                                                 //0x0
    ULONGLONG Prototype : 1;                                                  //0x0
    ULONGLONG Transition : 1;                                                 //0x0
    ULONGLONG filler1 : 16;                                                   //0x0
    ULONGLONG NextEntry : 36;                                                 //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016/2110%2021H2%20(November%202021%20Update)/_MMPTE
//0x8 bytes (sizeof)
struct _MMPTE
{
    union
    {
        ULONGLONG Long;                                                     //0x0
        volatile ULONGLONG VolatileLong;                                    //0x0
        struct _MMPTE_HARDWARE Hard;                                        //0x0
        struct _MMPTE_PROTOTYPE Proto;                                      //0x0
        struct _MMPTE_SOFTWARE Soft;                                        //0x0
        struct _MMPTE_TIMESTAMP TimeStamp;                                  //0x0
        struct _MMPTE_TRANSITION Trans;                                     //0x0
        struct _MMPTE_SUBSECTION Subsect;                                   //0x0
        struct _MMPTE_LIST List;                                            //0x0
    } u;                                                                    //0x0
};

//https://www.vergiliusproject.com/kernels/x64/Windows%2011/21H2%20(RTM)/_MMVAD
//0x88 bytes (sizeof)
struct _MMVAD
{
    struct _MMVAD_SHORT Core;                                               //0x0
    union
    {
        ULONG LongFlags2;                                                   //0x40
        volatile struct _MMVAD_FLAGS2 VadFlags2;                            //0x40
    } u2;                                                                   //0x40
    struct _SUBSECTION* Subsection;                                         //0x48
    struct _MMPTE* FirstPrototypePte;                                       //0x50
    struct _MMPTE* LastContiguousPte;                                       //0x58
    struct _LIST_ENTRY ViewLinks;                                           //0x60
    struct _EPROCESS* VadsProcess;                                          //0x70
    union
    {
        struct _MI_VAD_SEQUENTIAL_INFO SequentialVa;                        //0x78
        struct _MMEXTEND_INFO* ExtendedInfo;                                //0x78
    } u4;                                                                   //0x78
    struct _FILE_OBJECT* FileObject;                                        //0x80
};

//0xa8 bytes (sizeof)
struct _KAFFINITY_EX
{
    USHORT Count;                                                           //0x0
    USHORT Size;                                                            //0x2
    ULONG Reserved;                                                         //0x4
    ULONGLONG Bitmap[ 20 ];                                                   //0x8
};

//0x1 bytes (sizeof)
union _KEXECUTE_OPTIONS
{
    UCHAR ExecuteDisable : 1;                                                 //0x0
    UCHAR ExecuteEnable : 1;                                                  //0x0
    UCHAR DisableThunkEmulation : 1;                                          //0x0
    UCHAR Permanent : 1;                                                      //0x0
    UCHAR ExecuteDispatchEnable : 1;                                          //0x0
    UCHAR ImageDispatchEnable : 1;                                            //0x0
    UCHAR DisableExceptionChainValidation : 1;                                //0x0
    UCHAR Spare : 1;                                                          //0x0
    volatile UCHAR ExecuteOptions;                                          //0x0
    UCHAR ExecuteOptionsNV;                                                 //0x0
};

//0x4 bytes (sizeof)
union _KSTACK_COUNT
{
    LONG Value;                                                             //0x0
    ULONG State : 3;                                                          //0x0
    ULONG StackCount : 29;                                                    //0x0
};

//0x18 bytes (sizeof)
struct __DISPATCHER_HEADER
{
    union
    {
        volatile LONG Lock;                                                 //0x0
        LONG LockNV;                                                        //0x0
        struct
        {
            UCHAR Type;                                                     //0x0
            UCHAR Signalling;                                               //0x1
            UCHAR Size;                                                     //0x2
            UCHAR Reserved1;                                                //0x3
        };
        struct
        {
            UCHAR TimerType;                                                //0x0
            union
            {
                UCHAR TimerControlFlags;                                    //0x1
                struct
                {
                    UCHAR Absolute : 1;                                       //0x1
                    UCHAR Wake : 1;                                           //0x1
                    UCHAR EncodedTolerableDelay : 6;                          //0x1
                };
            };
            UCHAR Hand;                                                     //0x2
            union
            {
                UCHAR TimerMiscFlags;                                       //0x3
                struct
                {
                    UCHAR Index : 6;                                          //0x3
                    UCHAR Inserted : 1;                                       //0x3
                    volatile UCHAR Expired : 1;                               //0x3
                };
            };
        };
        struct
        {
            UCHAR Timer2Type;                                               //0x0
            union
            {
                UCHAR Timer2Flags;                                          //0x1
                struct
                {
                    UCHAR Timer2Inserted : 1;                                 //0x1
                    UCHAR Timer2Expiring : 1;                                 //0x1
                    UCHAR Timer2CancelPending : 1;                            //0x1
                    UCHAR Timer2SetPending : 1;                               //0x1
                    UCHAR Timer2Running : 1;                                  //0x1
                    UCHAR Timer2Disabled : 1;                                 //0x1
                    UCHAR Timer2ReservedFlags : 2;                            //0x1
                };
            };
            UCHAR Timer2ComponentId;                                        //0x2
            UCHAR Timer2RelativeId;                                         //0x3
        };
        struct
        {
            UCHAR QueueType;                                                //0x0
            union
            {
                UCHAR QueueControlFlags;                                    //0x1
                struct
                {
                    UCHAR Abandoned : 1;                                      //0x1
                    UCHAR DisableIncrement : 1;                               //0x1
                    UCHAR QueueReservedControlFlags : 6;                      //0x1
                };
            };
            UCHAR QueueSize;                                                //0x2
            UCHAR QueueReserved;                                            //0x3
        };
        struct
        {
            UCHAR ThreadType;                                               //0x0
            UCHAR ThreadReserved;                                           //0x1
            union
            {
                UCHAR ThreadControlFlags;                                   //0x2
                struct
                {
                    UCHAR CycleProfiling : 1;                                 //0x2
                    UCHAR CounterProfiling : 1;                               //0x2
                    UCHAR GroupScheduling : 1;                                //0x2
                    UCHAR AffinitySet : 1;                                    //0x2
                    UCHAR Tagged : 1;                                         //0x2
                    UCHAR EnergyProfiling : 1;                                //0x2
                    UCHAR SchedulerAssist : 1;                                //0x2
                    UCHAR ThreadReservedControlFlags : 1;                     //0x2
                };
            };
            union
            {
                UCHAR DebugActive;                                          //0x3
                struct
                {
                    UCHAR ActiveDR7 : 1;                                      //0x3
                    UCHAR Instrumented : 1;                                   //0x3
                    UCHAR Minimal : 1;                                        //0x3
                    UCHAR Reserved4 : 2;                                      //0x3
                    UCHAR AltSyscall : 1;                                     //0x3
                    UCHAR Emulation : 1;                                      //0x3
                    UCHAR Reserved5 : 1;                                      //0x3
                };
            };
        };
        struct
        {
            UCHAR MutantType;                                               //0x0
            UCHAR MutantSize;                                               //0x1
            UCHAR DpcActive;                                                //0x2
            UCHAR MutantReserved;                                           //0x3
        };
    };
    LONG SignalState;                                                       //0x4
    struct _LIST_ENTRY WaitListHead;                                        //0x8
};

//0x438 bytes (sizeof)
struct __KPROCESS
{
    struct __DISPATCHER_HEADER Header;                                       //0x0
    struct _LIST_ENTRY ProfileListHead;                                     //0x18
    ULONGLONG DirectoryTableBase;                                           //0x28
    struct _LIST_ENTRY ThreadListHead;                                      //0x30
    ULONG ProcessLock;                                                      //0x40
    ULONG ProcessTimerDelay;                                                //0x44
    ULONGLONG DeepFreezeStartTime;                                          //0x48
    struct _KAFFINITY_EX Affinity;                                          //0x50
    ULONGLONG AffinityPadding[ 12 ];                                          //0xf8
    struct _LIST_ENTRY ReadyListHead;                                       //0x158
    struct _SINGLE_LIST_ENTRY SwapListEntry;                                //0x168
    volatile struct _KAFFINITY_EX ActiveProcessors;                         //0x170
    ULONGLONG ActiveProcessorsPadding[ 12 ];                                  //0x218
    union
    {
        struct
        {
            ULONG AutoAlignment : 1;                                          //0x278
            ULONG DisableBoost : 1;                                           //0x278
            ULONG DisableQuantum : 1;                                         //0x278
            ULONG DeepFreeze : 1;                                             //0x278
            ULONG TimerVirtualization : 1;                                    //0x278
            ULONG CheckStackExtents : 1;                                      //0x278
            ULONG CacheIsolationEnabled : 1;                                  //0x278
            ULONG PpmPolicy : 3;                                              //0x278
            ULONG VaSpaceDeleted : 1;                                         //0x278
            ULONG ReservedFlags : 21;                                         //0x278
        };
        volatile LONG ProcessFlags;                                         //0x278
    };
    ULONG ActiveGroupsMask;                                                 //0x27c
    CHAR BasePriority;                                                      //0x280
    CHAR QuantumReset;                                                      //0x281
    CHAR Visited;                                                           //0x282
    union _KEXECUTE_OPTIONS Flags;                                          //0x283
    USHORT ThreadSeed[ 20 ];                                                  //0x284
    USHORT ThreadSeedPadding[ 12 ];                                           //0x2ac
    USHORT IdealProcessor[ 20 ];                                              //0x2c4
    USHORT IdealProcessorPadding[ 12 ];                                       //0x2ec
    USHORT IdealNode[ 20 ];                                                   //0x304
    USHORT IdealNodePadding[ 12 ];                                            //0x32c
    USHORT IdealGlobalNode;                                                 //0x344
    USHORT Spare1;                                                          //0x346
    union _KSTACK_COUNT StackCount;                                 //0x348
    struct _LIST_ENTRY ProcessListEntry;                                    //0x350
    ULONGLONG CycleTime;                                                    //0x360
    ULONGLONG ContextSwitches;                                              //0x368
    struct _KSCHEDULING_GROUP* SchedulingGroup;                             //0x370
    ULONG FreezeCount;                                                      //0x378
    ULONG KernelTime;                                                       //0x37c
    ULONG UserTime;                                                         //0x380
    ULONG ReadyTime;                                                        //0x384
    ULONGLONG UserDirectoryTableBase;                                       //0x388
    UCHAR AddressPolicy;                                                    //0x390
    UCHAR Spare2[ 71 ];                                                       //0x391
    VOID* InstrumentationCallback;                                          //0x3d8
    union
    {
        ULONGLONG SecureHandle;                                             //0x3e0
        struct
        {
            ULONGLONG SecureProcess : 1;                                      //0x3e0
            ULONGLONG Unused : 1;                                             //0x3e0
        } Flags;                                                            //0x3e0
    } SecureState;                                                          //0x3e0
    ULONGLONG KernelWaitTime;                                               //0x3e8
    ULONGLONG UserWaitTime;                                                 //0x3f0
    ULONGLONG EndPadding[ 8 ];                                                //0x3f8
};

//0x8 bytes (sizeof)
struct _EX_FAST_REF
{
    union
    {
        VOID* Object;                                                       //0x0
        ULONGLONG RefCnt : 4;                                                 //0x0
        ULONGLONG Value;                                                    //0x0
    };
};

//0x8 bytes (sizeof)
struct _SE_AUDIT_PROCESS_CREATION_INFO
{
    struct _OBJECT_NAME_INFORMATION* ImageFileName;                         //0x0
};

//0x20 bytes (sizeof)
struct _ALPC_PROCESS_CONTEXT
{
    struct _EX_PUSH_LOCK Lock;                                              //0x0
    struct _LIST_ENTRY ViewListHead;                                        //0x8
    volatile ULONGLONG PagedPoolQuotaCache;                                 //0x18
};

//0x4 bytes (sizeof)
struct _MMSUPPORT_FLAGS
{
    union
    {
        struct
        {
            UCHAR WorkingSetType : 3;                                         //0x0
            UCHAR Reserved0 : 3;                                              //0x0
            UCHAR MaximumWorkingSetHard : 1;                                  //0x0
            UCHAR MinimumWorkingSetHard : 1;                                  //0x0
            UCHAR SessionMaster : 1;                                          //0x1
            UCHAR TrimmerState : 2;                                           //0x1
            UCHAR Reserved : 1;                                               //0x1
            UCHAR PageStealers : 4;                                           //0x1
        };
        USHORT u1;                                                          //0x0
    };
    UCHAR MemoryPriority;                                                   //0x2
    union
    {
        struct
        {
            UCHAR WsleDeleted : 1;                                            //0x3
            UCHAR SvmEnabled : 1;                                             //0x3
            UCHAR ForceAge : 1;                                               //0x3
            UCHAR ForceTrim : 1;                                              //0x3
            UCHAR NewMaximum : 1;                                             //0x3
            UCHAR CommitReleaseState : 2;                                     //0x3
        };
        UCHAR u2;                                                           //0x3
    };
};

//0x80 bytes (sizeof)
struct _MMSUPPORT_SHARED
{
    volatile LONG WorkingSetLock;                                           //0x0
    LONG GoodCitizenWaiting;                                                //0x4
    ULONGLONG ReleasedCommitDebt;                                           //0x8
    ULONGLONG ResetPagesRepurposedCount;                                    //0x10
    VOID* WsSwapSupport;                                                    //0x18
    VOID* CommitReleaseContext;                                             //0x20
    VOID* AccessLog;                                                        //0x28
    volatile ULONGLONG ChargedWslePages;                                    //0x30
    ULONGLONG ActualWslePages;                                              //0x38
    ULONGLONG WorkingSetCoreLock;                                           //0x40
    VOID* ShadowMapping;                                                    //0x48
};

//0xc0 bytes (sizeof)
struct _MMSUPPORT_INSTANCE
{
    ULONG NextPageColor;                                                    //0x0
    ULONG PageFaultCount;                                                   //0x4
    ULONGLONG TrimmedPageCount;                                             //0x8
    struct _MMWSL_INSTANCE* VmWorkingSetList;                               //0x10
    struct _LIST_ENTRY WorkingSetExpansionLinks;                            //0x18
    ULONGLONG AgeDistribution[ 8 ];                                           //0x28
    struct _KGATE* ExitOutswapGate;                                         //0x68
    ULONGLONG MinimumWorkingSetSize;                                        //0x70
    ULONGLONG WorkingSetLeafSize;                                           //0x78
    ULONGLONG WorkingSetLeafPrivateSize;                                    //0x80
    ULONGLONG WorkingSetSize;                                               //0x88
    ULONGLONG WorkingSetPrivateSize;                                        //0x90
    ULONGLONG MaximumWorkingSetSize;                                        //0x98
    ULONGLONG PeakWorkingSetSize;                                           //0xa0
    ULONG HardFaultCount;                                                   //0xa8
    USHORT LastTrimStamp;                                                   //0xac
    USHORT PartitionId;                                                     //0xae
    ULONGLONG SelfmapLock;                                                  //0xb0
    struct _MMSUPPORT_FLAGS Flags;                                          //0xb8
};

//0x140 bytes (sizeof)
struct _MMSUPPORT_FULL
{
    struct _MMSUPPORT_INSTANCE Instance;                                    //0x0
    struct _MMSUPPORT_SHARED Shared;                                        //0xc0
};

//0x1 bytes (sizeof)
struct _PS_PROTECTION
{
    union
    {
        UCHAR Level;                                                        //0x0
        struct
        {
            UCHAR Type : 3;                                                   //0x0
            UCHAR Audit : 1;                                                  //0x0
            UCHAR Signer : 4;                                                 //0x0
        };
    };
};

//0x8 bytes (sizeof)
union _PS_INTERLOCKED_TIMER_DELAY_VALUES
{
    ULONGLONG DelayMs : 30;                                                   //0x0
    ULONGLONG CoalescingWindowMs : 30;                                        //0x0
    ULONGLONG Reserved : 1;                                                   //0x0
    ULONGLONG NewTimerWheel : 1;                                              //0x0
    ULONGLONG Retry : 1;                                                      //0x0
    ULONGLONG Locked : 1;                                                     //0x0
    ULONGLONG All;                                                          //0x0
};

//0x8 bytes (sizeof)
struct _JOBOBJECT_WAKE_FILTER
{
    ULONG HighEdgeFilter;                                                   //0x0
    ULONG LowEdgeFilter;                                                    //0x4
};

//0x30 bytes (sizeof)
struct _PS_PROCESS_WAKE_INFORMATION
{
    ULONGLONG NotificationChannel;                                          //0x0
    ULONG WakeCounters[ 7 ];                                                  //0x8
    struct _JOBOBJECT_WAKE_FILTER WakeFilter;                               //0x24
    ULONG NoWakeCounter;                                                    //0x2c
};

//0x10 bytes (sizeof)
struct _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
{
    struct _RTL_AVL_TREE Tree;                                              //0x0
    struct _EX_PUSH_LOCK Lock;                                              //0x8
};

//0x4 bytes (sizeof)
union _KE_PROCESS_CONCURRENCY_COUNT
{
    ULONG Fraction : 20;                                                      //0x0
    ULONG Count : 12;                                                         //0x0
    ULONG AllFields;                                                        //0x0
};

//0x8 bytes (sizeof)
struct _KE_IDEAL_PROCESSOR_SET_BREAKPOINTS
{
    union _KE_PROCESS_CONCURRENCY_COUNT Low;                                //0x0
    union _KE_PROCESS_CONCURRENCY_COUNT High;                               //0x4
};

//0x118 bytes (sizeof)
struct _KE_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK
{
    union _KE_PROCESS_CONCURRENCY_COUNT ExpectedConcurrencyCount;           //0x0
    struct _KE_IDEAL_PROCESSOR_SET_BREAKPOINTS Breakpoints;                 //0x4
    union
    {
        ULONG ConcurrencyCountFixed : 1;                                      //0xc
        ULONG AllFlags;                                                     //0xc
    } AssignmentFlags;                                                      //0xc
    struct _KAFFINITY_EX IdealProcessorSets;                                //0x10
};

//0x8 bytes (sizeof)
struct __EX_RUNDOWN_REF
{
    union
    {
        ULONGLONG Count;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};

//0xb80 bytes (sizeof)
struct _EPROCESS
{
    struct __KPROCESS Pcb;                                                   //0x0
    struct _EX_PUSH_LOCK ProcessLock;                                       //0x438
    VOID* UniqueProcessId;                                                  //0x440
    struct _LIST_ENTRY ActiveProcessLinks;                                  //0x448
    struct __EX_RUNDOWN_REF RundownProtect;                                  //0x458
    union
    {
        ULONG Flags2;                                                       //0x460
        struct
        {
            ULONG JobNotReallyActive : 1;                                     //0x460
            ULONG AccountingFolded : 1;                                       //0x460
            ULONG NewProcessReported : 1;                                     //0x460
            ULONG ExitProcessReported : 1;                                    //0x460
            ULONG ReportCommitChanges : 1;                                    //0x460
            ULONG LastReportMemory : 1;                                       //0x460
            ULONG ForceWakeCharge : 1;                                        //0x460
            ULONG CrossSessionCreate : 1;                                     //0x460
            ULONG NeedsHandleRundown : 1;                                     //0x460
            ULONG RefTraceEnabled : 1;                                        //0x460
            ULONG PicoCreated : 1;                                            //0x460
            ULONG EmptyJobEvaluated : 1;                                      //0x460
            ULONG DefaultPagePriority : 3;                                    //0x460
            ULONG PrimaryTokenFrozen : 1;                                     //0x460
            ULONG ProcessVerifierTarget : 1;                                  //0x460
            ULONG RestrictSetThreadContext : 1;                               //0x460
            ULONG AffinityPermanent : 1;                                      //0x460
            ULONG AffinityUpdateEnable : 1;                                   //0x460
            ULONG PropagateNode : 1;                                          //0x460
            ULONG ExplicitAffinity : 1;                                       //0x460
            ULONG ProcessExecutionState : 2;                                  //0x460
            ULONG EnableReadVmLogging : 1;                                    //0x460
            ULONG EnableWriteVmLogging : 1;                                   //0x460
            ULONG FatalAccessTerminationRequested : 1;                        //0x460
            ULONG DisableSystemAllowedCpuSet : 1;                             //0x460
            ULONG ProcessStateChangeRequest : 2;                              //0x460
            ULONG ProcessStateChangeInProgress : 1;                           //0x460
            ULONG InPrivate : 1;                                              //0x460
        };
    };
    union
    {
        ULONG Flags;                                                        //0x464
        struct
        {
            ULONG CreateReported : 1;                                         //0x464
            ULONG NoDebugInherit : 1;                                         //0x464
            ULONG ProcessExiting : 1;                                         //0x464
            ULONG ProcessDelete : 1;                                          //0x464
            ULONG ManageExecutableMemoryWrites : 1;                           //0x464
            ULONG VmDeleted : 1;                                              //0x464
            ULONG OutswapEnabled : 1;                                         //0x464
            ULONG Outswapped : 1;                                             //0x464
            ULONG FailFastOnCommitFail : 1;                                   //0x464
            ULONG Wow64VaSpace4Gb : 1;                                        //0x464
            ULONG AddressSpaceInitialized : 2;                                //0x464
            ULONG SetTimerResolution : 1;                                     //0x464
            ULONG BreakOnTermination : 1;                                     //0x464
            ULONG DeprioritizeViews : 1;                                      //0x464
            ULONG WriteWatch : 1;                                             //0x464
            ULONG ProcessInSession : 1;                                       //0x464
            ULONG OverrideAddressSpace : 1;                                   //0x464
            ULONG HasAddressSpace : 1;                                        //0x464
            ULONG LaunchPrefetched : 1;                                       //0x464
            ULONG Background : 1;                                             //0x464
            ULONG VmTopDown : 1;                                              //0x464
            ULONG ImageNotifyDone : 1;                                        //0x464
            ULONG PdeUpdateNeeded : 1;                                        //0x464
            ULONG VdmAllowed : 1;                                             //0x464
            ULONG ProcessRundown : 1;                                         //0x464
            ULONG ProcessInserted : 1;                                        //0x464
            ULONG DefaultIoPriority : 3;                                      //0x464
            ULONG ProcessSelfDelete : 1;                                      //0x464
            ULONG SetTimerResolutionLink : 1;                                 //0x464
        };
    };
    union _LARGE_INTEGER CreateTime;                                        //0x468
    ULONGLONG ProcessQuotaUsage[ 2 ];                                         //0x470
    ULONGLONG ProcessQuotaPeak[ 2 ];                                          //0x480
    ULONGLONG PeakVirtualSize;                                              //0x490
    ULONGLONG VirtualSize;                                                  //0x498
    struct _LIST_ENTRY SessionProcessLinks;                                 //0x4a0
    union
    {
        VOID* ExceptionPortData;                                            //0x4b0
        ULONGLONG ExceptionPortValue;                                       //0x4b0
        ULONGLONG ExceptionPortState : 3;                                     //0x4b0
    };
    struct _EX_FAST_REF Token;                                              //0x4b8
    ULONGLONG MmReserved;                                                   //0x4c0
    struct _EX_PUSH_LOCK AddressCreationLock;                               //0x4c8
    struct _EX_PUSH_LOCK PageTableCommitmentLock;                           //0x4d0
    struct _ETHREAD* RotateInProgress;                                      //0x4d8
    struct _ETHREAD* ForkInProgress;                                        //0x4e0
    struct _EJOB* volatile CommitChargeJob;                                 //0x4e8
    struct _RTL_AVL_TREE CloneRoot;                                         //0x4f0
    volatile ULONGLONG NumberOfPrivatePages;                                //0x4f8
    volatile ULONGLONG NumberOfLockedPages;                                 //0x500
    VOID* Win32Process;                                                     //0x508
    struct _EJOB* volatile Job;                                             //0x510
    VOID* SectionObject;                                                    //0x518
    VOID* SectionBaseAddress;                                               //0x520
    ULONG Cookie;                                                           //0x528
    struct _PAGEFAULT_HISTORY* WorkingSetWatch;                             //0x530
    VOID* Win32WindowStation;                                               //0x538
    VOID* InheritedFromUniqueProcessId;                                     //0x540
    volatile ULONGLONG OwnerProcessId;                                      //0x548
    struct _PEB* Peb;                                                       //0x550
    struct _MM_SESSION_SPACE* Session;                                      //0x558
    VOID* Spare1;                                                           //0x560
    struct _EPROCESS_QUOTA_BLOCK* QuotaBlock;                               //0x568
    struct _HANDLE_TABLE* ObjectTable;                                      //0x570
    VOID* DebugPort;                                                        //0x578
    struct _EWOW64PROCESS* WoW64Process;                                    //0x580
    struct _EX_FAST_REF DeviceMap;                                          //0x588
    VOID* EtwDataSource;                                                    //0x590
    ULONGLONG PageDirectoryPte;                                             //0x598
    struct _FILE_OBJECT* ImageFilePointer;                                  //0x5a0
    UCHAR ImageFileName[ 15 ];                                                //0x5a8
    UCHAR PriorityClass;                                                    //0x5b7
    VOID* SecurityPort;                                                     //0x5b8
    struct _SE_AUDIT_PROCESS_CREATION_INFO SeAuditProcessCreationInfo;      //0x5c0
    struct _LIST_ENTRY JobLinks;                                            //0x5c8
    VOID* HighestUserAddress;                                               //0x5d8
    struct _LIST_ENTRY ThreadListHead;                                      //0x5e0
    volatile ULONG ActiveThreads;                                           //0x5f0
    ULONG ImagePathHash;                                                    //0x5f4
    ULONG DefaultHardErrorProcessing;                                       //0x5f8
    LONG LastThreadExitStatus;                                              //0x5fc
    struct _EX_FAST_REF PrefetchTrace;                                      //0x600
    VOID* LockedPagesList;                                                  //0x608
    union _LARGE_INTEGER ReadOperationCount;                                //0x610
    union _LARGE_INTEGER WriteOperationCount;                               //0x618
    union _LARGE_INTEGER OtherOperationCount;                               //0x620
    union _LARGE_INTEGER ReadTransferCount;                                 //0x628
    union _LARGE_INTEGER WriteTransferCount;                                //0x630
    union _LARGE_INTEGER OtherTransferCount;                                //0x638
    ULONGLONG CommitChargeLimit;                                            //0x640
    volatile ULONGLONG CommitCharge;                                        //0x648
    volatile ULONGLONG CommitChargePeak;                                    //0x650
    struct _MMSUPPORT_FULL Vm;                                              //0x680
    struct _LIST_ENTRY MmProcessLinks;                                      //0x7c0
    ULONG ModifiedPageCount;                                                //0x7d0
    LONG ExitStatus;                                                        //0x7d4
    struct _RTL_AVL_TREE* VadRoot;                                          //0x7d8
    VOID* VadHint;                                                          //0x7e0
    ULONGLONG VadCount;                                                     //0x7e8
    volatile ULONGLONG VadPhysicalPages;                                    //0x7f0
    ULONGLONG VadPhysicalPagesLimit;                                        //0x7f8
    struct _ALPC_PROCESS_CONTEXT AlpcContext;                               //0x800
    struct _LIST_ENTRY TimerResolutionLink;                                 //0x820
    struct _PO_DIAG_STACK_RECORD* TimerResolutionStackRecord;               //0x830
    ULONG RequestedTimerResolution;                                         //0x838
    ULONG SmallestTimerResolution;                                          //0x83c
    union _LARGE_INTEGER ExitTime;                                          //0x840
    struct _INVERTED_FUNCTION_TABLE* InvertedFunctionTable;                 //0x848
    struct _EX_PUSH_LOCK InvertedFunctionTableLock;                         //0x850
    ULONG ActiveThreadsHighWatermark;                                       //0x858
    ULONG LargePrivateVadCount;                                             //0x85c
    struct _EX_PUSH_LOCK ThreadListLock;                                    //0x860
    VOID* WnfContext;                                                       //0x868
    struct _EJOB* ServerSilo;                                               //0x870
    UCHAR SignatureLevel;                                                   //0x878
    UCHAR SectionSignatureLevel;                                            //0x879
    struct _PS_PROTECTION Protection;                                       //0x87a
    UCHAR HangCount : 3;                                                      //0x87b
    UCHAR GhostCount : 3;                                                     //0x87b
    UCHAR PrefilterException : 1;                                             //0x87b
    union
    {
        ULONG Flags3;                                                       //0x87c
        struct
        {
            ULONG Minimal : 1;                                                //0x87c
            ULONG ReplacingPageRoot : 1;                                      //0x87c
            ULONG Crashed : 1;                                                //0x87c
            ULONG JobVadsAreTracked : 1;                                      //0x87c
            ULONG VadTrackingDisabled : 1;                                    //0x87c
            ULONG AuxiliaryProcess : 1;                                       //0x87c
            ULONG SubsystemProcess : 1;                                       //0x87c
            ULONG IndirectCpuSets : 1;                                        //0x87c
            ULONG RelinquishedCommit : 1;                                     //0x87c
            ULONG HighGraphicsPriority : 1;                                   //0x87c
            ULONG CommitFailLogged : 1;                                       //0x87c
            ULONG ReserveFailLogged : 1;                                      //0x87c
            ULONG SystemProcess : 1;                                          //0x87c
            ULONG HideImageBaseAddresses : 1;                                 //0x87c
            ULONG AddressPolicyFrozen : 1;                                    //0x87c
            ULONG ProcessFirstResume : 1;                                     //0x87c
            ULONG ForegroundExternal : 1;                                     //0x87c
            ULONG ForegroundSystem : 1;                                       //0x87c
            ULONG HighMemoryPriority : 1;                                     //0x87c
            ULONG EnableProcessSuspendResumeLogging : 1;                      //0x87c
            ULONG EnableThreadSuspendResumeLogging : 1;                       //0x87c
            ULONG SecurityDomainChanged : 1;                                  //0x87c
            ULONG SecurityFreezeComplete : 1;                                 //0x87c
            ULONG VmProcessorHost : 1;                                        //0x87c
            ULONG VmProcessorHostTransition : 1;                              //0x87c
            ULONG AltSyscall : 1;                                             //0x87c
            ULONG TimerResolutionIgnore : 1;                                  //0x87c
            ULONG DisallowUserTerminate : 1;                                  //0x87c
            ULONG EnableProcessRemoteExecProtectVmLogging : 1;                //0x87c
            ULONG EnableProcessLocalExecProtectVmLogging : 1;                 //0x87c
        };
    };
    LONG DeviceAsid;                                                        //0x880
    VOID* SvmData;                                                          //0x888
    struct _EX_PUSH_LOCK SvmProcessLock;                                    //0x890
    ULONGLONG SvmLock;                                                      //0x898
    struct _LIST_ENTRY SvmProcessDeviceListHead;                            //0x8a0
    ULONGLONG LastFreezeInterruptTime;                                      //0x8b0
    struct _PROCESS_DISK_COUNTERS* DiskCounters;                            //0x8b8
    VOID* PicoContext;                                                      //0x8c0
    VOID* EnclaveTable;                                                     //0x8c8
    ULONGLONG EnclaveNumber;                                                //0x8d0
    struct _EX_PUSH_LOCK EnclaveLock;                                       //0x8d8
    ULONG HighPriorityFaultsAllowed;                                        //0x8e0
    struct _PO_PROCESS_ENERGY_CONTEXT* EnergyContext;                       //0x8e8
    VOID* VmContext;                                                        //0x8f0
    ULONGLONG SequenceNumber;                                               //0x8f8
    ULONGLONG CreateInterruptTime;                                          //0x900
    ULONGLONG CreateUnbiasedInterruptTime;                                  //0x908
    ULONGLONG TotalUnbiasedFrozenTime;                                      //0x910
    ULONGLONG LastAppStateUpdateTime;                                       //0x918
    ULONGLONG LastAppStateUptime : 61;                                        //0x920
    ULONGLONG LastAppState : 3;                                               //0x920
    volatile ULONGLONG SharedCommitCharge;                                  //0x928
    struct _EX_PUSH_LOCK SharedCommitLock;                                  //0x930
    struct _LIST_ENTRY SharedCommitLinks;                                   //0x938
    union
    {
        struct
        {
            ULONGLONG AllowedCpuSets;                                       //0x948
            ULONGLONG DefaultCpuSets;                                       //0x950
        };
        struct
        {
            ULONGLONG* AllowedCpuSetsIndirect;                              //0x948
            ULONGLONG* DefaultCpuSetsIndirect;                              //0x950
        };
    };
    VOID* DiskIoAttribution;                                                //0x958
    VOID* DxgProcess;                                                       //0x960
    ULONG Win32KFilterSet;                                                  //0x968
    USHORT Machine;                                                         //0x96c
    USHORT Spare0;                                                          //0x96e
    union _PS_INTERLOCKED_TIMER_DELAY_VALUES ProcessTimerDelay;     //0x970
    volatile ULONG KTimerSets;                                              //0x978
    volatile ULONG KTimer2Sets;                                             //0x97c
    volatile ULONG ThreadTimerSets;                                         //0x980
    ULONGLONG VirtualTimerListLock;                                         //0x988
    struct _LIST_ENTRY VirtualTimerListHead;                                //0x990
    union
    {
        struct _WNF_STATE_NAME WakeChannel;                                 //0x9a0
        struct _PS_PROCESS_WAKE_INFORMATION WakeInfo;                       //0x9a0
    };
    union
    {
        ULONG MitigationFlags;                                              //0x9d0
        struct
        {
            ULONG ControlFlowGuardEnabled : 1;                                //0x9d0
            ULONG ControlFlowGuardExportSuppressionEnabled : 1;               //0x9d0
            ULONG ControlFlowGuardStrict : 1;                                 //0x9d0
            ULONG DisallowStrippedImages : 1;                                 //0x9d0
            ULONG ForceRelocateImages : 1;                                    //0x9d0
            ULONG HighEntropyASLREnabled : 1;                                 //0x9d0
            ULONG StackRandomizationDisabled : 1;                             //0x9d0
            ULONG ExtensionPointDisable : 1;                                  //0x9d0
            ULONG DisableDynamicCode : 1;                                     //0x9d0
            ULONG DisableDynamicCodeAllowOptOut : 1;                          //0x9d0
            ULONG DisableDynamicCodeAllowRemoteDowngrade : 1;                 //0x9d0
            ULONG AuditDisableDynamicCode : 1;                                //0x9d0
            ULONG DisallowWin32kSystemCalls : 1;                              //0x9d0
            ULONG AuditDisallowWin32kSystemCalls : 1;                         //0x9d0
            ULONG EnableFilteredWin32kAPIs : 1;                               //0x9d0
            ULONG AuditFilteredWin32kAPIs : 1;                                //0x9d0
            ULONG DisableNonSystemFonts : 1;                                  //0x9d0
            ULONG AuditNonSystemFontLoading : 1;                              //0x9d0
            ULONG PreferSystem32Images : 1;                                   //0x9d0
            ULONG ProhibitRemoteImageMap : 1;                                 //0x9d0
            ULONG AuditProhibitRemoteImageMap : 1;                            //0x9d0
            ULONG ProhibitLowILImageMap : 1;                                  //0x9d0
            ULONG AuditProhibitLowILImageMap : 1;                             //0x9d0
            ULONG SignatureMitigationOptIn : 1;                               //0x9d0
            ULONG AuditBlockNonMicrosoftBinaries : 1;                         //0x9d0
            ULONG AuditBlockNonMicrosoftBinariesAllowStore : 1;               //0x9d0
            ULONG LoaderIntegrityContinuityEnabled : 1;                       //0x9d0
            ULONG AuditLoaderIntegrityContinuity : 1;                         //0x9d0
            ULONG EnableModuleTamperingProtection : 1;                        //0x9d0
            ULONG EnableModuleTamperingProtectionNoInherit : 1;               //0x9d0
            ULONG RestrictIndirectBranchPrediction : 1;                       //0x9d0
            ULONG IsolateSecurityDomain : 1;                                  //0x9d0
        } MitigationFlagsValues;                                            //0x9d0
    };
    union
    {
        ULONG MitigationFlags2;                                             //0x9d4
        struct
        {
            ULONG EnableExportAddressFilter : 1;                              //0x9d4
            ULONG AuditExportAddressFilter : 1;                               //0x9d4
            ULONG EnableExportAddressFilterPlus : 1;                          //0x9d4
            ULONG AuditExportAddressFilterPlus : 1;                           //0x9d4
            ULONG EnableRopStackPivot : 1;                                    //0x9d4
            ULONG AuditRopStackPivot : 1;                                     //0x9d4
            ULONG EnableRopCallerCheck : 1;                                   //0x9d4
            ULONG AuditRopCallerCheck : 1;                                    //0x9d4
            ULONG EnableRopSimExec : 1;                                       //0x9d4
            ULONG AuditRopSimExec : 1;                                        //0x9d4
            ULONG EnableImportAddressFilter : 1;                              //0x9d4
            ULONG AuditImportAddressFilter : 1;                               //0x9d4
            ULONG DisablePageCombine : 1;                                     //0x9d4
            ULONG SpeculativeStoreBypassDisable : 1;                          //0x9d4
            ULONG CetUserShadowStacks : 1;                                    //0x9d4
            ULONG AuditCetUserShadowStacks : 1;                               //0x9d4
            ULONG AuditCetUserShadowStacksLogged : 1;                         //0x9d4
            ULONG UserCetSetContextIpValidation : 1;                          //0x9d4
            ULONG AuditUserCetSetContextIpValidation : 1;                     //0x9d4
            ULONG AuditUserCetSetContextIpValidationLogged : 1;               //0x9d4
            ULONG CetUserShadowStacksStrictMode : 1;                          //0x9d4
            ULONG BlockNonCetBinaries : 1;                                    //0x9d4
            ULONG BlockNonCetBinariesNonEhcont : 1;                           //0x9d4
            ULONG AuditBlockNonCetBinaries : 1;                               //0x9d4
            ULONG AuditBlockNonCetBinariesLogged : 1;                         //0x9d4
            ULONG XtendedControlFlowGuard : 1;                                //0x9d4
            ULONG AuditXtendedControlFlowGuard : 1;                           //0x9d4
            ULONG PointerAuthUserIp : 1;                                      //0x9d4
            ULONG AuditPointerAuthUserIp : 1;                                 //0x9d4
            ULONG AuditPointerAuthUserIpLogged : 1;                           //0x9d4
            ULONG CetDynamicApisOutOfProcOnly : 1;                            //0x9d4
            ULONG UserCetSetContextIpValidationRelaxedMode : 1;               //0x9d4
        } MitigationFlags2Values;                                           //0x9d4
    };
    VOID* PartitionObject;                                                  //0x9d8
    ULONGLONG SecurityDomain;                                               //0x9e0
    ULONGLONG ParentSecurityDomain;                                         //0x9e8
    VOID* CoverageSamplerContext;                                           //0x9f0
    VOID* MmHotPatchContext;                                                //0x9f8
    struct _KE_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK IdealProcessorAssignmentBlock; //0xa00
    struct _RTL_AVL_TREE DynamicEHContinuationTargetsTree;                  //0xb18
    struct _EX_PUSH_LOCK DynamicEHContinuationTargetsLock;                  //0xb20
    struct _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES DynamicEnforcedCetCompatibleRanges; //0xb28
    ULONG DisabledComponentFlags;                                           //0xb38
    volatile LONG PageCombineSequence;                                      //0xb3c
    struct _EX_PUSH_LOCK EnableOptionalXStateFeaturesLock;                  //0xb40
};