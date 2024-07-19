/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080gpu.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"
#include "ctrl/ctrl2080/ctrl2080gr.h"
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "nvcfg_sdk.h"
#include "nvstatus.h"

#define NV_GRID_LICENSE_INFO_MAX_LENGTH (128)

/* License info strings for vGPU products */
#define NV_GRID_LICENSE_FEATURE_VPC_EDITION                 "GRID-Virtual-PC,2.0;Quadro-Virtual-DWS,5.0;GRID-Virtual-WS,2.0;GRID-Virtual-WS-Ext,2.0"
#define NV_GRID_LICENSE_FEATURE_VAPPS_EDITION               "GRID-Virtual-Apps,3.0"
#define NV_GRID_LICENSE_FEATURE_VIRTUAL_WORKSTATION_EDITION "Quadro-Virtual-DWS,5.0;GRID-Virtual-WS,2.0;GRID-Virtual-WS-Ext,2.0"
#define NV_GRID_LICENSE_FEATURE_GAMING_EDITION              "GRID-vGaming,8.0"
#define NV_GRID_LICENSE_FEATURE_COMPUTE_EDITION             "NVIDIA-vComputeServer,9.0;Quadro-Virtual-DWS,5.0"

#define NV_GRID_LICENSED_PRODUCT_VWS     "NVIDIA RTX Virtual Workstation"
#define NV_GRID_LICENSED_PRODUCT_GAMING  "NVIDIA Cloud Gaming"
#define NV_GRID_LICENSED_PRODUCT_VPC     "NVIDIA Virtual PC"
#define NV_GRID_LICENSED_PRODUCT_VAPPS   "NVIDIA Virtual Applications"
#define NV_GRID_LICENSED_PRODUCT_COMPUTE "NVIDIA Virtual Compute Server"



/* NV20_SUBDEVICE_XX gpu control commands and parameters */



typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_GPU_INFO;

/* valid gpu info index values */

#define NV2080_CTRL_GPU_INFO_INDEX_INDEX                   23:0



#define NV2080_CTRL_GPU_INFO_INDEX_ECID_LO32                           (0x00000001U)
#define NV2080_CTRL_GPU_INFO_INDEX_ECID_HI32                           (0x00000002U)
#define NV2080_CTRL_GPU_INFO_INDEX_MINOR_REVISION_EXT                  (0x00000004U)


#define NV2080_CTRL_GPU_INFO_INDEX_NETLIST_REV0                        (0x00000012U)
#define NV2080_CTRL_GPU_INFO_INDEX_NETLIST_REV1                        (0x00000013U)


#define NV2080_CTRL_GPU_INFO_INDEX_ECID_EXTENDED                       (0x0000001bU)
#define NV2080_CTRL_GPU_INFO_INDEX_SYSMEM_ACCESS                       (0x0000001fU)


#define NV2080_CTRL_GPU_INFO_INDEX_GEMINI_BOARD                        (0x00000022U)


#define NV2080_CTRL_GPU_INFO_INDEX_SURPRISE_REMOVAL_POSSIBLE           (0x00000025U)
#define NV2080_CTRL_GPU_INFO_INDEX_IBMNPU_RELAXED_ORDERING             (0x00000026U)
#define NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED          (0x00000027U)
#define NV2080_CTRL_GPU_INFO_INDEX_NVSWITCH_PROXY_DETECTED             (0x00000028U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SR_SUPPORT                      (0x00000029U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SMC_MODE                        (0x0000002aU)
#define NV2080_CTRL_GPU_INFO_INDEX_SPLIT_VAS_MGMT_SERVER_CLIENT_RM     (0x0000002bU)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SM_VERSION                      (0x0000002cU)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY                  (0x0000002dU)


#define NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM             (0x0000002fU)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY                  (0x00000030U)
#define NV2080_CTRL_GPU_INFO_INDEX_NVENC_STATS_REPORTING_STATE         (0x00000031U)


#define NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED          (0x00000033U)
#define NV2080_CTRL_GPU_INFO_INDEX_DISPLAY_ENABLED                     (0x00000034U)
#define NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED               (0x00000035U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_PROFILING_CAPABILITY            (0x00000036U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_DEBUGGING_CAPABILITY            (0x00000037U)


#define NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY            (0x0000003aU)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY          (0x0000003bU)
#define NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU                             (0x0000003cU)
#define NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY                   (0x0000003dU)


#define NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED          (0x0000003fU)



#define NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE                             (0x00000041U)

#define NV2080_CTRL_GPU_INFO_INDEX_GROUP_ID                30:24


#define NV2080_CTRL_GPU_INFO_INDEX_RESERVED                31:31

/* valid minor revision extended values */
#define NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_NONE                   (0x00000000U)
#define NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_P                      (0x00000001U)
#define NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_V                      (0x00000002U)
#define NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_PV                     (0x00000003U)



/* valid system memory access capability values */
#define NV2080_CTRL_GPU_INFO_SYSMEM_ACCESS_NO                          (0x00000000U)
#define NV2080_CTRL_GPU_INFO_SYSMEM_ACCESS_YES                         (0x00000001U)



/* valid gemini board values */
#define NV2080_CTRL_GPU_INFO_INDEX_GEMINI_BOARD_NO                     (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GEMINI_BOARD_YES                    (0x00000001U)

/* valid surprise removal values */
#define NV2080_CTRL_GPU_INFO_INDEX_SURPRISE_REMOVAL_POSSIBLE_NO        (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_SURPRISE_REMOVAL_POSSIBLE_YES       (0x00000001U)

/* valid relaxed ordering values */
#define NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_DISABLED          (0x00000000U)
#define NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_ENABLED           (0x00000001U)
#define NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_UNSUPPORTED       (0xFFFFFFFFU)

/* valid poison fuse capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED_NO       (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED_YES      (0x00000001U)

/* valid nvswitch proxy detected values */
#define NV2080_CTRL_GPU_INFO_NVSWITCH_PROXY_DETECTED_NO                (0x00000000U)
#define NV2080_CTRL_GPU_INFO_NVSWITCH_PROXY_DETECTED_YES               (0x00000001U)

/* valid NVSR GPU support info values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SR_SUPPORT_NO                   (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SR_SUPPORT_YES                  (0x00000001U)

/* valid SMC mode values */
#define NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_UNSUPPORTED                  (0x00000000U)
#define NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLED                      (0x00000001U)
#define NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLED                     (0x00000002U)
#define NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLE_PENDING               (0x00000003U)
#define NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLE_PENDING              (0x00000004U)

/* valid split VAS mode values */
#define NV2080_CTRL_GPU_INFO_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_NO        (0x00000000U)
#define NV2080_CTRL_GPU_INFO_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_YES       (0x00000001U)

/* valid grid capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY_NO               (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY_YES              (0x00000001U)

/* valid per runlist channel ram capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM_DISABLED    (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM_ENABLED     (0x00000001U)

/* valid ATS capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY_NO               (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY_YES              (0x00000001U)

/* valid Nvenc Session Stats reporting state values */
#define NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_DISABLED      (0x00000000U)
#define NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED       (0x00000001U)
#define NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_NOT_SUPPORTED (0x00000002U)

/* valid 4K PAGE isolation requirement values */
#define NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED_NO       (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED_YES      (0x00000001U)

/* valid display enabled values */
#define NV2080_CTRL_GPU_INFO_DISPLAY_ENABLED_NO                        (0x00000000U)
#define NV2080_CTRL_GPU_INFO_DISPLAY_ENABLED_YES                       (0x00000001U)

/* valid mobile config enabled values */
#define NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED_NO            (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED_YES           (0x00000001U)


/* valid profiling capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_PROFILING_CAPABILITY_DISABLED   (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_PROFILING_CAPABILITY_ENABLED    (0x00000001U)

/* valid debugging capability values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_DEBUGGING_CAPABILITY_DISABLED   (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_DEBUGGING_CAPABILITY_ENABLED    (0x00000001U)



/* valid local EGM supported values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY_NO         (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY_YES        (0x00000001U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_PEERID                          31:1

/* valid self hosted values */
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY_NO       (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY_YES      (0x00000001U)

/* valid CMP (Crypto Mining Processor) SKU values */
#define NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU_NO                          (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU_YES                         (0x00000001U)


/* valid dma-buf suport values */
#define NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY_NO                (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY_YES               (0x00000001U)

/* valid resetless MIG device supported values */
#define NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED_NO       (0x00000000U)
#define NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED_YES      (0x00000001U)

/*
 * NV2080_CTRL_CMD_GPU_GET_INFO
 *
 * This command returns gpu information for the associated GPU.  Requests
 * to retrieve gpu information use a list of one or more NV2080_CTRL_GPU_INFO
 * structures.
 *
 *   gpuInfoListSize
 *     This field specifies the number of entries on the caller's
 *     gpuInfoList.
 *   gpuInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the gpu information is to be returned.
 *     This buffer must be at least as big as gpuInfoListSize multiplied
 *     by the size of the NV2080_CTRL_GPU_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_GPU_GET_INFO                                   (0x20800101U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_GPU_GET_INFO_PARAMS {
    NvU32 gpuInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 gpuInfoList, 8);
} NV2080_CTRL_GPU_GET_INFO_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_INFO_V2 (0x20800102U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_GPU_GET_INFO_V2_PARAMS {
    NvU32                gpuInfoListSize;
    NV2080_CTRL_GPU_INFO gpuInfoList[NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE];
} NV2080_CTRL_GPU_GET_INFO_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_NAME_STRING
 *
 * This command returns the name of the GPU in string form in either ASCII
 * or UNICODE format.
 *
 *   gpuNameStringFlags
 *     This field specifies flags to use while creating the GPU name string.
 *     Valid flags values:
 *       NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII
 *         The returned name string should be in standard ASCII format.
 *       NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_UNICODE
 *         The returned name string should be in unicode format.
 *   gpuNameString
 *     This field contains the buffer into which the name string should be
 *     returned.  The length of the returned string will be no more than
 *     NV2080_CTRL_GPU_MAX_NAME_STRING_LENGTH bytes in size.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_GPU_GET_NAME_STRING                (0x20800110U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS_MESSAGE_ID" */

#define NV2080_GPU_MAX_NAME_STRING_LENGTH                  (0x0000040U)

// This field is deprecated - 'gpuNameStringFlags' is now a simple scalar.
// Field maintained (and extended from 0:0) for compile-time compatibility.
#define NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE                    31:0

/* valid gpu name string flags */
#define NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII   (0x00000000U)
#define NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_UNICODE (0x00000001U)

#define NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS {
    NvU32 gpuNameStringFlags;
    union {
        NvU8  ascii[NV2080_GPU_MAX_NAME_STRING_LENGTH];
        NvU16 unicode[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    } gpuNameString;
} NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_SHORT_NAME_STRING
 *
 * This command returns the short name of the GPU in ASCII string form.
 *
 *   gpuShortNameString
 *     This field contains the buffer into which the short name string should
 *     be returned.  The length of the returned string will be no more than
 *     NV2080_MAX_NAME_STRING_LENGTH bytes in size.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_GPU_GET_SHORT_NAME_STRING (0x20800111U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS {
    NvU8 gpuShortNameString[NV2080_GPU_MAX_NAME_STRING_LENGTH];
} NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_POWER
 *
 * This command sets the power state for the GPU as a whole, various engines,
 * or clocks.
 *
 *   target
 *     One of NV2080_CTRL_GPU_SET_POWER_TARGET_*
 *
 *   newLevel
 *     One of NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_*
 *            NV2080_CTRL_GPU_SET_POWER_STATE_ENGINE_LEVEL_*
 *            NV2080_CTRL_GPU_SET_POWER_STATE_CLOCK_LEVEL_*
 *     depending on the target above.
 *
 *   oldLevel
 *     Previous level as appropriate.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GPU_SET_POWER (0x20800112U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_POWER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_POWER_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV2080_CTRL_GPU_SET_POWER_PARAMS {
    NvU32 target;
    NvU32 newLevel;
    NvU32 oldLevel;
} NV2080_CTRL_GPU_SET_POWER_PARAMS;



#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_0            (0x00000000U)
#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_1            (0x00000001U)
#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_2            (0x00000002U)
#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3            (0x00000003U)
#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_4            (0x00000004U)
#define NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7            (0x00000007U)

/*
 * NV2080_CTRL_CMD_GPU_GET_SDM
 *
 * This command returns the subdevice mask value for the associated subdevice.
 * The subdevice mask value can be used with the SET_SUBDEVICE_MASK instruction
 * provided by the NV36_CHANNEL_DMA and newer channel dma classes.
 *
 *   subdeviceMask [out]
 *     This field return the subdevice mask value.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_GET_SDM                            (0x20800118U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_SDM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_SDM_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_GPU_GET_SDM_PARAMS {
    NvU32 subdeviceMask;
} NV2080_CTRL_GPU_GET_SDM_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_SDM
 *
 * This command sets the subdevice instance and mask value for the associated subdevice.
 * The subdevice mask value can be used with the SET_SUBDEVICE_MASK instruction
 * provided by the NV36_CHANNEL_DMA and newer channel dma classes.
 * It must be called before the GPU HW is initialized otherwise
 * NV_ERR_INVALID_STATE is being returned.
 *
 *   subdeviceMask [in]
 *     This field configures the subdevice mask value for the GPU/Subdevice
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_DATA
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_SET_SDM (0x20800120U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_SDM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_SDM_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_GPU_SET_SDM_PARAMS {
    NvU32 subdeviceMask;
} NV2080_CTRL_GPU_SET_SDM_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_SIMULATION_INFO
 *
 * This command returns the associated subdevices' simulation information.
 *
 *   type
 *     This field returns the simulation type.
 *     One of NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_*
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_SIMULATION_INFO (0x20800119U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS {
    NvU32 type;
} NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS;

#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_NONE          (0x00000000U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_MODS_AMODEL   (0x00000001U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_LIVE_AMODEL   (0x00000002U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_FMODEL        (0x00000003U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_RTL           (0x00000004U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_EMU           (0x00000005U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_EMU_LOW_POWER (0x00000006U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_DFPGA         (0x00000007U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_DFPGA_RTL     (0x00000008U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_DFPGA_FMODEL  (0x00000009U)
#define NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_UNKNOWN       (0xFFFFFFFFU)

/*
 * NV2080_CTRL_GPU_REG_OP
 *
 * This structure describes register operation information for use with
 * the NV2080_CTRL_CMD_GPU_EXEC_REG_OPS command.  The structure describes
 * a single register operation.  The operation can be a read or write and
 * can involve either 32bits or 64bits of data.
 *
 * For 32bit read operations, the operation takes the following form:
 *
 *   regValueLo = read(bar0 + regOffset)
 *   regValueHi = 0
 *
 * For 64bit read operations, the operation takes the following form:
 *
 *   regValueLo = read(bar0 + regOffset)
 *   regValueHi = read(bar0 + regOffset + 4)
 *
 * For 32bit write operations, the operation takes the following form:
 *
 *   new = ((read(bar0 + regOffset) & ~regAndNMaskLo) | regValueLo)
 *   write(bar0 + regOffset, new)
 *
 * For 64bit write operations, the operation takes the following form:
 *
 *   new_lo = ((read(bar0 + regOffset) & ~regAndNMaskLo) | regValueLo)
 *   new_hi = ((read(bar0 + regOffset + 4) &  ~regAndNMaskHi) | regValueHi)
 *   write(bar0 + regOffset, new_lo)
 *   write(bar0 + regOffset + 4, new_hi)
 *
 * Details on the parameters follow:
 *
 *   regOp
 *     This field specifies the operation to be applied to the register
 *     specified by the regOffset parameter.  Valid values for this
 *     parameter are:
  *      NV2080_CTRL_GPU_REG_OP_READ_08
 *         The register operation should be a 8bit global privileged register read.
 *       NV2080_CTRL_GPU_REG_OP_WRITE_08
 *         The register operation should be a 8bit global privileged register write.
 *       NV2080_CTRL_GPU_REG_OP_READ_32
 *         The register operation should be a 32bit register read.
 *       NV2080_CTRL_GPU_REG_OP_WRITE_32
 *         The register operation should be a 32bit register write.
 *       NV2080_CTRL_GPU_REG_OP_READ_64
 *         The register operation should be a 64bit register read.
 *       NV2080_CTRL_GPU_REG_OP_WRITE_64
 *         The register operation should be a 64bit register write.
 *   regType
 *     This field specifies the type of the register specified by the
 *     regOffset parameter.  Valid values for this parameter are:
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GLOBAL
 *         The register is a global privileged register.  Read operations
 *         return the current value from the associated global register.
 *         Write operations for registers of this type take effect immediately.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX
 *         The register is a graphics context register.  Read operations
 *         return the current value from the associated global register.
 *         Write operations are applied to all existing graphics engine
 *         contexts.  Any newly created graphics engine contexts will also
 *         be modified.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_TPC
 *         This is a graphics context TPC register group. Write operations are
 *         applied to TPC group(s) specified by regGroupMask.
 *         This field is ignored for read operations.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_SM
 *         This is a graphics context SM register group that is inside TPC
 *         group.  Write operations are applied to SM group(s) specified by
 *         regGroupMask (TPC) and regSubGroupMask (SM). This field is ignored
 *         for read operations.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_CROP
 *         This is a graphics context CROP register group. Write operations
 *         are applied to registers specified by regGroupMask. This field is
 *         ignored for read operations.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_ZROP
 *         This is a graphics context ZROP register group. Write operations
 *         are applied to registers specified by regGroupMask. This field is
 *         ignored for read operations.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_FB
 *         This is a fb register group. Write operations are applied to
 *         registers specified by regGroupMask. This field is
 *         ignored for read operations.
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_QUAD
 *         This is a graphics context QUAD register group. Operations
 *         are applied to registers specified by regQuad value.
 *   regQuad
 *     This field specifies the quad to be accessed for register regOffsetwhen
 *     the regType specified is NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_QUAD.
 *   regGroupMask
 *     This field specifies which registers inside an array should be updated.
 *     This field is used when regType is one of below:
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_TPC
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_SM
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_CROP
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_ZROP
 *       NV2080_CTRL_GPU_REG_OP_TYPE_FB
 *     When regGroupMask is used, the regOffset MUST be the first register in
 *     an array.
 *   regSubGroupMask
 *     This field specifies which registers inside a group should be updated.
 *     This field is used for updating SM registers when regType is:
 *       NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_TPC
 *     When regSubGroupMask is used, regOffset MUST be the first register in an
 *     array AND also the first one in sub array.  regGroupMask specifies
 *     TPC(X) and regSubGroupMask specifies SM_CTX_N(Y)
 *   regStatus
 *     This field returns the completion status for the associated register
 *     operation in the form of a bitmask.  Possible status values for this
 *     field are:
 *       NV2080_CTRL_GPU_REG_OP_STATUS_SUCCESS
 *         This value indicates the operation completed successfully.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_OP
 *         This bit value indicates that the regOp value is not valid.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_TYPE
 *         This bit value indicates that the regType value is not valid.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_OFFSET
 *         This bit value indicates that the regOffset value is invalid.
 *         The regOffset value must be within the legal BAR0 range for the
 *         associated GPU and must target a supported register with a
 *         supported operation.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_UNSUPPORTED_OFFSET
 *         This bit value indicates that the operation to the register
 *         specified by the regOffset value is not supported for the
 *         associated GPU.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_MASK
 *         This bit value indicates that the regTpcMask value is invalid.
 *         The regTpcMask must be a subset of TPCs that are enabled on the
 *         associated GPU.
 *       NV2080_CTRL_GPU_REG_OP_STATUS_NOACCESS
 *         The caller does not have access to the register at given offset
 *   regOffset
 *     This field specifies the register offset to access.  The specified
 *     offset must be a valid BAR0 offset for the associated GPU.
 *   regValueLo
 *     This field contains the low 32bits of the register value.
 *     For read operations, this value returns the current value of the
 *     register specified by regOffset.  For write operations, this field
 *     specifies the logical OR value applied to the current value
 *     contained in the register specified by regOffset.
 *   regValueHi
 *     This field contains the high 32bits of the register value.
 *     For read operations, this value returns the current value of the
 *     register specified by regOffset + 4.  For write operations, this field
 *     specifies the logical OR value applied to the current value
 *     contained in the register specified by regOffset + 4.
 *   regAndNMaskLo
 *     This field contains the mask used to clear a desired field from
 *     the current value contained in the register specified by regOffsetLo.
 *     This field is negated and ANDed to this current register value.
 *     This field is only used for write operations.  This field is ignored
 *     for read operations.
 *   regAndNMaskHi
 *     This field contains the mask used to clear a desired field from
 *     the current value contained in the register specified by regOffsetHi.
 *     This field is negated and ANDed to this current register value.
 *     This field is only used for write operations.  This field is ignored
 *     for read operations.
 */
typedef struct NV2080_CTRL_GPU_REG_OP {
    NvU8  regOp;
    NvU8  regType;
    NvU8  regStatus;
    NvU8  regQuad;
    NvU32 regGroupMask;
    NvU32 regSubGroupMask;
    NvU32 regOffset;
    NvU32 regValueHi;
    NvU32 regValueLo;
    NvU32 regAndNMaskHi;
    NvU32 regAndNMaskLo;
} NV2080_CTRL_GPU_REG_OP;

/* valid regOp values */
#define NV2080_CTRL_GPU_REG_OP_READ_32               (0x00000000U)
#define NV2080_CTRL_GPU_REG_OP_WRITE_32              (0x00000001U)
#define NV2080_CTRL_GPU_REG_OP_READ_64               (0x00000002U)
#define NV2080_CTRL_GPU_REG_OP_WRITE_64              (0x00000003U)
#define NV2080_CTRL_GPU_REG_OP_READ_08               (0x00000004U)
#define NV2080_CTRL_GPU_REG_OP_WRITE_08              (0x00000005U)

/* valid regType values */
#define NV2080_CTRL_GPU_REG_OP_TYPE_GLOBAL           (0x00000000U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX           (0x00000001U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_TPC       (0x00000002U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_SM        (0x00000004U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_CROP      (0x00000008U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_ZROP      (0x00000010U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_FB               (0x00000020U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX_QUAD      (0x00000040U)
#define NV2080_CTRL_GPU_REG_OP_TYPE_DEVICE           (0x00000080U)

/* valid regStatus values (note: NvU8 ie, 1 byte) */
#define NV2080_CTRL_GPU_REG_OP_STATUS_SUCCESS        (0x00U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_OP     (0x01U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_TYPE   (0x02U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_OFFSET (0x04U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_UNSUPPORTED_OP (0x08U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_INVALID_MASK   (0x10U)
#define NV2080_CTRL_GPU_REG_OP_STATUS_NOACCESS       (0x20U)

/*
 * NV2080_CTRL_CMD_GPU_EXEC_REG_OPS
 *
 * This command is used to submit a buffer containing one or more
 * NV2080_CTRL_GPU_REG_OP structures for processing.  Each entry in the
 * buffer specifies a single read or write operation.  Each entry is checked
 * for validity in an initial pass over the buffer with the results for
 * each operation stored in the corresponding regStatus field. Unless
 * bNonTransactional flag is set to true, if any invalid entries are found
 * during this initial pass then none of the operations are executed. Entries
 * are processed in order within each regType with NV2080_CTRL_GPU_REG_OP_TYPE_GLOBAL
 * entries processed first followed by NV2080_CTRL_GPU_REG_OP_TYPE_GR_CTX entries.
 *
 *   hClientTarget
 *     This parameter specifies the handle of the client that owns the channel
 *     specified by hChannelTarget. If this parameter is set to 0 then the set
 *     of channel-specific register operations are applied to all current and
 *     future channels.
 *   hChannelTarget
 *     This parameter specifies the handle of the target channel (or channel
 *     group) object instance to which channel-specific register operations are
 *     to be directed. If hClientTarget is set to 0 then this parameter must
 *     also be set to 0.
 *   bNonTransactional
 *     This field specifies if command is non-transactional i.e. if set to
 *     true, all the valid operations will be executed.
 *   reserved00
 *     This parameter is reserved for future use.  It should be initialized to
 *     zero for correct operation.
 *   regOpCount
 *     This field specifies the number of entries on the caller's regOps
 *     list.
 *   regOps
 *     This field specifies a pointer in the caller's address space
 *     to the buffer from which the desired register information is to be
 *     retrieved.  This buffer must be at least as big as regInfoCount
 *     multiplied by the size of the NV2080_CTRL_GPU_REG_OP structure.
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine. When SMC is enabled, this
 *     is a mandatory parameter.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_EXEC_REG_OPS             (0x20800122U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS {
    NvHandle hClientTarget;
    NvHandle hChannelTarget;
    NvU32    bNonTransactional;
    NvU32    reserved00[2];
    NvU32    regOpCount;
    NV_DECLARE_ALIGNED(NvP64 regOps, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINES
 *
 * Returns a list of supported engine types along with the number of instances
 * of each type. Querying with engineList NULL returns engineCount.
 *
 *   engineCount
 *     This field specifies the number of entries on the caller's engineList
 *     field.
 *   engineList
 *     This field is a pointer to a buffer of NvU32 values representing the
 *     set of engines supported by the associated subdevice.  Refer to cl2080.h
 *     for the complete set of supported engine types.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_GPU_GET_ENGINES (0x20800123U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ENGINES_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV2080_CTRL_GPU_GET_ENGINES_PARAMS {
    NvU32 engineCount;
    NV_DECLARE_ALIGNED(NvP64 engineList, 8);
} NV2080_CTRL_GPU_GET_ENGINES_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_ENGINES_V2 (0x20800170U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS_MESSAGE_ID" */

/* Must match NV2080_ENGINE_TYPE_LAST from cl2080.h */
#define NV2080_GPU_MAX_ENGINES_LIST_SIZE   0x54U

#define NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS_MESSAGE_ID (0x70U)

typedef struct NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS {
    NvU32 engineCount;
    NvU32 engineList[NV2080_GPU_MAX_ENGINES_LIST_SIZE];
} NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST
 *
 * Returns a list of classes supported by a given engine type.
 *
 *   engineType
 *     This field specifies the engine type being queried.
 *     NV2080_CTRL_ENGINE_TYPE_ALLENGINES will return  classes
 *     supported by all engines.
 *
 *   numClasses
 *     This field specifies the number of classes supported by
 *     engineType.
 *
 *   classList
 *     This field is an array containing the list of supported
 *     classes. Is of type (NvU32*)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST (0x20800124U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS {
    NvU32 engineType;
    NvU32 numClasses;
    NV_DECLARE_ALIGNED(NvP64 classList, 8);
} NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS;


/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_FAULT_INFO
 *
 * This command returns the fault properties of the specified engine type.
 *
 *   engineType
 *     Input parameter.
 *     This field specifies the engine type being queried.
 *     Engine type is specified using the NV2080_ENGINE_TYPE_* defines in cl2080.h.
 *     The list of engines supported by a chip can be got using the
 *     NV2080_CTRL_CMD_GPU_GET_ENGINES ctrl call.
 *
 *   mmuFaultId
 *     Output parameter.
 *     This field returns the MMU fault ID for the specified engine.
 *     If the engine supports subcontext, this field provides the base fault id.
 *
 *   bSubcontextSupported
 *     Output parameter.
 *     Returns TRUE if subcontext faulting is supported by the engine.
 *     Engine that support subcontext use fault IDs in the range [mmuFaultId, mmuFaultId + maxSubCtx).
 *     "maxSubctx" can be found using the NV2080_CTRL_FIFO_INFO ctrl call with
 *     NV2080_CTRL_FIFO_INFO_INDEX_MAX_SUBCONTEXT_PER_GROUP as the index.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_GET_ENGINE_FAULT_INFO (0x20800125U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS {
    NvU32  engineType;
    NvU32  mmuFaultId;
    NvBool bSubcontextSupported;
} NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_QUERY_MODE
 *
 * This command is used to detect the mode of the GPU associated with the
 * subdevice.
 *
 *   mode
 *     This parameter returns the current mode of GPU.  Legal values for
 *     this parameter include:
 *       NV2080_CTRL_GPU_QUERY_MODE_GRAPHICS_MODE
 *         The GPU is currently operating in graphics mode.
 *       NV2080_CTRL_GPU_QUERY_MODE_COMPUTE_MODE
 *         The GPU is currently operating in compute mode.
 *       NV2080_CTRL_GPU_QUERY_MODE_UNKNOWN_MODE
 *         The current mode of the GPU could not be determined.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_GPU_QUERY_MODE           (0x20800128U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_QUERY_MODE_PARAMS_MESSAGE_ID" */

/* valid mode parameter values */
#define NV2080_CTRL_GPU_QUERY_MODE_UNKNOWN_MODE  (0x00000000U)
#define NV2080_CTRL_GPU_QUERY_MODE_GRAPHICS_MODE (0x00000001U)
#define NV2080_CTRL_GPU_QUERY_MODE_COMPUTE_MODE  (0x00000002U)

#define NV2080_CTRL_GPU_QUERY_MODE_PARAMS_MESSAGE_ID (0x28U)

typedef struct NV2080_CTRL_GPU_QUERY_MODE_PARAMS {
    NvU32 mode;
} NV2080_CTRL_GPU_QUERY_MODE_PARAMS;



/*!
 * NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY
 * Data block describing a virtual context buffer to be promoted
 *
 *  gpuPhysAddr [IN]
 *    GPU Physical Address for the buffer
 *  gpuVirtAddr [IN]
 *    GPU Virtual Address for the buffer
 *   size[IN]
 *    Size of this virtual context buffer
 *  physAttr [IN]
 *    Physical memory attributes (aperture, cacheable)
 *  bufferId [IN]
 *    Virtual context buffer type, data type NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_*
 *  bInitialize [IN]
 *   Flag indicating that this virtual context buffer should be initialized prior to promotion.
 *   The client must clear (memset) the buffer to 0x0 prior to initialization.
 *   Following buffers need initialization:
 *    1. NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN
 *    2. NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH
 *    3. NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP
 *    4. NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP
 *  bNonmapped [IN]
 *   Flag indicating that the virtual address is not to be promoted with this
 *   call. It is illegal to set this flag and not set bInitialize.
 */
typedef struct NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY {
    NV_DECLARE_ALIGNED(NvU64 gpuPhysAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 gpuVirtAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 physAttr;
    NvU16 bufferId;
    NvU8  bInitialize;
    NvU8  bNonmapped;
} NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY;

#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_MAIN                         0U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PM                           1U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PATCH                        2U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_BUFFER_BUNDLE_CB             3U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PAGEPOOL                     4U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_ATTRIBUTE_CB                 5U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_RTV_CB_GLOBAL                6U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_POOL                    7U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GFXP_CTRL_BLK                8U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_FECS_EVENT                   9U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_PRIV_ACCESS_MAP              10U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_UNRESTRICTED_PRIV_ACCESS_MAP 11U
#define NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ID_GLOBAL_PRIV_ACCESS_MAP       12U

#define NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES                        16U

/*
 * NV2080_CTRL_CMD_GPU_PROMOTE_CTX
 *
 * This command is used to promote a Virtual Context
 *
 *   engineType
 *     Engine Virtual Context is for
 *   hClient
 *     Client Handle for hVirtMemory
 *   ChID
 *     Hw Channel -- Actually hw index for channel (deprecated)
 *   hChanClient
 *     The client handle for hObject
 *   hObject
 *     Passed in object handle for either a single channel or a channel group
 *   hVirtMemory
 *     Virtual Address handle to map Virtual Context to
 *   virtAddress
 *     Virtual Address to map Virtual Context to
 *   size
 *     size of the Virtual Context
 *   entryCount
 *     Number of valid entries in the promotion entry list
 *   promoteEntry
 *     List of context buffer entries to issue promotions for.
 *
 *   When not using promoteEntry, only hVirtMemory or (virtAddress, size) should be
 *   specified, the code cases based on hVirtMemory(NULL vs non-NULL) so
 *   if both are specified, hVirtMemory has precedence.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED    - The Class does not support version info retrieval
 *   NV_ERR_INVALID_DEVICE   - The Class/Device is not yet ready to provide this info.
 *   NV_ERR_INVALID_ARGUMENT - Bad/Unknown Class ID specified.
 */
#define NV2080_CTRL_CMD_GPU_PROMOTE_CTX                                    (0x2080012bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS {
    NvU32    engineType;
    NvHandle hClient;
    NvU32    ChID;
    NvHandle hChanClient;
    NvHandle hObject;
    NvHandle hVirtMemory;
    NV_DECLARE_ALIGNED(NvU64 virtAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32    entryCount;
    // C form: NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY promoteEntry[NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES];
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY promoteEntry[NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES], 8);
} NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS;
typedef struct NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *PNV2080_CTRL_GPU_PROMOTE_CTX_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_EVICT_CTX
 *
 * This command is used to evict a Virtual Context
 *
 *   engineType
 *     Engine Virtual Context is for
 *   hClient
 *     Client Handle
 *   ChID
 *     Hw Channel -- Actually hw index for channel (deprecated)
 *   hChanClient
 *     Client handle for hObject
 *   hObject
 *     Passed in object handle for either a single channel or a channel group
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED    - The Class does not support version info retrieval
 *   NV_ERR_INVALID_DEVICE   - The Class/Device is not yet ready to provide this info.
 *   NV_ERR_INVALID_ARGUMENT - Bad/Unknown Class ID specified.
 */
#define NV2080_CTRL_CMD_GPU_EVICT_CTX (0x2080012cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_EVICT_CTX_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_EVICT_CTX_PARAMS_MESSAGE_ID (0x2CU)

typedef struct NV2080_CTRL_GPU_EVICT_CTX_PARAMS {
    NvU32    engineType;
    NvHandle hClient;
    NvU32    ChID;
    NvHandle hChanClient;
    NvHandle hObject;
} NV2080_CTRL_GPU_EVICT_CTX_PARAMS;
typedef struct NV2080_CTRL_GPU_EVICT_CTX_PARAMS *PNV2080_CTRL_GPU_EVICT_CTX_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_INITIALIZE_CTX
 *
 * This command is used to initialize a Virtual Context. The ctx buffer must be
 * cleared (zerod) by the caller prior to invoking this method.
 *
 *   engineType
 *     Engine Virtual Context is for
 *   hClient
 *     Client Handle for the hVirtMemory
 *   ChID
 *      Hw channel -- Actually channel index (deprecated)
 *   hChanClient
 *     The client handle for hObject
 *   hObject
 *     Passed in object handle for either a single channel or a channel group
 *   hVirtMemory
 *     Virtual Address where to map Virtual Context to
 *   physAddress
 *     Physical offset in FB to use as Virtual Context
 *   physAttr
 *     Physical memory attributes
 *   hDmaHandle
 *     Dma Handle when using discontiguous context buffers
 *   index
 *     Start offset in Virtual DMA Context
 *   size
 *     Size of the Virtual Context
 *
 *   Only hVirtMemory or size should be specified, the code cases based on hVirtMemory
 *   (NULL vs non-NULL) so if both are specified, hVirtMemory has precedence.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED    - The Class does not support version info retrieval
 *   NV_ERR_INVALID_DEVICE   - The Class/Device is not yet ready to provide this info.
 *   NV_ERR_INVALID_ARGUMENT - Bad/Unknown Class ID specified.
 */
#define NV2080_CTRL_CMD_GPU_INITIALIZE_CTX (0x2080012dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_MESSAGE_ID (0x2DU)

typedef struct NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS {
    NvU32    engineType;
    NvHandle hClient;
    NvU32    ChID;
    NvHandle hChanClient;
    NvHandle hObject;
    NvHandle hVirtMemory;
    NV_DECLARE_ALIGNED(NvU64 physAddress, 8);
    NvU32    physAttr;
    NvHandle hDmaHandle;
    NvU32    index;
    NV_DECLARE_ALIGNED(NvU64 size, 8);
} NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS;
typedef struct NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *PNV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS;

#define NV2080_CTRL_GPU_INITIALIZE_CTX_APERTURE              1:0
#define NV2080_CTRL_GPU_INITIALIZE_CTX_APERTURE_VIDMEM   (0x00000000U)
#define NV2080_CTRL_GPU_INITIALIZE_CTX_APERTURE_COH_SYS  (0x00000001U)
#define NV2080_CTRL_GPU_INITIALIZE_CTX_APERTURE_NCOH_SYS (0x00000002U)

#define NV2080_CTRL_GPU_INITIALIZE_CTX_GPU_CACHEABLE         2:2
#define NV2080_CTRL_GPU_INITIALIZE_CTX_GPU_CACHEABLE_YES (0x00000000U)
#define NV2080_CTRL_GPU_INITIALIZE_CTX_GPU_CACHEABLE_NO  (0x00000001U)

/*
 * NV2080_CTRL_GPU_INITIALIZE_CTX_PRESERVE_CTX - Tells RM Whether this Ctx buffer needs to
 * do a full initialization (Load the golden image). When a context is promoted on a different
 * channel than it was originally inited, the client can use this flag to tell RM
 * that this is an already inited Context. In such cases RM will update the internal state
 * to update the context address and state variables.
 */

#define NV2080_CTRL_GPU_INITIALIZE_CTX_PRESERVE_CTX              3:3
#define NV2080_CTRL_GPU_INITIALIZE_CTX_PRESERVE_CTX_NO   (0x00000000U)
#define NV2080_CTRL_GPU_INITIALIZE_CTX_PRESERVE_CTX_YES  (0x00000001U)

/*
 * NV2080_CTRL_CMD_CPU_QUERY_ECC_INTR
 * Queries the top level ECC PMC PRI register
 * TODO remove these parameters, tracked in bug #1975721
 */
#define NV2080_CTRL_CMD_GPU_QUERY_ECC_INTR               (0x2080012eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0x2E" */

typedef struct NV2080_CTRL_GPU_QUERY_ECC_INTR_PARAMS {
    NvU32 eccIntrStatus;
} NV2080_CTRL_GPU_QUERY_ECC_INTR_PARAMS;

/**
 * NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS
 *
 * This command is used to query the ECC status of a GPU by a subdevice
 * handle.  Please see the NV2080_CTRL_GPU_QUERY_ECC_UNIT_STATUS
 * data structure description below for details on the data reported
 * per hardware unit.
 *
 *   units
 *     Array of structures used to describe per-unit state
 *
 *   flags
 *     See interface flag definitions below.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */



#define NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS                   (0x2080012fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_MESSAGE_ID" */


#define NV2080_CTRL_GPU_ECC_UNIT_COUNT                         (0x0000001FU)



// Deprecated do not use
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_FLAGS_TYPE             0:0
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_FLAGS_TYPE_FILTERED   (0x00000000U)
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_FLAGS_TYPE_RAW        (0x00000001U)

#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_UNC_ERR_FALSE         0U
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_UNC_ERR_TRUE          1U
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_UNC_ERR_INDETERMINATE 2U

/*
 * NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS
 *
 * This structure represents the exception status of a class of per-unit
 * exceptions
 *
 *   count
 *     number of exceptions that have occurred since boot
 */
typedef struct NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS {
    NV_DECLARE_ALIGNED(NvU64 count, 8);
} NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS;

/*
 * NV2080_CTRL_GPU_QUERY_ECC_UNIT_STATUS
 *
 * This structure represents the per-unit ECC exception status
 *
 *   enabled
 *     ECC enabled yes/no for this unit
 *   scrubComplete
 *     Scrub has completed yes/no. A scrub is performed for some units to ensure
 *     the checkbits are consistent with the protected data.
 *   supported
 *     Whether HW supports ECC in this unit for this GPU
 *   dbe
 *     Double bit error (DBE) status. The value returned reflects a counter
 *     that is monotonic, but can be reset by clients.
 *   dbeNonResettable (deprecated do not use)
 *     Double bit error (DBE) status, not client resettable.
 *   sbe
 *     Single bit error (SBE) status. The value returned reflects a counter
 *     that is monotonic, but can be reset by clients.
 *   sbeNonResettable (deprecated do not use)
 *     Single bit error (SBE) status, not client resettable.
 *
 */
typedef struct NV2080_CTRL_GPU_QUERY_ECC_UNIT_STATUS {
    NvBool enabled;
    NvBool scrubComplete;
    NvBool supported;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS dbe, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS dbeNonResettable, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS sbe, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_QUERY_ECC_EXCEPTION_STATUS sbeNonResettable, 8);
} NV2080_CTRL_GPU_QUERY_ECC_UNIT_STATUS;

/*
 * NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS
 *
 * This structure returns ECC exception status and GPU Fatal Poison for all units
 *
 *   units
 *     This structure represents ECC exception status for all Units.
 *   bFatalPoisonError
 *     Whether GPU Fatal poison error occurred in this GPU. This will be set for Ampere_and_later
 *   uncorrectableError
 *     Indicates whether any uncorrectable GR ECC errors have occurred. When
 *     SMC is enabled, uncorrectableError is only valid when the client is
 *     subscribed to a partition. Check QUERY_ECC_STATUS_UNC_ERR_*
 *   flags
 *     Flags passed by caller. Refer  NV2080_CTRL_GPU_QUERY_ECC_STATUS_FLAGS_TYPE_* for details.
 *   grRouteInfo
 *     SMC partition information. This input is only valid when SMC is
 *     enabled on Ampere_and_later.
 *
 */
#define NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_MESSAGE_ID (0x2FU)

typedef struct NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_QUERY_ECC_UNIT_STATUS units[NV2080_CTRL_GPU_ECC_UNIT_COUNT], 8);
    NvBool bFatalPoisonError;
    NvU8   uncorrectableError;
    NvU32  flags;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_COMPUTE_MODE_RULES
 *
 * This command sets the compute mode rules for the associated subdevice.  The
 * default mode is equivalent to NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE.  This
 * command is available to clients with administrator privileges only.  An
 * attempt to use this command by a client without administrator privileged
 * results in the return of an NV_ERR_INSUFFICIENT_PERMISSIONS status.
 *
 *   rules
 *     This parameter is used to specify the rules that govern the GPU with
 *     respect to NV50_COMPUTE objects. Legal values for this parameter include:
 *
 *       NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE
 *         This mode indicate that no special restrictions apply to the
 *         allocation of NV50_COMPUTE objects.
 *
 *       NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE
 *         This mode means that only one instance of NV50_COMPUTE will be
 *         allowed at a time. This restriction is enforced at each subsequent
 *         NV50_COMPUTE allocation attempt. Setting this mode will not affect
 *         any existing compute programs that may be running. For example,
 *         if this mode is set while three compute programs are running, then
 *         all of those programs will be allowed to continue running. However,
 *         until they all finish running, no new NV50_COMPUTE objects may be
 *         allocated. User-mode clients should treat this as restricting access
 *         to a NV50_COMPUTE object to a single thread within a process.
 *
 *       NV2080_CTRL_GPU_COMPUTE_MODE_RULES_COMPUTE_PROHIBITED
 *         This mode means that that GPU is not ever allowed to instantiate an
 *         NV50_COMPUTE object, and thus cannot run any new compute programs.
 *         This restriction is enforced at each subsequent NV50_COMPUTE object
 *         allocation attempt. Setting this mode will not affect any existing
 *         compute programs that may be running. For example, if this mode is
 *         set while three compute programs are running, then all of those
 *         programs will be allowed to continue running. However, no new
 *         NV50_COMPUTE objects may be allocated.
 *
 *
 *       NV2080_CTRL_GPU_COMPUTE_MODE_EXCLUSIVE_COMPUTE_PROCESS
 *         This mode is identical to EXCLUSIVE_COMPUTE, where only one instance
 *         of NV50_COMPUTE will be allowed at a time. It is separate from
 *         EXCLUSIVE_COMPUTE to allow user-mode clients to differentiate
 *         exclusive access to a compute object from a single thread of a
 *         process from exclusive access to a compute object from all threads
 *         of a process. User-mode clients should not limit access to a
 *         NV50_COMPUTE object to a single thread when the GPU is set to
 *         EXCLUSIVE_COMPUTE_PROCESS.
 *
 *     An invalid rules parameter value results in the return of an
 *     NV_ERR_INVALID_ARGUMENT status.
 *
 *   flags
 *     Reserved. Caller should set this field to zero.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT (if an invalid rule number is provided)
 *   NV_ERR_INSUFFICIENT_PERMISSIONS (if the user is not the Administrator or superuser)
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_SET_COMPUTE_MODE_RULES                   (0x20800130U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS_MESSAGE_ID" */

/* valid rules parameter values */
#define NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE                      (0x00000000U)
#define NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE         (0x00000001U)
#define NV2080_CTRL_GPU_COMPUTE_MODE_RULES_COMPUTE_PROHIBITED        (0x00000002U)
#define NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE_PROCESS (0x00000003U)

#define NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS {
    NvU32 rules;
    NvU32 flags;
} NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_QUERY_COMPUTE_MODE_RULES
 *
 * This command queries the compute mode rules for the associated subdevice.
 * Please see the NV2080_CTRL_CMD_GPU_SET_COMPUTE_MODE_RULES command, above, for
 * details as to what the rules mean.
 *
 *   rules
 *     Specifies the rules that govern the GPU, with respect to NV50_COMPUTE
 *     objects.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_QUERY_COMPUTE_MODE_RULES (0x20800131U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS_MESSAGE_ID (0x31U)

typedef struct NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS {
    NvU32 rules;
} NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_QUERY_ECC_CONFIGURATION
 *
 * This command returns the current ECC configuration setting for
 * a GPU given its subdevice handle.  The value returned is
 * the current ECC setting for the GPU stored in non-volatile
 * memory on the board.
 *
 *   currentConfiguration
 *      The current ECC configuration setting.
 *
 *   defaultConfiguration
 *      The factory default ECC configuration setting.
 *
 * Please see the NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS command if
 * you wish to determine if ECC is currently enabled.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_GPU_QUERY_ECC_CONFIGURATION (0x20800133U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_ECC_CONFIGURATION_DISABLED  (0x00000000U)
#define NV2080_CTRL_GPU_ECC_CONFIGURATION_ENABLED   (0x00000001U)

#define NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS_MESSAGE_ID (0x33U)

typedef struct NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS {
    NvU32 currentConfiguration;
    NvU32 defaultConfiguration;
} NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_ECC_CONFIGURATION
 *
 * This command changes the ECC configuration setting for a GPU
 * given its subdevice handle.  The value specified is
 * stored in non-volatile memory on the board and will take
 * effect with the next GPU reset
 *
 *   newConfiguration
 *     The new configuration setting to take effect with
 *     the next GPU reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_SET_ECC_CONFIGURATION (0x20800134U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_ECC_CONFIGURATION_DISABLE (0x00000000U)
#define NV2080_CTRL_GPU_ECC_CONFIGURATION_ENABLE  (0x00000001U)

#define NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS_MESSAGE_ID (0x34U)

typedef struct NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS {
    NvU32 newConfiguration;
} NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_RESET_ECC_ERROR_STATUS
 *
 * This command resets volatile and/or persistent ECC error
 * status information for a GPU given its subdevice
 * handle.
 *
 *   statuses
 *     The ECC error statuses (the current, volatile
 *     and/or the persistent error counter(s)) to
 *     be reset by the command.
 *   flags
 *     FORCE_PURGE
 *          Forcibly clean all the ECC InfoROM state if this flag is set
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_RESET_ECC_ERROR_STATUS                     (0x20800136U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_ECC_ERROR_STATUS_NONE                          (0x00000000U)
#define NV2080_CTRL_GPU_ECC_ERROR_STATUS_VOLATILE                      (0x00000001U)
#define NV2080_CTRL_GPU_ECC_ERROR_STATUS_AGGREGATE                     (0x00000002U)

#define NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_FLAGS_FORCE_PURGE           0:0
#define NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_FLAGS_FORCE_PURGE_FALSE 0U
#define NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_FLAGS_FORCE_PURGE_TRUE  1U

#define NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS_MESSAGE_ID (0x36U)

typedef struct NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS {
    NvU32 statuses;
    NvU8  flags;
} NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_FERMI_GPC_INFO
 *
 * This command returns a mask of enabled GPCs for the associated GPU.
 *
 *    gpcMask
 *      This parameter returns a mask of enabled GPCs. Each GPC has an ID
 *      that's equivalent to the corresponding bit position in the mask.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_GET_FERMI_GPC_INFO (0x20800137U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS_MESSAGE_ID (0x37U)

typedef struct NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS {
    NvU32 gpcMask;
} NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_FERMI_TPC_INFO
 *
 * This command returns a mask of enabled TPCs for a specified GPC.
 *
 *    gpcId
 *      This parameter specifies the GPC for which TPC information is
 *      to be retrieved. If the GPC with this ID is not enabled this command
 *      will return an tpcMask value of zero.
 *
 *    tpcMask
 *      This parameter returns a mask of enabled TPCs for the specified GPC.
 *      Each TPC has an ID that's equivalent to the corresponding bit
 *      position in the mask.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_GET_FERMI_TPC_INFO (0x20800138U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS_MESSAGE_ID (0x38U)

typedef struct NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS {
    NvU32 gpcId;
    NvU32 tpcMask;
} NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_FERMI_ZCULL_INFO
 *
 * This command returns a mask of enabled ZCULLs for a specified GPC.
 *
 *    gpcId
 *      This parameter specifies the GPC for which ZCULL information is to be
 *      retrieved. If the GPC with this ID is not enabled this command will
 *      return an zcullMask value of zero.
 *
 *    zcullMask
 *      This parameter returns a mask of enabled ZCULLs for the specified GPC.
 *      Each ZCULL has an ID that's equivalent to the corresponding bit
 *      position in the mask.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_PARAM_STRUCT
 *
 * Deprecated: Please use GR based control call
 * NV2080_CTRL_CMD_GR_GET_ZCULL_MASK
 *
 */
#define NV2080_CTRL_CMD_GPU_GET_FERMI_ZCULL_INFO (0x20800139U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS {
    NvU32 gpcId;
    NvU32 zcullMask;
} NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_GET_OEM_BOARD_INFO
 *
 * If an InfoROM with a valid OEM Board Object is present, this
 * command returns relevant information from the object to the
 * caller.
 *
 * The following data are currently reported:
 *
 * buildDate
 *   The board's build date (8 digit BCD in format YYYYMMDD).
 *
 * marketingName
 *   The board's marketing name (24 ASCII letters e.g. "Quadro FX5800").
 *
 * boardSerialNumber
 *   The board's serial number.
 *
 * memoryManufacturer
 *   The board's memory manufacturer ('S'amsung/'H'ynix/'I'nfineon).
 *
 * memoryDateCode
 *   The board's memory datecode (LSB justified ASCII field with 0x00
 *   denoting empty space).
 *
 * productPartNumber
 *   The board's 900 product part number (LSB justified ASCII field with 0x00
 *   denoting empty space e.g. "900-21228-0208-200").
 *
 * boardRevision
 *   The board's revision (for e.g. A02, B01)
 *
 * boardType
 *   The board's type ('E'ngineering/'P'roduction)
 *
 * board699PartNumber
 *   The board's 699 product part number (LSB justified ASCII field with 0x00
 *   denoting empty space e.g. "699-21228-0208-200").
 *
 * board965PartNumber
 *   The board's 965 product part number (LSB justified ASCII field with 0x00
 *   denoting empty space e.g. "965-21228-0208-200").
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_OEM_BOARD_INFO    (0x2080013fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_GPU_MAX_MARKETING_NAME_LENGTH      (0x00000018U)
#define NV2080_GPU_MAX_SERIAL_NUMBER_LENGTH       (0x00000010U)
#define NV2080_GPU_MAX_MEMORY_PART_ID_LENGTH      (0x00000014U)
#define NV2080_GPU_MAX_MEMORY_DATE_CODE_LENGTH    (0x00000006U)
#define NV2080_GPU_MAX_PRODUCT_PART_NUMBER_LENGTH (0x00000014U)

#define NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS_MESSAGE_ID (0x3FU)

typedef struct NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS {
    NvU32 buildDate;
    NvU8  marketingName[NV2080_GPU_MAX_MARKETING_NAME_LENGTH];
    NvU8  serialNumber[NV2080_GPU_MAX_SERIAL_NUMBER_LENGTH];
    NvU8  memoryManufacturer;
    NvU8  memoryPartID[NV2080_GPU_MAX_MEMORY_PART_ID_LENGTH];
    NvU8  memoryDateCode[NV2080_GPU_MAX_MEMORY_DATE_CODE_LENGTH];
    NvU8  productPartNumber[NV2080_GPU_MAX_PRODUCT_PART_NUMBER_LENGTH];
    NvU8  boardRevision[3];
    NvU8  boardType;
    NvU8  board699PartNumber[NV2080_GPU_MAX_PRODUCT_PART_NUMBER_LENGTH];
    NvU8  board965PartNumber[NV2080_GPU_MAX_PRODUCT_PART_NUMBER_LENGTH];
} NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_GET_ID
 *
 * This command returns the gpuId of the associated object.
 *
 *   gpuId
 *     This field return the gpuId.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_ID (0x20800142U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ID_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV2080_CTRL_GPU_GET_ID_PARAMS {
    NvU32 gpuId;
} NV2080_CTRL_GPU_GET_ID_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_GPU_DEBUG_MODE
 *
 * This command is used to enable or disable GPU debug mode. While this mode
 * is enabled,  some client RM calls that can potentially timeout return
 * NV_ERR_BUSY_RETRY, signalling the client to try again once GPU
 * debug mode is disabled.
 *
 * mode
 *   This parameter specifies whether GPU debug mode is to be enabled or
 *   disabled. Possible values are:
 *
 *     NV2080_CTRL_GPU_DEBUG_MODE_ENABLED
 *     NV2080_CTRL_GPU_DEBUG_MODE_DISABLED
 *
 * Possible return status values are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV2080_CTRL_CMD_GPU_SET_GPU_DEBUG_MODE (0x20800143U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_GPU_DEBUG_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_GPU_DEBUG_MODE_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV2080_CTRL_GPU_SET_GPU_DEBUG_MODE_PARAMS {
    NvU32 mode;
} NV2080_CTRL_GPU_SET_GPU_DEBUG_MODE_PARAMS;

#define NV2080_CTRL_GPU_DEBUG_MODE_ENABLED     (0x00000001U)
#define NV2080_CTRL_GPU_DEBUG_MODE_DISABLED    (0x00000002U)

/*
 * NV2080_CTRL_CMD_GPU_GET_GPU_DEBUG_MODE
 *
 * This command is used to query whether debug mode is enabled on the current
 * GPU. Please see the description of NV2080_CTRL_CMD_GPU_SET_GPU_DEBUG_MODE
 * for more details on GPU debug mode.
 *
 * currentMode
 *   This parameter returns the state of GPU debug mode for the current GPU.
 *   Possible values are:
 *
 *     NV2080_CTRL_GPU_DEBUG_MODE_ENABLED
 *     NV2080_CTRL_GPU_DEBUG_MODE_DISABLED
 *
 * Possible return status values are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV2080_CTRL_CMD_GPU_GET_GPU_DEBUG_MODE (0x20800144U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_GPU_DEBUG_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_GPU_DEBUG_MODE_PARAMS_MESSAGE_ID (0x44U)

typedef struct NV2080_CTRL_GPU_GET_GPU_DEBUG_MODE_PARAMS {
    NvU32 currentMode;
} NV2080_CTRL_GPU_GET_GPU_DEBUG_MODE_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_PARTNERLIST
 *
 * Returns a list of engines that can partner or coexist
 * when using the target channel or partnership class.
 * This list may include all engines (pre-Kepler), or as few
 * as 1 engine (Kepler and beyond).
 *
 *   engineType
 *     This field specifies the target engine type.
 *     See cl2080.h for a list of valid engines.
 *
 *   partnershipClassId
 *     This field specifies the target channel
 *     or partnership class ID.
 *     An example of such a class is GF100_CHANNEL_GPFIFO.
 *
 *   runqueue
 *     This field is an index which indicates the runqueue to
 *     return the list of supported engines for. This is the
 *     same field as what NVOS04_FLAGS_GROUP_CHANNEL_RUNQUEUE
 *     specifies. This is only valid for TSG.
 *
 *   numPartners;
 *     This field returns the number of
 *     valid entries in the partnersList array
 *
 *   partnerList
 *     This field is an array containing the list of supported
 *     partner engines types, in no particular order, and
 *     may even be empty (numPartners = 0).
 *     See cl2080.h for a list of possible engines.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV2080_CTRL_CMD_GPU_GET_ENGINE_PARTNERLIST           (0x20800147U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS_MESSAGE_ID" */

/* this macro specifies the maximum number of partner entries */
#define NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS                  (0x00000020U)

#define NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS_MESSAGE_ID (0x47U)

typedef struct NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS {
    NvU32 engineType;
    NvU32 partnershipClassId;
    NvU32 runqueue;
    NvU32 numPartners;
    // C form: NvU32 partnerList[NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS];
    NvU32 partnerList[NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS];
} NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_GET_GID_INFO
 *
 * This command returns the GPU ID (GID) string for the associated
 * GPU.  This value can be useful for GPU identification and security
 * system validation.
 *
 * The GPU ID is a SHA-1 based 16 byte ID, formatted as a 32 character
 *      hexadecimal string as "GPU-%08x-%04x-%04x-%04x-%012x" (the
 *      canonical format of a UUID)
 *
 * The GPU IDs are generated using the ECID, PMC_BOOT_0, and
 * PMC_BOOT_42 of the GPU as the hash message.
 *
 *   index
 *     (Input) "Select which GID set to get." Or so the original documentation
 *     said. In reality, there is only one GID per GPU, and the implementation
 *     completely ignores this parameter. You can too.
 *
 *   flags (Input) The _FORMAT* flags designate ascii or binary format. Binary
 *     format returns the raw bytes of either the 16-byte SHA-1 ID or the
 *     32-byte SHA-256 ID.
 *
 *     The _TYPE* flags needs to specify the _SHA1 type.
 *
 *   length
 *     (Output) Actual GID length, in bytes.
 *
 *   data[NV2080_BUS_MAX_GID_LENGTH]
 *     (Output) Result buffer: the GID itself, in a format that is determined by
 *     the "flags" field (described above).
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_GPU_GET_GID_INFO      (0x2080014aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_MESSAGE_ID" */

/* maximum possible number of bytes of GID information returned */
#define NV2080_GPU_MAX_GID_LENGTH             (0x000000100ULL)

/* maximum possible number of bytes of GID information returned if given the BINARY and SHA1 flags */
#define NV2080_GPU_MAX_SHA1_BINARY_GID_LENGTH (0x000000010ULL)

#define NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_MESSAGE_ID (0x4AU)

typedef struct NV2080_CTRL_GPU_GET_GID_INFO_PARAMS {
    NvU32 index;
    NvU32 flags;
    NvU32 length;
    NvU8  data[NV2080_GPU_MAX_GID_LENGTH];
} NV2080_CTRL_GPU_GET_GID_INFO_PARAMS;

/* valid flags values */
#define NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT                  1:0
#define NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT_ASCII  (0x00000000U)
#define NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT_BINARY (0x00000002U)

#define NV2080_GPU_CMD_GPU_GET_GID_FLAGS_TYPE                    2:2
#define NV2080_GPU_CMD_GPU_GET_GID_FLAGS_TYPE_SHA1     (0x00000000U)

/*
 * NV2080_CTRL_CMD_GPU_GET_INFOROM_OBJECT_VERSION
 *
 * This command can be used by clients to retrieve the version of an
 * InfoROM object.
 *
 *   objectType
 *     This parameter specifies the name of the InfoROM object whose version
 *     should be queried.
 *
 *   version
 *     This parameter returns the version of the InfoROM object specified by
 *     the objectType parameter.
 *
 *   subversion
 *     This parameter returns the subversion of the InfoROM object specified
 *     by the objectType parameter.
 *
 * Possible return status values:
 *   NV_OK
 *   NV_ERR_STATE_IN_USE
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_GPU_GET_INFOROM_OBJECT_VERSION (0x2080014bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_INFOROM_OBJ_TYPE_LEN           3U

#define NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS_MESSAGE_ID (0x4BU)

typedef struct NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS {
    char objectType[NV2080_CTRL_GPU_INFOROM_OBJ_TYPE_LEN];
    NvU8 version;
    NvU8 subversion;
} NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS;


/*
 * NV2080_CTRL_CMD_SET_GPU_OPTIMUS_INFO
 *
 * This command will specify that system is Optimus enabled.
 *
 * isOptimusEnabled
 *     Set NV_TRUE if system is Optimus enabled.
 *
 * Possible status return values are:
 *      NV_OK
 */
#define NV2080_CTRL_CMD_SET_GPU_OPTIMUS_INFO (0x2080014cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS_MESSAGE_ID (0x4CU)

typedef struct NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS {
    NvBool isOptimusEnabled;
} NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_IP_VERSION
 *
 * Will return the IP VERSION on the given engine for engines that support
 * this capability.
 *
 *   targetEngine
 *     This parameter specifies the target engine type to query for IP_VERSION.
 *
 *   ipVersion
 *     This parameter returns the IP VERSION read from the unit's IP_VER
 *     register.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_GPU_GET_IP_VERSION (0x2080014dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS_MESSAGE_ID (0x4DU)

typedef struct NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS {
    NvU32 targetEngine;
    NvU32 ipVersion;
} NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS;

#define NV2080_CTRL_GPU_GET_IP_VERSION_DISPLAY     (0x00000001U)
#define NV2080_CTRL_GPU_GET_IP_VERSION_HDACODEC    (0x00000002U)
#define NV2080_CTRL_GPU_GET_IP_VERSION_PMGR        (0x00000003U)
#define NV2080_CTRL_GPU_GET_IP_VERSION_PPWR_PMU    (0x00000004U)
#define NV2080_CTRL_GPU_GET_IP_VERSION_DISP_FALCON (0x00000005U)



/*
 * NV2080_CTRL_CMD_GPU_ID_ILLUM_SUPPORT
 *
 * This command returns an indicator which reports if the specified Illumination control
 * attribute is supported
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_GPU_ILLUM_ATTRIB_LOGO_BRIGHTNESS 0U
#define NV2080_CTRL_GPU_ILLUM_ATTRIB_SLI_BRIGHTNESS  1U
#define NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT      (0x20800153U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS_MESSAGE_ID (0x53U)

typedef struct NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS {
    NvU32  attribute;
    NvBool bSupported;
} NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ID_ILLUM
 *
 * This command returns the current value of the specified Illumination control attribute.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_ILLUM (0x20800154U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ILLUM_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_CMD_GPU_ILLUM_PARAMS {
    NvU32 attribute;
    NvU32 value;
} NV2080_CTRL_CMD_GPU_ILLUM_PARAMS;

#define NV2080_CTRL_GPU_GET_ILLUM_PARAMS_MESSAGE_ID (0x54U)

typedef NV2080_CTRL_CMD_GPU_ILLUM_PARAMS NV2080_CTRL_GPU_GET_ILLUM_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_ID_ILLUM
 *
 * This command sets a new valuefor the specified Illumination control attribute.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_SET_ILLUM (0x20800155U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_ILLUM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_ILLUM_PARAMS_MESSAGE_ID (0x55U)

typedef NV2080_CTRL_CMD_GPU_ILLUM_PARAMS NV2080_CTRL_GPU_SET_ILLUM_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_INFOROM_IMAGE_VERSION
 *
 * This command can be used by clients to retrieve the version of the entire
 * InfoROM image.
 *
 *   version
 *      This parameter returns the version of the InfoROM image as a NULL-
 *      terminated character string of the form "XXXX.XXXX.XX.XX" where each
 *      'X' is an integer character.
 *
 * Possible status return values are:
 *   NVOS_STATUS_SUCCES
 *   NV_ERR_INSUFFICIENT_RESOURCES
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_DATA
 */
#define NV2080_CTRL_CMD_GPU_GET_INFOROM_IMAGE_VERSION (0x20800156U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_INFOROM_IMAGE_VERSION_LEN     16U

#define NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS_MESSAGE_ID (0x56U)

typedef struct NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS {
    NvU8 version[NV2080_CTRL_GPU_INFOROM_IMAGE_VERSION_LEN];
} NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_QUERY_INFOROM_ECC_SUPPORT
 *
 * This command returns whether or not ECC is supported via the InfoROM.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_QUERY_INFOROM_ECC_SUPPORT (0x20800157U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0x57" */

/*
 * NV2080_CTRL_GPU_PHYSICAL_BRIDGE_VERSION
 *
 * This structure contains information about a single physical bridge.
 *
 *   fwVersion
 *     This field specifies Firmware Version of the bridge stored in
 *     bridge EEPROM.
 *   oemVersion
 *     This field specifies Oem Version of the firmware stored in
 *     bridge EEPROM.
 *   siliconRevision
 *     This field contains the silicon revision of the bridge hardware.
 *     It is set by the chip manufacturer.
 *   hwbcResourceType
 *     This field specifies the hardware broadcast resource type.
 *     Value denotes the kind of bridge - PLX or BR04
 *
 */

typedef struct NV2080_CTRL_GPU_PHYSICAL_BRIDGE_VERSION_PARAMS {
    NvU32 fwVersion;
    NvU8  oemVersion;
    NvU8  siliconRevision;
    NvU8  hwbcResourceType;
} NV2080_CTRL_GPU_PHYSICAL_BRIDGE_VERSION_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO
 *
 * This command returns physical bridge information in the system.
 * Information consists of bridgeCount and a list of bridgeId's.
 * The bridge Id's are used by NV2080_CTRL_CMD_GPU_GET_PHYSICAL_BRIDGE_VERSION
 * to get firmware version, oem version and silicon revision info.
 *
 *   bridgeCount
 *     This field specifies the number of physical brides present
 *     in the system.
 *   hPhysicalBridges
 *     This field specifies an array of size NV2080_CTRL_MAX_PHYSICAL_BRIDGE.
 *     In this array, the bridge Id's are stored.
 *   bridgeList
 *     This field specifies an array of size NV2080_CTRL_MAX_PHYSICAL_BRIDGE.
 *     In this array, the bridge version details are stored.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO (0x2080015aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MAX_PHYSICAL_BRIDGE                      (100U)
#define NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS_MESSAGE_ID (0x5AU)

typedef struct NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS {
    NvU8                                           bridgeCount;
    NvHandle                                       hPhysicalBridges[NV2080_CTRL_MAX_PHYSICAL_BRIDGE];
    NV2080_CTRL_GPU_PHYSICAL_BRIDGE_VERSION_PARAMS bridgeList[NV2080_CTRL_MAX_PHYSICAL_BRIDGE];
} NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS;

/*
 * NV2080_CTRL_GPU_BRIDGE_VERSION
 *
 * This structure contains information about a single physical bridge.
 *
 *   bus
 *     This field specifies the bus id of the bridge.
 *   device
 *     This field specifies the device id of the bridge.
 *   func
 *     This field specifies the function id of the bridge.
 *   oemVersion
 *     This field specifies Oem Version of the firmware stored in
 *     bridge EEPROM.
 *   siliconRevision
 *     This field contains the silicon revision of the bridge hardware.
 *     It is set by the chip manufacturer.
 *   hwbcResourceType
 *     This field specifies the hardware broadcast resource type.
 *     Value denotes the kind of bridge - PLX or BR04
 *   domain
 *     This field specifies the respective domain of the PCI device.
 *   fwVersion
 *     This field specifies Firmware Version of the bridge stored in
 *     bridge EEPROM.
 *
 *   If (fwVersion, oemVersion, siliconRevision) == 0, it would mean that RM
 *   was unable to fetch the value from the bridge device.
 *
 */

typedef struct NV2080_CTRL_GPU_BRIDGE_VERSION_PARAMS {
    NvU8  bus;
    NvU8  device;
    NvU8  func;
    NvU8  oemVersion;
    NvU8  siliconRevision;
    NvU8  hwbcResourceType;
    NvU32 domain;
    NvU32 fwVersion;
} NV2080_CTRL_GPU_BRIDGE_VERSION_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU
 *
 * This command returns information about all the upstream bridges of the GPU.
 * Information consists of bridge firmware version and its bus topology.
 *
 *   bridgeCount
 *     This field specifies the number of physical brides present
 *     in the system.
 *   physicalBridgeIds
 *     This field specifies an array of size NV2080_CTRL_MAX_PHYSICAL_BRIDGE.
 *     In this array, the bridge Ids are stored.
 *   bridgeList
 *     This field specifies an array of size NV2080_CTRL_MAX_PHYSICAL_BRIDGE.
 *     In this array, the bridge version details are stored.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU (0x2080015bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS_MESSAGE_ID (0x5BU)

typedef struct NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS {
    NvU8                                  bridgeCount;
    NvU32                                 physicalBridgeIds[NV2080_CTRL_MAX_PHYSICAL_BRIDGE];
    NV2080_CTRL_GPU_BRIDGE_VERSION_PARAMS bridgeList[NV2080_CTRL_MAX_PHYSICAL_BRIDGE];
} NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_QUERY_SCRUBBER_STATUS
 *
 * This command is used to query the status of the HW scrubber. If a scrub is
 * in progress then the range which is being scrubbed is also reported back.
 *
 *   scrubberStatus
 *     Reports the status of the scrubber unit - running/idle.
 *
 *   remainingtimeMs
 *     If scrubbing is going on, reports the remaining time in milliseconds
 *     required to finish the scrub.
 *
 *   scrubStartAddr
 *     This parameter reports the start address of the ongoing scrub if scrub
 *     is going on, otherwise reports the start addr of the last finished scrub
 *
 *   scrubEndAddr
 *     This parameter reports the end address of the ongoing scrub if scrub
 *     is going on, otherwise reports the end addr of the last finished scrub.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_GPU_QUERY_SCRUBBER_STATUS (0x2080015fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS_MESSAGE_ID (0x5FU)

typedef struct NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS {
    NvU32 scrubberStatus;
    NvU32 remainingTimeMs;
    NV_DECLARE_ALIGNED(NvU64 scrubStartAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 scrubEndAddr, 8);
} NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS;

/* valid values for scrubber status */
#define NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_SCRUBBER_RUNNING (0x00000000U)
#define NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_SCRUBBER_IDLE    (0x00000001U)

/*
 * NV2080_CTRL_CMD_GPU_GET_VPR_CAPS
 *
 * This command is used to query the VPR capability information for a
 * GPU. If VPR is supported, the parameters are filled accordingly.
 * The addresses returned are all physical addresses.
 *
 *   minStartAddr
 *     Returns the minimum start address that can be possible for VPR.
 *
 *   maxEndAddr
 *     Returns the maximum end   address that can be possible for VPR.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_GPU_GET_VPR_CAPS                       (0x20800160U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 minStartAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 maxEndAddr, 8);
} NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS;



/*
 * NV2080_CTRL_CMD_GPU_HANDLE_GPU_SR
 *
 * Communicates to RM to handle GPU Surprise Removal
 * Called from client when it receives SR IRP from OS
 * Possible status values returned are:
 *   NVOS_STATUS_SUCCESS
 */
#define NV2080_CTRL_CMD_GPU_HANDLE_GPU_SR                      (0x20800167U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0x67" */


/*
 * NV2080_CTRL_CMD_GPU_GET_PES_INFO
 *
 * This command provides the PES count and mask of enabled PES for a
 * specified GPC. It also returns the TPC to PES mapping information
 * for a given GPU.
 *
 *   gpcId[IN]
 *     This parameter specifies the GPC for which PES information is to be
 *     retrieved. If the GPC with this ID is not enabled this command will
 *     return an activePesMask of zero
 *
 *   numPesInGpc[OUT]
 *     This parameter returns the number of PES in this GPC.
 *
 *   activePesMask[OUT]
 *     This parameter returns a mask of enabled PESs for the specified GPC.
 *     Each PES has an ID that is equivalent to the corresponding bit position
 *     in the mask.
 *
 *   maxTpcPerGpcCount[OUT]
 *     This parameter returns the max number of TPCs in a GPC.
 *
 *   tpcToPesMap[OUT]
 *     This array stores the TPC to PES mappings. The value at tpcToPesMap[tpcIndex]
 *     is the index of the PES it belongs to.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_PES_INFO                       (0x20800168U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PES_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GPU_GET_PES_INFO_MAX_TPC_PER_GPC_COUNT 10U

#define NV2080_CTRL_GPU_GET_PES_INFO_PARAMS_MESSAGE_ID (0x68U)

typedef struct NV2080_CTRL_GPU_GET_PES_INFO_PARAMS {
    NvU32 gpcId;
    NvU32 numPesInGpc;
    NvU32 activePesMask;
    NvU32 maxTpcPerGpcCount;
    NvU32 tpcToPesMap[NV2080_CTRL_CMD_GPU_GET_PES_INFO_MAX_TPC_PER_GPC_COUNT];
} NV2080_CTRL_GPU_GET_PES_INFO_PARAMS;

/* NV2080_CTRL_CMD_GPU_GET_OEM_INFO
 *
 * If an InfoROM with a valid OEM Object is present, this
 * command returns relevant information from the object to the
 * caller.
 *
 * oemInfo
 *  This array stores information specifically for OEM use
 *  (e.g. "their own serial number", "lot codes", etc)
 *  "The byte definition is up to the OEM"
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_GPU_GET_OEM_INFO (0x20800169U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_GPU_MAX_OEM_INFO_LENGTH   (0x000001F8U)

#define NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS_MESSAGE_ID (0x69U)

typedef struct NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS {
    NvU8 oemInfo[NV2080_GPU_MAX_OEM_INFO_LENGTH];
} NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_VPR_INFO
 *
 * This command is used to query the VPR information for a GPU.
 * The following VPR related information can be queried by selecting the queryType:
 *   1. The current VPR range.
 *   2. The max VPR range ever possible on this GPU.
 *
 *   queryType [in]
 *     This input parameter is used to select the type of information to query.
 *     Possible values for this parameter are:
 *       1. NV2080_CTRL_GPU_GET_VPR_INFO_QUERY_VPR_CAPS: Use this to query the
 *              max VPR range ever possible on this GPU.
 *       2. NV2080_CTRL_GPU_GET_VPR_INFO_QUERY_CUR_VPR_RANGE: Use this to query
 *              the current VPR range on this GPU.
 *
 *   bVprEnabled [out]
 *     For query type "NV2080_CTRL_GPU_GET_VPR_INFO_CUR_RANGE", this parameter
 *     returns if VPR is currently enabled or not.
 *
 *   vprStartAddress [out]
 *     For NV2080_CTRL_GPU_GET_VPR_INFO_CAPS, it returns minimum allowed VPR start address.
 *     For NV2080_CTRL_GPU_GET_VPR_INFO_RANGE, it returns current VPR start address.
 *
 *   vprEndAddress [out]
 *     For NV2080_CTRL_GPU_GET_VPR_INFO_CAPS, it returns maximum allowed VPR end address.
 *     For NV2080_CTRL_GPU_GET_VPR_INFO_RANGE, it returns current VPR end address.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_VPR_INFO (0x2080016bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS_MESSAGE_ID" */


typedef enum NV2080_CTRL_VPR_INFO_QUERY_TYPE {
    NV2080_CTRL_GPU_GET_VPR_INFO_QUERY_VPR_CAPS = 0,
    NV2080_CTRL_GPU_GET_VPR_INFO_QUERY_CUR_VPR_RANGE = 1,
} NV2080_CTRL_VPR_INFO_QUERY_TYPE;

#define NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS_MESSAGE_ID (0x6BU)

typedef struct NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS {
    NV2080_CTRL_VPR_INFO_QUERY_TYPE queryType;
    NvBool                          bIsVprEnabled;
    NV_DECLARE_ALIGNED(NvU64 vprStartAddressInBytes, 8);
    NV_DECLARE_ALIGNED(NvU64 vprEndAddressInBytes, 8);
} NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ENCODER_CAPACITY
 *
 * This command is used to query the encoder capacity of the GPU.
 *
 *   queryType [in]
 *     This input parameter is used to select the type of information to query.
 *     Possible values for this parameter are:
 *       1. NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_H264: Use this to query the
 *              H.264 encoding capacity on this GPU.
 *       2. NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_HEVC: Use this to query the
 *              H.265/HEVC encoding capacity on this GPU.
 *       3. NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_AV1: Use this to query the
 *              AV1 encoding capacity on this GPU.
 *
 *   encoderCapacity [out]
 *     Encoder capacity value from 0 to 100. Value of 0x00 indicates encoder performance
 *     may be minimal for this GPU and software should fall back to CPU-based encode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_GPU_GET_ENCODER_CAPACITY (0x2080016cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_ENCODER_CAPACITY_QUERY_TYPE {
    NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_H264 = 0,
    NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_HEVC = 1,
    NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_AV1 = 2,
} NV2080_CTRL_ENCODER_CAPACITY_QUERY_TYPE;

#define NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS_MESSAGE_ID (0x6CU)

typedef struct NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS {
    NV2080_CTRL_ENCODER_CAPACITY_QUERY_TYPE queryType;
    NvU32                                   encoderCapacity;
} NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS;

/*
 * NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS
 *
 * This command is used to retrieve the GPU's count of encoder sessions,
 * trailing average FPS and encode latency over all active sessions.
 *
 *   encoderSessionCount
 *     This field specifies count of all active encoder sessions on this GPU.
 *
 *   averageEncodeFps
 *     This field specifies the average encode FPS for this GPU.
 *
 *   averageEncodeLatency
 *     This field specifies the average encode latency in microseconds for this GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS (0x2080016dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS_MESSAGE_ID (0x6DU)

typedef struct NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS {
    NvU32 encoderSessionCount;
    NvU32 averageEncodeFps;
    NvU32 averageEncodeLatency;
} NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS;

#define NV2080_CTRL_GPU_NVENC_SESSION_INFO_MAX_COPYOUT_ENTRIES 0x200U  // 512 entries.

/*
 * NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO
 *
 * This command returns NVENC software sessions information for the associate GPU.
 * Request to retrieve session information use a list of one or more
 * NV2080_CTRL_NVENC_SW_SESSION_INFO structures.
 *
 *   sessionInfoTblEntry
 *     This field specifies the number of entries on the that are filled inside
 *     sessionInfoTbl. Max value of this field once returned from RM would be
 *     NV2080_CTRL_GPU_NVENC_SESSION_INFO_MAX_COPYOUT_ENTRIES,
 *
 *   sessionInfoTbl
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the NVENC session information is to be returned.
 *     When buffer is NULL, RM assume that client is querying sessions count value
 *     and return the current encoder session counts in sessionInfoTblEntry field.
 *     To get actual buffer data, client should allocate sessionInfoTbl of size
 *     NV2080_CTRL_GPU_NVENC_SESSION_INFO_MAX_COPYOUT_ENTRIES  multiplied by the
 *     size of the NV2080_CTRL_NVENC_SW_SESSION_INFO structure. RM will fill the
 *     current session data in sessionInfoTbl buffer and then update the
 *     sessionInfoTblEntry to reflect current session count value.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NO_MEMORY
 *   NV_ERR_INVALID_LOCK_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */

typedef struct NV2080_CTRL_NVENC_SW_SESSION_INFO {
    NvU32 processId;
    NvU32 subProcessId;
    NvU32 sessionId;
    NvU32 codecType;
    NvU32 hResolution;
    NvU32 vResolution;
    NvU32 averageEncodeFps;
    NvU32 averageEncodeLatency;
} NV2080_CTRL_NVENC_SW_SESSION_INFO;

#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS_MESSAGE_ID (0x6EU)

typedef struct NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS {
    NvU32 sessionInfoTblEntry;
    NV_DECLARE_ALIGNED(NvP64 sessionInfoTbl, 8);
} NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS;

#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO (0x2080016eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_GPU_SET_FABRIC_BASE_ADDR
 *
 * The command sets fabric base address which represents top N bits of a
 * peer memory address. These N bits will be used to index NvSwitch routing
 * tables to forward peer memory accesses to associated GPUs.
 *
 * The command is available to clients with administrator privileges only.
 * An attempt to use this command by a client without administrator privileged
 * results in the return of NV_ERR_INSUFFICIENT_PERMISSIONS status.
 *
 * The command allows to set fabricAddr once in a lifetime of a GPU. A GPU must
 * be destroyed in order to re-assign a different fabricAddr. An attempt to
 * re-assign address without destroying a GPU would result in the return of
 * NV_ERR_STATE_IN_USE status.
 *
 *   fabricBaseAddr[IN]
 *      - An address with at least 32GB alignment.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_STATE_IN_USE
 */

#define NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS_MESSAGE_ID (0x6FU)

typedef struct NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 fabricBaseAddr, 8);
} NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS;

#define NV2080_CTRL_CMD_GPU_SET_FABRIC_BASE_ADDR (0x2080016fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_GPU_VIRTUAL_INTERRUPT
 *
 * The command will trigger the specified interrupt on the host from a guest.
 *
 *   handle[IN]
 *      - An opaque handle that will be passed in along with the interrupt
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_GPU_VIRTUAL_INTERRUPT_PARAMS_MESSAGE_ID (0x72U)

typedef struct NV2080_CTRL_GPU_VIRTUAL_INTERRUPT_PARAMS {
    NvU32 handle;
} NV2080_CTRL_GPU_VIRTUAL_INTERRUPT_PARAMS;

#define NV2080_CTRL_CMD_GPU_VIRTUAL_INTERRUPT                                      (0x20800172U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_VIRTUAL_INTERRUPT_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS
 *
 * This control call is to query the status of gpu function registers
 *
 *    statusMask[IN]
 *        - Input mask of required status registers
 *    xusbData[OUT]
 *        - data from querying XUSB status register
 *    ppcData[OUT]
 *        - data from querying PPC status register
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */



#define NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS_MESSAGE_ID (0x73U)

typedef struct NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS {
    NvU32 statusMask;
    NvU32 xusbData;
    NvU32 ppcData;
} NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS;

#define NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS (0x20800173U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_GPU_PARTITION_SPAN
 *
 * This struct represents the span of a memory partition, which represents the
 * slices a given partition occupies (or may occupy) within a fixed range which
 * is defined per-chip. A partition containing more resources will cover more
 * GPU slices and therefore cover a larger span.
 *
 *   lo
 *      - The starting unit of this span, inclusive
 *
 *   hi
 *      - The ending unit of this span, inclusive
 *
 */
typedef struct NV2080_CTRL_GPU_PARTITION_SPAN {
    NV_DECLARE_ALIGNED(NvU64 lo, 8);
    NV_DECLARE_ALIGNED(NvU64 hi, 8);
} NV2080_CTRL_GPU_PARTITION_SPAN;

#define NV_GI_UUID_LEN 16U

/*
 * NV2080_CTRL_GPU_SET_PARTITION_INFO
 *
 * This command partitions a GPU into different SMC-Memory partitions.
 * The command will configure HW partition table to create work and memory
 * isolation.
 *
 * The command is available to clients with administrator privileges only.
 * An attempt to use this command by a client without administrator privileged
 * results in the return of NV_ERR_INSUFFICIENT_PERMISSIONS status.
 *
 * The command allows partitioning an invalid partition only. An attempt to
 * re-partition a valid partition will resule in NV_ERR_STATE_IN_USE.
 * Repartitioning can be done only if a partition has been destroyed/invalidated
 * before re-partitioning.
 *
 *   swizzId[IN/OUT]
 *      - PartitionID associated with a newly created partition. Input in case
 *        of partition invalidation.
 *
 *   uuid[OUT]
 *      - Uuid of a newly created partition.
 *
 *   partitionFlag[IN]
 *      - Flags to determine if GPU is requested to be partitioned in FULL,
 *        HALF, QUARTER or ONE_EIGHTHED and whether the partition requires
 *        any additional resources.
 *        When flags include NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA
 *        partition will be created with at least one video decode, jpeg and
 *        optical flow engines. This flag is valid only for partitions with
 *        a single GPC.
 *
 *   bValid[IN]
 *      - NV_TRUE if creating a partition. NV_FALSE if destroying a partition.
 *
 *   placement[IN]
 *      - Optional placement span to allocate the partition into. Valid
 *        placements are returned from NV2080_CTRL_CMD_GPU_GET_PARTITION_CAPACITY.
 *        The partition flag NV2080_CTRL_GPU_PARTITION_FLAG_PLACE_AT_SPAN must
 *        be set for this parameter to be used. If the flag is set and the given
 *        placement is not valid, an error will be returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_STATE_IN_USE
 */
typedef struct NV2080_CTRL_GPU_SET_PARTITION_INFO {
    NvU32  swizzId;
    NvU8   uuid[NV_GI_UUID_LEN];
    NvU32  partitionFlag;
    NvBool bValid;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PARTITION_SPAN placement, 8);
} NV2080_CTRL_GPU_SET_PARTITION_INFO;

#define PARTITIONID_INVALID                                      NV2080_CTRL_GPU_PARTITION_ID_INVALID
#define NV2080_CTRL_GPU_PARTITION_ID_INVALID                     0xFFFFFFFFU
#define NV2080_CTRL_GPU_MAX_PARTITIONS                           0x00000008U
#define NV2080_CTRL_GPU_MAX_PARTITION_IDS                        0x00000009U
#define NV2080_CTRL_GPU_MAX_SMC_IDS                              0x00000008U
#define NV2080_CTRL_GPU_MAX_GPC_PER_SMC                          0x0000000cU
#define NV2080_CTRL_GPU_MAX_CE_PER_SMC                           0x00000008U

#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE              1:0
#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL          0x00000000U
#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF          0x00000001U
#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER       0x00000002U
#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH        0x00000003U
#define NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE__SIZE         4U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE             4:2
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL         0x00000000U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF         0x00000001U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF    0x00000002U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER      0x00000003U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER 0x00000004U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH       0x00000005U
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE        6U


#define NV2080_CTRL_GPU_PARTITION_MAX_TYPES                      20U
#define NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA          30:30
#define NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA_DISABLE   0U
#define NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA_ENABLE    1U
#define NV2080_CTRL_GPU_PARTITION_FLAG_PLACE_AT_SPAN            31:31
#define NV2080_CTRL_GPU_PARTITION_FLAG_PLACE_AT_SPAN_DISABLE     0U
#define NV2080_CTRL_GPU_PARTITION_FLAG_PLACE_AT_SPAN_ENABLE      1U

// TODO XXX Bug 2657907 Remove these once clients update
#define NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                 (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _FULL)    | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _FULL))
#define NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU             (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _HALF)    | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _HALF))
#define NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU        (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _HALF)    | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _MINI_HALF))
#define NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU          (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _QUARTER) | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _QUARTER))
#define NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU     (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _QUARTER) | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _MINI_QUARTER))
#define NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU         (DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _MEMORY_SIZE, _EIGHTH)  | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _COMPUTE_SIZE, _EIGHTH))

#define NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS {
    NvU32 partitionCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_SET_PARTITION_INFO partitionInfo[NV2080_CTRL_GPU_MAX_PARTITIONS], 8);
} NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS;

#define NV2080_CTRL_CMD_GPU_SET_PARTITIONS (0x20800174U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_GPU_GET_PARTITION_INFO
 *
 * This command gets the partition information for requested partitions.
 * If GPU is not partitioned, the control call will return NV_ERR_NOT_SUPPORTED.
 *
 * The command will can return global partition information as well as single
 * partition information if global flag is not set.
 * In bare-metal user-mode can request all partition info while in virtualization
 * plugin should make an RPC with swizzId which is assigned to the requesting
 * VM.
 *
 *   swizzId[IN]
 *      - HW Partition ID associated with the requested partition.
 *
 *   partitionFlag[OUT]
 *      - partitionFlag that was provided during partition creation.
 *
 *   grEngCount[OUT]
 *      - Number of SMC engines/GR engines allocated in partition
 *        GrIDs in a partition will always start from 0 and end at grEngCount-1
 *
 *   veidCount[OUT]
 *      - VEID Count assigned to a partition. These will be divided across
 *        SMC engines once CONFIGURE_PARTITION call has been made. The current
 *        algorithm is to assign veidPerGpc * gpcCountPerSmc to a SMC engine.
 *
 *   smCount[OUT]
 *      - SMs assigned to a partition.
 *
 *   ceCount[OUT]
 *      - Copy Engines assigned to a partition.
 *
 *   nvEncCount[OUT]
 *      - NvEnc Engines assigned to a partition.
 *
 *   nvDecCount[OUT]
 *      - NvDec Engines assigned to a partition.
 *
 *   nvJpgCount[OUT]
 *      - NvJpg Engines assigned to a partition.
 *
 *   gpcCount[OUT]
 *      - Max GPCs assigned to a partition, including the GfxCapable ones.
 *
 *   virtualGpcCount[OUT]
 *      - Virtualized GPC count assigned to partition
 *
 *   gfxGpcCount[OUT]
 *      - Max GFX GPCs assigned to a partition. This is a subset of the GPCs incuded in gpcCount.
 *
 *   gpcsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS][OUT]
 *      - GPC count associated with every valid SMC/Gr, including the GPCs capable of GFX
 *
 *   virtualGpcsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS][OUT]
 *      - Virtualized GPC count associated with every valid SMC/Gr, including the GPCs capable of GFX
 *
 *   gfxGpcsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS][OUT]
 *      - GFX GPC count associated with every valid SMC/Gr. This is a subset of the GPCs included in gfxGpcCount
 *
 *   veidsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS][OUT]
 *      - VEID count associated with every valid SMC. VEIDs within this SMC
 *        will start from 0 and go till veidCount[SMC_ID] - 1.
 *
 *   span[OUT]
 *      - The span covered by this partition
 *
 *   bValid[OUT]
 *      - NV_TRUE if partition is valid else NV_FALSE.
 *
 *   bPartitionError[OUT]
 *      - NV_TRUE if partition had poison error which requires drain and reset
 *        else NV_FALSE.
 *
 *   validCTSIdMask[OUT]
 *      - Mask of CTS IDs usable by this partition, not reflecting current allocations
 *
 *   validGfxCTSIdMask[OUT]
 *      - Mask of CTS IDs that contain Gfx capable Grs usable by this partition, not reflecting current allocations
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_NOT_SUPPORTED
 */
typedef struct NV2080_CTRL_GPU_GET_PARTITION_INFO {
    NvU32  swizzId;
    NvU32  partitionFlag;
    NvU32  grEngCount;
    NvU32  veidCount;
    NvU32  smCount;
    NvU32  ceCount;
    NvU32  nvEncCount;
    NvU32  nvDecCount;
    NvU32  nvJpgCount;
    NvU32  nvOfaCount;
    NvU32  gpcCount;
    NvU32  virtualGpcCount;
    NvU32  gfxGpcCount;
    NvU32  gpcsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS];
    NvU32  virtualGpcsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS];
    NvU32  gfxGpcPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS];
    NvU32  veidsPerGr[NV2080_CTRL_GPU_MAX_SMC_IDS];
    NV_DECLARE_ALIGNED(NvU64 memSize, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PARTITION_SPAN span, 8);
    NvBool bValid;
    NvBool bPartitionError;
    NV_DECLARE_ALIGNED(NvU64 validCTSIdMask, 8);
    NV_DECLARE_ALIGNED(NvU64 validGfxCTSIdMask, 8);
} NV2080_CTRL_GPU_GET_PARTITION_INFO;

/*
 * NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS
 *
 *   queryPartitionInfo[IN]
 *      - Max sized array of NV2080_CTRL_GPU_GET_PARTITION_INFO to get partition
 *       Info
 *
 *   bGetAllPartitionInfo[In]
 *      - Flag to get all partitions info. Only root client will receive all
 *        partition's info. Non-Root clients should not use this flag
 *
 *   validPartitionCount[Out]
 *      - Valid partition count which has been filled by RM as part of the call
 *
 */
#define NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS_MESSAGE_ID (0x75U)

typedef struct NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_GET_PARTITION_INFO queryPartitionInfo[NV2080_CTRL_GPU_MAX_PARTITIONS], 8);
    NvU32  validPartitionCount;
    NvBool bGetAllPartitionInfo;
} NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_PARTITIONS      (0x20800175U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_GPU_CONFIGURE_PARTITION
 *
 * This command configures a partition by associating GPCs with SMC Engines
 * available in that partition. Engines which are to have GPCs assigned to them
 * shall not already have any GPCs assigned to them. It is not valid to both
 * assign GPCs and remove GPCs as part of a single call to this function.
 *
 *   swizzId[IN]
 *      - PartitionID for configuring partition. If partition has a valid
 *        context created, then configuration is not allowed.
 *
 *   gpcCountPerSmcEng[IN]
 *      - Number of GPCs expected to be configured per SMC. Supported
 *        configurations are 0, 1, 2, 4 or 8. "0" means a particular SMC
 *        engine will be disabled with no GPC connected to it.
 *
 *   updateSmcEngMask[IN]
 *      - Mask tracking valid entries of gpcCountPerSmcEng. A value of
 *        0 in bit index i indicates that engine i will keep its current
 *        configuration.
 *
 *   bUseAllGPCs[IN]
 *      - Flag specifying alternate configuration mode, indicating that in
 *        swizzid 0 only, all non-floorswept GPCs should be connected to the
 *        engine indicated by a raised bit in updateSmcEngMask. Only a single
 *        engine may be targeted by this operation. The gpcCountPerSmcEng
 *        parameter should not be used with this flag.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INSUFFICIENT_RESOURCES
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_STATE_IN_USE
 */
#define NV2080_CTRL_CMD_GPU_CONFIGURE_PARTITION (0x20800176U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_CONFIGURE_PARTITION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_CONFIGURE_PARTITION_PARAMS_MESSAGE_ID (0x76U)

typedef struct NV2080_CTRL_GPU_CONFIGURE_PARTITION_PARAMS {
    NvU32  swizzId;
    NvU32  gpcCountPerSmcEng[NV2080_CTRL_GPU_MAX_SMC_IDS];
    NvU32  updateSmcEngMask;
    NvBool bUseAllGPCs;
} NV2080_CTRL_GPU_CONFIGURE_PARTITION_PARAMS;


/*
 * NV2080_CTRL_GPU_FAULT_PACKET
 *
 * This struct represents a GMMU fault packet.
 *
 */
#define NV2080_CTRL_GPU_FAULT_PACKET_SIZE 32U
typedef struct NV2080_CTRL_GPU_FAULT_PACKET {
    NvU8 data[NV2080_CTRL_GPU_FAULT_PACKET_SIZE];
} NV2080_CTRL_GPU_FAULT_PACKET;

/*
 * NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT
 *
 * This command reports a nonreplayable fault packet to RM.
 * It is only used by UVM.
 *
 *   pFaultPacket[IN]
 *      - A fault packet that will be later cast to GMMU_FAULT_PACKET *.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GPU_REPORT_NON_REPLAYABLE_FAULT (0x20800177U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS_MESSAGE_ID (0x77U)

typedef struct NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS {
    NV2080_CTRL_GPU_FAULT_PACKET faultPacket;
} NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS;

/*
 *  NV2080_CTRL_CMD_GPU_EXEC_REG_OPS_VGPU
 *
 *  This command is similar to NV2080_CTRL_CMD_GPU_EXEC_REG_OPS, except it is used
 *  by the VGPU plugin client only. This command provides access to the subset of
 *  privileged registers.
 *
 *  See confluence page "vGPU UMED Security" for details.
 *
 */
#define NV2080_CTRL_CMD_GPU_EXEC_REG_OPS_VGPU (0x20800178U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_EXEC_REG_OPS_VGPU_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_EXEC_REG_OPS_VGPU_PARAMS_MESSAGE_ID (0x78U)

typedef NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS NV2080_CTRL_GPU_EXEC_REG_OPS_VGPU_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_RUNLIST_PRI_BASE
 *
 * This command returns the runlist pri base of the specified engine(s).
 *
 *   engineList
 *     Input array.
 *     This array specifies the engines being queried for information.
 *     The list of engines supported by a chip can be fetched using the
 *     NV2080_CTRL_CMD_GPU_GET_ENGINES/GET_ENGINES_V2 ctrl call.
 *
 *   runlistPriBase
 *     Output array.
 *     Returns the runlist pri base for the specified engines
 *     Else, will return _NULL when the input is a NV2080_ENGINE_TYPE_NULL
 *     and will return _ERROR when the control call fails due to an invalid argument
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_ENGINE_RUNLIST_PRI_BASE (0x20800179U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS_MESSAGE_ID (0x79U)

typedef struct NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS {
    NvU32 engineList[NV2080_GPU_MAX_ENGINES_LIST_SIZE];
    NvU32 runlistPriBase[NV2080_GPU_MAX_ENGINES_LIST_SIZE];
} NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS;

#define NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_NULL  (0xFFFFFFFFU)
#define NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_ERROR (0xFFFFFFFBU)

/*
 * NV2080_CTRL_CMD_GPU_GET_HW_ENGINE_ID
 *
 * This command returns the host hardware defined engine ID of the specified engine(s).
 *
 *   engineList
 *     Input array.
 *     This array specifies the engines being queried for information.
 *     The list of engines supported by a chip can be fetched using the
 *     NV2080_CTRL_CMD_GPU_GET_ENGINES/GET_ENGINES_V2 ctrl call.
 *
 *   hwEngineID
 *     Output array.
 *     Returns the host hardware engine ID(s) for the specified engines
 *     Else, will return _NULL when the input is a NV2080_ENGINE_TYPE_NULL
 *     and will return _ERROR when the control call fails due to an invalid argument
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_HW_ENGINE_ID              (0x2080017aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS_MESSAGE_ID (0x7AU)

typedef struct NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS {
    NvU32 engineList[NV2080_GPU_MAX_ENGINES_LIST_SIZE];
    NvU32 hwEngineID[NV2080_GPU_MAX_ENGINES_LIST_SIZE];
} NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS;

#define NV2080_CTRL_GPU_GET_HW_ENGINE_ID_NULL      (0xFFFFFFFFU)
#define NV2080_CTRL_GPU_GET_HW_ENGINE_ID_ERROR     (0xFFFFFFFBU)

/*
 * NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS
 *
 * This command is used to retrieve the GPU's count of FBC sessions,
 * average FBC calls and FBC latency over all active sessions.
 *
 *   sessionCount
 *     This field specifies count of all active fbc sessions on this GPU.
 *
 *   averageFPS
 *     This field specifies the average frames captured.
 *
 *   averageLatency
 *     This field specifies the average FBC latency in microseconds.
 *
 * Possible status values returned are :
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
*/
#define NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS (0x2080017bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS_MESSAGE_ID (0x7BU)

typedef struct NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS {
    NvU32 sessionCount;
    NvU32 averageFPS;
    NvU32 averageLatency;
} NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS;

/*
* NV2080_CTRL_NVFBC_SW_SESSION_INFO
*
*   processId[OUT]
*           Process id of the process owning the NvFBC session.
*           On VGX host, this will specify the vGPU plugin process id.
*   subProcessId[OUT]
*           Process id of the process owning the NvFBC session if the
*           session is on VGX guest, else the value is zero.
*   vgpuInstanceId[OUT]
*           vGPU on which the process owning the NvFBC session
*           is running if session is on VGX guest, else
*           the value is zero.
*   sessionId[OUT]
*           Unique session id of the NvFBC session.
*   sessionType[OUT]
*           Type of NvFBC session.
*   displayOrdinal[OUT]
*           Display identifier associated with the NvFBC session.
*   sessionFlags[OUT]
*           One or more of NV2080_CTRL_NVFBC_SESSION_FLAG_xxx.
*   hMaxResolution[OUT]
*           Max horizontal resolution supported by the NvFBC session.
*   vMaxResolution[OUT]
*           Max vertical resolution supported by the NvFBC session.
*   hResolution[OUT]
*           Horizontal resolution requested by caller in grab call.
*   vResolution[OUT]
*           Vertical resolution requested by caller in grab call.
*   averageFPS[OUT]
*           Average no. of frames captured per second.
*   averageLatency[OUT]
*           Average frame capture latency in microseconds.
*/

#define NV2080_CTRL_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED            0x00000001U
#define NV2080_CTRL_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED  0x00000002U
#define NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_NO_WAIT  0x00000004U
#define NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_INFINITE 0x00000008U
#define NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_TIMEOUT  0x00000010U

typedef struct NV2080_CTRL_NVFBC_SW_SESSION_INFO {
    NvU32 processId;
    NvU32 subProcessId;
    NvU32 vgpuInstanceId;
    NvU32 sessionId;
    NvU32 sessionType;
    NvU32 displayOrdinal;
    NvU32 sessionFlags;
    NvU32 hMaxResolution;
    NvU32 vMaxResolution;
    NvU32 hResolution;
    NvU32 vResolution;
    NvU32 averageFPS;
    NvU32 averageLatency;
} NV2080_CTRL_NVFBC_SW_SESSION_INFO;

/*
* NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO
*
* This command returns NVFBC software sessions information for the associate GPU.
*
*   sessionInfoCount
*     This field specifies the number of entries that are filled inside
*     sessionInfoTbl. Max value of this field once returned from RM would be
*     NV2080_GPU_NVFBC_MAX_COUNT.
*
*   sessionInfoTbl
*     This field specifies the array in which the NVFBC session information is to
*     be returned. RM will fill the current session data in sessionInfoTbl array
*     and then update the sessionInfoCount to reflect current session count value.
*
* Possible status values returned are:
*   NV_OK
*   NV_ERR_NO_MEMORY
*   NV_ERR_INVALID_LOCK_STATE
*   NV_ERR_INVALID_ARGUMENT
*/

#define NV2080_GPU_NVFBC_MAX_SESSION_COUNT 256U

#define NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS_MESSAGE_ID (0x7CU)

typedef struct NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS {
    NvU32                             sessionInfoCount;
    NV2080_CTRL_NVFBC_SW_SESSION_INFO sessionInfoTbl[NV2080_GPU_NVFBC_MAX_SESSION_COUNT];
} NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS;

#define NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO (0x2080017cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS_MESSAGE_ID" */



/*
 * NV2080_CTRL_CMD_GPU_GET_VMMU_SEGMENT_SIZE
 *
 * This command returns the VMMU page size
 *
 *   vmmuSegmentSize
 *     Output parameter.
 *     Returns the VMMU segment size (in bytes)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_VMMU_SEGMENT_SIZE (0x2080017eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS_MESSAGE_ID (0x7EU)

typedef struct NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 vmmuSegmentSize, 8);
} NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS;

#define NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_32MB     0x02000000U
#define NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_64MB     0x04000000U
#define NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_128MB    0x08000000U
#define NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_256MB    0x10000000U
#define NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_512MB    0x20000000U



/*
 * NV2080_CTRL_GPU_GET_PARTITION_CAPACITY
 *
 * This command returns the count of partitions of given size (represented by
 * NV2080_CTRL_GPU_PARTITION_FLAG_*) which can be requested via
 * NV2080_CTRL_GPU_SET_PARTITIONS ctrl call.
 * Note that this API does not "reserve" any partitions, and there is no
 * guarantee that the reported count of available partitions of a given size
 * will remain consistent following creation of partitions of different size
 * through NV2080_CTRL_GPU_SET_PARTITIONS.
 * Note that this API is unsupported if SMC is feature-disabled.
 *
 *   partitionFlag[IN]
 *      - Partition flag indicating size of requested partitions
 *
 *   partitionCount[OUT]
 *      - Available number of partitions of the given size which can currently be created.
 *
 *   availableSpans[OUT]
 *      - For each partition able to be created of the specified size, the span
 *        it could occupy.
 *
 *   availableSpansCount[OUT]
 *      - Number of valid entries in availableSpans.
 *
 *   totalPartitionCount[OUT]
 *      - Total number of partitions of the given size which can be created.
 *
 *   totalSpans[OUT]
 *      - List of spans which can possibly be occupied by partitions of the
 *        given type.
 *
 *   totalSpansCount[OUT]
 *      - Number of valid entries in totalSpans.
 *
 *   bStaticInfo[IN]
 *      - Flag indicating that client requests only the information from
 *        totalPartitionCount and totalSpans.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_PARTITION_CAPACITY (0x20800181U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS {
    NvU32  partitionFlag;
    NvU32  partitionCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PARTITION_SPAN availableSpans[NV2080_CTRL_GPU_MAX_PARTITIONS], 8);
    NvU32  availableSpansCount;
    NvU32  totalPartitionCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PARTITION_SPAN totalSpans[NV2080_CTRL_GPU_MAX_PARTITIONS], 8);
    NvU32  totalSpansCount;
    NvBool bStaticInfo;
} NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_CACHED_INFO
 *
 * This command returns cached(SW only) gpu information for the associated GPU.
 * Requests to retrieve gpu information use a list of one or more NV2080_CTRL_GPU_INFO
 * structures.
 * The gpuInfoList is aligned with NV2080_CTRL_GPU_GET_INFO_V2_PARAMS for security concern
 *
 *   gpuInfoListSize
 *     This field specifies the number of entries on the caller's
 *     gpuInfoList.
 *   gpuInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the gpu information is to be returned.
 *     This buffer must be at least as big as gpuInfoListSize multiplied
 *     by the size of the NV2080_CTRL_GPU_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_GPU_GET_CACHED_INFO (0x20800182U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_CACHED_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_CACHED_INFO_PARAMS_MESSAGE_ID (0x82U)

typedef NV2080_CTRL_GPU_GET_INFO_V2_PARAMS NV2080_CTRL_GPU_GET_CACHED_INFO_PARAMS;

/*
 * NV2080_CTRL_GPU_SET_PARTITIONING_MODE
 *
 * This command configures this GPU to control global mode for partitioning.
 * This command may not be sent to a GPU with any active partitions.
 * This command may be used to set the following modes:
 *
 * NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING
 *  NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_LEGACY
 *      This is the default mode. While this GPU is in this mode, no partitions
 *      will be allowed to be created via SET_PARTITIONS - a client must set one
 *      of the below modes prior to partitioning the GPU. When a client sets a
 *      GPU into this mode, any performance changes resulting from partitions
 *      made while in either of the below modes will be cleared. A
 *      physical-function-level reset is required after setting this mode.
 *
 *  NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_MAX_PERF
 *      In this mode, when the GPU is partitioned, each partition will have the
 *      maximum possible performance which can be evenly distributed among all
 *      partitions. The total performance of the GPU, taking into account all
 *      partitions created in this mode, may be less than that of a GPU running
 *      in legacy non-SMC mode. Partitions created while in this mode require a
 *      physical-function-level reset before the partitioning may take full
 *      effect.  Destroying all partitions while in this mode may be
 *      insufficient to restore full performance to the GPU - only by setting
 *      the mode to _LEGACY can this be achieved. A physical-function-level
 *      reset is NOT required after setting this mode.
 *
 *  NV2080_CTRL_GPU_SET_PARTIITONING_MODE_REPARTITIONING_FAST_RECONFIG
 *      By setting this mode, the performance of the GPU will be restricted such
 *      that all partitions will have a consistent fraction of the total
 *      available performance, which may be less than the maximum possible
 *      performance available to each partition. Creating or destroying
 *      partitions on this GPU while in this mode will not require a
 *      physical-function-level reset, and will not affect other active
 *      partitions. Destroying all partitions while in this mode may be
 *      insufficient to restore full performance to the GPU - only by setting
 *      the mode to _LEGACY can this be achieved. A physical-function-level
 *      reset is required after setting this mode.
 *
 * Parameters:
 *   partitioningMode[IN]
 *      - Partitioning Mode to set for this GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_SET_PARTITIONING_MODE                          (0x20800183U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING                1:0
#define NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_LEGACY        0U
#define NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_MAX_PERF      1U
#define NV2080_CTRL_GPU_SET_PARTITIONING_MODE_REPARTITIONING_FAST_RECONFIG 2U

#define NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS_MESSAGE_ID (0x83U)

typedef struct NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS {
    NvU32 partitioningMode;
} NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS;



/* NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_INFO
 *
 * This structure describes resources available in a partition requested of a
 * given type.
 *
 * [OUT] partitionFlag
 *      - Flags to specify in NV2080_CTRL_CMD_GPU_SET_PARTITIONS to request this
 *        partition
 *
 * [OUT] grCount
 *      - Total Number of SMC engines/GR engines (including GFX capable ones in this parition)
 *
 * [OUT] gfxGrCount
 *      - Number of SMC engines/GR engines capable of GFX. This is a subset of the engines included in grCount
 *
 * [OUT] gpcCount
 *      - Number of GPCs in this partition, including the GFX Capable ones.
 *
 * [OUT] virtualGpcCount
 *      - Number of virtualized GPCs in this partition, including the GFX Capable ones.
 *
 * [OUT] gfxGpcCount
 *      - Number of GFX Capable GPCs in this partition. This is a subset of the GPCs included in gpcCount.
 *
 * [OUT] veidCount
 *      - Number of VEIDS in this partition
 *
 * [OUT] smCount
 *      - Number of SMs in this partition
 *
 * [OUT] ceCount
 *      - Copy Engines in this partition
 *
 * [OUT] nvEncCount
 *      - Encoder Engines in this partition
 *
 * [OUT] nvDecCount
 *      - Decoder Engines in this partition
 *
 * [OUT] nvJpgCount
 *      - Jpg Engines in this partition
 *
 * [OUT] nvOfaCount
 *      - Ofa engines in this partition
 * [OUT] memorySize
 *      - Total available memory within this partition
 */
typedef struct NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_INFO {
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
    NV_DECLARE_ALIGNED(NvU64 memorySize, 8);
} NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_INFO;

/*
 * NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS
 *
 * This command returns information regarding GPU partitions which can be
 * requested via NV2080_CTRL_CMD_GPU_SET_PARTITIONS.
 *
 * [OUT] descCount
 *      - Number of valid partition types
 *
 * [OUT] partitionDescs
 *      - Information describing available partitions
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS {
    NvU32 descCount;
    // C form: NV2080_CTRL_GPU_DESCRIBE_PARTITION_INFO partitionDescs[NV2080_CTRL_GPU_PARTITION_MAX_TYPES];
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_INFO partitionDescs[NV2080_CTRL_GPU_PARTITION_MAX_TYPES], 8);
} NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS;

#define NV2080_CTRL_CMD_GPU_DESCRIBE_PARTITIONS (0x20800185U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS_MESSAGE_ID" */



/*
 * NV2080_CTRL_CMD_GPU_GET_MAX_SUPPORTED_PAGE_SIZE
 *
 * This command returns information regarding maximum page size supported
 * by GMMU on the platform on which RM is running.
 *
 * [OUT] maxSupportedPageSize
 *      - Maximum local vidmem page size supported by GMMU of a given GPU (HW)
 *        on a given platform (OS)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_MAX_SUPPORTED_PAGE_SIZE (0x20800188U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 maxSupportedPageSize, 8);
} NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS;



/*
 * NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC
 *
 * This command returns the max number of MMUs per GPC
 *
 *   gpcId [IN]
 *     Logical GPC id
 *   count [OUT]
 *     The number of MMUs per GPC
 *   grRouteInfo
 *     This parameter specifies the routing information used to
 *     disambiguate the target GR engine. When SMC is enabled, this
 *     is a mandatory parameter.
 */
#define NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC_PARAMS_MESSAGE_ID (0x8AU)

typedef struct NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC_PARAMS {
    NvU32 gpcId;
    NvU32 count;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_NUM_MMUS_PER_GPC (0x2080018aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_GPU_GET_ACTIVE_PARTITION_IDS
 *
 * This command returns the GPU partition IDs for all active partitions
 * If GPU is not partitioned, the control call will return partition count as "0"
 *
 *   swizzId[OUT]
 *      - HW Partition ID associated with the active partitions
 *
 *   partitionCount[OUT]
 *      - Number of active partitions in system
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS_MESSAGE_ID (0x8BU)

typedef struct NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS {
    NvU32 swizzId[NV2080_CTRL_GPU_MAX_PARTITION_IDS];
    NvU32 partitionCount;
} NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_ACTIVE_PARTITION_IDS               (0x2080018bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS_MESSAGE_ID" */



/*
 * NV2080_CTRL_CMD_GPU_GET_PIDS
 *
 * Given a resource identifier and its type, this command returns a set of
 * process identifiers (PIDs) of processes that have instantiated this resource.
 * For example, given a class number, this command returns a list of all
 * processes with clients that have matching object allocations.
 * This is a SMC aware call and the scope of the information gets restricted
 * based on partition subscription.
 * The call enforces partition subscription if SMC is enabled, and client is not
 * a monitoring client.
 * Monitoring clients get global information without any scope based filtering.
 * Monitoring clients are also not expected to subscribe to a partition when
 * SMC is enabled.
 *
 *   idType
 *     Type of the resource identifier. See below for a list of valid types.
 *   id
 *     Resource identifier.
 *   pidTblCount
 *      Number of entries in the PID table.
 *   pidTbl
 *     Table which will contain the PIDs. Each table entry is of type NvU32.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GPU_GET_PIDS       (0x2080018dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PIDS_PARAMS_MESSAGE_ID" */

/* max size of pidTable */
#define NV2080_CTRL_GPU_GET_PIDS_MAX_COUNT 950U

#define NV2080_CTRL_GPU_GET_PIDS_PARAMS_MESSAGE_ID (0x8DU)

typedef struct NV2080_CTRL_GPU_GET_PIDS_PARAMS {
    NvU32 idType;
    NvU32 id;
    NvU32 pidTblCount;
    NvU32 pidTbl[NV2080_CTRL_GPU_GET_PIDS_MAX_COUNT];
} NV2080_CTRL_GPU_GET_PIDS_PARAMS;

/*
 * Use class NV20_SUBDEVICE_0 with NV2080_CTRL_GPU_GET_PIDS_ID_TYPE_CLASS to query
 * PIDs with or without GPU contexts. For any other class id, PIDs only with GPU
 * contexts are returned.
 */
#define NV2080_CTRL_GPU_GET_PIDS_ID_TYPE_CLASS      (0x00000000U)
#define NV2080_CTRL_GPU_GET_PIDS_ID_TYPE_VGPU_GUEST (0x00000001U)

/*
 * NV2080_CTRL_SMC_SUBSCRIPTION_INFO
 *
 * This structure contains information about the SMC subscription type.
 * If MIG is enabled a valid ID is returned, it is set to PARTITIONID_INVALID otherwise.
 *
 *  computeInstanceId
 *      This parameter returns a valid compute instance ID
 *  gpuInstanceId
 *      This parameter returns a valid GPU instance ID
 */
typedef struct NV2080_CTRL_SMC_SUBSCRIPTION_INFO {
    NvU32 computeInstanceId;
    NvU32 gpuInstanceId;
} NV2080_CTRL_SMC_SUBSCRIPTION_INFO;

/*
 * NV2080_CTRL_GPU_PID_INFO_VIDEO_MEMORY_USAGE_DATA
 *
 * This structure contains the video memory usage information.
 *
 *   memPrivate
 *     This parameter returns the amount of memory exclusively owned
 *     (i.e. private) to the client
 *   memSharedOwned
 *     This parameter returns the amount of shared memory owned by the client
 *   memSharedDuped
 *     This parameter returns the amount of shared memory duped by the client
 *   protectedMemPrivate
 *     This parameter returns the amount of protected memory exclusively owned
 *     (i.e. private) to the client whenever memory protection is enabled
 *   protectedMemSharedOwned
 *     This parameter returns the amount of shared protected memory owned by the
 *     client whenever memory protection is enabled
 *   protectedMemSharedDuped
 *     This parameter returns the amount of shared protected memory duped by the
 *     client whenever memory protection is enabled
 */
typedef struct NV2080_CTRL_GPU_PID_INFO_VIDEO_MEMORY_USAGE_DATA {
    NV_DECLARE_ALIGNED(NvU64 memPrivate, 8);
    NV_DECLARE_ALIGNED(NvU64 memSharedOwned, 8);
    NV_DECLARE_ALIGNED(NvU64 memSharedDuped, 8);
    NV_DECLARE_ALIGNED(NvU64 protectedMemPrivate, 8);
    NV_DECLARE_ALIGNED(NvU64 protectedMemSharedOwned, 8);
    NV_DECLARE_ALIGNED(NvU64 protectedMemSharedDuped, 8);
} NV2080_CTRL_GPU_PID_INFO_VIDEO_MEMORY_USAGE_DATA;

#define NV2080_CTRL_GPU_PID_INFO_INDEX_VIDEO_MEMORY_USAGE (0x00000000U)

#define NV2080_CTRL_GPU_PID_INFO_INDEX_MAX                NV2080_CTRL_GPU_PID_INFO_INDEX_VIDEO_MEMORY_USAGE

typedef union NV2080_CTRL_GPU_PID_INFO_DATA {
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PID_INFO_VIDEO_MEMORY_USAGE_DATA vidMemUsage, 8);
} NV2080_CTRL_GPU_PID_INFO_DATA;


/*
 * NV2080_CTRL_GPU_PID_INFO
 *
 * This structure contains the per pid information. Each type of information
 * retrievable via NV2080_CTRL_CMD_GET_PID_INFO is assigned a unique index
 * below. In addition the process for which the lookup is for is also defined.
 * This is a SMC aware call and the scope of the information gets restricted
 * based on partition subscription.
 * The call enforces partition subscription if SMC is enabled, and client is not
 * a monitoring client.
 * Monitoring clients get global information without any scope based filtering.
 * Monitoring clients are also not expected to subscribe to a partition when
 * SMC is enabled.
 *
 *   pid
 *     This parameter specifies the PID of the process for which information is
 *     to be queried.
 *   index
 *     This parameter specifies the type of information being queried for the
 *     process of interest.
 *   result
 *     This parameter returns the result of the instruction's execution.
 *   data
 *     This parameter returns the data corresponding to the information which is
 *     being queried.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 *
 * Valid PID information indices are:
 *
 *   NV2080_CTRL_GPU_PID_INFO_INDEX_VIDEO_MEMORY_USAGE
 *     This index is used to request the amount of video memory on this GPU
 *     allocated to the process.
 */
typedef struct NV2080_CTRL_GPU_PID_INFO {
    NvU32                             pid;
    NvU32                             index;
    NvU32                             result;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PID_INFO_DATA data, 8);
    NV2080_CTRL_SMC_SUBSCRIPTION_INFO smcSubscription;
} NV2080_CTRL_GPU_PID_INFO;

/*
 * NV2080_CTRL_CMD_GPU_GET_PID_INFO
 *
 * This command allows querying per-process information from the RM. Clients
 * request information by specifying a unique informational index and the
 * Process ID of the process in question. The result is set to indicate success
 * and the information queried (if available) is returned in the data parameter.
 *
 *   pidInfoListCount
 *     The number of valid entries in the pidInfoList array.
 *   pidInfoList
 *     An array of NV2080_CTRL_GPU_PID_INFO of maximum length
 *     NV2080_CTRL_GPU_GET_PID_INFO_MAX_COUNT.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GPU_GET_PID_INFO       (0x2080018eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_PID_INFO_PARAMS_MESSAGE_ID" */

/* max size of pidInfoList */
#define NV2080_CTRL_GPU_GET_PID_INFO_MAX_COUNT 200U

#define NV2080_CTRL_GPU_GET_PID_INFO_PARAMS_MESSAGE_ID (0x8EU)

typedef struct NV2080_CTRL_GPU_GET_PID_INFO_PARAMS {
    NvU32 pidInfoListCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PID_INFO pidInfoList[NV2080_CTRL_GPU_GET_PID_INFO_MAX_COUNT], 8);
} NV2080_CTRL_GPU_GET_PID_INFO_PARAMS;



/*!
 * NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT
 *
 * @brief Handle VF PRI faults
 *
 *   faultType
 *     BAR1, BAR2, PHYSICAL or UNBOUND_INSTANCE
 *
 * Possible status values returned are:
 *     NV_OK
 *     NV_ERR_NOT_SUPPORTED
 *
 */

#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT                       (0x20800192U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_INVALID          0U
#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_BAR1             1U
#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_BAR2             2U
#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_PHYSICAL         3U
#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_TYPE_UNBOUND_INSTANCE 4U

#define NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_MESSAGE_ID (0x92U)

typedef struct NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS {
    NvU32 faultType;
} NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS;


/*!
 * Compute policy types to be specified by callers to set a config.
 *
 * _TIMESLICE
 *     Set the timeslice config for the requested GPU.
 *     Check @ref NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_DATA_TIMESLICE for
 *     permissible timeslice values.
 */
#define NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE      0U
#define NV2080_CTRL_GPU_COMPUTE_POLICY_MAX            1U

/*!
 * Enum consisting of permissible timeslice options that can configured
 * for a GPU. These can be queried by compute clients and the exact
 * timeslice values can be chosen appropriately as per GPU support
 */
typedef enum NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_DATA_TIMESLICE {
    NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_DEFAULT = 0,
    NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_SHORT = 1,
    NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_MEDIUM = 2,
    NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_LONG = 3,
    NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_MAX = 4,
} NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_DATA_TIMESLICE;

typedef struct NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG {
    /*!
     * NV2080_CTRL_GPU_COMPUTE_POLICY_<xyz>
     */
    NvU32 type;

    /*!
     * Union of type-specific data
     */
    union {
        NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_DATA_TIMESLICE timeslice;
    } data;
} NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG;

#define NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS_MESSAGE_ID (0x94U)

typedef struct NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS {
    NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG config;
} NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_COMPUTE_POLICY_CONFIG
 *
 * This command retrieves all compute policies configs for the associated gpu.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV2080_CTRL_CMD_GPU_GET_COMPUTE_POLICY_CONFIG  (0x20800195U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS_MESSAGE_ID" */

/*!
 * This define limits the max number of policy configs that can be handled by
 * NV2080_CTRL_CMD_GPU_GET_COMPUTE_POLICY_CONFIG command.
 *
 * @note Needs to be in sync (greater or equal) to NV2080_CTRL_GPU_COMPUTE_POLICY_MAX.
 */

#define NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_LIST_MAX 32U

#define NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS_MESSAGE_ID (0x95U)

typedef struct NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS {
    NvU32                                 numConfigs;

    /*!
     * C form:
     * NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG configList[NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_LIST_MAX];
     */
    NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG configList[NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_LIST_MAX];
} NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_GFID
 *
 * This command returns the GFID (GPU Function ID) for a given SR-IOV
 * Virtual Function (VF) of the physical GPU.
 *
 *   domain [IN]
 *     This field specifies the respective domain of the PCI device.
 *   bus [IN]
 *     This field specifies the bus id for a given VF.
 *   device [IN]
 *     This field specifies the device id for a given VF.
 *   func [IN]
 *     This field specifies the function id for a given VF.
 *   gfid[OUT]
 *      - This field returns GFID for a given VF BDF.
 *   gfidMask[OUT]
 *      - This field returns GFID mask value.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */

#define NV2080_CTRL_CMD_GPU_GET_GFID (0x20800196U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_GFID_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_GFID_PARAMS_MESSAGE_ID (0x96U)

typedef struct NV2080_CTRL_GPU_GET_GFID_PARAMS {
    NvU32 domain;
    NvU8  bus;
    NvU8  device;
    NvU8  func;
    NvU32 gfid;
    NvU32 gfidMask;
} NV2080_CTRL_GPU_GET_GFID_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY
 *
 * This command informs the GPU driver that the GPU partition associated with
 * a given GFID has been activated or will be deactivated.
 *
 *   gfid[IN]
 *      - The GPU function identifier for a given VF BDF
 *   bEnable [IN]
 *      - Set to NV_TRUE if the GPU partition has been activated.
 *      - Set to NV_FALSE if the GPU partition will be deactivated.
 *   fabricPartitionId [IN]
 *      - Set the fabric manager partition ID dring partition activation.
 *      - Ignored during partition deactivation.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */

#define NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY (0x20800197U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS_MESSAGE_ID (0x97U)

typedef struct NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS {
    NvU32  gfid;
    NvBool bEnable;
    NvU32  fabricPartitionId;
} NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS;

/*!
 * NV2080_CTRL_CMD_GPU_VALIDATE_MEM_MAP_REQUEST
 *
 * @brief Validate the address range for memory map request by comparing the
 *        user supplied address range with GPU BAR0/BAR1 range.
 *
 * @param[in]   addressStart    Start address for memory map request
 * @param[in]   addressLength   Length for for memory map request
 * @param[out]  protection      NV_PROTECT_READ_WRITE, if both read/write is allowed
 *                              NV_PROTECT_READABLE, if only read is allowed
 *
 * Possible status values returned are:
 *     NV_OK
 *     NV_ERR_PROTECTION_FAULT
 *
 */
#define NV2080_CTRL_CMD_GPU_VALIDATE_MEM_MAP_REQUEST (0x20800198U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS_MESSAGE_ID (0x98U)

typedef struct NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 addressStart, 8);
    NV_DECLARE_ALIGNED(NvU64 addressLength, 8);
    NvU32 protection;
} NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR
 *
 * @brief This command is similar to NV2080_CTRL_CMD_GPU_SET_FABRIC_BASE_ADDR
 * but will be used to set the EGM fabric base addr associated with the gpu.
 * Note: For EGM FLA, we will be making use of the existing control call i.e
 * NV2080_CTRL_CMD_FLA_RANGE
 *
 */
#define NV2080_CTRL_CMD_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR (0x20800199U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS_MESSAGE_ID (0x99U)

typedef struct NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 egmGpaFabricBaseAddr, 8);
} NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ENGINE_LOAD_TIMES
 *
 * This command is used to retrieve the load time (latency) of each engine.
 *
 *   engineCount
 *     This field specifies the number of entries of the following
 *     three arrays.
 *
 *   engineList[NV2080_GPU_MAX_ENGINE_OBJECTS]
 *     An array of NvU32 which stores each engine's descriptor.
 *
 *   engineStateLoadTime[NV2080_GPU_MAX_ENGINE_OBJECTS]
 *     A array of NvU64 which stores each engine's load time.
 *
 *   engineIsInit[NV2080_GPU_MAX_ENGINE_OBJECTS]
 *     A array of NvBool which stores each engine's initialization status.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_GPU_GET_ENGINE_LOAD_TIMES (0x2080019bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS        0xC8U

#define NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS_MESSAGE_ID (0x9BU)

typedef struct NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS {
    NvU32  engineCount;
    NvU32  engineList[NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS];
    NV_DECLARE_ALIGNED(NvU64 engineStateLoadTime[NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS], 8);
    NvBool engineIsInit[NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS];
} NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_ID_NAME_MAPPING
 *
 * This command is used to retrieve the mapping of engine ID and engine Name.
 *
 *   engineCount
 *     This field specifies the size of the mapping.
 *
 *   engineID
 *     An array of NvU32 which stores each engine's descriptor.
 *
 *   engineName
 *     An array of char[100] which stores each engine's name.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_GPU_GET_ID_NAME_MAPPING (0x2080019cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_ID_NAME_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_ID_NAME_MAPPING_PARAMS_MESSAGE_ID (0x9CU)

typedef struct NV2080_CTRL_GPU_GET_ID_NAME_MAPPING_PARAMS {
    NvU32 engineCount;
    NvU32 engineID[NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS];
    char  engineName[NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS][100];
} NV2080_CTRL_GPU_GET_ID_NAME_MAPPING_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_EXEC_REG_OPS_NOPTRS
 *
 * Same as above NV2080_CTRL_CMD_GPU_EXEC_REG_OPS except that this CTRL CMD will
 * not allow any embedded pointers. The regOps array is inlined as part of the
 * struct.
 * NOTE: This intended for gsp plugin only as it may override regOp access
 *       restrictions
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_GPU_EXEC_REG_OPS_NOPTRS (0x2080019dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_EXEC_REG_OPS_NOPTRS_PARAMS_MESSAGE_ID" */

/* setting this to 100 keeps it right below 4k in size */
#define NV2080_CTRL_REG_OPS_ARRAY_MAX           100U
#define NV2080_CTRL_GPU_EXEC_REG_OPS_NOPTRS_PARAMS_MESSAGE_ID (0x9DU)

typedef struct NV2080_CTRL_GPU_EXEC_REG_OPS_NOPTRS_PARAMS {
    NvHandle               hClientTarget;
    NvHandle               hChannelTarget;
    NvU32                  bNonTransactional;
    NvU32                  reserved00[2];
    NvU32                  regOpCount;
    NV2080_CTRL_GPU_REG_OP regOps[NV2080_CTRL_REG_OPS_ARRAY_MAX];
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GPU_EXEC_REG_OPS_NOPTRS_PARAMS;




/*!
 * NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO
 *
 * [in/out] gpuId
 *   GPU ID for which the capabilities are queried.
 *   For the NV2080_CTRL_CMD_GET_P2P_CAPS control:
 *     If bAllCaps == NV_TRUE, this parameter is an out parameter and equals to
 *     the GPU ID of an attached GPU.
 *     If bAllCaps == NV_FALSE, this parameter is an in parameter and the requester
 *     should set it to the ID of the GPU that needs to be queried from.
 * [in] gpuUuid
 *   Alternative to gpuId; used to identify target GPU for which caps are being queried.
 *   Option only available for Guest RPCs.
 *     If bUseUuid == NV_TRUE, gpuUuid is used in lieu of gpuId to identify target GPU.
 *     If bUseUuid == NV_FALSE, gpuUuid is ignored and gpuId is used by default.
 *     If bAllCaps == NV_TRUE, gpuUuid is ignored.
 * [out] p2pCaps
 *   Peer to peer capabilities discovered between the GPUs.
 *   See NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 for the list of valid values.
 * [out] p2pOptimalReadCEs
 *   Mask of CEs to use for p2p reads over Nvlink.
 * [out] p2pOptimalWriteCEs
 *   Mask of CEs to use for p2p writes over Nvlink.
 * [out] p2pCapsStatus
 *   Status of all supported p2p capabilities.
 *   See NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 for the list of valid values.
 * [out] busPeerId
 *   Bus peer ID. For an invalid or a non-existent peer this field
 *   has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 * [out] busEgmPeerId
 *   Bus EGM peer ID. For an invalid or a non-existent peer this field
 *   has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 */
#define NV2080_GET_P2P_CAPS_UUID_LEN         16U

typedef struct NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO {
    NvU32 gpuId;
    NvU8  gpuUuid[NV2080_GET_P2P_CAPS_UUID_LEN];
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NvU32 busPeerId;
    NvU32 busEgmPeerId;
} NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO;

/*!
 * NV2080_CTRL_CMD_GET_P2P_CAPS
 *
 * Returns peer to peer capabilities present between GPUs.
 * The caller must either specify bAllCaps to query the capabilities for
 * all the attached GPUs or they must pass a valid list of GPU IDs.
 *
 *   [in] bAllCaps
 *     Set to NV_TRUE to query the capabilities for all the attached GPUs.
 *     Set to NV_FALSE and specify peerGpuCount and peerGpuCaps[].gpuId
 *     to retrieve the capabilities only for the specified GPUs.
 *   [in] bUseUuid
 *     Option only available for Guest RPCs.
 *     Set to NV_TRUE to use gpuUuid in lieu of gpuId to identify target GPU.
 *     If bAllCaps == NV_TRUE, bUseUuid is ignored.
 *   [in/out] peerGpuCount
 *     The number of the peerGpuCaps entries.
 *     If bAllCaps == NV_TRUE, this parameter is an out parameter and equals to
 *     the total number of the attached GPUs.
 *     If bAllCaps == NV_FALSE, this parameter is an in parameter and the requester
 *     should set it to the number of the peerGpuCaps entries.
 *   [in/out] peerGpuCaps
 *     The array of NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO entries, describing
 *     the peer to peer capabilities of the GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT - Invalid peerGpuCount
 *   NV_ERR_OBJECT_NOT_FOUND - Invalid peerGpuCaps[].gpuId
 */
#define NV2080_CTRL_CMD_GET_P2P_CAPS (0x208001a0U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GET_P2P_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GET_P2P_CAPS_PARAMS_MESSAGE_ID (0xA0U)

typedef struct NV2080_CTRL_GET_P2P_CAPS_PARAMS {
    NvBool                                  bAllCaps;
    NvBool                                  bUseUuid;
    NvU32                                   peerGpuCount;
    NV2080_CTRL_GPU_P2P_PEER_CAPS_PEER_INFO peerGpuCaps[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
} NV2080_CTRL_GET_P2P_CAPS_PARAMS;



/*!
 * NV2080_CTRL_GPU_COMPUTE_PROFILE
 *
 * This structure specifies resources in an execution partition
 *
 *  id[OUT]
 *      - Total Number of GPCs in this partition
 *
 *  computeSize[OUT]
 *      - NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_* associated with this profile
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
typedef struct NV2080_CTRL_GPU_COMPUTE_PROFILE {
    NvU8  computeSize;
    NvU32 gfxGpcCount;
    NvU32 gpcCount;
    NvU32 veidCount;
    NvU32 smCount;
} NV2080_CTRL_GPU_COMPUTE_PROFILE;

/*!
 * NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS
 *
 * This structure specifies resources in an execution partition
 *
 *  profileCount[OUT]
 *      - Total Number of profiles filled
 *
 *  profiles[OUT]
 *      - NV2080_CTRL_GPU_COMPUTE_PROFILE filled with valid compute instance profiles
 */
#define NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_MESSAGE_ID (0xA2U)

typedef struct NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS {
    NvU32                           profileCount;
    NV2080_CTRL_GPU_COMPUTE_PROFILE profiles[NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE];
} NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS;

#define NV2080_CTRL_CMD_GPU_GET_COMPUTE_PROFILES       (0x208001a2) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_FABRIC_PROBE_STATE_UNSUPPORTED 0U
#define NV2080_CTRL_GPU_FABRIC_PROBE_STATE_NOT_STARTED 1U
#define NV2080_CTRL_GPU_FABRIC_PROBE_STATE_IN_PROGRESS 2U
#define NV2080_CTRL_GPU_FABRIC_PROBE_STATE_COMPLETE    3U

#define NV2080_GPU_FABRIC_CLUSTER_UUID_LEN             16U

#define NV2080_CTRL_GPU_FABRIC_PROBE_CAP_MC_SUPPORTED           NVBIT64(0)

#define NV2080_CTRL_GPU_FABRIC_PROBE_CAP_MC_MUTLINODE_SUPPORTED NVBIT64(1)



#define NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_DEGRADED_BW 1:0
#define NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_DEGRADED_BW_NOT_SUPPORTED 0
#define NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_DEGRADED_BW_TRUE          1
#define NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_DEGRADED_BW_FALSE         2

/*!
 * NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS
 *
 * This structure provides the GPU<-->FM probe state info on NVSwitch based
 * systems
 *
 *  state[OUT]
 *      - Current state of GPU<-->FM probe req/rsp
 *        Following values can be returned
 *        _UNSUPPORTED - system does not support this feature
 *        _NOT_STARTED - probe request is not yet sent to the FM
 *        _IN_PROGRESS - probe response is not yet received
 *        _COMPLETE    - probe response is received
 *        When state is NV2080_CTRL_GPU_FABRIC_PROBE_STATE_COMPLETE
 *        status has to be checked for probe response success/failure
 *  status[OUT]
 *      - Inband Probe response status
 *        A GPU which returns NV_OK status upon receiving the probe response
 *        can participate in P2P
 *  clusterUuid[OUT]
 *      - Uuid of the cluster to which this node belongs
 *  fabricPartitionId[OUT]
 *      - Fabric partition Id
 *  fabricCaps[OUT]
 *      - Summary of fabric capabilities received from probe resp
 *        Possible values are
 *            NV2080_CTRL_GPU_FABRIC_PROBE_CAP_*
 *  fabricCliqueId[OUT]
 *      - Unique ID of a set of GPUs within a fabric partition that can perform P2P
 *  fabricHealthMask[OUT]
 *      - Mask where bits indicate different status about the health of the fabric
 */
#define NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID (0xA3U)

typedef struct NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS {
    NvU8      state;
    NV_STATUS status;
    NvU8      clusterUuid[NV2080_GPU_FABRIC_CLUSTER_UUID_LEN];
    NvU16     fabricPartitionId;
    NV_DECLARE_ALIGNED(NvU64 fabricCaps, 8);
    NvU32     fabricCliqueId;
    NvU32     fabricHealthMask;
} NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS;

#define NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO (0x208001a3) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_GPU_GET_CHIP_DETAILS
 *
 * This command retrieves and constructs the GPU partnumber from the VBIOS.
 *
 * The following data are currently supported:
 *
 * pciDevId
 *    The PCI device ID
 *
 * chipSku
 *    The chip SKU information
 *
 * chipMajor
 *    The chip major number
 *
 * chipMinor
 *    The chip minor number
 *
 */
#define NV2080_CTRL_CMD_GPU_GET_CHIP_DETAILS      (0x208001a4U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS_MESSAGE_ID" */

/*
 * The string format for a GPU part number
 * The GPU part number is formatted with 4 hexadecimal digits for the PCI device ID, the chip SKU string,
 * the chip major number, and then the chip minor number.
 * Ordering of the fields for the string format must be synced with the NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS
 * struct fields.
 */
#define GPU_PART_NUMBER_FMT "%04X-%s-%X%X"

/* The maximum length for the chip sku */
#define NV2080_MAX_CHIP_SKU_LENGTH                0x00000004U

#define NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS_MESSAGE_ID (0xA4U)

typedef struct NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS {
    NvU32 pciDevId;
    NvU8  chipSku[NV2080_MAX_CHIP_SKU_LENGTH];
    NvU32 chipMajor;
    NvU32 chipMinor;
} NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP
 *
 * This command returns the host hardware defined engine ID of the specified engine(s).
 *
 *   swizzId[IN]
 *       - HW Partition ID associated with the requested partition.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP (0x208001a5U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP_PARAMS_MESSAGE_ID (0xA5U)

typedef struct NV2080_CTRL_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP_PARAMS {
    NvU32 swizzId;
} NV2080_CTRL_GPU_MOVE_RUNLISTS_ALLOCATION_TO_SUBHEAP_PARAMS;

#define NV2080_CTRL_CMD_GPU_MIGRATABLE_OPS      (0x208001a6U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_CMD_GPU_MIGRATABLE_OPS_GSP  (0x208001a7U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_MIGRATABLE_OPS_GSP_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_CMD_GPU_MIGRATABLE_OPS_VGPU (0x208001a8U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_MIGRATABLE_OPS_VGPU_PARAMS_MESSAGE_ID" */

/* setting this to 100 keeps it right below 4k in size */
#define NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX    50U
typedef struct NV2080_CTRL_GPU_MIGRATABLE_OPS_CMN_PARAMS {
    NvHandle               hClientTarget;
    NvHandle               hChannelTarget;
    NvU32                  bNonTransactional;
    NvU32                  regOpCount;
    NvU32                  smIds[NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX];
    NV2080_CTRL_GPU_REG_OP regOps[NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX];
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV2080_CTRL_GPU_MIGRATABLE_OPS_CMN_PARAMS;

#define NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_MESSAGE_ID (0xA6U)

typedef NV2080_CTRL_GPU_MIGRATABLE_OPS_CMN_PARAMS NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS;
#define NV2080_CTRL_GPU_MIGRATABLE_OPS_GSP_PARAMS_MESSAGE_ID (0xA7U)

typedef NV2080_CTRL_GPU_MIGRATABLE_OPS_CMN_PARAMS NV2080_CTRL_GPU_MIGRATABLE_OPS_GSP_PARAMS;
#define NV2080_CTRL_GPU_MIGRATABLE_OPS_VGPU_PARAMS_MESSAGE_ID (0xA8U)

typedef NV2080_CTRL_GPU_MIGRATABLE_OPS_CMN_PARAMS NV2080_CTRL_GPU_MIGRATABLE_OPS_VGPU_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_MARK_DEVICE_FOR_RESET
 *
 * INTERNAL DEBUG/TESTING USE ONLY
 *
 * Marks the device for reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_MARK_DEVICE_FOR_RESET   (0x208001a9U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xA9" */

/*
 * NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_RESET
 *
 * INTERNAL DEBUG/TESTING USE ONLY
 *
 * Unmarks the device for reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_RESET (0x208001aaU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xAA" */

/*
 * NV2080_CTRL_CMD_GPU_GET_RESET_STATUS
 *
 * Gets the current reset status of the device.
 *
 * bResetNeeded
 *   Set to NV_TRUE if the device needs to be reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_RESET_STATUS        (0x208001abU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS_MESSAGE_ID (0xABU)

typedef struct NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS {
    NvBool bResetRequired;
} NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_MARK_DEVICE_FOR_DRAIN_AND_RESET
 *
 * INTERNAL DEBUG/TESTING USE ONLY
 *
 * Marks the device for drain and reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_MARK_DEVICE_FOR_DRAIN_AND_RESET   (0x208001acU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xAC" */

/*
 * NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_DRAIN_AND_RESET
 *
 * INTERNAL DEBUG/TESTING USE ONLY
 *
 * Unmarks the device for drain and reset.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_DRAIN_AND_RESET (0x208001adU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | 0xAD" */

/*
 * NV2080_CTRL_CMD_GPU_GET_DRAIN_AND_RESET_STATUS
 *
 * Gets the current drain and reset status of the device. Drain and reset is used only SMC configs.
 *
 * bDrainRecommended
 *   Set to NV_TRUE if a drain and reset is recommended for the device.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_GPU_GET_DRAIN_AND_RESET_STATUS        (0x208001aeU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS_MESSAGE_ID (0xAEU)

typedef struct NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS {
    NvBool bDrainRecommended;
} NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS;

/*
 * NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2
 *
 * This command returns NVENC software sessions information for the associate GPU.
 * This command is similar to NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO but doesn't have
 * embedded pointers.
 *
 * Check NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO for detailed information.
 */

#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS_MESSAGE_ID (0xAFU)

typedef struct NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS {
    NvU32                             sessionInfoTblEntry;
    NV2080_CTRL_NVENC_SW_SESSION_INFO sessionInfoTbl[NV2080_CTRL_GPU_NVENC_SESSION_INFO_MAX_COPYOUT_ENTRIES];
} NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS;

#define NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2 (0x208001afU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_GPU_CONSTRUCTED_FALCON_INFO {
    NvU32 engDesc;
    NvU32 ctxAttr;
    NvU32 ctxBufferSize;
    NvU32 addrSpaceList;
    NvU32 registerBase;
} NV2080_CTRL_GPU_CONSTRUCTED_FALCON_INFO;
#define NV2080_CTRL_GPU_MAX_CONSTRUCTED_FALCONS         0x40

#define NV2080_CTRL_CMD_GPU_GET_CONSTRUCTED_FALCON_INFO (0x208001b0) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS_MESSAGE_ID (0xB0U)

typedef struct NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS {
    NvU32                                   numConstructedFalcons;
    NV2080_CTRL_GPU_CONSTRUCTED_FALCON_INFO constructedFalconsTable[NV2080_CTRL_GPU_MAX_CONSTRUCTED_FALCONS];
} NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_GPU_GET_VF_CAPS 
 *
 * This command will return the MSIX capabilities for virtual function
 * Parameters:
 * 
 * gfid [IN]
 *   The GPU function identifier for a given VF BDF
 *
 * vfMsixCap [out]
 *   This field returns the VF MSIX cap values
 *
 * Possible status values returned are:
 *   NV_OK
 */
typedef struct NV2080_VF_MSIX_CAPS {
    NvU32 msix_header;
    NvU32 msix_table;
    NvU32 msix_pba;
} NV2080_VF_MSIX_CAPS;

#define NV2080_CTRL_CMD_GPU_GET_VF_CAPS (0x208001b1) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS_MESSAGE_ID (0xB1U)

typedef struct NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS {
    NvU32               gfid;
    NV2080_VF_MSIX_CAPS vfMsixCap;
} NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS;

/*
 * NV2080_CTRL_GPU_GET_FIPS_STATUS
 *
 * @brief get FIPS status (enabled/disabled) from GSP-RM
 *
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS_MESSAGE_ID (0xe4U)

typedef struct NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS {
    NvBool bFipsEnabled;
} NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS;
#define NV2080_CTRL_GPU_GET_FIPS_STATUS (0x208001e4) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPU_INTERFACE_ID << 8) | NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS_MESSAGE_ID" */


/* _ctrl2080gpu_h_ */
