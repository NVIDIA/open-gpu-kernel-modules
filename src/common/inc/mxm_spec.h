/*
 * SPDX-FileCopyrightText: Copyright (c) 1993 - 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _MXM_SPEC_H_
#define _MXM_SPEC_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: MXM_SPEC.H                                                        *
*       Defines used for MXM                                                *
\***************************************************************************/


// MXM3.x ACPI revision ID
#define ACPI_MXM_REVISION_ID    0x00000300  // MXM revision ID

// Subfunctions of _DSM in MXM 3.x
#define NV_ACPI_DSM_MXM_FUNC_MXSS         0x00000000  // Supported Sub-Functions
#define NV_ACPI_DSM_MXM_FUNC_MXMI         0x00000018  // Platform MXM Capabilities
#define NV_ACPI_DSM_MXM_FUNC_MXMS         0x00000010  // Get the MXM Structure
#define NV_ACPI_DSM_MXM_FUNC_MXPP         0x00000004  // Get/Set Platform Policies
#define NV_ACPI_DSM_MXM_FUNC_MXDP         0x00000005  // Get/Set Display Config
#define NV_ACPI_DSM_MXM_FUNC_MDTL         0x00000006  // Get Display Toggle List
#define NV_ACPI_DSM_MXM_FUNC_MXCB         0x00000019  // Get Callbacks
#define NV_ACPI_DSM_MXM_FUNC_GETEVENTLIST 0x00000012  // Get Event List
#endif // _MXM_SPEC_H_
