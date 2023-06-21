/*******************************************************************************
    Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "nvtypes.h"

#ifndef _clc3b5_h_
#define _clc3b5_h_

#ifdef __cplusplus
extern "C" {
#endif

#define VOLTA_DMA_COPY_A                                                            (0x0000C3B5)

#define NVC3B5_NOP                                                              (0x00000100)
#define NVC3B5_NOP_PARAMETER                                                    31:0
#define NVC3B5_PM_TRIGGER                                                       (0x00000140)
#define NVC3B5_PM_TRIGGER_V                                                     31:0
#define NVC3B5_SET_SEMAPHORE_A                                                  (0x00000240)
#define NVC3B5_SET_SEMAPHORE_A_UPPER                                            16:0
#define NVC3B5_SET_SEMAPHORE_B                                                  (0x00000244)
#define NVC3B5_SET_SEMAPHORE_B_LOWER                                            31:0
#define NVC3B5_SET_SEMAPHORE_PAYLOAD                                            (0x00000248)
#define NVC3B5_SET_SEMAPHORE_PAYLOAD_PAYLOAD                                    31:0
#define NVC3B5_SET_RENDER_ENABLE_A                                              (0x00000254)
#define NVC3B5_SET_RENDER_ENABLE_A_UPPER                                        7:0
#define NVC3B5_SET_RENDER_ENABLE_B                                              (0x00000258)
#define NVC3B5_SET_RENDER_ENABLE_B_LOWER                                        31:0
#define NVC3B5_SET_RENDER_ENABLE_C                                              (0x0000025C)
#define NVC3B5_SET_RENDER_ENABLE_C_MODE                                         2:0
#define NVC3B5_SET_RENDER_ENABLE_C_MODE_FALSE                                   (0x00000000)
#define NVC3B5_SET_RENDER_ENABLE_C_MODE_TRUE                                    (0x00000001)
#define NVC3B5_SET_RENDER_ENABLE_C_MODE_CONDITIONAL                             (0x00000002)
#define NVC3B5_SET_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                         (0x00000003)
#define NVC3B5_SET_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                     (0x00000004)
#define NVC3B5_SET_SRC_PHYS_MODE                                                (0x00000260)
#define NVC3B5_SET_SRC_PHYS_MODE_TARGET                                         1:0
#define NVC3B5_SET_SRC_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVC3B5_SET_SRC_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVC3B5_SET_SRC_PHYS_MODE_TARGET_NONCOHERENT_SYSMEM                      (0x00000002)
#define NVC3B5_SET_SRC_PHYS_MODE_BASIC_KIND                                     5:2
#define NVC3B5_SET_DST_PHYS_MODE                                                (0x00000264)
#define NVC3B5_SET_DST_PHYS_MODE_TARGET                                         1:0
#define NVC3B5_SET_DST_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVC3B5_SET_DST_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVC3B5_SET_DST_PHYS_MODE_TARGET_NONCOHERENT_SYSMEM                      (0x00000002)
#define NVC3B5_SET_DST_PHYS_MODE_BASIC_KIND                                     5:2
#define NVC3B5_LAUNCH_DMA                                                       (0x00000300)
#define NVC3B5_LAUNCH_DMA_DATA_TRANSFER_TYPE                                    1:0
#define NVC3B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NONE                               (0x00000000)
#define NVC3B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PIPELINED                          (0x00000001)
#define NVC3B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_NON_PIPELINED                      (0x00000002)
#define NVC3B5_LAUNCH_DMA_FLUSH_ENABLE                                          2:2
#define NVC3B5_LAUNCH_DMA_FLUSH_ENABLE_FALSE                                    (0x00000000)
#define NVC3B5_LAUNCH_DMA_FLUSH_ENABLE_TRUE                                     (0x00000001)
#define NVC3B5_LAUNCH_DMA_FLUSH_TYPE                                            25:25
#define NVC3B5_LAUNCH_DMA_FLUSH_TYPE_SYS                                        (0x00000000)
#define NVC3B5_LAUNCH_DMA_FLUSH_TYPE_GL                                         (0x00000001)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_TYPE                                        4:3
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_TYPE_NONE                                   (0x00000000)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_ONE_WORD_SEMAPHORE             (0x00000001)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_FOUR_WORD_SEMAPHORE            (0x00000002)
#define NVC3B5_LAUNCH_DMA_INTERRUPT_TYPE                                        6:5
#define NVC3B5_LAUNCH_DMA_INTERRUPT_TYPE_NONE                                   (0x00000000)
#define NVC3B5_LAUNCH_DMA_INTERRUPT_TYPE_BLOCKING                               (0x00000001)
#define NVC3B5_LAUNCH_DMA_INTERRUPT_TYPE_NON_BLOCKING                           (0x00000002)
#define NVC3B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT                                     7:7
#define NVC3B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_BLOCKLINEAR                         (0x00000000)
#define NVC3B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC3B5_LAUNCH_DMA_DST_MEMORY_LAYOUT                                     8:8
#define NVC3B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_BLOCKLINEAR                         (0x00000000)
#define NVC3B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVC3B5_LAUNCH_DMA_MULTI_LINE_ENABLE                                     9:9
#define NVC3B5_LAUNCH_DMA_MULTI_LINE_ENABLE_FALSE                               (0x00000000)
#define NVC3B5_LAUNCH_DMA_MULTI_LINE_ENABLE_TRUE                                (0x00000001)
#define NVC3B5_LAUNCH_DMA_REMAP_ENABLE                                          10:10
#define NVC3B5_LAUNCH_DMA_REMAP_ENABLE_FALSE                                    (0x00000000)
#define NVC3B5_LAUNCH_DMA_REMAP_ENABLE_TRUE                                     (0x00000001)
#define NVC3B5_LAUNCH_DMA_FORCE_RMWDISABLE                                      11:11
#define NVC3B5_LAUNCH_DMA_FORCE_RMWDISABLE_FALSE                                (0x00000000)
#define NVC3B5_LAUNCH_DMA_FORCE_RMWDISABLE_TRUE                                 (0x00000001)
#define NVC3B5_LAUNCH_DMA_SRC_TYPE                                              12:12
#define NVC3B5_LAUNCH_DMA_SRC_TYPE_VIRTUAL                                      (0x00000000)
#define NVC3B5_LAUNCH_DMA_SRC_TYPE_PHYSICAL                                     (0x00000001)
#define NVC3B5_LAUNCH_DMA_DST_TYPE                                              13:13
#define NVC3B5_LAUNCH_DMA_DST_TYPE_VIRTUAL                                      (0x00000000)
#define NVC3B5_LAUNCH_DMA_DST_TYPE_PHYSICAL                                     (0x00000001)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION                                   17:14
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IMIN                              (0x00000000)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IMAX                              (0x00000001)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IXOR                              (0x00000002)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IAND                              (0x00000003)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IOR                               (0x00000004)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_IADD                              (0x00000005)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_INC                               (0x00000006)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_DEC                               (0x00000007)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_FADD                              (0x0000000A)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN                              18:18
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN_SIGNED                       (0x00000000)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_SIGN_UNSIGNED                     (0x00000001)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE                            19:19
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE_FALSE                      (0x00000000)
#define NVC3B5_LAUNCH_DMA_SEMAPHORE_REDUCTION_ENABLE_TRUE                       (0x00000001)
#define NVC3B5_LAUNCH_DMA_SRC_BYPASS_L2                                         20:20
#define NVC3B5_LAUNCH_DMA_SRC_BYPASS_L2_USE_PTE_SETTING                         (0x00000000)
#define NVC3B5_LAUNCH_DMA_SRC_BYPASS_L2_FORCE_VOLATILE                          (0x00000001)
#define NVC3B5_LAUNCH_DMA_DST_BYPASS_L2                                         21:21
#define NVC3B5_LAUNCH_DMA_DST_BYPASS_L2_USE_PTE_SETTING                         (0x00000000)
#define NVC3B5_LAUNCH_DMA_DST_BYPASS_L2_FORCE_VOLATILE                          (0x00000001)
#define NVC3B5_LAUNCH_DMA_VPRMODE                                               23:22
#define NVC3B5_LAUNCH_DMA_VPRMODE_VPR_NONE                                      (0x00000000)
#define NVC3B5_LAUNCH_DMA_VPRMODE_VPR_VID2VID                                   (0x00000001)
#define NVC3B5_LAUNCH_DMA_RESERVED_START_OF_COPY                                24:24
#define NVC3B5_LAUNCH_DMA_RESERVED_ERR_CODE                                     31:28
#define NVC3B5_OFFSET_IN_UPPER                                                  (0x00000400)
#define NVC3B5_OFFSET_IN_UPPER_UPPER                                            16:0
#define NVC3B5_OFFSET_IN_LOWER                                                  (0x00000404)
#define NVC3B5_OFFSET_IN_LOWER_VALUE                                            31:0
#define NVC3B5_OFFSET_OUT_UPPER                                                 (0x00000408)
#define NVC3B5_OFFSET_OUT_UPPER_UPPER                                           16:0
#define NVC3B5_OFFSET_OUT_LOWER                                                 (0x0000040C)
#define NVC3B5_OFFSET_OUT_LOWER_VALUE                                           31:0
#define NVC3B5_PITCH_IN                                                         (0x00000410)
#define NVC3B5_PITCH_IN_VALUE                                                   31:0
#define NVC3B5_PITCH_OUT                                                        (0x00000414)
#define NVC3B5_PITCH_OUT_VALUE                                                  31:0
#define NVC3B5_LINE_LENGTH_IN                                                   (0x00000418)
#define NVC3B5_LINE_LENGTH_IN_VALUE                                             31:0
#define NVC3B5_LINE_COUNT                                                       (0x0000041C)
#define NVC3B5_LINE_COUNT_VALUE                                                 31:0
#define NVC3B5_SET_REMAP_CONST_A                                                (0x00000700)
#define NVC3B5_SET_REMAP_CONST_A_V                                              31:0
#define NVC3B5_SET_REMAP_CONST_B                                                (0x00000704)
#define NVC3B5_SET_REMAP_CONST_B_V                                              31:0
#define NVC3B5_SET_REMAP_COMPONENTS                                             (0x00000708)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X                                       2:0
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_SRC_X                                 (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_SRC_Y                                 (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_SRC_Z                                 (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_SRC_W                                 (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_CONST_A                               (0x00000004)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_CONST_B                               (0x00000005)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_X_NO_WRITE                              (0x00000006)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y                                       6:4
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_SRC_X                                 (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y                                 (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Z                                 (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_SRC_W                                 (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_CONST_A                               (0x00000004)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_CONST_B                               (0x00000005)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE                              (0x00000006)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z                                       10:8
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_SRC_X                                 (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Y                                 (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z                                 (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_SRC_W                                 (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_CONST_A                               (0x00000004)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_CONST_B                               (0x00000005)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE                              (0x00000006)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W                                       14:12
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_SRC_X                                 (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_SRC_Y                                 (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_SRC_Z                                 (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_SRC_W                                 (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_CONST_A                               (0x00000004)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_CONST_B                               (0x00000005)
#define NVC3B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE                              (0x00000006)
#define NVC3B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE                              17:16
#define NVC3B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_ONE                          (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_TWO                          (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_THREE                        (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_FOUR                         (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS                          21:20
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_ONE                      (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_TWO                      (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_THREE                    (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_FOUR                     (0x00000003)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS                          25:24
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_ONE                      (0x00000000)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_TWO                      (0x00000001)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_THREE                    (0x00000002)
#define NVC3B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_FOUR                     (0x00000003)
#define NVC3B5_SET_DST_BLOCK_SIZE                                               (0x0000070C)
#define NVC3B5_SET_DST_BLOCK_SIZE_WIDTH                                         3:0
#define NVC3B5_SET_DST_BLOCK_SIZE_WIDTH_ONE_GOB                                 (0x00000000)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT                                        7:4
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_ONE_GOB                                (0x00000000)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_TWO_GOBS                               (0x00000001)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_FOUR_GOBS                              (0x00000002)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                             (0x00000003)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                           (0x00000004)
#define NVC3B5_SET_DST_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                         (0x00000005)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH                                         11:8
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_ONE_GOB                                 (0x00000000)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_TWO_GOBS                                (0x00000001)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_FOUR_GOBS                               (0x00000002)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_EIGHT_GOBS                              (0x00000003)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                            (0x00000004)
#define NVC3B5_SET_DST_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                          (0x00000005)
#define NVC3B5_SET_DST_BLOCK_SIZE_GOB_HEIGHT                                    15:12
#define NVC3B5_SET_DST_BLOCK_SIZE_GOB_HEIGHT_GOB_HEIGHT_FERMI_8                 (0x00000001)
#define NVC3B5_SET_DST_WIDTH                                                    (0x00000710)
#define NVC3B5_SET_DST_WIDTH_V                                                  31:0
#define NVC3B5_SET_DST_HEIGHT                                                   (0x00000714)
#define NVC3B5_SET_DST_HEIGHT_V                                                 31:0
#define NVC3B5_SET_DST_DEPTH                                                    (0x00000718)
#define NVC3B5_SET_DST_DEPTH_V                                                  31:0
#define NVC3B5_SET_DST_LAYER                                                    (0x0000071C)
#define NVC3B5_SET_DST_LAYER_V                                                  31:0
#define NVC3B5_SET_DST_ORIGIN                                                   (0x00000720)
#define NVC3B5_SET_DST_ORIGIN_X                                                 15:0
#define NVC3B5_SET_DST_ORIGIN_Y                                                 31:16
#define NVC3B5_SET_SRC_BLOCK_SIZE                                               (0x00000728)
#define NVC3B5_SET_SRC_BLOCK_SIZE_WIDTH                                         3:0
#define NVC3B5_SET_SRC_BLOCK_SIZE_WIDTH_ONE_GOB                                 (0x00000000)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT                                        7:4
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_ONE_GOB                                (0x00000000)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_TWO_GOBS                               (0x00000001)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_FOUR_GOBS                              (0x00000002)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                             (0x00000003)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                           (0x00000004)
#define NVC3B5_SET_SRC_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                         (0x00000005)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH                                         11:8
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_ONE_GOB                                 (0x00000000)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_TWO_GOBS                                (0x00000001)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_FOUR_GOBS                               (0x00000002)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_EIGHT_GOBS                              (0x00000003)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                            (0x00000004)
#define NVC3B5_SET_SRC_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                          (0x00000005)
#define NVC3B5_SET_SRC_BLOCK_SIZE_GOB_HEIGHT                                    15:12
#define NVC3B5_SET_SRC_BLOCK_SIZE_GOB_HEIGHT_GOB_HEIGHT_FERMI_8                 (0x00000001)
#define NVC3B5_SET_SRC_WIDTH                                                    (0x0000072C)
#define NVC3B5_SET_SRC_WIDTH_V                                                  31:0
#define NVC3B5_SET_SRC_HEIGHT                                                   (0x00000730)
#define NVC3B5_SET_SRC_HEIGHT_V                                                 31:0
#define NVC3B5_SET_SRC_DEPTH                                                    (0x00000734)
#define NVC3B5_SET_SRC_DEPTH_V                                                  31:0
#define NVC3B5_SET_SRC_LAYER                                                    (0x00000738)
#define NVC3B5_SET_SRC_LAYER_V                                                  31:0
#define NVC3B5_SET_SRC_ORIGIN                                                   (0x0000073C)
#define NVC3B5_SET_SRC_ORIGIN_X                                                 15:0
#define NVC3B5_SET_SRC_ORIGIN_Y                                                 31:16
#define NVC3B5_SRC_ORIGIN_X                                                     (0x00000744)
#define NVC3B5_SRC_ORIGIN_X_VALUE                                               31:0
#define NVC3B5_SRC_ORIGIN_Y                                                     (0x00000748)
#define NVC3B5_SRC_ORIGIN_Y_VALUE                                               31:0
#define NVC3B5_DST_ORIGIN_X                                                     (0x0000074C)
#define NVC3B5_DST_ORIGIN_X_VALUE                                               31:0
#define NVC3B5_DST_ORIGIN_Y                                                     (0x00000750)
#define NVC3B5_DST_ORIGIN_Y_VALUE                                               31:0
#define NVC3B5_PM_TRIGGER_END                                                   (0x00001114)
#define NVC3B5_PM_TRIGGER_END_V                                                 31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _clc3b5_h

