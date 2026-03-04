/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gm107_dev_ram_h__
#define __gm107_dev_ram_h__
#define NV_PRAMIN                             0x007FFFFF:0x00700000 /* RW--M */
#define NV_PRAMIN_DATA032(i)                     (0x00700000+(i)*4) /* RW-4A */
#define NV_PRAMIN_DATA032__SIZE_1                            524288 /*       */
#define NV_PRAMIN_DATA032_VALUE                                31:0 /* RWXUF */
#define NV_PRAMIN_DATA016(i)         (0x00700000+((i)/3)*4+((i)%3)) /* RW-2A */
#define NV_PRAMIN_DATA016__SIZE_1                           1572864 /*       */
#define NV_PRAMIN_DATA016_VALUE                                15:0 /* RWXUF */
#define NV_PRAMIN_DATA008(i)                       (0x00700000+(i)) /* RW-1A */
#define NV_PRAMIN_DATA008__SIZE_1                           2097152 /*       */
#define NV_PRAMIN_DATA008_VALUE                                 7:0 /* RWXUF */
#define NV_RAMIN_BASE_SHIFT                                      12 /*       */
#define NV_RAMIN_ALLOC_SIZE                                    4096 /*       */
#define NV_RAMIN_RAMFC                         (127*32+31):(0*32+0) /* RWXUF */
#define NV_RAMUSERD_PUT                        (16*32+31):(16*32+0) /* RWXUF */
#define NV_RAMUSERD_GET                        (17*32+31):(17*32+0) /* RWXUF */
#define NV_RAMUSERD_REF                        (18*32+31):(18*32+0) /* RWXUF */
#define NV_RAMUSERD_PUT_HI                     (19*32+31):(19*32+0) /* RWXUF */
#define NV_RAMUSERD_REF_THRESHOLD              (20*32+31):(20*32+0) /*       */
#define NV_RAMUSERD_TOP_LEVEL_GET              (22*32+31):(22*32+0) /* RWXUF */
#define NV_RAMUSERD_TOP_LEVEL_GET_HI           (23*32+31):(23*32+0) /* RWXUF */
#define NV_RAMUSERD_GET_HI                     (24*32+31):(24*32+0) /* RWXUF */
#define NV_RAMUSERD_GP_GET                     (34*32+31):(34*32+0) /* RWXUF */
#define NV_RAMUSERD_GP_PUT                     (35*32+31):(35*32+0) /* RWXUF */
#define NV_RAMUSERD_BASE_SHIFT                                    9 /*       */
#define NV_RAMUSERD_CHAN_SIZE                                   512 /*       */
#define NV_RAMUSERD_GP_TOP_LEVEL_GET           (22*32+31):(22*32+0) /* RWXUF */
#define NV_RAMUSERD_GP_TOP_LEVEL_GET_HI        (23*32+31):(23*32+0) /* RWXUF */
#define NV_RAMRL_BASE_SHIFT                                      12 /*       */
#define NV_RAMRL_ENTRY_TIMESLICE_SCALE_3                 0x00000003 /* RWI-V */
#define NV_RAMRL_ENTRY_TIMESLICE_TIMEOUT_128             0x00000080 /* RWI-V */
#define NV_RAMRL_ENTRY_SIZE                                       8 /*       */
#define NV_RAMRL_ENTRY_TSG_LENGTH_MAX                    0x00000020 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_TARGET               (128*32+1):(128*32+0) /* RWXUF */
#define NV_RAMIN_PAGE_DIR_BASE_TARGET_VID_MEM                  0x00000000 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_TARGET_SYS_MEM_COHERENT         0x00000002 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_TARGET_SYS_MEM_NONCOHERENT      0x00000003 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_VOL                  (128*32+2):(128*32+2) /* RWXUF */
#define NV_RAMIN_PAGE_DIR_BASE_VOL_TRUE                        0x00000001 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_VOL_FALSE                       0x00000000 /* RW--V */
#define NV_RAMIN_PAGE_DIR_BASE_LO                 (128*32+31):(128*32+12) /* RWXUF */
#define NV_RAMIN_PAGE_DIR_BASE_HI                   (129*32+7):(129*32+0) /* RWXUF */
#define NV_RAMIN_ADR_LIMIT_LO                     (130*32+31):(130*32+12) /* RWXUF */
#define NV_RAMIN_ADR_LIMIT_HI                       (131*32+7):(131*32+0) /* RWXUF */
#endif // __gm107_dev_ram_h__
