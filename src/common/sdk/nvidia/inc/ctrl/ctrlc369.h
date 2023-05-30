/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc369.finn
//

#include "ctrl/ctrlxxxx.h"
/* MMU_FAULT_BUFFER control commands and parameters */

#define NVC369_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0xC369, NVC369_CTRL_##cat, idx)

/* MMU_FAULT_BUFFER command categories (6bits) */
#define NVC369_CTRL_RESERVED         (0x00)
#define NVC369_CTRL_MMU_FAULT_BUFFER (0x01)

/*
 * NVC369_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC369_CTRL_CMD_NULL         (0xc3690000) /* finn: Evaluated from "(FINN_MMU_FAULT_BUFFER_RESERVED_INTERFACE_ID << 8) | 0x0" */






/*
 * NVC369_CTRL_CMD_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUFFER
 *
 * This call creates and registers a client buffer for the non replayable faults
 *
 *    pShadowBuffer [OUT]
 *       This parameter represents the pointer to the shadow buffer
 *
 *    pShadowBufferContext [OUT]
 *       Execution context for pShadowBuffer queue
 *
 *    bufferSize [OUT]
 *       Size in bytes of the shadow buffer for non replayable faults
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC369_CTRL_CMD_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF (0xc3690101) /* finn: Evaluated from "(FINN_MMU_FAULT_BUFFER_MMU_FAULT_BUFFER_INTERFACE_ID << 8) | NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS_MESSAGE_ID" */

#define NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pShadowBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pShadowBufferContext, 8);
    NV_DECLARE_ALIGNED(NvP64 pShadowBufferMetadata, 8);
    NvU32 bufferSize;
} NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS;


/*
 * NVC369_CTRL_CMD_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUFFER
 *
 * This call unregisters and destroys a client buffer for the non replayable
 * faults
 * 
 *    pShadowBuffer [IN]
 *       This parameter represents the pointer to the shadow buffer
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVC369_CTRL_CMD_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF (0xc3690102) /* finn: Evaluated from "(FINN_MMU_FAULT_BUFFER_MMU_FAULT_BUFFER_INTERFACE_ID << 8) | NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS_MESSAGE_ID" */

#define NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pShadowBuffer, 8);
} NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS;

/*
 * NVC369_CTRL_CMD_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUFFER
 *
 * This call creates and registers a client buffer for the replayable faults
 *
 *    pShadowBuffer [OUT]
 *       This parameter represents the pointer to the shadow buffer
 *
 *    bufferSize [OUT]
 *       Size in bytes of the shadow buffer for non replayable faults
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC369_CTRL_CMD_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF (0xc3690103) /* finn: Evaluated from "(FINN_MMU_FAULT_BUFFER_MMU_FAULT_BUFFER_INTERFACE_ID << 8) | NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS_MESSAGE_ID" */

#define NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pShadowBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pShadowBufferMetadata, 8);
    NvU32 bufferSize;
} NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS;

/*
 * NVC369_CTRL_CMD_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUFFER
 *
 * This call unregisters and destroys a client buffer for the replayable
 * faults
 * 
 *    pShadowBuffer [IN]
 *       This parameter represents the pointer to the shadow buffer
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVC369_CTRL_CMD_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF (0xc3690104) /* finn: Evaluated from "(FINN_MMU_FAULT_BUFFER_MMU_FAULT_BUFFER_INTERFACE_ID << 8) | NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS_MESSAGE_ID" */

#define NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pShadowBuffer, 8);
} NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS;

/* _ctrlc369_h_ */
