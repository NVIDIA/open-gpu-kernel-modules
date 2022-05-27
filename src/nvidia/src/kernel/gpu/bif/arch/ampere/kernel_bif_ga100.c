/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------ Includes ---------------------------------------- */
#include "gpu/bif/kernel_bif.h"
#include "ampere/ga100/dev_nv_xve_addendum.h"


/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief Apply WAR for bug 3208922 - disable P2P on Ampere NB
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifApplyWARBug3208922_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    if (IsMobile(pGpu))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED,  NV_TRUE);
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_TRUE);
    }
}

/*!
 * @brief Check for RO enablement request in emulated config space.
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 passthroughEmulatedConfig = osPciReadDword(osPciInitHandle(gpuGetDomain(pGpu),
                                                                     gpuGetBus(pGpu),
                                                                     gpuGetDevice(pGpu),
                                                                     0, NULL, NULL),
                                                     NV_XVE_PASSTHROUGH_EMULATED_CONFIG);
    NvBool roEnabled = DRF_VAL(_XVE, _PASSTHROUGH_EMULATED_CONFIG, _RELAXED_ORDERING_ENABLE, passthroughEmulatedConfig);

    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE, roEnabled);
}