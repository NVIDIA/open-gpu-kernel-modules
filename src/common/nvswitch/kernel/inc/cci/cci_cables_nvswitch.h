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

#ifndef _CCI_CABLES_NVSWITCH_H_
#define _CCI_CABLES_NVSWITCH_H_

#include "cci_nvswitch.h"
#include "nvlink_errors.h"
#include "nvtypes.h"
#include "nvfixedtypes.h"
#include "nvstatus.h"
#include "ctrl_dev_nvswitch.h"
#include "export_nvswitch.h"
#include "soe/cci/cci_onboard_phases.h"

#define CABLE_TYPE_INVALID       0x0
#define CABLE_TYPE_COPPER        0x1
#define CABLE_TYPE_DIRECT        0x2          
#define CABLE_TYPE_OPTICAL       0x3

#define CCI_ONBOARD_MAX_ATTEMPTS    (device->regkeys.cci_max_onboard_attempts)
#define CCI_ONBOARD_PHASE_POLL_MAX  (5)

// Union of different onboarding subphases
typedef union CCI_MODULE_ONBOARD_SUBPHASE
{
    CCI_MODULE_ONBOARD_SUBPHASE_OPTICAL optical;
} CCI_MODULE_ONBOARD_SUBPHASE;

// Defines the phase of onboarding that a module is in
typedef struct cci_module_onboard_state
{
    CCI_MODULE_ONBOARD_PHASE onboardPhase;
    CCI_MODULE_ONBOARD_SUBPHASE onboardSubPhase;
} CCI_MODULE_ONBOARD_STATE;

// Used to manage the SLEEP phase of a module
typedef struct cci_module_onboard_sleep_state
{
    NvU64 wakeUpTimestamp;
    CCI_MODULE_ONBOARD_STATE returnState;
} CCI_MODULE_ONBOARD_SLEEP_STATE;

// Stores discovered errors in regards to module onboarding
typedef struct cci_module_onboard_error
{
    NvBool bErrorsChecked;
    NvBool bOnboardFailure;
    CCI_MODULE_ONBOARD_STATE failedOnboardState; 
} CCI_MODULE_ONBOARD_ERROR;

// Defines the current state of a module
typedef struct cci_module_state
{
    NvU8   onboardAttempts;
    NvBool bModuleIdentified;
    NvBool bModuleOnboarded;
    NvBool bLinkTrainDeferred;
    NvBool bPartialLinkTrainComplete;
    NvBool bLinkTrainComplete;
    NvU8   preTrainCounter; 
    NvU64  linkTrainMask;
    NvU64  linkMaskActiveSaved;
    CCI_MODULE_ONBOARD_STATE currOnboardState;
    CCI_MODULE_ONBOARD_STATE prevOnboardState;
    CCI_MODULE_ONBOARD_SLEEP_STATE sleepState;
    CCI_MODULE_ONBOARD_ERROR onboardError;
} CCI_MODULE_STATE;


void cciModulesOnboardInit(nvswitch_device *device);
void cciModulesOnboardCallback(nvswitch_device *device);

void cciCablesInitializeCopperAsync(nvswitch_device *device, NvU8 moduleId);
void cciCablesInitializeDirectAsync(nvswitch_device *device, NvU8 moduleId);
void cciCablesInitializeOpticalAsync(nvswitch_device *device, NvU8 moduleId);

NvBool cciLinkTrainIdle(nvswitch_device *device, NvU8 linkId);
NvBool cciModuleOnboardFailed(nvswitch_device *device, NvU8 moduleId);

NvlStatus cciRequestALI(nvswitch_device *device, NvU64 linkMaskTrain);

void cciModuleOnboardShutdown(nvswitch_device *device);

// For phases that have work that needs to be performed in SOE
NvlStatus cciModulesOnboardSOE(nvswitch_device *device, NvU8 moduleId);

// Error management
void cciModuleOnboardCheckErrors(nvswitch_device *device, NvU8 moduleId);

// Helper functions
NvlStatus cciSetLedsInitialize(nvswitch_device *device, NvU8 moduleId);

void cciModuleOnboardPerformPhaseTransitionAsync(nvswitch_device *device, NvU8 moduleId, 
                                                 CCI_MODULE_ONBOARD_STATE nextState);
void cciModuleOnboardSleepAsync(nvswitch_device *device, NvU8 moduleId, NvU32 ms, 
                                CCI_MODULE_ONBOARD_STATE returnState);
void cciModuleOnboardPerformPhaseAsync(nvswitch_device *device, NvU8 moduleId,
                                       NvlStatus (*func)(nvswitch_device *, NvU8 moduleId),
                                       CCI_MODULE_ONBOARD_STATE nextStateSuccess,
                                       CCI_MODULE_ONBOARD_STATE nextStateFail);
#endif //_CCI_CABLES_NVSWITCH_H_
