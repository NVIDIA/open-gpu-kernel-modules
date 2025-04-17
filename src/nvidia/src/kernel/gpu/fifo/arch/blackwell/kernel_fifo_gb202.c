/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/gsplite/kernel_gsplite.h"
#include "utils/nvassert.h"
#include "gpu/gpu.h"

#include "vgpu/vgpu_events.h"

#include "published/blackwell/gb202/dev_fault.h"
#include "published/blackwell/gb202/dev_vm.h"
#include "published/blackwell/gb202/dev_ram.h"
#include "published/blackwell/gb202/dev_runlist.h"

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
kfifoGenerateWorkSubmitTokenHal_GB202
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32         *pGeneratedToken,
    NvBool         bUsedForHost
)
{
    NvU32          val = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    NV_ASSERT_OR_RETURN(pGeneratedToken != NULL, NV_ERR_INVALID_OBJECT);
    NV_ASSERT_OR_RETURN((pKernelChannel->pKernelChannelGroupApi != NULL) &&
                        (pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup != NULL),
                        NV_ERR_INVALID_STATE);

    NvU32          chId;
    NvU32          runlistId;
    runlistId = kchannelGetRunlistId(pKernelChannel);
    chId      = pKernelChannel->ChID;

    val = FLD_SET_DRF_NUM(_VIRTUAL, _FUNCTION_DOORBELL, _RUNLIST_ID,       runlistId, val);
    // For ESCHED notifications, we need to update ChID in Vector Field.
    val = FLD_SET_DRF_NUM(_VIRTUAL, _FUNCTION_DOORBELL, _VECTOR,           chId,      val);
    val = FLD_SET_DRF(_VIRTUAL, _FUNCTION_DOORBELL, _RUNLIST_DOORBELL, _ENABLE,   val);

    NV_PRINTF(LEVEL_INFO,
                "Generated workSubmitToken 0x%x for channel 0x%x runlist 0x%x\n",
                val, chId, runlistId);

    *pGeneratedToken = val;

    return NV_OK;
}

/*!
 * @brief Converts an mmu engine id (NV_PFAULT_MMU_ENG_ID_*) into a string.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] engineID NV_PFAULT_MMU_ENG_ID_*
 *
 * @returns a valid string if match found, NULL otherwise
 */
const char*
kfifoPrintInternalEngineCheck_GB202
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_GSPLITE:
            return "GSPLITE";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE1:
            return "GSPLITE1";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE2:
            return "GSPLITE2";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE3:
            return "GSPLITE3";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE4:
            return "GSPLITE4";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE5:
            return "GSPLITE5";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE6:
            return "GSPLITE6";
        case NV_PFAULT_MMU_ENG_ID_GSPLITE7:
            return "GSPLITE7";
        default:
            break;
    }

    return NULL;
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
kfifoGetClientIdString_GB202
(
    OBJGPU                  *pGpu,
    KernelFifo              *pKernelFifo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptInfo
)
{
    if (!pMmuExceptInfo->bGpc)
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_ESC0:
                return "HUBCLIENT_ESC0";
            case NV_PFAULT_CLIENT_HUB_ESC1:
                return "HUBCLIENT_ESC1";
            case NV_PFAULT_CLIENT_HUB_ESC2:
                return "HUBCLIENT_ESC2";
            case NV_PFAULT_CLIENT_HUB_ESC3:
                return "HUBCLIENT_ESC3";
            case NV_PFAULT_CLIENT_HUB_ESC4:
                return "HUBCLIENT_ESC4";
            case NV_PFAULT_CLIENT_HUB_ESC5:
                return "HUBCLIENT_ESC5";
            case NV_PFAULT_CLIENT_HUB_ESC6:
                return "HUBCLIENT_ESC6";
            case NV_PFAULT_CLIENT_HUB_ESC7:
                return "HUBCLIENT_ESC7";
            case NV_PFAULT_CLIENT_HUB_ESC8:
                return "HUBCLIENT_ESC8";
            case NV_PFAULT_CLIENT_HUB_ESC9:
                return "HUBCLIENT_ESC9";
            case NV_PFAULT_CLIENT_HUB_ESC10:
                return "HUBCLIENT_ESC10";
            case NV_PFAULT_CLIENT_HUB_ESC11:
                return "HUBCLIENT_ESC11";
            case NV_PFAULT_CLIENT_HUB_GSPLITE:
                return "HUBCLIENT_GSPLITE";
            case NV_PFAULT_CLIENT_HUB_GSPLITE1:
                return "HUBCLIENT_GSPLITE1";
            case NV_PFAULT_CLIENT_HUB_GSPLITE2:
                return "HUBCLIENT_GSPLITE2";
            case NV_PFAULT_CLIENT_HUB_GSPLITE3:
                return "HUBCLIENT_GSPLITE3";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER2:
                return "HUBCLIENT_VPR_SCRUBBER2";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER3:
                return "HUBCLIENT_VPR_SCRUBBER3";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER4:
                return "HUBCLIENT_VPR_SCRUBBER4";
            case NV_PFAULT_CLIENT_HUB_NVENC3:
                return "HUBCLIENT_NVENC3";
            case NV_PFAULT_CLIENT_HUB_FSP :
                return "HUBCLIENT_FSP";
            case NV_PFAULT_CLIENT_HUB_PD1:
                return "HUBCLIENT_PD1";
            case NV_PFAULT_CLIENT_HUB_PD2:
                return "HUBCLIENT_PD2";
            case NV_PFAULT_CLIENT_HUB_PD3:
                return "HUBCLIENT_PD3";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD1:
                return "HUBCLIENT_RASTERTWOD1";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD2:
                return "HUBCLIENT_RASTERTWOD2";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD3:
                return "HUBCLIENT_RASTERTWOD3";
            case NV_PFAULT_CLIENT_HUB_SCC1:
                return "HUBCLIENT_SCC1";
            case NV_PFAULT_CLIENT_HUB_SCC_NB1:
                return "HUBCLIENT_SCC_NB1";
            case NV_PFAULT_CLIENT_HUB_SCC2:
                return "HUBCLIENT_SCC2";
            case NV_PFAULT_CLIENT_HUB_SCC_NB2:
                return "HUBCLIENT_SCC_NB2";
            case NV_PFAULT_CLIENT_HUB_SCC3:
                return "HUBCLIENT_SCC3";
            case NV_PFAULT_CLIENT_HUB_SCC_NB3:
                return "HUBCLIENT_SCC_NB3";
            case NV_PFAULT_CLIENT_HUB_SSYNC1:
                return "HUBCLIENT_SSYNC1";
            case NV_PFAULT_CLIENT_HUB_SSYNC2:
                return "HUBCLIENT_SSYNC2";
            case NV_PFAULT_CLIENT_HUB_SSYNC3:
                return "HUBCLIENT_SSYNC3";
        }
    }

    // Fallback if the above doesn't cover the given client ID
    return kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, pMmuExceptInfo);
}

/**
 * @brief Verifies an SCG and runqueue combination
 *
 * @param pKernelFifo
 * @param scgType
 * @param runqueue
 *
 * @returns a boolean
 */
NvBool
kfifoValidateSCGTypeAndRunqueue_GB202
(
    KernelFifo *pKernelFifo,
    NvU32       scgType,
    NvU32       runqueue
)
{
    if (scgType == NV_RAMRL_ENTRY_CHAN_RUNQUEUE_SELECTOR_RUNQUEUE1 &&
        runqueue == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Invalid (SCG,runqueue) combination: (0x%x,0x%x)\n",
                  scgType, runqueue);
        return NV_FALSE;
    }

    return NV_TRUE;
}

/**
 * @brief Get the runlist base shift amount
 *
 * @param pKernelFifo
 *
 * @return shift amount
 */
NvU32
kfifoRunlistGetBaseShift_GB202
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_BASE_SHIFT;
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
kfifoStartChannelHalt_GB202
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
kfifoCompleteChannelHalt_GB202
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

