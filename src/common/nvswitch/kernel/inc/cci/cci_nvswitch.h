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

#ifndef _CCI_NVSWITCH_H_
#define _CCI_NVSWITCH_H_

#include "common_nvswitch.h"

#include "nvlink_errors.h"
#include "nvtypes.h"
#include "nvfixedtypes.h"
#include "nvstatus.h"
#include "ctrl_dev_nvswitch.h"
#include "export_nvswitch.h"

typedef struct CCI CCI, *PCCI;
struct nvswitch_device;

// Polling Callback ids
#define NVSWITCH_CCI_CALLBACK_SMBPBI               0
#define NVSWITCH_CCI_CALLBACK_LINK_STATE_UPDATE    1
#define NVSWITCH_CCI_CALLBACK_CDB                  2
#define NVSWITCH_CCI_CALLBACK_NUM_MAX              3

//
// Determines the range of frequencies that functions can
// run at.
// This rate must be divisible by client provided frequencies.
//
#define NVSWITCH_CCI_POLLING_RATE_HZ  50

// CCI LED states on LS10 Systems
#define CCI_LED_STATE_LOCATE     0U
#define CCI_LED_STATE_FAULT      1U
#define CCI_LED_STATE_OFF        2U
#define CCI_LED_STATE_INITIALIZE 3U
#define CCI_LED_STATE_UP_WARM    4U
#define CCI_LED_STATE_UP_ACTIVE  5U
#define CCI_NUM_LED_STATES       6U

//
// Mapping between XCVR module, nvlink-id, xvcr lane-mask
//
typedef struct nvswitch_cci_module_link_lane_map
{
    NvU8 moduleId;
    NvU8 linkId;
    NvU8 laneMask;
} NVSWITCH_CCI_MODULE_LINK_LANE_MAP;

CCI*      cciAllocNew(void);
NvBool    cciSupported(nvswitch_device *device);
NvlStatus cciInit(nvswitch_device *device, PCCI pCci, NvU32 pci_device_id);
NvlStatus cciLoad(nvswitch_device *device);
void      cciDestroy(nvswitch_device *device, PCCI pCci);
NvlStatus cciRead(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU32 addr, NvU32 length, NvU8 *pVal);
NvlStatus cciWrite(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU32 addr, NvU32 length, NvU8 *pVal);
NvlStatus cciSetBankAndPage(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 bank, NvU8 page);
NvlStatus cciSendCDBCommandAndGetResponse(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU32 command, NvU32 payLength, NvU8 *payload, NvU32 *resLength, NvU8 *response, NvBool padding);
NvlStatus cciSendCDBCommand(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU32 command, NvU32 length, NvU8 *pValArray, NvBool padding);
NvlStatus cciGetBankAndPage(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 *pBank, NvU8 *pPage);
NvlStatus cciGetCDBResponse(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 *response, NvU32 *resLength);
NvlStatus cciGetCDBStatus(nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 *pStatus);
NvlStatus cciWaitForCDBComplete(nvswitch_device *device, NvU32 client, NvU32 osfp);
NvlStatus cciRegisterCallback(nvswitch_device *device, NvU32 callbackId, void (*functionPtr)(nvswitch_device *device), NvU32 rateHz);

NvlStatus   cciModuleEject           (nvswitch_device *device, NvU8 moduleId);
NvBool      cciIsLinkManaged         (nvswitch_device *device, NvU32 linkNumber);
NvlStatus   cciGetLinkMode           (nvswitch_device *device, NvU32 linkNumber, NvU64 *mode);  
NvBool      cciReportLinkErrors      (nvswitch_device *device, NvU32 linkNumber);
NvlStatus   cciGetFWRevisions        (nvswitch_device *device, NvU32 client, NvU32 linkId, NVSWITCH_CCI_GET_FW_REVISIONS *pRevisions);
NvlStatus   cciGetXcvrFWRevisions    (nvswitch_device *device, NvU32 client, NvU32 osfp, NVSWITCH_CCI_GET_FW_REVISIONS *pRevisions);
void        cciDetectXcvrsPresent    (nvswitch_device *device);
NvlStatus   cciGetXcvrMask           (nvswitch_device *device, NvU32 *pMaskAll, NvU32 *pMaskPresent);
NvlStatus   cciGetXcvrLedState       (nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 *pLedState);
NvlStatus   cciSetXcvrLedState       (nvswitch_device *device, NvU32 client, NvU32 osfp, NvBool bSetLocate);
NvlStatus   cciSetNextXcvrLedState   (nvswitch_device *device, NvU32 client, NvU32 osfp, NvU8 nextLedState);
NvlStatus   cciConfigureNvlinkMode   (nvswitch_device *device, NvU32 client, NvU32 linkId, NvBool bTx, NvBool freeze_maintenance, NvBool restart_training, NvBool nvlink_mode);
NvlStatus   cciConfigureNvlinkModeModule (nvswitch_device *device, NvU32 client, NvU8 moduleId, NvU64 linkMask, NvBool freeze_maintenance, NvBool restart_training, NvBool nvlink_mode);
NvBool      cciCheckForPreTraining   (nvswitch_device *device, NvU32 client, NvU32 linkId, NvBool bTx);
NvlStatus   cciApplyControlSetValues (nvswitch_device *device, NvU32 client, NvU32 moduleMask);
NvlStatus   cciGetGradingValues      (nvswitch_device *device, NvU32 client, NvU32 linkId, NvU8 *laneMask, NVSWITCH_CCI_GRADING_VALUES *pGrading);

NvlStatus   cciGetCageMapping        (nvswitch_device *device, NvU8 cageIndex, NvU64 *pLinkMask, NvU64 *pEncodedValue);
NvBool      cciCmisAccessTryLock     (nvswitch_device *device, NvU8 cageIndex);
void        cciCmisAccessReleaseLock (nvswitch_device *device, NvU8 cageIndex);
NvlStatus   cciCmisRead              (nvswitch_device *device, NvU8 cageIndex, NvU8 bank, NvU8 page, NvU8 address, NvU8 count, NvU8 *pData);
NvlStatus   cciCmisWrite             (nvswitch_device *device, NvU8 cageIndex, NvU8 bank, NvU8 page, NvU8 address, NvU8 count, NvU8 *pData);
NvlStatus   cciCmisCageBezelMarking  (nvswitch_device *device, NvU8 cageIndex, char *pBezelMarking);

// CCI Control calls
NvlStatus   nvswitch_ctrl_get_cci_fw_revisions     (nvswitch_device *device, NVSWITCH_CCI_GET_FW_REVISION_PARAMS *pParams);
NvlStatus   nvswitch_ctrl_get_grading_values       (nvswitch_device *device, NVSWITCH_CCI_GET_GRADING_VALUES_PARAMS *pParams);
NvlStatus   nvswitch_ctrl_get_ports_cpld_info      (nvswitch_device *device, NVSWITCH_CCI_GET_PORTS_CPLD_INFO_PARAMS *pParams); 
NvlStatus   nvswitch_ctrl_set_locate_led           (nvswitch_device *device, NVSWITCH_CCI_SET_LOCATE_LED_PARAMS *pParams); 
NvlStatus   nvswitch_ctrl_cci_request_ali          (nvswitch_device *device, NVSWITCH_REQUEST_ALI_PARAMS *pParams); 

#endif //_CCI_NVSWITCH_H_
