/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc373_h_
#define _clc373_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC373_DISP_CAPABILITIES 0xC373

typedef volatile struct _clc373_tag0 {
    NvU32 dispCapabilities[0x400];
} _NvC373DispCapabilities,NvC373DispCapabilities_Map ;


#define NVC373_SYS_CAP                                                0x0 /* RW-4R */
#define NVC373_SYS_CAP_HEAD0_EXISTS                                          0:0 /* RWIVF */
#define NVC373_SYS_CAP_HEAD0_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD0_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD0_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD1_EXISTS                                          1:1 /* RWIVF */
#define NVC373_SYS_CAP_HEAD1_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD1_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD1_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD2_EXISTS                                          2:2 /* RWIVF */
#define NVC373_SYS_CAP_HEAD2_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD2_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD2_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD3_EXISTS                                          3:3 /* RWIVF */
#define NVC373_SYS_CAP_HEAD3_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD3_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD3_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD4_EXISTS                                          4:4 /* RWIVF */
#define NVC373_SYS_CAP_HEAD4_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD4_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD4_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD5_EXISTS                                          5:5 /* RWIVF */
#define NVC373_SYS_CAP_HEAD5_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD5_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD5_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD6_EXISTS                                          6:6 /* RWIVF */
#define NVC373_SYS_CAP_HEAD6_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD6_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD6_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD7_EXISTS                                          7:7 /* RWIVF */
#define NVC373_SYS_CAP_HEAD7_EXISTS_INIT                              0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD7_EXISTS_NO                                0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD7_EXISTS_YES                               0x00000001 /* RW--V */
#define NVC373_SYS_CAP_HEAD_EXISTS(i)                            (0+(i)):(0+(i)) /* RWIVF */
#define NVC373_SYS_CAP_HEAD_EXISTS__SIZE_1                                     8 /*       */
#define NVC373_SYS_CAP_HEAD_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_HEAD_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_HEAD_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR0_EXISTS                                           8:8 /* RWIVF */
#define NVC373_SYS_CAP_SOR0_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR0_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR0_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR1_EXISTS                                           9:9 /* RWIVF */
#define NVC373_SYS_CAP_SOR1_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR1_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR1_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR2_EXISTS                                         10:10 /* RWIVF */
#define NVC373_SYS_CAP_SOR2_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR2_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR2_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR3_EXISTS                                         11:11 /* RWIVF */
#define NVC373_SYS_CAP_SOR3_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR3_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR3_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR4_EXISTS                                         12:12 /* RWIVF */
#define NVC373_SYS_CAP_SOR4_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR4_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR4_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR5_EXISTS                                         13:13 /* RWIVF */
#define NVC373_SYS_CAP_SOR5_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR5_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR5_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR6_EXISTS                                         14:14 /* RWIVF */
#define NVC373_SYS_CAP_SOR6_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR6_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR6_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR7_EXISTS                                         15:15 /* RWIVF */
#define NVC373_SYS_CAP_SOR7_EXISTS_INIT                               0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR7_EXISTS_NO                                 0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR7_EXISTS_YES                                0x00000001 /* RW--V */
#define NVC373_SYS_CAP_SOR_EXISTS(i)                             (8+(i)):(8+(i)) /* RWIVF */
#define NVC373_SYS_CAP_SOR_EXISTS__SIZE_1                                      8 /*       */
#define NVC373_SYS_CAP_SOR_EXISTS_INIT                                0x00000000 /* RWI-V */
#define NVC373_SYS_CAP_SOR_EXISTS_NO                                  0x00000000 /* RW--V */
#define NVC373_SYS_CAP_SOR_EXISTS_YES                                 0x00000001 /* RW--V */
#define NVC373_SYS_CAPB                                                0x4 /* RW-4R */
#define NVC373_SYS_CAPB_WINDOW0_EXISTS                                        0:0 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW0_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW0_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW0_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW1_EXISTS                                        1:1 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW1_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW1_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW1_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW2_EXISTS                                        2:2 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW2_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW2_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW2_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW3_EXISTS                                        3:3 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW3_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW3_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW3_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW4_EXISTS                                        4:4 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW4_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW4_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW4_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW5_EXISTS                                        5:5 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW5_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW5_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW5_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW6_EXISTS                                        6:6 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW6_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW6_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW6_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW7_EXISTS                                        7:7 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW7_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW7_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW7_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW8_EXISTS                                        8:8 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW8_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW8_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW8_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW9_EXISTS                                        9:9 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW9_EXISTS_INIT                            0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW9_EXISTS_NO                              0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW9_EXISTS_YES                             0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW10_EXISTS                                     10:10 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW10_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW10_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW10_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW11_EXISTS                                     11:11 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW11_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW11_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW11_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW12_EXISTS                                     12:12 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW12_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW12_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW12_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW13_EXISTS                                     13:13 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW13_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW13_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW13_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW14_EXISTS                                     14:14 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW14_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW14_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW14_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW15_EXISTS                                     15:15 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW15_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW15_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW15_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW16_EXISTS                                     16:16 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW16_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW16_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW16_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW17_EXISTS                                     17:17 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW17_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW17_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW17_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW18_EXISTS                                     18:18 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW18_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW18_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW18_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW19_EXISTS                                     19:19 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW19_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW19_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW19_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW20_EXISTS                                     20:20 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW20_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW20_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW20_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW21_EXISTS                                     21:21 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW21_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW21_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW21_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW22_EXISTS                                     22:22 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW22_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW22_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW22_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW23_EXISTS                                     23:23 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW23_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW23_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW23_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW24_EXISTS                                     24:24 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW24_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW24_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW24_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW25_EXISTS                                     25:25 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW25_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW25_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW25_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW26_EXISTS                                     26:26 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW26_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW26_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW26_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW27_EXISTS                                     27:27 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW27_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW27_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW27_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW28_EXISTS                                     28:28 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW28_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW28_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW28_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW29_EXISTS                                     29:29 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW29_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW29_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW29_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW30_EXISTS                                     30:30 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW30_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW30_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW30_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW31_EXISTS                                     31:31 /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW31_EXISTS_INIT                           0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW31_EXISTS_NO                             0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW31_EXISTS_YES                            0x00000001 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW_EXISTS(i)                          (0+(i)):(0+(i)) /* RWIVF */
#define NVC373_SYS_CAPB_WINDOW_EXISTS__SIZE_1                                  32 /*       */
#define NVC373_SYS_CAPB_WINDOW_EXISTS_INIT                             0x00000000 /* RWI-V */
#define NVC373_SYS_CAPB_WINDOW_EXISTS_NO                               0x00000000 /* RW--V */
#define NVC373_SYS_CAPB_WINDOW_EXISTS_YES                              0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA(i)                                  (0x30+(i)*32) /* RW-4A */
#define NVC373_HEAD_CAPA__SIZE_1                                               8 /*       */
#define NVC373_HEAD_CAPA_SCALER                                              0:0 /* RWIVF */
#define NVC373_HEAD_CAPA_SCALER_TRUE                                  0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_SCALER_FALSE                                 0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_SCALER_INIT                                  0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_SCALER_HAS_YUV422                                   1:1 /* RWIVF */
#define NVC373_HEAD_CAPA_SCALER_HAS_YUV422_TRUE                       0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_SCALER_HAS_YUV422_FALSE                      0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_SCALER_HAS_YUV422_INIT                       0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_HSAT                                                2:2 /* RWIVF */
#define NVC373_HEAD_CAPA_HSAT_TRUE                                    0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_HSAT_FALSE                                   0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_HSAT_INIT                                    0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_OCSC                                                3:3 /* RWIVF */
#define NVC373_HEAD_CAPA_OCSC_TRUE                                    0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_OCSC_FALSE                                   0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_OCSC_INIT                                    0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_YUV422                                              4:4 /* RWIVF */
#define NVC373_HEAD_CAPA_YUV422_TRUE                                  0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_YUV422_FALSE                                 0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_YUV422_INIT                                  0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_LUT_TYPE                                            6:5 /* RWIVF */
#define NVC373_HEAD_CAPA_LUT_TYPE_NONE                                0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_LUT_TYPE_257                                 0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_LUT_TYPE_1025                                0x00000002 /* RW--V */
#define NVC373_HEAD_CAPA_LUT_TYPE_INIT                                0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPA_LUT_LOCATION                                        7:7 /* RWIVF */
#define NVC373_HEAD_CAPA_LUT_LOCATION_EARLY                           0x00000000 /* RW--V */
#define NVC373_HEAD_CAPA_LUT_LOCATION_LATE                            0x00000001 /* RW--V */
#define NVC373_HEAD_CAPA_LUT_LOCATION_INIT                            0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPD(i)                                  (0x3c+(i)*32) /* RW-4A */
#define NVC373_HEAD_CAPD__SIZE_1                                               8 /*       */
#define NVC373_HEAD_CAPD_MAX_PIXELS_2TAP422                                 15:0 /* RWIUF */
#define NVC373_HEAD_CAPD_MAX_PIXELS_2TAP422_INIT                      0x00000000 /* RWI-V */
#define NVC373_HEAD_CAPD_MAX_PIXELS_2TAP444                                31:16 /* RWIUF */
#define NVC373_HEAD_CAPD_MAX_PIXELS_2TAP444_INIT                      0x00000000 /* RWI-V */
#define NVC373_SOR_CAP(i)                                     (0x144+(i)*8) /* RW-4A */
#define NVC373_SOR_CAP__SIZE_1                                                 8 /*       */
#define NVC373_SOR_CAP_SINGLE_LVDS_18                                        0:0 /* RWIVF */
#define NVC373_SOR_CAP_SINGLE_LVDS_18_INIT                            0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_SINGLE_LVDS_18_FALSE                           0x00000000 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_LVDS_18_TRUE                            0x00000001 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_LVDS_24                                        1:1 /* RWIVF */
#define NVC373_SOR_CAP_SINGLE_LVDS_24_INIT                            0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_SINGLE_LVDS_24_FALSE                           0x00000000 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_LVDS_24_TRUE                            0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DUAL_LVDS_18                                          2:2 /* RWIVF */
#define NVC373_SOR_CAP_DUAL_LVDS_18_INIT                              0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DUAL_LVDS_18_FALSE                             0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DUAL_LVDS_18_TRUE                              0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DUAL_LVDS_24                                          3:3 /* RWIVF */
#define NVC373_SOR_CAP_DUAL_LVDS_24_INIT                              0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DUAL_LVDS_24_FALSE                             0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DUAL_LVDS_24_TRUE                              0x00000001 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_TMDS_A                                         8:8 /* RWIVF */
#define NVC373_SOR_CAP_SINGLE_TMDS_A_INIT                             0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_SINGLE_TMDS_A_FALSE                            0x00000000 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_TMDS_A_TRUE                             0x00000001 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_TMDS_B                                         9:9 /* RWIVF */
#define NVC373_SOR_CAP_SINGLE_TMDS_B_INIT                             0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_SINGLE_TMDS_B_FALSE                            0x00000000 /* RW--V */
#define NVC373_SOR_CAP_SINGLE_TMDS_B_TRUE                             0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DUAL_TMDS                                           11:11 /* RWIVF */
#define NVC373_SOR_CAP_DUAL_TMDS_INIT                                 0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DUAL_TMDS_FALSE                                0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DUAL_TMDS_TRUE                                 0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DISPLAY_OVER_PCIE                                   13:13 /* RWIVF */
#define NVC373_SOR_CAP_DISPLAY_OVER_PCIE_INIT                         0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DISPLAY_OVER_PCIE_FALSE                        0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DISPLAY_OVER_PCIE_TRUE                         0x00000001 /* RW--V */
#define NVC373_SOR_CAP_SDI                                                 16:16 /* RWIVF */
#define NVC373_SOR_CAP_SDI_INIT                                       0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_SDI_FALSE                                      0x00000000 /* RW--V */
#define NVC373_SOR_CAP_SDI_TRUE                                       0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DP_A                                                24:24 /* RWIVF */
#define NVC373_SOR_CAP_DP_A_INIT                                      0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DP_A_FALSE                                     0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DP_A_TRUE                                      0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DP_B                                                25:25 /* RWIVF */
#define NVC373_SOR_CAP_DP_B_INIT                                      0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DP_B_FALSE                                     0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DP_B_TRUE                                      0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DP_INTERLACE                                        26:26 /* RWIVF */
#define NVC373_SOR_CAP_DP_INTERLACE_INIT                              0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DP_INTERLACE_FALSE                             0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DP_INTERLACE_TRUE                              0x00000001 /* RW--V */
#define NVC373_SOR_CAP_DP_8_LANES                                          27:27 /* RWIVF */
#define NVC373_SOR_CAP_DP_8_LANES_INIT                                0x00000000 /* RWI-V */
#define NVC373_SOR_CAP_DP_8_LANES_FALSE                               0x00000000 /* RW--V */
#define NVC373_SOR_CAP_DP_8_LANES_TRUE                                0x00000001 /* RW--V */
#define NVC373_SOR_CLK_CAP(i)                                 (0x608+(i)*4) /* RW-4A */
#define NVC373_SOR_CLK_CAP__SIZE_1                                             8 /*       */
#define NVC373_SOR_CLK_CAP_DP_MAX                                            7:0 /* RWIUF */
#define NVC373_SOR_CLK_CAP_DP_MAX_INIT                                0x00000036 /* RWI-V */
#define NVC373_SOR_CLK_CAP_TMDS_MAX                                        23:16 /* RWIUF */
#define NVC373_SOR_CLK_CAP_TMDS_MAX_INIT                              0x0000003C /* RWI-V */
#define NVC373_SOR_CLK_CAP_LVDS_MAX                                        31:24 /* RWIUF */
#define NVC373_SOR_CLK_CAP_LVDS_MAX_INIT                              0x00000000 /* RWI-V */

#ifdef __cplusplus
};
#endif /* extern C */
#endif //_clc373_h_
