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
// Source file:      ctrl/ctrl2080/ctrl2080flcn.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"


//
// XAPICHK/XAPI_TEST breaks on including "nvmisc.h". Workaround: don't include
// the header in that case and just redefine the macros we need.
//
#include "nvmisc.h"
/*
 * Obsolete Falcon ID type. Use NV2080_ENGINE_TYPE_ instead
 */
#define FALCON_ID_PMU           (NV2080_ENGINE_TYPE_PMU)
#define FALCON_ID_DPU           (NV2080_ENGINE_TYPE_DPU)
#define FALCON_ID_SEC2          (NV2080_ENGINE_TYPE_SEC2)
#define FALCON_ID_FBFLCN        (NV2080_ENGINE_TYPE_FBFLCN)

/*
 * NV2080_CTRL_CMD_FLCN_GET_DMEM_USAGE
 *
 * This command returns total heap size and free heap size of a falcon engine
 *
 *  flcnID
 *    The falcon ID
 *
 *  heapSize
 *    Total heap size in byte
 *
 *  heapFree
 *    Total free heap size in byte
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_NOT_SUPPORTED
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_INVALID_OBJECT
 */
#define NV2080_CTRL_CMD_FLCN_GET_DMEM_USAGE (0x20803101) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS {
    NvU32 flcnID;
    NvU32 heapSize;
    NvU32 heapFree;
} NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS;

/*!
 * @defgroup NVOS_INST_EVT Instrumentation event types.
 * @{
 */

//! Reserved for uStreamer internal use.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_RSVD_DO_NOT_USE    0x00U

//! RTOS CTXSW includes next taskID and number of ODP for previous task.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_CTXSW_END          0x01U

//! Begin of a HW IRQ.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_HW_IRQ_BEGIN       0x02U

//! End of a HW IRQ, before stack pinning etc is performed.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_HW_IRQ_END         0x03U

//! RTOS Timer tick slip. (Only for # tick processed > 1).
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_TIMER_TICK         0x04U

//! Task start processing an event, includes taskId, eventType and unitId (optional).
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_TASK_EVENT_BEGIN   0x05U

//! Task finished processing an event, incldues taskId.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_TASK_EVENT_END     0x06U

//! Latency for inserting response into RM queue.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_RM_QUEUE_LATENCY   0x07U

//! Special / multi-purpose event, see field definition below.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_TASK_SPECIAL_EVENT 0x08U

//! Unused, recycle
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_UNUSED_0           0x09U

#define NV2080_CTRL_FLCN_NVOS_INST_EVT_DMA_END            0x0AU

//! Begin/end for arbitrary block of code. The payload contains a sub-ID for each location profiled.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_GENERIC_BEGIN      0x0BU
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_GENERIC_END        0x0CU

//! Queueing time for the most recent event.
#define NV2080_CTRL_FLCN_NVOS_INST_EVT_TASK_EVENT_LATENCY 0x0DU
/*!@}*/

#define NV2080_CTRL_FLCN_NVOS_INST_INVALID_TASK_ID        0xFFU

/*
 * NV2080_CTRL_CMD_FLCN_GET_ENGINE_ARCH
 *
 * Get the egine arch i.e FALCON, RISCV etc given the NV2080_ENGINE_TYPE_*.
 *
 */
#define NV2080_CTRL_CMD_FLCN_GET_ENGINE_ARCH              (0x20803118) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS {
    //! The engine type, from NV2080_ENGINE_TYPE_*
    NvU32 engine;

    //! The engine architecture - FALCON or RISC-V
    NvU32 engineArch;
} NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS;

/*!
 * @defgroup Engine Arch types
 * @{
 */
#define NV2080_CTRL_FLCN_GET_ENGINE_ARCH_DEFAULT                         0x0
#define NV2080_CTRL_FLCN_GET_ENGINE_ARCH_FALCON                          0x1
#define NV2080_CTRL_FLCN_GET_ENGINE_ARCH_RISCV                           0x2
#define NV2080_CTRL_FLCN_GET_ENGINE_ARCH_RISCV_EB                        0x3
/*!@}*/


/* ----------------------- uStreamer (INST v2) ------------------------------ */
/*!
 * @defgroup NV2080_CTRL_FLCN_USTREAMER_EVENT uStreamer event fields.
 *
 * This is a binary-packed representation of uStreamer events. There are
 * three main types of entry: Head, Payload, and Tail. COMM here is used
 * when a field is shared among multiple event types.
 *
 * @{
 */
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_COMM_FLAG                         31:31
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_COMM_HEAD                         30:30
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_VARIABLE                     29:29
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EXTEND                       28:28

/*!
 * Below DRF's need constants assigned to start and end so they can be represented in FINN properly
 * This is because FINN v1 will not have the ability to represent DRF's and bit fields yet.
 */
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_EXTENT         (27)
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_BASE           (20)
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID \
    (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_EXTENT) : \
    (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_BASE)
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_EXTENT  (28)
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_BASE    (24)
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT \
    (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_EXTENT) : \
    (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_BASE)

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_LENGTH                        19:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOAD                        7:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT                23:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_HEAD_TIME                          29:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_DATA_PAYLOAD                       30:0
/*!@}*/


/*!
 * @defgroup NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_BREAKDOWN
 *
 * These DRFs define breakdown of the compact payload for various event IDs.
 *
 * @{
 */

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_TASK_ID                               7:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON                               10:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON_YIELD                          0x0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON_INT0                           0x1
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON_TIMER_TICK                     0x2
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON_QUEUE_BLOCK                    0x3
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_REASON_DMA_SUSPENDED                  0x4
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_CTXSW_END_ODP_MISS_COUNT                      23:11

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TIMER_TICK_TIME_SLIP                           23:0

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_BEGIN_TASK_ID                        7:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_BEGIN_UNIT_ID                       15:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_BEGIN_EVENT_TYPE                   23:16

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_END_TASK_ID                          7:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_END_CALLBACK_ID                     15:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_END_RPC_FUNC                        15:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_END_RPC_FUNC_BOBJ_CMD_BASE          0xF0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_END_CLASS_ID                       23:16

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_RM_QUEUE_LATENCY_SHIFT                          10U

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_SPECIAL_EVENT_TASK_ID                      7:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_SPECIAL_EVENT_ID                          23:8
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_SPECIAL_EVENT_ID_RESERVED             0x000000
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_SPECIAL_EVENT_ID_CB_ENQUEUE_FAIL      0x000001

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_TASK_EVENT_LATENCY_SHIFT                         6U

#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_GENERIC_ID                                     11:0
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_GENERIC_ID_INVALID                            0x000
#define NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_PAYLOADCOMPACT_GENERIC_ID_VF_SWITCH_TOTAL                    0x001

/*!@}*/


/*!
 * @defgroup NV2080_CTRL_FLCN_USTREAMER_FEATURE
 *
 * This defines all the features currently supported by uStreamer. For a new
 * usecase of uStreamer, a feature should be defined here describing the usecase.
 * This value should be unique for each queue.
 *
 * @{
 */
#define NV2080_CTRL_FLCN_USTREAMER_FEATURE_DEFAULT                       0U
#define NV2080_CTRL_FLCN_USTREAMER_FEATURE__COUNT                        1U
/*!@}*/

/*!
 * @defgroup NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY
 *
 * This defines the DRF used for ustreamer queue policy
 *
 * @{
 */

#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IDLE_FLUSH                   0:0
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IDLE_FLUSH_DISABLED      0U
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IDLE_FLUSH_ENABLED       1U

#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_FULL_FLUSH                   1:1
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_FULL_FLUSH_DISABLED      0U
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_FULL_FLUSH_ENABLED       1U

#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IMMEDIATE_FLUSH              2:2
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IMMEDIATE_FLUSH_DISABLED 0U
#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IMMEDIATE_FLUSH_ENABLED  1U

#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_POLICY_IDLE_THRESHOLD              31:8

/*!@}*/

/*!
 * The maximum number of compact event types, calculated from the number of bits
 * in the event structure.
 */
#define NV2080_CTRL_FLCN_USTREAMER_NUM_EVT_TYPES_COMPACT                 (0x20U) /* finn: Evaluated from "(1 << (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_EXTENT - NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTIDCOMPACT_DRF_BASE + 1))" */

/*!
 * The maximum number of event types, calculated from the number of bits in the
 * event structure.
 */
#define NV2080_CTRL_FLCN_USTREAMER_NUM_EVT_TYPES                         (0x120U) /* finn: Evaluated from "((1 << (NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_EXTENT - NV2080_CTRL_FLCN_USTREAMER_EVENT_TAIL_EVENTID_DRF_BASE + 1)) + NV2080_CTRL_FLCN_USTREAMER_NUM_EVT_TYPES_COMPACT)" */

/*!
 * The number of bytes required in the event mask to contain all event types.
 */
#define NV2080_CTRL_FLCN_USTREAMER_MASK_SIZE_BYTES                       (0x24U) /* finn: Evaluated from "((NV2080_CTRL_FLCN_USTREAMER_NUM_EVT_TYPES + 7) / 8)" */

/*!
 * uStreamer Event Filter type, stored as a bitmask.
 */
typedef struct NV2080_CTRL_FLCN_USTREAMER_EVENT_FILTER {
    NvU8 mask[NV2080_CTRL_FLCN_USTREAMER_MASK_SIZE_BYTES];
} NV2080_CTRL_FLCN_USTREAMER_EVENT_FILTER;

/*!
 * NV2080_CTRL_CMD_FLCN_USTREAMER_QUEUE_INFO
 * Get queue info for mapping / unmapping
 */
#define NV2080_CTRL_CMD_FLCN_USTREAMER_QUEUE_INFO (0x20803120) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS {
    //! The engine type, from NV2080_ENGINE_TYPE_*
    NvU32 engine;

    //!
    // The page size of the requested queue in bytes.
    //
    NvU32 pageSize;

    //! Offset of the queue buffer in FB.
    NV_DECLARE_ALIGNED(NvUPtr offset, 8);

    //!
    // The size of the user-mapped instrumentation buffer. Measured in bytes.
    //
    NvU32 size;

    //!
    // The feature ID of the queue.
    //
    NvU8  queueFeatureId;
} NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FLCN_USTREAMER_CONTROL_GET/SET
 *
 * Get/set the event bitmask for the default queue.
 */
#define NV2080_CTRL_CMD_FLCN_USTREAMER_CONTROL_GET (0x20803122) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_USTREAMER_CONTROL_GET_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FLCN_USTREAMER_CONTROL_SET (0x20803123) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_USTREAMER_CONTROL_SET_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS {
    //! The engine type, from NV2080_ENGINE_TYPE_*
    NvU32                                   engine;

    /*!
     * The bitmask of which event types to log. An event type corresponding to
     * a bit with a zero will be ignored at the log site, which prevents it
     * from filling up the resident buffer in the PMU. In general, set this to
     * only log the event types you actually want to use.
     * Refer to NVOS_BM_* in nvos_utility.h for usage.
     */
    NV2080_CTRL_FLCN_USTREAMER_EVENT_FILTER eventFilter;

    //! The queueId of the queue whose eventFilter we want to interact with
    NvU8                                    queueId;
} NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS;

#define NV2080_CTRL_FLCN_USTREAMER_CONTROL_GET_PARAMS_MESSAGE_ID (0x22U)

typedef NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS NV2080_CTRL_FLCN_USTREAMER_CONTROL_GET_PARAMS;

#define NV2080_CTRL_FLCN_USTREAMER_CONTROL_SET_PARAMS_MESSAGE_ID (0x23U)

typedef NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS NV2080_CTRL_FLCN_USTREAMER_CONTROL_SET_PARAMS;

/*
 * NV2080_CTRL_CMD_FLCN_GET_CTX_BUFFER_INFO
 *
 * This command provides the attributes of the falcon engine context buffer
 *
 *   hUserClient [IN]
 *     This parameter specifies the client handle that owns this channel.
 *   hChannel [IN]
 *     This parameter specifies the channel or channel group (TSG) handle
 *   alignment
 *     Specifies the alignment requirement for each context buffer
 *   size
 *     Aligned size of context buffer
 *   bufferHandle
 *     Opaque pointer to memdesc. Used by kernel clients for tracking purpose only.
 *   pageCount
 *     allocation size in the form of pageCount
 *   physAddr
 *     Physical address of the buffer first page
 *   aperture
 *     allocation aperture. Could be SYSMEM, VIDMEM, UNKNOWN
 *   kind
 *     PTE kind of this allocation.
 *   pageSize
 *     Page size of the buffer.
 *   bIsContigous
 *     States if physical allocation for this buffer is contiguous. PageSize will
 *     have no meaning if this flag is set.
 *   bDeviceDescendant
 *     TRUE if the allocation is a constructed under a Device or Subdevice.
 *   uuid
 *     SHA1 UUID of the Device or Subdevice. Valid when bDeviceDescendant is TRUE.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FLCN_GET_CTX_BUFFER_INFO (0x20803124) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS {
    NvHandle hUserClient;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvP64 bufferHandle, 8);
    NV_DECLARE_ALIGNED(NvU64 pageCount, 8);
    NV_DECLARE_ALIGNED(NvU64 physAddr, 8);
    NvU32    aperture;
    NvU32    kind;
    NvU32    pageSize;
    NvBool   bIsContigous;
    NvBool   bDeviceDescendant;
    NvU8     uuid[16];
} NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS;

// Aperture flags
#define NV2080_CTRL_FLCN_CTX_BUFFER_INFO_APERTURE_UNKNWON ADDR_UNKNOWN
#define NV2080_CTRL_FLCN_CTX_BUFFER_INFO_APERTURE_SYSMEM ADDR_SYSMEM
#define NV2080_CTRL_FLCN_CTX_BUFFER_INFO_APERTURE_FBMEM ADDR_FBMEM

/*
 * NV2080_CTRL_CMD_FLCN_GET_CTX_BUFFER_SIZE
 *
 * This command provides the size of the falcon engine context buffer
 *
 *   hChannel [IN]
 *     This parameter specifies the channel or channel group (TSG) handle
 *   totalBufferSize [OUT]
 *     This parameter returns the total context buffers size.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FLCN_GET_CTX_BUFFER_SIZE (0x20803125) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLCN_INTERFACE_ID << 8) | NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS {
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 totalBufferSize, 8);
} NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS;



/* _ctrl2080flcn_h_ */
