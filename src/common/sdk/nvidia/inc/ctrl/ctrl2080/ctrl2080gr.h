/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080gr.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

#include "ctrl/ctrl0080/ctrl0080gr.h"        /* 2080 is partially derivative of 0080 */
#include "nvcfg_sdk.h"

/*
 * NV2080_CTRL_GR_ROUTE_INFO
 *
 * This structure specifies the routing information used to
 * disambiguate the target GR engine.
 *
 *   flags
 *     This field decides how the route field is interpreted
 *
 *   route
 *     This field has the data to identify target GR engine
 *
 */
#define NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE                 1:0
#define NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_NONE    0x0U
#define NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_ENGID   0x1U
#define NV2080_CTRL_GR_ROUTE_INFO_FLAGS_TYPE_CHANNEL 0x2U

#define NV2080_CTRL_GR_ROUTE_INFO_DATA_CHANNEL_HANDLE      31:0
#define NV2080_CTRL_GR_ROUTE_INFO_DATA_ENGID               31:0

typedef NV0080_CTRL_GR_ROUTE_INFO NV2080_CTRL_GR_ROUTE_INFO;

/* NV20_SUBDEVICE_XX gr control commands and parameters */

/*
 * NV2080_CTRL_GR_INFO
 *
 * This structure represents a single 32bit gr engine value.  Clients
 * request a particular gr engine value by specifying a unique gr
 * information index.
 *
 * Legal gr information index values are:
 *   NV2080_CTRL_GR_INFO_INDEX_BUFFER_ALIGNMENT
 *     This index is used to request the surface buffer alignment (in bytes)
 *     required by the associated subdevice.  The return value is GPU
 *     implementation-dependent.
 *   NV2080_CTRL_GR_INFO_INDEX_SWIZZLE_ALIGNMENT
 *     This index is used to request the required swizzled surface alignment
 *     (in bytes) supported by the associated subdevice.  The return value
 *     is GPU implementation-dependent.  A return value of 0 indicates the GPU
 *     does not support swizzled surfaces.
 *   NV2080_CTRL_GR_INFO_INDEX_VERTEX_CACHE_SIZE
 *     This index is used to request the vertex cache size (in entries)
 *     supported by the associated subdevice.  The return value is GPU
 *     implementation-dependent.  A value of 0 indicates the GPU does
 *     have a vertex cache.
 *   NV2080_CTRL_GR_INFO_INDEX_VPE_COUNT
 *     This index is used to request the number of VPE units supported by the
 *     associated subdevice.  The return value is GPU implementation-dependent.
 *     A return value of 0 indicates the GPU does not contain VPE units.
 *   NV2080_CTRL_GR_INFO_INDEX_SHADER_PIPE_COUNT
 *     This index is used to request the number of shader pipes supported by
 *     the associated subdevice.  The return value is GPU
 *     implementation-dependent.  A return value of 0 indicates the GPU does
 *     not contain dedicated shader units.
 *     For tesla: this value is the number of enabled TPCs
 *   NV2080_CTRL_GR_INFO_INDEX_SHADER_PIPE_SUB_COUNT
 *     This index is used to request the number of sub units per
 *     shader pipes supported by the associated subdevice.  The return
 *     value is GPU implementation-dependent.  A return value of 0 indicates
 *     the GPU does not contain dedicated shader units.
 *     For tesla: this value is the number of enabled SMs (per TPC)
 *   NV2080_CTRL_GR_INFO_INDEX_THREAD_STACK_SCALING_FACTOR
 *     This index is used to request the scaling factor for thread stack
 *     memory.
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_SM_REG_BANK_COUNT
 *     This index is used to request the number of SM register banks supported.
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_SM_REG_BANK_REG_COUNT
 *     This index is used to request the number of registers per SM register
 *     bank. A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_SM_VERSION
 *     This index is used to determine the SM version.
 *     A value of 0 indicates the GPU does not support this function.
 *     Otherwise one of NV2080_CTRL_GR_INFO_SM_VERSION_*.
 *   NV2080_CTRL_GR_INFO_INDEX_MAX_WARPS_PER_SM
 *     This index is used to determine the maximum number of warps
 *     (thread groups) per SM.
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_MAX_THREADS_PER_WARP
 *     This index is used to determine the maximum number of threads
 *     in each warp (thread group).
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_FB_MEMORY_REQUEST_GRANULARITY
 *     This index is used to request the default fb memory read/write request
 *     size in bytes (typically based on the memory configuration/controller).
 *     Smaller memory requests are likely to take as long as a full one.
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_HOST_MEMORY_REQUEST_GRANULARITY
 *     This index is used to request the default host memory read/write request
 *     size in bytes (typically based on the memory configuration/controller).
 *     Smaller memory requests are likely to take as long as a full one.
 *     A value of 0 indicates the GPU does not support this function.
 *   NV2080_CTRL_GR_INFO_INDEX_MAX_SP_PER_SM
 *     This index is used to request the maximum number of streaming processors
 *     per SM.
 *   NV2080_CTRL_GR_INFO_INDEX_LITTER_*
 *     This index is used to query the various LITTER size information from
 *     the chip.
 *   NV2080_CTRL_GR_INFO_INDEX_TIMESLICE_ENABLED
 *     This index is used to query whether the chip has timeslice mode enabled.
 *   NV2080_CTRL_GR_INFO_INDEX_GPU_CORE_COUNT
 *     This index is used to return the number of "GPU Cores"
 *     supported by the graphics pipeline
 *   NV2080_CTRL_GR_INFO_INDEX_RT_CORE_COUNT
 *     This index is used to return the number of "Ray Tracing Cores"
 *     supported by the graphics pipeline
 *    NV2080_CTRL_GR_INFO_INDEX_TENSOR_CORE_COUNT
 *     This index is used to return the number of "Tensor Cores"
 *     supported by the graphics pipeline
 *    NV2080_CTRL_GR_INFO_INDEX_GFX_CAPABILITIES
 *     This index is used to return the Graphics capabilities
 *     supported by the graphics pipeline
 */
typedef NV0080_CTRL_GR_INFO NV2080_CTRL_GR_INFO;

/*
 * Valid GR info index values
 * These indices are offset from supporting the 0080 version of this call
 */
#define NV2080_CTRL_GR_INFO_INDEX_MAXCLIPS                              NV0080_CTRL_GR_INFO_INDEX_MAXCLIPS
#define NV2080_CTRL_GR_INFO_INDEX_MIN_ATTRS_BUG_261894                  NV0080_CTRL_GR_INFO_INDEX_MIN_ATTRS_BUG_261894
#define NV2080_CTRL_GR_INFO_XBUF_MAX_PSETS_PER_BANK                     NV0080_CTRL_GR_INFO_XBUF_MAX_PSETS_PER_BANK
/**
 * This index is used to request the surface buffer alignment (in bytes)
 * required by the associated subdevice.  The return value is GPU
 * implementation-dependent.
 */
#define NV2080_CTRL_GR_INFO_INDEX_BUFFER_ALIGNMENT                      NV0080_CTRL_GR_INFO_INDEX_BUFFER_ALIGNMENT
#define NV2080_CTRL_GR_INFO_INDEX_SWIZZLE_ALIGNMENT                     NV0080_CTRL_GR_INFO_INDEX_SWIZZLE_ALIGNMENT
#define NV2080_CTRL_GR_INFO_INDEX_VERTEX_CACHE_SIZE                     NV0080_CTRL_GR_INFO_INDEX_VERTEX_CACHE_SIZE
/**
 * This index is used to request the number of VPE units supported by the
 * associated subdevice.  The return value is GPU implementation-dependent.
 * A return value of 0 indicates the GPU does not contain VPE units.
 */
#define NV2080_CTRL_GR_INFO_INDEX_VPE_COUNT                             NV0080_CTRL_GR_INFO_INDEX_VPE_COUNT
/**
 * This index is used to request the number of shader pipes supported by
 * the associated subdevice.  The return value is GPU
 * implementation-dependent.  A return value of 0 indicates the GPU does
 * not contain dedicated shader units.
 * For tesla: this value is the number of enabled TPCs
 */
#define NV2080_CTRL_GR_INFO_INDEX_SHADER_PIPE_COUNT                     NV0080_CTRL_GR_INFO_INDEX_SHADER_PIPE_COUNT
/**
 * This index is used to request the scaling factor for thread stack
 * memory.
 * A value of 0 indicates the GPU does not support this function.
 */
#define NV2080_CTRL_GR_INFO_INDEX_THREAD_STACK_SCALING_FACTOR           NV0080_CTRL_GR_INFO_INDEX_THREAD_STACK_SCALING_FACTOR
/**
 * This index is used to request the number of sub units per
 * shader pipes supported by the associated subdevice.  The return
 * value is GPU implementation-dependent.  A return value of 0 indicates
 * the GPU does not contain dedicated shader units.
 * For tesla: this value is the number of enabled SMs (per TPC)
 */
#define NV2080_CTRL_GR_INFO_INDEX_SHADER_PIPE_SUB_COUNT                 NV0080_CTRL_GR_INFO_INDEX_SHADER_PIPE_SUB_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_SM_REG_BANK_COUNT                     NV0080_CTRL_GR_INFO_INDEX_SM_REG_BANK_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_SM_REG_BANK_REG_COUNT                 NV0080_CTRL_GR_INFO_INDEX_SM_REG_BANK_REG_COUNT
/**
 * This index is used to determine the SM version.
 * A value of 0 indicates the GPU does not support this function.
 * Otherwise one of NV2080_CTRL_GR_INFO_SM_VERSION_*.
 */
#define NV2080_CTRL_GR_INFO_INDEX_SM_VERSION                            NV0080_CTRL_GR_INFO_INDEX_SM_VERSION
/**
 * This index is used to determine the maximum number of warps
 * (thread groups) per SM.
 * A value of 0 indicates the GPU does not support this function.
 */
#define NV2080_CTRL_GR_INFO_INDEX_MAX_WARPS_PER_SM                      NV0080_CTRL_GR_INFO_INDEX_MAX_WARPS_PER_SM
/**
 * This index is used to determine the maximum number of threads
 * in each warp (thread group).
 * A value of 0 indicates the GPU does not support this function.
 */
#define NV2080_CTRL_GR_INFO_INDEX_MAX_THREADS_PER_WARP                  NV0080_CTRL_GR_INFO_INDEX_MAX_THREADS_PER_WARP
#define NV2080_CTRL_GR_INFO_INDEX_GEOM_GS_OBUF_ENTRIES                  NV0080_CTRL_GR_INFO_INDEX_GEOM_GS_OBUF_ENTRIES
#define NV2080_CTRL_GR_INFO_INDEX_GEOM_XBUF_ENTRIES                     NV0080_CTRL_GR_INFO_INDEX_GEOM_XBUF_ENTRIES
#define NV2080_CTRL_GR_INFO_INDEX_FB_MEMORY_REQUEST_GRANULARITY         NV0080_CTRL_GR_INFO_INDEX_FB_MEMORY_REQUEST_GRANULARITY
#define NV2080_CTRL_GR_INFO_INDEX_HOST_MEMORY_REQUEST_GRANULARITY       NV0080_CTRL_GR_INFO_INDEX_HOST_MEMORY_REQUEST_GRANULARITY
#define NV2080_CTRL_GR_INFO_INDEX_MAX_SP_PER_SM                         NV0080_CTRL_GR_INFO_INDEX_MAX_SP_PER_SM
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS                       NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPS                       NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_ZCULL_BANKS                NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_ZCULL_BANKS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPC_PER_GPC                NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPC_PER_GPC
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_MIN_FBPS                   NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MIN_FBPS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_FBP_PORTS            NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_FBP_PORTS
#define NV2080_CTRL_GR_INFO_INDEX_TIMESLICE_ENABLED                     NV0080_CTRL_GR_INFO_INDEX_TIMESLICE_ENABLED
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPAS                      NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPAS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_PES_PER_GPC                NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_PES_PER_GPC
#define NV2080_CTRL_GR_INFO_INDEX_GPU_CORE_COUNT                        NV0080_CTRL_GR_INFO_INDEX_GPU_CORE_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPCS_PER_PES               NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPCS_PER_PES
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_HUB_PORTS            NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_HUB_PORTS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_SM_PER_TPC                 NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_SM_PER_TPC
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_HSHUB_FBP_PORTS            NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_HSHUB_FBP_PORTS
/**
 * This index is used to return the number of "Ray Tracing Cores"
 * supported by the graphics pipeline
 */
#define NV2080_CTRL_GR_INFO_INDEX_RT_CORE_COUNT                         NV0080_CTRL_GR_INFO_INDEX_RT_CORE_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_TENSOR_CORE_COUNT                     NV0080_CTRL_GR_INFO_INDEX_TENSOR_CORE_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GRS                        NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GRS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTCS                       NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTCS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_SLICES                 NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_SLICES
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCMMU_PER_GPC             NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCMMU_PER_GPC
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_PER_FBP                NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_PER_FBP
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_ROP_PER_GPC                NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_ROP_PER_GPC
#define NV2080_CTRL_GR_INFO_INDEX_FAMILY_MAX_TPC_PER_GPC                NV0080_CTRL_GR_INFO_INDEX_FAMILY_MAX_TPC_PER_GPC
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPA_PER_FBP               NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPA_PER_FBP
#define NV2080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT                  NV0080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_MAX_LEGACY_SUBCONTEXT_COUNT           NV0080_CTRL_GR_INFO_INDEX_MAX_LEGACY_SUBCONTEXT_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_MAX_PER_ENGINE_SUBCONTEXT_COUNT       NV0080_CTRL_GR_INFO_INDEX_MAX_PER_ENGINE_SUBCONTEXT_COUNT
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_SINGLETON_GPCS             NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_SINGLETON_GPCS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_GPCS                  NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_GPCS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_TPCS_PER_GFXC_GPC     NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_TPCS_PER_GFXC_GPC
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_SLICES_PER_LTC             NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_SLICES_PER_LTC

#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_SMC_ENGINES           NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_SMC_ENGINES


#define NV2080_CTRL_GR_INFO_INDEX_DUMMY                                 NV0080_CTRL_GR_INFO_INDEX_DUMMY
#define NV2080_CTRL_GR_INFO_INDEX_GFX_CAPABILITIES                      NV0080_CTRL_GR_INFO_INDEX_GFX_CAPABILITIES
#define NV2080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES                       NV0080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES
#define NV2080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS                NV0080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_MIN_SUBCTX_PER_SMC_ENG         NV0080_CTRL_GR_INFO_INDEX_LITTER_MIN_SUBCTX_PER_SMC_ENG
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS_PER_DIELET            NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS_PER_DIELET
#define NV2080_CTRL_GR_INFO_INDEX_LITTER_MAX_NUM_SMC_ENGINES_PER_DIELET NV0080_CTRL_GR_INFO_INDEX_LITTER_MAX_NUM_SMC_ENGINES_PER_DIELET

/* When adding a new INDEX, please update INDEX_MAX and MAX_SIZE accordingly
 * NOTE: 0080 functionality is merged with 2080 functionality, so this max size
 * reflects that.
 */
#define NV2080_CTRL_GR_INFO_INDEX_MAX                                   NV0080_CTRL_GR_INFO_INDEX_MAX
#define NV2080_CTRL_GR_INFO_MAX_SIZE                                    NV0080_CTRL_GR_INFO_MAX_SIZE

/* valid SM version return values */

#define NV2080_CTRL_GR_INFO_SM_VERSION_NONE                             (0x00000000U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_05                             (0x00000105U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_1                              (0x00000110U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_2                              (0x00000120U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_3                              (0x00000130U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_4                              (0x00000140U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_1_5                              (0x00000150U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_2_0                              (0x00000200U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_2_1                              (0x00000210U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_2_2                              (0x00000220U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_0                              (0x00000300U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_1                              (0x00000310U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_2                              (0x00000320U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_3                              (0x00000330U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_5                              (0x00000350U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_6                              (0x00000360U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_8                              (0x00000380U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_3_9                              (0x00000390U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_4_0                              (0x00000400U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_5_0                              (0x00000500U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_5_02                             (0x00000502U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_5_03                             (0x00000503U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_6_0                              (0x00000600U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_6_01                             (0x00000601U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_6_02                             (0x00000602U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_0                              (0x00000700U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_01                             (0x00000701U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_02                             (0x00000702U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_03                             (0x00000703U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_05                             (0x00000705U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_02                             (0x00000802U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_06                             (0x00000806U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_07                             (0x00000807U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_08                             (0x00000808U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_09                             (0x00000809U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_9_00                             (0x00000900U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_10_00                            (0x00000A00U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_10_01                            (0x00000A01U)


#define NV2080_CTRL_GR_INFO_SM_VERSION_10_03                            (0x00000A03U)


/*
 * TODO Bug 4333440 is introducing versions 12_*.
 * Eventually once 12_* is tested and validated, another
 * follow up change will be needed to remove 10_04 support.
 */
#define NV2080_CTRL_GR_INFO_SM_VERSION_10_04                            (0x00000A04U)
#define NV2080_CTRL_GR_INFO_SM_VERSION_12_00                            (0x00000C00U)


#define NV2080_CTRL_GR_INFO_SM_VERSION_12_01                            (0x00000C01U)



/* compatibility SM versions to match the official names in the ISA (e.g., SM5.2)  */
#define NV2080_CTRL_GR_INFO_SM_VERSION_5_2                              (NV2080_CTRL_GR_INFO_SM_VERSION_5_02)
#define NV2080_CTRL_GR_INFO_SM_VERSION_5_3                              (NV2080_CTRL_GR_INFO_SM_VERSION_5_03)
#define NV2080_CTRL_GR_INFO_SM_VERSION_6_1                              (NV2080_CTRL_GR_INFO_SM_VERSION_6_01)
#define NV2080_CTRL_GR_INFO_SM_VERSION_6_2                              (NV2080_CTRL_GR_INFO_SM_VERSION_6_02)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_1                              (NV2080_CTRL_GR_INFO_SM_VERSION_7_01)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_2                              (NV2080_CTRL_GR_INFO_SM_VERSION_7_02)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_3                              (NV2080_CTRL_GR_INFO_SM_VERSION_7_03)
#define NV2080_CTRL_GR_INFO_SM_VERSION_7_5                              (NV2080_CTRL_GR_INFO_SM_VERSION_7_05)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_2                              (NV2080_CTRL_GR_INFO_SM_VERSION_8_02)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_6                              (NV2080_CTRL_GR_INFO_SM_VERSION_8_06)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_7                              (NV2080_CTRL_GR_INFO_SM_VERSION_8_07)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_8                              (NV2080_CTRL_GR_INFO_SM_VERSION_8_08)
#define NV2080_CTRL_GR_INFO_SM_VERSION_8_9                              (NV2080_CTRL_GR_INFO_SM_VERSION_8_09)
#define NV2080_CTRL_GR_INFO_SM_VERSION_9_0                              (NV2080_CTRL_GR_INFO_SM_VERSION_9_00)
#define NV2080_CTRL_GR_INFO_SM_VERSION_10_0                             (NV2080_CTRL_GR_INFO_SM_VERSION_10_00)
#define NV2080_CTRL_GR_INFO_SM_VERSION_10_1                             (NV2080_CTRL_GR_INFO_SM_VERSION_10_01)


#define NV2080_CTRL_GR_INFO_SM_VERSION_10_3                             (NV2080_CTRL_GR_INFO_SM_VERSION_10_03)


#define NV2080_CTRL_GR_INFO_SM_VERSION_10_4                             (NV2080_CTRL_GR_INFO_SM_VERSION_10_04)



#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_2D            0:0
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_2D_FALSE                   0x0U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_2D_TRUE                    0x1U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_3D            1:1
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_3D_FALSE                   0x0U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_3D_TRUE                    0x1U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_COMPUTE       2:2
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_COMPUTE_FALSE              0x0U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_COMPUTE_TRUE               0x1U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_I2M           3:3
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_I2M_FALSE                  0x0U
#define NV2080_CTRL_GR_INFO_GFX_CAPABILITIES_I2M_TRUE                   0x1U

/**
 * NV2080_CTRL_CMD_GR_GET_INFO
 *
 * This command returns gr engine information for the associated GPU.
 * Requests to retrieve gr information use a list of one or more
 * NV2080_CTRL_GR_INFO structures.
 *
 *   grInfoListSize
 *     This field specifies the number of entries on the caller's
 *     grInfoList.
 *   grInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the gr information is to be returned.
 *     This buffer must be at least as big as grInfoListSize multiplied
 *     by the size of the NV2080_CTRL_GR_INFO structure.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine. When MIG is enabled, this
 *     is a mandatory parameter.
 */
#define NV2080_CTRL_CMD_GR_GET_INFO                                     (0x20801201U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_GR_GET_INFO_PARAMS {
    NvU32 grInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 grInfoList, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_GET_INFO_PARAMS;



/*
 * NV2080_CTRL_CMD_GR_CTXSW_ZCULL_MODE
 *
 * This command is used to set the zcull context switch mode for the specified
 * channel. A value of NV_ERR_NOT_SUPPORTED is returned if the
 * target channel does not support zcull context switch mode changes.
 *
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have it's zcull context switch mode changed.
 *   hShareClient
 *     Support for sharing zcull buffers across RM clients is no longer
 *     supported.  To maintain API compatibility, this field must match
 *     the hClient used in the control call.
 *   hShareChannel
 *     This parameter specifies the channel handle of
 *     the channel with which the zcull context buffer is to be shared.  This
 *     parameter is valid when zcullMode is set to SEPARATE_BUFFER.  This
 *     parameter should be set to the same value as hChannel if no
 *     sharing is intended.
 *   zcullMode
 *     This parameter specifies the new zcull context switch mode.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_GLOBAL
 *         This mode is the normal zcull operation where it is not
 *         context switched and there is one set of globally shared
 *         zcull memory and tables.  This mode is only supported as
 *         long as all channels use this mode.
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_NO_CTXSW
 *         This mode causes the zcull tables to be reset on a context
 *         switch, but the zcull buffer will not be saved/restored.
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_SEPARATE_BUFFER
 *         This mode will cause the zcull buffers and tables to be
 *         saved/restored on context switches.  If a share channel
 *         ID is given (shareChID), then the 2 channels will share
 *         the zcull context buffers.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_ZCULL_MODE (0x20801205U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS {
    NvHandle hChannel;
    NvHandle hShareClient;
    NvHandle hShareChannel;
    NvU32    zcullMode;
} NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS;
/* valid zcullMode values */
#define NV2080_CTRL_CTXSW_ZCULL_MODE_GLOBAL          (0x00000000U)
#define NV2080_CTRL_CTXSW_ZCULL_MODE_NO_CTXSW        (0x00000001U)
#define NV2080_CTRL_CTXSW_ZCULL_MODE_SEPARATE_BUFFER (0x00000002U)

/**
 * NV2080_CTRL_CMD_GR_GET_ZCULL_INFO
 *
 * This command is used to query the RM for zcull information that the
 * driver will need to allocate and manage the zcull regions.
 *
 *   widthAlignPixels
 *     This parameter returns the width alignment restrictions in pixels
 *     used to adjust a surface for proper aliquot coverage (typically
 *     #TPC's * 16).
 *
 *   heightAlignPixels
 *     This parameter returns the height alignment restrictions in pixels
 *     used to adjust a surface for proper aliquot coverage (typically 32).
 *
 *   pixelSquaresByAliquots
 *     This parameter returns the pixel area covered by an aliquot
 *     (typically #Zcull_banks * 16 * 16).
 *
 *   aliquotTotal
 *     This parameter returns the total aliquot pool available in HW.
 *
 *   zcullRegionByteMultiplier
 *     This parameter returns multiplier used to convert aliquots in a region
 *     to the number of bytes required to save/restore them.
 *
 *   zcullRegionHeaderSize
 *     This parameter returns the region header size which is required to be
 *     allocated and accounted for in any save/restore operation on a region.
 *
 *   zcullSubregionHeaderSize
 *     This parameter returns the subregion header size which is required to be
 *     allocated and accounted for in any save/restore operation on a region.
 *
 *   subregionCount
 *     This parameter returns the subregion count.
 *
 *   subregionWidthAlignPixels
 *     This parameter returns the subregion width alignment restrictions in
 *     pixels used to adjust a surface for proper aliquot coverage
 *     (typically #TPC's * 16).
 *
 *   subregionHeightAlignPixels
 *     This parameter returns the subregion height alignment restrictions in
 *     pixels used to adjust a surface for proper aliquot coverage
 *     (typically 62).
 *
 *   The callee should compute the size of a zcull region as follows.
 *     (numBytes = aliquots * zcullRegionByteMultiplier +
 *                 zcullRegionHeaderSize + zcullSubregionHeaderSize)
 */
#define NV2080_CTRL_CMD_GR_GET_ZCULL_INFO            (0x20801206U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_SUBREGION_SUPPORTED
#define NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS {
    NvU32 widthAlignPixels;
    NvU32 heightAlignPixels;
    NvU32 pixelSquaresByAliquots;
    NvU32 aliquotTotal;
    NvU32 zcullRegionByteMultiplier;
    NvU32 zcullRegionHeaderSize;
    NvU32 zcullSubregionHeaderSize;
    NvU32 subregionCount;
    NvU32 subregionWidthAlignPixels;
    NvU32 subregionHeightAlignPixels;
} NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_CTXSW_PM_MODE
 *
 * This command is used to set the pm context switch mode for the specified
 * channel. A value of NV_ERR_NOT_SUPPORTED is returned if the
 * target channel does not support pm context switch mode changes.
 *
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have its pm context switch mode changed.
 *   pmMode
 *     This parameter specifies the new pm context switch mode.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_CTXSW_PM_MODE_NO_CTXSW
 *         This mode says that the pms are not to be context switched.
 *       NV2080_CTRL_CTXSW_PM_MODE_CTXSW
 *         This mode says that the pms in Mode-B are to be context switched.
 *       NV2080_CTRL_CTXSW_PM_MODE_STREAM_OUT_CTXSW
 *         This mode says that the pms in Mode-E (stream out) are to be context switched.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_PM_MODE (0x20801207U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS {
    NvHandle hChannel;
    NvU32    pmMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS;

/* valid pmMode values */
#define NV2080_CTRL_CTXSW_PM_MODE_NO_CTXSW         (0x00000000U)
#define NV2080_CTRL_CTXSW_PM_MODE_CTXSW            (0x00000001U)
#define NV2080_CTRL_CTXSW_PM_MODE_STREAM_OUT_CTXSW (0x00000002U)

/*
 * NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND
 *
 * This command is used to set the zcull context switch mode and virtual address
 * for the specified channel. A value of NV_ERR_NOT_SUPPORTED is
 * returned if the target channel does not support zcull context switch mode
 * changes.
 *
 *   hClient
 *     This parameter specifies the client handle of
 *     that owns the zcull context buffer. This field must match
 *     the hClient used in the control call for non-kernel clients.
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have its zcull context switch mode changed.
 *   vMemPtr
 *     This parameter specifies the 64 bit virtual address
 *     for the allocated zcull context buffer.
 *   zcullMode
 *     This parameter specifies the new zcull context switch mode.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_GLOBAL
 *         This mode is the normal zcull operation where it is not
 *         context switched and there is one set of globally shared
 *         zcull memory and tables.  This mode is only supported as
 *         long as all channels use this mode.
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_NO_CTXSW
 *         This mode causes the zcull tables to be reset on a context
 *         switch, but the zcull buffer will not be saved/restored.
 *       NV2080_CTRL_GR_SET_CTXSW_ZCULL_MODE_SEPARATE_BUFFER
 *         This mode will cause the zcull buffers and tables to be
 *         saved/restored on context switches.  If a share channel
 *         ID is given (shareChID), then the 2 channels will share
 *         the zcull context buffers.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND        (0x20801208U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS {
    NvHandle hClient;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 vMemPtr, 8);
    NvU32    zcullMode;
} NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS;
/* valid zcullMode values same as above NV2080_CTRL_CTXSW_ZCULL_MODE */

/*
 * NV2080_CTRL_CMD_GR_CTXSW_PM_BIND
 *
 * This command is used to set the PM context switch mode and virtual address
 * for the specified channel. A value of NV_ERR_NOT_SUPPORTED is
 * returned if the target channel does not support PM context switch mode
 * changes.
 *
 *   hClient
 *     This parameter specifies the client handle of
 *     that owns the PM context buffer.
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have its PM context switch mode changed.
 *   vMemPtr
 *     This parameter specifies the 64 bit virtual address
 *     for the allocated PM context buffer.
 *   pmMode
 *     This parameter specifies the new PM context switch mode.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_GR_SET_CTXSW_PM_MODE_NO_CTXSW
 *         This mode says that the pms are not to be context switched
 *       NV2080_CTRL_GR_SET_CTXSW_PM_MODE_CTXSW
 *         This mode says that the pms are to be context switched
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_PM_BIND (0x20801209U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS {
    NvHandle hClient;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 vMemPtr, 8);
    NvU32    pmMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS;
/* valid pmMode values same as above NV2080_CTRL_CTXSW_PM_MODE */

/*
 * NV2080_CTRL_CMD_GR_CTXSW_SETUP_BIND
 *
 * This command is used to set the Setup context switch mode and virtual address
 * for the specified channel. A value of NV_ERR_NOT_SUPPORTED is
 * returned if the target channel does not support setup context switch mode
 * changes.
 *
 *   hClient
 *     This parameter specifies the client handle of
 *     that owns the Setup context buffer. This field must match
 *     the hClient used in the control call for non-kernel clients.
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have its Setup context switch mode changed.
 *   vMemPtr
 *     This parameter specifies the 64 bit virtual address
 *     for the allocated Setup context buffer.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_SETUP_BIND (0x2080123aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS_MESSAGE_ID (0x3AU)

typedef struct NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS {
    NvHandle hClient;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 vMemPtr, 8);
} NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_SET_GPC_TILE_MAP
 *
 * Send a list of values used to describe GPC/TPC tile mapping tables.
 *
 *   mapValueCount
 *     This field specifies the number of actual map entries.  This count
 *     should equal the number of TPCs in the system.
 *   mapValues
 *     This field is a pointer to a buffer of NvU08 values representing map
 *     data.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_GR_SET_GPC_TILE_MAP_MAX_VALUES 128U
#define NV2080_CTRL_CMD_GR_SET_GPC_TILE_MAP        (0x2080120aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS {
    NvU32 mapValueCount;
    NvU8  mapValues[NV2080_CTRL_GR_SET_GPC_TILE_MAP_MAX_VALUES];
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS;



/*
 * NV2080_CTRL_CMD_GR_CTXSW_SMPC_MODE
 *
 * This command is used to set the SMPC context switch mode for the specified
 * channel or channel group (TSG). A value of NV_ERR_NOT_SUPPORTED
 * is returned if the target channel/TSG does not support SMPC context switch
 * mode changes.  If a channel is part of a TSG, the user must send in the TSG
 * handle and not an individual channel handle, an error will be returned if a
 * channel handle is used in this case.
 *
 * SMPC = SM Performance Counters
 *
 *   hChannel
 *     This parameter specifies the channel or channel group (TSG) handle
 *     that is to have its SMPC context switch mode changed.
 *     If this parameter is set to 0, then the mode below applies to all current
 *     and future channels (i.e. we will be enabling/disabling global mode)
 *   smpcMode
 *     This parameter specifies the new SMPC context switch mode.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_GR_SET_CTXSW_SMPC_MODE_NO_CTXSW
 *         This mode says that the SMPC data is not to be context switched.
 *       NV2080_CTRL_GR_SET_CTXSW_SMPC_MODE_CTXSW
 *         This mode says that the SMPC data is to be context switched.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_SMPC_MODE (0x2080120eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS {
    NvHandle hChannel;
    NvU32    smpcMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS;

/* valid smpcMode values */
#define NV2080_CTRL_CTXSW_SMPC_MODE_NO_CTXSW                   (0x00000000U)
#define NV2080_CTRL_CTXSW_SMPC_MODE_CTXSW                      (0x00000001U)

/*
 * NV2080_CTRL_CMD_GR_GET_SM_TO_GPC_TPC_MAPPINGS
 *
 * This command returns an array of the mappings between SMs and GPC/TPCs.
 *
 *   smId
 *     An array of the mappings between SMs and GPC/TPCs.
 *   smCount
 *     Returns the number of valid mappings in the array.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_GET_SM_TO_GPC_TPC_MAPPINGS          (0x2080120fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_MAX_SM_COUNT 240U
#define NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS_MESSAGE_ID (0xFU)

typedef struct NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS {
    struct {
        NvU32 gpcId;
        NvU32 tpcId;
    } smId[NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_MAX_SM_COUNT];
    NvU32 smCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_SET_CTXSW_PREEMPTION_MODE
 *
 * This command is used to set the preemption context switch mode for the specified
 * channel. A value of NV_ERR_NOT_SUPPORTED is returned if the
 * target channel does not support preemption context switch mode changes.
 *
 *   flags
 *     This field specifies flags for the preemption mode changes.
 *     These flags can tell callee which mode is valid in the call
 *     since we handle graphics and/or compute
 *   hChannel
 *     This parameter specifies the channel handle of the channel
 *     that is to have it's preemption context switch mode set.
 *   gfxpPreemptMode
 *     This parameter specifies the new Graphics preemption context switch
 *     mode. Legal values for this parameter include:
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_WFI
 *         This mode is the normal wait-for-idle context switch mode.
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_GFXP
 *         This mode causes the graphics engine to allow preempting the
 *         channel mid-triangle.
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_GFXP_POOL
 *         This mode causes the graphics engine to use a shared pool of buffers
 *         to support GfxP with lower memory overhead
 *   cilpPreemptMode
 *     This parameter specifies the new Compute preemption context switch
 *     mode. Legal values for this parameter include:
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_WFI
 *         This mode is the normal wait-for-idle context switch mode.
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_CTA
 *         This mode causes the compute engine to allow preempting the channel
 *         at the instruction level.
 *       NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_CILP
 *         This mode causes the compute engine to allow preempting the channel
 *         at the instruction level.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_SET_CTXSW_PREEMPTION_MODE (0x20801210U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS {
    NvU32    flags;
    NvHandle hChannel;
    NvU32    gfxpPreemptMode;
    NvU32    cilpPreemptMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS;

/* valid preemption flags */
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_CILP                0:0
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_CILP_IGNORE (0x00000000U)
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_CILP_SET    (0x00000001U)
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_GFXP                1:1
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_GFXP_IGNORE (0x00000000U)
#define NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_FLAGS_GFXP_SET    (0x00000001U)

/* valid Graphics mode values */
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_WFI              (0x00000000U)
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_GFXP             (0x00000001U)
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_GFXP_POOL        (0x00000002U)

/* valid Compute mode values */
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_WFI          (0x00000000U)
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_CTA          (0x00000001U)
#define NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_COMPUTE_CILP         (0x00000002U)

/* valid preemption buffers */
typedef enum NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS {
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_MAIN = 0,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_SPILL = 1,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_PAGEPOOL = 2,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_BETACB = 3,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_RTV = 4,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL = 5,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL_CONTROL = 6,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL_CONTROL_CPU = 7,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_SETUP = 8,
    NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_END = 9,
} NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS;

/*
 * NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND
 *
 * This command is used to set the preemption context switch mode and virtual
 * addresses of the preemption buffers for the specified channel. A value of
 * NV_ERR_NOT_SUPPORTED is returned if the target channel does not
 * support preemption context switch mode changes.
 *
 *   flags
 *     This field specifies flags for the preemption mode changes.
 *     These flags can tell callee which mode is valid in the call
 *     since we handle graphics and/or compute
 *   hClient
 *     This parameter specifies the client handle of
 *     that owns the preemption context buffer.
 *   hChannel
 *     This parameter specifies the channel handle of the channel
 *     that is to have its preemption context switch mode set.
 *   vMemPtr
 *     This parameter specifies the 64 bit virtual address
 *     for the allocated preemption context buffer.
 *   gfxpPreemptMode
 *     This parameter specifies the new Graphics preemption context switch
 *     mode. Legal values for this parameter include:
 *       NV2080_CTRL_CTXSW_PREEMPTION_MODE_GFX_WFI
 *         This mode is the normal wait-for-idle context switch mode.
 *       NV2080_CTRL_CTXSW_PREEMPTION_MODE_GFX_GFXP
 *         This mode causes the graphics engine to allow preempting the
 *         channel mid-triangle.
 *   cilpPreemptMode
 *     This parameter specifies the new Compute preemption context switch
 *     mode. Legal values for this parameter include:
 *       NV2080_CTRL_CTXSW_PREEMPTION_MODE_COMPUTE_WFI
 *         This mode is the normal wait-for-idle context switch mode.
 *       NV2080_CTRL_CTXSW_PREEMPTION_MODE_COMPUTE_CTA
 *         This mode causes the compute engine to allow preempting the channel
 *         at the instruction level.
 *       NV2080_CTRL_CTXSW_PREEMPTION_MODE_COMPUTE_CILP
 *         This mode causes the compute engine to allow preempting the channel
 *         at the instruction level.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND (0x20801211U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS {
    NvU32    flags;
    NvHandle hClient;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 vMemPtrs[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_END], 8);
    NvU32    gfxpPreemptMode;
    NvU32    cilpPreemptMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS;
/* valid mode and flag values same as above NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE */

/*
 * NV2080_CTRL_CMD_GR_PC_SAMPLING_MODE
 *
 * This command is used to apply the WAR for PC sampling to avoid hang in
 * multi-ctx scenario.
 *
 *   hChannel
 *     This parameter specifies the channel or channel group (TSG) handle
 *     that is to have its PC Sampling mode changed.
 *   samplingMode
 *     This parameter specifies whether sampling is turned ON or OFF.
 *     Legal values for this parameter include:
 *       NV2080_CTRL_GR_SET_PC_SAMPLING_MODE_DISABLED
 *         This mode says that PC sampling is disabled for current context.
 *       NV2080_CTRL_GR_SET_PC_SAMPLING_MODE_ENABLED
 *         This mode says that PC sampling is disabled for current context.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_PC_SAMPLING_MODE (0x20801212U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS {
    NvHandle hChannel;
    NvU32    samplingMode;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS;

/* valid samplingMode values */
#define NV2080_CTRL_PC_SAMPLING_MODE_DISABLED (0x00000000U)
#define NV2080_CTRL_PC_SAMPLING_MODE_ENABLED  (0x00000001U)

/*
 * NV2080_CTRL_CMD_GR_GET_ROP_INFO
 *
 * Gets information about ROPs including the ROP unit count and information
 * about ROP operations per clock.
 *
 *   ropUnitCount
 *     The count of active ROP units.
 *   ropOperationsFactor.
 *     The number of ROP operations per clock for a single ROP unit.
 *   ropOperationsCount
 *     The number of ROP operations per clock across all active ROP units.
 */
#define NV2080_CTRL_CMD_GR_GET_ROP_INFO       (0x20801213U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_ROP_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_ROP_INFO_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_GR_GET_ROP_INFO_PARAMS {
    NvU32 ropUnitCount;
    NvU32 ropOperationsFactor;
    NvU32 ropOperationsCount;
} NV2080_CTRL_GR_GET_ROP_INFO_PARAMS;



/*
 * NV2080_CTRL_CMD_GR_GET_CTXSW_STATS
 *
 * This command is used to get the context switch statistics.  The user can
 * also add a flag to tell RM to reset the stats counters back to 0.
 *
 *   hChannel
 *     This parameter specifies the channel or channel group (TSG) handle
 *     that is to have the stats returned.  Note, must be the TSG handle if
 *     channel is part of a TSG.
 *   flags
 *     This parameter specifies processing flags. See possible flags below.
 *   saveCnt
 *     This parameter returns the number of saves on the channel.
 *   restoreCnt
 *     This parameter returns the number of restores on the channel.
 *   wfiSaveCnt
 *     This parameter returns the number of WFI saves on the channel.
 *   ctaSaveCnt
 *     This parameter returns the number of CTA saves on the channel.
 *   cilpSaveCnt
 *     This parameter returns the number of CILP saves on the channel.
 *   gfxpSaveCnt
 *     This parameter returns the number of GfxP saves on the channel.
 */
#define NV2080_CTRL_CMD_GR_GET_CTXSW_STATS (0x20801215U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS {
    NvHandle hChannel;
    NvU32    flags;
    NvU32    saveCnt;
    NvU32    restoreCnt;
    NvU32    wfiSaveCnt;
    NvU32    ctaSaveCnt;
    NvU32    cilpSaveCnt;
    NvU32    gfxpSaveCnt;
} NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS;
/* valid GET_CTXSW_STATS flags settings */
#define NV2080_CTRL_GR_GET_CTXSW_STATS_FLAGS_RESET                      0:0
#define NV2080_CTRL_GR_GET_CTXSW_STATS_FLAGS_RESET_FALSE (0x00000000U)
#define NV2080_CTRL_GR_GET_CTXSW_STATS_FLAGS_RESET_TRUE  (0x00000001U)



/*
 * NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_SIZE
 *
 * This command provides the size, alignment of all context buffers including global and
 * local context buffers which has been created & will be mapped on a context
 *
 *   hChannel [IN]
 *     This parameter specifies the channel or channel group (TSG) handle
 *   totalBufferSize [OUT]
 *     This parameter returns the total context buffers size.
 */
#define NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_SIZE (0x20801218U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS {
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 totalBufferSize, 8);
} NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS;

/*
 * NV2080_CTRL_GR_CTX_BUFFER_INFO
 *   alignment
 *     Specifies the alignment requirement for each context buffer
 *   size
 *     Aligned size of context buffer
 *   bufferHandle [deprecated]
 *     Opaque pointer to memdesc. Used by kernel clients for tracking purpose only.
 *   pageCount
 *     allocation size in the form of pageCount
 *   physAddr
 *     Physical address of the buffer first page
 *   bufferType
 *     NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID type of this buffer
 *   aperture
 *     allocation aperture. Could be SYSMEM, VIDMEM, UNKNOWN
 *   kind
 *     PTE kind of this allocation.
 *   pageSize
 *     Page size of the buffer.
 *   bIsContigous
 *     States if physical allocation for this buffer is contiguous. PageSize will
 *     have no meaning if this flag is set.
 *   bGlobalBuffer
 *     States if a defined buffer is global as global buffers need to be mapped
 *     only once in TSG.
 *   bLocalBuffer
 *     States if a buffer is local to a channel.
 *   bDeviceDescendant
 *     TRUE if the allocation is a constructed under a Device or Subdevice.
 *   uuid
 *     SHA1 UUID of the Device or Subdevice. Valid when deviceDescendant is TRUE.
 */
typedef struct NV2080_CTRL_GR_CTX_BUFFER_INFO {
    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvP64 bufferHandle, 8);
    NV_DECLARE_ALIGNED(NvU64 pageCount, 8);
    NV_DECLARE_ALIGNED(NvU64 physAddr, 8);
    NvU32  bufferType;
    NvU32  aperture;
    NvU32  kind;
    NvU32  pageSize;
    NvBool bIsContigous;
    NvBool bGlobalBuffer;
    NvBool bLocalBuffer;
    NvBool bDeviceDescendant;
    NvU8   uuid[16];
} NV2080_CTRL_GR_CTX_BUFFER_INFO;
typedef struct NV2080_CTRL_GR_CTX_BUFFER_INFO *PNV2080_CTRL_GR_CTX_BUFFER_INFO;

#define NV2080_CTRL_GR_MAX_CTX_BUFFER_COUNT    64U

/*
 * NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_INFO
 *
 * This command provides the size, alignment of all context buffers including global and
 * local context buffers which has been created & will be mapped on a context.
 * If the client invoking the command is a kernel client, the buffers are retained.
 *
 *   hUserClient [IN]
 *     This parameter specifies the client handle that owns this channel.
 *   hChannel [IN]
 *     This parameter specifies the channel or channel group (TSG) handle
 *   bufferCount [OUT]
 *     This parameter specifies the number of entries in ctxBufferInfo filled
 *     by the command.
 *   ctxBufferInfo [OUT]
 *     Array of context buffer info containing alignment, size etc.
 */
#define NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_INFO (0x20801219U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS {
    NvHandle hUserClient;
    NvHandle hChannel;
    NvU32    bufferCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTX_BUFFER_INFO ctxBufferInfo[NV2080_CTRL_GR_MAX_CTX_BUFFER_COUNT], 8);
} NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS;

// Aperture flags. The defines should match the defines in mem_desc.h
#define NV2080_CTRL_GR_CTX_BUFFER_INFO_APERTURE_UNKNOWN 0
#define NV2080_CTRL_GR_CTX_BUFFER_INFO_APERTURE_SYSMEM  1
#define NV2080_CTRL_GR_CTX_BUFFER_INFO_APERTURE_FBMEM   2

/*
 * NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER
 *     This command returns the global logical ordering of SM w.r.t GPCs/TPCs.
 *
 * NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS
 *     This structure holds the TPC/SM ordering info.
 *
 *     gpcId
 *         Logical GPC Id.
 *         This is the ordering of enabled GPCs post floor sweeping.
 *         The GPCs are numbered from 0 to N-1, where N is the enabled GPC count.
 *
 *     localTpcId
 *         Local Logical TPC Id.
 *         This is the ordering of enabled TPCs within a GPC post floor sweeping.
 *         This ID is used in conjunction with the gpcId.
 *         The TPCs are numbered from 0 to N-1, where N is the enabled TPC count for the given GPC.
 *
 *     localSmId
 *         Local Logical SM Id.
 *         This is the ordering of enabled SMs within a TPC post floor sweeping.
 *         This ID is used in conjunction with the localTpcId.
 *         The SMs are numbered from 0 to N-1, where N is the enabled SM count for the given TPC.
 *
 *     globalTpcId
 *         Global Logical TPC Id.
 *         This is the ordering of all enabled TPCs in the GPU post floor sweeping.
 *         The TPCs are numbered from 0 to N-1, where N is the enabled TPC count across all GPCs
 *
 *     globalSmId
 *         Global Logical SM Id array.
 *         This is the global ordering of all enabled SMs in the GPU post floor sweeping.
 *         The SMs are numbered from 0 to N-1, where N is the enabled SM count across all GPCs.
 *
 *     virtualGpcId
 *         Virtual GPC Id.
 *         This is the ordering of enabled GPCs post floor sweeping (ordered in increasing
 *         number of TPC counts) The GPCs are numbered from 0 to N-1, where N is the
 *         enabled GPC count and 8-23 for singleton TPC holders.
 *
 *     migratableTpcId
 *         Migratable TPC Id.
 *         This is the same as the Local Tpc Id for virtual GPC 0-8 (true physical gpcs) and 0 for
 *         virtual gpcs 8-23 that represent singleton tpcs.
 *
 *     numSm
 *         Enabled SM count across all GPCs.
 *         This represent the valid entries in the globalSmId array
 *
 *     numTpc
 *         Enabled TPC count across all GPCs.
 *
 *     grRouteInfo
 *         This parameter specifies the routing information used to
 *         disambiguate the target GR engine.
 *
 *     ugpuId
 *         Specifies the uGPU ID on Hopper+.
 *
 */
#define NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER              (0x2080121bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER_MAX_SM_COUNT 512U

#define NV2080_CTRL_GR_DISABLED_SM_VGPC_ID                  0xFFU

#define NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS_MESSAGE_ID (0x1BU)

typedef struct NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS {
    struct {
        NvU16 gpcId;
        NvU16 localTpcId;
        NvU16 localSmId;
        NvU16 globalTpcId;
        NvU16 virtualGpcId;
        NvU16 migratableTpcId;
        NvU16 ugpuId;
    } globalSmId[NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER_MAX_SM_COUNT];

    NvU16 numSm;
    NvU16 numTpc;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS;

/*
* NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL
*
* This command gives current resident channel on GR engine
*
*   chID [OUT]
*       RM returns current resident channel on GR engine
*   grRouteInfo [IN]
*       This parameter specifies the routing information used to
*       disambiguate the target GR engine.
*/
#define NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL (0x2080121cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS_MESSAGE_ID (0x1CU)

typedef struct NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS {
    NvU32 chID;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_VAT_ALARM_DATA
 *
 * This command provides the _VAT_ALARM data i.e. error and warning, counter and
 * timestamps along with max GPC and TPC per GPC count.
 *
 *   smVatAlarm [OUT]
 *     VAT Alarm data array per SM containing per GPC per TPC, counter and
 *      timestamp values for error and warning alarms.
 *   maxGpcCount [OUT]
 *     This parameter returns max GPC count.
 *   maxTpcPerGpcCount [OUT]
 *     This parameter returns the max TPC per GPC count.
 */
#define NV2080_CTRL_CMD_GR_GET_VAT_ALARM_MAX_GPC_COUNT         10U
#define NV2080_CTRL_CMD_GR_GET_VAT_ALARM_MAX_TPC_PER_GPC_COUNT 10U

#define NV2080_CTRL_CMD_GR_GET_VAT_ALARM_DATA                  (0x2080121dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_GR_VAT_ALARM_DATA_PER_TPC {
    NV_DECLARE_ALIGNED(NvU64 errorCounter, 8);
    NV_DECLARE_ALIGNED(NvU64 errorTimestamp, 8);
    NV_DECLARE_ALIGNED(NvU64 warningCounter, 8);
    NV_DECLARE_ALIGNED(NvU64 warningTimestamp, 8);
} NV2080_CTRL_GR_VAT_ALARM_DATA_PER_TPC;

typedef struct NV2080_CTRL_GR_VAT_ALARM_DATA_PER_GPC {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_VAT_ALARM_DATA_PER_TPC tpc[NV2080_CTRL_CMD_GR_GET_VAT_ALARM_MAX_TPC_PER_GPC_COUNT], 8);
} NV2080_CTRL_GR_VAT_ALARM_DATA_PER_GPC;

typedef struct NV2080_CTRL_GR_VAT_ALARM_DATA {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_VAT_ALARM_DATA_PER_GPC gpc[NV2080_CTRL_CMD_GR_GET_VAT_ALARM_MAX_GPC_COUNT], 8);
} NV2080_CTRL_GR_VAT_ALARM_DATA;

#define NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS_MESSAGE_ID (0x1DU)

typedef struct NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_VAT_ALARM_DATA smVatAlarm, 8);
    NvU32 maxGpcCount;
    NvU32 maxTpcPerGpcCount;
} NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS;
typedef struct NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *PNV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_ATTRIBUTE_BUFFER_SIZE
 *
 * This command provides the size of GR attribute buffer.
 *
 *   attribBufferSize [OUT]
 *     This parameter returns the attribute buffer size.
 */
#define NV2080_CTRL_CMD_GR_GET_ATTRIBUTE_BUFFER_SIZE (0x2080121eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS_MESSAGE_ID (0x1EU)

typedef struct NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS {
    NvU32 attribBufferSize;
} NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GFX_POOL_QUERY_SIZE
 *
 * This API queries size parameters for a request maximum graphics preemption
 * pool size.  It is only available to kernel callers
 *
 * NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS
 *   struct to return the size parameters
 *
 *   maxSlots
 *     Input specifying the maximum number of slots, RM will calculate the output
 *     parameters based on this.  Must be non-zero
 *   ctrlStructSize
 *     Output indicating the required size in bytes of the control structure to
 *     support a pool of maxSlots size.
 *   ctrlStructAlign
 *     Output indicating the required alignment of the control structure
 *   poolSize
 *     Output indicating the required size in bytes of the GfxP Pool.
 *   poolAlign
 *     Output indicating the required alignment of the GfxP Pool
 *   slotStride
 *     The number of bytes in each slot, i * slotStride gives the offset from the
 *     base of the pool to a given slot
 */
#define NV2080_CTRL_CMD_GR_GFX_POOL_QUERY_SIZE (0x2080121fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS_MESSAGE_ID (0x1FU)

typedef struct NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS {
    NvU32 maxSlots;
    NvU32 slotStride;
    NV_DECLARE_ALIGNED(NvU64 ctrlStructSize, 8);
    NV_DECLARE_ALIGNED(NvU64 ctrlStructAlign, 8);
    NV_DECLARE_ALIGNED(NvU64 poolSize, 8);
    NV_DECLARE_ALIGNED(NvU64 poolAlign, 8);
} NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GFX_POOL_INITIALIZE
 *
 * This API takes a CPU pointer to a GFxP Pool Control Structure and does the
 * required onetime initialization.  It should be called once and only once
 * before a pool is used.  It is only accessible to kernel callers.
 *
 * NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS
 *   struct to hand in the required info to RM
 *
 *   maxSlots
 *      Max pool slots
 *   hMemory
 *      Handle to GFX Pool memory
 *   offset
 *      Offset of the control structure in GFX Pool memory
 *   size
 *      Size of the control structure
 */
#define NV2080_CTRL_CMD_GR_GFX_POOL_INITIALIZE (0x20801220U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS {
    NvU32    maxSlots;
    NvHandle hMemory;
    NvU32    offset;
    NvU32    size;
} NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS;

#define NV2080_CTRL_GR_GFX_POOL_MAX_SLOTS     64U

/*
 * NV2080_CTRL_CMD_GR_GFX_POOL_ADD_SLOTS
 *
 * This API adds a list of buffer slots to a given control structure.  It can
 * only be called when no channel using the given pool is running or may become
 * running for the duration of this call.  If more slots are added than there
 * is room for in the control structure the behavior is undefined. It is only
 * accessible to kernel callers.
 *
 * NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS
 *
 *   numSlots
 *     This input indicates how many slots are being added and are contained in the slots parameter
 *   slots
 *     This input contains an array of the slots to be added to the control structure
 *   hMemory
 *      Handle to GFX Pool memory
 *   offset
 *      Offset of the control structure in GFX Pool memory
 *   size
 *      Size of the control structure
 */
#define NV2080_CTRL_CMD_GR_GFX_POOL_ADD_SLOTS (0x20801221U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS {
    NvU32    numSlots;
    NvU32    slots[NV2080_CTRL_GR_GFX_POOL_MAX_SLOTS];
    NvHandle hMemory;
    NvU32    offset;
    NvU32    size;
} NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GFX_POOL_REMOVE_SLOTS
 *
 * This API removes buffer slots from a given control structure.  It can
 * only be called when no channel using the given pool is running or may become
 * running for the duration of this call. It can operate in two modes, either
 * it will a specified number of slots, or a specified list of slots.
 *
 * It is only accessible to kernel callers.
 *
 * NV2080_CTRL_CMD_GR_GFX_POOL_REMOVE_SLOTS_PARAMS
 *
 *   numSlots
 *     This input indicates how many slots are being removed.  if
 *     bRemoveSpecificSlots is true, then it also indicates how many entries in
 *     the slots array are populated.
 *   slots
 *     This array is either an input or output.  If bRemoveSpecificSlots is true,
 *     then this will contain the list of slots to remove.  If it is false, then
 *     it will be populated by RM with the indexes of the slots that were
 *     removed.
 *   bRemoveSpecificSlots
 *     This input determines which mode the call will run in.  If true the caller
 *     will specify the list of slots they want removed, if any of those slots
 *     are not on the freelist, the call will fail.  If false they only specify
 *     the number of slots they want removed and RM will pick up to that
 *     many.  If there are not enough slots on the freelist to remove the
 *     requested amount, RM will return the number it was able to remove.
 *   hMemory
 *      Handle to GFX Pool memory
 *   offset
 *      Offset of the control structure in GFX Pool memory
 *   size
 *      Size of the control structure
 */
#define NV2080_CTRL_CMD_GR_GFX_POOL_REMOVE_SLOTS (0x20801222U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS {
    NvU32    numSlots;
    NvU32    slots[NV2080_CTRL_GR_GFX_POOL_MAX_SLOTS];
    NvBool   bRemoveSpecificSlots;
    NvHandle hMemory;
    NvU32    offset;
    NvU32    size;
} NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS;



#define NV2080_CTRL_CMD_GR_GET_CAPS_V2 (0x20801227U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x27U)

typedef NV0080_CTRL_GR_GET_CAPS_V2_PARAMS NV2080_CTRL_GR_GET_CAPS_V2_PARAMS;

#define NV2080_CTRL_CMD_GR_GET_INFO_V2 (0x20801228U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_INFO_V2_PARAMS_MESSAGE_ID (0x28U)

typedef NV0080_CTRL_GR_GET_INFO_V2_PARAMS NV2080_CTRL_GR_GET_INFO_V2_PARAMS;



/*
 * NV2080_CTRL_CMD_GR_GET_GPC_MASK
 *
 * This command returns a mask of enabled GPCs for the associated subdevice.
 *
 *    grRouteInfo[IN]
 *      This parameter specifies the routing information used to
 *      disambiguate the target GR engine.
 *
 *    gpcMask[OUT]
 *      This parameter returns a mask of enabled GPCs. Each GPC has an ID
 *      that's equivalent to the corresponding bit position in the mask.
 */
#define NV2080_CTRL_CMD_GR_GET_GPC_MASK (0x2080122aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_GPC_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_GPC_MASK_PARAMS_MESSAGE_ID (0x2AU)

typedef struct NV2080_CTRL_GR_GET_GPC_MASK_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32 gpcMask;
} NV2080_CTRL_GR_GET_GPC_MASK_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_TPC_MASK
 *
 * This command returns a mask of enabled TPCs for a specified GPC.
 *
 *    grRouteInfo[IN]
 *      This parameter specifies the routing information used to
 *      disambiguate the target GR engine.
 *
 *    gpcId[IN]
 *      This parameter specifies the GPC for which TPC information is
 *      to be retrieved. If the GPC with this ID is not enabled this command
 *      will return an tpcMask value of zero.
 *
 *    tpcMask[OUT]
 *      This parameter returns a mask of enabled TPCs for the specified GPC.
 *      Each TPC has an ID that's equivalent to the corresponding bit
 *      position in the mask.
 */
#define NV2080_CTRL_CMD_GR_GET_TPC_MASK (0x2080122bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_TPC_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_TPC_MASK_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV2080_CTRL_GR_GET_TPC_MASK_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32 gpcId;
    NvU32 tpcMask;
} NV2080_CTRL_GR_GET_TPC_MASK_PARAMS;

#define NV2080_CTRL_CMD_GR_SET_TPC_PARTITION_MODE (0x2080122cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID (0x2CU)

typedef NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_ENGINE_CONTEXT_PROPERTIES
 *
 * This command is used to provide the caller with the alignment and size
 * of the context save region for an engine
 *
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 *   engineId
 *     This parameter is an input parameter specifying the engineId for which
 *     the alignment/size is requested.
 *   alignment
 *     This parameter is an output parameter which will be filled in with the
 *     minimum alignment requirement.
 *   size
 *     This parameter is an output parameter which will be filled in with the
 *     minimum size of the context save region for the engine.
 *   bInfoPopulated
 *     This parameter will be set if alignment and size are already set with
 *     valid values from a previous call.
 */

#define NV2080_CTRL_CMD_GR_GET_ENGINE_CONTEXT_PROPERTIES (0x2080122dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS_MESSAGE_ID (0x2DU)

typedef struct NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32  engineId;
    NvU32  alignment;
    NvU32  size;
    NvBool bInfoPopulated;
} NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS;



/*
 * NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER
 *
 * This command provides an interface to retrieve the speed select values of
 * various instruction types.
 *
 *    grRouteInfo[IN]
 *      This parameter specifies the routing information used to
 *      disambiguate the target GR engine.
 *
 *    imla0[OUT]
 *       The current speed select for IMLA0.
 *
 *    fmla16[OUT]
 *       The current speed select for FMLA16.
 *
 *    dp[OUT]
 *       The current speed select for DP.
 *
 *    fmla32[OUT]
 *       The current speed select for FMLA32.
 *
 *    ffma[OUT]
 *       The current speed select for FFMA.
 *
 *    imla1[OUT]
 *       The current speed select for IMLA1.
 *
 *    imla2[OUT]
 *       The current speed select for IMLA2.
 *
 *    imla3[OUT]
 *       The current speed select for IMLA3.
 *
 *    imla4[OUT]
 *       The current speed select for IMLA4.
 */
#define NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER                       (0x20801230U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_FULL_SPEED          (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_2   (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_4   (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_8   (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_16  (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_32  (0x5U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA0_REDUCED_SPEED_1_64  (0x6U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_FULL_SPEED         (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_REDUCED_SPEED_1_2  (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_REDUCED_SPEED_1_4  (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_REDUCED_SPEED_1_8  (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_REDUCED_SPEED_1_16 (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA16_REDUCED_SPEED_1_32 (0x5U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_DP_FULL_SPEED             (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_DP_REDUCED_SPEED          (0x1U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_FULL_SPEED         (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_REDUCED_SPEED_1_2  (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_REDUCED_SPEED_1_4  (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_REDUCED_SPEED_1_8  (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_REDUCED_SPEED_1_16 (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FMLA32_REDUCED_SPEED_1_32 (0x5U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_FULL_SPEED           (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_REDUCED_SPEED_1_2    (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_REDUCED_SPEED_1_4    (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_REDUCED_SPEED_1_8    (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_REDUCED_SPEED_1_16   (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_FFMA_REDUCED_SPEED_1_32   (0x5U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_FULL_SPEED          (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_2   (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_4   (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_8   (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_16  (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_32  (0x5U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA1_REDUCED_SPEED_1_64  (0x6U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_FULL_SPEED          (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_2   (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_4   (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_8   (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_16  (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_32  (0x5U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA2_REDUCED_SPEED_1_64  (0x6U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_FULL_SPEED          (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_2   (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_4   (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_8   (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_16  (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_32  (0x5U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA3_REDUCED_SPEED_1_64  (0x6U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_FULL_SPEED          (0x0U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_2   (0x1U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_4   (0x2U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_8   (0x3U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_16  (0x4U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_32  (0x5U)
#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_IMLA4_REDUCED_SPEED_1_64  (0x6U)

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU8 imla0;
    NvU8 fmla16;
    NvU8 dp;
    NvU8 fmla32;
    NvU8 ffma;
    NvU8 imla1;
    NvU8 imla2;
    NvU8 imla3;
    NvU8 imla4;
} NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS;

#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE (0xFFU)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_FMLA16        (0x0U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_DP            (0x1U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_FMLA32        (0x2U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_FFMA          (0x3U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_IMLA0         (0x4U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_IMLA1         (0x5U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_IMLA2         (0x6U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_IMLA3         (0x7U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_IMLA4         (0x8U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_FP16          (0x9U)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_FP32          (0xAU)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_DFMA          (0xBU)
#define NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_DMLA          (0xCU)

/*
 * NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER_V2
 *
 * This command provides an interface to retrieve the speed select values of
 * various instruction types.
 *
 *   smIssueRateModifierListSize
 *     This field specifies the number of entries on the caller's
 *     smIssueRateModifierList.
 *     When caller passes smIssueRateModifierListSize = 0, all fuse
 *     values are returned.
 *   smIssueRateModifierList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the speed select values are to be returned.
 */
#define NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER_V2       (0x2080123cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_MESSAGE_ID" */

typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2;

#define NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_MESSAGE_ID (0x3CU)

typedef struct NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS {
    NvU32                                    smIssueRateModifierListSize;
    NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2 smIssueRateModifierList[NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE];
} NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_FECS_BIND_EVTBUF_FOR_UID
 *
 * *DEPRECATED* Use NV2080_CTRL_CMD_GR_FECS_BIND_EVTBUF_FOR_UID_V2 instead
 *
 * This command is used to create a FECS bind-point to an event buffer that
 * is filtered by UID.
 *
 *  hEventBuffer[IN]
 *      The event buffer to bind to
 *
 *  recordSize[IN]
 *      The size of the FECS record in bytes
 *
 *  levelOfDetail[IN]
 *      One of NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_:
 *          FULL: Report all CtxSw events
 *          SIMPLE: Report ACTIVE_REGION_START and ACTIVE_REGION_END only
 *          COMPAT: Events that KMD is interested in (for backwards compatibility)
 *          CUSTOM: Report events in the eventFilter field
 *      NOTE: RM may override the level-of-detail depending on the caller
 *
 *  eventFilter[IN]
 *      Bitmask of events to report if levelOfDetail is CUSTOM
 *
 *  bAllUsers[IN]
 *     Only report FECS CtxSw data for the current user if false, for all users if true
 */

#define NV2080_CTRL_CMD_GR_FECS_BIND_EVTBUF_FOR_UID (0x20801231U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD {
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_FULL = 0,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_SIMPLE = 1,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_COMPAT = 2,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_CUSTOM = 3,
} NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD;

#define NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS_MESSAGE_ID (0x31U)

typedef struct NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS {
    NvHandle                            hEventBuffer;
    NvU32                               recordSize;
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD levelOfDetail;
    NvU32                               eventFilter;
    NvBool                              bAllUsers;
} NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_PHYS_GPC_MASK
 *
 * This command returns a mask of physical GPC Ids for the associated syspipe
 *
 *    physSyspipeId[IN]
 *      This parameter specifies syspipe for which phys GPC mask is requested
 *
 *    gpcMask[OUT]
 *      This parameter returns a mask of mapped GPCs to provided syspipe.
 *      Each GPC-ID has a corresponding bit position in the mask.
 */
#define NV2080_CTRL_CMD_GR_GET_PHYS_GPC_MASK (0x20801232U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS_MESSAGE_ID (0x32U)

typedef struct NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS {
    NvU32 physSyspipeId;
    NvU32 gpcMask;
} NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_PPC_MASK
 *
 * This command returns a mask of enabled PPCs for a specified GPC.
 *
 *    grRouteInfo[IN]
 *      This parameter specifies the routing information used to
 *      disambiguate the target GR engine.
 *
 *    gpcId[IN]
 *      This parameter specifies the GPC for which TPC information is
 *      to be retrieved. If the GPC with this ID is not enabled this command
 *      will return an ppcMask value of zero.
 *
 *    ppcMask[OUT]
 *      This parameter returns a mask of enabled PPCs for the specified GPC.
 *      Each PPC has an ID that's equivalent to the corresponding bit
 *      position in the mask.
 */
#define NV2080_CTRL_CMD_GR_GET_PPC_MASK (0x20801233U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_PPC_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_PPC_MASK_PARAMS_MESSAGE_ID (0x33U)

typedef struct NV2080_CTRL_GR_GET_PPC_MASK_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32 gpcId;
    NvU32 ppcMask;
} NV2080_CTRL_GR_GET_PPC_MASK_PARAMS;

#define NV2080_CTRL_CMD_GR_GET_NUM_TPCS_FOR_GPC (0x20801234U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS_MESSAGE_ID (0x34U)

typedef struct NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS {
    NvU32 gpcId;
    NvU32 numTpcs;
} NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_CTXSW_MODES
 *
 * This command is used to get context switch modes for the specified
 * channel. A value of NV_ERR_NOT_SUPPORTED is returned if the
 * target channel does not support context switch mode changes.
 *
 *   hChannel
 *     This parameter specifies the channel handle of
 *     the channel that is to have its context switch modes retrieved.
 *   zcullMode
 *     See NV2080_CTRL_CMD_GR_CTXSW_ZCULL_MODE for possible return values
 *   pmMode
 *     See NV2080_CTRL_CMD_GR_CTXSW_PM_MODE for possible return values
 *   smpcMode
 *     See NV2080_CTRL_CMD_GR_CTXSW_SMPC_MODE for possible return values
 *   cilpPreemptMode
 *     See NV2080_CTRL_CMD_GR_SET_CTXSW_PREEMPTION_MODE for possible return values
 *   gfxpPreemptMode
 *     See NV2080_CTRL_CMD_GR_SET_CTXSW_PREEMPTION_MODE for possible return values
 */
#define NV2080_CTRL_CMD_GR_GET_CTXSW_MODES (0x20801235U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS_MESSAGE_ID (0x35U)

typedef struct NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS {
    NvHandle hChannel;
    NvU32    zcullMode;
    NvU32    pmMode;
    NvU32    smpcMode;
    NvU32    cilpPreemptMode;
    NvU32    gfxpPreemptMode;
} NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_GPC_TILE_MAP
 *
 * Get a list of values used to describe GPC/TPC tile mapping tables.
 *
 *   mapValueCount
 *     This field specifies the number of actual map entries.  This count
 *     should equal the number of TPCs in the system.
 *   mapValues
 *     This field is a pointer to a buffer of NvU08 values representing map
 *     data.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_GET_GPC_TILE_MAP (0x20801236U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS_MESSAGE_ID (0x36U)

typedef NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_ZCULL_MASK
 *
 * This command returns a mask of enabled ZCULLs for a specified GPC.
 *
 *    gpcId[IN]
 *      This parameter, physical GPC index, specifies the GPC for which ZCULL
 *      information is to be retrieved. If the GPC with this ID is not enabled
 *      this command will return a zcullMask value of zero.
 *
 *    zcullMask[OUT]
 *      This parameter returns a mask of enabled ZCULLs for the specified GPC.
 *      Each ZCULL has an ID that's equivalent to the corresponding bit
 *      position in the mask.
 */

#define NV2080_CTRL_CMD_GR_GET_ZCULL_MASK (0x20801237U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS_MESSAGE_ID (0x37U)

typedef struct NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS {
    NvU32 gpcId;
    NvU32 zcullMask;
} NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_FECS_BIND_EVTBUF_FOR_UID_V2
 *
 * This command is used to create a FECS bind-point to an event buffer that
 * is filtered by UID.
 *
 *  hEventBuffer[IN]
 *      The event buffer to bind to
 *
 *  recordSize[IN]
 *      The size of the FECS record in bytes
 *
 *  levelOfDetail[IN]
 *      One of NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_:
 *          FULL: Report all CtxSw events
 *          SIMPLE: Report ACTIVE_REGION_START and ACTIVE_REGION_END only
 *          COMPAT: Events that KMD is interested in (for backwards compatibility)
 *          CUSTOM: Report events in the eventFilter field
 *      NOTE: RM may override the level-of-detail depending on the caller
 *
 *  eventFilter[IN]
 *      Bitmask of events to report if levelOfDetail is CUSTOM
 *
 *  bAllUsers[IN]
 *     Only report FECS CtxSw data for the current user if false, for all users if true
 *
 *  reasonCode [OUT]
 *     Reason for failure
 */
#define NV2080_CTRL_CMD_GR_FECS_BIND_EVTBUF_FOR_UID_V2 (0x20801238U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_GR_FECS_BIND_EVTBUF_REASON_CODE {
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NONE = 0,
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_GPU_TOO_OLD = 1,
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NOT_ENABLED_GPU = 2,
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NOT_ENABLED = 3,
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NEED_ADMIN = 4,
    NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NEED_CAPABILITY = 5,
} NV2080_CTRL_GR_FECS_BIND_EVTBUF_REASON_CODE;

#define NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS_MESSAGE_ID (0x38U)

typedef struct NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS {
    NvHandle                            hEventBuffer;
    NvU32                               recordSize;
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD levelOfDetail;
    NvU32                               eventFilter;
    NvBool                              bAllUsers;
    NvU32                               reasonCode;
} NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_GFX_GPC_AND_TPC_INFO
 *
 * This command grabs information on GFX capable GPC's and TPC's for a specifc GR engine
 *
 *  grRouteInfo[IN]
 *      This parameter specifies the routing information used to
 *      disambiguate the target GR engine.
 *
 *  physGfxGpcMask [OUT]
 *     Physical mask of Gfx capable GPC's
 *
 *  numGfxTpc [OUT]
 *     Total number of Gfx capable TPC's
 */
#define NV2080_CTRL_CMD_GR_GET_GFX_GPC_AND_TPC_INFO (0x20801239U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32 physGfxGpcMask;
    NvU32 numGfxTpc;
} NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GR_GET_TPC_RECONFIG_MASK
 *
 * This command returns the TPC reconfig mask for a specific GPC
 *
 *  gpc[IN]
 *      The GPC for which the TPC reconfig mask needs to be queried.
 *      The GPC should be specified as a logical index.
 *
 *  tpcReconfigMask[OUT]
 *     Mask of reconfigurable TPCs in the specified GPC
 *
 *  grRouteInfo[IN]
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 */
#define NV2080_CTRL_CMD_GR_GET_TPC_RECONFIG_MASK (0x2080123bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GR_INTERFACE_ID << 8) | NV2080_CTRL_GR_GET_TPC_RECONFIG_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GR_GET_TPC_RECONFIG_MASK_PARAMS_MESSAGE_ID (0x3bU)

typedef struct NV2080_CTRL_GR_GET_TPC_RECONFIG_MASK_PARAMS {
    NvU32 gpc;
    NvU32 tpcReconfigMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GR_GET_TPC_RECONFIG_MASK_PARAMS;

/* _ctrl2080gr_h_ */
