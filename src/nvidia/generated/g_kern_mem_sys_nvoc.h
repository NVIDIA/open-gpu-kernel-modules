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

#define FB_SET_HWRESID_CTAGID_FERMI(h, i)     \
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
    void (*__kmemsysStateDestroy__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysGetFbNumaInfo__)(OBJGPU *, struct KernelMemorySystem *, NvU64 *, NvS32 *);
    NV_STATUS (*__kmemsysReadUsableFbSize__)(OBJGPU *, struct KernelMemorySystem *, NvU64 *);
    NV_STATUS (*__kmemsysInitFlushSysmemBuffer__)(OBJGPU *, struct KernelMemorySystem *);
    void (*__kmemsysProgramSysmemFlushBuffer__)(OBJGPU *, struct KernelMemorySystem *);
    NvBool (*__kmemsysIsPagePLCable__)(OBJGPU *, struct KernelMemorySystem *, NvU64, NvU64);
    NV_STATUS (*__kmemsysReadMIGMemoryCfg__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysInitMIGMemoryPartitionTable__)(OBJGPU *, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysReconcileTunableState__)(POBJGPU, struct KernelMemorySystem *, void *);
    NV_STATUS (*__kmemsysStateLoad__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStateUnload__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStatePostUnload__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStatePreUnload__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysStateInitUnlocked__)(POBJGPU, struct KernelMemorySystem *);
    void (*__kmemsysInitMissing__)(POBJGPU, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStatePreInitLocked__)(POBJGPU, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysStatePreInitUnlocked__)(POBJGPU, struct KernelMemorySystem *);
    NV_STATUS (*__kmemsysGetTunableState__)(POBJGPU, struct KernelMemorySystem *, void *);
    NV_STATUS (*__kmemsysCompareTunableState__)(POBJGPU, struct KernelMemorySystem *, void *, void *);
    void (*__kmemsysFreeTunableState__)(POBJGPU, struct KernelMemorySystem *, void *);
    NV_STATUS (*__kmemsysStatePostLoad__)(POBJGPU, struct KernelMemorySystem *, NvU32);
    NV_STATUS (*__kmemsysAllocTunableState__)(POBJGPU, struct KernelMemorySystem *, void **);
    NV_STATUS (*__kmemsysSetTunableState__)(POBJGPU, struct KernelMemorySystem *, void *);
    NvBool (*__kmemsysIsPresent__)(POBJGPU, struct KernelMemorySystem *);
    NvBool bDisableTiledCachingInvalidatesWithEccBug1521641;
    NvBool bGpuCacheEnable;
    NvBool bNumaNodesAdded;
    NvBool bL2CleanFbPull;
    NvBool bPreserveComptagBackingStoreOnSuspend;
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
#define kmemsysStateDestroy(pGpu, pKernelMemorySystem) kmemsysStateDestroy_DISPATCH(pGpu, pKernelMemorySystem)
#define kmemsysGetFbNumaInfo(pGpu, pKernelMemorySystem, physAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, numaNodeId)
#define kmemsysGetFbNumaInfo_HAL(pGpu, pKernelMemorySystem, physAddr, numaNodeId) kmemsysGetFbNumaInfo_DISPATCH(pGpu, pKernelMemorySystem, physAddr, numaNodeId)
#define kmemsysReadUsableFbSize(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
#define kmemsysReadUsableFbSize_HAL(pGpu, pKernelMemorySystem, pFbSize) kmemsysReadUsableFbSize_DISPATCH(pGpu, pKernelMemorySystem, pFbSize)
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
#define kmemsysReconcileTunableState(pGpu, pEngstate, pTunableState) kmemsysReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kmemsysStateLoad(pGpu, pEngstate, arg0) kmemsysStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStateUnload(pGpu, pEngstate, arg0) kmemsysStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStatePostUnload(pGpu, pEngstate, arg0) kmemsysStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStatePreUnload(pGpu, pEngstate, arg0) kmemsysStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysStateInitUnlocked(pGpu, pEngstate) kmemsysStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysInitMissing(pGpu, pEngstate) kmemsysInitMissing_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePreInitLocked(pGpu, pEngstate) kmemsysStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmemsysStatePreInitUnlocked(pGpu, pEngstate) kmemsysStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmemsysGetTunableState(pGpu, pEngstate, pTunableState) kmemsysGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kmemsysCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) kmemsysCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define kmemsysFreeTunableState(pGpu, pEngstate, pTunableState) kmemsysFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kmemsysStatePostLoad(pGpu, pEngstate, arg0) kmemsysStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmemsysAllocTunableState(pGpu, pEngstate, ppTunableState) kmemsysAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define kmemsysSetTunableState(pGpu, pEngstate, pTunableState) kmemsysSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
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

NV_STATUS kmemsysCacheOp_GM200(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg0, FB_CACHE_MEMTYPE arg1, FB_CACHE_OP operation);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysCacheOp(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, PMEMORY_DESCRIPTOR arg0, FB_CACHE_MEMTYPE arg1, FB_CACHE_OP operation) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, operation) kmemsysCacheOp_GM200(pGpu, pKernelMemorySystem, arg0, arg1, operation)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, arg0, arg1, operation) kmemsysCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, operation)

NV_STATUS kmemsysDoCacheOp_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0, NvU32 arg1, NvU32 arg2, PRMTIMEOUT arg3);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysDoCacheOp(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0, NvU32 arg1, NvU32 arg2, PRMTIMEOUT arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysDoCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3) kmemsysDoCacheOp_GM107(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3) kmemsysDoCacheOp(pGpu, pKernelMemorySystem, arg0, arg1, arg2, arg3)

NvU32 kmemsysReadL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvU32 kmemsysReadL2SysmemInvalidateReg(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return 0;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysReadL2SysmemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg_TU102(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysReadL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2SysmemInvalidateReg(pGpu, pKernelMemorySystem)

void kmemsysWriteL2SysmemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysWriteL2SysmemInvalidateReg(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysWriteL2SysmemInvalidateReg(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2SysmemInvalidateReg_TU102(pGpu, pKernelMemorySystem, arg0)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysWriteL2SysmemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2SysmemInvalidateReg(pGpu, pKernelMemorySystem, arg0)

NvU32 kmemsysReadL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NvU32 kmemsysReadL2PeermemInvalidateReg(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return 0;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysReadL2PeermemInvalidateReg(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg_TU102(pGpu, pKernelMemorySystem)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysReadL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem) kmemsysReadL2PeermemInvalidateReg(pGpu, pKernelMemorySystem)

void kmemsysWriteL2PeermemInvalidateReg_TU102(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline void kmemsysWriteL2PeermemInvalidateReg(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysWriteL2PeermemInvalidateReg(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2PeermemInvalidateReg_TU102(pGpu, pKernelMemorySystem, arg0)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysWriteL2PeermemInvalidateReg_HAL(pGpu, pKernelMemorySystem, arg0) kmemsysWriteL2PeermemInvalidateReg(pGpu, pKernelMemorySystem, arg0)

static inline void kmemsysAssertSysmemFlushBufferValid_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

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

static inline NV_STATUS kmemsysPreFillCacheOnlyMemory_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 arg0, NvU64 arg1) {
    return NV_OK;
}

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

NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments);

#ifdef __nvoc_kern_mem_sys_h_disabled
static inline NV_STATUS kmemsysSwizzIdToVmmuSegmentsRange(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU32 swizzId, NvU32 vmmuSegmentSize, NvU32 totalVmmuSegments) {
    NV_ASSERT_FAILED_PRECOMP("KernelMemorySystem was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_mem_sys_h_disabled
#define kmemsysSwizzIdToVmmuSegmentsRange(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange_GA100(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)
#endif //__nvoc_kern_mem_sys_h_disabled

#define kmemsysSwizzIdToVmmuSegmentsRange_HAL(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments) kmemsysSwizzIdToVmmuSegmentsRange(pGpu, pKernelMemorySystem, swizzId, vmmuSegmentSize, totalVmmuSegments)

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

static inline NV_STATUS kmemsysReadUsableFbSize_5baef9(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kmemsysReadUsableFbSize_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem, NvU64 *pFbSize) {
    return pKernelMemorySystem->__kmemsysReadUsableFbSize__(pGpu, pKernelMemorySystem, pFbSize);
}

static inline NV_STATUS kmemsysInitFlushSysmemBuffer_56cd7a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return NV_OK;
}

NV_STATUS kmemsysInitFlushSysmemBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

NV_STATUS kmemsysInitFlushSysmemBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline NV_STATUS kmemsysInitFlushSysmemBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return pKernelMemorySystem->__kmemsysInitFlushSysmemBuffer__(pGpu, pKernelMemorySystem);
}

static inline void kmemsysProgramSysmemFlushBuffer_b3696a(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    return;
}

void kmemsysProgramSysmemFlushBuffer_GM107(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

void kmemsysProgramSysmemFlushBuffer_GA100(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem);

static inline void kmemsysProgramSysmemFlushBuffer_DISPATCH(OBJGPU *pGpu, struct KernelMemorySystem *pKernelMemorySystem) {
    pKernelMemorySystem->__kmemsysProgramSysmemFlushBuffer__(pGpu, pKernelMemorySystem);
}

static inline NvBool kmemsysIsPagePLCable_cbe027(OBJGPU *pGpu, struct KernelMemorySystem *KernelMemorySystem, NvU64 physAddr, NvU64 pageSize) {
    return ((NvBool)(0 == 0));
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

static inline NV_STATUS kmemsysReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return pEngstate->__kmemsysReconcileTunableState__(pGpu, pEngstate, pTunableState);
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

static inline NV_STATUS kmemsysStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return pEngstate->__kmemsysStatePreUnload__(pGpu, pEngstate, arg0);
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

static inline NV_STATUS kmemsysGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return pEngstate->__kmemsysGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kmemsysCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__kmemsysCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline void kmemsysFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    pEngstate->__kmemsysFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kmemsysStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return pEngstate->__kmemsysStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmemsysAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void **ppTunableState) {
    return pEngstate->__kmemsysAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS kmemsysSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return pEngstate->__kmemsysSetTunableState__(pGpu, pEngstate, pTunableState);
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
    (kmemsys && ((offset) >= kmemsys->coherentCpuFbBase) &&               \
     (((NvU64)offset + size) <= kmemsys->coherentCpuFbEnd))

#endif // KERN_MEM_SYS_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERN_MEM_SYS_NVOC_H_
