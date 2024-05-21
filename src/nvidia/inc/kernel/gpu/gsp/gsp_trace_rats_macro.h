/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * This file defines macros to place tracepoints for RATS (RM All-around Trace
 * System). The names of the functions and variables associated with this are
 * temporary as we begin to unify all RM tracing tools under one system.
 */

#ifndef GSP_TRACE_RATS_MACRO_H
#define GSP_TRACE_RATS_MACRO_H

#include "core/core.h"

#define GSP_TRACING_RATS_ENABLED 0
#define GSP_TRACE_RATS_ADD_RECORD(recordIdentifier, pGpu, info) (void) 0

#include "kernel/gpu/gsp/kernel_gsp_trace_rats.h"
#include "class/cl90cdtrace.h"

#define KERNEL_GSP_TRACING_RATS_ENABLED 1

#ifndef GET_RATS_TIMESTAMP_NS
#define GET_RATS_TIMESTAMP_NS()         NV_ASSERT(0)
#endif

#endif
