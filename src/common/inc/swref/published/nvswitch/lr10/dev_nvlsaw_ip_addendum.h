/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef __lr10_dev_nvlsaw_ip_addendum_h__
#define __lr10_dev_nvlsaw_ip_addendum_h__

#define NV_NVLSAW_SW_SCRATCH_0_ERASE_LEDGER_CARVEOUT_OFFSET                 11:0
#define NV_NVLSAW_SW_SCRATCH_0_ERASE_LEDGER_CARVEOUT_SIZE                   23:12
// VBIOS write protect mode for nvflash
#define NV_NVLSAW_SW_SCRATCH_0_WRITE_PROTECT_MODE                           24:24
#define NV_NVLSAW_SW_SCRATCH_0_WRITE_PROTECT_MODE_DISABLED             0x00000000
#define NV_NVLSAW_SW_SCRATCH_0_WRITE_PROTECT_MODE_ENABLED              0x00000001

#define NV_NVLSAW_SW_SCRATCH_7_BIOS_OEM_VERSION                             7:0
#define NV_NVLSAW_SW_SCRATCH_7_RESERVED                                     31:8

#define NV_NVLSAW_SW_SCRATCH_2_INFOROM_WRITE_PROTECT_MODE                   0:0
#define NV_NVLSAW_SW_SCRATCH_2_INFOROM_WRITE_PROTECT_MODE_DISABLED   0x00000000
#define NV_NVLSAW_SW_SCRATCH_2_INFOROM_WRITE_PROTECT_MODE_ENABLED    0x00000001
#define NV_NVLSAW_SW_SCRATCH_2_INFOROM_CARVEOUT_OFFSET                      19:8
#define NV_NVLSAW_SW_SCRATCH_2_INFOROM_CARVEOUT_SIZE                        31:20

#define NV_NVLSAW_SCRATCH_COLD_OOB_BLACKLIST_DEVICE_REQUESTED                 0:0
#define NV_NVLSAW_SCRATCH_COLD_OOB_BLACKLIST_DEVICE_REQUESTED_ENABLE   0x00000000
#define NV_NVLSAW_SCRATCH_COLD_OOB_BLACKLIST_DEVICE_REQUESTED_DISABLE  0x00000001

// SCRATCH_12 is used to communicate fabric state to SOE.  Bit fields:
#define NV_NVLSAW_SW_SCRATCH_12_DEVICE_RESET_REQUIRED                       0:0
#define NV_NVLSAW_SW_SCRATCH_12_DEVICE_BLACKLIST_REASON                     5:1
#define NV_NVLSAW_SW_SCRATCH_12_DEVICE_FABRIC_STATE                         8:6
#define NV_NVLSAW_SW_SCRATCH_12_DRIVER_FABRIC_STATE                         11:9
#define NV_NVLSAW_SW_SCRATCH_12_FABRIC_MANAGER_ERROR                        18:12
#define NV_NVLSAW_SW_SCRATCH_12_EVENT_MESSAGE_COUNT                         26:19

#define NV_NVLSAW_SCRATCH_COLD_OOB_BLACKLIST_DEVICE_REQUESTED               0:0

#endif //__lr10_dev_nvlsaw_ip_addendum_h__
