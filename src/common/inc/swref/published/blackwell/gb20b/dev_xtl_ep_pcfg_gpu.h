/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb20b_dev_xtl_ep_pcfg_gpu_h__
#define __gb20b_dev_xtl_ep_pcfg_gpu_h__

#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC                                                                          0x000002B4           /* R--4R */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC__SAFETY                                                                  "parity"             /*       */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_ERROR                                                              15:0                 /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_ERROR_INIT                                                         0x00000000           /* R-C-V */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_ERROR_OVERRIDEABLE                                                 0                    /*       */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS                                                                  22:16                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS_INIT                                                             0x00000000           /* R-C-V */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS_OVERRIDEABLE                                                     0                    /*       */

#endif // __gb20b_dev_xtl_ep_pcfg_gpu_h__
