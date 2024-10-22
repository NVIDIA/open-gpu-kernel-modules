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

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/conf_compute/conf_compute.h"
// #include "class/cl2080.h"
// #include "libraries/nvport/nvport.h"
// #if RMCFG_FEATURE_PLATFORM_UNIX && RMCFG_MODULE_SPDM
// #include "kernel/gpu/spdm/libspdm_includes.h"
// #include "hal/library/cryptlib.h"
// #endif

/*!
 * Return the key ID for a given LCE channel and rotation operation.
 * If rotateOperation is ROTATE_IV_ALL_VALID then it will return the least
 * key ID of the key pair; ie the one that corresponds to an even numbered slot.
 *
 * @param[in]   pConfCompute    : conf comp pointer
 * @param[in]   pKernelChannel  : KernelChannel pointer
 * @param[in]   rotateOperation : The type of rotation operation
 * @param[out]  pKeyId          : pointer to a key Id
 */
NV_STATUS
confComputeGetLceKeyIdFromKChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    ROTATE_IV_TYPE       rotateOperation,
    NvU16               *pKeyId
)
{
    NV_ASSERT_OR_RETURN(pKeyId != NULL, NV_ERR_INVALID_PARAMETER);

    if ((rotateOperation == ROTATE_IV_ENCRYPT) ||
        (rotateOperation == ROTATE_IV_ALL_VALID))
    {
        *pKeyId = CC_LKEYID_LCE_H2D_KERN;
    }
    else
    {
        *pKeyId = CC_LKEYID_LCE_D2H_KERN;
    }

    return NV_OK;
}
