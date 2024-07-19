/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief  Collection of interfaces for perf profiling
 */

/* ------------------------ Includes --------------------------------------- */
#include "gpu/hwpm/kern_hwpm.h"
#include "ctrl/ctrlb0cc.h"

/* ------------------------ Static Function Prototypes --------------------- */
static RefcntStateChangeCallback _khwpmProfilerPmaVaSpaceAllocStateChange;

/* ------------------------ Public Functions  ------------------------------ */

static NV_STATUS
_khwpmProfilerPmaVaSpaceAllocStateChange
(
    OBJREFCNT   *pRefcnt,
    Dynamic     *pDynamic,
    REFCNT_STATE oldState,
    REFCNT_STATE newState
)
{
    NV_STATUS   status         = NV_OK;
    NvBool      bCreateVaSpace = NV_FALSE;
    KernelHwpm *pKernelHwpm    = dynamicCast(pDynamic, KernelHwpm);
    NvU32       bpcIdx;
    OBJGPU     *pGpu;

    if (pRefcnt == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (pKernelHwpm == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pGpu = ENG_GET_GPU(pKernelHwpm);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    bCreateVaSpace = (newState == REFCNT_STATE_ENABLED);
    bpcIdx = pRefcnt->tag;
    NV_ASSERT_OR_RETURN(bpcIdx < pKernelHwpm->maxCblocks, NV_ERR_INVALID_STATE);

    if (bCreateVaSpace)
    {
        if (pKernelHwpm->streamoutState[bpcIdx].bPmaVasRequested)
        {
            return NV_ERR_STATE_IN_USE;
        }

        status = khwpmStreamoutCreatePmaVaSpace(pGpu, pKernelHwpm, bpcIdx);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate PMA VA space for CBLOCK ID 0x%x. Error 0x%x\n", bpcIdx, status);
            return status;
        }

        pKernelHwpm->streamoutState[bpcIdx].bPmaVasRequested = NV_TRUE;
    }
    else
    {
        if (!pKernelHwpm->streamoutState[bpcIdx].bPmaVasRequested)
        {
            return NV_ERR_INVALID_STATE;
        }

        status = khwpmStreamoutFreePmaVaSpace(pGpu, pKernelHwpm, bpcIdx);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to free PMA VA space for CBLOCK ID 0x%x. Error 0x%x\n", bpcIdx, status);
            return status;
        }

        pKernelHwpm->streamoutState[bpcIdx].bPmaVasRequested = NV_FALSE;
    }

    return status;
}

/*!
 * @brief Creates the refcount object for HWPM Pma VA space
 * for a given PMA Channel ID
 */
static NV_STATUS
_khwpmProfilerPmaVaSpaceRefcntInit
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm,
    NvU32       bpcIdx
)
{
    NV_STATUS  status        = NV_OK;
    OBJREFCNT *pPmaVasRefcnt = NULL;

    status = objCreate(&pPmaVasRefcnt, pKernelHwpm, OBJREFCNT,
                       staticCast(pKernelHwpm, Dynamic), bpcIdx, &_khwpmProfilerPmaVaSpaceAllocStateChange, NULL);
    if (status != NV_OK)
    {
        return status;
    }

    pKernelHwpm->streamoutState[bpcIdx].pPmaVasRefcnt = pPmaVasRefcnt;

    return status;
}

static NV_STATUS
_khwpmInitPmaStreamAttributes
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm
)
{
    pKernelHwpm->numPma = pKernelHwpm->getProperty(pKernelHwpm, PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED) ? 2 : 1;
    NV_ASSERT_OR_RETURN(pKernelHwpm->numPma <= MAX_PMA_CREDIT_POOL, NV_ERR_INVALID_STATE);

    khwpmGetCblockInfo_HAL(pGpu, pKernelHwpm, &pKernelHwpm->maxCblocksPerPma, &pKernelHwpm->maxChannelPerCblock);

    pKernelHwpm->maxPmaChannels =
        pKernelHwpm->maxCblocksPerPma * pKernelHwpm->maxChannelPerCblock * pKernelHwpm->numPma;
    pKernelHwpm->maxCblocks = pKernelHwpm->numPma * pKernelHwpm->maxCblocksPerPma;

    return NV_OK;
}

NV_STATUS
khwpmStateInitUnlocked_IMPL
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm
)
{
    NV_STATUS status = NV_OK;
    NvU32     bpcIdx;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        _khwpmInitPmaStreamAttributes(pGpu, pKernelHwpm), khwpmStateInitUnlocked_exit);

    // Except full SRIOV, VA space creation is not required on vGPU guest
    // It will happen on vGPU host
    if (IS_VIRTUAL(pGpu) && !IS_SRIOV_FULL_GUEST(pGpu))
    {
        goto khwpmStateInitUnlocked_exit;
    }

    pKernelHwpm->vaSpaceBase = PERF_VASPACE_BASE;

    pKernelHwpm->vaSpaceSize = PERF_VASPACE_SIZE - PERF_VASPACE_BASE;

    pKernelHwpm->streamoutState = portMemAllocNonPaged(sizeof(HWPM_STREAMOUT_STATE) * pKernelHwpm->maxCblocks);
    if (pKernelHwpm->streamoutState == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto khwpmStateInitUnlocked_exit;
    }

    portMemSet(pKernelHwpm->streamoutState, 0, (sizeof(HWPM_STREAMOUT_STATE) * pKernelHwpm->maxCblocks));

    for (bpcIdx = 0; bpcIdx < pKernelHwpm->maxCblocks; bpcIdx++)
    {
        status = _khwpmProfilerPmaVaSpaceRefcntInit(pGpu, pKernelHwpm, bpcIdx);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Initialization of VaSpace Refcnt objects failed!\n");
            goto khwpmStateInitUnlocked_exit;
        }
    }

khwpmStateInitUnlocked_exit:
    if (status != NV_OK)
    {
        khwpmStateDestroy(pGpu, pKernelHwpm);
    }
    return status;
}

void
khwpmStateDestroy_IMPL
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm
)
{
    NvU32 bpcIdx;

    if (pKernelHwpm->streamoutState != NULL)
    {
        for (bpcIdx = 0; bpcIdx < pKernelHwpm->maxCblocks; bpcIdx++)
        {
            objDelete(pKernelHwpm->streamoutState[bpcIdx].pPmaVasRefcnt);
            pKernelHwpm->streamoutState[bpcIdx].pPmaVasRefcnt = NULL;
        }
        portMemFree(pKernelHwpm->streamoutState);
    }
}

NV_STATUS
khwpmGetRequestCgStatusMask
(
    NvU32                              *pCgStatusMask,
    HWPM_POWER_REQUEST_FEATURES_PARAMS *pParams
)
{
#define HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(powerFeature)                                  \
    do                                                                                           \
    {                                                                                            \
        NvU32 profilerCgStatus = 0;                                                              \
        NvU32 status = REF_VAL(HWPM_POWER_FEATURE_MASK_##powerFeature, pParams->statusMask);     \
        switch (status)                                                                          \
        {                                                                                        \
            case HWPM_POWER_FEATURE_REQUEST_FULFILLED:                                           \
                profilerCgStatus = NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED;            \
                break;                                                                           \
            case HWPM_POWER_FEATURE_REQUEST_REJECTED:                                            \
                profilerCgStatus = NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED;             \
                break;                                                                           \
            case HWPM_POWER_FEATURE_REQUEST_NOT_SUPPORTED:                                       \
                profilerCgStatus = NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED;        \
                break;                                                                           \
            case HWPM_POWER_FEATURE_REQUEST_FAILED:                                              \
                profilerCgStatus = NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED;               \
                break;                                                                           \
            default:                                                                             \
                return NV_ERR_INVALID_ARGUMENT;                                                  \
        }                                                                                        \
        *pCgStatusMask = FLD_SET_DRF_NUM(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK,                   \
                                         _##powerFeature, profilerCgStatus, (*pCgStatusMask));   \
    }                                                                                            \
    while (0)

    HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(ELCG);
    HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(BLCG);
    HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(SLCG);
    HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(ELPG);
    HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS(IDLE_SLOWDOWN);

#undef HWPM_PROFILER_GET_REQUEST_CG_STATUS_MASKS
    return NV_OK;
}
