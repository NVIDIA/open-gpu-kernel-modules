/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_clc86f_h__
#define __gh100_clc86f_h__

typedef volatile struct Nvc86fControl_struct {
 NvU32 Ignored00[0x010];        /*                                  0000-003f*/
 NvU32 Put;                     /* put offset, read/write           0040-0043*/
 NvU32 Get;                     /* get offset, read only            0044-0047*/
 NvU32 Reference;               /* reference value, read only       0048-004b*/
 NvU32 PutHi;                   /* high order put offset bits       004c-004f*/
 NvU32 Ignored01[0x002];        /*                                  0050-0057*/
 NvU32 TopLevelGet;             /* top level get offset, read only  0058-005b*/
 NvU32 TopLevelGetHi;           /* high order top level get bits    005c-005f*/
 NvU32 GetHi;                   /* high order get offset bits       0060-0063*/
 NvU32 Ignored02[0x007];        /*                                  0064-007f*/
 NvU32 Ignored03;               /* used to be engine yield          0080-0083*/
 NvU32 Ignored04[0x001];        /*                                  0084-0087*/
 NvU32 GPGet;                   /* GP FIFO get offset, read only    0088-008b*/
 NvU32 GPPut;                   /* GP FIFO put offset               008c-008f*/
 NvU32 Ignored05[0x5c];
} Nvc86fControl, HopperAControlGPFifo;

#define  HOPPER_CHANNEL_GPFIFO_A                           (0x0000C86F)
#define NVC86F_SET_OBJECT                                          (0x00000000)
#define NVC86F_SEM_ADDR_LO                                         (0x0000005c)
#define NVC86F_SEM_ADDR_LO_OFFSET                                         31:2
#define NVC86F_SEM_ADDR_HI                                         (0x00000060)
#define NVC86F_SEM_ADDR_HI_OFFSET                                         24:0
#define NVC86F_SEM_PAYLOAD_LO                                      (0x00000064)
#define NVC86F_SEM_PAYLOAD_HI                                      (0x00000068)
#define NVC86F_SEM_EXECUTE                                         (0x0000006c)
#define NVC86F_SEM_EXECUTE_OPERATION                                       2:0
#define NVC86F_SEM_EXECUTE_OPERATION_RELEASE                        0x00000001
#define NVC86F_SEM_EXECUTE_RELEASE_WFI                                   20:20
#define NVC86F_SEM_EXECUTE_RELEASE_WFI_DIS                          0x00000000
#define NVC86F_SEM_EXECUTE_PAYLOAD_SIZE                                  24:24
#define NVC86F_SEM_EXECUTE_PAYLOAD_SIZE_32BIT                       0x00000000

/* GPFIFO entry format */
#define NVC86F_GP_ENTRY__SIZE                                          8
#define NVC86F_GP_ENTRY0_FETCH                                       0:0
#define NVC86F_GP_ENTRY0_FETCH_UNCONDITIONAL                  0x00000000
#define NVC86F_GP_ENTRY0_FETCH_CONDITIONAL                    0x00000001
#define NVC86F_GP_ENTRY0_GET                                        31:2
#define NVC86F_GP_ENTRY0_OPERAND                                    31:0
#define NVC86F_GP_ENTRY0_PB_EXTENDED_BASE_OPERAND                   24:8
#define NVC86F_GP_ENTRY1_GET_HI                                      7:0
#define NVC86F_GP_ENTRY1_LEVEL                                       9:9
#define NVC86F_GP_ENTRY1_LEVEL_MAIN                           0x00000000
#define NVC86F_GP_ENTRY1_LEVEL_SUBROUTINE                     0x00000001
#define NVC86F_GP_ENTRY1_LENGTH                                    30:10
#define NVC86F_GP_ENTRY1_SYNC                                      31:31
#define NVC86F_GP_ENTRY1_SYNC_PROCEED                         0x00000000
#define NVC86F_GP_ENTRY1_SYNC_WAIT                            0x00000001
#define NVC86F_GP_ENTRY1_OPCODE                                      7:0
#define NVC86F_GP_ENTRY1_OPCODE_NOP                           0x00000000
#define NVC86F_GP_ENTRY1_OPCODE_ILLEGAL                       0x00000001
#define NVC86F_GP_ENTRY1_OPCODE_GP_CRC                        0x00000002
#define NVC86F_GP_ENTRY1_OPCODE_PB_CRC                        0x00000003
#define NVC86F_GP_ENTRY1_OPCODE_SET_PB_SEGMENT_EXTENDED_BASE  0x00000004

#endif // __gh100_clc86f_h__
