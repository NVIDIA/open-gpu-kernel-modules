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
#include "kernel/gpu/fifo/kernel_ctxshare.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/rmapi/rmapi.h"
#include "kernel/gpu/gpu.h"
#include "kernel/mem_mgr/mem.h"
#include "containers/eheap_old.h"

#include "nvrm_registry.h"

#include "published/volta/gv100/dev_fault.h"
#include "published/volta/gv100/dev_ram.h"

#include "ctrl/ctrlc36f.h"
#include "class/clc361.h"   // VOLTA_USERMODE_A

/**
 * Returns the default timeslice (in us) for a channelgroup as defined by hardware.
 */
NvU64
kfifoChannelGroupGetDefaultTimeslice_GV100
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_TSG_TIMESLICE_TIMEOUT_128 << NV_RAMRL_ENTRY_TSG_TIMESLICE_SCALE_3;
}

/**
* @brief Get the work submit token to be used to ring the doorbell
*
* @param[in]  pKernelFifo
* @param[in]  hClient
* @param[in]  hChannel
* @param[out] pWorkSubmitToken: Pointer to where the
*             updated token should be stored.
* TODO: Remove this call and let caller directly invoke the
* token generation HAL.
*/
NV_STATUS
kfifoRmctrlGetWorkSubmitToken_GV100
(
    KernelFifo *pKernelFifo,
    NvHandle hClient,
    NvHandle hChannel,
    NvU32 *pWorkSubmitToken
)
{
    NV_STATUS rmStatus;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS params;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    portMemSet(&params, 0, sizeof(params));

    if (pWorkSubmitToken == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "FAILED to get work submit token.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    rmStatus = pRmApi->Control(pRmApi, hClient, hChannel,
                               NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN,
                               &params, sizeof(params));
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "Unable to get work submit token.\n");
        return rmStatus;
    }
    *pWorkSubmitToken = params.workSubmitToken;
    return NV_OK;
}

NV_STATUS
kfifoEngineInfoXlate_GV100
(
    OBJGPU          *pGpu,
    KernelFifo      *pKernelFifo,
    ENGINE_INFO_TYPE inType,
    NvU32            inVal,
    ENGINE_INFO_TYPE outType,
    NvU32           *pOutVal
)
{
    extern NV_STATUS kfifoEngineInfoXlate_GM107(OBJGPU *pGpu, KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal, ENGINE_INFO_TYPE outType, NvU32 *pOutVal);

    // GR supports a range of faults ids
    if (inType == ENGINE_INFO_TYPE_MMU_FAULT_ID)
    {
        NvU32 grFaultId;
        // RM-SMC AMPERE-TODO this translation must be extended to work with SMC
        NvU32 maxSubctx = kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, NV_FALSE);

        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_GM107(pGpu, pKernelFifo,
                                                          ENGINE_INFO_TYPE_ENG_DESC, ENG_GR(0),
                                                          ENGINE_INFO_TYPE_MMU_FAULT_ID, &grFaultId));

        if ((inVal >= grFaultId) && (inVal < (grFaultId + maxSubctx)))
        {
            inVal = grFaultId;
        }
    }

    return kfifoEngineInfoXlate_GM107(pGpu, pKernelFifo, inType, inVal, outType, pOutVal);
}

// compile time check to ensure RM ctrl call does not support more than what the HW can.
ct_assert(NV2080_CTRL_CMD_FIFO_MAX_CHANNELS_PER_TSG == NV_RAMRL_ENTRY_TSG_LENGTH_MAX);

/*
 * @brief Gives the maxinum number of channels allowed per channel group
 */
NvU32
kfifoGetMaxChannelGroupSize_GV100
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_TSG_LENGTH_MAX;
}

/**
 * @brief Get usermode register offset and size
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[out] offset
 * @param[out] size
 */
NV_STATUS
kfifoGetUsermodeMapInfo_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU64      *pOffset,
    NvU32      *pSize
)
{
    NvU32 offset;

    NV_ASSERT_OK_OR_RETURN(gpuGetRegBaseOffset_HAL(pGpu, NV_REG_BASE_USERMODE, &offset));

    if (pOffset)
        *pOffset = offset;

    if (pSize)
        *pSize = DRF_SIZE(NVC361);

    return NV_OK;
}
/**
 * @brief Get the maximum number of subcontext within a TSG.
 *
 * @param  pKernelFifo
 */
NvU32
kfifoGetMaxSubcontext_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvBool      bLegacyMode
)
{
    extern NvU32 kfifoGetMaxSubcontext_GM200(OBJGPU *pGpu, KernelFifo *pKernelFifo, NvBool bLegacyMode);

    if (bLegacyMode ||
        !kfifoIsSubcontextSupported(pKernelFifo) ||
        !pGpu ||            // there is no GPU for some strange reason
        IsDFPGA(pGpu))      // dFPGA doesn't have GR
    {
        return 2;
    }

    if (pKernelFifo->maxSubcontextCount == 0)
    {
        NvU32 maxVeid = kfifoGetMaxSubcontextFromGr_HAL(pGpu, pKernelFifo);

        // Verify that subcontext mask array is properly sized
        NV_ASSERT_OR_RETURN(maxVeid / 32 <= SUBCTX_MASK_ARRAY_SIZE, 0);
        pKernelFifo->maxSubcontextCount = maxVeid;
    }

    return pKernelFifo->maxSubcontextCount;
}

/**
 * @brief Get the subcontext type info for the channel
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[in]  pKernelChannel
 * @param[out] pSubctxType
 */

void
kfifoGetSubctxType_GV100
(
    OBJGPU           *pGpu,
    KernelFifo       *pKernelFifo,
    KernelChannel    *pKernelChannel,
    NvU32            *pSubctxType
)
{
    NvU32 subctxType;
    KernelCtxShare *pKernelCtxShare = NULL;

    NV_ASSERT_OR_RETURN_VOID(pKernelChannel != NULL);

    // In case of lite channel mode there is no subcontext associated with a TSG. Return SYNC in such cases
    if (kfifoIsLiteModeEnabled_HAL(pGpu, pKernelFifo))
    {
        pSubctxType = NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC;
        return;
    }

    NV_ASSERT(pKernelChannel->subctxId != FIFO_PDB_IDX_BASE);

    KernelChannelGroup *pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
    EMEMBLOCK *pBlock = pKernelChannelGroup->pSubctxIdHeap->eheapGetBlock(
        pKernelChannelGroup->pSubctxIdHeap,
        pKernelChannel->subctxId,
        NV_FALSE);

    if (pBlock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "subcontext not allocated for this TSG\n");
        NV_ASSERT(pBlock);
        return;
    }

    pKernelCtxShare = (KernelCtxShare *)pBlock->pData;
    subctxType = DRF_VAL(_CTXSHARE, _ALLOCATION_FLAGS, _SUBCONTEXT, pKernelCtxShare->flags);
    if (subctxType >= NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SPECIFIED)
    {
        subctxType = NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC;
    }

    if (pSubctxType)
    {
        *pSubctxType = subctxType;
    }
}

/**
 * @brief Get the runlist entry size
 *
 * @param pKernelFifo
 *
 * @return size in bytes
 */
NvU32
kfifoRunlistGetEntrySize_GV100
(
    KernelFifo *pKernelFifo
)
{
    return NV_RAMRL_ENTRY_SIZE;
}
/*!
 * @brief Calculates the size of all fault method buffers
 *
 * @param[in] pGpu               OBJGPU  pointer
 * @param[in] pKernelFifo        KernelFifo pointer
 * @param[in] bCalcForFbRsvd     NV_TRUE if this calculation is requested by FB calc
 *
 * @return Size in bytes
 */
NvU32
kfifoCalcTotalSizeOfFaultMethodBuffers_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvBool      bCalcForFbRsvd
)
{
    NvU32   maxChannelGroups = 0;
    NvU32   runQueues        = 0;
    NvU32   totalSize        = 0;

    // Return 0 from guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return 0;
    }

    maxChannelGroups = kfifoGetMaxCeChannelGroups_HAL(pGpu, pKernelFifo);
    runQueues        = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);

    NV_ASSERT_OK(gpuGetCeFaultMethodBufferSize(pGpu, &totalSize));

    totalSize *= maxChannelGroups * runQueues;

    // FB reserved memory is required only when FB aperture is enabled for this buffer.
    if ((bCalcForFbRsvd) && (DRF_VAL( _REG_STR_RM, _INST_LOC_3, _FAULT_METHOD_BUFFER, pGpu->instLocOverrides3 ) !=
         NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER_VID))
    {
        totalSize = 0;
    }

    return totalSize;
}

/*!
 * Special function to be used early when the CHID_MGRs aren't and cannot be
 * constructed in all cases. Do not use otherwise
 */
NvU32
kfifoGetMaxCeChannelGroups_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32 numChannels = kfifoRunlistQueryNumChannels_HAL(pGpu, pKernelFifo, 0);

    if (pKernelFifo->bNumChannelsOverride)
        numChannels = NV_MIN(pKernelFifo->numChannelsOverride, numChannels);

    return numChannels;
}

/*
 * Allocate Memory Descriptors for Regmem VF page
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelFifo        KernelFifo pointer
 */
NV_STATUS
kfifoConstructUsermodeMemdescs_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32          attr           = 0;
    NvU32          attr2          = 0;
    NvU64          offset         = 0;
    NvU32          size           = 0;

    attr = FLD_SET_DRF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS, attr);
    attr = FLD_SET_DRF(OS32, _ATTR,  _COHERENCY, _CACHED, attr);

    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, attr2);

    NV_ASSERT_OK_OR_RETURN(kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo, &offset, &size));

    NV_ASSERT_OK_OR_RETURN(memCreateMemDesc(pGpu, &(pKernelFifo->pRegVF), ADDR_REGMEM,
                                            offset, size, attr, attr2));

    memdescSetFlag(pKernelFifo->pRegVF, MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK, NV_TRUE);

    return NV_OK;
}

/**
 * @brief Converts a MMU access type type (NV_PFAULT_ACCESS_TYPE_*) into a string.
 *
 * @param pGpu
 * @param pKernelFifo
 * @param faultId
 * @returns a string (always non-null)
 */
const char*
kfifoGetFaultAccessTypeString_GV100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       accessType
)
{
    switch (accessType)
    {
        case NV_PFAULT_ACCESS_TYPE_VIRT_READ:
            return "ACCESS_TYPE_VIRT_READ";
        case NV_PFAULT_ACCESS_TYPE_VIRT_WRITE:
            return "ACCESS_TYPE_VIRT_WRITE";
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_STRONG:
        case NV_PFAULT_ACCESS_TYPE_VIRT_ATOMIC_WEAK:
            return "ACCESS_TYPE_VIRT_ATOMIC";
        case NV_PFAULT_ACCESS_TYPE_VIRT_PREFETCH:
            return "ACCESS_TYPE_VIRT_PREFETCH";
        case NV_PFAULT_ACCESS_TYPE_PHYS_READ:
            return "ACCESS_TYPE_PHYS_READ";
        case NV_PFAULT_ACCESS_TYPE_PHYS_WRITE:
            return "ACCESS_TYPE_PHYS_WRITE";
        case NV_PFAULT_ACCESS_TYPE_PHYS_ATOMIC:
            return "ACCESS_TYPE_PHYS_ATOMIC";
        case NV_PFAULT_ACCESS_TYPE_PHYS_PREFETCH:
            return "ACCESS_TYPE_PHYS_PREFETCH";
        default:
            return "UNRECOGNIZED_ACCESS_TYPE";
    }
}

/*!
 * @brief Update the usermode doorbell register with work submit token to notify
 *        host that work is available on this channel.
 *
 * @param[in] pGpu
 * @param[in] pFifo
 * @param[in] pKernelChannel  Channel to ring the doorbell for
 */
NV_STATUS
kfifoRingChannelDoorBell_GV100
(
    OBJGPU          *pGpu,
    KernelFifo      *pKernelFifo,
    KernelChannel   *pKernelChannel
)
{
    NvU32        workSubmitToken;

    NV_ASSERT_OK_OR_RETURN(kfifoGenerateWorkSubmitToken(pGpu, pKernelFifo,
                                                        pKernelChannel, &workSubmitToken,
                                                        NV_TRUE));
    NV_ASSERT_OK_OR_RETURN(kfifoUpdateUsermodeDoorbell_HAL(pGpu, pKernelFifo,
                                                            workSubmitToken));
    return NV_OK;
}
