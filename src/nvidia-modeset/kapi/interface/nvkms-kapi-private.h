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

#if !defined(__NVKMS_KAPI_PRIVATE_H__)
#define __NVKMS_KAPI_PRIVATE_H__

#include "nvtypes.h"
#include "nvkms-api.h"

struct NvKmsKapiPrivAllocateChannelEventParams {
    NvU32 hClient;
    NvU32 hChannel;
};

struct NvKmsKapiPrivSurfaceParams {
    enum NvKmsSurfaceMemoryLayout layout;

    struct {
        struct {
            NvU32 x;
            NvU32 y;
            NvU32 z;
        } log2GobsPerBlock;

        NvU32 pitchInBlocks;
        NvBool genericMemory;
    } blockLinear;
};

struct NvKmsKapiPrivImportMemoryParams {
    int memFd;
    struct NvKmsKapiPrivSurfaceParams surfaceParams;
};

struct NvKmsKapiPrivExportMemoryParams {
    int memFd;
};

#endif /* !defined(__NVKMS_KAPI_PRIVATE_H__) */
