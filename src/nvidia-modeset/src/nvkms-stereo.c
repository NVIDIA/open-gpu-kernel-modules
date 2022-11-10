/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-evo.h"
#include "nvkms-stereo.h"

NvBool nvSetStereo(const NVDispEvoRec *pDispEvo,
                   const NvU32 apiHead,
                   NvBool enable)
{
    NvU32 head;
    NvU32 primaryHead =
        nvGetPrimaryHwHead(pDispEvo, apiHead);

    if (primaryHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    /* Only one head can drive stereo, make sure stereo is disabled
     * on all the seconday hardware heads. */
    FOR_EACH_EVO_HW_HEAD(pDispEvo, apiHead, head) {
        if (head == primaryHead) {
            continue;
        }
        if(!nvSetStereoEvo(pDispEvo, head, FALSE)) {
            nvAssert(!"Failed to disable stereo on secondary head");
        }
    }

    return nvSetStereoEvo(pDispEvo, primaryHead, enable);
}

NvBool nvGetStereo(const NVDispEvoRec *pDispEvo, const NvU32 apiHead)
{
    NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);

    if (head == NV_INVALID_HEAD) {
        return FALSE;
    }

    return nvGetStereoEvo(pDispEvo, head);
}
