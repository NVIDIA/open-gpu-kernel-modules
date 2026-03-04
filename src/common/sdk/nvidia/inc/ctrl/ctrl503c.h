/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl503c.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl503c/ctrl503cbase.h"

/*
 * NV503C_CTRL_CMD_REGISTER_VA_SPACE
 *
 * This command registers the specified GPU VA space with the given
 * NV50_THIRD_PARTY_P2P object, and returns a token that
 * uniquely identifies the VA space within the object's parent
 * client.
 *
 * Its parameter structure has the following fields:
 *
 *   hVASpace
 *      This field specifies the GPU VA space to be registered
 *      with the third-party P2P object.
 *
 *   vaSpaceToken
 *     Upon successful completion of the regristration attempt,
 *     this field holds the new VA space identifier.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_INVALID_OBJECT_HANDLE
 *   NVOS_STATUS_INVALID_ARGUMENT
 *   NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NV503C_CTRL_CMD_REGISTER_VA_SPACE (0x503c0102) /* finn: Evaluated from "(FINN_NV50_THIRD_PARTY_P2P_P2P_INTERFACE_ID << 8) | NV503C_CTRL_REGISTER_VA_SPACE_PARAMS_MESSAGE_ID" */

#define NV503C_CTRL_REGISTER_VA_SPACE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV503C_CTRL_REGISTER_VA_SPACE_PARAMS {
    NvHandle hVASpace;
    NV_DECLARE_ALIGNED(NvU64 vaSpaceToken, 8);
} NV503C_CTRL_REGISTER_VA_SPACE_PARAMS;


/*
 * NV503C_CTRL_CMD_UNREGISTER_VA_SPACE
 *
 * This command unregisters (a previously registered) GPU VA space.
 *
 * Its parameter structure has the following field:
 *
 *   hVASpace
 *      This field specifies the GPU VA space to be
 *      unregistered.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV503C_CTRL_CMD_UNREGISTER_VA_SPACE (0x503c0103) /* finn: Evaluated from "(FINN_NV50_THIRD_PARTY_P2P_P2P_INTERFACE_ID << 8) | NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS_MESSAGE_ID" */

#define NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS {
    NvHandle hVASpace;
} NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS;


/*
 * NV503C_CTRL_CMD_REGISTER_VIDMEM
 *
 * This command registers a video memory allocation with the given
 * NV50_THIRD_PARTY_P2P object.  Registration of video memory
 * allocations is required if they are to be made accessible via the
 * third-party P2P infrastructure.
 *
 * The vidmem allocation is made available to the users of the third-party P2P
 * APIs. It's exposed at the range specified by address and size starting at the
 * specified offset within the physical allocation. The same physical memory is
 * exposed as the NvRmMapMemoryDma() API would make accessible to the GPU if
 * used with equivalent parameters. Notably this API doesn't create any virtual
 * mappings nor verifies that any mappings are present, it only registers the
 * memory for the purpose of the third-party P2P infrastructure.
 *
 * The address range specified by address and size cannot overlap any previously
 * registered ranges for the given NV50_THIRD_PARTY_P2P object.
 *
 * Its parameter structure has the following field:
 *
 *   hMemory
 *      This field specifies the video memory allocation to be
 *      registered with the third-party P2P object.
 *
 *   address
 *      The address to register the video memory allocation at. Has to be
 *      aligned to 64K.
 *
 *   size
 *      Size in bytes, has to be non-0 and aligned to 64K. Offset + size cannot
 *      be larger than the vidmem allocation.
 *
 *   offset
 *      Offset within the video memory allocation where the registered address
 *      range starts. Has to be aligned to 64K.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NV503C_CTRL_CMD_REGISTER_VIDMEM (0x503c0104) /* finn: Evaluated from "(FINN_NV50_THIRD_PARTY_P2P_P2P_INTERFACE_ID << 8) | NV503C_CTRL_REGISTER_VIDMEM_PARAMS_MESSAGE_ID" */

#define NV503C_CTRL_REGISTER_VIDMEM_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV503C_CTRL_REGISTER_VIDMEM_PARAMS {
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 address, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
} NV503C_CTRL_REGISTER_VIDMEM_PARAMS;


/*
 * NV503C_CTRL_CMD_UNREGISTER_VIDMEM
 *
 * This command unregisters (a previously registered) video memory
 * allocation.
 *
 * Its parameter structure has the following field:
 *
 *   hMemory
 *      This field specifies the video memory allocation to be
 *      unregistered.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV503C_CTRL_CMD_UNREGISTER_VIDMEM (0x503c0105) /* finn: Evaluated from "(FINN_NV50_THIRD_PARTY_P2P_P2P_INTERFACE_ID << 8) | NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS_MESSAGE_ID" */

#define NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS {
    NvHandle hMemory;
} NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS;

/*
 * NV503C_CTRL_CMD_REGISTER_PID
 * 
 * This command registers the PID of the process that allocated
 * the RM client identified by the hClient argument with the 
 * third-party P2P object, granting this process access to any
 * underlying video memory.
 *
 * Its parameter structure has the following field:
 *
 *   hClient
 *      This field specifies the client id and should be the handle
 *      to a valid NV01_ROOT_USER instance.
 *
 * Possible status values returned are:
 *    NVOS_STATUS_SUCCES
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV503C_CTRL_CMD_REGISTER_PID (0x503c0106) /* finn: Evaluated from "(FINN_NV50_THIRD_PARTY_P2P_P2P_INTERFACE_ID << 8) | NV503C_CTRL_REGISTER_PID_PARAMS_MESSAGE_ID" */

#define NV503C_CTRL_REGISTER_PID_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV503C_CTRL_REGISTER_PID_PARAMS {
    NvHandle hClient;
} NV503C_CTRL_REGISTER_PID_PARAMS;

/* _ctrl503c_h_ */
