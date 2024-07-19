/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/*
 * nv_gpu_ops.h
 *
 * This file defines the interface between the common RM layer
 * and the OS specific platform layers. (Currently supported
 * are Linux and KMD)
 *
 */

#ifndef _NV_GPU_OPS_H_
#define _NV_GPU_OPS_H_
#include "nvgputypes.h"
#include "nv_uvm_types.h"

typedef struct gpuSession       *gpuSessionHandle;
typedef struct gpuDevice        *gpuDeviceHandle;
typedef struct gpuAddressSpace  *gpuAddressSpaceHandle;
typedef struct gpuTsg           *gpuTsgHandle;
typedef struct gpuChannel       *gpuChannelHandle;
typedef struct gpuObject        *gpuObjectHandle;

typedef struct gpuRetainedChannel_struct gpuRetainedChannel;

NV_STATUS nvGpuOpsCreateSession(struct gpuSession **session);

NV_STATUS nvGpuOpsDestroySession(struct gpuSession *session);

NV_STATUS nvGpuOpsDeviceCreate(struct gpuSession *session,
                               const gpuInfo *pGpuInfo,
                               const NvProcessorUuid *gpuGuid,
                               struct gpuDevice **device,
                               NvBool bCreateSmcPartition);

NV_STATUS nvGpuOpsDeviceDestroy(struct gpuDevice *device);

NV_STATUS nvGpuOpsAddressSpaceCreate(struct gpuDevice *device,
                                     NvU64 vaBase,
                                     NvU64 vaSize,
                                     NvBool enableAts,
                                     gpuAddressSpaceHandle *vaSpace,
                                     UvmGpuAddressSpaceInfo *vaSpaceInfo);

NV_STATUS nvGpuOpsGetP2PCaps(gpuDeviceHandle device1,
                             gpuDeviceHandle device2,
                             getP2PCapsParams *p2pCaps);

void nvGpuOpsAddressSpaceDestroy(gpuAddressSpaceHandle vaSpace);

NV_STATUS nvGpuOpsMemoryAllocFb (gpuAddressSpaceHandle vaSpace,
    NvLength length, NvU64 *gpuOffset, gpuAllocInfo * allocInfo);

NV_STATUS nvGpuOpsMemoryAllocSys (gpuAddressSpaceHandle vaSpace,
    NvLength length, NvU64 *gpuOffset, gpuAllocInfo * allocInfo);

NV_STATUS nvGpuOpsPmaAllocPages(void *pPma,
                                NvLength pageCount,
                                NvU64 pageSize,
                                gpuPmaAllocationOptions *pPmaAllocOptions,
                                NvU64 *pPages);

void nvGpuOpsPmaFreePages(void *pPma,
                          NvU64 *pPages,
                          NvLength pageCount,
                          NvU64 pageSize,
                          NvU32 flags);

NV_STATUS nvGpuOpsPmaPinPages(void *pPma,
                              NvU64 *pPages,
                              NvLength pageCount,
                              NvU64 pageSize,
                              NvU32 flags);

NV_STATUS nvGpuOpsTsgAllocate(gpuAddressSpaceHandle vaSpace,
                              const gpuTsgAllocParams *params,
                              gpuTsgHandle *tsgHandle);

NV_STATUS nvGpuOpsChannelAllocate(const gpuTsgHandle tsgHandle,
                                  const gpuChannelAllocParams *params,
                                  gpuChannelHandle *channelHandle,
                                  gpuChannelInfo *channelInfo);

NV_STATUS nvGpuOpsMemoryReopen(struct gpuAddressSpace *vaSpace,
     NvHandle hSrcClient, NvHandle hSrcAllocation, NvLength length, NvU64 *gpuOffset);

void nvGpuOpsTsgDestroy(struct gpuTsg *tsg);

void nvGpuOpsChannelDestroy(struct gpuChannel *channel);

void nvGpuOpsMemoryFree(gpuAddressSpaceHandle vaSpace,
     NvU64 pointer);

NV_STATUS  nvGpuOpsMemoryCpuMap(gpuAddressSpaceHandle vaSpace,
                                NvU64 memory, NvLength length,
                                void **cpuPtr, NvU64 pageSize);

void nvGpuOpsMemoryCpuUnMap(gpuAddressSpaceHandle vaSpace,
     void* cpuPtr);

NV_STATUS nvGpuOpsQueryCaps(struct gpuDevice *device,
                            gpuCaps *caps);

NV_STATUS nvGpuOpsQueryCesCaps(struct gpuDevice *device,
                               gpuCesCaps *caps);

NV_STATUS nvGpuOpsDupAllocation(struct gpuAddressSpace *srcVaSpace,
                                NvU64 srcAddress,
                                struct gpuAddressSpace *dstVaSpace,
                                NvU64 dstVaAlignment,
                                NvU64 *dstAddress);

NV_STATUS nvGpuOpsDupMemory(struct gpuDevice *device,
                            NvHandle hClient,
                            NvHandle hPhysMemory,
                            NvHandle *hDupMemory,
                            gpuMemoryInfo *pGpuMemoryInfo);

NV_STATUS nvGpuOpsGetGuid(NvHandle hClient, NvHandle hDevice,
                          NvHandle hSubDevice, NvU8 *gpuGuid,
                          unsigned guidLength);

NV_STATUS nvGpuOpsGetClientInfoFromPid(unsigned pid,
                                       const NvU8 *gpuUuid,
                                       NvHandle *hClient,
                                       NvHandle *hDevice,
                                       NvHandle *hSubDevice);

NV_STATUS nvGpuOpsFreeDupedHandle(struct gpuDevice *device,
                                  NvHandle hPhysHandle);

NV_STATUS nvGpuOpsGetAttachedGpus(NvU8 *guidList, unsigned *numGpus);

NV_STATUS nvGpuOpsGetGpuInfo(const NvProcessorUuid *gpuUuid,
                             const gpuClientInfo *pGpuClientInfo,
                             gpuInfo *pGpuInfo);

NV_STATUS nvGpuOpsGetGpuIds(const NvU8 *pUuid, unsigned uuidLength, NvU32 *pDeviceId,
                            NvU32 *pSubdeviceId);

NV_STATUS nvGpuOpsOwnPageFaultIntr(struct gpuDevice *device, NvBool bOwnInterrupts);

NV_STATUS nvGpuOpsServiceDeviceInterruptsRM(struct gpuDevice *device);

NV_STATUS nvGpuOpsCheckEccErrorSlowpath(struct gpuChannel * channel, NvBool *bEccDbeSet);

NV_STATUS nvGpuOpsSetPageDirectory(struct gpuAddressSpace * vaSpace,
                                   NvU64 physAddress, unsigned numEntries,
                                   NvBool bVidMemAperture, NvU32 pasid);

NV_STATUS nvGpuOpsUnsetPageDirectory(struct gpuAddressSpace * vaSpace);

NV_STATUS nvGpuOpsGetGmmuFmt(struct gpuAddressSpace * vaSpace, void ** pFmt);

NV_STATUS nvGpuOpsInvalidateTlb(struct gpuAddressSpace * vaSpace);

NV_STATUS nvGpuOpsGetFbInfo(struct gpuDevice *device, gpuFbInfo * fbInfo);

NV_STATUS nvGpuOpsGetEccInfo(struct gpuDevice *device, gpuEccInfo * eccInfo);

NV_STATUS nvGpuOpsInitFaultInfo(struct gpuDevice *device, gpuFaultInfo *pFaultInfo);

NV_STATUS nvGpuOpsDestroyFaultInfo(struct gpuDevice *device,
                                   gpuFaultInfo *pFaultInfo);

NV_STATUS nvGpuOpsHasPendingNonReplayableFaults(gpuFaultInfo *pFaultInfo, NvBool *hasPendingFaults);

NV_STATUS nvGpuOpsGetNonReplayableFaults(gpuFaultInfo *pFaultInfo, void *faultBuffer, NvU32 *numFaults);

NV_STATUS nvGpuOpsDupAddressSpace(struct gpuDevice *device,
                                  NvHandle hUserClient,
                                  NvHandle hUserVASpace,
                                  struct gpuAddressSpace **vaSpace,
                                  UvmGpuAddressSpaceInfo *vaSpaceInfo);

NV_STATUS nvGpuOpsGetPmaObject(struct gpuDevice *device,
                               void **pPma,
                               const UvmPmaStatistics **pPmaPubStats);

NV_STATUS nvGpuOpsInitAccessCntrInfo(struct gpuDevice *device, gpuAccessCntrInfo *pAccessCntrInfo, NvU32 accessCntrIndex);

NV_STATUS nvGpuOpsDestroyAccessCntrInfo(struct gpuDevice *device,
                                        gpuAccessCntrInfo *pAccessCntrInfo);

NV_STATUS nvGpuOpsOwnAccessCntrIntr(struct gpuSession *session,
                                    gpuAccessCntrInfo *pAccessCntrInfo,
                                    NvBool bOwnInterrupts);

NV_STATUS nvGpuOpsEnableAccessCntr(struct gpuDevice *device,
                                   gpuAccessCntrInfo *pAccessCntrInfo,
                                   gpuAccessCntrConfig *pAccessCntrConfig);

NV_STATUS nvGpuOpsDisableAccessCntr(struct gpuDevice *device, gpuAccessCntrInfo *pAccessCntrInfo);

NV_STATUS nvGpuOpsP2pObjectCreate(struct gpuDevice *device1,
                                  struct gpuDevice *device2,
                                  NvHandle *hP2pObject);

NV_STATUS nvGpuOpsP2pObjectDestroy(struct gpuSession *session,
                              NvHandle hP2pObject);

NV_STATUS nvGpuOpsGetExternalAllocPtes(struct gpuAddressSpace *vaSpace,
                                       NvHandle hDupedMemory,
                                       NvU64 offset,
                                       NvU64 size,
                                       gpuExternalMappingInfo *pGpuExternalMappingInfo);

NV_STATUS nvGpuOpsRetainChannel(struct gpuAddressSpace *vaSpace,
                                NvHandle hClient,
                                NvHandle hChannel,
                                gpuRetainedChannel **retainedChannel,
                                gpuChannelInstanceInfo *channelInstanceInfo);

void nvGpuOpsReleaseChannel(gpuRetainedChannel *retainedChannel);

NV_STATUS nvGpuOpsBindChannelResources(gpuRetainedChannel *retainedChannel,
                                       gpuChannelResourceBindParams *channelResourceBindParams);

void nvGpuOpsStopChannel(gpuRetainedChannel *retainedChannel, NvBool bImmediate);

NV_STATUS nvGpuOpsGetChannelResourcePtes(struct gpuAddressSpace *vaSpace,
                                         NvP64 resourceDescriptor,
                                         NvU64 offset,
                                         NvU64 size,
                                         gpuExternalMappingInfo *pGpuExternalMappingInfo);

NV_STATUS nvGpuOpsReportNonReplayableFault(struct gpuDevice *device,
                                           const void *pFaultPacket);

// Private interface used for windows only

#if defined(NV_WINDOWS)
NV_STATUS nvGpuOpsGetRmHandleForSession(gpuSessionHandle hSession, NvHandle *hRmClient);

NV_STATUS nvGpuOpsGetRmHandleForChannel(gpuChannelHandle hChannel, NvHandle *hRmChannel);
#endif // WINDOWS

// Interface used for SR-IOV heavy

NV_STATUS nvGpuOpsPagingChannelAllocate(struct gpuDevice *device,
                                        const gpuPagingChannelAllocParams *params,
                                        gpuPagingChannelHandle *channelHandle,
                                        gpuPagingChannelInfo *channelinfo);

void nvGpuOpsPagingChannelDestroy(UvmGpuPagingChannel *channel);

NV_STATUS nvGpuOpsPagingChannelsMap(struct gpuAddressSpace *srcVaSpace,
                                    NvU64 srcAddress,
                                    struct gpuDevice *device,
                                    NvU64 *dstAddress);

void nvGpuOpsPagingChannelsUnmap(struct gpuAddressSpace *srcVaSpace,
                                 NvU64 srcAddress,
                                 struct gpuDevice *device);

NV_STATUS nvGpuOpsPagingChannelPushStream(UvmGpuPagingChannel *channel,
                                          char *methodStream,
                                          NvU32 methodStreamSize);

NV_STATUS nvGpuOpsFlushReplayableFaultBuffer(gpuFaultInfo *pFaultInfo,
                                             NvBool bCopyAndFlush);

NV_STATUS nvGpuOpsTogglePrefetchFaults(gpuFaultInfo *pFaultInfo,
                                       NvBool bEnable);

// Interface used for CCSL
NV_STATUS nvGpuOpsCcslContextInit(struct ccslContext_t **ctx,
                                  gpuChannelHandle channel);
NV_STATUS nvGpuOpsCcslContextClear(struct ccslContext_t *ctx);
NV_STATUS nvGpuOpsCcslRotateKey(UvmCslContext *contextList[],
                                NvU32 contextListCount);
NV_STATUS nvGpuOpsCcslRotateIv(struct ccslContext_t *ctx,
                               NvU8 direction);
NV_STATUS nvGpuOpsCcslEncrypt(struct ccslContext_t *ctx,
                              NvU32 bufferSize,
                              NvU8 const *inputBuffer,
                              NvU8 *outputBuffer,
                              NvU8 *authTagBuffer);
NV_STATUS nvGpuOpsCcslEncryptWithIv(struct ccslContext_t *ctx,
                                    NvU32 bufferSize,
                                    NvU8 const *inputBuffer,
                                    NvU8 *encryptIv,
                                    NvU8 *outputBuffer,
                                    NvU8 *authTagBuffer);
NV_STATUS nvGpuOpsCcslDecrypt(struct ccslContext_t *ctx,
                              NvU32 bufferSize,
                              NvU8 const *inputBuffer,
                              NvU8 const *decryptIv,
                              NvU32 keyRotationId,
                              NvU8 *outputBuffer,
                              NvU8 const *addAuthData,
                              NvU32 addAuthDataSize,
                              NvU8 const *authTagBuffer);
NV_STATUS nvGpuOpsCcslSign(struct ccslContext_t *ctx,
                           NvU32 bufferSize,
                           NvU8 const *inputBuffer,
                           NvU8 *authTagBuffer);
NV_STATUS nvGpuOpsQueryMessagePool(struct ccslContext_t *ctx,
                                   NvU8 direction,
                                   NvU64 *messageNum);
NV_STATUS nvGpuOpsIncrementIv(struct ccslContext_t *ctx,
                              NvU8 direction,
                              NvU64 increment,
                              NvU8 *iv);
NV_STATUS nvGpuOpsLogEncryption(struct ccslContext_t *ctx,
                                NvU8 direction,
                                NvU32 bufferSize);

#endif /* _NV_GPU_OPS_H_*/
