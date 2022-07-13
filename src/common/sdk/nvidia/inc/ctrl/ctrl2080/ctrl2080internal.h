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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file: ctrl/ctrl2080/ctrl2080internal.finn
//

#include "nvimpshared.h"



#include "ctrl/ctrl2080/ctrl2080base.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080gr.h"        /* Some controls derivative of 2080gr */
#include "ctrl/ctrl0080/ctrl0080msenc.h"     /* NV0080_CTRL_MSENC_CAPS_TBL_SIZE    */
#include "ctrl/ctrl0080/ctrl0080bsp.h"       /* NV0080_CTRL_BSP_CAPS_TBL_SIZE      */
#include "ctrl/ctrl2080/ctrl2080fifo.h"      /* NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO */
#include "ctrl/ctrl0000/ctrl0000system.h"
/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_STATIC_INFO
 *
 *    This command obtains information from physical RM for use by CPU-RM.
 *
 *   feHwSysCap
 *     Display IP v03_00 and later.
 *     Contents of capability register.
 *
 *   windowPresentMask
 *     Display IP v03_00 and later.
 *     Mask for the present WINDOWs actually on the current chip.
 *   bFbRemapperEnabled
 *     Display IP v02_01 and later.
 *     Indicates that the display remapper HW exists and is enabled.
 */

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_STATIC_INFO (0x20800a01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS {
    NvU32  feHwSysCap;
    NvU32  windowPresentMask;
    NvBool bFbRemapperEnabled;
} NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS;



//
// MemorySystem settings that are static after GPU state init/load is finished.
//
// Fields are shared between the VGPU guest/GSP Client as well as the VGPU
// host/GSP-RM.
//
#define NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS_MESSAGE_ID (0x1CU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS {
    /*! Determines if RM should use 1 to 1 Comptagline allocation policy */
    NvBool bOneToOneComptagLineAllocation;

    /*! Determines if RM should use 1 to 4 Comptagline allocation policy */
    NvBool bUseOneToFourComptagLineAllocation;

    /*! Determines if RM should use raw Comptagline allocation policy */
    NvBool bUseRawModeComptaglineAllocation;

    /*! Has COMPBIT_BACKING_SIZE been overridden to zero (i.e. disabled)? */
    NvBool bDisableCompbitBacking;

    /*! Determine if we need to disable post L2 compression */
    NvBool bDisablePostL2Compression;

    /*! Is ECC DRAM feature supported? */
    NvBool bEnabledEccFBPA;

    NvBool bL2PreFill;

    /*! L2 cache size */
    NV_DECLARE_ALIGNED(NvU64 l2CacheSize, 8);

    NvBool bReservedMemAtBottom;

    /*! Indicate whether fpba is present or not */
    NvBool bFbpaPresent;

    /*! Size covered by one comptag */
    NvU32  comprPageSize;

    /*! log32(comprPageSize) */
    NvU32  comprPageShift;

    /*! Maximum number of pages that can be dynamaically blacklisted */
    NvU16  maximumBlacklistPages;

    /*! RAM type */
    NvU32  ramType;

    /*! LTC count */
    NvU32  ltcCount;

    /*! LTS per LTC count */
    NvU32  ltsPerLtcCount;

    /*! Ampere PLC bug */
    NvBool bDisablePlcForCertainOffsetsBug3046774;
} NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS;

/*!
 * Retrieve Memory System Static data.
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_STATIC_CONFIG        (0x20800a1c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_REGISTER_UVM_ACCESS_CNTR_BUFFER
 *
 * This command sends access counter buffer pages allocated by CPU-RM
 * to be setup and enabled in physical RM.
 *
 * bufferSize
 *   Size of the access counter buffer to register.
 *
 * bufferPteArray
 *   Pages of access counter buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER (0x20800a1d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_UVM_ACCESS_CNTR_BUFFER_MAX_PAGES    64
#define NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS_MESSAGE_ID (0x1DU)

typedef struct NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS {
    NvU32 bufferSize;
    NV_DECLARE_ALIGNED(NvU64 bufferPteArray[NV2080_CTRL_INTERNAL_UVM_ACCESS_CNTR_BUFFER_MAX_PAGES], 8);
} NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER
 *
 * This command requests physical RM to disable the access counter buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER (0x20800a1e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x1E" */

/*
 * NV2080_CTRL_CMD_INTERNAL_UVM_SERVICE_ACCESS_CNTR_BUFFER
 *
 * This command requests physical RM to service the access counter buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_UVM_SERVICE_ACCESS_CNTR_BUFFER    (0x20800a21) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x21" */

/*
 * NV2080_CTRL_CMD_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE
 *
 * This command retrieves the access counter buffer size from physical RM.
 *
 * bufferSize[OUT]
 *   Size of the access counter buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE   (0x20800a29) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS_MESSAGE_ID (0x29U)

typedef struct NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS {
    NvU32 bufferSize;
} NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS;

#define NV2080_CTRL_INTERNAL_GR_MAX_ENGINES          8

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_CAPS_V2
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CAPS (0x20800a1f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x1F" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_CAPS {
    NvU8 capsTbl[NV0080_CTRL_GR_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_STATIC_GR_CAPS;
typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_CAPS engineCaps[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER
 * @ref NV2080_CTRL_CMD_GR_GET_SM_TO_GPC_TPC_MAPPINGS
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER (0x20800a22) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x22" */



#define NV2080_CTRL_INTERNAL_GR_MAX_SM                          240

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER {
    struct {
        NvU16 gpcId;
        NvU16 localTpcId;
        NvU16 localSmId;
        NvU16 globalTpcId;
        NvU16 virtualGpcId;
        NvU16 migratableTpcId;
    } globalSmId[NV2080_CTRL_INTERNAL_GR_MAX_SM];

    NvU16 numSm;
    NvU16 numTpc;
} NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER globalSmOrder[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS;


/*!
 * Retrieve BSP Static data.
 */
#define NV2080_CTRL_CMD_INTERNAL_BSP_GET_CAPS (0x20800a24) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_MAX_BSPS     8

typedef struct NV2080_CTRL_INTERNAL_BSP_CAPS {
    NvU8 capsTbl[NV0080_CTRL_BSP_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_BSP_CAPS;

#define NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_BSP_CAPS caps[NV2080_CTRL_CMD_INTERNAL_MAX_BSPS];
    NvBool                        valid[NV2080_CTRL_CMD_INTERNAL_MAX_BSPS];
} NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS;

/*!
 * Retrieve MSENC Static data.
 */
#define NV2080_CTRL_CMD_INTERNAL_MSENC_GET_CAPS (0x20800a25) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS     3

typedef struct NV2080_CTRL_INTERNAL_MSENC_CAPS {
    NvU8 capsTbl[NV0080_CTRL_MSENC_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_MSENC_CAPS;

#define NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_MSENC_CAPS caps[NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS];
    NvBool                          valid[NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS];
} NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS;


#define NV2080_CTRL_INTERNAL_GR_MAX_GPC                             12
#define NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT                  10

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_GPC_MASK
 * @ref NV2080_CTRL_CMD_GR_GET_TPC_MASK
 * @ref NV2080_CTRL_CMD_GR_GET_PHYS_GPC_MASK
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS (0x20800a26) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x26" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS {
    NvU32 gpcMask;

    /*!
     * tpcMask is indexed by logical GPC ID for MIG case
     * and indexed by physical GPC ID for non-MIG case
     */
    NvU32 tpcMask[NV2080_CTRL_INTERNAL_GR_MAX_GPC];

    /*!
     * tpcCount is always indexed by logical GPC ID
     */
    NvU32 tpcCount[NV2080_CTRL_INTERNAL_GR_MAX_GPC];
    NvU32 physGpcMask;
    NvU32 mmuPerGpc[NV2080_CTRL_INTERNAL_GR_MAX_GPC];

    NvU32 tpcToPesMap[NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT];
    NvU32 numPesPerGpc[NV2080_CTRL_INTERNAL_GR_MAX_GPC];

    /*!
     * zcullMask is always indexed by physical GPC ID
     */
    NvU32 zcullMask[NV2080_CTRL_INTERNAL_GR_MAX_GPC];
} NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS {
    /*!
     * floorsweeping masks which are indexed via local GR index
     */
    NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS floorsweepingMasks[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS;

/*
 * NV2080_CTRL_CMD_KGR_GET_CTX_BUFFER_PTES
 *
 * This command returns physical addresses of specified context buffer.
 * To obtain addresses of whole buffer firstPage has to be advanced on
 * subsequent invocations of the control until whole buffer is probed.
 * If the buffer is contiguous, only single address will be returned by
 * this control.
 *
 *    bufferType[IN]
 *      Buffer type as returned by GET_CTX_BUFFER_INFO.
 *
 *    firstPage[IN]
 *      Index of the first page to return in 'physAddrs' array.
 *
 *    numPages[OUT]
 *      Number of entries filled in 'physAddrs' array. This will be 0
 *      if firstPage is greater or equal to number of pages managed by 'hBuffer'.
 *
 *    physAddrs[OUT]
 *      Physical addresses of pages comprising specified buffer.
 *
 *    bNoMorePages[OUT]
 *      End of buffer reached. Either 'physAddrs' contains last page of the
 *      buffer or 'firstPage' specifies index past the buffer.
 */
#define NV2080_CTRL_KGR_MAX_BUFFER_PTES         128
#define NV2080_CTRL_CMD_KGR_GET_CTX_BUFFER_PTES (0x20800a28) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS_MESSAGE_ID (0x28U)

typedef struct NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS {
    NvHandle hUserClient;
    NvHandle hChannel;
    NvU32    bufferType;
    NvU32    firstPage;
    NvU32    numPages;
    NV_DECLARE_ALIGNED(NvU64 physAddrs[NV2080_CTRL_KGR_MAX_BUFFER_PTES], 8);
    NvBool   bNoMorePages;
} NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS;

/*!
 * @ref NV0080_CTRL_CMD_GR_GET_INFO
 * @ref NV0080_CTRL_CMD_GR_GET_INFO_V2
 * @ref NV2080_CTRL_CMD_GR_GET_INFO
 * @ref NV2080_CTRL_CMD_GR_GET_INFO_V2
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_INFO (0x20800a2a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x2A" */



/*!
 * @ref NV2080_CTRL_GR_INFO
 */
typedef struct NV2080_CTRL_INTERNAL_GR_INFO {
    NvU32 index;
    NvU32 data;
} NV2080_CTRL_INTERNAL_GR_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_INFO {
    NV2080_CTRL_INTERNAL_GR_INFO infoList[NV0080_CTRL_GR_INFO_MAX_SIZE];
} NV2080_CTRL_INTERNAL_STATIC_GR_INFO;
typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO engineInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ZCULL_INFO
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ZCULL_INFO (0x20800a2c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x2C" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_ZCULL_INFO {
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
} NV2080_CTRL_INTERNAL_STATIC_GR_ZCULL_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_ZCULL_INFO engineZcullInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ROP_INFO
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ROP_INFO (0x20800a2e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x2E" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO {
    NvU32 ropUnitCount;
    NvU32 ropOperationsFactor;
    NvU32 ropOperationsCount;
} NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO engineRopInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_PPC_MASK
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PPC_MASKS (0x20800a30) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x30" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS {
    NvU32 mask[NV2080_CTRL_INTERNAL_GR_MAX_GPC];
} NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS enginePpcMasks[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ENGINE_CONTEXT_PROPERTIES
 * @ref NV2080_CTRL_CMD_GR_GET_ATTRIBUTE_BUFFER_SIZE
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO   (0x20800a32) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x32" */



#define NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT 0x19

typedef struct NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO {
    NvU32 size;
    NvU32 alignment;
} NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO {
    NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO engine[NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT];
} NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO engineContextBuffersInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER (0x20800a34) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x34" */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER {
    NvU8 imla0;
    NvU8 fmla16;
    NvU8 dp;
    NvU8 fmla32;
    NvU8 ffma;
    NvU8 imla1;
    NvU8 imla2;
    NvU8 imla3;
    NvU8 imla4;
} NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER smIssueRateModifier[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS;

/*
 * NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS
 *
 * This command obtains information from physical RM for use by CPU-RM.
 */

#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_CHIP_INFO      (0x20800a36) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS_MESSAGE_ID" */

/*
 * Maximum number of register bases to return.
 * These are indexed by NV_REG_BASE_* constants from gpu.h, and this value needs
 * to be updated if NV_REG_BASE_LAST ever goes over it. See the ct_assert() in gpu.h
 */
#define NV2080_CTRL_INTERNAL_GET_CHIP_INFO_REG_BASE_MAX 16
#define NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS_MESSAGE_ID (0x36U)

typedef struct NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS {
    NvU8   chipSubRev;
    NvU32  emulationRev1;
    NvBool isCmpSku;
    NvU32  bar1Size;
    NvU32  pciDeviceId;
    NvU32  pciSubDeviceId;
    NvU32  pciRevisionId;
    NvU32  regBases[NV2080_CTRL_INTERNAL_GET_CHIP_INFO_REG_BASE_MAX];
} NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS;

/**
 * NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE
 *
 * Set whether or not context switch logging is enabled
 *
 * bEnable
 *    Enable/Disable status for context switch logging
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE (0x20800a37) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS_MESSAGE_ID" */

/**
 * NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE
 *
 * Retrieve whether or not context switch logging is enabled
 *
 * bEnable
 *    Enable/Disable status for context switch logging
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE (0x20800a38) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_INTERNAL_GR_FECS_TRACE_HW_ENABLE_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvBool bEnable;
} NV2080_CTRL_INTERNAL_GR_FECS_TRACE_HW_ENABLE_PARAMS;
#define NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS_MESSAGE_ID (0x37U)

typedef NV2080_CTRL_INTERNAL_GR_FECS_TRACE_HW_ENABLE_PARAMS NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS;
#define NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS_MESSAGE_ID (0x38U)

typedef NV2080_CTRL_INTERNAL_GR_FECS_TRACE_HW_ENABLE_PARAMS NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS;

/**
 * NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET
 *
 * Set read offset into FECS context switch trace record
 *
 * offset
 *   Value indicating number of records by which to offset
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET (0x20800a39) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS_MESSAGE_ID" */

/**
 * NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET
 *
 * Set write offset into FECS context switch trace record
 *
 * offset
 *   Value indicating number of records by which to offset
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET (0x20800a3a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS_MESSAGE_ID" */

/**
 * NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET
 *
 * Get read offset into FECS context switch trace record
 *
 * offset
 *   Value indicating number of records by which to offset
 */

#define NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET (0x20800a3b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_INTERNAL_GR_FECS_TRACE_OFFSET_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
    NvU32 offset;
} NV2080_CTRL_INTERNAL_GR_FECS_TRACE_OFFSET_PARAMS;

#define NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS_MESSAGE_ID (0x39U)

typedef NV2080_CTRL_INTERNAL_GR_FECS_TRACE_OFFSET_PARAMS NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS;
#define NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS_MESSAGE_ID (0x3AU)

typedef NV2080_CTRL_INTERNAL_GR_FECS_TRACE_OFFSET_PARAMS NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS;
#define NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS_MESSAGE_ID (0x3BU)

typedef NV2080_CTRL_INTERNAL_GR_FECS_TRACE_OFFSET_PARAMS NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS;

/**
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE
 *
 * Get size of FECS record
 *
 * fecsRecordSize
 *   Size of FECS record
 */



#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE (0x20800a3d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE {
    NvU32 fecsRecordSize;
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS_MESSAGE_ID (0x3CU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE fecsRecordSize[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE_PARAMS_MESSAGE_ID (0x3DU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE_PARAMS;

/**
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES
 *
 * Get HW defines used to extract information from FECS records
 *
 * fecsRecordSize
 *  Size of FECS record
 *
 * timestampHiTagMask
 *  Mask for high bits of raw timestamp to extract tag
 *
 * timestampHiTagShift
 *  Shift for high bits of raw timestamp to extract tag
 *
 * timestampVMask
 *  Mask to extract timestamp from raw timestamp
 *
 * numLowerBitsZeroShift
 *  Number of bits timestamp is shifted by
 */



#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES (0x20800a3f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x3F" */

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES {
    NvU32 fecsRecordSize;
    NvU32 timestampHiTagMask;
    NvU8  timestampHiTagShift;
    NV_DECLARE_ALIGNED(NvU64 timestampVMask, 8);
    NvU8  numLowerBitsZeroShift;
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES fecsTraceDefines[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES], 8);
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS;

/**
 * NV2080_CTRL_CMD_INTERNAL_GET_DEVICE_INFO_TABLE
 * 
 * Parse the DEVICE_INFO2_TABLE on the physical side and return it to kernel.
 */
typedef struct NV2080_CTRL_INTERNAL_DEVICE_INFO {
    NvU32 faultId;
    NvU32 instanceId;
    NvU32 typeEnum;
    NvU32 resetId;
    NvU32 devicePriBase;
    NvU32 isEngine;
    NvU32 rlEngId;
    NvU32 runlistPriBase;
} NV2080_CTRL_INTERNAL_DEVICE_INFO;
#define NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES 88

#define NV2080_CTRL_CMD_INTERNAL_GET_DEVICE_INFO_TABLE   (0x20800a40) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS {
    NvU32                            numEntries;
    NV2080_CTRL_INTERNAL_DEVICE_INFO deviceInfoTable[NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES];
} NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP              (0x20800a41) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPU_USER_REGISTER_ACCESS_MAP_MAX_COMPRESSED_SIZE  4096
#define NV2080_CTRL_INTERNAL_GPU_USER_REGISTER_ACCESS_MAP_MAX_PROFILING_RANGES 4096

#define NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS {
    NvU32 userRegisterAccessMapSize;
    NvU32 compressedSize;
    NvU8  compressedData[NV2080_CTRL_INTERNAL_GPU_USER_REGISTER_ACCESS_MAP_MAX_COMPRESSED_SIZE];
    NvU32 profilingRangesSize;
    NvU8  profilingRanges[NV2080_CTRL_INTERNAL_GPU_USER_REGISTER_ACCESS_MAP_MAX_PROFILING_RANGES];
} NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS;

typedef struct NV2080_CTRL_INTERNAL_CONSTRUCTED_FALCON_INFO {
    NvU32 engDesc;
    NvU32 ctxAttr;
    NvU32 ctxBufferSize;
    NvU32 addrSpaceList;
    NvU32 registerBase;
} NV2080_CTRL_INTERNAL_CONSTRUCTED_FALCON_INFO;
#define NV2080_CTRL_CMD_INTERNAL_MAX_CONSTRUCTED_FALCONS     0x40

#define NV2080_CTRL_CMD_INTERNAL_GET_CONSTRUCTED_FALCON_INFO (0x20800a42) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS {
    NvU32                                        numConstructedFalcons;
    NV2080_CTRL_INTERNAL_CONSTRUCTED_FALCON_INFO constructedFalconsTable[NV2080_CTRL_CMD_INTERNAL_MAX_CONSTRUCTED_FALCONS];
} NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS;

/**
 * Get GR PDB properties synchronized between Kernel and Physical
 *
 * bPerSubCtxheaderSupported
 *   @ref PDB_PROP_GR_SUPPORTS_PER_SUBCONTEXT_CONTEXT_HEADER
 */



#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES (0x20800a48) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x48" */

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES {
    NvBool bPerSubCtxheaderSupported;
} NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES pdbTable[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_WRITE_INST_MEM
 *
 *   This command writes instance memory information in the display hardware registers.
 *
 *   instMemPhysAddr
 *     GPU physical address or IOVA address of the display instance memory.
 *
 *   instMemSize
 *     Size of the display instance memory.
 *
 *   instMemAddrSpace
 *     Address space of the display instance memory.
 *
 *   instMemCpuCacheAttr
 *     Cache attribute of the display instance memory.
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_WRITE_INST_MEM (0x20800a49) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS_MESSAGE_ID (0x49U)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 instMemPhysAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 instMemSize, 8);
    NvU32 instMemAddrSpace;
    NvU32 instMemCpuCacheAttr;
} NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS;

/*
 * NV2080_CTRL_INTERNAL_GPU_RECOVER_ALL_COMPUTE_CONTEXTS
 *
 * This command issues RC recovery for all compute contexts running on the given GPU.
 */
#define NV2080_CTRL_CMD_INTERNAL_RECOVER_ALL_COMPUTE_CONTEXTS (0x20800a4a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x4A" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_IP_VERSION
 *
 *    This command obtains IP version of display engine for use by Kernel RM.
 *
 *   ipVersion
 *     IP Version of display engine.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED - DISP has been disabled
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_IP_VERSION       (0x20800a4b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS_MESSAGE_ID (0x4BU)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS {
    NvU32 ipVersion;
} NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_GET_SMC_MODE
 *
 *  This command determines the current status of MIG MODE from Physical RM.
 *
 *  smcMode [OUT]
 *     Current MIG MODE of the GPU. Values range NV2080_CTRL_GPU_INFO_GPU_SMC_MODE*
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_SMC_MODE (0x20800a4c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS_MESSAGE_ID (0x4CU)

typedef struct NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS {
    NvU32 smcMode;
} NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR
 *
 *   head
 *     This parameter specifies the head for which the callback is to be registered/unregistered. This value must be
 *     less than the maximum number of heads supported by the GPU subdevice.
 *
 *   rgLineNum
 *     This indicates the RG scanout line number on which the callback will be executed.
 *
 *   intrLine
 *     Enable: [out] Which interrupt line was allocated for this head.
 *     Disable: [in] Which interrupt line to deallocate.
 *
 *   bEnable
 *     Should we allocate or deallocate an interrupt line?
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC - There was no available interrupt to allocate.
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR (0x20800a4d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS_MESSAGE_ID (0x4DU)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS {
    NvU32  head;
    NvU32  rgLineNum;
    NvU32  intrLine;
    NvBool bEnable;
} NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS;

/*!
 * NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO
 *
 * Description of a supported GPU instance.
 *
 *   partitionFlag [OUT]
 *     Allocation flag to be used to allocate a partition with this profile.
 *
 *   grCount [OUT]
 *     # GR engines
 *
 *   gpcCount [OUT]
 *     # total gpcs
 *
 *   veidCount [OUT]
 *     # total veids
 *
 *   smCount [OUT]
 *     # total SMs
 *
 *   ceCount [OUT]
 *     # CE engines
 *
 *   nvEncCount [OUT]
 *     # NVENC engines
 *
 *   nvDecCount [OUT]
 *     # NVDEC engines
 *
 *   nvJpgCount [OUT]
 *     # NVJPG engines
 *
 *   nvOfaCount [OUT]
 *     # NVOFA engines
 */
#define NV2080_CTRL_INTERNAL_GRMGR_PARTITION_MAX_TYPES      10



typedef struct NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO {
    NvU32 partitionFlag;
    NvU32 grCount;
    NvU32 gpcCount;
    NvU32 veidCount;
    NvU32 smCount;
    NvU32 ceCount;
    NvU32 nvEncCount;
    NvU32 nvDecCount;
    NvU32 nvJpgCount;
    NvU32 nvOfaCount;
} NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO;

/*!
 * NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS
 *
 * Returns the list of supported GPU instance profiles.
 *
 *   count [OUT]
 *     Number of supported profiles.
 *
 *   table [OUT]
 *     Supported profiles.
 */
typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS {
    NvU32                                    count;
    NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO table[NV2080_CTRL_INTERNAL_GRMGR_PARTITION_MAX_TYPES];
} NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM
 *
 *  This command instructs the physical MemorySystem to set up memory partitioning
 *  exterior boundaries in hardware.
 *
 *  partitionableMemSize [input]
 *      Size of the partitionable memory in bytes
 *
 *  bottomRsvdSize [input]
 *      Size of the reserved region below partitionable memory in bytes
 *
 *  topRsvdSize [input]
 *      Size of the reserved region above partitionable memory in bytes
 *
 *  partitionableStartAddr [output]
 *      Start address of the partitionable memory, aligned to HW constraints
 *
 *  partitionableEndAddr [output]
 *      End address of the partitionable memory, aligned to HW constraints
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM (0x20800a51) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 partitionableMemSize, 8);
    NV_DECLARE_ALIGNED(NvU64 bottomRsvdSize, 8);
    NV_DECLARE_ALIGNED(NvU64 topRsvdSize, 8);
    NV_DECLARE_ALIGNED(NvU64 partitionableStartAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 partitionableEndAddr, 8);
} NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS;



typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 engineMask, 8);
} NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS;


/*!
 * NV2080_CTRL_INTERNAL_MEMDESC_INFO
 *
 * A generic container structure representing a memory region to be used as a
 * component of other control call parameters.
 *
 */
typedef struct NV2080_CTRL_INTERNAL_MEMDESC_INFO {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NvU32 addressSpace;
    NvU32 cpuCacheAttrib;
} NV2080_CTRL_INTERNAL_MEMDESC_INFO;

#define NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_BUFFERS         2
#define NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_ID              64
/*!
 * NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS
 *
 * Promote a single partition's runlist buffers allocated by kernel Client RM to Physical RM
 *
 *   rlBuffers [IN]
 *     2D array of runlist buffers for a single partition
 *
 *   runlistIdMask [IN]
 *     Mask of runlists belonging to partition
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS (0x20800a53) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS_MESSAGE_ID (0x53U)

typedef struct NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_MEMDESC_INFO rlBuffers[NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_ID][NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_BUFFERS], 8);
    NV_DECLARE_ALIGNED(NvU64 runlistIdMask, 8);
    NvU32 swizzId;
} NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_SET_IMP_INIT_INFO (0x20800a54) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS_MESSAGE_ID (0x54U)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS {
    TEGRA_IMP_IMPORT_DATA tegraImpImportData;
} NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P
 *
 * Binds local GFID for SR-IOV P2P requests
 *
 *   localGfid [IN]
 *     GFID to bind in the P2P source GPU
 *
 *   peerId [IN]
 *     Peer ID of the P2P destination GPU
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P (0x20800a55) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS_MESSAGE_ID (0x55U)

typedef struct NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS {
    NvU32 localGfid;
    NvU32 peerId;
} NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P
 *
 * Binds remote GFID for SR-IOV P2P requests
 *
 *   remoteGfid [IN]
 *     GFID to bind in the P2P destination GPU
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P (0x20800a56) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS_MESSAGE_ID (0x56U)

typedef struct NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS {
    NvU32 remoteGfid;
} NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_BUS_FLUSH_WITH_SYSMEMBAR
 *
 * This command triggers a sysmembar to flush VIDMEM writes.
 * This command accepts no parameters.
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_FLUSH_WITH_SYSMEMBAR    (0x20800a70) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x70" */

/*
 * NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL
 *
 * Setup local PCIE P2P Mailbox
 *
 * local2Remote[IN]
 *   Local peer ID of remote gpu on local gpu
 *
 * remote2Local[IN]
 *   Remote peer ID of local gpu on remote gpu
 *
 * localP2PDomainRemoteAddr[IN]
 *   P2P domain remote address on local gpu
 *
 * remoteP2PDomainLocalAddr[IN]
 *   P2P domain local address on remote gpu
 *
 * remoteWMBoxLocalAddr[IN]
 *   Local mailbox address on remote gpu
 *
 * p2pWmbTag[OUT]
 *   Tag for mailbox to transport from local to remote GPU
 *
 * bNeedWarBug999673[IN]
 *   Set to true if WAR for bug 999673 is required
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL (0x20800a71) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS_MESSAGE_ID (0x71U)

typedef struct NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS {
    NvU32  local2Remote;
    NvU32  remote2Local;
    NV_DECLARE_ALIGNED(NvU64 localP2PDomainRemoteAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 remoteP2PDomainLocalAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 remoteWMBoxLocalAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 p2pWmbTag, 8);
    NvBool bNeedWarBug999673;
} NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS;

 /*
 * NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE
 *
 * Setup remote PCIE P2P Mailbox
 *
 * local2Remote[IN]
 *   Local peer ID of remote gpu on local gpu
 *
 * remote2Local[IN]
 *   Remote peer ID of local gpu on remote gpu
 *
 * localP2PDomainRemoteAddr[IN]
 *   P2P domain remote address on local gpu
 *
 * remoteP2PDomainLocalAddr[IN]
 *   P2P domain local address on remote gpu
 *
 * remoteWMBoxAddrU64[IN]
 *   Mailbox address on remote gpu
 *
 * p2pWmbTag[IN]
 *   Tag for mailbox to transport from local to remote GPU
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE (0x20800a72) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS_MESSAGE_ID (0x72U)

typedef struct NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS {
    NvU32 local2Remote;
    NvU32 remote2Local;
    NV_DECLARE_ALIGNED(NvU64 localP2PDomainRemoteAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 remoteP2PDomainLocalAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 remoteWMBoxAddrU64, 8);
    NV_DECLARE_ALIGNED(NvU64 p2pWmbTag, 8);
} NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_BUS_DESTROY_P2P_MAILBOX
 *
 * Destroy PCIE P2P Mailbox
 *
 * peerIdx[IN]
 *   Peer ID of the P2P destination GPU
 *
 * bNeedWarBug999673[IN]
 *   Set to true if WAR for bug 999673 is required
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_DESTROY_P2P_MAILBOX (0x20800a73) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS_MESSAGE_ID (0x73U)

typedef struct NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS {
    NvU32  peerIdx;
    NvBool bNeedWarBug999673;
} NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING
 *
 * Create C2C mapping to a given peer GPU
 *
 * peerId[IN]
 *   Peer ID for local to remote GPU
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING (0x20800a74) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS {
    NvU32 peerId;
} NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING
 *
 * Remove C2C mapping to a given peer GPU
 *
 * peerId[IN]
 *   Peer ID for local to remote GPU
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING (0x20800a75) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS_MESSAGE_ID (0x75U)

typedef struct NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS {
    NvU32 peerId;
} NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES
 *
 * Retrieves the corresponding SPAs (per the given GFID's VMMU mappings)
 * for the given array of GPAs.
 *
 *   gfid [IN]
 *     GFID to translate GPAs for
 *
 *   numEntries [IN]
 *     Number of entries (<= NV2080_CTRL_CMD_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES)
 *     to translate (i.e. number of elements in gpaEntries)
 *
 *   gpaEntries [IN]
 *     Array of GPAs to translate
 *
 *   spaEntries [OUT]
 *     Resulting array of SPAs
 */
#define NV2080_CTRL_CMD_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES (0x20800a57) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_VMMU_MAX_SPA_FOR_GPA_ENTRIES     128

#define NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS_MESSAGE_ID (0x57U)

typedef struct NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS {
    NvU32 gfid;
    NvU32 numEntries;
    NV_DECLARE_ALIGNED(NvU64 gpaEntries[NV2080_CTRL_INTERNAL_VMMU_MAX_SPA_FOR_GPA_ENTRIES], 8);
    NV_DECLARE_ALIGNED(NvU64 spaEntries[NV2080_CTRL_INTERNAL_VMMU_MAX_SPA_FOR_GPA_ENTRIES], 8);
} NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER
 *
 * Pass required pushbuffer parameters from kernel RM to Physical RM
 *
 *   addressSpace [IN]
 *     Address space of pushbuffer whtether it is ADDR_SYSMEM or ADDR_FBMEM
 *
 *   physicalAddr [IN]
 *     Physical address of pushbuffer
 *
 *   addressSpace [IN]
 *     Limit of the pushbuffer address, it should be less than 4K
 *
 *   cacheSnoop [IN]
 *     Cachesnoop supported or not
 *
 *   channelInstance [IN]
 *     Channel instance pass by the client to get corresponding dispchannel
 *
 *   hclass [IN]
 *     External class ID pass by the client to get the channel class
 *
 *   valid [IN]
 *     This bit indicates whether pushbuffer parameters are valid or not
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER (0x20800a58) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS_MESSAGE_ID (0x58U)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS {
    NvU32  addressSpace;
    NV_DECLARE_ALIGNED(NvU64 physicalAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 limit, 8);
    NvU32  cacheSnoop;
    NvU32  hclass;
    NvU32  channelInstance;
    NvBool valid;
} NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GMMU_GET_STATIC_INFO
 *
 *   This command obtains information from physical RM for use by CPU-RM.
 *
 *   replayableFaultBufferSize
 *     Default size of replayable fault buffer
 *
 *   nonReplayableFaultBufferSize
 *     Default size of non-replayable fault buffer
 *
 */

#define NV2080_CTRL_CMD_INTERNAL_GMMU_GET_STATIC_INFO (0x20800a59) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS_MESSAGE_ID (0x59U)

typedef struct NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS {
    NvU32 replayableFaultBufferSize;
    NvU32 nonReplayableFaultBufferSize;
} NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_CTXSW_MODES
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_GET_CTXSW_MODES           (0x20800a5a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x5A" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE
 *
 * Get heap reservation size needed by different module
 */
#define NV2080_CTRL_CMD_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE (0x20800a5b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS_MESSAGE_ID (0x5BU)

typedef struct NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS {
    NvU32 moduleIndex;
    NvU32 size;
} NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_INTR_GET_KERNEL_TABLE
 *
 *   tableLen [OUT]
 *     Number of valid records in table field.
 *
 *   table    [OUT]
 *     Interrupt table for Kernel RM.
 */
#define NV2080_CTRL_CMD_INTERNAL_INTR_GET_KERNEL_TABLE (0x20800a5c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_INTR_MAX_TABLE_SIZE       128

typedef struct NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_ENTRY {
    NvU16 engineIdx;
    NvU32 pmcIntrMask;
    NvU32 vectorStall;
    NvU32 vectorNonStall;
} NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_ENTRY;

#define NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS_MESSAGE_ID (0x5CU)

typedef struct NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS {
    NvU32                                            tableLen;
    NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_ENTRY table[NV2080_CTRL_INTERNAL_INTR_MAX_TABLE_SIZE];
} NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS;

/* Index to retrieve the needed heap space for specific module */
#define NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_GR           (0x00000000)

/*
 * NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK
 *
 * Checking if the reservation / release of the PERFMON HW is possible
 *
 *   bReservation [IN]
 *     NV_TRUE -> request for reservation, NV_FALSE -> request for release
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK (0x20800a98) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS_MESSAGE_ID" */


#define NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS_MESSAGE_ID (0x98U)

typedef struct NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS {
    NvBool bReservation;
} NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET
 *
 * Reserving / Releasing PERFMON HW
 *
 *   bReservation [IN]
 *     NV_TRUE -> request for reservation, NV_FALSE -> request for release
 *
 *   bClientHandlesGrGating [IN]
 *     DM-TODO: Add comment for this
 *
 *   bRmHandlesIdleSlow [IN]
 *     If the IDLE slowdown is required
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET (0x20800a99) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS_MESSAGE_ID (0x99U)

typedef struct NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS {
    NvBool bReservation;
    NvBool bClientHandlesGrGating;
    NvBool bRmHandlesIdleSlow;
} NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES
 *
 * Get active display devices
 *
 *   displayMask [OUT]
 *     Get the mask of the active display devices in VBIOS
 *
 *   numHeads [OUT]
 *     Number of heads display supported.
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES (0x20800a5d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS_MESSAGE_ID (0x5DU)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS {

    NvU32 displayMask;
    NvU32 numHeads;
} NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS;



/*
 * NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES
 *
 * Get FB Mem page ranges for all possible swizzIds
 *
 *   fbMemPageRanges [OUT]
 *      Mem page ranges for each swizzId in the form of {lo, hi}
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES (0x20800a60) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MAX_SWIZZ_ID                                      15

typedef struct NV2080_CTRL_INTERNAL_NV_RANGE {
    NV_DECLARE_ALIGNED(NvU64 lo, 8);
    NV_DECLARE_ALIGNED(NvU64 hi, 8);
} NV2080_CTRL_INTERNAL_NV_RANGE;

#define NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NV_RANGE fbMemPageRanges[NV2080_CTRL_INTERNAL_MAX_SWIZZ_ID], 8);
} NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_CHANNELS
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to get number of channels for a given runlist ID
 *
 *   runlistId   [IN]
 *   numChannels [OUT]
 */
#define NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_CHANNELS (0x20800a61) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS_MESSAGE_ID (0x61U)

typedef struct NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS {
    NvU32 runlistId;
    NvU32 numChannels;
} NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PROFILES
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_PROFILES
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PROFILES                    (0x20800a63) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x63" */

/*!
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_VALID_SWIZZID_MASK
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_VALID_SWIZZID_MASK
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_VALID_SWIZZID_MASK          (0x20800a64) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x64" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES
 * NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES       (0x20800a65) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x65" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES
 * NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES (0x20800a66) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x66" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG
 *
 * This command retrieves memory config from HW
 *
 *   memBoundaryCfgA [OUT]
 *      Memory boundary config A (4KB aligned)
 *
 *   memBoundaryCfgB [OUT]
 *      Memory boundary config B (4KB aligned)
 *
 *   memBoundaryCfgC [OUT]
 *      Memory boundary config C (64KB aligned)
 *
 *   memBoundaryCfg [OUT]
 *      Memory boundary config (64KB aligned)
 *      
 *   memBoundaryCfgValInit [OUT]
 *      Memory boundary config initial value (64KB aligned)
 */
#define NV2080_CTRL_CMD_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG                  (0x20800a67) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x67" */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG                   (0x20800a68) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x68" */

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 memBoundaryCfgA, 8);
    NV_DECLARE_ALIGNED(NvU64 memBoundaryCfgB, 8);
    NvU32 memBoundaryCfgC;
    NvU32 memBoundaryCfg;
    NvU32 memBoundaryCfgValInit;
} NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE  (0x20800a6b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_SIZE 8

#define NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS_MESSAGE_ID (0x6BU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS {
    NvU32 data[NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_SIZE];
} NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_RC_WATCHDOG_TIMEOUT
 *
 * Invoke RC recovery after watchdog timeout is hit.
 */
#define NV2080_CTRL_CMD_INTERNAL_RC_WATCHDOG_TIMEOUT     (0x20800a6a) /* finn: Evaluated from "((FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x6a)" */

/* !
 *  This command disables cuda limit activation at teardown of the client.
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_CUDA_LIMIT_DISABLE (0x20800a7a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x7A" */

/*
 *  This command is cleaning up OPTP when a client is found to have
 *  been terminated unexpectedly.
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_OPTP_CLI_CLEAR     (0x20800a7c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x7C" */


/*!
 * This command can be used to boost P-State up one level or to the highest for a limited
 * duration for the associated subdevice. Boosts from different clients are being tracked
 * independently. Note that there are other factors that can limit P-States so the resulting
 * P-State may differ from expectation.
 *
 *   flags
 *     This parameter specifies the actual command. _CLEAR is to clear existing boost.
 *     _BOOST_1LEVEL is to boost P-State one level higher. _BOOST_TO_MAX is to boost
 *     to the highest P-State.
 *   duration
 *     This parameter specifies the duration of the boost in seconds. This has to be less
 *     than NV2080_CTRL_PERF_BOOST_DURATION_MAX.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_2X       (0x20800a9a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X_MESSAGE_ID (0x9AU)

typedef struct NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X {

    NvBool flags;
    NvU32  duration;
} NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X;

#define NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_PSTATE   0U
#define NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_GPCCLK   1U
#define NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_LAST     NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_GPCCLK
#define NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM      (0x2U) /* finn: Evaluated from "NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_LAST + 1" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_CONTROL
 *
 * Activate/Deactivate GPU Boost Sync algorithm
 *
 *   bActivate [IN]
 *     GPU Boost Sync algorithm:
 *      NV_TRUE  -> activate
 *      NV_FALSE -> deactivate
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_CONTROL (0x20800a7e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS_MESSAGE_ID (0x7EU)

typedef struct NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS {
    NvBool bActivate;
} NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS
 *
 * Apply given limits to a specific subdevice
 *
 *   flags [IN]
 *     DM-TODO: write description here
 *
 *   bBridgeless [IN]
 *     Bridgeless information, for now supporting only MIO bridges
 * 
 *   currLimits
 *     Array of limits that will be applied
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS (0x20800a7f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS_MESSAGE_ID (0x7FU)

typedef struct NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS {
    NvU32  flags;
    NvBool bBridgeless;
    NvU32  currLimits[NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM];
} NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO
 *
 * Data for GPU Boost Sync structure initialization
 *
 *   hysteresisus [OUT]
 *     Hysteresis value for GPU Boost synchronization hysteresis algorithm.
 *
 *   bHystersisEnable [OUT]
 *     hysteresis algorithm for SLI GPU Boost synchronization:
 *      NV_TRUE  -> enabled,
 *      NV_FALSE -> disabled
 * 
 *   bSliGpuBoostSyncEnable [OUT]
 *     SLI GPU Boost feature is:
 *      NV_TRUE  -> enabled,
 *      NV_FALSE -> disabled
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO (0x20800a80) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS_MESSAGE_ID (0x80U)

typedef struct NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 hysteresisus, 8);
    NvBool bHystersisEnable;
    NvBool bSliGpuBoostSyncEnable;
} NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_FAULT_BUFFER
 *
 * This command sends replayable fault buffer pages allocated by CPU-RM
 * to be setup and enabled in physical RM.
 *
 * hClient
 *   Client handle.
 *
 * hObject
 *   Object handle.
 *
 * faultBufferSize
 *   Size of the replayable fault buffer to register.
 *
 * faultBufferPteArray
 *   Pages of replayable fault buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_FAULT_BUFFER (0x20800a9b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_FAULT_BUFFER_MAX_PAGES    256
#define NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS_MESSAGE_ID (0x9BU)

typedef struct NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS {
    NvHandle hClient;
    NvHandle hObject;
    NvU32    faultBufferSize;
    NV_DECLARE_ALIGNED(NvU64 faultBufferPteArray[NV2080_CTRL_INTERNAL_GMMU_FAULT_BUFFER_MAX_PAGES], 8);
} NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_FAULT_BUFFER
 *
 * This command requests physical RM to disable the replayable fault buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_FAULT_BUFFER             (0x20800a9c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x9C" */

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER
 *
 * This command sends client shadow fault buffer pages allocated by CPU-RM
 * to be setup and enabled in physical RM.
 *
 * shadowFaultBufferQueuePhysAddr
 *   Physical address of shadow fault buffer queue.
 *
 * faultBufferSize
 *   Size of the client shadow fault buffer to register.
 *
 * shadowFaultBufferPteArray
 *   Pages of client shadow fault buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER (0x20800a9d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_CLIENT_SHADOW_FAULT_BUFFER_MAX_PAGES    1500
#define NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID (0x9DU)

typedef struct NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 shadowFaultBufferQueuePhysAddr, 8);
    NvU32 shadowFaultBufferSize;
    NV_DECLARE_ALIGNED(NvU64 shadowFaultBufferPteArray[NV2080_CTRL_INTERNAL_GMMU_CLIENT_SHADOW_FAULT_BUFFER_MAX_PAGES], 8);
} NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER
 *
 * This command requests physical RM to disable the client shadow fault buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER (0x20800a9e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x9E" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_3X
 *
 * This command can be used to boost P-State the highest for a limited
 * duration for the associated subdevice. Boosts from different clients are being tracked
 * independently. Note that there are other factors that can limit P-States so the resulting
 * P-State may differ from expectation.
 *
 *   flags [IN]
 *     This parameter specifies the actual command. _CLEAR is to clear existing boost.
 *     and _BOOST_TO_MAX is to boost to the highest P-State.
 *
 *   boostDuration [IN]
 *     This parameter specifies the duration of the boost in seconds. This has to be less
 *     than NV2080_CTRL_PERF_BOOST_DURATION_MAX.
 *
 *   gfId [IN]
 *     This specifies Id of the Kernel RM that is requesting the Boost
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_3X                          (0x20800aa0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X_MESSAGE_ID (0xA0U)

typedef struct NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X {

    NvU32 flags;
    NvU32 boostDuration;
    NvU32 gfId;
} NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_CLEAR_3X
 *
 * Clear the boost for specific Kernel RM
 *
 *   bIsCudaClient [IN]
 *     Specifies if the request is for clearing the CUDA boost or regular boost
 *     NV_TRUE -> CUDA boost, NV_FALSE otherwise
 *
 *   gfId [IN]
 *     Specifies Id of the Kernel RM that is requesting Boost clear
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_CLEAR_3X (0x20800aa1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X_MESSAGE_ID (0xA1U)

typedef struct NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X {

    NvBool bIsCudaClient;
    NvU32  gfId;
} NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X;

/*
 * NV2080_CTRL_CMD_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO
 *
 * Retrieves skyline information about the GPU. Params are sized to currently known max
 * values, but will need to be modified in the future should that change.
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO          (0x208038a2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GRMGR_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_SKYLINES            8
#define NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_NON_SINGLETON_VGPCS 8
/*!
 * NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO
 * skylineVgpcSize[OUT]
 *      - TPC count of non-singleton VGPCs
 * singletonVgpcMask[OUT]
 *      - Mask of active Singletons
 * maxInstances[OUT]
 *      - Max allowed instances of this skyline concurrently on a GPU
 * computeSizeFlag
 *      - One of NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_* flags which is associated with this skyline
 * numNonSingletonVgpcs
 *      - Number of VGPCs with non-zero TPC counts which are not singletons
 */
typedef struct NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO {
    NvU8  skylineVgpcSize[NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_NON_SINGLETON_VGPCS];
    NvU32 singletonVgpcMask;
    NvU32 maxInstances;
    NvU32 computeSizeFlag;
    NvU32 numNonSingletonVgpcs;
} NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO;

/*!
 * NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS
 * skylineTable[OUT]
 *      - TPC count of non-singleton VGPCs
 *      - Mask of singleton vGPC IDs active
 *      - Max Instances of this skyline possible concurrently
 *      - Associated compute size with the indexed skyline
 *      - Number of VGPCs with non-zero TPC counts which are not singletons
 * validEntries[OUT]
 *      - Number of entries which contain valid info in skylineInfo
 */
#define NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS_MESSAGE_ID (0xA2U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO skylineTable[NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_SKYLINES];
    NvU32                                   validEntries;
} NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GPU_SET_PARTITIONING_MODE
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_PARTITIONING_MODE  (0x20800aa3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA3" */

/*!
 * @ref NV2080_CTRL_CMD_GPU_CONFIGURE_PARTITION
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_CONFIGURE_GPU_INSTANCE (0x20800aa4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA4" */

/*!
 * @ref NV2080_CTRL_CMD_GPU_SET_PARTITIONS
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES      (0x20800aa5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA5" */

/*!
 * @ref NV2080_CTRL_CMD_GPU_GET_PARTITIONS
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_GET_GPU_INSTANCES      (0x20800aa6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA6" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED
 *
 * Tell Physical RM whether any ZBC-kind surfaces are allocated.
 * If PF and all VFs report false, ZBC table can be flushed by Physical RM.
 *
 *   bZbcReferenced [IN]
 *     NV_TRUE -> ZBC-kind (and no _SKIP_ZBCREFCOUNT flag) are allocated in Kernel RM
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED     (0x20800a69) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_MESSAGE_ID (0x69U)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS {
    NvBool bZbcSurfacesExist;
} NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE
 *
 * Export the resource and placement information about a gpu instance such that a
 * similar gpu instance can be recreated from scratch in the same position on the
 * same or similar GPU. Note that different GPUs may have different physical
 * resources due to floorsweeping, and an imported gpu instance is not guaranteed
 * to get the exact same resources as the exported gpu instance, but the imported
 * gpu instance should behave identically with respect to fragmentation and
 * placement / span positioning.
 */
#define NV2080_CTRL_CMD_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE                     (0x20800aa7) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA7" */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE                      (0x20800aa8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA8" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE
 *
 * Create a gpu instance resembling the exported instance info. Note that
 * different GPUs may have different physical resources due to floorsweeping,
 * and an imported gpu instance is not guaranteed to get the exact same resources
 * as the exported gpu instance, but the imported gpu instance should behave
 * identically with respect to fragmentation and placement / span positioning.
 */
#define NV2080_CTRL_CMD_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE                     (0x20800aa9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xA9" */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE                      (0x20800aaa) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xAA" */

#define NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_MAX_ENGINES_MASK_SIZE 4
typedef struct NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO {
    NV_DECLARE_ALIGNED(NvU64 enginesMask[NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_MAX_ENGINES_MASK_SIZE], 8);
    NvU32 partitionFlags;
    NvU32 gpcMask;
    NvU32 veidOffset;
    NvU32 veidCount;
} NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO;

typedef struct NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS {
    NvU32 swizzId;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO info, 8);
} NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT
 *
 * Invalidate and/or evict the L2 cache
 *
 *   flags [IN]
 *     flags that specify required actions
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT (0x20800a6c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS_MESSAGE_ID (0x6cU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS {
    NvU32 flags;
} NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS;

#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_ALL          (0x00000001)
#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_FIRST        (0x00000002)
#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_LAST         (0x00000004)
#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_NORMAL       (0x00000008)
#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_CLEAN        (0x00000010)
#define NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_FLAGS_WAIT_FB_PULL (0x00000020)

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_FLUSH_L2_ALL_RAMS_AND_CACHES
 *
 * Flush all L2 Rams and Caches using the ELPG flush
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_FLUSH_L2_ALL_RAMS_AND_CACHES       (0x20800a6d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x6D" */



/*!
 * NV2080_CTRL_CMD_INTERNAL_BIF_GET_STATIC_INFO
 *
 * This command obtains information from physical RM for use by CPU-RM
 *
 * Data fields ->
 *   bPcieGen4Capable - tells whether PCIe is Gen4 capable
 */
#define NV2080_CTRL_CMD_INTERNAL_BIF_GET_STATIC_INFO                       (0x20800aac) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS_MESSAGE_ID (0xacU)

typedef struct NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS {
    NvBool bPcieGen4Capable;
    NvBool bIsC2CLinkUp;
    NV_DECLARE_ALIGNED(NvU64 dmaWindowStartAddress, 8);
} NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG
 *
 * Program HSHUB Peer Conn Config space.
 *
 *    programPeerMask[IN]
 *      If nonzero, the peer mask for programming peers based on hshub connectivity.
 *
 *    invalidatePeerMask[IN]
 *      If nonzero, the peer mask for invalidating peers.
 *
 *    programPciePeerMask[IN]
 *      If nonzero, the peer mask for programming peers in pcie case.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_WARN_NOTHING_TO_DO
 *     If all peer masks are zero.
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG (0x20800a88) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS {
    NvU32 programPeerMask;
    NvU32 invalidatePeerMask;
    NvU32 programPciePeerMask;
} NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_FIRST_LINK_PEER_ID
 *
 * Given a mask of link ids, find the first with a valid peerId.
 *
 *    linkMask[IN]
 *      Mask of linkIds to check.
 *
 *    peerId[OUT]
 *      The peerId for the lowest-index link with a valid peerId, if any.
 *      If none found, NV2080_CTRLINTERNAL_HSHUB_FIRST_LINK_PEER_ID_INVALID_PEER (return value will still be NV_OK).
 *
 * Possible status values returned are:
 *   NV_OK
 *   
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_FIRST_LINK_PEER_ID          (0x20800a89) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_INVALID_PEER 0xffffffff

#define NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS {
    NvU32 linkMask;
    NvU32 peerId;
} NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS
 *
 * Get the Hshub Ids connected to the other end of links.
 *
 *    linkMask[IN]
 *      A mask of link ids to query.
 *
 *    hshubIds[OUT]
 *      For each set bit in the link mask, the peer Hshub Id.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_WARN_NOTHING_TO_DO
 *     If the mask is zero.
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS        (0x20800a8a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_TABLE_SIZE 32

#define NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS_MESSAGE_ID (0x8aU)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS {
    NvU32 linkMask;
    NvU8  hshubIds[NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_TABLE_SIZE];
} NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_NUM_UNITS
 *
 * Return the number of HSHUB units.
 *
 *    numHshubs[OUT]
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_NUM_UNITS (0x20800a8b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS_MESSAGE_ID (0x8bU)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS {
    NvU32 numHshubs;
} NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_NEXT_HSHUB_ID
 *
 * Return the next hshubId after the given hshubId.
 *
 *    hshubId[IN/OUT]
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_NEXT_HSHUB_ID (0x20800a8c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS_MESSAGE_ID (0x8cU)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS {
    NvU8 hshubId;
} NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_COMPUTE_PEER_ADDR
 *
 * Enable compute peer addressing mode
 * This command accepts no parameters.
 */

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_COMPUTE_PEER_ADDR (0x20800aad) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xad" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR
 *
 * Get/Set NVSwitch fabric address for FLA
 *
 * [In] bGet
 *     Whether to get or set the NVSwitch fabric address
 * [In/Out] addr
 *     Address that is to be set or retrieved.
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS_MESSAGE_ID (0xaeU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS {
    NvBool bGet;
    NV_DECLARE_ALIGNED(NvU64 addr, 8);
} NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR (0x20800aae) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_BIF_GET_ASPM_L1_FLAGS
 *
 * This command obtains information from physical RM for use by CPU-RM
 *
 * Data fields ->
 *   bCyaMaskL1
 *   bEnableAspmDtL1
 */
#define NV2080_CTRL_CMD_INTERNAL_BIF_GET_ASPM_L1_FLAGS               (0x20800ab0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS_MESSAGE_ID (0xb0U)

typedef struct NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS {
    NvBool bCyaMaskL1;
    NvBool bEnableAspmDtL1;
} NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT
 *
 * Sets number of VM slots that are active in VGPU's scheduler
 *
 *   maxActiveVGpuVMCount [IN]
 *     Number of VM slots that are active in vGPU's scheduler.
 *
 * Possible status values returned are:
 * NV_OK
 * NV_ERR_OUT_OF_RANGE - Passed value is out of range
 * NV_ERR_NO_MEMORY    - Out of memory
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT              (0x20800ab1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS_MESSAGE_ID" */

/*!
 * Maximum value of VM slots that are active in vGPU's scheduler.
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_CF_CONTROLLERS_MAX_ACTIVE_VGPU_VM_COUNT_MAX_VALUE 32

#define NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS_MESSAGE_ID (0xB1U)

typedef struct NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS {
    NvU8 maxActiveVGpuVMCount;
} NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_DISABLE_NVLINK_PEERS
 *
 * Disable all NVLINK FB peers
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_DISABLE_NVLINK_PEERS         (0x20800a6e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x6E" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE
 *
 * Program GPU in raw / legacy compression mode
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE (0x20800a6f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS_MESSAGE_ID (0x6fU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS {
    NvBool bRawMode;
} NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS;



/*!
 * NV2080_CTRL_CMD_INTERNAL_BUS_GET_PCIE_P2P_CAPS
 *
 * This command returns the GPU's PCIE P2P caps
 *
 *   [in]  bCommonPciSwitchFound
 *     All GPUs are under the same PCI switch
 *   [out] p2pReadCapsStatus
 *   [out] p2pWriteCapsStatus
 *     These members returns status of all supported p2p capabilities. Valid
 *     status values include:
 *       NV0000_P2P_CAPS_STATUS_OK
 *         P2P capability is supported.
 *       NV0000_P2P_CAPS_STATUS_CHIPSET_NOT_SUPPORTED
 *         Chipset doesn't support p2p capability.
 *       NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED
 *         GPU doesn't support p2p capability.
 *       NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED
 *         IOH topology isn't supported. For e.g. root ports are on different
 *         IOH.
 *       NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY
 *         P2P Capability is disabled by a regkey.
 *       NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED
 *         P2P Capability is not supported.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_INTERNAL_GET_PCIE_P2P_CAPS (0x20800ab8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS_MESSAGE_ID (0xB8U)

typedef struct NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS {
    NvBool bCommonPciSwitchFound;
    NvU8   p2pReadCapsStatus;
    NvU8   p2pWriteCapsStatus;
} NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_BIF_SET_PCIE_RO
 *
 * Enable/disable PCIe Relaxed Ordering.
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_BIF_SET_PCIE_RO (0x20800ab9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS_MESSAGE_ID (0xb9U)

typedef struct NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS {
    // Enable/disable PCIe relaxed ordering
    NvBool enableRo;
} NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE
 *
 * Query Coherent FB Aperture Size.
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE (0x20800aba) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS_MESSAGE_ID (0xbaU)

typedef struct NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS {
    // Get Coherent Fb Aperture Size
    NV_DECLARE_ALIGNED(NvU64 coherentFbApertureSize, 8);
} NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS;

/* ctrl2080internal_h */
