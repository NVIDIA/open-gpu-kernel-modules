/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file  nv_sriov_defines.h
 * @brief Defines for doorbell tokens and scratch registers index.
 * 
 * Define:- 
 *    - VF scratch registers index used in VGPU-GSP RPC setup
 *    - VF scratch registers index used in FECS tracing support
 *    - Doorbell interrupt handles for Guest RM to GSP-Plugin communication
 *    - Doorbell interrupt handles for Host CPU-Plugin to GSP-Plugin communication
 */

// Guest interrupts

#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_REQUEST                            0:0
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_REQUEST_EN                         (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_REQUEST_DIS                        (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_HANDLE                             (0 ? NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_REQUEST                          1:1
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_REQUEST_EN                       (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_REQUEST_DIS                      (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_HANDLE                           (0 ? NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_REQUEST)

// vGPU HWS fast path: guest packs cmd + payload into VF scratch regs,
// then rings this bit. Host services in processGspNotifyInterrupt, switches
// on the cmd field, and calls the matching subdeviceCtrlCmdFifoVgpuSwRunlist*
// _Internal helper. Replaces the legacy CPU-vgpu-plugin RPC path for
// NV2080_CTRL_CMD_FIFO_VGPU_SWRUNLIST_{SUBMIT, CHANNEL_UPDATE}.
#define NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_REQUEST                             4:4
#define NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_REQUEST_EN                          (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_REQUEST_DIS                         (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_HANDLE                              (0 ? NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_VF_MASK                                         (NVBIT(NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_HANDLE) | \
                                                                                 NVBIT(NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_HANDLE) | \
                                                                                 NVBIT(NV_DOORBELL_NOTIFY_LEAF_VF_VGPU_HWS_HANDLE))

// Top half serviced interrupts (serviced by both guest and host)

#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_REQUEST                     2:2
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_REQUEST_EN                  (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_REQUEST_DIS                 (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_HANDLE                      (0 ? NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_TOP_HALF_MASK                                   NVBIT(NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_HANDLE)

// Host PF interrupts

#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_REQUEST                             3:3
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_REQUEST_EN                          (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_REQUEST_DIS                         (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_HANDLE                              (0 ? NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_REQUEST                  15:15
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_REQUEST_EN               (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_REQUEST_DIS              (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_HANDLE                   (0 ? NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_REQUEST            16:16
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_REQUEST_EN         (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_REQUEST_DIS        (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_HANDLE             (0 ? NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_HOST_PF_MASK                                    (NVBIT(NV_DOORBELL_NOTIFY_LEAF_SERVICE_NON_REPLAYABLE_FAULT_HANDLE) | \
                                                                                 NVBIT(NV_DOORBELL_NOTIFY_LEAF_SERVICE_TMR_HANDLE) | \
                                                                                 NVBIT(NV_DOORBELL_NOTIFY_LEAF_SERVICE_MMU_INFO_FAULT_HANDLE) )

// Host VF interrupts

#define NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_REQUEST                           17:17
#define NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_REQUEST_EN                        (0x1)
#define NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_REQUEST_DIS                       (0x0)
#define NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_HANDLE                            (0 ? NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_REQUEST)

#define NV_DOORBELL_NOTIFY_LEAF_HOST_VF_MASK                                    NVBIT(NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_HANDLE)

// current write offset of the FECS trace buffer
#define NV_VF_SCRATCH_REGISTER_FECS_TRACE_WR_RD_OFFSET                          0x0
// current read offset of the FECS trace buffer
#define NV_VF_SCRATCH_REGISTER_FECS_TRACE_RD_RD_OFFSET                          0x1

// used in VGPU-GSP RPC setup
#define NV_VF_SCRATCH_REGISTER_GUEST_RPC_LO                                     0x2
// used in VGPU-GSP RPC setup
#define NV_VF_SCRATCH_REGISTER_GUEST_RPC_HI                                     0x3

//
// vGPU HWS fast-path scratch registers.
//
// Only consulted by call sites compiled with
// GLOBAL_FEATURE_GR3643_VGPU_WINDOWS_HWS_SUPPORT; harmless otherwise.
//
// The guest writes request data into the request/result scratch register,
// publishes HEADER with STATUS=PENDING, and rings the doorbell. The host reads
// HEADER.CMD, runs the matching _INTERNAL helper, writes the raw NV_STATUS into
// the request/result register, and marks HEADER.STATUS=DONE. This preserves the
// synchronous ordering required by SUBMIT, where downstream operations expect
// the runlist state to be committed before they run.
//
//   HEADER  [3:0]    cmd        (NV_VGPU_HWS_CMD_*)
//           [19:4]   engineType (16 bits)
//           [24:20]  version    (NV_VGPU_HWS_PROTOCOL_VERSION_*)
//           [26:25]  status     (NV_VF_SCRATCH_VGPU_HWS_HEADER_STATUS_*)
//           [31:27]  reserved
//
//   For cmd == NV_VGPU_HWS_CMD_CHANNEL_UPDATE:
//     Request/result register:
//              [11:0]  vChId        (12 bits, bounded by MAX_CHANNEL_ENTRIES = 1024)
//              [13:12] channelLevel (2 bits, NV2080_CTRL_FIFO_VGPU_SWRUNLIST_CHANNEL_LEVEL_*)
//              [15:14] operation    (2 bits, NV2080_CTRL_FIFO_VGPU_SWRUNLIST_CHANNEL_UPDATE_OP_*)
//              [31:16] reserved
//
//   For cmd == NV_VGPU_HWS_CMD_SUBMIT:
//     Request/result register:
//              [0]     bUpdateRunlist
//              [31:1]  reserved
//
//   Request/result register:
//              [31:0] request payload while HEADER.STATUS is PENDING, then
//                     raw NV_STATUS after HEADER.STATUS is DONE.
//
#define NV_VF_SCRATCH_REGISTER_VGPU_HWS_HEADER                                  0x4
#define NV_VF_SCRATCH_REGISTER_VGPU_HWS_PAYLOAD                                 0x5

// HEADER bit fields
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_CMD                                       3:0
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_ENGINE_TYPE                               19:4
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_VERSION                                   24:20
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_STATUS                                    26:25
// [31:27] Reserved

// HEADER protocol values
#define NV_VGPU_HWS_PROTOCOL_VERSION_0                                          0

// HEADER cmd values
#define NV_VGPU_HWS_CMD_NONE                                                    0
#define NV_VGPU_HWS_CMD_CHANNEL_UPDATE                                          1
#define NV_VGPU_HWS_CMD_SUBMIT                                                  2

// HEADER.STATUS handshake values
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_STATUS_IDLE                               0
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_STATUS_PENDING                            1
#define NV_VF_SCRATCH_VGPU_HWS_HEADER_STATUS_DONE                               2

// PAYLOAD bit fields - CHANNEL_UPDATE
#define NV_VF_SCRATCH_VGPU_HWS_PAYLOAD_CU_VCHID                                 11:0
#define NV_VF_SCRATCH_VGPU_HWS_PAYLOAD_CU_LEVEL                                 13:12
#define NV_VF_SCRATCH_VGPU_HWS_PAYLOAD_CU_OPERATION                             15:14

// PAYLOAD - SUBMIT request
#define NV_VF_SCRATCH_VGPU_HWS_PAYLOAD_SUB_UPDATE_RUNLIST                       0:0

// PAYLOAD - response, after GSP-RM has captured the request
#define NV_VF_SCRATCH_VGPU_HWS_PAYLOAD_RESULT                                   31:0

// STATUS handshake values. Guest writes _PENDING (0xFFFFFFFF) before ringing
// the doorbell and polls until the value transitions to non-PENDING. Host
// writes the raw NV_STATUS returned by the _INTERNAL helper (0 = NV_OK; any
// non-zero value = NV_ERR_*). 0xFFFFFFFF is safely outside the NV_STATUS
// range ([0, 0xFFFF]) so it cannot collide with a real status code.
//
// The numeric value matches NV_VGPU_MSG_RESULT_RPC_PENDING in
// rpc_headers.h:152, the same sentinel legacy RPC uses for "host has not
// yet written the result." Keeping them aligned means any tooling that
// recognises 0xFFFFFFFF as "pending RPC reply" also treats a stuck bypass
// scratch register the same way.
#define NV_VF_SCRATCH_VGPU_HWS_STATUS_PENDING                                   0xFFFFFFFF

// Per-VF PRIV: GFID encoded in bits 37:32; per-GFID window size below.
#define NV_VF_PRIV_ADDRESS_GFID                                                 37:32
#define NV_VF_PRIV_PER_GFID_WINDOW_SIZE_64MB                                    (1ULL << 26)
#define NV_VF_PRIV_PER_GFID_WINDOW_SIZE_256MB                                   (1ULL << 28)
#define NV_VF_PRIV_PER_GFID_WINDOW_SIZE_2GB                                     (1ULL << 31)

#define MAX_PARTITIONS_WITH_GFID                                                (48)
#define MAX_PARTITIONS_WITH_GFID_32VM                                           (32)
#define MAX_PARTITIONS_WITH_GFID_MIG_ENABLED                                    (7)
#define MAX_PARTITIONS_WITH_GFID_1VM                                            (1)
