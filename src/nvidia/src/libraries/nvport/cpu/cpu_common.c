/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief CPU module function implementations which are shared across platforms
 *
 */

#include "nvport/nvport.h"
#include "cpu_common.h"

void
portCpuInitialize(void)
{
    PORT_CPU_SET_IMC_BAR_DESC_INIT_STATE(NV_FALSE);
}

void
portCpuShutdown(void)
{
    //
    // Not returning status to the caller since that seems like a norm in nvort
    // for init and shutdown functions
    //
    if (PORT_CPU_GET_IMC_BAR_DESC_INIT_STATE() == NV_TRUE)
    {
        //
        // If PORT_CPU_GET_IMC_BAR_DESC_INIT_STATE is true then 
        // portCpuExFreeImcBarDesc will be supported. Adding following check
        // to avoid compile time issues
        //
        #if PORT_IS_FUNC_SUPPORTED(portCpuExFreeImcBarDesc)
        if (portCpuExFreeImcBarDesc(PORT_CPU_GET_IMC_BAR_DESC()) != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
        }
        #endif
    }
    PORT_CPU_SET_IMC_BAR_DESC_INIT_STATE(NV_FALSE);
}
