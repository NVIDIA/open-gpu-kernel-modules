/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CCI_ONBOARD_PHASES_H_
#define _CCI_ONBOARD_PHASES_H_

// Defines common onboarding phases
typedef enum cci_module_onboard_phase
{
    CCI_ONBOARD_PHASE_CHECK_CONDITION = 0x0,
    CCI_ONBOARD_PHASE_IDENTIFY = 0x1,
    CCI_ONBOARD_PHASE_INIT_COPPER = 0x2,
    CCI_ONBOARD_PHASE_INIT_DIRECT = 0x3,
    CCI_ONBOARD_PHASE_INIT_OPTICAL = 0x4,
    CCI_ONBOARD_PHASE_LAUNCH_ALI = 0x5,
    CCI_ONBOARD_PHASE_SLEEP = 0x6,
    CCI_ONBOARD_PHASE_MONITOR = 0x7,
    CCI_ONBOARD_PHASE_NON_CONTINUOUS_ALI = 0x8
} CCI_MODULE_ONBOARD_PHASE;

// Defines onboarding phases that are specific to optical modules
typedef enum cci_module_onboard_subphase_optical
{
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_START = 0x0,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CMIS_SELECT_APPLICATION = 0x1,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CONFIGURE_LINKS = 0x2,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_DISABLE_ALI = 0x3,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SETUP = 0x4,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SEND_CDB = 0x5,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_POLL = 0x6,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_GO_TRANSPARENT = 0x7,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_RESET_LINKS = 0x8,
    CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_ENABLE_ALI = 0x9,
} CCI_MODULE_ONBOARD_SUBPHASE_OPTICAL;

#endif //_CCI_ONBOARD_PHASES_H_
