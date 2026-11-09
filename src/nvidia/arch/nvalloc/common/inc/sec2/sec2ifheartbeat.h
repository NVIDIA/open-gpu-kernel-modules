/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef _SEC2IFHEARTBEAT_H_
#define _SEC2IFHEARTBEAT_H_

/*!
 * @file   sec2ifheartbeat.h
 * @brief  Shared SEC2 authenticated-heartbeat register protocol.
 */

//
// SEC2 autonomously publishes one authenticated snapshot every 200 ms:
//
//   NV_PSEC_FALCON_COMMON_SCRATCH_GROUP_0(0..3) 128-bit AES-256-GCM authentication tag
//   NV_PSEC_FALCON_COMMON_SCRATCH_GROUP_1(0)    AES-256-GCM encrypted 32-bit nonce
//   NV_PSEC_FALCON_COMMON_SCRATCH_GROUP_1(1)    Dynamic 32-bit portion of the GCM IV
//   NV_PSEC_FALCON_COMMON_SCRATCH_GROUP_1(2)    Bi-direction handshake
//
#define RM_SEC2_HEARTBEAT_TAG_0_PSEC_GROUP_0_INDEX    (0U)
#define RM_SEC2_HEARTBEAT_TAG_1_PSEC_GROUP_0_INDEX    (1U)
#define RM_SEC2_HEARTBEAT_TAG_2_PSEC_GROUP_0_INDEX    (2U)
#define RM_SEC2_HEARTBEAT_TAG_3_PSEC_GROUP_0_INDEX    (3U)
// 
#define RM_SEC2_HEARTBEAT_NONCE_EXCHANGE_PSEC_GROUP_1_INDEX (0U)
#define RM_SEC2_HEARTBEAT_IV_PSEC_GROUP_1_INDEX             (1U)
#define RM_SEC2_HEARTBEAT_HANDSHAKE_PSEC_GROUP_1_INDEX      (2U)

#define RM_SEC2_HEARTBEAT_UPDATE_PERIOD_USEC           (200000U) // 0.2 sec

// The PSEC common scratch groups are not part of the published RM manuals, so
// keep their shared CPU-visible PRI addresses in this interface.
#define RM_SEC2_HEARTBEAT_PSEC_GROUP_0_BASE       (0x00840300U)
#define RM_SEC2_HEARTBEAT_PSEC_GROUP_1_BASE       (0x00840310U)
#define RM_SEC2_HEARTBEAT_PSEC_REG(base, index)   ((base) + ((index) * 4U))

#define RM_SEC2_HEARTBEAT_TAG_REG_0               \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_0_BASE, \
                                RM_SEC2_HEARTBEAT_TAG_0_PSEC_GROUP_0_INDEX)

#define RM_SEC2_HEARTBEAT_TAG_REG_1               \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_0_BASE, \
                                RM_SEC2_HEARTBEAT_TAG_1_PSEC_GROUP_0_INDEX)

#define RM_SEC2_HEARTBEAT_TAG_REG_2               \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_0_BASE, \
                                RM_SEC2_HEARTBEAT_TAG_2_PSEC_GROUP_0_INDEX)

#define RM_SEC2_HEARTBEAT_TAG_REG_3               \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_0_BASE, \
                                RM_SEC2_HEARTBEAT_TAG_3_PSEC_GROUP_0_INDEX)

#define RM_SEC2_HEARTBEAT_NONCE_EXCHANGE_REG      \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_1_BASE, \
                                RM_SEC2_HEARTBEAT_NONCE_EXCHANGE_PSEC_GROUP_1_INDEX)

#define RM_SEC2_HEARTBEAT_IV_REG                  \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_1_BASE, \
                                RM_SEC2_HEARTBEAT_IV_PSEC_GROUP_1_INDEX)

#define RM_SEC2_HEARTBEAT_HANDSHAKE_REG            \
    RM_SEC2_HEARTBEAT_PSEC_REG(RM_SEC2_HEARTBEAT_PSEC_GROUP_1_BASE, \
                                RM_SEC2_HEARTBEAT_HANDSHAKE_PSEC_GROUP_1_INDEX)

//
// Runtime AES-GCM IV format:
//                   
// Total 12 bytes. Set from low address to high address, || means concatenate
//  { 4-bytes nonce-A || 4-bytes nonce-B || 4-bytes nonce-C }
// nonce-c = (nonce-A ^ nonce-B)
//
#define RM_SEC2_HEARTBEAT_NONCE_SIZE_BYTES             (4)

#define RM_SEC2_HEARTBEAT_IV_SIZE_BYTES                (12U)
#define RM_SEC2_HEARTBEAT_IV_SIZE_WORDS                (RM_SEC2_HEARTBEAT_IV_SIZE_BYTES / 4U)

#define RM_SEC2_HEARTBEAT_TAG_SIZE_BYTES               (16U)
#define RM_SEC2_HEARTBEAT_TAG_SIZE_WORDS               (RM_SEC2_HEARTBEAT_TAG_SIZE_BYTES / 4U)

#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT             7:0 
#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_UNKNOWN     0
#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_SEC2_READY  0x01
#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_SEC2_ERROR  0x02
#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_RM_READY    0x03
#define NV_RM_SEC2_HEARTBEAT_HANDSHAKE_EVENT_RM_ERROR    0x04

#define RM_SEC2_HEARTBEAT_QUEUE_WORKITEM_RETRY_COUNT     (3)

#endif // _SEC2IFHEARTBEAT_H_
