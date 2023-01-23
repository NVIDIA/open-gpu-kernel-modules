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

#ifndef _cl_kepler_b_h_
#define _cl_kepler_b_h_

/* AUTO GENERATED FILE -- DO NOT EDIT */
/* Command: ../../class/bin/sw_header.pl kepler_b */

#include "nvtypes.h"

#define KEPLER_B    0xA197

#define NVA197_SET_OBJECT                                                                                  0x0000
#define NVA197_SET_OBJECT_CLASS_ID                                                                           15:0
#define NVA197_SET_OBJECT_ENGINE_ID                                                                         20:16

#define NVA197_NO_OPERATION                                                                                0x0100
#define NVA197_NO_OPERATION_V                                                                                31:0

#define NVA197_SET_NOTIFY_A                                                                                0x0104
#define NVA197_SET_NOTIFY_A_ADDRESS_UPPER                                                                     7:0

#define NVA197_SET_NOTIFY_B                                                                                0x0108
#define NVA197_SET_NOTIFY_B_ADDRESS_LOWER                                                                    31:0

#define NVA197_NOTIFY                                                                                      0x010c
#define NVA197_NOTIFY_TYPE                                                                                   31:0
#define NVA197_NOTIFY_TYPE_WRITE_ONLY                                                                  0x00000000
#define NVA197_NOTIFY_TYPE_WRITE_THEN_AWAKEN                                                           0x00000001

#define NVA197_WAIT_FOR_IDLE                                                                               0x0110
#define NVA197_WAIT_FOR_IDLE_V                                                                               31:0

#define NVA197_LOAD_MME_INSTRUCTION_RAM_POINTER                                                            0x0114
#define NVA197_LOAD_MME_INSTRUCTION_RAM_POINTER_V                                                            31:0

#define NVA197_LOAD_MME_INSTRUCTION_RAM                                                                    0x0118
#define NVA197_LOAD_MME_INSTRUCTION_RAM_V                                                                    31:0

#define NVA197_LOAD_MME_START_ADDRESS_RAM_POINTER                                                          0x011c
#define NVA197_LOAD_MME_START_ADDRESS_RAM_POINTER_V                                                          31:0

#define NVA197_LOAD_MME_START_ADDRESS_RAM                                                                  0x0120
#define NVA197_LOAD_MME_START_ADDRESS_RAM_V                                                                  31:0

#define NVA197_SET_MME_SHADOW_RAM_CONTROL                                                                  0x0124
#define NVA197_SET_MME_SHADOW_RAM_CONTROL_MODE                                                                1:0
#define NVA197_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_TRACK                                            0x00000000
#define NVA197_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_TRACK_WITH_FILTER                                0x00000001
#define NVA197_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_PASSTHROUGH                                      0x00000002
#define NVA197_SET_MME_SHADOW_RAM_CONTROL_MODE_METHOD_REPLAY                                           0x00000003

#define NVA197_PEER_SEMAPHORE_RELEASE_OFFSET_UPPER                                                         0x0128
#define NVA197_PEER_SEMAPHORE_RELEASE_OFFSET_UPPER_V                                                          7:0

#define NVA197_PEER_SEMAPHORE_RELEASE_OFFSET                                                               0x012c
#define NVA197_PEER_SEMAPHORE_RELEASE_OFFSET_V                                                               31:0

#define NVA197_SET_GLOBAL_RENDER_ENABLE_A                                                                  0x0130
#define NVA197_SET_GLOBAL_RENDER_ENABLE_A_OFFSET_UPPER                                                        7:0

#define NVA197_SET_GLOBAL_RENDER_ENABLE_B                                                                  0x0134
#define NVA197_SET_GLOBAL_RENDER_ENABLE_B_OFFSET_LOWER                                                       31:0

#define NVA197_SET_GLOBAL_RENDER_ENABLE_C                                                                  0x0138
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE                                                                2:0
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE_FALSE                                                   0x00000000
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE_TRUE                                                    0x00000001
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE_CONDITIONAL                                             0x00000002
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                         0x00000003
#define NVA197_SET_GLOBAL_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                     0x00000004

#define NVA197_SEND_GO_IDLE                                                                                0x013c
#define NVA197_SEND_GO_IDLE_V                                                                                31:0

#define NVA197_PM_TRIGGER                                                                                  0x0140
#define NVA197_PM_TRIGGER_V                                                                                  31:0

#define NVA197_PM_TRIGGER_WFI                                                                              0x0144
#define NVA197_PM_TRIGGER_WFI_V                                                                              31:0

#define NVA197_SET_INSTRUMENTATION_METHOD_HEADER                                                           0x0150
#define NVA197_SET_INSTRUMENTATION_METHOD_HEADER_V                                                           31:0

#define NVA197_SET_INSTRUMENTATION_METHOD_DATA                                                             0x0154
#define NVA197_SET_INSTRUMENTATION_METHOD_DATA_V                                                             31:0

#define NVA197_LINE_LENGTH_IN                                                                              0x0180
#define NVA197_LINE_LENGTH_IN_VALUE                                                                          31:0

#define NVA197_LINE_COUNT                                                                                  0x0184
#define NVA197_LINE_COUNT_VALUE                                                                              31:0

#define NVA197_OFFSET_OUT_UPPER                                                                            0x0188
#define NVA197_OFFSET_OUT_UPPER_VALUE                                                                         7:0

#define NVA197_OFFSET_OUT                                                                                  0x018c
#define NVA197_OFFSET_OUT_VALUE                                                                              31:0

#define NVA197_PITCH_OUT                                                                                   0x0190
#define NVA197_PITCH_OUT_VALUE                                                                               31:0

#define NVA197_SET_DST_BLOCK_SIZE                                                                          0x0194
#define NVA197_SET_DST_BLOCK_SIZE_WIDTH                                                                       3:0
#define NVA197_SET_DST_BLOCK_SIZE_WIDTH_ONE_GOB                                                        0x00000000
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT                                                                      7:4
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_ONE_GOB                                                       0x00000000
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_TWO_GOBS                                                      0x00000001
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                                     0x00000002
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                                    0x00000003
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                                  0x00000004
#define NVA197_SET_DST_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                                0x00000005
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH                                                                      11:8
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_ONE_GOB                                                        0x00000000
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_TWO_GOBS                                                       0x00000001
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_FOUR_GOBS                                                      0x00000002
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_EIGHT_GOBS                                                     0x00000003
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_SIXTEEN_GOBS                                                   0x00000004
#define NVA197_SET_DST_BLOCK_SIZE_DEPTH_THIRTYTWO_GOBS                                                 0x00000005

#define NVA197_SET_DST_WIDTH                                                                               0x0198
#define NVA197_SET_DST_WIDTH_V                                                                               31:0

#define NVA197_SET_DST_HEIGHT                                                                              0x019c
#define NVA197_SET_DST_HEIGHT_V                                                                              31:0

#define NVA197_SET_DST_DEPTH                                                                               0x01a0
#define NVA197_SET_DST_DEPTH_V                                                                               31:0

#define NVA197_SET_DST_LAYER                                                                               0x01a4
#define NVA197_SET_DST_LAYER_V                                                                               31:0

#define NVA197_SET_DST_ORIGIN_BYTES_X                                                                      0x01a8
#define NVA197_SET_DST_ORIGIN_BYTES_X_V                                                                      19:0

#define NVA197_SET_DST_ORIGIN_SAMPLES_Y                                                                    0x01ac
#define NVA197_SET_DST_ORIGIN_SAMPLES_Y_V                                                                    15:0

#define NVA197_LAUNCH_DMA                                                                                  0x01b0
#define NVA197_LAUNCH_DMA_DST_MEMORY_LAYOUT                                                                   0:0
#define NVA197_LAUNCH_DMA_DST_MEMORY_LAYOUT_BLOCKLINEAR                                                0x00000000
#define NVA197_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                                                      0x00000001
#define NVA197_LAUNCH_DMA_COMPLETION_TYPE                                                                     5:4
#define NVA197_LAUNCH_DMA_COMPLETION_TYPE_FLUSH_DISABLE                                                0x00000000
#define NVA197_LAUNCH_DMA_COMPLETION_TYPE_FLUSH_ONLY                                                   0x00000001
#define NVA197_LAUNCH_DMA_COMPLETION_TYPE_RELEASE_SEMAPHORE                                            0x00000002
#define NVA197_LAUNCH_DMA_INTERRUPT_TYPE                                                                      9:8
#define NVA197_LAUNCH_DMA_INTERRUPT_TYPE_NONE                                                          0x00000000
#define NVA197_LAUNCH_DMA_INTERRUPT_TYPE_INTERRUPT                                                     0x00000001
#define NVA197_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE                                                             12:12
#define NVA197_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE_FOUR_WORDS                                             0x00000000
#define NVA197_LAUNCH_DMA_SEMAPHORE_STRUCT_SIZE_ONE_WORD                                               0x00000001
#define NVA197_LAUNCH_DMA_REDUCTION_ENABLE                                                                    1:1
#define NVA197_LAUNCH_DMA_REDUCTION_ENABLE_FALSE                                                       0x00000000
#define NVA197_LAUNCH_DMA_REDUCTION_ENABLE_TRUE                                                        0x00000001
#define NVA197_LAUNCH_DMA_REDUCTION_OP                                                                      15:13
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_ADD                                                         0x00000000
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_MIN                                                         0x00000001
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_MAX                                                         0x00000002
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_INC                                                         0x00000003
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_DEC                                                         0x00000004
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_AND                                                         0x00000005
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_OR                                                          0x00000006
#define NVA197_LAUNCH_DMA_REDUCTION_OP_RED_XOR                                                         0x00000007
#define NVA197_LAUNCH_DMA_REDUCTION_FORMAT                                                                    3:2
#define NVA197_LAUNCH_DMA_REDUCTION_FORMAT_UNSIGNED_32                                                 0x00000000
#define NVA197_LAUNCH_DMA_REDUCTION_FORMAT_SIGNED_32                                                   0x00000001
#define NVA197_LAUNCH_DMA_SYSMEMBAR_DISABLE                                                                   6:6
#define NVA197_LAUNCH_DMA_SYSMEMBAR_DISABLE_FALSE                                                      0x00000000
#define NVA197_LAUNCH_DMA_SYSMEMBAR_DISABLE_TRUE                                                       0x00000001

#define NVA197_LOAD_INLINE_DATA                                                                            0x01b4
#define NVA197_LOAD_INLINE_DATA_V                                                                            31:0

#define NVA197_SET_I2M_SEMAPHORE_A                                                                         0x01dc
#define NVA197_SET_I2M_SEMAPHORE_A_OFFSET_UPPER                                                               7:0

#define NVA197_SET_I2M_SEMAPHORE_B                                                                         0x01e0
#define NVA197_SET_I2M_SEMAPHORE_B_OFFSET_LOWER                                                              31:0

#define NVA197_SET_I2M_SEMAPHORE_C                                                                         0x01e4
#define NVA197_SET_I2M_SEMAPHORE_C_PAYLOAD                                                                   31:0

#define NVA197_SET_I2M_SPARE_NOOP00                                                                        0x01f0
#define NVA197_SET_I2M_SPARE_NOOP00_V                                                                        31:0

#define NVA197_SET_I2M_SPARE_NOOP01                                                                        0x01f4
#define NVA197_SET_I2M_SPARE_NOOP01_V                                                                        31:0

#define NVA197_SET_I2M_SPARE_NOOP02                                                                        0x01f8
#define NVA197_SET_I2M_SPARE_NOOP02_V                                                                        31:0

#define NVA197_SET_I2M_SPARE_NOOP03                                                                        0x01fc
#define NVA197_SET_I2M_SPARE_NOOP03_V                                                                        31:0

#define NVA197_RUN_DS_NOW                                                                                  0x0200
#define NVA197_RUN_DS_NOW_V                                                                                  31:0

#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS                                                        0x0204
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD                           4:0
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD_INSTANTANEOUS             0x00000000
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__16                0x00000001
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__32                0x00000002
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__64                0x00000003
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__128               0x00000004
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__256               0x00000005
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__512               0x00000006
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__1024              0x00000007
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__2048              0x00000008
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__4096              0x00000009
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__8192              0x0000000A
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__16384             0x0000000B
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__32768             0x0000000C
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__65536             0x0000000D
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__131072             0x0000000E
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__262144             0x0000000F
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__524288             0x00000010
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__1048576             0x00000011
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__2097152             0x00000012
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD__4194304             0x00000013
#define NVA197_SET_OPPORTUNISTIC_EARLY_Z_HYSTERESIS_ACCUMULATED_PRIM_AREA_THRESHOLD_LATEZ_ALWAYS             0x0000001F

#define NVA197_SET_RASTER_PIPE_SYNC_CONTROL                                                                0x0208
#define NVA197_SET_RASTER_PIPE_SYNC_CONTROL_PRIM_AREA_THRESHOLD                                              21:0
#define NVA197_SET_RASTER_PIPE_SYNC_CONTROL_ENABLE                                                          24:24
#define NVA197_SET_RASTER_PIPE_SYNC_CONTROL_ENABLE_FALSE                                               0x00000000
#define NVA197_SET_RASTER_PIPE_SYNC_CONTROL_ENABLE_TRUE                                                0x00000001

#define NVA197_SET_ALIASED_LINE_WIDTH_ENABLE                                                               0x020c
#define NVA197_SET_ALIASED_LINE_WIDTH_ENABLE_V                                                                0:0
#define NVA197_SET_ALIASED_LINE_WIDTH_ENABLE_V_FALSE                                                   0x00000000
#define NVA197_SET_ALIASED_LINE_WIDTH_ENABLE_V_TRUE                                                    0x00000001

#define NVA197_SET_API_MANDATED_EARLY_Z                                                                    0x0210
#define NVA197_SET_API_MANDATED_EARLY_Z_ENABLE                                                                0:0
#define NVA197_SET_API_MANDATED_EARLY_Z_ENABLE_FALSE                                                   0x00000000
#define NVA197_SET_API_MANDATED_EARLY_Z_ENABLE_TRUE                                                    0x00000001

#define NVA197_SET_GS_DM_FIFO                                                                              0x0214
#define NVA197_SET_GS_DM_FIFO_SIZE_RASTER_ON                                                                 12:0
#define NVA197_SET_GS_DM_FIFO_SIZE_RASTER_OFF                                                               28:16
#define NVA197_SET_GS_DM_FIFO_SPILL_ENABLED                                                                 31:31
#define NVA197_SET_GS_DM_FIFO_SPILL_ENABLED_FALSE                                                      0x00000000
#define NVA197_SET_GS_DM_FIFO_SPILL_ENABLED_TRUE                                                       0x00000001

#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_PREFETCH_READ_REQUESTS                                         0x0218
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_PREFETCH_READ_REQUESTS_POLICY                                     5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_PREFETCH_READ_REQUESTS_POLICY_EVICT_FIRST                  0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_PREFETCH_READ_REQUESTS_POLICY_EVICT_NORMAL                 0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_PREFETCH_READ_REQUESTS_POLICY_EVICT_LAST                   0x00000002

#define NVA197_INVALIDATE_SHADER_CACHES                                                                    0x021c
#define NVA197_INVALIDATE_SHADER_CACHES_INSTRUCTION                                                           0:0
#define NVA197_INVALIDATE_SHADER_CACHES_INSTRUCTION_FALSE                                              0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_INSTRUCTION_TRUE                                               0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_DATA                                                                  4:4
#define NVA197_INVALIDATE_SHADER_CACHES_DATA_FALSE                                                     0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_DATA_TRUE                                                      0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_CONSTANT                                                            12:12
#define NVA197_INVALIDATE_SHADER_CACHES_CONSTANT_FALSE                                                 0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_CONSTANT_TRUE                                                  0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_LOCKS                                                                 1:1
#define NVA197_INVALIDATE_SHADER_CACHES_LOCKS_FALSE                                                    0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_LOCKS_TRUE                                                     0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_FLUSH_DATA                                                            2:2
#define NVA197_INVALIDATE_SHADER_CACHES_FLUSH_DATA_FALSE                                               0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_FLUSH_DATA_TRUE                                                0x00000001

#define NVA197_SET_VAB_VERTEX3F(i)                                                                 (0x0220+(i)*4)
#define NVA197_SET_VAB_VERTEX3F_V                                                                            31:0

#define NVA197_SET_VAB_VERTEX4F(i)                                                                 (0x0230+(i)*4)
#define NVA197_SET_VAB_VERTEX4F_V                                                                            31:0

#define NVA197_SET_VAB_NORMAL3F(i)                                                                 (0x0240+(i)*4)
#define NVA197_SET_VAB_NORMAL3F_V                                                                            31:0

#define NVA197_SET_VAB_COLOR3F(i)                                                                  (0x0250+(i)*4)
#define NVA197_SET_VAB_COLOR3F_V                                                                             31:0

#define NVA197_SET_VAB_COLOR4F(i)                                                                  (0x0260+(i)*4)
#define NVA197_SET_VAB_COLOR4F_V                                                                             31:0

#define NVA197_SET_VAB_COLOR4UB(i)                                                                 (0x0270+(i)*4)
#define NVA197_SET_VAB_COLOR4UB_V                                                                            31:0

#define NVA197_SET_VAB_TEX_COORD1F(i)                                                              (0x0280+(i)*4)
#define NVA197_SET_VAB_TEX_COORD1F_V                                                                         31:0

#define NVA197_SET_VAB_TEX_COORD2F(i)                                                              (0x0290+(i)*4)
#define NVA197_SET_VAB_TEX_COORD2F_V                                                                         31:0

#define NVA197_SET_VAB_TEX_COORD3F(i)                                                              (0x02a0+(i)*4)
#define NVA197_SET_VAB_TEX_COORD3F_V                                                                         31:0

#define NVA197_SET_VAB_TEX_COORD4F(i)                                                              (0x02b0+(i)*4)
#define NVA197_SET_VAB_TEX_COORD4F_V                                                                         31:0

#define NVA197_SET_GLOBAL_LOAD_VIA_TEXTURE                                                                 0x02c4
#define NVA197_SET_GLOBAL_LOAD_VIA_TEXTURE_ENABLE                                                             0:0
#define NVA197_SET_GLOBAL_LOAD_VIA_TEXTURE_ENABLE_FALSE                                                0x00000000
#define NVA197_SET_GLOBAL_LOAD_VIA_TEXTURE_ENABLE_TRUE                                                 0x00000001
#define NVA197_SET_GLOBAL_LOAD_VIA_TEXTURE_HEADER_INDEX                                                      23:4

#define NVA197_SET_TASK_CIRCULAR_BUFFER_THROTTLE                                                           0x02cc
#define NVA197_SET_TASK_CIRCULAR_BUFFER_THROTTLE_TASK_COUNT                                                  21:0

#define NVA197_SET_PRIM_CIRCULAR_BUFFER_THROTTLE                                                           0x02d0
#define NVA197_SET_PRIM_CIRCULAR_BUFFER_THROTTLE_PRIM_AREA                                                   21:0

#define NVA197_FLUSH_AND_INVALIDATE_ROP_MINI_CACHE                                                         0x02d4
#define NVA197_FLUSH_AND_INVALIDATE_ROP_MINI_CACHE_V                                                          0:0

#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE                                                              0x02d8
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_WIDTH                                                           3:0
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_WIDTH_ONE_GOB                                            0x00000000
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT                                                          7:4
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_ONE_GOB                                           0x00000000
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_TWO_GOBS                                          0x00000001
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                         0x00000002
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                        0x00000003
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                      0x00000004
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                    0x00000005
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_DEPTH                                                          11:8
#define NVA197_SET_SURFACE_CLIP_ID_BLOCK_SIZE_DEPTH_ONE_GOB                                            0x00000000

#define NVA197_SET_ALPHA_CIRCULAR_BUFFER_SIZE                                                              0x02dc
#define NVA197_SET_ALPHA_CIRCULAR_BUFFER_SIZE_CACHE_LINES_PER_SM                                              9:0

#define NVA197_SET_ZCULL_ROP_BYPASS                                                                        0x02e4
#define NVA197_SET_ZCULL_ROP_BYPASS_ENABLE                                                                    0:0
#define NVA197_SET_ZCULL_ROP_BYPASS_ENABLE_FALSE                                                       0x00000000
#define NVA197_SET_ZCULL_ROP_BYPASS_ENABLE_TRUE                                                        0x00000001
#define NVA197_SET_ZCULL_ROP_BYPASS_NO_STALL                                                                  4:4
#define NVA197_SET_ZCULL_ROP_BYPASS_NO_STALL_FALSE                                                     0x00000000
#define NVA197_SET_ZCULL_ROP_BYPASS_NO_STALL_TRUE                                                      0x00000001
#define NVA197_SET_ZCULL_ROP_BYPASS_CULL_EVERYTHING                                                           8:8
#define NVA197_SET_ZCULL_ROP_BYPASS_CULL_EVERYTHING_FALSE                                              0x00000000
#define NVA197_SET_ZCULL_ROP_BYPASS_CULL_EVERYTHING_TRUE                                               0x00000001
#define NVA197_SET_ZCULL_ROP_BYPASS_THRESHOLD                                                               15:12

#define NVA197_SET_ZCULL_SUBREGION                                                                         0x02e8
#define NVA197_SET_ZCULL_SUBREGION_ENABLE                                                                     0:0
#define NVA197_SET_ZCULL_SUBREGION_ENABLE_FALSE                                                        0x00000000
#define NVA197_SET_ZCULL_SUBREGION_ENABLE_TRUE                                                         0x00000001
#define NVA197_SET_ZCULL_SUBREGION_NORMALIZED_ALIQUOTS                                                       27:4

#define NVA197_SET_RASTER_BOUNDING_BOX                                                                     0x02ec
#define NVA197_SET_RASTER_BOUNDING_BOX_MODE                                                                   0:0
#define NVA197_SET_RASTER_BOUNDING_BOX_MODE_BOUNDING_BOX                                               0x00000000
#define NVA197_SET_RASTER_BOUNDING_BOX_MODE_FULL_VIEWPORT                                              0x00000001
#define NVA197_SET_RASTER_BOUNDING_BOX_PAD                                                                   11:4

#define NVA197_PEER_SEMAPHORE_RELEASE                                                                      0x02f0
#define NVA197_PEER_SEMAPHORE_RELEASE_V                                                                      31:0

#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION                                                              0x02f8
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_SUBREGION_ID                                                    7:0
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_ALIQUOTS                                                       23:8
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT                                                        27:24
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_16X16X2_4X4                                     0x00000000
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_ZS_16X16_4X4                                      0x00000001
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_16X16_4X2                                       0x00000002
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_16X16_2X4                                       0x00000003
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_16X8_4X4                                        0x00000004
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_8X8_4X2                                         0x00000005
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_8X8_2X4                                         0x00000006
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_16X16_4X8                                       0x00000007
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_4X8_2X2                                         0x00000008
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_ZS_16X8_4X2                                       0x00000009
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_ZS_16X8_2X4                                       0x0000000A
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_ZS_8X8_2X2                                        0x0000000B
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_Z_4X8_1X1                                         0x0000000C
#define NVA197_SET_ZCULL_SUBREGION_ALLOCATION_FORMAT_NONE                                              0x0000000F

#define NVA197_ASSIGN_ZCULL_SUBREGIONS                                                                     0x02fc
#define NVA197_ASSIGN_ZCULL_SUBREGIONS_ALGORITHM                                                              1:0
#define NVA197_ASSIGN_ZCULL_SUBREGIONS_ALGORITHM_Static                                                0x00000000
#define NVA197_ASSIGN_ZCULL_SUBREGIONS_ALGORITHM_Adaptive                                              0x00000001

#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE                                                             0x0300
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_ENABLE                                                         0:0
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_ENABLE_FALSE                                            0x00000000
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_ENABLE_TRUE                                             0x00000001
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_QUALIFY_BY_ANTI_ALIAS_ENABLE                                   1:1
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_QUALIFY_BY_ANTI_ALIAS_ENABLE_DISABLE                    0x00000000
#define NVA197_SET_PS_OUTPUT_SAMPLE_MASK_USAGE_QUALIFY_BY_ANTI_ALIAS_ENABLE_ENABLE                     0x00000001

#define NVA197_DRAW_ZERO_INDEX                                                                             0x0304
#define NVA197_DRAW_ZERO_INDEX_COUNT                                                                         31:0

#define NVA197_SET_L1_CONFIGURATION                                                                        0x0308
#define NVA197_SET_L1_CONFIGURATION_DIRECTLY_ADDRESSABLE_MEMORY                                               2:0
#define NVA197_SET_L1_CONFIGURATION_DIRECTLY_ADDRESSABLE_MEMORY_SIZE_16KB                              0x00000001
#define NVA197_SET_L1_CONFIGURATION_DIRECTLY_ADDRESSABLE_MEMORY_SIZE_48KB                              0x00000003

#define NVA197_SET_RENDER_ENABLE_CONTROL                                                                   0x030c
#define NVA197_SET_RENDER_ENABLE_CONTROL_CONDITIONAL_LOAD_CONSTANT_BUFFER                                     0:0
#define NVA197_SET_RENDER_ENABLE_CONTROL_CONDITIONAL_LOAD_CONSTANT_BUFFER_FALSE                        0x00000000
#define NVA197_SET_RENDER_ENABLE_CONTROL_CONDITIONAL_LOAD_CONSTANT_BUFFER_TRUE                         0x00000001

#define NVA197_SET_SPA_VERSION                                                                             0x0310
#define NVA197_SET_SPA_VERSION_MINOR                                                                          7:0
#define NVA197_SET_SPA_VERSION_MAJOR                                                                         15:8

#define NVA197_SET_IEEE_CLEAN_UPDATE                                                                       0x0314
#define NVA197_SET_IEEE_CLEAN_UPDATE_ENABLE                                                                   0:0
#define NVA197_SET_IEEE_CLEAN_UPDATE_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_IEEE_CLEAN_UPDATE_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_SNAP_GRID_LINE                                                                          0x0318
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL                                                         3:0
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__2X2                                             0x00000001
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__4X4                                             0x00000002
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__8X8                                             0x00000003
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__16X16                                           0x00000004
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__32X32                                           0x00000005
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__64X64                                           0x00000006
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__128X128                                         0x00000007
#define NVA197_SET_SNAP_GRID_LINE_LOCATIONS_PER_PIXEL__256X256                                         0x00000008
#define NVA197_SET_SNAP_GRID_LINE_ROUNDING_MODE                                                               8:8
#define NVA197_SET_SNAP_GRID_LINE_ROUNDING_MODE_RTNE                                                   0x00000000
#define NVA197_SET_SNAP_GRID_LINE_ROUNDING_MODE_TESLA                                                  0x00000001

#define NVA197_SET_SNAP_GRID_NON_LINE                                                                      0x031c
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL                                                     3:0
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__2X2                                         0x00000001
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__4X4                                         0x00000002
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__8X8                                         0x00000003
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__16X16                                       0x00000004
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__32X32                                       0x00000005
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__64X64                                       0x00000006
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__128X128                                     0x00000007
#define NVA197_SET_SNAP_GRID_NON_LINE_LOCATIONS_PER_PIXEL__256X256                                     0x00000008
#define NVA197_SET_SNAP_GRID_NON_LINE_ROUNDING_MODE                                                           8:8
#define NVA197_SET_SNAP_GRID_NON_LINE_ROUNDING_MODE_RTNE                                               0x00000000
#define NVA197_SET_SNAP_GRID_NON_LINE_ROUNDING_MODE_TESLA                                              0x00000001

#define NVA197_SET_TESSELLATION_PARAMETERS                                                                 0x0320
#define NVA197_SET_TESSELLATION_PARAMETERS_DOMAIN_TYPE                                                        1:0
#define NVA197_SET_TESSELLATION_PARAMETERS_DOMAIN_TYPE_ISOLINE                                         0x00000000
#define NVA197_SET_TESSELLATION_PARAMETERS_DOMAIN_TYPE_TRIANGLE                                        0x00000001
#define NVA197_SET_TESSELLATION_PARAMETERS_DOMAIN_TYPE_QUAD                                            0x00000002
#define NVA197_SET_TESSELLATION_PARAMETERS_SPACING                                                            5:4
#define NVA197_SET_TESSELLATION_PARAMETERS_SPACING_INTEGER                                             0x00000000
#define NVA197_SET_TESSELLATION_PARAMETERS_SPACING_FRACTIONAL_ODD                                      0x00000001
#define NVA197_SET_TESSELLATION_PARAMETERS_SPACING_FRACTIONAL_EVEN                                     0x00000002
#define NVA197_SET_TESSELLATION_PARAMETERS_OUTPUT_PRIMITIVES                                                  9:8
#define NVA197_SET_TESSELLATION_PARAMETERS_OUTPUT_PRIMITIVES_POINTS                                    0x00000000
#define NVA197_SET_TESSELLATION_PARAMETERS_OUTPUT_PRIMITIVES_LINES                                     0x00000001
#define NVA197_SET_TESSELLATION_PARAMETERS_OUTPUT_PRIMITIVES_TRIANGLES_CW                              0x00000002
#define NVA197_SET_TESSELLATION_PARAMETERS_OUTPUT_PRIMITIVES_TRIANGLES_CCW                             0x00000003

#define NVA197_SET_TESSELLATION_LOD_U0_OR_DENSITY                                                          0x0324
#define NVA197_SET_TESSELLATION_LOD_U0_OR_DENSITY_V                                                          31:0

#define NVA197_SET_TESSELLATION_LOD_V0_OR_DETAIL                                                           0x0328
#define NVA197_SET_TESSELLATION_LOD_V0_OR_DETAIL_V                                                           31:0

#define NVA197_SET_TESSELLATION_LOD_U1_OR_W0                                                               0x032c
#define NVA197_SET_TESSELLATION_LOD_U1_OR_W0_V                                                               31:0

#define NVA197_SET_TESSELLATION_LOD_V1                                                                     0x0330
#define NVA197_SET_TESSELLATION_LOD_V1_V                                                                     31:0

#define NVA197_SET_TG_LOD_INTERIOR_U                                                                       0x0334
#define NVA197_SET_TG_LOD_INTERIOR_U_V                                                                       31:0

#define NVA197_SET_TG_LOD_INTERIOR_V                                                                       0x0338
#define NVA197_SET_TG_LOD_INTERIOR_V_V                                                                       31:0

#define NVA197_RESERVED_TG07                                                                               0x033c
#define NVA197_RESERVED_TG07_V                                                                                0:0

#define NVA197_RESERVED_TG08                                                                               0x0340
#define NVA197_RESERVED_TG08_V                                                                                0:0

#define NVA197_RESERVED_TG09                                                                               0x0344
#define NVA197_RESERVED_TG09_V                                                                                0:0

#define NVA197_RESERVED_TG10                                                                               0x0348
#define NVA197_RESERVED_TG10_V                                                                                0:0

#define NVA197_RESERVED_TG11                                                                               0x034c
#define NVA197_RESERVED_TG11_V                                                                                0:0

#define NVA197_RESERVED_TG12                                                                               0x0350
#define NVA197_RESERVED_TG12_V                                                                                0:0

#define NVA197_RESERVED_TG13                                                                               0x0354
#define NVA197_RESERVED_TG13_V                                                                                0:0

#define NVA197_RESERVED_TG14                                                                               0x0358
#define NVA197_RESERVED_TG14_V                                                                                0:0

#define NVA197_RESERVED_TG15                                                                               0x035c
#define NVA197_RESERVED_TG15_V                                                                                0:0

#define NVA197_SET_SUBTILING_PERF_KNOB_A                                                                   0x0360
#define NVA197_SET_SUBTILING_PERF_KNOB_A_FRACTION_OF_SPM_REGISTER_FILE_PER_SUBTILE                            7:0
#define NVA197_SET_SUBTILING_PERF_KNOB_A_FRACTION_OF_SPM_PIXEL_OUTPUT_BUFFER_PER_SUBTILE                     15:8
#define NVA197_SET_SUBTILING_PERF_KNOB_A_FRACTION_OF_SPM_TRIANGLE_RAM_PER_SUBTILE                           23:16
#define NVA197_SET_SUBTILING_PERF_KNOB_A_FRACTION_OF_MAX_QUADS_PER_SUBTILE                                  31:24

#define NVA197_SET_SUBTILING_PERF_KNOB_B                                                                   0x0364
#define NVA197_SET_SUBTILING_PERF_KNOB_B_FRACTION_OF_MAX_PRIMITIVES_PER_SUBTILE                               7:0

#define NVA197_SET_SUBTILING_PERF_KNOB_C                                                                   0x0368
#define NVA197_SET_SUBTILING_PERF_KNOB_C_RESERVED                                                             0:0

#define NVA197_SET_ZCULL_SUBREGION_TO_REPORT                                                               0x036c
#define NVA197_SET_ZCULL_SUBREGION_TO_REPORT_ENABLE                                                           0:0
#define NVA197_SET_ZCULL_SUBREGION_TO_REPORT_ENABLE_FALSE                                              0x00000000
#define NVA197_SET_ZCULL_SUBREGION_TO_REPORT_ENABLE_TRUE                                               0x00000001
#define NVA197_SET_ZCULL_SUBREGION_TO_REPORT_SUBREGION_ID                                                    11:4

#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE                                                             0x0370
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_ENABLE                                                         0:0
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_ENABLE_FALSE                                            0x00000000
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_ENABLE_TRUE                                             0x00000001
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_TYPE                                                           6:4
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_TYPE_DEPTH_TEST                                         0x00000000
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_TYPE_DEPTH_TEST_NO_ACCEPT                               0x00000001
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_TYPE_DEPTH_TEST_LATE_Z                                  0x00000002
#define NVA197_SET_ZCULL_SUBREGION_REPORT_TYPE_TYPE_STENCIL_TEST                                       0x00000003

#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD                                                             0x0374
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_UNPARTITIONED_MODE                                          0:0
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_UNPARTITIONED_MODE_FALSE                             0x00000000
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_UNPARTITIONED_MODE_TRUE                              0x00000001
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_TIMESLICED_MODE                                             4:4
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_TIMESLICED_MODE_FALSE                                0x00000000
#define NVA197_SET_BALANCED_PRIMITIVE_WORKLOAD_IN_TIMESLICED_MODE_TRUE                                 0x00000001

#define NVA197_SET_MAX_PATCHES_PER_BATCH                                                                   0x0378
#define NVA197_SET_MAX_PATCHES_PER_BATCH_V                                                                    5:0

#define NVA197_SET_RASTER_ENABLE                                                                           0x037c
#define NVA197_SET_RASTER_ENABLE_V                                                                            0:0
#define NVA197_SET_RASTER_ENABLE_V_FALSE                                                               0x00000000
#define NVA197_SET_RASTER_ENABLE_V_TRUE                                                                0x00000001

#define NVA197_SET_STREAM_OUT_BUFFER_ENABLE(j)                                                    (0x0380+(j)*32)
#define NVA197_SET_STREAM_OUT_BUFFER_ENABLE_V                                                                 0:0
#define NVA197_SET_STREAM_OUT_BUFFER_ENABLE_V_FALSE                                                    0x00000000
#define NVA197_SET_STREAM_OUT_BUFFER_ENABLE_V_TRUE                                                     0x00000001

#define NVA197_SET_STREAM_OUT_BUFFER_ADDRESS_A(j)                                                 (0x0384+(j)*32)
#define NVA197_SET_STREAM_OUT_BUFFER_ADDRESS_A_UPPER                                                          7:0

#define NVA197_SET_STREAM_OUT_BUFFER_ADDRESS_B(j)                                                 (0x0388+(j)*32)
#define NVA197_SET_STREAM_OUT_BUFFER_ADDRESS_B_LOWER                                                         31:0

#define NVA197_SET_STREAM_OUT_BUFFER_SIZE(j)                                                      (0x038c+(j)*32)
#define NVA197_SET_STREAM_OUT_BUFFER_SIZE_BYTES                                                              31:0

#define NVA197_SET_STREAM_OUT_BUFFER_LOAD_WRITE_POINTER(j)                                        (0x0390+(j)*32)
#define NVA197_SET_STREAM_OUT_BUFFER_LOAD_WRITE_POINTER_START_OFFSET                                         31:0

#define NVA197_SET_VAB_DATA_TYPELESS(i)                                                            (0x0400+(i)*4)
#define NVA197_SET_VAB_DATA_TYPELESS_V                                                                       31:0

#define NVA197_SET_STREAM_OUT_CONTROL_STREAM(j)                                                   (0x0700+(j)*16)
#define NVA197_SET_STREAM_OUT_CONTROL_STREAM_SELECT                                                           1:0

#define NVA197_SET_STREAM_OUT_CONTROL_COMPONENT_COUNT(j)                                          (0x0704+(j)*16)
#define NVA197_SET_STREAM_OUT_CONTROL_COMPONENT_COUNT_MAX                                                     7:0

#define NVA197_SET_STREAM_OUT_CONTROL_STRIDE(j)                                                   (0x0708+(j)*16)
#define NVA197_SET_STREAM_OUT_CONTROL_STRIDE_BYTES                                                           31:0

#define NVA197_SET_RASTER_INPUT                                                                            0x0740
#define NVA197_SET_RASTER_INPUT_STREAM_SELECT                                                                 1:0

#define NVA197_SET_STREAM_OUTPUT                                                                           0x0744
#define NVA197_SET_STREAM_OUTPUT_ENABLE                                                                       0:0
#define NVA197_SET_STREAM_OUTPUT_ENABLE_FALSE                                                          0x00000000
#define NVA197_SET_STREAM_OUTPUT_ENABLE_TRUE                                                           0x00000001

#define NVA197_SET_DA_PRIMITIVE_RESTART_TOPOLOGY_CHANGE                                                    0x0748
#define NVA197_SET_DA_PRIMITIVE_RESTART_TOPOLOGY_CHANGE_ENABLE                                                0:0
#define NVA197_SET_DA_PRIMITIVE_RESTART_TOPOLOGY_CHANGE_ENABLE_FALSE                                   0x00000000
#define NVA197_SET_DA_PRIMITIVE_RESTART_TOPOLOGY_CHANGE_ENABLE_TRUE                                    0x00000001

#define NVA197_SET_ALPHA_FRACTION                                                                          0x074c
#define NVA197_SET_ALPHA_FRACTION_V                                                                           7:0

#define NVA197_SET_HYBRID_ANTI_ALIAS_CONTROL                                                               0x0754
#define NVA197_SET_HYBRID_ANTI_ALIAS_CONTROL_PASSES                                                           3:0
#define NVA197_SET_HYBRID_ANTI_ALIAS_CONTROL_CENTROID                                                         4:4
#define NVA197_SET_HYBRID_ANTI_ALIAS_CONTROL_CENTROID_PER_FRAGMENT                                     0x00000000
#define NVA197_SET_HYBRID_ANTI_ALIAS_CONTROL_CENTROID_PER_PASS                                         0x00000001

#define NVA197_SET_MAX_TI_WARPS_PER_BATCH                                                                  0x075c
#define NVA197_SET_MAX_TI_WARPS_PER_BATCH_V                                                                   5:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_WINDOW                                                              0x077c
#define NVA197_SET_SHADER_LOCAL_MEMORY_WINDOW_BASE_ADDRESS                                                   31:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_A                                                                   0x0790
#define NVA197_SET_SHADER_LOCAL_MEMORY_A_ADDRESS_UPPER                                                        7:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_B                                                                   0x0794
#define NVA197_SET_SHADER_LOCAL_MEMORY_B_ADDRESS_LOWER                                                       31:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_C                                                                   0x0798
#define NVA197_SET_SHADER_LOCAL_MEMORY_C_SIZE_UPPER                                                           5:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_D                                                                   0x079c
#define NVA197_SET_SHADER_LOCAL_MEMORY_D_SIZE_LOWER                                                          31:0

#define NVA197_SET_SHADER_LOCAL_MEMORY_E                                                                   0x07a0
#define NVA197_SET_SHADER_LOCAL_MEMORY_E_DEFAULT_SIZE_PER_WARP                                               25:0

#define NVA197_SET_COLOR_ZERO_BANDWIDTH_CLEAR                                                              0x07a4
#define NVA197_SET_COLOR_ZERO_BANDWIDTH_CLEAR_SLOT_DISABLE_MASK                                              14:0

#define NVA197_SET_Z_ZERO_BANDWIDTH_CLEAR                                                                  0x07a8
#define NVA197_SET_Z_ZERO_BANDWIDTH_CLEAR_SLOT_DISABLE_MASK                                                  14:0

#define NVA197_SET_ISBE_SAVE_RESTORE_PROGRAM                                                               0x07ac
#define NVA197_SET_ISBE_SAVE_RESTORE_PROGRAM_OFFSET                                                          31:0

#define NVA197_SET_VAB_VERTEX2F(i)                                                                 (0x07b0+(i)*4)
#define NVA197_SET_VAB_VERTEX2F_V                                                                            31:0

#define NVA197_SET_ZCULL_REGION_SIZE_A                                                                     0x07c0
#define NVA197_SET_ZCULL_REGION_SIZE_A_WIDTH                                                                 15:0

#define NVA197_SET_ZCULL_REGION_SIZE_B                                                                     0x07c4
#define NVA197_SET_ZCULL_REGION_SIZE_B_HEIGHT                                                                15:0

#define NVA197_SET_ZCULL_REGION_SIZE_C                                                                     0x07c8
#define NVA197_SET_ZCULL_REGION_SIZE_C_DEPTH                                                                 15:0

#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_C                                                             0x07cc
#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_C_DEPTH                                                         15:0

#define NVA197_SET_CULL_BEFORE_FETCH                                                                       0x07dc
#define NVA197_SET_CULL_BEFORE_FETCH_FETCH_STREAMS_ONCE                                                       0:0
#define NVA197_SET_CULL_BEFORE_FETCH_FETCH_STREAMS_ONCE_FALSE                                          0x00000000
#define NVA197_SET_CULL_BEFORE_FETCH_FETCH_STREAMS_ONCE_TRUE                                           0x00000001

#define NVA197_SET_ZCULL_REGION_LOCATION                                                                   0x07e0
#define NVA197_SET_ZCULL_REGION_LOCATION_START_ALIQUOT                                                       15:0
#define NVA197_SET_ZCULL_REGION_LOCATION_ALIQUOT_COUNT                                                      31:16

#define NVA197_SET_ZCULL_REGION_ALIQUOTS                                                                   0x07e4
#define NVA197_SET_ZCULL_REGION_ALIQUOTS_PER_LAYER                                                           15:0

#define NVA197_SET_ZCULL_STORAGE_A                                                                         0x07e8
#define NVA197_SET_ZCULL_STORAGE_A_ADDRESS_UPPER                                                              7:0

#define NVA197_SET_ZCULL_STORAGE_B                                                                         0x07ec
#define NVA197_SET_ZCULL_STORAGE_B_ADDRESS_LOWER                                                             31:0

#define NVA197_SET_ZCULL_STORAGE_C                                                                         0x07f0
#define NVA197_SET_ZCULL_STORAGE_C_LIMIT_ADDRESS_UPPER                                                        7:0

#define NVA197_SET_ZCULL_STORAGE_D                                                                         0x07f4
#define NVA197_SET_ZCULL_STORAGE_D_LIMIT_ADDRESS_LOWER                                                       31:0

#define NVA197_SET_ZT_READ_ONLY                                                                            0x07f8
#define NVA197_SET_ZT_READ_ONLY_ENABLE_Z                                                                      0:0
#define NVA197_SET_ZT_READ_ONLY_ENABLE_Z_FALSE                                                         0x00000000
#define NVA197_SET_ZT_READ_ONLY_ENABLE_Z_TRUE                                                          0x00000001
#define NVA197_SET_ZT_READ_ONLY_ENABLE_STENCIL                                                                4:4
#define NVA197_SET_ZT_READ_ONLY_ENABLE_STENCIL_FALSE                                                   0x00000000
#define NVA197_SET_ZT_READ_ONLY_ENABLE_STENCIL_TRUE                                                    0x00000001

#define NVA197_SET_TEXTURE_INSTRUCTION_OPERAND                                                             0x07fc
#define NVA197_SET_TEXTURE_INSTRUCTION_OPERAND_ORDERING                                                       0:0
#define NVA197_SET_TEXTURE_INSTRUCTION_OPERAND_ORDERING_FERMI_ORDER                                    0x00000000
#define NVA197_SET_TEXTURE_INSTRUCTION_OPERAND_ORDERING_KEPLER_ORDER                                   0x00000001

#define NVA197_SET_COLOR_TARGET_A(j)                                                              (0x0800+(j)*64)
#define NVA197_SET_COLOR_TARGET_A_OFFSET_UPPER                                                                7:0

#define NVA197_SET_COLOR_TARGET_B(j)                                                              (0x0804+(j)*64)
#define NVA197_SET_COLOR_TARGET_B_OFFSET_LOWER                                                               31:0

#define NVA197_SET_COLOR_TARGET_WIDTH(j)                                                          (0x0808+(j)*64)
#define NVA197_SET_COLOR_TARGET_WIDTH_V                                                                      27:0

#define NVA197_SET_COLOR_TARGET_HEIGHT(j)                                                         (0x080c+(j)*64)
#define NVA197_SET_COLOR_TARGET_HEIGHT_V                                                                     16:0

#define NVA197_SET_COLOR_TARGET_FORMAT(j)                                                         (0x0810+(j)*64)
#define NVA197_SET_COLOR_TARGET_FORMAT_V                                                                      7:0
#define NVA197_SET_COLOR_TARGET_FORMAT_V_DISABLED                                                      0x00000000
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF32_GF32_BF32_AF32                                           0x000000C0
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS32_GS32_BS32_AS32                                           0x000000C1
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU32_GU32_BU32_AU32                                           0x000000C2
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF32_GF32_BF32_X32                                            0x000000C3
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS32_GS32_BS32_X32                                            0x000000C4
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU32_GU32_BU32_X32                                            0x000000C5
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R16_G16_B16_A16                                               0x000000C6
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RN16_GN16_BN16_AN16                                           0x000000C7
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS16_GS16_BS16_AS16                                           0x000000C8
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU16_GU16_BU16_AU16                                           0x000000C9
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF16_GF16_BF16_AF16                                           0x000000CA
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF32_GF32                                                     0x000000CB
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS32_GS32                                                     0x000000CC
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU32_GU32                                                     0x000000CD
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF16_GF16_BF16_X16                                            0x000000CE
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8R8G8B8                                                      0x000000CF
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8RL8GL8BL8                                                   0x000000D0
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A2B10G10R10                                                   0x000000D1
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AU2BU10GU10RU10                                               0x000000D2
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8B8G8R8                                                      0x000000D5
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8BL8GL8RL8                                                   0x000000D6
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AN8BN8GN8RN8                                                  0x000000D7
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AS8BS8GS8RS8                                                  0x000000D8
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AU8BU8GU8RU8                                                  0x000000D9
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R16_G16                                                       0x000000DA
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RN16_GN16                                                     0x000000DB
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS16_GS16                                                     0x000000DC
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU16_GU16                                                     0x000000DD
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF16_GF16                                                     0x000000DE
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A2R10G10B10                                                   0x000000DF
#define NVA197_SET_COLOR_TARGET_FORMAT_V_BF10GF11RF11                                                  0x000000E0
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS32                                                          0x000000E3
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU32                                                          0x000000E4
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF32                                                          0x000000E5
#define NVA197_SET_COLOR_TARGET_FORMAT_V_X8R8G8B8                                                      0x000000E6
#define NVA197_SET_COLOR_TARGET_FORMAT_V_X8RL8GL8BL8                                                   0x000000E7
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R5G6B5                                                        0x000000E8
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A1R5G5B5                                                      0x000000E9
#define NVA197_SET_COLOR_TARGET_FORMAT_V_G8R8                                                          0x000000EA
#define NVA197_SET_COLOR_TARGET_FORMAT_V_GN8RN8                                                        0x000000EB
#define NVA197_SET_COLOR_TARGET_FORMAT_V_GS8RS8                                                        0x000000EC
#define NVA197_SET_COLOR_TARGET_FORMAT_V_GU8RU8                                                        0x000000ED
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R16                                                           0x000000EE
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RN16                                                          0x000000EF
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS16                                                          0x000000F0
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU16                                                          0x000000F1
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF16                                                          0x000000F2
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R8                                                            0x000000F3
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RN8                                                           0x000000F4
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RS8                                                           0x000000F5
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RU8                                                           0x000000F6
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8                                                            0x000000F7
#define NVA197_SET_COLOR_TARGET_FORMAT_V_X1R5G5B5                                                      0x000000F8
#define NVA197_SET_COLOR_TARGET_FORMAT_V_X8B8G8R8                                                      0x000000F9
#define NVA197_SET_COLOR_TARGET_FORMAT_V_X8BL8GL8RL8                                                   0x000000FA
#define NVA197_SET_COLOR_TARGET_FORMAT_V_Z1R5G5B5                                                      0x000000FB
#define NVA197_SET_COLOR_TARGET_FORMAT_V_O1R5G5B5                                                      0x000000FC
#define NVA197_SET_COLOR_TARGET_FORMAT_V_Z8R8G8B8                                                      0x000000FD
#define NVA197_SET_COLOR_TARGET_FORMAT_V_O8R8G8B8                                                      0x000000FE
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R32                                                           0x000000FF
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A16                                                           0x00000040
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AF16                                                          0x00000041
#define NVA197_SET_COLOR_TARGET_FORMAT_V_AF32                                                          0x00000042
#define NVA197_SET_COLOR_TARGET_FORMAT_V_A8R8                                                          0x00000043
#define NVA197_SET_COLOR_TARGET_FORMAT_V_R16_A16                                                       0x00000044
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF16_AF16                                                     0x00000045
#define NVA197_SET_COLOR_TARGET_FORMAT_V_RF32_AF32                                                     0x00000046
#define NVA197_SET_COLOR_TARGET_FORMAT_V_B8G8R8A8                                                      0x00000047

#define NVA197_SET_COLOR_TARGET_MEMORY(j)                                                         (0x0814+(j)*64)
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_WIDTH                                                            3:0
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_WIDTH_ONE_GOB                                             0x00000000
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT                                                           7:4
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_ONE_GOB                                            0x00000000
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_TWO_GOBS                                           0x00000001
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_FOUR_GOBS                                          0x00000002
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_EIGHT_GOBS                                         0x00000003
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_SIXTEEN_GOBS                                       0x00000004
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_HEIGHT_THIRTYTWO_GOBS                                     0x00000005
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH                                                           11:8
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_ONE_GOB                                             0x00000000
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_TWO_GOBS                                            0x00000001
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_FOUR_GOBS                                           0x00000002
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_EIGHT_GOBS                                          0x00000003
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_SIXTEEN_GOBS                                        0x00000004
#define NVA197_SET_COLOR_TARGET_MEMORY_BLOCK_DEPTH_THIRTYTWO_GOBS                                      0x00000005
#define NVA197_SET_COLOR_TARGET_MEMORY_LAYOUT                                                               12:12
#define NVA197_SET_COLOR_TARGET_MEMORY_LAYOUT_BLOCKLINEAR                                              0x00000000
#define NVA197_SET_COLOR_TARGET_MEMORY_LAYOUT_PITCH                                                    0x00000001
#define NVA197_SET_COLOR_TARGET_MEMORY_THIRD_DIMENSION_CONTROL                                              16:16
#define NVA197_SET_COLOR_TARGET_MEMORY_THIRD_DIMENSION_CONTROL_THIRD_DIMENSION_DEFINES_ARRAY_SIZE             0x00000000
#define NVA197_SET_COLOR_TARGET_MEMORY_THIRD_DIMENSION_CONTROL_THIRD_DIMENSION_DEFINES_DEPTH_SIZE             0x00000001

#define NVA197_SET_COLOR_TARGET_THIRD_DIMENSION(j)                                                (0x0818+(j)*64)
#define NVA197_SET_COLOR_TARGET_THIRD_DIMENSION_V                                                            27:0

#define NVA197_SET_COLOR_TARGET_ARRAY_PITCH(j)                                                    (0x081c+(j)*64)
#define NVA197_SET_COLOR_TARGET_ARRAY_PITCH_V                                                                31:0

#define NVA197_SET_COLOR_TARGET_LAYER(j)                                                          (0x0820+(j)*64)
#define NVA197_SET_COLOR_TARGET_LAYER_OFFSET                                                                 15:0

#define NVA197_SET_COLOR_TARGET_MARK(j)                                                           (0x0824+(j)*64)
#define NVA197_SET_COLOR_TARGET_MARK_IEEE_CLEAN                                                               0:0
#define NVA197_SET_COLOR_TARGET_MARK_IEEE_CLEAN_FALSE                                                  0x00000000
#define NVA197_SET_COLOR_TARGET_MARK_IEEE_CLEAN_TRUE                                                   0x00000001

#define NVA197_SET_VIEWPORT_SCALE_X(j)                                                            (0x0a00+(j)*32)
#define NVA197_SET_VIEWPORT_SCALE_X_V                                                                        31:0

#define NVA197_SET_VIEWPORT_SCALE_Y(j)                                                            (0x0a04+(j)*32)
#define NVA197_SET_VIEWPORT_SCALE_Y_V                                                                        31:0

#define NVA197_SET_VIEWPORT_SCALE_Z(j)                                                            (0x0a08+(j)*32)
#define NVA197_SET_VIEWPORT_SCALE_Z_V                                                                        31:0

#define NVA197_SET_VIEWPORT_OFFSET_X(j)                                                           (0x0a0c+(j)*32)
#define NVA197_SET_VIEWPORT_OFFSET_X_V                                                                       31:0

#define NVA197_SET_VIEWPORT_OFFSET_Y(j)                                                           (0x0a10+(j)*32)
#define NVA197_SET_VIEWPORT_OFFSET_Y_V                                                                       31:0

#define NVA197_SET_VIEWPORT_OFFSET_Z(j)                                                           (0x0a14+(j)*32)
#define NVA197_SET_VIEWPORT_OFFSET_Z_V                                                                       31:0

#define NVA197_SET_VIEWPORT_CLIP_HORIZONTAL(j)                                                    (0x0c00+(j)*16)
#define NVA197_SET_VIEWPORT_CLIP_HORIZONTAL_X0                                                               15:0
#define NVA197_SET_VIEWPORT_CLIP_HORIZONTAL_WIDTH                                                           31:16

#define NVA197_SET_VIEWPORT_CLIP_VERTICAL(j)                                                      (0x0c04+(j)*16)
#define NVA197_SET_VIEWPORT_CLIP_VERTICAL_Y0                                                                 15:0
#define NVA197_SET_VIEWPORT_CLIP_VERTICAL_HEIGHT                                                            31:16

#define NVA197_SET_VIEWPORT_CLIP_MIN_Z(j)                                                         (0x0c08+(j)*16)
#define NVA197_SET_VIEWPORT_CLIP_MIN_Z_V                                                                     31:0

#define NVA197_SET_VIEWPORT_CLIP_MAX_Z(j)                                                         (0x0c0c+(j)*16)
#define NVA197_SET_VIEWPORT_CLIP_MAX_Z_V                                                                     31:0

#define NVA197_SET_WINDOW_CLIP_HORIZONTAL(j)                                                       (0x0d00+(j)*8)
#define NVA197_SET_WINDOW_CLIP_HORIZONTAL_XMIN                                                               15:0
#define NVA197_SET_WINDOW_CLIP_HORIZONTAL_XMAX                                                              31:16

#define NVA197_SET_WINDOW_CLIP_VERTICAL(j)                                                         (0x0d04+(j)*8)
#define NVA197_SET_WINDOW_CLIP_VERTICAL_YMIN                                                                 15:0
#define NVA197_SET_WINDOW_CLIP_VERTICAL_YMAX                                                                31:16

#define NVA197_SET_CLIP_ID_EXTENT_X(j)                                                             (0x0d40+(j)*8)
#define NVA197_SET_CLIP_ID_EXTENT_X_MINX                                                                     15:0
#define NVA197_SET_CLIP_ID_EXTENT_X_WIDTH                                                                   31:16

#define NVA197_SET_CLIP_ID_EXTENT_Y(j)                                                             (0x0d44+(j)*8)
#define NVA197_SET_CLIP_ID_EXTENT_Y_MINY                                                                     15:0
#define NVA197_SET_CLIP_ID_EXTENT_Y_HEIGHT                                                                  31:16

#define NVA197_SET_MAX_STREAM_OUTPUT_GS_INSTANCES_PER_TASK                                                 0x0d60
#define NVA197_SET_MAX_STREAM_OUTPUT_GS_INSTANCES_PER_TASK_V                                                 10:0

#define NVA197_SET_API_VISIBLE_CALL_LIMIT                                                                  0x0d64
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V                                                                   3:0
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__0                                                         0x00000000
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__1                                                         0x00000001
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__2                                                         0x00000002
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__4                                                         0x00000003
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__8                                                         0x00000004
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__16                                                        0x00000005
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__32                                                        0x00000006
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__64                                                        0x00000007
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V__128                                                       0x00000008
#define NVA197_SET_API_VISIBLE_CALL_LIMIT_V_NO_CHECK                                                   0x0000000F

#define NVA197_SET_STATISTICS_COUNTER                                                                      0x0d68
#define NVA197_SET_STATISTICS_COUNTER_DA_VERTICES_GENERATED_ENABLE                                            0:0
#define NVA197_SET_STATISTICS_COUNTER_DA_VERTICES_GENERATED_ENABLE_FALSE                               0x00000000
#define NVA197_SET_STATISTICS_COUNTER_DA_VERTICES_GENERATED_ENABLE_TRUE                                0x00000001
#define NVA197_SET_STATISTICS_COUNTER_DA_PRIMITIVES_GENERATED_ENABLE                                          1:1
#define NVA197_SET_STATISTICS_COUNTER_DA_PRIMITIVES_GENERATED_ENABLE_FALSE                             0x00000000
#define NVA197_SET_STATISTICS_COUNTER_DA_PRIMITIVES_GENERATED_ENABLE_TRUE                              0x00000001
#define NVA197_SET_STATISTICS_COUNTER_VS_INVOCATIONS_ENABLE                                                   2:2
#define NVA197_SET_STATISTICS_COUNTER_VS_INVOCATIONS_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_VS_INVOCATIONS_ENABLE_TRUE                                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_GS_INVOCATIONS_ENABLE                                                   3:3
#define NVA197_SET_STATISTICS_COUNTER_GS_INVOCATIONS_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_GS_INVOCATIONS_ENABLE_TRUE                                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_GS_PRIMITIVES_GENERATED_ENABLE                                          4:4
#define NVA197_SET_STATISTICS_COUNTER_GS_PRIMITIVES_GENERATED_ENABLE_FALSE                             0x00000000
#define NVA197_SET_STATISTICS_COUNTER_GS_PRIMITIVES_GENERATED_ENABLE_TRUE                              0x00000001
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_SUCCEEDED_ENABLE                                   5:5
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_SUCCEEDED_ENABLE_FALSE                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_SUCCEEDED_ENABLE_TRUE                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_NEEDED_ENABLE                                      6:6
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_NEEDED_ENABLE_FALSE                         0x00000000
#define NVA197_SET_STATISTICS_COUNTER_STREAMING_PRIMITIVES_NEEDED_ENABLE_TRUE                          0x00000001
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_INVOCATIONS_ENABLE                                              7:7
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_INVOCATIONS_ENABLE_FALSE                                 0x00000000
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_INVOCATIONS_ENABLE_TRUE                                  0x00000001
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_PRIMITIVES_GENERATED_ENABLE                                     8:8
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_PRIMITIVES_GENERATED_ENABLE_FALSE                        0x00000000
#define NVA197_SET_STATISTICS_COUNTER_CLIPPER_PRIMITIVES_GENERATED_ENABLE_TRUE                         0x00000001
#define NVA197_SET_STATISTICS_COUNTER_PS_INVOCATIONS_ENABLE                                                   9:9
#define NVA197_SET_STATISTICS_COUNTER_PS_INVOCATIONS_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_PS_INVOCATIONS_ENABLE_TRUE                                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_TI_INVOCATIONS_ENABLE                                                 11:11
#define NVA197_SET_STATISTICS_COUNTER_TI_INVOCATIONS_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_TI_INVOCATIONS_ENABLE_TRUE                                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_TS_INVOCATIONS_ENABLE                                                 12:12
#define NVA197_SET_STATISTICS_COUNTER_TS_INVOCATIONS_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_STATISTICS_COUNTER_TS_INVOCATIONS_ENABLE_TRUE                                       0x00000001
#define NVA197_SET_STATISTICS_COUNTER_TS_PRIMITIVES_GENERATED_ENABLE                                        13:13
#define NVA197_SET_STATISTICS_COUNTER_TS_PRIMITIVES_GENERATED_ENABLE_FALSE                             0x00000000
#define NVA197_SET_STATISTICS_COUNTER_TS_PRIMITIVES_GENERATED_ENABLE_TRUE                              0x00000001
#define NVA197_SET_STATISTICS_COUNTER_TOTAL_STREAMING_PRIMITIVES_NEEDED_SUCCEEDED_ENABLE                    14:14
#define NVA197_SET_STATISTICS_COUNTER_TOTAL_STREAMING_PRIMITIVES_NEEDED_SUCCEEDED_ENABLE_FALSE             0x00000000
#define NVA197_SET_STATISTICS_COUNTER_TOTAL_STREAMING_PRIMITIVES_NEEDED_SUCCEEDED_ENABLE_TRUE             0x00000001
#define NVA197_SET_STATISTICS_COUNTER_VTG_PRIMITIVES_OUT_ENABLE                                             10:10
#define NVA197_SET_STATISTICS_COUNTER_VTG_PRIMITIVES_OUT_ENABLE_FALSE                                  0x00000000
#define NVA197_SET_STATISTICS_COUNTER_VTG_PRIMITIVES_OUT_ENABLE_TRUE                                   0x00000001
#define NVA197_SET_STATISTICS_COUNTER_ALPHA_BETA_CLOCKS_ENABLE                                              15:15
#define NVA197_SET_STATISTICS_COUNTER_ALPHA_BETA_CLOCKS_ENABLE_FALSE                                   0x00000000
#define NVA197_SET_STATISTICS_COUNTER_ALPHA_BETA_CLOCKS_ENABLE_TRUE                                    0x00000001

#define NVA197_SET_CLEAR_RECT_HORIZONTAL                                                                   0x0d6c
#define NVA197_SET_CLEAR_RECT_HORIZONTAL_XMIN                                                                15:0
#define NVA197_SET_CLEAR_RECT_HORIZONTAL_XMAX                                                               31:16

#define NVA197_SET_CLEAR_RECT_VERTICAL                                                                     0x0d70
#define NVA197_SET_CLEAR_RECT_VERTICAL_YMIN                                                                  15:0
#define NVA197_SET_CLEAR_RECT_VERTICAL_YMAX                                                                 31:16

#define NVA197_SET_VERTEX_ARRAY_START                                                                      0x0d74
#define NVA197_SET_VERTEX_ARRAY_START_V                                                                      31:0

#define NVA197_DRAW_VERTEX_ARRAY                                                                           0x0d78
#define NVA197_DRAW_VERTEX_ARRAY_COUNT                                                                       31:0

#define NVA197_SET_VIEWPORT_Z_CLIP                                                                         0x0d7c
#define NVA197_SET_VIEWPORT_Z_CLIP_RANGE                                                                      0:0
#define NVA197_SET_VIEWPORT_Z_CLIP_RANGE_NEGATIVE_W_TO_POSITIVE_W                                      0x00000000
#define NVA197_SET_VIEWPORT_Z_CLIP_RANGE_ZERO_TO_POSITIVE_W                                            0x00000001

#define NVA197_SET_COLOR_CLEAR_VALUE(i)                                                            (0x0d80+(i)*4)
#define NVA197_SET_COLOR_CLEAR_VALUE_V                                                                       31:0

#define NVA197_SET_Z_CLEAR_VALUE                                                                           0x0d90
#define NVA197_SET_Z_CLEAR_VALUE_V                                                                           31:0

#define NVA197_SET_SHADER_CACHE_CONTROL                                                                    0x0d94
#define NVA197_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE                                                0:0
#define NVA197_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE_FALSE                                   0x00000000
#define NVA197_SET_SHADER_CACHE_CONTROL_ICACHE_PREFETCH_ENABLE_TRUE                                    0x00000001

#define NVA197_FORCE_TRANSITION_TO_BETA                                                                    0x0d98
#define NVA197_FORCE_TRANSITION_TO_BETA_V                                                                     0:0

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_ENABLE                                                          0x0d9c
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_ENABLE_V                                                           0:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_ENABLE_V_FALSE                                              0x00000000
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_ENABLE_V_TRUE                                               0x00000001

#define NVA197_SET_STENCIL_CLEAR_VALUE                                                                     0x0da0
#define NVA197_SET_STENCIL_CLEAR_VALUE_V                                                                      7:0

#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI                                                             0x0da4
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION                                                    0:0
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION_FALSE                                       0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_INSTRUCTION_TRUE                                        0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA                                                    4:4
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA_FALSE                                       0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_GLOBAL_DATA_TRUE                                        0x00000001
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT                                                     12:12
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT_FALSE                                          0x00000000
#define NVA197_INVALIDATE_SHADER_CACHES_NO_WFI_CONSTANT_TRUE                                           0x00000001

#define NVA197_SET_FRONT_POLYGON_MODE                                                                      0x0dac
#define NVA197_SET_FRONT_POLYGON_MODE_V                                                                      31:0
#define NVA197_SET_FRONT_POLYGON_MODE_V_POINT                                                          0x00001B00
#define NVA197_SET_FRONT_POLYGON_MODE_V_LINE                                                           0x00001B01
#define NVA197_SET_FRONT_POLYGON_MODE_V_FILL                                                           0x00001B02

#define NVA197_SET_BACK_POLYGON_MODE                                                                       0x0db0
#define NVA197_SET_BACK_POLYGON_MODE_V                                                                       31:0
#define NVA197_SET_BACK_POLYGON_MODE_V_POINT                                                           0x00001B00
#define NVA197_SET_BACK_POLYGON_MODE_V_LINE                                                            0x00001B01
#define NVA197_SET_BACK_POLYGON_MODE_V_FILL                                                            0x00001B02

#define NVA197_SET_POLY_SMOOTH                                                                             0x0db4
#define NVA197_SET_POLY_SMOOTH_ENABLE                                                                         0:0
#define NVA197_SET_POLY_SMOOTH_ENABLE_FALSE                                                            0x00000000
#define NVA197_SET_POLY_SMOOTH_ENABLE_TRUE                                                             0x00000001

#define NVA197_SET_ZT_MARK                                                                                 0x0db8
#define NVA197_SET_ZT_MARK_IEEE_CLEAN                                                                         0:0
#define NVA197_SET_ZT_MARK_IEEE_CLEAN_FALSE                                                            0x00000000
#define NVA197_SET_ZT_MARK_IEEE_CLEAN_TRUE                                                             0x00000001

#define NVA197_SET_ZCULL_DIR_FORMAT                                                                        0x0dbc
#define NVA197_SET_ZCULL_DIR_FORMAT_ZDIR                                                                     15:0
#define NVA197_SET_ZCULL_DIR_FORMAT_ZDIR_LESS                                                          0x00000000
#define NVA197_SET_ZCULL_DIR_FORMAT_ZDIR_GREATER                                                       0x00000001
#define NVA197_SET_ZCULL_DIR_FORMAT_ZFORMAT                                                                 31:16
#define NVA197_SET_ZCULL_DIR_FORMAT_ZFORMAT_MSB                                                        0x00000000
#define NVA197_SET_ZCULL_DIR_FORMAT_ZFORMAT_FP                                                         0x00000001
#define NVA197_SET_ZCULL_DIR_FORMAT_ZFORMAT_ZTRICK                                                     0x00000002
#define NVA197_SET_ZCULL_DIR_FORMAT_ZFORMAT_ZF32_1                                                     0x00000003

#define NVA197_SET_POLY_OFFSET_POINT                                                                       0x0dc0
#define NVA197_SET_POLY_OFFSET_POINT_ENABLE                                                                   0:0
#define NVA197_SET_POLY_OFFSET_POINT_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_POLY_OFFSET_POINT_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_POLY_OFFSET_LINE                                                                        0x0dc4
#define NVA197_SET_POLY_OFFSET_LINE_ENABLE                                                                    0:0
#define NVA197_SET_POLY_OFFSET_LINE_ENABLE_FALSE                                                       0x00000000
#define NVA197_SET_POLY_OFFSET_LINE_ENABLE_TRUE                                                        0x00000001

#define NVA197_SET_POLY_OFFSET_FILL                                                                        0x0dc8
#define NVA197_SET_POLY_OFFSET_FILL_ENABLE                                                                    0:0
#define NVA197_SET_POLY_OFFSET_FILL_ENABLE_FALSE                                                       0x00000000
#define NVA197_SET_POLY_OFFSET_FILL_ENABLE_TRUE                                                        0x00000001

#define NVA197_SET_PATCH                                                                                   0x0dcc
#define NVA197_SET_PATCH_SIZE                                                                                 7:0

#define NVA197_SET_ZCULL_CRITERION                                                                         0x0dd8
#define NVA197_SET_ZCULL_CRITERION_SFUNC                                                                      7:0
#define NVA197_SET_ZCULL_CRITERION_SFUNC_NEVER                                                         0x00000000
#define NVA197_SET_ZCULL_CRITERION_SFUNC_LESS                                                          0x00000001
#define NVA197_SET_ZCULL_CRITERION_SFUNC_EQUAL                                                         0x00000002
#define NVA197_SET_ZCULL_CRITERION_SFUNC_LEQUAL                                                        0x00000003
#define NVA197_SET_ZCULL_CRITERION_SFUNC_GREATER                                                       0x00000004
#define NVA197_SET_ZCULL_CRITERION_SFUNC_NOTEQUAL                                                      0x00000005
#define NVA197_SET_ZCULL_CRITERION_SFUNC_GEQUAL                                                        0x00000006
#define NVA197_SET_ZCULL_CRITERION_SFUNC_ALWAYS                                                        0x00000007
#define NVA197_SET_ZCULL_CRITERION_NO_INVALIDATE                                                              8:8
#define NVA197_SET_ZCULL_CRITERION_NO_INVALIDATE_FALSE                                                 0x00000000
#define NVA197_SET_ZCULL_CRITERION_NO_INVALIDATE_TRUE                                                  0x00000001
#define NVA197_SET_ZCULL_CRITERION_FORCE_MATCH                                                                9:9
#define NVA197_SET_ZCULL_CRITERION_FORCE_MATCH_FALSE                                                   0x00000000
#define NVA197_SET_ZCULL_CRITERION_FORCE_MATCH_TRUE                                                    0x00000001
#define NVA197_SET_ZCULL_CRITERION_SREF                                                                     23:16
#define NVA197_SET_ZCULL_CRITERION_SMASK                                                                    31:24

#define NVA197_SET_SM_QUADRANT_SELECTION_CONTROL                                                           0x0ddc
#define NVA197_SET_SM_QUADRANT_SELECTION_CONTROL_MAX_PIXEL_WARPS                                              7:0

#define NVA197_SET_SM_TIMEOUT_INTERVAL                                                                     0x0de4
#define NVA197_SET_SM_TIMEOUT_INTERVAL_COUNTER_BIT                                                            5:0

#define NVA197_SET_DA_PRIMITIVE_RESTART_VERTEX_ARRAY                                                       0x0de8
#define NVA197_SET_DA_PRIMITIVE_RESTART_VERTEX_ARRAY_ENABLE                                                   0:0
#define NVA197_SET_DA_PRIMITIVE_RESTART_VERTEX_ARRAY_ENABLE_FALSE                                      0x00000000
#define NVA197_SET_DA_PRIMITIVE_RESTART_VERTEX_ARRAY_ENABLE_TRUE                                       0x00000001

#define NVA197_SET_DRAW_INLINE_VERTEX_VAB_UPDATE                                                           0x0dec
#define NVA197_SET_DRAW_INLINE_VERTEX_VAB_UPDATE_ENABLE                                                       0:0
#define NVA197_SET_DRAW_INLINE_VERTEX_VAB_UPDATE_ENABLE_FALSE                                          0x00000000
#define NVA197_SET_DRAW_INLINE_VERTEX_VAB_UPDATE_ENABLE_TRUE                                           0x00000001

#define NVA197_SET_WINDOW_OFFSET_X                                                                         0x0df8
#define NVA197_SET_WINDOW_OFFSET_X_V                                                                         16:0

#define NVA197_SET_WINDOW_OFFSET_Y                                                                         0x0dfc
#define NVA197_SET_WINDOW_OFFSET_Y_V                                                                         17:0

#define NVA197_SET_SCISSOR_ENABLE(j)                                                              (0x0e00+(j)*16)
#define NVA197_SET_SCISSOR_ENABLE_V                                                                           0:0
#define NVA197_SET_SCISSOR_ENABLE_V_FALSE                                                              0x00000000
#define NVA197_SET_SCISSOR_ENABLE_V_TRUE                                                               0x00000001

#define NVA197_SET_SCISSOR_HORIZONTAL(j)                                                          (0x0e04+(j)*16)
#define NVA197_SET_SCISSOR_HORIZONTAL_XMIN                                                                   15:0
#define NVA197_SET_SCISSOR_HORIZONTAL_XMAX                                                                  31:16

#define NVA197_SET_SCISSOR_VERTICAL(j)                                                            (0x0e08+(j)*16)
#define NVA197_SET_SCISSOR_VERTICAL_YMIN                                                                     15:0
#define NVA197_SET_SCISSOR_VERTICAL_YMAX                                                                    31:16

#define NVA197_SET_VAB_NORMAL3S(i)                                                                 (0x0f00+(i)*4)
#define NVA197_SET_VAB_NORMAL3S_V                                                                            31:0

#define NVA197_SET_BACK_STENCIL_FUNC_REF                                                                   0x0f54
#define NVA197_SET_BACK_STENCIL_FUNC_REF_V                                                                    7:0

#define NVA197_SET_BACK_STENCIL_MASK                                                                       0x0f58
#define NVA197_SET_BACK_STENCIL_MASK_V                                                                        7:0

#define NVA197_SET_BACK_STENCIL_FUNC_MASK                                                                  0x0f5c
#define NVA197_SET_BACK_STENCIL_FUNC_MASK_V                                                                   7:0

#define NVA197_SET_VERTEX_STREAM_SUBSTITUTE_A                                                              0x0f84
#define NVA197_SET_VERTEX_STREAM_SUBSTITUTE_A_ADDRESS_UPPER                                                   7:0

#define NVA197_SET_VERTEX_STREAM_SUBSTITUTE_B                                                              0x0f88
#define NVA197_SET_VERTEX_STREAM_SUBSTITUTE_B_ADDRESS_LOWER                                                  31:0

#define NVA197_SET_LINE_MODE_POLYGON_CLIP                                                                  0x0f8c
#define NVA197_SET_LINE_MODE_POLYGON_CLIP_GENERATED_EDGE                                                      0:0
#define NVA197_SET_LINE_MODE_POLYGON_CLIP_GENERATED_EDGE_DRAW_LINE                                     0x00000000
#define NVA197_SET_LINE_MODE_POLYGON_CLIP_GENERATED_EDGE_DO_NOT_DRAW_LINE                              0x00000001

#define NVA197_SET_SINGLE_CT_WRITE_CONTROL                                                                 0x0f90
#define NVA197_SET_SINGLE_CT_WRITE_CONTROL_ENABLE                                                             0:0
#define NVA197_SET_SINGLE_CT_WRITE_CONTROL_ENABLE_FALSE                                                0x00000000
#define NVA197_SET_SINGLE_CT_WRITE_CONTROL_ENABLE_TRUE                                                 0x00000001

#define NVA197_SET_VTG_WARP_WATERMARKS                                                                     0x0f98
#define NVA197_SET_VTG_WARP_WATERMARKS_LOW                                                                   15:0
#define NVA197_SET_VTG_WARP_WATERMARKS_HIGH                                                                 31:16

#define NVA197_SET_DEPTH_BOUNDS_MIN                                                                        0x0f9c
#define NVA197_SET_DEPTH_BOUNDS_MIN_V                                                                        31:0

#define NVA197_SET_DEPTH_BOUNDS_MAX                                                                        0x0fa0
#define NVA197_SET_DEPTH_BOUNDS_MAX_V                                                                        31:0

#define NVA197_SET_CT_MRT_ENABLE                                                                           0x0fac
#define NVA197_SET_CT_MRT_ENABLE_V                                                                            0:0
#define NVA197_SET_CT_MRT_ENABLE_V_FALSE                                                               0x00000000
#define NVA197_SET_CT_MRT_ENABLE_V_TRUE                                                                0x00000001

#define NVA197_SET_NONMULTISAMPLED_Z                                                                       0x0fb0
#define NVA197_SET_NONMULTISAMPLED_Z_V                                                                        0:0
#define NVA197_SET_NONMULTISAMPLED_Z_V_PER_SAMPLE                                                      0x00000000
#define NVA197_SET_NONMULTISAMPLED_Z_V_AT_PIXEL_CENTER                                                 0x00000001

#define NVA197_SET_SAMPLE_MASK_X0_Y0                                                                       0x0fbc
#define NVA197_SET_SAMPLE_MASK_X0_Y0_V                                                                       15:0

#define NVA197_SET_SAMPLE_MASK_X1_Y0                                                                       0x0fc0
#define NVA197_SET_SAMPLE_MASK_X1_Y0_V                                                                       15:0

#define NVA197_SET_SAMPLE_MASK_X0_Y1                                                                       0x0fc4
#define NVA197_SET_SAMPLE_MASK_X0_Y1_V                                                                       15:0

#define NVA197_SET_SAMPLE_MASK_X1_Y1                                                                       0x0fc8
#define NVA197_SET_SAMPLE_MASK_X1_Y1_V                                                                       15:0

#define NVA197_SET_SURFACE_CLIP_ID_MEMORY_A                                                                0x0fcc
#define NVA197_SET_SURFACE_CLIP_ID_MEMORY_A_OFFSET_UPPER                                                      7:0

#define NVA197_SET_SURFACE_CLIP_ID_MEMORY_B                                                                0x0fd0
#define NVA197_SET_SURFACE_CLIP_ID_MEMORY_B_OFFSET_LOWER                                                     31:0

#define NVA197_SET_BLEND_OPT_CONTROL                                                                       0x0fdc
#define NVA197_SET_BLEND_OPT_CONTROL_ALLOW_FLOAT_PIXEL_KILLS                                                  0:0
#define NVA197_SET_BLEND_OPT_CONTROL_ALLOW_FLOAT_PIXEL_KILLS_FALSE                                     0x00000000
#define NVA197_SET_BLEND_OPT_CONTROL_ALLOW_FLOAT_PIXEL_KILLS_TRUE                                      0x00000001

#define NVA197_SET_ZT_A                                                                                    0x0fe0
#define NVA197_SET_ZT_A_OFFSET_UPPER                                                                          7:0

#define NVA197_SET_ZT_B                                                                                    0x0fe4
#define NVA197_SET_ZT_B_OFFSET_LOWER                                                                         31:0

#define NVA197_SET_ZT_FORMAT                                                                               0x0fe8
#define NVA197_SET_ZT_FORMAT_V                                                                                4:0
#define NVA197_SET_ZT_FORMAT_V_Z16                                                                     0x00000013
#define NVA197_SET_ZT_FORMAT_V_Z24S8                                                                   0x00000014
#define NVA197_SET_ZT_FORMAT_V_X8Z24                                                                   0x00000015
#define NVA197_SET_ZT_FORMAT_V_S8Z24                                                                   0x00000016
#define NVA197_SET_ZT_FORMAT_V_V8Z24                                                                   0x00000018
#define NVA197_SET_ZT_FORMAT_V_ZF32                                                                    0x0000000A
#define NVA197_SET_ZT_FORMAT_V_ZF32_X24S8                                                              0x00000019
#define NVA197_SET_ZT_FORMAT_V_X8Z24_X16V8S8                                                           0x0000001D
#define NVA197_SET_ZT_FORMAT_V_ZF32_X16V8X8                                                            0x0000001E
#define NVA197_SET_ZT_FORMAT_V_ZF32_X16V8S8                                                            0x0000001F

#define NVA197_SET_ZT_BLOCK_SIZE                                                                           0x0fec
#define NVA197_SET_ZT_BLOCK_SIZE_WIDTH                                                                        3:0
#define NVA197_SET_ZT_BLOCK_SIZE_WIDTH_ONE_GOB                                                         0x00000000
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT                                                                       7:4
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_ONE_GOB                                                        0x00000000
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_TWO_GOBS                                                       0x00000001
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_FOUR_GOBS                                                      0x00000002
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_EIGHT_GOBS                                                     0x00000003
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_SIXTEEN_GOBS                                                   0x00000004
#define NVA197_SET_ZT_BLOCK_SIZE_HEIGHT_THIRTYTWO_GOBS                                                 0x00000005
#define NVA197_SET_ZT_BLOCK_SIZE_DEPTH                                                                       11:8
#define NVA197_SET_ZT_BLOCK_SIZE_DEPTH_ONE_GOB                                                         0x00000000

#define NVA197_SET_ZT_ARRAY_PITCH                                                                          0x0ff0
#define NVA197_SET_ZT_ARRAY_PITCH_V                                                                          31:0

#define NVA197_SET_SURFACE_CLIP_HORIZONTAL                                                                 0x0ff4
#define NVA197_SET_SURFACE_CLIP_HORIZONTAL_X                                                                 15:0
#define NVA197_SET_SURFACE_CLIP_HORIZONTAL_WIDTH                                                            31:16

#define NVA197_SET_SURFACE_CLIP_VERTICAL                                                                   0x0ff8
#define NVA197_SET_SURFACE_CLIP_VERTICAL_Y                                                                   15:0
#define NVA197_SET_SURFACE_CLIP_VERTICAL_HEIGHT                                                             31:16

#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS                                                       0x1000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_SYSTEM_MEMORY_VOLATILE                                   0:0
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_SYSTEM_MEMORY_VOLATILE_FALSE                      0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_SYSTEM_MEMORY_VOLATILE_TRUE                       0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_POLICY                                                   5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_POLICY_EVICT_FIRST                                0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_POLICY_EVICT_NORMAL                               0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_VAF_REQUESTS_POLICY_EVICT_LAST                                 0x00000002

#define NVA197_SET_TESSELLATION_CUT_HEIGHT                                                                 0x1008
#define NVA197_SET_TESSELLATION_CUT_HEIGHT_V                                                                  4:0

#define NVA197_SET_MAX_GS_INSTANCES_PER_TASK                                                               0x100c
#define NVA197_SET_MAX_GS_INSTANCES_PER_TASK_V                                                               10:0

#define NVA197_SET_MAX_GS_OUTPUT_VERTICES_PER_TASK                                                         0x1010
#define NVA197_SET_MAX_GS_OUTPUT_VERTICES_PER_TASK_V                                                         15:0

#define NVA197_SET_GS_OUTPUT_CB_STORAGE_MULTIPLIER                                                         0x1018
#define NVA197_SET_GS_OUTPUT_CB_STORAGE_MULTIPLIER_V                                                          9:0

#define NVA197_SET_BETA_CB_STORAGE_CONSTRAINT                                                              0x101c
#define NVA197_SET_BETA_CB_STORAGE_CONSTRAINT_ENABLE                                                          0:0
#define NVA197_SET_BETA_CB_STORAGE_CONSTRAINT_ENABLE_FALSE                                             0x00000000
#define NVA197_SET_BETA_CB_STORAGE_CONSTRAINT_ENABLE_TRUE                                              0x00000001

#define NVA197_SET_TI_OUTPUT_CB_STORAGE_MULTIPLIER                                                         0x1020
#define NVA197_SET_TI_OUTPUT_CB_STORAGE_MULTIPLIER_V                                                          9:0

#define NVA197_SET_ALPHA_CB_STORAGE_CONSTRAINT                                                             0x1024
#define NVA197_SET_ALPHA_CB_STORAGE_CONSTRAINT_ENABLE                                                         0:0
#define NVA197_SET_ALPHA_CB_STORAGE_CONSTRAINT_ENABLE_FALSE                                            0x00000000
#define NVA197_SET_ALPHA_CB_STORAGE_CONSTRAINT_ENABLE_TRUE                                             0x00000001

#define NVA197_SET_SPARE_NOOP00                                                                            0x1040
#define NVA197_SET_SPARE_NOOP00_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP01                                                                            0x1044
#define NVA197_SET_SPARE_NOOP01_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP02                                                                            0x1048
#define NVA197_SET_SPARE_NOOP02_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP03                                                                            0x104c
#define NVA197_SET_SPARE_NOOP03_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP04                                                                            0x1050
#define NVA197_SET_SPARE_NOOP04_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP05                                                                            0x1054
#define NVA197_SET_SPARE_NOOP05_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP06                                                                            0x1058
#define NVA197_SET_SPARE_NOOP06_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP07                                                                            0x105c
#define NVA197_SET_SPARE_NOOP07_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP08                                                                            0x1060
#define NVA197_SET_SPARE_NOOP08_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP09                                                                            0x1064
#define NVA197_SET_SPARE_NOOP09_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP10                                                                            0x1068
#define NVA197_SET_SPARE_NOOP10_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP11                                                                            0x106c
#define NVA197_SET_SPARE_NOOP11_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP12                                                                            0x1070
#define NVA197_SET_SPARE_NOOP12_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP13                                                                            0x1074
#define NVA197_SET_SPARE_NOOP13_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP14                                                                            0x1078
#define NVA197_SET_SPARE_NOOP14_V                                                                            31:0

#define NVA197_SET_SPARE_NOOP15                                                                            0x107c
#define NVA197_SET_SPARE_NOOP15_V                                                                            31:0

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM8                                                          0x10cc
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM8_ALL_COVERED_ALL_HIT_ONCE                                    7:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM8_ALL_COVERED                                               23:16

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM10                                                         0x10e0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM10_ALL_COVERED_ALL_HIT_ONCE                                   7:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM10_ALL_COVERED                                              23:16

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM16                                                         0x10e4
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM16_ALL_COVERED_ALL_HIT_ONCE                                   7:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_UNORM16_ALL_COVERED                                              23:16

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP11                                                            0x10e8
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP11_ALL_COVERED_ALL_HIT_ONCE                                      5:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP11_ALL_COVERED                                                 21:16

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP16                                                            0x10ec
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP16_ALL_COVERED_ALL_HIT_ONCE                                      7:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_FP16_ALL_COVERED                                                 23:16

#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_SRGB8                                                           0x10f0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_SRGB8_ALL_COVERED_ALL_HIT_ONCE                                     7:0
#define NVA197_SET_REDUCE_COLOR_THRESHOLDS_SRGB8_ALL_COVERED                                                23:16

#define NVA197_UNBIND_ALL                                                                                  0x10f4
#define NVA197_UNBIND_ALL_CONSTANT_BUFFERS                                                                    8:8
#define NVA197_UNBIND_ALL_CONSTANT_BUFFERS_FALSE                                                       0x00000000
#define NVA197_UNBIND_ALL_CONSTANT_BUFFERS_TRUE                                                        0x00000001

#define NVA197_SET_CLEAR_SURFACE_CONTROL                                                                   0x10f8
#define NVA197_SET_CLEAR_SURFACE_CONTROL_RESPECT_STENCIL_MASK                                                 0:0
#define NVA197_SET_CLEAR_SURFACE_CONTROL_RESPECT_STENCIL_MASK_FALSE                                    0x00000000
#define NVA197_SET_CLEAR_SURFACE_CONTROL_RESPECT_STENCIL_MASK_TRUE                                     0x00000001
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_CLEAR_RECT                                                       4:4
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_CLEAR_RECT_FALSE                                          0x00000000
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_CLEAR_RECT_TRUE                                           0x00000001
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_SCISSOR0                                                         8:8
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_SCISSOR0_FALSE                                            0x00000000
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_SCISSOR0_TRUE                                             0x00000001
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_VIEWPORT_CLIP0                                                 12:12
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_VIEWPORT_CLIP0_FALSE                                      0x00000000
#define NVA197_SET_CLEAR_SURFACE_CONTROL_USE_VIEWPORT_CLIP0_TRUE                                       0x00000001

#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_READ_REQUESTS                                   0x10fc
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_READ_REQUESTS_POLICY                               5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_READ_REQUESTS_POLICY_EVICT_FIRST             0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_READ_REQUESTS_POLICY_EVICT_NORMAL             0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_READ_REQUESTS_POLICY_EVICT_LAST             0x00000002

#define NVA197_NO_OPERATION_DATA_HI                                                                        0x110c
#define NVA197_NO_OPERATION_DATA_HI_V                                                                        31:0

#define NVA197_SET_DEPTH_BIAS_CONTROL                                                                      0x1110
#define NVA197_SET_DEPTH_BIAS_CONTROL_DEPTH_FORMAT_DEPENDENT                                                  0:0
#define NVA197_SET_DEPTH_BIAS_CONTROL_DEPTH_FORMAT_DEPENDENT_FALSE                                     0x00000000
#define NVA197_SET_DEPTH_BIAS_CONTROL_DEPTH_FORMAT_DEPENDENT_TRUE                                      0x00000001

#define NVA197_PM_TRIGGER_END                                                                              0x1114
#define NVA197_PM_TRIGGER_END_V                                                                              31:0

#define NVA197_SET_VERTEX_ID_BASE                                                                          0x1118
#define NVA197_SET_VERTEX_ID_BASE_V                                                                          31:0

#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A(i)                                              (0x1120+(i)*4)
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP0                                           0:0
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP1                                           1:1
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP2                                           2:2
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP3                                           3:3
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE0_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP0                                           4:4
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP1                                           5:5
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP2                                           6:6
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP3                                           7:7
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE1_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP0                                           8:8
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP1                                           9:9
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP2                                         10:10
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP3                                         11:11
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE2_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP0                                         12:12
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP1                                         13:13
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP2                                         14:14
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP3                                         15:15
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE3_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP0                                         16:16
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP1                                         17:17
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP2                                         18:18
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP3                                         19:19
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE4_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP0                                         20:20
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP1                                         21:21
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP2                                         22:22
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP3                                         23:23
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE5_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP0                                         24:24
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP1                                         25:25
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP2                                         26:26
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP3                                         27:27
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE6_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP0                                         28:28
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP1                                         29:29
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP2                                         30:30
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP3                                         31:31
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A_ATTRIBUTE7_COMP3_TRUE                               0x00000001

#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B(i)                                              (0x1128+(i)*4)
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP0                                           0:0
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP1                                           1:1
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP2                                           2:2
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP3                                           3:3
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE0_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP0                                           4:4
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP1                                           5:5
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP2                                           6:6
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP3                                           7:7
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE1_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP0                                           8:8
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP1                                           9:9
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP2                                         10:10
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP3                                         11:11
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE2_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP0                                         12:12
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP1                                         13:13
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP2                                         14:14
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP3                                         15:15
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE3_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP0                                         16:16
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP1                                         17:17
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP2                                         18:18
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP3                                         19:19
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE4_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP0                                         20:20
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP1                                         21:21
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP2                                         22:22
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP3                                         23:23
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE5_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP0                                         24:24
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP1                                         25:25
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP2                                         26:26
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP3                                         27:27
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE6_COMP3_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP0                                         28:28
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP0_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP0_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP1                                         29:29
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP1_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP1_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP2                                         30:30
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP2_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP2_TRUE                               0x00000001
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP3                                         31:31
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP3_FALSE                              0x00000000
#define NVA197_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B_ATTRIBUTE7_COMP3_TRUE                               0x00000001

#define NVA197_SET_BLEND_PER_FORMAT_ENABLE                                                                 0x1140
#define NVA197_SET_BLEND_PER_FORMAT_ENABLE_SNORM8_UNORM16_SNORM16                                             4:4
#define NVA197_SET_BLEND_PER_FORMAT_ENABLE_SNORM8_UNORM16_SNORM16_FALSE                                0x00000000
#define NVA197_SET_BLEND_PER_FORMAT_ENABLE_SNORM8_UNORM16_SNORM16_TRUE                                 0x00000001

#define NVA197_FLUSH_PENDING_WRITES                                                                        0x1144
#define NVA197_FLUSH_PENDING_WRITES_SM_DOES_GLOBAL_STORE                                                      0:0

#define NVA197_SET_VAB_DATA_CONTROL                                                                        0x114c
#define NVA197_SET_VAB_DATA_CONTROL_VAB_INDEX                                                                 7:0
#define NVA197_SET_VAB_DATA_CONTROL_COMPONENT_COUNT                                                          10:8
#define NVA197_SET_VAB_DATA_CONTROL_COMPONENT_BYTE_WIDTH                                                    14:12
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT                                                                  18:16
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_UNUSED_ENUM_DO_NOT_USE_BECAUSE_IT_WILL_GO_AWAY              0x00000000
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_SNORM                                                   0x00000001
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_UNORM                                                   0x00000002
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_SINT                                                    0x00000003
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_UINT                                                    0x00000004
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_USCALED                                                 0x00000005
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_SSCALED                                                 0x00000006
#define NVA197_SET_VAB_DATA_CONTROL_FORMAT_NUM_FLOAT                                                   0x00000007

#define NVA197_SET_VAB_DATA(i)                                                                     (0x1150+(i)*4)
#define NVA197_SET_VAB_DATA_V                                                                                31:0

#define NVA197_SET_VERTEX_ATTRIBUTE_A(i)                                                           (0x1160+(i)*4)
#define NVA197_SET_VERTEX_ATTRIBUTE_A_STREAM                                                                  4:0
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SOURCE                                                                  6:6
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SOURCE_ACTIVE                                                    0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SOURCE_INACTIVE                                                  0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_A_OFFSET                                                                 20:7
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS                                                  26:21
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R32_G32_B32_A32                             0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R32_G32_B32                                 0x00000002
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R16_G16_B16_A16                             0x00000003
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R32_G32                                     0x00000004
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R16_G16_B16                                 0x00000005
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_A8B8G8R8                                    0x0000002F
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R8_G8_B8_A8                                 0x0000000A
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_X8B8G8R8                                    0x00000033
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_A2B10G10R10                                 0x00000030
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_B10G11R11                                   0x00000031
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R16_G16                                     0x0000000F
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R32                                         0x00000012
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R8_G8_B8                                    0x00000013
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_G8R8                                        0x00000032
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R8_G8                                       0x00000018
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R16                                         0x0000001B
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_R8                                          0x0000001D
#define NVA197_SET_VERTEX_ATTRIBUTE_A_COMPONENT_BIT_WIDTHS_A8                                          0x00000034
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE                                                        29:27
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_UNUSED_ENUM_DO_NOT_USE_BECAUSE_IT_WILL_GO_AWAY             0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_SNORM                                         0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_UNORM                                         0x00000002
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_SINT                                          0x00000003
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_UINT                                          0x00000004
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_USCALED                                       0x00000005
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_SSCALED                                       0x00000006
#define NVA197_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE_NUM_FLOAT                                         0x00000007
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SWAP_R_AND_B                                                          31:31
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SWAP_R_AND_B_FALSE                                               0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_A_SWAP_R_AND_B_TRUE                                                0x00000001

#define NVA197_SET_VERTEX_ATTRIBUTE_B(i)                                                           (0x11a0+(i)*4)
#define NVA197_SET_VERTEX_ATTRIBUTE_B_STREAM                                                                  4:0
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SOURCE                                                                  6:6
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SOURCE_ACTIVE                                                    0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SOURCE_INACTIVE                                                  0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_B_OFFSET                                                                 20:7
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS                                                  26:21
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R32_G32_B32_A32                             0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R32_G32_B32                                 0x00000002
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R16_G16_B16_A16                             0x00000003
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R32_G32                                     0x00000004
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R16_G16_B16                                 0x00000005
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_A8B8G8R8                                    0x0000002F
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R8_G8_B8_A8                                 0x0000000A
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_X8B8G8R8                                    0x00000033
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_A2B10G10R10                                 0x00000030
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_B10G11R11                                   0x00000031
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R16_G16                                     0x0000000F
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R32                                         0x00000012
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R8_G8_B8                                    0x00000013
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_G8R8                                        0x00000032
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R8_G8                                       0x00000018
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R16                                         0x0000001B
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_R8                                          0x0000001D
#define NVA197_SET_VERTEX_ATTRIBUTE_B_COMPONENT_BIT_WIDTHS_A8                                          0x00000034
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE                                                        29:27
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_UNUSED_ENUM_DO_NOT_USE_BECAUSE_IT_WILL_GO_AWAY             0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_SNORM                                         0x00000001
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_UNORM                                         0x00000002
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_SINT                                          0x00000003
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_UINT                                          0x00000004
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_USCALED                                       0x00000005
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_SSCALED                                       0x00000006
#define NVA197_SET_VERTEX_ATTRIBUTE_B_NUMERICAL_TYPE_NUM_FLOAT                                         0x00000007
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SWAP_R_AND_B                                                          31:31
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SWAP_R_AND_B_FALSE                                               0x00000000
#define NVA197_SET_VERTEX_ATTRIBUTE_B_SWAP_R_AND_B_TRUE                                                0x00000001

#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST                                                  0x1214
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_START_INDEX                                        15:0
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_COUNT                                             27:16
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY                                          31:28
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POINTS                              0x00000000
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINES                               0x00000001
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_LOOP                           0x00000002
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_STRIP                          0x00000003
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLES                           0x00000004
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_STRIP                      0x00000005
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_FAN                        0x00000006
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUADS                               0x00000007
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUAD_STRIP                          0x00000008
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POLYGON                             0x00000009
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINELIST_ADJCY                      0x0000000A
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINESTRIP_ADJCY                     0x0000000B
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLELIST_ADJCY                  0x0000000C
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLESTRIP_ADJCY                 0x0000000D
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_PATCH                               0x0000000E

#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT                                             0x1218
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_START_INDEX                                   15:0
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_COUNT                                        27:16
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY                                     31:28
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POINTS                         0x00000000
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINES                          0x00000001
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_LOOP                      0x00000002
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_STRIP                     0x00000003
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLES                      0x00000004
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_STRIP                 0x00000005
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_FAN                   0x00000006
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUADS                          0x00000007
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUAD_STRIP                     0x00000008
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POLYGON                        0x00000009
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINELIST_ADJCY                 0x0000000A
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINESTRIP_ADJCY                0x0000000B
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLELIST_ADJCY             0x0000000C
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLESTRIP_ADJCY             0x0000000D
#define NVA197_DRAW_VERTEX_ARRAY_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_PATCH                          0x0000000E

#define NVA197_SET_CT_SELECT                                                                               0x121c
#define NVA197_SET_CT_SELECT_TARGET_COUNT                                                                     3:0
#define NVA197_SET_CT_SELECT_TARGET0                                                                          6:4
#define NVA197_SET_CT_SELECT_TARGET1                                                                          9:7
#define NVA197_SET_CT_SELECT_TARGET2                                                                        12:10
#define NVA197_SET_CT_SELECT_TARGET3                                                                        15:13
#define NVA197_SET_CT_SELECT_TARGET4                                                                        18:16
#define NVA197_SET_CT_SELECT_TARGET5                                                                        21:19
#define NVA197_SET_CT_SELECT_TARGET6                                                                        24:22
#define NVA197_SET_CT_SELECT_TARGET7                                                                        27:25

#define NVA197_SET_COMPRESSION_THRESHOLD                                                                   0x1220
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES                                                              3:0
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__0                                                    0x00000000
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__1                                                    0x00000001
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__2                                                    0x00000002
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__4                                                    0x00000003
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__8                                                    0x00000004
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__16                                                   0x00000005
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__32                                                   0x00000006
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__64                                                   0x00000007
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__128                                                  0x00000008
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__256                                                  0x00000009
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__512                                                  0x0000000A
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__1024                                                 0x0000000B
#define NVA197_SET_COMPRESSION_THRESHOLD_SAMPLES__2048                                                 0x0000000C

#define NVA197_SET_ZT_SIZE_A                                                                               0x1228
#define NVA197_SET_ZT_SIZE_A_WIDTH                                                                           27:0

#define NVA197_SET_ZT_SIZE_B                                                                               0x122c
#define NVA197_SET_ZT_SIZE_B_HEIGHT                                                                          16:0

#define NVA197_SET_ZT_SIZE_C                                                                               0x1230
#define NVA197_SET_ZT_SIZE_C_THIRD_DIMENSION                                                                 15:0
#define NVA197_SET_ZT_SIZE_C_CONTROL                                                                        16:16
#define NVA197_SET_ZT_SIZE_C_CONTROL_THIRD_DIMENSION_DEFINES_ARRAY_SIZE                                0x00000000
#define NVA197_SET_ZT_SIZE_C_CONTROL_ARRAY_SIZE_IS_ONE                                                 0x00000001

#define NVA197_SET_SAMPLER_BINDING                                                                         0x1234
#define NVA197_SET_SAMPLER_BINDING_V                                                                          0:0
#define NVA197_SET_SAMPLER_BINDING_V_INDEPENDENTLY                                                     0x00000000
#define NVA197_SET_SAMPLER_BINDING_V_VIA_HEADER_BINDING                                                0x00000001

#define NVA197_DRAW_AUTO                                                                                   0x123c
#define NVA197_DRAW_AUTO_BYTE_COUNT                                                                          31:0

#define NVA197_SET_CIRCULAR_BUFFER_SIZE                                                                    0x1280
#define NVA197_SET_CIRCULAR_BUFFER_SIZE_CACHE_LINES_PER_SM                                                    9:0

#define NVA197_SET_VTG_REGISTER_WATERMARKS                                                                 0x1284
#define NVA197_SET_VTG_REGISTER_WATERMARKS_LOW                                                               15:0
#define NVA197_SET_VTG_REGISTER_WATERMARKS_HIGH                                                             31:16

#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI                                                        0x1288
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES                                                     0:0
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES_ALL                                          0x00000000
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_LINES_ONE                                          0x00000001
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_NO_WFI_TAG                                                      25:4

#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_READ_REQUESTS                                      0x1290
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_READ_REQUESTS_POLICY                                  5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_READ_REQUESTS_POLICY_EVICT_FIRST               0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_READ_REQUESTS_POLICY_EVICT_NORMAL              0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_READ_REQUESTS_POLICY_EVICT_LAST                0x00000002

#define NVA197_SET_DA_PRIMITIVE_RESTART_INDEX_TOPOLOGY_CHANGE                                              0x12a4
#define NVA197_SET_DA_PRIMITIVE_RESTART_INDEX_TOPOLOGY_CHANGE_V                                              31:0

#define NVA197_SET_SHADER_SCHEDULING                                                                       0x12ac
#define NVA197_SET_SHADER_SCHEDULING_MODE                                                                     0:0
#define NVA197_SET_SHADER_SCHEDULING_MODE_OLDEST_THREAD_FIRST                                          0x00000000
#define NVA197_SET_SHADER_SCHEDULING_MODE_ROUND_ROBIN                                                  0x00000001

#define NVA197_CLEAR_ZCULL_REGION                                                                          0x12c8
#define NVA197_CLEAR_ZCULL_REGION_Z_ENABLE                                                                    0:0
#define NVA197_CLEAR_ZCULL_REGION_Z_ENABLE_FALSE                                                       0x00000000
#define NVA197_CLEAR_ZCULL_REGION_Z_ENABLE_TRUE                                                        0x00000001
#define NVA197_CLEAR_ZCULL_REGION_STENCIL_ENABLE                                                              4:4
#define NVA197_CLEAR_ZCULL_REGION_STENCIL_ENABLE_FALSE                                                 0x00000000
#define NVA197_CLEAR_ZCULL_REGION_STENCIL_ENABLE_TRUE                                                  0x00000001
#define NVA197_CLEAR_ZCULL_REGION_USE_CLEAR_RECT                                                              1:1
#define NVA197_CLEAR_ZCULL_REGION_USE_CLEAR_RECT_FALSE                                                 0x00000000
#define NVA197_CLEAR_ZCULL_REGION_USE_CLEAR_RECT_TRUE                                                  0x00000001
#define NVA197_CLEAR_ZCULL_REGION_USE_RT_ARRAY_INDEX                                                          2:2
#define NVA197_CLEAR_ZCULL_REGION_USE_RT_ARRAY_INDEX_FALSE                                             0x00000000
#define NVA197_CLEAR_ZCULL_REGION_USE_RT_ARRAY_INDEX_TRUE                                              0x00000001
#define NVA197_CLEAR_ZCULL_REGION_RT_ARRAY_INDEX                                                             20:5
#define NVA197_CLEAR_ZCULL_REGION_MAKE_CONSERVATIVE                                                           3:3
#define NVA197_CLEAR_ZCULL_REGION_MAKE_CONSERVATIVE_FALSE                                              0x00000000
#define NVA197_CLEAR_ZCULL_REGION_MAKE_CONSERVATIVE_TRUE                                               0x00000001

#define NVA197_SET_DEPTH_TEST                                                                              0x12cc
#define NVA197_SET_DEPTH_TEST_ENABLE                                                                          0:0
#define NVA197_SET_DEPTH_TEST_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_DEPTH_TEST_ENABLE_TRUE                                                              0x00000001

#define NVA197_SET_FILL_MODE                                                                               0x12d0
#define NVA197_SET_FILL_MODE_V                                                                               31:0
#define NVA197_SET_FILL_MODE_V_POINT                                                                   0x00000001
#define NVA197_SET_FILL_MODE_V_WIREFRAME                                                               0x00000002
#define NVA197_SET_FILL_MODE_V_SOLID                                                                   0x00000003

#define NVA197_SET_SHADE_MODE                                                                              0x12d4
#define NVA197_SET_SHADE_MODE_V                                                                              31:0
#define NVA197_SET_SHADE_MODE_V_FLAT                                                                   0x00000001
#define NVA197_SET_SHADE_MODE_V_GOURAUD                                                                0x00000002
#define NVA197_SET_SHADE_MODE_V_OGL_FLAT                                                               0x00001D00
#define NVA197_SET_SHADE_MODE_V_OGL_SMOOTH                                                             0x00001D01

#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_WRITE_REQUESTS                                  0x12d8
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_WRITE_REQUESTS_POLICY                              5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_FIRST             0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_NORMAL             0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_NONINTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_LAST             0x00000002

#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_WRITE_REQUESTS                                     0x12dc
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_WRITE_REQUESTS_POLICY                                 5:4
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_FIRST              0x00000000
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_NORMAL             0x00000001
#define NVA197_SET_L2_CACHE_CONTROL_FOR_ROP_INTERLOCKED_WRITE_REQUESTS_POLICY_EVICT_LAST               0x00000002

#define NVA197_SET_ALPHA_TO_COVERAGE_DITHER_CONTROL                                                        0x12e0
#define NVA197_SET_ALPHA_TO_COVERAGE_DITHER_CONTROL_DITHER_FOOTPRINT                                          3:0
#define NVA197_SET_ALPHA_TO_COVERAGE_DITHER_CONTROL_DITHER_FOOTPRINT_PIXELS_1X1                        0x00000000
#define NVA197_SET_ALPHA_TO_COVERAGE_DITHER_CONTROL_DITHER_FOOTPRINT_PIXELS_2X2                        0x00000001
#define NVA197_SET_ALPHA_TO_COVERAGE_DITHER_CONTROL_DITHER_FOOTPRINT_PIXELS_1X1_VIRTUAL_SAMPLES             0x00000002

#define NVA197_SET_BLEND_STATE_PER_TARGET                                                                  0x12e4
#define NVA197_SET_BLEND_STATE_PER_TARGET_ENABLE                                                              0:0
#define NVA197_SET_BLEND_STATE_PER_TARGET_ENABLE_FALSE                                                 0x00000000
#define NVA197_SET_BLEND_STATE_PER_TARGET_ENABLE_TRUE                                                  0x00000001

#define NVA197_SET_DEPTH_WRITE                                                                             0x12e8
#define NVA197_SET_DEPTH_WRITE_ENABLE                                                                         0:0
#define NVA197_SET_DEPTH_WRITE_ENABLE_FALSE                                                            0x00000000
#define NVA197_SET_DEPTH_WRITE_ENABLE_TRUE                                                             0x00000001

#define NVA197_SET_ALPHA_TEST                                                                              0x12ec
#define NVA197_SET_ALPHA_TEST_ENABLE                                                                          0:0
#define NVA197_SET_ALPHA_TEST_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_ALPHA_TEST_ENABLE_TRUE                                                              0x00000001

#define NVA197_SET_INLINE_INDEX4X8_ALIGN                                                                   0x1300
#define NVA197_SET_INLINE_INDEX4X8_ALIGN_COUNT                                                               29:0
#define NVA197_SET_INLINE_INDEX4X8_ALIGN_START                                                              31:30

#define NVA197_DRAW_INLINE_INDEX4X8                                                                        0x1304
#define NVA197_DRAW_INLINE_INDEX4X8_INDEX0                                                                    7:0
#define NVA197_DRAW_INLINE_INDEX4X8_INDEX1                                                                   15:8
#define NVA197_DRAW_INLINE_INDEX4X8_INDEX2                                                                  23:16
#define NVA197_DRAW_INLINE_INDEX4X8_INDEX3                                                                  31:24

#define NVA197_D3D_SET_CULL_MODE                                                                           0x1308
#define NVA197_D3D_SET_CULL_MODE_V                                                                           31:0
#define NVA197_D3D_SET_CULL_MODE_V_NONE                                                                0x00000001
#define NVA197_D3D_SET_CULL_MODE_V_CW                                                                  0x00000002
#define NVA197_D3D_SET_CULL_MODE_V_CCW                                                                 0x00000003

#define NVA197_SET_DEPTH_FUNC                                                                              0x130c
#define NVA197_SET_DEPTH_FUNC_V                                                                              31:0
#define NVA197_SET_DEPTH_FUNC_V_OGL_NEVER                                                              0x00000200
#define NVA197_SET_DEPTH_FUNC_V_OGL_LESS                                                               0x00000201
#define NVA197_SET_DEPTH_FUNC_V_OGL_EQUAL                                                              0x00000202
#define NVA197_SET_DEPTH_FUNC_V_OGL_LEQUAL                                                             0x00000203
#define NVA197_SET_DEPTH_FUNC_V_OGL_GREATER                                                            0x00000204
#define NVA197_SET_DEPTH_FUNC_V_OGL_NOTEQUAL                                                           0x00000205
#define NVA197_SET_DEPTH_FUNC_V_OGL_GEQUAL                                                             0x00000206
#define NVA197_SET_DEPTH_FUNC_V_OGL_ALWAYS                                                             0x00000207
#define NVA197_SET_DEPTH_FUNC_V_D3D_NEVER                                                              0x00000001
#define NVA197_SET_DEPTH_FUNC_V_D3D_LESS                                                               0x00000002
#define NVA197_SET_DEPTH_FUNC_V_D3D_EQUAL                                                              0x00000003
#define NVA197_SET_DEPTH_FUNC_V_D3D_LESSEQUAL                                                          0x00000004
#define NVA197_SET_DEPTH_FUNC_V_D3D_GREATER                                                            0x00000005
#define NVA197_SET_DEPTH_FUNC_V_D3D_NOTEQUAL                                                           0x00000006
#define NVA197_SET_DEPTH_FUNC_V_D3D_GREATEREQUAL                                                       0x00000007
#define NVA197_SET_DEPTH_FUNC_V_D3D_ALWAYS                                                             0x00000008

#define NVA197_SET_ALPHA_REF                                                                               0x1310
#define NVA197_SET_ALPHA_REF_V                                                                               31:0

#define NVA197_SET_ALPHA_FUNC                                                                              0x1314
#define NVA197_SET_ALPHA_FUNC_V                                                                              31:0
#define NVA197_SET_ALPHA_FUNC_V_OGL_NEVER                                                              0x00000200
#define NVA197_SET_ALPHA_FUNC_V_OGL_LESS                                                               0x00000201
#define NVA197_SET_ALPHA_FUNC_V_OGL_EQUAL                                                              0x00000202
#define NVA197_SET_ALPHA_FUNC_V_OGL_LEQUAL                                                             0x00000203
#define NVA197_SET_ALPHA_FUNC_V_OGL_GREATER                                                            0x00000204
#define NVA197_SET_ALPHA_FUNC_V_OGL_NOTEQUAL                                                           0x00000205
#define NVA197_SET_ALPHA_FUNC_V_OGL_GEQUAL                                                             0x00000206
#define NVA197_SET_ALPHA_FUNC_V_OGL_ALWAYS                                                             0x00000207
#define NVA197_SET_ALPHA_FUNC_V_D3D_NEVER                                                              0x00000001
#define NVA197_SET_ALPHA_FUNC_V_D3D_LESS                                                               0x00000002
#define NVA197_SET_ALPHA_FUNC_V_D3D_EQUAL                                                              0x00000003
#define NVA197_SET_ALPHA_FUNC_V_D3D_LESSEQUAL                                                          0x00000004
#define NVA197_SET_ALPHA_FUNC_V_D3D_GREATER                                                            0x00000005
#define NVA197_SET_ALPHA_FUNC_V_D3D_NOTEQUAL                                                           0x00000006
#define NVA197_SET_ALPHA_FUNC_V_D3D_GREATEREQUAL                                                       0x00000007
#define NVA197_SET_ALPHA_FUNC_V_D3D_ALWAYS                                                             0x00000008

#define NVA197_SET_DRAW_AUTO_STRIDE                                                                        0x1318
#define NVA197_SET_DRAW_AUTO_STRIDE_V                                                                        11:0

#define NVA197_SET_BLEND_CONST_RED                                                                         0x131c
#define NVA197_SET_BLEND_CONST_RED_V                                                                         31:0

#define NVA197_SET_BLEND_CONST_GREEN                                                                       0x1320
#define NVA197_SET_BLEND_CONST_GREEN_V                                                                       31:0

#define NVA197_SET_BLEND_CONST_BLUE                                                                        0x1324
#define NVA197_SET_BLEND_CONST_BLUE_V                                                                        31:0

#define NVA197_SET_BLEND_CONST_ALPHA                                                                       0x1328
#define NVA197_SET_BLEND_CONST_ALPHA_V                                                                       31:0

#define NVA197_INVALIDATE_SAMPLER_CACHE                                                                    0x1330
#define NVA197_INVALIDATE_SAMPLER_CACHE_LINES                                                                 0:0
#define NVA197_INVALIDATE_SAMPLER_CACHE_LINES_ALL                                                      0x00000000
#define NVA197_INVALIDATE_SAMPLER_CACHE_LINES_ONE                                                      0x00000001
#define NVA197_INVALIDATE_SAMPLER_CACHE_TAG                                                                  25:4

#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE                                                             0x1334
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_LINES                                                          0:0
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_LINES_ALL                                               0x00000000
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_LINES_ONE                                               0x00000001
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_TAG                                                           25:4

#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE                                                               0x1338
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_LINES                                                            0:0
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_LINES_ALL                                                 0x00000000
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_LINES_ONE                                                 0x00000001
#define NVA197_INVALIDATE_TEXTURE_DATA_CACHE_TAG                                                             25:4

#define NVA197_SET_BLEND_SEPARATE_FOR_ALPHA                                                                0x133c
#define NVA197_SET_BLEND_SEPARATE_FOR_ALPHA_ENABLE                                                            0:0
#define NVA197_SET_BLEND_SEPARATE_FOR_ALPHA_ENABLE_FALSE                                               0x00000000
#define NVA197_SET_BLEND_SEPARATE_FOR_ALPHA_ENABLE_TRUE                                                0x00000001

#define NVA197_SET_BLEND_COLOR_OP                                                                          0x1340
#define NVA197_SET_BLEND_COLOR_OP_V                                                                          31:0
#define NVA197_SET_BLEND_COLOR_OP_V_OGL_FUNC_SUBTRACT                                                  0x0000800A
#define NVA197_SET_BLEND_COLOR_OP_V_OGL_FUNC_REVERSE_SUBTRACT                                          0x0000800B
#define NVA197_SET_BLEND_COLOR_OP_V_OGL_FUNC_ADD                                                       0x00008006
#define NVA197_SET_BLEND_COLOR_OP_V_OGL_MIN                                                            0x00008007
#define NVA197_SET_BLEND_COLOR_OP_V_OGL_MAX                                                            0x00008008
#define NVA197_SET_BLEND_COLOR_OP_V_D3D_ADD                                                            0x00000001
#define NVA197_SET_BLEND_COLOR_OP_V_D3D_SUBTRACT                                                       0x00000002
#define NVA197_SET_BLEND_COLOR_OP_V_D3D_REVSUBTRACT                                                    0x00000003
#define NVA197_SET_BLEND_COLOR_OP_V_D3D_MIN                                                            0x00000004
#define NVA197_SET_BLEND_COLOR_OP_V_D3D_MAX                                                            0x00000005

#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF                                                                0x1344
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V                                                                31:0
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ZERO                                                 0x00004000
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE                                                  0x00004001
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_SRC_COLOR                                            0x00004300
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                                  0x00004301
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_SRC_ALPHA                                            0x00004302
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                                  0x00004303
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_DST_ALPHA                                            0x00004304
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                                  0x00004305
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_DST_COLOR                                            0x00004306
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_COLOR                                  0x00004307
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_SRC_ALPHA_SATURATE                                   0x00004308
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_CONSTANT_COLOR                                       0x0000C001
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                             0x0000C002
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_CONSTANT_ALPHA                                       0x0000C003
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                             0x0000C004
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_SRC1COLOR                                            0x0000C900
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_INVSRC1COLOR                                         0x0000C901
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_SRC1ALPHA                                            0x0000C902
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_INVSRC1ALPHA                                         0x0000C903
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_ZERO                                                 0x00000001
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_ONE                                                  0x00000002
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_SRCCOLOR                                             0x00000003
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVSRCCOLOR                                          0x00000004
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_SRCALPHA                                             0x00000005
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVSRCALPHA                                          0x00000006
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_DESTALPHA                                            0x00000007
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVDESTALPHA                                         0x00000008
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_DESTCOLOR                                            0x00000009
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVDESTCOLOR                                         0x0000000A
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_SRCALPHASAT                                          0x0000000B
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_BOTHSRCALPHA                                         0x0000000C
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_BOTHINVSRCALPHA                                      0x0000000D
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_BLENDFACTOR                                          0x0000000E
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVBLENDFACTOR                                       0x0000000F
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_SRC1COLOR                                            0x00000010
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVSRC1COLOR                                         0x00000011
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_SRC1ALPHA                                            0x00000012
#define NVA197_SET_BLEND_COLOR_SOURCE_COEFF_V_D3D_INVSRC1ALPHA                                         0x00000013

#define NVA197_SET_BLEND_COLOR_DEST_COEFF                                                                  0x1348
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V                                                                  31:0
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ZERO                                                   0x00004000
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE                                                    0x00004001
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_SRC_COLOR                                              0x00004300
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                                    0x00004301
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_SRC_ALPHA                                              0x00004302
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                                    0x00004303
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_DST_ALPHA                                              0x00004304
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                                    0x00004305
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_DST_COLOR                                              0x00004306
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_DST_COLOR                                    0x00004307
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_SRC_ALPHA_SATURATE                                     0x00004308
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_CONSTANT_COLOR                                         0x0000C001
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                               0x0000C002
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_CONSTANT_ALPHA                                         0x0000C003
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                               0x0000C004
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_SRC1COLOR                                              0x0000C900
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_INVSRC1COLOR                                           0x0000C901
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_SRC1ALPHA                                              0x0000C902
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_OGL_INVSRC1ALPHA                                           0x0000C903
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_ZERO                                                   0x00000001
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_ONE                                                    0x00000002
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_SRCCOLOR                                               0x00000003
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVSRCCOLOR                                            0x00000004
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_SRCALPHA                                               0x00000005
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVSRCALPHA                                            0x00000006
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_DESTALPHA                                              0x00000007
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVDESTALPHA                                           0x00000008
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_DESTCOLOR                                              0x00000009
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVDESTCOLOR                                           0x0000000A
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_SRCALPHASAT                                            0x0000000B
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_BLENDFACTOR                                            0x0000000E
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVBLENDFACTOR                                         0x0000000F
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_SRC1COLOR                                              0x00000010
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVSRC1COLOR                                           0x00000011
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_SRC1ALPHA                                              0x00000012
#define NVA197_SET_BLEND_COLOR_DEST_COEFF_V_D3D_INVSRC1ALPHA                                           0x00000013

#define NVA197_SET_BLEND_ALPHA_OP                                                                          0x134c
#define NVA197_SET_BLEND_ALPHA_OP_V                                                                          31:0
#define NVA197_SET_BLEND_ALPHA_OP_V_OGL_FUNC_SUBTRACT                                                  0x0000800A
#define NVA197_SET_BLEND_ALPHA_OP_V_OGL_FUNC_REVERSE_SUBTRACT                                          0x0000800B
#define NVA197_SET_BLEND_ALPHA_OP_V_OGL_FUNC_ADD                                                       0x00008006
#define NVA197_SET_BLEND_ALPHA_OP_V_OGL_MIN                                                            0x00008007
#define NVA197_SET_BLEND_ALPHA_OP_V_OGL_MAX                                                            0x00008008
#define NVA197_SET_BLEND_ALPHA_OP_V_D3D_ADD                                                            0x00000001
#define NVA197_SET_BLEND_ALPHA_OP_V_D3D_SUBTRACT                                                       0x00000002
#define NVA197_SET_BLEND_ALPHA_OP_V_D3D_REVSUBTRACT                                                    0x00000003
#define NVA197_SET_BLEND_ALPHA_OP_V_D3D_MIN                                                            0x00000004
#define NVA197_SET_BLEND_ALPHA_OP_V_D3D_MAX                                                            0x00000005

#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF                                                                0x1350
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V                                                                31:0
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ZERO                                                 0x00004000
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE                                                  0x00004001
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_SRC_COLOR                                            0x00004300
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                                  0x00004301
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_SRC_ALPHA                                            0x00004302
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                                  0x00004303
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_DST_ALPHA                                            0x00004304
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                                  0x00004305
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_DST_COLOR                                            0x00004306
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_COLOR                                  0x00004307
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_SRC_ALPHA_SATURATE                                   0x00004308
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_CONSTANT_COLOR                                       0x0000C001
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                             0x0000C002
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_CONSTANT_ALPHA                                       0x0000C003
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                             0x0000C004
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_SRC1COLOR                                            0x0000C900
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_INVSRC1COLOR                                         0x0000C901
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_SRC1ALPHA                                            0x0000C902
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_OGL_INVSRC1ALPHA                                         0x0000C903
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_ZERO                                                 0x00000001
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_ONE                                                  0x00000002
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_SRCCOLOR                                             0x00000003
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVSRCCOLOR                                          0x00000004
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_SRCALPHA                                             0x00000005
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVSRCALPHA                                          0x00000006
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_DESTALPHA                                            0x00000007
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVDESTALPHA                                         0x00000008
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_DESTCOLOR                                            0x00000009
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVDESTCOLOR                                         0x0000000A
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_SRCALPHASAT                                          0x0000000B
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_BOTHSRCALPHA                                         0x0000000C
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_BOTHINVSRCALPHA                                      0x0000000D
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_BLENDFACTOR                                          0x0000000E
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVBLENDFACTOR                                       0x0000000F
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_SRC1COLOR                                            0x00000010
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVSRC1COLOR                                         0x00000011
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_SRC1ALPHA                                            0x00000012
#define NVA197_SET_BLEND_ALPHA_SOURCE_COEFF_V_D3D_INVSRC1ALPHA                                         0x00000013

#define NVA197_SET_GLOBAL_COLOR_KEY                                                                        0x1354
#define NVA197_SET_GLOBAL_COLOR_KEY_ENABLE                                                                    0:0
#define NVA197_SET_GLOBAL_COLOR_KEY_ENABLE_FALSE                                                       0x00000000
#define NVA197_SET_GLOBAL_COLOR_KEY_ENABLE_TRUE                                                        0x00000001

#define NVA197_SET_BLEND_ALPHA_DEST_COEFF                                                                  0x1358
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V                                                                  31:0
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ZERO                                                   0x00004000
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE                                                    0x00004001
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_SRC_COLOR                                              0x00004300
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                                    0x00004301
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_SRC_ALPHA                                              0x00004302
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                                    0x00004303
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_DST_ALPHA                                              0x00004304
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                                    0x00004305
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_DST_COLOR                                              0x00004306
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_DST_COLOR                                    0x00004307
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_SRC_ALPHA_SATURATE                                     0x00004308
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_CONSTANT_COLOR                                         0x0000C001
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                               0x0000C002
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_CONSTANT_ALPHA                                         0x0000C003
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                               0x0000C004
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_SRC1COLOR                                              0x0000C900
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_INVSRC1COLOR                                           0x0000C901
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_SRC1ALPHA                                              0x0000C902
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_OGL_INVSRC1ALPHA                                           0x0000C903
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_ZERO                                                   0x00000001
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_ONE                                                    0x00000002
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_SRCCOLOR                                               0x00000003
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVSRCCOLOR                                            0x00000004
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_SRCALPHA                                               0x00000005
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVSRCALPHA                                            0x00000006
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_DESTALPHA                                              0x00000007
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVDESTALPHA                                           0x00000008
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_DESTCOLOR                                              0x00000009
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVDESTCOLOR                                           0x0000000A
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_SRCALPHASAT                                            0x0000000B
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_BLENDFACTOR                                            0x0000000E
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVBLENDFACTOR                                         0x0000000F
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_SRC1COLOR                                              0x00000010
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVSRC1COLOR                                           0x00000011
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_SRC1ALPHA                                              0x00000012
#define NVA197_SET_BLEND_ALPHA_DEST_COEFF_V_D3D_INVSRC1ALPHA                                           0x00000013

#define NVA197_SET_SINGLE_ROP_CONTROL                                                                      0x135c
#define NVA197_SET_SINGLE_ROP_CONTROL_ENABLE                                                                  0:0
#define NVA197_SET_SINGLE_ROP_CONTROL_ENABLE_FALSE                                                     0x00000000
#define NVA197_SET_SINGLE_ROP_CONTROL_ENABLE_TRUE                                                      0x00000001

#define NVA197_SET_BLEND(i)                                                                        (0x1360+(i)*4)
#define NVA197_SET_BLEND_ENABLE                                                                               0:0
#define NVA197_SET_BLEND_ENABLE_FALSE                                                                  0x00000000
#define NVA197_SET_BLEND_ENABLE_TRUE                                                                   0x00000001

#define NVA197_SET_STENCIL_TEST                                                                            0x1380
#define NVA197_SET_STENCIL_TEST_ENABLE                                                                        0:0
#define NVA197_SET_STENCIL_TEST_ENABLE_FALSE                                                           0x00000000
#define NVA197_SET_STENCIL_TEST_ENABLE_TRUE                                                            0x00000001

#define NVA197_SET_STENCIL_OP_FAIL                                                                         0x1384
#define NVA197_SET_STENCIL_OP_FAIL_V                                                                         31:0
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_KEEP                                                          0x00001E00
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_ZERO                                                          0x00000000
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_REPLACE                                                       0x00001E01
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_INCRSAT                                                       0x00001E02
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_DECRSAT                                                       0x00001E03
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_INVERT                                                        0x0000150A
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_INCR                                                          0x00008507
#define NVA197_SET_STENCIL_OP_FAIL_V_OGL_DECR                                                          0x00008508
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_KEEP                                                          0x00000001
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_ZERO                                                          0x00000002
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_REPLACE                                                       0x00000003
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_INCRSAT                                                       0x00000004
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_DECRSAT                                                       0x00000005
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_INVERT                                                        0x00000006
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_INCR                                                          0x00000007
#define NVA197_SET_STENCIL_OP_FAIL_V_D3D_DECR                                                          0x00000008

#define NVA197_SET_STENCIL_OP_ZFAIL                                                                        0x1388
#define NVA197_SET_STENCIL_OP_ZFAIL_V                                                                        31:0
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_KEEP                                                         0x00001E00
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_ZERO                                                         0x00000000
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_REPLACE                                                      0x00001E01
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_INCRSAT                                                      0x00001E02
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_DECRSAT                                                      0x00001E03
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_INVERT                                                       0x0000150A
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_INCR                                                         0x00008507
#define NVA197_SET_STENCIL_OP_ZFAIL_V_OGL_DECR                                                         0x00008508
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_KEEP                                                         0x00000001
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_ZERO                                                         0x00000002
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_REPLACE                                                      0x00000003
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_INCRSAT                                                      0x00000004
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_DECRSAT                                                      0x00000005
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_INVERT                                                       0x00000006
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_INCR                                                         0x00000007
#define NVA197_SET_STENCIL_OP_ZFAIL_V_D3D_DECR                                                         0x00000008

#define NVA197_SET_STENCIL_OP_ZPASS                                                                        0x138c
#define NVA197_SET_STENCIL_OP_ZPASS_V                                                                        31:0
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_KEEP                                                         0x00001E00
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_ZERO                                                         0x00000000
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_REPLACE                                                      0x00001E01
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_INCRSAT                                                      0x00001E02
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_DECRSAT                                                      0x00001E03
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_INVERT                                                       0x0000150A
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_INCR                                                         0x00008507
#define NVA197_SET_STENCIL_OP_ZPASS_V_OGL_DECR                                                         0x00008508
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_KEEP                                                         0x00000001
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_ZERO                                                         0x00000002
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_REPLACE                                                      0x00000003
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_INCRSAT                                                      0x00000004
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_DECRSAT                                                      0x00000005
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_INVERT                                                       0x00000006
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_INCR                                                         0x00000007
#define NVA197_SET_STENCIL_OP_ZPASS_V_D3D_DECR                                                         0x00000008

#define NVA197_SET_STENCIL_FUNC                                                                            0x1390
#define NVA197_SET_STENCIL_FUNC_V                                                                            31:0
#define NVA197_SET_STENCIL_FUNC_V_OGL_NEVER                                                            0x00000200
#define NVA197_SET_STENCIL_FUNC_V_OGL_LESS                                                             0x00000201
#define NVA197_SET_STENCIL_FUNC_V_OGL_EQUAL                                                            0x00000202
#define NVA197_SET_STENCIL_FUNC_V_OGL_LEQUAL                                                           0x00000203
#define NVA197_SET_STENCIL_FUNC_V_OGL_GREATER                                                          0x00000204
#define NVA197_SET_STENCIL_FUNC_V_OGL_NOTEQUAL                                                         0x00000205
#define NVA197_SET_STENCIL_FUNC_V_OGL_GEQUAL                                                           0x00000206
#define NVA197_SET_STENCIL_FUNC_V_OGL_ALWAYS                                                           0x00000207
#define NVA197_SET_STENCIL_FUNC_V_D3D_NEVER                                                            0x00000001
#define NVA197_SET_STENCIL_FUNC_V_D3D_LESS                                                             0x00000002
#define NVA197_SET_STENCIL_FUNC_V_D3D_EQUAL                                                            0x00000003
#define NVA197_SET_STENCIL_FUNC_V_D3D_LESSEQUAL                                                        0x00000004
#define NVA197_SET_STENCIL_FUNC_V_D3D_GREATER                                                          0x00000005
#define NVA197_SET_STENCIL_FUNC_V_D3D_NOTEQUAL                                                         0x00000006
#define NVA197_SET_STENCIL_FUNC_V_D3D_GREATEREQUAL                                                     0x00000007
#define NVA197_SET_STENCIL_FUNC_V_D3D_ALWAYS                                                           0x00000008

#define NVA197_SET_STENCIL_FUNC_REF                                                                        0x1394
#define NVA197_SET_STENCIL_FUNC_REF_V                                                                         7:0

#define NVA197_SET_STENCIL_FUNC_MASK                                                                       0x1398
#define NVA197_SET_STENCIL_FUNC_MASK_V                                                                        7:0

#define NVA197_SET_STENCIL_MASK                                                                            0x139c
#define NVA197_SET_STENCIL_MASK_V                                                                             7:0

#define NVA197_SET_DRAW_AUTO_START                                                                         0x13a4
#define NVA197_SET_DRAW_AUTO_START_BYTE_COUNT                                                                31:0

#define NVA197_SET_PS_SATURATE                                                                             0x13a8
#define NVA197_SET_PS_SATURATE_OUTPUT0                                                                        0:0
#define NVA197_SET_PS_SATURATE_OUTPUT0_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT0_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT1                                                                        4:4
#define NVA197_SET_PS_SATURATE_OUTPUT1_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT1_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT2                                                                        8:8
#define NVA197_SET_PS_SATURATE_OUTPUT2_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT2_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT3                                                                      12:12
#define NVA197_SET_PS_SATURATE_OUTPUT3_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT3_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT4                                                                      16:16
#define NVA197_SET_PS_SATURATE_OUTPUT4_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT4_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT5                                                                      20:20
#define NVA197_SET_PS_SATURATE_OUTPUT5_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT5_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT6                                                                      24:24
#define NVA197_SET_PS_SATURATE_OUTPUT6_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT6_TRUE                                                            0x00000001
#define NVA197_SET_PS_SATURATE_OUTPUT7                                                                      28:28
#define NVA197_SET_PS_SATURATE_OUTPUT7_FALSE                                                           0x00000000
#define NVA197_SET_PS_SATURATE_OUTPUT7_TRUE                                                            0x00000001

#define NVA197_SET_WINDOW_ORIGIN                                                                           0x13ac
#define NVA197_SET_WINDOW_ORIGIN_MODE                                                                         0:0
#define NVA197_SET_WINDOW_ORIGIN_MODE_UPPER_LEFT                                                       0x00000000
#define NVA197_SET_WINDOW_ORIGIN_MODE_LOWER_LEFT                                                       0x00000001
#define NVA197_SET_WINDOW_ORIGIN_FLIP_Y                                                                       4:4
#define NVA197_SET_WINDOW_ORIGIN_FLIP_Y_FALSE                                                          0x00000000
#define NVA197_SET_WINDOW_ORIGIN_FLIP_Y_TRUE                                                           0x00000001

#define NVA197_SET_LINE_WIDTH_FLOAT                                                                        0x13b0
#define NVA197_SET_LINE_WIDTH_FLOAT_V                                                                        31:0

#define NVA197_SET_ALIASED_LINE_WIDTH_FLOAT                                                                0x13b4
#define NVA197_SET_ALIASED_LINE_WIDTH_FLOAT_V                                                                31:0

#define NVA197_SET_LINE_MULTISAMPLE_OVERRIDE                                                               0x1418
#define NVA197_SET_LINE_MULTISAMPLE_OVERRIDE_ENABLE                                                           0:0
#define NVA197_SET_LINE_MULTISAMPLE_OVERRIDE_ENABLE_FALSE                                              0x00000000
#define NVA197_SET_LINE_MULTISAMPLE_OVERRIDE_ENABLE_TRUE                                               0x00000001

#define NVA197_SET_ALPHA_HYSTERESIS                                                                        0x1420
#define NVA197_SET_ALPHA_HYSTERESIS_ROUNDS_OF_ALPHA                                                           7:0

#define NVA197_INVALIDATE_SAMPLER_CACHE_NO_WFI                                                             0x1424
#define NVA197_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES                                                          0:0
#define NVA197_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES_ALL                                               0x00000000
#define NVA197_INVALIDATE_SAMPLER_CACHE_NO_WFI_LINES_ONE                                               0x00000001
#define NVA197_INVALIDATE_SAMPLER_CACHE_NO_WFI_TAG                                                           25:4

#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI                                                      0x1428
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES                                                   0:0
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES_ALL                                        0x00000000
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_LINES_ONE                                        0x00000001
#define NVA197_INVALIDATE_TEXTURE_HEADER_CACHE_NO_WFI_TAG                                                    25:4

#define NVA197_INVALIDATE_DA_DMA_CACHE                                                                     0x142c
#define NVA197_INVALIDATE_DA_DMA_CACHE_V                                                                      0:0

#define NVA197_SET_GLOBAL_BASE_VERTEX_INDEX                                                                0x1434
#define NVA197_SET_GLOBAL_BASE_VERTEX_INDEX_V                                                                31:0

#define NVA197_SET_GLOBAL_BASE_INSTANCE_INDEX                                                              0x1438
#define NVA197_SET_GLOBAL_BASE_INSTANCE_INDEX_V                                                              31:0

#define NVA197_SET_PS_WARP_WATERMARKS                                                                      0x1450
#define NVA197_SET_PS_WARP_WATERMARKS_LOW                                                                    15:0
#define NVA197_SET_PS_WARP_WATERMARKS_HIGH                                                                  31:16

#define NVA197_SET_PS_REGISTER_WATERMARKS                                                                  0x1454
#define NVA197_SET_PS_REGISTER_WATERMARKS_LOW                                                                15:0
#define NVA197_SET_PS_REGISTER_WATERMARKS_HIGH                                                              31:16

#define NVA197_STORE_ZCULL                                                                                 0x1464
#define NVA197_STORE_ZCULL_V                                                                                  0:0

#define NVA197_LOAD_ZCULL                                                                                  0x1500
#define NVA197_LOAD_ZCULL_V                                                                                   0:0

#define NVA197_SET_SURFACE_CLIP_ID_HEIGHT                                                                  0x1504
#define NVA197_SET_SURFACE_CLIP_ID_HEIGHT_V                                                                  31:0

#define NVA197_SET_CLIP_ID_CLEAR_RECT_HORIZONTAL                                                           0x1508
#define NVA197_SET_CLIP_ID_CLEAR_RECT_HORIZONTAL_XMIN                                                        15:0
#define NVA197_SET_CLIP_ID_CLEAR_RECT_HORIZONTAL_XMAX                                                       31:16

#define NVA197_SET_CLIP_ID_CLEAR_RECT_VERTICAL                                                             0x150c
#define NVA197_SET_CLIP_ID_CLEAR_RECT_VERTICAL_YMIN                                                          15:0
#define NVA197_SET_CLIP_ID_CLEAR_RECT_VERTICAL_YMAX                                                         31:16

#define NVA197_SET_USER_CLIP_ENABLE                                                                        0x1510
#define NVA197_SET_USER_CLIP_ENABLE_PLANE0                                                                    0:0
#define NVA197_SET_USER_CLIP_ENABLE_PLANE0_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE0_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE1                                                                    1:1
#define NVA197_SET_USER_CLIP_ENABLE_PLANE1_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE1_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE2                                                                    2:2
#define NVA197_SET_USER_CLIP_ENABLE_PLANE2_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE2_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE3                                                                    3:3
#define NVA197_SET_USER_CLIP_ENABLE_PLANE3_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE3_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE4                                                                    4:4
#define NVA197_SET_USER_CLIP_ENABLE_PLANE4_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE4_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE5                                                                    5:5
#define NVA197_SET_USER_CLIP_ENABLE_PLANE5_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE5_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE6                                                                    6:6
#define NVA197_SET_USER_CLIP_ENABLE_PLANE6_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE6_TRUE                                                        0x00000001
#define NVA197_SET_USER_CLIP_ENABLE_PLANE7                                                                    7:7
#define NVA197_SET_USER_CLIP_ENABLE_PLANE7_FALSE                                                       0x00000000
#define NVA197_SET_USER_CLIP_ENABLE_PLANE7_TRUE                                                        0x00000001

#define NVA197_SET_ZPASS_PIXEL_COUNT                                                                       0x1514
#define NVA197_SET_ZPASS_PIXEL_COUNT_ENABLE                                                                   0:0
#define NVA197_SET_ZPASS_PIXEL_COUNT_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_ZPASS_PIXEL_COUNT_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_POINT_SIZE                                                                              0x1518
#define NVA197_SET_POINT_SIZE_V                                                                              31:0

#define NVA197_SET_ZCULL_STATS                                                                             0x151c
#define NVA197_SET_ZCULL_STATS_ENABLE                                                                         0:0
#define NVA197_SET_ZCULL_STATS_ENABLE_FALSE                                                            0x00000000
#define NVA197_SET_ZCULL_STATS_ENABLE_TRUE                                                             0x00000001

#define NVA197_SET_POINT_SPRITE                                                                            0x1520
#define NVA197_SET_POINT_SPRITE_ENABLE                                                                        0:0
#define NVA197_SET_POINT_SPRITE_ENABLE_FALSE                                                           0x00000000
#define NVA197_SET_POINT_SPRITE_ENABLE_TRUE                                                            0x00000001

#define NVA197_SET_SHADER_EXCEPTIONS                                                                       0x1528
#define NVA197_SET_SHADER_EXCEPTIONS_ENABLE                                                                   0:0
#define NVA197_SET_SHADER_EXCEPTIONS_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_SHADER_EXCEPTIONS_ENABLE_TRUE                                                       0x00000001

#define NVA197_CLEAR_REPORT_VALUE                                                                          0x1530
#define NVA197_CLEAR_REPORT_VALUE_TYPE                                                                        4:0
#define NVA197_CLEAR_REPORT_VALUE_TYPE_DA_VERTICES_GENERATED                                           0x00000012
#define NVA197_CLEAR_REPORT_VALUE_TYPE_DA_PRIMITIVES_GENERATED                                         0x00000013
#define NVA197_CLEAR_REPORT_VALUE_TYPE_VS_INVOCATIONS                                                  0x00000015
#define NVA197_CLEAR_REPORT_VALUE_TYPE_TI_INVOCATIONS                                                  0x00000016
#define NVA197_CLEAR_REPORT_VALUE_TYPE_TS_INVOCATIONS                                                  0x00000017
#define NVA197_CLEAR_REPORT_VALUE_TYPE_TS_PRIMITIVES_GENERATED                                         0x00000018
#define NVA197_CLEAR_REPORT_VALUE_TYPE_GS_INVOCATIONS                                                  0x0000001A
#define NVA197_CLEAR_REPORT_VALUE_TYPE_GS_PRIMITIVES_GENERATED                                         0x0000001B
#define NVA197_CLEAR_REPORT_VALUE_TYPE_VTG_PRIMITIVES_OUT                                              0x0000001F
#define NVA197_CLEAR_REPORT_VALUE_TYPE_STREAMING_PRIMITIVES_SUCCEEDED                                  0x00000010
#define NVA197_CLEAR_REPORT_VALUE_TYPE_STREAMING_PRIMITIVES_NEEDED                                     0x00000011
#define NVA197_CLEAR_REPORT_VALUE_TYPE_TOTAL_STREAMING_PRIMITIVES_NEEDED_MINUS_SUCCEEDED               0x00000003
#define NVA197_CLEAR_REPORT_VALUE_TYPE_CLIPPER_INVOCATIONS                                             0x0000001C
#define NVA197_CLEAR_REPORT_VALUE_TYPE_CLIPPER_PRIMITIVES_GENERATED                                    0x0000001D
#define NVA197_CLEAR_REPORT_VALUE_TYPE_ZCULL_STATS                                                     0x00000002
#define NVA197_CLEAR_REPORT_VALUE_TYPE_PS_INVOCATIONS                                                  0x0000001E
#define NVA197_CLEAR_REPORT_VALUE_TYPE_ZPASS_PIXEL_CNT                                                 0x00000001
#define NVA197_CLEAR_REPORT_VALUE_TYPE_ALPHA_BETA_CLOCKS                                               0x00000004

#define NVA197_SET_ANTI_ALIAS_ENABLE                                                                       0x1534
#define NVA197_SET_ANTI_ALIAS_ENABLE_V                                                                        0:0
#define NVA197_SET_ANTI_ALIAS_ENABLE_V_FALSE                                                           0x00000000
#define NVA197_SET_ANTI_ALIAS_ENABLE_V_TRUE                                                            0x00000001

#define NVA197_SET_ZT_SELECT                                                                               0x1538
#define NVA197_SET_ZT_SELECT_TARGET_COUNT                                                                     0:0

#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL                                                                0x153c
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_COVERAGE                                                 0:0
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_COVERAGE_DISABLE                                  0x00000000
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_COVERAGE_ENABLE                                   0x00000001
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_ONE                                                      4:4
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_ONE_DISABLE                                       0x00000000
#define NVA197_SET_ANTI_ALIAS_ALPHA_CONTROL_ALPHA_TO_ONE_ENABLE                                        0x00000001

#define NVA197_SET_RENDER_ENABLE_A                                                                         0x1550
#define NVA197_SET_RENDER_ENABLE_A_OFFSET_UPPER                                                               7:0

#define NVA197_SET_RENDER_ENABLE_B                                                                         0x1554
#define NVA197_SET_RENDER_ENABLE_B_OFFSET_LOWER                                                              31:0

#define NVA197_SET_RENDER_ENABLE_C                                                                         0x1558
#define NVA197_SET_RENDER_ENABLE_C_MODE                                                                       2:0
#define NVA197_SET_RENDER_ENABLE_C_MODE_FALSE                                                          0x00000000
#define NVA197_SET_RENDER_ENABLE_C_MODE_TRUE                                                           0x00000001
#define NVA197_SET_RENDER_ENABLE_C_MODE_CONDITIONAL                                                    0x00000002
#define NVA197_SET_RENDER_ENABLE_C_MODE_RENDER_IF_EQUAL                                                0x00000003
#define NVA197_SET_RENDER_ENABLE_C_MODE_RENDER_IF_NOT_EQUAL                                            0x00000004

#define NVA197_SET_TEX_SAMPLER_POOL_A                                                                      0x155c
#define NVA197_SET_TEX_SAMPLER_POOL_A_OFFSET_UPPER                                                            7:0

#define NVA197_SET_TEX_SAMPLER_POOL_B                                                                      0x1560
#define NVA197_SET_TEX_SAMPLER_POOL_B_OFFSET_LOWER                                                           31:0

#define NVA197_SET_TEX_SAMPLER_POOL_C                                                                      0x1564
#define NVA197_SET_TEX_SAMPLER_POOL_C_MAXIMUM_INDEX                                                          19:0

#define NVA197_SET_SLOPE_SCALE_DEPTH_BIAS                                                                  0x156c
#define NVA197_SET_SLOPE_SCALE_DEPTH_BIAS_V                                                                  31:0

#define NVA197_SET_ANTI_ALIASED_LINE                                                                       0x1570
#define NVA197_SET_ANTI_ALIASED_LINE_ENABLE                                                                   0:0
#define NVA197_SET_ANTI_ALIASED_LINE_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_ANTI_ALIASED_LINE_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_TEX_HEADER_POOL_A                                                                       0x1574
#define NVA197_SET_TEX_HEADER_POOL_A_OFFSET_UPPER                                                             7:0

#define NVA197_SET_TEX_HEADER_POOL_B                                                                       0x1578
#define NVA197_SET_TEX_HEADER_POOL_B_OFFSET_LOWER                                                            31:0

#define NVA197_SET_TEX_HEADER_POOL_C                                                                       0x157c
#define NVA197_SET_TEX_HEADER_POOL_C_MAXIMUM_INDEX                                                           21:0

#define NVA197_SET_ACTIVE_ZCULL_REGION                                                                     0x1590
#define NVA197_SET_ACTIVE_ZCULL_REGION_ID                                                                     5:0

#define NVA197_SET_TWO_SIDED_STENCIL_TEST                                                                  0x1594
#define NVA197_SET_TWO_SIDED_STENCIL_TEST_ENABLE                                                              0:0
#define NVA197_SET_TWO_SIDED_STENCIL_TEST_ENABLE_FALSE                                                 0x00000000
#define NVA197_SET_TWO_SIDED_STENCIL_TEST_ENABLE_TRUE                                                  0x00000001

#define NVA197_SET_BACK_STENCIL_OP_FAIL                                                                    0x1598
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V                                                                    31:0
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_KEEP                                                     0x00001E00
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_ZERO                                                     0x00000000
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_REPLACE                                                  0x00001E01
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_INCRSAT                                                  0x00001E02
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_DECRSAT                                                  0x00001E03
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_INVERT                                                   0x0000150A
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_INCR                                                     0x00008507
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_OGL_DECR                                                     0x00008508
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_KEEP                                                     0x00000001
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_ZERO                                                     0x00000002
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_REPLACE                                                  0x00000003
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_INCRSAT                                                  0x00000004
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_DECRSAT                                                  0x00000005
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_INVERT                                                   0x00000006
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_INCR                                                     0x00000007
#define NVA197_SET_BACK_STENCIL_OP_FAIL_V_D3D_DECR                                                     0x00000008

#define NVA197_SET_BACK_STENCIL_OP_ZFAIL                                                                   0x159c
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V                                                                   31:0
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_KEEP                                                    0x00001E00
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_ZERO                                                    0x00000000
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_REPLACE                                                 0x00001E01
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_INCRSAT                                                 0x00001E02
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_DECRSAT                                                 0x00001E03
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_INVERT                                                  0x0000150A
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_INCR                                                    0x00008507
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_OGL_DECR                                                    0x00008508
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_KEEP                                                    0x00000001
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_ZERO                                                    0x00000002
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_REPLACE                                                 0x00000003
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_INCRSAT                                                 0x00000004
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_DECRSAT                                                 0x00000005
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_INVERT                                                  0x00000006
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_INCR                                                    0x00000007
#define NVA197_SET_BACK_STENCIL_OP_ZFAIL_V_D3D_DECR                                                    0x00000008

#define NVA197_SET_BACK_STENCIL_OP_ZPASS                                                                   0x15a0
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V                                                                   31:0
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_KEEP                                                    0x00001E00
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_ZERO                                                    0x00000000
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_REPLACE                                                 0x00001E01
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_INCRSAT                                                 0x00001E02
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_DECRSAT                                                 0x00001E03
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_INVERT                                                  0x0000150A
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_INCR                                                    0x00008507
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_OGL_DECR                                                    0x00008508
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_KEEP                                                    0x00000001
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_ZERO                                                    0x00000002
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_REPLACE                                                 0x00000003
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_INCRSAT                                                 0x00000004
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_DECRSAT                                                 0x00000005
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_INVERT                                                  0x00000006
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_INCR                                                    0x00000007
#define NVA197_SET_BACK_STENCIL_OP_ZPASS_V_D3D_DECR                                                    0x00000008

#define NVA197_SET_BACK_STENCIL_FUNC                                                                       0x15a4
#define NVA197_SET_BACK_STENCIL_FUNC_V                                                                       31:0
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_NEVER                                                       0x00000200
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_LESS                                                        0x00000201
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_EQUAL                                                       0x00000202
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_LEQUAL                                                      0x00000203
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_GREATER                                                     0x00000204
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_NOTEQUAL                                                    0x00000205
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_GEQUAL                                                      0x00000206
#define NVA197_SET_BACK_STENCIL_FUNC_V_OGL_ALWAYS                                                      0x00000207
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_NEVER                                                       0x00000001
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_LESS                                                        0x00000002
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_EQUAL                                                       0x00000003
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_LESSEQUAL                                                   0x00000004
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_GREATER                                                     0x00000005
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_NOTEQUAL                                                    0x00000006
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_GREATEREQUAL                                                0x00000007
#define NVA197_SET_BACK_STENCIL_FUNC_V_D3D_ALWAYS                                                      0x00000008

#define NVA197_SET_SRGB_WRITE                                                                              0x15b8
#define NVA197_SET_SRGB_WRITE_ENABLE                                                                          0:0
#define NVA197_SET_SRGB_WRITE_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_SRGB_WRITE_ENABLE_TRUE                                                              0x00000001

#define NVA197_SET_DEPTH_BIAS                                                                              0x15bc
#define NVA197_SET_DEPTH_BIAS_V                                                                              31:0

#define NVA197_SET_ZCULL_REGION_FORMAT                                                                     0x15c8
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE                                                                   3:0
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_4X4                                                      0x00000000
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_ZS_4X4                                                     0x00000001
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_4X2                                                      0x00000002
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_2X4                                                      0x00000003
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_16X8_4X4                                                 0x00000004
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_8X8_4X2                                                  0x00000005
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_8X8_2X4                                                  0x00000006
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_16X16_4X8                                                0x00000007
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_4X8_2X2                                                  0x00000008
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_ZS_16X8_4X2                                                0x00000009
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_ZS_16X8_2X4                                                0x0000000A
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_ZS_8X8_2X2                                                 0x0000000B
#define NVA197_SET_ZCULL_REGION_FORMAT_TYPE_Z_4X8_1X1                                                  0x0000000C

#define NVA197_SET_RT_LAYER                                                                                0x15cc
#define NVA197_SET_RT_LAYER_V                                                                                15:0
#define NVA197_SET_RT_LAYER_CONTROL                                                                         16:16
#define NVA197_SET_RT_LAYER_CONTROL_V_SELECTS_LAYER                                                    0x00000000
#define NVA197_SET_RT_LAYER_CONTROL_GEOMETRY_SHADER_SELECTS_LAYER                                      0x00000001

#define NVA197_SET_ANTI_ALIAS                                                                              0x15d0
#define NVA197_SET_ANTI_ALIAS_SAMPLES                                                                         3:0
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_1X1                                                         0x00000000
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_2X1                                                         0x00000001
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_2X2                                                         0x00000002
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_4X2                                                         0x00000003
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_4X2_D3D                                                     0x00000004
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_2X1_D3D                                                     0x00000005
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_4X4                                                         0x00000006
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_2X2_VC_4                                                    0x00000008
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_2X2_VC_12                                                   0x00000009
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_4X2_VC_8                                                    0x0000000A
#define NVA197_SET_ANTI_ALIAS_SAMPLES_MODE_4X2_VC_24                                                   0x0000000B

#define NVA197_SET_EDGE_FLAG                                                                               0x15e4
#define NVA197_SET_EDGE_FLAG_V                                                                                0:0
#define NVA197_SET_EDGE_FLAG_V_FALSE                                                                   0x00000000
#define NVA197_SET_EDGE_FLAG_V_TRUE                                                                    0x00000001

#define NVA197_DRAW_INLINE_INDEX                                                                           0x15e8
#define NVA197_DRAW_INLINE_INDEX_V                                                                           31:0

#define NVA197_SET_INLINE_INDEX2X16_ALIGN                                                                  0x15ec
#define NVA197_SET_INLINE_INDEX2X16_ALIGN_COUNT                                                              30:0
#define NVA197_SET_INLINE_INDEX2X16_ALIGN_START_ODD                                                         31:31
#define NVA197_SET_INLINE_INDEX2X16_ALIGN_START_ODD_FALSE                                              0x00000000
#define NVA197_SET_INLINE_INDEX2X16_ALIGN_START_ODD_TRUE                                               0x00000001

#define NVA197_DRAW_INLINE_INDEX2X16                                                                       0x15f0
#define NVA197_DRAW_INLINE_INDEX2X16_EVEN                                                                    15:0
#define NVA197_DRAW_INLINE_INDEX2X16_ODD                                                                    31:16

#define NVA197_SET_VERTEX_GLOBAL_BASE_OFFSET_A                                                             0x15f4
#define NVA197_SET_VERTEX_GLOBAL_BASE_OFFSET_A_UPPER                                                          7:0

#define NVA197_SET_VERTEX_GLOBAL_BASE_OFFSET_B                                                             0x15f8
#define NVA197_SET_VERTEX_GLOBAL_BASE_OFFSET_B_LOWER                                                         31:0

#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_A                                                             0x15fc
#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_A_WIDTH                                                         15:0

#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_B                                                             0x1600
#define NVA197_SET_ZCULL_REGION_PIXEL_OFFSET_B_HEIGHT                                                        15:0

#define NVA197_SET_POINT_SPRITE_SELECT                                                                     0x1604
#define NVA197_SET_POINT_SPRITE_SELECT_RMODE                                                                  1:0
#define NVA197_SET_POINT_SPRITE_SELECT_RMODE_ZERO                                                      0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_RMODE_FROM_R                                                    0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_RMODE_FROM_S                                                    0x00000002
#define NVA197_SET_POINT_SPRITE_SELECT_ORIGIN                                                                 2:2
#define NVA197_SET_POINT_SPRITE_SELECT_ORIGIN_BOTTOM                                                   0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_ORIGIN_TOP                                                      0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE0                                                               3:3
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE0_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE0_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE1                                                               4:4
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE1_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE1_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE2                                                               5:5
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE2_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE2_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE3                                                               6:6
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE3_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE3_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE4                                                               7:7
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE4_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE4_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE5                                                               8:8
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE5_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE5_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE6                                                               9:9
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE6_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE6_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE7                                                             10:10
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE7_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE7_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE8                                                             11:11
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE8_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE8_GENERATE                                               0x00000001
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE9                                                             12:12
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE9_PASSTHROUGH                                            0x00000000
#define NVA197_SET_POINT_SPRITE_SELECT_TEXTURE9_GENERATE                                               0x00000001

#define NVA197_SET_PROGRAM_REGION_A                                                                        0x1608
#define NVA197_SET_PROGRAM_REGION_A_ADDRESS_UPPER                                                             7:0

#define NVA197_SET_PROGRAM_REGION_B                                                                        0x160c
#define NVA197_SET_PROGRAM_REGION_B_ADDRESS_LOWER                                                            31:0

#define NVA197_SET_ATTRIBUTE_DEFAULT                                                                       0x1610
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_DIFFUSE                                                      0:0
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_DIFFUSE_VECTOR_0001                                   0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_DIFFUSE_VECTOR_1111                                   0x00000001
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_SPECULAR                                                     1:1
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_SPECULAR_VECTOR_0000                                  0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_COLOR_FRONT_SPECULAR_VECTOR_0001                                  0x00000001
#define NVA197_SET_ATTRIBUTE_DEFAULT_GENERIC_VECTOR                                                           2:2
#define NVA197_SET_ATTRIBUTE_DEFAULT_GENERIC_VECTOR_VECTOR_0000                                        0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_GENERIC_VECTOR_VECTOR_0001                                        0x00000001
#define NVA197_SET_ATTRIBUTE_DEFAULT_FIXED_FNC_TEXTURE                                                        3:3
#define NVA197_SET_ATTRIBUTE_DEFAULT_FIXED_FNC_TEXTURE_VECTOR_0000                                     0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_FIXED_FNC_TEXTURE_VECTOR_0001                                     0x00000001
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR0                                                               4:4
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR0_VECTOR_0001                                            0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR0_VECTOR_1111                                            0x00000001
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR1_TO_COLOR15                                                    5:5
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR1_TO_COLOR15_VECTOR_0000                                 0x00000000
#define NVA197_SET_ATTRIBUTE_DEFAULT_DX9_COLOR1_TO_COLOR15_VECTOR_0001                                 0x00000001

#define NVA197_END                                                                                         0x1614
#define NVA197_END_V                                                                                          0:0

#define NVA197_BEGIN                                                                                       0x1618
#define NVA197_BEGIN_OP                                                                                      15:0
#define NVA197_BEGIN_OP_POINTS                                                                         0x00000000
#define NVA197_BEGIN_OP_LINES                                                                          0x00000001
#define NVA197_BEGIN_OP_LINE_LOOP                                                                      0x00000002
#define NVA197_BEGIN_OP_LINE_STRIP                                                                     0x00000003
#define NVA197_BEGIN_OP_TRIANGLES                                                                      0x00000004
#define NVA197_BEGIN_OP_TRIANGLE_STRIP                                                                 0x00000005
#define NVA197_BEGIN_OP_TRIANGLE_FAN                                                                   0x00000006
#define NVA197_BEGIN_OP_QUADS                                                                          0x00000007
#define NVA197_BEGIN_OP_QUAD_STRIP                                                                     0x00000008
#define NVA197_BEGIN_OP_POLYGON                                                                        0x00000009
#define NVA197_BEGIN_OP_LINELIST_ADJCY                                                                 0x0000000A
#define NVA197_BEGIN_OP_LINESTRIP_ADJCY                                                                0x0000000B
#define NVA197_BEGIN_OP_TRIANGLELIST_ADJCY                                                             0x0000000C
#define NVA197_BEGIN_OP_TRIANGLESTRIP_ADJCY                                                            0x0000000D
#define NVA197_BEGIN_OP_PATCH                                                                          0x0000000E
#define NVA197_BEGIN_PRIMITIVE_ID                                                                           24:24
#define NVA197_BEGIN_PRIMITIVE_ID_FIRST                                                                0x00000000
#define NVA197_BEGIN_PRIMITIVE_ID_UNCHANGED                                                            0x00000001
#define NVA197_BEGIN_INSTANCE_ID                                                                            27:26
#define NVA197_BEGIN_INSTANCE_ID_FIRST                                                                 0x00000000
#define NVA197_BEGIN_INSTANCE_ID_SUBSEQUENT                                                            0x00000001
#define NVA197_BEGIN_INSTANCE_ID_UNCHANGED                                                             0x00000002
#define NVA197_BEGIN_SPLIT_MODE                                                                             30:29
#define NVA197_BEGIN_SPLIT_MODE_NORMAL_BEGIN_NORMAL_END                                                0x00000000
#define NVA197_BEGIN_SPLIT_MODE_NORMAL_BEGIN_OPEN_END                                                  0x00000001
#define NVA197_BEGIN_SPLIT_MODE_OPEN_BEGIN_OPEN_END                                                    0x00000002
#define NVA197_BEGIN_SPLIT_MODE_OPEN_BEGIN_NORMAL_END                                                  0x00000003

#define NVA197_SET_VERTEX_ID_COPY                                                                          0x161c
#define NVA197_SET_VERTEX_ID_COPY_ENABLE                                                                      0:0
#define NVA197_SET_VERTEX_ID_COPY_ENABLE_FALSE                                                         0x00000000
#define NVA197_SET_VERTEX_ID_COPY_ENABLE_TRUE                                                          0x00000001
#define NVA197_SET_VERTEX_ID_COPY_ATTRIBUTE_SLOT                                                             11:4

#define NVA197_ADD_TO_PRIMITIVE_ID                                                                         0x1620
#define NVA197_ADD_TO_PRIMITIVE_ID_V                                                                         31:0

#define NVA197_LOAD_PRIMITIVE_ID                                                                           0x1624
#define NVA197_LOAD_PRIMITIVE_ID_V                                                                           31:0

#define NVA197_SET_SHADER_BASED_CULL                                                                       0x162c
#define NVA197_SET_SHADER_BASED_CULL_BATCH_CULL_ENABLE                                                        1:1
#define NVA197_SET_SHADER_BASED_CULL_BATCH_CULL_ENABLE_FALSE                                           0x00000000
#define NVA197_SET_SHADER_BASED_CULL_BATCH_CULL_ENABLE_TRUE                                            0x00000001
#define NVA197_SET_SHADER_BASED_CULL_BEFORE_FETCH_ENABLE                                                      0:0
#define NVA197_SET_SHADER_BASED_CULL_BEFORE_FETCH_ENABLE_FALSE                                         0x00000000
#define NVA197_SET_SHADER_BASED_CULL_BEFORE_FETCH_ENABLE_TRUE                                          0x00000001

#define NVA197_SET_CLASS_VERSION                                                                           0x1638
#define NVA197_SET_CLASS_VERSION_CURRENT                                                                     15:0
#define NVA197_SET_CLASS_VERSION_OLDEST_SUPPORTED                                                           31:16

#define NVA197_SET_VAB_PAGE                                                                                0x163c
#define NVA197_SET_VAB_PAGE_READ_SELECT                                                                       0:0
#define NVA197_SET_VAB_PAGE_READ_SELECT_PAGES_0_AND_1                                                  0x00000000
#define NVA197_SET_VAB_PAGE_READ_SELECT_PAGES_0_AND_2                                                  0x00000001

#define NVA197_DRAW_INLINE_VERTEX                                                                          0x1640
#define NVA197_DRAW_INLINE_VERTEX_V                                                                          31:0

#define NVA197_SET_DA_PRIMITIVE_RESTART                                                                    0x1644
#define NVA197_SET_DA_PRIMITIVE_RESTART_ENABLE                                                                0:0
#define NVA197_SET_DA_PRIMITIVE_RESTART_ENABLE_FALSE                                                   0x00000000
#define NVA197_SET_DA_PRIMITIVE_RESTART_ENABLE_TRUE                                                    0x00000001

#define NVA197_SET_DA_PRIMITIVE_RESTART_INDEX                                                              0x1648
#define NVA197_SET_DA_PRIMITIVE_RESTART_INDEX_V                                                              31:0

#define NVA197_SET_DA_OUTPUT                                                                               0x164c
#define NVA197_SET_DA_OUTPUT_VERTEX_ID_USES_ARRAY_START                                                     12:12
#define NVA197_SET_DA_OUTPUT_VERTEX_ID_USES_ARRAY_START_FALSE                                          0x00000000
#define NVA197_SET_DA_OUTPUT_VERTEX_ID_USES_ARRAY_START_TRUE                                           0x00000001

#define NVA197_SET_ANTI_ALIASED_POINT                                                                      0x1658
#define NVA197_SET_ANTI_ALIASED_POINT_ENABLE                                                                  0:0
#define NVA197_SET_ANTI_ALIASED_POINT_ENABLE_FALSE                                                     0x00000000
#define NVA197_SET_ANTI_ALIASED_POINT_ENABLE_TRUE                                                      0x00000001

#define NVA197_SET_POINT_CENTER_MODE                                                                       0x165c
#define NVA197_SET_POINT_CENTER_MODE_V                                                                       31:0
#define NVA197_SET_POINT_CENTER_MODE_V_OGL                                                             0x00000000
#define NVA197_SET_POINT_CENTER_MODE_V_D3D                                                             0x00000001

#define NVA197_SET_LINE_SMOOTH_PARAMETERS                                                                  0x1668
#define NVA197_SET_LINE_SMOOTH_PARAMETERS_FALLOFF                                                            31:0
#define NVA197_SET_LINE_SMOOTH_PARAMETERS_FALLOFF__1_00                                                0x00000000
#define NVA197_SET_LINE_SMOOTH_PARAMETERS_FALLOFF__1_33                                                0x00000001
#define NVA197_SET_LINE_SMOOTH_PARAMETERS_FALLOFF__1_60                                                0x00000002

#define NVA197_SET_LINE_STIPPLE                                                                            0x166c
#define NVA197_SET_LINE_STIPPLE_ENABLE                                                                        0:0
#define NVA197_SET_LINE_STIPPLE_ENABLE_FALSE                                                           0x00000000
#define NVA197_SET_LINE_STIPPLE_ENABLE_TRUE                                                            0x00000001

#define NVA197_SET_LINE_SMOOTH_EDGE_TABLE(i)                                                       (0x1670+(i)*4)
#define NVA197_SET_LINE_SMOOTH_EDGE_TABLE_V0                                                                  7:0
#define NVA197_SET_LINE_SMOOTH_EDGE_TABLE_V1                                                                 15:8
#define NVA197_SET_LINE_SMOOTH_EDGE_TABLE_V2                                                                23:16
#define NVA197_SET_LINE_SMOOTH_EDGE_TABLE_V3                                                                31:24

#define NVA197_SET_LINE_STIPPLE_PARAMETERS                                                                 0x1680
#define NVA197_SET_LINE_STIPPLE_PARAMETERS_FACTOR                                                             7:0
#define NVA197_SET_LINE_STIPPLE_PARAMETERS_PATTERN                                                           23:8

#define NVA197_SET_PROVOKING_VERTEX                                                                        0x1684
#define NVA197_SET_PROVOKING_VERTEX_V                                                                         0:0
#define NVA197_SET_PROVOKING_VERTEX_V_FIRST                                                            0x00000000
#define NVA197_SET_PROVOKING_VERTEX_V_LAST                                                             0x00000001

#define NVA197_SET_TWO_SIDED_LIGHT                                                                         0x1688
#define NVA197_SET_TWO_SIDED_LIGHT_ENABLE                                                                     0:0
#define NVA197_SET_TWO_SIDED_LIGHT_ENABLE_FALSE                                                        0x00000000
#define NVA197_SET_TWO_SIDED_LIGHT_ENABLE_TRUE                                                         0x00000001

#define NVA197_SET_POLYGON_STIPPLE                                                                         0x168c
#define NVA197_SET_POLYGON_STIPPLE_ENABLE                                                                     0:0
#define NVA197_SET_POLYGON_STIPPLE_ENABLE_FALSE                                                        0x00000000
#define NVA197_SET_POLYGON_STIPPLE_ENABLE_TRUE                                                         0x00000001

#define NVA197_SET_SHADER_CONTROL                                                                          0x1690
#define NVA197_SET_SHADER_CONTROL_DEFAULT_PARTIAL                                                             0:0
#define NVA197_SET_SHADER_CONTROL_DEFAULT_PARTIAL_ZERO                                                 0x00000000
#define NVA197_SET_SHADER_CONTROL_DEFAULT_PARTIAL_INFINITY                                             0x00000001
#define NVA197_SET_SHADER_CONTROL_FP32_NAN_BEHAVIOR                                                           1:1
#define NVA197_SET_SHADER_CONTROL_FP32_NAN_BEHAVIOR_LEGACY                                             0x00000000
#define NVA197_SET_SHADER_CONTROL_FP32_NAN_BEHAVIOR_FP64_COMPATIBLE                                    0x00000001
#define NVA197_SET_SHADER_CONTROL_FP32_F2I_NAN_BEHAVIOR                                                       2:2
#define NVA197_SET_SHADER_CONTROL_FP32_F2I_NAN_BEHAVIOR_PASS_ZERO                                      0x00000000
#define NVA197_SET_SHADER_CONTROL_FP32_F2I_NAN_BEHAVIOR_PASS_INDEFINITE                                0x00000001

#define NVA197_LAUNCH_VERTEX                                                                               0x169c
#define NVA197_LAUNCH_VERTEX_V                                                                                0:0

#define NVA197_CHECK_CLASS_VERSION                                                                         0x16a0
#define NVA197_CHECK_CLASS_VERSION_CURRENT                                                                   15:0
#define NVA197_CHECK_CLASS_VERSION_OLDEST_SUPPORTED                                                         31:16

#define NVA197_SET_SPH_VERSION                                                                             0x16a4
#define NVA197_SET_SPH_VERSION_CURRENT                                                                       15:0
#define NVA197_SET_SPH_VERSION_OLDEST_SUPPORTED                                                             31:16

#define NVA197_CHECK_SPH_VERSION                                                                           0x16a8
#define NVA197_CHECK_SPH_VERSION_CURRENT                                                                     15:0
#define NVA197_CHECK_SPH_VERSION_OLDEST_SUPPORTED                                                           31:16

#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE                                                              0x16b4
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_ANTI_ALIAS_ENABLE                                    0:0
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_ANTI_ALIAS_ENABLE_DISABLE                     0x00000000
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_ANTI_ALIAS_ENABLE_ENABLE                      0x00000001
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_PS_SAMPLE_MASK_OUTPUT                                1:1
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_PS_SAMPLE_MASK_OUTPUT_DISABLE                 0x00000000
#define NVA197_SET_ALPHA_TO_COVERAGE_OVERRIDE_QUALIFY_BY_PS_SAMPLE_MASK_OUTPUT_ENABLE                  0x00000001

#define NVA197_SET_POLYGON_STIPPLE_PATTERN(i)                                                      (0x1700+(i)*4)
#define NVA197_SET_POLYGON_STIPPLE_PATTERN_V                                                                 31:0

#define NVA197_SET_AAM_VERSION                                                                             0x1790
#define NVA197_SET_AAM_VERSION_CURRENT                                                                       15:0
#define NVA197_SET_AAM_VERSION_OLDEST_SUPPORTED                                                             31:16

#define NVA197_CHECK_AAM_VERSION                                                                           0x1794
#define NVA197_CHECK_AAM_VERSION_CURRENT                                                                     15:0
#define NVA197_CHECK_AAM_VERSION_OLDEST_SUPPORTED                                                           31:16

#define NVA197_SET_ZT_LAYER                                                                                0x179c
#define NVA197_SET_ZT_LAYER_OFFSET                                                                           15:0

#define NVA197_SET_VAB_MEMORY_AREA_A                                                                       0x17bc
#define NVA197_SET_VAB_MEMORY_AREA_A_OFFSET_UPPER                                                             7:0

#define NVA197_SET_VAB_MEMORY_AREA_B                                                                       0x17c0
#define NVA197_SET_VAB_MEMORY_AREA_B_OFFSET_LOWER                                                            31:0

#define NVA197_SET_VAB_MEMORY_AREA_C                                                                       0x17c4
#define NVA197_SET_VAB_MEMORY_AREA_C_SIZE                                                                     1:0
#define NVA197_SET_VAB_MEMORY_AREA_C_SIZE_BYTES_64K                                                    0x00000001
#define NVA197_SET_VAB_MEMORY_AREA_C_SIZE_BYTES_128K                                                   0x00000002
#define NVA197_SET_VAB_MEMORY_AREA_C_SIZE_BYTES_256K                                                   0x00000003

#define NVA197_SET_INDEX_BUFFER_A                                                                          0x17c8
#define NVA197_SET_INDEX_BUFFER_A_ADDRESS_UPPER                                                               7:0

#define NVA197_SET_INDEX_BUFFER_B                                                                          0x17cc
#define NVA197_SET_INDEX_BUFFER_B_ADDRESS_LOWER                                                              31:0

#define NVA197_SET_INDEX_BUFFER_C                                                                          0x17d0
#define NVA197_SET_INDEX_BUFFER_C_LIMIT_ADDRESS_UPPER                                                         7:0

#define NVA197_SET_INDEX_BUFFER_D                                                                          0x17d4
#define NVA197_SET_INDEX_BUFFER_D_LIMIT_ADDRESS_LOWER                                                        31:0

#define NVA197_SET_INDEX_BUFFER_E                                                                          0x17d8
#define NVA197_SET_INDEX_BUFFER_E_INDEX_SIZE                                                                  1:0
#define NVA197_SET_INDEX_BUFFER_E_INDEX_SIZE_ONE_BYTE                                                  0x00000000
#define NVA197_SET_INDEX_BUFFER_E_INDEX_SIZE_TWO_BYTES                                                 0x00000001
#define NVA197_SET_INDEX_BUFFER_E_INDEX_SIZE_FOUR_BYTES                                                0x00000002

#define NVA197_SET_INDEX_BUFFER_F                                                                          0x17dc
#define NVA197_SET_INDEX_BUFFER_F_FIRST                                                                      31:0

#define NVA197_DRAW_INDEX_BUFFER                                                                           0x17e0
#define NVA197_DRAW_INDEX_BUFFER_COUNT                                                                       31:0

#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST                                                0x17e4
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_FIRST                                            15:0
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_COUNT                                           27:16
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY                                        31:28
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POINTS                            0x00000000
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINES                             0x00000001
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_LOOP                         0x00000002
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_STRIP                        0x00000003
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLES                         0x00000004
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_STRIP                    0x00000005
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_FAN                      0x00000006
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUADS                             0x00000007
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUAD_STRIP                        0x00000008
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POLYGON                           0x00000009
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINELIST_ADJCY                    0x0000000A
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINESTRIP_ADJCY                   0x0000000B
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLELIST_ADJCY                0x0000000C
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLESTRIP_ADJCY               0x0000000D
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_PATCH                             0x0000000E

#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST                                                0x17e8
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_FIRST                                            15:0
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_COUNT                                           27:16
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY                                        31:28
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POINTS                            0x00000000
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINES                             0x00000001
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_LOOP                         0x00000002
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_STRIP                        0x00000003
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLES                         0x00000004
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_STRIP                    0x00000005
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_FAN                      0x00000006
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUADS                             0x00000007
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUAD_STRIP                        0x00000008
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POLYGON                           0x00000009
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINELIST_ADJCY                    0x0000000A
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINESTRIP_ADJCY                   0x0000000B
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLELIST_ADJCY                0x0000000C
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLESTRIP_ADJCY               0x0000000D
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_PATCH                             0x0000000E

#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST                                                 0x17ec
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_FIRST                                             15:0
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_COUNT                                            27:16
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY                                         31:28
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POINTS                             0x00000000
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINES                              0x00000001
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_LOOP                          0x00000002
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINE_STRIP                         0x00000003
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLES                          0x00000004
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_STRIP                     0x00000005
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLE_FAN                       0x00000006
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUADS                              0x00000007
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_QUAD_STRIP                         0x00000008
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_POLYGON                            0x00000009
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINELIST_ADJCY                     0x0000000A
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_LINESTRIP_ADJCY                    0x0000000B
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLELIST_ADJCY                 0x0000000C
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_TRIANGLESTRIP_ADJCY                0x0000000D
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_FIRST_TOPOLOGY_PATCH                              0x0000000E

#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT                                           0x17f0
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_FIRST                                       15:0
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_COUNT                                      27:16
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY                                   31:28
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POINTS                       0x00000000
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINES                        0x00000001
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_LOOP                    0x00000002
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_STRIP                   0x00000003
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLES                    0x00000004
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_STRIP               0x00000005
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_FAN                 0x00000006
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUADS                        0x00000007
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUAD_STRIP                   0x00000008
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POLYGON                      0x00000009
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINELIST_ADJCY               0x0000000A
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINESTRIP_ADJCY              0x0000000B
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLELIST_ADJCY             0x0000000C
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLESTRIP_ADJCY             0x0000000D
#define NVA197_DRAW_INDEX_BUFFER32_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_PATCH                        0x0000000E

#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT                                           0x17f4
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_FIRST                                       15:0
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_COUNT                                      27:16
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY                                   31:28
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POINTS                       0x00000000
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINES                        0x00000001
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_LOOP                    0x00000002
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_STRIP                   0x00000003
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLES                    0x00000004
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_STRIP               0x00000005
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_FAN                 0x00000006
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUADS                        0x00000007
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUAD_STRIP                   0x00000008
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POLYGON                      0x00000009
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINELIST_ADJCY               0x0000000A
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINESTRIP_ADJCY              0x0000000B
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLELIST_ADJCY             0x0000000C
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLESTRIP_ADJCY             0x0000000D
#define NVA197_DRAW_INDEX_BUFFER16_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_PATCH                        0x0000000E

#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT                                            0x17f8
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_FIRST                                        15:0
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_COUNT                                       27:16
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY                                    31:28
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POINTS                        0x00000000
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINES                         0x00000001
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_LOOP                     0x00000002
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINE_STRIP                    0x00000003
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLES                     0x00000004
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_STRIP                0x00000005
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLE_FAN                  0x00000006
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUADS                         0x00000007
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_QUAD_STRIP                    0x00000008
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_POLYGON                       0x00000009
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINELIST_ADJCY                0x0000000A
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_LINESTRIP_ADJCY               0x0000000B
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLELIST_ADJCY             0x0000000C
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_TRIANGLESTRIP_ADJCY             0x0000000D
#define NVA197_DRAW_INDEX_BUFFER8_BEGIN_END_INSTANCE_SUBSEQUENT_TOPOLOGY_PATCH                         0x0000000E

#define NVA197_SET_DEPTH_BIAS_CLAMP                                                                        0x187c
#define NVA197_SET_DEPTH_BIAS_CLAMP_V                                                                        31:0

#define NVA197_SET_VERTEX_STREAM_INSTANCE_A(i)                                                     (0x1880+(i)*4)
#define NVA197_SET_VERTEX_STREAM_INSTANCE_A_IS_INSTANCED                                                      0:0
#define NVA197_SET_VERTEX_STREAM_INSTANCE_A_IS_INSTANCED_FALSE                                         0x00000000
#define NVA197_SET_VERTEX_STREAM_INSTANCE_A_IS_INSTANCED_TRUE                                          0x00000001

#define NVA197_SET_VERTEX_STREAM_INSTANCE_B(i)                                                     (0x18c0+(i)*4)
#define NVA197_SET_VERTEX_STREAM_INSTANCE_B_IS_INSTANCED                                                      0:0
#define NVA197_SET_VERTEX_STREAM_INSTANCE_B_IS_INSTANCED_FALSE                                         0x00000000
#define NVA197_SET_VERTEX_STREAM_INSTANCE_B_IS_INSTANCED_TRUE                                          0x00000001

#define NVA197_SET_ATTRIBUTE_POINT_SIZE                                                                    0x1910
#define NVA197_SET_ATTRIBUTE_POINT_SIZE_ENABLE                                                                0:0
#define NVA197_SET_ATTRIBUTE_POINT_SIZE_ENABLE_FALSE                                                   0x00000000
#define NVA197_SET_ATTRIBUTE_POINT_SIZE_ENABLE_TRUE                                                    0x00000001
#define NVA197_SET_ATTRIBUTE_POINT_SIZE_SLOT                                                                 11:4

#define NVA197_OGL_SET_CULL                                                                                0x1918
#define NVA197_OGL_SET_CULL_ENABLE                                                                            0:0
#define NVA197_OGL_SET_CULL_ENABLE_FALSE                                                               0x00000000
#define NVA197_OGL_SET_CULL_ENABLE_TRUE                                                                0x00000001

#define NVA197_OGL_SET_FRONT_FACE                                                                          0x191c
#define NVA197_OGL_SET_FRONT_FACE_V                                                                          31:0
#define NVA197_OGL_SET_FRONT_FACE_V_CW                                                                 0x00000900
#define NVA197_OGL_SET_FRONT_FACE_V_CCW                                                                0x00000901

#define NVA197_OGL_SET_CULL_FACE                                                                           0x1920
#define NVA197_OGL_SET_CULL_FACE_V                                                                           31:0
#define NVA197_OGL_SET_CULL_FACE_V_FRONT                                                               0x00000404
#define NVA197_OGL_SET_CULL_FACE_V_BACK                                                                0x00000405
#define NVA197_OGL_SET_CULL_FACE_V_FRONT_AND_BACK                                                      0x00000408

#define NVA197_SET_VIEWPORT_PIXEL                                                                          0x1924
#define NVA197_SET_VIEWPORT_PIXEL_CENTER                                                                      0:0
#define NVA197_SET_VIEWPORT_PIXEL_CENTER_AT_HALF_INTEGERS                                              0x00000000
#define NVA197_SET_VIEWPORT_PIXEL_CENTER_AT_INTEGERS                                                   0x00000001

#define NVA197_SET_VIEWPORT_SCALE_OFFSET                                                                   0x192c
#define NVA197_SET_VIEWPORT_SCALE_OFFSET_ENABLE                                                               0:0
#define NVA197_SET_VIEWPORT_SCALE_OFFSET_ENABLE_FALSE                                                  0x00000000
#define NVA197_SET_VIEWPORT_SCALE_OFFSET_ENABLE_TRUE                                                   0x00000001

#define NVA197_INVALIDATE_CONSTANT_BUFFER_CACHE                                                            0x1930
#define NVA197_INVALIDATE_CONSTANT_BUFFER_CACHE_THRU_L2                                                       0:0
#define NVA197_INVALIDATE_CONSTANT_BUFFER_CACHE_THRU_L2_FALSE                                          0x00000000
#define NVA197_INVALIDATE_CONSTANT_BUFFER_CACHE_THRU_L2_TRUE                                           0x00000001

#define NVA197_SET_VIEWPORT_CLIP_CONTROL                                                                   0x193c
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_MIN_Z_ZERO_MAX_Z_ONE                                                 0:0
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_MIN_Z_ZERO_MAX_Z_ONE_FALSE                                    0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_MIN_Z_ZERO_MAX_Z_ONE_TRUE                                     0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MIN_Z                                                          3:3
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MIN_Z_CLIP                                              0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MIN_Z_CLAMP                                             0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MAX_Z                                                          4:4
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MAX_Z_CLIP                                              0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_PIXEL_MAX_Z_CLAMP                                             0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND                                                   7:7
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_SCALE_256                                  0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_SCALE_1                                    0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_LINE_POINT_CULL_GUARDBAND                                          10:10
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_LINE_POINT_CULL_GUARDBAND_SCALE_256                           0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_LINE_POINT_CULL_GUARDBAND_SCALE_1                             0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP                                                      13:11
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_WZERO_CLIP                                      0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_PASSTHRU                                        0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_FRUSTUM_XY_CLIP                                 0x00000002
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_FRUSTUM_XYZ_CLIP                                0x00000003
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_WZERO_CLIP_NO_Z_CULL                            0x00000004
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_CLIP_FRUSTUM_Z_CLIP                                  0x00000005
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_Z                                                 2:1
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_Z_SAME_AS_XY_GUARDBAND                     0x00000000
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_Z_SCALE_256                                0x00000001
#define NVA197_SET_VIEWPORT_CLIP_CONTROL_GEOMETRY_GUARDBAND_Z_SCALE_1                                  0x00000002

#define NVA197_SET_USER_CLIP_OP                                                                            0x1940
#define NVA197_SET_USER_CLIP_OP_PLANE0                                                                        0:0
#define NVA197_SET_USER_CLIP_OP_PLANE0_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE0_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE1                                                                        4:4
#define NVA197_SET_USER_CLIP_OP_PLANE1_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE1_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE2                                                                        8:8
#define NVA197_SET_USER_CLIP_OP_PLANE2_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE2_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE3                                                                      12:12
#define NVA197_SET_USER_CLIP_OP_PLANE3_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE3_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE4                                                                      16:16
#define NVA197_SET_USER_CLIP_OP_PLANE4_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE4_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE5                                                                      20:20
#define NVA197_SET_USER_CLIP_OP_PLANE5_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE5_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE6                                                                      24:24
#define NVA197_SET_USER_CLIP_OP_PLANE6_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE6_CULL                                                            0x00000001
#define NVA197_SET_USER_CLIP_OP_PLANE7                                                                      28:28
#define NVA197_SET_USER_CLIP_OP_PLANE7_CLIP                                                            0x00000000
#define NVA197_SET_USER_CLIP_OP_PLANE7_CULL                                                            0x00000001

#define NVA197_SET_RENDER_ENABLE_OVERRIDE                                                                  0x1944
#define NVA197_SET_RENDER_ENABLE_OVERRIDE_MODE                                                                1:0
#define NVA197_SET_RENDER_ENABLE_OVERRIDE_MODE_USE_RENDER_ENABLE                                       0x00000000
#define NVA197_SET_RENDER_ENABLE_OVERRIDE_MODE_ALWAYS_RENDER                                           0x00000001
#define NVA197_SET_RENDER_ENABLE_OVERRIDE_MODE_NEVER_RENDER                                            0x00000002

#define NVA197_SET_PRIMITIVE_TOPOLOGY_CONTROL                                                              0x1948
#define NVA197_SET_PRIMITIVE_TOPOLOGY_CONTROL_OVERRIDE                                                        0:0
#define NVA197_SET_PRIMITIVE_TOPOLOGY_CONTROL_OVERRIDE_USE_TOPOLOGY_IN_BEGIN_METHODS                   0x00000000
#define NVA197_SET_PRIMITIVE_TOPOLOGY_CONTROL_OVERRIDE_USE_SEPARATE_TOPOLOGY_STATE                     0x00000001

#define NVA197_SET_WINDOW_CLIP_ENABLE                                                                      0x194c
#define NVA197_SET_WINDOW_CLIP_ENABLE_V                                                                       0:0
#define NVA197_SET_WINDOW_CLIP_ENABLE_V_FALSE                                                          0x00000000
#define NVA197_SET_WINDOW_CLIP_ENABLE_V_TRUE                                                           0x00000001

#define NVA197_SET_WINDOW_CLIP_TYPE                                                                        0x1950
#define NVA197_SET_WINDOW_CLIP_TYPE_V                                                                         1:0
#define NVA197_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE                                                        0x00000000
#define NVA197_SET_WINDOW_CLIP_TYPE_V_EXCLUSIVE                                                        0x00000001
#define NVA197_SET_WINDOW_CLIP_TYPE_V_CLIPALL                                                          0x00000002

#define NVA197_INVALIDATE_ZCULL                                                                            0x1958
#define NVA197_INVALIDATE_ZCULL_V                                                                            31:0
#define NVA197_INVALIDATE_ZCULL_V_INVALIDATE                                                           0x00000000

#define NVA197_SET_ZCULL                                                                                   0x1968
#define NVA197_SET_ZCULL_Z_ENABLE                                                                             0:0
#define NVA197_SET_ZCULL_Z_ENABLE_FALSE                                                                0x00000000
#define NVA197_SET_ZCULL_Z_ENABLE_TRUE                                                                 0x00000001
#define NVA197_SET_ZCULL_STENCIL_ENABLE                                                                       4:4
#define NVA197_SET_ZCULL_STENCIL_ENABLE_FALSE                                                          0x00000000
#define NVA197_SET_ZCULL_STENCIL_ENABLE_TRUE                                                           0x00000001

#define NVA197_SET_ZCULL_BOUNDS                                                                            0x196c
#define NVA197_SET_ZCULL_BOUNDS_Z_MIN_UNBOUNDED_ENABLE                                                        0:0
#define NVA197_SET_ZCULL_BOUNDS_Z_MIN_UNBOUNDED_ENABLE_FALSE                                           0x00000000
#define NVA197_SET_ZCULL_BOUNDS_Z_MIN_UNBOUNDED_ENABLE_TRUE                                            0x00000001
#define NVA197_SET_ZCULL_BOUNDS_Z_MAX_UNBOUNDED_ENABLE                                                        4:4
#define NVA197_SET_ZCULL_BOUNDS_Z_MAX_UNBOUNDED_ENABLE_FALSE                                           0x00000000
#define NVA197_SET_ZCULL_BOUNDS_Z_MAX_UNBOUNDED_ENABLE_TRUE                                            0x00000001

#define NVA197_SET_PRIMITIVE_TOPOLOGY                                                                      0x1970
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V                                                                      15:0
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_POINTLIST                                                      0x00000001
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LINELIST                                                       0x00000002
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LINESTRIP                                                      0x00000003
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_TRIANGLELIST                                                   0x00000004
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_TRIANGLESTRIP                                                  0x00000005
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LINELIST_ADJCY                                                 0x0000000A
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LINESTRIP_ADJCY                                                0x0000000B
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_TRIANGLELIST_ADJCY                                             0x0000000C
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_TRIANGLESTRIP_ADJCY                                            0x0000000D
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_PATCHLIST                                                      0x0000000E
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_POINTS                                                  0x00001001
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDLINELIST                                         0x00001002
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDTRIANGLELIST                                     0x00001003
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_LINELIST                                                0x0000100F
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_LINESTRIP                                               0x00001010
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDLINESTRIP                                        0x00001011
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_TRIANGLELIST                                            0x00001012
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_TRIANGLESTRIP                                           0x00001013
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDTRIANGLESTRIP                                    0x00001014
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_TRIANGLEFAN                                             0x00001015
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDTRIANGLEFAN                                      0x00001016
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_TRIANGLEFAN_IMM                                         0x00001017
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_LINELIST_IMM                                            0x00001018
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDTRIANGLELIST2                                    0x0000101A
#define NVA197_SET_PRIMITIVE_TOPOLOGY_V_LEGACY_INDEXEDLINELIST2                                        0x0000101B

#define NVA197_ZCULL_SYNC                                                                                  0x1978
#define NVA197_ZCULL_SYNC_V                                                                                  31:0

#define NVA197_SET_CLIP_ID_TEST                                                                            0x197c
#define NVA197_SET_CLIP_ID_TEST_ENABLE                                                                        0:0
#define NVA197_SET_CLIP_ID_TEST_ENABLE_FALSE                                                           0x00000000
#define NVA197_SET_CLIP_ID_TEST_ENABLE_TRUE                                                            0x00000001

#define NVA197_SET_SURFACE_CLIP_ID_WIDTH                                                                   0x1980
#define NVA197_SET_SURFACE_CLIP_ID_WIDTH_V                                                                   31:0

#define NVA197_SET_CLIP_ID                                                                                 0x1984
#define NVA197_SET_CLIP_ID_V                                                                                 31:0

#define NVA197_SET_DEPTH_BOUNDS_TEST                                                                       0x19bc
#define NVA197_SET_DEPTH_BOUNDS_TEST_ENABLE                                                                   0:0
#define NVA197_SET_DEPTH_BOUNDS_TEST_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_DEPTH_BOUNDS_TEST_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_BLEND_FLOAT_OPTION                                                                      0x19c0
#define NVA197_SET_BLEND_FLOAT_OPTION_ZERO_TIMES_ANYTHING_IS_ZERO                                             0:0
#define NVA197_SET_BLEND_FLOAT_OPTION_ZERO_TIMES_ANYTHING_IS_ZERO_FALSE                                0x00000000
#define NVA197_SET_BLEND_FLOAT_OPTION_ZERO_TIMES_ANYTHING_IS_ZERO_TRUE                                 0x00000001

#define NVA197_SET_LOGIC_OP                                                                                0x19c4
#define NVA197_SET_LOGIC_OP_ENABLE                                                                            0:0
#define NVA197_SET_LOGIC_OP_ENABLE_FALSE                                                               0x00000000
#define NVA197_SET_LOGIC_OP_ENABLE_TRUE                                                                0x00000001

#define NVA197_SET_LOGIC_OP_FUNC                                                                           0x19c8
#define NVA197_SET_LOGIC_OP_FUNC_V                                                                           31:0
#define NVA197_SET_LOGIC_OP_FUNC_V_CLEAR                                                               0x00001500
#define NVA197_SET_LOGIC_OP_FUNC_V_AND                                                                 0x00001501
#define NVA197_SET_LOGIC_OP_FUNC_V_AND_REVERSE                                                         0x00001502
#define NVA197_SET_LOGIC_OP_FUNC_V_COPY                                                                0x00001503
#define NVA197_SET_LOGIC_OP_FUNC_V_AND_INVERTED                                                        0x00001504
#define NVA197_SET_LOGIC_OP_FUNC_V_NOOP                                                                0x00001505
#define NVA197_SET_LOGIC_OP_FUNC_V_XOR                                                                 0x00001506
#define NVA197_SET_LOGIC_OP_FUNC_V_OR                                                                  0x00001507
#define NVA197_SET_LOGIC_OP_FUNC_V_NOR                                                                 0x00001508
#define NVA197_SET_LOGIC_OP_FUNC_V_EQUIV                                                               0x00001509
#define NVA197_SET_LOGIC_OP_FUNC_V_INVERT                                                              0x0000150A
#define NVA197_SET_LOGIC_OP_FUNC_V_OR_REVERSE                                                          0x0000150B
#define NVA197_SET_LOGIC_OP_FUNC_V_COPY_INVERTED                                                       0x0000150C
#define NVA197_SET_LOGIC_OP_FUNC_V_OR_INVERTED                                                         0x0000150D
#define NVA197_SET_LOGIC_OP_FUNC_V_NAND                                                                0x0000150E
#define NVA197_SET_LOGIC_OP_FUNC_V_SET                                                                 0x0000150F

#define NVA197_SET_Z_COMPRESSION                                                                           0x19cc
#define NVA197_SET_Z_COMPRESSION_ENABLE                                                                       0:0
#define NVA197_SET_Z_COMPRESSION_ENABLE_FALSE                                                          0x00000000
#define NVA197_SET_Z_COMPRESSION_ENABLE_TRUE                                                           0x00000001

#define NVA197_CLEAR_SURFACE                                                                               0x19d0
#define NVA197_CLEAR_SURFACE_Z_ENABLE                                                                         0:0
#define NVA197_CLEAR_SURFACE_Z_ENABLE_FALSE                                                            0x00000000
#define NVA197_CLEAR_SURFACE_Z_ENABLE_TRUE                                                             0x00000001
#define NVA197_CLEAR_SURFACE_STENCIL_ENABLE                                                                   1:1
#define NVA197_CLEAR_SURFACE_STENCIL_ENABLE_FALSE                                                      0x00000000
#define NVA197_CLEAR_SURFACE_STENCIL_ENABLE_TRUE                                                       0x00000001
#define NVA197_CLEAR_SURFACE_R_ENABLE                                                                         2:2
#define NVA197_CLEAR_SURFACE_R_ENABLE_FALSE                                                            0x00000000
#define NVA197_CLEAR_SURFACE_R_ENABLE_TRUE                                                             0x00000001
#define NVA197_CLEAR_SURFACE_G_ENABLE                                                                         3:3
#define NVA197_CLEAR_SURFACE_G_ENABLE_FALSE                                                            0x00000000
#define NVA197_CLEAR_SURFACE_G_ENABLE_TRUE                                                             0x00000001
#define NVA197_CLEAR_SURFACE_B_ENABLE                                                                         4:4
#define NVA197_CLEAR_SURFACE_B_ENABLE_FALSE                                                            0x00000000
#define NVA197_CLEAR_SURFACE_B_ENABLE_TRUE                                                             0x00000001
#define NVA197_CLEAR_SURFACE_A_ENABLE                                                                         5:5
#define NVA197_CLEAR_SURFACE_A_ENABLE_FALSE                                                            0x00000000
#define NVA197_CLEAR_SURFACE_A_ENABLE_TRUE                                                             0x00000001
#define NVA197_CLEAR_SURFACE_MRT_SELECT                                                                       9:6
#define NVA197_CLEAR_SURFACE_RT_ARRAY_INDEX                                                                 25:10

#define NVA197_CLEAR_CLIP_ID_SURFACE                                                                       0x19d4
#define NVA197_CLEAR_CLIP_ID_SURFACE_V                                                                       31:0

#define NVA197_SET_COLOR_COMPRESSION(i)                                                            (0x19e0+(i)*4)
#define NVA197_SET_COLOR_COMPRESSION_ENABLE                                                                   0:0
#define NVA197_SET_COLOR_COMPRESSION_ENABLE_FALSE                                                      0x00000000
#define NVA197_SET_COLOR_COMPRESSION_ENABLE_TRUE                                                       0x00000001

#define NVA197_SET_CT_WRITE(i)                                                                     (0x1a00+(i)*4)
#define NVA197_SET_CT_WRITE_R_ENABLE                                                                          0:0
#define NVA197_SET_CT_WRITE_R_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_CT_WRITE_R_ENABLE_TRUE                                                              0x00000001
#define NVA197_SET_CT_WRITE_G_ENABLE                                                                          4:4
#define NVA197_SET_CT_WRITE_G_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_CT_WRITE_G_ENABLE_TRUE                                                              0x00000001
#define NVA197_SET_CT_WRITE_B_ENABLE                                                                          8:8
#define NVA197_SET_CT_WRITE_B_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_CT_WRITE_B_ENABLE_TRUE                                                              0x00000001
#define NVA197_SET_CT_WRITE_A_ENABLE                                                                        12:12
#define NVA197_SET_CT_WRITE_A_ENABLE_FALSE                                                             0x00000000
#define NVA197_SET_CT_WRITE_A_ENABLE_TRUE                                                              0x00000001

#define NVA197_PIPE_NOP                                                                                    0x1a2c
#define NVA197_PIPE_NOP_V                                                                                    31:0

#define NVA197_SET_SPARE00                                                                                 0x1a30
#define NVA197_SET_SPARE00_V                                                                                 31:0

#define NVA197_SET_SPARE01                                                                                 0x1a34
#define NVA197_SET_SPARE01_V                                                                                 31:0

#define NVA197_SET_SPARE02                                                                                 0x1a38
#define NVA197_SET_SPARE02_V                                                                                 31:0

#define NVA197_SET_SPARE03                                                                                 0x1a3c
#define NVA197_SET_SPARE03_V                                                                                 31:0

#define NVA197_SET_REPORT_SEMAPHORE_A                                                                      0x1b00
#define NVA197_SET_REPORT_SEMAPHORE_A_OFFSET_UPPER                                                            7:0

#define NVA197_SET_REPORT_SEMAPHORE_B                                                                      0x1b04
#define NVA197_SET_REPORT_SEMAPHORE_B_OFFSET_LOWER                                                           31:0

#define NVA197_SET_REPORT_SEMAPHORE_C                                                                      0x1b08
#define NVA197_SET_REPORT_SEMAPHORE_C_PAYLOAD                                                                31:0

#define NVA197_SET_REPORT_SEMAPHORE_D                                                                      0x1b0c
#define NVA197_SET_REPORT_SEMAPHORE_D_OPERATION                                                               1:0
#define NVA197_SET_REPORT_SEMAPHORE_D_OPERATION_RELEASE                                                0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_OPERATION_ACQUIRE                                                0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_OPERATION_REPORT_ONLY                                            0x00000002
#define NVA197_SET_REPORT_SEMAPHORE_D_OPERATION_TRAP                                                   0x00000003
#define NVA197_SET_REPORT_SEMAPHORE_D_RELEASE                                                                 4:4
#define NVA197_SET_REPORT_SEMAPHORE_D_RELEASE_AFTER_ALL_PRECEEDING_READS_COMPLETE                      0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_RELEASE_AFTER_ALL_PRECEEDING_WRITES_COMPLETE                     0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_ACQUIRE                                                                 8:8
#define NVA197_SET_REPORT_SEMAPHORE_D_ACQUIRE_BEFORE_ANY_FOLLOWING_WRITES_START                        0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_ACQUIRE_BEFORE_ANY_FOLLOWING_READS_START                         0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION                                                     15:12
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_NONE                                           0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_DATA_ASSEMBLER                                 0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_VERTEX_SHADER                                  0x00000002
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_TESSELATION_INIT_SHADER                        0x00000008
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_TESSELATION_SHADER                             0x00000009
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_GEOMETRY_SHADER                                0x00000006
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_STREAMING_OUTPUT                               0x00000005
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_VPC                                            0x00000004
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_ZCULL                                          0x00000007
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_PIXEL_SHADER                                   0x0000000A
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_DEPTH_TEST                                     0x0000000C
#define NVA197_SET_REPORT_SEMAPHORE_D_PIPELINE_LOCATION_ALL                                            0x0000000F
#define NVA197_SET_REPORT_SEMAPHORE_D_COMPARISON                                                            16:16
#define NVA197_SET_REPORT_SEMAPHORE_D_COMPARISON_EQ                                                    0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_COMPARISON_GE                                                    0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE                                                         20:20
#define NVA197_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE_FALSE                                              0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_AWAKEN_ENABLE_TRUE                                               0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT                                                                27:23
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_NONE                                                      0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_DA_VERTICES_GENERATED                                     0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_DA_PRIMITIVES_GENERATED                                   0x00000003
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_VS_INVOCATIONS                                            0x00000005
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_TI_INVOCATIONS                                            0x0000001B
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_TS_INVOCATIONS                                            0x0000001D
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_TS_PRIMITIVES_GENERATED                                   0x0000001F
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_GS_INVOCATIONS                                            0x00000007
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_GS_PRIMITIVES_GENERATED                                   0x00000009
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ALPHA_BETA_CLOCKS                                         0x00000004
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_VTG_PRIMITIVES_OUT                                        0x00000012
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_TOTAL_STREAMING_PRIMITIVES_NEEDED_MINUS_SUCCEEDED             0x0000001E
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_STREAMING_PRIMITIVES_SUCCEEDED                            0x0000000B
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_STREAMING_PRIMITIVES_NEEDED                               0x0000000D
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_STREAMING_PRIMITIVES_NEEDED_MINUS_SUCCEEDED               0x00000006
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_STREAMING_BYTE_COUNT                                      0x0000001A
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_CLIPPER_INVOCATIONS                                       0x0000000F
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_CLIPPER_PRIMITIVES_GENERATED                              0x00000011
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZCULL_STATS0                                              0x0000000A
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZCULL_STATS1                                              0x0000000C
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZCULL_STATS2                                              0x0000000E
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZCULL_STATS3                                              0x00000010
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_PS_INVOCATIONS                                            0x00000013
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZPASS_PIXEL_CNT                                           0x00000002
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_ZPASS_PIXEL_CNT64                                         0x00000015
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_IEEE_CLEAN_COLOR_TARGET                                   0x00000018
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_IEEE_CLEAN_ZETA_TARGET                                    0x00000019
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_BOUNDING_RECTANGLE                                        0x0000001C
#define NVA197_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE                                                        28:28
#define NVA197_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE_FOUR_WORDS                                        0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_STRUCTURE_SIZE_ONE_WORD                                          0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_SUB_REPORT                                                              7:5
#define NVA197_SET_REPORT_SEMAPHORE_D_REPORT_DWORD_NUMBER                                                   21:21
#define NVA197_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE                                                           2:2
#define NVA197_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE_FALSE                                              0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_FLUSH_DISABLE_TRUE                                               0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE                                                        3:3
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE_FALSE                                           0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_ENABLE_TRUE                                            0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP                                                           11:9
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_ADD                                             0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_MIN                                             0x00000001
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_MAX                                             0x00000002
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_INC                                             0x00000003
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_DEC                                             0x00000004
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_AND                                             0x00000005
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_OR                                              0x00000006
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_OP_RED_XOR                                             0x00000007
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT                                                      18:17
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT_UNSIGNED_32                                     0x00000000
#define NVA197_SET_REPORT_SEMAPHORE_D_REDUCTION_FORMAT_SIGNED_32                                       0x00000001

#define NVA197_SET_VERTEX_STREAM_A_FORMAT(j)                                                      (0x1c00+(j)*16)
#define NVA197_SET_VERTEX_STREAM_A_FORMAT_STRIDE                                                             11:0
#define NVA197_SET_VERTEX_STREAM_A_FORMAT_ENABLE                                                            12:12
#define NVA197_SET_VERTEX_STREAM_A_FORMAT_ENABLE_FALSE                                                 0x00000000
#define NVA197_SET_VERTEX_STREAM_A_FORMAT_ENABLE_TRUE                                                  0x00000001

#define NVA197_SET_VERTEX_STREAM_A_LOCATION_A(j)                                                  (0x1c04+(j)*16)
#define NVA197_SET_VERTEX_STREAM_A_LOCATION_A_OFFSET_UPPER                                                    7:0

#define NVA197_SET_VERTEX_STREAM_A_LOCATION_B(j)                                                  (0x1c08+(j)*16)
#define NVA197_SET_VERTEX_STREAM_A_LOCATION_B_OFFSET_LOWER                                                   31:0

#define NVA197_SET_VERTEX_STREAM_A_FREQUENCY(j)                                                   (0x1c0c+(j)*16)
#define NVA197_SET_VERTEX_STREAM_A_FREQUENCY_V                                                               31:0

#define NVA197_SET_VERTEX_STREAM_B_FORMAT(j)                                                      (0x1d00+(j)*16)
#define NVA197_SET_VERTEX_STREAM_B_FORMAT_STRIDE                                                             11:0
#define NVA197_SET_VERTEX_STREAM_B_FORMAT_ENABLE                                                            12:12
#define NVA197_SET_VERTEX_STREAM_B_FORMAT_ENABLE_FALSE                                                 0x00000000
#define NVA197_SET_VERTEX_STREAM_B_FORMAT_ENABLE_TRUE                                                  0x00000001

#define NVA197_SET_VERTEX_STREAM_B_LOCATION_A(j)                                                  (0x1d04+(j)*16)
#define NVA197_SET_VERTEX_STREAM_B_LOCATION_A_OFFSET_UPPER                                                    7:0

#define NVA197_SET_VERTEX_STREAM_B_LOCATION_B(j)                                                  (0x1d08+(j)*16)
#define NVA197_SET_VERTEX_STREAM_B_LOCATION_B_OFFSET_LOWER                                                   31:0

#define NVA197_SET_VERTEX_STREAM_B_FREQUENCY(j)                                                   (0x1d0c+(j)*16)
#define NVA197_SET_VERTEX_STREAM_B_FREQUENCY_V                                                               31:0

#define NVA197_SET_BLEND_PER_TARGET_SEPARATE_FOR_ALPHA(j)                                         (0x1e00+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_SEPARATE_FOR_ALPHA_ENABLE                                                 0:0
#define NVA197_SET_BLEND_PER_TARGET_SEPARATE_FOR_ALPHA_ENABLE_FALSE                                    0x00000000
#define NVA197_SET_BLEND_PER_TARGET_SEPARATE_FOR_ALPHA_ENABLE_TRUE                                     0x00000001

#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP(j)                                                   (0x1e04+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V                                                               31:0
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_OGL_FUNC_SUBTRACT                                       0x0000800A
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_OGL_FUNC_REVERSE_SUBTRACT                               0x0000800B
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_OGL_FUNC_ADD                                            0x00008006
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_OGL_MIN                                                 0x00008007
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_OGL_MAX                                                 0x00008008
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_D3D_ADD                                                 0x00000001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_D3D_SUBTRACT                                            0x00000002
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_D3D_REVSUBTRACT                                         0x00000003
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_D3D_MIN                                                 0x00000004
#define NVA197_SET_BLEND_PER_TARGET_COLOR_OP_V_D3D_MAX                                                 0x00000005

#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF(j)                                         (0x1e08+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V                                                     31:0
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ZERO                                      0x00004000
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE                                       0x00004001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_SRC_COLOR                                 0x00004300
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                       0x00004301
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_SRC_ALPHA                                 0x00004302
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                       0x00004303
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_DST_ALPHA                                 0x00004304
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                       0x00004305
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_DST_COLOR                                 0x00004306
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_COLOR                       0x00004307
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_SRC_ALPHA_SATURATE                        0x00004308
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_CONSTANT_COLOR                            0x0000C001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                  0x0000C002
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_CONSTANT_ALPHA                            0x0000C003
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                  0x0000C004
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_SRC1COLOR                                 0x0000C900
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_INVSRC1COLOR                              0x0000C901
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_SRC1ALPHA                                 0x0000C902
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_OGL_INVSRC1ALPHA                              0x0000C903
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_ZERO                                      0x00000001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_ONE                                       0x00000002
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_SRCCOLOR                                  0x00000003
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVSRCCOLOR                               0x00000004
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_SRCALPHA                                  0x00000005
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVSRCALPHA                               0x00000006
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_DESTALPHA                                 0x00000007
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVDESTALPHA                              0x00000008
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_DESTCOLOR                                 0x00000009
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVDESTCOLOR                              0x0000000A
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_SRCALPHASAT                               0x0000000B
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_BOTHSRCALPHA                              0x0000000C
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_BOTHINVSRCALPHA                           0x0000000D
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_BLENDFACTOR                               0x0000000E
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVBLENDFACTOR                            0x0000000F
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_SRC1COLOR                                 0x00000010
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVSRC1COLOR                              0x00000011
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_SRC1ALPHA                                 0x00000012
#define NVA197_SET_BLEND_PER_TARGET_COLOR_SOURCE_COEFF_V_D3D_INVSRC1ALPHA                              0x00000013

#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF(j)                                           (0x1e0c+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V                                                       31:0
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ZERO                                        0x00004000
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE                                         0x00004001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_SRC_COLOR                                   0x00004300
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                         0x00004301
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_SRC_ALPHA                                   0x00004302
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                         0x00004303
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_DST_ALPHA                                   0x00004304
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                         0x00004305
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_DST_COLOR                                   0x00004306
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_DST_COLOR                         0x00004307
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_SRC_ALPHA_SATURATE                          0x00004308
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_CONSTANT_COLOR                              0x0000C001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                    0x0000C002
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_CONSTANT_ALPHA                              0x0000C003
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                    0x0000C004
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_SRC1COLOR                                   0x0000C900
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_INVSRC1COLOR                                0x0000C901
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_SRC1ALPHA                                   0x0000C902
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_OGL_INVSRC1ALPHA                                0x0000C903
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_ZERO                                        0x00000001
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_ONE                                         0x00000002
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_SRCCOLOR                                    0x00000003
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVSRCCOLOR                                 0x00000004
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_SRCALPHA                                    0x00000005
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVSRCALPHA                                 0x00000006
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_DESTALPHA                                   0x00000007
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVDESTALPHA                                0x00000008
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_DESTCOLOR                                   0x00000009
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVDESTCOLOR                                0x0000000A
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_SRCALPHASAT                                 0x0000000B
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_BLENDFACTOR                                 0x0000000E
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVBLENDFACTOR                              0x0000000F
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_SRC1COLOR                                   0x00000010
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVSRC1COLOR                                0x00000011
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_SRC1ALPHA                                   0x00000012
#define NVA197_SET_BLEND_PER_TARGET_COLOR_DEST_COEFF_V_D3D_INVSRC1ALPHA                                0x00000013

#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP(j)                                                   (0x1e10+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V                                                               31:0
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_OGL_FUNC_SUBTRACT                                       0x0000800A
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_OGL_FUNC_REVERSE_SUBTRACT                               0x0000800B
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_OGL_FUNC_ADD                                            0x00008006
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_OGL_MIN                                                 0x00008007
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_OGL_MAX                                                 0x00008008
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_D3D_ADD                                                 0x00000001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_D3D_SUBTRACT                                            0x00000002
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_D3D_REVSUBTRACT                                         0x00000003
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_D3D_MIN                                                 0x00000004
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_OP_V_D3D_MAX                                                 0x00000005

#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF(j)                                         (0x1e14+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V                                                     31:0
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ZERO                                      0x00004000
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE                                       0x00004001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_SRC_COLOR                                 0x00004300
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                       0x00004301
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_SRC_ALPHA                                 0x00004302
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                       0x00004303
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_DST_ALPHA                                 0x00004304
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                       0x00004305
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_DST_COLOR                                 0x00004306
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_DST_COLOR                       0x00004307
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_SRC_ALPHA_SATURATE                        0x00004308
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_CONSTANT_COLOR                            0x0000C001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                  0x0000C002
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_CONSTANT_ALPHA                            0x0000C003
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                  0x0000C004
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_SRC1COLOR                                 0x0000C900
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_INVSRC1COLOR                              0x0000C901
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_SRC1ALPHA                                 0x0000C902
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_OGL_INVSRC1ALPHA                              0x0000C903
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_ZERO                                      0x00000001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_ONE                                       0x00000002
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_SRCCOLOR                                  0x00000003
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVSRCCOLOR                               0x00000004
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_SRCALPHA                                  0x00000005
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVSRCALPHA                               0x00000006
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_DESTALPHA                                 0x00000007
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVDESTALPHA                              0x00000008
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_DESTCOLOR                                 0x00000009
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVDESTCOLOR                              0x0000000A
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_SRCALPHASAT                               0x0000000B
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_BOTHSRCALPHA                              0x0000000C
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_BOTHINVSRCALPHA                           0x0000000D
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_BLENDFACTOR                               0x0000000E
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVBLENDFACTOR                            0x0000000F
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_SRC1COLOR                                 0x00000010
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVSRC1COLOR                              0x00000011
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_SRC1ALPHA                                 0x00000012
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_SOURCE_COEFF_V_D3D_INVSRC1ALPHA                              0x00000013

#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF(j)                                           (0x1e18+(j)*32)
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V                                                       31:0
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ZERO                                        0x00004000
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE                                         0x00004001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_SRC_COLOR                                   0x00004300
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_SRC_COLOR                         0x00004301
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_SRC_ALPHA                                   0x00004302
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_SRC_ALPHA                         0x00004303
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_DST_ALPHA                                   0x00004304
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_DST_ALPHA                         0x00004305
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_DST_COLOR                                   0x00004306
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_DST_COLOR                         0x00004307
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_SRC_ALPHA_SATURATE                          0x00004308
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_CONSTANT_COLOR                              0x0000C001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_COLOR                    0x0000C002
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_CONSTANT_ALPHA                              0x0000C003
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_ONE_MINUS_CONSTANT_ALPHA                    0x0000C004
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_SRC1COLOR                                   0x0000C900
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_INVSRC1COLOR                                0x0000C901
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_SRC1ALPHA                                   0x0000C902
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_OGL_INVSRC1ALPHA                                0x0000C903
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_ZERO                                        0x00000001
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_ONE                                         0x00000002
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_SRCCOLOR                                    0x00000003
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVSRCCOLOR                                 0x00000004
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_SRCALPHA                                    0x00000005
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVSRCALPHA                                 0x00000006
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_DESTALPHA                                   0x00000007
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVDESTALPHA                                0x00000008
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_DESTCOLOR                                   0x00000009
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVDESTCOLOR                                0x0000000A
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_SRCALPHASAT                                 0x0000000B
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_BLENDFACTOR                                 0x0000000E
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVBLENDFACTOR                              0x0000000F
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_SRC1COLOR                                   0x00000010
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVSRC1COLOR                                0x00000011
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_SRC1ALPHA                                   0x00000012
#define NVA197_SET_BLEND_PER_TARGET_ALPHA_DEST_COEFF_V_D3D_INVSRC1ALPHA                                0x00000013

#define NVA197_SET_VERTEX_STREAM_LIMIT_A_A(j)                                                      (0x1f00+(j)*8)
#define NVA197_SET_VERTEX_STREAM_LIMIT_A_A_UPPER                                                              7:0

#define NVA197_SET_VERTEX_STREAM_LIMIT_A_B(j)                                                      (0x1f04+(j)*8)
#define NVA197_SET_VERTEX_STREAM_LIMIT_A_B_LOWER                                                             31:0

#define NVA197_SET_VERTEX_STREAM_LIMIT_B_A(j)                                                      (0x1f80+(j)*8)
#define NVA197_SET_VERTEX_STREAM_LIMIT_B_A_UPPER                                                              7:0

#define NVA197_SET_VERTEX_STREAM_LIMIT_B_B(j)                                                      (0x1f84+(j)*8)
#define NVA197_SET_VERTEX_STREAM_LIMIT_B_B_LOWER                                                             31:0

#define NVA197_SET_PIPELINE_SHADER(j)                                                             (0x2000+(j)*64)
#define NVA197_SET_PIPELINE_SHADER_ENABLE                                                                     0:0
#define NVA197_SET_PIPELINE_SHADER_ENABLE_FALSE                                                        0x00000000
#define NVA197_SET_PIPELINE_SHADER_ENABLE_TRUE                                                         0x00000001
#define NVA197_SET_PIPELINE_SHADER_TYPE                                                                       7:4
#define NVA197_SET_PIPELINE_SHADER_TYPE_VERTEX_CULL_BEFORE_FETCH                                       0x00000000
#define NVA197_SET_PIPELINE_SHADER_TYPE_VERTEX                                                         0x00000001
#define NVA197_SET_PIPELINE_SHADER_TYPE_TESSELLATION_INIT                                              0x00000002
#define NVA197_SET_PIPELINE_SHADER_TYPE_TESSELLATION                                                   0x00000003
#define NVA197_SET_PIPELINE_SHADER_TYPE_GEOMETRY                                                       0x00000004
#define NVA197_SET_PIPELINE_SHADER_TYPE_PIXEL                                                          0x00000005

#define NVA197_SET_PIPELINE_PROGRAM(j)                                                            (0x2004+(j)*64)
#define NVA197_SET_PIPELINE_PROGRAM_OFFSET                                                                   31:0

#define NVA197_SET_PIPELINE_RESERVED_A(j)                                                         (0x2008+(j)*64)
#define NVA197_SET_PIPELINE_RESERVED_A_V                                                                      0:0

#define NVA197_SET_PIPELINE_REGISTER_COUNT(j)                                                     (0x200c+(j)*64)
#define NVA197_SET_PIPELINE_REGISTER_COUNT_V                                                                  7:0

#define NVA197_SET_PIPELINE_BINDING(j)                                                            (0x2010+(j)*64)
#define NVA197_SET_PIPELINE_BINDING_GROUP                                                                     2:0

#define NVA197_SET_PIPELINE_RESERVED_B(j)                                                         (0x2014+(j)*64)
#define NVA197_SET_PIPELINE_RESERVED_B_V                                                                      0:0

#define NVA197_SET_PIPELINE_RESERVED_C(j)                                                         (0x2018+(j)*64)
#define NVA197_SET_PIPELINE_RESERVED_C_V                                                                      0:0

#define NVA197_SET_PIPELINE_RESERVED_D(j)                                                         (0x201c+(j)*64)
#define NVA197_SET_PIPELINE_RESERVED_D_V                                                                      0:0

#define NVA197_SET_PIPELINE_RESERVED_E(j)                                                         (0x2020+(j)*64)
#define NVA197_SET_PIPELINE_RESERVED_E_V                                                                      0:0

#define NVA197_SET_FALCON00                                                                                0x2300
#define NVA197_SET_FALCON00_V                                                                                31:0

#define NVA197_SET_FALCON01                                                                                0x2304
#define NVA197_SET_FALCON01_V                                                                                31:0

#define NVA197_SET_FALCON02                                                                                0x2308
#define NVA197_SET_FALCON02_V                                                                                31:0

#define NVA197_SET_FALCON03                                                                                0x230c
#define NVA197_SET_FALCON03_V                                                                                31:0

#define NVA197_SET_FALCON04                                                                                0x2310
#define NVA197_SET_FALCON04_V                                                                                31:0

#define NVA197_SET_FALCON05                                                                                0x2314
#define NVA197_SET_FALCON05_V                                                                                31:0

#define NVA197_SET_FALCON06                                                                                0x2318
#define NVA197_SET_FALCON06_V                                                                                31:0

#define NVA197_SET_FALCON07                                                                                0x231c
#define NVA197_SET_FALCON07_V                                                                                31:0

#define NVA197_SET_FALCON08                                                                                0x2320
#define NVA197_SET_FALCON08_V                                                                                31:0

#define NVA197_SET_FALCON09                                                                                0x2324
#define NVA197_SET_FALCON09_V                                                                                31:0

#define NVA197_SET_FALCON10                                                                                0x2328
#define NVA197_SET_FALCON10_V                                                                                31:0

#define NVA197_SET_FALCON11                                                                                0x232c
#define NVA197_SET_FALCON11_V                                                                                31:0

#define NVA197_SET_FALCON12                                                                                0x2330
#define NVA197_SET_FALCON12_V                                                                                31:0

#define NVA197_SET_FALCON13                                                                                0x2334
#define NVA197_SET_FALCON13_V                                                                                31:0

#define NVA197_SET_FALCON14                                                                                0x2338
#define NVA197_SET_FALCON14_V                                                                                31:0

#define NVA197_SET_FALCON15                                                                                0x233c
#define NVA197_SET_FALCON15_V                                                                                31:0

#define NVA197_SET_FALCON16                                                                                0x2340
#define NVA197_SET_FALCON16_V                                                                                31:0

#define NVA197_SET_FALCON17                                                                                0x2344
#define NVA197_SET_FALCON17_V                                                                                31:0

#define NVA197_SET_FALCON18                                                                                0x2348
#define NVA197_SET_FALCON18_V                                                                                31:0

#define NVA197_SET_FALCON19                                                                                0x234c
#define NVA197_SET_FALCON19_V                                                                                31:0

#define NVA197_SET_FALCON20                                                                                0x2350
#define NVA197_SET_FALCON20_V                                                                                31:0

#define NVA197_SET_FALCON21                                                                                0x2354
#define NVA197_SET_FALCON21_V                                                                                31:0

#define NVA197_SET_FALCON22                                                                                0x2358
#define NVA197_SET_FALCON22_V                                                                                31:0

#define NVA197_SET_FALCON23                                                                                0x235c
#define NVA197_SET_FALCON23_V                                                                                31:0

#define NVA197_SET_FALCON24                                                                                0x2360
#define NVA197_SET_FALCON24_V                                                                                31:0

#define NVA197_SET_FALCON25                                                                                0x2364
#define NVA197_SET_FALCON25_V                                                                                31:0

#define NVA197_SET_FALCON26                                                                                0x2368
#define NVA197_SET_FALCON26_V                                                                                31:0

#define NVA197_SET_FALCON27                                                                                0x236c
#define NVA197_SET_FALCON27_V                                                                                31:0

#define NVA197_SET_FALCON28                                                                                0x2370
#define NVA197_SET_FALCON28_V                                                                                31:0

#define NVA197_SET_FALCON29                                                                                0x2374
#define NVA197_SET_FALCON29_V                                                                                31:0

#define NVA197_SET_FALCON30                                                                                0x2378
#define NVA197_SET_FALCON30_V                                                                                31:0

#define NVA197_SET_FALCON31                                                                                0x237c
#define NVA197_SET_FALCON31_V                                                                                31:0

#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_A                                                              0x2380
#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_A_SIZE                                                           16:0

#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_B                                                              0x2384
#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_B_ADDRESS_UPPER                                                   7:0

#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_C                                                              0x2388
#define NVA197_SET_CONSTANT_BUFFER_SELECTOR_C_ADDRESS_LOWER                                                  31:0

#define NVA197_LOAD_CONSTANT_BUFFER_OFFSET                                                                 0x238c
#define NVA197_LOAD_CONSTANT_BUFFER_OFFSET_V                                                                 15:0

#define NVA197_LOAD_CONSTANT_BUFFER(i)                                                             (0x2390+(i)*4)
#define NVA197_LOAD_CONSTANT_BUFFER_V                                                                        31:0

#define NVA197_BIND_GROUP_RESERVED_A(j)                                                           (0x2400+(j)*32)
#define NVA197_BIND_GROUP_RESERVED_A_V                                                                        0:0

#define NVA197_BIND_GROUP_RESERVED_B(j)                                                           (0x2404+(j)*32)
#define NVA197_BIND_GROUP_RESERVED_B_V                                                                        0:0

#define NVA197_BIND_GROUP_RESERVED_C(j)                                                           (0x2408+(j)*32)
#define NVA197_BIND_GROUP_RESERVED_C_V                                                                        0:0

#define NVA197_BIND_GROUP_RESERVED_D(j)                                                           (0x240c+(j)*32)
#define NVA197_BIND_GROUP_RESERVED_D_V                                                                        0:0

#define NVA197_BIND_GROUP_CONSTANT_BUFFER(j)                                                      (0x2410+(j)*32)
#define NVA197_BIND_GROUP_CONSTANT_BUFFER_VALID                                                               0:0
#define NVA197_BIND_GROUP_CONSTANT_BUFFER_VALID_FALSE                                                  0x00000000
#define NVA197_BIND_GROUP_CONSTANT_BUFFER_VALID_TRUE                                                   0x00000001
#define NVA197_BIND_GROUP_CONSTANT_BUFFER_SHADER_SLOT                                                         8:4

#define NVA197_SET_COLOR_CLAMP                                                                             0x2600
#define NVA197_SET_COLOR_CLAMP_ENABLE                                                                         0:0
#define NVA197_SET_COLOR_CLAMP_ENABLE_FALSE                                                            0x00000000
#define NVA197_SET_COLOR_CLAMP_ENABLE_TRUE                                                             0x00000001

#define NVA197_NOOP_X_X_X_SET_VALVE                                                                        0x2604
#define NVA197_NOOP_X_X_X_SET_VALVE_HIGHER_PRIORITY                                                           0:0
#define NVA197_NOOP_X_X_X_SET_VALVE_HIGHER_PRIORITY_COMPUTE                                            0x00000000
#define NVA197_NOOP_X_X_X_SET_VALVE_HIGHER_PRIORITY_GRAPHICS                                           0x00000001

#define NVA197_SET_BINDLESS_TEXTURE                                                                        0x2608
#define NVA197_SET_BINDLESS_TEXTURE_CONSTANT_BUFFER_SLOT_SELECT                                               4:0

#define NVA197_SET_TRAP_HANDLER                                                                            0x260c
#define NVA197_SET_TRAP_HANDLER_OFFSET                                                                       31:0

#define NVA197_SET_STREAM_OUT_LAYOUT_SELECT(i,j)                                           (0x2800+(i)*128+(j)*4)
#define NVA197_SET_STREAM_OUT_LAYOUT_SELECT_ATTRIBUTE_NUMBER00                                                7:0
#define NVA197_SET_STREAM_OUT_LAYOUT_SELECT_ATTRIBUTE_NUMBER01                                               15:8
#define NVA197_SET_STREAM_OUT_LAYOUT_SELECT_ATTRIBUTE_NUMBER02                                              23:16
#define NVA197_SET_STREAM_OUT_LAYOUT_SELECT_ATTRIBUTE_NUMBER03                                              31:24

#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_VALUE(i)                                             (0x335c+(i)*4)
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_VALUE_V                                                        31:0

#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_EVENT(i)                                             (0x337c+(i)*4)
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_EVENT_EVENT                                                     7:0

#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A(i)                                         (0x339c+(i)*4)
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT0                                                1:0
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT0                                           4:2
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT1                                                6:5
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT1                                           9:7
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT2                                              11:10
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT2                                         14:12
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT3                                              16:15
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT3                                         19:17
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT4                                              21:20
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT4                                         24:22
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_EVENT5                                              26:25
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_BIT_SELECT5                                         29:27
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_A_SPARE                                               31:30

#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B(i)                                         (0x33bc+(i)*4)
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_EDGE                                                  0:0
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_MODE                                                  2:1
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_WINDOWED                                              3:3
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_CONTROL_B_FUNC                                                 19:4

#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_TRAP_CONTROL                                                 0x33dc
#define NVA197_SET_SHADER_PERFORMANCE_COUNTER_TRAP_CONTROL_MASK                                               7:0

#define NVA197_SET_MME_SHADOW_SCRATCH(i)                                                           (0x3400+(i)*4)
#define NVA197_SET_MME_SHADOW_SCRATCH_V                                                                      31:0

#define NVA197_CALL_MME_MACRO(j)                                                                   (0x3800+(j)*8)
#define NVA197_CALL_MME_MACRO_V                                                                              31:0

#define NVA197_CALL_MME_DATA(j)                                                                    (0x3804+(j)*8)
#define NVA197_CALL_MME_DATA_V                                                                               31:0

#endif /* _cl_kepler_b_h_ */
