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

#ifndef _clc971_h_
#define _clc971_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC971_DISP_SF_USER (0x000C971)

typedef volatile struct {
    NvU32 dispSfUserOffset[0x400];
} _NvC971DispSfUser, NvC971DispSfUserMap;

#define NVC971_SF_HDMI_INFO_IDX_AVI_INFOFRAME                             0x00000000 /*       */
#define NVC971_SF_HDMI_INFO_IDX_GCP                                       0x00000001 /*       */
#define NVC971_SF_HDMI_INFO_IDX_ACR                                       0x00000002 /*       */
#define NVC971_SF_HDMI_INFO_CTRL(i,j)                 (0x000E0000-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_CTRL__SIZE_1                                         8 /*       */
#define NVC971_SF_HDMI_INFO_CTRL__SIZE_2                                         3 /*       */
#define NVC971_SF_HDMI_INFO_CTRL_ENABLE                                        0:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_CTRL_ENABLE_NO                              0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_CTRL_ENABLE_YES                             0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_ENABLE_DIS                             0x00000000 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_ENABLE_EN                              0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW                                     9:9 /* RWIVF */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW_ENABLE                       0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW_EN                           0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW_DISABLE                      0x00000000 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW_DIS                          0x00000000 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_CHKSUM_HW_INIT                         0x00000001 /* RWI-V */
#define NVC971_SF_HDMI_INFO_CTRL_HBLANK                                      12:12 /* RWIVF */
#define NVC971_SF_HDMI_INFO_CTRL_HBLANK_DIS                             0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_CTRL_HBLANK_EN                              0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_VIDEO_FMT                                   16:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_CTRL_VIDEO_FMT_SW_CONTROLLED                0x00000000 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_VIDEO_FMT_HW_CONTROLLED                0x00000001 /* RW--V */
#define NVC971_SF_HDMI_INFO_CTRL_VIDEO_FMT_INIT                         0x00000001 /* RWI-V */
#define NVC971_SF_HDMI_INFO_STATUS(i,j)               (0x000E0004-0x000E0000+(i)*1024+(j)*64) /* R--4A */
#define NVC971_SF_HDMI_INFO_STATUS__SIZE_1                                       8 /*       */
#define NVC971_SF_HDMI_INFO_STATUS__SIZE_2                                       3 /*       */
#define NVC971_SF_HDMI_INFO_STATUS_SENT                                        0:0 /* R--VF */
#define NVC971_SF_HDMI_INFO_STATUS_SENT_DONE                            0x00000001 /* R---V */
#define NVC971_SF_HDMI_INFO_STATUS_SENT_WAITING                         0x00000000 /* R---V */
#define NVC971_SF_HDMI_INFO_HEADER(i,j)               (0x000E0008-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_HEADER__SIZE_1                                       8 /*       */
#define NVC971_SF_HDMI_INFO_HEADER__SIZE_2                                       3 /*       */
#define NVC971_SF_HDMI_INFO_HEADER_HB0                                         7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_HEADER_HB0_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_HEADER_HB1                                        15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_HEADER_HB1_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_HEADER_HB2                                       23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_HEADER_HB2_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW(i,j)         (0x000E000C-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW__SIZE_2                                 3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB0                                   7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB0_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB1                                  15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB1_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB2                                 23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB2_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB3                                 31:24 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_LOW_PB3_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH(i,j)        (0x000E0010-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH__SIZE_1                                8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH__SIZE_2                                3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB4                                  7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB4_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB5                                 15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB5_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB6                                23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK0_HIGH_PB6_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW(i,j)         (0x000E0014-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW__SIZE_2                                 3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB7                                   7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB7_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB8                                  15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB8_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB9                                 23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB9_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB10                                31:24 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_LOW_PB10_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH(i,j)        (0x000E0018-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH__SIZE_1                                8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH__SIZE_2                                3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB11                                 7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB11_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB12                                15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB12_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB13                               23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK1_HIGH_PB13_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW(i,j)         (0x000E001C-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW__SIZE_2                                 3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB14                                  7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB14_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB15                                 15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB15_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB16                                23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB16_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB17                                31:24 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_LOW_PB17_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH(i,j)        (0x000E0020-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH__SIZE_1                                8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH__SIZE_2                                3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB18                                 7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB18_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB19                                15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB19_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB20                               23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK2_HIGH_PB20_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW(i,j)         (0x000E0024-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW__SIZE_2                                 3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB21                                  7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB21_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB22                                 15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB22_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB23                                23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB23_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB24                                31:24 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_LOW_PB24_INIT                      0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH(i,j)        (0x000E0028-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH__SIZE_1                                8 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH__SIZE_2                                3 /*       */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB25                                 7:0 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB25_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB26                                15:8 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB26_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB27                               23:16 /* RWIVF */
#define NVC971_SF_HDMI_INFO_SUBPACK3_HIGH_PB27_INIT                     0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL(i)                    (0x000E0000-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1                                   8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE                                  0:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_NO                        0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_YES                       0x00000001 /* RW--V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_DIS                       0x00000000 /* RW--V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_EN                        0x00000001 /* RW--V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW                               9:9 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_ENABLE                 0x00000001 /* RW--V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_DISABLE                0x00000000 /* RW--V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_INIT                   0x00000001 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS(i)                  (0x000E0004-0x000E0000+(i)*1024) /* R--4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS_SENT                                  0:0 /* R-IVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_DONE                      0x00000001 /* R---V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_WAITING                   0x00000000 /* R---V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_INIT                      0x00000000 /* R-I-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER(i)                  (0x000E0008-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER__SIZE_1                                 8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB0                                   7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB0_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB1                                  15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB1_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB2                                 23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_HEADER_HB2_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(i)            (0x000E000C-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW__SIZE_1                           8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0                             7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1                            15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2                           23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3                           31:24 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(i)           (0x000E0010-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH__SIZE_1                          8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4                            7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4_INIT                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5                           15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5_INIT                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6                          23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6_INIT                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(i)            (0x000E0014-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW__SIZE_1                           8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7                             7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8                            15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9                           23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10                          31:24 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10_INIT                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(i)           (0x000E0018-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH__SIZE_1                          8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11                           7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11_INIT               0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12                          15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12_INIT               0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13                         23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13_INIT               0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(i)            (0x000E001C-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW__SIZE_1                           8 /*       */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14                             7:0 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15                            15:8 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16                           23:16 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17                           31:24 /* RWIVF */
#define NVC971_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17_INIT                 0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_GCP_CTRL(i)                              (0x000E0040-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_GCP_CTRL__SIZE_1                                             8 /*       */
#define NVC971_SF_HDMI_GCP_CTRL_ENABLE                                            0:0 /* RWIVF */
#define NVC971_SF_HDMI_GCP_CTRL_ENABLE_NO                                  0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_GCP_CTRL_ENABLE_YES                                 0x00000001 /* RW--V */
#define NVC971_SF_HDMI_GCP_CTRL_ENABLE_DIS                                 0x00000000 /* RW--V */
#define NVC971_SF_HDMI_GCP_CTRL_ENABLE_EN                                  0x00000001 /* RW--V */
#define NVC971_SF_HDMI_GCP_STATUS(i)                            (0x000E0044-0x000E0000+(i)*1024) /* R--4A */
#define NVC971_SF_HDMI_GCP_STATUS__SIZE_1                                           8 /*       */
#define NVC971_SF_HDMI_GCP_STATUS_SENT                                            0:0 /* R-IVF */
#define NVC971_SF_HDMI_GCP_STATUS_SENT_DONE                                0x00000001 /* R---V */
#define NVC971_SF_HDMI_GCP_STATUS_SENT_WAITING                             0x00000000 /* R---V */
#define NVC971_SF_HDMI_GCP_STATUS_SENT_INIT                                0x00000000 /* R-I-V */
#define NVC971_SF_HDMI_GCP_SUBPACK(i)                           (0x000E004C-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_HDMI_GCP_SUBPACK__SIZE_1                                          8 /*       */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB0                                            7:0 /* RWIVF */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB0_INIT                                0x00000001 /* RWI-V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB0_SET_AVMUTE                          0x00000001 /* RW--V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB0_CLR_AVMUTE                          0x00000010 /* RW--V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1                                           15:8 /* RWIVF */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1_INIT                                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB2                                          23:16 /* RWIVF */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB2_INIT                                0x00000000 /* RWI-V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1_CTRL                                     24:24 /* RWIVF */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1_CTRL_INIT                           0x00000001 /* RWI-V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1_CTRL_SW                             0x00000000 /* RW--V */
#define NVC971_SF_HDMI_GCP_SUBPACK_SB1_CTRL_HW                             0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL(i,j)                          (0x000E0130-0x000E0000+(i)*1024+(j)*8) /* RW-4A */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL__SIZE_1                                         8 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL__SIZE_2                                        10 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_ENABLE                                        0:0 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_ENABLE_NO                              0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_ENABLE_YES                             0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE                                      3:1 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_ALWAYS                        0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_ONCE                          0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_ALWAYS                    0x00000002 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_ONCE                      0x00000003 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_TRIGGER                   0x00000004 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_LOC                                           5:4 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_LOC_VBLANK                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_LOC_VSYNC                              0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_LOC_LINE                               0x00000002 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_OFFSET                                       10:6 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_OFFSET_INIT                            0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SIZE                                        18:14 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SIZE_INIT                              0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_BUSY                                        22:22 /* R-IVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_BUSY_NO                                0x00000000 /* R-I-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_BUSY_YES                               0x00000001 /* R---V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SENT                                        23:23 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SENT_NO                                0x00000000 /* R-I-V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SENT_YES                               0x00000001 /* R---V */
#define NVC971_SF_GENERIC_INFOFRAME_CTRL_SENT_CLEAR                             0x00000001 /* -W--C */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG(i,j)                        (0x000E0134-0x000E0000+(i)*1024+(j)*8) /* RW-4A */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG__SIZE_1                                       8 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG__SIZE_2                                      10 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_FID                                         7:0 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_FID_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID                                    23:8 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_INIT                         0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED                          24:24 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED_NO                  0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED_YES                 0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN                        25:25 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN_NO                0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN_YES               0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM                               29:29 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM_NO                       0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM_YES                      0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_NEW                                       30:30 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_NEW_INIT                             0x00000000 /* R-I-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_NEW_DONE                             0x00000000 /* R---V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_NEW_PENDING                          0x00000001 /* R---T */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_NEW_TRIGGER                          0x00000001 /* -W--T */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL                            31:31 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL_ACT                   0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL_ARM                   0x00000001 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_CTRL(i)                  (0x000E03F0-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_CTRL__SIZE_1                                    8 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_CTRL_OFFSET                                   4:0 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_CTRL_OFFSET_INIT                       0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_DATA(i)                  (0x000E03F4-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_GENERIC_INFOFRAME_DATA__SIZE_1                                          8 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE0                                         7:0 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE0_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE1                                        15:8 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE1_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE2                                       23:16 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE2_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE3                                       31:24 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_DATA_BYTE3_INIT                             0x00000000 /* RWI-V */
#define NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL(i)                  (0x000E03F8-0x000E0000+(i)*1024) /* RW-4A */
#define NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL__SIZE_1                                        8 /*       */
#define NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY                               1:1 /* RWIVF */
#define NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY_HIGH                   0x00000000 /* RW--V */
#define NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY_LOW                    0x00000001 /* RWI-V */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _clc971_h_
