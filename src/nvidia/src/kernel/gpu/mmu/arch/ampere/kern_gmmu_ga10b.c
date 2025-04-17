/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"

#include "published/ampere/ga10b/dev_ram.h"

/*!
* @brief Get the MAGIC Value properties for the instance block write
*
* @param[in] pKernelGmmu     The KernelGmmu
* @param[in] pInstBlkDesc    Memory descriptor for the instance block of the engine
* @param[out] pOffset        Pointer to offset of NV_RAMIN_ENGINE_CTXSW_FW_MAGIC_VALUE
* @param[out] pData          Pointer to value to write
*
* @returns NV_STATUS
*/
NV_STATUS
kgmmuInstBlkMagicValueGet_GA10B
(
    KernelGmmu          *pKernelGmmu,
    NvU32               *pOffset,
    NvU32               *pData
)
{
    *pOffset = SF_OFFSET(NV_RAMIN_ENGINE_CTXSW_FW_MAGIC_VALUE);
    *pData   = NV_RAMIN_ENGINE_CTXSW_FW_MAGIC_VALUE_V;

    return NV_OK;
}
