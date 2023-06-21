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
// Source file:      ctrl/ctrl2080/ctrl2080gpio.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"



#define NV2080_CTRL_CMD_INTERNAL_GPIO_PROGRAM_DIRECTION (0x20802300) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPIO_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS_MESSAGE_ID (0x00U)

typedef struct NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS {
    NvU32  gpioPin; // in
    NvBool bInput; // in
} NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_GPIO_PROGRAM_OUTPUT (0x20802301) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPIO_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS_MESSAGE_ID (0x01U)

typedef struct NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS {
    NvU32 gpioPin; // in
    NvU32 value; // in
} NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_GPIO_READ_INPUT (0x20802302) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPIO_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS_MESSAGE_ID (0x02U)

typedef struct NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS {
    NvU32 gpioPin; // in
    NvU32 value; // out
} NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS;

#define NV2080_CTRL_CMD_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION (0x20802303) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GPIO_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS_MESSAGE_ID (0x03U)

typedef struct NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS {
    NvU32 function; // in
    NvU32 pin; // in
} NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS;

/* _ctrl2080gpio_h_ */

