/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef JT_H
#define JT_H

//
// JT ACPI _DSM method related definitions
//
#define JT_REVISION_ID                  0x00000103 // Revision number

// subfunction 0 is common use: NV_ACPI_ALL_FUNC_SUPPORT
// #define JT_FUNC_SUPPORT                 0x00000000 // Function is supported?
#define JT_FUNC_CAPS                    0x00000001 // Capabilities
#define JT_FUNC_POLICYSELECT            0x00000002 // Query Policy Selector Status (reserved for future use)
#define JT_FUNC_POWERCONTROL            0x00000003 // dGPU Power Control
#define JT_FUNC_PLATPOLICY              0x00000004 // Query the Platform Policies (reserved for future use)
#define JT_FUNC_DISPLAYSTATUS           0x00000005 // Query the Display Hot-Key
#define JT_FUNC_MDTL                    0x00000006 // Display Hot-Key Toggle List

//
// JT_FUNC_CAPS return buffer definitions
//
#define NV_JT_FUNC_CAPS_JT_ENABLED                                 0:0
#define NV_JT_FUNC_CAPS_JT_ENABLED_FALSE                    0x00000000
#define NV_JT_FUNC_CAPS_JT_ENABLED_TRUE                     0x00000001
#define NV_JT_FUNC_CAPS_NVSR_ENABLED                               2:1
#define NV_JT_FUNC_CAPS_NVSR_ENABLED_TRUE                   0x00000000
#define NV_JT_FUNC_CAPS_NVSR_ENABLED_FALSE                  0x00000001
#define NV_JT_FUNC_CAPS_PPR                                        4:3
#define NV_JT_FUNC_CAPS_PPR_GC6                             0x00000000
#define NV_JT_FUNC_CAPS_PPR_GC6S3SR                         0x00000002
#define NV_JT_FUNC_CAPS_SRPR                                       5:5
#define NV_JT_FUNC_CAPS_SRPR_PANEL                          0x00000000
#define NV_JT_FUNC_CAPS_SRPR_SUSPEND                        0x00000001
#define NV_JT_FUNC_CAPS_FBPR                                       7:6
#define NV_JT_FUNC_CAPS_FBPR_GC6_ON                         0x00000000
#define NV_JT_FUNC_CAPS_FBPR_GC6_S3                         0x00000002
#define NV_JT_FUNC_CAPS_GPR                                        9:8
#define NV_JT_FUNC_CAPS_GPR_COMBINED                        0x00000000
#define NV_JT_FUNC_CAPS_GPR_PERGPU                          0x00000001
#define NV_JT_FUNC_CAPS_GCR                                      10:10
#define NV_JT_FUNC_CAPS_GCR_EXTERNAL                        0x00000000
#define NV_JT_FUNC_CAPS_GCR_INTEGRATED                      0x00000001
#define NV_JT_FUNC_CAPS_PTH_ENABLED                              11:11
#define NV_JT_FUNC_CAPS_PTH_ENABLED_YES                     0x00000000
#define NV_JT_FUNC_CAPS_PTH_ENABLED_NO                      0x00000001
#define NV_JT_FUNC_CAPS_NOT                                      12:12
#define NV_JT_FUNC_CAPS_NOT_GC6DONE                         0x00000000
#define NV_JT_FUNC_CAPS_NOT_LINKCHANGE                      0x00000001
#define NV_JT_FUNC_CAPS_MSHYB_ENABLED                            13:13
#define NV_JT_FUNC_CAPS_MSHYB_ENABLED_FALSE                 0x00000000
#define NV_JT_FUNC_CAPS_MSHYB_ENABLED_TRUE                  0x00000001
#define NV_JT_FUNC_CAPS_RPC                                      14:14
#define NV_JT_FUNC_CAPS_RPC_DEFAULT                         0x00000000
#define NV_JT_FUNC_CAPS_RPC_FINEGRAIN                       0x00000001
#define NV_JT_FUNC_CAPS_GC6V                                     16:15
#define NV_JT_FUNC_CAPS_GC6V_GC6E                           0x00000000
#define NV_JT_FUNC_CAPS_GC6V_GC6A                           0x00000001
#define NV_JT_FUNC_CAPS_GC6V_GC6R                           0x00000002
#define NV_JT_FUNC_CAPS_GEI_ENABLED                              17:17
#define NV_JT_FUNC_CAPS_GEI_ENABLED_FALSE                   0x00000000
#define NV_JT_FUNC_CAPS_GEI_ENABLED_TRUE                    0x00000001
#define NV_JT_FUNC_CAPS_GSW_ENABLED                              18:18
#define NV_JT_FUNC_CAPS_GSW_ENABLED_FALSE                   0x00000000
#define NV_JT_FUNC_CAPS_GSW_ENABLED_TRUE                    0x00000001
#define NV_JT_FUNC_CAPS_REVISION_ID                              31:20
#define NV_JT_FUNC_CAPS_REVISION_ID_1_00                    0x00000100
#define NV_JT_FUNC_CAPS_REVISION_ID_1_01                    0x00000101
#define NV_JT_FUNC_CAPS_REVISION_ID_1_03                    0x00000103
#define NV_JT_FUNC_CAPS_REVISION_ID_2_00                    0x00000200

//
// JT_FUNC_POWERCONTROL argument definitions (Rev 1.0)
//

//
// GPU Power Control
//
#define NV_JT_FUNC_POWERCONTROL_GPU_POWER_CONTROL                  2:0
#define NV_JT_FUNC_POWERCONTROL_GPU_POWER_CONTROL_GSS       0x00000000
//
// JT_FUNC_POWERCONTROL return buffer definitions
//
#define NV_JT_FUNC_POWERCONTROL_GPU_GC_STATE                       2:0
#define NV_JT_FUNC_POWERCONTROL_GPU_GC_STATE_TRANSITION     0x00000000
#define NV_JT_FUNC_POWERCONTROL_GPU_GC_STATE_ON             0x00000001
#define NV_JT_FUNC_POWERCONTROL_GPU_GC_STATE_OFF            0x00000002
#define NV_JT_FUNC_POWERCONTROL_GPU_GC_STATE_GC6            0x00000003
#define NV_JT_FUNC_POWERCONTROL_GPU_POWER_STATE                    3:3
#define NV_JT_FUNC_POWERCONTROL_GPU_POWER_STATE_OFF         0x00000000
#define NV_JT_FUNC_POWERCONTROL_GPU_POWER_STATE_PWOK        0x00000001

#endif // JT_H

