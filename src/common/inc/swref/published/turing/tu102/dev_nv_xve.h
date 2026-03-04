/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_nv_xve_h__
#define __tu102_dev_nv_xve_h__
#define NV_PCFG                                              0x00088FFF:0x00088000 /* RW--D */
#define NV_XVE_ID                                                       0x00000000 /* R--4R */
#define NV_XVE_ID_VENDOR                                                      15:0 /* C--VF */
#define NV_XVE_ID_VENDOR_NVIDIA                                         0x000010DE /* C---V */
#define NV_XVE_SW_RESET                                                 0x00000718 /* RW-4R */
#define NV_XVE_DEVICE_CAPABILITY                                        0x0000007C /* R--4R */
#define NV_XVE_DEVICE_CAPABILITY_FUNCTION_LEVEL_RESET                        28:28 /* R-XVF */
#define NV_XVE_DEVICE_CAPABILITY_FUNCTION_LEVEL_RESET_NOT_SUPPORTED     0x00000000 /* R---V */
#define NV_XVE_DEVICE_CAPABILITY_FUNCTION_LEVEL_RESET_SUPPORTED         0x00000001 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS                                    0x00000080 /* RW-4R */
#define NV_XVE_DEVICE_CONTROL_STATUS_INITIATE_FN_LVL_RST                     15:15 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_TRANSACTIONS_PENDING                    21:21 /* R-IVF */
#define NV_XVE_MSIX_CAP_HDR                                             0x000000C8 /* RW-4R */
#define NV_XVE_MSIX_CAP_HDR_ENABLE                                           31:31 /* RWIVF */
#define NV_XVE_MSIX_CAP_HDR_ENABLE_ENABLED                              0x00000001 /* RW--V */
#define NV_XVE_MSIX_CAP_HDR_ENABLE_DISABLED                             0x00000000 /* RWI-V */
#define NV_XVE_PRIV_MISC_1                                              0x0000041C /* RW-4R */
#define NV_XVE_PRIV_MISC_1_CYA_HIDE_MSIX_CAP                                 29:29 /* RWCVF */
#define NV_XVE_PRIV_MISC_1_CYA_HIDE_MSIX_CAP_TRUE                       0x00000001 /* RW--V */
#define NV_XVE_PRIV_MISC_1_CYA_HIDE_MSIX_CAP_FALSE                      0x00000000 /* RWC-V */
#define NV_XVE_SRIOV_CAP_HDR3                                           0x00000BD8 /* R--4R */
#define NV_XVE_SRIOV_CAP_HDR3_TOTAL_VFS                                      31:16 /* R-EVF */
#define NV_XVE_SRIOV_CAP_HDR5                                           0x00000BE0 /* R--4R */
#define NV_XVE_SRIOV_CAP_HDR5_FIRST_VF_OFFSET                                 15:0 /* R-IVF */
#define NV_XVE_SRIOV_CAP_HDR9                                           0x00000BF0 /* RW-4R */
#define NV_XVE_SRIOV_CAP_HDR10                                          0x00000BF4 /* RW-4R */
#define NV_XVE_SRIOV_CAP_HDR11_VF_BAR1_HI                               0x00000BF8 /* RW-4R */
#define NV_XVE_SRIOV_CAP_HDR12                                          0x00000BFC /* RW-4R */
#define NV_XVE_SRIOV_CAP_HDR13_VF_BAR2_HI                               0x00000C00 /* RW-4R */
#endif // __tu102_dev_nv_xve_h__
