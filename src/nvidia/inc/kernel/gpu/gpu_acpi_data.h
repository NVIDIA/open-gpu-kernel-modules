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

#ifndef _GPU_ACPI_DATA_H_
#define _GPU_ACPI_DATA_H_

#include "ctrl/ctrl0073/ctrl0073system.h"

#include "nvctassert.h"
#include "acpigenfuncs.h"
#include "nvstatus.h"
#include "gpu/gpu_halspec.h"

#define MAX_DSM_SUPPORTED_FUNCS_RTN_LEN 8 // # bytes to store supported functions

/* Indicates the current state of mux */
typedef enum
{
    dispMuxState_None = 0,
    dispMuxState_IntegratedGPU,
    dispMuxState_DiscreteGPU,
} DISPMUXSTATE;

typedef struct {
    // supported function status and cache
    NvU32  suppFuncStatus;
    NvU8   suppFuncs[MAX_DSM_SUPPORTED_FUNCS_RTN_LEN];
    NvU32  suppFuncsLen;
    NvBool bArg3isInteger;
    // callback status and cache
    NvU32  callbackStatus;
    NvU32  callback;
} ACPI_DSM_CACHE;

typedef struct {

    ACPI_DSM_CACHE                   dsm[ACPI_DSM_FUNCTION_COUNT];
    ACPI_DSM_FUNCTION                dispStatusHotplugFunc;
    ACPI_DSM_FUNCTION                dispStatusConfigFunc;
    ACPI_DSM_FUNCTION                perfPostPowerStateFunc;
    ACPI_DSM_FUNCTION                stereo3dStateActiveFunc;
    NvU32                            dsmPlatCapsCache[ACPI_DSM_FUNCTION_COUNT];
    NvU32                            MDTLFeatureSupport;

    // cache of generic func/subfunction remappings.
    ACPI_DSM_FUNCTION                dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_COUNT];
    NvU32                            dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_COUNT];
    NvU32                            dsmCurrentFuncSupport;

} ACPI_DATA;

typedef struct DOD_METHOD_DATA
{
    NV_STATUS status;
    NvU32     acpiIdListLen;
    NvU32     acpiIdList[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];
} DOD_METHOD_DATA;

typedef struct JT_METHOD_DATA
{
    NV_STATUS status;
    NvU32     jtCaps;
    NvU16     jtRevId;
    NvBool    bSBIOSCaps;
} JT_METHOD_DATA;

typedef struct MUX_METHOD_DATA_ELEMENT
{
    NvU32       acpiId;
    NvU32       mode;
    NV_STATUS   status;
} MUX_METHOD_DATA_ELEMENT;

typedef struct MUX_METHOD_DATA
{
    NvU32                       tableLen;
    MUX_METHOD_DATA_ELEMENT     acpiIdMuxModeTable[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];
    MUX_METHOD_DATA_ELEMENT     acpiIdMuxPartTable[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];
    MUX_METHOD_DATA_ELEMENT     acpiIdMuxStateTable[NV0073_CTRL_SYSTEM_ACPI_ID_MAP_MAX_DISPLAYS];    
} MUX_METHOD_DATA;

typedef struct CAPS_METHOD_DATA
{
    NV_STATUS status;
    NvU32     optimusCaps;
} CAPS_METHOD_DATA;

typedef struct ACPI_METHOD_DATA
{
    NvBool                                               bValid;
    DOD_METHOD_DATA                                      dodMethodData;
    JT_METHOD_DATA                                       jtMethodData;
    MUX_METHOD_DATA                                      muxMethodData;
    CAPS_METHOD_DATA                                     capsMethodData;
} ACPI_METHOD_DATA;

#endif // _GPU_ACPI_DATA_H_
