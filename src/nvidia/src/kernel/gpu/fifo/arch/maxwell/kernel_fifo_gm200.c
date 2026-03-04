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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "vgpu/rpc.h"

#include "published/maxwell/gm200/dev_fifo.h"

/**
 * @brief
 *
 * @param pGpu
 * @param pKernelFifo
 * @param pbdmaCount
 */
NvU32
kfifoGetNumPBDMAs_GM200
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    const ENGINE_INFO *pEngineInfo = kfifoGetEngineInfo(pKernelFifo);
    NvU32 numPbdmas;

    /*!
     * If pEngineInfo is NULL, i.e. before fifoConstructEngineList,
     * then read HW and return value, else return cached value
     */
    if (pEngineInfo == NULL)
    {
        NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu), 0);

        numPbdmas = GPU_REG_RD_DRF(pGpu, _PFIFO, _CFG0, _NUM_PBDMA);

        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            numPbdmas = 0;
        }

        NV_PRINTF(LEVEL_INFO, "%d PBDMAs\n", numPbdmas);
    }
    else
    {
        numPbdmas = pEngineInfo->maxNumPbdmas;
    }

    return numPbdmas;
}
