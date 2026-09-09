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

#ifndef __gb202_dev_esched_pbdma_h__
#define __gb202_dev_esched_pbdma_h__
#define NV_PBDMA       0x000007ff:0x00000000 /* RW--D */
#define NV_PBDMA_MISC_EXECUTE_STATE                                             0x050 /* RW-4R */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENGINE                                 0:0 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENGINE_ENG0_LOCAL                        0 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENGINE_ENG1_LOCAL                        1 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID                          1:1 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID_TRUE                       1 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_ENG_CTX_VALID_FALSE                      0 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID                           2:2 /* RWXUF */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID_TRUE                        1 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_TARGET_CE_CTX_VALID_FALSE                       0 /* RW--V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL                           12:12 /* R--VF */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL_NON_PRIVILEGED       0x00000000 /* R---V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_AUTH_LEVEL_PRIVILEGED           0x00000001 /* R---V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_EVICT_LAST_COPY                      16:16 /* R--VF */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_EVICT_LAST_COPY_FALSE           0x00000000 /* R---V */
#define NV_PBDMA_MISC_EXECUTE_STATE_CONFIG_EVICT_LAST_COPY_TRUE            0x00000001 /* R---V */
#endif // __gb202_dev_esched_pbdma_h__

