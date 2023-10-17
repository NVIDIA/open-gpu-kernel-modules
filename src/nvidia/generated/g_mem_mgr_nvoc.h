#ifndef _G_MEM_MGR_NVOC_H_
#define _G_MEM_MGR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "g_mem_mgr_nvoc.h"

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include "core/core.h"
#include "core/info_block.h"
#include "gpu/eng_state.h"

#include "gpu/mem_mgr/fbsr.h"
#include "gpu/gpu.h"

#include "mem_mgr/mem.h"

#include "mem_mgr/mem_list.h"

#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "containers/map.h"
#include "gpu/mem_mgr/heap_base.h"
#include "mem_mgr/vaspace.h"

#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"

struct CeUtils;

#ifndef __NVOC_CLASS_CeUtils_TYPEDEF__
#define __NVOC_CLASS_CeUtils_TYPEDEF__
typedef struct CeUtils CeUtils;
#endif /* __NVOC_CLASS_CeUtils_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtils
#define __nvoc_class_id_CeUtils 0x8b8bae
#endif /* __nvoc_class_id_CeUtils */



typedef volatile struct _cl906f_tag1 Nv906fControl;
typedef struct KERNEL_MIG_GPU_INSTANCE KERNEL_MIG_GPU_INSTANCE;

typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64              offset;
} TRANSFER_SURFACE;

// Memory transfer engine types.
typedef enum
{
    TRANSFER_TYPE_PROCESSOR = 0,       // CPU/GSP/DPU depending on execution context
    TRANSFER_TYPE_GSP_DMA,             // Dma engine internal to GSP
    TRANSFER_TYPE_CE,                  // Copy Engine using CeUtils channel
    TRANSFER_TYPE_CE_PRI,              // Copy Engine using PRIs
    TRANSFER_TYPE_BAR0,                // Copy using BAR0 PRAMIN
} TRANSFER_TYPE;

#define TRANSFER_FLAGS_NONE                   0
#define TRANSFER_FLAGS_DEFER_FLUSH            NVBIT32(0) // Applicable only for write operations
#define TRANSFER_FLAGS_SHADOW_ALLOC           NVBIT32(1) // Applicable only for non-PROCESSOR transfers
#define TRANSFER_FLAGS_SHADOW_INIT_MEM        NVBIT32(2) // Applicable only for non-PROCESSOR transfers
#define TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING NVBIT32(3) // Require long lived PROCESSOR mapping
#define TRANSFER_FLAGS_DESTROY_MAPPING        NVBIT32(4) // Destroy any cached mappings when complete
#define TRANSFER_FLAGS_USE_BAR1               NVBIT32(5) // Use only BAR1 for PROCESSOR transfers
#define TRANSFER_FLAGS_PREFER_CE              NVBIT32(6) // Use CE if possible (BAR0 on simulation for perf)
#define TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH     NVBIT32(7) // Defer CE flush; only affects PRI CE operations

// Protection flags: at most 1 may be set, none means READ_WRITE by default
#define TRANSFER_FLAGS_MAP_PROTECT_READABLE   NVBIT32(8) // Transfer is only reading data
#define TRANSFER_FLAGS_MAP_PROTECT_WRITEABLE  NVBIT32(9) // Transfer is only writing data

typedef struct
{
    NvU32   bar1Size;
    NvU32   bar1AvailSize;
    NvU32   bankSwizzleAlignment;
    NvU32   bar1MaxContigAvailSize;
} GETBAR1INFO, *PGETBAR1INFO;

//
// RM Default PTE kind
// Bug #2242255, introducing the RM Default kind to allow sharing memory between
// different architectures especially between Turing+ and Pre Turing chips
//
#define RM_DEFAULT_PTE_KIND                 0x100

typedef enum
{
    FB_IS_KIND_Z,                           // Kind is a Z buffer
    FB_IS_KIND_ZBC,                         // Zero bandwidth clears
    FB_IS_KIND_ZBC_ALLOWS_1,                // ZBC with 1 bit of tag
    FB_IS_KIND_ZBC_ALLOWS_2,                // ZBC with 2 bits of tag
    FB_IS_KIND_ZBC_ALLOWS_4,                // ZBC with 4 bits of tag
    FB_IS_KIND_COMPRESSIBLE,                // Any compressible kind
    FB_IS_KIND_COMPRESSIBLE_1,              // Compressible with 1 comp tag bit
    FB_IS_KIND_COMPRESSIBLE_2,              // Compressible with 2 comp tag bits
    FB_IS_KIND_COMPRESSIBLE_4,              // Compressible with 4 comp tag bits
    FB_IS_KIND_SUPPORTED,                   // Kind is supported
    FB_IS_KIND_DISALLOW_PLC,                // Kind Disallows PLC
} FB_IS_KIND_OP;

// Surface compression parameters
typedef struct COMPR_INFO
{
    // Surface kind; if not compressed, following parameters are ignored
    NvU32  kind;

    // Compression page shift;  0 if kind is uncompressed
    NvU32  compPageShift;

    //
    // Are comptags are determined per-page by PA?
    // If set, following parameters are ignored
    //
    NvBool bPhysBasedComptags;

    // see GMMU_COMPR_INFO
    NvU32  compPageIndexLo;
    NvU32  compPageIndexHi;
    NvU32  compTagLineMin;
    NvU32  compTagLineMultiplier;
} COMPR_INFO;

//
// Individual entry for logging Fb reserved use-cases
//
typedef struct NV_FB_RSVD_BLOCK_LOG_ENTRY
{
    // Owner tag associated with reservation block
    NvU32 ownerId;

    // Size of the memory reserved
    NvU64 rsvdSize;
} NV_FB_RSVD_BLOCK_LOG_ENTRY;

// Total number of FB internal reservation enries
#define NV_FB_RSVD_BLOCK_LOG_ENTRY_MAX 10U

//
// Structure for logging Fb reserved use-cases
//
typedef struct NV_FB_RSVD_BLOCK_LOG_INFO
{
    // Counter for logging entries
    NvU32 counter;

    // List of all reserved entries
    NV_FB_RSVD_BLOCK_LOG_ENTRY rsvdBlockList[NV_FB_RSVD_BLOCK_LOG_ENTRY_MAX];
} NV_FB_RSVD_BLOCK_LOG_INFO;

//
// Macro for initializing reserved block log data
//
#define NV_FB_RSVD_BLOCK_LOG_INIT(pMem)                                                                    \
        {                                                                                                  \
            ((pMem)->rsvdBlockInfo).counter = 0;                                                           \
            for (NvU32 i = 0; i < NV_FB_RSVD_BLOCK_LOG_ENTRY_MAX; i++)                                     \
            {                                                                                              \
                ((pMem)->rsvdBlockInfo).rsvdBlockList[i].ownerId = 0;                                      \
                ((pMem)->rsvdBlockInfo).rsvdBlockList[i].rsvdSize = 0;                                     \
            }                                                                                              \
        }

//
// Macro for adding new reserved block entry to the list
// If unable to log, marks the status as NV_ERR_NO_MEMORY otherwise keeps it unchanged
//
#define NV_FB_RSVD_BLOCK_LOG_ENTRY_ADD(status, pMem, tag, size)                                            \
        {                                                                                                  \
            if(((pMem)->rsvdBlockInfo).counter < NV_FB_RSVD_BLOCK_LOG_ENTRY_MAX)                           \
            {                                                                                              \
                ((pMem)->rsvdBlockInfo).rsvdBlockList[((pMem)->rsvdBlockInfo).counter].ownerId = (tag);    \
                ((pMem)->rsvdBlockInfo).rsvdBlockList[((pMem)->rsvdBlockInfo).counter].rsvdSize = (size);  \
                (((pMem)->rsvdBlockInfo).counter)++;                                                       \
            }                                                                                              \
            else                                                                                           \
            {                                                                                              \
                status = NV_ERR_NO_MEMORY;                                                                 \
            }                                                                                              \
        }

//
// Fixed Channel Properties for Memutils Object
//

typedef NV_STATUS FbScrubCallback(OBJGPU *);

#define BLOCK_INDEX_FROM_ADDR(addr,size)             ((NvU32)((addr) >> size))
#define BLOCK_ADDR_FROM_INDEX(idx,size)              (((NvU64)(idx)) << size)

#define MEMUTILS_SIZE_PER_BLOCK_INBYTES               (0x68)
#define MEMUTILS_TOTAL_SIZE_PER_BLOCK_INBYTES         (0x60) //(COPY + PB SEMA)
#define MEMUTILS_TD_BLOCKS_PER_CHUNK                   0x40

#define BLOCK_INDEX_FROM_ADDR(addr,size)              ((NvU32)((addr) >> size))
#define BLOCK_ADDR_FROM_INDEX(idx,size)               (((NvU64)(idx)) << size)

#define MEMUTILS_NUM_PAYLOAD_SEMAPHORES               (2)
#define MEMUTILS_NUM_GPFIFIO_ENTRIES                  (32)
// PB size should be a multiple of chunk size
#define MEMUTILS_CHANNEL_PB_SIZE                      (0x10 * MEMUTILS_SIZE_PER_BLOCK_INBYTES  * \
                                                       MEMUTILS_TD_BLOCKS_PER_CHUNK)
#define MEMUTILS_CHANNEL_SEMAPHORE_SIZE               (4 * MEMUTILS_NUM_PAYLOAD_SEMAPHORES)
#define MEMUTILS_CHANNEL_NOTIFIER_SIZE                (sizeof(NvNotification) * 1)

// offset and line length should be a multiple of 4KB
#define MEMUTIL_SCRUB_OFFSET_ALIGNMENT        (4 * 1024)
#define MEMUTIL_SCRUB_LINE_LENGTH_ALIGNMENT   (4 * 1024)

typedef enum {
    CE_SCRUBBER_CHANNEL,
    FAST_SCRUBBER_CHANNEL,
    COPY_CHANNEL,
    SWL_SCRUBBER_CHANNEL,
    MAX_CHANNEL_TYPE
} CHANNEL_KIND;

// This will be moved to a channel object next
typedef struct OBJCHANNEL
{
    NvHandle                        deviceId;           // Device Handle
    NvHandle                        physMemId;          // Memory Handle
    NvHandle                        channelId;          // Channel Handle
    NvHandle                        subdeviceId;        // Subdevice Handle
    NvHandle                        errNotifierIdVirt;
    NvHandle                        errNotifierIdPhys;
    NvHandle                        engineObjectId;
    NvHandle                        eventId;
    NvHandle                        pushBufferId;
    NvHandle                        bitMapSemPhysId;
    NvHandle                        bitMapSemVirtId;
    NvHandle                        hVASpaceId;           // VASpace handle, when scrubber in virtual mode
    NvHandle                        hFbAlias;             // Used only for virtual channels
    NvHandle                        hFbAliasVA;
    // to be moved later

    NvU32                           channelSize;
    NvU32                           channelNumGpFifioEntries;
    NvU32                           channelPbSize;
    NvU32                           channelNotifierSize;
    NvU32                           methodSizePerBlock;
    NvU32                           semaOffset;
    NvU32                           finishPayloadOffset;
    NvU32                           authTagBufSemaOffset;
    NvU32                           finishPayload;
    NvBool                          isChannelSynchronized;
    NvBool                          isProgressChecked;
//
// RM internal channels are created as privileged channels (physical address access) by default
// For MMU Bug: 2739505, we need to switch to use channels in non-privileged mode.
//
    NvBool                          bUseVasForCeCopy;         // set to NV_TRUE, when scrubber operates in virtual address
    struct RsClient                        *pRsClient;
    struct OBJVASPACE                     *pVAS;
    NvU32                           engineType;
    NvU64                           startFbOffset;
    NvU64                           fbSize;
    NvU64                           fbAliasVA;
    NvU64                           vaStartOffset;
    // to be moved to a separate object later

    NvU32                           *pBlockPendingState;
    NvU32                           *pBlockDoneState;
    NvU32                           blockCount;
    NvHandle                        hClient;
    NvBool                          bClientAllocated;
    NvU64                           pbGpuVA;
    NvU64                           pbGpuBitMapVA;
    NvU64                           pbGpuNotifierVA;
    MEMORY_DESCRIPTOR               *pUserdMemdesc;
    MEMORY_DESCRIPTOR               *pChannelBufferMemdesc;
    MEMORY_DESCRIPTOR               *pErrNotifierMemdesc;
    NvU8                            *pbCpuVA;
    NvU8                            *pbBitMapVA;
    Nv906fControl                   *pControlGPFifo;
    NvU32                           classEngineID;
    NVOS10_EVENT_KERNEL_CALLBACK_EX callback;
    NvU32                           state;
    NvU32                           hTdCopyClass;
    NvU32                           sec2Class;
    NvU32                           minBlockSize;
    NvU32                           maxBlockSize;
    NvU32                           channelPutOffset;
    NvU8                            blockShift;
    NvU32                           lastPayloadPushed;
    NvBool                          isChannelActive;
    NvU32                           workSubmitToken;
    //
    // Work submit token read from notifier memory.
    //
    NvNotification                  *pTokenFromNotifier;
    NvU32                           lastSubmittedEntry;
    NvHandle                        lastAllocatedHandle;
    CHANNEL_KIND                    type;

    // Used for Volta+
    NvHandle                        doorbellRegionHandle;
    NvU8                            *pDoorbellRegion;
    NvU32                           *pDoorbellRegisterOffset;
    NvBool                          bUseDoorbellRegister;
    NvHandle                        hUserD;
    NvBool                          bClientUserd;

    OBJGPU                         *pGpu;
    NvU32                           ceId;

    // Used by Partition Scrubber
    KERNEL_MIG_GPU_INSTANCE         *pKernelMIGGpuInstance;
    NvHandle                        hPartitionRef;
} OBJCHANNEL, *POBJCHANNEL;

#define NV_METHOD(SubCh, Method, Num)                        \
    (DRF_DEF(906F, _DMA_INCR, _OPCODE,     _VALUE)         | \
     DRF_NUM(906F, _DMA_INCR, _COUNT,      Num)            | \
     DRF_NUM(906F, _DMA_INCR, _SUBCHANNEL, SubCh)          | \
     DRF_NUM(906F, _DMA_INCR, _ADDRESS,    (Method) >> 2))

#define PUSH_DATA(Data) MEM_WR32(ptr++, (Data))

#define PUSH_PAIR(SubCh, Method, Data)            \
    do                                            \
    {                                             \
        PUSH_DATA(NV_METHOD(SubCh, (Method), 1)); \
        PUSH_DATA((Data));                        \
    } while (0)

//-----------------------------------------------------------------------------

typedef struct
{
    NvU32  lastSubmittedBlock;
    NvBool isTopDownScrubber;
    NvBool isActive;
    NvU32  scrubberState;
    NvU32  currentFbRegion;
    NvU32  startBlock;
    NvU32  endBlock;
    NvU32 *pPendingBitMap;
    NvU32 *pDoneBitMap;
    NvU32  blockCount;
    struct OBJCE *pCe;
    NvBool bCeInUse;
    OBJCHANNEL tdHeapState;
    OBJCHANNEL allocationScrubberState;
} OBJSCRUB, *POBJSCRUB;

typedef struct
{
    NvU64   base;               // Base/start address of the region
    NvU64   limit;              // Last/end address of region
    NvU64   rsvdSize;           // Memory RM may be required to allocate in this region
    NvBool  bRsvdRegion;        // Reserved region -- not publicly usable
    NvU32   performance;        // Relative performance.  Higher is faster
    NvBool  bSupportCompressed; // Support compressed kinds
    NvBool  bSupportISO;        // Support ISO (display, cursor, video) surfaces
    NvBool  bProtected;         // Represents a protected region of memory.
    NvBool  bInternalHeap;      // PMA:Used for internal RM allocations
    NvBool  bLostOnSuspend;     // Not required to be Saved during S/R.
} FB_REGION_DESCRIPTOR, *PFB_REGION_DESCRIPTOR;

#define MAX_FB_REGIONS  16

// Maximum number of contexts created for WHQL test WDDM Max Contexts
#define WHQL_TEST_MAX_CONTEXTS          100

// Object 'get' macros for FB relative object retrievals.
#define MEMORY_MANAGER_GET_HEAP(p)          ((p)->pHeap)

typedef struct _def_fb_mem_node
{
    struct _def_fb_mem_node *pNext;

    NvBool bFreeDescriptor;
    PMEMORY_DESCRIPTOR pMemDesc;

} FB_MEM_NODE, *PFB_MEM_NODE;

// defines for MemoryManager::fbsrReservedRanges
#define MAX_FBSR_RESERVED_REGIONS                   6           // Max. Memory descriptors for RM Instance memory
#define FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE    0
#define FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE     1
#define FBSR_RESERVED_INST_MEMORY_GSP_HEAP          2
#define FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR       3
#define FBSR_RESERVED_INST_MEMORY_GSP_WPR           4
#define FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE     5

/*!
 * MemoryManager provides the root memory management of GPU video memory.
 * External entities might provide suballocators on top of MemoryManager.
 *
 * MemoryManager can have static information on the memory system (e.g.: list of
 * kinds, etc), however MemoryManager does not have direct access to the GPU
 * memory system (e.g.: BAR0 registers). It relies on KernelMemorySystem for
 * operations on the memory system.
 *
 * MemoryManager is instantiated in VGPU guest/GSP Client as well as the VGPU
 * host/GSP-RM.
 */

#define MEM_MGR_STUB_ORIN(...)

#ifdef NVOC_MEM_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RM_POOL_ALLOC_MEM_RESERVE_INFO;

struct __nvoc_inner_struc_MemoryManager_1__ {
    NvU64 fbUsableMemSize;
    NvU64 fbTotalMemSizeMb;
    NvU64 fbAddrSpaceSizeMb;
    NvU64 mapRamSizeMb;
    NvU64 fbOverrideSizeMb;
    NvU64 reservedMemSize;
    NvU32 numFBRegions;
    FB_REGION_DESCRIPTOR fbRegion[16];
    NvU32 numFBRegionPriority;
    NvU64 maxFBPSize;
    NvU64 minFBPSize;
    NvU32 fbRegionPriority[16];
    NvU64 ReservedConsoleDispMemSize;
    PMEMORY_DESCRIPTOR pReservedConsoleMemDesc;
    NvU32 lowerRangeMag;
    NvU32 lowerRangeScale;
    NvU32 middleRangeMag;
    NvU32 middleRangeScale;
    NvU32 upperRangeMag;
    NvU32 upperRangeScale;
};

struct MIG_MEMORY_PARTITIONING_INFO {
    struct NV_RANGE partitionableMemoryRange;
    struct NV_RANGE partitionableBar1Range;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
};


struct MemoryManager {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct MemoryManager *__nvoc_pbase_MemoryManager;
    NV_STATUS (*__memmgrConstructEngine__)(OBJGPU *, struct MemoryManager *, ENGDESCRIPTOR);
    NV_STATUS (*__memmgrStatePreInitLocked__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrStateInitLocked__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrStateLoad__)(OBJGPU *, struct MemoryManager *, NvU32);
    NV_STATUS (*__memmgrStatePostLoad__)(OBJGPU *, struct MemoryManager *, NvU32);
    NV_STATUS (*__memmgrStatePreUnload__)(OBJGPU *, struct MemoryManager *, NvU32);
    NV_STATUS (*__memmgrStateUnload__)(OBJGPU *, struct MemoryManager *, NvU32);
    void (*__memmgrStateDestroy__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrMemUtilsSec2CtxInit__)(OBJGPU *, struct MemoryManager *, OBJCHANNEL *);
    NvBool (*__memmgrMemUtilsCheckMemoryFastScrubEnable__)(OBJGPU *, struct MemoryManager *, NvU32, NvBool, RmPhysAddr, NvU32, NV_ADDRESS_SPACE);
    NV_STATUS (*__memmgrAllocDetermineAlignment__)(OBJGPU *, struct MemoryManager *, NvU64 *, NvU64 *, NvU64, NvU32, NvU32, NvU32, NvU64);
    NvU64 (*__memmgrGetMaxContextSize__)(OBJGPU *, struct MemoryManager *);
    void (*__memmgrScrubRegistryOverrides__)(OBJGPU *, struct MemoryManager *);
    NvU32 (*__memmgrGetPteKindBl__)(OBJGPU *, struct MemoryManager *);
    NvU32 (*__memmgrGetPteKindPitch__)(OBJGPU *, struct MemoryManager *);
    NvU32 (*__memmgrChooseKindCompressC__)(OBJGPU *, struct MemoryManager *, FB_ALLOC_PAGE_FORMAT *);
    NV_STATUS (*__memmgrGetFlaKind__)(OBJGPU *, struct MemoryManager *, NvU32 *);
    NvBool (*__memmgrIsApertureSupportedByFla__)(OBJGPU *, struct MemoryManager *, NV_ADDRESS_SPACE);
    NvU32 (*__memmgrDetermineComptag__)(OBJGPU *, struct MemoryManager *, RmPhysAddr);
    NV_STATUS (*__memmgrCheckReservedMemorySize__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrReadMmuLock__)(OBJGPU *, struct MemoryManager *, NvBool *, NvU64 *, NvU64 *);
    NV_STATUS (*__memmgrBlockMemLockedMemory__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrInsertUnprotectedRegionAtBottomOfFb__)(OBJGPU *, struct MemoryManager *, NvU64 *);
    void (*__memmgrGetDisablePlcKind__)(struct MemoryManager *, NvU32 *);
    void (*__memmgrEnableDynamicPageOfflining__)(OBJGPU *, struct MemoryManager *);
    NV_STATUS (*__memmgrGetBlackListPages__)(OBJGPU *, struct MemoryManager *, BLACKLIST_ADDRESS *, NvU32 *);
    NV_STATUS (*__memmgrStatePreLoad__)(POBJGPU, struct MemoryManager *, NvU32);
    NV_STATUS (*__memmgrStatePostUnload__)(POBJGPU, struct MemoryManager *, NvU32);
    NV_STATUS (*__memmgrStateInitUnlocked__)(POBJGPU, struct MemoryManager *);
    void (*__memmgrInitMissing__)(POBJGPU, struct MemoryManager *);
    NV_STATUS (*__memmgrStatePreInitUnlocked__)(POBJGPU, struct MemoryManager *);
    NvBool (*__memmgrIsPresent__)(POBJGPU, struct MemoryManager *);
    NvBool bFbsrWddmModeEnabled;
    NvBool bFbRegionsSupported;
    NvBool bPmaSupportedOnPlatform;
    NvBool bPmaEnabled;
    NvBool bPmaInitialized;
    NvBool bPmaForcePersistence;
    NvBool bPmaAddrTree;
    NvBool bClientPageTablesPmaManaged;
    NvBool bScanoutSysmem;
    NvBool bMixedDensityFbp;
    NvBool bPreferSlowRegion;
    NvBool bPersistentStandbyBuffer;
    NvBool bEnableFbsrPagedDma;
    NvBool bDisallowSplitLowerMemory;
    NvBool bIgnoreUpperMemory;
    NvBool bSmallPageCompression;
    NvBool bSysmemCompressionSupportDef;
    NvBool bBug1698088IncreaseRmReserveMemoryWar;
    NvBool bBug2301372IncreaseRmReserveMemoryWar;
    NvBool bEnableFbsrFileMode;
    NvBool bEnableDynamicPageOfflining;
    NvBool bVgpuPmaSupport;
    NvBool bScrubChannelSetupInProgress;
    NvBool bBug3922001DisableCtxBufOnSim;
    NvBool bEnableDynamicGranularityPageArrays;
    NvBool bAllowNoncontiguousAllocation;
    NvBool bLocalEgmSupported;
    NvBool bLocalEgmEnabled;
    NvU32 localEgmPeerId;
    NvS32 localEgmNodeId;
    NvU64 localEgmBasePhysAddr;
    NvU64 localEgmSize;
    NvBool bEccInterleavedVidmemScrub;
    NvBool bScrubberInitialized;
    NvBool bAllowSysmemHugePages;
    NvBool bEccScrubOverride;
    NvU64 sysmemPageSize;
    struct Heap *pHeap;
    NvBool bScrubOnFreeEnabled;
    NvBool bFastScrubberEnabled;
    NvBool bDisableAsyncScrubforMods;
    NvBool bUseVasForCeMemoryOps;
    NvBool bRmExecutingEccScrub;
    NvBool bBug1441072EccScrubWar;
    NvU64 heapStartOffset;
    NvU64 rsvdMemoryBase;
    NvU32 rsvdMemorySize;
    struct CeUtils *pCeUtils;
    struct CeUtils *pCeUtilsSuspended;
    NvBool bDisableGlobalCeUtils;
    OBJSCRUB eccScrubberState;
    struct __nvoc_inner_struc_MemoryManager_1__ Ram;
    NvU32 PteKindOverride;
    NvU64 scratchDwordOffset;
    NvU32 zbcSurfaces;
    NvU64 overrideInitHeapMin;
    NvU64 overrideHeapMax;
    NvU64 rsvdMemorySizeIncrement;
    struct OBJFBSR *pFbsr[8];
    struct OBJFBSR *pActiveFbsr;
    NvU32 fbsrStartMode;
    NvU32 fixedFbsrModesMask;
    MEMORY_DESCRIPTOR *fbsrReservedRanges[6];
    PFB_MEM_NODE pMemHeadNode;
    PFB_MEM_NODE pMemTailNode;
    struct RM_POOL_ALLOC_MEM_RESERVE_INFO *pPageLevelReserve;
    struct MIG_MEMORY_PARTITIONING_INFO MIGMemoryPartitioningInfo;
    NV_FB_RSVD_BLOCK_LOG_INFO rsvdBlockInfo;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NvHandle hThirdPartyP2P;
    NvBool bMonitoredFenceSupported;
    NvBool b64BitSemaphoresSupported;
    NvBool bGenericKindSupport;
    NvBool bLocalizedMemPoolEnabled;
};

#ifndef __NVOC_CLASS_MemoryManager_TYPEDEF__
#define __NVOC_CLASS_MemoryManager_TYPEDEF__
typedef struct MemoryManager MemoryManager;
#endif /* __NVOC_CLASS_MemoryManager_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryManager
#define __nvoc_class_id_MemoryManager 0x22ad47
#endif /* __nvoc_class_id_MemoryManager */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryManager;

#define __staticCast_MemoryManager(pThis) \
    ((pThis)->__nvoc_pbase_MemoryManager)

#ifdef __nvoc_mem_mgr_h_disabled
#define __dynamicCast_MemoryManager(pThis) ((MemoryManager*)NULL)
#else //__nvoc_mem_mgr_h_disabled
#define __dynamicCast_MemoryManager(pThis) \
    ((MemoryManager*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryManager)))
#endif //__nvoc_mem_mgr_h_disabled

#define PDB_PROP_MEMMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_MEMMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryManager(MemoryManager**, Dynamic*, NvU32);
#define __objCreate_MemoryManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_MemoryManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define memmgrConstructEngine(pGpu, pMemoryManager, arg0) memmgrConstructEngine_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrStatePreInitLocked(pGpu, pMemoryManager) memmgrStatePreInitLocked_DISPATCH(pGpu, pMemoryManager)
#define memmgrStateInitLocked(pGpu, pMemoryManager) memmgrStateInitLocked_DISPATCH(pGpu, pMemoryManager)
#define memmgrStateLoad(pGpu, pMemoryManager, arg0) memmgrStateLoad_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrStatePostLoad(pGpu, pMemoryManager, arg0) memmgrStatePostLoad_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrStatePreUnload(pGpu, pMemoryManager, arg0) memmgrStatePreUnload_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrStateUnload(pGpu, pMemoryManager, arg0) memmgrStateUnload_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrStateDestroy(pGpu, pMemoryManager) memmgrStateDestroy_DISPATCH(pGpu, pMemoryManager)
#define memmgrMemUtilsSec2CtxInit(pGpu, pMemoryManager, arg0) memmgrMemUtilsSec2CtxInit_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrMemUtilsSec2CtxInit_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsSec2CtxInit_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrMemUtilsCheckMemoryFastScrubEnable(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4) memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4)
#define memmgrMemUtilsCheckMemoryFastScrubEnable_HAL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4) memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4)
#define memmgrAllocDetermineAlignment(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment) memmgrAllocDetermineAlignment_DISPATCH(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment)
#define memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment) memmgrAllocDetermineAlignment_DISPATCH(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment)
#define memmgrGetMaxContextSize(pGpu, pMemoryManager) memmgrGetMaxContextSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetMaxContextSize_HAL(pGpu, pMemoryManager) memmgrGetMaxContextSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubRegistryOverrides(pGpu, pMemoryManager) memmgrScrubRegistryOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubRegistryOverrides_HAL(pGpu, pMemoryManager) memmgrScrubRegistryOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindBl(pGpu, pMemoryManager) memmgrGetPteKindBl_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindBl_HAL(pGpu, pMemoryManager) memmgrGetPteKindBl_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindPitch(pGpu, pMemoryManager) memmgrGetPteKindPitch_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindPitch_HAL(pGpu, pMemoryManager) memmgrGetPteKindPitch_DISPATCH(pGpu, pMemoryManager)
#define memmgrChooseKindCompressC(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressC_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrChooseKindCompressC_HAL(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressC_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrGetFlaKind(pGpu, pMemoryManager, arg0) memmgrGetFlaKind_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrGetFlaKind_HAL(pGpu, pMemoryManager, arg0) memmgrGetFlaKind_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrIsApertureSupportedByFla(pGpu, pMemoryManager, arg0) memmgrIsApertureSupportedByFla_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrIsApertureSupportedByFla_HAL(pGpu, pMemoryManager, arg0) memmgrIsApertureSupportedByFla_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrDetermineComptag(pGpu, pMemoryManager, arg0) memmgrDetermineComptag_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrDetermineComptag_HAL(pGpu, pMemoryManager, arg0) memmgrDetermineComptag_DISPATCH(pGpu, pMemoryManager, arg0)
#define memmgrCheckReservedMemorySize(pGpu, pMemoryManager) memmgrCheckReservedMemorySize_DISPATCH(pGpu, pMemoryManager)
#define memmgrCheckReservedMemorySize_HAL(pGpu, pMemoryManager) memmgrCheckReservedMemorySize_DISPATCH(pGpu, pMemoryManager)
#define memmgrReadMmuLock(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi) memmgrReadMmuLock_DISPATCH(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi)
#define memmgrReadMmuLock_HAL(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi) memmgrReadMmuLock_DISPATCH(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi)
#define memmgrBlockMemLockedMemory(pGpu, pMemoryManager) memmgrBlockMemLockedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrBlockMemLockedMemory_HAL(pGpu, pMemoryManager) memmgrBlockMemLockedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrInsertUnprotectedRegionAtBottomOfFb(pGpu, pMemoryManager, pSize) memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(pGpu, pMemoryManager, pSize)
#define memmgrInsertUnprotectedRegionAtBottomOfFb_HAL(pGpu, pMemoryManager, pSize) memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(pGpu, pMemoryManager, pSize)
#define memmgrGetDisablePlcKind(pMemoryManager, pteKind) memmgrGetDisablePlcKind_DISPATCH(pMemoryManager, pteKind)
#define memmgrGetDisablePlcKind_HAL(pMemoryManager, pteKind) memmgrGetDisablePlcKind_DISPATCH(pMemoryManager, pteKind)
#define memmgrEnableDynamicPageOfflining(pGpu, pMemoryManager) memmgrEnableDynamicPageOfflining_DISPATCH(pGpu, pMemoryManager)
#define memmgrEnableDynamicPageOfflining_HAL(pGpu, pMemoryManager) memmgrEnableDynamicPageOfflining_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetBlackListPages(pGpu, pMemoryManager, pBlAddrs, pCount) memmgrGetBlackListPages_DISPATCH(pGpu, pMemoryManager, pBlAddrs, pCount)
#define memmgrGetBlackListPages_HAL(pGpu, pMemoryManager, pBlAddrs, pCount) memmgrGetBlackListPages_DISPATCH(pGpu, pMemoryManager, pBlAddrs, pCount)
#define memmgrStatePreLoad(pGpu, pEngstate, arg0) memmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define memmgrStatePostUnload(pGpu, pEngstate, arg0) memmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define memmgrStateInitUnlocked(pGpu, pEngstate) memmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define memmgrInitMissing(pGpu, pEngstate) memmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define memmgrStatePreInitUnlocked(pGpu, pEngstate) memmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define memmgrIsPresent(pGpu, pEngstate) memmgrIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS memmgrSavePowerMgmtState_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSavePowerMgmtState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSavePowerMgmtState(pGpu, pMemoryManager) memmgrSavePowerMgmtState_KERNEL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrSavePowerMgmtState_HAL(pGpu, pMemoryManager) memmgrSavePowerMgmtState(pGpu, pMemoryManager)

NV_STATUS memmgrRestorePowerMgmtState_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrRestorePowerMgmtState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrRestorePowerMgmtState(pGpu, pMemoryManager) memmgrRestorePowerMgmtState_KERNEL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrRestorePowerMgmtState_HAL(pGpu, pMemoryManager) memmgrRestorePowerMgmtState(pGpu, pMemoryManager)

NvU64 memmgrDeterminePageSize_IMPL(struct MemoryManager *pMemoryManager, NvHandle hClient, NvU64 memSize, NvU32 memFormat, NvU32 pageFormatFlags, NvU32 *pRetAttr, NvU32 *pRetAttr2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrDeterminePageSize(struct MemoryManager *pMemoryManager, NvHandle hClient, NvU64 memSize, NvU32 memFormat, NvU32 pageFormatFlags, NvU32 *pRetAttr, NvU32 *pRetAttr2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDeterminePageSize(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2) memmgrDeterminePageSize_IMPL(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrDeterminePageSize_HAL(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2) memmgrDeterminePageSize(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2)

static inline NV_STATUS memmgrReserveConsoleRegion_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_REGION_DESCRIPTOR *arg0) {
    return NV_OK;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveConsoleRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_REGION_DESCRIPTOR *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReserveConsoleRegion(pGpu, pMemoryManager, arg0) memmgrReserveConsoleRegion_56cd7a(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrReserveConsoleRegion_HAL(pGpu, pMemoryManager, arg0) memmgrReserveConsoleRegion(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrAllocateConsoleRegion_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_REGION_DESCRIPTOR *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocateConsoleRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_REGION_DESCRIPTOR *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocateConsoleRegion(pGpu, pMemoryManager, arg0) memmgrAllocateConsoleRegion_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrAllocateConsoleRegion_HAL(pGpu, pMemoryManager, arg0) memmgrAllocateConsoleRegion(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrGetKindComprForGpu_KERNEL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg0, OBJGPU *pGpu, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetKindComprForGpu(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg0, OBJGPU *pGpu, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetKindComprForGpu(pMemoryManager, arg0, pGpu, offset, kind, pComprInfo) memmgrGetKindComprForGpu_KERNEL(pMemoryManager, arg0, pGpu, offset, kind, pComprInfo)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetKindComprForGpu_HAL(pMemoryManager, arg0, pGpu, offset, kind, pComprInfo) memmgrGetKindComprForGpu(pMemoryManager, arg0, pGpu, offset, kind, pComprInfo)

static inline NV_STATUS memmgrScrubInit_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrScrubInit(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubInit(pGpu, pMemoryManager) memmgrScrubInit_56cd7a(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubInit_HAL(pGpu, pMemoryManager) memmgrScrubInit(pGpu, pMemoryManager)

NV_STATUS memmgrScrubHandlePostSchedulingEnable_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrScrubHandlePostSchedulingEnable(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubHandlePostSchedulingEnable(pGpu, pMemoryManager) memmgrScrubHandlePostSchedulingEnable_GP100(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubHandlePostSchedulingEnable_HAL(pGpu, pMemoryManager) memmgrScrubHandlePostSchedulingEnable(pGpu, pMemoryManager)

static inline void memmgrGetScrubState_f2d351(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0, NvU64 *arg1, NvBool *arg2) {
    NV_ASSERT_PRECOMP(0);
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetScrubState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0, NvU64 *arg1, NvBool *arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetScrubState(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrGetScrubState_f2d351(pGpu, pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetScrubState_HAL(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrGetScrubState(pGpu, pMemoryManager, arg0, arg1, arg2)

static inline void memmgrScrubInternalRegions_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubInternalRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubInternalRegions(pGpu, pMemoryManager) memmgrScrubInternalRegions_b3696a(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubInternalRegions_HAL(pGpu, pMemoryManager) memmgrScrubInternalRegions(pGpu, pMemoryManager)

static inline NvBool memmgrEccScrubInProgress_491d52(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return ((NvBool)(0 != 0));
}

NvBool memmgrEccScrubInProgress_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrEccScrubInProgress(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrEccScrubInProgress(pGpu, pMemoryManager) memmgrEccScrubInProgress_491d52(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrEccScrubInProgress_HAL(pGpu, pMemoryManager) memmgrEccScrubInProgress(pGpu, pMemoryManager)

static inline void memmgrAsyncScrubRegion_f2d351(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0, NvU64 arg1) {
    NV_ASSERT_PRECOMP(0);
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrAsyncScrubRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0, NvU64 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAsyncScrubRegion(pGpu, pMemoryManager, arg0, arg1) memmgrAsyncScrubRegion_f2d351(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrAsyncScrubRegion_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrAsyncScrubRegion(pGpu, pMemoryManager, arg0, arg1)

NV_STATUS memmgrScrubHandlePreSchedulingDisable_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrScrubHandlePreSchedulingDisable(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubHandlePreSchedulingDisable(pGpu, pMemoryManager) memmgrScrubHandlePreSchedulingDisable_GP100(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubHandlePreSchedulingDisable_HAL(pGpu, pMemoryManager) memmgrScrubHandlePreSchedulingDisable(pGpu, pMemoryManager)

static inline void memmgrScrubDestroy_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubDestroy(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubDestroy(pGpu, pMemoryManager) memmgrScrubDestroy_b3696a(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubDestroy_HAL(pGpu, pMemoryManager) memmgrScrubDestroy(pGpu, pMemoryManager)

static inline void memmgrScrubMemory_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0, NvU64 arg1) {
    return;
}

void memmgrScrubMemory_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0, NvU64 arg1);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0, NvU64 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubMemory(pGpu, pMemoryManager, arg0, arg1) memmgrScrubMemory_b3696a(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubMemory_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrScrubMemory(pGpu, pMemoryManager, arg0, arg1)

NV_STATUS memmgrMemUtilsMemSetBlocking_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, RmPhysAddr arg1, NvU64 arg2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsMemSetBlocking(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, RmPhysAddr arg1, NvU64 arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsMemSetBlocking(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrMemUtilsMemSetBlocking_GM107(pGpu, pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsMemSetBlocking_HAL(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrMemUtilsMemSetBlocking(pGpu, pMemoryManager, arg0, arg1, arg2)

NV_STATUS memmgrMemUtilsMemSet_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, RmPhysAddr arg1, NvU64 arg2, NvU32 arg3, NvU32 *arg4);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsMemSet(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, RmPhysAddr arg1, NvU64 arg2, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsMemSet(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4) memmgrMemUtilsMemSet_GM107(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsMemSet_HAL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4) memmgrMemUtilsMemSet(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4)

NV_STATUS memmgrMemUtilsAllocateEccScrubber_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsAllocateEccScrubber(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsAllocateEccScrubber(pGpu, pMemoryManager, arg0) memmgrMemUtilsAllocateEccScrubber_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsAllocateEccScrubber_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsAllocateEccScrubber(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrMemUtilsAllocateEccAllocScrubber_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsAllocateEccAllocScrubber(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsAllocateEccAllocScrubber(pGpu, pMemoryManager, arg0) memmgrMemUtilsAllocateEccAllocScrubber_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsAllocateEccAllocScrubber_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsAllocateEccAllocScrubber(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrMemUtilsChannelInitialize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsChannelInitialize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsChannelInitialize(pGpu, pMemoryManager, arg0) memmgrMemUtilsChannelInitialize_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsChannelInitialize(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrMemUtilsCopyEngineInitialize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsCopyEngineInitialize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsCopyEngineInitialize(pGpu, pMemoryManager, arg0) memmgrMemUtilsCopyEngineInitialize_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsCopyEngineInitialize(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrMemUtilsGetCopyEngineClass_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pClass);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsGetCopyEngineClass(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pClass) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsGetCopyEngineClass(pGpu, pMemoryManager, pClass) memmgrMemUtilsGetCopyEngineClass_GM107(pGpu, pMemoryManager, pClass)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsGetCopyEngineClass_HAL(pGpu, pMemoryManager, pClass) memmgrMemUtilsGetCopyEngineClass(pGpu, pMemoryManager, pClass)

NV_STATUS memmgrMemUtilsCreateMemoryAlias_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsCreateMemoryAlias(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsCreateMemoryAlias(pGpu, pMemoryManager, arg0) memmgrMemUtilsCreateMemoryAlias_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrMemUtilsCreateMemoryAlias_HAL(pGpu, pMemoryManager, arg0) memmgrMemUtilsCreateMemoryAlias(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrAllocHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocHal(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocHal(pGpu, pMemoryManager, pFbAllocInfo) memmgrAllocHal_GM107(pGpu, pMemoryManager, pFbAllocInfo)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrAllocHal_HAL(pGpu, pMemoryManager, pFbAllocInfo) memmgrAllocHal(pGpu, pMemoryManager, pFbAllocInfo)

NV_STATUS memmgrFreeHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo, PRMTIMEOUT pTimeout);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFreeHal(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo, PRMTIMEOUT pTimeout) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFreeHal(pGpu, pMemoryManager, pFbAllocInfo, pTimeout) memmgrFreeHal_GM107(pGpu, pMemoryManager, pFbAllocInfo, pTimeout)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrFreeHal_HAL(pGpu, pMemoryManager, pFbAllocInfo, pTimeout) memmgrFreeHal(pGpu, pMemoryManager, pFbAllocInfo, pTimeout)

static inline NV_STATUS memmgrUpdateSurfaceCompression_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *arg0, NvBool arg1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrUpdateSurfaceCompression(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *arg0, NvBool arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrUpdateSurfaceCompression(pGpu, pMemoryManager, arg0, arg1) memmgrUpdateSurfaceCompression_5baef9(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrUpdateSurfaceCompression_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrUpdateSurfaceCompression(pGpu, pMemoryManager, arg0, arg1)

NV_STATUS memmgrGetBankPlacementData_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pBankPlacementLowData);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetBankPlacementData(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pBankPlacementLowData) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetBankPlacementData(pGpu, pMemoryManager, pBankPlacementLowData) memmgrGetBankPlacementData_GM107(pGpu, pMemoryManager, pBankPlacementLowData)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetBankPlacementData_HAL(pGpu, pMemoryManager, pBankPlacementLowData) memmgrGetBankPlacementData(pGpu, pMemoryManager, pBankPlacementLowData)

void memmgrDirtyForPmTest_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool partialDirty);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrDirtyForPmTest(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool partialDirty) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDirtyForPmTest(pGpu, pMemoryManager, partialDirty) memmgrDirtyForPmTest_GM107(pGpu, pMemoryManager, partialDirty)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrDirtyForPmTest_HAL(pGpu, pMemoryManager, partialDirty) memmgrDirtyForPmTest(pGpu, pMemoryManager, partialDirty)

NvU32 memmgrGetReservedHeapSizeMb_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetReservedHeapSizeMb(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetReservedHeapSizeMb(pGpu, pMemoryManager) memmgrGetReservedHeapSizeMb_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetReservedHeapSizeMb_HAL(pGpu, pMemoryManager) memmgrGetReservedHeapSizeMb(pGpu, pMemoryManager)

NV_STATUS memmgrInitFbRegionsHal_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrInitFbRegionsHal_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitFbRegionsHal(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitFbRegionsHal(pGpu, pMemoryManager) memmgrInitFbRegionsHal_56cd7a(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrInitFbRegionsHal_HAL(pGpu, pMemoryManager) memmgrInitFbRegionsHal(pGpu, pMemoryManager)

void memmgrHandleSizeOverrides_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrHandleSizeOverrides(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrHandleSizeOverrides(pGpu, pMemoryManager) memmgrHandleSizeOverrides_GP100(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrHandleSizeOverrides_HAL(pGpu, pMemoryManager) memmgrHandleSizeOverrides(pGpu, pMemoryManager)

NV_STATUS memmgrFinishHandleSizeOverrides_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFinishHandleSizeOverrides(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFinishHandleSizeOverrides(pGpu, pMemoryManager) memmgrFinishHandleSizeOverrides_GP100(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrFinishHandleSizeOverrides_HAL(pGpu, pMemoryManager) memmgrFinishHandleSizeOverrides(pGpu, pMemoryManager)

NV_STATUS memmgrGetBAR1InfoForDevice_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, PGETBAR1INFO bar1Info);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetBAR1InfoForDevice(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, PGETBAR1INFO bar1Info) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, bar1Info) memmgrGetBAR1InfoForDevice_GM107(pGpu, pMemoryManager, pDevice, bar1Info)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetBAR1InfoForDevice_HAL(pGpu, pMemoryManager, pDevice, bar1Info) memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, bar1Info)

static inline NvU64 memmgrGetFbTaxSize_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetFbTaxSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetFbTaxSize(pGpu, pMemoryManager) memmgrGetFbTaxSize_4a4dee(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager) memmgrGetFbTaxSize(pGpu, pMemoryManager)

NvU64 memmgrGetVgpuHostRmReservedFb_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);

NvU64 memmgrGetVgpuHostRmReservedFb_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);

NvU64 memmgrGetVgpuHostRmReservedFb_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetVgpuHostRmReservedFb(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetVgpuHostRmReservedFb(pGpu, pMemoryManager, vgpuTypeId) memmgrGetVgpuHostRmReservedFb_KERNEL(pGpu, pMemoryManager, vgpuTypeId)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetVgpuHostRmReservedFb_HAL(pGpu, pMemoryManager, vgpuTypeId) memmgrGetVgpuHostRmReservedFb(pGpu, pMemoryManager, vgpuTypeId)

NvU64 memmgrGetRsvdSizeForSr_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetRsvdSizeForSr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetRsvdSizeForSr(pGpu, pMemoryManager) memmgrGetRsvdSizeForSr_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetRsvdSizeForSr_HAL(pGpu, pMemoryManager) memmgrGetRsvdSizeForSr(pGpu, pMemoryManager)

static inline NvBool memmgrVerifyDepthSurfaceAttrs_cbe027(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 arg1) {
    return ((NvBool)(0 == 0));
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrVerifyDepthSurfaceAttrs(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrVerifyDepthSurfaceAttrs(pMemoryManager, arg0, arg1) memmgrVerifyDepthSurfaceAttrs_cbe027(pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrVerifyDepthSurfaceAttrs_HAL(pMemoryManager, arg0, arg1) memmgrVerifyDepthSurfaceAttrs(pMemoryManager, arg0, arg1)

NV_STATUS memmgrAllocMemToSaveVgaWorkspace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg0, MEMORY_DESCRIPTOR **arg1);

static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg0, MEMORY_DESCRIPTOR **arg1) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg0, MEMORY_DESCRIPTOR **arg1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg0, MEMORY_DESCRIPTOR **arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocMemToSaveVgaWorkspace(pGpu, pMemoryManager, arg0, arg1) memmgrAllocMemToSaveVgaWorkspace_5baef9(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrAllocMemToSaveVgaWorkspace_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrAllocMemToSaveVgaWorkspace(pGpu, pMemoryManager, arg0, arg1)

NvBool memmgrComparePhysicalAddresses_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvU64 arg1, NvU32 arg2, NvU64 arg3);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrComparePhysicalAddresses(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvU64 arg1, NvU32 arg2, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrComparePhysicalAddresses(pGpu, pMemoryManager, arg0, arg1, arg2, arg3) memmgrComparePhysicalAddresses_GM107(pGpu, pMemoryManager, arg0, arg1, arg2, arg3)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrComparePhysicalAddresses_HAL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3) memmgrComparePhysicalAddresses(pGpu, pMemoryManager, arg0, arg1, arg2, arg3)

RmPhysAddr memmgrGetInvalidOffset_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline RmPhysAddr memmgrGetInvalidOffset(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    RmPhysAddr ret;
    portMemSet(&ret, 0, sizeof(RmPhysAddr));
    return ret;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetInvalidOffset(pGpu, pMemoryManager) memmgrGetInvalidOffset_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetInvalidOffset_HAL(pGpu, pMemoryManager) memmgrGetInvalidOffset(pGpu, pMemoryManager)

NvU32 memmgrGetAddrSpaceSizeMB_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetAddrSpaceSizeMB(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetAddrSpaceSizeMB(pGpu, pMemoryManager) memmgrGetAddrSpaceSizeMB_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetAddrSpaceSizeMB_HAL(pGpu, pMemoryManager) memmgrGetAddrSpaceSizeMB(pGpu, pMemoryManager)

NvU32 memmgrGetUsableMemSizeMB_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetUsableMemSizeMB(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetUsableMemSizeMB(pGpu, pMemoryManager) memmgrGetUsableMemSizeMB_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetUsableMemSizeMB_HAL(pGpu, pMemoryManager) memmgrGetUsableMemSizeMB(pGpu, pMemoryManager)

NV_STATUS memmgrGetSurfacePhysAttr_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *pMemory, NvU64 *pOffset, NvU32 *pMemAperture, NvU32 *pMemKind, NvU32 *pComprOffset, NvU32 *pComprKind, NvU32 *pLineMin, NvU32 *pLineMax, NvU32 *pZCullId, NvU32 *pGpuCacheAttr, NvU32 *pGpuP2PCacheAttr, NvU64 *contigSegmentSize);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetSurfacePhysAttr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *pMemory, NvU64 *pOffset, NvU32 *pMemAperture, NvU32 *pMemKind, NvU32 *pComprOffset, NvU32 *pComprKind, NvU32 *pLineMin, NvU32 *pLineMax, NvU32 *pZCullId, NvU32 *pGpuCacheAttr, NvU32 *pGpuP2PCacheAttr, NvU64 *contigSegmentSize) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetSurfacePhysAttr(pGpu, pMemoryManager, pMemory, pOffset, pMemAperture, pMemKind, pComprOffset, pComprKind, pLineMin, pLineMax, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr, contigSegmentSize) memmgrGetSurfacePhysAttr_GM107(pGpu, pMemoryManager, pMemory, pOffset, pMemAperture, pMemKind, pComprOffset, pComprKind, pLineMin, pLineMax, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr, contigSegmentSize)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetSurfacePhysAttr_HAL(pGpu, pMemoryManager, pMemory, pOffset, pMemAperture, pMemKind, pComprOffset, pComprKind, pLineMin, pLineMax, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr, contigSegmentSize) memmgrGetSurfacePhysAttr(pGpu, pMemoryManager, pMemory, pOffset, pMemAperture, pMemKind, pComprOffset, pComprKind, pLineMin, pLineMax, pZCullId, pGpuCacheAttr, pGpuP2PCacheAttr, contigSegmentSize)

static inline NvBool memmgrVerifyComprAttrs_cbe027(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 arg1, NvU32 arg2) {
    return ((NvBool)(0 == 0));
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrVerifyComprAttrs(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrVerifyComprAttrs(pMemoryManager, arg0, arg1, arg2) memmgrVerifyComprAttrs_cbe027(pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrVerifyComprAttrs_HAL(pMemoryManager, arg0, arg1, arg2) memmgrVerifyComprAttrs(pMemoryManager, arg0, arg1, arg2)

NvBool memmgrIsKindCompressible_TU102(struct MemoryManager *pMemoryManager, NvU32 arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsKindCompressible(struct MemoryManager *pMemoryManager, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrIsKindCompressible(pMemoryManager, arg0) memmgrIsKindCompressible_TU102(pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrIsKindCompressible_HAL(pMemoryManager, arg0) memmgrIsKindCompressible(pMemoryManager, arg0)

static inline NvBool memmgrIsKindBlocklinear_491d52(struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsKindBlocklinear(struct MemoryManager *pMemoryManager, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrIsKindBlocklinear(pMemoryManager, arg0) memmgrIsKindBlocklinear_491d52(pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrIsKindBlocklinear_HAL(pMemoryManager, arg0) memmgrIsKindBlocklinear(pMemoryManager, arg0)

NvU32 memmgrChooseKindZ_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrChooseKindZ(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChooseKindZ(pGpu, pMemoryManager, arg0) memmgrChooseKindZ_TU102(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChooseKindZ_HAL(pGpu, pMemoryManager, arg0) memmgrChooseKindZ(pGpu, pMemoryManager, arg0)

NvU32 memmgrChooseKindCompressZ_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrChooseKindCompressZ(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChooseKindCompressZ(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressZ_TU102(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChooseKindCompressZ_HAL(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressZ(pGpu, pMemoryManager, arg0)

static inline NvU32 memmgrChooseKindCompressCForMS2_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return 0;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrChooseKindCompressCForMS2(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChooseKindCompressCForMS2(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressCForMS2_4a4dee(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChooseKindCompressCForMS2_HAL(pGpu, pMemoryManager, arg0) memmgrChooseKindCompressCForMS2(pGpu, pMemoryManager, arg0)

NvU32 memmgrGetUncompressedKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetUncompressedKind(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetUncompressedKind(pGpu, pMemoryManager, kind, releaseReacquire) memmgrGetUncompressedKind_TU102(pGpu, pMemoryManager, kind, releaseReacquire)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager, kind, releaseReacquire) memmgrGetUncompressedKind(pGpu, pMemoryManager, kind, releaseReacquire)

static inline NV_STATUS memmgrGetUncompressedKindForMS2_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 *arg1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetUncompressedKindForMS2(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetUncompressedKindForMS2(pGpu, pMemoryManager, arg0, arg1) memmgrGetUncompressedKindForMS2_5baef9(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetUncompressedKindForMS2_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrGetUncompressedKindForMS2(pGpu, pMemoryManager, arg0, arg1)

NV_STATUS memmgrChooseKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0, NvU32 arg1, NvU32 *arg2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrChooseKind(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0, NvU32 arg1, NvU32 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChooseKind(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrChooseKind_TU102(pGpu, pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChooseKind_HAL(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrChooseKind(pGpu, pMemoryManager, arg0, arg1, arg2)

NvBool memmgrIsKind_TU102(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg0, NvU32 arg1);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsKind(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrIsKind(pMemoryManager, arg0, arg1) memmgrIsKind_TU102(pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrIsKind_HAL(pMemoryManager, arg0, arg1) memmgrIsKind(pMemoryManager, arg0, arg1)

NvU32 memmgrGetMessageKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetMessageKind(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetMessageKind(pGpu, pMemoryManager) memmgrGetMessageKind_TU102(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetMessageKind_HAL(pGpu, pMemoryManager) memmgrGetMessageKind(pGpu, pMemoryManager)

NvU32 memmgrGetDefaultPteKindForNoHandle_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetDefaultPteKindForNoHandle(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetDefaultPteKindForNoHandle(pGpu, pMemoryManager) memmgrGetDefaultPteKindForNoHandle_TU102(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetDefaultPteKindForNoHandle_HAL(pGpu, pMemoryManager) memmgrGetDefaultPteKindForNoHandle(pGpu, pMemoryManager)

NvBool memmgrIsSurfaceBlockLinear_TU102(struct MemoryManager *pMemoryManager, Memory *arg0, NvU32 arg1, NvU32 arg2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsSurfaceBlockLinear(struct MemoryManager *pMemoryManager, Memory *arg0, NvU32 arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrIsSurfaceBlockLinear(pMemoryManager, arg0, arg1, arg2) memmgrIsSurfaceBlockLinear_TU102(pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrIsSurfaceBlockLinear_HAL(pMemoryManager, arg0, arg1, arg2) memmgrIsSurfaceBlockLinear(pMemoryManager, arg0, arg1, arg2)

NvU32 memmgrGetHwPteKindFromSwPteKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetHwPteKindFromSwPteKind(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetHwPteKindFromSwPteKind(pGpu, pMemoryManager, pteKind) memmgrGetHwPteKindFromSwPteKind_TU102(pGpu, pMemoryManager, pteKind)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetHwPteKindFromSwPteKind_HAL(pGpu, pMemoryManager, pteKind) memmgrGetHwPteKindFromSwPteKind(pGpu, pMemoryManager, pteKind)

NvU32 memmgrGetSwPteKindFromHwPteKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetSwPteKindFromHwPteKind(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetSwPteKindFromHwPteKind(pGpu, pMemoryManager, pteKind) memmgrGetSwPteKindFromHwPteKind_TU102(pGpu, pMemoryManager, pteKind)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetSwPteKindFromHwPteKind_HAL(pGpu, pMemoryManager, pteKind) memmgrGetSwPteKindFromHwPteKind(pGpu, pMemoryManager, pteKind)

void memmgrGetPteKindForScrubber_TU102(struct MemoryManager *pMemoryManager, NvU32 *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetPteKindForScrubber(struct MemoryManager *pMemoryManager, NvU32 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetPteKindForScrubber(pMemoryManager, arg0) memmgrGetPteKindForScrubber_TU102(pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetPteKindForScrubber_HAL(pMemoryManager, arg0) memmgrGetPteKindForScrubber(pMemoryManager, arg0)

NvU32 memmgrGetCtagOffsetFromParams_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetCtagOffsetFromParams(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetCtagOffsetFromParams(pGpu, pMemoryManager, arg0) memmgrGetCtagOffsetFromParams_TU102(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetCtagOffsetFromParams_HAL(pGpu, pMemoryManager, arg0) memmgrGetCtagOffsetFromParams(pGpu, pMemoryManager, arg0)

void memmgrSetCtagOffsetInParams_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0, NvU32 arg1);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrSetCtagOffsetInParams(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetCtagOffsetInParams(pGpu, pMemoryManager, arg0, arg1) memmgrSetCtagOffsetInParams_TU102(pGpu, pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrSetCtagOffsetInParams_HAL(pGpu, pMemoryManager, arg0, arg1) memmgrSetCtagOffsetInParams(pGpu, pMemoryManager, arg0, arg1)

void memmgrChannelPushSemaphoreMethodsBlock_GP100(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU64 arg1, NvU32 arg2, NvU32 **arg3);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrChannelPushSemaphoreMethodsBlock(struct MemoryManager *pMemoryManager, NvU32 arg0, NvU64 arg1, NvU32 arg2, NvU32 **arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChannelPushSemaphoreMethodsBlock(pMemoryManager, arg0, arg1, arg2, arg3) memmgrChannelPushSemaphoreMethodsBlock_GP100(pMemoryManager, arg0, arg1, arg2, arg3)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChannelPushSemaphoreMethodsBlock_HAL(pMemoryManager, arg0, arg1, arg2, arg3) memmgrChannelPushSemaphoreMethodsBlock(pMemoryManager, arg0, arg1, arg2, arg3)

void memmgrChannelPushAddressMethodsBlock_GP100(struct MemoryManager *pMemoryManager, NvBool arg0, NvU32 arg1, RmPhysAddr arg2, NvU32 **arg3);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrChannelPushAddressMethodsBlock(struct MemoryManager *pMemoryManager, NvBool arg0, NvU32 arg1, RmPhysAddr arg2, NvU32 **arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrChannelPushAddressMethodsBlock(pMemoryManager, arg0, arg1, arg2, arg3) memmgrChannelPushAddressMethodsBlock_GP100(pMemoryManager, arg0, arg1, arg2, arg3)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrChannelPushAddressMethodsBlock_HAL(pMemoryManager, arg0, arg1, arg2, arg3) memmgrChannelPushAddressMethodsBlock(pMemoryManager, arg0, arg1, arg2, arg3)

NV_STATUS memmgrScrubMapDoorbellRegion_GV100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrScrubMapDoorbellRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrScrubMapDoorbellRegion(pGpu, pMemoryManager, arg0) memmgrScrubMapDoorbellRegion_GV100(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrScrubMapDoorbellRegion_HAL(pGpu, pMemoryManager, arg0) memmgrScrubMapDoorbellRegion(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrSetAllocParameters_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetAllocParameters(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetAllocParameters(pGpu, pMemoryManager, pFbAllocInfo) memmgrSetAllocParameters_GM107(pGpu, pMemoryManager, pFbAllocInfo)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrSetAllocParameters_HAL(pGpu, pMemoryManager, pFbAllocInfo) memmgrSetAllocParameters(pGpu, pMemoryManager, pFbAllocInfo)

void memmgrCalcReservedFbSpaceForUVM_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrCalcReservedFbSpaceForUVM(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrCalcReservedFbSpaceForUVM(pGpu, pMemoryManager, arg0) memmgrCalcReservedFbSpaceForUVM_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrCalcReservedFbSpaceForUVM_HAL(pGpu, pMemoryManager, arg0) memmgrCalcReservedFbSpaceForUVM(pGpu, pMemoryManager, arg0)

void memmgrCalcReservedFbSpaceHal_FWCLIENT(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0, NvU64 *arg1, NvU64 *arg2);

void memmgrCalcReservedFbSpaceHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0, NvU64 *arg1, NvU64 *arg2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrCalcReservedFbSpaceHal(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0, NvU64 *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrCalcReservedFbSpaceHal(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrCalcReservedFbSpaceHal_FWCLIENT(pGpu, pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrCalcReservedFbSpaceHal_HAL(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrCalcReservedFbSpaceHal(pGpu, pMemoryManager, arg0, arg1, arg2)

static inline NvU32 memmgrGetGrHeapReservationSize_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetGrHeapReservationSize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetGrHeapReservationSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetGrHeapReservationSize(pGpu, pMemoryManager) memmgrGetGrHeapReservationSize_4a4dee(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetGrHeapReservationSize_HAL(pGpu, pMemoryManager) memmgrGetGrHeapReservationSize(pGpu, pMemoryManager)

NvU32 memmgrGetRunlistEntriesReservedFbSpace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetRunlistEntriesReservedFbSpace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetRunlistEntriesReservedFbSpace(pGpu, pMemoryManager) memmgrGetRunlistEntriesReservedFbSpace_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetRunlistEntriesReservedFbSpace_HAL(pGpu, pMemoryManager) memmgrGetRunlistEntriesReservedFbSpace(pGpu, pMemoryManager)

NvU32 memmgrGetUserdReservedFbSpace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetUserdReservedFbSpace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetUserdReservedFbSpace(pGpu, pMemoryManager) memmgrGetUserdReservedFbSpace_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetUserdReservedFbSpace_HAL(pGpu, pMemoryManager) memmgrGetUserdReservedFbSpace(pGpu, pMemoryManager)

NV_STATUS memmgrInitReservedMemory_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitReservedMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitReservedMemory(pGpu, pMemoryManager, arg0) memmgrInitReservedMemory_GM107(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrInitReservedMemory_HAL(pGpu, pMemoryManager, arg0) memmgrInitReservedMemory(pGpu, pMemoryManager, arg0)

NV_STATUS memmgrPreInitReservedMemory_FWCLIENT(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrPreInitReservedMemory_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPreInitReservedMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPreInitReservedMemory(pGpu, pMemoryManager) memmgrPreInitReservedMemory_FWCLIENT(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrPreInitReservedMemory_HAL(pGpu, pMemoryManager) memmgrPreInitReservedMemory(pGpu, pMemoryManager)

NV_STATUS memmgrInitBaseFbRegions_FWCLIENT(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrInitBaseFbRegions_GP102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitBaseFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitBaseFbRegions(pGpu, pMemoryManager) memmgrInitBaseFbRegions_FWCLIENT(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrInitBaseFbRegions_HAL(pGpu, pMemoryManager) memmgrInitBaseFbRegions(pGpu, pMemoryManager)

NV_STATUS memmgrSetMemDescPageSize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg0, ADDRESS_TRANSLATION arg1, RM_ATTR_PAGE_SIZE arg2);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetMemDescPageSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg0, ADDRESS_TRANSLATION arg1, RM_ATTR_PAGE_SIZE arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetMemDescPageSize(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrSetMemDescPageSize_GM107(pGpu, pMemoryManager, arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, arg0, arg1, arg2) memmgrSetMemDescPageSize(pGpu, pMemoryManager, arg0, arg1, arg2)

NV_STATUS memmgrSetPartitionableMem_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetPartitionableMem(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetPartitionableMem(pGpu, pMemoryManager) memmgrSetPartitionableMem_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrSetPartitionableMem_HAL(pGpu, pMemoryManager) memmgrSetPartitionableMem(pGpu, pMemoryManager)

NV_STATUS memmgrAllocMIGGPUInstanceMemory_PF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle *phMemory, struct NV_RANGE *pAddrRange, struct Heap **ppMemoryPartitionHeap);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocMIGGPUInstanceMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle *phMemory, struct NV_RANGE *pAddrRange, struct Heap **ppMemoryPartitionHeap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap) memmgrAllocMIGGPUInstanceMemory_PF(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrAllocMIGGPUInstanceMemory_HAL(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap) memmgrAllocMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap)

NV_STATUS memmgrGetBlackListPagesForHeap_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetBlackListPagesForHeap(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetBlackListPagesForHeap(pGpu, pMemoryManager, pHeap) memmgrGetBlackListPagesForHeap_GM107(pGpu, pMemoryManager, pHeap)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetBlackListPagesForHeap_HAL(pGpu, pMemoryManager, pHeap) memmgrGetBlackListPagesForHeap(pGpu, pMemoryManager, pHeap)

static inline NV_STATUS memmgrDiscoverMIGPartitionableMemoryRange_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct NV_RANGE *pMemoryRange) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrDiscoverMIGPartitionableMemoryRange(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct NV_RANGE *pMemoryRange) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDiscoverMIGPartitionableMemoryRange(pGpu, pMemoryManager, pMemoryRange) memmgrDiscoverMIGPartitionableMemoryRange_46f6a7(pGpu, pMemoryManager, pMemoryRange)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrDiscoverMIGPartitionableMemoryRange_HAL(pGpu, pMemoryManager, pMemoryRange) memmgrDiscoverMIGPartitionableMemoryRange(pGpu, pMemoryManager, pMemoryRange)

NvU32 memmgrGetFBEndReserveSizeEstimate_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetFBEndReserveSizeEstimate(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetFBEndReserveSizeEstimate(pGpu, pMemoryManager) memmgrGetFBEndReserveSizeEstimate_GM107(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrGetFBEndReserveSizeEstimate_HAL(pGpu, pMemoryManager) memmgrGetFBEndReserveSizeEstimate(pGpu, pMemoryManager)

NV_STATUS memmgrValidateFBEndReservation_PF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrValidateFBEndReservation(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrValidateFBEndReservation(pGpu, pMemoryManager) memmgrValidateFBEndReservation_PF(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrValidateFBEndReservation_HAL(pGpu, pMemoryManager) memmgrValidateFBEndReservation(pGpu, pMemoryManager)

static inline NV_STATUS memmgrReserveMemoryForPmu_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveMemoryForPmu(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReserveMemoryForPmu(pGpu, pMemoryManager) memmgrReserveMemoryForPmu_56cd7a(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrReserveMemoryForPmu_HAL(pGpu, pMemoryManager) memmgrReserveMemoryForPmu(pGpu, pMemoryManager)

void memmgrFreeFbsrMemory_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFreeFbsrMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFreeFbsrMemory(pGpu, pMemoryManager) memmgrFreeFbsrMemory_KERNEL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrFreeFbsrMemory_HAL(pGpu, pMemoryManager) memmgrFreeFbsrMemory(pGpu, pMemoryManager)

static inline NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReserveVgaWorkspaceMemDescForFbsr(pGpu, pMemoryManager) memmgrReserveVgaWorkspaceMemDescForFbsr_46f6a7(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrReserveVgaWorkspaceMemDescForFbsr_HAL(pGpu, pMemoryManager) memmgrReserveVgaWorkspaceMemDescForFbsr(pGpu, pMemoryManager)

static inline NV_STATUS memmgrCalculateHeapOffsetWithGSP_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *offset) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrCalculateHeapOffsetWithGSP(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *offset) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrCalculateHeapOffsetWithGSP(pGpu, pMemoryManager, offset) memmgrCalculateHeapOffsetWithGSP_46f6a7(pGpu, pMemoryManager, offset)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrCalculateHeapOffsetWithGSP_HAL(pGpu, pMemoryManager, offset) memmgrCalculateHeapOffsetWithGSP(pGpu, pMemoryManager, offset)

NvBool memmgrIsGspOwnedMemory_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc);


#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsGspOwnedMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrIsGspOwnedMemory(pGpu, pMemoryManager, pMemDesc) memmgrIsGspOwnedMemory_KERNEL(pGpu, pMemoryManager, pMemDesc)
#endif //__nvoc_mem_mgr_h_disabled

#define memmgrIsGspOwnedMemory_HAL(pGpu, pMemoryManager, pMemDesc) memmgrIsGspOwnedMemory(pGpu, pMemoryManager, pMemDesc)

NV_STATUS memmgrConstructEngine_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, ENGDESCRIPTOR arg0);

static inline NV_STATUS memmgrConstructEngine_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, ENGDESCRIPTOR arg0) {
    return pMemoryManager->__memmgrConstructEngine__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrStatePreInitLocked_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrStatePreInitLocked__(pGpu, pMemoryManager);
}

NV_STATUS memmgrStateInitLocked_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrStateInitLocked_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrStateInitLocked__(pGpu, pMemoryManager);
}

NV_STATUS memmgrStateLoad_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0);

static inline NV_STATUS memmgrStateLoad_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return pMemoryManager->__memmgrStateLoad__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrStatePostLoad_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0);

static inline NV_STATUS memmgrStatePostLoad_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return pMemoryManager->__memmgrStatePostLoad__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrStatePreUnload_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0);

static inline NV_STATUS memmgrStatePreUnload_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return pMemoryManager->__memmgrStatePreUnload__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrStateUnload_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0);

static inline NV_STATUS memmgrStateUnload_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0) {
    return pMemoryManager->__memmgrStateUnload__(pGpu, pMemoryManager, arg0);
}

void memmgrStateDestroy_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrStateDestroy_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrStateDestroy__(pGpu, pMemoryManager);
}

NV_STATUS memmgrMemUtilsSec2CtxInit_GH100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);

static inline NV_STATUS memmgrMemUtilsSec2CtxInit_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS memmgrMemUtilsSec2CtxInit_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    return pMemoryManager->__memmgrMemUtilsSec2CtxInit__(pGpu, pMemoryManager, arg0);
}

NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_GH100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvBool arg1, RmPhysAddr arg2, NvU32 arg3, NV_ADDRESS_SPACE arg4);

static inline NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_491d52(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvBool arg1, RmPhysAddr arg2, NvU32 arg3, NV_ADDRESS_SPACE arg4) {
    return ((NvBool)(0 != 0));
}

static inline NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg0, NvBool arg1, RmPhysAddr arg2, NvU32 arg3, NV_ADDRESS_SPACE arg4) {
    return pMemoryManager->__memmgrMemUtilsCheckMemoryFastScrubEnable__(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4);
}

NV_STATUS memmgrAllocDetermineAlignment_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment);

NV_STATUS memmgrAllocDetermineAlignment_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment);

static inline NV_STATUS memmgrAllocDetermineAlignment_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment) {
    return pMemoryManager->__memmgrAllocDetermineAlignment__(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment);
}

NvU64 memmgrGetMaxContextSize_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_AD102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetMaxContextSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetMaxContextSize__(pGpu, pMemoryManager);
}

void memmgrScrubRegistryOverrides_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

void memmgrScrubRegistryOverrides_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrScrubRegistryOverrides_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrScrubRegistryOverrides__(pGpu, pMemoryManager);
}

NvU32 memmgrGetPteKindBl_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetPteKindBl_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 memmgrGetPteKindBl_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetPteKindBl__(pGpu, pMemoryManager);
}

NvU32 memmgrGetPteKindPitch_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetPteKindPitch_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 memmgrGetPteKindPitch_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetPteKindPitch__(pGpu, pMemoryManager);
}

NvU32 memmgrChooseKindCompressC_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0);

static inline NvU32 memmgrChooseKindCompressC_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 memmgrChooseKindCompressC_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg0) {
    return pMemoryManager->__memmgrChooseKindCompressC__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrGetFlaKind_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg0);

static inline NV_STATUS memmgrGetFlaKind_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS memmgrGetFlaKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg0) {
    return pMemoryManager->__memmgrGetFlaKind__(pGpu, pMemoryManager, arg0);
}

NvBool memmgrIsApertureSupportedByFla_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0);

static inline NvBool memmgrIsApertureSupportedByFla_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool memmgrIsApertureSupportedByFla_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0) {
    return pMemoryManager->__memmgrIsApertureSupportedByFla__(pGpu, pMemoryManager, arg0);
}

NvU32 memmgrDetermineComptag_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0);

static inline NvU32 memmgrDetermineComptag_13cd8d(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

static inline NvU32 memmgrDetermineComptag_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg0) {
    return pMemoryManager->__memmgrDetermineComptag__(pGpu, pMemoryManager, arg0);
}

NV_STATUS memmgrCheckReservedMemorySize_GK104(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrCheckReservedMemorySize_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

static inline NV_STATUS memmgrCheckReservedMemorySize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrCheckReservedMemorySize__(pGpu, pMemoryManager);
}

NV_STATUS memmgrReadMmuLock_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi);

static inline NV_STATUS memmgrReadMmuLock_e133c0(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi) {
    *pbIsValid = ((NvBool)(0 != 0));
    return NV_OK;
}

static inline NV_STATUS memmgrReadMmuLock_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi) {
    return pMemoryManager->__memmgrReadMmuLock__(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi);
}

NV_STATUS memmgrBlockMemLockedMemory_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrBlockMemLockedMemory_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

static inline NV_STATUS memmgrBlockMemLockedMemory_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrBlockMemLockedMemory__(pGpu, pMemoryManager);
}

NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize);

static inline NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize) {
    return NV_OK;
}

static inline NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize) {
    return pMemoryManager->__memmgrInsertUnprotectedRegionAtBottomOfFb__(pGpu, pMemoryManager, pSize);
}

void memmgrGetDisablePlcKind_GA100(struct MemoryManager *pMemoryManager, NvU32 *pteKind);

static inline void memmgrGetDisablePlcKind_b3696a(struct MemoryManager *pMemoryManager, NvU32 *pteKind) {
    return;
}

static inline void memmgrGetDisablePlcKind_DISPATCH(struct MemoryManager *pMemoryManager, NvU32 *pteKind) {
    pMemoryManager->__memmgrGetDisablePlcKind__(pMemoryManager, pteKind);
}

void memmgrEnableDynamicPageOfflining_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

void memmgrEnableDynamicPageOfflining_GA102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrEnableDynamicPageOfflining_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}

static inline void memmgrEnableDynamicPageOfflining_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrEnableDynamicPageOfflining__(pGpu, pMemoryManager);
}

NV_STATUS memmgrGetBlackListPages_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount);

NV_STATUS memmgrGetBlackListPages_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount);

static inline NV_STATUS memmgrGetBlackListPages_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount) {
    return pMemoryManager->__memmgrGetBlackListPages__(pGpu, pMemoryManager, pBlAddrs, pCount);
}

static inline NV_STATUS memmgrStatePreLoad_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate, NvU32 arg0) {
    return pEngstate->__memmgrStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS memmgrStatePostUnload_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate, NvU32 arg0) {
    return pEngstate->__memmgrStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS memmgrStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__memmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline void memmgrInitMissing_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    pEngstate->__memmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS memmgrStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__memmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool memmgrIsPresent_DISPATCH(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__memmgrIsPresent__(pGpu, pEngstate);
}

static inline NvBool memmgrIsLocalEgmSupported(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bLocalEgmSupported;
}

static inline NvBool memmgrIsLocalEgmEnabled(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bLocalEgmEnabled;
}

static inline NvU32 memmgrLocalEgmPeerId(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->localEgmPeerId;
}

static inline NvBool memmgrIsScrubOnFreeEnabled(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bScrubOnFreeEnabled;
}

static inline NvBool memmgrIsFastScrubberEnabled(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bFastScrubberEnabled;
}

static inline NvBool memmgrUseVasForCeMemoryOps(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bUseVasForCeMemoryOps;
}

static inline NvBool memmgrRmExecutingEccScrub(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bRmExecutingEccScrub;
}

static inline NvBool memmgrBug1441072EccScrubWar(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bBug1441072EccScrubWar;
}

static inline NvBool memmgrIsPmaInitialized(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bPmaInitialized;
}

static inline void memmgrSetPmaInitialized(struct MemoryManager *pMemoryManager, NvBool val) {
    pMemoryManager->bPmaInitialized = val;
}

static inline NvBool memmgrAreFbRegionsSupported(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bFbRegionsSupported;
}

static inline NvBool memmgrIsPmaSupportedOnPlatform(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bPmaSupportedOnPlatform;
}

static inline NvBool memmgrIsPmaEnabled(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bPmaEnabled;
}

static inline NvBool memmgrIsPmaForcePersistence(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bPmaForcePersistence;
}

static inline void memmgrSetPmaForcePersistence(struct MemoryManager *pMemoryManager, NvBool val) {
    pMemoryManager->bPmaForcePersistence = val;
}

static inline NvBool memmgrAreClientPageTablesPmaManaged(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bClientPageTablesPmaManaged;
}

static inline void memmgrSetClientPageTablesPmaManaged(struct MemoryManager *pMemoryManager, NvBool val) {
    pMemoryManager->bClientPageTablesPmaManaged = val;
}

static inline NvBool memmgrIsPmaAddrTree(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bPmaAddrTree;
}

static inline NvU64 memmgrGetRsvdMemoryBase(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->rsvdMemoryBase;
}

static inline NvU32 memmgrGetRsvdMemorySize(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->rsvdMemorySize;
}

static inline NvBool memmgrBug3922001DisableCtxBufOnSim(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bBug3922001DisableCtxBufOnSim;
}

void memmgrDestruct_IMPL(struct MemoryManager *pMemoryManager);

#define __nvoc_memmgrDestruct(pMemoryManager) memmgrDestruct_IMPL(pMemoryManager)
NV_STATUS memmgrAllocResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo) memmgrAllocResources_IMPL(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFree_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *arg0, NvHandle arg1, NvHandle arg2, NvHandle arg3, NvU32 arg4, MEMORY_DESCRIPTOR *arg5);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFree(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *arg0, NvHandle arg1, NvHandle arg2, NvHandle arg3, NvU32 arg4, MEMORY_DESCRIPTOR *arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFree(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5) memmgrFree_IMPL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAddMemNode_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvBool bFreeDescriptor);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAddMemNode(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvBool bFreeDescriptor) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAddMemNode(pGpu, pMemoryManager, pMemDesc, bFreeDescriptor) memmgrAddMemNode_IMPL(pGpu, pMemoryManager, pMemDesc, bFreeDescriptor)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAddMemNodes_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool bSaveAllRmAllocations);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAddMemNodes(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool bSaveAllRmAllocations) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAddMemNodes(pGpu, pMemoryManager, bSaveAllRmAllocations) memmgrAddMemNodes_IMPL(pGpu, pMemoryManager, bSaveAllRmAllocations)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrRemoveMemNodes_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRemoveMemNodes(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrRemoveMemNodes(pGpu, pMemoryManager) memmgrRemoveMemNodes_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

struct Heap *memmgrGetDeviceSuballocator_IMPL(struct MemoryManager *pMemoryManager, NvBool bForceSubheap);

#ifdef __nvoc_mem_mgr_h_disabled
static inline struct Heap *memmgrGetDeviceSuballocator(struct MemoryManager *pMemoryManager, NvBool bForceSubheap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetDeviceSuballocator(pMemoryManager, bForceSubheap) memmgrGetDeviceSuballocator_IMPL(pMemoryManager, bForceSubheap)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemCopy_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, TRANSFER_SURFACE *pSrc, NvU32 size, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemCopy(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, TRANSFER_SURFACE *pSrc, NvU32 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemCopy(pMemoryManager, pDst, pSrc, size, flags) memmgrMemCopy_IMPL(pMemoryManager, pDst, pSrc, size, flags)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemSet_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, NvU32 value, NvU32 size, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemSet(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, NvU32 value, NvU32 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemSet(pMemoryManager, pDst, value, size, flags) memmgrMemSet_IMPL(pMemoryManager, pDst, value, size, flags)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemWrite_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, void *pBuf, NvU64 size, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemWrite(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, void *pBuf, NvU64 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemWrite(pMemoryManager, pDst, pBuf, size, flags) memmgrMemWrite_IMPL(pMemoryManager, pDst, pBuf, size, flags)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemRead_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pSrc, void *pBuf, NvU64 size, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemRead(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pSrc, void *pBuf, NvU64 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemRead(pMemoryManager, pSrc, pBuf, size, flags) memmgrMemRead_IMPL(pMemoryManager, pSrc, pBuf, size, flags)
#endif //__nvoc_mem_mgr_h_disabled

NvU8 *memmgrMemBeginTransfer_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU8 *memmgrMemBeginTransfer(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemBeginTransfer(pMemoryManager, pTransferInfo, shadowBufSize, flags) memmgrMemBeginTransfer_IMPL(pMemoryManager, pTransferInfo, shadowBufSize, flags)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrMemEndTransfer_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemEndTransfer(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemEndTransfer(pMemoryManager, pTransferInfo, shadowBufSize, flags) memmgrMemEndTransfer_IMPL(pMemoryManager, pTransferInfo, shadowBufSize, flags)
#endif //__nvoc_mem_mgr_h_disabled

NvU8 *memmgrMemDescBeginTransfer_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU8 *memmgrMemDescBeginTransfer(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemDescBeginTransfer(pMemoryManager, pMemDesc, flags) memmgrMemDescBeginTransfer_IMPL(pMemoryManager, pMemDesc, flags)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrMemDescEndTransfer_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemDescEndTransfer(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemDescEndTransfer(pMemoryManager, pMemDesc, flags) memmgrMemDescEndTransfer_IMPL(pMemoryManager, pMemDesc, flags)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemDescMemSet_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU32 flags);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemDescMemSet(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemDescMemSet(pMemoryManager, pMemDesc, value, flags) memmgrMemDescMemSet_IMPL(pMemoryManager, pMemDesc, value, flags)
#endif //__nvoc_mem_mgr_h_disabled

NV_ADDRESS_SPACE memmgrAllocGetAddrSpace_IMPL(struct MemoryManager *pMemoryManager, NvU32 flags, NvU32 attr);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_ADDRESS_SPACE memmgrAllocGetAddrSpace(struct MemoryManager *pMemoryManager, NvU32 flags, NvU32 attr) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    NV_ADDRESS_SPACE ret;
    portMemSet(&ret, 0, sizeof(NV_ADDRESS_SPACE));
    return ret;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocGetAddrSpace(pMemoryManager, flags, attr) memmgrAllocGetAddrSpace_IMPL(pMemoryManager, flags, attr)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrCreateHeap_IMPL(struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrCreateHeap(struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrCreateHeap(pMemoryManager) memmgrCreateHeap_IMPL(pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrGetUsedRamSize_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetUsedRamSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetUsedRamSize(pGpu, pMemoryManager, arg0) memmgrGetUsedRamSize_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocHwResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocHwResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocHwResources(pGpu, pMemoryManager, arg0) memmgrAllocHwResources_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFreeHwResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFreeHwResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFreeHwResources(pGpu, pMemoryManager, arg0) memmgrFreeHwResources_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NvBool memmgrLargePageSupported_IMPL(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrLargePageSupported(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrLargePageSupported(pMemoryManager, arg0) memmgrLargePageSupported_IMPL(pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NvBool memmgrComprSupported_IMPL(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrComprSupported(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrComprSupported(pMemoryManager, arg0) memmgrComprSupported_IMPL(pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NvU32 memmgrGetMappableRamSizeMb_IMPL(struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrGetMappableRamSizeMb(struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetMappableRamSizeMb(pMemoryManager) memmgrGetMappableRamSizeMb_IMPL(pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

PFB_REGION_DESCRIPTOR memmgrLookupFbRegionByOffset_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr fbOffset, RmPhysAddr fbLimit);

#ifdef __nvoc_mem_mgr_h_disabled
static inline PFB_REGION_DESCRIPTOR memmgrLookupFbRegionByOffset(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr fbOffset, RmPhysAddr fbLimit) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrLookupFbRegionByOffset(pGpu, pMemoryManager, fbOffset, fbLimit) memmgrLookupFbRegionByOffset_IMPL(pGpu, pMemoryManager, fbOffset, fbLimit)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFillMemdescForPhysAttr_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg0, ADDRESS_TRANSLATION arg1, NvU64 *arg2, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 *arg7, NvU64 *arg8);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFillMemdescForPhysAttr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg0, ADDRESS_TRANSLATION arg1, NvU64 *arg2, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 *arg7, NvU64 *arg8) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFillMemdescForPhysAttr(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) memmgrFillMemdescForPhysAttr_IMPL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrSetPlatformPmaSupport_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetPlatformPmaSupport(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetPlatformPmaSupport(pGpu, pMemoryManager) memmgrSetPlatformPmaSupport_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrRegionSetupForPma_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegionSetupForPma(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrRegionSetupForPma(pGpu, pMemoryManager) memmgrRegionSetupForPma_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrInitFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitFbRegions(pGpu, pMemoryManager) memmgrInitFbRegions_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrRegionSetupCommon_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegionSetupCommon(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrRegionSetupCommon(pGpu, pMemoryManager) memmgrRegionSetupCommon_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrRegenerateFbRegionPriority_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegenerateFbRegionPriority(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrRegenerateFbRegionPriority(pGpu, pMemoryManager) memmgrRegenerateFbRegionPriority_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NvU32 memmgrInsertFbRegion_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PFB_REGION_DESCRIPTOR arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrInsertFbRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PFB_REGION_DESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInsertFbRegion(pGpu, pMemoryManager, arg0) memmgrInsertFbRegion_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrDumpFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrDumpFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDumpFbRegions(pGpu, pMemoryManager) memmgrDumpFbRegions_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrClearFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrClearFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrClearFbRegions(pGpu, pMemoryManager) memmgrClearFbRegions_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrReleaseConsoleRegion_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrReleaseConsoleRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReleaseConsoleRegion(pGpu, pMemoryManager) memmgrReleaseConsoleRegion_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

PMEMORY_DESCRIPTOR memmgrGetReservedConsoleMemDesc_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline PMEMORY_DESCRIPTOR memmgrGetReservedConsoleMemDesc(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager) memmgrGetReservedConsoleMemDesc_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrReserveBar2BackingStore_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrReserveBar2BackingStore(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReserveBar2BackingStore(pGpu, pMemoryManager, arg0) memmgrReserveBar2BackingStore_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrCalcReservedFbSpace_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrCalcReservedFbSpace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrCalcReservedFbSpace(pGpu, pMemoryManager) memmgrCalcReservedFbSpace_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrMemUtilsSetupChannelBufferSizes_IMPL(struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, NvU32 arg1);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemUtilsSetupChannelBufferSizes(struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsSetupChannelBufferSizes(pMemoryManager, arg0, arg1) memmgrMemUtilsSetupChannelBufferSizes_IMPL(pMemoryManager, arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemUtilsChannelSchedulingSetup_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsChannelSchedulingSetup(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsChannelSchedulingSetup(pGpu, pMemoryManager, arg0) memmgrMemUtilsChannelSchedulingSetup_IMPL(pGpu, pMemoryManager, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrGetKindComprFromMemDesc_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg0, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetKindComprFromMemDesc(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg0, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetKindComprFromMemDesc(pMemoryManager, arg0, offset, kind, pComprInfo) memmgrGetKindComprFromMemDesc_IMPL(pMemoryManager, arg0, offset, kind, pComprInfo)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFillComprInfo_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0, NvU32 arg1, NvU32 arg2, NvU64 arg3, NvU32 arg4, COMPR_INFO *arg5);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFillComprInfo(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg0, NvU32 arg1, NvU32 arg2, NvU64 arg3, NvU32 arg4, COMPR_INFO *arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFillComprInfo(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5) memmgrFillComprInfo_IMPL(pGpu, pMemoryManager, arg0, arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrComprInfoDisableCompression_IMPL(struct MemoryManager *pMemoryManager, COMPR_INFO *pComprInfo);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrComprInfoDisableCompression(struct MemoryManager *pMemoryManager, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrComprInfoDisableCompression(pMemoryManager, pComprInfo) memmgrComprInfoDisableCompression_IMPL(pMemoryManager, pComprInfo)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrFillComprInfoUncompressed_IMPL(struct MemoryManager *pMemoryManager, NvU32 kind, COMPR_INFO *pComprInfo);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFillComprInfoUncompressed(struct MemoryManager *pMemoryManager, NvU32 kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFillComprInfoUncompressed(pMemoryManager, kind, pComprInfo) memmgrFillComprInfoUncompressed_IMPL(pMemoryManager, kind, pComprInfo)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPmaInitialize_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMA *pPma);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPmaInitialize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMA *pPma) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPmaInitialize(pGpu, pMemoryManager, pPma) memmgrPmaInitialize_IMPL(pGpu, pMemoryManager, pPma)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPmaRegisterRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap, PMA *pPma);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPmaRegisterRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap, PMA *pPma) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPmaRegisterRegions(pGpu, pMemoryManager, pHeap, pPma) memmgrPmaRegisterRegions_IMPL(pGpu, pMemoryManager, pHeap, pPma)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrInitInternalChannels_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitInternalChannels(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitInternalChannels(pGpu, pMemoryManager) memmgrInitInternalChannels_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrDestroyInternalChannels_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrDestroyInternalChannels(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDestroyInternalChannels(pGpu, pMemoryManager) memmgrDestroyInternalChannels_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrInitCeUtils_IMPL(struct MemoryManager *pMemoryManager, NvBool bFifoLite);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitCeUtils(struct MemoryManager *pMemoryManager, NvBool bFifoLite) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrInitCeUtils(pMemoryManager, bFifoLite) memmgrInitCeUtils_IMPL(pMemoryManager, bFifoLite)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrDestroyCeUtils_IMPL(struct MemoryManager *pMemoryManager, NvBool bSuspendCeUtils);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrDestroyCeUtils(struct MemoryManager *pMemoryManager, NvBool bSuspendCeUtils) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrDestroyCeUtils(pMemoryManager, bSuspendCeUtils) memmgrDestroyCeUtils_IMPL(pMemoryManager, bSuspendCeUtils)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrSetMIGPartitionableBAR1Range_IMPL(OBJGPU *arg0, struct MemoryManager *arg1);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetMIGPartitionableBAR1Range(OBJGPU *arg0, struct MemoryManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetMIGPartitionableBAR1Range(arg0, arg1) memmgrSetMIGPartitionableBAR1Range_IMPL(arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

struct NV_RANGE memmgrGetMIGPartitionableBAR1Range_IMPL(OBJGPU *arg0, struct MemoryManager *arg1);

#ifdef __nvoc_mem_mgr_h_disabled
static inline struct NV_RANGE memmgrGetMIGPartitionableBAR1Range(OBJGPU *arg0, struct MemoryManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    struct NV_RANGE ret;
    portMemSet(&ret, 0, sizeof(struct NV_RANGE));
    return ret;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetMIGPartitionableBAR1Range(arg0, arg1) memmgrGetMIGPartitionableBAR1Range_IMPL(arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrSetMIGPartitionableMemoryRange_IMPL(OBJGPU *arg0, struct MemoryManager *arg1, struct NV_RANGE arg2);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrSetMIGPartitionableMemoryRange(OBJGPU *arg0, struct MemoryManager *arg1, struct NV_RANGE arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrSetMIGPartitionableMemoryRange(arg0, arg1, arg2) memmgrSetMIGPartitionableMemoryRange_IMPL(arg0, arg1, arg2)
#endif //__nvoc_mem_mgr_h_disabled

struct NV_RANGE memmgrGetMIGPartitionableMemoryRange_IMPL(OBJGPU *arg0, struct MemoryManager *arg1);

#ifdef __nvoc_mem_mgr_h_disabled
static inline struct NV_RANGE memmgrGetMIGPartitionableMemoryRange(OBJGPU *arg0, struct MemoryManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    struct NV_RANGE ret;
    portMemSet(&ret, 0, sizeof(struct NV_RANGE));
    return ret;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetMIGPartitionableMemoryRange(arg0, arg1) memmgrGetMIGPartitionableMemoryRange_IMPL(arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFreeMIGGPUInstanceMemory_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle hMemory, struct Heap **ppMemoryPartitionHeap);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFreeMIGGPUInstanceMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle hMemory, struct Heap **ppMemoryPartitionHeap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFreeMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, hMemory, ppMemoryPartitionHeap) memmgrFreeMIGGPUInstanceMemory_IMPL(pGpu, pMemoryManager, swizzId, hMemory, ppMemoryPartitionHeap)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPageLevelPoolsCreate_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPageLevelPoolsCreate(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsCreate(pGpu, pMemoryManager) memmgrPageLevelPoolsCreate_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrPageLevelPoolsDestroy_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrPageLevelPoolsDestroy(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsDestroy(pGpu, pMemoryManager) memmgrPageLevelPoolsDestroy_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPageLevelPoolsGetInfo_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, struct RM_POOL_ALLOC_MEM_RESERVE_INFO **arg0);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPageLevelPoolsGetInfo(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, struct RM_POOL_ALLOC_MEM_RESERVE_INFO **arg0) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsGetInfo(pGpu, pMemoryManager, pDevice, arg0) memmgrPageLevelPoolsGetInfo_IMPL(pGpu, pMemoryManager, pDevice, arg0)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocMIGMemoryAllocationInternalHandles_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocMIGMemoryAllocationInternalHandles(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrAllocMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager) memmgrAllocMIGMemoryAllocationInternalHandles_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrFreeMIGMemoryAllocationInternalHandles_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFreeMIGMemoryAllocationInternalHandles(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrFreeMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager) memmgrFreeMIGMemoryAllocationInternalHandles_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrGetFreeMemoryForAllMIGGPUInstances_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetFreeMemoryForAllMIGGPUInstances(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetFreeMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, pBytes) memmgrGetFreeMemoryForAllMIGGPUInstances_IMPL(pGpu, pMemoryManager, pBytes)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrGetTotalMemoryForAllMIGGPUInstances_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetTotalMemoryForAllMIGGPUInstances(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetTotalMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, pBytes) memmgrGetTotalMemoryForAllMIGGPUInstances_IMPL(pGpu, pMemoryManager, pBytes)
#endif //__nvoc_mem_mgr_h_disabled

void memmgrGetTopLevelScrubberStatus_IMPL(OBJGPU *arg0, struct MemoryManager *arg1, NvBool *pbTopLevelScrubberEnabled, NvBool *pbTopLevelScrubberConstructed);

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetTopLevelScrubberStatus(OBJGPU *arg0, struct MemoryManager *arg1, NvBool *pbTopLevelScrubberEnabled, NvBool *pbTopLevelScrubberConstructed) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrGetTopLevelScrubberStatus(arg0, arg1, pbTopLevelScrubberEnabled, pbTopLevelScrubberConstructed) memmgrGetTopLevelScrubberStatus_IMPL(arg0, arg1, pbTopLevelScrubberEnabled, pbTopLevelScrubberConstructed)
#endif //__nvoc_mem_mgr_h_disabled

MEMORY_DESCRIPTOR *memmgrMemUtilsGetMemDescFromHandle_IMPL(struct MemoryManager *pMemoryManager, NvHandle hClient, NvHandle hMemory);

#ifdef __nvoc_mem_mgr_h_disabled
static inline MEMORY_DESCRIPTOR *memmgrMemUtilsGetMemDescFromHandle(struct MemoryManager *pMemoryManager, NvHandle hClient, NvHandle hMemory) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, hClient, hMemory) memmgrMemUtilsGetMemDescFromHandle_IMPL(pMemoryManager, hClient, hMemory)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrVerifyGspDmaOps_IMPL(OBJGPU *arg0, struct MemoryManager *arg1);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrVerifyGspDmaOps(OBJGPU *arg0, struct MemoryManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrVerifyGspDmaOps(arg0, arg1) memmgrVerifyGspDmaOps_IMPL(arg0, arg1)
#endif //__nvoc_mem_mgr_h_disabled

NV_STATUS memmgrReserveMemoryForFsp_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveMemoryForFsp(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_mgr_h_disabled
#define memmgrReserveMemoryForFsp(pGpu, pMemoryManager) memmgrReserveMemoryForFsp_IMPL(pGpu, pMemoryManager)
#endif //__nvoc_mem_mgr_h_disabled

#undef PRIVATE_FIELD


#endif // MEM_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MGR_NVOC_H_
