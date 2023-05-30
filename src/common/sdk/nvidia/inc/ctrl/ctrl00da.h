/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl00da.finn
//



/* NV_SEMAPHORE_SURFACE control commands and parameters */

#define NV_SEMAPHORE_SURFACE_CTRL_CMD(cat,idx)       NVXXXX_CTRL_CMD(0x00DA, NV00DA_CTRL_##cat, idx)

/*
* NV_SEMAPHORE_SURFACE_CTRL_CMD_REF_MEMORY
*   Duplicate the memory object(s) bound to the semaphore surface into the RM
*   client of the caller.
*
*   The handle values are generated and returned by resource manager if the
*   client specified a handle value of zero.
*
*   If the semaphore surface has a valid max submitted value memory object, but
*   the GPU + system do not require separate max submitted value and semaphore
*   value surfaces, the handles provided by the client must be equal, and the
*   handles returned by resource manager will also be equal. In such cases, the
*   client must recognize that both handles correspond to a single reference to
*   a single object, and hence the handle must be freed only once by the client.
*
*   If the GPU does not require a max submitted value memory object, the handle
*   value must be set to zero and the returned handle will always be zero as
*   well.
*
* RETURNS:
*   NVOS_STATUS_SUCCESS if the memory object(s) were successfully duplicated
*       into the calling client.
*   NVOS_STATUS_ERROR_INVALID_PARAMETER if any of the rules regarding the
*       max submitted value handle value were violated.
*   An error code forwarded from NvRmDupObject for any other failures.
*/
#define NV_SEMAPHORE_SURFACE_CTRL_CMD_REF_MEMORY (0xda0001) /* finn: Evaluated from "(FINN_NV_SEMAPHORE_SURFACE_INTERFACE_ID << 8) | NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS_MESSAGE_ID" */

#define NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS_MESSAGE_ID (0x01U)

typedef struct NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS {
    NvHandle hSemaphoreMem;
    NvHandle hMaxSubmittedMem;
} NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS;

/*
* NV_SEMAPHORE_SURFACE_CTRL_CMD_BIND_CHANNEL
*   Associates a channel with the semaphore surface.  All channels which will
*   wait on or signal semaphores in a semaphore surface should first register
*   with it to ensure proper event delivery and error handling.
*
*   engineMask is a bitfield whose contents should be defined by setting bit
*   index <n> to '1' if the corresponding engine index will be used.  See
*   cl2080.h for a list of engine indices.  For example, this would indicate
*   a channel making use of the engines GR0(graphics/compute), COPY0-COPY9,
*   and host:
*
*     NvU64 engineMask = (1ULL << NV2080_ENGINE_TYPE_GR0) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY0) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY1) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY2) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY3) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY4) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY5) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY6) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY7) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY8) |
*                        (1ULL << NV2080_ENGINE_TYPE_COPY9) |
*                        (1ULL << NV2080_ENGINE_TYPE_HOST);
*/
#define NV_SEMAPHORE_SURFACE_CTRL_CMD_BIND_CHANNEL (0xda0002) /* finn: Evaluated from "(FINN_NV_SEMAPHORE_SURFACE_INTERFACE_ID << 8) | NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS_MESSAGE_ID (0x02U)

typedef struct NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS {
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hChannel;
    NV_DECLARE_ALIGNED(NvU64 engineMask, 8);
} NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS;

/*
* NV_SEMAPHORE_SURFACE_CTRL_CMD_REGISTER_WAITER
*   Ask RM to signal the specified OS event and/or set the semaphore to a new
*   value when the value at the specified index is >= a desired value.
*
*   index - Specifies the semaphore slot within the surface to which the the wait
*       applies.
*   waitValue - The value to wait for.
*   newValue - Specifies a value to set the semaphore to automatically when the
*       specified semaphore slot reaches waitValue. "0" means the semaphore
*       value is not altered by this waiter.
*   notificationHandle - The OS event (kernel callback or userspace event
*       handle) to notify when the value is reached, or 0 if no notification
*       is required.
*
*   The waiter must specify at least one action.
*
* RETURNS:
*   NVOS_STATUS_SUCCESS if the waitValue has not been reached and a waiter was
*       successfully registered
*   NVOS_STATUS_SUCCESS if the waitValue has been reached, newValue was applied,
*       and notificationHandle was 0 (No notification was requested).
*   NVOS_STATUS_ERROR_ALREADY_SIGNALLED if the waitValue has been reached and
*       newValue was applied if it was not 0, but no notification was registered
*       or generated on notificationHandle. Other notifications generated as a
*       side effect of newValue being applied, if any, were generated.
*   NVOS_STATUS_ERROR_STATE_IN_USE if newValue is not 0 and the specified index
*       already has an auto-update value registered. No waiter is registered.
*   NVOS_STATUS_ERROR_STATE_IN_USE if the specified notification handle is
*       already registered as a waiter for the specified wait_value at the
*       specified index.
*       already has an auto-update value registered. No waiter is registered.
*   NVOS_STATUS_ERROR_* miscelaneous internal errors. No waiter is registered.
*/
#define NV_SEMAPHORE_SURFACE_CTRL_CMD_REGISTER_WAITER (0xda0003) /* finn: Evaluated from "(FINN_NV_SEMAPHORE_SURFACE_INTERFACE_ID << 8) | NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS_MESSAGE_ID" */

#define NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS_MESSAGE_ID (0x03U)

typedef struct NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 index, 8);
    NV_DECLARE_ALIGNED(NvU64 waitValue, 8);
    NV_DECLARE_ALIGNED(NvU64 newValue, 8);
    NV_DECLARE_ALIGNED(NvU64 notificationHandle, 8);
} NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS;

/*
* NV_SEMAPHORE_SURFACE_CTRL_CMD_SET_VALUE
*   Modify a semaphore surface semaphore value, awakening any CPU waiters in
*   the process. newValue must be >= the current value at the specified index.
*/
#define NV_SEMAPHORE_SURFACE_CTRL_CMD_SET_VALUE (0xda0004) /* finn: Evaluated from "(FINN_NV_SEMAPHORE_SURFACE_INTERFACE_ID << 8) | NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS_MESSAGE_ID" */

#define NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS_MESSAGE_ID (0x04U)

typedef struct NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 index, 8);
    NV_DECLARE_ALIGNED(NvU64 newValue, 8);
} NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS;

/*
* NV_SEMAPHORE_SURFACE_CTRL_CMD_UNREGISTER_WAITER
*   Remove a previously registered notification handle from an index + value
*   tuple's list of waiters.
*
*   index - Specifies the semaphore slot within the surface on which the waiter
*       was previously registered.
*   waitValue - The value the wait was registered for.
*   notificationHandle - The OS event (kernel callback or userspace event
*       handle) registered as a waiter.
*
* RETURNS:
*   NVOS_STATUS_SUCCESS the waiter was successfully removed from the list of
*       pending waiters.
*   NVOS_STATUS_ERROR_* miscelaneous internal errors, or the waiter was not
*       found in the list of pending waiters. The waiter may have already been
*       called, or may be in a list of imminent notifications the RM is
*       processing.
*/
#define NV_SEMAPHORE_SURFACE_CTRL_CMD_UNREGISTER_WAITER (0xda0005) /* finn: Evaluated from "(FINN_NV_SEMAPHORE_SURFACE_INTERFACE_ID << 8) | NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS_MESSAGE_ID" */

#define NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS_MESSAGE_ID (0x05U)

typedef struct NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 index, 8);
    NV_DECLARE_ALIGNED(NvU64 waitValue, 8);
    NV_DECLARE_ALIGNED(NvU64 notificationHandle, 8);
} NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS;

/* _ctrl00da_h_ */

