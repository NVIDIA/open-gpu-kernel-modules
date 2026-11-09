/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

// TDISP Defines for polling-based communication between Kernel-RM and GSP.
#define NV_TDISP_TSM_READY_TO_START             (0xA7010102U)
#define NV_TDISP_DSM_READY_TO_START             (0xA7010103U)
#define NV_TDISP_TSM_RUN_STATE_SUCCESS          (0xA7010104U)
#define NV_TDISP_DSM_RUN_STATE_SUCCESS          (0xA7010105U)
#define NV_TDISP_TSM_VALIDATION_COMPLETED       (0xA7010106U)
#define NV_TDISP_DSM_VALIDATION_ACK             (0xA7010107U)
#define NV_TDISP_DSM_RUN_STATE_FAILURE          (0xA7010108U)
#define NV_TDISP_DSM_INIT_FAILURE               (0xA7010109U)
#define NV_TDISP_TSM_PROCEED_TO_GSPRM           (0xA701010BU)
#define NV_TDISP_DSM_UNLOAD_START               (0xA701010CU)
#define NV_TDISP_DSM_UNLOAD_SUCCESS             (0xA701010DU)
#define NV_TDISP_DSM_UNLOAD_FAILURE             (0xA701010EU)
#define NV_TDISP_DSM_UNLOAD_PROCEED             (0xA701010FU)

// Keep local: this shared transport header cannot include RM-private register headers.
#define NV_TDISP_GSP_SCRATCH_PF_RW_REG(i)       (0xD80100U + ((i) * 4U))

//
// Lower alias of the PF read/write GSP scratch range. Decoder-trap slot 31 in
// INIT and slot 16 in RUN redirect the permitted entries to
// NV_TDISP_GSP_SCRATCH_PF_RW_REG().
//
#define NV_TDISP_GSP_SCRATCH_PF_RW_ALIAS_REG(i) (0xEF0100U + ((i) * 4U))

#define NV_TDISP_DVSEC0_HEADER_2_AND_GENERAL    (0x00000AF8U)
#define NV_TDISP_DVSEC0_TDI_STATE_SHIFT         (30U)
#define NV_TDISP_DVSEC0_TDI_STATE_MASK          (0x3U)
#define NV_TDISP_DVSEC0_TDI_STATE_UNLOCKED      (0x0U)
#define NV_TDISP_DVSEC0_TDI_STATE_LOCKED        (0x1U)
#define NV_TDISP_DVSEC0_TDI_STATE_RUN           (0x2U)
#define NV_TDISP_DVSEC0_TDI_STATE_ERROR         (0x3U)
#define NV_TDISP_DVSEC0_TDI_STATE(reg)          (((reg) >> NV_TDISP_DVSEC0_TDI_STATE_SHIFT) & \
                                                 NV_TDISP_DVSEC0_TDI_STATE_MASK)

//
// The SPDM communication between Kernel-RM and GSP utilizes a buffer in sysmem accessible
// to both endpoints. The sysmem buffer will contain both a custom NV-defined header
// describing the message type and size, and the SPDM payload itself.
//

#define NV_SPDM_SYSMEM_SURFACE_SIZE_PAGE_ALIGNED  (0x2000)
#define NV_SPDM_SYSMEM_SURFACE_SIZE_DEFAULT       (0x1200)
#define NV_SPDM_SYSMEM_SURFACE_SIZE_CORIM         (0x2000)

#if defined(NVRM) || (defined(GENERATE_CORIM) && GENERATE_CORIM)
#define NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES      NV_SPDM_SYSMEM_SURFACE_SIZE_CORIM
#else
#define NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES      NV_SPDM_SYSMEM_SURFACE_SIZE_DEFAULT
#endif

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
