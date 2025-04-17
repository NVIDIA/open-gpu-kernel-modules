
#ifndef _G_KERN_MEM_SYS_NVOC_H_
#define _G_KERN_MEM_SYS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kern_mem_sys_nvoc.h"

#ifndef KERN_MEM_SYS_H
#define KERN_MEM_SYS_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "containers/map.h"
#include "gpu/mem_mgr/heap_base.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
//
// FB Cache (opcode, mem target) defines used by kmemsysCacheOp hal API
//
typedef enum
{
    FB_CACHE_OP_UNDEFINED = 0,
    // invalidate cache lines without writeback of dirty lines to memory
    FB_CACHE_INVALIDATE = 1,
    // writeback dirty lines but leave the lines in valid cache state
    FB_CACHE_WRITEBACK,
    // writeback dirty lines and then invalidates the cache state
    FB_CACHE_EVICT,
} FB_CACHE_OP;

// target memory types for cache operations
typedef enum
{
    FB_CACHE_MEM_UNDEFINED = 0,
    FB_CACHE_SYSTEM_MEMORY = 1,
    FB_CACHE_VIDEO_MEMORY,
    FB_CACHE_PEER_MEMORY,
    FB_CACHE_DIRTY,
    FB_CACHE_COMPTAG_MEMORY,
    FB_CACHE_DIRTY_ALL,
} FB_CACHE_MEMTYPE;

typedef enum
{
    FB_CACHE_STATE_ENABLED,
    FB_CACHE_STATE_DISABLED,
} FB_CACHE_STATE;

typedef enum
{
    FB_CACHE_WRITE_MODE_WRITETHROUGH,
    FB_CACHE_WRITE_MODE_WRITEBACK,
} FB_CACHE_WRITE_MODE;

typedef enum
{
    FB_CACHE_BYPASS_MODE_ENABLED,
    FB_CACHE_BYPASS_MODE_DISABLED,
} FB_CACHE_BYPASS_MODE; // FERMI (TEST) ONLY

typedef enum
{
    FB_CACHE_RCM_STATE_FULL,
    FB_CACHE_RCM_STATE_TRANSITIONING,
    FB_CACHE_RCM_STATE_REDUCED,
    FB_CACHE_RCM_STATE_ZERO_CACHE,
} FB_CACHE_RCM_STATE;

/*! Tracks NUMA information of GPU memory partitions */
typedef struct
{
    NvBool bInUse;     // Is the partition in use?
    NvU64  offset;     // FB offset of the partition
    NvU64  size;       // FB size of the partition
    NvU32  numaNodeId; // OS NUMA Node Id of the partition.
} MEM_PARTITION_NUMA_INFO;

typedef struct MIG_MEM_BOUNDARY_CONFIG_TABLE
{
    /*!
     * Memory boundary config A (4KB aligned)
     */
    NvU64 memBoundaryCfgA;

    /*!
     * Memory boundary config B (4KB aligned)
     */
    NvU64 memBoundaryCfgB;

    /*!
     * Memory boundary config C (64KB aligned)
     */
    NvU32 memBoundaryCfgC;
} MIG_MEM_BOUNDARY_CONFIG_TABLE;

/*!
 * @brief Structure carrying memory configuration information for specific GPU instance
 *        The information will be used to allocate memory when a GPU instance is
 *        created or queried. The structure will be indexed with swizzIDs
 */
typedef struct MIG_GPU_INSTANCE_MEMORY_CONFIG
{
    /*!
     * First VMMU segment from where the GPU instance memory starts
     */
    NvU64 startingVmmuSegment;

    /*!
     * Size of the GPU instance memory in the form of number of vmmu segments
     */
    NvU64 memSizeInVmmuSegment;

    /*!
     * GPU Instance memory config initialization state
     */
    NvBool bInitialized;
} MIG_GPU_INSTANCE_MEMORY_CONFIG;

/* @ref NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS */
typedef NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS MEMORY_SYSTEM_STATIC_CONFIG;

#define FB_HWRESID_CTAGID_FERMI              15:0
#define FB_HWRESID_ZCULL_FERMI               30:16

#define FB_HWRESID_ZCULL_SHIFT_FERMI(i)      (1 << (i))

#define FB_HWRESID_CTAGID_VAL_FERMI(n)        \
    (((n) >> DRF_SHIFT(FB_HWRESID_CTAGID_FERMI)) & DRF_MASK(FB_HWRESID_CTAGID_FERMI))

#define FB_HWRESID_CTAGID_NUM_FERMI(i)        \
   (((i) & DRF_MASK(FB_HWRESID_CTAGID_FERMI)) << DRF_SHIFT(FB_HWRESID_CTAGID_FERMI))

#define FB_SET_HWRESID_CTAGID_FERMI(h, i)                                                      \
    h = ( ((h) & ~(DRF_MASK(FB_HWRESID_CTAGID_FERMI) << DRF_SHIFT(FB_HWRESID_CTAGID_FERMI))) | \
      FB_HWRESID_CTAGID_NUM_FERMI(i) )

#define FB_HWRESID_ZCULL_NUM_FERMI(i)        \
   (((1<<i) & DRF_MASK(FB_HWRESID_ZCULL_FERMI)) << DRF_SHIFT(FB_HWRESID_ZCULL_FERMI))

#define FB_HWRESID_ZCULL_VAL_FERMI(n)        \
    (((n) >> DRF_SHIFT(FB_HWRESID_ZCULL_FERMI)) & DRF_MASK(FB_HWRESID_ZCULL_FERMI))

#define KMEMSYS_FB_NUMA_ONLINE_BASE                                       0

#define KMEMSYS_FB_NUMA_ONLINE_SIZE(memorySize, memblockSize)              NV_ALIGN_DOWN64(memorySize, memblockSize)

/*!
 * KernelMemorySystem is a logical abstraction of the GPU memory system. This
 * type is instantiated in VGPU guest/GSP Client as well as the VGPU
 * host/GSP-RM.
 *
 * When KernelMemorySystem wants to read or write hardware state, it does not
 * have access to the registers on the GPU, it can however perform operations
 * using the following mechanisms:
 *
 * 1.) access registers are virtualized across VFs, e.g.: registers within
 * NV_VIRTUAL_FUNCTION_PRIV_XYZ.
 *
 * 2.) send a RPC to the VGPU Host/GSP-RM to perform the operation.
 *
 * Operations such as "get memory system bus width" are appropriate for this
 * interface. Anything related to managing of the memory page
 * tables/allocations should live in MemoryManager.
 */


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_MEM_SYS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelMemorySystem;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelMemorySystem;


struct KernelMemorySystem {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelMemorySystem *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelMemorySystem *__nvoc_pbase_KernelMemorySystem;    // kmemsys

    // Vtable with 31 per-object function pointers
    NV_STATUS (*__kmemsysGetFbNumaInfo__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU64 *, NvU64 *, NvS32 *);  // halified (2 hals) body
    NV_STATUS (*__kmemsysReadUsableFbSize__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kmemsysGetUsableFbSize__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kmemsysCacheOp__)(OBJGPU *, struct KernelMemorySystem * /*this*/, PMEMORY_DESCRIPTOR, FB_CACHE_MEMTYPE, FB_CACHE_OP);  // halified (2 hals) body
    NV_STATUS (*__kmemsysDoCacheOp__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32, NvU32, NvU32, PRMTIMEOUT);  // halified (2 hals) body
    NvU32 (*__kmemsysReadL2SysmemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    void (*__kmemsysWriteL2SysmemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // halified (2 hals) body
    NvU32 (*__kmemsysReadL2PeermemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    void (*__kmemsysWriteL2PeermemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // halified (2 hals) body
    struct IoAperture * (*__kmemsysInitHshub0Aperture__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    void (*__kmemsysDestroyHshub0Aperture__)(OBJGPU *, struct KernelMemorySystem * /*this*/, struct IoAperture *);  // halified (2 hals) body
    NV_STATUS (*__kmemsysInitFlushSysmemBuffer__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    void (*__kmemsysProgramSysmemFlushBuffer__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (6 hals) body
    NvU32 (*__kmemsysGetFlushSysmemBufferAddrShift__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (4 hals) body
    NvBool (*__kmemsysIsPagePLCable__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU64, NvU64);  // halified (4 hals) body
    NV_STATUS (*__kmemsysReadMIGMemoryCfg__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kmemsysInitMIGMemoryPartitionTable__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kmemsysSwizzIdToVmmuSegmentsRange__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals)
    NV_STATUS (*__kmemsysNumaAddMemory__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32, NvU64, NvU64, NvS32 *);  // halified (2 hals) body
    void (*__kmemsysNumaRemoveMemory__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kmemsysNumaRemoveAllMemory__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kmemsysPopulateMIGGPUInstanceMemConfig__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kmemsysSetupAllAtsPeers__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    void (*__kmemsysRemoveAllAtsPeers__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    NvBool (*__kmemsysAssertFbAckTimeoutPending__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    NvU32 (*__kmemsysGetMaxFbpas__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    NvU32 (*__kmemsysGetEccDedCountSize__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (3 hals) body
    NvU32 (*__kmemsysGetEccDedCountRegAddr__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32, NvU32);  // halified (3 hals) body
    NvU16 (*__kmemsysGetMaximumBlacklistPages__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kmemsysGetFbInfos__)(OBJGPU *, struct KernelMemorySystem * /*this*/, struct RsClient *, Device *, NvHandle, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *, NvU64 *);  // halified (2 hals)
    NvBool (*__kmemsysCheckReadoutEccEnablement__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // halified (2 hals) body

    // 1 PDB property

    // Data members
    NvBool bDisableTiledCachingInvalidatesWithEccBug1521641;
    NvBool bGpuCacheEnable;
    NvBool bNumaNodesAdded;
    NvBool bL2CleanFbPull;
    NvU32 l2WriteMode;
    NvBool bPreserveComptagBackingStoreOnSuspend;
    NvBool bBug3656943WAR;
    NvU32 overrideToGMK;
    NvBool bDisablePlcForCertainOffsetsBug3046774;
    const MEMORY_SYSTEM_STATIC_CONFIG *pStaticConfig;
    MEM_PARTITION_NUMA_INFO *memPartitionNumaInfo;
    MIG_MEM_BOUNDARY_CONFIG_TABLE memBoundaryCfgTable;
    MIG_GPU_INSTANCE_MEMORY_CONFIG gpuInstanceMemConfig[15];
    NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS migMemoryPartitionTable;
    PMEMORY_DESCRIPTOR pSysmemFlushBufferMemDesc;
    NvU64 sysmemFlushBuffer;
    NvU64 fbOverrideStartKb;
    NvU64 coherentCpuFbBase;
    NvU64 coherentCpuFbEnd;
    NvU64 coherentRsvdFbBase;
    NvU64 numaOnlineBase;
    NvU64 numaOnlineSize;
    NvU64 numaMigPartitionSize[15];
    NvBool bNumaMigPartitionSizeEnumerated;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelMemorySystem {
    NV_STATUS (*__kmemsysConstructEngine__)(OBJGPU *, struct KernelMemorySystem * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePreInitLocked__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStateInitLocked__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePreLoad__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePostLoad__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStateLoad__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePreUnload__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmemsysStateUnload__)(OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kmemsysStateDestroy__)(OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kmemsysInitMissing__)(struct OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePreInitUnlocked__)(struct OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmemsysStateInitUnlocked__)(struct OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmemsysStatePostUnload__)(struct OBJGPU *, struct KernelMemorySystem * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kmemsysIsPresent__)(struct OBJGPU *, struct KernelMemorySystem * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelMemorySystem {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelMemorySystem vtable;
};

#ifndef __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
#define __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
typedef struct KernelMemorySystem KernelMemorySystem;
#endif /* __NVOC_CLASS_KernelMemorySystem_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMemorySystem
#define __nvoc_class_id_KernelMemorySystem 0x7faff1
#endif /* __nvoc_class_id_KernelMemorySystem */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem;

#define __staticCast_KernelMemorySystem(pThis) \
    ((pThis)->__nvoc_pbase_KernelMemorySystem)

#ifdef __nvoc_kern_mem_sys_h_disabled
#define __dynamicCast_KernelMemorySystem(pThis) ((KernelMemorySystem*) NULL)
#else //__nvoc_kern_mem_sys_h_disabled
#define __dynamicCast_KernelMemorySystem(pThis) \
    ((KernelMemorySystem*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMemorySystem)))
#endif //__nvoc_kern_mem_sys_h_disabled

// Property macros
#define PDB_PROP_KMEMSYS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMEMSYS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMemorySystem(KernelMemorySystem**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMemorySystem(KernelMemorySystem**, Dynamic*, NvU32);
#define __objCreate_KernelMemorySystem(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMemorySystem((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kmemsysConstructEngine_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysConstructEngine__
#define kmemsysConstructEngine(pGpu, pKernelMemorySystem, arg3) kmemsysConstructEngine_DISPATCH(pGpu, pKernelMemorySystem, arg3)
#define kmemsysStatePreInitLocked_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreInitLocked__
#define kmemsysStatePreInitLocked(pGpu, pKernelMemorySystem) kmemsysStatePreInitLocked_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysStateInitLocked_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateInitLocked__
#define kmemsysStateInitLocked(pGpu, pKernelMemorySystem) kmemsysStateInitLocked_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysStatePreLoad_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreLoad__
#define kmemsysStatePreLoad(pGpu, pKernelMemorySystem, flags) kmemsysStatePreLoad_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStatePostLoad_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePostLoad__
#define kmemsysStatePostLoad(pGpu, pKernelMemorySystem, flags) kmemsysStatePostLoad_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStateLoad_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateLoad__
#define kmemsysStateLoad(pGpu, pKernelMemorySystem, flags) kmemsysStateLoad_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStatePreUnload_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreUnload__
#define kmemsysStatePreUnload(pGpu, pKernelMemorySystem, flags) kmemsysStatePreUnload_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStateUnload_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateUnload__
#define kmemsysStateUnload(pGpu, pKernelMemorySystem, flags) kmemsysStateUnload_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStateDestroy_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateDestroy__
#define kmemsysStateDestroy(pGpu, pKernelMemorySystem) kmemsysStateDestroy_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFbNumaInfo_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetFbNumaInfo__
#define kmemsysGetFbNumaInfo(pGpu, pKernelMemorySystem, physAddr, rsvdPhysAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, rsvdPhysAddr, numaNodeId)
#define kmemsysGetFbNumaInfo_HAL(pGpu, pKernelMemorySystem, physAddr, rsvdPhysAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, rsvdPhysAddr, numaNodeId)
#define kmemsysReadUsableFbSize_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysReadUsableFbSize__
#define kmemsysReadUsableFbSize(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysReadUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysGetUsableFbSize_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetUsableFbSize__
#define kmemsysGetUsableFbSize(pGpu, pKernelMemorySystem, pFbSize) kmemsysGetUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize) kmemsysGetUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysCacheOp_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysCacheOp__
#define kmemsysCacheOp(pGpu, pKernelMemorySystem, arg3, arg4, operation) kmemsysCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg3, arg4, operation)
#define kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, arg3, arg4, operation) kmemsysCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg3, arg4, operation)
#define kmemsysDoCacheOp_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysDoCacheOp__
#define kmemsysDoCacheOp(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6) kmemsysDoCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6)
#define kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6) kmemsysDoCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6)
#define kmemsysReadL2SysmemInvalidateReg_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysReadL2SysmemInvalidateReg__
#define kmemsysReadL2SysmemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysWriteL2SysmemInvalidateReg_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysWriteL2SysmemInvalidateReg__
#define kmemsysWriteL2SysmemInvalidateReg(pGpu, pKernelMemorySystem, arg3) kmemsysWriteL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg3)
#define kmemsysWriteL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg3) kmemsysWriteL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg3)
#define kmemsysReadL2PeermemInvalidateReg_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysReadL2PeermemInvalidateReg__
#define kmemsysReadL2PeermemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysWriteL2PeermemInvalidateReg_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysWriteL2PeermemInvalidateReg__
#define kmemsysWriteL2PeermemInvalidateReg(pGpu, pKernelMemorySystem, arg3) kmemsysWriteL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg3)
#define kmemsysWriteL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg3) kmemsysWriteL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg3)
#define kmemsysInitHshub0Aperture_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysInitHshub0Aperture__
#define kmemsysInitHshub0Aperture(pGpu, pKernelMemorySystem) kmemsysInitHshub0Aperture_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem) kmemsysInitHshub0Aperture_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysDestroyHshub0Aperture_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysDestroyHshub0Aperture__
#define kmemsysDestroyHshub0Aperture(pGpu, pKernelMemorySystem, pIoAperture) kmemsysDestroyHshub0Aperture_DISPATCH(pGpu, pKernelMemorySystem, pIoAperture)
#define kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pIoAperture) kmemsysDestroyHshub0Aperture_DISPATCH(pGpu, pKernelMemorySystem, pIoAperture)
#define kmemsysInitFlushSysmemBuffer_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysInitFlushSysmemBuffer__
#define kmemsysInitFlushSysmemBuffer(pGpu, pKernelMemorySystem) kmemsysInitFlushSysmemBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitFlushSysmemBuffer_HAL(pGpu, pKernelMemorySystem) kmemsysInitFlushSysmemBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysProgramSysmemFlushBuffer_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysProgramSysmemFlushBuffer__
#define kmemsysProgramSysmemFlushBuffer(pGpu, pKernelMemorySystem) kmemsysProgramSysmemFlushBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysProgramSysmemFlushBuffer_HAL(pGpu, pKernelMemorySystem) kmemsysProgramSysmemFlushBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFlushSysmemBufferAddrShift_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetFlushSysmemBufferAddrShift__
#define kmemsysGetFlushSysmemBufferAddrShift(pGpu, pKernelMemorySystem) kmemsysGetFlushSysmemBufferAddrShift_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem) kmemsysGetFlushSysmemBufferAddrShift_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysIsPagePLCable_FNPTR(KernelMemorySystem) KernelMemorySystem->__kmemsysIsPagePLCable__
#define kmemsysIsPagePLCable(pGpu, KernelMemorySystem, physAddr, pageSize) kmemsysIsPagePLCable_DISPATCH(pGpu, KernelMemorySystem, physAddr, pageSize)
#define kmemsysIsPagePLCable_HAL(pGpu, KernelMemorySystem, physAddr, pageSize) kmemsysIsPagePLCable_DISPATCH(pGpu, KernelMemorySystem, physAddr, pageSize)
#define kmemsysReadMIGMemoryCfg_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysReadMIGMemoryCfg__
#define kmemsysReadMIGMemoryCfg(pGpu, pKernelMemorySystem) kmemsysReadMIGMemoryCfg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadMIGMemoryCfg_HAL(pGpu, pKernelMemorySystem) kmemsysReadMIGMemoryCfg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitMIGMemoryPartitionTable_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysInitMIGMemoryPartitionTable__
#define kmemsysInitMIGMemoryPartitionTable(pGpu, pKernelMemorySystem) kmemsysInitMIGMemoryPartitionTable_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitMIGMemoryPartitionTable_HAL(pGpu, pKernelMemorySystem) kmemsysInitMIGMemoryPartitionTable_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSwizzIdToVmmuSegmentsRange_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysSwizzIdToVmmuSegmentsRange__
#define kmemsysSwizzIdToVmmuSegmentsRange(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)
#define kmemsysSwizzIdToVmmuSegmentsRange_HAL(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)
#define kmemsysNumaAddMemory_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysNumaAddMemory__
#define kmemsysNumaAddMemory(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId) kmemsysNumaAddMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId)
#define kmemsysNumaAddMemory_HAL(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId) kmemsysNumaAddMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId)
#define kmemsysNumaRemoveMemory_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysNumaRemoveMemory__
#define kmemsysNumaRemoveMemory(pGpu, pKernelMemorySystem, swizzId) kmemsysNumaRemoveMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId)
#define kmemsysNumaRemoveMemory_HAL(pGpu, pKernelMemorySystem, swizzId) kmemsysNumaRemoveMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId)
#define kmemsysNumaRemoveAllMemory_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysNumaRemoveAllMemory__
#define kmemsysNumaRemoveAllMemory(pGpu, pKernelMemorySystem) kmemsysNumaRemoveAllMemory_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysNumaRemoveAllMemory_HAL(pGpu, pKernelMemorySystem) kmemsysNumaRemoveAllMemory_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysPopulateMIGGPUInstanceMemConfig_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysPopulateMIGGPUInstanceMemConfig__
#define kmemsysPopulateMIGGPUInstanceMemConfig(pGpu, pKernelMemorySystem) kmemsysPopulateMIGGPUInstanceMemConfig_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysPopulateMIGGPUInstanceMemConfig_HAL(pGpu, pKernelMemorySystem) kmemsysPopulateMIGGPUInstanceMemConfig_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSetupAllAtsPeers_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysSetupAllAtsPeers__
#define kmemsysSetupAllAtsPeers(pGpu, pKernelMemorySystem) kmemsysSetupAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSetupAllAtsPeers_HAL(pGpu, pKernelMemorySystem) kmemsysSetupAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysRemoveAllAtsPeers_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysRemoveAllAtsPeers__
#define kmemsysRemoveAllAtsPeers(pGpu, pKernelMemorySystem) kmemsysRemoveAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysRemoveAllAtsPeers_HAL(pGpu, pKernelMemorySystem) kmemsysRemoveAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysAssertFbAckTimeoutPending_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysAssertFbAckTimeoutPending__
#define kmemsysAssertFbAckTimeoutPending(pGpu, pKernelMemorySystem) kmemsysAssertFbAckTimeoutPending_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysAssertFbAckTimeoutPending_HAL(pGpu, pKernelMemorySystem) kmemsysAssertFbAckTimeoutPending_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetMaxFbpas_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetMaxFbpas__
#define kmemsysGetMaxFbpas(pGpu, pKernelMemorySystem) kmemsysGetMaxFbpas_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetMaxFbpas_HAL(pGpu, pKernelMemorySystem) kmemsysGetMaxFbpas_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetEccDedCountSize_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetEccDedCountSize__
#define kmemsysGetEccDedCountSize(pGpu, pKernelMemorySystem) kmemsysGetEccDedCountSize_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetEccDedCountSize_HAL(pGpu, pKernelMemorySystem) kmemsysGetEccDedCountSize_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetEccDedCountRegAddr_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetEccDedCountRegAddr__
#define kmemsysGetEccDedCountRegAddr(pGpu, pKernelMemorySystem, fbpa, subp) kmemsysGetEccDedCountRegAddr_DISPATCH(pGpu, pKernelMemorySystem, fbpa, subp)
#define kmemsysGetEccDedCountRegAddr_HAL(pGpu, pKernelMemorySystem, fbpa, subp) kmemsysGetEccDedCountRegAddr_DISPATCH(pGpu, pKernelMemorySystem, fbpa, subp)
#define kmemsysGetMaximumBlacklistPages_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysGetMaximumBlacklistPages__
#define kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem) kmemsysGetMaximumBlacklistPages_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetMaximumBlacklistPages_HAL(pGpu, pKernelMemorySystem) kmemsysGetMaximumBlacklistPages_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFbInfos_FNPTR(arg_this) arg_this->__kmemsysGetFbInfos__
#define kmemsysGetFbInfos(arg1, arg_this, arg3, arg4, hSubdevice, pParams, pFbInfoListIndicesUnset) kmemsysGetFbInfos_DISPATCH(arg1, arg_this, arg3, arg4, hSubdevice, pParams, pFbInfoListIndicesUnset)
#define kmemsysGetFbInfos_HAL(arg1, arg_this, arg3, arg4, hSubdevice, pParams, pFbInfoListIndicesUnset) kmemsysGetFbInfos_DISPATCH(arg1, arg_this, arg3, arg4, hSubdevice, pParams, pFbInfoListIndicesUnset)
#define kmemsysCheckReadoutEccEnablement_FNPTR(pKernelMemorySystem) pKernelMemorySystem->__kmemsysCheckReadoutEccEnablement__
#define kmemsysCheckReadoutEccEnablement(pGpu, pKernelMemorySystem) kmemsysCheckReadoutEccEnablement_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysCheckReadoutEccEnablement_HAL(pGpu, pKernelMemorySystem) kmemsysCheckReadoutEccEnablement_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kmemsysInitMissing(pGpu, pEngstate) kmemsysInitMissing_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kmemsysStatePreInitUnlocked(pGpu, pEngstate) kmemsysStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kmemsysStateInitUnlocked(pGpu, pEngstate) kmemsysStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kmemsysStatePostUnload(pGpu, pEngstate, arg3) kmemsysStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmemsysIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kmemsysIsPresent(pGpu, pEngstate) kmemsysIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kmemsysConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, ENGDESCRIPTOR arg3) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysConstructEngine__(pGpu, pKernelMemorySystem, arg3);
}

static inline NV_STATUS kmemsysStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreInitLocked__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateInitLocked__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysStatePreLoad_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreLoad__(pGpu, pKernelMemorySystem, flags);
}

static inline NV_STATUS kmemsysStatePostLoad_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePostLoad__(pGpu, pKernelMemorySystem, flags);
}

static inline NV_STATUS kmemsysStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateLoad__(pGpu, pKernelMemorySystem, flags);
}

static inline NV_STATUS kmemsysStatePreUnload_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStatePreUnload__(pGpu, pKernelMemorySystem, flags);
}

static inline NV_STATUS kmemsysStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateUnload__(pGpu, pKernelMemorySystem, flags);
}

static inline void kmemsysStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__nvoc_metadata_ptr->vtable.__kmemsysStateDestroy__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysGetFbNumaInfo_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvU64 *rsvdPhysAddr, NvS32 *numaNodeId) {
    return pKernelMemorySystem->__kmemsysGetFbNumaInfo__(pGpu, pKernelMemorySystem, physAddr, rsvdPhysAddr, numaNodeId);
}

static inline NV_STATUS kmemsysReadUsableFbSize_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    return pKernelMemorySystem->__kmemsysReadUsableFbSize__(pGpu, pKernelMemorySystem, pFbSize);
}

static inline NV_STATUS kmemsysGetUsableFbSize_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    return pKernelMemorySystem->__kmemsysGetUsableFbSize__(pGpu, pKernelMemorySystem, pFbSize);
}

static inline NV_STATUS kmemsysCacheOp_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg3, FB_CACHE_MEMTYPE arg4, FB_CACHE_OP operation) {
    return pKernelMemorySystem->__kmemsysCacheOp__(pGpu, pKernelMemorySystem, arg3, arg4, operation);
}

static inline NV_STATUS kmemsysDoCacheOp_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3, NvU32 arg4, NvU32 arg5, PRMTIMEOUT arg6) {
    return pKernelMemorySystem->__kmemsysDoCacheOp__(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6);
}

static inline NvU32 kmemsysReadL2SysmemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadL2SysmemInvalidateReg__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysWriteL2SysmemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3) {
    pKernelMemorySystem->__kmemsysWriteL2SysmemInvalidateReg__(pGpu, pKernelMemorySystem, arg3);
}

static inline NvU32 kmemsysReadL2PeermemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadL2PeermemInvalidateReg__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysWriteL2PeermemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3) {
    pKernelMemorySystem->__kmemsysWriteL2PeermemInvalidateReg__(pGpu, pKernelMemorySystem, arg3);
}

static inline struct IoAperture * kmemsysInitHshub0Aperture_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitHshub0Aperture__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysDestroyHshub0Aperture_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, struct IoAperture *pIoAperture) {
    pKernelMemorySystem->__kmemsysDestroyHshub0Aperture__(pGpu, pKernelMemorySystem, pIoAperture);
}

static inline NV_STATUS kmemsysInitFlushSysmemBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitFlushSysmemBuffer__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysProgramSysmemFlushBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysProgramSysmemFlushBuffer__(pGpu, pKernelMemorySystem);
}

static inline NvU32 kmemsysGetFlushSysmemBufferAddrShift_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysGetFlushSysmemBufferAddrShift__(pGpu, pKernelMemorySystem);
}

static inline NvBool kmemsysIsPagePLCable_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    return KernelMemorySystem->__kmemsysIsPagePLCable__(pGpu, KernelMemorySystem, physAddr, pageSize);
}

static inline NV_STATUS kmemsysReadMIGMemoryCfg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadMIGMemoryCfg__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysInitMIGMemoryPartitionTable_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitMIGMemoryPartitionTable__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments) {
    return pKernelMemorySystem->__kmemsysSwizzIdToVmmuSegmentsRange__(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments);
}

static inline NV_STATUS kmemsysNumaAddMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId) {
    return pKernelMemorySystem->__kmemsysNumaAddMemory__(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId);
}

static inline void kmemsysNumaRemoveMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId) {
    pKernelMemorySystem->__kmemsysNumaRemoveMemory__(pGpu, pKernelMemorySystem, swizzId);
}

static inline void kmemsysNumaRemoveAllMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysNumaRemoveAllMemory__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysPopulateMIGGPUInstanceMemConfig_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysPopulateMIGGPUInstanceMemConfig__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysSetupAllAtsPeers_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysSetupAllAtsPeers__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysRemoveAllAtsPeers_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysRemoveAllAtsPeers__(pGpu, pKernelMemorySystem);
}

static inline NvBool kmemsysAssertFbAckTimeoutPending_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysAssertFbAckTimeoutPending__(pGpu, pKernelMemorySystem);
}

static inline NvU32 kmemsysGetMaxFbpas_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysGetMaxFbpas__(pGpu, pKernelMemorySystem);
}

static inline NvU32 kmemsysGetEccDedCountSize_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysGetEccDedCountSize__(pGpu, pKernelMemorySystem);
}

static inline NvU32 kmemsysGetEccDedCountRegAddr_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp) {
    return pKernelMemorySystem->__kmemsysGetEccDedCountRegAddr__(pGpu, pKernelMemorySystem, fbpa, subp);
}

static inline NvU16 kmemsysGetMaximumBlacklistPages_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysGetMaximumBlacklistPages__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysGetFbInfos_DISPATCH(OBJGPU *arg1, struct KernelMemorySystem *arg_this, struct RsClient *arg3, Device *arg4, NvHandle hSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams, NvU64 *pFbInfoListIndicesUnset) {
    return arg_this->__kmemsysGetFbInfos__(arg1, arg_this, arg3, arg4, hSubdevice, pParams, pFbInfoListIndicesUnset);
}

static inline NvBool kmemsysCheckReadoutEccEnablement_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysCheckReadoutEccEnablement__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kmemsysInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmemsysStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmemsysStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmemsysStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmemsysStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmemsysStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmemsysStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kmemsysIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmemsysIsPresent__(pGpu, pEngstate);
}

static inline void kmemsysAssertSysmemFlushBufferValid_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysAssertSysmemFlushBufferValid_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GB10B(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysAssertSysmemFlushBufferValid(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysAssertSysmemFlushBufferValid(pGpu, pKernelMemorySystem) kmemsysAssertSysmemFlushBufferValid_b3696a(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysAssertSysmemFlushBufferValid_HAL(pGpu, pKernelMemorySystem) kmemsysAssertSysmemFlushBufferValid(pGpu, pKernelMemorySystem)

NV_STATUS kmemsysInitStaticConfig_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, MEMORY_SYSTEM_STATIC_CONFIG *pConfig);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysInitStaticConfig(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, MEMORY_SYSTEM_STATIC_CONFIG *pConfig) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysInitStaticConfig(pGpu, pKernelMemorySystem, pConfig) kmemsysInitStaticConfig_KERNEL(pGpu, pKernelMemorySystem, pConfig)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysInitStaticConfig_HAL(pGpu, pKernelMemorySystem, pConfig) kmemsysInitStaticConfig(pGpu, pKernelMemorySystem, pConfig)

static inline NV_STATUS kmemsysPreFillCacheOnlyMemory_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg3, NvU64 arg4) {
    return NV_OK;
}

NV_STATUS kmemsysPreFillCacheOnlyMemory_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg3, NvU64 arg4);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysPreFillCacheOnlyMemory(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPreFillCacheOnlyMemory(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysPreFillCacheOnlyMemory_56cd7a(pGpu, pKernelMemorySystem, arg3, arg4)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysPreFillCacheOnlyMemory(pGpu, pKernelMemorySystem, arg3, arg4)

static inline NV_STATUS kmemsysCheckDisplayRemapperRange_14278f(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_INVALID_STATE);
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysCheckDisplayRemapperRange(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysCheckDisplayRemapperRange(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysCheckDisplayRemapperRange_14278f(pGpu, pKernelMemorySystem, arg3, arg4)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysCheckDisplayRemapperRange_HAL(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysCheckDisplayRemapperRange(pGpu, pKernelMemorySystem, arg3, arg4)

NV_STATUS kmemsysPostHeapCreate_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysPostHeapCreate(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPostHeapCreate(pGpu, pKernelMemorySystem) kmemsysPostHeapCreate_KERNEL(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPostHeapCreate_HAL(pGpu, pKernelMemorySystem) kmemsysPostHeapCreate(pGpu, pKernelMemorySystem)

static inline void kmemsysPreHeapDestruct_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysPreHeapDestruct(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPreHeapDestruct(pGpu, pKernelMemorySystem) kmemsysPreHeapDestruct_b3696a(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPreHeapDestruct_HAL(pGpu, pKernelMemorySystem) kmemsysPreHeapDestruct(pGpu, pKernelMemorySystem)

NV_STATUS kmemsysAllocComprResources_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, FB_ALLOC_INFO *arg3, NvU64 arg4, NvU32 arg5, NvU32 *arg6, NvU32 arg7);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysAllocComprResources(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, FB_ALLOC_INFO *arg3, NvU64 arg4, NvU32 arg5, NvU32 *arg6, NvU32 arg7) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysAllocComprResources(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6, arg7) kmemsysAllocComprResources_KERNEL(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysAllocComprResources_HAL(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6, arg7) kmemsysAllocComprResources(pGpu, pKernelMemorySystem, arg3, arg4, arg5, arg6, arg7)

static inline void kmemsysFreeComprResources_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 hwResId) {
    return;
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysFreeComprResources(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 hwResId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysFreeComprResources(pGpu, pKernelMemorySystem, hwResId) kmemsysFreeComprResources_b3696a(pGpu, pKernelMemorySystem, hwResId)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysFreeComprResources_HAL(pGpu, pKernelMemorySystem, hwResId) kmemsysFreeComprResources(pGpu, pKernelMemorySystem, hwResId)

NvBool kmemsysNeedInvalidateGpuCacheOnMap_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bIsVolatile, NvU32 aperture);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvBool kmemsysNeedInvalidateGpuCacheOnMap(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bIsVolatile, NvU32 aperture) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysNeedInvalidateGpuCacheOnMap(pGpu, pKernelMemorySystem, bIsVolatile, aperture) kmemsysNeedInvalidateGpuCacheOnMap_GV100(pGpu, pKernelMemorySystem, bIsVolatile, aperture)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysNeedInvalidateGpuCacheOnMap_HAL(pGpu, pKernelMemorySystem, bIsVolatile, aperture) kmemsysNeedInvalidateGpuCacheOnMap(pGpu, pKernelMemorySystem, bIsVolatile, aperture)

static inline NvBool kmemsysCbcIsSafe_88bc07(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_TRUE;
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvBool kmemsysCbcIsSafe(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysCbcIsSafe(pGpu, pKernelMemorySystem) kmemsysCbcIsSafe_88bc07(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysCbcIsSafe_HAL(pGpu, pKernelMemorySystem) kmemsysCbcIsSafe(pGpu, pKernelMemorySystem)

void kmemsysGetEccCounts_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 *arg3, NvU32 *arg4);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysGetEccCounts(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetEccCounts(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysGetEccCounts_TU102(pGpu, pKernelMemorySystem, arg3, arg4)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysGetEccCounts_HAL(pGpu, pKernelMemorySystem, arg3, arg4) kmemsysGetEccCounts(pGpu, pKernelMemorySystem, arg3, arg4)

NvU32 kmemsysGetL2EccDedCountRegAddr_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvU32 kmemsysGetL2EccDedCountRegAddr(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return 0;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetL2EccDedCountRegAddr(pGpu, pKernelMemorySystem, fbpa, subp) kmemsysGetL2EccDedCountRegAddr_TU102(pGpu, pKernelMemorySystem, fbpa, subp)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysGetL2EccDedCountRegAddr_HAL(pGpu, pKernelMemorySystem, fbpa, subp) kmemsysGetL2EccDedCountRegAddr(pGpu, pKernelMemorySystem, fbpa, subp)

static inline NV_STATUS kmemsysPrepareForXVEReset_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysPrepareForXVEReset(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPrepareForXVEReset(pGpu, pKernelMemorySystem) kmemsysPrepareForXVEReset_56cd7a(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPrepareForXVEReset_HAL(pGpu, pKernelMemorySystem) kmemsysPrepareForXVEReset(pGpu, pKernelMemorySystem)

NV_STATUS kmemsysConstructEngine_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, ENGDESCRIPTOR arg3);

NV_STATUS kmemsysStatePreInitLocked_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysStatePreLoad_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

NV_STATUS kmemsysStatePostLoad_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

NV_STATUS kmemsysStateLoad_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

NV_STATUS kmemsysStatePreUnload_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

NV_STATUS kmemsysStateUnload_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

void kmemsysStateDestroy_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysGetFbNumaInfo_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvU64 *rsvdPhysAddr, NvS32 *numaNodeId);

static inline NV_STATUS kmemsysGetFbNumaInfo_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvU64 *rsvdPhysAddr, NvS32 *numaNodeId) {
    return NV_OK;
}

NV_STATUS kmemsysReadUsableFbSize_GP102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);

NV_STATUS kmemsysReadUsableFbSize_GA102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);

NV_STATUS kmemsysGetUsableFbSize_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);

static inline NV_STATUS kmemsysGetUsableFbSize_5baef9(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kmemsysCacheOp_GM200(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg3, FB_CACHE_MEMTYPE arg4, FB_CACHE_OP operation);

NV_STATUS kmemsysCacheOp_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg3, FB_CACHE_MEMTYPE arg4, FB_CACHE_OP operation);

NV_STATUS kmemsysDoCacheOp_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3, NvU32 arg4, NvU32 arg5, PRMTIMEOUT arg6);

NV_STATUS kmemsysDoCacheOp_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3, NvU32 arg4, NvU32 arg5, PRMTIMEOUT arg6);

NvU32 kmemsysReadL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysReadL2SysmemInvalidateReg_68b109(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, -1);
}

void kmemsysWriteL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3);

static inline void kmemsysWriteL2SysmemInvalidateReg_f2d351(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
}

NvU32 kmemsysReadL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysReadL2PeermemInvalidateReg_68b109(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, -1);
}

void kmemsysWriteL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3);

static inline void kmemsysWriteL2PeermemInvalidateReg_f2d351(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
}

struct IoAperture *kmemsysInitHshub0Aperture_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline struct IoAperture *kmemsysInitHshub0Aperture_fa6e19(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return ((void *)0);
}

void kmemsysDestroyHshub0Aperture_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, struct IoAperture *pIoAperture);

static inline void kmemsysDestroyHshub0Aperture_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, struct IoAperture *pIoAperture) {
    return;
}

static inline NV_STATUS kmemsysInitFlushSysmemBuffer_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

NV_STATUS kmemsysInitFlushSysmemBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysInitFlushSysmemBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysProgramSysmemFlushBuffer_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysProgramSysmemFlushBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GB10B(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysGetFlushSysmemBufferAddrShift_4a4dee(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return 0;
}

NvU32 kmemsysGetFlushSysmemBufferAddrShift_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvU32 kmemsysGetFlushSysmemBufferAddrShift_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvU32 kmemsysGetFlushSysmemBufferAddrShift_GB10B(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvBool kmemsysIsPagePLCable_88bc07(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    return NV_TRUE;
}

NvBool kmemsysIsPagePLCable_GA100(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize);

NvBool kmemsysIsPagePLCable_GA102(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize);

static inline NvBool kmemsysIsPagePLCable_84161d(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_TRUE);
}

static inline NV_STATUS kmemsysReadMIGMemoryCfg_46f6a7(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kmemsysReadMIGMemoryCfg_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysInitMIGMemoryPartitionTable_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

NV_STATUS kmemsysInitMIGMemoryPartitionTable_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments);

NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments);

NV_STATUS kmemsysNumaAddMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId);

static inline NV_STATUS kmemsysNumaAddMemory_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId) {
    return NV_OK;
}

void kmemsysNumaRemoveMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId);

static inline void kmemsysNumaRemoveMemory_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId) {
    return;
}

void kmemsysNumaRemoveAllMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysNumaRemoveAllMemory_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

static inline NV_STATUS kmemsysPopulateMIGGPUInstanceMemConfig_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

NV_STATUS kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysSetupAllAtsPeers_46f6a7(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kmemsysSetupAllAtsPeers_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysRemoveAllAtsPeers_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysRemoveAllAtsPeers_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvBool kmemsysAssertFbAckTimeoutPending_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvBool kmemsysAssertFbAckTimeoutPending_GB100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvBool kmemsysAssertFbAckTimeoutPending_3dd2c9(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_FALSE;
}

NvU32 kmemsysGetMaxFbpas_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvU32 kmemsysGetMaxFbpas_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysGetMaxFbpas_4a4dee(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return 0;
}

NvU32 kmemsysGetEccDedCountSize_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvU32 kmemsysGetEccDedCountSize_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysGetEccDedCountSize_4a4dee(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return 0;
}

NvU32 kmemsysGetEccDedCountRegAddr_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp);

NvU32 kmemsysGetEccDedCountRegAddr_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp);

static inline NvU32 kmemsysGetEccDedCountRegAddr_4a4dee(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 fbpa, NvU32 subp) {
    return 0;
}

NvU16 kmemsysGetMaximumBlacklistPages_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvU16 kmemsysGetMaximumBlacklistPages_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysGetFbInfos_ac1694(OBJGPU *arg1, struct KernelMemorySystem *arg2, struct RsClient *arg3, Device *arg4, NvHandle hSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams, NvU64 *pFbInfoListIndicesUnset) {
    return NV_OK;
}

NV_STATUS kmemsysGetFbInfos_VF(OBJGPU *arg1, struct KernelMemorySystem *arg2, struct RsClient *arg3, Device *arg4, NvHandle hSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams, NvU64 *pFbInfoListIndicesUnset);

NvBool kmemsysCheckReadoutEccEnablement_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NvBool kmemsysCheckReadoutEccEnablement_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvBool kmemsysIsL2CleanFbPull(struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->bL2CleanFbPull;
}

void kmemsysDestruct_IMPL(struct KernelMemorySystem *pKernelMemorySystem);

#define __nvoc_kmemsysDestruct(pKernelMemorySystem) kmemsysDestruct_IMPL(pKernelMemorySystem)
NV_STATUS kmemsysFlushGpuCache_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, struct KernelBus *pKernelBus, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysFlushGpuCache(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, struct KernelBus *pKernelBus, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysFlushGpuCache(pGpu, pKernelMemorySystem, pKernelBus, pCacheFlushParams) kmemsysFlushGpuCache_IMPL(pGpu, pKernelMemorySystem, pKernelBus, pCacheFlushParams)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysEnsureSysmemFlushBufferInitialized_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysEnsureSysmemFlushBufferInitialized(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysEnsureSysmemFlushBufferInitialized(pGpu, pKernelMemorySystem) kmemsysEnsureSysmemFlushBufferInitialized_IMPL(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

const MEMORY_SYSTEM_STATIC_CONFIG *kmemsysGetStaticConfig_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline const MEMORY_SYSTEM_STATIC_CONFIG *kmemsysGetStaticConfig(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NULL;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetStaticConfig(pGpu, pKernelMemorySystem) kmemsysGetStaticConfig_IMPL(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysSetupCoherentCpuLink_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bFlush);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSetupCoherentCpuLink(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bFlush) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSetupCoherentCpuLink(pGpu, pKernelMemorySystem, bFlush) kmemsysSetupCoherentCpuLink_IMPL(pGpu, pKernelMemorySystem, bFlush)
#endif //__nvoc_kern_mem_sys_h_disabled

void kmemsysTeardownCoherentCpuLink_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bFlush);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysTeardownCoherentCpuLink(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvBool bFlush) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysTeardownCoherentCpuLink(pGpu, pKernelMemorySystem, bFlush) kmemsysTeardownCoherentCpuLink_IMPL(pGpu, pKernelMemorySystem, bFlush)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysSendL2InvalidateEvict_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSendL2InvalidateEvict(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSendL2InvalidateEvict(pGpu, pKernelMemorySystem, flags) kmemsysSendL2InvalidateEvict_IMPL(pGpu, pKernelMemorySystem, flags)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysSendFlushL2AllRamsAndCaches_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSendFlushL2AllRamsAndCaches(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSendFlushL2AllRamsAndCaches(pGpu, pKernelMemorySystem) kmemsysSendFlushL2AllRamsAndCaches_IMPL(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysSwizzIdToMIGMemSize_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE totalRange, NvU32 *pPartitionSizeFlag, NvU64 *pSizeInBytes);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSwizzIdToMIGMemSize(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE totalRange, NvU32 *pPartitionSizeFlag, NvU64 *pSizeInBytes) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSwizzIdToMIGMemSize(pGpu, pKernelMemorySystem, swizzId, totalRange, pPartitionSizeFlag, pSizeInBytes) kmemsysSwizzIdToMIGMemSize_IMPL(pGpu, pKernelMemorySystem, swizzId, totalRange, pPartitionSizeFlag, pSizeInBytes)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysSwizzIdToMIGMemRange_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE totalRange, struct NV_RANGE *pAddrRange);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSwizzIdToMIGMemRange(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE totalRange, struct NV_RANGE *pAddrRange) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSwizzIdToMIGMemRange(pGpu, pKernelMemorySystem, swizzId, totalRange, pAddrRange) kmemsysSwizzIdToMIGMemRange_IMPL(pGpu, pKernelMemorySystem, swizzId, totalRange, pAddrRange)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysGetMIGGPUInstanceMemInfo_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE *pAddrRange);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysGetMIGGPUInstanceMemInfo(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, struct NV_RANGE *pAddrRange) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetMIGGPUInstanceMemInfo(pGpu, pKernelMemorySystem, swizzId, pAddrRange) kmemsysGetMIGGPUInstanceMemInfo_IMPL(pGpu, pKernelMemorySystem, swizzId, pAddrRange)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysGetMIGGPUInstanceMemConfigFromSwizzId_IMPL(OBJGPU *arg1, struct KernelMemorySystem *arg2, NvU32 swizzId, const MIG_GPU_INSTANCE_MEMORY_CONFIG **arg4);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysGetMIGGPUInstanceMemConfigFromSwizzId(OBJGPU *arg1, struct KernelMemorySystem *arg2, NvU32 swizzId, const MIG_GPU_INSTANCE_MEMORY_CONFIG **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetMIGGPUInstanceMemConfigFromSwizzId(arg1, arg2, swizzId, arg4) kmemsysGetMIGGPUInstanceMemConfigFromSwizzId_IMPL(arg1, arg2, swizzId, arg4)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysInitMIGGPUInstanceMemConfigForSwizzId_IMPL(OBJGPU *arg1, struct KernelMemorySystem *arg2, NvU32 swizzId, NvU64 startingVmmuSegment, NvU64 memSizeInVmmuSegment);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysInitMIGGPUInstanceMemConfigForSwizzId(OBJGPU *arg1, struct KernelMemorySystem *arg2, NvU32 swizzId, NvU64 startingVmmuSegment, NvU64 memSizeInVmmuSegment) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysInitMIGGPUInstanceMemConfigForSwizzId(arg1, arg2, swizzId, startingVmmuSegment, memSizeInVmmuSegment) kmemsysInitMIGGPUInstanceMemConfigForSwizzId_IMPL(arg1, arg2, swizzId, startingVmmuSegment, memSizeInVmmuSegment)
#endif //__nvoc_kern_mem_sys_h_disabled

#undef PRIVATE_FIELD


#define IS_COHERENT_CPU_ATS_OFFSET(kmemsys, offset, length)                             \
    (kmemsys && ((offset) >= (kmemsys->coherentCpuFbBase + kmemsys->numaOnlineBase)) && \
     (((NvU64)offset + length) <= (kmemsys->coherentCpuFbBase + kmemsys->numaOnlineBase + kmemsys->numaOnlineSize)))

#define IS_COHERENT_FB_OFFSET(kmemsys, offset, length)                        \
    (kmemsys && (kmemsys->numaOnlineSize == 0) &&                             \
     ((offset) >= (kmemsys->coherentCpuFbBase)) &&                            \
     (((NvU64)offset + length) <= (kmemsys->coherentCpuFbEnd)))

#endif // KERN_MEM_SYS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_MEM_SYS_NVOC_H_
