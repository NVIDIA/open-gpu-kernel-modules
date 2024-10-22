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

#define NVOC_KERNEL_GRAPHICS_OBJECT_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/core/locks.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "platform/sli/sli.h"

#include "class/cl0020.h"

#include "ctrl/ctrl2080/ctrl2080gr.h" // NV2080_CTRL_GR_ROUTE_INFO

/*!
 * This function calls promote context RPC to promote GR buffers.
 * Each ctx buffer may or may not may not have its PA updated (initialized) in
 * physical RM, and each ctx buffer should have its VA updated (promoted) in
 * physical RM at least once per VAS, of which there may be multiple in a TSG.
 * When UVM is enabled, this function is responsible only for updating the PA of
 * a given context buffer, and not the VA.
 */
NV_STATUS
kgrobjPromoteContext_IMPL
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject,
    KernelGraphics *pKernelGraphics
)
{
    NvU32 promoteIds[NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES];
    NvU32 promoteIdsSize;
    NvBool bAttemptPromote;
    NvU32 i;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS params;
    NvU32 entryCount;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS status;
    Device *pDevice = GPU_RES_GET_DEVICE(pKernelGraphicsObject);
    Subdevice *pSubdevice;
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);

    if (IS_MODS_AMODEL(pGpu))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        subdeviceGetByDeviceAndGpu(
            RES_GET_CLIENT(pKernelGraphicsObject),
            pDevice,
            pGpu,
            &pSubdevice));

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    kgrobjGetPromoteIds_HAL(pGpu, pKernelGraphicsObject,
                            NV_ARRAY_ELEMENTS(promoteIds),
                            promoteIds,
                            &promoteIdsSize,
                            &bAttemptPromote);

    if (promoteIdsSize == 0)
        return NV_OK;

    portMemSet(&params, 0, sizeof(params));

    entryCount = 0;
    for (i = 0; i < promoteIdsSize; ++i)
    {
        NvBool bInitialize;
        NvBool bPromote = NV_FALSE;

        // Setup parameters to initialize the PA if necessary
        NV_ASSERT_OK_OR_RETURN(
            kgrctxPrepareInitializeCtxBuffer(pGpu,
                                             pKernelGraphicsObject->pKernelGraphicsContext,
                                             pKernelGraphics,
                                             pChannelDescendant->pKernelChannel,
                                             promoteIds[i],
                                             &params.promoteEntry[entryCount],
                                             &bInitialize));

        if (bAttemptPromote)
        {
            // Setup parameters to promote the VA if necessary
            NV_ASSERT_OK_OR_RETURN(
                kgrctxPreparePromoteCtxBuffer(pGpu,
                                              pKernelGraphicsObject->pKernelGraphicsContext,
                                              pChannelDescendant->pKernelChannel,
                                              promoteIds[i],
                                              &params.promoteEntry[entryCount],
                                              &bPromote));
        }

        // If initialization / promotion was necessary, then move to next index
        if (bInitialize || bPromote)
            entryCount++;
    }

    // Nothing to promote, so return early
    if (entryCount == 0)
        return NV_OK;

    params.engineType  = NV2080_ENGINE_TYPE_GR(kgraphicsGetInstance(pGpu, pKernelGraphics));
    params.hChanClient = RES_GET_CLIENT_HANDLE(pChannelDescendant);
    params.hObject     = RES_GET_PARENT_HANDLE(pChannelDescendant);
    params.entryCount  = entryCount;

    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pSubdevice),
                             RES_GET_HANDLE(pSubdevice),
                             NV2080_CTRL_CMD_GPU_PROMOTE_CTX,
                             &params,
                             sizeof(params));

    if (status == NV_OK)
    {
        //
        // If we successfully promoted the PA, flip a flag to ensure we don't
        // try to promote it again. The VA_LIST should already track this for
        // VA, but we can't rely on it for PA due to UVM.
        //
        for (i = 0; i < entryCount; ++i)
        {
            if (!params.promoteEntry[i].bInitialize)
                continue;

            kgrctxMarkCtxBufferInitialized(pGpu,
                                           pKernelGraphicsObject->pKernelGraphicsContext,
                                           pKernelGraphics,
                                           pChannelDescendant->pKernelChannel,
                                           params.promoteEntry[i].bufferId);
        }
    }

    return status;
}

/*!
 * @brief Construct the Kernel GR object for the given GPU
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphics
 * @param[in]  pKernelGraphicsObject
 */
static NV_STATUS
_kgrAlloc
(
    OBJGPU               *pGpu,
    KernelGraphics       *pKernelGraphics,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NvU32 numGpcs;
    NvU32 classNum = pChannelDescendant->resourceDesc.externalClassId;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NvU32 gfid = kchannelGetGfid(pChannelDescendant->pKernelChannel);

    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    // Prevent ctx creation on sys pipes with 0 GPCs attached
    if (!IS_MODS_AMODEL(pGpu))
    {
        numGpcs = nvPopCount32(pKernelGraphicsStaticInfo->floorsweepingMasks.gpcMask);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, numGpcs > 0, NV_ERR_INSUFFICIENT_RESOURCES);
    }

    // Each object may need to create an Mmio mapping
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrobjSetComputeMmio_HAL(pGpu, pKernelGraphicsObject));

    // Ensure that ctx buffer information is initialized
    if (!IS_MODS_AMODEL(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(
            kgraphicsInitializeDeferredStaticData(pGpu, pKernelGraphics, NV01_NULL_OBJECT, NV01_NULL_OBJECT));
    }

    // Allocate FECS buffer in Guest for SRIOV configs.
    if (kgrctxShouldManageCtxBuffers_HAL(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, gfid) || IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgraphicsAllocKgraphicsBuffers_HAL(pGpu, pKernelGraphics, pKernelGraphicsObject->pKernelGraphicsContext, pChannelDescendant->pKernelChannel));
    }

    if (kgrctxShouldManageCtxBuffers_HAL(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, gfid))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxAllocCtxBuffers(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, pKernelGraphics, pKernelGraphicsObject));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxMapCtxBuffers(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, pKernelGraphics, pKernelGraphicsObject));
    }

    kgrctxIncObjectCount_HAL(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, classNum);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrobjPromoteContext(pGpu, pKernelGraphicsObject, pKernelGraphics));

    return NV_OK;
}

/*!
 * @brief Destruct the Kernel GR object
 *
 * @param[in]  pKernelGraphicsObject
 * @param[in]  bDestructor              NV_TRUE if called from destructor, used to share
 *                                       cleanup code with constructor
 */
static void _kgrobjDestruct
(
    KernelGraphicsObject *pKernelGraphicsObject,
    NvBool bDestructor
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelGraphicsObject);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    {
        ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
        NV_STATUS status = NV_OK;
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = {0};
        Device *pDevice = GPU_RES_GET_DEVICE(pChannelDescendant);
        NvHandle hParent = RES_GET_PARENT_HANDLE(pChannelDescendant);
        NvU32 classNum = pChannelDescendant->resourceDesc.externalClassId;

        // If MIG is enabled, perform GR instance routing based upon parent channel handle
        kgrmgrCtrlSetChannelHandle(hParent, &grRouteInfo);
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));

        if (status != NV_OK)
            SLI_LOOP_CONTINUE;

        // Free Compute Mmio mapping
        kgrobjFreeComputeMmio_HAL(pGpu, pKernelGraphicsObject);

        if (bDestructor)
            kgrctxDecObjectCount_HAL(pGpu, pKernelGraphicsObject->pKernelGraphicsContext, classNum);

        if (kgrobjShouldCleanup_HAL(pGpu, pKernelGraphicsObject))
        {
            kgrctxUnmapCtxBuffers(pGpu,
                                  pKernelGraphicsObject->pKernelGraphicsContext,
                                  pKernelGraphicsObject,
                                  pKernelGraphics,
                                  bDestructor);
        }
    }
    SLI_LOOP_END;
}

//
// Graphics engine object creation routine.
//
NV_STATUS
kgrobjConstruct_IMPL
(
    KernelGraphicsObject         *pKernelGraphicsObject,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;
    NV_STATUS          status = NV_OK;
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    Device            *pDevice = GPU_RES_GET_DEVICE(pKernelGraphicsObject);

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_STATE);

    NV_PRINTF(LEVEL_INFO, "class: 0x%x on channel: 0x%08x\n",
              pChannelDescendant->resourceDesc.externalClassId, kchannelGetDebugTag(pKernelChannel));

    //
    // Legacy code believed this to be possible, but Resource Server should
    // prevent NV01_NULL_OBJECT from being chosen as a handle.
    //
    NV_ASSERT_OR_RETURN(pParams->hResource != NV01_NULL_OBJECT,
                        NV_ERR_INVALID_STATE);

    {
        KernelChannel *pkChannel = pChannelDescendant->pKernelChannel;

        NV_ASSERT_OR_RETURN(pkChannel != NULL, NV_ERR_INVALID_STATE);

        if (kgrctxFromKernelChannel(pkChannel, &pKernelGraphicsObject->pKernelGraphicsContext) == NV_OK)
        {
            KernelGraphicsContext *pKernelGraphicsContext = pKernelGraphicsObject->pKernelGraphicsContext;

            //
            // Add each gr object as a dependant of the context such that all
            // objects are guaranteed to be torn down before the context
            //
            refAddDependant(RES_GET_REF(pKernelGraphicsContext),
                            RES_GET_REF(pKernelGraphicsObject));

        }
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KernelGraphics *pKernelGraphics;
        NV2080_CTRL_GR_ROUTE_INFO grRouteInfo = {0};

        // If MIG is enabled, perform GR instance routing based upon parent channel handle
        kgrmgrCtrlSetChannelHandle(pParams->hParent, &grRouteInfo);
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager, pDevice, &grRouteInfo, &pKernelGraphics));
        if (status != NV_OK)
            SLI_LOOP_BREAK;

        // Construct the Kernel Object
        status = _kgrAlloc(pGpu, pKernelGraphics, pKernelGraphicsObject);

        if (status != NV_OK)
            SLI_LOOP_BREAK;
    }
    SLI_LOOP_END;

    // failed
    if (status != NV_OK)
    {
        // Destroy the kernel object from the constructor
        _kgrobjDestruct(pKernelGraphicsObject, NV_FALSE);
    }

    return status;
}

/*!
 * @brief GR object Destructor
 *
 * @param[in]  pKernelGraphicsObject
 */
void
kgrobjDestruct_IMPL
(
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    // Destroy the kernel object from the destructor
    _kgrobjDestruct(pKernelGraphicsObject, NV_TRUE);
}

NV_STATUS
kgrobjGetMemInterMapParams_IMPL
(
    KernelGraphicsObject *pKernelGraphicsObject,
    RMRES_MEM_INTER_MAP_PARAMS *pParams
)
{
    MEMORY_DESCRIPTOR *pSrcMemDesc = pKernelGraphicsObject->pMmioMemDesc;

    if (pParams->bSubdeviceHandleProvided)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast DMA mappings of non-memory objects not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pSrcMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    pParams->pSrcMemDesc = pSrcMemDesc;
    pParams->pSrcGpu = pSrcMemDesc->pGpu;

    return NV_OK;
}

/*!
 * @brief Set up MMIO memDesc for Compute Object
 */
NV_STATUS
kgrobjSetComputeMmio_IMPL
(
    OBJGPU               *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    ChannelDescendant *pChanDes = staticCast(pKernelGraphicsObject, ChannelDescendant);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32 classNum = pChanDes->resourceDesc.externalClassId;
    NvU32 objType;
    NV_ADDRESS_SPACE addrSpace;

    kgrmgrGetGrObjectType(classNum, &objType);

    // Nothing to do for non-compute
    if (objType != GR_OBJECT_TYPE_COMPUTE)
        return NV_OK;

    // This can be called multiple times in SLI, so just skip it if present
    if (pKernelGraphicsObject->pMmioMemDesc != NULL)
        return NV_OK;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        addrSpace = ADDR_SYSMEM;
    }
    else
    {
        addrSpace = ADDR_FBMEM;
    }

    // Set up MMIO memDesc to allow GPU mappings of compute object
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        memdescCreate(&pKernelGraphicsObject->pMmioMemDesc, pGpu, RM_PAGE_SIZE, 0,
                    NV_TRUE, addrSpace, NV_MEMORY_UNCACHED,
                    MEMDESC_FLAGS_NONE));

    //
    // The address field is completely ignored for these mappings.  We use the
    // chid as the address strictly for debug purposes.
    //
    memdescDescribe(pKernelGraphicsObject->pMmioMemDesc, addrSpace /* Ignored */,
                    RM_PAGE_SIZE * pChanDes->pKernelChannel->ChID,
                    RM_PAGE_SIZE);

    memdescSetPteKind(pKernelGraphicsObject->pMmioMemDesc,
                      memmgrGetMessageKind_HAL(pGpu, pMemoryManager));

    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pKernelGraphicsObject->pMmioMemDesc, AT_GPU,
                                 RM_ATTR_PAGE_SIZE_4KB);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        //
        // DMA mapping path in RM checks to make sure the sysmem page is actually allocated before allowing
        // the mapping
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, memdescAlloc(pKernelGraphicsObject->pMmioMemDesc));

        //
        // Flag needed to differentiate between SKED and internal MMIO
        // - SKED reflected surface is encoded with aperture = sysncoh and KIND = SKED
        // - MMIO surface is encoded with aperture = syscoh and KIND = SKED
        //
        memdescSetFlag(pKernelGraphicsObject->pMmioMemDesc, MEMDESC_FLAGS_ALLOC_SKED_REFLECTED, NV_TRUE);
    }

    return NV_OK;
}

/*!
 * @brief Free up MMIO memDesc for Compute Object
 */
void
kgrobjFreeComputeMmio_IMPL
(
    OBJGPU               *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    memdescDestroy(pKernelGraphicsObject->pMmioMemDesc);
    pKernelGraphicsObject->pMmioMemDesc = NULL;
}

/*!
 * @brief Should we perform grobj cleanup?
 * If client RM is not managing the ctx buffers, kernel RM should not unmap ctx buffers
 */
NvBool
kgrobjShouldCleanup_KERNEL
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NvU32              gfid = kchannelGetGfid(pChannelDescendant->pKernelChannel);

    return gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) && !IS_GFID_VF(gfid);
}

/*!
 * @brief Should we perform grobj cleanup?
 * If client RM is managing the ctx buffers, physical RM should not unmap ctx buffers
 */
NvBool
kgrobjShouldCleanup_PHYSICAL
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NvU32              gfid = kchannelGetGfid(pChannelDescendant->pKernelChannel);

    return !gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) || (gpuIsSriovEnabled(pGpu) && IS_GFID_PF(gfid));
}

/*!
 * @brief Retrieve the IDs of ctx buffers which need to be promoted for this ctx
 *        This version only serves SRIOV-Heavy, which allocates just the FECS
 *        event buffer in guest RM and promotes its PA to the host, while the
 *        rest of the buffers are managed by the host.
 */
void
kgrobjGetPromoteIds_VF
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject,
    NvU32 maxPromoteIds,
    NvU32 *pPromoteIds,
    NvU32 *pNumEntries,
    NvBool *pbPromote
)
{
    void kgrobjGetPromoteIds_FWCLIENT(OBJGPU *, KernelGraphicsObject *, NvU32, NvU32 *, NvU32 *, NvBool *);
    NvU32 promoteSriovHeavy[] =
    {
        //
        // For SRIOV Heavy, guest allocates FECS event buffer and informs host
        // of its address, other buffers are managed by host
        //
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT
    };
    NvBool bSriovHeavyEnabled = gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

    if (!bSriovHeavyEnabled)
    {
        // Use the same implementation as FWCLIENT
        kgrobjGetPromoteIds_FWCLIENT(pGpu, pKernelGraphicsObject, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote);
        return;
    }

    *pNumEntries = 0;
    // SRIOV Heavy only initializes and does not promote the FECS buffer
    *pbPromote = NV_FALSE;

    NV_ASSERT_OR_RETURN_VOID(NV_ARRAY_ELEMENTS(promoteSriovHeavy) <= maxPromoteIds);
    *pNumEntries = NV_ARRAY_ELEMENTS(promoteSriovHeavy);
    portMemCopy(pPromoteIds, sizeof(promoteSriovHeavy), promoteSriovHeavy, sizeof(promoteSriovHeavy));
}

/*!
 * @brief Retrieve the IDs of ctx buffers which need to be promoted for this ctx
 *        This version serves SRIOV and FWCLIENT, which allocate and map the ctx
 *        buffers themselves and promote the PA/VA to host RM.
 */
void
kgrobjGetPromoteIds_FWCLIENT
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject,
    NvU32 maxPromoteIds,
    NvU32 *pPromoteIds,
    NvU32 *pNumEntries,
    NvBool *pbPromote
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NvBool bAcquire3d;

    // Buffers which need to be promoted if we are not allocating a 3d context
    NvU32 promoteNon3d[] =
    {
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP
    };
    // Buffers which need to be promoted if we are allocating a 3d context
    NvU32 promote3d[] =
    {
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP
    };

    *pNumEntries = 0;
    *pbPromote = NV_TRUE;

    if (!gpuIsClientRmAllocatedCtxBufferEnabled(pGpu))
        return;

    // Do we need to promote any 3D-specific context buffers?
    if (pChannelDescendant->pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {
        bAcquire3d = NV_TRUE;
    }
    else
    {
        NvU32 classNum = pChannelDescendant->resourceDesc.externalClassId;
        NvU32 objType;
        kgrmgrGetGrObjectType(classNum, &objType);
        bAcquire3d = ((objType == GR_OBJECT_TYPE_2D) || (objType == GR_OBJECT_TYPE_3D));
    }

    // Determine which set of buffers we need to try to init/promote
    if (bAcquire3d)
    {
        NV_ASSERT_OR_RETURN_VOID(NV_ARRAY_ELEMENTS(promote3d) <= maxPromoteIds);
        *pNumEntries = NV_ARRAY_ELEMENTS(promote3d);
        portMemCopy(pPromoteIds, sizeof(promote3d), promote3d, sizeof(promote3d));
    }
    else
    {
        NV_ASSERT_OR_RETURN_VOID(NV_ARRAY_ELEMENTS(promoteNon3d) <= maxPromoteIds);
        *pNumEntries = NV_ARRAY_ELEMENTS(promoteNon3d);
        portMemCopy(pPromoteIds, sizeof(promoteNon3d), promoteNon3d, sizeof(promoteNon3d));
    }
}

/*!
 * @brief Retrieve the IDs of ctx buffers which need to be promoted for this ctx
 *        This version serves bare metal and GSP when these environments are
 *        managing the ctx buffers. There is additional physical-only
 *        initialization required for these buffers on allocation which would
 *        otherwise be handled when these buffers are promoted from client RM,
 *        but in absence of above should be called manually by physical RM to
 *        keep the flow / initialization consistent.
 */
void
kgrobjGetPromoteIds_PHYSICAL
(
    OBJGPU *pGpu,
    KernelGraphicsObject *pKernelGraphicsObject,
    NvU32 maxPromoteIds,
    NvU32 *pPromoteIds,
    NvU32 *pNumEntries,
    NvBool *pbPromote
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKernelGraphicsObject, ChannelDescendant);
    NvU32 gfid = kchannelGetGfid(pChannelDescendant->pKernelChannel);
    NvBool bAcquire3d;

    // Buffers which need to be promoted if we are not allocating a 3d context
    NvU32 promoteNon3d[] =
    {
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP
    };
    // Buffers which need to be promoted if we are allocating a 3d context
    NvU32 promote3d[] =
    {
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP,
        NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP
    };

    *pNumEntries = 0;
    *pbPromote = NV_FALSE;

    // If client is managing ctx buffers, we expect client to initialize these buffers
    if (gpuIsClientRmAllocatedCtxBufferEnabled(pGpu) && !(gpuIsSriovEnabled(pGpu) && IS_GFID_PF(gfid)))
        return;

    // Do we need to promote any 3D-specific context buffers?
    if (pChannelDescendant->pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {
        bAcquire3d = NV_TRUE;
    }
    else
    {
        NvU32 classNum = pChannelDescendant->resourceDesc.externalClassId;
        NvU32 objType;
        kgrmgrGetGrObjectType(classNum, &objType);
        bAcquire3d = ((objType == GR_OBJECT_TYPE_2D) || (objType == GR_OBJECT_TYPE_3D));
    }

    // Determine which set of buffers we need to try to init/promote
    if (bAcquire3d)
    {
        NV_ASSERT_OR_RETURN_VOID(NV_ARRAY_ELEMENTS(promote3d) <= maxPromoteIds);
        *pNumEntries = NV_ARRAY_ELEMENTS(promote3d);
        portMemCopy(pPromoteIds, sizeof(promote3d), promote3d, sizeof(promote3d));
    }
    else
    {
        NV_ASSERT_OR_RETURN_VOID(NV_ARRAY_ELEMENTS(promoteNon3d) <= maxPromoteIds);
        *pNumEntries = NV_ARRAY_ELEMENTS(promoteNon3d);
        portMemCopy(pPromoteIds, sizeof(promoteNon3d), promoteNon3d, sizeof(promoteNon3d));
    }
}

