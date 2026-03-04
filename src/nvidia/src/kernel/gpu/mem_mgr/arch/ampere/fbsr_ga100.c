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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/fbsr.h"

NV_STATUS
fbsrSendMemsysProgramRawCompressionMode_GA100
(
    OBJGPU  *pGpu,
    OBJFBSR *pFbsr,
    NvBool   bRawMode
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS params = {0};

    params.bRawMode = bRawMode;

    return pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE,
                           &params, sizeof(params));
}

/*!
 * Start save/restore operation
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 * @param[in]     op           Type of operation
 *
 * @returns NV_OK on success
 */
NV_STATUS
fbsrBegin_GA100(OBJGPU *pGpu, OBJFBSR *pFbsr, FBSR_OP_TYPE op)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if ((op == FBSR_OP_RESTORE) && !IS_VIRTUAL(pGpu) &&
        pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend)
    {
        const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
                kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

        /*
         * Temporarily disable raw mode to prevent FBSR restore operations
         * from corrupting compressed surfaces.
         *
         * See bug 3172217
         */

        if (pMemorySystemConfig->bUseRawModeComptaglineAllocation)
        {
            NV_ASSERT_OK(fbsrSendMemsysProgramRawCompressionMode_HAL(pGpu, pFbsr, NV_FALSE));
            pFbsr->bRawModeWasEnabled = NV_TRUE;
        }
    }

    return fbsrBegin_GM107(pGpu, pFbsr, op);
}

/*!
 * End save/restore operation
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[in]     pFbsr        OBJFBSR pointer
 *
 * @returns NV_OK on success
 */
NV_STATUS
fbsrEnd_GA100(OBJGPU *pGpu, OBJFBSR *pFbsr)
{
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    NV_STATUS status = fbsrEnd_GM107(pGpu, pFbsr);

    if ((pFbsr->op == FBSR_OP_RESTORE) && pFbsr->bRawModeWasEnabled &&
        !IS_VIRTUAL(pGpu) && pKernelMemorySystem->bPreserveComptagBackingStoreOnSuspend)
    {
        /*
         * Reenable raw mode if it was disabled by fbsrBegin_GA100.
         */
        NV_ASSERT_OK(fbsrSendMemsysProgramRawCompressionMode_HAL(pGpu, pFbsr, NV_TRUE));
        pFbsr->bRawModeWasEnabled = NV_FALSE;
    }

    return status;
}
