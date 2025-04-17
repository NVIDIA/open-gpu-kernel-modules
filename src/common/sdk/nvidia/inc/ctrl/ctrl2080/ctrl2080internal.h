/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080internal.finn
//

#include "nvimpshared.h"
#include "cc_drv.h"
#include "ctrl/ctrl2080/ctrl2080base.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080gr.h"        /* Some controls derivative of 2080gr */
#include "ctrl/ctrl0080/ctrl0080msenc.h"     /* NV0080_CTRL_MSENC_CAPS_TBL_SIZE    */
#include "ctrl/ctrl0080/ctrl0080bsp.h"       /* NV0080_CTRL_BSP_CAPS_TBL_SIZE      */
#include "ctrl/ctrl2080/ctrl2080fifo.h"      /* NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO */
#include "ctrl/ctrl0073/ctrl0073system.h"    /* NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS */
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl90f1.h"
#include "ctrl/ctrl30f1.h"
#include "nvcfg_sdk.h"

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
 *   numHeads
 *     Display IP v02_01 and later.
 *     Provides the number of heads HW support.
 */

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_STATIC_INFO (0x20800a01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS {
    NvU32  feHwSysCap;
    NvU32  windowPresentMask;
    NvBool bFbRemapperEnabled;
    NvU32  numHeads;
    NvU32  i2cPort;
    NvU32  internalDispActiveMask;
    NvU32  embeddedDisplayPortMask;
    NvBool bExternalMuxSupported;
    NvBool bInternalMuxSupported;
    NvU32  numDispChannels;
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

    /*! Indicate whether fpba is present or not */
    NvBool bFbpaPresent;

    /*! Size covered by one comptag */
    NvU32  comprPageSize;

    /*! log32(comprPageSize) */
    NvU32  comprPageShift;

    /*! RAM type */
    NvU32  ramType;

    /*! LTC count */
    NvU32  ltcCount;

    /*! LTS per LTC count */
    NvU32  ltsPerLtcCount;
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
 * accessCounterIndex
 *   Index of access counter buffer to register.
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
    NvU32 accessCounterIndex;
    NvU32 bufferSize;
    NV_DECLARE_ALIGNED(NvU64 bufferPteArray[NV2080_CTRL_INTERNAL_UVM_ACCESS_CNTR_BUFFER_MAX_PAGES], 8);
} NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER
 *
 * This command requests physical RM to disable the access counter buffer.
 *
 * accessCounterIndex
 *   Index of access counter buffer to unregister.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER (0x20800a1e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS_MESSAGE_ID (0x1EU)

typedef struct NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS {
    NvU32 accessCounterIndex;
} NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS;

#define NV2080_CTRL_INTERNAL_GR_MAX_ENGINES         8

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_CAPS_V2
 */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_CAPS {
    NvU8 capsTbl[NV0080_CTRL_GR_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_STATIC_GR_CAPS;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_CAPS engineCaps[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CAPS (0x20800a1f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CAPS_PARAMS_MESSAGE_ID (0x1FU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CAPS_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER
 * @ref NV2080_CTRL_CMD_GR_GET_SM_TO_GPC_TPC_MAPPINGS
 */



#define NV2080_CTRL_INTERNAL_GR_MAX_SM                         240

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER {
    struct {
        NvU16 gpcId;
        NvU16 localTpcId;
        NvU16 localSmId;
        NvU16 globalTpcId;
        NvU16 virtualGpcId;
        NvU16 migratableTpcId;
        NvU16 ugpuId;
    } globalSmId[NV2080_CTRL_INTERNAL_GR_MAX_SM];

    NvU16 numSm;
    NvU16 numTpc;
} NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER globalSmOrder[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER (0x20800a22) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER_PARAMS_MESSAGE_ID (0x22U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_GLOBAL_SM_ORDER_PARAMS;

/*!
 * BSP Static data.
 */

#define NV2080_CTRL_CMD_INTERNAL_MAX_BSPS 8

typedef struct NV2080_CTRL_INTERNAL_BSP_CAPS {
    NvU8 capsTbl[NV0080_CTRL_BSP_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_BSP_CAPS;

typedef struct NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_BSP_CAPS caps[NV2080_CTRL_CMD_INTERNAL_MAX_BSPS];
    NvBool                        valid[NV2080_CTRL_CMD_INTERNAL_MAX_BSPS];
} NV2080_CTRL_INTERNAL_BSP_GET_CAPS_PARAMS;

/*!
 * MSENC Static data.
 */

#define NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS 8

typedef struct NV2080_CTRL_INTERNAL_MSENC_CAPS {
    NvU8 capsTbl[NV0080_CTRL_MSENC_CAPS_TBL_SIZE];
} NV2080_CTRL_INTERNAL_MSENC_CAPS;

typedef struct NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS {
    NV2080_CTRL_INTERNAL_MSENC_CAPS caps[NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS];
    NvBool                          valid[NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS];
} NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS;


#define NV2080_CTRL_INTERNAL_GR_MAX_GPC                            12
#define NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT                 10

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_GPC_MASK
 * @ref NV2080_CTRL_CMD_GR_GET_TPC_MASK
 * @ref NV2080_CTRL_CMD_GR_GET_PHYS_GPC_MASK
 */



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

    NvU32 physGfxGpcMask;
    NvU32 numGfxTpc;
} NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS_MESSAGE_ID (0x27U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS {
    /*!
     * floorsweeping masks which are indexed via local GR index
     */
    NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS floorsweepingMasks[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS (0x20800a26) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS_PARAMS_MESSAGE_ID (0x26U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FLOORSWEEPING_MASKS_PARAMS;

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

/*!
 * @ref NV0080_CTRL_CMD_GR_GET_INFO
 * @ref NV0080_CTRL_CMD_GR_GET_INFO_V2
 * @ref NV2080_CTRL_CMD_GR_GET_INFO
 * @ref NV2080_CTRL_CMD_GR_GET_INFO_V2
 */



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
#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO engineInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_INFO (0x20800a2a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_INFO_PARAMS_MESSAGE_ID (0x2AU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ZCULL_INFO
 */



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

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS_MESSAGE_ID (0x2DU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_ZCULL_INFO engineZcullInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ZCULL_INFO (0x20800a2c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ZCULL_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ZCULL_INFO_PARAMS_MESSAGE_ID (0x2CU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ZCULL_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ROP_INFO
 */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO {
    NvU32 ropUnitCount;
    NvU32 ropOperationsFactor;
    NvU32 ropOperationsCount;
} NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS_MESSAGE_ID (0x2FU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO engineRopInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_ROP_INFO (0x20800a2e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ROP_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ROP_INFO_PARAMS_MESSAGE_ID (0x2EU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_ROP_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_PPC_MASK
 */



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS {
    NvU32 mask[NV2080_CTRL_INTERNAL_GR_MAX_GPC];
} NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS_MESSAGE_ID (0x31U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS enginePpcMasks[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PPC_MASKS (0x20800a30) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PPC_MASKS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PPC_MASKS_PARAMS_MESSAGE_ID (0x30U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PPC_MASKS_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_ENGINE_CONTEXT_PROPERTIES
 * @ref NV2080_CTRL_CMD_GR_GET_ATTRIBUTE_BUFFER_SIZE
 */



#define NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT 0x1a

typedef struct NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO {
    NvU32 size;
    NvU32 alignment;
} NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO {
    NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_BUFFER_INFO engine[NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT];
} NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS_MESSAGE_ID (0x33U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO engineContextBuffersInfo[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO (0x20800a32) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO_PARAMS_MESSAGE_ID (0x32U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_CONTEXT_BUFFERS_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER
 */



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

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_MESSAGE_ID (0x35U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER smIssueRateModifier[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER (0x20800a34) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_MESSAGE_ID (0x34U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS;

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



typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES {
    NvU32 fecsRecordSize;
    NvU32 timestampHiTagMask;
    NvU8  timestampHiTagShift;
    NV_DECLARE_ALIGNED(NvU64 timestampVMask, 8);
    NvU8  numLowerBitsZeroShift;
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS_MESSAGE_ID (0x3EU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES fecsTraceDefines[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES], 8);
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES (0x20800a3f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES_PARAMS_MESSAGE_ID (0x3FU)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES_PARAMS;

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
    NvU32 groupId;
    NvU32 ginTargetId;
    NvU32 deviceBroadcastPriBase;
    NvU32 groupLocalInstanceId;
} NV2080_CTRL_INTERNAL_DEVICE_INFO;
#define NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES 512

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

/**
 * Get GR PDB properties synchronized between Kernel and Physical
 *
 * bPerSubCtxheaderSupported
 *   @ref PDB_PROP_GR_SUPPORTS_PER_SUBCONTEXT_CONTEXT_HEADER
 */



typedef struct NV2080_CTRL_INTERNAL_NV_RANGE {
    NV_DECLARE_ALIGNED(NvU64 lo, 8);
    NV_DECLARE_ALIGNED(NvU64 hi, 8);
} NV2080_CTRL_INTERNAL_NV_RANGE;

/*!
 * NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS
 *
 * This structure specifies a target swizz-id and mem_range to update
 *
 *  swizzId[IN]
 *      - Targeted swizz-id for which the memRange is being set
 *
 *  memAddrRange[IN]
 *      - Memory Range for given GPU instance
 */
#define NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS {
    NvU32 swizzId;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NV_RANGE memAddrRange, 8);
} NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE (0x20800a44) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS_MESSAGE_ID (0x44U)

typedef NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE (0x20800a43) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS_MESSAGE_ID" */



#define NV2080_CTRL_INTERNAL_GR_INIT_BUG4208224_WAR_PARAMS_MESSAGE_ID (0x45U)

typedef struct NV2080_CTRL_INTERNAL_GR_INIT_BUG4208224_WAR_PARAMS {
    NvBool bTeardown;
} NV2080_CTRL_INTERNAL_GR_INIT_BUG4208224_WAR_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_KGR_INIT_BUG4208224_WAR (0x20800a46) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS_MESSAGE_ID (0x46U)

typedef NV2080_CTRL_INTERNAL_GR_INIT_BUG4208224_WAR_PARAMS NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS;

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES {
    NvBool bPerSubCtxheaderSupported;
} NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS_MESSAGE_ID (0x47U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES pdbTable[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES (0x20800a48) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES_PARAMS_MESSAGE_ID (0x48U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_PDB_PROPERTIES_PARAMS;

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
 *     # GR engines, including the GFX capable ones.
 *
 *   gfxGrCount [OUT]
 *     # GR engines capable of Gfx, which is a subset of the GR engines included in grCount
 *
 *   gpcCount [OUT]
 *     # total gpcs, including the GFX capable ones.
 *
 *   virtualGpcCount [OUT]
 *     # virtualized gpcs, including the GFX capable ones.
 *
 *   gfxGpcCount [OUT]
 *     # total gpcs capable of Gfx. This is a subset of the GPCs included in gpcCount.
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
 *
 *   validCTSIdMask [OUT]
 *     # mask of CTS IDs which can be assigned under this profile
 * 
 *   validGfxCTSIdMask [OUT]
 *     # mask of CTS IDs that contain Gfx capable Grs which can be assigned under this profile
 */
#define NV2080_CTRL_INTERNAL_GRMGR_PARTITION_MAX_TYPES      90



typedef struct NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO {
    NvU32 partitionFlag;
    NvU32 grCount;
    NvU32 gfxGrCount;
    NvU32 gpcCount;
    NvU32 virtualGpcCount;
    NvU32 gfxGpcCount;
    NvU32 veidCount;
    NvU32 smCount;
    NvU32 ceCount;
    NvU32 nvEncCount;
    NvU32 nvDecCount;
    NvU32 nvJpgCount;
    NvU32 nvOfaCount;
    NV_DECLARE_ALIGNED(NvU64 validCTSIdMask, 8);
    NV_DECLARE_ALIGNED(NvU64 validGfxCTSIdMask, 8);
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
#define NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS_MESSAGE_ID (0x4FU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS {
    NvU32 count;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_MIGMGR_PROFILE_INFO table[NV2080_CTRL_INTERNAL_GRMGR_PARTITION_MAX_TYPES], 8);
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



#define NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_MAX_ENGINES_MASK_SIZE 4

#define NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS_MESSAGE_ID (0x52U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 engineMask[NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_MAX_ENGINES_MASK_SIZE], 8);
} NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS;

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

#define NV2080_CTRL_CMD_INTERNAL_GET_EGPU_BRIDGE_INFO (0x20800a55) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS_MESSAGE_ID (0x55U)

typedef struct NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS {
    NvU16  pciDeviceId;
    NvU16  pciSubDeviceId;
    NvBool iseGPUBridge;
    NvU8   approvedBusType;
} NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS;

#define NV2080_CTRL_INTERNAL_EGPU_BUS_TYPE_NONE              (0x00000000)
#define NV2080_CTRL_INTERNAL_EGPU_BUS_TYPE_CUSTOM            (0x00000001)
#define NV2080_CTRL_INTERNAL_EGPU_BUS_TYPE_TB2               (0x00000002)
#define NV2080_CTRL_INTERNAL_EGPU_BUS_TYPE_TB3               (0x00000003)


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
 *   pbTargetAperture [IN]
 *     Indicates the PushBuffer Target Aperture type (IOVA, PCI, PCI_COHERENT or NVM)
 *
 *   channelPBSize [IN]
 *     Indicates the PushBuffer size requested by client
 *
 *   subDeviceId [IN]
 *     One-hot encoded subDeviceId (i.e. SDM) that will be used to address the channel 
 *     in the pushbuffer stream (via SSDM method)
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
    NvU32  pbTargetAperture;
    NvU32  channelPBSize;
    NvU32  subDeviceId;
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
    NvU32 replayableShadowFaultBufferMetadataSize;
    NvU32 nonReplayableFaultBufferSize;
    NvU32 nonReplayableShadowFaultBufferMetadataSize;
} NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GR_GET_CTXSW_MODES
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_GET_CTXSW_MODES (0x20800a5a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_GET_CTXSW_MODES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GR_GET_CTXSW_MODES_PARAMS_MESSAGE_ID (0x5AU)

typedef NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS NV2080_CTRL_INTERNAL_GR_GET_CTXSW_MODES_PARAMS;

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
 *   tableLen   [OUT]
 *     Number of valid records in table field.
 *
 *   table      [OUT]
 *     Interrupt table for Kernel RM.
 *
 *   subtreeMap [OUT]
 *     Subtree range for each NV2080_INTR_CATEGORY.
 */
#define NV2080_CTRL_CMD_INTERNAL_INTR_GET_KERNEL_TABLE (0x20800a5c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_INTR_MAX_TABLE_SIZE       128

/*!
 * Categories of interrupts.
 *
 * Each of these categories get a separate range of interrupt subtrees (top
 * level bits).
 */
typedef enum NV2080_INTR_CATEGORY {
    NV2080_INTR_CATEGORY_DEFAULT = 0,
    NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE = 1,
    NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE_NOTIFICATION = 2,
    NV2080_INTR_CATEGORY_RUNLIST = 3,
    NV2080_INTR_CATEGORY_RUNLIST_NOTIFICATION = 4,
    NV2080_INTR_CATEGORY_UVM_OWNED = 5,
    NV2080_INTR_CATEGORY_UVM_SHARED = 6,
    NV2080_INTR_CATEGORY_ENUM_COUNT = 7,
} NV2080_INTR_CATEGORY;

#define NV2080_INTR_INVALID_SUBTREE NV_U8_MAX

typedef struct NV2080_INTR_CATEGORY_SUBTREE_MAP {
    NvU8 subtreeStart;
    NvU8 subtreeEnd;
} NV2080_INTR_CATEGORY_SUBTREE_MAP;

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
    NV2080_INTR_CATEGORY_SUBTREE_MAP                 subtreeMap[NV2080_INTR_CATEGORY_ENUM_COUNT];
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
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PROFILES (0x20800a63) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PROFILES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PROFILES_PARAMS_MESSAGE_ID (0x63U)

typedef NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PROFILES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES
 * NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES (0x20800a65) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS_MESSAGE_ID (0x65U)

typedef NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES
 * NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES
 */
#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES (0x20800a66) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS_MESSAGE_ID (0x66U)

typedef NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG
 * NV2080_CTRL_CMD_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG
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
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG (0x20800a68) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS_MESSAGE_ID (0x68U)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 memBoundaryCfgA, 8);
    NV_DECLARE_ALIGNED(NvU64 memBoundaryCfgB, 8);
    NvU32 memBoundaryCfgC;
    NvU32 memBoundaryCfg;
    NvU32 memBoundaryCfgValInit;
} NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG (0x20800a67) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS_MESSAGE_ID (0x67U)

typedef NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS NV2080_CTRL_INTERNAL_KMEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS;

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
#define NV2080_CTRL_CMD_INTERNAL_RC_WATCHDOG_TIMEOUT      (0x20800a6a) /* finn: Evaluated from "((FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x6a)" */

/* !
 *  This command is used to get the current AUX power state of the sub-device.
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_GET_AUX_POWER_STATE (0x20800a81) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS {
    NvU32 powerState;
} NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS;

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
#define NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_2X (0x20800a9a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X_MESSAGE_ID" */

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

// Valid fault buffer types
#define NV2080_CTRL_FAULT_BUFFER_NON_REPLAYABLE                           (0x00000000)
#define NV2080_CTRL_FAULT_BUFFER_REPLAYABLE                               (0x00000001)

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
 * shadowFaultBufferType
 *   Replayable or non-replayable fault buffer
 *
 * faultBufferSharedMemoryPhysAddr
 *   Fault buffer shared memory address. Used only by the Replayable fault buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER (0x20800a9d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_CLIENT_SHADOW_FAULT_BUFFER_MAX_PAGES    3000
#define NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID (0x9DU)

typedef struct NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 shadowFaultBufferQueuePhysAddr, 8);
    NvU32 shadowFaultBufferSize;
    NvU32 shadowFaultBufferMetadataSize;
    NV_DECLARE_ALIGNED(NvU64 shadowFaultBufferPteArray[NV2080_CTRL_INTERNAL_GMMU_CLIENT_SHADOW_FAULT_BUFFER_MAX_PAGES], 8);
    NvU32 shadowFaultBufferType;
    NV_DECLARE_ALIGNED(NvU64 faultBufferSharedMemoryPhysAddr, 8);
} NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER
 *
 * This command requests physical RM to disable the client shadow fault buffer.
 *
 * shadowFaultBufferType
 *   Replayable or non-replayable fault buffer
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER (0x20800a9e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS_MESSAGE_ID (0x9EU)

typedef struct NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS {
    NvU32 shadowFaultBufferType;
} NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER
 *
 * Pin PDEs for  Global VA range on the server RM and then mirror the client's page
 * directory/tables in the server.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER (0x20800a9f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS_MESSAGE_ID (0x9FU)

typedef struct NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS {
    NV_DECLARE_ALIGNED(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS PdeCopyParams, 8);
} NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS;

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
 *   bOverrideInfinite[IN]
 *      This parameter specifies if we want to override already registered infinite boost for the specific Kernel RM.
 *      This should be NV_TRUE only in case when we are removing the current infinite boost for a specific Kernel RM
 *      and setting the boost duration to a next maximum duration registered for the Kernel RM in question.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_3X (0x20800aa0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X_MESSAGE_ID (0xA0U)

typedef struct NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X {
    NvU32  flags;
    NvU32  boostDuration;
    NvU32  gfId;
    NvBool bOverrideInfinite;
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
#define NV2080_CTRL_CMD_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO          (0x20800aa2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_GRMGR_GET_SKYLINE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_SKYLINES            8
#define NV2080_CTRL_INTERNAL_GRMGR_SKYLINE_INFO_MAX_NON_SINGLETON_VGPCS 12
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
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_PARTITIONING_MODE (0x20800aa3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_SET_PARTITIONING_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MIGMGR_SET_PARTITIONING_MODE_PARAMS_MESSAGE_ID (0xA3U)

typedef NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_SET_PARTITIONING_MODE_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GPU_CONFIGURE_PARTITION
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_CONFIGURE_GPU_INSTANCE (0x20800aa4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_CONFIGURE_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MIGMGR_CONFIGURE_GPU_INSTANCE_PARAMS_MESSAGE_ID (0xA4U)

typedef NV2080_CTRL_GPU_CONFIGURE_PARTITION_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_CONFIGURE_GPU_INSTANCE_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GPU_SET_PARTITIONS
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_SET_GPU_INSTANCES (0x20800aa5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_SET_GPU_INSTANCES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MIGMGR_SET_GPU_INSTANCES_PARAMS_MESSAGE_ID (0xA5U)

typedef NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_SET_GPU_INSTANCES_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_GPU_GET_PARTITIONS
 */
#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_GET_GPU_INSTANCES (0x20800aa6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_GET_GPU_INSTANCES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MIGMGR_GET_GPU_INSTANCES_PARAMS_MESSAGE_ID (0xA6U)

typedef NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_GET_GPU_INSTANCES_PARAMS;

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
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED (0x20800a69) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_MESSAGE_ID" */

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
#define NV2080_CTRL_CMD_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE                     (0x20800aa7) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE                      (0x20800aa8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE
 *
 * Create a gpu instance resembling the exported instance info. Note that
 * different GPUs may have different physical resources due to floorsweeping,
 * and an imported gpu instance is not guaranteed to get the exact same resources
 * as the exported gpu instance, but the imported gpu instance should behave
 * identically with respect to fragmentation and placement / span positioning.
 */
#define NV2080_CTRL_CMD_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE                     (0x20800aa9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE                      (0x20800aaa) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_MAX_ENGINES_MASK_SIZE 4
typedef struct NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO {
    NV_DECLARE_ALIGNED(NvU64 enginesMask[NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_MAX_ENGINES_MASK_SIZE], 8);
    NvU32 partitionFlags;
    NvU32 gpcMask;
    NvU32 virtualGpcCount;
    NvU32 veidOffset;
    NvU32 veidCount;
} NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO;

typedef struct NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS {
    NvU32 swizzId;
    NvU8  uuid[NV_GI_UUID_LEN];
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO info, 8);
} NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS;

#define NV2080_CTRL_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID (0xA7U)

typedef NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS NV2080_CTRL_INTERNAL_KMIGMGR_EXPORT_GPU_INSTANCE_PARAMS;

#define NV2080_CTRL_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID (0xA8U)

typedef NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_EXPORT_GPU_INSTANCE_PARAMS;

#define NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID (0xA9U)

typedef NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_GPU_INSTANCE_PARAMS;

#define NV2080_CTRL_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE_PARAMS_MESSAGE_ID (0xAAU)

typedef NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS NV2080_CTRL_INTERNAL_MIGMGR_IMPORT_GPU_INSTANCE_PARAMS;

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
    NvBool bIsDeviceMultiFunction;
    NvBool bGcxPmuCfgSpaceRestore;
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

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_EGM_CONFIG
 *
 * Program HSHUB for EGM peer id.
 *
 *    egmPeerId[IN]
 *      EGM peer id to program in the HSHUB registers.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_EGM_CONFIG (0x20800a8d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS_MESSAGE_ID (0x8dU)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS {
    NvU32 egmPeerId;
} NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS;

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

/*
 * NV2080_CTRL_CMD_INTERNAL_CCU_GET_SAMPLE_INFO
 *
 * This command gets the CCU samples Info from physical-RM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_CCU_GET_SAMPLE_INFO (0x20800ab2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xB2" */

typedef struct NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS {
    NvU32 ccuSampleSize;
} NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_CCU_MAP
 *
 * This command gets the shared buffer memory descriptor from the CPU-RM and maps to it
 * in physical-RM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_ADDRESS
 */
#define NV2080_CTRL_CMD_INTERNAL_CCU_MAP              (0x20800ab3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CCU_DEV_SHRBUF_COUNT_MAX 1

typedef struct NV2080_CTRL_INTERNAL_CCU_MAP_INFO {
    NV_DECLARE_ALIGNED(NvU64 phyAddr, 8);
    NvU32 shrBufSize;
    NvU32 cntBlkSize;
} NV2080_CTRL_INTERNAL_CCU_MAP_INFO;

#define NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS_MESSAGE_ID (0xB3U)

typedef struct NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_CCU_MAP_INFO mapInfo[NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_SIZE + NV2080_CTRL_INTERNAL_CCU_DEV_SHRBUF_COUNT_MAX], 8);
} NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP
 *
 * This command unmaps the shared buffer memory mapping in physical-RM
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP (0x20800ab4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS_MESSAGE_ID (0xB4U)

typedef struct NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS {
    NvBool bDevShrBuf;
    NvBool bMigShrBuf;
} NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS;

/*!
 * NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PEER_INFO
 *
 * [in] gpuId
 *   GPU ID.
 * [in] gpuInstance
 *   GPU instance.
 * [in] p2pCaps
 *   Peer to peer capabilities discovered between the GPUs.
 *   See NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 for the list of valid values.
 * [in] p2pOptimalReadCEs
 *   Mask of CEs to use for p2p reads over Nvlink.
 * [in] p2pOptimalWriteCEs
 *   Mask of CEs to use for p2p writes over Nvlink.
 * [in] p2pCapsStatus
 *   Status of all supported p2p capabilities.
 *   See NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 for the list of valid values.
 * [in] busPeerId
 *   Bus peer ID. For an invalid or a non-existent peer this field
 *   has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 * [in] busEgmPeerId
 *   Bus EGM peer ID. For an invalid or a non-existent peer this field
 *   has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 */
typedef struct NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PEER_INFO {
    NvU32 gpuId;
    NvU32 gpuInstance;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NvU32 busPeerId;
    NvU32 busEgmPeerId;
} NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PEER_INFO;

/*!
 * NV2080_CTRL_CMD_INTERNAL_SET_P2P_CAPS
 *
 * An internal call to propagate the peer to peer capabilities of peer GPUs
 * to the Physical RM. These capabilities are to be consumed by the vGPU GSP plugin.
 * This control is used to both add and and update the peer to peer capabilities.
 * The existing GPU entries will be updated and those which don't exist will be added.
 * Use NV2080_CTRL_CMD_INTERNAL_REMOVE_P2P_CAPS to remove the added entries.
 *
 *   [in] peerGpuCount
 *     The number of the peerGpuInfos entries.
 *   [in] peerGpuInfos
 *     The array of NV2080_CTRL_CMD_INTERNAL_SET_P2P_CAPS entries, describing
 *     the peer to peer capabilities of the specified GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT - Invalid peerGpuCount
 *   NV_ERR_INSUFFICIENT_RESOURCES - Total GPU count exceeds the maximum value
 */
#define NV2080_CTRL_CMD_INTERNAL_SET_P2P_CAPS (0x20800ab5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS_MESSAGE_ID (0xB5U)

typedef struct NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS {
    NvU32                                       peerGpuCount;
    NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PEER_INFO peerGpuInfos[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
} NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_REMOVE_P2P_CAPS
 *
 * An internal call to remove the cached peer to peer capabilities of peer GPUs
 * from the Physical RM.
 *
 *   [in] peerGpuIdCount
 *     The number of the peerGpuIds entries.
 *   [in] peerGpuIds
 *     The array of GPU IDs, specifying the GPU for which the entries need to be removed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT - Invalid peerGpuIdCount
 *   NV_ERR_OBJECT_NOT_FOUND - Invalid peerGpuIds[] entry
 */
#define NV2080_CTRL_CMD_INTERNAL_REMOVE_P2P_CAPS (0x20800ab6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS_MESSAGE_ID (0xB6U)

typedef struct NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS {
    NvU32 peerGpuIdCount;
    NvU32 peerGpuIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
} NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS;



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

#define NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS_MESSAGE_ID (0xB9U)

typedef struct NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS {
    // Enable/disable PCIe relaxed ordering
    NvBool enableRo;
} NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE
 *
 * An internal call to invoke the sequence VGA register reads & writes to
 * perform save and restore of VGA
 *
 *   [in] bSave
 *     To indicate whether save or restore needs to be performed.
 *   [in] bUseVbios
 *     Primary VGA indication from OS.
 *   [out] bReturnEarly
 *     To indicate caller to return after this call.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OPERATION
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE (0x20800a76) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS_MESSAGE_ID (0x76U)

typedef struct NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS {
    NvBool bSave;
    NvBool bUseVbios;
    NvBool bReturnEarly;
} NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE
 *
 * To perform save or restore operation from/to saved fonts.
 *
 *   [in] bSave
 *     To indicate whether save or restore needs to be performed.
 *   [in] bUseVbios
 *     Primary VGA indication from OS.
 *   [in] bVbiosCallSuccessful
 *     Indicates if vbios invocation was successful or not. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE (0x20800a77) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS_MESSAGE_ID (0x77U)

typedef struct NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS {
    NvBool bSave;
    NvBool bUseVbios;
    NvBool bVbiosCallSuccessful;
} NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS;

/*!
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES
 * @ref NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES
 */

/*!
 * NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE
 *
 * This structure specifies resources in an execution partition
 *
 *  computeSize[OUT]
 *      - NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_* associated with this profile
 *
 *  gfxGpcCount[OUT]
 *      - Total Number of GFX Supporting GPCs in this partition
 *
 *  gpcCount[OUT]
 *      - Total Number of GPCs in this partition (including GFX Supported GPCs)
 *
 *  veidCount[OUT]
 *      - Number of VEIDs allocated to this profile
 *
 *  smCount[OUT]
 *      - Number of SMs usable in this profile
 */
typedef struct NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE {
    NvU8  computeSize;
    NvU32 gfxGpcCount;
    NvU32 gpcCount;
    NvU32 veidCount;
    NvU32 smCount;
    NvU32 physicalSlots;
} NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE;

/*!
 * NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS
 *
 * This structure specifies resources in an execution partition
 *
 *  profileCount[OUT]
 *      - Total Number of profiles filled
 *
 *  profiles[OUT]
 *      - NV2080_CTRL_GPU_COMPUTE_PROFILE filled with valid compute instance profiles
 */
#define NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS_MESSAGE_ID (0xBBU)

typedef struct NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS {
    NvU32                                       profileCount;
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE profiles[NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE];
} NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES (0x20800aba) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES_PARAMS_MESSAGE_ID (0xBAU)

typedef NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS NV2080_CTRL_INTERNAL_STATIC_KMIGMGR_GET_COMPUTE_PROFILES_PARAMS;



/*
 * NV2080_CTRL_CMD_INTERNAL_CCU_SET_STREAM_STATE
 *
 * This command sets the ccu stream to enable/disable state.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_CCU_SET_STREAM_STATE (0x20800abd) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS_MESSAGE_ID (0xBDU)

typedef struct NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS {
    NvBool bStreamState;
} NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_ATTACH_AND_INIT
 *
 * Attach GPU to the external device.
 *
 *   [in] bExtDevFound
 *     To enable GPIO interrupts.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_ATTACH_AND_INIT (0x20800abe) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS_MESSAGE_ID (0xBEU)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS {
    NvBool bExtDevFound;
} NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS
 *
 * Optimize the Gsync timing parameters
 *
 *   [in] timingParameters
 *     Timing parameters passed by client.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS (0x20800abf) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS_MESSAGE_ID (0xBFU)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS {
    NV_DECLARE_ALIGNED(NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS timingParameters, 8);
} NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_DISPLAY_IDS
 *
 * Get displayIDs supported by the display.
 *
 *   [out] displayIds
 *     Associated display ID with head.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_DISPLAY_IDS (0x20800ac0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS_MESSAGE_ID" */

#define NV2080_MAX_NUM_HEADS                           4

#define NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS_MESSAGE_ID (0xC0U)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS {
    NvU32 displayIds[NV2080_MAX_NUM_HEADS];
} NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_STREO_SYNC
 *
 * Set the Stereo sync for Gsync
 *
 *   [in] slave
 *     Slave GPU head status.
 *   [in] localSlave
 *     Slave GPU head status but are not coupled.
 *   [in] master
 *     Master GPU head status.
 *   [in] regstatus
 *     Register status of status1 register.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_STREO_SYNC (0x20800ac1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS_MESSAGE_ID (0xC1U)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS {
    NvU32 slave[NV2080_MAX_NUM_HEADS];
    NvU32 localSlave[NV2080_MAX_NUM_HEADS];
    NvU32 master[NV2080_MAX_NUM_HEADS];
    NvU32 regStatus;
} NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES
 *
 * Get vertical active lines for given head.
 *
 *   [in] head
 *     For the headIdx which we need active.
 *   [out] vActiveLines
 *     Vertical active lines.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES (0x20800ac4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS_MESSAGE_ID (0xC4U)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS {
    NvU32 headIdx;
    NvU32 vActiveLines;
} NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_IS_DISPLAYID_VALID
 *
 * Verifies if this displayId is valid.
 *
 *   [in] displays
 *     Displays given by the client
 *
 *   [out] displayId
 *     DisplayId for the given display
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_IS_DISPLAYID_VALID (0x20800ac9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS_MESSAGE_ID (0xC9U)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS {
    NvU32 displays;
    NvU32 displayId;
} NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC
 *
 * Disable the raster sync gpio on the other P2060 GPU
 * that's connected to master over Video bridge.
 *
 *   [in] bEnableMaster
 *     If it is master gpu.
 *
 *   [out] bRasterSyncGpioSaved
 *     If raster sync GPIO direction is saved or not.
 *
 *   [in/out] bRasterSyncGpioDirection
 *     During save it gets the direction.
 *     In restores it sets the direction.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC (0x20800aca) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS_MESSAGE_ID (0xCAU)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS {
    NvBool bEnableMaster;
    NvBool bRasterSyncGpioSaved;
    NvU32  bRasterSyncGpioDirection;
} NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS;



/*!
 * NV2080_CTRL_CMD_INTERNAL_FBSR_INIT
 *
 * Initialize FBSR on GSP to prepare for suspend-resume
 *
 *   [in] hClient
 *     Handle to client of SYSMEM memlist object
 *   [in] hSysMem
 *     Handle to SYSMEM memlist object
 *   [in] bEnteringGcoffState
 *     Value of PDB_PROP_GPU_GCOFF_STATE_ENTERING
 *   [in] sysmemAddrOfSuspendResumeData
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_FBSR_INIT (0x20800ac2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS_MESSAGE_ID (0xC2U)

typedef struct NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS {
    NvHandle hClient;
    NvHandle hSysMem;
    NvBool   bEnteringGcoffState;
    NV_DECLARE_ALIGNED(NvU64 sysmemAddrOfSuspendResumeData, 8);
} NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING
 *
 * Disable all the active channels during suspend
 * Resume FIFO scheduling from GSP after resume on Kernel-RM
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING (0x20800ac3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS_MESSAGE_ID (0xC3U)

typedef struct NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS {
    NvBool bDisableActiveChannels;
} NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB
 *
 * This command is used to get the amount of host reserved FB
 *
 *  hostReservedFb [OUT]
 *      Amount of FB reserved for the host
 *  vgpuTypeId [IN]
 *      The Type ID for VGPU profile
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB (0x20800ac5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS_MESSAGE_ID (0xC5U)

typedef struct NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 hostReservedFb, 8);
    NvU32 vgpuTypeId;
} NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_INIT_BRIGHTC_STATE_LOAD
 *
 * This command initiates brightc module state load.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_INIT_BRIGHTC_STATE_LOAD (0x20800ac6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ACPI_DSM_READ_SIZE                   (0x1000) /* finn: Evaluated from "(4 * 1024)" */

#define NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS_MESSAGE_ID (0xC6U)

typedef struct NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS {
    NvU32 status;
    NvU16 backLightDataSize;
    NvU8  backLightData[NV2080_CTRL_ACPI_DSM_READ_SIZE];
} NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS;

/*
 * NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL
 *
 * Returns number of active links allowed per IOCTRL
 *
 * [Out] numActiveLinksPerIoctrl
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS_MESSAGE_ID (0xC7U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS {
    NvU32 numActiveLinksPerIoctrl;
} NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS;
#define NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL (0x20800ac7U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS_MESSAGE_ID" */
/*
 * NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL
 *
 * Returns number of links per IOCTRL
 *
 * [Out] numLinksPerIoctrl
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS_MESSAGE_ID (0xC8U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS {
    NvU32 numLinksPerIoctrl;
} NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS;
#define NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL (0x20800ac8U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE
 *
 * Update the system control capability
 *
 *   bIsSysCtrlSupported  [IN]
        If the system control is supported
 *   bIsPlatformLegacy [OUT]
        If the platform is legacy
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE  (0x20800acb) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS_MESSAGE_ID (0xCBU)

typedef struct NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS {
    NvBool bIsSysCtrlSupported;
    NvBool bIsPlatformLegacy;
} NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS;

/*!
 * Macros for PFM_REQ_HNDLR_STATE_SYNC data types.
 */
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_TYPE_PMGR   0x00U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_TYPE_THERM  0x01U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_TYPE_SMBPBI 0x02U

/*!
 * Structure representing static data for a PFM_REQ_HNDLR_STATE_SYNC_SMBPBI.
 */
typedef struct NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_SMBPBI {
    /*!
     * PFM sensor ID
     */
    NvU32 sensorId;

    /*!
     * PFM sensor limit value if required
     */
    NvU32 limit;
} NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_SMBPBI;

/*!
 * Structure of static information describing PFM_REQ_HNDLR_STATE_SYNC data types.
 */
typedef struct NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA {
    /*!
     * @ref NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_TYPE_<xyz>
     */
    NvU8 type;

    /*!
     * Type-specific information.
     */
    union {
        NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA_SMBPBI smbpbi;
    } data;
} NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA;

/*!
 * Macros for PFM_REQ_HNDLR_STATE_SYNC flags for specific operation.
 */
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_PMGR_LOAD       0x00U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_THERM_INIT      0x01U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_SMBPBI_OP_CLEAR 0x02U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_SMBPBI_OP_SET   0x03U

/*!
 * Structure of static information describing PFM_REQ_HNDLR_STATE_SYNC params.
 */
typedef struct NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS {
    /*!
     * @ref NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_<xyz>
     */
    NvU8                                               flags;

    /*!
     * Type-specific information.
     */
    NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_DATA syncData;
} NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC
 *
 * State sync with platform req handler and SMBPBI
 *
 *   flags [IN]
 *     Flags that needs sync operation between physical and kernel
 *
 *   syncData [IN]
 *     Sync payload data
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC (0x20800acc) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC_PARAMS_MESSAGE_ID (0xCCU)

typedef NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS NV2080_CTRL_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC
 *
 * State sync with platform req handler and SMBPBI
 *
 *   flags [IN]
 *     Flags that needs sync operation between physical and kernel
 *
 *   syncData [IN]
 *     Sync payload data
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC (0x20800acd) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC_PARAMS_MESSAGE_ID (0xCDU)

typedef NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS NV2080_CTRL_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC_PARAMS;

/*!
 * Macros for NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE flag
 */
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_GET_PM1_FORCED_OFF_STATUS 0x00U
#define NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_GET_PM1_STATUS            0x01U

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE
 *
 * Queries PM1 Forced off / PM1 Available status
 *
 *   flag        [IN]
 *     Fetch PM1 Forced off / PM1 Available status based on value.
 *   bStatus     [OUT]
 *     PM1 Forced off / PM1 Available is true or false.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMETS
 */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE    (0x20800ace) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS_MESSAGE_ID (0xCEU)

typedef struct NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS {
    /*!
     * Fetch PM1 Forced off / PM1 Available status based on value.
     */
    NvU8   flag;

    /*!
     * PM1 Forced off / PM1 Available status
     */
    NvBool bStatus;
} NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE
 *
 * Set PM1 state to enabled / disabled (boost clocks).
 *
 *   bEnable   [IN]
 *     NV_TRUE means enable PM1, NV_FALSE means disable.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE (0x20800acf) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS_MESSAGE_ID (0xCFU)

typedef struct NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS {

    /*!
     * NV_TRUE means enable PM1, NV_FALSE means disable.
     */
    NvBool bEnable;
} NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT
 *
 * Updates EDPpeak Limit of GPU
 *
 *   bEnable        [IN]
 *     Enable or Reset the settings
 *   clientLimit    [IN]
 *     Client requested limit
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_NOT_READY
 */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT (0x20800ad0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS_MESSAGE_ID (0xD0U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS {
    NvBool bEnable;
    NvU32  clientLimit;
} NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT
 *
 * Updates Target Temperature of GPU
 *
 *   targetTemp        [IN]
 *     Target Temperature Set from SBIOS
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT (0x20800ad1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS_MESSAGE_ID (0xD1U)

typedef struct NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS {
    NvS32 targetTemp;
} NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE
 *
 * Enable / disable CTGP MODE
 *
 *   bEnable        [IN]
 *     Enable or Reset the settings
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE (0x20800ad2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS_MESSAGE_ID (0xD2U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS {
    NvBool bEnable;
} NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2
 *
 * Configuration of the turbo v2 parameters for NVPCF-Turbo arb control
 *
 *   ctgpOffsetmW        [IN]
 *     TGP MODE Offset in mW
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2 (0x20800ad3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS_MESSAGE_ID (0xD3U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS {
    NvU32 ctgpOffsetmW;
} NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO
 *
 * Query VPstatese info. Get VPS PS2.0 support / get highest VP State Idx and requested VP State Idx
 *
 *   bVpsPs20Supported     [OUT]
 *     Reflects Vpstates PS20 support
 *   vPstateIdxHighest     [OUT}
 *     Reflects Highest VPstate Idx
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO (0x20800ad4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS_MESSAGE_ID (0xD4U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS {
    /*!
     * Reflects Vpstates PS20 support
     */
    NvBool bVpsPs20Supported;

    /*!
     * Get highest VPState Idx from VBIOS
     */
    NvU32  vPstateIdxHighest;
} NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING
 *
 * Get vPstate mapping for requested pStateIdx
 *
 *   pStateIdx     [IN]
 *     Requested PState Idx
 *   vPstateIdx    [OUT}
 *     Mapped VPstate Idx
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING (0x20800ad5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS_MESSAGE_ID (0xD5U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS {
    /*!
     * Requested PState Idx
     */
    NvU32 pStateIdx;

    /*!
     * Mapped VPstate Idx
     */
    NvU32 vPstateIdxMapping;
} NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE
 *
 * Set requested VPstate
 *
 *   vPstateIdx   [IN]
 *     VPstate Idx to be set
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE (0x20800ad6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS_MESSAGE_ID (0xD6U)

typedef struct NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS {

    /*!
     * NV_TRUE means enable PM1, NV_FALSE means disable.
     */
    NvU32 vPstateIdx;
} NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_GCX_ENTRY_PREREQUISITE
 *
 * This command gets if GPU is in a proper state (P8 and engine idle) to be ready to enter RTD3
 *
 * Possible status return values are:
 *   NV_OK     Success
 */
#define NV2080_CTRL_CMD_INTERNAL_GCX_ENTRY_PREREQUISITE (0x2080a7d7) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_POWER_LEGACY_NON_PRIVILEGED_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS_MESSAGE_ID (0xD7U)

typedef struct NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS {
    NvBool bIsGC6Satisfied;
    NvBool bIsGCOFFSatisfied;
} NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS;

/*
 *  This command unsets Dynamic Boost limit when nvidia-powerd is terminated unexpectedly.
 */
#define NV2080_CTRL_CMD_INTERNAL_PMGR_UNSET_DYNAMIC_BOOST_LIMIT (0x20800a7b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x7B" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to get number of secure channels supported on SEC2 and CE
 *
 *   maxSec2SecureChannels  [OUT]
 *   maxCeSecureChannels    [OUT]
 */
#define NV2080_CTRL_CMD_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS   (0x20800ad8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS_MESSAGE_ID (0xD8U)

typedef struct NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS {
    NvU32 maxSec2SecureChannels;
    NvU32 maxCeSecureChannels;
} NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_DEPENDENCY_CHECK
 *
 * This command checks if all the dependant modules to PRH have been initialized. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_DEPENDENCY_CHECK (0x20800a7a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0x7A" */

/*
 * NV2080_CTRL_CMD_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to disable the GPU system memory access after quiescing the GPU or
 * re-enable sysmem access.
 *
 *   bDisable   [IN]
 *     If NV_TRUE the GPU is quiesced and system memory access is disabled .
 *     If NV_FALSE the GPU system memory access is re-enabled and the GPU is resumed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS    (0x20800adb) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS_MESSAGE_ID (0xDBU)

typedef struct NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS {
    NvBool bDisable;
} NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_DISP_PINSETS_TO_LOCKPINS (0x20800adc) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS_MESSAGE_ID (0xDCU)

typedef struct NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS {
    NvU32  pinSetIn; // in
    NvU32  pinSetOut; // in
    NvBool bMasterScanLock; // out
    NvU32  masterScanLockPin; // out
    NvBool bSlaveScanLock; // out
    NvU32  slaveScanLockPin; // out
} NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_DETECT_HS_VIDEO_BRIDGE            (0x20800add) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xDD" */

#define NV2080_CTRL_CMD_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL (0x20800ade) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS_MESSAGE_ID (0xDEU)

typedef struct NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS {
    NvU32  pinSet; // in
    NvU32  gpioFunction; // out
    NvU32  gpioPin; // out
    NvBool gpioDirection; // out
} NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS;

/* NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA
 *
 * This command sets up ACPI DDC Edid data.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA (0x20800adf) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS_MESSAGE_ID" */

/* From ACPI6.5 spec., the max size of EDID data from SBIOS(_DDC) is 512B */
#define MAX_EDID_SIZE_FROM_SBIOS                      512U

typedef struct NV2080_CTRL_INTERNAL_EDID_DATA {
    NvU32 status;
    NvU32 acpiId;
    NvU32 bufferSize;
    NvU8  edidBuffer[MAX_EDID_SIZE_FROM_SBIOS];
} NV2080_CTRL_INTERNAL_EDID_DATA;

#define NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS_MESSAGE_ID (0xDFU)

typedef struct NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS {
    NvU32                          tableLen;
    NV2080_CTRL_INTERNAL_EDID_DATA edidTable[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];
} NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_DISPLAY_ACPI_SUBSYSTEM_ACTIVATED
 *
 * This command intializes display ACPI child devices.
 * This command accepts no parameters.
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_ACPI_SUBSYSTEM_ACTIVATED (0x20800af0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xF0" */

/* NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_MODESET */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_MODESET              (0x20800af1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xF1" */

/* NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_MODESET */
#define NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_MODESET             (0x20800af2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | 0xF2" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 * This structure provides the params for getting GPU Fabric Probe Internal
 * Info from GSP to CPU RM
 *
 *  numProbes[OUT]
 *      - Number of probe requests sent
 */
#define NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID (0xF4U)

typedef struct NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 numProbes, 8);
} NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_GET_FABRIC_PROBE_INFO
 *
 * This command is used to get NV2080_CTRL_CMD_INTERNAL_GPU_FABRIC_PROBE_INFO_PARAMS
 * from GSP to CPU RM.
 * This command accepts NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_FABRIC_PROBE_INFO (0x208001f4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 * This structure provides the params for starting GPU Fabric Probe
 *
 *  bwMode[IN]
 *      - Nvlink Bandwidth mode
 *
 *  bLocalEgmEnabled[IN]
 *      - EGM Enablement Status that needs to be set in GSP-RM
 */
#define NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID (0xF5U)

typedef struct NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS {
    NvU8   bwMode;
    NvBool bLocalEgmEnabled;
} NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE
 *
 * This command is used to trigger start of GPU FABRIC PROBE PROCESS on GSP.
 * This command accepts NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE   (0x208001f5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_STOP_FABRIC_PROBE
 *
 * This command is used to trigger stop of GPU FABRIC PROBE PROCESS on GSP.
 * This command accepts no parameters
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_STOP_FABRIC_PROBE    (0x208001f6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xF6" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_SUSPEND_FABRIC_PROBE
 *
 * This command is used to trigger suspend of GPU FABRIC PROBE PROCESS on GSP.
 * This command accepts no parameters
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_SUSPEND_FABRIC_PROBE (0x208001f7) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xF7" */


/*!
 * NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 * This structure provides the params for resuming GPU Fabric Probe
 *
 *  bwMode[IN]
 *      - Nvlink Bandwidth mode
 */
#define NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID (0xF8U)

typedef struct NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS {
    NvU8 bwMode;
} NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_RESUME_FABRIC_PROBE
 *
 * This command is used to trigger resume of GPU FABRIC PROBE PROCESS on GSP.
 * This command accepts NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_RESUME_FABRIC_PROBE      (0x208001f8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE
 *
 * This command is used to invalidate/reset GPU_FABRIC_PROBE_INFO on GSP.
 * This command accepts no parameters
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE  (0x208001f9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xF9" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to get static conf compute info
 *
 *      bIsBar1Trusted: [OUT]
 *          Is BAR1 trusted to access CPR
 *      bIsPcieTrusted: [OUT]
 *          Is PCIE trusted to access CPR
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO (0x20800af3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS_MESSAGE_ID (0xF3U)

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS {
    NvBool bIsBar1Trusted;
    NvBool bIsPcieTrusted;
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to derive SWL keys and IV masks for a given engine
 *
 *      engineId: [IN]
 *          NV2080_ENGINE_TYPE_* for engine for which keys and IV mask should be derived
 *      ivMaskSet: [OUT]
 *          Set of IV masks for given engine
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS (0x20800ae1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SIZE         3U
#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_KERNEL   0U
#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_USER     1U
#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_SCRUBBER 2U
#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_COUNT    3U
#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_LCE_COUNT    6U

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK {
    NvU32 ivMask[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SIZE];
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK;

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS_MESSAGE_ID (0xE1U)

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS {
    NvU32                                    engineId;
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_COUNT];
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to derive LCE keys and IV masks for a given engine
 *
 *      engineId: [IN]
 *          NV2080_ENGINE_TYPE_* for engine for which keys and IV mask should be derived
 *      ivMaskSet: [OUT]
 *          Set of IV masks for given engine
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS (0x20800ae2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS_MESSAGE_ID (0xE2U)

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS {
    NvU32                                    engineId;
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_LCE_COUNT];
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_ROTATE_KEY
 *
 * This command handles key rotation for a given H2D key (and corresponding D2H key)
 * by deriving new key on GSP and updating the key on relevant SEC2 or LCE.
 * It also updates IVs for all channels using the key and conditionally re-enables them
 * and notifies clients of key rotation status at the end.
 *
 *      globalH2DKey : [IN]
 *          global h2d key to be rotated
 *      updatedEncryptIVMask: [OUT]
 *          Encrypt IV mask post IV key rotation for a given engine's kernel channel
 *      updatedDecryptIVMask: [OUT]
 *          Decrypt IV mask post IV key rotation for a given engine's kernel channel
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_ROTATE_KEYS (0x20800ae5) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS_MESSAGE_ID (0xE5U)

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS {
    NvU32 globalH2DKey;
    NvU32 updatedEncryptIVMask[CC_AES_256_GCM_IV_SIZE_DWORD];
    NvU32 updatedDecryptIVMask[CC_AES_256_GCM_IV_SIZE_DWORD];
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION
 *
 * This command RCs all channels that use the given key and have not reported 
 * idle via NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION yet.
 * RM needs to RC such channels before going ahead with key rotation.
 *
 *      globalH2DKey : [IN]
 *          global h2d key whose channels will be RCed
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION (0x20800ae6) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS_MESSAGE_ID (0xE6U)

typedef struct NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS {
    NvU32 exceptionType;
    NvU32 globalH2DKey;
} NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE
 *
 * This control call can be used to set gpu state on GSP to accept client requests
 * or to block client requests.
 * This is a internal command sent from Kernel RM to Physical RM.
 *
 *      bAcceptClientRequest:[IN]
 *          NV_TRUE : set gpu state to accept client work requests
 *          NV_FALSE: set gpu state to block client work requests
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE (0x20800ae7) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS_MESSAGE_ID (0xE7U)

typedef struct NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS {
    NvBool bAcceptClientRequest;
} NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY
 *
 *    This control call can be used to set CC security policy on GSP.
 *    This is a internal command sent from Kernel RM to Physical RM.
 *
 *    attackerAdvantage [IN]
 *      The minimum and maximum values for attackerAdvantage.
 *      The probability of an attacker successfully guessing the contents of
 *      an encrypted packet go up ("attacker advantage"). 
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_STATE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY (0x20800ae8) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS_MESSAGE_ID (0xE8U)

typedef struct NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 attackerAdvantage, 8);
} NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS;


/*!
 * NV2080_CTRL_CMD_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to update the logical Uproc Id for the configuration.
 *
 *   logicalUprocId  [OUT]
 */
#define NV2080_CTRL_CMD_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID (0x20800aef) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID_PARAMS_MESSAGE_ID (0xEFU)

typedef struct NV2080_CTRL_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID_PARAMS {
    NvU8 logicalUprocId;
} NV2080_CTRL_INTERNAL_FIFO_GET_LOGICAL_UPROC_ID_PARAMS;



/*
 * NV2080_CTRL_CMD_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP
 *
 * This command is used by CPU-RM to perform memory operations using GSP
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_TIMEOUT_RETRY
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP (0x20800afa) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_MEMMGR_MEMORY_OP {
    NV2080_CTRL_MEMMGR_MEMORY_OP_MEMCPY = 0,
    NV2080_CTRL_MEMMGR_MEMORY_OP_MEMSET = 1,
} NV2080_CTRL_MEMMGR_MEMORY_OP;

typedef struct NV2080_CTRL_INTERNAL_TRANSFER_SURFACE_INFO {
    /*!
     * Base physical address of the surface
     */
    NV_DECLARE_ALIGNED(NvU64 baseAddr, 8);

    /*!
     * Size of the surface in bytes
     */
    NV_DECLARE_ALIGNED(NvU64 size, 8);

    /*!
     * Offset in bytes into the surface where read/write must happen
     */
    NV_DECLARE_ALIGNED(NvU64 offset, 8);

    /*!
     * Aperture where the surface is allocated
     */
    NvU32 aperture;

    /*!
     * CPU caching attribute of the surface
     */
    NvU32 cpuCacheAttrib;
} NV2080_CTRL_INTERNAL_TRANSFER_SURFACE_INFO;

#define CC_AES_256_GCM_AUTH_TAG_SIZE_BYTES (0x10U) /* finn: Evaluated from "(128 / 8)" */

#define NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS_MESSAGE_ID (0xFAU)

typedef struct NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS {

    /*!
     * Source surface info
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_TRANSFER_SURFACE_INFO src, 8);

    /*!
     * Authentication tag if data is encrypted
     */
    NvU8                         authTag[CC_AES_256_GCM_AUTH_TAG_SIZE_BYTES];

    /*!
     * Destination surface info
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_TRANSFER_SURFACE_INFO dst, 8);

    /*!
     * Size of the data to be transferred
     */
    NV_DECLARE_ALIGNED(NvU64 transferSize, 8);

    /*!
     * To be set in case of memset
     */
    NvU32                        value;

    /*!
     * Memory op to be performed
     */
    NV2080_CTRL_MEMMGR_MEMORY_OP memop;
} NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to get local GPU's ATS config
 *
 *      addrSysPhys : [OUT]
 *          System Physical Address
 *      addrWidth   : [OUT]
 *          Address width value
 *      mask        : [OUT]
 *          Mask value
 *      maskWidth   : [OUT]
 *          Mask width value
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG (0x20800afb) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS_MESSAGE_ID (0xFBU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 addrSysPhys, 8);
    NvU32 addrWidth;
    NvU32 mask;
    NvU32 maskWidth;
} NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to set peer ATS config using the parameters passed in.
 *
 *      peerId      : [IN]
 *          Peer Id of the peer for which ATS config is to be programmed
 *      addrSysPhys : [IN]
 *          System Physical Address
 *      addrWidth   : [IN]
 *          Address width value
 *      mask        : [IN]
 *          Mask value
 *      maskWidth   : [IN]
 *          Mask width value
 */
#define NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG (0x20800afc) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS_MESSAGE_ID (0xFCU)

typedef struct NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS {
    NvU32 peerId;
    NV_DECLARE_ALIGNED(NvU64 addrSysPhys, 8);
    NvU32 addrWidth;
    NvU32 mask;
    NvU32 maskWidth;
} NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO
 *
 * Get GPU EDPpeak Limit information
 *
 *   limitMin        [OUT]
 *     Minimum allowed limit value on EDPp policy on both AC and DC
 *   limitRated      [OUT]
 *      Rated/default allowed limit value on EDPp policy on AC
 *   limitMax        [OUT]
 *     Maximum allowed limit value on EDPp policy on AC
 *   limitCurr       [OUT]
 *     Current resultant limit effective on EDPp policy on AC and DC
 *   limitBattRated  [OUT]
 *     Default/rated allowed limit on EDPp policy on DC
 *   limitBattMax    [OUT]
 *     Maximum allowed limit on EDPp policy on DC
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_GENERIC
 */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO (0x20800afd) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS_MESSAGE_ID (0xFDU)

typedef struct NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS {
    NvU32 limitMin;
    NvU32 limitRated;
    NvU32 limitMax;
    NvU32 limitCurr;
    NvU32 limitBattRated;
    NvU32 limitBattMax;
} NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_INTERNAL_INIT_USER_SHARED_DATA
 *
 * @brief Initialize/Destroy RM User Shared Data memory mapping on physical RM
 *
 * @param[in]  bInit    If this is an init or a destroy request
 * @param[in]  physAddr Physical address of memdesc to link physical to kernel
 *                      0 to de-initialize
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS_MESSAGE_ID (0xFEU)

typedef struct NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS {
    NvBool bInit;
    NV_DECLARE_ALIGNED(NvU64 physAddr, 8);
} NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_INIT_USER_SHARED_DATA (0x20800afe) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL
 *
 * @brief Set mask of data to be polled on physical for RUSD
 *
 * @param[in]  polledDataMask Bitmask of data requested, defined in cl00de
 * @param[in]  pollFrequencyMs Requested polling interval, in ms
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS_MESSAGE_ID (0xFFU)

typedef struct NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 polledDataMask, 8);
    NvU32 pollFrequencyMs;
} NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL (0x20800aff) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS
 *
 * This structure provides the params for starting GPU Fabric Probe
 *
 *  tracepointMask[IN]
 *      - tracepoint selection filter
 *  bufferAddr[IN]
 *      - physical address of tracing buffer for VGPU
 *  bufferSize[IN]
 *      - size of gsp side logging buffer
 *  bufferWatermark[IN]
 *      - entry threshold for GSP to issue RPC of logged entries to kernel RM
 *  flag[IN]
 *      - indicates which operation to perform
 */
#define NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS_MESSAGE_ID (0xE3U)

typedef struct NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 tracepointMask, 8);
    NV_DECLARE_ALIGNED(NvU64 bufferAddr, 8);
    NvU32 bufferSize;
    NvU32 bufferWatermark;
    NvU8  flag;
} NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS;

/*!
 * Macros for INTERNAL_CONTROL_GSP_TRACE flags for specific operation.
 */
#define NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_START_KEEP_OLDEST 0x00U
#define NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_START_KEEP_NEWEST 0x01U
#define NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_STOP              0x02U

/*
 * NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE
 *
 * This command is used to start GSP-RM trace tool.
 * This command accepts NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE                        (0x208001e3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES
 *
 * @brief get state (enabled/disabled) of SEC2 classes
 *
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES_PARAMS_MESSAGE_ID (0xAFU)

typedef struct NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES_PARAMS {
    NvBool bMaxwellSec2Enabled;
    NvBool bNv95A1TsecEnabled;
    NvBool bHopperSec2WorkLaunchAEnabled;
} NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES (0x20800aaf) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_CMD_INTERNAL_GET_ENABLED_SEC2_CLASSES_PARAMS_MESSAGE_ID" */


/*!
 * @ref NV2080_CTRL_CMD_INTERNAL_GR_CTXSW_SETUP_BIND
 */
#define NV2080_CTRL_CMD_INTERNAL_GR_CTXSW_SETUP_BIND      (0x20800ae4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GR_CTXSW_SETUP_BIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GR_CTXSW_SETUP_BIND_PARAMS_MESSAGE_ID (0xE4U)

typedef NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS NV2080_CTRL_INTERNAL_GR_CTXSW_SETUP_BIND_PARAMS;

/*!
 * NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER
 *
 * @brief Notify the offloaded RM that CPU-RM enters the power management cycle.
 *
 *      bInPMTransition : [IN]
 *      newPMLevel : [IN]
 *          New PM Level : NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_[0-7]
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS_MESSAGE_ID (0xE9U)

typedef struct NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS {
    NvBool bInPMTransition;
    NvU32  newPMLevel;
} NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS;
#define NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER      (0x20800ae9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_FATAL_RECOVERY
 *
 * This command is used to perform recovery actions after the fabric has been
 * idled due to a fatal nvlink error.
 * This command accepts no parameters.
 *
 *   bSuccessful
 *     NV_TRUE if recovery was successful, NV_FALSE otherwise
 */
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY (0x20800aea) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS_MESSAGE_ID (0xEAU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS {
    NvBool bSuccessful;
} NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS;

/*!
 * NV2080_CTRL_CMD_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP
 *
 * @brief To get the free heap size of GSP-RM
 *
 *      freeHeapSize : [OUT]
 *
 * @return NV_OK
 */
#define NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS_MESSAGE_ID (0xEBU)

typedef struct NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 freeHeapSize, 8);
} NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP (0x20800aeb) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_GPU_SET_ILLUM
 *
 * This command sets a new value for the specified Illumination control attribute.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS_MESSAGE_ID (0xECU)

typedef struct NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS {
    NvU32 attribute;
    NvU32 value;
} NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_GPU_SET_ILLUM (0x20800aecU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS_MESSAGE_ID" */
/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR
 *
 * @brief NVIDIA RTX 5000 (GP180 SKU500) Windows-specific war
 * to pull gpio19 (stereo pin) low for bug3362661.
 *
 *   [in] bApplyStereoPinAlwaysHiWar
 *     If need to driver stereo pin(GPIO19) low(_IO_INPUT)
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS_MESSAGE_ID (0xEDU)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS {
    NvBool bApplyStereoPinAlwaysHiWar;
} NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR (0x20800aed) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS_MESSAGE_ID" */

/*!
 * NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM
 * 
 * Returns the maximum number of HSHUBs in a shim instance.
 * 
 *  maxHshubs[OUT]
 *      The maximum number of HSHUBs in a shim instance.
 *
 *  @return NV_OK
 */
#define NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM        (0x20800a79) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS_MESSAGE_ID (0x79U)

typedef struct NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS {
    NvU32 maxHshubs;
} NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS;


/*!
 * NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_RASTER_MODE
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS {
    NvU32 rasterSyncDecodeMode;
} NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE (0x20800a14) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS
 *
 * This is an internal command sent from kernel-RM to physical-RM to retrieve GPU PF BAR1 SPA
 * BAR1 SPA is required for BAR1 mapping in Direct NIC case for DMA(Direct Memory Access) of FB.
 *
 *  spaValue[OUT]
 *      - BAR1 SPA of GPU PF
 */
#define NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS_MESSAGE_ID (0xEEU)

typedef struct NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 spaValue, 8);
} NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_GPU_GET_PF_BAR1_SPA       (0x20800aee) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_NVLINK_PEER
 *
 * This command is used to enable RM NVLink enabled peer state.
 * Note: This just updates the RM state. To reflect the state in the registers,
 *       use NV2080_CTRL_CMD_NVLINK_SET_NVLINK_PEER
 *
 * [in] peerMask
 *     Mask of Peer IDs for which USE_NVLINK_PEER needs to be enabled
 * [in] bEnable
 *     Whether the bit needs to be set or unset
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the USE_NVLINK_PEER bit was enabled successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip, or
 *     If unsetting USE_NVLINK_PEER bit is not supported
 *
 */
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_NVLINK_PEER (0x20800a21U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS {
    NvU32  peerMask;
    NvBool bEnable;
} NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS;

/*
 * NVLINK Link states
 * These should ALWAYS match the nvlink core library defines in nvlink.h
 */
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_OFF                      0x00U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_HS                       0x01U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SAFE                     0x02U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_FAULT                    0x03U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RECOVERY                 0x04U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_FAIL                     0x05U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DETECT                   0x06U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RESET                    0x07U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ENABLE_PM                0x08U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_PM               0x09U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SLEEP                    0x0AU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_SAVE_STATE               0x0BU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_RESTORE_STATE            0x0CU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_PRE_HS                   0x0EU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_ERR_DETECT       0x0FU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_LANE_DISABLE             0x10U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_LANE_SHUTDOWN            0x11U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_TRAFFIC_SETUP            0x12U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITPHASE1               0x13U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITNEGOTIATE            0x14U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITNEGOTIATE       0x15U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITOPTIMIZE             0x16U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_POST_INITOPTIMIZE        0x17U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_DISABLE_HEARTBEAT        0x18U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_CONTAIN                  0x19U
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITTL                   0x1AU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INITPHASE5               0x1BU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ALI                      0x1CU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_ACTIVE_PENDING           0x1DU
#define NV2080_INTERNAL_NVLINK_CORE_LINK_STATE_INVALID                  0xFFU

/*
 * NVLINK TX Sublink states
 * These should ALWAYS match the nvlink core library defines in nvlink.h
 */
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_HS                          0x00U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_SINGLE_LANE                 0x04U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_LOW_POWER                   0x04U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_TRAIN                       0x05U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_SAFE                        0x06U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_OFF                         0x07U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_COMMON_MODE                 0x08U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_COMMON_MODE_DISABLE         0x09U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_DATA_READY                  0x0AU
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_EQ                          0x0BU
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_PRBS_EN                     0x0CU
#define NV2080_NVLINK_CORE_SUBLINK_STATE_TX_POST_HS                     0x0DU

/*
 * NVLINK RX Sublink states
 * These should ALWAYS match the nvlink core library defines in nvlink.h
 */
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_HS                          0x00U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_SINGLE_LANE                 0x04U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_LOW_POWER                   0x04U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_TRAIN                       0x05U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_SAFE                        0x06U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_OFF                         0x07U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_RXCAL                       0x08U
#define NV2080_NVLINK_CORE_SUBLINK_STATE_RX_INIT_TERM                   0x09U

/*
 * Link training seed values
 * These should ALWAYS match the values defined in nvlink.h
 */
#define NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_NUM                        6U
#define NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE                (0x7U) /* finn: Evaluated from "NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_NUM + 1" */

// NVLINK callback types
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_DL_LINK_MODE      0x00U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_DL_LINK_MODE      0x01U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_TL_LINK_MODE      0x02U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_TL_LINK_MODE      0x03U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_TX_SUBLINK_MODE   0x04U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_TX_SUBLINK_MODE   0x05U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_RX_SUBLINK_MODE   0x06U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_RX_SUBLINK_MODE   0x07U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_RX_SUBLINK_DETECT 0x08U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_SET_RX_SUBLINK_DETECT 0x09U
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_WRITE_DISCOVERY_TOKEN 0x0AU
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_READ_DISCOVERY_TOKEN  0x0BU
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_TRAINING_COMPLETE     0x0CU
#define NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE_GET_UPHY_LOAD         0x0DU

/*
 * Structure to store the GET_DL_MODE callback params.
 * mode
 *     The current Nvlink DL mode
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_DL_LINK_MODE_PARAMS {
    NvU32 mode;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_DL_LINK_MODE_PARAMS;

/*
 * Structure to store the SET_DL_LINK_MODE callback OFF params
 * seedData
 *     The output seed data
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_OFF_PARAMS {
    NvU32 seedData[NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_OFF_PARAMS;

/*
 * Structure to store the SET_DL_LINK_MODE callback PRE_HS params
 * remoteDeviceType
 *     The input remote Device Type
 * ipVerDlPl
 *     The input DLPL version
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_PRE_HS_PARAMS {
    NvU32 remoteDeviceType;
    NvU32 ipVerDlPl;
} NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_PRE_HS_PARAMS;

/*
 * Structure to store SET_DL_LINK_MODE callback INIT_PHASE1 params
 * seedData[]
 *     The input seed data
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_INIT_PHASE1_PARAMS {
    NvU32 seedData[NV2080_CTRL_INTERNAL_NVLINK_MAX_SEED_BUFFER_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_INIT_PHASE1_PARAMS;

/*
 * Structure to store the Nvlink Remote and Local SID info
 * remoteSid
 *     The output remote SID
 * remoteDeviceType
 *     The output remote Device Type
 * remoteLinkId
 *     The output remote link ID
 * localSid
 *     The output local SID
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_REMOTE_LOCAL_SID_INFO {
    NV_DECLARE_ALIGNED(NvU64 remoteSid, 8);
    NvU32 remoteDeviceType;
    NvU32 remoteLinkId;
    NV_DECLARE_ALIGNED(NvU64 localSid, 8);
} NV2080_CTRL_INTERNAL_NVLINK_REMOTE_LOCAL_SID_INFO;

/*
 * Structure to store the SET_DL_LINK_MODE callback POST_INITNEGOTIATE params
 * bInitnegotiateConfigGood
 *     The output bool if the config is good
 * remoteLocalSidInfo
 *     The output structure containing the Nvlink Remote/Local SID info
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITNEGOTIATE_PARAMS {
    NvBool bInitnegotiateConfigGood;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_REMOTE_LOCAL_SID_INFO remoteLocalSidInfo, 8);
} NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITNEGOTIATE_PARAMS;

/*
 * Structure to store the SET_DL_LINK_MODE callback POST_INITOPTIMIZE params
 * bPollDone
 *     The output bool if the polling has finished
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITOPTIMIZE_PARAMS {
    NvBool bPollDone;
} NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITOPTIMIZE_PARAMS;

/*
 * Structure to store the SET_DL_LINK_MODE callback params
 * mode
 *     The input nvlink state to set
 * bSync
 *     The input sync boolean
 * linkMode
 *     The input link mode to be set for the callback
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_DL_LINK_MODE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 mode, 8);
    NvBool bSync;
    NvU32  linkMode;
    union {
        NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_OFF_PARAMS               linkModeOffParams;
        NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_PRE_HS_PARAMS            linkModePreHsParams;
        NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_INIT_PHASE1_PARAMS       linkModeInitPhase1Params;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITNEGOTIATE_PARAMS linkModePostInitNegotiateParams, 8);
        NV2080_CTRL_INTERNAL_NVLINK_SET_DL_LINK_MODE_POST_INITOPTIMIZE_PARAMS linkModePostInitOptimizeParams;
    } linkModeParams;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_DL_LINK_MODE_PARAMS;

/*
 * Structure to store the GET_TL_MODE callback params.
 * mode
 *     The current Nvlink TL mode
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_TL_LINK_MODE_PARAMS {
    NvU32 mode;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_TL_LINK_MODE_PARAMS;

/*
 * Structure to store the SET_TL_LINK_MODE callback params
 * mode
 *     The input nvlink mode to set
 * bSync
 *     The input sync boolean
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TL_LINK_MODE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 mode, 8);
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TL_LINK_MODE_PARAMS;

/*
 * Structure to store the GET_RX/TX_SUBLINK_MODE callback params
 * sublinkMode
 *     The current Sublink mode
 * sublinkSubMode
 *     The current Sublink sub mode
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_SUBLINK_MODE_PARAMS {
    NvU32 sublinkMode;
    NvU32 sublinkSubMode;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_SUBLINK_MODE_PARAMS;

/*
 * Structure to store the SET_TL_LINK_MODE callback params
 * mode
 *     The input nvlink mode to set
 * bSync
 *     The input sync boolean
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TX_SUBLINK_MODE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 mode, 8);
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TX_SUBLINK_MODE_PARAMS;

/*
 * Structure to store the SET_RX_SUBLINK_MODE callback params
 * mode
 *     The input nvlink mode to set
 * bSync
 *     The input sync boolean
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_SUBLINK_MODE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 mode, 8);
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_SUBLINK_MODE_PARAMS;

/*
 * Structure to store the GET_RX_SUBLINK_DETECT callback params
 * laneRxdetStatusMask
 *     The output RXDET per-lane status mask 
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_RX_DETECT_PARAMS {
    NvU32 laneRxdetStatusMask;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_RX_DETECT_PARAMS;

/*
 * Structure to store the SET_RX_DETECT callback params
 * bSync
 *     The input bSync boolean
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_DETECT_PARAMS {
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_DETECT_PARAMS;

/*
 * Structure to store the RD_WR_DISCOVERY_TOKEN callback params
 * ipVerDlPl
 *     The input DLPL version
 * token
 *     The output token
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS {
    NvU32 ipVerDlPl;
    NV_DECLARE_ALIGNED(NvU64 token, 8);
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS;

/*
 * Structure to store the GET_UPHY_LOAD callback params
 * bUnlocked
 *     The output unlocked boolean
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_UPHY_LOAD_PARAMS {
    NvBool bUnlocked;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_UPHY_LOAD_PARAMS;

/*
 * Structure to store the Union of Callback params
 * type
 *     The input type of callback to be executed
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE {
    NvU8 type;
    union {
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_DL_LINK_MODE_PARAMS getDlLinkMode;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_DL_LINK_MODE_PARAMS setDlLinkMode, 8);
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_TL_LINK_MODE_PARAMS getTlLinkMode;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TL_LINK_MODE_PARAMS setTlLinkMode, 8);
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_SUBLINK_MODE_PARAMS getTxSublinkMode;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_TX_SUBLINK_MODE_PARAMS setTxSublinkMode, 8);
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_SUBLINK_MODE_PARAMS getRxSublinkMode;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_SUBLINK_MODE_PARAMS setRxSublinkMode, 8);
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_RX_DETECT_PARAMS    getRxSublinkDetect;
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_SET_RX_DETECT_PARAMS    setRxSublinkDetect;
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS writeDiscoveryToken, 8);
        NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_RD_WR_DISCOVERY_TOKEN_PARAMS readDiscoveryToken, 8);
        NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_GET_UPHY_LOAD_PARAMS    getUphyLoad;
    } callbackParams;
} NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE;

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK
 *
 * Generic NvLink callback RPC to route commands to GSP
 *
 * [In] linkdId
 *     ID of the link to be used
 * [In/Out] callBackType
 *     Callback params
 */
#define NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS {
    NvU32 linkId;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_CALLBACK_TYPE callbackType, 8);
} NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_CORE_CALLBACK (0x20800a24U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID
 *
 * Update Remote and Local Sid info via GSP
 *
 * [In] linkId
 *     ID of the link to be used
 * [Out] remoteLocalSidInfo
 *     The output structure containing the Nvlink Remote/Local SID info
 */
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS {
    NvU32 linkId;
    NV_DECLARE_ALIGNED(NV2080_CTRL_INTERNAL_NVLINK_REMOTE_LOCAL_SID_INFO remoteLocalSidInfo, 8);
} NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID (0x20800a25U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_ALI_ENABLED
 *
 * Returns if ALI is enabled
 *
 * [Out] bEnableAli
 *     Output boolean for ALI enablement
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS_MESSAGE_ID (0x29U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS {
    NvBool bEnableAli;
} NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_ALI_ENABLED           (0x20800a29U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_TYPE_PROGRAM 0x0U
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_TYPE_RESET   0x1U

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_HSHUB_MUX
 *
 * Generic Hshub Mux Update RPC to route commands to GSP
 *
 * [In] updateType
 *     HSHUB Mux update type to program or reset Mux
 * [In] bSysMem
 *     Boolean to differentiate between sysmen and peer mem
 * [In] peerMask
 *     Mask of peer IDs. Only parsed when bSysMem is false
 */
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS {
    NvBool updateType;
    NvBool bSysMem;
    NvU32  peerMask;
} NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_HSHUB_MUX (0x20800a42U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER
 *
 * Performs all the necessary actions required before setting a peer on NVLink
 *
 * [In] peerId
 *     Peer ID which will be set on NVLink
 * [In] peerLinkMask
 *     Mask of links that connects the given peer
 * [In] bNvswitchConn
 *     Is the GPU connected to NVSwitch
 */
#define NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS_MESSAGE_ID (0x4EU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS {
    NvU32  peerId;
    NV_DECLARE_ALIGNED(NvU64 peerLinkMask, 8);
    NvBool bEgmPeer;
    NvBool bNvswitchConn;
} NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER (0x20800a4eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER
 *
 * Performs all the necessary actions required after setting a peer on NVLink
 *
 * [In] peerMask
 *     Mask of Peer IDs which has been set on NVLink
 */
#define NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS {
    NvU32 peerMask;
} NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER (0x20800a50U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_LOG_OOB_XID
 *
 * Log an XID message to OOB.
 *
 * xid [in]
 *   The XID number of the message.
 *
 * message [in]
 *   The text message, including the NULL terminator.
 *
 * len [in]
 *   The length, in bytes, of the text message, excluding the NULL terminator.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_INTERNAL_LOG_OOB_XID                   (0x20800a56U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS_MESSAGE_ID" */

#define NV2080_INTERNAL_OOB_XID_MESSAGE_BUFFER_SIZE            (81U)

#define NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS_MESSAGE_ID (0x56U)

typedef struct NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS {
    NvU32 xid;
    NvU8  message[NV2080_INTERNAL_OOB_XID_MESSAGE_BUFFER_SIZE];
    NvU32 len;
} NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS;

#define NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_TYPE_SYSMEM 0x1U
#define NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_TYPE_PEER   0x2U

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING
 *
 * Performs all the necessary actions required to remove NVLink mapping (sysmem or peer or both)
 *
 * [In] mapTypeMask
 *     Remove NVLink mapping for the given map types (sysmem or peer or both)
 * [In] peerMask
 *     Mask of Peer IDs which needs to be removed on NVLink
 *     Only parsed if mapTypeMask accounts peer
 * [In] bL2Entry
 *     Is the peer removal happening because links are entering L2 low power state?
 *     Only parsed if mapTypeMask accounts peer
 */
#define NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS_MESSAGE_ID (0x5FU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS {
    NvU32  mapTypeMask;
    NvU32  peerMask;
    NvBool bL2Entry;
} NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING (0x20800a5fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE
 *
 * Performs all the necessary actions required to save/restore HSHUB state during NVLink L2 entry/exit
 *
 * [In] bSave
 *     Whether this is a save/restore operation
 * [In] linkMask
 *     Mask of links for which HSHUB config registers need to be saved/restored
 */
#define NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS_MESSAGE_ID (0x62U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS {
    NvBool bSave;
    NvU32  linkMask;
} NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE (0x20800a62U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_SET        (0x00000000)
#define NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_SAVE       (0x00000001)
#define NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_FLAGS_RESTORE    (0x00000002)

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_PROGRAM_BUFFERREADY
 *
 * Performs all the necessary actions required to save/restore bufferready state during NVLink L2 entry/exit
 *
 * [In] flags
 *     Whether to set, save or restore bufferready
 * [In] bSysmem
 *     Whether to perform the operation for sysmem links or peer links
 * [In] peerLinkMask
 *     Mask of peer links for which bufferready state need to be set/saved/restored
 */
#define NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS_MESSAGE_ID (0x64U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS {
    NvU32  flags;
    NvBool bSysmem;
    NV_DECLARE_ALIGNED(NvU64 peerLinkMask, 8);
} NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_PROGRAM_BUFFERREADY (0x20800a64U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG
 *
 * Performs all the necessary actions required to update the current Nvlink configuration 
 *
 * [out] bNvlinkSysmemEnabled
 *     Whether sysmem nvlink support was enabled
 */
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS_MESSAGE_ID (0x78U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS {
    NvBool bNvlinkSysmemEnabled;
} NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG (0x20800a78U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK
 *
 * Synchronizes the peerLinkMask between CPU-RM and GSP-RM
 *
 * [In] gpuInst
 *     Gpu instance
 * [In] peerLinkMask
 *     Mask of links to the given peer GPU
 */
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS_MESSAGE_ID (0x7DU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS {
    NvU32 gpuInst;
    NV_DECLARE_ALIGNED(NvU64 peerLinkMask, 8);
} NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK (0x20800a7dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION
 *
 * Updates the remote connection information for a link
 *
 * [In] linkId
 *     Id of the link to be used
 * [In] bConnected
 *     Boolean that tracks whether the link is connected
 * [In] remoteDeviceType
 *     Tracks whether the remote device is switch/gpu/ibmnpu/tegra
 * [In] remoteLinkNumber
 *     Tracks the link number for the connected remote device
 */
#define NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS_MESSAGE_ID (0x82U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 remoteDeviceType, 8);
    NV_DECLARE_ALIGNED(NvU64 remoteChipSid, 8);
    NvU32  linkId;
    NvU32  laneRxdetStatusMask;
    NvU32  remoteLinkNumber;
    NvU32  remotePciDeviceId;
    NvU32  remoteDomain;
    NvU8   remoteBus;
    NvU8   remoteDevice;
    NvU8   remoteFunction;
    NvBool bConnected;
} NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION (0x20800a82U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY
 *
 * Enable links post topology via GSP
 *
 * [In]  linkMask
 *     Mask of links to enable
 * [Out] initializedLinks
 *     Mask of links that were initialized
 */
#define NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS_MESSAGE_ID (0x83U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS {
    NvU32 linkMask;
    NvU32 initializedLinks;
} NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY (0x20800a83U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI
 *
 * [In] linkMask
 *     Mask of enabled links to train
 * [In] bSync
 *     The input sync boolean
 */
#define NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS_MESSAGE_ID (0x84U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS {
    NvU32  linkMask;
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI (0x20800a84U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE           64

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET
 *
 * Get link mask post Rx detection
 *
 * [Out] postRxDetLinkMask
 *     Mask of links discovered
 * [Out] laneRxdetStatusMask
 *     RXDET per-lane status mask
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 postRxDetLinkMask, 8);
    NvU32 laneRxdetStatusMask[NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET (0x20800a85U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_LINK_TRAIN_ALI
 *
 * [In] linkMask
 *     Mask of enabled links to train
 * [In] bSync
 *     The input sync boolean
 */
#define NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS_MESSAGE_ID (0x86U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS {
    NvU32  linkMask;
    NvBool bSync;
} NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_LINK_TRAIN_ALI (0x20800a86U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_INTERNAL_NVLINK_DEVICE_LINK_VALUES {
    NvBool bValid;
    NvU8   linkId;
    NvU32  ioctrlId;
    NvU8   pllMasterLinkId;
    NvU8   pllSlaveLinkId;
    NvU32  ipVerDlPl;
} NV2080_CTRL_INTERNAL_NVLINK_DEVICE_LINK_VALUES;

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO
 *
 * [Out] ioctrlMask
 *    Mask of IOCTRLs discovered from PTOP device info table
 * [Out] ioctrlNumEntries
 *    Number of IOCTRL entries in the PTOP device info table
 * [Out] ioctrlSize
 *    Maximum number of entries in the PTOP device info table
 * [Out] discoveredLinks
 *    Mask of links discovered from all the IOCTRLs
 * [Out] ipVerNvlink
 *    IP revision of the NVLink HW
 * [Out] maxSupportedLinks
 *    Maximum number of links supported for a given arch
 * [Out] linkInfo
 *    Per link information
 */

#define NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS {
    NvU32                                          ioctrlMask;
    NvU8                                           ioctrlNumEntries;
    NvU32                                          ioctrlSize;
    NV_DECLARE_ALIGNED(NvU64 discoveredLinks, 8);
    NvU32                                          ipVerNvlink;
    NvU32                                          maxSupportedLinks;
    NV2080_CTRL_INTERNAL_NVLINK_DEVICE_LINK_VALUES linkInfo[NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO (0x20800a87U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_MAX_LINKS_PER_IOCTRL_SW    6U

typedef struct NV2080_CTRL_INTERNAL_NVLINK_DEVICE_IP_REVISION_VALUES {
    NvU32 ipVerIoctrl;
    NvU32 ipVerMinion;
} NV2080_CTRL_INTERNAL_NVLINK_DEVICE_IP_REVISION_VALUES;

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO
 *
 * [In] ioctrlIdx
 *    IOCTRL index
 * [Out] PublicId
 *    PublicId of the IOCTRL discovered
 * [Out] localDiscoveredLinks
 *    Mask of discovered links local to the IOCTRL
 * [Out] localGlobalLinkOffset
 *    Global link offsets for the locally discovered links
 * [Out] ioctrlDiscoverySize
 *    IOCTRL table size
 * [Out] numDevices
 *    Number of devices discovered from the IOCTRL
 * [Out] deviceIpRevisions
 *    IP revisions for the devices discovered in the IOCTRL
 */

#define NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS_MESSAGE_ID (0x8EU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS {
    NvU32                                                 ioctrlIdx;
    NvU32                                                 PublicId;
    NvU32                                                 localDiscoveredLinks;
    NvU32                                                 localGlobalLinkOffset;
    NvU32                                                 ioctrlDiscoverySize;
    NvU8                                                  numDevices;
    NV2080_CTRL_INTERNAL_NVLINK_DEVICE_IP_REVISION_VALUES ipRevisions;
} NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO (0x20800a8eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_PROGRAM_LINK_SPEED
 *
 * Program NVLink Speed from OS/VBIOS
 *
 * [In] bPlatformLinerateDefined
 *    Whether line rate is defined in the platform
 * [In] platformLineRate
 *    Platform defined line rate
 * [Out] nvlinkLinkSpeed
 *    The line rate that was programmed for the links
 */
#define NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS_MESSAGE_ID (0x8FU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS {
    NvBool bPlatformLinerateDefined;
    NvU32  platformLineRate;
    NvU32  nvlinkLinkSpeed;
} NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_PROGRAM_LINK_SPEED (0x20800a8fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_ARE_LINKS_TRAINED
 *
 * [In] linkMask
 *     Mask of links whose state will be checked
 * [In] bActiveOnly
 *     The input boolean to check for Link Active state
 * [Out] bIsLinkActive
 *     Boolean array to track if the link is trained
 */
#define NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS_MESSAGE_ID (0x90U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS {
    NvU32  linkMask;
    NvBool bActiveOnly;
    NvBool bIsLinkActive[NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_ARE_LINKS_TRAINED (0x20800a90U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_NVLINK_RESET_FLAGS_ASSERT      (0x00000000)
#define NV2080_CTRL_INTERNAL_NVLINK_RESET_FLAGS_DEASSERT    (0x00000001)
#define NV2080_CTRL_INTERNAL_NVLINK_RESET_FLAGS_TOGGLE      (0x00000002)

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_RESET_LINKS
 *
 * [In] linkMask
 *     Mask of links which need to be reset
 * [In] flags
 *     Whether to assert, de-assert or toggle the Nvlink reset
 */

#define NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS_MESSAGE_ID (0x91U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS {
    NvU32 linkMask;
    NvU32 flags;
} NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_RESET_LINKS (0x20800a91U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS
 *
 * [In] linkMask
 *     Mask of links for which DL interrrupts need to be disabled
 */
#define NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS_MESSAGE_ID (0x92U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS {
    NvU32 linkMask;
} NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS (0x20800a92U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS_MESSAGE_ID" */

/*
 * Structure to store the GET_LINK_AND_CLOCK__INFO params
 *
 * [Out] bLinkConnectedToSystem
 *     Boolean indicating sysmem connection of a link
 * [Out] bLinkConnectedToPeer
 *     Boolean indicating peer connection of a link
 * [Out] bLinkReset
 *     Whether the link is in reset
 * [Out] subLinkWidth
 *     Number of lanes per sublink
 * [Out] linkState
 *     Mode of the link
 * [Out] txSublinkState
 *     Tx sublink state
 * [Out] rxSublinkState
 *     Rx sublink state
 * [Out] bLaneReversal
 *     Boolean indicating if a link's lanes are reversed
 * [Out] nvlinkLinkClockKHz
 *     Link clock value in KHz
 * [Out] nvlinkLineRateMbps
 *     Link line rate in Mbps
 * [Out] nvlinkLinkClockMhz
 *     Link clock in MHz
 * [Out] nvlinkLinkDataRateKiBps
 *     Link Data rate in KiBps
 * [Out] nvlinkRefClkType
 *     Current Nvlink refclk source
 * [Out] nvlinkReqLinkClockMhz
 *     Requested link clock value
 * [Out] nvlinkMinL1Threshold
 *     Requested link Min L1 Threshold
 * [Out] nvlinkMaxL1Threshold
 *     Requested link Max L1 Threshold
 * [Out] nvlinkL1ThresholdUnits
 *     Requested link L1 Threshold Units
 */
typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_VALUES {
    NvBool bLinkConnectedToSystem;
    NvBool bLinkConnectedToPeer;
    NvBool bLinkReset;
    NvU8   subLinkWidth;
    NvU32  linkState;
    NvU32  txSublinkState;
    NvU32  rxSublinkState;
    NvBool bLaneReversal;
    NvU32  nvlinkLinkClockKHz;
    NvU32  nvlinkLineRateMbps;
    NvU32  nvlinkLinkClockMhz;
    NvU32  nvlinkLinkDataRateKiBps;
    NvU8   nvlinkRefClkType;
    NvU32  nvlinkReqLinkClockMhz;
    NvU32  nvlinkMinL1Threshold;
    NvU32  nvlinkMaxL1Threshold;
    NvU32  nvlinkL1ThresholdUnits;
} NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_VALUES;

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO
 *
 * [In] linkMask
 *     Mask of enabled links to loop over
 * [Out] nvlinkRefClkSpeedKHz
 *     Ref clock value n KHz
 * [Out] linkInfo
 *     Per link information
 */
#define NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS_MESSAGE_ID (0x93U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NvU32                                                 nvlinkRefClkSpeedKHz;
    NvBool                                                bSublinkStateInst; // whether instantaneous sublink state is needed
    NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_VALUES linkInfo[NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO (0x20800a93U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM
 *
 * Updates the HSHUB sysmem config resgister state to reflect sysmem NVLinks
 *
 * [In] sysmemLinkMask
 *     Mask of discovered sysmem NVLinks
 */
#define NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS_MESSAGE_ID (0x94U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS {
    NvU32 sysmemLinkMask;
} NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS;
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM (0x20800a94U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS
 *
 * Process NVLink forced configurations which includes setting of HSHUB and memory system
 *
 * [In] bLegacyForcedConfig
 *     Tracks whether the forced config is legacy forced config or chiplib config
 * [Out] bOverrideComputePeerMode
 *     Whether compute peer mode was enabled
 * [In] phase
 *     Only applicable when bLegacyForcedConfig is true
 *     Tracks the set of registers to program from the NVLink table
 * [In] linkConnection
 *     Array of chiplib configurations
 */
#define NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS_MESSAGE_ID (0x95U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS {
    NvBool bLegacyForcedConfig;
    NvBool bOverrideComputePeerMode;
    NvU32  phase;
    NvU32  linkConnection[NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS (0x20800a95U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS
 *
 * Sync the NVLink lane shutdown properties with GSP-RM
 *
 * [In] bLaneShutdownOnUnload
 *     Whether nvlink shutdown should be triggered on driver unload
 */
#define NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS_MESSAGE_ID (0x96U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS {
    NvBool bLaneShutdownOnUnload;
} NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS (0x20800a96U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS
 *
 * Enable ATS functionality related to NVLink sysmem if hardware support is available
 *
 * [In] notUsed
 */
#define NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS_MESSAGE_ID (0x97U)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS {
    NvU32 notUsed;
} NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS (0x20800a97U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK
 *
 *  Get the mask of Nvlink links connected to system
 *
 * [Out] sysmemLinkMask
 *      Mask of Nvlink links connected to system
 */
#define NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS_MESSAGE_ID (0xABU)

typedef struct NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS {
    NvU32 sysmemLinkMask;
} NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK (0x20800aabU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO
 *
 *  Send CMC gsplite the address and size of log buffer allocated on sysmem
 *
 * [In] PublicId
 *     ID of the gsplite engine
 * [In] logBufferSize
 *     Size of the log buffer allocated on sysmem
 * [In] logBufferAddr
 *     Start address of the log buffer
 */
#define NV2080_CTRL_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV2080_CTRL_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO_PARAMS {
    NvU32 PublicId;
    NV_DECLARE_ALIGNED(NvU64 logBufferSize, 8);
    NV_DECLARE_ALIGNED(NvU64 logBufferAddr, 8);
} NV2080_CTRL_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO              (0x20800a89U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO_PARAMS_MESSAGE_ID" */


/*
* NV2080_CTRL_CMD_INTERNAL_NVLINK_REPLAY_SUPPRESSED_ERRORS
*
* Request from CPU-RM to proccess supressed errors during boot on GSP
* This command accepts no parameters.
*/
#define NV2080_CTRL_CMD_INTERNAL_NVLINK_REPLAY_SUPPRESSED_ERRORS         (0x20800b01U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_2_INTERFACE_ID << 8) | 0x1" */


/*!
 * @ref NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER_V2
 */




typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER_V2 {
    NvU32                                    smIssueRateModifierListSize;
    NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2 smIssueRateModifierList[NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE];
} NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER_V2;

#define NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_MESSAGE_ID (0x02U)

typedef struct NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS {
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER_V2 smIssueRateModifierV2[NV2080_CTRL_INTERNAL_GR_MAX_ENGINES];
} NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_V2 (0x20800b03) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_2_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_MESSAGE_ID (0x03U)

typedef NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS NV2080_CTRL_INTERNAL_STATIC_KGR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS;

/* ctrl2080internal_h */
