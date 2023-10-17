/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "virtualization/common_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"

void
vgpuMgrFillVgpuType(NVA081_CTRL_VGPU_INFO *pVgpuInfo, VGPU_TYPE *pVgpuTypeNode)
{
}

NV_STATUS
vgpuMgrReserveSystemChannelIDs
(
    OBJGPU *pGpu,
    VGPU_TYPE *vgpuTypeInfo,
    NvU32 gfid,
    NvU32 *pChidOffset,
    NvU32 *pChannelCount,
    Device *pMigDevice,
    NvU32 numChannels,
    NvU32 engineFifoListNumEntries,
    FIFO_ENGINE_LIST *engineFifoList
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
vgpuMgrFreeSystemChannelIDs
(
    OBJGPU *pGpu,
    NvU32 gfid,
    NvU32 *pChidOffset,
    NvU32 *pChannelCount,
    Device *pMigDevice,
    NvU32 engineFifoListNumEntries,
    FIFO_ENGINE_LIST *engineFifoList
)
{
}
