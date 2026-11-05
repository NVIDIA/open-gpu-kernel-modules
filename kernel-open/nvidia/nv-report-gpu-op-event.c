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

#include "nv-linux.h"

#define CREATE_TRACE_POINTS
#include "nv-report-gpu-op-event.h"


NV_STATUS NV_API_CALL nv_emit_tracepoint_gpu_op_event(nv_state_t *nv,
                                                      const nv_gpu_op_event_t *op_event)
{
    struct pci_dev   *pci_dev = NULL;
    nv_linux_state_t *nvl;

    if (nv != NULL)
    {
        nvl = NV_GET_NVL_FROM_NV_STATE(nv);
        if (nvl != NULL)
            pci_dev = nvl->pci_dev;
    }

    switch (op_event->log_level)
    {
        case GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR:
            trace_nvidia_gpu_error(pci_dev, op_event);
            break;
        case GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING:
            trace_nvidia_gpu_warning(pci_dev, op_event);
            break;
        case GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE:
            trace_nvidia_gpu_notice(pci_dev, op_event);
            break;
        case GPU_OPERATIONAL_EVENT_LOG_LEVEL_DIAGNOSTIC:
            trace_nvidia_gpu_diagnostic(pci_dev, op_event);
            break;
        case GPU_OPERATIONAL_EVENT_LOG_LEVEL_TELEMETRY:
            trace_nvidia_gpu_telemetry(pci_dev, op_event);
            break;
        default:
            WARN_ONCE(1, "nv_emit_tracepoint_gpu_op_event: unknown log level %u\n",
                      op_event->log_level);
            return NV_ERR_INVALID_ARGUMENT;
    }
    return NV_OK;
}
