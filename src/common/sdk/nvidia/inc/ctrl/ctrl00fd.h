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
// Source file:      ctrl/ctrl00fd.finn
//

#include "ctrl/ctrlxxxx.h"

#define NV00FD_CTRL_CMD(cat,idx)       NVXXXX_CTRL_CMD(0x00fd, NV00FD_CTRL_##cat, idx)

/* NV00FD command categories (6bits) */
#define NV00FD_CTRL_RESERVED         (0x00)
#define NV00FD_CTRL_MULTICAST_FABRIC (0x01)

/*
 * NV00FD_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00FD_CTRL_CMD_NULL         (0xfd0000) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV00FD_CTRL_CMD_GET_INFO
 *
 * Queries multicast memory fabric allocation attributes.
 *
 *  alignment [OUT]
 *    Alignment for the allocation.
 *
 *  allocSize [OUT]
 *    Size of the allocation.
 *
 *  pageSize [OUT]
 *    Page size of the allocation.
 *
 *  numMaxGpus [OUT]
 *    Maximum number of attachable GPUs
 *
 *  numAttachedGpus [OUT]
 *    Number of GPUs currently attached
 *
 */
#define NV00FD_CTRL_CMD_GET_INFO (0xfd0101) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00FD_CTRL_GET_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 alignment, 8);
    NV_DECLARE_ALIGNED(NvU64 allocSize, 8);
    NvU32 pageSize;
    NvU32 numMaxGpus;
    NvU32 numAttachedGpus;
} NV00FD_CTRL_GET_INFO_PARAMS;

/*
 * NV00FD_CTRL_CMD_ATTACH_MEM
 *
 * Attaches the physical memory handle and in turn the memory
 * owner of the physical memory to the Multicast FLA object.
 *
 *  hSubdevice [IN]
 *    Subdevice handle of the owner GPU
 *
 *  hMemory [IN]
 *    Physical memory handle to be attached.
 *
 *  offset [IN]
 *    Offset into the MCFLA object. Must be at least physical pagesize aligned.
 *
 *  mapOffSet [IN]
 *    Offset into the physical memory descriptor.
 *    Must be physical memory pagesize aligned.
 *
 *  mapLength [IN]
 *    Length of physical memory handle to be mapped.
 *    Must be physical memory pagesize aligned and less than or equal to
 *    MCFLA alloc size
 *
 *  flags [IN]
 *    For future use only. Must be zero for now.
 *
 *  Restrictions:
 *  a. Memory belonging to only NVSwitch P2P supported GPUs
 *     which can do multicast can be attached
 *  b. Physical memory with 2MB or 512MB pagesize is allowed
 *  c. Memory of an already attached GPU should not be attached
 *  d. Only vidmem physical memory handle can be attached
 *
 */
#define NV00FD_CTRL_CMD_ATTACH_MEM (0xfd0102) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_ATTACH_MEM_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_ATTACH_MEM_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00FD_CTRL_ATTACH_MEM_PARAMS {
    NvHandle hSubdevice;
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 mapOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 mapLength, 8);
    NvU32    flags;
} NV00FD_CTRL_ATTACH_MEM_PARAMS;

/*
 * NV00FD_CTRL_CMD_REGISTER_EVENT
 *
 * Allows clients to optionally register for events after the Multicast
 * FLA object is duped under another client.
 *
 *  pOsEvent [IN]
 *    OS event handle created with NvRmAllocOsEvent().
 *
 */
#define NV00FD_CTRL_CMD_REGISTER_EVENT (0xfd0103) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_REGISTER_EVENT_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_REGISTER_EVENT_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV00FD_CTRL_REGISTER_EVENT_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pOsEvent, 8);
} NV00FD_CTRL_REGISTER_EVENT_PARAMS;

/*
 * NV00FD_CTRL_CMD_ATTACH_GPU
 *
 * Attaches GPU to the Multicast FLA object. This step must be done before
 * attaching memory to the Multicast FLA object.
 *
 *  hSubdevice [IN]
 *    Subdevice handle of the owner GPU
 *
 *  flags [IN]
 *    For future use only. Must be zero for now.
 *
 *  devDescriptor [IN]
 *    devDescriptor is a file descriptor for unix RM clients, but a void
 *    pointer for windows RM clients. It is transparent to RM clients i.e. RM's
 *    user-mode shim populates this field on behalf of clients.
 */
#define NV00FD_CTRL_CMD_ATTACH_GPU (0xfd0104) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_ATTACH_GPU_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_ATTACH_GPU_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV00FD_CTRL_ATTACH_GPU_PARAMS {
    NvHandle hSubdevice;
    NvU32    flags;
    NV_DECLARE_ALIGNED(NvU64 devDescriptor, 8);
} NV00FD_CTRL_ATTACH_GPU_PARAMS;

/*
 * NV00FD_CTRL_CMD_DETACH_MEM
 *
 * Detaches the physical memory handle for a given GPU.
 *
 *  hSubdevice [IN]
 *    Subdevice handle of the GPU for which memory to be detached.
 *
 *  offset [IN]
 *    Offset into the MCFLA object at which memory to be detached. Same as
 *    NV00FD_CTRL_CMD_ATTACH_MEM.
 *
 *  flags [IN]
 *    For future use only. Must be zero for now.
 */
#define NV00FD_CTRL_CMD_DETACH_MEM (0xfd0105) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_DETACH_MEM_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_DETACH_MEM_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV00FD_CTRL_DETACH_MEM_PARAMS {
    NvHandle hSubdevice;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NvU32    flags;
} NV00FD_CTRL_DETACH_MEM_PARAMS;

/*
 * NV00FD_CTRL_CMD_ATTACH_REMOTE_GPU
 *
 * Attaches a remote GPU to the Multicast FLA object. This control call is
 * restricted to the privileged clients like IMEX daemon.
 *
 *  gpuFabricProbeHandle [IN]
 *    Fabric probe handle of the remote GPU
 *
 *  key [IN]
 *    Key is used by the GFM in the MCFLA team response as an ID to allow the
 *    RM to correlate it with the MCFLA team request.
 *
 *  cliqueId [IN]
 *    Clique ID of the remote GPU being attached.
 *
 *  nodeId [IN]
 *    nodeID from which the remote GPU is being attached.
 */
#define NV00FD_CTRL_CMD_ATTACH_REMOTE_GPU (0xfd0106) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 gpuFabricProbeHandle, 8);
    NV_DECLARE_ALIGNED(NvU64 key, 8);
    NvU32 cliqueId;
    NvU16 nodeId;
} NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS;

/*
 * NV00FD_CTRL_CMD_SET_FAILURE
 *
 * Marks the MCLFA object allocation with a failure. This control call is
 * restricted to the privileged client like IMEX daemon.
 *
 *  status [IN]
 *    Failure status, shouldn't be NV_OK.
 */
#define NV00FD_CTRL_CMD_SET_FAILURE (0xfd0107) /* finn: Evaluated from "(FINN_NV_MEMORY_MULTICAST_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00FD_CTRL_SET_FAILURE_PARAMS_MESSAGE_ID" */

#define NV00FD_CTRL_SET_FAILURE_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV00FD_CTRL_SET_FAILURE_PARAMS {
    NV_STATUS status;
} NV00FD_CTRL_SET_FAILURE_PARAMS;



/* _ctrl00fd_h_ */
