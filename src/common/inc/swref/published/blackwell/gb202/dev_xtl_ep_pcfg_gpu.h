/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb202_dev_xtl_ep_pcfg_gpu_h__
#define __gb202_dev_xtl_ep_pcfg_gpu_h__

#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_1                                                                        0x000002B4           /* R--4R */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2                                                                        0x000002B8           /* R--4R */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_IFF_POS                                                                31:25                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_IFF_POS_INIT                                                           0x00000000           /* R-C-V */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_IFF_POS_OVERRIDEABLE                                                   0                    /*       */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_BAR_FIREWALL_ENGAGE                                                    24:24                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_BAR_FIREWALL_ENGAGE_INIT                                               0x00000000           /* R-C-V */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_2_BAR_FIREWALL_ENGAGE_OVERRIDEABLE                                       0                    /*       */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2                                                                   0x00000084           /* R--4R */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_32BIT                                            7:7                  /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_32BIT_SUPPORTED                                  0x00000001           /* R-I-V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_32BIT_NOT_SUPPORTED                              0x00000000           /* R---V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_32BIT_OVERRIDEABLE                               1                    /*       */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_64BIT                                            8:8                  /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_64BIT_SUPPORTED                                  0x00000001           /* R-I-V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_64BIT_NOT_SUPPORTED                              0x00000000           /* R---V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_ATOMIC_COMPLETER_64BIT_OVERRIDEABLE                               1                    /*       */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_CAS_COMPLETER_128BIT                                              9:9                  /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_CAS_COMPLETER_128BIT_NOT_SUPPORTED                                0x00000000           /* R-I-V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_CAS_COMPLETER_128BIT_SUPPORTED                                    0x00000001           /* R---V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2_CAS_COMPLETER_128BIT_OVERRIDEABLE                                 1                    /*       */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2                                                                 0x00000088           /* RW-4R */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2_LTR_ENABLE                                                      10:10                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2_LTR_ENABLE_DEFAULT                                              0x00000000           /* RWI-V */


#endif // __gb202_dev_xtl_ep_pcfg_gpu_h__

