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
#include "kernel/gpu/gpu_access.h"

#include "published/pascal/gp102/dev_pbdma.h"

static NvBool _kfifoIsValidCETag_GP102(OBJGPU *pGpu, KernelFifo *pKernelFifo,
                                       NvU32 ceEngineTag);

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
kfifoValidateSCGTypeAndRunqueue_GP102
(
    KernelFifo *pKernelFifo,
    NvU32       scgType,
    NvU32       runqueue
)
{
    if (scgType == NV_PPBDMA_SET_CHANNEL_INFO_SCG_TYPE_COMPUTE1 &&
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
 * @brief Verifies an engine tag and runqueue combination
 *
 * @param pGpu
 * @param pKernelFifo
 * @param engDesc
 * @param runqueue
 *
 * @returns a boolean
 */
NvBool
kfifoValidateEngineAndRunqueue_GP102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engDesc,
    NvU32       runqueue
)
{
    NvBool bIsGrCe = NV_FALSE;
    switch (runqueue)
    {
        case 0:
            // All engines can run on runqueue 0
            return NV_TRUE;
        case 1:
            if (IS_GR(engDesc))
                return NV_TRUE;

            if (IS_CE(engDesc))
            {
                bIsGrCe = _kfifoIsValidCETag_GP102(pGpu, pKernelFifo, engDesc);
                if(!bIsGrCe)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Runqueue 1 only supports GR/GRCE\n");
                }
                return bIsGrCe;
            }

            // To handle NV50_DEFERRED_API_CLASS alloc
            if (engDesc == ENG_SW)
                return NV_TRUE;

            NV_PRINTF(LEVEL_ERROR, "Runqueue 1 only supports GR/GRCE\n");

            return NV_FALSE;
        default:
            // Only 0 and 1 are supported
            NV_PRINTF(LEVEL_ERROR, "Unsupported runqueue: 0x%x\n", runqueue);
            NV_ASSERT(0);
            break;
    }

    return NV_FALSE;
}


/**
 * @brief Verifies an engine tag and runqueue combination
 *
 * @param pGpu
 * @param pKernelFifo
 * @param engDesc
 * @param runqueue
 *
 * @returns a boolean
 */
NvBool
kfifoValidateEngineAndSubctxType_GP102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       engDesc,
    NvU32       subctxType
)
{
    NvBool bIsGrCe = NV_FALSE;
    switch (subctxType)
    {
        case 0:
            // All engines can run on SYNC subcontext
            return NV_TRUE;
        case 1:
            if (IS_GR(engDesc))
                return NV_TRUE;

            if (IS_CE(engDesc))
            {
                bIsGrCe = _kfifoIsValidCETag_GP102(pGpu, pKernelFifo, engDesc);
                if(!bIsGrCe)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "ASYNC Subcontext only supported on GR/GRCE\n");
                }
                return bIsGrCe;
            }

            // To handle NV50_DEFERRED_API_CLASS alloc
            if (engDesc == ENG_SW)
                return NV_TRUE;

            NV_PRINTF(LEVEL_ERROR,
                      "ASYNC Subcontext only supported on GR/GRCE\n");

            return NV_FALSE;
        default:
            // Only 0 and 1 are supported
            NV_PRINTF(LEVEL_ERROR, "Unsupported Subcontext Type: 0x%x\n",
                      subctxType);
            NV_ASSERT(0);
            break;
    }

    return NV_FALSE;
}

static NvBool
_kfifoIsValidCETag_GP102
(
    OBJGPU *pGpu,
    KernelFifo *pKernelFifo,
    NvU32 ceEngineTag
)
{
    NvU32 ceRunlist    = 0;
    NvU32 *pCePbdmaIds;
    NvU32 numCePbdmaIds;
    NvU32 srcRunlist   = 0;
    NvU32 *pSrcPbdmaIds;
    NvU32 numSrcPbdmaIds;
    NvU32 i;
    NvU32 grEngineTag = ENG_GR(0);

    if (kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                 ENGINE_INFO_TYPE_ENG_DESC, ceEngineTag,
                                 ENGINE_INFO_TYPE_RUNLIST, &ceRunlist) != NV_OK)
        return NV_FALSE;

    NV_ASSERT_OR_RETURN(
        kfifoGetEnginePbdmaIds_HAL(pGpu, pKernelFifo,
                                  ENGINE_INFO_TYPE_ENG_DESC, ceEngineTag,
                                  &pCePbdmaIds, &numCePbdmaIds) == NV_OK,
        NV_FALSE);

    NV_ASSERT_OR_RETURN(
        kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                 ENGINE_INFO_TYPE_ENG_DESC, grEngineTag,
                                 ENGINE_INFO_TYPE_RUNLIST, &srcRunlist) == NV_OK,
        NV_FALSE);

    NV_ASSERT_OR_RETURN(
        kfifoGetEnginePbdmaIds_HAL(pGpu, pKernelFifo,
                                  ENGINE_INFO_TYPE_ENG_DESC, grEngineTag,
                                  &pSrcPbdmaIds, &numSrcPbdmaIds) == NV_OK,
        NV_FALSE);

    if (ceRunlist != srcRunlist)
    {
        return NV_FALSE;
    }

    for (i = 0; i < numCePbdmaIds; i++)
    {
        NvU32 j;
        for (j = 0; j < numSrcPbdmaIds; j++)
        {
            if (pCePbdmaIds[i] == pSrcPbdmaIds[j])
            {
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}
