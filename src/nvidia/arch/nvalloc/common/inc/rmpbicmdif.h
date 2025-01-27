/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Shared postbox interface defines for RM and PMU.
 */

#ifndef RMPBICMDIF_H
#define RMPBICMDIF_H

/*
 * Define the status of postbox interface at different instances.
 *
 * The values should be in accordance to the spec and must not be changed.
 * A new PBI command must be submitted with the status NV_PBI_COMMAND_STATUS_UNDEFINED.
 */

#define NV_PBI_COMMAND_STATUS                                7:0
#define NV_PBI_COMMAND_STATUS_UNDEFINED                      0x00 // command submitted to PMU
#define NV_PBI_COMMAND_STATUS_SUCCESS                        0x01 // command successfully completed by PMU
#define NV_PBI_COMMAND_STATUS_PENDING                        0x02 // command accepted by PMU
#define NV_PBI_COMMAND_STATUS_BUSY                           0x03 // command processing in PMU
#define NV_PBI_COMMAND_STATUS_UNSPECIFIED_FAILURE            0x04 // unknown failure or hang
#define NV_PBI_COMMAND_STATUS_INVALID_ADDRESS                0x05 // invalid address submitted to PMU
#define NV_PBI_COMMAND_STATUS_MORE_DATA                      0x06 // user needs to send more data to PMU
#define NV_PBI_COMMAND_STATUS_INVALID_COMMAND                0x07 // invalid command submitted

/*
 * This corresponds to reserved bits of command register
 */
#define NV_PBI_COMMAND_RSVD_0                                15:8
#define NV_PBI_COMMAND_RSVD_0_VAL                            0x00

/*
 * This specifies the dword index if client is sending multiple dwords
 * for single Dword this should be zero
 */
#define NV_PBI_COMMAND_BUFFER_INDEX                          19:16

/*
 * This specifies the total no. of dwords passed by client
 */
#define NV_PBI_COMMAND_BUFFER_SIZE                           23:20

/*
 * These corresponds the different function ID's supported out of PBI
 */
#define NV_PBI_COMMAND_FUNC_ID                               27:24
#define NV_PBI_COMMAND_FUNC_ID_GET_CAPABILITIES              (0)
#define NV_PBI_COMMAND_FUNC_ID_EXECUTE_ROUTINE               (11)

//
// Definitions for common 'Execute Routine' calls that are
// shared across all GPUs.
//
// Common routine IDs must be in the range 0x80 to 0xff.
//
// Get GID:
#define NV_PBI_EXECUTE_ROUTINE_GET_GID                       0x80
// Get Feature:
#define NV_PBI_EXECUTE_ROUTINE_GET_FEATURE                   0x81
#define NV_PBI_EXECUTE_ROUTINE_GET_FEATURE_EXCLUSION         5:0
#define NV_PBI_EXECUTE_ROUTINE_GET_FEATURE_EXCLUSION_ALLOWED 0x2

/*
 * This corresponds to reserved field of command register
 */
#define NV_PBI_COMMAND_RSVD_1                                28:28
#define NV_PBI_COMMAND_RSVD_1_VAL                            0x00

/*
 * If this bit is set system will be notified on command completion
 */
#define NV_PBI_COMMAND_SYS_NOTIFY                            29:29
#define NV_PBI_COMMAND_SYS_NOTIFY_TRUE                       0x01
#define NV_PBI_COMMAND_SYS_NOTIFY_FALSE                      0x00

/*
 * If this bit is set driver will be notified of command completion status
 */
#define NV_PBI_COMMAND_DRV_NOTIFY                            30:30
#define NV_PBI_COMMAND_DRV_NOTIFY_TRUE                       0x01
#define NV_PBI_COMMAND_DRV_NOTIFY_FALSE                      0x00

/*
 * Defines the interrupt state of the PBI command
 */
#define NV_PBI_COMMAND_INTERRUPT                             31:31
#define NV_PBI_COMMAND_INTERRUPT_TRUE                        0x01
#define NV_PBI_COMMAND_INTERRUPT_FALSE                       0x00

/*
 * This sets the different fields of command register
 */
#define PBI_SET_COMMAND_PARAMS(status, r0, index, sz, cmd, r1, sys, \
                               drv, intr, val)                      \
{                                                                   \
    val = DRF_NUM(_PBI, _COMMAND, _STATUS, status)                | \
          DRF_NUM(_PBI, _COMMAND, _RSVD_0, r0)                    | \
          DRF_NUM(_PBI, _COMMAND, _BUFFER_INDEX, index)           | \
          DRF_NUM(_PBI, _COMMAND, _BUFFER_SIZE, sz)               | \
          DRF_NUM(_PBI, _COMMAND, _FUNC_ID, cmd)                  | \
          DRF_NUM(_PBI, _COMMAND, _RSVD_1, r1)                    | \
          DRF_NUM(_PBI, _COMMAND, _SYS_NOTIFY, sys)               | \
          DRF_NUM(_PBI, _COMMAND, _DRV_NOTIFY, drv)               | \
          DRF_NUM(_PBI, _COMMAND, _INTERRUPT, intr);                \
}

/*
 * postbox_clients_mutex_id
 *
 * Define the Mutex ID for different PBI clients
 */
enum postbox_clients_mutex_id
{
    PBI_CLIENT_NONE                     = 0x00,
    PBI_CLIENT_DRIVER_PCIPBI_SHIM       = 0x15
};

#endif /* RMPBICMDIF_H */
