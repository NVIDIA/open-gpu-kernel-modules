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

#ifndef __gr100_dev_gsp_h__
#define __gr100_dev_gsp_h__

#define NV_PGSP_FALCON_MAILBOX0                                                                          0x110040       /* RW-4R */
#define NV_PGSP_FALCON_MAILBOX0_DATA                                                                     31:0           /* RWIVF */
#define NV_PGSP_FALCON_MAILBOX0_DATA_INIT                                                                0x00000000     /* RWI-V */

#define NV_PGSP_EC_FEATURE                                                                               0x00110700     /* R--4R */
#define NV_PGSP_EC_FEATURE_NUM_ERR                                                                            31:16     /* R-IVF */
#define NV_PGSP_EC_FEATURE_NUM_ERR_INIT                                                                      0x0012     /* RWI-V */
#define NV_PGSP_EC_FEATURE_NUM_ERR_SLICES                                                                       5:0     /* R-IVF */
#define NV_PGSP_EC_FEATURE_NUM_ERR_SLICES_INIT                                                                 0x01     /* RWI-V */

#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS                                                           0x00110738     /* RW-4R */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR18                                                          18:18     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR18_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR17                                                          17:17     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR17_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR16                                                          16:16     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR16_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR15                                                          15:15     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR15_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR14                                                          14:14     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR14_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR13                                                          13:13     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR13_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR12                                                          12:12     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR12_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR11                                                          11:11     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR11_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR10                                                          10:10     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR10_INIT                                                       0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR9                                                             9:9     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR9_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR8                                                             8:8     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR8_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR7                                                             7:7     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR7_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR6                                                             6:6     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR6_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR5                                                             5:5     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR5_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR4                                                             4:4     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR4_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR3                                                             3:3     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR3_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR2                                                             2:2     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR2_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR1                                                             1:1     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR1_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR0                                                             0:0     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_MISSIONERR_STATUS_ERR0_INIT                                                        0x0     /* RWI-V */

#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS                                                            0x00110748     /* RW-4R */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR18                                                           18:18     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR18_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR17                                                           17:17     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR17_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR16                                                           16:16     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR16_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR15                                                           15:15     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR15_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR14                                                           14:14     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR14_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR13                                                           13:13     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR13_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR12                                                           12:12     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR12_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR11                                                           11:11     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR11_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR10                                                           10:10     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR10_INIT                                                        0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR9                                                              9:9     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR9_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR8                                                              8:8     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR8_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR7                                                              7:7     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR7_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR6                                                              6:6     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR6_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR5                                                              5:5     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR5_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR4                                                              4:4     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR4_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR3                                                              3:3     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR3_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR2                                                              2:2     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR2_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR1                                                              1:1     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR1_INIT                                                         0x0     /* RWI-V */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR0                                                              0:0     /* RWIVF */
#define NV_PGSP_EC_ERRSLICE0_LATENTERR_STATUS_ERR0_INIT                                                         0x0     /* RWI-V */

#endif // __gr100_dev_gsp_h__
