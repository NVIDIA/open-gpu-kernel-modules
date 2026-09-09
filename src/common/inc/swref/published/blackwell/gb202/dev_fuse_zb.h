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

#ifndef __gb202_dev_fuse_zb_h__
#define __gb202_dev_fuse_zb_h__
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION                                                       0x000041C0 /* RW-4R */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION__PRIV_LEVEL_MASK 0x000000FC /*       */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION_DATA                                                        14:0 /* RWIVF */
#define NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION_DATA_INIT                                             0x00000000 /* RWI-V */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION                                                      0x00004140 /* RW-4R */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION__PRIV_LEVEL_MASK 0x000000FC /*       */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION_DATA                                                       14:0 /* RWIVF */
#define NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION_DATA_INIT                                            0x00000000 /* RWI-V */

#endif // __gb202_dev_fuse_zb_h__
