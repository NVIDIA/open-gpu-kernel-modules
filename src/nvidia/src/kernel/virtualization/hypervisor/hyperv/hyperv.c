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
#define FEATURE_IDENTIFICATION_LEAF  0x40000003
#define IMPLEMENTATION_RECOMMENDATIONS_LEAF  0x40000004
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

    // See if CreatePartitions is set (which is 0th bit in ebx),
    // which determines if we are in parent/root partition.
    // or
    // when nested virtualization is enabled, CreatePartitions is set.
    // Hence see if 12th bit of eax is set which indicates if the hypervisor
    // is nested within a Hyper-V partition.
    if(!(ebx & 0x1) || (eax & 0x1000))
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

