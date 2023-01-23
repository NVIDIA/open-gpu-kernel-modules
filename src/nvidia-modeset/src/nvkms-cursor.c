/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* this source file contains routines for setting and moving the cursor.
 * NV50 specific */

#include "nvkms-cursor.h"
#include "nvkms-types.h"
#include "nvkms-dma.h"
#include "nvkms-utils.h"
#include "nvkms-rm.h"
#include "nvkms-evo.h"
#include "nvkms-vrr.h"
#include "nvkms-surface.h"
#include "nvkms-flip.h"

#include "nvkms-rmapi.h"

#include <class/cl917a.h> /* sizeof(GK104DispCursorControlPio) */

#include <nvos.h> /* NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS */

/*!
 * Get the NVSurfaceEvoPtrs described by NvKmsSetCursorImageCommonParams.
 *
 * Look up the surfaces described by NvKmsSetCursorImageCommonParams,
 * and check that the surfaces are valid for use by cursor on the
 * given pDevEvo.
 *
 * \param[in]  pDevEvo     The device on which the cursor image will be set.
 * \param[in]  pParams     The parameter structure indicating the surfaces.
 * \param[out] pSurfaceEvo The array of surfaces to be assigned.
 *
 * \return  If the parameters are valid, return TRUE and assign
 *          pSurfaceEvo.  Otherwise, return FALSE.
 */
NvBool nvGetCursorImageSurfaces(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsSetCursorImageCommonParams *pParams,
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES])
{
    NvU32 eye;

    nvkms_memset(pSurfaceEvos, 0, sizeof(NVSurfaceEvoRec *) * NVKMS_MAX_EYES);

    /* XXX NVKMS TODO: add support for stereo cursor */
    nvAssert(pParams->surfaceHandle[NVKMS_RIGHT] == 0);

    for (eye = 0; eye < ARRAY_LEN(pParams->surfaceHandle); eye++) {
        if (pParams->surfaceHandle[eye] != 0) {
            NVSurfaceEvoPtr pSurfaceEvo = NULL;
            pSurfaceEvo =
                nvEvoGetSurfaceFromHandle(pDevEvo,
                                          pOpenDevSurfaceHandles,
                                          pParams->surfaceHandle[eye],
                                          TRUE /* isUsedByCursorChannel */,
                                          FALSE /* isUsedByLayerChannel */);
            if ((pSurfaceEvo == NULL) ||
                (pSurfaceEvo->isoType != NVKMS_MEMORY_ISO)) {
                return FALSE;
            }

            pSurfaceEvos[eye] = pSurfaceEvo;
        }
    }

    return TRUE;
}

static void
SetCursorImage(NVDispEvoPtr pDispEvo,
               const NvU32 head,
               NVSurfaceEvoRec *pSurfaceEvoNew,
               const struct NvKmsCompositionParams *pCursorCompParams)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoUpdateState updateState = { };
    const NvU32 sd = pDispEvo->displayOwner;
    NvBool changed = FALSE;

    NVSurfaceEvoPtr pSurfaceEvoOld =
        pDevEvo->gpus[sd].headState[head].cursor.pSurfaceEvo;

    if (pSurfaceEvoNew != NULL &&
        nvkms_memcmp(pCursorCompParams,
                     &pDevEvo->gpus[sd].headState[head].cursor.cursorCompParams,
                     sizeof(*pCursorCompParams)) != 0) {
        pDevEvo->gpus[sd].headState[head].cursor.cursorCompParams =
            *pCursorCompParams;
        changed = TRUE;
    }

    if (pSurfaceEvoNew != pSurfaceEvoOld) {

        if (pSurfaceEvoNew != NULL) {
            nvEvoIncrementSurfaceRefCnts(pSurfaceEvoNew);
        }

        if (pSurfaceEvoOld) {
            nvEvoDecrementSurfaceRefCnts(pSurfaceEvoOld);
        }

        pDevEvo->gpus[sd].headState[head].cursor.pSurfaceEvo = pSurfaceEvoNew;
        changed = TRUE;
    }

    if (changed) {
        nvPushEvoSubDevMaskDisp(pDispEvo);
        pDevEvo->hal->SetCursorImage(
            pDevEvo,
            head,
            pDevEvo->gpus[sd].headState[head].cursor.pSurfaceEvo,
            &updateState,
            &pDevEvo->gpus[sd].headState[head].cursor.cursorCompParams);
        nvEvoUpdateAndKickOff(pDispEvo, FALSE, &updateState,
                              TRUE /* releaseElv */);
        nvPopEvoSubDevMask(pDevEvo);
    }

    if (pSurfaceEvoNew) {
        nvTriggerVrrUnstallSetCursorImage(pDispEvo, changed);
    }
}

static NvBool
FlipCursorImage(NVDispEvoPtr pDispEvo,
                const struct NvKmsPerOpenDev *pOpenDevice,
                NvU32 head,
                const struct NvKmsSetCursorImageCommonParams *pImageParams)
{
    const NvU32 sd = pDispEvo->displayOwner;
    NvBool ret;
    struct NvKmsFlipParams *pFlipParams;
    struct NvKmsFlipRequest *pFlipRequest;

    pFlipParams = nvCalloc(1, sizeof(*pFlipParams));
    if (pFlipParams == NULL) {
        return FALSE;
    }

    pFlipRequest = &pFlipParams->request;

    pFlipRequest->sd[sd].head[head].cursor.image = *pImageParams;
    pFlipRequest->sd[sd].head[head].cursor.imageSpecified = TRUE;

    pFlipRequest->sd[sd].requestedHeadsBitMask = NVBIT(head);

    pFlipRequest->commit = TRUE;

    ret = nvFlipEvo(pDispEvo->pDevEvo,
                    pOpenDevice,
                    pFlipRequest,
                    &pFlipParams->reply,
                    FALSE /* skipUpdate */,
                    FALSE /* allowFlipLock */);

    nvFree(pFlipParams);

    return ret;
}

NvBool nvSetCursorImage(
    NVDispEvoPtr pDispEvo,
    const struct NvKmsPerOpenDev *pOpenDevice,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvU32 head,
    const struct NvKmsSetCursorImageCommonParams *pParams)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES];
    NVSurfaceEvoPtr pSurfaceEvoNew;
    NvBool flipCursorImage = FALSE;

    if (!nvGetCursorImageSurfaces(pDevEvo, pOpenDevSurfaceHandles,
                                  pParams, pSurfaceEvos)) {
        return FALSE;
    }

    pSurfaceEvoNew = pSurfaceEvos[NVKMS_LEFT];

    /*
     * Use flip to apply or remove workaround for hardware bug 2052012
     */
    if (NV5070_CTRL_SYSTEM_GET_CAP(
            pDevEvo->capsBits,
            NV5070_CTRL_SYSTEM_CAPS_BUG_2052012_GLITCHY_MCLK_SWITCH)) {
        const NvU32 sd = pDispEvo->displayOwner;

        NVSurfaceEvoPtr pSurfaceEvoOld =
            pDevEvo->gpus[sd].headState[head].cursor.pSurfaceEvo;

        if ((pSurfaceEvoOld != pSurfaceEvoNew) &&
            (pSurfaceEvoOld == NULL || pSurfaceEvoNew == NULL)) {
            flipCursorImage = TRUE;
        }
    }

    if (flipCursorImage) {
        return FlipCursorImage(pDispEvo,
                               pOpenDevice, head, pParams);
    }

    SetCursorImage(pDispEvo,
                   head,
                   pSurfaceEvoNew,
                   &pParams->cursorCompParams);
    return TRUE;
}

void nvEvoMoveCursorInternal(NVDispEvoPtr pDispEvo,
                             NvU32 head, NvS16 x, NvS16 y)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];

    pDevEvo->cursorHal->MoveCursor(pDevEvo, sd, head, x, y);

    /* If the cursor is visible, trigger VRR unstall to display the
     * cursor at the new postion */
    if (pEvoSubDev->headState[head].cursor.pSurfaceEvo) {
        nvTriggerVrrUnstallMoveCursor(pDispEvo);
    }
}

void nvEvoMoveCursor(NVDispEvoPtr pDispEvo, NvU32 head,
                     const struct NvKmsMoveCursorCommonParams *pParams)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;

    /* XXX NVKMS TODO: validate x,y against current viewport in? */

    pDevEvo->gpus[sd].headState[head].cursor.x = pParams->x;
    pDevEvo->gpus[sd].headState[head].cursor.y = pParams->y;

    nvEvoMoveCursorInternal(pDispEvo,
                            head, pParams->x, pParams->y);
}

// Allocate and map cursor position PIO channels
NvBool nvAllocCursorEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS PioChannelAllocParams = { 0 };
        NVDispEvoPtr pDispEvo;
        NvU32 sd;

        PioChannelAllocParams.channelInstance = head;
        // No notifiers in cursor channel
        PioChannelAllocParams.hObjectNotify = 0;
        pDevEvo->cursorHandle[head] =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (nvRmApiAlloc(
                nvEvoGlobal.clientHandle,
                pDevEvo->displayHandle,
                pDevEvo->cursorHandle[head],
                pDevEvo->cursorHal->klass,
                &PioChannelAllocParams) != NVOS_STATUS_SUCCESS) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                             "Failed to allocate CURSOR PIO for head %d",
                             head);
            nvFreeCursorEvo(pDevEvo);
            return FALSE;
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
            void *pPioDisplayChannel;
            NvU32 status;

            status = nvRmApiMapMemory(
                        nvEvoGlobal.clientHandle,
                        pDevEvo->pSubDevices[sd]->handle,
                        pDevEvo->cursorHandle[head],
                        0,
                        sizeof(GK104DispCursorControlPio),
                        &pPioDisplayChannel,
                        0);
            if (status != NVOS_STATUS_SUCCESS) {
                nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                                  "Failed to map CURSOR PIO for head %d",
                                  head);
                nvFreeCursorEvo(pDevEvo);
                return FALSE;
            }
            pEvoSubDev->cursorPio[head] = pPioDisplayChannel;
        }
    }

    return TRUE;
}

// Free and unmap Cursor PIO Channels
void nvFreeCursorEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDispEvoPtr pDispEvo;
        NvU32 sd;
        NvU32 status;

        if (pDevEvo->cursorHandle[head] == 0) {
            continue;
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
            NvU32 status;

            if (pEvoSubDev->cursorPio[head] == NULL) {
                continue;
            }

            status = nvRmApiUnmapMemory(
                        nvEvoGlobal.clientHandle,
                        pDevEvo->pSubDevices[sd]->handle,
                        pDevEvo->cursorHandle[head],
                        pEvoSubDev->cursorPio[head],
                        0);

            if (status != NVOS_STATUS_SUCCESS) {
                nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                                  "Failed to unmap cursor channel memory");
            }
            pEvoSubDev->cursorPio[head] = NULL;
        }

        status = nvRmApiFree(
                    nvEvoGlobal.clientHandle,
                    pDevEvo->displayHandle,
                    pDevEvo->cursorHandle[head]);

        if (status != NVOS_STATUS_SUCCESS) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                             "Failed to tear down Cursor channel");
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->cursorHandle[head]);

        pDevEvo->cursorHandle[head] = 0;
    }
}

extern NVEvoCursorHAL nvEvoCursor91;
extern NVEvoCursorHAL nvEvoCursorC3;
extern NVEvoCursorHAL nvEvoCursorC5;
extern NVEvoCursorHAL nvEvoCursorC6;

enum NvKmsAllocDeviceStatus nvInitDispHalCursorEvo(NVDevEvoPtr pDevEvo)
{
    static const NVEvoCursorHALPtr cursorTable[] = {
        &nvEvoCursor91,
        &nvEvoCursorC3,
        &nvEvoCursorC5,
        &nvEvoCursorC6,
    };

    int i;

    for (i = 0; i < ARRAY_LEN(cursorTable); i++) {
        if (nvRmEvoClassListCheck(pDevEvo, cursorTable[i]->klass)) {

            pDevEvo->cursorHal = cursorTable[i];

            return NVKMS_ALLOC_DEVICE_STATUS_SUCCESS;
        }
    }

    return NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
}
