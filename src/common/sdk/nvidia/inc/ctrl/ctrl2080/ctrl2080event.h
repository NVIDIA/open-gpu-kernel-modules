/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080event.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

#include "nv_vgpu_types.h"
/* NV20_SUBDEVICE_XX event-related control commands and parameters */

/*
 * NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION
 *
 * This command sets event notification state for the associated subdevice.
 * This command requires that an instance of NV01_EVENT has been previously
 * bound to the associated subdevice object.
 *
 *   event
 *     This parameter specifies the type of event to which the specified
 *     action is to be applied.  This parameter must specify a valid
 *     NV2080_NOTIFIERS value (see cl2080.h for more details) and should
 *     not exceed one less NV2080_NOTIFIERS_MAXCOUNT.
 *   action
 *     This parameter specifies the desired event notification action.
 *     Valid notification actions include:
 *       NV2080_CTRL_SET_EVENT_NOTIFICATION_DISABLE
 *         This action disables event notification for the specified
 *         event for the associated subdevice object.
 *       NV2080_CTRL_SET_EVENT_NOTIFICATION_SINGLE
 *         This action enables single-shot event notification for the
 *         specified event for the associated subdevice object.
 *       NV2080_CTRL_SET_EVENT_NOTIFICATION_REPEAT
 *         This action enables repeated event notification for the specified
 *         event for the associated system controller object.
 *    bNotifyState
 *      This boolean is used to indicate the current state of the notifier
 *      at the time of event registration. This is optional and its semantics
 *      needs to be agreed upon by the notifier and client using the notifier
 *    info32
 *      This is used to send 32-bit initial state info with the notifier at
 *      time of event registration
 *    info16
 *      This is used to send 16-bit initial state info with the notifier at
 *      time of event registration
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION (0x20800301) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS {
    NvU32  event;
    NvU32  action;
    NvBool bNotifyState;
    NvU32  info32;
    NvU16  info16;
} NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS;

/* valid action values */
#define NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE (0x00000000)
#define NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE  (0x00000001)
#define NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT  (0x00000002)

/* XUSB/PPC D-state defines */
#define NV2080_EVENT_DSTATE_XUSB_D0                       (0x00000000)
#define NV2080_EVENT_DSTATE_XUSB_D3                       (0x00000003)
#define NV2080_EVENT_DSTATE_XUSB_INVALID                  (0xFFFFFFFF)
#define NV2080_EVENT_DSTATE_PPC_D0                        (0x00000000)
#define NV2080_EVENT_DSTATE_PPC_D3                        (0x00000003)
#define NV2080_EVENT_DSTATE_PPC_INVALID                   (0xFFFFFFFF)

// HDACODEC Decice DState, D3_COLD is only for verbose mapping, it cannot be logged
typedef enum NV2080_EVENT_HDACODEC_DSTATE {
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_D0 = 0,
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_D1 = 1,
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_D2 = 2,
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_D3_HOT = 3,
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_D3_COLD = 4,
    NV2080_EVENT_HDACODEC_DEVICE_DSTATE_DSTATE_MAX = 5,
} NV2080_EVENT_HDACODEC_DSTATE;

/*
 * NV2080_CTRL_CMD_EVENT_SET_TRIGGER
 *
 * This command triggers a software event for the associated subdevice.
 * This command accepts no parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_EVENT_SET_TRIGGER         (0x20800302) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | 0x2" */

/*
 * NV2080_CTRL_CMD_EVENT_SET_NOTIFIER_MEMORY
 *
 *     hMemory
 *       This parameter specifies the handle of the memory object
 *       that identifies the memory address translation for this
 *       subdevice instance's notification(s).  The beginning of the
 *       translation points to an array of notification data structures.
 *       The size of the translation must be at least large enough to hold the
 *       maximum number of notification data structures identified by
 *       the NV2080_MAX_NOTIFIERS value.
 *       Legal argument values must be instances of the following classes:
 *         NV01_NULL
 *         NV04_MEMORY
 *       When hMemory specifies the NV01_NULL_OBJECT value then any existing
 *       memory translation connection is cleared.  There must not be any
 *       pending notifications when this command is issued.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_EVENT_SET_MEMORY_NOTIFIES (0x20800303) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS {
    NvHandle hMemory;
} NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS;

#define NV2080_EVENT_MEMORY_NOTIFIES_STATUS_NOTIFIED 0
#define NV2080_EVENT_MEMORY_NOTIFIES_STATUS_PENDING  1
#define NV2080_EVENT_MEMORY_NOTIFIES_STATUS_ERROR    2

/*
 * NV2080_CTRL_CMD_EVENT_SET_SEMAPHORE_MEMORY
 *
 *     hSemMemory
 *       This parameter specifies the handle of the memory object that
 *       identifies the semaphore memory associated with this subdevice
 *       event notification.  Once this is set RM will generate an event
 *       only when there is a change in the semaphore value.  It is
 *       expected that the semaphore memory value will be updated by
 *       the GPU indicating that there is an event pending. This
 *       command is used by VGX plugin to determine which virtual
 *       machine has generated a particular event.
 *
 *     semOffset
 *       This parameter indicates the memory offset of the semaphore.
 *
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_PARAM_STRUCT
 *      NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_EVENT_SET_SEMAPHORE_MEMORY   (0x20800304) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS {
    NvHandle hSemMemory;
    NvU32    semOffset;
} NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS;

/*
 * NV2080_CTRL_CMD_EVENT_SET_GUEST_MSI
 *
 *     hSemMemory
 *       This parameter specifies the handle of the memory object that
 *       identifies the semaphore memory associated with this subdevice
 *       event notification.  Once this is set RM will generate an event
 *       only when there is a change in the semaphore value.  It is
 *       expected that the semaphore memory value will be updated by
 *       the GPU indicating that there is an event pending. This
 *       command is used by VGX plugin to determine which virtual
 *       machine has generated a particular event.
 *
 *     guestMSIAddr
 *       This parameter indicates the guest allocated MSI address.
 *
 *     guestMSIData
 *       This parameter indicates the MSI data set by the guest OS.
 *
 *     vmIdType
 *       This parameter specifies the type of guest virtual machine identifier
 *
 *     guestVmId
 *       This parameter specifies the guest virtual machine identifier
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_PARAM_STRUCT
 *      NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_EVENT_SET_GUEST_MSI (0x20800305) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_GUEST_MSI_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_GUEST_MSI_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_EVENT_SET_GUEST_MSI_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 guestMSIAddr, 8);
    NvU32      guestMSIData;
    NvHandle   hSemMemory;
    NvBool     isReset;
    VM_ID_TYPE vmIdType;
    NV_DECLARE_ALIGNED(VM_ID guestVmId, 8);
} NV2080_CTRL_EVENT_SET_GUEST_MSI_PARAMS;


/*
 * NV2080_CTRL_CMD_EVENT_SET_SEMA_MEM_VALIDATION
 *
 *     hSemMemory
 *       This parameter specifies the handle of the memory object that
 *       identifies the semaphore memory associated with this subdevice
 *       event notification.  Once this is set RM will generate an event
 *       only when there is a change in the semaphore value.  It is
 *       expected that the semaphore memory value will be updated by
 *       the GPU indicating that there is an event pending. This
 *       command is used by VGX plugin to determine which virtual
 *       machine has generated a particular event.
 *
 *     isSemaMemValidationEnabled
 *       This parameter used to enable/disable change in sema value check
 *       while generating an event.
 *
 * Possible status values returned are:
 *      NVOS_STATUS_SUCCESS
 *      NVOS_STATUS_ERROR_INVALID_OBJECT_HANDLE
 *      NVOS_STATUS_ERROR_INVALID_ARGUMENT
 */


#define NV2080_CTRL_CMD_EVENT_SET_SEMA_MEM_VALIDATION (0x20800306) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS {
    NvHandle hSemMemory;
    NvBool   isSemaMemValidationEnabled;
} NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS;


/*
 * NV2080_CTRL_CMD_EVENT_SET_VMBUS_CHANNEL
 *
 *     hSemMemory
 *       This parameter specifies the handle of the memory object that
 *       identifies the semaphore memory associated with this subdevice
 *       event notification.  Once this is set RM will generate an event
 *       only when there is a change in the semaphore value.  It is
 *       expected that the semaphore memory value will be updated by
 *       the GPU indicating that there is an event pending. This
 *       command is used by VGX plugin to determine which virtual
 *       machine has generated a particular event.
 *
 *     vmIdType
 *       This parameter specifies the type of guest virtual machine identifier
 *
 *     guestVmId
 *       This parameter specifies the guest virtual machine identifier
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_EVENT_SET_VMBUS_CHANNEL (0x20800307) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_VMBUS_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_VMBUS_CHANNEL_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_EVENT_SET_VMBUS_CHANNEL_PARAMS {
    NvHandle   hSemMemory;
    VM_ID_TYPE vmIdType;
    NV_DECLARE_ALIGNED(VM_ID guestVmId, 8);
} NV2080_CTRL_EVENT_SET_VMBUS_CHANNEL_PARAMS;


/*
 * NV2080_CTRL_CMD_EVENT_SET_TRIGGER_FIFO
 *
 * This command triggers a FIFO event for the associated subdevice.
 *
 *  hEvent
 *    Handle of the event that should be notified. If zero, all
 *    non-stall interrupt events for this subdevice will be notified.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_EVENT_SET_TRIGGER_FIFO (0x20800308) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS {
    NvHandle hEvent;
} NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS;

/*
 * NV2080_CTRL_CMD_EVENT_VIDEO_BIND_EVTBUF_FOR_UID
 *
 * This command is used to create a video bind-point to an event buffer that
 * is filtered by UID.
 *
 *  hEventBuffer[IN]
 *      The event buffer to bind to
 *
 *  recordSize[IN]
 *      The size of the FECS record in bytes
 *
 *  levelOfDetail[IN]
 *      One of NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_:
 *          FULL: Report all CtxSw events
 *          SIMPLE: Report engine start and engine end events only
 *          CUSTOM: Report events in the eventFilter field
 *      NOTE: RM may override the level-of-detail depending on the caller
 *
 *  eventFilter[IN]
 *      Bitmask of events to report if levelOfDetail is CUSTOM
 *
 *  bAllUsers[IN]
 *     Only report video data for the current user if false, for all users if true
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_EVENT_VIDEO_BIND_EVTBUF (0x20800309) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_EVENT_INTERFACE_ID << 8) | NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD {
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_FULL = 0,
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_SIMPLE = 1,
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_CUSTOM = 2,
} NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD;

#define NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS {
    NvHandle                                hEventBuffer;
    NvU32                                   recordSize;
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD levelOfDetail;
    NvU32                                   eventFilter;
    NvBool                                  bAllUsers;
} NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS;

/* _ctrl2080event_h_ */
