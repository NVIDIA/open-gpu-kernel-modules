/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrld0cd.finn
//

#include "ctrl/ctrl2080/ctrl2080boardobj.h"

/*!
 * Structure describing WPPS profile selection control params.
 */
typedef struct NVD0CD_CTRL_WPPS_PROFILES_CONTROL {
    /*!
     * Mask of active performance modes.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E255 activeModes;
} NVD0CD_CTRL_WPPS_PROFILES_CONTROL;

/*!
 * NVD0CD_CTRL_CMD_SET_PROFILES
 *
 * Sets the active WPPS (Workload Power Profile Settings) profiles.
 */
#define NVD0CD_CTRL_CMD_SET_PROFILES (0xd0cd0101) /* finn: Evaluated from "(FINN_WPPS_CONFIG_SESSION_WPPS_INTERFACE_ID << 8) | NVD0CD_CTRL_SET_PROFILES_PARAMS_MESSAGE_ID" */

#define NVD0CD_CTRL_SET_PROFILES_PARAMS_MESSAGE_ID (0x1U)

typedef NVD0CD_CTRL_WPPS_PROFILES_CONTROL NVD0CD_CTRL_SET_PROFILES_PARAMS;

