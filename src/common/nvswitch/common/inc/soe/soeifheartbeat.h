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

#ifndef _SOEIFHEARTBEAT_H_
#define _SOEIFHEARTBEAT_H_

#include "nvfixedtypes.h"

/*!
 * @file   soeifheartbeat.h
 * @brief  SOE Heartbeat Command Queue
 */

/* ------------------------ Defines ---------------------------------*/

typedef struct
{
    NvU8  msgType;
} RM_SOE_HEARTBEAT_MSG_OSFP_THERM;

typedef union
{
    NvU8  msgType;
    RM_SOE_HEARTBEAT_MSG_OSFP_THERM msg;
} RM_SOE_HEARTBEAT_MSG;

// OSFP Thermal Message IDs
enum
{
    RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_WARN_ACTIVATED,
    RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_WARN_DEACTIVATED,
    RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_OVERT_ACTIVATED,
    RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_OVERT_DEACTIVATED,
    RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_HEARTBEAT_SHUTDOWN,
};

#endif  // _SOEIFHEARTBEAT_H_

