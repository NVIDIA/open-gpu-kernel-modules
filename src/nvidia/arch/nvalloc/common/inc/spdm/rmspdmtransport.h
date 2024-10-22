/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RM_SPDM_TRANSPORT_H_
#define _RM_SPDM_TRANSPORT_H_

/* ------------------------- Macros and Defines ----------------------------- */
//
// The following defines and macros are shared for any message-related constants
// shared between RM and endpoint.
//

// Defines used for polling-based communication between Kernel-RM and GSP.
#define NV_SPDM_PARTITION_BOOT_SUCCESS           (0x59D3B007)
#define NV_SPDM_REQUESTER_SECRETS_DERIVED        (0x59D35DED)
#define NV_SPDM_NO_MESSAGE_PENDING_TOKEN         (0x00000000)
#define NV_SPDM_REQUESTER_MESSAGE_PENDING_TOKEN  (0xAFAFAFAF)
#define NV_SPDM_RESPONDER_MESSAGE_PENDING_TOKEN  (0xFAFAFAFA)
#define NV_SPDM_SECRET_TEARDOWN_SUCCESS          (0x59D35EDE)
#define NV_SPDM_SECRET_TEARDOWN_FAILURE          (0x59D35EFA)
#define NV_SPDM_SECRET_TEARDOWN_ACK              (0x59D35EAC)

//
// The SPDM communication between Kernel-RM and GSP utilizes a buffer in sysmem accessible
// to both endpoints. The sysmem buffer will contain both a custom NV-defined header
// describing the message type and size, and the SPDM payload itself.
//

//
// Size of the full transportation buffer (header + SPDM payload) allocated in sysmem.
// We need to align sysmem allocations with page size, but need less space. So we
// over-allocate and tell GSP we only have the size we actually need.
//
#define NV_SPDM_SYSMEM_SURFACE_SIZE_PAGE_ALIGNED  (0x2000)
#define NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES      (0x1200)
#define NV_SPDM_SYSMEM_SURFACE_ALIGNMENT_IN_BYTES (256)

// Struct and defines for the header which prepends the SPDM payload.
#pragma pack(1)
typedef struct _NV_SPDM_DESC_HEADER
{
    NvU32 msgType;
    NvU32 msgSizeByte;
    NvU32 version;
} NV_SPDM_DESC_HEADER, *PNV_SPDM_DESC_HEADER;
#pragma pack()

#define NV_SPDM_MESSAGE_TYPE_NORMAL               (0)
#define NV_SPDM_MESSAGE_TYPE_SECURED              (1)
#define NV_SPDM_MESSAGE_TYPE_APPLICATION          (2)
#define NV_SPDM_DESC_HEADER_VERSION_1_0           (0x10)
#define NV_SPDM_DESC_HEADER_VERSION_CURRENT       (NV_SPDM_DESC_HEADER_VERSION_1_0)
#define NV_SPDM_DESC_HEADER_ALIGNMENT_IN_BYTES    (NV_SPDM_SYSMEM_SURFACE_ALIGNMENT_IN_BYTES)

// Define utilized to determine size available for SPDM payload
#define NV_SPDM_MAX_SPDM_PAYLOAD_SIZE             (NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES - sizeof(NV_SPDM_DESC_HEADER))
#define NV_SPDM_MAX_TRANSCRIPT_BUFFER_SIZE        (2 * NV_SPDM_MAX_SPDM_PAYLOAD_SIZE)

// Limited by the transport size, do not increase without increasing transport buffer.
#define NV_SPDM_MAX_RANDOM_MSG_BYTES              (0x0)

#ifdef NVRM
#include "gpu/mem_mgr/mem_desc.h"
#else
#ifndef ADDR_SYSMEM
// System memory (PCI)
#define ADDR_SYSMEM                    (1)
#endif // ADDR_SYSMEM
#endif //NVRM

#define NV_SPDM_DMA_ADDR_SPACE_DEFAULT (ADDR_SYSMEM)
#define NV_SPDM_DMA_REGION_ID_DEFAULT  (0)

#endif // _RM_SPDM_TRANSPORT_H_
