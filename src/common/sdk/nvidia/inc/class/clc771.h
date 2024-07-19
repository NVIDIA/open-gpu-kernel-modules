/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _clc771_h_
#define _clc771_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC771_DISP_SF_USER (0x000C771)

typedef volatile struct _clc771_tag0 {
    NvU32 dispSfUserOffset[0x400];    /* NV_PDISP_SF_USER   0x000D0FFF:0x000D0000 */
} _NvC771DispSfUser, NvC771DispSfUserMap;

#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL(i)                    (0x000E0000-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1                                   8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE                                  0:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_NO                        0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_YES                       0x00000001 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_DIS                       0x00000000 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_EN                        0x00000001 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER                                   4:4 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_DIS                        0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_EN                         0x00000001 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE                                  8:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_DIS                       0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_EN                        0x00000001 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW                               9:9 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_ENABLE                 0x00000001 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_DISABLE                0x00000000 /* RW--V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_INIT                   0x00000001 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS(i)                  (0x000E0004-0x000E0000+(i)*1024) /* R--4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS__SIZE_1                                 8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS_SENT                                  0:0 /* R-IVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_DONE                      0x00000001 /* R---V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_WAITING                   0x00000000 /* R---V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_INIT                      0x00000000 /* R-I-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER(i)                  (0x000E0008-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER__SIZE_1                                 8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB0                                   7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB0_INIT                       0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB1                                  15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB1_INIT                       0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB2                                 23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_HEADER_HB2_INIT                       0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(i)            (0x000E000C-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW__SIZE_1                           8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0                             7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1                            15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2                           23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3                           31:24 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(i)           (0x000E0010-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH__SIZE_1                          8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4                            7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4_INIT                0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5                           15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5_INIT                0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6                          23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6_INIT                0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(i)            (0x000E0014-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW__SIZE_1                           8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7                             7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8                            15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9                           23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10                          31:24 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10_INIT                0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(i)           (0x000E0018-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH__SIZE_1                          8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11                           7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11_INIT               0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12                          15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12_INIT               0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13                         23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13_INIT               0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(i)            (0x000E001C-0x000E0000+(i)*1024) /* RW-4A */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW__SIZE_1                           8 /*       */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14                             7:0 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15                            15:8 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16                           23:16 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17                           31:24 /* RWIVF */
#define NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17_INIT                 0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_CTRL(i,j)                          (0x000E0200-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_CTRL_ENABLE                                        0:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_CTRL_ENABLE_NO                              0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_CTRL_ENABLE_YES                             0x00000001 /* RW--V */
#define NVC771_SF_SHARED_GENERIC_CTRL_SINGLE                                        4:4 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_CTRL_SINGLE_NO                              0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_CTRL_SINGLE_YES                             0x00000001 /* RW--V */
#define NVC771_SF_SHARED_GENERIC_CTRL_CHECKSUM_HW                                 16:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_CTRL_CHECKSUM_HW_NO                         0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_CTRL_CHECKSUM_HW_YES                        0x00000001 /* RW--V */
#define NVC771_SF_SHARED_GENERIC_CTRL_LOC                                           9:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_CTRL_LOC_VBLANK                             0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_CTRL_LOC_VSYNC                              0x00000001 /* RW--V */
#define NVC771_SF_SHARED_GENERIC_CTRL_LOC_LOADV                              0x00000002 /* RW--V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER(i,j)                (0x000E0204-0x000E0000+(i)*1024+(j)*40) /* RW-4A */

#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_1                               8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_2                               6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB0                                 7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB0_INIT                     0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB1                                15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB1_INIT                     0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB2                               23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB2_INIT                     0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB3                               31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER_HB3_INIT                     0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0(i,j)              (0x000E0208-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB0                               7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB0_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB1                              15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB1_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB2                             23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB2_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB3                             31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0_DB3_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1(i,j)              (0x000E020C-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB4                               7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB4_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB5                              15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB5_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB6                             23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB6_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB7                             31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1_DB7_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2(i,j)              (0x000E0210-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB8                               7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB8_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB9                              15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB9_INIT                   0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB10                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB10_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB11                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2_DB11_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3(i,j)              (0x000E0214-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB12                              7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB12_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB13                             15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB13_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB14                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB14_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB15                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3_DB15_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4(i,j)              (0x000E0218-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB16                              7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB16_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB17                             15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB17_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB18                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB18_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB19                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4_DB19_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5(i,j)              (0x000E021C-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB20                              7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB20_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB21                             15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB21_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB22                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB22_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB23                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5_DB23_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6(i,j)              (0x000E0220-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB24                              7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB24_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB25                             15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB25_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB26                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB26_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB27                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6_DB27_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7(i,j)              (0x000E0224-0x000E0000+(i)*1024+(j)*40) /* RW-4A */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7__SIZE_1                             8 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7__SIZE_2                             6 /*       */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB28                              7:0 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB28_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB29                             15:8 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB29_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB30                            23:16 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB30_INIT                  0x00000000 /* RWI-V */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB31                            31:24 /* RWIVF */
#define NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7_DB31_INIT                  0x00000000 /* RWI-V */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif
