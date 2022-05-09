/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_RM_H__
#define __NVKMS_RM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvkms-types.h"
#include <class/cl0092.h> /* NV0092_REGISTER_RG_LINE_CALLBACK_FN */
#include <class/cl9010.h> /* OSVBLANKCALLBACKPROC */

NvBool nvWriteDPCDReg(NVConnectorEvoPtr pConnectorEvo,
                      NvU32 dpcdAddr,
                      NvU8 dpcdData);

NvBool nvRmRegisterCallback(const NVDevEvoRec *pDevEvo,
                            NVOS10_EVENT_KERNEL_CALLBACK_EX *cb,
                            struct nvkms_ref_ptr *ref_ptr,
                            NvU32 parentHandle,
                            NvU32 eventHandle,
                            Callback5ArgVoidReturn func,
                            NvU32 event);

enum NvKmsAllocDeviceStatus nvRmAllocDisplays(NVDevEvoPtr pDevEvo);
void nvRmDestroyDisplays(NVDevEvoPtr pDevEvo);
enum NvKmsBeginEndModeset {
    BEGIN_MODESET,
    END_MODESET
};
void nvRmBeginEndModeset(NVDispEvoPtr pDispEvo, enum NvKmsBeginEndModeset, NvU32 mask);
NvU32 nvRmAllocDisplayId(const NVDispEvoRec *pDispEvo, const NVDpyIdList dpyList);
void nvRmFreeDisplayId(const NVDispEvoRec *pDispEvo, NvU32 dpyId);
void nvRmGetConnectorORInfo(NVConnectorEvoPtr pConnectorEvo, NvBool assertOnly);
NVDpyIdList nvRmGetConnectedDpys(const NVDispEvoRec *pDispEvo,
                                 NVDpyIdList dpyIdList);
NvBool nvRmResumeDP(NVDevEvoPtr pDevEvo);
void nvRmPauseDP(NVDevEvoPtr pDevEvo);
NvBool nvRmSetDpmsEvo(NVDpyEvoPtr pDpyEvo, NvS64 value);
NvBool nvRmAllocSysmem(NVDevEvoPtr pDevEvo, NvU32 memoryHandle,
                       NvU32 *ctxDmaFlags, void **ppBase, NvU64 size,
                       NvKmsMemoryIsoType isoType);
NvBool nvRMAllocateBaseChannels(NVDevEvoPtr pDevEvo);
NvBool nvRMAllocateOverlayChannels(NVDevEvoPtr pDevEvo);
NvBool nvRMAllocateWindowChannels(NVDevEvoPtr pDevEvo);
NvBool nvRMSetupEvoCoreChannel(NVDevEvoPtr pDevEvo);
void nvRMFreeBaseChannels(NVDevEvoPtr pDevEvo);
void nvRMFreeOverlayChannels(NVDevEvoPtr pDevEvo);
void nvRMFreeWindowChannels(NVDevEvoPtr pDevEvo);
void nvRMFreeEvoCoreChannel(NVDevEvoPtr pDevEvo);
NvBool nvRMSyncEvoChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 errorToken);
NvBool nvRMIdleBaseChannel(NVDevEvoPtr pDevEvo, NvU32 head, NvU32 sd,
                           NvBool *stoppedBase);
NvBool nvRmEvoClassListCheck(const NVDevEvoRec *pDevEvo, NvU32 classID);
NvU32 nvRmEvoBindDispContextDMA(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 hCtxDma);
NvU32 nvRmEvoAllocateAndBindDispContextDMA(
    NVDevEvoPtr pDevEvo,
    NvU32 hMemory,
    const enum NvKmsSurfaceMemoryLayout layout,
    NvU64 limit);
NvBool nvRmEvoAllocAndBindSyncpt(
    NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel,
    NvU32 id,
    NvU32 *pSyncptHandle,
    NvU32 *pSyncptCtxDmaHandle);
void nvRmEvoFreePreSyncpt(NVDevEvoRec *pDevEvo,
                          NVEvoChannel *pChannel);
NvBool nvRmGarbageCollectSyncpts(
    NVDevEvoRec *pDevEvo);
void nvRmEvoFreeSyncpt(NVDevEvoRec *pDevEvo,
                       NVEvoSyncpt *pEvoSyncpt);
void nvRmEvoFreeDispContextDMA(NVDevEvoPtr pDevEvo,
                               NvU32 *hDispCtxDma);
void nvRmEvoUnMapVideoMemory(NVDevEvoPtr pDevEvo,
                             NvU32 memoryHandle,
                             void *subDeviceAddress[NVKMS_MAX_SUBDEVICES]);
NvBool nvRmEvoMapVideoMemory(NVDevEvoPtr pDevEvo,
                             NvU32 memoryHandle, NvU64 size,
                             void *subDeviceAddress[NVKMS_MAX_SUBDEVICES],
                             NvU32 subDeviceMask);
NvBool nvRmAllocDeviceEvo(NVDevEvoPtr pDevEvo,
                          const struct NvKmsAllocDeviceRequest *pRequest);
void nvRmFreeDeviceEvo(NVDevEvoPtr pDevEvo);
NvBool nvRmIsPossibleToActivateDpyIdList(NVDispEvoPtr pDispEvo,
                                         const NVDpyIdList dpyIdList);
NvBool nvRmVTSwitch(NVDevEvoPtr pDevEvo, NvU32 cmd);
NvBool nvRmGetVTFBInfo(NVDevEvoPtr pDevEvo);
void nvRmImportFbConsoleMemory(NVDevEvoPtr pDevEvo);
NvBool nvRmAllocEvoDma(NVDevEvoPtr pDevEvo,
                       NVEvoDmaPtr pDma,
                       NvU64 limit,
                       NvU32 ctxDmaFlags,
                       NvU32 subDeviceMask);
void nvRmFreeEvoDma(NVDevEvoPtr pDevEvo, NVEvoDmaPtr pDma);
NvBool nvRmQueryDpAuxLog(NVDispEvoRec *pDispEvo, NvS64 *pValue);
NvU64 nvRmGetGpuTime(NVDevEvoPtr pDevEvo);
NvBool nvRmSetGc6Allowed(NVDevEvoPtr pDevEvo, NvBool allowed);
NvU32 nvRmAddRgLine1Callback(
    const NVDispEvoRec *pDispEvo,
    NvU32 head,
    NV0092_REGISTER_RG_LINE_CALLBACK_FN pCallback);
void nvRmRemoveRgLine1Callback(const NVDispEvoRec *pDispEvo,
                               NvU32 callbackObjectHandle);
NvU32 nvRmAddVBlankCallback(
    const NVDispEvoRec *pDispEvo,
    NvU32 head,
    OSVBLANKCALLBACKPROC pCallback);
void nvRmRemoveVBlankCallback(const NVDispEvoRec *pDispEvo,
                              NvU32 callbackObjectHandle);
void nvRmMuxInit(NVDevEvoPtr pDevEvo);
NvBool nvRmMuxPre(const NVDpyEvoRec *pDpyEvo, NvMuxState state);
NvBool nvRmMuxSwitch(const NVDpyEvoRec *pDpyEvo, NvMuxState state);
NvBool nvRmMuxPost(const NVDpyEvoRec *pDpyEvo, NvMuxState state);
NvMuxState nvRmMuxState(const NVDpyEvoRec *pDpyEvo);

void nvRmRegisterBacklight(NVDispEvoRec *pDispEvo);
void nvRmUnregisterBacklight(NVDispEvoRec *pDispEvo);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_RM_H__ */
