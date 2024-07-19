/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080ce.finn
//



#include "nvcfg_sdk.h"


/* NV20_SUBDEVICE_XX ce control commands and parameters */

/*
 * NV2080_CTRL_CMD_CE_GET_CAPS
 *
 * This command returns the set of CE capabilities for the device
 * in the form of an array of unsigned bytes.
 *
 *   ceEngineType
 *     This parameter specifies the copy engine type
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table per CE.
 *     This value should be set to NV2080_CTRL_CE_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the CE caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_CE_GET_CAPS  (0x20802a01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_CAPS_PARAMS_MESSAGE_ID" */

/*
 * Size in bytes of CE caps table.  This value should be one greater
 * than the largest byte_index value below.
 */
#define NV2080_CTRL_CE_CAPS_TBL_SIZE 2

#define NV2080_CTRL_CE_GET_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_CE_GET_CAPS_PARAMS {
    NvU32 ceEngineType;
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV2080_CTRL_CE_GET_CAPS_PARAMS;

#define NV2080_CTRL_CMD_CE_GET_CAPS_V2 (0x20802a03) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_CE_GET_CAPS_V2_PARAMS {
    NvU32 ceEngineType;
    NvU8  capsTbl[NV2080_CTRL_CE_CAPS_TBL_SIZE];
} NV2080_CTRL_CE_GET_CAPS_V2_PARAMS;

/* extract cap bit setting from tbl */
#define NV2080_CTRL_CE_GET_CAP(tbl,c)               (((NvU8)tbl[(1?c)]) & (0?c))

/* caps format is byte_index:bit_mask */
#define NV2080_CTRL_CE_CAPS_CE_GRCE                          0:0x01
#define NV2080_CTRL_CE_CAPS_CE_SHARED                        0:0x02
#define NV2080_CTRL_CE_CAPS_CE_SYSMEM_READ                   0:0x04
#define NV2080_CTRL_CE_CAPS_CE_SYSMEM_WRITE                  0:0x08
#define NV2080_CTRL_CE_CAPS_CE_NVLINK_P2P                    0:0x10
#define NV2080_CTRL_CE_CAPS_CE_SYSMEM                        0:0x20
#define NV2080_CTRL_CE_CAPS_CE_P2P                           0:0x40
#define NV2080_CTRL_CE_CAPS_CE_BL_SIZE_GT_64K_SUPPORTED      0:0x80
#define NV2080_CTRL_CE_CAPS_CE_SUPPORTS_NONPIPELINED_BL      1:0x01
#define NV2080_CTRL_CE_CAPS_CE_SUPPORTS_PIPELINED_BL         1:0x02
#define NV2080_CTRL_CE_CAPS_CE_CC_SECURE                     1:0x04
#define NV2080_CTRL_CE_CAPS_CE_DECOMP_SUPPORTED              1:0x08

/*
 *   NV2080_CTRL_CE_CAPS_CE_GRCE
 *     Set if the CE is synchronous with GR
 *
 *   NV2080_CTRL_CE_CAPS_CE_SHARED
 *     Set if the CE shares physical CEs with any other CE
 *
 *   NV2080_CTRL_CE_CAPS_CE_SYSMEM_READ
 *     Set if the CE can give enhanced performance for SYSMEM reads over other CEs
 *
 *   NV2080_CTRL_CE_CAPS_CE_SYSMEM_WRITE
 *     Set if the CE can give enhanced performance for SYSMEM writes over other CEs
 *
 *   NV2080_CTRL_CE_CAPS_CE_NVLINK_P2P
 *     Set if the CE can be used for P2P transactions using NVLINK
 *     Once a CE is exposed for P2P over NVLINK, it will remain available for the life of RM
 *     PCE2LCE mapping may change based on the number of GPUs registered in RM however
 *
 *   NV2080_CTRL_CE_CAPS_CE_SYSMEM
 *     Set if the CE can be used for SYSMEM transactions
 *
 *   NV2080_CTRL_CE_CAPS_CE_P2P
 *     Set if the CE can be used for P2P transactions
 *
 *   NV2080_CTRL_CE_CAPS_CE_BL_SIZE_GT_64K_SUPPORTED
 *     Set if the CE supports BL copy size greater than 64K
 *
 *   NV2080_CTRL_CE_CAPS_CE_SUPPORTS_NONPIPELINED_BL
 *     Set if the CE supports non-pipelined Block linear
 *
 *   NV2080_CTRL_CE_CAPS_CE_SUPPORTS_PIPELINED_BL
 *     Set if the CE supports pipelined Block Linear
 *
 *   NV2080_CTRL_CE_CAPS_CE_CC_SECURE
 *     Set if the CE is capable of encryption/decryption
 *
 *   NV2080_CTRL_CE_CAPS_CE_DECOMP_SUPPORTED
 *     Set if the CE is capable of handling decompression workloads;
 *     async copies will not be supported on the same CE
 */

/*
 * NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK
 *
 * This command returns the mapping of PCE's for the given LCE.
 *
 *   ceEngineType
 *     This parameter specifies the copy engine type
 *   pceMask
 *     This parameter specifies a mask of PCEs that correspond
 *     to the LCE specified in ceEngineType
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

/*
 * The pceMask is local to the CE shim that ceEngineType belongs to.
 */



#define NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK (0x20802a02) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS {
    NvU32 ceEngineType;
    NvU32 pceMask;
} NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS;

/*
 * NV2080_CTRL_CMD_CE_SET_PCE_LCE_CONFIG
 *
 * This command sets the PCE2LCE configuration
 *
 *   pceLceConfig[NV2080_CTRL_MAX_PCES]
 *     This parameter specifies the PCE-LCE mapping requested
 *   grceLceConfig[NV2080_CTRL_MAX_GRCES]
 *     This parameter specifies which LCE is the GRCE sharing with
 *     0xF -> Does not share with any LCE
 *     0-MAX_LCE -> Shares with the given LCE
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_CE_SET_PCE_LCE_CONFIG (0x20802a04) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_SET_PCE_LCE_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MAX_PCES                  32
#define NV2080_CTRL_MAX_GRCES                 4

#define NV2080_CTRL_CE_SET_PCE_LCE_CONFIG_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_CE_SET_PCE_LCE_CONFIG_PARAMS {
    NvU32 ceEngineType;
    NvU32 pceLceMap[NV2080_CTRL_MAX_PCES];
    NvU32 grceSharedLceMap[NV2080_CTRL_MAX_GRCES];
} NV2080_CTRL_CE_SET_PCE_LCE_CONFIG_PARAMS;

/*
 * NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS
 *
 * This command updates the PCE-LCE mappings
 *
 *   pceLceMap [IN]
 *     This parameter contains the array of PCE to LCE mappings.
 *     The array is indexed by the PCE index, and contains the
 *     LCE index that the PCE is assigned to.  A unused PCE is
 *     tagged with NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE.
 *
 *   grceConfig [IN]
 *     This parameter contains the array of GRCE configs.
 *     0xF -> GRCE does not share with any LCE
 *     0-MAX_LCE -> GRCE shares with the given LCE
 *
 *   exposeCeMask [IN]
 *     This parameter specifies the mask of LCEs to export to the
 *     clients after the update.
 *
 *   bUpdateNvlinkPceLce [IN]
 *     Whether PCE-LCE mappings need to be updated for nvlink topology.
 *     If this is NV_FALSE, RM would ignore the above values.  However,
 *     PCE-LCE mappings will still be updated if there were any regkey
 *     overrides.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */

/*
 * This command updates the PCE-LCE mappings for one CE shim.  On
 * GPUs with multiple CE shims, this interface must be called for
 * each shim.
 *
 *   shimInstance [IN]
 *     Specify which CE shim instance to operate on.
 */



#define NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS (0x20802a05) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS {
    NvU32  pceLceMap[NV2080_CTRL_MAX_PCES];
    NvU32  grceConfig[NV2080_CTRL_MAX_GRCES];
    NvU32  exposeCeMask;
    NvBool bUpdateNvlinkPceLce;
} NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS;

#define NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE      0xf

/*
 * NV2080_CTRL_CMD_CE_UPDATE_CLASS_DB
 *
 * This function triggers an update of the exported CE classes.  CEs with
 * no physical resources will not be exported.  A record of these
 * will be return in in the stubbedCeMask.
 *
 * An example if NV2080_ENGINE_TYPE_COPY4 is stubbed (1<<4) will be
 * set in stubbedCeMask.
 */

/*
 * This function operates on all CE shims.
 */


#define NV2080_CTRL_CMD_CE_UPDATE_CLASS_DB (0x20802a06) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS {
    NvU32 stubbedCeMask;
} NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS;

/*
 * NV2080_CTRL_CMD_CE_GET_PHYSICAL_CAPS
 *
 * Query _CE_GRCE, _CE_SHARED, _CE_SUPPORTS_PIPELINED_BL, _CE_SUPPORTS_NONPIPELINED_BL bits of CE
 * capabilities.
 *
 */

#define NV2080_CTRL_CMD_CE_GET_PHYSICAL_CAPS (0x20802a07) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_PHYSICAL_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_PHYSICAL_CAPS_PARAMS_MESSAGE_ID (0x7U)

typedef NV2080_CTRL_CE_GET_CAPS_V2_PARAMS NV2080_CTRL_CE_GET_PHYSICAL_CAPS_PARAMS;

#define NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS {
    NvU32 size;
} NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS;

#define NV2080_CTRL_CMD_CE_GET_FAULT_METHOD_BUFFER_SIZE (0x20802a08) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASKS
 *
 * Get HSHUB and FBHUB PCE masks.
 *
 *   [out] hshubPceMasks
 *     PCE mask for each HSHUB
 *   [out] fbhubPceMask
 *     FBHUB PCE mask
 */

#define NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASK             (0x20802a09) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_MAX_HSHUBS                       32

#define NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS {
    NvU32 hshubPceMasks[NV2080_CTRL_CE_MAX_HSHUBS];
    NvU32 fbhubPceMask;
} NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS;

/*
 * NV2080_CTRL_CMD_CE_GET_ALL_CAPS
 *
 * Query caps of all CEs.
 *
 *   [out] capsTbl
 *     Array of CE caps in the order of CEs. The caps bits interpretation is the same as in
 *     NV2080_CTRL_CMD_CE_GET_CAPS.
 *   [out] present
 *     Bit mask indicating which CEs are usable by the client and have their caps indicated in capsTbl.
 *     If a CE is not marked present, its caps bits should be ignored.
 *     If client is subscribed to a MIG instance, only the CEs present in the instance are tagged as such.
 */

#define NV2080_CTRL_CMD_CE_GET_ALL_CAPS (0x20802a0a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MAX_CES             64

#define NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_MESSAGE_ID (0xaU)

typedef struct NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS {
    NvU8 capsTbl[NV2080_CTRL_MAX_CES][NV2080_CTRL_CE_CAPS_TBL_SIZE];
    NV_DECLARE_ALIGNED(NvU64 present, 8);
} NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS;

#define NV2080_CTRL_CMD_CE_GET_ALL_PHYSICAL_CAPS (0x20802a0b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_ALL_PHYSICAL_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_ALL_PHYSICAL_CAPS_PARAMS_MESSAGE_ID (0xbU)

typedef NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS NV2080_CTRL_CE_GET_ALL_PHYSICAL_CAPS_PARAMS;


/*
 * NV2080_CTRL_CMD_CE_GET_LCE_SHIM_INFO
 *
 * This command queries LCE shim information of a specified CE.
 * The information includes the shim instance the CE belongs to.
 * And the local LCE within the shim.
 *
 *   [in] ceEngineType
 *     This parameter specifies the copy engine type, NV2080 define
 *   [out] shimInstance
 *     The shim instance the ceEngineType belongs to.
 *   [out] shimLocalLceIdx
 *     The local LCE index within the shim
 *
 */

#define NV2080_CTRL_CMD_CE_GET_LCE_SHIM_INFO (0x20802a0c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS_MESSAGE_ID (0xcU)

typedef struct NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS {
    NvU32 ceEngineType;
    NvU32 shimInstance;
    NvU32 shimLocalLceIdx;
} NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS;

/*
 * This command is identical to NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS
 * but supports more than one CE shim.
 *
 * This command updates the PCE-LCE mappings for one CE shim.  On
 * GPUs with multiple CE shims, this interface must be called for
 * each shim.
 *
 *   shimInstance [IN]
 *     Specify which CE shim instance to operate on.
 */

#define NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS_V2 (0x20802a0d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS_MESSAGE_ID (0xdU)

typedef struct NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS {
    NvU32  pceLceMap[NV2080_CTRL_MAX_PCES];
    NvU32  grceConfig[NV2080_CTRL_MAX_GRCES];
    NvU32  exposeCeMask;
    NvBool bUpdateNvlinkPceLce;
    NvU32  shimInstance;
} NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS;

/*
 * This command is identical to NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASK_PARAMS
 * but supports more than one CE shim.
 *
 * This command gets HSHUB/CEHUB and FBHUB PCE Mask.  On
 * GPUs with multiple CE shims, this interface must be called for
 * each shim.
 *
 *   [in] shimInstance
 *     Specify which CE shim instance to operate on.
 */

#define NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASK_V2 (0x20802a0e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS_MESSAGE_ID (0xeU)

typedef struct NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS {
    NvU32 connectingHubPceMasks[NV2080_CTRL_CE_MAX_HSHUBS];
    NvU32 fbhubPceMask;
    NvU32 shimInstance;
} NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS;

typedef enum NV2080_CTRL_CE_LCE_TYPE {
    NV2080_CTRL_CE_LCE_TYPE_PCIE = 1,
    NV2080_CTRL_CE_LCE_TYPE_DECOMP = 2,
    NV2080_CTRL_CE_LCE_TYPE_SCRUB = 3,
    NV2080_CTRL_CE_LCE_TYPE_NVLINK_PEER = 4,
    NV2080_CTRL_CE_LCE_TYPE_C2C = 5,
} NV2080_CTRL_CE_LCE_TYPE;

/*
 * NV2080_CTRL_CMD_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE
 * 
 * This command queries the PCE config required for the specified LCE type.
 *
 *  [in] lceType
 *      LCE type. Should be one of NV2080_CTRL_CE_LCE_TYPE_* values.
 *  [out] numPces
 *      Number of PCEs supported per LCE
 *  [out] numLces
 *      Maximum number of LCEs supported by the chip for the specified LCE type.
 *  [out] supportedPceMask
 *      The mask of the PCEs that support the specified LCE type.
 *  [out] supportedLceMask
 *      The mask of the LCEs that support the specified LCE type.
 *  [out] pcePerHshub
 *      Numbers of PCEs from any given HSHUB that can be assigned to this LCE type.
 *
 *  @return NV_OK
 */

#define NV2080_CTRL_CMD_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE (0x20802a0f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS_MESSAGE_ID (0xfU)

typedef struct NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS {
    NV2080_CTRL_CE_LCE_TYPE lceType;
    NvU32                   numPces;
    NvU32                   numLces;
    NvU32                   supportedPceMask;
    NvU32                   supportedLceMask;
    NvU32                   pcePerHshub;
} NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS;

/*
 * NV2080_CTRL_CMD_CE_GET_DECOMP_LCE_MASK
 *
 * This command gets the mask of LCEs that are enabled for decomp workloads.
 * On GPUs with multiple CE shims, this interface must be called for
 * each shim.
 *
 *   [in] shimInstance
 *     Specify which CE shim instance to operate on.
 *   [out] decompLceMask
 *     Returns a 64-bit mask of which LCEs in given shim are marked as decomp CEs
 */

#define NV2080_CTRL_CMD_CE_GET_DECOMP_LCE_MASK (0x20802a11) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_CE_INTERFACE_ID << 8) | NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 decompLceMask, 8);
    NvU32 shimInstance;
} NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS;



/* _ctrl2080ce_h_ */
