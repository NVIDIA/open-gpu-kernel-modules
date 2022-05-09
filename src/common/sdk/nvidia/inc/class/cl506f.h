/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl506f_h_
#define _cl506f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/* class NV50_CHANNEL_GPFIFO */
#define  NV50_CHANNEL_GPFIFO                                       (0x0000506F)

/* NvNotification[] elements */
#define NV506F_NOTIFIERS_RC                                        (0)
#define NV506F_NOTIFIERS_SW                                        (1)
#define NV506F_NOTIFIERS_GR_DEBUG_INTR                             (2)
#define NV506F_NOTIFIERS_MAXCOUNT                                  (3)

/* NvNotification[] fields and values */
#define NV506f_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT              (0x2000)
#define NV506f_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT          (0x4000)
/* pio method data structure */
typedef volatile struct _cl506f_tag0 {
 NvV32 Reserved00[0x7c0];
} Nv506fTypedef, Nv50ChannelGPFifo;
#define NV506F_TYPEDEF                                        Nv50ChannelGPFifo
/* pio flow control data structure */
typedef volatile struct _cl506f_tag1 {
 NvU32 Ignored00[0x010];        /*                                  0000-0039*/
 NvU32 Put;                     /* put offset, read/write           0040-0043*/
 NvU32 Get;                     /* get offset, read only            0044-0047*/
 NvU32 Reference;               /* reference value, read only       0048-004b*/
 NvU32 PutHi;                   /* high order put offset bits       004c-004f*/
 NvU32 SetReference;            /* set reference value              0050-0053*/
 NvU32 Ignored02[0x001];        /*                                  0054-0057*/
 NvU32 TopLevelGet;             /* top level get offset, read only  0058-005b*/
 NvU32 TopLevelGetHi;           /* high order top level get bits    005c-005f*/
 NvU32 GetHi;                   /* high order get offset bits       0060-0063*/
 NvU32 Ignored03[0x007];        /*                                  0064-007f*/
 NvU32 Yield;                   /* engine yield, write only         0080-0083*/
 NvU32 Ignored04[0x001];        /*                                  0084-0087*/
 NvU32 GPGet;                   /* GP FIFO get offset, read only    0088-008b*/
 NvU32 GPPut;                   /* GP FIFO put offset               008c-008f*/
 NvU32 Ignored05[0x3dc];
} Nv506fControl, Nv50ControlGPFifo;
/* fields and values */
#define NV506F_NUMBER_OF_SUBCHANNELS                               (8)
#define NV506F_SET_OBJECT                                          (0x00000000)
#define NV506F_SET_REFERENCE                                       (0x00000050)
#define NV506F_SET_CONTEXT_DMA_SEMAPHORE                           (0x00000060)
#define NV506F_SEMAPHORE_OFFSET                                    (0x00000064)
#define NV506F_SEMAPHORE_ACQUIRE                                   (0x00000068)
#define NV506F_SEMAPHORE_RELEASE                                   (0x0000006c)
#define NV506F_YIELD                                               (0x00000080)

//
// GPFIFO entry format
//
#define NV506F_GP_ENTRY__SIZE                                   8
#define NV506F_GP_ENTRY0_DISABLE                              0:0
#define NV506F_GP_ENTRY0_DISABLE_NOT                   0x00000000
#define NV506F_GP_ENTRY0_DISABLE_SKIP                  0x00000001
#define NV506F_GP_ENTRY0_NO_CONTEXT_SWITCH                    1:1
#define NV506F_GP_ENTRY0_NO_CONTEXT_SWITCH_FALSE       0x00000000
#define NV506F_GP_ENTRY0_NO_CONTEXT_SWITCH_TRUE        0x00000001
#define NV506F_GP_ENTRY0_GET                                 31:2
#define NV506F_GP_ENTRY1_GET_HI                               7:0
#define NV506F_GP_ENTRY1_PRIV                                 8:8
#define NV506F_GP_ENTRY1_PRIV_USER                     0x00000000
#define NV506F_GP_ENTRY1_PRIV_KERNEL                   0x00000001
#define NV506F_GP_ENTRY1_LEVEL                                9:9
#define NV506F_GP_ENTRY1_LEVEL_MAIN                    0x00000000
#define NV506F_GP_ENTRY1_LEVEL_SUBROUTINE              0x00000001
#define NV506F_GP_ENTRY1_LENGTH                             31:10

/* dma method descriptor formats */
#define NV506F_DMA_PRIMARY_OPCODE                                  1:0
#define NV506F_DMA_PRIMARY_OPCODE_USES_SECONDARY                   (0x00000000)
#define NV506F_DMA_PRIMARY_OPCODE_RESERVED                         (0x00000003)
#define NV506F_DMA_METHOD_ADDRESS                                  12:2
#define NV506F_DMA_METHOD_SUBCHANNEL                               15:13
#define NV506F_DMA_TERT_OP                                         17:16
#define NV506F_DMA_TERT_OP_GRP0_INC_METHOD                         (0x00000000)
#define NV506F_DMA_TERT_OP_GRP0_SET_SUB_DEV_MASK                   (0x00000001)
#define NV506F_DMA_TERT_OP_GRP0_DOUBLE_HEADER                      (0x00000003)
#define NV506F_DMA_TERT_OP_GRP2_NON_INC_METHOD                     (0x00000000)
#define NV506F_DMA_TERT_OP_GRP2_RESERVED01                         (0x00000001)
#define NV506F_DMA_TERT_OP_GRP2_RESERVED10                         (0x00000002)
#define NV506F_DMA_TERT_OP_GRP2_RESERVED11                         (0x00000003)
#define NV506F_DMA_METHOD_COUNT                                    28:18
#define NV506F_DMA_SEC_OP                                          31:29
#define NV506F_DMA_SEC_OP_GRP0_USE_TERT                            (0x00000000)
#define NV506F_DMA_SEC_OP_GRP2_USE_TERT                            (0x00000002)
#define NV506F_DMA_SEC_OP_GRP3_RESERVED                            (0x00000003)
#define NV506F_DMA_SEC_OP_GRP4_RESERVED                            (0x00000004)
#define NV506F_DMA_SEC_OP_GRP5_RESERVED                            (0x00000005)
#define NV506F_DMA_SEC_OP_GRP6_RESERVED                            (0x00000006)
#define NV506F_DMA_SEC_OP_GRP7_RESERVED                            (0x00000007)
#define NV506F_DMA_LONG_COUNT                                      31:0 
/* dma legacy method descriptor format */
#define NV506F_DMA_OPCODE2                                         1:0
#define NV506F_DMA_OPCODE2_NONE                                    (0x00000000)
#define NV506F_DMA_OPCODE                                          31:29
#define NV506F_DMA_OPCODE_METHOD                                   (0x00000000)
#define NV506F_DMA_OPCODE_NONINC_METHOD                            (0x00000002)
#define NV506F_DMA_OPCODE3_NONE                                    (0x00000000)
/* dma data format */
#define NV506F_DMA_DATA                                            31:0
/* dma double header descriptor format */
#define NV506F_DMA_DH_OPCODE2                                      1:0
#define NV506F_DMA_DH_OPCODE2_NONE                                 (0x00000000)
#define NV506F_DMA_DH_METHOD_ADDRESS                               12:2
#define NV506F_DMA_DH_METHOD_SUBCHANNEL                            15:13
#define NV506F_DMA_DH_OPCODE3                                      17:16
#define NV506F_DMA_DH_OPCODE3_DOUBLE_HEADER                        (0x00000003)
#define NV506F_DMA_DH_OPCODE                                       31:29
#define NV506F_DMA_DH_OPCODE_METHOD                                (0x00000000)
/* dma double header method count format */
#define NV506F_DMA_DH_METHOD_COUNT                                 23:0
/* dma double header data format */
#define NV506F_DMA_DH_DATA                                         31:0
/* dma nop format */
#define NV506F_DMA_NOP                                             (0x00000000)
/* dma set subdevice mask format */
#define NV506F_DMA_SET_SUBDEVICE_MASK                              (0x00010000)
#define NV506F_DMA_SET_SUBDEVICE_MASK_VALUE                        15:4
#define NV506F_DMA_OPCODE3                                         17:16
#define NV506F_DMA_OPCODE3_SET_SUBDEVICE_MASK                      (0x00000001)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl506f_h_ */
