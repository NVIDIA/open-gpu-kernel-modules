/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file    dev_nvlsaw_ip_addendum.h
 * @brief   NVSwitch specific defines that are missing in the dev_nvlsaw_ip.h manual.
 */

#ifndef __ls10_dev_nvlsaw_ip_addendum_h__
#define __ls10_dev_nvlsaw_ip_addendum_h__

/*
 * SOE ATTACH-DETACH registers to track SOE & Driver
 * status for load and shutdown.
 *
 * SCRATCH_13 tracks status of SOE &
 * SCRATCH_3 tracks status of driver
 */
#define NV_NVLSAW_SOE_ATTACH_DETACH                        NV_NVLSAW_SW_SCRATCH_3
#define NV_NVLSAW_SOE_ATTACH_DETACH_STATUS                                    0:0
#define NV_NVLSAW_SOE_ATTACH_DETACH_STATUS_ATTACHED                           0x1
#define NV_NVLSAW_SOE_ATTACH_DETACH_STATUS_DETACHED                           0x0

#define NV_NVLSAW_DRIVER_ATTACH_DETACH                    NV_NVLSAW_SW_SCRATCH_13
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_STATUS                                 0:0
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_STATUS_ATTACHED                        0x1
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_STATUS_DETACHED                        0x0
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_DEVICE_RESET_REQUIRED                  5:5
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_DEVICE_BLACKLIST_REASON                10:6
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_DEVICE_FABRIC_STATE                    13:11
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_DRIVER_FABRIC_STATE                    16:14
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_FABRIC_MANAGER_ERROR                   23:17
#define NV_NVLSAW_DRIVER_ATTACH_DETACH_EVENT_MESSAGE_COUNT                    31:24

#define NV_NVLSAW_TNVL_MODE                NV_NVLSAW_SECURE_SCRATCH_WARM_GROUP_3(0)
#define NV_NVLSAW_TNVL_MODE_STATUS                                              0:0
#define NV_NVLSAW_TNVL_MODE_STATUS_DISABLED                                     0x0
#define NV_NVLSAW_TNVL_MODE_STATUS_ENABLED                                      0x1

#endif //__ls10_dev_nvlsaw_ip_addendum_h__
