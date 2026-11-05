/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_nv_xve3g_vf_h__
#define __ga100_dev_nv_xve3g_vf_h__

/* Published subset of GA100 VF PCIe configuration-space definitions. */
#define NV_XVE_VF_PCIE_CAP0                                        0x00000040 /* R--4R */
#define NV_XVE_VF_PCIE_CAP0_LIST_CAPABILITY_ID                            7:0 /* C--VF */
#define NV_XVE_VF_PCIE_CAP0_LIST_CAPABILITY_ID_INIT                0x00000010 /* C---V */
#define NV_XVE_VF_PCIE_CAP0_LIST_NEXT_CAPABILITY_PTR                     15:8 /* C--VF */
#define NV_XVE_VF_PCIE_CAP3                                        0x0000004C /* R--4R */
#define NV_XVE_VF_PCIE_CAP3_MAX_LINK_SPEED                                3:0 /* R-XVF */
#define NV_XVE_VF_PCIE_CAP3_MAX_LINK_WIDTH                                9:4 /* R-XVF */

#endif // __ga100_dev_nv_xve3g_vf_h__
