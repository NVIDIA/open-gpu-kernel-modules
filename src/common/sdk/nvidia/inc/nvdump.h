/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

////////////////////////////////////////////////////////////////////////////////
//
//  Module: nvDump.h
//      Shared definitions for HW/SW dumping facility residing in resman/rc.
//
// **************************************************************************

#ifndef _NVDUMP_H_
#define _NVDUMP_H_

#include "nvtypes.h"

//-------------------------------------------------------------------------
// DEFINTIONS
//-------------------------------------------------------------------------

#define NVDUMP_SUB_ALLOC_NOT_ENCODED      0x01
#define NVDUMP_SUB_ALLOC_VALID            0x02
#define NVDUMP_SUB_ALLOC_HAS_MORE         0x04

#define NVDUMP_DEBUG_BUFFER_MAX_SIZE            4096   // max of 4K per buffer
#define NVDUMP_DEBUG_BUFFER_MAX_SUBALLOCATIONS  256

#define NVDUMP_CONFIG_SIGNATURE "NVDUMPCONFIGSIG" // multiple of 8 bytes

typedef enum
{
    // The following components are GPU instance specific:
    NVDUMP_COMPONENT_DEBUG_BUFFERS = 0,
    NVDUMP_COMPONENT_ENG_MC,
    NVDUMP_COMPONENT_ENG_FIFO,
    NVDUMP_COMPONENT_ENG_GRAPHICS,
    NVDUMP_COMPONENT_ENG_FB,
    NVDUMP_COMPONENT_ENG_DISP,
    NVDUMP_COMPONENT_ENG_FAN,
    NVDUMP_COMPONENT_ENG_THERMAL,
    NVDUMP_COMPONENT_ENG_FUSE,
    NVDUMP_COMPONENT_ENG_VBIOS,
    NVDUMP_COMPONENT_ENG_PERF,
    NVDUMP_COMPONENT_ENG_BUS,
    NVDUMP_COMPONENT_ENG_PMU,
    NVDUMP_COMPONENT_ENG_ALL,
    NVDUMP_COMPONENT_ENG_CE,
    NVDUMP_COMPONENT_ENG_GPU,
    NVDUMP_COMPONENT_ENG_LPWR,
    NVDUMP_COMPONENT_ENG_NVD,
    NVDUMP_COMPONENT_ENG_VOLT,
    NVDUMP_COMPONENT_ENG_CLK,
    NVDUMP_COMPONENT_ENG_SEC2,
    NVDUMP_COMPONENT_ENG_NVLINK,
    NVDUMP_COMPONENT_ENG_BSP,
    NVDUMP_COMPONENT_ENG_DPU,
    NVDUMP_COMPONENT_ENG_FBFLCN,
    NVDUMP_COMPONENT_ENG_HDA,
    NVDUMP_COMPONENT_ENG_MSENC,
    NVDUMP_COMPONENT_ENG_GSP,
    NVDUMP_COMPONENT_ENG_INFOROM,
    NVDUMP_COMPONENT_ENG_GCX,
    NVDUMP_COMPONENT_ENG_KGSP,
    // The following components are global to the system:
    NVDUMP_COMPONENT_SYS_RCDB = 0x400,
    NVDUMP_COMPONENT_SYS_SYSINFO,
    NVDUMP_COMPONENT_SYS_ALL,
    // The following components are nvlog related.
    NVDUMP_COMPONENT_NVLOG_RM = 0x800,
    NVDUMP_COMPONENT_NVLOG_ALL,
    // Reserved
    NVDUMP_COMPONENT_NVLOG_RESERVED = 0xB00,
} NVDUMP_COMPONENT;

#define NVDUMP_IS_GPU_COMPONENT(c)   ((c) < NVDUMP_COMPONENT_SYS_RCDB)
#define NVDUMP_IS_SYS_COMPONENT(c)   (((c) >= NVDUMP_COMPONENT_SYS_RCDB) && \
                                      ((c) < NVDUMP_COMPONENT_NVLOG_RM))
#define NVDUMP_IS_NVLOG_COMPONENT(c) (((c) >= NVDUMP_COMPONENT_NVLOG_RM) && \
                                      ((c) < NVDUMP_COMPONENT_NVLOG_RESERVED))

typedef enum
{
    NVDUMP_BUFFER_PROVIDED = 0,                // Dump buffer provided by caller
    NVDUMP_BUFFER_ALLOCATE,                    // Dump buffer to be allocated
    NVDUMP_BUFFER_COUNT,                       // Just count, no buffer needed
} NVDUMP_BUFFER_POLICY;

typedef enum
{
    NVDUMP_STATUS_IDLE,
    NVDUMP_STATUS_ERROR,
    NVDUMP_STATUS_COUNT_REQUESTED,
    NVDUMP_STATUS_COUNT_COMPLETE,
    NVDUMP_STATUS_DUMP_REQUESTED,
    NVDUMP_STATUS_DUMP_BUFFER_FULL,
    NVDUMP_STATUS_DUMP_END_OF_MSG,
    NVDUMP_STATUS_DUMP_COMPLETE,
} NVDUMP_STATUS;

//
// The following structures require that all elements are precisely sized
// and aligned on natural boundaries.
//

typedef struct
{
    NvP64 address NV_ALIGN_BYTES(8);
    NvU32 size;
    NvU32 curNumBytes;
} NVDUMP_BUFFER;

// Max number of bytes that can be returned in a dump buffer
#define NVDUMP_MAX_DUMP_SIZE (1 << 20) // 1 MB

typedef struct
{
    NvU8 sigHead[sizeof(NVDUMP_CONFIG_SIGNATURE)] NV_ALIGN_BYTES(8);

    NvP64 debuggerControlFuncAddr NV_ALIGN_BYTES(8);
    NVDUMP_BUFFER buffer;
    NvU32 gpuSelect;
    NvU32 component;
    NvU32 dumpStatus;
    NvU32 rmStatus;

    NvU8 sigTail[sizeof(NVDUMP_CONFIG_SIGNATURE)] NV_ALIGN_BYTES(8);
} NVDUMP_CONFIG;

typedef struct
{
     NvU16     length;
     NvU16     start;
     NvU16     end;
     NvU16     flags;
     NvU16     tag;
} NVDUMP_SUB_ALLOC_HEADER;

//
// Export is needed to allow remote kernel debuggers without symbols
// to find global NVDUMP_CONFIG variables in driver export address table.
//
#define NVDUMP_EXPORT

#endif // _NVDUMP_H_

