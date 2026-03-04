
#ifndef _G_MEM_MGR_NVOC_H_
#define _G_MEM_MGR_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_mem_mgr_nvoc.h"

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include "core/core.h"
#include "gpu/eng_state.h"

#include "gpu/mem_mgr/fbsr.h"
#include "gpu/gpu.h"

#include "mem_mgr/mem.h"
#include "kernel/gpu_mgr/gpu_mgr.h"

#include "mem_mgr/mem_list.h"

#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "containers/map.h"
#include "gpu/mem_mgr/heap_base.h"
#include "mem_mgr/vaspace.h"

struct _PMA;
typedef struct _PMA PMA;

#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS


struct CeUtils;

#ifndef __NVOC_CLASS_CeUtils_TYPEDEF__
#define __NVOC_CLASS_CeUtils_TYPEDEF__
typedef struct CeUtils CeUtils;
#endif /* __NVOC_CLASS_CeUtils_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtils
#define __nvoc_class_id_CeUtils 0x8b8bae
#endif /* __nvoc_class_id_CeUtils */



struct SysmemScrubber;

#ifndef __NVOC_CLASS_SysmemScrubber_TYPEDEF__
#define __NVOC_CLASS_SysmemScrubber_TYPEDEF__
typedef struct SysmemScrubber SysmemScrubber;
#endif /* __NVOC_CLASS_SysmemScrubber_TYPEDEF__ */

#ifndef __nvoc_class_id_SysmemScrubber
#define __nvoc_class_id_SysmemScrubber 0x266962
#endif /* __nvoc_class_id_SysmemScrubber */



typedef volatile struct _cl906f_tag1 Nv906fControl;
typedef struct KERNEL_MIG_GPU_INSTANCE KERNEL_MIG_GPU_INSTANCE;

typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64              offset;

    //
    // Private, should only be used by MemUtils layer
    // Expected to be 0 when map is called
    // Become 0 when unmapped
    //
    void              *pMapping;
    void              *pMappingPriv;
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

#define TRANSFER_FLAGS_NONE                             0
#define TRANSFER_FLAGS_DEFER_FLUSH                      NVBIT32(0) // Applicable only for write operations
#define TRANSFER_FLAGS_SHADOW_ALLOC                     NVBIT32(1) // Applicable only for non-PROCESSOR transfers
#define TRANSFER_FLAGS_SHADOW_INIT_MEM                  NVBIT32(2) // Applicable only for non-PROCESSOR transfers
#define TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING           NVBIT32(3) // Require long lived PROCESSOR mapping
#define TRANSFER_FLAGS_DESTROY_MAPPING                  NVBIT32(4) // Destroy any cached mappings when complete
#define TRANSFER_FLAGS_USE_BAR1                         NVBIT32(5) // Use only BAR1 for PROCESSOR transfers
#define TRANSFER_FLAGS_PREFER_CE                        NVBIT32(6) // Use CE if possible (BAR0 on simulation for perf)
#define TRANSFER_FLAGS_CE_PRI_DEFER_FLUSH               NVBIT32(7) // Defer CE flush; only affects PRI CE operations

// Protection flags: at most 1 may be set, none means READ_WRITE by default
#define TRANSFER_FLAGS_MAP_PROTECT_READABLE             NVBIT32(8) // Transfer is only reading data
#define TRANSFER_FLAGS_MAP_PROTECT_WRITEABLE            NVBIT32(9) // Transfer is only writing data

#define TRANSFER_FLAGS_PREFER_PROCESSOR                 NVBIT32(10) // Use BAR1/2 if possible
#define TRANSFER_FLAGS_ALLOW_MAPPING_REUSE              NVBIT32(11) // Prefer existing full-allocation mapping
                                                                    // (see memdescGetKernelMapping())
                                                                    // Only affects BeginTransfer/EndTransfer
                                                                    // Mapping lifetime controlled by original mapper
                                                                    // Intented for short uses,
                                                                    // where it can't be unmapped by the owner

#define TRANSFER_FLAGS_FLUSH_CPU_CACHE_WAR_BUG4686457   NVBIT32(12) // Flush the data from CPU cache
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
    FB_IS_KIND_SWIZZLED,                    // Kind is swizzled (not pitch or GMK)
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

    NvBool                          bUseBar1;

    NvBool bSecure;

} OBJCHANNEL;

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
} OBJSCRUB;

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
    NvBool  bPreserveOnSuspend; // Required to be Saved during S/R.
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
#define MAX_FBSR_RESERVED_REGIONS                   4  // Max. Memory descriptors for RM Instance memory
#define FBSR_RESERVED_INST_MEMORY_BEFORE_BAR2PTE    0  // Described on Kernel-RM and Physical-RM (Monolithic / GSP offload)
#define FBSR_RESERVED_INST_MEMORY_AFTER_BAR2PTE     1  // Described on Kernel-RM and Physical-RM (Monolithic / GSP offload)
#define FBSR_RESERVED_INST_MEMORY_GSP_NON_WPR       2  // Described on Physical-RM (GSP offload)
#define FBSR_RESERVED_INST_MEMORY_VGA_WORKSPACE     3  // Described on Physical-RM (GSP offload)

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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryManager;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__MemoryManager;

struct RM_POOL_ALLOC_MEM_RESERVE_INFO;

struct __nvoc_inner_struc_MemoryManager_1__ {
    NvBool bEnabled;
    NvU32 peerId;
};

struct __nvoc_inner_struc_MemoryManager_2__ {
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

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryManager *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct MemoryManager *__nvoc_pbase_MemoryManager;    // memmgr

    // Vtable with 87 per-object function pointers
    NvU64 (*__memmgrDeterminePageSize__)(struct MemoryManager * /*this*/, NvHandle, NvU64, NvU32, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__memmgrFreeHwResources__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *);  // halified (2 hals) body
    NV_STATUS (*__memmgrCreateHeap__)(struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrInitFbRegions__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocateConsoleRegion__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__memmgrScrubHandlePostSchedulingEnable__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrScrubHandlePreSchedulingDisable__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrMemUtilsChannelInitialize__)(OBJGPU *, struct MemoryManager * /*this*/, OBJCHANNEL *);  // halified (2 hals) body
    NV_STATUS (*__memmgrMemUtilsCopyEngineInitialize__)(OBJGPU *, struct MemoryManager * /*this*/, OBJCHANNEL *);  // halified (2 hals) body
    NV_STATUS (*__memmgrMemUtilsSec2CtxInit__)(OBJGPU *, struct MemoryManager * /*this*/, OBJCHANNEL *);  // halified (2 hals) body
    NV_STATUS (*__memmgrMemUtilsGetCopyEngineClass__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__memmgrMemUtilsCreateMemoryAlias__)(OBJGPU *, struct MemoryManager * /*this*/, OBJCHANNEL *);  // halified (2 hals) body
    NvBool (*__memmgrMemUtilsCheckMemoryFastScrubEnable__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32, NvBool, RmPhysAddr, NvU32, NV_ADDRESS_SPACE);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocHal__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *);  // halified (2 hals) body
    NV_STATUS (*__memmgrFreeHal__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *, PRMTIMEOUT);  // halified (2 hals) body
    NV_STATUS (*__memmgrGetBankPlacementData__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32 *);  // halified (2 hals) body
    void (*__memmgrDirtyForPmTest__)(OBJGPU *, struct MemoryManager * /*this*/, NvBool);  // halified (2 hals) body
    NvU64 (*__memmgrGetReservedHeapSizeMb__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocDetermineAlignment__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64 *, NvU64 *, NvU64, NvU32, NvU32, NvU32, NvU64);  // halified (3 hals) body
    NvU64 (*__memmgrGetMaxContextSize__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (6 hals) body
    void (*__memmgrHandleSizeOverrides__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrFinishHandleSizeOverrides__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrGetBAR1InfoForDevice__)(OBJGPU *, struct MemoryManager * /*this*/, struct Device *, PGETBAR1INFO);  // halified (2 hals) body
    NvU64 (*__memmgrGetFbTaxSize__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    void (*__memmgrScrubRegistryOverrides__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (3 hals) body
    NvU64 (*__memmgrGetRsvdSizeForSr__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvBool (*__memmgrComparePhysicalAddresses__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32, NvU64, NvU32, NvU64);  // halified (2 hals) body
    RmPhysAddr (*__memmgrGetInvalidOffset__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU64 (*__memmgrGetAddrSpaceSizeMB__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU64 (*__memmgrGetUsableMemSizeMB__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvBool (*__memmgrIsKindCompressible__)(struct MemoryManager * /*this*/, NvU32);  // halified (2 hals) body
    NvU32 (*__memmgrGetPteKindBl__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (3 hals) body
    NvU32 (*__memmgrGetPteKindPitch__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU32 (*__memmgrChooseKindZ__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_PAGE_FORMAT *);  // halified (4 hals) body
    NvU32 (*__memmgrChooseKindCompressZ__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_PAGE_FORMAT *);  // halified (4 hals) body
    NvU32 (*__memmgrChooseKindCompressC__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_PAGE_FORMAT *);  // halified (2 hals) body
    NvU32 (*__memmgrGetPteKindGenericMemoryCompressible__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU32 (*__memmgrGetUncompressedKind__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32, NvBool);  // halified (4 hals) body
    NvU32 (*__memmgrGetCompressedKind__)(struct MemoryManager * /*this*/, NvU32, NvBool);  // halified (4 hals) body
    NV_STATUS (*__memmgrChooseKind__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_PAGE_FORMAT *, NvU32, NvU32 *);  // halified (2 hals) body
    NvBool (*__memmgrIsKind__)(struct MemoryManager * /*this*/, FB_IS_KIND_OP, NvU32);  // halified (3 hals)
    NvU32 (*__memmgrGetMessageKind__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU32 (*__memmgrGetDefaultPteKindForNoHandle__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrGetFlaKind__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32 *);  // halified (2 hals) body
    NvBool (*__memmgrIsMemDescSupportedByFla__)(OBJGPU *, struct MemoryManager * /*this*/, MEMORY_DESCRIPTOR *);  // halified (3 hals)
    NvBool (*__memmgrIsValidFlaPageSize__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64, NvBool);  // halified (3 hals) body
    NvU32 (*__memmgrGetHwPteKindFromSwPteKind__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // halified (2 hals) body
    NvU32 (*__memmgrGetSwPteKindFromHwPteKind__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // halified (2 hals) body
    void (*__memmgrGetPteKindForScrubber__)(struct MemoryManager * /*this*/, NvU32 *);  // halified (2 hals) body
    NvU32 (*__memmgrGetCtagOffsetFromParams__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *);  // halified (2 hals) body
    void (*__memmgrSetCtagOffsetInParams__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *, NvU32);  // halified (2 hals) body
    NvU32 (*__memmgrDetermineComptag__)(OBJGPU *, struct MemoryManager * /*this*/, RmPhysAddr);  // halified (2 hals) body
    NV_STATUS (*__memmgrScrubMapDoorbellRegion__)(OBJGPU *, struct MemoryManager * /*this*/, OBJCHANNEL *);  // halified (2 hals) body
    NV_STATUS (*__memmgrSetAllocParameters__)(OBJGPU *, struct MemoryManager * /*this*/, FB_ALLOC_INFO *);  // halified (2 hals) body
    void (*__memmgrCalcReservedFbSpaceForUVM__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64 *);  // halified (2 hals) body
    void (*__memmgrCalcReservedFbSpaceHal__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64 *, NvU64 *, NvU64 *);  // halified (2 hals) body
    NvU32 (*__memmgrGetGrHeapReservationSize__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvU32 (*__memmgrGetRunlistEntriesReservedFbSpace__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (3 hals) body
    NvU32 (*__memmgrGetUserdReservedFbSpace__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__memmgrCheckReservedMemorySize__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrInitReservedMemory__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64);  // halified (2 hals) body
    NV_STATUS (*__memmgrPreInitReservedMemory__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrReadMmuLock__)(OBJGPU *, struct MemoryManager * /*this*/, NvBool *, NvU64 *, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__memmgrBlockMemLockedMemory__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrInsertUnprotectedRegionAtBottomOfFb__)(OBJGPU *, struct MemoryManager * /*this*/, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__memmgrInitBaseFbRegions__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    void (*__memmgrGetDisablePlcKind__)(struct MemoryManager * /*this*/, NvU32 *);  // halified (2 hals) body
    void (*__memmgrEnableDynamicPageOfflining__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__memmgrSetMemDescPageSize__)(OBJGPU *, struct MemoryManager * /*this*/, PMEMORY_DESCRIPTOR, ADDRESS_TRANSLATION, RM_ATTR_PAGE_SIZE);  // halified (2 hals) body
    NV_STATUS (*__memmgrSetPartitionableMem__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocMIGGPUInstanceMemory__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32, NvHandle *, struct NV_RANGE *, struct Heap **);  // halified (2 hals)
    NV_STATUS (*__memmgrGetBlackListPagesForHeap__)(OBJGPU *, struct MemoryManager * /*this*/, struct Heap *);  // halified (2 hals) body
    NV_STATUS (*__memmgrGetBlackListPages__)(OBJGPU *, struct MemoryManager * /*this*/, BLACKLIST_ADDRESS *, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__memmgrDiscoverMIGPartitionableMemoryRange__)(OBJGPU *, struct MemoryManager * /*this*/, struct NV_RANGE *);  // halified (2 hals) body
    NvU32 (*__memmgrGetFBEndReserveSizeEstimate__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__memmgrInitZeroFbRegionsHal__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocScanoutCarveoutRegionResources__)(struct MemoryManager * /*this*/, NV_MEMORY_ALLOCATION_PARAMS *, NvU32, NvU32 *, MEMORY_DESCRIPTOR *);  // halified (2 hals) body
    NV_STATUS (*__memmgrAllocFromScanoutCarveoutRegion__)(POBJGPU, struct MemoryManager * /*this*/, NvU32, NV_MEMORY_ALLOCATION_PARAMS *, NvU32 *, PMEMORY_DESCRIPTOR *);  // halified (2 hals) body
    void (*__memmgrFreeScanoutCarveoutRegionResources__)(struct MemoryManager * /*this*/, NvU64);  // halified (2 hals) body
    void (*__memmgrFreeFromScanoutCarveoutRegion__)(POBJGPU, struct MemoryManager * /*this*/, PMEMORY_DESCRIPTOR);  // halified (2 hals) body
    NV_STATUS (*__memmgrCreateScanoutCarveoutHeap__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrDestroyScanoutCarveoutHeap__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__memmgrDuplicateFromScanoutCarveoutRegion__)(POBJGPU, struct MemoryManager * /*this*/, PMEMORY_DESCRIPTOR);  // halified (2 hals) body
    NvBool (*__memmgrIsMemoryIoCoherent__)(OBJGPU *, struct MemoryManager * /*this*/, NV_MEMORY_ALLOCATION_PARAMS *);  // halified (2 hals) body
    NvU8 (*__memmgrGetLocalizedOffset__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvBool (*__memmgrIsFlaSysmemSupported__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body
    NvBool (*__memmgrGetLocalizedMemorySupported__)(OBJGPU *, struct MemoryManager * /*this*/);  // halified (2 hals) body

    // 1 PDB property
//  NvBool PDB_PROP_MEMMGR_IS_MISSING inherited from OBJENGSTATE

    // Data members
    OBJEHEAP *pScanoutHeap;
    NvBool bFbsrWddmModeEnabled;
    NvBool bFbRegionsSupported;
    NvBool bPmaSupportedOnPlatform;
    NvBool bPmaEnabled;
    NvBool bPmaInitialized;
    NvBool bPmaForcePersistence;
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
    NvBool bPlatformFullyCoherent;
    NvBool bAllowNoncontiguousAllocation;
    NvBool bLocalEgmSupported;
    struct __nvoc_inner_struc_MemoryManager_1__ localEgmOverride;
    NvBool bLocalEgmEnabled;
    NvU32 localEgmPeerId;
    NvS32 localEgmNodeId;
    NvU64 localEgmBasePhysAddr;
    NvU64 localEgmSize;
    NvBool bForceEnableFlaSysmem;
    NvBool bEccInterleavedVidmemScrub;
    NvBool bScrubberInitialized;
    NvBool bAllowSysmemHugePages;
    NvBool bEccScrubOverride;
    struct Heap *pHeap;
    NvBool bScrubOnFreeEnabled;
    NvBool bFastScrubberEnabled;
    NvBool bFastScrubberSupportsSysmem;
    NvBool bSysmemPageSizeDefaultAllowLargePages;
    NvBool bDisableAsyncScrubforMods;
    NvBool bUseVasForCeMemoryOps;
    NvBool bCePhysicalVidmemAccessNotSupported;
    NvBool bRmExecutingEccScrub;
    NvBool bBug1441072EccScrubWar;
    NvU64 heapStartOffset;
    NvU64 rsvdMemoryBase;
    NvU64 rsvdMemorySize;
    struct CeUtils *pCeUtils;
    NvBool bDisableGlobalCeUtils;
    struct SysmemScrubber *pSysmemScrubber;
    OBJSCRUB eccScrubberState;
    struct __nvoc_inner_struc_MemoryManager_2__ Ram;
    PMEMORY_DESCRIPTOR pReservedConsoleMemDesc;
    NvU32 PteKindOverride;
    NvU32 zbcSurfaces[8];
    NvU64 overrideInitHeapMin;
    NvU64 overrideHeapMax;
    NvU64 rsvdMemorySizeIncrement;
    NvU64 overrideMaxContextSizeRsvdMemory;
    struct OBJFBSR *pFbsr[8];
    struct OBJFBSR *pActiveFbsr;
    NvU32 fbsrStartMode;
    NvU32 fixedFbsrModesMask;
    MEMORY_DESCRIPTOR *fbsrReservedRanges[4];
    PFB_MEM_NODE pMemHeadNode;
    PFB_MEM_NODE pMemTailNode;
    struct RM_POOL_ALLOC_MEM_RESERVE_INFO *pPageLevelReserve;
    struct MIG_MEMORY_PARTITIONING_INFO MIGMemoryPartitioningInfo;
    NV_FB_RSVD_BLOCK_LOG_INFO rsvdBlockInfo;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NvBool bReservedMemAtBottom;
    NvU64 bug4146226ReserveOffset;
    NvBool bBug4146226ReserveWar;
    NvHandle hThirdPartyP2P;
    NvBool bMonitoredFenceSupported;
    NvBool b64BitSemaphoresSupported;
    NvBool bGenericKindSupport;
    NvBool bSkipCompressionCheck;
    NvBool bUseVirtualCopyOnSuspend;
    NvBool bLocalizedMemorySupported;
    NvU64 localizedMask;
    NvHandle hGspHeapSysMemHandle;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__MemoryManager {
    NV_STATUS (*__memmgrConstructEngine__)(OBJGPU *, struct MemoryManager * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePreInitLocked__)(OBJGPU *, struct MemoryManager * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStateInitLocked__)(OBJGPU *, struct MemoryManager * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStateLoad__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePostLoad__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePreUnload__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__memmgrStateUnload__)(OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__memmgrStateDestroy__)(OBJGPU *, struct MemoryManager * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__memmgrInitMissing__)(struct OBJGPU *, struct MemoryManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePreInitUnlocked__)(struct OBJGPU *, struct MemoryManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__memmgrStateInitUnlocked__)(struct OBJGPU *, struct MemoryManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePreLoad__)(struct OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__memmgrStatePostUnload__)(struct OBJGPU *, struct MemoryManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__memmgrIsPresent__)(struct OBJGPU *, struct MemoryManager * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryManager {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__MemoryManager vtable;
};

#ifndef __NVOC_CLASS_MemoryManager_TYPEDEF__
#define __NVOC_CLASS_MemoryManager_TYPEDEF__
typedef struct MemoryManager MemoryManager;
#endif /* __NVOC_CLASS_MemoryManager_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryManager
#define __nvoc_class_id_MemoryManager 0x22ad47
#endif /* __nvoc_class_id_MemoryManager */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryManager;

#define __staticCast_MemoryManager(pThis) \
    ((pThis)->__nvoc_pbase_MemoryManager)

#ifdef __nvoc_mem_mgr_h_disabled
#define __dynamicCast_MemoryManager(pThis) ((MemoryManager*) NULL)
#else //__nvoc_mem_mgr_h_disabled
#define __dynamicCast_MemoryManager(pThis) \
    ((MemoryManager*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryManager)))
#endif //__nvoc_mem_mgr_h_disabled

// Property macros
#define PDB_PROP_MEMMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_MEMMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryManager(MemoryManager**, Dynamic*, NvU32);
#define __objCreate_MemoryManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_MemoryManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros for implementation functions
void memmgrDestruct_IMPL(struct MemoryManager *pMemoryManager);
#define __nvoc_memmgrDestruct(pMemoryManager) memmgrDestruct_IMPL(pMemoryManager)

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSavePowerMgmtState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrSavePowerMgmtState(pGpu, pMemoryManager) memmgrSavePowerMgmtState_KERNEL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrRestorePowerMgmtState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRestorePowerMgmtState(pGpu, pMemoryManager) memmgrRestorePowerMgmtState_KERNEL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocResources(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo) memmgrAllocResources_IMPL(pGpu, pMemoryManager, pAllocRequest, pFbAllocInfo)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFree_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *arg3, NvHandle arg4, NvHandle arg5, NvHandle arg6, NvU32 arg7, MEMORY_DESCRIPTOR *arg8);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFree(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *arg3, NvHandle arg4, NvHandle arg5, NvHandle arg6, NvU32 arg7, MEMORY_DESCRIPTOR *arg8) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFree(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8) memmgrFree_IMPL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAddMemNode_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvBool bFreeDescriptor);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAddMemNode(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvBool bFreeDescriptor) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAddMemNode(pGpu, pMemoryManager, pMemDesc, bFreeDescriptor) memmgrAddMemNode_IMPL(pGpu, pMemoryManager, pMemDesc, bFreeDescriptor)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAddMemNodes_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool bSaveAllRmAllocations);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAddMemNodes(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool bSaveAllRmAllocations) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAddMemNodes(pGpu, pMemoryManager, bSaveAllRmAllocations) memmgrAddMemNodes_IMPL(pGpu, pMemoryManager, bSaveAllRmAllocations)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrRemoveMemNodes_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRemoveMemNodes(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRemoveMemNodes(pGpu, pMemoryManager) memmgrRemoveMemNodes_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

struct Heap * memmgrGetDeviceSuballocator_IMPL(struct MemoryManager *pMemoryManager, NvBool bForceSubheap);
#ifdef __nvoc_mem_mgr_h_disabled
static inline struct Heap * memmgrGetDeviceSuballocator(struct MemoryManager *pMemoryManager, NvBool bForceSubheap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetDeviceSuballocator(pMemoryManager, bForceSubheap) memmgrGetDeviceSuballocator_IMPL(pMemoryManager, bForceSubheap)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemCopy_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, TRANSFER_SURFACE *pSrc, NvU32 size, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemCopy(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, TRANSFER_SURFACE *pSrc, NvU32 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemCopy(pMemoryManager, pDst, pSrc, size, flags) memmgrMemCopy_IMPL(pMemoryManager, pDst, pSrc, size, flags)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemsetInBlocks_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU64 baseOffset, NvU64 size, NvU32 flags, NvU32 blockSize);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemsetInBlocks(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU64 baseOffset, NvU64 size, NvU32 flags, NvU32 blockSize) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemsetInBlocks(pMemoryManager, pMemDesc, value, baseOffset, size, flags, blockSize) memmgrMemsetInBlocks_IMPL(pMemoryManager, pMemDesc, value, baseOffset, size, flags, blockSize)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemSet_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, NvU32 value, NvU32 size, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemSet(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, NvU32 value, NvU32 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemSet(pMemoryManager, pDst, value, size, flags) memmgrMemSet_IMPL(pMemoryManager, pDst, value, size, flags)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemWrite_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, void *pBuf, NvU64 size, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemWrite(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pDst, void *pBuf, NvU64 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemWrite(pMemoryManager, pDst, pBuf, size, flags) memmgrMemWrite_IMPL(pMemoryManager, pDst, pBuf, size, flags)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemRead_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pSrc, void *pBuf, NvU64 size, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemRead(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pSrc, void *pBuf, NvU64 size, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemRead(pMemoryManager, pSrc, pBuf, size, flags) memmgrMemRead_IMPL(pMemoryManager, pSrc, pBuf, size, flags)
#endif // __nvoc_mem_mgr_h_disabled

NvU8 * memmgrMemBeginTransfer_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU8 * memmgrMemBeginTransfer(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemBeginTransfer(pMemoryManager, pTransferInfo, shadowBufSize, flags) memmgrMemBeginTransfer_IMPL(pMemoryManager, pTransferInfo, shadowBufSize, flags)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrMemEndTransfer_IMPL(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemEndTransfer(struct MemoryManager *pMemoryManager, TRANSFER_SURFACE *pTransferInfo, NvU64 shadowBufSize, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemEndTransfer(pMemoryManager, pTransferInfo, shadowBufSize, flags) memmgrMemEndTransfer_IMPL(pMemoryManager, pTransferInfo, shadowBufSize, flags)
#endif // __nvoc_mem_mgr_h_disabled

NvU8 * memmgrMemDescBeginTransfer_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU8 * memmgrMemDescBeginTransfer(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemDescBeginTransfer(pMemoryManager, pMemDesc, flags) memmgrMemDescBeginTransfer_IMPL(pMemoryManager, pMemDesc, flags)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrMemDescEndTransfer_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemDescEndTransfer(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemDescEndTransfer(pMemoryManager, pMemDesc, flags) memmgrMemDescEndTransfer_IMPL(pMemoryManager, pMemDesc, flags)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemDescMemSet_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU32 flags);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemDescMemSet(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *pMemDesc, NvU32 value, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemDescMemSet(pMemoryManager, pMemDesc, value, flags) memmgrMemDescMemSet_IMPL(pMemoryManager, pMemDesc, value, flags)
#endif // __nvoc_mem_mgr_h_disabled

NV_ADDRESS_SPACE memmgrAllocGetAddrSpace_IMPL(struct MemoryManager *pMemoryManager, NvU32 flags, NvU32 attr);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_ADDRESS_SPACE memmgrAllocGetAddrSpace(struct MemoryManager *pMemoryManager, NvU32 flags, NvU32 attr) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocGetAddrSpace(pMemoryManager, flags, attr) memmgrAllocGetAddrSpace_IMPL(pMemoryManager, flags, attr)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrGetUsedRamSize_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetUsedRamSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetUsedRamSize(pGpu, pMemoryManager, arg3) memmgrGetUsedRamSize_IMPL(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrCheckZeroPmaUsage_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrCheckZeroPmaUsage(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrCheckZeroPmaUsage(pGpu, pMemoryManager) memmgrCheckZeroPmaUsage_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocHwResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocHwResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocHwResources(pGpu, pMemoryManager, arg3) memmgrAllocHwResources_IMPL(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

NvBool memmgrLargePageSupported_IMPL(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrLargePageSupported(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrLargePageSupported(pMemoryManager, arg2) memmgrLargePageSupported_IMPL(pMemoryManager, arg2)
#endif // __nvoc_mem_mgr_h_disabled

NvBool memmgrComprSupported_IMPL(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrComprSupported(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrComprSupported(pMemoryManager, arg2) memmgrComprSupported_IMPL(pMemoryManager, arg2)
#endif // __nvoc_mem_mgr_h_disabled

NvU64 memmgrGetMappableRamSizeMb_IMPL(struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetMappableRamSizeMb(struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetMappableRamSizeMb(pMemoryManager) memmgrGetMappableRamSizeMb_IMPL(pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

PFB_REGION_DESCRIPTOR memmgrLookupFbRegionByOffset_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr fbOffset, RmPhysAddr fbLimit);
#ifdef __nvoc_mem_mgr_h_disabled
static inline PFB_REGION_DESCRIPTOR memmgrLookupFbRegionByOffset(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr fbOffset, RmPhysAddr fbLimit) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrLookupFbRegionByOffset(pGpu, pMemoryManager, fbOffset, fbLimit) memmgrLookupFbRegionByOffset_IMPL(pGpu, pMemoryManager, fbOffset, fbLimit)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFillMemdescForPhysAttr_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg3, ADDRESS_TRANSLATION arg4, NvU64 *arg5, NvU32 *arg6, NvU32 *arg7, NvU32 *arg8, NvU32 *arg9, NvU32 *arg10, NvU64 *arg11);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFillMemdescForPhysAttr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg3, ADDRESS_TRANSLATION arg4, NvU64 *arg5, NvU32 *arg6, NvU32 *arg7, NvU32 *arg8, NvU32 *arg9, NvU32 *arg10, NvU64 *arg11) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFillMemdescForPhysAttr(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) memmgrFillMemdescForPhysAttr_IMPL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrSetPlatformPmaSupport_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetPlatformPmaSupport(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrSetPlatformPmaSupport(pGpu, pMemoryManager) memmgrSetPlatformPmaSupport_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrRegionSetupForPma_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegionSetupForPma(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRegionSetupForPma(pGpu, pMemoryManager) memmgrRegionSetupForPma_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrRegionSetupCommon_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegionSetupCommon(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRegionSetupCommon(pGpu, pMemoryManager) memmgrRegionSetupCommon_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrRegenerateFbRegionPriority_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrRegenerateFbRegionPriority(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRegenerateFbRegionPriority(pGpu, pMemoryManager) memmgrRegenerateFbRegionPriority_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NvU32 memmgrInsertFbRegion_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PFB_REGION_DESCRIPTOR arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrInsertFbRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PFB_REGION_DESCRIPTOR arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrInsertFbRegion(pGpu, pMemoryManager, arg3) memmgrInsertFbRegion_IMPL(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrDumpFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrDumpFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrDumpFbRegions(pGpu, pMemoryManager) memmgrDumpFbRegions_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrClearFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrClearFbRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrClearFbRegions(pGpu, pMemoryManager) memmgrClearFbRegions_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveVbiosVgaRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveVbiosVgaRegions(pGpu, pMemoryManager) memmgrReserveVbiosVgaRegions_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveConsoleRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveConsoleRegion(pGpu, pMemoryManager) memmgrReserveConsoleRegion_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrReleaseConsoleRegion_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrReleaseConsoleRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReleaseConsoleRegion(pGpu, pMemoryManager) memmgrReleaseConsoleRegion_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

PMEMORY_DESCRIPTOR memmgrGetReservedConsoleMemDesc_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline PMEMORY_DESCRIPTOR memmgrGetReservedConsoleMemDesc(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetReservedConsoleMemDesc(pGpu, pMemoryManager) memmgrGetReservedConsoleMemDesc_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrReserveBar2BackingStore_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrReserveBar2BackingStore(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveBar2BackingStore(pGpu, pMemoryManager, arg3) memmgrReserveBar2BackingStore_IMPL(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrCalcReservedFbSpace_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrCalcReservedFbSpace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrCalcReservedFbSpace(pGpu, pMemoryManager) memmgrCalcReservedFbSpace_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrMemUtilsSetupChannelBufferSizes_IMPL(struct MemoryManager *pMemoryManager, OBJCHANNEL *arg2, NvU32 arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrMemUtilsSetupChannelBufferSizes(struct MemoryManager *pMemoryManager, OBJCHANNEL *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsSetupChannelBufferSizes(pMemoryManager, arg2, arg3) memmgrMemUtilsSetupChannelBufferSizes_IMPL(pMemoryManager, arg2, arg3)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrMemUtilsChannelSchedulingSetup_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsChannelSchedulingSetup(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsChannelSchedulingSetup(pGpu, pMemoryManager, arg3) memmgrMemUtilsChannelSchedulingSetup_IMPL(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetKindComprForGpu(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg2, OBJGPU *pGpu, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetKindComprForGpu(pMemoryManager, arg2, pGpu, offset, kind, pComprInfo) memmgrGetKindComprForGpu_KERNEL(pMemoryManager, arg2, pGpu, offset, kind, pComprInfo)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrComprMappingSupported(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrComprMappingSupported(pMemoryManager, arg2) memmgrComprMappingSupported_88bc07(pMemoryManager, arg2)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrGetKindComprFromMemDesc_IMPL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg2, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetKindComprFromMemDesc(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg2, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetKindComprFromMemDesc(pMemoryManager, arg2, offset, kind, pComprInfo) memmgrGetKindComprFromMemDesc_IMPL(pMemoryManager, arg2, offset, kind, pComprInfo)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFillComprInfo_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3, NvU32 arg4, NvU32 arg5, NvU64 arg6, NvU32 arg7, COMPR_INFO *arg8);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFillComprInfo(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3, NvU32 arg4, NvU32 arg5, NvU64 arg6, NvU32 arg7, COMPR_INFO *arg8) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFillComprInfo(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8) memmgrFillComprInfo_IMPL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7, arg8)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrComprInfoDisableCompression_IMPL(struct MemoryManager *pMemoryManager, COMPR_INFO *pComprInfo);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrComprInfoDisableCompression(struct MemoryManager *pMemoryManager, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrComprInfoDisableCompression(pMemoryManager, pComprInfo) memmgrComprInfoDisableCompression_IMPL(pMemoryManager, pComprInfo)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrFillComprInfoUncompressed_IMPL(struct MemoryManager *pMemoryManager, NvU32 kind, COMPR_INFO *pComprInfo);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFillComprInfoUncompressed(struct MemoryManager *pMemoryManager, NvU32 kind, COMPR_INFO *pComprInfo) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFillComprInfoUncompressed(pMemoryManager, kind, pComprInfo) memmgrFillComprInfoUncompressed_IMPL(pMemoryManager, kind, pComprInfo)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPmaInitialize_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMA **ppPma);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPmaInitialize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMA **ppPma) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrPmaInitialize(pGpu, pMemoryManager, ppPma) memmgrPmaInitialize_IMPL(pGpu, pMemoryManager, ppPma)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPmaRegisterRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap, PMA *pPma);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPmaRegisterRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap, PMA *pPma) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrPmaRegisterRegions(pGpu, pMemoryManager, pHeap, pPma) memmgrPmaRegisterRegions_IMPL(pGpu, pMemoryManager, pHeap, pPma)
#endif // __nvoc_mem_mgr_h_disabled

NvU64 memmgrGetClientFbAddrSpaceSize_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetClientFbAddrSpaceSize(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetClientFbAddrSpaceSize(pGpu, pMemoryManager) memmgrGetClientFbAddrSpaceSize_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrScrubInit(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrScrubInit(pGpu, pMemoryManager) memmgrScrubInit_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrInitInternalChannels_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitInternalChannels(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrInitInternalChannels(pGpu, pMemoryManager) memmgrInitInternalChannels_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrDestroyInternalChannels_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrDestroyInternalChannels(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrDestroyInternalChannels(pGpu, pMemoryManager) memmgrDestroyInternalChannels_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetScrubState(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3, NvU64 *arg4, NvBool *arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetScrubState(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrGetScrubState_f2d351(pGpu, pMemoryManager, arg3, arg4, arg5)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubInternalRegions(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrScrubInternalRegions(pGpu, pMemoryManager) memmgrScrubInternalRegions_b3696a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrEccScrubInProgress(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrEccScrubInProgress(pGpu, pMemoryManager) memmgrEccScrubInProgress_3dd2c9(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrAsyncScrubRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAsyncScrubRegion(pGpu, pMemoryManager, arg3, arg4) memmgrAsyncScrubRegion_f2d351(pGpu, pMemoryManager, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubDestroy(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrScrubDestroy(pGpu, pMemoryManager) memmgrScrubDestroy_b3696a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrScrubMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrScrubMemory(pGpu, pMemoryManager, arg3, arg4) memmgrScrubMemory_b3696a(pGpu, pMemoryManager, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsMemSetBlocking(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3, RmPhysAddr arg4, NvU64 arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsMemSetBlocking(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrMemUtilsMemSetBlocking_92bfc3(pGpu, pMemoryManager, arg3, arg4, arg5)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsMemSet(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3, RmPhysAddr arg4, NvU64 arg5, NvU32 arg6, NvU32 *arg7) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsMemSet(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7) memmgrMemUtilsMemSet_92bfc3(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsAllocateEccScrubber(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsAllocateEccScrubber(pGpu, pMemoryManager, arg3) memmgrMemUtilsAllocateEccScrubber_92bfc3(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrMemUtilsAllocateEccAllocScrubber(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsAllocateEccAllocScrubber(pGpu, pMemoryManager, arg3) memmgrMemUtilsAllocateEccAllocScrubber_92bfc3(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrInitCeUtils_IMPL(struct MemoryManager *pMemoryManager, NvBool bFifoLite, NvBool bVirtualMode);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitCeUtils(struct MemoryManager *pMemoryManager, NvBool bFifoLite, NvBool bVirtualMode) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrInitCeUtils(pMemoryManager, bFifoLite, bVirtualMode) memmgrInitCeUtils_IMPL(pMemoryManager, bFifoLite, bVirtualMode)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrDestroyCeUtils_IMPL(struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrDestroyCeUtils(struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrDestroyCeUtils(pMemoryManager) memmgrDestroyCeUtils_IMPL(pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrGetInternalClientHandles_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *arg3, NvHandle *arg4, NvHandle *arg5, NvHandle *arg6);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetInternalClientHandles(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *arg3, NvHandle *arg4, NvHandle *arg5, NvHandle *arg6) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetInternalClientHandles(pGpu, pMemoryManager, arg3, arg4, arg5, arg6) memmgrGetInternalClientHandles_IMPL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrUpdateSurfaceCompression(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrUpdateSurfaceCompression(pGpu, pMemoryManager, arg3, arg4) memmgrUpdateSurfaceCompression_5baef9(pGpu, pMemoryManager, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrInitFbRegionsHal(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrInitFbRegionsHal(pGpu, pMemoryManager) memmgrInitFbRegionsHal_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU64 memmgrGetVgpuHostRmReservedFb(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetVgpuHostRmReservedFb(pGpu, pMemoryManager, vgpuTypeId) memmgrGetVgpuHostRmReservedFb_KERNEL(pGpu, pMemoryManager, vgpuTypeId)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrVerifyDepthSurfaceAttrs(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrVerifyDepthSurfaceAttrs(pMemoryManager, arg2, arg3) memmgrVerifyDepthSurfaceAttrs_88bc07(pMemoryManager, arg2, arg3)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg3, MEMORY_DESCRIPTOR **arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocMemToSaveVgaWorkspace(pGpu, pMemoryManager, arg3, arg4) memmgrAllocMemToSaveVgaWorkspace_5baef9(pGpu, pMemoryManager, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrVerifyComprAttrs(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrVerifyComprAttrs(pMemoryManager, arg2, arg3, arg4) memmgrVerifyComprAttrs_88bc07(pMemoryManager, arg2, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsKindBlocklinear(struct MemoryManager *pMemoryManager, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrIsKindBlocklinear(pMemoryManager, arg2) memmgrIsKindBlocklinear_3dd2c9(pMemoryManager, arg2)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvU32 memmgrChooseKindCompressCForMS2(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return 0;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrChooseKindCompressCForMS2(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressCForMS2_4a4dee(pGpu, pMemoryManager, arg3)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetUncompressedKindForMS2(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetUncompressedKindForMS2(pGpu, pMemoryManager, arg3, arg4) memmgrGetUncompressedKindForMS2_5baef9(pGpu, pMemoryManager, arg3, arg4)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NvBool memmgrIsSurfaceBlockLinear(struct MemoryManager *pMemoryManager, Memory *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_FALSE;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrIsSurfaceBlockLinear(pMemoryManager, arg2, arg3) memmgrIsSurfaceBlockLinear_TU102(pMemoryManager, arg2, arg3)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrChannelPushSemaphoreMethodsBlock(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU64 arg3, NvU32 arg4, NvU32 **arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrChannelPushSemaphoreMethodsBlock(pMemoryManager, arg2, arg3, arg4, arg5) memmgrChannelPushSemaphoreMethodsBlock_f2d351(pMemoryManager, arg2, arg3, arg4, arg5)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrChannelPushAddressMethodsBlock(struct MemoryManager *pMemoryManager, NvBool arg2, NvU32 arg3, RmPhysAddr arg4, NvU32 **arg5) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrChannelPushAddressMethodsBlock(pMemoryManager, arg2, arg3, arg4, arg5) memmgrChannelPushAddressMethodsBlock_f2d351(pMemoryManager, arg2, arg3, arg4, arg5)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrSetMIGPartitionableBAR1Range_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSetMIGPartitionableBAR1Range(OBJGPU *arg1, struct MemoryManager *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrSetMIGPartitionableBAR1Range(arg1, arg_this) memmgrSetMIGPartitionableBAR1Range_IMPL(arg1, arg_this)
#endif // __nvoc_mem_mgr_h_disabled

struct NV_RANGE memmgrGetMIGPartitionableBAR1Range_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this);
#ifdef __nvoc_mem_mgr_h_disabled
static inline struct NV_RANGE memmgrGetMIGPartitionableBAR1Range(OBJGPU *arg1, struct MemoryManager *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    struct NV_RANGE ret;
    portMemSet(&ret, 0, sizeof(struct NV_RANGE));
    return ret;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetMIGPartitionableBAR1Range(arg1, arg_this) memmgrGetMIGPartitionableBAR1Range_IMPL(arg1, arg_this)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrSetMIGPartitionableMemoryRange_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this, struct NV_RANGE arg3);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrSetMIGPartitionableMemoryRange(OBJGPU *arg1, struct MemoryManager *arg_this, struct NV_RANGE arg3) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrSetMIGPartitionableMemoryRange(arg1, arg_this, arg3) memmgrSetMIGPartitionableMemoryRange_IMPL(arg1, arg_this, arg3)
#endif // __nvoc_mem_mgr_h_disabled

struct NV_RANGE memmgrGetMIGPartitionableMemoryRange_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this);
#ifdef __nvoc_mem_mgr_h_disabled
static inline struct NV_RANGE memmgrGetMIGPartitionableMemoryRange(OBJGPU *arg1, struct MemoryManager *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    struct NV_RANGE ret;
    portMemSet(&ret, 0, sizeof(struct NV_RANGE));
    return ret;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetMIGPartitionableMemoryRange(arg1, arg_this) memmgrGetMIGPartitionableMemoryRange_IMPL(arg1, arg_this)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrFreeMIGGPUInstanceMemory_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle hMemory, struct Heap **ppMemoryPartitionHeap);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrFreeMIGGPUInstanceMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle hMemory, struct Heap **ppMemoryPartitionHeap) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFreeMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, hMemory, ppMemoryPartitionHeap) memmgrFreeMIGGPUInstanceMemory_IMPL(pGpu, pMemoryManager, swizzId, hMemory, ppMemoryPartitionHeap)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPageLevelPoolsCreate_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPageLevelPoolsCreate(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsCreate(pGpu, pMemoryManager) memmgrPageLevelPoolsCreate_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrPageLevelPoolsDestroy_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrPageLevelPoolsDestroy(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsDestroy(pGpu, pMemoryManager) memmgrPageLevelPoolsDestroy_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrPageLevelPoolsGetInfo_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, struct RM_POOL_ALLOC_MEM_RESERVE_INFO **arg4);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrPageLevelPoolsGetInfo(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, struct RM_POOL_ALLOC_MEM_RESERVE_INFO **arg4) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrPageLevelPoolsGetInfo(pGpu, pMemoryManager, pDevice, arg4) memmgrPageLevelPoolsGetInfo_IMPL(pGpu, pMemoryManager, pDevice, arg4)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocMIGMemoryAllocationInternalHandles_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocMIGMemoryAllocationInternalHandles(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager) memmgrAllocMIGMemoryAllocationInternalHandles_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrFreeMIGMemoryAllocationInternalHandles_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFreeMIGMemoryAllocationInternalHandles(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFreeMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager) memmgrFreeMIGMemoryAllocationInternalHandles_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrGetFreeMemoryForAllMIGGPUInstances_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetFreeMemoryForAllMIGGPUInstances(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetFreeMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, pBytes) memmgrGetFreeMemoryForAllMIGGPUInstances_IMPL(pGpu, pMemoryManager, pBytes)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrGetTotalMemoryForAllMIGGPUInstances_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetTotalMemoryForAllMIGGPUInstances(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pBytes) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetTotalMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, pBytes) memmgrGetTotalMemoryForAllMIGGPUInstances_IMPL(pGpu, pMemoryManager, pBytes)
#endif // __nvoc_mem_mgr_h_disabled

void memmgrGetTopLevelScrubberStatus_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this, NvBool *pbTopLevelScrubberEnabled, NvBool *pbTopLevelScrubberConstructed);
#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrGetTopLevelScrubberStatus(OBJGPU *arg1, struct MemoryManager *arg_this, NvBool *pbTopLevelScrubberEnabled, NvBool *pbTopLevelScrubberConstructed) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetTopLevelScrubberStatus(arg1, arg_this, pbTopLevelScrubberEnabled, pbTopLevelScrubberConstructed) memmgrGetTopLevelScrubberStatus_IMPL(arg1, arg_this, pbTopLevelScrubberEnabled, pbTopLevelScrubberConstructed)
#endif // __nvoc_mem_mgr_h_disabled

MEMORY_DESCRIPTOR * memmgrMemUtilsGetMemDescFromHandle_IMPL(struct MemoryManager *pMemoryManager, NvHandle hClient, NvHandle hMemory);
#ifdef __nvoc_mem_mgr_h_disabled
static inline MEMORY_DESCRIPTOR * memmgrMemUtilsGetMemDescFromHandle(struct MemoryManager *pMemoryManager, NvHandle hClient, NvHandle hMemory) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NULL;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, hClient, hMemory) memmgrMemUtilsGetMemDescFromHandle_IMPL(pMemoryManager, hClient, hMemory)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrVerifyGspDmaOps_IMPL(OBJGPU *arg1, struct MemoryManager *arg_this);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrVerifyGspDmaOps(OBJGPU *arg1, struct MemoryManager *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrVerifyGspDmaOps(arg1, arg_this) memmgrVerifyGspDmaOps_IMPL(arg1, arg_this)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrValidateFBEndReservation(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrValidateFBEndReservation(pGpu, pMemoryManager) memmgrValidateFBEndReservation_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrAllocReservedFBRegionMemdesc_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **ppMemdesc, NvU64 rangeStart, NvU64 allocSize, NvU64 memdescFlags, NV_FB_ALLOC_RM_INTERNAL_OWNER allocTag);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrAllocReservedFBRegionMemdesc(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **ppMemdesc, NvU64 rangeStart, NvU64 allocSize, NvU64 memdescFlags, NV_FB_ALLOC_RM_INTERNAL_OWNER allocTag) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrAllocReservedFBRegionMemdesc(pGpu, pMemoryManager, ppMemdesc, rangeStart, allocSize, memdescFlags, allocTag) memmgrAllocReservedFBRegionMemdesc_IMPL(pGpu, pMemoryManager, ppMemdesc, rangeStart, allocSize, memdescFlags, allocTag)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveMemoryForFakeWPR(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveMemoryForFakeWPR(pGpu, pMemoryManager) memmgrReserveMemoryForFakeWPR_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveMemoryForPmu(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveMemoryForPmu(pGpu, pMemoryManager) memmgrReserveMemoryForPmu_56cd7a(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

NV_STATUS memmgrReserveMemoryForFsp_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);
#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveMemoryForFsp(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveMemoryForFsp(pGpu, pMemoryManager) memmgrReserveMemoryForFsp_IMPL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline void memmgrFreeFbsrMemory(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrFreeFbsrMemory(pGpu, pMemoryManager) memmgrFreeFbsrMemory_KERNEL(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrReserveVgaWorkspaceMemDescForFbsr(pGpu, pMemoryManager) memmgrReserveVgaWorkspaceMemDescForFbsr_46f6a7(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrCalculateHeapOffsetWithGSP(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *offset) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrCalculateHeapOffsetWithGSP(pGpu, pMemoryManager, offset) memmgrCalculateHeapOffsetWithGSP_46f6a7(pGpu, pMemoryManager, offset)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrSaveDisplayFrmReservedRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrSaveDisplayFrmReservedRegion(pGpu, pMemoryManager) memmgrSaveDisplayFrmReservedRegion_46f6a7(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrRestoreDisplayFrmReservedRegion(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrRestoreDisplayFrmReservedRegion(pGpu, pMemoryManager) memmgrRestoreDisplayFrmReservedRegion_46f6a7(pGpu, pMemoryManager)
#endif // __nvoc_mem_mgr_h_disabled

#ifdef __nvoc_mem_mgr_h_disabled
static inline NV_STATUS memmgrGetCarveoutRegionInfo(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MemoryManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mgr_h_disabled
#define memmgrGetCarveoutRegionInfo(pGpu, pMemoryManager, pParams) memmgrGetCarveoutRegionInfo_KERNEL(pGpu, pMemoryManager, pParams)
#endif // __nvoc_mem_mgr_h_disabled


// Wrapper macros for halified functions
#define memmgrConstructEngine_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrConstructEngine__
#define memmgrConstructEngine(pGpu, pMemoryManager, arg3) memmgrConstructEngine_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrStatePreInitLocked_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePreInitLocked__
#define memmgrStatePreInitLocked(pGpu, pMemoryManager) memmgrStatePreInitLocked_DISPATCH(pGpu, pMemoryManager)
#define memmgrStateInitLocked_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateInitLocked__
#define memmgrStateInitLocked(pGpu, pMemoryManager) memmgrStateInitLocked_DISPATCH(pGpu, pMemoryManager)
#define memmgrStateLoad_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateLoad__
#define memmgrStateLoad(pGpu, pMemoryManager, arg3) memmgrStateLoad_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrStatePostLoad_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePostLoad__
#define memmgrStatePostLoad(pGpu, pMemoryManager, arg3) memmgrStatePostLoad_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrStatePreUnload_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePreUnload__
#define memmgrStatePreUnload(pGpu, pMemoryManager, arg3) memmgrStatePreUnload_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrStateUnload_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateUnload__
#define memmgrStateUnload(pGpu, pMemoryManager, arg3) memmgrStateUnload_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrStateDestroy_FNPTR(pMemoryManager) pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateDestroy__
#define memmgrStateDestroy(pGpu, pMemoryManager) memmgrStateDestroy_DISPATCH(pGpu, pMemoryManager)
#define memmgrSavePowerMgmtState_HAL(pGpu, pMemoryManager) memmgrSavePowerMgmtState(pGpu, pMemoryManager)
#define memmgrRestorePowerMgmtState_HAL(pGpu, pMemoryManager) memmgrRestorePowerMgmtState(pGpu, pMemoryManager)
#define memmgrDeterminePageSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrDeterminePageSize__
#define memmgrDeterminePageSize(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2) memmgrDeterminePageSize_DISPATCH(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2)
#define memmgrDeterminePageSize_HAL(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2) memmgrDeterminePageSize_DISPATCH(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2)
#define memmgrFreeHwResources_FNPTR(pMemoryManager) pMemoryManager->__memmgrFreeHwResources__
#define memmgrFreeHwResources(pGpu, pMemoryManager, arg3) memmgrFreeHwResources_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrFreeHwResources_HAL(pGpu, pMemoryManager, arg3) memmgrFreeHwResources_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrCreateHeap_FNPTR(pMemoryManager) pMemoryManager->__memmgrCreateHeap__
#define memmgrCreateHeap(pMemoryManager) memmgrCreateHeap_DISPATCH(pMemoryManager)
#define memmgrCreateHeap_HAL(pMemoryManager) memmgrCreateHeap_DISPATCH(pMemoryManager)
#define memmgrInitFbRegions_FNPTR(pMemoryManager) pMemoryManager->__memmgrInitFbRegions__
#define memmgrInitFbRegions(pGpu, pMemoryManager) memmgrInitFbRegions_DISPATCH(pGpu, pMemoryManager)
#define memmgrInitFbRegions_HAL(pGpu, pMemoryManager) memmgrInitFbRegions_DISPATCH(pGpu, pMemoryManager)
#define memmgrReserveVbiosVgaRegions_HAL(pGpu, pMemoryManager) memmgrReserveVbiosVgaRegions(pGpu, pMemoryManager)
#define memmgrReserveConsoleRegion_HAL(pGpu, pMemoryManager) memmgrReserveConsoleRegion(pGpu, pMemoryManager)
#define memmgrAllocateConsoleRegion_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocateConsoleRegion__
#define memmgrAllocateConsoleRegion(pGpu, pMemoryManager) memmgrAllocateConsoleRegion_DISPATCH(pGpu, pMemoryManager)
#define memmgrAllocateConsoleRegion_HAL(pGpu, pMemoryManager) memmgrAllocateConsoleRegion_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetKindComprForGpu_HAL(pMemoryManager, arg2, pGpu, offset, kind, pComprInfo) memmgrGetKindComprForGpu(pMemoryManager, arg2, pGpu, offset, kind, pComprInfo)
#define memmgrComprMappingSupported_HAL(pMemoryManager, arg2) memmgrComprMappingSupported(pMemoryManager, arg2)
#define memmgrScrubInit_HAL(pGpu, pMemoryManager) memmgrScrubInit(pGpu, pMemoryManager)
#define memmgrScrubHandlePostSchedulingEnable_FNPTR(pMemoryManager) pMemoryManager->__memmgrScrubHandlePostSchedulingEnable__
#define memmgrScrubHandlePostSchedulingEnable(pGpu, pMemoryManager) memmgrScrubHandlePostSchedulingEnable_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubHandlePostSchedulingEnable_HAL(pGpu, pMemoryManager) memmgrScrubHandlePostSchedulingEnable_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetScrubState_HAL(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrGetScrubState(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrScrubInternalRegions_HAL(pGpu, pMemoryManager) memmgrScrubInternalRegions(pGpu, pMemoryManager)
#define memmgrEccScrubInProgress_HAL(pGpu, pMemoryManager) memmgrEccScrubInProgress(pGpu, pMemoryManager)
#define memmgrAsyncScrubRegion_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrAsyncScrubRegion(pGpu, pMemoryManager, arg3, arg4)
#define memmgrScrubHandlePreSchedulingDisable_FNPTR(pMemoryManager) pMemoryManager->__memmgrScrubHandlePreSchedulingDisable__
#define memmgrScrubHandlePreSchedulingDisable(pGpu, pMemoryManager) memmgrScrubHandlePreSchedulingDisable_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubHandlePreSchedulingDisable_HAL(pGpu, pMemoryManager) memmgrScrubHandlePreSchedulingDisable_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubDestroy_HAL(pGpu, pMemoryManager) memmgrScrubDestroy(pGpu, pMemoryManager)
#define memmgrScrubMemory_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrScrubMemory(pGpu, pMemoryManager, arg3, arg4)
#define memmgrMemUtilsMemSetBlocking_HAL(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrMemUtilsMemSetBlocking(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrMemUtilsMemSet_HAL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7) memmgrMemUtilsMemSet(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7)
#define memmgrMemUtilsAllocateEccScrubber_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsAllocateEccScrubber(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsAllocateEccAllocScrubber_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsAllocateEccAllocScrubber(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsChannelInitialize_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsChannelInitialize__
#define memmgrMemUtilsChannelInitialize(pGpu, pMemoryManager, arg3) memmgrMemUtilsChannelInitialize_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsChannelInitialize_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsCopyEngineInitialize_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsCopyEngineInitialize__
#define memmgrMemUtilsCopyEngineInitialize(pGpu, pMemoryManager, arg3) memmgrMemUtilsCopyEngineInitialize_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsCopyEngineInitialize_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsSec2CtxInit_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsSec2CtxInit__
#define memmgrMemUtilsSec2CtxInit(pGpu, pMemoryManager, arg3) memmgrMemUtilsSec2CtxInit_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsSec2CtxInit_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsSec2CtxInit_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsGetCopyEngineClass_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsGetCopyEngineClass__
#define memmgrMemUtilsGetCopyEngineClass(pGpu, pMemoryManager, pClass) memmgrMemUtilsGetCopyEngineClass_DISPATCH(pGpu, pMemoryManager, pClass)
#define memmgrMemUtilsGetCopyEngineClass_HAL(pGpu, pMemoryManager, pClass) memmgrMemUtilsGetCopyEngineClass_DISPATCH(pGpu, pMemoryManager, pClass)
#define memmgrMemUtilsCreateMemoryAlias_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsCreateMemoryAlias__
#define memmgrMemUtilsCreateMemoryAlias(pGpu, pMemoryManager, arg3) memmgrMemUtilsCreateMemoryAlias_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsCreateMemoryAlias_HAL(pGpu, pMemoryManager, arg3) memmgrMemUtilsCreateMemoryAlias_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrMemUtilsCheckMemoryFastScrubEnable_FNPTR(pMemoryManager) pMemoryManager->__memmgrMemUtilsCheckMemoryFastScrubEnable__
#define memmgrMemUtilsCheckMemoryFastScrubEnable(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7) memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7)
#define memmgrMemUtilsCheckMemoryFastScrubEnable_HAL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7) memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7)
#define memmgrAllocHal_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocHal__
#define memmgrAllocHal(pGpu, pMemoryManager, pFbAllocInfo) memmgrAllocHal_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo)
#define memmgrAllocHal_HAL(pGpu, pMemoryManager, pFbAllocInfo) memmgrAllocHal_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo)
#define memmgrFreeHal_FNPTR(pMemoryManager) pMemoryManager->__memmgrFreeHal__
#define memmgrFreeHal(pGpu, pMemoryManager, pFbAllocInfo, pTimeout) memmgrFreeHal_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo, pTimeout)
#define memmgrFreeHal_HAL(pGpu, pMemoryManager, pFbAllocInfo, pTimeout) memmgrFreeHal_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo, pTimeout)
#define memmgrUpdateSurfaceCompression_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrUpdateSurfaceCompression(pGpu, pMemoryManager, arg3, arg4)
#define memmgrGetBankPlacementData_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetBankPlacementData__
#define memmgrGetBankPlacementData(pGpu, pMemoryManager, pBankPlacementLowData) memmgrGetBankPlacementData_DISPATCH(pGpu, pMemoryManager, pBankPlacementLowData)
#define memmgrGetBankPlacementData_HAL(pGpu, pMemoryManager, pBankPlacementLowData) memmgrGetBankPlacementData_DISPATCH(pGpu, pMemoryManager, pBankPlacementLowData)
#define memmgrDirtyForPmTest_FNPTR(pMemoryManager) pMemoryManager->__memmgrDirtyForPmTest__
#define memmgrDirtyForPmTest(pGpu, pMemoryManager, partialDirty) memmgrDirtyForPmTest_DISPATCH(pGpu, pMemoryManager, partialDirty)
#define memmgrDirtyForPmTest_HAL(pGpu, pMemoryManager, partialDirty) memmgrDirtyForPmTest_DISPATCH(pGpu, pMemoryManager, partialDirty)
#define memmgrGetReservedHeapSizeMb_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetReservedHeapSizeMb__
#define memmgrGetReservedHeapSizeMb(pGpu, pMemoryManager) memmgrGetReservedHeapSizeMb_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetReservedHeapSizeMb_HAL(pGpu, pMemoryManager) memmgrGetReservedHeapSizeMb_DISPATCH(pGpu, pMemoryManager)
#define memmgrAllocDetermineAlignment_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocDetermineAlignment__
#define memmgrAllocDetermineAlignment(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment) memmgrAllocDetermineAlignment_DISPATCH(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment)
#define memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment) memmgrAllocDetermineAlignment_DISPATCH(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment)
#define memmgrInitFbRegionsHal_HAL(pGpu, pMemoryManager) memmgrInitFbRegionsHal(pGpu, pMemoryManager)
#define memmgrGetMaxContextSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetMaxContextSize__
#define memmgrGetMaxContextSize(pGpu, pMemoryManager) memmgrGetMaxContextSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetMaxContextSize_HAL(pGpu, pMemoryManager) memmgrGetMaxContextSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrHandleSizeOverrides_FNPTR(pMemoryManager) pMemoryManager->__memmgrHandleSizeOverrides__
#define memmgrHandleSizeOverrides(pGpu, pMemoryManager) memmgrHandleSizeOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrHandleSizeOverrides_HAL(pGpu, pMemoryManager) memmgrHandleSizeOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrFinishHandleSizeOverrides_FNPTR(pMemoryManager) pMemoryManager->__memmgrFinishHandleSizeOverrides__
#define memmgrFinishHandleSizeOverrides(pGpu, pMemoryManager) memmgrFinishHandleSizeOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrFinishHandleSizeOverrides_HAL(pGpu, pMemoryManager) memmgrFinishHandleSizeOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetBAR1InfoForDevice_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetBAR1InfoForDevice__
#define memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, bar1Info) memmgrGetBAR1InfoForDevice_DISPATCH(pGpu, pMemoryManager, pDevice, bar1Info)
#define memmgrGetBAR1InfoForDevice_HAL(pGpu, pMemoryManager, pDevice, bar1Info) memmgrGetBAR1InfoForDevice_DISPATCH(pGpu, pMemoryManager, pDevice, bar1Info)
#define memmgrGetFbTaxSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetFbTaxSize__
#define memmgrGetFbTaxSize(pGpu, pMemoryManager) memmgrGetFbTaxSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager) memmgrGetFbTaxSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetVgpuHostRmReservedFb_HAL(pGpu, pMemoryManager, vgpuTypeId) memmgrGetVgpuHostRmReservedFb(pGpu, pMemoryManager, vgpuTypeId)
#define memmgrScrubRegistryOverrides_FNPTR(pMemoryManager) pMemoryManager->__memmgrScrubRegistryOverrides__
#define memmgrScrubRegistryOverrides(pGpu, pMemoryManager) memmgrScrubRegistryOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrScrubRegistryOverrides_HAL(pGpu, pMemoryManager) memmgrScrubRegistryOverrides_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetRsvdSizeForSr_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetRsvdSizeForSr__
#define memmgrGetRsvdSizeForSr(pGpu, pMemoryManager) memmgrGetRsvdSizeForSr_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetRsvdSizeForSr_HAL(pGpu, pMemoryManager) memmgrGetRsvdSizeForSr_DISPATCH(pGpu, pMemoryManager)
#define memmgrVerifyDepthSurfaceAttrs_HAL(pMemoryManager, arg2, arg3) memmgrVerifyDepthSurfaceAttrs(pMemoryManager, arg2, arg3)
#define memmgrAllocMemToSaveVgaWorkspace_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrAllocMemToSaveVgaWorkspace(pGpu, pMemoryManager, arg3, arg4)
#define memmgrComparePhysicalAddresses_FNPTR(pMemoryManager) pMemoryManager->__memmgrComparePhysicalAddresses__
#define memmgrComparePhysicalAddresses(pGpu, pMemoryManager, arg3, arg4, arg5, arg6) memmgrComparePhysicalAddresses_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5, arg6)
#define memmgrComparePhysicalAddresses_HAL(pGpu, pMemoryManager, arg3, arg4, arg5, arg6) memmgrComparePhysicalAddresses_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5, arg6)
#define memmgrGetInvalidOffset_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetInvalidOffset__
#define memmgrGetInvalidOffset(pGpu, pMemoryManager) memmgrGetInvalidOffset_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetInvalidOffset_HAL(pGpu, pMemoryManager) memmgrGetInvalidOffset_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetAddrSpaceSizeMB_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetAddrSpaceSizeMB__
#define memmgrGetAddrSpaceSizeMB(pGpu, pMemoryManager) memmgrGetAddrSpaceSizeMB_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetAddrSpaceSizeMB_HAL(pGpu, pMemoryManager) memmgrGetAddrSpaceSizeMB_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetUsableMemSizeMB_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetUsableMemSizeMB__
#define memmgrGetUsableMemSizeMB(pGpu, pMemoryManager) memmgrGetUsableMemSizeMB_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetUsableMemSizeMB_HAL(pGpu, pMemoryManager) memmgrGetUsableMemSizeMB_DISPATCH(pGpu, pMemoryManager)
#define memmgrVerifyComprAttrs_HAL(pMemoryManager, arg2, arg3, arg4) memmgrVerifyComprAttrs(pMemoryManager, arg2, arg3, arg4)
#define memmgrIsKindCompressible_FNPTR(pMemoryManager) pMemoryManager->__memmgrIsKindCompressible__
#define memmgrIsKindCompressible(pMemoryManager, arg2) memmgrIsKindCompressible_DISPATCH(pMemoryManager, arg2)
#define memmgrIsKindCompressible_HAL(pMemoryManager, arg2) memmgrIsKindCompressible_DISPATCH(pMemoryManager, arg2)
#define memmgrIsKindBlocklinear_HAL(pMemoryManager, arg2) memmgrIsKindBlocklinear(pMemoryManager, arg2)
#define memmgrGetPteKindBl_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetPteKindBl__
#define memmgrGetPteKindBl(pGpu, pMemoryManager) memmgrGetPteKindBl_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindBl_HAL(pGpu, pMemoryManager) memmgrGetPteKindBl_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindPitch_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetPteKindPitch__
#define memmgrGetPteKindPitch(pGpu, pMemoryManager) memmgrGetPteKindPitch_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindPitch_HAL(pGpu, pMemoryManager) memmgrGetPteKindPitch_DISPATCH(pGpu, pMemoryManager)
#define memmgrChooseKindZ_FNPTR(pMemoryManager) pMemoryManager->__memmgrChooseKindZ__
#define memmgrChooseKindZ(pGpu, pMemoryManager, arg3) memmgrChooseKindZ_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindZ_HAL(pGpu, pMemoryManager, arg3) memmgrChooseKindZ_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindCompressZ_FNPTR(pMemoryManager) pMemoryManager->__memmgrChooseKindCompressZ__
#define memmgrChooseKindCompressZ(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressZ_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindCompressZ_HAL(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressZ_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindCompressC_FNPTR(pMemoryManager) pMemoryManager->__memmgrChooseKindCompressC__
#define memmgrChooseKindCompressC(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressC_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindCompressC_HAL(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressC_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChooseKindCompressCForMS2_HAL(pGpu, pMemoryManager, arg3) memmgrChooseKindCompressCForMS2(pGpu, pMemoryManager, arg3)
#define memmgrGetPteKindGenericMemoryCompressible_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetPteKindGenericMemoryCompressible__
#define memmgrGetPteKindGenericMemoryCompressible(pGpu, pMemoryManager) memmgrGetPteKindGenericMemoryCompressible_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetPteKindGenericMemoryCompressible_HAL(pGpu, pMemoryManager) memmgrGetPteKindGenericMemoryCompressible_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetUncompressedKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetUncompressedKind__
#define memmgrGetUncompressedKind(pGpu, pMemoryManager, kind, releaseReacquire) memmgrGetUncompressedKind_DISPATCH(pGpu, pMemoryManager, kind, releaseReacquire)
#define memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager, kind, releaseReacquire) memmgrGetUncompressedKind_DISPATCH(pGpu, pMemoryManager, kind, releaseReacquire)
#define memmgrGetUncompressedKindForMS2_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrGetUncompressedKindForMS2(pGpu, pMemoryManager, arg3, arg4)
#define memmgrGetCompressedKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetCompressedKind__
#define memmgrGetCompressedKind(pMemoryManager, kind, bDisablePlc) memmgrGetCompressedKind_DISPATCH(pMemoryManager, kind, bDisablePlc)
#define memmgrGetCompressedKind_HAL(pMemoryManager, kind, bDisablePlc) memmgrGetCompressedKind_DISPATCH(pMemoryManager, kind, bDisablePlc)
#define memmgrChooseKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrChooseKind__
#define memmgrChooseKind(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrChooseKind_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrChooseKind_HAL(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrChooseKind_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrIsKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrIsKind__
#define memmgrIsKind(pMemoryManager, arg2, arg3) memmgrIsKind_DISPATCH(pMemoryManager, arg2, arg3)
#define memmgrIsKind_HAL(pMemoryManager, arg2, arg3) memmgrIsKind_DISPATCH(pMemoryManager, arg2, arg3)
#define memmgrGetMessageKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetMessageKind__
#define memmgrGetMessageKind(pGpu, pMemoryManager) memmgrGetMessageKind_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetMessageKind_HAL(pGpu, pMemoryManager) memmgrGetMessageKind_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetDefaultPteKindForNoHandle_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetDefaultPteKindForNoHandle__
#define memmgrGetDefaultPteKindForNoHandle(pGpu, pMemoryManager) memmgrGetDefaultPteKindForNoHandle_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetDefaultPteKindForNoHandle_HAL(pGpu, pMemoryManager) memmgrGetDefaultPteKindForNoHandle_DISPATCH(pGpu, pMemoryManager)
#define memmgrIsSurfaceBlockLinear_HAL(pMemoryManager, arg2, arg3) memmgrIsSurfaceBlockLinear(pMemoryManager, arg2, arg3)
#define memmgrGetFlaKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetFlaKind__
#define memmgrGetFlaKind(pGpu, pMemoryManager, arg3) memmgrGetFlaKind_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrGetFlaKind_HAL(pGpu, pMemoryManager, arg3) memmgrGetFlaKind_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrIsMemDescSupportedByFla_FNPTR(arg_this) arg_this->__memmgrIsMemDescSupportedByFla__
#define memmgrIsMemDescSupportedByFla(arg1, arg_this, arg3) memmgrIsMemDescSupportedByFla_DISPATCH(arg1, arg_this, arg3)
#define memmgrIsMemDescSupportedByFla_HAL(arg1, arg_this, arg3) memmgrIsMemDescSupportedByFla_DISPATCH(arg1, arg_this, arg3)
#define memmgrIsValidFlaPageSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrIsValidFlaPageSize__
#define memmgrIsValidFlaPageSize(pGpu, pMemoryManager, pageSize, bIsMulticast) memmgrIsValidFlaPageSize_DISPATCH(pGpu, pMemoryManager, pageSize, bIsMulticast)
#define memmgrIsValidFlaPageSize_HAL(pGpu, pMemoryManager, pageSize, bIsMulticast) memmgrIsValidFlaPageSize_DISPATCH(pGpu, pMemoryManager, pageSize, bIsMulticast)
#define memmgrGetHwPteKindFromSwPteKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetHwPteKindFromSwPteKind__
#define memmgrGetHwPteKindFromSwPteKind(pGpu, pMemoryManager, pteKind) memmgrGetHwPteKindFromSwPteKind_DISPATCH(pGpu, pMemoryManager, pteKind)
#define memmgrGetHwPteKindFromSwPteKind_HAL(pGpu, pMemoryManager, pteKind) memmgrGetHwPteKindFromSwPteKind_DISPATCH(pGpu, pMemoryManager, pteKind)
#define memmgrGetSwPteKindFromHwPteKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetSwPteKindFromHwPteKind__
#define memmgrGetSwPteKindFromHwPteKind(pGpu, pMemoryManager, pteKind) memmgrGetSwPteKindFromHwPteKind_DISPATCH(pGpu, pMemoryManager, pteKind)
#define memmgrGetSwPteKindFromHwPteKind_HAL(pGpu, pMemoryManager, pteKind) memmgrGetSwPteKindFromHwPteKind_DISPATCH(pGpu, pMemoryManager, pteKind)
#define memmgrGetPteKindForScrubber_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetPteKindForScrubber__
#define memmgrGetPteKindForScrubber(pMemoryManager, arg2) memmgrGetPteKindForScrubber_DISPATCH(pMemoryManager, arg2)
#define memmgrGetPteKindForScrubber_HAL(pMemoryManager, arg2) memmgrGetPteKindForScrubber_DISPATCH(pMemoryManager, arg2)
#define memmgrGetCtagOffsetFromParams_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetCtagOffsetFromParams__
#define memmgrGetCtagOffsetFromParams(pGpu, pMemoryManager, arg3) memmgrGetCtagOffsetFromParams_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrGetCtagOffsetFromParams_HAL(pGpu, pMemoryManager, arg3) memmgrGetCtagOffsetFromParams_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrSetCtagOffsetInParams_FNPTR(pMemoryManager) pMemoryManager->__memmgrSetCtagOffsetInParams__
#define memmgrSetCtagOffsetInParams(pGpu, pMemoryManager, arg3, arg4) memmgrSetCtagOffsetInParams_DISPATCH(pGpu, pMemoryManager, arg3, arg4)
#define memmgrSetCtagOffsetInParams_HAL(pGpu, pMemoryManager, arg3, arg4) memmgrSetCtagOffsetInParams_DISPATCH(pGpu, pMemoryManager, arg3, arg4)
#define memmgrDetermineComptag_FNPTR(pMemoryManager) pMemoryManager->__memmgrDetermineComptag__
#define memmgrDetermineComptag(pGpu, pMemoryManager, arg3) memmgrDetermineComptag_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrDetermineComptag_HAL(pGpu, pMemoryManager, arg3) memmgrDetermineComptag_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrChannelPushSemaphoreMethodsBlock_HAL(pMemoryManager, arg2, arg3, arg4, arg5) memmgrChannelPushSemaphoreMethodsBlock(pMemoryManager, arg2, arg3, arg4, arg5)
#define memmgrChannelPushAddressMethodsBlock_HAL(pMemoryManager, arg2, arg3, arg4, arg5) memmgrChannelPushAddressMethodsBlock(pMemoryManager, arg2, arg3, arg4, arg5)
#define memmgrScrubMapDoorbellRegion_FNPTR(pMemoryManager) pMemoryManager->__memmgrScrubMapDoorbellRegion__
#define memmgrScrubMapDoorbellRegion(pGpu, pMemoryManager, arg3) memmgrScrubMapDoorbellRegion_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrScrubMapDoorbellRegion_HAL(pGpu, pMemoryManager, arg3) memmgrScrubMapDoorbellRegion_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrSetAllocParameters_FNPTR(pMemoryManager) pMemoryManager->__memmgrSetAllocParameters__
#define memmgrSetAllocParameters(pGpu, pMemoryManager, pFbAllocInfo) memmgrSetAllocParameters_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo)
#define memmgrSetAllocParameters_HAL(pGpu, pMemoryManager, pFbAllocInfo) memmgrSetAllocParameters_DISPATCH(pGpu, pMemoryManager, pFbAllocInfo)
#define memmgrCalcReservedFbSpaceForUVM_FNPTR(pMemoryManager) pMemoryManager->__memmgrCalcReservedFbSpaceForUVM__
#define memmgrCalcReservedFbSpaceForUVM(pGpu, pMemoryManager, arg3) memmgrCalcReservedFbSpaceForUVM_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrCalcReservedFbSpaceForUVM_HAL(pGpu, pMemoryManager, arg3) memmgrCalcReservedFbSpaceForUVM_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrCalcReservedFbSpaceHal_FNPTR(pMemoryManager) pMemoryManager->__memmgrCalcReservedFbSpaceHal__
#define memmgrCalcReservedFbSpaceHal(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrCalcReservedFbSpaceHal_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrCalcReservedFbSpaceHal_HAL(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrCalcReservedFbSpaceHal_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrGetGrHeapReservationSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetGrHeapReservationSize__
#define memmgrGetGrHeapReservationSize(pGpu, pMemoryManager) memmgrGetGrHeapReservationSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetGrHeapReservationSize_HAL(pGpu, pMemoryManager) memmgrGetGrHeapReservationSize_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetRunlistEntriesReservedFbSpace_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetRunlistEntriesReservedFbSpace__
#define memmgrGetRunlistEntriesReservedFbSpace(pGpu, pMemoryManager) memmgrGetRunlistEntriesReservedFbSpace_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetRunlistEntriesReservedFbSpace_HAL(pGpu, pMemoryManager) memmgrGetRunlistEntriesReservedFbSpace_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetUserdReservedFbSpace_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetUserdReservedFbSpace__
#define memmgrGetUserdReservedFbSpace(pGpu, pMemoryManager) memmgrGetUserdReservedFbSpace_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetUserdReservedFbSpace_HAL(pGpu, pMemoryManager) memmgrGetUserdReservedFbSpace_DISPATCH(pGpu, pMemoryManager)
#define memmgrCheckReservedMemorySize_FNPTR(pMemoryManager) pMemoryManager->__memmgrCheckReservedMemorySize__
#define memmgrCheckReservedMemorySize(pGpu, pMemoryManager) memmgrCheckReservedMemorySize_DISPATCH(pGpu, pMemoryManager)
#define memmgrCheckReservedMemorySize_HAL(pGpu, pMemoryManager) memmgrCheckReservedMemorySize_DISPATCH(pGpu, pMemoryManager)
#define memmgrInitReservedMemory_FNPTR(pMemoryManager) pMemoryManager->__memmgrInitReservedMemory__
#define memmgrInitReservedMemory(pGpu, pMemoryManager, arg3) memmgrInitReservedMemory_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrInitReservedMemory_HAL(pGpu, pMemoryManager, arg3) memmgrInitReservedMemory_DISPATCH(pGpu, pMemoryManager, arg3)
#define memmgrPreInitReservedMemory_FNPTR(pMemoryManager) pMemoryManager->__memmgrPreInitReservedMemory__
#define memmgrPreInitReservedMemory(pGpu, pMemoryManager) memmgrPreInitReservedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrPreInitReservedMemory_HAL(pGpu, pMemoryManager) memmgrPreInitReservedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrReadMmuLock_FNPTR(pMemoryManager) pMemoryManager->__memmgrReadMmuLock__
#define memmgrReadMmuLock(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi) memmgrReadMmuLock_DISPATCH(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi)
#define memmgrReadMmuLock_HAL(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi) memmgrReadMmuLock_DISPATCH(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi)
#define memmgrBlockMemLockedMemory_FNPTR(pMemoryManager) pMemoryManager->__memmgrBlockMemLockedMemory__
#define memmgrBlockMemLockedMemory(pGpu, pMemoryManager) memmgrBlockMemLockedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrBlockMemLockedMemory_HAL(pGpu, pMemoryManager) memmgrBlockMemLockedMemory_DISPATCH(pGpu, pMemoryManager)
#define memmgrInsertUnprotectedRegionAtBottomOfFb_FNPTR(pMemoryManager) pMemoryManager->__memmgrInsertUnprotectedRegionAtBottomOfFb__
#define memmgrInsertUnprotectedRegionAtBottomOfFb(pGpu, pMemoryManager, pSize) memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(pGpu, pMemoryManager, pSize)
#define memmgrInsertUnprotectedRegionAtBottomOfFb_HAL(pGpu, pMemoryManager, pSize) memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(pGpu, pMemoryManager, pSize)
#define memmgrInitBaseFbRegions_FNPTR(pMemoryManager) pMemoryManager->__memmgrInitBaseFbRegions__
#define memmgrInitBaseFbRegions(pGpu, pMemoryManager) memmgrInitBaseFbRegions_DISPATCH(pGpu, pMemoryManager)
#define memmgrInitBaseFbRegions_HAL(pGpu, pMemoryManager) memmgrInitBaseFbRegions_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetDisablePlcKind_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetDisablePlcKind__
#define memmgrGetDisablePlcKind(pMemoryManager, pteKind) memmgrGetDisablePlcKind_DISPATCH(pMemoryManager, pteKind)
#define memmgrGetDisablePlcKind_HAL(pMemoryManager, pteKind) memmgrGetDisablePlcKind_DISPATCH(pMemoryManager, pteKind)
#define memmgrEnableDynamicPageOfflining_FNPTR(pMemoryManager) pMemoryManager->__memmgrEnableDynamicPageOfflining__
#define memmgrEnableDynamicPageOfflining(pGpu, pMemoryManager) memmgrEnableDynamicPageOfflining_DISPATCH(pGpu, pMemoryManager)
#define memmgrEnableDynamicPageOfflining_HAL(pGpu, pMemoryManager) memmgrEnableDynamicPageOfflining_DISPATCH(pGpu, pMemoryManager)
#define memmgrSetMemDescPageSize_FNPTR(pMemoryManager) pMemoryManager->__memmgrSetMemDescPageSize__
#define memmgrSetMemDescPageSize(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrSetMemDescPageSize_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, arg3, arg4, arg5) memmgrSetMemDescPageSize_DISPATCH(pGpu, pMemoryManager, arg3, arg4, arg5)
#define memmgrSetPartitionableMem_FNPTR(pMemoryManager) pMemoryManager->__memmgrSetPartitionableMem__
#define memmgrSetPartitionableMem(pGpu, pMemoryManager) memmgrSetPartitionableMem_DISPATCH(pGpu, pMemoryManager)
#define memmgrSetPartitionableMem_HAL(pGpu, pMemoryManager) memmgrSetPartitionableMem_DISPATCH(pGpu, pMemoryManager)
#define memmgrAllocMIGGPUInstanceMemory_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocMIGGPUInstanceMemory__
#define memmgrAllocMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap) memmgrAllocMIGGPUInstanceMemory_DISPATCH(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap)
#define memmgrAllocMIGGPUInstanceMemory_HAL(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap) memmgrAllocMIGGPUInstanceMemory_DISPATCH(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap)
#define memmgrGetBlackListPagesForHeap_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetBlackListPagesForHeap__
#define memmgrGetBlackListPagesForHeap(pGpu, pMemoryManager, pHeap) memmgrGetBlackListPagesForHeap_DISPATCH(pGpu, pMemoryManager, pHeap)
#define memmgrGetBlackListPagesForHeap_HAL(pGpu, pMemoryManager, pHeap) memmgrGetBlackListPagesForHeap_DISPATCH(pGpu, pMemoryManager, pHeap)
#define memmgrGetBlackListPages_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetBlackListPages__
#define memmgrGetBlackListPages(pGpu, pMemoryManager, pBlAddrs, pCount) memmgrGetBlackListPages_DISPATCH(pGpu, pMemoryManager, pBlAddrs, pCount)
#define memmgrGetBlackListPages_HAL(pGpu, pMemoryManager, pBlAddrs, pCount) memmgrGetBlackListPages_DISPATCH(pGpu, pMemoryManager, pBlAddrs, pCount)
#define memmgrDiscoverMIGPartitionableMemoryRange_FNPTR(pMemoryManager) pMemoryManager->__memmgrDiscoverMIGPartitionableMemoryRange__
#define memmgrDiscoverMIGPartitionableMemoryRange(pGpu, pMemoryManager, pMemoryRange) memmgrDiscoverMIGPartitionableMemoryRange_DISPATCH(pGpu, pMemoryManager, pMemoryRange)
#define memmgrDiscoverMIGPartitionableMemoryRange_HAL(pGpu, pMemoryManager, pMemoryRange) memmgrDiscoverMIGPartitionableMemoryRange_DISPATCH(pGpu, pMemoryManager, pMemoryRange)
#define memmgrGetFBEndReserveSizeEstimate_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetFBEndReserveSizeEstimate__
#define memmgrGetFBEndReserveSizeEstimate(pGpu, pMemoryManager) memmgrGetFBEndReserveSizeEstimate_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetFBEndReserveSizeEstimate_HAL(pGpu, pMemoryManager) memmgrGetFBEndReserveSizeEstimate_DISPATCH(pGpu, pMemoryManager)
#define memmgrValidateFBEndReservation_HAL(pGpu, pMemoryManager) memmgrValidateFBEndReservation(pGpu, pMemoryManager)
#define memmgrReserveMemoryForFakeWPR_HAL(pGpu, pMemoryManager) memmgrReserveMemoryForFakeWPR(pGpu, pMemoryManager)
#define memmgrReserveMemoryForPmu_HAL(pGpu, pMemoryManager) memmgrReserveMemoryForPmu(pGpu, pMemoryManager)
#define memmgrFreeFbsrMemory_HAL(pGpu, pMemoryManager) memmgrFreeFbsrMemory(pGpu, pMemoryManager)
#define memmgrReserveVgaWorkspaceMemDescForFbsr_HAL(pGpu, pMemoryManager) memmgrReserveVgaWorkspaceMemDescForFbsr(pGpu, pMemoryManager)
#define memmgrCalculateHeapOffsetWithGSP_HAL(pGpu, pMemoryManager, offset) memmgrCalculateHeapOffsetWithGSP(pGpu, pMemoryManager, offset)
#define memmgrInitZeroFbRegionsHal_FNPTR(pMemoryManager) pMemoryManager->__memmgrInitZeroFbRegionsHal__
#define memmgrInitZeroFbRegionsHal(pGpu, pMemoryManager) memmgrInitZeroFbRegionsHal_DISPATCH(pGpu, pMemoryManager)
#define memmgrInitZeroFbRegionsHal_HAL(pGpu, pMemoryManager) memmgrInitZeroFbRegionsHal_DISPATCH(pGpu, pMemoryManager)
#define memmgrAllocScanoutCarveoutRegionResources_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocScanoutCarveoutRegionResources__
#define memmgrAllocScanoutCarveoutRegionResources(pMemoryManager, pAllocData, owner, pHeapFlag, pMemDesc) memmgrAllocScanoutCarveoutRegionResources_DISPATCH(pMemoryManager, pAllocData, owner, pHeapFlag, pMemDesc)
#define memmgrAllocScanoutCarveoutRegionResources_HAL(pMemoryManager, pAllocData, owner, pHeapFlag, pMemDesc) memmgrAllocScanoutCarveoutRegionResources_DISPATCH(pMemoryManager, pAllocData, owner, pHeapFlag, pMemDesc)
#define memmgrAllocFromScanoutCarveoutRegion_FNPTR(pMemoryManager) pMemoryManager->__memmgrAllocFromScanoutCarveoutRegion__
#define memmgrAllocFromScanoutCarveoutRegion(pGpu, pMemoryManager, owner, pVidHeapAlloc, pHeapFlag, ppMemDesc) memmgrAllocFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, owner, pVidHeapAlloc, pHeapFlag, ppMemDesc)
#define memmgrAllocFromScanoutCarveoutRegion_HAL(pGpu, pMemoryManager, owner, pVidHeapAlloc, pHeapFlag, ppMemDesc) memmgrAllocFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, owner, pVidHeapAlloc, pHeapFlag, ppMemDesc)
#define memmgrFreeScanoutCarveoutRegionResources_FNPTR(pMemoryManager) pMemoryManager->__memmgrFreeScanoutCarveoutRegionResources__
#define memmgrFreeScanoutCarveoutRegionResources(pMemoryManager, base) memmgrFreeScanoutCarveoutRegionResources_DISPATCH(pMemoryManager, base)
#define memmgrFreeScanoutCarveoutRegionResources_HAL(pMemoryManager, base) memmgrFreeScanoutCarveoutRegionResources_DISPATCH(pMemoryManager, base)
#define memmgrFreeFromScanoutCarveoutRegion_FNPTR(pMemoryManager) pMemoryManager->__memmgrFreeFromScanoutCarveoutRegion__
#define memmgrFreeFromScanoutCarveoutRegion(pGpu, pMemoryManager, pMemDesc) memmgrFreeFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, pMemDesc)
#define memmgrFreeFromScanoutCarveoutRegion_HAL(pGpu, pMemoryManager, pMemDesc) memmgrFreeFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, pMemDesc)
#define memmgrCreateScanoutCarveoutHeap_FNPTR(pMemoryManager) pMemoryManager->__memmgrCreateScanoutCarveoutHeap__
#define memmgrCreateScanoutCarveoutHeap(pGpu, pMemoryManager) memmgrCreateScanoutCarveoutHeap_DISPATCH(pGpu, pMemoryManager)
#define memmgrCreateScanoutCarveoutHeap_HAL(pGpu, pMemoryManager) memmgrCreateScanoutCarveoutHeap_DISPATCH(pGpu, pMemoryManager)
#define memmgrDestroyScanoutCarveoutHeap_FNPTR(pMemoryManager) pMemoryManager->__memmgrDestroyScanoutCarveoutHeap__
#define memmgrDestroyScanoutCarveoutHeap(pGpu, pMemoryManager) memmgrDestroyScanoutCarveoutHeap_DISPATCH(pGpu, pMemoryManager)
#define memmgrDestroyScanoutCarveoutHeap_HAL(pGpu, pMemoryManager) memmgrDestroyScanoutCarveoutHeap_DISPATCH(pGpu, pMemoryManager)
#define memmgrDuplicateFromScanoutCarveoutRegion_FNPTR(pMemoryManager) pMemoryManager->__memmgrDuplicateFromScanoutCarveoutRegion__
#define memmgrDuplicateFromScanoutCarveoutRegion(pGpu, pMemoryManager, pMemDesc) memmgrDuplicateFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, pMemDesc)
#define memmgrDuplicateFromScanoutCarveoutRegion_HAL(pGpu, pMemoryManager, pMemDesc) memmgrDuplicateFromScanoutCarveoutRegion_DISPATCH(pGpu, pMemoryManager, pMemDesc)
#define memmgrSaveDisplayFrmReservedRegion_HAL(pGpu, pMemoryManager) memmgrSaveDisplayFrmReservedRegion(pGpu, pMemoryManager)
#define memmgrRestoreDisplayFrmReservedRegion_HAL(pGpu, pMemoryManager) memmgrRestoreDisplayFrmReservedRegion(pGpu, pMemoryManager)
#define memmgrGetCarveoutRegionInfo_HAL(pGpu, pMemoryManager, pParams) memmgrGetCarveoutRegionInfo(pGpu, pMemoryManager, pParams)
#define memmgrIsMemoryIoCoherent_FNPTR(pMemoryManager) pMemoryManager->__memmgrIsMemoryIoCoherent__
#define memmgrIsMemoryIoCoherent(pGpu, pMemoryManager, pAllocData) memmgrIsMemoryIoCoherent_DISPATCH(pGpu, pMemoryManager, pAllocData)
#define memmgrIsMemoryIoCoherent_HAL(pGpu, pMemoryManager, pAllocData) memmgrIsMemoryIoCoherent_DISPATCH(pGpu, pMemoryManager, pAllocData)
#define memmgrGetLocalizedOffset_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetLocalizedOffset__
#define memmgrGetLocalizedOffset(pGpu, pMemoryManager) memmgrGetLocalizedOffset_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetLocalizedOffset_HAL(pGpu, pMemoryManager) memmgrGetLocalizedOffset_DISPATCH(pGpu, pMemoryManager)
#define memmgrIsFlaSysmemSupported_FNPTR(pMemoryManager) pMemoryManager->__memmgrIsFlaSysmemSupported__
#define memmgrIsFlaSysmemSupported(pGpu, pMemoryManager) memmgrIsFlaSysmemSupported_DISPATCH(pGpu, pMemoryManager)
#define memmgrIsFlaSysmemSupported_HAL(pGpu, pMemoryManager) memmgrIsFlaSysmemSupported_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetLocalizedMemorySupported_FNPTR(pMemoryManager) pMemoryManager->__memmgrGetLocalizedMemorySupported__
#define memmgrGetLocalizedMemorySupported(pGpu, pMemoryManager) memmgrGetLocalizedMemorySupported_DISPATCH(pGpu, pMemoryManager)
#define memmgrGetLocalizedMemorySupported_HAL(pGpu, pMemoryManager) memmgrGetLocalizedMemorySupported_DISPATCH(pGpu, pMemoryManager)
#define memmgrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define memmgrInitMissing(pGpu, pEngstate) memmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define memmgrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define memmgrStatePreInitUnlocked(pGpu, pEngstate) memmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define memmgrStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define memmgrStateInitUnlocked(pGpu, pEngstate) memmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define memmgrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define memmgrStatePreLoad(pGpu, pEngstate, arg3) memmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define memmgrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define memmgrStatePostUnload(pGpu, pEngstate, arg3) memmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define memmgrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define memmgrIsPresent(pGpu, pEngstate) memmgrIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS memmgrConstructEngine_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, ENGDESCRIPTOR arg3) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrConstructEngine__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePreInitLocked__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrStateInitLocked_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateInitLocked__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrStateLoad_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateLoad__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrStatePostLoad_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePostLoad__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrStatePreUnload_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStatePreUnload__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrStateUnload_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    return pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateUnload__(pGpu, pMemoryManager, arg3);
}

static inline void memmgrStateDestroy_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__nvoc_metadata_ptr->vtable.__memmgrStateDestroy__(pGpu, pMemoryManager);
}

static inline NvU64 memmgrDeterminePageSize_DISPATCH(struct MemoryManager *pMemoryManager, NvHandle hClient, NvU64 memSize, NvU32 memFormat, NvU32 pageFormatFlags, NvU32 *pRetAttr, NvU32 *pRetAttr2) {
    return pMemoryManager->__memmgrDeterminePageSize__(pMemoryManager, hClient, memSize, memFormat, pageFormatFlags, pRetAttr, pRetAttr2);
}

static inline NV_STATUS memmgrFreeHwResources_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3) {
    return pMemoryManager->__memmgrFreeHwResources__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrCreateHeap_DISPATCH(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrCreateHeap__(pMemoryManager);
}

static inline NV_STATUS memmgrInitFbRegions_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrInitFbRegions__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrAllocateConsoleRegion_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrAllocateConsoleRegion__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrScrubHandlePostSchedulingEnable_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrScrubHandlePostSchedulingEnable__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrScrubHandlePreSchedulingDisable_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrScrubHandlePreSchedulingDisable__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrMemUtilsChannelInitialize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return pMemoryManager->__memmgrMemUtilsChannelInitialize__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrMemUtilsCopyEngineInitialize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return pMemoryManager->__memmgrMemUtilsCopyEngineInitialize__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrMemUtilsSec2CtxInit_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return pMemoryManager->__memmgrMemUtilsSec2CtxInit__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrMemUtilsGetCopyEngineClass_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pClass) {
    return pMemoryManager->__memmgrMemUtilsGetCopyEngineClass__(pGpu, pMemoryManager, pClass);
}

static inline NV_STATUS memmgrMemUtilsCreateMemoryAlias_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return pMemoryManager->__memmgrMemUtilsCreateMemoryAlias__(pGpu, pMemoryManager, arg3);
}

static inline NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvBool arg4, RmPhysAddr arg5, NvU32 arg6, NV_ADDRESS_SPACE arg7) {
    return pMemoryManager->__memmgrMemUtilsCheckMemoryFastScrubEnable__(pGpu, pMemoryManager, arg3, arg4, arg5, arg6, arg7);
}

static inline NV_STATUS memmgrAllocHal_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    return pMemoryManager->__memmgrAllocHal__(pGpu, pMemoryManager, pFbAllocInfo);
}

static inline NV_STATUS memmgrFreeHal_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo, PRMTIMEOUT pTimeout) {
    return pMemoryManager->__memmgrFreeHal__(pGpu, pMemoryManager, pFbAllocInfo, pTimeout);
}

static inline NV_STATUS memmgrGetBankPlacementData_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pBankPlacementLowData) {
    return pMemoryManager->__memmgrGetBankPlacementData__(pGpu, pMemoryManager, pBankPlacementLowData);
}

static inline void memmgrDirtyForPmTest_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool partialDirty) {
    pMemoryManager->__memmgrDirtyForPmTest__(pGpu, pMemoryManager, partialDirty);
}

static inline NvU64 memmgrGetReservedHeapSizeMb_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetReservedHeapSizeMb__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrAllocDetermineAlignment_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment) {
    return pMemoryManager->__memmgrAllocDetermineAlignment__(pGpu, pMemoryManager, pMemSize, pAlign, alignPad, allocFlags, retAttr, retAttr2, hwAlignment);
}

static inline NvU64 memmgrGetMaxContextSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetMaxContextSize__(pGpu, pMemoryManager);
}

static inline void memmgrHandleSizeOverrides_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrHandleSizeOverrides__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrFinishHandleSizeOverrides_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrFinishHandleSizeOverrides__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrGetBAR1InfoForDevice_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, PGETBAR1INFO bar1Info) {
    return pMemoryManager->__memmgrGetBAR1InfoForDevice__(pGpu, pMemoryManager, pDevice, bar1Info);
}

static inline NvU64 memmgrGetFbTaxSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetFbTaxSize__(pGpu, pMemoryManager);
}

static inline void memmgrScrubRegistryOverrides_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrScrubRegistryOverrides__(pGpu, pMemoryManager);
}

static inline NvU64 memmgrGetRsvdSizeForSr_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetRsvdSizeForSr__(pGpu, pMemoryManager);
}

static inline NvBool memmgrComparePhysicalAddresses_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvU64 arg4, NvU32 arg5, NvU64 arg6) {
    return pMemoryManager->__memmgrComparePhysicalAddresses__(pGpu, pMemoryManager, arg3, arg4, arg5, arg6);
}

static inline RmPhysAddr memmgrGetInvalidOffset_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetInvalidOffset__(pGpu, pMemoryManager);
}

static inline NvU64 memmgrGetAddrSpaceSizeMB_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetAddrSpaceSizeMB__(pGpu, pMemoryManager);
}

static inline NvU64 memmgrGetUsableMemSizeMB_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetUsableMemSizeMB__(pGpu, pMemoryManager);
}

static inline NvBool memmgrIsKindCompressible_DISPATCH(struct MemoryManager *pMemoryManager, NvU32 arg2) {
    return pMemoryManager->__memmgrIsKindCompressible__(pMemoryManager, arg2);
}

static inline NvU32 memmgrGetPteKindBl_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetPteKindBl__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrGetPteKindPitch_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetPteKindPitch__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrChooseKindZ_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    return pMemoryManager->__memmgrChooseKindZ__(pGpu, pMemoryManager, arg3);
}

static inline NvU32 memmgrChooseKindCompressZ_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    return pMemoryManager->__memmgrChooseKindCompressZ__(pGpu, pMemoryManager, arg3);
}

static inline NvU32 memmgrChooseKindCompressC_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    return pMemoryManager->__memmgrChooseKindCompressC__(pGpu, pMemoryManager, arg3);
}

static inline NvU32 memmgrGetPteKindGenericMemoryCompressible_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetPteKindGenericMemoryCompressible__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrGetUncompressedKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire) {
    return pMemoryManager->__memmgrGetUncompressedKind__(pGpu, pMemoryManager, kind, releaseReacquire);
}

static inline NvU32 memmgrGetCompressedKind_DISPATCH(struct MemoryManager *pMemoryManager, NvU32 kind, NvBool bDisablePlc) {
    return pMemoryManager->__memmgrGetCompressedKind__(pMemoryManager, kind, bDisablePlc);
}

static inline NV_STATUS memmgrChooseKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3, NvU32 arg4, NvU32 *arg5) {
    return pMemoryManager->__memmgrChooseKind__(pGpu, pMemoryManager, arg3, arg4, arg5);
}

static inline NvBool memmgrIsKind_DISPATCH(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg2, NvU32 arg3) {
    return pMemoryManager->__memmgrIsKind__(pMemoryManager, arg2, arg3);
}

static inline NvU32 memmgrGetMessageKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetMessageKind__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrGetDefaultPteKindForNoHandle_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetDefaultPteKindForNoHandle__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrGetFlaKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg3) {
    return pMemoryManager->__memmgrGetFlaKind__(pGpu, pMemoryManager, arg3);
}

static inline NvBool memmgrIsMemDescSupportedByFla_DISPATCH(OBJGPU *arg1, struct MemoryManager *arg_this, MEMORY_DESCRIPTOR *arg3) {
    return arg_this->__memmgrIsMemDescSupportedByFla__(arg1, arg_this, arg3);
}

static inline NvBool memmgrIsValidFlaPageSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 pageSize, NvBool bIsMulticast) {
    return pMemoryManager->__memmgrIsValidFlaPageSize__(pGpu, pMemoryManager, pageSize, bIsMulticast);
}

static inline NvU32 memmgrGetHwPteKindFromSwPteKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    return pMemoryManager->__memmgrGetHwPteKindFromSwPteKind__(pGpu, pMemoryManager, pteKind);
}

static inline NvU32 memmgrGetSwPteKindFromHwPteKind_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    return pMemoryManager->__memmgrGetSwPteKindFromHwPteKind__(pGpu, pMemoryManager, pteKind);
}

static inline void memmgrGetPteKindForScrubber_DISPATCH(struct MemoryManager *pMemoryManager, NvU32 *arg2) {
    pMemoryManager->__memmgrGetPteKindForScrubber__(pMemoryManager, arg2);
}

static inline NvU32 memmgrGetCtagOffsetFromParams_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3) {
    return pMemoryManager->__memmgrGetCtagOffsetFromParams__(pGpu, pMemoryManager, arg3);
}

static inline void memmgrSetCtagOffsetInParams_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3, NvU32 arg4) {
    pMemoryManager->__memmgrSetCtagOffsetInParams__(pGpu, pMemoryManager, arg3, arg4);
}

static inline NvU32 memmgrDetermineComptag_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3) {
    return pMemoryManager->__memmgrDetermineComptag__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrScrubMapDoorbellRegion_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return pMemoryManager->__memmgrScrubMapDoorbellRegion__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrSetAllocParameters_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    return pMemoryManager->__memmgrSetAllocParameters__(pGpu, pMemoryManager, pFbAllocInfo);
}

static inline void memmgrCalcReservedFbSpaceForUVM_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3) {
    pMemoryManager->__memmgrCalcReservedFbSpaceForUVM__(pGpu, pMemoryManager, arg3);
}

static inline void memmgrCalcReservedFbSpaceHal_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3, NvU64 *arg4, NvU64 *arg5) {
    pMemoryManager->__memmgrCalcReservedFbSpaceHal__(pGpu, pMemoryManager, arg3, arg4, arg5);
}

static inline NvU32 memmgrGetGrHeapReservationSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetGrHeapReservationSize__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrGetRunlistEntriesReservedFbSpace_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetRunlistEntriesReservedFbSpace__(pGpu, pMemoryManager);
}

static inline NvU32 memmgrGetUserdReservedFbSpace_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetUserdReservedFbSpace__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrCheckReservedMemorySize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrCheckReservedMemorySize__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrInitReservedMemory_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3) {
    return pMemoryManager->__memmgrInitReservedMemory__(pGpu, pMemoryManager, arg3);
}

static inline NV_STATUS memmgrPreInitReservedMemory_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrPreInitReservedMemory__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrReadMmuLock_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi) {
    return pMemoryManager->__memmgrReadMmuLock__(pGpu, pMemoryManager, pbIsValid, pMmuLockLo, pMmuLockHi);
}

static inline NV_STATUS memmgrBlockMemLockedMemory_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrBlockMemLockedMemory__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize) {
    return pMemoryManager->__memmgrInsertUnprotectedRegionAtBottomOfFb__(pGpu, pMemoryManager, pSize);
}

static inline NV_STATUS memmgrInitBaseFbRegions_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrInitBaseFbRegions__(pGpu, pMemoryManager);
}

static inline void memmgrGetDisablePlcKind_DISPATCH(struct MemoryManager *pMemoryManager, NvU32 *pteKind) {
    pMemoryManager->__memmgrGetDisablePlcKind__(pMemoryManager, pteKind);
}

static inline void memmgrEnableDynamicPageOfflining_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    pMemoryManager->__memmgrEnableDynamicPageOfflining__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrSetMemDescPageSize_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg3, ADDRESS_TRANSLATION arg4, RM_ATTR_PAGE_SIZE arg5) {
    return pMemoryManager->__memmgrSetMemDescPageSize__(pGpu, pMemoryManager, arg3, arg4, arg5);
}

static inline NV_STATUS memmgrSetPartitionableMem_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrSetPartitionableMem__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrAllocMIGGPUInstanceMemory_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle *phMemory, struct NV_RANGE *pAddrRange, struct Heap **ppMemoryPartitionHeap) {
    return pMemoryManager->__memmgrAllocMIGGPUInstanceMemory__(pGpu, pMemoryManager, swizzId, phMemory, pAddrRange, ppMemoryPartitionHeap);
}

static inline NV_STATUS memmgrGetBlackListPagesForHeap_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap) {
    return pMemoryManager->__memmgrGetBlackListPagesForHeap__(pGpu, pMemoryManager, pHeap);
}

static inline NV_STATUS memmgrGetBlackListPages_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount) {
    return pMemoryManager->__memmgrGetBlackListPages__(pGpu, pMemoryManager, pBlAddrs, pCount);
}

static inline NV_STATUS memmgrDiscoverMIGPartitionableMemoryRange_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct NV_RANGE *pMemoryRange) {
    return pMemoryManager->__memmgrDiscoverMIGPartitionableMemoryRange__(pGpu, pMemoryManager, pMemoryRange);
}

static inline NvU32 memmgrGetFBEndReserveSizeEstimate_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetFBEndReserveSizeEstimate__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrInitZeroFbRegionsHal_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrInitZeroFbRegionsHal__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrAllocScanoutCarveoutRegionResources_DISPATCH(struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData, NvU32 owner, NvU32 *pHeapFlag, MEMORY_DESCRIPTOR *pMemDesc) {
    return pMemoryManager->__memmgrAllocScanoutCarveoutRegionResources__(pMemoryManager, pAllocData, owner, pHeapFlag, pMemDesc);
}

static inline NV_STATUS memmgrAllocFromScanoutCarveoutRegion_DISPATCH(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NvU32 owner, NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc, NvU32 *pHeapFlag, PMEMORY_DESCRIPTOR *ppMemDesc) {
    return pMemoryManager->__memmgrAllocFromScanoutCarveoutRegion__(pGpu, pMemoryManager, owner, pVidHeapAlloc, pHeapFlag, ppMemDesc);
}

static inline void memmgrFreeScanoutCarveoutRegionResources_DISPATCH(struct MemoryManager *pMemoryManager, NvU64 base) {
    pMemoryManager->__memmgrFreeScanoutCarveoutRegionResources__(pMemoryManager, base);
}

static inline void memmgrFreeFromScanoutCarveoutRegion_DISPATCH(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc) {
    pMemoryManager->__memmgrFreeFromScanoutCarveoutRegion__(pGpu, pMemoryManager, pMemDesc);
}

static inline NV_STATUS memmgrCreateScanoutCarveoutHeap_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrCreateScanoutCarveoutHeap__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrDestroyScanoutCarveoutHeap_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrDestroyScanoutCarveoutHeap__(pGpu, pMemoryManager);
}

static inline NV_STATUS memmgrDuplicateFromScanoutCarveoutRegion_DISPATCH(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc) {
    return pMemoryManager->__memmgrDuplicateFromScanoutCarveoutRegion__(pGpu, pMemoryManager, pMemDesc);
}

static inline NvBool memmgrIsMemoryIoCoherent_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData) {
    return pMemoryManager->__memmgrIsMemoryIoCoherent__(pGpu, pMemoryManager, pAllocData);
}

static inline NvU8 memmgrGetLocalizedOffset_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetLocalizedOffset__(pGpu, pMemoryManager);
}

static inline NvBool memmgrIsFlaSysmemSupported_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrIsFlaSysmemSupported__(pGpu, pMemoryManager);
}

static inline NvBool memmgrGetLocalizedMemorySupported_DISPATCH(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->__memmgrGetLocalizedMemorySupported__(pGpu, pMemoryManager);
}

static inline void memmgrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__memmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS memmgrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__memmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS memmgrStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__memmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS memmgrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__memmgrStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS memmgrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__memmgrStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool memmgrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__memmgrIsPresent__(pGpu, pEngstate);
}

NV_STATUS memmgrSavePowerMgmtState_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


NV_STATUS memmgrRestorePowerMgmtState_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


static inline NV_STATUS memmgrReserveVbiosVgaRegions_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


static inline NV_STATUS memmgrReserveConsoleRegion_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


NV_STATUS memmgrGetKindComprForGpu_KERNEL(struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR *arg2, OBJGPU *pGpu, NvU64 offset, NvU32 *kind, COMPR_INFO *pComprInfo);


static inline NvBool memmgrComprMappingSupported_88bc07(struct MemoryManager *pMemoryManager, NV_ADDRESS_SPACE arg2) {
    return NV_TRUE;
}


static inline NV_STATUS memmgrScrubInit_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


static inline void memmgrGetScrubState_f2d351(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3, NvU64 *arg4, NvBool *arg5) {
    NV_ASSERT_PRECOMP(0);
}


static inline void memmgrScrubInternalRegions_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}


static inline NvBool memmgrEccScrubInProgress_3dd2c9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_FALSE;
}

NvBool memmgrEccScrubInProgress_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvBool memmgrEccScrubInProgress_88bc07(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_TRUE;
}


static inline void memmgrAsyncScrubRegion_f2d351(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_PRECOMP(0);
}


static inline void memmgrScrubDestroy_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}


static inline void memmgrScrubMemory_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3, NvU64 arg4) {
    return;
}

void memmgrScrubMemory_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3, NvU64 arg4);


static inline NV_STATUS memmgrMemUtilsMemSetBlocking_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3, RmPhysAddr arg4, NvU64 arg5) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrMemUtilsMemSet_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3, RmPhysAddr arg4, NvU64 arg5, NvU32 arg6, NvU32 *arg7) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrMemUtilsAllocateEccScrubber_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrMemUtilsAllocateEccAllocScrubber_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrUpdateSurfaceCompression_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, Memory *arg3, NvBool arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


NV_STATUS memmgrInitFbRegionsHal_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrInitFbRegionsHal_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


NvU64 memmgrGetVgpuHostRmReservedFb_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);

NvU64 memmgrGetVgpuHostRmReservedFb_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);

NvU64 memmgrGetVgpuHostRmReservedFb_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId);

static inline NvU64 memmgrGetVgpuHostRmReservedFb_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 vgpuTypeId) {
    return 0;
}


static inline NvBool memmgrVerifyDepthSurfaceAttrs_88bc07(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU32 arg3) {
    return NV_TRUE;
}


NV_STATUS memmgrAllocMemToSaveVgaWorkspace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg3, MEMORY_DESCRIPTOR **arg4);

static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg3, MEMORY_DESCRIPTOR **arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS memmgrAllocMemToSaveVgaWorkspace_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_DESCRIPTOR **arg3, MEMORY_DESCRIPTOR **arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


static inline NvBool memmgrVerifyComprAttrs_88bc07(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU32 arg3, NvU32 arg4) {
    return NV_TRUE;
}


static inline NvBool memmgrIsKindBlocklinear_3dd2c9(struct MemoryManager *pMemoryManager, NvU32 arg2) {
    return NV_FALSE;
}


static inline NvU32 memmgrChooseKindCompressCForMS2_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3) {
    return 0;
}


static inline NV_STATUS memmgrGetUncompressedKindForMS2_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


NvBool memmgrIsSurfaceBlockLinear_TU102(struct MemoryManager *pMemoryManager, Memory *arg2, NvU32 arg3);


static inline void memmgrChannelPushSemaphoreMethodsBlock_f2d351(struct MemoryManager *pMemoryManager, NvU32 arg2, NvU64 arg3, NvU32 arg4, NvU32 **arg5) {
    NV_ASSERT_PRECOMP(0);
}


static inline void memmgrChannelPushAddressMethodsBlock_f2d351(struct MemoryManager *pMemoryManager, NvBool arg2, NvU32 arg3, RmPhysAddr arg4, NvU32 **arg5) {
    NV_ASSERT_PRECOMP(0);
}


static inline NV_STATUS memmgrValidateFBEndReservation_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


static inline NV_STATUS memmgrReserveMemoryForFakeWPR_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


static inline NV_STATUS memmgrReserveMemoryForPmu_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}


void memmgrFreeFbsrMemory_KERNEL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


static inline NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrReserveVgaWorkspaceMemDescForFbsr_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);


static inline NV_STATUS memmgrCalculateHeapOffsetWithGSP_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *offset) {
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrSaveDisplayFrmReservedRegion_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}


static inline NV_STATUS memmgrRestoreDisplayFrmReservedRegion_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}


NV_STATUS memmgrGetCarveoutRegionInfo_KERNEL(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams);

NV_STATUS memmgrGetCarveoutRegionInfo_GB10B(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams);

static inline NV_STATUS memmgrGetCarveoutRegionInfo_56cd7a(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams) {
    return NV_OK;
}


NV_STATUS memmgrConstructEngine_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, ENGDESCRIPTOR arg3);

NV_STATUS memmgrStatePreInitLocked_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrStateInitLocked_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrStateLoad_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3);

NV_STATUS memmgrStatePostLoad_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3);

NV_STATUS memmgrStatePreUnload_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3);

NV_STATUS memmgrStateUnload_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3);

void memmgrStateDestroy_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrDeterminePageSize_IMPL(struct MemoryManager *pMemoryManager, NvHandle hClient, NvU64 memSize, NvU32 memFormat, NvU32 pageFormatFlags, NvU32 *pRetAttr, NvU32 *pRetAttr2);

static inline NvU64 memmgrDeterminePageSize_b845ef(struct MemoryManager *pMemoryManager, NvHandle hClient, NvU64 memSize, NvU32 memFormat, NvU32 pageFormatFlags, NvU32 *pRetAttr, NvU32 *pRetAttr2) {
    return 4096;
}

NV_STATUS memmgrFreeHwResources_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3);

static inline NV_STATUS memmgrFreeHwResources_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrCreateHeap_IMPL(struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrCreateHeap_56cd7a(struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

static inline NV_STATUS memmgrInitFbRegions_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrInitFbRegions_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrAllocateConsoleRegion_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrAllocateConsoleRegion_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrAllocateConsoleRegion_GB10B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrScrubHandlePostSchedulingEnable_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrScrubHandlePostSchedulingEnable_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrScrubHandlePreSchedulingDisable_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrScrubHandlePreSchedulingDisable_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrMemUtilsChannelInitialize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);

static inline NV_STATUS memmgrMemUtilsChannelInitialize_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return NV_OK;
}

NV_STATUS memmgrMemUtilsCopyEngineInitialize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);

static inline NV_STATUS memmgrMemUtilsCopyEngineInitialize_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return NV_OK;
}

NV_STATUS memmgrMemUtilsSec2CtxInit_GH100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);

static inline NV_STATUS memmgrMemUtilsSec2CtxInit_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrMemUtilsGetCopyEngineClass_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pClass);

static inline NV_STATUS memmgrMemUtilsGetCopyEngineClass_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pClass) {
    return NV_OK;
}

NV_STATUS memmgrMemUtilsCreateMemoryAlias_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);

static inline NV_STATUS memmgrMemUtilsCreateMemoryAlias_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return NV_OK;
}

NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_GH100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvBool arg4, RmPhysAddr arg5, NvU32 arg6, NV_ADDRESS_SPACE arg7);

static inline NvBool memmgrMemUtilsCheckMemoryFastScrubEnable_3dd2c9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvBool arg4, RmPhysAddr arg5, NvU32 arg6, NV_ADDRESS_SPACE arg7) {
    return NV_FALSE;
}

NV_STATUS memmgrAllocHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo);

static inline NV_STATUS memmgrAllocHal_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrFreeHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo, PRMTIMEOUT pTimeout);

static inline NV_STATUS memmgrFreeHal_92bfc3(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo, PRMTIMEOUT pTimeout) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrGetBankPlacementData_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pBankPlacementLowData);

static inline NV_STATUS memmgrGetBankPlacementData_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *pBankPlacementLowData) {
    return NV_ERR_NOT_SUPPORTED;
}

void memmgrDirtyForPmTest_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool partialDirty);

static inline void memmgrDirtyForPmTest_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool partialDirty) {
    return;
}

NvU64 memmgrGetReservedHeapSizeMb_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetReservedHeapSizeMb_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NV_STATUS memmgrAllocDetermineAlignment_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment);

NV_STATUS memmgrAllocDetermineAlignment_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment);

static inline NV_STATUS memmgrAllocDetermineAlignment_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pMemSize, NvU64 *pAlign, NvU64 alignPad, NvU32 allocFlags, NvU32 retAttr, NvU32 retAttr2, NvU64 hwAlignment) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NvU64 memmgrGetMaxContextSize_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_AD102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_AD104(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU64 memmgrGetMaxContextSize_GB202(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetMaxContextSize_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

void memmgrHandleSizeOverrides_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrHandleSizeOverrides_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}

NV_STATUS memmgrFinishHandleSizeOverrides_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrFinishHandleSizeOverrides_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrGetBAR1InfoForDevice_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, PGETBAR1INFO bar1Info);

static inline NV_STATUS memmgrGetBAR1InfoForDevice_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Device *pDevice, PGETBAR1INFO bar1Info) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU64 memmgrGetFbTaxSize_VF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetFbTaxSize_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

void memmgrScrubRegistryOverrides_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

void memmgrScrubRegistryOverrides_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrScrubRegistryOverrides_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}

NvU64 memmgrGetRsvdSizeForSr_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetRsvdSizeForSr_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvBool memmgrComparePhysicalAddresses_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvU64 arg4, NvU32 arg5, NvU64 arg6);

static inline NvBool memmgrComparePhysicalAddresses_86b752(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 arg3, NvU64 arg4, NvU32 arg5, NvU64 arg6) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

RmPhysAddr memmgrGetInvalidOffset_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline RmPhysAddr memmgrGetInvalidOffset_c732fb(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 4294967295U;
}

NvU64 memmgrGetAddrSpaceSizeMB_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetAddrSpaceSizeMB_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU64 memmgrGetUsableMemSizeMB_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU64 memmgrGetUsableMemSizeMB_13cd8d(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

NvBool memmgrIsKindCompressible_TU102(struct MemoryManager *pMemoryManager, NvU32 arg2);

static inline NvBool memmgrIsKindCompressible_3dd2c9(struct MemoryManager *pMemoryManager, NvU32 arg2) {
    return NV_FALSE;
}

NvU32 memmgrGetPteKindBl_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetPteKindBl_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 memmgrGetPteKindBl_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetPteKindPitch_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetPteKindPitch_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrChooseKindZ_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

NvU32 memmgrChooseKindZ_GB202(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

NvU32 memmgrChooseKindZ_GB20B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

static inline NvU32 memmgrChooseKindZ_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 memmgrChooseKindCompressZ_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

NvU32 memmgrChooseKindCompressZ_GB202(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

NvU32 memmgrChooseKindCompressZ_GB20B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

static inline NvU32 memmgrChooseKindCompressZ_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 memmgrChooseKindCompressC_GP100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3);

static inline NvU32 memmgrChooseKindCompressC_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 memmgrGetPteKindGenericMemoryCompressible_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetPteKindGenericMemoryCompressible_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetUncompressedKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire);

NvU32 memmgrGetUncompressedKind_GB202(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire);

NvU32 memmgrGetUncompressedKind_GB20B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire);

static inline NvU32 memmgrGetUncompressedKind_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 kind, NvBool releaseReacquire) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 memmgrGetCompressedKind_TU102(struct MemoryManager *pMemoryManager, NvU32 kind, NvBool bDisablePlc);

NvU32 memmgrGetCompressedKind_GB202(struct MemoryManager *pMemoryManager, NvU32 kind, NvBool bDisablePlc);

NvU32 memmgrGetCompressedKind_GB20B(struct MemoryManager *pMemoryManager, NvU32 kind, NvBool bDisablePlc);

static inline NvU32 memmgrGetCompressedKind_d1515c(struct MemoryManager *pMemoryManager, NvU32 kind, NvBool bDisablePlc) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, kind);
}

NV_STATUS memmgrChooseKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3, NvU32 arg4, NvU32 *arg5);

static inline NV_STATUS memmgrChooseKind_474d46(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_PAGE_FORMAT *arg3, NvU32 arg4, NvU32 *arg5) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvBool memmgrIsKind_TU102(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg2, NvU32 arg3);

NvBool memmgrIsKind_GB202(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg2, NvU32 arg3);

NvBool memmgrIsKind_GB20B(struct MemoryManager *pMemoryManager, FB_IS_KIND_OP arg2, NvU32 arg3);

NvU32 memmgrGetMessageKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetMessageKind_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetDefaultPteKindForNoHandle_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetDefaultPteKindForNoHandle_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NV_STATUS memmgrGetFlaKind_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg3);

static inline NV_STATUS memmgrGetFlaKind_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 *arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool memmgrIsMemDescSupportedByFla_GA100(OBJGPU *arg1, struct MemoryManager *arg2, MEMORY_DESCRIPTOR *arg3);

NvBool memmgrIsMemDescSupportedByFla_GB100(OBJGPU *arg1, struct MemoryManager *arg2, MEMORY_DESCRIPTOR *arg3);

static inline NvBool memmgrIsMemDescSupportedByFla_46f6a7(OBJGPU *arg1, struct MemoryManager *arg2, MEMORY_DESCRIPTOR *arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool memmgrIsValidFlaPageSize_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 pageSize, NvBool bIsMulticast);

NvBool memmgrIsValidFlaPageSize_GB100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 pageSize, NvBool bIsMulticast);

static inline NvBool memmgrIsValidFlaPageSize_3dd2c9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 pageSize, NvBool bIsMulticast) {
    return NV_FALSE;
}

NvU32 memmgrGetHwPteKindFromSwPteKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind);

static inline NvU32 memmgrGetHwPteKindFromSwPteKind_6a0a80(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    return pteKind;
}

NvU32 memmgrGetSwPteKindFromHwPteKind_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind);

static inline NvU32 memmgrGetSwPteKindFromHwPteKind_6a0a80(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 pteKind) {
    return pteKind;
}

void memmgrGetPteKindForScrubber_TU102(struct MemoryManager *pMemoryManager, NvU32 *arg2);

static inline void memmgrGetPteKindForScrubber_f2d351(struct MemoryManager *pMemoryManager, NvU32 *arg2) {
    NV_ASSERT_PRECOMP(0);
}

NvU32 memmgrGetCtagOffsetFromParams_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3);

static inline NvU32 memmgrGetCtagOffsetFromParams_1a0c2b(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3) {
    return -1;
}

void memmgrSetCtagOffsetInParams_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3, NvU32 arg4);

static inline void memmgrSetCtagOffsetInParams_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *arg3, NvU32 arg4) {
    return;
}

NvU32 memmgrDetermineComptag_TU102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3);

static inline NvU32 memmgrDetermineComptag_13cd8d(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, RmPhysAddr arg3) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

NV_STATUS memmgrScrubMapDoorbellRegion_GV100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3);

static inline NV_STATUS memmgrScrubMapDoorbellRegion_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, OBJCHANNEL *arg3) {
    return NV_OK;
}

NV_STATUS memmgrSetAllocParameters_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo);

static inline NV_STATUS memmgrSetAllocParameters_dffb6f(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, FB_ALLOC_INFO *pFbAllocInfo) {
    NV_ASSERT_PRECOMP(0);
    return NV_OK;
}

void memmgrCalcReservedFbSpaceForUVM_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3);

static inline void memmgrCalcReservedFbSpaceForUVM_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3) {
    return;
}

void memmgrCalcReservedFbSpaceHal_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3, NvU64 *arg4, NvU64 *arg5);

static inline void memmgrCalcReservedFbSpaceHal_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *arg3, NvU64 *arg4, NvU64 *arg5) {
    return;
}

NvU32 memmgrGetGrHeapReservationSize_VF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetGrHeapReservationSize_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetGrHeapReservationSize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetRunlistEntriesReservedFbSpace_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetRunlistEntriesReservedFbSpace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU32 memmgrGetUserdReservedFbSpace_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvU32 memmgrGetUserdReservedFbSpace_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrCheckReservedMemorySize_GK104(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrCheckReservedMemorySize_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrInitReservedMemory_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3);

static inline NV_STATUS memmgrInitReservedMemory_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 arg3) {
    return NV_OK;
}

NV_STATUS memmgrPreInitReservedMemory_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrPreInitReservedMemory_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrReadMmuLock_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi);

static inline NV_STATUS memmgrReadMmuLock_ccda6f(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvBool *pbIsValid, NvU64 *pMmuLockLo, NvU64 *pMmuLockHi) {
    *pbIsValid = NV_FALSE;
    return NV_OK;
}

NV_STATUS memmgrBlockMemLockedMemory_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrBlockMemLockedMemory_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize);

static inline NV_STATUS memmgrInsertUnprotectedRegionAtBottomOfFb_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU64 *pSize) {
    return NV_OK;
}

NV_STATUS memmgrInitBaseFbRegions_FWCLIENT(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrInitBaseFbRegions_VF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrInitBaseFbRegions_GP102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrInitBaseFbRegions_T234(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrInitBaseFbRegions_5baef9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void memmgrGetDisablePlcKind_GA100(struct MemoryManager *pMemoryManager, NvU32 *pteKind);

static inline void memmgrGetDisablePlcKind_b3696a(struct MemoryManager *pMemoryManager, NvU32 *pteKind) {
    return;
}

void memmgrEnableDynamicPageOfflining_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

void memmgrEnableDynamicPageOfflining_GA102(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline void memmgrEnableDynamicPageOfflining_b3696a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return;
}

NV_STATUS memmgrSetMemDescPageSize_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg3, ADDRESS_TRANSLATION arg4, RM_ATTR_PAGE_SIZE arg5);

static inline NV_STATUS memmgrSetMemDescPageSize_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR arg3, ADDRESS_TRANSLATION arg4, RM_ATTR_PAGE_SIZE arg5) {
    return NV_OK;
}

static inline NV_STATUS memmgrSetPartitionableMem_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrSetPartitionableMem_IMPL(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrAllocMIGGPUInstanceMemory_VF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle *phMemory, struct NV_RANGE *pAddrRange, struct Heap **ppMemoryPartitionHeap);

NV_STATUS memmgrAllocMIGGPUInstanceMemory_PF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NvU32 swizzId, NvHandle *phMemory, struct NV_RANGE *pAddrRange, struct Heap **ppMemoryPartitionHeap);

NV_STATUS memmgrGetBlackListPagesForHeap_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap);

static inline NV_STATUS memmgrGetBlackListPagesForHeap_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct Heap *pHeap) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrGetBlackListPages_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount);

NV_STATUS memmgrGetBlackListPages_GA100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount);

static inline NV_STATUS memmgrGetBlackListPages_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, BLACKLIST_ADDRESS *pBlAddrs, NvU32 *pCount) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrDiscoverMIGPartitionableMemoryRange_VF(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct NV_RANGE *pMemoryRange);

static inline NV_STATUS memmgrDiscoverMIGPartitionableMemoryRange_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, struct NV_RANGE *pMemoryRange) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 memmgrGetFBEndReserveSizeEstimate_GB100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NvU32 memmgrGetFBEndReserveSizeEstimate_GM107(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

NV_STATUS memmgrInitZeroFbRegionsHal_GB10B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrInitZeroFbRegionsHal_56cd7a(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_OK;
}

NV_STATUS memmgrAllocScanoutCarveoutRegionResources_GB10B(struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData, NvU32 owner, NvU32 *pHeapFlag, MEMORY_DESCRIPTOR *pMemDesc);

static inline NV_STATUS memmgrAllocScanoutCarveoutRegionResources_46f6a7(struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData, NvU32 owner, NvU32 *pHeapFlag, MEMORY_DESCRIPTOR *pMemDesc) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrAllocFromScanoutCarveoutRegion_GB10B(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NvU32 owner, NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc, NvU32 *pHeapFlag, PMEMORY_DESCRIPTOR *ppMemDesc);

static inline NV_STATUS memmgrAllocFromScanoutCarveoutRegion_46f6a7(POBJGPU pGpu, struct MemoryManager *pMemoryManager, NvU32 owner, NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc, NvU32 *pHeapFlag, PMEMORY_DESCRIPTOR *ppMemDesc) {
    return NV_ERR_NOT_SUPPORTED;
}

void memmgrFreeScanoutCarveoutRegionResources_GB10B(struct MemoryManager *pMemoryManager, NvU64 base);

static inline void memmgrFreeScanoutCarveoutRegionResources_b3696a(struct MemoryManager *pMemoryManager, NvU64 base) {
    return;
}

void memmgrFreeFromScanoutCarveoutRegion_GB10B(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc);

static inline void memmgrFreeFromScanoutCarveoutRegion_b3696a(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc) {
    return;
}

NV_STATUS memmgrCreateScanoutCarveoutHeap_GB10B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrCreateScanoutCarveoutHeap_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrDestroyScanoutCarveoutHeap_GB10B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NV_STATUS memmgrDestroyScanoutCarveoutHeap_46f6a7(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS memmgrDuplicateFromScanoutCarveoutRegion_GB10B(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc);

static inline NV_STATUS memmgrDuplicateFromScanoutCarveoutRegion_46f6a7(POBJGPU pGpu, struct MemoryManager *pMemoryManager, PMEMORY_DESCRIPTOR pMemDesc) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool memmgrIsMemoryIoCoherent_GB20B(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData);

static inline NvBool memmgrIsMemoryIoCoherent_88bc07(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, NV_MEMORY_ALLOCATION_PARAMS *pAllocData) {
    return NV_TRUE;
}

NvU8 memmgrGetLocalizedOffset_GB100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvU8 memmgrGetLocalizedOffset_4a4dee(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return 0;
}

NvBool memmgrIsFlaSysmemSupported_GB100(OBJGPU *pGpu, struct MemoryManager *pMemoryManager);

static inline NvBool memmgrIsFlaSysmemSupported_3dd2c9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_FALSE;
}

static inline NvBool memmgrGetLocalizedMemorySupported_88bc07(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_TRUE;
}

static inline NvBool memmgrGetLocalizedMemorySupported_3dd2c9(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return NV_FALSE;
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

static inline NvU64 memmgrLocalEgmBaseAddress(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->localEgmBasePhysAddr;
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

static inline NvU64 memmgrGetRsvdMemoryBase(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->rsvdMemoryBase;
}

static inline NvU64 memmgrGetRsvdMemorySize(struct MemoryManager *pMemoryManager) {
    return pMemoryManager->rsvdMemorySize;
}

static inline NvBool memmgrBug3922001DisableCtxBufOnSim(OBJGPU *pGpu, struct MemoryManager *pMemoryManager) {
    return pMemoryManager->bBug3922001DisableCtxBufOnSim;
}

#undef PRIVATE_FIELD


#endif // MEM_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MGR_NVOC_H_
