/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef DEV_P2060_H
#define DEV_P2060_H

#define NV_P2060_STATUS                                         0x00 /* R--1R */
#define NV_P2060_STATUS_VAL                                      7:0 /* R-XVF */
#define NV_P2060_STATUS_VCXO                                     1:0 /* R-XVF */
#define NV_P2060_STATUS_VCXO_NOLOCK_TOO_FAST                    0x00 /* R---V */
#define NV_P2060_STATUS_VCXO_NOLOCK_TOO_SLOW                    0x01 /* R---V */
#define NV_P2060_STATUS_VCXO_LOCK                               0x02 /* R---V */
#define NV_P2060_STATUS_VCXO_NOT_SERVO                          0x03 /* R---V */
#define NV_P2060_STATUS_SYNC_LOSS                                2:2 /* R-XVF */
#define NV_P2060_STATUS_SYNC_LOSS_FALSE                         0x00 /* R---V */
#define NV_P2060_STATUS_SYNC_LOSS_TRUE                          0x01 /* R---V */
#define NV_P2060_STATUS_RESERVED1                                3:3 /* RWXVF */
#define NV_P2060_STATUS_GPU_STEREO                               4:4 /* R-XVF */
#define NV_P2060_STATUS_GPU_STEREO_NOT_ACTIVE                   0x00 /* R---V */
#define NV_P2060_STATUS_GPU_STEREO_ACTIVE                       0x01 /* R---V */
#define NV_P2060_STATUS_MSTR_STEREO                              5:5 /* R-XVF */
#define NV_P2060_STATUS_MSTR_STEREO_NOT_ACTIVE                  0x00 /* R---V */
#define NV_P2060_STATUS_MSTR_STEREO_ACTIVE                      0x01 /* R---V */
#define NV_P2060_STATUS_STEREO                                   6:6 /* R-XVF */
#define NV_P2060_STATUS_STEREO_NOLOCK                           0x00 /* R---V */
#define NV_P2060_STATUS_STEREO_LOCK                             0x01 /* R---V */
#define NV_P2060_STATUS_RESERVED2                                7:7 /* RWXVF */

#define NV_P2060_STATUS2                                        0x01 /* RW-1R */
#define NV_P2060_STATUS2_VAL                                     7:0 /* R-XVF */
#define NV_P2060_STATUS2_PORT0                                   0:0 /* RWIVF */
#define NV_P2060_STATUS2_PORT0_INPUT                            0x00 /* RWI-V */
#define NV_P2060_STATUS2_PORT0_OUTPUT                           0x01 /* RW--V */
#define NV_P2060_STATUS2_PORT1                                   1:1 /* RWIVF */
#define NV_P2060_STATUS2_PORT1_INPUT                            0x00 /* RWI-V */
#define NV_P2060_STATUS2_PORT1_OUTPUT                           0x01 /* RW--V */
#define NV_P2060_STATUS2_ETHER0_DETECTED                         2:2 /* RWIVF */
#define NV_P2060_STATUS2_ETHER0_DETECTED_FALSE                  0x00 /* RWI-V */
#define NV_P2060_STATUS2_ETHER0_DETECTED_TRUE                   0x01 /* R---V */
#define NV_P2060_STATUS2_ETHER1_DETECTED                         3:3 /* RWIVF */
#define NV_P2060_STATUS2_ETHER1_DETECTED_FALSE                  0x00 /* RWI-V */
#define NV_P2060_STATUS2_ETHER1_DETECTED_TRUE                   0x01 /* R---V */
#define NV_P2060_STATUS2_HS_DETECT                               5:4 /* RWXVF */
#define NV_P2060_STATUS2_HS_DETECT_NONE                         0x00 /* R---V */
#define NV_P2060_STATUS2_HS_DETECT_TTL                          0x01 /* R---V */
#define NV_P2060_STATUS2_HS_DETECT_COMPOSITE                    0x02 /* R---V */
#define NV_P2060_STATUS2_HS_DETECT_NOT_IN_USE                   0x03 /* R---V */
#define NV_P2060_STATUS2_GPU_PORT                                7:6 /* R-XVF */
#define NV_P2060_STATUS2_GPU_PORT_CONN0                         0x00 /* R---V */
#define NV_P2060_STATUS2_GPU_PORT_CONN1                         0x01 /* R---V */
#define NV_P2060_STATUS2_GPU_PORT_CONN2                         0x02 /* R---V */
#define NV_P2060_STATUS2_GPU_PORT_CONN3                         0x03 /* R---V */

#define NV_P2060_STATUS3                                        0x02 /* RW-1R */
#define NV_P2060_STATUS3_VAL                                     7:0 /* R-XVF */
#define NV_P2060_STATUS3_RESERVED                                0:0 /* R-XVF */
#define NV_P2060_STATUS3_LB_INT_FAIL                             1:1 /* R-XVF */
#define NV_P2060_STATUS3_LB_INT_FAIL_FALSE                      0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_INT_FAIL_TRUE                       0x01 /* RW--V */
#define NV_P2060_STATUS3_LB_VTGRST_FAIL                          2:2 /* R-XVF */
#define NV_P2060_STATUS3_LB_VTGRST_FAIL_FALSE                   0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_VTGRST_FAIL_TRUE                    0x01 /* RW--V */
#define NV_P2060_STATUS3_LB_GSWPRDY_FAIL                         3:3 /* R-XVF */
#define NV_P2060_STATUS3_LB_GSWPRDY_FAIL_FALSE                  0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_GSWPRDY_FAIL_TRUE                   0x01 /* RW--V */
#define NV_P2060_STATUS3_LB_SYNC_FAIL                            4:4 /* RWXVF */
#define NV_P2060_STATUS3_LB_SYNC_FAIL_FALSE                     0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_SYNC_FAIL_TRUE                      0x01 /* RW--V */
#define NV_P2060_STATUS3_LB_STEREO_FAIL                          5:5 /* RWXVF */
#define NV_P2060_STATUS3_LB_STEREO_FAIL_FALSE                   0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_STEREO_FAIL_TRUE                    0x01 /* RW--V */
#define NV_P2060_STATUS3_LB_SWPRDY_FAIL                          6:6 /* RWXVF */
#define NV_P2060_STATUS3_LB_SWPRDY_FAIL_FALSE                   0x00 /* RW--V */
#define NV_P2060_STATUS3_LB_SWPRDY_FAIL_TRUE                    0x01 /* RW--V */
#define NV_P2060_STATUS3_GENLOCKED                               7:7 /* RWXVF */
#define NV_P2060_STATUS3_GENLOCKED_FALSE                        0x00 /* RW--V */
#define NV_P2060_STATUS3_GENLOCKED_TRUE                         0x01 /* RW--V */

#define NV_P2060_STATUS4                                        0x13 /* RW-1R */
#define NV_P2060_STATUS4_VAL                                     7:0 /* R-XVF */
#define NV_P2060_STATUS4_INT_GROUP                               7:6 /* R-XVF */
#define NV_P2060_STATUS4_INT_GROUP_LOSS                         0x00 /* R-XVF */
#define NV_P2060_STATUS4_INT_GROUP_GAIN                         0x01 /* R-XVF */
#define NV_P2060_STATUS4_INT_GROUP_MISC                         0x02 /* R-XVF */
#define NV_P2060_STATUS4_SYNC                                    0:0 /* R---V */
#define NV_P2060_STATUS4_STEREO                                  1:1 /* R---V */
#define NV_P2060_STATUS4_HS                                      2:2 /* R---V */
#define NV_P2060_STATUS4_RJ45                                    3:3 /* R---V */
#define NV_P2060_STATUS4_RESERVED_GRP01                          5:4 /* R---V */
//Value 1 in bits 0-5 indicate loss and gain depending on interrupt group 00/01 (bit 6-7)
#define NV_P2060_STATUS4_FRM_CNT_MATCH_INT                       0:0 /* R-XVF */
#define NV_P2060_STATUS4_FRM_CNT_MATCH_INT_CLEAR                0x00 /* R---V */
#define NV_P2060_STATUS4_FRM_CNT_MATCH_INT_PENDING              0x01 /* R---V */
#define NV_P2060_STATUS4_SWAPRDY_INT                             1:1 /* R-XVF */
#define NV_P2060_STATUS4_SWAPRDY_INT_CLEAR                      0x00 /* R---V */
#define NV_P2060_STATUS4_SWAPRDY_INT_PENDING                    0x01 /* R---V */
#define NV_P2060_STATUS4_ERROR_INT                               2:2 /* R-XVF */
#define NV_P2060_STATUS4_ERROR_INT_CLEAR                        0x00 /* R---V */
#define NV_P2060_STATUS4_ERROR_INT_PENDING                      0x01 /* R---V */
#define NV_P2060_STATUS4_FRM_CNT_ROLLOVER_INT                    3:3 /* R-XVF */
#define NV_P2060_STATUS4_FRM_CNT_ROLLOVER_INT_CLEAR             0x00 /* R---V */
#define NV_P2060_STATUS4_FRM_CNT_ROLLOVER_INT_PENDING           0x01 /* R---V */
#define NV_P2060_STATUS4_RESERVED_GRP10                          5:4 /* R---V */
//Value 1 in bits 0-5 indicate interrupt pending depending on interrupt group 10 (bit 6-7)

#define NV_P2060_CONTROL                                        0x03 /* RW-1R */
#define NV_P2060_CONTROL_I_AM                                    0:0 /* RWXVF */
#define NV_P2060_CONTROL_I_AM_SLAVE                             0x00 /* RWI-V */
#define NV_P2060_CONTROL_I_AM_MASTER                            0x01 /* RWI-V */
#define NV_P2060_CONTROL_SYNC_POLARITY                           2:1 /* RWXVF */
#define NV_P2060_CONTROL_SYNC_POLARITY_RISING_EDGE              0x00 /* RW--V */
#define NV_P2060_CONTROL_SYNC_POLARITY_FALLING_EDGE             0x01 /* RW--V */
#define NV_P2060_CONTROL_SYNC_POLARITY_BOTH                     0x02 /* RW--V */
#define NV_P2060_CONTROL_TEST_MODE                               3:3 /* RWXVF */
#define NV_P2060_CONTROL_TEST_MODE_OFF                          0x00 /* RW--V */
#define NV_P2060_CONTROL_TEST_MODE_ON                           0x01 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SRC                                5:4 /* RWXVF */
#define NV_P2060_CONTROL_SYNC_SRC_CONN0                         0x00 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SRC_CONN1                         0x01 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SRC_CONN2                         0x02 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SRC_CONN3                         0x03 /* RW--V */
#define NV_P2060_CONTROL_INTERLACE_MODE                          6:6 /* RWXVF */
#define NV_P2060_CONTROL_INTERLACE_MODE_FALSE                   0x00 /* RW--V */
#define NV_P2060_CONTROL_INTERLACE_MODE_TRUE                    0x01 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SELECT                             7:7 /* RWXVF */
#define NV_P2060_CONTROL_SYNC_SELECT_INTERNAL                   0x00 /* RW--V */
#define NV_P2060_CONTROL_SYNC_SELECT_HOUSE                      0x01 /* RW--V */

#define NV_P2060_CONTROL2                                       0x04 /* RW-1R */
#define NV_P2060_CONTROL2_LAMUX                                  1:0 /* RWXVF */
#define NV_P2060_CONTROL2_LAMUX_0                               0x00 /* RWI-V */
#define NV_P2060_CONTROL2_FRAMERATE_RPT                          3:2 /* RWXVF */
#define NV_P2060_CONTROL2_FRAMERATE_RPT_LIVE                    0x00 /* RW--V */
#define NV_P2060_CONTROL2_FRAMERATE_RPT_MIN                     0x02 /* RW--V */
#define NV_P2060_CONTROL2_FRAMERATE_RPT_MAX                     0x03 /* RW--V */
#define NV_P2060_CONTROL2_RESET                                  4:4 /* RWXVF */
#define NV_P2060_CONTROL2_RESET_FALSE                           0x00 /* RW--V */
#define NV_P2060_CONTROL2_RESET_TRUE                            0x01 /* RW--V */
#define NV_P2060_CONTROL2_SWAP_READY                             5:5 /* RWXVF */
#define NV_P2060_CONTROL2_SWAP_READY_DISABLE                    0x00 /* RW--V */
#define NV_P2060_CONTROL2_SWAP_READY_ENABLE                     0x01 /* RW--V */
#define NV_P2060_CONTROL2_RESERVED                               6:6 /* RWXVF */
#define NV_P2060_CONTROL2_LOOPBACK_MODE                          7:7 /* RWXVF */
#define NV_P2060_CONTROL2_LOOPBACK_MODE_OFF                     0x00 /* RW--V */
#define NV_P2060_CONTROL2_LOOPBACK_MODE_ON                      0x01 /* RW--V */

#define NV_P2060_CONTROL3                                       0x05 /* RW-1R */
#define NV_P2060_CONTROL3_INTERRUPT                              6:0 /* RWXVF */
#define NV_P2060_CONTROL3_INTERRUPT_DISABLE                     0x00 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_STEREO_CHG               0x01 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_ERROR                    0x02 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_FRAME_MATCH              0x04 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_HS_CHG                   0x08 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_SYNC_CHG                 0x10 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_RJ45_CHG                 0x20 /* RW--V */
#define NV_P2060_CONTROL3_INTERRUPT_ON_ALL                      0x7f /* RW--V */
#define NV_P2060_CONTROL3_RESYNC                                 7:7 /* RWXVF */
#define NV_P2060_CONTROL3_RESYNC_OFF                            0x00 /* RW--V */
#define NV_P2060_CONTROL3_RESYNC_ON                             0x01 /* RW--V */

#define NV_P2060_CONTROL4                                       0x06 /* RW-1R */
#define NV_P2060_CONTROL4_SWPRDYINT_DELAY                        2:0 /* RWXVF */
#define NV_P2060_CONTROL4_STEREO_LOCK_MODE                       3:3 /* RWXVF */
#define NV_P2060_CONTROL4_STEREO_LOCK_MODE_OFF                  0x00 /* RW--V */
#define NV_P2060_CONTROL4_STEREO_LOCK_MODE_ON                   0x01 /* RW--V */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC                        4:4 /* RWXVF */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC_OFF                   0x00 /* RW--V */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC_ON                    0x01 /* RW--V */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC_POL                    5:5 /* RWXVF */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC_POL_LOW               0x00 /* RW--V */
#define NV_P2060_CONTROL4_EXT_STEREO_SYNC_POL_HI                0x01 /* RW--V */
#define NV_P2060_CONTROL4_RESERVED2                              7:6 /* RWXVF */

#define NV_P2060_FPGA                                           0x07 /* R--1R */
#define NV_P2060_FPGA_REV                                        3:0 /* R-XVF */

#define NV_P2060_FPGA_ID                                         7:4 /* R-XVF */
#define NV_P2060_FPGA_ID_0                                      0x00 /* R---V */
#define NV_P2060_FPGA_ID_5                                      0x05 /* R---V */

#define NV_P2061_FPGA_ID                                         7:4 /* R-XVF */
#define NV_P2061_FPGA_ID_4                                      0x04 /* R---V */

#define NV_P2060_SYNC_SKEW_LOW                                  0x08 /* RW-1R */
#define NV_P2060_SYNC_SKEW_LOW_VAL                               7:0 /* RWIVF */
#define NV_P2060_SYNC_SKEW_LOW_VAL_0                            0x00 /* RWI-V */

#define NV_P2060_SYNC_SKEW_HIGH                                 0x09 /* RW-1R */
#define NV_P2060_SYNC_SKEW_HIGH_VAL                              7:0 /* RWIVF */
#define NV_P2060_SYNC_SKEW_HIGH_VAL_0                           0x00 /* RWI-V */

#define NV_P2060_SYNC_SKEW_UPPER                                0x35 /* RW-1R */
#define NV_P2060_SYNC_SKEW_UPPER_VAL                             7:0 /* RWIVF */
#define NV_P2060_SYNC_SKEW_UPPER_VAL_0                          0x00 /* RWI-V */

#define NV_P2060_START_DELAY_LOW                                0x0A /* RW-1R */
#define NV_P2060_START_DELAY_LOW_VAL                             7:0 /* RWIVF */
#define NV_P2060_START_DELAY_LOW_VAL_0                          0x00 /* RWI-V */

#define NV_P2060_START_DELAY_HIGH                               0x0B /* RW-1R */
#define NV_P2060_START_DELAY_HIGH_VAL                            7:0 /* RWIVF */
#define NV_P2060_START_DELAY_HIGH_VAL_0                         0x00 /* RWI-V */

#define NV_P2060_NSYNC                                          0x0C /* RW-1R */
#define NV_P2060_NSYNC_FL                                        2:0 /* RWIVF */
#define NV_P2060_NSYNC_GPU                                       6:4 /* RWIVF */
#define NV_P2060_NSYNC_ALL                                       7:0 /* RWIVF */

#define NV_P2060_FRAMECNTR_LOW                                  0x0D /* R--1R */
#define NV_P2060_FRAMECNTR_LOW_VAL                               7:0 /* RWIVF */
#define NV_P2060_FRAMECNTR_LOW_VAL_0                            0x00 /* RWI-V */

#define NV_P2060_FRAMECNTR_MID                                  0x0E /* R--1R */
#define NV_P2060_FRAMECNTR_MID_VAL                               7:0 /* RWIVF */
#define NV_P2060_FRAMECNTR_MID_VAL_0                            0x00 /* RWI-V */

#define NV_P2060_FRAMECNTR_HIGH                                 0x0F /* R--1R */
#define NV_P2060_FRAMECNTR_HIGH_VAL                              7:0 /* RWIVF */
#define NV_P2060_FRAMECNTR_HIGH_VAL_0                           0x00 /* RWI-V */

#define NV_P2060_FRAMERATE_LOW                                  0x10 /* R--1R */
#define NV_P2060_FRAMERATE_LOW_VAL                               7:0 /* RWIVF */
#define NV_P2060_FRAMERATE_LOW_VAL_0                            0x00 /* RWI-V */

#define NV_P2060_FRAMERATE_MID                                  0x11 /* R--1R */
#define NV_P2060_FRAMERATE_MID_VAL                               7:0 /* RWIVF */
#define NV_P2060_FRAMERATE_MID_VAL_0                            0x00 /* RWI-V */

#define NV_P2060_FRAMERATE_HIGH                                 0x12 /* R--1R */
#define NV_P2060_FRAMERATE_HIGH_VAL                              7:0 /* RWIVF */
#define NV_P2060_FRAMERATE_HIGH_VAL_0                           0x00 /* RWI-V */

#define NV_P2060_FPGA_EXREV                                     0x17 /* R--1R */
#define NV_P2060_FPGA_EXREV_VAL                                  7:0 /* RWIVF */
#define NV_P2060_FPGA_EXREV_VAL_0                               0x00 /* RWI-V */

#define NV_P2060_FPGA_ASGN_ID_0                                 0x18 /* R--1R */
#define NV_P2060_FPGA_ASGN_ID_0_VAL                              7:0 /* RWIVF */
#define NV_P2060_FPGA_ASGN_ID_1                                 0x19 /* R--1R */
#define NV_P2060_FPGA_ASGN_ID_1_VAL                              7:0 /* RWIVF */
#define NV_P2060_FPGA_ASGN_ID_2                                 0x1A /* R--1R */
#define NV_P2060_FPGA_ASGN_ID_2_VAL                              7:0 /* RWIVF */
#define NV_P2060_FPGA_ASGN_ID_3                                 0x1B /* R--1R */
#define NV_P2060_FPGA_ASGN_ID_3_VAL                              7:0 /* RWIVF */

#define NV_P2060_FPGA_ASGN_ID(i)                               (0x18 + i)

#define NV_P2060_FRAME_CMPR_LOW                                 0x1D /* R--1R */
#define NV_P2060_FRAME_CMPR_LOW_VAL                              7:0 /* RWIVF */
#define NV_P2060_FRAME_CMPR_LOW_VAL_0                           0x00 /* RWI-V */

#define NV_P2060_FRAME_CMPR_MID                                 0x1E /* R--1R */
#define NV_P2060_FRAME_CMPR_MID_VAL                              7:0 /* RWIVF */
#define NV_P2060_FRAME_CMPR_MID_VAL_0                           0x00 /* RWI-V */

#define NV_P2060_FRAME_CMPR_HIGH                                0x1F /* R--1R */
#define NV_P2060_FRAME_CMPR_HIGH_VAL                             7:0 /* RWIVF */
#define NV_P2060_FRAME_CMPR_HIGH_VAL_0                          0x00 /* RWI-V */

#define NV_P2060_HS_FRAMERATE_LOW                               0x20 /* R--1R */
#define NV_P2060_HS_FRAMERATE_LOW_VAL                            7:0 /* RWIVF */
#define NV_P2060_HS_FRAMERATE_LOW_VAL_0                         0x00 /* RWI-V */

#define NV_P2060_HS_FRAMERATE_MID                               0x21 /* R--1R */
#define NV_P2060_HS_FRAMERATE_MID_VAL                            7:0 /* RWIVF */
#define NV_P2060_HS_FRAMERATE_MID_VAL_0                         0x00 /* RWI-V */

#define NV_P2060_HS_FRAMERATE_HIGH                              0x22 /* R--1R */
#define NV_P2060_HS_FRAMERATE_HIGH_VAL                           7:0 /* RWIVF */
#define NV_P2060_HS_FRAMERATE_HIGH_VAL_0                        0x00 /* RWI-V */

#define NV_P2060_MOSAIC_MODE                                    0x23 /* RW-1R */
#define NV_P2060_MOSAIC_MODE_TS                                  1:0 /* RWIVF */
#define NV_P2060_MOSAIC_MODE_TS_CONN0                           0x00 /* R---V */
#define NV_P2060_MOSAIC_MODE_TS_CONN1                           0x01 /* RW--V */
#define NV_P2060_MOSAIC_MODE_TS_CONN2                           0x02 /* RW--V */
#define NV_P2060_MOSAIC_MODE_TS_CONN3                           0x03 /* RW--V */
#define NV_P2060_MOSAIC_MODE_GROUP                               2:2 /* RWIVF */
#define NV_P2060_MOSAIC_MODE_GROUP_ZERO                         0x00 /* RW--V */
#define NV_P2060_MOSAIC_MODE_GROUP_ONE                          0x01 /* RW--V */
#define NV_P2060_MOSAIC_MODE_ENABLE                              3:3 /* RWIVF */
#define NV_P2060_MOSAIC_MODE_ENABLE_FALSE                       0x00 /* RW--V */
#define NV_P2060_MOSAIC_MODE_ENABLE_TRUE                        0x01 /* RW--V */
#define NV_P2060_MOSAIC_MODE_RESERVED                            7:4 /* RWIVF */

#define NV_P2060_MULTIPLIER_DIVIDER                             0x2F /* RW-1R */
#define NV_P2060_MULTIPLIER_DIVIDER_VALUE_MINUS_ONE              2:0 /* RWIVF */
#define NV_P2060_MULTIPLIER_DIVIDER_VALUE_MINUS_ONE_MAX          0x7
#define NV_P2060_MULTIPLIER_DIVIDER_MODE                         7:7 /* RWIVF */
#define NV_P2060_MULTIPLIER_DIVIDER_MODE_MULTIPLY                0x0 /* RWIVF */
#define NV_P2060_MULTIPLIER_DIVIDER_MODE_DIVIDE                  0x1 /* RWIVF */

#endif //DEV_P2060_H

