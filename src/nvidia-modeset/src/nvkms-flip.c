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

#include "nvkms-dma.h"
#include "nvkms-evo.h"
#include "nvkms-flip.h"
#include "nvkms-flip-workarea.h"
#include "nvkms-surface.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-rm.h"
#include "nvkms-vrr.h"
#include "nvkms-cursor.h"
#include "nvkms-types.h"

#include "nvkms-sync.h"

static void SchedulePostFlipIMPTimer(NVDevEvoPtr pDevEvo);

// The EVO .mfs file defines the maximum minPresentInterval to be 8.
#define NV_MAX_SWAP_INTERVAL 8

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
static NvBool AssignSurfaceArray(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES],
    const NVEvoChannelMask channelMask,
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
                                          channelMask);
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
static NvBool AssignNIsoEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsNIsoSurface *pParamsNIso,
    const NvBool notifier, /* TRUE=notifier; FALSE=semaphore */
    const NVEvoChannel *pChannel,
    NVFlipNIsoSurfaceEvoHwState *pNIsoState)
{
    NVSurfaceEvoPtr pSurfaceEvo;
    NvU32 elementSizeInBytes = 0, offsetInBytes, maxBytes;

    nvAssert(pParamsNIso->surfaceHandle != 0);

    pSurfaceEvo =
        nvEvoGetSurfaceFromHandle(pDevEvo,
                                  pOpenDevSurfaceHandles,
                                  pParamsNIso->surfaceHandle,
                                  pChannel->channelMask);
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
            /* Legacy notifier size depends on the channel. */
            elementSizeInBytes = pChannel->caps.legacyNotifierFormatSizeBytes;
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
        NvBool overlay = !!(pChannel->channelMask &
                            NV_EVO_CHANNEL_MASK_OVERLAY_ALL);
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


static NvBool AssignCompletionNotifierEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsCompletionNotifierDescription *pParamsNotif,
    const NVEvoChannel *pChannel,
    NVFlipCompletionNotifierEvoHwState *pNotif)
{
    NvBool ret;

    nvkms_memset(pNotif, 0, sizeof(*pNotif));

    /* If no surface is specified, we should not use a notifier. */
    if (pParamsNotif->surface.surfaceHandle == 0) {
        return TRUE;
    }

    ret = AssignNIsoEvoHwState(pDevEvo,
                               pOpenDevSurfaceHandles,
                               &pParamsNotif->surface,
                               TRUE, /* notifier */
                               pChannel,
                               &pNotif->surface);
    if (ret) {
        pNotif->awaken = pParamsNotif->awaken;
    }

    return ret;
}

static NvBool AssignSemaphoreEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NVEvoChannel *pChannel,
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

        ret = AssignNIsoEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pChannelSyncObjects->u.semaphores.acquire.surface,
                    FALSE, /* notifier */
                    pChannel,
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

        ret = AssignNIsoEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pChannelSyncObjects->u.semaphores.release.surface,
                    FALSE, /* notifier */
                    pChannel,
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

static NvBool AssignPreSyncptEvoHwState(
    NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject)
{
    NvBool ret, bFound = FALSE;
    NvU32 id = 0;
    NvU32 hSyncptCtxDma, hSyncpt;
    NvU32 value;
    enum NvKmsSyncptType preType;

    nvAssert(pDevEvo->pAllSyncptUsedInCurrentFlip != NULL);

    nvAssert(pChannelSyncObjects->useSyncpt);

    preType = pChannelSyncObjects->u.syncpts.pre.type;

    if (preType == NVKMS_SYNCPT_TYPE_NONE) {
        return TRUE;
    }

    if (preType == NVKMS_SYNCPT_TYPE_FD) {
        /*! Get id from fd using nvhost API */
        NvKmsSyncPtOpParams params = { };
        params.fd_to_id_and_thresh.fd =
            pChannelSyncObjects->u.syncpts.pre.u.fd;
        ret = nvkms_syncpt_op(NVKMS_SYNCPT_OP_FD_TO_ID_AND_THRESH,
                              &params);
        if (!ret) {
            return FALSE;
        }
        id = params.fd_to_id_and_thresh.id;
        value = params.fd_to_id_and_thresh.thresh;
    } else {
        id  = pChannelSyncObjects->u.syncpts.pre.u.raw.id;
        value = pChannelSyncObjects->u.syncpts.pre.u.raw.value;
    }
    if (id >= NV_SYNCPT_GLOBAL_TABLE_LENGTH) {
        return FALSE;
    }
    /*! use id value to check the global table */
    bFound = (pDevEvo->preSyncptTable[id].hCtxDma != 0);
    if (bFound == FALSE) {
        /*! Register - allocate and bind ctxdma to syncpt*/
        ret = nvRmEvoAllocAndBindSyncpt(pDevEvo,
                                        pChannel,
                                        id,
                                        &hSyncpt,
                                        &hSyncptCtxDma);
        if (!ret) {
            nvAssert(!"Failed to register pre-syncpt");
            return FALSE;
        }

        /*! Fill the Entry in Global Table */
        pDevEvo->preSyncptTable[id].hCtxDma = hSyncptCtxDma;
        pDevEvo->preSyncptTable[id].hSyncpt = hSyncpt;
        pDevEvo->preSyncptTable[id].channelMask |= pChannel->channelMask;
        pDevEvo->pAllSyncptUsedInCurrentFlip[id] = NV_TRUE;
        pDevEvo->preSyncptTable[id].id = id;
    } else {
        /*!
         * syncpt found, just bind the context dma of this syncpt
         * to the window if it is not already.
         */
        if ((pDevEvo->preSyncptTable[id].channelMask &
             pChannel->channelMask) == 0) {

            ret = nvRmEvoBindDispContextDMA(pDevEvo,
                                            pChannel,
                                            pDevEvo->preSyncptTable[id].hCtxDma);
            if (ret != NVOS_STATUS_SUCCESS) {
                nvAssert(!"Failed to bind pre-syncpt with ctxdma");
                return ret;
            }
            pDevEvo->preSyncptTable[id].channelMask |= pChannel->channelMask;
            pDevEvo->pAllSyncptUsedInCurrentFlip[id] = NV_TRUE;
            /*! hSyncpt already allocated for id*/
         }
    }
    /*! Fill pre-syncpt related information in hardware state */
    pFlipSyncObject->u.syncpts.preCtxDma = pDevEvo->preSyncptTable[id].hCtxDma;
    pFlipSyncObject->u.syncpts.preValue = value;
    pFlipSyncObject->usingSyncpt = TRUE;

    return TRUE;
}

static NvBool AssignPostSyncptEvoHwState(
    NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject)
{
    enum NvKmsSyncptType postType;
    NvU32 threshold;
    NvKmsSyncPtOpParams params = { };
    NvBool ret = TRUE;

    nvAssert(pChannelSyncObjects->useSyncpt);

    postType = pChannelSyncObjects->u.syncpts.requestedPostType;

    /*!
     * It is possible that syncpt is mentioned but post-syncpt
     * is not specified (case where only pre-syncpt used)
     */
    if (postType == NVKMS_SYNCPT_TYPE_NONE) {
        return TRUE;
    }


    /*! read max value of current syncpt id */
    params.read_maxval.id = pChannel->postSyncpt.id;
    ret = nvkms_syncpt_op(NVKMS_SYNCPT_OP_READ_MAXVAL, &params);
    if (!ret) {
        nvAssert(!"Failed syncpt op READ_MAXVAL");
        return FALSE;
    }

    /*! return threshold to caller but increase only when programming hw */
    threshold = params.read_maxval.maxval + 1;

    /*! each channel associated with one post-syncpt */
    pFlipSyncObject->u.syncpts.postCtxDma = pChannel->postSyncpt.hCtxDma;
    pFlipSyncObject->u.syncpts.postValue = threshold;

    pFlipSyncObject->usingSyncpt = TRUE;

    return TRUE;
}

void nvFillPostSyncptReplyOneChannel(
    NVEvoChannel *pChannel,
    enum NvKmsSyncptType postType,
    struct NvKmsSyncpt *postSyncpt,
    const NVFlipSyncObjectEvoHwState *pHwSyncObject)
{
    if (postType == NVKMS_SYNCPT_TYPE_RAW) {
        postSyncpt->u.raw.id = pChannel->postSyncpt.id;
        postSyncpt->u.raw.value = pHwSyncObject->u.syncpts.postValue;
        postSyncpt->type = NVKMS_SYNCPT_TYPE_RAW;
    } else if (postType == NVKMS_SYNCPT_TYPE_FD) {
        NvBool ret = TRUE;
        NvKmsSyncPtOpParams params = { };
        params.id_and_thresh_to_fd.id = pChannel->postSyncpt.id;
        params.id_and_thresh_to_fd.thresh =
            pHwSyncObject->u.syncpts.postValue;

        ret = nvkms_syncpt_op(NVKMS_SYNCPT_OP_ID_AND_THRESH_TO_FD, &params);
        if (!ret) {
            nvAssert(!"Failed syncpt op ID_AND_THRESH_TO_FD");
            return;
        }
        postSyncpt->u.fd = params.id_and_thresh_to_fd.fd;
        postSyncpt->type = NVKMS_SYNCPT_TYPE_FD;
    }
}

static void FillPostSyncptReply(
    NVDevEvoRec *pDevEvo,
    NvU32 sd,
    const struct NvKmsFlipRequestOneSubDevice *pRequestOneSubDevice,
    struct NvKmsFlipReplyOneSubDevice *pReplyOneSubDevice,
    const struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU32 head;

    /*! check for valid config */
    if (!pDevEvo->supportsSyncpts) {
        return;
    }

    for (head = 0; head < ARRAY_LEN(pRequestOneSubDevice->head); head++) {
        const struct NvKmsFlipCommonParams *pRequestParams =
            &pRequestOneSubDevice->head[head];
        struct NvKmsFlipCommonReplyOneHead *pReplyParams =
            &pReplyOneSubDevice->head[head];
        const NVFlipEvoHwState *pFlipState =
            &pWorkArea->sd[sd].head[head].newState;
        NvU32 layer;

        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            if (!pRequestParams->layer[layer].syncObjects.specified ||
                !pRequestParams->layer[layer].syncObjects.val.useSyncpt) {
                continue;
            }

            nvFillPostSyncptReplyOneChannel(
                pDevEvo->head[head].layer[layer],
                pRequestParams->layer[layer].syncObjects.val.u.syncpts.requestedPostType,
                &pReplyParams->layer[layer].postSyncpt,
                &pFlipState->layer[layer].syncObject);
        }
    }
}

NvBool nvHandleSyncptRegistration(
    NVDevEvoRec *pDevEvo,
    NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    NvBool ret = TRUE;
    NvU32 layer;

    if (!pDevEvo->supportsSyncpts) {
         for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            if (pParams->layer[layer].syncObjects.specified &&
                pParams->layer[layer].syncObjects.val.useSyncpt) {
                return FALSE;
            }
        }

        return TRUE;
    }

    pDevEvo->pAllSyncptUsedInCurrentFlip =
        nvCalloc(1, sizeof(NvBool) * NV_SYNCPT_GLOBAL_TABLE_LENGTH);
    if (pDevEvo->pAllSyncptUsedInCurrentFlip == NULL) {
        return FALSE;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (!pParams->layer[layer].syncObjects.specified ||
            !pParams->layer[layer].syncObjects.val.useSyncpt) {
            continue;
        }

        nvkms_memset(&pFlipState->layer[layer].syncObject,
                     0,
                     sizeof(pFlipState->layer[layer].syncObject));

        ret = AssignPreSyncptEvoHwState(pDevEvo,
                                        pDevEvo->head[head].layer[layer],
                                        &pParams->layer[layer].syncObjects.val,
                                        &pFlipState->layer[layer].syncObject);
        if (!ret) {
            nvAssert(!"Failed to store hw state for layer pre-syncpt");
            goto done;
        }

        ret = AssignPostSyncptEvoHwState(pDevEvo,
                                         pDevEvo->head[head].layer[layer],
                                         &pParams->layer[layer].syncObjects.val,
                                         &pFlipState->layer[layer].syncObject);
        if (!ret) {
            nvAssert(!"Failed to store hw state for layer post-syncpt");
            goto done;
        }
    }

done:
    nvFree(pDevEvo->pAllSyncptUsedInCurrentFlip);
    pDevEvo->pAllSyncptUsedInCurrentFlip = NULL;
    return ret;
}


void nvClearFlipEvoHwState(
    NVFlipEvoHwState *pFlipState)
{
    NvU32 i;

    nvkms_memset(pFlipState, 0, sizeof(*pFlipState));

    for (i = 0; i < ARRAY_LEN(pFlipState->layer); i++) {
        pFlipState->layer[i].cscMatrix = NVKMS_IDENTITY_CSC_MATRIX;
    }
}

/*!
 * Initialize NVFlipEvoHwState with a current snapshot from headState.
 */
void nvInitFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState)
{
    const NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVEvoSubDevHeadStateRec *pSdHeadState;
    NvU32 i;

    nvClearFlipEvoHwState(pFlipState);

    if (!nvHeadIsActive(pDispEvo, head)) {
        return;
    }

    pSdHeadState = &pDevEvo->gpus[sd].headState[head];

    pFlipState->viewPortPointIn = pSdHeadState->viewPortPointIn;
    pFlipState->cursor = pSdHeadState->cursor;

    ct_assert(ARRAY_LEN(pFlipState->layer) == ARRAY_LEN(pSdHeadState->layer));

    for (i = 0; i < ARRAY_LEN(pFlipState->layer); i++) {
        pFlipState->layer[i] = pSdHeadState->layer[i];
    }

    // pFlipState->usage describes the usage bounds that will be necessary after
    // this flip is complete.  Initialize it using pSdHeadState->targetUsage,
    // which describes the usage bounds that will be required just before this
    // flip occurs, rather than pSdHeadState->usage, which describes the usage
    // bounds currently programmed into the hardware.
    //
    // pSdHeadState->usage may have higher bounds than pSdHeadState->targetUsage
    // if TryLoweringUsageBounds has not yet noticed that a satellite channel is
    // no longer in use, or a flip to NULL in a satellite channel is pending but
    // has not yet occurred.
    pFlipState->usage = pSdHeadState->targetUsage;

    pFlipState->disableMidFrameAndDWCFWatermark =
        pSdHeadState->targetDisableMidFrameAndDWCFWatermark;
}


static NvBool IsLayerDirty(const struct NvKmsFlipCommonParams *pParams,
                           const NvU32 layer)
{
    return pParams->layer[layer].surface.specified ||
           pParams->layer[layer].sizeIn.specified ||
           pParams->layer[layer].sizeOut.specified ||
           pParams->layer[layer].outputPosition.specified ||
           pParams->layer[layer].completionNotifier.specified ||
           pParams->layer[layer].syncObjects.specified ||
           pParams->layer[layer].compositionParams.specified ||
           pParams->layer[layer].csc.specified;
}

/*!
 * Check whether the flipPermissions for pOpenDev allow the flipping
 * requested by NvKmsFlipCommonParams.
 */
static NvBool CheckFlipPermissions(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams)
{
    const int dispIndex = pDevEvo->gpus[sd].pDispEvo->displayOwner;
    const struct NvKmsFlipPermissions *pFlipPermissions =
        nvGetFlipPermissionsFromOpenDev(pOpenDev);
    const struct NvKmsModesetPermissions *pModesetPermissions =
        nvGetModesetPermissionsFromOpenDev(pOpenDev);
    const NvU8 allLayersMask = NVBIT(pDevEvo->head[head].numLayers) - 1;
    NvU8 layerMask = 0;
    NvU32 layer;

    nvAssert(pOpenDev != NULL);
    nvAssert(pFlipPermissions != NULL);
    nvAssert(pModesetPermissions != NULL);

    layerMask = pFlipPermissions->disp[dispIndex].head[head].layerMask;

    /*
     * If the client has modeset permissions for this disp+head, allow
     * the client to also perform flips on any layer.
     */
    if (!nvDpyIdListIsEmpty(pModesetPermissions->disp[dispIndex].
                            head[head].dpyIdList)) {
        layerMask = allLayersMask;
    }

    /* Changing viewPortIn requires permission to alter all layers. */

    if (pParams->viewPortIn.specified && (layerMask != allLayersMask)) {
        return FALSE;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (IsLayerDirty(pParams, layer) && ((layerMask & NVBIT(layer)) == 0)) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Determine whether a base channel flip requires a non-tearing present mode.
 *
 * EVO requires a non-tearing flip when certain parameters are changing.  See
 * NV_DISP_BASE_STATE_ERROR_052 in dispClass024XBaseUpdateErrorChecks.mfs.
 */
static NvBool FlipRequiresNonTearingMode(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVFlipChannelEvoHwState *pOld,
    const NVFlipChannelEvoHwState *pNew)
{
    // TODO: Do we need to care about the right eye here?  The error check
    // doesn't.
    const NVSurfaceEvoRec *pOldSurf = pOld->pSurfaceEvo[NVKMS_LEFT];
    const NVSurfaceEvoRec *pNewSurf = pNew->pSurfaceEvo[NVKMS_LEFT];
    NvU32 oldHwFormat = 0, newHwFormat = 0;

    if (pOldSurf == NULL || pNewSurf == NULL) {
        return TRUE;
    }

    // If these functions actually return FALSE at this point, then something is
    // really wrong...
    if (!pDevEvo->hal->ValidateWindowFormat(
            pOldSurf->format, NULL, &oldHwFormat)) {
        nvAssert(FALSE);
    }

    if (!pDevEvo->hal->ValidateWindowFormat(
            pNewSurf->format, NULL, &newHwFormat)) {
        nvAssert(FALSE);
    }

    // Commented entries are things checked in the .mfs that are not yet
    // supported in NVKMS.
    return // SuperSample
           oldHwFormat != newHwFormat ||
           // Gamma
           // Layout (i.e. frame, field1, or field2)
           pOldSurf->widthInPixels != pNewSurf->widthInPixels ||
           pOldSurf->heightInPixels != pNewSurf->heightInPixels ||
           pOldSurf->layout != pNewSurf->layout;
           // UseGainOfs
           // NewBaseLut -- USE_CORE_LUT is programmed in InitChannel*
           // NewOutputLut
}


/*!
 * Apply flip overrides if necessary.
 *
 * 1. Override swap intervals for VRR.
 * 2. If the flip is changing certain parameters, override the tearing mode.
 */
static NvBool ApplyBaseFlipOverrides(
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    NVFlipChannelEvoHwState *pNew,
    NvBool allowVrr)
{
    const NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVFlipChannelEvoHwState *pOld =
        &pDevEvo->gpus[sd].headState[head].layer[NVKMS_MAIN_LAYER];

    // Apply VRR swap interval overrides.
    //
    // Note that this applies the overrides whenever the client requests VRR and
    // VRR is enabled, regardless of whether actually activating it later
    // succeeds.
    if (allowVrr) {
        if (!nvHeadIsActive(pDispEvo, head)) {
            //
            // XXX If VRR is allowed then modeset should have happened before
            // base channel flip, currently we don't know how to do modeset
            // and program base channel for VRR at same time. This should be
            // revisited as part of bug 1731279.
            //
            return FALSE;
        }
        nvApplyVrrBaseFlipOverrides(pDevEvo->gpus[sd].pDispEvo, head,
                                    pOld, pNew);
    }

    if (!nvHeadIsActive(pDispEvo, head)) {
        //
        // This is possible when modeset and base flip happening at same time,
        // tearing parameter does not make sense in that case,
        // it should is disabled.
        //
        pNew->tearing = FALSE;
    } else {
        // Force non-tearing mode if EVO requires it.
        if (FlipRequiresNonTearingMode(pDevEvo, head, pOld, pNew)) {
            pNew->tearing = FALSE;
        }
    }

    return TRUE;
}

static NvBool ValidateScalingUsageBounds(
    const struct NvKmsScalingUsageBounds *pS,
    const struct NvKmsScalingUsageBounds *pMaxS)
{
    return (pS->maxVDownscaleFactor <= pMaxS->maxVDownscaleFactor) &&
           (pS->maxHDownscaleFactor <= pMaxS->maxHDownscaleFactor) &&
           (pS->vTaps <= pMaxS->vTaps) &&
           (!pS->vUpscalingAllowed || pMaxS->vUpscalingAllowed);
}

/*!
 * Validate the requested usage bounds against the specified maximums.
 */
static NvBool ValidateUsageBounds(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const struct NvKmsUsageBounds *pUsage,
    const struct NvKmsUsageBounds *pGuaranteedUsage)
{
    NvU32 i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const NvU64 supportedSurfaceFormatsUnion =
            pUsage->layer[i].supportedSurfaceMemoryFormats |
            pGuaranteedUsage->layer[i].supportedSurfaceMemoryFormats;

        if ((pUsage->layer[i].usable && !pGuaranteedUsage->layer[i].usable) ||
            (supportedSurfaceFormatsUnion !=
             pGuaranteedUsage->layer[i].supportedSurfaceMemoryFormats) ||
            !ValidateScalingUsageBounds(&pUsage->layer[i].scaling,
                                        &pGuaranteedUsage->layer[i].scaling)) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Assign pFlipState->usage.
 */
static NvBool AssignUsageBounds(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState)
{
    struct NvKmsUsageBounds *pUsage = &pFlipState->usage;
    int i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const NVFlipChannelEvoHwState *pLayerFlipState = &pFlipState->layer[i];

        nvInitScalingUsageBounds(pDevEvo, &pUsage->layer[i].scaling);

        if (pLayerFlipState->pSurfaceEvo[NVKMS_LEFT]) {
            pUsage->layer[i].usable = TRUE;
            pUsage->layer[i].supportedSurfaceMemoryFormats =
                nvEvoGetFormatsWithEqualOrLowerUsageBound(
                    pLayerFlipState->pSurfaceEvo[NVKMS_LEFT]->format,
                    pDevEvo->caps.layerCaps[i].supportedSurfaceMemoryFormats);

            /* Scaling is not currently supported for the main layer. Bug 3488083 */
            if (i != NVKMS_MAIN_LAYER && pDevEvo->hal->GetWindowScalingCaps) {
                const NVEvoScalerCaps *pScalerCaps =
                    pDevEvo->hal->GetWindowScalingCaps(pDevEvo);

                if (!nvComputeScalingUsageBounds(pScalerCaps,
                                                 pLayerFlipState->sizeIn.width,
                                                 pLayerFlipState->sizeIn.height,
                                                 pLayerFlipState->sizeOut.width,
                                                 pLayerFlipState->sizeOut.height,
                                                 pLayerFlipState->hTaps,
                                                 pLayerFlipState->vTaps,
                                                 &pUsage->layer[i].scaling)) {
                    return FALSE;
                }
            }
        } else {
            pUsage->layer[i].usable = FALSE;
            pUsage->layer[i].supportedSurfaceMemoryFormats = 0;
        }
    }

    return TRUE;
}

static NvBool OverrideUsageBounds(const NVDevEvoRec *pDevEvo,
                                  NVFlipEvoHwState *pFlipState,
                                  const struct NvKmsFlipCommonParams *pParams,
                                  NvU32 sd,
                                  NvU32 head,
                                  const struct NvKmsUsageBounds *pPossibleUsage)
{
    NvU32 i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const struct NvKmsScalingUsageBounds *pPossibleScaling =
            &pPossibleUsage->layer[i].scaling;
        struct NvKmsScalingUsageBounds *pTargetScaling =
            &pFlipState->usage.layer[i].scaling;
        NvU16 possibleV = pPossibleScaling->maxVDownscaleFactor;
        NvU16 possibleH = pPossibleScaling->maxHDownscaleFactor;
        NvU16 targetV = pTargetScaling->maxVDownscaleFactor;
        NvU16 targetH = pTargetScaling->maxHDownscaleFactor;

        if (!pFlipState->usage.layer[i].usable) {
            continue;
        }

        if (pParams->layer[i].maxDownscaleFactors.specified) {
            NvU16 requestedV = pParams->layer[i].maxDownscaleFactors.vertical;
            NvU16 requestedH = pParams->layer[i].maxDownscaleFactors.horizontal;

            if ((requestedV < targetV) || (requestedH < targetH)) {
                return FALSE;
            }

            if ((requestedV > possibleV) || (requestedH > possibleH)) {
                return FALSE;
            }

            pTargetScaling->maxVDownscaleFactor = requestedV;
            pTargetScaling->maxHDownscaleFactor = requestedH;
        } else {
            /*
             * Calculate max H/V downscale factor by quantizing the range.
             *
             * E.g.,
             * max H/V downscale factor supported by HW is 4x for 5-tap and 2x
             * for 2-tap mode. If 5-tap mode is required, the target usage bound
             * that nvkms will attempt to program will either allow up to 2x
             * downscaling, or up to 4x downscaling. If 2-tap mode is required,
             * the target usage bound that NVKMS will attempt to program will
             * allow up to 2x downscaling. Example: to downscale from 4096x2160
             * -> 2731x864 in 5-tap mode, NVKMS would specify up to 2x for the
             * H downscale bound (required is 1.5x), and up to 4x for the V
             * downscale bound (required is 2.5x).
             */
            if (targetV > NV_EVO_SCALE_FACTOR_1X) {
                const NvU16 possibleMid =
                    NV_EVO_SCALE_FACTOR_1X + ((possibleV - NV_EVO_SCALE_FACTOR_1X) / 2);

                if (targetV <= possibleMid) {
                    pTargetScaling->maxVDownscaleFactor = possibleMid;
                } else {
                    pTargetScaling->maxVDownscaleFactor = possibleV;
                }
            }

            if (targetH > NV_EVO_SCALE_FACTOR_1X) {
                const NvU16 possibleMid =
                    NV_EVO_SCALE_FACTOR_1X + ((possibleH - NV_EVO_SCALE_FACTOR_1X) / 2);

                if (targetH <= possibleMid) {
                    pTargetScaling->maxHDownscaleFactor = possibleMid;
                } else {
                    pTargetScaling->maxHDownscaleFactor = possibleH;
                }
            }
        }

        pTargetScaling->vTaps = pPossibleScaling->vTaps;
        pTargetScaling->vUpscalingAllowed = pPossibleScaling->vUpscalingAllowed;
    }

    return TRUE;
}

static NvBool FlipTimeStampValidForChannel(
    const NVEvoChannel *pChannel,
    NvU64 timeStamp)
{
    if (pChannel->caps.validTimeStampBits < 64) {
        const NvU64 validTimeStampMask =
            NVBIT64(pChannel->caps.validTimeStampBits) - 1;
        if ((timeStamp & ~validTimeStampMask) != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

static NvBool ValidatePerLayerCompParams(
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

static NvBool UpdateLayerFlipEvoHwStateCommon(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NvU32 layer,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    const NVEvoChannel *pChannel = pDevEvo->head[head].layer[layer];
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[layer];
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);
    NvBool ret;

    if (pParams->layer[layer].surface.specified) {
        ret = AssignSurfaceArray(pDevEvo,
                                 pOpenDevSurfaceHandles,
                                 pParams->layer[layer].surface.handle,
                                 pChannel->channelMask,
                                 pHwState->pSurfaceEvo);
        if (!ret) {
            return FALSE;
        }

        /*
         * Verify the (rotation, reflectionX, reflectionY) is a
         * combination currently supported.
         */
        if ((NVBIT(NvKmsRRParamsToCapBit(&pParams->layer[layer].surface.rrParams)) &
             pDevEvo->caps.validLayerRRTransforms) == 0) {
            return FALSE;
        }
        pHwState->rrParams = pParams->layer[layer].surface.rrParams;

    }

    /* Verify the timestamp is in the valid range for this channel. */
    if (!FlipTimeStampValidForChannel(pChannel,
                                      pParams->layer[layer].timeStamp)) {
        return FALSE;
    }
    pHwState->timeStamp = pParams->layer[layer].timeStamp;

    /*!
     * The NVKMS_SYNCPT_TYPE* types are handled earlier in the flip path (in
     * the function nvHandleSyncptRegistration)
     */
    if (pParams->layer[layer].syncObjects.specified &&
        !pParams->layer[layer].syncObjects.val.useSyncpt) {

        if (pParams->layer[layer].syncObjects.val.u.semaphores.acquire.surface.surfaceHandle != 0 ||
            pParams->layer[layer].syncObjects.val.u.semaphores.release.surface.surfaceHandle != 0) {
            if (pParams->layer[layer].skipPendingFlips) {
                return FALSE;
            }
        }

        ret = AssignSemaphoreEvoHwState(pDevEvo,
                                        pOpenDevSurfaceHandles,
                                        pChannel,
                                        sd,
                                        &pParams->layer[layer].syncObjects.val,
                                        &pHwState->syncObject);
        if (!ret) {
            return FALSE;
        }
    }

    if (pHwState->pSurfaceEvo[NVKMS_LEFT]) {
        pHwState->minPresentInterval =
            pParams->layer[layer].minPresentInterval;
    } else {
        /* The hardware requires that MPI be 0 when disabled. */
        pHwState->minPresentInterval = 0;
    }

    if (pParams->layer[layer].sizeIn.specified) {
        pHwState->sizeIn = pParams->layer[layer].sizeIn.val;
    }

    if (pParams->layer[layer].sizeOut.specified) {
        pHwState->sizeOut = pParams->layer[layer].sizeOut.val;
    }

    /*
     * If supportsWindowMode = TRUE, the sizeIn/sizeOut dimensions can be
     * any arbitrary (valid) values.
     *
     * If supportsWindowMode = FALSE (legacy EVO main layer), the sizeIn
     * /sizeOut dimensions must match the size of the surface for that layer.
     *
     * Note that if sizeIn/Out dimensions are invalid i.e. with a width or
     * height of zero, this will be rejected by a call to
     * ValidateFlipChannelEvoHwState() later in the code path.
     *
     * Note that if scaling is unsupported, i.e. that sizeIn cannot differ
     * from sizeOut, then any unsupported configurations will be caught by the
     * ComputeWindowScalingTaps() call later on in this function.
     */
    if (!pDevEvo->caps.layerCaps[layer].supportsWindowMode &&
        (pHwState->pSurfaceEvo[NVKMS_LEFT] != NULL)) {
        const NVSurfaceEvoRec *pSurfaceEvo =
            pHwState->pSurfaceEvo[NVKMS_LEFT];

        if ((pHwState->sizeIn.width != pSurfaceEvo->widthInPixels) ||
            (pHwState->sizeIn.height != pSurfaceEvo->heightInPixels)) {
            return FALSE;
        }

        if ((pHwState->sizeOut.width != pSurfaceEvo->widthInPixels) ||
            (pHwState->sizeOut.height != pSurfaceEvo->heightInPixels)) {
            return FALSE;
        }
    }

    /*
     * Allow the client to specify non-origin outputPosition only if the
     * layer supports window mode.
     *
     * If window mode is unsupported but the client specifies non-origin
     * outputPosition, return FALSE.
     */
    if (pDevEvo->caps.layerCaps[layer].supportsWindowMode) {
        if (pParams->layer[layer].outputPosition.specified) {
            const NvS16 x = pParams->layer[layer].outputPosition.val.x;
            const NvS16 y = pParams->layer[layer].outputPosition.val.y;
            if ((pHwState->outputPosition.x != x) ||
                (pHwState->outputPosition.y != y)) {
                pHwState->outputPosition.x = x;
                pHwState->outputPosition.y = y;
                pFlipState->dirty.layerPosition[layer] = TRUE;
            }
        }
    } else if (pParams->layer[layer].outputPosition.specified &&
               ((pParams->layer[layer].outputPosition.val.x != 0) ||
                (pParams->layer[layer].outputPosition.val.y != 0))) {
        return FALSE;
    }

    if (pParams->layer[layer].compositionParams.specified) {
        pHwState->composition =
            pParams->layer[layer].compositionParams.val;
    }

    if (pHwState->composition.depth == 0) {
        pHwState->composition.depth =
            NVKMS_MAX_LAYERS_PER_HEAD - layer;
    }

    /* XXX Move ValidatePerLayerCompParams() call to nvValidateFlipEvoHwState() */
    if (!ValidatePerLayerCompParams(
            &pHwState->composition,
            &pDevEvo->caps.layerCaps[layer].composition,
            pHwState->pSurfaceEvo[NVKMS_LEFT])) {
        return FALSE;
    }

    if (!pDevEvo->hal->ComputeWindowScalingTaps(pDevEvo,
                                                pChannel,
                                                pHwState)) {
        return FALSE;
    }

    if (pParams->layer[layer].completionNotifier.specified) {
        ret = AssignCompletionNotifierEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pParams->layer[layer].completionNotifier.val,
                    pChannel,
                    &pFlipState->layer[layer].completionNotifier);
        if (!ret) {
            return FALSE;
        }
    }

    pFlipState->dirty.layer[layer] = TRUE;

    return TRUE;
}

static NvBool UpdateMainLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState,
    NvBool allowVrr)
{
    const NVEvoChannel *pChannel =
        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[NVKMS_MAIN_LAYER];

    if (!IsLayerDirty(pParams, NVKMS_MAIN_LAYER)) {
        return TRUE;
    }

    if (!UpdateLayerFlipEvoHwStateCommon(pOpenDev, pDevEvo, sd, head,
                                         NVKMS_MAIN_LAYER,
                                         pParams, pFlipState)) {
        return FALSE;
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].csc.specified) {
        if (pParams->layer[NVKMS_MAIN_LAYER].csc.useMain) {
            return FALSE;
        } else {
            pHwState->cscMatrix = pParams->layer[NVKMS_MAIN_LAYER].csc.matrix;
        }
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].surface.specified) {
        if (pParams->layer[NVKMS_MAIN_LAYER].perEyeStereoFlip &&
            !pChannel->caps.perEyeStereoFlips) {
            return FALSE;
        }

        pHwState->perEyeStereoFlip =
            pParams->layer[NVKMS_MAIN_LAYER].perEyeStereoFlip;
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].tearing && !pChannel->caps.tearingFlips) {
        return FALSE;
    }

    // EVO will throw an invalid argument exception if
    // minPresentInterval is too large, or if tearing is enabled and
    // it's not zero.
    if (pParams->layer[NVKMS_MAIN_LAYER].minPresentInterval > NV_MAX_SWAP_INTERVAL ||
        (pParams->layer[NVKMS_MAIN_LAYER].tearing &&
         pParams->layer[NVKMS_MAIN_LAYER].minPresentInterval != 0)) {
        return FALSE;
    }

    pHwState->tearing = pParams->layer[NVKMS_MAIN_LAYER].tearing;

    if (!ApplyBaseFlipOverrides(pDevEvo,
                                sd, head, &pFlipState->layer[NVKMS_MAIN_LAYER],
                                allowVrr)) {
        return FALSE;
    }

    return TRUE;
}

static NvBool UpdateCursorLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);

    if (pParams->cursor.imageSpecified) {
        NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES] = { };

        if (!nvGetCursorImageSurfaces(pDevEvo,
                pOpenDevSurfaceHandles,
                &pParams->cursor.image,
                pSurfaceEvos)) {
            return FALSE;
        }

        /* XXX NVKMS TODO: add support for stereo cursor */
        if (pSurfaceEvos[NVKMS_RIGHT] != NULL) {
            return FALSE;
        }

        pFlipState->cursor.pSurfaceEvo = pSurfaceEvos[NVKMS_LEFT];

        if (pFlipState->cursor.pSurfaceEvo != NULL) {
            if (!ValidatePerLayerCompParams(&pParams->cursor.image.cursorCompParams,
                                            &pDevEvo->caps.cursorCompositionCaps,
                                            pFlipState->cursor.pSurfaceEvo)) {
                return FALSE;
            }

            pFlipState->cursor.cursorCompParams =
                pParams->cursor.image.cursorCompParams;
        }

        pFlipState->dirty.cursorSurface = TRUE;
    }

    if (pParams->cursor.positionSpecified) {
        pFlipState->cursor.x = pParams->cursor.position.x;
        pFlipState->cursor.y = pParams->cursor.position.y;

        pFlipState->dirty.cursorPosition = TRUE;
    }

    return TRUE;
}

static NvBool UpdateOverlayLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NvU32 layer,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[layer];

    nvAssert(layer != NVKMS_MAIN_LAYER);

    if (!IsLayerDirty(pParams, layer)) {
        return TRUE;
    }

    if (pParams->layer[layer].skipPendingFlips ||
        pParams->layer[layer].perEyeStereoFlip) {
        return FALSE;
    }

    if (!UpdateLayerFlipEvoHwStateCommon(pOpenDev, pDevEvo, sd, head, layer,
                                         pParams, pFlipState)) {
        return FALSE;
    }

    if (pParams->layer[layer].csc.specified) {
        if (pParams->layer[layer].csc.useMain) {
            if (pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT]) {
                pHwState->cscMatrix =
                    pFlipState->layer[NVKMS_MAIN_LAYER].cscMatrix;
            }
        } else {
            pHwState->cscMatrix = pParams->layer[layer].csc.matrix;
        }
    }

    return TRUE;
}

/*!
 * Update the NVFlipEvoHwState, using NvKmsFlipCommonParams.
 *
 * Propagate the requested configuration from NvKmsFlipCommonParams to
 * NVFlipEvoHwState, performing steps such as translating from
 * NvKmsSurfaceHandle to NVSurfaceEvoRecs.  Validate the NvKmsFlipCommonParams
 * parameters, but defer more general validation of the resulting
 * NVFlipEvoHwState until nvValidateFlipEvoHwState(), which callers must call
 * separately.
 *
 * The NVFlipEvoHwState should first be initialized by calling
 * nvInitFlipEvoHwState().
 *
 * No NVKMS hardware or software state should be altered here, because
 * this function is used before we have decided to commit the proposed
 * NVFlipEvoHwState to hardware.
 *
 * \param[in]  pOpenDev       The pOpenDev of the client doing the flip.
 * \param[in]  pDevEvo        The device on which the surface image will be set.
 * \param[in]  sd             The subdevice for the flip, as specified by the
 *                            client.
 * \param[in]  head           The head for the flip, as specified by the client.
 * \param[in]  pParams        The requested flip, NvKmsFlipCommonParams.
 * \param[in,out] pFlipState  The resulting NVFlipEvoHwState.
 * \param[in]  allowVrr       Whether VRR flipping should be allowed.
 * \param[in]  pPossibleUsage Possible usage.
 *
 * \return  If pFlipState could be updated, return TRUE.
 *          Otherwise, return FALSE.
 */
NvBool nvUpdateFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState,
    NvBool allowVrr,
    const struct NvKmsUsageBounds *pPossibleUsage)
{
    NvU32 layer;

    if (!CheckFlipPermissions(pOpenDev, pDevEvo, sd, head, pParams)) {
        return FALSE;
    }

    if (pParams->viewPortIn.specified) {
        pFlipState->dirty.viewPortPointIn = TRUE;
        pFlipState->viewPortPointIn = pParams->viewPortIn.point;
    }

    if (!UpdateCursorLayerFlipEvoHwState(pOpenDev, pDevEvo, pParams,
                                         pFlipState)) {
        return FALSE;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (layer == NVKMS_MAIN_LAYER) {
            if (!UpdateMainLayerFlipEvoHwState(pOpenDev, pDevEvo, sd, head,
                                               pParams, pFlipState, allowVrr)) {
                return FALSE;
            }
            continue;
        }

        if (!UpdateOverlayLayerFlipEvoHwState(pOpenDev, pDevEvo, sd, head,
                                              layer, pParams, pFlipState)) {
            return FALSE;
        }
    }

    if (!AssignUsageBounds(pDevEvo, head, pFlipState)) {
        return FALSE;
    }

    if (!OverrideUsageBounds(pDevEvo, pFlipState, pParams, sd, head,
                             pPossibleUsage)) {
        return FALSE;
    }


    /*
     * If there is active cursor/cropped-window(overlay) without full screen
     * window(base/core) then NVKMS is supposed to disable MidFrame/DWCF
     * watermark.
     */

    pFlipState->disableMidFrameAndDWCFWatermark = FALSE;

    if (NV5070_CTRL_SYSTEM_GET_CAP(
            pDevEvo->capsBits,
            NV5070_CTRL_SYSTEM_CAPS_BUG_2052012_GLITCHY_MCLK_SWITCH) &&
        !pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT]) {

        if (pFlipState->cursor.pSurfaceEvo != NULL) {
            pFlipState->disableMidFrameAndDWCFWatermark = TRUE;
        } else {
            NvU32 layer;

            /*
             * XXX TODO: Check the output size of the overlay in order
             * to determine if it will be fullscreen or not.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (layer != NVKMS_MAIN_LAYER &&
                    pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL) {
                    pFlipState->disableMidFrameAndDWCFWatermark = TRUE;
                    break;
                }
            }
        }
    }

    return TRUE;
}

/*
 * Checks that if the surface is NULL (i.e. no image will be shown), various
 * other elements must be NULL as well. If the surface is not NULL, verifies
 * that the sizeIn/Out have nonzero values.
 */
inline static NvBool ValidateFlipChannelEvoHwState(
    const NVFlipChannelEvoHwState *pState)
{
    if (pState->pSurfaceEvo[NVKMS_LEFT] != NULL) {
        /* Verify sizes are valid. */
        if ((pState->sizeIn.width == 0) || (pState->sizeIn.height == 0) ||
            (pState->sizeOut.width == 0) || (pState->sizeOut.height == 0)) {
            return FALSE;
        }

        return TRUE;
    }

    if (pState->completionNotifier.surface.pSurfaceEvo != NULL) {
        return FALSE;
    }

    if (!pState->syncObject.usingSyncpt) {
        if (pState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo != NULL) {
            return FALSE;
        }

        if (pState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo != NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

static NvBool ValidateSurfaceSize(
    const NVDevEvoRec *pDevEvo,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const struct NvKmsRect *sourceFetchRect)
{
    NvU8 planeIndex;

    if ((pSurfaceEvo->widthInPixels > pDevEvo->caps.maxWidthInPixels) ||
        (pSurfaceEvo->heightInPixels > pDevEvo->caps.maxHeight)) {
        return FALSE;
    }

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {

        NvU64 planePitch = pSurfaceEvo->planes[planeIndex].pitch;

        /*
         * Convert planePitch to units of bytes if it's currently specified in
         * units of blocks. Each block is 64-bytes wide.
         */
        if (pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
            planePitch <<= NVKMS_BLOCK_LINEAR_LOG_GOB_WIDTH;
        }

        if (planePitch > pDevEvo->caps.maxWidthInBytes) {
            return FALSE;
        }
    }

    if (!pDevEvo->hal->ValidateWindowFormat(pSurfaceEvo->format,
                                            sourceFetchRect,
                                            NULL)) {
        return FALSE;
    }

    return TRUE;
}

static NvBool
ValidateMainFlipChannelEvoHwState(const NVDevEvoRec *pDevEvo,
                                  const NVFlipChannelEvoHwState *pHwState,
                                  const NVHwModeTimingsEvo *pTimings,
                                  struct NvKmsPoint viewPortPointIn)
{
    NvU32 eye;
    const NVSurfaceEvoRec *pFirstSurfaceEvo = NULL;

    /*
     * This struct represents the source fetch rectangle for a given surface,
     * and will be populated later as such. This function doesn't explicitly set
     * sourceFetchRect.{x,y} because NVKMS currently doesn't support programming
     * source fetch offsets, so the init value of 0 should be fine for both of
     * these fields.
     */
    struct NvKmsRect sourceFetchRect = {0};

    if (!ValidateFlipChannelEvoHwState(pHwState)) {
        return FALSE;
    }

    for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {
        const NVSurfaceEvoRec *pSurfaceEvo = pHwState->pSurfaceEvo[eye];

        if (pSurfaceEvo == NULL) {
            continue;
        }

        if (pFirstSurfaceEvo == NULL) {
            pFirstSurfaceEvo = pSurfaceEvo;
        } else if (pSurfaceEvo->widthInPixels !=
                   pFirstSurfaceEvo->widthInPixels ||
                   pSurfaceEvo->heightInPixels !=
                   pFirstSurfaceEvo->heightInPixels) {
            return FALSE;
        }

        sourceFetchRect.width = pHwState->sizeIn.width;
        sourceFetchRect.height = pHwState->sizeIn.height;

        if (!ValidateSurfaceSize(pDevEvo, pSurfaceEvo, &sourceFetchRect)) {
            return FALSE;
        }

        /* The use of A_plus_B_greater_than_C_U16 is only valid if these
         * fit in a U16 */
        nvAssert(pSurfaceEvo->widthInPixels <= NV_U16_MAX);
        nvAssert(pSurfaceEvo->heightInPixels <= NV_U16_MAX);
        /* And the checks above in ValidateSurfaceSize should have
         * guaranteed that. */
        nvAssert(pDevEvo->caps.maxWidthInPixels <= NV_U16_MAX);
        nvAssert(pDevEvo->caps.maxHeight <= NV_U16_MAX);

        /*
         * Validate that the requested viewport parameters fit within the
         * specified surface, unless the main layer is allowed to be smaller
         * than the viewport.
         */
        if (!pDevEvo->caps.layerCaps[NVKMS_MAIN_LAYER].supportsWindowMode) {
            if (A_plus_B_greater_than_C_U16(viewPortPointIn.x,
                                            pTimings->viewPort.in.width,
                                            pSurfaceEvo->widthInPixels)) {
                return FALSE;
            }

            if (A_plus_B_greater_than_C_U16(viewPortPointIn.y,
                                            pTimings->viewPort.in.height,
                                            pSurfaceEvo->heightInPixels)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static NvBool
ValidateOverlayFlipChannelEvoHwState(const NVDevEvoRec *pDevEvo,
                                     const NvU32 head,
                                     const NvU32 layer,
                                     const NVFlipChannelEvoHwState *pHwState)
{
    const NVSurfaceEvoRec *pSurfaceEvo = pHwState->pSurfaceEvo[NVKMS_LEFT];

    /*
     * This struct represents the source fetch rectangle for a given surface,
     * and will be populated later as such. This function doesn't explicitly set
     * sourceFetchRect.{x,y} because NVKMS currently doesn't support programming
     * source fetch offsets, so the init value of 0 should be fine for both of
     * these fields.
     */
    struct NvKmsRect sourceFetchRect = {0};

    nvAssert(layer != NVKMS_MAIN_LAYER);

    if (!ValidateFlipChannelEvoHwState(pHwState)) {
        return FALSE;
    }

    if (pSurfaceEvo == NULL) {
        return TRUE;
    }

    sourceFetchRect.width = pHwState->sizeIn.width;
    sourceFetchRect.height = pHwState->sizeIn.height;

    if (!ValidateSurfaceSize(pDevEvo, pSurfaceEvo, &sourceFetchRect)) {
        return FALSE;
    }

    /* Validate input size against surface size. */
    if (pHwState->sizeIn.width > pSurfaceEvo->widthInPixels ||
        pHwState->sizeIn.height > pSurfaceEvo->heightInPixels) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Perform validation of the the given NVFlipEvoHwState.
 */
NvBool nvValidateFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVHwModeTimingsEvo *pTimings,
    const NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {

        if (pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL) {
            NvU32 tmpLayer;

            /* Depth should be different for each of the layers owned by the head */
            for (tmpLayer = 0; tmpLayer < pDevEvo->head[head].numLayers; tmpLayer++) {
                if (pFlipState->layer[tmpLayer].pSurfaceEvo[NVKMS_LEFT] == NULL) {
                    continue;
                }

                if ((tmpLayer != layer) &&
                    (pFlipState->layer[tmpLayer].composition.depth ==
                        pFlipState->layer[layer].composition.depth)) {
                    return FALSE;
                }
            }

            /* Depth of the main layer should be the greatest one */
            if ((pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT] != NULL) &&
                (pFlipState->layer[layer].composition.depth >
                    pFlipState->layer[NVKMS_MAIN_LAYER].composition.depth)) {
                return FALSE;
            }
        }

        if (layer == NVKMS_MAIN_LAYER) {
            if (!ValidateMainFlipChannelEvoHwState(pDevEvo,
                                                   &pFlipState->layer[layer],
                                                   pTimings,
                                                   pFlipState->viewPortPointIn)) {
                return FALSE;
            }
            continue;
        }

        if (pFlipState->dirty.layer[layer] &&
            !ValidateOverlayFlipChannelEvoHwState(pDevEvo,
                                                  head,
                                                  layer,
                                                  &pFlipState->layer[layer])) {
            return FALSE;
        }
    }

    /* XXX NVKMS TODO: validate cursor x,y against current viewport in? */

    return ValidateUsageBounds(pDevEvo,
                               head,
                               &pFlipState->usage,
                               &pTimings->viewPort.possibleUsage);
}


/*!
 * Validate overlay should be enabled only with valid core scanout surface.
 */
static NvBool ValidatePerDispState(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsFlipWorkArea *pWorkArea)
{
    const NVDispEvoRec *pDispEvo;
    NvU32 sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVEvoSubDevHeadStateRec *pSdHeadState =
                &pDevEvo->gpus[pDispEvo->displayOwner].headState[head];
            const NVSurfaceEvoRec *pMainScanoutSurface =
                pSdHeadState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            const NVFlipEvoHwState *pFlipState =
                &pWorkArea->sd[sd].head[head].newState;
            NvU32 layer;

            if (pFlipState->dirty.layer[NVKMS_MAIN_LAYER]) {
                pMainScanoutSurface =
                    pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            }

            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (layer == NVKMS_MAIN_LAYER) {
                    continue;
                }

                if (pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL &&
                    pMainScanoutSurface == NULL) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*
 * Record in the updateState that the given channel needs interlocked
 * window immediate updates.
 */
static void UpdateWinImmInterlockState(NVDevEvoPtr pDevEvo,
                                       NVEvoUpdateState *updateState,
                                       const NVEvoChannel *pChannel)
{
    const NvU32 subDevMask = nvPeekEvoSubDevMask(pDevEvo);
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (subDevMask & (1 << sd)) {
            updateState->subdev[sd].winImmInterlockMask |=
                pChannel->channelMask;
        }
    }
}

/*!
 * Record in the updateState that the given channel's method are eligible for
 * flip locking.
 */
static void UpdateUpdateFlipLockState(NVDevEvoPtr pDevEvo,
                                      NVEvoUpdateState *updateState,
                                      const NVEvoChannel *pChannel)
{
    const NvU32 subDevMask = nvPeekEvoSubDevMask(pDevEvo);
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (subDevMask & (1 << sd)) {
            updateState->subdev[sd].flipLockQualifyingMask |=
                pChannel->channelMask;
        }
    }
}

/*!
 * Program a flip on all requested layers on the specified head.
 *
 * This also updates pDispEvo->headState[head], caching what was programmed.

 * \param[in,out] pDispEvo    The disp on which the flip should be performed.
 * \param[in]     head        The head on which the flip should be performed.
 * \param[in]     pFlipState  The description of how to update each layer.
 * \param[in,out] updateState Indicates which channels require UPDATEs
 */
void nvFlipEvoOneHead(
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NVFlipEvoHwState *pFlipState,
    NvBool allowFlipLock,
    NVEvoUpdateState *updateState)
{
    const NvU32 subDeviceMask = NVBIT(sd);
    const NVDispHeadStateEvoRec *pHeadState =
        &pDevEvo->gpus[sd].pDispEvo->headState[head];
    NvBool bypassComposition = pHeadState->bypassComposition;
    NVEvoSubDevHeadStateRec *pSdHeadState =
        &pDevEvo->gpus[sd].headState[head];
    NvU32 layer;

    /*
     * Provide the pre-update hardware state (in pSdHeadState) and the new
     * target state (pFlipState) to the HAL implementation so that it has the
     * information it needs to implement the workaround for hardware bug
     * 2193096, which requires special logic on transitions between NULL and
     * non-NULL ctxdmas (and vice versa).
     */
    pDevEvo->hal->FlipTransitionWAR(pDevEvo, sd, head,
                                    pSdHeadState, pFlipState,
                                    updateState);

    /*
     * Promote the software state first, such that the hardware programming
     * paths below see the new state atomically.
     */
    if (pFlipState->dirty.viewPortPointIn) {
        pSdHeadState->viewPortPointIn = pFlipState->viewPortPointIn;
    }

    if (pFlipState->dirty.cursorSurface || pFlipState->dirty.cursorPosition) {
        pSdHeadState->cursor = pFlipState->cursor;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->dirty.layer[layer]) {
            pSdHeadState->layer[layer] = pFlipState->layer[layer];
        }
    }

    if (pFlipState->dirty.viewPortPointIn) {
        nvSetViewPortPointInEvo(pDevEvo->gpus[sd].pDispEvo,
                                head,
                                pFlipState->viewPortPointIn.x,
                                pFlipState->viewPortPointIn.y,
                                updateState);
    }

    if (pFlipState->dirty.cursorSurface) {
        nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
        pDevEvo->hal->SetCursorImage(pDevEvo,
                                     head,
                                     pSdHeadState->cursor.pSurfaceEvo,
                                     updateState,
                                     &pSdHeadState->cursor.cursorCompParams);
        nvPopEvoSubDevMask(pDevEvo);
    }

    if (pFlipState->dirty.cursorPosition) {
        nvEvoMoveCursorInternal(pDevEvo->gpus[sd].pDispEvo,
                                head,
                                pFlipState->cursor.x,
                                pFlipState->cursor.y);
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (!pFlipState->dirty.layer[layer]) {
            continue;
        }

        if (pFlipState->dirty.layerPosition[layer]) {
            /* Ensure position updates are supported on this layer. */
            nvAssert(pDevEvo->caps.layerCaps[layer].supportsWindowMode);

            pDevEvo->hal->SetImmPointOut(pDevEvo,
                                         pDevEvo->head[head].layer[layer],
                                         sd,
                                         updateState,
                                         pFlipState->layer[layer].outputPosition.x,
                                         pFlipState->layer[layer].outputPosition.y);

            if (pDevEvo->hal->caps.supportsSynchronizedOverlayPositionUpdate) {
                UpdateWinImmInterlockState(pDevEvo, updateState,
                                           pDevEvo->head[head].layer[layer]);
            }
        }

        nvPushEvoSubDevMask(pDevEvo, subDeviceMask);
        pDevEvo->hal->Flip(pDevEvo,
                           pDevEvo->head[head].layer[layer],
                           &pFlipState->layer[layer],
                           updateState,
                           bypassComposition);
        if (layer == NVKMS_MAIN_LAYER && allowFlipLock) {
            UpdateUpdateFlipLockState(pDevEvo, updateState,
                                      pDevEvo->head[head].layer[layer]);
        }
        nvPopEvoSubDevMask(pDevEvo);
    }

    pSdHeadState->targetUsage = pFlipState->usage;

    pSdHeadState->targetDisableMidFrameAndDWCFWatermark =
        pFlipState->disableMidFrameAndDWCFWatermark;
}

static void ChangeSurfaceFlipRefCount(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo,
    NvBool increase)
{
    if (pSurfaceEvo != NULL) {
        if (increase) {
            nvEvoIncrementSurfaceRefCnts(pSurfaceEvo);
        } else {
            nvEvoDecrementSurfaceRefCnts(pSurfaceEvo);
        }
    }
}

void nvUpdateSurfacesFlipRefCount(
    NVDevEvoPtr pDevEvo,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState,
    NvBool increase)
{
    NvU32 i;

    ChangeSurfaceFlipRefCount(
        pDevEvo,
        pFlipState->cursor.pSurfaceEvo,
        increase);

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        NVFlipChannelEvoHwState *pLayerFlipState = &pFlipState->layer[i];

        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->pSurfaceEvo[NVKMS_LEFT],
            increase);
        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->pSurfaceEvo[NVKMS_RIGHT],
            increase);
        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->completionNotifier.surface.pSurfaceEvo,
            increase);

        if (!pLayerFlipState->syncObject.usingSyncpt) {
            ChangeSurfaceFlipRefCount(
                pDevEvo,
                pLayerFlipState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo,
                increase);
            ChangeSurfaceFlipRefCount(
                pDevEvo,
                pLayerFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo,
                increase);
        }
    }
}

static void UnionScalingUsageBounds(
    const struct NvKmsScalingUsageBounds *a,
    const struct NvKmsScalingUsageBounds *b,
    struct NvKmsScalingUsageBounds *ret)
{
    ret->maxVDownscaleFactor = NV_MAX(a->maxVDownscaleFactor,
                                      b->maxVDownscaleFactor);
    ret->maxHDownscaleFactor = NV_MAX(a->maxHDownscaleFactor,
                                      b->maxHDownscaleFactor);
    ret->vTaps = NV_MAX(a->vTaps, b->vTaps);
    ret->vUpscalingAllowed = a->vUpscalingAllowed || b->vUpscalingAllowed;
}

struct NvKmsUsageBounds nvUnionUsageBounds(
    const struct NvKmsUsageBounds *a,
    const struct NvKmsUsageBounds *b)
{
    struct NvKmsUsageBounds ret;
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(a->layer); i++) {
        nvAssert(a->layer[i].usable ==
                 !!a->layer[i].supportedSurfaceMemoryFormats);
        nvAssert(b->layer[i].usable ==
                 !!b->layer[i].supportedSurfaceMemoryFormats);

        ret.layer[i].usable = a->layer[i].usable || b->layer[i].usable;

        ret.layer[i].supportedSurfaceMemoryFormats =
            a->layer[i].supportedSurfaceMemoryFormats |
            b->layer[i].supportedSurfaceMemoryFormats;

        UnionScalingUsageBounds(&a->layer[i].scaling,
                                &b->layer[i].scaling,
                                &ret.layer[i].scaling);
    }

    return ret;
}

NvBool UsageBoundsEqual(
    const struct NvKmsUsageBounds *a,
    const struct NvKmsUsageBounds *b)
{
    NvU32 layer;

    for (layer = 0; layer < ARRAY_LEN(a->layer); layer++) {
        if (!nvEvoLayerUsageBoundsEqual(a, b, layer)) {
            return FALSE;
        }
    }

    return TRUE;
}

static NvBool AllocatePreFlipBandwidth(NVDevEvoPtr pDevEvo,
                                       struct NvKmsFlipWorkArea *pWorkArea)
{
    NVValidateImpOneDispHeadParamsRec *timingsParams = NULL;
    struct NvKmsUsageBounds *currentAndNew = NULL;
    struct NvKmsUsageBounds *guaranteedAndCurrent = NULL;
    NVDispEvoPtr pDispEvo;
    NvU32 head;
    NvBool recheckIMP = FALSE;
    NvBool ret = TRUE;

    if (!pDevEvo->isSOCDisplay) {
        return TRUE;
    }

    timingsParams =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*timingsParams));
    if (timingsParams == NULL) {
        return FALSE;
    }

    currentAndNew =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*currentAndNew));
    if (currentAndNew == NULL) {
        nvFree(timingsParams);
        return FALSE;
    }

    guaranteedAndCurrent =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*guaranteedAndCurrent));
    if (guaranteedAndCurrent == NULL) {
        nvFree(timingsParams);
        nvFree(currentAndNew);
        return FALSE;
    }

    pDispEvo = pDevEvo->pDispEvo[0];

    // SOC Display never has more than one disp
    nvAssert(pDevEvo->nDispEvo == 1);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        const struct NvKmsUsageBounds *pCurrent =
            &pDevEvo->gpus[0].headState[head].preallocatedUsage;
        const struct NvKmsUsageBounds *pNew =
            &pWorkArea->sd[0].head[head].newState.usage;

        if (pHeadState->activeRmId == 0) {
            continue;
        }

        timingsParams[head].pConnectorEvo = pHeadState->pConnectorEvo;
        timingsParams[head].activeRmId = pHeadState->activeRmId;
        timingsParams[head].pTimings = &pHeadState->timings;

        currentAndNew[head] = nvUnionUsageBounds(pCurrent, pNew);
        guaranteedAndCurrent[head] = nvUnionUsageBounds(
            &pHeadState->timings.viewPort.guaranteedUsage,
            pCurrent);

        if (!ValidateUsageBounds(pDevEvo,
                                 head,
                                 pNew,
                                 &guaranteedAndCurrent[head])) {
            recheckIMP = TRUE;
        }

        guaranteedAndCurrent[head] =
            nvUnionUsageBounds(&guaranteedAndCurrent[head], pNew);
        timingsParams[head].pUsage = &guaranteedAndCurrent[head];
    }

    if (recheckIMP) {
        ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                                   FALSE /* requireBootClocks */,
                                   NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE,
                                   NULL /* pMinIsoBandwidthKBPS */,
                                   NULL /* pMinDramFloorKBPS */);
        if (ret) {
            for (head = 0; head < pDevEvo->numHeads; head++) {
                pDevEvo->gpus[0].headState[head].preallocatedUsage =
                    currentAndNew[head];
            }
        }
    }

    nvFree(timingsParams);
    nvFree(currentAndNew);
    nvFree(guaranteedAndCurrent);

    if (ret) {
        nvScheduleLowerDispBandwidthTimer(pDevEvo);
    }

    return ret;
}

/*!
 * If the satellite channel is active then pre-NVDisplay hardware does not allow
 * to change its usage bounds in non-interlock update. The nvSetUsageBoundsEvo()
 * code path for pre-NVDisplay hardware, interlocks the satellite channels with
 * the usage bounds update. This makes it essential to poll for
 * NO_METHOD_PENDING state of the satellite channels, otherwise blocking
 * pre-flip IMP update will also get stuck.
 *
 * It is not possible to interlock flip-locked satellite channels with the core
 * channel usage bounds update; in that case, reject the flip.  Do not allow
 * client to make any change in surface usage bounds parameters without
 * deactivating channel first, if channel is flip-locked.
 */
static NvBool PrepareToDoPreFlipIMP(NVDevEvoPtr pDevEvo,
                                    struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU64 startTime = 0;
    NvU32 timeout = 2000000; /* 2 seconds */
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVEvoHeadControlPtr pHC =
                &pEvoSubDev->headControl[head];
            const NVEvoSubDevHeadStateRec *pCurrentFlipState =
                &pDevEvo->gpus[sd].headState[head];
            const NVSurfaceEvoRec *pCurrentBaseSurf =
                pCurrentFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            const struct NvKmsUsageBounds *pCurrentUsage =
                &pCurrentFlipState->usage;

            NVFlipEvoHwState *pNewFlipState =
                &pWorkArea->sd[sd].head[head].newState;
            const NVSurfaceEvoRec *pNewBaseSurf =
                pNewFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            struct NvKmsUsageBounds *pNewUsage =
                &pNewFlipState->usage;

            struct NvKmsUsageBounds *pPreFlipUsage =
                &pWorkArea->sd[sd].head[head].preFlipUsage;

            NvU32 layer;

            *pPreFlipUsage = nvUnionUsageBounds(pNewUsage,
                                                pCurrentUsage);

            if (pDevEvo->hal->caps.supportsNonInterlockedUsageBoundsUpdate) {
                /*
                 * NVDisplay does not interlock the satellite channel
                 * with its usage bounds update.
                 */
                continue;
            }

            /*
             * If head is flip-locked then do not change usage
             * bounds while base channel is active.
             */
            if (pHC->flipLock &&
                 /* If the base channel is active before and after flip then
                  * current and new base usage bounds should be same. */
                ((pNewBaseSurf != NULL &&
                  pCurrentBaseSurf != NULL &&
                  !nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                              pNewUsage, NVKMS_MAIN_LAYER)) ||
                  /* If the base channel is active before flip then current and
                   * preflip base usage bounds should be same. */
                 (pCurrentBaseSurf != NULL &&
                  !nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                              pPreFlipUsage, NVKMS_MAIN_LAYER)))) {
                return FALSE;
            }

            /*
             * Poll for NO_METHOD_PENDING state if usage
             * bounds of the channel are changed.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (!nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                                pPreFlipUsage, layer) &&
                    !nvEvoPollForNoMethodPending(pDevEvo,
                                                 sd,
                                                 pDevEvo->head[head].layer[layer],
                                                 &startTime,
                                                 timeout)) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*!
 * Tasks need to perform before triggering flip, they all should be done here.
 *
 * If necessary, raise usage bounds and/or disable MidFrameAndDWCFWatermark
 * (bug 200508242) in the core channel and do an IMP update.
 *
 * Note that this function only raises usage bounds and/or disables
 * MidFrameAndDWCFWatermark, never lowers usage bounds and/or enables
 * MidFrameAndDWCFWatermark. This allows it to run before queuing a flip even
 * if there are still pending flips in a base channel.
 */
static void PreFlipIMP(NVDevEvoPtr pDevEvo,
                       const struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NVEvoUpdateState updateState = { };
        NvBool update = FALSE;

        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            const NVFlipEvoHwState *pNewState =
                &pWorkArea->sd[sd].head[head].newState;
            const struct NvKmsUsageBounds *pPreFlipUsage =
                &pWorkArea->sd[sd].head[head].preFlipUsage;
            struct NvKmsUsageBounds *pCurrentUsage =
                &pDevEvo->gpus[sd].headState[head].usage;

            if (!UsageBoundsEqual(pCurrentUsage, pPreFlipUsage)) {
                update |= nvSetUsageBoundsEvo(pDevEvo, sd, head,
                                              pPreFlipUsage, &updateState);
            }

            if (!pDevEvo->gpus[sd].
                headState[head].disableMidFrameAndDWCFWatermark &&
                pNewState->disableMidFrameAndDWCFWatermark) {

                nvEnableMidFrameAndDWCFWatermark(pDevEvo,
                                                 sd,
                                                 head,
                                                 FALSE /* enable */,
                                                 &updateState);
                update = TRUE;
            }
        }

        if (update) {
            nvDoIMPUpdateEvo(pDispEvo, &updateState);
        }
    }
}

static void LowerDispBandwidth(void *dataPtr, NvU32 dataU32)
{
    NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP];
    struct NvKmsUsageBounds *guaranteedAndCurrent;
    NVDevEvoPtr pDevEvo = dataPtr;
    NVDispEvoPtr pDispEvo;
    NvU32 head;
    NvBool ret;

    guaranteedAndCurrent =
        nvCalloc(1, sizeof(*guaranteedAndCurrent) * NVKMS_MAX_HEADS_PER_DISP);
    if (guaranteedAndCurrent == NULL) {
        nvAssert(guaranteedAndCurrent != NULL);
        return;
    }

    nvkms_memset(&timingsParams, 0, sizeof(timingsParams));

    pDispEvo = pDevEvo->pDispEvo[0];

    // SOC Display never has more than one disp
    nvAssert(pDevEvo->nDispEvo == 1);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        const struct NvKmsUsageBounds *pGuaranteed =
            &pHeadState->timings.viewPort.guaranteedUsage;
        const struct NvKmsUsageBounds *pCurrent =
            &pDevEvo->gpus[0].headState[head].usage;

        if (pHeadState->activeRmId == 0) {
            continue;
        }

        timingsParams[head].pConnectorEvo = pHeadState->pConnectorEvo;
        timingsParams[head].activeRmId = pHeadState->activeRmId;
        timingsParams[head].pTimings = &pHeadState->timings;

        guaranteedAndCurrent[head] = nvUnionUsageBounds(pGuaranteed, pCurrent);
        timingsParams[head].pUsage = &guaranteedAndCurrent[head];
    }

    ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                               FALSE /* requireBootClocks */,
                               NV_EVO_REALLOCATE_BANDWIDTH_MODE_POST,
                               NULL /* pMinIsoBandwidthKBPS */,
                               NULL /* pMinDramFloorKBPS */);
    if (ret) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            pDevEvo->gpus[0].headState[head].preallocatedUsage =
                pDevEvo->gpus[0].headState[head].usage;
        }
    }

    nvAssert(ret);

    nvFree(guaranteedAndCurrent);
}

void nvCancelLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo)
{
    nvkms_free_timer(pDevEvo->lowerDispBandwidthTimer);
    pDevEvo->lowerDispBandwidthTimer = NULL;
}

void nvScheduleLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo)
{
    nvAssert(pDevEvo->isSOCDisplay);

    nvCancelLowerDispBandwidthTimer(pDevEvo);

    pDevEvo->lowerDispBandwidthTimer =
        nvkms_alloc_timer(LowerDispBandwidth,
                          pDevEvo,
                          0, /* dataU32 */
                          30000000 /* 30 seconds */);
}

/*!
 * Check whether the core, base, and overlay channels are idle (i.e. no methods
 * pending in the corresponding pushbuffer) and lower the usage bounds if
 * possible.
 */
static NvBool TryLoweringUsageBoundsOneHead(NVDevEvoPtr pDevEvo, NvU32 sd,
                                            NvU32 head,
                                            NVEvoUpdateState *updateState)
{
    const NVEvoSubDevHeadStateRec *pHeadState =
        &pDevEvo->gpus[sd].headState[head];
    const struct NvKmsUsageBounds *pCurrent = &pHeadState->usage;
    const struct NvKmsUsageBounds *pTarget = &pHeadState->targetUsage;
    struct NvKmsUsageBounds newUsage = *pCurrent;
    NvBool changed = FALSE;
    NvBool scheduleLater = FALSE;
    int i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        if (pCurrent->layer[i].usable && !pTarget->layer[i].usable) {
            NvBool isMethodPending;

            if (pDevEvo->hal->IsChannelMethodPending(
                                        pDevEvo,
                                        pDevEvo->head[head].layer[i],
                                        sd,
                                        &isMethodPending) && !isMethodPending) {
                newUsage.layer[i] = pTarget->layer[i];
                changed = TRUE;
            } else {
                scheduleLater = TRUE;
            }
        } else if ((pCurrent->layer[i].usable && pTarget->layer[i].usable) &&
                   ((pCurrent->layer[i].supportedSurfaceMemoryFormats !=
                     pTarget->layer[i].supportedSurfaceMemoryFormats) ||
                    (!nvEvoScalingUsageBoundsEqual(&pCurrent->layer[i].scaling,
                                                   &pTarget->layer[i].scaling)))) {
            NvBool isMethodPending;

            if (pDevEvo->hal->IsChannelMethodPending(
                                        pDevEvo,
                                        pDevEvo->head[head].layer[i],
                                        sd,
                                        &isMethodPending) && !isMethodPending) {
                newUsage.layer[i] = pTarget->layer[i];
                changed = TRUE;
            } else {
                scheduleLater = TRUE;
            }
        }
    }

    if (scheduleLater) {
        SchedulePostFlipIMPTimer(pDevEvo);
    }

    if (changed) {
        changed = nvSetUsageBoundsEvo(pDevEvo, sd, head, &newUsage,
                                      updateState);
    }

    return changed;
}

static NvBool
TryEnablingMidFrameAndDWCFWatermarkOneHead(NVDevEvoPtr pDevEvo,
                                           NvU32 sd,
                                           NvU32 head,
                                           NVEvoUpdateState *updateState)
{
    const NVEvoSubDevHeadStateRec *pHeadState =
        &pDevEvo->gpus[sd].headState[head];
    NvBool changed = FALSE;

    if (pHeadState->disableMidFrameAndDWCFWatermark &&
        !pHeadState->targetDisableMidFrameAndDWCFWatermark) {

        NvBool isIdle;

        if (pDevEvo->hal->IsChannelIdle(pDevEvo,
                                        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                        sd,
                                        &isIdle) && isIdle) {
            nvEnableMidFrameAndDWCFWatermark(pDevEvo,
                                             sd,
                                             head,
                                             TRUE /* enable */,
                                             updateState);
            changed = TRUE;
        } else {
            // Schedule another timer to try again later.
            SchedulePostFlipIMPTimer(pDevEvo);
        }
    }

    return changed;
}

static void
TryToDoPostFlipIMP(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    pDevEvo->postFlipIMPTimer = NULL;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NVEvoUpdateState updateState = { };
        NvBool update = FALSE;

        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            if (TryLoweringUsageBoundsOneHead(pDevEvo, sd, head,
                                              &updateState)) {
                update = TRUE;
            }

            if (TryEnablingMidFrameAndDWCFWatermarkOneHead(
                    pDevEvo,
                    sd,
                    head,
                    &updateState)) {
                update = TRUE;
            }
        }

        if (update) {
            nvDoIMPUpdateEvo(pDispEvo, &updateState);
        }
    }
}

static void SchedulePostFlipIMPTimer(NVDevEvoPtr pDevEvo)
{
    if (!pDevEvo->postFlipIMPTimer) {
        pDevEvo->postFlipIMPTimer =
            nvkms_alloc_timer(
                TryToDoPostFlipIMP,
                pDevEvo,
                0, /* dataU32 */
                10000000 /* 10 seconds */);
    }
}

void nvEvoCancelPostFlipIMPTimer(NVDevEvoPtr pDevEvo)
{
    nvkms_free_timer(pDevEvo->postFlipIMPTimer);
    pDevEvo->postFlipIMPTimer = NULL;
}

/*!
 * If necessary, schedule a timer to see if usage bounds can be lowered.
 */
static void SchedulePostFlipIMP(NVDevEvoPtr pDevEvo)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    // If a timer is already scheduled, do nothing.
    if (pDevEvo->postFlipIMPTimer) {
        return;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            const NVEvoSubDevHeadStateRec *pHeadState =
                &pDevEvo->gpus[sd].headState[head];

            if (!UsageBoundsEqual(&pHeadState->usage,
                                  &pHeadState->targetUsage) ||
                (pHeadState->disableMidFrameAndDWCFWatermark !=
                 pHeadState->targetDisableMidFrameAndDWCFWatermark)) {

                SchedulePostFlipIMPTimer(pDevEvo);
                return;
            }
        }
    }
}

/*!
 * Program a flip on all requested layers on all requested heads on
 * all requested disps in NvKmsFlipRequest.
 *
 * /param[in]   skipUpdate      Update software state tracking, but don't kick
 *                              off or perform an UPDATE.
 *
 *                              Note that this should be used only when the
 *                              satellite channels (including the cursor) are
 *                              disabled -- only the core channel should be
 *                              displaying anything, and only the core surface
 *                              should be specified in a skipUpdate flip.
 * /param[in]   allowFlipLock    Whether this update should use fliplocked base
 *                               flips. This is used on nvdisplay to set the
 *                               interlock mask to include all fliplocked
 *                               channels if necessary.  This should currently
 *                               only be set when this flip was initiated
 *                               through NVKMS_IOCTL_FLIP.
 */
NvBool nvFlipEvo(NVDevEvoPtr pDevEvo,
                 const struct NvKmsPerOpenDev *pOpenDev,
                 const struct NvKmsFlipRequest *request,
                 struct NvKmsFlipReply *reply,
                 NvBool skipUpdate,
                 NvBool allowFlipLock)
{
    NvU32 head, sd;
    NvU32 requestedHeadCount, activeHeadCount, dirtyBaseChannelCount;
    NvBool ret = FALSE;
    NvBool changed = FALSE;
    NvBool allowVrr = request->allowVrr;
    NVDispEvoPtr pDispEvo;
    struct NvKmsFlipWorkArea *pWorkArea =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_FLIP_WORK_AREA,
                      sizeof(*pWorkArea));

    nvkms_memset(pWorkArea, 0, sizeof(*pWorkArea));

    /*
     * Do not execute NVKMS_IOCTL_FLIP if the display channel yet has not
     * been transitioned from vbios to driver. A modeset requires, to make
     * display channel transition from vbios to driver.
     *
     * The NVKMS client should do modeset before initiating
     * NVKMS_IOCTL_FLIP requests.
     */
    if (pDevEvo->coreInitMethodsPending) {
        goto done;
    }

    /*
     * Initialize the work area. Note we take two snapshots of the
     * current headState: newState and oldState.  newState will
     * describe the new configuration.  After that is applied, we will
     * refer to oldState to identify any surfaces that are no longer
     * in use.
     */

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        for (head = 0; head < ARRAY_LEN(pWorkArea->sd[sd].head); head++) {
            nvInitFlipEvoHwState(pDevEvo, sd, head,
                                 &pWorkArea->sd[sd].head[head].newState);
            nvInitFlipEvoHwState(pDevEvo, sd, head,
                                 &pWorkArea->sd[sd].head[head].oldState);
        }
    }


    /*!
     * Count active and requested heads so we can make a decision about VRR
     * and register syncpts if specified.
     */
    requestedHeadCount = activeHeadCount = dirtyBaseChannelCount = 0;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        const struct NvKmsFlipRequestOneSubDevice *pRequestOneSubDevice =
            &request->sd[sd];

        for (head = 0; head < ARRAY_LEN(pRequestOneSubDevice->head); head++) {
            const NvBool headActive = nvHeadIsActive(pDispEvo, head);

            if (headActive) {
                activeHeadCount++;
            }

            if (NVBIT(head) & pRequestOneSubDevice->requestedHeadsBitMask) {
                requestedHeadCount++;
            }

            if (headActive) {
                if (!nvHandleSyncptRegistration(
                        pDevEvo,
                        head,
                        &pRequestOneSubDevice->head[head],
                        &pWorkArea->sd[sd].head[head].newState)) {
                    goto done;
                }
            }
        }
    }

    /* Deactivate VRR if only a subset of the heads are requested */
    if (requestedHeadCount != activeHeadCount) {
        allowVrr = FALSE;
    }

    /* Validate the flip parameters and update the work area. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        const struct NvKmsFlipRequestOneSubDevice *pRequestOneSubDevice =
            &request->sd[sd];

        for (head = 0; head < ARRAY_LEN(pRequestOneSubDevice->head); head++) {
            const NVDispHeadStateEvoRec *pHeadState;
            const NvBool headActive = nvHeadIsActive(pDispEvo, head);

            if (!(NVBIT(head) & pRequestOneSubDevice->requestedHeadsBitMask)) {
                continue;
            }

            if (!headActive) {
                goto done;
            }

            pHeadState = &pDispEvo->headState[head];

            if (!nvUpdateFlipEvoHwState(
                    pOpenDev,
                    pDevEvo,
                    sd,
                    head,
                    &pRequestOneSubDevice->head[head],
                    &pWorkArea->sd[sd].head[head].newState,
                    allowVrr,
                    &pHeadState->timings.viewPort.possibleUsage)) {
                goto done;
            }

            if (pWorkArea->sd[sd].head[head].newState.dirty.layer[NVKMS_MAIN_LAYER]) {
                dirtyBaseChannelCount++;
            }

            if (!nvValidateFlipEvoHwState(
                    pDevEvo,
                    head,
                    &pHeadState->timings,
                    &pWorkArea->sd[sd].head[head].newState)) {
                goto done;
            }

            pWorkArea->sd[sd].changed = TRUE;
            changed = TRUE;
        }
    }

    /* Deactivate VRR if only a subset of the heads are being flipped */
    if (dirtyBaseChannelCount != activeHeadCount) {
        allowVrr = FALSE;
    }

    if (!ValidatePerDispState(pDevEvo, pWorkArea)) {
        goto done;
    }

    /* If nothing changed, fail. */

    if (!changed) {
        goto done;
    }

    ret = AllocatePreFlipBandwidth(pDevEvo, pWorkArea);
    if (!ret) {
        goto done;
    }

    if (!request->commit) {
        ret = NV_TRUE;
        goto done;
    }

    if (!PrepareToDoPreFlipIMP(pDevEvo, pWorkArea)) {
        goto done;
    }

    /*
     * At this point, something changed on at least one head of one
     * subdevice, and has been validated.  Apply the request to our
     * hardware and software state.  We must not fail beyond this
     * point.
     */

    ret = TRUE;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            // Increase refCnt of surfaces used AFTER flip
            nvUpdateSurfacesFlipRefCount(
                pDevEvo,
                head,
                &pWorkArea->sd[sd].head[head].newState,
                NV_TRUE);
        }
    }

    PreFlipIMP(pDevEvo, pWorkArea);

    /* Apply NvKmsFlipRequest::allowVrr only if a base channel has become dirty */
    if (dirtyBaseChannelCount > 0) {
        nvSetVrrActive(pDevEvo, allowVrr);
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        const struct NvKmsFlipRequestOneSubDevice *pRequestOneSubDevice =
            &request->sd[sd];

        NVEvoUpdateState updateState = { };

        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        pDispEvo = pDevEvo->gpus[sd].pDispEvo;

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            const NVFlipEvoHwState *pFlipState =
                &pWorkArea->sd[sd].head[head].newState;
            const struct NvKmsFlipCommonParams *pParams =
                &pRequestOneSubDevice->head[head];

            if (pParams->layer[NVKMS_MAIN_LAYER].skipPendingFlips &&
                pFlipState->dirty.layer[NVKMS_MAIN_LAYER] &&
                !skipUpdate) {
                pDevEvo->hal->AccelerateChannel(
                    pDevEvo,
                    pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                    sd,
                    &pWorkArea->sd[sd].head[head].oldAccelerators);
                pWorkArea->sd[sd].head[head].accelerated = TRUE;
            }

            nvFlipEvoOneHead(pDevEvo, sd, head,
                             &pWorkArea->sd[sd].head[head].newState,
                             allowFlipLock,
                             &updateState);
        }

        if (!skipUpdate) {
            pDevEvo->hal->Update(pDevEvo, &updateState, TRUE /* releaseElv */);
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            // Decrease refCnt of surfaces used BEFORE the flip
            nvUpdateSurfacesFlipRefCount(
                pDevEvo,
                head,
                &pWorkArea->sd[sd].head[head].oldState,
                NV_FALSE);
        }

        FillPostSyncptReply(pDevEvo,
                            sd,
                            &request->sd[sd],
                            &reply->sd[sd],
                            pWorkArea);

    }

    {
        NvU64 startTime = 0;
        const NvU32 timeout = 2000000; /* 2 seconds */

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (!pWorkArea->sd[sd].changed) {
                continue;
            }

            for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
                if (!pWorkArea->sd[sd].head[head].accelerated) {
                    continue;
                }

                if (!nvEvoPollForNoMethodPending(pDevEvo,
                                                 sd,
                                                 pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                                 &startTime,
                                                 timeout)) {
                    nvAssert(!"Timed out while idling base channel");
                }

                pDevEvo->hal->ResetChannelAccelerators(
                    pDevEvo,
                    pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                    sd,
                    pWorkArea->sd[sd].head[head].oldAccelerators);
            }
        }
    }

    if (dirtyBaseChannelCount > 0) {
        nvSetNextVrrFlipTypeAndIndex(pDevEvo, reply);
    } else {
        // TODO Schedule vrr unstall; per-disp/per-device?
    }

    if (!skipUpdate) {
        // Note that usage bounds are not lowered here, because the flip
        // queued by this function may not occur until later. Instead, schedule
        // a timer for later to check if the usage bounds can be lowered.
        SchedulePostFlipIMP(pDevEvo);

        pDevEvo->skipConsoleRestore = FALSE;
    }

    /* fall through */

done:

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_FLIP_WORK_AREA);

    return ret;
}
