/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file holds GPU firmware related registry key definitions that are
// shared between Windows and Unix
//

#ifndef NV_FIRMWARE_REGISTRY_H
#define NV_FIRMWARE_REGISTRY_H

//
// Registry key that when enabled, will enable use of GPU firmware.
//
// Possible mode values:
//  0 - Do not enable GPU firmware
//  1 - Enable GPU firmware
//  2 - (Default) Use the default enablement policy for GPU firmware
//
// Setting this to anything other than 2 will alter driver firmware-
// enablement policies, possibly disabling GPU firmware where it would
// have otherwise been enabled by default.
//
// Policy bits:
//
// POLICY_ALLOW_FALLBACK:
//  As the normal behavior is to fail GPU initialization if this registry
//  entry is set in such a way that results in an invalid configuration, if
//  instead the user would like the driver to automatically try to fallback
//  to initializing the failing GPU with firmware disabled, then this bit can
//  be set (ex: 0x11 means try to enable GPU firmware but fall back if needed).
//  Note that this can result in a mixed mode configuration (ex: GPU0 has
//  firmware enabled, but GPU1 does not).
//
#define NV_REG_STR_ENABLE_GPU_FIRMWARE                   "EnableGpuFirmware"

#define NV_REG_ENABLE_GPU_FIRMWARE_MODE_MASK              0x0000000F
#define NV_REG_ENABLE_GPU_FIRMWARE_MODE_DISABLED          0x00000000
#define NV_REG_ENABLE_GPU_FIRMWARE_MODE_ENABLED           0x00000001
#define NV_REG_ENABLE_GPU_FIRMWARE_MODE_DEFAULT           0x00000002

#define NV_REG_ENABLE_GPU_FIRMWARE_POLICY_MASK            0x000000F0
#define NV_REG_ENABLE_GPU_FIRMWARE_POLICY_ALLOW_FALLBACK  0x00000010

#define NV_REG_ENABLE_GPU_FIRMWARE_DEFAULT_VALUE          0x00000012

//
// Registry key that when enabled,  will send GPU firmware logs
// to the system log, when possible.
//
// Possible values:
//  0 - Do not send GPU firmware logs to the system log
//  1 - Enable sending of GPU firmware logs to the system log
//  2 - (Default) Enable sending of GPU firmware logs to the system log for
//      the debug kernel driver build only
//
#define NV_REG_STR_ENABLE_GPU_FIRMWARE_LOGS                "EnableGpuFirmwareLogs"

#define NV_REG_ENABLE_GPU_FIRMWARE_LOGS_DISABLE            0x00000000
#define NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE             0x00000001
#define NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE_ON_DEBUG    0x00000002

#endif  // NV_FIRMWARE_REGISTRY_H
