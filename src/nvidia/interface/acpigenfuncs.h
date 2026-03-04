/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvacpitypes.h"

#ifndef _ACPIGENFUNCS_H_
#define _ACPIGENFUNCS_H_

#define NV_ACPI_DSM_READ_SIZE (4*1024)

#define NV_ACPI_GENERIC_FUNC_START                  0x0200
#define NV_ACPI_GENERIC_FUNC_COUNT                  8

// Only use these when using the generic function ACPI_DSM_FUNCTION_CURRENT.
#define NV_ACPI_GENERIC_FUNC_DISPLAYSTATUS          (NV_ACPI_GENERIC_FUNC_START+0x00)  // Get Display & Hot-Key information
#define NV_ACPI_GENERIC_FUNC_MDTL                   (NV_ACPI_GENERIC_FUNC_START+0x01)  // Display Toggle List
#define NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE           (NV_ACPI_GENERIC_FUNC_START+0x02)  // Get the firmware object
#define NV_ACPI_GENERIC_FUNC_GETALLOBJS             (NV_ACPI_GENERIC_FUNC_START+0x03)  // Get the directory and all objects
#define NV_ACPI_GENERIC_FUNC_GETEVENTLIST           (NV_ACPI_GENERIC_FUNC_START+0x04)  // Get the List of required Event Notifiers and their meaning
#define NV_ACPI_GENERIC_FUNC_CALLBACKS              (NV_ACPI_GENERIC_FUNC_START+0x05)  // Get the list of system-required callbacks
#define NV_ACPI_GENERIC_FUNC_GETBACKLIGHT           (NV_ACPI_GENERIC_FUNC_START+0x06)  // Get the Backlight
#define NV_ACPI_GENERIC_FUNC_MSTL                   (NV_ACPI_GENERIC_FUNC_START+0x07)  // Get Multiple Stream Topology Toggle info

// structure used for NV_ACPI_GENERIC_FUNC_CTL_TESTSUBFUNCENABLED and NV_ACPI_GENERIC_FUNC_CTL_REMAPFUNC calls.
typedef struct
{
    ACPI_DSM_FUNCTION acpiDsmFunction;
    NvU32             acpiDsmSubFunction;
    NvU32             status;
} DSMTESTCTL, *PDSMTESTCTL;

// when adding new generic functions, change NV_ACPI_GENERIC_FUNC_LAST_SUBFUNCTION to last entry.
#define NV_ACPI_GENERIC_FUNC_LAST_SUBFUNCTION       (NV_ACPI_GENERIC_FUNC_MSTL)
ct_assert(NV_ACPI_GENERIC_FUNC_COUNT == ((NV_ACPI_GENERIC_FUNC_LAST_SUBFUNCTION-NV_ACPI_GENERIC_FUNC_START)+1));

// These are not DSM functions, but used by clients (such as DD) to choose special ctrl0073 processing related to DSM.
#define NV_ACPI_GENERIC_FUNC_CTL_START              0x0600
#define NV_ACPI_GENERIC_FUNC_CTL_TESTSUBFUNCENABLED (NV_ACPI_GENERIC_FUNC_CTL_START+0x00) // exec testIfDsmSubFunctionEnabled
#define NV_ACPI_GENERIC_FUNC_CTL_REMAPFUNC          (NV_ACPI_GENERIC_FUNC_CTL_START+0x01) // exec remapDsmFunctionAndSubFunction
#define NV_ACPI_GENERIC_FUNC_CTL_GETFUNCSUPPORT     (NV_ACPI_GENERIC_FUNC_CTL_START+0x02) // get generic dsm supported functions
                                                                                          //
// when adding new control functions, change NV_ACPI_GENERIC_FUNC_CTL_LAST_SUBFUNCTION to last entry.
#define NV_ACPI_GENERIC_FUNC_CTL_LAST_SUBFUNCTION   (NV_ACPI_GENERIC_FUNC_CTL_GETFUNCSUPPORT)
#define NV_ACPI_GENERIC_FUNC_CTL_COUNT              ((NV_ACPI_GENERIC_FUNC_CTL_LAST_SUBFUNCTION-NV_ACPI_GENERIC_FUNC_CTL_START)+1)

#define IS_GENERIC_DSM_FUNC_SUPPORTED(package, subfunc) (((package >> (subfunc-NV_ACPI_GENERIC_FUNC_START)) & NVBIT(0)) ? true : false)

// status for dsm functions.
#define DSM_FUNC_STATUS_UNKNOWN  0 // untried
#define DSM_FUNC_STATUS_FAILED   1 // tried but failed
#define DSM_FUNC_STATUS_SUCCESS  2 // tried and successful
#define DSM_FUNC_STATUS_DISABLED 3 // disabled via regkey
#define DSM_FUNC_STATUS_OVERRIDE 4 // regkey or code hack override

//
// common NV definitions used in ACPI dsm calls in particular.
//
#define NV_ACPI_ALL_FUNC_SUPPORT       0x00000000   // Common is supported subfunction.
#define NV_ACPI_ALL_FUNC_SUPPORTED     NVBIT(NV_ACPI_ALL_FUNC_SUPPORT) // is common Function supported?
#define NV_ACPI_ALL_SUBFUNC_UNKNOWN   0xFFFFFFFF   // Common define for unknown ACPI sub-function

#endif // _ACPIGENFUNCS_H_

