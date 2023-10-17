/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_KMS_PRIVATE_H__
#define __NV_KMS_PRIVATE_H__

#include "nvkms-types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NvKmsPerOpenDev *nvAllocPerOpenDev(struct NvKmsPerOpen *pOpen,
                                          NVDevEvoPtr pDevEvo, NvBool isPrivileged);

void nvFreePerOpenDev(struct NvKmsPerOpen *pOpen,
                      struct NvKmsPerOpenDev *pOpenDev);

void nvSendDpyEventEvo(const NVDpyEvoRec *pDpyEvo, const NvU32 eventType);

void nvSendDpyAttributeChangedEventEvo(const NVDpyEvoRec *pDpyEvo,
                                       const enum NvKmsDpyAttribute attribute,
                                       const NvS64 value);

void nvSendFrameLockAttributeChangedEventEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    const enum NvKmsFrameLockAttribute attribute,
    const NvS64 value);

void nvSendFlipOccurredEventEvo(const NVDispEvoRec *pDispEvo,
                                const NvU32 apiHead, const NvU32 layer);

void nvSendUnicastEvent(struct NvKmsPerOpen *pOpen);

void nvRemoveUnicastEvent(struct NvKmsPerOpen *pOpen);

#if defined(DEBUG)
NvBool nvSurfaceEvoInAnyOpens(const NVSurfaceEvoRec *pSurfaceEvo);
#endif

const struct NvKmsFlipPermissions *nvGetFlipPermissionsFromOpenDev(
    const struct NvKmsPerOpenDev *pOpenDev);

const struct NvKmsModesetPermissions *nvGetModesetPermissionsFromOpenDev(
    const struct NvKmsPerOpenDev *pOpenDev);

NVEvoApiHandlesRec *nvGetSurfaceHandlesFromOpenDev(
    struct NvKmsPerOpenDev *pOpenDev);
const NVEvoApiHandlesRec *nvGetSurfaceHandlesFromOpenDevConst(
    const struct NvKmsPerOpenDev *pOpenDev);

void nvKmsServiceNonStallInterrupt(void *dataPtr, NvU32 dataU32);

#ifdef __cplusplus
};
#endif

#endif /* __NV_KMS_PRIVATE_H__ */
