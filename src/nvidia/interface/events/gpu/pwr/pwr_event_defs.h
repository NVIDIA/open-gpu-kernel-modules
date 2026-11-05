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

#ifndef _PWR_EVENT_DEFS_H_
#define _PWR_EVENT_DEFS_H_

#include "nvtypes.h"

typedef NvU16 PWR_OP_EVENT_CODE;
enum
{
    PWR_OP_EVENT_CODE_PSHC_REENGAGED                                = 0x0001U,
    PWR_OP_EVENT_CODE_PSHC_DISENGAGED                               = 0x0002U,
    PWR_OP_EVENT_CODE_PSHC_LOW_LIFETIME                             = 0x0003U,
    PWR_OP_EVENT_CODE_PSHC_ZERO_LIFETIME                            = 0x0004U,
};

// Reason the PSHC disengaged. Values match the PMU->RM RPC data0 payload
// (NV2080_CTRL_PMGR_ISINK_CONTROLLERS_XID_ISINK_DISENGAGE_REASON_*).
typedef NvU32 PSHC_DISENGAGE_REASON;
enum
{
    PSHC_DISENGAGE_REASON_SLOWDOWN                                  = 0x01U,
    PSHC_DISENGAGE_REASON_TIMEOUT                                   = 0x02U,
    PSHC_DISENGAGE_REASON_ZERO_LIFETIME                             = 0x03U,
};

#endif // _PWR_EVENT_DEFS_H_
