/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clca71_h_
#define _clca71_h_

#ifdef __cplusplus
extern "C" {
#endif

#define NVCA71_DISP_SF_USER (0x000CA71)

typedef volatile struct _clca71_tag0 {
    NvU32 dispSfUserOffset[0x400];    /* NV_PDISP_SF_USER   0x000D0FFF:0x000D0000 */
} _NvCA71DispSfUser, NvCA71DispSfUserMap;

#define NVCA71_SF_HDMI_INFO_CTRL(i,j)                 (0x000E0000-0x000E0000+(i)*1024+(j)*64) /* RW-4A */
#define NVCA71_SF_HDMI_INFO_CTRL__SIZE_1                                         8 /*       */
#define NVCA71_SF_HDMI_INFO_CTRL__SIZE_2                                         3 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL(i)                    (0x000E0000-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1                                   8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE                                  0:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_NO                        0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_YES                       0x00000001 /* RW--V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_DIS                       0x00000000 /* RW--V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_ENABLE_EN                        0x00000001 /* RW--V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW                               9:9 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_ENABLE                 0x00000001 /* RW--V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_DISABLE                0x00000000 /* RW--V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_CTRL_CHKSUM_HW_INIT                   0x00000001 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS(i)                  (0x000E0004-0x000E0000+(i)*1024) /* R--4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS__SIZE_1                                 8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS_SENT                                  0:0 /* R-IVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_DONE                      0x00000001 /* R---V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_WAITING                   0x00000000 /* R---V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_STATUS_SENT_INIT                      0x00000000 /* R-I-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER(i)                  (0x000E0008-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER__SIZE_1                                 8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB0                                   7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB0_INIT                       0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB1                                  15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB1_INIT                       0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB2                                 23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_HEADER_HB2_INIT                       0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(i)            (0x000E000C-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW__SIZE_1                           8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0                             7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB0_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1                            15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB1_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2                           23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB2_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3                           31:24 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW_PB3_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(i)           (0x000E0010-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH__SIZE_1                          8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4                            7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB4_INIT                0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5                           15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB5_INIT                0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6                          23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH_PB6_INIT                0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(i)            (0x000E0014-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW__SIZE_1                           8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7                             7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB7_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8                            15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB8_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9                           23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB9_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10                          31:24 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW_PB10_INIT                0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(i)           (0x000E0018-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH__SIZE_1                          8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11                           7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB11_INIT               0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12                          15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB12_INIT               0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13                         23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH_PB13_INIT               0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(i)            (0x000E001C-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW__SIZE_1                           8 /*       */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14                             7:0 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB14_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15                            15:8 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB15_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16                           23:16 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB16_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17                           31:24 /* RWIVF */
#define NVCA71_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW_PB17_INIT                 0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL(i,j)                          (0x000E0130-0x000E0000+(i)*1024+(j)*8) /* RW-4A */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL__SIZE_1                                         8 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL__SIZE_2                                        10 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_ENABLE                                        0:0 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_ENABLE_NO                              0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_ENABLE_YES                             0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE                                      3:1 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_ALWAYS                        0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_ONCE                          0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_ALWAYS                    0x00000002 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_ONCE                      0x00000003 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_RUN_MODE_FID_TRIGGER                   0x00000004 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_LOC                                           5:4 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_LOC_VBLANK                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_LOC_VSYNC                              0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_LOC_LINE                               0x00000002 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_OFFSET                                       10:6 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_OFFSET_INIT                            0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SIZE                                        18:14 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SIZE_INIT                              0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_BUSY                                        22:22 /* R-IVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_BUSY_NO                                0x00000000 /* R-I-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_BUSY_YES                               0x00000001 /* R---V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SENT                                        23:23 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SENT_NO                                0x00000000 /* R-I-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SENT_YES                               0x00000001 /* R---V */
#define NVCA71_SF_GENERIC_INFOFRAME_CTRL_SENT_CLEAR                             0x00000001 /* -W--C */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG(i,j)                        (0x000E0134-0x000E0000+(i)*1024+(j)*8) /* RW-4A */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG__SIZE_1                                       8 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG__SIZE_2                                      10 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_FID                                         7:0 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_FID_INIT                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID                                    23:8 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_INIT                         0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED                          24:24 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED_NO                  0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_LINE_ID_REVERSED_YES                 0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN                        25:25 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN_NO                0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_AS_SDP_OVERRIDE_EN_YES               0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM                               29:29 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM_NO                       0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_HW_CHECKSUM_YES                      0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_NEW                                       30:30 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_NEW_INIT                             0x00000000 /* R-I-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_NEW_DONE                             0x00000000 /* R---V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_NEW_PENDING                          0x00000001 /* R---T */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_NEW_TRIGGER                          0x00000001 /* -W--T */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL                            31:31 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL_ACT                   0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_CONFIG_MTD_STATE_CTRL_ARM                   0x00000001 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_CTRL(i)                  (0x000E03F0-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_CTRL__SIZE_1                                    8 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_CTRL_OFFSET                                   4:0 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_CTRL_OFFSET_INIT                       0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA(i)                  (0x000E03F4-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA__SIZE_1                                          8 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE0                                         7:0 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE0_INIT                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE1                                        15:8 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE1_INIT                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE2                                       23:16 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE2_INIT                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE3                                       31:24 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_DATA_BYTE3_INIT                             0x00000000 /* RWI-V */
#define NVCA71_SF_GENERIC_INFOFRAME_MISC_CTRL(i)                  (0x000E03F8-0x000E0000+(i)*1024) /* RW-4A */
#define NVCA71_SF_GENERIC_INFOFRAME_MISC_CTRL__SIZE_1                                        8 /*       */
#define NVCA71_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY                               1:1 /* RWIVF */
#define NVCA71_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY_HIGH                   0x00000000 /* RW--V */
#define NVCA71_SF_GENERIC_INFOFRAME_MISC_CTRL_AUDIO_PRIORITY_LOW                    0x00000001 /* RWI-V */

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _clca71_h_
