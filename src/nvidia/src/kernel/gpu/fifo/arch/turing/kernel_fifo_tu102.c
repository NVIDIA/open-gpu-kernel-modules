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

#include "kernel/gpu/bus/kern_bus.h"
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
 */
NV_STATUS
kfifoUpdateUsermodeDoorbell_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       workSubmitToken
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
kfifoGenerateWorkSubmitTokenHal_TU102
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
            "FAILED " FMT_CHANNEL_DEBUG_TAG " is not assigned to runlist yet\n",
            kchannelGetDebugTag(pKernelChannel));
        return NV_ERR_INVALID_STATE;
    }

    // Here we construct token to be a concatenation of runlist id and channel id
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _RUNLIST_ID, kchannelGetRunlistId(pKernelChannel), val);
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _VECTOR, chId, val);
    *pGeneratedToken = val;

    NV_PRINTF(LEVEL_INFO,
        "Generated workSubmitToken 0x%x for " FMT_CHANNEL_DEBUG_TAG " runlist 0x%x\n",
        *pGeneratedToken,
        kchannelGetDebugTag(pKernelChannel),
        kchannelGetRunlistId(pKernelChannel));

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

/**
 * @brief Pre-allocate BAR1 userd space
 *
 * @param   pGpu
 * @param   pKernelFifo
 *
 * @returns NV_STATUS
 */
NV_STATUS
kfifoPreAllocUserD_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    OBJGPU     *pParentGpu             = gpumgrGetParentGPU(pGpu);
    KernelFifo *pParentKernelFifo      = GPU_GET_KERNEL_FIFO(pParentGpu);
    KernelBus  *pKernelBus             = GPU_GET_KERNEL_BUS(pGpu);
    NvBool      bCoherentCpuMapping    = NV_FALSE;
    NV_STATUS   status                 = NV_OK;
    NvU64       temp                   = 0;
    NvU32       userdSize;
    NvU32       userdShift;
    NvU32       numChannels;
    NvBool      bFifoFirstInit;
    NvU32       flags                  = MEMDESC_FLAGS_NONE;
    NvU32       mapFlags               = BUS_MAP_FB_FLAGS_MAP_DOWNWARDS |
                                         BUS_MAP_FB_FLAGS_MAP_UNICAST;
    NvU32       currentGpuInst         = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    CHID_MGR   *pChidMgr               = kfifoGetChidMgr(pGpu, pKernelFifo, 0);

    MemoryManager     *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager  *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    PREALLOCATED_USERD_INFO *pUserdInfo  = &pParentKernelFifo->userdInfo;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // We don't support RM allocated USERD for vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        return NV_OK;
    }

    bCoherentCpuMapping = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING);

    if (pUserdInfo->userdBar1CpuPtr == NULL)
    {
        bFifoFirstInit = NV_TRUE;
    }
    else
    {
        mapFlags |= BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED;
        bFifoFirstInit = NV_FALSE;
    }

    //
    // Allocate the physical memory associated with the UserD if this is
    // the first GPU to init fifo. This relies on the assumption that
    // UserD is shared physmem.
    //
    if (bFifoFirstInit)
    {
        pUserdInfo->userdBar1MapStartOffset   =  0;
        pUserdInfo->userdBar1MapSize          =  0;

        // This is a WAR for HW bug 600241
        if (pUserdInfo->userdAperture == ADDR_SYSMEM)
        {
            pKernelFifo->bUserdInSystemMemory = NV_TRUE;
        }
    }

    kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, &userdShift);

    numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr);

    // Alloc USERD of size numChannels * sizeof( USERD ) for each gpu
    status = memdescCreate(&pUserdInfo->userdPhysDesc[currentGpuInst], pGpu,
                           userdSize * numChannels,
                           1ULL << userdShift,
                           NV_TRUE,
                           pUserdInfo->userdAperture,
                           pUserdInfo->userdAttr,
                           flags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not memdescCreate for USERD for %x #channels\n",
                  numChannels);
        DBG_BREAKPOINT();
        goto fail;
    }
    temp = pUserdInfo->userdPhysDesc[currentGpuInst]->Size;

    //
    // For vGPU, do not allocate USERD memory in guest.
    // vGPU does all HW management in host, so host RM will
    // allocate the real USERD memory.
    //
    if (IS_VIRTUAL(pGpu))
    {
        // Force page size to 4KB to match host phys access
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager,
                                     pUserdInfo->userdPhysDesc[currentGpuInst],
                                     AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
        mapFlags |= BUS_MAP_FB_FLAGS_PAGE_SIZE_4K;
        if (bFifoFirstInit)
        {
            pUserdInfo->userdBar1MapStartOffset = kfifoGetUserdBar1MapStartOffset_HAL(pGpu, pKernelFifo);
        }
    }
    else
    {
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_81,
                        pUserdInfo->userdPhysDesc[currentGpuInst]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not allocate USERD for %x #channels\n",
                      numChannels);
            DBG_BREAKPOINT();
            goto fail;
        }

        // Force page size to 4KB in broadcast to match host phys access
        memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pUserdInfo->userdPhysDesc[currentGpuInst],
                                     AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
        mapFlags |= BUS_MAP_FB_FLAGS_PAGE_SIZE_4K;

        //
        // If coherent link is available, just get a coherent mapping to USERD and
        // lie about the BAR1 offset, since we are not using BAR1
        // TODO: Make these bar1 offsets unicast on each gpu as well
        //
        if (bCoherentCpuMapping &&
            (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM))
        {

            NV_PRINTF(LEVEL_INFO, "Mapping USERD with coherent link (USERD in FBMEM).\n");
            NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
            NV_ASSERT(pUserdInfo->userdPhysDesc[currentGpuInst]->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS);

            if (bFifoFirstInit)
            {
                pUserdInfo->userdBar1MapStartOffset =  pUserdInfo->userdPhysDesc[currentGpuInst]->_pteArray[0] +
                                                       pUserdInfo->userdPhysDesc[currentGpuInst]->PteAdjust;
            }
        }
        //
        // get sysmem mapping for USERD if USERD is in sysmem and reflected BAR access is not allowed
        //
        else if ((bCoherentCpuMapping &&
                 memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_SYSMEM &&
                 !kbusIsReflectedMappingAccessAllowed(pKernelBus)) ||
                 kbusIsBar1Disabled(pKernelBus))
        {
            NV_PRINTF(LEVEL_INFO, "Mapping USERD with coherent link (USERD in SYSMEM).\n");

            if (bFifoFirstInit)
            {
                pUserdInfo->userdBar1MapStartOffset =
                        memdescGetPhysAddr(pUserdInfo->userdPhysDesc[currentGpuInst], AT_CPU, 0);
            }
        }
        else
        {
            // vGpu may boot with partitioning enabled but that's not true for host RM
            if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
            {
                status = NV_ERR_INVALID_STATE;
                NV_PRINTF(LEVEL_ERROR, "Pre-allocated USERD is not supported with MIG\n");
                DBG_BREAKPOINT();
                goto fail;
            }
            // Now BAR1 map it
            status = kbusMapFbApertureSingle(pGpu, pKernelBus, pUserdInfo->userdPhysDesc[currentGpuInst], 0,
                                             &pUserdInfo->userdBar1MapStartOffset,
                                             &temp, mapFlags | BUS_MAP_FB_FLAGS_PRE_INIT, NULL);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Could not map USERD to BAR1\n");
                DBG_BREAKPOINT();
                goto fail;
            }

            // Add current GPU to list of GPUs referencing pFifo userD bar1
            pUserdInfo->userdBar1RefMask |= NVBIT(pGpu->gpuInstance);
        }
    }

    if (bFifoFirstInit)
    {
        pUserdInfo->userdBar1MapSize = NvU64_LO32(temp);

        if (bCoherentCpuMapping &&
            (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM))
        {
            status = kbusMapCoherentCpuMapping_HAL(pGpu, pKernelBus,
                                                   pUserdInfo->userdPhysDesc[currentGpuInst],
                                                   0,
                                                   pUserdInfo->userdBar1MapSize,
                                                   NV_PROTECT_READ_WRITE,
                                                   (void**)&pUserdInfo->userdBar1CpuPtr,
                                                   (void**)&pUserdInfo->userdBar1Priv);
        }
        else if ((bCoherentCpuMapping &&
                 memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_SYSMEM &&
                 !kbusIsReflectedMappingAccessAllowed(pKernelBus)) &&
                 !kbusIsBar1Disabled(pKernelBus))
        {
            status = osMapPciMemoryKernelOld(pGpu,
                                             pUserdInfo->userdBar1MapStartOffset,
                                             pUserdInfo->userdBar1MapSize,
                                             NV_PROTECT_READ_WRITE,
                                             (void**)&pUserdInfo->userdBar1CpuPtr,
                                             NV_MEMORY_CACHED);
        }
        else if (kbusIsBar1Disabled(pKernelBus))
        {
            status = memdescMap(pUserdInfo->userdPhysDesc[currentGpuInst],
                                0,
                                pUserdInfo->userdBar1MapSize,
                                NV_TRUE,
                                NV_PROTECT_READ_WRITE,
                                (void**)&pUserdInfo->userdBar1CpuPtr,
                                (void**)&pUserdInfo->userdBar1Priv);
        }
        else
        {
            // Cpu map the BAR1 snoop range
            status = osMapPciMemoryKernelOld(pGpu, gpumgrGetGpuPhysFbAddr(pGpu) +
                                             pUserdInfo->userdBar1MapStartOffset,
                                             pUserdInfo->userdBar1MapSize,
                                             NV_PROTECT_READ_WRITE,
                                             (void**)&pUserdInfo->userdBar1CpuPtr,
                                             NV_MEMORY_UNCACHED);
        }

        if ((pUserdInfo->userdBar1CpuPtr == NULL) && (status != NV_OK))
        {
            NV_PRINTF(LEVEL_ERROR, "Could not cpu map BAR1 snoop range\n");
            DBG_BREAKPOINT();
            goto fail;
        }
    }

    NV_PRINTF(LEVEL_INFO,
              "USERD Preallocated phys @ 0x%llx bar1 offset @ 0x%llx of size 0x%x\n",
              memdescGetPhysAddr(pUserdInfo->userdPhysDesc[currentGpuInst], AT_GPU, 0),
              pUserdInfo->userdBar1MapStartOffset,
              pUserdInfo->userdBar1MapSize);

    return status;

fail:
    kfifoFreePreAllocUserD_HAL(pGpu, pKernelFifo);

    return status;
}

/**
 * @brief Free the pre-allocated BAR1 userd space
 *
 * @param   pGpu
 * @param   pKernelFifo
 *
 * @returns NV_STATUS
 */
void
kfifoFreePreAllocUserD_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    OBJGPU            *pParentGpu           = gpumgrGetParentGPU(pGpu);
    KernelBus         *pKernelBus           = GPU_GET_KERNEL_BUS(pGpu);
    NvU32              currentGpuInst       = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    KernelFifo        *pParentKernelFifo    = GPU_GET_KERNEL_FIFO(pParentGpu);
    PREALLOCATED_USERD_INFO *pUserdInfo     = &pParentKernelFifo->userdInfo;
    NvBool             bCoherentCpuMapping  = pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        (memdescGetAddressSpace(pUserdInfo->userdPhysDesc[currentGpuInst]) == ADDR_FBMEM);

    // We don't support RM allocated USERD for vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        return;
    }

    if (gpumgrGetBcEnabledStatus(pGpu))
    {
        DBG_BREAKPOINT();
    }

    if (bCoherentCpuMapping)
    {
        NV_PRINTF(LEVEL_INFO, "Unmapping USERD from NVLINK.\n");
        NV_ASSERT(pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED));
    }

    if (pUserdInfo->userdBar1CpuPtr)
    {
        if (bCoherentCpuMapping)
        {
            kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus,
                pUserdInfo->userdPhysDesc[currentGpuInst],
                pUserdInfo->userdBar1CpuPtr,
                pUserdInfo->userdBar1Priv);
        }
        else if (kbusIsBar1Disabled(pKernelBus))
        {
            memdescUnmap(pUserdInfo->userdPhysDesc[currentGpuInst],
                            NV_TRUE,
                            (void*)pUserdInfo->userdBar1CpuPtr,
                            (void*)pUserdInfo->userdBar1Priv);
        }
        else
        {
            osUnmapPciMemoryKernelOld(pGpu, pUserdInfo->userdBar1CpuPtr);
        }

        pUserdInfo->userdBar1CpuPtr = NULL;
    }

    if (pUserdInfo->userdBar1MapSize)
    {
        if ((!IS_VIRTUAL(pGpu)) && (!bCoherentCpuMapping))
        {
            if ((pUserdInfo->userdBar1RefMask & NVBIT(pGpu->gpuInstance)) != 0)
            {
                //
                // Unmap in UC for each GPU with a pKernelFifo userd
                // reference mapped through bar1
                //
                kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                          pUserdInfo->userdPhysDesc[currentGpuInst],
                                          pUserdInfo->userdBar1MapStartOffset,
                                          pUserdInfo->userdBar1MapSize,
                                          BUS_MAP_FB_FLAGS_MAP_UNICAST | BUS_MAP_FB_FLAGS_PRE_INIT);
                pUserdInfo->userdBar1RefMask &= (~NVBIT(pGpu->gpuInstance));
            }

        }
    }

    // Unallocated memdescFrees are allowed.
    memdescFree(pUserdInfo->userdPhysDesc[currentGpuInst]);
    memdescDestroy(pUserdInfo->userdPhysDesc[currentGpuInst]);
    pUserdInfo->userdPhysDesc[currentGpuInst] = NULL;
    NV_PRINTF(LEVEL_INFO, "Freeing preallocated USERD phys and bar1 range\n");
}

//
// Returns the BAR1 offset and size of the entire USERD mapping.
//
NV_STATUS
kfifoGetUserdBar1MapInfo_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU64      *pBar1MapOffset,
    NvU32      *pBar1MapSize
)
{
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    // We don't support RM allocated USERD in vGPU guest with SRIOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        *pBar1MapOffset = 0;
        *pBar1MapSize   = 0;

        return NV_OK;
    }

    if (pUserdInfo->userdBar1MapSize == 0 )
    {
        NV_PRINTF(LEVEL_ERROR, "BAR1 map of USERD has not been setup yet\n");
        NV_ASSERT( 0 );
        return NV_ERR_GENERIC;
    }

    *pBar1MapOffset = pUserdInfo->userdBar1MapStartOffset;
    *pBar1MapSize   = pUserdInfo->userdBar1MapSize;

    return NV_OK;
}

/**
 * @brief Determines the aperture and attribute of memory where userd is located.
 *
 * @param pKernelFifo[in]
 * @param pUserdAperture[out]
 * @param pUserdAttribute[out]
 *
 * @returns NV_STATUS
 */
NV_STATUS
kfifoGetUserdLocation_TU102
(
    KernelFifo *pKernelFifo,
    NvU32 *pUserdAperture,
    NvU32 *pUserdAttribute
)
{
    const PREALLOCATED_USERD_INFO *pUserdInfo = kfifoGetPreallocatedUserdInfo(pKernelFifo);

    NV_ASSERT_OR_RETURN(pUserdAperture != NULL && pUserdAttribute != NULL,
                        NV_ERR_INVALID_POINTER);

    *pUserdAperture = pUserdInfo->userdAperture;
    *pUserdAttribute = pUserdInfo->userdAttr;

    return NV_OK;
}
