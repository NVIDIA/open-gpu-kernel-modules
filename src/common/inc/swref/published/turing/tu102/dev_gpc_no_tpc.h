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

#ifndef ___dev_gpc_no_tpc_h__
#define ___dev_gpc_no_tpc_h__

#define NV_PGPC_PRI_SETUP_RM_ATTRIB_CB_BASE                                              0x00000810 /*       */
#define NV_PGPC_PRI_SETUP_RM_ATTRIB_CB_BASE_ADDR_39_12                                         27:0 /*       */
#define NV_PGPC_PRI_SETUP_RM_ATTRIB_CB_BASE_ADDR_39_12_ALIGN_BITS                        0x0000000c /*       */
#define NV_PGPC_PRI_SETUP_RM_ATTRIB_CB_BASE_VALID                                             31:31 /*       */
#define NV_PGPC_PRI_SETUP_RM_ATTRIB_CB_BASE_VALID_TRUE                                            1 /*       */
#define NV_PGPC_PRI_SWDX_RM_SPILL_BUFFER_ADDR                                            0x000001D8 /*       */
#define NV_PGPC_PRI_SWDX_RM_SPILL_BUFFER_ADDR_39_8                                             31:0 /*       */
#define NV_PGPC_PRI_SWDX_RM_SPILL_BUFFER_ADDR_39_8_ALIGN_BITS                            0x00000008 /*       */
#define NV_PGPC_PRI_SWDX_RM_SPILL_BUFFER_SIZE                                            0x000001DC /*       */
#define NV_PGPC_PRI_SWDX_RM_SPILL_BUFFER_SIZE_256B                                             15:0 /*       */
#define NV_PGPC_PRI_SWDX_BETA_CB_CTRL                                                    0x000001E4 /*       */
#define NV_PGPC_PRI_SWDX_BETA_CB_CTRL_CBES_RESERVE                                             11:0 /*       */
#define NV_PGPC_PRI_SWDX_BETA_CB_CTRL_CBES_RESERVE_NON_GFXP                                       0 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_BASE                                               0x00000E24 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_BASE_ADDR_39_8                                           31:0 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_SIZE                                               0x00000E28 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_SIZE_DIV_256B                                            10:0 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_SIZE_VALID                                              31:31 /*       */
#define NV_PGPC_PRI_SWDX_RM_BUNDLE_CB_SIZE_VALID_TRUE                                             1 /*       */
#define NV_PGPC_PRI_SWDX_TC_BETA_CB_SIZE(i)                                      (0x00000EA0+(i)*4) /*       */
#define NV_PGPC_PRI_SWDX_TC_BETA_CB_SIZE_V                                                     21:0 /*       */
#define NV_PGPC_PRI_GCC_RM_RTV_CB_BASE                                                   0x00001034 /*       */
#define NV_PGPC_PRI_GCC_RM_RTV_CB_BASE_ADDR_39_8                                         31:0       /*       */
#define NV_PGPC_PRI_GCC_RM_PAGEPOOL_BASE                                                 0x00001004 /*       */
#define NV_PGPC_PRI_GCC_RM_PAGEPOOL_BASE_ADDR_39_8                                       31:0       /*       */
#define NV_PGPC_PRI_GCC_RM_PAGEPOOL                                                      0x00001008 /*       */
#define NV_PGPC_PRI_GCC_RM_PAGEPOOL_TOTAL_PAGES                                          9:0        /*       */

#endif // ___dev_gpc_no_tpc_h__
