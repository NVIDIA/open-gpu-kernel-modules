
#ifndef _G_KERN_GMMU_NVOC_H_
#define _G_KERN_GMMU_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
3* SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel GMMU module header
*       Defines and structures used on CPU RM for the GMMU object.
*
******************************************************************************/

#pragma once
#include "g_kern_gmmu_nvoc.h"

#ifndef KERN_GMMU_H
#define KERN_GMMU_H

#include "core/core.h"
#include "core/strict.h"
#include "nvtypes.h"
#include "nvoc/prelude.h"
#include "nvoc/object.h"
#include "gpu/mmu/mmu_trace.h"
#include "mmu/gmmu_fmt.h"
#include "class/cl90f1.h"    // FERMI_VASPACE_A

#include "gpu/gpu_timeout.h"
#include "containers/queue.h"
#include "gpu/eng_state.h"
#include "gpu/intr/intr_service.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h" // RM_PAGE_SIZE_64K
#include "mmu/mmu_walk.h"

#include "gpu/gpu_halspec.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"  // NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS

#include "class/clc369.h" // MMU_FAULT_BUFFER

typedef struct COMPR_INFO COMPR_INFO;

typedef struct GVAS_GPU_STATE GVAS_GPU_STATE;

typedef struct _fifo_mmu_exception_data FIFO_MMU_EXCEPTION_DATA;

/*!
 * Family of GMMU formats sharing the same version and PDE/PTE defines
 * but with differing big page sizes.
 * The term "family" is used here in the mathematical (set theory) sense.
 *
 * nv4kPte: GV100+ supports NV4K encoding, @ref gmmuStateInitHal_GV100 for more
 *
 */
typedef struct
{
    GMMU_FMT_PDE_MULTI pdeMulti;
    GMMU_FMT_PDE       pde;
    GMMU_FMT_PTE       pte;
    GMMU_ENTRY_VALUE   sparsePte;
    GMMU_ENTRY_VALUE   sparsePde;
    GMMU_ENTRY_VALUE   sparsePdeMulti;
    GMMU_ENTRY_VALUE   nv4kPte;
    GMMU_ENTRY_VALUE   bug2720120WarPde0;
    GMMU_ENTRY_VALUE   bug2720120WarPde1;
    GMMU_FMT          *pFmts[GMMU_FMT_MAX_BIG_PAGE_SIZES];
} GMMU_FMT_FAMILY;

/*!
 * This structure contains information needed for issuing a commit TLB invalidate
 * through RmTest.
 */
typedef struct
{
    NvU32       gfid;
    NvBool      invalidateAll;
} COMMIT_TLB_INVALIDATE_TEST_PARAMS;

/*!
 * This structure contains information needed for issuing a TLB invalidate.
 */
typedef struct
{
    RmPhysAddr pdbAddress;
    NvU32      pdbAperture;
    NvU32      gfid;
    NvU32      regVal;
    RMTIMEOUT  timeout;
} TLB_INVALIDATE_PARAMS;

typedef enum
{
    NON_REPLAYABLE_FAULT_BUFFER = 0,
    REPLAYABLE_FAULT_BUFFER,
    //this should always be the last entry
    NUM_FAULT_BUFFERS
} FAULT_BUFFER_TYPE;

/*!
 * This structure holds information about a page
 * of memory backing the fault buffer.
 */
typedef struct
{
    /*! Virtual address of this page */
    NvP64 pAddress;

    /*! Cookie returned by memdescMap() */
    NvP64 pPriv;
} GMMU_FAULT_BUFFER_PAGE;

/*!
 * This structure holds the information about MMU HW Fault buffer which is mapped on BAR2
 * and is utilized by MMU for reporting MMU faults to SW
 */
struct HW_FAULT_BUFFER
{
    NvU64 bar2FaultBufferAddr;
    MEMORY_DESCRIPTOR *pFaultBufferMemDesc;
    /*!
     * cookie that is stored for the CPU mapping
     */
    NvP64 hCpuFaultBuffer;
    NvP64 kernelVaddr;

    GMMU_FAULT_BUFFER_PAGE *pBufferPages;

    NvU32 cachedGetIndex;

    /*!
     * cached fault buffer size
     */
    NvU32 faultBufferSize;
};

/*!
 * This structure holds information shared between CPU-RM
 * and GSP-RM
 */
typedef struct
{
    /*!
     * The GET index of replayable shadow buffer. This
     * is updated by UVM driver and read by GSP-RM
     */
    NvU32 swGetIndex;
} FAULT_BUFFER_SHARED_MEMORY;

/*!
 * This structure holds information about the client shadow fault buffer.
 */
typedef struct
{
    /*!
     * Pointer to circular queue structure shared by the RM with a
     * privileged client, used as the shadow fault buffer for holding
     * non-replayable faults.
     * This structure is shared between CPU-RM and GSP-RM in GSP
     * enabled driver.
     */
    NvP64 pQueue;

    /*! Memory descriptors associated with the queue. */
    MEMORY_DESCRIPTOR *pQueueMemDesc;

    NvP64 pQueueAddress;

    /*!
     * Execution context for the queue. Holds environment specific
     * data that enable queue usage
     */
    QueueContext queueContext;

    /*! Cookie returned by memdescMap() */
    NvP64 pQueuePriv;

    /*! Memory descriptor associated with the buffer. */
    MEMORY_DESCRIPTOR *pBufferMemDesc;

    NvP64 pBufferAddress;

    /*! Cookie returned by memdescMap() */
    NvP64 pBufferPriv;

    /*! GSP only split mapping of the buffer. */
    GMMU_FAULT_BUFFER_PAGE *pBufferPages;

    NvU32 numBufferPages;

    /*!
     * Start index of the page containing the fault buffer metadata.
     * 0 if no metadata is present.
     */
    NvU32 metadataStartIndex;

    /*!
     * Used only by the replayable fault buffer. Memory descriptor used to
     * describe shared memory b/w CPU-RM and GSP-RM.
     */
    MEMORY_DESCRIPTOR *pFaultBufferSharedMemDesc;

    NvP64 pFaultBufferSharedMemoryAddress;

    NvP64 pFaultBufferSharedMemoryPriv;

    NvP64 pFaultBufferMetadataAddress;

} GMMU_CLIENT_SHADOW_FAULT_BUFFER;

/*!
 * Top level structure containing all dataStructures used in MMU fault handling.
 */
struct GMMU_FAULT_BUFFER
{
    struct HW_FAULT_BUFFER hwFaultBuffers[NUM_FAULT_BUFFERS];

    /*!
     * Unique client and object handle stored
     * In VOLTA this is for MMU_FAULT_BUFFER, in PASCAL for MAXWELL_FAULT_BUFFER_A
     */
    NvHandle hFaultBufferClient;
    NvHandle hFaultBufferObject;

    /*!
     * Pointer to Circular Queue structure used as shadow fault buffer for
     * holding fatal fault packets serviced by RM
     */
    NvP64 pRmShadowFaultBuffer;

    /*!
     * Client shadow fault buffer data and pointer protected by gpu locks.
     * Client may allocate upto 2 shadow buffers one each for replayable and
     * non-replayable faults
     */
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer[NUM_FAULT_BUFFERS];
    GMMU_CLIENT_SHADOW_FAULT_BUFFER clientShadowFaultBuffer[NUM_FAULT_BUFFERS];

    /*!
     * SpinLock to protect shadow buffer pointers
     */
    PORT_SPINLOCK *pShadowFaultBufLock;

    /*!
     * Flag stating fatalfault interrupt pending
     */
    NvS32 fatalFaultIntrPending;

    /*! Generational counter for fault buffer. Incremented when the fault buffer wraps around. */
    volatile NvU64 faultBufferGenerationCounter;
};

typedef struct GMMU_FAULT_PACKET
{
    // 32 bytes MMU fault packet
    NvU8 faultPacket[NVC369_BUF_SIZE];
} GMMU_FAULT_PACKET;

// Initialize Circular Queue for MMU Shadow fault buffer
MAKE_QUEUE_CIRCULAR(GMMU_SHADOW_FAULT_BUF, GMMU_FAULT_PACKET);

#define GMMU_FAULT_PACKET_METADATA_SIZE                32
#define GMMU_FAULT_PACKET_METADATA_AUTHTAG_IDX          0
#define GMMU_FAULT_PACKET_METADATA_AUTHTAG_SIZE        16
#define GMMU_FAULT_PACKET_METADATA_VALID_IDX           16
#define GMMU_FAULT_PACKET_METADATA_VALID_SIZE           1
#define GMMU_FAULT_PACKET_METADATA_VALID_YES      NV_TRUE
#define GMMU_FAULT_PACKET_METADATA_VALID_NO      NV_FALSE

typedef struct GMMU_FAULT_PACKET_METADATA
{
    NvU8 metadata[GMMU_FAULT_PACKET_METADATA_SIZE];
} GMMU_FAULT_PACKET_METADATA;

/*!
 * Structure that holds different parameters passed by an engine to kgmmuInstBlkInit
 * for initializing their instance blocks.
 */
typedef struct
{
    NvBool               bIsClientAdmin;
    NvBool               bIsFaultReplayable;
    /*
     * Defer the bus flush during the instance block init.
     * If this field is set, the kgmmuInstBlkInit() routine won't do flush after the CPU writes.
     * The caller of the kgmmuInstBlkInit() function has to explicit flush.
     * This is useful if the caller does back to back updates to instance block.
     * For e.g. Subcontext array init during channel setup.
     */
    NvBool               bDeferFlush;
    NvU64                uvmKernelPrivRegion;

    // Instance block is being updated for a zombie subcontext.
    NvBool               bIsZombieSubctx;
    NvU8                *pInstBlk;      // VA of instance block.
} INST_BLK_INIT_PARAMS, *PINST_BLK_INIT_PARAMS;

typedef enum
{
    fault_invalidPde              = 0x00000000,
    fault_invalidPdeSize          = 0x00000001,
    fault_invalidPte              = 0x00000002,
    fault_limitViolation          = 0x00000003,
    fault_unboundInstBlock        = 0x00000004,
    fault_privViolation           = 0x00000005,
    fault_write                   = 0x00000006,
    fault_read                    = 0x00000007,
    fault_pitchMaskViolation      = 0x00000008,
    fault_workCreation            = 0x00000009,
    fault_unsupportedAperture     = 0x0000000a,
    fault_compressionFailure      = 0x0000000b,
    fault_cc_violation            = 0x0000000b,
    fault_unsupportedKind         = 0x0000000c,
    fault_regionViolation         = 0x0000000d,
    fault_poison                  = 0x0000000e,
    fault_atomic                  = 0x0000000f
} FAULT_TYPE;

typedef struct
{
    INST_BLOCK_DESC         mmuFaultInstBlock;
    NvU64                   mmuFaultAddress;
    NvU64                   mmuFaultTimestamp;
    FAULT_TYPE              mmuFaultType;
    NvU32                   mmuFaultAccessType;
    NvU32                   mmuFaultEngineId;
    NvU32                   mmuFaultClientId;
    NvU32                   mmuFaultClientType;
    NvU32                   mmuFaultGpcId;
    NvU8                    bFaultEntryValid        : 1;
    NvU8                    bFaultInProtectedMode   : 1;
    NvU8                    bFaultTypeReplayable    : 1;
    NvU8                    bReplayableFaultEn      : 1;
} MMU_FAULT_BUFFER_ENTRY;

/*!
 * This structure contains information needed for targetted fault cancel
 * This is passed in by UVM using SW methods (cl0076.h)
 */
typedef struct
{
    NvU32 clientId;
    NvU32 gpcId;
    INST_BLOCK_DESC instBlock;
} GMMU_FAULT_CANCEL_INFO;

#define VMMU_MAX_GFID 64

/*! Fake sparse table defines */
#define NV_GMMU_FAKE_SPARSE_TABLE_LEVEL_LO          38  // GH100 PDE2 virtAddrBitLo=38
#define NV_GMMU_FAKE_SPARSE_TABLE_LEVELS            3   // PDE2 to PDE4
#define NV_GMMU_FAKE_SPARSE_TABLE_BITS_PER_LEVEL    9
#define NV_GMMU_FAKE_SPARSE_TABLE_ENTRY_SIZE        8


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelGmmu {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelGmmu *__nvoc_pbase_KernelGmmu;    // kgmmu

    // Vtable with 90 per-object function pointers
    NV_STATUS (*__kgmmuConstructEngine__)(OBJGPU *, struct KernelGmmu * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateInitLocked__)(OBJGPU *, struct KernelGmmu * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePostLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kgmmuStatePreUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    void (*__kgmmuStateDestroy__)(OBJGPU *, struct KernelGmmu * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kgmmuRegisterIntrService__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvBool (*__kgmmuClearInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceClearInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__kgmmuServiceInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__kgmmuServiceNotificationInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    NV_STATUS (*__kgmmuInstBlkVaLimitGet__)(struct KernelGmmu * /*this*/, struct OBJVASPACE *, NvU32, INST_BLK_INIT_PARAMS *, NvU32 *, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCommitTlbInvalidate__)(OBJGPU *, struct KernelGmmu * /*this*/, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    NvU32 (*__kgmmuSetTlbInvalidateMembarWarParameters__)(OBJGPU *, struct KernelGmmu * /*this*/, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuSetTlbInvalidationScope__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    void (*__kgmmuFmtInitPteComptagLine__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitPeerPteFld__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitPte__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *, const NvBool);  // halified (2 hals) body
    void (*__kgmmuFmtInitPde__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PDE *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *);  // halified (2 hals) body
    NvBool (*__kgmmuFmtIsVersionSupported__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitLevels__)(struct KernelGmmu * /*this*/, MMU_FMT_LEVEL *, const NvU32, const NvU32, const NvU32);  // halified (4 hals) body
    void (*__kgmmuFmtInitPdeMulti__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PDE_MULTI *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFmtFamiliesInit__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePtePcfFromSw__)(struct KernelGmmu * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePtePcfFromHw__)(struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePdePcfFromSw__)(struct KernelGmmu * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePdePcfFromHw__)(struct KernelGmmu * /*this*/, NvU32, GMMU_APERTURE, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuGetFaultRegisterMappings__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvU32 *, NvP64 *);  // halified (2 hals) body
    const char * (*__kgmmuGetFaultTypeString__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuIssueReplayableFaultBufferFlush__)(OBJGPU *, struct KernelGmmu * /*this*/, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuToggleFaultOnPrefetch__)(OBJGPU *, struct KernelGmmu * /*this*/, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferAllocSharedMemory__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    void (*__kgmmuFaultBufferFreeSharedMemory__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    NV_STATUS (*__kgmmuSetupWarForBug2720120__)(struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuGetGraphicsEngineId__)(struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuReadShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEngineBar1__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEngineBar2__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEnginePhysical__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCopyMmuFaults__)(OBJGPU *, struct KernelGmmu * /*this*/, struct THREAD_STATE_NODE *, NvU32 *, FAULT_BUFFER_TYPE, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuParseFaultPacket__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, NvP64);  // halified (2 hals) body
    void (*__kgmmuFaultBufferClearPackets__)(OBJGPU *, struct KernelGmmu * /*this*/, struct HW_FAULT_BUFFER *, NvU32, NvU32);  // halified (2 hals) body
    GMMU_FAULT_PACKET * (*__kgmmuFaultBufferGetFault__)(OBJGPU *, struct KernelGmmu * /*this*/, struct HW_FAULT_BUFFER *, NvU32);  // halified (2 hals) body
    NvU32 (*__kgmmuCopyFaultPacketToClientShadowBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, struct GMMU_FAULT_BUFFER *, FAULT_BUFFER_TYPE, NvU32, NvU32, NvU32, struct THREAD_STATE_NODE *, NvU32 *);  // halified (3 hals) body
    NvBool (*__kgmmuIsReplayableShadowFaultBufferFull__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_CLIENT_SHADOW_FAULT_BUFFER *, NvU32, NvU32);  // halified (3 hals) body
    NvU32 (*__kgmmuReadClientShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_BUFFER_TYPE);  // halified (4 hals) body
    void (*__kgmmuWriteClientShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_BUFFER_TYPE, NvU32);  // halified (4 hals) body
    NV_STATUS (*__kgmmuInitCeMmuFaultIdRange__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuGetMinCeEngineId__)(struct KernelGmmu * /*this*/);  // halified (3 hals) body
    NvU32 (*__kgmmuGetMaxCeEngineId__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (5 hals) body
    NV_STATUS (*__kgmmuFaultBufferMap__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferUnmap__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferInit__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferDestroy__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableFaultBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuDisableFaultBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32);  // halified (2 hals) body
    NvU32 (*__kgmmuSetAndGetDefaultFaultBufferSize__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE, NvU32);  // halified (2 hals) body
    void (*__kgmmuReadMmuFaultInstHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kgmmuReadMmuFaultAddrHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvU32 (*__kgmmuReadMmuFaultInfo__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    void (*__kgmmuWriteMmuFaultBufferSize__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals) body
    void (*__kgmmuWriteMmuFaultBufferHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableMmuFaultInterrupts__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuDisableMmuFaultInterrupts__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableMmuFaultOverflowIntr__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuSignExtendFaultAddress__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU64 *);  // halified (3 hals) body
    NV_STATUS (*__kgmmuGetFaultType__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_TYPE *);  // halified (3 hals) body
    NvBool (*__kgmmuIsP2PUnboundInstFault__)(struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kgmmuServiceVfPriFaults__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuTestVidmemAccessBitBufferError__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuDisableVidmemAccessBitBuf__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableVidmemAccessBitBuf__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    void (*__kgmmuClearAccessCounterWriteNak__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceMthdBuffFaultInBar2Fault__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultCancelTargeted__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_FAULT_CANCEL_INFO *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultCancelIssueInvalidate__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_FAULT_CANCEL_INFO *, TLB_INVALIDATE_PARAMS *, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceMmuFault__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, FIFO_MMU_EXCEPTION_DATA *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceUnboundInstBlockFault__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, FIFO_MMU_EXCEPTION_DATA *);  // halified (2 hals) body
    NvU32 (*__kgmmuGetEccCounts__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCreateFakeSparseTables__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals)
    NvU8 * (*__kgmmuGetFakeSparseEntry__)(OBJGPU *, struct KernelGmmu * /*this*/, const MMU_FMT_LEVEL *);  // halified (2 hals)
    void (*__kgmmuInitMissing__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreInitLocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateInitUnlocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreLoad__)(struct OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePostUnload__)(struct OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgmmuIsPresent__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 4 PDB properties
    NvBool PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED;
    NvBool PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED;
    NvBool PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE;
    NvBool PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE;

    // Data members
    NvBool bReportFlaTranslationXid;
    MEMORY_DESCRIPTOR *pFakeSparseBuffer;
    NvU64 fakeSparseEntry[3];
    NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo;
    NvU64 defaultBigPageSize;
    NvU32 uvmSharedIntrRmOwnsMask;
    GMMU_FMT_FAMILY *PRIVATE_FIELD(pFmtFamilies)[3];
    NvU32 PRIVATE_FIELD(PDEAperture);
    NvU32 PRIVATE_FIELD(PDEAttr);
    NvU32 PRIVATE_FIELD(PDEBAR1Aperture);
    NvU32 PRIVATE_FIELD(PDEBAR1Attr);
    NvU32 PRIVATE_FIELD(PTEAperture);
    NvU32 PRIVATE_FIELD(PTEAttr);
    NvU32 PRIVATE_FIELD(PTEBAR1Aperture);
    NvU32 PRIVATE_FIELD(PTEBAR1Attr);
    NvU64 PRIVATE_FIELD(overrideBigPageSize);
    NvBool PRIVATE_FIELD(bEnablePerVaspaceBigPage);
    NvBool PRIVATE_FIELD(bIgnoreHubTlbInvalidate);
    NvU64 PRIVATE_FIELD(maxVASize);
    struct NV_FIELD_ENUM_ENTRY PRIVATE_FIELD(pdeApertures)[5];
    struct NV_FIELD_ENUM_ENTRY PRIVATE_FIELD(pteApertures)[5];
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pWarSmallPageTable);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pWarPageDirectory0);
    struct GMMU_FAULT_BUFFER PRIVATE_FIELD(mmuFaultBuffer)[64];
    NvU64 PRIVATE_FIELD(sysmemBaseAddress);
    NvU32 PRIVATE_FIELD(minCeMmuFaultId);
    NvU32 PRIVATE_FIELD(maxCeMmuFaultId);
    NvBool PRIVATE_FIELD(bHugePageSupported);
    NvBool PRIVATE_FIELD(bPageSize512mbSupported);
    NvBool PRIVATE_FIELD(bPageSize256gbSupported);
    NvBool PRIVATE_FIELD(bBug2720120WarEnabled);
    NvBool PRIVATE_FIELD(bVaspaceInteropSupported);
};


struct KernelGmmu_PRIVATE {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelGmmu *__nvoc_pbase_KernelGmmu;    // kgmmu

    // Vtable with 90 per-object function pointers
    NV_STATUS (*__kgmmuConstructEngine__)(OBJGPU *, struct KernelGmmu * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateInitLocked__)(OBJGPU *, struct KernelGmmu * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePostLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kgmmuStatePreUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    void (*__kgmmuStateDestroy__)(OBJGPU *, struct KernelGmmu * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kgmmuRegisterIntrService__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvBool (*__kgmmuClearInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceClearInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__kgmmuServiceInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__kgmmuServiceNotificationInterrupt__)(OBJGPU *, struct KernelGmmu * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    NV_STATUS (*__kgmmuInstBlkVaLimitGet__)(struct KernelGmmu * /*this*/, struct OBJVASPACE *, NvU32, INST_BLK_INIT_PARAMS *, NvU32 *, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCommitTlbInvalidate__)(OBJGPU *, struct KernelGmmu * /*this*/, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    NvU32 (*__kgmmuSetTlbInvalidateMembarWarParameters__)(OBJGPU *, struct KernelGmmu * /*this*/, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuSetTlbInvalidationScope__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, TLB_INVALIDATE_PARAMS *);  // halified (2 hals) body
    void (*__kgmmuFmtInitPteComptagLine__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitPeerPteFld__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitPte__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PTE *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *, const NvBool);  // halified (2 hals) body
    void (*__kgmmuFmtInitPde__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PDE *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *);  // halified (2 hals) body
    NvBool (*__kgmmuFmtIsVersionSupported__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuFmtInitLevels__)(struct KernelGmmu * /*this*/, MMU_FMT_LEVEL *, const NvU32, const NvU32, const NvU32);  // halified (4 hals) body
    void (*__kgmmuFmtInitPdeMulti__)(struct KernelGmmu * /*this*/, struct GMMU_FMT_PDE_MULTI *, const NvU32, const struct NV_FIELD_ENUM_ENTRY *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFmtFamiliesInit__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePtePcfFromSw__)(struct KernelGmmu * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePtePcfFromHw__)(struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePdePcfFromSw__)(struct KernelGmmu * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuTranslatePdePcfFromHw__)(struct KernelGmmu * /*this*/, NvU32, GMMU_APERTURE, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuGetFaultRegisterMappings__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvU32 *, NvP64 *);  // halified (2 hals) body
    const char * (*__kgmmuGetFaultTypeString__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuIssueReplayableFaultBufferFlush__)(OBJGPU *, struct KernelGmmu * /*this*/, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuToggleFaultOnPrefetch__)(OBJGPU *, struct KernelGmmu * /*this*/, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferAllocSharedMemory__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    void (*__kgmmuFaultBufferFreeSharedMemory__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    NV_STATUS (*__kgmmuSetupWarForBug2720120__)(struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuGetGraphicsEngineId__)(struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuReadShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEngineBar1__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEngineBar2__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuIsFaultEnginePhysical__)(struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCopyMmuFaults__)(OBJGPU *, struct KernelGmmu * /*this*/, struct THREAD_STATE_NODE *, NvU32 *, FAULT_BUFFER_TYPE, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuParseFaultPacket__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, NvP64);  // halified (2 hals) body
    void (*__kgmmuFaultBufferClearPackets__)(OBJGPU *, struct KernelGmmu * /*this*/, struct HW_FAULT_BUFFER *, NvU32, NvU32);  // halified (2 hals) body
    GMMU_FAULT_PACKET * (*__kgmmuFaultBufferGetFault__)(OBJGPU *, struct KernelGmmu * /*this*/, struct HW_FAULT_BUFFER *, NvU32);  // halified (2 hals) body
    NvU32 (*__kgmmuCopyFaultPacketToClientShadowBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, struct GMMU_FAULT_BUFFER *, FAULT_BUFFER_TYPE, NvU32, NvU32, NvU32, struct THREAD_STATE_NODE *, NvU32 *);  // halified (3 hals) body
    NvBool (*__kgmmuIsReplayableShadowFaultBufferFull__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_CLIENT_SHADOW_FAULT_BUFFER *, NvU32, NvU32);  // halified (3 hals) body
    NvU32 (*__kgmmuReadClientShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_BUFFER_TYPE);  // halified (4 hals) body
    void (*__kgmmuWriteClientShadowBufPutIndex__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_BUFFER_TYPE, NvU32);  // halified (4 hals) body
    NV_STATUS (*__kgmmuInitCeMmuFaultIdRange__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgmmuGetMinCeEngineId__)(struct KernelGmmu * /*this*/);  // halified (3 hals) body
    NvU32 (*__kgmmuGetMaxCeEngineId__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (5 hals) body
    NV_STATUS (*__kgmmuFaultBufferMap__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferUnmap__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferInit__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferDestroy__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferLoad__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultBufferUnload__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableFaultBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuDisableFaultBuffer__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvBool, NvU32);  // halified (2 hals) body
    NvU32 (*__kgmmuSetAndGetDefaultFaultBufferSize__)(OBJGPU *, struct KernelGmmu * /*this*/, FAULT_BUFFER_TYPE, NvU32);  // halified (2 hals) body
    void (*__kgmmuReadMmuFaultInstHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kgmmuReadMmuFaultAddrHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvU32 (*__kgmmuReadMmuFaultInfo__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    void (*__kgmmuWriteMmuFaultBufferSize__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals) body
    void (*__kgmmuWriteMmuFaultBufferHiLo__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, NvU32, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableMmuFaultInterrupts__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuDisableMmuFaultInterrupts__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableMmuFaultOverflowIntr__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuSignExtendFaultAddress__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU64 *);  // halified (3 hals) body
    NV_STATUS (*__kgmmuGetFaultType__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32, FAULT_TYPE *);  // halified (3 hals) body
    NvBool (*__kgmmuIsP2PUnboundInstFault__)(struct KernelGmmu * /*this*/, NvU32, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kgmmuServiceVfPriFaults__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__kgmmuTestVidmemAccessBitBufferError__)(OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kgmmuDisableVidmemAccessBitBuf__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuEnableVidmemAccessBitBuf__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    void (*__kgmmuClearAccessCounterWriteNak__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceMthdBuffFaultInBar2Fault__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultCancelTargeted__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_FAULT_CANCEL_INFO *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuFaultCancelIssueInvalidate__)(OBJGPU *, struct KernelGmmu * /*this*/, GMMU_FAULT_CANCEL_INFO *, TLB_INVALIDATE_PARAMS *, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceMmuFault__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, FIFO_MMU_EXCEPTION_DATA *);  // halified (2 hals) body
    NV_STATUS (*__kgmmuServiceUnboundInstBlockFault__)(OBJGPU *, struct KernelGmmu * /*this*/, NvP64, FIFO_MMU_EXCEPTION_DATA *);  // halified (2 hals) body
    NvU32 (*__kgmmuGetEccCounts__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgmmuCreateFakeSparseTables__)(OBJGPU *, struct KernelGmmu * /*this*/);  // halified (2 hals)
    NvU8 * (*__kgmmuGetFakeSparseEntry__)(OBJGPU *, struct KernelGmmu * /*this*/, const MMU_FMT_LEVEL *);  // halified (2 hals)
    void (*__kgmmuInitMissing__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreInitLocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStateInitUnlocked__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePreLoad__)(struct OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgmmuStatePostUnload__)(struct OBJGPU *, struct KernelGmmu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgmmuIsPresent__)(struct OBJGPU *, struct KernelGmmu * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 4 PDB properties
    NvBool PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED;
    NvBool PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED;
    NvBool PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE;
    NvBool PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE;

    // Data members
    NvBool bReportFlaTranslationXid;
    MEMORY_DESCRIPTOR *pFakeSparseBuffer;
    NvU64 fakeSparseEntry[3];
    NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo;
    NvU64 defaultBigPageSize;
    NvU32 uvmSharedIntrRmOwnsMask;
    GMMU_FMT_FAMILY *pFmtFamilies[3];
    NvU32 PDEAperture;
    NvU32 PDEAttr;
    NvU32 PDEBAR1Aperture;
    NvU32 PDEBAR1Attr;
    NvU32 PTEAperture;
    NvU32 PTEAttr;
    NvU32 PTEBAR1Aperture;
    NvU32 PTEBAR1Attr;
    NvU64 overrideBigPageSize;
    NvBool bEnablePerVaspaceBigPage;
    NvBool bIgnoreHubTlbInvalidate;
    NvU64 maxVASize;
    struct NV_FIELD_ENUM_ENTRY pdeApertures[5];
    struct NV_FIELD_ENUM_ENTRY pteApertures[5];
    MEMORY_DESCRIPTOR *pWarSmallPageTable;
    MEMORY_DESCRIPTOR *pWarPageDirectory0;
    struct GMMU_FAULT_BUFFER mmuFaultBuffer[64];
    NvU64 sysmemBaseAddress;
    NvU32 minCeMmuFaultId;
    NvU32 maxCeMmuFaultId;
    NvBool bHugePageSupported;
    NvBool bPageSize512mbSupported;
    NvBool bPageSize256gbSupported;
    NvBool bBug2720120WarEnabled;
    NvBool bVaspaceInteropSupported;
};

#ifndef __NVOC_CLASS_KernelGmmu_TYPEDEF__
#define __NVOC_CLASS_KernelGmmu_TYPEDEF__
typedef struct KernelGmmu KernelGmmu;
#endif /* __NVOC_CLASS_KernelGmmu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGmmu
#define __nvoc_class_id_KernelGmmu 0x29362f
#endif /* __nvoc_class_id_KernelGmmu */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGmmu;

#define __staticCast_KernelGmmu(pThis) \
    ((pThis)->__nvoc_pbase_KernelGmmu)

#ifdef __nvoc_kern_gmmu_h_disabled
#define __dynamicCast_KernelGmmu(pThis) ((KernelGmmu*)NULL)
#else //__nvoc_kern_gmmu_h_disabled
#define __dynamicCast_KernelGmmu(pThis) \
    ((KernelGmmu*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGmmu)))
#endif //__nvoc_kern_gmmu_h_disabled

// Property macros
#define PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE_BASE_CAST
#define PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE_BASE_NAME PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE
#define PDB_PROP_KGMMU_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGMMU_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED_BASE_CAST
#define PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED_BASE_NAME PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED
#define PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE_BASE_CAST
#define PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE_BASE_NAME PDB_PROP_KGMMU_REPLAYABLE_FAULT_BUFFER_IN_USE
#define PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED_BASE_CAST
#define PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED_BASE_NAME PDB_PROP_KGMMU_FAULT_BUFFER_DISABLED

NV_STATUS __nvoc_objCreateDynamic_KernelGmmu(KernelGmmu**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGmmu(KernelGmmu**, Dynamic*, NvU32);
#define __objCreate_KernelGmmu(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGmmu((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kgmmuConstructEngine_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuConstructEngine__
#define kgmmuConstructEngine(pGpu, pKernelGmmu, arg3) kgmmuConstructEngine_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStateInitLocked_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStateInitLocked__
#define kgmmuStateInitLocked(pGpu, pKernelGmmu) kgmmuStateInitLocked_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuStateLoad_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStateLoad__
#define kgmmuStateLoad(pGpu, pKernelGmmu, arg3) kgmmuStateLoad_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStateUnload_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStateUnload__
#define kgmmuStateUnload(pGpu, pKernelGmmu, arg3) kgmmuStateUnload_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStatePostLoad_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStatePostLoad__
#define kgmmuStatePostLoad(pGpu, pKernelGmmu, arg3) kgmmuStatePostLoad_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStatePostLoad_HAL(pGpu, pKernelGmmu, arg3) kgmmuStatePostLoad_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStatePreUnload_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStatePreUnload__
#define kgmmuStatePreUnload(pGpu, pKernelGmmu, arg3) kgmmuStatePreUnload_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStatePreUnload_HAL(pGpu, pKernelGmmu, arg3) kgmmuStatePreUnload_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuStateDestroy_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuStateDestroy__
#define kgmmuStateDestroy(pGpu, pKernelGmmu) kgmmuStateDestroy_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuRegisterIntrService_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuRegisterIntrService__
#define kgmmuRegisterIntrService(pGpu, pKernelGmmu, arg3) kgmmuRegisterIntrService_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuClearInterrupt_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuClearInterrupt__
#define kgmmuClearInterrupt(pGpu, pKernelGmmu, pParams) kgmmuClearInterrupt_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuServiceInterrupt_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceInterrupt__
#define kgmmuServiceInterrupt(pGpu, pKernelGmmu, pParams) kgmmuServiceInterrupt_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuServiceNotificationInterrupt_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceNotificationInterrupt__
#define kgmmuServiceNotificationInterrupt(pGpu, pKernelGmmu, pParams) kgmmuServiceNotificationInterrupt_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuServiceNotificationInterrupt_HAL(pGpu, pKernelGmmu, pParams) kgmmuServiceNotificationInterrupt_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuInstBlkVaLimitGet_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuInstBlkVaLimitGet__
#define kgmmuInstBlkVaLimitGet(pKernelGmmu, pVAS, subctxId, pParams, pOffset, pData) kgmmuInstBlkVaLimitGet_DISPATCH(pKernelGmmu, pVAS, subctxId, pParams, pOffset, pData)
#define kgmmuInstBlkVaLimitGet_HAL(pKernelGmmu, pVAS, subctxId, pParams, pOffset, pData) kgmmuInstBlkVaLimitGet_DISPATCH(pKernelGmmu, pVAS, subctxId, pParams, pOffset, pData)
#define kgmmuCommitTlbInvalidate_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuCommitTlbInvalidate__
#define kgmmuCommitTlbInvalidate(pGpu, pKernelGmmu, pParams) kgmmuCommitTlbInvalidate_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuCommitTlbInvalidate_HAL(pGpu, pKernelGmmu, pParams) kgmmuCommitTlbInvalidate_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuSetTlbInvalidateMembarWarParameters_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuSetTlbInvalidateMembarWarParameters__
#define kgmmuSetTlbInvalidateMembarWarParameters(pGpu, pKernelGmmu, pParams) kgmmuSetTlbInvalidateMembarWarParameters_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuSetTlbInvalidateMembarWarParameters_HAL(pGpu, pKernelGmmu, pParams) kgmmuSetTlbInvalidateMembarWarParameters_DISPATCH(pGpu, pKernelGmmu, pParams)
#define kgmmuSetTlbInvalidationScope_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuSetTlbInvalidationScope__
#define kgmmuSetTlbInvalidationScope(pGpu, pKernelGmmu, flags, pParams) kgmmuSetTlbInvalidationScope_DISPATCH(pGpu, pKernelGmmu, flags, pParams)
#define kgmmuSetTlbInvalidationScope_HAL(pGpu, pKernelGmmu, flags, pParams) kgmmuSetTlbInvalidationScope_DISPATCH(pGpu, pKernelGmmu, flags, pParams)
#define kgmmuFmtInitPteComptagLine_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitPteComptagLine__
#define kgmmuFmtInitPteComptagLine(pKernelGmmu, pPte, version) kgmmuFmtInitPteComptagLine_DISPATCH(pKernelGmmu, pPte, version)
#define kgmmuFmtInitPteComptagLine_HAL(pKernelGmmu, pPte, version) kgmmuFmtInitPteComptagLine_DISPATCH(pKernelGmmu, pPte, version)
#define kgmmuFmtInitPeerPteFld_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitPeerPteFld__
#define kgmmuFmtInitPeerPteFld(pKernelGmmu, pPte, version) kgmmuFmtInitPeerPteFld_DISPATCH(pKernelGmmu, pPte, version)
#define kgmmuFmtInitPeerPteFld_HAL(pKernelGmmu, pPte, version) kgmmuFmtInitPeerPteFld_DISPATCH(pKernelGmmu, pPte, version)
#define kgmmuFmtInitPte_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitPte__
#define kgmmuFmtInitPte(pKernelGmmu, pPte, version, pPteApertures, bUnifiedAperture) kgmmuFmtInitPte_DISPATCH(pKernelGmmu, pPte, version, pPteApertures, bUnifiedAperture)
#define kgmmuFmtInitPte_HAL(pKernelGmmu, pPte, version, pPteApertures, bUnifiedAperture) kgmmuFmtInitPte_DISPATCH(pKernelGmmu, pPte, version, pPteApertures, bUnifiedAperture)
#define kgmmuFmtInitPde_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitPde__
#define kgmmuFmtInitPde(pKernelGmmu, pPde, version, pPdeApertures) kgmmuFmtInitPde_DISPATCH(pKernelGmmu, pPde, version, pPdeApertures)
#define kgmmuFmtInitPde_HAL(pKernelGmmu, pPde, version, pPdeApertures) kgmmuFmtInitPde_DISPATCH(pKernelGmmu, pPde, version, pPdeApertures)
#define kgmmuFmtIsVersionSupported_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtIsVersionSupported__
#define kgmmuFmtIsVersionSupported(pKernelGmmu, version) kgmmuFmtIsVersionSupported_DISPATCH(pKernelGmmu, version)
#define kgmmuFmtIsVersionSupported_HAL(pKernelGmmu, version) kgmmuFmtIsVersionSupported_DISPATCH(pKernelGmmu, version)
#define kgmmuFmtInitLevels_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitLevels__
#define kgmmuFmtInitLevels(pKernelGmmu, pLevels, numLevels, version, bigPageShift) kgmmuFmtInitLevels_DISPATCH(pKernelGmmu, pLevels, numLevels, version, bigPageShift)
#define kgmmuFmtInitLevels_HAL(pKernelGmmu, pLevels, numLevels, version, bigPageShift) kgmmuFmtInitLevels_DISPATCH(pKernelGmmu, pLevels, numLevels, version, bigPageShift)
#define kgmmuFmtInitPdeMulti_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtInitPdeMulti__
#define kgmmuFmtInitPdeMulti(pKernelGmmu, pPdeMulti, version, pPdeApertures) kgmmuFmtInitPdeMulti_DISPATCH(pKernelGmmu, pPdeMulti, version, pPdeApertures)
#define kgmmuFmtInitPdeMulti_HAL(pKernelGmmu, pPdeMulti, version, pPdeApertures) kgmmuFmtInitPdeMulti_DISPATCH(pKernelGmmu, pPdeMulti, version, pPdeApertures)
#define kgmmuFmtFamiliesInit_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFmtFamiliesInit__
#define kgmmuFmtFamiliesInit(pGpu, pKernelGmmu) kgmmuFmtFamiliesInit_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFmtFamiliesInit_HAL(pGpu, pKernelGmmu) kgmmuFmtFamiliesInit_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuTranslatePtePcfFromSw_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuTranslatePtePcfFromSw__
#define kgmmuTranslatePtePcfFromSw(pKernelGmmu, arg2, arg3) kgmmuTranslatePtePcfFromSw_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, arg2, arg3) kgmmuTranslatePtePcfFromSw_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuTranslatePtePcfFromHw_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuTranslatePtePcfFromHw__
#define kgmmuTranslatePtePcfFromHw(pKernelGmmu, arg2, arg3, arg4) kgmmuTranslatePtePcfFromHw_DISPATCH(pKernelGmmu, arg2, arg3, arg4)
#define kgmmuTranslatePtePcfFromHw_HAL(pKernelGmmu, arg2, arg3, arg4) kgmmuTranslatePtePcfFromHw_DISPATCH(pKernelGmmu, arg2, arg3, arg4)
#define kgmmuTranslatePdePcfFromSw_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuTranslatePdePcfFromSw__
#define kgmmuTranslatePdePcfFromSw(pKernelGmmu, arg2, arg3) kgmmuTranslatePdePcfFromSw_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, arg2, arg3) kgmmuTranslatePdePcfFromSw_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuTranslatePdePcfFromHw_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuTranslatePdePcfFromHw__
#define kgmmuTranslatePdePcfFromHw(pKernelGmmu, arg2, arg3, arg4) kgmmuTranslatePdePcfFromHw_DISPATCH(pKernelGmmu, arg2, arg3, arg4)
#define kgmmuTranslatePdePcfFromHw_HAL(pKernelGmmu, arg2, arg3, arg4) kgmmuTranslatePdePcfFromHw_DISPATCH(pKernelGmmu, arg2, arg3, arg4)
#define kgmmuGetFaultRegisterMappings_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetFaultRegisterMappings__
#define kgmmuGetFaultRegisterMappings(pGpu, pKernelGmmu, index, pFaultBufferGet, pFaultBufferPut, pFaultBufferInfo, faultIntr, faultIntrSet, faultIntrClear, faultMask, pPrefetchCtrl) kgmmuGetFaultRegisterMappings_DISPATCH(pGpu, pKernelGmmu, index, pFaultBufferGet, pFaultBufferPut, pFaultBufferInfo, faultIntr, faultIntrSet, faultIntrClear, faultMask, pPrefetchCtrl)
#define kgmmuGetFaultRegisterMappings_HAL(pGpu, pKernelGmmu, index, pFaultBufferGet, pFaultBufferPut, pFaultBufferInfo, faultIntr, faultIntrSet, faultIntrClear, faultMask, pPrefetchCtrl) kgmmuGetFaultRegisterMappings_DISPATCH(pGpu, pKernelGmmu, index, pFaultBufferGet, pFaultBufferPut, pFaultBufferInfo, faultIntr, faultIntrSet, faultIntrClear, faultMask, pPrefetchCtrl)
#define kgmmuGetFaultTypeString_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetFaultTypeString__
#define kgmmuGetFaultTypeString(pKernelGmmu, faultType) kgmmuGetFaultTypeString_DISPATCH(pKernelGmmu, faultType)
#define kgmmuGetFaultTypeString_HAL(pKernelGmmu, faultType) kgmmuGetFaultTypeString_DISPATCH(pKernelGmmu, faultType)
#define kgmmuIssueReplayableFaultBufferFlush_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIssueReplayableFaultBufferFlush__
#define kgmmuIssueReplayableFaultBufferFlush(pGpu, pKernelGmmu, bCopyAndFlush) kgmmuIssueReplayableFaultBufferFlush_DISPATCH(pGpu, pKernelGmmu, bCopyAndFlush)
#define kgmmuIssueReplayableFaultBufferFlush_HAL(pGpu, pKernelGmmu, bCopyAndFlush) kgmmuIssueReplayableFaultBufferFlush_DISPATCH(pGpu, pKernelGmmu, bCopyAndFlush)
#define kgmmuToggleFaultOnPrefetch_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuToggleFaultOnPrefetch__
#define kgmmuToggleFaultOnPrefetch(pGpu, pKernelGmmu, bEnable) kgmmuToggleFaultOnPrefetch_DISPATCH(pGpu, pKernelGmmu, bEnable)
#define kgmmuToggleFaultOnPrefetch_HAL(pGpu, pKernelGmmu, bEnable) kgmmuToggleFaultOnPrefetch_DISPATCH(pGpu, pKernelGmmu, bEnable)
#define kgmmuFaultBufferAllocSharedMemory_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferAllocSharedMemory__
#define kgmmuFaultBufferAllocSharedMemory(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferAllocSharedMemory_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuFaultBufferAllocSharedMemory_HAL(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferAllocSharedMemory_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuFaultBufferFreeSharedMemory_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferFreeSharedMemory__
#define kgmmuFaultBufferFreeSharedMemory(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferFreeSharedMemory_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuFaultBufferFreeSharedMemory_HAL(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferFreeSharedMemory_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuSetupWarForBug2720120_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuSetupWarForBug2720120__
#define kgmmuSetupWarForBug2720120(pKernelGmmu) kgmmuSetupWarForBug2720120_DISPATCH(pKernelGmmu)
#define kgmmuSetupWarForBug2720120_HAL(pKernelGmmu) kgmmuSetupWarForBug2720120_DISPATCH(pKernelGmmu)
#define kgmmuGetGraphicsEngineId_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetGraphicsEngineId__
#define kgmmuGetGraphicsEngineId(pKernelGmmu) kgmmuGetGraphicsEngineId_DISPATCH(pKernelGmmu)
#define kgmmuGetGraphicsEngineId_HAL(pKernelGmmu) kgmmuGetGraphicsEngineId_DISPATCH(pKernelGmmu)
#define kgmmuReadShadowBufPutIndex_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuReadShadowBufPutIndex__
#define kgmmuReadShadowBufPutIndex(pGpu, pKernelGmmu, type) kgmmuReadShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, type)
#define kgmmuReadShadowBufPutIndex_HAL(pGpu, pKernelGmmu, type) kgmmuReadShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, type)
#define kgmmuIsFaultEngineBar1_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIsFaultEngineBar1__
#define kgmmuIsFaultEngineBar1(pKernelGmmu, arg2) kgmmuIsFaultEngineBar1_DISPATCH(pKernelGmmu, arg2)
#define kgmmuIsFaultEngineBar1_HAL(pKernelGmmu, arg2) kgmmuIsFaultEngineBar1_DISPATCH(pKernelGmmu, arg2)
#define kgmmuIsFaultEngineBar2_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIsFaultEngineBar2__
#define kgmmuIsFaultEngineBar2(pKernelGmmu, arg2) kgmmuIsFaultEngineBar2_DISPATCH(pKernelGmmu, arg2)
#define kgmmuIsFaultEngineBar2_HAL(pKernelGmmu, arg2) kgmmuIsFaultEngineBar2_DISPATCH(pKernelGmmu, arg2)
#define kgmmuIsFaultEnginePhysical_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIsFaultEnginePhysical__
#define kgmmuIsFaultEnginePhysical(pKernelGmmu, arg2) kgmmuIsFaultEnginePhysical_DISPATCH(pKernelGmmu, arg2)
#define kgmmuIsFaultEnginePhysical_HAL(pKernelGmmu, arg2) kgmmuIsFaultEnginePhysical_DISPATCH(pKernelGmmu, arg2)
#define kgmmuCopyMmuFaults_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuCopyMmuFaults__
#define kgmmuCopyMmuFaults(pGpu, pKernelGmmu, pThreadState, entriesCopied, type, bPollForValidBit) kgmmuCopyMmuFaults_DISPATCH(pGpu, pKernelGmmu, pThreadState, entriesCopied, type, bPollForValidBit)
#define kgmmuCopyMmuFaults_HAL(pGpu, pKernelGmmu, pThreadState, entriesCopied, type, bPollForValidBit) kgmmuCopyMmuFaults_DISPATCH(pGpu, pKernelGmmu, pThreadState, entriesCopied, type, bPollForValidBit)
#define kgmmuParseFaultPacket_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuParseFaultPacket__
#define kgmmuParseFaultPacket(pGpu, pKernelGmmu, pFaultPacket, pParsedFaultEntry) kgmmuParseFaultPacket_DISPATCH(pGpu, pKernelGmmu, pFaultPacket, pParsedFaultEntry)
#define kgmmuParseFaultPacket_HAL(pGpu, pKernelGmmu, pFaultPacket, pParsedFaultEntry) kgmmuParseFaultPacket_DISPATCH(pGpu, pKernelGmmu, pFaultPacket, pParsedFaultEntry)
#define kgmmuFaultBufferClearPackets_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferClearPackets__
#define kgmmuFaultBufferClearPackets(pGpu, pKernelGmmu, pFaultBuffer, beginIdx, numFaultPackets) kgmmuFaultBufferClearPackets_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, beginIdx, numFaultPackets)
#define kgmmuFaultBufferClearPackets_HAL(pGpu, pKernelGmmu, pFaultBuffer, beginIdx, numFaultPackets) kgmmuFaultBufferClearPackets_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, beginIdx, numFaultPackets)
#define kgmmuFaultBufferGetFault_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferGetFault__
#define kgmmuFaultBufferGetFault(pGpu, pKernelGmmu, pFaultBuffer, idx) kgmmuFaultBufferGetFault_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, idx)
#define kgmmuFaultBufferGetFault_HAL(pGpu, pKernelGmmu, pFaultBuffer, idx) kgmmuFaultBufferGetFault_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, idx)
#define kgmmuCopyFaultPacketToClientShadowBuffer_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuCopyFaultPacketToClientShadowBuffer__
#define kgmmuCopyFaultPacketToClientShadowBuffer(pGpu, pKernelGmmu, pFaultBuffer, type, getIndex, shadowBufPutIndex, maxBufferEntries, pThreadState, pFaultsCopied) kgmmuCopyFaultPacketToClientShadowBuffer_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, type, getIndex, shadowBufPutIndex, maxBufferEntries, pThreadState, pFaultsCopied)
#define kgmmuCopyFaultPacketToClientShadowBuffer_HAL(pGpu, pKernelGmmu, pFaultBuffer, type, getIndex, shadowBufPutIndex, maxBufferEntries, pThreadState, pFaultsCopied) kgmmuCopyFaultPacketToClientShadowBuffer_DISPATCH(pGpu, pKernelGmmu, pFaultBuffer, type, getIndex, shadowBufPutIndex, maxBufferEntries, pThreadState, pFaultsCopied)
#define kgmmuIsReplayableShadowFaultBufferFull_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIsReplayableShadowFaultBufferFull__
#define kgmmuIsReplayableShadowFaultBufferFull(pGpu, pKernelGmmu, pClientFaultBuf, shadowBufPutIndex, maxBufferEntries) kgmmuIsReplayableShadowFaultBufferFull_DISPATCH(pGpu, pKernelGmmu, pClientFaultBuf, shadowBufPutIndex, maxBufferEntries)
#define kgmmuIsReplayableShadowFaultBufferFull_HAL(pGpu, pKernelGmmu, pClientFaultBuf, shadowBufPutIndex, maxBufferEntries) kgmmuIsReplayableShadowFaultBufferFull_DISPATCH(pGpu, pKernelGmmu, pClientFaultBuf, shadowBufPutIndex, maxBufferEntries)
#define kgmmuReadClientShadowBufPutIndex_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuReadClientShadowBufPutIndex__
#define kgmmuReadClientShadowBufPutIndex(pGpu, pKernelGmmu, gfid, type) kgmmuReadClientShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, gfid, type)
#define kgmmuReadClientShadowBufPutIndex_HAL(pGpu, pKernelGmmu, gfid, type) kgmmuReadClientShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, gfid, type)
#define kgmmuWriteClientShadowBufPutIndex_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuWriteClientShadowBufPutIndex__
#define kgmmuWriteClientShadowBufPutIndex(pGpu, pKernelGmmu, gfid, type, putIndex) kgmmuWriteClientShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, gfid, type, putIndex)
#define kgmmuWriteClientShadowBufPutIndex_HAL(pGpu, pKernelGmmu, gfid, type, putIndex) kgmmuWriteClientShadowBufPutIndex_DISPATCH(pGpu, pKernelGmmu, gfid, type, putIndex)
#define kgmmuInitCeMmuFaultIdRange_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuInitCeMmuFaultIdRange__
#define kgmmuInitCeMmuFaultIdRange(pGpu, pKernelGmmu) kgmmuInitCeMmuFaultIdRange_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuInitCeMmuFaultIdRange_HAL(pGpu, pKernelGmmu) kgmmuInitCeMmuFaultIdRange_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuGetMinCeEngineId_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetMinCeEngineId__
#define kgmmuGetMinCeEngineId(pKernelGmmu) kgmmuGetMinCeEngineId_DISPATCH(pKernelGmmu)
#define kgmmuGetMinCeEngineId_HAL(pKernelGmmu) kgmmuGetMinCeEngineId_DISPATCH(pKernelGmmu)
#define kgmmuGetMaxCeEngineId_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetMaxCeEngineId__
#define kgmmuGetMaxCeEngineId(pGpu, pKernelGmmu) kgmmuGetMaxCeEngineId_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuGetMaxCeEngineId_HAL(pGpu, pKernelGmmu) kgmmuGetMaxCeEngineId_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultBufferMap_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferMap__
#define kgmmuFaultBufferMap(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferMap_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferMap_HAL(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferMap_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferUnmap_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferUnmap__
#define kgmmuFaultBufferUnmap(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferUnmap_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferUnmap_HAL(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferUnmap_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferInit_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferInit__
#define kgmmuFaultBufferInit(pGpu, pKernelGmmu) kgmmuFaultBufferInit_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultBufferInit_HAL(pGpu, pKernelGmmu) kgmmuFaultBufferInit_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultBufferDestroy_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferDestroy__
#define kgmmuFaultBufferDestroy(pGpu, pKernelGmmu) kgmmuFaultBufferDestroy_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultBufferDestroy_HAL(pGpu, pKernelGmmu) kgmmuFaultBufferDestroy_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultBufferLoad_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferLoad__
#define kgmmuFaultBufferLoad(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferLoad_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferLoad_HAL(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferLoad_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferUnload_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultBufferUnload__
#define kgmmuFaultBufferUnload(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferUnload_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuFaultBufferUnload_HAL(pGpu, pKernelGmmu, index, gfid) kgmmuFaultBufferUnload_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuEnableFaultBuffer_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuEnableFaultBuffer__
#define kgmmuEnableFaultBuffer(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid) kgmmuEnableFaultBuffer_DISPATCH(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid)
#define kgmmuEnableFaultBuffer_HAL(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid) kgmmuEnableFaultBuffer_DISPATCH(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid)
#define kgmmuDisableFaultBuffer_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuDisableFaultBuffer__
#define kgmmuDisableFaultBuffer(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid) kgmmuDisableFaultBuffer_DISPATCH(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid)
#define kgmmuDisableFaultBuffer_HAL(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid) kgmmuDisableFaultBuffer_DISPATCH(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid)
#define kgmmuSetAndGetDefaultFaultBufferSize_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuSetAndGetDefaultFaultBufferSize__
#define kgmmuSetAndGetDefaultFaultBufferSize(pGpu, pKernelGmmu, index, gfid) kgmmuSetAndGetDefaultFaultBufferSize_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuSetAndGetDefaultFaultBufferSize_HAL(pGpu, pKernelGmmu, index, gfid) kgmmuSetAndGetDefaultFaultBufferSize_DISPATCH(pGpu, pKernelGmmu, index, gfid)
#define kgmmuReadMmuFaultInstHiLo_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuReadMmuFaultInstHiLo__
#define kgmmuReadMmuFaultInstHiLo(pGpu, pKernelGmmu, arg3, arg4) kgmmuReadMmuFaultInstHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuReadMmuFaultInstHiLo_HAL(pGpu, pKernelGmmu, arg3, arg4) kgmmuReadMmuFaultInstHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuReadMmuFaultAddrHiLo_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuReadMmuFaultAddrHiLo__
#define kgmmuReadMmuFaultAddrHiLo(pGpu, pKernelGmmu, arg3, arg4) kgmmuReadMmuFaultAddrHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuReadMmuFaultAddrHiLo_HAL(pGpu, pKernelGmmu, arg3, arg4) kgmmuReadMmuFaultAddrHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuReadMmuFaultInfo_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuReadMmuFaultInfo__
#define kgmmuReadMmuFaultInfo(pGpu, pKernelGmmu) kgmmuReadMmuFaultInfo_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuReadMmuFaultInfo_HAL(pGpu, pKernelGmmu) kgmmuReadMmuFaultInfo_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuWriteMmuFaultBufferSize_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuWriteMmuFaultBufferSize__
#define kgmmuWriteMmuFaultBufferSize(pGpu, pKernelGmmu, arg3, arg4, gfid) kgmmuWriteMmuFaultBufferSize_DISPATCH(pGpu, pKernelGmmu, arg3, arg4, gfid)
#define kgmmuWriteMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, arg3, arg4, gfid) kgmmuWriteMmuFaultBufferSize_DISPATCH(pGpu, pKernelGmmu, arg3, arg4, gfid)
#define kgmmuWriteMmuFaultBufferHiLo_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuWriteMmuFaultBufferHiLo__
#define kgmmuWriteMmuFaultBufferHiLo(pGpu, pKernelGmmu, arg3, arg4, arg5, gfid) kgmmuWriteMmuFaultBufferHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4, arg5, gfid)
#define kgmmuWriteMmuFaultBufferHiLo_HAL(pGpu, pKernelGmmu, arg3, arg4, arg5, gfid) kgmmuWriteMmuFaultBufferHiLo_DISPATCH(pGpu, pKernelGmmu, arg3, arg4, arg5, gfid)
#define kgmmuEnableMmuFaultInterrupts_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuEnableMmuFaultInterrupts__
#define kgmmuEnableMmuFaultInterrupts(pGpu, pKernelGmmu, index) kgmmuEnableMmuFaultInterrupts_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuEnableMmuFaultInterrupts_HAL(pGpu, pKernelGmmu, index) kgmmuEnableMmuFaultInterrupts_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuDisableMmuFaultInterrupts_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuDisableMmuFaultInterrupts__
#define kgmmuDisableMmuFaultInterrupts(pGpu, pKernelGmmu, index) kgmmuDisableMmuFaultInterrupts_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuDisableMmuFaultInterrupts_HAL(pGpu, pKernelGmmu, index) kgmmuDisableMmuFaultInterrupts_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuEnableMmuFaultOverflowIntr_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuEnableMmuFaultOverflowIntr__
#define kgmmuEnableMmuFaultOverflowIntr(pGpu, pKernelGmmu, index) kgmmuEnableMmuFaultOverflowIntr_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuEnableMmuFaultOverflowIntr_HAL(pGpu, pKernelGmmu, index) kgmmuEnableMmuFaultOverflowIntr_DISPATCH(pGpu, pKernelGmmu, index)
#define kgmmuSignExtendFaultAddress_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuSignExtendFaultAddress__
#define kgmmuSignExtendFaultAddress(pGpu, pKernelGmmu, pMmuFaultAddress) kgmmuSignExtendFaultAddress_DISPATCH(pGpu, pKernelGmmu, pMmuFaultAddress)
#define kgmmuSignExtendFaultAddress_HAL(pGpu, pKernelGmmu, pMmuFaultAddress) kgmmuSignExtendFaultAddress_DISPATCH(pGpu, pKernelGmmu, pMmuFaultAddress)
#define kgmmuGetFaultType_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetFaultType__
#define kgmmuGetFaultType(pGpu, pKernelGmmu, fault, pMmuFaultType) kgmmuGetFaultType_DISPATCH(pGpu, pKernelGmmu, fault, pMmuFaultType)
#define kgmmuGetFaultType_HAL(pGpu, pKernelGmmu, fault, pMmuFaultType) kgmmuGetFaultType_DISPATCH(pGpu, pKernelGmmu, fault, pMmuFaultType)
#define kgmmuIsP2PUnboundInstFault_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuIsP2PUnboundInstFault__
#define kgmmuIsP2PUnboundInstFault(pKernelGmmu, arg2, arg3) kgmmuIsP2PUnboundInstFault_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuIsP2PUnboundInstFault_HAL(pKernelGmmu, arg2, arg3) kgmmuIsP2PUnboundInstFault_DISPATCH(pKernelGmmu, arg2, arg3)
#define kgmmuServiceVfPriFaults_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceVfPriFaults__
#define kgmmuServiceVfPriFaults(pGpu, pKernelGmmu, faultType) kgmmuServiceVfPriFaults_DISPATCH(pGpu, pKernelGmmu, faultType)
#define kgmmuServiceVfPriFaults_HAL(pGpu, pKernelGmmu, faultType) kgmmuServiceVfPriFaults_DISPATCH(pGpu, pKernelGmmu, faultType)
#define kgmmuTestVidmemAccessBitBufferError_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuTestVidmemAccessBitBufferError__
#define kgmmuTestVidmemAccessBitBufferError(pGpu, pKernelGmmu, arg3) kgmmuTestVidmemAccessBitBufferError_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuTestVidmemAccessBitBufferError_HAL(pGpu, pKernelGmmu, arg3) kgmmuTestVidmemAccessBitBufferError_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuDisableVidmemAccessBitBuf_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuDisableVidmemAccessBitBuf__
#define kgmmuDisableVidmemAccessBitBuf(pGpu, pKernelGmmu) kgmmuDisableVidmemAccessBitBuf_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuDisableVidmemAccessBitBuf_HAL(pGpu, pKernelGmmu) kgmmuDisableVidmemAccessBitBuf_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuEnableVidmemAccessBitBuf_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuEnableVidmemAccessBitBuf__
#define kgmmuEnableVidmemAccessBitBuf(pGpu, pKernelGmmu) kgmmuEnableVidmemAccessBitBuf_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuEnableVidmemAccessBitBuf_HAL(pGpu, pKernelGmmu) kgmmuEnableVidmemAccessBitBuf_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuClearAccessCounterWriteNak_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuClearAccessCounterWriteNak__
#define kgmmuClearAccessCounterWriteNak(pGpu, pKernelGmmu) kgmmuClearAccessCounterWriteNak_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuClearAccessCounterWriteNak_HAL(pGpu, pKernelGmmu) kgmmuClearAccessCounterWriteNak_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuServiceMthdBuffFaultInBar2Fault_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceMthdBuffFaultInBar2Fault__
#define kgmmuServiceMthdBuffFaultInBar2Fault(pGpu, pKernelGmmu) kgmmuServiceMthdBuffFaultInBar2Fault_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuServiceMthdBuffFaultInBar2Fault_HAL(pGpu, pKernelGmmu) kgmmuServiceMthdBuffFaultInBar2Fault_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuFaultCancelTargeted_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultCancelTargeted__
#define kgmmuFaultCancelTargeted(pGpu, pKernelGmmu, arg3) kgmmuFaultCancelTargeted_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuFaultCancelTargeted_HAL(pGpu, pKernelGmmu, arg3) kgmmuFaultCancelTargeted_DISPATCH(pGpu, pKernelGmmu, arg3)
#define kgmmuFaultCancelIssueInvalidate_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuFaultCancelIssueInvalidate__
#define kgmmuFaultCancelIssueInvalidate(pGpu, pKernelGmmu, pCancelInfo, pParams, bGlobal) kgmmuFaultCancelIssueInvalidate_DISPATCH(pGpu, pKernelGmmu, pCancelInfo, pParams, bGlobal)
#define kgmmuFaultCancelIssueInvalidate_HAL(pGpu, pKernelGmmu, pCancelInfo, pParams, bGlobal) kgmmuFaultCancelIssueInvalidate_DISPATCH(pGpu, pKernelGmmu, pCancelInfo, pParams, bGlobal)
#define kgmmuServiceMmuFault_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceMmuFault__
#define kgmmuServiceMmuFault(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData) kgmmuServiceMmuFault_DISPATCH(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData)
#define kgmmuServiceMmuFault_HAL(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData) kgmmuServiceMmuFault_DISPATCH(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData)
#define kgmmuServiceUnboundInstBlockFault_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuServiceUnboundInstBlockFault__
#define kgmmuServiceUnboundInstBlockFault(pGpu, pKernelGmmu, arg3, arg4) kgmmuServiceUnboundInstBlockFault_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuServiceUnboundInstBlockFault_HAL(pGpu, pKernelGmmu, arg3, arg4) kgmmuServiceUnboundInstBlockFault_DISPATCH(pGpu, pKernelGmmu, arg3, arg4)
#define kgmmuGetEccCounts_FNPTR(pKernelGmmu) pKernelGmmu->__kgmmuGetEccCounts__
#define kgmmuGetEccCounts(pGpu, pKernelGmmu) kgmmuGetEccCounts_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuGetEccCounts_HAL(pGpu, pKernelGmmu) kgmmuGetEccCounts_DISPATCH(pGpu, pKernelGmmu)
#define kgmmuCreateFakeSparseTables_FNPTR(arg_this) arg_this->__kgmmuCreateFakeSparseTables__
#define kgmmuCreateFakeSparseTables(arg1, arg_this) kgmmuCreateFakeSparseTables_DISPATCH(arg1, arg_this)
#define kgmmuCreateFakeSparseTables_HAL(arg1, arg_this) kgmmuCreateFakeSparseTables_DISPATCH(arg1, arg_this)
#define kgmmuGetFakeSparseEntry_FNPTR(arg_this) arg_this->__kgmmuGetFakeSparseEntry__
#define kgmmuGetFakeSparseEntry(arg1, arg_this, arg3) kgmmuGetFakeSparseEntry_DISPATCH(arg1, arg_this, arg3)
#define kgmmuGetFakeSparseEntry_HAL(arg1, arg_this, arg3) kgmmuGetFakeSparseEntry_DISPATCH(arg1, arg_this, arg3)
#define kgmmuInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kgmmuInitMissing(pGpu, pEngstate) kgmmuInitMissing_DISPATCH(pGpu, pEngstate)
#define kgmmuStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kgmmuStatePreInitLocked(pGpu, pEngstate) kgmmuStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgmmuStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kgmmuStatePreInitUnlocked(pGpu, pEngstate) kgmmuStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgmmuStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kgmmuStateInitUnlocked(pGpu, pEngstate) kgmmuStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgmmuStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kgmmuStatePreLoad(pGpu, pEngstate, arg3) kgmmuStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgmmuStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kgmmuStatePostUnload(pGpu, pEngstate, arg3) kgmmuStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgmmuIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kgmmuIsPresent(pGpu, pEngstate) kgmmuIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kgmmuConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, ENGDESCRIPTOR arg3) {
    return pKernelGmmu->__kgmmuConstructEngine__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuStateInitLocked__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return pKernelGmmu->__kgmmuStateLoad__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return pKernelGmmu->__kgmmuStateUnload__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuStatePostLoad_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return pKernelGmmu->__kgmmuStatePostLoad__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuStatePreUnload_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return pKernelGmmu->__kgmmuStatePreUnload__(pGpu, pKernelGmmu, arg3);
}

static inline void kgmmuStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    pKernelGmmu->__kgmmuStateDestroy__(pGpu, pKernelGmmu);
}

static inline void kgmmuRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceRecord arg3[175]) {
    pKernelGmmu->__kgmmuRegisterIntrService__(pGpu, pKernelGmmu, arg3);
}

static inline NvBool kgmmuClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceClearInterruptArguments *pParams) {
    return pKernelGmmu->__kgmmuClearInterrupt__(pGpu, pKernelGmmu, pParams);
}

static inline NvU32 kgmmuServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelGmmu->__kgmmuServiceInterrupt__(pGpu, pKernelGmmu, pParams);
}

static inline NV_STATUS kgmmuServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pKernelGmmu->__kgmmuServiceNotificationInterrupt__(pGpu, pKernelGmmu, pParams);
}

static inline NV_STATUS kgmmuInstBlkVaLimitGet_DISPATCH(struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pParams, NvU32 *pOffset, NvU64 *pData) {
    return pKernelGmmu->__kgmmuInstBlkVaLimitGet__(pKernelGmmu, pVAS, subctxId, pParams, pOffset, pData);
}

static inline NV_STATUS kgmmuCommitTlbInvalidate_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams) {
    return pKernelGmmu->__kgmmuCommitTlbInvalidate__(pGpu, pKernelGmmu, pParams);
}

static inline NvU32 kgmmuSetTlbInvalidateMembarWarParameters_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams) {
    return pKernelGmmu->__kgmmuSetTlbInvalidateMembarWarParameters__(pGpu, pKernelGmmu, pParams);
}

static inline NV_STATUS kgmmuSetTlbInvalidationScope_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 flags, TLB_INVALIDATE_PARAMS *pParams) {
    return pKernelGmmu->__kgmmuSetTlbInvalidationScope__(pGpu, pKernelGmmu, flags, pParams);
}

static inline void kgmmuFmtInitPteComptagLine_DISPATCH(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version) {
    pKernelGmmu->__kgmmuFmtInitPteComptagLine__(pKernelGmmu, pPte, version);
}

static inline void kgmmuFmtInitPeerPteFld_DISPATCH(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version) {
    pKernelGmmu->__kgmmuFmtInitPeerPteFld__(pKernelGmmu, pPte, version);
}

static inline void kgmmuFmtInitPte_DISPATCH(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPteApertures, const NvBool bUnifiedAperture) {
    pKernelGmmu->__kgmmuFmtInitPte__(pKernelGmmu, pPte, version, pPteApertures, bUnifiedAperture);
}

static inline void kgmmuFmtInitPde_DISPATCH(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE *pPde, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures) {
    pKernelGmmu->__kgmmuFmtInitPde__(pKernelGmmu, pPde, version, pPdeApertures);
}

static inline NvBool kgmmuFmtIsVersionSupported_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 version) {
    return pKernelGmmu->__kgmmuFmtIsVersionSupported__(pKernelGmmu, version);
}

static inline void kgmmuFmtInitLevels_DISPATCH(struct KernelGmmu *pKernelGmmu, MMU_FMT_LEVEL *pLevels, const NvU32 numLevels, const NvU32 version, const NvU32 bigPageShift) {
    pKernelGmmu->__kgmmuFmtInitLevels__(pKernelGmmu, pLevels, numLevels, version, bigPageShift);
}

static inline void kgmmuFmtInitPdeMulti_DISPATCH(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE_MULTI *pPdeMulti, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures) {
    pKernelGmmu->__kgmmuFmtInitPdeMulti__(pKernelGmmu, pPdeMulti, version, pPdeApertures);
}

static inline NV_STATUS kgmmuFmtFamiliesInit_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuFmtFamiliesInit__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuTranslatePtePcfFromSw_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3) {
    return pKernelGmmu->__kgmmuTranslatePtePcfFromSw__(pKernelGmmu, arg2, arg3);
}

static inline NV_STATUS kgmmuTranslatePtePcfFromHw_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvBool arg3, NvU32 *arg4) {
    return pKernelGmmu->__kgmmuTranslatePtePcfFromHw__(pKernelGmmu, arg2, arg3, arg4);
}

static inline NV_STATUS kgmmuTranslatePdePcfFromSw_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3) {
    return pKernelGmmu->__kgmmuTranslatePdePcfFromSw__(pKernelGmmu, arg2, arg3);
}

static inline NV_STATUS kgmmuTranslatePdePcfFromHw_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2, GMMU_APERTURE arg3, NvU32 *arg4) {
    return pKernelGmmu->__kgmmuTranslatePdePcfFromHw__(pKernelGmmu, arg2, arg3, arg4);
}

static inline NV_STATUS kgmmuGetFaultRegisterMappings_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvP64 *pFaultBufferGet, NvP64 *pFaultBufferPut, NvP64 *pFaultBufferInfo, NvP64 *faultIntr, NvP64 *faultIntrSet, NvP64 *faultIntrClear, NvU32 *faultMask, NvP64 *pPrefetchCtrl) {
    return pKernelGmmu->__kgmmuGetFaultRegisterMappings__(pGpu, pKernelGmmu, index, pFaultBufferGet, pFaultBufferPut, pFaultBufferInfo, faultIntr, faultIntrSet, faultIntrClear, faultMask, pPrefetchCtrl);
}

static inline const char * kgmmuGetFaultTypeString_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 faultType) {
    return pKernelGmmu->__kgmmuGetFaultTypeString__(pKernelGmmu, faultType);
}

static inline NV_STATUS kgmmuIssueReplayableFaultBufferFlush_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bCopyAndFlush) {
    return pKernelGmmu->__kgmmuIssueReplayableFaultBufferFlush__(pGpu, pKernelGmmu, bCopyAndFlush);
}

static inline NV_STATUS kgmmuToggleFaultOnPrefetch_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bEnable) {
    return pKernelGmmu->__kgmmuToggleFaultOnPrefetch__(pGpu, pKernelGmmu, bEnable);
}

static inline NV_STATUS kgmmuFaultBufferAllocSharedMemory_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    return pKernelGmmu->__kgmmuFaultBufferAllocSharedMemory__(pGpu, pKernelGmmu, arg3);
}

static inline void kgmmuFaultBufferFreeSharedMemory_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    pKernelGmmu->__kgmmuFaultBufferFreeSharedMemory__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuSetupWarForBug2720120_DISPATCH(struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuSetupWarForBug2720120__(pKernelGmmu);
}

static inline NvU32 kgmmuGetGraphicsEngineId_DISPATCH(struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuGetGraphicsEngineId__(pKernelGmmu);
}

static inline NvU32 kgmmuReadShadowBufPutIndex_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE type) {
    return pKernelGmmu->__kgmmuReadShadowBufPutIndex__(pGpu, pKernelGmmu, type);
}

static inline NvBool kgmmuIsFaultEngineBar1_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2) {
    return pKernelGmmu->__kgmmuIsFaultEngineBar1__(pKernelGmmu, arg2);
}

static inline NvBool kgmmuIsFaultEngineBar2_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2) {
    return pKernelGmmu->__kgmmuIsFaultEngineBar2__(pKernelGmmu, arg2);
}

static inline NvBool kgmmuIsFaultEnginePhysical_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2) {
    return pKernelGmmu->__kgmmuIsFaultEnginePhysical__(pKernelGmmu, arg2);
}

static inline NV_STATUS kgmmuCopyMmuFaults_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *pThreadState, NvU32 *entriesCopied, FAULT_BUFFER_TYPE type, NvBool bPollForValidBit) {
    return pKernelGmmu->__kgmmuCopyMmuFaults__(pGpu, pKernelGmmu, pThreadState, entriesCopied, type, bPollForValidBit);
}

static inline NV_STATUS kgmmuParseFaultPacket_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pFaultPacket, NvP64 pParsedFaultEntry) {
    return pKernelGmmu->__kgmmuParseFaultPacket__(pGpu, pKernelGmmu, pFaultPacket, pParsedFaultEntry);
}

static inline void kgmmuFaultBufferClearPackets_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 beginIdx, NvU32 numFaultPackets) {
    pKernelGmmu->__kgmmuFaultBufferClearPackets__(pGpu, pKernelGmmu, pFaultBuffer, beginIdx, numFaultPackets);
}

static inline GMMU_FAULT_PACKET * kgmmuFaultBufferGetFault_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 idx) {
    return pKernelGmmu->__kgmmuFaultBufferGetFault__(pGpu, pKernelGmmu, pFaultBuffer, idx);
}

static inline NvU32 kgmmuCopyFaultPacketToClientShadowBuffer_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct GMMU_FAULT_BUFFER *pFaultBuffer, FAULT_BUFFER_TYPE type, NvU32 getIndex, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries, struct THREAD_STATE_NODE *pThreadState, NvU32 *pFaultsCopied) {
    return pKernelGmmu->__kgmmuCopyFaultPacketToClientShadowBuffer__(pGpu, pKernelGmmu, pFaultBuffer, type, getIndex, shadowBufPutIndex, maxBufferEntries, pThreadState, pFaultsCopied);
}

static inline NvBool kgmmuIsReplayableShadowFaultBufferFull_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientFaultBuf, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries) {
    return pKernelGmmu->__kgmmuIsReplayableShadowFaultBufferFull__(pGpu, pKernelGmmu, pClientFaultBuf, shadowBufPutIndex, maxBufferEntries);
}

static inline NvU32 kgmmuReadClientShadowBufPutIndex_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type) {
    return pKernelGmmu->__kgmmuReadClientShadowBufPutIndex__(pGpu, pKernelGmmu, gfid, type);
}

static inline void kgmmuWriteClientShadowBufPutIndex_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type, NvU32 putIndex) {
    pKernelGmmu->__kgmmuWriteClientShadowBufPutIndex__(pGpu, pKernelGmmu, gfid, type, putIndex);
}

static inline NV_STATUS kgmmuInitCeMmuFaultIdRange_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuInitCeMmuFaultIdRange__(pGpu, pKernelGmmu);
}

static inline NvU32 kgmmuGetMinCeEngineId_DISPATCH(struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuGetMinCeEngineId__(pKernelGmmu);
}

static inline NvU32 kgmmuGetMaxCeEngineId_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuGetMaxCeEngineId__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuFaultBufferMap_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return pKernelGmmu->__kgmmuFaultBufferMap__(pGpu, pKernelGmmu, index, gfid);
}

static inline NV_STATUS kgmmuFaultBufferUnmap_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return pKernelGmmu->__kgmmuFaultBufferUnmap__(pGpu, pKernelGmmu, index, gfid);
}

static inline NV_STATUS kgmmuFaultBufferInit_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuFaultBufferInit__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuFaultBufferDestroy_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuFaultBufferDestroy__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuFaultBufferLoad_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return pKernelGmmu->__kgmmuFaultBufferLoad__(pGpu, pKernelGmmu, index, gfid);
}

static inline NV_STATUS kgmmuFaultBufferUnload_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return pKernelGmmu->__kgmmuFaultBufferUnload__(pGpu, pKernelGmmu, index, gfid);
}

static inline NV_STATUS kgmmuEnableFaultBuffer_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid) {
    return pKernelGmmu->__kgmmuEnableFaultBuffer__(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid);
}

static inline NV_STATUS kgmmuDisableFaultBuffer_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid) {
    return pKernelGmmu->__kgmmuDisableFaultBuffer__(pGpu, pKernelGmmu, index, bIsErrorRecovery, gfid);
}

static inline NvU32 kgmmuSetAndGetDefaultFaultBufferSize_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE index, NvU32 gfid) {
    return pKernelGmmu->__kgmmuSetAndGetDefaultFaultBufferSize__(pGpu, pKernelGmmu, index, gfid);
}

static inline void kgmmuReadMmuFaultInstHiLo_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4) {
    pKernelGmmu->__kgmmuReadMmuFaultInstHiLo__(pGpu, pKernelGmmu, arg3, arg4);
}

static inline void kgmmuReadMmuFaultAddrHiLo_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4) {
    pKernelGmmu->__kgmmuReadMmuFaultAddrHiLo__(pGpu, pKernelGmmu, arg3, arg4);
}

static inline NvU32 kgmmuReadMmuFaultInfo_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuReadMmuFaultInfo__(pGpu, pKernelGmmu);
}

static inline void kgmmuWriteMmuFaultBufferSize_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 gfid) {
    pKernelGmmu->__kgmmuWriteMmuFaultBufferSize__(pGpu, pKernelGmmu, arg3, arg4, gfid);
}

static inline void kgmmuWriteMmuFaultBufferHiLo_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 arg5, NvU32 gfid) {
    pKernelGmmu->__kgmmuWriteMmuFaultBufferHiLo__(pGpu, pKernelGmmu, arg3, arg4, arg5, gfid);
}

static inline NV_STATUS kgmmuEnableMmuFaultInterrupts_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return pKernelGmmu->__kgmmuEnableMmuFaultInterrupts__(pGpu, pKernelGmmu, index);
}

static inline NV_STATUS kgmmuDisableMmuFaultInterrupts_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return pKernelGmmu->__kgmmuDisableMmuFaultInterrupts__(pGpu, pKernelGmmu, index);
}

static inline NV_STATUS kgmmuEnableMmuFaultOverflowIntr_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return pKernelGmmu->__kgmmuEnableMmuFaultOverflowIntr__(pGpu, pKernelGmmu, index);
}

static inline void kgmmuSignExtendFaultAddress_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU64 *pMmuFaultAddress) {
    pKernelGmmu->__kgmmuSignExtendFaultAddress__(pGpu, pKernelGmmu, pMmuFaultAddress);
}

static inline NV_STATUS kgmmuGetFaultType_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 fault, FAULT_TYPE *pMmuFaultType) {
    return pKernelGmmu->__kgmmuGetFaultType__(pGpu, pKernelGmmu, fault, pMmuFaultType);
}

static inline NvBool kgmmuIsP2PUnboundInstFault_DISPATCH(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 arg3) {
    return pKernelGmmu->__kgmmuIsP2PUnboundInstFault__(pKernelGmmu, arg2, arg3);
}

static inline NV_STATUS kgmmuServiceVfPriFaults_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 faultType) {
    return pKernelGmmu->__kgmmuServiceVfPriFaults__(pGpu, pKernelGmmu, faultType);
}

static inline NvBool kgmmuTestVidmemAccessBitBufferError_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return pKernelGmmu->__kgmmuTestVidmemAccessBitBufferError__(pGpu, pKernelGmmu, arg3);
}

static inline void kgmmuDisableVidmemAccessBitBuf_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    pKernelGmmu->__kgmmuDisableVidmemAccessBitBuf__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuEnableVidmemAccessBitBuf_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuEnableVidmemAccessBitBuf__(pGpu, pKernelGmmu);
}

static inline void kgmmuClearAccessCounterWriteNak_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    pKernelGmmu->__kgmmuClearAccessCounterWriteNak__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuServiceMthdBuffFaultInBar2Fault_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuServiceMthdBuffFaultInBar2Fault__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuFaultCancelTargeted_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *arg3) {
    return pKernelGmmu->__kgmmuFaultCancelTargeted__(pGpu, pKernelGmmu, arg3);
}

static inline NV_STATUS kgmmuFaultCancelIssueInvalidate_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *pCancelInfo, TLB_INVALIDATE_PARAMS *pParams, NvBool bGlobal) {
    return pKernelGmmu->__kgmmuFaultCancelIssueInvalidate__(pGpu, pKernelGmmu, pCancelInfo, pParams, bGlobal);
}

static inline NV_STATUS kgmmuServiceMmuFault_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pParsedFaultInfo, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData) {
    return pKernelGmmu->__kgmmuServiceMmuFault__(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData);
}

static inline NV_STATUS kgmmuServiceUnboundInstBlockFault_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 arg3, FIFO_MMU_EXCEPTION_DATA *arg4) {
    return pKernelGmmu->__kgmmuServiceUnboundInstBlockFault__(pGpu, pKernelGmmu, arg3, arg4);
}

static inline NvU32 kgmmuGetEccCounts_DISPATCH(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return pKernelGmmu->__kgmmuGetEccCounts__(pGpu, pKernelGmmu);
}

static inline NV_STATUS kgmmuCreateFakeSparseTables_DISPATCH(OBJGPU *arg1, struct KernelGmmu *arg_this) {
    return arg_this->__kgmmuCreateFakeSparseTables__(arg1, arg_this);
}

static inline NvU8 * kgmmuGetFakeSparseEntry_DISPATCH(OBJGPU *arg1, struct KernelGmmu *arg_this, const MMU_FMT_LEVEL *arg3) {
    return arg_this->__kgmmuGetFakeSparseEntry__(arg1, arg_this, arg3);
}

static inline void kgmmuInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    pEngstate->__kgmmuInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgmmuStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return pEngstate->__kgmmuStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgmmuStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return pEngstate->__kgmmuStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgmmuStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return pEngstate->__kgmmuStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgmmuStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3) {
    return pEngstate->__kgmmuStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgmmuStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3) {
    return pEngstate->__kgmmuStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kgmmuIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return pEngstate->__kgmmuIsPresent__(pGpu, pEngstate);
}

static inline NvU32 kgmmuService_4a4dee(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return 0;
}


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuService(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuService(pGpu, pKernelGmmu) kgmmuService_4a4dee(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuService_HAL(pGpu, pKernelGmmu) kgmmuService(pGpu, pKernelGmmu)

NvU64 kgmmuGetMaxBigPageSize_GM107(struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetMaxBigPageSize(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetMaxBigPageSize(pKernelGmmu) kgmmuGetMaxBigPageSize_GM107(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetMaxBigPageSize_HAL(pKernelGmmu) kgmmuGetMaxBigPageSize(pKernelGmmu)

static inline NvU32 kgmmuGetVaspaceClass_f515df(struct KernelGmmu *pKernelGmmu) {
    return (37105);
}


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuGetVaspaceClass(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetVaspaceClass(pKernelGmmu) kgmmuGetVaspaceClass_f515df(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetVaspaceClass_HAL(pKernelGmmu) kgmmuGetVaspaceClass(pKernelGmmu)

NV_STATUS kgmmuInstBlkAtsGet_GV100(struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, NvU32 subctxid, NvU32 *pOffset, NvU32 *pData);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuInstBlkAtsGet(struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, NvU32 subctxid, NvU32 *pOffset, NvU32 *pData) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuInstBlkAtsGet(pKernelGmmu, pVAS, subctxid, pOffset, pData) kgmmuInstBlkAtsGet_GV100(pKernelGmmu, pVAS, subctxid, pOffset, pData)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuInstBlkAtsGet_HAL(pKernelGmmu, pVAS, subctxid, pOffset, pData) kgmmuInstBlkAtsGet(pKernelGmmu, pVAS, subctxid, pOffset, pData)

static inline NV_STATUS kgmmuInstBlkMagicValueGet_46f6a7(struct KernelGmmu *pKernelGmmu, NvU32 *pOffset, NvU32 *pData) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuInstBlkMagicValueGet(struct KernelGmmu *pKernelGmmu, NvU32 *pOffset, NvU32 *pData) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuInstBlkMagicValueGet(pKernelGmmu, pOffset, pData) kgmmuInstBlkMagicValueGet_46f6a7(pKernelGmmu, pOffset, pData)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuInstBlkMagicValueGet_HAL(pKernelGmmu, pOffset, pData) kgmmuInstBlkMagicValueGet(pKernelGmmu, pOffset, pData)

NV_STATUS kgmmuInstBlkPageDirBaseGet_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, INST_BLK_INIT_PARAMS *pParams, NvU32 subctxid, NvU32 *pOffsetLo, NvU32 *pDataLo, NvU32 *pOffsetHi, NvU32 *pDataHi);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuInstBlkPageDirBaseGet(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, INST_BLK_INIT_PARAMS *pParams, NvU32 subctxid, NvU32 *pOffsetLo, NvU32 *pDataLo, NvU32 *pOffsetHi, NvU32 *pDataHi) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuInstBlkPageDirBaseGet(pGpu, pKernelGmmu, pVAS, pParams, subctxid, pOffsetLo, pDataLo, pOffsetHi, pDataHi) kgmmuInstBlkPageDirBaseGet_GV100(pGpu, pKernelGmmu, pVAS, pParams, subctxid, pOffsetLo, pDataLo, pOffsetHi, pDataHi)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuInstBlkPageDirBaseGet_HAL(pGpu, pKernelGmmu, pVAS, pParams, subctxid, pOffsetLo, pDataLo, pOffsetHi, pDataHi) kgmmuInstBlkPageDirBaseGet(pGpu, pKernelGmmu, pVAS, pParams, subctxid, pOffsetLo, pDataLo, pOffsetHi, pDataHi)

NvU32 kgmmuGetPDBAllocSize_GP100(struct KernelGmmu *pKernelGmmu, const MMU_FMT_LEVEL *arg2, NvU64 arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuGetPDBAllocSize(struct KernelGmmu *pKernelGmmu, const MMU_FMT_LEVEL *arg2, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetPDBAllocSize(pKernelGmmu, arg2, arg3) kgmmuGetPDBAllocSize_GP100(pKernelGmmu, arg2, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetPDBAllocSize_HAL(pKernelGmmu, arg2, arg3) kgmmuGetPDBAllocSize(pKernelGmmu, arg2, arg3)

NvU64 kgmmuGetBigPageSize_GM107(struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetBigPageSize(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetBigPageSize(pKernelGmmu) kgmmuGetBigPageSize_GM107(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetBigPageSize_HAL(pKernelGmmu) kgmmuGetBigPageSize(pKernelGmmu)

void kgmmuFmtInitCaps_GM20X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT *pFmt);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuFmtInitCaps(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT *pFmt) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtInitCaps(pKernelGmmu, pFmt) kgmmuFmtInitCaps_GM20X(pKernelGmmu, pFmt)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuFmtInitCaps_HAL(pKernelGmmu, pFmt) kgmmuFmtInitCaps(pKernelGmmu, pFmt)

void kgmmuFmtInitPteApertures_GM10X(struct KernelGmmu *pKernelGmmu, struct NV_FIELD_ENUM_ENTRY *pEntries);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuFmtInitPteApertures(struct KernelGmmu *pKernelGmmu, struct NV_FIELD_ENUM_ENTRY *pEntries) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtInitPteApertures(pKernelGmmu, pEntries) kgmmuFmtInitPteApertures_GM10X(pKernelGmmu, pEntries)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuFmtInitPteApertures_HAL(pKernelGmmu, pEntries) kgmmuFmtInitPteApertures(pKernelGmmu, pEntries)

void kgmmuFmtInitPdeApertures_GM10X(struct KernelGmmu *pKernelGmmu, struct NV_FIELD_ENUM_ENTRY *pEntries);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuFmtInitPdeApertures(struct KernelGmmu *pKernelGmmu, struct NV_FIELD_ENUM_ENTRY *pEntries) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtInitPdeApertures(pKernelGmmu, pEntries) kgmmuFmtInitPdeApertures_GM10X(pKernelGmmu, pEntries)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuFmtInitPdeApertures_HAL(pKernelGmmu, pEntries) kgmmuFmtInitPdeApertures(pKernelGmmu, pEntries)

void kgmmuInvalidateTlb_GM107(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pRootPageDir, NvU32 vaspaceFlags, VAS_PTE_UPDATE_TYPE update_type, NvU32 gfid, NvU32 invalidation_scope);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuInvalidateTlb(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pRootPageDir, NvU32 vaspaceFlags, VAS_PTE_UPDATE_TYPE update_type, NvU32 gfid, NvU32 invalidation_scope) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuInvalidateTlb(pGpu, pKernelGmmu, pRootPageDir, vaspaceFlags, update_type, gfid, invalidation_scope) kgmmuInvalidateTlb_GM107(pGpu, pKernelGmmu, pRootPageDir, vaspaceFlags, update_type, gfid, invalidation_scope)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu, pRootPageDir, vaspaceFlags, update_type, gfid, invalidation_scope) kgmmuInvalidateTlb(pGpu, pKernelGmmu, pRootPageDir, vaspaceFlags, update_type, gfid, invalidation_scope)

NV_STATUS kgmmuCommitInvalidateTlbTest_GM107(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, COMMIT_TLB_INVALIDATE_TEST_PARAMS *pTestParams);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuCommitInvalidateTlbTest(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, COMMIT_TLB_INVALIDATE_TEST_PARAMS *pTestParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuCommitInvalidateTlbTest(pGpu, pKernelGmmu, pTestParams) kgmmuCommitInvalidateTlbTest_GM107(pGpu, pKernelGmmu, pTestParams)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuCommitInvalidateTlbTest_HAL(pGpu, pKernelGmmu, pTestParams) kgmmuCommitInvalidateTlbTest(pGpu, pKernelGmmu, pTestParams)

NV_STATUS kgmmuCheckPendingInvalidates_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, RMTIMEOUT *pTimeOut, NvU32 gfid);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuCheckPendingInvalidates(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, RMTIMEOUT *pTimeOut, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuCheckPendingInvalidates(pGpu, pKernelGmmu, pTimeOut, gfid) kgmmuCheckPendingInvalidates_TU102(pGpu, pKernelGmmu, pTimeOut, gfid)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, pTimeOut, gfid) kgmmuCheckPendingInvalidates(pGpu, pKernelGmmu, pTimeOut, gfid)

void kgmmuSetPdbToInvalidate_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuSetPdbToInvalidate(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuSetPdbToInvalidate(pGpu, pKernelGmmu, pParams) kgmmuSetPdbToInvalidate_TU102(pGpu, pKernelGmmu, pParams)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuSetPdbToInvalidate_HAL(pGpu, pKernelGmmu, pParams) kgmmuSetPdbToInvalidate(pGpu, pKernelGmmu, pParams)

NV_STATUS kgmmuEnableComputePeerAddressing_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 flags);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuEnableComputePeerAddressing(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuEnableComputePeerAddressing(pGpu, pKernelGmmu, flags) kgmmuEnableComputePeerAddressing_IMPL(pGpu, pKernelGmmu, flags)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuEnableComputePeerAddressing_HAL(pGpu, pKernelGmmu, flags) kgmmuEnableComputePeerAddressing(pGpu, pKernelGmmu, flags)

void kgmmuDetermineMaxVASize_GM107(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuDetermineMaxVASize(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuDetermineMaxVASize(pGpu, pKernelGmmu) kgmmuDetermineMaxVASize_GM107(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuDetermineMaxVASize_HAL(pGpu, pKernelGmmu) kgmmuDetermineMaxVASize(pGpu, pKernelGmmu)

NV_STATUS kgmmuChangeReplayableFaultOwnership_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuChangeReplayableFaultOwnership(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuChangeReplayableFaultOwnership(pGpu, pKernelGmmu, arg3) kgmmuChangeReplayableFaultOwnership_GV100(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuChangeReplayableFaultOwnership_HAL(pGpu, pKernelGmmu, arg3) kgmmuChangeReplayableFaultOwnership(pGpu, pKernelGmmu, arg3)

NV_STATUS kgmmuServiceReplayableFault_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuServiceReplayableFault(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuServiceReplayableFault(pGpu, pKernelGmmu) kgmmuServiceReplayableFault_TU102(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuServiceReplayableFault_HAL(pGpu, pKernelGmmu) kgmmuServiceReplayableFault(pGpu, pKernelGmmu)

NV_STATUS kgmmuReportFaultBufferOverflow_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuReportFaultBufferOverflow(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuReportFaultBufferOverflow(pGpu, pKernelGmmu) kgmmuReportFaultBufferOverflow_GV100(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuReportFaultBufferOverflow_HAL(pGpu, pKernelGmmu) kgmmuReportFaultBufferOverflow(pGpu, pKernelGmmu)

NV_STATUS kgmmuReadFaultBufferGetPtr_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 *pGetOffset, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuReadFaultBufferGetPtr(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 *pGetOffset, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuReadFaultBufferGetPtr(pGpu, pKernelGmmu, index, pGetOffset, arg5) kgmmuReadFaultBufferGetPtr_TU102(pGpu, pKernelGmmu, index, pGetOffset, arg5)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuReadFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, index, pGetOffset, arg5) kgmmuReadFaultBufferGetPtr(pGpu, pKernelGmmu, index, pGetOffset, arg5)

NV_STATUS kgmmuWriteFaultBufferGetPtr_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 getValue, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuWriteFaultBufferGetPtr(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 getValue, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuWriteFaultBufferGetPtr(pGpu, pKernelGmmu, index, getValue, arg5) kgmmuWriteFaultBufferGetPtr_TU102(pGpu, pKernelGmmu, index, getValue, arg5)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuWriteFaultBufferGetPtr_HAL(pGpu, pKernelGmmu, index, getValue, arg5) kgmmuWriteFaultBufferGetPtr(pGpu, pKernelGmmu, index, getValue, arg5)

NV_STATUS kgmmuReadFaultBufferPutPtr_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 *pPutOffset, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuReadFaultBufferPutPtr(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 *pPutOffset, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuReadFaultBufferPutPtr(pGpu, pKernelGmmu, index, pPutOffset, arg5) kgmmuReadFaultBufferPutPtr_TU102(pGpu, pKernelGmmu, index, pPutOffset, arg5)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuReadFaultBufferPutPtr_HAL(pGpu, pKernelGmmu, index, pPutOffset, arg5) kgmmuReadFaultBufferPutPtr(pGpu, pKernelGmmu, index, pPutOffset, arg5)

NvU32 kgmmuReadMmuFaultBufferSize_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 gfid);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuReadMmuFaultBufferSize(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuReadMmuFaultBufferSize(pGpu, pKernelGmmu, arg3, gfid) kgmmuReadMmuFaultBufferSize_TU102(pGpu, pKernelGmmu, arg3, gfid)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuReadMmuFaultBufferSize_HAL(pGpu, pKernelGmmu, arg3, gfid) kgmmuReadMmuFaultBufferSize(pGpu, pKernelGmmu, arg3, gfid)

NvU32 kgmmuReadMmuFaultStatus_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuReadMmuFaultStatus(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuReadMmuFaultStatus(pGpu, pKernelGmmu, gfid) kgmmuReadMmuFaultStatus_TU102(pGpu, pKernelGmmu, gfid)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuReadMmuFaultStatus_HAL(pGpu, pKernelGmmu, gfid) kgmmuReadMmuFaultStatus(pGpu, pKernelGmmu, gfid)

void kgmmuWriteMmuFaultStatus_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuWriteMmuFaultStatus(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuWriteMmuFaultStatus(pGpu, pKernelGmmu, arg3) kgmmuWriteMmuFaultStatus_TU102(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuWriteMmuFaultStatus_HAL(pGpu, pKernelGmmu, arg3) kgmmuWriteMmuFaultStatus(pGpu, pKernelGmmu, arg3)

NvBool kgmmuIsNonReplayableFaultPending_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvBool kgmmuIsNonReplayableFaultPending(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuIsNonReplayableFaultPending(pGpu, pKernelGmmu, arg3) kgmmuIsNonReplayableFaultPending_TU102(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuIsNonReplayableFaultPending_HAL(pGpu, pKernelGmmu, arg3) kgmmuIsNonReplayableFaultPending(pGpu, pKernelGmmu, arg3)

NV_STATUS kgmmuClientShadowFaultBufferAlloc_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuClientShadowFaultBufferAlloc(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferAlloc(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferAlloc_GV100(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuClientShadowFaultBufferAlloc_HAL(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferAlloc(pGpu, pKernelGmmu, arg3)

NV_STATUS kgmmuClientShadowFaultBufferFree_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuClientShadowFaultBufferFree(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferFree(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferFree_GV100(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuClientShadowFaultBufferFree_HAL(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferFree(pGpu, pKernelGmmu, arg3)

void kgmmuEncodeSysmemAddrs_GM107(struct KernelGmmu *pKernelGmmu, NvU64 *pAddresses, NvU64 count);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuEncodeSysmemAddrs(struct KernelGmmu *pKernelGmmu, NvU64 *pAddresses, NvU64 count) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuEncodeSysmemAddrs(pKernelGmmu, pAddresses, count) kgmmuEncodeSysmemAddrs_GM107(pKernelGmmu, pAddresses, count)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuEncodeSysmemAddrs_HAL(pKernelGmmu, pAddresses, count) kgmmuEncodeSysmemAddrs(pKernelGmmu, pAddresses, count)

NvU8 kgmmuGetHwPteApertureFromMemdesc_GM107(struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pDesc);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU8 kgmmuGetHwPteApertureFromMemdesc(struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetHwPteApertureFromMemdesc(pKernelGmmu, pDesc) kgmmuGetHwPteApertureFromMemdesc_GM107(pKernelGmmu, pDesc)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetHwPteApertureFromMemdesc_HAL(pKernelGmmu, pDesc) kgmmuGetHwPteApertureFromMemdesc(pKernelGmmu, pDesc)

NvBool kgmmuTestAccessCounterWriteNak_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvBool kgmmuTestAccessCounterWriteNak(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuTestAccessCounterWriteNak(pGpu, pKernelGmmu) kgmmuTestAccessCounterWriteNak_TU102(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuTestAccessCounterWriteNak_HAL(pGpu, pKernelGmmu) kgmmuTestAccessCounterWriteNak(pGpu, pKernelGmmu)

NV_STATUS kgmmuEnableNvlinkComputePeerAddressing_GV100(struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuEnableNvlinkComputePeerAddressing(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuEnableNvlinkComputePeerAddressing(pKernelGmmu) kgmmuEnableNvlinkComputePeerAddressing_GV100(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuEnableNvlinkComputePeerAddressing_HAL(pKernelGmmu) kgmmuEnableNvlinkComputePeerAddressing(pKernelGmmu)

void kgmmuClearNonReplayableFaultIntr_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuClearNonReplayableFaultIntr(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClearNonReplayableFaultIntr(pGpu, pKernelGmmu, arg3) kgmmuClearNonReplayableFaultIntr_TU102(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuClearNonReplayableFaultIntr_HAL(pGpu, pKernelGmmu, arg3) kgmmuClearNonReplayableFaultIntr(pGpu, pKernelGmmu, arg3)

void kgmmuClearReplayableFaultIntr_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuClearReplayableFaultIntr(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClearReplayableFaultIntr(pGpu, pKernelGmmu, arg3) kgmmuClearReplayableFaultIntr_TU102(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuClearReplayableFaultIntr_HAL(pGpu, pKernelGmmu, arg3) kgmmuClearReplayableFaultIntr(pGpu, pKernelGmmu, arg3)

void kgmmuPrintFaultInfo_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, FIFO_MMU_EXCEPTION_DATA *arg4);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuPrintFaultInfo(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, FIFO_MMU_EXCEPTION_DATA *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuPrintFaultInfo(pGpu, pKernelGmmu, arg3, arg4) kgmmuPrintFaultInfo_TU102(pGpu, pKernelGmmu, arg3, arg4)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuPrintFaultInfo_HAL(pGpu, pKernelGmmu, arg3, arg4) kgmmuPrintFaultInfo(pGpu, pKernelGmmu, arg3, arg4)

NV_STATUS kgmmuServiceNonReplayableFault_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuServiceNonReplayableFault(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuServiceNonReplayableFault(pGpu, pKernelGmmu) kgmmuServiceNonReplayableFault_GV100(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuServiceNonReplayableFault_HAL(pGpu, pKernelGmmu) kgmmuServiceNonReplayableFault(pGpu, pKernelGmmu)

NV_STATUS kgmmuHandleNonReplayableFaultPacket_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_PACKET *arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuHandleNonReplayableFaultPacket(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_PACKET *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuHandleNonReplayableFaultPacket(pGpu, pKernelGmmu, arg3) kgmmuHandleNonReplayableFaultPacket_GV100(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuHandleNonReplayableFaultPacket_HAL(pGpu, pKernelGmmu, arg3) kgmmuHandleNonReplayableFaultPacket(pGpu, pKernelGmmu, arg3)

NV_STATUS kgmmuNotifyNonReplayableFault_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuNotifyNonReplayableFault(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuNotifyNonReplayableFault(pGpu, pKernelGmmu, arg3) kgmmuNotifyNonReplayableFault_GV100(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuNotifyNonReplayableFault_HAL(pGpu, pKernelGmmu, arg3) kgmmuNotifyNonReplayableFault(pGpu, pKernelGmmu, arg3)

NvU32 kgmmuGetFaultInfoFromFaultPckt_GV100(struct KernelGmmu *pKernelGmmu, MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuGetFaultInfoFromFaultPckt(struct KernelGmmu *pKernelGmmu, MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetFaultInfoFromFaultPckt(pKernelGmmu, pParsedFaultEntry) kgmmuGetFaultInfoFromFaultPckt_GV100(pKernelGmmu, pParsedFaultEntry)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuGetFaultInfoFromFaultPckt_HAL(pKernelGmmu, pParsedFaultEntry) kgmmuGetFaultInfoFromFaultPckt(pKernelGmmu, pParsedFaultEntry)

static inline NV_STATUS kgmmuServiceChannelMmuFault_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuServiceChannelMmuFault_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuServiceChannelMmuFault(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuServiceChannelMmuFault(pGpu, pKernelGmmu, pParsedFaultEntry, pMmuExceptionData, pKernelChannel) kgmmuServiceChannelMmuFault_92bfc3(pGpu, pKernelGmmu, pParsedFaultEntry, pMmuExceptionData, pKernelChannel)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuServiceChannelMmuFault_HAL(pGpu, pKernelGmmu, pParsedFaultEntry, pMmuExceptionData, pKernelChannel) kgmmuServiceChannelMmuFault(pGpu, pKernelGmmu, pParsedFaultEntry, pMmuExceptionData, pKernelChannel)

NV_STATUS kgmmuServicePriFaults_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuServicePriFaults(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuServicePriFaults(pGpu, pKernelGmmu) kgmmuServicePriFaults_GV100(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuServicePriFaults_HAL(pGpu, pKernelGmmu) kgmmuServicePriFaults(pGpu, pKernelGmmu)

NV_STATUS kgmmuCheckAndDecideBigPageSize_GP100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuCheckAndDecideBigPageSize(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuCheckAndDecideBigPageSize(pGpu, pKernelGmmu) kgmmuCheckAndDecideBigPageSize_GP100(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuCheckAndDecideBigPageSize_HAL(pGpu, pKernelGmmu) kgmmuCheckAndDecideBigPageSize(pGpu, pKernelGmmu)

NV_STATUS kgmmuCreateFakeSparseTablesInternal_KERNEL(OBJGPU *arg1, struct KernelGmmu *arg2);


#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuCreateFakeSparseTablesInternal(OBJGPU *arg1, struct KernelGmmu *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuCreateFakeSparseTablesInternal(arg1, arg2) kgmmuCreateFakeSparseTablesInternal_KERNEL(arg1, arg2)
#endif //__nvoc_kern_gmmu_h_disabled

#define kgmmuCreateFakeSparseTablesInternal_HAL(arg1, arg2) kgmmuCreateFakeSparseTablesInternal(arg1, arg2)

NV_STATUS kgmmuConstructEngine_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, ENGDESCRIPTOR arg3);

NV_STATUS kgmmuStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NV_STATUS kgmmuStateLoad_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

NV_STATUS kgmmuStateUnload_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

NV_STATUS kgmmuStatePostLoad_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

NV_STATUS kgmmuStatePreUnload_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

void kgmmuStateDestroy_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

void kgmmuRegisterIntrService_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceRecord arg3[175]);

NvBool kgmmuClearInterrupt_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceClearInterruptArguments *pParams);

NvU32 kgmmuServiceInterrupt_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams);

static inline NV_STATUS kgmmuServiceNotificationInterrupt_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return NV_OK;
}

NV_STATUS kgmmuInstBlkVaLimitGet_GV100(struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pParams, NvU32 *pOffset, NvU64 *pData);

static inline NV_STATUS kgmmuInstBlkVaLimitGet_f03539(struct KernelGmmu *pKernelGmmu, struct OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pParams, NvU32 *pOffset, NvU64 *pData) {
    *pOffset = 0;
    return NV_OK;
}

NV_STATUS kgmmuCommitTlbInvalidate_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams);

NV_STATUS kgmmuCommitTlbInvalidate_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams);

NvU32 kgmmuSetTlbInvalidateMembarWarParameters_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams);

static inline NvU32 kgmmuSetTlbInvalidateMembarWarParameters_4a4dee(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, TLB_INVALIDATE_PARAMS *pParams) {
    return 0;
}

NV_STATUS kgmmuSetTlbInvalidationScope_GA100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 flags, TLB_INVALIDATE_PARAMS *pParams);

static inline NV_STATUS kgmmuSetTlbInvalidationScope_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 flags, TLB_INVALIDATE_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

void kgmmuFmtInitPteComptagLine_TU10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version);

static inline void kgmmuFmtInitPteComptagLine_b3696a(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version) {
    return;
}

void kgmmuFmtInitPeerPteFld_TU10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version);

static inline void kgmmuFmtInitPeerPteFld_b3696a(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version) {
    return;
}

void kgmmuFmtInitPte_GP10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPteApertures, const NvBool bUnifiedAperture);

void kgmmuFmtInitPte_GH10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PTE *pPte, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPteApertures, const NvBool bUnifiedAperture);

void kgmmuFmtInitPde_GP10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE *pPde, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures);

void kgmmuFmtInitPde_GH10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE *pPde, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures);

NvBool kgmmuFmtIsVersionSupported_GP10X(struct KernelGmmu *pKernelGmmu, NvU32 version);

NvBool kgmmuFmtIsVersionSupported_GH10X(struct KernelGmmu *pKernelGmmu, NvU32 version);

void kgmmuFmtInitLevels_GP10X(struct KernelGmmu *pKernelGmmu, MMU_FMT_LEVEL *pLevels, const NvU32 numLevels, const NvU32 version, const NvU32 bigPageShift);

void kgmmuFmtInitLevels_GA10X(struct KernelGmmu *pKernelGmmu, MMU_FMT_LEVEL *pLevels, const NvU32 numLevels, const NvU32 version, const NvU32 bigPageShift);

void kgmmuFmtInitLevels_GH10X(struct KernelGmmu *pKernelGmmu, MMU_FMT_LEVEL *pLevels, const NvU32 numLevels, const NvU32 version, const NvU32 bigPageShift);

void kgmmuFmtInitLevels_GB10X(struct KernelGmmu *pKernelGmmu, MMU_FMT_LEVEL *pLevels, const NvU32 numLevels, const NvU32 version, const NvU32 bigPageShift);

void kgmmuFmtInitPdeMulti_GP10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE_MULTI *pPdeMulti, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures);

void kgmmuFmtInitPdeMulti_GH10X(struct KernelGmmu *pKernelGmmu, struct GMMU_FMT_PDE_MULTI *pPdeMulti, const NvU32 version, const struct NV_FIELD_ENUM_ENTRY *pPdeApertures);

NV_STATUS kgmmuFmtFamiliesInit_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NV_STATUS kgmmuFmtFamiliesInit_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NV_STATUS kgmmuTranslatePtePcfFromSw_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3);

static inline NV_STATUS kgmmuTranslatePtePcfFromSw_56cd7a(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3) {
    return NV_OK;
}

NV_STATUS kgmmuTranslatePtePcfFromHw_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvBool arg3, NvU32 *arg4);

static inline NV_STATUS kgmmuTranslatePtePcfFromHw_56cd7a(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvBool arg3, NvU32 *arg4) {
    return NV_OK;
}

NV_STATUS kgmmuTranslatePdePcfFromSw_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3);

static inline NV_STATUS kgmmuTranslatePdePcfFromSw_56cd7a(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 *arg3) {
    return NV_OK;
}

NV_STATUS kgmmuTranslatePdePcfFromHw_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2, GMMU_APERTURE arg3, NvU32 *arg4);

static inline NV_STATUS kgmmuTranslatePdePcfFromHw_56cd7a(struct KernelGmmu *pKernelGmmu, NvU32 arg2, GMMU_APERTURE arg3, NvU32 *arg4) {
    return NV_OK;
}

NV_STATUS kgmmuGetFaultRegisterMappings_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvP64 *pFaultBufferGet, NvP64 *pFaultBufferPut, NvP64 *pFaultBufferInfo, NvP64 *faultIntr, NvP64 *faultIntrSet, NvP64 *faultIntrClear, NvU32 *faultMask, NvP64 *pPrefetchCtrl);

NV_STATUS kgmmuGetFaultRegisterMappings_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvP64 *pFaultBufferGet, NvP64 *pFaultBufferPut, NvP64 *pFaultBufferInfo, NvP64 *faultIntr, NvP64 *faultIntrSet, NvP64 *faultIntrClear, NvU32 *faultMask, NvP64 *pPrefetchCtrl);

const char *kgmmuGetFaultTypeString_GP100(struct KernelGmmu *pKernelGmmu, NvU32 faultType);

const char *kgmmuGetFaultTypeString_GB100(struct KernelGmmu *pKernelGmmu, NvU32 faultType);

NV_STATUS kgmmuIssueReplayableFaultBufferFlush_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bCopyAndFlush);

static inline NV_STATUS kgmmuIssueReplayableFaultBufferFlush_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bCopyAndFlush) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuToggleFaultOnPrefetch_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bEnable);

static inline NV_STATUS kgmmuToggleFaultOnPrefetch_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool bEnable) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultBufferAllocSharedMemory_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

static inline NV_STATUS kgmmuFaultBufferAllocSharedMemory_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    return NV_OK;
}

void kgmmuFaultBufferFreeSharedMemory_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

static inline void kgmmuFaultBufferFreeSharedMemory_b3696a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    return;
}

NV_STATUS kgmmuSetupWarForBug2720120_GA100(struct KernelGmmu *pKernelGmmu);

static inline NV_STATUS kgmmuSetupWarForBug2720120_56cd7a(struct KernelGmmu *pKernelGmmu) {
    return NV_OK;
}

NvU32 kgmmuGetGraphicsEngineId_GV100(struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetGraphicsEngineId_GH100(struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuReadShadowBufPutIndex_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE type);

static inline NvU32 kgmmuReadShadowBufPutIndex_4a4dee(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE type) {
    return 0;
}

NvBool kgmmuIsFaultEngineBar1_TU102(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

NvBool kgmmuIsFaultEngineBar1_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

NvBool kgmmuIsFaultEngineBar2_TU102(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

NvBool kgmmuIsFaultEngineBar2_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

NvBool kgmmuIsFaultEnginePhysical_GV100(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

NvBool kgmmuIsFaultEnginePhysical_GH100(struct KernelGmmu *pKernelGmmu, NvU32 arg2);

static inline NV_STATUS kgmmuCopyMmuFaults_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *pThreadState, NvU32 *entriesCopied, FAULT_BUFFER_TYPE type, NvBool bPollForValidBit) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuCopyMmuFaults_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct THREAD_STATE_NODE *pThreadState, NvU32 *entriesCopied, FAULT_BUFFER_TYPE type, NvBool bPollForValidBit);

static inline NV_STATUS kgmmuParseFaultPacket_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pFaultPacket, NvP64 pParsedFaultEntry) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuParseFaultPacket_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pFaultPacket, NvP64 pParsedFaultEntry);

static inline void kgmmuFaultBufferClearPackets_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 beginIdx, NvU32 numFaultPackets) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuFaultBufferClearPackets_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 beginIdx, NvU32 numFaultPackets);

static inline GMMU_FAULT_PACKET *kgmmuFaultBufferGetFault_dc3e6c(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 idx) {
    NV_ASSERT_PRECOMP(0);
    return ((void *)0);
}

GMMU_FAULT_PACKET *kgmmuFaultBufferGetFault_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct HW_FAULT_BUFFER *pFaultBuffer, NvU32 idx);

static inline NvU32 kgmmuCopyFaultPacketToClientShadowBuffer_13cd8d(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct GMMU_FAULT_BUFFER *pFaultBuffer, FAULT_BUFFER_TYPE type, NvU32 getIndex, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries, struct THREAD_STATE_NODE *pThreadState, NvU32 *pFaultsCopied) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

NvU32 kgmmuCopyFaultPacketToClientShadowBuffer_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct GMMU_FAULT_BUFFER *pFaultBuffer, FAULT_BUFFER_TYPE type, NvU32 getIndex, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries, struct THREAD_STATE_NODE *pThreadState, NvU32 *pFaultsCopied);

NvU32 kgmmuCopyFaultPacketToClientShadowBuffer_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, struct GMMU_FAULT_BUFFER *pFaultBuffer, FAULT_BUFFER_TYPE type, NvU32 getIndex, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries, struct THREAD_STATE_NODE *pThreadState, NvU32 *pFaultsCopied);

static inline NvBool kgmmuIsReplayableShadowFaultBufferFull_ceaee8(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientFaultBuf, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

NvBool kgmmuIsReplayableShadowFaultBufferFull_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientFaultBuf, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries);

static inline NvBool kgmmuIsReplayableShadowFaultBufferFull_491d52(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientFaultBuf, NvU32 shadowBufPutIndex, NvU32 maxBufferEntries) {
    return ((NvBool)(0 != 0));
}

static inline NvU32 kgmmuReadClientShadowBufPutIndex_13cd8d(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

NvU32 kgmmuReadClientShadowBufPutIndex_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type);

NvU32 kgmmuReadClientShadowBufPutIndex_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type);

static inline NvU32 kgmmuReadClientShadowBufPutIndex_4a4dee(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type) {
    return 0;
}

static inline void kgmmuWriteClientShadowBufPutIndex_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type, NvU32 putIndex) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuWriteClientShadowBufPutIndex_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type, NvU32 putIndex);

void kgmmuWriteClientShadowBufPutIndex_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type, NvU32 putIndex);

static inline void kgmmuWriteClientShadowBufPutIndex_b3696a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 gfid, FAULT_BUFFER_TYPE type, NvU32 putIndex) {
    return;
}

NV_STATUS kgmmuInitCeMmuFaultIdRange_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline NV_STATUS kgmmuInitCeMmuFaultIdRange_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return NV_OK;
}

NvU32 kgmmuGetMinCeEngineId_GV100(struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMinCeEngineId_GH100(struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMinCeEngineId_GB100(struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMaxCeEngineId_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMaxCeEngineId_GA100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMaxCeEngineId_AD102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMaxCeEngineId_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

NvU32 kgmmuGetMaxCeEngineId_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline NV_STATUS kgmmuFaultBufferMap_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultBufferMap_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid);

static inline NV_STATUS kgmmuFaultBufferUnmap_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultBufferUnmap_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid);

static inline NV_STATUS kgmmuFaultBufferInit_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return NV_OK;
}

NV_STATUS kgmmuFaultBufferInit_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline NV_STATUS kgmmuFaultBufferDestroy_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return NV_OK;
}

NV_STATUS kgmmuFaultBufferDestroy_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline NV_STATUS kgmmuFaultBufferLoad_ac1694(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return NV_OK;
}

NV_STATUS kgmmuFaultBufferLoad_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid);

static inline NV_STATUS kgmmuFaultBufferUnload_ac1694(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid) {
    return NV_OK;
}

NV_STATUS kgmmuFaultBufferUnload_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvU32 gfid);

static inline NV_STATUS kgmmuEnableFaultBuffer_395e98(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuEnableFaultBuffer_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid);

static inline NV_STATUS kgmmuDisableFaultBuffer_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuDisableFaultBuffer_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index, NvBool bIsErrorRecovery, NvU32 gfid);

static inline NvU32 kgmmuSetAndGetDefaultFaultBufferSize_13cd8d(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE index, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

NvU32 kgmmuSetAndGetDefaultFaultBufferSize_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE index, NvU32 gfid);

static inline void kgmmuReadMmuFaultInstHiLo_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuReadMmuFaultInstHiLo_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4);

static inline void kgmmuReadMmuFaultAddrHiLo_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuReadMmuFaultAddrHiLo_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 *arg3, NvU32 *arg4);

static inline NvU32 kgmmuReadMmuFaultInfo_a547a8(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_PRECOMP(0);
    return -1;
}

NvU32 kgmmuReadMmuFaultInfo_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline void kgmmuWriteMmuFaultBufferSize_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuWriteMmuFaultBufferSize_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 gfid);

static inline void kgmmuWriteMmuFaultBufferHiLo_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 arg5, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuWriteMmuFaultBufferHiLo_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU32 arg5, NvU32 gfid);

static inline NV_STATUS kgmmuEnableMmuFaultInterrupts_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuEnableMmuFaultInterrupts_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuDisableMmuFaultInterrupts_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuDisableMmuFaultInterrupts_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuEnableMmuFaultOverflowIntr_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuEnableMmuFaultOverflowIntr_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 index) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void kgmmuSignExtendFaultAddress_f2d351(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU64 *pMmuFaultAddress) {
    NV_ASSERT_PRECOMP(0);
}

void kgmmuSignExtendFaultAddress_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU64 *pMmuFaultAddress);

void kgmmuSignExtendFaultAddress_GH100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU64 *pMmuFaultAddress);

static inline NV_STATUS kgmmuGetFaultType_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 fault, FAULT_TYPE *pMmuFaultType) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuGetFaultType_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 fault, FAULT_TYPE *pMmuFaultType);

NV_STATUS kgmmuGetFaultType_GB100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 fault, FAULT_TYPE *pMmuFaultType);

static inline NvBool kgmmuIsP2PUnboundInstFault_92bfc3(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NvBool kgmmuIsP2PUnboundInstFault_GA100(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 arg3);

static inline NvBool kgmmuIsP2PUnboundInstFault_491d52(struct KernelGmmu *pKernelGmmu, NvU32 arg2, NvU32 arg3) {
    return ((NvBool)(0 != 0));
}

NV_STATUS kgmmuServiceVfPriFaults_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 faultType);

static inline NV_STATUS kgmmuServiceVfPriFaults_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 faultType) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool kgmmuTestVidmemAccessBitBufferError_491d52(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kgmmuTestVidmemAccessBitBufferError_ceaee8(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

static inline void kgmmuDisableVidmemAccessBitBuf_b3696a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return;
}

static inline void kgmmuDisableVidmemAccessBitBuf_e426af(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline NV_STATUS kgmmuEnableVidmemAccessBitBuf_46f6a7(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgmmuEnableVidmemAccessBitBuf_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline void kgmmuClearAccessCounterWriteNak_b3696a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return;
}

static inline void kgmmuClearAccessCounterWriteNak_e426af(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline NV_STATUS kgmmuServiceMthdBuffFaultInBar2Fault_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return NV_OK;
}

static inline NV_STATUS kgmmuServiceMthdBuffFaultInBar2Fault_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultCancelTargeted_VF(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *arg3);

static inline NV_STATUS kgmmuFaultCancelTargeted_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *arg3) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultCancelTargeted_GP100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *arg3);

static inline NV_STATUS kgmmuFaultCancelIssueInvalidate_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *pCancelInfo, TLB_INVALIDATE_PARAMS *pParams, NvBool bGlobal) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgmmuFaultCancelIssueInvalidate_GP100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, GMMU_FAULT_CANCEL_INFO *pCancelInfo, TLB_INVALIDATE_PARAMS *pParams, NvBool bGlobal);

NV_STATUS kgmmuServiceMmuFault_GV100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pParsedFaultInfo, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData);

NV_STATUS kgmmuServiceMmuFault_GA100(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 pParsedFaultInfo, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData);

static inline NV_STATUS kgmmuServiceUnboundInstBlockFault_56cd7a(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 arg3, FIFO_MMU_EXCEPTION_DATA *arg4) {
    return NV_OK;
}

static inline NV_STATUS kgmmuServiceUnboundInstBlockFault_92bfc3(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvP64 arg3, FIFO_MMU_EXCEPTION_DATA *arg4) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 kgmmuGetEccCounts_TU102(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

static inline NvU32 kgmmuGetEccCounts_4a4dee(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    return 0;
}

NV_STATUS kgmmuCreateFakeSparseTables_GH100(OBJGPU *arg1, struct KernelGmmu *arg2);

static inline NV_STATUS kgmmuCreateFakeSparseTables_56cd7a(OBJGPU *arg1, struct KernelGmmu *arg2) {
    return NV_OK;
}

NvU8 *kgmmuGetFakeSparseEntry_GH100(OBJGPU *arg1, struct KernelGmmu *arg2, const MMU_FMT_LEVEL *arg3);

static inline NvU8 *kgmmuGetFakeSparseEntry_fa6e19(OBJGPU *arg1, struct KernelGmmu *arg2, const MMU_FMT_LEVEL *arg3) {
    return ((void *)0);
}

static inline NvU32 kgmmuGetPDEAperture(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PDEAperture;
}

static inline NvU32 kgmmuGetPTEAperture(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PTEAperture;
}

static inline NvU32 kgmmuGetPDEBAR1Aperture(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PDEBAR1Aperture;
}

static inline NvU32 kgmmuGetPTEBAR1Aperture(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PTEBAR1Aperture;
}

static inline NvU32 kgmmuGetPDEBAR1Attr(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PDEBAR1Attr;
}

static inline NvU32 kgmmuGetPTEBAR1Attr(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PTEBAR1Attr;
}

static inline NvU32 kgmmuGetPDEAttr(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PDEAttr;
}

static inline NvU32 kgmmuGetPTEAttr(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->PTEAttr;
}

static inline NvU64 kgmmuGetBigPageSizeOverride(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->overrideBigPageSize;
}

static inline void kgmmuSetBigPageSizeOverride(struct KernelGmmu *pKernelGmmu, NvU64 bigPageSize) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    pKernelGmmu_PRIVATE->overrideBigPageSize = bigPageSize;
}

static inline NvBool kgmmuIsPerVaspaceBigPageEn(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bEnablePerVaspaceBigPage;
}

static inline NvBool kgmmuIsIgnoreHubTlbInvalidate(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bIgnoreHubTlbInvalidate;
}

static inline NvBool kgmmuIsHugePageSupported(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bHugePageSupported;
}

static inline NvBool kgmmuIsPageSize512mbSupported(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bPageSize512mbSupported;
}

static inline NvBool kgmmuIsPageSize256gbSupported(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bPageSize256gbSupported;
}

static inline NvBool kgmmuIsBug2720120WarEnabled(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bBug2720120WarEnabled;
}

static inline NvBool kgmmuIsVaspaceInteropSupported(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->bVaspaceInteropSupported;
}

static inline NvU64 kgmmuGetMaxVASize(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->maxVASize;
}

static inline NvU64 kgmmuGetSysBaseAddress(struct KernelGmmu *pKernelGmmu) {
    struct KernelGmmu_PRIVATE *pKernelGmmu_PRIVATE = (struct KernelGmmu_PRIVATE *)pKernelGmmu;
    return pKernelGmmu_PRIVATE->sysmemBaseAddress;
}

void kgmmuDestruct_IMPL(struct KernelGmmu *pKernelGmmu);

#define __nvoc_kgmmuDestruct(pKernelGmmu) kgmmuDestruct_IMPL(pKernelGmmu)
NV_STATUS kgmmuFmtInit_IMPL(struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFmtInit(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtInit(pKernelGmmu) kgmmuFmtInit_IMPL(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

GMMU_APERTURE kgmmuGetMemAperture_IMPL(struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pMemDesc);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline GMMU_APERTURE kgmmuGetMemAperture(struct KernelGmmu *pKernelGmmu, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    GMMU_APERTURE ret;
    portMemSet(&ret, 0, sizeof(GMMU_APERTURE));
    return ret;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetMemAperture(pKernelGmmu, pMemDesc) kgmmuGetMemAperture_IMPL(pKernelGmmu, pMemDesc)
#endif //__nvoc_kern_gmmu_h_disabled

const GMMU_FMT_FAMILY *kgmmuFmtGetFamily_IMPL(struct KernelGmmu *pKernelGmmu, NvU32 version);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline const GMMU_FMT_FAMILY *kgmmuFmtGetFamily(struct KernelGmmu *pKernelGmmu, NvU32 version) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtGetFamily(pKernelGmmu, version) kgmmuFmtGetFamily_IMPL(pKernelGmmu, version)
#endif //__nvoc_kern_gmmu_h_disabled

const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *kgmmuGetStaticInfo_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *kgmmuGetStaticInfo(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetStaticInfo(pGpu, pKernelGmmu) kgmmuGetStaticInfo_IMPL(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

const struct GMMU_FMT *kgmmuFmtGet_IMPL(struct KernelGmmu *pKernelGmmu, NvU32 version, NvU64 bigPageSize);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline const struct GMMU_FMT *kgmmuFmtGet(struct KernelGmmu *pKernelGmmu, NvU32 version, NvU64 bigPageSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtGet(pKernelGmmu, version, bigPageSize) kgmmuFmtGet_IMPL(pKernelGmmu, version, bigPageSize)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuExtractPteInfo_IMPL(struct KernelGmmu *pKernelGmmu, union GMMU_ENTRY_VALUE *arg2, NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK *arg3, const struct GMMU_FMT *arg4, const MMU_FMT_LEVEL *arg5);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuExtractPteInfo(struct KernelGmmu *pKernelGmmu, union GMMU_ENTRY_VALUE *arg2, NV0080_CTRL_DMA_PTE_INFO_PTE_BLOCK *arg3, const struct GMMU_FMT *arg4, const MMU_FMT_LEVEL *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuExtractPteInfo(pKernelGmmu, arg2, arg3, arg4, arg5) kgmmuExtractPteInfo_IMPL(pKernelGmmu, arg2, arg3, arg4, arg5)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuFieldSetKindCompTags_IMPL(struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *pFmt, const MMU_FMT_LEVEL *pLevel, const COMPR_INFO *pCompr, NvU64 physAddr, NvU64 surfOffset, NvU32 pteIndex, NvU8 *pEntries);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuFieldSetKindCompTags(struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *pFmt, const MMU_FMT_LEVEL *pLevel, const COMPR_INFO *pCompr, NvU64 physAddr, NvU64 surfOffset, NvU32 pteIndex, NvU8 *pEntries) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFieldSetKindCompTags(pKernelGmmu, pFmt, pLevel, pCompr, physAddr, surfOffset, pteIndex, pEntries) kgmmuFieldSetKindCompTags_IMPL(pKernelGmmu, pFmt, pLevel, pCompr, physAddr, surfOffset, pteIndex, pEntries)
#endif //__nvoc_kern_gmmu_h_disabled

NvBool kgmmuFmtIsBigPageSizeSupported_IMPL(struct KernelGmmu *pKernelGmmu, NvU64 bigPageSize);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvBool kgmmuFmtIsBigPageSizeSupported(struct KernelGmmu *pKernelGmmu, NvU64 bigPageSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtIsBigPageSizeSupported(pKernelGmmu, bigPageSize) kgmmuFmtIsBigPageSizeSupported_IMPL(pKernelGmmu, bigPageSize)
#endif //__nvoc_kern_gmmu_h_disabled

const struct GMMU_FMT *kgmmuFmtGetLatestSupportedFormat_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline const struct GMMU_FMT *kgmmuFmtGetLatestSupportedFormat(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFmtGetLatestSupportedFormat(pGpu, pKernelGmmu) kgmmuFmtGetLatestSupportedFormat_IMPL(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

NvU32 kgmmuGetFaultBufferReservedFbSpaceSize_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU32 kgmmuGetFaultBufferReservedFbSpaceSize(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetFaultBufferReservedFbSpaceSize(pGpu, pKernelGmmu) kgmmuGetFaultBufferReservedFbSpaceSize_IMPL(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferReplayableSetup_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvHandle arg3, NvHandle arg4, NvU32 arg5, RmPhysAddr *arg6);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferReplayableSetup(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvHandle arg3, NvHandle arg4, NvU32 arg5, RmPhysAddr *arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferReplayableSetup(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6) kgmmuFaultBufferReplayableSetup_IMPL(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6)
#endif //__nvoc_kern_gmmu_h_disabled

NvU64 kgmmuGetMinBigPageSize_IMPL(struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetMinBigPageSize(struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetMinBigPageSize(pKernelGmmu) kgmmuGetMinBigPageSize_IMPL(pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuInstBlkInit_IMPL(struct KernelGmmu *pKernelGmmu, PMEMORY_DESCRIPTOR pInstBlkDesc, struct OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pInstBlkParams);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuInstBlkInit(struct KernelGmmu *pKernelGmmu, PMEMORY_DESCRIPTOR pInstBlkDesc, struct OBJVASPACE *pVAS, NvU32 subctxId, INST_BLK_INIT_PARAMS *pInstBlkParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuInstBlkInit(pKernelGmmu, pInstBlkDesc, pVAS, subctxId, pInstBlkParams) kgmmuInstBlkInit_IMPL(pKernelGmmu, pInstBlkDesc, pVAS, subctxId, pInstBlkParams)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferReplayableAllocate_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvHandle arg3, NvHandle arg4);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferReplayableAllocate(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvHandle arg3, NvHandle arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferReplayableAllocate(pGpu, pKernelGmmu, arg3, arg4) kgmmuFaultBufferReplayableAllocate_IMPL(pGpu, pKernelGmmu, arg3, arg4)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferReplayableDestroy_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferReplayableDestroy(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferReplayableDestroy(pGpu, pKernelGmmu) kgmmuFaultBufferReplayableDestroy_IMPL(pGpu, pKernelGmmu)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferAlloc_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferAlloc(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferAlloc(pGpu, pKernelGmmu, arg3, arg4) kgmmuFaultBufferAlloc_IMPL(pGpu, pKernelGmmu, arg3, arg4)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferCreateMemDesc_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU64 arg5, MEMORY_DESCRIPTOR **arg6);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferCreateMemDesc(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 arg4, NvU64 arg5, MEMORY_DESCRIPTOR **arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferCreateMemDesc(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6) kgmmuFaultBufferCreateMemDesc_IMPL(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferGetAddressSpace_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 *arg4, NvU32 *arg5);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferGetAddressSpace(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3, NvU32 *arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferGetAddressSpace(pGpu, pKernelGmmu, arg3, arg4, arg5) kgmmuFaultBufferGetAddressSpace_IMPL(pGpu, pKernelGmmu, arg3, arg4, arg5)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferFree_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferFree(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferFree(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferFree_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuFaultBufferUnregister_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuFaultBufferUnregister(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuFaultBufferUnregister(pGpu, pKernelGmmu, arg3) kgmmuFaultBufferUnregister_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuClientShadowFaultBufferAllocate_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuClientShadowFaultBufferAllocate(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferAllocate(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferAllocate_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuClientShadowFaultBufferDestroy_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuClientShadowFaultBufferDestroy(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferDestroy(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferDestroy_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

NV_STATUS kgmmuClientShadowFaultBufferRegister_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NV_STATUS kgmmuClientShadowFaultBufferRegister(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferRegister(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferRegister_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuClientShadowFaultBufferUnregister_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuClientShadowFaultBufferUnregister(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferUnregister(pGpu, pKernelGmmu, arg3) kgmmuClientShadowFaultBufferUnregister_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuClientShadowFaultBufferPagesDestroy_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3, FAULT_BUFFER_TYPE arg4);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuClientShadowFaultBufferPagesDestroy(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3, FAULT_BUFFER_TYPE arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferPagesDestroy(pGpu, pKernelGmmu, arg3, arg4) kgmmuClientShadowFaultBufferPagesDestroy_IMPL(pGpu, pKernelGmmu, arg3, arg4)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuClientShadowFaultBufferQueueDestroy_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3, FAULT_BUFFER_TYPE arg4);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuClientShadowFaultBufferQueueDestroy(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3, FAULT_BUFFER_TYPE arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuClientShadowFaultBufferQueueDestroy(pGpu, pKernelGmmu, arg3, arg4) kgmmuClientShadowFaultBufferQueueDestroy_IMPL(pGpu, pKernelGmmu, arg3, arg4)
#endif //__nvoc_kern_gmmu_h_disabled

NvU64 kgmmuGetSizeOfPageTables_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *arg3, NvU64 arg4, NvU64 arg5, NvU64 arg6);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetSizeOfPageTables(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *arg3, NvU64 arg4, NvU64 arg5, NvU64 arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetSizeOfPageTables(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6) kgmmuGetSizeOfPageTables_IMPL(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6)
#endif //__nvoc_kern_gmmu_h_disabled

NvU64 kgmmuGetSizeOfPageDirs_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *arg3, NvU64 arg4, NvU64 arg5, NvU64 arg6);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetSizeOfPageDirs(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, const struct GMMU_FMT *arg3, NvU64 arg4, NvU64 arg5, NvU64 arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetSizeOfPageDirs(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6) kgmmuGetSizeOfPageDirs_IMPL(pGpu, pKernelGmmu, arg3, arg4, arg5, arg6)
#endif //__nvoc_kern_gmmu_h_disabled

GMMU_APERTURE kgmmuGetExternalAllocAperture_IMPL(NvU32 addressSpace);

#define kgmmuGetExternalAllocAperture(addressSpace) kgmmuGetExternalAllocAperture_IMPL(addressSpace)
void kgmmuEncodePhysAddrs_IMPL(struct KernelGmmu *pKernelGmmu, const GMMU_APERTURE aperture, NvU64 *pAddresses, NvU64 fabricBaseAddress, NvU64 count);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuEncodePhysAddrs(struct KernelGmmu *pKernelGmmu, const GMMU_APERTURE aperture, NvU64 *pAddresses, NvU64 fabricBaseAddress, NvU64 count) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuEncodePhysAddrs(pKernelGmmu, aperture, pAddresses, fabricBaseAddress, count) kgmmuEncodePhysAddrs_IMPL(pKernelGmmu, aperture, pAddresses, fabricBaseAddress, count)
#endif //__nvoc_kern_gmmu_h_disabled

NvU64 kgmmuEncodePhysAddr_IMPL(struct KernelGmmu *pKernelGmmu, const GMMU_APERTURE aperture, NvU64 physAddr, NvU64 fabricBaseAddress);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuEncodePhysAddr(struct KernelGmmu *pKernelGmmu, const GMMU_APERTURE aperture, NvU64 physAddr, NvU64 fabricBaseAddress) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuEncodePhysAddr(pKernelGmmu, aperture, physAddr, fabricBaseAddress) kgmmuEncodePhysAddr_IMPL(pKernelGmmu, aperture, physAddr, fabricBaseAddress)
#endif //__nvoc_kern_gmmu_h_disabled

void kgmmuAccessCntrChangeIntrOwnership_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void kgmmuAccessCntrChangeIntrOwnership(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuAccessCntrChangeIntrOwnership(pGpu, pKernelGmmu, arg3) kgmmuAccessCntrChangeIntrOwnership_IMPL(pGpu, pKernelGmmu, arg3)
#endif //__nvoc_kern_gmmu_h_disabled

void *kgmmuGetShadowFaultBufferCslContext_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE type);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline void *kgmmuGetShadowFaultBufferCslContext(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, FAULT_BUFFER_TYPE type) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetShadowFaultBufferCslContext(pGpu, pKernelGmmu, type) kgmmuGetShadowFaultBufferCslContext_IMPL(pGpu, pKernelGmmu, type)
#endif //__nvoc_kern_gmmu_h_disabled

NvS32 *kgmmuGetFatalFaultIntrPendingState_IMPL(struct KernelGmmu *pKernelGmmu, NvU8 gfid);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvS32 *kgmmuGetFatalFaultIntrPendingState(struct KernelGmmu *pKernelGmmu, NvU8 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetFatalFaultIntrPendingState(pKernelGmmu, gfid) kgmmuGetFatalFaultIntrPendingState_IMPL(pKernelGmmu, gfid)
#endif //__nvoc_kern_gmmu_h_disabled

struct HW_FAULT_BUFFER *kgmmuGetHwFaultBufferPtr_IMPL(struct KernelGmmu *pKernelGmmu, NvU8 gfid, NvU8 faultBufferIndex);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline struct HW_FAULT_BUFFER *kgmmuGetHwFaultBufferPtr(struct KernelGmmu *pKernelGmmu, NvU8 gfid, NvU8 faultBufferIndex) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return NULL;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetHwFaultBufferPtr(pKernelGmmu, gfid, faultBufferIndex) kgmmuGetHwFaultBufferPtr_IMPL(pKernelGmmu, gfid, faultBufferIndex)
#endif //__nvoc_kern_gmmu_h_disabled

NvU64 kgmmuGetFaultBufferGenCnt_IMPL(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU8 gfid);

#ifdef __nvoc_kern_gmmu_h_disabled
static inline NvU64 kgmmuGetFaultBufferGenCnt(OBJGPU *pGpu, struct KernelGmmu *pKernelGmmu, NvU8 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGmmu was disabled!");
    return 0;
}
#else //__nvoc_kern_gmmu_h_disabled
#define kgmmuGetFaultBufferGenCnt(pGpu, pKernelGmmu, gfid) kgmmuGetFaultBufferGenCnt_IMPL(pGpu, pKernelGmmu, gfid)
#endif //__nvoc_kern_gmmu_h_disabled

#undef PRIVATE_FIELD


// defines for TLB Invalidation scope
#define NV_GMMU_INVAL_SCOPE_ALL_TLBS       0x00000000
#define NV_GMMU_INVAL_SCOPE_LINK_TLBS      0x00000001
#define NV_GMMU_INVAL_SCOPE_NON_LINK_TLBS  0x00000002

// bit fields for uvmSharedIntrRmOwnsMask
#define RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY          NVBIT(0)
#define RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_ERROR           NVBIT(1)
#define RM_UVM_SHARED_INTR_MASK_MMU_ECC_UNCORRECTED_ERROR_NOTIFY   NVBIT(2)
#define RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_NOTIFY        NVBIT(3)
#define RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_OVERFLOW      NVBIT(4)
#define RM_UVM_SHARED_INTR_MASK_MMU_NONREPLAYABLE_FAULT_NOTIFY     NVBIT(5)
#define RM_UVM_SHARED_INTR_MASK_MMU_NONREPLAYABLE_FAULT_OVERFLOW   NVBIT(6)
#define RM_UVM_SHARED_INTR_MASK_MMU_OTHER_FAULT_NOTIFY             NVBIT(7)
#define RM_UVM_SHARED_INTR_MASK_ALL                                (NVBIT(8) - 1)

/*!
 * Constants used for UVM mirroring loops.
 */
#define GMMU_USER_PAGE_DIR_INDEX       0
#define GMMU_KERNEL_PAGE_DIR_INDEX     1
#define GMMU_MAX_PAGE_DIR_INDEX_COUNT  (GMMU_KERNEL_PAGE_DIR_INDEX + 1)

/*!
 * Page table walker callbacks used for map/unmap operations.
 */
extern const MMU_WALK_CALLBACKS  g_gmmuWalkCallbacks;
extern const MMU_WALK_CALLBACKS  g_bar2WalkCallbacks;
extern const MMU_TRACE_CALLBACKS g_gmmuTraceCallbacks;

void       gmmuMemDescCacheFree(GVAS_GPU_STATE *pGpuState);

#endif // KERN_GMMU_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_GMMU_NVOC_H_
