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

#include <nvkms-sync.h>

#include <nvmisc.h>
#include <class/cl917c.h> /* NV_DISP_BASE_NOTIFIER_1, NV_DISP_NOTIFICATION_2 */
#include <class/clc37d.h> /* NV_DISP_NOTIFIER */

/*
 * HW will never write 1 to lower 32bits of timestamp
 */
#define NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_LO_INVALID 1

/*
 * Higher 32bits of timestamp will be 0 only during first ~4sec of
 * boot. So for practical purposes, we can consider 0 as invalid.
 */
#define NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_HI_INVALID 0

static void GetNotifierTimeStamp(volatile const NvU32 *notif,
                                 NvU32 timeStampLoIdx,
                                 NvU32 timeStampHiIdx,
                                 struct nvKmsParsedNotifier *out)
{
    NvU32 lo, hi;
    NvU32 pollCount = 0;

    /*
     * Caller of ParseNotifier() is expected to poll for notifier
     * status to become BEGUN/FINISHED for valid timestamp.
     */
    if (out->status == NVKMS_NOTIFIER_STATUS_NOT_BEGUN) {
        return;
    }

    /*
     * HW does 4B writes to notifier, so poll till both timestampLo
     * and timestampHi bytes become valid.
     */
    do {
        lo = notif[timeStampLoIdx];
        hi = notif[timeStampHiIdx];

        if ((lo != NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_LO_INVALID) &&
            (hi != NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_HI_INVALID)) {
            out->timeStamp = (NvU64)lo | ((NvU64)hi << 32);
            out->timeStampValid = NV_TRUE;
            break;
        }

        if (++pollCount >=  100) {
            break;
        }
    } while (1);
}

static void SetNotifierLegacy(NvBool overlay, volatile void *in, NvBool begun,
                              NvU64 timeStamp)
{
    volatile NvU32 *notif = in;

    if (overlay) {
        notif[NV_DISP_NOTIFICATION_2_INFO16_3] = begun ?
            DRF_DEF(_DISP, _NOTIFICATION_2__3, _STATUS, _BEGUN) :
            DRF_DEF(_DISP, _NOTIFICATION_2__3, _STATUS, _NOT_BEGUN);

        notif[NV_DISP_NOTIFICATION_2_TIME_STAMP_0] = begun ? NvU64_LO32(timeStamp) :
            NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_LO_INVALID;
        notif[NV_DISP_NOTIFICATION_2_TIME_STAMP_1] = begun ? NvU64_HI32(timeStamp) :
            NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_HI_INVALID;
    } else {
        notif[NV_DISP_BASE_NOTIFIER_1__0] = begun ?
            DRF_DEF(_DISP, _BASE_NOTIFIER_1__0, _STATUS, _BEGUN) :
            DRF_DEF(_DISP, _BASE_NOTIFIER_1__0, _STATUS, _NOT_BEGUN);
    }
}

static void SetNotifierFourWord(volatile void *in, NvBool begun,
                                NvU64 timeStamp)
{
    volatile NvU32 *notif = in;

    notif[NV_DISP_NOTIFICATION_2_INFO16_3] = begun ?
        DRF_DEF(_DISP, _NOTIFICATION_2__3, _STATUS, _BEGUN) :
        DRF_DEF(_DISP, _NOTIFICATION_2__3, _STATUS, _NOT_BEGUN);

    notif[NV_DISP_NOTIFICATION_2_TIME_STAMP_0] = begun ? NvU64_LO32(timeStamp) :
        NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_LO_INVALID;
    notif[NV_DISP_NOTIFICATION_2_TIME_STAMP_1] = begun ? NvU64_HI32(timeStamp) :
        NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_HI_INVALID;
}

static void SetNotifierFourWordNVDisplay(volatile void *in, NvBool begun,
                                         NvU64 timeStamp)
{
    volatile NvU32 *notif = in;

    notif[NV_DISP_NOTIFIER__0] = begun ?
        DRF_DEF(_DISP, _NOTIFIER__0, _STATUS, _BEGUN) :
        DRF_DEF(_DISP, _NOTIFIER__0, _STATUS, _NOT_BEGUN);

    notif[NV_DISP_NOTIFIER__2] = begun ? NvU64_LO32(timeStamp) :
        NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_LO_INVALID;
    notif[NV_DISP_NOTIFIER__3] = begun ? NvU64_HI32(timeStamp) :
        NVKMS_LIB_SYNC_NOTIFIER_TIMESTAMP_HI_INVALID;
}

static void SetNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                        NvU32 index, void *base, NvBool begun, NvU64 timeStamp)
{
    const NvU32 sizeInBytes = nvKmsSizeOfNotifier(format, overlay);
    void *notif =
        (void *)((char *)base + (sizeInBytes * index));

    switch (format) {
    case NVKMS_NISO_FORMAT_LEGACY:
        SetNotifierLegacy(overlay, notif, begun, timeStamp);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
        SetNotifierFourWord(notif, begun, timeStamp);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        SetNotifierFourWordNVDisplay(notif, begun, timeStamp);
        break;
    }
}

void nvKmsSetNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                      NvU32 index, void *base, NvU64 timeStamp)
{
    SetNotifier(format, overlay, index, base, NV_TRUE, timeStamp);
}

void nvKmsResetNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                        NvU32 index, void *base)
{
    SetNotifier(format, overlay, index, base, NV_FALSE, 0);
}

static void ParseNotifierLegacy(NvBool overlay, volatile const void *in,
                                struct nvKmsParsedNotifier *out)
{
    volatile const NvU32 *notif = in;

    if (overlay) {
        NvU32 notif3;

        /* Read this once since it may be in video memory and we need multiple
         * fields */
        notif3 = notif[NV_DISP_NOTIFICATION_2_INFO16_3];

        switch(DRF_VAL(_DISP, _NOTIFICATION_2__3, _STATUS, notif3)) {
        case NV_DISP_NOTIFICATION_2__3_STATUS_NOT_BEGUN:
            out->status = NVKMS_NOTIFIER_STATUS_NOT_BEGUN;
            break;
        case NV_DISP_NOTIFICATION_2__3_STATUS_BEGUN:
            out->status = NVKMS_NOTIFIER_STATUS_BEGUN;
            break;
        case NV_DISP_NOTIFICATION_2__3_STATUS_FINISHED:
            out->status = NVKMS_NOTIFIER_STATUS_FINISHED;
            break;
        }

        out->presentCount =
            DRF_VAL(_DISP, _NOTIFICATION_2_INFO16_3, _PRESENT_COUNT, notif3);

        GetNotifierTimeStamp(notif,
                             NV_DISP_NOTIFICATION_2_TIME_STAMP_0,
                             NV_DISP_NOTIFICATION_2_TIME_STAMP_1,
                             out);
    } else {
        NvU32 notif0;

        /* There's a timestamp available in this notifier, but it's a weird
         * 14-bit "audit timestamp" that's not useful for us. */
        out->timeStampValid = NV_FALSE;

        /* Read this once since it may be in video memory and we need multiple
         * fields */
        notif0 = notif[NV_DISP_BASE_NOTIFIER_1__0];

        switch(DRF_VAL(_DISP, _BASE_NOTIFIER_1__0, _STATUS, notif0)) {
        case NV_DISP_BASE_NOTIFIER_1__0_STATUS_NOT_BEGUN:
            out->status = NVKMS_NOTIFIER_STATUS_NOT_BEGUN;
            break;
        case NV_DISP_BASE_NOTIFIER_1__0_STATUS_BEGUN:
            out->status = NVKMS_NOTIFIER_STATUS_BEGUN;
            break;
        case NV_DISP_BASE_NOTIFIER_1__0_STATUS_FINISHED:
            out->status = NVKMS_NOTIFIER_STATUS_FINISHED;
            break;
        }

        out->presentCount =
            DRF_VAL(_DISP, _BASE_NOTIFIER_1__0, _PRESENTATION_COUNT, notif0);
    }
}

static void ParseNotifierFourWord(const void *in,
                                  struct nvKmsParsedNotifier *out)
{
    volatile const NvU32 *notif = in;
    NvU32 notif3;

    /* Read this once since it may be in video memory and we need multiple
     * fields */
    notif3 = notif[NV_DISP_NOTIFICATION_2_INFO16_3];

    switch(DRF_VAL(_DISP, _NOTIFICATION_2__3, _STATUS, notif3)) {
    case NV_DISP_NOTIFICATION_2__3_STATUS_NOT_BEGUN:
        out->status = NVKMS_NOTIFIER_STATUS_NOT_BEGUN;
        break;
    case NV_DISP_NOTIFICATION_2__3_STATUS_BEGUN:
        out->status = NVKMS_NOTIFIER_STATUS_BEGUN;
        break;
    case NV_DISP_NOTIFICATION_2__3_STATUS_FINISHED:
        out->status = NVKMS_NOTIFIER_STATUS_FINISHED;
        break;
    }

    out->presentCount =
        DRF_VAL(_DISP, _NOTIFICATION_2_INFO16_3, _PRESENT_COUNT, notif3);

    GetNotifierTimeStamp(notif,
                         NV_DISP_NOTIFICATION_2_TIME_STAMP_0,
                         NV_DISP_NOTIFICATION_2_TIME_STAMP_1,
                         out);
}

static void ParseNotifierFourWordNVDisplay(const void *in,
                                           struct nvKmsParsedNotifier *out)
{
    volatile const NvU32 *notif = in;
    NvU32 notif0;

    /* Read this once since it may be in video memory and we need multiple
     * fields */
    notif0 = notif[NV_DISP_NOTIFIER__0];

    switch(DRF_VAL(_DISP, _NOTIFIER__0, _STATUS, notif0)) {
    case NV_DISP_NOTIFIER__0_STATUS_NOT_BEGUN:
        out->status = NVKMS_NOTIFIER_STATUS_NOT_BEGUN;
        break;
    case NV_DISP_NOTIFIER__0_STATUS_BEGUN:
        out->status = NVKMS_NOTIFIER_STATUS_BEGUN;
        break;
    case NV_DISP_NOTIFIER__0_STATUS_FINISHED:
        out->status = NVKMS_NOTIFIER_STATUS_FINISHED;
        break;
    }

    out->presentCount =
        DRF_VAL(_DISP, _NOTIFIER__0, _PRESENT_COUNT, notif0);

    GetNotifierTimeStamp(notif,
                         NV_DISP_NOTIFIER__2,
                         NV_DISP_NOTIFIER__3,
                         out);
}

void nvKmsParseNotifier(enum NvKmsNIsoFormat format, NvBool overlay,
                        NvU32 index, const void *base,
                        struct nvKmsParsedNotifier *out)
{
    const NvU32 sizeInBytes = nvKmsSizeOfNotifier(format, overlay);
    const void *notif =
        (const void *)((const char *)base + (sizeInBytes * index));

    switch (format) {
    case NVKMS_NISO_FORMAT_LEGACY:
        ParseNotifierLegacy(overlay, notif, out);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
        ParseNotifierFourWord(notif, out);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        ParseNotifierFourWordNVDisplay(notif, out);
        break;
    }
}

NvU32 nvKmsSemaphorePayloadOffset(enum NvKmsNIsoFormat format)
{
    switch (format) {
    case NVKMS_NISO_FORMAT_LEGACY:
        return 0;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
        return NV_DISP_NOTIFICATION_2_INFO32_2;
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        return NV_DISP_NOTIFIER__0;
    }

    return 0;
}

static void ResetSemaphoreLegacy(volatile void *in, NvU32 payload)
{
    volatile NvU32 *sema = in;

    *sema = payload;
}

static void ResetSemaphoreFourWord(volatile void *in, NvU32 payload)
{
    volatile NvU32 *sema = in;

    sema[NV_DISP_NOTIFICATION_2_INFO32_2] = payload;
}

static void ResetSemaphoreFourWordNVDisplay(volatile void *in, NvU32 payload)
{
    volatile NvU32 *sema = in;

    sema[NV_DISP_NOTIFIER__0] = payload;
}

void nvKmsResetSemaphore(enum NvKmsNIsoFormat format,
                         NvU32 index, void *base,
                         NvU32 payload)
{
    const NvU32 sizeInBytes = nvKmsSizeOfSemaphore(format);
    void *sema =
        (void *)((char *)base + (sizeInBytes * index));

    switch (format) {
    case NVKMS_NISO_FORMAT_LEGACY:
        ResetSemaphoreLegacy(sema, payload);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
        ResetSemaphoreFourWord(sema, payload);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        ResetSemaphoreFourWordNVDisplay(sema, payload);
        break;
    }
}

static NvU32 ParseSemaphoreLegacy(const volatile void *in)
{
    const volatile NvU32 *sema = in;

    return *sema;
}

static NvU32 ParseSemaphoreFourWord(const volatile void *in)
{
    const volatile NvU32 *sema = in;

    return sema[NV_DISP_NOTIFICATION_2_INFO32_2];
}

static NvU32 ParseSemaphoreFourWordNVDisplay(const volatile void *in)
{
    const volatile NvU32 *sema = in;

    return sema[NV_DISP_NOTIFIER__0];
}

void nvKmsParseSemaphore(enum NvKmsNIsoFormat format,
                         NvU32 index, const void *base,
                         struct nvKmsParsedSemaphore *out)
{
    const NvU32 sizeInBytes = nvKmsSizeOfSemaphore(format);
    const void *sema =
        (const void *)((const char *)base + (sizeInBytes * index));
    NvU32 payload = 0;

    switch (format) {
    case NVKMS_NISO_FORMAT_LEGACY:
        payload = ParseSemaphoreLegacy(sema);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD:
        payload = ParseSemaphoreFourWord(sema);
        break;
    case NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY:
        payload = ParseSemaphoreFourWordNVDisplay(sema);
        break;
    }

    out->payload = payload;
}
