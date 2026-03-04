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

/*!
 * Provides GB202 specific KernelGsp HAL implementations.
 */

#include "rmconfig.h"
#include "gpu/gsp/kernel_gsp.h"
#include "os/os.h"

#define getUpperPCIBits(Id) ((Id >> 16) & 0xFFFF)
#define PCI_TABLE_ENTRY(id, subId) ((getUpperPCIBits(pciId) == id) && (getUpperPCIBits(subDeviceId) == subId))

static NvBool _is48VmEnabled(NvU64 pciId, NvU64 subDeviceId)
{
    if (PCI_TABLE_ENTRY(0x2BB5, 0x204E) || PCI_TABLE_ENTRY(0x2BB1, 0x204B))
        return NV_TRUE;

    return NV_FALSE;
}

NvU64 kgspVgpuFwHeapSize_GB202(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // This check only allows a subset of GPUs to support 48VMs. This comes from 
    // VGPU requirements.
    if (_is48VmEnabled(pGpu->idInfo.PCIDeviceID, pGpu->idInfo.PCISubDeviceID))
        return GSP_FW_HEAP_SIZE_VGPU_48VMS;

    return GSP_FW_HEAP_SIZE_VGPU_DEFAULT;
}

NvU64 kgspVgpuNumVgpuPartitions_GB202(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    // This check only allows a subset of GPUs to support 48VMs. This comes from 
    // VGPU requirements.
    if (_is48VmEnabled(pGpu->idInfo.PCIDeviceID, pGpu->idInfo.PCISubDeviceID))
        return MAX_PARTITIONS_WITH_GFID;

    return MAX_PARTITIONS_WITH_GFID_32VM;
}
