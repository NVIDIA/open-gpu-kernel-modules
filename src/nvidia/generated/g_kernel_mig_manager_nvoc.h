#ifndef _G_KERNEL_MIG_MANAGER_NVOC_H_
#define _G_KERNEL_MIG_MANAGER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_mig_manager_nvoc.h"

#ifndef KERNEL_MIG_MANAGER_H
#define KERNEL_MIG_MANAGER_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/nvbitmask.h"

#include "ctrl/ctrlc637.h"

typedef struct KERNEL_MIG_GPU_INSTANCE KERNEL_MIG_GPU_INSTANCE;

// Forward declaration of opaque type
typedef struct KERNEL_MIG_MANAGER_PRIVATE_DATA KERNEL_MIG_MANAGER_PRIVATE_DATA;
typedef struct MIG_GPU_INSTANCE MIG_GPU_INSTANCE;

#define  IS_MIG_ENABLED(pGpu) (((pGpu) != NULL) && (GPU_GET_KERNEL_MIG_MANAGER(pGpu) != NULL) && \
                               kmigmgrIsMIGEnabled((pGpu), GPU_GET_KERNEL_MIG_MANAGER(pGpu)))
#define  IS_MIG_IN_USE(pGpu)  (((pGpu) != NULL) && (GPU_GET_KERNEL_MIG_MANAGER(pGpu) != NULL) && \
                               kmigmgrIsMIGGpuInstancingEnabled((pGpu), GPU_GET_KERNEL_MIG_MANAGER(pGpu)))

#define FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pLocal)                 \
    {                                                                                \
        NvU32 i;                                                                     \
        for (i = 0; i < KMIGMGR_MAX_GPU_INSTANCES; ++i)                              \
        {                                                                            \
            (pLocal) = kmigmgrGetMIGGpuInstanceSlot((pGpu), (pKernelMIGManager), i); \
            if (((pLocal) == NULL) || !(pLocal)->bValid)                             \
                continue;

#define FOR_EACH_VALID_GPU_INSTANCE_END()                                           \
        }                                                                           \
    }

#define KMIGMGR_SWIZZID_INVALID             0xFFFFFFFF
#define KMIGMGR_MAX_GPU_SWIZZID             15
#define KMIGMGR_MAX_GPU_INSTANCES           GPUMGR_MAX_GPU_INSTANCES
#define KMIGMGR_MAX_COMPUTE_INSTANCES       GPUMGR_MAX_COMPUTE_INSTANCES
#define KMIGMGR_COMPUTE_INSTANCE_ID_INVALID 0xFFFFFFFF
#define KMIGMGR_COMPUTE_SIZE_INVALID        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE
#define KMIGMGR_MAX_GPU_CTSID               21
#define KMIGMGR_CTSID_INVALID               0xFFFFFFFFUL
#define KMIGMGR_SPAN_OFFSET_INVALID         KMIGMGR_CTSID_INVALID

#define KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID            \
    ((KMIGMGR_MAX_GPU_SWIZZID * KMIGMGR_MAX_GPU_SWIZZID) + \
     KMIGMGR_MAX_COMPUTE_INSTANCES)

MAKE_BITVECTOR(GFID_BIT_VECTOR, VMMU_MAX_GFID);

typedef struct KMIGMGR_INSTANCE_HANDLES
{
    /*!
     * Client handle to make calls into this instance
     */
    NvHandle hClient;

    /*!
     * Device handle to make calls into this instance
     */
    NvHandle hDevice;

    /*!
     * Subdevice handle to make calls into this instance
     */
    NvHandle hSubdevice;

    /*!
     * Subscription handle to make calls into this instance
     */
    NvHandle hSubscription;
} KMIGMGR_INSTANCE_HANDLES;

typedef struct MIG_RESOURCE_ALLOCATION
{
    /*!
     * Logical GPC-IDs which are associated with this instance
     * As current assumption is that GPCs within a instance is always
     * physically contiguous, so we can use start and count also saving some
     * memory however it will enforce contiguity restriction which may not be
     * in future.
     */
    NvU32 gpcIds[KGRMGR_MAX_GPC];

    /*!
     * Number of GPCs associated with this instance
     */
    NvU32 gpcCount;

    /*!
     * Number of GFX GPCs associated with this instance. This should be a subset of gpcs included in gpcCount.
     */
    NvU32 gfxGpcCount;

    /*!
     * VEID start offset for this instance
     */
    NvU32 veidOffset;

    /*!
     * Number of VEIDs associated with this instance
     */
    NvU32 veidCount;

    /*!
     * Bitvector of partitionable engines associated with this instance.
     */
    ENGTYPE_BIT_VECTOR engines;

    /*!
     * Bitvector of local engine IDs associated with this instance.
     */
    ENGTYPE_BIT_VECTOR localEngines;

    /*!
     * Virtualized GPC Count
    */
    NvU32 virtualGpcCount;

    /*!
     * Number of SMs
     */
    NvU32 smCount;
} MIG_RESOURCE_ALLOCATION;

typedef struct MIG_COMPUTE_INSTANCE
{
    /*!
     * Resource allocated for this instance
     */
    MIG_RESOURCE_ALLOCATION resourceAllocation;

    /*!
     * States that this is a valid compute instance
     */
    NvBool bValid;

    /*!
     * Flags indicating which engines (if any) are shared across multiple compute
     * instances. Bit positions in this flag correspond to
     * NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_*
     */
    NvU32 sharedEngFlag;

    /*!
     * Compute instance ID
     */
    NvU32 id;

    /*!
     * Shared object to track instance reference count
     */
    struct RsShared *pShare;

    /*!
     * Opaque pointer to os-specific capabilities
     */
    OS_RM_CAPS *pOsRmCaps;

    /*!
     * Compute instance UUID
     */
    NvUuid uuid;

    /*!
     * Handles for RPC's into this instance
     */
    KMIGMGR_INSTANCE_HANDLES instanceHandles;

    /*!
     * Span start of this compute instance indicating the "position" of the
     * instance within a GPU instance's view. For non-CTS ID enabled chips,
     * this corresponds to the start of a VEID segment. For CTS-ID chips, this
     * corresponds to the offset from the first CTS ID of a given profile size.
     */
    NvU32 spanStart;

    /*!
     * Compute Profile size associated with this MIG compute instance
     * To associate an instance with a given compute profile, since a CTS
     * ID may not have been assigned.
     */
    NvU32 computeSize;
} MIG_COMPUTE_INSTANCE;

/*!
 * @brief Situational params for compute instance creation API
 *
 * This structure comes with two specializations:
 *  TYPE_REQUEST
 *      Parameter refers to request data passed in via EXEC_PARTITIONS_CREATE ctrl
 *      call. All resources claimed by new compute instance are chosen via allocator,
 *      and the API may create multiple compute instances.
 *  TYPE_RESTORE
 *      Parameter refers to saved compute instance data. Most resources claimed by new
 *      compute instance are determined by the save data, and others are claimed via
 *      allocator.
 *  requestFlags
 *  TYPE_REQUEST_WITH_IDS
 *      Parameter refers to request data passed in via EXEC_PARTITIONS_CREATE ctrl
 *          call. All resources claimed by new instance are chosen via allocator unless
 *          the _AT_SPAN flag is also specified.
 *      RM also tries to allocate instance with compute instance id
 *      requested by user. This flag is only supported on vGPU enabled RM build
 *      and will be removed when vgpu plugin implements virtualized compute
 *      instance ID support. (bug 2938187)
 *      TYPE_REQUEST_AT_SPAN
 *          Parameter refers to request data passed in via EXEC_PARTITIONS_CREATE ctrl
 *          call. All resources claimed by new instance are attempt to be claimed by
 *          the RM allocater starting at the specified resource span.
 */
typedef struct KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS
{
    enum
    {
        KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST,
        KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE
    } type;
    union
    {
        struct
        {
            NvU32 count;
            NVC637_CTRL_EXEC_PARTITIONS_INFO *pReqComputeInstanceInfo;
            NvU32 requestFlags;
        } request;
        struct
        {
            struct GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave;
        } restore;
    } inst;
} KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS;

typedef struct KMIGMGR_CONFIGURE_INSTANCE_PARAMS
{
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE profile;
    NvU32 ctsId;
    NvU32 veidSpanStart;
} KMIGMGR_CONFIGURE_INSTANCE_REQUEST;

typedef struct KERNEL_MIG_GPU_INSTANCE
{
    /*! Structure containing GPU instance profile */
    const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO *pProfile;

    /*!
     * Resource allocated for this instance
     */
    MIG_RESOURCE_ALLOCATION resourceAllocation;

    /*!
     * Mask of physical engines in this GPU instance which are assigned exclusively
     * to some compute instance. Indexed via RM_ENGINE_TYPE_*
     */
    ENGTYPE_BIT_VECTOR exclusiveEngMask;

    /*!
     * Mask of physical engines in this GPU instance which are assigned to at least
     * one compute instance, but may be assigned to others.
     * Indexed via RM_ENGINE_TYPE_*
     */
    ENGTYPE_BIT_VECTOR sharedEngMask;

    /*!
     * compute instance info.
     */
    MIG_COMPUTE_INSTANCE MIGComputeInstance[KMIGMGR_MAX_COMPUTE_INSTANCES];

    /*!
     * Bit Vector of GFID's associated with this instance.
     */
    GFID_BIT_VECTOR gfidMap;

    /*!
     * GPU instance ID
     */
    NvU32 swizzId;

    /*!
     * Validated user-provided instance flags - NV2080_CTRL_GPU_PARTITION_FLAG_*
     */
    NvU32 partitionFlag;

    /*!
     * Memory handle associated with partitioned memory
     */
    NvHandle hMemory;

    /*!
     * Shared object to track instance reference count
     */
    struct RsShared *pShare;

    /*!
     * Heap used for managing instance's memory
     */
    struct Heap *pMemoryPartitionHeap;

    /*!
     * States that this instance is valid
     */
    NvBool bValid;

    /*!
     * Indicates that the GPU instance scrubber is initialized and should be
     * accounted for / ignored in the instance refcount when determining
     * whether or not a instance can be destroyed.
     */
    NvBool bMemoryPartitionScrubberInitialized;

    /*!
     * Physical memory address range for this instance.
     */
    NV_RANGE memRange;

    /*!
     * Memory pool for client page table allocations
     */
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pPageTableMemPool;

    /*!
     * Physical MIG GPU Instance info for this instance
     */
    MIG_GPU_INSTANCE *pMIGGpuInstance;

    /*!
     * Mask of runlistIds for engines that belong to this instance
     */
    NvU64 runlistIdMask;

    /*!
     * Opaque pointer to os-specific capabilities
     */
    OS_RM_CAPS *pOsRmCaps;

    /*!
     * Handles for RPC's into this instance
     */
    KMIGMGR_INSTANCE_HANDLES instanceHandles;

    /*!
     * Mask of CTS IDs in use
     */
    NvU64 ctsIdsInUseMask;

    /*!
     * GR to CTS ID mapping
     */
    NvU32 grCtsIdMap[KMIGMGR_MAX_COMPUTE_INSTANCES];

    /*!
     * Mask tracking which compute spans are currently in-use
     */
    NvU32 spanInUseMask;
} KERNEL_MIG_GPU_INSTANCE;

/*!
 * @brief Situational params for GPU instance creation API
 *
 * This structure comes with two specializations:
 *  TYPE_REQUEST
 *      Parameter refers to request data passed in via SET_PARTITIONS ctrl
 *      call. All resources claimed by new GPU instance are chosen via allocator.
 *  TYPE_RESTORE
 *      Parameter refers to saved GPU instance data. Most resources claimed by new
 *      GPU instance are determined by the save data, and others are claimed via
 *      allocator.
 */
typedef struct KMIGMGR_CREATE_GPU_INSTANCE_PARAMS
{
    enum
    {
        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_REQUEST,
        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_RESTORE
    } type;
    union
    {
        struct
        {
            NvU32    partitionFlag;
            NV_RANGE placement;
            NvBool   bUsePlacement;
        } request;
        struct
        {
            struct GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave;
        } restore;
    } inst;
} KMIGMGR_CREATE_GPU_INSTANCE_PARAMS;

/*!
 * @brief Packed pointer to a GPU instance/compute instance combo
 * @note  Having NULL pKernelMIGGpuInstance and non-NULL pMIGComputeInstance is never expected
 */
struct MIG_INSTANCE_REF
{
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
};

typedef struct KERNEL_MIG_MANAGER_STATIC_INFO
{
    /*! @ref NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_PROFILES */
    NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pProfiles;

    /*! Mask of partitionable engines which are present on this GPU. */
    NvU32 partitionableEngineMask[NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX];

    /*! Per swizzId FB memory page ranges */
    NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pSwizzIdFbMemPageRanges;

    /*! Compute instance profiles */
    NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *pCIProfiles;

    /*! Skyline info used to determine GPU and compute instance resources available */
    NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS *pSkylineInfo;
} KERNEL_MIG_MANAGER_STATIC_INFO;

/*!
 * KernelMIGManager provides kernel side services for managing MIG instances.
 * It also maintains state relating to GPU partitioning and related state.
 */
#ifdef NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelMIGManager {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelMIGManager *__nvoc_pbase_KernelMIGManager;
    NV_STATUS (*__kmigmgrConstructEngine__)(OBJGPU *, struct KernelMIGManager *, ENGDESCRIPTOR);
    NV_STATUS (*__kmigmgrStateInitLocked__)(OBJGPU *, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStateUnload__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrCreateGPUInstanceCheck__)(OBJGPU *, struct KernelMIGManager *, NvBool);
    NvBool (*__kmigmgrIsDevinitMIGBitSet__)(OBJGPU *, struct KernelMIGManager *);
    NvBool (*__kmigmgrIsGPUInstanceCombinationValid__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsGPUInstanceFlagValid__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsMemoryPartitioningRequested__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsMemoryPartitioningNeeded__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    struct NV_RANGE (*__kmigmgrMemSizeFlagToSwizzIdRange__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStateLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStatePreLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStatePostUnload__)(POBJGPU, struct KernelMIGManager *, NvU32);
    void (*__kmigmgrStateDestroy__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreUnload__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStateInitUnlocked__)(POBJGPU, struct KernelMIGManager *);
    void (*__kmigmgrInitMissing__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreInitLocked__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreInitUnlocked__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePostLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsPresent__)(POBJGPU, struct KernelMIGManager *);
    NvBool PRIVATE_FIELD(bIsA100ReducedConfig);
    KERNEL_MIG_MANAGER_PRIVATE_DATA *PRIVATE_FIELD(pPrivate);
    KERNEL_MIG_GPU_INSTANCE PRIVATE_FIELD(kernelMIGGpuInstance)[8];
    NvBool PRIVATE_FIELD(bMIGEnabled);
    NvU64 PRIVATE_FIELD(swizzIdInUseMask);
    NvBool PRIVATE_FIELD(bRestoreWatchdog);
    NvBool PRIVATE_FIELD(bReenableWatchdog);
    union ENGTYPE_BIT_VECTOR PRIVATE_FIELD(partitionableEnginesInUse);
    NvBool PRIVATE_FIELD(bDeviceProfilingInUse);
    NvBool PRIVATE_FIELD(bMIGAutoOnlineEnabled);
    NvBool PRIVATE_FIELD(bBootConfigSupported);
    NvBool PRIVATE_FIELD(bAutoUpdateBootConfig);
};

struct KernelMIGManager_PRIVATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelMIGManager *__nvoc_pbase_KernelMIGManager;
    NV_STATUS (*__kmigmgrConstructEngine__)(OBJGPU *, struct KernelMIGManager *, ENGDESCRIPTOR);
    NV_STATUS (*__kmigmgrStateInitLocked__)(OBJGPU *, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStateUnload__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrCreateGPUInstanceCheck__)(OBJGPU *, struct KernelMIGManager *, NvBool);
    NvBool (*__kmigmgrIsDevinitMIGBitSet__)(OBJGPU *, struct KernelMIGManager *);
    NvBool (*__kmigmgrIsGPUInstanceCombinationValid__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsGPUInstanceFlagValid__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsMemoryPartitioningRequested__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsMemoryPartitioningNeeded__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    struct NV_RANGE (*__kmigmgrMemSizeFlagToSwizzIdRange__)(OBJGPU *, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStateLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStatePreLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStatePostUnload__)(POBJGPU, struct KernelMIGManager *, NvU32);
    void (*__kmigmgrStateDestroy__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreUnload__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NV_STATUS (*__kmigmgrStateInitUnlocked__)(POBJGPU, struct KernelMIGManager *);
    void (*__kmigmgrInitMissing__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreInitLocked__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePreInitUnlocked__)(POBJGPU, struct KernelMIGManager *);
    NV_STATUS (*__kmigmgrStatePostLoad__)(POBJGPU, struct KernelMIGManager *, NvU32);
    NvBool (*__kmigmgrIsPresent__)(POBJGPU, struct KernelMIGManager *);
    NvBool bIsA100ReducedConfig;
    KERNEL_MIG_MANAGER_PRIVATE_DATA *pPrivate;
    KERNEL_MIG_GPU_INSTANCE kernelMIGGpuInstance[8];
    NvBool bMIGEnabled;
    NvU64 swizzIdInUseMask;
    NvBool bRestoreWatchdog;
    NvBool bReenableWatchdog;
    union ENGTYPE_BIT_VECTOR partitionableEnginesInUse;
    NvBool bDeviceProfilingInUse;
    NvBool bMIGAutoOnlineEnabled;
    NvBool bBootConfigSupported;
    NvBool bAutoUpdateBootConfig;
};

#ifndef __NVOC_CLASS_KernelMIGManager_TYPEDEF__
#define __NVOC_CLASS_KernelMIGManager_TYPEDEF__
typedef struct KernelMIGManager KernelMIGManager;
#endif /* __NVOC_CLASS_KernelMIGManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMIGManager
#define __nvoc_class_id_KernelMIGManager 0x01c1bf
#endif /* __nvoc_class_id_KernelMIGManager */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager;

#define __staticCast_KernelMIGManager(pThis) \
    ((pThis)->__nvoc_pbase_KernelMIGManager)

#ifdef __nvoc_kernel_mig_manager_h_disabled
#define __dynamicCast_KernelMIGManager(pThis) ((KernelMIGManager*)NULL)
#else //__nvoc_kernel_mig_manager_h_disabled
#define __dynamicCast_KernelMIGManager(pThis) \
    ((KernelMIGManager*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMIGManager)))
#endif //__nvoc_kernel_mig_manager_h_disabled

#define PDB_PROP_KMIGMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMIGMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMIGManager(KernelMIGManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMIGManager(KernelMIGManager**, Dynamic*, NvU32);
#define __objCreate_KernelMIGManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMIGManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kmigmgrConstructEngine(arg0, arg1, arg2) kmigmgrConstructEngine_DISPATCH(arg0, arg1, arg2)
#define kmigmgrStateInitLocked(arg0, arg1) kmigmgrStateInitLocked_DISPATCH(arg0, arg1)
#define kmigmgrStateUnload(arg0, arg1, flags) kmigmgrStateUnload_DISPATCH(arg0, arg1, flags)
#define kmigmgrCreateGPUInstanceCheck(arg0, arg1, bMemoryPartitioningNeeded) kmigmgrCreateGPUInstanceCheck_DISPATCH(arg0, arg1, bMemoryPartitioningNeeded)
#define kmigmgrCreateGPUInstanceCheck_HAL(arg0, arg1, bMemoryPartitioningNeeded) kmigmgrCreateGPUInstanceCheck_DISPATCH(arg0, arg1, bMemoryPartitioningNeeded)
#define kmigmgrIsDevinitMIGBitSet(arg0, arg1) kmigmgrIsDevinitMIGBitSet_DISPATCH(arg0, arg1)
#define kmigmgrIsDevinitMIGBitSet_HAL(arg0, arg1) kmigmgrIsDevinitMIGBitSet_DISPATCH(arg0, arg1)
#define kmigmgrIsGPUInstanceCombinationValid(arg0, arg1, gpuInstanceFlag) kmigmgrIsGPUInstanceCombinationValid_DISPATCH(arg0, arg1, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceCombinationValid_HAL(arg0, arg1, gpuInstanceFlag) kmigmgrIsGPUInstanceCombinationValid_DISPATCH(arg0, arg1, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceFlagValid(arg0, arg1, gpuInstanceFlag) kmigmgrIsGPUInstanceFlagValid_DISPATCH(arg0, arg1, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceFlagValid_HAL(arg0, arg1, gpuInstanceFlag) kmigmgrIsGPUInstanceFlagValid_DISPATCH(arg0, arg1, gpuInstanceFlag)
#define kmigmgrIsMemoryPartitioningRequested(arg0, arg1, partitionFlags) kmigmgrIsMemoryPartitioningRequested_DISPATCH(arg0, arg1, partitionFlags)
#define kmigmgrIsMemoryPartitioningRequested_HAL(arg0, arg1, partitionFlags) kmigmgrIsMemoryPartitioningRequested_DISPATCH(arg0, arg1, partitionFlags)
#define kmigmgrIsMemoryPartitioningNeeded(arg0, arg1, swizzId) kmigmgrIsMemoryPartitioningNeeded_DISPATCH(arg0, arg1, swizzId)
#define kmigmgrIsMemoryPartitioningNeeded_HAL(arg0, arg1, swizzId) kmigmgrIsMemoryPartitioningNeeded_DISPATCH(arg0, arg1, swizzId)
#define kmigmgrMemSizeFlagToSwizzIdRange(arg0, arg1, memSizeFlag) kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(arg0, arg1, memSizeFlag)
#define kmigmgrMemSizeFlagToSwizzIdRange_HAL(arg0, arg1, memSizeFlag) kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(arg0, arg1, memSizeFlag)
#define kmigmgrStateLoad(pGpu, pEngstate, arg0) kmigmgrStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmigmgrStatePreLoad(pGpu, pEngstate, arg0) kmigmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmigmgrStatePostUnload(pGpu, pEngstate, arg0) kmigmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmigmgrStateDestroy(pGpu, pEngstate) kmigmgrStateDestroy_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreUnload(pGpu, pEngstate, arg0) kmigmgrStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmigmgrStateInitUnlocked(pGpu, pEngstate) kmigmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrInitMissing(pGpu, pEngstate) kmigmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreInitLocked(pGpu, pEngstate) kmigmgrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreInitUnlocked(pGpu, pEngstate) kmigmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePostLoad(pGpu, pEngstate, arg0) kmigmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmigmgrIsPresent(pGpu, pEngstate) kmigmgrIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kmigmgrLoadStaticInfo_KERNEL(OBJGPU *arg0, struct KernelMIGManager *arg1);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrLoadStaticInfo(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrLoadStaticInfo(arg0, arg1) kmigmgrLoadStaticInfo_KERNEL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrLoadStaticInfo_HAL(arg0, arg1) kmigmgrLoadStaticInfo(arg0, arg1)

static inline NV_STATUS kmigmgrSetStaticInfo_46f6a7(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetStaticInfo(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetStaticInfo(arg0, arg1) kmigmgrSetStaticInfo_46f6a7(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrSetStaticInfo_HAL(arg0, arg1) kmigmgrSetStaticInfo(arg0, arg1)

static inline void kmigmgrClearStaticInfo_b3696a(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return;
}


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrClearStaticInfo(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearStaticInfo(arg0, arg1) kmigmgrClearStaticInfo_b3696a(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrClearStaticInfo_HAL(arg0, arg1) kmigmgrClearStaticInfo(arg0, arg1)

static inline NV_STATUS kmigmgrSaveToPersistenceFromVgpuStaticInfo_46f6a7(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSaveToPersistenceFromVgpuStaticInfo(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSaveToPersistenceFromVgpuStaticInfo(arg0, arg1) kmigmgrSaveToPersistenceFromVgpuStaticInfo_46f6a7(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrSaveToPersistenceFromVgpuStaticInfo_HAL(arg0, arg1) kmigmgrSaveToPersistenceFromVgpuStaticInfo(arg0, arg1)

NV_STATUS kmigmgrDeleteGPUInstanceRunlists_FWCLIENT(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDeleteGPUInstanceRunlists(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDeleteGPUInstanceRunlists(arg0, arg1, arg2) kmigmgrDeleteGPUInstanceRunlists_FWCLIENT(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrDeleteGPUInstanceRunlists_HAL(arg0, arg1, arg2) kmigmgrDeleteGPUInstanceRunlists(arg0, arg1, arg2)

NV_STATUS kmigmgrCreateGPUInstanceRunlists_FWCLIENT(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrCreateGPUInstanceRunlists(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrCreateGPUInstanceRunlists(arg0, arg1, arg2) kmigmgrCreateGPUInstanceRunlists_FWCLIENT(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrCreateGPUInstanceRunlists_HAL(arg0, arg1, arg2) kmigmgrCreateGPUInstanceRunlists(arg0, arg1, arg2)

NV_STATUS kmigmgrRestoreFromPersistence_PF(OBJGPU *arg0, struct KernelMIGManager *arg1);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrRestoreFromPersistence(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrRestoreFromPersistence(arg0, arg1) kmigmgrRestoreFromPersistence_PF(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrRestoreFromPersistence_HAL(arg0, arg1) kmigmgrRestoreFromPersistence(arg0, arg1)

void kmigmgrDetectReducedConfig_KERNEL(OBJGPU *arg0, struct KernelMIGManager *arg1);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDetectReducedConfig(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDetectReducedConfig(arg0, arg1) kmigmgrDetectReducedConfig_KERNEL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrDetectReducedConfig_HAL(arg0, arg1) kmigmgrDetectReducedConfig(arg0, arg1)

static inline NV_STATUS kmigmgrGenerateComputeInstanceUuid_5baef9(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvU32 globalGrIdx, NvUuid *pUuid) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGenerateComputeInstanceUuid(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvU32 globalGrIdx, NvUuid *pUuid) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGenerateComputeInstanceUuid(arg0, arg1, swizzId, globalGrIdx, pUuid) kmigmgrGenerateComputeInstanceUuid_5baef9(arg0, arg1, swizzId, globalGrIdx, pUuid)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrGenerateComputeInstanceUuid_HAL(arg0, arg1, swizzId, globalGrIdx, pUuid) kmigmgrGenerateComputeInstanceUuid(arg0, arg1, swizzId, globalGrIdx, pUuid)

NV_STATUS kmigmgrCreateComputeInstances_FWCLIENT(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvBool bQuery, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS arg3, NvU32 *pCIIds, NvBool bCreateCap);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrCreateComputeInstances(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvBool bQuery, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS arg3, NvU32 *pCIIds, NvBool bCreateCap) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrCreateComputeInstances(arg0, arg1, arg2, bQuery, arg3, pCIIds, bCreateCap) kmigmgrCreateComputeInstances_FWCLIENT(arg0, arg1, arg2, bQuery, arg3, pCIIds, bCreateCap)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrCreateComputeInstances_HAL(arg0, arg1, arg2, bQuery, arg3, pCIIds, bCreateCap) kmigmgrCreateComputeInstances(arg0, arg1, arg2, bQuery, arg3, pCIIds, bCreateCap)

NV_STATUS kmigmgrSetMIGState_FWCLIENT(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bMemoryPartitioningNeeded, NvBool bEnable, NvBool bUnload);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetMIGState(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bMemoryPartitioningNeeded, NvBool bEnable, NvBool bUnload) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetMIGState(arg0, arg1, bMemoryPartitioningNeeded, bEnable, bUnload) kmigmgrSetMIGState_FWCLIENT(arg0, arg1, bMemoryPartitioningNeeded, bEnable, bUnload)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrSetMIGState_HAL(arg0, arg1, bMemoryPartitioningNeeded, bEnable, bUnload) kmigmgrSetMIGState(arg0, arg1, bMemoryPartitioningNeeded, bEnable, bUnload)

NV_STATUS kmigmgrGetComputeProfileFromGpcCount_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpcCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromGpcCount(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpcCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromGpcCount(arg0, arg1, gpcCount, pProfile) kmigmgrGetComputeProfileFromGpcCount_IMPL(arg0, arg1, gpcCount, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrGetComputeProfileFromGpcCount_HAL(arg0, arg1, gpcCount, pProfile) kmigmgrGetComputeProfileFromGpcCount(arg0, arg1, gpcCount, pProfile)

NvBool kmigmgrIsCTSAlignmentRequired_PF(OBJGPU *arg0, struct KernelMIGManager *arg1);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsCTSAlignmentRequired(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsCTSAlignmentRequired(arg0, arg1) kmigmgrIsCTSAlignmentRequired_PF(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrIsCTSAlignmentRequired_HAL(arg0, arg1) kmigmgrIsCTSAlignmentRequired(arg0, arg1)

NV_STATUS kmigmgrRestoreFromBootConfig_PF(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrRestoreFromBootConfig(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrRestoreFromBootConfig(pGpu, pKernelMIGManager) kmigmgrRestoreFromBootConfig_PF(pGpu, pKernelMIGManager)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrRestoreFromBootConfig_HAL(pGpu, pKernelMIGManager) kmigmgrRestoreFromBootConfig(pGpu, pKernelMIGManager)

NV_STATUS kmigmgrConstructEngine_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS kmigmgrConstructEngine_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__kmigmgrConstructEngine__(arg0, arg1, arg2);
}

NV_STATUS kmigmgrStateInitLocked_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

static inline NV_STATUS kmigmgrStateInitLocked_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return arg1->__kmigmgrStateInitLocked__(arg0, arg1);
}

NV_STATUS kmigmgrStateUnload_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 flags);

static inline NV_STATUS kmigmgrStateUnload_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 flags) {
    return arg1->__kmigmgrStateUnload__(arg0, arg1, flags);
}

NV_STATUS kmigmgrCreateGPUInstanceCheck_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bMemoryPartitioningNeeded);

static inline NV_STATUS kmigmgrCreateGPUInstanceCheck_46f6a7(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bMemoryPartitioningNeeded) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kmigmgrCreateGPUInstanceCheck_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bMemoryPartitioningNeeded) {
    return arg1->__kmigmgrCreateGPUInstanceCheck__(arg0, arg1, bMemoryPartitioningNeeded);
}

NvBool kmigmgrIsDevinitMIGBitSet_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1);

static inline NvBool kmigmgrIsDevinitMIGBitSet_491d52(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsDevinitMIGBitSet_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    return arg1->__kmigmgrIsDevinitMIGBitSet__(arg0, arg1);
}

NvBool kmigmgrIsGPUInstanceCombinationValid_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag);

NvBool kmigmgrIsGPUInstanceCombinationValid_GH100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag);

static inline NvBool kmigmgrIsGPUInstanceCombinationValid_491d52(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsGPUInstanceCombinationValid_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag) {
    return arg1->__kmigmgrIsGPUInstanceCombinationValid__(arg0, arg1, gpuInstanceFlag);
}

NvBool kmigmgrIsGPUInstanceFlagValid_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag);

NvBool kmigmgrIsGPUInstanceFlagValid_GH100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag);

static inline NvBool kmigmgrIsGPUInstanceFlagValid_491d52(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsGPUInstanceFlagValid_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 gpuInstanceFlag) {
    return arg1->__kmigmgrIsGPUInstanceFlagValid__(arg0, arg1, gpuInstanceFlag);
}

NvBool kmigmgrIsMemoryPartitioningRequested_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 partitionFlags);

static inline NvBool kmigmgrIsMemoryPartitioningRequested_491d52(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 partitionFlags) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsMemoryPartitioningRequested_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 partitionFlags) {
    return arg1->__kmigmgrIsMemoryPartitioningRequested__(arg0, arg1, partitionFlags);
}

NvBool kmigmgrIsMemoryPartitioningNeeded_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId);

static inline NvBool kmigmgrIsMemoryPartitioningNeeded_491d52(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsMemoryPartitioningNeeded_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    return arg1->__kmigmgrIsMemoryPartitioningNeeded__(arg0, arg1, swizzId);
}

static inline struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_d64cd6(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 memSizeFlag) {
    return NV_RANGE_EMPTY;
}

struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_GA100(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 memSizeFlag);

static inline struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 memSizeFlag) {
    return arg1->__kmigmgrMemSizeFlagToSwizzIdRange__(arg0, arg1, memSizeFlag);
}

static inline NV_STATUS kmigmgrStateLoad_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kmigmgrStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmigmgrStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kmigmgrStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmigmgrStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kmigmgrStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kmigmgrStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    pEngstate->__kmigmgrStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kmigmgrStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmigmgrStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kmigmgrInitMissing_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    pEngstate->__kmigmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kmigmgrStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kmigmgrIsPresent_DISPATCH(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrIsPresent__(pGpu, pEngstate);
}

static inline NvBool kmigmgrUseLegacyVgpuPolicy(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kmigmgrIsMIGNvlinkP2PSupportOverridden(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    return ((NvBool)(0 != 0));
}

static inline void kmigmgrSetMIGEnabled(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvBool bEnabled) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    pKernelMIGManager_PRIVATE->bMIGEnabled = bEnabled;
}

static inline const union ENGTYPE_BIT_VECTOR *kmigmgrGetPartitionableEnginesInUse(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    return &pKernelMIGManager_PRIVATE->partitionableEnginesInUse;
}

static inline NvBool kmigmgrIsA100ReducedConfig(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    return pKernelMIGManager_PRIVATE->bIsA100ReducedConfig;
}

static inline void kmigmgrSetIsA100ReducedConfig(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvBool bA100ReducedConfig) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    pKernelMIGManager_PRIVATE->bIsA100ReducedConfig = bA100ReducedConfig;
}

NV_STATUS kmigmgrIncRefCount_IMPL(struct RsShared *arg0);

#define kmigmgrIncRefCount(arg0) kmigmgrIncRefCount_IMPL(arg0)
NV_STATUS kmigmgrDecRefCount_IMPL(struct RsShared *arg0);

#define kmigmgrDecRefCount(arg0) kmigmgrDecRefCount_IMPL(arg0)
struct MIG_INSTANCE_REF kmigmgrMakeGIReference_IMPL(KERNEL_MIG_GPU_INSTANCE *arg0);

#define kmigmgrMakeGIReference(arg0) kmigmgrMakeGIReference_IMPL(arg0)
struct MIG_INSTANCE_REF kmigmgrMakeCIReference_IMPL(KERNEL_MIG_GPU_INSTANCE *arg0, MIG_COMPUTE_INSTANCE *arg1);

#define kmigmgrMakeCIReference(arg0, arg1) kmigmgrMakeCIReference_IMPL(arg0, arg1)
NV_STATUS kmigmgrEngineTypeXlate_IMPL(union ENGTYPE_BIT_VECTOR *pSrc, RM_ENGINE_TYPE srcEngineType, union ENGTYPE_BIT_VECTOR *pDst, RM_ENGINE_TYPE *pDstEngineType);

#define kmigmgrEngineTypeXlate(pSrc, srcEngineType, pDst, pDstEngineType) kmigmgrEngineTypeXlate_IMPL(pSrc, srcEngineType, pDst, pDstEngineType)
NvBool kmigmgrIsInstanceAttributionIdValid_IMPL(NvU16 id);

#define kmigmgrIsInstanceAttributionIdValid(id) kmigmgrIsInstanceAttributionIdValid_IMPL(id)
struct MIG_INSTANCE_REF kmigmgrMakeNoMIGReference_IMPL(void);

#define kmigmgrMakeNoMIGReference() kmigmgrMakeNoMIGReference_IMPL()
NvBool kmigmgrIsMIGReferenceValid_IMPL(struct MIG_INSTANCE_REF *arg0);

#define kmigmgrIsMIGReferenceValid(arg0) kmigmgrIsMIGReferenceValid_IMPL(arg0)
NvBool kmigmgrAreMIGReferencesSame_IMPL(struct MIG_INSTANCE_REF *arg0, struct MIG_INSTANCE_REF *arg1);

#define kmigmgrAreMIGReferencesSame(arg0, arg1) kmigmgrAreMIGReferencesSame_IMPL(arg0, arg1)
NvU32 kmigmgrCountEnginesOfType_IMPL(const union ENGTYPE_BIT_VECTOR *arg0, RM_ENGINE_TYPE arg1);

#define kmigmgrCountEnginesOfType(arg0, arg1) kmigmgrCountEnginesOfType_IMPL(arg0, arg1)
NvU16 kmigmgrGetAttributionIdFromMIGReference_IMPL(struct MIG_INSTANCE_REF arg0);

#define kmigmgrGetAttributionIdFromMIGReference(arg0) kmigmgrGetAttributionIdFromMIGReference_IMPL(arg0)
NV_STATUS kmigmgrAllocateInstanceEngines_IMPL(union ENGTYPE_BIT_VECTOR *pSourceEngines, NvBool bShared, struct NV_RANGE engTypeRange, NvU32 reqEngCount, union ENGTYPE_BIT_VECTOR *pOutEngines, union ENGTYPE_BIT_VECTOR *pExclusiveEngines, union ENGTYPE_BIT_VECTOR *pSharedEngines, union ENGTYPE_BIT_VECTOR *pAllocatableEngines);

#define kmigmgrAllocateInstanceEngines(pSourceEngines, bShared, engTypeRange, reqEngCount, pOutEngines, pExclusiveEngines, pSharedEngines, pAllocatableEngines) kmigmgrAllocateInstanceEngines_IMPL(pSourceEngines, bShared, engTypeRange, reqEngCount, pOutEngines, pExclusiveEngines, pSharedEngines, pAllocatableEngines)
void kmigmgrGetLocalEngineMask_IMPL(union ENGTYPE_BIT_VECTOR *pPhysicalEngineMask, union ENGTYPE_BIT_VECTOR *pLocalEngineMask);

#define kmigmgrGetLocalEngineMask(pPhysicalEngineMask, pLocalEngineMask) kmigmgrGetLocalEngineMask_IMPL(pPhysicalEngineMask, pLocalEngineMask)
NV_STATUS kmigmgrAllocGPUInstanceHandles_IMPL(OBJGPU *arg0, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE *arg1);

#define kmigmgrAllocGPUInstanceHandles(arg0, swizzId, arg1) kmigmgrAllocGPUInstanceHandles_IMPL(arg0, swizzId, arg1)
void kmigmgrFreeGPUInstanceHandles_IMPL(KERNEL_MIG_GPU_INSTANCE *arg0);

#define kmigmgrFreeGPUInstanceHandles(arg0) kmigmgrFreeGPUInstanceHandles_IMPL(arg0)
NvBool kmigmgrIsGPUInstanceReadyToBeDestroyed_IMPL(KERNEL_MIG_GPU_INSTANCE *arg0);

#define kmigmgrIsGPUInstanceReadyToBeDestroyed(arg0) kmigmgrIsGPUInstanceReadyToBeDestroyed_IMPL(arg0)
void kmigmgrDestruct_IMPL(struct KernelMIGManager *arg0);

#define __nvoc_kmigmgrDestruct(arg0) kmigmgrDestruct_IMPL(arg0)
void kmigmgrInitRegistryOverrides_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrInitRegistryOverrides(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitRegistryOverrides(arg0, arg1) kmigmgrInitRegistryOverrides_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

KERNEL_MIG_GPU_INSTANCE *kmigmgrGetMIGGpuInstanceSlot_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 i);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline KERNEL_MIG_GPU_INSTANCE *kmigmgrGetMIGGpuInstanceSlot(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 i) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMIGGpuInstanceSlot(arg0, arg1, i) kmigmgrGetMIGGpuInstanceSlot_IMPL(arg0, arg1, i)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGSupported_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGSupported(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGSupported(arg0, arg1) kmigmgrIsMIGSupported_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGEnabled_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGEnabled(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGEnabled(arg0, arg1) kmigmgrIsMIGEnabled_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGGpuInstancingEnabled_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGGpuInstancingEnabled(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGGpuInstancingEnabled(arg0, arg1) kmigmgrIsMIGGpuInstancingEnabled_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGMemPartitioningEnabled_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGMemPartitioningEnabled(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGMemPartitioningEnabled(arg0, arg1) kmigmgrIsMIGMemPartitioningEnabled_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

const KERNEL_MIG_MANAGER_STATIC_INFO *kmigmgrGetStaticInfo_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline const KERNEL_MIG_MANAGER_STATIC_INFO *kmigmgrGetStaticInfo(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetStaticInfo(arg0, arg1) kmigmgrGetStaticInfo_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSaveToPersistence_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSaveToPersistence(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSaveToPersistence(arg0, arg1) kmigmgrSaveToPersistence_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDisableWatchdog_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDisableWatchdog(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDisableWatchdog(arg0, arg1) kmigmgrDisableWatchdog_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrRestoreWatchdog_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrRestoreWatchdog(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrRestoreWatchdog(arg0, arg1) kmigmgrRestoreWatchdog_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetSwizzIdInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetSwizzIdInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetSwizzIdInUse(arg0, arg1, swizzId) kmigmgrSetSwizzIdInUse_IMPL(arg0, arg1, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrClearSwizzIdInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrClearSwizzIdInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearSwizzIdInUse(arg0, arg1, swizzId) kmigmgrClearSwizzIdInUse_IMPL(arg0, arg1, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsSwizzIdInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsSwizzIdInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsSwizzIdInUse(arg0, arg1, swizzId) kmigmgrIsSwizzIdInUse_IMPL(arg0, arg1, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInvalidSwizzIdMask_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvU64 *pUnsupportedSwizzIdMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInvalidSwizzIdMask(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvU64 *pUnsupportedSwizzIdMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInvalidSwizzIdMask(arg0, arg1, swizzId, pUnsupportedSwizzIdMask) kmigmgrGetInvalidSwizzIdMask_IMPL(arg0, arg1, swizzId, pUnsupportedSwizzIdMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGNvlinkP2PSupported_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGNvlinkP2PSupported(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGNvlinkP2PSupported(arg0, arg1) kmigmgrIsMIGNvlinkP2PSupported_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU64 kmigmgrGetSwizzIdInUseMask_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU64 kmigmgrGetSwizzIdInUseMask(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSwizzIdInUseMask(arg0, arg1) kmigmgrGetSwizzIdInUseMask_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetEnginesInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, union ENGTYPE_BIT_VECTOR *pEngines);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetEnginesInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, union ENGTYPE_BIT_VECTOR *pEngines) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetEnginesInUse(arg0, arg1, pEngines) kmigmgrSetEnginesInUse_IMPL(arg0, arg1, pEngines)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrClearEnginesInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, union ENGTYPE_BIT_VECTOR *pEngines);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrClearEnginesInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, union ENGTYPE_BIT_VECTOR *pEngines) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearEnginesInUse(arg0, arg1, pEngines) kmigmgrClearEnginesInUse_IMPL(arg0, arg1, pEngines)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEngineInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEngineInUse(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEngineInUse(arg0, arg1, rmEngineType) kmigmgrIsEngineInUse_IMPL(arg0, arg1, rmEngineType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEnginePartitionable_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEnginePartitionable(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEnginePartitionable(arg0, arg1, rmEngineType) kmigmgrIsEnginePartitionable_IMPL(arg0, arg1, rmEngineType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEngineInInstance_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE globalRmEngType, struct MIG_INSTANCE_REF arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEngineInInstance(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE globalRmEngType, struct MIG_INSTANCE_REF arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEngineInInstance(arg0, arg1, globalRmEngType, arg2) kmigmgrIsEngineInInstance_IMPL(arg0, arg1, globalRmEngType, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrCreateGPUInstance_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 *pSwizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2, NvBool bValid, NvBool bCreateCap);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrCreateGPUInstance(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 *pSwizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2, NvBool bValid, NvBool bCreateCap) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrCreateGPUInstance(arg0, arg1, pSwizzId, arg2, bValid, bCreateCap) kmigmgrCreateGPUInstance_IMPL(arg0, arg1, pSwizzId, arg2, bValid, bCreateCap)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGPUInstance_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvBool bUnload);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGPUInstance(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, NvBool bUnload) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGPUInstance(arg0, arg1, swizzId, bUnload) kmigmgrInvalidateGPUInstance_IMPL(arg0, arg1, swizzId, bUnload)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceScrubber_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceScrubber(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceScrubber(arg0, arg1, arg2) kmigmgrInitGPUInstanceScrubber_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceScrubber_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceScrubber(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceScrubber(arg0, arg1, arg2) kmigmgrDestroyGPUInstanceScrubber_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceBufPools_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceBufPools(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceBufPools(arg0, arg1, arg2) kmigmgrInitGPUInstanceBufPools_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceGrBufPools_IMPL(OBJGPU *pGpu, struct KernelMIGManager *arg0, KERNEL_MIG_GPU_INSTANCE *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceGrBufPools(OBJGPU *pGpu, struct KernelMIGManager *arg0, KERNEL_MIG_GPU_INSTANCE *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceGrBufPools(pGpu, arg0, arg1) kmigmgrInitGPUInstanceGrBufPools_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceGrBufPools_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceGrBufPools(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceGrBufPools(arg0, arg1, arg2) kmigmgrDestroyGPUInstanceGrBufPools_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstancePool_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstancePool(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstancePool(arg0, arg1, arg2) kmigmgrInitGPUInstancePool_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstancePool_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstancePool(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstancePool(arg0, arg1, arg2) kmigmgrDestroyGPUInstancePool_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceRunlistBufPools_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceRunlistBufPools(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceRunlistBufPools(arg0, arg1, arg2) kmigmgrInitGPUInstanceRunlistBufPools_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceRunlistBufPools_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceRunlistBufPools(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceRunlistBufPools(arg0, arg1, arg2) kmigmgrDestroyGPUInstanceRunlistBufPools_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrPrintSubscribingClients_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrPrintSubscribingClients(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrPrintSubscribingClients(arg0, arg1, swizzId) kmigmgrPrintSubscribingClients_IMPL(arg0, arg1, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrInitGPUInstanceInfo_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrInitGPUInstanceInfo(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceInfo(arg0, arg1, arg2) kmigmgrInitGPUInstanceInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrTrimInstanceRunlistBufPools_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrTrimInstanceRunlistBufPools(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrTrimInstanceRunlistBufPools(arg0, arg1, arg2) kmigmgrTrimInstanceRunlistBufPools_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetDeviceProfilingInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetDeviceProfilingInUse(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetDeviceProfilingInUse(arg0, arg1) kmigmgrSetDeviceProfilingInUse_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrClearDeviceProfilingInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrClearDeviceProfilingInUse(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearDeviceProfilingInUse(arg0, arg1) kmigmgrClearDeviceProfilingInUse_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsDeviceProfilingInUse_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsDeviceProfilingInUse(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsDeviceProfilingInUse(arg0, arg1) kmigmgrIsDeviceProfilingInUse_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsDeviceUsingDeviceProfiling_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsDeviceUsingDeviceProfiling(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsDeviceUsingDeviceProfiling(arg0, arg1, pDevice) kmigmgrIsDeviceUsingDeviceProfiling_IMPL(arg0, arg1, pDevice)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrEnableAllLCEs_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bEnableAllLCEs);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrEnableAllLCEs(OBJGPU *arg0, struct KernelMIGManager *arg1, NvBool bEnableAllLCEs) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrEnableAllLCEs(arg0, arg1, bEnableAllLCEs) kmigmgrEnableAllLCEs_IMPL(arg0, arg1, bEnableAllLCEs)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInstanceRefFromDevice_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *arg2, struct MIG_INSTANCE_REF *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInstanceRefFromDevice(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *arg2, struct MIG_INSTANCE_REF *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInstanceRefFromDevice(arg0, arg1, arg2, arg3) kmigmgrGetInstanceRefFromDevice_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetMemoryPartitionHeapFromDevice_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *arg2, struct Heap **arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetMemoryPartitionHeapFromDevice(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *arg2, struct Heap **arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMemoryPartitionHeapFromDevice(arg0, arg1, arg2, arg3) kmigmgrGetMemoryPartitionHeapFromDevice_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetSwizzIdFromDevice_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice, NvU32 *pSwizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSwizzIdFromDevice(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice, NvU32 *pSwizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSwizzIdFromDevice(arg0, arg1, pDevice, pSwizzId) kmigmgrGetSwizzIdFromDevice_IMPL(arg0, arg1, pDevice, pSwizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrPrintGPUInstanceInfo_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrPrintGPUInstanceInfo(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrPrintGPUInstanceInfo(arg0, arg1, arg2) kmigmgrPrintGPUInstanceInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetGPUInstanceInfo_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetGPUInstanceInfo(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetGPUInstanceInfo(arg0, arg1, swizzId, arg2) kmigmgrSetGPUInstanceInfo_IMPL(arg0, arg1, swizzId, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGPUInstanceInfo_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE **arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGPUInstanceInfo(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGPUInstanceInfo(arg0, arg1, swizzId, arg2) kmigmgrGetGPUInstanceInfo_IMPL(arg0, arg1, swizzId, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetLocalToGlobalEngineType_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, struct MIG_INSTANCE_REF arg2, RM_ENGINE_TYPE localEngType, RM_ENGINE_TYPE *pGlobalEngType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetLocalToGlobalEngineType(OBJGPU *arg0, struct KernelMIGManager *arg1, struct MIG_INSTANCE_REF arg2, RM_ENGINE_TYPE localEngType, RM_ENGINE_TYPE *pGlobalEngType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetLocalToGlobalEngineType(arg0, arg1, arg2, localEngType, pGlobalEngType) kmigmgrGetLocalToGlobalEngineType_IMPL(arg0, arg1, arg2, localEngType, pGlobalEngType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGlobalToLocalEngineType_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, struct MIG_INSTANCE_REF arg2, RM_ENGINE_TYPE globalEngType, RM_ENGINE_TYPE *pLocalEngType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGlobalToLocalEngineType(OBJGPU *arg0, struct KernelMIGManager *arg1, struct MIG_INSTANCE_REF arg2, RM_ENGINE_TYPE globalEngType, RM_ENGINE_TYPE *pLocalEngType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGlobalToLocalEngineType(arg0, arg1, arg2, globalEngType, pLocalEngType) kmigmgrGetGlobalToLocalEngineType_IMPL(arg0, arg1, arg2, globalEngType, pLocalEngType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrFilterEngineList_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, struct Subdevice *arg2, RM_ENGINE_TYPE *pEngineTypes, NvU32 *pEngineCount);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrFilterEngineList(OBJGPU *arg0, struct KernelMIGManager *arg1, struct Subdevice *arg2, RM_ENGINE_TYPE *pEngineTypes, NvU32 *pEngineCount) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFilterEngineList(arg0, arg1, arg2, pEngineTypes, pEngineCount) kmigmgrFilterEngineList_IMPL(arg0, arg1, arg2, pEngineTypes, pEngineCount)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrFilterEnginePartnerList_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, struct Subdevice *arg2, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrFilterEnginePartnerList(OBJGPU *arg0, struct KernelMIGManager *arg1, struct Subdevice *arg2, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFilterEnginePartnerList(arg0, arg1, arg2, arg3) kmigmgrFilterEnginePartnerList_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetProfileByPartitionFlag_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 partitionFlag, const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO **arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetProfileByPartitionFlag(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 partitionFlag, const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetProfileByPartitionFlag(arg0, arg1, partitionFlag, arg2) kmigmgrGetProfileByPartitionFlag_IMPL(arg0, arg1, partitionFlag, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSaveComputeInstances_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, GPUMGR_SAVE_COMPUTE_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSaveComputeInstances(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, GPUMGR_SAVE_COMPUTE_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSaveComputeInstances(arg0, arg1, arg2, arg3) kmigmgrSaveComputeInstances_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetPartitioningMode_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetPartitioningMode(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetPartitioningMode(arg0, arg1) kmigmgrSetPartitioningMode_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetMIGReferenceFromEngineType_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType, struct MIG_INSTANCE_REF *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetMIGReferenceFromEngineType(OBJGPU *arg0, struct KernelMIGManager *arg1, RM_ENGINE_TYPE rmEngineType, struct MIG_INSTANCE_REF *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMIGReferenceFromEngineType(arg0, arg1, rmEngineType, arg2) kmigmgrGetMIGReferenceFromEngineType_IMPL(arg0, arg1, rmEngineType, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGPUInstanceScrubberCe_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice, NvU32 *ceInst);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGPUInstanceScrubberCe(OBJGPU *arg0, struct KernelMIGManager *arg1, Device *pDevice, NvU32 *ceInst) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGPUInstanceScrubberCe(arg0, arg1, pDevice, ceInst) kmigmgrGetGPUInstanceScrubberCe_IMPL(arg0, arg1, pDevice, ceInst)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDescribeGPUInstances_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDescribeGPUInstances(OBJGPU *arg0, struct KernelMIGManager *arg1, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDescribeGPUInstances(arg0, arg1, arg2) kmigmgrDescribeGPUInstances_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSwizzIdToResourceAllocation_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_RESOURCE_ALLOCATION *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSwizzIdToResourceAllocation(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_RESOURCE_ALLOCATION *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSwizzIdToResourceAllocation(arg0, arg1, swizzId, arg2, arg3, arg4) kmigmgrSwizzIdToResourceAllocation_IMPL(arg0, arg1, swizzId, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrAllocComputeInstanceHandles_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrAllocComputeInstanceHandles(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrAllocComputeInstanceHandles(arg0, arg1, arg2, arg3) kmigmgrAllocComputeInstanceHandles_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrFreeComputeInstanceHandles_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrFreeComputeInstanceHandles(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFreeComputeInstanceHandles(arg0, arg1, arg2, arg3) kmigmgrFreeComputeInstanceHandles_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrReleaseComputeInstanceEngines_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrReleaseComputeInstanceEngines(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, MIG_COMPUTE_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrReleaseComputeInstanceEngines(arg0, arg1, arg2, arg3) kmigmgrReleaseComputeInstanceEngines_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDeleteComputeInstance_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 CIId, NvBool bUnload);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDeleteComputeInstance(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 CIId, NvBool bUnload) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDeleteComputeInstance(arg0, arg1, arg2, CIId, bUnload) kmigmgrDeleteComputeInstance_IMPL(arg0, arg1, arg2, CIId, bUnload)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrConfigureGPUInstance_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, const KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi, NvU32 updateEngMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrConfigureGPUInstance(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 swizzId, const KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi, NvU32 updateEngMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrConfigureGPUInstance(arg0, arg1, swizzId, pConfigRequestPerCi, updateEngMask) kmigmgrConfigureGPUInstance_IMPL(arg0, arg1, swizzId, pConfigRequestPerCi, updateEngMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGrGpcMapping_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 grIdx);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGrGpcMapping(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGrGpcMapping(arg0, arg1, arg2, grIdx) kmigmgrInvalidateGrGpcMapping_IMPL(arg0, arg1, arg2, grIdx)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGr_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 grIdx);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGr(OBJGPU *arg0, struct KernelMIGManager *arg1, KERNEL_MIG_GPU_INSTANCE *arg2, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGr(arg0, arg1, arg2, grIdx) kmigmgrInvalidateGr_IMPL(arg0, arg1, arg2, grIdx)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetNextComputeSize_IMPL(NvBool bGetNextSmallest, NvU32 computeSize);

#define kmigmgrGetNextComputeSize(bGetNextSmallest, computeSize) kmigmgrGetNextComputeSize_IMPL(bGetNextSmallest, computeSize)
NV_STATUS kmigmgrGetSkylineFromSize_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO **ppSkyline);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSkylineFromSize(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO **ppSkyline) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSkylineFromSize(arg0, arg1, computeSize, ppSkyline) kmigmgrGetSkylineFromSize_IMPL(arg0, arg1, computeSize, ppSkyline)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromSize_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromSize(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromSize(arg0, arg1, computeSize, pProfile) kmigmgrGetComputeProfileFromSize_IMPL(arg0, arg1, computeSize, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromSmCount_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 smCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromSmCount(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 smCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromSmCount(arg0, arg1, smCount, pProfile) kmigmgrGetComputeProfileFromSmCount_IMPL(arg0, arg1, smCount, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromCTSId_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromCTSId(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromCTSId(arg0, arg1, ctsId, pProfile) kmigmgrGetComputeProfileFromCTSId_IMPL(arg0, arg1, ctsId, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInvalidCTSIdMask_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId, NvU64 *pInvalidCTSIdMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInvalidCTSIdMask(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId, NvU64 *pInvalidCTSIdMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInvalidCTSIdMask(arg0, arg1, ctsId, pInvalidCTSIdMask) kmigmgrGetInvalidCTSIdMask_IMPL(arg0, arg1, ctsId, pInvalidCTSIdMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

struct NV_RANGE kmigmgrComputeProfileSizeToCTSIdRange_IMPL(NvU32 computeSize);

#define kmigmgrComputeProfileSizeToCTSIdRange(computeSize) kmigmgrComputeProfileSizeToCTSIdRange_IMPL(computeSize)
NV_STATUS kmigmgrGetFreeCTSId_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 *pCtsId, NvU64 globalValidCtsMask, NvU64 globalValidGfxCtsMask, NvU64 ctsIdsInUseMask, NvU32 profileSize, NvBool bRestrictWithGfx, NvBool bGfxRequested);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetFreeCTSId(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 *pCtsId, NvU64 globalValidCtsMask, NvU64 globalValidGfxCtsMask, NvU64 ctsIdsInUseMask, NvU32 profileSize, NvBool bRestrictWithGfx, NvBool bGfxRequested) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetFreeCTSId(arg0, arg1, pCtsId, globalValidCtsMask, globalValidGfxCtsMask, ctsIdsInUseMask, profileSize, bRestrictWithGfx, bGfxRequested) kmigmgrGetFreeCTSId_IMPL(arg0, arg1, pCtsId, globalValidCtsMask, globalValidGfxCtsMask, ctsIdsInUseMask, profileSize, bRestrictWithGfx, bGfxRequested)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetComputeSizeFromCTSId_IMPL(NvU32 ctsId);

#define kmigmgrGetComputeSizeFromCTSId(ctsId) kmigmgrGetComputeSizeFromCTSId_IMPL(ctsId)
NvU32 kmigmgrSmallestComputeProfileSize_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU32 kmigmgrSmallestComputeProfileSize(OBJGPU *arg0, struct KernelMIGManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSmallestComputeProfileSize(arg0, arg1) kmigmgrSmallestComputeProfileSize_IMPL(arg0, arg1)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrSetCTSIdInUse_IMPL(KERNEL_MIG_GPU_INSTANCE *arg0, NvU32 ctsId, NvU32 grId, NvBool bInUse);

#define kmigmgrSetCTSIdInUse(arg0, ctsId, grId, bInUse) kmigmgrSetCTSIdInUse_IMPL(arg0, ctsId, grId, bInUse)
NV_STATUS kmigmgrXlateSpanStartToCTSId_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, NvU32 spanStart, NvU32 *pCtsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrXlateSpanStartToCTSId(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 computeSize, NvU32 spanStart, NvU32 *pCtsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrXlateSpanStartToCTSId(arg0, arg1, computeSize, spanStart, pCtsId) kmigmgrXlateSpanStartToCTSId_IMPL(arg0, arg1, computeSize, spanStart, pCtsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetSlotBasisMask_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU64 *pMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSlotBasisMask(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU64 *pMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSlotBasisMask(arg0, arg1, pMask) kmigmgrGetSlotBasisMask_IMPL(arg0, arg1, pMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetSpanStartFromCTSId_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU32 kmigmgrGetSpanStartFromCTSId(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU32 ctsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSpanStartFromCTSId(arg0, arg1, ctsId) kmigmgrGetSpanStartFromCTSId_IMPL(arg0, arg1, ctsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsCTSIdAvailable_IMPL(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU64 ctsIdValidMask, NvU64 ctsIdInUseMask, NvU32 ctsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsCTSIdAvailable(OBJGPU *arg0, struct KernelMIGManager *arg1, NvU64 ctsIdValidMask, NvU64 ctsIdInUseMask, NvU32 ctsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsCTSIdAvailable(arg0, arg1, ctsIdValidMask, ctsIdInUseMask, ctsId) kmigmgrIsCTSIdAvailable_IMPL(arg0, arg1, ctsIdValidMask, ctsIdInUseMask, ctsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrUpdateCiConfigForVgpu_IMPL(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvU32 execPartCount, NvU32 *pExecPartId, NvU32 gfid, NvBool bDelete);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrUpdateCiConfigForVgpu(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvU32 execPartCount, NvU32 *pExecPartId, NvU32 gfid, NvBool bDelete) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrUpdateCiConfigForVgpu(pGpu, pKernelMIGManager, execPartCount, pExecPartId, gfid, bDelete) kmigmgrUpdateCiConfigForVgpu_IMPL(pGpu, pKernelMIGManager, execPartCount, pExecPartId, gfid, bDelete)
#endif //__nvoc_kernel_mig_manager_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_MIG_MANAGER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_MIG_MANAGER_NVOC_H_
