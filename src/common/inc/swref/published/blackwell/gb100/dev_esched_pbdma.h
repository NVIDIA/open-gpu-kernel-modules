/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gb100_dev_esched_pbdma_h__
#define __gb100_dev_esched_pbdma_h__
#define NV_PBDMA       0x000007ff:0x00000000 /* RW--D */
#define NV_PBDMA_GP_BASE                                     0x090 /* RW-4R */
#define NV_PBDMA_GP_BASE_RSVD                                  2:0 /* RWXUF */
#define NV_PBDMA_GP_BASE_RSVD_ZERO                      0x00000000 /* RW--V */
#define NV_PBDMA_GP_BASE_HI                                  0x094 /* RW-4R */
#define NV_PBDMA_GP_BASE_HI_OFFSET                            24:0 /* RWXUF */
#define NV_PBDMA_GP_BASE_HI_OFFSET_ZERO                 0x00000000 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE                            0x0b4 /* RW-4R */
#define NV_PBDMA_MISC_FETCH_STATE_GP_INFO_LIMIT2                      28:24 /* RWXUF */
#define NV_PBDMA_MISC_FETCH_STATE_GP_INFO_LIMIT2_ZERO            0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE                    0x05C /* RW-4R */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_VALUE               24:8 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_VALUE_ZERO    0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD               31:25 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD_ZERO     0x00000000 /* RW--V */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD2                7:0 /* RWXUF */
#define NV_PBDMA_PB_SEGMENT_EXTENDED_BASE_RSVD2_ZERO    0x00000000 /* RW--V */
#define NV_PBDMA_SIGNATURE                                   0x038 /* RW-4R */
#define NV_PBDMA_SIGNATURE_HW                                 15:0 /* RWXUF */
#define NV_PBDMA_SIGNATURE_HW_VALID                     0x0000face /* RW--V */
#define NV_PBDMA_SIGNATURE_HW_HOST_CLASS_ID                 51567 /* RW--V */
#define NV_PBDMA_SIGNATURE_SW                                31:16 /* RWXUF */
#define NV_PBDMA_SIGNATURE_SW_ZERO                      0x00000000 /* RW--V */
#define NV_PBDMA_PB_HEADER                                   0x420 /* RW-4R */
#define NV_PBDMA_PB_HEADER_METHOD_OR_SDMASK                   15:2 /* RWXUF */
#define NV_PBDMA_PB_HEADER_METHOD                             13:2 /*       */
#define NV_PBDMA_PB_HEADER_METHOD_ZERO                  0x00000000 /*       */
#define NV_PBDMA_PB_HEADER_SDMASK                             15:4 /*       */
#define NV_PBDMA_PB_HEADER_SUBCHANNEL                        18:16 /* RWXUF */
#define NV_PBDMA_PB_HEADER_SUBCHANNEL_ZERO              0x00000000 /* RW--V */
#define NV_PBDMA_PB_HEADER_LEVEL                             20:20 /* RWXVF */
#define NV_PBDMA_PB_HEADER_LEVEL_MAIN                   0x00000000 /* RW--V */
#define NV_PBDMA_PB_HEADER_LEVEL_SUBROUTINE             0x00000001 /* RW--V */
#define NV_PBDMA_PB_HEADER_FIRST                             22:22 /* RWXVF */
#define NV_PBDMA_PB_HEADER_FIRST_FALSE                  0x00000000 /* RW--V */
#define NV_PBDMA_PB_HEADER_FIRST_TRUE                   0x00000001 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE_PB_HEADER_TYPE                        5:3 /* RWXUF */
#define NV_PBDMA_MISC_FETCH_STATE_PB_HEADER_TYPE_INC             0x00000001 /* RW--V */
#define NV_PBDMA_HCE_CTRL                                    0x054 /* RW-4R */
#define NV_PBDMA_HCE_CTRL_HCE_PRIV_MODE                        5:5 /* RWXUF */
#define NV_PBDMA_HCE_CTRL_HCE_PRIV_MODE_NO              0x00000000 /* RW--V */
#define NV_PBDMA_HCE_CTRL_HCE_PRIV_MODE_YES             0x00000001 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE                          0x050 /* RW-4R */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENGINE              1:0 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID       2:2 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID_TRUE    1 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID_FALSE   0 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID        3:3 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID_TRUE     1 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID_FALSE    0 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL        13:13 /* R--VF */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL_NON_PRIVILEGED       0x00000000 /* R---V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL_PRIVILEGED           0x00000001 /* R---V */
#define NV_PBDMA_SET_CHANNEL_INFO_VEID                       13:8 /* RWXUF */
#define NV_PBDMA_SET_CHANNEL_INFO_CHID                       27:16 /* R--VF */
#define NV_PBDMA_SUBDEVICE_ID                                 11:0 /* RWXUF */
#define NV_PBDMA_SUBDEVICE_ID_ENABLE                    0x00000FFF /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_STATUS                    17:17 /* RWXUF */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_STATUS_INACTIVE      0x00000000 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_STATUS_ACTIVE        0x00000001 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_CHANNEL_DMA               18:18 /* RWXUF */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_CHANNEL_DMA_DISABLE  0x00000000 /* RW--V */
#define NV_PBDMA_MISC_FETCH_STATE_SUBDEVICE_CHANNEL_DMA_ENABLE   0x00000001 /* RW--V */
#endif // __gb100_dev_esched_pbdma_h__
