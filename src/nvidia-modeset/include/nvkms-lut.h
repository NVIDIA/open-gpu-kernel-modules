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

#ifndef __NVKMS_LUT_H__
#define __NVKMS_LUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvkms-types.h"

NvBool nvSetTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                              NVFlipEvoHwState *pFlipState,
                              NvU32 head);
void nvRefTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                            NVFlipEvoHwState *pFlipState,
                            NvU32 head);
void nvUnrefTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                              NVFlipEvoHwState *pFlipState,
                              NvU32 head);

NvBool nvAllocLutSurfacesEvo(NVDevEvoPtr pDevEvo);

void nvFreeLutSurfacesEvo(NVDevEvoPtr pDevEvo);

void nvUploadDataToLutSurfaceEvo(NVLutSurfaceEvoPtr pSurfEvo,
                                 const NVEvoLutDataRec *pLUTBuffer,
                                 NVDispEvoPtr pDispEvo);

static inline void nvCancelLutUpdateEvo(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    nvkms_free_timer(pDevEvo->lut.apiHead[apiHead].disp[pDispEvo->displayOwner].updateTimer);
    pDevEvo->lut.apiHead[apiHead].disp[pDispEvo->displayOwner].updateTimer = NULL;
}

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_LUT_H__ */
