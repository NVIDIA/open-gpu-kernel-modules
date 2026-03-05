/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Top-level header-file that defines the GSP data structures used to request
 * firmware load/execution by the CPU.
 */

#ifndef _RMGSPSEQ_H_
#define _RMGSPSEQ_H_

#include "rmflcnbl.h"

/*!
 * Structure for loading/executing generic bootloader
 */
typedef struct GspLoadExecGenericBootloaderParams
{
    RM_FLCN_BL_DMEM_DESC dmemDesc;
    NvU32 dmemDescSize;
    NvU32 addrSpace;
    NvU32 cpuCacheAttrib;
    NvU32 reserved[4];
} GspLoadExecGenericBootloaderParams;

/*!
 * Structure for loading/executing HS binary
 */
typedef struct GspLoadExecHsBinaryParams
{
    NvU64 imemPhysAddr;
    NvU64 dmemPhysAddr;
    NvU64 reserved64[2];
    NvU32 ucodeImemVA;
    NvU32 ucodeImemPA;
    NvU32 ucodeImemSize;
    NvU32 ucodeDmemVA;
    NvU32 ucodeDmemPA;
    NvU32 ucodeDmemSize;
    NvU32 hsSigDmemAddr;
    NvU32 engineIdMask;
    NvU32 ucodeId;
    NvU32 reserved32[3];
} GspLoadExecHsBinaryParams;

#endif // _RMGSPSEQ_H_
