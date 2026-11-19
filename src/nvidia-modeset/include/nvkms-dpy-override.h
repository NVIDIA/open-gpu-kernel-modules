/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_DPY_OVERRIDE_H__
#define __NVKMS_DPY_OVERRIDE_H__

#include "nvkms-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The GPU ID and display name of the display override that applies to all
 * displays of all GPUs.  A display override for a specific GPU and display
 * takes precedence over it.
 */
#define NV_DPY_OVERRIDE_ALL_GPU_ID  NV0000_CTRL_GPU_INVALID_ID
#define NV_DPY_OVERRIDE_ALL_NAME    "all"

typedef struct _DpyOverride {
    NVListRec entry;

    NvU32 gpuId;
    char name[NVKMS_DPY_NAME_SIZE];

    NvBool connected;
    NVEdidRec edid;

    /*
     * Upper limit for the DisplayPort link rate that the GPU supports on this
     * connector, in units of 10Mbps.  0 means no limit.
     */
    NvU32 dpMaxLinkRate10M;
} NVDpyOverrideRec, *NVDpyOverridePtr;

NVDpyOverrideRec *nvCreateDpyOverride(NvU32 gpuId,
                              const char *name,
                              NvBool connected,
                              const char *edid,
                              size_t edidSize);

NvBool nvSetDpyOverrideDpMaxLinkRate(NvU32 gpuId,
                                     const char *name,
                                     NvU32 linkRate10M);

NvU32 nvDpyEvoGetOverrideDpMaxLinkRate(const NVDpyEvoRec *pDpyEvo);

void nvDeleteDpyOverride(NvU32 gpuId, const char *name);

void nvLogDpyOverrides(NvU32 gpuId, NVEvoInfoStringPtr pInfoStr);

NVDpyOverridePtr nvDpyEvoGetOverride(const NVDpyEvoRec *pDpyEvo);
size_t nvReadDpyOverrideEdid(const NVDpyOverrideRec *pDpyOverride, NvU8 *buff,
                             size_t len);

void nvClearDpyOverrides(void);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_DPY_OVERRIDE_H__ */
