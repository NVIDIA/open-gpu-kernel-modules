/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * GSP MESSAGE QUEUE
 */

#ifndef _MESSAGE_QUEUE_PRIV_H_
#define _MESSAGE_QUEUE_PRIV_H_

#include "msgq/msgq.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/conf_compute/ccsl.h"
#include "nvctassert.h"

// Shared memory layout.
//
// Each of the following are page aligned:
//   Page table for entire shared memory layout.
//   Command queue header
//   Command queue entries
//   Status queue header
//   Status queue entries

// NOTE: GSP Message Queue encryption is currently *NOT* ABI stable
typedef struct GSP_MSG_QUEUE_ENCRYPTION_TAG
{
    NvU8  authTagBuffer[16];         // Authentication tag buffer
} GSP_MSG_QUEUE_ENCRYPTION_TAG;

//
// A GSP message queue element is structured as layered protocol headers:
//
//   MCTP transport -> [optional encryption tag] -> NVDM -> RPC/GMCAPI payload
//   |-----------------plaintext------------------|-encrypted (if CC enabled)-|
//
// The MCTP transport layer provides overall framing: routing info, the NVDM
// message type (per the MCTP specification, the message type is encoded in the
// MCTP header, not the NVDM header), and the total transfer size including any
// padding needed for encryption alignment.
//
// When confidential compute is enabled, an AEAD authentication tag sits
// between the MCTP and NVDM layers. Everything after the tag - the NVDM header,
// payload size, and all payload data - is encrypted. The tag itself and
// the MCTP framing above it remain in plaintext.
//
// The NVDM layer carries the payload size and is followed by the actual
// protocol-specific data: either a vGPU RPC header or a GMCAPI header, each
// with their own length fields and command-specific parameters.
//
// For example, consider a GMCAPI message with a payload of 100 bytes, with CC:
//   Overall size of the message would be:
//     16 bytes for the MCTP header (4x NvU32)
//     16 bytes for the encryption tag
//     8 bytes for the NVDM header (size+reserved)
//     32 bytes for the GMCAPI_HEADER (see gmcapi_base.h)
//     100 bytes for the GMCAPI payload
//     3924 bytes for the padding to align the message to 4kb
//
//   The sizes would be:
//     mctpPayloadSize = 4096
//     nvdmPayloadSize = 132
//     GMCAPI_HEADER.length = 100
//
//   Without CC, we would not have the encryption tag and the padding, so:
//     mctpPayloadSize = 156
//     nvdmPayloadSize = 132
//     GMCAPI_HEADER.length = 100
//
typedef struct NV_ABI_STABLE GSP_MSG_QUEUE_ELEMENT
{
#define MCTP_MAGIC      0x4D435450   // "MCTP"
#define MCTP_MAGIC_SEEN 0x5345454E   // "SEEN"
    NvU32 mctpMagic;                 // "MCTP"
    NvU32 mctpPayloadSize;           // Size of the full (encrypted?) payload
    NvU32 mctpHeader;                // MCTP transport header
    NvU32 nvdmHeader;                // NVDM over MCTP header

#ifdef GSPRM_HWASAN_ENABLE
    // This is required to align `payload` on a granule-boundary. The specific
    // alignment has to do with implementation details of shadow memory. For
    // our purposes, we just need to have it be 16-byte aligned.
    NvU8 __hwasan_meta_chromatics_padding[8];
#endif
    union
    {
        struct
        {
            GSP_MSG_QUEUE_ENCRYPTION_TAG encryptionTag;
            NvU32 nvdmPayloadSize;
            NvU32 reserved;
            NvU8 payload[];  // vGPU RPC or GMCAPI payload
        } withEncryption;
        struct
        {
            NvU32 nvdmPayloadSize;
            NvU32 reserved;
            NvU8 payload[];  // vGPU RPC or GMCAPI payload
        } noEncryption;
    };
} GSP_MSG_QUEUE_ELEMENT;

#define GSP_MSG_QUEUE_ELEMENT_ENCRYPTION_OFFSET \
    (NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, withEncryption) + sizeof(GSP_MSG_QUEUE_ENCRYPTION_TAG))

#define GSP_MSG_QUEUE_ELEMENT_SIZE_WITH_ENCRYPTION sizeof(GSP_MSG_QUEUE_ELEMENT)
#define GSP_MSG_QUEUE_ELEMENT_SIZE_NO_ENCRYPTION \
    (NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, withEncryption) + 2 * sizeof(NvU32))

ct_assert(GSP_MSG_QUEUE_ELEMENT_SIZE_WITH_ENCRYPTION ==
    (GSP_MSG_QUEUE_ELEMENT_SIZE_NO_ENCRYPTION + sizeof(GSP_MSG_QUEUE_ENCRYPTION_TAG)));

#ifdef GSPRM_HWASAN_ENABLE
// Ensure payload is granule-aligned (16 bytes) for both encryption modes
ct_assert((NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, withEncryption.payload) % 16) == 0);
ct_assert((NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, noEncryption.payload) % 16) == 0);
#endif


typedef struct _message_queue_info
{
    // Parameters
    NvLength               commandQueueSize;
    NvLength               statusQueueSize;
    NvLength               queueElementHdrSize;
    NvLength               queueElementSizeMin;
    NvLength               queueElementSizeMax;
    NvU32                  queueHeaderAlign;
    NvU32                  queueElementAlign;

    // Shared memory area.
    void                  *pCommandQueue;
    void                  *pStatusQueue;
    rpc_message_header_v  *pRpcMsgBuf;    // RPC message buffer VA.

    // Other CPU-side fields
    void                  *pWorkArea;
    GSP_MSG_QUEUE_ELEMENT *pCmdQueueElement;    // Working copy of command queue element.
    void                  *pMetaData;
    msgqHandle             hQueue;              // Do not allow requests when hQueue is null.
    NvU32                  txBufferFull;
    NvU32                  queueIdx;            // QueueIndex used to identify which task the message is supposed to be sent to.
    NvBool                 bErrorInjectionEnabled;
    NvBool                 bEncryptionEnabled;
} MESSAGE_QUEUE_INFO;

typedef struct MESSAGE_QUEUE_COLLECTION
{
    // Parameters
    NvLength               pageTableEntryCount;
    NvLength               pageTableSize;

    // Shared memory area.
    MEMORY_DESCRIPTOR     *pSharedMemDesc;
    RmPhysAddr             sharedMemPA;   // Page table for all of shared mem.

    MESSAGE_QUEUE_INFO rpcQueues[RPC_QUEUE_COUNT];
} MESSAGE_QUEUE_COLLECTION;

static NV_INLINE NvLength
gspMsgQueueGetMaxRpcSize(NvLength queueElementSizeMax, NvLength queueElementHdrSize)
{
    return queueElementSizeMax - queueElementHdrSize;
}

static NV_INLINE NvU32
gspMsgQueueBytesToElements(NvU32 bytes, NvLength queueElementSizeMin)
{
    return NV_DIV_AND_CEIL(bytes, queueElementSizeMin);
}

static NV_INLINE GSP_MSG_QUEUE_ENCRYPTION_TAG *
gspMsgQueueGetEncryptionTag(GSP_MSG_QUEUE_ELEMENT *pQueueElem)
{
    return &pQueueElem->withEncryption.encryptionTag;
}

static NV_INLINE void *
gspMsgQueueGetRpcMessageHeader
(
    MESSAGE_QUEUE_INFO *pMQI,
    GSP_MSG_QUEUE_ELEMENT *pQueueElem
)
{
    return (pMQI->bEncryptionEnabled) ? pQueueElem->withEncryption.payload : pQueueElem->noEncryption.payload;
}

static NV_INLINE NV_STATUS
gspMsgQueueCCEncrypt
(
    void *pCcslCtx,
    MESSAGE_QUEUE_INFO *pMQI,
    GSP_MSG_QUEUE_ELEMENT *pElement,
    NvU32 payloadSize
)
{
    GSP_MSG_QUEUE_ENCRYPTION_TAG *pCcTag = gspMsgQueueGetEncryptionTag(pElement);
    NvU8 *pRpcPayload = (NvU8 *)pElement + GSP_MSG_QUEUE_ELEMENT_ENCRYPTION_OFFSET;

    return ccslEncryptWithRotationChecks(pCcslCtx, payloadSize, pRpcPayload,
                                         NULL, 0,
                                         pRpcPayload, pCcTag->authTagBuffer);
}

static NV_INLINE NV_STATUS
gspMsgQueueCCDecrypt
(
    void *pCcslCtx,
    MESSAGE_QUEUE_INFO *pMQI,
    GSP_MSG_QUEUE_ELEMENT *pElement,
    NvU32 payloadSize
)
{
    GSP_MSG_QUEUE_ENCRYPTION_TAG *pCcTag = gspMsgQueueGetEncryptionTag(pElement);
    NvU8 *pRpcPayload = (NvU8 *)pElement + GSP_MSG_QUEUE_ELEMENT_ENCRYPTION_OFFSET;

    return ccslDecryptWithRotationChecks(pCcslCtx, payloadSize, pRpcPayload,
                                         NULL, NULL, 0,
                                         pRpcPayload, pCcTag->authTagBuffer);
}

#endif // _MESSAGE_QUEUE_PRIV_H_
