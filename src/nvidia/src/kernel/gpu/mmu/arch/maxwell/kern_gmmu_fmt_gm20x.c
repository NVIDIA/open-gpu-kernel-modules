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

#if !defined(SRT_BUILD)
#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED
#include "gpu/mmu/kern_gmmu.h"
#endif
#include "mmu/gmmu_fmt.h"

#ifndef NV_MMU_PTE_COMPTAG_USABLE
#define NV_MMU_PTE_COMPTAG_USABLE    (1*32+27):(1*32+12) /* RWXVF */
#endif
#ifndef NV_MMU_PTE_COMPTAG_SUB_INDEX
#define NV_MMU_PTE_COMPTAG_SUB_INDEX (1*32+28):(1*32+28) /* RWXVF */
#endif

void kgmmuFmtInitCaps_GM20X(KernelGmmu *pKernelGmmu,
                            GMMU_FMT *pFmt)
{
    pFmt->bSparseHwSupport = NV_TRUE;
}

void kgmmuFmtInitPteComptagLine_GM20X(KernelGmmu *pKernelGmmu,
                                      GMMU_FMT_PTE *pPte,
                                      const NvU32 version)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_1);
    INIT_FIELD_DESC32(&pPte->fldCompTagLine, NV_MMU_PTE_COMPTAG_USABLE);
    INIT_FIELD_DESC32(&pPte->fldCompTagSubIndex, NV_MMU_PTE_COMPTAG_SUB_INDEX);
}
