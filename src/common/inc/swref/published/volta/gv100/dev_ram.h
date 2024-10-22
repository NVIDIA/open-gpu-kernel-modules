/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gv100_dev_ram_h__
#define __gv100_dev_ram_h__
#define NV_PRAMIN                             0x007FFFFF:0x00700000 /* RW--M */
#define NV_RAMIN_BASE_SHIFT                                      12 /*       */
#define NV_RAMIN_ALLOC_SIZE                                    4096 /*       */
#define NV_RAMIN_ENABLE_ATS                        (135*32+31):(135*32+31) /* RWXUF */
#define NV_RAMIN_ENABLE_ATS_TRUE                                0x00000001 /* RW--V */
#define NV_RAMIN_ENABLE_ATS_FALSE                               0x00000000 /* RW--V */
#define NV_RAMIN_PASID                 (135*32+(20-1)):(135*32+0) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET(i)             ((168+(i)*4)*32+1):((168+(i)*4)*32+0) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET__SIZE_1                         64 /*       */
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_VID_MEM                  0x00000000 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_INVALID                  0x00000001 /* RW--V */ // Note: INVALID should match PEER
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_SYS_MEM_COHERENT         0x00000002 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_TARGET_SYS_MEM_NONCOHERENT      0x00000003 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_VOL(i)                  ((168+(i)*4)*32+2):((168+(i)*4)*32+2) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_VOL__SIZE_1                         64 /*       */
#define NV_RAMIN_SC_PAGE_DIR_BASE_VOL_TRUE                        0x00000001 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_VOL_FALSE                       0x00000000 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_TEX(i)     ((168+(i)*4)*32+4):((168+(i)*4)*32+4) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_TEX__SIZE_1                         64 /*       */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_TEX_DISABLED       0x00000000 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_TEX_ENABLED        0x00000001 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_GCC(i)     ((168+(i)*4)*32+5):((168+(i)*4)*32+5) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_GCC__SIZE_1                         64 /*       */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_GCC_DISABLED       0x00000000 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_FAULT_REPLAY_GCC_ENABLED        0x00000001 /* RW--V */
#define NV_RAMIN_SC_USE_VER2_PT_FORMAT(i)          ((168+(i)*4)*32+10):((168+(i)*4)*32+10) /* RWXUF */
#define NV_RAMIN_SC_USE_VER2_PT_FORMAT__SIZE_1                   64 /*       */
#define NV_RAMIN_SC_USE_VER2_PT_FORMAT_FALSE                       0x00000000 /* RW--V */
#define NV_RAMIN_SC_USE_VER2_PT_FORMAT_TRUE                        0x00000001 /* RW--V */
#define NV_RAMIN_SC_BIG_PAGE_SIZE(i)                    ((168+(i)*4)*32+11):((168+(i)*4)*32+11) /* RWXUF */
#define NV_RAMIN_SC_BIG_PAGE_SIZE__SIZE_1                   64 /*       */
#define NV_RAMIN_SC_BIG_PAGE_SIZE_64KB                            0x00000001 /* RW--V */
#define NV_RAMIN_SC_PAGE_DIR_BASE_LO(i)                ((168+(i)*4)*32+31):((168+(i)*4)*32+12) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_LO__SIZE_1                   64 /*       */
#define NV_RAMIN_SC_PAGE_DIR_BASE_HI(i)                 ((169+(i)*4)*32+31):((169+(i)*4)*32+0) /* RWXUF */
#define NV_RAMIN_SC_PAGE_DIR_BASE_HI__SIZE_1                   64 /*       */
#define NV_RAMIN_SC_ENABLE_ATS(i)                       ((170+(i)*4)*32+31):((170+(i)*4)*32+31) /* RWXUF */
#define NV_RAMIN_SC_PASID(i)                       ((170+(i)*4)*32+(20-1)):((170+(i)*4)*32+0) /* RWXUF */
#define NV_RAMRL_ENTRY_TSG_TIMESLICE_SCALE_3                      0x00000003 /* RWI-V */
#define NV_RAMRL_ENTRY_TSG_TIMESLICE_TIMEOUT_128                  0x00000080 /* RWI-V */
#define NV_RAMRL_ENTRY_TSG_LENGTH_MAX                             0x00000080 /* RW--V */
#define NV_RAMRL_ENTRY_SIZE                                       16 /*       */
#endif // __gv100_dev_ram_h__
