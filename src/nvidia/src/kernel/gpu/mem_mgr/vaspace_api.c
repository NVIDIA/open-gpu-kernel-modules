/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "os/os.h"
#include "core/locks.h"

#include "gpu/mem_mgr/vaspace_api.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "mem_mgr/virtual_mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/device/device.h"
#include "class/cl90f1.h"
#include "gpu/bus/kern_bus.h"

#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/virt_mem_mgr.h"

#include "vgpu/rpc.h"

/* ------------------ static and helper functions prototypes------------------*/
static NvU32 translateAllocFlagsToVASpaceFlags(NvU32 allocFlags, NvU32 *translatedFlags,
                                               NvBool bKernelClient, NvU32 gfid);
static NvU32 translatePageSizeToVASpaceFlags(NV_VASPACE_ALLOCATION_PARAMETERS *pNvVASpaceAllocParams);
static NV_STATUS _vaspaceapiManagePageLevelsForSplitVaSpace(OBJGPU *pGpu, Device *pDevice, NvU32 gpuMask, NvU32 flags, VASPACEAPI_MANAGE_PAGE_LEVELS_ACTION action);

NvBool
vaspaceapiCanCopy_IMPL(VaSpaceApi *pResource)
{
    return NV_TRUE;
}

NV_STATUS
vaspaceapiConstruct_IMPL
(
    VaSpaceApi                   *pVaspaceApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef                    *pResourceRef          = pCallContext->pResourceRef;
    NvHandle                          hParent               = pResourceRef->pParentRef->hResource;
    NV_VASPACE_ALLOCATION_PARAMETERS *pNvVASpaceAllocParams;
    NvU32                             allocFlags;
    NvU32                             flags                 = 0;
    NvU32                             gpuMask               = 0;
    OBJVMM                           *pVmm                  = SYS_GET_VMM(SYS_GET_INSTANCE());
    OBJVASPACE                       *pVAS                  = NULL;
    NvU64                             vasLimit              = 0;
    NvU64                             vaStartInternal       = 0;
    NvU64                             vaLimitInternal       = 0;
    NV_STATUS                         status                = NV_OK;
    OBJGPU                           *pGpu                  = GPU_RES_GET_GPU(pVaspaceApi);
    KernelBus                        *pKernelBus            = GPU_GET_KERNEL_BUS(pGpu);
    OBJGPUGRP                        *pGpuGrp               = GPU_RES_GET_GPUGRP(pVaspaceApi);
    Device                           *pDevice               = GPU_RES_GET_DEVICE(pVaspaceApi);
    NvBool                            bLockAcquired         = NV_FALSE;
    MemoryManager                    *pMemoryManager        = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager                 *pKernelMIGManager     = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU64                             originalVaBase;
    NvU64                             originalVaSize;
    NvU32                             gfid                  = 0;

    if (RS_IS_COPY_CTOR(pParams))
    {
        NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                     GPU_LOCK_GRP_ALL,
                                                     GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                                                     RM_LOCK_MODULES_MEM,
                                                     &gpuMask));

        status = vaspaceapiCopyConstruct_IMPL(pVaspaceApi, pCallContext, pParams);

        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

        return status;
    }

    if (gpuIsSriovEnabled(pGpu))
        NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    pNvVASpaceAllocParams = pParams->pAllocParams;
    allocFlags            = pNvVASpaceAllocParams->flags;

    // These input parameters get overwritten later but original values are needed
    originalVaBase = pNvVASpaceAllocParams->vaBase;
    originalVaSize = pNvVASpaceAllocParams->vaSize;

    // Translate & validate flags
    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          translateAllocFlagsToVASpaceFlags(allocFlags, &flags,
                                                            (pCallContext->secInfo.privLevel >=
                                                             RS_PRIV_LEVEL_KERNEL), gfid));

    //
    // Make sure this GPU is not already locked by this thread
    // Ideally this thread shouldn't have locked any GPU in the system but
    // checking this is sufficient as memory allocation from PMA requires
    // current GPU's lock not to be held
    //
    if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_ERROR, "VASpace alloc should be called without acquiring GPU lock\n");
        LOCK_ASSERT_AND_RETURN(0);
    }

    if (pNvVASpaceAllocParams->index == NV_VASPACE_ALLOCATION_INDEX_GPU_HOST)
    {
        gpuMask = NVBIT(pGpu->gpuInstance);
    }
    else
    {
        gpuMask = gpumgrGetGpuMask(pGpu);
    }

    status = _vaspaceapiManagePageLevelsForSplitVaSpace(pGpu, pDevice, gpuMask, flags, VASPACEAPI_MANAGE_PAGE_LEVELS_RESERVE);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    //--------------------------------------------------------------------------------
    // we acquire the GPU lock below.
    // From here down do not return early, use goto done
    //--------------------------------------------------------------------------------

    // Acquire the lock *only after* PMA is done allocating.
    status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM);
    NV_ASSERT_OR_GOTO(status == NV_OK, done);
    bLockAcquired = NV_TRUE;

    if ((pParams->externalClassId == FERMI_VASPACE_A) &&
        (pNvVASpaceAllocParams->index == NV_VASPACE_ALLOCATION_INDEX_GPU_HOST))
    {
        if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_ASSERT_OR_GOTO(0, done);
        }
    }

    // Per channel vaspace binding option has to be enabled!
    if (pDevice->vaMode == NV_DEVICE_ALLOCATION_VAMODE_SINGLE_VASPACE)
    {
        status = NV_ERR_INVALID_STATE;
        NV_ASSERT_OR_GOTO(0, done);
    }

    //
    // When MIG is enabled, ensure the client has a valid subscription.
    // While only split VA spaces should require this (and this will be
    // checked by _vaspaceapiManagePageLevelsForSplitVaSpace), check for
    // all platforms once the GPU lock is held, for consistency/to weed
    // out any cases where the client creates the VAS before subscribing
    // to the partition.
    //
    // Only check for cases where split VA space management is disabled -
    // other configurations (such as MODS SRIOV MIG testing) which use
    // split VA spaces have problems with this check.
    //
    if ((!NV_IS_MODS) && (pParams->externalClassId == FERMI_VASPACE_A) &&
        (!gpuIsSplitVasManagementServerClientRmEnabled(pGpu)) &&
        (pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
    {
        MIG_INSTANCE_REF ref;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref),
            done);
    }

    // RS-TODO - Move this to allocWithResServer
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {

        NvBool bSendRPC         = NV_TRUE;
        NvBool bIsFlaCapability = kbusIsFlaSupported(pKernelBus);

        if (gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
        {
            // If we are in SR-IOV heavy mode then we need to send RPC to host
            bSendRPC = NV_TRUE;

            // If FLA capability is disabled then do not send RPC.
            if ((pNvVASpaceAllocParams->index == NV_VASPACE_ALLOCATION_INDEX_GPU_FLA  ||
                 (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_FLA)) && !bIsFlaCapability)
            {
                bSendRPC = NV_FALSE;
            }
        }
        else
        {
            if (IS_VIRTUAL_WITH_SRIOV(pGpu))
            {
                // In case of SR-IOV, the BAR1 and FLA is managed by the guest. So, no need
                // to communicate with the host for BAR1 and FLA VA.
                if (pNvVASpaceAllocParams->index == NV_VASPACE_ALLOCATION_INDEX_GPU_HOST)
                    bSendRPC = NV_FALSE;
            }

            if ((IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITH_SRIOV(pGpu)) &&
                ((pNvVASpaceAllocParams->index == NV_VASPACE_ALLOCATION_INDEX_GPU_FLA)  ||
                (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_FLA)))
                bSendRPC = NV_FALSE;
        }

        if (bSendRPC)
        {
            NV_RM_RPC_ALLOC_OBJECT(pGpu,
                                   pParams->hClient,
                                   pParams->hParent,
                                   pParams->hResource,
                                   pParams->externalClassId,
                                   pNvVASpaceAllocParams,
                                   sizeof(*pNvVASpaceAllocParams),
                                   status);
            if (status != NV_OK)
            {
                goto done;
            }
        }
    }

    if ((pParams->externalClassId == FERMI_VASPACE_A) &&
        (pNvVASpaceAllocParams->index != NV_VASPACE_ALLOCATION_INDEX_GPU_NEW))
    {
        switch (pNvVASpaceAllocParams->index)
        {
            case NV_VASPACE_ALLOCATION_INDEX_GPU_HOST:
                pVAS = kbusGetBar1VASpace_HAL(pGpu, pKernelBus);
                break;
            case NV_VASPACE_ALLOCATION_INDEX_GPU_GLOBAL:
                gpugrpGetGlobalVASpace(pGpuGrp, &pVAS);
                break;
            case NV_VASPACE_ALLOCATION_INDEX_GPU_DEVICE:
                if (vaspaceGetByHandleOrDeviceDefault(pCallContext->pClient, hParent, NV01_NULL_OBJECT, &pVAS) != NV_OK)
                {
                    status = NV_ERR_INVALID_STATE;
                    goto done;
                }
                break;
            case NV_VASPACE_ALLOCATION_INDEX_GPU_FLA:
                status = kbusGetFlaVaspace_HAL(pGpu, pKernelBus, &pVAS);
                //
                // special handling here, because FLA VAspace can fail
                // and tests can still run assuming FLA is not supported.
                // This should not happen, but if it happens RM should fail
                // grcefully. Filing bug for MODS to fix this. And then we can
                // remove this special handling.
                //
                if (pVAS == NULL)
                    goto done;
                break;
        }
        if (pVAS == NULL)
        {
            status = NV_ERR_INVALID_STATE;
            NV_ASSERT_OR_GOTO(0, done);
        }

        vaspaceIncRefCnt(pVAS);
    }
    else
    {
        // validate the big page size
        if (!((pNvVASpaceAllocParams->bigPageSize == NV_VASPACE_BIG_PAGE_SIZE_64K) ||
              (pNvVASpaceAllocParams->bigPageSize == NV_VASPACE_BIG_PAGE_SIZE_128K) ||
              (pNvVASpaceAllocParams->bigPageSize == 0)))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_ASSERT_OR_GOTO(0, done);
        }

        //
        // Guest page tables are allocated in guest subheap only
        // for non SRIOV guests. For SR-IOV guests page table go
        // into subheap only if the guest does not manage VA.
        //
        if (!gpuIsSplitVasManagementServerClientRmEnabled(pGpu) ||
            !IS_VIRTUAL(pGpu))
        {
            flags |= VASPACE_FLAGS_ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR;
        }

        // Apply ATS restrictions
        if (flags & VASPACE_FLAGS_ENABLE_ATS)
        {
            if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED))
            {
                status = NV_ERR_NOT_SUPPORTED;
                NV_ASSERT_OR_GOTO(0, done);
            }
        }

        if ((pMemoryManager != NULL) &&
            memmgrIsPmaInitialized(pMemoryManager) &&
            memmgrAreClientPageTablesPmaManaged(pMemoryManager) &&
            !(allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_EXTERNALLY_OWNED) &&
            !(allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_FLA) &&
            !(allocFlags & NV_VASPACE_ALLOCATION_FLAGS_PTETABLE_HEAP_MANAGED))
        {
            flags |= VASPACE_FLAGS_PTETABLE_PMA_MANAGED;
        }

        // Get flags for the requested big page size
        flags |= translatePageSizeToVASpaceFlags(pNvVASpaceAllocParams);

        if (originalVaSize != 0)
        {
            vasLimit = originalVaBase + originalVaSize - 1;
            if (vasLimit < originalVaBase)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Integer overflow !!! Invalid parameters for vaBase:%llx, vaSize:%llx\n",
                          originalVaBase,
                          originalVaSize);
                status = NV_ERR_INVALID_ARGUMENT;
                NV_ASSERT_OR_GOTO(0, done);
            }
        }

        //
        // Bug 3610538 For unlinked SLI, clients want to restrict internal buffers to
        // Internal VA range. setting internal va range to match what we use for
        // windows.
        //
        if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_VA_INTERNAL_LIMIT)
        {
            vaStartInternal = SPLIT_VAS_SERVER_RM_MANAGED_VA_START;
            vaLimitInternal = SPLIT_VAS_SERVER_RM_MANAGED_VA_START +
                              SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE - 1;
        }

        // Finally call the factory
        status = vmmCreateVaspace(pVmm, pParams->externalClassId,
                                  pNvVASpaceAllocParams->index,
                                  gpuMask,
                                  originalVaBase,
                                  vasLimit,
                                  vaStartInternal,
                                  vaLimitInternal,
                                  NULL,
                                  flags,
                                  &pVAS);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not construct VA space. Status %x\n", status);
            pVAS = NULL;
            goto done;
        }
    }

    pVaspaceApi->pVASpace = pVAS;

    NV_PRINTF(LEVEL_INFO,
              "Created vaspaceapi 0x%x, hParent 0x%x, device 0x%x, client 0x%x, varef"
              " 0x%p, parentref 0x%p\n", pResourceRef->hResource, hParent,
              RES_GET_HANDLE(pDevice), RES_GET_CLIENT_HANDLE(pDevice), pResourceRef,
              pCallContext->pResourceRef->pParentRef);

    // Return the actual VAS base and size.
    pNvVASpaceAllocParams->vaBase = vaspaceGetVaStart(pVAS);
    pNvVASpaceAllocParams->vaSize = (vaspaceGetVaLimit(pVAS) - pNvVASpaceAllocParams->vaBase) + 1;

done:
    if (status == NV_OK)
    {
        (void)_vaspaceapiManagePageLevelsForSplitVaSpace(pGpu, pDevice, gpuMask, flags, VASPACEAPI_MANAGE_PAGE_LEVELS_TRIM);
    }
    else
    {
        (void)_vaspaceapiManagePageLevelsForSplitVaSpace(pGpu, pDevice, gpuMask, flags, VASPACEAPI_MANAGE_PAGE_LEVELS_RELEASE);
    }

    if (bLockAcquired)
    {
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    return status;
}

NV_STATUS
vaspaceapiCopyConstruct_IMPL
(
    VaSpaceApi                   *pVaspaceApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsClient                   *pSrcClient = pParams->pSrcClient;
    RsResourceRef                 *pSrcRef = pParams->pSrcRef;
    VaSpaceApi             *pSrcVaspaceApi = dynamicCast(pSrcRef->pResource, VaSpaceApi);
    NV_STATUS                       status = NV_OK;
    OBJGPU                           *pGpu = GPU_RES_GET_GPU(pSrcVaspaceApi);
    NvHandle                   hVASpaceSrc = pSrcRef->hResource;
    NvHandle                    hClientSrc = pSrcClient->hClient;
    NvHandle                       hClient = pCallContext->pClient->hClient;
    NvHandle                       hParent = pCallContext->pResourceRef->pParentRef->hResource;
    NvHandle                      hVASpace = pCallContext->pResourceRef->hResource;
    OBJVASPACE                       *pVAS = pSrcVaspaceApi->pVASpace;
    NvBool                          bFlaVA = NV_FALSE;

    NV_ASSERT_OR_RETURN(pVAS, NV_ERR_INVALID_ARGUMENT);

    bFlaVA = ((IS_VIRTUAL_WITH_SRIOV(pGpu) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
               IS_GSP_CLIENT(pGpu)) &&
              (vaspaceGetFlags(pVAS) & VASPACE_FLAGS_FLA);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, there is nothing to do at this point in the
    // guest OS (where IS_VIRTUAL(pGpu) is true).
    //
    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) && !bFlaVA)
    {
        NV_RM_RPC_DUP_OBJECT(pGpu, hClient, hParent, hVASpace, hClientSrc,
                             hVASpaceSrc, 0,
                             NV_FALSE, // destructor explicitly issues RPC_FREE
                             pCallContext->pResourceRef, status);
    }


    NV_PRINTF(LEVEL_INFO,
              "Shared vaspaceapi 0x%x, device 0x%x, client 0x%x, as vaspace 0x%x for "
              "hParent 0x%x device 0x%x client 0x%x varef 0x%p, deviceref 0x%p\n",
              hVASpaceSrc, pSrcRef->pParentRef->hResource, hClientSrc,
              hVASpace, hParent, RES_GET_HANDLE(GPU_RES_GET_DEVICE(pVaspaceApi)),
              hClient, pCallContext->pResourceRef, pCallContext->pResourceRef->pParentRef);

    vaspaceIncRefCnt(pVAS);
    pVaspaceApi->pVASpace = pVAS;

    return status;

}

void
vaspaceapiDestruct_IMPL(VaSpaceApi *pVaspaceApi)
{
    NV_STATUS           status       = NV_OK;
    OBJGPU             *pGpu         = GPU_RES_GET_GPU(pVaspaceApi);
    OBJVMM             *pVmm         = SYS_GET_VMM(SYS_GET_INSTANCE());
    OBJGVASPACE        *pGVAS;
    NvHandle            hVASpace;
    NvHandle            hParent;
    NvHandle            hClient;
    RsClient           *pRsClient;
    CALL_CONTEXT       *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    Device             *pDevice      = GPU_RES_GET_DEVICE(pVaspaceApi);
    NvBool              bBar1VA      = NV_FALSE;
    NvBool              bFlaVA       = NV_FALSE;

    GPU_RES_SET_THREAD_BC_STATE(pVaspaceApi);

    resGetFreeParams(staticCast(pVaspaceApi, RsResource), &pCallContext, &pParams);
    pRsClient = pCallContext->pClient;
    hClient   = pRsClient->hClient;
    hParent   = pCallContext->pResourceRef->pParentRef->hResource;
    hVASpace  = pCallContext->pResourceRef->hResource;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        bBar1VA = !!(vaspaceGetFlags(pVaspaceApi->pVASpace) & VASPACE_FLAGS_BAR_BAR1);
        bFlaVA = !!(vaspaceGetFlags(pVaspaceApi->pVASpace) & VASPACE_FLAGS_FLA);
    }
    else
    {
        if (IS_GSP_CLIENT(pGpu) && (vaspaceGetFlags(pVaspaceApi->pVASpace) & VASPACE_FLAGS_FLA))
            bFlaVA = NV_TRUE;
    }

    pGVAS = dynamicCast(pVaspaceApi->pVASpace, OBJGVASPACE);
    if (pGVAS != NULL)
    {
        (void)_vaspaceapiManagePageLevelsForSplitVaSpace(pGpu, pDevice, pVaspaceApi->pVASpace->gpuMask, pGVAS->flags,
                                                         VASPACEAPI_MANAGE_PAGE_LEVELS_RELEASE);
    }

    if ((vaspaceGetFlags(pVaspaceApi->pVASpace) & VASPACE_FLAGS_FLA))
    {
        if (GPU_GET_KERNEL_BUS(pGpu)->flaInfo.pFlaVAS == NULL)
        {
            NV_PRINTF(LEVEL_INFO, "Skipping Legacy FLA vaspace destruct, gpu:%x \n",
                      pGpu->gpuInstance);
            goto skip_destroy;
        }
    }

    vmmDestroyVaspace(pVmm, pVaspaceApi->pVASpace);

skip_destroy:
    //
    // RS-TODO: Move out to freeWithResServ?
    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    // In case of SR-IOV, the BAR1 is managed by the guest. So, no need
    // to communicate with the host for BAR1 VA.
    //
    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) && !bBar1VA && !bFlaVA)
    {
        NV_RM_RPC_FREE(pGpu, hClient, hParent, hVASpace, status);
        NV_ASSERT(NV_OK == status);
    }

    NV_PRINTF(LEVEL_INFO,
              "Destroyed vaspaceapi 0x%x, hParent 0x%x, device 0x%x, client 0x%x "
              "varef 0x%p, deviceref 0x%p\n", hVASpace, hParent,
              RES_GET_HANDLE(pDevice), hClient,
              pCallContext->pResourceRef,
              pCallContext->pResourceRef->pParentRef);
}

/**
 * @brief Translate flags to vaspace equivalent flags and perform error check.
 *
 * @param[in] allocFlags  Client handle
 * @param[out] translatedFlags  The translated internal flags.
 **/
static NV_STATUS translateAllocFlagsToVASpaceFlags(NvU32 allocFlags, NvU32 *translatedFlags,
                                                   NvBool bKernelClient, NvU32 gfid)
{
    NV_STATUS status = NV_OK;
    NvU32     flags  = 0;

    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_MINIMIZE_PTETABLE_SIZE)
    {
        flags |= VASPACE_FLAGS_MINIMIZE_PTETABLE_SIZE;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_RETRY_PTE_ALLOC_IN_SYS)
    {
        flags |= VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_SHARED_MANAGEMENT)
    {
        flags |= VASPACE_FLAGS_SHARED_MANAGEMENT;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_MIRRORED)
    {
        flags |= VASPACE_FLAGS_SET_MIRRORED;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_ENABLE_PAGE_FAULTING)
    {
        flags |= VASPACE_FLAGS_ENABLE_FAULTING;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_EXTERNALLY_OWNED)
    {
        flags |= VASPACE_FLAGS_IS_EXTERNALLY_OWNED;
        flags |= VASPACE_FLAGS_DISABLE_SPLIT_VAS;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_ENABLE_NVLINK_ATS)
    {
        flags |= VASPACE_FLAGS_ENABLE_ATS;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_ALLOW_ZERO_ADDRESS)
    {
        flags |= VASPACE_FLAGS_ALLOW_ZERO_ADDRESS;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_IS_FLA)
    {
        flags |= VASPACE_FLAGS_FLA;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_SKIP_SCRUB_MEMPOOL)
    {
        flags |= VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE)
    {
        flags |= VASPACE_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_REQUIRE_FIXED_OFFSET)
    {
        flags |= VASPACE_FLAGS_REQUIRE_FIXED_OFFSET;
    }
    if (allocFlags & NV_VASPACE_ALLOCATION_FLAGS_VA_INTERNAL_LIMIT)
    {
        flags |= VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS;
    }
    flags |= VASPACE_FLAGS_ENABLE_VMM;

    // Validate the flag combinations
    NV_CHECK_OR_RETURN(LEVEL_WARNING,
        !((flags & VASPACE_FLAGS_IS_EXTERNALLY_OWNED) &&
            (flags & VASPACE_FLAGS_SET_MIRRORED)),
        NV_ERR_INVALID_ARGUMENT);

    //
    // MODS environment requires ATS to be enabled but RM to continue to own
    // page table management with PASID coming from the MODS OS layer.
    //
    // For production driver, PASID comes from UVM via NV0080_CTRL_DMA_SET_PAGE_DIRECTORY
    // and so only VASPACE_FLAGS_IS_EXTERNALLY_OWNED or VASPACE_FLAGS_SHARED_MANAGEMENT is
    // supported which is where NV0080_CTRL_DMA_SET_PAGE_DIRECTORY happens.
    // ATS on VASPACE_FLAGS_SHARED_MANAGEMENT is allowed only when it is from
    // kernel client(UVM) or for VF context (where vaspace is allocated in the host for a
    // kernel client created vaspace within the VF's VM).
    //
    NV_CHECK_OR_RETURN(LEVEL_WARNING,
        !((flags & VASPACE_FLAGS_ENABLE_ATS) &&
            !((flags & VASPACE_FLAGS_IS_EXTERNALLY_OWNED) ||
              ((flags & VASPACE_FLAGS_SHARED_MANAGEMENT) && (bKernelClient || IS_GFID_VF(gfid))))),
        NV_ERR_INVALID_ARGUMENT);
    //
    // 1766112: Prevent channels in fault-capable VAS from running unless bound
    //  User-mode clients can allocate a fault-capable VAS and schedule it
    //  without registering it with the UVM driver if it is not marked as
    //  externally owned. This will cause what looks like a hang on the GPU
    //  until the app is killed.
    //  ATS still requires non-externally-owned fault-capable VAS in MODS,
    //  but otherwise this combination is disallowed.
    //
    NV_CHECK_OR_RETURN(LEVEL_WARNING,
        !((flags & VASPACE_FLAGS_ENABLE_FAULTING) &&
            !(flags & VASPACE_FLAGS_IS_EXTERNALLY_OWNED)),
        NV_ERR_INVALID_ARGUMENT);

    *translatedFlags |= flags;

    return status;
}
/**
 * @brief Translate page size to vaspace flags.
 *
 * @param[in] pNvVASpaceAllocParams  user allocation params which has the pagesize.
 * @return[out] flags that vaspace would understand.
 **/
static NvU32 translatePageSizeToVASpaceFlags(NV_VASPACE_ALLOCATION_PARAMETERS *pNvVASpaceAllocParams)
{
    NvU32 flags = 0;

    switch (pNvVASpaceAllocParams->bigPageSize)
    {
        case RM_PAGE_SIZE_64K:
            flags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _64K);
            break;
        case RM_PAGE_SIZE_128K:
            flags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _128K);
            break;
        default:
            flags |= DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _DEFAULT);
            break;
    }

    return flags;
}

/*!
 * Helper function to reserve/release/trim the page levels for Server RM.
 * This function is only needed for client VA space allocation, so we will
 * return directly when that is not the case.
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] pDevice   Device pointer
 * @param[in] gpuMask   GPU mask
 * @param[in] flags     Flags for the corresponding VA allocation
 * @param[in] action    Requested action to manage the page levels
 */
static NV_STATUS
_vaspaceapiManagePageLevelsForSplitVaSpace
(
    OBJGPU                              *pGpu,
    Device                              *pDevice,
    NvU32                                gpuMask,
    NvU32                                flags,
    VASPACEAPI_MANAGE_PAGE_LEVELS_ACTION action
)
{
    NV_STATUS status = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool bCallingContextPlugin;

    // Split VA space is only supported in Client-RM with PMA enabled
    if ((!IS_VIRTUAL_WITH_SRIOV(pGpu) && !IS_GSP_CLIENT(pGpu)) ||
        !RMCFG_FEATURE_PMA ||
        !memmgrIsPmaInitialized(pMemoryManager) ||
        !memmgrAreClientPageTablesPmaManaged(pMemoryManager) ||
        !!(flags & VASPACE_FLAGS_DISABLE_SPLIT_VAS))
    {
        return NV_OK;
    }

    //
    // For Split VAS mechanism, the top 3 page levels for Server RM's carveout region
    // is allocated by the client RM. With PMA managed pagetables, we need to reserve
    // them upfront in memory pool, before it can be allocated.
    //
    NV_ASSERT_OK_OR_RETURN(vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin));
    if (!(flags & VASPACE_FLAGS_FLA) && !bCallingContextPlugin)
    {
        // Loop over each GPU associated with VAS.
        FOR_EACH_GPU_IN_MASK_UC(32, pSys, pGpu, gpuMask)
        {
            RM_POOL_ALLOC_MEM_RESERVE_INFO *pMemPool = NULL;

            //
            // TODO - This call may not work properly at SMC environment, so
            // need to thoutoughly test it against SMC sanities before enabling
            // this function on non Client-RM environment.
            //
            NV_ASSERT_OK_OR_RETURN(memmgrPageLevelPoolsGetInfo(pGpu, pMemoryManager, pDevice, &pMemPool));

            if (action == VASPACEAPI_MANAGE_PAGE_LEVELS_RESERVE)
            {
                status = rmMemPoolReserve(pMemPool, 3 * RM_PAGE_SIZE, flags);
                NV_ASSERT_OR_RETURN(status == NV_OK, status);
            }
            else if (action == VASPACEAPI_MANAGE_PAGE_LEVELS_RELEASE)
            {
                rmMemPoolRelease(pMemPool, flags);
            }
            else if (action == VASPACEAPI_MANAGE_PAGE_LEVELS_TRIM)
            {
                rmMemPoolTrim(pMemPool, 1, flags);
            }
        }
        FOR_EACH_GPU_IN_MASK_UC_END
    }

    return status;
}
