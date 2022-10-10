/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gh100_dev_vm_h__
#define __gh100_dev_vm_h__
#define NV_VIRTUAL_FUNCTION_PRIV                       0x0002FFFF:0x00000000 /* RW--D */
#define NV_VIRTUAL_FUNCTION                            0x0003FFFF:0x00030000 /* RW--D */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE                                    0x00000F10   /* R--4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_TOKEN                   (31-1):0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED                           0x00000F14   /* R--4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED_TOKEN         (31-1):0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED_STATUS                                   31:31   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE_COMPLETED_STATUS_BUSY                         0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE                                   0x00000F18   /* R--4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_TOKEN                  (31-1):0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED                         0x00000F1C   /* R--4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED_TOKEN        (31-1):0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED_STATUS                                  31:31   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_PEERMEM_INVALIDATE_COMPLETED_STATUS_BUSY                        0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR                                                0x00000F70   /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MODE                                                  9:9   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MODE_PHYSICAL                                  0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(i)                                                (i) /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1                                                   16 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1                                            16  /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1                                          16 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_TIMER(i)               (0x2300+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_TIMER__SIZE_1                      2  /*        */

#endif // __gh100_dev_vm_h__
