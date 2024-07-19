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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/ce/kernel_ce_shared.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "vgpu/vgpu_events.h"

#include "published/ampere/ga100/dev_fault.h"
#include "published/ampere/ga100/dev_ram.h"
#include "published/ampere/ga100/dev_ctrl.h"
#include "published/ampere/ga100/dev_runlist.h"

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
        return nvPopCount64(kgrmgrGetGrIdxVeidMask(pGpu, pKernelGraphicsManager, grIdx));
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
        NV_ASSERT_OK_OR_RETURN(kfifoGenerateInternalWorkSubmitToken_HAL(pGpu, pKernelFifo, pKernelChannel, &val));
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

/*!
 * @brief Provides PBDMA Fault IDs for requested engines
 *
 * @param[IN]   pGpu         Pointer to OBJGPU instance
 * @param[IN]   pKernelFifo  Pointer to KernelFifo instance
 * @param[IN]   type         Engine_info_type to determine if en engine need to be searched
 *                           or not.
 * @param[IN]   val          Engine value associated with the requested INFO_TYPE
 * @param[OUT]  ppPbdmaFaultIds  Buffer to fill output PBDMA_FAULT_IDS
 * @param[OUT]  pNumPbdmas   Number of PBDMAs or valid buffer entries in above buffer
 *
 */
NV_STATUS
kfifoGetEnginePbdmaFaultIds_GA100
(
    OBJGPU             *pGpu,
    KernelFifo         *pKernelFifo,
    ENGINE_INFO_TYPE    type,
    NvU32               val,
    NvU32             **ppPbdmaFaultIds,
    NvU32              *pNumPbdmas
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 i;

    *pNumPbdmas = 0;

    if (pEngineInfo == NULL)
    {
        NV_ASSERT_OK_OR_RETURN(kfifoConstructEngineList_HAL(pGpu, pKernelFifo));

        pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
        NV_ASSERT_OR_RETURN(pEngineInfo != NULL, NV_ERR_INVALID_STATE);
    }

    if (type == ENGINE_INFO_TYPE_INVALID)
    {
        NV_ASSERT_OR_RETURN(val < pEngineInfo->engineInfoListSize, NV_ERR_INVALID_ARGUMENT);
        *ppPbdmaFaultIds = pEngineInfo->engineInfoList[val].pbdmaFaultIds;
        *pNumPbdmas = pEngineInfo->engineInfoList[val].numPbdmas;
        return NV_OK;
    }

    for (i = 0; i < pEngineInfo->engineInfoListSize; i++)
    {
        if (pEngineInfo->engineInfoList[i].engineData[type] == val)
        {
            *ppPbdmaFaultIds = pEngineInfo->engineInfoList[i].pbdmaFaultIds;
            *pNumPbdmas = pEngineInfo->engineInfoList[i].numPbdmas;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/**
 * @brief Get the number of PBDMAs
 *
 * @param pGpu         OBJGPU pointer
 * @param pKernelFifo  KernelFifo pointer
 */
NvU32
kfifoGetNumPBDMAs_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);

    NV_PRINTF(LEVEL_INFO, "%d PBDMAs\n", pEngineInfo->maxNumPbdmas);
    NV_ASSERT(pEngineInfo->maxNumPbdmas != 0);

    return pEngineInfo->maxNumPbdmas;
}

/**
* @brief Convert PBDMA ID to string
* @param[in] pGpu
* @param[in] pKernelFifo
* @param[in] pbdmaId
*
* @return cont string
*/
const char*
kfifoPrintPbdmaId_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       pbdmaId
)
{
    NV_ASSERT_OR_RETURN(pbdmaId < kfifoGetNumPBDMAs_HAL(pGpu, pKernelFifo), "UNKNOWN");
    static const char* pbdmaIdString[64] = { "HOST0" , "HOST1" , "HOST2" , "HOST3" , "HOST4" , "HOST5" , "HOST6" , "HOST7" ,
                                             "HOST8" , "HOST9" , "HOST10", "HOST11", "HOST12", "HOST13", "HOST14", "HOST15",
                                             "HOST16", "HOST17", "HOST18", "HOST19", "HOST20", "HOST21", "HOST22", "HOST23",
                                             "HOST24", "HOST25", "HOST26", "HOST27", "HOST28", "HOST29", "HOST30", "HOST31",
                                             "HOST32", "HOST33", "HOST34", "HOST35", "HOST36", "HOST37", "HOST38", "HOST39",
                                             "HOST40", "HOST41", "HOST42", "HOST43", "HOST44", "HOST45", "HOST46", "HOST47",
                                             "HOST48", "HOST49", "HOST50", "HOST51", "HOST52", "HOST53", "HOST54", "HOST55",
                                             "HOST56", "HOST57", "HOST58", "HOST59", "HOST60", "HOST61", "HOST62", "HOST63"};

    return pbdmaIdString[pbdmaId];
}

/*!
 * @brief Converts a mmu engine id (NV_PFAULT_MMU_ENG_ID_*) into a string.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineID NV_PFAULT_MMU_ENG_ID_*
 *
 * @returns a string (always non-null)
 */
const char*
kfifoPrintInternalEngine_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    NV_STATUS   status = NV_OK;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32       pbdmaId;
    NvU32       engTag;

    if (kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, engineID))
    {
        NV_ASSERT_OR_RETURN(kfifoGetPbdmaIdFromMmuFaultId(pGpu, pKernelFifo, engineID, &pbdmaId) == NV_OK, "UNKNOWN");
        return kfifoPrintPbdmaId_HAL(pGpu, pKernelFifo, pbdmaId);
    }

    if (kgmmuIsFaultEngineBar1_HAL(pKernelGmmu, engineID))
    {
        return "BAR1";
    }
    else if (kgmmuIsFaultEngineBar2_HAL(pKernelGmmu, engineID))
    {
        return "BAR2";
    }

    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_DISPLAY:
            return "DISPLAY";
        case NV_PFAULT_MMU_ENG_ID_IFB:
            return "IFB";
        case NV_PFAULT_MMU_ENG_ID_SEC:
            return "SEC";
        case NV_PFAULT_MMU_ENG_ID_PERF:
            return "PERF";
        case NV_PFAULT_MMU_ENG_ID_NVDEC0:
            return "NVDEC0";
        case NV_PFAULT_MMU_ENG_ID_NVDEC1:
            return "NVDEC1";
        case NV_PFAULT_MMU_ENG_ID_NVDEC2:
            return "NVDEC2";
        case NV_PFAULT_MMU_ENG_ID_NVDEC3:
            return "NVDEC3";
        case NV_PFAULT_MMU_ENG_ID_CE0:
            return "CE0";
        case NV_PFAULT_MMU_ENG_ID_CE1:
            return "CE1";
        case NV_PFAULT_MMU_ENG_ID_CE2:
            return "CE2";
        case NV_PFAULT_MMU_ENG_ID_CE3:
            return "CE3";
        case NV_PFAULT_MMU_ENG_ID_CE4:
            return "CE4";
        case NV_PFAULT_MMU_ENG_ID_CE5:
            return "CE5";
        case NV_PFAULT_MMU_ENG_ID_CE6:
            return "CE6";
        case NV_PFAULT_MMU_ENG_ID_CE7:
            return "CE7";
        case NV_PFAULT_MMU_ENG_ID_CE8:
            return "CE8";
        case NV_PFAULT_MMU_ENG_ID_CE9:
            return "CE9";
        case NV_PFAULT_MMU_ENG_ID_PWR_PMU:
            return "PMU";
        case NV_PFAULT_MMU_ENG_ID_PTP:
            return "PTP";
        case NV_PFAULT_MMU_ENG_ID_NVENC1:
            return "NVENC1";
        case NV_PFAULT_MMU_ENG_ID_PHYSICAL:
            return "PHYSICAL";
        case NV_PFAULT_MMU_ENG_ID_NVJPG0:
            return "NVJPG";
        case NV_PFAULT_MMU_ENG_ID_OFA0:
            return "OFA";
        case NV_PFAULT_MMU_ENG_ID_FLA:
            return "FLA";
        default:
        {
            const char *engine =
                    kfifoPrintInternalEngineCheck_HAL(pGpu, pKernelFifo, engineID);
            if (engine != NULL)
            {
                return engine;
            }
        }
    }

    status = kfifoEngineInfoXlate_HAL(pGpu,
                                      pKernelFifo,
                                      ENGINE_INFO_TYPE_MMU_FAULT_ID,
                                      engineID,
                                      ENGINE_INFO_TYPE_ENG_DESC,
                                      &engTag);
    if ((NV_OK == status) && IS_GR(engTag))
    {
        switch (engTag)
        {
            case ENG_GR(0):
                return "GRAPHICS";
            case ENG_GR(1):
                return "GR1";
            case ENG_GR(2):
                return "GR2";
            case ENG_GR(3):
                return "GR3";
            case ENG_GR(4):
                return "GR4";
            case ENG_GR(5):
                return "GR5";
            case ENG_GR(6):
                return "GR6";
            case ENG_GR(7):
                return "GR7";
        }
    }

    return "UNKNOWN";
}

/*!
 * @brief Converts a mmu engine id (NV_PFAULT_MMU_ENG_ID_*) into a string.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineID NV_PFAULT_MMU_ENG_ID_*
 *
 * @returns a string
 */
const char*
kfifoPrintInternalEngineCheck_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_NVDEC4:
            return "NVDEC4";
        case NV_PFAULT_MMU_ENG_ID_NVENC0:
            return "NVENC0";
        default:
            return NULL;
    }
}

/**
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdString_GA100
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptInfo
)
{
    if (pMmuExceptInfo->bGpc)
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_GPC_T1_0:
                return "GPCCLIENT_T1_0";
            case NV_PFAULT_CLIENT_GPC_T1_1:
                return "GPCCLIENT_T1_1";
            case NV_PFAULT_CLIENT_GPC_T1_2:
                return "GPCCLIENT_T1_2";
            case NV_PFAULT_CLIENT_GPC_T1_3:
                return "GPCCLIENT_T1_3";
            case NV_PFAULT_CLIENT_GPC_T1_4:
                return "GPCCLIENT_T1_4";
            case NV_PFAULT_CLIENT_GPC_T1_5:
                return "GPCCLIENT_T1_5";
            case NV_PFAULT_CLIENT_GPC_T1_6:
                return "GPCCLIENT_T1_6";
            case NV_PFAULT_CLIENT_GPC_T1_7:
                return "GPCCLIENT_T1_7";
            case NV_PFAULT_CLIENT_GPC_PE_0:
                return "GPCCLIENT_PE_0";
            case NV_PFAULT_CLIENT_GPC_PE_1:
                return "GPCCLIENT_PE_1";
            case NV_PFAULT_CLIENT_GPC_PE_2:
                return "GPCCLIENT_PE_2";
            case NV_PFAULT_CLIENT_GPC_PE_3:
                return "GPCCLIENT_PE_3";
            case NV_PFAULT_CLIENT_GPC_PE_4:
                return "GPCCLIENT_PE_4";
            case NV_PFAULT_CLIENT_GPC_PE_5:
                return "GPCCLIENT_PE_5";
            case NV_PFAULT_CLIENT_GPC_PE_6:
                return "GPCCLIENT_PE_6";
            case NV_PFAULT_CLIENT_GPC_PE_7:
                return "GPCCLIENT_PE_7";
            case NV_PFAULT_CLIENT_GPC_RAST:
                return "GPCCLIENT_RAST";
            case NV_PFAULT_CLIENT_GPC_GCC:
                return "GPCCLIENT_GCC";
            case NV_PFAULT_CLIENT_GPC_GPCCS:
                return "GPCCLIENT_GPCCS";
            case NV_PFAULT_CLIENT_GPC_PROP_0:
                return "GPCCLIENT_PROP_0";
            case NV_PFAULT_CLIENT_GPC_PROP_1:
                return "GPCCLIENT_PROP_1";
            case NV_PFAULT_CLIENT_GPC_T1_8:
                return "GPCCLIENT_T1_8";
            case NV_PFAULT_CLIENT_GPC_T1_9:
                return "GPCCLIENT_T1_9";
            case NV_PFAULT_CLIENT_GPC_T1_10:
                return "GPCCLIENT_T1_10";
            case NV_PFAULT_CLIENT_GPC_T1_11:
                return "GPCCLIENT_T1_11";
            case NV_PFAULT_CLIENT_GPC_T1_12:
                return "GPCCLIENT_T1_12";
            case NV_PFAULT_CLIENT_GPC_T1_13:
                return "GPCCLIENT_T1_13";
            case NV_PFAULT_CLIENT_GPC_T1_14:
                return "GPCCLIENT_T1_14";
            case NV_PFAULT_CLIENT_GPC_T1_15:
                return "GPCCLIENT_T1_15";
            case NV_PFAULT_CLIENT_GPC_TPCCS_0:
                return "GPCCLIENT_TPCCS_0";
            case NV_PFAULT_CLIENT_GPC_TPCCS_1:
                return "GPCCLIENT_TPCCS_1";
            case NV_PFAULT_CLIENT_GPC_TPCCS_2:
                return "GPCCLIENT_TPCCS_2";
            case NV_PFAULT_CLIENT_GPC_TPCCS_3:
                return "GPCCLIENT_TPCCS_3";
            case NV_PFAULT_CLIENT_GPC_TPCCS_4:
                return "GPCCLIENT_TPCCS_4";
            case NV_PFAULT_CLIENT_GPC_TPCCS_5:
                return "GPCCLIENT_TPCCS_5";
            case NV_PFAULT_CLIENT_GPC_TPCCS_6:
                return "GPCCLIENT_TPCCS_6";
            case NV_PFAULT_CLIENT_GPC_TPCCS_7:
                return "GPCCLIENT_TPCCS_7";
            case NV_PFAULT_CLIENT_GPC_PE_8:
                return "GPCCLIENT_PE_8";
            case NV_PFAULT_CLIENT_GPC_TPCCS_8:
                return "GPCCLIENT_TPCCS_8";
            case NV_PFAULT_CLIENT_GPC_T1_16:
                return "GPCCLIENT_T1_16";
            case NV_PFAULT_CLIENT_GPC_T1_17:
                return "GPCCLIENT_T1_17";
            case NV_PFAULT_CLIENT_GPC_ROP_0:
                return "GPCCLIENT_ROP_0";
            case NV_PFAULT_CLIENT_GPC_ROP_1:
                return "GPCCLIENT_ROP_1";
            case NV_PFAULT_CLIENT_GPC_GPM:
                return "GPCCLIENT_GPM";
            default:
                return "UNRECOGNIZED_CLIENT";
        }
    }
    else
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_VIP:
                return "HUBCLIENT_VIP";
            case NV_PFAULT_CLIENT_HUB_CE0:
                return "HUBCLIENT_CE0";
            case NV_PFAULT_CLIENT_HUB_CE1:
                return "HUBCLIENT_CE1";
            case NV_PFAULT_CLIENT_HUB_DNISO:
                return "HUBCLIENT_DNISO";
            case NV_PFAULT_CLIENT_HUB_FE:
                return "HUBCLIENT_FE";
            case NV_PFAULT_CLIENT_HUB_FECS:
                return "HUBCLIENT_FECS";
            case NV_PFAULT_CLIENT_HUB_HOST:
                return "HUBCLIENT_HOST";
            case NV_PFAULT_CLIENT_HUB_HOST_CPU:
                return "HUBCLIENT_HOST_CPU";
            case NV_PFAULT_CLIENT_HUB_HOST_CPU_NB:
                return "HUBCLIENT_HOST_CPU_NB";
            case NV_PFAULT_CLIENT_HUB_ISO:
                return "HUBCLIENT_ISO";
            case NV_PFAULT_CLIENT_HUB_MMU:
                return "HUBCLIENT_MMU";
            case NV_PFAULT_CLIENT_HUB_NVDEC0:
                return "HUBCLIENT_NVDEC0";
            case NV_PFAULT_CLIENT_HUB_NVENC1:
                return "HUBCLIENT_NVENC1";
            case NV_PFAULT_CLIENT_HUB_NISO:
                return "HUBCLIENT_NISO";
            case NV_PFAULT_CLIENT_HUB_P2P:
                return "HUBCLIENT_P2P";
            case NV_PFAULT_CLIENT_HUB_PD:
                return "HUBCLIENT_PD";
            case NV_PFAULT_CLIENT_HUB_PERF0:
                return "HUBCLIENT_PERF";
            case NV_PFAULT_CLIENT_HUB_PMU:
                return "HUBCLIENT_PMU";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD:
                return "HUBCLIENT_RASTERTWOD";
            case NV_PFAULT_CLIENT_HUB_SCC:
                return "HUBCLIENT_SCC";
            case NV_PFAULT_CLIENT_HUB_SCC_NB:
                return "HUBCLIENT_SCC_NB";
            case NV_PFAULT_CLIENT_HUB_SEC:
                return "HUBCLIENT_SEC";
            case NV_PFAULT_CLIENT_HUB_SSYNC:
                return "HUBCLIENT_SSYNC";
            case NV_PFAULT_CLIENT_HUB_CE2:
                return "HUBCLIENT_CE2";
            case NV_PFAULT_CLIENT_HUB_XV:
                return "HUBCLIENT_XV";
            case NV_PFAULT_CLIENT_HUB_MMU_NB:
                return "HUBCLIENT_MMU_NB";
            case NV_PFAULT_CLIENT_HUB_DFALCON:
                return "HUBCLIENT_DFALCON";
            case NV_PFAULT_CLIENT_HUB_SKED:
                return "HUBCLIENT_SKED";
            case NV_PFAULT_CLIENT_HUB_AFALCON:
                return "HUBCLIENT_AFALCON";
            case NV_PFAULT_CLIENT_HUB_DONT_CARE:
                return "HUBCLIENT_DONT_CARE";
            case NV_PFAULT_CLIENT_HUB_HSCE0:
                return "HUBCLIENT_HSCE0";
            case NV_PFAULT_CLIENT_HUB_HSCE1:
                return "HUBCLIENT_HSCE1";
            case NV_PFAULT_CLIENT_HUB_HSCE2:
                return "HUBCLIENT_HSCE2";
            case NV_PFAULT_CLIENT_HUB_HSCE3:
                return "HUBCLIENT_HSCE3";
            case NV_PFAULT_CLIENT_HUB_HSCE4:
                return "HUBCLIENT_HSCE4";
            case NV_PFAULT_CLIENT_HUB_HSCE5:
                return "HUBCLIENT_HSCE5";
            case NV_PFAULT_CLIENT_HUB_HSCE6:
                return "HUBCLIENT_HSCE6";
            case NV_PFAULT_CLIENT_HUB_HSCE7:
                return "HUBCLIENT_HSCE7";
            case NV_PFAULT_CLIENT_HUB_HSCE8:
                return "HUBCLIENT_HSCE8";
            case NV_PFAULT_CLIENT_HUB_HSCE9:
                return "HUBCLIENT_HSCE9";
            case NV_PFAULT_CLIENT_HUB_HSHUB:
                return "HUBCLIENT_HSHUB";
            case NV_PFAULT_CLIENT_HUB_PTP_X0:
                return "HUBCLIENT_PTP_X0";
            case NV_PFAULT_CLIENT_HUB_PTP_X1:
                return "HUBCLIENT_PTP_X1";
            case NV_PFAULT_CLIENT_HUB_PTP_X2:
                return "HUBCLIENT_PTP_X2";
            case NV_PFAULT_CLIENT_HUB_PTP_X3:
                return "HUBCLIENT_PTP_X3";
            case NV_PFAULT_CLIENT_HUB_PTP_X4:
                return "HUBCLIENT_PTP_X4";
            case NV_PFAULT_CLIENT_HUB_PTP_X5:
                return "HUBCLIENT_PTP_X5";
            case NV_PFAULT_CLIENT_HUB_PTP_X6:
                return "HUBCLIENT_PTP_X6";
            case NV_PFAULT_CLIENT_HUB_PTP_X7:
                return "HUBCLIENT_PTP_X7";
            case NV_PFAULT_CLIENT_HUB_NVENC2:
                return "HUBCLIENT_NVENC2";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER0:
                return "HUBCLIENT_VPR_SCRUBBER0";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER1:
                return "HUBCLIENT_VPR_SCRUBBER1";
            case NV_PFAULT_CLIENT_HUB_DWBIF:
                return "HUBCLIENT_DWBIF";
            case NV_PFAULT_CLIENT_HUB_FBFALCON:
                return "HUBCLIENT_FBFALCON";
            case NV_PFAULT_CLIENT_HUB_CE_SHIM:
                return "HUBCLIENT_CE_SHIM";
            case NV_PFAULT_CLIENT_HUB_GSP:
                return "HUBCLIENT_GSP";
            case NV_PFAULT_CLIENT_HUB_NVDEC1:
                return "HUBCLIENT_NVDEC1";
            case NV_PFAULT_CLIENT_HUB_NVDEC2:
                return "HUBCLIENT_NVDEC2";
            case NV_PFAULT_CLIENT_HUB_NVJPG0:
                return "HUBCLIENT_NVJPG0";
            case NV_PFAULT_CLIENT_HUB_NVDEC3:
                return "HUBCLIENT_NVDEC3";
            case NV_PFAULT_CLIENT_HUB_OFA0:
                return "HUBCLIENT_OFA0";
            case NV_PFAULT_CLIENT_HUB_HSCE10:
                return "HUBCLIENT_HSCE10";
            case NV_PFAULT_CLIENT_HUB_HSCE11:
                return "HUBCLIENT_HSCE11";
            case NV_PFAULT_CLIENT_HUB_HSCE12:
                return "HUBCLIENT_HSCE12";
            case NV_PFAULT_CLIENT_HUB_HSCE13:
                return "HUBCLIENT_HSCE13";
            case NV_PFAULT_CLIENT_HUB_HSCE14:
                return "HUBCLIENT_HSCE14";
            case NV_PFAULT_CLIENT_HUB_HSCE15:
                return "HUBCLIENT_HSCE15";
            case NV_PFAULT_CLIENT_HUB_FE1:
                return "HUBCLIENT_FE1";
            case NV_PFAULT_CLIENT_HUB_FE2:
                return "HUBCLIENT_FE2";
            case NV_PFAULT_CLIENT_HUB_FE3:
                return "HUBCLIENT_FE3";
            case NV_PFAULT_CLIENT_HUB_FE4:
                return "HUBCLIENT_FE4";
            case NV_PFAULT_CLIENT_HUB_FE5:
                return "HUBCLIENT_FE5";
            case NV_PFAULT_CLIENT_HUB_FE6:
                return "HUBCLIENT_FE6";
            case NV_PFAULT_CLIENT_HUB_FE7:
                return "HUBCLIENT_FE7";
            case NV_PFAULT_CLIENT_HUB_FECS1:
                return "HUBCLIENT_FECS1";
            case NV_PFAULT_CLIENT_HUB_FECS2:
                return "HUBCLIENT_FECS2";
            case NV_PFAULT_CLIENT_HUB_FECS3:
                return "HUBCLIENT_FECS3";
            case NV_PFAULT_CLIENT_HUB_FECS4:
                return "HUBCLIENT_FECS4";
            case NV_PFAULT_CLIENT_HUB_FECS5:
                return "HUBCLIENT_FECS5";
            case NV_PFAULT_CLIENT_HUB_FECS6:
                return "HUBCLIENT_FECS6";
            case NV_PFAULT_CLIENT_HUB_FECS7:
                return "HUBCLIENT_FECS7";
            case NV_PFAULT_CLIENT_HUB_SKED1:
                return "HUBCLIENT_SKED1";
            case NV_PFAULT_CLIENT_HUB_SKED2:
                return "HUBCLIENT_SKED2";
            case NV_PFAULT_CLIENT_HUB_SKED3:
                return "HUBCLIENT_SKED3";
            case NV_PFAULT_CLIENT_HUB_SKED4:
                return "HUBCLIENT_SKED4";
            case NV_PFAULT_CLIENT_HUB_SKED5:
                return "HUBCLIENT_SKED5";
            case NV_PFAULT_CLIENT_HUB_SKED6:
                return "HUBCLIENT_SKED6";
            case NV_PFAULT_CLIENT_HUB_SKED7:
                return "HUBCLIENT_SKED7";
            case NV_PFAULT_CLIENT_HUB_ESC:
                return "HUBCLIENT_ESC";
            default:
                return kfifoGetClientIdStringCheck_HAL(pGpu, pKernelFifo, pMmuExceptInfo->clientId);
        }
    }
}


/**
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] clientId

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdStringCheck_GA100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       clientId
)
{
    switch (clientId)
    {
        case NV_PFAULT_CLIENT_HUB_NVENC0:
            return "HUBCLIENT_NVENC0";
        case NV_PFAULT_CLIENT_HUB_NVDEC4:
            return "HUBCLIENT_NVDEC4";
        default:
            return "UNRECOGNIZED_CLIENT";

    }
}

/**
 * @brief Starts halting a channel. A start operation must be matched with a
 * complete operation later to wait for the channel to be preempted.
 *
 * @param[in] pGpu           GPU object pointer
 * @param[in] pKernelFifo    Kernel FIFO object pointer
 * @param[in] pKernelChannel Pointer to the channel to be halted.
 */
void
kfifoStartChannelHalt_GA100
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel
)
{
    NvU32       chramPriBase;
    NvU32       channelVal;
    NvU32       runlistId;
    NvU32       runlistPriBase;
    NvU32       runlistVal = 0;

    runlistId = kchannelGetRunlistId(pKernelChannel);
    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
            ENGINE_INFO_TYPE_RUNLIST,        runlistId,
            ENGINE_INFO_TYPE_CHRAM_PRI_BASE, &chramPriBase) != NV_OK)
    {
        return;
    }
    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
            ENGINE_INFO_TYPE_RUNLIST,          runlistId,
            ENGINE_INFO_TYPE_RUNLIST_PRI_BASE, &runlistPriBase) != NV_OK)
    {
        return;
    }

    // Disable this channel.
    channelVal = FLD_SET_DRF(_CHRAM, _CHANNEL, _WRITE_CONTROL, _ONES_CLEAR_BITS, 0);
    channelVal = FLD_SET_DRF(_CHRAM, _CHANNEL, _ENABLE, _IN_USE, channelVal);
    GPU_REG_WR32(pGpu, chramPriBase + NV_CHRAM_CHANNEL(pKernelChannel->ChID), channelVal);

    // Preempt the channel.
    runlistVal = FLD_SET_DRF(_RUNLIST, _PREEMPT, _TYPE, _RUNLIST, 0);
    GPU_REG_WR32(pGpu, runlistPriBase + NV_RUNLIST_PREEMPT, runlistVal);
}

/**
 * @brief Completes halting a channel, waiting the channel preemption to
 * complete, up to the specified timeout.
 *
 * @param[in] pGpu           GPU object pointer
 * @param[in] pKernelFifo    Kernel FIFO object pointer
 * @param[in] pKernelChannel Pointer to the channel in process of being halted.
 * @param[in] pTimeout       Specifies the timeout to wait for the channel
 *                           preemption.
 */
void
kfifoCompleteChannelHalt_GA100
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel,
    RMTIMEOUT     *pTimeout
)
{
    NvU32       runlistId;
    NvU32       runlistPriBase;
    NvU32       runlistVal = 0;

    runlistId = kchannelGetRunlistId(pKernelChannel);
    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
            ENGINE_INFO_TYPE_RUNLIST,          runlistId,
            ENGINE_INFO_TYPE_RUNLIST_PRI_BASE, &runlistPriBase) != NV_OK)
    {
        return;
    }

    // Wait for the preemption to complete.
    do
    {
        if (gpuCheckTimeout(pGpu, pTimeout) == NV_ERR_TIMEOUT)
        {
            break;
        }

        runlistVal = GPU_REG_RD32(pGpu, runlistPriBase + NV_RUNLIST_PREEMPT);
    } while (FLD_TEST_DRF(_RUNLIST, _PREEMPT, _RUNLIST_PREEMPT_PENDING, _TRUE, runlistVal));
}

