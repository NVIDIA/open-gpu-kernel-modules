/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_KAPI_NOTIFIERS_H__

#define __NVKMS_KAPI_NOTIFIERS_H__

#include "nvkms-kapi-internal.h"

#define NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER 0x2
#define NVKMS_KAPI_NOTIFIER_SIZE 0x10

static inline NvU32 NVKMS_KAPI_INC_NOTIFIER_INDEX(const NvU32 index)
{
    return (index + 1) % NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER;
}

static inline NvU32 NVKMS_KAPI_DEC_NOTIFIER_INDEX(const NvU32 index)
{
    if (index == 0) {
        /*
         * Wrap "backwards" to the largest allowed notifier index.
         */
        return NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER - 1;
    }

    return index - 1;
}

static inline NvU32 NVKMS_KAPI_NOTIFIER_INDEX(NvU32 head, NvU32 layer,
                                              NvU32 index)
{
    NvU64 notifierIndex = 0;

    notifierIndex = head *
                    NVKMS_MAX_LAYERS_PER_HEAD *
                    NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER;

    notifierIndex += layer *
                     NVKMS_KAPI_MAX_NOTIFERS_PER_LAYER;

    notifierIndex += index;

    return notifierIndex;
}

static inline NvU32 NVKMS_KAPI_NOTIFIER_OFFSET(NvU32 head,
                                               NvU32 layer, NvU32 index)
{
    return NVKMS_KAPI_NOTIFIER_INDEX(head, layer, index) *
        NVKMS_KAPI_NOTIFIER_SIZE;
}

NvBool nvKmsKapiAllocateNotifiers(struct NvKmsKapiDevice *device, NvBool inVideoMemory);

void nvKmsKapiFreeNotifiers(struct NvKmsKapiDevice *device);

NvBool nvKmsKapiIsNotifierFinish(const struct NvKmsKapiDevice *device,
                                 const NvU32 head, const NvU32 layer,
                                 const NvU32 index);

void nvKmsKapiNotifierSetNotBegun(struct NvKmsKapiDevice *device,
                                  NvU32 head, NvU32 layer, NvU32 index);

#endif /* __NVKMS_KAPI_NOTIFIERS_H__ */
