#ifndef _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
#define _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_kernel_crashcat_engine_nvoc.h"

#ifndef KERNEL_CRASHCAT_ENGINE_H
#define KERNEL_CRASHCAT_ENGINE_H

#include "containers/map.h"
#include "core/core.h"
#include "crashcat/crashcat_engine.h"
#include "gpu/gpu_halspec.h"
#include "gpu/mem_mgr/mem_desc.h"

typedef struct KernelCrashCatEngineConfig
{
    const char *pName;      // Engine name passed to nvErrorLog_va() for crash reports
    NvU32 allocQueueSize;   // Size of the system memory buffer to allocate for the CrashCat queue
    NvU32 errorId;          // Error ID passed to nvErrorLog_va() for crash reports
    NvU8 dmemPort;          // DMEM port allocated for CrashCat usage
    NvBool bEnable;         // Enable CrashCat monitoring for the engine
} KernelCrashCatEngineConfig;

/*!
 * Base implementation of CrashCatEngine in RM.
 */
#ifdef NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelCrashCatEngine {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct CrashCatEngine __nvoc_base_CrashCatEngine;
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;
    NvBool (*__kcrashcatEngineConfigured__)(struct KernelCrashCatEngine *);
    void (*__kcrashcatEngineUnload__)(struct KernelCrashCatEngine *);
    void (*__kcrashcatEngineVprintf__)(struct KernelCrashCatEngine *, NvBool, const char *, va_list);
    NvU32 (*__kcrashcatEngineRegRead__)(struct OBJGPU *, struct KernelCrashCatEngine *, NvU32);
    void (*__kcrashcatEngineRegWrite__)(struct OBJGPU *, struct KernelCrashCatEngine *, NvU32, NvU32);
    NvU32 (*__kcrashcatEngineMaskDmemAddr__)(struct OBJGPU *, struct KernelCrashCatEngine *, NvU32);
    NvU32 (*__kcrashcatEnginePriRead__)(struct KernelCrashCatEngine *, NvU32);
    void (*__kcrashcatEnginePriWrite__)(struct KernelCrashCatEngine *, NvU32, NvU32);
    void *(*__kcrashcatEngineMapBufferDescriptor__)(struct KernelCrashCatEngine *, CrashCatBufferDescriptor *);
    void (*__kcrashcatEngineUnmapBufferDescriptor__)(struct KernelCrashCatEngine *, CrashCatBufferDescriptor *);
    void (*__kcrashcatEngineSyncBufferDescriptor__)(struct KernelCrashCatEngine *, CrashCatBufferDescriptor *, NvU32, NvU32);
    void (*__kcrashcatEngineReadDmem__)(struct KernelCrashCatEngine *, NvU32, NvU32, void *);
    void (*__kcrashcatEngineReadEmem__)(struct KernelCrashCatEngine *, NvU64, NvU64, void *);
    const NvU32 *(*__kcrashcatEngineGetScratchOffsets__)(struct KernelCrashCatEngine *, NV_CRASHCAT_SCRATCH_GROUP_ID);
    NvU32 (*__kcrashcatEngineGetWFL0Offset__)(struct KernelCrashCatEngine *);
    NvBool PRIVATE_FIELD(bConfigured);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pQueueMemDesc);
    const char *PRIVATE_FIELD(pName);
    NvU32 PRIVATE_FIELD(errorId);
    struct OBJGPU *PRIVATE_FIELD(pGpu);
    NvU32 PRIVATE_FIELD(dmemPort);
    char PRIVATE_FIELD(printBuffer)[512];
    char PRIVATE_FIELD(fmtBuffer)[512];
};

#ifndef __NVOC_CLASS_KernelCrashCatEngine_TYPEDEF__
#define __NVOC_CLASS_KernelCrashCatEngine_TYPEDEF__
typedef struct KernelCrashCatEngine KernelCrashCatEngine;
#endif /* __NVOC_CLASS_KernelCrashCatEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCrashCatEngine
#define __nvoc_class_id_KernelCrashCatEngine 0xc37aef
#endif /* __nvoc_class_id_KernelCrashCatEngine */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

#define __staticCast_KernelCrashCatEngine(pThis) \
    ((pThis)->__nvoc_pbase_KernelCrashCatEngine)

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
#define __dynamicCast_KernelCrashCatEngine(pThis) ((KernelCrashCatEngine*)NULL)
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define __dynamicCast_KernelCrashCatEngine(pThis) \
    ((KernelCrashCatEngine*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCrashCatEngine)))
#endif //__nvoc_kernel_crashcat_engine_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelCrashCatEngine(KernelCrashCatEngine**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCrashCatEngine(KernelCrashCatEngine**, Dynamic*, NvU32);
#define __objCreate_KernelCrashCatEngine(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCrashCatEngine((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kcrashcatEngineConfigured(arg0) kcrashcatEngineConfigured_DISPATCH(arg0)
#define kcrashcatEngineUnload(arg0) kcrashcatEngineUnload_DISPATCH(arg0)
#define kcrashcatEngineVprintf(arg0, bReportStart, fmt, args) kcrashcatEngineVprintf_DISPATCH(arg0, bReportStart, fmt, args)
#define kcrashcatEngineRegRead(pGpu, arg0, offset) kcrashcatEngineRegRead_DISPATCH(pGpu, arg0, offset)
#define kcrashcatEngineRegWrite(pGpu, arg0, offset, data) kcrashcatEngineRegWrite_DISPATCH(pGpu, arg0, offset, data)
#define kcrashcatEngineMaskDmemAddr(pGpu, arg0, addr) kcrashcatEngineMaskDmemAddr_DISPATCH(pGpu, arg0, addr)
#define kcrashcatEnginePriRead(arg0, offset) kcrashcatEnginePriRead_DISPATCH(arg0, offset)
#define kcrashcatEnginePriWrite(arg0, offset, data) kcrashcatEnginePriWrite_DISPATCH(arg0, offset, data)
#define kcrashcatEngineMapBufferDescriptor(arg0, pBufDesc) kcrashcatEngineMapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define kcrashcatEngineUnmapBufferDescriptor(arg0, pBufDesc) kcrashcatEngineUnmapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define kcrashcatEngineSyncBufferDescriptor(arg0, pBufDesc, offset, size) kcrashcatEngineSyncBufferDescriptor_DISPATCH(arg0, pBufDesc, offset, size)
#define kcrashcatEngineReadDmem(arg0, offset, size, pBuf) kcrashcatEngineReadDmem_DISPATCH(arg0, offset, size, pBuf)
#define kcrashcatEngineReadDmem_HAL(arg0, offset, size, pBuf) kcrashcatEngineReadDmem_DISPATCH(arg0, offset, size, pBuf)
#define kcrashcatEngineReadEmem(arg0, offset, size, pBuf) kcrashcatEngineReadEmem_DISPATCH(arg0, offset, size, pBuf)
#define kcrashcatEngineReadEmem_HAL(arg0, offset, size, pBuf) kcrashcatEngineReadEmem_DISPATCH(arg0, offset, size, pBuf)
#define kcrashcatEngineGetScratchOffsets(arg0, scratchGroupId) kcrashcatEngineGetScratchOffsets_DISPATCH(arg0, scratchGroupId)
#define kcrashcatEngineGetScratchOffsets_HAL(arg0, scratchGroupId) kcrashcatEngineGetScratchOffsets_DISPATCH(arg0, scratchGroupId)
#define kcrashcatEngineGetWFL0Offset(arg0) kcrashcatEngineGetWFL0Offset_DISPATCH(arg0)
#define kcrashcatEngineGetWFL0Offset_HAL(arg0) kcrashcatEngineGetWFL0Offset_DISPATCH(arg0)
NvBool kcrashcatEngineConfigured_IMPL(struct KernelCrashCatEngine *arg0);

static inline NvBool kcrashcatEngineConfigured_DISPATCH(struct KernelCrashCatEngine *arg0) {
    return arg0->__kcrashcatEngineConfigured__(arg0);
}

void kcrashcatEngineUnload_IMPL(struct KernelCrashCatEngine *arg0);

static inline void kcrashcatEngineUnload_DISPATCH(struct KernelCrashCatEngine *arg0) {
    arg0->__kcrashcatEngineUnload__(arg0);
}

void kcrashcatEngineVprintf_IMPL(struct KernelCrashCatEngine *arg0, NvBool bReportStart, const char *fmt, va_list args);

static inline void kcrashcatEngineVprintf_DISPATCH(struct KernelCrashCatEngine *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    arg0->__kcrashcatEngineVprintf__(arg0, bReportStart, fmt, args);
}

static inline NvU32 kcrashcatEngineRegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg0, NvU32 offset) {
    return arg0->__kcrashcatEngineRegRead__(pGpu, arg0, offset);
}

static inline void kcrashcatEngineRegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg0, NvU32 offset, NvU32 data) {
    arg0->__kcrashcatEngineRegWrite__(pGpu, arg0, offset, data);
}

static inline NvU32 kcrashcatEngineMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg0, NvU32 addr) {
    return arg0->__kcrashcatEngineMaskDmemAddr__(pGpu, arg0, addr);
}

NvU32 kcrashcatEnginePriRead_IMPL(struct KernelCrashCatEngine *arg0, NvU32 offset);

static inline NvU32 kcrashcatEnginePriRead_DISPATCH(struct KernelCrashCatEngine *arg0, NvU32 offset) {
    return arg0->__kcrashcatEnginePriRead__(arg0, offset);
}

void kcrashcatEnginePriWrite_IMPL(struct KernelCrashCatEngine *arg0, NvU32 offset, NvU32 data);

static inline void kcrashcatEnginePriWrite_DISPATCH(struct KernelCrashCatEngine *arg0, NvU32 offset, NvU32 data) {
    arg0->__kcrashcatEnginePriWrite__(arg0, offset, data);
}

void *kcrashcatEngineMapBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc);

static inline void *kcrashcatEngineMapBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return arg0->__kcrashcatEngineMapBufferDescriptor__(arg0, pBufDesc);
}

void kcrashcatEngineUnmapBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc);

static inline void kcrashcatEngineUnmapBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc) {
    arg0->__kcrashcatEngineUnmapBufferDescriptor__(arg0, pBufDesc);
}

void kcrashcatEngineSyncBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);

static inline void kcrashcatEngineSyncBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg0->__kcrashcatEngineSyncBufferDescriptor__(arg0, pBufDesc, offset, size);
}

void kcrashcatEngineReadDmem_TU102(struct KernelCrashCatEngine *arg0, NvU32 offset, NvU32 size, void *pBuf);

static inline void kcrashcatEngineReadDmem_DISPATCH(struct KernelCrashCatEngine *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    arg0->__kcrashcatEngineReadDmem__(arg0, offset, size, pBuf);
}

static inline void kcrashcatEngineReadEmem_2fced3(struct KernelCrashCatEngine *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kcrashcatEngineReadEmem_DISPATCH(struct KernelCrashCatEngine *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    arg0->__kcrashcatEngineReadEmem__(arg0, offset, size, pBuf);
}

const NvU32 *kcrashcatEngineGetScratchOffsets_TU102(struct KernelCrashCatEngine *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);

static inline const NvU32 *kcrashcatEngineGetScratchOffsets_DISPATCH(struct KernelCrashCatEngine *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg0->__kcrashcatEngineGetScratchOffsets__(arg0, scratchGroupId);
}

NvU32 kcrashcatEngineGetWFL0Offset_TU102(struct KernelCrashCatEngine *arg0);

static inline NvU32 kcrashcatEngineGetWFL0Offset_DISPATCH(struct KernelCrashCatEngine *arg0) {
    return arg0->__kcrashcatEngineGetWFL0Offset__(arg0);
}

NV_STATUS kcrashcatEngineConfigure_IMPL(struct KernelCrashCatEngine *arg0, KernelCrashCatEngineConfig *pEngConfig);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline NV_STATUS kcrashcatEngineConfigure(struct KernelCrashCatEngine *arg0, KernelCrashCatEngineConfig *pEngConfig) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineConfigure(arg0, pEngConfig) kcrashcatEngineConfigure_IMPL(arg0, pEngConfig)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

MEMORY_DESCRIPTOR *kcrashcatEngineGetQueueMemDesc_IMPL(struct KernelCrashCatEngine *arg0);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline MEMORY_DESCRIPTOR *kcrashcatEngineGetQueueMemDesc(struct KernelCrashCatEngine *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineGetQueueMemDesc(arg0) kcrashcatEngineGetQueueMemDesc_IMPL(arg0)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

NV_STATUS kcrashcatEngineRegisterCrashBuffer_IMPL(struct KernelCrashCatEngine *arg0, MEMORY_DESCRIPTOR *arg1);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline NV_STATUS kcrashcatEngineRegisterCrashBuffer(struct KernelCrashCatEngine *arg0, MEMORY_DESCRIPTOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineRegisterCrashBuffer(arg0, arg1) kcrashcatEngineRegisterCrashBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

void kcrashcatEngineUnregisterCrashBuffer_IMPL(struct KernelCrashCatEngine *arg0, MEMORY_DESCRIPTOR *arg1);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline void kcrashcatEngineUnregisterCrashBuffer(struct KernelCrashCatEngine *arg0, MEMORY_DESCRIPTOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineUnregisterCrashBuffer(arg0, arg1) kcrashcatEngineUnregisterCrashBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_CRASHCAT_ENGINE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
