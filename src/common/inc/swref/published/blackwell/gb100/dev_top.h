/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_top_h__
#define __gb100_dev_top_h__

#define NV_PTOP_DEVICE_INFO_CFG                                                0x000224fc /* RW-4R */
#define NV_PTOP_DEVICE_INFO_CFG_VERSION                                               3:0 /* RWDUF */
#define NV_PTOP_DEVICE_INFO_CFG_VERSION_INIT                                          0x2 /* RWD-V */
#define NV_PTOP_DEVICE_INFO2(i)                                        (0x00022800+(i)*4) /* RW-4A */
#define NV_PTOP_DEVICE_INFO2__SIZE_1                                                  353 /*       */
#define NV_PTOP_DEVICE_INFO2_ROW_VALUE                                               31:0 /* RWDVF */
#define NV_PTOP_DEVICE_INFO2_ROW_VALUE_INVALID                                 0x00000000 /* R-D-V */
#define NV_PTOP_DEVICE_INFO2_ROW_CHAIN                                              31:31 /*       */
#define NV_PTOP_DEVICE_INFO2_ROW_CHAIN_MORE                                           0x1 /*       */
#define NV_PTOP_DEVICE_INFO2_ROW_CHAIN_LAST                                           0x0 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_FAULT_ID                                            10:0 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_FAULT_ID_INVALID                                   0x000 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_GROUP_ID                                           15:11 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_INSTANCE_ID                                        23:16 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_TYPE_ENUM                                          30:24 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_TYPE_ENUM_LCE                                       0x13 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_TYPE_ENUM_HSHUB                                     0x18 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_RESET_ID                                           39:32 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_RESET_ID_INVALID                                    0x00 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_DEVICE_PRI_BASE                                    57:40 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_IS_ENGINE                                          62:62 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_IS_ENGINE_TRUE                                       0x1 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_IS_ENGINE_FALSE                                      0x0 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_RLENG_ID                                           65:64 /*       */
#define NV_PTOP_DEVICE_INFO2_DEV_RUNLIST_PRI_BASE                                   89:74 /*       */
#define NV_PTOP0_DEVICE_INFO_CFG                                               0x000224fc /* RW-4R */
#define NV_PTOP0_DEVICE_INFO_CFG_VERSION                                              3:0 /* RWDUF */
#define NV_PTOP0_DEVICE_INFO_CFG_VERSION_INIT                                         0x2 /* RWD-V */
#define NV_PTOP0_DEVICE_INFO2(i)                                       (0x00022800+(i)*4) /* RW-4A */
#define NV_PTOP0_DEVICE_INFO2__SIZE_1                                                 353 /*       */
#define NV_PTOP0_DEVICE_INFO2_ROW_VALUE                                              31:0 /* RWDVF */
#define NV_PTOP0_DEVICE_INFO2_ROW_VALUE_INVALID                                0x00000000 /* R-D-V */
#define NV_PTOP0_DEVICE_INFO2_ROW_CHAIN                                             31:31 /*       */
#define NV_PTOP0_DEVICE_INFO2_ROW_CHAIN_MORE                                          0x1 /*       */
#define NV_PTOP0_DEVICE_INFO2_ROW_CHAIN_LAST                                          0x0 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_FAULT_ID                                           10:0 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_FAULT_ID_INVALID                                  0x000 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_GROUP_ID                                          15:11 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_INSTANCE_ID                                       23:16 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_TYPE_ENUM                                         30:24 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_TYPE_ENUM_LCE                                      0x13 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_TYPE_ENUM_HSHUB                                    0x18 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_RESET_ID                                          39:32 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_RESET_ID_INVALID                                   0x00 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_DEVICE_PRI_BASE                                   57:40 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_IS_ENGINE                                         62:62 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_IS_ENGINE_TRUE                                      0x1 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_IS_ENGINE_FALSE                                     0x0 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_RLENG_ID                                          65:64 /*       */
#define NV_PTOP0_DEVICE_INFO2_DEV_RUNLIST_PRI_BASE                                  89:74 /*       */
#define NV_PTOP1_DEVICE_INFO_CFG                                               0x000324fc /* RW-4R */
#define NV_PTOP1_DEVICE_INFO_CFG_VERSION                                              3:0 /* RWDUF */
#define NV_PTOP1_DEVICE_INFO_CFG_VERSION_INIT                                         0x2 /* RWD-V */
#define NV_PTOP1_DEVICE_INFO2(i)                                       (0x00032800+(i)*4) /* RW-4A */
#define NV_PTOP1_DEVICE_INFO2__SIZE_1                                                 353 /*       */
#define NV_PTOP1_DEVICE_INFO2_ROW_VALUE                                              31:0 /* RWDVF */
#define NV_PTOP1_DEVICE_INFO2_ROW_VALUE_INVALID                                0x00000000 /* R-D-V */
#define NV_PTOP1_DEVICE_INFO2_ROW_CHAIN                                             31:31 /*       */
#define NV_PTOP1_DEVICE_INFO2_ROW_CHAIN_MORE                                          0x1 /*       */
#define NV_PTOP1_DEVICE_INFO2_ROW_CHAIN_LAST                                          0x0 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_FAULT_ID                                           10:0 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_FAULT_ID_INVALID                                  0x000 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_GROUP_ID                                          15:11 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_INSTANCE_ID                                       23:16 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_TYPE_ENUM                                         30:24 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_TYPE_ENUM_LCE                                      0x13 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_TYPE_ENUM_HSHUB                                    0x18 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_RESET_ID                                          39:32 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_RESET_ID_INVALID                                   0x00 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_DEVICE_PRI_BASE                                   57:40 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_IS_ENGINE                                         62:62 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_IS_ENGINE_TRUE                                      0x1 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_IS_ENGINE_FALSE                                     0x0 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_RLENG_ID                                          65:64 /*       */
#define NV_PTOP1_DEVICE_INFO2_DEV_RUNLIST_PRI_BASE                                  89:74 /*       */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_DEVICES                                          15:4 /* RWXUF */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_DEVICES_INIT                                    0x099 /* RWD-V */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_ROWS_PER_DEVICE                                 19:16 /* RWXUF */
#define NV_PTOP_DEVICE_INFO_CFG_MAX_ROWS_PER_DEVICE_INIT                              0x3 /* RW--V */
#define NV_PTOP_DEVICE_INFO_CFG_NUM_ROWS                                            31:20 /* RWXUF */
#define NV_PTOP_DEVICE_INFO_CFG_NUM_ROWS_INIT                                       0x161 /* RWD-V */

#endif // __gb100_dev_top_h__
