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

#ifndef NVHYBRID_ACPI_H
#define NVHYBRID_ACPI_H

#if defined(__cplusplus)
extern "C"
{
#endif // defined(__cplusplus)

/*****************************************************************************
*
* Module:  nvAcpi.h
*
* Description:
* Header file for ACPI related things, including Hybrid and MXM
*
******************************************************************************/



#define NVHG_FUNC_SUPPORT       0x00000000  // Supported Sub-Functions
#define NVHG_FUNC_HYBRIDCAPS    0x00000001  // Capabilities & Mutex
#define NVHG_FUNC_POLICYSELECT  0x00000002  // Query Policy Selector Status
#define NVHG_FUNC_POWERCONTROL  0x00000003  // dGPU Power Control
#define NVHG_FUNC_PLATPOLICY    0x00000004  // Query/Set Platform Policy
#define NVHG_FUNC_DISPLAYSTATUS 0x00000005  // Get Display & Hot-Key information
#define NVHG_FUNC_MDTL          0x00000006  // Display Toggle List
#define NVHG_FUNC_HCSMBLIST     0x00000007  // List of available addresses (MCP7x Desktop Only)
#define NVHG_FUNC_HCSMBADDR     0x00000008  // Returns the SMBus address (MCP7x Desktop Only)
#define NVHG_FUNC_HCREADBYTE    0x00000009  // Read a byte from the EC (MCP7x Desktop Only)
#define NVHG_FUNC_HCSENDBYTE    0x0000000a  // Send a command byte to the EC (MCP7x Desktop Only)
#define NVHG_FUNC_HCGETSTATUS   0x0000000b  // Querying the status of the Adapters' Hybrid EC after an SMbus Host Notify (MCP7x Desktop Only)
#define NVHG_FUNC_HCTRIGDDC     0x0000000c  // Trigger reading a DDC block (Montevina Hybrid & MCP7x)
#define NVHG_FUNC_HCGETDDC      0x0000000d  // Get the DDC block (Montevina Hybrid & MCP7x)
#define NVHG_FUNC_GETMEMTABLE   0x0000000e  // Get the system memory configuration
#define NVHG_FUNC_GETMEMCFG     0x0000000f  // Get the system video memory settings
#define NVHG_FUNC_GETOBJBYTYPE  0x00000010  // Get the firmware object
#define NVHG_FUNC_GETALLOBJS    0x00000011  // Get the directory and all objects

#define NVHG_FUNC_GETEVENTLIST  0x00000012  // Get the List of required Event Notifiers and their meaning
#define NVHG_FUNC_CALLBACKS     0x00000013  // Get the list of system-required callbacks
#define NVHG_FUNC_GETBACKLIGHT  0x00000014  // Get the backlight table

#define NVHG_ERROR_SUCCESS      0x00000000  // Success
#define NVHG_ERROR_UNSPECIFIED  0x80000001  // Generic unspecified error code
#define NVHG_ERROR_UNSUPPORTED  0x80000002  // FunctionCode or SubFunctionCode not supported by this system
#define NVHG_ERROR_PARM_INVALID 0x80000003  // Parameter is invalid (i.e. start page beyond end of buffer)

// ****************************************************
// For MXDS Display Output Mux Control Method
// ****************************************************
#define MXDS_METHOD_GET_MUX_STATE                                           0x00000000
#define MXDS_METHOD_SET_DISP_MUX_TO_THISGPU                                 0x00000001
#define MXDS_METHOD_SET_BACKLIGHT_MUX_TO_THISGPU                            0x00000002
#define MXDS_METHOD_SET_DISP_AND_BACKLIGHT_MUX_TO_THISGPU                   0x00000003

// return buffer definitions
#define MXDS_METHOD_MUX_STATE_NOT_MUXED                                     0x00000000
#define MXDS_METHOD_MUX_STATE_IS_MUXED                                      0x00000001

// 
// ACPI _MXDS (Switch DispMux state) specific defines
// These defines are as per the ACPI spec from Bug 2297713
//
#define MXDS_METHOD_MUX_OP                                                         3:0
#define MXDS_METHOD_MUX_OP_GET                                              0x00000000
#define MXDS_METHOD_MUX_OP_SET                                              0x00000001
#define MXDS_METHOD_MUX_SET_MODE                                                   4:4
#define MXDS_METHOD_MUX_SET_MODE_IGPU                                       0x00000000
#define MXDS_METHOD_MUX_SET_MODE_DGPU                                       0x00000001

// 
// ACPI _LRST (LCD VDD force reset) specific defines
// These defines are as per the ACPI spec from NVIDIA
// DDS Partner Guidelines For Notebook (OEM Doc)
//
#define LRST_METHOD_FORCE_RESET_OP                                                 2:0
#define LRST_METHOD_FORCE_RESET_OP_GET                                      0x00000000
#define LRST_METHOD_FORCE_RESET_OP_SET_LOW                                  0x00000001
#define LRST_METHOD_FORCE_RESET_OP_SET_HIGH                                 0x00000002

#if defined(__cplusplus)
} // extern "C"
#endif // defined(__cplusplus)

#endif // _NVHYBRID_ACPI_H_
