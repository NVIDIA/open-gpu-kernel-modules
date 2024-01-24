/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CCI_PRIV_NVSWITCH_H_
#define _CCI_PRIV_NVSWITCH_H_

#include "common_nvswitch.h"

#include "nvtypes.h"
#include "cci/cci_nvswitch.h"
#include "cci/cci_cdb_async_nvswitch.h"
#include "cci/cci_cables_nvswitch.h"
#include "soe/cci/cpld_machx03.h"
#include "soe/cci/cci_cmis.h"

//
// CCI is the top-level management state for all cable controllers on a device.
// The management tasks related to cable controllers is encapsulated by a PCS
// or Platform Cable System, for which CCI is largely a container.
//

#define NVSWITCH_CCI_LINK_NUM_MAX      64
#define NVSWITCH_CCI_OSFP_NUM_MAX      32

// Vendor specified number(Used across all LR10/LS10 systems)
#define NVSWITCH_CCI_CDB_CMD_ID         0xcd19

// LS10 Saved LED state
#define CCI_LED_STATE_LED_A CPLD_MACHXO3_LED_STATE_REG_LED_A
#define CCI_LED_STATE_LED_B CPLD_MACHXO3_LED_STATE_REG_LED_B

// Timeout for CMIS access locks
#define NVSWITCH_CCI_CMIS_LOCK_TIMEOUT (10 * NVSWITCH_INTERVAL_1SEC_IN_NS)

// Cable Controller Interface
struct CCI
{
    // Links that are supported by CCI. The value here is defined in the BIOS
    // and is a static property of the system.  See repeater bit in NVLink.
    NvU64  linkMask;

    // ================================================================
    // === State below this line has been moved and can be deleted. ===
    // ================================================================

    // Other member variables specific to CCI go here
    NvBool bDiscovered;
    NvBool bSupported;
    NvBool bInitialized;
    NvU32  osfpMaskAll;         // All the possible module positions
    NvU32  osfpMaskPresent;     // Currently present modules
    NvU32  cagesMask;           // All the possible module cage positions
    NvU32  modulesMask;         // Currently present modules(currently mirrors osfpMaskPresent)
    NvU32  numLinks;
    NVSWITCH_CCI_MODULE_LINK_LANE_MAP *osfp_map;
    struct NVSWITCH_I2C_DEVICE_DESCRIPTOR *osfp_i2c_info;
    NvU32  osfp_map_size;
    NvU32  osfp_num;

    struct {
        void (*functionPtr)(struct nvswitch_device*);
        NvU32 interval;
    } callbackList[NVSWITCH_CCI_CALLBACK_NUM_MAX];
    NvU32  callbackCounter;
    NvU8   xcvrCurrentLedState[NVSWITCH_CCI_OSFP_NUM_MAX];
    NvU8   xcvrNextLedState[NVSWITCH_CCI_OSFP_NUM_MAX];
    NvU64  tpCounterPreviousSum[NVSWITCH_CCI_LINK_NUM_MAX];

    // LS10 cable initialization
    NvU8   cableType[NVSWITCH_CCI_OSFP_NUM_MAX];
    NvBool rxDetEnable[NVSWITCH_CCI_OSFP_NUM_MAX];

    // LS10 Async module onboarding
    CCI_MODULE_STATE moduleState[NVSWITCH_CCI_OSFP_NUM_MAX];

    // LS10 Async CDB management
    CCI_CDB_STATE cdbState[NVSWITCH_CCI_OSFP_NUM_MAX];

    // LS10 Module info
    NvBool isFlatMemory[NVSWITCH_CCI_OSFP_NUM_MAX];
    struct {
        NvBool bLocked;
        NvU32 pid;
        NvU64 timestamp;
    } cmisAccessLock[NVSWITCH_CCI_OSFP_NUM_MAX];

    // LS10 link training mode
    NvBool bModeContinuousALI[NVSWITCH_CCI_OSFP_NUM_MAX];
    NvU64  linkMaskAliRequested; 

    // LS10 Module HW state
    NvBool isFaulty[NVSWITCH_CCI_OSFP_NUM_MAX];

};

// Helper functions for CCI subcomponents
NvlStatus   cciGetModuleId(nvswitch_device *device, NvU32 linkId, NvU32 *pModuleId);
NvBool      cciModulePresent(nvswitch_device *device, NvU32 moduleId);
void        cciGetModulePresenceChange(nvswitch_device *device, NvU32 *pModuleMask);
NvlStatus   cciResetModule(nvswitch_device *device, NvU32 moduleId);
NvlStatus   cciGetXcvrFWInfo(nvswitch_device *device, NvU32 moduleId, NvU8 *pInfo);
NvlStatus   cciSetLPMode(nvswitch_device *device, NvU8 moduleId, NvBool bAssert);
NvBool      cciCheckLPMode(nvswitch_device *device, NvU8 moduleId);
void        cciPingModules(nvswitch_device *device, NvU32 *pMaskPresent);
void        cciGetAllLinks(nvswitch_device *device, NvU64 *pLinkMaskAll);
NvlStatus   cciGetModuleMask(nvswitch_device *device, NvU64 linkMask, NvU32 *pModuleMask);
NvBool      cciCheckXcvrForLinkTraffic(nvswitch_device *device, NvU32 osfp, NvU64 linkMask);
NvlStatus   cciGetLaneMask(nvswitch_device *device, NvU32 linkId, NvU8 *laneMask);
NvBool      cciModuleHWGood(nvswitch_device *device, NvU32 moduleId);
void        cciSetModulePower(nvswitch_device *device, NvU32 moduleId, NvBool bPowerOn);

#endif //_CCI_PRIV_NVSWITCH_H_
