/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080vgpumgrinternal.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"
#include "ctrl/ctrla081.h"

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK
 *
 * This command is used to bootload GSP VGPU plugin task.
 * Can be called only with SR-IOV and with VGPU_GSP_PLUGIN_OFFLOAD feature.
 *
 * dbdf                        - domain (31:16), bus (15:8), device (7:3), function (2:0)
 * gfid                        - Gfid
 * vgpuType                    - The Type ID for VGPU profile
 * vmPid                       - Plugin process ID of vGPU guest instance
 * swizzId                     - SwizzId
 * numChannels                 - Number of channels
 * numPluginChannels           - Number of plugin channels
 * bDisableSmcPartitionRestore - If set to true, SMC default execution partition
 *                               save/restore will not be done in host-RM
 * guestFbPhysAddrList         - list of VMMU segment aligned physical address of guest FB memory
 * guestFbLengthList           - list of guest FB memory length in bytes
 * pluginHeapMemoryPhysAddr    - plugin heap memory offset
 * pluginHeapMemoryLength      - plugin heap memory length in bytes
 * migRmHeapMemoryPhysAddr     - Mig rm heap memory region's physical offset.
 * migRmHeapMemoryLength       - Mig rm heap memory length in bytes
 * bDeviceProfilingEnabled     - If set to true, profiling is allowed
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK (0x20804001) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MAX_VMMU_SEGMENTS                                   384

/* Must match NV2080_ENGINE_TYPE_LAST from cl2080.h */
#define NV2080_GPU_MAX_ENGINES                                          0x54

#define NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS {
    NvU32  dbdf;
    NvU32  gfid;
    NvU32  vgpuType;
    NvU32  vmPid;
    NvU32  swizzId;
    NvU32  numChannels;
    NvU32  numPluginChannels;
    NvU32  chidOffset[NV2080_GPU_MAX_ENGINES];
    NvBool bDisableDefaultSmcExecPartRestore;
    NvU32  numGuestFbSegments;
    NV_DECLARE_ALIGNED(NvU64 guestFbPhysAddrList[NV2080_CTRL_MAX_VMMU_SEGMENTS], 8);
    NV_DECLARE_ALIGNED(NvU64 guestFbLengthList[NV2080_CTRL_MAX_VMMU_SEGMENTS], 8);
    NV_DECLARE_ALIGNED(NvU64 pluginHeapMemoryPhysAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 pluginHeapMemoryLength, 8);
    NV_DECLARE_ALIGNED(NvU64 ctrlBuffOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 initTaskLogBuffOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 initTaskLogBuffSize, 8);
    NV_DECLARE_ALIGNED(NvU64 vgpuTaskLogBuffOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 vgpuTaskLogBuffSize, 8);
    NV_DECLARE_ALIGNED(NvU64 kernelLogBuffOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 kernelLogBuffSize, 8);
    NV_DECLARE_ALIGNED(NvU64 migRmHeapMemoryPhysAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 migRmHeapMemoryLength, 8);
    NvBool bDeviceProfilingEnabled;
} NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK
 *
 * This command is used to shutdown GSP VGPU plugin task.
 * Can be called only with SR-IOV and with VGPU_GSP_PLUGIN_OFFLOAD feature.
 *
 * gfid                        - Gfid
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK (0x20804002) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS {
    NvU32 gfid;
} NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE
 *
 * This command is used to add a new vGPU config to the pGPU in physical RM.
 * Unlike NVA081_CTRL_CMD_VGPU_CONFIG_SET_INFO, it does no validation
 * and is only to be used internally.
 *
 * discardVgpuTypes [IN]
 *  This parameter specifies if existing vGPU configuration should be
 *  discarded for given pGPU
 *
 * vgpuInfoCount [IN]
 *   This parameter specifies the number of entries of virtual GPU type
*    information
 *
 * vgpuInfo [IN]
 *   This parameter specifies virtual GPU type information
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE (0x20804003) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS {
    NvBool discardVgpuTypes;
    NvU32  vgpuInfoCount;
    NV_DECLARE_ALIGNED(NVA081_CTRL_VGPU_INFO vgpuInfo[NVA081_MAX_VGPU_TYPES_PER_PGPU], 8);
} NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS;

/*
 * NV2080_GUEST_VM_INFO
 *
 * This structure represents vGPU guest's (VM's) information
 *
 * vmPid [OUT]
 *  This param specifies the vGPU plugin process ID
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
 * guestVmInfoState [OUT]
 *  This param stores the current state of guest dependent fields
 *
 */
typedef struct NV2080_GUEST_VM_INFO {
    NvU32               vmPid;
    NvU32               guestOs;
    NvU32               migrationProhibited;
    NvU32               guestNegotiatedVgpuVersion;
    NvU32               frameRateLimit;
    NvBool              licensed;
    NvU32               licenseState;
    NvU32               licenseExpiryTimestamp;
    NvU8                licenseExpiryStatus;
    NvU8                guestDriverVersion[NVA081_VGPU_STRING_BUFFER_SIZE];
    NvU8                guestDriverBranch[NVA081_VGPU_STRING_BUFFER_SIZE];
    GUEST_VM_INFO_STATE guestVmInfoState;
} NV2080_GUEST_VM_INFO;

/*
 * NV2080_GUEST_VGPU_DEVICE
 *
 * This structure represents host vgpu device's (assigned to VM) information
 *
 * gfid [OUT]
 *  This parameter specifies the gfid of vGPU assigned to VM.
 * vgpuPciId [OUT]
 *  This parameter specifies vGPU PCI ID
 * vgpuDeviceInstanceId [OUT]
 *  This paramter specifies the vGPU device instance per VM to be used for supporting
 *  multiple vGPUs per VM.
 * fbUsed [OUT]
 *  This parameter specifies FB usage in bytes
 * eccState [OUT]
 *  This parameter specifies the ECC state of the virtual GPU.
 *  One of NVA081_CTRL_ECC_STATE_xxx values.
 * bDriverLoaded [OUT]
 *  This parameter specifies whether driver is loaded on this particular vGPU.
 *
 */
typedef struct NV2080_HOST_VGPU_DEVICE {
    NvU32  gfid;
    NV_DECLARE_ALIGNED(NvU64 vgpuPciId, 8);
    NvU32  vgpuDeviceInstanceId;
    NV_DECLARE_ALIGNED(NvU64 fbUsed, 8);
    NvU32  encoderCapacity;
    NvU32  eccState;
    NvBool bDriverLoaded;
} NV2080_HOST_VGPU_DEVICE;

/*
 * NV2080_VGPU_GUEST
 *
 * This structure represents a vGPU guest
 *
 */
typedef struct NV2080_VGPU_GUEST {
    NV2080_GUEST_VM_INFO guestVmInfo;
    NV_DECLARE_ALIGNED(NV2080_HOST_VGPU_DEVICE vgpuDevice, 8);
} NV2080_VGPU_GUEST;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU
 *
 * This command enumerates list of vGPU guest instances per pGpu
 *
 * numVgpu [OUT]
 *  This parameter specifies the number of virtual GPUs created on this physical GPU
 *
 * vgpuGuest [OUT]
 *  This parameter specifies an array containing guest vgpu's information for
 *  all vGPUs created on this physical GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU (0x20804004) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS {
    NvU32 numVgpu;
    NV_DECLARE_ALIGNED(NV2080_VGPU_GUEST vgpuGuest[NVA081_MAX_VGPU_PER_PGPU], 8);
} NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO
 *
 * This command is used clear guest vm info. It should be used when
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE
 * is called with NVA081_NOTIFIERS_EVENT_VGPU_GUEST_DESTROYED state.
 *
 * gfid [IN]
 *  This parameter specifies the gfid of vGPU assigned to VM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO (0x20804005) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS {
    NvU32 gfid;
} NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE
 *
 * This command is used to get the FB usage of all vGPU instances running on a GPU.
 *
 * vgpuCount [OUT]
 *  This field specifies the number of vGPU devices for which FB usage is returned.
 * vgpuFbUsage [OUT]
 *  This is an array of type NV2080_VGPU_FB_USAGE, which contains a list of vGPU gfid
 *  and their corresponding FB usage in bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE (0x20804006) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS_MESSAGE_ID" */

typedef struct NV2080_VGPU_FB_USAGE {
    NvU32 gfid;
    NV_DECLARE_ALIGNED(NvU64 fbUsed, 8);
} NV2080_VGPU_FB_USAGE;

#define NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS {
    NvU32 vgpuCount;
    NV_DECLARE_ALIGNED(NV2080_VGPU_FB_USAGE vgpuFbUsage[NVA081_MAX_VGPU_PER_PGPU], 8);
} NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY
 *
 * This command is used to set vGPU instance's (represented by gfid) encoder Capacity.
 *
 * gfid [IN]
 *  This parameter specifies the gfid of vGPU assigned to VM.
 * encoderCapacity [IN]
 *  Encoder capacity value from 0 to 100. Value of 0x00 indicates encoder performance
 *  may be minimal for this GPU and software should fall back to CPU-based encode.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY (0x20804007) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS {
    NvU32 gfid;
    NvU32 encoderCapacity;
} NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP
 *
 * This command is used to cleanup all the GSP VGPU plugin task allocated resources after its shutdown.
 * Can be called only with SR-IOV and with VGPU_GSP_PLUGIN_OFFLOAD feature.
 *
 * gfid [IN]
 *  This parameter specifies the gfid of vGPU assigned to VM.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP (0x20804008) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS {
    NvU32 gfid;
} NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS;

#define NV2080_CTRL_MAX_NVU32_TO_CONVERTED_STR_LEN             8
#define NV2080_CTRL_MAX_GPC_COUNT                              32

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING
 *
 * Reroutes kvgpumgrGetPgpuFSEncoding to vgpuMgrGetPgpuFSEncoding.
 *
 * pgpuString [OUT]
 *  Resulting PGPU string
 * pgpuStringSize
 *  PGPU string size
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING (0x20804009) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS {
    NvU8  pgpuString[NVA081_PGPU_METADATA_STRING_SIZE];
    NvU32 pgpuStringSize;
} NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT
 *
 * Reroutes kvgpumgrCheckPgpuMigrationSupport to vgpuMgrCheckPgpuMigrationSupport.
 *
 * bIsMigrationSupported [OUT]
 *  Resulting status of the migration support
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT (0x2080400a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS {
    NvBool bIsMigrationSupported;
} NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG
 *
 * Sets vgpu manager parameters. This control is used after GSP initialization.
 *
 * bSupportHeterogeneousTimeSlicedVgpuTypes [IN]
 *  Enable/disable heterogeneous time-sliced vgpu types
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG (0x2080400b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS {
    NvBool bSupportHeterogeneousTimeSlicedVgpuTypes;
} NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS;

/*
 * NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT
 *
 * Reroutes NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_FREE_STATES to GSP RM to enable
 * kernel clients to utilize NVA082_CTRL_CMD_HOST_VGPU_DEVICE_FREE_STATES.
 *
 * gfid [IN]
 *   This parameter specifies the gfid of vGPU assigned to VM.
 * flags [IN]
 *   Specifies what component of HostVgpuDevice to free.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_FREE_STATES (0x2080400c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_VGPU_MGR_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS {
    NvU32 gfid;
    NvU32 flags;
} NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS;

/* _ctrl2080vgpumgrinternal_h_ */
