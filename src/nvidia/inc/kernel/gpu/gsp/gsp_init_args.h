/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvctassert.h"
#include "gpu/mem_mgr/rm_page_size.h"

#define WPR_ALIGNMENT RM_PAGE_SIZE_128K

typedef struct NV_ABI_STABLE MESSAGE_QUEUE_INIT_ARGUMENTS
{
#define MESSAGE_QUEUE_INIT_FLAG_ENCRYPTION          0x01
    NvU64 flags;

    NvU64 sharedMemPhysAddr;
    NvU64 cmdQueueOffset;
    NvU64 statQueueOffset;
    NvU32 queueElementHdrSize;
    NvU32 queueElementSizeMin;
    NvU32 queueElementSizeMax;
    NvU32 queueHeaderAlign;
    NvU32 queueElementAlign;
    NvU32 pageTableEntryCount;
    NvU8  reserved[8];
} MESSAGE_QUEUE_INIT_ARGUMENTS;

typedef struct NV_ABI_STABLE GSP_SR_INIT_ARGUMENTS
{
    NvU32 oldLevel;
    NvU32 flags;
    NvU8  reserved[8];
} GSP_SR_INIT_ARGUMENTS;

/*!
 * (Cached) GSP fw RM initialization arguments.
 */
typedef struct NV_ABI_STABLE GSP_ARGUMENTS_CACHED
{
#define GSP_ARGUMENTS_MAGIC_VALUE (' ' << 24 | 'P' << 16 | 'S' << 8 | 'G')
    NvU32                             magic;
    NvU16                             size;
    NvU8                              reserved[2];

#define GSP_ARGUMENTS_FLAG_IN_PM_TRANSITION          0x01
#define GSP_ARGUMENTS_FLAG_STACK_IN_DMEM             0x02
#define GSP_ARGUMENTS_FLAG_RECOVERY_MARGIN_PRESENT   0x04
#define GSP_ARGUMENTS_FLAG_SCAN_RECOVERY_MARGIN      0x08
    NvU64                             flags;

    MESSAGE_QUEUE_INIT_ARGUMENTS      messageQueueInitArguments;

    GSP_SR_INIT_ARGUMENTS             srInitArguments;

    NvU8                              reserved2[4];
    NvU32                             gpuInstance;

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

    struct
    {
        NvU64                         pa;
        NvU64                         size;
    } rmStateMonitorBufferArgs;

    struct
    {
        NvU64                         radix3;
        NvU64                         size;
    } bindataArgs;

    struct
    {
        NvU64                         pa;
        NvU64                         size;
    } vbiosOverrideArgs;

    //
    // Other data can be added here, so long as the whole structure is <=4kb
    //
} GSP_ARGUMENTS_CACHED;

ct_assert(sizeof(GSP_ARGUMENTS_CACHED) <= 4096);

#endif // GSP_INIT_ARGS_H
