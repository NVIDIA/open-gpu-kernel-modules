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

#ifndef __NVKMS_HEADSURFACE_CONFIG_H__
#define __NVKMS_HEADSURFACE_CONFIG_H__

#include "nvkms-types.h"
#include "nvkms-softfloat.h"          /* NvKmsMatrixF32 */
#include "nvidia-headsurface-types.h" /* NvHsStaticWarpMesh */
#include "g_shader_names.h"           /* ProgramName */

typedef enum {
    /*
     * The head config does not need headSurface; there would be
     * nothing headSurface could do to make the configuration
     * achievable.
     */
    NVKMS_HEAD_SURFACE_CONFIG_STATE_NO_HEAD_SURFACE,

    /*
     * The head config might be achievable in hardware, but
     * headSurface might help if we fail to program it in hardware.
     */
    NVKMS_HEAD_SURFACE_CONFIG_STATE_MAYBE_HEAD_SURFACE,

    /*
     * HeadSurface is needed, but display hardware should still be used for
     * ViewPortOut positioning within Raster.  I.e., from a display hardware
     * standpoint, ViewPortIn == ViewPortOut, but ViewPortOut!=Raster.
     */
    NVKMS_HEAD_SURFACE_CONFIG_STATE_PARTIAL_HEAD_SURFACE,

    /*
     * HeadSurface is needed, and should also be used for ViewPortOut
     * scaling.  I.e., from a display hardware standpoint, ViewPortIn
     * == ViewPortOut, and ViewPortOut == Raster.
     */
    NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE,
} NVHsConfigState;

/*
 * Configuration state to be used by NVHsChannelEvoRec.
 */
typedef struct {

    NVHsConfigState state;

    /*
     * Which eyes are required by this headSurface configuration.  Valid
     * combinations are:
     *
     * NVBIT(NVKMS_LEFT), or
     * NVBIT(NVKMS_LEFT) | NVBIT(NVKMS_RIGHT)
     */
    NvU8 eyeMask;

    /*
     * The dimensions of headSurface frames.  This is only assigned if ::state
     * is PARTIAL_HEAD_SURFACE or FULL_HEAD_SURFACE.
     *
     * If state is PARTIAL_HEAD_SURFACE, then frameSize will be the size of the
     * display engine's viewPortOut.  Any difference between the
     * client-requested viewPortOut and the active raster region will be
     * resolved by the display engine.  Also, NVHsChannelConfig::viewPortOut
     * will equal frameSize.
     *
     * If state is FULL_HEAD_SURFACE, then frameSize will be the same size as
     * the display engine's active raster region.  Any difference between the
     * client-requested viewPortOut and the active raster region will be
     * resolved by headSurface.  NVHsChannelConfig::viewPortOut might be smaller
     * than frameSize.
     */
    struct NvKmsSize frameSize;

    /*
     * The size of the surface for headSurface.
     *
     * Normally, ::surfaceSize will be equal to ::frameSize.  However, when
     * SwapGroup is enabled, surfaceSize is bloated to twice ::frameSize.
     *
     * Note that the actual vidmem surface might be allocated larger than
     * surfaceSize: the video memory allocation is the maximum of the
     * surfaceSize for this head across all subdevices.
     */
    struct NvKmsSize surfaceSize;

    /*
     * When SwapGroup is enabled, we need a staging surface to perform
     * screen-aligned 2d blits to assemble frames of SwapGroup and non-SwapGroup
     * content, prior to applying headSurface transformations.
     *
     * When SwapGroup is not enabled ::stagingSurfaceSize will be zero size.
     */
    struct NvKmsSize stagingSurfaceSize;

    /* The region within the headSurface surfaces, where the image should be
       rendered. */
    struct NvKmsRect viewPortOut;

    /* The region headSurface will read from. */
    struct NvKmsRect viewPortIn;

    NvBool hs10bpcHint;
    NvBool yuv420;
    NvBool blendAfterWarp;

    enum NvKmsPixelShiftMode pixelShift;
    enum NvKmsResamplingMethod resamplingMethod;
    struct NvKmsMatrixF32 transform;
    NvHsStaticWarpMesh staticWarpMesh;

    /*
     * Note that any NVSurfaceEvoPtr's stored here require special attention
     * for reference counting.  See, e.g., HsConfigUpdateSurfaceRefCount().
     */

    NVSurfaceEvoPtr pBlendTexSurface;
    NVSurfaceEvoPtr pOffsetTexSurface;

    struct {
        NVSurfaceEvoPtr pSurface;
        NvU32 vertexCount;
        enum NvKmsWarpMeshDataType dataType;
    } warpMesh;

    NVFlipCursorEvoHwState cursor;

    /* XXX NVKMS HEADSURFACE TODO: plumb through dvc  */
    NvS32 dvc;

    NvBool neededForModeset;
    NvBool neededForSwapGroup;

} NVHsChannelConfig;

typedef struct {

    NVHsChannelEvoPtr pHsChannel;
    NvBool channelReused;

    NVHsChannelConfig channelConfig;

    /* The initial surfaces to read from when initializing headSurface. */
    struct {
        NVSurfaceEvoPtr pSurfaceEvo[NVKMS_MAX_EYES];
    } layer[NVKMS_MAX_LAYERS_PER_HEAD];
} NVHsConfigOneHead;

typedef struct {
    /* State that is per-disp, per-api-head. */
    NVHsConfigOneHead apiHead[NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP];

    /* State that is per-head, but spans subdevices. */
    NVHsStateOneHeadAllDisps apiHeadAllDisps[NVKMS_MAX_HEADS_PER_DISP];

    /*
     * Whether apiHeadAllDisps[]::surface[] were reused from the current
     * configuration.
     *
     * surfacesReused[] is indexed by the api heads.
     */
    NvBool surfacesReused[NVKMS_MAX_HEADS_PER_DISP];

    /*
     * Whether the modeset asked to apply the requested configuration, or only
     * test if the requested configuration is valid.  I.e.,
     * NvKmsSetModeParams::request::commit.
     */
    NvBool commit;

} NVHsConfig;

NvBool nvHsConfigInitModeset(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsSetModeRequest *pRequest,
    struct NvKmsSetModeReply *pReply,
    const struct NvKmsPerOpenDev *pOpenDev,
    NVHsConfig *pHsConfig);

void nvHsConfigInitSwapGroup(
    const NVDevEvoRec *pDevEvo,
    const NVSwapGroupRec *pSwapGroup,
    const NvBool neededForSwapGroup,
    NVHsConfig *pHsConfig);

NvBool nvHsConfigDowngrade(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsSetModeRequest *pRequest,
    NVHsConfig *pHsConfig);

NvBool nvHsConfigAllocResources(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig);

void nvHsConfigFreeResources(
    NVDevEvoRec *pDevEvo,
    NVHsConfig *pHsConfig);

void nvHsConfigStop(
    NVDevEvoPtr pDevEvo,
    const NVHsConfig *pHsConfig);

void nvHsConfigStart(
    NVDevEvoPtr pDevEvo,
    NVHsConfig *pHsConfig);

NvBool nvHsConfigPatchSetModeRequest(const NVDevEvoRec *pDevEvo,
                                     const NVHsConfig *pHsConfig,
                                     struct NvKmsPerOpenDev *pOpenDev,
                                     struct NvKmsSetModeRequest *pRequest,
                                     NvU32 patchedHeadsMask[NVKMS_MAX_SUBDEVICES]);
void
nvHsConfigClearPatchedSetModeRequest(const NVDevEvoRec *pDevEvo,
                                     struct NvKmsPerOpenDev *pOpenDev,
                                     struct NvKmsSetModeRequest *pRequest,
                                     const NvU32 patchedHeadsMask[NVKMS_MAX_SUBDEVICES]);

#endif /* __NVKMS_HEADSURFACE_CONFIG_H__ */
