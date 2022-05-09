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

#if !defined(NVKMS_SYNC_H)
#define NVKMS_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvkms-api-types.h"

/* These functions are implemented in nvkms-lib. */

enum nvKmsNotifierStatus {
    NVKMS_NOTIFIER_STATUS_NOT_BEGUN,
    NVKMS_NOTIFIER_STATUS_BEGUN,
    NVKMS_NOTIFIER_STATUS_FINISHED,
};

struct nvKmsParsedNotifier {
    NvU64 timeStamp;
    NvBool timeStampValid;
    enum nvKmsNotifierStatus status;
    NvU8 presentCount;
};

static inline NvU32 nvKmsSizeOfNotifier(enum NvKmsNIsoFormat format,
                                        NvBool overlay) {
    switch (format) {
    default:
    case NVKMS_NISO_FORMAT_LEGACY:
        return overlay ? 16 : 4;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        return 16;
    }
}

void nvKmsResetNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                        NvU32 index, void *base);

void nvKmsParseNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                        NvU32 index, const void *base,
                        struct nvKmsParsedNotifier *out);

struct nvKmsParsedSemaphore {
    NvU32 payload;
};

static inline NvU32 nvKmsSizeOfSemaphore(enum NvKmsNIsoFormat format) {
    switch (format) {
    default:
    case NVKMS_NISO_FORMAT_LEGACY:
        return 4;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        return 16;
    }
}

NvU32 nvKmsSemaphorePayloadOffset(enum NvKmsNIsoFormat format);

void nvKmsResetSemaphore(enum NvKmsNIsoFormat format,
                         NvU32 index, void *base,
                         NvU32 payload);

void nvKmsParseSemaphore(enum NvKmsNIsoFormat format,
                         NvU32 index, const void *base,
                         struct nvKmsParsedSemaphore *out);

#ifdef __cplusplus
};
#endif

#endif /* NVKMS_SYNC_H */
