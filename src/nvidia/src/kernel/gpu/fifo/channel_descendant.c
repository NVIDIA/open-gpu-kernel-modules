/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/locks.h"
#include "rmapi/event.h"
#include "rmapi/rmapi.h"
#include "rmapi/rmapi_utils.h"
#include "kernel/gpu/falcon/kernel_falcon.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "platform/sli/sli.h"

NV_STATUS
chandesConstruct_IMPL
(
    ChannelDescendant *pChannelDescendant,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
    PARAM_TO_ENGDESC_FUNCTION *pParamToEngDescFn
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    NV_STATUS         status = NV_OK;
    RsResourceRef    *pResourceRef = pCallContext->pResourceRef;
    RsResourceRef    *pParentRef = pResourceRef->pParentRef;
    RsResource       *pParent = NULL;
    KernelChannel    *pKernelChannel;
    CLASSDESCRIPTOR   internalClassDescriptor;
    CLASSDESCRIPTOR  *pClassDescriptor;
    KernelFifo       *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvBool            bMIGInUse = IS_MIG_IN_USE(pGpu);

    pParent = pParentRef->pResource;
    if (pParent == NULL)
    {
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    pKernelChannel = dynamicCast(pParent, KernelChannel);
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    // Bad class creation can happen when GPU is in low power because class DB is invalid
    NV_ASSERT(IS_VIRTUAL(pGpu) || gpuIsGpuFullPowerForPmResume(pGpu));

    NV_ASSERT(rmapiLockIsOwner() || rmapiInRtd3PmPath());

    //
    // If debug mode is enabled on this GPU, check if the GPU is occupied by a
    // long running compute object.
    //
    if (gpuIsDebuggerActive_HAL(pGpu))
    {
        return NV_ERR_BUSY_RETRY;
    }

    //
    // Lookup the CLASSDESCRIPTOR (external class id, engine tuple)
    //
    pClassDescriptor = NULL;

    //
    // Check if we have moved to per engine channel Ram, if yes we should have
    // engineType set in channel
    //
    if (kfifoIsPerRunlistChramEnabled(pKernelFifo) &&
        (!RM_ENGINE_TYPE_IS_VALID(kchannelGetEngineType(pKernelChannel))))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Channel should have engineType associated with it\n");
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    //
    // From Ampere onwards a client can't create a channel without engineType,
    // so engineType from channel gets first priority while determining class
    // descriptor. For legacy chips, we will fall back to user-allocated params
    // or default engine determination based on classId
    //
    if (kfifoIsHostEngineExpansionSupported(pKernelFifo) &&
        RM_ENGINE_TYPE_IS_VALID(kchannelGetEngineType(pKernelChannel)) &&
       ((gpuIsCCorApmFeatureEnabled(pGpu) || bMIGInUse) ||
        kfifoIsPerRunlistChramEnabled(pKernelFifo)))
    {
        if (rmapiutilIsExternalClassIdInternalOnly(pParams->externalClassId))
        {
            ENGDESCRIPTOR engDesc;

            //
            // Internal classes do not appear in the classdb, as they are not
            // allowed to be allocated directly from usermode. Use the channel's
            // engine type to determine the engdesc and don't bother checking
            // the classdb.
            //
            NV_ASSERT_OK_OR_RETURN(
                kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                         ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32)kchannelGetEngineType(pKernelChannel),
                                         ENGINE_INFO_TYPE_ENG_DESC, &engDesc));
            portMemSet(&internalClassDescriptor, 0, sizeof(internalClassDescriptor));
            internalClassDescriptor.externalClassId = pParams->externalClassId;
            internalClassDescriptor.engDesc = engDesc;
            pClassDescriptor = &internalClassDescriptor;
        }
        else
        {
            // bypass this check for ENG_SW classes, as they can be under any engine
            status = gpuGetClassByClassId(pGpu, pParams->externalClassId, &pClassDescriptor);
            if ((status != NV_OK) || (pClassDescriptor->engDesc != ENG_SW))
            {
                NvU32 engDesc;
                RM_ENGINE_TYPE rmEngineType = kchannelGetEngineType(pKernelChannel);
                // detect the GRCE case where we may be allocating a CE object on GR channel
                if ((status == NV_OK) && IS_CE(pClassDescriptor->engDesc) && RM_ENGINE_TYPE_IS_GR(rmEngineType))
                {
                    //
                    // Get the partner CE of GR engine based on runqueue of this channel
                    // Use this partner CE alongside externalClassId to fetch the correct class descriptor
                    //
                    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS partnerParams = {0};
                    partnerParams.engineType = gpuGetNv2080EngineType(rmEngineType);
                    partnerParams.runqueue = kchannelGetRunqueue(pKernelChannel);
                    NV_ASSERT_OK_OR_RETURN(kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, &partnerParams));
                    NV_ASSERT_OR_RETURN(partnerParams.numPartners == 1, NV_ERR_INVALID_STATE);
                    rmEngineType = gpuGetRmEngineType(partnerParams.partnerList[0]);
                }

                // Get the engDesc from engineType
                NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                                ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                                                (NvU32)rmEngineType,
                                                                ENGINE_INFO_TYPE_ENG_DESC,
                                                                &engDesc));

                status = gpuGetClassByEngineAndClassId(pGpu, pParams->externalClassId,
                                                       engDesc, &pClassDescriptor);
            }
        }
    }
    else if ((pParams->pAllocParams != NULL) && (pParamToEngDescFn != NULL))
    {
        //
        // For classes like copy engine the per-engine code determines which
        // engine index to use based on the allocation params.
        //
        ENGDESCRIPTOR engDesc = pParamToEngDescFn(pGpu, pParams->externalClassId,
                                                  pParams->pAllocParams);

        if (rmapiutilIsExternalClassIdInternalOnly(pParams->externalClassId))
        {
            //
            // Internal classes do not appear in the classdb, as they are not
            // allowed to be allocated directly from usermode. Use the internal
            // class's paramToEngDescFn to determine the engdesc and don't
            // bother checking the classdb.
            //
            portMemSet(&internalClassDescriptor, 0, sizeof(internalClassDescriptor));
            internalClassDescriptor.externalClassId = pParams->externalClassId;
            internalClassDescriptor.engDesc = engDesc;
            pClassDescriptor = &internalClassDescriptor;
            status = NV_OK;
        }
        else if (engDesc != ENG_INVALID)
        {
            status = gpuGetClassByEngineAndClassId(pGpu, pParams->externalClassId,
                                                   engDesc, &pClassDescriptor);
        }
        else
        {
            status = NV_ERR_INVALID_CLASS;
        }
    }
    else
    {
        status = gpuGetClassByClassId(pGpu, pParams->externalClassId, &pClassDescriptor);
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "bad class 0x%x\n", pParams->externalClassId);
        return NV_ERR_INVALID_CLASS;
    }

    ENGDESCRIPTOR engDesc = pClassDescriptor->engDesc;

    //
    // Verify the engine exists
    //
    if (IS_GR(engDesc))
    {
        //
        // Graphics engine can be disabled on kernel RM, so instead we are checking
        // the existence of KernelGraphics engine here when engDesc = ENG_GR(X)
        //
        engDesc = MKENGDESC(classId(KernelGraphics), GET_GR_IDX(engDesc));
    }
    //
    // skip checking engstate for CE, engine guaranteed to exist if pParamToEngDescFn is succesful
    //
    if (!IS_CE(engDesc))
    {
        void *pEngObject = gpuGetEngstate(pGpu, engDesc);
        //
        // In a kernel-only config, falcons are represented by KernelFalcons and do not have an
        // engstate.
        //
        if (pEngObject == NULL)
            pEngObject = kflcnGetKernelFalconForEngine(pGpu, engDesc);

        if (pEngObject == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "engine is missing for class 0x%x\n",
                    pParams->externalClassId);
            return NV_ERR_INVALID_CLASS;
        }
    }

    pChannelDescendant->pKernelChannel = pKernelChannel;

    pChannelDescendant->resourceDesc.externalClassId = pClassDescriptor->externalClassId;
    pChannelDescendant->resourceDesc.engDesc         = pClassDescriptor->engDesc;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    status = kfifoRunlistSetIdByEngine_HAL(pGpu, pKernelFifo, pKernelChannel, pChannelDescendant->resourceDesc.engDesc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid object allocation request on channel:0x%08x\n",
                  kchannelGetDebugTag(pKernelChannel));
        SLI_LOOP_RETURN(status);
    }
    SLI_LOOP_END

    status = kchannelRegisterChild(pKernelChannel, pChannelDescendant);

    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

void
chandesDestruct_IMPL
(
    ChannelDescendant *pChannelDescendant
)
{
    NV_STATUS          status;

    // scrub event references for this object
    CliDelObjectEvents(RES_GET_REF(pChannelDescendant));

    status = kchannelDeregisterChild(pChannelDescendant->pKernelChannel, pChannelDescendant);
    NV_ASSERT(status == NV_OK);

    chandesDestroy_HAL(pChannelDescendant);
}

NV_STATUS
chandesGetSwMethods_IMPL
(
    ChannelDescendant *pChannelDescendant,
    const METHOD     **ppMethods,
    NvU32             *pNumMethods
)
{
    // Default behavior is SW methods not supported. Subclasses can implement
    // handlers if required.
    return NV_ERR_NOT_SUPPORTED;
}

//---------------------------------------------------------------------------
//
// Helpers for ChannelDescendant::chandesGetSwMethods method table
//
//---------------------------------------------------------------------------

NV_STATUS mthdNoOperation
(
    OBJGPU *pGpu,
    ChannelDescendant *Object,
    NvU32   Offset,
    NvU32   Data
)
{
    NV_PRINTF(LEVEL_INFO, "Method NoOperation: Class=0x%x Data=0x%x\n",
              Object->resourceDesc.externalClassId, Data);
    return (NV_OK);
}

/*
 * Check whether the software method should? * stall the PBDMA through the execution
 * of the software method. By default - yes
 */
NvBool chandesIsSwMethodStalling_IMPL(ChannelDescendant *pChannelDescendant, NvU32 hHandle)
{
    return NV_TRUE;
}

NV_STATUS
chandesCheckMemInterUnmap_IMPL
(
    ChannelDescendant *pChannelDescendant,
    NvBool bSubdeviceHandleProvided
)
{
    if (bSubdeviceHandleProvided)
    {
        NV_PRINTF(LEVEL_ERROR, "Unicast DMA mappings of non-memory objects not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}
