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

#ifndef ___dev_graphics_nobundle_h__
#define ___dev_graphics_nobundle_h__

#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_BASE                                       0x00408070 /*       */
#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_BASE_ADDR_39_8                                   31:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_BASE_ADDR_39_8_ALIGN_BITS                  0x00000008 /*       */
#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_SIZE                                       0x00408074 /*       */
#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_SIZE_DIV_256B                                    14:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_RTV_CB_SIZE_DIV_256B_BYTE_GRANULARITY                 0x0100 /*       */
#define NV_PGRAPH_PRI_SCC_RM_GFXP_RESERVE                                      0x00408078 /*       */
#define NV_PGRAPH_PRI_SCC_RM_GFXP_RESERVE_RTV_CB_SIZE_DIV_256B                        8:0 /*       */
#define NV_PGRAPH_PRI_CWD_PARTITION_CTL                                        0x00405b2c /*       */
#define NV_PGRAPH_PRI_CWD_PARTITION_DATA                                       0x00405b30 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1                                      0x004064c4 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_BATCHES                                15:0 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_BATCHES_INIT                         0xffff /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_OUTPUT                                31:16 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_OUTPUT_GRANULARITY                   0x0080 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2                                      0x004064c8 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_STATE_LIMIT                               28:16 /*       */
#define NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_STATE_LIMIT_SCC_BUNDLE_GRANULARITY       0x0020 /*       */
#define NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_BETA                              0x00405830 /*       */
#define NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_BETA_CBSIZE                             21:0 /*       */
#define NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_ALPHA                             0x0040585c /*       */
#define NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_ALPHA_CBSIZE                            15:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE                                    0x00408004 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE_ADDR_39_8                                31:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE_ADDR_39_8_ALIGN_BITS               0x00000008 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE                                    0x00408008 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_DIV_256B                                 10:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_DIV_256B_BYTE_GRANULARITY               0x100 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_VALID                                   31:31 /*       */
#define NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_VALID_TRUE                                0x1 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE                                     0x0040800c /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE_ADDR_39_8                                 31:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE_ADDR_39_8_ALIGN_BITS                0x00000008 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL                                          0x00408010 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES                                     9:0 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES_HWMAX                             0x000 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES_BYTE_GRANULARITY                  0x100 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_MAX_VALID_PAGES                               19:10 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_MAX_VALID_PAGES_MAX                           0x1f6 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_VALID                                         31:31 /*       */
#define NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_VALID_TRUE                                      0x1 /*       */

#endif // ___dev_graphics_nobundle_h__
