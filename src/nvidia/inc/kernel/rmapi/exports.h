/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _EXPORTS_H
#define _EXPORTS_H

#include "core/core.h"

//
// !! Deprecated. Do not use these exported API functions. Instead use the
// User or Kernel ones below depending on if they are called from Kernel or
// User space.
//
// A User export is to be used for code paths originating from user space and
// MUST pass only user client handles and user-mode pointers. On most OSes, RM
// will sanity check the use of handles and pointers against incorrect or
// malicious use.
//
// A Kernel export is to be used for code paths originating from kernel space
// and MUST pass only kernel client handles and kernel-mode pointers. By default
// RM will skip any validation checks when a Kernel export is called. The onus
// is on the caller that only valid handles and pointers are passed.
// TBD. RM may enable the checks on debug builds or when a regkey is set.
//
// For more information refer to the Kernel_Client_Data_Validation wiki page
//
// WARNING!! RM has validation checks for handles and pointers. An incorrect use
// of export can cause RM failing the API.
//
void        Nv01AllocMemory                 (NVOS02_PARAMETERS*);
void        Nv01AllocObject                 (NVOS05_PARAMETERS*);
void        Nv04Alloc                       (NVOS21_PARAMETERS*);
void        Nv04AllocWithAccess             (NVOS64_PARAMETERS*);
void        Nv01Free                        (NVOS00_PARAMETERS*);
void        Nv04Control                     (NVOS54_PARAMETERS*);
void        Nv04VidHeapControl              (NVOS32_PARAMETERS*);
void        Nv04IdleChannels                (NVOS30_PARAMETERS*);
void        Nv04MapMemory                   (NVOS33_PARAMETERS*);
void        Nv04UnmapMemory                 (NVOS34_PARAMETERS*);
void        Nv04I2CAccess                   (NVOS_I2C_ACCESS_PARAMS*);
void        Nv04AllocContextDma             (NVOS39_PARAMETERS*);
void        Nv04BindContextDma              (NVOS49_PARAMETERS*);
void        Nv04MapMemoryDma                (NVOS46_PARAMETERS*);
void        Nv04UnmapMemoryDma              (NVOS47_PARAMETERS*);
void        Nv04DupObject                   (NVOS55_PARAMETERS*);
void        Nv04Share                       (NVOS57_PARAMETERS*);
void        Nv04AddVblankCallback           (NVOS61_PARAMETERS*);

// exported "User" API functions
void        Nv01AllocMemoryUser                 (NVOS02_PARAMETERS*);
void        Nv01AllocObjectUser                 (NVOS05_PARAMETERS*);
void        Nv04AllocUser                       (NVOS21_PARAMETERS*);
void        Nv04AllocWithAccessUser             (NVOS64_PARAMETERS*);
void        Nv01FreeUser                        (NVOS00_PARAMETERS*);
void        Nv04ControlUser                     (NVOS54_PARAMETERS*);
void        Nv04VidHeapControlUser              (NVOS32_PARAMETERS*);
void        Nv04IdleChannelsUser                (NVOS30_PARAMETERS*);
void        Nv04MapMemoryUser                   (NVOS33_PARAMETERS*);
void        Nv04UnmapMemoryUser                 (NVOS34_PARAMETERS*);
void        Nv04I2CAccessUser                   (NVOS_I2C_ACCESS_PARAMS*);
void        Nv04AllocContextDmaUser             (NVOS39_PARAMETERS*);
void        Nv04BindContextDmaUser              (NVOS49_PARAMETERS*);
void        Nv04MapMemoryDmaUser                (NVOS46_PARAMETERS*);
void        Nv04UnmapMemoryDmaUser              (NVOS47_PARAMETERS*);
void        Nv04DupObjectUser                   (NVOS55_PARAMETERS*);
void        Nv04ShareUser                       (NVOS57_PARAMETERS*);
void        Nv04AddVblankCallbackUser           (NVOS61_PARAMETERS*);

// exported "Kernel" API functions
void        Nv01AllocMemoryKernel                 (NVOS02_PARAMETERS*);
void        Nv01AllocObjectKernel                 (NVOS05_PARAMETERS*);
void        Nv04AllocKernel                       (NVOS21_PARAMETERS*);
void        Nv04AllocWithAccessKernel             (NVOS64_PARAMETERS*);
void        Nv01FreeKernel                        (NVOS00_PARAMETERS*);
void        Nv04ControlKernel                     (NVOS54_PARAMETERS*);
void        Nv04VidHeapControlKernel              (NVOS32_PARAMETERS*);
void        Nv04IdleChannelsKernel                (NVOS30_PARAMETERS*);
void        Nv04MapMemoryKernel                   (NVOS33_PARAMETERS*);
void        Nv04UnmapMemoryKernel                 (NVOS34_PARAMETERS*);
void        Nv04I2CAccessKernel                   (NVOS_I2C_ACCESS_PARAMS*);
void        Nv04AllocContextDmaKernel             (NVOS39_PARAMETERS*);
void        Nv04BindContextDmaKernel              (NVOS49_PARAMETERS*);
void        Nv04MapMemoryDmaKernel                (NVOS46_PARAMETERS*);
void        Nv04UnmapMemoryDmaKernel              (NVOS47_PARAMETERS*);
void        Nv04DupObjectKernel                   (NVOS55_PARAMETERS*);
void        Nv04ShareKernel                       (NVOS57_PARAMETERS*);
void        Nv04AddVblankCallbackKernel           (NVOS61_PARAMETERS*);

// exported "WithSecInfo" API functions
void        Nv01AllocMemoryWithSecInfo            (NVOS02_PARAMETERS*, API_SECURITY_INFO);
void        Nv01AllocObjectWithSecInfo            (NVOS05_PARAMETERS*, API_SECURITY_INFO);
void        Nv04AllocWithSecInfo                  (NVOS21_PARAMETERS*, API_SECURITY_INFO);
void        Nv04AllocWithAccessSecInfo            (NVOS64_PARAMETERS*, API_SECURITY_INFO);
void        Nv01FreeWithSecInfo                   (NVOS00_PARAMETERS*, API_SECURITY_INFO);
void        Nv04ControlWithSecInfo                (NVOS54_PARAMETERS*, API_SECURITY_INFO);
void        Nv04VidHeapControlWithSecInfo         (NVOS32_PARAMETERS*, API_SECURITY_INFO);
void        Nv04IdleChannelsWithSecInfo           (NVOS30_PARAMETERS*, API_SECURITY_INFO);
void        Nv04MapMemoryWithSecInfo              (NVOS33_PARAMETERS*, API_SECURITY_INFO);
void        Nv04UnmapMemoryWithSecInfo            (NVOS34_PARAMETERS*, API_SECURITY_INFO);
void        Nv04I2CAccessWithSecInfo              (NVOS_I2C_ACCESS_PARAMS*, API_SECURITY_INFO);
void        Nv04AllocContextDmaWithSecInfo        (NVOS39_PARAMETERS*, API_SECURITY_INFO);
void        Nv04BindContextDmaWithSecInfo         (NVOS49_PARAMETERS*, API_SECURITY_INFO);
void        Nv04MapMemoryDmaWithSecInfo           (NVOS46_PARAMETERS*, API_SECURITY_INFO);
void        Nv04UnmapMemoryDmaWithSecInfo         (NVOS47_PARAMETERS*, API_SECURITY_INFO);
void        Nv04DupObjectWithSecInfo              (NVOS55_PARAMETERS*, API_SECURITY_INFO);
void        Nv04ShareWithSecInfo                  (NVOS57_PARAMETERS*, API_SECURITY_INFO);

#endif // _EXPORTS_H
