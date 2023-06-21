/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * This file contains nvidia-push device and channel setup structures and
 * functions.
 */

#ifndef __NVIDIA_PUSH_INIT_H__
#define __NVIDIA_PUSH_INIT_H__


#include "nvidia-push-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Return the index of the first class table element supported on this device.
 *
 * pClassTable is an array where each element corresponds to a class
 * the caller supports.  The first field in the array element should
 * be an NvPushSupportedClass struct.  There may be additional fields
 * in the array element that are specific to the caller.  The
 * classTableStride argument indicates the size in bytes of one array
 * element, such that nvPushGetSupportedClassIndex() can step from one
 * array element to the next by adding classTableStride.
 *
 * nvPushGetSupportedClassIndex() will query the list of classes
 * supported by this device, and return the index of the first
 * pClassTable array element that is supported by the device.  -1 is
 * returned if there is no match.
 *
 * \param pDevice          The nvidia-push device whose class list to consider.
 * \param pClassTable      The table of classes supported.
 * \param classTableStride The size in bytes of one table element.
 * \param classTableLength The number of table elements.
 *
 * \return  The index of the first table element that matches, or -1.
 */

typedef struct _NvPushSupportedClass {
    NvU32 classNumber;
    NVAModelConfig amodelConfig;
} NvPushSupportedClass;

int nvPushGetSupportedClassIndex(
    NvPushDevicePtr pDevice,
    const void *pClassTable,
    size_t classTableStride,
    size_t classTableLength);

/*
 * Parameter structure populated by the host driver when requesting an
 * NvPushDeviceRec.
 */
typedef struct _NvPushAllocDeviceParams {

    /* Pointer to host device, filled by host driver as needed */
    void           *hostDevice;

    const NvPushImports *pImports;

    /* The host driver's RMAPI client (NV0000) handle. */
    NvU32           clientHandle;

    /* TRUE iff this device is in client-side SLI mode. */
    NvBool          clientSli;

    /* The number of subDevices allocated by the host driver. */
    NvU32           numSubDevices;

    struct {
        /* The host driver's RMAPI device (NV0080) handles */
        NvU32       deviceHandle;
        /* The host driver's RMAPI subDevice (NV2080) handles. */
        NvU32       handle;
        /* FERMI_VASPACE_A object in which channels on this device should be
         * mapped. */
        NvU32       gpuVASpaceObject;
        /* ctxDma handle to be used with MapMemoryDma. */
        NvU32       gpuVASpace;
    } subDevice[NV_MAX_SUBDEVICES];

    struct {
        /*
         * The Amodel configuration requested by the host driver.
         */
        NVAModelConfig config;
    } amodel;

    /* Whether channels on this device will be used to program Tegra. */
    NvBool          isTegra;

    /*
     * Pool of RMAPI object handles.  The host driver should populate
     * all of the elements in this array before calling
     * nvPushAllocDevice(), and release all of these handles if
     * nvPushAllocDevice() fails, or after calling nvPushFreeDevice().
     *
     * The number of possible handles is:
     *
     *  hUserMode (per-sd)
     */
#define NV_PUSH_DEVICE_HANDLE_POOL_NUM \
    (NV_MAX_SUBDEVICES)

    NvU32           handlePool[NV_PUSH_DEVICE_HANDLE_POOL_NUM];

    NvU32           numClasses;
    const NvU32    *supportedClasses;

    NvPushConfidentialComputeMode confidentialComputeMode;
} NvPushAllocDeviceParams;

NvBool nvPushAllocDevice(
    const NvPushAllocDeviceParams *pParams,
    NvPushDevicePtr pDevice);

void nvPushFreeDevice(
    NvPushDevicePtr pDevice);


/*
 * Parameter structure populated by the host driver when requesting an
 * NvPushChannelRec.
 */
typedef struct _NvPushAllocChannelParams {

    /* NV2080_ENGINE_TYPE_ */
    NvU32           engineType;

    /*
     * Whether to log the pushbuffer in nvdiss format, by calling
     * nvPushImportLogNvDiss().
     */
    NvBool          logNvDiss;

    /*
     * Normally, the pushbuffer utility library will time out when
     * waiting for things (space in the pushbuffer, waiting for
     * notifiers, etc).  When the channel is created with
     * noTimeout=TRUE, the channel will wait indefinitely for these
     * things.
     */
    NvBool          noTimeout;

    /*
     * Normally, the pushbuffer utility library checks for channel
     * errors and reports them to the host driver by calling
     * nvPushImportChannelErrorOccurred().  Host drivers can set
     * ignoreChannelErrors=TRUE to disable this check.
     */
    NvBool          ignoreChannelErrors;

    /*
     * This flag specifies if channel is intended to be used for
     * encryption/decryption of data between SYSMEM <-> VIDMEM. Only CE
     * & SEC2 Channels are capable of handling encrypted content.
     */
    NvBool          secureChannel;

    /*
     * DIFR stands for Display Idle Frame Refresh in which a CE is used to
     * prefetch framebuffer pixels into the GPU's L2 cache. The prefetch
     * operation requires the channel to be specifically configured for DIFR
     * prefetching. This flag indicates if this channel is intended to be
     * used for just that.
     */
    NvBool          difrPrefetch;

    /*
     * Host drivers should specify how many notifiers they want.  The
     * pushbuffer utility library will allocate memory to hold this
     * many notifiers on each subDevice, plus an error notifier.
     *
     * The 'notifierIndex' argument to, e.g., nvPushGetNotifierCpuAddress()
     * should be in the range [0,numNotifiers).
     */
    NvU8            numNotifiers;

    /*
     * The size of the "main" pushbuffer in bytes.  Note this does not
     * include space for gpfifo entries or progress tracking:
     * nvidia-push will implicitly pad the total pushbuffer for those
     * items.
     */
    NvU32           pushBufferSizeInBytes;

    /*
     * Pool of RMAPI object handles.  The host driver should populate
     * all of the elements in this array before calling
     * nvPushAllocChannel(), and release all of these handles if
     * nvPushAllocChannel() fails, or after calling nvPushFreeChannel().
     *
     * The number of possible handles is:
     *
     *  progressSemaphore hMemory (per-sd) +
     *  pushbufferHandle (per-device)      +
     *  pushbufferVAHandle (per-sd)        +
     *  userD.hMemory (per-sd)             +
     *  channelHandle (per-sd)             +
     *  notifier memoryHandle (per-device) +
     *  error notifier ctxDma (per-device)
     */
#define NV_PUSH_CHANNEL_HANDLE_POOL_NUM \
    (NV_MAX_SUBDEVICES     +    \
     1                     +    \
     NV_MAX_SUBDEVICES     +    \
     NV_MAX_SUBDEVICES     +    \
     NV_MAX_SUBDEVICES     +    \
     1                     +    \
     1)

    NvU32           handlePool[NV_PUSH_CHANNEL_HANDLE_POOL_NUM];

    /*
     * A pointer to an NvPushDeviceRec, initialized with
     * nvPushAllocDevice().  One or more NvPushChannelRecs may share
     * the same NvPushDevicePtr.
     *
     * This pDevice should be kept allocated until all
     * NvPushChannelRecs using it have been freed.
     */
    NvPushDevicePtr pDevice;

} NvPushAllocChannelParams;

NvBool nvPushAllocChannel(
    const NvPushAllocChannelParams *pParams,
    NvPushChannelPtr buffer);

void nvPushFreeChannel(
    NvPushChannelPtr buffer);


void nvPushInitWaitForNotifier(
    NvPushChannelPtr pChannel,
    NvU32 notifierIndex,
    NvU32 subdeviceMask);

#ifdef __cplusplus
};
#endif

#endif /*__NVIDIA_PUSH_INIT_H__ */
