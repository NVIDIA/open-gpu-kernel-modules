/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc371_h_
#define _clc371_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVC371_DISP_SF_USER (0x000C371)

typedef volatile struct _clc371_tag0 {
    NvU32 dispSfUserOffset[0x400];
} _NvC371DispSfUser, NvC371DispSfUserMap;

#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL(i)                    (0x00690000-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1                                   4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE                                  0:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_NO                        0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_YES                       0x00000001 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_DIS                       0x00000000 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_EN                        0x00000001 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER                                   4:4 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_DIS                        0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_OTHER_EN                         0x00000001 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE                                  8:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_DIS                       0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_SINGLE_EN                        0x00000001 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW                               9:9 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_ENABLE                 0x00000001 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_DISABLE                0x00000000 /* RW--V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_INIT                   0x00000001 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS(i)                  (0x00690004-0x00690000+(i)*1024) /* R--4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS__SIZE_1                                 4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS_SENT                                  0:0 /* R--VF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_DONE                      0x00000001 /* R---V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_WAITING                   0x00000000 /* R---V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_INIT                      0x00000000 /* R-I-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER(i)                  (0x00690008-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER__SIZE_1                                 4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB0                                   7:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB0_INIT                       0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB1                                  15:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB1_INIT                       0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB2                                 23:16 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_HEADER_HB2_INIT                       0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(i)            (0x0069000C-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW__SIZE_1                           4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0                             7:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1                            15:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2                           23:16 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3                           31:24 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(i)           (0x00690010-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH__SIZE_1                          4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4                            7:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4_INIT                0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5                           15:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5_INIT                0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6                          23:16 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6_INIT                0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(i)            (0x00690014-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW__SIZE_1                           4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7                             7:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8                            15:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9                           23:16 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9_INIT                 0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10                          31:24 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10_INIT                0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(i)           (0x00690018-0x00690000+(i)*1024) /* RWX4A */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH__SIZE_1                          4 /*       */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11                           7:0 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11_INIT               0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12                          15:8 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12_INIT               0x00000000 /* RWI-V */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13                         23:16 /* RWIVF */
#define NVC371_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13_INIT               0x00000000 /* RWI-V */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _clc371_h_
