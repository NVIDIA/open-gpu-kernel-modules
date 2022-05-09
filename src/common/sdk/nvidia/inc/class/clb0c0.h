/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl_maxwell_compute_a_h_
#define _cl_maxwell_compute_a_h_

/* AUTO GENERATED FILE -- DO NOT EDIT */
/* Command: ../../../../class/bin/sw_header.pl maxwell_compute_a */

#include "nvtypes.h"

#define MAXWELL_COMPUTE_A    0xB0C0

#define NVB0C0_SET_OBJECT                                                                                  0x0000
#define NVB0C0_SET_OBJECT_CLASS_ID                                                                           15:0
#define NVB0C0_SET_OBJECT_ENGINE_ID                                                                         20:16

#define NVB0C0_NO_OPERATION                                                                                0x0100
#define NVB0C0_NO_OPERATION_V                                                                                31:0

#define NVB0C0_SET_NOTIFY_A                                                                                0x0104
#define NVB0C0_SET_NOTIFY_A_ADDRESS_UPPER                                                                     7:0

#define NVB0C0_SET_NOTIFY_B                                                                                0x0108
#define NVB0C0_SET_NOTIFY_B_ADDRESS_LOWER                                                                    31:0

#define NVB0C0_NOTIFY                                                                                      0x010c
#define NVB0C0_NOTIFY_TYPE                                                                                   31:0
#define NVB0C0_NOTIFY_TYPE_WRITE_ONLY                                                                  0x00000000
#define NVB0C0_NOTIFY_TYPE_WRITE_THEN_AWAKEN                                                           0x00000001

#define NVB0C0_WAIT_FOR_IDLE                                                                               0x0110
#define NVB0C0_WAIT_FOR_IDLE_V                                                                               31:0

#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_A                                                                  0x0130
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_A_OFFSET_UPPER                                                        7:0

#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_B                                                                  0x0134
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_B_OFFSET_LOWER                                                       31:0

#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C                                                                  0x0138
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE                                                                2:0
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE_FALSE                                                   0x00000000
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE_TRUE                                                    0x00000001
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE_CONDITIONAL                                             0x00000002
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                         0x00000003
#define NVB0C0_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                     0x00000004

#define NVB0C0_SEND_GO_IDLE                                                                                0x013c
#define NVB0C0_SEND_GO_IDLE_V                                                                                31:0

#define NVB0C0_PM_TRIGGER                                                                                  0x0140
#define NVB0C0_PM_TRIGGER_V                                                                                  31:0

#define NVB0C0_PM_TRIGGER_WFI                                                                              0x0144
#define NVB0C0_PM_TRIGGER_WFI_V                                                                              31:0

#define NVB0C0_SET_INSTRUMENTATION_METHOD_HEADER                                                           0x0150
#define NVB0C0_SET_INSTRUMENTATION_METHOD_HEADER_V                                                           31:0

#define NVB0C0_SET_INSTRUMENTATION_METHOD_DATA                                                             0x0154
#define NVB0C0_SET_INSTRUMENTATION_METHOD_DATA_V                                                             31:0

#define NVB0C0_LINE_LENGTH_IN                                                                              0x0180
#define NVB0C0_LINE_LENGTH_IN_VALUE                                                                          31:0

#define NVB0C0_LINE_COUNT                                                                                  0x0184
#define NVB0C0_LINE_COUNT_VALUE                                                                              31:0

#define NVB0C0_OFFSET_OUT_UPPER                                                                            0x0188
#define NVB0C0_OFFSET_OUT_UPPER_VALUE                                                                         7:0

#define NVB0C0_OFFSET_OUT                                                                                  0x018c
#define NVB0C0_OFFSET_OUT_VALUE                                                                              31:0

#define NVB0C0_PITCH_OUT                                                                                   0x0190
#define NVB0C0_PITCH_OUT_VALUE                                                                               31:0

#define NVB0C0_SET_DST_BLOCK_SIZE                                                                          0x0194
#define NVB0C0_SET_DST_BLOCK_SIZE_WIDTH                                                                       3:0
#define NVB0C0_SET_DST_BLOCK_SIZE_WIDTH_ONE_GOB                                                        0x00000000
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT                                                                      7:4
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_ONE_GOB                                                       0x00000000
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_TWO_GOBS                                                      0x00000001
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                                     0x00000002
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                                    0x00000003
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                                  0x00000004
#define NVB0C0_SET_DST_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                                0x00000005
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH                                                                      11:8
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_ONE_GOB                                                        0x00000000
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_TWO_GOBS                                                       0x00000001
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_FOUR_GOBS                                                      0x00000002
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_EIGHT_GOBS                                                     0x00000003
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                                                   0x00000004
#define NVB0C0_SET_DST_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                                                 0x00000005

#define NVB0C0_SET_DST_WIDTH                                                                               0x0198
#define NVB0C0_SET_DST_WIDTH_V                                                                               31:0

#define NVB0C0_SET_DST_HEIGHT                                                                              0x019c
#define NVB0C0_SET_DST_HEIGHT_V                                                                              31:0

#define NVB0C0_SET_DST_DEPTH                                                                               0x01a0
#define NVB0C0_SET_DST_DEPTH_V                                                                               31:0

#define NVB0C0_SET_DST_LAYER                                                                               0x01a4
#define NVB0C0_SET_DST_LAYER_V                                                                               31:0

#define NVB0C0_SET_DST_ORIGIN_BYTES_X                                                                      0x01a8
#define NVB0C0_SET_DST_ORIGIN_BYTES_X_V                                                                      19:0

#define NVB0C0_SET_DST_ORIGIN_SAMPLES_Y                                                                    0x01ac
#define NVB0C0_SET_DST_ORIGIN_SAMPLES_Y_V                                                                    15:0

#define NVB0C0_LAUNCH_DMA                                                                                  0x01b0
#define NVB0C0_LAUNCH_DMA_DST_MEMORY_LAYOUT                                                                   0:0
#define NVB0C0_LAUNCH_DMA_DST_MEMORY_LAYOUT_BLOCKLINEAR                                                0x00000000
#define NVB0C0_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                                                      0x00000001
#define NVB0C0_LAUNCH_DMA_COMPLETION_TYPE                                                                     5:4
#define NVB0C0_LAUNCH_DMA_COMPLETION_TYPE_FLUSH_DISABLE                                                0x00000000
#define NVB0C0_LAUNCH_DMA_COMPLETION_TYPE_FLUSH_ONLY                                                   0x00000001
#define NVB0C0_LAUNCH_DMA_COMPLETION_TYPE_RELEASE_SEMAPHORE                                            0x00000002
#define NVB0C0_LAUNCH_DMA_INTERRUPT_TYPE                                                                      9:8
#define NVB0C0_LAUNCH_DMA_INTERRUPT_TYPE_NONE                                                          0x00000000
#define NVB0C0_LAUNCH_DMA_INTERRUPT_TYPE_INTERRUPT                                                     0x00000001
#define NVB0C0_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE                                                             12:12
#define NVB0C0_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE_FOUR_WORDS                                             0x00000000
#define NVB0C0_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE_ONE_WORD                                               0x00000001
#define NVB0C0_LAUNCH_DMA_REDUCTION_ENABLE                                                                    1:1
#define NVB0C0_LAUNCH_DMA_REDUCTION_ENABLE_FALSE                                                       0x00000000
#define NVB0C0_LAUNCH_DMA_REDUCTION_ENABLE_TRUE                                                        0x00000001
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP                                                                      15:13
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_ADD                                                         0x00000000
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_MIN                                                         0x00000001
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_MAX                                                         0x00000002
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_INC                                                         0x00000003
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_DEC                                                         0x00000004
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_AND                                                         0x00000005
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_OR                                                          0x00000006
#define NVB0C0_LAUNCH_DMA_REDUCTION_OP_RED_XOR                                                         0x00000007
#define NVB0C0_LAUNCH_DMA_REDUCTION_FORMAT                                                                    3:2
#define NVB0C0_LAUNCH_DMA_REDUCTION_FORMAT_UNSIGNED_32                                                 0x00000000
#define NVB0C0_LAUNCH_DMA_REDUCTION_FORMAT_SIGNED_32                                                   0x00000001
#define NVB0C0_LAUNCH_DMA_SYSMEMBAR_DISABLE                                                                   6:6
#define NVB0C0_LAUNCH_DMA_SYSMEMBAR_DISABLE_FALSE                                                      0x00000000
#define NVB0C0_LAUNCH_DMA_SYSMEMBAR_DISABLE_TRUE                                                       0x00000001

#define NVB0C0_LOAD_INLINE_DATA                                                                            0x01b4
#define NVB0C0_LOAD_INLINE_DATA_V                                                                            31:0

#define NVB0C0_SET_I2M_SEMAPHORE_A                                                                         0x01dc
#define NVB0C0_SET_I2M_SEMAPHORE_A_OFFSET_UPPER                                                               7:0

#define NVB0C0_SET_I2M_SEMAPHORE_B                                                                         0x01e0
#define NVB0C0_SET_I2M_SEMAPHORE_B_OFFSET_LOWER                                                              31:0

#define NVB0C0_SET_I2M_SEMAPHORE_C                                                                         0x01e4
#define NVB0C0_SET_I2M_SEMAPHORE_C_PAYLOAD                                                                   31:0

#define NVB0C0_SET_I2M_SPARE_NOOP00                                                                        0x01f0
#define NVB0C0_SET_I2M_SPARE_NOOP00_V                                                                        31:0

#define NVB0C0_SET_I2M_SPARE_NOOP01                                                                        0x01f4
#define NVB0C0_SET_I2M_SPARE_NOOP01_V                                                                        31:0

#define NVB0C0_SET_I2M_SPARE_NOOP02                                                                        0x01f8
#define NVB0C0_SET_I2M_SPARE_NOOP02_V                                                                        31:0

#define NVB0C0_SET_I2M_SPARE_NOOP03                                                                        0x01fc
#define NVB0C0_SET_I2M_SPARE_NOOP03_V                                                                        31:0

#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_A                                                              0x0200
#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_A_ADDRESS_UPPER                                                   7:0

#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_B                                                              0x0204
#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_B_ADDRESS_LOWER                                                  31:0

#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_C                                                              0x0208
#define NVB0C0_SET_VALID_SPAN_OVERFLOW_AREA_C_SIZE                                                           31:0

#define NVB0C0_SET_COALESCE_WAITING_PERIOD_UNIT                                                            0x020c
#define NVB0C0_SET_COALESCE_WAITING_PERIOD_UNIT_CLOCKS                                                       31:0

#define NVB0C0_PERFMON_TRANSFER                                                                            0x0210
#define NVB0C0_PERFMON_TRANSFER_V                                                                            31:0

#define NVB0C0_SET_SHADER_SHARED_MEMORY_WINDOW                                                             0x0214
#define NVB0C0_SET_SHADER_SHARED_MEMORY_WINDOW_BASE_ADDRESS                                                  31:0

#define NVB0C0_SET_SELECT_MAXWELL_TEXTURE_HEADERS                                                          0x0218
#define NVB0C0_SET_SELECT_MAXWELL_TEXTURE_HEADERS_V                                                           0:0
#define NVB0C0_SET_SELECT_MAXWELL_TEXTURE_HEADERS_V_FALSE                                              0x00000000
#define NVB0C0_SET_SELECT_MAXWELL_TEXTURE_HEADERS_V_TRUE                                               0x00000001

#define NVB0C0_INVALIDATE_SHADER_CACHES                                                                    0x021c
#define NVB0C0_INVALIDATE_SHADER_CACHES_INSTRUCTION                                                           0:0
#define NVB0C0_INVALIDATE_SHADER_CACHES_INSTRUCTION_FALSE                                              0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_INSTRUCTION_TRUE                                               0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_DATA                                                                  4:4
#define NVB0C0_INVALIDATE_SHADER_CACHES_DATA_FALSE                                                     0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_DATA_TRUE                                                      0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_CONSTANT                                                            12:12
#define NVB0C0_INVALIDATE_SHADER_CACHES_CONSTANT_FALSE                                                 0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_CONSTANT_TRUE                                                  0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_LOCKS                                                                 1:1
#define NVB0C0_INVALIDATE_SHADER_CACHES_LOCKS_FALSE                                                    0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_LOCKS_TRUE                                                     0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_FLUSH_DATA                                                            2:2
#define NVB0C0_INVALIDATE_SHADER_CACHES_FLUSH_DATA_FALSE                                               0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_FLUSH_DATA_TRUE                                                0x00000001

#define NVB0C0_SET_RESERVED_SW_METHOD00                                                                    0x0220
#define NVB0C0_SET_RESERVED_SW_METHOD00_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD01                                                                    0x0224
#define NVB0C0_SET_RESERVED_SW_METHOD01_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD02                                                                    0x0228
#define NVB0C0_SET_RESERVED_SW_METHOD02_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD03                                                                    0x022c
#define NVB0C0_SET_RESERVED_SW_METHOD03_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD04                                                                    0x0230
#define NVB0C0_SET_RESERVED_SW_METHOD04_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD05                                                                    0x0234
#define NVB0C0_SET_RESERVED_SW_METHOD05_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD06                                                                    0x0238
#define NVB0C0_SET_RESERVED_SW_METHOD06_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD07                                                                    0x023c
#define NVB0C0_SET_RESERVED_SW_METHOD07_V                                                                    31:0

#define NVB0C0_SET_CWD_CONTROL                                                                             0x0240
#define NVB0C0_SET_CWD_CONTROL_SM_SELECTION                                                                   0:0
#define NVB0C0_SET_CWD_CONTROL_SM_SELECTION_LOAD_BALANCED                                              0x00000000
#define NVB0C0_SET_CWD_CONTROL_SM_SELECTION_ROUND_ROBIN                                                0x00000001

#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI                                                      0x0244
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES                                                   0:0
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES_ALL                                        0x00000000
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES_ONE                                        0x00000001
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_TAG                                                    25:4

#define NVB0C0_SET_CWD_REF_COUNTER                                                                         0x0248
#define NVB0C0_SET_CWD_REF_COUNTER_SELECT                                                                     5:0
#define NVB0C0_SET_CWD_REF_COUNTER_VALUE                                                                     23:8

#define NVB0C0_SET_RESERVED_SW_METHOD08                                                                    0x024c
#define NVB0C0_SET_RESERVED_SW_METHOD08_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD09                                                                    0x0250
#define NVB0C0_SET_RESERVED_SW_METHOD09_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD10                                                                    0x0254
#define NVB0C0_SET_RESERVED_SW_METHOD10_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD11                                                                    0x0258
#define NVB0C0_SET_RESERVED_SW_METHOD11_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD12                                                                    0x025c
#define NVB0C0_SET_RESERVED_SW_METHOD12_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD13                                                                    0x0260
#define NVB0C0_SET_RESERVED_SW_METHOD13_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD14                                                                    0x0264
#define NVB0C0_SET_RESERVED_SW_METHOD14_V                                                                    31:0

#define NVB0C0_SET_RESERVED_SW_METHOD15                                                                    0x0268
#define NVB0C0_SET_RESERVED_SW_METHOD15_V                                                                    31:0

#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_A                                                          0x0274
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_A_ADDRESS_UPPER                                               7:0

#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_B                                                          0x0278
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_B_ADDRESS_LOWER                                              31:0

#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_C                                                          0x027c
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_C_BYTE_COUNT                                                 16:0
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_C_THRU_L2                                                   31:31
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_C_THRU_L2_FALSE                                        0x00000000
#define NVB0C0_INVALIDATE_CONSTANT_BUFFER_CACHE_C_THRU_L2_TRUE                                         0x00000001

#define NVB0C0_SET_COMPUTE_CLASS_VERSION                                                                   0x0280
#define NVB0C0_SET_COMPUTE_CLASS_VERSION_CURRENT                                                             15:0
#define NVB0C0_SET_COMPUTE_CLASS_VERSION_OLDEST_SUPPORTED                                                   31:16

#define NVB0C0_CHECK_COMPUTE_CLASS_VERSION                                                                 0x0284
#define NVB0C0_CHECK_COMPUTE_CLASS_VERSION_CURRENT                                                           15:0
#define NVB0C0_CHECK_COMPUTE_CLASS_VERSION_OLDEST_SUPPORTED                                                 31:16

#define NVB0C0_SET_QMD_VERSION                                                                             0x0288
#define NVB0C0_SET_QMD_VERSION_CURRENT                                                                       15:0
#define NVB0C0_SET_QMD_VERSION_OLDEST_SUPPORTED                                                             31:16

#define NVB0C0_CHECK_QMD_VERSION                                                                           0x0290
#define NVB0C0_CHECK_QMD_VERSION_CURRENT                                                                     15:0
#define NVB0C0_CHECK_QMD_VERSION_OLDEST_SUPPORTED                                                           31:16

#define NVB0C0_SET_CWD_SLOT_COUNT                                                                          0x02b0
#define NVB0C0_SET_CWD_SLOT_COUNT_V                                                                           7:0

#define NVB0C0_SEND_PCAS_A                                                                                 0x02b4
#define NVB0C0_SEND_PCAS_A_QMD_ADDRESS_SHIFTED8                                                              31:0

#define NVB0C0_SEND_PCAS_B                                                                                 0x02b8
#define NVB0C0_SEND_PCAS_B_FROM                                                                              23:0
#define NVB0C0_SEND_PCAS_B_DELTA                                                                            31:24

#define NVB0C0_SEND_SIGNALING_PCAS_B                                                                       0x02bc
#define NVB0C0_SEND_SIGNALING_PCAS_B_INVALIDATE                                                               0:0
#define NVB0C0_SEND_SIGNALING_PCAS_B_INVALIDATE_FALSE                                                  0x00000000
#define NVB0C0_SEND_SIGNALING_PCAS_B_INVALIDATE_TRUE                                                   0x00000001
#define NVB0C0_SEND_SIGNALING_PCAS_B_SCHEDULE                                                                 1:1
#define NVB0C0_SEND_SIGNALING_PCAS_B_SCHEDULE_FALSE                                                    0x00000000
#define NVB0C0_SEND_SIGNALING_PCAS_B_SCHEDULE_TRUE                                                     0x00000001

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_A                                                     0x02e4
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_A_SIZE_UPPER                                             7:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_B                                                     0x02e8
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_B_SIZE_LOWER                                            31:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_C                                                     0x02ec
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_NON_THROTTLED_C_MAX_SM_COUNT                                           8:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_A                                                         0x02f0
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_A_SIZE_UPPER                                                 7:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_B                                                         0x02f4
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_B_SIZE_LOWER                                                31:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_C                                                         0x02f8
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_THROTTLED_C_MAX_SM_COUNT                                               8:0

#define NVB0C0_SET_SPA_VERSION                                                                             0x0310
#define NVB0C0_SET_SPA_VERSION_MINOR                                                                          7:0
#define NVB0C0_SET_SPA_VERSION_MAJOR                                                                         15:8

#define NVB0C0_SET_FALCON00                                                                                0x0500
#define NVB0C0_SET_FALCON00_V                                                                                31:0

#define NVB0C0_SET_FALCON01                                                                                0x0504
#define NVB0C0_SET_FALCON01_V                                                                                31:0

#define NVB0C0_SET_FALCON02                                                                                0x0508
#define NVB0C0_SET_FALCON02_V                                                                                31:0

#define NVB0C0_SET_FALCON03                                                                                0x050c
#define NVB0C0_SET_FALCON03_V                                                                                31:0

#define NVB0C0_SET_FALCON04                                                                                0x0510
#define NVB0C0_SET_FALCON04_V                                                                                31:0

#define NVB0C0_SET_FALCON05                                                                                0x0514
#define NVB0C0_SET_FALCON05_V                                                                                31:0

#define NVB0C0_SET_FALCON06                                                                                0x0518
#define NVB0C0_SET_FALCON06_V                                                                                31:0

#define NVB0C0_SET_FALCON07                                                                                0x051c
#define NVB0C0_SET_FALCON07_V                                                                                31:0

#define NVB0C0_SET_FALCON08                                                                                0x0520
#define NVB0C0_SET_FALCON08_V                                                                                31:0

#define NVB0C0_SET_FALCON09                                                                                0x0524
#define NVB0C0_SET_FALCON09_V                                                                                31:0

#define NVB0C0_SET_FALCON10                                                                                0x0528
#define NVB0C0_SET_FALCON10_V                                                                                31:0

#define NVB0C0_SET_FALCON11                                                                                0x052c
#define NVB0C0_SET_FALCON11_V                                                                                31:0

#define NVB0C0_SET_FALCON12                                                                                0x0530
#define NVB0C0_SET_FALCON12_V                                                                                31:0

#define NVB0C0_SET_FALCON13                                                                                0x0534
#define NVB0C0_SET_FALCON13_V                                                                                31:0

#define NVB0C0_SET_FALCON14                                                                                0x0538
#define NVB0C0_SET_FALCON14_V                                                                                31:0

#define NVB0C0_SET_FALCON15                                                                                0x053c
#define NVB0C0_SET_FALCON15_V                                                                                31:0

#define NVB0C0_SET_FALCON16                                                                                0x0540
#define NVB0C0_SET_FALCON16_V                                                                                31:0

#define NVB0C0_SET_FALCON17                                                                                0x0544
#define NVB0C0_SET_FALCON17_V                                                                                31:0

#define NVB0C0_SET_FALCON18                                                                                0x0548
#define NVB0C0_SET_FALCON18_V                                                                                31:0

#define NVB0C0_SET_FALCON19                                                                                0x054c
#define NVB0C0_SET_FALCON19_V                                                                                31:0

#define NVB0C0_SET_FALCON20                                                                                0x0550
#define NVB0C0_SET_FALCON20_V                                                                                31:0

#define NVB0C0_SET_FALCON21                                                                                0x0554
#define NVB0C0_SET_FALCON21_V                                                                                31:0

#define NVB0C0_SET_FALCON22                                                                                0x0558
#define NVB0C0_SET_FALCON22_V                                                                                31:0

#define NVB0C0_SET_FALCON23                                                                                0x055c
#define NVB0C0_SET_FALCON23_V                                                                                31:0

#define NVB0C0_SET_FALCON24                                                                                0x0560
#define NVB0C0_SET_FALCON24_V                                                                                31:0

#define NVB0C0_SET_FALCON25                                                                                0x0564
#define NVB0C0_SET_FALCON25_V                                                                                31:0

#define NVB0C0_SET_FALCON26                                                                                0x0568
#define NVB0C0_SET_FALCON26_V                                                                                31:0

#define NVB0C0_SET_FALCON27                                                                                0x056c
#define NVB0C0_SET_FALCON27_V                                                                                31:0

#define NVB0C0_SET_FALCON28                                                                                0x0570
#define NVB0C0_SET_FALCON28_V                                                                                31:0

#define NVB0C0_SET_FALCON29                                                                                0x0574
#define NVB0C0_SET_FALCON29_V                                                                                31:0

#define NVB0C0_SET_FALCON30                                                                                0x0578
#define NVB0C0_SET_FALCON30_V                                                                                31:0

#define NVB0C0_SET_FALCON31                                                                                0x057c
#define NVB0C0_SET_FALCON31_V                                                                                31:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_WINDOW                                                              0x077c
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_WINDOW_BASE_ADDRESS                                                   31:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_A                                                                   0x0790
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_A_ADDRESS_UPPER                                                        7:0

#define NVB0C0_SET_SHADER_LOCAL_MEMORY_B                                                                   0x0794
#define NVB0C0_SET_SHADER_LOCAL_MEMORY_B_ADDRESS_LOWER                                                       31:0

#define NVB0C0_SET_SHADER_CACHE_CONTROL                                                                    0x0d94
#define NVB0C0_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE                                                0:0
#define NVB0C0_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE_FALSE                                   0x00000000
#define NVB0C0_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE_TRUE                                    0x00000001

#define NVB0C0_SET_SM_TIMEOUT_INTERVAL                                                                     0x0de4
#define NVB0C0_SET_SM_TIMEOUT_INTERVAL_COUNTER_BIT                                                            5:0

#define NVB0C0_SET_SPARE_NOOP12                                                                            0x0f44
#define NVB0C0_SET_SPARE_NOOP12_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP13                                                                            0x0f48
#define NVB0C0_SET_SPARE_NOOP13_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP14                                                                            0x0f4c
#define NVB0C0_SET_SPARE_NOOP14_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP15                                                                            0x0f50
#define NVB0C0_SET_SPARE_NOOP15_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP00                                                                            0x1040
#define NVB0C0_SET_SPARE_NOOP00_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP01                                                                            0x1044
#define NVB0C0_SET_SPARE_NOOP01_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP02                                                                            0x1048
#define NVB0C0_SET_SPARE_NOOP02_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP03                                                                            0x104c
#define NVB0C0_SET_SPARE_NOOP03_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP04                                                                            0x1050
#define NVB0C0_SET_SPARE_NOOP04_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP05                                                                            0x1054
#define NVB0C0_SET_SPARE_NOOP05_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP06                                                                            0x1058
#define NVB0C0_SET_SPARE_NOOP06_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP07                                                                            0x105c
#define NVB0C0_SET_SPARE_NOOP07_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP08                                                                            0x1060
#define NVB0C0_SET_SPARE_NOOP08_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP09                                                                            0x1064
#define NVB0C0_SET_SPARE_NOOP09_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP10                                                                            0x1068
#define NVB0C0_SET_SPARE_NOOP10_V                                                                            31:0

#define NVB0C0_SET_SPARE_NOOP11                                                                            0x106c
#define NVB0C0_SET_SPARE_NOOP11_V                                                                            31:0

#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI                                                        0x1288
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES                                                     0:0
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES_ALL                                          0x00000000
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES_ONE                                          0x00000001
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_TAG                                                      25:4

#define NVB0C0_ACTIVATE_PERF_SETTINGS_FOR_COMPUTE_CONTEXT                                                  0x12a8
#define NVB0C0_ACTIVATE_PERF_SETTINGS_FOR_COMPUTE_CONTEXT_ALL                                                 0:0
#define NVB0C0_ACTIVATE_PERF_SETTINGS_FOR_COMPUTE_CONTEXT_ALL_FALSE                                    0x00000000
#define NVB0C0_ACTIVATE_PERF_SETTINGS_FOR_COMPUTE_CONTEXT_ALL_TRUE                                     0x00000001

#define NVB0C0_INVALIDATE_SAMPLER_CACHE                                                                    0x1330
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_LINES                                                                 0:0
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_LINES_ALL                                                      0x00000000
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_LINES_ONE                                                      0x00000001
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_TAG                                                                  25:4

#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE                                                             0x1334
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_LINES                                                          0:0
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_LINES_ALL                                               0x00000000
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_LINES_ONE                                               0x00000001
#define NVB0C0_INVALIDATE_TEXTURE_HEADER_CACHE_TAG                                                           25:4

#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE                                                               0x1338
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_LINES                                                            0:0
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_LINES_ALL                                                 0x00000000
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_LINES_ONE                                                 0x00000001
#define NVB0C0_INVALIDATE_TEXTURE_DATA_CACHE_TAG                                                             25:4

#define NVB0C0_INVALIDATE_SAMPLER_CACHE_NO_WFI                                                             0x1424
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES                                                          0:0
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES_ALL                                               0x00000000
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES_ONE                                               0x00000001
#define NVB0C0_INVALIDATE_SAMPLER_CACHE_NO_WFI_TAG                                                           25:4

#define NVB0C0_SET_SHADER_EXCEPTIONS                                                                       0x1528
#define NVB0C0_SET_SHADER_EXCEPTIONS_ENABLE                                                                   0:0
#define NVB0C0_SET_SHADER_EXCEPTIONS_ENABLE_FALSE                                                      0x00000000
#define NVB0C0_SET_SHADER_EXCEPTIONS_ENABLE_TRUE                                                       0x00000001

#define NVB0C0_SET_RENDER_ENABLE_A                                                                         0x1550
#define NVB0C0_SET_RENDER_ENABLE_A_OFFSET_UPPER                                                               7:0

#define NVB0C0_SET_RENDER_ENABLE_B                                                                         0x1554
#define NVB0C0_SET_RENDER_ENABLE_B_OFFSET_LOWER                                                              31:0

#define NVB0C0_SET_RENDER_ENABLE_C                                                                         0x1558
#define NVB0C0_SET_RENDER_ENABLE_C_MODE                                                                       2:0
#define NVB0C0_SET_RENDER_ENABLE_C_MODE_FALSE                                                          0x00000000
#define NVB0C0_SET_RENDER_ENABLE_C_MODE_TRUE                                                           0x00000001
#define NVB0C0_SET_RENDER_ENABLE_C_MODE_CONDITIONAL                                                    0x00000002
#define NVB0C0_SET_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                                0x00000003
#define NVB0C0_SET_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                            0x00000004

#define NVB0C0_SET_TEX_SAMPLER_POOL_A                                                                      0x155c
#define NVB0C0_SET_TEX_SAMPLER_POOL_A_OFFSET_UPPER                                                            7:0

#define NVB0C0_SET_TEX_SAMPLER_POOL_B                                                                      0x1560
#define NVB0C0_SET_TEX_SAMPLER_POOL_B_OFFSET_LOWER                                                           31:0

#define NVB0C0_SET_TEX_SAMPLER_POOL_C                                                                      0x1564
#define NVB0C0_SET_TEX_SAMPLER_POOL_C_MAXIMUM_INDEX                                                          19:0

#define NVB0C0_SET_TEX_HEADER_POOL_A                                                                       0x1574
#define NVB0C0_SET_TEX_HEADER_POOL_A_OFFSET_UPPER                                                             7:0

#define NVB0C0_SET_TEX_HEADER_POOL_B                                                                       0x1578
#define NVB0C0_SET_TEX_HEADER_POOL_B_OFFSET_LOWER                                                            31:0

#define NVB0C0_SET_TEX_HEADER_POOL_C                                                                       0x157c
#define NVB0C0_SET_TEX_HEADER_POOL_C_MAXIMUM_INDEX                                                           21:0

#define NVB0C0_SET_PROGRAM_REGION_A                                                                        0x1608
#define NVB0C0_SET_PROGRAM_REGION_A_ADDRESS_UPPER                                                             7:0

#define NVB0C0_SET_PROGRAM_REGION_B                                                                        0x160c
#define NVB0C0_SET_PROGRAM_REGION_B_ADDRESS_LOWER                                                            31:0

#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI                                                             0x1698
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION                                                    0:0
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION_FALSE                                       0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION_TRUE                                        0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA                                                    4:4
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA_FALSE                                       0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA_TRUE                                        0x00000001
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT                                                     12:12
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT_FALSE                                          0x00000000
#define NVB0C0_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT_TRUE                                           0x00000001

#define NVB0C0_SET_RENDER_ENABLE_OVERRIDE                                                                  0x1944
#define NVB0C0_SET_RENDER_ENABLE_OVERRIDE_MODE                                                                1:0
#define NVB0C0_SET_RENDER_ENABLE_OVERRIDE_MODE_USE_RENDER_ENABLE                                       0x00000000
#define NVB0C0_SET_RENDER_ENABLE_OVERRIDE_MODE_ALWAYS_RENDER                                           0x00000001
#define NVB0C0_SET_RENDER_ENABLE_OVERRIDE_MODE_NEVER_RENDER                                            0x00000002

#define NVB0C0_PIPE_NOP                                                                                    0x1a2c
#define NVB0C0_PIPE_NOP_V                                                                                    31:0

#define NVB0C0_SET_SPARE00                                                                                 0x1a30
#define NVB0C0_SET_SPARE00_V                                                                                 31:0

#define NVB0C0_SET_SPARE01                                                                                 0x1a34
#define NVB0C0_SET_SPARE01_V                                                                                 31:0

#define NVB0C0_SET_SPARE02                                                                                 0x1a38
#define NVB0C0_SET_SPARE02_V                                                                                 31:0

#define NVB0C0_SET_SPARE03                                                                                 0x1a3c
#define NVB0C0_SET_SPARE03_V                                                                                 31:0

#define NVB0C0_SET_REPORT_SEMAPHORE_A                                                                      0x1b00
#define NVB0C0_SET_REPORT_SEMAPHORE_A_OFFSET_UPPER                                                            7:0

#define NVB0C0_SET_REPORT_SEMAPHORE_B                                                                      0x1b04
#define NVB0C0_SET_REPORT_SEMAPHORE_B_OFFSET_LOWER                                                           31:0

#define NVB0C0_SET_REPORT_SEMAPHORE_C                                                                      0x1b08
#define NVB0C0_SET_REPORT_SEMAPHORE_C_PAYLOAD                                                                31:0

#define NVB0C0_SET_REPORT_SEMAPHORE_D                                                                      0x1b0c
#define NVB0C0_SET_REPORT_SEMAPHORE_D_OPERATION                                                               1:0
#define NVB0C0_SET_REPORT_SEMAPHORE_D_OPERATION_RELEASE                                                0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_OPERATION_TRAP                                                   0x00000003
#define NVB0C0_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE                                                         20:20
#define NVB0C0_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE_FALSE                                              0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE_TRUE                                               0x00000001
#define NVB0C0_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE                                                        28:28
#define NVB0C0_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE_FOUR_WORDS                                        0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE_ONE_WORD                                          0x00000001
#define NVB0C0_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE                                                           2:2
#define NVB0C0_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE_FALSE                                              0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE_TRUE                                               0x00000001
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE                                                        3:3
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE_FALSE                                           0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE_TRUE                                            0x00000001
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP                                                           11:9
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_ADD                                             0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_MIN                                             0x00000001
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_MAX                                             0x00000002
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_INC                                             0x00000003
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_DEC                                             0x00000004
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_AND                                             0x00000005
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_OR                                              0x00000006
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_XOR                                             0x00000007
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT                                                      18:17
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT_UNSIGNED_32                                     0x00000000
#define NVB0C0_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT_SIGNED_32                                       0x00000001

#define NVB0C0_SET_BINDLESS_TEXTURE                                                                        0x2608
#define NVB0C0_SET_BINDLESS_TEXTURE_CONSTANT_BUFFER_SLOT_SELECT                                               2:0

#define NVB0C0_SET_TRAP_HANDLER                                                                            0x260c
#define NVB0C0_SET_TRAP_HANDLER_OFFSET                                                                       31:0

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_VALUE_UPPER(i)                                       (0x333c+(i)*4)
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_VALUE_UPPER_V                                                  31:0

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_VALUE(i)                                             (0x335c+(i)*4)
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_VALUE_V                                                        31:0

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_EVENT(i)                                             (0x337c+(i)*4)
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_EVENT_EVENT                                                     7:0

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A(i)                                         (0x339c+(i)*4)
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT0                                                1:0
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT0                                           4:2
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT1                                                6:5
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT1                                           9:7
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT2                                              11:10
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT2                                         14:12
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT3                                              16:15
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT3                                         19:17
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT4                                              21:20
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT4                                         24:22
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT5                                              26:25
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT5                                         29:27
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_SPARE                                               31:30

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B(i)                                         (0x33bc+(i)*4)
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_EDGE                                                  0:0
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_MODE                                                  2:1
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_WINDOWED                                              3:3
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_FUNC                                                 19:4

#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_TRAP_CONTROL                                                 0x33dc
#define NVB0C0_SET_SHADER_PERFORMANCE_COUNTER_TRAP_CONTROL_MASK                                               7:0

#define NVB0C0_START_SHADER_PERFORMANCE_COUNTER                                                            0x33e0
#define NVB0C0_START_SHADER_PERFORMANCE_COUNTER_COUNTER_MASK                                                  7:0

#define NVB0C0_STOP_SHADER_PERFORMANCE_COUNTER                                                             0x33e4
#define NVB0C0_STOP_SHADER_PERFORMANCE_COUNTER_COUNTER_MASK                                                   7:0

#define NVB0C0_SET_MME_SHADOW_SCRATCH(i)                                                           (0x3400+(i)*4)
#define NVB0C0_SET_MME_SHADOW_SCRATCH_V                                                                      31:0

#endif /* _cl_maxwell_compute_a_h_ */
