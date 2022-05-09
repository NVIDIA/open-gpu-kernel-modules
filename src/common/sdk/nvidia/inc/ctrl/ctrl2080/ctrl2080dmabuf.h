/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file: ctrl/ctrl2080/ctrl2080dmabuf.finn
//



/*
 * NV2080_CTRL_CMD_DMABUF_EXPORT_OBJECTS_TO_FD
 *
 * Exports RM vidmem handles to a dma-buf fd.
 *
 * The objects in the 'handles' array are exported to the fd as range:
 * [index, index + numObjects).
 *
 * A dma-buf fd is created the first time this control call is called.
 * The fd is an input parameter for subsequent calls to attach additional handles
 * over NV2080_CTRL_DMABUF_MAX_HANDLES.
 *
 * fd
 *   A dma-buf file descriptor. If -1, a new FD will be created.
 *
 * totalObjects
 *   The total number of objects that the client wishes to export to the FD.
 *   This parameter will be honored only when the FD is getting created.
 *
 * numObjects
 *   The number of handles the user wishes to export in this call.
 *
 * index
 *   The index into the export fd at which to start exporting the handles in
 *   'handles'. This index cannot overlap a previously used index.
 *
 * totalSize
 *   The total size of memory being exported in bytes, needed to create the dma-buf.
 *   This size includes the memory that will be exported in future export calls
 *   for this dma-buf.
 *
 * handles
 *   An array of {handle, offset, size} that describes the dma-buf.
 *   The offsets and sizes must be OS page-size aligned.
 *
 * Limitations:
 *   1. This call only supports vidmem objects for now.
 *   2. All memory handles should belong to the same GPU or the same GPU MIG instance.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 *    NV_ERR_NO_MEMORY
 *    NV_ERR_OPERATING_SYSTEM
 *    NV_ERR_IN_USE
 *    NV_ERR_INVALID_OBJECT
 *    NV_ERR_INVALID_OBJECT_PARENT
 */
#define NV2080_CTRL_CMD_DMABUF_EXPORT_OBJECTS_TO_FD (0x20803a01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_DMABUF_INTERFACE_ID << 8) | NV2080_CTRL_DMABUF_EXPORT_MEM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_DMABUF_MAX_HANDLES              128

typedef struct NV2080_CTRL_DMABUF_MEM_HANDLE_INFO {
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
} NV2080_CTRL_DMABUF_MEM_HANDLE_INFO;

#define NV2080_CTRL_DMABUF_EXPORT_MEM_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_DMABUF_EXPORT_MEM_PARAMS {
    NvS32 fd;
    NvU32 totalObjects;
    NvU32 numObjects;
    NvU32 index;
    NV_DECLARE_ALIGNED(NvU64 totalSize, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_DMABUF_MEM_HANDLE_INFO handles[NV2080_CTRL_DMABUF_MAX_HANDLES], 8);
} NV2080_CTRL_DMABUF_EXPORT_MEM_PARAMS;

// _ctrl2080dmabuf_h_
