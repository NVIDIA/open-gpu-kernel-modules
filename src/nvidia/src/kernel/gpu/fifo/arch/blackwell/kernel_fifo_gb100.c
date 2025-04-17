/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "utils/nvassert.h"
#include "gpu/gpu.h"

#include "published/blackwell/gb100/dev_fault.h"
#include "published/blackwell/gb100/dev_vm.h"
#include "published/blackwell/gb100/hwproject.h"

#include "vgpu/vgpu_events.h"

/*
 * @brief Reserve PBDMA fault IDs to support subcontexts
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pEngineInfoList
 * @param[in] engineInfoListSize
 *
 * @returns NV_OK if success
 */
NV_STATUS
kfifoReservePbdmaFaultIds_GB100
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    FIFO_ENGINE_LIST *pEngineInfoList,
    NvU32             engineInfoListSize
)
{
    ENGINE_INFO *pEngineInfo = &pKernelFifo->engineInfo;
    NvU32        deviceIndex;
    NvU32        baseGrPbdmaId;
    NvU32        maxVeidCount = NV_LITTER_NUM_SUBCTX;

    for (deviceIndex = 0; deviceIndex < engineInfoListSize; deviceIndex++)
    {
        if (IS_GR(pEngineInfoList[deviceIndex].engineData[ENGINE_INFO_TYPE_ENG_DESC]))
        {
            // Both entries at pbdmaFaultIdsBase[] contains the same PBDMA faultId, so taking 0th entry
            baseGrPbdmaId = pEngineInfoList[deviceIndex].pbdmaFaultIds[0];

            for (NvU32 count = 0; count < maxVeidCount; count++)
            {
                bitVectorSet(&pEngineInfo->validEngineIdsForPbdmas, baseGrPbdmaId + count);
            }
            break;
        }
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
kfifoGenerateWorkSubmitTokenHal_GB100
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

    // Here we construct token to be a concatenation of runlist id and channel id
    val = FLD_SET_DRF_NUM(_VIRTUAL, _FUNCTION_DOORBELL, _RUNLIST_ID, runlistId, val);
    val = FLD_SET_DRF_NUM(_VIRTUAL, _FUNCTION_DOORBELL, _VECTOR,     chId,      val);

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
 * @returns a string (always non-null)
 */
const char*
kfifoPrintInternalEngine_GB100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    if (kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, engineID))
    {
        return kfifoPrintFaultingPbdmaEngineName(pGpu, pKernelFifo, engineID);
    }

    switch (engineID)
    {
        case NV_PFAULT_MMU_ENG_ID_GSPLITE:
            return "GSPLITE";
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

    // fallback to _GH100 HAL for non-pbdma faults
    return kfifoPrintInternalEngine_GH100(pGpu, pKernelFifo, engineID);
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
kfifoGetClientIdString_GB100
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
        }
    }

    // Fallback if the above doesn't cover the given client ID
    return kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, pMmuExceptInfo);
}
