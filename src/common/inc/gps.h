/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GPS_H
#define GPS_H

#define GPS_REVISION_ID                   0x00000100
#define GPS_2X_REVISION_ID                0x00000200

#define GPS_FUNC_SUPPORT                  0x00000000    // Bit list of supported functions
#define GPS_FUNC_GETOBJBYTYPE             0x00000010    // Fetch any specific Object by Type
#define GPS_FUNC_GETALLOBJS               0x00000011    // Fetch all Objects
#define GPS_FUNC_REQUESTDXSTATE           0x00000012    // Request D-Notifier state
#define GPS_FUNC_GETCALLBACKS             0x00000013    // Get system requested callbacks
#define GPS_FUNC_PCONTROL                 0x0000001C    // GPU power control function
#define GPS_FUNC_PSHARESTATUS             0x00000020    // Get system requested Power Steering settings
#define GPS_FUNC_GETPSS                   0x00000021    // Get _PSS object
#define GPS_FUNC_SETPPC                   0x00000022    // Set _PPC object
#define GPS_FUNC_GETPPC                   0x00000023    // Get _PPC object
#define GPS_FUNC_GETPPL                   0x00000024    // Get CPU package power limits
#define GPS_FUNC_SETPPL                   0x00000025    // Set CPU package power limits
#define GPS_FUNC_GETTRL                   0x00000026    // Get CPU turbo ratio limits
#define GPS_FUNC_SETTRL                   0x00000027    // Set CPU turbo ratio limits
#define GPS_FUNC_GETPPM                   0x00000028    // Get system power modes
#define GPS_FUNC_SETPPM                   0x00000029    // Set system power modes
#define GPS_FUNC_PSHAREPARAMS             0x0000002A    // Get sensor information and capabilities
#define GPS_FUNC_SETEDPPLIMITINFO         0x0000002B    // Send the GPU EDPPeak limit info to platform
#define GPS_FUNC_GETEDPPLIMIT             0x0000002C    // Get EDPPeak limit from platform

#define GPS_EVENT_STATUS_CHANGE           0x000000C0    // when received call GPS_FUNC_PCONTROL,
                                                        // depends on whether system is GPS enabled.

#endif // GPS_H

