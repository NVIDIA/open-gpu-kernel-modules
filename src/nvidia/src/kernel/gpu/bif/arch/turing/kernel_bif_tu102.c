/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------- System Includes -------------------------------- */
#include "gpu/bif/kernel_bif.h"
#include "gpu/gpu.h"

#include "published/turing/tu102/dev_nv_xve.h"
#include "published/turing/tu102/dev_vm.h"


/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief Check if MSIX is enabled in HW
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  True if MSIX enabled else False
 */
NvBool
kbifIsMSIXEnabledInHW_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    if (IS_VIRTUAL(pGpu))
    {
        // SR-IOV guests only support MSI-X
        return IS_VIRTUAL_WITH_SRIOV(pGpu);
    }
    else
    {
        if (GPU_BUS_CFG_RD32(pGpu, NV_XVE_MSIX_CAP_HDR, &data32) != NV_OK)
        {
            NV_ASSERT_FAILED("Unable to read NV_XVE_MSIX_CAP_HDR\n");
            return NV_FALSE;
        }
        return FLD_TEST_DRF(_XVE, _MSIX_CAP_HDR, _ENABLE, _ENABLED, data32);
    }
}

/*!
 * @brief Disables P2P reads/writes on VF
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifDisableP2PTransactions_TU102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED, NV_TRUE);
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_TRUE);
    }
}

