/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_fuse_zb_h__
#define __gb100_dev_fuse_zb_h__
#define NV_FUSE_ZB_FEATURE_READOUT                                                               0x00003814     /* R--4R */
#define NV_FUSE_ZB_FEATURE_READOUT_ECC_DRAM                                                      16:16          /* R--VF */
#define NV_FUSE_ZB_FEATURE_READOUT_ECC_DRAM_DISABLED                                             0x00000000     /* R---V */
#define NV_FUSE_ZB_FEATURE_READOUT_ECC_DRAM_ENABLED                                              0x00000001     /* R---V */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION                                                       0x000041C0 /* RW-4R */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION__PRIV_LEVEL_MASK 0x000000FC /*       */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION_DATA_INIT                                             0x00000000 /* RWI-V */
#define NV_FUSE_ZB_STATUS_OPT_DISPLAY                                                                  0x00000C04 /* R-I4R */
#define NV_FUSE_ZB_STATUS_OPT_DISPLAY_DATA                                                                    0:0 /* R-IVF */
#define NV_FUSE_ZB_STATUS_OPT_DISPLAY_DATA_ENABLE                                                      0x00000000 /* R---V */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION                                                      0x00004140 /* RW-4R */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION__PRIV_LEVEL_MASK 0x000000FC /*       */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION_DATA                                                       15:0 /* RWIVF */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION_DATA_INIT                                            0x00000000 /* RWI-V */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS                                                         0x0000074C /* RW-4R */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS__PRIV_LEVEL_MASK 0x000000FC /*       */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS_DATA                                                           0:0 /* RWIVF */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS_DATA_INIT                                               0x00000001 /* RWI-V */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS_DATA_NO                                                 0x00000000 /* RW--V */
#define NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS_DATA_YES                                                0x00000001 /* RW--V */

#endif // __gb100_dev_fuse_zb_h__
