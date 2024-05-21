
#ifndef _G_KERNEL_VIDEO_ENGINE_NVOC_H_
#define _G_KERNEL_VIDEO_ENGINE_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_video_engine_nvoc.h"

#ifndef KERNEL_VIDEO_ENGINE_H
#define KERNEL_VIDEO_ENGINE_H

#include "kernel/core/core.h"
#include "kernel/gpu/eng_desc.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/gpu_halspec.h"
#include "kernel/mem_mgr/mem.h"
#include "kernel/gpuvideo/video_event.h"

#define IS_VIDEO_ENGINE(engDesc) (IS_NVDEC(engDesc) || IS_OFA(engDesc) || IS_NVJPEG(engDesc) || IS_MSENC(engDesc))

typedef struct
{
    /*!
     * Trace buffer for Kernel events.
     */
    MEMORY_DESCRIPTOR *pTraceBufferEngineMemDesc;

    /*!
     * Mapped memdesc pointer for kernel events
     */
    VIDEO_TRACE_RING_BUFFER *pTraceBufferEngine;

    /*!
     * Local scratch system memory for variable data.
     */
    NvU8 *pTraceBufferVariableData;

    /*!
     * Timestamp for Engine start event used for generate noisy timestamp
     */
    NvU64 noisyTimestampStart;

#if PORT_IS_MODULE_SUPPORTED(crypto)
    /*!
     * Random number generator used for generate noisy timestamp
     */
    PORT_CRYPTO_PRNG      *pVideoLogPrng;
#endif

    /*!
     * Variable holding the regeky value
     */
    NvU32 eventTraceRegkeyData;

    /*!
     * Flag to allow engine to log events even if no client is bound
     */
    NvBool bAlwaysLogging;
} VIDEO_TRACE_INFO;

// Basic implementation of KernelVideoEngine that can be instantiated.

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_VIDEO_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelVideoEngine {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct KernelVideoEngine *__nvoc_pbase_KernelVideoEngine;    // kvideng

    // Data members
    ENGDESCRIPTOR physEngDesc;
    VIDEO_TRACE_INFO videoTraceInfo;
    NvBool bVideoTraceEnabled;
};

#ifndef __NVOC_CLASS_KernelVideoEngine_TYPEDEF__
#define __NVOC_CLASS_KernelVideoEngine_TYPEDEF__
typedef struct KernelVideoEngine KernelVideoEngine;
#endif /* __NVOC_CLASS_KernelVideoEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelVideoEngine
#define __nvoc_class_id_KernelVideoEngine 0x9e2f3e
#endif /* __nvoc_class_id_KernelVideoEngine */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVideoEngine;

#define __staticCast_KernelVideoEngine(pThis) \
    ((pThis)->__nvoc_pbase_KernelVideoEngine)

#ifdef __nvoc_kernel_video_engine_h_disabled
#define __dynamicCast_KernelVideoEngine(pThis) ((KernelVideoEngine*)NULL)
#else //__nvoc_kernel_video_engine_h_disabled
#define __dynamicCast_KernelVideoEngine(pThis) \
    ((KernelVideoEngine*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelVideoEngine)))
#endif //__nvoc_kernel_video_engine_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelVideoEngine(KernelVideoEngine**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelVideoEngine(KernelVideoEngine**, Dynamic*, NvU32, struct OBJGPU * arg_pGpu, ENGDESCRIPTOR arg_physEngDesc);
#define __objCreate_KernelVideoEngine(ppNewObj, pParent, createFlags, arg_pGpu, arg_physEngDesc) \
    __nvoc_objCreate_KernelVideoEngine((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_physEngDesc)


// Wrapper macros

// Dispatch functions
NvBool kvidengIsVideoTraceLogSupported_IMPL(struct OBJGPU *pGpu);


#define kvidengIsVideoTraceLogSupported(pGpu) kvidengIsVideoTraceLogSupported_IMPL(pGpu)
#define kvidengIsVideoTraceLogSupported_HAL(pGpu) kvidengIsVideoTraceLogSupported(pGpu)

NV_STATUS kvidengInitLogging_KERNEL(struct OBJGPU *pGpu, struct KernelVideoEngine *pKernelVideoEngine);


#ifdef __nvoc_kernel_video_engine_h_disabled
static inline NV_STATUS kvidengInitLogging(struct OBJGPU *pGpu, struct KernelVideoEngine *pKernelVideoEngine) {
    NV_ASSERT_FAILED_PRECOMP("KernelVideoEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_video_engine_h_disabled
#define kvidengInitLogging(pGpu, pKernelVideoEngine) kvidengInitLogging_KERNEL(pGpu, pKernelVideoEngine)
#endif //__nvoc_kernel_video_engine_h_disabled

#define kvidengInitLogging_HAL(pGpu, pKernelVideoEngine) kvidengInitLogging(pGpu, pKernelVideoEngine)

void kvidengFreeLogging_KERNEL(struct OBJGPU *pGpu, struct KernelVideoEngine *pKernelVideoEngine);


#ifdef __nvoc_kernel_video_engine_h_disabled
static inline void kvidengFreeLogging(struct OBJGPU *pGpu, struct KernelVideoEngine *pKernelVideoEngine) {
    NV_ASSERT_FAILED_PRECOMP("KernelVideoEngine was disabled!");
}
#else //__nvoc_kernel_video_engine_h_disabled
#define kvidengFreeLogging(pGpu, pKernelVideoEngine) kvidengFreeLogging_KERNEL(pGpu, pKernelVideoEngine)
#endif //__nvoc_kernel_video_engine_h_disabled

#define kvidengFreeLogging_HAL(pGpu, pKernelVideoEngine) kvidengFreeLogging(pGpu, pKernelVideoEngine)

NV_STATUS kvidengRingbufferMakeSpace_IMPL(struct OBJGPU *pGpu, NvU32 oldReadPtr, NvU32 size, VIDEO_TRACE_RING_BUFFER *pTraceBuffer);

#define kvidengRingbufferMakeSpace(pGpu, oldReadPtr, size, pTraceBuffer) kvidengRingbufferMakeSpace_IMPL(pGpu, oldReadPtr, size, pTraceBuffer)
NvU32 kvidengRingbufferGetDataSize_IMPL(struct OBJGPU *pGpu, VIDEO_TRACE_RING_BUFFER *arg2);

#define kvidengRingbufferGetDataSize(pGpu, arg2) kvidengRingbufferGetDataSize_IMPL(pGpu, arg2)
struct KernelVideoEngine *kvidengFromEngDesc_IMPL(struct OBJGPU *pGpu, NvU32 engDesc);

#define kvidengFromEngDesc(pGpu, engDesc) kvidengFromEngDesc_IMPL(pGpu, engDesc)
NV_STATUS kvidengConstruct_IMPL(struct KernelVideoEngine *arg_pKernelVideoEngine, struct OBJGPU *arg_pGpu, ENGDESCRIPTOR arg_physEngDesc);

#define __nvoc_kvidengConstruct(arg_pKernelVideoEngine, arg_pGpu, arg_physEngDesc) kvidengConstruct_IMPL(arg_pKernelVideoEngine, arg_pGpu, arg_physEngDesc)
NvU32 kvidengRingbufferGet_IMPL(struct OBJGPU *arg1, struct KernelVideoEngine *arg2, NvU8 *pDataOut, NvU32 sizeOut, VIDEO_TRACE_RING_BUFFER *arg5);

#ifdef __nvoc_kernel_video_engine_h_disabled
static inline NvU32 kvidengRingbufferGet(struct OBJGPU *arg1, struct KernelVideoEngine *arg2, NvU8 *pDataOut, NvU32 sizeOut, VIDEO_TRACE_RING_BUFFER *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelVideoEngine was disabled!");
    return 0;
}
#else //__nvoc_kernel_video_engine_h_disabled
#define kvidengRingbufferGet(arg1, arg2, pDataOut, sizeOut, arg5) kvidengRingbufferGet_IMPL(arg1, arg2, pDataOut, sizeOut, arg5)
#endif //__nvoc_kernel_video_engine_h_disabled

NvU32 kvidengEventbufferGetRecord_IMPL(struct OBJGPU *arg1, struct KernelVideoEngine *arg2, VIDEO_TRACE_RING_BUFFER *pTraceBuffer, VIDEO_ENGINE_EVENT__RECORD *pRecord, NvU32 magic_hi, NvU32 magic_lo);

#ifdef __nvoc_kernel_video_engine_h_disabled
static inline NvU32 kvidengEventbufferGetRecord(struct OBJGPU *arg1, struct KernelVideoEngine *arg2, VIDEO_TRACE_RING_BUFFER *pTraceBuffer, VIDEO_ENGINE_EVENT__RECORD *pRecord, NvU32 magic_hi, NvU32 magic_lo) {
    NV_ASSERT_FAILED_PRECOMP("KernelVideoEngine was disabled!");
    return 0;
}
#else //__nvoc_kernel_video_engine_h_disabled
#define kvidengEventbufferGetRecord(arg1, arg2, pTraceBuffer, pRecord, magic_hi, magic_lo) kvidengEventbufferGetRecord_IMPL(arg1, arg2, pTraceBuffer, pRecord, magic_hi, magic_lo)
#endif //__nvoc_kernel_video_engine_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_VIDEO_ENGINE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_VIDEO_ENGINE_NVOC_H_
