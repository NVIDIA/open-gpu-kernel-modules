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

// FIXME XXX
#define NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/ce/kernel_ce_shared.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "vgpu/vgpu_events.h"

#include "published/ampere/ga100/dev_ram.h"
#include "published/ampere/ga100/dev_ctrl.h"


NV_STATUS
kfifoEngineInfoXlate_GA100
(
    OBJGPU          *pGpu,
    KernelFifo      *pKernelFifo,
    ENGINE_INFO_TYPE inType,
    NvU32            inVal,
    ENGINE_INFO_TYPE outType,
    NvU32           *pOutVal
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    // We no longer store ENGINE_INFO_TYPE_INTR on Ampere+ (bug 24110055)
    if (inType == ENGINE_INFO_TYPE_INTR || outType == ENGINE_INFO_TYPE_INTR)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // We need extra logic for translation when SMC is enabled and input or output is
    // MMU_FAULT_ID because device Info cannot translate MMU_FAULT_ID to/from any type for GR > 0
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        NvU32 baseGrFaultId;
        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_GV100(pGpu, pKernelFifo,
                                                          ENGINE_INFO_TYPE_ENG_DESC, ENG_GR(0),
                                                          ENGINE_INFO_TYPE_MMU_FAULT_ID, &baseGrFaultId));

        if (inType == ENGINE_INFO_TYPE_MMU_FAULT_ID)
        {
            NvU32 subctxId, grIdx;
            NvU32 maxSubctx = kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, NV_FALSE);

            // check if input fault ID corresponds to GR
            if ((inVal >= baseGrFaultId) && (inVal < (baseGrFaultId + maxSubctx)))
            {
                subctxId = inVal - baseGrFaultId;
                NV_ASSERT_OK_OR_RETURN(kgrmgrGetGrIdxForVeid(pGpu, pKernelGraphicsManager, subctxId, &grIdx));
                inVal = RM_ENGINE_TYPE_GR(grIdx);
                inType = ENGINE_INFO_TYPE_RM_ENGINE_TYPE;
            }
        }

        if (outType == ENGINE_INFO_TYPE_MMU_FAULT_ID)
        {
            NvU32 grIdx, startSubctxId;
            NV_STATUS status;
            RM_ENGINE_TYPE rmEngineType;

            status = kfifoEngineInfoXlate_GV100(pGpu, pKernelFifo, inType, inVal,
                                                ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType);
            if (status != NV_OK)
                return status;

            // check if rmEngineType corresponding to input is GR
            if (RM_ENGINE_TYPE_IS_GR(rmEngineType))
            {
                grIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);
                NV_ASSERT_OK_OR_RETURN(kgrmgrGetVeidBaseForGrIdx(pGpu, pKernelGraphicsManager, grIdx, &startSubctxId));
                *pOutVal = baseGrFaultId + startSubctxId;
                return NV_OK;
            }
        }
    }

    return kfifoEngineInfoXlate_GV100(pGpu, pKernelFifo, inType, inVal, outType, pOutVal);
}


/*!
 * @brief Get the local maximum number of subctx allowed in this TSG
 *
 * @param      pGpu
 * @param      pKernelFifo
 * @param[in]  pKernelChannelGroup
 * @param[in]  bLegacyMode          Is TSG in legacy mode.
 */
NvU32
kfifoChannelGroupGetLocalMaxSubcontext_GA100
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    KernelChannelGroup *pKernelChannelGroup,
    NvBool              bLegacyMode
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_MIG_IN_USE(pGpu) && !bLegacyMode &&
        RM_ENGINE_TYPE_IS_GR(pKernelChannelGroup->engineType))
    {
        NvU32 grIdx = RM_ENGINE_TYPE_GR_IDX(pKernelChannelGroup->engineType);
        return nvPopCount64(pKernelGraphicsManager->grIdxVeidMask[grIdx]);
    }

    // In SMC-Legacy mode, revert to pre-Ampere behavior
    return kfifoChannelGroupGetLocalMaxSubcontext_GM107(pGpu, pKernelFifo,
                                                        pKernelChannelGroup,
                                                        bLegacyMode);
}

/*!
 * @brief Update the usermode doorbell register with work submit token to notify
 *        host that work is available on this channel.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] workSubmitToken Token to update the doorbell with
 * @param[in] runlistId       Runlist ID
 */
NV_STATUS
kfifoUpdateUsermodeDoorbell_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       workSubmitToken,
    NvU32       runlistId
)
{
    //
    // Updating the usermode doorbell is different for CPU vs. GSP.
    //
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        return kfifoUpdateUsermodeDoorbell_TU102(pGpu, pKernelFifo, workSubmitToken, runlistId);
    }
    else
    {
        return kfifoUpdateInternalDoorbellForUsermode_HAL(pGpu, pKernelFifo, workSubmitToken, runlistId);
    }

    return NV_OK;
}

/*!
 * @brief Construct the worksubmit token. Caller cannot make assumption about this handle.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[in]  pKernelChannel
 * @param[out] pGeneratedToken Store the generated token
 * @param[in]  bUsedForHost    Used on Host RM
 *
 */
NV_STATUS
kfifoGenerateWorkSubmitToken_GA100
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32         *pGeneratedToken,
    NvBool         bUsedForHost
)
{
    NvU32          chId;
    NvU32          gfId;
    NvU32          runlistId;
    NvU32          val = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    NV_ASSERT_OR_RETURN(pGeneratedToken != NULL, NV_ERR_INVALID_OBJECT);
    NV_ASSERT_OR_RETURN((pKernelChannel->pKernelChannelGroupApi != NULL) &&
                        (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup != NULL),
                        NV_ERR_INVALID_STATE);

    chId = pKernelChannel->ChID;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfId));

    if (!RMCFG_FEATURE_PLATFORM_GSP || (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_GFID_VF(gfId)))
    {

        // TODO: Remove check on Ampere. Bug 200606706.
        if (!bUsedForHost && IS_GFID_VF(gfId))
        {
            NvU32 vChId;

            NV_ASSERT_OK_OR_RETURN(kfifoGetVChIdForSChId_HAL(pGpu, pKernelFifo,
                                                             chId, gfId,
                                                             kchannelGetEngineType(pKernelChannel),
                                                             &vChId));
            chId = vChId;
        }

        // TODO: Remove, on Ampere channels should be set to a valid runlist before allocation. Bug 200606706.
        if (!kchannelIsRunlistSet(pGpu, pKernelChannel))
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "FAILED Channel 0x%x is not assigned to runlist yet\n",
                      chId);
            return NV_ERR_INVALID_STATE;
        }

        runlistId = kchannelGetRunlistId(pKernelChannel);

        // Here we construct token to be a concatenation of runlist id and channel id
        val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _RUNLIST_ID, runlistId, val);
        val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _VECTOR,     chId,      val);

        NV_PRINTF(LEVEL_INFO,
                  "Generated workSubmitToken 0x%x for channel 0x%x runlist 0x%x\n",
                  val, chId, runlistId);
    }
    else // RMCFG_FEATURE_PLATFORM_GSP
    {
        NV_ASSERT_OK_OR_RETURN(kfifoGenerateInternalWorkSubmitToken_HAL(pGpu, pKernelFifo, pKernelChannel));
    }

    *pGeneratedToken = val;

    return NV_OK;
}

/**
 * @brief Get the runlist base shift amount
 *
 * @param pKernelFifo
 *
 * @return shift amount
 */
NvU32
kfifoRunlistGetBaseShift_GA100
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_BASE_SHIFT;
}

/*!
 * Special function to be used early when the CHID_MGRs aren't and cannot be
 * constructed in all cases. Do not use otherwise
 */
NvU32
kfifoGetMaxCeChannelGroups_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    ENGDESCRIPTOR eng = 0;
    NvU32 deviceIndex;
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 maxCeChannels = 0;

    // If called before kfifoConstructEngineList has executed
    if (pEngineInfo == NULL)
        return 0;

    //
    // We can't use chidMgr here as this gets called before chidMgr is completely initialized
    // Use device info table instead
    //
    for (deviceIndex = 0; deviceIndex < pEngineInfo->engineInfoListSize; deviceIndex++)
    {
        eng = pEngineInfo->engineInfoList[deviceIndex].engineData[ENGINE_INFO_TYPE_ENG_DESC];

        // All GR CE use the same pool as GR
        if ((eng == ENG_GR(0)) ||
            (IS_CE(eng) &&
             (!ceIsCeGrce(pGpu, pEngineInfo->engineInfoList[deviceIndex].engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE]))))
        {
            maxCeChannels += kfifoRunlistQueryNumChannels_HAL(pGpu, pKernelFifo, 0);
        }
    }

    // override max channels if we can run out of BAR2 page tables
    if (kbusIsBug2751296LimitBar2PtSize(GPU_GET_KERNEL_BUS(pGpu)))
    {
        // 2k for GR CE and 2k for the rest
        maxCeChannels = 4096;
    }

    return maxCeChannels;
}
