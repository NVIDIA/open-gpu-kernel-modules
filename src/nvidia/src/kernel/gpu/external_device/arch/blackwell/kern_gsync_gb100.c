/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/external_device/gsync.h"
#include "gpu/external_device/dac_p2060.h"

// Returns false if this firmware is acceptable to the GPU
// Returns true if this firmware is outdated etc.
NvBool
gsyncmgrIsFirmwareGPUMismatch_GB100
(
    OBJGPU *pGpu,
    OBJGSYNC *pGsync
)
{
    DACEXTERNALDEVICE *pExtDev;

    NV_ASSERT_OR_RETURN(pGsync != NULL, NV_TRUE);
    pExtDev = pGsync->pExtDev;
    NV_ASSERT_OR_RETURN(pExtDev != NULL, NV_TRUE);

    // Requires a P2061 board with 3.00+
    if (pExtDev->deviceId == DAC_EXTERNAL_DEVICE_P2061)
    {
        if (P2061_FW_REV(pExtDev) >= 0x300)
        {
            // Acceptable firmware
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}
