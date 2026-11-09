/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_ctxsw_prog_h__
#define __tu102_dev_ctxsw_prog_h__

#define NV_CTXSW_MAIN_IMAGE_POOL_PTR                                           0x00000000 /*       */
#define NV_CTXSW_MAIN_IMAGE_SIZE                                               0x00000004 /*       */
#define NV_CTXSW_MAIN_IMAGE_POOL_PTR_HI                                        0x000000e8 /*       */
#define NV_CTXSW_MAIN_IMAGE_CTL                                                0x0000000C /*       */
#define NV_CTXSW_MAIN_IMAGE_CTL_TYPE                                                  5:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_CTL_TYPE_PER_VEID_HEADER                                 0x21 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRI_PATCH_COUNT                                    0x00000010 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRI_PATCH_ADR_LO                                   0x00000014 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRI_PATCH_ADR_HI                                   0x00000018 /*       */
#define NV_CTXSW_MAIN_IMAGE_ZCULL_PTR                                          0x00000020 /*       */
#define NV_CTXSW_MAIN_IMAGE_ZCULL_PTR_V                                              31:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_ZCULL_PTR_HI                                       0x00000060 /*       */
#define NV_CTXSW_MAIN_IMAGE_ZCULL_PTR_HI_V                                           16:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_PM                                                 0x00000028 /*       */
#define NV_CTXSW_MAIN_IMAGE_SMPC_MODE                                                 5:3 /*       */
#define NV_CTXSW_MAIN_IMAGE_SMPC_MODE_NO_CTXSW                                          0 /*       */
#define NV_CTXSW_MAIN_IMAGE_SMPC_MODE_CTXSW                                             1 /*       */
#define NV_CTXSW_MAIN_IMAGE_PM_PTR                                             0x0000002c /*       */
#define NV_CTXSW_MAIN_IMAGE_PM_PTR_HI                                          0x00000094 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_HI                             0x00000064 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_HI_V                                 16:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR                                0x00000068 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_V                                    31:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_VEID0_HI                       0x00000070 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_VEID0_HI_V                           16:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_VEID0                          0x00000074 /*       */
#define NV_CTXSW_MAIN_IMAGE_FULL_PREEMPTION_PTR_VEID0_V                              31:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTEXT_BUFFER_PTR_HI                              0x00000078 /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTEXT_BUFFER_PTR_HI_V                                  16:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTEXT_BUFFER_PTR                                 0x0000007c /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTEXT_BUFFER_PTR_V                                     31:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_GRAPHICS_PREEMPTION_OPTIONS                        0x00000080 /*       */
#define NV_CTXSW_MAIN_IMAGE_GRAPHICS_PREEMPTION_OPTIONS_CONTROL                       1:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_GRAPHICS_PREEMPTION_OPTIONS_CONTROL_WFI                     0 /*       */
#define NV_CTXSW_MAIN_IMAGE_GRAPHICS_PREEMPTION_OPTIONS_CONTROL_GFXP                    1 /*       */
#define NV_CTXSW_MAIN_IMAGE_COMPUTE_PREEMPTION_OPTIONS                         0x00000084 /*       */
#define NV_CTXSW_MAIN_IMAGE_COMPUTE_PREEMPTION_OPTIONS_CONTROL                        1:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_COMPUTE_PREEMPTION_OPTIONS_CONTROL_WFI                      0 /*       */
#define NV_CTXSW_MAIN_IMAGE_COMPUTE_PREEMPTION_OPTIONS_CONTROL_CTA                      1 /*       */
#define NV_CTXSW_MAIN_IMAGE_COMPUTE_PREEMPTION_OPTIONS_CONTROL_CILP                     2 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG                             0x000000a0 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE                               1:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE_ALLOW_ALL              0x00000000 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE_ALLOW_NONE             0x00000001 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE_USE_MAP                0x00000002 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_LO                            0x000000a4 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_LO_V                                31:0 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_HI                            0x000000a8 /*       */
#define NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_HI_V                                16:0 /*       */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GFXP_RTV_SIZE                              0x00000058 /*       */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_GLOBAL_RTV_SIZE                            0x0000005c /*       */
#define NV_CTXSW_GFXP_POOL_CTRL_BLK_RTV_OFFSET                                 0x00000060 /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_RTV_PTR                                     0x00000098 /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_RTV_PTR_HI                                  0x0000009c /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_CB_PTR                                      0x000000b8 /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_CB_PTR_HI                                   0x000000bc /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_PAGEPOOL_PTR                                0x000000c0 /*       */
#define NV_CTXSW_MAIN_IMAGE_GLOBAL_PAGEPOOL_PTR_HI                             0x000000c4 /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTROL_BLOCK_PTR                                  0x000000c8 /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTROL_BLOCK_PTR_HI                               0x000000cc /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_WFI_SAVE_OPERATIONS                            0x000000d0 /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_CTA_SAVE_OPERATIONS                            0x000000d4 /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_GFXP_SAVE_OPERATIONS                           0x000000d8 /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_CILP_SAVE_OPERATIONS                           0x000000dc /*       */
#define NV_CTXSW_MAIN_IMAGE_CONTEXT_ID                                         0x000000f0 /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_SAVE_OPERATIONS                                0x000000f4 /*       */
#define NV_CTXSW_MAIN_IMAGE_NUM_RESTORE_OPERATIONS                             0x000000f8 /*       */

#endif // __tu102_dev_ctxsw_prog_h__
