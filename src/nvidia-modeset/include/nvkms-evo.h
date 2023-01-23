/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_H__
#define __NVKMS_H__

#include "nvkms-types.h"
#include "nvkms-modeset-types.h"
#include "nvkms-api.h"

#ifdef __cplusplus
extern "C" {
#endif

extern NVEvoInfoStringRec dummyInfoString;

NVDevEvoPtr nvFindDevEvoByDeviceId(NvU32 deviceId);
NvU8 nvGetGpuLogIndex(void);
void nvEvoDetachConnector(NVConnectorEvoRec *pConnectorEvo, const NvU32 head,
                          NVEvoModesetUpdateState *pModesetUpdateState);
void nvEvoAttachConnector(NVConnectorEvoRec *pConnectorEvo,
                          const NvU32 head,
                          NVDPLibModesetStatePtr pDpLibModesetState,
                          NVEvoModesetUpdateState *pModesetUpdateState);
void nvEvoUpdateAndKickOff(const NVDispEvoRec *pDispEvo, NvBool sync,
                           NVEvoUpdateState *updateState, NvBool releaseElv);
void nvDoIMPUpdateEvo(NVDispEvoPtr pDispEvo,
                      NVEvoUpdateState *updateState);
void nvEvoArmLightweightSupervisor(NVDispEvoPtr pDispEvo,
                                   const NvU32 head,
                                   NvBool isVrr,
                                   NvBool enable);

void nvSetViewPortsEvo(NVDispEvoPtr pDispEvo,
                       const NvU32 head, NVEvoUpdateState *updateState);
void nvSetViewPortPointInEvo(NVDispEvoPtr pDispEvo,
                             const NvU32 head,
                             const NvU16 x,
                             NvU16 y,
                             NVEvoUpdateState *updateState);
void
nvConstructNvModeTimingsFromHwModeTimings(const NVHwModeTimingsEvo *pTimings,
                                          NvModeTimingsPtr pModeTimings);
void nvEvoSetTimings(NVDispEvoPtr pDispEvo, const NvU32 head,
                     NVEvoUpdateState *updateState);
void nvInitScalingUsageBounds(const NVDevEvoRec *pDevEvo,
                              struct NvKmsScalingUsageBounds *pScaling);
NvBool nvComputeScalingUsageBounds(const NVEvoScalerCaps *pScalerCaps,
                                   const NvU32 inWidth, const NvU32 inHeight,
                                   const NvU32 outWidth, const NvU32 outHeight,
                                   NVEvoScalerTaps hTaps, NVEvoScalerTaps vTaps,
                                   struct NvKmsScalingUsageBounds *out);
NvBool nvAssignScalerTaps(const NVDevEvoRec *pDevEvo,
                          const NVEvoScalerCaps *pScalerCaps,
                          const NvU32 inWidth, const NvU32 inHeight,
                          const NvU32 outWidth, const NvU32 outHeight,
                          NvBool doubleScan,
                          NVEvoScalerTaps *hTapsOut, NVEvoScalerTaps *vTapsOut);
NvBool nvValidateHwModeTimingsViewPort(const NVDevEvoRec *pDevEvo,
                                       const NVEvoScalerCaps *pScalerCaps,
                                       NVHwModeTimingsEvoPtr pTimings,
                                       NVEvoInfoStringPtr pInfoString);
void nvAssignDefaultUsageBounds(const NVDispEvoRec *pDispEvo,
                                NVHwModeViewPortEvo *pViewPort);
void nvUnionUsageBounds(const struct NvKmsUsageBounds *a,
                        const struct NvKmsUsageBounds *b,
                        struct NvKmsUsageBounds *ret);
NvBool UsageBoundsEqual(const struct NvKmsUsageBounds *a,
                        const struct NvKmsUsageBounds *b);
NvU64 nvEvoGetFormatsWithEqualOrLowerUsageBound(
    const enum NvKmsSurfaceMemoryFormat format,
    const NvU64 supportedFormatsCapMask);
void nvCancelLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo);
void nvScheduleLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo);
void nvAssertAllDpysAreInactive(NVDevEvoPtr pDevEvo);
void nvEvoLockStatePreModeset(NVDevEvoPtr pDevEvo, NvU32 *dispNeedsEarlyUpdate,
                              NVEvoUpdateState *updateState);
void nvEvoLockStatePostModeset(NVDevEvoPtr pDevEvo, const NvBool doRasterLock);
NvBool nvSetUsageBoundsEvo(
    NVDevEvoPtr pDevEvo,
    NvU32 sd,
    NvU32 head,
    const struct NvKmsUsageBounds *pUsage,
    NVEvoUpdateState *updateState);
void nvEnableMidFrameAndDWCFWatermark(NVDevEvoPtr pDevEvo,
                                      NvU32 sd,
                                      NvU32 head,
                                      NvBool enable,
                                      NVEvoUpdateState *pUpdateState);

void nvEvoHeadSetControlOR(NVDispEvoPtr pDispEvo,
                           const NvU32 head, NVEvoUpdateState *pUpdateState);

void nvChooseDitheringEvo(
    const NVConnectorEvoRec *pConnectorEvo,
    const enum nvKmsPixelDepth pixelDepth,
    const NVDpyAttributeRequestedDitheringConfig *pReqDithering,
    NVDpyAttributeCurrentDitheringConfig *pCurrDithering);

void nvSetDitheringEvo(
    NVDispEvoPtr pDispEvo,
    const NvU32 head,
    const NVDpyAttributeCurrentDitheringConfig *pCurrDithering,
    NVEvoUpdateState *pUpdateState);

NvBool nvEnableFrameLockEvo(NVDispEvoPtr pDispEvo);
NvBool nvDisableFrameLockEvo(NVDispEvoPtr pDispEvo);
NvBool nvQueryRasterLockEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *val);
NvBool nvSetFlipLockEvo(NVDpyEvoPtr pDpyEvo, NvS64 value);
NvBool nvGetFlipLockEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue);
NvBool nvAllowFlipLockEvo(NVDispEvoPtr pDispEvo, NvS64 value);
NvBool nvSetStereoEvo(const NVDispEvoRec *pDispEvo,
                      const NvU32 head, NvBool enable);
NvBool nvGetStereoEvo(const NVDispEvoRec *pDispEvo, const NvU32 head);
struct NvKmsCompositionParams nvDefaultCursorCompositionParams(const NVDevEvoRec *pDevEvo);
NvBool nvAllocCoreChannelEvo(NVDevEvoPtr pDevEvo);
void nvFreeCoreChannelEvo(NVDevEvoPtr pDevEvo);

void nvEvoUpdateSliVideoBridge(NVDevEvoPtr pDevEvo);

void nvSetDVCEvo(NVDispEvoPtr pDispEvo,
                 const NvU32 head,
                 NvS32 dvc,
                 NVEvoUpdateState *updateState);
void nvSetImageSharpeningEvo(NVDispEvoRec *pDispEvo, const NvU32 head,
                             const NvU32 value, NVEvoUpdateState *updateState);

NvBool nvLayerSetPositionEvo(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsSetLayerPositionRequest *pRequest);

NvBool nvConstructHwModeTimingsEvo(const NVDpyEvoRec *pDpyEvo,
                                   const struct NvKmsMode *pKmsMode,
                                   const struct NvKmsSize *pViewPortSizeIn,
                                   const struct NvKmsRect *pViewPortOut,
                                   NVHwModeTimingsEvoPtr pTimings,
                                   const struct NvKmsModeValidationParams
                                   *pParams,
                                   NVEvoInfoStringPtr pInfoString);

NvBool nvConstructHwModeTimingsImpCheckEvo(
    const NVConnectorEvoRec                *pConnectorEvo,
    NVHwModeTimingsEvoPtr                   pTimings,
    const struct NvKmsModeValidationParams *pParams,
    NVEvoInfoStringPtr                      pInfoString,
    const int                               head);

NvBool nvDowngradeHwModeTimingsDpPixelDepthEvo(
    NVHwModeTimingsEvoPtr pTimings,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorRangeValue colorRange);

NvBool nvDPValidateModeEvo(NVDpyEvoPtr pDpyEvo,
                           NVHwModeTimingsEvoPtr pTimings,
                           const struct NvKmsModeValidationParams *pParams);

NvBool nvEvoUpdateHwModeTimingsViewPort(
    const NVDpyEvoRec *pDpyEvo,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    const struct NvKmsSize *pViewPortSizeIn,
    const struct NvKmsRect *pViewPortOut,
    NVHwModeTimingsEvo *pTimings);

typedef struct _NVValidateImpOneDispHeadParamsRec
{
    const NVConnectorEvoRec *pConnectorEvo;
    const struct NvKmsUsageBounds *pUsage;
    NvU32                 activeRmId;
    NVHwModeTimingsEvoPtr pTimings;
} NVValidateImpOneDispHeadParamsRec;

NvBool nvValidateImpOneDisp(
    NVDispEvoPtr                            pDispEvo,
    const NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvBool                                  requireBootClocks,
    NVEvoReallocateBandwidthMode            reallocBandwidth,
    NvU32                                   *pMinIsoBandwidthKBPS,
    NvU32                                   *pMinDramFloorKBPS);

NvBool nvAllocateDisplayBandwidth(
    NVDispEvoPtr pDispEvo,
    NvU32 newIsoBandwidthKBPS,
    NvU32 newDramFloorKBPS);

NvBool nvValidateImpOneDispDowngrade(
    NVDispEvoPtr                            pDispEvo,
    const NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvBool                                  requireBootClocks,
    NVEvoReallocateBandwidthMode            reallocBandwidth,
    NvU32                                   downgradePossibleHeadsBitMask);

NvBool nvFrameLockServerPossibleEvo(const NVDpyEvoRec *pDpyEvo);
NvBool nvFrameLockClientPossibleEvo(const NVDpyEvoRec *pDpyEvo);

void nvEvoSetLut(NVDispEvoPtr pDispEvo, NvU32 head, NvBool kickoff,
                 const struct NvKmsSetLutCommonParams *pParams);
NvBool nvValidateSetLutCommonParams(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsSetLutCommonParams *pParams);

void nvChooseCurrentColorSpaceAndRangeEvo(
    enum nvKmsPixelDepth pixelDepth,
    enum NvYuv420Mode yuv420Mode,
    enum NvKmsOutputTf tf,
    const enum NvKmsDpyAttributeRequestedColorSpaceValue requestedColorSpace,
    const enum NvKmsDpyAttributeColorRangeValue requestedColorRange,
    enum NvKmsDpyAttributeCurrentColorSpaceValue *pCurrentColorSpace,
    enum NvKmsDpyAttributeColorRangeValue *pCurrentColorRange);

void nvUpdateCurrentHardwareColorSpaceAndRangeEvo(
    NVDispEvoPtr pDispEvo,
    const NvU32 head,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorRangeValue colorRange,
    NVEvoUpdateState *pUpdateState);

NvBool nvAssignSOREvo(NVConnectorEvoPtr pConnectorEvo, NvU32 sorExcludeMask);

void nvSetSwapBarrierNotifyEvo(NVDispEvoPtr pDispEvo,
                               NvBool enable, NvBool isPre);

void nvUnbloatHwModeTimingsEvo(NVHwModeTimingsEvoPtr pTimings, NvU32 factor);

NvBool nvReadCRC32Evo(NVDispEvoPtr pDispEvo, NvU32 head,
                      CRC32NotifierCrcOut *crcOut /* out */);

NvBool nvFreeDevEvo(NVDevEvoPtr pDevEvo);
NVDevEvoPtr nvAllocDevEvo(const struct NvKmsAllocDeviceRequest *pRequest,
                          enum NvKmsAllocDeviceStatus *pStatus);
NvU32 nvGetActiveSorMask(const NVDispEvoRec *pDispEvo);
NvBool nvUpdateFlipLockEvoOneHead(NVDispEvoPtr pDispEvo, const NvU32 head,
                                  NvU32 *val, NvBool set,
                                  NvBool *needsEarlyUpdate,
                                  NVEvoUpdateState *updateState);

void nvEvoUpdateCurrentPalette(NVDispEvoPtr pDispEvo,
                               NvU32 head, NvBool kickOff);

NvBool nvEvoPollForNoMethodPending(NVDevEvoPtr pDevEvo,
                                   const NvU32 sd,
                                   NVEvoChannelPtr pChannel,
                                   NvU64 *pStartTime,
                                   const NvU32 timeout);

static inline void nvAssertSameSemaphoreSurface(
    const NVFlipChannelEvoHwState *pHwState)
{

    /*!
     * pHwState->syncObject contains separate fields to track the semaphore
     * surface used for acquire, and the semaphore surface used for release.
     * Prior to NvDisplay 4.0, display HW only supports using a single semaphore
     * surface for both acquire and release. As such, assert that the semaphore
     * surfaces in pHwState->syncObject are the same, and that we're also not
     * using syncpoints. This is enforced during flip validation.
     */

    nvAssert(pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo ==
             pHwState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo);

    nvAssert(!pHwState->syncObject.usingSyncpt);
}

void nvDPSerializerHandleDPIRQ(NVDispEvoPtr pDispEvo,
                               NVConnectorEvoPtr pConnectorEvo);

void nvDPSerializerPreSetMode(NVDispEvoPtr pDispEvo,
                              NVConnectorEvoPtr pConnectorEvo);

void nvDPSerializerPostSetMode(NVDispEvoPtr pDispEvo,
                               NVConnectorEvoPtr pConnectorEvo);

NvBool nvFramelockSetControlUnsyncEvo(NVDispEvoPtr pDispEvo, const NvU32 headMask,
                                      NvBool server);

NvBool nvIsHDRCapableHead(NVDispEvoPtr pDispEvo,
                          NvU32 apiHead);

NvU32 nvGetHDRSrcMaxLum(const NVFlipChannelEvoHwState *pHwState);

NvBool nvNeedsTmoLut(NVDevEvoPtr pDevEvo,
                     NVEvoChannelPtr pChannel,
                     const NVFlipChannelEvoHwState *pHwState,
                     NvU32 srcMaxLum,
                     NvU32 targetMaxCLL);

NvBool nvIsCscMatrixIdentity(const struct NvKmsCscMatrix *matrix);

void nvSuspendDevEvo(NVDevEvoRec *pDevEvo);
NvBool nvResumeDevEvo(NVDevEvoRec *pDevEvo);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_H__ */
