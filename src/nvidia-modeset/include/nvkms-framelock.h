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

#ifndef __NVKMS_FRAMELOCK_H__
#define __NVKMS_FRAMELOCK_H__

#include "nvkms-types.h"

void nvAllocFrameLocksEvo(NVDevEvoPtr pDevEvo);
void nvFreeFrameLocksEvo(NVDevEvoPtr pDevEvo);

NvBool nvFrameLockSetUseHouseSyncEvo(NVFrameLockEvoPtr, NvU32);
NvBool nvFrameLockGetStatusEvo(const NVFrameLockEvoRec *,
                               enum NvKmsFrameLockAttribute attribute,
                               NvS64*);

NvBool nvSetFrameLockDisplayConfigEvo(NVDpyEvoRec *pDpyEvo, NvS64 val);
NvBool nvGetFrameLockDisplayConfigEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *val);
NvBool nvGetFrameLockDisplayConfigValidValuesEvo(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues);

NvBool nvSetDispAttributeEvo(NVDispEvoPtr pDispEvo,
                             struct NvKmsSetDispAttributeParams *pParams);

NvBool nvGetDispAttributeEvo(NVDispEvoPtr pDispEvo,
                             struct NvKmsGetDispAttributeParams *pParams);

NvBool nvGetDispAttributeValidValuesEvo(
    const NVDispEvoRec *pDispEvo,
    struct NvKmsGetDispAttributeValidValuesParams *pParams);

NvBool nvSetFrameLockAttributeEvo(
    NVFrameLockEvoRec *pFrameLockEvo,
    const struct NvKmsSetFrameLockAttributeParams *pParams);

NvBool nvGetFrameLockAttributeEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsGetFrameLockAttributeParams *pParams);

NvBool nvGetFrameLockAttributeValidValuesEvo(
    const NVFrameLockEvoRec *pFrameLockEvo,
    struct NvKmsGetFrameLockAttributeValidValuesParams *pParams);

NvU32 nvGetFramelockServerHead(const NVDispEvoRec *pDispEvo);
NvU32 nvGetFramelockClientHeadsMask(const NVDispEvoRec *pDispEvo);

static inline NvBool
nvIsFramelockableHead(const NVDispEvoRec *pDispEvo, const NvU32 head)
{
    return (head != NV_INVALID_HEAD) &&
           ((head == nvGetFramelockServerHead(pDispEvo)) ||
            ((NVBIT(head) & nvGetFramelockClientHeadsMask(pDispEvo)) != 0x0));
}

void nvUpdateGLSFramelock(const NVDispEvoRec *pDispEvo, const NvU32 head,
                          const NvBool enable, const NvBool server);

#endif /* __NVKMS_FRAMELOCK_H__ */
