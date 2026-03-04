/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#undef TRACE_SYSTEM
#define TRACE_SYSTEM nvswitch

#if !defined(_TRACE_NVSWITCH_EVENT_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_NVSWITCH_EVENT_H

#include <linux/tracepoint.h>

TRACE_EVENT(nvswitch_dev_sxid,

    TP_PROTO(const struct pci_dev *pdev, uint32_t error_code, const char *msg),

    TP_ARGS(pdev, error_code, msg),

    TP_STRUCT__entry(
        __string(dev, pci_name(pdev))
        __field (u32, error_code)
        __string(msg, msg)
    ),

    TP_fast_assign(
#if NV_ASSIGN_STR_ARGUMENT_COUNT == 1
        __assign_str(dev);
        __assign_str(msg);
#else
        __assign_str(dev, pci_name(pdev));
        __assign_str(msg, msg);
#endif
        __entry->error_code = error_code;
    ),

    TP_printk("SXid (PCI:%s): %u, %s", __get_str(dev), __entry->error_code, __get_str(msg))
);

#endif

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE nvswitch_event
#include <trace/define_trace.h>
