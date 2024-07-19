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

#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/mig_mgr/compute_instance_subscription.h"
#include "kernel/gpu/mig_mgr/gpu_instance_subscription.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu/ce/kernel_ce.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/bus/kern_bus.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "kernel/gpu/gpu_fabric_probe.h"
#include "rmapi/client.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "vgpu/rpc.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/core/locks.h"
#include "class/cl503b.h"
#include "nv_ref.h"
#include "platform/sli/sli.h"
#include "nvrm_registry.h"

#include "kernel/gpu/ccu/kernel_ccu.h"

#include "class/cl503c.h"

struct KERNEL_MIG_MANAGER_PRIVATE_DATA
{
    NvBool bInitialized;
    KERNEL_MIG_MANAGER_STATIC_INFO staticInfo;
};

typedef struct
{
    struct
    {
        NvBool bValid;
        NvU32  flags;
        NV_RANGE placement;
    } GIs[NV2080_CTRL_GPU_MAX_PARTITIONS];
    struct
    {
        NvBool bValid;
        NvU32 flags;
        NvU32 ceCount;
        NvU32 nvEncCount;
        NvU32 nvDecCount;
        NvU32 nvJpgCount;
        NvU32 ofaCount;
        NvU32 spanStart;
        NvU32 GIIdx;
    } CIs[NVC637_CTRL_MAX_EXEC_PARTITIONS];
} MIG_BOOT_CONFIG;

/*!
 * @brief   Function to increment gi/ci refcount
 */
NV_STATUS
kmigmgrIncRefCount_IMPL
(
    RsShared *pShared
)
{
    NvS32 refCount;

    NV_ASSERT_OR_RETURN(pShared != NULL, NV_ERR_INVALID_ARGUMENT);

    serverRefShare(&g_resServ, pShared);
    refCount = serverGetShareRefCount(&g_resServ, pShared);

    // Make sure refCount didn't overflow
    NV_ASSERT_OR_RETURN(refCount > 0, NV_ERR_INVALID_STATE);
    return NV_OK;
}

/*!
 * @brief   Function to decrement gi/ci refcount
 */
NV_STATUS
kmigmgrDecRefCount_IMPL
(
    RsShared *pShared
)
{
    NvS32 refCount;

    NV_ASSERT_OR_RETURN(pShared != NULL, NV_ERR_INVALID_ARGUMENT);

    refCount = serverGetShareRefCount(&g_resServ, pShared);
    serverFreeShare(&g_resServ, pShared);
    --refCount;

    // Make sure refCount didn't underflow
    NV_ASSERT_OR_RETURN(refCount > 0, NV_ERR_INVALID_STATE);
    return NV_OK;
}

/*! @brief create a reference to a single GPU instance, no compute instance */
MIG_INSTANCE_REF
kmigmgrMakeGIReference_IMPL
(
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    MIG_INSTANCE_REF ref = { pKernelMIGGpuInstance, NULL };
    return ref;
}

/*! @brief create a reference to a compute instance */
MIG_INSTANCE_REF
kmigmgrMakeCIReference_IMPL
(
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance
)
{
    MIG_INSTANCE_REF ref = { pKernelMIGGpuInstance, pMIGComputeInstance };
    return ref;
}

/*! @brief create a Ref referencing no GI/CI */
MIG_INSTANCE_REF
kmigmgrMakeNoMIGReference_IMPL(void)
{
    MIG_INSTANCE_REF ref = { NULL, NULL };
    return ref;
}

/*! @brief check if MIG attribution id is valid for max instances */
NvBool
kmigmgrIsInstanceAttributionIdValid_IMPL
(
    NvU16 id
)
{
    return (((id / KMIGMGR_MAX_GPU_SWIZZID) <= KMIGMGR_MAX_GPU_INSTANCES) &&
            ((id % KMIGMGR_MAX_GPU_SWIZZID) <= KMIGMGR_MAX_COMPUTE_INSTANCES));
}

/*! @brief check if existing valid instance ref is passed in */
NvBool
kmigmgrIsMIGReferenceValid_IMPL
(
    MIG_INSTANCE_REF *pRef
)
{
    // Invalid argument
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pRef != NULL, NV_FALSE);
    // Invalid argument
    NV_CHECK_OR_RETURN(LEVEL_SILENT, !((pRef->pKernelMIGGpuInstance == NULL) &&
                       (pRef->pMIGComputeInstance != NULL)), NV_FALSE);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pRef->pKernelMIGGpuInstance != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pRef->pKernelMIGGpuInstance->bValid, NV_FALSE);

    // If we reached this point, the GPU instance is valid
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pRef->pMIGComputeInstance != NULL, NV_TRUE);
    NV_ASSERT_OR_RETURN(pRef->pMIGComputeInstance->bValid, NV_FALSE);

    return NV_TRUE;
}

/*! @brief check if the same instance(s) are passed in; only compare GI if lhs has no CI */
NvBool
kmigmgrAreMIGReferencesSame_IMPL
(
    MIG_INSTANCE_REF *pRefA,
    MIG_INSTANCE_REF *pRefB
)
{
    NV_CHECK_OR_RETURN(LEVEL_SILENT, kmigmgrIsMIGReferenceValid(pRefA) &&
                       kmigmgrIsMIGReferenceValid(pRefB), NV_FALSE);

    if ((pRefA->pKernelMIGGpuInstance != pRefB->pKernelMIGGpuInstance) ||
        ((pRefA->pMIGComputeInstance != NULL) &&
         (pRefA->pMIGComputeInstance != pRefB->pMIGComputeInstance)))
    {
      return NV_FALSE;
    }

    return NV_TRUE;
}

/*!
 * @brief Count set bits within range indicated by given base type in bitvector
 *
 * @param[in] pEngines     Bitvector to count
 * @param[in] rmEngineType 0th index RM_ENGINE_TYPE, only partitionable engines supported
 */
NvU32
kmigmgrCountEnginesOfType_IMPL
(
    const ENGTYPE_BIT_VECTOR *pEngines,
    RM_ENGINE_TYPE rmEngineType
)
{
    NV_RANGE range = rangeMake(rmEngineType, rmEngineType);

    if (pEngines == NULL)
        return 0;

    if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType))
        return 0;

    if (RM_ENGINE_TYPE_IS_GR(rmEngineType))
        range = RM_ENGINE_RANGE_GR();
    else if (RM_ENGINE_TYPE_IS_COPY(rmEngineType))
        range = RM_ENGINE_RANGE_COPY();
    else if (RM_ENGINE_TYPE_IS_NVDEC(rmEngineType))
        range = RM_ENGINE_RANGE_NVDEC();
    else if (RM_ENGINE_TYPE_IS_NVENC(rmEngineType))
        range = RM_ENGINE_RANGE_NVENC();
    else if (RM_ENGINE_TYPE_IS_NVJPEG(rmEngineType))
        range = RM_ENGINE_RANGE_NVJPEG();
    else if (RM_ENGINE_TYPE_IS_OFA(rmEngineType))
        range = RM_ENGINE_RANGE_OFA();

    return kmigmgrCountEnginesInRange(pEngines, range);
}

/*!
 * @brief Count set bits within range indicated.
 *
 * @param[in] pEngines     Bitvector to count
 * @param[in] range        Range to count for engines present in pEngines
 */
NvU32
kmigmgrCountEnginesInRange_IMPL
(
    const ENGTYPE_BIT_VECTOR *pEngines,
    NV_RANGE range
)
{
    ENGTYPE_BIT_VECTOR mask;

    if ((pEngines == NULL) || rangeIsEmpty(range))
        return 0;

    bitVectorClrAll(&mask);
    bitVectorSetRange(&mask, range);
    bitVectorAnd(&mask, &mask, pEngines);
    return bitVectorCountSetBits(&mask);
}

/*!
  * Utility fn to get the range of Async CE engines on this GPU.
  *
  * @return  NV_RANGE of Async CE Engines.
  *          Note: Not all engines in this range are valid, depending on what engines are available/floorswept.
  */
NV_RANGE
kmigmgrGetAsyncCERange_IMPL
(
    OBJGPU *pGpu
)
{
    NV_RANGE allCesRange = RM_ENGINE_RANGE_COPY();
    RM_ENGINE_TYPE firstAsyncCE = gpuGetRmEngineType(gpuGetFirstAsyncLce_HAL(pGpu));

    return rangeMake(firstAsyncCE, allCesRange.hi);
}

/*!
 * @brief Calculate the attribution ID for the given MIG instance reference.
 *
 * @note the attribution ID is an encoding of gpu/compute instance IDs dependent
 *       upon the maximum values of these IDs which must be queried by the
 *       recipient in order to decode. Attribution values for NULL or lone
 *       GPU instances will produce non-zero attribution IDs which will decode to
 *       out-of-range values for both IDs.
 *
 * @param[in] ref   Reference to a Gi/CI
 *
 * @return the encoded attribution ID
 */
NvU16
kmigmgrGetAttributionIdFromMIGReference_IMPL
(
    MIG_INSTANCE_REF ref
)
{
    NvU16 giID = KMIGMGR_MAX_GPU_SWIZZID;
    NvU16 ciID = KMIGMGR_MAX_COMPUTE_INSTANCES;

    //
    // Inverting this encoding depends upon the compute instance IDs having a
    // shorter range than the gpu instance IDs, otherwise high compute instance
    // IDs will cause aliasing
    //
    ct_assert(KMIGMGR_MAX_COMPUTE_INSTANCES < KMIGMGR_MAX_GPU_SWIZZID);

    // We are also depending on this encoding fitting in 16 bits...
    ct_assert((KMIGMGR_MAX_GPU_SWIZZID * KMIGMGR_MAX_COMPUTE_INSTANCES) <= NV_U16_MAX);

    if (kmigmgrIsMIGReferenceValid(&ref) &&
        (ref.pKernelMIGGpuInstance->swizzId < KMIGMGR_MAX_GPU_SWIZZID))
    {
        giID = (NvU16)ref.pKernelMIGGpuInstance->swizzId;
        if ((ref.pMIGComputeInstance != NULL) &&
            (ref.pMIGComputeInstance->id < KMIGMGR_MAX_COMPUTE_INSTANCES))
        {
            ciID = (NvU16)ref.pMIGComputeInstance->id;
        }
    }

    return (giID * KMIGMGR_MAX_GPU_SWIZZID) + ciID;
}

/*!
 * @brief   Function to convert an engine type from one bitvector to a
 *          corresponding engine type in another bitvector. The two bitvectors
 *          are expected to have the same set bit count.
 */
NV_STATUS
kmigmgrEngineTypeXlate_IMPL
(
    ENGTYPE_BIT_VECTOR *pSrc,
    RM_ENGINE_TYPE srcEngineType,
    ENGTYPE_BIT_VECTOR *pDst,
    RM_ENGINE_TYPE *pDstEngineType
)
{
    RM_ENGINE_TYPE tempSrcEngineType;
    RM_ENGINE_TYPE tempDstEngineType;
    NvBool bFound;

    NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDst != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstEngineType != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!bitVectorTest(pSrc, srcEngineType))
        return NV_ERR_OBJECT_NOT_FOUND;

    // Iterate over both masks at the same time
    bFound = NV_FALSE;
    FOR_EACH_IN_BITVECTOR_PAIR(pSrc, tempSrcEngineType, pDst, tempDstEngineType)
    {
        bFound = (srcEngineType == tempSrcEngineType);
        if (bFound)
            break;
    }
    FOR_EACH_IN_BITVECTOR_PAIR_END();

    // We already checked that the engine is present above, this should never fire
    NV_ASSERT(bFound);

    *pDstEngineType = tempDstEngineType;

    return NV_OK;
}

/*!
 * @brief   Function to convert a bitVector from Source domain to Destination domain
 *
 * @param[in]     pSrcRef    Source bitVector domain for reference
 * @param[in]     pSrc       Source bitVector to be converted
 * @param[in]     pDstRef    Destination bitVector domain for reference
 * @param[out]    pDst       bitVector after conversion
 */
NV_STATUS
kmigmgrEngBitVectorXlate_IMPL
(
    ENGTYPE_BIT_VECTOR *pSrcRef,
    ENGTYPE_BIT_VECTOR *pSrc,
    ENGTYPE_BIT_VECTOR *pDstRef,
    ENGTYPE_BIT_VECTOR *pDst
)
{
    RM_ENGINE_TYPE srcRmEngineType;
    RM_ENGINE_TYPE dstRmEngineType;

    NV_ASSERT_OR_RETURN(pSrcRef != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstRef != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDst != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorClrAll(pDst);
    FOR_EACH_IN_BITVECTOR(pSrc, srcRmEngineType)
    {
        NV_ASSERT_OK_OR_RETURN(
            kmigmgrEngineTypeXlate(pSrcRef, srcRmEngineType,
                                   pDstRef, &dstRmEngineType));

        bitVectorSet(pDst, dstRmEngineType);
    }
    FOR_EACH_IN_BITVECTOR_END();

    return NV_OK;
}

//
// below algorithm depends on contiguity of all partitionable engine values
// in RM_ENGINE_TYPE, so add asserts here.
// Note - this only checks the first and last ID, a proper check would account
// for all entries, but that's not possible at this time.
//
ct_assert((RM_ENGINE_TYPE_GR(RM_ENGINE_TYPE_GR_SIZE - 1) -
           RM_ENGINE_TYPE_GR(0)) == (RM_ENGINE_TYPE_GR_SIZE - 1));
ct_assert((RM_ENGINE_TYPE_COPY(RM_ENGINE_TYPE_COPY_SIZE - 1) -
           RM_ENGINE_TYPE_COPY(0)) == (RM_ENGINE_TYPE_COPY_SIZE - 1));
ct_assert((RM_ENGINE_TYPE_NVDEC(RM_ENGINE_TYPE_NVDEC_SIZE - 1) -
           RM_ENGINE_TYPE_NVDEC(0)) == (RM_ENGINE_TYPE_NVDEC_SIZE - 1));
ct_assert((RM_ENGINE_TYPE_NVENC(RM_ENGINE_TYPE_NVENC_SIZE - 1) -
           RM_ENGINE_TYPE_NVENC(0)) == (RM_ENGINE_TYPE_NVENC_SIZE - 1));

/*!
 * @brief   Chooses the engines of the given type to allocate. Supports
 *          shared/exclusive ownership arbitration.
 *
 * @param[IN]   pSourceEngines       Mask of engines in an instance
 * @param[IN}   bShared              NV_TRUE if engines should be shared
 * @param[IN]   engTypeRange         NV_RANGE of bit indices for this eng type
 * @param[IN]   regEngCount          Requested number of engines in this CI
 * @param[I/O]  pOutEngines          Mask of engines already/newly allocated
 * @param[I/O]  pExclusiveEngines    Mask of already exclusively-allocated engines
 * @param[I/O]  pSharedEngines       Mask of engines shared by other instances
 * @param[IN]   pAllocatableEngines  Mask of engines that are allocatable
 */
NV_STATUS
kmigmgrAllocateInstanceEngines_IMPL
(
    ENGTYPE_BIT_VECTOR *pSourceEngines,
    NvBool bShared,
    NV_RANGE engTypeRange,
    NvU32 reqEngCount,
    ENGTYPE_BIT_VECTOR *pOutEngines,
    ENGTYPE_BIT_VECTOR *pExclusiveEngines,
    ENGTYPE_BIT_VECTOR *pSharedEngines,
    ENGTYPE_BIT_VECTOR *pAllocatableEngines
)
{
    NvU32 allocated = 0;
    ENGTYPE_BIT_VECTOR engines;
    RM_ENGINE_TYPE rmEngineType;
    NvU32 localIdx;

    // Ensure allocatableEngines is subset of sourceEngines
    bitVectorClrAll(&engines);
    bitVectorAnd(&engines, pAllocatableEngines, pSourceEngines);
    NV_ASSERT_OR_RETURN(bitVectorTestEqual(&engines, pAllocatableEngines), NV_ERR_INVALID_STATE);

    // If using shared engines, allocate as many from existing shared engines as possible
    if (bShared)
    {
        bitVectorClrAll(&engines);
        bitVectorSetRange(&engines, engTypeRange);
        bitVectorAnd(&engines, &engines, pSourceEngines);
        localIdx = 0;
        FOR_EACH_IN_BITVECTOR(&engines, rmEngineType)
        {
            if (allocated == reqEngCount)
                break;

            // Skip engines that aren't allocatable or aren't in the shared pool already
            if (!bitVectorTest(pAllocatableEngines, rmEngineType) ||
                !bitVectorTest(pSharedEngines, rmEngineType))
            {
                localIdx++;
                continue;
            }

            // assign the engine
            bitVectorSet(pOutEngines, engTypeRange.lo + localIdx);

            localIdx++;
            allocated++;
        }
        FOR_EACH_IN_BITVECTOR_END();
    }

    // Allocate the rest from the free pool
    bitVectorClrAll(&engines);
    bitVectorSetRange(&engines, engTypeRange);
    bitVectorAnd(&engines, &engines, pSourceEngines);
    localIdx = 0;
    FOR_EACH_IN_BITVECTOR(&engines, rmEngineType)
    {
        if (allocated == reqEngCount)
            break;

        // Skip non-allocatable or in-use engines
        if (!bitVectorTest(pAllocatableEngines, rmEngineType) ||
            bitVectorTest(pSharedEngines, rmEngineType) ||
            bitVectorTest(pExclusiveEngines, rmEngineType))
        {
            localIdx++;
            continue;
        }

        // Add the engine to the appropriate in-use pool
        bitVectorSet((bShared ? pSharedEngines : pExclusiveEngines), rmEngineType);

        // Assign the engine
        bitVectorSet(pOutEngines, engTypeRange.lo + localIdx);

        localIdx++;
        allocated++;
    }
    FOR_EACH_IN_BITVECTOR_END();

    NV_CHECK_OR_RETURN(LEVEL_SILENT, allocated == reqEngCount, NV_ERR_INSUFFICIENT_RESOURCES);
    return NV_OK;
}

/*!
 * @brief Convert global/physical engine mask to logical/local (no-hole) mask
 *
 * @param[in] pPhysicalEngineMask   Bitvector storing physical mask
 * @param[in] pLocalEngineMask      Bitvector storing local mask
 */
void
kmigmgrGetLocalEngineMask_IMPL
(
    OBJGPU *pGpu,
    ENGTYPE_BIT_VECTOR *pPhysicalEngineMask,
    ENGTYPE_BIT_VECTOR *pLocalEngineMask
)
{
    NV_RANGE range;
    NvU32 count;
    bitVectorClrAll(pLocalEngineMask);

    count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_GR(0));
    if (count > 0)
    {
        range = rangeMake(RM_ENGINE_TYPE_GR(0), RM_ENGINE_TYPE_GR(count - 1));
        bitVectorSetRange(pLocalEngineMask, range);
    }
    {
        count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_COPY(0));
        if (count > 0)
        {
            range = rangeMake(RM_ENGINE_TYPE_COPY(0), RM_ENGINE_TYPE_COPY(count - 1));
            bitVectorSetRange(pLocalEngineMask, range);
        }
    }
    count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_NVDEC(0));
    if (count > 0)
    {
        range = rangeMake(RM_ENGINE_TYPE_NVDEC(0), RM_ENGINE_TYPE_NVDEC(count - 1));
        bitVectorSetRange(pLocalEngineMask, range);
    }

    count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_NVENC(0));
    if (count > 0)
    {
        range = rangeMake(RM_ENGINE_TYPE_NVENC(0), RM_ENGINE_TYPE_NVENC(count - 1));
        bitVectorSetRange(pLocalEngineMask, range);
    }

    count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_NVJPEG(0));
    if (count > 0)
    {
        range = rangeMake(RM_ENGINE_TYPE_NVJPEG(0), RM_ENGINE_TYPE_NVJPEG(count - 1));
        bitVectorSetRange(pLocalEngineMask, range);
    }

    count = kmigmgrCountEnginesOfType(pPhysicalEngineMask, RM_ENGINE_TYPE_OFA(0));
    if (count > 0)
    {
        range = rangeMake(RM_ENGINE_TYPE_OFA(0), RM_ENGINE_TYPE_OFA(count - 1));
        bitVectorSetRange(pLocalEngineMask, range);
    }
}

NV_STATUS
_kmigmgrAllocThirdPartyP2PObject
(
    OBJGPU  *pGpu,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV503C_ALLOC_PARAMETERS params;
    NvHandle hThirdPartyP2P = NV01_NULL_OBJECT;

    portMemSet(&params, 0, sizeof(params));
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        params.flags = NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_NVLINK;
    }
    else
    {
        params.flags = NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_BAR1;
    }

    status = pRmApi->Alloc(pRmApi,
                           pKernelMIGGpuInstance->instanceHandles.hClient,
                           pKernelMIGGpuInstance->instanceHandles.hSubdevice,
                           &hThirdPartyP2P,
                           NV50_THIRD_PARTY_P2P,
                           &params,
                           sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "Error creating internal ThirdPartyP2P object: 0x%x\n",
                  status);
        hThirdPartyP2P = NV01_NULL_OBJECT;
    }

    pKernelMIGGpuInstance->instanceHandles.hThirdPartyP2P = hThirdPartyP2P;

    return NV_OK;
}

/*!
 * @brief   Create client and subdevice handles to make calls into this gpu instance
 */
NV_STATUS
kmigmgrAllocGPUInstanceHandles_IMPL
(
    OBJGPU *pGpu,
    NvU32 swizzId,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle hSubscription = NV01_NULL_OBJECT;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NVC637_ALLOCATION_PARAMETERS params;
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClient, &hDevice, &hSubdevice));

    pKernelMIGGpuInstance->instanceHandles.hClient = hClient;
    pKernelMIGGpuInstance->instanceHandles.hDevice = hDevice;
    pKernelMIGGpuInstance->instanceHandles.hSubdevice = hSubdevice;

    portMemSet(&params, 0, sizeof(params));
    params.swizzId = swizzId;
    NV_ASSERT_OK_OR_GOTO(status,
                         pRmApi->Alloc(pRmApi, hClient, hSubdevice, &hSubscription,
                                       AMPERE_SMC_PARTITION_REF, &params, sizeof(params)),
                         failed);

    pKernelMIGGpuInstance->instanceHandles.hSubscription = hSubscription;

    {
        NV_ASSERT_OK_OR_GOTO(status,
                             _kmigmgrAllocThirdPartyP2PObject(pGpu, pKernelMIGGpuInstance),
                             failed);
    }

    return NV_OK;

failed:
    kmigmgrFreeGPUInstanceHandles(pKernelMIGGpuInstance);

    return status;
}

/*!
 * @brief   Delete created gpu instance handles if they exist
 */
void
kmigmgrFreeGPUInstanceHandles_IMPL
(
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    if (pKernelMIGGpuInstance->instanceHandles.hClient != NV01_NULL_OBJECT)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        pRmApi->Free(pRmApi, pKernelMIGGpuInstance->instanceHandles.hClient, pKernelMIGGpuInstance->instanceHandles.hClient);
        pKernelMIGGpuInstance->instanceHandles.hClient = NV01_NULL_OBJECT;
        pKernelMIGGpuInstance->instanceHandles.hDevice = NV01_NULL_OBJECT;
        pKernelMIGGpuInstance->instanceHandles.hSubdevice = NV01_NULL_OBJECT;
        pKernelMIGGpuInstance->instanceHandles.hSubscription = NV01_NULL_OBJECT;
        pKernelMIGGpuInstance->instanceHandles.hThirdPartyP2P = NV01_NULL_OBJECT;
    }
}

/*!
 * @brief   Checks if all references to gpu instance are internal
 */
NvBool
kmigmgrIsGPUInstanceReadyToBeDestroyed_IMPL
(
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NvS32 targetRefCount;
    NvS32 actualRefCount;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelMIGGpuInstance->pShare != NULL, NV_TRUE);

    //
    // Initial refCount is increased to "1" when gpu instance is created and then
    // every subscription by a client should increase the refcount
    //
    targetRefCount = 1;

    // A client handle is allocated to support internal GR Routing
    if (pKernelMIGGpuInstance->instanceHandles.hClient != NV01_NULL_OBJECT)
        targetRefCount++;

    //
    // GPU instance scrubber is initialized during gpu instance creation and deleted
    // when gpu instance is invalidated, and subscribes to the gpu instance, so must
    // be accounted for in the target ref count
    //
    if (pKernelMIGGpuInstance->bMemoryPartitionScrubberInitialized)
        targetRefCount++;

    actualRefCount = serverGetShareRefCount(&g_resServ, pKernelMIGGpuInstance->pShare);
    if (actualRefCount > targetRefCount)
        return NV_FALSE;

    // Mismatch here indicates programming error
    NV_ASSERT(actualRefCount == targetRefCount);
    return NV_TRUE;
}

NV_STATUS
kmigmgrConstructEngine_IMPL
(
    OBJGPU           *pGpu,
    KernelMIGManager *pKernelMIGManager,
    ENGDESCRIPTOR    engDesc
)
{
    NvU32 GIIdx;
    KERNEL_MIG_MANAGER_PRIVATE_DATA *pPrivate;

    pKernelMIGManager->bMIGEnabled = NV_FALSE;
    pKernelMIGManager->swizzIdInUseMask = 0x0;

    pPrivate = portMemAllocNonPaged(sizeof(*pPrivate));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pPrivate != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pPrivate, 0, sizeof(*pPrivate));
    pKernelMIGManager->pPrivate = pPrivate;

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pKernelMIGManager->kernelMIGGpuInstance); ++GIIdx)
    {
        kmigmgrInitGPUInstanceInfo(pGpu, pKernelMIGManager,
                                   &pKernelMIGManager->kernelMIGGpuInstance[GIIdx]);
    }

    kmigmgrInitRegistryOverrides(pGpu, pKernelMIGManager);

    return NV_OK;
}

void
kmigmgrDestruct_IMPL
(
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 GIIdx;
    NvU32 CIIdx;

    portMemFree(pKernelMIGManager->pPrivate->staticInfo.pProfiles);
    pKernelMIGManager->pPrivate->staticInfo.pProfiles = NULL;
    portMemFree(pKernelMIGManager->pPrivate->staticInfo.pSwizzIdFbMemPageRanges);
    pKernelMIGManager->pPrivate->staticInfo.pSwizzIdFbMemPageRanges = NULL;
    portMemFree(pKernelMIGManager->pPrivate->staticInfo.pCIProfiles);
    pKernelMIGManager->pPrivate->staticInfo.pCIProfiles = NULL;
    portMemFree(pKernelMIGManager->pPrivate->staticInfo.pSkylineInfo);
    pKernelMIGManager->pPrivate->staticInfo.pSkylineInfo = NULL;

    portMemFree(pKernelMIGManager->pPrivate);
    pKernelMIGManager->pPrivate = NULL;

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pKernelMIGManager->kernelMIGGpuInstance); ++GIIdx)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = &pKernelMIGManager->kernelMIGGpuInstance[GIIdx];

        // Shouldn't have any valid GPU instance
        if (pKernelMIGGpuInstance->bValid)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Deleting valid GPU instance with swizzId - %d. Should have been deleted before shutdown!\n",
                      pKernelMIGGpuInstance->swizzId);
        }

        for (CIIdx = 0;
             CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
             ++CIIdx)
        {
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];

            // Shouldn't have any valid compute instance
            if (pMIGComputeInstance->bValid)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Deleting valid compute instance - %d. Should have been deleted before shutdown!\n",
                          CIIdx);
            }
        }
    }
}

/*!
 * @brief   Handle KMIGMGR init which must occur after GPU post load.
 *
 * @param[in] pGpu
 * @param[in] pUnusedData Unused callback data
 */
static NV_STATUS
_kmigmgrHandlePostSchedulingEnableCallback
(
    OBJGPU *pGpu,
    void   *pUnusedData
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    if (!IS_VIRTUAL(pGpu))
    {
        NvBool bTopLevelScrubberEnabled = NV_FALSE;
        NvBool bTopLevelScrubberConstructed = NV_FALSE;

        memmgrGetTopLevelScrubberStatus(pGpu, pMemoryManager,
            &bTopLevelScrubberEnabled, &bTopLevelScrubberConstructed);

        //
        // This callback is handled as part of the same routine that triggers
        // scrubber initialization. Unfortunately this callback depends on the
        // scrubber being initialized first, and we cannot enforce that the scrubber
        // callback always goes first. However, the trigger routine does support a
        // retry mechanism that will allow us to get called back after all of the
        // other callbacks in the list are completed. We signal for retry by
        // returning NV_WARN_MORE_PROCESSING_REQUIRED if the scrubber is enabled but
        // hasn't been intialized yet. The warning will be quashed on the first
        // attempt, but will then be reported and trigger initialization failure if
        // it happens again on the retry.
        //
        // Bug: 2997744, skipping the check here because top level scrubber creation is dealyed until
        // GPU instances are created in MIG enabled guest
        //
        NV_CHECK_OR_RETURN(LEVEL_SILENT,
                           !bTopLevelScrubberEnabled || bTopLevelScrubberConstructed,
                           NV_WARN_MORE_PROCESSING_REQUIRED);
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memmgrSetPartitionableMem_HAL(pGpu, pMemoryManager));

    if (IS_GSP_CLIENT(pGpu) && !IS_VIRTUAL(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS params;

        portMemSet(&params, 0x0, sizeof(params));
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_GET_SMC_MODE,
                            &params,
                            sizeof(params)));

        if (params.smcMode == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_UNSUPPORTED)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED, NV_FALSE);
        }
    }

    if ((pKernelMIGManager == NULL) || !kmigmgrIsMIGSupported(pGpu, pKernelMIGManager))
    {
        NV_PRINTF(LEVEL_INFO, "MIG not supported on this GPU.\n");
        return NV_OK;
    }

    if (!IS_MIG_ENABLED(pGpu) && !IS_VIRTUAL(pGpu) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED) &&
        (gpumgrIsSystemMIGEnabled(gpuGetDBDF(pGpu)) || pKernelMIGManager->bMIGAutoOnlineEnabled || pKernelMIGManager->bBootConfigSupported))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS params;

        portMemSet(&params, 0x0, sizeof(params));
        params.partitioningMode = NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_FAST_RECONFIG;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_PARTITIONING_MODE,
                            &params,
                            sizeof(params)));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrSetPartitioningMode(pGpu, pKernelMIGManager));
    }

    gpumgrCacheSetMIGEnabled(pGpu, pKernelMIGManager->bMIGEnabled);

    // Populate static info collection even if MIG is not enabled.
    if ((!pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) && IS_SILICON(pGpu)) ||
        (IS_VIRTUAL(pGpu) && IS_MIG_ENABLED(pGpu)))
    {
        // Initialize static info derived from physical RM
        NV_ASSERT_OK_OR_RETURN(kmigmgrLoadStaticInfo_HAL(pGpu, pKernelMIGManager));

        //
        // Populate static GPU instance memory config which will be used to manage
        // GPU instance memory
        //
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        NV_ASSERT_OK_OR_RETURN(kmemsysPopulateMIGGPUInstanceMemConfig_HAL(pGpu, pKernelMemorySystem));

        // KERNEL_ONLY variants require static info to detect reduced configs
        kmigmgrDetectReducedConfig_HAL(pGpu, pKernelMIGManager);
    }

    NV_ASSERT_OK_OR_RETURN(kmigmgrRestoreFromPersistence_HAL(pGpu, pKernelMIGManager));

    return NV_OK;
}

static NV_STATUS _kmigmgrHandlePreSchedulingDisableCallback
(
    OBJGPU *pGpu,
    void *pUnusedData
)
{
    NvU32 GIIdx;
    NvU32 CIIdx;
    NV_STATUS rmStatus = NV_OK;
    NvBool bDisable = NV_FALSE;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pKernelMIGManager->kernelMIGGpuInstance); ++GIIdx)
    {
        if (pKernelMIGManager->kernelMIGGpuInstance[GIIdx].bValid)
        {
            kmigmgrDestroyGPUInstanceScrubber(pGpu, pKernelMIGManager, &pKernelMIGManager->kernelMIGGpuInstance[GIIdx]);
        }
    }

    if (IS_VIRTUAL(pGpu) && kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
        return NV_OK;

    //
    // Update persistent instance topology so that we can recreate it on next
    // GPU attach.
    //
    NV_ASSERT_OK(kmigmgrSaveToPersistence(pGpu, pKernelMIGManager));

    if (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
        return NV_OK;

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pKernelMIGManager->kernelMIGGpuInstance); ++GIIdx)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = &pKernelMIGManager->kernelMIGGpuInstance[GIIdx];
        NvU32 swizzId;

        // Skip invalid gpu instances
        if (!pKernelMIGGpuInstance->bValid)
            continue;

        swizzId = pKernelMIGGpuInstance->swizzId;

        // Shouldn't be any valid gpu instances
        NV_PRINTF(LEVEL_ERROR,
                  "Invalidating valid gpu instance with swizzId = %d\n",
                  swizzId);

        for (CIIdx = 0;
             CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
             ++CIIdx)
        {
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance =
                &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];

            // Skip invalid compute instances
            if (!pMIGComputeInstance->bValid)
                continue;

            // Shouldn't be any valid compute instances
            NV_PRINTF(LEVEL_ERROR,
                      "Invalidating valid compute instance with id = %d\n",
                      CIIdx);

            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, CIIdx, NV_TRUE));

            if (IS_GSP_CLIENT(pGpu))
            {
                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
                NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS params;

                portMemSet(&params, 0, sizeof(params));
                params.execPartCount = 1;
                params.execPartId[0] = CIIdx;

                NV_ASSERT_OK(
                    pRmApi->Control(pRmApi,
                                    pKernelMIGGpuInstance->instanceHandles.hClient,
                                    pKernelMIGGpuInstance->instanceHandles.hSubscription,
                                    NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE,
                                    &params,
                                    sizeof(params)));
            }
        }

        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kmigmgrInvalidateGPUInstance(pGpu, pKernelMIGManager, swizzId, NV_TRUE));

        if (IS_GSP_CLIENT(pGpu))
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
            NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS params;

            portMemSet(&params, 0, sizeof(params));
            params.partitionCount = 1;
            params.partitionInfo[0].bValid = NV_FALSE;
            params.partitionInfo[0].swizzId = swizzId;

            NV_ASSERT_OK(
                pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES,
                                &params,
                                sizeof(params)));
        }

        // There was an active gpu instance, we need to disable MIG later
        bDisable = NV_TRUE;
    }

    // Disable MIG
    if (pKernelMIGManager->swizzIdInUseMask != 0x0)
    {
        NV_ASSERT(0);
        NV_PRINTF(LEVEL_ERROR, "leaked swizzid mask 0x%llx !!\n", pKernelMIGManager->swizzIdInUseMask);
    }

    if (bDisable)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kmigmgrSetMIGState(pGpu, pKernelMIGManager, NV_TRUE, NV_FALSE, NV_TRUE));
    }

    return NV_OK;
}

NV_STATUS
kmigmgrStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    //
    // Configure MIG Mode based on devinit's determination of MIG enable
    // preconditions being met or not. Devinit will set SW_SCRATCH bit if MIG
    // mode was requested and was able to be supported / enabled.
    //
    if (kmigmgrIsDevinitMIGBitSet_HAL(pGpu, pKernelMIGManager))
        pKernelMIGManager->bMIGEnabled = NV_TRUE;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, kmigmgrIsMIGSupported(pGpu, pKernelMIGManager), NV_OK);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED))
    {
        NvU32 data32;
        if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_SET_MIG_AUTO_ONLINE_MODE, &data32))
        {
            if (NV_REG_STR_RM_SET_MIG_AUTO_ONLINE_MODE_ENABLED == data32)
            {
                pKernelMIGManager->bMIGAutoOnlineEnabled = NV_TRUE;
            }
        }
    }

    // Setup a callback to initialize state at the very end of GPU post load
    NV_ASSERT_OK(
        kfifoAddSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            _kmigmgrHandlePostSchedulingEnableCallback, NULL,
            _kmigmgrHandlePreSchedulingDisableCallback, NULL));

    return NV_OK;
}

/*! State unload */
NV_STATUS
kmigmgrStateUnload_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 flags
)
{
    kmigmgrClearStaticInfo_HAL(pGpu, pKernelMIGManager);

    // Nothing to do if MIG is not supported
    NV_CHECK_OR_RETURN(LEVEL_SILENT, kmigmgrIsMIGSupported(pGpu, pKernelMIGManager), NV_OK);

    kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
        _kmigmgrHandlePostSchedulingEnableCallback, NULL,
        _kmigmgrHandlePreSchedulingDisableCallback, NULL);

    return NV_OK;
}

/*! Init registry overrides */
void
kmigmgrInitRegistryOverrides_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 data32;

    //
    // Try reading boot config feature flags regkey from NvGlobal regkeys first.
    // If the NvGlobal regkey is not found, try reading from per-GPU regkeys.
    //
    if (osGetNvGlobalRegistryDword(pGpu, NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, &data32) == NV_OK)
    {
        pKernelMIGManager->bGlobalBootConfigUsed = NV_TRUE;
    }
    else if (osReadRegistryDword(pGpu, NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, &data32) == NV_OK)
    {
        // Do nothing
    }
    else
    {
        data32 = DRF_DEF(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, _SUPPORTED, _DEFAULT) |
                 DRF_DEF(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, _AUTO_UPDATE, _DEFAULT);
    }

    pKernelMIGManager->bBootConfigSupported = FLD_TEST_DRF(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, _SUPPORTED, _TRUE, data32);
    pKernelMIGManager->bAutoUpdateBootConfig = FLD_TEST_DRF(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_FEATURE_FLAGS, _SUPPORTED, _TRUE, data32);

}

/**
 * @brief Retrieve data block for GPU instance at given slot
 */
KERNEL_MIG_GPU_INSTANCE *
kmigmgrGetMIGGpuInstanceSlot_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 i
)
{
    NV_ASSERT_OR_RETURN(i < NV_ARRAY_ELEMENTS(pKernelMIGManager->kernelMIGGpuInstance), NULL);
    return &pKernelMIGManager->kernelMIGGpuInstance[i];
}

/**
 * @brief Returns true if MIG is supported.
 * Also MIG is not supported on platforms that support ATS over NVLink.
 */
NvBool
kmigmgrIsMIGSupported_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    return pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED);
}

/*!
 * @brief Determines if MIG is enabled in supported system or not
 */
NvBool
kmigmgrIsMIGEnabled_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    return kmigmgrIsMIGSupported(pGpu, pKernelMIGManager) && pKernelMIGManager->bMIGEnabled;
}

/*!
 * @brief Determines if MIG GPU instancing is enabled
 */
NvBool
kmigmgrIsMIGGpuInstancingEnabled_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    return (IS_MIG_ENABLED(pGpu) &&
            (pKernelMIGManager->swizzIdInUseMask != 0));
}

/*!
 * @brief Determines if MIG memory partitioning is enabled
 */
NvBool
kmigmgrIsMIGMemPartitioningEnabled_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 swizzId;

    if (!IS_MIG_IN_USE(pGpu))
    {
        return NV_FALSE;
    }

    FOR_EACH_INDEX_IN_MASK(64, swizzId, pKernelMIGManager->swizzIdInUseMask)
    {
        if (kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId))
        {
            return NV_TRUE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_FALSE;
}

/*!
 * @brief Determines if NvLink and P2P are compatible with MIG
 */
NvBool
kmigmgrIsMIGNvlinkP2PSupported_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    //
    // No need to make decision based on any override if MIG is not supported/enabled
    // on a specific chip
    //
    if (!IS_MIG_ENABLED(pGpu))
    {
        return NV_TRUE;
    }

    // MIG+NVLINK not supported by default
    return NV_FALSE;
}

/*! Retrieve immutable static data */
const KERNEL_MIG_MANAGER_STATIC_INFO *
kmigmgrGetStaticInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    KERNEL_MIG_MANAGER_PRIVATE_DATA *pPrivate = (KERNEL_MIG_MANAGER_PRIVATE_DATA *)pKernelMIGManager->pPrivate;
    return ((pPrivate != NULL) && pPrivate->bInitialized) ? &pPrivate->staticInfo : NULL;
}

/*! Initialize static information sourced from VGPU static info */
NV_STATUS
kmigmgrLoadStaticInfo_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV_STATUS status = NV_OK;
    KERNEL_MIG_MANAGER_PRIVATE_DATA *pPrivate = (KERNEL_MIG_MANAGER_PRIVATE_DATA *)pKernelMIGManager->pPrivate;
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pPrivate != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (pPrivate->bInitialized)
        return NV_OK;

    pPrivate->staticInfo.pProfiles = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pProfiles));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pProfiles != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pProfiles, 0x0, sizeof(*pPrivate->staticInfo.pProfiles));
    // In VGPU only one profile is visible describing all resources available
    {
        NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO *pPartitionDesc = &pPrivate->staticInfo.pProfiles->table[0];

        pPartitionDesc->partitionFlag   = (DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _FULL) |
                                           DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, _FULL));

        pPartitionDesc->grCount         = pVSI->gpuPartitionInfo.grEngCount;
        pPartitionDesc->gpcCount        = pVSI->gpuPartitionInfo.gpcCount;
        pPartitionDesc->virtualGpcCount = pVSI->gpuPartitionInfo.virtualGpcCount;
        pPartitionDesc->gfxGpcCount     = pVSI->gpuPartitionInfo.gfxGpcCount;
        pPartitionDesc->veidCount       = pVSI->gpuPartitionInfo.veidCount;
        pPartitionDesc->smCount         = pVSI->gpuPartitionInfo.smCount;
        pPartitionDesc->ceCount         = pVSI->gpuPartitionInfo.ceCount;
        pPartitionDesc->nvEncCount      = pVSI->gpuPartitionInfo.nvEncCount;
        pPartitionDesc->nvDecCount      = pVSI->gpuPartitionInfo.nvDecCount;
        pPartitionDesc->nvJpgCount      = pVSI->gpuPartitionInfo.nvJpgCount;
        pPartitionDesc->nvOfaCount      = pVSI->gpuPartitionInfo.nvOfaCount;
        pPartitionDesc->validCTSIdMask  = pVSI->gpuPartitionInfo.validCTSIdMask;
        pPrivate->staticInfo.pProfiles->count = 1;
    }

    bitVectorClrAll(&pPrivate->staticInfo.partitionableEngines);

    // Use the engine info list to populate the partitionable engines in guest
    {
        KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        const NvU32 numEngines = kfifoGetNumEschedDrivenEngines(pKernelFifo);
        NvU32 engine;

        NV_ASSERT_OK(gpuUpdateEngineTable(pGpu));
        for (engine = 0; engine < numEngines; ++engine)
        {
            RM_ENGINE_TYPE rmEngineType;

            NV_ASSERT_OK_OR_RETURN(
                kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                         ENGINE_INFO_TYPE_INVALID, engine,
                                         ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType));

            // Skip invalid engine type values
            if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType))
            {
                NV_ASSERT(0);
                continue;
            }

            // Skip engines which are not partitionable
            if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType))
                continue;

            bitVectorSet(&pPrivate->staticInfo.partitionableEngines, rmEngineType);
        }
    }

    pPrivate->staticInfo.pCIProfiles = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pCIProfiles));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pCIProfiles != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pCIProfiles, 0x0, sizeof(*pPrivate->staticInfo.pCIProfiles));
    {
        NvU32 entryCount = 0;
        NvU32 i;

        NV_ASSERT(pVSI->ciProfiles.profileCount <= NV_ARRAY_ELEMENTS(pPrivate->staticInfo.pCIProfiles->profiles));

        for (i = 0; i < pVSI->ciProfiles.profileCount; i++)
        {
            NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pCIProfile;

            // Filter any profiles which would not have fit on the VGPU's instance anyway
            if (pVSI->ciProfiles.profiles[i].smCount > pVSI->gpuPartitionInfo.smCount)
                continue;

            pCIProfile = &pPrivate->staticInfo.pCIProfiles->profiles[entryCount];
            pCIProfile->gfxGpcCount   = pVSI->ciProfiles.profiles[i].gfxGpcCount;
            pCIProfile->computeSize   = pVSI->ciProfiles.profiles[i].computeSize;
            pCIProfile->gpcCount      = pVSI->ciProfiles.profiles[i].gpcCount;
            pCIProfile->physicalSlots = pVSI->ciProfiles.profiles[i].gpcCount;
            pCIProfile->veidCount     = pVSI->ciProfiles.profiles[i].veidCount;
            pCIProfile->smCount       = pVSI->ciProfiles.profiles[i].smCount;

            entryCount++;
        }
        pPrivate->staticInfo.pCIProfiles->profileCount = entryCount;
    }

    // Not used by VGPU
    pPrivate->staticInfo.pSwizzIdFbMemPageRanges = NULL;

    // Support to be added for skylines on vgpu as part of bug 3424046
    pPrivate->staticInfo.pSkylineInfo = NULL;

    // Publish static data
    pPrivate->bInitialized = NV_TRUE;

    // Load fake static info for VGPU
    kmigmgrSetStaticInfo_HAL(pGpu, pKernelMIGManager);

    return NV_OK;

failed:
    portMemFree(pPrivate->staticInfo.pProfiles);
    pPrivate->staticInfo.pProfiles = NULL;
    portMemFree(pPrivate->staticInfo.pSwizzIdFbMemPageRanges);
    pPrivate->staticInfo.pSwizzIdFbMemPageRanges = NULL;
    portMemFree(pPrivate->staticInfo.pCIProfiles);
    pPrivate->staticInfo.pCIProfiles = NULL;

    return status;
}

/*! Initialize static information queried from Physical RM */
NV_STATUS
kmigmgrLoadStaticInfo_KERNEL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    KERNEL_MIG_MANAGER_PRIVATE_DATA *pPrivate = (KERNEL_MIG_MANAGER_PRIVATE_DATA *)pKernelMIGManager->pPrivate;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;
    NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS params = {0};
    ENGTYPE_BIT_VECTOR partitionableNv2080Engines;
    NvU32 nv2080EngineType;

    NV_ASSERT_OR_RETURN(pPrivate != NULL, NV_ERR_INVALID_STATE);

    if (pPrivate->bInitialized)
        return NV_OK;

    //
    // HACK
    // Some of the static data implementations depend on other static data. We
    // must publish early to make the data accessible as it becomes available.
    //
    pPrivate->bInitialized = NV_TRUE;

    bitVectorClrAll(&pPrivate->staticInfo.partitionableEngines);

    if (IS_GSP_CLIENT(pGpu))
    {
        NV_CHECK(LEVEL_ERROR, kmigmgrEnableAllLCEs(pGpu, pKernelMIGManager, NV_TRUE) == NV_OK);
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES,
                        &params,
                        sizeof(params)),
        failed);

    //
    // Copy over the engineMask and save it in the staticInfo for later use.
    // In staticInfo, we use RMEngineTypes, so convert the nv2080 types before saving.
    //
    bitVectorFromRaw(&partitionableNv2080Engines,
                     params.engineMask,
                     sizeof(params.engineMask));
    FOR_EACH_IN_BITVECTOR(&partitionableNv2080Engines, nv2080EngineType)
    {
        bitVectorSet(&pPrivate->staticInfo.partitionableEngines,
            gpuGetRmEngineType(nv2080EngineType));
    }
    FOR_EACH_IN_BITVECTOR_END();


    pPrivate->staticInfo.pSkylineInfo = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSkylineInfo));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pSkylineInfo != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pSkylineInfo, 0x0, sizeof(*pPrivate->staticInfo.pSkylineInfo));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO,
                        pPrivate->staticInfo.pSkylineInfo,
                        sizeof(*pPrivate->staticInfo.pSkylineInfo)),
        failed);

    pPrivate->staticInfo.pCIProfiles = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pCIProfiles));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pCIProfiles != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pCIProfiles, 0x0, sizeof(*pPrivate->staticInfo.pCIProfiles));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES,
                        pPrivate->staticInfo.pCIProfiles,
                        sizeof(*pPrivate->staticInfo.pCIProfiles)),
        failed);

    pPrivate->staticInfo.pProfiles = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pProfiles));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pProfiles != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pProfiles, 0x0, sizeof(*pPrivate->staticInfo.pProfiles));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PROFILES,
                        pPrivate->staticInfo.pProfiles,
                        sizeof(*pPrivate->staticInfo.pProfiles)),
        failed);

    if (IS_GSP_CLIENT(pGpu))
    {
        NV_CHECK(LEVEL_ERROR, kmigmgrEnableAllLCEs(pGpu, pKernelMIGManager, NV_FALSE) == NV_OK);
    }

    pPrivate->staticInfo.pSwizzIdFbMemPageRanges = portMemAllocNonPaged(sizeof(*pPrivate->staticInfo.pSwizzIdFbMemPageRanges));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
        pPrivate->staticInfo.pSwizzIdFbMemPageRanges != NULL,
        status = NV_ERR_NO_MEMORY;
        goto failed;);
    portMemSet(pPrivate->staticInfo.pSwizzIdFbMemPageRanges, 0x0, sizeof(*pPrivate->staticInfo.pSwizzIdFbMemPageRanges));

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES,
                             pPrivate->staticInfo.pSwizzIdFbMemPageRanges,
                             sizeof(*pPrivate->staticInfo.pSwizzIdFbMemPageRanges));

    if (status == NV_ERR_NOT_SUPPORTED)
    {
        // Only supported on specific GPU's
        status = NV_OK;
        portMemFree(pPrivate->staticInfo.pSwizzIdFbMemPageRanges);
        pPrivate->staticInfo.pSwizzIdFbMemPageRanges = NULL;
    }
    else if (status != NV_OK)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status, failed);
    }

    return status;

failed:
    portMemFree(pPrivate->staticInfo.pProfiles);
    pPrivate->staticInfo.pProfiles = NULL;
    portMemFree(pPrivate->staticInfo.pSwizzIdFbMemPageRanges);
    pPrivate->staticInfo.pSwizzIdFbMemPageRanges = NULL;
    portMemFree(pPrivate->staticInfo.pCIProfiles);
    pPrivate->staticInfo.pCIProfiles = NULL;
    portMemFree(pKernelMIGManager->pPrivate->staticInfo.pSkylineInfo);
    pKernelMIGManager->pPrivate->staticInfo.pSkylineInfo = NULL;

    pPrivate->bInitialized = NV_FALSE;

    return status;
}

/*!
 * @brief Clears Static information set for vGPU
 */
void
kmigmgrClearStaticInfo_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 i;

    // Nothing to do
    if (!kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
        return;

    for (i = 0; i < KMIGMGR_MAX_GPU_INSTANCES; ++i)
    {
        if (pKernelMIGManager->kernelMIGGpuInstance[i].pShare != NULL)
        {
            serverFreeShare(&g_resServ, pKernelMIGManager->kernelMIGGpuInstance[i].pShare);
            pKernelMIGManager->kernelMIGGpuInstance[i].pShare = NULL;
        }

        kmigmgrInitGPUInstanceInfo(pGpu, pKernelMIGManager, &pKernelMIGManager->kernelMIGGpuInstance[i]);
    }
}

/*!
 * @brief   Save MIG topology from VGPU static info to persistence, if available.
 */
NV_STATUS
kmigmgrSaveToPersistenceFromVgpuStaticInfo_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY *pTopologySave;
    GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave;
    ENGTYPE_BIT_VECTOR engines;
    NvBool bTopologyValid;
    NvU32 GIIdx;
    NvU32 CIIdx;
    NvU32 savedCIIdx;
    NvU32 assignableGrMask;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       gpumgrGetSystemMIGInstanceTopo(gpuGetDBDF(pGpu), &pTopologySave),
                       NV_OK);

    // check VSI to see whether we've restored from it before.
    NV_CHECK_OR_RETURN(LEVEL_SILENT, !pTopologySave->bVgpuRestoredFromStaticInfo, NV_OK);

    // Check to see whether there is anything already saved
    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); ++GIIdx)
    {
        pGPUInstanceSave = &pTopologySave->saveGI[GIIdx];
        if (pGPUInstanceSave->bValid)
            break;
    }

    bTopologyValid = (GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI));
    NV_CHECK_OR_RETURN(LEVEL_SILENT, !bTopologyValid, NV_OK);

    // There can only be one saved GPU instance in VGPU
    pGPUInstanceSave = &pTopologySave->saveGI[0];

    pGPUInstanceSave->bValid = NV_TRUE;
    pGPUInstanceSave->swizzId = 0;
    pGPUInstanceSave->giInfo.partitionFlags = (DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _FULL) |
                                             DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, _FULL) );

    gpumgrCacheCreateGpuInstance(pGpu, pGPUInstanceSave->swizzId);

    bitVectorClrAll(&engines);

    if (pVSI->gpuPartitionInfo.grEngCount > 0)
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_GR(0),
                                    RM_ENGINE_TYPE_GR(pVSI->gpuPartitionInfo.grEngCount - 1)));
    {
        if (pVSI->gpuPartitionInfo.ceCount > 0)
            bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_COPY(0),
                                    RM_ENGINE_TYPE_COPY(pVSI->gpuPartitionInfo.ceCount - 1)));

    }

    if (pVSI->gpuPartitionInfo.nvDecCount > 0)
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_NVDEC(0),
                                    RM_ENGINE_TYPE_NVDEC(pVSI->gpuPartitionInfo.nvDecCount - 1)));

    if (pVSI->gpuPartitionInfo.nvEncCount > 0)
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_NVENC(0),
                                    RM_ENGINE_TYPE_NVENC(pVSI->gpuPartitionInfo.nvEncCount - 1)));

    if (pVSI->gpuPartitionInfo.nvJpgCount > 0)
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_NVJPEG(0),
                                    RM_ENGINE_TYPE_NVJPEG(pVSI->gpuPartitionInfo.nvJpgCount - 1)));

    if (pVSI->gpuPartitionInfo.nvOfaCount > 0)
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_OFA(0),
                                    RM_ENGINE_TYPE_OFA(pVSI->gpuPartitionInfo.nvOfaCount - 1)));

    bitVectorToRaw(&engines,
                   pGPUInstanceSave->giInfo.enginesMask,
                   sizeof(pGPUInstanceSave->giInfo.enginesMask));

    // Create a mask of GR IDs to later use in restoring
    assignableGrMask = 0x0;
    for (i = 0; i < pVSI->gpuPartitionInfo.grEngCount; i++)
        if (pVSI->gpuPartitionInfo.gpcsPerGr[i] != 0)
            assignableGrMask |= NVBIT32(i);

    NV_ASSERT_OR_RETURN(nvPopCount32(assignableGrMask) <= pVSI->execPartitionInfo.execPartCount, NV_ERR_INSUFFICIENT_RESOURCES);

    pGPUInstanceSave->giInfo.veidOffset = 0;
    pGPUInstanceSave->giInfo.veidCount = pVSI->gpuPartitionInfo.veidCount;
    pGPUInstanceSave->giInfo.gpcMask = DRF_MASK(pVSI->gpuPartitionInfo.gpcCount - 1 : 0);
    pGPUInstanceSave->giInfo.virtualGpcCount = pVSI->gpuPartitionInfo.virtualGpcCount;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        osRmCapRegisterSmcPartition(pGpu->pOsRmCaps, &pGPUInstanceSave->pOsRmCaps, pGPUInstanceSave->swizzId));

    savedCIIdx = 0;
    for (CIIdx = 0; CIIdx < pVSI->execPartitionInfo.execPartCount; ++CIIdx)
    {
        GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave = &pGPUInstanceSave->saveCI[savedCIIdx];
        NVC637_CTRL_EXEC_PARTITIONS_INFO *pExecPartInfo = &pVSI->execPartitionInfo.execPartInfo[CIIdx];
        NvU32 grIdx = pVSI->execSyspipeInfo.syspipeId[CIIdx];
        ENGTYPE_BIT_VECTOR engines;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, grIdx < RM_ENGINE_TYPE_GR_SIZE, NV_ERR_INVALID_STATE);

        pComputeInstanceSave->bValid = NV_TRUE;
        pComputeInstanceSave->ciInfo.sharedEngFlags = pExecPartInfo->sharedEngFlag;
        pComputeInstanceSave->id = pVSI->execPartitionInfo.execPartId[CIIdx];

        //
        // This association is not strictly enforced when allocating compute instances
        // however, the ordering RM expects here is that the local GR index per GI
        // matches with the execPartId that it was created to. Simply perform a non-fatal
        // check for logging purposes.
        //
        NV_CHECK(LEVEL_WARNING, pComputeInstanceSave->id == grIdx);

        bitVectorClrAll(&engines);
        bitVectorSetRange(&engines,
                          rangeMake(RM_ENGINE_TYPE_GR(grIdx),
                                    RM_ENGINE_TYPE_GR(grIdx)));

    {
        if (pExecPartInfo->ceCount > 0)
            bitVectorSetRange(&engines,
                              rangeMake(RM_ENGINE_TYPE_COPY(0),
                                        RM_ENGINE_TYPE_COPY(pExecPartInfo->ceCount - 1)));
    }

        if (pExecPartInfo->nvDecCount > 0)
            bitVectorSetRange(&engines,
                              rangeMake(RM_ENGINE_TYPE_NVDEC(0),
                                        RM_ENGINE_TYPE_NVDEC(pExecPartInfo->nvDecCount - 1)));

        if (pExecPartInfo->nvEncCount > 0)
            bitVectorSetRange(&engines,
                              rangeMake(RM_ENGINE_TYPE_NVENC(0),
                                        RM_ENGINE_TYPE_NVENC(pExecPartInfo->nvEncCount - 1)));

        if (pExecPartInfo->nvJpgCount > 0)
            bitVectorSetRange(&engines,
                              rangeMake(RM_ENGINE_TYPE_NVJPEG(0),
                                        RM_ENGINE_TYPE_NVJPEG(pExecPartInfo->nvJpgCount - 1)));

        if (pExecPartInfo->ofaCount > 0)
            bitVectorSetRange(&engines,
                              rangeMake(RM_ENGINE_TYPE_OFA(0),
                                        RM_ENGINE_TYPE_OFA(pExecPartInfo->ofaCount - 1)));

        bitVectorToRaw(&engines,
                       pComputeInstanceSave->ciInfo.enginesMask,
                       sizeof(pComputeInstanceSave->ciInfo.enginesMask));

        pComputeInstanceSave->ciInfo.gpcMask = DRF_MASK(pExecPartInfo->gpcCount - 1 : 0);
        pComputeInstanceSave->ciInfo.gfxGpcCount = pExecPartInfo->gfxGpcCount;
        pComputeInstanceSave->ciInfo.spanStart = pExecPartInfo->spanStart;
        pComputeInstanceSave->ciInfo.smCount = pExecPartInfo->smCount;
        pComputeInstanceSave->ciInfo.computeSize = pExecPartInfo->computeSize;
        pComputeInstanceSave->ciInfo.veidCount = pExecPartInfo->veidCount;
        pComputeInstanceSave->ciInfo.veidOffset = pExecPartInfo->veidStartOffset;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              osRmCapRegisterSmcExecutionPartition(pGPUInstanceSave->pOsRmCaps,
                                                                   &(pComputeInstanceSave->pOsRmCaps),
                                                                   pComputeInstanceSave->id));
        gpumgrCacheCreateComputeInstance(pGpu, pGPUInstanceSave->swizzId,
                                         pComputeInstanceSave->id);

        ++savedCIIdx;
    }

    //
    // Make sure that we never try to restore from static info again, to allow
    // guest to change the topology themselves.
    //
    pTopologySave->bVgpuRestoredFromStaticInfo = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Sets static KernelMIGManager, KernelGraphicsManager, and partitionInfo inside a vGPU
 */
NV_STATUS
kmigmgrSetStaticInfo_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo;
    VGPU_STATIC_INFO *pVSI;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    MIG_RESOURCE_ALLOCATION *pResourceAllocation;
    NvU32 veidOffset = 0;
    NV_RANGE memoryRange;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    RM_ENGINE_TYPE globalEngineType;
    RM_ENGINE_TYPE localEngineType;
    ENGTYPE_BIT_VECTOR *pGlobalMask;
    ENGTYPE_BIT_VECTOR *pLocalMask;

    pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_OBJECT_NOT_FOUND);

    // If MIG isn't enabled for this VM, nothing to do
    NV_CHECK_OR_RETURN(LEVEL_SILENT, IS_MIG_ENABLED(pGpu), NV_OK);

    pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);

    //
    // Fill required GPU instance info and create state needed in
    // KernelMIGManager/KernelGraphicsManager For legacy MIG vgpu policy, there
    // is only one GPU instance with no compute instances, so we statically
    // setup its resources taken from plugin and assign swizzID-0 to it inside
    // vGPU. For production vgpu policy, guest is responsible for requesting GPU
    // instance and compute instance creation, so we initialize RM with the
    // correct resource counts and let the rest of the instancing APIs work as
    // in host RM.
    //

    if (kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
    {
        //
        // In legacy flow, mark swizzid 0 as in use, as the GPU instance is
        // pre-populated
        //
        NV_ASSERT_OK_OR_RETURN(kmigmgrSetSwizzIdInUse(pGpu, pKernelMIGManager, 0));
    }
    else
    {
        //
        // In Prod flow, copy any GPU instance info retrieved from vpgu static info
        // into persistent storage on first driver boot.
        //
        NV_ASSERT_OK(kmigmgrSaveToPersistenceFromVgpuStaticInfo_HAL(pGpu, pKernelMIGManager));
    }

    if (kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
    {
        // In legacy flow, engines are pre-configured. VEIDs are marked as in-use
        kgrmgrSetVeidInUseMask(pGpu, pKernelGraphicsManager, DRF_MASK64(pVSI->gpuPartitionInfo.veidCount - 1 : 0));
        NV_PRINTF(LEVEL_INFO, "VF VEID in use mask: 0x%llX\n", kgrmgrGetVeidInUseMask(pGpu, pKernelGraphicsManager));
    }

    NV_ASSERT_OK_OR_RETURN(memmgrDiscoverMIGPartitionableMemoryRange_HAL(pGpu, pMemoryManager, &memoryRange));

    memmgrSetMIGPartitionableMemoryRange(pGpu, pMemoryManager, memoryRange);

    if (kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
    {
        GPUMGR_SAVE_GPU_INSTANCE save = { 0 };
        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS params =
        {
            .type = KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_RESTORE,
            .inst.restore.pGPUInstanceSave = &save
        };
        NvUuid uuid;

        save.bValid = NV_TRUE;
        save.swizzId = 0;
        save.giInfo.partitionFlags = (DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, _FULL) |
                                      DRF_DEF(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, _FULL) );

        bitVectorToRaw(&pStaticInfo->partitionableEngines,
                        save.giInfo.enginesMask,
                        sizeof(save.giInfo.enginesMask));

        save.giInfo.veidOffset = 0;
        save.giInfo.veidCount = pVSI->gpuPartitionInfo.veidCount;
        save.giInfo.gpcMask = DRF_MASK(pVSI->gpuPartitionInfo.gpcCount - 1 : 0);

        NV_ASSERT_OK_OR_RETURN(kmigmgrGenerateGPUInstanceUuid_HAL(pGpu, pKernelMIGManager, 0, &uuid));

        // Create static GPU instance for legacy vgpu flow
        NV_ASSERT_OK_OR_RETURN(kmigmgrSetGPUInstanceInfo(pGpu, pKernelMIGManager, 0/*SwizzID-0*/, uuid.uuid, params));
        NV_ASSERT_OK_OR_RETURN(kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, 0, &pKernelMIGGpuInstance));

        gpumgrCacheCreateGpuInstance(pGpu, 0);

        pResourceAllocation = &pKernelMIGGpuInstance->resourceAllocation;
        pResourceAllocation->virtualGpcCount = pVSI->gpuPartitionInfo.virtualGpcCount;
        pResourceAllocation->gfxGpcCount = pVSI->gpuPartitionInfo.gfxGpcCount;

        pGlobalMask = &pResourceAllocation->engines;
        pLocalMask = &pResourceAllocation->localEngines;

        // Iterate over both global/local masks at the same time
        FOR_EACH_IN_BITVECTOR_PAIR(pGlobalMask, globalEngineType, pLocalMask, localEngineType)
        {
            NvU32 globalGrIdx;
            NvU32 localGrIdx;

            if (!RM_ENGINE_TYPE_IS_GR(globalEngineType))
                continue;

            globalGrIdx = RM_ENGINE_TYPE_GR_IDX(globalEngineType);
            localGrIdx = RM_ENGINE_TYPE_GR_IDX(localEngineType);

            if (pVSI->gpuPartitionInfo.veidsPerGr[localGrIdx] != 0)
            {
                // set VEID mask for grIdx
                kgrmgrSetGrIdxVeidMask(pGpu, pKernelGraphicsManager, globalGrIdx, DRF_MASK64(pVSI->gpuPartitionInfo.veidsPerGr[localGrIdx] - 1:0) << veidOffset);
                veidOffset += pVSI->gpuPartitionInfo.veidsPerGr[localGrIdx];
            }

            if (pVSI->gpuPartitionInfo.gpcsPerGr[localGrIdx] != 0)
            {
                KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, globalGrIdx);

                kgraphicsInvalidateStaticInfo(pGpu, pKernelGraphics);
                NV_ASSERT_OK(kgraphicsLoadStaticInfo_HAL(pGpu, pKernelGraphics, 0));
            }
        }
        FOR_EACH_IN_BITVECTOR_PAIR_END();
    }

    return NV_OK;
}

/*!
 * @brief Disable RC Watchdog
 */
NV_STATUS
kmigmgrDisableWatchdog_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMigManager
)
{
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    NvU32 wdFlags = pKernelRc->watchdog.flags;
    NvS32 enableRequestsRefcount;
    NvS32 disableRequestsRefcount;
    NvS32 softDisableRequestsRefcount;

    krcWatchdogGetReservationCounts(pKernelRc,
                                    &enableRequestsRefcount,
                                    &disableRequestsRefcount,
                                    &softDisableRequestsRefcount);

    //
    // If clients have made requests to the watchdog, we can't enable MIG until
    // these clients have gone away because we disallow them from modifying WD
    // state while MIG is active but these clients need to release their
    // refcount on exit
    //
    if ((enableRequestsRefcount != 0) || (disableRequestsRefcount != 0) ||
        (softDisableRequestsRefcount != 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to disable watchdog with outstanding reservations - enable: %d disable: %d softDisable: %d.\n",
                  enableRequestsRefcount,
                  disableRequestsRefcount,
                  softDisableRequestsRefcount);

        return NV_ERR_STATE_IN_USE;
    }

    NV_CHECK_OR_RETURN(LEVEL_SILENT, (wdFlags & WATCHDOG_FLAGS_INITIALIZED) != 0x0, NV_OK);

    pKernelMigManager->bRestoreWatchdog = NV_TRUE;
    pKernelMigManager->bReenableWatchdog = (wdFlags & WATCHDOG_FLAGS_DISABLED) == 0x0;

    return krcWatchdogShutdown(pGpu, pKernelRc);
}

/*!
 * @brief Enable RC Watchdog if it was enabled before kmigmgrDisableWatchdog invocation
 */
NV_STATUS
kmigmgrRestoreWatchdog_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMigManager
)
{
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelMigManager->bRestoreWatchdog, NV_OK);

    if (pKernelMigManager->bReenableWatchdog)
    {
        krcWatchdogEnable(pKernelRc, NV_FALSE /* bOverRide */);
    }

    pKernelMigManager->bRestoreWatchdog = NV_FALSE;
    pKernelMigManager->bReenableWatchdog = NV_FALSE;

    return krcWatchdogInit_HAL(pGpu, pKernelRc);
}

/*!
 * @brief   Function to set swizzId in use
 */
NV_STATUS
kmigmgrSetSwizzIdInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    // Validate that same ID is not already set and then set the ID
    NvU64 mask = NVBIT64(swizzId);

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (mask & pKernelMIGManager->swizzIdInUseMask)
    {
        NV_PRINTF(LEVEL_ERROR, "SwizzID - %d already in use\n", swizzId);
        DBG_BREAKPOINT();
        return NV_ERR_STATE_IN_USE;
    }

    pKernelMIGManager->swizzIdInUseMask |= mask;

    return NV_OK;
}

/*!
 * @brief   Function to mark swizzId free
 */
NV_STATUS
kmigmgrClearSwizzIdInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    // Validate that same ID is not already set and then set the ID
    NvU64 mask = NVBIT64(swizzId);

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!(mask & pKernelMIGManager->swizzIdInUseMask))
    {
        NV_PRINTF(LEVEL_ERROR, "SwizzID - %d not in use\n", swizzId);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    pKernelMIGManager->swizzIdInUseMask &= ~mask;

    return NV_OK;
}

/*!
 * @brief   Function to see if swizzId in use
 */
NvBool
kmigmgrIsSwizzIdInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    NvU64 mask = NVBIT64(swizzId);

    if (mask & pKernelMIGManager->swizzIdInUseMask)
        return NV_TRUE;

    return NV_FALSE;
}

/*
 * @brief Return global swizzId mask
 */
NvU64
kmigmgrGetSwizzIdInUseMask_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    return pKernelMIGManager->swizzIdInUseMask;
}

/*!
 * @brief   Marks the given engines as in use by some GPU instance
 */
NV_STATUS
kmigmgrSetEnginesInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    ENGTYPE_BIT_VECTOR *pEngines
)
{
    ENGTYPE_BIT_VECTOR tempEngines;

    NV_ASSERT_OR_RETURN(pEngines != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorAnd(&tempEngines, pEngines, &pKernelMIGManager->partitionableEnginesInUse);
    // Ensure no engine in given mask is marked as in-use
    NV_ASSERT_OR_RETURN(bitVectorTestAllCleared(&tempEngines), NV_ERR_STATE_IN_USE);

    // partitionableEnginesInUse |= pEngines
    bitVectorOr(&pKernelMIGManager->partitionableEnginesInUse,
                &pKernelMIGManager->partitionableEnginesInUse,
                pEngines);
    return NV_OK;
}

/*!
 * @brief   Marks the given sys pipes as no longer in use by any GPU instance
 */
NV_STATUS
kmigmgrClearEnginesInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    ENGTYPE_BIT_VECTOR *pEngines
)
{
    ENGTYPE_BIT_VECTOR tempEngines;

    NV_ASSERT_OR_RETURN(pEngines != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorAnd(&tempEngines, pEngines, &pKernelMIGManager->partitionableEnginesInUse);
    // Ensure every engine in given mask is marked as in-use
    NV_ASSERT_OR_RETURN(bitVectorTestEqual(&tempEngines, pEngines), NV_ERR_STATE_IN_USE);

    // partitionableEnginesInUse &= ~(pEngines)
    bitVectorComplement(&tempEngines, pEngines);
    bitVectorAnd(&pKernelMIGManager->partitionableEnginesInUse,
                 &pKernelMIGManager->partitionableEnginesInUse,
                 &tempEngines);
    return NV_OK;
}

/*!
 * @brief   Checks whether given engine is in use by any GPU instance
 */
NvBool
kmigmgrIsEngineInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    RM_ENGINE_TYPE rmEngineType
)
{
    return bitVectorTest(&pKernelMIGManager->partitionableEnginesInUse, rmEngineType);
}

/*
 * @brief   Determines whether RM_ENGINE_TYPE can be partitioned
 */
NvBool
kmigmgrIsEnginePartitionable_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    RM_ENGINE_TYPE rmEngineType
)
{
    return kmigmgrIsMIGSupported(pGpu, pKernelMIGManager) &&
           (RM_ENGINE_TYPE_IS_COPY(rmEngineType) ||
            RM_ENGINE_TYPE_IS_GR(rmEngineType) ||
            RM_ENGINE_TYPE_IS_NVDEC(rmEngineType) ||
            RM_ENGINE_TYPE_IS_NVENC(rmEngineType) ||
            RM_ENGINE_TYPE_IS_NVJPEG(rmEngineType) ||
            RM_ENGINE_TYPE_IS_OFA(rmEngineType));
}

/*!
 * @brief   Function to determine whether global RM_ENGINE_TYPE belongs to given
 *          gpu/compute instance.
 *
 * @return NV_TRUE if this engine falls within the given instance. NV_FALSE
 * otherwise. Non-partitioned engines fall within all instances.
 */
NvBool
kmigmgrIsEngineInInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    RM_ENGINE_TYPE globalRmEngType,
    MIG_INSTANCE_REF ref
)
{
    return kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                             globalRmEngType, NULL) == NV_OK;
}

/*!
 * @brief   Function to determine whether local RM_ENGINE_TYPE belongs to given
 *          gpu/compute instance.
 *
 * @return NV_TRUE if this engine falls within the given instance. NV_FALSE
 * otherwise. Non-partitioned engines fall within all instances.
 */
NvBool
kmigmgrIsLocalEngineInInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    RM_ENGINE_TYPE localRmEngType,
    MIG_INSTANCE_REF ref
)
{
    ENGTYPE_BIT_VECTOR *pLocalEngines;

    if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, localRmEngType))
    {
        return NV_TRUE;
    }

    pLocalEngines = (ref.pMIGComputeInstance != NULL) ?
                    &ref.pMIGComputeInstance->resourceAllocation.localEngines :
                    &ref.pKernelMIGGpuInstance->resourceAllocation.localEngines;

    return bitVectorTest(pLocalEngines, localRmEngType);
}

/*!
 * @brief   Trim runlist buffer pools
 */
void
kmigmgrTrimInstanceRunlistBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    RM_ENGINE_TYPE rmEngineType;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId))
        return;

    if (!ctxBufPoolIsSupported(pGpu))
        return;

    for (rmEngineType = 0; rmEngineType < RM_ENGINE_TYPE_LAST; rmEngineType++)
    {
        if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType) ||
            !kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType) ||
            !kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, kmigmgrMakeGIReference(pKernelMIGGpuInstance)))
        {
            continue;
        }

        if (kfifoGetRunlistBufPool(pGpu, pKernelFifo, rmEngineType) != NULL)
        {
            ctxBufPoolTrim(kfifoGetRunlistBufPool(pGpu, pKernelFifo, rmEngineType));
        }
    }
}

//
// Creates runlist buffers for engines belonging to this GPU instance from non-partitionable memory and
// recreates these runlist buffers in GPU instance's memory.
//
NV_STATUS
kmigmgrCreateGPUInstanceRunlists_FWCLIENT
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 index;
    NvU32 runlistId;
    RM_ENGINE_TYPE rmEngineType;
    NvU32 engDesc;
    NV_STATUS status = NV_OK;
    NvU32 numEngines = kfifoGetNumEschedDrivenEngines(pKernelFifo);
    NvU32 maxRunlists = kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo);
    NvU64 runlistAlign;
    NvU64 allocFlags;
    NvU32 attr;
    NV_ADDRESS_SPACE aperture;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams;

    // TODO: Mem partitioning check should suffice here
    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId) ||
        !ctxBufPoolIsSupported(pGpu))
    {
        return NV_OK;
    }

    kfifoRunlistGetBufAllocParams(pGpu, &aperture, &attr, &allocFlags);
    allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;

    for (index = 0; index < numEngines; index++)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_INVALID, index,
                                     ENGINE_INFO_TYPE_RUNLIST, &runlistId),
            failed);

        if ((runlistId >= maxRunlists) || (runlistId >= NV_NBITS_IN_TYPE(pKernelMIGGpuInstance->runlistIdMask)))
        {
            status = NV_ERR_INVALID_STATE;
            goto failed;
        }

        // some engines share runlists. so skip if have already dealt with this runlist
        if ((pKernelMIGGpuInstance->runlistIdMask & NVBIT64(runlistId)) != 0x0)
        {
            continue;
        }

        NV_ASSERT_OK_OR_GOTO(status,
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RUNLIST, runlistId,
                                     ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType),
            failed);

        NV_ASSERT_OK_OR_GOTO(status,
            kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                     ENGINE_INFO_TYPE_RUNLIST, runlistId,
                                     ENGINE_INFO_TYPE_ENG_DESC, &engDesc),
            failed);

        // Check if this is a partitionable engine. Non-partitionable engine runlists can stay in RM reserved memory
        if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType))
        {
            continue;
        }

        // if partitionable engine doesn't belong to this GPU instance then nothing to do
        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, kmigmgrMakeGIReference(pKernelMIGGpuInstance)))
        {
            continue;
        }

        //
        // Sched is only managed by Physical RM.
        // If running on GSP client, we will instead allocate the runlist buffers from the ctxbuf pool
        // and promote them to GSP later. GSP will skip the runlist buffer allocation during schedInit
        // and wait for the RPC to memdescDescribe the allocation from client RM.
        //
        // OBJSCHEDMGR is not valid in kernel RM. Allocate and store runlist buffers in OBJFIFO,
        // which will be sent to GSP to store in its schedmgr
        //
        NV_ASSERT_OK_OR_GOTO(status,
            kfifoRunlistAllocBuffers(pGpu, pKernelFifo,
                                     NV_TRUE,
                                     aperture,
                                     runlistId,
                                     attr,
                                     allocFlags,
                                     0,
                                     NV_TRUE,
                                     pKernelFifo->pppRunlistBufMemDesc[runlistId]),
            failed);

        // Add runlist to GPU instance
        pKernelMIGGpuInstance->runlistIdMask |= NVBIT64(runlistId);
    }

    runlistAlign = NVBIT64(kfifoRunlistGetBaseShift_HAL(pKernelFifo));

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    NV_ASSERT_OR_GOTO(pParams != NULL, failed);

    ct_assert(sizeof(pParams->runlistIdMask) == sizeof(pKernelMIGGpuInstance->runlistIdMask));
    pParams->runlistIdMask = pKernelMIGGpuInstance->runlistIdMask;
    pParams->swizzId = pKernelMIGGpuInstance->swizzId;

    for (runlistId = 0; runlistId < maxRunlists; runlistId++)
    {
        if (pParams->runlistIdMask & NVBIT64(runlistId))
        {
            for (index = 0; index < NUM_BUFFERS_PER_RUNLIST; index++)
            {
                MEMORY_DESCRIPTOR *pSourceMemDesc = pKernelFifo->pppRunlistBufMemDesc[runlistId][index];

                pParams->rlBuffers[runlistId][index].base = (NvU64)memdescGetPhysAddr(pSourceMemDesc, AT_GPU, 0);
                pParams->rlBuffers[runlistId][index].size = pSourceMemDesc->ActualSize;
                pParams->rlBuffers[runlistId][index].alignment = runlistAlign;
                pParams->rlBuffers[runlistId][index].addressSpace = memdescGetAddressSpace(pSourceMemDesc);
                pParams->rlBuffers[runlistId][index].cpuCacheAttrib = attr;

            }
        }
    }

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS,
                             pParams,
                             sizeof(*pParams));

    portMemFree(pParams);

    NV_ASSERT_OK_OR_GOTO(status, status, failed);

    //
    // Trim out any additional memory after runlist buffers are allocated
    // from ctx buf pools
    //
    kmigmgrTrimInstanceRunlistBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    return NV_OK;

failed:
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
        kmigmgrDeleteGPUInstanceRunlists_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance));

    return status;
}

//
// Deletes runlist buffers for all partitionable engines from GPU instance's memory and
// reallocates these runlist buffers in non-partitionable memory.
//
NV_STATUS
kmigmgrDeleteGPUInstanceRunlists_FWCLIENT
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 runlistId;
    NV_STATUS status = NV_OK;
    NvU32 bufIdx;
    MEMORY_DESCRIPTOR **ppRlBuffer;

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId) ||
        !ctxBufPoolIsSupported(pGpu))
    {
        NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance->runlistIdMask == 0, NV_ERR_INVALID_STATE);
        return NV_OK;
    }

    FOR_EACH_INDEX_IN_MASK(64, runlistId, pKernelMIGGpuInstance->runlistIdMask)
    {
        for (bufIdx = 0; bufIdx < NUM_BUFFERS_PER_RUNLIST; bufIdx++)
        {
            ppRlBuffer = &(pKernelFifo->pppRunlistBufMemDesc[runlistId][bufIdx]);

            if (*ppRlBuffer != NULL)
            {
                memdescFree(*ppRlBuffer);
                memdescDestroy(*ppRlBuffer);
                *ppRlBuffer = NULL;
            }
        }

        // remove runlist from GPU instance
        pKernelMIGGpuInstance->runlistIdMask &= ~(NVBIT64(runlistId));

    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

/*!
 * @brief   Load MIG instance topology from persistence, if available.
 *          If MIG is disabled, this operation will be skipped with a warning.
 */
NV_STATUS
kmigmgrRestoreFromPersistence_PF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY *pTopologySave = NULL;
    NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pPartImportParams = NULL;
    NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *pExecPartImportParams = NULL;
    NvU32 GIIdx;
    NvU32 CIIdx;
    NvBool bTopologyValid;
    NvHandle hClient = NV01_NULL_OBJECT;
    NvHandle hDevice = NV01_NULL_OBJECT;
    NvHandle hSubdevice = NV01_NULL_OBJECT;

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       gpumgrGetSystemMIGInstanceTopo(gpuGetDBDF(pGpu), &pTopologySave),
                       NV_OK);

    // Check to see whether there was actually anything saved
    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); ++GIIdx)
    {
        GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave = &pTopologySave->saveGI[GIIdx];
        if (pGPUInstanceSave->bValid)
            break;
    }

    bTopologyValid = (GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI));
    if (!bTopologyValid)
    {
        // The boot config is honored only if no topology was saved previously (e.g. on reboot)
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kmigmgrRestoreFromBootConfig_HAL(pGpu, pKernelMIGManager));
        return NV_OK;
    }

    if (!IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_WARNING, "Skipping reinitialization of persistent MIG instances due to MIG disablement!\n");
        //
        // If we ended up here, we have inconsistent state in that there are instances to be restored
        // but MIG is disabled. This also means, that /proc filesystem is populated with nodes for the
        // instances that we are expected to restore, but wont do so. Clean them up.
        //
        gpumgrUnregisterRmCapsForMIGGI(gpuGetDBDF(pGpu));
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClient, &hDevice, &hSubdevice));

    pPartImportParams = portMemAllocNonPaged(sizeof(*pPartImportParams));
    NV_CHECK_OR_ELSE(LEVEL_ERROR, pPartImportParams != NULL,
        status = NV_ERR_NO_MEMORY;
        goto cleanup; );
    pExecPartImportParams = portMemAllocNonPaged(sizeof(*pExecPartImportParams));
    NV_CHECK_OR_ELSE(LEVEL_ERROR, pExecPartImportParams != NULL,
        status = NV_ERR_NO_MEMORY;
        goto cleanup; );

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); ++GIIdx)
    {
        GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave = &pTopologySave->saveGI[GIIdx];
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

        if (!pGPUInstanceSave->bValid)
            continue;

        portMemSet(pPartImportParams, 0, sizeof(*pPartImportParams));
        pPartImportParams->swizzId = pGPUInstanceSave->swizzId;
        portMemCopy(&pPartImportParams->info, sizeof(pPartImportParams->info),
                    &pGPUInstanceSave->giInfo, sizeof(pGPUInstanceSave->giInfo));

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi,
                            hClient,
                            hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE,
                            pPartImportParams,
                            sizeof(*pPartImportParams)),
            cleanup);

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, pGPUInstanceSave->swizzId, &pKernelMIGGpuInstance),
            cleanup);

        // Restore capability caps
        pKernelMIGGpuInstance->pOsRmCaps = pGPUInstanceSave->pOsRmCaps;

        for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pGPUInstanceSave->saveCI); ++CIIdx)
        {
            GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave = &pGPUInstanceSave->saveCI[CIIdx];
            NvHandle hSubscription;
            NVC637_ALLOCATION_PARAMETERS alloc;

            if (!pComputeInstanceSave->bValid)
                continue;

            portMemSet(&alloc, 0, sizeof(alloc));
            alloc.swizzId = pGPUInstanceSave->swizzId;
            NV_ASSERT_OK_OR_GOTO(status,
                pRmApi->AllocWithSecInfo(pRmApi,
                                         hClient,
                                         hSubdevice,
                                         &hSubscription,
                                         AMPERE_SMC_PARTITION_REF,
                                         &alloc,
                                         sizeof(alloc),
                                         RMAPI_ALLOC_FLAGS_NONE,
                                         NULL,
                                         &pRmApi->defaultSecInfo),
                cleanup);

            portMemSet(pExecPartImportParams, 0, sizeof(*pExecPartImportParams));
            pExecPartImportParams->id = pComputeInstanceSave->id;
            pExecPartImportParams->bCreateCap = NV_FALSE;
            portMemCopy(&pExecPartImportParams->info, sizeof(pExecPartImportParams->info),
                        &pComputeInstanceSave->ciInfo, sizeof(pComputeInstanceSave->ciInfo));

            NV_ASSERT_OK_OR_GOTO(status,
                pRmApi->Control(pRmApi,
                                hClient,
                                hSubscription,
                                NVC637_CTRL_CMD_EXEC_PARTITIONS_IMPORT,
                                pExecPartImportParams,
                                sizeof(*pExecPartImportParams)),
                cleanup);

            // Restore capability caps
            pKernelMIGGpuInstance->MIGComputeInstance[pExecPartImportParams->id].pOsRmCaps = pComputeInstanceSave->pOsRmCaps;

            pRmApi->Free(pRmApi, hClient, hSubscription);
        }
    }

cleanup:
    rmapiutilFreeClientAndDeviceHandles(pRmApi, &hClient, &hDevice, &hSubdevice);
    portMemFree(pPartImportParams);
    portMemFree(pExecPartImportParams);

    //
    // Let stateUnload handle an error teardown case, since it has to be
    // coordinated between CPU/GSP
    //
    return status;
}

/*!
 * @brief   Load MIG instance topology from persistence, if available.
 *          If MIG is disabled, this operation will be skipped with a warning.
 */
NV_STATUS
kmigmgrRestoreFromPersistence_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV_STATUS status = NV_OK;
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY *pTopologySave = NULL;
    NvU32 GIIdx;
    NvU32 CIIdx;
    NvBool bTopologyValid;
    NvBool bMemoryPartitioningNeeded;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       gpumgrGetSystemMIGInstanceTopo(gpuGetDBDF(pGpu), &pTopologySave),
                       NV_OK);

    // Check to see whether there was actually anything saved
    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); ++GIIdx)
    {
        GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave = &pTopologySave->saveGI[GIIdx];
        if (pGPUInstanceSave->bValid)
            break;
    }

    bTopologyValid = (GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI));
    NV_CHECK_OR_RETURN(LEVEL_SILENT, bTopologyValid, NV_OK);

    if (!IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_WARNING, "Skipping reinitialization of persistent MIG instances due to MIG disablement!\n");
        gpumgrUnregisterRmCapsForMIGGI(gpuGetDBDF(pGpu));
        return NV_OK;
    }

    bMemoryPartitioningNeeded = kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pTopologySave->saveGI[0].swizzId);

    // Perform all initialization that must be done when MIG is first enabled
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmigmgrSetMIGState(pGpu, pKernelMIGManager, bMemoryPartitioningNeeded, NV_TRUE, NV_FALSE));

    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(pTopologySave->saveGI); ++GIIdx)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;
        GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave = &pTopologySave->saveGI[GIIdx];
        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS restore =
        {
            .type = KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_RESTORE,
            .inst.restore.pGPUInstanceSave = pGPUInstanceSave
        };
        NvU32 swizzId = pGPUInstanceSave->swizzId;
        NvUuid uuid;

        if (!pGPUInstanceSave->bValid)
            continue;

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGenerateGPUInstanceUuid_HAL(pGpu, pKernelMIGManager, swizzId, &uuid),
            fail);

        // Create a GPU instance using the saved data
        NV_CHECK_OK_OR_GOTO(status, LEVEL_WARNING,
            kmigmgrCreateGPUInstance(pGpu, pKernelMIGManager, swizzId, uuid.uuid, restore, NV_TRUE, NV_FALSE),
            fail);

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGPUInstance),
            fail);

        // Restore capability caps
        pKernelMIGGPUInstance->pOsRmCaps = pGPUInstanceSave->pOsRmCaps;

        for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pGPUInstanceSave->saveCI); ++CIIdx)
        {
            GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave = &pGPUInstanceSave->saveCI[CIIdx];
            KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS restore =
            {
                .type = KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE,
                .inst.restore.pComputeInstanceSave = pComputeInstanceSave
            };
            //
            // This id variable actually doesn't need to be initialized since the callee
            // is not referencing to its value. But GCC13 is unhappy with that, thus WAR
            // this issue by initializing it.
            //
            NvU32 id = pComputeInstanceSave->id;

            if (!pComputeInstanceSave->bValid)
                continue;

            // Create a compute instance on this GPU instance using the saved data
            NV_CHECK_OK_OR_GOTO(status, LEVEL_WARNING,
                kmigmgrCreateComputeInstances_HAL(pGpu, pKernelMIGManager, pKernelMIGGPUInstance, NV_FALSE, restore, &id, NV_FALSE),
                fail);

            // Restore capability caps
            pKernelMIGGPUInstance->MIGComputeInstance[id].pOsRmCaps = pComputeInstanceSave->pOsRmCaps;
        }
    }

    return NV_OK;

fail:

    // Clean up anything we created and bail
    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGPUInstance->MIGComputeInstance); ++CIIdx)
        {
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pKernelMIGGPUInstance->MIGComputeInstance[CIIdx];

            // Skip invalid compute instances
            if (!pMIGComputeInstance->bValid)
                continue;

            NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
                kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGPUInstance, CIIdx, NV_TRUE));
        }

        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
            kmigmgrInvalidateGPUInstance(pGpu, pKernelMIGManager, pKernelMIGGPUInstance->swizzId, NV_TRUE));
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();

    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
        kmigmgrSetMIGState(pGpu, pKernelMIGManager, bMemoryPartitioningNeeded, NV_FALSE, NV_FALSE));

    return status;
}

/*
 * @brief Initialize MIG gpu instance
 */
void
kmigmgrInitGPUInstanceInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NvU32 i;

    bitVectorClrAll(&pKernelMIGGpuInstance->exclusiveEngMask);
    bitVectorClrAll(&pKernelMIGGpuInstance->sharedEngMask);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++i)
    {
        NV_ASSERT(!pKernelMIGGpuInstance->MIGComputeInstance[i].bValid);
        pKernelMIGGpuInstance->MIGComputeInstance[i].pOsRmCaps = NULL;
        pKernelMIGGpuInstance->MIGComputeInstance[i].id = KMIGMGR_COMPUTE_INSTANCE_ID_INVALID;
    }

    pKernelMIGGpuInstance->swizzId = KMIGMGR_SWIZZID_INVALID;
    pKernelMIGGpuInstance->hMemory = NV01_NULL_OBJECT;
    pKernelMIGGpuInstance->pShare = NULL;
    pKernelMIGGpuInstance->pMemoryPartitionHeap = NULL;
    pKernelMIGGpuInstance->bValid = NV_FALSE;
    pKernelMIGGpuInstance->memRange = NV_RANGE_EMPTY;
    pKernelMIGGpuInstance->pMIGGpuInstance = NULL;
    pKernelMIGGpuInstance->pOsRmCaps = NULL;
    pKernelMIGGpuInstance->pProfile = NULL;

    portMemSet(&pKernelMIGGpuInstance->resourceAllocation, 0x0, sizeof(pKernelMIGGpuInstance->resourceAllocation));
}

/*!
 * @brief   Checks Devinit owned scratch bit to see if MIG is enabled or not
 *
 * @return  NV_TRUE if there is valid GPU instance in VGPU static info
 */
NvBool
kmigmgrIsDevinitMIGBitSet_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_FALSE);

    return pVSI->gpuPartitionInfo.swizzId != KMIGMGR_SWIZZID_INVALID;
}

/*!
 * @brief   Function to set device profiling in use
 */
NV_STATUS
kmigmgrSetDeviceProfilingInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV_ASSERT_OR_RETURN(!kmigmgrIsDeviceProfilingInUse(pGpu, pKernelMIGManager),
                        NV_ERR_STATE_IN_USE);
    pKernelMIGManager->bDeviceProfilingInUse = NV_TRUE;
    return NV_OK;
}

/*!
 * @brief   Function to clear device profiling in-use
 */
void
kmigmgrClearDeviceProfilingInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    pKernelMIGManager->bDeviceProfilingInUse = NV_FALSE;
}

/*!
 * @brief   Function to check if device profiling is in-use
 */
NvBool
kmigmgrIsDeviceProfilingInUse_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    return pKernelMIGManager->bDeviceProfilingInUse;
}

/*!
 * @brief   Function to check if specific device is subscribed to DeviceProfiling
 */
NvBool
kmigmgrIsDeviceUsingDeviceProfiling_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Device *pDevice
)
{
    RsClient *pRsClient;
    GPUInstanceSubscription *pGPUInstanceSubscription;
    Subdevice *pSubdevice;
    NV_STATUS status;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, IS_MIG_ENABLED(pGpu), NV_FALSE);

    if (!kmigmgrIsDeviceProfilingInUse(pGpu, pKernelMIGManager))
    {
        return NV_FALSE;
    }

    NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_ARGUMENT);
    pRsClient = RES_GET_CLIENT(pDevice);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByInstance(pRsClient, RES_GET_HANDLE(pDevice), 0, &pSubdevice));

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        gisubscriptionGetGPUInstanceSubscription(pRsClient, RES_GET_HANDLE(pSubdevice), &pGPUInstanceSubscription),
        return NV_FALSE; );

    return gisubscriptionIsDeviceProfiling(pGPUInstanceSubscription);
}

/*!
 * @brief enable all LCE engines for use by GPU instances
 */
NV_STATUS
kmigmgrEnableAllLCEs_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bEnableAllLCEs
)
{
    KernelCE *pKCe = NULL;

    //
    // AMODEL support of CEs is faked. No actual work needs to be done for
    // AMODEL here, so just return NV_OK early to avoid triggering assertions.
    //
    NV_CHECK_OR_RETURN(LEVEL_SILENT, !IsAMODEL(pGpu), NV_OK);

    NV_ASSERT_OK_OR_RETURN(kceFindFirstInstance(pGpu, &pKCe));

    if (bEnableAllLCEs)
    {
        NV_ASSERT_OK_OR_RETURN(kceUpdateClassDB_HAL(pGpu, pKCe));
    }
    else
    {
        KCE_ITER_SHIM_BEGIN(pGpu, pKCe)
            NV_ASSERT_OK_OR_RETURN(kceTopLevelPceLceMappingsUpdate(pGpu, pKCe));
        KCE_ITER_END
    }

    return NV_OK;
}

/*!
 * @brief   Retrieves instance(s) associated with a device, if applicable
 */
NV_STATUS
kmigmgrGetInstanceRefFromDevice_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Device *pDevice,
    MIG_INSTANCE_REF *pRef
)
{
    NV_STATUS status = NV_OK;
    RsClient *pRsClient;
    GPUInstanceSubscription *pGPUInstanceSubscription;
    ComputeInstanceSubscription *pComputeInstanceSubscription = NULL;
    Subdevice *pSubdevice;
    MIG_INSTANCE_REF ref;

    NV_ASSERT_OR_RETURN(pRef != NULL, NV_ERR_INVALID_ARGUMENT);
    *pRef = kmigmgrMakeNoMIGReference();

    if (!IS_MIG_IN_USE(pGpu))
    {
        return NV_ERR_INVALID_STATE;
    }

    NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_ARGUMENT);
    pRsClient = RES_GET_CLIENT(pDevice);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByInstance(pRsClient, RES_GET_HANDLE(pDevice), 0, &pSubdevice));

    NV_CHECK_OK_OR_RETURN(LEVEL_NOTICE,
        gisubscriptionGetGPUInstanceSubscription(pRsClient, RES_GET_HANDLE(pSubdevice),
                                                 &pGPUInstanceSubscription));

    ref.pKernelMIGGpuInstance = gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription);

    status = cisubscriptionGetComputeInstanceSubscription(pRsClient,
                                                          RES_GET_HANDLE(pGPUInstanceSubscription),
                                                          &pComputeInstanceSubscription);
    if (status == NV_OK)
    {
        ref = kmigmgrMakeCIReference(gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription),
                                     cisubscriptionGetMIGComputeInstance(pComputeInstanceSubscription));
    }
    else
    {
        ref = kmigmgrMakeGIReference(gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription));
        // Quash status, this is optional
        status = NV_OK;
    }

    NV_CHECK_OR_RETURN(LEVEL_SILENT, kmigmgrIsMIGReferenceValid(&ref), NV_ERR_INVALID_STATE);
    *pRef = ref;
    return status;
}

/*!
 * @brief   Retrieves GPU instance heap associated with a device, if applicable
 */
NV_STATUS
kmigmgrGetMemoryPartitionHeapFromDevice_IMPL
(
    OBJGPU           *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Device           *pDevice,
    Heap            **ppMemoryPartitionHeap
)
{
    MIG_INSTANCE_REF ref;
    NV_STATUS rmStatus = NV_OK;
    NvHandle hClient;

    NV_ASSERT_OR_RETURN(IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_ARGUMENT);
    hClient = RES_GET_CLIENT_HANDLE(pDevice);

    rmStatus = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref);
    if ((rmStatus != NV_OK) || !kmigmgrIsMIGReferenceValid(&ref))
    {
        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilegeByHandle(hClient);

        // It's okay for kernel/root clients to not be associated to a GPU instance
        if (privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            rmStatus = NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to get GPU instance for non-privileged client hClient=0x%08x!\n",
                      hClient);

            // if we got here due to a bogus GPU instance info, actually return an error
            if (rmStatus == NV_OK)
                rmStatus = NV_ERR_INVALID_STATE;
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(ppMemoryPartitionHeap != NULL, NV_ERR_INVALID_ARGUMENT);
        *ppMemoryPartitionHeap = ref.pKernelMIGGpuInstance->pMemoryPartitionHeap;
        NV_PRINTF(LEVEL_INFO,
                  "GPU instance heap found for hClient = 0x%08x with swizzId = %d!\n",
                  hClient, ref.pKernelMIGGpuInstance->swizzId);
    }

    return rmStatus;
}

/*!
 * @brief   Retrieves swizzid associated with a client, if applicable
 */
NV_STATUS
kmigmgrGetSwizzIdFromDevice_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Device *pDevice,
    NvU32 *pSwizzId
)
{
    MIG_INSTANCE_REF ref;
    NV_ASSERT_OK_OR_RETURN(
        kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

    *pSwizzId = ref.pKernelMIGGpuInstance->swizzId;
    return NV_OK;
}

/*!
 * @brief   Printout properties of specified MIG gpu instance
 */
void
kmigmgrPrintGPUInstanceInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    NV_STATUS status;
    const MIG_GPU_INSTANCE_MEMORY_CONFIG *pGPUInstanceMemConfig;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_RANGE partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);

    NvU32 grCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                              RM_ENGINE_TYPE_GR(0));
    NvU32 ceCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                              RM_ENGINE_TYPE_COPY(0));
    NvU32 decCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               RM_ENGINE_TYPE_NVDEC(0));
    NvU32 encCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               RM_ENGINE_TYPE_NVENC(0));
    NvU32 jpgCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               RM_ENGINE_TYPE_NVJPG);
    NvU32 ofaCount = kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               RM_ENGINE_TYPE_OFA(0));

#define PADDING_STR "-----------------------------------------------------------------"

    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "SwizzId",
              "SwizzId Table Mask",
              "Gpc Count");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18d | %18s | %18d  |\n",
              pKernelMIGGpuInstance->swizzId,
              "NOT IMPLEMENTED",
              pKernelMIGGpuInstance->resourceAllocation.gpcCount);
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "OBJGR Count",
              "OBJCE Count",
              "NVDEC Count");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);

    NV_PRINTF(LEVEL_INFO, "| %18d | %18d | %18d  |\n",
              grCount,
              ceCount,
              decCount);
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "NVENC Count",
              "NVJPG Count",
              "NVOFA Count");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18d | %18d | %18d  |\n",
              encCount,
              jpgCount,
              ofaCount);
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "VEID Offset",
              "VEID Count",
              "VEID-GR Map");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18d | %18d | %18llx  |\n",
              pKernelMIGGpuInstance->resourceAllocation.veidOffset,
              pKernelMIGGpuInstance->resourceAllocation.veidCount,
              DRF_MASK64(pKernelMIGGpuInstance->resourceAllocation.veidCount : 0) << pKernelMIGGpuInstance->resourceAllocation.veidOffset);
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %29s | %29s |\n",
              "Partitionable",
              "Partitionable");
    NV_PRINTF(LEVEL_INFO, "| %29s | %29s |\n",
              "Memory Start Addr",
              "Memory End Addr");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %29llx | %29llx |\n",
              partitionableMemoryRange.lo,
              partitionableMemoryRange.hi);
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "Local Instance",
              "Local Instance",
              "Local Instance");
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "Memory Start Addr",
              "Memory End Addr",
              "Size in Bytes");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18llx | %18llx | %18llx  |\n",
              pKernelMIGGpuInstance->memRange.lo,
              pKernelMIGGpuInstance->memRange.hi,
              rangeLength(pKernelMIGGpuInstance->memRange));
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "Local Instance",
              "Local Instance",
              "Local Instance");
    NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n",
              "Start VMMU Seg.",
              "End VMMU Seg.",
              "Size in VMMU Seg.");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);

    status = kmemsysGetMIGGPUInstanceMemConfigFromSwizzId(pGpu, pKernelMemorySystem,
                                                          pKernelMIGGpuInstance->swizzId,
                                                          &pGPUInstanceMemConfig);
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        // Guest does not populate VMMU segment details.
        NV_ASSERT_OR_RETURN_VOID(IS_VIRTUAL(pGpu));
        NV_PRINTF(LEVEL_INFO, "| %18s | %18s | %18s  |\n", "N/A", "N/A", "N/A");
    }
    else
    {
        NV_ASSERT_OR_RETURN_VOID(status == NV_OK);
        NV_PRINTF(LEVEL_INFO, "| %18llx | %18llx | %18llx  |\n",
                  pGPUInstanceMemConfig->startingVmmuSegment,
                  (pGPUInstanceMemConfig->startingVmmuSegment +
                   pGPUInstanceMemConfig->memSizeInVmmuSegment) - 1,
                  pGPUInstanceMemConfig->memSizeInVmmuSegment);
    }
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
#undef PADDING_STR
#endif // NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
}

/*!
 * @brief   Function to set GPU instance information representing provided swizzId.
 */
NV_STATUS
kmigmgrSetGPUInstanceInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvU8 *pUuid,
    KMIGMGR_CREATE_GPU_INSTANCE_PARAMS params
)
{
    NvU32 i;
    NvHandle hMemory = NV01_NULL_OBJECT;
    NV_RANGE addrRange = NV_RANGE_EMPTY;
    NV_STATUS rmStatus = NV_OK;
    Heap *pMemoryPartitionHeap = NULL;
    NvU32 partitionFlag = (params.type == KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_REQUEST)
        ? params.inst.request.partitionFlag
        : params.inst.restore.pGPUInstanceSave->giInfo.partitionFlags;

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    for (i = 0; i < KMIGMGR_MAX_GPU_INSTANCES; ++i)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = &pKernelMIGManager->kernelMIGGpuInstance[i];

        // Find first invalid GPU instance and use it to save GPU instance data
        if (!pKernelMIGGpuInstance->bValid)
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            rmStatus = memmgrAllocMIGGPUInstanceMemory_HAL(pGpu, pMemoryManager, swizzId,
                                                           &hMemory, &addrRange,
                                                           &pMemoryPartitionHeap);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, rmStatus == NV_OK, rmStatus);

            // Mark GPU instance as valid as we use GPU instance Invalidation for cleanup
            pKernelMIGGpuInstance->bValid = NV_TRUE;
            pKernelMIGGpuInstance->swizzId = swizzId;
            pKernelMIGGpuInstance->hMemory = hMemory;
            pKernelMIGGpuInstance->memRange = addrRange;
            pKernelMIGGpuInstance->pMemoryPartitionHeap = pMemoryPartitionHeap;
            pKernelMIGGpuInstance->partitionFlag = partitionFlag;
            portMemCopy(pKernelMIGGpuInstance->uuid.uuid, sizeof(pKernelMIGGpuInstance->uuid.uuid),
                        pUuid, NVC637_UUID_LEN);

            //
            // Offloading of VGPU to GSP requires that the memRange in KERNEL_MIG_GPU_INSTANCE
            // be populated, as the plugin will query only within GSP for GPU INSTANCE information.
            // CPU-RM is the entity which actually calculates and allocates memory, so with
            // VGPU offloaded, GSP-RM must be updated with the memRange info.
            //
            if (IS_GSP_CLIENT(pGpu) && !IS_VIRTUAL(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
            {
                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
                NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS memParams;

                memParams.swizzId = pKernelMIGGpuInstance->swizzId;
                memParams.memAddrRange.lo = pKernelMIGGpuInstance->memRange.lo;
                memParams.memAddrRange.hi = pKernelMIGGpuInstance->memRange.hi;
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    pRmApi->Control(pRmApi,
                                    pGpu->hInternalClient,
                                    pGpu->hInternalSubdevice,
                                    NV2080_CTRL_CMD_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE,
                                    &memParams,
                                    sizeof(memParams)));
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrGetProfileByPartitionFlag(pGpu, pKernelMIGManager, partitionFlag, &pKernelMIGGpuInstance->pProfile));

            // Allocate RsShared for the GPU instance
            NV_ASSERT_OK_OR_RETURN(serverAllocShare(&g_resServ, classInfo(RsShared),
                                                    &pKernelMIGGpuInstance->pShare));

            // Get resources associated with this swizzId
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrSwizzIdToResourceAllocation(pGpu, pKernelMIGManager, swizzId, params,
                                                   pKernelMIGGpuInstance,
                                                   &pKernelMIGGpuInstance->resourceAllocation));

            pKernelMIGGpuInstance->resourceAllocation.gfxGpcCount = pKernelMIGGpuInstance->pProfile->gfxGpcCount;

            // Set assigned engines as in use
            NV_ASSERT_OK_OR_RETURN(
                kmigmgrSetEnginesInUse(pGpu, pKernelMIGManager, &pKernelMIGGpuInstance->resourceAllocation.engines));

            // Update engine tracking bitmasks for CI management later
            bitVectorClrAll(&pKernelMIGGpuInstance->exclusiveEngMask);
            bitVectorClrAll(&pKernelMIGGpuInstance->sharedEngMask);

            // Print GPU instance info for debug
            NV_PRINTF(LEVEL_INFO, "CREATING GPU instance\n");
            kmigmgrPrintGPUInstanceInfo(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

            break;
        }
    }

    NV_ASSERT_OR_RETURN(i < KMIGMGR_MAX_GPU_INSTANCES, NV_ERR_INSUFFICIENT_RESOURCES);
    return rmStatus;
}

/*!
 * @brief   Function to get GPU instance information representing provided swizzId.
 */
NV_STATUS
kmigmgrGetGPUInstanceInfo_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    KERNEL_MIG_GPU_INSTANCE **ppKernelMIGGpuInstance
)
{
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        if (pKernelMIGGPUInstance->swizzId == swizzId)
        {
            *ppKernelMIGGpuInstance = pKernelMIGGPUInstance;
            return NV_OK;
        }
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();

    return NV_ERR_INVALID_ARGUMENT;
}

/*!
 * @brief   Function to convert local RM_ENGINE_TYPE to global
 *          RM_ENGINE_TYPE for partitionable engines
 *          Currently It support GR, CE, NVDEC, NVENC, NVJPG
 */
NV_STATUS
kmigmgrGetLocalToGlobalEngineType_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    MIG_INSTANCE_REF ref,
    RM_ENGINE_TYPE localEngType,
    RM_ENGINE_TYPE *pGlobalEngType
)
{
    NV_ASSERT_OR_RETURN(kmigmgrIsMIGReferenceValid(&ref), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_VALID(localEngType),
                        NV_ERR_INVALID_ARGUMENT);

    if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, localEngType))
    {
        //
        // Return same engineId as local if called for non-partitioned
        // 2080type engines like host engines, PMU SEC etc.
        //
        *pGlobalEngType = localEngType;
        return NV_OK;
    }

    if (ref.pMIGComputeInstance != NULL)
    {
        // Replace the CI-local input index with GI-local
        if (kmigmgrEngineTypeXlate(&ref.pMIGComputeInstance->resourceAllocation.localEngines, localEngType,
                                   &ref.pMIGComputeInstance->resourceAllocation.engines, &localEngType) != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Compute instance Local Engine type 0x%x is not allocated to Compute instance\n",
                      localEngType);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Replace the GI-local input index with global
    if (kmigmgrEngineTypeXlate(&ref.pKernelMIGGpuInstance->resourceAllocation.localEngines, localEngType,
                               &ref.pKernelMIGGpuInstance->resourceAllocation.engines, &localEngType) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU instance Local Engine type 0x%x is not allocated to GPU instance\n",
                  localEngType);
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pGlobalEngType = localEngType;
    return NV_OK;
}

/*!
 * @brief   Function to convert global RM_ENGINE_TYPE to local
 *          RM_ENGINE_TYPE for partitionable engines
 *          Currently it supports GR, CE, NVDEC, NVENC, NVJPG
 */
NV_STATUS
kmigmgrGetGlobalToLocalEngineType_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    MIG_INSTANCE_REF ref,
    RM_ENGINE_TYPE globalEngType,
    RM_ENGINE_TYPE *pLocalEngType
)
{
    NV_ASSERT_OR_RETURN(kmigmgrIsMIGReferenceValid(&ref), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_VALID(globalEngType),
                        NV_ERR_INVALID_ARGUMENT);

    if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, globalEngType))
    {
        //
        // Return same engineId as global if called for non-partitioned
        // rm engine types like host engines, PMU SEC etc.
        //
        if (pLocalEngType != NULL)
        {
            *pLocalEngType = globalEngType;
        }
        return NV_OK;
    }

    // Replace the global input index with GI-local
    if (kmigmgrEngineTypeXlate(&ref.pKernelMIGGpuInstance->resourceAllocation.engines, globalEngType,
                               &ref.pKernelMIGGpuInstance->resourceAllocation.localEngines, &globalEngType) != NV_OK)
    {
        if (pLocalEngType != NULL)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Global Engine type 0x%x is not allocated to GPU instance\n",
                      globalEngType);
        }
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (ref.pMIGComputeInstance != NULL)
    {
        // Replace the GI-local input index with CI-local
        if (kmigmgrEngineTypeXlate(&ref.pMIGComputeInstance->resourceAllocation.engines, globalEngType,
                                   &ref.pMIGComputeInstance->resourceAllocation.localEngines, &globalEngType) != NV_OK)
        {
            if (pLocalEngType != NULL)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GPU instance Local Engine type 0x%x is not allocated to compute instance\n",
                          globalEngType);
            }
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (pLocalEngType != NULL)
    {
        *pLocalEngType = globalEngType;
    }
    return NV_OK;
}

/*!
 * @brief   Function to retrieve list of engine types belonging to this
 *          GPU instance. When MIG is enabled, GRCEs are filtered from the engine
 *          list, as well as any local GR engine indices outside of the range
 *          allocated to this GPU instance. When MIG is disabled, all non-legacy GR
 *          engines are filtered from the enginelist, but no CEs are filtered.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   pSubdevice
 * @param[OUT]  pEngineTypes       Engine type list
 * @param[OUT]  pEngineCount       Engine type count
 *
 * @return NV_STATUS
 *         NV_OK on success
 *         NV_ERR_INVALID_ARGUMENT if invalid subdevice
 *         NV_ERR_INVALID_STATE if subdevice is not partitioned
 */
NV_STATUS
kmigmgrFilterEngineList_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Subdevice *pSubdevice,
    RM_ENGINE_TYPE *pEngineTypes,
    NvU32 *pEngineCount
)
{
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    NvU32 i;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvBool bDeviceUsingDeviceProfiling = kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice);

    *pEngineCount = 0;
    for (i = 0; i < pGpu->engineDB.size; ++i)
    {
        RM_ENGINE_TYPE rmEngineType = pGpu->engineDB.pType[i];
        RM_ENGINE_TYPE newEngineType = rmEngineType;
        NvBool bAddEngine = NV_TRUE;

        if (!bDeviceUsingDeviceProfiling)
        {
            if (bMIGInUse)
            {
                MIG_INSTANCE_REF ref;
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

                if (kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
                {
                    // Override the engine type with the local engine idx
                    NV_ASSERT_OK(kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                                   rmEngineType,
                                                                   &newEngineType));
                }
                else
                {
                    bAddEngine = NV_FALSE;
                }
            }
            else if (RM_ENGINE_TYPE_IS_GR(rmEngineType) &&
                    (RM_ENGINE_TYPE_GR_IDX(rmEngineType) != 0))
            {
                bAddEngine = NV_FALSE;
            }
        }

        if (bAddEngine)
        {
            pEngineTypes[(*pEngineCount)++] = newEngineType;
        }
    }

    return NV_OK;
}

/**
 * @brief Removes all engines which are not in this client's GPU instance from the
 *        partnerlist.
 *
 * @param[IN]      pGpu
 * @param[IN]      pKernelMIGManager
 * @param[IN]      pSubdevice
 * @param[IN/OUT]  pPartnerListParams   Client Partner list params
 *
 * @return NV_STATUS
 *         NV_OK on success or MIG disabled
 *         NV_ERR_INVALID_ARGUMENT on bad pParams
 */
NV_STATUS
kmigmgrFilterEnginePartnerList_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams
)
{
    NvU32 i, j;
    MIG_INSTANCE_REF ref;

    NV_ASSERT_OR_RETURN(NULL != pPartnerListParams, NV_ERR_INVALID_ARGUMENT);

    // MIG disabled, nothing to do
    if (!IS_MIG_IN_USE(pGpu))
    {
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(
        kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, GPU_RES_GET_DEVICE(pSubdevice), &ref));

    for (i = 0; i < pPartnerListParams->numPartners; ++i)
    {
        RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(pPartnerListParams->partnerList[i]);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
        {
            // Filter this entry from the partner list
            for (j = i; j < pPartnerListParams->numPartners - 1; ++j)
            {
                pPartnerListParams->partnerList[j] = pPartnerListParams->partnerList[j + 1];
            }

            pPartnerListParams->numPartners--;

            // Break early to prevent underflow of i
            if (0 == pPartnerListParams->numPartners)
            {
                break;
            }

            i--;
        }
        else
        {
            // Convert rmEngineId to MIG local Ids and then to NV2080 Engine type
            RM_ENGINE_TYPE localRmEngineType;
            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                            rmEngineType,
                                            &localRmEngineType));
            NV_PRINTF(LEVEL_INFO, "Overriding global engine type 0x%x to local engine type 0x%x (0x%x) due to MIG\n",
                rmEngineType, localRmEngineType, localRmEngineType);
            pPartnerListParams->partnerList[i] = gpuGetNv2080EngineType(localRmEngineType);
        }
    }

    return NV_OK;
}

/*!
 * @brief   Finds a GPU Instance profile matching the input request flag
 */
NV_STATUS
kmigmgrGetProfileByPartitionFlag_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 partitionFlag,
    const NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO **ppProfile
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;

    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pProfiles != NULL, NV_ERR_INVALID_STATE);

    for (i = 0; i < pStaticInfo->pProfiles->count; ++i)
    {
        if (pStaticInfo->pProfiles->table[i].partitionFlag == partitionFlag)
        {
            *ppProfile = &pStaticInfo->pProfiles->table[i];
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_STATE;
}

/*
 * @brief Determine illegal swizzIds based on global swizzId mask
 */
NV_STATUS
kmigmgrGetInvalidSwizzIdMask_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvU64 *pUnsupportedSwizzIdMask
)
{
    NvU64 i;
    NvU64 gpuSlice[KGRMGR_MAX_GR] =
    {
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(7)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(8)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(4) | NVBIT64(9)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(4) | NVBIT64(10)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(5) | NVBIT64(11)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(5) | NVBIT64(12)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(6) | NVBIT64(13)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(6) | NVBIT64(14))
    };

    NV_ASSERT_OR_RETURN(NULL != pUnsupportedSwizzIdMask, NV_ERR_INVALID_ARGUMENT);

    // All bits corresponding to nonexistent swizzids are invalid
    *pUnsupportedSwizzIdMask = DRF_SHIFTMASK64(63:KMIGMGR_MAX_GPU_SWIZZID);

    for (i = 0; i < KGRMGR_MAX_GR; ++i)
    {
        if (0 != (gpuSlice[i] & NVBIT64(swizzId)))
        {
            *pUnsupportedSwizzIdMask |= gpuSlice[i];
        }
    }

    return NV_OK;
}

/*!
 * @brief Processes request to update partitioning mode to the given value.
 */
NV_STATUS
kmigmgrSetPartitioningMode_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS params;
    KernelCcu *pKccu = GPU_GET_KERNEL_CCU(pGpu);

    portMemSet(&params, 0x0, sizeof(params));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_GPU_GET_SMC_MODE,
                        &params,
                        sizeof(params)));

    // Should never have reached this far
    NV_ASSERT_OR_RETURN(params.smcMode != NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_UNSUPPORTED,
                        NV_ERR_INVALID_STATE);

    //
    // If pending state, do not update mode in response to request. Mode will be
    // updated on next GPU reset.
    //
    if ((params.smcMode == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLE_PENDING) ||
        (params.smcMode == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLE_PENDING))
    {
        return NV_OK;
    }

    pKernelMIGManager->bMIGEnabled = (params.smcMode == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLED);

    gpumgrCacheSetMIGEnabled(pGpu, pKernelMIGManager->bMIGEnabled);

    // MIG Mode might not have been enabled yet, so load static info if enabled
    if (IS_MIG_ENABLED(pGpu))
    {
        // Initialize static info derived from physical RM
        NV_ASSERT_OK_OR_RETURN(kmigmgrLoadStaticInfo_HAL(pGpu, pKernelMIGManager));

        //
        // Populate static GPU instance memory config which will be used to manage
        // GPU instance memory
        //
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        NV_ASSERT_OK_OR_RETURN(kmemsysPopulateMIGGPUInstanceMemConfig_HAL(pGpu, pKernelMemorySystem));

        NV_ASSERT_OK(gpuDisableAccounting(pGpu, NV_TRUE));
    }

    kbusUpdateRusdStatistics(pGpu);

    if (pKccu)
    {
        kccuMigShrBufHandler_HAL(pGpu, pKccu, pKernelMIGManager->bMIGEnabled);
    }
    return NV_OK;
}

/**
 * @brief   Function to get reference of gpu / compute instance which
 * contains the given engine. If no instances are found, an error is returned.
 */
NV_STATUS
kmigmgrGetMIGReferenceFromEngineType_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    RM_ENGINE_TYPE rmEngineType,
    MIG_INSTANCE_REF *pRef
)
{
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    NvU32 CIIdx;

    NV_ASSERT_OR_RETURN(pRef != NULL, NV_ERR_INVALID_ARGUMENT);
    // Default to non-attributed channel
    *pRef = kmigmgrMakeNoMIGReference();

    // Bail out early if there are no instances to attribute to
    if (!IS_MIG_IN_USE(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    //
    // if this happens to be an RM internal channel not bound to an engine,
    // attribute it to no instance
    //
    if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType))
        return NV_ERR_INVALID_ARGUMENT;

    // Engine is not partitionable, attribute to no instance
    if (!kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType))
        return NV_ERR_INVALID_ARGUMENT;

    pKernelMIGGPUInstance = NULL;
    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        if (kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType,
                                      kmigmgrMakeGIReference(pKernelMIGGPUInstance)))
        {
            break;
        }
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();

    // Engine was partitionable, but not in any of our gpu instance.
    if ((pKernelMIGGPUInstance == NULL) || !pKernelMIGGPUInstance->bValid)
        return NV_ERR_INVALID_STATE;

    *pRef = kmigmgrMakeGIReference(pKernelMIGGPUInstance);

    // Attempt to find a compute instance which contains this engine
    for (CIIdx = 0;
         CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGPUInstance->MIGComputeInstance);
         ++CIIdx)
    {
        pMIGComputeInstance = &pKernelMIGGPUInstance->MIGComputeInstance[CIIdx];

        if (!pMIGComputeInstance->bValid)
            continue;

        if (kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType,
                                      kmigmgrMakeCIReference(pKernelMIGGPUInstance, pMIGComputeInstance)))
        {
            break;
        }
    }

    if (CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGPUInstance->MIGComputeInstance))
        *pRef = kmigmgrMakeCIReference(pKernelMIGGPUInstance, pMIGComputeInstance);

    return NV_OK;
}

/*!
 * @brief Check if we are running on a reduced config GPU then set the corresponding flag
 */
void
kmigmgrDetectReducedConfig_KERNEL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;

    NV_ASSERT_OR_RETURN_VOID(pStaticInfo != NULL);

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; ++i)
    {
        // Reduced config A100 does not support 1/8 compute size
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH)
        {
            return;
        }
    }

    pKernelMIGManager->bIsA100ReducedConfig = NV_TRUE;
}

/*!
 * @brief   Get the CE in GI that can be used for scrubbing
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   pDevice            Device subscribed to GI
 * @param[OUT]  pCeInst            Scrubber CE
 */
NV_STATUS
kmigmgrGetGPUInstanceScrubberCe_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    Device *pDevice,
    NvU32 *pCeInst
)
{
    MIG_INSTANCE_REF ref;
    ENGTYPE_BIT_VECTOR ces;
    RM_ENGINE_TYPE rmEngineType;
    *pCeInst = RM_ENGINE_TYPE_NULL;

    NV_ASSERT_OK_OR_RETURN(
        kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

    bitVectorClrAll(&ces);
    bitVectorSetRange(&ces, RM_ENGINE_RANGE_COPY());
    bitVectorAnd(&ces, &ces, &ref.pKernelMIGGpuInstance->resourceAllocation.engines);

    NV_ASSERT_OR_RETURN(!bitVectorTestAllCleared(&ces), NV_ERR_INSUFFICIENT_RESOURCES);

    // Pick the first non-GRCE in the instance
    FOR_EACH_IN_BITVECTOR(&ces, rmEngineType)
    {
        if (!ceIsCeGrce(pGpu, rmEngineType))
        {
            *pCeInst = RM_ENGINE_TYPE_COPY_IDX(rmEngineType);
            break;
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    // If can't find any non-GRCE, then just pick the first CE
    if (*pCeInst == RM_ENGINE_TYPE_NULL)
    {
        *pCeInst = RM_ENGINE_TYPE_COPY_IDX(bitVectorCountTrailingZeros(&ces));
    }

    return NV_OK;
}

/*!
 * @brief   Copy gpu instance type cache to user provided params for
 *          DESCRIBE_PARTITIONS
 */
NV_STATUS
kmigmgrDescribeGPUInstances_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;
    NvU32 entryCount;

    if ((pStaticInfo == NULL) || (pStaticInfo->pProfiles == NULL))
        return NV_ERR_NOT_SUPPORTED;

    entryCount = 0;
    for (i = 0; i < pStaticInfo->pProfiles->count; ++i)
    {
        if (IS_VIRTUAL(pGpu))
        {
            VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
            NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);
            pParams->partitionDescs[entryCount].memorySize = pVSI->gpuPartitionInfo.memSize;
        }
        else
        {
            KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
            NV_RANGE addrRange = NV_RANGE_EMPTY;
            NvU32 swizzId;
            NvU32 memorySize = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE,
                                       pStaticInfo->pProfiles->table[i].partitionFlag);

            // Retrieve a valid id for this flag combination
            switch (memorySize)
            {
                case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
                    swizzId = 0;
                    break;
                case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF:
                    swizzId = 1;
                    break;
                case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER:
                    swizzId = 3;
                    break;
                case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH:
                    swizzId = 7;
                    break;
                default:
                    NV_ASSERT(0);
                    continue;
            }

            NV_ASSERT_OK(kmemsysGetMIGGPUInstanceMemInfo(pGpu, pKernelMemorySystem, swizzId, &addrRange));
            pParams->partitionDescs[entryCount].memorySize = rangeLength(addrRange);
        }

        pParams->partitionDescs[entryCount].partitionFlag   = pStaticInfo->pProfiles->table[i].partitionFlag;
        pParams->partitionDescs[entryCount].grCount         = pStaticInfo->pProfiles->table[i].grCount;
        pParams->partitionDescs[entryCount].gfxGrCount      = pStaticInfo->pProfiles->table[i].gfxGrCount;
        pParams->partitionDescs[entryCount].gpcCount        = pStaticInfo->pProfiles->table[i].gpcCount;
        pParams->partitionDescs[entryCount].gfxGpcCount     = pStaticInfo->pProfiles->table[i].gfxGpcCount;
        pParams->partitionDescs[entryCount].virtualGpcCount = pStaticInfo->pProfiles->table[i].virtualGpcCount;
        pParams->partitionDescs[entryCount].veidCount       = pStaticInfo->pProfiles->table[i].veidCount;
        pParams->partitionDescs[entryCount].smCount         = pStaticInfo->pProfiles->table[i].smCount;
        pParams->partitionDescs[entryCount].ceCount         = pStaticInfo->pProfiles->table[i].ceCount;
        pParams->partitionDescs[entryCount].nvEncCount      = pStaticInfo->pProfiles->table[i].nvEncCount;
        pParams->partitionDescs[entryCount].nvDecCount      = pStaticInfo->pProfiles->table[i].nvDecCount;
        pParams->partitionDescs[entryCount].nvJpgCount      = pStaticInfo->pProfiles->table[i].nvJpgCount;
        pParams->partitionDescs[entryCount].nvOfaCount      = pStaticInfo->pProfiles->table[i].nvOfaCount;

        entryCount++;
    }
    pParams->descCount = pStaticInfo->pProfiles->count;

    return NV_OK;
}

/*!
 * @brief   Saves MIG compute instance topology in provided structure
 */
NV_STATUS
kmigmgrSaveComputeInstances_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSaves
)
{
    NvU32 CIIdx;
    NvU32 ciCount = 0;

    // Sanity checks
    NV_ASSERT_OR_RETURN((pKernelMIGGpuInstance != NULL) && (pComputeInstanceSaves != NULL),
                        NV_ERR_INVALID_ARGUMENT);

    for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++CIIdx)
    {
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];
        GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave = &pComputeInstanceSaves[ciCount];
        NvU32 gpcIdx;

        // Skip invalid compute instances
        if (!pMIGComputeInstance->bValid)
            continue;

        portMemSet(pComputeInstanceSave, 0, sizeof(*pComputeInstanceSave));
        pComputeInstanceSave->bValid = NV_TRUE;
        pComputeInstanceSave->ciInfo.sharedEngFlags = pMIGComputeInstance->sharedEngFlag;
        pComputeInstanceSave->id = CIIdx;
        pComputeInstanceSave->pOsRmCaps = pMIGComputeInstance->pOsRmCaps;
        bitVectorToRaw(&pMIGComputeInstance->resourceAllocation.engines,
                       &pComputeInstanceSave->ciInfo.enginesMask,
                       sizeof(pComputeInstanceSave->ciInfo.enginesMask));
        if (IS_GSP_CLIENT(pGpu))
        {
            for (gpcIdx = 0; gpcIdx < pMIGComputeInstance->resourceAllocation.gpcCount; ++gpcIdx)
            {
                 pComputeInstanceSave->ciInfo.gpcMask |=
                     NVBIT32(pMIGComputeInstance->resourceAllocation.gpcIds[gpcIdx]);
            }
        }
        else
        {
            pComputeInstanceSave->ciInfo.gpcMask = DRF_MASK(pMIGComputeInstance->resourceAllocation.gpcCount - 1 : 0);
        }

        pComputeInstanceSave->ciInfo.gfxGpcCount = pMIGComputeInstance->resourceAllocation.gfxGpcCount;
        pComputeInstanceSave->ciInfo.veidOffset = pMIGComputeInstance->resourceAllocation.veidOffset;
        pComputeInstanceSave->ciInfo.veidCount = pMIGComputeInstance->resourceAllocation.veidCount;
        pComputeInstanceSave->ciInfo.smCount = pMIGComputeInstance->resourceAllocation.smCount;
        pComputeInstanceSave->ciInfo.spanStart = pMIGComputeInstance->spanStart;
        pComputeInstanceSave->ciInfo.computeSize = pMIGComputeInstance->computeSize;

        portMemCopy(pComputeInstanceSave->ciInfo.uuid, sizeof(pComputeInstanceSave->ciInfo.uuid),
                    pMIGComputeInstance->uuid.uuid, sizeof(pMIGComputeInstance->uuid.uuid));

        ++ciCount;
    }

    return NV_OK;
}

/*!
 * @brief   Function to get SwizzId to allowed GrIdx, physical GPC_IDs,
 *          physical CE_IDs and VEIDs in a GPU instance
 *
 * @param[IN]   swizzId              SwizzId used by the GPU instance
 * @param[OUT]  pResourceAllocation  Structure containing engine configs for a
 *                                   GPU instance. This contains engineCount and
 *                                   engine Ids.
 */
NV_STATUS
kmigmgrSwizzIdToResourceAllocation_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    KMIGMGR_CREATE_GPU_INSTANCE_PARAMS params,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    MIG_RESOURCE_ALLOCATION *pResourceAllocation
)
{
    NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO info;
    NvU32 tempGpcMask;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, swizzId < KMIGMGR_MAX_GPU_SWIZZID, NV_ERR_INVALID_ARGUMENT);

    if (params.type == KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_REQUEST)
    {
        NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS export;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        portMemSet(&export, 0, sizeof(export));
        export.swizzId = swizzId;

        // Retrieve the info of the gpu instance GSP just created
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE,
                            &export,
                            sizeof(export)));
        info = export.info;
    }
    else
    {
        info = params.inst.restore.pGPUInstanceSave->giInfo;
    }

    pResourceAllocation->gpcCount = 0;
    tempGpcMask = info.gpcMask;
    while (tempGpcMask != 0x0)
    {
        NvU32 gpcIdx = portUtilCountTrailingZeros32(tempGpcMask);
        pResourceAllocation->gpcIds[(pResourceAllocation->gpcCount)++] = gpcIdx;
        tempGpcMask &= ~(NVBIT32(gpcIdx));
    }

    pResourceAllocation->veidCount = info.veidCount;
    pResourceAllocation->veidOffset = info.veidOffset;
    pResourceAllocation->virtualGpcCount = info.virtualGpcCount;

    // Use profile SM count for filling the resource allocation
    pResourceAllocation->smCount = pKernelMIGGpuInstance->pProfile->smCount;

    bitVectorFromRaw(&pResourceAllocation->engines, info.enginesMask, sizeof(info.enginesMask));

    // Cache the local engine mask for this instance
    kmigmgrGetLocalEngineMask(pGpu, &pResourceAllocation->engines, &pResourceAllocation->localEngines);

    return NV_OK;
}

// Create client and subdevice handles to make calls into this compute instance
NV_STATUS
kmigmgrAllocComputeInstanceHandles_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle hGPUInstanceSubscription = NV01_NULL_OBJECT;
    NvHandle hComputeInstanceSubscription = NV01_NULL_OBJECT;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClient, &hDevice, &hSubdevice));

    {
        NVC637_ALLOCATION_PARAMETERS params;
        portMemSet(&params, 0, sizeof(params));
        params.swizzId = pKernelMIGGpuInstance->swizzId;
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Alloc(pRmApi, hClient, hSubdevice, &hGPUInstanceSubscription, AMPERE_SMC_PARTITION_REF, &params, sizeof(params)),
            failed);
    }

    {
        NVC638_ALLOCATION_PARAMETERS params;
        portMemSet(&params, 0, sizeof(params));
        params.execPartitionId = pMIGComputeInstance->id;
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Alloc(pRmApi, hClient, hGPUInstanceSubscription, &hComputeInstanceSubscription, AMPERE_SMC_EXEC_PARTITION_REF, &params, sizeof(params)),
            failed);
    }

    pMIGComputeInstance->instanceHandles.hClient = hClient;
    pMIGComputeInstance->instanceHandles.hSubdevice = hSubdevice;
    pMIGComputeInstance->instanceHandles.hSubscription = hComputeInstanceSubscription;

    return NV_OK;

failed:
    pRmApi->Free(pRmApi, hClient, hClient);
    return status;
}

/*!
 * @brief Get Compute Instance UUID
 *
 * @param[IN]  pGpu
 * @param[IN]  pKernelMIGManager
 * @param[IN]  swizzId               GPU instance swizz ID
 * @param[IN]  globalGrIdx           physical syspipe ID
 * @param[OUT] pUuid                 Compute Instance UUID
 */
NV_STATUS
kmigmgrGenerateComputeInstanceUuid_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvU32 globalGrIdx,
    NvUuid *pUuid
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NvU16 chipId = gpuGetChipIdFromPmcBoot42(pGpu->chipId1);
    NvU64 gid;

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(&gid, sizeof(gid), pVSI->gidInfo.data, sizeof(gid));

    //
    // We can't use PDI for the vGPU use-case. We need a unique ID per VM.
    // So, for vGPU, read the first 64-bits from the host generated UUID.
    // These bits represent a timestamp, which should be unique per VM.
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        nvGenerateSmcUuid(chipId, gid, swizzId, globalGrIdx, pUuid));

    return NV_OK;
}

/*!
 * @brief Get GPU Instance UUID
 *
 * @param[IN]  pGpu
 * @param[IN]  pKernelMIGManager
 * @param[IN]  swizzId               GPU instance swizz ID
 * @param[OUT] pUuid                 GPU Instance UUID
 */
NV_STATUS
kmigmgrGenerateGPUInstanceUuid_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvUuid *pUuid
)
{
    return kmigmgrGenerateComputeInstanceUuid_HAL(
                pGpu, pKernelMIGManager, swizzId, GR_INDEX_INVALID, pUuid);
}

/*!
 * @brief   create compute instances
 *
 * @param[IN]  pGpu
 * @param[IN]  pKernelMIGManager
 * @param[IN]  pKernelMIGGpuInstance
 * @param[IN]  bQuery                If NV_TRUE, don't save created instances
 * @param[IN]  params                List of requested compute instance to create
 * @param[OUT] pCIIDs                IDs of created instances
 * @param[IN]  bCreateCap            Flag stating if MIG CI capabilities needs to be created
 */
NV_STATUS
kmigmgrCreateComputeInstances_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvBool bQuery,
    KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS params,
    NvU32 *pCIIDs,
    NvBool bCreateCap
)
{
    NV_STATUS status = NV_OK;
    NvU32 count;
    ENGTYPE_BIT_VECTOR shadowExclusiveEngMask;
    ENGTYPE_BIT_VECTOR shadowSharedEngMask;
    MIG_COMPUTE_INSTANCE *pComputeInstanceInfo;
    NvU32 CIIdx;
    NvU32 freeSlots;
    NvU32 createdInstances;
    NvU32 inUseGpcCount;
    NvU32 remainingGpcCount;
    NvU32 i;
    NvU64 shadowCTSInUseMask;
    NvU64 shadowVeidInUseMask;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi = NULL;
    NvBool bIsCTSRequired = kmigmgrIsCTSAlignmentRequired_HAL(pGpu, pKernelMIGManager);

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);

    count = (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST)
            ? params.inst.request.count
            : 1;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, count != 0, NV_ERR_INVALID_ARGUMENT);

    pComputeInstanceInfo = portMemAllocNonPaged(sizeof(*pComputeInstanceInfo) *
                                                KMIGMGR_MAX_COMPUTE_INSTANCES);
    NV_CHECK_OR_RETURN(LEVEL_NOTICE, pComputeInstanceInfo != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pComputeInstanceInfo, 0, sizeof(*pComputeInstanceInfo) *
                                        KMIGMGR_MAX_COMPUTE_INSTANCES);

    pConfigRequestPerCi = portMemAllocStackOrHeap(sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);
    NV_ASSERT_OR_ELSE(pConfigRequestPerCi != NULL, status = NV_ERR_NO_MEMORY; goto done;);

    portMemSet(pConfigRequestPerCi, 0, sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);

    // Check that there's enough open compute instance slots, and count used GPCs
    freeSlots = 0;
    inUseGpcCount = 0;
    for (CIIdx = 0;
         CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
         ++CIIdx)
    {
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];

        if (pMIGComputeInstance->bValid)
        {
            NvU32 smCount = pMIGComputeInstance->resourceAllocation.smCount;
            NvU32 gpcCount = pMIGComputeInstance->resourceAllocation.gpcCount;

            NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE ciProfile;

            NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                kmigmgrGetComputeProfileForRequest(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, smCount, gpcCount, &ciProfile),
                goto done; );

            inUseGpcCount += ciProfile.gpcCount;
        }
        else
        {
            freeSlots++;
        }
    }
    NV_CHECK_OR_ELSE(LEVEL_SILENT, freeSlots >= count,
                     status = NV_ERR_INSUFFICIENT_RESOURCES; goto done);

    //
    // Check that we have enough spare GPCs. We're going to reuse the GPU Instance
    // configuration logic later on to do the actual allocation, so for now just
    // check the count.
    //
    NV_ASSERT_OR_ELSE(pKernelMIGGpuInstance->resourceAllocation.virtualGpcCount >= inUseGpcCount,
                      status = NV_ERR_INVALID_STATE; goto done);
    remainingGpcCount = pKernelMIGGpuInstance->resourceAllocation.virtualGpcCount - inUseGpcCount;

    //
    // Cache local copies of the resource pools, we'll commit them later if we
    // have to
    //
    bitVectorCopy(&shadowExclusiveEngMask, &pKernelMIGGpuInstance->exclusiveEngMask);
    bitVectorCopy(&shadowSharedEngMask, &pKernelMIGGpuInstance->sharedEngMask);
    shadowCTSInUseMask = pKernelMIGGpuInstance->ctsIdsInUseMask;
    shadowVeidInUseMask = kgrmgrGetVeidInUseMask(pGpu, pKernelGraphicsManager);
    for (CIIdx = 0; CIIdx < count; ++CIIdx)
    {
        NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pCIProfile;
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pComputeInstanceInfo[CIIdx];
        MIG_RESOURCE_ALLOCATION *pResourceAllocation = &pMIGComputeInstance->resourceAllocation;
        NvU32 smCount =
                (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST)
                ? params.inst.request.pReqComputeInstanceInfo[CIIdx].smCount
                : params.inst.restore.pComputeInstanceSave->ciInfo.smCount;
        NvU32 gpcCount =
                (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST)
                ? params.inst.request.pReqComputeInstanceInfo[CIIdx].gpcCount
                : nvPopCount32(params.inst.restore.pComputeInstanceSave->ciInfo.gpcMask);
        pMIGComputeInstance->bValid = NV_TRUE;
        pMIGComputeInstance->sharedEngFlag =
                (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST)
                ? params.inst.request.pReqComputeInstanceInfo[CIIdx].sharedEngFlag
                : params.inst.restore.pComputeInstanceSave->ciInfo.sharedEngFlags;
        NvU32 spanStart;
        NvU32 ctsId;

        if (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST)
        {
            spanStart = KMIGMGR_SPAN_OFFSET_INVALID;
            if (FLD_TEST_REF(NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_AT_SPAN, _TRUE, params.inst.request.requestFlags))
            {
                NvU32 veidSizePerSpan;

                NV_ASSERT_OK_OR_GOTO(status,
                    kgrmgrGetVeidSizePerSpan(pGpu, pKernelGraphicsManager, &veidSizePerSpan),
                    done);

                //
                // Select spanStart from spanStart field, else calculate the spanStart using the veid offset passed in.
                // This is done specifically to accomodate legacy flows which don't have knowledge of the new spanStart field
                //
                spanStart = (params.inst.request.pReqComputeInstanceInfo[CIIdx].spanStart != 0)
                            ? params.inst.request.pReqComputeInstanceInfo[CIIdx].spanStart
                            : params.inst.request.pReqComputeInstanceInfo[CIIdx].veidStartOffset / veidSizePerSpan;
            }
        }
        else
        {
            spanStart = params.inst.restore.pComputeInstanceSave->ciInfo.spanStart;
        }

        pConfigRequestPerCi[CIIdx].veidSpanStart = spanStart;
        pCIProfile = &pConfigRequestPerCi[CIIdx].profile;
        ctsId = KMIGMGR_CTSID_INVALID;
        if (kmigmgrGetComputeProfileForRequest(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, smCount, gpcCount, pCIProfile) == NV_OK)
        {
            // CTS and Span allocation is done early to help prevent spurious requests
            if (bIsCTSRequired)
            {
                if (spanStart != KMIGMGR_SPAN_OFFSET_INVALID)
                {
                    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        kmigmgrXlateSpanStartToCTSId(pGpu, pKernelMIGManager,
                                                     pCIProfile->computeSize,
                                                     spanStart,
                                                     &ctsId),
                        done);

                    NV_CHECK_OR_ELSE(LEVEL_ERROR,
                        kmigmgrIsCTSIdAvailable(pGpu, pKernelMIGManager,
                                                pKernelMIGGpuInstance->pProfile->validCTSIdMask,
                                                shadowCTSInUseMask,
                                                ctsId),
                        status = NV_ERR_STATE_IN_USE; goto done; );
                }
                else
                {
                    // Don't know how to allocate GfxGpc in VF yet
                    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        kmigmgrGetFreeCTSId(pGpu, pKernelMIGManager,
                                            &ctsId,
                                            pKernelMIGGpuInstance->pProfile->validCTSIdMask,
                                            0x0,
                                            shadowCTSInUseMask,
                                            pCIProfile->computeSize,
                                            NV_FALSE,
                                            NV_FALSE),
                        done);
                }

                NV_CHECK_OR_ELSE(LEVEL_ERROR, ctsId < KMIGMGR_MAX_GPU_CTSID,
                    status = NV_ERR_INVALID_STATE; goto done; );

                pConfigRequestPerCi[CIIdx].veidSpanStart = kmigmgrGetSpanStartFromCTSId(pGpu, pKernelMIGManager, ctsId);
                shadowCTSInUseMask |= NVBIT64(ctsId);
            }
        }
        else
        {
            // If no CI profile was available. Populate one with bare-necessities
            pCIProfile->computeSize = KMIGMGR_COMPUTE_SIZE_INVALID;
            pCIProfile->gpcCount = gpcCount;
            pCIProfile->smCount = gpcCount * (pKernelMIGGpuInstance->pProfile->smCount / pKernelMIGGpuInstance->pProfile->gpcCount);

            // Force non-profile requests to go through VEID allocator
            pConfigRequestPerCi[CIIdx].veidSpanStart = KMIGMGR_SPAN_OFFSET_INVALID;
        }

        pConfigRequestPerCi[CIIdx].ctsId = ctsId;

        // Perform VEID request checks or use the best fit allocator to find a slot
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kgrmgrCheckVeidsRequest(pGpu, pKernelGraphicsManager,
                                    &shadowVeidInUseMask,
                                    pCIProfile->veidCount,
                                    &pConfigRequestPerCi[CIIdx].veidSpanStart,
                                    pKernelMIGGpuInstance),
            done);

        // Perform checks and VEID allocation
        if (!bIsCTSRequired)
        {
            //
            // Only perform explicit GPC checks if CTS alignment isn't required. A similar case
            // is covered by CTS requirements.
            //
            if (remainingGpcCount < pCIProfile->gpcCount)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Not enough remaining GPCs (%d) for compute instance request (%d).\n",
                          remainingGpcCount, pCIProfile->gpcCount);
                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto done;
            }
            remainingGpcCount -= pCIProfile->gpcCount;
        }

        if (params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE)
        {
            ENGTYPE_BIT_VECTOR engines;
            bitVectorClrAll(&pResourceAllocation->engines);

            // Set engines requested directly in resource allocation mask
            bitVectorFromRaw(&pResourceAllocation->engines,
                             params.inst.restore.pComputeInstanceSave->ciInfo.enginesMask,
                             sizeof(params.inst.restore.pComputeInstanceSave->ciInfo.enginesMask));

            // Sanity check that all engines requested exist in the GI engine mask
            bitVectorClrAll(&engines);
            bitVectorAnd(&engines, &pResourceAllocation->engines, &pKernelMIGGpuInstance->resourceAllocation.localEngines);
            NV_CHECK_OR_ELSE(LEVEL_ERROR,
                bitVectorTestEqual(&engines, &pResourceAllocation->engines),
                status = NV_ERR_INVALID_ARGUMENT; goto done;);

            // Set Shared/Exclusive Engine Masks for GRs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_GR());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);

            // Only 1 GR can be requested per compute instance
            NV_CHECK_OR_ELSE(LEVEL_ERROR,
                (kmigmgrCountEnginesOfType(&engines, RM_ENGINE_TYPE_GR(0)) == 1),
                status = NV_ERR_INVALID_ARGUMENT; goto done;);

            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NONE) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }

            // Set Shared/Exclusive Engine Masks for CEs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_COPY());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);
            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_CE) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }

            // Set Shared/Exclusive Engine Masks for NVDECs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_NVDEC());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);
            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVDEC) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }

            // Set Shared/Exclusive Engine Masks for NVENCs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_NVENC());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);
            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVENC) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }

            // Set Shared/Exclusive Engine Masks for NVJPEGs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_NVJPEG());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);
            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVJPG) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }

            // Set Shared/Exclusive Engine Masks for OFAs restored
            bitVectorClrAll(&engines);
            bitVectorSetRange(&engines, RM_ENGINE_RANGE_OFA());
            bitVectorAnd(&engines, &engines, &pResourceAllocation->engines);
            if ((pMIGComputeInstance->sharedEngFlag & NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_OFA) != 0x0)
                bitVectorOr(&shadowSharedEngMask, &shadowSharedEngMask, &engines);
            else
            {
                ENGTYPE_BIT_VECTOR tempVector;

                // Exclusive engine mask should not intersect with the current exclusive mask
                bitVectorAnd(&tempVector, &engines, &shadowExclusiveEngMask);
                NV_CHECK_OR_ELSE(LEVEL_ERROR,
                    bitVectorTestAllCleared(&tempVector),
                    status = NV_ERR_STATE_IN_USE; goto done;);
                bitVectorOr(&shadowExclusiveEngMask, &shadowExclusiveEngMask, &engines);
            }
        }
        else
        {
            NvU32 grCount = 1;
            NvU32 ceCount = params.inst.request.pReqComputeInstanceInfo[CIIdx].ceCount;
            NvU32 decCount = params.inst.request.pReqComputeInstanceInfo[CIIdx].nvDecCount;
            NvU32 encCount = params.inst.request.pReqComputeInstanceInfo[CIIdx].nvEncCount;
            NvU32 jpgCount = params.inst.request.pReqComputeInstanceInfo[CIIdx].nvJpgCount;
            NvU32 ofaCount = params.inst.request.pReqComputeInstanceInfo[CIIdx].ofaCount;

            bitVectorClrAll(&pResourceAllocation->engines);

            // Allocate the GR engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NONE) != 0x0),
                                               RM_ENGINE_RANGE_GR(),
                                               grCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);

            // Allocate the Copy engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_CE) != 0x0),
                                               RM_ENGINE_RANGE_COPY(),
                                               ceCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);

            // Allocate the NVDEC engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVDEC) != 0x0),
                                               RM_ENGINE_RANGE_NVDEC(),
                                               decCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);

            // Allocate the NVENC engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVENC) != 0x0),
                                               RM_ENGINE_RANGE_NVENC(),
                                               encCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);

            // Allocate the NVJPG engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVJPG) != 0x0),
                                               RM_ENGINE_RANGE_NVJPEG(),
                                               jpgCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);

            // Allocate the NVOFA engines for this compute instance
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrAllocateInstanceEngines(&pKernelMIGGpuInstance->resourceAllocation.engines,
                                               ((pMIGComputeInstance->sharedEngFlag &
                                                NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_OFA) != 0x0),
                                               RM_ENGINE_RANGE_OFA(),
                                               ofaCount,
                                               &pResourceAllocation->engines,
                                               &shadowExclusiveEngMask,
                                               &shadowSharedEngMask,
                                               &pKernelMIGGpuInstance->resourceAllocation.engines), done);
        }

        // Cache local mask of engine IDs for this compute instance
        kmigmgrGetLocalEngineMask(pGpu, &pResourceAllocation->engines,
                                  &pResourceAllocation->localEngines);
    }

    // Commit the allocations to the instance
    if (!bQuery)
    {
        NvU32 swizzId = pKernelMIGGpuInstance->swizzId;
        KMIGMGR_CONFIGURE_INSTANCE_REQUEST configRequestsPerCiOrdered[KMIGMGR_MAX_COMPUTE_INSTANCES] = {0};
        NvU32 updateEngMask;
        NvU32 updateEngMaskShadow;

        // Populate configure GPU instance parameters with compute instance info
        updateEngMask = 0x0;

        for (CIIdx = 0; CIIdx < count; ++CIIdx)
        {
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pComputeInstanceInfo[CIIdx];
            MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;
            RM_ENGINE_TYPE localEngineType;

            //
            // Xlate from CI-local GR 0 to GI-local GR idx
            // We can't use kmigmgrGetLocalToGlobalEngineType because these
            // compute instances aren't committed yet
            //
            NV_ASSERT_OK(
                kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                       &pComputeResourceAllocation->engines, &localEngineType));

            updateEngMask |= NVBIT32(RM_ENGINE_TYPE_GR_IDX(localEngineType));
        }

        //
        // Reorder the entries in pConfigRequestPerCi per the GR engine assigned to each CI
        // (Sorted from lower GR to higer GR), so kmigmgrConfigureGPUInstance can configure
        // each CI with correct GR.
        //
        updateEngMaskShadow = updateEngMask;
        i = 0;
        while (updateEngMaskShadow != 0)
        {
            for (CIIdx = 0; CIIdx < count; ++CIIdx)
            {
                RM_ENGINE_TYPE localRmEngineType;
                MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pComputeInstanceInfo[CIIdx];
                MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;
                NV_ASSERT_OK(
                    kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                           &pComputeResourceAllocation->engines, &localRmEngineType));

                if (portUtilCountTrailingZeros32(updateEngMaskShadow) == RM_ENGINE_TYPE_GR_IDX(localRmEngineType))
                {
                    configRequestsPerCiOrdered[i] = pConfigRequestPerCi[CIIdx];
                    updateEngMaskShadow &= ~NVBIT32(RM_ENGINE_TYPE_GR_IDX(localRmEngineType));
                    i++;
                    break;
                }
            }
            NV_ASSERT(CIIdx < count);
        }

        // Configure the GR engines for each compute instance
        status = kmigmgrConfigureGPUInstance(pGpu, pKernelMIGManager, swizzId,
                                             configRequestsPerCiOrdered,
                                             updateEngMask);

        // Do our best to deconfigure the engines we configured so far, then bail
        if (status != NV_OK)
        {
            portMemSet(pConfigRequestPerCi, 0x0, sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);
            // Quash status. This is best-effort cleanup
            (void)kmigmgrConfigureGPUInstance(pGpu, pKernelMIGManager, swizzId,
                                              pConfigRequestPerCi,
                                              updateEngMask);

            goto done;
        }

        // Update the GI pools with the result of this allocation
        bitVectorCopy(&pKernelMIGGpuInstance->exclusiveEngMask, &shadowExclusiveEngMask);
        bitVectorCopy(&pKernelMIGGpuInstance->sharedEngMask, &shadowSharedEngMask);

        // update each compute instance gpc ids and veid info
        for (CIIdx = 0; CIIdx < count; ++CIIdx)
        {
            MIG_RESOURCE_ALLOCATION *pResourceAllocation = &pKernelMIGGpuInstance->resourceAllocation;
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pComputeInstanceInfo[CIIdx];
            MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;
            NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pCIProfile;
            RM_ENGINE_TYPE globalEngineType;
            NvU32 globalGrIdx;

            //
            // Xlate from CI-local GR 0 to global GR idx
            // We can't use kmigmgrGetLocalToGlobalEngineType because these
            // compute instances aren't committed yet
            //
            NV_ASSERT_OK(
                kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                       &pComputeResourceAllocation->engines, &globalEngineType));

            NV_ASSERT_OK(
                kmigmgrEngineTypeXlate(&pResourceAllocation->localEngines, globalEngineType,
                                       &pResourceAllocation->engines, &globalEngineType));
            globalGrIdx = RM_ENGINE_TYPE_GR_IDX(globalEngineType);
            pCIProfile = &pConfigRequestPerCi[CIIdx].profile;

            pComputeResourceAllocation->gpcCount = pCIProfile->gpcCount;
            pComputeResourceAllocation->smCount = pCIProfile->smCount;
            if (pCIProfile->computeSize != KMIGMGR_COMPUTE_SIZE_INVALID)
            {
                pComputeResourceAllocation->veidCount = pCIProfile->veidCount;
            }
            else
            {
                pComputeResourceAllocation->veidCount = (pResourceAllocation->veidCount / pResourceAllocation->gpcCount) *
                                                         pComputeResourceAllocation->virtualGpcCount;
            }

            pMIGComputeInstance->spanStart = pConfigRequestPerCi[CIIdx].veidSpanStart;
            pMIGComputeInstance->computeSize = pConfigRequestPerCi[CIIdx].profile.computeSize;

            kgrmgrGetVeidBaseForGrIdx(pGpu, pKernelGraphicsManager, globalGrIdx, &pComputeResourceAllocation->veidOffset);

            pComputeResourceAllocation->veidOffset = pComputeResourceAllocation->veidOffset - pResourceAllocation->veidOffset;
        }

        // Copy over the local cached compute instance info
        createdInstances = 0;
        for (CIIdx = 0;
             CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
             ++CIIdx)
        {
            if (pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].bValid)
                continue;

            if ((params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE) &&
                (params.inst.restore.pComputeInstanceSave->id != CIIdx))
            {
                continue;
            }

            if (FLD_TEST_REF(NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_WITH_PART_ID, _TRUE, params.inst.request.requestFlags) &&
                (pCIIDs[0] != CIIdx))
            {
                continue;
            }

            NV_ASSERT(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].id ==
                      KMIGMGR_COMPUTE_INSTANCE_ID_INVALID);

            portMemCopy(&pKernelMIGGpuInstance->MIGComputeInstance[CIIdx],
                        sizeof(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx]),
                        &pComputeInstanceInfo[createdInstances],
                        sizeof(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx]));

            pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].id = CIIdx;

            pCIIDs[createdInstances++] = CIIdx;

            if (createdInstances == count)
                break;
        }

        for (i = 0; i < createdInstances; ++i)
        {
            MIG_RESOURCE_ALLOCATION *pResourceAllocation;
            MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation;
            MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
            RM_ENGINE_TYPE globalEngineType;
            NvU32 globalGrIdx;

            //
            // As per the current design, index for the pMIGComputeInstance
            // array is same as the compute instance ID.
            //
            CIIdx = pCIIDs[i];

            pResourceAllocation = &pKernelMIGGpuInstance->resourceAllocation;

            pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];
            pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;

            NV_ASSERT_OK(
                kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                       &pComputeResourceAllocation->engines, &globalEngineType));
            NV_ASSERT_OK(
                kmigmgrEngineTypeXlate(&pResourceAllocation->localEngines, globalEngineType,
                                       &pResourceAllocation->engines, &globalEngineType));
            globalGrIdx = RM_ENGINE_TYPE_GR_IDX(globalEngineType);

            NV_ASSERT(pMIGComputeInstance->id == CIIdx);

            //
            // Register instance with the capability framework only if it explicitly
            // requested. Otherwise, we rely on the persistent state.
            //
            if (bCreateCap)
            {
                // Register compute instance with the capability framework
                NV_ASSERT_OK_OR_GOTO(status,
                    osRmCapRegisterSmcExecutionPartition(pKernelMIGGpuInstance->pOsRmCaps,
                                                         &pMIGComputeInstance->pOsRmCaps,
                                                         pMIGComputeInstance->id),
                    cleanup_created_instances);
            }

            // Populate UUID
            NV_ASSERT_OK_OR_GOTO(status,
                kmigmgrGenerateComputeInstanceUuid_HAL(pGpu, pKernelMIGManager, swizzId, globalGrIdx,
                                                       &pMIGComputeInstance->uuid),
                cleanup_created_instances);

            // Allocate RsShared for the instance
            NV_ASSERT_OK_OR_GOTO(
                status,
                serverAllocShare(&g_resServ, classInfo(RsShared),
                                 &pMIGComputeInstance->pShare),
                cleanup_created_instances);

            // Allocate subscribed handles for this instance
            NV_ASSERT_OK_OR_GOTO(status,
                kmigmgrAllocComputeInstanceHandles(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, pMIGComputeInstance),
                cleanup_created_instances);

            {
                KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, globalGrIdx);
                fecsSetRoutingInfo(pGpu,
                                   pKernelGraphics,
                                   pMIGComputeInstance->instanceHandles.hClient,
                                   pMIGComputeInstance->instanceHandles.hSubdevice,
                                   0);

                NV_ASSERT_OK_OR_GOTO(status,
                    kgraphicsCreateGoldenImageChannel(pGpu, pKernelGraphics),
                    cleanup_created_instances);
            }
            KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);
            if (pKernelCcu != NULL)
            {
                NV_ASSERT_OK(kccuInitVgpuMigSharedBuffer(pGpu, pKernelCcu, swizzId, pMIGComputeInstance->id));
            }
        }
    }

    status = NV_OK;
    goto done;

cleanup_created_instances:
    for (i = 0; i < createdInstances; ++i)
    {
        (void)kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                           pCIIDs[i], NV_FALSE);
    }

done:
    portMemFree(pComputeInstanceInfo);
    portMemFreeStackOrHeap(pConfigRequestPerCi);

    return status;
}

/*!
 * @brief   create compute instances for CPU-RM
 *
 * @param[IN]  pGpu
 * @param[IN]  pKernelMIGManager
 * @param[IN]  pKernelMIGGpuInstance
 * @param[IN]  bQuery                If NV_TRUE, don't save created instances
 * @param[IN]  params                List of requested compute instance to create
 * @param[OUT] pCIIDs                IDs of created instances
 * @param[IN]  bCreateCap            Flag stating if MIG CI capabilities needs to be created
 */
NV_STATUS
kmigmgrCreateComputeInstances_FWCLIENT
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvBool bQuery,
    KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS params,
    NvU32 *pCIIDs,
    NvBool bCreateCap
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_STATUS status = NV_OK;
    KernelGraphics *pKernelGraphics;
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    MIG_RESOURCE_ALLOCATION *pResourceAllocation;
    MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation;
    NVC637_CTRL_EXEC_PARTITIONS_EXPORTED_INFO info;
    NvU32 CIIdx = pCIIDs[0];
    NvU32 tempGpcMask;
    KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi;
    RM_ENGINE_TYPE localEngineType;
    RM_ENGINE_TYPE globalEngineType;
    NvU32 globalGrIdx;
    NvU64 shadowVeidInUseMask;

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(params.type == KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(params.inst.restore.pComputeInstanceSave != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(params.inst.restore.pComputeInstanceSave->bValid, NV_ERR_INVALID_ARGUMENT);

    // CPU-RM will always restore the CI state created by GSP-RM, so will always be commit operation
    NV_ASSERT_OR_RETURN(!bQuery, NV_ERR_INVALID_ARGUMENT);

    pMIGComputeInstance = portMemAllocNonPaged(sizeof(*pMIGComputeInstance));
    NV_CHECK_OR_RETURN(LEVEL_NOTICE, pMIGComputeInstance != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pMIGComputeInstance, 0, sizeof(*pMIGComputeInstance));

    pResourceAllocation = &pKernelMIGGpuInstance->resourceAllocation;
    pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;

    NV_ASSERT_OR_RETURN(!pMIGComputeInstance->bValid, NV_ERR_INVALID_STATE);

    pConfigRequestPerCi = portMemAllocStackOrHeap(sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);
    NV_ASSERT_OR_RETURN(pConfigRequestPerCi != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pConfigRequestPerCi, 0x0, sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);

    info = params.inst.restore.pComputeInstanceSave->ciInfo;

    if (kmigmgrIsCTSAlignmentRequired_HAL(pGpu, pKernelMIGManager))
    {

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrXlateSpanStartToCTSId(pGpu, pKernelMIGManager,
                                         info.computeSize,
                                         info.spanStart,
                                         &pConfigRequestPerCi[0].ctsId),
            done);

            NV_CHECK_OR_ELSE(LEVEL_ERROR,
                kmigmgrIsCTSIdAvailable(pGpu, pKernelMIGManager,
                                        pKernelMIGGpuInstance->pProfile->validCTSIdMask,
                                        pKernelMIGGpuInstance->ctsIdsInUseMask,
                                        pConfigRequestPerCi[0].ctsId),
                status = NV_ERR_STATE_IN_USE; goto done; );
    }
    else
    {
        pConfigRequestPerCi[0].ctsId = KMIGMGR_CTSID_INVALID;
    }

    portMemCopy(pMIGComputeInstance->uuid.uuid, sizeof(pMIGComputeInstance->uuid.uuid),
                info.uuid, sizeof(info.uuid));
    pMIGComputeInstance->sharedEngFlag = info.sharedEngFlags;

    pComputeResourceAllocation->gpcCount = 0;
    tempGpcMask = info.gpcMask;
    while (tempGpcMask != 0x0)
    {
        NvU32 gpcIdx = portUtilCountTrailingZeros32(tempGpcMask);
        pComputeResourceAllocation->gpcIds[(pComputeResourceAllocation->gpcCount)++] = gpcIdx;
        tempGpcMask &= ~(NVBIT32(gpcIdx));
    }
    pComputeResourceAllocation->gfxGpcCount = info.gfxGpcCount;
    pComputeResourceAllocation->veidCount = info.veidCount;
    pComputeResourceAllocation->veidOffset = info.veidOffset;
    pComputeResourceAllocation->smCount = info.smCount;
    pMIGComputeInstance->computeSize = info.computeSize;

    bitVectorFromRaw(&pComputeResourceAllocation->engines, info.enginesMask, sizeof(info.enginesMask));

    // Cache the local engine mask for this CI
    kmigmgrGetLocalEngineMask(pGpu, &pComputeResourceAllocation->engines, &pComputeResourceAllocation->localEngines);

    pMIGComputeInstance->bValid = NV_TRUE;
    pMIGComputeInstance->id = CIIdx;

    // Populate configure GPU instance parameters with compute instance info

    //
    // Xlate from CI-local GR 0 to GI-local GR idx
    // We can't use kmigmgrGetLocalToGlobalEngineType because these
    // compute instances aren't committed yet
    //
    NV_ASSERT_OK(
        kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                               &pComputeResourceAllocation->engines, &localEngineType));

    // Create a pseduo-profile based upon info retrieved from GSP-RM
    pConfigRequestPerCi[0].profile.computeSize = info.computeSize;
    pConfigRequestPerCi[0].profile.smCount     = pComputeResourceAllocation->smCount;
    pConfigRequestPerCi[0].profile.gpcCount    = pComputeResourceAllocation->gpcCount;
    pConfigRequestPerCi[0].profile.veidCount   = pComputeResourceAllocation->veidCount;
    pConfigRequestPerCi[0].veidSpanStart       = info.spanStart;

    shadowVeidInUseMask = kgrmgrGetVeidInUseMask(pGpu, pKernelGraphicsManager);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        kgrmgrCheckVeidsRequest(pGpu, pKernelGraphicsManager,
                                &shadowVeidInUseMask,
                                pConfigRequestPerCi[0].profile.veidCount,
                                &pConfigRequestPerCi[0].veidSpanStart,
                                pKernelMIGGpuInstance),
        done);


    NV_ASSERT(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].id == KMIGMGR_COMPUTE_INSTANCE_ID_INVALID);

    pMIGComputeInstance->spanStart = pConfigRequestPerCi[0].veidSpanStart;

    portMemCopy(&pKernelMIGGpuInstance->MIGComputeInstance[CIIdx],
                sizeof(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx]),
                pMIGComputeInstance,
                sizeof(*pMIGComputeInstance));

    // Configure the GR engines for each compute instance
    status = kmigmgrConfigureGPUInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId,
                                         pConfigRequestPerCi,
                                         NVBIT32(RM_ENGINE_TYPE_GR_IDX(localEngineType)));

    // Do our best to deconfigure the engines we configured so far, then bail
    if (status != NV_OK)
    {
        portMemSet(&pKernelMIGGpuInstance->MIGComputeInstance[CIIdx], 0x0, sizeof(pKernelMIGGpuInstance->MIGComputeInstance[CIIdx]));
        portMemSet(pConfigRequestPerCi, 0x0, sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);
        // Quash status. This is best-effort cleanup
        (void)kmigmgrConfigureGPUInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId,
                                          pConfigRequestPerCi,
                                          NVBIT32(RM_ENGINE_TYPE_GR_IDX(localEngineType)));

        goto done;
    }

    //
    // Register instance with the capability framework only if it explicitly
    // requested. Otherwise, we rely on the persistent state.
    //
    if (bCreateCap)
    {
        // Register compute instance with the capability framework
        NV_ASSERT_OK_OR_GOTO(status,
            osRmCapRegisterSmcExecutionPartition(pKernelMIGGpuInstance->pOsRmCaps,
                                                 &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].pOsRmCaps,
                                                 pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].id),
            cleanup_created_instances);
    }

    // Allocate RsShared for the instance
    NV_ASSERT_OK_OR_GOTO(status,
        serverAllocShare(&g_resServ, classInfo(RsShared),
                         &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].pShare),
        cleanup_created_instances);

    // Allocate subscribed handles for this instance
    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        NV_ASSERT_OK_OR_GOTO(status,
        kmigmgrAllocComputeInstanceHandles(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx]),
        cleanup_created_instances);

        NV_ASSERT_OK(
            kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                   &pComputeResourceAllocation->engines, &globalEngineType));
        NV_ASSERT_OK(
            kmigmgrEngineTypeXlate(&pResourceAllocation->localEngines, globalEngineType,
                                   &pResourceAllocation->engines, &globalEngineType));
        globalGrIdx = RM_ENGINE_TYPE_GR_IDX(globalEngineType);

        pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, globalGrIdx);
        fecsSetRoutingInfo(pGpu,
                           pKernelGraphics,
                           pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].instanceHandles.hClient,
                           pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].instanceHandles.hSubdevice,
                           0);

        NV_ASSERT_OK_OR_GOTO(status,
            kgraphicsCreateGoldenImageChannel(pGpu, pKernelGraphics),
            cleanup_created_instances);
    }

    status = NV_OK;
    goto done;

cleanup_created_instances:
    (void)kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                       CIIdx, NV_FALSE);
done:
    portMemFreeStackOrHeap(pConfigRequestPerCi);
    portMemFree(pMIGComputeInstance);
    return status;
}

// Delete created instance handles if they exist
void
kmigmgrFreeComputeInstanceHandles_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance
)
{
    if (pMIGComputeInstance->instanceHandles.hClient != NV01_NULL_OBJECT)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        pRmApi->Free(pRmApi, pMIGComputeInstance->instanceHandles.hClient, pMIGComputeInstance->instanceHandles.hClient);
        pMIGComputeInstance->instanceHandles.hClient = NV01_NULL_OBJECT;
        pMIGComputeInstance->instanceHandles.hSubdevice = NV01_NULL_OBJECT;
        pMIGComputeInstance->instanceHandles.hSubscription = NV01_NULL_OBJECT;
    }
}

/*!
 * @brief   Releases the engines owned by this Compute Instance of the given class
 *          of engine (GR, COPY, etc) to the GPU Instance resource pools.
 */
void
kmigmgrReleaseComputeInstanceEngines_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance
)
{
    RM_ENGINE_TYPE globalEngineType;
    RM_ENGINE_TYPE localEngineType;
    ENGTYPE_BIT_VECTOR *pGlobalMask;
    ENGTYPE_BIT_VECTOR *pLocalMask;

    NV_ASSERT_OR_RETURN_VOID(pKernelMIGGpuInstance != NULL);
    NV_ASSERT_OR_RETURN_VOID(pMIGComputeInstance != NULL);

    pGlobalMask = &pKernelMIGGpuInstance->resourceAllocation.engines;
    pLocalMask = &pKernelMIGGpuInstance->resourceAllocation.localEngines;

    // Iterate over both global/local masks at the same time
    FOR_EACH_IN_BITVECTOR_PAIR(pGlobalMask, globalEngineType, pLocalMask, localEngineType)
    {
        NvU32 CIIdx;

        // Skip anything not owned by this compute instance
        if (!bitVectorTest(&pMIGComputeInstance->resourceAllocation.engines, localEngineType))
            continue;

        //
        // Clear this engine from the exclusive ownership mask. If it was being
        // shared, it already isn't in the exclusive ownership mask, so doing
        // this for all engines in this compute instance isn't harmful.
        //
        bitVectorClr(&pKernelMIGGpuInstance->exclusiveEngMask, globalEngineType);

        // If this engine was exclusively owned, nothing else to do
        if (!bitVectorTest(&pKernelMIGGpuInstance->sharedEngMask, globalEngineType))
            continue;

        // Determine if any other compute instance owns this engine
        for (CIIdx = 0;
             CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
             ++CIIdx)
        {
            if (!pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].bValid)
                continue;

            if (bitVectorTest(&pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].resourceAllocation.engines,
                              localEngineType))
            {
                break;
            }
        }

        // If engine is still owned by someone, don't mark it unused
        if (CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance))
            continue;

        // mark this engine as no longer being shared by anyone
        bitVectorClr(&pKernelMIGGpuInstance->sharedEngMask, globalEngineType);
    }
    FOR_EACH_IN_BITVECTOR_PAIR_END();
}

/*!
 * @brief   Function to delete Compute Instance
 *
 * @param[IN]  pGpu
 * @param[IN]  pKernelMIGManager
 * @param[IN]  pKernelMIGGpuInstance
 * @param[IN]  CIID                  Compute Instance ID
 * @param[IN]  bUnload               NV_TRUE if called during gpu state unload path
 */
NV_STATUS
kmigmgrDeleteComputeInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvU32 CIID,
    NvBool bUnload
)
{
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation;
    ENGTYPE_BIT_VECTOR grEngines;
    NvU32 swizzId;
    KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestPerCi;
    NvU32 updateEngMask;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(CIID < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance),
                        NV_ERR_INVALID_ARGUMENT);

    // Make sure that the targeted compute instance is still valid
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       pKernelMIGGpuInstance->MIGComputeInstance[CIID].bValid,
                       NV_WARN_NOTHING_TO_DO);

    pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[CIID];
    pComputeResourceAllocation = &pMIGComputeInstance->resourceAllocation;

    //
    // Initial refCount is increased to "1" when instance is created and then
    // every subscription by a client should increase the refcount
    //
    if ((pMIGComputeInstance->pShare != NULL) &&
        (serverGetShareRefCount(&g_resServ, pMIGComputeInstance->pShare) > 2))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Compute Instance with id - %d still in use by other clients\n",
                  CIID);

        return NV_ERR_STATE_IN_USE;
    }

    if (!bUnload)
    {
        //
        // Unregister instance from the capability framework only if
        // it is explicitly destroyed i.e. not during GPU state unload path.
        //
        // Note that the saved instance persistent state will be freed by
        // _gpumgrUnregisterRmCapsForMIGCI during driver unload.
        //
        osRmCapUnregister(&pMIGComputeInstance->pOsRmCaps);
    }

    // Deconfigure the GR engine for this compute instance
    swizzId = pKernelMIGGpuInstance->swizzId;

    pConfigRequestPerCi = portMemAllocStackOrHeap(sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);
    NV_ASSERT_OR_RETURN(pConfigRequestPerCi != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pConfigRequestPerCi, 0x0, sizeof(*pConfigRequestPerCi) * KMIGMGR_MAX_COMPUTE_INSTANCES);

    bitVectorClrAll(&grEngines);
    bitVectorSetRange(&grEngines, RM_ENGINE_RANGE_GR());
    bitVectorAnd(&grEngines, &grEngines, &pComputeResourceAllocation->engines);
    NV_ASSERT_OR_ELSE(!bitVectorTestAllCleared(&grEngines), status = NV_ERR_INVALID_STATE; goto done;);
    updateEngMask = NVBIT32(RM_ENGINE_TYPE_GR_IDX(bitVectorCountTrailingZeros(&grEngines)));
    NV_ASSERT_OK_OR_GOTO(status,
        kmigmgrConfigureGPUInstance(pGpu, pKernelMIGManager, swizzId, pConfigRequestPerCi, updateEngMask),
        done);

    {
        RM_ENGINE_TYPE globalRmEngType;
        MIG_INSTANCE_REF ref = kmigmgrMakeCIReference(pKernelMIGGpuInstance, pMIGComputeInstance);
        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              RM_ENGINE_TYPE_GR(0),
                                              &globalRmEngType),
            done);

        // Free up the internal handles for this compute instance
        kmigmgrFreeComputeInstanceHandles(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, pMIGComputeInstance);

        fecsSetRoutingInfo(pGpu,
                           GPU_GET_KERNEL_GRAPHICS(pGpu, RM_ENGINE_TYPE_GR_IDX(globalRmEngType)),
                           pKernelMIGGpuInstance->instanceHandles.hClient,
                           pKernelMIGGpuInstance->instanceHandles.hSubdevice,
                           RM_ENGINE_TYPE_GR_IDX(bitVectorCountTrailingZeros(&grEngines)));

        if (pMIGComputeInstance->pShare != NULL)
        {
            serverFreeShare(&g_resServ, pMIGComputeInstance->pShare);
            pMIGComputeInstance->pShare = NULL;
        }
    }

    // Mark this compute instance as invalid
    pMIGComputeInstance->bValid = NV_FALSE;

    // Release this compute instance's engines
    kmigmgrReleaseComputeInstanceEngines(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, pMIGComputeInstance);

    if (IS_VIRTUAL(pGpu))
    {
        KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);
        if (pKernelCcu != NULL)
        {
            NV_ASSERT_OK(kccuDeInitVgpuMigSharedBuffer(pGpu, pKernelCcu, swizzId, pMIGComputeInstance->id));
        }
    }

    // Now that we no longer need it, clear the shared engine flag
    pMIGComputeInstance->sharedEngFlag = 0x0;
    pMIGComputeInstance->id = KMIGMGR_COMPUTE_INSTANCE_ID_INVALID;

    pMIGComputeInstance->pOsRmCaps = NULL;

done:
    portMemFreeStackOrHeap(pConfigRequestPerCi);

    return status;
}

/*!
 * @brief print out the CI configuration of this GI
 */
static void
_kmigmgrPrintComputeInstances
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
#define PADDING_STR "----------------------------------------------------"
    RM_ENGINE_TYPE rmEngineType;
    NvU32 CIIdx;

    NV_PRINTF(LEVEL_INFO, "\n");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %14s | %14s | %14s |\n",
              "SwizzId",
              "GR Count",
              "Gpc Count");
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
    NV_PRINTF(LEVEL_INFO, "| %14d | %14d | %14d |\n",
              pKernelMIGGpuInstance->swizzId,
              kmigmgrCountEnginesOfType(&pKernelMIGGpuInstance->resourceAllocation.engines, RM_ENGINE_TYPE_GR(0)),
              pKernelMIGGpuInstance->resourceAllocation.gpcCount);

    for (CIIdx = 0;
         CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
         ++CIIdx)
    {
        MIG_RESOURCE_ALLOCATION *pComputeResourceAllocation;

        if (!pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].bValid)
        {
            continue;
        }

        pComputeResourceAllocation = &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx].resourceAllocation;

        NV_ASSERT_OK(
            kmigmgrEngineTypeXlate(&pComputeResourceAllocation->localEngines, RM_ENGINE_TYPE_GR(0),
                                   &pComputeResourceAllocation->engines, &rmEngineType));

        NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);
        if (IS_GSP_CLIENT(pGpu))
        {
            NvU32 gpcIdx;
            NvU32 gpcMask = 0x0;

            for (gpcIdx = 0; gpcIdx < pComputeResourceAllocation->gpcCount; ++gpcIdx)
            {
                 gpcMask |= NVBIT32(pComputeResourceAllocation->gpcIds[gpcIdx]);
            }
            NV_PRINTF(LEVEL_INFO, "| %23s | %23s |\n",
                      "Gr Engine IDX",
                      "GPC Mask");
            NV_PRINTF(LEVEL_INFO, "| %23d | %23X |\n",
                      RM_ENGINE_TYPE_GR_IDX(rmEngineType),
                      gpcMask);
        }
        else
        {
            // gpcMask is not meaningful in VGPU, thus only printing gpcCount
            NV_PRINTF(LEVEL_INFO, "| %23s | %23s |\n",
                      "Gr Engine IDX",
                      "GPC Count");
            NV_PRINTF(LEVEL_INFO, "| %23d | %23X |\n",
                      RM_ENGINE_TYPE_GR_IDX(rmEngineType),
                      pComputeResourceAllocation->gpcCount);
        }
    }
    NV_PRINTF(LEVEL_INFO, "%s\n", PADDING_STR);

#undef PADDING_STR
#endif // NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
}

/*!
 * @brief   Function to configure a specific GPU instance by setting available
 *          GPCs with requested GR Engines
 *
 * @param[IN]   pGpu
 * @param[IN}   pKernelMIGManager
 * @param[OUT]  swizzId             SwizzId for this GPU instance
 * @param[IN]   pGpcCountPerGr      Requested num GPCs for every GR engine in
 *                                  this instance
 * @param[IN]   updateEngMask       Entry valid flag for each engine in instance
 *
 * @return  Returns NV_STATUS
 *          NV_OK
 *          NV_ERR_INVALID_ARGUMENT
 *          NV_WARN_NOTHING_TO_DO
 *          NV_ERR_INSUFFICIENT_RESOURCES
 */
NV_STATUS
kmigmgrConfigureGPUInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    const KMIGMGR_CONFIGURE_INSTANCE_REQUEST *pConfigRequestsPerCi,
    NvU32 updateEngMask
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS status = NV_OK;
    NvU32 i;
    NvU32 j;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
    NvBool bAssigning;
    RM_ENGINE_TYPE checkGrs[RM_ENGINE_TYPE_GR_SIZE];
    NvU32 checkGrCount = 0;
    RM_ENGINE_TYPE rmEngineType;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvBool bIsCTSRequired = kmigmgrIsCTSAlignmentRequired_HAL(pGpu, pKernelMIGManager);
    NvU32 localIdx;

    // Sanity check the GPU instance requested to be configured
    if (!kmigmgrIsSwizzIdInUse(pGpu, pKernelMIGManager, swizzId))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid swizzId - %d.\n", swizzId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    bAssigning = NV_FALSE;
    portMemSet(checkGrs, 0, sizeof(checkGrs));

    i = 0;
    localIdx = 0;
    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

        // Skip over invalid entries
        if (!(updateEngMask & NVBIT32(i)))
        {
            i++;
            continue;
        }

        // Resource checks are verified by CTS ID assignment when required, else use GPC count
        if (bIsCTSRequired)
        {
            NV_CHECK_OR_RETURN(LEVEL_ERROR,
                pConfigRequestsPerCi[localIdx].ctsId != KMIGMGR_CTSID_INVALID,
                NV_ERR_INSUFFICIENT_RESOURCES);
        }
        else
        {
            // Make sure no requested GPC count is greater than instance GPC count
            if (pConfigRequestsPerCi[localIdx].profile.gpcCount > pKernelMIGGpuInstance->resourceAllocation.gpcCount)
            {
                 NV_PRINTF(LEVEL_ERROR,
                           "Invalid GPC count - %d requested for GrIdx - %d.\n",
                           pConfigRequestsPerCi[localIdx].profile.gpcCount,
                           engineIdx);
                 return NV_ERR_INVALID_ARGUMENT;
            }
        }

        bAssigning = bAssigning || pConfigRequestsPerCi[localIdx].profile.gpcCount > 0;
        checkGrs[checkGrCount++] = rmEngineType;

        localIdx++;
        i++;
    }
    FOR_EACH_IN_BITVECTOR_END();

    //
    // Return an error if there are any channels on any engines targeted by this
    // request
    //
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       !kfifoEngineListHasChannel(pGpu, pKernelFifo, checkGrs, checkGrCount),
                       NV_ERR_STATE_IN_USE);

    if (!bAssigning)
    {
        // Invalidate targeted engines
        i = 0;
        FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
        {
            NvU32 engineIdx;

            if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
                continue;

            engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

            if (updateEngMask & NVBIT32(i))
            {
                NV_ASSERT_OK_OR_RETURN(
                    kmigmgrInvalidateGr(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, engineIdx));
            }

            i++;
        }
        FOR_EACH_IN_BITVECTOR_END();

        return NV_OK;
    }

    //
    // Client passes the logical GR-IDs while RM works with physical GR-IDs
    // Walk the list of physical GRs associated with this GPU instance and then
    // set GPCs as requested
    //
    i = 0;
    localIdx = 0;
    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        NvU32 gpcCount = pConfigRequestsPerCi[localIdx].profile.gpcCount;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

        if (!(updateEngMask & NVBIT32(i)))
        {
            i++;
            continue;
        }

        if (gpcCount == 0)
        {
            localIdx++;
            i++;
            continue;
        }

        // Update the GR to VEID mapping
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kgrmgrAllocVeidsForGrIdx(pGpu,
                                     pKernelGraphicsManager,
                                     engineIdx,
                                     pConfigRequestsPerCi[localIdx].veidSpanStart,
                                     pConfigRequestsPerCi[localIdx].profile.veidCount,
                                     pKernelMIGGpuInstance),
            cleanup);

        i++;
    }
    FOR_EACH_IN_BITVECTOR_END();

    _kmigmgrPrintComputeInstances(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    i = 0;
    localIdx = 0;
    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        NvU32 gpcCount = pConfigRequestsPerCi[localIdx].profile.gpcCount;
        KernelGraphics *pKGr;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

        if (!(updateEngMask & NVBIT32(i)))
        {
            i++;
            continue;
        }

        if (gpcCount == 0)
        {
            localIdx++;
            i++;
            continue;
        }

        if (bIsCTSRequired)
            kmigmgrSetCTSIdInUse(pKernelMIGGpuInstance, pConfigRequestsPerCi[localIdx].ctsId, engineIdx, NV_TRUE);

        pKGr = GPU_GET_KERNEL_GRAPHICS(pGpu, engineIdx);
        // Re-pull public static data for kernel graphics
        status = kgraphicsLoadStaticInfo_HAL(pGpu, pKGr, pKernelMIGGpuInstance->swizzId);
        if (status != NV_OK)
            goto cleanup;

        // record sizes of local GR ctx buffers for this GR
        status = kgrmgrDiscoverMaxLocalCtxBufInfo(pGpu, pKernelGraphicsManager, pKGr, swizzId);
        if (status != NV_OK)
            goto cleanup;

        i++;
    }
    FOR_EACH_IN_BITVECTOR_END();

    return status;

cleanup:

    j = 0;
    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;

        // Rollback all previous validations
        if (j == i)
            break;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

        if (updateEngMask & NVBIT32(j))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to configure GPU instance. Invalidating GRID - %d\n",
                      engineIdx);

            // Invalidate assignments to this GR, clear global state
            kmigmgrInvalidateGr(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, engineIdx);
        }

        j++;
    }
    FOR_EACH_IN_BITVECTOR_END();

    return status;
}

// invalidate GR to GPC mappings
NV_STATUS
kmigmgrInvalidateGrGpcMapping_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvU32 grIdx
)
{
    NV_STATUS status = NV_OK;
    NvU32 gfid;
    NvBool bCallingContextPlugin;
    KernelGraphics *pKernelGraphics;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
    NV_ASSERT_OK_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin));
    if (bCallingContextPlugin)
    {
        gfid = GPU_GFID_PF;
    }

    // Release CTS-ID fields
    if (kmigmgrIsCTSAlignmentRequired_HAL(pGpu, pKernelMIGManager))
        kmigmgrSetCTSIdInUse(pKernelMIGGpuInstance, KMIGMGR_CTSID_INVALID, grIdx, NV_FALSE);

    // Free global ctx buffers, this will need to be regenerated
    pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
    fecsBufferTeardown(pGpu, pKernelGraphics);
    kgraphicsFreeGlobalCtxBuffers(pGpu, pKernelGraphics, gfid);

    // clear cached ctx buf sizes
    kgraphicsClearCtxBufferInfo(pGpu, pKernelGraphics);

    return status;
}

// invalidate a GR engine
NV_STATUS
kmigmgrInvalidateGr_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvU32 grIdx
)
{
    KernelGraphics *pKGr = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmigmgrInvalidateGrGpcMapping(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, grIdx));

    kgrmgrClearVeidsForGrIdx(pGpu, pKernelGraphicsManager, grIdx);

    kgraphicsInvalidateStaticInfo(pGpu, pKGr);
    return NV_OK;
}

/*!
 * @brief   Function to invalidate a gpu instance
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   swizzId             swizzId which is getting invalidated
 * @param[IN]   bUnload             NV_TRUE if called from gpu state unload path
 *
 * @return  Returns NV_STATUS
 *          NV_OK
 *          NV_ERR_INVALID_ARGUMENT   No GPC associated with Gr
 */
NV_STATUS
kmigmgrInvalidateGPUInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvBool bUnload
)
{
    NV_STATUS rmStatus = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
    NvU32 i;
    RM_ENGINE_TYPE rmEngineType;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    // Sanity checks
    rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance);
    if (rmStatus != NV_OK)
    {
        // Didn't find requested gpu instance
        NV_PRINTF(LEVEL_ERROR, "No valid gpu instance with SwizzId - %d found\n",
                  swizzId);
        return rmStatus;
    }

    // Make sure that no client is using this gpu instance
    if (!kmigmgrIsGPUInstanceReadyToBeDestroyed(pKernelMIGGpuInstance))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Gpu instance with SwizzId - %d still in use by other clients\n",
                  swizzId);

        kmigmgrPrintSubscribingClients(pGpu, pKernelMIGManager, swizzId);
        return NV_ERR_STATE_IN_USE;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++i)
    {
        if (pKernelMIGGpuInstance->MIGComputeInstance[i].bValid)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Cannot destroy gpu instance %u with valid compute instance %d \n",
                      swizzId, i);

            return NV_ERR_STATE_IN_USE;
        }
    }

    NV_PRINTF(LEVEL_INFO, "FREEING GPU INSTANCE\n");
    kmigmgrPrintGPUInstanceInfo(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    if (!bUnload)
    {
        //
        // Unregister gpu instance from the capability framework only if
        // it is explicitly destroyed i.e. not during GPU state unload path.
        //
        // Note that the saved gpu instance persistent state will be freed by
        // _gpumgrUnregisterRmCapsForSmcPartitions during driver unload.
        //
        osRmCapUnregister(&pKernelMIGGpuInstance->pOsRmCaps);
    }

    // Remove GR->GPC mappings in GPU instance Info
    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        KernelGraphics *pKernelGraphics;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kmigmgrInvalidateGr(pGpu, pKernelMIGManager, pKernelMIGGpuInstance, engineIdx));

        pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, engineIdx);
        fecsClearRoutingInfo(pGpu, pKernelGraphics);
    }
    FOR_EACH_IN_BITVECTOR_END();

    // Delete client handle after all GR's are invalidated
    kmigmgrFreeGPUInstanceHandles(pKernelMIGGpuInstance);

    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        kmigmgrClearEnginesInUse(pGpu, pKernelMIGManager, &pKernelMIGGpuInstance->resourceAllocation.engines));

    // Destroy runlist buffer pools
    kmigmgrDestroyGPUInstanceGrBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    if (kmigmgrIsSwizzIdInUse(pGpu, pKernelMIGManager, swizzId))
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kmigmgrClearSwizzIdInUse(pGpu, pKernelMIGManager, swizzId));
    }

    // Sanity check that requested swizzID is not set in swizzIdMask
    NV_ASSERT_OR_ELSE(!(NVBIT64(swizzId) & pKernelMIGManager->swizzIdInUseMask), rmStatus = NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        kmemsysInitMIGMemoryPartitionTable_HAL(pGpu, pKernelMemorySystem));

    // Destroy gpu instance scrubber
    kmigmgrDestroyGPUInstanceScrubber(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    // Destroy gpu instance pool for page table mem
    kmigmgrDestroyGPUInstancePool(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    // Delete gpu instance engine runlists
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        kmigmgrDeleteGPUInstanceRunlists_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance));

    // Destroy runlist buffer pools
    kmigmgrDestroyGPUInstanceRunlistBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    // Free gpu instance memory
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        memmgrFreeMIGGPUInstanceMemory(pGpu, pMemoryManager, swizzId, pKernelMIGGpuInstance->hMemory, &pKernelMIGGpuInstance->pMemoryPartitionHeap));

    if (pKernelMIGGpuInstance->pShare != NULL)
    {
        serverFreeShare(&g_resServ, pKernelMIGGpuInstance->pShare);
        pKernelMIGGpuInstance->pShare = NULL;
    }

    // Initialize gpu instance info to initial value
    kmigmgrInitGPUInstanceInfo(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);

    //
    // Only partitions in which VGPU guests are booted require changing
    // engine interrupt vectors to deterministic values for migration.
    //
    if (IS_GSP_CLIENT(pGpu) && gpuIsSriovEnabled(pGpu))
    {
        //
        // When running in GSP offload mode, KernelRM must re-fetch the
        // interrupt table on every change to the MIG partitioning layout.
        //
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            intrRefetchInterruptTable_HAL(pGpu, GPU_GET_INTR(pGpu)));
    }

    return rmStatus;
}

/*!
 * @brief   Init gpu instance scrubber
 */
NV_STATUS
kmigmgrInitGPUInstanceScrubber_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (!IsSLIEnabled(pGpu) &&
         memmgrIsScrubOnFreeEnabled(pMemoryManager) &&
         memmgrIsPmaInitialized(pMemoryManager))
    {
        NV_ASSERT_OK_OR_RETURN(scrubberConstruct(pGpu, pKernelMIGGpuInstance->pMemoryPartitionHeap));
        pKernelMIGGpuInstance->bMemoryPartitionScrubberInitialized = NV_TRUE;
    }

    return NV_OK;
}

/*!
 * @brief   Destroy gpu instance scrubber
 */
void
kmigmgrDestroyGPUInstanceScrubber_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    OBJMEMSCRUB *pMemscrub = NULL;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (!pKernelMIGGpuInstance->bMemoryPartitionScrubberInitialized)
        return;

    if (!IsSLIEnabled(pGpu) &&
         memmgrIsScrubOnFreeEnabled(pMemoryManager) &&
         memmgrIsPmaInitialized(pMemoryManager))
    {
        pMemscrub = pKernelMIGGpuInstance->pMemoryPartitionHeap->pmaObject.pScrubObj;
        scrubberDestruct(pGpu, pKernelMIGGpuInstance->pMemoryPartitionHeap, pMemscrub);
        pKernelMIGGpuInstance->bMemoryPartitionScrubberInitialized = NV_FALSE;
    }
}

/*!
 * @brief   Releases GR buffer memory back from global buffer pools and destroys
 *          these pools for all GR engines that belong to this gpu instance.
 */
void
kmigmgrDestroyGPUInstanceGrBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    RM_ENGINE_TYPE rmEngineType;

    if (!ctxBufPoolIsSupported(pGpu))
        return;

    NV_ASSERT(pKernelMIGGpuInstance != NULL);

    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        KernelGraphics *pKernelGraphics;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);
        pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, engineIdx);

        kgraphicsDestroyCtxBufPool(pGpu, pKernelGraphics);
    }
    FOR_EACH_IN_BITVECTOR_END();
}

/*!
 * @brief   Destroy per-gpu instance memory pool for client page tables
 */
void
kmigmgrDestroyGPUInstancePool_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (!memmgrIsPmaInitialized(pMemoryManager) ||
        !memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        NV_ASSERT_OR_GOTO((pKernelMIGGpuInstance->pPageTableMemPool == NULL), destroy_pool);
        return;
    }

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId))
    {
        NV_ASSERT_OR_GOTO((pKernelMIGGpuInstance->pPageTableMemPool == NULL), destroy_pool);
        return;
    }

    if (pKernelMIGGpuInstance->pPageTableMemPool == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "page table memory pool not setup\n");
        return;
    }

destroy_pool:
    rmMemPoolDestroy(pKernelMIGGpuInstance->pPageTableMemPool);
    pKernelMIGGpuInstance->pPageTableMemPool = NULL;
}

/*!
 * @brief   Releases runlist buffer memory back from runlist buffer pools and destroys the
 *          runlist buffer pools for engines that belong to these gpu instance.
 */
void
kmigmgrDestroyGPUInstanceRunlistBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    RM_ENGINE_TYPE rmEngineType;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId))
        return;

    if (!ctxBufPoolIsSupported(pGpu))
        return;

    for (rmEngineType = 0; rmEngineType < RM_ENGINE_TYPE_LAST; rmEngineType++)
    {
        if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType) ||
            !kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType) ||
            !kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, kmigmgrMakeGIReference(pKernelMIGGpuInstance)))
        {
            continue;
        }

        if (pKernelFifo->pRunlistBufPool[rmEngineType] != NULL)
        {
            ctxBufPoolRelease(pKernelFifo->pRunlistBufPool[rmEngineType]);
            ctxBufPoolDestroy(&pKernelFifo->pRunlistBufPool[rmEngineType]);
        }
    }
}

/*!
 * @brief   Print out clients subscribing to specified gpu instance
 */
void
kmigmgrPrintSubscribingClients_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    RmClient **ppClient;
    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient != NULL;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        RmClient *pClient = *ppClient;
        RsClient *pRsClient = staticCast(pClient, RsClient);
        MIG_INSTANCE_REF ref;
        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(pClient);
        RS_ITERATOR it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

        while (clientRefIterNext(pRsClient, &it))
        {
            NV_STATUS status;
            Device *pDevice = dynamicCast(it.pResourceRef->pResource, Device);

            if (pDevice == NULL)
                continue;

            status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                     pDevice, &ref);
            if (status != NV_OK)
                continue;

            if (ref.pKernelMIGGpuInstance->swizzId != swizzId)
                continue;

            (void)privLevel;
            NV_PRINTF(LEVEL_INFO, "%s client %x device %x currently subscribed to swizzId %u\n",
                      (privLevel >= RS_PRIV_LEVEL_KERNEL) ? "Kernel" : "Usermode",
                      pRsClient->hClient, RES_GET_HANDLE(pDevice), swizzId);
        }
    }
}

/*!
 * @brief   Function to enable/disable MIG mode
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   bMemoryPartitioningNeeded   Is Memory partitioning required?
 * @param[IN]   bEnable                     Enable/Disable MIG
 * @param[IN]   bUnload                     RM unload path
 *
 * @return  Returns NV_STATUS
 *          NV_OK
 *          NV_WARN_NOTHING_TO_DO
 *          NV_ERR_INVALID_STATE
 */
NV_STATUS
kmigmgrSetMIGState_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bMemoryPartitioningNeeded,
    NvBool bEnable,
    NvBool bUnload
)
{
    if (bEnable)
    {
        KernelGraphics *pKGr = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);

        kgraphicsInvalidateStaticInfo(pGpu, pKGr);
    }

    return NV_OK;
}

/*!
 * @brief   Function to enable/disable MIG mode
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   bMemoryPartitioningNeeded   Is Memory partitioning required?
 * @param[IN]   bEnable                     Enable/Disable MIG
 * @param[IN]   bUnload                     RM unload path
 *
 * @return  Returns NV_STATUS
 *          NV_OK
 *          NV_WARN_NOTHING_TO_DO
 *          NV_ERR_INVALID_STATE
 */
NV_STATUS
kmigmgrSetMIGState_FWCLIENT
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bMemoryPartitioningNeeded,
    NvBool bEnable,
    NvBool bUnload
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_STATUS rmStatus = NV_OK;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool bPrevMIGState = pKernelMIGManager->bMIGEnabled;

    if (bEnable)
    {
        KernelGraphics *pKGr = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kgrmgrDiscoverMaxGlobalCtxBufSizes(pGpu, pKernelGraphicsManager, pKGr, bMemoryPartitioningNeeded),
            done);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrDisableWatchdog(pGpu, pKernelMIGManager),
            cleanup_disableWatchdog);

        // Before enabling MIG, deconfigure GR0 in legacy mode
        kgraphicsInvalidateStaticInfo(pGpu, pKGr);

        //
        // Destroy all global ctx buffers, we will need to recreate them in
        // partitionable memory later.
        //
        fecsBufferTeardown(pGpu, pKGr);

        kgraphicsFreeGlobalCtxBuffers(pGpu, pKGr, GPU_GFID_PF);

        //
        // Save the pre-MIG top-level scrubber status for later
        // Destroy the top level scrubber if it exists
        //
        NV_ASSERT_OK_OR_GOTO(rmStatus,
            memmgrDestroyInternalChannels(pGpu, pMemoryManager),
            cleanup_destroyInternalChannels);

        //
        // Preexisting channel and memory allocation checks should be done after
        // all buffers(like global Gr buffers) and pre-created channels(like scrubber, watchdog etc.)
        // are destroyed.
        //
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrCreateGPUInstanceCheck_HAL(pGpu, pKernelMIGManager, bMemoryPartitioningNeeded),
            cleanup_createPartitionCheck);

        // On Nvswitch based systems, suspend gpu fabric probe on nvlink inband
        gpuFabricProbeSuspend(pGpu->pGpuFabricProbeInfoKernel);

        // Ensure NVLINK is shutdown before enabling MIG
        if (!kmigmgrIsMIGNvlinkP2PSupportOverridden(pGpu, pKernelMIGManager) ||
            bMemoryPartitioningNeeded)
        {
#if (defined(DEBUG) || defined(DEVELOP))
            KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

            if (pKernelNvlink != NULL)
            {
                NvU32 linkId;

                //TODO: Remove below code once a more robust SRT is available to test for this condition
                FOR_EACH_INDEX_IN_MASK(32, linkId, knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink))
                {
                    NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS params;

                    params.linkId = linkId;
                    params.callbackType.type = NV2080_CTRL_NVLINK_CALLBACK_TYPE_GET_DL_LINK_MODE;
                    NV_CHECK_OK(rmStatus, LEVEL_ERROR,
                        knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                            NV2080_CTRL_CMD_NVLINK_CORE_CALLBACK,
                                            (void *)&params, sizeof(params)));

                    if ((params.callbackType.callbackParams.getDlLinkMode.mode != NV2080_NVLINK_CORE_LINK_STATE_SLEEP) ||
                        (params.callbackType.callbackParams.getDlLinkMode.mode != NV2080_NVLINK_CORE_LINK_STATE_OFF))
                    {
                        NV_PRINTF(LEVEL_ERROR, "Nvlink %d is not asleep upon enteing MIG mode!\n", linkId);
                    }
                }
                FOR_EACH_INDEX_IN_MASK_END
            }
            rmStatus = NV_OK;
#endif
            NV_ASSERT_OK_OR_GOTO(rmStatus,
                gpuDeleteClassFromClassDBByClassId(pGpu, NV50_P2P),
                cleanup_disableNvlink);
        }

        // Enable ctx buf pool before allocating any resources that uses it.
        pGpu->setProperty(pGpu, PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA, bMemoryPartitioningNeeded);

        // Add the MIG-specific classes
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            gpuAddClassToClassDBByClassId(pGpu, AMPERE_SMC_PARTITION_REF));

        if (rmStatus != NV_OK)
            goto cleanup_addClassToClassDB;

        // Allocate handles for memory partitioning if needed
        if (bMemoryPartitioningNeeded)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                memmgrAllocMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager),
                cleanup_memsysConfigL2EvictLast);
        }

        // initialize pKernelFifo->pppRunlistBufMemDesc based on max possible # of runlists.
        {
            MEMORY_DESCRIPTOR ***pppMemDesc = NULL;
            NvU32 maxRunlists = kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo);
            NvU32 rowSize = sizeof(pppMemDesc) * maxRunlists;
            NvU32 arrSize =  rowSize * NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_BUFFERS;
            NvU32 i;

            // Should not have already been initialized
            NV_ASSERT(pKernelFifo->pppRunlistBufMemDesc == NULL);

            pppMemDesc = portMemAllocNonPaged(rowSize);
            NV_ASSERT_OR_ELSE(pppMemDesc != NULL, rmStatus = NV_ERR_NO_MEMORY; goto cleanup_initialize_runlistBufMemDesc;);
            portMemSet(pppMemDesc, 0, rowSize);

            *pppMemDesc = portMemAllocNonPaged(arrSize);
            NV_ASSERT_OR_ELSE(*pppMemDesc != NULL, rmStatus = NV_ERR_NO_MEMORY; goto cleanup_initialize_runlistBufMemDesc;);
            portMemSet(*pppMemDesc, 0, arrSize);

            // Set up pointers for the 2D array
            for (i = 0; i < maxRunlists; i++)
            {
                pppMemDesc[i] = *pppMemDesc + (NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_BUFFERS * i);
            }

            pKernelFifo->pppRunlistBufMemDesc = pppMemDesc;
        }

        //
        // Populate static GPU instance memory config which will be used to manage
        // GPU instance memory
        //
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        NV_ASSERT_OK_OR_RETURN(kmemsysPopulateMIGGPUInstanceMemConfig_HAL(pGpu, pKernelMemorySystem));
    }
    else
    {
        if (bMemoryPartitioningNeeded)
        {
            memmgrFreeMIGMemoryAllocationInternalHandles(pGpu, pMemoryManager);
        }

cleanup_initialize_runlistBufMemDesc:

        if (pKernelFifo->pppRunlistBufMemDesc != NULL)
        {
            portMemFree(*(pKernelFifo->pppRunlistBufMemDesc));
            portMemFree(pKernelFifo->pppRunlistBufMemDesc);
        }

        pKernelFifo->pppRunlistBufMemDesc = NULL;

cleanup_memsysConfigL2EvictLast:

cleanup_addClassToClassDB:
        // Delete the MIG GR classes as MIG is disabled
        NV_ASSERT_OK(
            gpuDeleteClassFromClassDBByClassId(pGpu, AMPERE_SMC_PARTITION_REF));

        //
        // Disable ctx buf pool after freeing any resources that uses it.
        // Leave enabled on platforms that support it outside MIG.
        //
        pGpu->setProperty(pGpu, PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA,
            gpuIsCtxBufAllocInPmaSupported_HAL(pGpu));

        //
        // HACK: GSP-RM always enables/disables LCEs during MIG enable/disable.
        //       Client-RM must always follow it to update its settings accordingly,
        //       so it should only call it for MIG disable (and not as part of MIG
        //       enable).
        //
        if (!bEnable)
        {
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                kmigmgrEnableAllLCEs(pGpu, pKernelMIGManager, NV_FALSE));
        }

cleanup_disableNvlink:
        // Add P2P class back to class DB as memory partitioning is disabled
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            gpuAddClassToClassDBByClassId(pGpu, NV50_P2P));

        //
        // On Nvswitch based systems, resume the gpu fabric probe
        // request on nvlink inband to register the GPU with the nvswitch fabric
        //
        if (pGpu->pGpuFabricProbeInfoKernel != NULL)
        {
            NV_ASSERT_OK(gpuFabricProbeResume(pGpu->pGpuFabricProbeInfoKernel));
        }

cleanup_createPartitionCheck:
        if (!bUnload)
        {
            // Init top level scrubber if it existed before
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                memmgrInitInternalChannels(pGpu, pMemoryManager));
        }
cleanup_destroyInternalChannels:

        // Set kmigmgr state to reflect MIG disabled while reconfiguring for NON-MIG
        pKernelMIGManager->bMIGEnabled = NV_FALSE;

        if (!bUnload)
        {
            KernelGraphics *pKGr = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);

            // Since MIG is now disabled, reconfigure GR0 in legacy mode
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                kgraphicsLoadStaticInfo(pGpu, pKGr, KMIGMGR_SWIZZID_INVALID));
            NV_ASSERT_OK(
                kmigmgrRestoreWatchdog(pGpu, pKernelMIGManager));
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                kgraphicsCreateGoldenImageChannel(pGpu, pKGr));
        }

        //
        // Restore previous kmigmgr MIG state. kmigmgrSetMIGState should not
        // permanently modify bMIGEnabled. Restore the value to whatever was
        // present on entry to this function.
        //
        pKernelMIGManager->bMIGEnabled = bPrevMIGState;

cleanup_disableWatchdog:
        goto done;
    }

done:
    //
    // Restore previous kmigmgr MIG state. kmigmgrSetMIGState should not
    // permanently modify bMIGEnabled. Restore the value to whatever was
    // present on entry to this function.
    //
    pKernelMIGManager->bMIGEnabled = bPrevMIGState;
    return rmStatus;
}

/*!
 * @brief   Function to create or destroy GPU instance
 *
 * @param[IN]     pGpu
 * @param[IN]     pKernelMIGManager
 * @param[IN]     swizzId             SwizzId allocated for this gpu instance
 * @param[IN]     pUuid               UUID of the GPU instance
 * @param[IN]     params              Gpu instance creation parameters
 * @param[IN]     bValid              Flag stating if gpu instance is created or destroyed
 * @param[IN]     bCreateCap          Flag stating if MIG capabilities needs to be created
 */
NV_STATUS
kmigmgrCreateGPUInstance_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId,
    NvU8 *pUuid,
    KMIGMGR_CREATE_GPU_INSTANCE_PARAMS params,
    NvBool bValid,
    NvBool bCreateCap
)
{
    NV_STATUS rmStatus = NV_OK;

    // If making a gpu instance valid, memory should be allocated accordingly
    if (bValid)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        RM_ENGINE_TYPE rmEngineType;

        //
        // Determine SwizzID for this gpu instance. If this isn't a restore, this
        // has already been determined by physical RM.
        //
        if (params.type == KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_RESTORE)
        {
            NV_ASSERT_OR_RETURN(!kmigmgrIsSwizzIdInUse(pGpu, pKernelMIGManager, swizzId),
                                NV_ERR_INVALID_STATE);
        }

        //
        // HACK: GSP-RM updated the PCE-LCE mappings while setting MIG state.
        //       The Client-RM hasn't had an opportunity to refresh its mappings
        //       yet until the first gpu instance creation, so do it now.
        //
        if ((pKernelMIGManager->swizzIdInUseMask == 0x0) && IS_GSP_CLIENT(pGpu))
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrEnableAllLCEs(pGpu, pKernelMIGManager, NV_TRUE), invalidate);
        }

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrSetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, pUuid, params), invalidate);

        // Mark swizzId as "in-use" in cached mask
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrSetSwizzIdInUse(pGpu, pKernelMIGManager, swizzId), invalidate);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance), invalidate);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrAllocGPUInstanceHandles(pGpu, swizzId, pKernelMIGGpuInstance), invalidate);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrInitGPUInstanceBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance), invalidate);

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
            kmigmgrCreateGPUInstanceRunlists_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance), invalidate);

        NV_ASSERT_OK_OR_GOTO(rmStatus,
            kmemsysInitMIGMemoryPartitionTable_HAL(pGpu, pKernelMemorySystem), invalidate);

        FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
        {
            NvU32 engineIdx;
            KernelGraphics *pKernelGraphics;
            RM_ENGINE_TYPE localEngineType;

            if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
                continue;

            engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);
            pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, engineIdx);

            NV_ASSERT_OK_OR_GOTO(rmStatus,
                kmigmgrGetGlobalToLocalEngineType(pGpu,
                                                  pKernelMIGManager,
                                                  kmigmgrMakeGIReference(pKernelMIGGpuInstance),
                                                  rmEngineType,
                                                  &localEngineType),
                invalidate);

            fecsSetRoutingInfo(pGpu,
                               pKernelGraphics,
                               pKernelMIGGpuInstance->instanceHandles.hClient,
                               pKernelMIGGpuInstance->instanceHandles.hSubdevice,
                               RM_ENGINE_TYPE_GR_IDX(localEngineType));
        }
        FOR_EACH_IN_BITVECTOR_END();

        // Init gpu instance pool for page table mem
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrInitGPUInstancePool(pGpu, pKernelMIGManager, pKernelMIGGpuInstance), invalidate);

        // Init gpu instance scrubber
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            kmigmgrInitGPUInstanceScrubber(pGpu, pKernelMIGManager, pKernelMIGGpuInstance), invalidate);

        //
        // Only partitions in which VGPU guests are booted require changing
        // engine interrupt vectors to deterministic values for migration.
        //
        if (IS_GSP_CLIENT(pGpu) && gpuIsSriovEnabled(pGpu))
        {
            //
            // Making changes to MIG partition structure reassigns engine interrupts
            // of the engines assigned to a partition. This is done so that a guest
            // VM sees the same vectors across a migration (suspend/resume / cloning
            // / resuming from a snapshot, etc.).
            //
            // Physical RM will update the interrupt table to reflect the actual
            // changes made to engine interrupts.
            //
            // When running in GSP offload mode, KernelRM must re-fetch the
            // interrupt table on every change to the MIG partitioning layout.
            //
            // The changes to partitions as well as preparing an existing partition
            // to boot a VM are done together on Physical RM in
            // NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK.
            //
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                intrRefetchInterruptTable_HAL(pGpu, GPU_GET_INTR(pGpu)),
                invalidate);
        }

        //
        // Register gpu instance with the capability framework only if it explicitly
        // requested. Otherwise, we rely on the persistent state.
        //
        if (bCreateCap)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                osRmCapRegisterSmcPartition(pGpu->pOsRmCaps, &pKernelMIGGpuInstance->pOsRmCaps,
                                            pKernelMIGGpuInstance->swizzId), invalidate);
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Invalidating swizzId - %d.\n", swizzId);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrInvalidateGPUInstance(pGpu, pKernelMIGManager, swizzId, NV_FALSE));
    }

    return rmStatus;

invalidate:
    kmigmgrInvalidateGPUInstance(pGpu, pKernelMIGManager, swizzId, NV_FALSE);

    return rmStatus;
}

/*
 * @brief   Init per-gpu instance memory pool so that memory for client page tables
 *          can be allocated from this memory pool
 */
NV_STATUS
kmigmgrInitGPUInstancePool_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT *pFmt = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    NvU32 version;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!memmgrIsPmaInitialized(pMemoryManager) ||
        !memmgrAreClientPageTablesPmaManaged(pMemoryManager))
    {
        return NV_OK;
    }

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance->swizzId))
        return NV_OK;

    NV_ASSERT_OR_RETURN(pFmt != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance->pMemoryPartitionHeap != NULL, NV_ERR_INVALID_STATE);

    version = ((pFmt->version == GMMU_FMT_VERSION_1) ? POOL_CONFIG_GMMU_FMT_1 : POOL_CONFIG_GMMU_FMT_2);

    NV_ASSERT_OK_OR_RETURN(
        rmMemPoolSetup((void*)&pKernelMIGGpuInstance->pMemoryPartitionHeap->pmaObject,
                       &pKernelMIGGpuInstance->pPageTableMemPool, version));

    // Allocate the pool in CPR in case of Confidential Compute
    if (gpuIsCCFeatureEnabled(pGpu))
    {
        rmMemPoolAllocateProtectedMemory(pKernelMIGGpuInstance->pPageTableMemPool, NV_TRUE);
    }

    return NV_OK;
}

/*
 * @brief   Initializes ctx buf pools for runlist buffer and GR global ctx buffers
 *          for engines that belong to this gpu instance.
 */
NV_STATUS
kmigmgrInitGPUInstanceBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    Heap *pHeap;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32 pmaConfig = PMA_QUERY_NUMA_ONLINED;
    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);
    pHeap = pKernelMIGGpuInstance->pMemoryPartitionHeap;
    NV_ASSERT_OR_RETURN(pHeap != NULL, NV_ERR_INVALID_STATE);

    if (!ctxBufPoolIsSupported(pGpu))
        return NV_OK;

    //
    // We have to drop GPU lock before making allocations from PMA
    // as RM allocations can trigger UVM evictions.
    // However, in this case we can skip dropping GPU lock as gpu instance PMA
    // isn't visible to UVM yet.
    // This is just a sanity check to make sure this assumption is correct and
    // allocation from PMA cannot trigger UVM evictions.
    //
    // When FB memory is onlined as NUMA node, kernel can directly alloc FB memory
    // and hence free memory can not be expected to be same as total memory.
    //
    if (memmgrIsPmaInitialized(pMemoryManager) &&
        (pmaQueryConfigs(&pHeap->pmaObject, &pmaConfig) == NV_OK) &&
        !(pmaConfig & PMA_QUERY_NUMA_ONLINED))
    {
        NvU64 freeSpace, totalSpace;
        pmaGetFreeMemory(&pHeap->pmaObject, &freeSpace);
        pmaGetTotalMemory(&pHeap->pmaObject, &totalSpace);
        if (freeSpace != totalSpace)
        {
            NV_PRINTF(LEVEL_ERROR, "Assumption that PMA is empty at this time is broken\n");
            NV_PRINTF(LEVEL_ERROR, "free space = 0x%llx bytes total space = 0x%llx bytes\n",
                freeSpace, totalSpace);
            NV_PRINTF(LEVEL_ERROR, "This means PMA allocations may trigger UVM evictions at this point causing deadlocks!\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kmigmgrInitGPUInstanceRunlistBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance));
    NV_ASSERT_OK_OR_RETURN(kmigmgrInitGPUInstanceGrBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance));
    return NV_OK;
}

/*
 * Initializes the runlist buffer pools for engines that belong to this gpu instance
 * Also reserves memory for runlist buffers into these pools.
 * later, runlists will be allocated from these pools.
 */
NV_STATUS
kmigmgrInitGPUInstanceRunlistBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    RM_ENGINE_TYPE    rmEngineType;
    KernelFifo       *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CTX_BUF_INFO      runlistBufInfo[NUM_BUFFERS_PER_RUNLIST] = {0};
    NvU64             rlSize;
    NvU64             rlAlign;
    NvU32             swizzId;
    NvU32             i;
    NvU32             runlistId;
    Heap             *pHeap;

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);
    swizzId = pKernelMIGGpuInstance->swizzId;
    pHeap = pKernelMIGGpuInstance->pMemoryPartitionHeap;
    NV_ASSERT_OR_RETURN(pHeap != NULL, NV_ERR_INVALID_STATE);

    if (!kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, swizzId))
        return NV_OK;

    for (rmEngineType = 0; rmEngineType < RM_ENGINE_TYPE_LAST; rmEngineType++)
    {
        if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType) ||
            !kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, rmEngineType) ||
            !kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, kmigmgrMakeGIReference(pKernelMIGGpuInstance)))
        {
            continue;
        }

        // Get runlist ID for Engine type.
        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                        ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)rmEngineType,
                                                        ENGINE_INFO_TYPE_RUNLIST, &runlistId));

        //
        // ctx buf pools only support HW runlists today
        // we assume TSGs are supported for all runlists which is true for Ampere
        //
        for (i = 0; i < NUM_BUFFERS_PER_RUNLIST; i++)
        {
            NV_ASSERT_OK_OR_RETURN(kfifoGetRunlistBufInfo(pGpu, pKernelFifo, runlistId, NV_TRUE,
                                   0, &rlSize, &rlAlign));
            runlistBufInfo[i].size = rlSize;
            runlistBufInfo[i].align = rlAlign;
            runlistBufInfo[i].attr = RM_ATTR_PAGE_SIZE_DEFAULT;
            runlistBufInfo[i].bContig = NV_TRUE;
        }

        NV_ASSERT_OK_OR_RETURN(ctxBufPoolInit(pGpu, pHeap, &pKernelFifo->pRunlistBufPool[rmEngineType]));
        NV_ASSERT_OR_RETURN(pKernelFifo->pRunlistBufPool[rmEngineType] != NULL, NV_ERR_INVALID_STATE);

        //
        // Skip scrubber for runlist buffer alloctions since gpu instance scrubber is not setup yet
        // and it will be destroyed before deleting the runlist buffer pool.
        //
        ctxBufPoolSetScrubSkip(pKernelFifo->pRunlistBufPool[rmEngineType], NV_TRUE);
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolReserve(pGpu, pKernelFifo->pRunlistBufPool[rmEngineType], &runlistBufInfo[0], NUM_BUFFERS_PER_RUNLIST));
    }

    return NV_OK;
}

/*
 * @brief   Initializes gr buffer pools for all GR engines that belong to this gpu instance
 *          Also reserves memory for global GR buffers into these pools.
 */
NV_STATUS
kmigmgrInitGPUInstanceGrBufPools_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    GR_GLOBALCTX_BUFFER    bufId;
    NvU32                  bufCount;
    CTX_BUF_INFO           globalCtxBufInfo[GR_GLOBALCTX_BUFFER_COUNT];
    Heap                  *pHeap = NULL;
    NV_STATUS              rmStatus = NV_OK;
    RM_ENGINE_TYPE         rmEngineType;

    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);
    pHeap = pKernelMIGGpuInstance->pMemoryPartitionHeap;
    NV_ASSERT_OR_RETURN(pHeap != NULL, NV_ERR_INVALID_STATE);

    bufCount = 0;
    FOR_EACH_IN_ENUM(GR_GLOBALCTX_BUFFER, bufId)
    {
        if (kgrmgrIsGlobalCtxBufSupported(bufId, NV_FALSE))
        {
            const CTX_BUF_INFO *pBufInfo = kgrmgrGetGlobalCtxBufInfo(pGpu, pKernelGraphicsManager, bufId);
            NV_ASSERT_OR_RETURN(pBufInfo != NULL, NV_ERR_INVALID_STATE);

            globalCtxBufInfo[bufCount] = *pBufInfo;

            if ((bufId == GR_GLOBALCTX_BUFFER_FECS_EVENT) || (bufId == GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP))
            {
                globalCtxBufInfo[bufCount].bContig = NV_TRUE;
            }
            else if ((bufId == GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP) || (bufId == GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP))
            {
                globalCtxBufInfo[bufCount].bContig = gpuIsClientRmAllocatedCtxBufferEnabled(pGpu);
            }
            kgrmgrSetGlobalCtxBufInfo(pGpu, pKernelGraphicsManager, bufId,
                                      globalCtxBufInfo[bufCount].size,
                                      globalCtxBufInfo[bufCount].align,
                                      globalCtxBufInfo[bufCount].attr,
                                      globalCtxBufInfo[bufCount].bContig);
            bufCount++;
        }
    }
    FOR_EACH_IN_ENUM_END;

    FOR_EACH_IN_BITVECTOR(&pKernelMIGGpuInstance->resourceAllocation.engines, rmEngineType)
    {
        NvU32 engineIdx;
        KernelGraphics *pKernelGraphics;
        CTX_BUF_POOL_INFO *pGrCtxBufPool;

        if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
            continue;

        engineIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);
        pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, engineIdx);

        NV_ASSERT_OK_OR_GOTO(rmStatus,
            kgraphicsInitCtxBufPool(pGpu, pKernelGraphics, pHeap),
            failed);

        pGrCtxBufPool = kgraphicsGetCtxBufPool(pGpu, pKernelGraphics);

        if (pGrCtxBufPool == NULL)
        {
            rmStatus = NV_ERR_INVALID_STATE;
            goto failed;
        }

        //
        // Skip scrubber for GR buffer alloctions since gpu instance scrubber is not setup yet
        // and it will be destroyed before deleting the GR buffer pool.
        //
        ctxBufPoolSetScrubSkip(pGrCtxBufPool, NV_TRUE);
        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            ctxBufPoolReserve(pGpu, pGrCtxBufPool, &globalCtxBufInfo[0], bufCount),
            failed);
    }
    FOR_EACH_IN_BITVECTOR_END();

    return NV_OK;

failed:
    kmigmgrDestroyGPUInstanceGrBufPools(pGpu, pKernelMIGManager, pKernelMIGGpuInstance);
    return rmStatus;
}

/*!
 * @brief   Save MIG instance topology to persistence, if available.
 */
NV_STATUS
kmigmgrSaveToPersistence_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY *pTopologySave = NULL;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;
    NvU32 gpcIdx;
    NvU32 savedGIIdx;

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       gpumgrGetSystemMIGInstanceTopo(gpuGetDBDF(pGpu), &pTopologySave),
                       NV_OK);

    // Clear existing topology, if any.
    portMemSet(pTopologySave->saveGI, 0, sizeof(pTopologySave->saveGI));

    // Update the MIG enablement bit
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED))
    {
        gpumgrSetSystemMIGEnabled(gpuGetDBDF(pGpu), pKernelMIGManager->bMIGEnabled);
    }

    // If there are no instances then don't bother checking anything.
    NV_CHECK_OR_RETURN(LEVEL_SILENT, IS_MIG_IN_USE(pGpu), NV_OK);

    savedGIIdx = 0;
    FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
    {
        GPUMGR_SAVE_GPU_INSTANCE *pGPUInstanceSave = &pTopologySave->saveGI[savedGIIdx];

        pGPUInstanceSave->bValid = NV_TRUE;
        pGPUInstanceSave->swizzId = pKernelMIGGPUInstance->swizzId;
        pGPUInstanceSave->pOsRmCaps = pKernelMIGGPUInstance->pOsRmCaps;
        pGPUInstanceSave->giInfo.partitionFlags = pKernelMIGGPUInstance->partitionFlag;
        bitVectorToRaw(&pKernelMIGGPUInstance->resourceAllocation.engines,
                       pGPUInstanceSave->giInfo.enginesMask, sizeof(pGPUInstanceSave->giInfo.enginesMask));
        for (gpcIdx = 0; gpcIdx < pKernelMIGGPUInstance->resourceAllocation.gpcCount; ++gpcIdx)
        {
             pGPUInstanceSave->giInfo.gpcMask |= NVBIT32(pKernelMIGGPUInstance->resourceAllocation.gpcIds[gpcIdx]);
        }
        pGPUInstanceSave->giInfo.veidOffset = pKernelMIGGPUInstance->resourceAllocation.veidOffset;
        pGPUInstanceSave->giInfo.veidCount = pKernelMIGGPUInstance->resourceAllocation.veidCount;
        pGPUInstanceSave->giInfo.virtualGpcCount = pKernelMIGGPUInstance->resourceAllocation.virtualGpcCount;

        NV_ASSERT_OK_OR_RETURN(kmigmgrSaveComputeInstances(pGpu, pKernelMIGManager, pKernelMIGGPUInstance,
                                                           pGPUInstanceSave->saveCI));

        ++savedGIIdx;
    }
    FOR_EACH_VALID_GPU_INSTANCE_END();

    return NV_OK;
}

/*!
 * @brief  Update MIG CI config for CPU-RM if compute instance is created
 * by a guest and RPC is directly handled by GSP-RM
 */
NV_STATUS
kmigmgrUpdateCiConfigForVgpu_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 execPartCount,
    NvU32 *pExecPartId,
    NvU32 gfid,
    NvBool bDelete
)
{
    NvU32 i;
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    RsClient *pRsClient;
    GPUInstanceSubscription *pGPUInstanceSubscription;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    Subdevice *pSubdevice;

    if (!(IS_GSP_CLIENT(pGpu) && IS_MIG_IN_USE(pGpu) &&
         IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(execPartCount <= NVC637_CTRL_MAX_EXEC_PARTITIONS,
                        NV_ERR_INVALID_ARGUMENT);

    // Get hostVgpuDevice from provided GFID and validate the subscription
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetHostVgpuDeviceFromGfid(pGpu->gpuId, gfid,
                                                                         &pKernelHostVgpuDevice));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, serverGetClientUnderLock(&g_resServ, pKernelHostVgpuDevice->hMigClient,
                                                                &pRsClient));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, subdeviceGetByInstance(pRsClient, pKernelHostVgpuDevice->hMigDevice, 0,
                                                              &pSubdevice));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
          gisubscriptionGetGPUInstanceSubscription(pRsClient, RES_GET_HANDLE(pSubdevice),
                                                   &pGPUInstanceSubscription));

    pKernelMIGGpuInstance = gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription);

    if (!bDelete)
    {
        NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS execPartExportParams;

        // Create the execution partition state on CPU
        for (i = 0; i < execPartCount; i++)
        {
            GPUMGR_SAVE_COMPUTE_INSTANCE save;
            KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS restore =
            {
                .type = KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE,
                .inst.restore.pComputeInstanceSave = &save,
            };

            portMemSet(&execPartExportParams, 0, sizeof(execPartExportParams));
            execPartExportParams.id = pExecPartId[i];

            // Retrieve the CI state created by GSP-RM, then restore it to CPU-RM
            NV_ASSERT_OK_OR_GOTO(status,
                pRmApi->Control(pRmApi,
                                pKernelMIGGpuInstance->instanceHandles.hClient,
                                pKernelMIGGpuInstance->instanceHandles.hSubscription,
                                NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT,
                                &execPartExportParams,
                                sizeof(execPartExportParams)),
                failed);

            portMemSet(&save, 0, sizeof(save));
            save.bValid = NV_TRUE;
            save.id = pExecPartId[i];
            save.ciInfo = execPartExportParams.info;

            NV_ASSERT_OK_OR_GOTO(status,
                kmigmgrCreateComputeInstances_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                                  NV_FALSE, restore, &pExecPartId[i], NV_TRUE),
                failed);
        }
    }
    else
    {
        for (i = 0; i < execPartCount; i++)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                             pExecPartId[i], NV_FALSE));
        }
    }

    // Generate a CPU event to notify CPU clients of updated config
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SMC_CONFIG_UPDATE, NULL,
                            0, 0, 0);
    return NV_OK;

failed:
    // Send an RPC to GSP-RM to cleanup the state as we failed the call
    if (!bDelete)
    {
        NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS params;
        portMemSet(&params, 0, sizeof(params));
        params.execPartCount = 1;
        params.execPartId[0] = pExecPartId[i];
        NV_RM_RPC_CONTROL(pGpu, pKernelMIGGpuInstance->instanceHandles.hClient,
                          pKernelMIGGpuInstance->instanceHandles.hSubscription,
                          NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE, &params,
                           sizeof(params), status);
    }

    return status;
}

// Control call for getting active gpu instance Ids
NV_STATUS
subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU64             validSwizzIdMask;

    pParams->partitionCount = 0;

    ct_assert(NV2080_CTRL_GPU_MAX_PARTITIONS == KMIGMGR_MAX_GPU_INSTANCES);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if ((pKernelMIGManager == NULL) || !pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
    {
        NV_PRINTF(LEVEL_INFO, "MIG not supported on this GPU.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "MIG Mode has not been turned on.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // We can always have device_monitoring swizzID available in system even without
    // GPU split into MIG instances
    //
    pParams->swizzId[pParams->partitionCount++] = NVC637_DEVICE_LEVEL_SWIZZID;

    // Populate all active swizzIDs
    validSwizzIdMask = pKernelMIGManager->swizzIdInUseMask;
    while(validSwizzIdMask != 0x0)
    {
        pParams->swizzId[pParams->partitionCount] = portUtilCountTrailingZeros64(validSwizzIdMask);
        validSwizzIdMask &= ~NVBIT64(pParams->swizzId[pParams->partitionCount]);
        pParams->partitionCount++;
    }

    return NV_OK;
}

//
// Control call to determine the number of gpu instances of the given size which
// can still be created, given the current configuration of the GPU.
//
NV_STATUS
subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams
)
{
    NV_STATUS         status = NV_OK;
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvHandle          hClient = RES_GET_CLIENT_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_CHECK_OR_RETURN(LEVEL_INFO, kmigmgrIsMIGSupported(pGpu, pKernelMIGManager), NV_ERR_NOT_SUPPORTED);

    if (IS_VIRTUAL(pGpu))
    {
        // This is not supported in legacy MIG vGPU policy
        if (kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager))
            return NV_ERR_NOT_SUPPORTED;

        if (!pParams->bStaticInfo)
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

            NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

            // Only expose current capacity to admins or capable clients.
            if (!rmclientIsCapableOrAdminByHandle(hClient,
                                                  NV_RM_CAP_SYS_SMC_CONFIG,
                                                  pCallContext->secInfo.privLevel))
            {
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }

            if (!kmigmgrIsGPUInstanceCombinationValid_HAL(pGpu, pKernelMIGManager, pParams->partitionFlag) ||
                !FLD_TEST_DRF(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, _FULL, pParams->partitionFlag))
            {
                pParams->partitionCount = 0;
                pParams->availableSpansCount = 0;
            }
            else
            {
                if (IS_MIG_IN_USE(pGpu))
                {
                    pParams->partitionCount = 0;
                    pParams->availableSpansCount = 0;
                }
                else
                {
                    pParams->partitionCount = 1;
                    pParams->availableSpansCount = 1;
                    pParams->availableSpans[0].lo = NV_RANGE_EMPTY.lo;
                    pParams->availableSpans[0].hi = NV_RANGE_EMPTY.hi;
                }
            }
        }

        if (!kmigmgrIsGPUInstanceCombinationValid_HAL(pGpu, pKernelMIGManager, pParams->partitionFlag) ||
            !FLD_TEST_DRF(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, _FULL, pParams->partitionFlag))
        {
            pParams->totalPartitionCount = 0;
            pParams->totalSpansCount = 0;
        }
        else
        {
            pParams->totalPartitionCount = 1;
            pParams->totalSpansCount = 1;
            pParams->totalSpans[0].lo = NV_RANGE_EMPTY.lo;
            pParams->totalSpans[0].hi = NV_RANGE_EMPTY.hi;
        }

        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;

    return status;
}

//
// Control call to provide information about gpu instances which can be created on
// this GPU.
//
NV_STATUS
subdeviceCtrlCmdGpuDescribePartitions_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
    {
        NV_PRINTF(LEVEL_INFO, "MIG not supported on this GPU.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    return kmigmgrDescribeGPUInstances(pGpu, pKernelMIGManager, pParams);
}

//
// Control call to set the global partitioning mode for this GPU. This call may
// require a PF-FLR to be performed on the GPU before work may be submitted on
// the GPU.
//
NV_STATUS
subdeviceCtrlCmdGpuSetPartitioningMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pKernelMIGManager == NULL) || !kmigmgrIsMIGSupported(pGpu, pKernelMIGManager))
    {
        NV_PRINTF(LEVEL_INFO, "MIG not supported on this GPU.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_PARTITIONING_MODE,
                        pParams,
                        sizeof(*pParams)));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kmigmgrSetPartitioningMode(pGpu, pKernelMIGManager));

    return NV_OK;
}

/*!
 * @brief  Process a single request to create / destroy a gpu instance.
 *         Handles enabling / disabling MIG mode on entry/exit.
 */
static NV_STATUS
_kmigmgrProcessGPUInstanceEntry
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NV2080_CTRL_GPU_SET_PARTITION_INFO *pEntry
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams = portMemAllocNonPaged(sizeof(*pParams));
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams != NULL, NV_ERR_NO_MEMORY);

    pParams->partitionCount = 1;
    pParams->partitionInfo[0] = *pEntry;

    //
    // Mirrored GPU Instance Management:
    // 1: CPU enable MIG
    // 2: GSP enable MIG
    // 3: GSP create gpu instance
    // 4: CPU create gpu instance
    // 5: CPU delete gpu instance
    // 6: GSP delete gpu instance
    // 7: GSP disable MIG
    // 8: CPU disable MIG
    //

    // Step 1, 2: If this is the first gpu instance, enable MIG
    if (pEntry->bValid && (pKernelMIGManager->swizzIdInUseMask == 0x0))
    {
        NvBool bMemoryPartitioningRequested = kmigmgrIsMemoryPartitioningRequested_HAL(pGpu, pKernelMIGManager, pEntry->partitionFlag);

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrSetMIGState(pGpu, pKernelMIGManager, bMemoryPartitioningRequested, NV_TRUE, NV_FALSE),
            cleanup_params);
    }

    if (pEntry->bValid)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES,
                            pParams,
                            sizeof(*pParams)),
            cleanup_smc_state);
        pEntry->swizzId = pParams->partitionInfo[0].swizzId;
        portMemCopy(&pEntry->uuid, sizeof(pEntry->uuid),
                    &pParams->partitionInfo[0].uuid, sizeof(pParams->partitionInfo[0].uuid));
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS request =
        {
            .type = KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_REQUEST,
            .inst.request.partitionFlag = pEntry->partitionFlag,
            .inst.request.bUsePlacement =
                FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_PLACE_AT_SPAN, _ENABLE,
                             pEntry->partitionFlag),
            .inst.request.placement = rangeMake(pEntry->placement.lo, pEntry->placement.hi)
        };
        request.inst.request.partitionFlag = FLD_SET_DRF(2080_CTRL_GPU, _PARTITION_FLAG, _PLACE_AT_SPAN, _DISABLE,
                                                         request.inst.request.partitionFlag);

        // Step 3, 4, 5, 6: Create / delete gpu instance
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrCreateGPUInstance(pGpu, pKernelMIGManager, pEntry->swizzId, pEntry->uuid,
                                     request, pEntry->bValid, NV_TRUE /* create MIG capabilities */),
            cleanup_rpc);
    }

    if (!pEntry->bValid)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES,
                            pParams,
                            sizeof(*pParams)),
            cleanup_params);

        gpumgrCacheDestroyGpuInstance(pGpu, pEntry->swizzId);
    }
    else
    {
        gpumgrCacheCreateGpuInstance(pGpu, pEntry->swizzId);
    }

    // Step 7, 8: If this is the last gpu instance to go, disable MIG
    if (pKernelMIGManager->swizzIdInUseMask == 0x0)
    {
        NvBool bMemoryPartitioningNeeded = kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pParams->partitionInfo[0].swizzId);

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrSetMIGState(pGpu, pKernelMIGManager, bMemoryPartitioningNeeded, NV_FALSE, NV_FALSE),
            cleanup_params);
    }

    portMemFree(pParams);
    return status;

cleanup_rpc:
    if (pEntry->bValid)
    {
        // Reuse the same RPC information we prepared earlier, but flip the bValid bit
        pParams->partitionInfo[0].bValid = NV_FALSE;
        NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                     pGpu->hInternalClient,
                                     pGpu->hInternalSubdevice,
                                     NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES,
                                     pParams,
                                     sizeof(*pParams)));
    }

cleanup_smc_state:
    if (pEntry->bValid && (pKernelMIGManager->swizzIdInUseMask == 0x0))
    {
        NvBool bMemoryPartitioningRequested = kmigmgrIsMemoryPartitioningRequested_HAL(pGpu, pKernelMIGManager, pEntry->partitionFlag);

        NV_ASSERT_OK(
            kmigmgrSetMIGState(pGpu, pKernelMIGManager, bMemoryPartitioningRequested, NV_FALSE, NV_FALSE));
    }

cleanup_params:
    portMemFree(pParams);
    return status;
}

/*!
 * @brief  Control call for dividing GPU into requested gpu instances
 *
 * @returns NV_OK if successful.
 *          NV_ERR_INVALID_ARGUMENT if parameter is not found
 *          NV_ERR_NOT_SUPPORTED if parameter is not supported
 *
 */
NV_STATUS
subdeviceCtrlCmdGpuSetPartitions_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams
)
{
    NV_STATUS         rmStatus = NV_OK;
    NvU32             i;
    NvU32             j;
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle          hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    CALL_CONTEXT     *pCallContext = resservGetTlsCallContext();

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (!rmclientIsCapableOrAdminByHandle(hClient,
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_ERROR, "Non-privileged context issued privileged cmd\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    NV_CHECK_OR_RETURN(LEVEL_INFO, IS_MIG_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);

    // Sanity checks
    if (pParams->partitionCount > KMIGMGR_MAX_GPU_INSTANCES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    else if (0 == pParams->partitionCount)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    for (i = 0; i < pParams->partitionCount; i++)
    {
        if (pParams->partitionInfo[i].bValid)
        {
            NvU32 partitionFlag = FLD_SET_DRF(2080_CTRL_GPU, _PARTITION_FLAG, _PLACE_AT_SPAN, _DISABLE,
                                              pParams->partitionInfo[i].partitionFlag);
            NV_CHECK_OR_RETURN(LEVEL_ERROR,
                kmigmgrIsGPUInstanceCombinationValid_HAL(pGpu, pKernelMIGManager, partitionFlag),
                NV_ERR_NOT_SUPPORTED);
        }
    }

    // This is not supported in vGPU
    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    for (i = 0; i < pParams->partitionCount; i++)
    {
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
            _kmigmgrProcessGPUInstanceEntry(pGpu, pKernelMIGManager, &pParams->partitionInfo[i]),
            cleanup);
    }

    //
    // Generate a subdevice event stating something has changed in GPU instance
    // config. Clients currently do not care about changes and their scope
    //
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SMC_CONFIG_UPDATE, NULL, 0, 0, 0);

    return rmStatus;

cleanup:
    // Invalidate gpu instances which has been created
    for (j = 0; j < i; j++)
    {
        pParams->partitionInfo[j].bValid = !pParams->partitionInfo[j].bValid;
        NV_ASSERT_OK(
            _kmigmgrProcessGPUInstanceEntry(pGpu, pKernelMIGManager, &pParams->partitionInfo[j]));
        pParams->partitionInfo[j].bValid = !pParams->partitionInfo[j].bValid;
    }

    return rmStatus;
}

// Control call for getting specific gpu instance info
NV_STATUS
subdeviceCtrlCmdGpuGetPartitions_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams
)
{
    NV_STATUS                             rmStatus = NV_OK;
    NvU32                                 i;
    OBJGPU                               *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager                     *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    MIG_INSTANCE_REF                      ref;
    NvU64                                 validSwizzIdMask;
    NvHandle                              hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pRpcParams = NULL;

    ct_assert(NV2080_CTRL_GPU_MAX_PARTITIONS == KMIGMGR_MAX_GPU_INSTANCES);
    ct_assert(NV2080_CTRL_GPU_MAX_GPC_PER_SMC == KGRMGR_MAX_GPC);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    pRpcParams = portMemAllocNonPaged(sizeof(*pRpcParams));
    NV_CHECK_OR_RETURN(LEVEL_INFO, pRpcParams != NULL, NV_ERR_NO_MEMORY);

    *pRpcParams = *pParams;

    if (!IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);


        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_WARNING,
            pRmApi->Control(pRmApi,
                            pRmCtrlParams->hClient,
                            pRmCtrlParams->hObject,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_GET_GPU_INSTANCES,
                            pRpcParams,
                            sizeof(*pRpcParams)), done);
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
    {
        NV_PRINTF(LEVEL_INFO, "MIG not supported on this GPU.\n");
        rmStatus = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    if (!IS_MIG_ENABLED(pGpu))
        NV_PRINTF(LEVEL_INFO, "Entered MIG API with MIG disabled.\n");

    if (!IS_MIG_IN_USE(pGpu))
    {
        // set the valid gpu instance count to "0" and return
        pParams->validPartitionCount = 0;
        rmStatus = NV_OK;
        goto done;
    }

    // See if all gpu instances are requested and get info for all gpu instance
    if (pParams->bGetAllPartitionInfo)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

        NV_ASSERT_OR_ELSE(pCallContext != NULL,
                          rmStatus = NV_ERR_INVALID_STATE; goto done);

        if (!rmclientIsCapableOrAdminByHandle(hClient,
                                              NV_RM_CAP_SYS_SMC_CONFIG,
                                              pCallContext->secInfo.privLevel))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Non privileged client requesting global gpu instance info\n");
            rmStatus = NV_ERR_INSUFFICIENT_PERMISSIONS;
            goto done;
        }

        // Take all swizzId's for consideration
        validSwizzIdMask = pKernelMIGManager->swizzIdInUseMask;
    }
    else
    {
        rmStatus = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                   GPU_RES_GET_DEVICE(pSubdevice), &ref);
        if (rmStatus != NV_OK)
        {
            // set the valid gpu instance count to "0" and return
            pParams->validPartitionCount = 0;
            rmStatus = NV_OK;
            goto done;
        }

        validSwizzIdMask = NVBIT64(ref.pKernelMIGGpuInstance->swizzId);
    }

    pParams->validPartitionCount = 0;
    for (i = 0; i < KMIGMGR_MAX_GPU_INSTANCES; i++)
    {
        MIG_RESOURCE_ALLOCATION *pResourceAllocation;
        NvU32 swizzId = portUtilCountTrailingZeros64(validSwizzIdMask);
        NvU32 j;
        RM_ENGINE_TYPE rmEngineType;

        rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &ref.pKernelMIGGpuInstance);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to get gpu instance info for swizzId - %d\n",
                      swizzId);
            goto done;
        }

        pResourceAllocation = &ref.pKernelMIGGpuInstance->resourceAllocation;

        pParams->queryPartitionInfo[i].partitionFlag = ref.pKernelMIGGpuInstance->partitionFlag;
        pParams->queryPartitionInfo[i].swizzId = ref.pKernelMIGGpuInstance->swizzId;
        pParams->queryPartitionInfo[i].grEngCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_GR(0));
        pParams->queryPartitionInfo[i].smCount = ref.pKernelMIGGpuInstance->pProfile->smCount;
        pParams->queryPartitionInfo[i].veidCount = pResourceAllocation->veidCount;
        pParams->queryPartitionInfo[i].ceCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_COPY(0));

        pParams->queryPartitionInfo[i].gpcCount = pResourceAllocation->gpcCount;
        pParams->queryPartitionInfo[i].gfxGpcCount = pResourceAllocation->gfxGpcCount;
        pParams->queryPartitionInfo[i].virtualGpcCount = pResourceAllocation->virtualGpcCount;
        pParams->queryPartitionInfo[i].nvDecCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_NVDEC(0));
        pParams->queryPartitionInfo[i].nvEncCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_NVENC(0));
        pParams->queryPartitionInfo[i].nvJpgCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_NVJPG);
        pParams->queryPartitionInfo[i].nvOfaCount =
            kmigmgrCountEnginesOfType(&pResourceAllocation->engines, RM_ENGINE_TYPE_OFA(0));
        pParams->queryPartitionInfo[i].memSize = rangeLength(ref.pKernelMIGGpuInstance->memRange);
        pParams->queryPartitionInfo[i].validCTSIdMask = ref.pKernelMIGGpuInstance->pProfile->validCTSIdMask;
        pParams->queryPartitionInfo[i].validGfxCTSIdMask = ref.pKernelMIGGpuInstance->pProfile->validGfxCTSIdMask;
        pParams->queryPartitionInfo[i].bValid = NV_TRUE;

        if (IS_VIRTUAL(pGpu))
        {
            NV_RANGE span = NV_RANGE_EMPTY;
            VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
            NV_ASSERT_OR_ELSE(pVSI != NULL,
                              rmStatus = NV_ERR_OBJECT_NOT_FOUND; goto done);

            // VGPU doesn't support this
            pParams->queryPartitionInfo[i].bPartitionError = NV_FALSE;
            pParams->queryPartitionInfo[i].span.lo = span.lo;
            pParams->queryPartitionInfo[i].span.hi = span.hi;

            // Fill GPCs associated with every GR
            j = 0;
            FOR_EACH_IN_BITVECTOR(&pResourceAllocation->engines, rmEngineType)
            {
                if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
                    continue;

                pParams->queryPartitionInfo[i].gpcsPerGr[j] = pVSI->gpuPartitionInfo.gpcsPerGr[j];
                pParams->queryPartitionInfo[i].veidsPerGr[j] = pVSI->gpuPartitionInfo.veidsPerGr[j];
                pParams->queryPartitionInfo[i].virtualGpcsPerGr[j] = pVSI->gpuPartitionInfo.virtualGpcsPerGr[j];
                pParams->queryPartitionInfo[i].gfxGpcPerGr[j] = pVSI->gpuPartitionInfo.gfxGpcPerGr[j];

                j++;
            }
            FOR_EACH_IN_BITVECTOR_END();
        }
        else
        {
            NV_ASSERT_OR_ELSE(pRpcParams->queryPartitionInfo[i].bValid,
                              rmStatus = NV_ERR_INVALID_STATE; goto done);
            NV_ASSERT_OR_ELSE(
                pParams->queryPartitionInfo[i].swizzId == pRpcParams->queryPartitionInfo[i].swizzId,
                rmStatus = NV_ERR_INVALID_STATE; goto done);

            // Fill GPCs associated with every GR
            j = 0;
            FOR_EACH_IN_BITVECTOR(&pResourceAllocation->engines, rmEngineType)
            {
                if (!RM_ENGINE_TYPE_IS_GR(rmEngineType))
                    continue;

                pParams->queryPartitionInfo[i].gpcsPerGr[j] = pRpcParams->queryPartitionInfo[i].gpcsPerGr[j];
                pParams->queryPartitionInfo[i].gfxGpcPerGr[j] = pRpcParams->queryPartitionInfo[i].gfxGpcPerGr[j];
                pParams->queryPartitionInfo[i].veidsPerGr[j] = pRpcParams->queryPartitionInfo[i].veidsPerGr[j];
                pParams->queryPartitionInfo[i].virtualGpcsPerGr[j] = pRpcParams->queryPartitionInfo[i].virtualGpcsPerGr[j];

                j++;
            }
            FOR_EACH_IN_BITVECTOR_END();

            // Take the value provided by physical
            pParams->queryPartitionInfo[i].bPartitionError = pRpcParams->queryPartitionInfo[i].bPartitionError;
            pParams->queryPartitionInfo[i].span = pRpcParams->queryPartitionInfo[i].span;
        }

        ++pParams->validPartitionCount;

        validSwizzIdMask &= ~NVBIT64(swizzId);
        if (validSwizzIdMask == 0)
        {
            break;
        }
    }

done:
    portMemFree(pRpcParams);

    return rmStatus;
}

NV_STATUS
subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // No gpu instances to export
    if (!IS_MIG_IN_USE(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    // An unprivileged client has no use case for import/export
    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pSubdevice),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Guest RM does not support import/export
    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE,
                        pParams,
                        sizeof(*pParams)));

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
        return NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    // An unprivileged client has no use case for import/export
    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pSubdevice),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Guest RM does not support import/export
    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager) == 0x0)
    {
        NvBool bMemoryPartitioningNeeded = kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pParams->swizzId);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrSetMIGState(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu), bMemoryPartitioningNeeded, NV_TRUE, NV_FALSE));
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE,
                        pParams,
                        sizeof(*pParams)),
        cleanup_mig_state);

    if (IS_GSP_CLIENT(pGpu))
    {
        GPUMGR_SAVE_GPU_INSTANCE *pSave = portMemAllocNonPaged(sizeof(*pSave));
        NV_CHECK_OR_ELSE(LEVEL_ERROR,
            pSave != NULL,
            status = NV_ERR_NO_MEMORY;
            goto cleanup_mig_state;);

        KMIGMGR_CREATE_GPU_INSTANCE_PARAMS restore =
        {
            .type = KMIGMGR_CREATE_GPU_INSTANCE_PARAMS_TYPE_RESTORE,
            .inst.restore.pGPUInstanceSave = pSave,
        };
        pSave->bValid = NV_TRUE;
        pSave->swizzId = pParams->swizzId;
        pSave->pOsRmCaps = NULL;
        portMemCopy(&(pSave->giInfo), sizeof(pSave->giInfo), &pParams->info, sizeof(pParams->info));

        status = kmigmgrCreateGPUInstance(pGpu, pKernelMIGManager, pParams->swizzId, pParams->uuid,
                                          restore, NV_TRUE, NV_FALSE);

        portMemFree(pSave);
        NV_CHECK_OR_GOTO(LEVEL_ERROR, status == NV_OK, cleanup_rpc);
    }

    return NV_OK;

cleanup_rpc:
    {
        NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS params;

        portMemSet(&params, 0, sizeof(params));
        params.partitionCount = 1;
        params.partitionInfo[0].bValid = NV_FALSE;
        params.partitionInfo[0].swizzId = pParams->swizzId;

        NV_ASSERT_OK(
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES,
                            pParams,
                            sizeof(*pParams)));
    }

cleanup_mig_state:
    if (kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager) == 0x0)
    {
        NvBool bMemoryPartitioningNeeded = kmigmgrIsMemoryPartitioningNeeded_HAL(pGpu, pKernelMIGManager, pParams->swizzId);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrSetMIGState(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu), bMemoryPartitioningNeeded, NV_FALSE, NV_FALSE));
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdGpuGetComputeProfiles_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32 maxSmCount = NV_U32_MAX;
    NvU32 maxPhysicalSlotCount = NV_U32_MAX;
    MIG_INSTANCE_REF ref;
    NvU32 entryCount;
    NvU32 i;

    if (!IS_MIG_ENABLED(pGpu))
        return NV_ERR_INVALID_STATE;

    //
    // Grab MIG partition reference if available. The profile's SM count is used
    // to filter out compute profiles which wouldn't fit on the GI anyway. This
    // is not fatal as we still want to allow compute profiles for entire GPU view
    // to be queried without a specific GPU instance.
    //
    if (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref) == NV_OK)
    {
        maxSmCount = ref.pKernelMIGGpuInstance->pProfile->smCount;
        maxPhysicalSlotCount = ref.pKernelMIGGpuInstance->pProfile->virtualGpcCount;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo->pCIProfiles != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT(pStaticInfo->pCIProfiles->profileCount <= NV_ARRAY_ELEMENTS(pParams->profiles));

    entryCount = 0;
    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if ((pStaticInfo->pCIProfiles->profiles[i].smCount > maxSmCount) ||
            (pStaticInfo->pCIProfiles->profiles[i].physicalSlots > maxPhysicalSlotCount))
        {
            continue;
        }

        // If there are any duplicate compute profiles (i.e. same GPC and SM counts), skip broadcasting the
        // profile out.
        if ((entryCount > 0) &&
            (pParams->profiles[entryCount - 1].gfxGpcCount == pStaticInfo->pCIProfiles->profiles[i].gfxGpcCount) &&
            (pParams->profiles[entryCount - 1].gpcCount == pStaticInfo->pCIProfiles->profiles[i].gpcCount) &&
            (pParams->profiles[entryCount - 1].smCount == pStaticInfo->pCIProfiles->profiles[i].smCount))
        {
           continue;
        }

        pParams->profiles[entryCount].computeSize = pStaticInfo->pCIProfiles->profiles[i].computeSize;
        pParams->profiles[entryCount].gfxGpcCount = pStaticInfo->pCIProfiles->profiles[i].gfxGpcCount;
        pParams->profiles[entryCount].gpcCount    = pStaticInfo->pCIProfiles->profiles[i].physicalSlots;
        pParams->profiles[entryCount].smCount     = pStaticInfo->pCIProfiles->profiles[i].smCount;
        pParams->profiles[entryCount].veidCount   = pStaticInfo->pCIProfiles->profiles[i].veidCount;
        entryCount++;
    }
    pParams->profileCount = entryCount;
    return NV_OK;
}

/*!
 * @brief   Function to get the next computeSize flag either larger or smaller than
 *          the passed in flag.
 *
 * @param[IN]     bGetNextSmallest   Flag controlling whether the next largest or smallest
 *                                   compute size is returned
 * @param[IN]     computeSize        Base computeSize to lookup
 *
 * @return        Input is the original compute size
 *                  a.) If compute size input is KMIGMGR_COMPUTE_SIZE_INVALID, out is:
 *                     1.) NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL if bGetNextSmallest
 *                     2.) NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH if !bGetNextSmallest
 *                  b.) Else output is next largest/smallest based upon bGetNextSmallest
 */
NvU32
kmigmgrGetNextComputeSize_IMPL
(
    NvBool bGetNextSmallest,
    NvU32 computeSize
)
{
    const NvU32 computeSizeFlags[] =
    {
        KMIGMGR_COMPUTE_SIZE_INVALID,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH,
        KMIGMGR_COMPUTE_SIZE_INVALID
    };

    NV_ASSERT_OR_RETURN(computeSize <= KMIGMGR_COMPUTE_SIZE_INVALID, KMIGMGR_COMPUTE_SIZE_INVALID);

    if (computeSize == KMIGMGR_COMPUTE_SIZE_INVALID)
    {
        return (bGetNextSmallest) ? computeSizeFlags[1] : computeSizeFlags[NV_ARRAY_ELEMENTS(computeSizeFlags) - 2];
    }
    else
    {
        NvU32 i;

        for (i = 1; i < NV_ARRAY_ELEMENTS(computeSizeFlags) - 1; i++)
            if (computeSizeFlags[i] == computeSize)
                return (bGetNextSmallest) ? computeSizeFlags[i + 1] : computeSizeFlags[i - 1];

        // Requested input flag was not found
        return KMIGMGR_COMPUTE_SIZE_INVALID;
    }
}

/*!
 * @brief   Function to lookup a skyline for a given compute size
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   computeSize             Compute size to find skyline for
 * @param[OUT]  pSkyline                Pointer to NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO struct filled with
 *                                      a copy of the skyline info associated with the gpc count
 */
NV_STATUS
kmigmgrGetSkylineFromSize_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 computeSize,
    const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO **ppSkyline
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;

    NV_ASSERT_OR_RETURN(ppSkyline != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pSkylineInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);

    for (i = 0; i < pStaticInfo->pSkylineInfo->validEntries; i++)
    {
        if (pStaticInfo->pSkylineInfo->skylineTable[i].computeSizeFlag == computeSize)
        {
            *ppSkyline = &pStaticInfo->pSkylineInfo->skylineTable[i];
            return NV_OK;
        }
    }
    NV_PRINTF(LEVEL_INFO, "No skyline for with compute size %d\n", computeSize);
    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief   Function to lookup a compute profile for a given compute size
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   computeSize             Compute size to find skyline for
 * @param[OUT]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the gpc count
 */
NV_STATUS
kmigmgrGetComputeProfileFromSize_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 computeSize,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;

    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pCIProfiles != NULL, NV_ERR_OBJECT_NOT_FOUND);

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == computeSize)
        {
            portMemCopy(pProfile, sizeof(*pProfile), &pStaticInfo->pCIProfiles->profiles[i], sizeof(pStaticInfo->pCIProfiles->profiles[i]));
            return NV_OK;
        }
    }
    NV_PRINTF(LEVEL_INFO, "Found no Compute Profile for computeSize=%d\n", computeSize);
    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief   Function to lookup a compute profile for a given SM count
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   smCount                 SM Count to look up the associated compute profile
 * @param[OUT]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the SM count
 */
NV_STATUS
kmigmgrGetComputeProfileFromSmCount_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 smCount,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 indexMask;
    NvU32 index;
    NvU32 i;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, smCount != 0, NV_ERR_OBJECT_NOT_FOUND);
    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pCIProfiles != NULL, NV_ERR_OBJECT_NOT_FOUND);

    // Assertion for catching overflow of bitmask early
    NV_ASSERT_OR_RETURN(pStaticInfo->pCIProfiles->profileCount < 32, NV_ERR_INVALID_STATE);

    indexMask = 0x0;
    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if (pStaticInfo->pCIProfiles->profiles[i].smCount == smCount)
        {
            indexMask |= NVBIT32(i);
        }
    }

    index = portUtilCountTrailingZeros32(indexMask);
    if (indexMask == 0x0)
    {
        NV_PRINTF(LEVEL_ERROR, "Found no Compute Profile for smCount=%d\n", smCount);
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    else if (nvPopCount32(indexMask) > 1)
    {
        NvU32 prevSlots = pStaticInfo->pCIProfiles->profiles[index].physicalSlots;

        //
        // In the event there are multiple profiles with the same SM count, check to see if
        // the profiles have matching physical slots. If they don't, then just return and
        // let caller handle it.
        //
        FOR_EACH_INDEX_IN_MASK(32, i, indexMask)
        {
            if (prevSlots != pStaticInfo->pCIProfiles->profiles[i].physicalSlots)
            {
                NV_PRINTF(LEVEL_WARNING, "Profiles aliased. Falling back to GPC look-up\n");
                return NV_ERR_OBJECT_NOT_FOUND;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END
    }

    // If all the aliased profiles share physical slots, take the first index (i.e. the smallest) profile.
    portMemCopy(pProfile, sizeof(*pProfile), &pStaticInfo->pCIProfiles->profiles[index], sizeof(pStaticInfo->pCIProfiles->profiles[index]));
    return NV_OK;
}


/*!
 * @brief   Function to lookup a compute profile for a given GPC count. This function converts
 *          the provided gpcCount into a COMPUTE_SIZE partition flag which is then looked up
 *          in the static info compute profile list.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   gpcCount                GPC Count to look up the associated compute profile
 * @param[OUT]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the GPC count
 */
NV_STATUS
kmigmgrGetComputeProfileFromGpcCount_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpcCount,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvBool bReducedConfig = kmigmgrIsA100ReducedConfig(pGpu, pKernelMIGManager);
    NvU32 compSize;
    NvU32 maxGpc;
    NvU32 maxMIG;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pCIProfiles != NULL, NV_ERR_OBJECT_NOT_FOUND);

    maxMIG = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES].data;
    maxGpc = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;
    if (bReducedConfig)
        maxGpc /= 2;

    if ((gpcCount <= (maxGpc / 8)) && ((maxMIG / 8) > 0))
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH;
    else if ((gpcCount <= (maxGpc / 4)) && ((maxMIG / 4) > 0))
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER;
    else if ((gpcCount <= ((maxGpc / 2) - 1)) && (((maxMIG / 2) - 1) > 0))
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF;
    else if ((gpcCount <= (maxGpc / 2)) && ((maxMIG / 2) > 0))
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF;
    else
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL;

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == compSize)
        {
            if (pStaticInfo->pCIProfiles->profiles[i].gpcCount != gpcCount)
            {
                NV_PRINTF(LEVEL_INFO, "GPC count %d doesn't match compute size %d \n", gpcCount, compSize);
            }
            portMemCopy(pProfile, sizeof(*pProfile), &pStaticInfo->pCIProfiles->profiles[i], sizeof(pStaticInfo->pCIProfiles->profiles[i]));
            return NV_OK;
        }
    }

    NV_PRINTF(LEVEL_INFO, "Found no Compute Profile for gpcCount=%d\n", gpcCount);
    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief   Function to lookup a compute profile for a given cts ID
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   ctsId                   CTS ID to find compute profile for
 * @param[OUT]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the gpc count
 */
NV_STATUS
kmigmgrGetComputeProfileFromCTSId_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 ctsId,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 computeSize;

    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pCIProfiles != NULL, NV_ERR_OBJECT_NOT_FOUND);

    computeSize = kmigmgrGetComputeSizeFromCTSId(ctsId);
    return kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, computeSize, pProfile);
}

/*!
 * @brief   Function which returns a mask of CTS IDs which are not usable when the input CTS
 *          ID is in-use.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   ctsId              Input CTS ID to look-up invalid mask for
 * @param[OUT]  pInvalidCTSIdMask  Output mask of CTS IDs not useable with input ID
 */
NV_STATUS
kmigmgrGetInvalidCTSIdMask_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 ctsId,
    NvU64 *pInvalidCTSIdMask
)
{
    //
    // +---------------------------------------+
    // |                   0                   |
    // +-------------------+-------------------+
    // |         1         |         2         |
    // +-------------------+-------------------+
    // |         3         |         4         |
    // +---------+---------+---------+---------+
    // |    5    |    6    |    7    |    8    |
    // +---------+---------+---------+---------+
    // |    9    |    10   |    11   |    12   |
    // +----+----+----+----+----+----+----+----+
    // | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 |
    // +----+----+----+----+----+----+----+----+
    //
    NvU64 gpcSlot[KGRMGR_MAX_GR] =
    {
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(5) | NVBIT64(9)  | NVBIT64(13)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(5) | NVBIT64(9)  | NVBIT64(14)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(6) | NVBIT64(10) | NVBIT64(15)),
        (NVBIT64(0) | NVBIT64(1) | NVBIT64(3) | NVBIT64(6) | NVBIT64(10) | NVBIT64(16)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(4) | NVBIT64(7) | NVBIT64(11) | NVBIT64(17)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(4) | NVBIT64(7) | NVBIT64(11) | NVBIT64(18)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(4) | NVBIT64(8) | NVBIT64(12) | NVBIT64(19)),
        (NVBIT64(0) | NVBIT64(2) | NVBIT64(4) | NVBIT64(8) | NVBIT64(12) | NVBIT64(20))
    };
    NvU64 i;

    NV_ASSERT_OR_RETURN(NULL != pInvalidCTSIdMask, NV_ERR_INVALID_ARGUMENT);

    // All bits corresponding to nonexistent CTS ids are invalid
    *pInvalidCTSIdMask = DRF_SHIFTMASK64(63:KMIGMGR_MAX_GPU_CTSID);

    for (i = 0; i < KGRMGR_MAX_GR; ++i)
    {
        if (0 != (gpcSlot[i] & NVBIT64(ctsId)))
        {
            *pInvalidCTSIdMask |= gpcSlot[i];
        }
    }

    return NV_OK;
}

/*!
 * @brief Returns the range of possible CTS IDs for a given compute size flag
 */
NV_RANGE
kmigmgrComputeProfileSizeToCTSIdRange_IMPL
(
    NvU32 computeSize
)
{
    switch (computeSize)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
            return rangeMake(0,0);

        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF:
            return rangeMake(1,2);

        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
            return rangeMake(3,4);

        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER:
            return rangeMake(5,8);

        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER:
            return rangeMake(9,12);

        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH:
            return rangeMake(13,20);

        default:
            return NV_RANGE_EMPTY;
    }
}

/*!
 * @brief   Returns the span covered by the CTS ID
 */
NV_RANGE
kmigmgrCtsIdToSpan_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 ctsId
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvU32 spanLen;
    NV_RANGE ret;

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_RANGE_EMPTY);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, NV_RANGE_EMPTY);

    spanLen = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;

    if (kmigmgrIsA100ReducedConfig(pGpu, pKernelMIGManager))
        spanLen /= 2;

    switch (ctsId)
    {
        case 0:
            ret = rangeMake(0, spanLen - 1);
            break;
        case 1:
        case 3:
            ret = rangeMake(0, (spanLen/2) - 1);
            break;
        case 2:
        case 4:
            ret = rangeMake(spanLen/2, spanLen - 1);
            break;
        case 5:
        case 9:
            ret = rangeMake(0, (spanLen/4) - 1);
            break;
        case 6:
        case 10:
            ret = rangeMake((spanLen/4), (spanLen/2) - 1);
            break;
        case 7:
        case 11:
            ret = rangeMake((spanLen/2), (3*(spanLen/4)) - 1);
            break;
        case 8:
        case 12:
            ret = rangeMake((3*(spanLen/4)), spanLen - 1);
            break;
        case 13:
            ret = rangeMake(0, 0);
            break;
        case 14:
            ret = rangeMake(1, 1);
            break;
        case 15:
            ret = rangeMake(2, 2);
            break;
        case 16:
            ret = rangeMake(3, 3);
            break;
        case 17:
            ret = rangeMake(4, 4);
            break;
        case 18:
            ret = rangeMake(5, 5);
            break;
        case 19:
            ret = rangeMake(6, 6);
            break;
        case 20:
            ret = rangeMake(7, 7);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported CTS ID 0x%x\n", ctsId);
            DBG_BREAKPOINT();
            ret = NV_RANGE_EMPTY;
            break;
    }

    return ret;
}

/*!
 * @brief   Function to get next free CTS ID
 *
 * @param[IN]   pGpu
 * @param[IN]   pMIGManager
 * @param[OUT]  pCtsId                 CTS ID to be used if NV_OK returned
 * @param[IN]   globalValidCtsMask     Mask of CTS IDs which could possibly be allocated
 * @param[IN]   globalValidGfxCtsMask  Mask of Gfx capable CTS IDs which could possibly be allocated.
 *                                     Unused if bRestrictWithGfx is NV_FALSE
 * @param[IN]   ctsIdsInUseMask        Mask of CTS IDs currently in use
 * @param[IN]   profileSize            Profile size to get a CTS ID for
 * @param[IN]   bRestrictWithGfx       Whether to restrict the CTS ID chosen with Gfx info
 * @param[IN]   bGfxRequested          Whether Gfx info is requested. Unused if bRestrictWithGfx is NV_FALSE
 *
 * @return  Returns NV_STATUS
 *          NV_OK
 *          NV_ERR_INVALID_ARGUMENT        If un-supported partition size is
 *                                         requested
 *          NV_ERR_INSUFFICIENT_RESOURCES  If a CTS ID cannot be assigned
 */
NV_STATUS
kmigmgrGetFreeCTSId_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 *pCtsId,
    NvU64  globalValidCtsMask,
    NvU64  globalValidGfxCtsMask,
    NvU64  ctsIdsInUseMask,
    NvU32  profileSize,
    NvBool bRestrictWithGfx,
    NvBool bGfxRequested
)
{
    NV_RANGE ctsRange = kmigmgrComputeProfileSizeToCTSIdRange(profileSize);
    NvU64 validMask;
    NvU32 maxRemainingCapacity;
    NvU32 idealCTSId;
    NvU32 ctsId;
    NvU64 shadowValidCTSIdMask;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, !rangeIsEmpty(ctsRange), NV_ERR_INSUFFICIENT_RESOURCES);
    NV_ASSERT_OR_RETURN(pCtsId != NULL, NV_ERR_INVALID_ARGUMENT);

    // construct a mask of all non-floorswept ctsIds
    validMask = globalValidCtsMask;

    // Remove all ctsIds with slices currently in use
    FOR_EACH_INDEX_IN_MASK(64, ctsId, ctsIdsInUseMask)
    {
        NvU64 invalidMask;

        NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, ctsId, &invalidMask));

        validMask &= ~invalidMask;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // compute valid ctsIds for this request that can still be assigned
    shadowValidCTSIdMask = validMask;
    validMask &= DRF_SHIFTMASK64(ctsRange.hi:ctsRange.lo);

    // If there are no valid, open ctsIds, then bail here
    NV_CHECK_OR_RETURN(LEVEL_SILENT, validMask != 0x0, NV_ERR_INSUFFICIENT_RESOURCES);

    // Determine which available CTS ids will reduce the remaining capacity the least
    maxRemainingCapacity = 0;
    idealCTSId = portUtilCountTrailingZeros64(validMask);
    FOR_EACH_INDEX_IN_MASK(64, ctsId, validMask)
    {
        NvU64 invalidMask;
        NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, ctsId, &invalidMask));

        NvU32 remainingCapacity = nvPopCount64(shadowValidCTSIdMask & ~invalidMask);

        if (remainingCapacity > maxRemainingCapacity)
        {
            maxRemainingCapacity = remainingCapacity;
            idealCTSId = ctsId;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    *pCtsId = idealCTSId;

    return NV_OK;
}

/*! @brief  This function determines whether or not CTS alignment and slot requirements are needed.
 *          For PF, this is determined by whether or not a MINI_QUARTER skyline exists.
 */
NvBool
kmigmgrIsCTSAlignmentRequired_PF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    const NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO *pUnused;

    // CTS alignment is always required when a unique MINI_QUARTER is present
    return (kmigmgrGetSkylineFromSize(pGpu, pKernelMIGManager,
                NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER, &pUnused) == NV_OK);
}

/*! @brief  This function determines whether or not CTS alignment and slot requirements are needed.
 *          For VF, this is determined by whether or not a MINI_QUARTER compute profile exists.
 */
NvBool
kmigmgrIsCTSAlignmentRequired_VF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE unused;

    // CTS alignment is always required when a unique MINI_QUARTER is present
    return (kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager,
                NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER, &unused) == NV_OK);
}

/*!
 * @brief  Returns the computeSize flag of a given CTS ID
 */
NvU32
kmigmgrGetComputeSizeFromCTSId_IMPL
(
    NvU32 ctsId
)
{
    NvU32 computeSize = kmigmgrGetNextComputeSize(NV_TRUE, KMIGMGR_COMPUTE_SIZE_INVALID);

    while (computeSize != KMIGMGR_COMPUTE_SIZE_INVALID)
    {
        NV_RANGE range = kmigmgrComputeProfileSizeToCTSIdRange(computeSize);
        if ((range.lo <= ctsId) && (ctsId <= range.hi))
            break;
        computeSize = kmigmgrGetNextComputeSize(NV_TRUE, computeSize);
    }

    return computeSize;
}

/*!
 * @brief Returns Compute size of the smallest supported compute profile
 */
NvU32
kmigmgrSmallestComputeProfileSize_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 computeSize = kmigmgrGetNextComputeSize(NV_FALSE, KMIGMGR_COMPUTE_SIZE_INVALID);

    while (computeSize != KMIGMGR_COMPUTE_SIZE_INVALID)
    {
        NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE unused;
        if (kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, computeSize, &unused) == NV_OK)
            break;
        computeSize = kmigmgrGetNextComputeSize(NV_FALSE, computeSize);
    }

    return computeSize;
}

/*!
 * @brief  Sets/resets various CTS tracking structures in a GPU instance
 *         based upon whether bInUse is set
 *
 * @param[IN]   pKernelMIGGpuInstance
 * @param[IN]   ctsId                 CTS ID to be set/reset
 * @param[IN]   grId                  Global GR engine targeted for CTS ID
 * @param[IN]   bInUse                Flag indicating to set/reset cts tracking structures
 *
 */
void
kmigmgrSetCTSIdInUse_IMPL
(
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvU32 ctsId,
    NvU32 grId,
    NvBool bInUse
)
{
    NV_ASSERT_OR_RETURN_VOID(pKernelMIGGpuInstance != NULL);

    if (bInUse)
    {
        pKernelMIGGpuInstance->grCtsIdMap[grId] = ctsId;

        // Nothing to set in ctsIdInUseMask if KMIGMGR_CTSID_INVALID passed in
        NV_ASSERT_OR_RETURN_VOID(ctsId != KMIGMGR_CTSID_INVALID);

        pKernelMIGGpuInstance->ctsIdsInUseMask |= NVBIT64(ctsId);
    }
    else
    {
        //
        // Take CTS ID directly from gr mapping array to ensure both structures
        // remain in-sync.
        //
        ctsId = pKernelMIGGpuInstance->grCtsIdMap[grId];

        // Nothing to do if nothing was set
        NV_CHECK_OR_RETURN_VOID(LEVEL_WARNING, ctsId != KMIGMGR_CTSID_INVALID);

        pKernelMIGGpuInstance->ctsIdsInUseMask &= ~NVBIT64(ctsId);
        pKernelMIGGpuInstance->grCtsIdMap[grId] = KMIGMGR_CTSID_INVALID;
    }
}

/*!
 * @brief  Translates a spanStart and computeSize to the corresponding CTS ID.
 *         When an invalid compute size is passed in, this function will still
 *         return NV_OK, but populates an invalid CTS ID for use.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   computeSize        Compute size of CTS to get span offset of
 * @param[IN]   spanStart          spanStart requested
 * @param[OUT]   pCtsId             Output CTS ID in computeSize's range
 *
 */
NV_STATUS
kmigmgrXlateSpanStartToCTSId_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 computeSize,
    NvU32 spanStart,
    NvU32 *pCtsId
)
{
    NV_RANGE computeSizeIdRange;
    NvU64 computeSizeIdMask;
    NvU64 slotBasisMask;
    NvU32 slotsPerCTS;

    NV_ASSERT_OR_RETURN(pCtsId != NULL, NV_ERR_INVALID_ARGUMENT);

    //
    // Initialize output to invalid CTS ID, as KMIGMGR_COMPUTE_SIZE_INVALID may have been passed in
    // which is ok. It Is the callers rsponsibility to check for the CTS ID validitiy.
    //
    *pCtsId = KMIGMGR_CTSID_INVALID;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, computeSize != KMIGMGR_COMPUTE_SIZE_INVALID, NV_OK);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kmigmgrGetSlotBasisMask(pGpu, pKernelMIGManager, &slotBasisMask));

    // Validate that the spanStart does not exceed the basis slot count (which constitutes the acceptable span range)
    NV_CHECK_OR_RETURN(LEVEL_ERROR, spanStart < nvPopCount64(slotBasisMask), NV_ERR_INVALID_ARGUMENT);

    computeSizeIdRange = kmigmgrComputeProfileSizeToCTSIdRange(computeSize);

    // Grab the first CTS ID for computeSize, as it doesn't really mater which one we choose here.
    NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, computeSizeIdRange.lo, &computeSizeIdMask));

    // slots per CTSID is number of basis IDs marked in the invalid mask for this ID
    slotsPerCTS = nvPopCount64(computeSizeIdMask & slotBasisMask);

    if ((spanStart % slotsPerCTS) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Compute span start of %d is not aligned\n", spanStart);
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pCtsId = computeSizeIdRange.lo + (spanStart / slotsPerCTS);

    // The ID returned should be within the computeSize's range at this point
    NV_ASSERT((computeSizeIdRange.lo <= *pCtsId) && (*pCtsId <= computeSizeIdRange.hi));

    return NV_OK;
}

/*!
 * @brief  Retrievies the mask of CTS IDs which are used  to derive other properties
 *         such as spans, offsets, and capacities.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[OUT]  computeSize        Mask of all CTS IDs part of the profile slot basis
 */
NV_STATUS
kmigmgrGetSlotBasisMask_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU64 *pMask
)
{
    NV_RANGE slotBasisIdRange;
    NvU32 slotBasisComputeSize;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMask != NULL, NV_ERR_INVALID_ARGUMENT);

    slotBasisComputeSize = kmigmgrSmallestComputeProfileSize(pGpu, pKernelMIGManager);
    slotBasisIdRange = kmigmgrComputeProfileSizeToCTSIdRange(slotBasisComputeSize);

    NV_ASSERT_OR_RETURN(!rangeIsEmpty(slotBasisIdRange), NV_ERR_INVALID_STATE);

    *pMask = DRF_SHIFTMASK64(slotBasisIdRange.hi:slotBasisIdRange.lo);

    return NV_OK;
}

/*!
 * @brief  Translates a CTS ID to the corresponding spanStart of the CTS
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   ctsId
 *
 */
NvU32
kmigmgrGetSpanStartFromCTSId_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 ctsId
)
{
    NvU32 computeSize = kmigmgrGetComputeSizeFromCTSId(ctsId);
    NV_RANGE computeSizeIdRange;
    NvU64 computeSizeIdMask;
    NvU64 slotBasisMask;
    NvU32 slotsPerCTS;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, computeSize != KMIGMGR_COMPUTE_SIZE_INVALID, 0);

    computeSizeIdRange = kmigmgrComputeProfileSizeToCTSIdRange(computeSize);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kmigmgrGetSlotBasisMask(pGpu, pKernelMIGManager, &slotBasisMask));

    // Grab the first CTS ID for computeSize, as it doesn't really mater which one we choose here.
    NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, computeSizeIdRange.lo, &computeSizeIdMask));

    // slots per CTSID is number of basis IDs marked in the invalid mask for this ID
    slotsPerCTS = nvPopCount64(computeSizeIdMask & slotBasisMask);

    return (ctsId - computeSizeIdRange.lo) * slotsPerCTS;
}

/*!
 * @brief   Function checking whether the passed-in ctsId is available given the
 *          current states of ctsIdValidMask and ctsIdInUseMask
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   ctsIdValidMask  Valid CTS ID mask to compare against
 * @param[IN]   ctsIdInUseMask  Mask of CTS IDs which are marked as being used
 * @param[IN]   ctsid           CTS ID to check
 */
NvBool
kmigmgrIsCTSIdAvailable_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU64 ctsIdValidMask,
    NvU64 ctsIdInUseMask,
    NvU32 ctsId
)
{
    NvU64 invalidMask = 0x0;
    NvU32 i;

    FOR_EACH_INDEX_IN_MASK(64, i, ctsIdInUseMask)
    {
        NvU64 mask;

        NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, i, &mask));

        invalidMask |= mask;
    }
    FOR_EACH_INDEX_IN_MASK_END;
    return !!((ctsIdValidMask & ~invalidMask) & NVBIT64(ctsId));
}

#define _kmigmgrReadRegistryDword(pGpu, pKernelMIGManager, pRegParmStr, pData) \
            ((pKernelMIGManager)->bGlobalBootConfigUsed                        \
                ? osGetNvGlobalRegistryDword((pGpu), (pRegParmStr), (pData))   \
                : osReadRegistryDword((pGpu), (pRegParmStr), (pData)))

/*!
 * @brief   Read MIG boot config from the registry
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 */
static NvBool
_kmigmgrReadBootConfig
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    MIG_BOOT_CONFIG *pBootConfig
)
{
    NvU32 data32;
    NvBool bCIAssignmentPresent = NV_FALSE;

    ct_assert(NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI__SIZE == NV_ARRAY_ELEMENTS(pBootConfig->GIs));
    ct_assert(NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI__SIZE == NV_ARRAY_ELEMENTS(pBootConfig->CIs));
    ct_assert(NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT_GI__SIZE == NV_ARRAY_ELEMENTS(pBootConfig->CIs));

    // Read GPU instance config regkeys
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pBootConfig->GIs); i++)
    {
        char regStr[sizeof(NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI(0))];

        nvDbgSnprintf(regStr, sizeof(regStr), NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI(%u), i);
        if (_kmigmgrReadRegistryDword(pGpu, pKernelMIGManager, regStr, &data32) != NV_OK)
        {
            // Do not break here, so we could later check if there are any holes in the config
            continue;
        }

        pBootConfig->GIs[i].flags           = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_GI, _FLAGS, data32);
        pBootConfig->GIs[i].placement.lo    = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_GI, _PLACEMENT_LO, data32);
        pBootConfig->GIs[i].placement.hi    = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_GI, _PLACEMENT_HI, data32);

        if (DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_GI, _REQ_DEC_JPG_OFA, data32))
        {
            pBootConfig->GIs[i].flags |= DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE);
        }

        if (!rangeIsEmpty(pBootConfig->GIs[i].placement))
        {
            pBootConfig->GIs[i].flags |= DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _PLACE_AT_SPAN, _ENABLE);
        }

        NV_PRINTF(LEVEL_INFO, "Found a GI config regkey '%s': flags=0x%x, placementLo=%llu, placementHi=%llu\n",
                  regStr, pBootConfig->GIs[i].flags, pBootConfig->GIs[i].placement.lo,
                  pBootConfig->GIs[i].placement.hi);

        // Ensure that the specified flags are valid
        if (!kmigmgrIsGPUInstanceCombinationValid_HAL(pGpu, pKernelMIGManager, pBootConfig->GIs[i].flags))
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid partition flags 0x%x in %s\n", pBootConfig->GIs[i].flags, regStr);
            return NV_FALSE;
        }

        pBootConfig->GIs[i].bValid = NV_TRUE;
    }

    // Read compute instance assignment regkey
    if (_kmigmgrReadRegistryDword(pGpu, pKernelMIGManager, NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT, &data32) == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Found a CI assignment regkey '" NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT "': value=%x", data32);

        for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pBootConfig->CIs); i++)
        {
            pBootConfig->CIs[i].GIIdx = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT, _GI(i), data32);
        }

        bCIAssignmentPresent = NV_TRUE;
    }

    // Read compute instance config regkeys
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pBootConfig->CIs); i++)
    {
        char regStr[sizeof(NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI(0))];

        nvDbgSnprintf(regStr, sizeof(regStr), NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI(%u), i);
        if (_kmigmgrReadRegistryDword(pGpu, pKernelMIGManager, regStr, &data32) != NV_OK)
        {
            // If the CI entry is not specified, its assignment should be 0
            if (pBootConfig->CIs[i].GIIdx != 0)
            {
                NV_PRINTF(LEVEL_ERROR, "CI assignment for GI #%u must be 0\n", i);
                return NV_FALSE;
            }

            // Do not break here, so we could later check if there are any holes in the config
            continue;
        }

        // The CI assigment regkey must be present if there are any CIs being specified
        if (!bCIAssignmentPresent)
        {
            NV_PRINTF(LEVEL_ERROR, "Regkey '" NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT "' is missing\n");
            return NV_FALSE;
        }

        pBootConfig->CIs[i].flags       = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _FLAGS, data32);
        pBootConfig->CIs[i].spanStart   = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _PLACEMENT_LO, data32);
        pBootConfig->CIs[i].ceCount     = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _CES, data32);
        pBootConfig->CIs[i].nvDecCount  = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _DECS, data32);
        pBootConfig->CIs[i].nvEncCount  = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _ENCS, data32);
        pBootConfig->CIs[i].nvJpgCount  = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _JPGS, data32);
        pBootConfig->CIs[i].ofaCount    = DRF_VAL(_REG_STR_RM, _MIG_BOOT_CONFIGURATION_CI, _OFAS, data32);
        pBootConfig->CIs[i].bValid      = NV_TRUE;

        NV_PRINTF(LEVEL_INFO, "Found a CI config regkey '%s': flags=0x%x, placementLo=%u, CEs=%u, DECs=%u, ENCs=%u, JPGs=%u, OFAs=%u\n",
                  regStr, pBootConfig->CIs[i].flags, pBootConfig->CIs[i].spanStart, pBootConfig->CIs[i].ceCount,
                  pBootConfig->CIs[i].nvDecCount, pBootConfig->CIs[i].nvEncCount, pBootConfig->CIs[i].nvJpgCount,
                  pBootConfig->CIs[i].ofaCount);
    }

    // Check that the GPU instances config has no holes
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pBootConfig->GIs) - 1; i++)
    {
        if (!pBootConfig->GIs[i].bValid && pBootConfig->GIs[i + 1].bValid)
        {
            NV_PRINTF(LEVEL_ERROR, "Regkey '" NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI(%u) "' is missing\n", i);
            return NV_FALSE;
        }
    }

    // Check that the compute instances config has no holes
    for (NvU32 i = 0; i < NV_ARRAY_ELEMENTS(pBootConfig->CIs) - 1; i++)
    {
        if (!pBootConfig->CIs[i].bValid && pBootConfig->CIs[i + 1].bValid)
        {
            NV_PRINTF(LEVEL_ERROR, "Regkey '" NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI(%u) "' is missing\n", i);
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/*!
 * @brief   Create GPU and compute instances based on the boot config
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 */
NV_STATUS
kmigmgrRestoreFromBootConfig_PF
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NV_STATUS status = NV_OK;
    NvU32 GIIdx;
    NvU32 CIIdx;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV2080_CTRL_GPU_SET_PARTITION_INFO partitionInfo[NV2080_CTRL_GPU_MAX_PARTITIONS] = {0};
    MIG_BOOT_CONFIG bootConfig = {0};

    // Check that boot config is supported
    NV_CHECK_OR_RETURN(LEVEL_INFO, pKernelMIGManager->bBootConfigSupported, NV_OK);

    // If MIG isn't enabled, nothing to do
    NV_CHECK_OR_RETURN(LEVEL_INFO, IS_MIG_ENABLED(pGpu), NV_OK);

    // Read the boot config from the registry
    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       _kmigmgrReadBootConfig(pGpu, pKernelMIGManager, &bootConfig),
                       NV_ERR_INVALID_PARAMETER);

    // Create the GPU instances
    for (GIIdx = 0; GIIdx < NV_ARRAY_ELEMENTS(bootConfig.GIs); GIIdx++)
    {
        if (!bootConfig.GIs[GIIdx].bValid)
        {
            break;
        }

        partitionInfo[GIIdx].bValid         = NV_TRUE;
        partitionInfo[GIIdx].partitionFlag  = bootConfig.GIs[GIIdx].flags;
        partitionInfo[GIIdx].placement.lo   = bootConfig.GIs[GIIdx].placement.lo;
        partitionInfo[GIIdx].placement.hi   = bootConfig.GIs[GIIdx].placement.hi;

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            _kmigmgrProcessGPUInstanceEntry(pGpu, pKernelMIGManager, &partitionInfo[GIIdx]),
            cleanupGI);
    }

    // Create the compute instances
    for (CIIdx = 0; CIIdx < NV2080_CTRL_GPU_MAX_PARTITIONS; CIIdx++)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
        NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE computeProfileInfo = {0};
        NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS createParams = {0};

        if (!bootConfig.CIs[CIIdx].bValid)
        {
            break;
        }

        // Find what compute profile corresponds to the specified partition flags
        NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
            kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, bootConfig.CIs[CIIdx].flags, &computeProfileInfo),
            NV_PRINTF(LEVEL_ERROR, "Invalid partition flags 0x%x for CI #%u\n", bootConfig.CIs[CIIdx].flags, CIIdx);
            goto cleanupCI);

        NV_ASSERT_OK_OR_GOTO(status,
            kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                      partitionInfo[bootConfig.CIs[CIIdx].GIIdx].swizzId,
                                      &pKernelMIGGpuInstance),
            cleanupCI);

        createParams.execPartCount                  = 1;
        createParams.flags                          = DRF_DEF(C637_CTRL, _DMA_EXEC_PARTITIONS_CREATE_REQUEST, _AT_SPAN, _TRUE);
        createParams.execPartInfo[0].gpcCount       = computeProfileInfo.gpcCount;
        createParams.execPartInfo[0].smCount        = computeProfileInfo.smCount;
        createParams.execPartInfo[0].computeSize    = computeProfileInfo.computeSize;
        createParams.execPartInfo[0].ceCount        = bootConfig.CIs[CIIdx].ceCount;
        createParams.execPartInfo[0].nvEncCount     = bootConfig.CIs[CIIdx].nvEncCount;
        createParams.execPartInfo[0].nvDecCount     = bootConfig.CIs[CIIdx].nvDecCount;
        createParams.execPartInfo[0].nvJpgCount     = bootConfig.CIs[CIIdx].nvJpgCount;
        createParams.execPartInfo[0].ofaCount       = bootConfig.CIs[CIIdx].ofaCount;
        createParams.execPartInfo[0].spanStart      = bootConfig.CIs[CIIdx].spanStart;
        createParams.execPartInfo[0].sharedEngFlag  = NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_CE    |
                                                      NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVDEC |
                                                      NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVENC |
                                                      NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_OFA   |
                                                      NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVJPG;

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi,
                            pKernelMIGGpuInstance->instanceHandles.hClient,
                            pKernelMIGGpuInstance->instanceHandles.hSubscription,
                            NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE,
                            &createParams,
                            sizeof(createParams)),
            cleanupCI);
    }

    return NV_OK;

cleanupCI:
    // Remove all compute instances on the created GPU instances
    for (NvU32 i = 0; i < CIIdx; i++)
    {
        NvU32 tmpStatus;
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
        NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS getParams = {0};
        NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS deleteParams = {0};

        NV_ASSERT_OK_OR_ELSE(tmpStatus,
            kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, partitionInfo[i].swizzId, &pKernelMIGGpuInstance),
            continue);

        NV_ASSERT_OK_OR_ELSE(tmpStatus,
            pRmApi->Control(pRmApi,
                            pKernelMIGGpuInstance->instanceHandles.hClient,
                            pKernelMIGGpuInstance->instanceHandles.hSubscription,
                            NVC637_CTRL_CMD_EXEC_PARTITIONS_GET,
                            &getParams,
                            sizeof(getParams)),
            continue);

        deleteParams.execPartCount = getParams.execPartCount;
        portMemCopy(deleteParams.execPartId, sizeof(deleteParams.execPartId),
                    getParams.execPartId, sizeof(getParams.execPartId));

        NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                     pKernelMIGGpuInstance->instanceHandles.hClient,
                                     pKernelMIGGpuInstance->instanceHandles.hSubscription,
                                     NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE,
                                     &deleteParams,
                                     sizeof(deleteParams)));
    }

cleanupGI:
    // Invalidate the created GPU instances
    for (NvU32 i = 0; i < GIIdx; i++)
    {
        partitionInfo[i].bValid = NV_FALSE;
        NV_ASSERT_OK(_kmigmgrProcessGPUInstanceEntry(pGpu, pKernelMIGManager, &partitionInfo[i]));
    }

    return status;
}

/*!
 * Returns the Smallest Compute size (NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_*)
 * that is supported on this GPU.
 *
 * @param[in]   pGpu - OBJGPU
 * @param[in]   pKernelMIGManager - KernelMIGManager Object
 * @param[out]  pSmallestComputeSize - NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_* flag for the smallest supported size.
 *              This should be used only for MIG Supported GPUs.
 *
 * @return NV_OK on success,
 *         NV_ERR_NOT_SUPPORTED when the MaxMIG count is unsupported
 *         NV_ERR_INVALID_STATE When this is called before the internal data is initialized
 */
NV_STATUS
kmigmgrGetSmallestGpuInstanceSize_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 *pSmallestComputeSize
)
{
    NvU32 maxMIG;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, NV_ERR_INVALID_STATE);

    maxMIG = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES].data;
    switch (maxMIG)
    {
        case 8:
            *pSmallestComputeSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH;
            break;
        case 4:
            *pSmallestComputeSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER;
            break;
        case 2:
            *pSmallestComputeSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF;
            break;
        case 1:
            *pSmallestComputeSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "maxMIG(%d) is unsupported\n", maxMIG);
            return NV_ERR_NOT_SUPPORTED;
            break;
    }

    return NV_OK;
}
/*!
 * @brief   Function to lookup a compute profile for a sm or gpc count. This function
 *          has the caller provide a KERNEL_MIG_GPU_INSTANCE object, to catch requests
 *          which lie outside the normal bounds of the GPU instance.
 *          If the request is not at (or above) the GPU instances limit, then the CI
 *          profile will be selected by using the smCountRequest first, and only use
 *          gpcCount if the SM count look-up fails.
 *
 * @param[in]   pGpu
 * @param[in]   pKernelMIGManager
 * @param[in]   pKernelMIGGpuInstance   GPU instance for which the request was made
 * @param[in]   smCountRequest          SM Count to look up the associated compute profile
 * @param[in]   gpcCountRequest         GPC Count to look up the associated compute profile if SM lookup fails
 * @param[out]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the requested SM or GPC count.
 */
NV_STATUS
kmigmgrGetComputeProfileForRequest_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance,
    NvU32 smCountRequest,
    NvU32 gpcCountRequest,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    NvU32 computeSize;
    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_ERR_INVALID_ARGUMENT);

    // If SM Count is >= the GI's total size, use GI's computeSize as CI profile
    computeSize = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, pKernelMIGGpuInstance->partitionFlag);
    if (!IS_SILICON(pGpu) &&
        (pKernelMIGGpuInstance->resourceAllocation.smCount <= smCountRequest))
    {
        NV_PRINTF(LEVEL_INFO, "CI request is at GPU instance's limit. Using GPU instance's size: %d\n", computeSize);
        if (kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, computeSize, pProfile) == NV_OK)
            return NV_OK;
    }

    if (kmigmgrGetComputeProfileFromSmCount(pGpu, pKernelMIGManager, smCountRequest, pProfile) == NV_OK)
    {
        return NV_OK;
    }

    if (!IS_SILICON(pGpu) && (pKernelMIGGpuInstance->resourceAllocation.gpcCount == gpcCountRequest))
    {
        NV_PRINTF(LEVEL_INFO, "CI request is at GPU instance's limit. Using GPU instance's size: %d\n", computeSize);
        if (kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, computeSize, pProfile) == NV_OK)
            return NV_OK;
    }

    // Do basic GPC look-up as last resort if all the above failed
    return kmigmgrGetComputeProfileFromGpcCount_HAL(pGpu, pKernelMIGManager, gpcCountRequest, pProfile);
}

/*!
 * @brief   Returns if all allocated VEIDs in a GPU instance are contiguous and
 *          have no holes
 */
NvBool
kmigmgrIsPartitionVeidAllocationContiguous_IMPL
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NvU32 ciIdx;
    NvU64 instanceVeidMask = 0x0;
    NvU64 tempMask;
    NvU64 shift;

    // Sanity checks
    NV_ASSERT_OR_RETURN(pKernelMIGGpuInstance != NULL, NV_FALSE);

    for (ciIdx = 0; ciIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++ciIdx)
    {
        NvU32 veidStart;
        NvU32 veidEnd;
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance = &pKernelMIGGpuInstance->MIGComputeInstance[ciIdx];

        // Skip invalid compute instances
        if (!pMIGComputeInstance->bValid)
            continue;

          veidStart = pMIGComputeInstance->resourceAllocation.veidOffset;
          veidEnd = veidStart + pMIGComputeInstance->resourceAllocation.veidCount - 1;
          instanceVeidMask |= DRF_SHIFTMASK64(veidEnd:veidStart);
    }

    // If mask is fully populated or empty, no need to check
    if ((instanceVeidMask == 0) || (instanceVeidMask == NV_U64_MAX))
        return NV_TRUE;

    // Count the zeros at the end to align mask to always start with "1"
    shift = portUtilCountTrailingZeros64(instanceVeidMask);
    tempMask = (instanceVeidMask >> shift);

    //
    // If the above mask is contiguous "1s", an addition will result in next
    // pow-2, so simply check if we have only one-bit set or multiple.
    //
    return ONEBITSET(tempMask + 1);
}
