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

#include "core/locks.h"
#include "vgpu/rpc.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "nvmisc.h"

NvBool ceIsCeGrce(OBJGPU *pGpu, RM_ENGINE_TYPE rmCeEngineType)
{
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS partnerParams = {0};
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (IsAMODEL(pGpu) || IsT234DorBetter(pGpu))
        return NV_FALSE;

    NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_COPY(rmCeEngineType), NV_FALSE);

    NvU32      i;
    NV_STATUS  status = NV_OK;

    partnerParams.engineType = gpuGetNv2080EngineType(rmCeEngineType);
    partnerParams.numPartners = 0;

    // See if the hal wants to handle this
    status = kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, &partnerParams);
    if (status != NV_OK)
    {
        // For channels that the hal didnt handle, we should just return
        // all of the supported engines except for the target engine.
        //
        // Update the engine Database
        status = gpuUpdateEngineTable(pGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not update the engine db. This is fatal\n");
            DBG_BREAKPOINT();
            return NV_FALSE;
        }

        // Make sure it all will fit
        if (pGpu->engineDB.size > NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "PartnerList space too small. This is fatal\n");
            DBG_BREAKPOINT();
            return NV_FALSE;
        }

        // Copy over all of the engines except the target
        for (i = 0; i < pGpu->engineDB.size; i++)
        {
            // Skip the engine handed in
            if (pGpu->engineDB.pType[i] != rmCeEngineType )
            {
                partnerParams.partnerList[partnerParams.numPartners++] =
                    gpuGetNv2080EngineType(pGpu->engineDB.pType[i]);
            }
        }
    }

    // check if gr is in the partnerList
    for (i = 0; i < partnerParams.numPartners; i++)
    {
        if (NV2080_ENGINE_TYPE_IS_GR(partnerParams.partnerList[i]))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * Checks if a given GRCE is partnered with the given GR engine
 * 
 * @param[in] rmCeEngineType CE Engine to check
 * @param[in] rmGrEngineType GR Engine type to check if the CE is partnered with
 * 
 * @return NvBool depending on whether the engines passed in are partnered or not. 
 */
NvBool
ceIsPartneredWithGr
(
    OBJGPU *pGpu,
    RM_ENGINE_TYPE rmCeEngineType,
    RM_ENGINE_TYPE rmGrEngineType
)
{
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS partnerParams = {0};
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    if (IsAMODEL(pGpu) || IsT234DorBetter(pGpu))
        return NV_FALSE;

    NV_ASSERT_OR_RETURN(RM_ENGINE_TYPE_IS_COPY(rmCeEngineType), NV_FALSE);

    NvU32   i;
    NV_STATUS status = NV_OK;

    partnerParams.engineType = gpuGetNv2080EngineType(rmCeEngineType);
    partnerParams.numPartners = 0;

    // See if the hal wants to handle this
    status = kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, &partnerParams);
    if (status != NV_OK)
    {
        // For channels that the hal didnt handle, we should just return
        // all of the supported engines except for the target engine.
        //
        // Update the engine Database
        status = gpuUpdateEngineTable(pGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not update the engine db. This is fatal\n");
            DBG_BREAKPOINT();
            return NV_FALSE;
        }

        NV_ASSERT_OR_RETURN(pGpu->engineDB.size <= NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS, NV_FALSE);

        // Copy over all of the engines except the target
        for (i = 0; i < pGpu->engineDB.size; i++)
        {
            // Skip the engine handed in
            if (pGpu->engineDB.pType[i] != rmCeEngineType )
            {
                partnerParams.partnerList[partnerParams.numPartners++] =
                    gpuGetNv2080EngineType(pGpu->engineDB.pType[i]);
            }
        }
    }

    // check if gr is in the partnerList
    for (i = 0; i < partnerParams.numPartners; i++)
    {
        if (NV2080_ENGINE_TYPE_IS_GR(partnerParams.partnerList[i]) &&
            (gpuGetNv2080EngineType(rmGrEngineType) == partnerParams.partnerList[i]))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NvU32 ceCountGrCe(OBJGPU *pGpu)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32      engIdx;
    NvU32      grCeCount;

    if (pKernelBus == NULL || IsAMODEL(pGpu))
        return 0;

    grCeCount = 0;

    //
    // bug 2188230: Until FIFO reports max instance id for a given engine type,
    // start by iterating over all CE indices supported by POBJGPU
    //
    for (engIdx = 0; engIdx < GPU_MAX_CES; ++engIdx)
    {
        if (kbusCheckEngine_HAL(pGpu, pKernelBus, ENG_CE(engIdx)) &&
            ceIsCeGrce(pGpu, RM_ENGINE_TYPE_COPY(engIdx)))
        {
            grCeCount++;
        }
    }

    return grCeCount;
}

//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdCeGetCapsV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvU32 ceNumber;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(pCeCapsParams->ceEngineType);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    NV_PRINTF(LEVEL_INFO, "NV2080_CTRL_CE_GET_CAPS_V2 ceEngineType = %d\n", pCeCapsParams->ceEngineType);

    if (!RM_ENGINE_TYPE_IS_COPY(rmEngineType))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OK_OR_RETURN(ceIndexFromType(pGpu,
                                           GPU_RES_GET_DEVICE(pSubdevice),
                                           rmEngineType, &ceNumber));

    {
        KernelCE *pKCe = GPU_GET_KCE(pGpu, ceNumber);

        // Return an unsupported error for not present or stubbed CEs as they are
        // not supposed to be user visible and cannot be allocated anyway.
        if (pKCe == NULL)
        {
            NV_PRINTF(LEVEL_INFO, "Skipping stubbed CE %d\n", ceNumber);
            return NV_ERR_NOT_SUPPORTED;
        }

        // now fill in caps for this CE
        return kceGetDeviceCaps(pGpu, pKCe, rmEngineType, NvP64_VALUE(pCeCapsParams->capsTbl));
    }
}

NV_STATUS
subdeviceCtrlCmdCeGetAllCaps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams
)
{
    RM_API *pRmApi;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    ct_assert(NV2080_CTRL_MAX_CES  <= NV_ARRAY_ELEMENTS(pCeCapsParams->capsTbl));

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        //
        // Since some CE capabilities depend on the nvlink topology,
        // trigger topology detection before updating the CE caps
        //
        if ((pKernelNvlink != NULL) && !knvlinkIsForcedConfig(pGpu, pKernelNvlink) &&
             kmigmgrIsMIGNvlinkP2PSupported(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu)))
        {
            knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
        }
    }

    portMemSet(pCeCapsParams, 0, sizeof(*pCeCapsParams));

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_CE_GET_ALL_PHYSICAL_CAPS,
                                           pCeCapsParams,
                                           sizeof(*pCeCapsParams)));

    {
        KernelCE *pKCe;
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        KCE_ITER_DEVICE_BEGIN(pGpu, pKCe, pDevice)
            if (pKCe->bStubbed)
                continue;

            pCeCapsParams->present |= BIT64(kceInst);

            NvU8 *pKCeCaps = pCeCapsParams->capsTbl[kceInst];

            if (pKernelNvlink != NULL)
                kceGetNvlinkCaps(pGpu, pKCe, pKCeCaps);
        KCE_ITER_END
    }

    return NV_OK;
}

// removal tracking bug: 3748354
/*!
 * Checks whether given NV2080_ENGINE_TYPE_COPYN supports decompression workloads
 *
 * @param[in]  pGpu            OBJGPU pointer
 * @param[in]  nv2080engineId  NV2080_ENGINE_TYPE_COPYN
 *
 * Returns NV_TRUE if the ceEngineType is set in the decompLceMask
 */
NvBool ceIsDecompLce(OBJGPU *pGpu, NvU32 nv2080EngineId)
{
    NvBool decompCapPresent = NV_FALSE;
    KernelCE *pKCe = NULL;

    // Find 1st available pKCe to call below func
    KCE_ITER_ALL_BEGIN(pGpu, pKCe, 0)
        if (pKCe != NULL)
            break;
    KCE_ITER_END

    if (pKCe != NULL)
        decompCapPresent = kceCheckForDecompCapability_HAL(pGpu, pKCe, nv2080EngineId);

    return decompCapPresent;
}
