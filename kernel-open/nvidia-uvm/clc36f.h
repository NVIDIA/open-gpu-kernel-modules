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

#ifndef _clc36f_h_
#define _clc36f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/* class VOLTA_CHANNEL_GPFIFO  */
/*
 * Documentation for VOLTA_CHANNEL_GPFIFO can be found in dev_pbdma.ref,
 * chapter "User Control Registers". It is documented as device NV_UDMA.
 * The GPFIFO format itself is also documented in dev_pbdma.ref,
 * NV_PPBDMA_GP_ENTRY_*. The pushbuffer format is documented in dev_ram.ref,
 * chapter "FIFO DMA RAM", NV_FIFO_DMA_*.
 *
 * Note there is no .mfs file for this class.
 */
#define  VOLTA_CHANNEL_GPFIFO_A                           (0x0000C36F)

#define NVC36F_TYPEDEF                             VOLTA_CHANNELChannelGPFifoA

/* dma flow control data structure */
typedef volatile struct Nvc36fControl_struct {
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
} Nvc36fControl, VoltaAControlGPFifo;

/* fields and values */
#define NVC36F_NUMBER_OF_SUBCHANNELS                               (8)
#define NVC36F_SET_OBJECT                                          (0x00000000)
#define NVC36F_SET_OBJECT_NVCLASS                                         15:0
#define NVC36F_SET_OBJECT_ENGINE                                         20:16
#define NVC36F_SET_OBJECT_ENGINE_SW                                 0x0000001f
#define NVC36F_ILLEGAL                                             (0x00000004)
#define NVC36F_ILLEGAL_HANDLE                                             31:0
#define NVC36F_NOP                                                 (0x00000008)
#define NVC36F_NOP_HANDLE                                                 31:0
#define NVC36F_SEMAPHOREA                                          (0x00000010)
#define NVC36F_SEMAPHOREA_OFFSET_UPPER                                     7:0
#define NVC36F_SEMAPHOREB                                          (0x00000014)
#define NVC36F_SEMAPHOREB_OFFSET_LOWER                                    31:2
#define NVC36F_SEMAPHOREC                                          (0x00000018)
#define NVC36F_SEMAPHOREC_PAYLOAD                                         31:0
#define NVC36F_SEMAPHORED                                          (0x0000001C)
#define NVC36F_SEMAPHORED_OPERATION                                        4:0
#define NVC36F_SEMAPHORED_OPERATION_ACQUIRE                         0x00000001
#define NVC36F_SEMAPHORED_OPERATION_RELEASE                         0x00000002
#define NVC36F_SEMAPHORED_OPERATION_ACQ_GEQ                         0x00000004
#define NVC36F_SEMAPHORED_OPERATION_ACQ_AND                         0x00000008
#define NVC36F_SEMAPHORED_OPERATION_REDUCTION                       0x00000010
#define NVC36F_SEMAPHORED_ACQUIRE_SWITCH                                 12:12
#define NVC36F_SEMAPHORED_ACQUIRE_SWITCH_DISABLED                   0x00000000
#define NVC36F_SEMAPHORED_ACQUIRE_SWITCH_ENABLED                    0x00000001
#define NVC36F_SEMAPHORED_RELEASE_WFI                                    20:20
#define NVC36F_SEMAPHORED_RELEASE_WFI_EN                            0x00000000
#define NVC36F_SEMAPHORED_RELEASE_WFI_DIS                           0x00000001
#define NVC36F_SEMAPHORED_RELEASE_SIZE                                   24:24
#define NVC36F_SEMAPHORED_RELEASE_SIZE_16BYTE                       0x00000000
#define NVC36F_SEMAPHORED_RELEASE_SIZE_4BYTE                        0x00000001
#define NVC36F_SEMAPHORED_REDUCTION                                      30:27
#define NVC36F_SEMAPHORED_REDUCTION_MIN                             0x00000000
#define NVC36F_SEMAPHORED_REDUCTION_MAX                             0x00000001
#define NVC36F_SEMAPHORED_REDUCTION_XOR                             0x00000002
#define NVC36F_SEMAPHORED_REDUCTION_AND                             0x00000003
#define NVC36F_SEMAPHORED_REDUCTION_OR                              0x00000004
#define NVC36F_SEMAPHORED_REDUCTION_ADD                             0x00000005
#define NVC36F_SEMAPHORED_REDUCTION_INC                             0x00000006
#define NVC36F_SEMAPHORED_REDUCTION_DEC                             0x00000007
#define NVC36F_SEMAPHORED_FORMAT                                         31:31
#define NVC36F_SEMAPHORED_FORMAT_SIGNED                             0x00000000
#define NVC36F_SEMAPHORED_FORMAT_UNSIGNED                           0x00000001
#define NVC36F_NON_STALL_INTERRUPT                                 (0x00000020)
#define NVC36F_NON_STALL_INTERRUPT_HANDLE                                 31:0
#define NVC36F_FB_FLUSH                                            (0x00000024) // Deprecated - use MEMBAR TYPE SYS_MEMBAR
#define NVC36F_FB_FLUSH_HANDLE                                            31:0
// NOTE - MEM_OP_A and MEM_OP_B have been replaced in gp100 with methods for
// specifying the page address for a targeted TLB invalidate and the uTLB for
// a targeted REPLAY_CANCEL for UVM.
// The previous MEM_OP_A/B functionality is in MEM_OP_C/D, with slightly
// rearranged fields.
#define NVC36F_MEM_OP_A                                            (0x00000028)
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_CLIENT_UNIT_ID        5:0  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_INVALIDATION_SIZE                   5:0  // Used to specify size of invalidate, used for invalidates which are not of the REPLAY_CANCEL_TARGETED type
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_GPC_ID               10:6  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_CANCEL_MMU_ENGINE_ID                6:0  // only relevant for REPLAY_CANCEL_VA_GLOBAL
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR                         11:11
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_EN                 0x00000001
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_DIS                0x00000000
#define NVC36F_MEM_OP_A_TLB_INVALIDATE_TARGET_ADDR_LO                    31:12
#define NVC36F_MEM_OP_B                                            (0x0000002c)
#define NVC36F_MEM_OP_B_TLB_INVALIDATE_TARGET_ADDR_HI                     31:0
#define NVC36F_MEM_OP_C                                            (0x00000030)
#define NVC36F_MEM_OP_C_MEMBAR_TYPE                                        2:0
#define NVC36F_MEM_OP_C_MEMBAR_TYPE_SYS_MEMBAR                      0x00000000
#define NVC36F_MEM_OP_C_MEMBAR_TYPE_MEMBAR                          0x00000001
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB                                 0:0
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_ONE                      0x00000000
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_ALL                      0x00000001  // Probably nonsensical for MMU_TLB_INVALIDATE_TARGETED
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_GPC                                 1:1
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_GPC_ENABLE                   0x00000000
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_GPC_DISABLE                  0x00000001
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY                              4:2  // only relevant if GPC ENABLE
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_NONE                  0x00000000
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START                 0x00000001
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START_ACK_ALL         0x00000002
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_TARGETED       0x00000003
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_GLOBAL         0x00000004
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_VA_GLOBAL      0x00000005
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE                            6:5  // only relevant if GPC ENABLE
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_NONE                0x00000000
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_GLOBALLY            0x00000001
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_INTRANODE           0x00000002
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE                         9:7 //only relevant for REPLAY_CANCEL_VA_GLOBAL
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_READ                 0
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_WRITE                1
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_STRONG        2
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_RSVRVD               3
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_WEAK          4
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_ALL           5
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_WRITE_AND_ATOMIC     6
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ALL                  7
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL                    9:7  // Invalidate affects this level and all below
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_ALL         0x00000000  // Invalidate tlb caches at all levels of the page table
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_PTE_ONLY    0x00000001
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE0  0x00000002
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE1  0x00000003
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE2  0x00000004
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3  0x00000005
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4  0x00000006
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE5  0x00000007
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE                          11:10  // only relevant if PDB_ONE
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_VID_MEM             0x00000000
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_COHERENT    0x00000002
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_NONCOHERENT 0x00000003
#define NVC36F_MEM_OP_C_TLB_INVALIDATE_PDB_ADDR_LO                       31:12  // only relevant if PDB_ONE
#define NVC36F_MEM_OP_C_ACCESS_COUNTER_CLR_TARGETED_NOTIFY_TAG            19:0
// MEM_OP_D MUST be preceded by MEM_OPs A-C.
#define NVC36F_MEM_OP_D                                            (0x00000034)
#define NVC36F_MEM_OP_D_TLB_INVALIDATE_PDB_ADDR_HI                        26:0  // only relevant if PDB_ONE
#define NVC36F_MEM_OP_D_OPERATION                                        31:27
#define NVC36F_MEM_OP_D_OPERATION_MEMBAR                            0x00000005
#define NVC36F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE                0x00000009
#define NVC36F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE_TARGETED       0x0000000a
#define NVC36F_MEM_OP_D_OPERATION_L2_PEERMEM_INVALIDATE             0x0000000d
#define NVC36F_MEM_OP_D_OPERATION_L2_SYSMEM_INVALIDATE              0x0000000e
// CLEAN_LINES is an alias for Tegra/GPU IP usage
#define NVC36F_MEM_OP_B_OPERATION_L2_INVALIDATE_CLEAN_LINES         0x0000000e
#define NVC36F_MEM_OP_D_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVC36F_MEM_OP_D_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVC36F_MEM_OP_D_OPERATION_L2_WAIT_FOR_SYS_PENDING_READS     0x00000015
#define NVC36F_MEM_OP_D_OPERATION_ACCESS_COUNTER_CLR                0x00000016
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE                            1:0
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_MIMC                0x00000000
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_MOMC                0x00000001
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_ALL                 0x00000002
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_TARGETED            0x00000003
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE                   2:2
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE_MIMC       0x00000000
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE_MOMC       0x00000001
#define NVC36F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_BANK                   6:3
#define NVC36F_SET_REFERENCE                                       (0x00000050)
#define NVC36F_SET_REFERENCE_COUNT                                        31:0
#define NVC36F_SEM_ADDR_LO                                         (0x0000005c)
#define NVC36F_SEM_ADDR_LO_OFFSET                                         31:2
#define NVC36F_SEM_ADDR_HI                                         (0x00000060)
#define NVC36F_SEM_ADDR_HI_OFFSET                                          7:0
#define NVC36F_SEM_PAYLOAD_LO                                      (0x00000064)
#define NVC36F_SEM_PAYLOAD_LO_PAYLOAD                                     31:0
#define NVC36F_SEM_PAYLOAD_HI                                      (0x00000068)
#define NVC36F_SEM_PAYLOAD_HI_PAYLOAD                                     31:0
#define NVC36F_SEM_EXECUTE                                         (0x0000006c)
#define NVC36F_SEM_EXECUTE_OPERATION                                       2:0
#define NVC36F_SEM_EXECUTE_OPERATION_ACQUIRE                        0x00000000
#define NVC36F_SEM_EXECUTE_OPERATION_RELEASE                        0x00000001
#define NVC36F_SEM_EXECUTE_OPERATION_ACQ_STRICT_GEQ                 0x00000002
#define NVC36F_SEM_EXECUTE_OPERATION_ACQ_CIRC_GEQ                   0x00000003
#define NVC36F_SEM_EXECUTE_OPERATION_ACQ_AND                        0x00000004
#define NVC36F_SEM_EXECUTE_OPERATION_ACQ_NOR                        0x00000005
#define NVC36F_SEM_EXECUTE_OPERATION_REDUCTION                      0x00000006
#define NVC36F_SEM_EXECUTE_ACQUIRE_SWITCH_TSG                            12:12
#define NVC36F_SEM_EXECUTE_ACQUIRE_SWITCH_TSG_DIS                   0x00000000
#define NVC36F_SEM_EXECUTE_ACQUIRE_SWITCH_TSG_EN                    0x00000001
#define NVC36F_SEM_EXECUTE_RELEASE_WFI                                   20:20
#define NVC36F_SEM_EXECUTE_RELEASE_WFI_DIS                          0x00000000
#define NVC36F_SEM_EXECUTE_RELEASE_WFI_EN                           0x00000001
#define NVC36F_SEM_EXECUTE_PAYLOAD_SIZE                                  24:24
#define NVC36F_SEM_EXECUTE_PAYLOAD_SIZE_32BIT                       0x00000000
#define NVC36F_SEM_EXECUTE_PAYLOAD_SIZE_64BIT                       0x00000001
#define NVC36F_SEM_EXECUTE_RELEASE_TIMESTAMP                             25:25
#define NVC36F_SEM_EXECUTE_RELEASE_TIMESTAMP_DIS                    0x00000000
#define NVC36F_SEM_EXECUTE_RELEASE_TIMESTAMP_EN                     0x00000001
#define NVC36F_SEM_EXECUTE_REDUCTION                                     30:27
#define NVC36F_SEM_EXECUTE_REDUCTION_IMIN                           0x00000000
#define NVC36F_SEM_EXECUTE_REDUCTION_IMAX                           0x00000001
#define NVC36F_SEM_EXECUTE_REDUCTION_IXOR                           0x00000002
#define NVC36F_SEM_EXECUTE_REDUCTION_IAND                           0x00000003
#define NVC36F_SEM_EXECUTE_REDUCTION_IOR                            0x00000004
#define NVC36F_SEM_EXECUTE_REDUCTION_IADD                           0x00000005
#define NVC36F_SEM_EXECUTE_REDUCTION_INC                            0x00000006
#define NVC36F_SEM_EXECUTE_REDUCTION_DEC                            0x00000007
#define NVC36F_SEM_EXECUTE_REDUCTION_FORMAT                              31:31
#define NVC36F_SEM_EXECUTE_REDUCTION_FORMAT_SIGNED                  0x00000000
#define NVC36F_SEM_EXECUTE_REDUCTION_FORMAT_UNSIGNED                0x00000001
#define NVC36F_WFI                                                 (0x00000078)
#define NVC36F_WFI_SCOPE                                                   0:0
#define NVC36F_WFI_SCOPE_CURRENT_SCG_TYPE                           0x00000000
#define NVC36F_WFI_SCOPE_CURRENT_VEID                               0x00000000
#define NVC36F_WFI_SCOPE_ALL                                        0x00000001
#define NVC36F_CRC_CHECK                                           (0x0000007c)
#define NVC36F_CRC_CHECK_VALUE                                            31:0
#define NVC36F_YIELD                                               (0x00000080)
#define NVC36F_YIELD_OP                                                    1:0
#define NVC36F_YIELD_OP_NOP                                         0x00000000
#define NVC36F_YIELD_OP_RUNLIST_TIMESLICE                           0x00000002
#define NVC36F_YIELD_OP_TSG                                         0x00000003
#define NVC36F_CLEAR_FAULTED                                       (0x00000084)
#define NVC36F_CLEAR_FAULTED_CHID                                         11:0
#define NVC36F_CLEAR_FAULTED_TYPE                                        31:31
#define NVC36F_CLEAR_FAULTED_TYPE_PBDMA_FAULTED                     0x00000000
#define NVC36F_CLEAR_FAULTED_TYPE_ENG_FAULTED                       0x00000001


/* GPFIFO entry format */
#define NVC36F_GP_ENTRY__SIZE                                   8
#define NVC36F_GP_ENTRY0_FETCH                                0:0
#define NVC36F_GP_ENTRY0_FETCH_UNCONDITIONAL           0x00000000
#define NVC36F_GP_ENTRY0_FETCH_CONDITIONAL             0x00000001
#define NVC36F_GP_ENTRY0_GET                                 31:2
#define NVC36F_GP_ENTRY0_OPERAND                             31:0
#define NVC36F_GP_ENTRY1_GET_HI                               7:0
#define NVC36F_GP_ENTRY1_PRIV                                 8:8
#define NVC36F_GP_ENTRY1_PRIV_USER                     0x00000000
#define NVC36F_GP_ENTRY1_PRIV_KERNEL                   0x00000001
#define NVC36F_GP_ENTRY1_LEVEL                                9:9
#define NVC36F_GP_ENTRY1_LEVEL_MAIN                    0x00000000
#define NVC36F_GP_ENTRY1_LEVEL_SUBROUTINE              0x00000001
#define NVC36F_GP_ENTRY1_LENGTH                             30:10
#define NVC36F_GP_ENTRY1_SYNC                               31:31
#define NVC36F_GP_ENTRY1_SYNC_PROCEED                  0x00000000
#define NVC36F_GP_ENTRY1_SYNC_WAIT                     0x00000001
#define NVC36F_GP_ENTRY1_OPCODE                               7:0
#define NVC36F_GP_ENTRY1_OPCODE_NOP                    0x00000000
#define NVC36F_GP_ENTRY1_OPCODE_ILLEGAL                0x00000001
#define NVC36F_GP_ENTRY1_OPCODE_GP_CRC                 0x00000002
#define NVC36F_GP_ENTRY1_OPCODE_PB_CRC                 0x00000003

/* dma method formats */
#define NVC36F_DMA_METHOD_ADDRESS_OLD                              12:2
#define NVC36F_DMA_METHOD_ADDRESS                                  11:0
#define NVC36F_DMA_SUBDEVICE_MASK                                  15:4
#define NVC36F_DMA_METHOD_SUBCHANNEL                               15:13
#define NVC36F_DMA_TERT_OP                                         17:16
#define NVC36F_DMA_TERT_OP_GRP0_INC_METHOD                         (0x00000000)
#define NVC36F_DMA_TERT_OP_GRP0_SET_SUB_DEV_MASK                   (0x00000001)
#define NVC36F_DMA_TERT_OP_GRP0_STORE_SUB_DEV_MASK                 (0x00000002)
#define NVC36F_DMA_TERT_OP_GRP0_USE_SUB_DEV_MASK                   (0x00000003)
#define NVC36F_DMA_TERT_OP_GRP2_NON_INC_METHOD                     (0x00000000)
#define NVC36F_DMA_METHOD_COUNT_OLD                                28:18
#define NVC36F_DMA_METHOD_COUNT                                    28:16
#define NVC36F_DMA_IMMD_DATA                                       28:16
#define NVC36F_DMA_SEC_OP                                          31:29
#define NVC36F_DMA_SEC_OP_GRP0_USE_TERT                            (0x00000000)
#define NVC36F_DMA_SEC_OP_INC_METHOD                               (0x00000001)
#define NVC36F_DMA_SEC_OP_GRP2_USE_TERT                            (0x00000002)
#define NVC36F_DMA_SEC_OP_NON_INC_METHOD                           (0x00000003)
#define NVC36F_DMA_SEC_OP_IMMD_DATA_METHOD                         (0x00000004)
#define NVC36F_DMA_SEC_OP_ONE_INC                                  (0x00000005)
#define NVC36F_DMA_SEC_OP_RESERVED6                                (0x00000006)
#define NVC36F_DMA_SEC_OP_END_PB_SEGMENT                           (0x00000007)
/* dma incrementing method format */
#define NVC36F_DMA_INCR_ADDRESS                                    11:0
#define NVC36F_DMA_INCR_SUBCHANNEL                                 15:13
#define NVC36F_DMA_INCR_COUNT                                      28:16
#define NVC36F_DMA_INCR_OPCODE                                     31:29
#define NVC36F_DMA_INCR_OPCODE_VALUE                               (0x00000001)
#define NVC36F_DMA_INCR_DATA                                       31:0
/* dma non-incrementing method format */
#define NVC36F_DMA_NONINCR_ADDRESS                                 11:0
#define NVC36F_DMA_NONINCR_SUBCHANNEL                              15:13
#define NVC36F_DMA_NONINCR_COUNT                                   28:16
#define NVC36F_DMA_NONINCR_OPCODE                                  31:29
#define NVC36F_DMA_NONINCR_OPCODE_VALUE                            (0x00000003)
#define NVC36F_DMA_NONINCR_DATA                                    31:0
/* dma increment-once method format */
#define NVC36F_DMA_ONEINCR_ADDRESS                                 11:0
#define NVC36F_DMA_ONEINCR_SUBCHANNEL                              15:13
#define NVC36F_DMA_ONEINCR_COUNT                                   28:16
#define NVC36F_DMA_ONEINCR_OPCODE                                  31:29
#define NVC36F_DMA_ONEINCR_OPCODE_VALUE                            (0x00000005)
#define NVC36F_DMA_ONEINCR_DATA                                    31:0
/* dma no-operation format */
#define NVC36F_DMA_NOP                                             (0x00000000)
/* dma immediate-data format */
#define NVC36F_DMA_IMMD_ADDRESS                                    11:0
#define NVC36F_DMA_IMMD_SUBCHANNEL                                 15:13
#define NVC36F_DMA_IMMD_DATA                                       28:16
#define NVC36F_DMA_IMMD_OPCODE                                     31:29
#define NVC36F_DMA_IMMD_OPCODE_VALUE                               (0x00000004)
/* dma set sub-device mask format */
#define NVC36F_DMA_SET_SUBDEVICE_MASK_VALUE                        15:4
#define NVC36F_DMA_SET_SUBDEVICE_MASK_OPCODE                       31:16
#define NVC36F_DMA_SET_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000001)
/* dma store sub-device mask format */
#define NVC36F_DMA_STORE_SUBDEVICE_MASK_VALUE                      15:4
#define NVC36F_DMA_STORE_SUBDEVICE_MASK_OPCODE                     31:16
#define NVC36F_DMA_STORE_SUBDEVICE_MASK_OPCODE_VALUE               (0x00000002)
/* dma use sub-device mask format */
#define NVC36F_DMA_USE_SUBDEVICE_MASK_OPCODE                       31:16
#define NVC36F_DMA_USE_SUBDEVICE_MASK_OPCODE_VALUE                 (0x00000003)
/* dma end-segment format */
#define NVC36F_DMA_ENDSEG_OPCODE                                   31:29
#define NVC36F_DMA_ENDSEG_OPCODE_VALUE                             (0x00000007)
/* dma legacy incrementing/non-incrementing formats */
#define NVC36F_DMA_ADDRESS                                         12:2
#define NVC36F_DMA_SUBCH                                           15:13
#define NVC36F_DMA_OPCODE3                                         17:16
#define NVC36F_DMA_OPCODE3_NONE                                    (0x00000000)
#define NVC36F_DMA_COUNT                                           28:18
#define NVC36F_DMA_OPCODE                                          31:29
#define NVC36F_DMA_OPCODE_METHOD                                   (0x00000000)
#define NVC36F_DMA_OPCODE_NONINC_METHOD                            (0x00000002)
#define NVC36F_DMA_DATA                                            31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc36f_h_ */
