/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFSOE_H_
#define _SOEIFSOE_H_

/*!
 * @file   soeifsoe.h
 * @brief  SOE Command/Message Interfaces - SOE Management
 */
#ifndef AES128_BLOCK_SIZE_BYTES
#define AES128_BLOCK_SIZE_BYTES 16
#endif
/*!
 * Defines the identifiers various high-level types of sequencer commands and
 * messages.
 *
 * _SOE_INIT @ref RM_SOE_INIT_MSG_SOE_INIT
 */
enum
{
    RM_SOE_INIT_MSG_ID_SOE_INIT = 0,
};

/*
 * CMDQ and MSGQ lengths used in INIT-MSG on LS10
 *
 * These values are hard-coded in soe-riscv.
 * They need to be same in SOE and driver and
 * are protected by compile time checks in SOE.
 *
 */
#define SOE_CMD_QUEUE_LENGTH    (0x80ul)
#define SOE_MSG_QUEUE_LENGTH    (0x80ul)

/*!
 * Define SOE CMDQ and MSGQ EMEM offsets
 */
#define SOE_EMEM_CHANNEL_CMDQ_OFFSET 0x1000080
#define SOE_EMEM_CHANNEL_MSGQ_OFFSET 0x1000000

/*!
 * Defines the logical queue IDs that must be used when submitting commands
 * to or reading messages from SOE. The identifiers must begin with zero and
 * should increment sequentially. _CMDQ_LOG_ID__LAST must always be set to the
 * last command queue identifier. _NUM must always be set to the last identifier
 * plus one.
 */
#define SOE_RM_CMDQ_LOG_ID       0
#define SOE_RM_CMDQ_LOG_ID__LAST 0
#define SOE_RM_MSGQ_LOG_ID       1
#define SOE_QUEUE_NUM            2

/*!
 * Message sent from SOE to the RM upon initialization as an event to
 * indicate that SOE is initialized and "ready" to process commands from
 * the RM.
 */
typedef struct
{
    NvU8  msgType;
    NvU8  numQueues;

    NvU16 osDebugEntryPoint;   //<! DMEM address of SOE's DEBUG_INFO

    /*!
     * SOE command and message queue locations and sizes are determined at SOE
     * build-time. Each queue is represented by a single element in this array.
     *
     * When EMEM support is enabled, the offsets for queues and the
     * rmManagedArea will be DMEM addresses located in EMEM, which is
     * mapped right on top of the DMEM VA space.
     */
    struct
    {
        NvU32 queueOffset;      //<! DMEM address of the start of the queue
        NvU16 queueSize;        //<! Size of the queue (in bytes)
        NvU8  queuePhyId;       //<! Physical/register-index of the queue
        NvU8  queueLogId;       //<! Logical ID of the queue
    } qInfo[SOE_QUEUE_NUM];

    NvU32  rmManagedAreaOffset;                         //<! DMEM address of the RM-managed area
    NvU16  rmManagedAreaSize;                           //<! Size (in bytes) of the RM-managed area
    NvU8   devidEnc[AES128_BLOCK_SIZE_BYTES];           //<! Encrypted DEVID for devid name lookup
    NvU8   devidDerivedKey[AES128_BLOCK_SIZE_BYTES];    //<! Derived key used by RM for further decryption of devid name

    FLCN_STATUS status;

} RM_SOE_INIT_MSG_SOE_INIT;

typedef union
{
    NvU8                      msgType;
    RM_SOE_INIT_MSG_SOE_INIT  soeInit;
} RM_FLCN_MSG_SOE_INIT;

#endif  // _SOEIFSOE_H_
