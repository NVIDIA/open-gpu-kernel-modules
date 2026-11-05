/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef ___dev_tpc_h__
#define ___dev_tpc_h__

#define NV_PTPC_PRI_PE_PIN_CB_GLOBAL_BASE_ADDR                         0x00000048 /*       */
#define NV_PTPC_PRI_PE_PIN_CB_GLOBAL_BASE_ADDR_V                             27:0 /*       */
#define NV_PTPC_PRI_PE_PIN_CB_GLOBAL_BASE_ADDR_VALID                        28:28 /*       */
#define NV_PTPC_PRI_PE_PIN_CB_GLOBAL_BASE_ADDR_VALID_TRUE              0x00000001 /*       */
#define NV_PTPC_PRI_MPC_VTG_CB_GLOBAL_BASE_ADDR                        0x0000042c /*       */
#define NV_PTPC_PRI_MPC_VTG_CB_GLOBAL_BASE_ADDR_V                            27:0 /*       */
#define NV_PTPC_PRI_MPC_VTG_CB_GLOBAL_BASE_ADDR_VALID                       28:28 /*       */
#define NV_PTPC_PRI_MPC_VTG_CB_GLOBAL_BASE_ADDR_VALID_TRUE             0x00000001 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_0                                        0x00000600 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_0_BASE_ADDR_43_12                              31:0 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1                                        0x00000604 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_SIZE_DIV_128B                                20:0 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_SIZE_DIV_128B_GRANULARITY                    0x80 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_BASE_ADDR_48_44                             28:24 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_LTC_CLASS                                   30:30 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_LTC_CLASS_PASS_THRU                           0x0 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_VALID                                       31:31 /*       */
#define NV_PTPC_PRI_TEX_RM_CB_1_VALID_TRUE                                    0x1 /*       */

#endif // ___dev_tpc_h__
