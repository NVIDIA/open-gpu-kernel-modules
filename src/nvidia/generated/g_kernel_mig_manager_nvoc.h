
#ifndef _G_KERNEL_MIG_MANAGER_NVOC_H_
#define _G_KERNEL_MIG_MANAGER_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    /*!
     * Internal ThirdPartyP2P handle for tracking persistent mappings under MIG.
     */
    NvHandle hThirdPartyP2P;
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

    /*!
     * GPU Instance UUID
     */
    NvUuid uuid;
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

    /*! Partitionable engines which are present on this GPU. */
    ENGTYPE_BIT_VECTOR partitionableEngines;

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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelMIGManager {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelMIGManager *__nvoc_pbase_KernelMIGManager;    // kmigmgr

    // Vtable with 35 per-object function pointers
    NV_STATUS (*__kmigmgrConstructEngine__)(OBJGPU *, struct KernelMIGManager * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateInitLocked__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateUnload__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrLoadStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSetStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    void (*__kmigmgrClearStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSaveToPersistenceFromVgpuStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrDeleteGPUInstanceRunlists__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateGPUInstanceRunlists__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrRestoreFromPersistence__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateGPUInstanceCheck__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsDevinitMIGBitSet__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (3 hals)
    NvBool (*__kmigmgrIsGPUInstanceCombinationValid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (3 hals)
    NvBool (*__kmigmgrIsGPUInstanceFlagValid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (4 hals)
    NV_STATUS (*__kmigmgrGenerateComputeInstanceUuid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32, NvU32, NvUuid *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrGenerateGPUInstanceUuid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32, NvUuid *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateComputeInstances__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *, NvBool, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS, NvU32 *, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsMemoryPartitioningRequested__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    NvBool (*__kmigmgrIsMemoryPartitioningNeeded__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    struct NV_RANGE (*__kmigmgrMemSizeFlagToSwizzIdRange__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (3 hals)
    struct NV_RANGE (*__kmigmgrSwizzIdToSpan__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSetMIGState__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvBool, NvBool, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsCTSAlignmentRequired__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (3 hals)
    NV_STATUS (*__kmigmgrRestoreFromBootConfig__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals) body
    void (*__kmigmgrInitMissing__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreInitLocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreInitUnlocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateInitUnlocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePostLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreUnload__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePostUnload__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kmigmgrStateDestroy__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kmigmgrIsPresent__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)

    // Data members
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
    NvBool PRIVATE_FIELD(bGlobalBootConfigUsed);
    NvU64 PRIVATE_FIELD(validGlobalCTSIdMask);
    NvU64 PRIVATE_FIELD(validGlobalGfxCTSIdMask);
};


struct KernelMIGManager_PRIVATE {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelMIGManager *__nvoc_pbase_KernelMIGManager;    // kmigmgr

    // Vtable with 35 per-object function pointers
    NV_STATUS (*__kmigmgrConstructEngine__)(OBJGPU *, struct KernelMIGManager * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateInitLocked__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateUnload__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmigmgrLoadStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSetStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    void (*__kmigmgrClearStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSaveToPersistenceFromVgpuStaticInfo__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrDeleteGPUInstanceRunlists__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateGPUInstanceRunlists__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrRestoreFromPersistence__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateGPUInstanceCheck__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsDevinitMIGBitSet__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (3 hals)
    NvBool (*__kmigmgrIsGPUInstanceCombinationValid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (3 hals)
    NvBool (*__kmigmgrIsGPUInstanceFlagValid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (4 hals)
    NV_STATUS (*__kmigmgrGenerateComputeInstanceUuid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32, NvU32, NvUuid *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrGenerateGPUInstanceUuid__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32, NvUuid *);  // halified (2 hals)
    NV_STATUS (*__kmigmgrCreateComputeInstances__)(OBJGPU *, struct KernelMIGManager * /*this*/, KERNEL_MIG_GPU_INSTANCE *, NvBool, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS, NvU32 *, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsMemoryPartitioningRequested__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    NvBool (*__kmigmgrIsMemoryPartitioningNeeded__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    struct NV_RANGE (*__kmigmgrMemSizeFlagToSwizzIdRange__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (3 hals)
    struct NV_RANGE (*__kmigmgrSwizzIdToSpan__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // halified (2 hals)
    NV_STATUS (*__kmigmgrSetMIGState__)(OBJGPU *, struct KernelMIGManager * /*this*/, NvBool, NvBool, NvBool);  // halified (2 hals)
    NvBool (*__kmigmgrIsCTSAlignmentRequired__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (3 hals)
    NV_STATUS (*__kmigmgrRestoreFromBootConfig__)(OBJGPU *, struct KernelMIGManager * /*this*/);  // halified (2 hals) body
    void (*__kmigmgrInitMissing__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreInitLocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreInitUnlocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateInitUnlocked__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStateLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePostLoad__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePreUnload__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmigmgrStatePostUnload__)(struct OBJGPU *, struct KernelMIGManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kmigmgrStateDestroy__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kmigmgrIsPresent__)(struct OBJGPU *, struct KernelMIGManager * /*this*/);  // virtual inherited (engstate) base (engstate)

    // Data members
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
    NvBool bGlobalBootConfigUsed;
    NvU64 validGlobalCTSIdMask;
    NvU64 validGlobalGfxCTSIdMask;
};

#ifndef __NVOC_CLASS_KernelMIGManager_TYPEDEF__
#define __NVOC_CLASS_KernelMIGManager_TYPEDEF__
typedef struct KernelMIGManager KernelMIGManager;
#endif /* __NVOC_CLASS_KernelMIGManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMIGManager
#define __nvoc_class_id_KernelMIGManager 0x01c1bf
#endif /* __nvoc_class_id_KernelMIGManager */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager;

#define __staticCast_KernelMIGManager(pThis) \
    ((pThis)->__nvoc_pbase_KernelMIGManager)

#ifdef __nvoc_kernel_mig_manager_h_disabled
#define __dynamicCast_KernelMIGManager(pThis) ((KernelMIGManager*)NULL)
#else //__nvoc_kernel_mig_manager_h_disabled
#define __dynamicCast_KernelMIGManager(pThis) \
    ((KernelMIGManager*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMIGManager)))
#endif //__nvoc_kernel_mig_manager_h_disabled

// Property macros
#define PDB_PROP_KMIGMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMIGMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMIGManager(KernelMIGManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMIGManager(KernelMIGManager**, Dynamic*, NvU32);
#define __objCreate_KernelMIGManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMIGManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kmigmgrConstructEngine_FNPTR(arg_this) arg_this->__kmigmgrConstructEngine__
#define kmigmgrConstructEngine(arg1, arg_this, arg3) kmigmgrConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define kmigmgrStateInitLocked_FNPTR(arg_this) arg_this->__kmigmgrStateInitLocked__
#define kmigmgrStateInitLocked(arg1, arg_this) kmigmgrStateInitLocked_DISPATCH(arg1, arg_this)
#define kmigmgrStateUnload_FNPTR(arg_this) arg_this->__kmigmgrStateUnload__
#define kmigmgrStateUnload(arg1, arg_this, flags) kmigmgrStateUnload_DISPATCH(arg1, arg_this, flags)
#define kmigmgrLoadStaticInfo_FNPTR(arg_this) arg_this->__kmigmgrLoadStaticInfo__
#define kmigmgrLoadStaticInfo(arg1, arg_this) kmigmgrLoadStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrLoadStaticInfo_HAL(arg1, arg_this) kmigmgrLoadStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrSetStaticInfo_FNPTR(arg_this) arg_this->__kmigmgrSetStaticInfo__
#define kmigmgrSetStaticInfo(arg1, arg_this) kmigmgrSetStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrSetStaticInfo_HAL(arg1, arg_this) kmigmgrSetStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrClearStaticInfo_FNPTR(arg_this) arg_this->__kmigmgrClearStaticInfo__
#define kmigmgrClearStaticInfo(arg1, arg_this) kmigmgrClearStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrClearStaticInfo_HAL(arg1, arg_this) kmigmgrClearStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrSaveToPersistenceFromVgpuStaticInfo_FNPTR(arg_this) arg_this->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__
#define kmigmgrSaveToPersistenceFromVgpuStaticInfo(arg1, arg_this) kmigmgrSaveToPersistenceFromVgpuStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrSaveToPersistenceFromVgpuStaticInfo_HAL(arg1, arg_this) kmigmgrSaveToPersistenceFromVgpuStaticInfo_DISPATCH(arg1, arg_this)
#define kmigmgrDeleteGPUInstanceRunlists_FNPTR(arg_this) arg_this->__kmigmgrDeleteGPUInstanceRunlists__
#define kmigmgrDeleteGPUInstanceRunlists(arg1, arg_this, arg3) kmigmgrDeleteGPUInstanceRunlists_DISPATCH(arg1, arg_this, arg3)
#define kmigmgrDeleteGPUInstanceRunlists_HAL(arg1, arg_this, arg3) kmigmgrDeleteGPUInstanceRunlists_DISPATCH(arg1, arg_this, arg3)
#define kmigmgrCreateGPUInstanceRunlists_FNPTR(arg_this) arg_this->__kmigmgrCreateGPUInstanceRunlists__
#define kmigmgrCreateGPUInstanceRunlists(arg1, arg_this, arg3) kmigmgrCreateGPUInstanceRunlists_DISPATCH(arg1, arg_this, arg3)
#define kmigmgrCreateGPUInstanceRunlists_HAL(arg1, arg_this, arg3) kmigmgrCreateGPUInstanceRunlists_DISPATCH(arg1, arg_this, arg3)
#define kmigmgrRestoreFromPersistence_FNPTR(arg_this) arg_this->__kmigmgrRestoreFromPersistence__
#define kmigmgrRestoreFromPersistence(arg1, arg_this) kmigmgrRestoreFromPersistence_DISPATCH(arg1, arg_this)
#define kmigmgrRestoreFromPersistence_HAL(arg1, arg_this) kmigmgrRestoreFromPersistence_DISPATCH(arg1, arg_this)
#define kmigmgrCreateGPUInstanceCheck_FNPTR(arg_this) arg_this->__kmigmgrCreateGPUInstanceCheck__
#define kmigmgrCreateGPUInstanceCheck(arg1, arg_this, bMemoryPartitioningNeeded) kmigmgrCreateGPUInstanceCheck_DISPATCH(arg1, arg_this, bMemoryPartitioningNeeded)
#define kmigmgrCreateGPUInstanceCheck_HAL(arg1, arg_this, bMemoryPartitioningNeeded) kmigmgrCreateGPUInstanceCheck_DISPATCH(arg1, arg_this, bMemoryPartitioningNeeded)
#define kmigmgrIsDevinitMIGBitSet_FNPTR(arg_this) arg_this->__kmigmgrIsDevinitMIGBitSet__
#define kmigmgrIsDevinitMIGBitSet(arg1, arg_this) kmigmgrIsDevinitMIGBitSet_DISPATCH(arg1, arg_this)
#define kmigmgrIsDevinitMIGBitSet_HAL(arg1, arg_this) kmigmgrIsDevinitMIGBitSet_DISPATCH(arg1, arg_this)
#define kmigmgrIsGPUInstanceCombinationValid_FNPTR(arg_this) arg_this->__kmigmgrIsGPUInstanceCombinationValid__
#define kmigmgrIsGPUInstanceCombinationValid(arg1, arg_this, gpuInstanceFlag) kmigmgrIsGPUInstanceCombinationValid_DISPATCH(arg1, arg_this, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceCombinationValid_HAL(arg1, arg_this, gpuInstanceFlag) kmigmgrIsGPUInstanceCombinationValid_DISPATCH(arg1, arg_this, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceFlagValid_FNPTR(arg_this) arg_this->__kmigmgrIsGPUInstanceFlagValid__
#define kmigmgrIsGPUInstanceFlagValid(arg1, arg_this, gpuInstanceFlag) kmigmgrIsGPUInstanceFlagValid_DISPATCH(arg1, arg_this, gpuInstanceFlag)
#define kmigmgrIsGPUInstanceFlagValid_HAL(arg1, arg_this, gpuInstanceFlag) kmigmgrIsGPUInstanceFlagValid_DISPATCH(arg1, arg_this, gpuInstanceFlag)
#define kmigmgrGenerateComputeInstanceUuid_FNPTR(arg_this) arg_this->__kmigmgrGenerateComputeInstanceUuid__
#define kmigmgrGenerateComputeInstanceUuid(arg1, arg_this, swizzId, globalGrIdx, arg5) kmigmgrGenerateComputeInstanceUuid_DISPATCH(arg1, arg_this, swizzId, globalGrIdx, arg5)
#define kmigmgrGenerateComputeInstanceUuid_HAL(arg1, arg_this, swizzId, globalGrIdx, arg5) kmigmgrGenerateComputeInstanceUuid_DISPATCH(arg1, arg_this, swizzId, globalGrIdx, arg5)
#define kmigmgrGenerateGPUInstanceUuid_FNPTR(arg_this) arg_this->__kmigmgrGenerateGPUInstanceUuid__
#define kmigmgrGenerateGPUInstanceUuid(arg1, arg_this, swizzId, arg4) kmigmgrGenerateGPUInstanceUuid_DISPATCH(arg1, arg_this, swizzId, arg4)
#define kmigmgrGenerateGPUInstanceUuid_HAL(arg1, arg_this, swizzId, arg4) kmigmgrGenerateGPUInstanceUuid_DISPATCH(arg1, arg_this, swizzId, arg4)
#define kmigmgrCreateComputeInstances_FNPTR(arg_this) arg_this->__kmigmgrCreateComputeInstances__
#define kmigmgrCreateComputeInstances(arg1, arg_this, arg3, bQuery, arg5, pCIIds, bCreateCap) kmigmgrCreateComputeInstances_DISPATCH(arg1, arg_this, arg3, bQuery, arg5, pCIIds, bCreateCap)
#define kmigmgrCreateComputeInstances_HAL(arg1, arg_this, arg3, bQuery, arg5, pCIIds, bCreateCap) kmigmgrCreateComputeInstances_DISPATCH(arg1, arg_this, arg3, bQuery, arg5, pCIIds, bCreateCap)
#define kmigmgrIsMemoryPartitioningRequested_FNPTR(arg_this) arg_this->__kmigmgrIsMemoryPartitioningRequested__
#define kmigmgrIsMemoryPartitioningRequested(arg1, arg_this, partitionFlags) kmigmgrIsMemoryPartitioningRequested_DISPATCH(arg1, arg_this, partitionFlags)
#define kmigmgrIsMemoryPartitioningRequested_HAL(arg1, arg_this, partitionFlags) kmigmgrIsMemoryPartitioningRequested_DISPATCH(arg1, arg_this, partitionFlags)
#define kmigmgrIsMemoryPartitioningNeeded_FNPTR(arg_this) arg_this->__kmigmgrIsMemoryPartitioningNeeded__
#define kmigmgrIsMemoryPartitioningNeeded(arg1, arg_this, swizzId) kmigmgrIsMemoryPartitioningNeeded_DISPATCH(arg1, arg_this, swizzId)
#define kmigmgrIsMemoryPartitioningNeeded_HAL(arg1, arg_this, swizzId) kmigmgrIsMemoryPartitioningNeeded_DISPATCH(arg1, arg_this, swizzId)
#define kmigmgrMemSizeFlagToSwizzIdRange_FNPTR(arg_this) arg_this->__kmigmgrMemSizeFlagToSwizzIdRange__
#define kmigmgrMemSizeFlagToSwizzIdRange(arg1, arg_this, memSizeFlag) kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(arg1, arg_this, memSizeFlag)
#define kmigmgrMemSizeFlagToSwizzIdRange_HAL(arg1, arg_this, memSizeFlag) kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(arg1, arg_this, memSizeFlag)
#define kmigmgrSwizzIdToSpan_FNPTR(arg_this) arg_this->__kmigmgrSwizzIdToSpan__
#define kmigmgrSwizzIdToSpan(arg1, arg_this, swizzId) kmigmgrSwizzIdToSpan_DISPATCH(arg1, arg_this, swizzId)
#define kmigmgrSwizzIdToSpan_HAL(arg1, arg_this, swizzId) kmigmgrSwizzIdToSpan_DISPATCH(arg1, arg_this, swizzId)
#define kmigmgrSetMIGState_FNPTR(arg_this) arg_this->__kmigmgrSetMIGState__
#define kmigmgrSetMIGState(arg1, arg_this, bMemoryPartitioningNeeded, bEnable, bUnload) kmigmgrSetMIGState_DISPATCH(arg1, arg_this, bMemoryPartitioningNeeded, bEnable, bUnload)
#define kmigmgrSetMIGState_HAL(arg1, arg_this, bMemoryPartitioningNeeded, bEnable, bUnload) kmigmgrSetMIGState_DISPATCH(arg1, arg_this, bMemoryPartitioningNeeded, bEnable, bUnload)
#define kmigmgrIsCTSAlignmentRequired_FNPTR(arg_this) arg_this->__kmigmgrIsCTSAlignmentRequired__
#define kmigmgrIsCTSAlignmentRequired(arg1, arg_this) kmigmgrIsCTSAlignmentRequired_DISPATCH(arg1, arg_this)
#define kmigmgrIsCTSAlignmentRequired_HAL(arg1, arg_this) kmigmgrIsCTSAlignmentRequired_DISPATCH(arg1, arg_this)
#define kmigmgrRestoreFromBootConfig_FNPTR(pKernelMIGManager) pKernelMIGManager->__kmigmgrRestoreFromBootConfig__
#define kmigmgrRestoreFromBootConfig(pGpu, pKernelMIGManager) kmigmgrRestoreFromBootConfig_DISPATCH(pGpu, pKernelMIGManager)
#define kmigmgrRestoreFromBootConfig_HAL(pGpu, pKernelMIGManager) kmigmgrRestoreFromBootConfig_DISPATCH(pGpu, pKernelMIGManager)
#define kmigmgrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kmigmgrInitMissing(pGpu, pEngstate) kmigmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kmigmgrStatePreInitLocked(pGpu, pEngstate) kmigmgrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kmigmgrStatePreInitUnlocked(pGpu, pEngstate) kmigmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kmigmgrStateInitUnlocked(pGpu, pEngstate) kmigmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmigmgrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kmigmgrStatePreLoad(pGpu, pEngstate, arg3) kmigmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kmigmgrStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateLoad__
#define kmigmgrStateLoad(pGpu, pEngstate, arg3) kmigmgrStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kmigmgrStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define kmigmgrStatePostLoad(pGpu, pEngstate, arg3) kmigmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kmigmgrStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kmigmgrStatePreUnload(pGpu, pEngstate, arg3) kmigmgrStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmigmgrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kmigmgrStatePostUnload(pGpu, pEngstate, arg3) kmigmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmigmgrStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__
#define kmigmgrStateDestroy(pGpu, pEngstate) kmigmgrStateDestroy_DISPATCH(pGpu, pEngstate)
#define kmigmgrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kmigmgrIsPresent(pGpu, pEngstate) kmigmgrIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kmigmgrConstructEngine_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, ENGDESCRIPTOR arg3) {
    return arg_this->__kmigmgrConstructEngine__(arg1, arg_this, arg3);
}

static inline NV_STATUS kmigmgrStateInitLocked_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrStateInitLocked__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrStateUnload_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 flags) {
    return arg_this->__kmigmgrStateUnload__(arg1, arg_this, flags);
}

static inline NV_STATUS kmigmgrLoadStaticInfo_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrLoadStaticInfo__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrSetStaticInfo_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrSetStaticInfo__(arg1, arg_this);
}

static inline void kmigmgrClearStaticInfo_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    arg_this->__kmigmgrClearStaticInfo__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrSaveToPersistenceFromVgpuStaticInfo_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrDeleteGPUInstanceRunlists_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, KERNEL_MIG_GPU_INSTANCE *arg3) {
    return arg_this->__kmigmgrDeleteGPUInstanceRunlists__(arg1, arg_this, arg3);
}

static inline NV_STATUS kmigmgrCreateGPUInstanceRunlists_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, KERNEL_MIG_GPU_INSTANCE *arg3) {
    return arg_this->__kmigmgrCreateGPUInstanceRunlists__(arg1, arg_this, arg3);
}

static inline NV_STATUS kmigmgrRestoreFromPersistence_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrRestoreFromPersistence__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrCreateGPUInstanceCheck_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvBool bMemoryPartitioningNeeded) {
    return arg_this->__kmigmgrCreateGPUInstanceCheck__(arg1, arg_this, bMemoryPartitioningNeeded);
}

static inline NvBool kmigmgrIsDevinitMIGBitSet_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrIsDevinitMIGBitSet__(arg1, arg_this);
}

static inline NvBool kmigmgrIsGPUInstanceCombinationValid_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 gpuInstanceFlag) {
    return arg_this->__kmigmgrIsGPUInstanceCombinationValid__(arg1, arg_this, gpuInstanceFlag);
}

static inline NvBool kmigmgrIsGPUInstanceFlagValid_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 gpuInstanceFlag) {
    return arg_this->__kmigmgrIsGPUInstanceFlagValid__(arg1, arg_this, gpuInstanceFlag);
}

static inline NV_STATUS kmigmgrGenerateComputeInstanceUuid_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 swizzId, NvU32 globalGrIdx, NvUuid *arg5) {
    return arg_this->__kmigmgrGenerateComputeInstanceUuid__(arg1, arg_this, swizzId, globalGrIdx, arg5);
}

static inline NV_STATUS kmigmgrGenerateGPUInstanceUuid_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 swizzId, NvUuid *arg4) {
    return arg_this->__kmigmgrGenerateGPUInstanceUuid__(arg1, arg_this, swizzId, arg4);
}

static inline NV_STATUS kmigmgrCreateComputeInstances_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, KERNEL_MIG_GPU_INSTANCE *arg3, NvBool bQuery, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS arg5, NvU32 *pCIIds, NvBool bCreateCap) {
    return arg_this->__kmigmgrCreateComputeInstances__(arg1, arg_this, arg3, bQuery, arg5, pCIIds, bCreateCap);
}

static inline NvBool kmigmgrIsMemoryPartitioningRequested_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 partitionFlags) {
    return arg_this->__kmigmgrIsMemoryPartitioningRequested__(arg1, arg_this, partitionFlags);
}

static inline NvBool kmigmgrIsMemoryPartitioningNeeded_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 swizzId) {
    return arg_this->__kmigmgrIsMemoryPartitioningNeeded__(arg1, arg_this, swizzId);
}

static inline struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 memSizeFlag) {
    return arg_this->__kmigmgrMemSizeFlagToSwizzIdRange__(arg1, arg_this, memSizeFlag);
}

static inline struct NV_RANGE kmigmgrSwizzIdToSpan_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvU32 swizzId) {
    return arg_this->__kmigmgrSwizzIdToSpan__(arg1, arg_this, swizzId);
}

static inline NV_STATUS kmigmgrSetMIGState_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this, NvBool bMemoryPartitioningNeeded, NvBool bEnable, NvBool bUnload) {
    return arg_this->__kmigmgrSetMIGState__(arg1, arg_this, bMemoryPartitioningNeeded, bEnable, bUnload);
}

static inline NvBool kmigmgrIsCTSAlignmentRequired_DISPATCH(OBJGPU *arg1, struct KernelMIGManager *arg_this) {
    return arg_this->__kmigmgrIsCTSAlignmentRequired__(arg1, arg_this);
}

static inline NV_STATUS kmigmgrRestoreFromBootConfig_DISPATCH(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    return pKernelMIGManager->__kmigmgrRestoreFromBootConfig__(pGpu, pKernelMIGManager);
}

static inline void kmigmgrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    pEngstate->__kmigmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmigmgrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return pEngstate->__kmigmgrStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmigmgrStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return pEngstate->__kmigmgrStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmigmgrStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return pEngstate->__kmigmgrStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmigmgrStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return pEngstate->__kmigmgrStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmigmgrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return pEngstate->__kmigmgrStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kmigmgrStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    pEngstate->__kmigmgrStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kmigmgrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return pEngstate->__kmigmgrIsPresent__(pGpu, pEngstate);
}

void kmigmgrDetectReducedConfig_KERNEL(OBJGPU *arg1, struct KernelMIGManager *arg2);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDetectReducedConfig(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDetectReducedConfig(arg1, arg2) kmigmgrDetectReducedConfig_KERNEL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrDetectReducedConfig_HAL(arg1, arg2) kmigmgrDetectReducedConfig(arg1, arg2)

NV_STATUS kmigmgrGetComputeProfileFromGpcCount_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpcCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);


#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromGpcCount(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpcCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromGpcCount(arg1, arg2, gpcCount, pProfile) kmigmgrGetComputeProfileFromGpcCount_IMPL(arg1, arg2, gpcCount, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

#define kmigmgrGetComputeProfileFromGpcCount_HAL(arg1, arg2, gpcCount, pProfile) kmigmgrGetComputeProfileFromGpcCount(arg1, arg2, gpcCount, pProfile)

NV_STATUS kmigmgrConstructEngine_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, ENGDESCRIPTOR arg3);

NV_STATUS kmigmgrStateInitLocked_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrStateUnload_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 flags);

NV_STATUS kmigmgrLoadStaticInfo_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrLoadStaticInfo_KERNEL(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrSetStaticInfo_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

static inline NV_STATUS kmigmgrSetStaticInfo_46f6a7(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

void kmigmgrClearStaticInfo_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

static inline void kmigmgrClearStaticInfo_b3696a(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    return;
}

NV_STATUS kmigmgrSaveToPersistenceFromVgpuStaticInfo_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

static inline NV_STATUS kmigmgrSaveToPersistenceFromVgpuStaticInfo_46f6a7(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kmigmgrDeleteGPUInstanceRunlists_56cd7a(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    return NV_OK;
}

NV_STATUS kmigmgrDeleteGPUInstanceRunlists_FWCLIENT(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

static inline NV_STATUS kmigmgrCreateGPUInstanceRunlists_56cd7a(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    return NV_OK;
}

NV_STATUS kmigmgrCreateGPUInstanceRunlists_FWCLIENT(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

NV_STATUS kmigmgrRestoreFromPersistence_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrRestoreFromPersistence_PF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrCreateGPUInstanceCheck_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bMemoryPartitioningNeeded);

static inline NV_STATUS kmigmgrCreateGPUInstanceCheck_46f6a7(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bMemoryPartitioningNeeded) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool kmigmgrIsDevinitMIGBitSet_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NvBool kmigmgrIsDevinitMIGBitSet_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2);

static inline NvBool kmigmgrIsDevinitMIGBitSet_491d52(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    return ((NvBool)(0 != 0));
}

NvBool kmigmgrIsGPUInstanceCombinationValid_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag);

NvBool kmigmgrIsGPUInstanceCombinationValid_GH100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag);

static inline NvBool kmigmgrIsGPUInstanceCombinationValid_491d52(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag) {
    return ((NvBool)(0 != 0));
}

NvBool kmigmgrIsGPUInstanceFlagValid_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag);

NvBool kmigmgrIsGPUInstanceFlagValid_GH100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag);

NvBool kmigmgrIsGPUInstanceFlagValid_GB100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag);

static inline NvBool kmigmgrIsGPUInstanceFlagValid_491d52(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 gpuInstanceFlag) {
    return ((NvBool)(0 != 0));
}

NV_STATUS kmigmgrGenerateComputeInstanceUuid_VF(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU32 globalGrIdx, NvUuid *arg5);

static inline NV_STATUS kmigmgrGenerateComputeInstanceUuid_5baef9(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU32 globalGrIdx, NvUuid *arg5) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kmigmgrGenerateGPUInstanceUuid_VF(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvUuid *arg4);

static inline NV_STATUS kmigmgrGenerateGPUInstanceUuid_5baef9(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvUuid *arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kmigmgrCreateComputeInstances_VF(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvBool bQuery, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS arg5, NvU32 *pCIIds, NvBool bCreateCap);

NV_STATUS kmigmgrCreateComputeInstances_FWCLIENT(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvBool bQuery, KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS arg5, NvU32 *pCIIds, NvBool bCreateCap);

NvBool kmigmgrIsMemoryPartitioningRequested_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 partitionFlags);

static inline NvBool kmigmgrIsMemoryPartitioningRequested_491d52(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 partitionFlags) {
    return ((NvBool)(0 != 0));
}

NvBool kmigmgrIsMemoryPartitioningNeeded_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

static inline NvBool kmigmgrIsMemoryPartitioningNeeded_491d52(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    return ((NvBool)(0 != 0));
}

static inline struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_d64cd6(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 memSizeFlag) {
    return NV_RANGE_EMPTY;
}

struct NV_RANGE kmigmgrMemSizeFlagToSwizzIdRange_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 memSizeFlag);

struct NV_RANGE kmigmgrSwizzIdToSpan_GA100(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

static inline struct NV_RANGE kmigmgrSwizzIdToSpan_d64cd6(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    return NV_RANGE_EMPTY;
}

NV_STATUS kmigmgrSetMIGState_VF(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bMemoryPartitioningNeeded, NvBool bEnable, NvBool bUnload);

NV_STATUS kmigmgrSetMIGState_FWCLIENT(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bMemoryPartitioningNeeded, NvBool bEnable, NvBool bUnload);

static inline NvBool kmigmgrIsCTSAlignmentRequired_cbe027(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    return ((NvBool)(0 == 0));
}

NvBool kmigmgrIsCTSAlignmentRequired_VF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NvBool kmigmgrIsCTSAlignmentRequired_PF(OBJGPU *arg1, struct KernelMIGManager *arg2);

NV_STATUS kmigmgrRestoreFromBootConfig_PF(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager);

static inline NV_STATUS kmigmgrRestoreFromBootConfig_56cd7a(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    return NV_OK;
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

static inline NvU64 kmigmgrGetValidGlobalCTSIdMask(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    return pKernelMIGManager_PRIVATE->validGlobalCTSIdMask;
}

static inline void kmigmgrSetValidGlobalCTSIdMask(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvU64 validGlobalCTSIdMask) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    pKernelMIGManager_PRIVATE->validGlobalCTSIdMask = validGlobalCTSIdMask;
}

static inline NvU64 kmigmgrGetValidGlobalGfxCTSIdMask(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    return pKernelMIGManager_PRIVATE->validGlobalGfxCTSIdMask;
}

static inline void kmigmgrSetValidGlobalGfxCTSIdMask(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, NvU64 validGlobalGfxCTSIdMask) {
    struct KernelMIGManager_PRIVATE *pKernelMIGManager_PRIVATE = (struct KernelMIGManager_PRIVATE *)pKernelMIGManager;
    pKernelMIGManager_PRIVATE->validGlobalGfxCTSIdMask = validGlobalGfxCTSIdMask;
}

NV_STATUS kmigmgrIncRefCount_IMPL(struct RsShared *arg1);

#define kmigmgrIncRefCount(arg1) kmigmgrIncRefCount_IMPL(arg1)
NV_STATUS kmigmgrDecRefCount_IMPL(struct RsShared *arg1);

#define kmigmgrDecRefCount(arg1) kmigmgrDecRefCount_IMPL(arg1)
struct MIG_INSTANCE_REF kmigmgrMakeGIReference_IMPL(KERNEL_MIG_GPU_INSTANCE *arg1);

#define kmigmgrMakeGIReference(arg1) kmigmgrMakeGIReference_IMPL(arg1)
struct MIG_INSTANCE_REF kmigmgrMakeCIReference_IMPL(KERNEL_MIG_GPU_INSTANCE *arg1, MIG_COMPUTE_INSTANCE *arg2);

#define kmigmgrMakeCIReference(arg1, arg2) kmigmgrMakeCIReference_IMPL(arg1, arg2)
NV_STATUS kmigmgrEngineTypeXlate_IMPL(union ENGTYPE_BIT_VECTOR *pSrc, RM_ENGINE_TYPE srcEngineType, union ENGTYPE_BIT_VECTOR *pDst, RM_ENGINE_TYPE *pDstEngineType);

#define kmigmgrEngineTypeXlate(pSrc, srcEngineType, pDst, pDstEngineType) kmigmgrEngineTypeXlate_IMPL(pSrc, srcEngineType, pDst, pDstEngineType)
NV_STATUS kmigmgrEngBitVectorXlate_IMPL(union ENGTYPE_BIT_VECTOR *pSrcRef, union ENGTYPE_BIT_VECTOR *pSrc, union ENGTYPE_BIT_VECTOR *pDstRef, union ENGTYPE_BIT_VECTOR *pDst);

#define kmigmgrEngBitVectorXlate(pSrcRef, pSrc, pDstRef, pDst) kmigmgrEngBitVectorXlate_IMPL(pSrcRef, pSrc, pDstRef, pDst)
NvBool kmigmgrIsInstanceAttributionIdValid_IMPL(NvU16 id);

#define kmigmgrIsInstanceAttributionIdValid(id) kmigmgrIsInstanceAttributionIdValid_IMPL(id)
struct MIG_INSTANCE_REF kmigmgrMakeNoMIGReference_IMPL(void);

#define kmigmgrMakeNoMIGReference() kmigmgrMakeNoMIGReference_IMPL()
NvBool kmigmgrIsMIGReferenceValid_IMPL(struct MIG_INSTANCE_REF *arg1);

#define kmigmgrIsMIGReferenceValid(arg1) kmigmgrIsMIGReferenceValid_IMPL(arg1)
NvBool kmigmgrAreMIGReferencesSame_IMPL(struct MIG_INSTANCE_REF *arg1, struct MIG_INSTANCE_REF *arg2);

#define kmigmgrAreMIGReferencesSame(arg1, arg2) kmigmgrAreMIGReferencesSame_IMPL(arg1, arg2)
NvU32 kmigmgrCountEnginesOfType_IMPL(const union ENGTYPE_BIT_VECTOR *arg1, RM_ENGINE_TYPE arg2);

#define kmigmgrCountEnginesOfType(arg1, arg2) kmigmgrCountEnginesOfType_IMPL(arg1, arg2)
NvU32 kmigmgrCountEnginesInRange_IMPL(const union ENGTYPE_BIT_VECTOR *arg1, struct NV_RANGE arg2);

#define kmigmgrCountEnginesInRange(arg1, arg2) kmigmgrCountEnginesInRange_IMPL(arg1, arg2)
struct NV_RANGE kmigmgrGetAsyncCERange_IMPL(OBJGPU *arg1);

#define kmigmgrGetAsyncCERange(arg1) kmigmgrGetAsyncCERange_IMPL(arg1)
NvU16 kmigmgrGetAttributionIdFromMIGReference_IMPL(struct MIG_INSTANCE_REF arg1);

#define kmigmgrGetAttributionIdFromMIGReference(arg1) kmigmgrGetAttributionIdFromMIGReference_IMPL(arg1)
NV_STATUS kmigmgrAllocateInstanceEngines_IMPL(union ENGTYPE_BIT_VECTOR *pSourceEngines, NvBool bShared, struct NV_RANGE engTypeRange, NvU32 reqEngCount, union ENGTYPE_BIT_VECTOR *pOutEngines, union ENGTYPE_BIT_VECTOR *pExclusiveEngines, union ENGTYPE_BIT_VECTOR *pSharedEngines, union ENGTYPE_BIT_VECTOR *pAllocatableEngines);

#define kmigmgrAllocateInstanceEngines(pSourceEngines, bShared, engTypeRange, reqEngCount, pOutEngines, pExclusiveEngines, pSharedEngines, pAllocatableEngines) kmigmgrAllocateInstanceEngines_IMPL(pSourceEngines, bShared, engTypeRange, reqEngCount, pOutEngines, pExclusiveEngines, pSharedEngines, pAllocatableEngines)
void kmigmgrGetLocalEngineMask_IMPL(OBJGPU *arg1, union ENGTYPE_BIT_VECTOR *pPhysicalEngineMask, union ENGTYPE_BIT_VECTOR *pLocalEngineMask);

#define kmigmgrGetLocalEngineMask(arg1, pPhysicalEngineMask, pLocalEngineMask) kmigmgrGetLocalEngineMask_IMPL(arg1, pPhysicalEngineMask, pLocalEngineMask)
NV_STATUS kmigmgrAllocGPUInstanceHandles_IMPL(OBJGPU *arg1, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE *arg3);

#define kmigmgrAllocGPUInstanceHandles(arg1, swizzId, arg3) kmigmgrAllocGPUInstanceHandles_IMPL(arg1, swizzId, arg3)
void kmigmgrFreeGPUInstanceHandles_IMPL(KERNEL_MIG_GPU_INSTANCE *arg1);

#define kmigmgrFreeGPUInstanceHandles(arg1) kmigmgrFreeGPUInstanceHandles_IMPL(arg1)
NvBool kmigmgrIsGPUInstanceReadyToBeDestroyed_IMPL(KERNEL_MIG_GPU_INSTANCE *arg1);

#define kmigmgrIsGPUInstanceReadyToBeDestroyed(arg1) kmigmgrIsGPUInstanceReadyToBeDestroyed_IMPL(arg1)
void kmigmgrDestruct_IMPL(struct KernelMIGManager *arg1);

#define __nvoc_kmigmgrDestruct(arg1) kmigmgrDestruct_IMPL(arg1)
void kmigmgrInitRegistryOverrides_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrInitRegistryOverrides(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitRegistryOverrides(arg1, arg2) kmigmgrInitRegistryOverrides_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

KERNEL_MIG_GPU_INSTANCE *kmigmgrGetMIGGpuInstanceSlot_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 i);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline KERNEL_MIG_GPU_INSTANCE *kmigmgrGetMIGGpuInstanceSlot(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 i) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMIGGpuInstanceSlot(arg1, arg2, i) kmigmgrGetMIGGpuInstanceSlot_IMPL(arg1, arg2, i)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGSupported_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGSupported(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGSupported(arg1, arg2) kmigmgrIsMIGSupported_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGEnabled_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGEnabled(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGEnabled(arg1, arg2) kmigmgrIsMIGEnabled_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGGpuInstancingEnabled_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGGpuInstancingEnabled(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGGpuInstancingEnabled(arg1, arg2) kmigmgrIsMIGGpuInstancingEnabled_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGMemPartitioningEnabled_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGMemPartitioningEnabled(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGMemPartitioningEnabled(arg1, arg2) kmigmgrIsMIGMemPartitioningEnabled_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

const KERNEL_MIG_MANAGER_STATIC_INFO *kmigmgrGetStaticInfo_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline const KERNEL_MIG_MANAGER_STATIC_INFO *kmigmgrGetStaticInfo(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetStaticInfo(arg1, arg2) kmigmgrGetStaticInfo_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSaveToPersistence_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSaveToPersistence(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSaveToPersistence(arg1, arg2) kmigmgrSaveToPersistence_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDisableWatchdog_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDisableWatchdog(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDisableWatchdog(arg1, arg2) kmigmgrDisableWatchdog_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrRestoreWatchdog_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrRestoreWatchdog(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrRestoreWatchdog(arg1, arg2) kmigmgrRestoreWatchdog_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetSwizzIdInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetSwizzIdInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetSwizzIdInUse(arg1, arg2, swizzId) kmigmgrSetSwizzIdInUse_IMPL(arg1, arg2, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrClearSwizzIdInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrClearSwizzIdInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearSwizzIdInUse(arg1, arg2, swizzId) kmigmgrClearSwizzIdInUse_IMPL(arg1, arg2, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsSwizzIdInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsSwizzIdInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsSwizzIdInUse(arg1, arg2, swizzId) kmigmgrIsSwizzIdInUse_IMPL(arg1, arg2, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInvalidSwizzIdMask_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU64 *pUnsupportedSwizzIdMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInvalidSwizzIdMask(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU64 *pUnsupportedSwizzIdMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInvalidSwizzIdMask(arg1, arg2, swizzId, pUnsupportedSwizzIdMask) kmigmgrGetInvalidSwizzIdMask_IMPL(arg1, arg2, swizzId, pUnsupportedSwizzIdMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsMIGNvlinkP2PSupported_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsMIGNvlinkP2PSupported(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsMIGNvlinkP2PSupported(arg1, arg2) kmigmgrIsMIGNvlinkP2PSupported_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU64 kmigmgrGetSwizzIdInUseMask_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU64 kmigmgrGetSwizzIdInUseMask(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSwizzIdInUseMask(arg1, arg2) kmigmgrGetSwizzIdInUseMask_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetEnginesInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, union ENGTYPE_BIT_VECTOR *pEngines);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetEnginesInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, union ENGTYPE_BIT_VECTOR *pEngines) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetEnginesInUse(arg1, arg2, pEngines) kmigmgrSetEnginesInUse_IMPL(arg1, arg2, pEngines)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrClearEnginesInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, union ENGTYPE_BIT_VECTOR *pEngines);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrClearEnginesInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, union ENGTYPE_BIT_VECTOR *pEngines) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearEnginesInUse(arg1, arg2, pEngines) kmigmgrClearEnginesInUse_IMPL(arg1, arg2, pEngines)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEngineInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEngineInUse(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEngineInUse(arg1, arg2, rmEngineType) kmigmgrIsEngineInUse_IMPL(arg1, arg2, rmEngineType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEnginePartitionable_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEnginePartitionable(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEnginePartitionable(arg1, arg2, rmEngineType) kmigmgrIsEnginePartitionable_IMPL(arg1, arg2, rmEngineType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsEngineInInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE globalRmEngType, struct MIG_INSTANCE_REF arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsEngineInInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE globalRmEngType, struct MIG_INSTANCE_REF arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsEngineInInstance(arg1, arg2, globalRmEngType, arg4) kmigmgrIsEngineInInstance_IMPL(arg1, arg2, globalRmEngType, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsLocalEngineInInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE localRmEngType, struct MIG_INSTANCE_REF arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsLocalEngineInInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE localRmEngType, struct MIG_INSTANCE_REF arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsLocalEngineInInstance(arg1, arg2, localRmEngType, arg4) kmigmgrIsLocalEngineInInstance_IMPL(arg1, arg2, localRmEngType, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrCreateGPUInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU8 *pUuid, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg5, NvBool bValid, NvBool bCreateCap);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrCreateGPUInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU8 *pUuid, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg5, NvBool bValid, NvBool bCreateCap) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrCreateGPUInstance(arg1, arg2, swizzId, pUuid, arg5, bValid, bCreateCap) kmigmgrCreateGPUInstance_IMPL(arg1, arg2, swizzId, pUuid, arg5, bValid, bCreateCap)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGPUInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvBool bUnload);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGPUInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvBool bUnload) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGPUInstance(arg1, arg2, swizzId, bUnload) kmigmgrInvalidateGPUInstance_IMPL(arg1, arg2, swizzId, bUnload)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceScrubber_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceScrubber(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceScrubber(arg1, arg2, arg3) kmigmgrInitGPUInstanceScrubber_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceScrubber_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceScrubber(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceScrubber(arg1, arg2, arg3) kmigmgrDestroyGPUInstanceScrubber_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceBufPools_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceBufPools(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceBufPools(arg1, arg2, arg3) kmigmgrInitGPUInstanceBufPools_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceGrBufPools_IMPL(OBJGPU *pGpu, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceGrBufPools(OBJGPU *pGpu, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceGrBufPools(pGpu, arg2, arg3) kmigmgrInitGPUInstanceGrBufPools_IMPL(pGpu, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceGrBufPools_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceGrBufPools(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceGrBufPools(arg1, arg2, arg3) kmigmgrDestroyGPUInstanceGrBufPools_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstancePool_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstancePool(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstancePool(arg1, arg2, arg3) kmigmgrInitGPUInstancePool_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstancePool_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstancePool(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstancePool(arg1, arg2, arg3) kmigmgrDestroyGPUInstancePool_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInitGPUInstanceRunlistBufPools_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInitGPUInstanceRunlistBufPools(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceRunlistBufPools(arg1, arg2, arg3) kmigmgrInitGPUInstanceRunlistBufPools_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrDestroyGPUInstanceRunlistBufPools_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrDestroyGPUInstanceRunlistBufPools(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDestroyGPUInstanceRunlistBufPools(arg1, arg2, arg3) kmigmgrDestroyGPUInstanceRunlistBufPools_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrPrintSubscribingClients_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrPrintSubscribingClients(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrPrintSubscribingClients(arg1, arg2, swizzId) kmigmgrPrintSubscribingClients_IMPL(arg1, arg2, swizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrInitGPUInstanceInfo_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrInitGPUInstanceInfo(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInitGPUInstanceInfo(arg1, arg2, arg3) kmigmgrInitGPUInstanceInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrTrimInstanceRunlistBufPools_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrTrimInstanceRunlistBufPools(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrTrimInstanceRunlistBufPools(arg1, arg2, arg3) kmigmgrTrimInstanceRunlistBufPools_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetDeviceProfilingInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetDeviceProfilingInUse(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetDeviceProfilingInUse(arg1, arg2) kmigmgrSetDeviceProfilingInUse_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrClearDeviceProfilingInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrClearDeviceProfilingInUse(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrClearDeviceProfilingInUse(arg1, arg2) kmigmgrClearDeviceProfilingInUse_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsDeviceProfilingInUse_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsDeviceProfilingInUse(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsDeviceProfilingInUse(arg1, arg2) kmigmgrIsDeviceProfilingInUse_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsDeviceUsingDeviceProfiling_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsDeviceUsingDeviceProfiling(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsDeviceUsingDeviceProfiling(arg1, arg2, pDevice) kmigmgrIsDeviceUsingDeviceProfiling_IMPL(arg1, arg2, pDevice)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrEnableAllLCEs_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bEnableAllLCEs);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrEnableAllLCEs(OBJGPU *arg1, struct KernelMIGManager *arg2, NvBool bEnableAllLCEs) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrEnableAllLCEs(arg1, arg2, bEnableAllLCEs) kmigmgrEnableAllLCEs_IMPL(arg1, arg2, bEnableAllLCEs)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInstanceRefFromDevice_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *arg3, struct MIG_INSTANCE_REF *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInstanceRefFromDevice(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *arg3, struct MIG_INSTANCE_REF *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInstanceRefFromDevice(arg1, arg2, arg3, arg4) kmigmgrGetInstanceRefFromDevice_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetMemoryPartitionHeapFromDevice_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *arg3, struct Heap **arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetMemoryPartitionHeapFromDevice(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *arg3, struct Heap **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMemoryPartitionHeapFromDevice(arg1, arg2, arg3, arg4) kmigmgrGetMemoryPartitionHeapFromDevice_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetSwizzIdFromDevice_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice, NvU32 *pSwizzId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSwizzIdFromDevice(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice, NvU32 *pSwizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSwizzIdFromDevice(arg1, arg2, pDevice, pSwizzId) kmigmgrGetSwizzIdFromDevice_IMPL(arg1, arg2, pDevice, pSwizzId)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrPrintGPUInstanceInfo_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrPrintGPUInstanceInfo(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrPrintGPUInstanceInfo(arg1, arg2, arg3) kmigmgrPrintGPUInstanceInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetGPUInstanceInfo_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU8 *pUuid, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg5);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetGPUInstanceInfo(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, NvU8 *pUuid, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetGPUInstanceInfo(arg1, arg2, swizzId, pUuid, arg5) kmigmgrSetGPUInstanceInfo_IMPL(arg1, arg2, swizzId, pUuid, arg5)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGPUInstanceInfo_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE **arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGPUInstanceInfo(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, KERNEL_MIG_GPU_INSTANCE **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGPUInstanceInfo(arg1, arg2, swizzId, arg4) kmigmgrGetGPUInstanceInfo_IMPL(arg1, arg2, swizzId, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetLocalToGlobalEngineType_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, struct MIG_INSTANCE_REF arg3, RM_ENGINE_TYPE localEngType, RM_ENGINE_TYPE *pGlobalEngType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetLocalToGlobalEngineType(OBJGPU *arg1, struct KernelMIGManager *arg2, struct MIG_INSTANCE_REF arg3, RM_ENGINE_TYPE localEngType, RM_ENGINE_TYPE *pGlobalEngType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetLocalToGlobalEngineType(arg1, arg2, arg3, localEngType, pGlobalEngType) kmigmgrGetLocalToGlobalEngineType_IMPL(arg1, arg2, arg3, localEngType, pGlobalEngType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGlobalToLocalEngineType_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, struct MIG_INSTANCE_REF arg3, RM_ENGINE_TYPE globalEngType, RM_ENGINE_TYPE *pLocalEngType);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGlobalToLocalEngineType(OBJGPU *arg1, struct KernelMIGManager *arg2, struct MIG_INSTANCE_REF arg3, RM_ENGINE_TYPE globalEngType, RM_ENGINE_TYPE *pLocalEngType) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGlobalToLocalEngineType(arg1, arg2, arg3, globalEngType, pLocalEngType) kmigmgrGetGlobalToLocalEngineType_IMPL(arg1, arg2, arg3, globalEngType, pLocalEngType)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrFilterEngineList_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, struct Subdevice *arg3, RM_ENGINE_TYPE *pEngineTypes, NvU32 *pEngineCount);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrFilterEngineList(OBJGPU *arg1, struct KernelMIGManager *arg2, struct Subdevice *arg3, RM_ENGINE_TYPE *pEngineTypes, NvU32 *pEngineCount) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFilterEngineList(arg1, arg2, arg3, pEngineTypes, pEngineCount) kmigmgrFilterEngineList_IMPL(arg1, arg2, arg3, pEngineTypes, pEngineCount)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrFilterEnginePartnerList_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, struct Subdevice *arg3, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrFilterEnginePartnerList(OBJGPU *arg1, struct KernelMIGManager *arg2, struct Subdevice *arg3, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFilterEnginePartnerList(arg1, arg2, arg3, arg4) kmigmgrFilterEnginePartnerList_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetProfileByPartitionFlag_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 partitionFlag, const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO **arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetProfileByPartitionFlag(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 partitionFlag, const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetProfileByPartitionFlag(arg1, arg2, partitionFlag, arg4) kmigmgrGetProfileByPartitionFlag_IMPL(arg1, arg2, partitionFlag, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSaveComputeInstances_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, GPUMGR_SAVE_COMPUTE_INSTANCE *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSaveComputeInstances(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, GPUMGR_SAVE_COMPUTE_INSTANCE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSaveComputeInstances(arg1, arg2, arg3, arg4) kmigmgrSaveComputeInstances_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSetPartitioningMode_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSetPartitioningMode(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSetPartitioningMode(arg1, arg2) kmigmgrSetPartitioningMode_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetMIGReferenceFromEngineType_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType, struct MIG_INSTANCE_REF *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetMIGReferenceFromEngineType(OBJGPU *arg1, struct KernelMIGManager *arg2, RM_ENGINE_TYPE rmEngineType, struct MIG_INSTANCE_REF *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetMIGReferenceFromEngineType(arg1, arg2, rmEngineType, arg4) kmigmgrGetMIGReferenceFromEngineType_IMPL(arg1, arg2, rmEngineType, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetSmallestGpuInstanceSize_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 *pComputeSizeFlag);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSmallestGpuInstanceSize(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 *pComputeSizeFlag) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSmallestGpuInstanceSize(arg1, arg2, pComputeSizeFlag) kmigmgrGetSmallestGpuInstanceSize_IMPL(arg1, arg2, pComputeSizeFlag)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetGPUInstanceScrubberCe_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice, NvU32 *ceInst);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetGPUInstanceScrubberCe(OBJGPU *arg1, struct KernelMIGManager *arg2, Device *pDevice, NvU32 *ceInst) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetGPUInstanceScrubberCe(arg1, arg2, pDevice, ceInst) kmigmgrGetGPUInstanceScrubberCe_IMPL(arg1, arg2, pDevice, ceInst)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDescribeGPUInstances_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDescribeGPUInstances(OBJGPU *arg1, struct KernelMIGManager *arg2, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDescribeGPUInstances(arg1, arg2, arg3) kmigmgrDescribeGPUInstances_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrSwizzIdToResourceAllocation_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg4, KERNEL_MIG_GPU_INSTANCE *arg5, MIG_RESOURCE_ALLOCATION *arg6);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrSwizzIdToResourceAllocation(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, KMIGMGR_CREATE_GPU_INSTANCE_PARAMS arg4, KERNEL_MIG_GPU_INSTANCE *arg5, MIG_RESOURCE_ALLOCATION *arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSwizzIdToResourceAllocation(arg1, arg2, swizzId, arg4, arg5, arg6) kmigmgrSwizzIdToResourceAllocation_IMPL(arg1, arg2, swizzId, arg4, arg5, arg6)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrAllocComputeInstanceHandles_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrAllocComputeInstanceHandles(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrAllocComputeInstanceHandles(arg1, arg2, arg3, arg4) kmigmgrAllocComputeInstanceHandles_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrFreeComputeInstanceHandles_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrFreeComputeInstanceHandles(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrFreeComputeInstanceHandles(arg1, arg2, arg3, arg4) kmigmgrFreeComputeInstanceHandles_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrReleaseComputeInstanceEngines_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline void kmigmgrReleaseComputeInstanceEngines(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, MIG_COMPUTE_INSTANCE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrReleaseComputeInstanceEngines(arg1, arg2, arg3, arg4) kmigmgrReleaseComputeInstanceEngines_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrDeleteComputeInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 CIId, NvBool bUnload);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrDeleteComputeInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 CIId, NvBool bUnload) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrDeleteComputeInstance(arg1, arg2, arg3, CIId, bUnload) kmigmgrDeleteComputeInstance_IMPL(arg1, arg2, arg3, CIId, bUnload)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrConfigureGPUInstance_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, const KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi, NvU32 updateEngMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrConfigureGPUInstance(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 swizzId, const KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi, NvU32 updateEngMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrConfigureGPUInstance(arg1, arg2, swizzId, pConfigRequestPerCi, updateEngMask) kmigmgrConfigureGPUInstance_IMPL(arg1, arg2, swizzId, pConfigRequestPerCi, updateEngMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGrGpcMapping_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 grIdx);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGrGpcMapping(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGrGpcMapping(arg1, arg2, arg3, grIdx) kmigmgrInvalidateGrGpcMapping_IMPL(arg1, arg2, arg3, grIdx)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrInvalidateGr_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 grIdx);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrInvalidateGr(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrInvalidateGr(arg1, arg2, arg3, grIdx) kmigmgrInvalidateGr_IMPL(arg1, arg2, arg3, grIdx)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetNextComputeSize_IMPL(NvBool bGetNextSmallest, NvU32 computeSize);

#define kmigmgrGetNextComputeSize(bGetNextSmallest, computeSize) kmigmgrGetNextComputeSize_IMPL(bGetNextSmallest, computeSize)
NV_STATUS kmigmgrGetSkylineFromSize_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO **ppSkyline);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSkylineFromSize(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO **ppSkyline) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSkylineFromSize(arg1, arg2, computeSize, ppSkyline) kmigmgrGetSkylineFromSize_IMPL(arg1, arg2, computeSize, ppSkyline)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromSize_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromSize(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromSize(arg1, arg2, computeSize, pProfile) kmigmgrGetComputeProfileFromSize_IMPL(arg1, arg2, computeSize, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileForRequest_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 smCountRequest, NvU32 gpcCountRequest, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileForRequest(OBJGPU *arg1, struct KernelMIGManager *arg2, KERNEL_MIG_GPU_INSTANCE *arg3, NvU32 smCountRequest, NvU32 gpcCountRequest, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileForRequest(arg1, arg2, arg3, smCountRequest, gpcCountRequest, pProfile) kmigmgrGetComputeProfileForRequest_IMPL(arg1, arg2, arg3, smCountRequest, gpcCountRequest, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromSmCount_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 smCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromSmCount(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 smCount, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromSmCount(arg1, arg2, smCount, pProfile) kmigmgrGetComputeProfileFromSmCount_IMPL(arg1, arg2, smCount, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetComputeProfileFromCTSId_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetComputeProfileFromCTSId(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId, NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetComputeProfileFromCTSId(arg1, arg2, ctsId, pProfile) kmigmgrGetComputeProfileFromCTSId_IMPL(arg1, arg2, ctsId, pProfile)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetInvalidCTSIdMask_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId, NvU64 *pInvalidCTSIdMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetInvalidCTSIdMask(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId, NvU64 *pInvalidCTSIdMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetInvalidCTSIdMask(arg1, arg2, ctsId, pInvalidCTSIdMask) kmigmgrGetInvalidCTSIdMask_IMPL(arg1, arg2, ctsId, pInvalidCTSIdMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

struct NV_RANGE kmigmgrComputeProfileSizeToCTSIdRange_IMPL(NvU32 computeSize);

#define kmigmgrComputeProfileSizeToCTSIdRange(computeSize) kmigmgrComputeProfileSizeToCTSIdRange_IMPL(computeSize)
struct NV_RANGE kmigmgrCtsIdToSpan_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline struct NV_RANGE kmigmgrCtsIdToSpan(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    struct NV_RANGE ret;
    portMemSet(&ret, 0, sizeof(struct NV_RANGE));
    return ret;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrCtsIdToSpan(arg1, arg2, ctsId) kmigmgrCtsIdToSpan_IMPL(arg1, arg2, ctsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetFreeCTSId_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 *pCtsId, NvU64 globalValidCtsMask, NvU64 globalValidGfxCtsMask, NvU64 ctsIdsInUseMask, NvU32 profileSize, NvBool bRestrictWithGfx, NvBool bGfxRequested);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetFreeCTSId(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 *pCtsId, NvU64 globalValidCtsMask, NvU64 globalValidGfxCtsMask, NvU64 ctsIdsInUseMask, NvU32 profileSize, NvBool bRestrictWithGfx, NvBool bGfxRequested) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetFreeCTSId(arg1, arg2, pCtsId, globalValidCtsMask, globalValidGfxCtsMask, ctsIdsInUseMask, profileSize, bRestrictWithGfx, bGfxRequested) kmigmgrGetFreeCTSId_IMPL(arg1, arg2, pCtsId, globalValidCtsMask, globalValidGfxCtsMask, ctsIdsInUseMask, profileSize, bRestrictWithGfx, bGfxRequested)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetComputeSizeFromCTSId_IMPL(NvU32 ctsId);

#define kmigmgrGetComputeSizeFromCTSId(ctsId) kmigmgrGetComputeSizeFromCTSId_IMPL(ctsId)
NvU32 kmigmgrSmallestComputeProfileSize_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU32 kmigmgrSmallestComputeProfileSize(OBJGPU *arg1, struct KernelMIGManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrSmallestComputeProfileSize(arg1, arg2) kmigmgrSmallestComputeProfileSize_IMPL(arg1, arg2)
#endif //__nvoc_kernel_mig_manager_h_disabled

void kmigmgrSetCTSIdInUse_IMPL(KERNEL_MIG_GPU_INSTANCE *arg1, NvU32 ctsId, NvU32 grId, NvBool bInUse);

#define kmigmgrSetCTSIdInUse(arg1, ctsId, grId, bInUse) kmigmgrSetCTSIdInUse_IMPL(arg1, ctsId, grId, bInUse)
NV_STATUS kmigmgrXlateSpanStartToCTSId_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, NvU32 spanStart, NvU32 *pCtsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrXlateSpanStartToCTSId(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 computeSize, NvU32 spanStart, NvU32 *pCtsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrXlateSpanStartToCTSId(arg1, arg2, computeSize, spanStart, pCtsId) kmigmgrXlateSpanStartToCTSId_IMPL(arg1, arg2, computeSize, spanStart, pCtsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NV_STATUS kmigmgrGetSlotBasisMask_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU64 *pMask);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NV_STATUS kmigmgrGetSlotBasisMask(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU64 *pMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSlotBasisMask(arg1, arg2, pMask) kmigmgrGetSlotBasisMask_IMPL(arg1, arg2, pMask)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvU32 kmigmgrGetSpanStartFromCTSId_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvU32 kmigmgrGetSpanStartFromCTSId(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU32 ctsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrGetSpanStartFromCTSId(arg1, arg2, ctsId) kmigmgrGetSpanStartFromCTSId_IMPL(arg1, arg2, ctsId)
#endif //__nvoc_kernel_mig_manager_h_disabled

NvBool kmigmgrIsCTSIdAvailable_IMPL(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU64 ctsIdValidMask, NvU64 ctsIdInUseMask, NvU32 ctsId);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsCTSIdAvailable(OBJGPU *arg1, struct KernelMIGManager *arg2, NvU64 ctsIdValidMask, NvU64 ctsIdInUseMask, NvU32 ctsId) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsCTSIdAvailable(arg1, arg2, ctsIdValidMask, ctsIdInUseMask, ctsId) kmigmgrIsCTSIdAvailable_IMPL(arg1, arg2, ctsIdValidMask, ctsIdInUseMask, ctsId)
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

NvBool kmigmgrIsPartitionVeidAllocationContiguous_IMPL(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, KERNEL_MIG_GPU_INSTANCE *arg3);

#ifdef __nvoc_kernel_mig_manager_h_disabled
static inline NvBool kmigmgrIsPartitionVeidAllocationContiguous(OBJGPU *pGpu, struct KernelMIGManager *pKernelMIGManager, KERNEL_MIG_GPU_INSTANCE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelMIGManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_mig_manager_h_disabled
#define kmigmgrIsPartitionVeidAllocationContiguous(pGpu, pKernelMIGManager, arg3) kmigmgrIsPartitionVeidAllocationContiguous_IMPL(pGpu, pKernelMIGManager, arg3)
#endif //__nvoc_kernel_mig_manager_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_MIG_MANAGER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_MIG_MANAGER_NVOC_H_
