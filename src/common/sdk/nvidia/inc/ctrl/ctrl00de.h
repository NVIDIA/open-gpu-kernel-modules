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
// Source file:      ctrl/ctrl00de.finn
//

#include "ctrl/ctrlxxxx.h"

#define NV00DE_CTRL_CMD(cat, idx) NVXXXX_CTRL_CMD(0x00de, NV00DE_CTRL_##cat, idx)

/* RM_USER_SHARED_DATA control commands and parameters */

/*
 * NV00DE_CTRL_CMD_REQUEST_DATA_POLL
 *
 * @brief Request some polled data elements to be updated
 *        Equivalent to requesting polling using NV00DE_ALLOC_PARAMETERS->polledDataMask
 *        See cl00de.h for mask bits
 *
 * @param[in]  polledDataMask Bitmask of data to be updated
 *
 * @return NV_OK on success
 * @return NV_ERR_ otherwise
 */
#define NV00DE_CTRL_CMD_REQUEST_DATA_POLL (0xde0001U) /* finn: Evaluated from "(FINN_RM_USER_SHARED_DATA_INTERFACE_ID << 8) | NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS_MESSAGE_ID" */

#define NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 polledDataMask, 8);
} NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS;

#define NV00DE_RUSD_POLLING_INTERVAL_MIN      100

/*
 * NV00DE_CTRL_CMD_REQUEST_POLL_INTERVAL
 *
 * @brief Privileged control to set RUSD polling interval.
 *        Valid interval is between NV00DE_RUSD_POLLING_INTERVAL_MIN and
 *        the default value (decided by SKU and can be overriden by regkey).
 *        A 0 polling interval is a special value that resets the interval for
 *        this RUSD instance to the default value.
 *        Polling interval less than NV00DE_RUSD_POLLING_INTERVAL_MIN is
 *        invalid. Polling interval greater than the default value is considered
 *        a reset to default value as well. The reason is that a polling interval
 *        request from client is considered satisfied as long as RUSD polls more
 *        frequently than the request. The clients do not need to know the
 *        default or actual frequency that RUSD is running. Hence, we don't
 *        return an error when client requests a polling frequency higher than
 *        the default value.
 *
 *        RUSD will poll at the minimum of all requested polling intervals that
 *        are valid.
 *
 *        The polling interval can change when:
 *        1. A client requests a polling interval
 *        2. A client is freed
 *
 * @param[in] pollingIntervalMs
 *
 * @return NV_OK on success
 *         NV_ERR_ otherwise
 */
#define NV00DE_CTRL_CMD_REQUEST_POLL_INTERVAL (0xde0002U) /* finn: Evaluated from "(FINN_RM_USER_SHARED_DATA_INTERFACE_ID << 8) | NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM_MESSAGE_ID" */

#define NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM_MESSAGE_ID (0x2U)

typedef struct NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM {
    NvU32 pollingIntervalMs;
} NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM;

/* _ctrl00de.h_ */
