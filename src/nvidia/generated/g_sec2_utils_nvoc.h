
#ifndef _G_SEC2_UTILS_NVOC_H_
#define _G_SEC2_UTILS_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

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

#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SEC2_UTILS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct Sec2Utils {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Sec2Utils *__nvoc_pbase_Sec2Utils;    // sec2utils

    // Data members
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    OBJCHANNEL *pChannel;
    struct OBJGPU *pGpu;
    NvU32 sec2Class;
    NvU64 lastSubmittedPayload;
    NvU64 lastCompletedPayload;
    CCSL_CONTEXT *pCcslCtx;
    SEC2UTILS_BUFFER_INFO scrubMthdAuthTagBuf;
    SEC2UTILS_BUFFER_INFO semaMthdAuthTagBuf;
    NvU32 authTagPutIndex;
    NvU32 authTagGetIndex;
};

#ifndef __NVOC_CLASS_Sec2Utils_TYPEDEF__
#define __NVOC_CLASS_Sec2Utils_TYPEDEF__
typedef struct Sec2Utils Sec2Utils;
#endif /* __NVOC_CLASS_Sec2Utils_TYPEDEF__ */

#ifndef __nvoc_class_id_Sec2Utils
#define __nvoc_class_id_Sec2Utils 0x7716b1
#endif /* __nvoc_class_id_Sec2Utils */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Utils;

#define __staticCast_Sec2Utils(pThis) \
    ((pThis)->__nvoc_pbase_Sec2Utils)

#ifdef __nvoc_sec2_utils_h_disabled
#define __dynamicCast_Sec2Utils(pThis) ((Sec2Utils*)NULL)
#else //__nvoc_sec2_utils_h_disabled
#define __dynamicCast_Sec2Utils(pThis) \
    ((Sec2Utils*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Sec2Utils)))
#endif //__nvoc_sec2_utils_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Sec2Utils(Sec2Utils**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Sec2Utils(Sec2Utils**, Dynamic*, NvU32, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance);
#define __objCreate_Sec2Utils(ppNewObj, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance) \
    __nvoc_objCreate_Sec2Utils((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_pKernelMIGGPUInstance)


// Wrapper macros

// Dispatch functions
NV_STATUS sec2utilsConstruct_IMPL(struct Sec2Utils *arg_psec2utils, struct OBJGPU *arg_pGpu, KERNEL_MIG_GPU_INSTANCE *arg_pKernelMIGGPUInstance);

#define __nvoc_sec2utilsConstruct(arg_psec2utils, arg_pGpu, arg_pKernelMIGGPUInstance) sec2utilsConstruct_IMPL(arg_psec2utils, arg_pGpu, arg_pKernelMIGGPUInstance)
void sec2utilsDestruct_IMPL(struct Sec2Utils *psec2utils);

#define __nvoc_sec2utilsDestruct(psec2utils) sec2utilsDestruct_IMPL(psec2utils)
NV_STATUS sec2utilsMemset_IMPL(struct Sec2Utils *psec2utils, SEC2UTILS_MEMSET_PARAMS *pParams);

#ifdef __nvoc_sec2_utils_h_disabled
static inline NV_STATUS sec2utilsMemset(struct Sec2Utils *psec2utils, SEC2UTILS_MEMSET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Utils was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_sec2_utils_h_disabled
#define sec2utilsMemset(psec2utils, pParams) sec2utilsMemset_IMPL(psec2utils, pParams)
#endif //__nvoc_sec2_utils_h_disabled

NvU64 sec2utilsUpdateProgress_IMPL(struct Sec2Utils *psec2utils);

#ifdef __nvoc_sec2_utils_h_disabled
static inline NvU64 sec2utilsUpdateProgress(struct Sec2Utils *psec2utils) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Utils was disabled!");
    return 0;
}
#else //__nvoc_sec2_utils_h_disabled
#define sec2utilsUpdateProgress(psec2utils) sec2utilsUpdateProgress_IMPL(psec2utils)
#endif //__nvoc_sec2_utils_h_disabled

void sec2utilsServiceInterrupts_IMPL(struct Sec2Utils *psec2utils);

#ifdef __nvoc_sec2_utils_h_disabled
static inline void sec2utilsServiceInterrupts(struct Sec2Utils *psec2utils) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Utils was disabled!");
}
#else //__nvoc_sec2_utils_h_disabled
#define sec2utilsServiceInterrupts(psec2utils) sec2utilsServiceInterrupts_IMPL(psec2utils)
#endif //__nvoc_sec2_utils_h_disabled

#undef PRIVATE_FIELD


#endif // SEC2_UTILS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SEC2_UTILS_NVOC_H_
