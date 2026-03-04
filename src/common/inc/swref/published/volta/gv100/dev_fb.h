/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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


#ifndef __gv100_dev_fb_h__
#define __gv100_dev_fb_h__
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG                                      0x00100A00 /* RW-4R */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_THRESHOLD                                  15:0 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_GRANULARITY                          17:16 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_GRANULARITY_64K                 0x00000000 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_GRANULARITY_2M                  0x00000001 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_GRANULARITY_16M                 0x00000002 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_GRANULARITY_16G                 0x00000003 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_GRANULARITY                          19:18 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_GRANULARITY_64K                 0x00000000 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_GRANULARITY_2M                  0x00000001 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_GRANULARITY_16M                 0x00000002 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_GRANULARITY_16G                 0x00000003 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_USE_LIMIT                            29:28 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_USE_LIMIT_NONE                  0x00000000 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_USE_LIMIT_QTR                   0x00000001 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_USE_LIMIT_HALF                  0x00000002 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MIMC_USE_LIMIT_FULL                  0x00000003 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_USE_LIMIT                            31:30 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_USE_LIMIT_NONE                  0x00000000 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_USE_LIMIT_QTR                   0x00000001 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_USE_LIMIT_HALF                  0x00000002 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_CONFIG_MOMC_USE_LIMIT_FULL                  0x00000003 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_LO               0x00100A04 /* RW-4R */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_LO_EN                   0:0 /* RWIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_LO_EN_FALSE      0x00000000 /* RWI-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_LO_EN_TRUE       0x00000001 /* RW--V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO                               0x00100A18 /* R--4R */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL                                 0:0 /* R-IVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL_FALSE                           0x0 /* R-I-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL_TRUE                            0x1 /* R---V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_PUSHED                               1:1 /* R-IVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_PUSHED_FALSE                         0x0 /* R---V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_PUSHED_TRUE                          0x1 /* R-I-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_WRITE_NACK                         24:24 /* R-IVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_WRITE_NACK_FALSE                     0x0 /* R-I-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_WRITE_NACK_TRUE                      0x1 /* R---V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR                                0x00100A1C /* -W-4R */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MIMC                                  0:0 /* -WIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MIMC_INIT                             0x0 /* -WI-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MIMC_CLR                              0x1 /* -W--V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MOMC                                  1:1 /* -WIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MOMC_INIT                             0x0 /* -WI-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_MOMC_CLR                              0x1 /* -W--V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_ALL_COUNTERS                          2:2 /* -WIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_ALL_COUNTERS_INIT                     0x0 /* -WI-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_ALL_COUNTERS_CLR                      0x1 /* -W--V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_WRITE_NACK                          31:31 /* -WIVF */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_WRITE_NACK_INIT                       0x0 /* -WI-V */
#define NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_CLR_WRITE_NACK_CLR                        0x1 /* -W--V */
#define NV_PFB_PRI_MMU_NON_REPLAY_FAULT_BUFFER               0
#define NV_PFB_PRI_MMU_REPLAY_FAULT_BUFFER                   1
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET(i)                         (0x00100E2C+(i)*20) /* RW-4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET__SIZE_1                     2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_PTR                              19:0 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_PTR_RESET                  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED                30:30 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_NO        0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_YES       0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_CLEAR     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW                        31:31 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_NO                0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_YES               0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_CLEAR             0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT(i)                         (0x00100E30+(i)*20) /* R--4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT__SIZE_1                    2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_PTR                               19:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_PTR_RESET                   0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED                 30:30 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED_NO         0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED_YES        0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW                         31:31 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW_NO                 0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW_YES                0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_ADDR                                      31:12 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_ADDR_HI_ADDR                                       31:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ENGINE_ID                                   8:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE                                  11:10 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ADDR                                      31:12 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_HI_ADDR                                       31:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_FAULT_TYPE                                     4:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_FAULT_TYPE_RESET                        0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_REPLAYABLE_FAULT                               7:7 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_REPLAYABLE_FAULT_RESET                  0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_CLIENT                                        14:8 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_CLIENT_RESET                            0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE                                  19:16 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_READ                        0x00000000 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_WRITE                       0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_ATOMIC                      0x00000002 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_PREFETCH                    0x00000003 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_READ                   0x00000000 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_WRITE                  0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_ATOMIC                 0x00000002 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_ATOMIC_STRONG          0x00000002 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_PREFETCH               0x00000003 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_VIRT_ATOMIC_WEAK            0x00000004 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_PHYS_READ                   0x00000008 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_PHYS_WRITE                  0x00000009 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_PHYS_ATOMIC                 0x0000000a /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_PHYS_PREFETCH               0x0000000b /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INFO_ACCESS_TYPE_RESET                       0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_CLIENT_TYPE                                  20:20 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_CLIENT_TYPE_RESET                       0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_GPC_ID                                       28:24 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_GPC_ID_RESET                            0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_PROTECTED_MODE                               29:29 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_PROTECTED_MODE_RESET                    0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_REPLAYABLE_FAULT_EN                          30:30 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_REPLAYABLE_FAULT_EN_RESET               0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INFO_VALID                                        31:31 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INFO_VALID_RESET                             0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS                                           0x00100E60 /* RW-4R */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS                                0:0 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_RESET                   0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_CLEAR                   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_PHYS_SET                     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT                                1:1 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_RESET                   0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_CLEAR                   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR1_VIRT_SET                     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS                                2:2 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_RESET                   0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_CLEAR                   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_PHYS_SET                     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT                                3:3 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_RESET                   0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_CLEAR                   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_BAR2_VIRT_SET                     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS                                 4:4 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_RESET                    0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_CLEAR                    0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_PHYS_SET                      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT                                 5:5 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_RESET                    0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_CLEAR                    0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_IFB_VIRT_SET                      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS                               6:6 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_RESET                  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_CLEAR                  0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_PHYS_SET                    0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT                               7:7 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_RESET                  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_CLEAR                  0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_DROPPED_OTHER_VIRT_SET                    0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE                                       8:8 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_RESET                          0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_SET                            0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE                                   9:9 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_RESET                      0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_SET                        0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_ERROR                               10:10 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_ERROR_RESET                    0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_ERROR_SET                      0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_ERROR                           11:11 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_ERROR_RESET                0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_ERROR_SET                  0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_OVERFLOW                            12:12 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_OVERFLOW_RESET                 0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_OVERFLOW_SET                   0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_OVERFLOW                        13:13 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_OVERFLOW_RESET             0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_OVERFLOW_SET               0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_GETPTR_CORRUPTED                    14:14 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_GETPTR_CORRUPTED_RESET         0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_REPLAYABLE_GETPTR_CORRUPTED_SET           0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_GETPTR_CORRUPTED                15:15 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_GETPTR_CORRUPTED_RESET     0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_NON_REPLAYABLE_GETPTR_CORRUPTED_SET       0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_BUSY                                           30:30 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_BUSY_FALSE                                0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_BUSY_TRUE                                 0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_VALID                                          31:31 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_STATUS_VALID_RESET                               0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_VALID_CLEAR                               0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_STATUS_VALID_SET                                 0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_VAL                          19:0 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_VAL_RESET              0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR               29:29 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR_DISABLE  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR_ENABLE   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT                 30:30 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT_NO         0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT_YES        0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE                      31:31 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_FALSE           0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_TRUE            0x00000001 /* RW--V */
#endif // __gv100_dev_fb_h__
