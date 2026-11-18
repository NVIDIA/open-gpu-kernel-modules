/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __vgpu_dev_vgpu_gsp_shared_h__
#define __vgpu_dev_vgpu_gsp_shared_h__

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvctassert.h"

/* Value published in message_seq_num after the GSP plugin has booted. */
#define GSP_PLUGIN_BOOTLOADED 0x4E654A6FU

/******************************************************************************/
/* GSP control buffer shared between the CPU and GSP plugins - START          */
/******************************************************************************/

/*
 * GSP plugin heap memory layout
 *
 * +--------------------------------+ offset = 0
 * |         CONTROL BUFFER         |
 * +--------------------------------+
 * |        RESPONSE BUFFER         |
 * +--------------------------------+
 * |         MESSAGE BUFFER         |
 * +--------------------------------+
 * |        MIGRATION BUFFER        |
 * +--------------------------------+
 * |    GSP PLUGIN ERROR BUFFER     |
 * +--------------------------------+
 * |    INIT TASK LOG BUFFER        |
 * +--------------------------------+
 * |      VGPU TASK LOG BUFFER      |
 * +--------------------------------+
 * |       KERNEL LOG BUFFER        |
 * +--------------------------------+
 * |   GUEST RPC TRACE BUFFER       |
 * +--------------------------------+
 * |      MEMORY AVAILABLE FOR      |
 * | GSP PLUGIN INTERNAL HEAP USAGE |
 * +--------------------------------+
 */
#define VGPU_CPU_GSP_CTRL_BUFF_VERSION_V1             0x1U
#define VGPU_CPU_GSP_CTRL_BUFF_VERSION                0x2U

/* Region sizes and the total communication buffer size are in bytes. */
#define VGPU_CPU_GSP_CTRL_BUFF_REGION_SIZE            4096U
#define VGPU_CPU_GSP_RESPONSE_BUFF_REGION_SIZE        4096U
#define VGPU_CPU_GSP_MESSAGE_BUFF_REGION_SIZE         4096U
#define VGPU_CPU_GSP_MIGRATION_BUFF_REGION_SIZE       (2U * 1024U * 1024U)
#define VGPU_CPU_GSP_ERROR_BUFF_REGION_SIZE           4096U
#define VGPU_CPU_GSP_INIT_TASK_LOG_BUFF_REGION_SIZE   (128U * 1024U)
#define VGPU_CPU_GSP_VGPU_TASK_LOG_BUFF_REGION_SIZE   (256U * 1024U)
#define VGPU_CPU_GSP_KERNEL_TASK_LOG_BUFF_REGION_SIZE (64U * 1024U)
#define VGPU_CPU_GSP_GUEST_RPC_TRACE_BUFF_REGION_SIZE (64U * 1024U)
#define VGPU_CPU_GSP_COMMUNICATION_BUFF_TOTAL_SIZE  (VGPU_CPU_GSP_CTRL_BUFF_REGION_SIZE            + \
                                                     VGPU_CPU_GSP_RESPONSE_BUFF_REGION_SIZE        + \
                                                     VGPU_CPU_GSP_MESSAGE_BUFF_REGION_SIZE         + \
                                                     VGPU_CPU_GSP_MIGRATION_BUFF_REGION_SIZE       + \
                                                     VGPU_CPU_GSP_ERROR_BUFF_REGION_SIZE           + \
                                                     VGPU_CPU_GSP_INIT_TASK_LOG_BUFF_REGION_SIZE   + \
                                                     VGPU_CPU_GSP_VGPU_TASK_LOG_BUFF_REGION_SIZE   + \
                                                     VGPU_CPU_GSP_KERNEL_TASK_LOG_BUFF_REGION_SIZE + \
                                                     VGPU_CPU_GSP_GUEST_RPC_TRACE_BUFF_REGION_SIZE)

/*
 * These values are part of the CPU-GSP wire ABI. Keep them independent from
 * private plugin and SDK constants so changes outside this interface cannot
 * silently alter the shared layouts.
 */
#define VGPU_CPU_GSP_MAX_INSTANCES          16U
#define VGPU_CPU_GSP_MAX_HEADS              4U
#define VGPU_CPU_GSP_UUID_SIZE              16U
#define VGPU_CPU_GSP_GUEST_INFO_BUFFER_SIZE 256U

/* Protocol booleans in the response buffer occupy a fixed 32-bit word. */
typedef NvU32 VGPU_CPU_GSP_BOOL;

/*
 * Guest/host VGX protocol compatibility version. This selects a compatible
 * guest RPC interface; it is not a marketing driver version.
 */
typedef struct
{
    /* Major and minor components of the VGX protocol version. */
    NvU32 major_number;
    NvU32 minor_number;
} VGPU_CPU_GSP_VGX_VERSION;

/*
 * Guest driver metadata collected by the GSP plugin. The three length fields
 * are guest-provided byte counts and are not guaranteed to equal strlen() or
 * to include a terminating NUL. Consumers must bound them to the fixed buffer
 * size. guest_branch_buffer has no corresponding length field.
 */
typedef struct
{
    /* Keep the VGX version first; its position is part of the wire ABI. */
    VGPU_CPU_GSP_VGX_VERSION vgx_version;

    /* Guest-reported byte lengths for the first three buffers below. */
    NvU32                    guest_driver_version_buffer_length;
    NvU32                    guest_version_buffer_length;
    NvU32                    guest_title_buffer_length;

    /* Changelist from which the guest driver was built. */
    NvU32                    guest_changelist_number;

    /* Guest driver version, guest OS version, product title, and branch. */
    char                     guest_driver_version_buffer[VGPU_CPU_GSP_GUEST_INFO_BUFFER_SIZE];
    char                     guest_version_buffer[VGPU_CPU_GSP_GUEST_INFO_BUFFER_SIZE];
    char                     guest_title_buffer[VGPU_CPU_GSP_GUEST_INFO_BUFFER_SIZE];
    char                     guest_branch_buffer[VGPU_CPU_GSP_GUEST_INFO_BUFFER_SIZE];
} VGPU_CPU_GSP_GUEST_INFO;

/*
 * Control buffer:
 * CPU plugin - request producer
 * GSP plugin - request consumer
 *
 * The CPU plugin writes an RPC payload to the message buffer, selects an
 * action in message_type, increments message_seq_num, and then notifies the
 * GSP plugin. All *_buff_offset fields are byte offsets from the base of the
 * GSP plugin heap shown above. During boot only, the GSP plugin writes
 * GSP_PLUGIN_BOOTLOADED to message_seq_num as a readiness handshake.
 */
typedef union
{
    NvU8 buf[VGPU_CPU_GSP_CTRL_BUFF_REGION_SIZE];
    struct
    {
        /* Layout version of this control and response protocol. */
        volatile NvU32  version;

        /* Requested MESSAGE action and the request's incrementing sequence. */
        volatile NvU32  message_type;
        volatile NvU32  message_seq_num;

        /* Locations of the remaining shared regions in the GSP plugin heap. */
        volatile NvU64  response_buff_offset;
        volatile NvU64  message_buff_offset;
        volatile NvU64  migration_buff_offset;
        volatile NvU64  error_buff_offset;
        volatile NvU64  guest_rpc_trace_buff_offset;

        /*
         * CPU byte cursor in the circular migration buffer: consumer GET for
         * save and producer PUT for restore.
         */
        volatile NvU32  migration_buf_cpu_access_offset;

        /* Set while a CPU-managed migration operation is active. */
        volatile NvBool is_migration_in_progress;

        /* CPU consumer positions, expressed as entry indices, for the rings. */
        volatile NvU32  error_buff_cpu_get_idx;
        volatile NvU32  guest_rpc_trace_buff_cpu_get_idx;

        /* Number of valid host_info entries; at most VGPU_CPU_GSP_MAX_INSTANCES. */
        volatile NvU32  attached_vgpu_count;

        /* Set after CPU RM completes the required graphics initialization. */
        volatile NvBool is_gr_init_done;

        /* Host identity associated with each attached vGPU instance. */
        volatile struct
        {
            /* vGPU type configuration identifier. */
            NvU32 vgpu_type_id;

            /* Packed host PCI location key used to correlate the physical GPU. */
            NvU32 host_gpu_pci_id;

            /* PCI device ID associated with the vGPU; this is not a PCI BDF. */
            NvU32 pci_dev_id;

            /* Stable binary UUID of the vGPU instance. */
            NvU8  vgpu_uuid[VGPU_CPU_GSP_UUID_SIZE];
        } host_info[VGPU_CPU_GSP_MAX_INSTANCES];
    };
} VGPU_CPU_GSP_CTRL_BUFF_REGION;

/*
 * CPU-to-GSP request IDs carried in the NvU32 message_type field. Zero is
 * reserved and NV_VGPU_CPU_RPC_MSG_MAX is an exclusive sentinel. The numeric
 * values are an append-only wire ABI and must not be reordered or reused.
 * Request payload layouts are private and are not defined by this header.
 */
typedef enum
{
    NV_VGPU_CPU_RPC_MSG_VERSION_NEGOTIATION = 1,
    NV_VGPU_CPU_RPC_MSG_SETUP_CONFIG_PARAMS_AND_INIT,
    NV_VGPU_CPU_RPC_MSG_RESET,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_STOP_WORK,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_CANCEL_STOP,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_SAVE_STATE,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_CANCEL_SAVE,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_RESTORE_STATE,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_RESTORE_DEFERRED_STATE,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_RESUME_WORK,
    NV_VGPU_CPU_RPC_MSG_CONSOLE_VNC_STATE,
    NV_VGPU_CPU_RPC_MSG_VF_BAR0_REG_ACCESS,
    NV_VGPU_CPU_RPC_MSG_UPDATE_BME_STATE,
    NV_VGPU_CPU_RPC_MSG_RESET_MIGRATION_BUFFER_PTR,
    NV_VGPU_CPU_RPC_MSG_RELEASE_CLIENT_DATABASE,
    NV_VGPU_CPU_RPC_MSG_CHECK_IS_ALIVE,
    NV_VGPU_CPU_RPC_MSG_SEND_STATIC_INFO,
    NV_VGPU_CPU_RPC_MSG_GET_CONTEXT_BUFFER_FB_RANGES,
    NV_VGPU_CPU_RPC_MSG_MIGRATION_DISABLE_UVM_USER_ADMIN_CHANNELS,
    NV_VGPU_CPU_RPC_MSG_SEND_GPU_STUN_EVENT,
    NV_VGPU_CPU_RPC_MSG_MAX, /* Sentinel; not a request action. */
} MESSAGE;

/*
 * GSP-produced snapshot of one displayless head's surface state. The GSP
 * plugin writes sequence_update_start before the fields and
 * sequence_update_end after them. A reader has a consistent snapshot only
 * when both sequence values are equal. These values are generations, not
 * timestamps. Dimensions are in pixels; size, offset, pitch, and effective
 * page size are in bytes.
 */
typedef struct
{
    /* Sequence markers used to detect a concurrent update. */
    volatile NvU64             sequence_update_start;
    volatile NvU64             sequence_update_end;

    /* Host RM page size used for the surface. */
    volatile NvU32             effective_fb_page_size;

    /* Dimensions of the visible rectangle and its backing surface. */
    volatile NvU32             rect_width;
    volatile NvU32             rect_height;
    volatile NvU32             surface_width;
    volatile NvU32             surface_height;

    /* Allocation size, memory offset, RM format, kind, pitch, and layout type. */
    volatile NvU32             surface_size;
    volatile NvU32             surface_offset;
    volatile NvU32             surface_format;
    volatile NvU32             surface_kind;
    volatile NvU32             surface_pitch;
    volatile NvU32             surface_type;

    /* Encoded block height: values 0 through 5 represent 1 through 32 GOBs. */
    volatile NvU8              surface_block_height;

    /* Current display and allocation state for the surface. */
    volatile VGPU_CPU_GSP_BOOL is_blanking_enabled;
    volatile VGPU_CPU_GSP_BOOL is_flip_pending;
    volatile VGPU_CPU_GSP_BOOL is_free_pending;
    volatile VGPU_CPU_GSP_BOOL is_memory_blocklinear;
} VGPU_CPU_GSP_DISPLAYLESS_SURFACE;

/*
 * GSP plugin response buffer:
 * CPU plugin - response consumer
 * GSP plugin - response producer
 *
 * The GSP plugin publishes completion by copying the request sequence into
 * message_seq_num_processed. message_seq_num_received is a best-effort
 * diagnostic that indicates processing has started. The CPU plugin initializes
 * or resets selected fields while setting up communication and migration.
 */
typedef union
{
    NvU8 buf[VGPU_CPU_GSP_RESPONSE_BUFF_REGION_SIZE];
    struct
    {
        /*
         * Sequence last accepted by the GSP plugin. This diagnostic is not
         * updated for the initial version-negotiation request.
         */
        volatile NvU32                            message_seq_num_received;

        /* Sequence last completed by the GSP plugin. */
        volatile NvU32                            message_seq_num_processed;

        /*
         * Raw completion status for message_seq_num_processed. Zero denotes
         * success; nonzero values are protocol status codes, not errno values.
         */
        volatile NvU32                            result_code;

        /* Guest RM RPC protocol version negotiated by the GSP plugin. */
        volatile NvU32                            guest_rpc_version;

        /*
         * GSP byte cursor in the circular migration buffer: producer PUT for
         * save and consumer GET for restore.
         */
        volatile NvU32                            migration_buf_gsp_access_offset;

        /*
         * Producer-done flag for migration state saving. The CPU clears it
         * before save and the GSP sets it after it stops producing data.
         */
        volatile NvU32                            migration_state_save_complete;

        /* False while an active developer-tools session blocks migration. */
        volatile VGPU_CPU_GSP_BOOL                is_migration_allowed;

        /* Per-head displayless surface snapshots. */
        volatile VGPU_CPU_GSP_DISPLAYLESS_SURFACE surface[VGPU_CPU_GSP_MAX_HEADS];

        /* GSP producer position in the error ring, expressed as an entry index. */
        volatile NvU32                            error_buff_gsp_put_idx;

        /* License state, packed OS type, and frame-rate limit in frames/second. */
        volatile NvU32                            grid_license_state;
        volatile NvU32                            guest_os_type;
        volatile NvU32                            frl_config;

        /* Guest metadata and a flag indicating that the metadata is valid. */
        VGPU_CPU_GSP_GUEST_INFO                   guest_info;
        volatile VGPU_CPU_GSP_BOOL                is_guest_info_populated;

        /* GSP producer position in the trace ring, expressed as an entry index. */
        volatile NvU32                            guest_rpc_trace_buff_gsp_put_idx;

        /* Guest STUN/UNSTUN completion status. */
        volatile NvU32                            guest_stun_status;

        /* Set when the GSP plugin skipped the stun event. */
        volatile VGPU_CPU_GSP_BOOL                guest_stun_event_skipped;
    };
} VGPU_CPU_GSP_RESPONSE_BUFF_REGION;

/*
 * Freeze the CPU-GSP wire ABI. In particular, NvBool fields in the control
 * buffer are one byte while VGPU_CPU_GSP_BOOL fields are four bytes.
 */
ct_assert(sizeof(NvBool) == 1U);
ct_assert(sizeof(VGPU_CPU_GSP_BOOL) == 4U);
ct_assert(VGPU_CPU_GSP_COMMUNICATION_BUFF_TOTAL_SIZE == 2637824U);

ct_assert(sizeof(VGPU_CPU_GSP_VGX_VERSION) == 8U);
ct_assert(sizeof(VGPU_CPU_GSP_GUEST_INFO) == 1048U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, vgx_version) == 0U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_driver_version_buffer_length) == 8U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_version_buffer_length) == 12U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_title_buffer_length) == 16U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_changelist_number) == 20U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_driver_version_buffer) == 24U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_version_buffer) == 280U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_title_buffer) == 536U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_GUEST_INFO, guest_branch_buffer) == 792U);

ct_assert(sizeof(VGPU_CPU_GSP_DISPLAYLESS_SURFACE) == 80U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, sequence_update_start) == 0U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, sequence_update_end) == 8U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, effective_fb_page_size) == 16U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, surface_block_height) == 60U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, is_blanking_enabled) == 64U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, is_flip_pending) == 68U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, is_free_pending) == 72U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_DISPLAYLESS_SURFACE, is_memory_blocklinear) == 76U);

ct_assert(sizeof(VGPU_CPU_GSP_CTRL_BUFF_REGION) == VGPU_CPU_GSP_CTRL_BUFF_REGION_SIZE);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, version) == 0U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, message_type) == 4U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, message_seq_num) == 8U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, response_buff_offset) == 16U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, message_buff_offset) == 24U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, migration_buff_offset) == 32U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, error_buff_offset) == 40U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, guest_rpc_trace_buff_offset) == 48U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, migration_buf_cpu_access_offset) == 56U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, is_migration_in_progress) == 60U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, error_buff_cpu_get_idx) == 64U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, guest_rpc_trace_buff_cpu_get_idx) == 68U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, attached_vgpu_count) == 72U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, is_gr_init_done) == 76U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_CTRL_BUFF_REGION, host_info) == 80U);

ct_assert(sizeof(VGPU_CPU_GSP_RESPONSE_BUFF_REGION) == VGPU_CPU_GSP_RESPONSE_BUFF_REGION_SIZE);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, message_seq_num_received) == 0U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, message_seq_num_processed) == 4U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, result_code) == 8U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_rpc_version) == 12U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, migration_buf_gsp_access_offset) == 16U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, migration_state_save_complete) == 20U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, is_migration_allowed) == 24U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, surface) == 32U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, error_buff_gsp_put_idx) == 352U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, grid_license_state) == 356U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_os_type) == 360U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, frl_config) == 364U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_info) == 368U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, is_guest_info_populated) == 1416U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_rpc_trace_buff_gsp_put_idx) == 1420U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_stun_status) == 1424U);
ct_assert(NV_OFFSETOF(VGPU_CPU_GSP_RESPONSE_BUFF_REGION, guest_stun_event_skipped) == 1428U);

/******************************************************************************/
/* GSP control buffer shared between the CPU and GSP plugins - END            */
/******************************************************************************/

#endif // __vgpu_dev_vgpu_gsp_shared_h__
