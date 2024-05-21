/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/********************* VGPU Specific FB Routines *********************\
 *                                                                   *
 *   VGPU specific VMA routines reside in this file.                 *
 *                                                                   *
\*********************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "vgpu/rpc.h"
#include "gpu/device/device.h"

NV_STATUS
dmaConstructHal_VF(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    return NV_OK;
}

//
// deviceCtrlCmdDmaFlush_VF
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaFlush_VF
(
    Device *pDevice,
    NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams
)
{
    NV_STATUS status = NV_OK;
    return status;
}
