/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080dma.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX dma control commands and parameters */

#include "ctrl2080common.h"

/*
 * NV2080_CTRL_CMD_DMA_INVALIDATE_TLB
 *
 * This command invalidates the GPU TLB. This is intended to be used
 * by RM clients that manage their own TLB consistency when updating
 * page tables on their own, or with DEFER_TLB_INVALIDATION options
 * to other RM APIs.
 *
 *    hVASpace
 *       This parameter specifies the VASpace object whose MMU TLB entries needs to be invalidated.
 *       Specifying a GMMU VASpace object handle will invalidate the GMMU TLB for the particular VASpace.
 *       Specifying a SMMU VASpace object handle will flush the entire SMMU TLB & PTC.
 *
 * This call can be used with the NV50_DEFERRED_API_CLASS (class 0x5080).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_TIMEOUT_RETRY
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_DMA_INVALIDATE_TLB (0x20802502) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_DMA_INTERFACE_ID << 8) | NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS {
    NvHandle hClient; // Deprecated. Kept here for compactibility with chips_GB9-2-1-1
    NvHandle hDevice; // Deprecated. Kept here for compactibility with chips_GB9-2-1-1
    NvU32    engine;  // Deprecated. Kept here for compactibility with chips_GB9-2-1-1
    NvHandle hVASpace;
} NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS;

#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_GRAPHICS             0:0
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_GRAPHICS_FALSE    (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_GRAPHICS_TRUE     (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VIDEO                1:1
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VIDEO_FALSE       (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VIDEO_TRUE        (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_DISPLAY              2:2
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_DISPLAY_FALSE     (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_DISPLAY_TRUE      (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_CAPTURE              3:3
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_CAPTURE_FALSE     (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_CAPTURE_TRUE      (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_IFB                  4:4
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_IFB_FALSE         (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_IFB_TRUE          (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MV                   5:5
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MV_FALSE          (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MV_TRUE           (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MPEG                 6:6
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MPEG_FALSE        (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_MPEG_TRUE         (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VLD                  7:7
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VLD_FALSE         (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_VLD_TRUE          (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_ENCRYPTION           8:8
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_ENCRYPTION_FALSE  (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_ENCRYPTION_TRUE   (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_PERFMON              9:9
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_PERFMON_FALSE     (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_PERFMON_TRUE      (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_POSTPROCESS          10:10
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_POSTPROCESS_FALSE (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_POSTPROCESS_TRUE  (0x00000001)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_BAR                  11:11
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_BAR_FALSE         (0x00000000)
#define NV2080_CTRL_DMA_INVALIDATE_TLB_ENGINE_BAR_TRUE          (0x00000001)

/*
 * NV2080_CTRL_DMA_INFO
 *
 * This structure represents a single 32bit dma engine value.  Clients
 * request a particular DMA engine value by specifying a unique dma
 * information index.
 *
 * Legal dma information index values are:
 *   NV2080_CTRL_DMA_INFO_INDEX_SYSTEM_ADDRESS_SIZE
 *     This index can be used to request the system address size in bits.
 */
typedef struct NV2080_CTRL_DMA_INFO {
    NvU32 index;
    NvU32 data;
} NV2080_CTRL_DMA_INFO;

/* valid dma info index values */
#define NV2080_CTRL_DMA_INFO_INDEX_SYSTEM_ADDRESS_SIZE (0x000000000)

/* set INDEX_MAX to greatest possible index value */
#define NV2080_CTRL_DMA_INFO_INDEX_MAX                 NV2080_CTRL_DMA_INFO_INDEX_SYSTEM_ADDRESS_SIZE

/*
 * NV2080_CTRL_CMD_DMA_GET_INFO
 *
 * This command returns dma engine information for the associated GPU.
 * Requests to retrieve dma information use an array of one or more
 * NV2080_CTRL_DMA_INFO structures.
 *
 *   dmaInfoTblSize
 *     This field specifies the number of valid entries in the dmaInfoList
 *     array.  This value cannot exceed NV2080_CTRL_DMA_GET_INFO_MAX_ENTRIES.
 *   dmaInfoTbl
 *     This parameter contains the client's dma info table into
 *     which the dma info values will be transferred by the RM.
 *     The dma info table is an array of NV2080_CTRL_DMA_INFO structures.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_DMA_GET_INFO                   (0x20802503) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_DMA_INTERFACE_ID << 8) | NV2080_CTRL_DMA_GET_INFO_PARAMS_MESSAGE_ID" */

/* maximum number of NV2080_CTRL_DMA_INFO entries per request */
#define NV2080_CTRL_DMA_GET_INFO_MAX_ENTRIES           (256)

#define NV2080_CTRL_DMA_GET_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_DMA_GET_INFO_PARAMS {
    NvU32                dmaInfoTblSize;
    /*
     * C form:
     * NV2080_CTRL_DMA_INFO dmaInfoTbl[NV2080_CTRL_DMA_GET_INFO_MAX_ENTRIES];
     */
    NV2080_CTRL_DMA_INFO dmaInfoTbl[NV2080_CTRL_DMA_GET_INFO_MAX_ENTRIES];
} NV2080_CTRL_DMA_GET_INFO_PARAMS;

typedef struct NV2080_CTRL_DMA_UPDATE_COMPTAG_INFO_TILE_INFO {
    /*! 
     * 64KB aligned address of source 64KB tile for comptag reswizzle.
     */
    NvU32 srcAddr;

    /*! 
     * 64KB aligned address of destination 64KB tile for comptag reswizzle.
     */
    NvU32 dstAddr;

    /*!
     * Comptag index assigned to the 64K sized tile relative to
     * the compcacheline. Absolute comptag index would be:
     * startComptagIndex + relComptagIndex.
     */
    NvU16 relComptagIndex;
} NV2080_CTRL_DMA_UPDATE_COMPTAG_INFO_TILE_INFO;

// _ctrl2080dma_h_
