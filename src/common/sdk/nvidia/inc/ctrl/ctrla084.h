/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla084.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h" // NV2080_GPU_MAX_GID_LENGTH
#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES

/* NVA084_KERNEL_HOST_VGPU_DEVICE control commands and parameters */
#define NVA084_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0xA084U, NVA084_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVA084_CTRL_RESERVED                                         (0x00)
#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE                          (0x01)

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO
 *
 * This command sets the guest vgpu device's information
 *
 * Parameter:
 *
 * vgpuUuid [IN]
 *  This parameter specifies the universaly unique identifier of the guest vGPU device
 *
 * vgpuDeviceInstanceId [IN]
 *  This parameter specifies the vGPU device instance
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO (0xa0840101) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS {
    NvU8 vgpuUuid[NV2080_GPU_MAX_GID_LENGTH];
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS;

/* NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE
 *
 * This command triggers the notifier for vGPU guest.
 *
 * Parameters:
 *
 * vmLifeCycleState[IN]
 * The life cycle event of the vGPU guest. This can be:
 *   NVA081_NOTIFIERS_EVENT_VGPU_GUEST_DESTROYED
 *   NVA081_NOTIFIERS_EVENT_VGPU_GUEST_CREATED
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE (0xa0840102) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS {
    NvU32 vmLifeCycleState;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS;

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO
 *
 * This command is used to copy offlined page mapping info from plugin to host vgpu device
 *
 *   guestFbSegmentPageSize [in]
 *     Guest FB segment page size
 *
 *   offlinedPageCount [in]
 *     Offlined page count in the guest FB range
 *
 *   gpa [in]
 *     This array represents guest page address list of offlined page
 *
 *   hMemory [in]
 *     This array represents memory handle list of good page
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO (0xa0840103) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS {
    NvU32    guestFbSegmentPageSize;
    NvU32    offlinedPageCount;
    NV_DECLARE_ALIGNED(NvU64 gpa[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES], 8);
    NvHandle hMemory[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS;

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS
 *
 * Config space access for the virtual function
 *
 * Parameters:
 * 
 * offset [IN]
 *   Offset within the config space
 *
 * numBytes [IN]
 *   Number of bytes to be read/written: 1/2/4
 *
 * accessType [IN]
 *   To indicate whether it is a read operation or a write operation
 *
 * value [INOUT]
 *   Value to be read or written
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS (0xa0840104) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_VF_CONFIG_SPACE_ACCESS_TYPE_READ                   0x1
#define NVA084_CTRL_VF_CONFIG_SPACE_ACCESS_TYPE_WRITE                  0x2

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS {
    NvU16 offset;
    NvU8  numBytes;
    NvU8  accessType;
    NvU32 value;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS;

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF
 *
 * This command is used to create a bind-point on a host system
 * for the collection of guest VM FECS events
 *
 *  hEventBufferClient[IN]
 *      The client of the event buffer to bind to
 *
 *  hEventBufferSubdevice[IN]
 *      The subdevice of the event buffer to bind to
 *
 *  hEventBuffer[IN]
 *      The event buffer to bind to
 *
 *  reasonCode[OUT]
 *      The reason for failure (if any); see NV2080_CTRL_GR_FECS_BIND_EVTBUF_REASON_CODE
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF (0xa0840105) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS {
    NvHandle hEventBufferClient;
    NvHandle hEventBufferSubdevice;
    NvHandle hEventBuffer;
    NvU32    reasonCode;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS;

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL
 *
 * The command will trigger the specified interrupt on the host from CPU Plugin.
 *
 *   handle[IN]
 *      - An opaque handle that will be passed in along with the interrupt
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL (0xa0840106) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS {
    NvU32 handle;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS;

/* valid action values */
#define NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated host vgpu device.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated host vgpu device object.
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION (0xa0840107) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x7U)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS {
    //
    // @todo: We will define the actual event values later based on the use case.
    // These event values are only for Test purpose.
    //
    NvU32  event;
    NvU32  action;
    NvBool bNotifyState;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS;

/* NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_GER_BAR_MAPPING_RANGES
 *
 * This command is used to get Bar mapping ranges in RM.
 *
 * Parameters:
 * offsets [OUT]
 *  Offsets of the ranges
 * sizes [OUT]
 *  Sizes of the ranges
 * mitigated [OUT]
 *  Specifies whether it's mitigated range
 * numRanges [OUT]
 *  Number of ranges
 *
 * osPageSize [IN]
 *  Page size.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES (0xa084010a) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES     10

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS_MESSAGE_ID (0xAU)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offsets[NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES], 8);
    NV_DECLARE_ALIGNED(NvU64 sizes[NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES], 8);
    NvU32  numRanges;
    NvU32  osPageSize;
    NvBool mitigated[NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES];
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS;

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_RESTORE_DEFAULT_EXEC_PARTITION
 *
 * This command restores the vGPU device's default EXEC partition saved in
 * KERNEL_HOST_VGPU_DEVICE.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_RESTORE_DEFAULT_EXEC_PARTITION (0xa084010b) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | 0xB" */

/*
 * NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_FREE_STATES
 *
 * This command is a KernelHostVgpuDeviceApi counterpart of
 * NVA082_CTRL_CMD_HOST_VGPU_DEVICE_FREE_STATES, that allows
 * to clean up a VF state.
 *
 * Parameters:
 *
 * flags [IN]
 *   Specifies what states need to be freed, same as in NVA082_CTRL_CMD_HOST_VGPU_DEVICE_FREE_STATES
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_FREE_STATES                    (0xa084010c) /* finn: Evaluated from "(FINN_NVA084_KERNEL_HOST_VGPU_DEVICE_KERNEL_HOST_VGPU_DEVICE_INTERFACE_ID << 8) | NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS_MESSAGE_ID" */

#define NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS_MESSAGE_ID (0xCU)

typedef struct NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS {
    NvU32 flags;
} NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS;

/* _ctrla084_h_ */
