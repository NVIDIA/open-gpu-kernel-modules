/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-utils-flip.h"
#include "nvkms-private.h"
#include "nvkms-surface.h"
#include "nvkms-cursor.h"
#include "nvkms-sync.h"

/*!
 * Assign the elements in an NVSurfaceEvoPtr[NVKMS_MAX_EYES] array.
 *
 * Use NVEvoApiHandlesRec to translate an
 * NvKmsSurfaceHandle[NVKMS_MAX_EYES] array into an an
 * NVSurfaceEvoPtr[NVKMS_MAX_EYES] array.
 *
 * \param[in]  pOpenDevSurfaceHandles  The client's surfaces.
 * \param[in]  surfaceHandles          The handles naming surfaces.
 * \param[out] pSurfaceEvos            The surface pointers.
 *
 * \return  Return TRUE if all surfaceHandles could be successfully
 *          translated into pSurfaceEvos.  Otherwise, return FALSE.
 */
NvBool nvAssignSurfaceArray(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES],
    const NvBool isUsedByCursorChannel,
    const NvBool isUsedByLayerChannel,
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES])
{
    NvU32 eye;

    nvkms_memset(pSurfaceEvos, 0, sizeof(NVSurfaceEvoRec *) * NVKMS_MAX_EYES);

    for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {
        if (surfaceHandles[eye] != 0) {
            pSurfaceEvos[eye] =
                nvEvoGetSurfaceFromHandle(pDevEvo,
                                          pOpenDevSurfaceHandles,
                                          surfaceHandles[eye],
                                          isUsedByCursorChannel,
                                          isUsedByLayerChannel);
            if ((pSurfaceEvos[eye] == NULL) ||
                (pSurfaceEvos[eye]->isoType != NVKMS_MEMORY_ISO)) {
                return FALSE;
            }
        }
    }
    return TRUE;
}


/*!
 * Assign the NVFlipNIsoSurfaceEvoHwState.
 *
 * Use the given NvKmsNIsoSurface to populate the
 * NVFlipNIsoSurfaceEvoHwState.  Validate that NvKmsNIsoSurface
 * description is legitimate.
 *
 * \param[in]  pDevEvo        The device where the surface will be used.
 * \param[in]  pOpenDevSurfaceHandles  The client's surfaces.
 * \param[in]  pParamsNIso    The client's description of the NISO surface.
 * \param[in]  notifier       Whether the NISO surface is a notifier.
 * \param[in]  pChannel       The channel where the surface will be used.
 * \param[out] pNIsoState     The NVKMS presentation of the NISO surface.
 *
 * \return  Return TRUE if the NVFlipNIsoSurfaceEvoHwState could be
 *          assigned and validated.  Otherwise, return FALSE and leave
 *          the NVFlipNIsoSurfaceEvoHwState untouched.
 */
NvBool nvAssignNIsoEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsNIsoSurface *pParamsNIso,
    const NvBool notifier, /* TRUE=notifier; FALSE=semaphore */
    const NvU32 layer,
    NVFlipNIsoSurfaceEvoHwState *pNIsoState)
{
    NVSurfaceEvoPtr pSurfaceEvo;
    NvU32 elementSizeInBytes = 0, offsetInBytes, maxBytes;

    nvAssert(pParamsNIso->surfaceHandle != 0);

    pSurfaceEvo =
        nvEvoGetSurfaceFromHandle(pDevEvo,
                                  pOpenDevSurfaceHandles,
                                  pParamsNIso->surfaceHandle,
                                  FALSE /* isUsedByCursorChannel */,
                                  TRUE /* isUsedByLayerChannel */);
    if (pSurfaceEvo == NULL) {
        return FALSE;
    }

    /* Attempt to validate the surface: */

    /* Only pitch surfaces can be used */
    if (pSurfaceEvo->layout != NvKmsSurfaceMemoryLayoutPitch) {
        return FALSE;
    }

    if (pSurfaceEvo->isoType != NVKMS_MEMORY_NISO) {
        return FALSE;
    }

    if ((pParamsNIso->format != NVKMS_NISO_FORMAT_FOUR_WORD) &&
        (pParamsNIso->format != NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY) &&
        (pParamsNIso->format != NVKMS_NISO_FORMAT_LEGACY)) {
        return FALSE;
    }

    if ((pDevEvo->caps.validNIsoFormatMask &
         (1 << pParamsNIso->format)) == 0) {
        return FALSE;
    }

    /* Check that the item fits within the surface. */
    switch (pParamsNIso->format) {
    case NVKMS_NISO_FORMAT_FOUR_WORD:
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        elementSizeInBytes = 16;
        break;
    case NVKMS_NISO_FORMAT_LEGACY:
        if (notifier) {
            /* Legacy notifier size depends on the layer. */
            elementSizeInBytes =
                pDevEvo->caps.legacyNotifierFormatSizeBytes[layer];
        } else {
            /* Legacy semaphores are always 4 bytes. */
            elementSizeInBytes = 4;
        }
        break;
    }

#if defined(DEBUG)
    /* Assert that the size calculated by nvkms-sync library is the same as the
     * one we derived from channel caps above. */
    if (notifier) {
        NvBool overlay = (layer != NVKMS_MAIN_LAYER);
        NvU32 libSize = nvKmsSizeOfNotifier(pParamsNIso->format, overlay);
        nvAssert(libSize == elementSizeInBytes);
    } else {
        nvAssert(nvKmsSizeOfSemaphore(pParamsNIso->format) == elementSizeInBytes);
    }
#endif
    /*
     * offsetInWords is an NvU16 and offsetInBytes is an NvU32, so
     * neither of the expressions:
     *   offsetInWords * 4
     *   offsetInBytes + elementSizeInBytes
     * should ever overflow.
     */

    ct_assert(sizeof(pParamsNIso->offsetInWords) == 2);

    offsetInBytes = ((NvU32)pParamsNIso->offsetInWords) * 4;

    /*
     * Compute the upper extent of the NISO element within the surface.
     */

    maxBytes = offsetInBytes + elementSizeInBytes;

    if (maxBytes > pSurfaceEvo->planes[0].rmObjectSizeInBytes) {
        return FALSE;
    }

    /* EVO expects the NISO element to fit within a 4k page. */

    if (maxBytes > 4096) {
        return FALSE;
    }

    /*
     * XXX NVKMS TODO: Check that the surface is in vidmem if
     * NV5070_CTRL_SYSTEM_CAPS_BUG_644815_DNISO_VIDMEM_ONLY
     */

    pNIsoState->pSurfaceEvo = pSurfaceEvo;
    pNIsoState->format = pParamsNIso->format;
    pNIsoState->offsetInWords = pParamsNIso->offsetInWords;

    return TRUE;
}

NvBool nvAssignCompletionNotifierEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsCompletionNotifierDescription *pParamsNotif,
    const NvU32 layer,
    NVFlipCompletionNotifierEvoHwState *pNotif)
{
    NvBool ret;

    nvkms_memset(pNotif, 0, sizeof(*pNotif));

    /* If no surface is specified, we should not use a notifier. */
    if (pParamsNotif->surface.surfaceHandle == 0) {
        return TRUE;
    }

    ret = nvAssignNIsoEvoHwState(pDevEvo,
                                 pOpenDevSurfaceHandles,
                                 &pParamsNotif->surface,
                                 TRUE, /* notifier */
                                 layer,
                                 &pNotif->surface);
    if (ret) {
        pNotif->awaken = pParamsNotif->awaken;
    }

    return ret;
}

NvBool nvAssignSemaphoreEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvU32 layer,
    const NvU32 sd,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject)
{
    NvBool ret;

    nvAssert(!pChannelSyncObjects->useSyncpt);

    nvkms_memset(pFlipSyncObject, 0, sizeof(*pFlipSyncObject));

    if (!pDevEvo->hal->caps.supportsIndependentAcqRelSemaphore) {
        /*! acquire and release sema surface needs to be same */
        if (pChannelSyncObjects->u.semaphores.acquire.surface.surfaceHandle !=
            pChannelSyncObjects->u.semaphores.release.surface.surfaceHandle) {
            return FALSE;
        }
        if (pChannelSyncObjects->u.semaphores.acquire.surface.format !=
            pChannelSyncObjects->u.semaphores.release.surface.format) {
            return FALSE;
        }
        if (pChannelSyncObjects->u.semaphores.acquire.surface.offsetInWords !=
            pChannelSyncObjects->u.semaphores.release.surface.offsetInWords) {
            return FALSE;
        }
    }

    /*! If no surface is specified, we should not use a semaphore.*/
    if (pChannelSyncObjects->u.semaphores.acquire.surface.surfaceHandle != 0) {

        ret = nvAssignNIsoEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pChannelSyncObjects->u.semaphores.acquire.surface,
                    FALSE, /* notifier */
                    layer,
                    &pFlipSyncObject->u.semaphores.acquireSurface);
        if (ret) {
            pFlipSyncObject->u.semaphores.acquireValue =
                pChannelSyncObjects->u.semaphores.acquire.value;
        } else {
            return ret;
        }
    }

    /*! If no surface is specified, we should not use a semaphore.*/
    if (pChannelSyncObjects->u.semaphores.release.surface.surfaceHandle != 0) {

        ret = nvAssignNIsoEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pChannelSyncObjects->u.semaphores.release.surface,
                    FALSE, /* notifier */
                    layer,
                    &pFlipSyncObject->u.semaphores.releaseSurface);
        if (ret) {
            pFlipSyncObject->u.semaphores.releaseValue =
                pChannelSyncObjects->u.semaphores.release.value;
        } else {
            return ret;
        }
    }

    return TRUE;
}

NvBool nvValidatePerLayerCompParams(
    const struct NvKmsCompositionParams *pCompParams,
    const struct NvKmsCompositionCapabilities *pCaps,
    NVSurfaceEvoPtr pSurfaceEvo)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo = (pSurfaceEvo != NULL) ?
        nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvo->format) : NULL;
    const enum NvKmsCompositionColorKeySelect colorKeySelect =
        pCompParams->colorKeySelect;
    NvU32 match;

    if ((pCaps->supportedColorKeySelects & NVBIT(colorKeySelect)) == 0x0) {
        return FALSE;
    }

    NVKMS_COMPOSITION_FOR_MATCH_BITS(colorKeySelect, match) {
        if ((pCaps->colorKeySelect[colorKeySelect].supportedBlendModes[match] &
                NVBIT(pCompParams->blendingMode[match])) == 0x0) {
            return FALSE;
        }

        switch (pCompParams->blendingMode[match]) {
        case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA:
        case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA:
            if (pCompParams->surfaceAlpha != 0) {
                return FALSE;
            }
            break;
        default:
            break;
        }
    }

    /* Match and nomatch pixels should not use alpha blending mode at once. */
    if ((colorKeySelect != NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) &&
        (NvKmsIsCompositionModeUseAlpha(pCompParams->blendingMode[0])) &&
        (NvKmsIsCompositionModeUseAlpha(pCompParams->blendingMode[1]))) {
       return FALSE;
   }

    /*
     * If surface is NULL, no further validation required. The composition
     * parameters do not take effect if surface is NULL.
     */
    if (pFormatInfo == NULL || pFormatInfo->isYUV) {
        return TRUE;
    }

    /* Disable color keying for 8 Bpp surfaces. */
    if ((colorKeySelect == NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) ||
        (colorKeySelect == NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST)) {

        if (pFormatInfo->rgb.bytesPerPixel > 4) {
            return FALSE;
        }
    }

    return TRUE;
}

NvBool
nvAssignCursorSurface(const struct NvKmsPerOpenDev *pOpenDev,
                      const NVDevEvoRec *pDevEvo,
                      const struct NvKmsSetCursorImageCommonParams *pImgParams,
                      NVSurfaceEvoPtr *pSurfaceEvo)

{
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES] = { };

    if (!nvGetCursorImageSurfaces(pDevEvo,
                                  pOpenDevSurfaceHandles,
                                  pImgParams,
                                  pSurfaceEvos)) {
        return FALSE;
    }

    /* XXX NVKMS TODO: add support for stereo cursor */
    if (pSurfaceEvos[NVKMS_RIGHT] != NULL) {
        return FALSE;
    }

    if (pSurfaceEvos[NVKMS_LEFT] != NULL) {
        if (!nvValidatePerLayerCompParams(&pImgParams->cursorCompParams,
                                          &pDevEvo->caps.cursorCompositionCaps,
                                          pSurfaceEvos[NVKMS_LEFT])) {
            return FALSE;
        }
    }

    *pSurfaceEvo = pSurfaceEvos[NVKMS_LEFT];

    return TRUE;
}

