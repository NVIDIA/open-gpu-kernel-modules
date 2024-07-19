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

#ifndef __tu102_dev_nv_xve3g_vf_h__
#define __tu102_dev_nv_xve3g_vf_h__

#define NV_XVE_VF_MSIX_CAP_HDR                                    0x0000007C /* RW-4R */
#define NV_XVE_VF_MSIX_CAP_HDR_ENABLE                                  31:31 /* RWIVF */
#define NV_XVE_VF_MSIX_CAP_HDR_ENABLE_ENABLED                     0x00000001 /* RW--V */
#define NV_XVE_VF_MSIX_CAP_HDR_ENABLE_DISABLED                    0x00000000 /* RWI-V */
#define NV_XVE_VF_MSIX_CAP_HDR_FUNCTION_MASK                           30:30 /* RWIVF */
#define NV_XVE_VF_MSIX_CAP_HDR_FUNCTION_MASK_MASKED               0x00000001 /* RW--V */
#define NV_XVE_VF_MSIX_CAP_HDR_FUNCTION_MASK_UNMASKED             0x00000000 /* RWI-V */
#define NV_XVE_VF_MSIX_CAP_TABLE                                    0x00000080 /* R--4R */
#define NV_XVE_VF_MSIX_CAP_PBA                                    0x00000084 /* R--4R */
#endif // __tu102_dev_nv_xve3g_vf_h__
