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


/********************* Chip Specific HAL CCU Routines *********************\
 *                                                                         *
 *   The Blackwell specific HAL CCU routines reside in this file.             *
 *                                                                         *
\**************************************************************************/

#include "kernel/gpu/ccu/kernel_ccu.h"

/*!
 * Get CCU shared buffer informtion
 *
 * @param[in] pGpu              GPU object pointer
 * @param[in/out] pKernelCcu    KernelCcu object pointer
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_OBJECT
 */
NV_STATUS
kccuGetBufSize_GB100
(
    OBJGPU    *pGpu,
    KernelCcu *pKernelCcu
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS inParams = {0};

    // Internal RM api ctrl call to physical RM to map shared buffer memdesc
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CCU_GET_SAMPLE_INFO,
                             &inParams,
                             sizeof(inParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU sample size retrieval failed with status: 0x%x\n", status);
        return status;
    }

    pKernelCcu->devBufSize = 2 * (inParams.ccuSampleSize - sizeof(NvU64));
    // Plus head & tail timestamp size + mig-SwizzId size
    pKernelCcu->devSharedBufSize = pKernelCcu->devBufSize +
                                   2 * CCU_TIMESTAMP_SIZE + CCU_MIG_SWIZZID_SIZE;

    pKernelCcu->migBufSize = inParams.ccuSampleSize - sizeof(NvU64);
    // Plus head & tail timestamp size + mig-SwizzId size + compute-instId size
    pKernelCcu->migSharedBufSize = pKernelCcu->migBufSize + 
                                   2 * CCU_TIMESTAMP_SIZE + CCU_MIG_SWIZZID_SIZE + CCU_MIG_COMPUTEID_SIZE;

    return NV_OK;
}