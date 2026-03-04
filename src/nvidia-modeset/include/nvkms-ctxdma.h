/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_CTXDMA_H__
#define __NVKMS_CTXDMA_H__

#include "nvkms-types.h"
#include "nvkms-flip-workarea.h"

NvBool nvCtxDmaRegisterPreSyncpt(NVDevEvoRec *pDevEvo,
                                 struct NvKmsFlipWorkArea *pWorkArea);

void nvCtxDmaFreeSyncptHandle(NVDevEvoRec *pDevEvo, NVEvoSyncpt *pSyncpt);

NvU32 nvCtxDmaBind(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel, NvU32 hCtxDma);

void nvCtxDmaFree(NVDevEvoPtr pDevEvo, NvU32 deviceHandle, NvU32 *hDispCtxDma);

NvU32 nvCtxDmaAlloc(NVDevEvoPtr pDevEvo, NvU32 *pCtxDmaHandle,
                    NvU32  memoryHandle, NvU32 localCtxDmaFlags, NvU64 limit);

#endif /*  __NVKMS_CTXDMA_H__ */
