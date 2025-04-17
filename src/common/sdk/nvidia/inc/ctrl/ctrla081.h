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
// Source file:      ctrl/ctrla081.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "nv_vgpu_types.h"
#include "nvcfg_sdk.h"
/* NVA081_VGPU_CONFIG control commands and parameters */

#define NVA081_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0xA081, NVA081_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVA081_CTRL_RESERVED                 (0x00)
#define NVA081_CTRL_VGPU_CONFIG              (0x01)

#define NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE 0x00
#define NVA081_MAX_VGPU_TYPES_PER_PGPU       0x64
#define NVA081_MAX_VGPU_PER_PGPU             32
#define NVA081_MAX_VGPU_PER_GI               12
#define NVA081_VM_UUID_SIZE                  16
#define NVA081_VGPU_STRING_BUFFER_SIZE       32
#define NVA081_VGPU_SIGNATURE_SIZE           128
#define NVA081_VM_NAME_SIZE                  128
#define NVA081_PCI_CONFIG_SPACE_SIZE         0x100
#define NVA081_PGPU_METADATA_STRING_SIZE     256
#define NVA081_EXTRA_PARAMETERS_SIZE         1024
#define NVA081_PLACEMENT_ID_INVALID          0xFFFFU
#define NVA081_CONFIG_PARAMS_MAX_LENGTH      1024

#define NVA081_MAX_BAR_REGION_COUNT          4
#define NVA081_MAX_SPARSE_REGION_COUNT       5

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_SET_INFO
 *
 * This command sets the vGPU config information in RM
 *
 * Parameters:
 *
 * discardVgpuTypes [IN]
 *  This parameter specifies if existing vGPU configuration should be
 *  discarded for given pGPU
 *
 * vgpuInfo [IN]
 *  This parameter specifies virtual GPU type information
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_SET_INFO (0xa0810101) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS_MESSAGE_ID" */

/*
 * NVA081_CTRL_VGPU_CONFIG_INFO
 *
 * This structure represents the per vGPU information
 *
 */
typedef struct NVA081_CTRL_VGPU_INFO {
    // This structure should be in sync with NVA082_CTRL_CMD_HOST_VGPU_DEVICE_GET_VGPU_TYPE_INFO_PARAMS
    NvU32 vgpuType;
    NvU8  vgpuName[NVA081_VGPU_STRING_BUFFER_SIZE];
    NvU8  vgpuClass[NVA081_VGPU_STRING_BUFFER_SIZE];
    NvU8  vgpuSignature[NVA081_VGPU_SIGNATURE_SIZE];
    NvU8  license[NV_GRID_LICENSE_INFO_MAX_LENGTH];
    NvU32 maxInstance;
    NvU32 numHeads;
    NvU32 maxResolutionX;
    NvU32 maxResolutionY;
    NvU32 maxPixels;
    NvU32 frlConfig;
    NvU32 cudaEnabled;
    NvU32 eccSupported;
    NvU32 gpuInstanceSize;
    NvU32 multiVgpuSupported;
    NV_DECLARE_ALIGNED(NvU64 vdevId, 8);
    NV_DECLARE_ALIGNED(NvU64 pdevId, 8);
    NV_DECLARE_ALIGNED(NvU64 profileSize, 8);
    NV_DECLARE_ALIGNED(NvU64 fbLength, 8);
    NV_DECLARE_ALIGNED(NvU64 gspHeapSize, 8);
    NV_DECLARE_ALIGNED(NvU64 fbReservation, 8);
    NV_DECLARE_ALIGNED(NvU64 mappableVideoSize, 8);
    NvU32 encoderCapacity;
    NV_DECLARE_ALIGNED(NvU64 bar1Length, 8);
    NvU32 frlEnable;
    NvU8  adapterName[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU16 adapterName_Unicode[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU8  shortGpuNameString[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU8  licensedProductName[NV_GRID_LICENSE_INFO_MAX_LENGTH];
    NvU32 vgpuExtraParams[NVA081_EXTRA_PARAMETERS_SIZE];
    NvU32 ftraceEnable;
    NvU32 gpuDirectSupported;
    NvU32 nvlinkP2PSupported;
    NvU32 maxInstancePerGI;
    NvU32 multiVgpuExclusive;
    NvU32 exclusiveType;
    NvU32 exclusiveSize;
    // used only by NVML
    NvU32 gpuInstanceProfileId;
    NvU32 placementSize;
    NvU32 homogeneousPlacementCount;
    NvU32 homogeneousPlacementIds[NVA081_MAX_VGPU_PER_PGPU];
    NvU32 heterogeneousPlacementCount;
    NvU32 heterogeneousPlacementIds[NVA081_MAX_VGPU_PER_PGPU];
} NVA081_CTRL_VGPU_INFO;

/*
 * NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS
 *
 * This structure represents the vGPU configuration information
 *
 */
#define NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS {
    NvBool discardVgpuTypes;
    NV_DECLARE_ALIGNED(NVA081_CTRL_VGPU_INFO vgpuInfo, 8);
    NvU32  vgpuConfigState;
} NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS;

/* VGPU Config state values */
#define NVA081_CTRL_VGPU_CONFIG_STATE_UNINITIALIZED         0
#define NVA081_CTRL_VGPU_CONFIG_STATE_IN_PROGRESS           1
#define NVA081_CTRL_VGPU_CONFIG_STATE_READY                 2

/*
 * NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU
 *
 * This command enumerates list of vGPU guest instances per pGpu
 *
 * Parameters:
 *
 * vgpuType [OUT]
 *  This parameter specifies the virtual GPU type for this physical GPU
 *
 * numVgpu [OUT]
 *  This parameter specifies the number of virtual GPUs created on this physical GPU
 *
 * guestInstanceInfo [OUT]
 *  This parameter specifies an array containing guest instance's information for
 *  all instances created on this physical GPU
 *
 * guestVgpuInfo [OUT]
 *  This parameter specifies an array containing guest vgpu's information for
 *  all vGPUs created on this physical GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU (0xa0810102) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS_MESSAGE_ID" */

/*
 * NVA081_GUEST_VM_INFO
 *
 * This structure represents vGPU guest's (VM's) information
 *
 * vmPid [OUT]
 *  This param specifies the vGPU plugin process ID
 * vmIdType [OUT]
 *  This param specifies the VM ID type, i.e. DOMAIN_ID or UUID
 * guestOs [OUT]
 *  This param specifies the vGPU guest OS type
 * migrationProhibited [OUT]
 *  This flag indicates whether migration is prohibited for VM or not
 * guestNegotiatedVgpuVersion [OUT]
 *  This param specifies the vGPU version of guest driver after negotiation
 * frameRateLimit [OUT]
 *  This param specifies the current value of FRL set for guest
 * licensed [OUT]
 *  This param specifies whether the VM is Unlicensed/Licensed
 * licenseState [OUT]
 *  This param specifies the current state of the GRID license state machine
 * licenseExpiryTimestamp [OUT]
 *  License expiry time in seconds since UNIX epoch
 * licenseExpiryStatus [OUT]
 *  License expiry status
 * guestDriverVersion [OUT]
 *  This param specifies the driver version of the driver installed on the VM
 * guestDriverBranch [OUT]
 *  This param specifies the driver branch of the driver installed on the VM
 * vmName [OUT]
 *  This param stores the name assigned to VM (KVM only)
 * guestVmInfoState [OUT]
 *  This param stores the current state of guest dependent fields
 *
 */
typedef struct NVA081_GUEST_VM_INFO {
    NvU32               vmPid;
    VM_ID_TYPE          vmIdType;
    NvU32               guestOs;
    NvU32               migrationProhibited;
    NvU32               guestNegotiatedVgpuVersion;
    NvU32               frameRateLimit;
    NvBool              licensed;
    NvU32               licenseState;
    NvU32               licenseExpiryTimestamp;
    NvU8                licenseExpiryStatus;
    NV_DECLARE_ALIGNED(VM_ID guestVmId, 8);
    NvU8                guestDriverVersion[NVA081_VGPU_STRING_BUFFER_SIZE];
    NvU8                guestDriverBranch[NVA081_VGPU_STRING_BUFFER_SIZE];
    NvU8                vmName[NVA081_VM_NAME_SIZE];
    GUEST_VM_INFO_STATE guestVmInfoState;
} NVA081_GUEST_VM_INFO;

/*
 * NVA081_GUEST_VGPU_DEVICE
 *
 * This structure represents host vgpu device's (assigned to VM) information
 *
 * eccState [OUT]
 *  This parameter specifies the ECC state of the virtual GPU.
 *  One of NVA081_CTRL_ECC_STATE_xxx values.
 * bDriverLoaded [OUT]
 *  This parameter specifies whether driver is loaded on this particular vGPU.
 * swizzId [OUT]
 *  This param specifies the GPU Instance ID or Swizz ID
 * placementId [OUT]
 *  This param specifies the placement ID of heterogeneous timesliced vGPU instance.
 *  Otherwise it is NVA081_PLACEMENT_ID_INVALID.
 * vgpuDevName [OUT]
 *  This param specifies the VF BDF of the virtual GPU.
 *
 */
typedef struct NVA081_HOST_VGPU_DEVICE {
    NvU32  vgpuType;
    NvU32  vgpuDeviceInstanceId;
    NV_DECLARE_ALIGNED(NvU64 vgpuPciId, 8);
    NvU8   vgpuUuid[VM_UUID_SIZE];
    NvU8   vgpuDevName[VM_UUID_SIZE];
    NvU32  encoderCapacity;
    NV_DECLARE_ALIGNED(NvU64 fbUsed, 8);
    NvU32  eccState;
    NvBool bDriverLoaded;
    NvU32  swizzId;
    NvU32  placementId;
} NVA081_HOST_VGPU_DEVICE;

/* ECC state values */
#define NVA081_CTRL_ECC_STATE_UNKNOWN       0
#define NVA081_CTRL_ECC_STATE_NOT_SUPPORTED 1
#define NVA081_CTRL_ECC_STATE_DISABLED      2
#define NVA081_CTRL_ECC_STATE_ENABLED       3

/*
 * NVA081_VGPU_GUEST
 *
 * This structure represents a vGPU guest
 *
 */
typedef struct NVA081_VGPU_GUEST {
    NV_DECLARE_ALIGNED(NVA081_GUEST_VM_INFO guestVmInfo, 8);
    NV_DECLARE_ALIGNED(NVA081_HOST_VGPU_DEVICE vgpuDevice, 8);
} NVA081_VGPU_GUEST;

/*
 * NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS
 *
 * This structure represents the information of vGPU guest instances per pGpu
 *
 */
#define NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS {
    NvU32 vgpuType;
    NvU32 numVgpu;
    NV_DECLARE_ALIGNED(NVA081_VGPU_GUEST vgpuGuest[NVA081_MAX_VGPU_PER_PGPU], 8);
} NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_VGPU_TYPE_INFO
 *
 * This command fetches vGPU type info from RM.
 *
 * Parameters:
 *
 * vgpuType [IN]
 *  This parameter specifies the virtual GPU type for which vGPU info should be returned.
 *
 * vgpuTypeInfo [OUT]
 *  This parameter returns NVA081_CTRL_VGPU_INFO data for the vGPU type specified by vgpuType.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_VGPU_TYPE_INFO (0xa0810103) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS_MESSAGE_ID" */

/*
 * NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS
 *
 */
#define NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS {
    NvU32 vgpuType;
    NV_DECLARE_ALIGNED(NVA081_CTRL_VGPU_INFO vgpuTypeInfo, 8);
} NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS;

/*
 * NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS
 *      This structure represents supported/creatable vGPU types on a pGPU
 */
typedef struct NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS {
    /*
     * [IN] GPU Instance ID or Swizz ID
     */
    NvU32 gpuInstanceId;

    /*
     * [OUT] vGPU config state on a pGPU
     */
    NvU32 vgpuConfigState;

    /*
     * [OUT] Count of supported/creatable vGPU types on a pGPU
     */
    NvU32 numVgpuTypes;

    /*
     * [OUT] - Array of vGPU type ids supported/creatable on a pGPU
     */
    NvU32 vgpuTypes[NVA081_MAX_VGPU_TYPES_PER_PGPU];
} NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS;


/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_SUPPORTED_VGPU_TYPES
 *
 * This command fetches count and list of vGPU types supported on a pGpu from RM
 *
 * Parameters:
 *
 * numVgpuTypes [OUT]
 *  This parameter returns the number of vGPU types supported on this pGPU
 *
 * vgpuTypes [OUT]
 *  This parameter returns list of supported vGPUs types on this pGPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_SUPPORTED_VGPU_TYPES (0xa0810104) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_SUPPORTED_VGPU_TYPES_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_SUPPORTED_VGPU_TYPES_PARAMS_MESSAGE_ID (0x4U)

typedef NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS NVA081_CTRL_VGPU_CONFIG_GET_SUPPORTED_VGPU_TYPES_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_CREATABLE_VGPU_TYPES
 *
 * This command fetches count and list of vGPU types creatable on a pGpu from RM
 *
 * Parameters:
 *
 * numVgpuTypes [OUT]
 *  This parameter returns the number of vGPU types creatable on this pGPU
 *
 * vgpuTypes [OUT]
 *  This parameter returns list of creatable vGPUs types on this pGPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_CREATABLE_VGPU_TYPES (0xa0810105) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_VGPU_TYPES_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_VGPU_TYPES_PARAMS_MESSAGE_ID (0x5U)

typedef NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_VGPU_TYPES_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated subdevice/pGPU.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated subdevice object.
 *
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NVA081_NOTIFIERS value (see cla081.h for more details) and should
 *     not exceed one less NVA081_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NVA081_CTRL_SET_EVENT_NOTIFICATION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated subdevice object.
 *       NVA081_CTRL_SET_EVENT_NOTIFICATION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated subdevice object.
 *       NVA081_CTRL_SET_EVENT_NOTIFICATION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated system controller object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_EVENT_SET_NOTIFICATION (0xa0810106) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32 event;
    NvU32 action;
} NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS;


/* valid event action values */
#define NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE              (0x00000000)
#define NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE               (0x00000001)
#define NVA081_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT               (0x00000002)

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_UPDATE_PGPU_INFO
 *
 * This command register the GPU to Linux kernel's mdev module for vGPU on KVM.
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_UPDATE_PGPU_INFO                   (0xa0810109) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | 0x9" */

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_SET_VGPU_INSTANCE_ENCODER_CAPACITY
 *
 * This command is used to set vGPU instance's (represented by vgpuUuid) encoder Capacity.
 *
 *   vgpuUuid
 *     This parameter specifies the uuid of vGPU assigned to VM.
 *   encoderCapacity
 *     Encoder capacity value from 0 to 100. Value of 0x00 indicates encoder performance
 *     may be minimal for this GPU and software should fall back to CPU-based encode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_SET_VGPU_INSTANCE_ENCODER_CAPACITY (0xa0810110) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS_MESSAGE_ID" */

/*
 * NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS
 *
 * This structure represents  encoder capacity for vgpu instance.
 */
#define NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS_MESSAGE_ID (0x10U)

typedef struct NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS {
    NvU8  vgpuUuid[VM_UUID_SIZE];
    NvU32 encoderCapacity;
} NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_VGPU_FB_USAGE
 *
 * This command is used to get the FB usage of all vGPU instances running on a GPU.
 *
 *  vgpuCount
 *      This field specifies the number of vGPU devices for which FB usage is returned.
 *  vgpuFbUsage
 *      This is an array of type NVA081_VGPU_FB_USAGE, which contains a list of vGPUs
 *      and their corresponding FB usage in bytes;
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_VGPU_FB_USAGE (0xa0810111) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS_MESSAGE_ID" */

typedef struct NVA081_VGPU_FB_USAGE {
    NvU8 vgpuUuid[VM_UUID_SIZE];
    NV_DECLARE_ALIGNED(NvU64 fbUsed, 8);
} NVA081_VGPU_FB_USAGE;

/*
 * NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS
 *
 * This structure represents the FB usage information of vGPU instances running on a GPU.
 */
#define NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS_MESSAGE_ID (0x11U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS {
    NvU32 vgpuCount;
    NV_DECLARE_ALIGNED(NVA081_VGPU_FB_USAGE vgpuFbUsage[NVA081_MAX_VGPU_PER_PGPU], 8);
} NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP
 *
 * This command is used to query whether pGPU is live migration capable or not.
 *
 *  bMigrationCap
 *      Set to NV_TRUE if pGPU is migration capable.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_REQUEST
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP (0xa0810112) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS_MESSAGE_ID" */

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS_MESSAGE_ID (0x12U)

typedef struct NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS {
    NvBool bMigrationCap;
} NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_HOST_FB_RESERVATION
 *
 * This command is used to get the host FB requirements
 *
 *  hostReservedFb [OUT]
 *      Amount of FB reserved for the host
 *  eccAndPrReservedFb [OUT]
 *      Amount of FB reserved for the ecc and page retirement
 *  totalReservedFb [OUT]
 *      Total FB reservation
 *  vgpuTypeId [IN]
 *      The Type ID for VGPU profile
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_HOST_FB_RESERVATION (0xa0810113) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS_MESSAGE_ID (0x13U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 hostReservedFb, 8);
    NV_DECLARE_ALIGNED(NvU64 eccAndPrReservedFb, 8);
    NV_DECLARE_ALIGNED(NvU64 totalReservedFb, 8);
    NvU32 vgpuTypeId;
} NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_PGPU_METADATA_STRING
 *
 * This command is used to get the pGpu metadata string.
 *
 * pGpuString
 *     String holding pGpu Metadata
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_PGPU_METADATA_STRING (0xa0810114) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS_MESSAGE_ID (0x14U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS {
    NvU8 pGpuString[NVA081_PGPU_METADATA_STRING_SIZE];
} NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS;

#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT (0xa0810115) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS_MESSAGE_ID (0x15U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS {
    NvBool doorbellEmulationEnabled;
} NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_FREE_SWIZZID
 *
 * This command is used to get free swizzid from RM
 *
 * gpuPciId [IN]
 *  This param specifies the PCI device ID of VF on which VM is running
 *
 * vgpuTypeId [IN]
 *  This param specifies the Type ID for VGPU profile
 *
 * swizzId [OUT]
 *  This param specifies the GPU Instance ID or Swizz ID
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_FREE_SWIZZID (0xa0810116) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS_MESSAGE_ID (0x16U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS {
    NvU32 gpuPciId;
    NvU32 vgpuTypeId;
    NvU32 swizzId;
} NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS;

/*
 * NVA081_CTRL_CMD_PGPU_GET_MULTI_VGPU_SUPPORT_INFO
 *
 * This command is used to get multi vGPU related info for the physical GPU.
 *
 * fractionalmultiVgpuSupported [OUT]
 *  This param specifies whether fractional multi-vGPU is supported
 *
 * heterogeneousTimesliceProfilesSupported [OUT]
 *  This param specifies whether concurrent execution of timesliced vGPU profiles of differing types is supported
 *
 * heterogeneousTimesliceSizesSupported [OUT]
 *  This param specifies whether concurrent execution of timesliced vGPU profiles of differing framebuffer sizes is supported
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA081_CTRL_CMD_PGPU_GET_MULTI_VGPU_SUPPORT_INFO (0xa0810117) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS_MESSAGE_ID (0x17U)

typedef struct NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS {
    NvU32 fractionalmultiVgpuSupported;
    NvU32 heterogeneousTimesliceProfilesSupported;
    NvU32 heterogeneousTimesliceSizesSupported;
} NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS;

/*
 * NVA081_CTRL_CMD_GET_VGPU_DRIVER_CAPS
 *
 * This command is used to get vGPU driver capabilities.
 *
 * heterogeneousMultiVgpuSupported [OUT]
 *  This param specifies whether heterogeneous multi-vGPU is supported
 * warmUpdateSupported [OUT]
 *  This param specifies FSR / warm driver update operation is supported
 *  ie. supports FSR and warm update of vGPU host driver without terminating the running guest VM
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA081_CTRL_CMD_GET_VGPU_DRIVER_CAPS (0xa0810118) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS_MESSAGE_ID (0x18U)

typedef struct NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS {
    NvU32 heterogeneousMultiVgpuSupported;
    NvU32 warmUpdateSupported;
} NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_SET_PGPU_INFO
 *
 * This command is to set pgpu info
 *
 * fractionalMultiVgpu [IN]
 *  This param specifies the fractional multivgpu is enabled or disabled on GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_SET_PGPU_INFO (0xa0810119) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS_MESSAGE_ID (0x19U)

typedef struct NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS {
    NvU32 fractionalMultiVgpu;
} NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_VALIDATE_SWIZZID
 *
 * This command is used to vallidate input swizzid from RM
 *
 * vgpuTypeId [IN]
 *  This param specifies the Type ID for VGPU profile
 *
 * swizzId [IN]
 *  This param specifies the GPU Instance ID or Swizz ID
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_VALIDATE_SWIZZID (0xa081011a) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS_MESSAGE_ID (0x1aU)

typedef struct NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS {
    NvU32 vgpuTypeId;
    NvU32 swizzId;
} NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_UPDATE_PLACEMENT_INFO
 *
 * This command is used to get a placement Id from RM for timesliced
 * heterogeneous or homogeneous vGPUs. The physical GPU shall be used
 * in either heterogeneous or homogeneous mode at a time.
 *
 * isHeterogeneousEnabled [OUT]
 *  This param specific whether timesliced heterogeneous vGPU is enabled
 *  for different FB sized profiles.
 *
 * placementId [IN / OUT]
 *  This param specifies the input placement ID provided by hypervisor
 *  or output placement ID reserved by RM for vGPU type ID.
 *
 * vgpuTypeId [IN]
 *  This param specifies the Type ID for VGPU profile
 *
 * vgpuDevName [IN]
 *  This param specifies the VF BDF of the virtual GPU.
 *
 * guestFbLength [OUT]
 *  This param specifies the FB size assigned to the VM.
 *
 * guestFbOffset [OUT]
 *  This param specifies the starting FB offset assigned to the VM.
 *
 * gspHeapOffset [OUT]
 *  This param specifies the heap offset of gsp vgpu task.
 *
 * guestBar1PFOffset [OUT]
 *  This param specifies the starting PF BAR1 offset assigned to the VM.
 *  Only applicable for the legacy GPUs. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_UPDATE_PLACEMENT_INFO (0xa081011b) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS_MESSAGE_ID (0x1bU)

typedef struct NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS {
    NvBool isHeterogeneousEnabled;
    NvU16  placementId;
    NvU32  vgpuTypeId;
    NvU8   vgpuDevName[VM_UUID_SIZE];
    NV_DECLARE_ALIGNED(NvU64 guestFbLength, 8);
    NV_DECLARE_ALIGNED(NvU64 guestFbOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 gspHeapOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 guestBar1PFOffset, 8);
} NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS
 *
 * This command fetches count and list of creatable vGPU placements
 *
 * Parameters:
 *
 * gpuInstanceId [IN]
 *  GPU Instance ID or Swizz ID
 *
 * vgpuTypeId [IN]
 *  The client provided vGPU type ID
 *
 * placementSize [OUT]
 *  The number of placement slots occupied by the vGPU type
 *
 * count [OUT]
 *  This parameter returns the number of placements supported for the vGPU
 *
 * placementIds [OUT]
 *  Array of creatable placement IDs
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS (0xa081011c) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS_MESSAGE_ID (0x1cU)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS {
    NvU32 gpuInstanceId;
    NvU32 vgpuTypeId;
    NvU32 placementSize;
    NvU32 count;
    NvU32 placementIds[NVA081_MAX_VGPU_PER_PGPU];
} NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS;

/*
 * NVA081_CTRL_CMD_PGPU_GET_VGPU_STREAMING_CAPABILITY
 *
 * This command is used to get streaming capability for the physical GPU.
 *
 * streamingCapability [OUT]
 *  This param specifies whether vGPU profiles on the GPU supports migration data streaming
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_PGPU_GET_VGPU_STREAMING_CAPABILITY (0xa081011d) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS_MESSAGE_ID (0x1dU)

typedef struct NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS {
    NvBool streamingCapability;
} NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS;

/* vGPU capabilities */
#define NVA081_CTRL_VGPU_CAPABILITY_MINI_QUARTER_GPU                 0
#define NVA081_CTRL_VGPU_CAPABILITY_COMPUTE_MEDIA_ENGINE_GPU         1
#define NVA081_CTRL_VGPU_CAPABILITY_WARM_UPDATE                      2
#define NVA081_CTRL_VGPU_CAPABILITY_DEVICE_STREAMING                 3
#define NVA081_CTRL_VGPU_CAPABILITY_READ_DEVICE_BUFFER_BW            4
#define NVA081_CTRL_VGPU_CAPABILITY_WRITE_DEVICE_BUFFER_BW           5
#define NVA081_CTRL_VGPU_CAPABILITY_HETEROGENEOUS_TIMESLICE_SIZES    6
#define NVA081_CTRL_VGPU_CAPABILITY_HETEROGENEOUS_TIMESLICE_PROFILES 7
#define NVA081_CTRL_VGPU_CAPABILITY_FRACTIONAL_MULTI_VGPU            8
#define NVA081_CTRL_VGPU_CAPABILITY_HOMOGENEOUS_PLACEMENT_ID         9

#define NVA081_CTRL_VGPU_CAPABILITY_MIG_TIMESLICING_SUPPORTED        10
#define NVA081_CTRL_VGPU_CAPABILITY_MIG_TIMESLICING_MODE_ENABLED     11



/*
 * NVA081_CTRL_CMD_VGPU_SET_CAPABILITY
 *
 * This command is used to set vGPU capability for the physical GPU.
 *
 * capability [IN]
 *  This param specifies the requested capabiity of the device that is to be set
 *  One of NVA081_CTRL_VGPU_CAPABILITY* values
 *
 * state [IN]
 *  This param specifies the state of the capability
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_SET_CAPABILITY                          (0xa081011e) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS_MESSAGE_ID (0x1eU)

typedef struct NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS {
    NvU32  capability;
    NvBool state;
} NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_GET_CAPABILITY
 *
 * This command is to get state of vGPU capability for the physical GPU.
 *
 * capability [IN]
 *  This param specifies the requested capabiity of the device that is to be set
 *  One of NVA081_CTRL_VGPU_CAPABILITY* values
 *
 * state [OUT]
 *  This param specifies the state of the capability
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_GET_CAPABILITY (0xa081011f) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS_MESSAGE_ID (0x1fU)

typedef struct NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS {
    NvU32 capability;
    NvU32 state;
} NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_SET_VM_NAME
 *
 * This command is to set VM name for the host.
 *
 * vmIdType [IN]
 *  This param provides the guest VM ID type based on the host.
 *
 * guestVmId [IN]
 *  This param provides the guest VM indentifier to RM based on the host.
 *
 * vmName [IN]
 *  This param provides the VM name of the vGPU device attached
 *  to the above mentioned host VM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA081_CTRL_CMD_VGPU_SET_VM_NAME (0xa0810120) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS_MESSAGE_ID (0x20U)

typedef struct NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS {
    VM_ID_TYPE vmIdType;
    NV_DECLARE_ALIGNED(VM_ID guestVmId, 8);
    NvU8       vmName[NVA081_VM_NAME_SIZE];
} NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_GET_BAR_INFO
 *
 * This command is to get the bar info for a vGPU.
 *
 * gpuPciId [IN]
 *  This param specifies the PCI device ID of VF on which VM is running
 *
 * vgpuName [IN]
 *  This param provides the vGPU device name to RM.
 *
 * configParams [IN]
 *  This param provides the vGPU config params to RM
 *
 * barSizes [OUT]
 *  This param provides the BAR size for each region index of the device
 *
 * sparseOffsets [OUT]
 *  This param provides the offset of each sparse mmap region in BAR0
 *
 * sparseSizes [OUT]
 *  This param provides the size of each sparse mmap region in BAR0
 *
 * sparseCount [OUT]
 *  This param provides the number of sparse mmap regions in BAR0
 *
 * isBar064bit [OUT]
 *  This param provides whether the BAR0 is 64bit of the vGPU device
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA081_CTRL_CMD_VGPU_GET_BAR_INFO (0xa0810121) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_GET_BAR_INFO_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_GET_BAR_INFO_PARAMS_MESSAGE_ID (0x21U)

typedef struct NVA081_CTRL_VGPU_GET_BAR_INFO_PARAMS {
    NvU32  gpuPciId;
    NvU8   vgpuName[VM_UUID_SIZE];
    NvU8   configParams[NVA081_CONFIG_PARAMS_MAX_LENGTH];
    NV_DECLARE_ALIGNED(NvU64 barSizes[NVA081_MAX_BAR_REGION_COUNT], 8);
    NV_DECLARE_ALIGNED(NvU64 sparseOffsets[NVA081_MAX_SPARSE_REGION_COUNT], 8);
    NV_DECLARE_ALIGNED(NvU64 sparseSizes[NVA081_MAX_SPARSE_REGION_COUNT], 8);
    NvU32  sparseCount;
    NvBool isBar064bit;
} NVA081_CTRL_VGPU_GET_BAR_INFO_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH
 *
 * This command is to get the migration bandwidth of the physical GPU.
 *
 * migrationBandwidth [OUT]
 *  This param specifies the migration bandwidth of GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH (0xa0810122) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS_MESSAGE_ID (0x22U)

typedef struct NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS {
    NvU32 migrationBandwidth;
} NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS;

/*
 * NVA081_CTRL_CMD_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE
 *
 * This command enumerates list of vGPU guest instances per GPU instance
 *
 * gpuInstanceId [IN]
 *  This parameter specifies the GPU Instance Id or Swizz Id
 *
 * numVgpu [OUT]
 *  This parameter specifies the number of virtual GPUs created on a GPU instance
 *
 * vgpuInstanceIds [OUT]
 *  This parameter specifies an array of vGPU type ids active on a GPU instance
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA081_CTRL_CMD_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE (0xa0810124) /* finn: Evaluated from "(FINN_NVA081_VGPU_CONFIG_VGPU_CONFIG_INTERFACE_ID << 8) | NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS_MESSAGE_ID" */

#define NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS_MESSAGE_ID (0x24U)

typedef struct NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS {
    NvU32 gpuInstanceId;
    NvU32 numVgpu;
    NvU32 vgpuInstanceIds[NVA081_MAX_VGPU_PER_GI];
} NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS;

/* _ctrlA081vgpuconfig_h_ */
