/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSP_INIT_ARGS_H
#define GSP_INIT_ARGS_H

#include "nvtypes.h"

typedef struct {
    NvU64 sharedMemPhysAddr;
    NvU32 pageTableEntryCount;
    NvLength cmdQueueOffset;
    NvLength statQueueOffset;
} MESSAGE_QUEUE_INIT_ARGUMENTS;

typedef struct {
    NvU32 oldLevel;
    NvU32 flags;
    NvBool bInPMTransition;
} GSP_SR_INIT_ARGUMENTS;

/*!
 * (Cached) GSP fw RM initialization arguments.
 */
typedef struct
{
    MESSAGE_QUEUE_INIT_ARGUMENTS      messageQueueInitArguments;
    GSP_SR_INIT_ARGUMENTS             srInitArguments;
    NvU32                             gpuInstance;
    NvBool                            bDmemStack;

    struct
    {
        NvU64                         pa;
        NvU64                         size;
    } profilerArgs;

    struct
    {
        NvU64                         pa;
        NvU64                         size;
    } sysmemHeapArgs;

} GSP_ARGUMENTS_CACHED;

#endif // GSP_INIT_ARGS_H
