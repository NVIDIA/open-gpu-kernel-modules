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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "mmu/gmmu_fmt.h"

/*!
 *  PD3 [48:47]
 *  |
 *  v
 *  PD2 [46:38]
 *  |
 *  v
 *  PD1 [37:29] / PT_512M [37:29] (512MB page)
 *  |
 *  v
 *  PD0 [28:21] / PT_HUGE [28:21] (2MB page)
 *  |        \
 *  |         \
 *  v          v
 *  PT_SMALL  PT_BIG (64KB page)
 *  [20:12]   [20:16]
 */
void kgmmuFmtInitLevels_GA10X(KernelGmmu    *pKernelGmmu,
                              MMU_FMT_LEVEL *pLevels,
                              const NvU32    numLevels,
                              const NvU32    version,
                              const NvU32    bigPageShift)
{
    kgmmuFmtInitLevels_GP10X(pKernelGmmu, pLevels, numLevels, version, bigPageShift);

    // Page directory 1 can now hold a PTE pointing to a 512MB Page
    pLevels[2].bPageTable = NV_TRUE;
}
