/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000gpu.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrlxxxx.h"
#include "nvlimits.h"

/* NV01_ROOT (client) GPU control commands and parameters */

/*
 * NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS
 *
 * This command returns a table of attached gpuId values.
 * The table is NV0000_CTRL_GPU_MAX_ATTACHED_GPUS entries in size.
 *
 *   gpuIds[]
 *     This parameter returns the table of attached GPU IDs.
 *     The GPU ID is an opaque platform-dependent value that can be used
 *     with the NV0000_CTRL_CMD_GPU_GET_ID_INFO command to retrieve
 *     additional information about the GPU.  The valid entries in gpuIds[]
 *     are contiguous, with a value of NV0000_CTRL_GPU_INVALID_ID indicating
 *     the invalid entries.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS (0x201U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_MAX_ATTACHED_GPUS    32U
#define NV0000_CTRL_GPU_INVALID_ID           (0xffffffffU)

#define NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_GPU_MAX_ATTACHED_GPUS];
} NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS;

/*
 * Deprecated. Please use NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2 instead.
 */
#define NV0000_CTRL_CMD_GPU_GET_ID_INFO (0x202U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_ID_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_MAX_SZNAME      128U

#define NV0000_CTRL_NO_NUMA_NODE        (-1)

#define NV0000_CTRL_GPU_GET_ID_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_GPU_GET_ID_INFO_PARAMS {
    NvU32 gpuId;
    NvU32 gpuFlags;
    NvU32 deviceInstance;
    NvU32 subDeviceInstance;
    NV_DECLARE_ALIGNED(NvP64 szName, 8);
    NvU32 sliStatus;
    NvU32 boardId;
    NvU32 gpuInstance;
    NvS32 numaId;
} NV0000_CTRL_GPU_GET_ID_INFO_PARAMS;

#define NV0000_CTRL_SLI_STATUS_OK                (0x00000000U)
#define NV0000_CTRL_SLI_STATUS_OS_NOT_SUPPORTED  (0x00000002U)
#define NV0000_CTRL_SLI_STATUS_GPU_NOT_SUPPORTED (0x00000040U)
#define NV0000_CTRL_SLI_STATUS_INVALID_GPU_COUNT (0x00000001U)

/*
 * NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2
 * This command returns GPU instance information for the specified GPU.
 *
 *   [in] gpuId
 *     This parameter should specify a valid GPU ID value.  If there
 *     is no GPU present with the specified ID, a status of
 *     NV_ERR_INVALID_ARGUMENT is returned.
 *   [out] gpuFlags
 *     This parameter returns various flags values for the specified GPU.
 *     Valid flag values include:
 *       NV0000_CTRL_GPU_ID_INFO_IN_USE
 *         When true this flag indicates there are client references
 *         to the GPU in the form of device class instantiations (see
 *         NV01_DEVICE or NV03_DEVICE descriptions for details).
 *       NV0000_CTRL_GPU_ID_INFO_LINKED_INTO_SLI_DEVICE
 *         When true this flag indicates the GPU is linked into an
 *         active SLI device.
 *       NV0000_CTRL_GPU_ID_INFO_MOBILE
 *         When true this flag indicates the GPU is a mobile GPU.
 *       NV0000_CTRL_GPU_ID_BOOT_MASTER
 *         When true this flag indicates the GPU is the boot master GPU.
 *       NV0000_CTRL_GPU_ID_INFO_SOC
 *         When true this flag indicates the GPU is part of a
 *         System-on-Chip (SOC).
 *       NV0000_CTRL_GPU_ID_INFO_ATS_ENABLED
 *         When ATS is enabled on the system.
 *   [out] deviceInstance
 *     This parameter returns the broadcast device instance number associated
 *     with the specified GPU.  This value can be used to instantiate
 *     a broadcast reference to the GPU using the NV01_DEVICE classes.
 *   [out] subDeviceInstance
 *     This parameter returns the unicast subdevice instance number
 *     associated with the specified GPU.  This value can be used to
 *     instantiate a unicast reference to the GPU using the NV20_SUBDEVICE
 *     classes.
 *   [out] sliStatus
 *     This parameters returns the SLI status for the specified GPU.
 *     Legal values for this member are described by NV0000_CTRL_SLI_STATUS.
 *   [out] boardId
 *     This parameter returns the board ID value with which the
 *     specified GPU is associated.  Multiple GPUs can share the
 *     same board ID in multi-GPU configurations.
 *   [out] gpuInstance
 *     This parameter returns the GPU instance number for the specified GPU.
 *     GPU instance numbers are assigned in bus-probe order beginning with
 *     zero and are limited to one less the number of GPUs in the system.
 *   [out] numaId
 *     This parameter returns the ID of NUMA node for the specified GPU or
 *     the subscribed MIG partition when MIG is enabled.
 *     In case there is no NUMA node, NV0000_CTRL_NO_NUMA_NODE is returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */



#define NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2       (0x205U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS {
    NvU32 gpuId;
    NvU32 gpuFlags;
    NvU32 deviceInstance;
    NvU32 subDeviceInstance;
    NvU32 sliStatus;
    NvU32 boardId;
    NvU32 gpuInstance;
    NvS32 numaId;
} NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS;


/* valid flags values */
#define NV0000_CTRL_GPU_ID_INFO_IN_USE                             0:0
#define NV0000_CTRL_GPU_ID_INFO_IN_USE_FALSE                 (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_IN_USE_TRUE                  (0x00000001U)
#define NV0000_CTRL_GPU_ID_INFO_LINKED_INTO_SLI_DEVICE             1:1
#define NV0000_CTRL_GPU_ID_INFO_LINKED_INTO_SLI_DEVICE_FALSE (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_LINKED_INTO_SLI_DEVICE_TRUE  (0x00000001U)
#define NV0000_CTRL_GPU_ID_INFO_MOBILE                             2:2
#define NV0000_CTRL_GPU_ID_INFO_MOBILE_FALSE                 (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_MOBILE_TRUE                  (0x00000001U)
#define NV0000_CTRL_GPU_ID_INFO_BOOT_MASTER                        3:3
#define NV0000_CTRL_GPU_ID_INFO_BOOT_MASTER_FALSE            (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_BOOT_MASTER_TRUE             (0x00000001U)


#define NV0000_CTRL_GPU_ID_INFO_SOC                                5:5
#define NV0000_CTRL_GPU_ID_INFO_SOC_FALSE                    (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_SOC_TRUE                     (0x00000001U)
#define NV0000_CTRL_GPU_ID_INFO_ATS_ENABLED                        6:6
#define NV0000_CTRL_GPU_ID_INFO_ATS_ENABLED_FALSE            (0x00000000U)
#define NV0000_CTRL_GPU_ID_INFO_ATS_ENABLED_TRUE             (0x00000001U)

/*
 * NV0000_CTRL_CMD_GPU_GET_INIT_STATUS
 *
 * This command returns the initialization status for the specified GPU, and
 * will return NV_ERR_INVALID_STATE if called prior to GPU
 * initialization.
 *
 *   gpuId
 *     This parameter should specify a valid GPU ID value.  If there
 *     is no GPU present with the specified ID, a status of
 *     NV_ERR_INVALID_ARGUMENT is returned.
 *   status
 *     This parameter returns the status code identifying the initialization
 *     state of the GPU. If this parameter has the value NV_OK,
 *     then no errors were detected during GPU initialization. Otherwise, this
 *     parameter specifies the top-level error that was detected during GPU
 *     initialization. Note that a value of NV_OK only means that
 *     no errors were detected during the actual GPU initialization, and other
 *     errors may have occurred that prevent the GPU from being attached or
 *     accessible via the NV01_DEVICE or NV20_SUBDEVICE classes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0000_CTRL_CMD_GPU_GET_INIT_STATUS                  (0x203U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS {
    NvU32 gpuId;
    NvU32 status;
} NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_GET_DEVICE_IDS
 *
 * This command returns a mask of valid device IDs.  These device IDs
 * can be used to instantiate the NV01_DEVICE_0 class (see NV01_DEVICE_0
 * for more information).
 *
 *   deviceIds
 *     This parameter returns the mask of valid device IDs.  Each enabled bit
 *     in the mask corresponds to a valid device instance.  Valid device
 *     instances can be used to initialize the NV0080_ALLOC_PARAMETERS
 *     structure when using NvRmAlloc to instantiate device handles.  The
 *     number of device IDs will not exceed NV_MAX_DEVICES in number.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0000_CTRL_CMD_GPU_GET_DEVICE_IDS (0x204U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS {
    NvU32 deviceIds;
} NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS;



/*
 * NV0000_CTRL_CMD_GPU_GET_PROBED_IDS
 *
 * This command returns a table of probed gpuId values.
 * The table is NV0000_CTRL_GPU_MAX_PROBED_GPUS entries in size.
 *
 *   gpuIds[]
 *     This parameter returns the table of probed GPU IDs.
 *     The GPU ID is an opaque platform-dependent value that can
 *     be used with the NV0000_CTRL_CMD_GPU_ATTACH_IDS and
 *     NV0000_CTRL_CMD_GPU_DETACH_ID commands to attach and detach
 *     the GPU.
 *     The valid entries in gpuIds[] are contiguous, with a value
 *     of NV0000_CTRL_GPU_INVALID_ID indicating the invalid entries.
 *   excludedGpuIds[]
 *     This parameter returns the table of excluded GPU IDs.
 *     An excluded GPU ID is an opaque platform-dependent value that
 *     can be used with NV0000_CTRL_CMD_GPU_GET_PCI_INFO and
 *     NV0000_CTRL_CMD_GPU_GET_UUID_INFO.
 *     The valid entries in excludedGpuIds[] are contiguous, with a value
 *     of NV0000_CTRL_GPU_INVALID_ID indicating the invalid entries.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_GPU_GET_PROBED_IDS (0x214U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_MAX_PROBED_GPUS    NV_MAX_DEVICES

#define NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_GPU_MAX_PROBED_GPUS];
    NvU32 excludedGpuIds[NV0000_CTRL_GPU_MAX_PROBED_GPUS];
} NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_GET_PCI_INFO
 *
 * This command takes a gpuId and returns PCI bus information about
 * the device. If the OS does not support returning PCI bus
 * information, this call will return NV_ERR_NOT_SUPPORTED
 *
 *   gpuId
 *     This parameter should specify a valid GPU ID value.  If there
 *     is no GPU present with the specified ID, a status of
 *     NV_ERR_INVALID_ARGUMENT is returned.
 *
 *   domain
 *     This parameter returns the PCI domain of the GPU.
 *
 *   bus
 *     This parameter returns the PCI bus of the GPU.
 *
 *   slot
 *     This parameter returns the PCI slot of the GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_GPU_GET_PCI_INFO (0x21bU) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS_MESSAGE_ID (0x1BU)

typedef struct NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS {
    NvU32 gpuId;
    NvU32 domain;
    NvU16 bus;
    NvU16 slot;
} NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_ATTACH_IDS
 *
 * This command attaches the GPUs with the gpuIds matching those in
 * the table provided by the client.
 * The table is NV0000_CTRL_GPU_MAX_PROBED_GPUS entries in size.
 *
 *   gpuIds[]
 *     This parameter holds the table of gpuIds to attach. At least
 *     one gpuId must be specified; clients may use the special
 *     gpuId value NV0000_CTRL_GPU_ATTACH_ALL_PROBED_IDS to indicate
 *     that all probed GPUs are to be attached.
 *     The entries in gpuIds[] must be contiguous, with a value of
 *     NV0000_CTRL_GPU_INVALID_ID to indicate the first invalid
 *     entry.
 *     If one or more of the gpuId values do not specify a GPU found
 *     in the system, the NV_ERR_INVALID_ARGUMENT error
 *     status is returned.
 *
 *   failedId
 *     If NV0000_CTRL_GPU_ATTACH_ALL_PROBED_IDS is specified and
 *     a GPU cannot be attached, the NV0000_CTRL_CMD_GPU_ATTACH_IDS
 *     command returns an error code and saves the failing GPU's
 *     gpuId in this field.
 *
 * If a table of gpuIds is provided, these gpuIds will be validated
 * against the RM's table of probed gpuIds and attached in turn,
 * if valid; if NV0000_CTRL_GPU_ATTACH_ALL_PROBED_IDS is used, all
 * probed gpuIds will be attached, in the order the associated GPUs
 * were probed in by the RM.
 *
 * If a gpuId fails to attach, this gpuId is stored in the failedId
 * field. Any GPUs attached by the command prior the failure are
 * detached.
 *
 * If multiple clients use NV0000_CTRL_CMD_GPU_ATTACH_IDS to attach
 * a gpuId, the RM ensures that the gpuId won't be detached until
 * all clients have issued a call to NV0000_CTRL_CMD_GPU_DETACH_IDS
 * to detach the gpuId (or have terminated).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OPERATING_SYSTEM
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_IRQ_EDGE_TRIGGERED
 *   NV_ERR_IRQ_NOT_FIRING
 */
#define NV0000_CTRL_CMD_GPU_ATTACH_IDS        (0x215U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_ATTACH_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_ATTACH_ALL_PROBED_IDS (0x0000ffffU)

#define NV0000_CTRL_GPU_ATTACH_IDS_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV0000_CTRL_GPU_ATTACH_IDS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_GPU_MAX_PROBED_GPUS];
    NvU32 failedId;
} NV0000_CTRL_GPU_ATTACH_IDS_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_DETACH_IDS
 *
 * This command detaches the GPUs with the gpuIds matching those in
 * the table provided by the client.
 * The table is NV0000_CTRL_GPU_MAX_ATTACHED_GPUS entries in size.
 *
 *   gpuIds[]
 *     This parameter holds the table of gpuIds to detach. At least
 *     one gpuId must be specified; clients may use the special
 *     gpuId NV0000_CTRL_GPU_DETACH_ALL_ATTACHED_IDS to indicate that
 *     all attached GPUs are to be detached.
 *     The entries in gpuIds[] must be contiguous, with a value of
 *     NV0000_CTRL_GPU_INVALID_ID to indicate the first invalid
 *     entry.
 *     If one or more of the gpuId values do not specify a GPU found
 *     in the system, the NV_ERR_INVALID_ARGUMENT error
 *     status is returned.
 *
 * If a table of gpuIds is provided, these gpuIds will be validated
 * against the RM's list of attached gpuIds; each valid gpuId is
 * detached immediately if it's no longer in use (i.e. if there are
 * no client references to the associated GPU in the form of
 * device class instantiations (see the NV01_DEVICE or NV03_DEVICE
 * descriptions for details)) and if no other client still requires
 * the associated GPU to be attached.
 *
 * If a given gpuId can't be detached immediately, it will instead
 * be detached when the last client reference is freed or when
 * the last client that issued NV0000_CTRL_CMD_GPU_ATTACH_IDS for
 * this gpuId either issues NV0000_CTRL_CMD_GPU_DETACH_IDS or exits
 * without detaching the gpuId explicitly.
 *
 * Clients may use the NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS command
 * to obtain a table of the attached gpuIds.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OPERATING_SYSTEM
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_GPU_DETACH_IDS          (0x216U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_DETACH_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_DETACH_ALL_ATTACHED_IDS (0x0000ffffU)

#define NV0000_CTRL_GPU_DETACH_IDS_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV0000_CTRL_GPU_DETACH_IDS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_GPU_MAX_ATTACHED_GPUS];
} NV0000_CTRL_GPU_DETACH_IDS_PARAMS;



/*
 * NV0000_CTRL_CMD_GPU_GET_VIDEO_LINKS
 *
 * This command returns information about video bridge connections
 * detected between GPUs in the system, organized as a table
 * with one row per attached GPU and none, one or more peer GPUs
 * listed in the columns of each row, if connected to the row head
 * GPU via a video bridge.
 *
 *   gpuId
 *     For each row, this field holds the GPU ID of the GPU
 *     whose connections are listed in the row.
 *
 *   connectedGpuIds
 *     For each row, this table holds the GPU IDs of the
 *     GPUs connected to the GPU identified via the 'gpuId'
 *     field.
 *
 *   links
 *     This table holds information about the video bridges
 *     connected between GPUs in the system.  Each row
 *     represents connections to a single GPU.
 *
 * Please note: the table only reports video links between already
 * attached GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_GPU_GET_VIDEO_LINKS (0x219U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_MAX_VIDEO_LINKS     8U

typedef struct NV0000_CTRL_GPU_VIDEO_LINKS {
    NvU32 gpuId;
    NvU32 connectedGpuIds[NV0000_CTRL_GPU_MAX_VIDEO_LINKS];
} NV0000_CTRL_GPU_VIDEO_LINKS;

#define NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS {
    NV0000_CTRL_GPU_VIDEO_LINKS links[NV0000_CTRL_GPU_MAX_ATTACHED_GPUS];
} NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS;



/*
 * NV0000_CTRL_CMD_GPU_GET_SVM_SIZE
 *
 * This command is used to get the SVM size.
 *
 *   gpuId
 *     This parameter uniquely identifies the GPU whose associated
 *     SVM size is to be returned. The value of this field must
 *     match one of those in the table returned by
 *     NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS
 *
 *   SvmSize
 *     SVM size is returned in this.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 *
 */
#define NV0000_CTRL_CMD_GPU_GET_SVM_SIZE (0x240U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS {
    NvU32 gpuId;
    NvU32 svmSize;
} NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS;



/*
 * NV0000_CTRL_CMD_GPU_GET_UUID_INFO
 *
 * This command returns requested information pertaining to the GPU
 * specified by the GPU UUID passed in.
 *
 * Generally only GPUs that have been attached are visible to this call. Therefore
 * queries on unattached GPUs will fail with NV_ERR_OBJECT_NOT_FOUND.  However, 
 * a query for a SHA1 UUID may succeed for an unattached GPU in cases where the GID
 * is cached, such as an excluded GPU.
 *
 *   gpuGuid (INPUT)
 *     The GPU UUID of the gpu whose parameters are to be returned. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 *   flags (INPUT)
 *     The _FORMAT* flags designate ascii string format or a binary format.
 *
 *     The _TYPE* flags designate either SHA-1-based (32-hex-character) or
 *     SHA-256-based (64-hex-character).
 *
 *   gpuId (OUTPUT)
 *     The GPU ID of the GPU identified by gpuGuid. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 *   deviceInstance (OUTPUT)
 *     The device instance of the GPU identified by gpuGuid. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 *   subdeviceInstance (OUTPUT)
 *     The subdevice instance of the GPU identified by gpuGuid. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *
 */
#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO                                   (0x274U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS_MESSAGE_ID" */

/* maximum possible number of bytes of GID information */
#define NV0000_GPU_MAX_GID_LENGTH                                           (0x00000100U)

#define NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS {
    NvU8  gpuUuid[NV0000_GPU_MAX_GID_LENGTH];
    NvU32 flags;
    NvU32 gpuId;
    NvU32 deviceInstance;
    NvU32 subdeviceInstance;
} NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS;

#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT                       1:0
#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_ASCII  (0x00000000U)
#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_BINARY (0x00000002U)

#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_TYPE                         2:2
#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_TYPE_SHA1     (0x00000000U)
#define NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_TYPE_SHA256   (0x00000001U)

/*
 * NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID
 *
 * This command returns the GPU UUID for the provided GPU ID.
 * Note that only GPUs that have been attached are visible to this call.
 * Therefore queries on unattached GPUs will fail
 * with NV_ERR_OBJECT_NOT_FOUND.
 *
 *   gpuId (INPUT)
 *     The GPU ID whose parameters are to be returned. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 *   flags (INPUT)
 *
 *     NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_FORMAT_ASCII
 *       This value is used to request the GPU UUID in ASCII format.
 *     NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_FORMAT_BINARY
 *       This value is used to request the GPU UUID in binary format.
 *
 *     NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_TYPE_SHA1
 *       This value is used to request that the GPU UUID value
 *       be SHA1-based (32-hex-character).
 *     NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_TYPE_SHA256
 *       This value is used to request that the GPU UUID value
 *       be SHA256-based (64-hex-character).
 *
 *   gpuUuid[NV0000_GPU_MAX_GID_LENGTH] (OUTPUT)
 *     The GPU UUID of the GPU identified by GPU ID. Refer to
 *     NV0000_CTRL_CMD_GPU_GET_ID_INFO for more information.
 *
 *   uuidStrLen (OUTPUT)
 *     The length of the UUID returned which is related to the format that
 *     was requested using flags.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID              (0x275U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS_MESSAGE_ID (0x75U)

typedef struct NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS {
    NvU32 gpuId;
    NvU32 flags;
    NvU8  gpuUuid[NV0000_GPU_MAX_GID_LENGTH];
    NvU32 uuidStrLen;
} NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS;

/* valid format values */
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_FORMAT                       1:0
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_FORMAT_ASCII  (0x00000000U)
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_FORMAT_BINARY (0x00000002U)

/*valid type values*/
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_TYPE                         2:2
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_TYPE_SHA1     (0x00000000U)
#define NV0000_CTRL_CMD_GPU_GET_UUID_FROM_GPU_ID_FLAGS_TYPE_SHA256   (0x00000001U)



/*
 * NV0000_CTRL_CMD_GPU_MODIFY_DRAIN_STATE
 *
 * This command is used to enter or exit the so called "drain" state.
 * When this state is enabled, the existing clients continue executing
 * as usual, however no new client connections are allowed.
 * This is done in order to "drain" the system of the running clients
 * in preparation to selectively powering down the GPU.
 * No GPU can enter a bleed state if that GPU is in an SLI group.
 * In that case, NV_ERR_IN_USE is returned.
 * Requires administrator privileges.
 *
 * It is expected, that the "drain" state will be eventually deprecated
 * and replaced with another mechanism to quiesce a GPU (Bug 1718113).
 *
 *  gpuId (INPUT)
 *    This parameter should specify a valid GPU ID value.  If there
 *    is no GPU present with the specified ID, a status of
 *    NV_ERR_INVALID_ARGUMENT is returned.
 *  newState (INPUT)
 *    This input parameter is used to enter or exit the "drain"
 *    software state of the GPU specified by the gpuId parameter.
 *    Possible values are:
 *      NV0000_CTRL_GPU_DRAIN_STATE_ENABLED
 *      NV0000_CTRL_GPU_DRAIN_STATE_DISABLED
 *  flags (INPUT)
 *    NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE
 *      if set, upon reaching quiescence, a request will be made to
 *      the OS to "forget" the PCI device associated with the
 *      GPU specified by the gpuId parameter, in case such a request
 *      is supported by the OS. Otherwise, NV_ERR_NOT_SUPPORTED
 *      will be returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_IN_USE
 */

#define NV0000_CTRL_CMD_GPU_MODIFY_DRAIN_STATE         (0x278U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS_MESSAGE_ID" */

/* Possible values of newState */
#define NV0000_CTRL_GPU_DRAIN_STATE_DISABLED           (0x00000000U)
#define NV0000_CTRL_GPU_DRAIN_STATE_ENABLED            (0x00000001U)

/* Defined bits for the "flags" argument */
#define NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE (0x00000001U)
#define NV0000_CTRL_GPU_DRAIN_STATE_FLAG_LINK_DISABLE  (0x00000002U)

#define NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS_MESSAGE_ID (0x78U)

typedef struct NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS {
    NvU32 gpuId;
    NvU32 newState;
    NvU32 flags;
} NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_QUERY_DRAIN_STATE
 *
 *  gpuId (INPUT)
 *    This parameter should specify a valid GPU ID value.  If there
 *    is no GPU present with the specified ID, a status of
 *    NVOS_STATUS_ERROR_INVALID_ARGUMENT is returned.
 *  drainState (OUTPUT)
 *    This parameter returns a value indicating if the "drain"
 *    state is currently enabled or not for the specified GPU. See the
 *    description of NV0000_CTRL_CMD_GPU_MODIFY_DRAIN_STATE.
 *    Possible values are:
 *      NV0000_CTRL_GPU_DRAIN_STATE_ENABLED
 *      NV0000_CTRL_GPU_DRAIN_STATE_DISABLED
 *  flags (OUTPUT)
 *    NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE
 *      if set, upon reaching quiesence, the GPU device will be
 *      removed automatically from the kernel space, similar
 *      to what writing "1" to the sysfs "remove" node does.
 *    NV0000_CTRL_GPU_DRAIN_STATE_FLAG_LINK_DISABLE
 *      after removing the GPU, also disable the parent bridge's
 *      PCIe link. This flag can only be set in conjunction with
 *      NV0000_CTRL_GPU_DRAIN_STATE_FLAG_REMOVE_DEVICE, and then
 *      only when the GPU is already idle (not attached).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_GPU_QUERY_DRAIN_STATE (0x279U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS_MESSAGE_ID (0x79U)

typedef struct NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS {
    NvU32 gpuId;
    NvU32 drainState;
    NvU32 flags;
} NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_DISCOVER
 *
 * This request asks the OS to scan the PCI tree or a sub-tree for GPUs,
 * that are not yet known to the OS, and to make them available for use.
 * If all of domain:bus:slot.function are zeros, the entire tree is scanned,
 * otherwise the parameters identify the bridge device, that roots the
 * subtree to be scanned.
 * Requires administrator privileges.
 *
 *  domain (INPUT)
 *    PCI domain of the bridge
 *  bus (INPUT)
 *    PCI bus of the bridge
 *  slot (INPUT)
 *    PCI slot of the bridge
 *  function (INPUT)
 *    PCI function of the bridge
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_OPERATING_SYSTEM
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_GPU_DISCOVER (0x27aU) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | 0x7A" */

typedef struct NV0000_CTRL_GPU_DISCOVER_PARAMS {
    NvU32 domain;
    NvU8  bus;
    NvU8  slot;
    NvU8  function;
} NV0000_CTRL_GPU_DISCOVER_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_GET_MEMOP_ENABLE
 *
 * This command is used to get the content of the MemOp (CUDA Memory Operation)
 * enablement mask, which can be overridden by using the MemOpOverride RegKey.
 *
 * The enableMask member must be treated as a bitmask, where each bit controls
 * the enablement of a feature.
 *
 * So far, the only feature which is defined controls to whole MemOp APIs.
 *
 * Possible status values returned are:
 *   NV_OK
 *
 */
#define NV0000_CTRL_CMD_GPU_GET_MEMOP_ENABLE (0x27bU) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS_MESSAGE_ID (0x7BU)

typedef struct NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS {
    NvU32 enableMask;
} NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS;

#define NV0000_CTRL_GPU_FLAGS_MEMOP_ENABLE   (0x00000001U)



/*
 * NV0000_CTRL_CMD_GPU_DISABLE_NVLINK_INIT
 *
 * This privileged command is used to disable initialization for the NVLinks
 * provided in the mask.
 *
 * The mask must be applied before the GPU is attached. DISABLE_NVLINK_INIT
 * is an NOP for non-NVLink GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_IN_USE
 *
 */
#define NV0000_CTRL_CMD_GPU_DISABLE_NVLINK_INIT (0x281U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS {
    NvU32  gpuId;
    NvU32  mask;
    NvBool bSkipHwNvlinkDisable;
} NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS;


#define NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PARAM_DATA     0x00000175U
#define NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PROPERTIES_IN  6U
#define NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PROPERTIES_OUT 5U

/*
 * NV0000_CTRL_CMD_GPU_LEGACY_CONFIG
 *
 * Path to use legacy RM GetConfig/Set API. This API is being phased out.
 */
#define NV0000_CTRL_CMD_GPU_LEGACY_CONFIG                (0x282U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS_MESSAGE_ID (0x82U)

typedef struct NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS {
    NvHandle hContext;    /* [in]  - Handle of object to perform operation on (Device, Subdevice, etc) */
    NvU32    opType;      /* [in]  - Type of API */
    NvV32    index;       /* [in]  - command type */
    NvU32    dataType;    /* [out] - data union type */

    union {
        struct {
            NvU8  paramData[NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PARAM_DATA];
            NvU32 paramSize;
        } configEx;
        struct {
            NvU32 propertyId;
            NvU32 propertyIn[NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PROPERTIES_IN];
            NvU32 propertyOut[NV0000_CTRL_GPU_LEGACY_CONFIG_MAX_PROPERTIES_OUT];
        } reservedProperty;
    } data;
} NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS;

#define NV0000_CTRL_GPU_LEGACY_CONFIG_OP_TYPE_GET_EX   (0x00000002U)
#define NV0000_CTRL_GPU_LEGACY_CONFIG_OP_TYPE_SET_EX   (0x00000003U)
#define NV0000_CTRL_GPU_LEGACY_CONFIG_OP_TYPE_RESERVED (0x00000004U)

/*
 * NV0000_CTRL_CMD_IDLE_CHANNELS
 */
#define NV0000_CTRL_CMD_IDLE_CHANNELS                  (0x283U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS_MESSAGE_ID (0x83U)

typedef struct NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS {
    NvHandle hDevice;
    NvHandle hChannel;
    NvV32    numChannels;
    /* C form: NvP64 phClients NV_ALIGN_BYTES(8); */
    NV_DECLARE_ALIGNED(NvP64 phClients, 8);
    /* C form: NvP64 phDevices NV_ALIGN_BYTES(8); */
    NV_DECLARE_ALIGNED(NvP64 phDevices, 8);
    /* C form: NvP64 phChannels NV_ALIGN_BYTES(8); */
    NV_DECLARE_ALIGNED(NvP64 phChannels, 8);
    NvV32    flags;
    NvV32    timeout;
} NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS;

#define NV0000_CTRL_GPU_IMAGE_TYPE_GSP           (0x00000001U)
#define NV0000_CTRL_GPU_IMAGE_TYPE_GSP_LOG       (0x00000002U)
#define NV0000_CTRL_GPU_IMAGE_TYPE_BINDATA_IMAGE (0x00000003U)
/*
 * NV0000_CTRL_CMD_PUSH_UCODE_IMAGE
 *
 * This command is used to push the GSP ucode into RM.
 * This function is used only on VMware
 *
 * Possible status values returned are:
 *   NV_OK                   The sent data is stored successfully
 *   NV_ERR_INVALID_ARGUMENT if the arguments are not proper
 *   NV_ERR_NO_MEMORY        if memory allocation failed
 *   NV_ERR_NOT_SUPPORTED    if function is invoked on non-GSP setup or any
 *                              setup other than VMware host
 *
 */
#define NV0000_CTRL_CMD_PUSH_UCODE_IMAGE         (0x285) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_PUSH_UCODE_IMAGE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_PUSH_UCODE_IMAGE_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV0000_CTRL_GPU_PUSH_UCODE_IMAGE_PARAMS {
    NvU8 image;
    NV_DECLARE_ALIGNED(NvU64 totalSize, 8);
    NV_DECLARE_ALIGNED(NvP64 pData, 8);
} NV0000_CTRL_GPU_PUSH_UCODE_IMAGE_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_SET_NVLINK_BW_MODE
 *
 * This command is used to set NVLINK bandwidth for power saving
 *
 * The setting must be applied before the GPU is attached.
 * NVLINK_BW_MODE is an NOP for non-NVLink GPUs.
 *
 *   [in] mode
 *      BW mode requested defined as a DRF
 *      Possible Legacy values that can be set in bits 2:0:
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_FULL
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_OFF
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_MIN
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_HALF
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_3QUARTER
 *      Link count can be requested on Blackwell+ in bits 7:3
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_IN_USE
 */

#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SETTING_LEGACY     2:0
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SETTING_LINK_COUNT 7:3

#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_FULL       (0x00U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_OFF        (0x01U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_MIN        (0x02U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_HALF       (0x03U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_3QUARTER   (0x04U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_LINK_COUNT (0x05U)

#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_UNSET    (0x00U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_PER_NODE (0x01U)
#define NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_PER_GPU  (0x02U)


#define NV0000_CTRL_CMD_GPU_SET_NVLINK_BW_MODE (0x286U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS_MESSAGE_ID" */
#define NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS_MESSAGE_ID (0x86U)

typedef struct NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS {
    NvU8 mode;
} NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_GET_NVLINK_BW_MODE
 *
 * This command is used to get NVLINK bandwidth for power saving
 *
 * NVLINK_BW_MODE is an NOP for non-NVLink GPUs.
 *
 *   [out] mode
 *      BW mode currently set for the GPUs on the system.
 *      Possible values are:
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_FULL
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_OFF
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_MIN
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_HALF
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_3QUARTER
 *   [out] bwModeScope
 *      Scope of the bw mode setting on the system.
 *      Possible values are:
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_UNSET
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_PER_NODE
 *        NV0000_CTRL_CMD_GPU_NVLINK_BW_MODE_SCOPE_PER_GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_DEVICE
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_IN_USE
 */

#define NV0000_CTRL_CMD_GPU_GET_NVLINK_BW_MODE (0x287U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS_MESSAGE_ID" */
#define NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS {
    NvU8 mode;
    NvU8 bwModeScope;
} NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS;

/*
 * NV0000_CTRL_GPU_ACTIVE_DEVICE
 *
 * This structure describes a single MIG or plain device in the system
 * available for use.
 *
 *   gpuId
 *     ID of an attached GPU.
 *   gpuInstanceId
 *     MIG gpu instance id of an instance existing on this GPU.
 *     NV0000_CTRL_GPU_INVALID_ID if the GPU is not in MIG mode.
 *   computeInstanceId
 *     MIG compute instance id of an instance existing on this GPU.
 *     NV0000_CTRL_GPU_INVALID_ID if the GPU is not in MIG mode.
 *
 */
typedef struct NV0000_CTRL_GPU_ACTIVE_DEVICE {
    NvU32 gpuId;
    NvU32 gpuInstanceId;
    NvU32 computeInstanceId;
} NV0000_CTRL_GPU_ACTIVE_DEVICE;

/*
 * NV0000_CTRL_CMD_GPU_GET_ACTIVE_DEVICE_IDS
 *
 * This command returns a list of valid GPUs treating uniformly MIG devices
 * and GPUs not in MIG mode.
 *
 *   [out] devices
 *      List of devices aviable for use.
 *   [out] numDevices
 *      Number of valid entries in gpus
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0000_CTRL_CMD_GPU_GET_ACTIVE_DEVICE_IDS (0x288U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_MAX_ACTIVE_DEVICES        256U

#define NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS {
    NvU32                         numDevices;
    NV0000_CTRL_GPU_ACTIVE_DEVICE devices[NV0000_CTRL_GPU_MAX_ACTIVE_DEVICES];
} NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_ASYNC_ATTACH_ID
 *
 * This command attaches the GPU with the given gpuId, similar to
 * NV0000_CTRL_CMD_GPU_ATTACH_IDS. However, this command instructs the RM
 * to perform the attach in the background.
 *
 * After calling this command, clients are expected to call
 * NV0000_CTRL_CMD_GPU_WAIT_ATTACH_ID before performing any operation that
 * depends on the GPU being attached.
 *
 * If the gpuId fails to attach, either this command or the subsequent
 * NV0000_CTRL_CMD_GPU_WAIT_ATTACH_ID command may fail.
 *
 * If clients from multiple processes use this command or the
 * NV0000_CTRL_CMD_GPU_ATTACH_IDS command to attach a gpuId, the RM ensures
 * that the gpuId won't be detached until all processes have issued a call to
 * NV0000_CTRL_CMD_GPU_DETACH_IDS to detach the gpuId (or have terminated).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OPERATING_SYSTEM
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_GPU_ASYNC_ATTACH_ID (0x289U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS {
    NvU32 gpuId;
} NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS;

/*
 * NV0000_CTRL_CMD_GPU_WAIT_ATTACH_ID
 *
 * This command waits for and returns the status of a background attach
 * operation started by NV0000_CTRL_CMD_GPU_ASYNC_ATTACH_ID.
 *
 * Clients are expected to call this command after calling
 * NV0000_CTRL_CMD_GPU_ASYNC_ATTACH_ID before performing any operation that
 * depends on the GPU being attached.
 *
 * If the gpuId fails to attach, either this command or the previous
 * NV0000_CTRL_CMD_GPU_ASYNC_ATTACH_ID command may fail.
 *
 * Calling this command for a gpuId that is already attached (for example,
 * after a successful NV0000_CTRL_CMD_GPU_ATTACH_IDS) is a no-op.
 *
 * Calling this command for a gpuId that is neither attached nor has a
 * background attach operation will result in NV_ERR_INVALID_ARGUMENT.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OPERATING_SYSTEM
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_IRQ_EDGE_TRIGGERED
 *   NV_ERR_IRQ_NOT_FIRING
 */
#define NV0000_CTRL_CMD_GPU_WAIT_ATTACH_ID (0x290U) /* finn: Evaluated from "(FINN_NV01_ROOT_GPU_INTERFACE_ID << 8) | NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS_MESSAGE_ID (0x90U)

typedef struct NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS {
    NvU32 gpuId;
} NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS;

/* _ctrl0000gpu_h_ */

