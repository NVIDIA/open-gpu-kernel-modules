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

#ifndef __v03_00_dev_disp_h__
#define __v03_00_dev_disp_h__
#define NV_PDISP_CHN_NUM_CORE                                                           0 /*       */
#define NV_PDISP_CHN_NUM_WIN(i)                                                   (1+(i)) /*       */
#define NV_PDISP_CHN_NUM_WIN__SIZE_1                                                   32 /*       */
#define NV_PDISP_CHN_NUM_WINIM(i)                                                (33+(i)) /*       */
#define NV_PDISP_CHN_NUM_WINIM__SIZE_1                                                 32 /*       */
#define NV_PDISP_CHN_NUM_CURS(i)                                                 (73+(i)) /*       */
#define NV_PDISP_CHN_NUM_CURS__SIZE_1                                                   8 /*       */
#define NV_PDISP_FE_HW_SYS_CAP_HEAD_EXISTS(i)                             (0+(i)):(0+(i)) /* R--VF */
#define NV_PDISP_FE_HW_SYS_CAP_HEAD_EXISTS__SIZE_1                                      8 /*       */
#define NV_PDISP_FE_HW_SYS_CAP_HEAD_EXISTS_YES                                 0x00000001 /* R---V */
#define NV_PDISP_FE_SW                                              0x00640FFF:0x00640000 /* RW--D */
#define NV_PDISP_SF_USER_0                                          0x006F03FF:0x006F0000 /* RW--D */
#define NV_UDISP_HASH_BASE                                                     0x00000000 /*       */
#define NV_UDISP_HASH_LIMIT                                                    0x00001FFF /*       */
#define NV_UDISP_OBJ_MEM_BASE                                                  0x00002000 /*       */
#define NV_UDISP_OBJ_MEM_LIMIT                                                 0x0000FFFF /*       */
#define NV_UDISP_HASH_TBL_CLIENT_ID                                    (1*32+13):(1*32+0) /* RWXVF */
#define NV_UDISP_HASH_TBL_INSTANCE                                    (1*32+24):(1*32+14) /* RWXUF */
#define NV_UDISP_HASH_TBL_CHN                                         (1*32+31):(1*32+25) /* RWXUF */
#define NV_DMA_TARGET_NODE                                              (0*32+1):(0*32+0) /* RWXVF */
#define NV_DMA_TARGET_NODE_PHYSICAL_NVM                                        0x00000001 /* RW--V */
#define NV_DMA_TARGET_NODE_PHYSICAL_PCI                                        0x00000002 /* RW--V */
#define NV_DMA_TARGET_NODE_PHYSICAL_PCI_COHERENT                               0x00000003 /* RW--V */
#define NV_DMA_ACCESS                                                   (0*32+2):(0*32+2) /* RWXVF */
#define NV_DMA_ACCESS_READ_ONLY                                                0x00000000 /* RW--V */
#define NV_DMA_ACCESS_READ_AND_WRITE                                           0x00000001 /* RW--V */
#define NV_DMA_KIND                                                   (0*32+20):(0*32+20) /* RWXVF */
#define NV_DMA_KIND_PITCH                                                      0x00000000 /* RW--V */
#define NV_DMA_KIND_BLOCKLINEAR                                                0x00000001 /* RW--V */
#define NV_DMA_ADDRESS_BASE_LO                                         (1*32+31):(1*32+0) /* RWXUF */
#define NV_DMA_ADDRESS_BASE_HI                                          (2*32+6):(2*32+0) /* RWXUF */
#define NV_DMA_ADDRESS_LIMIT_LO                                        (3*32+31):(3*32+0) /* RWXUF */
#define NV_DMA_ADDRESS_LIMIT_HI                                         (4*32+6):(4*32+0) /* RWXUF */
#define NV_DMA_SIZE                                                                    20 /*       */
#define NV_UDISP_FE_CHN_ASSY_BASEADR_CORE                                      0x00680000 /*       */
#define NV_UDISP_FE_CHN_ARMED_BASEADR_CORE                             (0x00680000+32768) /*       */
#define NV_UDISP_FE_CHN_ASSY_BASEADR_WIN(i)                       ((0x00690000+(i)*4096)) /*       */
#define NV_UDISP_FE_CHN_ASSY_BASEADR_WINIM(i)                ((0x00690000+((i+32)*4096))) /*       */
#define NV_UDISP_FE_CHN_ASSY_BASEADR_CURS(i)                        (0x006D8000+(i)*4096) /* RW-4A */
#define NV_UDISP_FE_CHN_ASSY_BASEADR(i)    ((i)>0?(((0x00690000+(i-1)*4096))):0x00680000) /*       */
#define NV_UDISP_FE_CHN_ASSY_BASEADR__SIZE_1                                           81 /*       */
#define NV_PDISP_RG_DPCA(i)                                         (0x00616330+(i)*2048) /* R--4A */
#define NV_PDISP_RG_DPCA__SIZE_1                                                        8 /*       */
#define NV_PDISP_RG_DPCA_LINE_CNT                                                    15:0 /* R--UF */
#define NV_PDISP_RG_DPCA_FRM_CNT                                                    31:16 /* R--UF */
#define NV_PDISP_FE_FLIPLOCK                                                   0x0061206C /* RW-4R */
#define NV_PDISP_FE_FLIPLOCK_LSR_MIN_TIME                                            23:0 /* RWIVF */

#define NV_PDISP_FE_RM_INTR_DISPATCH                                          0x00611EC0 /* R--4R */

#define NV_PDISP_FE_RM_INTR_DISPATCH_HEAD_TIMING(i)                                      (0+(i)):(0+(i)) /*       */
#define NV_PDISP_FE_RM_INTR_DISPATCH_HEAD_TIMING__SIZE_1                                               8 /*       */
#define NV_PDISP_FE_RM_INTR_DISPATCH_HEAD_TIMING_NOT_PENDING                                  0x00000000 /*       */
#define NV_PDISP_FE_RM_INTR_DISPATCH_HEAD_TIMING_PENDING                                      0x00000001 /*       */

#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING(i)                                       (0x00611C00+(i)*4) /* R--4A */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING__SIZE_1                                                   8 /*       */

#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_LAST_DATA                                               1:1 /* R-IVF */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_LAST_DATA_NOT_PENDING                            0x00000000 /* R-I-V */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_LAST_DATA_PENDING                                0x00000001 /* R---V */

#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_A                                               5:5 /* R-IVF */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_A_NOT_PENDING                            0x00000000 /* R-I-V */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_A_PENDING                                0x00000001 /* R---V */

#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_B                                               6:6 /* R-IVF */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_B_NOT_PENDING                            0x00000000 /* R-I-V */
#define NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING_RG_LINE_B_PENDING                                0x00000001 /* R---V */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING(i)                                           (0x00611800+(i)*4) /* RW-4A */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING__SIZE_1                                                       8 /*       */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_LAST_DATA                                                   1:1 /* RWIVF */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_LAST_DATA_INIT                                       0x00000000 /* R-I-V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_LAST_DATA_NOT_PENDING                                0x00000000 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_LAST_DATA_PENDING                                    0x00000001 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_LAST_DATA_RESET                                      0x00000001 /* -W--V */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_A                                                   5:5 /* RWIVF */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_A_INIT                                       0x00000000 /* R-I-V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_A_NOT_PENDING                                0x00000000 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_A_PENDING                                    0x00000001 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_A_RESET                                      0x00000001 /* -W--V */

#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_B                                                   6:6 /* RWIVF */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_B_INIT                                       0x00000000 /* R-I-V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_B_NOT_PENDING                                0x00000000 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_B_PENDING                                    0x00000001 /* R---V */
#define NV_PDISP_FE_EVT_STAT_HEAD_TIMING_RG_LINE_B_RESET                                      0x00000001 /* -W--V */

#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP                                     0x00611C30 /* R--4R */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_AWAKEN                                     8:8 /* R-IVF */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_AWAKEN_NOT_PENDING                  0x00000000 /* R-I-V */
#define NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP_AWAKEN_PENDING                      0x00000001 /* R---V */

#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER(i)                    (0x0061611C+(i)*2048) /* RW-4A */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER__SIZE_1                                   8 /*       */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER_VALUE                                  31:0 /* RWIUF */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER_VALUE_INIT                       0x00000000 /* RWI-V */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER_VALUE_ZERO                       0x00000000 /* RW--V */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER_VALUE_HW                         0x00000000 /* R---V */
#define NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER_VALUE_SW                         0x00000000 /* -W--V */

#endif // __v03_00_dev_disp_h__
