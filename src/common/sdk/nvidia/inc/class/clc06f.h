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

#ifndef _clc06f_h_
#define _clc06f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/* class PASCAL_CHANNEL_GPFIFO  */
/*
 * Documentation for PASCAL_CHANNEL_GPFIFO can be found in dev_pbdma.ref,
 * chapter "User Control Registers". It is documented as device NV_UDMA.
 * The GPFIFO format itself is also documented in dev_pbdma.ref,
 * NV_PPBDMA_GP_ENTRY_*. The pushbuffer format is documented in dev_ram.ref,
 * chapter "FIFO DMA RAM", NV_FIFO_DMA_*.
 *
 * Note there is no .mfs file for this class.
 */
#define  PASCAL_CHANNEL_GPFIFO_A                           (0x0000C06F)

#define NVC06F_TYPEDEF                             PASCAL_CHANNELChannelGPFifoA

/* dma flow control data structure */
typedef volatile struct Nvc06fControl_struct {
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
} Nvc06fControl, PascalAControlGPFifo;

/* fields and values */
#define NVC06F_NUMBER_OF_SUBCHANNELS                               (8)
#define NVC06F_SET_OBJECT                                          (0x00000000)
#define NVC06F_SET_OBJECT_NVCLASS                                         15:0
#define NVC06F_SET_OBJECT_ENGINE                                         20:16
#define NVC06F_SET_OBJECT_ENGINE_SW                                 0x0000001f
#define NVC06F_ILLEGAL                                             (0x00000004)
#define NVC06F_ILLEGAL_HANDLE                                             31:0
#define NVC06F_NOP                                                 (0x00000008)
#define NVC06F_NOP_HANDLE                                                 31:0
#define NVC06F_SEMAPHOREA                                          (0x00000010)
#define NVC06F_SEMAPHOREA_OFFSET_UPPER                                     7:0
#define NVC06F_SEMAPHOREB                                          (0x00000014)
#define NVC06F_SEMAPHOREB_OFFSET_LOWER                                    31:2
#define NVC06F_SEMAPHOREC                                          (0x00000018)
#define NVC06F_SEMAPHOREC_PAYLOAD                                         31:0
#define NVC06F_SEMAPHORED                                          (0x0000001C)
#define NVC06F_SEMAPHORED_OPERATION                                        4:0
#define NVC06F_SEMAPHORED_OPERATION_ACQUIRE                         0x00000001
#define NVC06F_SEMAPHORED_OPERATION_RELEASE                         0x00000002
#define NVC06F_SEMAPHORED_OPERATION_ACQ_GEQ                         0x00000004
#define NVC06F_SEMAPHORED_OPERATION_ACQ_AND                         0x00000008
#define NVC06F_SEMAPHORED_OPERATION_REDUCTION                       0x00000010
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH                                 12:12
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH_DISABLED                   0x00000000
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH_ENABLED                    0x00000001
#define NVC06F_SEMAPHORED_RELEASE_WFI                                    20:20
#define NVC06F_SEMAPHORED_RELEASE_WFI_EN                            0x00000000
#define NVC06F_SEMAPHORED_RELEASE_WFI_DIS                           0x00000001
#define NVC06F_SEMAPHORED_RELEASE_SIZE                                   24:24
#define NVC06F_SEMAPHORED_RELEASE_SIZE_16BYTE                       0x00000000
#define NVC06F_SEMAPHORED_RELEASE_SIZE_4BYTE                        0x00000001
#define NVC06F_SEMAPHORED_REDUCTION                                      30:27
#define NVC06F_SEMAPHORED_REDUCTION_MIN                             0x00000000
#define NVC06F_SEMAPHORED_REDUCTION_MAX                             0x00000001
#define NVC06F_SEMAPHORED_REDUCTION_XOR                             0x00000002
#define NVC06F_SEMAPHORED_REDUCTION_AND                             0x00000003
#define NVC06F_SEMAPHORED_REDUCTION_OR                              0x00000004
#define NVC06F_SEMAPHORED_REDUCTION_ADD                             0x00000005
#define NVC06F_SEMAPHORED_REDUCTION_INC                             0x00000006
#define NVC06F_SEMAPHORED_REDUCTION_DEC                             0x00000007
#define NVC06F_SEMAPHORED_FORMAT                                         31:31
#define NVC06F_SEMAPHORED_FORMAT_SIGNED                             0x00000000
#define NVC06F_SEMAPHORED_FORMAT_UNSIGNED                           0x00000001
#define NVC06F_NON_STALL_INTERRUPT                                 (0x00000020)
#define NVC06F_NON_STALL_INTERRUPT_HANDLE                                 31:0
#define NVC06F_FB_FLUSH                                            (0x00000024) // Deprecated - use MEMBAR TYPE SYS_MEMBAR
#define NVC06F_FB_FLUSH_HANDLE                                            31:0
// NOTE - MEM_OP_A and MEM_OP_B have been replaced in gp100 with methods for
// specifying the page address for a targeted TLB invalidate and the uTLB for
// a targeted REPLAY_CANCEL for UVM.
// The previous MEM_OP_A/B functionality is in MEM_OP_C/D, with slightly
// rearranged fields.
#define NVC06F_MEM_OP_A                                            (0x00000028)
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_CLIENT_UNIT_ID        5:0  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_GPC_ID               10:6  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR                         11:11
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_EN                 0x00000001
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_DIS                0x00000000
#define NVC06F_MEM_OP_A_TLB_INVALIDATE_TARGET_ADDR_LO                    31:12
#define NVC06F_MEM_OP_B                                            (0x0000002c)
#define NVC06F_MEM_OP_B_TLB_INVALIDATE_TARGET_ADDR_HI                     31:0
#define NVC06F_MEM_OP_C                                            (0x00000030)
#define NVC06F_MEM_OP_C_MEMBAR_TYPE                                        2:0
#define NVC06F_MEM_OP_C_MEMBAR_TYPE_SYS_MEMBAR                      0x00000000
#define NVC06F_MEM_OP_C_MEMBAR_TYPE_MEMBAR                          0x00000001
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB                                 0:0
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_ONE                      0x00000000
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_ALL                      0x00000001  // Probably nonsensical for MMU_TLB_INVALIDATE_TARGETED
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_GPC                                 1:1
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_GPC_ENABLE                   0x00000000
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_GPC_DISABLE                  0x00000001
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY                              4:2  // only relevant if GPC ENABLE
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY_NONE                  0x00000000
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START                 0x00000001
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START_ACK_ALL         0x00000002
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_TARGETED       0x00000003
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_GLOBAL         0x00000004
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE                            6:5  // only relevant if GPC ENABLE
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_NONE                0x00000000
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_GLOBALLY            0x00000001
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_INTRANODE           0x00000002
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL                    9:7  // Invalidate affects this level and all below
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_ALL         0x00000000  // Invalidate tlb caches at all levels of the page table
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_PTE_ONLY    0x00000001
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE0  0x00000002
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE1  0x00000003
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE2  0x00000004
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3  0x00000005
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4  0x00000006
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE5  0x00000007
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE                          11:10  // only relevant if PDB_ONE
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_VID_MEM             0x00000000
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_COHERENT    0x00000002
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_NONCOHERENT 0x00000003
#define NVC06F_MEM_OP_C_TLB_INVALIDATE_PDB_ADDR_LO                       31:12  // only relevant if PDB_ONE
// MEM_OP_D MUST be preceded by MEM_OPs A-C.
#define NVC06F_MEM_OP_D                                            (0x00000034)
#define NVC06F_MEM_OP_D_TLB_INVALIDATE_PDB_ADDR_HI                        26:0  // only relevant if PDB_ONE
#define NVC06F_MEM_OP_D_OPERATION                                        31:27
#define NVC06F_MEM_OP_D_OPERATION_MEMBAR                            0x00000005
#define NVC06F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE                0x00000009
#define NVC06F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE_TARGETED       0x0000000a
#define NVC06F_MEM_OP_D_OPERATION_L2_PEERMEM_INVALIDATE             0x0000000d
#define NVC06F_MEM_OP_D_OPERATION_L2_SYSMEM_INVALIDATE              0x0000000e
// CLEAN_LINES is an alias for Tegra/GPU IP usage
#define NVC06F_MEM_OP_D_OPERATION_L2_INVALIDATE_CLEAN_LINES         0x0000000e
// This B alias is confusing but it was missed as part of the update. Left here
// for compatibility.
#define NVC06F_MEM_OP_B_OPERATION_L2_INVALIDATE_CLEAN_LINES         0x0000000e
#define NVC06F_MEM_OP_D_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVC06F_MEM_OP_D_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVC06F_MEM_OP_D_OPERATION_L2_WAIT_FOR_SYS_PENDING_READS     0x00000015
#define NVC06F_SET_REFERENCE                                       (0x00000050)
#define NVC06F_SET_REFERENCE_COUNT                                        31:0
// Syncpoint methods are only available on Tegra parts.  Attempting to use
// them on discrete GPUs will result in Host raising NV_PPBDMA_INTR_0_METHOD.
#define NVC06F_SYNCPOINTA                                          (0x00000070)
#define NVC06F_SYNCPOINTA_PAYLOAD                                         31:0
#define NVC06F_SYNCPOINTB                                          (0x00000074)
#define NVC06F_SYNCPOINTB_OPERATION                                        0:0
#define NVC06F_SYNCPOINTB_OPERATION_WAIT                            0x00000000
#define NVC06F_SYNCPOINTB_OPERATION_INCR                            0x00000001
#define NVC06F_SYNCPOINTB_WAIT_SWITCH                                      4:4
#define NVC06F_SYNCPOINTB_WAIT_SWITCH_DIS                           0x00000000
#define NVC06F_SYNCPOINTB_WAIT_SWITCH_EN                            0x00000001
#define NVC06F_SYNCPOINTB_SYNCPT_INDEX                                    19:8
#define NVC06F_WFI                                                 (0x00000078)
#define NVC06F_WFI_SCOPE                                                   0:0
#define NVC06F_WFI_SCOPE_CURRENT_SCG_TYPE                           0x00000000
#define NVC06F_WFI_SCOPE_ALL                                        0x00000001
#define NVC06F_CRC_CHECK                                           (0x0000007c)
#define NVC06F_CRC_CHECK_VALUE                                            31:0
#define NVC06F_YIELD                                               (0x00000080)
#define NVC06F_YIELD_OP                                                    1:0
#define NVC06F_YIELD_OP_NOP                                         0x00000000
#define NVC06F_YIELD_OP_PBDMA_TIMESLICE                             0x00000001
#define NVC06F_YIELD_OP_RUNLIST_TIMESLICE                           0x00000002
#define NVC06F_YIELD_OP_TSG                                         0x00000003


/* GPFIFO entry format */
#define NVC06F_GP_ENTRY__SIZE                                   8
#define NVC06F_GP_ENTRY0_FETCH                                0:0
#define NVC06F_GP_ENTRY0_FETCH_UNCONDITIONAL           0x00000000
#define NVC06F_GP_ENTRY0_FETCH_CONDITIONAL             0x00000001
#define NVC06F_GP_ENTRY0_GET                                 31:2
#define NVC06F_GP_ENTRY0_OPERAND                             31:0
#define NVC06F_GP_ENTRY1_GET_HI                               7:0
#define NVC06F_GP_ENTRY1_PRIV                                 8:8
#define NVC06F_GP_ENTRY1_PRIV_USER                     0x00000000
#define NVC06F_GP_ENTRY1_PRIV_KERNEL                   0x00000001
#define NVC06F_GP_ENTRY1_LEVEL                                9:9
#define NVC06F_GP_ENTRY1_LEVEL_MAIN                    0x00000000
#define NVC06F_GP_ENTRY1_LEVEL_SUBROUTINE              0x00000001
#define NVC06F_GP_ENTRY1_LENGTH                             30:10
#define NVC06F_GP_ENTRY1_SYNC                               31:31
#define NVC06F_GP_ENTRY1_SYNC_PROCEED                  0x00000000
#define NVC06F_GP_ENTRY1_SYNC_WAIT                     0x00000001
#define NVC06F_GP_ENTRY1_OPCODE                               7:0
#define NVC06F_GP_ENTRY1_OPCODE_NOP                    0x00000000
#define NVC06F_GP_ENTRY1_OPCODE_ILLEGAL                0x00000001
#define NVC06F_GP_ENTRY1_OPCODE_GP_CRC                 0x00000002
#define NVC06F_GP_ENTRY1_OPCODE_PB_CRC                 0x00000003

/* dma method formats */
#define NVC06F_DMA_METHOD_ADDRESS_OLD                              12:2
#define NVC06F_DMA_METHOD_ADDRESS                                  11:0
#define NVC06F_DMA_SUBDEVICE_MASK                                  15:4
#define NVC06F_DMA_METHOD_SUBCHANNEL                               15:13
#define NVC06F_DMA_TERT_OP                                         17:16
#define NVC06F_DMA_TERT_OP_GRP0_INC_METHOD                         (0x00000000)
#define NVC06F_DMA_TERT_OP_GRP0_SET_SUB_DEV_MASK                   (0x00000001)
#define NVC06F_DMA_TERT_OP_GRP0_STORE_SUB_DEV_MASK                 (0x00000002)
#define NVC06F_DMA_TERT_OP_GRP0_USE_SUB_DEV_MASK                   (0x00000003)
#define NVC06F_DMA_TERT_OP_GRP2_NON_INC_METHOD                     (0x00000000)
#define NVC06F_DMA_METHOD_COUNT_OLD                                28:18
#define NVC06F_DMA_METHOD_COUNT                                    28:16
#define NVC06F_DMA_IMMD_DATA                                       28:16
#define NVC06F_DMA_SEC_OP                                          31:29
#define NVC06F_DMA_SEC_OP_GRP0_USE_TERT                            (0x00000000)
#define NVC06F_DMA_SEC_OP_INC_METHOD                               (0x00000001)
#define NVC06F_DMA_SEC_OP_GRP2_USE_TERT                            (0x00000002)
#define NVC06F_DMA_SEC_OP_NON_INC_METHOD                           (0x00000003)
#define NVC06F_DMA_SEC_OP_IMMD_DATA_METHOD                         (0x00000004)
#define NVC06F_DMA_SEC_OP_ONE_INC                                  (0x00000005)
#define NVC06F_DMA_SEC_OP_RESERVED6                                (0x00000006)
#define NVC06F_DMA_SEC_OP_END_PB_SEGMENT                           (0x00000007)
/* dma incrementing method format */
#define NVC06F_DMA_INCR_ADDRESS                                    11:0
#define NVC06F_DMA_INCR_SUBCHANNEL                                 15:13
#define NVC06F_DMA_INCR_COUNT                                      28:16
#define NVC06F_DMA_INCR_OPCODE                                     31:29
#define NVC06F_DMA_INCR_OPCODE_VALUE                               (0x00000001)
#define NVC06F_DMA_INCR_DATA                                       31:0
/* dma non-incrementing method format */
#define NVC06F_DMA_NONINCR_ADDRESS                                 11:0
#define NVC06F_DMA_NONINCR_SUBCHANNEL                              15:13
#define NVC06F_DMA_NONINCR_COUNT                                   28:16
#define NVC06F_DMA_NONINCR_OPCODE                                  31:29
#define NVC06F_DMA_NONINCR_OPCODE_VALUE                            (0x00000003)
#define NVC06F_DMA_NONINCR_DATA                                    31:0
/* dma increment-once method format */
#define NVC06F_DMA_ONEINCR_ADDRESS                                 11:0
#define NVC06F_DMA_ONEINCR_SUBCHANNEL                              15:13
#define NVC06F_DMA_ONEINCR_COUNT                                   28:16
#define NVC06F_DMA_ONEINCR_OPCODE                                  31:29
#define NVC06F_DMA_ONEINCR_OPCODE_VALUE                            (0x00000005)
#define NVC06F_DMA_ONEINCR_DATA                                    31:0
/* dma no-operation format */
#define NVC06F_DMA_NOP                                             (0x00000000)
/* dma immediate-data format */
#define NVC06F_DMA_IMMD_ADDRESS                                    11:0
#define NVC06F_DMA_IMMD_SUBCHANNEL                                 15:13
#define NVC06F_DMA_IMMD_DATA                                       28:16
#define NVC06F_DMA_IMMD_OPCODE                                     31:29
#define NVC06F_DMA_IMMD_OPCODE_VALUE                               (0x00000004)
/* dma set sub-device mask format */
#define NVC06F_DMA_SET_SUBDEVICE_MASK_VALUE                        15:4
#define NVC06F_DMA_SET_SUBDEVICE_MASK_OPCODE                       31:16
#define NVC06F_DMA_SET_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000001)
/* dma store sub-device mask format */
#define NVC06F_DMA_STORE_SUBDEVICE_MASK_VALUE                      15:4
#define NVC06F_DMA_STORE_SUBDEVICE_MASK_OPCODE                     31:16
#define NVC06F_DMA_STORE_SUBDEVICE_MASK_OPCODE_VALUE               (0x00000002)
/* dma use sub-device mask format */
#define NVC06F_DMA_USE_SUBDEVICE_MASK_OPCODE                       31:16
#define NVC06F_DMA_USE_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000003)
/* dma end-segment format */
#define NVC06F_DMA_ENDSEG_OPCODE                                   31:29
#define NVC06F_DMA_ENDSEG_OPCODE_VALUE                             (0x00000007)
/* dma legacy incrementing/non-incrementing formats */
#define NVC06F_DMA_ADDRESS                                         12:2
#define NVC06F_DMA_SUBCH                                           15:13
#define NVC06F_DMA_OPCODE3                                         17:16
#define NVC06F_DMA_OPCODE3_NONE                                    (0x00000000)
#define NVC06F_DMA_COUNT                                           28:18
#define NVC06F_DMA_OPCODE                                          31:29
#define NVC06F_DMA_OPCODE_METHOD                                   (0x00000000)
#define NVC06F_DMA_OPCODE_NONINC_METHOD                            (0x00000002)
#define NVC06F_DMA_DATA                                            31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc06f_h_ */
