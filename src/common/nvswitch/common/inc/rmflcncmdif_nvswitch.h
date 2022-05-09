/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMFLCNCMDIF_NVSWITCH_H_
#define _RMFLCNCMDIF_NVSWITCH_H_

/*!
 * @file   rmflcncmdif_nvswitch.h
 * @brief  Top-level header-file that defines the generic command/message
 *         interfaces that may be used to communicate with the falcon (e.g. SOE)
 */

#include "flcnifcmn.h"
#include "rmsoecmdif.h"

/*!
 * Generic command struct which can be used in generic falcon code
 */
typedef union RM_FLCN_CMD
{
    RM_FLCN_CMD_GEN     cmdGen;

    RM_FLCN_CMD_SOE     cmdSoe;
} RM_FLCN_CMD, *PRM_FLCN_CMD;

/*!
 * Falcon Message structure
 */
typedef union RM_FLCN_MSG
{
    RM_FLCN_MSG_GEN     msgGen;     // Generic Message

    RM_FLCN_MSG_SOE     msgSoe;     // SOE message
} RM_FLCN_MSG, *PRM_FLCN_MSG;

#endif // _RMFLCNCMDIF_NVSWITCH_H_
