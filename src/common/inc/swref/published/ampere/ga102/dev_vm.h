/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga102_dev_vm_h__
#define __ga102_dev_vm_h__
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP(i)                                             (0x1600+(i)*4) /* R--4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP__SIZE_1                                                     1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_VALUE                                                    31:0 /* R--VF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(i)                                                (i) /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE__SIZE_1                                            64 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE_INTR_PENDING                                        1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE_INTR_NOT_PENDING                                    0 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(i)                                      (0x1608+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1                                              1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_VALUE                                             31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE(i)                                         (i) /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE__SIZE_1                                     64 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE_ENABLE                                       1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE_ENABLED                                      1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE_DISABLED                                     0 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR(i)                                    (0x1610+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR__SIZE_1                                            1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_VALUE                                           31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE(i)                                       (i) /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE__SIZE_1                                   64 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE_DISABLE                                    1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE_ENABLED                                    1 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE_DISABLED                                   0 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(i)                                            (0x1000+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1                                                    8 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_VALUE                                                   31:0 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_VALUE_INIT                                        0x00000000 /* R---V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(i)                                     (0x1200+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1                                             8 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET_VALUE                                            31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET_VALUE_INIT                                 0x00000000 /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(i)                                   (0x1400+(i)*4) /* RW-4A */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1                                           8 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR_VALUE                                          31:0 /* RWIVF */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR_VALUE_INIT                               0x00000000 /* R-I-V */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_TRIGGER                                0x00001640 /* -W-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_TRIGGER_VECTOR                               11:0 /* -WXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_DOORBELL                                          0x2200 /* -W-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(i)                        (0x2100+(i)*4) /* RW-4A */
#endif // __ga102_dev_vm_h__
