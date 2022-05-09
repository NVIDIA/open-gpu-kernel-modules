/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* DisplayPort management routines */

#include <stdarg.h>

#include "nvkms-utils.h"

#include "dp_hostimp.h"

void *dpMalloc(NvLength sz)
{
    return nvAlloc(sz);
}

void dpFree(void *p)
{
    nvFree(p);
}

void dpPrint(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    nvVEvoLog(EVO_LOG_INFO, NV_INVALID_GPU_LOG_INDEX, format, ap);
    va_end(ap);
}

void dpDebugBreakpoint(void)
{
    nvAssert(!"DisplayPort library debug breakpoint");
}

#if NV_DP_ASSERT_ENABLED
void dpAssert(const char *expression, const char *file,
              const char *function, int line)
{
    nvDebugAssert(expression, file, function, line);
}
#endif

void dpTraceEvent(NV_DP_TRACING_EVENT event,
                  NV_DP_TRACING_PRIORITY priority, NvU32 numArgs, ...)
{
    // To support DPlib tracing, implement this function.
}

