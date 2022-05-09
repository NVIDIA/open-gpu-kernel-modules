/*******************************************************************************
    Copyright (c) 2014 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef _clc06f_h_
#define _clc06f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  PASCAL_CHANNEL_GPFIFO_A                           (0x0000C06F)

/* class PASCAL_CHANNEL_GPFIFO_A */
#define NVC06F_SET_OBJECT                                          (0x00000000)
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
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH                                 12:12
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH_DISABLED                   0x00000000
#define NVC06F_SEMAPHORED_ACQUIRE_SWITCH_ENABLED                    0x00000001


/* GPFIFO entry format */
#define NVC06F_GP_ENTRY__SIZE                                   8
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
#define NVC06F_MEM_OP_B_OPERATION_L2_INVALIDATE_CLEAN_LINES         0x0000000e
#define NVC06F_MEM_OP_D_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVC06F_MEM_OP_D_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVC06F_MEM_OP_D_OPERATION_L2_WAIT_FOR_SYS_PENDING_READS     0x00000015
#define NVC06F_SET_REFERENCE                                       (0x00000050)
#define NVC06F_SET_REFERENCE_COUNT                                        31:0

#define NVC06F_WFI                                                 (0x00000078)
#define NVC06F_WFI_SCOPE                                                   0:0
#define NVC06F_WFI_SCOPE_CURRENT_SCG_TYPE                           0x00000000
#define NVC06F_WFI_SCOPE_ALL                                        0x00000001


#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc06f_h_ */
