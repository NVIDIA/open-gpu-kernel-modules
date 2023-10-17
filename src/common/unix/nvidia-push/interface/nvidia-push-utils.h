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

/* This file contains push buffer utility functions and declarations */

#ifndef __NVIDIA_PUSH_UTILS_H__
#define __NVIDIA_PUSH_UTILS_H__

#include "nvidia-push-types.h"
#include "nvlimits.h"

#include "class/cla16f.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline NvBool nvPushIsAModel(const NvPushDeviceRec *pDevice)
{
    return FALSE;
}


/* declare prototypes: */
NvBool nvPushCheckChannelError(NvPushChannelPtr pChannel);
void nvPushKickoff(NvPushChannelPtr);
NvBool nvPushIdleChannelTest(NvPushChannelPtr pChannel, NvU32 timeoutMSec);
NvBool nvPushIdleChannel(NvPushChannelPtr);

void nvPushWaitForNotifier(
    NvPushChannelPtr pChannel,
    NvU32 notifierIndex,
    NvU32 subdeviceMask,
    NvBool yield,
    NvPushImportEvent *pEvent,
    int id);

void nvPushReleaseTimelineSemaphore(
    NvPushChannelPtr p,
    void *cpuAddress,
    NvU64 gpuAddress,
    NvU64 val);

void nvPushAcquireTimelineSemaphore(
    NvPushChannelPtr p,
    NvU64 gpuAddress,
    NvU64 val);

NvBool nvPushDecodeMethod(NvU32 header, NvU32 *count);
void nvPushSetObject(NvPushChannelPtr p, NvU32 subch, NvU32 *object);
void nvPushSetSubdeviceMask(NvPushChannelPtr p, NvU32 mask);
void __nvPushMakeRoom(NvPushChannelPtr, NvU32 count);

#define NV_PUSH_SUBDEVICE_MASK_PRIMARY 0x00000001
#define NV_PUSH_SUBDEVICE_MASK_ALL DRF_MASK(NVA16F_DMA_SET_SUBDEVICE_MASK_VALUE)

/*
 * Evaluates to TRUE if the two subDevMasks are equivalent for the given SLI
 * device
 */
static inline NvBool nvPushSubDeviceMaskEquiv(
    const NvPushDeviceRec *pDevice,
    NvU32 maskA,
    NvU32 maskB)
{
    const NvU32 allSubDevices = (1 << pDevice->numSubDevices) - 1;

    return (maskA & allSubDevices) == (maskB & allSubDevices);
}

/* Evaluates to TRUE if subDevMask will write to all of the GPUs */
static inline NvBool nvPushSubDeviceMaskAllActive(
    const NvPushDeviceRec *pDevice,
    NvU32 subDevMask)
{
    return nvPushSubDeviceMaskEquiv(pDevice, subDevMask,
                                    NV_PUSH_SUBDEVICE_MASK_ALL);
}

#define NV_PUSH_NOTIFIER_INTERNAL_BIT 0x80
ct_assert(NV_PUSH_NOTIFIER_INTERNAL_BIT >=
          NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1);
#define NV_PUSH_ERROR_NOTIFIER_INDEX \
            (NV_PUSH_NOTIFIER_INTERNAL_BIT | \
             NV_CHANNELGPFIFO_NOTIFICATION_TYPE_ERROR)
#define NV_PUSH_TOKEN_NOTIFIER_INDEX \
            (NV_PUSH_NOTIFIER_INTERNAL_BIT | \
             NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN)

/*
 * Notifiers for use by nvidia-push, not exposed to clients:
 * NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1: defined by RM
 * NV_MAX_SUBDEVICES: one for each subdevice to track work submission token
 */
#define NV_PUSH_NUM_INTERNAL_NOTIFIERS \
    (NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1 + NV_MAX_SUBDEVICES)

static inline NvU32 __nvPushGetNotifierRawIndex(
    const NvPushDeviceRec *pDevice,
    NvU32 notifierIndex,
    NvU32 sd)
{
    if (notifierIndex & NV_PUSH_NOTIFIER_INTERNAL_BIT) {
        return notifierIndex & ~NV_PUSH_NOTIFIER_INTERNAL_BIT;
    } else {
        return (notifierIndex * pDevice->numSubDevices) + sd +
                NV_PUSH_NUM_INTERNAL_NOTIFIERS;
    }
}

static inline NvNotification *nvPushGetNotifierCpuAddress(
    const NvPushChannelRec *pChannel,
    NvU32 notifierIndex,
    NvU32 sd)
{
    const NvU32 rawIndex =
        __nvPushGetNotifierRawIndex(pChannel->pDevice, notifierIndex, sd);

    return &pChannel->notifiers.cpuAddress[rawIndex];
}

static inline NvU64 nvPushGetNotifierGpuAddress(
    const NvPushChannelRec *pChannel,
    NvU32 notifierIndex,
    NvU32 sd)
{
    const NvU32 rawIndex =
        __nvPushGetNotifierRawIndex(pChannel->pDevice, notifierIndex, sd);
    const size_t offset = rawIndex * sizeof(NvNotification);

    return pChannel->notifiers.gpuAddress + offset;
}


extern NvU32 nvPushReadGetOffset(NvPushChannelPtr push_buffer, NvBool minimum);


/*!
 * Make room in the pushbuffer, checking for errors.
 *
 * If a channel error occurred, channelErrorOccurred is set to TRUE.
 * nvPushCheckForRoomAndErrors() is designed to be called just before a
 * nvPushMethod() with the same size.
 */
static inline void nvPushCheckForRoomAndErrors(
    NvPushChannelPtr pChannel,
    NvU32 count)
{
    pChannel->channelErrorOccurred = FALSE;

    if (pChannel->main.freeDwords < (count + 1)) {
        __nvPushMakeRoom(pChannel, count + 1);
    }
}

#ifdef __cplusplus
};
#endif

#endif /* __NVIDIA_PUSH_UTILS_H__ */
