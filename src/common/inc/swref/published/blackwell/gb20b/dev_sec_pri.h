/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES
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
#ifndef __gb20b_dev_sec_pri_h__
#define __gb20b_dev_sec_pri_h__

#define NV_PSEC_EMEMC(i)                                               (0x00840ac0+(i)*8) /* RW-4A */
#define NV_PSEC_EMEMC__SIZE_1                                                           8 /*       */
#define NV_PSEC_EMEMC_OFFS                                                            7:2 /* RWIVF */
#define NV_PSEC_EMEMC_OFFS_INIT                                                      0x00 /* RWI-V */
#define NV_PSEC_EMEMC_BLK                                                            15:8 /* RWIVF */
#define NV_PSEC_EMEMC_BLK_INIT                                                       0x00 /* RWI-V */
#define NV_PSEC_EMEMC_AINCW                                                         24:24 /* RWIVF */
#define NV_PSEC_EMEMC_AINCW_INIT                                                      0x0 /* RWI-V */
#define NV_PSEC_EMEMC_AINCW_TRUE                                                      0x1 /* RW--V */
#define NV_PSEC_EMEMC_AINCW_FALSE                                                     0x0 /* RW--V */
#define NV_PSEC_EMEMC_AINCR                                                         25:25 /* RWIVF */
#define NV_PSEC_EMEMC_AINCR_INIT                                                      0x0 /* RWI-V */
#define NV_PSEC_EMEMC_AINCR_TRUE                                                      0x1 /* RW--V */
#define NV_PSEC_EMEMC_AINCR_FALSE                                                     0x0 /* RW--V */
#define NV_PSEC_EMEMD(i)                                               (0x00840ac4+(i)*8) /* RW-4A */
#define NV_PSEC_EMEMD__SIZE_1                                                           8 /*       */
#define NV_PSEC_EMEMD_DATA                                                           31:0 /* RWXVF */

#define NV_PSEC_QUEUE_HEAD(i)                                          (0x00840c00+(i)*8) /* RW-4A */
#define NV_PSEC_QUEUE_HEAD__SIZE_1                                                      8 /*       */
#define NV_PSEC_QUEUE_HEAD_ADDRESS                                                   31:0 /* RWIVF */
#define NV_PSEC_QUEUE_HEAD_ADDRESS_INIT                                        0x00000000 /* RWI-V */
#define NV_PSEC_QUEUE_TAIL(i)                                          (0x00840c04+(i)*8) /* RW-4A */
#define NV_PSEC_QUEUE_TAIL__SIZE_1                                                      8 /*       */
#define NV_PSEC_QUEUE_TAIL_ADDRESS                                                   31:0 /* RWIVF */
#define NV_PSEC_QUEUE_TAIL_ADDRESS_INIT                                        0x00000000 /* RWI-V */

#define NV_PSEC_MSGQ_HEAD(i)                                           (0x00840c80+(i)*8) /* RW-4A */
#define NV_PSEC_MSGQ_HEAD__SIZE_1                                                       8 /*       */
#define NV_PSEC_MSGQ_HEAD_VAL                                                        31:0 /* RWIUF */
#define NV_PSEC_MSGQ_HEAD_VAL_INIT                                             0x00000000 /* RWI-V */
#define NV_PSEC_MSGQ_TAIL(i)                                           (0x00840c84+(i)*8) /* RW-4A */
#define NV_PSEC_MSGQ_TAIL__SIZE_1                                                       8 /*       */
#define NV_PSEC_MSGQ_TAIL_VAL                                                        31:0 /* RWIUF */
#define NV_PSEC_MSGQ_TAIL_VAL_INIT                                             0x00000000 /* RWI-V */

#endif // __gb20b_dev_sec_pri_h__
