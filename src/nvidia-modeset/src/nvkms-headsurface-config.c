/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-headsurface.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-headsurface-config.h"
#include "nvkms-headsurface-3d.h"
#include "nvkms-headsurface-matrix.h"
#include "nvkms-headsurface-swapgroup.h"
#include "nvkms-utils-flip.h"
#include "nvkms-flip.h"
#include "nvkms-utils.h"
#include "nvkms-surface.h"
#include "nvkms-private.h"
#include "nvkms-evo.h"
#include "nvkms-modeset.h"
#include "nvkms-stereo.h"
#include "nvkms-prealloc.h"
#include "nvidia-push-utils.h" /* nvPushIdleChannel() */

/*!
 * Use warp and blend if any of the warp and blend surfaces were specified.
 */
static NvBool UsesWarpAndBlend(
    const struct NvKmsSetModeOneHeadRequest *pRequestHead)
{
    return pRequestHead->headSurface.warpMesh.surfaceHandle  != 0 ||
           pRequestHead->headSurface.blendTexSurfaceHandle  != 0 ||
           pRequestHead->headSurface.offsetTexSurfaceHandle != 0;
}

/*
 * If 3D space, the identity matrix would be
 *
 *   1 0 0
 *   0 1 0
 *   0 0 1
 *
 * but for 2D homogeneous coordinate space, any matrix with:
 *
 *   n 0 0
 *   0 n 0
 *   0 0 n
 *
 * is an identity matrix.
 */
static NvBool Is2dHomogeneousIdentity(const struct NvKmsMatrix *m)
{
    return m->m[0][1] == 0 &&
           m->m[0][2] == 0 &&
           m->m[1][0] == 0 &&
           m->m[1][2] == 0 &&
           m->m[2][0] == 0 &&
           m->m[2][1] == 0 &&
           m->m[0][0] == m->m[1][1] &&
           m->m[1][1] == m->m[2][2];
}

/*
 * A scaling transform is any where:
 *
 *   a 0 0
 *   0 b 0
 *   0 0 c
 */
static NvBool IsScalingTransform(const struct NvKmsMatrix *m)
{
    return m->m[0][1] == 0 &&
           m->m[0][2] == 0 &&
           m->m[1][0] == 0 &&
           m->m[1][2] == 0 &&
           m->m[2][0] == 0 &&
           m->m[2][1] == 0;
}

static inline NvBool StateNeedsHeadSurface(const NVHsConfigState state)
{
    return state == NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE ||
           state == NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
}

static inline void CopyHsStateOneHeadAllDisps(
    NVHsStateOneHeadAllDisps *pDst,
    const NVHsStateOneHeadAllDisps *pSrc)
{
    nvkms_memcpy(pDst, pSrc, sizeof(NVHsStateOneHeadAllDisps));
}

static inline void MoveHsStateOneHeadAllDisps(
    NVHsStateOneHeadAllDisps *pDst,
    NVHsStateOneHeadAllDisps *pSrc)
{
    CopyHsStateOneHeadAllDisps(pDst, pSrc);
    nvkms_memset(pSrc, 0, sizeof(NVHsStateOneHeadAllDisps));
}

/*!
 * Free the surfaces tracked in pHsOneHeadAllDisps.
 *
 * surfacesReused indicates that the NVHsSurfaceRecs were reused from the
 * current configuration, and therefore should not actually be freed.  If
 * surfacesReused is TRUE, update the pHsOneHeadAllDisps structure, but do not
 * free the NVHsSurfaceRecs.
 *
 * \param[in]  pDevEvo             The device.
 * \param[in]  pHsOneHeadAllDisps  The structure tracking the surfaces.
 * \param[in]  surfacesReused      Whether the surface was reused.
 */
static void HsConfigFreeHeadSurfaceSurfaces(
    NVDevEvoRec *pDevEvo,
    NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps,
    NvBool surfacesReused)
{
    int eye, buf;

    for (buf = 0; buf < pHsOneHeadAllDisps->surfaceCount; buf++) {

        /*
         * If we get here, we expect that the headSurface surfaces are still
         * allocated.  But depending on the configuration, we may only have left
         * surfaces, not right surfaces, so only assert for the left eye.
         */
        nvAssert(pHsOneHeadAllDisps->surfaces[NVKMS_LEFT][buf].pSurface !=
                 NULL);

        for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
            if (!surfacesReused) {
                nvHsFreeSurface(
                    pDevEvo,
                    pHsOneHeadAllDisps->surfaces[eye][buf].pSurface);
                nvHsFreeSurface(
                    pDevEvo,
                    pHsOneHeadAllDisps->surfaces[eye][buf].pStagingSurface);
            }
            pHsOneHeadAllDisps->surfaces[eye][buf].pSurface = NULL;
            pHsOneHeadAllDisps->surfaces[eye][buf].pStagingSurface = NULL;
        }
    }
    pHsOneHeadAllDisps->surfaceCount = 0;
}


/*!
 * Update the NVHsChannelConfig's surfaceSize.
 */
static void HsConfigUpdateOneHeadSurfaceSize(
    NVHsChannelConfig *pChannelConfig)
{
    pChannelConfig->surfaceSize = pChannelConfig->frameSize;

    pChannelConfig->stagingSurfaceSize.width = 0;
    pChannelConfig->stagingSurfaceSize.height = 0;

    /*
     * When SwapGroup is enabled, we double the size of the surface and allocate
     * staging surfaces.
     *
     * Note we double the height, not the width, for better cache locality:
     * frames of headSurface will be rendered to either the top of bottom half
     * of the surface.
     */
    if (pChannelConfig->neededForSwapGroup) {
        pChannelConfig->surfaceSize.height *= 2;

        pChannelConfig->stagingSurfaceSize.width =
            pChannelConfig->viewPortIn.width;
        pChannelConfig->stagingSurfaceSize.height =
            pChannelConfig->viewPortIn.height;
    }
}


/*!
 * Update NVHsConfigOneHead
 *
 * Given the modetimings and NvKmsSetModeHeadSurfaceParams, update the
 * NVHsConfigOneHead.  If state is PARTIAL or FULL, compute the needed size of
 * the headSurface surfaces.
 *
 * \param[in]      state           To what extent, if any, headSurface should
 *                                 be used.
 * \param[in]      pTimings        The modetimings in use on the head.
 * \param[in]      p               The requested configuration from the client.
 * \param[in,out]  pChannelConfig  The headSurface channel config for the head.
 */
static NvBool HsConfigUpdateOneHead(
    const NVHsConfigState state,
    const NVHwModeTimingsEvo *pTimings,
    const struct NvKmsSetModeHeadSurfaceParams *p,
    NVHsChannelConfig *pChannelConfig)
{
    struct NvKmsSize size = { 0 };
    struct NvKmsRect viewPortOut = { 0 };

    if (state == NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE) {
        /*
         * If PARTIAL, the viewPortOut and surface will have the same size, and
         * the viewPortOut will be positioned at the origin of the surface.
         * Note that for double scan modes, the headSurface viewPortOut height
         * will _not_ be doubled (the line doubling will be done using display
         * hardware).
         */
        viewPortOut = nvEvoViewPortOutClientView(pTimings);
        viewPortOut.x = 0;
        viewPortOut.y = 0;
        size.width = viewPortOut.width;
        size.height = viewPortOut.height;

        /* SW yuv420 modes should always be forced to FULL_HEAD_SURFACE. */
        nvAssert(pTimings->yuv420Mode != NV_YUV420_MODE_SW);

    } else if (state == NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE) {
        /*
         * Note that for both double scan and SW yuv420 modes, the headSurface
         * viewPortOut will be adjusted relative to viewPortIn.  In both cases
         * viewPortOut will match the modetimings and headSurface, not the
         * display hardware, will perform the needed scaling/conversion from
         * viewPortIn to viewPortOut.
         */
        viewPortOut = nvEvoViewPortOutHwView(pTimings);
        size.width = nvEvoVisibleWidth(pTimings);
        size.height = nvEvoVisibleHeight(pTimings);
    }

    /* viewPortOut must fit within frameSize */
    nvAssert((viewPortOut.x + viewPortOut.width) <= size.width);
    nvAssert((viewPortOut.y + viewPortOut.height) <= size.height);

    pChannelConfig->state = state;

    pChannelConfig->frameSize = size;
    pChannelConfig->viewPortOut = viewPortOut;

    HsConfigUpdateOneHeadSurfaceSize(pChannelConfig);

    if (StateNeedsHeadSurface(state)) {
        return nvHsAssignTransformMatrix(pChannelConfig, p);
    }

    return TRUE;
}

static NvBool HsConfigInitModesetOneHeadWarpAndBlendSurface(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const NvKmsSurfaceHandle handle,
    NVSurfaceEvoPtr *ppSurface)
{
    if (handle == 0) {
        *ppSurface = NULL;
        return TRUE;
    }

    *ppSurface =
        nvEvoGetSurfaceFromHandleNoDispHWAccessOk(pDevEvo,
                                                  pOpenDevSurfaceHandles,
                                                  handle);

    return *ppSurface != NULL;
}

/*!
 * Initialize NVHsChannelConfig::warpMesh.
 *
 * If the client's request is invalid, return FALSE.
 * Otherwise, assign NVHsChannelConfig::warpMesh appropriately and return TRUE.
 *
 * \param[in]  pOpenDevSurfaceHandles The client's api handles structure.
 * \param[in]  p                      The client request structure.
 * \param[out] pChannelConfig         The channel configuration to be assigned.
 */
static NvBool HsConfigInitModesetWarpMesh(
    const NVDevEvoRec *pDevEvo,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsSetModeHeadSurfaceParams *p,
    NVHsChannelConfig *pChannelConfig)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo;
    NVSurfaceEvoPtr pSurface;
    NvU32 neededSize;

    if (p->warpMesh.surfaceHandle == 0) {
        return TRUE;
    }

    pSurface =
        nvEvoGetSurfaceFromHandleNoDispHWAccessOk(pDevEvo,
                                                  pOpenDevSurfaceHandles,
                                                  p->warpMesh.surfaceHandle);
    if (pSurface == NULL) {
        return FALSE;
    }

    switch (p->warpMesh.dataType) {
    case NVKMS_WARP_MESH_DATA_TYPE_TRIANGLES_XYUVRQ:
        if ((p->warpMesh.vertexCount % 3) != 0) {
            return FALSE;
        }
        break;
    case NVKMS_WARP_MESH_DATA_TYPE_TRIANGLE_STRIP_XYUVRQ:
        break;
    default:
        return FALSE;
    }

    if (p->warpMesh.vertexCount < 3) {
        return FALSE;
    }

    if (!NV_IS_ALIGNED(pSurface->widthInPixels, 1024)) {
        return FALSE;
    }

    pFormatInfo = nvKmsGetSurfaceMemoryFormatInfo(pSurface->format);
    if (pFormatInfo->rgb.bytesPerPixel != 4) {
        return FALSE;
    }

    if (pSurface->layout != NvKmsSurfaceMemoryLayoutPitch) {
        return FALSE;
    }

    neededSize = p->warpMesh.vertexCount * sizeof(Nv3dFloat) * 6;

    if (neededSize > pSurface->planes[0].rmObjectSizeInBytes) {
        return FALSE;
    }

    pChannelConfig->warpMesh.pSurface = pSurface;
    pChannelConfig->warpMesh.vertexCount = p->warpMesh.vertexCount;
    pChannelConfig->warpMesh.dataType = p->warpMesh.dataType;

    return TRUE;
}

/*!
 * Initialize pHsConfigOneHead.
 *
 * This is called once by nvHsConfigInitModeset(), and therefore should only
 * assign configuration parameters that do not change when NVHsConfigState
 * changes.
 *
 * Configuration that changes based on NVHsConfigState should be handled by
 * HsConfigUpdateOneHead(), so that it can be adjusted by nvHsConfigDowngrade()
 * if necessary.
 */
static NvBool HsConfigInitModesetOneHead(
    const NVDevEvoRec *pDevEvo,
    NVHsConfigState state,
    const NvU8 eyeMask,
    const NvU32 apiHead,
    const NvU32 dispIndex,
    const NVHwModeTimingsEvo *pTimings,
    const struct NvKmsSize *pViewPortSizeIn,
    const struct NvKmsFlipCommonParams *pFlipParams,
    const struct NvKmsSetModeHeadSurfaceParams *pHSParams,
    const struct NvKmsPerOpenDev *pOpenDev,
    NVHsConfigOneHead *pHsConfigOneHead)
{
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);
    NVHsChannelConfig *pChannelConfig = &pHsConfigOneHead->channelConfig;
    const NVDispEvoRec *pDispEvo = pDevEvo->pDispEvo[dispIndex];
    NvU32 layer;

    /* The passed-in state indicates whether modeset needs headSurface. */

    pChannelConfig->neededForModeset = StateNeedsHeadSurface(state);

    /*
     * If the current SwapGroup configuration needs headSurface, preserve that
     * in pChannelConfig and override the state.  Note we determine this by
     * inspecting the SwapGroups on the device: it isn't sufficient to look at
     * pDispEvo->pHsChannel[apiHead]->config.neededForSwapGroup because
     * pDispEvo->pHsChannel[apiHead] will by NULL if the head was previously
     * disabled and the new modeset is enabling the head.
     */
    pChannelConfig->neededForSwapGroup =
        nvHsSwapGroupIsHeadSurfaceNeeded(pDispEvo, apiHead);

    if (pDispEvo->pHsChannel[apiHead] != NULL) {
        nvAssert(pDispEvo->pHsChannel[apiHead]->config.neededForSwapGroup ==
                 pChannelConfig->neededForSwapGroup);
    }

    /*
     * If headSurface is needed for SwapGroup, make sure the state is at least
     * PARTIAL.
     */
    if (pChannelConfig->neededForSwapGroup && !StateNeedsHeadSurface(state)) {
        state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
    }

    if (state == NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE) {
        nvkms_memset(pHsConfigOneHead, 0, sizeof(*pHsConfigOneHead));
        return TRUE;
    }

    pChannelConfig->eyeMask = eyeMask;

    /*
     * XXX NVKMS HEADSURFACE TODO: Update viewPortIn.[xy] for panning updates.
     */
    if (pFlipParams->viewPortIn.specified) {
        pChannelConfig->viewPortIn.x = pFlipParams->viewPortIn.point.x;
        pChannelConfig->viewPortIn.y = pFlipParams->viewPortIn.point.y;
    } else {
        pChannelConfig->viewPortIn.x = 0;
        pChannelConfig->viewPortIn.y = 0;
    }

    pChannelConfig->viewPortIn.width = pViewPortSizeIn->width;
    pChannelConfig->viewPortIn.height = pViewPortSizeIn->height;

    pChannelConfig->yuv420 = (pTimings->yuv420Mode == NV_YUV420_MODE_SW);

    pChannelConfig->blendAfterWarp = pHSParams->blendAfterWarp;
    pChannelConfig->pixelShift = pHSParams->pixelShift;
    pChannelConfig->resamplingMethod = pHSParams->resamplingMethod;

    if (!HsConfigInitModesetOneHeadWarpAndBlendSurface(
            pDevEvo,
            pOpenDevSurfaceHandles,
            pHSParams->blendTexSurfaceHandle,
            &pChannelConfig->pBlendTexSurface)) {
        return FALSE;
    }

    if (!HsConfigInitModesetOneHeadWarpAndBlendSurface(
            pDevEvo,
            pOpenDevSurfaceHandles,
            pHSParams->offsetTexSurfaceHandle,
            &pChannelConfig->pOffsetTexSurface)) {
        return FALSE;
    }

    if (!HsConfigInitModesetWarpMesh(pDevEvo,
                                     pOpenDevSurfaceHandles,
                                     pHSParams,
                                     pChannelConfig)) {
        return FALSE;
    }

    /*
     * Modeset does not inherit the old flip state, therefore
     * make sure to clear the surfaces if the client hasn't specified
     * new surfaces.
     */
    for (layer = 0; layer < ARRAY_LEN(pFlipParams->layer); layer++) {
        if (pFlipParams->layer[layer].surface.specified) {
            NvBool ret = nvAssignSurfaceArray(pDevEvo,
                                    pOpenDevSurfaceHandles,
                                    pFlipParams->layer[layer].surface.handle,
                                    FALSE /* isUsedByCursorChannel */,
                                    TRUE /* isUsedByLayerChannel */,
                                    pHsConfigOneHead->layer[layer].pSurfaceEvo);
            if (!ret) {
                return FALSE;
            }
        } else {
            nvkms_memset(pHsConfigOneHead->layer[layer].pSurfaceEvo,
                         0,
                         sizeof(pHsConfigOneHead->layer[layer].pSurfaceEvo));
        }
    }

    /* XXX make cursor stereo-aware */
    if (pFlipParams->cursor.imageSpecified) {
        if (!nvAssignCursorSurface(pOpenDev, pDevEvo, &pFlipParams->cursor.image,
                                  &pChannelConfig->cursor.pSurfaceEvo)) {
            return FALSE;
        }
    } else {
        pChannelConfig->cursor.pSurfaceEvo = NULL;
    }

    if (pFlipParams->cursor.positionSpecified) {
        pChannelConfig->cursor.x = pFlipParams->cursor.position.x;
        pChannelConfig->cursor.y = pFlipParams->cursor.position.y;
    } else {
        pChannelConfig->cursor.x = 0;
        pChannelConfig->cursor.y = 0;
    }

    {
        const NVSurfaceEvoRec *pMainSurfaceEvo =
            pHsConfigOneHead->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];

        if ((pMainSurfaceEvo != NULL) &&
                ((pMainSurfaceEvo->format ==
                    NvKmsSurfaceMemoryFormatA2B10G10R10) ||
                 (pMainSurfaceEvo->format ==
                    NvKmsSurfaceMemoryFormatX2B10G10R10))) {
            pChannelConfig->hs10bpcHint = TRUE;
        } else {
            pChannelConfig->hs10bpcHint = FALSE;
        }
    }

    if (!HsConfigUpdateOneHead(state, pTimings, pHSParams, pChannelConfig)) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Validate the requested headSurface configuration.
 *
 * If the configuration is valid return TRUE.
 * Otherwise, assign pReplyHead->status and return FALSE.
 *
 * The general rules for when to use which status:
 *
 * INVALID_HEAD_SURFACE is reported for bad API usage; e.g., things such as
 * unrecognized enum values.
 *
 * UNSUPPORTED_HEAD_SURFACE_COMBO is reported when headSurface does not support
 * the requested combination of feaures.
 *
 * UNSUPPORTED_HEAD_SURFACE_FEATURE is reported if the requested configuration
 * is not supported on the current GPU (Quadro checks, and similar).
 */
static NvBool HsConfigValidate(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    struct NvKmsSetModeOneHeadReply *pReplyHead)
{
    const NvModeTimings *pModeTimings = &pRequestHead->mode.timings;
    const struct NvKmsSetModeHeadSurfaceParams *p = &pRequestHead->headSurface;

    /*
     * Validate that the requested rotation is a recognized value.
     */
    switch (p->rotation) {
    case NVKMS_ROTATION_0:
    case NVKMS_ROTATION_90:
    case NVKMS_ROTATION_180:
    case NVKMS_ROTATION_270:
        break;
    default:
        goto failInvalid;
    }

    /*
     * Check warp&blend feature compatibility.
     */
    if (UsesWarpAndBlend(pRequestHead)) {
        if (pModeTimings->yuv420Mode == NV_YUV420_MODE_SW) {
            goto failUnsupportedCombo;
        }
    }

    /*
     * Validate that the requested pixelShift is a recognized value.
     */
    switch (p->pixelShift) {
    case NVKMS_PIXEL_SHIFT_NONE:
    case NVKMS_PIXEL_SHIFT_4K_TOP_LEFT:
    case NVKMS_PIXEL_SHIFT_4K_BOTTOM_RIGHT:
    case NVKMS_PIXEL_SHIFT_8K:
        break;
    default:
        goto failInvalid;
    }

    /*
     * Check pixelShift feature compatibility.
     */
    if (p->pixelShift != NVKMS_PIXEL_SHIFT_NONE) {
        if (UsesWarpAndBlend(pRequestHead)) {
            goto failUnsupportedCombo;
        }

        if (pModeTimings->yuv420Mode == NV_YUV420_MODE_SW) {
            goto failUnsupportedCombo;
        }
    }


    /*
     * Validate that the requested resamplingMethod is a recognized value.
     */

    switch (p->resamplingMethod) {
    case NVKMS_RESAMPLING_METHOD_BILINEAR:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_TRIANGULAR:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_BELL_SHAPED:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_BSPLINE:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_TRIANGULAR:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BELL_SHAPED:
    case NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BSPLINE:
    case NVKMS_RESAMPLING_METHOD_NEAREST:
        break;
    default:
        goto failInvalid;
    }

    /*
     * Check resamplingMethod feature compatibility.
     *
     * The overlay, yuv420, and pixelShift headsurface shaders all texture
     * from pixel centers, relying on non-filtered input, and perform
     * bilinear filtering manually in the shader.  Reject combinations
     * of these modes with non-bilinear filtering modes.
     */
    if (p->resamplingMethod != NVKMS_RESAMPLING_METHOD_BILINEAR) {

        if (p->pixelShift != NVKMS_PIXEL_SHIFT_NONE) {
            goto failUnsupportedCombo;
        }

        if (pModeTimings->yuv420Mode == NV_YUV420_MODE_SW) {
            goto failUnsupportedCombo;
        }

        if (p->fakeOverlay) {
            goto failUnsupportedCombo;
        }
    }

    /*
     * PixelShift8k hijacks stereo; prohibit PixelShift8k with real stereo.
     */
    if ((pRequestHead->headSurface.pixelShift == NVKMS_PIXEL_SHIFT_8K) &&
        (pRequestHead->modeValidationParams.stereoMode !=
         NVKMS_STEREO_DISABLED)) {
        goto failUnsupportedCombo;
    }

    return TRUE;

failInvalid:

    pReplyHead->status = NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_HEAD_SURFACE;
    return FALSE;

failUnsupportedCombo:

    pReplyHead->status =
        NVKMS_SET_MODE_ONE_HEAD_STATUS_UNSUPPORTED_HEAD_SURFACE_COMBO;
    return FALSE;
}

/*!
 * Copy the configuration described in an NVHsChannelEvoRec to an
 * NVHsConfigOneHead.
 *
 * When changing the headSurface configuration, we build an NVHsConfig, which
 * describes the headSurface configuration across all heads on the device.
 *
 * But, the requested configuration may only specify a subset of heads, and the
 * rest of the heads are expected to preserve their current configuration.
 *
 * Use this function to copy the configuration of an existing NVHsChannelEvoRec
 * to an NVHsConfigOneHead.
 */
static void HsConfigCopyHsChannelToHsConfig(
    NVHsConfigOneHead *pHsConfigOneHead,
    const NVHsChannelEvoRec *pHsChannel)
{
    NvU8 eye, layer;

    pHsConfigOneHead->channelConfig = pHsChannel->config;

    /*
     * Initialize the surfaces to be used by pHsConfigOneHead.  We use the
     * surfaces most recently pushed on the flip queue.  NVKMS should drain the
     * current flip queue as part of applying a new headSurface configuration.
     * If the flip queue is already drained, use the surfaces in the flipqueue's
     * 'current',
     *
     * Note that we do not need to update any reference counts here: that is
     * done when the hsConfig is applied to the device by nvHsConfigApply().
     * And, in that path refcnts are always incremented for the new surfaces
     * before refcnts are decremented for the old surfaces.
     */

    /* both structures have the same number of layers */
    ct_assert(ARRAY_LEN(pHsConfigOneHead->layer) ==
              ARRAY_LEN(pHsChannel->flipQueue));

    for (layer = 0; layer < ARRAY_LEN(pHsConfigOneHead->layer); layer++) {

        const NVHsLayerRequestedFlipState *pFlipState =
            HsGetLastFlipQueueEntry(pHsChannel, layer);

        /* both structures have the same number of eyes */
        ct_assert(ARRAY_LEN(pHsConfigOneHead->layer[layer].pSurfaceEvo) ==
                  ARRAY_LEN(pFlipState->pSurfaceEvo));

        for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
            pHsConfigOneHead->layer[layer].pSurfaceEvo[eye] =
                pFlipState->pSurfaceEvo[eye];
        }
    }
}

/*!
 * Return whether the NvKmsSetModeRequest requests a change on the disp+head.
 */
static NvBool HsConfigHeadRequested(
    const struct NvKmsSetModeRequest *pRequest,
    const NvU32 dispIndex,
    const NvU32 apiHead)
{
    if ((pRequest->requestedDispsBitMask & NVBIT(dispIndex)) == 0) {
        return FALSE;
    }

    if ((pRequest->disp[dispIndex].requestedHeadsBitMask & NVBIT(apiHead)) == 0) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Get the initial NVHsConfig for the configuration.
 *
 * For each head that will be active, there are four possible states,
 * described by NVHsConfigState.
 *
 * Construct an optimistic proposed headSurface configuration,
 * setting the most hardware-dependent state per head.
 *
 * If the modeset fails with that configuration, the caller will "downgrade" the
 * configuration via nvHsConfigDowngrade() and try again.
 *
 * \param[in]   pDevEvo    The device.
 * \param[in]   pRequest   The requested configuration from the client.
 * \param[out]  pReply     The modeset reply structure.
 * \param[in]   pOpenDev   The per-open device data for the client.
 * \param[out]  pHsConfig  The NVHsConfig to populate.
 *
 * Return FALSE if the requested configuration is not possible, and assign
 * status fields in pReply as appropriate.
 */
NvBool nvHsConfigInitModeset(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsSetModeRequest *pRequest,
    struct NvKmsSetModeReply *pReply,
    const struct NvKmsPerOpenDev *pOpenDev,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;
    NvBool ret;

    nvkms_memset(pHsConfig, 0, sizeof(*pHsConfig));

    /*
     * Cache the 'commit' flag, to decide later whether to actually allocate
     * resources for this pHsConfig.
     */
    pHsConfig->commit = pRequest->commit;

    if (!nvGetAllowHeadSurfaceInNvKms(pDevEvo, pOpenDev, pRequest)) {
        return TRUE;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequest->disp[dispIndex].head[apiHead];
            struct NvKmsSetModeOneHeadReply *pReplyHead =
                &pReply->disp[dispIndex].head[apiHead];
            NVHwModeTimingsEvo *pTimings;

            NvU8 eyeMask = NVBIT(NVKMS_LEFT);
            NVHsConfigState state =
                NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE;

            const NvBool is2dHomogeneousIdentity =
                !pRequestHead->headSurface.transformSpecified ||
                Is2dHomogeneousIdentity(&pRequestHead->headSurface.transform);

            /* Skip this head if it is not specified in the request. */

            if (!HsConfigHeadRequested(pRequest, dispIndex, apiHead)) {

                /*
                 * If the head is not specified in the new request, but
                 * currently has a headSurface configuration, propagate that
                 * configuration to the new pHsConfig.
                 */
                const NVHsChannelEvoRec *pHsChannel =
                    pDispEvo->pHsChannel[apiHead];

                if (pHsChannel != NULL) {
                    HsConfigCopyHsChannelToHsConfig(
                        &pHsConfig->apiHead[dispIndex][apiHead], pHsChannel);
                }

                continue;
            }

            /* Skip this head if it is not driving any dpys. */

            if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
                continue;
            }

            if (!HsConfigValidate(pDevEvo, pRequestHead, pReplyHead)) {
                return FALSE;
            }

            pTimings =
                nvPreallocGet(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS,
                              sizeof(*pTimings));
            nvkms_memset(pTimings, 0, sizeof(*pTimings));

            if (!nvGetHwModeTimings(pDispEvo, apiHead, pRequestHead,
                                    pTimings, NULL /* pDpyColor */,
                                    NULL /* pInfoFrameCtrl */)) {
                nvPreallocRelease(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS);
                return FALSE;
            }

            if (pRequestHead->headSurface.pixelShift !=
                NVKMS_PIXEL_SHIFT_NONE) {

                if (pRequestHead->headSurface.pixelShift ==
                    NVKMS_PIXEL_SHIFT_8K) {
                    eyeMask |= NVBIT(NVKMS_RIGHT);
                }

                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
                goto done;
            }

            /*
             * If any of the stereo modes were requested, we'll need NVKMS_RIGHT
             * surfaces if we enable headSurface.
             */
            if (pRequestHead->modeValidationParams.stereoMode !=
                NVKMS_STEREO_DISABLED) {
                eyeMask |= NVBIT(NVKMS_RIGHT);
            }

            if (pTimings->yuv420Mode == NV_YUV420_MODE_SW) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
                goto done;
            }

            /*
             * XXX NVKMS HEADSURFACE TODO: should resamplingMethod only apply if
             * there is viewport scaling?
             */
            if (pRequestHead->headSurface.resamplingMethod !=
                NVKMS_RESAMPLING_METHOD_DEFAULT) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.forceFullCompositionPipeline) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.forceCompositionPipeline) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            /*
             * If Warp & Blend is attempted, transforms are bypassed. We want
             * headSurface buffers no matter what.
             */
            if (UsesWarpAndBlend(pRequestHead)) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.fakeOverlay) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.rotation != NVKMS_ROTATION_0) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.reflectionX) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            if (pRequestHead->headSurface.reflectionY) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                goto done;
            }

            if (is2dHomogeneousIdentity) {
                const NvU16 hVisible = nvEvoVisibleWidth(pTimings);
                const NvU16 vVisible = nvEvoVisibleHeight(pTimings);

                if (pTimings->viewPort.out.width != pTimings->viewPort.in.width ||
                    pTimings->viewPort.out.height != pTimings->viewPort.in.height) {
                    /*
                     * If ViewPortIn is not the size of ViewPortOut, we might need
                     * headSurface to scale.
                     */
                    state = NVKMS_HEAD_SURFACE_CONFIG_STATE_MAYBE_HEAD_SURFACE;
                } else if ((pTimings->viewPort.out.xAdjust != 0) ||
                           (pTimings->viewPort.out.yAdjust != 0) ||
                           (pTimings->viewPort.out.width != hVisible) ||
                           (pTimings->viewPort.out.height != vVisible)) {
                    /*
                     * If ViewPortOut is not the size of raster, we might need
                     * headSurface to position ViewPortOut.
                     */
                    state = NVKMS_HEAD_SURFACE_CONFIG_STATE_MAYBE_HEAD_SURFACE;

                } else {
                    /*
                     * If this is an identity transform, ViewPortIn is the
                     * same as ViewPortOut, and ViewPortOut is the same as
                     * raster, we don't need headSurface.
                     */
                    state = NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE;

                }
                goto done;
            }

            /*
             * A scaling transformation might be possible with display hardware.
             */
            if (pRequestHead->headSurface.transformSpecified &&
                IsScalingTransform(&pRequestHead->headSurface.transform)) {
                state = NVKMS_HEAD_SURFACE_CONFIG_STATE_MAYBE_HEAD_SURFACE;
                goto done;
            }

            /*
             * Otherwise, the transformation is more complicated:  fall back to
             * headSurface.
             */

            state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
done:
            ret = HsConfigInitModesetOneHead(pDevEvo,
                                             state,
                                             eyeMask,
                                             apiHead,
                                             dispIndex,
                                             pTimings,
                                             &pRequestHead->viewPortSizeIn,
                                             &pRequestHead->flip,
                                             &pRequestHead->headSurface,
                                             pOpenDev,
                                             &pHsConfig->apiHead[dispIndex][apiHead]);

            nvPreallocRelease(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS);

            if (!ret) {
                pReplyHead->status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_HEAD_SURFACE;
                return FALSE;
            }
        }
    }

    return TRUE;
}


/*!
 * Initialize pHsConfigOneHead for SwapGroup.
 *
 * In the case that headSurface is not needed for modeset, but is needed for
 * SwapGroup, initialize the given pHsConfigOneHead.
 *
 * This should parallel HsConfigInitModesetOneHead()
 */
static void HsConfigInitSwapGroupOneHead(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    NVHsConfigOneHead *pHsConfigOneHead)
{
    static const struct NvKmsSetModeHeadSurfaceParams
        nullHeadSurfaceParams = { };

    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    const NVHwModeTimingsEvo *pTimings = &pApiHeadState->timings;
    const NVHsConfigState state =
        NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
    NVHsChannelConfig *pChannelConfig = &pHsConfigOneHead->channelConfig;
    NvU32 layer;

    pChannelConfig->eyeMask = NVBIT(NVKMS_LEFT);

    if (pApiHeadState->stereo.mode != NVKMS_STEREO_DISABLED) {
        pChannelConfig->eyeMask |= NVBIT(NVKMS_RIGHT);
    }

    pChannelConfig->viewPortIn.x = pApiHeadState->viewPortPointIn.x;
    pChannelConfig->viewPortIn.y = pApiHeadState->viewPortPointIn.y;

    pChannelConfig->viewPortIn.width = pTimings->viewPort.in.width;
    pChannelConfig->viewPortIn.height = pTimings->viewPort.in.height;

    pChannelConfig->hs10bpcHint = pApiHeadState->hs10bpcHint;

    for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
        nvApiHeadGetLayerSurfaceArray(pDispEvo, apiHead, layer,
            pHsConfigOneHead->layer[layer].pSurfaceEvo);
    }

    nvApiHeadGetCursorInfo(pDispEvo, apiHead,
                           &pChannelConfig->cursor.pSurfaceEvo,
                           &pChannelConfig->cursor.x,
                           &pChannelConfig->cursor.y);

    HsConfigUpdateOneHead(state, pTimings,
                          &nullHeadSurfaceParams, pChannelConfig);
}


/*!
 * Initial NVHsConfig, applying neededForSwapGroup for the given pSwapGroup to
 * the current headSurface configuration.
 */
void nvHsConfigInitSwapGroup(
    const NVDevEvoRec *pDevEvo,
    const NVSwapGroupRec *pSwapGroup,
    const NvBool neededForSwapGroup,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(pHsConfig, 0, sizeof(*pHsConfig));

    pHsConfig->commit = TRUE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numHeads; apiHead++) {
            NvBool neededForModeset = FALSE;

            const NVHsChannelEvoRec *pHsChannel =
                pDispEvo->pHsChannel[apiHead];

            NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[dispIndex][apiHead];
            NVHsChannelConfig *pChannelConfig = &pHsConfigOneHead->channelConfig;

            /*
             * The console surface may not be set up to be the source of
             * headSurface operations, and NVKMS may be unloaded, so we can't
             * have the display rely on headSurface.
             */
            if (nvEvoIsConsoleActive(pDevEvo)) {
                continue;
            }

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            /*
             * If the head currently has a headSurface configuration, propagate
             * that to the new pHsConfig.
             */
            if (pHsChannel != NULL) {
                HsConfigCopyHsChannelToHsConfig(pHsConfigOneHead, pHsChannel);
            }

            /* If this head is not part of the SwapGroup, don't change it. */

            if (pDispEvo->pSwapGroup[apiHead] != pSwapGroup) {
                continue;
            }

            neededForModeset = pHsConfigOneHead->channelConfig.neededForModeset;

            pChannelConfig->neededForSwapGroup = neededForSwapGroup;

            /*
             * neededForModeset describes whether the current headSurface
             * configuration is enabled due to the modeset.
             *
             * neededForSwapGroup describes whether the new headSurface
             * configuration needs to be enabled due to enabling a SwapGroup.
             *
             * Update pHsConfigOneHead for the given combination of
             * neededForModeset + neededForSwapGroup.
             */

            if (neededForModeset) {

                /*
                 * HeadSurface is already enabled (it is needed for modeset),
                 * and we are toggling the neededForSwapGroup field.
                 *
                 * We should already have a pHsChannel.
                 */
                nvAssert(pHsChannel != NULL);

                /*
                 * neededForSwapGroup impacts the computation of
                 * pChannelConfig->surfaceSize, so recompute that now.
                 */
                HsConfigUpdateOneHeadSurfaceSize(pChannelConfig);

            } else {

                if (neededForSwapGroup) {

                    /*
                     * HeadSurface is not needed in the current configuration
                     * for modeset, but now it is needed for SwapGroup.
                     *
                     * We don't yet have a pHsChannel.
                     *
                     * We need to initialize pHsConfigOneHead, similar to what
                     * HsConfigInitModesetOneHead() does.
                     */
                    nvAssert(pHsChannel == NULL);

                    HsConfigInitSwapGroupOneHead(pDispEvo, apiHead,
                                                 pHsConfigOneHead);

                } else {

                    /*
                     * We have headSurface currently enabled.  However, it is
                     * not needed for modeset, and now it isn't needed for
                     * SwapGroup.
                     *
                     * Clear the pHsConfigOneHead.
                     */

                    nvAssert(pHsChannel != NULL);

                    nvkms_memset(pHsConfigOneHead, 0, sizeof(*pHsConfigOneHead));
                }
            }
        }
    }
}


/*!
 * "Downgrade" the NVHsConfig.
 *
 * The caller unsuccessfully attempted a modeset with the given NVHsConfig,
 * where the heads with state==MAYBE_HEAD_SURFACE used the display hardware
 * instead of headSurface, and the heads with state==PARTIAL_HEAD_SURFACE used
 * display hardware for ViewPortOut ==> Raster scaling.
 *
 * Demote one of the heads along the path MAYBE_HEAD_SURFACE ->
 * PARTIAL_HEAD_SURFACE -> FULL_HEAD_SURFACE, so that the caller can try the
 * modeset again.
 *
 * \param[in]      pDevEvo    The device.
 * \param[in]      pRequest   The requested configuration from the client.
 * \param[in,out]  pHsConfig  The NVHsConfig to downgrade.
 *
 * \return  TRUE if a head could be downgraded.  FALSE if there are no
 *          more heads to downgrade.
 */
NvBool nvHsConfigDowngrade(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsSetModeRequest *pRequest,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead, try;
    NVDispEvoPtr pDispEvo;

    for (try = 0; try < 2; try++) {

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

            for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                    &pRequest->disp[dispIndex].head[apiHead];
                NVHsChannelConfig *pChannelConfig =
                    &pHsConfig->apiHead[dispIndex][apiHead].channelConfig;

                NVHsConfigState state =
                    NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE;

                if (!HsConfigHeadRequested(pRequest, dispIndex, apiHead)) {
                    continue;
                }

                /* Skip this head if it is not driving any dpys. */

                if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
                    continue;
                }

                /*
                 * On the first try, downgrade from
                 * MAYBE_HEAD_SURFACE to PARTIAL_HEAD_SURFACE.
                 */
                if ((try == 0) &&
                    (pChannelConfig->state ==
                     NVKMS_HEAD_SURFACE_CONFIG_STATE_MAYBE_HEAD_SURFACE)) {

                    state = NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE;
                }

                /*
                 * On the second try, downgrade from
                 * PARTIAL_HEAD_SURFACE to FULL_HEAD_SURFACE.
                 */
                if ((try == 1) &&
                    (pChannelConfig->state ==
                     NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE)) {

                    state = NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE;
                }

                if (state != NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE) {
                    NvU32 ret;
                    const struct NvKmsSetModeHeadSurfaceParams *p =
                        &pRequest->disp[dispIndex].head[apiHead].headSurface;
                    NVHwModeTimingsEvo *pTimings =
                        nvPreallocGet(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS,
                                      sizeof(*pTimings));
                    nvkms_memset(pTimings, 0, sizeof(*pTimings));

                    if (!nvGetHwModeTimings(pDispEvo, apiHead, pRequestHead,
                                            pTimings, NULL /* pDpyColor */,
                                            NULL /* pInfoFrameCtrl */)) {
                        nvPreallocRelease(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS);
                        return FALSE;
                    }

                    ret = HsConfigUpdateOneHead(state,
                                                pTimings,
                                                p,
                                                pChannelConfig);

                    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS);

                    return ret;
                }
            }
        }
    }

    return FALSE;
}

/*!
 * Return whether the given pDevEvoHsConfig satisfies the specified eyeMask.
 */
static NvBool HsConfigEyeMasksMatch(
    const NVHsStateOneHeadAllDisps *pDevEvoHsConfig,
    const NvU8 eyeMask)
{
    NvU8 eye, buf;

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

        if ((NVBIT(eye) & eyeMask) == 0) {
            continue;
        }

        for (buf = 0; buf < pDevEvoHsConfig->surfaceCount; buf++) {
            if (pDevEvoHsConfig->surfaces[eye][buf].pSurface == NULL) {
                return FALSE;
            }
        }
    }

    return TRUE;
}


/*!
 * Return an NvKmsSize that is the maximum of sizeA and sizeB in each dimension.
 */
static inline struct NvKmsSize HsConfigGetMaxNvKmsSize(
    struct NvKmsSize sizeA,
    struct NvKmsSize sizeB)
{
    struct NvKmsSize maxSize;

    maxSize.width = NV_MAX(sizeA.width, sizeB.width);
    maxSize.height = NV_MAX(sizeA.height, sizeB.height);

    return maxSize;
}

/*!
 * Reconcile hs10bpcHint across multiple disps.
 *
 * If any disp is X2B10G10R10, use X2B10G10R10.  Otherwise, use A8R8G8B8.
 */
static enum NvKmsSurfaceMemoryFormat HsConfigGetMaxFormat(
    enum NvKmsSurfaceMemoryFormat prevFormat,
    NvBool hs10bpcHint)
{
    /*
     * prevFormat is initialized to 0; it should not collide with X2B10G10R10 or
     * A8R8G8B8
     */
    ct_assert(NvKmsSurfaceMemoryFormatX2B10G10R10 != 0);
    ct_assert(NvKmsSurfaceMemoryFormatA8R8G8B8 != 0);

    if (prevFormat == NvKmsSurfaceMemoryFormatX2B10G10R10) {
        return prevFormat;
    }

    return hs10bpcHint ?
        NvKmsSurfaceMemoryFormatX2B10G10R10 : NvKmsSurfaceMemoryFormatA8R8G8B8;
}

/*!
 * Return whether sizeA is greater than or equal to sizeB in both dimensions.
 */
static inline NvBool HsConfigNvKmsSizeIsGreaterOrEqual(
    struct NvKmsSize sizeA,
    struct NvKmsSize sizeB)
{
    return (sizeA.width >= sizeB.width) && (sizeA.height >= sizeB.height);
}


typedef struct _NVHsConfigAllocResourcesWorkArea {
    NvBool needsHeadSurface;
    NvBool neededForSwapGroup;
    NvU8 eyeMask;
    struct NvKmsSize headSurfaceSize;
    struct NvKmsSize headSurfaceStagingSize;
    enum NvKmsSurfaceMemoryFormat format;
} NVHsConfigAllocResourcesWorkArea;


/*!
 * Allocate an NVHsSurfaceRec and clear its memory on all subdevices.
 *
 * \return  NULL if there was an allocation failure.  Otherwise, return a
 *          pointer to the allocated NVHsSurfaceRec.
 */
static NVHsSurfacePtr HsConfigAllocSurfacesOneSurface(
    NVDevEvoRec *pDevEvo,
    const NVHsConfig *pHsConfig,
    const NvU32 apiHead,
    const NvBool displayHardwareAccess,
    struct NvKmsSize surfaceSize,
    const enum NvKmsSurfaceMemoryFormat format)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;
    NVHsSurfacePtr pSurface;

    pSurface = nvHsAllocSurface(pDevEvo,
                                displayHardwareAccess,
                                format,
                                surfaceSize.width,
                                surfaceSize.height);
    if (pSurface == NULL) {
        return NULL;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        const NVHsConfigOneHead *pHsConfigOneHead =
            &pHsConfig->apiHead[dispIndex][apiHead];
        NVHsChannelEvoRec *pHsChannel = pHsConfigOneHead->pHsChannel;

        if (pHsChannel != NULL) {
            const struct NvKmsRect surfaceRect = {
                .x = 0,
                .y = 0,
                .width = surfaceSize.width,
                .height = surfaceSize.height,
            };

            nvHs3dClearSurface(pHsChannel, pSurface, surfaceRect,
                               pHsConfigOneHead->channelConfig.yuv420);
        }
    }

    return pSurface;
}


/*!
 * Allocate all the surfaces need for one 'buf'.
 *
 * Only update pHsConfig if all allocations succeed.
 *
 * \return  FALSE if there was an allocation failure.  Otherwise, return TRUE
 *          and update pHsConfig to point at the new allocations.
 */
static NvBool HsConfigAllocSurfacesOneBuf(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig,
    const NvU32 apiHead,
    const NvU8 buf,
    const NVHsConfigAllocResourcesWorkArea *pWorkArea)
{
    NVHsSurfacePtr pSurface[NVKMS_MAX_EYES] = { };
    NVHsSurfacePtr pStagingSurface[NVKMS_MAX_EYES] = { };
    NvU8 eye;

    const NvBool needsStaging =
        (pWorkArea->headSurfaceStagingSize.width != 0) &&
        (pWorkArea->headSurfaceStagingSize.height != 0);

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

        if ((NVBIT(eye) & pWorkArea->eyeMask) == 0) {
            continue;
        }

        pSurface[eye] =
            HsConfigAllocSurfacesOneSurface(pDevEvo,
                                            pHsConfig,
                                            apiHead,
                                            TRUE, /* requireDisplayHardwareAccess */
                                            pWorkArea->headSurfaceSize,
                                            pWorkArea->format);
        if (pSurface[eye] == NULL) {
            goto fail;
        }

        if (needsStaging) {
            pStagingSurface[eye] =
                HsConfigAllocSurfacesOneSurface(
                    pDevEvo,
                    pHsConfig,
                    apiHead,
                    FALSE, /* displayHardwareAccess */
                    pWorkArea->headSurfaceStagingSize,
                    pWorkArea->format);
            if (pStagingSurface[eye] == NULL) {
                goto fail;
            }
        }
    }

    /* All allocations succeeded, we can safely update pHsConfig. */

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
        pHsConfig->apiHeadAllDisps[apiHead].surfaces[eye][buf].pSurface =
            pSurface[eye];
        pHsConfig->apiHeadAllDisps[apiHead].surfaces[eye][buf].pStagingSurface =
            pStagingSurface[eye];
    }

    return TRUE;

fail:

    /* Something failed; free everything.  nvHsFreeSurface(NULL) is a noop. */

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
        nvHsFreeSurface(pDevEvo, pSurface[eye]);
        nvHsFreeSurface(pDevEvo, pStagingSurface[eye]);
    }

    return FALSE;
}


/*!
 * Allocate all the surfaces needed for one head.
 *
 * \return  FALSE if there was an irrecoverable allocation failure.  Otherwise,
 *          return TRUE and update pHsConfig.
 */
static NvBool HsConfigAllocSurfacesOneHead(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig,
    const NvU32 apiHead,
    const NVHsConfigAllocResourcesWorkArea *pWorkArea)
{
    NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps =
        &pHsConfig->apiHeadAllDisps[apiHead];
    NvU8 buf;

    pHsOneHeadAllDisps->surfaceCount = 0;

    for (buf = 0; buf < NVKMS_HEAD_SURFACE_MAX_BUFFERS; buf++) {

        /*
         * HeadSurface normally double buffers its rendering and flipping
         * (NVKMS_HEAD_SURFACE_MAX_BUFFERS == 2), but in most configurations it
         * can function single-buffered if necessary (so only buf == 0 is
         * "mustHave").
         *
         * However, for SwapGroups, the two buffers are used differently, and we
         * cannot really accommodate single-buffered use.  So, both buffers are
         * "mustHave" when neededForSwapGroup.
         */
        const NvBool mustHave = (buf == 0) || pWorkArea->neededForSwapGroup;

        if (!HsConfigAllocSurfacesOneBuf(pDevEvo,
                                         pHsConfig,
                                         apiHead,
                                         buf,
                                         pWorkArea)) {
            if (mustHave) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate memory for composition pipeline");
                HsConfigFreeHeadSurfaceSurfaces(pDevEvo,
                                                pHsOneHeadAllDisps,
                                                FALSE /* surfacesReused */);
                return FALSE;
            } else {
                nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                    "Failed to allocate memory for composition pipeline; continuing with potential tearing.");
                break;
            }
        }

        pHsOneHeadAllDisps->surfaceCount++;
    }

    nvAssert(pHsOneHeadAllDisps->surfaceCount > 0);

    pHsOneHeadAllDisps->size = pWorkArea->headSurfaceSize;
    pHsOneHeadAllDisps->stagingSize = pWorkArea->headSurfaceStagingSize;

    return TRUE;
}


/*!
 * Allocate resources needed for the NVHsConfig.
 *
 * The headSurface configuration may need additional resources to be allocated.
 * Determine what resources are needed, allocate them, and track them in the
 * NVHsConfig.  We do not know if this configuration will be usable by modeset,
 * so we do not alter pDevEvo state in this function.
 *
 * This function could be called multiple times for the same NVHsConfig.  If a
 * modeset fails, the caller will call nvHsConfigFreeResources(), then
 * nvHsConfigDowngrade(), and then call this again.
 *
 * \param[in]      pDevEvo    The device.
 * \param[in,out]  pHsConfig  The NVHsConfig to allocate resources for.
 *
 * \return  TRUE if needed resources were allocated.  FALSE if resources
 *          could not be allocated.
 */
NvBool nvHsConfigAllocResources(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;

    NVHsConfigAllocResourcesWorkArea workArea[NVKMS_MAX_HEADS_PER_DISP] = { };

    /*
     * Handle SLI Mosaic: surface allocations are broadcast across
     * subdevices, so compute the maximum surface sizes needed for
     * each head on all subdevices.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVHsChannelConfig *pChannelConfig =
                &pHsConfig->apiHead[dispIndex][apiHead].channelConfig;

            /* Do we need headSurface on this head? */
            if (!StateNeedsHeadSurface(pChannelConfig->state)) {
                continue;
            }

            /*
             * XXX NVKMS HEADSURFACE TODO: perform validation of headSurface
             * here.
             */

            /*
             * If the client allocated the device with no3d, but we need
             * headSurface, fail.
             */
            if (pDevEvo->pHsDevice == NULL) {
                return FALSE;
            }

            workArea[apiHead].needsHeadSurface = TRUE;
            workArea[apiHead].eyeMask |= pChannelConfig->eyeMask;

            workArea[apiHead].neededForSwapGroup =
                workArea[apiHead].neededForSwapGroup ||
                pChannelConfig->neededForSwapGroup;

            workArea[apiHead].headSurfaceSize =
                HsConfigGetMaxNvKmsSize(workArea[apiHead].headSurfaceSize,
                                        pChannelConfig->surfaceSize);

            workArea[apiHead].headSurfaceStagingSize =
                HsConfigGetMaxNvKmsSize(workArea[apiHead].headSurfaceStagingSize,
                                        pChannelConfig->stagingSurfaceSize);
            workArea[apiHead].format =
                HsConfigGetMaxFormat(workArea[apiHead].format,
                                     pChannelConfig->hs10bpcHint);
        }
    }

    /*
     * Return early without any resource allocation if this configuration is not
     * going to be committed.
     */
    if (!pHsConfig->commit) {
        return TRUE;
    }

    /* Allocate the 3d channel where necessary. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numHeads; apiHead++) {
            NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[dispIndex][apiHead];
            NVHsChannelConfig *pChannelConfig =
                &pHsConfigOneHead->channelConfig;

            if (!StateNeedsHeadSurface(pChannelConfig->state)) {
                continue;
            }

            if (pDispEvo->pHsChannel[apiHead] != NULL) {
                /* Reuse the existing headSurface channel, if it exists. */
                pHsConfigOneHead->pHsChannel = pDispEvo->pHsChannel[apiHead];
                pHsConfigOneHead->channelReused = TRUE;
            } else {
                /* Otherwise, allocate a new channel. */
                pHsConfigOneHead->channelReused = FALSE;
                pHsConfigOneHead->pHsChannel =
                    nvHsAllocChannel(pDispEvo, apiHead);

                if (pHsConfigOneHead->pHsChannel == NULL) {
                    nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to allocate channel for composition pipeline");
                    nvHsConfigFreeResources(pDevEvo, pHsConfig);
                    return FALSE;
                }
            }
        }
    }

    /*
     * Assign NVHsConfig::apiHeadAllDisps[], and either reuse the existing surfaces
     * (if they are large enough), or allocate new surfaces.
     */
    for (apiHead = 0; apiHead < pDevEvo->numHeads; apiHead++) {
        const NVHsStateOneHeadAllDisps *pDevEvoHsConfig =
            &pDevEvo->apiHeadSurfaceAllDisps[apiHead];

        /* There should not (yet?) be any surfaces allocated for this head */
        nvAssert(pHsConfig->apiHeadAllDisps[apiHead].surfaceCount == 0);

        if (!workArea[apiHead].needsHeadSurface) {
            continue;
        }

        /*
         * If NVKMS already has sufficiently large surfaces for this head, reuse
         * them instead of allocating new ones.
         *
         * XXX NVKMS HEADSURFACE TODO: when transitioning from a large mode to a
         * small mode, this will keep the large mode's headSurface surfaces.
         * Perhaps we should not reuse the existing surfaces if they are
         * significantly larger than necessary.  Or, perhaps we should do some
         * sort of headSurface compaction after applying surfaces to the device?
         * What if the current config is tearing (surfaceCount == 1), and we
         * could upgrade to (surfaceCount == 2)?
         *
         * The same problem applies when transitioning from stereo to mono
         * (we'll leave a right eye surface allocated but unused).
         *
         * The same problem applies when transitioning from a configuration that
         * needs stagingSurfaces to a configuration that doesn't.
         */
        if ((pDevEvoHsConfig->surfaceCount > 0) &&
            HsConfigNvKmsSizeIsGreaterOrEqual(
                pDevEvoHsConfig->size,
                workArea[apiHead].headSurfaceSize) &&
            HsConfigNvKmsSizeIsGreaterOrEqual(
                pDevEvoHsConfig->stagingSize,
                workArea[apiHead].headSurfaceStagingSize) &&
            HsConfigEyeMasksMatch(pDevEvoHsConfig, workArea[apiHead].eyeMask)) {
            CopyHsStateOneHeadAllDisps(&pHsConfig->apiHeadAllDisps[apiHead],
                                       pDevEvoHsConfig);
            pHsConfig->surfacesReused[apiHead] = TRUE;
            continue;
        }

        /* Otherwise, allocate new surfaces. */

        if (!HsConfigAllocSurfacesOneHead(pDevEvo,
                                          pHsConfig,
                                          apiHead,
                                          &workArea[apiHead])) {
            nvHsConfigFreeResources(pDevEvo, pHsConfig);
            return FALSE;
        }

        /*
         * XXX NVKMS HEADSURFACE TODO: Populate the surface with the correct
         * screen contents.
         *
         * It is unclear if that is desirable behavior: if we're reusing an
         * existing headSurface surface, we shouldn't clobber existing content
         * before we flip.
         */
    }

    return TRUE;
}

/*!
 * Free resources allocated for NVHsConfig but not used by the current
 * configuration.
 *
 * nvHsConfigAllocResources() allocates resources, after first attempting to
 * reuse the current configuration's existing resources.  Those will be
 * propagated to the pDevEvo by nvHsConfigApply() if the modeset succeeds.
 *
 * However, if the modeset fails, this function needs to free everything
 * allocated by nvHsConfigAllocResources().
 *
 * \param[in]      pDevEvo    The device
 * \param[in,out]  pHsConfig  The NVHsConfigRec whose resources should be freed.
 */
void nvHsConfigFreeResources(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numHeads; apiHead++) {
            NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[dispIndex][apiHead];

            if (pHsConfigOneHead->pHsChannel == NULL) {
                continue;
            }

            if (!pHsConfigOneHead->channelReused) {
                nvHsFreeChannel(pHsConfigOneHead->pHsChannel);
            }

            pHsConfigOneHead->pHsChannel = NULL;
            pHsConfigOneHead->channelReused = FALSE;
        }
    }

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        HsConfigFreeHeadSurfaceSurfaces(pDevEvo,
                                        &pHsConfig->apiHeadAllDisps[apiHead],
                                        pHsConfig->surfacesReused[apiHead]);
        pHsConfig->surfacesReused[apiHead] = FALSE;
    }
}

/*!
 * Initialize each layer's flip queue.
 */
static void HsConfigInitFlipQueue(
    NVHsChannelEvoPtr pHsChannel,
    const NVHsConfigOneHead *pHsConfigOneHead)
{
    NvU8 layer, eye;

    /*
     * Initialize flipQueueMainLayerState with the surfaces specified in the modeset
     * request.
     */
    for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {
        pHsChannel->flipQueueMainLayerState.pSurfaceEvo[eye] =
            pHsConfigOneHead->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[eye];
    }

    /*
     * Push a single flip queue entry into each layer's flip queue, using the
     * surfaces specified in the modeset request.  Later, the nvHsNextFrame() =>
     * HsUpdateFlipQueueCurrent() call chain will pop the entry from the queue
     * and put it in "current".
     *
     * This might seem a little indirect, but we do things this way so that we
     * use common paths for reference count bookkeeping.
     */
    for (layer = 0; layer < ARRAY_LEN(pHsConfigOneHead->layer); layer++) {

        NVHsLayerRequestedFlipState hwState = { };

        nvkms_memset(&pHsChannel->flipQueue[layer], 0,
                     sizeof(pHsChannel->flipQueue[layer]));

        nvListInit(&pHsChannel->flipQueue[layer].queue);

        for (eye = 0; eye < ARRAY_LEN(hwState.pSurfaceEvo); eye++) {
            hwState.pSurfaceEvo[eye] =
                pHsConfigOneHead->layer[layer].pSurfaceEvo[eye];
        }

        nvHsPushFlipQueueEntry(pHsChannel, layer, &hwState);
    }
}

static void HsConfigUpdateSurfaceRefCount(
    NVDevEvoPtr pDevEvo,
    const NVHsChannelConfig *pChannelConfig,
    NvBool increase)
{
    HsChangeSurfaceFlipRefCount(
        pDevEvo, pChannelConfig->warpMesh.pSurface, increase);

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pChannelConfig->pBlendTexSurface, increase);

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pChannelConfig->pOffsetTexSurface, increase);

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pChannelConfig->cursor.pSurfaceEvo, increase);
}

/*!
 * Check if flipLock should be allowed on this device.
 *
 * If any head has headSurface enabled, flipLock might interfere with per-head
 * presentation, so prohibit flipLock.
 */
static NvBool HsConfigAllowFlipLock(const NVDevEvoRec *pDevEvo)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (pDispEvo->pHsChannel[apiHead] != NULL) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static void HsMainLayerFlip(
    NVHsChannelEvoPtr pHsChannel,
    NvKmsSurfaceHandle surfaceHandle[NVKMS_MAX_EYES],
    const struct NvKmsPoint viewPortPointIn,
    const struct NvKmsSetCursorImageCommonParams cursorImage,
    const struct NvKmsMoveCursorCommonParams cursorPosition)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 apiHead = pHsChannel->apiHead;
    const NvU32 sd = pDispEvo->displayOwner;
    NvBool ret;
    NvU8 eye;
    struct NvKmsFlipCommonParams *pParamsOneHead;

    /*
     * Use preallocated memory, so that we don't have to allocate
     * memory here (and deal with allocation failure).
     */
    struct NvKmsFlipRequestOneHead *pFlipHead = &pHsChannel->scratchParams;

    nvkms_memset(pFlipHead, 0, sizeof(*pFlipHead));

    pFlipHead->sd = sd;
    pFlipHead->head = apiHead;
    pParamsOneHead = &pFlipHead->flip;

    pParamsOneHead->layer[NVKMS_MAIN_LAYER].surface.specified = TRUE;

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].surface.handle[eye] = surfaceHandle[eye];
    }

    if (surfaceHandle[NVKMS_LEFT] != 0) {
        NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
            nvGetSurfaceHandlesFromOpenDev(pDevEvo->pNvKmsOpenDev);
        NVSurfaceEvoPtr pSurfaceEvo =
            nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles, surfaceHandle[NVKMS_LEFT]);

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.specified = TRUE;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val.width =
            pSurfaceEvo->widthInPixels;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val.height =
            pSurfaceEvo->heightInPixels;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeOut.specified = TRUE;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeOut.val =
            pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val;
    }

    /* clear completion notifier and sync objects */
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].completionNotifier.specified = TRUE;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.specified = TRUE;

    pParamsOneHead->viewPortIn.specified = TRUE;
    pParamsOneHead->viewPortIn.point = viewPortPointIn;

    pParamsOneHead->cursor.image = cursorImage;
    pParamsOneHead->cursor.imageSpecified = TRUE;

    pParamsOneHead->cursor.position = cursorPosition;
    pParamsOneHead->cursor.positionSpecified = TRUE;

    ret = nvFlipEvo(pDevEvo,
                    pDevEvo->pNvKmsOpenDev,
                    pFlipHead,
                    1     /* numFlipHeads */,
                    TRUE  /* commit */,
                    NULL  /* pReply */,
                    FALSE /* skipUpdate */,
                    FALSE /* allowFlipLock */);

    if (!ret) {
        nvAssert(!"headSurface main layer flip failed?");
    }
}

/*!
 * When disabling headSurface, restore the non-headSurface surface, if
 * necessary.
 *
 * HeadSurface is disabled in two paths: as part of modeset, and as part of
 * leaving a SwapGroup.  In the modeset case, we do not need to do anything
 * here: the modeset already specified the correct surface.
 *
 * But, in the case of disabling headSurface due to leaving a SwapGroup, we need
 * to restore the client's non-headSurface surface(s) (i.e., the surfaces in
 * flipQueueMainLayerState).  So, we check for the case of transitioning from a
 * configuration with:
 *
 *   neededForModeset   == FALSE
 *   neededForSwapGroup == TRUE
 *
 * to a configuration with:
 *
 *   neededForModeset   == FALSE
 *   neededForSwapGroup == FALSE
 *
 * To flip in that case, use HsMainLayerFlip() => nvFlipEvo(), which populates an
 * NvKmsFlipRequest structure.  This takes surface handles, so temporarily
 * generate NvKmsSurfaceHandles in pNvKmsOpenDev's namespace.
 */
static void HsConfigRestoreMainLayerSurface(
    NVDispEvoPtr pDispEvo,
    NVHsChannelEvoPtr pHsChannelOld,
    const NVHsChannelConfig *pHsChannelConfigNew)
{
    if (!pHsChannelOld->config.neededForModeset &&
         pHsChannelOld->config.neededForSwapGroup &&
        !pHsChannelConfigNew->neededForModeset &&
        !pHsChannelConfigNew->neededForSwapGroup) {

        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NVEvoApiHandlesRec *pNvKmsOpenDevSurfaceHandles =
            nvGetSurfaceHandlesFromOpenDev(pDevEvo->pNvKmsOpenDev);
        NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_EYES] = { };
        NvKmsSurfaceHandle surfaceHandle[NVKMS_MAX_EYES] = { };
        NvU8 eye;

        struct NvKmsSetCursorImageCommonParams cursorImage = { };

        const struct NvKmsMoveCursorCommonParams cursorPosition = {
            .x = pHsChannelOld->config.cursor.x,
            .y = pHsChannelOld->config.cursor.y,
        };

        const struct NvKmsPoint viewPortPointIn = {
            .x = pHsChannelOld->config.viewPortIn.x,
            .y = pHsChannelOld->config.viewPortIn.y,
        };

        for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

            pSurfaceEvo[eye] =
                pHsChannelOld->flipQueueMainLayerState.pSurfaceEvo[eye];

            if (pSurfaceEvo[eye] == NULL) {
                continue;
            }

            surfaceHandle[eye] =
                nvEvoCreateApiHandle(pNvKmsOpenDevSurfaceHandles,
                                     pSurfaceEvo[eye]);
        }

        if (pHsChannelOld->config.cursor.pSurfaceEvo != NULL) {

            cursorImage.surfaceHandle[NVKMS_LEFT] =
                nvEvoCreateApiHandle(pNvKmsOpenDevSurfaceHandles,
                                     pHsChannelOld->config.cursor.pSurfaceEvo);
            cursorImage.cursorCompParams.colorKeySelect =
                NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE;
            cursorImage.cursorCompParams.blendingMode[1] =
                NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
        }

        HsMainLayerFlip(
            pHsChannelOld,
            surfaceHandle,
            viewPortPointIn,
            cursorImage,
            cursorPosition);

        for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

            if (pSurfaceEvo[eye] == NULL) {
                continue;
            }

            nvEvoDestroyApiHandle(pNvKmsOpenDevSurfaceHandles,
                                  surfaceHandle[eye]);
        }

        if (cursorImage.surfaceHandle[NVKMS_LEFT] != 0) {
            nvEvoDestroyApiHandle(pNvKmsOpenDevSurfaceHandles,
                                  cursorImage.surfaceHandle[NVKMS_LEFT]);
        }
    }
}

/*!
 * Enable or disable fliplock on all channels using headsurface for swapgroups,
 * waiting for idle if necessary.
 */
static void HsConfigUpdateFlipLockForSwapGroups(NVDevEvoPtr pDevEvo,
                                                NvBool enable)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;
    NvU32 flipLockToggleApiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };
    NvBool found = FALSE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

            if (pHsChannel == NULL) {
                continue;
            }

            /*
             * This function is called in two cases, when disabling fliplock for
             * the pHsChannels in the previous config, and when enabling
             * fliplock for the pHsChannels in the new config.  In either case,
             * if the old config wasn't using fliplock for swapgroups, or the
             * new config won't be using fliplock for swapgroups, don't change
             * the fliplock state here.
             */
            if (!pHsChannel->config.neededForSwapGroup) {
                continue;
            }

            flipLockToggleApiHeadMaskPerSd[pDispEvo->displayOwner] |=
                NVBIT(apiHead);
            found = TRUE;
        }
    }

    if (!found) {
        return;
    }

    nvApiHeadUpdateFlipLock(pDevEvo, flipLockToggleApiHeadMaskPerSd, enable);
}

/*!
 * Tear down all existing headSurface configs on the device, disabling
 * fliplock, flipping base to NULL, restoring core surfaces, and releasing
 * swapgroups if necessary.
 *
 * This function is not allowed to fail: it is called after we have committed to
 * performing the modeset.
 *
 * \param[in,out]  pDevEvo    The device.
 * \param[in]      pHsConfig  The NVHsConfig that will be applied later in this
 *                            transition; this is used to decide whether to
 *                            restore the non-headSurface surface when leaving
 *                            a SwapGroup.
 */
void nvHsConfigStop(
    NVDevEvoPtr pDevEvo,
    const NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;
    NVHsDeviceEvoPtr pHsDevice = pDevEvo->pHsDevice;
    NvU32 hsDisableApiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };

    /*
     * We should only get here if this configuration is going to be committed.
     */
    nvAssert(pHsConfig->commit);

    /*
     * If fliplock was in use for headsurface swapgroups on any channel, wait
     * for those channels to be idle, applying accelerators to ignore
     * fliplock/interlock if necessary, and disable fliplock on those channels.
     */
    HsConfigUpdateFlipLockForSwapGroups(pDevEvo, FALSE /* enable */);

    /* Flip all headSurface heads to NULL. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

            if (pHsChannel != NULL) {
                hsDisableApiHeadMaskPerSd[pDispEvo->displayOwner] |= NVBIT(apiHead);

                if (pHsChannel->config.pixelShift == NVKMS_PIXEL_SHIFT_8K) {
                    nvSetStereo(pDispEvo, apiHead, FALSE);
                }

                if (pHsChannel->config.neededForSwapGroup) {
                    pHsChannel->viewportFlipPending = FALSE;
                    nvHsRemoveRgLine1Callback(pHsChannel);
                }

                nvHsRemoveVBlankCallback(pHsChannel);
                nvHsFlip(pHsDevice,
                         pHsChannel,
                         0 /* eyeMask: ignored when disabling */,
                         FALSE /* perEyeStereoFlip: ignored when disabling */,
                         0 /* index: ignored when disabling */,
                         NULL /* NULL == disable */,
                         FALSE /* isFirstFlip */,
                         FALSE /* allowFlipLock */);
            }
        }
    }

    /*
     * Wait for main layer channels to be idle on all channels that previously
     * had headSurface enabled in order to allow semaphore releases from
     * previous headSurface flips to complete. This wait should not timeout,
     * so if it does, just assert instead of forcing the channels idle.
     */
    nvApiHeadIdleMainLayerChannels(pDevEvo,
                                   hsDisableApiHeadMaskPerSd);

    /* Update bookkeeping and restore the original surface in main layer. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

            if (pHsChannel != NULL) {
                nvHsFreeStatistics(pHsChannel);
                nvHsDrainFlipQueue(pHsChannel);

                HsConfigRestoreMainLayerSurface(
                    pDispEvo,
                    pHsChannel,
                    &pHsConfig->apiHead[dispIndex][apiHead].channelConfig);
            }
        }
    }

    /*
     * At this point any active swapgroups with pending flips have completed
     * those flips (by force if necessary) and flipped back to core, so release
     * any deferred request fifos that are waiting for that pending flip to
     * complete.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVSwapGroupRec *pSwapGroup = pDispEvo->pSwapGroup[apiHead];
            if ((pSwapGroup != NULL) &&
                pSwapGroup->pendingFlip) {
                nvHsSwapGroupRelease(pDevEvo, pSwapGroup);
            }
        }
    }

    /* finally, make sure any remaining rendering commands have landed */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];
            if (pHsChannel != NULL) {
                nvPushIdleChannel(&pHsChannel->nvPush.channel);
            }
        }
    }
}

/*!
 * Apply the new NVHsConfig to the device.
 *
 * As resources are propagated from pHsConfig to pDevEvo, remove them from
 * pHsConfig, so that nvHsConfigFreeResources() can safely be called on the
 * pHsConfig.
 *
 * This function is not allowed to fail: it is called after we have committed to
 * performing the modeset.
 *
 * \param[in,out]  pDevEvo    The device.
 * \param[in,out]  pHsConfig  The NVHsConfig to apply.
 */
void nvHsConfigStart(
    NVDevEvoPtr pDevEvo,
    NVHsConfig *pHsConfig)
{
    NvU32 dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;
    NVHsDeviceEvoPtr pHsDevice = pDevEvo->pHsDevice;
    NvBool allowFlipLock;

    /*
     * We should only get here if this configuration is going to be committed.
     */
    nvAssert(pHsConfig->commit);

    /* Update channels. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[dispIndex][apiHead];

            /*
             * If we have a new configuration, increment its surface reference
             * counts.
             */
            if (pHsConfigOneHead->pHsChannel != NULL) {
                HsConfigUpdateSurfaceRefCount(
                    pDevEvo,
                    &pHsConfigOneHead->channelConfig,
                    TRUE /* increase */);
            }

            /*
             * If we have an old configuration, decrement its surface reference
             * counts.
             */
            if (pDispEvo->pHsChannel[apiHead] != NULL) {
                HsConfigUpdateSurfaceRefCount(
                    pDevEvo,
                    &pDispEvo->pHsChannel[apiHead]->config,
                    FALSE /* increase */);
            }

            /* If there is no channel before or after, continue. */

            if ((pDispEvo->pHsChannel[apiHead] == NULL) &&
                (pHsConfigOneHead->pHsChannel == NULL)) {
                continue;
            }

            /* If the channel is used before and after, continue. */

            if ((pDispEvo->pHsChannel[apiHead] != NULL) &&
                (pHsConfigOneHead->pHsChannel != NULL)) {
                nvAssert(pHsConfigOneHead->channelReused);
                nvAssert(pDispEvo->pHsChannel[apiHead] ==
                         pHsConfigOneHead->pHsChannel);
                continue;
            }

            /* Free any channels no longer needed. */

            if ((pDispEvo->pHsChannel[apiHead] != NULL) &&
                (pHsConfigOneHead->pHsChannel == NULL)) {
                nvHsFreeChannel(pDispEvo->pHsChannel[apiHead]);
                pDispEvo->pHsChannel[apiHead] = NULL;
                continue;
            }

            /*
             * Otherwise, propagate the channel configuration from pHsConfig to
             * pDispEvo.
             */
            nvAssert(pDispEvo->pHsChannel[apiHead] == NULL);
            pDispEvo->pHsChannel[apiHead] = pHsConfigOneHead->pHsChannel;
            pHsConfigOneHead->pHsChannel = NULL;
        }
    }

    /* Update surfaces. */

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

        NVHsStateOneHeadAllDisps *pDevEvoHsConfig =
            &pDevEvo->apiHeadSurfaceAllDisps[apiHead];
        NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps =
            &pHsConfig->apiHeadAllDisps[apiHead];

        /*
         * If the device is currently using headSurface on this head, but the
         * new configuration is not, free the surfaces.
         */
        if ((pDevEvoHsConfig->surfaceCount > 0) &&
            (pHsOneHeadAllDisps->surfaceCount == 0)) {

            HsConfigFreeHeadSurfaceSurfaces(pDevEvo, pDevEvoHsConfig, FALSE);
            continue;
        }

        /*
         * If the device is currently not using headSurface on this head, but
         * the new configuration is, propagate resources.
         */
        if ((pDevEvoHsConfig->surfaceCount == 0) &&
            (pHsOneHeadAllDisps->surfaceCount > 0)) {

            MoveHsStateOneHeadAllDisps(pDevEvoHsConfig, pHsOneHeadAllDisps);
            continue;
        }

        /*
         * If the device is currently using headSurface on this head, and the
         * new configuration also is, reconcile the two.
         */
        if ((pDevEvoHsConfig->surfaceCount > 0) &&
            (pHsOneHeadAllDisps->surfaceCount > 0)) {

            /*
             * If the new configuration is reusing the device's surfaces, then
             * this head is done.
             */
            if (pHsConfig->surfacesReused[apiHead]) {
                nvAssert(nvkms_memcmp(pDevEvoHsConfig, pHsOneHeadAllDisps,
                                      sizeof(*pDevEvoHsConfig)) == 0);
                continue;
            }

            /*
             * Otherwise, the new configuration had to allocate new surfaces.
             * Free the old surfaces and replace them.
             */
            HsConfigFreeHeadSurfaceSurfaces(pDevEvo, pDevEvoHsConfig, FALSE);

            MoveHsStateOneHeadAllDisps(pDevEvoHsConfig, pHsOneHeadAllDisps);
        }
    }

    /*
     * Update the flip lock prohibit/allow state, based on whether heads will
     * have headSurface enabled in the new configuration.  This will allow
     * headSurface flips to proceed independently of non-headSurface flips
     * on configurations that would otherwise implicitly enable fliplock
     * in FinishModesetOneTopology, which should allow the first flip to
     * complete below.  After that, fliplock may be enabled again for
     * headSurface swapgroups if necessary.
     */

    allowFlipLock = HsConfigAllowFlipLock(pDevEvo);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvAllowFlipLockEvo(pDispEvo, allowFlipLock);
    }

    /* Enable headSurface for the new configuration. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];
            const NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[dispIndex][apiHead];

            if (pHsChannel != NULL) {

                nvHsAddVBlankCallback(pHsChannel);

                nvHsAllocStatistics(pHsChannel);

                /* Apply the new configuration to pHsChannel. */

                pHsChannel->config = pHsConfigOneHead->channelConfig;

                if (pHsChannel->config.neededForSwapGroup) {
                    nvHsAddRgLine1Callback(pHsChannel);
                }

                /*
                 * nvHsConfigPatchSetModeRequest() used
                 * surfaces[eye][nextIndex].pSurface as the surface during
                 * modeset.  Now that we know that the modeset succeeded,
                 * increment nextIndex.
                 */
                HsIncrementNextIndex(pHsDevice, pHsChannel);

                /*
                 * Reset nextOffset: non-SwapGroup configurations rely on
                 * nextOffset being 0 to avoid rendering and flipping to an
                 * invalid configuration within a headSurface surface, and
                 * SwapGroup configurations rely on nextOffset being 0 to avoid
                 * the combination of HsIncrementNextOffset below and the
                 * first flip to offset 0 in HsFlipHelper resulting in the
                 * active offset and next offset both being 0, which would cause
                 * a hang in HsServiceRGLineInterrupt.
                 */
                pHsChannel->nextOffset = 0;

                if (pHsChannel->config.neededForSwapGroup) {
                    HsIncrementNextOffset(pHsDevice, pHsChannel);
                }

                /* Do the one-time set up of the channel. */

                nvHs3dSetConfig(pHsChannel);

                /* Render the first frame. */

                nvHsInitNotifiers(pHsDevice, pHsChannel);
                HsConfigInitFlipQueue(pHsChannel, pHsConfigOneHead);
                nvHsNextFrame(pHsDevice, pHsChannel,
                              NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME);

                if (pHsChannel->config.pixelShift == NVKMS_PIXEL_SHIFT_8K) {
                    nvSetStereo(pDispEvo, apiHead, TRUE);
                }
            }
        }
    }

    /*
     * If fliplock is necessary for headsurface swapgroups on any channel,
     * wait for idle (which shouldn't timeout since fliplock was disabled
     * above) and enable fliplock on those channels.
     */
    HsConfigUpdateFlipLockForSwapGroups(pDevEvo, TRUE /* enable */);
}

static void HsConfigPatchRequestedViewPortOneHead(
    const NVDispEvoRec *pDispEvo,
    struct NvKmsSetModeOneHeadRequest *pRequestHead,
    const NVHsConfigOneHead *pHsConfigOneHead)
{
    const NVHsChannelConfig *pChannelConfig = &pHsConfigOneHead->channelConfig;

    nvAssert(StateNeedsHeadSurface(pChannelConfig->state));

    /*
     * Patch ViewPortIn: whenever using headSurface, the display
     * hardware's ViewPortIn is the size of the headSurface frame.
     */
    pRequestHead->viewPortSizeIn.width = pChannelConfig->frameSize.width;
    pRequestHead->viewPortSizeIn.height = pChannelConfig->frameSize.height;

    /*
     * Patch ViewPortOut: if PARTIAL, then headSurface uses the display hardware
     * to do ViewPortIn => ViewPortOut scaling.  In that case, we keep the
     * client-requested ViewPortOut (i.e., change nothing here).  But, if FULL,
     * then we program the display hardware with a ViewPortOut the size of the
     * visible region of the mode: the surface, ViewPortIn, ViewPortOut, and
     * visible region are all the same size.
     */
    if (pChannelConfig->state ==
        NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE) {
        /*
         * If ViewPortOut is not specified in the request, then the ViewPortOut
         * will be programmed with the same size as the visible region of
         * the mode.
         */
        nvkms_memset(&pRequestHead->viewPortOut,
                     0, sizeof(pRequestHead->viewPortOut));
        pRequestHead->viewPortOutSpecified = FALSE;
    }
}


/*!
 * Modify the NvKmsSetModeRequest with the desired NVHsConfig.
 *
 * After calling this function, a modeset will be attempted.  If that modeset
 * fails, then the previous NvKmsSetModeRequest will be restored, and the
 * NVHsConfig will be downgraded, and the process will be tried again.
 *
 * This function is a noop if pHsConfig does not enable headSurface for any
 * heads.
 *
 * \param[in]      pDevEvo    The device
 * \param[in]      pHsConfig  The NVHsConfigRec to apply to pRequest.
 * \param[in,out]  pOpenDev   The per-open device data for the modeset client.
 * \param[in,out]  pRequest   The modeset request to be modified.
 * \param[out]     patchedApiHeadsMask[] The per sub-device mask of the api
 *                                       heads which are patched by this
 *                                       function.
 *
 * \return  TRUE if pRequest could be modified as necessary.
 *          FALSE if an error occurred and the modeset should be aborted.
 *          The patchedApiHeadsMask[] output parameter is used by
 *          nvHsConfigClearPatchedSetModeReq() to free resources like
 *          surface handles before clearing the input modeset
 *          request.
 */
NvBool nvHsConfigPatchSetModeRequest(const NVDevEvoRec *pDevEvo,
                                     const NVHsConfig *pHsConfig,
                                     struct NvKmsPerOpenDev *pOpenDev,
                                     struct NvKmsSetModeRequest *pRequest,
                                     NvU32 patchedApiHeadsMask[NVKMS_MAX_SUBDEVICES])
{
    NvU32 ret = TRUE;
    NvU32 apiHead, sd, eye;
    NVDispEvoPtr pDispEvo;
    NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDev(pOpenDev);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[sd];

        patchedApiHeadsMask[sd] = 0x0;
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVHsConfigOneHead *pHsConfigOneHead =
                &pHsConfig->apiHead[sd][apiHead];
            const NVHsChannelConfig *pChannelConfig =
                &pHsConfigOneHead->channelConfig;

            const NVHsChannelEvoRec *pHsChannel = pHsConfigOneHead->pHsChannel;

            struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];

            NvU32 layer;

            if (!HsConfigHeadRequested(pRequest, sd, apiHead)) {
                continue;
            }

            if (!StateNeedsHeadSurface(pChannelConfig->state)) {
                continue;
            }

            /* If this is a commit-ful modeset, we should have a channel. */
            if (pHsConfig->commit) {
                nvAssert(pHsChannel != NULL);
            }

            patchedApiHeadsMask[sd] |= NVBIT(apiHead);

            /*
             * XXX NVKMS HEADSURFACE TODO: update the cursor configuration as
             * necessary.
             */

            /*
             * Construct a new NvKmsFlipCommonParams request reflecting
             * pHsConfig.  This is per-disp.
             */
            nvkms_memset(&pRequestHead->flip, 0, sizeof(pRequestHead->flip));

            pRequestHead->flip.viewPortIn.specified = TRUE;
            pRequestHead->flip.viewPortIn.point.x = 0;
            pRequestHead->flip.viewPortIn.point.y = 0;

            pRequestHead->flip.cursor.imageSpecified = TRUE;
            pRequestHead->flip.cursor.positionSpecified = TRUE;

            for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                pRequestHead->flip.layer[layer].surface.specified = TRUE;
                pRequestHead->flip.layer[layer].completionNotifier.specified = TRUE;
                pRequestHead->flip.layer[layer].syncObjects.specified = TRUE;
                pRequestHead->flip.layer[layer].compositionParams.specified = TRUE;

                if (layer != NVKMS_MAIN_LAYER) {
                    continue;
                }

                pRequestHead->flip.layer[layer].csc.specified = TRUE;
                pRequestHead->flip.layer[layer].csc.matrix = NVKMS_IDENTITY_CSC_MATRIX;

                for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
                    if ((NVBIT(eye) & pChannelConfig->eyeMask) == 0) {
                        continue;
                    }

                    /* If this is a commit-ful modeset, we should have a surface. */
                    if (pHsConfig->commit) {
                        NVHsSurfaceRec *pHsSurface =
                            pHsConfig->apiHeadAllDisps[apiHead].
                            surfaces[eye][pHsChannel->nextIndex].pSurface;
                        nvAssert(pHsSurface != NULL);

                        NVSurfaceEvoRec *pSurfaceEvo =
                            nvHsGetNvKmsSurface(pDevEvo,
                                                pHsSurface->nvKmsHandle,
                                                TRUE /* requireDisplayHardwareAccess */);
                        nvAssert(pSurfaceEvo != NULL);

                        pRequestHead->flip.layer[layer].surface.handle[eye] =
                            nvEvoCreateApiHandle(pOpenDevSurfaceHandles,
                                pSurfaceEvo);
                        if (pRequestHead->flip.layer[layer].surface.handle[eye] != 0x0) {
                            nvEvoIncrementSurfaceStructRefCnt(pSurfaceEvo);
                        } else {
                            ret = FALSE;
                        }
                    } else {
                        pRequestHead->flip.layer[layer].surface.handle[eye] = 0;
                    }
                }

                if (pRequestHead->flip.layer[layer].surface.handle[NVKMS_LEFT] != 0) {
                    const NVHsSurfaceRec *pHsSurface =
                        pHsConfig->apiHeadAllDisps[apiHead].
                        surfaces[NVKMS_LEFT][pHsChannel->nextIndex].pSurface;

                    pRequestHead->flip.layer[layer].sizeIn.specified = TRUE;
                    pRequestHead->flip.layer[layer].sizeIn.val.width =
                        pHsSurface->pSurfaceEvo->widthInPixels;
                    pRequestHead->flip.layer[layer].sizeIn.val.height =
                        pHsSurface->pSurfaceEvo->heightInPixels;

                    pRequestHead->flip.layer[layer].sizeOut.specified = TRUE;
                    pRequestHead->flip.layer[layer].sizeOut.val =
                        pRequestHead->flip.layer[layer].sizeIn.val;
                }
            }

            HsConfigPatchRequestedViewPortOneHead(pDispEvo, pRequestHead,
                                                  pHsConfigOneHead);
        }
    }

    return ret;
}

void
nvHsConfigClearPatchedSetModeRequest(const NVDevEvoRec *pDevEvo,
                                     struct NvKmsPerOpenDev *pOpenDev,
                                     struct NvKmsSetModeRequest *pRequest,
                                     const NvU32 patchedApiHeadsMask[NVKMS_MAX_SUBDEVICES])
{
    NvU32 sd;
    NVDispEvoPtr pDispEvo;
    NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDev(pOpenDev);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 apiHead;
        struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[sd];

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];
            NvU32 eye;

            if ((NVBIT(apiHead) & patchedApiHeadsMask[sd]) == 0x0) {
                continue;
            }


            for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
                const NvKmsSurfaceHandle surfaceHandle =
                    pRequestHead->flip.layer[NVKMS_MAIN_LAYER].surface.handle[eye];

                if (surfaceHandle != 0x0) {
                    NVSurfaceEvoPtr pSurfaceEvo =
                        nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles,
                            surfaceHandle);
                    nvEvoDestroyApiHandle(pOpenDevSurfaceHandles, surfaceHandle);
                    nvEvoDecrementSurfaceStructRefCnt(pSurfaceEvo);
                }
            }
        }
    }

    nvkms_memset(pRequest, 0, sizeof(*pRequest));
}
