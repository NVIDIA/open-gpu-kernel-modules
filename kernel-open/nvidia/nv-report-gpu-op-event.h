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

#include "conftest.h"

#undef TRACE_SYSTEM
#define TRACE_SYSTEM nvidia

#if !defined(_TRACE_NV_GPU_OP_EVENT_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_NV_GPU_OP_EVENT_H

#include <linux/tracepoint.h>
#include "events/gpu/gpu_event_defs.h"

DECLARE_EVENT_CLASS(nvidia_gpu_event_class,

    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),

    TP_ARGS(pdev, id),

    TP_STRUCT__entry(
        __string( device,   pdev ? pci_name(pdev) : "unknown")
        __array(  u8,   device_uuid, 16)
        __field(  u16,  category)
        __field(  u16,  event_code)
        __field(  u8,   severity)
        __field(  u8,   scope)
        __field(  u8,   originator)
        __field(  u8,   attributes)
        __field(  u8,   group_attributes)
        __field(  u8,   log_level)
        __field(  u64,  trace_id)
        __field(  u64,  instance_id)
        __field(  u64,  timestamp)
        __array(  char, module_sig, 16)
        __field(  u8,   module_instance)
        __field(  u8,   chiplet_id)
        __field(  u8,   mig_attribution)
    ),

    TP_fast_assign(
#if NV_ASSIGN_STR_ARGUMENT_COUNT == 1
        __assign_str(device);
#else
        __assign_str(device, pdev ? pci_name(pdev) : "unknown");
#endif
        memcpy(__entry->device_uuid, id->device_uuid, 16);
        __entry->category         = id->category;
        __entry->event_code       = id->event_code;
        __entry->severity         = id->severity;
        __entry->scope            = id->scope;
        __entry->originator       = id->originator;
        __entry->attributes       = id->attributes;
        __entry->group_attributes = id->group_attributes;
        __entry->log_level        = id->log_level;
        __entry->trace_id         = id->trace_id;
        __entry->instance_id      = id->instance_id;
        __entry->timestamp        = id->timestamp;
        memcpy(__entry->module_sig, id->module_sig, 16);
        __entry->module_instance  = id->module_instance;
        __entry->chiplet_id       = id->chiplet_id;
        __entry->mig_attribution  = id->mig_attribution;
    ),

    TP_printk(
        "gpu=%s cat=0x%x code=0x%x sev=%u log=%u trace=0x%llx",
        __get_str(device),
        __entry->category,
        __entry->event_code,
        __entry->severity,
        __entry->log_level,
        __entry->trace_id
    )
);

DEFINE_EVENT(nvidia_gpu_event_class, nvidia_gpu_error,
    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),
    TP_ARGS(pdev, id)
);

DEFINE_EVENT(nvidia_gpu_event_class, nvidia_gpu_warning,
    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),
    TP_ARGS(pdev, id)
);

DEFINE_EVENT(nvidia_gpu_event_class, nvidia_gpu_notice,
    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),
    TP_ARGS(pdev, id)
);

DEFINE_EVENT(nvidia_gpu_event_class, nvidia_gpu_diagnostic,
    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),
    TP_ARGS(pdev, id)
);

DEFINE_EVENT(nvidia_gpu_event_class, nvidia_gpu_telemetry,
    TP_PROTO(const struct pci_dev *pdev,
             const nv_gpu_op_event_t *id),
    TP_ARGS(pdev, id)
);

#endif // !defined(_TRACE_NV_GPU_OP_EVENT_H) || defined(TRACE_HEADER_MULTI_READ)

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE nv-report-gpu-op-event
#include <trace/define_trace.h>
