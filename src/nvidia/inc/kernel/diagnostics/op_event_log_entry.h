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

#ifndef _OP_EVENT_LOG_ENTRY_H_
#define _OP_EVENT_LOG_ENTRY_H_

#if defined(SRT_BUILD)
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#include "nvtypes.h"
#include "nvstatus.h"

//
// Wire-format renderers for OpEventLog entries.
//
// Today the log's typed subtype is exclusively GPU operational events
// (GpuOperationalEvent / GpuXidEventCtx), so this module owns the wire
// encoding for that single subtype. If future work introduces additional
// operational-event subtypes (e.g. via an OpEventData / GpuOpEventData
// union direction), a sibling module per subtype is the natural
// extension and the push-path dispatch picks the right one based on
// the entry's subtype tag.
//

struct OpEventLogEntry;

//
// Render an event log entry's event as an NV_GPU_OPERATIONAL_EVENT_RECORD
// body (post-header) plus its serialized GOE context in the vardata
// buffer.
//
NV_STATUS opEventLogEntryRenderToEventBufferGoe(const struct OpEventLogEntry *pEntry,
                                                NvU32 eventIndex, NvU8 groupSize,
                                                NvU8 groupIndex, NvU8 bindId,
                                                NvU8 *pInlinePayload, NvU32 inlinePayloadCap,
                                                NvU8 *pVardata, NvU32 vardataCap,
                                                NvU32 *pInlinePayloadWritten,
                                                NvU32 *pVardataWritten);

NV_STATUS opEventLogEntryRenderToEventBufferCperSubset(const struct OpEventLogEntry *pEntry,
                                                       const NvU8 *pEventIndices,
                                                       NvU32 eventIndexCount,
                                                       NvU8 bindId,
                                                       NvU8 *pInlinePayload,
                                                       NvU32 inlinePayloadCap,
                                                       NvU8 *pVardata,
                                                       NvU32 vardataCap,
                                                       NvU32 *pInlinePayloadWritten,
                                                       NvU32 *pVardataWritten);

//
// Build a CPER record into pBuffer for an event log entry. Also used by
// the RM API SYSTEM_READ_CPER path.
//
NV_STATUS opEventLogEntryRenderCper(const struct OpEventLogEntry *pEntry,
                                    NvU8 *pBuffer, NvU32 bufferCap,
                                    NvU32 *pRecordSize);

#endif // _OP_EVENT_LOG_ENTRY_H_
