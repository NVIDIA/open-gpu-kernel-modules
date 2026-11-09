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

#if defined(SRT_BUILD)
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#include "g_bus_events_nvoc.h"

#ifndef _BUS_EVENTS_H_
#define _BUS_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/bus/bus_event_defs.h"

typedef struct BusC2cContainmentCtx
{
    EventContextHeader header;
    NvU32 containmentCode;
} BusC2cContainmentCtx;

NVOC_EVENT(busc2ccontnmt, export) class BusC2cContainment : public GpuInterconnectError
{
public:
    BusC2cContainmentCtx context;
    GpuXidEventCtx xid179;

    NV_STATUS busc2ccontnmtConstruct(BusC2cContainment *pEvent,
                                     EventBus *pEventBus,
                                     NvU32 severity,
                                     NvU32 containmentCode)
    : GpuInterconnectError(pEventBus,
                           "GPU-BUS",
                           BUS_OP_EVENT_CODE_C2C_CONTAINMENT,
                           severity,
                           REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
                           GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
                           GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT,
                           GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
};

NVOC_EVENT(busgpufob, export) class BusGpuFallenOffBus : public GpuInterconnectError
{
public:
    GpuXidEventCtx xid79;

    NV_STATUS busgpufobConstruct(BusGpuFallenOffBus *pEvent,
                                 EventBus *pEventBus)
    : GpuInterconnectError(pEventBus,
                           "GPU-BUS",
                           BUS_OP_EVENT_CODE_GPU_FALLEN_OFF_BUS,
                           OPERATIONAL_EVENT_SEVERITY_FATAL,
                           REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
                           GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
                           GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK,
                           GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
};

NV_STATUS busc2ccontnmtDeserialize(OBJGPU *pGpu,
                                   const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
                                   const NvU8 *pContextData,
                                   NvU32 contextDataSize);

NV_STATUS busgpufobDeserialize(OBJGPU *pGpu,
                               const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
                               const NvU8 *pContextData,
                               NvU32 contextDataSize);

#endif // _BUS_EVENTS_H_

