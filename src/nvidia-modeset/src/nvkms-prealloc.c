/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-types.h"
#include "nvkms-flip-workarea.h"
#include "nvkms-modeset-types.h"
#include "nvkms-modeset-workarea.h"
#include "nvkms-setlut-workarea.h"
#include "nvkms-prealloc.h"
#include "nvkms-utils.h"

#include "nvkms-api.h"

#include <nvmisc.h>

static size_t GetSizeForType(NVDevEvoPtr pDevEvo, enum NVPreallocType type)
{
    switch (type) {
    case PREALLOC_TYPE_IMP_PARAMS:
        return pDevEvo->hal->caps.impStructSize;
    case PREALLOC_TYPE_SHUT_DOWN_HEADS_SET_MODE: /* fall through */
    case PREALLOC_TYPE_RESTORE_CONSOLE_SET_MODE:
        return sizeof(struct NvKmsSetModeParams);
    case PREALLOC_TYPE_MODE_SET_WORK_AREA:
        return sizeof(NVModeSetWorkArea);
    case PREALLOC_TYPE_FLIP_WORK_AREA:
        return sizeof(struct NvKmsFlipWorkArea);
    case PREALLOC_TYPE_PROPOSED_MODESET_HW_STATE: /* fallthrough */
    case PREALLOC_TYPE_VALIDATE_PROPOSED_MODESET_HW_STATE:
        return sizeof(NVProposedModeSetHwState);
    case PREALLOC_TYPE_VALIDATE_MODE_HW_MODE_TIMINGS: /* fallthrough */
    case PREALLOC_TYPE_HS_INIT_CONFIG_HW_TIMINGS:
        return sizeof(NVHwModeTimingsEvo);
    case PREALLOC_TYPE_VALIDATE_MODE_HDMI_FRL_CONFIG:
        return sizeof(HDMI_FRL_CONFIG);
    case PREALLOC_TYPE_VALIDATE_MODE_DSC_INFO:
        return sizeof(NVDscInfoEvoRec);
    case PREALLOC_TYPE_HS_PATCHED_MODESET_REQUEST:
        return sizeof(struct NvKmsSetModeRequest);
    case PREALLOC_TYPE_MODE_SET_REPLY_TMP_USAGE_BOUNDS:
        return sizeof(struct NvKmsUsageBounds);
    case PREALLOC_TYPE_VALIDATE_MODE_IMP_OUT_HW_MODE_TIMINGS:
        return sizeof(NVHwModeTimingsEvo) * NVKMS_MAX_HEADS_PER_DISP;
    case PREALLOC_TYPE_VALIDATE_MODE_TMP_USAGE_BOUNDS:
        return sizeof(struct NvKmsUsageBounds);
    case PREALLOC_TYPE_DPLIB_IS_MODE_POSSIBLE_PARAMS:
        return sizeof(NVDpLibIsModePossibleParamsRec);
    case PREALLOC_TYPE_SET_LUT_WORK_AREA:
        return sizeof(struct NvKmsSetLutWorkArea);
    case PREALLOC_TYPE_MAX:
        /* Not a real option, but added for -Wswitch-enum */
        break;
    }

    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
        "Unknown prealloc type %d in GetSizeForType.", type);

    return 0;
}

void *nvPreallocGet(
    NVDevEvoPtr pDevEvo,
    enum NVPreallocType type,
    size_t sizeCheck)
{
    struct NVDevPreallocRec *pPrealloc = &pDevEvo->prealloc;
    size_t size = GetSizeForType(pDevEvo, type);

    if (size != sizeCheck) {
        nvAssert(size == sizeCheck);
        return NULL;
    }

    if ((pPrealloc->used[type / 8] & NVBIT(type % 8)) != 0) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
            "Prealloc type %d already used in nvPreallocGet.", type);
        return NULL;
    }

    /* Since these are preallocated, they should not be NULL. */
    if (pPrealloc->ptr[type] == NULL) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
            "Prealloc type %d NULL in nvPreallocGet.", type);
    }

    pPrealloc->used[type / 8] |= NVBIT(type % 8);

    return pPrealloc->ptr[type];
}

void nvPreallocRelease(
    NVDevEvoPtr pDevEvo,
    enum NVPreallocType type)
{
    struct NVDevPreallocRec *pPrealloc = &pDevEvo->prealloc;

    if ((pPrealloc->used[type / 8] & NVBIT(type % 8)) == 0) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
            "Prealloc type %d not used in nvPreallocRelease.", type);
    }

    pPrealloc->used[type / 8] &= ~(NvU8)NVBIT(type % 8);
}

NvBool nvPreallocAlloc(NVDevEvoPtr pDevEvo)
{
    struct NVDevPreallocRec *pPrealloc = &pDevEvo->prealloc;
    NvU32 type;

    for (type = 0; type < PREALLOC_TYPE_MAX; type++) {
        size_t size = GetSizeForType(pDevEvo, type);
        if (size == 0) {
            goto fail;
        }
        pPrealloc->ptr[type] = nvAlloc(size);
        if (pPrealloc->ptr[type] == NULL) {
            goto fail;
        }
    }

    nvkms_memset(pPrealloc->used, 0, sizeof(pPrealloc->used));

    return TRUE;

fail:
    nvPreallocFree(pDevEvo);
    return FALSE;
}

void nvPreallocFree(NVDevEvoPtr pDevEvo)
{
    struct NVDevPreallocRec *pPrealloc = &pDevEvo->prealloc;
    NvU32 type;

    for (type = 0; type < PREALLOC_TYPE_MAX; type++) {
        if ((pDevEvo->prealloc.used[type / 8] & NVBIT(type % 8)) != 0) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                "Prealloc type %d still used in nvPreallocFree.", type);
        }

        nvFree(pPrealloc->ptr[type]);
        pPrealloc->ptr[type] = NULL;
    }
}
