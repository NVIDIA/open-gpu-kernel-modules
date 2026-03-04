/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-types.h"
#include "nvkms-utils.h"
#include "nvkms-surface.h"
#include "nvkms-modeset.h"
#include "nvkms-vblank-sem-control.h"


static void
VblankSemControlWrite(
    NVVblankSemControlHeadEntry *pEntry,
    NvU64 vblankCount,
    NvBool bAccel)
{
    volatile struct NvKmsVblankSemControlDataOneHead *pData =
        pEntry->pDataOneHead;
    const NvU32 requestCounter =
        bAccel ? pData->requestCounterAccel : pData->requestCounter;

    //
    // Write the current vblankCount and GPU time, and release the
    // semaphore.  Be sure to release the semaphore last, so that consumers
    // of these fields can use the semaphore to know when the other fields
    // are ready.
    //
    pData->vblankCount = vblankCount;

    //
    // Use gcc builtin to ensure the pData->semaphore write is ordered after the
    // above.
    //
    __sync_synchronize();

    pData->semaphore = requestCounter;

    //
    // Record the request count and current vblankCount, for computation
    // next time.
    //
    pEntry->previousRequestCounter = requestCounter;
    pEntry->previousVblankCount    = vblankCount;
}

static void VblankSemControlCallback(
    NVDispEvoRec *pDispEvo,
    NVVBlankCallbackPtr pCallbackData)
{
    NVDispVblankApiHeadState *pVblankApiHeadState = pCallbackData->pUserData;
    const NvU64 vblankCount = pVblankApiHeadState->vblankCount;
    NVVblankSemControlHeadEntry *pEntry;

    nvListForEachEntry(
        pEntry, &pVblankApiHeadState->vblankSemControl.list, listEntry) {

        volatile struct NvKmsVblankSemControlDataOneHead *pData =
            pEntry->pDataOneHead;
        NvU32 flags, swapInterval;

        const NvU32 requestCounter = pData->requestCounter;

        //
        // Use gcc builtin to ensure the pData->requestCounter read is ordered
        // before the below.
        //
        __sync_synchronize();

        /*
         * If this entry does not have a new request, skip it.  But, still
         * update the vblankCount so that the client always has access to the
         * current vblankCount.
         */
        if (requestCounter == pEntry->previousRequestCounter) {
            pData->vblankCount = vblankCount;
            continue;
        }

        flags = pData->flags;

        swapInterval = DRF_VAL(KMS, _VBLANK_SEM_CONTROL, _SWAP_INTERVAL, flags);

        /*
         * If the requested swapInterval is not satisfied, skip this entry.
         */
        if (swapInterval != 0) {
            if (vblankCount < (pEntry->previousVblankCount + swapInterval)) {
                continue;
            }
        }

        VblankSemControlWrite(pEntry, vblankCount, FALSE /* bAccel */);
    }
}

static NvBool EnableVblankSemControlOneHead(
    NVDispEvoRec *pDispEvo,
    NvU32 apiHead,
    NVVblankSemControl *pVblankSemControl,
    struct NvKmsVblankSemControlDataOneHead *pDataOneHead)
{
    NVDispVblankApiHeadState *pVblankApiHeadState = &pDispEvo->vblankApiHeadState[apiHead];
    NVVblankSemControlHeadEntry *pEntry;
    const NvBool isFirstEntry =
        nvListIsEmpty(&pVblankApiHeadState->vblankSemControl.list);

    pEntry = &pVblankSemControl->headEntry[apiHead];

    pEntry->pDataOneHead = pDataOneHead;
    pEntry->previousRequestCounter = 0;
    pEntry->previousVblankCount = pVblankApiHeadState->vblankCount;

    //
    // If this is the first enabled vblank sem control on head, add a vblank
    // callback.  Note we specify addToFront=true, so that this callback is
    // sequenced before any NotifyVblank callbacks (those use addToFront=false).
    //
    if (isFirstEntry) {
        pVblankApiHeadState->vblankSemControl.pCallbackPtr =
            nvApiHeadRegisterVBlankCallback(pDispEvo,
                                            apiHead,
                                            VblankSemControlCallback,
                                            pVblankApiHeadState,
                                            0 /* listIndex */);
        if (pVblankApiHeadState->vblankSemControl.pCallbackPtr == NULL) {
            nvkms_memset(pEntry, 0, sizeof(*pEntry));
            return FALSE;
        }
    }

    nvListAdd(&pEntry->listEntry, &pVblankApiHeadState->vblankSemControl.list);

    return TRUE;
}

static void DisableVblankSemControlOneHead(
    NVDispEvoRec *pDispEvo,
    NvU32 apiHead,
    NVVblankSemControlHeadEntry *pEntry)
{
    NVDispVblankApiHeadState *pVblankApiHeadState = &pDispEvo->vblankApiHeadState[apiHead];

    //
    // Accelerate any pending semaphores before disabling the vblank sem control.
    //
    VblankSemControlWrite(
        pEntry, pVblankApiHeadState->vblankCount, TRUE /* bAccel */);

    nvListDel(&pEntry->listEntry);

    //
    // If that was the last enabled vblank sem control on head, delete the
    // vblank callback.
    //
    if (nvListIsEmpty(&pVblankApiHeadState->vblankSemControl.list)) {
        nvApiHeadUnregisterVBlankCallback(
            pDispEvo, pVblankApiHeadState->vblankSemControl.pCallbackPtr);
        pVblankApiHeadState->vblankSemControl.pCallbackPtr = NULL;
    }

    nvkms_memset(pEntry, 0, sizeof(*pEntry));
}

static NvBool EnableVblankSemControlValidate(
    NVDevEvoRec *pDevEvo,
    NVDispEvoRec *pDispEvo,
    NVSurfaceEvoRec *pSurfaceEvo,
    NvU64 surfaceOffset)
{
    if (!pDevEvo->supportsVblankSemControl) {
        return FALSE;
    }

    /*
     * We cannot enable VblankSemControl if the requested offset within the
     * surface is too large.
     */
    if (A_plus_B_greater_than_C_U64(
            surfaceOffset,
            sizeof(struct NvKmsVblankSemControlData),
            pSurfaceEvo->planes[0].rmObjectSizeInBytes)) {
        return FALSE;
    }

    /*
     * NvKmsVblankSemControlData must be at least 8-byte aligned, so that GPU
     * semaphore releases can write to 8-byte fields within it with natural
     * alignment.
     */
    if ((surfaceOffset % sizeof(NvU64)) != 0) {
        return FALSE;
    }

    if (nvEvoSurfaceRefCntsTooLarge(pSurfaceEvo)) {
        return FALSE;
    }

    return TRUE;
}

static void DisableVblankSemControl(
    NVDispEvoPtr pDispEvo,
    NVVblankSemControl *pVblankSemControl)
{
    NvU32 apiHead;

    for (apiHead = 0; apiHead < ARRAY_LEN(pVblankSemControl->headEntry); apiHead++) {
        NVVblankSemControlHeadEntry *pEntry =
            &pVblankSemControl->headEntry[apiHead];
        DisableVblankSemControlOneHead(pDispEvo, apiHead, pEntry);
    }
}

NvBool nvEvoDisableVblankSemControl(
    NVDevEvoRec *pDevEvo,
    NVVblankSemControl *pVblankSemControl)
{
    NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[pVblankSemControl->dispIndex];

    if (!pDevEvo->supportsVblankSemControl) {
        return FALSE;
    }

    DisableVblankSemControl(pDispEvo, pVblankSemControl);

    nvEvoDecrementSurfaceRefCnts(pDevEvo, pVblankSemControl->pSurfaceEvo);
    nvFree(pVblankSemControl);
    return TRUE;
}

NVVblankSemControl *nvEvoEnableVblankSemControl(
    NVDevEvoRec *pDevEvo,
    NVDispEvoRec *pDispEvo,
    NVSurfaceEvoRec *pSurfaceEvo,
    NvU64 surfaceOffset)
{
    struct NvKmsVblankSemControlData *pData;
    NVVblankSemControl *pVblankSemControl;
    NvU32 apiHead;

    if (!EnableVblankSemControlValidate(pDevEvo, pDispEvo,
                                        pSurfaceEvo, surfaceOffset)) {
        return NULL;
    }

    /*
     * Lazily map the surface; note we'll just leave the surface mapped after
     * this point.
     */
    if (pSurfaceEvo->cpuAddress[0] == NULL) {
        if (!nvEvoCpuMapSurface(pDevEvo, pSurfaceEvo)) {
            return NULL;
        }
    }

    pData = (struct NvKmsVblankSemControlData *)
        (((NvU8 *) pSurfaceEvo->cpuAddress[0]) + surfaceOffset);

    pVblankSemControl = nvCalloc(1, sizeof(*pVblankSemControl));

    if (pVblankSemControl == NULL) {
        return NULL;
    }

    pVblankSemControl->dispIndex = pDispEvo->displayOwner;
    pVblankSemControl->pSurfaceEvo = pSurfaceEvo;

    for (apiHead = 0; apiHead < ARRAY_LEN(pVblankSemControl->headEntry); apiHead++) {
        if (!EnableVblankSemControlOneHead(pDispEvo,
                                           apiHead,
                                           pVblankSemControl,
                                           &pData->head[apiHead])) {
            /*
             * EnableVblankSemControlOneHead() failed for one head, but previous
             * heads may have succeeded.  Unroll by disabling vblank_sem_control
             * for all heads where the pVblankSemControl is enabled.
             */
            DisableVblankSemControl(pDispEvo, pVblankSemControl);
            nvFree(pVblankSemControl);
            return NULL;
        }
    }

    nvEvoIncrementSurfaceRefCnts(pSurfaceEvo);

    return pVblankSemControl;
}

NvBool nvEvoAccelVblankSemControls(
    NVDevEvoPtr pDevEvo,
    NVDispEvoRec *pDispEvo,
    NvU32 apiHeadMask)
{
    NvU32 apiHead;

    if (!pDevEvo->supportsVblankSemControl) {
        return FALSE;
    }

    FOR_ALL_HEADS(apiHead, apiHeadMask) {

        NVDispVblankApiHeadState *pVblankApiHeadState =
            &pDispEvo->vblankApiHeadState[apiHead];
        NVVblankSemControlHeadEntry *pEntry;

        nvListForEachEntry(
            pEntry, &pVblankApiHeadState->vblankSemControl.list, listEntry) {

            VblankSemControlWrite(
                pEntry,
                pVblankApiHeadState->vblankCount,
                TRUE /* bAccel */);
        }
    }

    return TRUE;
}
