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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel.h"

#include "gpu/gpu_access.h"
#include "gpu/gpu.h"
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_fault.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_ctrl.h"
#include "published/turing/tu102/hwproject.h"

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
kfifoUpdateUsermodeDoorbell_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       workSubmitToken,
    NvU32       runlistId
)
{
    NV_PRINTF(LEVEL_INFO, "Poking workSubmitToken 0x%x\n", workSubmitToken);

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_DOORBELL, workSubmitToken);

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
kfifoGenerateWorkSubmitToken_TU102
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32         *pGeneratedToken,
    NvBool         bUsedForHost
)
{
    NvU32          chId;
    NvU32          gfid;
    NvU32          val = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    NV_ASSERT_OR_RETURN(pGeneratedToken != NULL, NV_ERR_INVALID_ARGUMENT);

    chId = pKernelChannel->ChID;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    //
    // In case of vGPU with SR-IOV, host RM is currently generating token using
    // virtual chid that was allocated inside the guest. This needs to change
    // once the guest starts managing its own channels. The guest would then
    // generate its own tokens.
    //
    if (!bUsedForHost && IS_GFID_VF(gfid))
    {
        NvU32 vChId;

        NV_ASSERT_OK_OR_RETURN(kfifoGetVChIdForSChId_HAL(pGpu, pKernelFifo,
                                                         chId, gfid,
                                                         kchannelGetEngineType(pKernelChannel),
                                                         &vChId));
        chId = vChId;
    }

    if (!kchannelIsRunlistSet(pGpu, pKernelChannel))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FAILED Channel 0x%x is not assigned to runlist yet\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_ERR_INVALID_STATE;
    }

    // Here we construct token to be a concatenation of runlist id and channel id
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _RUNLIST_ID, kchannelGetRunlistId(pKernelChannel), val);
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _VECTOR, chId, val);
    *pGeneratedToken = val;

    NV_PRINTF(LEVEL_INFO,
              "Generated workSubmitToken 0x%x for channel 0x%x runlist 0x%x\n",
              *pGeneratedToken, chId, kchannelGetRunlistId(pKernelChannel));

    return NV_OK;
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
kfifoPrintPbdmaId_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       pbdmaId
)
{
    NV_ASSERT_OR_RETURN(pbdmaId < NV_HOST_NUM_PBDMA, "UNKNOWN");
    static const char* pbdmaIdString[NV_HOST_NUM_PBDMA] = { "HOST0",
                                                            "HOST1",
                                                            "HOST2",
                                                            "HOST3",
                                                            "HOST4",
                                                            "HOST5",
                                                            "HOST6",
                                                            "HOST7",
                                                            "HOST8",
                                                            "HOST9",
                                                            "HOST10",
                                                            "HOST11",
#if NV_HOST_NUM_PBDMA > 12
#error Uninitialized elements of static array.
#endif
                                                          };

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
kfifoPrintInternalEngine_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engineID
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32 pbdmaId;

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
    else
    {
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
            case NV_PFAULT_MMU_ENG_ID_PWR_PMU:
                return "PMU";
            case NV_PFAULT_MMU_ENG_ID_PTP:
                return "PTP";
            case NV_PFAULT_MMU_ENG_ID_NVENC0:
                return "NVENC0";
            case NV_PFAULT_MMU_ENG_ID_NVENC1:
                return "NVENC1";
            case NV_PFAULT_MMU_ENG_ID_PHYSICAL:
                return "PHYSICAL";
            case NV_PFAULT_MMU_ENG_ID_NVJPG0:
                return "NVJPG";
            default:
            {
                NV_STATUS status = NV_OK;
                NvU32     engTag;

                status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_MMU_FAULT_ID,
                    engineID, ENGINE_INFO_TYPE_ENG_DESC, &engTag);
                if ((status == NV_OK) && (IS_GR(engTag)))
                {
                    return "GRAPHICS";
                }

            }
        }
    }

    return "UNKNOWN";
}

/*!
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdString_TU102
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
            case NV_PFAULT_CLIENT_GPC_PE_0:
                return "GPCCLIENT_PE_0";
            case NV_PFAULT_CLIENT_GPC_T1_1:
                return "GPCCLIENT_T1_1";
            case NV_PFAULT_CLIENT_GPC_PE_1:
                return "GPCCLIENT_PE_1";
            case NV_PFAULT_CLIENT_GPC_T1_2:
                return "GPCCLIENT_T1_2";
            case NV_PFAULT_CLIENT_GPC_PE_2:
                return "GPCCLIENT_PE_2";
            case NV_PFAULT_CLIENT_GPC_T1_3:
                return "GPCCLIENT_T1_3";
            case NV_PFAULT_CLIENT_GPC_PE_3:
                return "GPCCLIENT_PE_3";
            case NV_PFAULT_CLIENT_GPC_T1_4:
                return "GPCCLIENT_T1_4";
            case NV_PFAULT_CLIENT_GPC_PE_4:
                return "GPCCLIENT_PE_4";
            case NV_PFAULT_CLIENT_GPC_T1_5:
                return "GPCCLIENT_T1_5";
            case NV_PFAULT_CLIENT_GPC_PE_5:
                return "GPCCLIENT_PE_5";
            case NV_PFAULT_CLIENT_GPC_T1_6:
                return "GPCCLIENT_T1_6";
            case NV_PFAULT_CLIENT_GPC_PE_6:
                return "GPCCLIENT_PE_6";
            case NV_PFAULT_CLIENT_GPC_T1_7:
                return "GPCCLIENT_T1_7";
            case NV_PFAULT_CLIENT_GPC_PE_7:
                return "GPCCLIENT_PE_7";
            case NV_PFAULT_CLIENT_GPC_T1_8:
                return "GPCCLIENT_T1_8";
            case NV_PFAULT_CLIENT_GPC_PE_8:
                return "GPCCLIENT_PE_8";
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
            case NV_PFAULT_CLIENT_GPC_TPCCS_8:
                return "GPCCLIENT_TPCCS_8";
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
            default:
                return "UNRECOGNIZED_CLIENT";
        }
    }
    else
    {
        switch (pMmuExceptInfo->clientId)
        {
            case NV_PFAULT_CLIENT_HUB_CE0:
                return "HUBCLIENT_CE0";
            case NV_PFAULT_CLIENT_HUB_HSCE0:
                return "HUBCLIENT_HSCE0";
            case NV_PFAULT_CLIENT_HUB_CE1:
                return "HUBCLIENT_CE1";
            case NV_PFAULT_CLIENT_HUB_HSCE1:
                return "HUBCLIENT_HSCE1";
            case NV_PFAULT_CLIENT_HUB_CE2:
                return "HUBCLIENT_CE2";
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
            case NV_PFAULT_CLIENT_HUB_NVDEC1:
                return "HUBCLIENT_NVDEC1";
            case NV_PFAULT_CLIENT_HUB_NVDEC2:
                return "HUBCLIENT_NVDEC2";
            case NV_PFAULT_CLIENT_HUB_NVENC0:
                return "HUBCLIENT_NVENC0";
            case NV_PFAULT_CLIENT_HUB_NVENC1:
                return "HUBCLIENT_NVENC1";
            case NV_PFAULT_CLIENT_HUB_NISO:
                return "HUBCLIENT_NISO";
            case NV_PFAULT_CLIENT_HUB_P2P:
                return "HUBCLIENT_P2P";
            case NV_PFAULT_CLIENT_HUB_PD:
                return "HUBCLIENT_PD";
            case NV_PFAULT_CLIENT_HUB_PERF:
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
                return "HUBCLIENT_SEC2";
            case NV_PFAULT_CLIENT_HUB_SSYNC:
                return "HUBCLIENT_SSYNC";
            case NV_PFAULT_CLIENT_HUB_VIP:
                return "HUBCLIENT_VIP";
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
            case NV_PFAULT_CLIENT_HUB_DWBIF:
                return "HUBCLIENT_DWBIF";
            case NV_PFAULT_CLIENT_HUB_GSP:
                return "HUBCLIENT_GSP";
            case NV_PFAULT_CLIENT_HUB_FBFALCON:
                return "HUBCLIENT_FBFLCN";
            case NV_PFAULT_CLIENT_HUB_NVJPG0:
                return "HUBCLIENT_NVJPG0";
            default:
                return "UNRECOGNIZED_CLIENT";
        }
    }
}
