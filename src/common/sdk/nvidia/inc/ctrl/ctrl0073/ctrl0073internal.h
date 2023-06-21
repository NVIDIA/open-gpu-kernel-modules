/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0073/ctrl0073internal.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"
#include "ctrl/ctrl0073/ctrl0073system.h"

#define NV0073_CTRL_CMD_INTERNAL_GET_HOTPLUG_UNPLUG_STATE (0x730401U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_INTERNAL_INTERFACE_ID << 8) | NV0073_CTRL_INTERNAL_GET_HOTPLUG_UNPLUG_STATE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_INTERNAL_GET_HOTPLUG_UNPLUG_STATE_PARAMS_MESSAGE_ID (0x1U)

typedef NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS NV0073_CTRL_INTERNAL_GET_HOTPLUG_UNPLUG_STATE_PARAMS;

#define NV0073_CTRL_CMD_INTERNAL_VRR_SET_RGLINE_ACTIVE (0x730402U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_INTERNAL_INTERFACE_ID << 8) | NV0073_CTRL_CMD_INTERNAL_VRR_SET_RGLINE_ACTIVE_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_INTERNAL_VRR_SET_RGLINE_ACTIVE_PARAMS_MESSAGE_ID (0x2U)

typedef NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS NV0073_CTRL_CMD_INTERNAL_VRR_SET_RGLINE_ACTIVE_PARAMS;

/* ctrl0073internal_h */
