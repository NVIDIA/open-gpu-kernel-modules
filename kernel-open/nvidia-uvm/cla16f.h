/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cla16f_h_
#define _cla16f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/* class KEPLER_CHANNEL_GPFIFO  */
/*
 * Documentation for KEPLER_CHANNEL_GPFIFO can be found in dev_pbdma.ref,
 * chapter "User Control Registers". It is documented as device NV_UDMA.
 * The GPFIFO format itself is also documented in dev_pbdma.ref,
 * NV_PPBDMA_GP_ENTRY_*. The pushbuffer format is documented in dev_ram.ref,
 * chapter "FIFO DMA RAM", NV_FIFO_DMA_*.
 *
 */
#define  KEPLER_CHANNEL_GPFIFO_B                           (0x0000A16F)

/* pio method data structure */
typedef volatile struct _cla16f_tag0 {
 NvV32 Reserved00[0x7c0];
} NvA16FTypedef, KEPLER_ChannelGPFifoB;
#define NVA16F_TYPEDEF                               KEPLER_CHANNELChannelGPFifo
/* dma flow control data structure */
typedef volatile struct _cla16f_tag1 {
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
} NvA16FControl, KeplerBControlGPFifo;
/* fields and values */
#define NVA16F_NUMBER_OF_SUBCHANNELS                               (8)
#define NVA16F_SET_OBJECT                                          (0x00000000)
#define NVA16F_SET_OBJECT_NVCLASS                                         15:0
#define NVA16F_SET_OBJECT_ENGINE                                         20:16
#define NVA16F_SET_OBJECT_ENGINE_SW                                 0x0000001f
#define NVA16F_ILLEGAL                                             (0x00000004)
#define NVA16F_ILLEGAL_HANDLE                                             31:0
#define NVA16F_NOP                                                 (0x00000008)
#define NVA16F_NOP_HANDLE                                                 31:0
#define NVA16F_SEMAPHOREA                                          (0x00000010)
#define NVA16F_SEMAPHOREA_OFFSET_UPPER                                     7:0
#define NVA16F_SEMAPHOREB                                          (0x00000014)
#define NVA16F_SEMAPHOREB_OFFSET_LOWER                                    31:2
#define NVA16F_SEMAPHOREC                                          (0x00000018)
#define NVA16F_SEMAPHOREC_PAYLOAD                                         31:0
#define NVA16F_SEMAPHORED                                          (0x0000001C)
#define NVA16F_SEMAPHORED_OPERATION                                        4:0
#define NVA16F_SEMAPHORED_OPERATION_ACQUIRE                         0x00000001
#define NVA16F_SEMAPHORED_OPERATION_RELEASE                         0x00000002
#define NVA16F_SEMAPHORED_OPERATION_ACQ_GEQ                         0x00000004
#define NVA16F_SEMAPHORED_OPERATION_ACQ_AND                         0x00000008
#define NVA16F_SEMAPHORED_OPERATION_REDUCTION                       0x00000010
#define NVA16F_SEMAPHORED_ACQUIRE_SWITCH                                 12:12
#define NVA16F_SEMAPHORED_ACQUIRE_SWITCH_DISABLED                   0x00000000
#define NVA16F_SEMAPHORED_ACQUIRE_SWITCH_ENABLED                    0x00000001
#define NVA16F_SEMAPHORED_RELEASE_WFI                                    20:20
#define NVA16F_SEMAPHORED_RELEASE_WFI_EN                            0x00000000
#define NVA16F_SEMAPHORED_RELEASE_WFI_DIS                           0x00000001
#define NVA16F_SEMAPHORED_RELEASE_SIZE                                   24:24
#define NVA16F_SEMAPHORED_RELEASE_SIZE_16BYTE                       0x00000000
#define NVA16F_SEMAPHORED_RELEASE_SIZE_4BYTE                        0x00000001
#define NVA16F_SEMAPHORED_REDUCTION                                      30:27
#define NVA16F_SEMAPHORED_REDUCTION_MIN                             0x00000000
#define NVA16F_SEMAPHORED_REDUCTION_MAX                             0x00000001
#define NVA16F_SEMAPHORED_REDUCTION_XOR                             0x00000002
#define NVA16F_SEMAPHORED_REDUCTION_AND                             0x00000003
#define NVA16F_SEMAPHORED_REDUCTION_OR                              0x00000004
#define NVA16F_SEMAPHORED_REDUCTION_ADD                             0x00000005
#define NVA16F_SEMAPHORED_REDUCTION_INC                             0x00000006
#define NVA16F_SEMAPHORED_REDUCTION_DEC                             0x00000007
#define NVA16F_SEMAPHORED_FORMAT                                         31:31
#define NVA16F_SEMAPHORED_FORMAT_SIGNED                             0x00000000
#define NVA16F_SEMAPHORED_FORMAT_UNSIGNED                           0x00000001
#define NVA16F_NON_STALL_INTERRUPT                                 (0x00000020)
#define NVA16F_NON_STALL_INTERRUPT_HANDLE                                 31:0
#define NVA16F_FB_FLUSH                                            (0x00000024)
#define NVA16F_FB_FLUSH_HANDLE                                            31:0
#define NVA16F_MEM_OP_A                                            (0x00000028)
#define NVA16F_MEM_OP_A_OPERAND_LOW                                       31:2
#define NVA16F_MEM_OP_A_TLB_INVALIDATE_ADDR                               29:2
#define NVA16F_MEM_OP_A_TLB_INVALIDATE_TARGET                            31:30
#define NVA16F_MEM_OP_A_TLB_INVALIDATE_TARGET_VID_MEM               0x00000000
#define NVA16F_MEM_OP_A_TLB_INVALIDATE_TARGET_SYS_MEM_COHERENT      0x00000002
#define NVA16F_MEM_OP_A_TLB_INVALIDATE_TARGET_SYS_MEM_NONCOHERENT   0x00000003
#define NVA16F_MEM_OP_B                                            (0x0000002c)
#define NVA16F_MEM_OP_B_OPERAND_HIGH                                       7:0
#define NVA16F_MEM_OP_B_OPERATION                                        31:27
#define NVA16F_MEM_OP_B_OPERATION_SYSMEMBAR_FLUSH                   0x00000005
#define NVA16F_MEM_OP_B_OPERATION_SOFT_FLUSH                        0x00000006
#define NVA16F_MEM_OP_B_OPERATION_MMU_TLB_INVALIDATE                0x00000009
#define NVA16F_MEM_OP_B_OPERATION_L2_PEERMEM_INVALIDATE             0x0000000d
#define NVA16F_MEM_OP_B_OPERATION_L2_SYSMEM_INVALIDATE              0x0000000e
#define NVA16F_MEM_OP_B_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVA16F_MEM_OP_B_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_PDB                             0:0
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_PDB_ONE                  0x00000000
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_PDB_ALL                  0x00000001
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_GPC                             1:1
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_GPC_ENABLE               0x00000000
#define NVA16F_MEM_OP_B_MMU_TLB_INVALIDATE_GPC_DISABLE              0x00000001
#define NVA16F_SET_REFERENCE                                       (0x00000050)
#define NVA16F_SET_REFERENCE_COUNT                                        31:0
#define NVA16F_WFI                                                 (0x00000078)
#define NVA16F_WFI_HANDLE                                                 31:0
#define NVA16F_CRC_CHECK                                           (0x0000007c)
#define NVA16F_CRC_CHECK_VALUE                                            31:0
#define NVA16F_YIELD                                               (0x00000080)
#define NVA16F_YIELD_OP                                                    1:0
#define NVA16F_YIELD_OP_NOP                                         0x00000000


/* GPFIFO entry format */
#define NVA16F_GP_ENTRY__SIZE                                   8
#define NVA16F_GP_ENTRY0_FETCH                                0:0
#define NVA16F_GP_ENTRY0_FETCH_UNCONDITIONAL           0x00000000
#define NVA16F_GP_ENTRY0_FETCH_CONDITIONAL             0x00000001
#define NVA16F_GP_ENTRY0_GET                                 31:2
#define NVA16F_GP_ENTRY0_OPERAND                             31:0
#define NVA16F_GP_ENTRY1_GET_HI                               7:0
#define NVA16F_GP_ENTRY1_PRIV                                 8:8
#define NVA16F_GP_ENTRY1_PRIV_USER                     0x00000000
#define NVA16F_GP_ENTRY1_PRIV_KERNEL                   0x00000001
#define NVA16F_GP_ENTRY1_LEVEL                                9:9
#define NVA16F_GP_ENTRY1_LEVEL_MAIN                    0x00000000
#define NVA16F_GP_ENTRY1_LEVEL_SUBROUTINE              0x00000001
#define NVA16F_GP_ENTRY1_LENGTH                             30:10
#define NVA16F_GP_ENTRY1_SYNC                               31:31
#define NVA16F_GP_ENTRY1_SYNC_PROCEED                  0x00000000
#define NVA16F_GP_ENTRY1_SYNC_WAIT                     0x00000001
#define NVA16F_GP_ENTRY1_OPCODE                               7:0
#define NVA16F_GP_ENTRY1_OPCODE_NOP                    0x00000000
#define NVA16F_GP_ENTRY1_OPCODE_ILLEGAL                0x00000001
#define NVA16F_GP_ENTRY1_OPCODE_GP_CRC                 0x00000002
#define NVA16F_GP_ENTRY1_OPCODE_PB_CRC                 0x00000003

/* dma method formats */
#define NVA16F_DMA_METHOD_ADDRESS_OLD                              12:2
#define NVA16F_DMA_METHOD_ADDRESS                                  11:0
#define NVA16F_DMA_SUBDEVICE_MASK                                  15:4
#define NVA16F_DMA_METHOD_SUBCHANNEL                               15:13
#define NVA16F_DMA_TERT_OP                                         17:16
#define NVA16F_DMA_TERT_OP_GRP0_INC_METHOD                         (0x00000000)
#define NVA16F_DMA_TERT_OP_GRP0_SET_SUB_DEV_MASK                   (0x00000001)
#define NVA16F_DMA_TERT_OP_GRP0_STORE_SUB_DEV_MASK                 (0x00000002)
#define NVA16F_DMA_TERT_OP_GRP0_USE_SUB_DEV_MASK                   (0x00000003)
#define NVA16F_DMA_TERT_OP_GRP2_NON_INC_METHOD                     (0x00000000)
#define NVA16F_DMA_METHOD_COUNT_OLD                                28:18
#define NVA16F_DMA_METHOD_COUNT                                    28:16
#define NVA16F_DMA_IMMD_DATA                                       28:16
#define NVA16F_DMA_SEC_OP                                          31:29
#define NVA16F_DMA_SEC_OP_GRP0_USE_TERT                            (0x00000000)
#define NVA16F_DMA_SEC_OP_INC_METHOD                               (0x00000001)
#define NVA16F_DMA_SEC_OP_GRP2_USE_TERT                            (0x00000002)
#define NVA16F_DMA_SEC_OP_NON_INC_METHOD                           (0x00000003)
#define NVA16F_DMA_SEC_OP_IMMD_DATA_METHOD                         (0x00000004)
#define NVA16F_DMA_SEC_OP_ONE_INC                                  (0x00000005)
#define NVA16F_DMA_SEC_OP_RESERVED6                                (0x00000006)
#define NVA16F_DMA_SEC_OP_END_PB_SEGMENT                           (0x00000007)
/* dma incrementing method format */
#define NVA16F_DMA_INCR_ADDRESS                                    11:0
#define NVA16F_DMA_INCR_SUBCHANNEL                                 15:13
#define NVA16F_DMA_INCR_COUNT                                      28:16
#define NVA16F_DMA_INCR_OPCODE                                     31:29
#define NVA16F_DMA_INCR_OPCODE_VALUE                               (0x00000001)
#define NVA16F_DMA_INCR_DATA                                       31:0
/* dma non-incrementing method format */
#define NVA16F_DMA_NONINCR_ADDRESS                                 11:0
#define NVA16F_DMA_NONINCR_SUBCHANNEL                              15:13
#define NVA16F_DMA_NONINCR_COUNT                                   28:16
#define NVA16F_DMA_NONINCR_OPCODE                                  31:29
#define NVA16F_DMA_NONINCR_OPCODE_VALUE                            (0x00000003)
#define NVA16F_DMA_NONINCR_DATA                                    31:0
/* dma increment-once method format */
#define NVA16F_DMA_ONEINCR_ADDRESS                                 11:0
#define NVA16F_DMA_ONEINCR_SUBCHANNEL                              15:13
#define NVA16F_DMA_ONEINCR_COUNT                                   28:16
#define NVA16F_DMA_ONEINCR_OPCODE                                  31:29
#define NVA16F_DMA_ONEINCR_OPCODE_VALUE                            (0x00000005)
#define NVA16F_DMA_ONEINCR_DATA                                    31:0
/* dma no-operation format */
#define NVA16F_DMA_NOP                                             (0x00000000)
/* dma immediate-data format */
#define NVA16F_DMA_IMMD_ADDRESS                                    11:0
#define NVA16F_DMA_IMMD_SUBCHANNEL                                 15:13
#define NVA16F_DMA_IMMD_DATA                                       28:16
#define NVA16F_DMA_IMMD_OPCODE                                     31:29
#define NVA16F_DMA_IMMD_OPCODE_VALUE                               (0x00000004)
/* dma set sub-device mask format */
#define NVA16F_DMA_SET_SUBDEVICE_MASK_VALUE                        15:4
#define NVA16F_DMA_SET_SUBDEVICE_MASK_OPCODE                       31:16
#define NVA16F_DMA_SET_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000001)
/* dma store sub-device mask format */
#define NVA16F_DMA_STORE_SUBDEVICE_MASK_VALUE                      15:4
#define NVA16F_DMA_STORE_SUBDEVICE_MASK_OPCODE                     31:16
#define NVA16F_DMA_STORE_SUBDEVICE_MASK_OPCODE_VALUE               (0x00000002)
/* dma use sub-device mask format */
#define NVA16F_DMA_USE_SUBDEVICE_MASK_OPCODE                       31:16
#define NVA16F_DMA_USE_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000003)
/* dma end-segment format */
#define NVA16F_DMA_ENDSEG_OPCODE                                   31:29
#define NVA16F_DMA_ENDSEG_OPCODE_VALUE                             (0x00000007)
/* dma legacy incrementing/non-incrementing formats */
#define NVA16F_DMA_ADDRESS                                         12:2
#define NVA16F_DMA_SUBCH                                           15:13
#define NVA16F_DMA_OPCODE3                                         17:16
#define NVA16F_DMA_OPCODE3_NONE                                    (0x00000000)
#define NVA16F_DMA_COUNT                                           28:18
#define NVA16F_DMA_OPCODE                                          31:29
#define NVA16F_DMA_OPCODE_METHOD                                   (0x00000000)
#define NVA16F_DMA_OPCODE_NONINC_METHOD                            (0x00000002)
#define NVA16F_DMA_DATA                                            31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cla16F_h_ */
