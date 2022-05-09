/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/mem_sys/kern_mem_sys.h"

/*!
 * @brief Get physical address of the FB memory on systems where GPU memory
 * is onlined to the OS
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 * @param[in] physAddr            Physical Address of FB memory 
 * @param[in] numaNodeId          NUMA node id where FB memory is added to the
 *                                kernel
 *
 * @return  NV_OK on success
 */
NV_STATUS
kmemsysGetFbNumaInfo_GV100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64              *physAddr,
    NvS32              *numaNodeId
)
{
    NV_STATUS     status;

    status = osGetFbNumaInfo(pGpu, physAddr, numaNodeId);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "NUMA FB Physical address: 0x%llx Node ID: 0x%x\n",
                  *physAddr, *numaNodeId);
    }

    return status;
}
