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

#ifndef NVOP_H
#define NVOP_H

#define NVOP_REVISION_ID            0x00000100

// subfunction 0 is common use: NV_ACPI_ALL_FUNC_SUPPORT
// #define NVOP_FUNC_SUPPORT           0x00000000    // Bit list of supported functions*
#define NVOP_FUNC_DISPLAYSTATUS     0x00000005    // Query the Display Hot-Key**
#define NVOP_FUNC_MDTL              0x00000006    // Query Display Toggle List**
#define NVOP_FUNC_HCSMBADDR         0x00000007    // Get the SBIOS SMBus address for hybrid uController
#define NVOP_FUNC_GETOBJBYTYPE      0x00000010    // Get the Firmware Object*
#define NVOP_FUNC_GETALLOBJS        0x00000011    // Get the directory and all Objects *
#define NVOP_FUNC_OPTIMUSCAPS       0x0000001A    // Optimus Capabilities***
#define NVOP_FUNC_OPTIMUSFLAG       0x0000001B    // Update GPU MultiFunction State to sbios
// * Required if any other functions are used.
// ** Required for Optimus-specific display hotkey functionality
// *** Required for Optimus-specific dGPU subsystem power control

// Function 1A: OPTIMUSCAPS
// Args
#define NVOP_FUNC_OPTIMUSCAPS_FLAGS                                                 0:0
#define NVOP_FUNC_OPTIMUSCAPS_FLAGS_ABSENT                                   0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_FLAGS_PRESENT                                  0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_TARGET                                1:1
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_TARGET_SBIOS                   0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_TARGET_DRIVER                  0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_WR_EN                                 2:2
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_WR_EN_FALSE                    0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_WR_EN_TRUE                     0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_POWER_CONTROL                                       25:24
#define NVOP_FUNC_OPTIMUSCAPS_POWER_CONTROL_DONOT_POWER_DOWN_DGPU            0x00000002
#define NVOP_FUNC_OPTIMUSCAPS_POWER_CONTROL_POWER_DOWN_DGPU                  0x00000003

// Returns
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_STATE                                         0:0
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_STATE_DISABLED                         0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_STATE_ENABLED                          0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_POWER                                            4:3
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_POWER_OFF                                 0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_POWER_RESERVED1                           0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_POWER_RESERVED2                           0x00000002
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_POWER_STABILIZED                          0x00000003
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_HOTPLUG_CAPABILITIES                             6:6
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_HOTPLUG_CAPABILITIES_COCONNECTED          0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_MUXED_DDC                                        7:7
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_MUXED_DDC_FALSE                           0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_DGPU_MUXED_DDC_TRUE                            0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_ACTUAL                                8:8
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_ACTUAL_SBIOS                   0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_CFG_SPACE_OWNER_ACTUAL_DRIVER                  0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_CAPABILITIES                                26:24
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_CAPABILITIES_ABSENT                    0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_CAPABILITIES_DYNAMIC_POWER_CONTROL     0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_HDAUDIO_CAPABILITIES                        28:27
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_HDAUDIO_CAPABILITIES_ABSENT            0x00000000
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_HDAUDIO_CAPABILITIES_DISABLED          0x00000001
#define NVOP_FUNC_OPTIMUSCAPS_OPTIMUS_HDAUDIO_CAPABILITIES_PRESENT           0x00000002

// Function 1B: OPTIMUSFLAG
// Args
#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_CONTROL                             0:0
#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_CONTROL_DISABLE                     0x00000000
#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_CONTROL_ENABLE                      0x00000001

#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_STATECHANGE_REQUEST                 1:1 
#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_STATECHANGE_REQUEST_IGNORE          0x00000000
#define NVOP_FUNC_OPTIMUSFLAG_AUDIOCODEC_STATECHANGE_REQUEST_EXECUTE         0x00000001

#define NVOP_FUNC_OPTIMUSFLAG_APPLICATIONS_COUNT                             9:2 

#define NVOP_FUNC_OPTIMUSFLAG_APPLICATIONS_COUNT_CHANGE_REQUEST              10:10
#define NVOP_FUNC_OPTIMUSFLAG_APPLICATIONS_COUNT_CHANGE_REQUEST_IGNORE       0x000000000
#define NVOP_FUNC_OPTIMUSFLAG_APPLICATIONS_COUNT_CHANGE_REQUEST_EXECUTE      0x000000001


// Function 1B: OPTIMUSFLAG
// return 
#define NVOP_RET_OPTIMUSFLAG_AUDIOCODEC_STATE                                 0:0
#define NVOP_RET_OPTIMUSFLAG_AUDIOCODEC_STATE_DISABLE                         0x00000000
#define NVOP_RET_OPTIMUSFLAG_AUDIOCODEC_STATE_ENABLE                          0x00000001

#define NVOP_RET_OPTIMUSFLAG_POLICY                                           3:2
#define NVOP_RET_OPTIMUSFLAG_POLICY_GPU_POWEROFF                              0x00000000
#define NVOP_RET_OPTIMUSFLAG_POLICY_GPU_POWERON                               0x00000001

#define NVOP_RET_OPTIMUSFLAG_POLICYCHANGE_REQUEST                             4:4
#define NVOP_RET_OPTIMUSFLAG_POLICYCHANGE_REQUEST_IGNORE                      0x00000000
#define NVOP_RET_OPTIMUSFLAG_POLICYCHANGE_REQUEST_EXECUTE                     0x00000001

#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_POLICY                                 6:5
#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_POLICY_OPTIMUS                         0x00000000
#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_POLICY_IGPU                            0x00000001
#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_POLICY_DGPU                            0x00000002

#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_REQUEST                                7:7
#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_REQUEST_IGNORE                         0x00000000
#define NVOP_RET_OPTIMUSFLAG_FORCE_GPU_REQUEST_EXECUTE                        0x00000001
#endif // NVOP_H

