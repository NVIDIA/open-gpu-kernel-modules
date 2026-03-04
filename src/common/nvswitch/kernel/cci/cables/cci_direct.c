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

#include "common_nvswitch.h"
#include "cci/cci_nvswitch.h"
#include "cci/cci_priv_nvswitch.h"
#include "cci/cci_cables_nvswitch.h"

void 
cciCablesInitializeDirectAsync
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextState;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    NVSWITCH_PRINT(device, INFO, 
                "%s: Not implemented.\n",
                __FUNCTION__);
    nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}
