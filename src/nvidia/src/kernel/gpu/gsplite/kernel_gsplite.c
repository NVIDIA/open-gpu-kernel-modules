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

/****************************************************************************
*
*   Kernel GSPLITE module
*
****************************************************************************/

#include "kernel/gpu/gsplite/kernel_gsplite.h"
#include "kernel/gpu/mem_mgr/rm_page_size.h"
#include "kernel/os/os.h"
#include "nvrm_registry.h"

// Setup registry based overrides
static void _kgspliteInitRegistryOverrides(OBJGPU *, KernelGsplite *);

NV_STATUS
kgspliteConstructEngine_IMPL(OBJGPU *pGpu, KernelGsplite *pKernelGsplite, ENGDESCRIPTOR engDesc)
{
    // Initialize PublicId
    pKernelGsplite->PublicId = GET_KERNEL_GSPLITE_IDX(engDesc);

    {
        pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_TRUE);
    }

    // Initialize based on registry keys
    _kgspliteInitRegistryOverrides(pGpu, pKernelGsplite);

    return NV_OK;
}

NV_STATUS
kgspliteStateInitUnlocked_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{

    return NV_OK;
}

void kgspliteDestruct_IMPL
(
    KernelGsplite *pKernelGsplite
)
{
}

/*!
 * Initialize all registry overrides for this object
 */
static void
_kgspliteInitRegistryOverrides
(
    OBJGPU         *pGpu,
    KernelGsplite  *pKernelGsplite
)
{
    NvU32  data;

    /* Temporarily use regkey to determine if KernelGsplite should be present or destroyed */
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSPLITE_ENABLE_MASK, &data) == NV_OK)
    {
        if ((data & NVBIT(pKernelGsplite->PublicId)) != 0)
        {
            NV_PRINTF(LEVEL_INFO, "KernelGsplite%d enabled due to regkey override.\n", pKernelGsplite->PublicId);

            pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_FALSE);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "KernelGsplite%d missing due to lack of regkey override.\n", pKernelGsplite->PublicId);

            pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_TRUE);
        }
    }

    NV_PRINTF(LEVEL_INFO, "KernelGsplite%d: %s\n", pKernelGsplite->PublicId,
              pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING) ? "disabled" : "enabled");

    return;
}


