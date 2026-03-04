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

#ifndef __ga100_dev_vm_h__
#define __ga100_dev_vm_h__
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
#define NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_TRIGGER_VECTOR                               11:0 /* -W-VF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE                                   0x000030B0 /* RW-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_VA                                   0:0 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_VA_FALSE                      0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_VA_TRUE                       0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_PDB                                  1:1 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_PDB_FALSE                     0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ALL_PDB_TRUE                      0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_HUBTLB_ONLY                              2:2 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_HUBTLB_ONLY_FALSE                 0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_HUBTLB_ONLY_TRUE                  0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY                                   5:3 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_NONE                       0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_START                      0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_START_ACK_ALL              0x00000002 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_CANCEL_TARGETED            0x00000003 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_CANCEL_GLOBAL              0x00000004 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_REPLAY_CANCEL_VA_GLOBAL           0x00000005 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_SYS_MEMBAR                              6:6 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_SYS_MEMBAR_FALSE                 0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_SYS_MEMBAR_TRUE                  0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ACK                                     8:7 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ACK_NONE_REQUIRED                0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ACK_INTRANODE                    0x00000002 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_ACK_GLOBALLY                     0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CANCEL_CLIENT_ID                       14:9 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CANCEL_GPC_ID                         19:15 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_INVAL_SCOPE                           16:15 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_INVAL_SCOPE_ALL_TLBS             0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_INVAL_SCOPE_LINK_TLBS            0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_INVAL_SCOPE_NON_LINK_TLBS        0x00000002 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CANCEL_CLIENT_TYPE                    20:20 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CANCEL_CLIENT_TYPE_GPC           0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CANCEL_CLIENT_TYPE_HUB           0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_PASID                             21:21 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_PASID_FALSE                  0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_PASID_TRUE                   0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_SIZE                              22:22 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_SIZE_FALSE                   0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_USE_SIZE_TRUE                    0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PROP_FLUSH                            23:23 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PROP_FLUSH_FALSE                 0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PROP_FLUSH_TRUE                  0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL                           26:24 /* RWXVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_ALL                  0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_PTE_ONLY             0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE0           0x00000002 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE1           0x00000003 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE2           0x00000004 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE3           0x00000005 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE4           0x00000006 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_UP_TO_PDE5           0x00000007 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_READ          0x00000000 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_WRITE         0x00000001 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_ATOMIC_STRONG 0x00000002 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_RSVRVD        0x00000003 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_ATOMIC_WEAK   0x00000004 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_ATOMIC_ALL    0x00000005 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_WRITE_AND_ATOMIC 0x00000006 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_CACHE_LEVEL_CANCEL_ALL           0x00000007 /* RW--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_TRIGGER                                31:31 /* -WEVF */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_TRIGGER_FALSE                     0x00000000 /* -WE-V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_TRIGGER_TRUE                      0x00000001 /* -W--V */
#define NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_MAX_CACHELINE_SIZE                0x00000010 /*       */
#define NV_VIRTUAL_FUNCTION_PRIV_DOORBELL                                         0x2200 /* -W-4R */
#define NV_VIRTUAL_FUNCTION_TIME_0                                                0x30080 /* R--4R */
#define NV_VIRTUAL_FUNCTION_TIME_0_NSEC                                           31:5 /* R-XUF */
#define NV_VIRTUAL_FUNCTION_TIME_1                                                0x30084 /* R--4R */
#define NV_VIRTUAL_FUNCTION_TIME_1_NSEC                                           28:0 /* R-XUF */
#define NV_VIRTUAL_FUNCTION_DOORBELL                                              0x30090 /* -W-4R */
#define NV_VIRTUAL_FUNCTION_PRIV_MAILBOX_SCRATCH(i)                               (0x2100+(i)*4) /* RW-4A */
#endif // __ga100_dev_vm_h__
