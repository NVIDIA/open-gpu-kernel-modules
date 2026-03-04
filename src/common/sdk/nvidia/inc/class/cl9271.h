/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl9271_h_
#define _cl9271_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NV9271_DISP_SF_USER 0x9271

typedef volatile struct _cl9271_tag0 {
    NvU32 dispSfUserOffset[0x400];    /* NV_PDISP_SF_USER   0x00690FFF:0x00690000 */
} _Nv9271DispSfUser, Nv9271DispSfUserMap;

#define NV9271_SF_HDMI_INFO_IDX_AVI_INFOFRAME                             0x00000000 /*       */
#define NV9271_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME                         0x00000001 /*       */
#define NV9271_SF_HDMI_INFO_IDX_GCP                                       0x00000003 /*       */
#define NV9271_SF_HDMI_INFO_IDX_VSI                                       0x00000004 /*       */
#define NV9271_SF_HDMI_INFO_CTRL(i,j)                 (0x00690000-0x00690000+(i)*1024+(j)*64) /* RWX4A */
#define NV9271_SF_HDMI_INFO_CTRL__SIZE_1                                         4 /*       */
#define NV9271_SF_HDMI_INFO_CTRL__SIZE_2                                         5 /*       */
#define NV9271_SF_HDMI_INFO_CTRL_ENABLE                                        0:0 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_ENABLE_NO                              0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_INFO_CTRL_ENABLE_YES                             0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_ENABLE_DIS                             0x00000000 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_ENABLE_EN                              0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_OTHER                                         4:4 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_OTHER_DIS                              0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_INFO_CTRL_OTHER_EN                               0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_SINGLE                                        8:8 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_SINGLE_DIS                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_INFO_CTRL_SINGLE_EN                              0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW                                     9:9 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW_ENABLE                       0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW_EN                           0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW_DISABLE                      0x00000000 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW_DIS                          0x00000000 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_CHKSUM_HW_INIT                         0x00000001 /* RWI-V */
#define NV9271_SF_HDMI_INFO_CTRL_HBLANK                                      12:12 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_HBLANK_DIS                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_INFO_CTRL_HBLANK_EN                              0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_VIDEO_FMT                                   16:16 /* RWIVF */
#define NV9271_SF_HDMI_INFO_CTRL_VIDEO_FMT_SW_CONTROLLED                0x00000000 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_VIDEO_FMT_HW_CONTROLLED                0x00000001 /* RW--V */
#define NV9271_SF_HDMI_INFO_CTRL_VIDEO_FMT_INIT                         0x00000001 /* RWI-V */
#define NV9271_SF_HDMI_INFO_STATUS(i,j)               (0x00690004-0x00690000+(i)*1024+(j)*64) /* R--4A */
#define NV9271_SF_HDMI_INFO_STATUS__SIZE_1                                       4 /*       */
#define NV9271_SF_HDMI_INFO_STATUS__SIZE_2                                       5 /*       */
#define NV9271_SF_HDMI_INFO_STATUS_SENT                                        0:0 /* R--VF */
#define NV9271_SF_HDMI_INFO_STATUS_SENT_DONE                            0x00000001 /* R---V */
#define NV9271_SF_HDMI_INFO_STATUS_SENT_WAITING                         0x00000000 /* R---V */
#define NV9271_SF_HDMI_INFO_STATUS_SENT_INIT                            0x00000000 /* R-I-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL(i)                    (0x00690000-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1                                   4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE                                  0:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_NO                        0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_YES                       0x00000001 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_DIS                       0x00000000 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_EN                        0x00000001 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER                                   4:4 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_DIS                        0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_EN                         0x00000001 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE                                  8:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_DIS                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_EN                        0x00000001 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW                               9:9 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_ENABLE                 0x00000001 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_DISABLE                0x00000000 /* RW--V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_INIT                   0x00000001 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS(i)                  (0x00690004-0x00690000+(i)*1024) /* R--4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS_SENT                                  0:0 /* R--VF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_DONE                      0x00000001 /* R---V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_WAITING                   0x00000000 /* R---V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_INIT                      0x00000000 /* R-I-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER(i)                  (0x00690008-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB0                                   7:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB0_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB1                                  15:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB1_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB2                                 23:16 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_HEADER_HB2_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(i)            (0x0069000C-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW__SIZE_1                           4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0                             7:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1                            15:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2                           23:16 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3                           31:24 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(i)           (0x00690010-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH__SIZE_1                          4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4                            7:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4_INIT                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5                           15:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5_INIT                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6                          23:16 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6_INIT                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(i)            (0x00690014-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW__SIZE_1                           4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7                             7:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8                            15:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9                           23:16 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9_INIT                 0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10                          31:24 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10_INIT                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(i)           (0x00690018-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH__SIZE_1                          4 /*       */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11                           7:0 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11_INIT               0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12                          15:8 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12_INIT               0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13                         23:16 /* RWIVF */
#define NV9271_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13_INIT               0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_HEADER(i)                        (0x00690048-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_HEADER__SIZE_1                                       4 /*       */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB0                                         7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB0_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB1                                        15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB1_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB2                                       23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_HEADER_HB2_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW(i)                  (0x0069004C-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB0                                   7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB0_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB1                                  15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB1_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB2                                 23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB2_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB3                                 31:24 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_LOW_PB3_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH(i)                 (0x00690050-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB4                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB4_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB5                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB5_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB6                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK0_HIGH_PB6_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW(i)                  (0x00690054-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB7                                   7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB7_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB8                                  15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB8_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB9                                 23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB9_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB10                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_LOW_PB10_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH(i)                 (0x00690058-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB11                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB11_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB12                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB12_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB13                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK1_HIGH_PB13_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW(i)                  (0x0069005C-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB14                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB14_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB15                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB15_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB16                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB16_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB17                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_LOW_PB17_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH(i)                 (0x00690060-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB18                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB18_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB19                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB19_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB20                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK2_HIGH_PB20_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW(i)                  (0x00690064-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB21                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB21_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB22                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB22_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB23                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB23_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB24                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_LOW_PB24_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH(i)                 (0x00690068-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB25                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB25_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB26                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB26_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB27                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_GENERIC_SUBPACK3_HIGH_PB27_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GCP_SUBPACK(i)                           (0x006900CC-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_GCP_SUBPACK__SIZE_1                                          4 /*       */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB0                                            7:0 /* RWIVF */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB0_INIT                                0x00000001 /* RWI-V */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB0_SET_AVMUTE                          0x00000001 /* RW--V */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB0_CLR_AVMUTE                          0x00000010 /* RW--V */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB1                                           15:8 /* RWIVF */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB1_INIT                                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB2                                          23:16 /* RWIVF */
#define NV9271_SF_HDMI_GCP_SUBPACK_SB2_INIT                                0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_HEADER(i)                        (0x00690108-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_HEADER__SIZE_1                                       4 /*       */
#define NV9271_SF_HDMI_VSI_HEADER_HB0                                         7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_HEADER_HB0_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_HEADER_HB1                                        15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_HEADER_HB1_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_HEADER_HB2                                       23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_HEADER_HB2_INIT                             0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW(i)                  (0x0069010C-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB0                                   7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB0_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB1                                  15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB1_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB2                                 23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB2_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB3                                 31:24 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_LOW_PB3_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH(i)                 (0x00690110-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB4                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB4_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB5                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB5_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB6                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK0_HIGH_PB6_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW(i)                  (0x00690114-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB7                                   7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB7_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB8                                  15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB8_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB9                                 23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB9_INIT                       0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB10                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_LOW_PB10_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH(i)                 (0x00690118-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB11                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB11_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB12                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB12_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB13                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK1_HIGH_PB13_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW(i)                  (0x0069011C-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB14                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB14_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB15                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB15_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB16                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB16_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB17                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_LOW_PB17_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH(i)                 (0x00690120-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB18                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB18_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB19                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB19_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB20                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK2_HIGH_PB20_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW(i)                  (0x00690124-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW__SIZE_1                                 4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB21                                  7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB21_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB22                                 15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB22_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB23                                23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB23_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB24                                31:24 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_LOW_PB24_INIT                      0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH(i)                 (0x00690128-0x00690000+(i)*1024) /* RWX4A */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH__SIZE_1                                4 /*       */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB25                                 7:0 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB25_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB26                                15:8 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB26_INIT                     0x00000000 /* RWI-V */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB27                               23:16 /* RWIVF */
#define NV9271_SF_HDMI_VSI_SUBPACK3_HIGH_PB27_INIT                     0x00000000 /* RWI-V */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _cl9271_h_
