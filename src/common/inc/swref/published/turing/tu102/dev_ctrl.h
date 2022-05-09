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

#ifndef __tu102_dev_ctrl_h__
#define __tu102_dev_ctrl_h__
#define NV_CTRL_CPU_INTR_TOP(i)             (0x00B73400+(i)*4) /* R--4A */
#define NV_CTRL_CPU_INTR_TOP__SIZE_1 64 /*       */
#define NV_CTRL_CPU_INTR_TOP_VALUE                        31:0 /* R--VF */
#define NV_CTRL_CPU_INTR_TOP_EN_SET(i)               (0x00B73800+(i)*4) /* RW-4A */
#define NV_CTRL_CPU_INTR_TOP_EN_SET__SIZE_1   64 /*       */
#define NV_CTRL_CPU_INTR_TOP_EN_SET_VALUE                          31:0 /* RWIVF */
#define NV_CTRL_CPU_INTR_TOP_EN_SET_VALUE_INIT               0x00000000 /* R-I-V */
#define NV_CTRL_CPU_INTR_TOP_EN_CLEAR(i)             (0x00B73C00+(i)*4) /* RW-4A */
#define NV_CTRL_CPU_INTR_TOP_EN_CLEAR__SIZE_1 64 /*       */
#define NV_CTRL_CPU_INTR_TOP_EN_CLEAR_VALUE                        31:0 /* RWIVF */
#define NV_CTRL_CPU_INTR_TOP_EN_CLEAR_VALUE_INIT             0x00000000 /* R-I-V */
#define NV_CTRL_CPU_INTR_LEAF(i)                                         (0x00B74000+(i)*4) /* RW-4A */
#define NV_CTRL_CPU_INTR_LEAF__SIZE_1    1024 /*       */
#define NV_CTRL_CPU_INTR_LEAF_VALUE                                                    31:0 /* RWIVF */
#define NV_CTRL_CPU_INTR_LEAF_VALUE_INIT                                         0x00000000 /* R-I-V */
#define NV_CTRL_CPU_INTR_LEAF_ARRAY_SIZE_PER_FN  16 /*       */
#define NV_CTRL_CPU_INTR_LEAF_EN_SET(i)                                           (0x00B78000+(i)*4) /* RW-4A */
#define NV_CTRL_CPU_INTR_LEAF_EN_SET__SIZE_1      1024 /*       */
#define NV_CTRL_CPU_INTR_LEAF_EN_SET_VALUE                                                      31:0 /* RWIVF */
#define NV_CTRL_CPU_INTR_LEAF_EN_SET_VALUE_INIT                                           0x00000000 /* R-I-V */
#define NV_CTRL_CPU_INTR_LEAF_EN_CLEAR(i)                                         (0x00B7C000+(i)*4) /* RW-4A */
#define NV_CTRL_CPU_INTR_LEAF_EN_CLEAR__SIZE_1    1024 /*       */
#define NV_CTRL_CPU_INTR_LEAF_EN_CLEAR_VALUE                                                    31:0 /* RWIVF */
#define NV_CTRL_CPU_INTR_LEAF_EN_CLEAR_VALUE_INIT                                         0x00000000 /* R-I-V */
#define NV_CTRL_LEGACY_ENGINE_STALL_INTR_BASE_VECTORID         0xB66880 /* C--4R */
#define NV_CTRL_LEGACY_ENGINE_STALL_INTR_BASE_VECTORID_VECTOR      11:0 /* C--UF */
#define NV_CTRL_LEGACY_ENGINE_STALL_INTR_BASE_VECTORID_VECTOR_INIT   192 /* C---V */
#define NV_CTRL_LEGACY_ENGINE_NONSTALL_INTR_BASE_VECTORID       0xB66884 /* C--4R */
#define NV_CTRL_LEGACY_ENGINE_NONSTALL_INTR_BASE_VECTORID_VECTOR    11:0 /* C--UF */
#define NV_CTRL_LEGACY_ENGINE_NONSTALL_INTR_BASE_VECTORID_VECTOR_INIT   0 /* C---V */
#define NV_CTRL_VIRTUAL_INTR_LEAF(i)              (0x00B66800+(i)*4) /* RW-4A */
#define NV_CTRL_VIRTUAL_INTR_LEAF__SIZE_1                          2 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_PENDING                       31:0 /* RWIVF */
#define NV_CTRL_VIRTUAL_INTR_LEAF_PENDING_INIT                     0 /* RWI-V */
#define NV_CTRL_VIRTUAL_INTR_LEAF_PENDING_INTR                     1 /* R---V */
#define NV_CTRL_VIRTUAL_INTR_LEAF_PENDING_CLEAR                    1 /* -W--V */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET(i)           (0x00B66820+(i)*4) /* RW-4A */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET__SIZE_1                       2 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VALUE                      31:0 /* RWIVF */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VALUE_INIT                    0 /* RWI-V */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VECTOR(i)                   (i) /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VECTOR_ENABLE                 1 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VECTOR_ENABLED                1 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_SET_VECTOR_DISABLED               0 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR(i)         (0x00B66840+(i)*4) /* RW-4A */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR__SIZE_1                     2 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VALUE                    31:0 /* RWIVF */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VALUE_INIT                  0 /* RWI-V */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VECTOR(i)                 (i) /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VECTOR_DISABLE              1 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VECTOR_ENABLED              1 /*       */
#define NV_CTRL_VIRTUAL_INTR_LEAF_EN_CLEAR_VECTOR_DISABLED             0 /*       */
#define NV_CTRL_CPU_INTR_LEAF_TRIGGER(i)                   (0x00B66C00+(i)*4) /* -W-4A */
#define NV_CTRL_CPU_INTR_LEAF_TRIGGER__SIZE_1       64 /*       */
#define NV_CTRL_CPU_INTR_LEAF_TRIGGER_VECTOR                             11:0 /* -WXVF */
#define NV_CTRL_CPU_DOORBELL_VECTORID                                      0x00B6687C /* C--4R */
#define NV_CTRL_CPU_DOORBELL_VECTORID_VALUE                                      11:0 /* C--VF */
#define NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT 129 /* C---V */
#define NV_CTRL_VF_DOORBELL_VECTOR                                  11:0 /* -WXUF */
#define NV_CTRL_VF_DOORBELL_RUNLIST_ID                             22:16 /* -WXUF */
#endif // __tu102_dev_ctrl_h__
