/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_hostimp.h                                                      *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_HOSTIMP_H
#define INCLUDED_DP_HOSTIMP_H

#include "nvtypes.h"
#include "dp_tracing.h"
#include "dp_printf.h"

extern "C" void * dpMalloc(NvLength size);
extern "C" void dpFree(void * ptr);
extern "C" void dpDebugBreakpoint();
// Note: dpPrint() implementations are expected to append a newline themselves.
extern "C" void dpPrint(const char * formatter, ...);
extern "C" void dpPrintf(DP_LOG_LEVEL severity, const char * formatter, ...);
extern "C" void dpTraceEvent(NV_DP_TRACING_EVENT event,
                             NV_DP_TRACING_PRIORITY priority, NvU32 numArgs, ...);

#if defined(_DEBUG) || defined(DEBUG)
  #define NV_DP_ASSERT_ENABLED 1
#else
  #define NV_DP_ASSERT_ENABLED 0
#endif

#if NV_DP_ASSERT_ENABLED
extern "C" void dpAssert(const char *expression, const char *file,
                         const char *function, int line);
#endif

#endif // INCLUDED_DP_HOSTIMP_H
