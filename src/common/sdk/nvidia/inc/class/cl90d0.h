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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl90d0.finn
//

#include "class/cl90cd.h"
#include "class/cl90cdtypes.h"

/*
 * NV_OPERATIONAL_EVENT_BUFFER_BIND
 *
 *   Subscription object for operational-event delivery through an existing
 *   NV_EVENT_BUFFER. The bind object carries the operational scope, filters,
 *   and capability descriptors for its lifetime.
 */
#define NV_OPERATIONAL_EVENT_BUFFER_BIND                       (0x90d0U) /* finn: Evaluated from "NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS_MESSAGE_ID" */

#define NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_INVALID      (0U)
#define NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE          (1U)
#define NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_CPER         (2U)

/*
 * Initial implementation supports DEVICE scope. MIG scopes are ABI-defined for
 * planned support and return NV_ERR_NOT_SUPPORTED until that support lands.
 */
#define NV_OPERATIONAL_EVENT_BUFFER_SCOPE_DEVICE               (0U)
#define NV_OPERATIONAL_EVENT_BUFFER_SCOPE_MIG_GPU_INSTANCE     (1U)
#define NV_OPERATIONAL_EVENT_BUFFER_SCOPE_MIG_COMPUTE_INSTANCE (2U)

/*
 * Capability descriptors are reserved for future capability enforcement. The
 * current path is CAP_SYS_ADMIN gated; callers should pass zeroed descriptors
 * until validation support lands. Nonzero descriptors may return
 * NV_ERR_NOT_SUPPORTED.
 */
#define NV_OPERATIONAL_EVENT_BUFFER_CAP_SLOTS                  (8U)
#define NV_OPERATIONAL_EVENT_BUFFER_CAP_SCOPE                  (0U)
#define NV_OPERATIONAL_EVENT_BUFFER_CAP_MIG_MONITOR            (1U)
#define NV_OPERATIONAL_EVENT_BUFFER_CAP_DEBUG_MONITOR          (2U)

#define NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_HISTORY    (0ULL)
#define NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_AFTER_BIND (0xFFFFFFFFFFFFFFFFULL)

#define NV_OPERATIONAL_EVENT_RECORD_SUBTYPE_GOE_V1             (1U)
#define NV_OPERATIONAL_EVENT_RECORD_SUBTYPE_CPER_V1            (1U)

#define NV_OPERATIONAL_EVENT_SEVERITY_ALL                      (0U)
#define NV_OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL            (10U)
#define NV_OPERATIONAL_EVENT_SEVERITY_CORRECTED                (20U)
#define NV_OPERATIONAL_EVENT_SEVERITY_RECOVERABLE              (30U)
#define NV_OPERATIONAL_EVENT_SEVERITY_FATAL                    (40U)

#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL                 (0U)
#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_TELEMETRY           (10U)
#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_DIAGNOSTIC          (20U)
#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE              (30U)
#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING             (40U)
#define NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR               (50U)

#define NV_OPERATIONAL_EVENT_ATTR_UNCONTAINED              0:0
#define NV_OPERATIONAL_EVENT_ATTR_UNCONTAINED_FALSE        0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_UNCONTAINED_TRUE         0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_LATENT                   1:1
#define NV_OPERATIONAL_EVENT_ATTR_LATENT_FALSE             0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_LATENT_TRUE              0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_PROPAGATED               2:2
#define NV_OPERATIONAL_EVENT_ATTR_PROPAGATED_FALSE         0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_PROPAGATED_TRUE          0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_COMPONENT_RESET          3:3
#define NV_OPERATIONAL_EVENT_ATTR_COMPONENT_RESET_FALSE    0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_COMPONENT_RESET_TRUE     0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_THRESHOLD_EXCEEDED       4:4
#define NV_OPERATIONAL_EVENT_ATTR_THRESHOLD_EXCEEDED_FALSE 0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_THRESHOLD_EXCEEDED_TRUE  0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_PRIMARY                  5:5
#define NV_OPERATIONAL_EVENT_ATTR_PRIMARY_FALSE            0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_PRIMARY_TRUE             0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_OVERFLOW                 6:6
#define NV_OPERATIONAL_EVENT_ATTR_OVERFLOW_FALSE           0x00000000U
#define NV_OPERATIONAL_EVENT_ATTR_OVERFLOW_TRUE            0x00000001U
#define NV_OPERATIONAL_EVENT_ATTR_RESERVED                 7:7

#define NV_OPERATIONAL_EVENT_GROUP_ATTR_RECOVERED       0:0
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_RECOVERED_FALSE 0x00000000U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_RECOVERED_TRUE  0x00000001U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_PREVERR         1:1
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_PREVERR_FALSE   0x00000000U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_PREVERR_TRUE    0x00000001U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED       2:2
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED_FALSE 0x00000000U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED_TRUE  0x00000001U
#define NV_OPERATIONAL_EVENT_GROUP_ATTR_RESERVED        7:3

/*
 * NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS
 *
 *   hEventBuffer [IN]
 *       Existing NV_EVENT_BUFFER handle that receives matching records.
 *
 *   recordFormat [IN]
 *       Requested delivery format, one of NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_*.
 *
 *   scope [IN]
 *       Operational subscription scope.
 *
 *   resourceUuid [IN]
 *       UUID for the resource selected by scope.
 *
 *   capDescriptors [IN]
 *       Opaque descriptor slots for future capability validation.
 *
 *   minLogLevel [IN]
 *       Whole-group log-level filter evaluated over the scope-visible subset.
 *       NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL matches every emitted log level.
 *
 *   minSeverity [IN]
 *       Whole-group severity filter evaluated over the scope-visible subset.
 *       NV_OPERATIONAL_EVENT_SEVERITY_ALL matches every emitted severity.
 *
 *   startingCursor [IN]
 *       NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_HISTORY requests history;
 *       NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_AFTER_BIND requests new
 *       groups only.
 *
 *   bindId [OUT]
 *       Opaque 8-bit EventBuffer-scoped identifier emitted in records.
 */
#define NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS_MESSAGE_ID (0x90d0U)

typedef struct NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS {
    NvHandle hEventBuffer;
    NvU8     recordFormat;
    NvU8     scope;
    NvU8     minLogLevel;
    NvU8     minSeverity;
    NvU8     bindId;
    NvU8     _rsvd[3];
    NvU8     resourceUuid[16];
    NV_DECLARE_ALIGNED(NvU64 startingCursor, 8);
    NV_DECLARE_ALIGNED(NvU64 capDescriptors[NV_OPERATIONAL_EVENT_BUFFER_CAP_SLOTS], 8);
} NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS;

/*
 * NV_OPERATIONAL_EVENT_RECORD
 *
 * Common operational-event EventBuffer record prefix.
 *
 *   header
 *       Generic EventBuffer record header. Its type/subtype identify the
 *       operational payload shape, and varData points at any associated
 *       vardata payload.
 *
 *   traceId
 *       Full 64-bit originator-prefixed trace identifier. This is the
 *       causal-chain correlation key.
 *
 *   instanceId
 *       Originator-scoped event sequence identifier.
 *
 *   timestamp
 *       Event timestamp in microseconds.
 *
 *   moduleSignature / eventCategory / moduleEventCode
 *       Source module and module-local event identity.
 *
 *   severity
 *       Operational severity value for the event.
 *
 *   attributes
 *       Per-event operational attributes bitfield.
 *
 *   groupAttributes
 *       Group-level qualifiers shared by all records emitted for the same
 *       operational event group.
 *
 *   bindId
 *       Opaque EventBuffer-scoped bind identifier used to serialize this
 *       record.
 *
 *   cursorValue
 *       Operational group cursor. When this group is encoded as CPER,
 *       cursorValue equals the CPER Error Record Header recordId and is the
 *       primary in-band GOE to OOB CPER group correlation key.
 *
 *   numContexts / totalContextSize
 *       Count and total byte size of context records in vardata.
 *
 *   groupCperSize
 *       Total CPER encoding size for this group, or zero when unavailable.
 *
 *   groupSize / groupIndex
 *       Original producer-side group metadata: the count of records the
 *       producer composed into this scope-visible group and this record's
 *       index within it. NOT a delivery guarantee -- under back-pressure
 *       the producer may drop individual records, so consumers may see
 *       a span of records that share a cursorValue / groupSize value
 *       but with groupIndex values that skip (e.g. 0, 1, 3 of a
 *       groupSize=5 group is a valid observation). Reassembly logic
 *       must tolerate missing indices.
 */
typedef struct NV_OPERATIONAL_EVENT_RECORD {
    NV_EVENT_BUFFER_RECORD_HEADER header;
    NV_DECLARE_ALIGNED(NvU64 traceId, 8);
    NV_DECLARE_ALIGNED(NvU64 instanceId, 8);
    NV_DECLARE_ALIGNED(NvU64 timestamp, 8);
    char                          moduleSignature[16];
    NvU16                         eventCategory;
    NvU16                         moduleEventCode;
    NvU8                          severity;
    NvU8                          attributes;
    NvU8                          groupAttributes;
    NvU8                          bindId;
    NV_DECLARE_ALIGNED(NvU64 cursorValue, 8);
    NvU32                         numContexts;
    NvU32                         totalContextSize;
    NvU32                         groupCperSize;
    NvU8                          groupSize;
    NvU8                          groupIndex;
    NvU8                          _rsvd2[2];
} NV_OPERATIONAL_EVENT_RECORD;

/*
 * NV_GPU_OPERATIONAL_EVENT_RECORD
 *
 * GPU operational-event record. The first member is the common operational
 * record prefix, whose first member is NV_EVENT_BUFFER_RECORD_HEADER.
 *
 *   event
 *       Common operational-event, group, and delivery metadata.
 *
 *   deviceUuid
 *       GPU UUID for the affected device.
 *
 *   scope
 *       GPU_OPERATIONAL_EVENT_SCOPE value for the event's impact scope.
 *
 *   originator
 *       GPU_OPERATIONAL_EVENT_ORIGINATOR value for the event bus that assigned
 *       the trace and sequence identifiers.
 *
 *   moduleInstance / chipletId / migAttribution
 *       GPU-local attribution fields used by the GOE profile.
 *
 *   logLevel
 *       Per-event log level used by subscription filtering.
 *
 *   reportingSource
 *       GPU_OPERATIONAL_EVENT_REPORTING_SOURCE value classifying how the
 *       producer detected the event (SW_CHECK / INTERRUPT / FW_FAULT /
 *       TIMEOUT / UNKNOWN). The kernel-side OpEventLog feeds this into CPER
 *       notify type selection.
 */
typedef struct NV_GPU_OPERATIONAL_EVENT_RECORD {
    NV_DECLARE_ALIGNED(NV_OPERATIONAL_EVENT_RECORD event, 8);
    NvU8 deviceUuid[16];
    NvU8 scope;
    NvU8 originator;
    NvU8 moduleInstance;
    NvU8 chipletId;
    NvU8 migAttribution;
    NvU8 logLevel;
    NvU8 reportingSource;
    NvU8 _rsvd[1];
} NV_GPU_OPERATIONAL_EVENT_RECORD;

/*
 * NV_OPERATIONAL_EVENT_CONTEXT
 *
 * Context records are stored back-to-back in vardata. FINN emits the fixed
 * record prefix; payload bytes immediately follow this prefix and are padded
 * to NV_EVENT_VARDATA_GRANULARITY before the next context record.
 *
 *   contextType
 *       Data format type for this context definition.
 *
 *   dataFormatVersion
 *       Version of this context type's format.
 *
 *   dataSize
 *       Size of the context payload in bytes, excluding alignment padding.
 */
typedef struct NV_OPERATIONAL_EVENT_CONTEXT {
    NvU16 contextType;
    NvU16 dataFormatVersion;
    NvU32 dataSize;
} NV_OPERATIONAL_EVENT_CONTEXT;

typedef NV_OPERATIONAL_EVENT_CONTEXT NV_GPU_OPERATIONAL_EVENT_CONTEXT;

/*
 * NV_OPERATIONAL_EVENT_CPER_RECORD
 *
 * EventBuffer wrapper for a CPER blob stored in vardata.
 *
 *   header
 *       Generic EventBuffer record header. Its type/subtype identify the CPER
 *       wrapper and varData points at the CPER bytes in vardata.
 *
 *   cursorValue
 *       Operational group cursor. This matches NV_OPERATIONAL_EVENT_RECORD
 *       cursorValue and the CPER Error Record Header recordId for the encoded
 *       group.
 *
 *   cperSize
 *       Size in bytes of the CPER payload stored in vardata.
 *
 *   bindId
 *       Opaque EventBuffer-scoped bind identifier used to serialize this
 *       record.
 */
typedef struct NV_OPERATIONAL_EVENT_CPER_RECORD {
    NV_EVENT_BUFFER_RECORD_HEADER header;
    NV_DECLARE_ALIGNED(NvU64 cursorValue, 8);
    NvU32                         cperSize;
    NvU8                          bindId;
    NvU8                          _rsvd[3];
} NV_OPERATIONAL_EVENT_CPER_RECORD;

