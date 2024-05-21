/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOE_LR10_H_
#define _SOE_LR10_H_

#include "lr10.h"

typedef const struct
{

    NvU32 appVersion;
    NvU32 appCodeStartOffset;
    NvU32 appCodeSize;
    NvU32 appCodeImemOffset;
    NvU32 appCodeIsSecure;
    NvU32 appDataStartOffset;
    NvU32 appDataSize;
    NvU32 appDataDmemOffset;
} SOE_UCODE_APP_INFO_LR10, *PSOE_UCODE_APP_INFO_LR10;

typedef const struct
{

    NvU32 version;
    NvU32 numApps;
    NvU32 codeEntryPoint;
    SOE_UCODE_APP_INFO_LR10 apps[0];
} SOE_UCODE_HDR_INFO_LR10, *PSOE_UCODE_HDR_INFO_LR10;

#define NVSWITCH_SOE_WR32_LR10(_d, _instance, _dev, _reg, _data) \
        NVSWITCH_ENG_WR32_LR10(_d, SOE, , _instance, _dev, _reg, _data)

#define NVSWITCH_SOE_RD32_LR10(_d, _instance, _dev, _reg) \
     NVSWITCH_ENG_RD32_LR10(_d, SOE, _instance, _dev, _reg)

//
// Internal function declarations
//
NvlStatus nvswitch_init_soe_lr10(nvswitch_device *device);
void nvswitch_soe_unregister_events_lr10(nvswitch_device *device);
void nvswitch_therm_soe_callback_lr10(nvswitch_device *device, union RM_FLCN_MSG *pMsg,
         void *pParams, NvU32 seqDesc, NV_STATUS status);
NvlStatus nvswitch_soe_register_event_callbacks_lr10(nvswitch_device *device);
void nvswitch_soe_init_l2_state_lr10(nvswitch_device *device);
NvlStatus nvswitch_soe_issue_ingress_stop_lr10(nvswitch_device *device, NvU32 nport, NvBool bStop);
#endif //_SOE_LR10_H_
