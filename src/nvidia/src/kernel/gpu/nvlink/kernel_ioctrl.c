/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

/*!
 * @brief Construct a KernelIoctrl object
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pKernelIoctrl KernelIoctrl object pointer
 * @param[in]  publicId      The ID of the ioctrl to be constructed
 *
 * @return  NV_OK on success
 */
NV_STATUS
kioctrlConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    KernelIoctrl  *pKernelIoctrl,
    NvU32          publicId
)
{
    NV_STATUS    status = NV_OK;
    OBJENGSTATE *pEngstate;

    pKernelIoctrl->PublicId = publicId;

    // Default the IOCTRL IP version
    pKernelIoctrl->ipVerIoctrl = NVLINK_VERSION_10;

    pEngstate = dynamicCast(pKernelIoctrl, OBJENGSTATE);
    pEngstate->engDesc = ENG_KERNEL_IOCTRL(publicId);

    status = kioctrlMinionConstruct_HAL(pGpu, pKernelIoctrl);

    return status;
}

/*!
 * @brief Destruct a KernelIoctrl object
 *
 * @param[in] pKernelIoctrl  KernelIoctrl object pointer
 */
void
kioctrlDestructEngine_IMPL
(
    KernelIoctrl *pKernelIoctrl
)
{
    return;
}
