/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _ACPICOMMON_H_
#define _ACPICOMMON_H_

#include "acpigenfuncs.h"
#include "core/core.h"
#include "rmconfig.h"

#include "platform/nbsi/nbsi_read.h"
NV_STATUS getAcpiDsmObjectData(OBJGPU *, NvU8**, NvU32 *, ACPI_DSM_FUNCTION, NBSI_GLOB_TYPE, NBSI_VALIDATE);

NV_STATUS testIfDsmFuncSupported(OBJGPU *, ACPI_DSM_FUNCTION);
NV_STATUS testIfDsmSubFunctionEnabled(OBJGPU *, ACPI_DSM_FUNCTION, NvU32);
NV_STATUS remapDsmFunctionAndSubFunction(OBJGPU *, ACPI_DSM_FUNCTION *, NvU32 *);
NV_STATUS getDsmGetObjectSubfunction(OBJGPU *, ACPI_DSM_FUNCTION *, NvU32 *, NvU32*);
void cacheDsmSupportedFunction(OBJGPU *, ACPI_DSM_FUNCTION, NvU32, NvU32 *, NvU32);
NV_STATUS checkDsmCall(OBJGPU *, ACPI_DSM_FUNCTION *, NvU32 *, NvU32 *, NvU16 *);
void acpiDsmInit(OBJGPU *);
NV_STATUS getLicenseKey(OBJGPU *, NvU32, NvU32 *, NvU16 *);
void uncacheDsmFuncStatus(OBJGPU *, ACPI_DSM_FUNCTION, NvU32);

// useful macros
#if NV_PRINTF_ENABLED
#define DSMFuncStr(func) (func <= ACPI_DSM_FUNCTION_CURRENT ? DSMCalls[func] : DSMCalls[ACPI_DSM_FUNCTION_COUNT])
#endif

#define isDsmGetSuppFuncListCached(pGpu, acpiDsmFunction) (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus != DSM_FUNC_STATUS_UNKNOWN)
#define isDsmGetSuppFuncListFailed(pGpu, acpiDsmFunction) (pGpu->acpi.dsm[acpiDsmFunction].suppFuncStatus == DSM_FUNC_STATUS_FAILED)
#define isGenericDsmFunction(acpiDsmFunction) (acpiDsmFunction >= ACPI_DSM_FUNCTION_COUNT)
#define isGenericDsmSubFunction(acpiDsmSubFunction) (acpiDsmSubFunction >= NV_ACPI_GENERIC_FUNC_START)


#define NV_ACPI_ALL_FUNC_SUPPORT       0x00000000   // Common is supported subfunction.
#define NV_ACPI_ALL_FUNC_SUPPORTED     NVBIT(NV_ACPI_ALL_FUNC_SUPPORT) // is common Function supported?
#define NV_ACPI_ALL_SUBFUNC_UNKNOWN   0xFFFFFFFF   // Common define for unknown ACPI sub-function

// All the callbacks (MXM, NBCI, NVHG) use the same bits. These are common.
#define NV_ACPI_CALLBACKS_ARG_POSTPOWERSTATE                               2:2
#define NV_ACPI_CALLBACKS_ARG_POSTPOWERSTATE_NOTIFY                 0x00000001
#define NV_ACPI_CALLBACKS_ARG_CURRENTPOWERSTATE                            7:4
#define NV_ACPI_CALLBACKS_ARG_3DSTEREOSTATE_ACTIVE                         8:8
#define NV_ACPI_CALLBACKS_ARG_3DSTEREOSTATE_ACTIVE_NO               0x00000000
#define NV_ACPI_CALLBACKS_ARG_3DSTEREOSTATE_ACTIVE_YES              0x00000001


#define NV_ACPI_CALLBACKS_RET_POSTPOWERSTATE                               2:2
#define NV_ACPI_CALLBACKS_RET_POSTPOWERSTATE_NOTIFY                 0x00000001
#define NV_ACPI_CALLBACKS_RET_HOTPLUG                                      9:9
#define NV_ACPI_CALLBACKS_RET_HOTPLUG_NOTIFY                        0x00000001
#define NV_ACPI_CALLBACKS_RET_CONFIG                                     10:10
#define NV_ACPI_CALLBACKS_RET_CONFIG_NOTIFY                         0x00000001
#define NV_ACPI_CALLBACKS_RET_3DSTEREOSTATE_ACTIVE                       12:12
#define NV_ACPI_CALLBACKS_RET_3DSTEREOSTATE_ACTIVE_NOTIFY           0x00000001

#define ACPI_NOTIFY_DOCK_EVENT              0x77
#define ACPI_NOTIFY_PANEL_SWITCH            0x80
#define ACPI_NOTIFY_DEVICE_HOTPLUG          0x81
#define ACPI_NOTIFY_CYCLE_DISPLAY_HOTKEY    0x82
#define ACPI_NOTIFY_NEXT_DISPLAY_HOTKEY     0x83
#define ACPI_NOTIFY_PREV_DISPLAY_HOTKEY     0x84
#define ACPI_NOTIFY_CYCLE_BRIGHTNESS_HOTKEY 0x85
#define ACPI_NOTIFY_INC_BRIGHTNESS_HOTKEY   0x86
#define ACPI_NOTIFY_DEC_BRIGHTNESS_HOTKEY   0x87
#define ACPI_NOTIFY_ZERO_BRIGHTNESS_HOTKEY  0x88
#define ACPI_NOTIFY_VIDEO_WAKEUP            0x90

#define ACPI_NOTIFY_GPS_STATUS_CHANGE       0xC0

#define ACPI_NOTIFY_BACKLIGHT_OFF           0xC1
#define ACPI_NOTIFY_BACKLIGHT_ON            0xC2

#define ACPI_NOTIFY_POWER_LEVEL_D1          0xD1
#define ACPI_NOTIFY_POWER_LEVEL_D2          0xD2
#define ACPI_NOTIFY_POWER_LEVEL_D3          0xD3
#define ACPI_NOTIFY_POWER_LEVEL_D4          0xD4
#define ACPI_NOTIFY_POWER_LEVEL_D5          0xD5

#define ACPI_VIDEO_NOTIFY_PROBE             0x81

#define NV_ACPI_DSM_READ_SIZE          (4*1024)  // 4K as per spec

// ****************************************************
// For _ROM Get ROM Data Method
// ****************************************************
#define ROM_METHOD_MAX_RETURN_BUFFER_SIZE                                   0x1000

// these are debug strings for printing which DSM subfunction didn't work.
// These map directly to the ACPI_DSM_FUNCTION enum in gpu/gpu.h.
#if NV_PRINTF_ENABLED
extern const char * const DSMCalls[];
#endif

#endif // _ACPICOMMON_H_

