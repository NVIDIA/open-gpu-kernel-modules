/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *   The HOPPER specific HAL CCU routines reside in this file.             *
 *                                                                         *
\**************************************************************************/

#include "kernel/gpu/ccu/kernel_ccu.h"

/*!
 * Assign CCU shared buffer to mig inst
 *
* @param[in] pGpu          GPU object pointer
* @param[in] pKernelCcu    KernelCcu object pointer
* @param[in] bMigEnabled   Mig enabled
 *
 * @return  NV_OK
 * @return  NV_ERR_INSUFFICIENT_RESOURCES
 */
NV_STATUS
kccuMigShrBufHandler_GH100
(
    OBJGPU    *pGpu,
    KernelCcu *pKernelCcu,
    NvBool    bMigEnabled
)
{
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Create/delete CCU shared buffers for mig (migEnabled:%u)\n",
              bMigEnabled);

    if (bMigEnabled)
    {
        if (!pKernelCcu->bMigShrBufAllocated)
        {
            status = kccuInitMigSharedBuffer(pGpu, pKernelCcu);
            if (status == NV_OK)
            {
                pKernelCcu->bMigShrBufAllocated = NV_TRUE;
                kccuShrBufInfoToCcu(pGpu, pKernelCcu, CCU_MIG_SHRBUF_ID_START);
            }
        }

        return status;
    }

    if (pKernelCcu->bMigShrBufAllocated)
    {
        NvU32 i = 0;
        NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS params = { 0 };

        // Internal RM api ctrl call to physical RM to unmap shared buffer memdesc
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        params.bMigShrBuf = NV_TRUE;
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP,
                                 &params,
                                 sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "CCU mig memdesc unmap request failed with status: 0x%x\n", status);
        }

        // Free mig shared buffer
        for (i = CCU_MIG_SHRBUF_ID_START; i < CCU_SHRBUF_COUNT_MAX; i++)
        {
            if (pKernelCcu->pMemDesc[i] != NULL)
            {
                kccuShrBufIdxCleanup(pGpu, pKernelCcu, i);
            }
        }

        pKernelCcu->bMigShrBufAllocated = NV_FALSE;
    }

    return status;
}

/*!
 * Get CCU shared buffer informtion
 *
 * @param[in] pGpu              GPU object pointer
 * @param[in/out] pKernelCcu    KernelCcu object pointer
 *
 * @return  NV_OK
 */
NV_STATUS
kccuGetBufSize_GH100
(
    OBJGPU    *pGpu,
    KernelCcu *pKernelCcu
)
{
    pKernelCcu->devBufSize = CCU_DEV_BUF_SIZE_16KB;
    pKernelCcu->devSharedBufSize = CCU_DEV_SHARED_BUF_SIZE;
    pKernelCcu->migBufSize = CCU_MIG_BUF_SIZE_2KB;
    pKernelCcu->migSharedBufSize = CCU_MIG_SHARED_BUF_SIZE;

    return NV_OK;
}
