/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _KERN_HFRP_COMMON_H_
#define _KERN_HFRP_COMMON_H_

#include "nvtypes.h"

typedef enum
{
    HFRP_COMMAND_MAILBOX_INDEX_PMU      = 0,
    HFRP_RESPONSE_MAILBOX_INDEX_PMU     = 1,
    HFRP_COMMAND_MAILBOX_INDEX_SHIM     = 2,
    HFRP_RESPONSE_MAILBOX_INDEX_SHIM    = 2,
    HFRP_COMMAND_MAILBOX_INDEX_DISPLAY  = 3,
    HFRP_RESPONSE_MAILBOX_INDEX_DISPLAY = 4
} HFRP_MAILBOX_INDEX;

// Size of range of registers for which the aperture is created
#define HFRP_MAILBOX_ACCESS_RANGE           0x200

// Mailbox Layout Address Offsets 1.0 version
// Mailbox Layout Address Offsets 1.0 version
#define HFRP_COMMAND_BUFFER_HEAD_PTR_ADDR_ONE_MAILBOX_INTERFACE     0x110
#define HFRP_COMMAND_BUFFER_TAIL_PTR_ADDR_ONE_MAILBOX_INTERFACE     0x189
#define HFRP_COMMAND_BUFFER_START_ADDR_ONE_MAILBOX_INTERFACE        0x114
#define HFRP_COMMAND_BUFFER_END_ADDR_ONE_MAILBOX_INTERFACE          0x187
#define HFRP_RESPONSE_BUFFER_HEAD_PTR_ADDR_ONE_MAILBOX_INTERFACE    0x188
#define HFRP_RESPONSE_BUFFER_TAIL_PTR_ADDR_ONE_MAILBOX_INTERFACE    0x111
#define HFRP_RESPONSE_BUFFER_START_ADDR_ONE_MAILBOX_INTERFACE       0x18C
#define HFRP_RESPONSE_BUFFER_END_ADDR_ONE_MAILBOX_INTERFACE         0x1FF

#define HFRP_COMMAND_BUFFER_HEAD_PTR_ADDR_TWO_MAILBOX_INTERFACE     0x110
#define HFRP_COMMAND_BUFFER_TAIL_PTR_ADDR_TWO_MAILBOX_INTERFACE     (0x111 + HFRP_MAILBOX_ACCESS_RANGE)
#define HFRP_COMMAND_BUFFER_START_ADDR_TWO_MAILBOX_INTERFACE        0x114
#define HFRP_COMMAND_BUFFER_END_ADDR_TWO_MAILBOX_INTERFACE          0x1FF
#define HFRP_RESPONSE_BUFFER_HEAD_PTR_ADDR_TWO_MAILBOX_INTERFACE    (0x110 + HFRP_MAILBOX_ACCESS_RANGE)
#define HFRP_RESPONSE_BUFFER_TAIL_PTR_ADDR_TWO_MAILBOX_INTERFACE    0x111
#define HFRP_RESPONSE_BUFFER_START_ADDR_TWO_MAILBOX_INTERFACE       (0x114 + HFRP_MAILBOX_ACCESS_RANGE)
#define HFRP_RESPONSE_BUFFER_END_ADDR_TWO_MAILBOX_INTERFACE         (0x1FF + HFRP_MAILBOX_ACCESS_RANGE)

#define HFRP_IRQ_IN_SET_ADDR                                0x100
#define HFRP_IRQ_OUT_SET_ADDR                               0x104
#define HFRP_IRQ_IN_CLR_ADDR                                0x108
#define HFRP_IRQ_OUT_CLR_ADDR                               0x10C

// Size of message (command or response) header in bytes
#define HFRP_MESSAGE_HEADER_BYTE_SIZE       4U

#define HFRP_MESSAGE_FIELD_SIZE             7U : 0U
#define HFRP_MESSAGE_FIELD_SEQUENCE_ID      17U : 8U
#define HFRP_MESSAGE_FIELD_INDEX_OR_STATUS  27U : 18U

//
// Maximum values of Sequence Id index and Sequence Id Array index (each
// Sequence Id array element has 32 bits that represent 32 Sequence Ids)
//
#define HFRP_NUMBER_OF_SEQUENCEID_INDEX             0x400
#define HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX       (HFRP_NUMBER_OF_SEQUENCEID_INDEX / 32U)
#define HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX    0x3FF

// HFRP IRQ Reset and Doorbell bit indices
#define HFRP_IRQ_RESET_BIT_INDEX            0U
#define HFRP_IRQ_DOORBELL_BIT_INDEX         1U

// Mailbox Interface types
#define HFRP_ONE_MAILBOX_INTERFACE          0U
#define HFRP_TWO_MAILBOX_INTERFACE          1U

// Mailbox flags
#define HFRP_COMMAND_MAILBOX_FLAG           0U
#define HFRP_RESPONSE_MAILBOX_FLAG          1U

// macros for supporting DRF operations
#define NV_HFRP_BYTE_FIELD(x)               (8U * (x) + 7U) : (8U * (x))

typedef struct
{
    NvU32 hfrpCommandBufferHeadPtrAddr;
    NvU32 hfrpCommandBufferTailPtrAddr;
    NvU32 hfrpCommandBufferStartAddr;
    NvU32 hfrpCommandBufferEndAddr;
    NvU32 hfrpResponseBufferHeadPtrAddr;
    NvU32 hfrpResponseBufferTailPtrAddr;
    NvU32 hfrpResponseBufferStartAddr;
    NvU32 hfrpResponseBufferEndAddr;
    NvU32 hfrpIrqInSetAddr;
    NvU32 hfrpIrqOutSetAddr;
    NvU32 hfrpIrqInClrAddr;
    NvU32 hfrpIrqOutClrAddr;
} HFRP_MAILBOX_IO_INFO;

typedef struct
{
    NvU32 sequenceIdState[HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX];
    NvU8 *pResponsePayloadArray[HFRP_NUMBER_OF_SEQUENCEID_INDEX];
    NvU16 *pResponseStatusArray[HFRP_NUMBER_OF_SEQUENCEID_INDEX];
    NvU32 *pResponsePayloadSizeArray[HFRP_NUMBER_OF_SEQUENCEID_INDEX];
    NV_STATUS *pStatusArray[HFRP_NUMBER_OF_SEQUENCEID_INDEX];
    NvU8 sequenceIdArrayIndex;
} HFRP_SEQUENCEID_INFO;

typedef struct
{
    HFRP_MAILBOX_IO_INFO    mailboxIoInfo;
    HFRP_SEQUENCEID_INFO    sequenceIdInfo;
} HFRP_INFO;

#endif  // _KERN_HFRP_COMMON_H_
