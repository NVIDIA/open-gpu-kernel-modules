/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl9072.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV9072_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x9072, NV9072_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV9072_CTRL_RESERVED (0x00)
#define NV9072_CTRL_DISP_SW  (0x01)


/*
 * NV9072_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV9072_CTRL_CMD_NULL (0x90720000) /* finn: Evaluated from "(FINN_GF100_DISP_SW_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV9072_CTRL_CMD_NOTIFY_ON_VBLANK
 *
 * This command implements an out-of-band version of the
 * GF100_DISP_SW class's NV9072_NOTIFY_ON_VBLANK method.
 *
 * Parameters:
 *
 *   data
 *     Valid data accepted by the NV9072_NOTIFY_ON_VBLANK method.
 *   bHeadDisabled
 *     Specifies whether head is active while adding vblank
 *     callback.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NVOS_STATUS_INVALID_PARAM_STRUCT
 *    NV_ERR_INVALID_STATE
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NV9072_CTRL_CMD_NOTIFY_ON_VBLANK (0x90720101) /* finn: Evaluated from "(FINN_GF100_DISP_SW_DISP_SW_INTERFACE_ID << 8) | NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS_MESSAGE_ID" */

#define NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS {
    NvU32  data;
    NvBool bHeadDisabled;
} NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS;

/* _ctrl9072.h_ */
