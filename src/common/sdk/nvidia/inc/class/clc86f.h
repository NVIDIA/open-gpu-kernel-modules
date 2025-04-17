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

#define  HOPPER_CHANNEL_GPFIFO_A                           (0x0000C86F)

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

#define NVC86F_SET_OBJECT                                          (0x00000000)
// NOTE - MEM_OP_A and MEM_OP_B have been replaced in gp100 with methods for
// specifying the page address for a targeted TLB invalidate and the uTLB for
// a targeted REPLAY_CANCEL for UVM.
// The previous MEM_OP_A/B functionality is in MEM_OP_C/D, with slightly
// rearranged fields.
#define NVC86F_MEM_OP_A                                            (0x00000028)
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_CLIENT_UNIT_ID        5:0  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVALIDATION_SIZE                   5:0  // Used to specify size of invalidate, used for invalidates which are not of the REPLAY_CANCEL_TARGETED type
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_CANCEL_TARGET_GPC_ID               10:6  // only relevant for REPLAY_CANCEL_TARGETED
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVAL_SCOPE                         7:6  // only relevant for invalidates with NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_NONE for invalidating  link TLB only, or non-link TLB only or all TLBs
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVAL_SCOPE_ALL_TLBS                  0
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVAL_SCOPE_LINK_TLBS                 1
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVAL_SCOPE_NON_LINK_TLBS             2
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_INVAL_SCOPE_RSVRVD                    3
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_CANCEL_MMU_ENGINE_ID                8:0  // only relevant for REPLAY_CANCEL_VA_GLOBAL
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR                         11:11
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_EN                 0x00000001
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_SYSMEMBAR_DIS                0x00000000
#define NVC86F_MEM_OP_A_TLB_INVALIDATE_TARGET_ADDR_LO                    31:12
#define NVC86F_MEM_OP_B                                            (0x0000002c)
#define NVC86F_MEM_OP_B_TLB_INVALIDATE_TARGET_ADDR_HI                     31:0
#define NVC86F_MEM_OP_C                                            (0x00000030)
#define NVC86F_MEM_OP_C_MEMBAR_TYPE                                        2:0
#define NVC86F_MEM_OP_C_MEMBAR_TYPE_SYS_MEMBAR                      0x00000000
#define NVC86F_MEM_OP_C_MEMBAR_TYPE_MEMBAR                          0x00000001
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB                                 0:0
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_ONE                      0x00000000
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_ALL                      0x00000001  // Probably nonsensical for MMU_TLB_INVALIDATE_TARGETED
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_GPC                                 1:1
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_GPC_ENABLE                   0x00000000
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_GPC_DISABLE                  0x00000001
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY                              4:2  // only relevant if GPC ENABLE
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_NONE                  0x00000000
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START                 0x00000001
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_START_ACK_ALL         0x00000002
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_TARGETED       0x00000003
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_GLOBAL         0x00000004
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_REPLAY_CANCEL_VA_GLOBAL      0x00000005
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE                            6:5  // only relevant if GPC ENABLE
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_NONE                0x00000000
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_GLOBALLY            0x00000001
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACK_TYPE_INTRANODE           0x00000002
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE                         9:7 //only relevant for REPLAY_CANCEL_VA_GLOBAL
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_READ                 0
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_WRITE                1
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_STRONG        2
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_RSVRVD               3
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_WEAK          4
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ATOMIC_ALL           5
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_WRITE_AND_ATOMIC     6
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_ACCESS_TYPE_VIRT_ALL                  7
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL                    9:7  // Invalidate affects this level and all below
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_ALL         0x00000000  // Invalidate tlb caches at all levels of the page table
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_PTE_ONLY    0x00000001
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE0  0x00000002
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE1  0x00000003
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE2  0x00000004
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3  0x00000005
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE4  0x00000006
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE5  0x00000007
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE                          11:10  // only relevant if PDB_ONE
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_VID_MEM             0x00000000
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_COHERENT    0x00000002
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_APERTURE_SYS_MEM_NONCOHERENT 0x00000003
#define NVC86F_MEM_OP_C_TLB_INVALIDATE_PDB_ADDR_LO                       31:12  // only relevant if PDB_ONE
#define NVC86F_MEM_OP_C_ACCESS_COUNTER_CLR_TARGETED_NOTIFY_TAG            19:0
// MEM_OP_D MUST be preceded by MEM_OPs A-C.
#define NVC86F_MEM_OP_D                                            (0x00000034)
#define NVC86F_MEM_OP_D_TLB_INVALIDATE_PDB_ADDR_HI                        26:0  // only relevant if PDB_ONE
#define NVC86F_MEM_OP_D_OPERATION                                        31:27
#define NVC86F_MEM_OP_D_OPERATION_MEMBAR                            0x00000005
#define NVC86F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE                0x00000009
#define NVC86F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE_TARGETED       0x0000000a
#define NVC86F_MEM_OP_D_OPERATION_MMU_OPERATION                     0x0000000b
#define NVC86F_MEM_OP_D_OPERATION_L2_PEERMEM_INVALIDATE             0x0000000d
#define NVC86F_MEM_OP_D_OPERATION_L2_SYSMEM_INVALIDATE              0x0000000e
// CLEAN_LINES is an alias for Tegra/GPU IP usage
#define NVC86F_MEM_OP_B_OPERATION_L2_INVALIDATE_CLEAN_LINES         0x0000000e
#define NVC86F_MEM_OP_D_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVC86F_MEM_OP_D_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVC86F_MEM_OP_D_OPERATION_L2_WAIT_FOR_SYS_PENDING_READS     0x00000015
#define NVC86F_MEM_OP_D_OPERATION_ACCESS_COUNTER_CLR                0x00000016
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE                            1:0
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_MIMC                0x00000000
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_MOMC                0x00000001
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_ALL                 0x00000002
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TYPE_TARGETED            0x00000003
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE                   2:2
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE_MIMC       0x00000000
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_TYPE_MOMC       0x00000001
#define NVC86F_MEM_OP_D_ACCESS_COUNTER_CLR_TARGETED_BANK                   6:3
#define NVC86F_MEM_OP_D_MMU_OPERATION_TYPE                               23:20
#define NVC86F_MEM_OP_D_MMU_OPERATION_TYPE_RESERVED                 0x00000000
#define NVC86F_MEM_OP_D_MMU_OPERATION_TYPE_VIDMEM_ACCESS_BIT_DUMP   0x00000001
#define NVC86F_SEM_ADDR_LO                                         (0x0000005c)
#define NVC86F_SEM_ADDR_LO_OFFSET                                         31:2
#define NVC86F_SEM_ADDR_HI                                         (0x00000060)
#define NVC86F_SEM_ADDR_HI_OFFSET                                         24:0
#define NVC86F_SEM_PAYLOAD_LO                                      (0x00000064)
#define NVC86F_SEM_PAYLOAD_HI                                      (0x00000068)
#define NVC86F_SEM_EXECUTE                                         (0x0000006c)
#define NVC86F_SEM_EXECUTE_OPERATION                                       2:0
#define NVC86F_SEM_EXECUTE_OPERATION_ACQUIRE                        0x00000000
#define NVC86F_SEM_EXECUTE_OPERATION_RELEASE                        0x00000001
#define NVC86F_SEM_EXECUTE_OPERATION_ACQ_CIRC_GEQ                   0x00000003
#define NVC86F_SEM_EXECUTE_ACQUIRE_SWITCH_TSG                            12:12
#define NVC86F_SEM_EXECUTE_ACQUIRE_SWITCH_TSG_EN                    0x00000001
#define NVC86F_SEM_EXECUTE_RELEASE_WFI                                   20:20
#define NVC86F_SEM_EXECUTE_RELEASE_WFI_DIS                          0x00000000
#define NVC86F_SEM_EXECUTE_PAYLOAD_SIZE                                  24:24
#define NVC86F_SEM_EXECUTE_PAYLOAD_SIZE_32BIT                       0x00000000
#define NVC86F_SEM_EXECUTE_RELEASE_TIMESTAMP                             25:25
#define NVC86F_SEM_EXECUTE_RELEASE_TIMESTAMP_DIS                    0x00000000
#define NVC86F_SEM_EXECUTE_RELEASE_TIMESTAMP_EN                     0x00000001
#define NVC86F_WFI                                                 (0x00000078)
#define NVC86F_WFI_SCOPE                                                   0:0
#define NVC86F_WFI_SCOPE_CURRENT_SCG_TYPE                           0x00000000
#define NVC86F_WFI_SCOPE_CURRENT_VEID                               0x00000000
#define NVC86F_WFI_SCOPE_ALL                                        0x00000001

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
