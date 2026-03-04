/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc973_h_
#define _clc973_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC973_DISP_CAPABILITIES 0xC973

#define NVC973_SYS_CAP                                                0x0 /* RW-4R */
#define NVC973_SYS_CAP_HEAD0_EXISTS                                          0:0 /* RWIVF */
#define NVC973_SYS_CAP_HEAD0_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD0_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD1_EXISTS                                          1:1 /* RWIVF */
#define NVC973_SYS_CAP_HEAD1_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD1_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD2_EXISTS                                          2:2 /* RWIVF */
#define NVC973_SYS_CAP_HEAD2_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD2_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD3_EXISTS                                          3:3 /* RWIVF */
#define NVC973_SYS_CAP_HEAD3_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD3_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD4_EXISTS                                          4:4 /* RWIVF */
#define NVC973_SYS_CAP_HEAD4_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD4_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD5_EXISTS                                          5:5 /* RWIVF */
#define NVC973_SYS_CAP_HEAD5_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD5_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD6_EXISTS                                          6:6 /* RWIVF */
#define NVC973_SYS_CAP_HEAD6_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD6_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD7_EXISTS                                          7:7 /* RWIVF */
#define NVC973_SYS_CAP_HEAD7_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD7_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC973_SYS_CAP_HEAD_EXISTS(i)                            (0+(i)):(0+(i)) /* RWIVF */
#define NVC973_SYS_CAP_HEAD_EXISTS__SIZE_1                                     8 /*       */
#define NVC973_SYS_CAP_HEAD_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_HEAD_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR0_EXISTS                                           8:8 /* RWIVF */
#define NVC973_SYS_CAP_SOR0_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR0_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR1_EXISTS                                           9:9 /* RWIVF */
#define NVC973_SYS_CAP_SOR1_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR1_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR2_EXISTS                                         10:10 /* RWIVF */
#define NVC973_SYS_CAP_SOR2_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR2_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR3_EXISTS                                         11:11 /* RWIVF */
#define NVC973_SYS_CAP_SOR3_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR3_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR4_EXISTS                                         12:12 /* RWIVF */
#define NVC973_SYS_CAP_SOR4_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR4_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR5_EXISTS                                         13:13 /* RWIVF */
#define NVC973_SYS_CAP_SOR5_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR5_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR6_EXISTS                                         14:14 /* RWIVF */
#define NVC973_SYS_CAP_SOR6_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR6_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR7_EXISTS                                         15:15 /* RWIVF */
#define NVC973_SYS_CAP_SOR7_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR7_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_SOR_EXISTS(i)                             (8+(i)):(8+(i)) /* RWIVF */
#define NVC973_SYS_CAP_SOR_EXISTS__SIZE_1                                      8 /*       */
#define NVC973_SYS_CAP_SOR_EXISTS_NO                                  0x00000000 /* RW--V */
#define NVC973_SYS_CAP_SOR_EXISTS_YES                                 0x00000001 /* RW--V */
#define NVC973_SYS_CAP_DSI0_EXISTS                                         20:20 /* RWIVF */
#define NVC973_SYS_CAP_DSI0_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_DSI0_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_DSI1_EXISTS                                         21:21 /* RWIVF */
#define NVC973_SYS_CAP_DSI1_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_DSI1_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_DSI2_EXISTS                                         22:22 /* RWIVF */
#define NVC973_SYS_CAP_DSI2_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_DSI2_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_DSI3_EXISTS                                         23:23 /* RWIVF */
#define NVC973_SYS_CAP_DSI3_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC973_SYS_CAP_DSI3_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC973_SYS_CAP_DSI_EXISTS(i)                           (20+(i)):(20+(i)) /* RWIVF */
#define NVC973_SYS_CAP_DSI_EXISTS__SIZE_1                                      4 /*       */
#define NVC973_SYS_CAP_DSI_EXISTS_NO                                  0x00000000 /* RW--V */
#define NVC973_SYS_CAP_DSI_EXISTS_YES                                 0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA                                       0x10 /* RW-4R */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRIES                             15:0 /* RWIUF */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRY_WIDTH                        17:16 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRY_WIDTH_32B               0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRY_WIDTH_64B               0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRY_WIDTH_128B              0x00000002 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_MEMPOOL_ENTRY_WIDTH_256B              0x00000003 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_ROTATION                           18:18 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_ROTATION_FALSE                0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_ROTATION_TRUE                 0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_PLANAR                             19:19 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_PLANAR_FALSE                  0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_PLANAR_TRUE                   0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_VGA                                20:20 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_VGA_FALSE                     0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_VGA_TRUE                      0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MEMPOOL_COMPRESSION                21:21 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MEMPOOL_COMPRESSION_FALSE     0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MEMPOOL_COMPRESSION_TRUE      0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MSCG                               22:22 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MSCG_FALSE                    0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MSCG_TRUE                     0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MCLK_SWITCH                        23:23 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MCLK_SWITCH_FALSE             0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_MCLK_SWITCH_TRUE              0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_LATENCY_EVENT                      26:26 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_LATENCY_EVENT_FALSE           0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_SUPPORT_LATENCY_EVENT_TRUE            0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_REQUEST_SIZE_PER_LINE_NON_ROTATION         31:30 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPA_REQUEST_SIZE_PER_LINE_NON_ROTATION_32B 0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_REQUEST_SIZE_PER_LINE_NON_ROTATION_64B 0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_REQUEST_SIZE_PER_LINE_NON_ROTATION_128B 0x00000002 /* RW--V */
#define NVC973_IHUB_COMMON_CAPA_REQUEST_SIZE_PER_LINE_NON_ROTATION_256B 0x00000003 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC                                       0x18 /* RW-4R */
#define NVC973_IHUB_COMMON_CAPC_PITCH_REQUEST_SIZE                           1:0 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPC_PITCH_REQUEST_SIZE_32B                0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_PITCH_REQUEST_SIZE_64B                0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_PITCH_REQUEST_SIZE_128B               0x00000002 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_PITCH_REQUEST_SIZE_256B               0x00000003 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED                           6:4 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED_NONE               0x00000000 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED_TWO                0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED_FOUR               0x00000002 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED_EIGHT              0x00000003 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_MAX_LINES_BUFFERED_SIXTEEN            0x00000004 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_SEMI_PLANAR                        11:11 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_SEMI_PLANAR_FALSE             0x00000000 /* RWI-V */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_SEMI_PLANAR_TRUE              0x00000001 /* RW--V */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_HOR_VER_FLIP                       12:12 /* RWIVF */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_HOR_VER_FLIP_FALSE            0x00000000 /* RWI-V */
#define NVC973_IHUB_COMMON_CAPC_SUPPORT_HOR_VER_FLIP_TRUE             0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA(i)                                      (0x680+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPA__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPA_FULL_WIDTH                                              4:0 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_UNIT_WIDTH                                              9:5 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC0_PRESENT                                         16:16 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC0_PRESENT_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC0_PRESENT_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC1_PRESENT                                         17:17 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC1_PRESENT_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_OCSC1_PRESENT_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_SCLR_PRESENT                                          18:18 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_SCLR_PRESENT_TRUE                                0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_SCLR_PRESENT_FALSE                               0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_HCLPF_PRESENT                                         19:19 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_HCLPF_PRESENT_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_HCLPF_PRESENT_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_DTH_PRESENT                                           20:20 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_DTH_PRESENT_TRUE                                 0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_DTH_PRESENT_FALSE                                0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_OSCAN_PRESENT                                         21:21 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_OSCAN_PRESENT_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_OSCAN_PRESENT_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_DSC_PRESENT                                           22:22 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_DSC_PRESENT_TRUE                                 0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_DSC_PRESENT_FALSE                                0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_VFILTER_PRESENT                                       23:23 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPA_VFILTER_PRESENT_TRUE                             0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPA_VFILTER_PRESENT_FALSE                            0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB(i)                                      (0x684+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPB__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPB_VGA                                                     0:0 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPB_VGA_TRUE                                         0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB_VGA_FALSE                                        0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_SZ                                                12:1 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_LOGNR                                            15:13 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_SFCLOAD                                          17:17 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_SFCLOAD_TRUE                                0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_SFCLOAD_FALSE                               0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_DIRECT                                           18:18 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_DIRECT_TRUE                                 0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPB_OLUT_DIRECT_FALSE                                0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC(i)                                      (0x688+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPC__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC0_PRECISION                                         4:0 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC0_UNITY_CLAMP                                       5:5 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC0_UNITY_CLAMP_TRUE                           0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC0_UNITY_CLAMP_FALSE                          0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC1_PRECISION                                        12:8 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC1_UNITY_CLAMP                                     13:13 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC1_UNITY_CLAMP_TRUE                           0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_OCSC1_UNITY_CLAMP_FALSE                          0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_SF_PRECISION                                     20:16 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_CI_PRECISION                                     24:21 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_EXT_RGB                                       25:25 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_EXT_RGB_TRUE                             0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_EXT_RGB_FALSE                            0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_MAX_SCALE_FACTOR                              28:28 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_MAX_SCALE_FACTOR_2X                      0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_VS_MAX_SCALE_FACTOR_4X                      0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_HS_MAX_SCALE_FACTOR                              30:30 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_HS_MAX_SCALE_FACTOR_2X                      0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPC_SCLR_HS_MAX_SCALE_FACTOR_4X                      0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPD(i)                                      (0x68c+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPD__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPD_VSCLR_MAX_PIXELS_2TAP                                  15:0 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPD_VSCLR_MAX_PIXELS_5TAP                                 31:16 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPE(i)                                      (0x690+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPE__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE422                                         16:16 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE422_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE422_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE420                                         17:17 /* RWIVF */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE420_TRUE                               0x00000001 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPE_DSC_NATIVE420_FALSE                              0x00000000 /* RW--V */
#define NVC973_POSTCOMP_HDR_CAPF(i)                                      (0x694+(i)*32) /* RW-4A */
#define NVC973_POSTCOMP_HDR_CAPF__SIZE_1                                                   8 /*       */
#define NVC973_POSTCOMP_HDR_CAPF_VFILTER_MAX_PIXELS                                     15:0 /* RWIVF */
#define NVC973_SOR_CAP(i)                                     (0x144+(i)*8) /* RW-4A */
#define NVC973_SOR_CAP__SIZE_1                                                 8 /*       */
#define NVC973_SOR_CAP_SINGLE_LVDS_18                                        0:0 /* RWIVF */
#define NVC973_SOR_CAP_SINGLE_LVDS_18_FALSE                           0x00000000 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_LVDS_18_TRUE                            0x00000001 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_LVDS_24                                        1:1 /* RWIVF */
#define NVC973_SOR_CAP_SINGLE_LVDS_24_FALSE                           0x00000000 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_LVDS_24_TRUE                            0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DUAL_LVDS_18                                          2:2 /* RWIVF */
#define NVC973_SOR_CAP_DUAL_LVDS_18_FALSE                             0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DUAL_LVDS_18_TRUE                              0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DUAL_LVDS_24                                          3:3 /* RWIVF */
#define NVC973_SOR_CAP_DUAL_LVDS_24_FALSE                             0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DUAL_LVDS_24_TRUE                              0x00000001 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_TMDS_A                                         8:8 /* RWIVF */
#define NVC973_SOR_CAP_SINGLE_TMDS_A_FALSE                            0x00000000 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_TMDS_A_TRUE                             0x00000001 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_TMDS_B                                         9:9 /* RWIVF */
#define NVC973_SOR_CAP_SINGLE_TMDS_B_FALSE                            0x00000000 /* RW--V */
#define NVC973_SOR_CAP_SINGLE_TMDS_B_TRUE                             0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DUAL_TMDS                                           11:11 /* RWIVF */
#define NVC973_SOR_CAP_DUAL_TMDS_FALSE                                0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DUAL_TMDS_TRUE                                 0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DISPLAY_OVER_PCIE                                   13:13 /* RWIVF */
#define NVC973_SOR_CAP_DISPLAY_OVER_PCIE_FALSE                        0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DISPLAY_OVER_PCIE_TRUE                         0x00000001 /* RW--V */
#define NVC973_SOR_CAP_SDI                                                 16:16 /* RWIVF */
#define NVC973_SOR_CAP_SDI_FALSE                                      0x00000000 /* RW--V */
#define NVC973_SOR_CAP_SDI_TRUE                                       0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DP_A                                                24:24 /* RWIVF */
#define NVC973_SOR_CAP_DP_A_FALSE                                     0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DP_A_TRUE                                      0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DP_B                                                25:25 /* RWIVF */
#define NVC973_SOR_CAP_DP_B_FALSE                                     0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DP_B_TRUE                                      0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DP_INTERLACE                                        26:26 /* RWIVF */
#define NVC973_SOR_CAP_DP_INTERLACE_FALSE                             0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DP_INTERLACE_TRUE                              0x00000001 /* RW--V */
#define NVC973_SOR_CAP_DP_8_LANES                                          27:27 /* RWIVF */
#define NVC973_SOR_CAP_DP_8_LANES_FALSE                               0x00000000 /* RW--V */
#define NVC973_SOR_CAP_DP_8_LANES_TRUE                                0x00000001 /* RW--V */
#define NVC973_SOR_CAP_HDMI_FRL                                            28:28 /* RWIVF */
#define NVC973_SOR_CAP_HDMI_FRL_FALSE                                 0x00000000 /* RW--V */
#define NVC973_SOR_CAP_HDMI_FRL_TRUE                                  0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA(i)                              (0x780+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_FULL_WIDTH                                      4:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_UNIT_WIDTH                                      9:5 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_ALPHA_WIDTH                                   13:10 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC00_PRESENT                                 16:16 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC00_PRESENT_TRUE                       0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC00_PRESENT_FALSE                      0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC0LUT_PRESENT                               17:17 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC0LUT_PRESENT_TRUE                     0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC0LUT_PRESENT_FALSE                    0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC01_PRESENT                                 18:18 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC01_PRESENT_TRUE                       0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC01_PRESENT_FALSE                      0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_SCLR_PRESENT                                  19:19 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_SCLR_PRESENT_TRUE                        0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_SCLR_PRESENT_FALSE                       0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_TMO_PRESENT                                   20:20 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_TMO_PRESENT_TRUE                         0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_TMO_PRESENT_FALSE                        0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_GMA_PRESENT                                   21:21 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_GMA_PRESENT_TRUE                         0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_GMA_PRESENT_FALSE                        0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC10_PRESENT                                 22:22 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC10_PRESENT_TRUE                       0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC10_PRESENT_FALSE                      0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC1LUT_PRESENT                               23:23 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC1LUT_PRESENT_TRUE                     0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC1LUT_PRESENT_FALSE                    0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC11_PRESENT                                 24:24 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC11_PRESENT_TRUE                       0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPA_CSC11_PRESENT_FALSE                      0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB(i)                              (0x784+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_FMT_PRECISION                                   4:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_LOGSZ                                      9:6 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_LOGNR                                    12:10 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_SFCLOAD                                  14:14 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_SFCLOAD_TRUE                        0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_SFCLOAD_FALSE                       0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_DIRECT                                   15:15 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_DIRECT_TRUE                         0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPB_ILUT_DIRECT_FALSE                        0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC(i)                              (0x788+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC00_PRECISION                                 4:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC00_UNITY_CLAMP                               5:5 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC00_UNITY_CLAMP_TRUE                   0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC00_UNITY_CLAMP_FALSE                  0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC0LUT_DIRECT                                15:15 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC0LUT_DIRECT_TRUE                      0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC0LUT_DIRECT_FALSE                     0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC01_PRECISION                               20:16 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC01_UNITY_CLAMP                             21:21 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC01_UNITY_CLAMP_TRUE                   0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPC_CSC01_UNITY_CLAMP_FALSE                  0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD(i)                              (0x78c+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_LOGSZ                                       3:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_LOGNR                                       6:4 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_SFCLOAD                                     8:8 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_SFCLOAD_TRUE                         0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_SFCLOAD_FALSE                        0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_DIRECT                                      9:9 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_DIRECT_TRUE                          0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_TMO_DIRECT_FALSE                         0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_SF_PRECISION                             16:12 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_CI_PRECISION                             20:17 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_EXT_RGB                               21:21 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_EXT_RGB_TRUE                     0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_EXT_RGB_FALSE                    0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_EXT_ALPHA                                22:22 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_EXT_ALPHA_TRUE                      0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_EXT_ALPHA_FALSE                     0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_MAX_SCALE_FACTOR                      28:28 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_MAX_SCALE_FACTOR_2X              0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_VS_MAX_SCALE_FACTOR_4X              0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_HS_MAX_SCALE_FACTOR                      30:30 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_HS_MAX_SCALE_FACTOR_2X              0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPD_SCLR_HS_MAX_SCALE_FACTOR_4X              0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE(i)                              (0x790+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC10_PRECISION                                 4:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC10_UNITY_CLAMP                               5:5 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC10_UNITY_CLAMP_TRUE                   0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC10_UNITY_CLAMP_FALSE                  0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC1LUT_DIRECT                                15:15 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC1LUT_DIRECT_TRUE                      0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC1LUT_DIRECT_FALSE                     0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC11_PRECISION                               20:16 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC11_UNITY_CLAMP                             21:21 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC11_UNITY_CLAMP_TRUE                   0x00000001 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPE_CSC11_UNITY_CLAMP_FALSE                  0x00000000 /* RW--V */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPF(i)                              (0x794+(i)*32) /* RW-4A */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPF__SIZE_1                                          32 /*       */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPF_VSCLR_MAX_PIXELS_2TAP                          15:0 /* RWIVF */
#define NVC973_PRECOMP_WIN_PIPE_HDR_CAPF_VSCLR_MAX_PIXELS_5TAP                         31:16 /* RWIVF */

#ifdef __cplusplus
};
#endif /* extern C */
#endif //_clc973_h_
