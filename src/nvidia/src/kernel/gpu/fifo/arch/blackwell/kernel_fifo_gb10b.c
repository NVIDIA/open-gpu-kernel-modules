/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "vgpu/vgpu_events.h"

#include "published/blackwell/gb10b/dev_fault.h"

/**
 * @brief Converts a subid/clientid into a client string
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] pMmuExceptData

 * @returns a string (always non-null)
 */
const char*
kfifoGetClientIdString_GB10B
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
            case NV_PFAULT_CLIENT_HUB_GSPLITE:
                return "HUBCLIENT_GSPLITE";
            case NV_PFAULT_CLIENT_HUB_GSPLITE1:
                return "HUBCLIENT_GSPLITE1";
            case NV_PFAULT_CLIENT_HUB_GSPLITE2:
                return "HUBCLIENT_GSPLITE2";
            case NV_PFAULT_CLIENT_HUB_GSPLITE3:
                return "HUBCLIENT_GSPLITE3";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER2:
                return "HUBCLIENT_VPR_SCRUBBER2";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER3:
                return "HUBCLIENT_VPR_SCRUBBER3";
            case NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER4:
                return "HUBCLIENT_VPR_SCRUBBER4";
            case NV_PFAULT_CLIENT_HUB_NVENC3:
                return "HUBCLIENT_NVENC3";
            case NV_PFAULT_CLIENT_HUB_PD1:
                return "HUBCLIENT_PD1";
            case NV_PFAULT_CLIENT_HUB_PD2:
                return "HUBCLIENT_PD2";
            case NV_PFAULT_CLIENT_HUB_PD3:
                return "HUBCLIENT_PD3";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD1:
                return "HUBCLIENT_RASTERTWOD1";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD2:
                return "HUBCLIENT_RASTERTWOD2";
            case NV_PFAULT_CLIENT_HUB_RASTERTWOD3:
                return "HUBCLIENT_RASTERTWOD3";
            case NV_PFAULT_CLIENT_HUB_SCC1:
                return "HUBCLIENT_SCC1";
            case NV_PFAULT_CLIENT_HUB_SCC_NB1:
                return "HUBCLIENT_SCC_NB1";
            case NV_PFAULT_CLIENT_HUB_SCC2:
                return "HUBCLIENT_SCC2";
            case NV_PFAULT_CLIENT_HUB_SCC_NB2:
                return "HUBCLIENT_SCC_NB2";
            case NV_PFAULT_CLIENT_HUB_SCC3:
                return "HUBCLIENT_SCC3";
            case NV_PFAULT_CLIENT_HUB_SCC_NB3:
                return "HUBCLIENT_SCC_NB3";
            case NV_PFAULT_CLIENT_HUB_SSYNC1:
                return "HUBCLIENT_SSYNC1";
            case NV_PFAULT_CLIENT_HUB_SSYNC2:
                return "HUBCLIENT_SSYNC2";
            case NV_PFAULT_CLIENT_HUB_SSYNC3:
                return "HUBCLIENT_SSYNC3";
        }
    }

    // Fallback if the above doesn't cover the given client ID
    return kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, pMmuExceptInfo);
}
