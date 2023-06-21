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
#ifndef __gh100_dev_fsp_pri_h__
#define __gh100_dev_fsp_pri_h__

#define NV_PFSP_EMEMC(i)                                                                                 (0x008F2ac0+(i)*8) /* RW-4A */
#define NV_PFSP_EMEMC__SIZE_1                                                                            8              /*       */
#define NV_PFSP_EMEMC_OFFS                                                                               7:2            /* RWIVF */
#define NV_PFSP_EMEMC_OFFS_INIT                                                                          0x00000000     /* RWI-V */
#define NV_PFSP_EMEMC_BLK                                                                                15:8           /* RWIVF */
#define NV_PFSP_EMEMC_BLK_INIT                                                                           0x00000000     /* RWI-V */
#define NV_PFSP_EMEMC_AINCW                                                                              24:24          /* RWIVF */
#define NV_PFSP_EMEMC_AINCW_INIT                                                                         0x00000000     /* RWI-V */
#define NV_PFSP_EMEMC_AINCW_TRUE                                                                         0x00000001     /* RW--V */
#define NV_PFSP_EMEMC_AINCW_FALSE                                                                        0x00000000     /* RW--V */
#define NV_PFSP_EMEMC_AINCR                                                                              25:25          /* RWIVF */
#define NV_PFSP_EMEMC_AINCR_INIT                                                                         0x00000000     /* RWI-V */
#define NV_PFSP_EMEMC_AINCR_TRUE                                                                         0x00000001     /* RW--V */
#define NV_PFSP_EMEMC_AINCR_FALSE                                                                        0x00000000     /* RW--V */
#define NV_PFSP_EMEMD(i)                                                                                 (0x008F2ac4+(i)*8) /* RW-4A */
#define NV_PFSP_EMEMD__SIZE_1                                                                            8              /*       */
#define NV_PFSP_EMEMD_DATA                                                                               31:0           /* RWXVF */

#define NV_PFSP_MSGQ_HEAD(i)                                                                             (0x008F2c80+(i)*8) /* RW-4A */
#define NV_PFSP_MSGQ_HEAD__SIZE_1                                                                        8              /*       */
#define NV_PFSP_MSGQ_HEAD_VAL                                                                            31:0           /* RWIUF */
#define NV_PFSP_MSGQ_HEAD_VAL_INIT                                                                       0x00000000     /* RWI-V */
#define NV_PFSP_MSGQ_TAIL(i)                                                                             (0x008F2c84+(i)*8) /* RW-4A */
#define NV_PFSP_MSGQ_TAIL__SIZE_1                                                                        8              /*       */
#define NV_PFSP_MSGQ_TAIL_VAL                                                                            31:0           /* RWIUF */
#define NV_PFSP_MSGQ_TAIL_VAL_INIT                                                                       0x00000000     /* RWI-V */

#define NV_PFSP_QUEUE_HEAD(i)                                                                            (0x008F2c00+(i)*8) /* RW-4A */
#define NV_PFSP_QUEUE_HEAD__SIZE_1                                                                       8              /*       */
#define NV_PFSP_QUEUE_HEAD_ADDRESS                                                                       31:0           /* RWIVF */
#define NV_PFSP_QUEUE_HEAD_ADDRESS_INIT                                                                  0x00000000     /* RWI-V */
#define NV_PFSP_QUEUE_TAIL(i)                                                                            (0x008F2c04+(i)*8) /* RW-4A */
#define NV_PFSP_QUEUE_TAIL__SIZE_1                                                                       8              /*       */
#define NV_PFSP_QUEUE_TAIL_ADDRESS                                                                       31:0           /* RWIVF */
#define NV_PFSP_QUEUE_TAIL_ADDRESS_INIT                                                                  0x00000000     /* RWI-V */

#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(i)                                                         (0x008f0320+(i)*4) /* RW-4A */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2__SIZE_1                                                    4              /*       */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2__DEVICE_MAP                                                0x00000016 /*       */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2_VAL                                                        31:0           /* RWIVF */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2_VAL_INIT                                                   0x00000000     /* RWI-V */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_3(i)                                                         (0x008f0330+(i)*4) /* RW-4A */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_3__SIZE_1                                                    4              /*       */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_3_VAL                                                        31:0           /* RWIVF */
#define NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_3_VAL_INIT                                                   0x00000000     /* RWI-V */

#endif // __gh100_dev_fsp_pri_h__
