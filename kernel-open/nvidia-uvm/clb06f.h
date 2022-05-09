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

#ifndef _clB06f_h_
#define _clB06f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  MAXWELL_CHANNEL_GPFIFO_A                           (0x0000B06F)

/* class MAXWELL_CHANNEL_GPFIFO  */
#define NVB06F_SET_OBJECT                                          (0x00000000)
#define NVB06F_NOP                                                 (0x00000008)
#define NVB06F_NOP_HANDLE                                                 31:0
#define NVB06F_SEMAPHOREA                                          (0x00000010)
#define NVB06F_SEMAPHOREA_OFFSET_UPPER                                     7:0
#define NVB06F_SEMAPHOREB                                          (0x00000014)
#define NVB06F_SEMAPHOREB_OFFSET_LOWER                                    31:2
#define NVB06F_SEMAPHOREC                                          (0x00000018)
#define NVB06F_SEMAPHOREC_PAYLOAD                                         31:0
#define NVB06F_SEMAPHORED                                          (0x0000001C)
#define NVB06F_SEMAPHORED_OPERATION                                        4:0
#define NVB06F_SEMAPHORED_OPERATION_ACQUIRE                         0x00000001
#define NVB06F_SEMAPHORED_OPERATION_RELEASE                         0x00000002
#define NVB06F_SEMAPHORED_OPERATION_ACQ_GEQ                         0x00000004
#define NVB06F_SEMAPHORED_ACQUIRE_SWITCH                                 12:12
#define NVB06F_SEMAPHORED_ACQUIRE_SWITCH_DISABLED                   0x00000000
#define NVB06F_SEMAPHORED_ACQUIRE_SWITCH_ENABLED                    0x00000001
#define NVB06F_SEMAPHORED_RELEASE_WFI                                    20:20
#define NVB06F_SEMAPHORED_RELEASE_WFI_EN                            0x00000000
#define NVB06F_SEMAPHORED_RELEASE_WFI_DIS                           0x00000001
#define NVB06F_SEMAPHORED_RELEASE_SIZE                                   24:24
#define NVB06F_SEMAPHORED_RELEASE_SIZE_16BYTE                       0x00000000
#define NVB06F_SEMAPHORED_RELEASE_SIZE_4BYTE                        0x00000001

#define NVB06F_NON_STALL_INTERRUPT                                 (0x00000020)
// NOTE - MEM_OP_A and MEM_OP_B have been removed for gm20x to make room for
// possible future MEM_OP features.  MEM_OP_C/D have identical functionality
// to the previous MEM_OP_A/B methods.
#define NVB06F_MEM_OP_C                                            (0x00000030)
#define NVB06F_MEM_OP_C_OPERAND_LOW                                       31:2
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_PDB                                 0:0
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_PDB_ONE                      0x00000000
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_PDB_ALL                      0x00000001
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_GPC                                 1:1
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_GPC_ENABLE                   0x00000000
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_GPC_DISABLE                  0x00000001
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_TARGET                            11:10
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_TARGET_VID_MEM               0x00000000
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_TARGET_SYS_MEM_COHERENT      0x00000002
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_TARGET_SYS_MEM_NONCOHERENT   0x00000003
#define NVB06F_MEM_OP_C_TLB_INVALIDATE_ADDR_LO                           31:12
#define NVB06F_MEM_OP_D                                            (0x00000034)
#define NVB06F_MEM_OP_D_OPERAND_HIGH                                       7:0
#define NVB06F_MEM_OP_D_OPERATION                                        31:27
#define NVB06F_MEM_OP_D_OPERATION_MEMBAR                            0x00000005
#define NVB06F_MEM_OP_D_OPERATION_MMU_TLB_INVALIDATE                0x00000009
#define NVB06F_MEM_OP_D_OPERATION_L2_PEERMEM_INVALIDATE             0x0000000d
#define NVB06F_MEM_OP_D_OPERATION_L2_SYSMEM_INVALIDATE              0x0000000e
#define NVB06F_MEM_OP_D_OPERATION_L2_CLEAN_COMPTAGS                 0x0000000f
#define NVB06F_MEM_OP_D_OPERATION_L2_FLUSH_DIRTY                    0x00000010
#define NVB06F_MEM_OP_D_TLB_INVALIDATE_ADDR_HI                             7:0
#define NVB06F_WFI                                                 (0x00000078)

/* GPFIFO entry format */
#define NVB06F_GP_ENTRY__SIZE                                   8
#define NVB06F_GP_ENTRY0_GET                                 31:2
#define NVB06F_GP_ENTRY0_OPERAND                             31:0
#define NVB06F_GP_ENTRY1_GET_HI                               7:0
#define NVB06F_GP_ENTRY1_PRIV                                 8:8
#define NVB06F_GP_ENTRY1_PRIV_USER                     0x00000000
#define NVB06F_GP_ENTRY1_PRIV_KERNEL                   0x00000001
#define NVB06F_GP_ENTRY1_LEVEL                                9:9
#define NVB06F_GP_ENTRY1_LEVEL_MAIN                    0x00000000
#define NVB06F_GP_ENTRY1_LEVEL_SUBROUTINE              0x00000001
#define NVB06F_GP_ENTRY1_LENGTH                             30:10

/* dma method formats */
#define NVB06F_DMA_SEC_OP                                          31:29
#define NVB06F_DMA_SEC_OP_INC_METHOD                               (0x00000001)
#define NVB06F_DMA_SEC_OP_NON_INC_METHOD                           (0x00000003)
/* dma incrementing method format */
#define NVB06F_DMA_INCR_ADDRESS                                    11:0
#define NVB06F_DMA_INCR_SUBCHANNEL                                 15:13
#define NVB06F_DMA_INCR_COUNT                                      28:16
#define NVB06F_DMA_INCR_OPCODE                                     31:29
#define NVB06F_DMA_INCR_OPCODE_VALUE                               (0x00000001)
#define NVB06F_DMA_INCR_DATA                                       31:0
/* dma non-incrementing method format */
#define NVB06F_DMA_NONINCR_ADDRESS                                 11:0
#define NVB06F_DMA_NONINCR_SUBCHANNEL                              15:13
#define NVB06F_DMA_NONINCR_COUNT                                   28:16
#define NVB06F_DMA_NONINCR_OPCODE                                  31:29
#define NVB06F_DMA_NONINCR_OPCODE_VALUE                            (0x00000003)
#define NVB06F_DMA_NONINCR_DATA                                    31:0
/* dma increment-once method format */
#define NVB06F_DMA_ONEINCR_ADDRESS                                 11:0
#define NVB06F_DMA_ONEINCR_SUBCHANNEL                              15:13
#define NVB06F_DMA_ONEINCR_COUNT                                   28:16
#define NVB06F_DMA_ONEINCR_OPCODE                                  31:29
#define NVB06F_DMA_ONEINCR_OPCODE_VALUE                            (0x00000005)
#define NVB06F_DMA_ONEINCR_DATA                                    31:0
/* dma no-operation format */
#define NVB06F_DMA_NOP                                             (0x00000000)
/* dma immediate-data format */
#define NVB06F_DMA_IMMD_ADDRESS                                    11:0
#define NVB06F_DMA_IMMD_SUBCHANNEL                                 15:13
#define NVB06F_DMA_IMMD_DATA                                       28:16
#define NVB06F_DMA_IMMD_OPCODE                                     31:29
#define NVB06F_DMA_IMMD_OPCODE_VALUE                               (0x00000004)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clB06F_h_ */
