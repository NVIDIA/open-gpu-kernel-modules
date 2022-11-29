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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-3d-types-priv.h"
#include "nvidia-3d-fermi.h"
#include "nvidia-3d.h"
#include "nvidia-3d-imports.h"
#include "nvidia-3d-constant-buffers.h"
#include "nvidia-3d-shader-constants.h"
#include "nvidia-3d-vertex-arrays.h"
#include "nvidia-push-utils.h" /* nvPushSetObject */

#include <class/cl9097.h>
#include <class/cla06fsubch.h>

#include <xz.h>

#if NV_PUSH_ALLOW_FLOAT
  #define NV3D_FLOAT_ONE  (1.00f)
#else
  #define NV3D_FLOAT_ONE  0x3F800000 /* 1.00f */
#endif

static void *DecompressUsingXz(
    const Nv3dChannelRec *p3dChannel,
    const void *compressedData,
    size_t compressedSize,
    size_t decompressedSize)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    void *decompressedData = nv3dImportAlloc(decompressedSize);
    struct xz_dec *xzState;
    enum xz_ret ret;

    struct xz_buf xzBuf = {
        .in = compressedData,
        .in_pos = 0,
        .in_size = compressedSize,
        .out = decompressedData,
        .out_pos = 0,
        .out_size = decompressedSize,
    };

    if (decompressedData == NULL) {
        return NULL;
    }

    xz_crc32_init();

    xzState = xz_dec_init(XZ_SINGLE, 0);

    if (xzState == NULL) {
        pImports->logError(pPushDevice,
                           "Failed to initialize xz decompression.");
        goto fail;
    }

    ret = xz_dec_run(xzState, &xzBuf);

    xz_dec_end(xzState);

    if (ret != XZ_STREAM_END) {
        pImports->logError(pPushDevice, "Failed to decompress xz data.");
        goto fail;
    }

    return decompressedData;

fail:
    nv3dImportFree(decompressedData);
    return NULL;
}

static void *DecompressPrograms(const Nv3dChannelRec *p3dChannel)
{
    const Nv3dChannelProgramsRec *pPrograms = &p3dChannel->programs;
    const size_t compressedSize =
        pPrograms->code.compressedEnd - pPrograms->code.compressedStart;

    nvAssert(pPrograms->code.compressedEnd > pPrograms->code.compressedStart);

    return DecompressUsingXz(p3dChannel,
                             pPrograms->code.compressedStart,
                             compressedSize,
                             pPrograms->code.decompressedSize);
}

/*
 * This function attempts to upload the precompiled shaders to the GPU through
 * a temporary CPU mapping.
 * Failure of this function is not fatal -- we can fall back to uploading
 * through the pushbuffer.
 */
static NvBool UploadPrograms(Nv3dChannelPtr p3dChannel, const void *programCode)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    const size_t size = p3dChannel->programs.code.decompressedSize;
    NvU32 sd;

    for (sd = 0; sd < pPushDevice->numSubDevices; sd++) {
        NvU32 status;
        void *ptr;
        const NvU32 hMemory = pPushDevice->clientSli ?
            p3dChannel->surface.handle[sd] :
            p3dChannel->surface.handle[0];

        status = pImports->rmApiMapMemory(pPushDevice,
                                          pPushDevice->subDevice[sd].handle,
                                          hMemory,
                                          p3dChannel->surface.programOffset,
                                          size,
                                          &ptr,
                                          0);
        if (status != NVOS_STATUS_SUCCESS) {
            return FALSE;
        }

        nvAssert((size % 4) == 0);
        nvDmaMoveDWORDS(ptr, programCode, size / 4);

        status = pImports->rmApiUnmapMemory(pPushDevice,
                                            pPushDevice->subDevice[sd].handle,
                                            hMemory,
                                            ptr,
                                            0);
        nvAssert(status == NVOS_STATUS_SUCCESS);
    }

    return TRUE;
}

NvBool nv3dInitChannel(Nv3dChannelPtr p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    const Nv3dDeviceCapsRec *pCaps = &p3dChannel->p3dDevice->caps;
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;
    const NvU64 tex0GpuAddress = nv3dGetTextureGpuAddress(p3dChannel, 0);
    NvU64 gpuAddress;
    NvU32 i;
    void *programCode = DecompressPrograms(p3dChannel);

    if (programCode == NULL) {
        return FALSE;
    }

    /*
     * nv3dAllocChannel() should have been called to assign p3dDevice.
     */
    nvAssert(p3dChannel->p3dDevice != NULL);

    /*
     * nv3dAllocChannelObject() should have been called to assign
     * pPushChannel.
     */
    nvAssert(p3dChannel->pPushChannel != NULL);

    /*
     * nv3dAllocChannelSurface() should have been called to allocate
     * the surface.
     */
    nvAssert(p3dChannel->surface.handle[0] != 0);

    nv3dClearProgramCache(p3dChannel);

    p3dChannel->currentPrimitiveMode = ~0;

    nvPushSetObject(p, NVA06F_SUBCHANNEL_3D, p3dChannel->handle);

    // Ct[0]'s format defaults to A8R8G8B8, rather than DISABLED.
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_COLOR_TARGET_FORMAT(0),
        NV3D_C(9097, SET_COLOR_TARGET_FORMAT, V, DISABLED));

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_ZT_SELECT,
        NV3D_V(9097, SET_ZT_SELECT, TARGET_COUNT, 0));

    // Set a substitute stream address.  This is used when the Vertex Attribute
    // Fetch unit tries to fetch outside the bounds of an enabled stream, which
    // should never happen.  However, AModel always fetches this value
    // regardless of whether it actually needs it, so it causes MMU errors if
    // it's not set.
    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_VERTEX_STREAM_SUBSTITUTE_A, 2);
        nvPushSetMethodDataU64(p, p3dChannel->surface.gpuAddress);

    if (p3dChannel->programLocalMemorySize) {
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
            NV9097_SET_SHADER_LOCAL_MEMORY_A, 4);
        // ADDRESS_{UPPER,LOWER}
        nvPushSetMethodDataU64(p,
            nv3dGetProgramLocalMemoryGpuAddress(p3dChannel));
        // SIZE_{UPPER,LOWER}
        nvPushSetMethodDataU64(p, p3dChannel->programLocalMemorySize);
    }

    // Point rasterization.
    nvPushImmed(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_POINT_CENTER_MODE, OGL);

    // SPA Control.
    nvPushImmed(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_SAMPLER_BINDING, VIA_HEADER_BINDING);

    // Viewport parameters.
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_VIEWPORT_SCALE_OFFSET,
        NV3D_C(9097, SET_VIEWPORT_SCALE_OFFSET, ENABLE, FALSE));

    // Viewport clip.  There are 16 viewports
    for (i = 0; i < 16; i++) {
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
            NV9097_SET_VIEWPORT_CLIP_HORIZONTAL(i), 2);
        nvPushSetMethodData(p, pCaps->maxDim << 16);
        nvPushSetMethodData(p, pCaps->maxDim << 16);
    }

    nvPushImmed(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_PROVOKING_VERTEX, LAST);

    // Use one rop state for all targets
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_SINGLE_ROP_CONTROL,
        NV3D_C(9097, SET_SINGLE_ROP_CONTROL, ENABLE, TRUE));
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_SINGLE_CT_WRITE_CONTROL,
        NV3D_C(9097, SET_SINGLE_CT_WRITE_CONTROL, ENABLE, TRUE));

    // Set up blending: enable Ct[0]. It's disabled by default for the rest.
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND(0),
            NV3D_C(9097, SET_BLEND, ENABLE, TRUE));

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_CONST_ALPHA, 1);
    nv3dPushFloat(p, NV3D_FLOAT_ONE);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_BLEND_SEPARATE_FOR_ALPHA, 2);
    nvPushSetMethodData(p,
        NV3D_C(9097, SET_BLEND_SEPARATE_FOR_ALPHA, ENABLE, FALSE));
    nvPushSetMethodData(p,
        NV3D_C(9097, SET_BLEND_COLOR_OP, V, OGL_FUNC_ADD));

    // Upload the pixel shaders.  First, attempt to upload through a CPU
    // mapping (which is generally faster); if that fails (e.g., because there
    // is no space in BAR1 for the mapping), then fall back to uploading inline
    // through the pushbuffer.
    if (!UploadPrograms(p3dChannel, programCode)) {
        pHal->uploadDataInline(p3dChannel,
                               nv3dGetProgramGpuAddress(p3dChannel),
                               0,
                               programCode,
                               p3dChannel->programs.code.decompressedSize);
    }

    nv3dImportFree(programCode);
    programCode = NULL;

    for (i = 0; i < p3dChannel->programs.constants.count; i++) {
        const Nv3dShaderConstBufInfo *pInfo =
            &p3dChannel->programs.constants.info[i];

        pHal->uploadDataInline(p3dChannel,
                               nv3dGetProgramConstantsGpuAddress(p3dChannel),
                               pInfo->offset,
                               pInfo->data,
                               pInfo->size);
    }

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_INVALIDATE_SHADER_CACHES, 1);
    nvPushSetMethodData(p,
        DRF_DEF(9097, _INVALIDATE_SHADER_CACHES, _INSTRUCTION, _TRUE) |
        DRF_DEF(9097, _INVALIDATE_SHADER_CACHES, _CONSTANT, _TRUE));

    if (pCaps->hasProgramRegion) {
        gpuAddress = nv3dGetProgramGpuAddress(p3dChannel);

        nvAssert((gpuAddress & 255) == 0);
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_PROGRAM_REGION_A, 2);
        nvPushSetMethodDataU64(p, gpuAddress);
    }

    // Initialize the texture header and sampler area.
    //
    // To update these things, we upload data through the pushbuffer.  The
    // upload has an alignment twice the size of a texture header/sampler, so we
    // interleave the two.  Texture samplers come first.  Thus, "texture sampler
    // 2i+1" is actually texture header 2i.  This allows us to use a single
    // upload to update a single texture sampler/header pair if we so desire.
    gpuAddress = tex0GpuAddress + offsetof(Nv3dTexture, samp);
    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_TEX_SAMPLER_POOL_A, 3);
    nvPushSetMethodDataU64(p, gpuAddress);
    nvPushSetMethodData(p, 0); // Max index.  0 because we use VIA_HEADER mode.

    gpuAddress = tex0GpuAddress + offsetof(Nv3dTexture, head);
    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_TEX_HEADER_POOL_A, 3);
    nvPushSetMethodDataU64(p, gpuAddress);
    nvPushSetMethodData(p, 2 * (NV_MAX(p3dChannel->numTextures, 1) - 1)); // Max index

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_WINDOW_ORIGIN,
        NV3D_C(9097, SET_WINDOW_ORIGIN, MODE, UPPER_LEFT) |
        NV3D_C(9097, SET_WINDOW_ORIGIN, FLIP_Y, TRUE));

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_ZCULL_BOUNDS, 1);
    nvPushSetMethodData(p,
        NV3D_C(9097, SET_ZCULL_BOUNDS, Z_MIN_UNBOUNDED_ENABLE, FALSE) |
        NV3D_C(9097, SET_ZCULL_BOUNDS, Z_MAX_UNBOUNDED_ENABLE, FALSE));

    pHal->setSpaVersion(p3dChannel);

    pHal->initChannel(p3dChannel);

    _nv3dInitializeStreams(p3dChannel);

    return TRUE;
}

void nv3dLoadProgram(
    Nv3dChannelRec *p3dChannel,
    int programIndex)
{
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;
    const Nv3dProgramInfo *pgm = &p3dChannel->programs.info[programIndex];
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvAssert(programIndex < p3dChannel->programs.num);
    nvAssert(programIndex >= 0);
    nvAssert(pgm->stage < ARRAY_LEN(p3dChannel->currentProgramIndex));
    nvAssert(pgm->bindGroup <= NV3D_HW_BIND_GROUP_LAST);

    if (p3dChannel->currentProgramIndex[pgm->stage] == programIndex) {
        return;
    }

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_PIPELINE_SHADER(pgm->stage), 1);
    nvPushSetMethodData(p,
        NV3D_C(9097, SET_PIPELINE_SHADER, ENABLE, TRUE) |
        NV3D_V(9097, SET_PIPELINE_SHADER, TYPE, pgm->type));

    pHal->setProgramOffset(p3dChannel, pgm->stage, pgm->offset);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_PIPELINE_REGISTER_COUNT(pgm->stage), 2);
    nvPushSetMethodData(p, pgm->registerCount);
    nvPushSetMethodData(p, pgm->bindGroup);

    // Bind or invalidate the compiler-generated constant buffer slot, which the
    // compiler always puts in NV3D_CB_SLOT_COMPILER.
    if (pgm->constIndex == -1) {
        nv3dBindCb(p3dChannel, pgm->bindGroup,
                   NV3D_CB_SLOT_COMPILER, FALSE);
    } else if (p3dChannel->programs.constants.size > 0) {
        const Nv3dShaderConstBufInfo *pInfo =
            &p3dChannel->programs.constants.info[pgm->constIndex];
        const NvU64 gpuAddress =
            nv3dGetProgramConstantsGpuAddress(p3dChannel) + pInfo->offset;
        const NvU32 paddedSize =
            NV_ALIGN_UP(pInfo->size, p3dChannel->programs.constants.sizeAlign);

        nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
            NV9097_SET_CONSTANT_BUFFER_SELECTOR_A, 3);
        nvPushSetMethodData(p, paddedSize);
        nvPushSetMethodDataU64(p, gpuAddress);
        nv3dBindCb(p3dChannel, pgm->bindGroup, NV3D_CB_SLOT_COMPILER, TRUE);
    }

    p3dChannel->currentProgramIndex[pgm->stage] = programIndex;
}

void _nv3dSetProgramOffsetFermi(
    Nv3dChannelRec *p3dChannel,
    NvU32 stage,
    NvU32 offset)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_PIPELINE_PROGRAM(stage), 1);
    nvPushSetMethodData(p, offset);
}

void _nv3dInvalidateTexturesFermi(
    Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_INVALIDATE_SAMPLER_CACHE,
        NV3D_C(9097, INVALIDATE_SAMPLER_CACHE, LINES, ALL));
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_INVALIDATE_TEXTURE_HEADER_CACHE,
        NV3D_C(9097, INVALIDATE_TEXTURE_HEADER_CACHE, LINES, ALL));
}

void nv3dSetBlendColorCoefficients(
    Nv3dChannelPtr p3dChannel,
    enum Nv3dBlendOperation op,
    NvBool forceNoDstAlphaBits,
    NvBool dualSourceBlending)
{
#define SFACTOR(factor) (NV9097_SET_BLEND_COLOR_SOURCE_COEFF_V_OGL_##factor)
#define DFACTOR(factor) (NV9097_SET_BLEND_COLOR_DEST_COEFF_V_OGL_##factor)
#define OP(op)          (NV3D_BLEND_OP_##op)

    static const struct {
        NvU32 sfactor;
        NvU32 dfactor;
    } BlendOps[] = {
        [OP(CLEAR)]        = {SFACTOR(ZERO),                DFACTOR(ZERO)},
        [OP(SRC)]          = {SFACTOR(ONE),                 DFACTOR(ZERO)},
        [OP(DST)]          = {SFACTOR(ZERO),                DFACTOR(ONE)},
        [OP(OVER)]         = {SFACTOR(ONE),                 DFACTOR(ONE_MINUS_SRC_ALPHA)},
        [OP(OVER_REVERSE)] = {SFACTOR(ONE_MINUS_DST_ALPHA), DFACTOR(ONE)},
        [OP(IN)]           = {SFACTOR(DST_ALPHA),           DFACTOR(ZERO)},
        [OP(IN_REVERSE)]   = {SFACTOR(ZERO),                DFACTOR(SRC_ALPHA)},
        [OP(OUT)]          = {SFACTOR(ONE_MINUS_DST_ALPHA), DFACTOR(ZERO)},
        [OP(OUT_REVERSE)]  = {SFACTOR(ZERO),                DFACTOR(ONE_MINUS_SRC_ALPHA)},
        [OP(ATOP)]         = {SFACTOR(DST_ALPHA),           DFACTOR(ONE_MINUS_SRC_ALPHA)},
        [OP(ATOP_REVERSE)] = {SFACTOR(ONE_MINUS_DST_ALPHA), DFACTOR(SRC_ALPHA)},
        [OP(XOR)]          = {SFACTOR(ONE_MINUS_DST_ALPHA), DFACTOR(ONE_MINUS_SRC_ALPHA)},
        [OP(ADD)]          = {SFACTOR(ONE),                 DFACTOR(ONE)},
        [OP(SATURATE)]     = {SFACTOR(SRC_ALPHA_SATURATE),  DFACTOR(ONE)}
    };

    NvU32 sfactor, dfactor;

    nvAssert(op < ARRAY_LEN(BlendOps));

    sfactor = BlendOps[op].sfactor;
    dfactor = BlendOps[op].dfactor;

    // if we're rendering to a picture that has an XRGB format that HW doesn't
    // support, feed in the 1.0 constant DstAlpha value
    if (forceNoDstAlphaBits) {
        switch (sfactor) {
            case SFACTOR(DST_ALPHA):
                sfactor = SFACTOR(CONSTANT_ALPHA);
                break;
            case SFACTOR(ONE_MINUS_DST_ALPHA):
                sfactor = SFACTOR(ONE_MINUS_CONSTANT_ALPHA);
                break;
            default:
                break;
        }
    }

    // If dual-source blending is enabled, swap the dfactor for one that uses
    // the second source color.
    if (dualSourceBlending) {
        switch (dfactor) {
            case DFACTOR(SRC_ALPHA):
            case DFACTOR(SRC_COLOR):
                dfactor = DFACTOR(SRC1COLOR);
                break;
            case DFACTOR(ONE_MINUS_SRC_ALPHA):
            case DFACTOR(ONE_MINUS_SRC_COLOR):
                dfactor = DFACTOR(INVSRC1COLOR);
                break;
            default:
                break;
        }
    }

    Nv3dBlendState nv3dBlendStateColor = { };

    nv3dBlendStateColor.blendEquation = NV3D_C(9097, SET_BLEND_COLOR_OP, V, OGL_FUNC_ADD);
    nv3dBlendStateColor.blendFactorSrc = sfactor;
    nv3dBlendStateColor.blendFactorDst = dfactor;

    nv3dSetBlend(p3dChannel, &nv3dBlendStateColor, NULL, NULL);
}

void nv3dSetBlend(
    Nv3dChannelPtr p3dChannel,
    const Nv3dBlendState *blendStateColor,
    const Nv3dBlendState *blendStateAlpha,
    const Nv3dColor *blendColor)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    const Nv3dColor defaultColor = {
        NV3D_FLOAT_ONE,
        NV3D_FLOAT_ONE,
        NV3D_FLOAT_ONE,
        NV3D_FLOAT_ONE
    };

    if (blendColor == NULL) {
        blendColor = &defaultColor;
    }

    if (blendStateColor == NULL && blendStateAlpha == NULL) {
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND(0),
            NV3D_C(9097, SET_BLEND, ENABLE, FALSE));
        return;
    }

    if (blendStateColor != NULL) {
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_COLOR_OP, 3);
        nvPushSetMethodData(p, blendStateColor->blendEquation);
        nvPushSetMethodData(p, blendStateColor->blendFactorSrc);
        nvPushSetMethodData(p, blendStateColor->blendFactorDst);
    }

    if (blendStateAlpha != NULL) {
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_SEPARATE_FOR_ALPHA,
            NV3D_C(9097, SET_BLEND_SEPARATE_FOR_ALPHA, ENABLE, TRUE));
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_ALPHA_OP, 2);
        nvPushSetMethodData(p, blendStateAlpha->blendEquation);
        nvPushSetMethodData(p, blendStateAlpha->blendFactorSrc);
        nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_ALPHA_DEST_COEFF, 1);
        nvPushSetMethodData(p, blendStateAlpha->blendFactorDst);
    } else {
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_SEPARATE_FOR_ALPHA,
            NV3D_C(9097, SET_BLEND_SEPARATE_FOR_ALPHA, ENABLE, FALSE));
    }

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND_CONST_RED, 4);
    nv3dPushFloat(p, blendColor->red);
    nv3dPushFloat(p, blendColor->green);
    nv3dPushFloat(p, blendColor->blue);
    nv3dPushFloat(p, blendColor->alpha);

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_BLEND(0),
        NV3D_C(9097, SET_BLEND, ENABLE, TRUE));

}

void _nv3dSetVertexStreamEndFermi(
    Nv3dChannelPtr p3dChannel,
    enum Nv3dVertexAttributeStreamType stream,
    const Nv3dVertexAttributeStreamRec *pStream)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_VERTEX_STREAM_LIMIT_A_A(stream), 2);
    nvPushSetMethodDataU64(p, pStream->end - 1);
}
