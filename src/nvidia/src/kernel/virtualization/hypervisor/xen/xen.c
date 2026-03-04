/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Xen hypervisor support
 */

#include "os/os.h"
#include "virtualization/hypervisor/hypervisor.h"

static NvBool _dummyXenIsP2PSupported(NvU32 gpuMask)
{
    return NV_FALSE;
}

//
// For hypervisors other than HyperV we support, we do not need special detection
// for identification of Child partition. Just to keep consistency in
// _hypervisor_ops structure, we keep this function which always returns true
// for Child partition.
//
static NV_STATUS _dummyXenPostDetection(OBJOS *pOS, NvBool *result)
{
    *result = NV_TRUE;
    return NV_OK;
}

HYPERVISOR_OPS xenHypervisorOps =
{
    SFINIT(.hypervisorName, "Xen"),
    SFINIT(.hypervisorSig, "XenVMMXenVMM"),
    SFINIT(.hypervisorPostDetection, _dummyXenPostDetection),
    SFINIT(.hypervisorIsPcieP2PSupported, _dummyXenIsP2PSupported),
};

