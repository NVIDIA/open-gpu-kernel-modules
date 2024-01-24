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

/******************************************************************************
*
*   Description:
*       This file contains functions managing DispChannel and its derived classes.
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "resserv/resserv.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"

#include "gpu/device/device.h"
#include "gpu/gpu_resource.h"
#include "gpu/disp/disp_channel.h"
#include "gpu/disp/disp_objs.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"
#include "gpu/mem_mgr/context_dma.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"
#include "vgpu/rpc.h"

static void
dispchnParseAllocParams
(
    DispChannel *pDispChannel,
    void        *pAllocParams,
    NvU32       *pChannelInstance,
    NvHandle    *pHObjectBuffer,
    NvU32       *pInitialGetPutOffset,
    NvBool      *pAllowGrabWithinSameClient,
    NvBool      *pConnectPbAtGrab
)
{
    NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS *pDmaChannelAllocParams = NULL;
    NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS *pPioChannelAllocParams = NULL;

    *pAllowGrabWithinSameClient = NV_FALSE;
    *pConnectPbAtGrab = NV_FALSE;

    if (pDispChannel->bIsDma)
    {
        pDmaChannelAllocParams = pAllocParams;
        *pChannelInstance      = pDmaChannelAllocParams->channelInstance;
        *pHObjectBuffer        = pDmaChannelAllocParams->hObjectBuffer;
        *pInitialGetPutOffset  = pDmaChannelAllocParams->offset;

        if (FLD_TEST_DRF(50VAIO_CHANNELDMA_ALLOCATION, _FLAGS,
                         _CONNECT_PB_AT_GRAB, _YES,
                         pDmaChannelAllocParams->flags))
        {
            *pConnectPbAtGrab = NV_TRUE;
        }

        if (pDmaChannelAllocParams->hObjectNotify != 0)
        {
            NV_PRINTF(LEVEL_WARNING, "Error notifier parameter is not used in Display channel allocation.\n");
        }
    }
    else
    {
        pPioChannelAllocParams = pAllocParams;
        *pChannelInstance      = pPioChannelAllocParams->channelInstance;
        *pHObjectBuffer        = 0; // No one should look at this. So, 0 should be fine.
        *pInitialGetPutOffset  = 0; // No one should look at this. So, 0 should be fine.

        if (pPioChannelAllocParams->hObjectNotify != 0)
        {
            NV_PRINTF(LEVEL_WARNING, "Error notifier parameter is not used in Display channel allocation.\n");
        }
    }
}

NV_STATUS
dispchnConstruct_IMPL
(
    DispChannel                  *pDispChannel,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    NvU32                         isDma
)
{
    OBJGPU         *pGpu = GPU_RES_GET_GPU(pDispChannel);
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_STATUS       rmStatus = NV_OK;
    NvU32           channelInstance;
    NvHandle        hObjectBuffer;
    NvBool          bIsDma = !!isDma;
    NvU32           initialGetPutOffset;
    NvBool          allowGrabWithinSameClient;
    NvBool          connectPbAtGrab;
    DISPCHNCLASS    internalDispChnClass;
    void           *pAllocParams = pParams->pAllocParams;
    RsResourceRef  *pParentRef = RES_GET_REF(pDispChannel)->pParentRef;
    DispObject     *pDispObject = dynamicCast(pParentRef->pResource, DispObject);
    ContextDma     *pBufferContextDma = NULL;
    NvU32           hClass = RES_GET_EXT_CLASS_ID(pDispChannel);

    NV_ASSERT_OR_RETURN(pDispObject, NV_ERR_INVALID_OBJECT_HANDLE);

    if (pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_USER_ROOT)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failure allocating display class 0x%08x: Only root(admin)/kernel clients are allowed\n",
                  pParams->externalClassId);

        //
        // GPUSWSEC-1560 introduced a central object privilege check in RS. Please mark derived external classes
        // of DispChannel privileged in their RS_ENTRY. Since DispChannel doesn't have an external class of its own
        // and is used as a base class, leaving this check inline to catch future derivations.
        //
        osAssertFailed();

        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    //
    // Make sure this channel class is supported on this chip.
    // Need to have the check below since, the switch in RmAlloc
    // doesn't tell if the current chip supports the class
    //
    if (!gpuIsClassSupported(pGpu, RES_GET_EXT_CLASS_ID(pDispChannel)))
    {
        NV_PRINTF(LEVEL_ERROR, "Unsupported class in\n");
        return NV_ERR_INVALID_CLASS;
    }

    // Move params into RM's address space
    pDispChannel->pDispObject = pDispObject;
    pDispChannel->bIsDma = bIsDma;
    dispchnParseAllocParams(pDispChannel, pAllocParams,
                            &channelInstance,
                            &hObjectBuffer,
                            &initialGetPutOffset,
                            &allowGrabWithinSameClient,
                            &connectPbAtGrab);

    rmStatus = kdispGetIntChnClsForHwCls(pKernelDisplay,
                                         RES_GET_EXT_CLASS_ID(pDispChannel),
                                        &internalDispChnClass);
    if (rmStatus != NV_OK)
        return rmStatus;

    if (internalDispChnClass == dispChnClass_Any)
    {
        //
        // Any channel is kernel only channel, Physical RM doesn't need ANY channel information.
        // return from here as ANY channel is constructed.
        //
        pDispChannel->DispClass        = internalDispChnClass;
        pDispChannel->InstanceNumber   = channelInstance;
        return NV_OK;
    }

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_TRUE, NV_FALSE);
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
    {
        rmStatus = kdispSetPushBufferParamsToPhysical_HAL(pGpu,
                                            pKernelDisplay,
                                            pDispChannel,
                                            hObjectBuffer,
                                            pBufferContextDma,
                                            hClass,
                                            channelInstance,
                                            internalDispChnClass);
        if (rmStatus != NV_OK)
           return rmStatus;
    }
    SLI_LOOP_END

    // Acquire the underlying HW resources
    rmStatus = kdispAcquireDispChannelHw_HAL(pKernelDisplay,
                                             pDispChannel,
                                             channelInstance,
                                             hObjectBuffer,
                                             initialGetPutOffset,
                                             allowGrabWithinSameClient,
                                             connectPbAtGrab);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "disp channel[0x%x] alloc failed. Return status = 0x%x\n",
                  channelInstance, rmStatus);

        return rmStatus;
    }

    // Channel allocation is successful, initialize new channel's data structures
    pDispChannel->DispClass        = internalDispChnClass;
    pDispChannel->InstanceNumber   = channelInstance;
    dispchnSetRegBaseOffsetAndSize(pDispChannel, pGpu);

    // Map memory for parent GPU
    rmStatus = kdispMapDispChannel_HAL(pKernelDisplay, pDispChannel);

    // setup to return pControl to client
    if (pDispChannel->bIsDma)
    {
        NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS *pDmaChannelAllocParams = pAllocParams;
        pDmaChannelAllocParams->pControl = pDispChannel->pControl;
    }
    else
    {
        NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS *pPioChannelAllocParams = pAllocParams;
        pPioChannelAllocParams->pControl = pDispChannel->pControl;
    }

    return rmStatus;
}

//
// Performs grab operation for a channel.
//
// Pre-Volta Linux swapgroups is the only remaining use of channel grabbing.
// Bug 2869820 is tracking the transition of swapgroups from requiring this
// RM feature.
//
NV_STATUS
dispchnGrabChannel_IMPL
(
    DispChannel *pDispChannel,
    NvHandle     hClient,
    NvHandle     hParent,
    NvHandle     hChannel,
    NvU32        hClass,
    void        *pAllocParams
)
{
    NV_STATUS          rmStatus = NV_OK;
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pDispChannel);
    KernelDisplay     *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32              channelInstance;
    NvHandle           hObjectBuffer;
    NvU32              initialGetPutOffset;
    NvBool             allowGrabWithinSameClient;
    NvBool             connectPbAtGrab;
    ContextDma        *pBufferContextDma = NULL;
    DISPCHNCLASS       internalDispChnClass;

    if (RES_GET_PARENT_HANDLE(pDispChannel) != hParent)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "disp channel grab failed because of bad display parent 0x%x\n",
                  hParent);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    // Move params into RM's address space
    dispchnParseAllocParams(pDispChannel, pAllocParams,
                            &channelInstance,
                            &hObjectBuffer,
                            &initialGetPutOffset,
                            &allowGrabWithinSameClient,
                            &connectPbAtGrab);

    //
    // The handle already exists in our DB.
    // The supplied params must be same as what we already have with us
    //
    if (RES_GET_EXT_CLASS_ID(pDispChannel) != hClass                ||
        pDispChannel->InstanceNumber       != channelInstance)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Information supplied for handle 0x%x doesn't match that in RM's client DB\n",
                  hChannel);
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    rmStatus = kdispGetIntChnClsForHwCls(pKernelDisplay,
                                     hClass,
                                    &internalDispChnClass);

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_TRUE, NV_FALSE);
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
    {
      rmStatus = kdispSetPushBufferParamsToPhysical_HAL(pGpu,
                                           pKernelDisplay,
                                           pDispChannel,
                                           hObjectBuffer,
                                           pBufferContextDma,
                                           hClass,
                                           channelInstance,
                                           internalDispChnClass);
      if (rmStatus != NV_OK)
          return rmStatus;
    }
    SLI_LOOP_END

    // Acquire the underlying HW resources
    rmStatus = kdispAcquireDispChannelHw_HAL(pKernelDisplay,
                                             pDispChannel,
                                             channelInstance,
                                             hObjectBuffer,
                                             initialGetPutOffset,
                                             allowGrabWithinSameClient,
                                             connectPbAtGrab);

    // setup to return pControl to client
    if (pDispChannel->bIsDma)
    {
        NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS *pDmaChannelAllocParams = pAllocParams;
        pDmaChannelAllocParams->pControl = pDispChannel->pControl;
    }
    else
    {
        NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS *pPioChannelAllocParams = pAllocParams;
        pPioChannelAllocParams->pControl = pDispChannel->pControl;
    }

    return rmStatus;
}

NV_STATUS
dispchnGetRegBaseOffsetAndSize_IMPL
(
    DispChannel *pDispChannel,
    OBJGPU *pGpu,
    NvU32 *pOffset,
    NvU32 *pSize
)
{
    if (pOffset)
        *pOffset = pDispChannel->ControlOffset;

    if (pSize)
        *pSize = pDispChannel->ControlLength;

    return NV_OK;
}

void
dispchnSetRegBaseOffsetAndSize_IMPL
(
    DispChannel *pDispChannel,
    OBJGPU      *pGpu
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    (void)kdispGetDisplayChannelUserBaseAndSize_HAL(pGpu, pKernelDisplay,
                                                    pDispChannel->DispClass,
                                                    pDispChannel->InstanceNumber,
                                                   &pDispChannel->ControlOffset,
                                                   &pDispChannel->ControlLength);

    // Tegra offsets needs to be subtracted with -0x610000.
    pDispChannel->ControlOffset += kdispGetBaseOffset_HAL(pGpu, pKernelDisplay);
}

/*!
 * @brief Maps channel user area for parent GPU.
 */
NV_STATUS
kdispMapDispChannel_IMPL
(
    KernelDisplay *pKernelDisplay,
    DispChannel   *pDispChannel
)
{
    NV_STATUS       rmStatus;
    OBJGPU         *pGpu        = GPU_RES_GET_GPU(pDispChannel);
    RsClient       *pClient     = RES_GET_CLIENT(pDispChannel);
    RmClient       *pRmClient   = dynamicCast(pClient, RmClient);
    RS_PRIV_LEVEL   privLevel   = rmclientGetCachedPrivilege(pRmClient);
    RM_API         *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);

    //
    // Only need the map for the parent GPU since we require the client to
    // use RmMapMemory for subdevice channel mapping.
    //
    rmStatus = osMapGPU(pGpu, privLevel,
                        pDispChannel->ControlOffset,
                        pDispChannel->ControlLength,
                        NV_PROTECT_READ_WRITE,
                       &pDispChannel->pControl,
                       &pDispChannel->pPriv);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "disp channel[0x%x] mapping failed. Return status = 0x%x\n",
                  pDispChannel->InstanceNumber, rmStatus);

        (void) pRmApi->Free(pRmApi,
                            RES_GET_CLIENT_HANDLE(pDispChannel),
                            RES_GET_HANDLE(pDispChannel));

        return rmStatus;
    }

    return NV_OK;
}

/*!
 * @brief Unbinds Context DMAs and unmaps channel user area for the given channel.
 */
void kdispUnbindUnmapDispChannel_IMPL
(
    KernelDisplay *pKernelDisplay,
    DispChannel *pDispChannel
)
{
    OBJGPU             *pGpu        = GPU_RES_GET_GPU(pDispChannel);
    RsClient           *pClient     = RES_GET_CLIENT(pDispChannel);
    RmClient           *pRmClient   = dynamicCast(pClient, RmClient);
    RS_PRIV_LEVEL       privLevel   = rmclientGetCachedPrivilege(pRmClient);

    // Unbind all ContextDmas from this channel
    dispchnUnbindAllCtx(pGpu, pDispChannel);

    // Unmap the channel
    osUnmapGPU(pGpu->pOsGpuInfo, privLevel, pDispChannel->pControl,
               pDispChannel->ControlLength, pDispChannel->pPriv);
}

void
dispchnDestruct_IMPL
(
    DispChannel *pDispChannel
)
{
    NV_STATUS           rmStatus  = NV_OK;
    OBJGPU             *pGpu      = GPU_RES_GET_GPU(pDispChannel);
    KernelDisplay      *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    RM_API             *pRmApi    = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    LOCK_METER_DATA(FREE_CHANNEL_DISP, pDispChannel->DispClass, 0, 0);

    //
    // Before freeing the CORE channel, make sure all satellite channels are
    // torn down. This is currently necessary on UNIX to deal with cases
    // where X (i.e. the userspace display driver) terminates before other
    // RM clients with satellite channel allocations, e.g. OpenGL clients with
    // BASE channel allocations.
    //
    if ((pDispChannel->DispClass == dispChnClass_Core) &&
        pKernelDisplay->bWarPurgeSatellitesOnCoreFree)
    {
        RmClient     **ppClient;
        RmClient      *pClient;
        RsClient      *pRsClient;
        RS_ITERATOR    it;
        Device        *pDevice;
        OBJGPU        *pTmpGpu;
        DispChannel   *pTmpDispChannel;

        NV_ASSERT(gpuIsGpuFullPower(pGpu));

        for (ppClient = serverutilGetFirstClientUnderLock();
             ppClient;
             ppClient = serverutilGetNextClientUnderLock(ppClient))
        {
            pClient = *ppClient;
            pRsClient = staticCast(pClient, RsClient);

            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

            while (clientRefIterNext(it.pClient, &it))
            {
                RS_ITERATOR    dispIt;
                RsResourceRef *pResourceRef;
                DispObject    *pDispObject;

                pDevice = dynamicCast(it.pResourceRef->pResource, Device);

                pTmpGpu = GPU_RES_GET_GPU(pDevice);
                if (pTmpGpu != pGpu)
                    continue;

                rmStatus = dispobjGetByDevice(pRsClient, pDevice, &pDispObject);
                if (rmStatus != NV_OK)
                    continue;

                pResourceRef = RES_GET_REF(pDispObject);

                dispIt = clientRefIter(pRsClient, pResourceRef, classId(DispChannel), RS_ITERATE_CHILDREN, NV_FALSE);

                while (clientRefIterNext(dispIt.pClient, &dispIt))
                {
                    pTmpDispChannel = dynamicCast(dispIt.pResourceRef->pResource, DispChannel);

                    if (pTmpDispChannel->DispClass != dispChnClass_Core)
                    {
                        rmStatus = pRmApi->Free(pRmApi,
                                                RES_GET_CLIENT_HANDLE(pTmpDispChannel),
                                                RES_GET_HANDLE(pTmpDispChannel));

                        if (rmStatus == NV_OK)
                        {
                            // Client's resource map has been modified, re-snap iterators
                            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
                            dispIt = clientRefIter(pRsClient, it.pResourceRef, classId(DispChannel), RS_ITERATE_DESCENDANTS, NV_FALSE);
                        }
                        else
                        {
                            NV_PRINTF(LEVEL_ERROR,
                                      "Failed to free satellite DispChannel 0x%x!\n",
                                      RES_GET_HANDLE(pTmpDispChannel));
                        }
                    }
                }
            }
        }
    }

    //
    // Unbind all context dmas bound to this channel, unmap the channel and
    // finally release HW resources.
    //
    kdispUnbindUnmapDispChannel_HAL(pKernelDisplay, pDispChannel);
    rmStatus = kdispReleaseDispChannelHw_HAL(pKernelDisplay, pDispChannel);

    if (rmStatus != NV_OK)
    {
        // Try to avoid returning error codes on free under new resource server design
        NV_ASSERT(0);
    }
}

NV_STATUS
dispchnpioConstruct_IMPL
(
    DispChannelPio               *pDispChannelPio,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
dispchndmaConstruct_IMPL
(
    DispChannelDma               *pDispChannelDma,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
dispchnGetByHandle_IMPL
(
    RsClient     *pClient,
    NvHandle      hDisplayChannel,
    DispChannel **ppDispChannel
)
{
    RsResourceRef  *pResourceRef;
    NV_STATUS       status;

    *ppDispChannel = NULL;

    status = clientGetResourceRef(pClient, hDisplayChannel, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppDispChannel = dynamicCast(pResourceRef->pResource, DispChannel);

    return (*ppDispChannel) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

//
// Bind the DMA context to a display channel
//
NV_STATUS
dispchnBindCtx_IMPL
(
    OBJGPU       *pGpu,
    ContextDma   *pContextDma,
    NvHandle     hChannel
)
{
    RsClient     *pClient = RES_GET_CLIENT(pContextDma);
    DispChannel  *pDispChannel = NULL;
    NV_STATUS rmStatus = NV_OK;
    KernelDisplay *pKernelDisplay;
    DisplayInstanceMemory *pInstMem;

    // Look-up channel
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        dispchnGetByHandle(pClient, hChannel, &pDispChannel));

    // Ensure ContextDma and DisplayChannel are on the save device
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pContextDma->pDevice == GPU_RES_GET_DEVICE(pDispChannel),
                       NV_ERR_INVALID_DEVICE);

    //
    // Enforce alignment requirements
    // ISO  ctx dmas need to be a multiple of 256B and 256B aligned
    // NISO ctx dmas need to be a multiple of 4K   and 4K   aligned
    // We can only ensure common minimum -- 4K alignment and 4K size
    // Limit alignment is handled by rounding up in lower-level code.
    // This will be in hw in future.
    //
    if (pContextDma->pMemDesc->PteAdjust != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ISO ctx dmas must be 4K aligned. PteAdjust = 0x%x\n",
                  pContextDma->pMemDesc->PteAdjust);
        return NV_ERR_INVALID_OFFSET;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    pInstMem = KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay);

    rmStatus = instmemBindContextDma(pGpu, pInstMem, pContextDma, pDispChannel);
    if (rmStatus != NV_OK)
    {
        SLI_LOOP_RETURN(rmStatus);
    }

    SLI_LOOP_END

    return NV_OK;
}

NV_STATUS
dispchnUnbindCtx_IMPL
(
    OBJGPU      *pGpu,
    ContextDma  *pContextDma,
    NvHandle     hChannel
)
{
    RsClient     *pClient = RES_GET_CLIENT(pContextDma);
    DispChannel  *pDispChannel = NULL;
    NV_STATUS  rmStatus = NV_OK;
    KernelDisplay *pKernelDisplay;
    DisplayInstanceMemory *pInstMem;
    NvBool bFound = NV_FALSE;

    // Look-up channel given by client
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        dispchnGetByHandle(pClient, hChannel, &pDispChannel));

    // Ensure ContextDma and DisplayChannel are on the save device
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pContextDma->pDevice == GPU_RES_GET_DEVICE(pDispChannel),
                       NV_ERR_INVALID_DEVICE);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    pInstMem = KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay);

    rmStatus = instmemUnbindContextDma(pGpu, pInstMem, pContextDma, pDispChannel);
    if (rmStatus == NV_OK)
    {
        bFound = NV_TRUE;
    }

    SLI_LOOP_END

    return bFound ? NV_OK : NV_ERR_INVALID_STATE;
}

/*!
 * @brief Unbind all ContextDmas from the given channel
 */
void
dispchnUnbindAllCtx_IMPL
(
    OBJGPU      *pGpu,
    DispChannel *pDispChannel
)
{
    KernelDisplay *pKernelDisplay;
    DisplayInstanceMemory *pInstMem;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    pInstMem = KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay);

    instmemUnbindDispChannelContextDmas(pGpu, pInstMem, pDispChannel);

    SLI_LOOP_END
}

/*!
 * @brief Unbind ContextDma from all display channels
 */
void
dispchnUnbindCtxFromAllChannels_IMPL
(
    OBJGPU      *pGpu,
    ContextDma  *pContextDma
)
{
    KernelDisplay *pKernelDisplay;
    DisplayInstanceMemory *pInstMem;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    pInstMem = KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay);

    instmemUnbindContextDmaFromAllChannels(pGpu, pInstMem, pContextDma);

    SLI_LOOP_END
}

NV_STATUS
kdispSetPushBufferParamsToPhysical_IMPL
(
    OBJGPU          *pGpu,
    KernelDisplay   *pKernelDisplay,
    DispChannel     *pDispChannel,
    NvHandle         hObjectBuffer,
    ContextDma      *pBufferContextDma,
    NvU32            hClass,
    NvU32            channelInstance,
    DISPCHNCLASS    internalDispChnClass
)
{
    RsClient       *pClient  = RES_GET_CLIENT(pDispChannel);
    RM_API         *pRmApi   = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS       rmStatus = NV_OK;
    NvU32           dispChannelNum;
    NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS  pushBufferParams = {0};

    rmStatus = kdispGetChannelNum_HAL(pKernelDisplay, internalDispChnClass, channelInstance, &dispChannelNum);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    pushBufferParams.hclass = hClass;
    pushBufferParams.channelInstance = channelInstance;

    if (pDispChannel->bIsDma)
    {
        rmStatus = ctxdmaGetByHandle(pClient, hObjectBuffer, &pBufferContextDma);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "disp channel[0x%x] didn't have valid ctxdma 0x%x\n",
                      channelInstance, hObjectBuffer);
            return rmStatus;
        }

        pushBufferParams.limit = pBufferContextDma->Limit;
        pushBufferParams.addressSpace = memdescGetAddressSpace(pBufferContextDma->pMemDesc);
        if ((pushBufferParams.addressSpace != ADDR_SYSMEM) && (pushBufferParams.addressSpace != ADDR_FBMEM))
        {
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
        }
        // Generate PUSHBUFFER_ADDR. Shift the addr to get the size in 4KB
        pushBufferParams.physicalAddr = memdescGetPhysAddr(memdescGetMemDescFromGpu(pBufferContextDma->pMemDesc, pGpu), AT_GPU, 0);
        pushBufferParams.cacheSnoop= pBufferContextDma->CacheSnoop;
        pushBufferParams.pbTargetAperture = kdispGetPBTargetAperture_HAL(pGpu,
                                                                         pKernelDisplay,
                                                                         pushBufferParams.addressSpace,
                                                                         pushBufferParams.cacheSnoop);
        pushBufferParams.valid = NV_TRUE;
    }
    else
    {
        pushBufferParams.valid = NV_FALSE;
    }

    pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER,
                            &pushBufferParams, sizeof(pushBufferParams));

    return NV_OK;
}
