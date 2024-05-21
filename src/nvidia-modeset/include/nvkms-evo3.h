/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_EVO_3_H__
#define __NVKMS_EVO_3_H__

#include "nvkms-types.h"
#include "nv-float.h"
#include "nvkms-softfloat.h"
#include <class/clc57d.h> // NVC57D_CORE_CHANNEL_DMA

#include <ctrl/ctrlc372/ctrlc372chnc.h>

#define NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C6        \
    (NVBIT64(NvKmsSurfaceMemoryFormatRF16GF16BF16XF16) |    \
     NVBIT64(NvKmsSurfaceMemoryFormatX2B10G10R10))

#define NV_EVO3_SUPPORTED_DITHERING_MODES                               \
    ((1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO)        | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_DYNAMIC_2X2) | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_STATIC_2X2)  | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_TEMPORAL))

#define NV_EVO3_SUPPORTED_CURSOR_COMP_BLEND_MODES              \
    ((1 << NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)                    | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA)         | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)             | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA) | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA))

#define NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C5                                     \
    (DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _ILUT_ALLOWED, _TRUE)        | \
     DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _TMO_LUT_ALLOWED, _TRUE))

// HW supports ratio = 1, 2 (downscaling), 4 (downscaling)
#define NUM_SCALER_RATIOS 3

// There are 16 phases stored in matrix, but HW can derive the values of phase
// +16 and -16 from phase 0. Therefore, SW loads phase +16/-16 in phase 0 coeff
// values.
// coeff values in phase 0.
#define NUM_TAPS5_COEFF_PHASES 16

// There are 5 coefficient values per phase (or matrix row), but SW doesn't need
// to upload c2. So, the value here is set to 4.
#define NUM_TAPS5_COEFF_VALUES 4

extern const NvU32 scalerTaps5Coeff[NUM_SCALER_RATIOS][NUM_TAPS5_COEFF_PHASES][NUM_TAPS5_COEFF_VALUES];

struct EvoClampRangeC5 {
    NvU32 green, red_blue;
};

/*
 * Converts FP32 to fixed point S5.14 coefficient format
 */
static inline NvU32 nvCscCoefConvertS514(float32_t x)
{
    /* more concisely, (NvS32)floor(x * 65536.0 + 2.0) */
    const NvS32 y = f32_to_i32(f32_mulAdd(x,
                                          NvU32viewAsF32(NV_FLOAT_65536),
                                          NvU32viewAsF32(NV_FLOAT_TWO)),
                               softfloat_round_min, FALSE);
    return (NvU32)(0x001ffffc & clamp_S32(y, -0x100000, 0xfffff));
}

NvBool nvComputeMinFrameIdle(
    const NVHwModeTimingsEvo *pTimings,
    NvU16 *pLeadingRasterLines,
    NvU16 *pTrailingRasterLines);

void nvEvoSetControlC3(NVDevEvoPtr pDevEvo, int sd);

void nvEvoORSetControlC3(NVDevEvoPtr pDevEvo,
                              const NVConnectorEvoRec *pConnectorEvo,
                              const enum nvKmsTimingsProtocol protocol,
                              const NvU32 orIndex,
                              const NvU32 headMask,
                              NVEvoUpdateState *updateState);

NvU32 nvEvoGetPixelDepthC3(const enum nvKmsPixelDepth pixelDepth);

NvBool nvEvoSetUsageBoundsC5(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                  const struct NvKmsUsageBounds *pUsage,
                                  NVEvoUpdateState *updateState);

void nvEvoUpdateC3(NVDevEvoPtr pDevEvo,
                        const NVEvoUpdateState *updateState,
                        NvBool releaseElv);

NvBool
nvEvoSetCtrlIsModePossibleParams3(NVDispEvoPtr pDispEvo,
                                  const NVEvoIsModePossibleDispInput *pInput,
                                  NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pImp);
void
nvEvoSetIsModePossibleDispOutput3(const NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pImp,
                                  const NvBool result,
                                  NVEvoIsModePossibleDispOutput *pOutput);

void
nvEvoIsModePossibleC3(NVDispEvoPtr pDispEvo,
                    const NVEvoIsModePossibleDispInput *pInput,
                    NVEvoIsModePossibleDispOutput *pOutput);

void nvEvoPrePostIMPC3(NVDispEvoPtr pDispEvo, NvBool isPre);

void nvEvoSetNotifierC3(NVDevEvoRec *pDevEvo,
                             const NvBool notify,
                             const NvBool awaken,
                             const NvU32 notifier,
                             NVEvoUpdateState *updateState);

NvBool nvEvoGetCapabilitiesC6(NVDevEvoPtr pDevEvo);

void
nvEvoFlipC6(NVDevEvoPtr pDevEvo,
          NVEvoChannelPtr pChannel,
          const NVFlipChannelEvoHwState *pHwState,
          NVEvoUpdateState *updateState,
          NvBool bypassComposition);

void nvEvoFlipTransitionWARC6(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                   const NVEvoSubDevHeadStateRec *pSdHeadState,
                                   const NVFlipEvoHwState *pFlipState,
                                   NVEvoUpdateState *updateState);

void
nvEvoFillLUTSurfaceC5(NVEvoLutEntryRec *pLUTBuffer,
                    const NvU16 *red,
                    const NvU16 *green,
                    const NvU16 *blue,
                    int nColorMapEntries, int depth);

void nvSetupOutputLUT5(NVDevEvoPtr pDevEvo,
                       const NVDispHeadStateEvoRec *pHeadState,
                       const int head,
                       NVLutSurfaceEvoPtr pLutSurfEvo,
                       NvBool enableBaseLut,
                       NvBool enableOutputLut,
                       NVEvoUpdateState *updateState,
                       NvBool bypassComposition,
                       NVSurfaceDescriptor **pSurfaceDesc,
                       NvU32 *lutSize,
                       NvBool *disableOcsc0,
                       NvU32 *fpNormScale,
                       NvBool *isLutModeVss);

NvBool nvEvoGetHeadSetControlCursorValueC3(const NVDevEvoRec *pDevEvo,
                                                const NVSurfaceEvoRec *pSurfaceEvo,
                                                NvU32 *pValue);

NvBool nvEvoValidateCursorSurfaceC3(const NVDevEvoRec *pDevEvo,
                                         const NVSurfaceEvoRec *pSurfaceEvo);

NvBool nvEvoValidateWindowFormatC6(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut);

void nvEvoInitCompNotifierC3(const NVDispEvoRec *pDispEvo, int idx);

NvBool nvEvoIsCompNotifierCompleteC3(NVDispEvoPtr pDispEvo, int idx);

void nvEvoWaitForCompNotifierC3(const NVDispEvoRec *pDispEvo, int idx);

void nvEvoInitChannel3(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel);

void nvInitScalerCoefficientsPrecomp5(NVEvoChannelPtr pChannel,
                                           NvU32 coeff, NvU32 index);

void nvEvoInitDefaultLutC5(NVDevEvoPtr pDevEvo);

void nvEvoInitWindowMappingC5(const NVDispEvoRec *pDispEvo,
                                   NVEvoModesetUpdateState *pModesetUpdateState);

NvBool nvEvoIsChannelIdleC3(NVDevEvoPtr pDevEvo,
                                 NVEvoChannelPtr pChan,
                                 NvU32 sd,
                                 NvBool *result);

NvBool nvEvoIsChannelMethodPendingC3(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChan,
                                          NvU32 sd,
                                          NvBool *result);

NvBool nvEvoForceIdleSatelliteChannelC3(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState);

NvBool nvEvoForceIdleSatelliteChannelIgnoreLockC3(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState);

void nvEvoAccelerateChannelC3(NVDevEvoPtr pDevEvo,
                                   NVEvoChannelPtr pChannel,
                                   const NvU32 sd,
                                   const NvBool trashPendingMethods,
                                   const NvBool unblockMethodsInExecutation,
                                   NvU32 *pOldAccelerators);

void nvEvoResetChannelAcceleratorsC3(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChannel,
                                          const NvU32 sd,
                                          const NvBool trashPendingMethods,
                                          const NvBool unblockMethodsInExecutation,
                                          NvU32 oldAccelerators);

NvBool nvEvoAllocRmCtrlObjectC3(NVDevEvoPtr pDevEvo);

void nvEvoFreeRmCtrlObjectC3(NVDevEvoPtr pDevEvo);

void nvEvoSetImmPointOutC3(NVDevEvoPtr pDevEvo,
                                NVEvoChannelPtr pChannel,
                                NvU32 sd,
                                NVEvoUpdateState *updateState,
                                NvU16 x, NvU16 y);

NvBool nvEvoQueryHeadCRC32_C3(NVDevEvoPtr pDevEvo,
                                   NVEvoDmaPtr pDma,
                                   NvU32 sd,
                                   NvU32 entry_count,
                                   CRC32NotifierCrcOut *crc32,
                                   NvU32 *numCRC32);

void nvEvoGetScanLineC3(const NVDispEvoRec *pDispEvo,
                             const NvU32 head,
                             NvU16 *pScanLine,
                             NvBool *pInBlankingPeriod);

NvU32 nvEvoGetActiveViewportOffsetC3(NVDispEvoRec *pDispEvo, NvU32 head);

NvBool nvEvoComputeWindowScalingTapsC5(const NVDevEvoRec *pDevEvo,
                                            const NVEvoChannel *pChannel,
                                            NVFlipChannelEvoHwState *pHwState);

const struct NvKmsCscMatrix* nvEvoGetOCsc1MatrixC5(const NVDispHeadStateEvoRec *pHeadState);

struct EvoClampRangeC5 nvEvoGetOCsc1ClampRange(const NVDispHeadStateEvoRec *pHeadState);

void nvEvo3PickOCsc0(NVDispEvoPtr pDispEvo, const NvU32 head, struct NvKms3x4MatrixF32 *ocsc0Matrix);

static inline const NVEvoScalerCaps*
nvEvoGetWindowScalingCapsC3(const NVDevEvoRec *pDevEvo)
{
    /*
     * Use window 0 by default. This should be fine for now since precomp
     * scaling will only be enabled on Orin, and all windows have the same
     * capabilities on Orin.
     *
     * The mapping in this function can be updated if/when precomp scaling
     * support is extended to other display architectures.
     */
    return &pDevEvo->gpus[0].capabilities.window[0].scalerCaps;
}

static inline NvU32 nvGetMaxPixelsFetchedPerLine(NvU16 inWidth,
                                               NvU16 maxHDownscaleFactor)
{
    /*
     * Volta should be:
     * (((SetViewportSizeIn.Width + 6) * SetMaxInputScaleFactor.Horizontal + 1023 ) >> 10 ) + 6
     *
     * Turing should be:
     * (((SetViewportSizeIn.Width + 6) * SetMaxInputScaleFactor.Horizontal + 1023 ) >> 10 ) + 8
     *
     * Ampere, which adds "overfetch" to have tiled displays / 2-head-1-OR use cases without
     * visual artefacts at head boundaries:
     * (((SetViewportSizeIn.Width + 14) * SetMaxInputScaleFactor.Horizontal + 1023) >> 10) + 8
     *
     * We don't have to be super-precise when programming maxPixelsFetchedPerLine,
     * so return realistic worst-case value.
     */
    return (((inWidth + 14) * maxHDownscaleFactor + 1023) >> 10) + 8;
}

#endif /* __NVKMS_EVO_3_H__ */
