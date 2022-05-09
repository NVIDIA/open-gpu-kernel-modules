/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief THREAD module implementation for Unix kernelmode
 *
 * This implementation uses the NVIDIA OS interface into the unix kernels.
 */

#if !PORT_IS_KERNEL_BUILD
#error "This file can only be compiled as part of the kernel build."
#endif

#if !NVOS_IS_UNIX
#error "This file can only be compiled on Unix."
#endif

#include "nvport/nvport.h"
#include "os-interface.h"

// Invalid value for thread.
const PORT_THREAD PORT_THREAD_INVALID = {0ULL};

// Invalid value for process.
const PORT_PROCESS PORT_PROCESS_INVALID = {0ULL};

NvU64 portThreadGetCurrentThreadId()
{
    NvU64 tid = 0;
    os_get_current_thread(&tid);
    return tid;
}

void portThreadYield()
{
    os_schedule();
}

