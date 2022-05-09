/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 /******************************* DisplayPort ******************************\
*                                                                           *
* Module: dp_tracing.h                                                      *
*    Header file for support of tracing, implemented by a host provider     *
*    Because this is platform-agnostic, the tracing API                     *
*    is left up to the host interface.                                      *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_TRACING_H
#define INCLUDED_DP_TRACING_H

#include "nvtypes.h"

typedef enum NV_DP_TRACING_EVENT
{
    TRACE_DP_ID_HOTPLUG,
    TRACE_DP_ID_NEW_SINK_DETECTED,
    TRACE_DP_ID_NEW_SINK_REPORTED,
    TRACE_DP_ID_NEW_MST_DEVICE,
    TRACE_DP_ID_LOST_DEVICE,
    TRACE_DP_ID_LINK_ASSESSMENT,
    TRACE_DP_ID_LINK_TRAINING_START,
    TRACE_DP_ID_LINK_TRAINING_DONE,
    TRACE_DP_ID_NOTIFY_ATTACH_BEGIN,
    TRACE_DP_ID_NOTIFY_ATTACH_BEGIN_STATUS,
    TRACE_DP_ID_NOTIFY_ATTACH_END,
    TRACE_DP_ID_NOTIFY_DETACH_BEGIN,
    TRACE_DP_ID_NOTIFY_DETACH_END,
    TRACE_DP_ID_MESSAGE_EXPIRED
} NV_DP_TRACING_EVENT;

typedef enum NV_DP_TRACING_PRIORITY
{
    TRACE_DP_PRIORITY_ERROR,
    TRACE_DP_PRIORITY_WARNING,
    TRACE_DP_PRIORITY_INFO
} NV_DP_TRACING_PRIORITY;

#define NV_DPTRACE_MAX_PARAMS 8

#define _NV_DPTRACE_EXPAND_HELPER(x) x
#define _NV_DPTRACE_EXPAND(x) _NV_DPTRACE_EXPAND_HELPER(x)

//
// _COUNT_ARGS: Counts the size of an argument list.
//
// For example, if the argument list is two-arguments "A, B", then call it like this:
// _COUNT_ARGS(_placeholder, A, B, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
//
// which maps to the argument names like this:
// _COUNT_ARGS(_0=_placeholder, _1=A, _2=B, _3=9, _4=8, _5=7, _6=6, _7=5, _8=4,, _9=3, _10=2, ...)
//
// and thus _COUNT_ARGS will return 2, the correct size of the argument list.
//
#define _NV_DPTRACE_COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10

#define NV_DPTRACE_ERROR(...)   NV_DPTRACE_EVENT(TRACE_DP_PRIORITY_ERROR, __VA_ARGS__)
#define NV_DPTRACE_WARNING(...) NV_DPTRACE_EVENT(TRACE_DP_PRIORITY_WARNING, __VA_ARGS__)
#define NV_DPTRACE_INFO(...)    NV_DPTRACE_EVENT(TRACE_DP_PRIORITY_INFO, __VA_ARGS__)

//
// When ##__VA_ARGS__ is used, it will delete a preceding comma (',') when
// __VA_ARGS__ is blank (i.e. zero-length argument list). This allows
// the zero-argument case to work without resulting in a syntax error.
// 
// We have a placeholder argument as the first parameter to _COUNT_ARGS
// so that we can take advantage of this comma-deleting behavior.
//
// However, there shouldn't be a zero-arg case as of now, because the first arg is the event.
//
#define NV_DPTRACE_EVENT(priority, ...)                                             \
    _NV_DPTRACE_SEND(priority, _NV_DPTRACE_EXPAND(_NV_DPTRACE_COUNT_ARGS(_0, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)), __VA_ARGS__)

#define _NV_DPTRACE_SEND(priority, argc, ...) _NV_DPTRACE_EXPAND(_NV_DPTRACE_SEND_N(priority, argc, __VA_ARGS__))
#define _NV_DPTRACE_SEND_N(priority, argc, ...) _NV_DPTRACE_EXPAND(_NV_DPTRACE_##argc(priority, __VA_ARGS__))  

// The first argument is the event - macro number is one higher than num args passed to dpTraceEvent
#define _NV_DPTRACE_1(priority, event)                                               \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 0);

#define _NV_DPTRACE_2(priority, event, p1)                                           \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 1, p1);

#define _NV_DPTRACE_3(priority, event, p1, p2)                                       \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 2, p1, p2);

#define _NV_DPTRACE_4(priority, event, p1, p2, p3)                                   \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 3, p1, p2, p3);

#define _NV_DPTRACE_5(priority, event, p1, p2, p3, p4)                               \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 4, p1, p2, p3, p4);

#define _NV_DPTRACE_6(priority, event, p1, p2, p3, p4, p5)                           \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 5, p1, p2, p3, p4, p5);

#define _NV_DPTRACE_7(priority, event, p1, p2, p3, p4, p5, p6)                       \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 6, p1, p2, p3, p4, p5, p6);

#define _NV_DPTRACE_8(priority, event, p1, p2, p3, p4, p5, p6, p7)                   \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 7, p1, p2, p3, p4, p5, p6, p7);

#define _NV_DPTRACE_9(priority, event, p1, p2, p3, p4, p5, p6, p7, p8)               \
    dpTraceEvent(TRACE_DP_ID_##event, priority, 8, p1, p2, p3, p4, p5, p6, p7, p8);

#endif // INCLUDED_DP_TRACING_H
