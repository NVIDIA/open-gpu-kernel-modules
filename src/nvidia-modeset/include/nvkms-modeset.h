/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_MODESET_H__
#define __NVKMS_MODESET_H__

#include "nvkms-types.h"

#ifdef __cplusplus
extern "C" {
#endif

NvBool nvSetDispModeEvo(NVDevEvoPtr pDevEvo,
                        const struct NvKmsPerOpenDev *pOpenDev,
                        const struct NvKmsSetModeRequest *pRequest,
                        struct NvKmsSetModeReply *pReply,
                        NvBool bypassComposition,
                        NvBool doRasterLock);

typedef NvBool (*NVShutDownHeadsTestFunc)(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head);

void nvShutDownHeads(NVDevEvoPtr pDevEvo, NVShutDownHeadsTestFunc pTestFunc);

NVVBlankCallbackPtr nvRegisterVBlankCallback(NVDispEvoPtr pDispEvo,
                                             NvU32 head,
                                             NVVBlankCallbackProc pCallback,
                                             void *pUserData);
void nvUnregisterVBlankCallback(NVDispEvoPtr pDispEvo,
                                NvU32 head,
                                NVVBlankCallbackPtr pCallback);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_MODESET_H__ */
