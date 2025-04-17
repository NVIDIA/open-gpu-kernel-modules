/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef RMAPI_UTILS_H
#define RMAPI_UTILS_H

#include "rmapi/rmapi.h"


//
// Alloc a client, device and subdevice handle for a gpu
//
NV_STATUS
rmapiutilAllocClientAndDeviceHandles
(
    RM_API   *pRmApi,
    OBJGPU   *pGpu,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubDevice
);

//
// Free client, device and subdevice handles
//
void
rmapiutilFreeClientAndDeviceHandles
(
    RM_API   *pRmApi,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubDevice
);

//
// Return NV_TRUE if the given external class ID is an INTERNAL_ONLY class
//
NvBool rmapiutilIsExternalClassIdInternalOnly(NvU32 externalClassId);

//
// Return the flags and access right associated with this RM control command
//
NV_STATUS rmapiutilGetControlInfo(NvU32 cmd, NvU32 *pFlags,
                                  NvU32 *pAccessRight, NvU32 *pParamsSize);

NvBool rmapiutilSkipErrorMessageForUnsupportedVgpuGuestControl(NvU32 cmd);

#endif /* RMAPI_UTILS_H */
