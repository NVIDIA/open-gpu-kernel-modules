/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Blackwell specific Descriptor List management functions              *
*                                                                           *
\***************************************************************************/

#include "gpu/gpu.h"
#include "published/blackwell/gb20c/dev_disp_misc.h"


 /* @brief This function returns if SDM mode is enabled on iGPU
 *
 * @param[in]  pGpu   OBJGPU pointer
 *
 * @returns           The function returns if device is in SDM mode
 */

NvBool
gpuIsSocSdmEnabled_GB20C(OBJGPU *pGpu)
{
    NvU32 data32=0U;

    data32 = GPU_REG_RD32(pGpu, NV_PDISP_MISC_SDM_RESOURCE);
    return FLD_TEST_DRF(_PDISP_MISC, _SDM_RESOURCE, _NVDISPLAY, _SDM, data32)? NV_TRUE: NV_FALSE;
}
