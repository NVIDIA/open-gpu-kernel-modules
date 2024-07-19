/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file contains core definitions (structures and enums) for use in the
 * rest of the nvidia-push code.
 */

#ifndef __NVIDIA_PUSH_TYPES_H__
#define __NVIDIA_PUSH_TYPES_H__

#include <stddef.h>          /* size_t */



#include "nvtypes.h"
#include "nvlimits.h"
#include "nvmisc.h"
#include "nvgputypes.h"      /* NvNotificationRec */
#include "nv_common_utils.h" /* TRUE/FALSE */
#include "nvctassert.h"
#include "nv_assert.h"       /* nvAssert() */
#include "nv_amodel_enum.h"  /* NVAModelConfig */
#include "nvos.h"            /* NV_CHANNELGPFIFO_NOTIFICATION_* */

#ifdef __cplusplus
extern "C" {
#endif

#define NV_PUSH_NOTIFIER_SHORT_TIMEOUT 3000 /* in milliseconds (ie:  3 seconds) */
#define NV_PUSH_NOTIFIER_LONG_TIMEOUT 10000 /* in milliseconds (ie: 10 seconds) */

# define NV_PUSH_PRINTF_FORMAT_ARGUMENT
# define NV_PUSH_PRINTF_ATTRIBUTES(_fmt,_var) \
    __attribute__((format (printf, _fmt, _var)))


#if defined(NV_PUSH_IN_KERNEL)
#  define NV_PUSH_ALLOW_FLOAT 0
#else
#  define NV_PUSH_ALLOW_FLOAT 1
#endif

typedef union _NvPushChannelUnion
{
    NvU32 u;
#if NV_PUSH_ALLOW_FLOAT
    float f;
#endif
} NvPushChannelUnion;

typedef enum _NvPushConfidentialComputeMode {
    /* Confidential computing is not in use. */
    NV_PUSH_CONFIDENTIAL_COMPUTE_MODE_NONE,

    /*
     * The confidential compute mode of operation is Hopper Confidential
     * Compute (HCC).
     */
    NV_PUSH_CONFIDENTIAL_COMPUTE_MODE_HCC,
} NvPushConfidentialComputeMode;

typedef struct _NvPushChannelRec NvPushChannelRec;
typedef struct _NvPushChannelRec *NvPushChannelPtr;

typedef struct _nv_push_hal {
    void (*kickoff)(struct _NvPushChannelRec*, NvU32 oldGpPut, NvU32 newGpPut);
    void (*releaseTimelineSemaphore)(NvPushChannelPtr, void *cpuAddress, NvU64 gpuAddress, NvU64 val);
    void (*acquireTimelineSemaphore)(NvPushChannelPtr, NvU64 gpuAddress, NvU64 val);
    struct {
        /* Requires USERD memory to be specified at channel allocation */
        NvU32 clientAllocatesUserD                      :1;

        /* On Tegra, we currently need to allocate double the requested GPFIFO
         * entries */
        NvU32 allocateDoubleSizeGpFifo                  :1;

        /* Use Volta+ semaphore methods */
        NvU32 voltaSemMethods                           :1;

        NvU32 extendedBase                              :1;
    } caps;
} NvPushHal;

typedef struct _NvPushDeviceRec {

    void           *hostDevice;     /* Provided by the host driver */

    NvBool          hostLBoverflowBug1667921 : 1;
    NvBool          clientSli : 1;   /* Provided by the host driver */
    NvBool          hasFb : 1;       /* Computed from supportedClasses[] */

    NvU32           clientHandle;    /* Provided by the host driver */
    NvU32           numSubDevices;   /* Provided by the host driver */

    NvU32           numClasses;      /* Provided by the host driver */
    const NvU32    *supportedClasses;/* Provided by the host driver */

    struct {
        NvU32       handle;          /* Provided by the host driver */
        NvU32       deviceHandle;    /* Provided by the host driver */
        NvU32       gpuVASpaceObject;/* Provided by the host driver */
        NvU32       gpuVASpaceCtxDma;/* Provided by the host driver */
        NvU32       hUserMode;       /* VOLTA_USERMODE_A object */
        void       *pUserMode;       /* VOLTA_USERMODE_A mapping */
    } subDevice[NV_MAX_SUBDEVICES];

    NvU32           gpfifoClass;
    size_t          userDSize;

    NVAModelConfig  amodelConfig;

    NvPushHal hal;
    const struct _NvPushImports *pImports;

    /* Provided by the host driver */
    NvPushConfidentialComputeMode confidentialComputeMode;
} NvPushDeviceRec, *NvPushDevicePtr;


typedef struct _NvPushChannelSegmentRec
{
    NvU32               freeDwords;  // free space (in dwords)
    NvU32               sizeInBytes; // Push buffer size (in bytes)
    NvU32               putOffset;   // Offset of last kickoff
    NvPushChannelUnion *base;        // Push buffer start pointer
    NvPushChannelUnion *buffer;      // Push buffer current pointer
    NvU64               gpuMapOffset;
} NvPushChannelSegmentRec, *NvPushChannelSegmentPtr;

struct _NvPushChannelRec
{
    NvBool          initialized              : 1;
    NvBool          logNvDiss                : 1;
    NvBool          noTimeout                : 1;
    NvBool          ignoreChannelErrors      : 1;
    NvBool          channelErrorOccurred     : 1;

    NvU32           channelHandle[NV_MAX_SUBDEVICES];
    NvU32           pushbufferHandle;
    NvU32           pushbufferVAHandle[NV_MAX_SUBDEVICES];
    NvPushChannelSegmentRec main;

    void           *control[NV_MAX_SUBDEVICES];
    NvU32           numGpFifoEntries;
    NvU32          *gpfifo;  // GPFIFO entries
    NvU32           gpPutOffset; // GPFIFO entries last kicked off offset
    NvU32           currentSubDevMask;

    NvPushChannelSegmentRec progressTracker;
    struct {
        NvU32       handle[NV_MAX_SUBDEVICES];
        void       *ptr[NV_MAX_SUBDEVICES];
        NvU64       gpuVA;
    } progressSemaphore;

    struct {
        NvU32 hMemory;
    } userD[NV_MAX_SUBDEVICES];

    struct {
        NvU8            num;
        NvU32           memoryHandle;
        NvNotification *cpuAddress;
        NvU64           gpuAddress;
        NvU32           errorCtxDma;
    } notifiers;

    NvPushDeviceRec *pDevice;
};

/* Opaque type, only used by pointer within the push buffer utility library. */
typedef struct _NvPushImportEvent NvPushImportEvent;

/* Table of function pointers to be provided by the nvidia-push host driver. */
typedef struct _NvPushImports {

    NvU32  (*rmApiControl)          (NvPushDevicePtr pDevice,
                                     NvU32 hObject,
                                     NvU32 cmd,
                                     void *pParams,
                                     NvU32 paramsSize);

    NvU32  (*rmApiAlloc)            (NvPushDevicePtr pDevice,
                                     NvU32 hParent,
                                     NvU32 hObject,
                                     NvU32 hClass,
                                     void *pAllocParams);

    NvU32  (*rmApiFree)             (NvPushDevicePtr pDevice,
                                     NvU32 hParent,
                                     NvU32 hObject);

    NvU32  (*rmApiMapMemoryDma)     (NvPushDevicePtr pDevice,
                                     NvU32 hDevice,
                                     NvU32 hDma,
                                     NvU32 hMemory,
                                     NvU64 offset,
                                     NvU64 length,
                                     NvU32 flags,
                                     NvU64 *pDmaOffset);

    NvU32  (*rmApiUnmapMemoryDma)   (NvPushDevicePtr pDevice,
                                     NvU32 hDevice,
                                     NvU32 hDma,
                                     NvU32 hMemory,
                                     NvU32 flags,
                                     NvU64 dmaOffset);

    NvU32  (*rmApiAllocMemory64)    (NvPushDevicePtr pDevice,
                                     NvU32 hParent,
                                     NvU32 hMemory,
                                     NvU32 hClass,
                                     NvU32 flags,
                                     void **ppAddress,
                                     NvU64 *pLimit);

    NvU32  (*rmApiVidHeapControl)   (NvPushDevicePtr pDevice,
                                     void *pVidHeapControlParms);

    NvU32  (*rmApiMapMemory)        (NvPushDevicePtr pDevice,
                                     NvU32 hDevice,
                                     NvU32 hMemory,
                                     NvU64 offset,
                                     NvU64 length,
                                     void **ppLinearAddress,
                                     NvU32 flags);

    NvU32  (*rmApiUnmapMemory)      (NvPushDevicePtr pDevice,
                                     NvU32 hDevice,
                                     NvU32 hMemory,
                                     void *pLinearAddress,
                                     NvU32 flags);

    NvU64  (*getMilliSeconds)       (NvPushDevicePtr pDevice);

    void   (*yield)                 (NvPushDevicePtr pDevice);

    NvBool (*waitForEvent)          (NvPushDevicePtr pDevice,
                                     NvPushImportEvent *pEvent,
                                     NvU64 timeout);

    void   (*emptyEventFifo)        (NvPushDevicePtr pDevice,
                                     NvPushImportEvent *pEvent);

    void   (*channelErrorOccurred)  (NvPushChannelPtr pChannel, NvU32 channelErrCode);

    void   (*pushbufferWrapped)     (NvPushChannelPtr pChannel);

    void   (*logError)              (NvPushDevicePtr pDevice,
                                     NV_PUSH_PRINTF_FORMAT_ARGUMENT const char *fmt, ...)
        NV_PUSH_PRINTF_ATTRIBUTES(2,3);

    /*
     * The logNvDiss() import, in DEBUG builds, logs strings to be
     * parsed by nvdiss.  Note that multiple nvPushImportLogNvDiss()
     * calls may be used to build one line of output (so, respect the
     * newlines provided in the strings).
     */
#if defined(DEBUG)
    void   (*logNvDiss)              (NvPushChannelPtr pChannel,
                                      NV_PUSH_PRINTF_FORMAT_ARGUMENT const char *fmt, ...)
        NV_PUSH_PRINTF_ATTRIBUTES(2,3);
#endif

} NvPushImports;


void __nvPushMakeRoom(NvPushChannelPtr, NvU32 count);

#ifdef __cplusplus
};
#endif

#endif /* __NVIDIA_PUSH_TYPES_H__ */
