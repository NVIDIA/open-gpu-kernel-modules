/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/bif/kernel_bif.h"

#include "published/volta/gv11b/dev_boot.h"


/*!
 * @brief  Get the PMC bit of the valid Engines to reset.
 *
 * @return All valid engines
 */
NvU32
kbifGetValidEnginesToReset_GV11B
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    return (DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _CE2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PWR, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PDISP, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVDEC, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC0, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC1, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _NVENC2, _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _SEC, _ENABLED));
}
