/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _SOE_LS10_H_
#define _SOE_LS10_H_

#include "ls10.h"

//
// Functions shared with LR10
//

#include "rmflcncmdif_nvswitch.h"

NvlStatus nvswitch_init_soe_ls10(nvswitch_device *device);
NvlStatus nvswitch_unload_soe_ls10(nvswitch_device *device);
NvlStatus nvswitch_soe_register_event_callbacks_ls10(nvswitch_device *device);
void nvswitch_cci_soe_callback_ls10(nvswitch_device *device, RM_FLCN_MSG *pGenMsg,
                                    void *pParams, NvU32 seqDesc, NV_STATUS status);
NvlStatus nvswitch_set_nport_tprod_state_ls10(nvswitch_device *device, NvU32 nport);
void      nvswitch_soe_unregister_events_ls10(nvswitch_device *device);
void      nvswitch_therm_soe_callback_ls10(nvswitch_device *device, union RM_FLCN_MSG *pMsg,
                                            void *pParams, NvU32 seqDesc, NV_STATUS status);
NvlStatus nvswitch_soe_register_event_callbacks_ls10(nvswitch_device *device);
NvlStatus nvswitch_soe_restore_nport_state_ls10(nvswitch_device *device, NvU32 nport);
NvlStatus nvswitch_soe_issue_nport_reset_ls10(nvswitch_device *device, NvU32 nport);
void      nvswitch_soe_init_l2_state_ls10(nvswitch_device *device);
void      nvswitch_heartbeat_soe_callback_ls10(nvswitch_device *device, RM_FLCN_MSG *pGenMsg,
                                               void *pParams, NvU32 seqDesc, NV_STATUS status);
NvlStatus nvswitch_soe_set_nport_interrupts_ls10(nvswitch_device *device, NvU32 nport, NvBool bEnable);
void      nvswitch_soe_disable_nport_fatal_interrupts_ls10(nvswitch_device *device, NvU32 nport,
                                                        NvU32 nportIntrEnable, NvU8 nportIntrType);
NvlStatus nvswitch_soe_issue_ingress_stop_ls10(nvswitch_device *device, NvU32 nport, NvBool bStop);
#endif //_SOE_LS10_H_
