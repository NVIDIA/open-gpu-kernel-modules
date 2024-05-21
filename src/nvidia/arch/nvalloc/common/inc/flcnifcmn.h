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

/*!
 * flcn Command/Message Interfaces - Common
 */

#ifndef FLCNIFCMN_H
#define FLCNIFCMN_H

#include "nvtypes.h"
#include "flcnretval.h"

/*!
 * @ref NvU64_ALIGN32 , NvU64_ALIGN32_PACK, NvU64_ALIGN32_UNPACK
 * TODO: Use NV types directly
 */
typedef NvU64_ALIGN32   RM_FLCN_U64;
typedef NvU64_ALIGN32 *PRM_FLCN_U64;

#define RM_FLCN_U64_IS_ZERO                               NvU64_ALIGN32_IS_ZERO
#define RM_FLCN_U64_PACK                                     NvU64_ALIGN32_PACK
#define RM_FLCN_U64_UNPACK                                 NvU64_ALIGN32_UNPACK

/*!
 * @brief   Header preceding each CMD/MSG exchanged through falcon's queues.
 *
 * @note    Data package sent to falcon are referred as CMDs (commands).
 *          Data package sent by falcon are referred as MSGs (messages).
 */
typedef struct
{
    /*!
     * Unit ID identifies falcon's task/unit receiving/issuing this message.
     */
    NvU8    unitId;
    /*!
     * Total CMD/MSG size (including header).
     */
    NvU8    size;
    /*!
     * Flags identifying state of CMD/MSG.
     */
    NvU8    ctrlFlags;
    /*!
     * Sequence # ID to track each request sent to falcon (where applicable).
     */
    NvU8    seqNumId;
} RM_FLCN_QUEUE_HDR,
*PRM_FLCN_QUEUE_HDR;

/*!
 * Convenience macro for determining the size of the falcon's queue header:
 */
#define RM_FLCN_QUEUE_HDR_SIZE          sizeof(RM_FLCN_QUEUE_HDR)

/*!
 * Generic Falcon rewind unit ID.
 */
#define  RM_FLCN_UNIT_ID_REWIND         (0x00U)

/*!
 * Generic CMD structure to hold the header.
 */
typedef struct
{
    RM_FLCN_QUEUE_HDR   hdr;
    NvU32               cmd;
} RM_FLCN_CMD_GEN;

/*!
 * Generic MSG structure to hold the header.
 */
typedef struct
{
    RM_FLCN_QUEUE_HDR   hdr;
    NvU32               msg;
} RM_FLCN_MSG_GEN;

/*!
 * Convenience macros for determining the size of body for a command or message:
 */
#define RM_FLCN_CMD_BODY_SIZE(u,t)      sizeof(RM_FLCN_##u##_CMD_##t)
#define RM_FLCN_MSG_BODY_SIZE(u,t)      sizeof(RM_FLCN_##u##_MSG_##t)

/*!
 * Convenience macros for determining the size of a command or message:
 */
#define RM_FLCN_CMD_SIZE(u,t)  \
    (RM_FLCN_QUEUE_HDR_SIZE + RM_FLCN_CMD_BODY_SIZE(u,t))

#define RM_FLCN_MSG_SIZE(u,t)  \
    (RM_FLCN_QUEUE_HDR_SIZE + RM_FLCN_MSG_BODY_SIZE(u,t))

/*!
 * Convenience macros for determining the type of a command or message
 * (intended to be used symmetrically with the CMD and MSG _SIZE macros):
 */
#define RM_FLCN_CMD_TYPE(u,t)           RM_FLCN_##u##_CMD_ID_##t
#define RM_FLCN_MSG_TYPE(u,t)           RM_FLCN_##u##_MSG_ID_##t

/*!
 * @brief   Falcons' queue header flags (@ref RM_FLCN_QUEUE_HDR::ctrlFlags).
 *
 * RM_FLCN_QUEUE_FLAGS_STATUS
 *  Set by command's sender to request back message confirming the completion of
 *  command's execution.  In RM->FLCN communication response is required to free
 *  command related data tracked within RM (***_SEQ_INFO).
 *
 * RM_FLCN_QUEUE_HDR_FLAGS_EVENT
 *  Set by the falcon to distinguish messages sent to RM from command responses.
 *
 * RM_FLCN_QUEUE_HDR_FLAGS_UNIT_ACK
 *  Used internally within the falcon to track completed commands when updating
 *  get (tail) command queue pointers.
 */
#define RM_FLCN_QUEUE_HDR_FLAGS_STATUS              NVBIT(0)
#define RM_FLCN_QUEUE_HDR_FLAGS_EVENT               NVBIT(2)
#define RM_FLCN_QUEUE_HDR_FLAGS_UNIT_ACK            NVBIT(5)

#endif  // FLCNIFCMN_H
