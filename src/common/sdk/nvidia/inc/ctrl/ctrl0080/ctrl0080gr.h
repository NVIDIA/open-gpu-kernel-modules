/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0080/ctrl0080gr.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"
#include "nvcfg_sdk.h"

typedef struct NV0080_CTRL_GR_ROUTE_INFO {
    NvU32 flags;
    NV_DECLARE_ALIGNED(NvU64 route, 8);
} NV0080_CTRL_GR_ROUTE_INFO;

/* NV01_DEVICE_XX/NV03_DEVICE gr engine control commands and parameters */

/**
 * NV0080_CTRL_CMD_GR_GET_CAPS
 *
 * This command returns the set of graphics capabilities for the device
 * in the form of an array of unsigned bytes.  Graphics capabilities
 * include supported features and required workarounds for the graphics
 * engine(s) within the device, each represented by a byte offset into the
 * table and a bit position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0080_CTRL_GR_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the graphics caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 */
#define NV0080_CTRL_CMD_GR_GET_CAPS (0x801102) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_GR_GET_CAPS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0080_CTRL_GR_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV0080_CTRL_GR_GET_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV0080_CTRL_GR_GET_CAP(tbl,c)              (((NvU8)tbl[(1?c)]) & (0?c))



/*
 * Size in bytes of gr caps table.  This value should be one greater
 * than the largest byte_index value above.
 */
#define NV0080_CTRL_GR_CAPS_TBL_SIZE            23



/*
 * NV0080_CTRL_CMD_GR_INFO
 *
 * This structure represents a single 32bit graphics engine value.  Clients
 * request a particular graphics engine value by specifying a unique bus
 * information index.
 *
 * Legal graphics information index values are:
 *   NV0080_CTRL_GR_INFO_INDEX_MAXCLIPS
 *     This index is used to request the number of clip IDs supported by
 *     the device.
 *   NV0080_CTRL_GR_INFO_INDEX_MIN_ATTRS_BUG_261894
 *     This index is used to request the minimum number of attributes that
 *     need to be enabled to avoid bug 261894.  A return value of 0
 *     indicates that there is no minimum and the bug is not present on this
 *     system.
 */
typedef NVXXXX_CTRL_XXX_INFO NV0080_CTRL_GR_INFO;

/* valid graphics info index values */
#define NV0080_CTRL_GR_INFO_INDEX_MAXCLIPS                              (0x00000000)
#define NV0080_CTRL_GR_INFO_INDEX_MIN_ATTRS_BUG_261894                  (0x00000001)
#define NV0080_CTRL_GR_INFO_XBUF_MAX_PSETS_PER_BANK                     (0x00000002)
#define NV0080_CTRL_GR_INFO_INDEX_BUFFER_ALIGNMENT                      (0x00000003)
#define NV0080_CTRL_GR_INFO_INDEX_SWIZZLE_ALIGNMENT                     (0x00000004)
#define NV0080_CTRL_GR_INFO_INDEX_VERTEX_CACHE_SIZE                     (0x00000005)
#define NV0080_CTRL_GR_INFO_INDEX_VPE_COUNT                             (0x00000006)
#define NV0080_CTRL_GR_INFO_INDEX_SHADER_PIPE_COUNT                     (0x00000007)
#define NV0080_CTRL_GR_INFO_INDEX_THREAD_STACK_SCALING_FACTOR           (0x00000008)
#define NV0080_CTRL_GR_INFO_INDEX_SHADER_PIPE_SUB_COUNT                 (0x00000009)
#define NV0080_CTRL_GR_INFO_INDEX_SM_REG_BANK_COUNT                     (0x0000000A)
#define NV0080_CTRL_GR_INFO_INDEX_SM_REG_BANK_REG_COUNT                 (0x0000000B)
#define NV0080_CTRL_GR_INFO_INDEX_SM_VERSION                            (0x0000000C)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_WARPS_PER_SM                      (0x0000000D)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_THREADS_PER_WARP                  (0x0000000E)
#define NV0080_CTRL_GR_INFO_INDEX_GEOM_GS_OBUF_ENTRIES                  (0x0000000F)
#define NV0080_CTRL_GR_INFO_INDEX_GEOM_XBUF_ENTRIES                     (0x00000010)
#define NV0080_CTRL_GR_INFO_INDEX_FB_MEMORY_REQUEST_GRANULARITY         (0x00000011)
#define NV0080_CTRL_GR_INFO_INDEX_HOST_MEMORY_REQUEST_GRANULARITY       (0x00000012)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_SP_PER_SM                         (0x00000013)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS                       (0x00000014)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPS                       (0x00000015)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_ZCULL_BANKS                (0x00000016)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPC_PER_GPC                (0x00000017)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MIN_FBPS                   (0x00000018)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_FBP_PORTS            (0x00000019)
#define NV0080_CTRL_GR_INFO_INDEX_TIMESLICE_ENABLED                     (0x0000001A)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPAS                      (0x0000001B)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_PES_PER_GPC                (0x0000001C)
#define NV0080_CTRL_GR_INFO_INDEX_GPU_CORE_COUNT                        (0x0000001D)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPCS_PER_PES               (0x0000001E)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_MXBAR_HUB_PORTS            (0x0000001F)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_SM_PER_TPC                 (0x00000020)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_HSHUB_FBP_PORTS            (0x00000021)
#define NV0080_CTRL_GR_INFO_INDEX_RT_CORE_COUNT                         (0x00000022)
#define NV0080_CTRL_GR_INFO_INDEX_TENSOR_CORE_COUNT                     (0x00000023)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GRS                        (0x00000024)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTCS                       (0x00000025)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_SLICES                 (0x00000026)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCMMU_PER_GPC             (0x00000027)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_LTC_PER_FBP                (0x00000028)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_ROP_PER_GPC                (0x00000029)
#define NV0080_CTRL_GR_INFO_INDEX_FAMILY_MAX_TPC_PER_GPC                (0x0000002A)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_FBPA_PER_FBP               (0x0000002B)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT                  (0x0000002C)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_LEGACY_SUBCONTEXT_COUNT           (0x0000002D)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_PER_ENGINE_SUBCONTEXT_COUNT       (0x0000002E)


#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_SLICES_PER_LTC             (0x00000032)

#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GFXC_SMC_ENGINES           (0x00000033)


#define NV0080_CTRL_GR_INFO_INDEX_DUMMY                                 (0x00000033)
#define NV0080_CTRL_GR_INFO_INDEX_GFX_CAPABILITIES                      (0x00000034)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES                       (0x00000035)
#define NV0080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS                (0x00000036)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_MIN_SUBCTX_PER_SMC_ENG         (0x00000037)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS_PER_DIELET            (0x00000038)
#define NV0080_CTRL_GR_INFO_INDEX_LITTER_MAX_NUM_SMC_ENGINES_PER_DIELET (0x00000039)

/* When adding a new INDEX, please update MAX_SIZE accordingly
 * NOTE: 0080 functionality is merged with 2080 functionality, so this max size
 * reflects that.
 */
#define NV0080_CTRL_GR_INFO_INDEX_MAX                                   (0x00000039)
#define NV0080_CTRL_GR_INFO_MAX_SIZE                                    (0x3a) /* finn: Evaluated from "(NV0080_CTRL_GR_INFO_INDEX_MAX + 1)" */

/*
 * NV0080_CTRL_CMD_GR_GET_INFO
 *
 * This command returns graphics engine information for the associate GPU.
 * Request to retrieve graphics information use a list of one or more
 * NV0080_CTRL_GR_INFO structures.
 *
 *   grInfoListSize
 *     This field specifies the number of entries on the caller's
 *     grInfoList.
 *   grInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the bus information is to be returned.
 *     This buffer must be at least as big as grInfoListSize multiplied
 *     by the size of the NV0080_CTRL_GR_INFO structure.
 */
#define NV0080_CTRL_CMD_GR_GET_INFO                                     (0x801104) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_GR_GET_INFO_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0080_CTRL_GR_GET_INFO_PARAMS {
    NvU32 grInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 grInfoList, 8);
} NV0080_CTRL_GR_GET_INFO_PARAMS;

/*
 * NV0080_CTRL_CMD_GR_GET_TPC_PARTITION_MODE
 *     This command gets the current partition mode of a TSG context.
 *
 * NV0080_CTRL_CMD_GR_SET_TPC_PARTITION_MODE
 *     This command sets the partition mode of a TSG context.
 *
 * NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS
 *     This structure defines the parameters used for TPC partitioning mode SET/GET commands
 *
 *     hChannelGroup [IN]
 *         RM Handle to the TSG
 *
 *     mode [IN/OUT]
 *         Partitioning mode enum value
 *             For the SET cmd, this is an input parameter
 *             For the GET cmd, this is an output parameter
 *
 *     bEnableAllTpcs [IN]
 *         Flag to enable all TPCs by default
 *
 *     grRouteInfo[IN]
 *         This parameter specifies the routing information used to
 *         disambiguate the target GR engine.
 *
 */
#define NV0080_CTRL_CMD_GR_GET_TPC_PARTITION_MODE (0x801107) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_GET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_CMD_GR_SET_TPC_PARTITION_MODE (0x801108) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID" */

/* Enum for listing TPC partitioning modes */
typedef enum NV0080_CTRL_GR_TPC_PARTITION_MODE {
    NV0080_CTRL_GR_TPC_PARTITION_MODE_NONE = 0,
    NV0080_CTRL_GR_TPC_PARTITION_MODE_STATIC = 1,
    NV0080_CTRL_GR_TPC_PARTITION_MODE_DYNAMIC = 2,
} NV0080_CTRL_GR_TPC_PARTITION_MODE;

typedef struct NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS {
    NvHandle                          hChannelGroup;   // [in]
    NV0080_CTRL_GR_TPC_PARTITION_MODE mode;            // [in/out]
    NvBool                            bEnableAllTpcs;  // [in/out]
    NV_DECLARE_ALIGNED(NV0080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);     // [in]
} NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS;

#define NV0080_CTRL_GR_GET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID (0x7U)

typedef NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS NV0080_CTRL_GR_GET_TPC_PARTITION_MODE_PARAMS;

#define NV0080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS_MESSAGE_ID (0x8U)

typedef NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS NV0080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS;

/**
 * NV0080_CTRL_CMD_GR_GET_CAPS_V2
 *
 * This command returns the same set of graphics capabilities for the device
 * as @ref NV0080_CTRL_CMD_GR_GET_CAPS. The difference is in the structure
 * NV0080_CTRL_GR_GET_INFO_V2_PARAMS, which contains a statically sized array,
 * rather than a caps table pointer and a caps table size in
 * NV0080_CTRL_GR_GET_INFO_PARAMS. Additionally,
 * NV0080_CTRL_GR_GET_INFO_V2_PARAMS contains a parameter for specifying routing
 * information, used for MIG.
 *
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the graphics caps bits will be written by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine.
 *
 *   bCapsPopulated
 *     This parameter indicates that the capsTbl has been partially populated by
 *     previous calls to NV0080_CTRL_CMD_GR_GET_CAPS_V2 on other subdevices.
 */
#define NV0080_CTRL_CMD_GR_GET_CAPS_V2 (0x801109) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_GR_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0080_CTRL_GR_GET_CAPS_V2_PARAMS {
    NvU8   capsTbl[NV0080_CTRL_GR_CAPS_TBL_SIZE];
    NV_DECLARE_ALIGNED(NV0080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvBool bCapsPopulated;
} NV0080_CTRL_GR_GET_CAPS_V2_PARAMS;

#define NV0080_CTRL_CMD_GR_GET_INFO_V2 (0x801110) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_GR_INTERFACE_ID << 8) | NV0080_CTRL_GR_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_GR_GET_INFO_V2_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0080_CTRL_GR_GET_INFO_V2_PARAMS {
    NvU32               grInfoListSize;
    NV0080_CTRL_GR_INFO grInfoList[NV0080_CTRL_GR_INFO_MAX_SIZE];
    NV_DECLARE_ALIGNED(NV0080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV0080_CTRL_GR_GET_INFO_V2_PARAMS;

/* _ctrl0080gr_h_ */
