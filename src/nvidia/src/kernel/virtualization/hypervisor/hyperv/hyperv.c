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

/*!
 * @file
 * @brief MS Hyper-V hypervisor support
 */

#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"

static NvBool _dummyHypervIsP2PSupported(NvU32 gpuMask)
{
    return NV_FALSE;
}

//
// For hypervisors like HyperV we need to identify whether we are running in
// non-root partition.
// If we are running in Child partition, then only we enable NMOS code path.
//
#define FEATURE_IDENTIFICATION_LEAF          0x40000003
#define IMPLEMENTATION_RECOMMENDATIONS_LEAF  0x40000004

#define NESTED_HYPERVISOR  0x00001000   // 12th bit in EAX

#define CREATE_PARTITIONS  0x00000001   //  0th bit in EBX
#define CPU_MANAGEMENT     0x00001000   // 12th bit in EBX

static NV_STATUS _childPartitionDetection(OBJOS *pOS, NvBool *result)
{
    NvU32 eax = 0, ebx = 0;
    NvU32 dummyRegister = 0;    // This is placeholder reg to pass as an argument
                                // to read registers values we don't care

    NV_ASSERT(result != NULL);
    *result = NV_FALSE;

    // See if we are in parent/child partition
    if ((osNv_cpuid(FEATURE_IDENTIFICATION_LEAF, 0, &dummyRegister, &ebx, &dummyRegister, &dummyRegister) == 0) ||
        (osNv_cpuid(IMPLEMENTATION_RECOMMENDATIONS_LEAF, 0, &eax, &dummyRegister, &dummyRegister, &dummyRegister) == 0))
    {
        NV_PRINTF(LEVEL_WARNING, "CPUID is NOT supported!\n");
            return NV_ERR_NOT_SUPPORTED;
    }

    // See if CpuManagement bit is set which determines if we are in parent/root partition.
    // or
    // when CpuManagement bit is not set but CreatePartitions bit is set then
    // it's a L1VH child partition 
    // or
    // when CpuManagement bit is set but NestedHypervisor bit is also set then
    // it's a nested hypervisor child partition otherwsie it's a regular child partition.
    if(!(ebx & CPU_MANAGEMENT) && (ebx & CREATE_PARTITIONS))
    {
        // We are in L1VH child partition
        *result = NV_TRUE;
    }
    else if ((ebx & CPU_MANAGEMENT) && (eax & NESTED_HYPERVISOR))
    {
        // We are in nested hypervisor child partition
        *result = NV_TRUE;
    }
    else if (ebx & CPU_MANAGEMENT)
    {
        // We are in parent/root partition
        *result = NV_FALSE;
    }
    else
    {
        // We are in child partition
        *result = NV_TRUE;
    }

    return NV_OK;
}

HYPERVISOR_OPS hypervHypervisorOps =
{
    SFINIT(.hypervisorName, "HyperV"),
    SFINIT(.hypervisorSig, "Microsoft Hv"),
    SFINIT(.hypervisorPostDetection, _childPartitionDetection),
    SFINIT(.hypervisorIsPcieP2PSupported, _dummyHypervIsP2PSupported),
};

