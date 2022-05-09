/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief DEBUG module implementation for Unix kernelspace.
 */
#ifndef _NVPORT_DEBUG_UNIX_KERNEL_OS_H_
#define _NVPORT_DEBUG_UNIX_KERNEL_OS_H_
#ifdef __cplusplus
extern "C" {
#endif

#if !PORT_IS_KERNEL_BUILD
#error "This file can only be compiled as part of the kernel build."
#endif
#if !NVOS_IS_UNIX
#error "This file can only be compiled on Unix."
#endif

#include "nv-kernel-interface-api.h"
void NV_API_CALL os_dbg_breakpoint(void);
void NV_API_CALL out_string(const char *str);
int  NV_API_CALL nv_printf(NvU32 debuglevel, const char *format, ...);

// No init/shutdown needed
#define portDbgInitialize()
#define portDbgShutdown()


PORT_DEBUG_INLINE void
portDbgPrintString
(
    const char *str,
    NvLength length
)
{
    out_string(str);
}

#define portDbgPrintf(fmt, ...) nv_printf(0xFFFFFFFF, fmt, ##__VA_ARGS__)
#undef portDbgPrintf_SUPPORTED
#define portDbgPrintf_SUPPORTED 1

#define portDbgExPrintfLevel(level, fmt, ...) nv_printf(level, fmt, ##__VA_ARGS__)
#undef portDbgExPrintfLevel_SUPPORTED
#define portDbgExPrintfLevel_SUPPORTED 1

#define PORT_BREAKPOINT() os_dbg_breakpoint()

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // _NVPORT_DEBUG_UNIX_KERNEL_OS_H_
