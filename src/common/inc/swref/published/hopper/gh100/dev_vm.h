/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_LO(i)                              (0x00010000+(i)*16) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_ADDR_HI(i)                              (0x00010004+(i)*16) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_DATA(i)                                 (0x00010008+(i)*16) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL(i)                       (0x0001000C+(i)*16) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL__SIZE_1    9 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO               0x00003108 /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO_BASE               31:12 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI               0x0000310C /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_DOORBELL                                          0x2200 /* -W-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(i)                              (0x1000+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_VALUE                                     31:0 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_VALUE_INIT                          0x00000000 /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(i)                       (0x1200+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET_VALUE                              31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET_VALUE_INIT                   0x00000000 /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(i)                     (0x1400+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR_VALUE                            31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR_VALUE_INIT                 0x00000000 /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_PAGE_FAULT_CTRL                           0x00003070 /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR                                                0x00000F60   /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_MAP                                                 31:10   /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_PENDING                                          0:0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_PENDING_EMPTY                             0x00000000   /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_PENDING_BUSY                              0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_OUTSTANDING                                      1:1   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_OUTSTANDING_FALSE                         0x00000000   /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_BAR1_OUTSTANDING_TRUE                          0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_MODE                                                  9:9   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_MODE_PHYSICAL                                  0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_MODE_VIRTUAL                                   0x00000001   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET                                              11:10   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_VID_MEM                                 0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_SYS_MEM_COHERENT                        0x00000002   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_TARGET_SYS_MEM_NONCOHERENT                     0x00000003   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_PTR                                                 31:12   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_LOW_ADDR_PTR_0                                          0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_HIGH_ADDR                                               0x00000F64   /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_HIGH_ADDR_PTR                                                 31:0   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_HIGH_ADDR_PTR_0                                         0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR1_BLOCK_PTR_SHIFT                                                       12   /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR                                                0x00000F70   /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MAP                                                 31:10   /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_PENDING                                          0:0   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_PENDING_EMPTY                             0x00000000   /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_PENDING_BUSY                              0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_OUTSTANDING                                      1:1   /* R-IUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_OUTSTANDING_FALSE                         0x00000000   /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_BAR2_OUTSTANDING_TRUE                          0x00000001   /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MODE                                                  9:9   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MODE_PHYSICAL                                  0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_MODE_VIRTUAL                                   0x00000001   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_TARGET                                              11:10   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_TARGET_VID_MEM                                 0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_TARGET_SYS_MEM_COHERENT                        0x00000002   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_TARGET_SYS_MEM_NONCOHERENT                     0x00000003   /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_PTR                                                 31:12   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_LOW_ADDR_PTR_0                                          0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_HIGH_ADDR                                               0x00000F74   /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_HIGH_ADDR_PTR                 (52-33):0   /* RWIUF */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_HIGH_ADDR_PTR_0                                         0x00000000   /* RWI-V */
#define NV_VIRTUAL_FUNCTION_PRIV_FUNC_BAR2_BLOCK_PTR_SHIFT                                                       12   /*       */
#endif // __gh100_dev_vm_h__
