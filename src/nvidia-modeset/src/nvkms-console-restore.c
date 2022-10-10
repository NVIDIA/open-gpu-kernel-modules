/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-console-restore.h"
#include "nvkms-dpy.h"
#include "nvkms-flip.h"
#include "nvkms-modepool.h"
#include "nvkms-modeset.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-rm.h"
#include "nvkms-utils.h"

#include "dp/nvdp-connector.h"

/*!
 * Find the first valid mode of given dimensions (width and height) that passes
 * IMP at boot clocks. If input dimensions are not given then return first
 * valid mode that passes IMP at boot clocks.
 */
static NvBool FindMode(NVDpyEvoPtr pDpyEvo,
                       const enum NvKmsSurfaceMemoryFormat format,
                       const NvU32 width,
                       const NvU32 height,
                       struct NvKmsMode *pModeOut)
{
    NvU32 index = 0;

    while (TRUE) {
        struct NvKmsValidateModeIndexParams params = { };

        params.request.dpyId = pDpyEvo->id;
        params.request.modeIndex = index++;
        params.request.modeValidation.overrides = NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS;

        nvValidateModeIndex(pDpyEvo, &params.request, &params.reply);

        if (params.reply.end) {
            break;
        }

        if (!params.reply.valid) {
            continue;
        }

        if (!(NVBIT64(format) &
              params.reply.modeUsage.layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats)) {
            continue;
        }

        if (height != 0 && height != params.reply.mode.timings.vVisible) {
            continue;
        }

        if (width != 0 && width != params.reply.mode.timings.hVisible) {
            continue;
        }

        *pModeOut = params.reply.mode;
        return TRUE;
    }

    return FALSE;
}

/*!
 * Make sure pDispEvo->connectedDpys is up to date.
 *
 * Do this by querying the dpy dynamic data for all dpys. The results aren't
 * actually important, but querying the dynamic data has the side effect of
 * updating pDispEvo->connectedDpys.
 */
static NVDpyIdList UpdateConnectedDpys(NVDispEvoPtr pDispEvo)
{
    NVDpyEvoPtr pDpyEvo;
    struct NvKmsQueryDpyDynamicDataParams *pParams =
        nvCalloc(1, sizeof(*pParams));

    if (!pParams) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_WARN,
            "Failed to allocate NvKmsQueryDpyDynamicDataParams");
        return pDispEvo->connectedDisplays;
    }

    FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->validDisplays, pDispEvo) {
        nvkms_memset(pParams, 0, sizeof(*pParams));
        nvDpyGetDynamicData(pDpyEvo, pParams);
    }

    nvFree(pParams);

    return pDispEvo->connectedDisplays;
}

static void FlipBaseToNull(NVDevEvoPtr pDevEvo)
{
    struct NvKmsFlipParams *pParams = nvCalloc(1, sizeof(*pParams));
    struct NvKmsFlipRequest *pRequest;
    NvU32 sd;
    NVDispEvoPtr pDispEvo;
    NvBool ret = TRUE;

    if (!pParams) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_WARN,
            "Failed to allocate flip parameters for console restore base flip "
            "to NULL");
        return;
    }

    pRequest = &pParams->request;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        struct NvKmsFlipRequestOneSubDevice *pRequestSd =
            &pRequest->sd[sd];
        NvU32 head;
        for (head = 0; head < pDevEvo->numHeads; head++) {
            struct NvKmsFlipCommonParams *pRequestHead =
                &pRequestSd->head[head];
            NvU32 layer;

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            pRequestSd->requestedHeadsBitMask |= NVBIT(head);

            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                pRequestHead->layer[layer].surface.specified = TRUE;
                // No need to specify sizeIn/sizeOut as we are flipping NULL surface.
                pRequestHead->layer[layer].compositionParams.specified = TRUE;
                pRequestHead->layer[layer].completionNotifier.specified = TRUE;
                pRequestHead->layer[layer].syncObjects.specified = TRUE;
            }

            pRequest->commit = TRUE;
        }
    }

    // If no heads require changes, there's nothing to do.
    if (pRequest->commit) {
        ret = nvFlipEvo(pDevEvo, pDevEvo->pNvKmsOpenDev, pRequest,
                        &pParams->reply, FALSE /* skipUpdate */,
                        FALSE /* allowFlipLock */);
    }
    nvFree(pParams);

    if (!ret) {
        nvAssert(!"Console restore failed to flip base to NULL");
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 head;
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvBool stoppedBase;
            ret = nvRMIdleBaseChannel(pDevEvo, head, sd, &stoppedBase);
            if (!ret) {
                nvAssert(!"Console restore failed to idle base");
            }
        }
    }
}

static NvBool InitModeOneHeadRequest(
    NVDpyEvoRec *pDpyEvo,
    NVSurfaceEvoPtr pSurfaceEvo,
    const struct NvKmsMode *pOverrideMode,
    const struct NvKmsSize *pOverrideViewPortSizeIn,
    const struct NvKmsPoint *pOverrideViewPortPointIn,
    const NvU32 head,
    struct NvKmsSetModeOneHeadRequest *pRequestHead)
{

    struct NvKmsFlipCommonParams *pFlip = &pRequestHead->flip;
    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 layer;

    if (pOverrideMode != NULL) {
        pRequestHead->mode = *pOverrideMode;
    } else {
        if (!FindMode(pDpyEvo,
                      pSurfaceEvo->format,
                      0 /* Ignore mode width */,
                      0 /* Ignore mode height */,
                      &pRequestHead->mode)) {
            return FALSE;
        }
    }

    pRequestHead->dpyIdList = nvAddDpyIdToEmptyDpyIdList(pDpyEvo->id);
    pRequestHead->modeValidationParams.overrides =
        NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS;
    if (pOverrideViewPortSizeIn != NULL) {
        pRequestHead->viewPortSizeIn = *pOverrideViewPortSizeIn;
    } else {
        pRequestHead->viewPortSizeIn.width = pSurfaceEvo->widthInPixels;
        pRequestHead->viewPortSizeIn.height = pSurfaceEvo->heightInPixels;
    }

    pFlip->viewPortIn.specified = TRUE;
    if (pOverrideViewPortPointIn != NULL) {
        pFlip->viewPortIn.point = *pOverrideViewPortPointIn;
    }
    pFlip->layer[NVKMS_MAIN_LAYER].surface.handle[NVKMS_LEFT] =
        pDevEvo->fbConsoleSurfaceHandle;

    pFlip->layer[NVKMS_MAIN_LAYER].sizeIn.specified = TRUE;
    pFlip->layer[NVKMS_MAIN_LAYER].sizeIn.val.width = pSurfaceEvo->widthInPixels;
    pFlip->layer[NVKMS_MAIN_LAYER].sizeIn.val.height = pSurfaceEvo->heightInPixels;

    pFlip->layer[NVKMS_MAIN_LAYER].sizeOut.specified = TRUE;
    pFlip->layer[NVKMS_MAIN_LAYER].sizeOut.val =
        pFlip->layer[NVKMS_MAIN_LAYER].sizeIn.val;

    /* Disable other layers except Main */
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {

        if (layer == NVKMS_MAIN_LAYER) {
            pFlip->layer[layer].csc.matrix = NVKMS_IDENTITY_CSC_MATRIX;
            pFlip->layer[layer].csc.specified = TRUE;
        }
        pFlip->layer[layer].surface.specified = TRUE;

        pFlip->layer[layer].completionNotifier.specified = TRUE;
        pFlip->layer[layer].syncObjects.specified = TRUE;
        pFlip->layer[layer].compositionParams.specified = TRUE;
    }

    // Disable other features.
    pFlip->cursor.imageSpecified = TRUE;
    pRequestHead->lut.input.specified = TRUE;
    pRequestHead->lut.output.specified = TRUE;
    pRequestHead->lut.synchronous = TRUE;
    pRequestHead->allowGsync = FALSE;
    pRequestHead->allowAdaptiveSync =
        NVKMS_ALLOW_ADAPTIVE_SYNC_DISABLED;

    return TRUE;
}

static NvBool
ConstructModeOneHeadRequestForOneDpy(NVDpyEvoRec *pDpyEvo,
                                     NVSurfaceEvoPtr pSurfaceEvo,
                                     struct NvKmsSetModeParams *pParams,
                                     const NvU32 dispIndex,
                                     NvU32 *pAvailableHeadsMask)
{
    NvBool ret = FALSE;
    const NvU32 possibleHeads = *pAvailableHeadsMask &
                                 pDpyEvo->pConnectorEvo->validHeadMask;

    if (possibleHeads == 0 || pDpyEvo->isVrHmd) {
        goto done;
    }

    const NvU32 head = BIT_IDX_32(LOWESTBIT(possibleHeads));

    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    struct NvKmsSetModeRequest *pRequest = &pParams->request;
    struct NvKmsSetModeOneDispRequest *pRequestDisp =
        &pRequest->disp[dispIndex];
    struct NvKmsSetModeOneHeadRequest *pRequestHead =
        &pRequestDisp->head[head];

    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    if (!InitModeOneHeadRequest(pDpyEvo,
                                pSurfaceEvo,
                                NULL /* Use default Mode */,
                                NULL /* Use default ViewPortSizeIn */,
                                NULL /* Use default ViewPortPointIn */,
                                head,
                                pRequestHead)) {
        goto done;
    }

    nvAssert(!pRequestHead->viewPortOutSpecified);
    nvAssert(!pRequest->commit);

    while (!nvSetDispModeEvo(pDevEvo,
                             pDevEvo->pNvKmsOpenDev,
                             pRequest,
                             &pParams->reply,
                             TRUE /* bypassComposition */,
                             FALSE /* doRasterLock */)) {
        /*
         * If validation is failing even after disabling scaling then leave
         * this dpy inactive.
         */
        if (pRequestHead->viewPortOutSpecified) {
            nvkms_memset(pRequestHead, 0, sizeof(*pRequestHead));
            goto done;
        }

        /* Disable scaling and try again */
        pRequestHead->viewPortOut = (struct NvKmsRect) {
            .height = pRequestHead->viewPortSizeIn.height,
            .width = pRequestHead->viewPortSizeIn.width,
            .x = 0,
            .y = 0,
        };
        pRequestHead->viewPortOutSpecified = TRUE;
    }

    *pAvailableHeadsMask &= ~NVBIT(head);

    ret = TRUE;

done:

    return ret;
}

typedef struct _TiledDisplayInfo {
    NVDpyIdList detectedDpysList;
    NvBool isDetectComplete;
    NvBool isCapToScaleSingleTile;
} TiledDisplayInfo;

/*
 * Detect Tiled-display of topology-id described in given pDisplayIdInfo.
 *
 * Loop over given all dpys from candidateConnectedDpys list, look for matching
 * topology-id. Add dpys of matching topology-id into
 * detectedTiledDisplayDpysList list. Mark Tiled-Display detect complete if all
 * exact number of tiles are found.
 */
static NvBool DetectTiledDisplay(const NVDispEvoRec *pDispEvo,
                                 const NVT_DISPLAYID_INFO *pDisplayIdInfo,
                                 const NVDpyIdList candidateConnectedDpys,
                                 TiledDisplayInfo *pTiledDisplayInfo)
{
    const NVT_TILEDDISPLAY_TOPOLOGY_ID nullTileDisplayTopoId = { 0 };
    const NVDpyEvoRec *pDpyEvo;
    const NvU32 numTiles = pDisplayIdInfo->tile_topology.row *
                           pDisplayIdInfo->tile_topology.col;
    const NvU32 numTilesMask = NVBIT(numTiles) - 1;
    NvU32 detectedTilesCount = 0;
    NvU32 detectedTilesMask = 0;

    NVDpyIdList detectedTiledDisplayDpysList = nvEmptyDpyIdList();

    /*
     * If parsed edid is valid and tile_topology_id is non-zero then the dpy
     * is considered a valid tile of a tiled display.
     *
     * The 'tile_topology_id' is a triplet of ids consisting of vendor_id,
     * product_id, and serial_number.  The DisplayId specification does not
     * clearly define an invalid 'tile_topology_id', but here the
     * tile_topology_id is considered invalid only if all three ids are zero
     * which is consistent with other protocols like RandR1.2 'The tile group
     * identifier'.
     */
    if (!nvkms_memcmp(&pDisplayIdInfo->tile_topology_id,
                      &nullTileDisplayTopoId, sizeof(nullTileDisplayTopoId))) {
        return FALSE;
    }

    /*
     * Reject Tiled-Display consists of multiple physical display enclosures or
     * requires to configure bezel.
     */
    if (!pDisplayIdInfo->tile_capability.bSingleEnclosure ||
         pDisplayIdInfo->tile_capability.bHasBezelInfo) {
        return FALSE;
    }

    /*
     * Reject Tiled-Display which has number of horizontal or vertical tiles
     * greater than 4.
     */
    if (pDisplayIdInfo->tile_topology.row <= 0 ||
        pDisplayIdInfo->tile_topology.col <= 0 ||
        pDisplayIdInfo->tile_topology.row > 4 ||
        pDisplayIdInfo->tile_topology.col > 4) {
        return FALSE;
    }

    FOR_ALL_EVO_DPYS(pDpyEvo, candidateConnectedDpys, pDispEvo) {
        const NVT_EDID_INFO *pEdidInfo = &pDpyEvo->parsedEdid.info;
        const NVT_DISPLAYID_INFO *pDpyDisplayIdInfo =
            &pEdidInfo->ext_displayid;

        if (!pDpyEvo->parsedEdid.valid) {
            continue;
        }

        if (nvkms_memcmp(&pDisplayIdInfo->tile_topology_id,
                         &pDpyDisplayIdInfo->tile_topology_id,
                         sizeof(&pDpyDisplayIdInfo->tile_topology_id))) {
            continue;
        }

        /*
         * Tiled-Display Topology:
         *
         *        |-----------col
         *
         * ___    +------------+------------+...
         *  |     | (x=0,y=0)  | (x=1,y=0)  |
         *  |     |            |            |
         *  |     |            |            |
         *  |     +------------+------------+
         * row    | (x=0,y=1)  | (x=1,y=1)  |
         *        |            |            |
         *        |            |            |
         *        +------------+------------+
         *        .
         *        .
         *        .
         */
        if (pDpyDisplayIdInfo->tile_topology.row !=
            pDisplayIdInfo->tile_topology.row) {
            continue;
        }

        if (pDpyDisplayIdInfo->tile_topology.col !=
            pDisplayIdInfo->tile_topology.col) {
            continue;
        }

        if (pDpyDisplayIdInfo->tile_location.x >=
            pDpyDisplayIdInfo->tile_topology.col) {
            continue;
        }

        if (pDpyDisplayIdInfo->tile_location.y >=
            pDpyDisplayIdInfo->tile_topology.row) {
            continue;
        }

        nvAssert(pDpyDisplayIdInfo->tile_capability.single_tile_behavior ==
                 pDisplayIdInfo->tile_capability.single_tile_behavior);

        detectedTiledDisplayDpysList =
            nvAddDpyIdToDpyIdList(pDpyEvo->id, detectedTiledDisplayDpysList);

        detectedTilesMask |= NVBIT((pDpyDisplayIdInfo->tile_location.y *
                                    pDpyDisplayIdInfo->tile_topology.col) +
                                   (pDpyDisplayIdInfo->tile_location.x));
        detectedTilesCount++;
    }

    pTiledDisplayInfo->detectedDpysList = detectedTiledDisplayDpysList;

    if (detectedTilesCount != numTiles || detectedTilesMask != numTilesMask) {
        pTiledDisplayInfo->isDetectComplete = FALSE;
    } else {
        pTiledDisplayInfo->isDetectComplete = TRUE;
    }

    pTiledDisplayInfo->isCapToScaleSingleTile =
        pDisplayIdInfo->tile_capability.single_tile_behavior ==
        NVT_SINGLE_TILE_BEHAVIOR_SCALE;

    return TRUE;
}

/* Construct modeset request for given Tiled-display */
static NvBool
ConstructModeRequestForTiledDisplay(const NVDispEvoRec *pDispEvo,
                                    NVSurfaceEvoPtr pSurfaceEvo,
                                    struct NvKmsSetModeParams *pParams,
                                    const NvU32 dispIndex,
                                    NVDpyIdList tiledDisplayDpysList,
                                    NvU32 *pAvailableHeadsMask)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    /*
     * Get arbitrary dpy from tiledDisplayDpysList,
     * to extract Tiled-Display information which should be same across all
     * tiles.
     */
    NVDpyEvoRec *pArbitraryDpyEvo =
        nvGetOneArbitraryDpyEvo(tiledDisplayDpysList, pDispEvo);
    const NVT_DISPLAYID_INFO *pPrimaryDisplayIdInfo =
        &pArbitraryDpyEvo->parsedEdid.info.ext_displayid;
    const NvU32 numRows = pPrimaryDisplayIdInfo->tile_topology.row;
    const NvU32 numColumns = pPrimaryDisplayIdInfo->tile_topology.col;
    /*
     * Split entire input viewport across all tiles of Tiled-Display.
     */
    const struct NvKmsSize viewPortSizeIn = {
        .width  = (pSurfaceEvo->widthInPixels / numColumns),
        .height = (pSurfaceEvo->heightInPixels / numRows),
    };
    struct NvKmsSetModeRequest *pRequest = &pParams->request;
    struct NvKmsSetModeOneDispRequest *pRequestDisp =
        &pRequest->disp[dispIndex];
    NvU32 firstClaimedHead = NV_INVALID_HEAD;
    NvU32 claimedHeadMask = 0x0;
    NVDpyEvoRec *pDpyEvo;
    NvU32 head;

    /*
     * Return failure if not enough number of heads available to construct
     * modeset request for Tiled-Display.
     */
    if (nvPopCount32(*pAvailableHeadsMask) <
        nvCountDpyIdsInDpyIdList(tiledDisplayDpysList)) {
        return FALSE;
    }

    /*
     * Return failure if input viewport has not been split across
     * tiles evenly.
     */
    if ((pSurfaceEvo->widthInPixels % numRows != 0) ||
        (pSurfaceEvo->heightInPixels % numColumns != 0)) {
        return FALSE;
    }

    FOR_ALL_EVO_DPYS(pDpyEvo, tiledDisplayDpysList, pDispEvo) {
        const NVT_DISPLAYID_INFO *pDpyDisplayIdInfo =
                                  &pDpyEvo->parsedEdid.info.ext_displayid;
        const struct NvKmsPoint viewPortPointIn = {
            .x = pDpyDisplayIdInfo->tile_location.x * viewPortSizeIn.width,
            .y = pDpyDisplayIdInfo->tile_location.y * viewPortSizeIn.height
        };
        const NvU32 possibleHeads = *pAvailableHeadsMask &
                                     pDpyEvo->pConnectorEvo->validHeadMask &
                                     ~claimedHeadMask;

        if (possibleHeads == 0 || pDpyEvo->isVrHmd) {
            goto failed;
        }

        const NvU32 head = BIT_IDX_32(LOWESTBIT(possibleHeads));
        struct NvKmsSetModeOneHeadRequest *pRequestHead =
            &pRequestDisp->head[head];
        struct NvKmsMode mode;

        if (firstClaimedHead == NV_INVALID_HEAD) {
            /*
             * Find mode of native dimensions reported in Tiled-Display
             * information.
             */
            if (!FindMode(pDpyEvo,
                          pSurfaceEvo->format,
                          pPrimaryDisplayIdInfo->native_resolution.width,
                          pPrimaryDisplayIdInfo->native_resolution.height,
                          &mode)) {
                goto failed;
            }

            firstClaimedHead = head;
        } else {
            /* All tiles should support same set of modes */
            mode = pRequestDisp->head[firstClaimedHead].mode;
        }

        claimedHeadMask |= NVBIT(head);

        if (!InitModeOneHeadRequest(pDpyEvo,
                                    pSurfaceEvo,
                                    &mode,
                                    &viewPortSizeIn,
                                    &viewPortPointIn,
                                    head,
                                    pRequestHead)) {
            goto failed;
        }
    }

    nvAssert(!pRequest->commit);

    if (!nvSetDispModeEvo(pDevEvo,
                          pDevEvo->pNvKmsOpenDev,
                          pRequest,
                          &pParams->reply,
                          TRUE /* bypassComposition */,
                          FALSE /* doRasterLock */)) {
        goto failed;
    }
    *pAvailableHeadsMask &= ~claimedHeadMask;

    return TRUE;

failed:

    for (head = 0; head < ARRAY_LEN(pRequestDisp->head); head++) {
        if ((NVBIT(head) & claimedHeadMask) == 0x0) {
            continue;
        }
        nvkms_memset(&pRequestDisp->head[head],
                     0,
                     sizeof(pRequestDisp->head[head]));
    }

    return FALSE;
}

static NvBool isDpMSTModeActiveOnAnyConnector(NVDevEvoPtr pDevEvo)
{
    NvU32 i;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVDispHeadStateEvoRec *pHeadState =
                &pDispEvo->headState[head];
            const NVConnectorEvoRec *pConnectorEvo = pHeadState->pConnectorEvo;

            if ((pConnectorEvo != NULL) &&
                    nvConnectorUsesDPLib(pConnectorEvo)) {
                const enum NVDpLinkMode activeLinkMode =
                    nvDPGetActiveLinkMode(pConnectorEvo->pDpLibConnector);

                nvAssert(activeLinkMode != NV_DP_LINK_MODE_OFF);

                if (activeLinkMode == NV_DP_LINK_MODE_MST) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/*!
 * Attempt to restore the console.
 *
 * If a framebuffer console surface was successfully imported from RM, then use
 * the core channel to set a mode that displays it.
 *
 * Enables as many heads as possible in a clone configuration. In first pass
 * for connected boot dpys and in second pass for other remaining dpys:
 *
 *   1. Populate modeset request to enable given dpy.
 *
 *   2. Do modeset request validation, if fails then disable scaling. If
 *   modeset request validation fails even after disabling scaling then do not
 *   enable that dpy.
 *
 * If console restore succeeds, set pDevEvo->skipConsoleRestore to skip
 * deallocating the core channel and triggering RM's console restore code.
 */
NvBool nvEvoRestoreConsole(NVDevEvoPtr pDevEvo, const NvBool allowMST)
{
    NvBool ret = FALSE;
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pDevEvo->pNvKmsOpenDev);
    NVSurfaceEvoPtr pSurfaceEvo =
        nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles,
                                     pDevEvo->fbConsoleSurfaceHandle);
    struct NvKmsSetModeParams *params;

    /*
     * If this function fails to restore a console then NVKMS frees
     * and reallocates the core channel, to attempt the console
     * restore using Resman. The core channel reallocation also may
     * fail and nvEvoRestoreConsole() again may get called from
     * nvFreeDevEvo() when client frees the NVKMS device.
     *
     * If nvEvoRestoreConsole() gets called after the core channel
     * allocation/reallocation failure then do nothing and return
     * early.
     */
    if (pDevEvo->displayHandle == 0x0) {
        goto done;
    }

    /*
     * If any DP-MST mode is active on any connector of this device but
     * DP-MST is disallowed then force console-restore.
     */
    if (pDevEvo->skipConsoleRestore &&
            !allowMST && isDpMSTModeActiveOnAnyConnector(pDevEvo)) {
        pDevEvo->skipConsoleRestore = FALSE;
    }

    if (pDevEvo->skipConsoleRestore) {
        ret = TRUE;
        goto done;
    }

    if (!pSurfaceEvo) {
        // No console surface to restore.
        goto done;
    }

    FlipBaseToNull(pDevEvo);

    params = nvPreallocGet(pDevEvo, PREALLOC_TYPE_RESTORE_CONSOLE_SET_MODE,
                           sizeof(*params));
    nvkms_memset(params, 0, sizeof(*params));

    nvDPSetAllowMultiStreaming(pDevEvo, allowMST);

    // Construct the request.
    //
    // To start with, try to enable as many connected dpys as possible,
    // preferring boot displays first.
    struct NvKmsSetModeRequest *pRequest = &params->request;
    NvBool foundDpysConfigForConsoleRestore = FALSE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 availableHeadsMask = NVBIT(pDevEvo->numHeads) - 1;
        NVDpyIdList connectedDpys = UpdateConnectedDpys(pDispEvo);
        const NVDpyIdList connectedBootDpys =
            nvIntersectDpyIdListAndDpyIdList(connectedDpys,
                                             pDispEvo->bootDisplays);
        struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[dispIndex];
        int pass;

        pRequest->requestedDispsBitMask |= NVBIT(dispIndex);
        pRequestDisp->requestedHeadsBitMask = availableHeadsMask;

        // Only enable heads on the subdevice that actually contains the
        // console.
        if (dispIndex != pDevEvo->vtFbInfo.subDeviceInstance) {
            continue;
        }

        NVDpyIdList handledDpysList = nvEmptyDpyIdList();

        for (pass = 0; pass < 2; pass++) {
            NVDpyIdList candidateDpys;
            NVDpyEvoPtr pDpyEvo;

            if (availableHeadsMask == 0) {
                break;
            }

            if (pass == 0) {
                candidateDpys = connectedBootDpys;
            } else {
                candidateDpys = nvDpyIdListMinusDpyIdList(connectedDpys,
                                                          connectedBootDpys);
            }

            FOR_ALL_EVO_DPYS(pDpyEvo, candidateDpys, pDispEvo) {
                NvBool isTiledDisplayFound = FALSE;
                TiledDisplayInfo tiledDisplayInfo = { };
                NvBool isTiledDisplayEnable = FALSE;
                const NVT_DISPLAYID_INFO *pDpyDisplayIdInfo =
                    pDpyEvo->parsedEdid.valid ?
                        &pDpyEvo->parsedEdid.info.ext_displayid : NULL;
                NvBool done = FALSE;

                if (availableHeadsMask == 0) {
                    break;
                }

                if (nvDpyIdIsInDpyIdList(pDpyEvo->id,
                                         handledDpysList)) {
                    continue;
                }

                isTiledDisplayFound =
                    pDpyDisplayIdInfo != NULL &&
                    DetectTiledDisplay(pDispEvo,
                                       pDpyDisplayIdInfo,
                                       nvDpyIdListMinusDpyIdList(
                                           connectedDpys, handledDpysList),
                                       &tiledDisplayInfo);

                /*
                 * Construct modeset request for Tiled-Display which don't have
                 * a capability to scale single tile input across entire
                 * display. If fails then fallback to construct modeset request
                 * for this single dpy.
                 */

                if (isTiledDisplayFound &&
                    tiledDisplayInfo.isDetectComplete &&
                    !tiledDisplayInfo.isCapToScaleSingleTile) {

                    done = ConstructModeRequestForTiledDisplay(
                                                    pDispEvo,
                                                    pSurfaceEvo,
                                                    params,
                                                    dispIndex,
                                                    tiledDisplayInfo.detectedDpysList,
                                                    &availableHeadsMask);
                    isTiledDisplayEnable = done;
                }

                /*
                 * If Tiled-Display has capability to scale single tile input
                 * across entire display then for console restore it is
                 * sufficient to light up any single tile and ignore rest of
                 * remaining tiles.
                 */

                if (!done ||
                    !isTiledDisplayFound ||
                    !tiledDisplayInfo.isDetectComplete ||
                    tiledDisplayInfo.isCapToScaleSingleTile) {

                    done = ConstructModeOneHeadRequestForOneDpy(
                                                    pDpyEvo,
                                                    pSurfaceEvo,
                                                    params,
                                                    dispIndex,
                                                    &availableHeadsMask);
                    isTiledDisplayEnable =
                        done && tiledDisplayInfo.isCapToScaleSingleTile;
                }

                handledDpysList =
                    nvAddDpyIdToDpyIdList(pDpyEvo->id, handledDpysList);

                if (isTiledDisplayEnable) {
                    handledDpysList = nvAddDpyIdListToDpyIdList(
                        tiledDisplayInfo.detectedDpysList,
                        handledDpysList);
                }

                foundDpysConfigForConsoleRestore =
                    foundDpysConfigForConsoleRestore || done;

            }
        }
    }

    /*
     * Disable all (flip/raster) locks, dirty locking state in hardware
     * left behind by NVKMS console restore causes XID errors and engine hang
     * on next modeset because the NVKMS doesn't get back existing display
     * hardware state at the time of initialization.
     */

    if (foundDpysConfigForConsoleRestore) {
        pRequest->commit = TRUE;

        ret = nvSetDispModeEvo(pDevEvo,
                               pDevEvo->pNvKmsOpenDev,
                               pRequest,
                               &params->reply,
                               TRUE /* bypassComposition */,
                               FALSE /* doRasterLock */);
    }

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_RESTORE_CONSOLE_SET_MODE);

done:
    nvkms_free_timer(pDevEvo->consoleRestoreTimer);
    pDevEvo->consoleRestoreTimer = NULL;

    /* If console restore failed then simply shut down all heads */
    if (!ret) {
        nvShutDownHeads(pDevEvo, NULL /* pTestFunc, shut down all heads */);
    }

    // If restoring the console from here succeeded, then skip triggering RM's
    // console restore.
    pDevEvo->skipConsoleRestore = ret;
    return ret;
}
