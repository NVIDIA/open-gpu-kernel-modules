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

#ifndef __gb20b_dev_mmu_h__
#define __gb20b_dev_mmu_h__
#define NV_MMU_PTE_KIND_INVALID                       0x07 /* R---V */
#define NV_MMU_PTE_KIND_PITCH                         0x00 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY                                                  0x6 /* R---V */
#define NV_MMU_PTE_KIND_Z16                                                             0x1 /* R---V */
#define NV_MMU_PTE_KIND_S8                                                              0x2 /* R---V */
#define NV_MMU_PTE_KIND_S8Z24                                                           0x3 /* R---V */
#define NV_MMU_PTE_KIND_ZF32_X24S8                                                      0x4 /* R---V */
#define NV_MMU_PTE_KIND_Z24S8                                                           0x5 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE                                     0x8 /* R---V */
#define NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC                         0x9 /* R---V */
#define NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC                                     0xA /* R---V */
#define NV_MMU_PTE_KIND_Z16_COMPRESSIBLE_DISABLE_PLC                                    0xB /* R---V */
#define NV_MMU_PTE_KIND_S8Z24_COMPRESSIBLE_DISABLE_PLC                                  0xC /* R---V */
#define NV_MMU_PTE_KIND_ZF32_X24S8_COMPRESSIBLE_DISABLE_PLC                             0xD /* R---V */
#define NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC                                  0xE /* R---V */
#define NV_MMU_PTE_KIND_SMSKED_MESSAGE                                                  0xF /* R---V */
#define NV_MMU_CLIENT_KIND                                    2:0 /* RWXVF */
#define NV_MMU_CLIENT_KIND_Z16                                0x1 /* R---V */
#define NV_MMU_CLIENT_KIND_S8                                 0x2 /* R---V */
#define NV_MMU_CLIENT_KIND_S8Z24                              0x3 /* R---V */
#define NV_MMU_CLIENT_KIND_ZF32_X24S8                         0x4 /* R---V */
#define NV_MMU_CLIENT_KIND_Z24S8                              0x5 /* R---V */
#define NV_MMU_CLIENT_KIND_GENERIC_MEMORY                     0x6 /* R---V */
#define NV_MMU_CLIENT_KIND_INVALID                            0x7 /* R---V */
#endif // __gb20b_dev_mmu_h__
