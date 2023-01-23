/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvkms-headsurface-3d.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h" /* nvVEvoLog() */
#include "nvkms-sync.h"  /* nvKmsSemaphorePayloadOffset() */
#include "nvkms-rm.h"    /* nvRmEvoClassListCheck() */

#include "nvidia-push-methods.h" /* nvPushMethod(), nvPushSet*() */
#include "nvidia-push-utils.h" /* nvPushKickoff(), nvPushGetNotifierCpuAddress() */
#include "nvkms-push.h"

#include "nv_list.h" /* nv_container_of */

#include "nvidia-3d-color-targets.h"    /* nv3dSelectColorTarget() */
#include "nvidia-3d-utils.h"            /* nv3dSetSurfaceClip() */
#include "nvidia-3d-constant-buffers.h" /* nv3dSelectCb() */
#include "nvidia-3d-shader-constants.h" /* NV3D_CB_SLOT_MISC1 */

#include "nvidia-3d-imports.h"

#include "g_nvidia-headsurface-shader-info.h" /* nvHeadSurfaceShaderInfo[] */
#include "nvidia-headsurface-types.h"

#include "nvkms-softfloat.h"

#include "nvidia-headsurface-constants.h"

#include <nv-float.h>

#include <class/cl2080.h> /* NV20_SUBDEVICE_0 */
#include <class/cl902d.h> /* FERMI_TWOD_A */
#include <class/cla06f.h> /* KEPLER_CHANNEL_GPFIFO_A */

/*
 * Define constant buffer indices used by headSurface.
 */
typedef enum {
    NVKMS_HEADSURFACE_CONSTANT_BUFFER_FRAGMENT_PROGRAM,
    NVKMS_HEADSURFACE_CONSTANT_BUFFER_VERTEX_PROGRAM,
    NVKMS_HEADSURFACE_CONSTANT_BUFFER_STATIC_WARP_MESH,
    NVKMS_HEADSURFACE_CONSTANT_BUFFER_COUNT,
} NvHs3dConstantBufferIndex;


void *nv3dImportAlloc(size_t size)
{
    return nvAlloc(size);
}

void nv3dImportFree(void *ptr)
{
    nvFree(ptr);
}

int nv3dImportMemCmp(const void *a, const void *b, size_t size)
{
    return nvkms_memcmp(a, b, size);
}

void nv3dImportMemSet(void *s, int c, size_t size)
{
    nvkms_memset(s, c, size);
}

void nv3dImportMemCpy(void *dest, const void *src, size_t size)
{
    nvkms_memcpy(dest, src, size);
}

void nv3dImportMemMove(void *dest, const void *src, size_t size)
{
    nvkms_memmove(dest, src, size);
}

NvBool nvHs3dAllocDevice(NVHsDeviceEvoRec *pHsDevice)
{
    Nv3dAllocDeviceParams params = { };

    params.pPushDevice = &pHsDevice->pDevEvo->nvPush.device;

    if (!nv3dAllocDevice(&params, &pHsDevice->nv3d.device)) {
        goto fail;
    }

    return TRUE;

fail:
    nvHs3dFreeDevice(pHsDevice);
    return FALSE;
}

void nvHs3dFreeDevice(NVHsDeviceEvoRec *pHsDevice)
{
    if (pHsDevice == NULL) {
        return;
    }

    nv3dFreeDevice(&pHsDevice->nv3d.device);
}

static void FreeNvPushChannel(NVHsChannelEvoRec *pHsChannel)
{
    const NVDispEvoRec *pDispEvo;
    NVDevEvoPtr pDevEvo;
    NvU32 h;

    if ((pHsChannel == NULL) || (pHsChannel->pDispEvo == NULL)) {
        return;
    }

    pDispEvo = pHsChannel->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    nvPushFreeChannel(&pHsChannel->nvPush.channel);

    for (h = 0; h < ARRAY_LEN(pHsChannel->nvPush.handlePool); h++) {
        if (pHsChannel->nvPush.handlePool[h] != 0) {
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pHsChannel->nvPush.handlePool[h]);
            pHsChannel->nvPush.handlePool[h] = 0;
        }
    }
}

static NvBool AllocNvPushChannel(NVHsChannelEvoRec *pHsChannel)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NvPushAllocChannelParams params = { };
    NvU32 h;

    params.engineType = NV2080_ENGINE_TYPE_GRAPHICS;
    params.logNvDiss = FALSE;
    params.noTimeout = FALSE;
    params.ignoreChannelErrors = FALSE;
    params.numNotifiers = NVKMS_HEADSURFACE_MAX_SEMAPHORES;

    /*
     * XXX NVKMS HEADSURFACE TODO: Pushbuffer memory can be used faster than
     * it's drained in complex headsurface swapgroup configurations, since
     * there's no throttling on viewportin flips being scheduled in response to
     * a vblank callback before previous rendering has completed.
     *
     * This size was raised from 8k to 128k to WAR the issue, and a proper fix
     * will be added in bug 2397492, after which this limit can be lowered
     * again.
     *
     * Throttling is now implemented using the RG line 1 interrupt headsurface
     * rendering mechanism, so this limit can be lowered once the old
     * vblank-triggered viewport flipping mechanism is removed.
     */
    params.pushBufferSizeInBytes = 128 * 1024; /* arbitrary */

    ct_assert(sizeof(params.handlePool) ==
              sizeof(pHsChannel->nvPush.handlePool));

    for (h = 0; h < ARRAY_LEN(pHsChannel->nvPush.handlePool); h++) {
        pHsChannel->nvPush.handlePool[h] =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
        params.handlePool[h] = pHsChannel->nvPush.handlePool[h];
    }

    params.pDevice = &pDevEvo->nvPush.device;

    if (!nvPushAllocChannel(&params, &pHsChannel->nvPush.channel)) {
        FreeNvPushChannel(pHsChannel);
        return FALSE;
    }

    return TRUE;
}

static void FreeNv3dChannel(NVHsChannelEvoRec *pHsChannel)
{
    const NVDispEvoRec *pDispEvo;
    NVDevEvoPtr pDevEvo;

    if ((pHsChannel == NULL) || (pHsChannel->pDispEvo == NULL)) {
        return;
    }

    pDispEvo = pHsChannel->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    nv3dFreeChannelSurface(&pHsChannel->nv3d.channel);
    nv3dFreeChannelObject(&pHsChannel->nv3d.channel);
    nv3dFreeChannelState(&pHsChannel->nv3d.channel);

    if (pHsChannel->nv3d.handle != 0) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pHsChannel->nv3d.handle);
        pHsChannel->nv3d.handle = 0;
    }
}


static NvBool AllocNv3dChannel(NVHsChannelEvoRec *pHsChannel)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    Nv3dAllocChannelStateParams stateParams = { };
    Nv3dAllocChannelObjectParams objectParams = { };

    stateParams.p3dDevice = &pDevEvo->pHsDevice->nv3d.device;
    stateParams.numTextures = NVKMS_HEADSURFACE_TEXINFO_NUM;
    stateParams.numConstantBuffers = NVKMS_HEADSURFACE_CONSTANT_BUFFER_COUNT;
    stateParams.numTextureBindings =
        NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_NUM;

    /*
     * XXX NVKMS HEADSURFACE TODO: set hasFrameBoundaries to TRUE, but how to
     * trigger a frame boundary?
     */
    stateParams.hasFrameBoundaries = FALSE;

    if (!nv3dAllocChannelState(&stateParams, &pHsChannel->nv3d.channel)) {
        goto fail;
    }

    pHsChannel->nv3d.handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    objectParams.pPushChannel = &pHsChannel->nvPush.channel;
    objectParams.handle[0] = pHsChannel->nv3d.handle;

    if (!nv3dAllocChannelObject(&objectParams, &pHsChannel->nv3d.channel)) {
        goto fail;
    }

    if (!nv3dAllocChannelSurface(&pHsChannel->nv3d.channel)) {
        goto fail;
    }

    return TRUE;

fail:
    FreeNv3dChannel(pHsChannel);
    return FALSE;
}

static void FreeNv2dChannel(NVHsChannelEvoRec *pHsChannel)
{
    const NVDispEvoRec *pDispEvo;
    NVDevEvoPtr pDevEvo;

    if ((pHsChannel == NULL) || (pHsChannel->pDispEvo == NULL)) {
        return;
    }

    pDispEvo = pHsChannel->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    if (pHsChannel->nv2d.handle != 0) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pHsChannel->nv2d.handle);
        pHsChannel->nv2d.handle = 0;
    }
}

static NvBool AllocNv2dChannel(NVHsChannelEvoRec *pHsChannel)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NvU32 ret;

    if (!nvRmEvoClassListCheck(pDevEvo, FERMI_TWOD_A)) {
        goto fail;
    }

    pHsChannel->nv2d.handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pHsChannel->nvPush.channel.channelHandle[0],
                       pHsChannel->nv2d.handle,
                       FERMI_TWOD_A,
                       NULL);

    if (ret != NVOS_STATUS_SUCCESS) {
        goto fail;
    }

    return TRUE;

fail:
    FreeNv2dChannel(pHsChannel);
    return FALSE;
}

NvBool nvHs3dAllocChannel(NVHsChannelEvoPtr pHsChannel)
{
    const NvU32 dispSdMask = NVBIT(pHsChannel->pDispEvo->displayOwner);
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    if (!AllocNvPushChannel(pHsChannel)) {
        goto fail;
    }

    if (!AllocNv3dChannel(pHsChannel)) {
        goto fail;
    }

    if (!AllocNv2dChannel(pHsChannel)) {
        goto fail;
    }

    p = &pHsChannel->nvPush.channel;

    /*
     * pHsChannel will only be used on this pDispEvo; set the channel's
     * subdevice mask to this pDispEvo's subdevice mask.
     */
    nvPushSetSubdeviceMask(p, dispSdMask);

    if (!nv3dInitChannel(&pHsChannel->nv3d.channel)) {
        goto fail;
    }

    nvPushKickoff(p);

    return TRUE;

fail:
    nvHs3dFreeChannel(pHsChannel);
    return FALSE;
}

void nvHs3dFreeChannel(NVHsChannelEvoPtr pHsChannel)
{
    if (pHsChannel == NULL) {
        return;
    }

    FreeNv2dChannel(pHsChannel);

    FreeNv3dChannel(pHsChannel);

    FreeNvPushChannel(pHsChannel);
}

/*!
 * HsSurfaceFormatTable[] defines a mapping between NvKmsSurfaceMemoryFormat
 * enum values, NV902D_SET_SRC_FORMAT values, NV9097_SET_COLOR_TARGET_FORMAT
 * values and various nvidia-3d values needed to assign an Nv3dRenderTexInfo
 * structure.
 */
static const struct {
    NvU32 cl902d;
    NvU32 cl9097;
    enum Nv3dTexHeaderComponentSizes sizes;
    enum Nv3dTexHeaderDataType dataType;
    struct {
        enum Nv3dTexHeaderSource x;
        enum Nv3dTexHeaderSource y;
        enum Nv3dTexHeaderSource z;
        enum Nv3dTexHeaderSource w;
    } source;
} HsSurfaceFormatTable[] = {

#define ENTRY(_nvKmsFmt, _cl902d, _cl9097, _nv3dSizes, _nv3dDataType, _x, _y, _z, _w) \
                                                                                      \
    [NvKmsSurfaceMemoryFormat ## _nvKmsFmt] = {                                       \
        .cl902d   = NV902D_SET_SRC_FORMAT_V_ ## _cl902d,                              \
        .cl9097   = NV9097_SET_COLOR_TARGET_FORMAT_V_ ## _cl9097,                     \
        .sizes    = NV3D_TEXHEAD_ ## _nv3dSizes,                                      \
        .dataType = NV3D_TEXHEAD_NUM_ ## _nv3dDataType,                               \
        .source.x = NV3D_TEXHEAD_IN_ ## _x,                                           \
        .source.y = NV3D_TEXHEAD_IN_ ## _y,                                           \
        .source.z = NV3D_TEXHEAD_IN_ ## _z,                                           \
        .source.w = NV3D_TEXHEAD_IN_ ## _w,                                           \
    }

    ENTRY(I8,               Y8,                  R8,                  R8,           UINT,  R, R, R, R),
    ENTRY(A1R5G5B5,         A1R5G5B5,            A1R5G5B5,            A1B5G5R5,     UNORM, B, G, R, A),
    ENTRY(X1R5G5B5,         X1R5G5B5,            X1R5G5B5,            A1B5G5R5,     UNORM, B, G, R, ONE_FLOAT),
    ENTRY(R5G6B5,           R5G6B5,              R5G6B5,              B5G6R5,       UNORM, B, G, R, ONE_FLOAT),
    ENTRY(A8R8G8B8,         A8R8G8B8,            A8R8G8B8,            A8B8G8R8,     UNORM, B, G, R, A),
    ENTRY(X8R8G8B8,         X8R8G8B8,            X8R8G8B8,            A8B8G8R8,     UNORM, B, G, R, ONE_FLOAT),
    ENTRY(A2B10G10R10,      A2B10G10R10,         A2B10G10R10,         A2B10G10R10,  UNORM, R, G, B, A),
    ENTRY(X2B10G10R10,      A2B10G10R10,         A2B10G10R10,         A2B10G10R10,  UNORM, R, G, B, ONE_FLOAT),
    ENTRY(A8B8G8R8,         A8B8G8R8,            A8B8G8R8,            A8B8G8R8,     UNORM, R, G, B, A),
    ENTRY(X8B8G8R8,         X8B8G8R8,            X8B8G8R8,            A8B8G8R8,     UNORM, R, G, B, ONE_FLOAT),
    ENTRY(R16G16B16A16,     R16_G16_B16_A16,     R16_G16_B16_A16,     R16G16B16A16, UNORM, R, G, B, A),
    ENTRY(RF32GF32BF32AF32, RF32_GF32_BF32_AF32, RF32_GF32_BF32_AF32, R32G32B32A32, FLOAT, R, G, B, A),

#undef ENTRY

};

/*!
 * Return the NV9097_SET_COLOR_TARGET_FORMAT that corresponds to the provided
 * NvKmsSurfaceMemoryFormat.
 */
static NvU32 HsGetColorTargetFormat(enum NvKmsSurfaceMemoryFormat format)
{
    nvAssert(format < ARRAY_LEN(HsSurfaceFormatTable));

    return HsSurfaceFormatTable[format].cl9097;
}

/*!
 * Given NvKmsSurfaceMemoryFormat, assign Nv3dRenderTexInfo fields:
 *
 * Nv3dRenderTexInfo::sizes
 * Nv3dRenderTexInfo::dataType
 * Nv3dRenderTexInfo::source
 */
static void AssignRenderTexInfoSizesDataTypeSource(
    enum NvKmsSurfaceMemoryFormat format,
    Nv3dRenderTexInfo *pTexInfo)
{
    nvAssert(format < ARRAY_LEN(HsSurfaceFormatTable));

    pTexInfo->sizes    = HsSurfaceFormatTable[format].sizes;
    pTexInfo->dataType = HsSurfaceFormatTable[format].dataType;
    pTexInfo->source.x = HsSurfaceFormatTable[format].source.x;
    pTexInfo->source.y = HsSurfaceFormatTable[format].source.y;
    pTexInfo->source.z = HsSurfaceFormatTable[format].source.z;
    pTexInfo->source.w = HsSurfaceFormatTable[format].source.w;
}

/*!
 * Return the NV902D_SET_SRC_FORMAT that corresponds to the provided
 * NvKmsSurfaceMemoryFormat.
 */
static NvU32 HsGet2dFormat(enum NvKmsSurfaceMemoryFormat format)
{
    nvAssert(format < ARRAY_LEN(HsSurfaceFormatTable));

    return HsSurfaceFormatTable[format].cl902d;
}

/*!
 * Assign the Nv3dRenderTexInfo, given an NVSurfaceEvoRec.
 */
static void AssignRenderTexInfo(
    const NVSurfaceEvoRec *pSurface,
    const NvBool normalizedCoords,
    const NvBool filtering,
    Nv3dRenderTexInfo *pTexInfo)
{
    nvkms_memset(pTexInfo, 0, sizeof(*pTexInfo));

    if (pSurface == NULL) {
        return;
    }

    AssignRenderTexInfoSizesDataTypeSource(pSurface->format, pTexInfo);

    if (pSurface->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        pTexInfo->texType = NV3D_TEX_TYPE_TWO_D_BLOCKLINEAR;
    } else {
        pTexInfo->texType = NV3D_TEX_TYPE_TWO_D_PITCH;
    }

    pTexInfo->offset = pSurface->gpuAddress;

    pTexInfo->normalizedCoords = normalizedCoords;
    pTexInfo->repeatType = NV3D_TEXHEAD_REPEAT_TYPE_NONE;

    pTexInfo->filtering = filtering;
    pTexInfo->pitch = pSurface->planes[0].pitch;
    pTexInfo->width = pSurface->widthInPixels;
    pTexInfo->height = pSurface->heightInPixels;


    /*
     * When NVKMS clients register surfaces, they only specify log2GobsPerBlockY
     * (not X or Z).  This should be okay for now:
     *
     * X is never non-zero: on >= FERMI the only valid GOBS_PER_BLOCK_WIDTH is
     * ONE_GOB:
     *
     * cl9097tex.h:
     * #define NV9097_TEXHEAD2_GOBS_PER_BLOCK_WIDTH                 21:19
     * #define NV9097_TEXHEAD2_GOBS_PER_BLOCK_WIDTH_ONE_GOB         0x00000000
     *
     * clc097tex.h:
     * #define NVC097_TEXHEAD_BL_GOBS_PER_BLOCK_WIDTH               MW(98:96)
     * #define NVC097_TEXHEAD_BL_GOBS_PER_BLOCK_WIDTH_ONE_GOB       0x00000000
     *
     * Z would only be non-zero for a surface with three dimensions.
     */

    pTexInfo->log2GobsPerBlock.x = 0;
    pTexInfo->log2GobsPerBlock.y = pSurface->log2GobsPerBlockY;
    pTexInfo->log2GobsPerBlock.z = 0;
}

static void HsGetYuv420Black(
    const enum NvKmsSurfaceMemoryFormat format,
    NvU32 color[4])
{
    switch(format) {
    case NvKmsSurfaceMemoryFormatI8:
        nvAssert(!"headSurface cannot render to an I8 surface");
        break;
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
        color[0] = 0x3d042108; /* 1/31 in 5bpc ==> 0.032258f */
        color[1] = 0x3d042108; /* 1/31 in 5bpc ==> 0.032258f */
        color[2] = 0x3ef7bdef; /* 15/31 in 5bpc ==> 0.483871f */
        break;
    case NvKmsSurfaceMemoryFormatR5G6B5:
        color[0] = 0x3d042108; /* 1/31 in 5bpc ==> 0.032258f */
        color[1] = 0x3d430c31; /* 3/63 in 6bpc ==> 0.047619f */
        color[2] = 0x3ef7bdef; /* 15/31 in 5bpc ==> 0.483871f */
        break;
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
        color[0] = 0x3d70f0f1; /* 15/255 in 8bpc ==> 0.058824f */
        color[1] = 0x3d70f0f1; /* 15/255 in 8bpc ==> 0.058824f */
        color[2] = 0x3efefeff; /* 127/255 in 8bpc ==> 0.498039f */
        break;
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
        color[0] = 0x3d7c3f10; /* 63/1023 in 10bpc ==> 0.061584f */
        color[1] = 0x3d7c3f10; /* 63/1023 in 10bpc ==> 0.061584f */
        color[2] = 0x3effbff0; /* 511/1023 in 10bpc ==> 0.499511f */
        break;
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        nvAssert(!"HeadSurface doesn't render to these formats");
        break;
    }

    color[3] = NV_FLOAT_ZERO;
}

void nvHs3dClearSurface(
    NVHsChannelEvoRec *pHsChannel,
    const NVHsSurfaceRec *pHsSurface,
    const struct NvKmsRect surfaceRect,
    NvBool yuv420)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    Nv3dChannelRec *p3d = &pHsChannel->nv3d.channel;
    const NVSurfaceEvoRec *pSurfaceEvo = pHsSurface->pSurfaceEvo;

    const int ct = 0;
    const NvBool blockLinear = TRUE;
    const NvU32 surfaceFormat = HsGetColorTargetFormat(pSurfaceEvo->format);
    const NvU16 x = surfaceRect.x;
    const NvU16 y = surfaceRect.y;
    const NvU16 w = surfaceRect.width;
    const NvU16 h = surfaceRect.height;

    NvU32 clearColor[4] = { 0, 0, 0, 0 };

    if (yuv420) {
        HsGetYuv420Black(surfaceFormat, clearColor);
    }

    nv3dSelectColorTarget(p3d, ct);

    nv3dSetColorTarget(p3d,
                       ct,
                       surfaceFormat,
                       pSurfaceEvo->gpuAddress,
                       blockLinear,
                       pHsSurface->gobsPerBlock,
                       pSurfaceEvo->widthInPixels,
                       pSurfaceEvo->heightInPixels);

    nv3dSetSurfaceClip(p3d, x, y, w, h);
    nv3dSetBlendColorCoefficients(p3d, NV3D_BLEND_OP_SRC, FALSE, FALSE);
    nv3dClearSurface(p3d, clearColor, x, y, w, h);

    nvPushKickoff(p);
}

/*!
 * The 3D engine can perform bilinear and nearest resampling as part of normal
 * texture usage.  But if a more complex resampling method is requested, then we
 * will need to use an appropriate headSurface fragment program.
 */
static NvBool HsIsCustomSampling(enum NvKmsResamplingMethod resamplingMethod)
{
    return (resamplingMethod != NVKMS_RESAMPLING_METHOD_BILINEAR) &&
           (resamplingMethod != NVKMS_RESAMPLING_METHOD_NEAREST);
}

/*!
 * Return the headSurface fragment program that matches the configuration
 * described by NVHsChannelConfig.
 *
 * \param[in]  pChannelConfig  The channel configuration.
 */
static ProgramName Hs3dGetFragmentProgram(
    const NVHsChannelConfig *pChannelConfig,
    const enum NvKmsPixelShiftMode pixelShift,
    const NvBool overlay)
{
    const NvBool blend = pChannelConfig->pBlendTexSurface != NULL;
    const NvBool offset = pChannelConfig->pOffsetTexSurface != NULL;
    const NvBool blendAfterWarp = pChannelConfig->blendAfterWarp;
    const NvBool yuv420 = pChannelConfig->yuv420;
    const NvBool pixelShiftEnabled = pixelShift != NVKMS_PIXEL_SHIFT_NONE;
    const NvBool customSampling =
        HsIsCustomSampling(pChannelConfig->resamplingMethod);
    const NvBool blendOffsetOrderMatters = (blend || offset);

    int i;

    for (i = 0; i < ARRAY_LEN(nvHeadSurfaceShaderInfo); i++) {
        if ((nvHeadSurfaceShaderInfo[i].blend == blend) &&
            (nvHeadSurfaceShaderInfo[i].offset == offset) &&
            (nvHeadSurfaceShaderInfo[i].overlay == overlay) &&
            (nvHeadSurfaceShaderInfo[i].yuv420 == yuv420) &&
            (nvHeadSurfaceShaderInfo[i].pixelShift == pixelShiftEnabled) &&
            (nvHeadSurfaceShaderInfo[i].customSampling == customSampling) &&
            (!blendOffsetOrderMatters ||
             (nvHeadSurfaceShaderInfo[i].blendAfterWarp == blendAfterWarp))) {

            return PROGRAM_NVIDIA_HEADSURFACE_FRAGMENT + i;
        }
    }

    nvAssert(!"Missing headSurface fragment program.");

    return PROGRAM_NVIDIA_HEADSURFACE_FRAGMENT;
}

/*!
 * Given the rest of NVHsChannelConfig, return the srcFiltering configuration.
 */
static NvBool Hs3dGetSrcFiltering(
    const NVHsChannelConfig *pChannelConfig,
    const enum NvKmsPixelShiftMode pixelShift,
    const NvBool overlay)
{
    if (overlay) {
        return FALSE;
    }

    if (pChannelConfig->yuv420) {
        return FALSE;
    }

    if (pixelShift != NVKMS_PIXEL_SHIFT_NONE) {
        return FALSE;
    }

    if (pChannelConfig->resamplingMethod == NVKMS_RESAMPLING_METHOD_NEAREST) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Load the warp mesh for this head.
 */
static void LoadStaticWarpMesh(
    NVHsChannelEvoPtr pHsChannel)
{
    const NvHsStaticWarpMesh *swm = &pHsChannel->config.staticWarpMesh;
    Nv3dChannelRec *p3d = &pHsChannel->nv3d.channel;

    /* We use a constant buffer slot to store the static warp mesh. */

    ct_assert(sizeof(*swm) <= NV3D_CONSTANT_BUFFER_SIZE);

    nv3dSelectCb(p3d, NVKMS_HEADSURFACE_CONSTANT_BUFFER_STATIC_WARP_MESH);

    nv3dLoadConstants(p3d, 0, sizeof(*swm), swm);
}

/*!
 * Initialize the TWOD object in the channel.
 */
static void Hs3dSetup2d(NVHsChannelEvoPtr pHsChannel)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    nvAssert(!p->pDevice->clientSli || p->pDevice->numSubDevices == 1);
    nvPushSetObject(p, NVA06F_SUBCHANNEL_2D, &pHsChannel->nv2d.handle);
}

/*!
 * Set up TWOD to read from the source surface.
 */
static void Hs3dSetup2dBlitSrc(
    NVHsChannelEvoPtr pHsChannel,
    const NVSurfaceEvoRec *pSrc)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    const NvU32 colorFormat = HsGet2dFormat(pSrc->format);

    if (pSrc->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        nvPushMethod(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_SRC_MEMORY_LAYOUT, 2);
        nvPushSetMethodData(p, NV902D_SET_SRC_MEMORY_LAYOUT_V_BLOCKLINEAR);
        /* NV902D_SET_SRC_BLOCK_SIZE */
        nvPushSetMethodData(p,
            NV3D_V(902D, SET_SRC_BLOCK_SIZE, DEPTH, 0) |
            NV3D_V(902D, SET_SRC_BLOCK_SIZE, HEIGHT, pSrc->log2GobsPerBlockY));
    } else {
        nvPushImmed(p, NVA06F_SUBCHANNEL_2D,
            NV902D_SET_SRC_MEMORY_LAYOUT, PITCH);
        nvPushMethod(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_SRC_PITCH, 1);
        nvPushSetMethodData(p, pSrc->planes[0].pitch);
    }
    nvPushMethod(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_SRC_WIDTH, 4);
    nvPushSetMethodData(p, pSrc->widthInPixels);
    /* NV902D_SET_SRC_HEIGHT */
    nvPushSetMethodData(p, pSrc->heightInPixels);
    /* NV902D_SET_SRC_OFFSET */
    nvPushSetMethodDataU64(p, pSrc->gpuAddress);

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_SRC_FORMAT, colorFormat);
}

/*!
 * Set up TWOD to write to the destination surface.
 */
static void Hs3dSetup2dBlitDst(
    NVHsChannelEvoPtr pHsChannel,
    const NVHsSurfaceRec *pDst)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    const NVSurfaceEvoRec *pSurfaceEvo = pDst->pSurfaceEvo;
    const NvU32 colorFormat = HsGet2dFormat(pSurfaceEvo->format);

    /* NVHsSurfaceRec surfaces are always blockLinear. */
    nvAssert(pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear);

    nvPushMethod(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_DST_MEMORY_LAYOUT, 2);
    nvPushSetMethodData(p, NV902D_SET_DST_MEMORY_LAYOUT_V_BLOCKLINEAR);

    /* NV902D_SET_DST_BLOCK_SIZE */
    nvAssert(pDst->gobsPerBlock.x == 0);
    nvPushSetMethodData(p,
        NV3D_V(902D, SET_DST_BLOCK_SIZE, DEPTH, pDst->gobsPerBlock.z) |
        NV3D_V(902D, SET_DST_BLOCK_SIZE, HEIGHT, pDst->gobsPerBlock.y));

    nvPushMethod(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_DST_WIDTH, 4);
    nvPushSetMethodData(p, pSurfaceEvo->widthInPixels);
    /* NV902D_SET_DST_HEIGHT */
    nvPushSetMethodData(p, pSurfaceEvo->heightInPixels);
    /* NV902D_SET_DST_OFFSET */
    nvPushSetMethodDataU64(p, pSurfaceEvo->gpuAddress);

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_DST_FORMAT, colorFormat);
}

/*!
 * Prepare TWOD to perform blits between surfaces.
 */
static void Hs3dSetup2dBlit(
    NVHsChannelEvoPtr pHsChannel,
    const NVSurfaceEvoRec *pSrc,
    const NVHsSurfaceRec *pDst)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    nvPushImmed(p, NVA06F_SUBCHANNEL_2D, NV902D_SET_OPERATION, SRCCOPY);

    Hs3dSetup2dBlitSrc(pHsChannel, pSrc);
    Hs3dSetup2dBlitDst(pHsChannel, pDst);
}

/*!
 * Perform a blit, using TWOD, between the surfaces defined in the previous call
 * to Hs3dSetup2dBlit().  This can be called multiple times per
 * Hs3dSetup2dBlit().
 */
static void Hs3d2dBlit(
    NVHsChannelEvoPtr pHsChannel,
    struct NvKmsPoint srcPoint,
    struct NvKmsPoint dstPoint,
    struct NvKmsSize size)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_2D,
        NV902D_SET_PIXELS_FROM_MEMORY_DST_X0, 12);
    nvPushSetMethodData(p, dstPoint.x);  /* Destination X     */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DST_Y0 */
    nvPushSetMethodData(p, dstPoint.y);  /* Destination Y     */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DST_WIDTH */
    nvPushSetMethodData(p, size.width);  /* Blit Width        */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DST_HEIGHT */
    nvPushSetMethodData(p, size.height); /* Blit Height       */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_FRAC */
    nvPushSetMethodData(p, 0x0);         /* du/dx fraction    */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DU_DX_INT */
    nvPushSetMethodData(p, 0x1);         /* du/dx int         */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_FRAC */
    nvPushSetMethodData(p, 0x0);         /* dv/dy fraction    */
    /* NV902D_SET_PIXELS_FROM_MEMORY_DV_DY_INT */
    nvPushSetMethodData(p, 0x1);         /* dv/dy int         */
    /* NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_FRAC */
    nvPushSetMethodData(p, 0x0);         /* Source X fraction */
    /* NV902D_SET_PIXELS_FROM_MEMORY_SRC_X0_INT */
    nvPushSetMethodData(p, srcPoint.x);  /* Source X int      */
    /* NV902D_SET_PIXELS_FROM_MEMORY_SRC_Y0_FRAC */
    nvPushSetMethodData(p, 0x0);         /* Source Y fraction */
    /* NV902D_PIXELS_FROM_MEMORY_SRC_Y0_INT */
    nvPushSetMethodData(p, srcPoint.y);  /* Source Y int      */
}

/*!
 * Initialize the pHsChannel for rendering.
 *
 * This should be called once, when the headSurface configuration is applied to
 * the device.  Here we do any work that is static for the given headSurface
 * configuration.
 *
 * This function cannot fail.
 *
 * \param[in,out]  pHsChannel        The channel to update.
 */
void nvHs3dSetConfig(NVHsChannelEvoPtr pHsChannel)
{
    LoadStaticWarpMesh(pHsChannel);

    nvkms_memset(pHsChannel->nv3d.texInfo, 0,
                 sizeof(pHsChannel->nv3d.texInfo));

    /* Set up sampler from blend surface. */

    AssignRenderTexInfo(
        pHsChannel->config.pBlendTexSurface,
        TRUE /* normalizedCoords */,
        TRUE /* filtering */,
        &pHsChannel->nv3d.texInfo[NVKMS_HEADSURFACE_TEXINFO_BLEND]);

    /* Set up sampler from offset surface. */

    AssignRenderTexInfo(
        pHsChannel->config.pOffsetTexSurface,
        TRUE /* normalizedCoords */,
        TRUE /* filtering */,
        &pHsChannel->nv3d.texInfo[NVKMS_HEADSURFACE_TEXINFO_OFFSET]);

    Hs3dSetup2d(pHsChannel);
}

static NvU32 HsGetSatCos(NvS32 dvc)
{
    // Digital vibrance is between -1024 (NV_EVO_DVC_MIN) and 1023
    // (NV_EVO_DVC_MAX), normalized to 0.0f-2.0f for this shader,
    // defaulting to 1.0f. This mimics nvSetDVCEvo().
    //   (dvc + 1024) / 1024.0f
    const NvU32 a = NV_MAX(dvc + 1024, 0);
    const float32_t b = ui32_to_f32(a);
    const float32_t c = ui32_to_f32(1024);
    const float32_t d = f32_div(b, c);

    return F32viewAsNvU32(d);
}

/*!
 * Load the fragment program uniforms needed for the headSurface configuration.
 */
static void LoadFragmentProgramUniforms(
    NVHsChannelEvoPtr pHsChannel,
    const enum NvKmsPixelShiftMode pixelShift,
    const NvBool overlay,
    const struct NvKmsPoint viewPortPointIn)
{
    const NVHsChannelConfig *pChannelConfig = &pHsChannel->config;
    const NvBool pixelShiftEnabled = pixelShift != NVKMS_PIXEL_SHIFT_NONE;
    const NvBool customSampling =
        HsIsCustomSampling(pChannelConfig->resamplingMethod);

    NvHsFragmentUniforms fragmentUniforms = { };

    /* XXX NVKMS HEADSURFACE TODO: plumb colorRange  */
    const enum NvKmsDpyAttributeColorRangeValue colorRange =
        NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;

    const NvBool needsFragmentUniforms =
        pChannelConfig->blendAfterWarp ||
        pChannelConfig->yuv420 ||
        overlay ||
        pixelShiftEnabled ||
        customSampling;

    if (!needsFragmentUniforms) {
        return;
    }

    if (customSampling) {

#if defined(DEBUG)
        /*
         * The customSampling calculation above should ensure that
         * pChannelConfig->resamplingMethod is one of the following values.  And
         * the enums and constants should be defined such that they have the
         * same values.
         */
        NvBool found = FALSE;

#define CHECK(_x)                                                     \
        do {                                                          \
            ct_assert(NVKMS_RESAMPLING_METHOD_ ## _x ==               \
                      NVIDIA_HEADSURFACE_RESAMPLING_METHOD_ ## _x);   \
            if (pChannelConfig->resamplingMethod ==                   \
                NVKMS_RESAMPLING_METHOD_ ## _x) {                     \
                found = TRUE;                                         \
            }                                                         \
        } while (0)

        CHECK(BICUBIC_TRIANGULAR);
        CHECK(BICUBIC_BELL_SHAPED);
        CHECK(BICUBIC_BSPLINE);
        CHECK(BICUBIC_ADAPTIVE_TRIANGULAR);
        CHECK(BICUBIC_ADAPTIVE_BELL_SHAPED);
        CHECK(BICUBIC_ADAPTIVE_BSPLINE);

#undef CHECK

        nvAssert(found);
#endif /* DEBUG */

        fragmentUniforms.resamplingMethod = pChannelConfig->resamplingMethod;
    }

    if (pChannelConfig->blendAfterWarp) {
        fragmentUniforms.vertexScale.x = pChannelConfig->viewPortOut.width;
        fragmentUniforms.vertexScale.y = pChannelConfig->viewPortOut.height;
    }

    // The following uniforms are all necessary for the LUT and colorRange
    // to be applied in the headSurface shader for the overlay, YUV420, or
    // pixelshift mode.
    if (pChannelConfig->yuv420 || overlay || pixelShiftEnabled) {

        const int nPaletteEntries = 0; /* XXX NVKMS HEADSURFACE TODO */
        const int depth = 24; /* XXX NVKMS HEADSURFACE TODO */

        fragmentUniforms.numLutEntries.x = nPaletteEntries;
        fragmentUniforms.numLutEntries.y = nPaletteEntries;
        fragmentUniforms.numLutEntries.z = nPaletteEntries;

        // In depth 16 (R5G6B5) the LUT has half as many entries for red
        // and blue as it does for green.
        if (depth == 16) {
            nvAssert(fragmentUniforms.numLutEntries.x % 2 == 0);
            fragmentUniforms.numLutEntries.x /= 2;
            nvAssert(fragmentUniforms.numLutEntries.z % 2 == 0);
            fragmentUniforms.numLutEntries.z /= 2;
        }
    }

    if (pChannelConfig->yuv420 || pixelShiftEnabled) {
        int i, j;

        // primaryTextureBias is used in the fragment shader as well in
        // YUV420/pixelshift modes in order to map YUV420/pixelshift
        // transformed fragment coordinates to the viewport, since the
        // vertex shader's transformation is skipped in YUV420/pixelshift
        // modes.

        fragmentUniforms.primaryTextureBias.x = viewPortPointIn.x;
        fragmentUniforms.primaryTextureBias.y = viewPortPointIn.y;

        fragmentUniforms.cursorPosition.x = pChannelConfig->cursor.x +
                                            fragmentUniforms.primaryTextureBias.x;
        fragmentUniforms.cursorPosition.y = pChannelConfig->cursor.y +
                                            fragmentUniforms.primaryTextureBias.y;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                fragmentUniforms.transform[i][j] =
                    F32viewAsNvU32(pChannelConfig->transform.m[i][j]);
            }
        }
    }

    // In pixelshift mode, a 2x width/height source surface is
    // downsampled to the destination surface.  This tells the shader
    // whether to copy the upper left or bottom right pixels of each
    // pixel quad for the destination surface.
    if (pixelShift == NVKMS_PIXEL_SHIFT_4K_TOP_LEFT) {
        fragmentUniforms.pixelShiftOffset.x = NV_FLOAT_NEG_QUARTER;
        fragmentUniforms.pixelShiftOffset.y = NV_FLOAT_NEG_QUARTER;
    } else if (pixelShift == NVKMS_PIXEL_SHIFT_4K_BOTTOM_RIGHT) {
        fragmentUniforms.pixelShiftOffset.x = NV_FLOAT_QUARTER;
        fragmentUniforms.pixelShiftOffset.y = NV_FLOAT_QUARTER;
    } else {
        // When we get here, we should only see pixelShift of 4k or none (not
        // 8k): 8k should be translated into different 4k configs for each eye,
        // higher in the call chain.
        nvAssert(pixelShift == NVKMS_PIXEL_SHIFT_NONE);
    }

    if (pChannelConfig->yuv420) {
        // Since YUV 4:2:0 is only currently supported on 4K modes, hardcode
        // the ITU-R BT.709 colorspace conversion matrix.
        // The following 5 coefficients are copied from items 4.2 and 4.3 of
        // Rec. ITU-R BT.709-5.
        fragmentUniforms.luminanceCoefficient.x = 0x3e59b3d0; /* R : 0.2126f */
        fragmentUniforms.luminanceCoefficient.y = 0x3f371759; /* G : 0.7152f */
        fragmentUniforms.luminanceCoefficient.z = 0x3d93dd98; /* B : 0.0722f */
        fragmentUniforms.chromaCoefficient.x    = 0x3f228f5c; /* Cr: 0.6350f */
        fragmentUniforms.chromaCoefficient.y    = 0x3f09f55a; /* Cb: 0.5389f */

        // Range compression is disabled in HW and applied manually in the
        // shader for YUV420 mode.
        if (colorRange == NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL) {
            fragmentUniforms.luminanceScale = NV_FLOAT_ONE;
            fragmentUniforms.luminanceBlackLevel = NV_FLOAT_ZERO;
            fragmentUniforms.chrominanceScale = NV_FLOAT_ONE;
        } else if (colorRange == NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED) {
            // ITU-R BT.709 mandates a limited color range with the
            // following 8bpc quantization values (section 6.10):
            //
            // Luminance black level 16
            // Luminance nominal range 16-235 (i.e., range of 219)
            // Chrominance nominal range 16-240 (i.e., range of 224)
            //
            // Divide these values by 255.0f to normalize to the range 0.0f-1.0f
            // for the yuv420 shader's color range quantization.

            // 219.0f / 255.0f == 0.858824f ==> 0x3f5bdbdc
            fragmentUniforms.luminanceScale = 0x3f5bdbdc;

            //  16.0f / 255.0f == 0.062745f ==> 0x3d808081
            fragmentUniforms.luminanceBlackLevel = 0x3d808081;

            // 224.0f / 255.0f == 0.878431f ==> 0x3f60e0e1
            fragmentUniforms.chrominanceScale = 0x3f60e0e1;
        } else {
            nvAssert(!"Invalid color range");
        }
        // Chrominance black level is 128 for both full and limited range.
        // Divide by 255.0f to normalize to the range 0.0f-1.0f.
        // 128.0f / 255.0f == 0.501961 ==> 0x3f008081
        fragmentUniforms.chrominanceBlackLevel = 0x3f008081;

        fragmentUniforms.satCos = HsGetSatCos(pChannelConfig->dvc);

        // Default digital vibrance is 1024, from an input dvc of 0.  If
        // dvc is not default, useSatHue tells the shader to use the
        // modified satCos.
        if (pChannelConfig->dvc != 0) {
            fragmentUniforms.useSatHue = 1;
        }
    }

    nv3dSelectCb(&pHsChannel->nv3d.channel,
                 NVKMS_HEADSURFACE_CONSTANT_BUFFER_FRAGMENT_PROGRAM);
    nv3dBindCb(&pHsChannel->nv3d.channel, NV3D_HW_BIND_GROUP_FRAGMENT,
               NV3D_CB_SLOT_MISC1, TRUE);
    nv3dLoadConstants(&pHsChannel->nv3d.channel, 0,
                      sizeof(fragmentUniforms), &fragmentUniforms);
}

/*!
 * Load the vertex program uniforms needed for the headSurface configuration.
 */
static void LoadVertexProgramUniforms(
    NVHsChannelEvoPtr pHsChannel,
    const struct NvKmsPoint viewPortPointIn)
{
    const NVHsChannelConfig *pChannelConfig = &pHsChannel->config;
    Nv3dChannelRec *p3d = &pHsChannel->nv3d.channel;
    NvHsVertexUniforms uniforms = { };

    /* Scale incoming vertices by the output resolution. */
    uniforms.vertexScale.x = pChannelConfig->viewPortOut.width;
    uniforms.vertexScale.y = pChannelConfig->viewPortOut.height;

    /* Map primary texture coordinates to the display viewport. */

    uniforms.primaryTextureScale.x = pChannelConfig->viewPortIn.width;
    uniforms.primaryTextureScale.y = pChannelConfig->viewPortIn.height;

    uniforms.primaryTextureBias.x = viewPortPointIn.x;
    uniforms.primaryTextureBias.y = viewPortPointIn.y;

    uniforms.cursorPosition.x = pChannelConfig->cursor.x +
                                uniforms.primaryTextureBias.x;
    uniforms.cursorPosition.y = pChannelConfig->cursor.y +
                                uniforms.primaryTextureBias.y;

    /* Bind the constant buffer. */
    nv3dSelectCb(p3d, NVKMS_HEADSURFACE_CONSTANT_BUFFER_VERTEX_PROGRAM);
    nv3dBindCb(p3d, NV3D_HW_BIND_GROUP_VERTEX, NV3D_CB_SLOT_MISC1, TRUE);

    /* Upload the uniforms in it. */
    nv3dLoadConstants(p3d, 0, sizeof(uniforms), &uniforms);
}

/*!
 * Assign the 'textures' texture binding indices array.
 *
 * The array should have NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_NUM
 * elements.
 */
static void AssignTextureBindingIndices(
    const NVHsChannelEvoRec *pHsChannel,
    const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD],
    int *textures)
{
    const struct {
        enum NVHsChannelTexInfoEnum texture;
        const NVSurfaceEvoRec *pSurface;
    } textureTable[NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_NUM] = {

#define ENTRY(_binding, _texinfo, _surface)                                   \
        [NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_ ## _binding ## _TEX] = { \
            .texture = NVKMS_HEADSURFACE_TEXINFO_ ## _texinfo,                \
            .pSurface = _surface,                                             \
        }

        ENTRY(PRIMARY, SRC, pSurfaceEvo[NVKMS_MAIN_LAYER]),
        ENTRY(CURSOR, CURSOR, pHsChannel->config.cursor.pSurfaceEvo),
        ENTRY(BLEND, BLEND, pHsChannel->config.pBlendTexSurface),
        ENTRY(OFFSET, OFFSET, pHsChannel->config.pOffsetTexSurface),
        ENTRY(OVERLAY, OVERLAY, pSurfaceEvo[NVKMS_OVERLAY_LAYER]),

#undef ENTRY
    };

    NvU32 i;

    for (i = 0; i < ARRAY_LEN(textureTable); i++) {
        if (textureTable[i].pSurface != NULL) {
            textures[i] = textureTable[i].texture;
        } else {
            textures[i] = NV3D_TEXTURE_INDEX_INVALID;
        }
    }
}

/*!
 * Get the warp mesh data to use to draw headSurface.
 *
 * If the client specified a warpMesh surface, we use that.  Otherwise, use the
 * static warp mesh computed earlier.
 */
static void GetWarpMeshData(
    const NVHsChannelEvoPtr pHsChannel,
    Nv3dStreamSurfaceRec *pStreamSurf,
    NvU32 *pOp,
    NvU32 *pVertexCount)
{
    const NvU32 opTable[] = {
        [NVKMS_WARP_MESH_DATA_TYPE_TRIANGLES_XYUVRQ] =
            NV3D_C(9097, BEGIN, OP, TRIANGLES),
        [NVKMS_WARP_MESH_DATA_TYPE_TRIANGLE_STRIP_XYUVRQ] =
            NV3D_C(9097, BEGIN, OP, TRIANGLE_STRIP),
    };

    const NVSurfaceEvoRec *pSurface = pHsChannel->config.warpMesh.pSurface;
    Nv3dChannelRec *p3d = &pHsChannel->nv3d.channel;

    enum NvKmsWarpMeshDataType dataType;

    nvkms_memset(pStreamSurf, 0, sizeof(*pStreamSurf));

    if (pSurface != NULL) {

        pStreamSurf->gpuAddress = pSurface->gpuAddress;
        pStreamSurf->size = pSurface->planes[0].rmObjectSizeInBytes;
        dataType = pHsChannel->config.warpMesh.dataType;
        *pVertexCount = pHsChannel->config.warpMesh.vertexCount;

    } else {

        pStreamSurf->gpuAddress =
            nv3dGetConstantBufferGpuAddress(p3d,
                NVKMS_HEADSURFACE_CONSTANT_BUFFER_STATIC_WARP_MESH);
        pStreamSurf->size = NV3D_CONSTANT_BUFFER_SIZE;
        dataType = NVKMS_WARP_MESH_DATA_TYPE_TRIANGLE_STRIP_XYUVRQ;
        *pVertexCount = 4;
    }

    nvAssert((dataType == NVKMS_WARP_MESH_DATA_TYPE_TRIANGLES_XYUVRQ) ||
             (dataType == NVKMS_WARP_MESH_DATA_TYPE_TRIANGLE_STRIP_XYUVRQ));

    *pOp = opTable[dataType];
}

/*!
 * Program the WindowOffset method.
 */
static void HsSetWindowOffset(
    NVHsChannelEvoRec *pHsChannel,
    NvS16 x, NvS16 y)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_WINDOW_OFFSET_X, 2);
    nvPushSetMethodData(p, x);
    nvPushSetMethodData(p, y);
}

/*!
 * Return a pointer to the semaphore structure for this headSurface channel and
 * semaphore index.
 *
 * Note we abuse the nvidia-push library's notifier infrastructure as a
 * convenient mechanism to allocate and map memory for us.  NvNotification and
 * NvGpuSemaphore structures are the same size, so this works out.
 */
static inline NvGpuSemaphore *Hs3dGetSemaphore(
    const NVHsChannelEvoRec *pHsChannel,
    NvU8 index)
{
    const NvPushChannelRec *pChannel = &pHsChannel->nvPush.channel;
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;

    NvNotification *pNotifier =
        nvPushGetNotifierCpuAddress(pChannel, index, sd);

    ct_assert(sizeof(NvNotification) == sizeof(NvGpuSemaphore));

    return (NvGpuSemaphore *) pNotifier;
}

#if NVKMS_PROCFS_ENABLE
/*!
 * Get the semaphore index (e.g., to be passed into
 * nvPushGetNotifierGpuAddress()) for an (eye, slot) pair.
 */
static inline NvU8 Hs3dGetStatisticsSemaphoreIndex(const NvU8 eye,
                                                   const NvU8 slot)
{
    nvAssert((eye == NVKMS_LEFT) || (eye == NVKMS_RIGHT));
    nvAssert(slot < NVKMS_HEADSURFACE_STATS_MAX_SLOTS);

    const NvU8 maxSemsPerEye = NVKMS_HEAD_SURFACE_STATS_SEMAPHORE_STAGE_COUNT;
    const NvU8 maxSemsPerSlot = maxSemsPerEye * NVKMS_MAX_EYES;

    const NvU8 index = (eye * maxSemsPerEye) + (slot * maxSemsPerSlot);

    nvAssert(index < NVKMS_HEADSURFACE_STATS_MAX_SEMAPHORES);

    return index;
}

/*!
 * Return the 64-bit nanosecond timeStamp from the given NvGpuSemaphore.
 */
static inline NvU64 Hs3dGetSemaphoreTime(const NvGpuSemaphore *pSemaphore)
{
    const NvU64 low = pSemaphore->timeStamp.nanoseconds[0];
    const NvU64 high = pSemaphore->timeStamp.nanoseconds[1];

    return (high << 32) | low;
}

static void Hs3dStatisticsReleaseSemaphore(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 semIndex,
    const NvU64 nFrames)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;

    const NvU64 gpuAddress = nvPushGetNotifierGpuAddress(p, semIndex, sd);

    /*
     * Use the current frame number as the payload, so we can assert that we're
     * inspecting the right, and correctly released, semaphores when computing
     * deltaTime below.
     */
    const NvU32 payload = NvU64_LO32(nFrames);

    const NvU32 operation =
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, OPERATION, RELEASE) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, RELEASE,
               AFTER_ALL_PRECEEDING_WRITES_COMPLETE) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, STRUCTURE_SIZE, FOUR_WORDS)|
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, FLUSH_DISABLE, TRUE) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, PIPELINE_LOCATION, ALL);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_REPORT_SEMAPHORE_A, 4);
    nvPushSetMethodDataU64(p, gpuAddress);
    nvPushSetMethodData(p, payload);
    nvPushSetMethodData(p, operation);
}

static void Hs3dStatisticsComputeFps(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 eye,
    const NvU8 slot,
    const NvU64 currentTime)
{
    NVHsChannelStatisticsOneEyeRec *pPerEye =
        &pHsChannel->statistics.perEye[eye][slot];

    NvU64 elapsedTime;
    NvU32 elapsedTimeMs;

    if (pPerEye->fps.startTime == 0) {
        pPerEye->fps.startTime = currentTime;
    }

    pPerEye->fps.nFrames++;

    elapsedTime = currentTime - pPerEye->fps.startTime;

    /*
     * To maintain precision without floating point math, convert
     * the time from nanoseconds to milliseconds (divide by 1000000)
     * and multiply nFrames by 1000000.  This yields frames per
     * milliseconds.
     */
    elapsedTimeMs = (elapsedTime + 500000) / 1000000;

    if (elapsedTimeMs > 5000) { /* 5 seconds */

        pPerEye->fps.framesPerMs =
            (pPerEye->fps.nFrames * 1000000) / elapsedTimeMs;

        pPerEye->fps.nFrames = 0;
        pPerEye->fps.startTime = currentTime;
    }
}

#endif /* NVKMS_PROCFS_ENABLE */

/*!
 * Collect statistics on headSurface rendering.
 *
 * Hs3dStatisticsBefore() should be called before pushing the frame's rendering
 * methods, and Hs3dStatisticsAfter() should be called after pushing the frame's
 * rendering methods.  Both times, we push methods to do a 3D engine semaphore
 * release.
 *
 * Also, in the "Before" case, we look at the semaphores that were released
 * during the previous frame (we assume that, by the time we are here for Frame
 * N, the rendering and semaphore releases completed for Frame N-1), and compute
 * the time between Frame N-1's "before" and "after" semaphore releases.
 */


static void Hs3dStatisticsBefore(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 eye,
    const NvU8 slot)
{
#if NVKMS_PROCFS_ENABLE
    NVHsChannelStatisticsOneEyeRec *pPerEye =
        &pHsChannel->statistics.perEye[eye][slot];

    const NvU8 semIndex = Hs3dGetStatisticsSemaphoreIndex(eye, slot);

    if (pPerEye->nFrames == 0) {
        goto done;
    }

    /* Compute the statistics for the previous frame. */

    const NvU32 prevPayload = NvU64_LO32(pPerEye->nFrames - 1);

    const NvU8 beforeIndex =
        semIndex + NVKMS_HEADSURFACE_STATS_SEMAPHORE_BEFORE;
    const NvU8 afterIndex =
        semIndex + NVKMS_HEADSURFACE_STATS_SEMAPHORE_AFTER;

    NvGpuSemaphore *pBefore = Hs3dGetSemaphore(pHsChannel, beforeIndex);
    NvGpuSemaphore *pAfter = Hs3dGetSemaphore(pHsChannel, afterIndex);

    const NvU64 beforeTime = Hs3dGetSemaphoreTime(pBefore);
    const NvU64 afterTime = Hs3dGetSemaphoreTime(pAfter);
    const NvU64 deltaTime = afterTime - beforeTime;

    /*
     * The payload for the before and after semaphores should be the
     * previous frame number unless (a) we're looking at the wrong
     * semaphores or (b) the semaphores weren't released yet.
     */
    nvAssert(pBefore->data[0] == prevPayload);
    nvAssert(pAfter->data[0] == prevPayload);
    (void)prevPayload;

    if (afterTime < beforeTime) {
        nvEvoLogDispDebug(pHsChannel->pDispEvo, EVO_LOG_ERROR,
                          "Hs3dStatisticsBefore(): "
                          "afterTime (%" NvU64_fmtu " nsec) < "
                          "beforeTime (%" NvU64_fmtu " nsec)",
                          afterTime, beforeTime);
    }

    pPerEye->gpuTimeSpent += deltaTime;

    /*
     * Compute the frames per second of headSurface for this eye + slot, so that
     * we can take advantage of the nanosecond time extracted above from the
     * released semaphores.
     */
    Hs3dStatisticsComputeFps(pHsChannel, eye, slot, beforeTime);

done:
    Hs3dStatisticsReleaseSemaphore(
        pHsChannel,
        semIndex + NVKMS_HEADSURFACE_STATS_SEMAPHORE_BEFORE,
        pPerEye->nFrames);

#endif /* NVKMS_PROCFS_ENABLE */
}

static void Hs3dStatisticsAfter(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 eye,
    const NvU8 slot)
{
#if NVKMS_PROCFS_ENABLE
    NVHsChannelStatisticsOneEyeRec *pPerEye =
        &pHsChannel->statistics.perEye[eye][slot];
    const NvU8 semIndex = Hs3dGetStatisticsSemaphoreIndex(eye, slot);

    Hs3dStatisticsReleaseSemaphore(
        pHsChannel,
        semIndex + NVKMS_HEADSURFACE_STATS_SEMAPHORE_AFTER,
        pPerEye->nFrames);

    pPerEye->nFrames++;

#endif /* NVKMS_PROCFS_ENABLE */
}

/*!
 * Return the semaphore value showing the viewport offset for the most recently
 * completed frame of non-swapgroup headsurface rendering.
 */
NvU32 nvHs3dLastRenderedOffset(NVHsChannelEvoPtr pHsChannel)
{
    const NvU8 semIndex = NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX;
    const NvGpuSemaphore *sema = Hs3dGetSemaphore(pHsChannel, semIndex);

    return sema->data[0];
}

/*!
 * Push a semaphore write of the viewport offset used for the previous frame
 * of non-swapgroup headsurface rendering to
 * NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX followed by a
 * non-stall interrupt.
 */
void nvHs3dPushPendingViewportFlip(NVHsChannelEvoPtr pHsChannel)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;
    const NvU8 semIndex = NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX;

    const NvU64 gpuAddress = nvPushGetNotifierGpuAddress(p, semIndex, sd);
    const NvU32 payload = pHsChannel->nextOffset;
    const NvU32 semaphoreOperation =
        DRF_DEF(A06F, _SEMAPHORED, _OPERATION, _RELEASE) |
        DRF_DEF(A06F, _SEMAPHORED, _RELEASE_WFI, _DIS) |
        DRF_DEF(A06F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE);

    nvAssert(!pHsChannel->viewportFlipPending);

    pHsChannel->viewportFlipPending = TRUE;

    nvPushMethod(p, 0, NVA06F_SEMAPHOREA, 4);
    nvPushSetMethodDataU64(p, gpuAddress);
    nvPushSetMethodData(p, payload);
    nvPushSetMethodData(p, semaphoreOperation);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NVA06F_NON_STALL_INTERRUPT, 1);
    nvPushSetMethodData(p, 0);
    nvPushKickoff(p);

}

struct NvHs3dRenderFrameWorkArea {

    struct {
        NvBool enabled;
        NvBool honorSwapGroupClipList;

        struct {
            const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD];
        } src;

        struct {
            const NVHsSurfaceRec *pHsSurface;
        } dst;

    } staging;

    struct {

        struct {
            const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD];
            struct NvKmsPoint viewPortPointIn;
        } src;

        struct {
            const NVHsSurfaceRec *pHsSurface;
        } dst;

    } threeD;
};

/*!
 * Assign the NvHs3dRenderFrameWorkArea structure.
 *
 * Whether SwapGroup is enabled impacts the nvHs3dRenderFrame() pipeline.
 */
static NvBool Hs3dAssignRenderFrameWorkArea(
    const NVHsChannelEvoRec *pHsChannel,
    const NvBool honorSwapGroupClipList,
    const NvU8 dstEye,
    const NvU8 dstBufferIndex,
    const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD],
    struct NvHs3dRenderFrameWorkArea *pWorkArea)
{
    const NVDevEvoRec *pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    const NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps =
        &pDevEvo->apiHeadSurfaceAllDisps[pHsChannel->apiHead];

    const NVHsSurfaceRec *pSurface;
    const struct NvKmsPoint origin = { .x = 0, .y = 0};

    NvU8 layer;

    nvkms_memset(pWorkArea, 0, sizeof(*pWorkArea));

    nvAssert(dstEye < ARRAY_LEN(pHsOneHeadAllDisps->surfaces));
    nvAssert(dstBufferIndex < ARRAY_LEN(pHsOneHeadAllDisps->surfaces[dstEye]));

    pSurface = pHsOneHeadAllDisps->surfaces[dstEye][dstBufferIndex].pSurface;

    if (pSurface == NULL) {
        return FALSE;
    }

    /* Assign the src and dst of the operations needed by headSurface. */

    /*
     * We only need the staging buffer if there are both swapgroup and
     * non-swapgroup content on the screen which we can see from the
     * cliplist.
     *
     * NOTE: We probably should assert here that viewPortIn minus swapgroup
     * clip list equals viewPortIn. The client already does the same,
     * communicates that to NVKMS and that is cached in the
     * swapGroupIsFullscreen flag.
     */

    const NVSwapGroupRec *pSwapGroup =
        pHsChannel->pDispEvo->pSwapGroup[pHsChannel->apiHead];

    if (pHsChannel->config.neededForSwapGroup &&
        pSwapGroup &&
        !pSwapGroup->swapGroupIsFullscreen) {

        const NVHsSurfaceRec *pStagingSurface =
            pHsOneHeadAllDisps->surfaces[dstEye][dstBufferIndex].pStagingSurface;
        nvAssert(pStagingSurface != NULL);

        /*
         * The 'staging' operation uses the client-provided surfaces as src, and
         * uses the staging surface as dst.
         */

        pWorkArea->staging.enabled = TRUE;
        pWorkArea->staging.honorSwapGroupClipList = honorSwapGroupClipList;

        ct_assert(ARRAY_LEN(pWorkArea->staging.src.pSurfaceEvo) ==
                    NVKMS_MAX_LAYERS_PER_HEAD);

        for (layer = 0; layer < NVKMS_MAX_LAYERS_PER_HEAD; layer++) {
            pWorkArea->staging.src.pSurfaceEvo[layer] = pSurfaceEvo[layer];
        }
        pWorkArea->staging.dst.pHsSurface = pStagingSurface;

        /*
         * The 'threeD' operation uses the staging surface as src, and the
         * headSurface surface as dst.
         */

        pWorkArea->threeD.src.pSurfaceEvo[NVKMS_MAIN_LAYER] =
            pStagingSurface->pSurfaceEvo;
        pWorkArea->threeD.src.viewPortPointIn = origin;
        pWorkArea->threeD.dst.pHsSurface = pSurface;

    } else {

        /* Disable the 'staging' operation. */

        pWorkArea->staging.enabled = FALSE;

        /*
         * The 'threeD' operation uses client-provided surfaces as src, and the
         * headSurface surface as dst.
         */

        ct_assert(ARRAY_LEN(pWorkArea->threeD.src.pSurfaceEvo) ==
                    NVKMS_MAX_LAYERS_PER_HEAD);

        for (layer = 0; layer < NVKMS_MAX_LAYERS_PER_HEAD; layer++) {
            pWorkArea->threeD.src.pSurfaceEvo[layer] = pSurfaceEvo[layer];
        }
        pWorkArea->threeD.src.viewPortPointIn.x = pHsChannel->config.viewPortIn.x;
        pWorkArea->threeD.src.viewPortPointIn.y = pHsChannel->config.viewPortIn.y;

        pWorkArea->threeD.dst.pHsSurface = pSurface;
    }

    return TRUE;
}

/*!
 * Get the clip list for the SwapGroup associated with this pHsChannel.  If
 * there is no client-specified clip list, use the entire viewPortIn as the clip
 * list.
 *
 * \param[in]   pHsChannel  The channel in use.
 * \param[in]   honorSwapGroupClipList
 *                          Whether the returned clip list should honor 
 *                          the SwapGroup's current clip list.
 * \param[out]  pNClips     The number of rects in the returned clip list.
 * \param[out]  ppClipList  The returned clip list.
 */
static void Hs3dUpdateStagingSurfaceGetClipList(
    const NVHsChannelEvoRec *pHsChannel,
    const NvBool honorSwapGroupClipList,
    NvU16 *pNClips,
    const struct NvKmsRect **ppClipList)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    const NVSwapGroupRec *pSwapGroup;

    nvAssert(pDispEvo != NULL);

    pSwapGroup = pDispEvo->pSwapGroup[pHsChannel->apiHead];

    nvAssert(pSwapGroup != NULL);

    if (pSwapGroup->pClipList == NULL ||
        !honorSwapGroupClipList) {

        *pNClips = 1;
        *ppClipList = &pHsChannel->config.viewPortIn;
    } else {
        *pNClips = pSwapGroup->nClips;
        *ppClipList = pSwapGroup->pClipList;
    }
}

/*!
 * Intersect two rects and return the resulting rect.
 *
 * To do this, convert from NvKmsRect {x,y,width,height} to {x0,x1,y0,y1}, and
 * take the max of the x0 values and the min of the x1 values.  E.g.,
 *
 * Ax0   Ax1
 *  +-----+
 *  | Bx0 | Bx1
 *  |  +--+--+
 *  |  |  |  |
 *  +--+--+  |
 *     |     |
 *     +-----+
 *     .  .
 *     .  .
 *   Cx0  .   = MAX(Ax0, Bx0)
 *        Cx1 = MIN(Ax1, Bx1)
 *
 * Note that the NvKmsRects are expected to describe regions within the NvU16
 * coordinate space.  I.e., x1 = x + width should not overflow 16 bits.  The
 * headSurface ViewPortIn has this property, and nvHsSetSwapGroupClipList()
 * guarantees this for the client-specified clip list.
 *
 * If intersection is empty, e.g.,
 *
 * Ax0  Ax1  Bx0  Bx1
 *  +----+    +----+
 *  |    |    |    |
 *  +----+    +----+
 *       .    .
 *       .    .
 *       .   Cx0 = MAX(Ax0, Bx0)
 *      Cx1      = MIN(Ax1, Bx1)
 *
 * return a rect with width=0, height=0.
 */
static struct NvKmsRect Hs3dIntersectRects(
    const struct NvKmsRect rectA,
    const struct NvKmsRect rectB)
{
    struct NvKmsRect rect = { };

    const NvU16 Ax1 = rectA.x + rectA.width;
    const NvU16 Ay1 = rectA.y + rectA.height;

    const NvU16 Bx1 = rectB.x + rectB.width;
    const NvU16 By1 = rectB.y + rectB.height;

    const NvU16 Cx0 = NV_MAX(rectA.x, rectB.x);
    const NvU16 Cy0 = NV_MAX(rectA.y, rectB.y);

    const NvU16 Cx1 = NV_MIN(Ax1, Bx1);
    const NvU16 Cy1 = NV_MIN(Ay1, By1);

    nvAssert(!A_plus_B_greater_than_C_U16(rectA.x, rectA.width, NV_U16_MAX));
    nvAssert(!A_plus_B_greater_than_C_U16(rectA.y, rectA.height, NV_U16_MAX));

    nvAssert(!A_plus_B_greater_than_C_U16(rectB.x, rectB.width, NV_U16_MAX));
    nvAssert(!A_plus_B_greater_than_C_U16(rectB.y, rectB.height, NV_U16_MAX));

    if ((Cx0 >= Cx1) || (Cy0 >= Cy1)) {
        /* Disjoint; return empty rect. */
        return rect;
    }

    rect.x = Cx0;
    rect.y = Cy0;
    rect.width = Cx1 - Cx0;
    rect.height = Cy1 - Cy0;

    return rect;
}

/*!
 * Compute the parameters to perform a (non-scaling) blit from client-specified
 * input surface to the headSurface staging surface.
 *
 * \param[in]   clipRect    In source surface coordinate space.
 * \param[in]   viewPortIn  In source surface coordinate space.
 * \param[out]  pSrcPoint   In source surface coordinate space.
 * \param[out]  pDstPoint   In dest surface coordinate space.
 * \param[out]  pSize       Size of rect to blit.
 *
 * \return  Return FALSE if clipRect and viewPortIn have an empty intersection.
 *          Otherwise, return TRUE and assign the [out] params.
 */
static NvBool Hs3dUpdateStagingSurfaceGetBlitParams(
    const struct NvKmsRect clipRect,
    const struct NvKmsRect viewPortIn,
    struct NvKmsPoint *pSrcPoint,
    struct NvKmsPoint *pDstPoint,
    struct NvKmsSize *pSize)
{
    const struct NvKmsRect rect = Hs3dIntersectRects(clipRect, viewPortIn);

    if (rect.width == 0 || rect.height == 0) {
        return FALSE;
    }

    pSize->width = rect.width;
    pSize->height = rect.height;

    pSrcPoint->x = rect.x;
    pSrcPoint->y = rect.y;

    /*
     * pDstPoint is in the staging surface, which is viewPortIn-sized; position
     * pDstPoint relative to viewPortIn.
     */

    nvAssert(rect.x >= viewPortIn.x);
    pDstPoint->x = rect.x - viewPortIn.x;

    nvAssert(rect.y >= viewPortIn.y);
    pDstPoint->y = rect.y - viewPortIn.y;

    return TRUE;
}

/*!
 * If the staging surface is enabled, copy the content from the client-provided
 * surfaces into the staging surface.
 *
 * \param[in,out]  pHsChannel  The channel to use for rendering.
 * \param[in]      pWorkArea   The description of what surfaces to use for src
 *                             and dst of the TWOD blits.
 */
static void Hs3dUpdateStagingSurface(
    NVHsChannelEvoPtr pHsChannel,
    const struct NvHs3dRenderFrameWorkArea *pWorkArea)
{
    NvU16 i, nClips = 0;
    const struct NvKmsRect *pClipList = NULL;

    if (!pWorkArea->staging.enabled) {
        return;
    }

    nvAssert(pWorkArea->staging.dst.pHsSurface != NULL);

    if (pWorkArea->staging.src.pSurfaceEvo[NVKMS_MAIN_LAYER] == NULL) {
        return;
    }

    Hs3dUpdateStagingSurfaceGetClipList(
        pHsChannel,
        pWorkArea->staging.honorSwapGroupClipList,
        &nClips,
        &pClipList);

    Hs3dSetup2dBlit(pHsChannel,
                    pWorkArea->staging.src.pSurfaceEvo[NVKMS_MAIN_LAYER],
                    pWorkArea->staging.dst.pHsSurface);

    for (i = 0; i < nClips; i++) {

        struct NvKmsPoint srcPoint = { };
        struct NvKmsPoint dstPoint = { };
        struct NvKmsSize size = { };

        if (!Hs3dUpdateStagingSurfaceGetBlitParams(
                pClipList[i], pHsChannel->config.viewPortIn,
                &srcPoint, &dstPoint, &size)) {
            continue;
        }

        Hs3d2dBlit(pHsChannel, srcPoint, dstPoint, size);
    }
}

/*!
 * Render a headSurface frame.
 *
 * \param[in,out]  pHsChannel      The channel to use for rendering.
 * \param[in]      requestType     The type of headSurface frame to render.
 * \param[in]      honorSwapGroupClipList
 *                                 Whether to clip the rendering against
 *                                 the SwapGroup's current clip list.
 * \param[in]      dstEye          The NVKMS_{LEFT,RIGHT} to which we are
 *                                 rendering.
 * \param[in]      dstBufferIndex  The index of the buffer to render into.
 * \param[in]      pixelShift      The pixelShift configuration to use.
 * \param[in]      destRect        The region of pDest to render into.
 * \param[in]      pSurfaceEvo     The surfaces to read from.
 *
 * \return  Return TRUE if there was a headSurface buffer to render into.
 *          Return FALSE if no headSurface buffer was present.
 */
NvBool nvHs3dRenderFrame(
    NVHsChannelEvoPtr pHsChannel,
    const NvHsNextFrameRequestType requestType,
    const NvBool honorSwapGroupClipList,
    const NvU8 dstEye,
    const NvU8 dstBufferIndex,
    const enum NvKmsPixelShiftMode pixelShift,
    const struct NvKmsRect destRect,
    const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD])
{
    Nv3dChannelRec *p3d = &pHsChannel->nv3d.channel;
    NvBool useOverlay;
    const Nv3dVertexAttributeInfoRec attribs[] = {
        NV3D_ATTRIB_ENTRY(POSITION,  DYNAMIC, 2_32_FLOAT),
        NV3D_ATTRIB_ENTRY(TEXCOORD0, DYNAMIC, 4_32_FLOAT),
        NV3D_ATTRIB_END,
    };

    NvBool srcFiltering;
    ProgramName fragmentProgram;

    Nv3dStreamSurfaceRec streamSurf;
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;
    NvU32 op, vertexCount;

    int textures[NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_NUM] = { };

    const NvU8 statisticsSlot =
        Hs3dStatisticsGetSlot(pHsChannel, requestType,
                              dstBufferIndex, honorSwapGroupClipList);

    struct NvHs3dRenderFrameWorkArea workArea;

    if (!Hs3dAssignRenderFrameWorkArea(pHsChannel,
                                       honorSwapGroupClipList,
                                       dstEye,
                                       dstBufferIndex,
                                       pSurfaceEvo, &workArea)) {
        return FALSE;
    }

    useOverlay = (workArea.threeD.src.pSurfaceEvo[NVKMS_OVERLAY_LAYER] != NULL);
    srcFiltering =
        Hs3dGetSrcFiltering(&pHsChannel->config, pixelShift, useOverlay);
    fragmentProgram =
        Hs3dGetFragmentProgram(&pHsChannel->config, pixelShift, useOverlay);

    Hs3dStatisticsBefore(pHsChannel, dstEye, statisticsSlot);

    Hs3dUpdateStagingSurface(pHsChannel, &workArea);

    AssignTextureBindingIndices(pHsChannel, workArea.threeD.src.pSurfaceEvo,
                                textures);

    /* Set up sampler from source surfaces. */

    AssignRenderTexInfo(
        workArea.threeD.src.pSurfaceEvo[NVKMS_MAIN_LAYER],
        FALSE /* normalizedCoords */,
        srcFiltering,
        &pHsChannel->nv3d.texInfo[NVKMS_HEADSURFACE_TEXINFO_SRC]);

    AssignRenderTexInfo(
        pHsChannel->config.cursor.pSurfaceEvo,
        FALSE /* normalizedCoords */,
        srcFiltering,
        &pHsChannel->nv3d.texInfo[NVKMS_HEADSURFACE_TEXINFO_CURSOR]);

    AssignRenderTexInfo(
        workArea.threeD.src.pSurfaceEvo[NVKMS_OVERLAY_LAYER],
        FALSE /* normalizedCoords */,
        srcFiltering,
        &pHsChannel->nv3d.texInfo[NVKMS_HEADSURFACE_TEXINFO_OVERLAY]);

    /* XXX NVKMS HEADSURFACE TODO: sampler from LUT */

    /* Set up the source textures. */

    nv3dLoadTextures(p3d, 0 /* first texture */,
                     pHsChannel->nv3d.texInfo,
                     ARRAY_LEN(pHsChannel->nv3d.texInfo));

    nv3dBindTextures(p3d, fragmentProgram, textures);

    /*
     * Set up the destination.
     *
     * Note: we rely on nvHs3dClearSurface() for setting up the color target and
     * surface clip.
     */
    nvHs3dClearSurface(pHsChannel, workArea.threeD.dst.pHsSurface,
                       destRect, pHsChannel->config.yuv420);

    HsSetWindowOffset(pHsChannel, destRect.x, destRect.y);

    /* Load the vertex shader. */
    nv3dLoadProgram(p3d, PROGRAM_NVIDIA_HEADSURFACE_VERTEX);

    /* Load the fragment shader. */
    nv3dLoadProgram(p3d, fragmentProgram);

    /*
     * Load vertex and fragment program uniforms.
     *
     * XXX NVKMS HEADSURFACE TODO: the inputs that influence the program
     * uniforms /could/ change from one frame to the next, but in the steady
     * state they won't.  Should we add tracking to only reload the program
     * uniforms when their inputs change?
     */

    LoadFragmentProgramUniforms(pHsChannel, pixelShift, useOverlay,
                                workArea.threeD.src.viewPortPointIn);

    LoadVertexProgramUniforms(pHsChannel, workArea.threeD.src.viewPortPointIn);

    /* Get the mesh data to use for this frame. */

    GetWarpMeshData(pHsChannel, &streamSurf, &op, &vertexCount);

    /* Draw the frame of headSurface using a vertex array. */

    nv3dVasSetup(p3d, attribs, &streamSurf);
    nv3dVasBegin(p3d, op);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_VERTEX_ARRAY_START, 2);
    nvPushSetMethodData(p, 0);
    nvPushSetMethodData(p, vertexCount);

    nv3dVasEnd(p3d);

    Hs3dStatisticsAfter(pHsChannel, dstEye, statisticsSlot);

    nvPushKickoff(p);

    return TRUE;
}

/*!
 * Use the graphics channel to release the described semaphore.
 *
 * \param[in,out] pHsChannel    The channel to use for the release.
 * \param[in]     pSurfaceEvo   The semaphore surface.
 * \param[in]     nIsoFormat    The NISO format of the surface.
 * \param[in]     offsetInWords The offset to the semaphore within the surface.
 * \param[in]     payload       The payload to write to the semaphore.
 * \param[in]     allPreceedingReads  Whether to wait for preceding
 *                              reads or writes.
 */
void nvHs3dReleaseSemaphore(
    NVHsChannelEvoPtr pHsChannel,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const enum NvKmsNIsoFormat nIsoFormat,
    const NvU16 offsetInWords,
    const NvU32 payload,
    const NvBool allPreceedingReads)
{
    NvPushChannelPtr p = &pHsChannel->nvPush.channel;

    const NvU32 payloadByteOffsetInSemaphore =
        nvKmsSemaphorePayloadOffset(nIsoFormat) * 4;

    const NvU64 gpuAddress =
        pSurfaceEvo->gpuAddress +
        (offsetInWords * 4) +
        payloadByteOffsetInSemaphore;

    const NvU32 afterAllPreceedingReadsOrWrites =
        allPreceedingReads ?
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, RELEASE,
               AFTER_ALL_PRECEEDING_READS_COMPLETE) :
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, RELEASE,
               AFTER_ALL_PRECEEDING_WRITES_COMPLETE);

    const NvU32 operation =
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, OPERATION, RELEASE) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, STRUCTURE_SIZE, ONE_WORD) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, FLUSH_DISABLE, TRUE) |
        NV3D_C(9097, SET_REPORT_SEMAPHORE_D, PIPELINE_LOCATION, ALL) |
        afterAllPreceedingReadsOrWrites;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_REPORT_SEMAPHORE_A, 4);
    nvPushSetMethodDataU64(p, gpuAddress);
    nvPushSetMethodData(p, payload);
    nvPushSetMethodData(p, operation);

    nvPushKickoff(p);
}
