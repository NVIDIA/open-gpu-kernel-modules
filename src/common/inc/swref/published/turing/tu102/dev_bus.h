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

#ifndef __tu102_dev_bus_h__
#define __tu102_dev_bus_h__

#define NV_PBUS_VBIOS_SCRATCH(i)                  (0x00001400+(i)*4) /*       */

#define NV_PBUS_IFR_FMT_FIXED0                        0x00000000 /*       */
#define NV_PBUS_IFR_FMT_FIXED0_SIGNATURE                    31:0 /*       */
#define NV_PBUS_IFR_FMT_FIXED0_SIGNATURE_VALUE        0x4947564E /*       */
#define NV_PBUS_IFR_FMT_FIXED1                        0x00000004 /*       */
#define NV_PBUS_IFR_FMT_FIXED1_VERSIONSW                    15:8 /*       */
#define NV_PBUS_IFR_FMT_FIXED1_FIXED_DATA_SIZE             30:16 /*       */
#define NV_PBUS_IFR_FMT_FIXED2                        0x00000008 /*       */
#define NV_PBUS_IFR_FMT_FIXED2_TOTAL_DATA_SIZE              19:0 /*       */

#define NV_PBUS_BAR1_BLOCK                               0x00001704 /* RW-4R */
#define NV_PBUS_BAR1_BLOCK_MAP                                 29:0 /*       */
#define NV_PBUS_BAR1_BLOCK_PTR                                 27:0 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_PTR_0                         0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_TARGET                             29:28 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_TARGET_VID_MEM                0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_COHERENT       0x00000002 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_NONCOHERENT    0x00000003 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_MODE                               31:31 /* RWIUF */
#define NV_PBUS_BAR1_BLOCK_MODE_PHYSICAL                 0x00000000 /* RWI-V */
#define NV_PBUS_BAR1_BLOCK_MODE_VIRTUAL                  0x00000001 /* RW--V */
#define NV_PBUS_BAR1_BLOCK_PTR_SHIFT                             12 /*       */
#define NV_PBUS_BAR2_BLOCK                               0x00001714 /* RW-4R */
#define NV_PBUS_BAR2_BLOCK_MAP                                 29:0 /*       */
#define NV_PBUS_BAR2_BLOCK_PTR                                 27:0 /* RWIUF */
#define NV_PBUS_BAR2_BLOCK_PTR_0                         0x00000000 /* RWI-V */
#define NV_PBUS_BAR2_BLOCK_TARGET                             29:28 /* RWIUF */
#define NV_PBUS_BAR2_BLOCK_TARGET_VID_MEM                0x00000000 /* RWI-V */
#define NV_PBUS_BAR2_BLOCK_TARGET_SYS_MEM_COHERENT       0x00000002 /* RW--V */
#define NV_PBUS_BAR2_BLOCK_TARGET_SYS_MEM_NONCOHERENT    0x00000003 /* RW--V */
#define NV_PBUS_BAR2_BLOCK_DEBUG_CYA                          30:30 /* RWIUF */
#define NV_PBUS_BAR2_BLOCK_DEBUG_CYA_OFF                 0x00000001 /* RW--V */
#define NV_PBUS_BAR2_BLOCK_DEBUG_CYA_ON                  0x00000000 /* RW--V */
#define NV_PBUS_BAR2_BLOCK_DEBUG_CYA_INIT                0x00000001 /* RWI-V */
#define NV_PBUS_BAR2_BLOCK_MODE                               31:31 /* RWIUF */
#define NV_PBUS_BAR2_BLOCK_MODE_PHYSICAL                 0x00000000 /* RWI-V */
#define NV_PBUS_BAR2_BLOCK_MODE_VIRTUAL                  0x00000001 /* RW--V */
#define NV_PBUS_BAR2_BLOCK_PTR_SHIFT                             12 /*       */
#define NV_PBUS_BAR2_BLOCK_RESERVED                                  30:30 /*       */
#define NV_PBUS_BAR2_BLOCK_RESERVED_DEFAULT                          0x00000001 /*       */
#define NV_PBUS_BIND_STATUS_BAR1_PENDING                        0:0 /* R-IUF */
#define NV_PBUS_BIND_STATUS_BAR1_PENDING_EMPTY           0x00000000 /* R-I-V */
#define NV_PBUS_BIND_STATUS_BAR1_PENDING_BUSY            0x00000001 /* R---V */
#define NV_PBUS_BIND_STATUS_BAR1_OUTSTANDING                    1:1 /* R-IUF */
#define NV_PBUS_BIND_STATUS_BAR1_OUTSTANDING_FALSE       0x00000000 /* R-I-V */
#define NV_PBUS_BIND_STATUS_BAR1_OUTSTANDING_TRUE        0x00000001 /* R---V */
#define NV_PBUS_BIND_STATUS_BAR2_PENDING                        2:2 /* R-IUF */
#define NV_PBUS_BIND_STATUS_BAR2_PENDING_EMPTY           0x00000000 /* R-I-V */
#define NV_PBUS_BIND_STATUS_BAR2_PENDING_BUSY            0x00000001 /* R---V */
#define NV_PBUS_BIND_STATUS_BAR2_OUTSTANDING                    3:3 /* R-IUF */
#define NV_PBUS_BIND_STATUS_BAR2_OUTSTANDING_FALSE       0x00000000 /* R-I-V */
#define NV_PBUS_BIND_STATUS_BAR2_OUTSTANDING_TRUE        0x00000001 /* R---V */

#endif // __tu102_dev_bus_h__
