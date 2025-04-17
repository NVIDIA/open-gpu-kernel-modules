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

/*!
 * Provides GB10Y specific KernelGsp HAL implementations.
 */

#include "rmconfig.h"
#include "gpu/gsp/kernel_gsp.h"

/*!
 * Populate WPR meta structure. On GB10Y, this consists of determining the
 * minimum sizes of various regions that Client RM provides as inputs.
 * The actual offsets are determined by secure ACR ucode and patched
 * into the GspFwWprMeta structure provided to GSP-RM, so most of the
 * structure is 0-initialized here.
 *
 * For GB10Y, unlike on dGPU, this structure is in sysmem.
 *
 *   ---------------------------- <- WPR2 End (1M aligned)
 *   | (potential align. gap)   |
 *   ---------------------------- <- gspFwWprEnd (128K aligned)
 *   | BOOT BIN (e.g. GSP-FMC)  |
 *   ---------------------------- <- bootBinOffset
 *   | GSP FW ELF               |
 *   ---------------------------- <- gspFwOffset
 *   | GSP FW (WPR) HEAP        |
 *   ---------------------------- <- gspFwHeapOffset
 *   | ACR-placed metadata      |
 *   | (struct GspFwWprMeta)    |
 *   ---------------------------- <- gspFwWprStart (128K aligned)
 *   | WPR and LSB Header       |
 *   ---------------------------- <- WPR2 Start, gspFwRsvdStart
 *
 * @param       pGpu          GPU object pointer
 * @param       pKernelGsp    KernelGsp object pointer
 * @param       pGspFw        Pointer to GSP-RM fw image.
 * @param       pFbRegionInfo Pointer to fb region table to fill in.
 */
NV_STATUS
kgspPopulateWprMeta_GB10B
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    GspFwWprMeta *pWprMeta = pKernelGsp->pWprMeta;
    
    // Layout is close to GH100, so use that HAL first then override as necessary
    NV_ASSERT_OK_OR_RETURN(kgspPopulateWprMeta_GH100(pGpu, pKernelGsp, pGspFw));

    //
    // We send these to FSP as the size to reserve above FRTS.
    // The actual offsets get filled in by ACR ucode when it sets up WPR2.
    //
    pWprMeta->vgaWorkspaceSize = 0; // No VGA Workspace on iGPU
    
    pWprMeta->pmuReservedSize = 0; // No PMU Reserved memory in GB10B

    //
    // No Non-WPR heap is required for iGPU because sysmem heap is available to
    // GSP-RM immediately upon booting
    //
    pWprMeta->nonWprHeapSize = 0;

    return NV_OK;
}

