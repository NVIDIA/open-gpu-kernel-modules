/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _PCIE_EVENT_CTX_DEFS_H_
#define _PCIE_EVENT_CTX_DEFS_H_

#include "events/gpu/gpu_event_ctx_defs.h"
#include "nvtypes.h"

typedef NvU16 PCIE_EVENT_CTX_TYPE;
enum
{
    PCIE_EVENT_CTX_TYPES_BASE = GPU_OPERATIONAL_EVENT_CTX_TYPES_GPU_PCIE_BASE,
    PCIE_EVENT_CTX_TYPE_FATAL_TIMEOUT_DATA,             // 0xA601
    PCIE_EVENT_CTX_TYPE_CORRECTABLE_ERROR_FAULT_DATA,   // 0xA602
    PCIE_EVENT_CTX_TYPE_RESET_FAILURE_DATA,             // 0xA603
};

/*
 * PCIE Fatal Timeout Data (Context Type PCIE_EVENT_CTX_TYPE_FATAL_TIMEOUT_DATA, Version 1.0)
 *
 * Captures the XAL endpoint register state at the moment of the fatal timeout.
 * errorType identifies which timeout source fired; regVal is the raw register
 * value read from that source.
 *
 * Total size: 8 bytes
 */
#define PCIE_FATAL_TIMEOUT_DATA_VERSION_MAJOR 1
#define PCIE_FATAL_TIMEOUT_DATA_VERSION_MINOR 0
#define PCIE_FATAL_TIMEOUT_DATA_VERSION       \
    ((PCIE_FATAL_TIMEOUT_DATA_VERSION_MINOR) | \
     (PCIE_FATAL_TIMEOUT_DATA_VERSION_MAJOR << 8))

//
// Identifies which XAL endpoint register triggered the fatal timeout.
// Values correspond to timeout status registers
//
typedef NvU32 PCIE_FATAL_TIMEOUT_ERROR_TYPE;
enum
{
    PCIE_FATAL_ERR_TYPE_NONE              = 0U,
    PCIE_FATAL_ERR_TYPE_PRI_REQ_TIMEOUT   = 1U,
    PCIE_FATAL_ERR_TYPE_PRI_RSP_TIMEOUT   = 2U,
    PCIE_FATAL_ERR_TYPE_LMEM_TIMEOUT      = 3U,
    PCIE_FATAL_ERR_TYPE_TRS_TIMEOUT       = 4U
};

typedef struct PCIE_FATAL_TIMEOUT_DATA
{
    NvU32 errorType;
    NvU32 regVal;
} PCIE_FATAL_TIMEOUT_DATA;

/*
 * PCIE Correctable Error Fault Data
 * (Context Type PCIE_EVENT_CTX_TYPE_CORRECTABLE_ERROR_FAULT_DATA, Version 1.0)
 *
 * Captures the CE rate tuple identifying the correctable error threshold event.
 *
 * Total size: 12 bytes
 */
#define PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION_MAJOR 1
#define PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION_MINOR 0
#define PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION       \
    ((PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION_MINOR) | \
     (PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION_MAJOR << 8))

typedef struct PCIE_CORRECTABLE_ERROR_FAULT_DATA
{
    NvU32 ceRateTupleNum;
    NvU32 ceRateNerdNumber;
    NvU32 ceRateTupleVerNum;
} PCIE_CORRECTABLE_ERROR_FAULT_DATA;

/*
 * PCIE Reset Failure Data (Context Type PCIE_EVENT_CTX_TYPE_RESET_FAILURE_DATA, Version 1.0)
 *
 * Captures the status and recommended recovery action for a PCIe reset failure.
 *
 * Total size: 8 bytes
 */
#define PCIE_RESET_FAILURE_DATA_VERSION_MAJOR 1
#define PCIE_RESET_FAILURE_DATA_VERSION_MINOR 0
#define PCIE_RESET_FAILURE_DATA_VERSION       \
    ((PCIE_RESET_FAILURE_DATA_VERSION_MINOR) | \
     (PCIE_RESET_FAILURE_DATA_VERSION_MAJOR << 8))

typedef struct PCIE_RESET_FAILURE_DATA
{
    NvU32 resetStatus;
    NvU32 resetRecAction;
} PCIE_RESET_FAILURE_DATA;

#endif // _PCIE_EVENT_CTX_DEFS_H_
