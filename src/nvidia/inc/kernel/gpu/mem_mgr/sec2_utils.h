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

#include "g_sec2_utils_nvoc.h"

#ifndef SEC2_UTILS_H
#define SEC2_UTILS_H

#include "gpu/gpu_resource.h" // GpuResource

#include "kernel/gpu/mem_mgr/channel_utils.h"

#include "kernel/gpu/conf_compute/ccsl_context.h"

#define SEC2_AUTH_TAG_BUF_SIZE_BYTES (16)
#define SHA_256_HASH_SIZE_BYTES (32)

typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 offset;
    NvU64 length;
    NvU64 submittedWorkId; 
} SEC2UTILS_MEMSET_PARAMS;

typedef struct
{
    NvHandle hPhysMem;
    NvHandle hVirtMem;
    NvHandle hVASpace;
    NvU64    gpuVA;
    NvU64    size;
    MEMORY_DESCRIPTOR *pMemDesc;
} SEC2UTILS_BUFFER_INFO;

NVOC_PREFIX(sec2utils) class Sec2Utils : Object
{
public:
    NV_STATUS sec2utilsConstruct(Sec2Utils *psec2utils, OBJGPU *pGpu, KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance);

    void sec2utilsDestruct(Sec2Utils *psec2utils);

    NV_STATUS sec2utilsMemset(Sec2Utils *psec2utils, SEC2UTILS_MEMSET_PARAMS *pParams);

    NvU64 sec2utilsUpdateProgress(Sec2Utils *psec2utils);
    void sec2utilsServiceInterrupts(Sec2Utils *psec2utils);

    //
    // Internal states
    //

    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;

    OBJCHANNEL *pChannel;
    OBJGPU *pGpu;

    NvU32 sec2Class;
    NvU64 lastSubmittedPayload;
    NvU64 lastCompletedPayload;

    CCSL_CONTEXT *pCcslCtx;

    //
    // 4K page consisting of 128 auth tag buffers.
    // One buffer is used per sec2 method stream.
    // RM scrubber can submit multiple scrub method streams per sec2 operation.
    //
    SEC2UTILS_BUFFER_INFO scrubMthdAuthTagBuf;

    //
    // 4K page consisting of 128 auth tag buffers.
    // One buffer is used per scrubber operation (not per sec2 scrub method stream).
    // One scrub op corresponds to one call to sec2UtilsMemSet
    //
    SEC2UTILS_BUFFER_INFO semaMthdAuthTagBuf;

    //
    // Updated by RM to point to current auth tag buffer index (0-127)
    // so its incremented for every scrub method stream.
    //
    NvU32 authTagPutIndex;

    //
    // Updated by SEC2 engine to point to last used/ last "seen" auth tag buffer index.
    // This is updated based on completion of every scrub method stream.
    //
    NvU32 authTagGetIndex;
};

#endif // SEC2_UTILS_H
