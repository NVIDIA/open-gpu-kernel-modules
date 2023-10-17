#ifndef _G_KERN_MEM_SYS_NVOC_H_
#define _G_KERN_MEM_SYS_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kern_mem_sys_nvoc.h"

#ifndef KERN_MEM_SYS_H
#define KERN_MEM_SYS_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "containers/map.h"
#include "gpu/mem_mgr/heap_base.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
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

#ifdef NVOC_KERN_MEM_SYS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelMemorySystem {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelMemorySystem *__nvoc_pbase_KernelMemorySystem;
    NV_STATUS (*__kmemsysConstructEngine__)(OBJGPU *, struct KernelMemorySystem *, ENGDESCRIPTOR);
    NV_STATUS (*__kmemsysStateInitLocked__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStatePreLoad__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStatePostLoad__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStatePreUnload__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    void (*__kmemsysStateDestroy__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysGetFbNumaInfo__)(OBJGPU *, struct KernelMemorySystem *, NvU64 *, NvS32 *);
    NV_STATUS (*__kmemsysReadUsableFbSize__)(OBJGPU *, struct KernelMemorySystem *, NvU64 *);
    NV_STATUS (*__kmemsysCacheOp__)(OBJGPU *, struct KernelMemorySystem *, PMEMORY_DESCRIPTOR, FB_CACHE_MEMTYPE, FB_CACHE_OP);
    NV_STATUS (*__kmemsysDoCacheOp__)(OBJGPU *, struct KernelMemorySystem *, NvU32, NvU32, NvU32, PRMTIMEOUT);
    NvU32 (*__kmemsysReadL2SysmemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysWriteL2SysmemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    NvU32 (*__kmemsysReadL2PeermemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysWriteL2PeermemInvalidateReg__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysInitFlushSysmemBuffer__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysProgramSysmemFlushBuffer__)(OBJGPU *, struct KernelMemorySystem *);
    NvBool (*__kmemsysIsPagePLCable__)(OBJGPU *, struct KernelMemorySystem *, NvU64, NvU64);
    NV_STATUS (*__kmemsysReadMIGMemoryCfg__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysInitMIGMemoryPartitionTable__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysSwizzIdToVmmuSegmentsRange__)(OBJGPU *, struct KernelMemorySystem *, NvU32, NvU32, NvU32);
    NV_STATUS (*__kmemsysNumaAddMemory__)(OBJGPU *, struct KernelMemorySystem *, NvU32, NvU64, NvU64, NvS32 *);
    void (*__kmemsysNumaRemoveMemory__)(OBJGPU *, struct KernelMemorySystem *, NvU32);
    void (*__kmemsysNumaRemoveAllMemory__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysSetupAllAtsPeers__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysRemoveAllAtsPeers__)(OBJGPU *, struct KernelMemorySystem *);
    NvBool (*__kmemsysAssertFbAckTimeoutPending__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysCheckEccCounts__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysClearEccCounts__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStateLoad__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStateUnload__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStatePostUnload__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStateInitUnlocked__)(POBJGPU, struct KernelMemorySystem *);
    void (*__kmemsysInitMissing__)(POBJGPU, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStatePreInitLocked__)(POBJGPU, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStatePreInitUnlocked__)(POBJGPU, struct KernelMemorySystem *);
    NvBool (*__kmemsysIsPresent__)(POBJGPU, struct KernelMemorySystem *);
    NvBool bDisableTiledCachingInvalidatesWithEccBug1521641;
    NvBool bGpuCacheEnable;
    NvBool bNumaNodesAdded;
    NvBool bL2CleanFbPull;
    NvBool bPreserveComptagBackingStoreOnSuspend;
    NvBool bBug3656943WAR;
    const MEMORY_SYSTEM_STATIC_CONFIG *pStaticConfig;
    MEM_PARTITION_NUMA_INFO *memPartitionNumaInfo;
    MIG_MEM_BOUNDARY_CONFIG_TABLE memBoundaryCfgTable;
    MIG_GPU_INSTANCE_MEMORY_CONFIG gpuInstanceMemConfig[15];
    NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS migMemoryPartitionTable;
    PMEMORY_DESCRIPTOR pSysmemFlushBufferMemDesc;
    NvU64 sysmemFlushBuffer;
    NvU64 coherentCpuFbBase;
    NvU64 coherentCpuFbEnd;
    NvU64 numaOnlineBase;
    NvU64 numaOnlineSize;
};

#ifndef __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
#define __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
typedef struct KernelMemorySystem KernelMemorySystem;
#endif /* __NVOC_CLASS_KernelMemorySystem_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMemorySystem
#define __nvoc_class_id_KernelMemorySystem 0x7faff1
#endif /* __nvoc_class_id_KernelMemorySystem */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem;

#define __staticCast_KernelMemorySystem(pThis) \
    ((pThis)->__nvoc_pbase_KernelMemorySystem)

#ifdef __nvoc_kern_mem_sys_h_disabled
#define __dynamicCast_KernelMemorySystem(pThis) ((KernelMemorySystem*)NULL)
#else //__nvoc_kern_mem_sys_h_disabled
#define __dynamicCast_KernelMemorySystem(pThis) \
    ((KernelMemorySystem*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMemorySystem)))
#endif //__nvoc_kern_mem_sys_h_disabled

#define PDB_PROP_KMEMSYS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMEMSYS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMemorySystem(KernelMemorySystem**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMemorySystem(KernelMemorySystem**, Dynamic*, NvU32);
#define __objCreate_KernelMemorySystem(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMemorySystem((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kmemsysConstructEngine(pGpu, pKernelMemorySystem, arg0) kmemsysConstructEngine_DISPATCH(pGpu, pKernelMemorySystem, arg0)
#define kmemsysStateInitLocked(pGpu, pKernelMemorySystem) kmemsysStateInitLocked_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysStatePreLoad(pGpu, pKernelMemorySystem, flags) kmemsysStatePreLoad_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStatePostLoad(pGpu, pKernelMemorySystem, flags) kmemsysStatePostLoad_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStatePreUnload(pGpu, pKernelMemorySystem, flags) kmemsysStatePreUnload_DISPATCH(pGpu, pKernelMemorySystem, flags)
#define kmemsysStateDestroy(pGpu, pKernelMemorySystem) kmemsysStateDestroy_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFbNumaInfo(pGpu, pKernelMemorySystem, physAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, numaNodeId)
#define kmemsysGetFbNumaInfo_HAL(pGpu, pKernelMemorySystem, physAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, numaNodeId)
#define kmemsysReadUsableFbSize(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysReadUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, operation) kmemsysCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg0, arg1, operation)
#define kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, arg0, arg1, operation) kmemsysCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg0, arg1, operation)
#define kmemsysDoCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3) kmemsysDoCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3)
#define kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3) kmemsysDoCacheOp_DISPATCH(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3)
#define kmemsysReadL2SysmemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysWriteL2SysmemInvalidateReg(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg0)
#define kmemsysWriteL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2SysmemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg0)
#define kmemsysReadL2PeermemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysWriteL2PeermemInvalidateReg(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg0)
#define kmemsysWriteL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2PeermemInvalidateReg_DISPATCH(pGpu, pKernelMemorySystem, arg0)
#define kmemsysInitFlushSysmemBuffer(pGpu, pKernelMemorySystem) kmemsysInitFlushSysmemBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitFlushSysmemBuffer_HAL(pGpu, pKernelMemorySystem) kmemsysInitFlushSysmemBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysProgramSysmemFlushBuffer(pGpu, pKernelMemorySystem) kmemsysProgramSysmemFlushBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysProgramSysmemFlushBuffer_HAL(pGpu, pKernelMemorySystem) kmemsysProgramSysmemFlushBuffer_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysIsPagePLCable(pGpu, KernelMemorySystem, physAddr, pageSize) kmemsysIsPagePLCable_DISPATCH(pGpu, KernelMemorySystem, physAddr, pageSize)
#define kmemsysIsPagePLCable_HAL(pGpu, KernelMemorySystem, physAddr, pageSize) kmemsysIsPagePLCable_DISPATCH(pGpu, KernelMemorySystem, physAddr, pageSize)
#define kmemsysReadMIGMemoryCfg(pGpu, pKernelMemorySystem) kmemsysReadMIGMemoryCfg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysReadMIGMemoryCfg_HAL(pGpu, pKernelMemorySystem) kmemsysReadMIGMemoryCfg_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitMIGMemoryPartitionTable(pGpu, pKernelMemorySystem) kmemsysInitMIGMemoryPartitionTable_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysInitMIGMemoryPartitionTable_HAL(pGpu, pKernelMemorySystem) kmemsysInitMIGMemoryPartitionTable_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSwizzIdToVmmuSegmentsRange(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)
#define kmemsysSwizzIdToVmmuSegmentsRange_HAL(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)
#define kmemsysNumaAddMemory(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId) kmemsysNumaAddMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId)
#define kmemsysNumaAddMemory_HAL(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId) kmemsysNumaAddMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId)
#define kmemsysNumaRemoveMemory(pGpu, pKernelMemorySystem, swizzId) kmemsysNumaRemoveMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId)
#define kmemsysNumaRemoveMemory_HAL(pGpu, pKernelMemorySystem, swizzId) kmemsysNumaRemoveMemory_DISPATCH(pGpu, pKernelMemorySystem, swizzId)
#define kmemsysNumaRemoveAllMemory(pGpu, pKernelMemorySystem) kmemsysNumaRemoveAllMemory_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysNumaRemoveAllMemory_HAL(pGpu, pKernelMemorySystem) kmemsysNumaRemoveAllMemory_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSetupAllAtsPeers(pGpu, pKernelMemorySystem) kmemsysSetupAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysSetupAllAtsPeers_HAL(pGpu, pKernelMemorySystem) kmemsysSetupAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysRemoveAllAtsPeers(pGpu, pKernelMemorySystem) kmemsysRemoveAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysRemoveAllAtsPeers_HAL(pGpu, pKernelMemorySystem) kmemsysRemoveAllAtsPeers_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysAssertFbAckTimeoutPending(pGpu, pKernelMemorySystem) kmemsysAssertFbAckTimeoutPending_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysAssertFbAckTimeoutPending_HAL(pGpu, pKernelMemorySystem) kmemsysAssertFbAckTimeoutPending_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysCheckEccCounts(pGpu, pKernelMemorySystem) kmemsysCheckEccCounts_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysCheckEccCounts_HAL(pGpu, pKernelMemorySystem) kmemsysCheckEccCounts_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysClearEccCounts(pGpu, pKernelMemorySystem) kmemsysClearEccCounts_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysClearEccCounts_HAL(pGpu, pKernelMemorySystem) kmemsysClearEccCounts_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysStateLoad(pGpu, pEngstate, arg0) kmemsysStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStateUnload(pGpu, pEngstate, arg0) kmemsysStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStatePostUnload(pGpu, pEngstate, arg0) kmemsysStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStateInitUnlocked(pGpu, pEngstate) kmemsysStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysInitMissing(pGpu, pEngstate) kmemsysInitMissing_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePreInitLocked(pGpu, pEngstate) kmemsysStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePreInitUnlocked(pGpu, pEngstate) kmemsysStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysIsPresent(pGpu, pEngstate) kmemsysIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kmemsysGetUsableFbSize_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysGetUsableFbSize(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetUsableFbSize(pGpu, pKernelMemorySystem, pFbSize) kmemsysGetUsableFbSize_KERNEL(pGpu, pKernelMemorySystem, pFbSize)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize) kmemsysGetUsableFbSize(pGpu, pKernelMemorySystem, pFbSize)

static inline void kmemsysAssertSysmemFlushBufferValid_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysAssertSysmemFlushBufferValid_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysAssertSysmemFlushBufferValid_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysAssertSysmemFlushBufferValid(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysAssertSysmemFlushBufferValid(pGpu, pKernelMemorySystem) kmemsysAssertSysmemFlushBufferValid_b3696a(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysAssertSysmemFlushBufferValid_HAL(pGpu, pKernelMemorySystem) kmemsysAssertSysmemFlushBufferValid(pGpu, pKernelMemorySystem)

NvU32 kmemsysGetFlushSysmemBufferAddrShift_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvU32 kmemsysGetFlushSysmemBufferAddrShift(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return 0;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetFlushSysmemBufferAddrShift(pGpu, pKernelMemorySystem) kmemsysGetFlushSysmemBufferAddrShift_GM107(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysGetFlushSysmemBufferAddrShift_HAL(pGpu, pKernelMemorySystem) kmemsysGetFlushSysmemBufferAddrShift(pGpu, pKernelMemorySystem)

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

static inline NV_STATUS kmemsysPreFillCacheOnlyMemory_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1) {
    return NV_OK;
}

NV_STATUS kmemsysPreFillCacheOnlyMemory_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysPreFillCacheOnlyMemory(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPreFillCacheOnlyMemory(pGpu, pKernelMemorySystem, arg0, arg1) kmemsysPreFillCacheOnlyMemory_56cd7a(pGpu, pKernelMemorySystem, arg0, arg1)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem, arg0, arg1) kmemsysPreFillCacheOnlyMemory(pGpu, pKernelMemorySystem, arg0, arg1)

static inline NV_STATUS kmemsysCheckDisplayRemapperRange_14278f(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_INVALID_STATE);
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysCheckDisplayRemapperRange(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysCheckDisplayRemapperRange(pGpu, pKernelMemorySystem, arg0, arg1) kmemsysCheckDisplayRemapperRange_14278f(pGpu, pKernelMemorySystem, arg0, arg1)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysCheckDisplayRemapperRange_HAL(pGpu, pKernelMemorySystem, arg0, arg1) kmemsysCheckDisplayRemapperRange(pGpu, pKernelMemorySystem, arg0, arg1)

static inline void kmemsysPostHeapCreate_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysPostHeapCreate(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPostHeapCreate(pGpu, pKernelMemorySystem) kmemsysPostHeapCreate_b3696a(pGpu, pKernelMemorySystem)
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

NV_STATUS kmemsysAllocComprResources_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, FB_ALLOC_INFO *arg0, NvU64 arg1, NvU32 arg2, NvU32 *arg3, NvU32 arg4);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysAllocComprResources(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, FB_ALLOC_INFO *arg0, NvU64 arg1, NvU32 arg2, NvU32 *arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysAllocComprResources(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3, arg4) kmemsysAllocComprResources_KERNEL(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysAllocComprResources_HAL(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3, arg4) kmemsysAllocComprResources(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3, arg4)

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

NV_STATUS kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);


#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysPopulateMIGGPUInstanceMemConfig(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysPopulateMIGGPUInstanceMemConfig(pGpu, pKernelMemorySystem) kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysPopulateMIGGPUInstanceMemConfig_HAL(pGpu, pKernelMemorySystem) kmemsysPopulateMIGGPUInstanceMemConfig(pGpu, pKernelMemorySystem)

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

NV_STATUS kmemsysConstructEngine_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, ENGDESCRIPTOR arg0);

static inline NV_STATUS kmemsysConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, ENGDESCRIPTOR arg0) {
    return pKernelMemorySystem->__kmemsysConstructEngine__(pGpu, pKernelMemorySystem, arg0);
}

NV_STATUS kmemsysStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysStateInitLocked__(pGpu, pKernelMemorySystem);
}

NV_STATUS kmemsysStatePreLoad_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

static inline NV_STATUS kmemsysStatePreLoad_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__kmemsysStatePreLoad__(pGpu, pKernelMemorySystem, flags);
}

NV_STATUS kmemsysStatePostLoad_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

static inline NV_STATUS kmemsysStatePostLoad_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__kmemsysStatePostLoad__(pGpu, pKernelMemorySystem, flags);
}

NV_STATUS kmemsysStatePreUnload_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags);

static inline NV_STATUS kmemsysStatePreUnload_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 flags) {
    return pKernelMemorySystem->__kmemsysStatePreUnload__(pGpu, pKernelMemorySystem, flags);
}

void kmemsysStateDestroy_IMPL(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysStateDestroy__(pGpu, pKernelMemorySystem);
}

NV_STATUS kmemsysGetFbNumaInfo_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvS32 *numaNodeId);

static inline NV_STATUS kmemsysGetFbNumaInfo_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvS32 *numaNodeId) {
    return NV_OK;
}

static inline NV_STATUS kmemsysGetFbNumaInfo_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *physAddr, NvS32 *numaNodeId) {
    return pKernelMemorySystem->__kmemsysGetFbNumaInfo__(pGpu, pKernelMemorySystem, physAddr, numaNodeId);
}

NV_STATUS kmemsysReadUsableFbSize_GP102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);

NV_STATUS kmemsysReadUsableFbSize_GA102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize);

static inline NV_STATUS kmemsysReadUsableFbSize_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    return pKernelMemorySystem->__kmemsysReadUsableFbSize__(pGpu, pKernelMemorySystem, pFbSize);
}

NV_STATUS kmemsysCacheOp_GM200(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg0, FB_CACHE_MEMTYPE arg1, FB_CACHE_OP operation);

NV_STATUS kmemsysCacheOp_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg0, FB_CACHE_MEMTYPE arg1, FB_CACHE_OP operation);

static inline NV_STATUS kmemsysCacheOp_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg0, FB_CACHE_MEMTYPE arg1, FB_CACHE_OP operation) {
    return pKernelMemorySystem->__kmemsysCacheOp__(pGpu, pKernelMemorySystem, arg0, arg1, operation);
}

NV_STATUS kmemsysDoCacheOp_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0, NvU32 arg1, NvU32 arg2, PRMTIMEOUT arg3);

NV_STATUS kmemsysDoCacheOp_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0, NvU32 arg1, NvU32 arg2, PRMTIMEOUT arg3);

static inline NV_STATUS kmemsysDoCacheOp_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0, NvU32 arg1, NvU32 arg2, PRMTIMEOUT arg3) {
    return pKernelMemorySystem->__kmemsysDoCacheOp__(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3);
}

NvU32 kmemsysReadL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysReadL2SysmemInvalidateReg_68b109(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, -1);
}

static inline NvU32 kmemsysReadL2SysmemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadL2SysmemInvalidateReg__(pGpu, pKernelMemorySystem);
}

void kmemsysWriteL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0);

static inline void kmemsysWriteL2SysmemInvalidateReg_f2d351(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kmemsysWriteL2SysmemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    pKernelMemorySystem->__kmemsysWriteL2SysmemInvalidateReg__(pGpu, pKernelMemorySystem, arg0);
}

NvU32 kmemsysReadL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvU32 kmemsysReadL2PeermemInvalidateReg_68b109(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, -1);
}

static inline NvU32 kmemsysReadL2PeermemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadL2PeermemInvalidateReg__(pGpu, pKernelMemorySystem);
}

void kmemsysWriteL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0);

static inline void kmemsysWriteL2PeermemInvalidateReg_f2d351(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kmemsysWriteL2PeermemInvalidateReg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    pKernelMemorySystem->__kmemsysWriteL2PeermemInvalidateReg__(pGpu, pKernelMemorySystem, arg0);
}

NV_STATUS kmemsysInitFlushSysmemBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysInitFlushSysmemBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysInitFlushSysmemBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitFlushSysmemBuffer__(pGpu, pKernelMemorySystem);
}

void kmemsysProgramSysmemFlushBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysProgramSysmemFlushBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysProgramSysmemFlushBuffer__(pGpu, pKernelMemorySystem);
}

NvBool kmemsysIsPagePLCable_GA100(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize);

NvBool kmemsysIsPagePLCable_GA102(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize);

static inline NvBool kmemsysIsPagePLCable_510167(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 == 0)));
}

static inline NvBool kmemsysIsPagePLCable_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    return KernelMemorySystem->__kmemsysIsPagePLCable__(pGpu, KernelMemorySystem, physAddr, pageSize);
}

static inline NV_STATUS kmemsysReadMIGMemoryCfg_46f6a7(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kmemsysReadMIGMemoryCfg_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysReadMIGMemoryCfg_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysReadMIGMemoryCfg__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysInitMIGMemoryPartitionTable_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

NV_STATUS kmemsysInitMIGMemoryPartitionTable_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysInitMIGMemoryPartitionTable_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitMIGMemoryPartitionTable__(pGpu, pKernelMemorySystem);
}

NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments);

NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments);

static inline NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments) {
    return pKernelMemorySystem->__kmemsysSwizzIdToVmmuSegmentsRange__(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments);
}

NV_STATUS kmemsysNumaAddMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId);

static inline NV_STATUS kmemsysNumaAddMemory_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId) {
    return NV_OK;
}

static inline NV_STATUS kmemsysNumaAddMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU64 offset, NvU64 size, NvS32 *numaNodeId) {
    return pKernelMemorySystem->__kmemsysNumaAddMemory__(pGpu, pKernelMemorySystem, swizzId, offset, size, numaNodeId);
}

void kmemsysNumaRemoveMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId);

static inline void kmemsysNumaRemoveMemory_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId) {
    return;
}

static inline void kmemsysNumaRemoveMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId) {
    pKernelMemorySystem->__kmemsysNumaRemoveMemory__(pGpu, pKernelMemorySystem, swizzId);
}

void kmemsysNumaRemoveAllMemory_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysNumaRemoveAllMemory_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

static inline void kmemsysNumaRemoveAllMemory_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysNumaRemoveAllMemory__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysSetupAllAtsPeers_46f6a7(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kmemsysSetupAllAtsPeers_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysSetupAllAtsPeers_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysSetupAllAtsPeers__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysRemoveAllAtsPeers_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysRemoveAllAtsPeers_GV100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysRemoveAllAtsPeers_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysRemoveAllAtsPeers__(pGpu, pKernelMemorySystem);
}

NvBool kmemsysAssertFbAckTimeoutPending_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NvBool kmemsysAssertFbAckTimeoutPending_491d52(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmemsysAssertFbAckTimeoutPending_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysAssertFbAckTimeoutPending__(pGpu, pKernelMemorySystem);
}

void kmemsysCheckEccCounts_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysCheckEccCounts_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

static inline void kmemsysCheckEccCounts_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysCheckEccCounts__(pGpu, pKernelMemorySystem);
}

NV_STATUS kmemsysClearEccCounts_GH100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysClearEccCounts_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

static inline NV_STATUS kmemsysClearEccCounts_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysClearEccCounts__(pGpu, pKernelMemorySystem);
}

static inline NV_STATUS kmemsysStateLoad_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return pEngstate->__kmemsysStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmemsysStateUnload_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return pEngstate->__kmemsysStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmemsysStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return pEngstate->__kmemsysStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmemsysStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__kmemsysStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kmemsysInitMissing_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    pEngstate->__kmemsysInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmemsysStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__kmemsysStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmemsysStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__kmemsysStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool kmemsysIsPresent_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return pEngstate->__kmemsysIsPresent__(pGpu, pEngstate);
}

static inline NvBool kmemsysIsL2CleanFbPull(struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->bL2CleanFbPull;
}

void kmemsysDestruct_IMPL(struct KernelMemorySystem *pKernelMemorySystem);

#define __nvoc_kmemsysDestruct(pKernelMemorySystem) kmemsysDestruct_IMPL(pKernelMemorySystem)
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

NV_STATUS kmemsysGetMIGGPUInstanceMemConfigFromSwizzId_IMPL(OBJGPU *arg0, struct KernelMemorySystem *arg1, NvU32 swizzId, const MIG_GPU_INSTANCE_MEMORY_CONFIG **arg2);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysGetMIGGPUInstanceMemConfigFromSwizzId(OBJGPU *arg0, struct KernelMemorySystem *arg1, NvU32 swizzId, const MIG_GPU_INSTANCE_MEMORY_CONFIG **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysGetMIGGPUInstanceMemConfigFromSwizzId(arg0, arg1, swizzId, arg2) kmemsysGetMIGGPUInstanceMemConfigFromSwizzId_IMPL(arg0, arg1, swizzId, arg2)
#endif //__nvoc_kern_mem_sys_h_disabled

NV_STATUS kmemsysInitMIGGPUInstanceMemConfigForSwizzId_IMPL(OBJGPU *arg0, struct KernelMemorySystem *arg1, NvU32 swizzId, NvU64 startingVmmuSegment, NvU64 memSizeInVmmuSegment);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysInitMIGGPUInstanceMemConfigForSwizzId(OBJGPU *arg0, struct KernelMemorySystem *arg1, NvU32 swizzId, NvU64 startingVmmuSegment, NvU64 memSizeInVmmuSegment) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysInitMIGGPUInstanceMemConfigForSwizzId(arg0, arg1, swizzId, startingVmmuSegment, memSizeInVmmuSegment) kmemsysInitMIGGPUInstanceMemConfigForSwizzId_IMPL(arg0, arg1, swizzId, startingVmmuSegment, memSizeInVmmuSegment)
#endif //__nvoc_kern_mem_sys_h_disabled

#undef PRIVATE_FIELD


#define IS_COHERENT_CPU_ATS_OFFSET(kmemsys, offset, length)              \
    (kmemsys && ((offset) >= kmemsys->coherentCpuFbBase) &&              \
     (((NvU64)offset + size) <= kmemsys->coherentCpuFbEnd))

#endif // KERN_MEM_SYS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_MEM_SYS_NVOC_H_
