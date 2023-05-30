/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlcbca.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV_COUNTER_COLLECTION_UNIT control commands and parameters */

#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD(cat,idx)                   NVXXXX_CTRL_CMD(0xCBCA, NVCBCA_CTRL_##cat, idx)

#define NVCBCA_CTRL_RESERVED                     (0x00)
#define NVCBCA_CTRL_COUNTER_COLLECTION_UNIT      (0x01)

/*
 * NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible return values:
 *   NV_OK
 */
#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_NULL (0xcbca0000) /* finn: Evaluated from "(FINN_NV_COUNTER_COLLECTION_UNIT_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_SUBSCRIBE
 *
 * This command is used to subscribe performance counter data
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_SUBSCRIBE (0xcbca0101) /* finn: Evaluated from "(FINN_NV_COUNTER_COLLECTION_UNIT_CCU_INTERFACE_ID << 8) | NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS_MESSAGE_ID" */

#define NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 bufferSize, 8);
    NvU32 counterBlockSize;
} NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS;

/*
 * NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_UNSUBSCRIBE
 *
 * This command is used to unsubscribe performance counter data
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_UNSUBSCRIBE      (0xcbca0102) /* finn: Evaluated from "(FINN_NV_COUNTER_COLLECTION_UNIT_CCU_INTERFACE_ID << 8) | 0x2" */

/*
 * NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_SET_STREAM_STATE
 *
 * This command is used to set the counter collection unit stream state
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_SET_STREAM_STATE (0xcbca0103) /* finn: Evaluated from "(FINN_NV_COUNTER_COLLECTION_UNIT_CCU_INTERFACE_ID << 8) | NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS_MESSAGE_ID" */

#define NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS {
    NvBool bStreamState;
} NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS;

/*
 * NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_GET_STREAM_STATE
 *
 * This command is used to get the counter collection unit stream state
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV_COUNTER_COLLECTION_UNIT_CTRL_CMD_GET_STREAM_STATE (0xcbca0104) /* finn: Evaluated from "(FINN_NV_COUNTER_COLLECTION_UNIT_CCU_INTERFACE_ID << 8) | 0x4" */

/* _ctrlcbca_h_ */

