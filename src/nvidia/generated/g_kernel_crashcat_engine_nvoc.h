
#ifndef _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
#define _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelCrashCatEngine {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct CrashCatEngine __nvoc_base_CrashCatEngine;

    // Ancestor object pointers for `staticCast` feature
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine super
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;    // kcrashcatEngine

    // Vtable with 15 per-object function pointers
    NvBool (*__kcrashcatEngineConfigured__)(struct KernelCrashCatEngine * /*this*/);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEngineUnload__)(struct KernelCrashCatEngine * /*this*/);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEngineVprintf__)(struct KernelCrashCatEngine * /*this*/, NvBool, const char *, va_list);  // virtual override (crashcatEngine) base (crashcatEngine)
    NvU32 (*__kcrashcatEngineRegRead__)(struct OBJGPU *, struct KernelCrashCatEngine * /*this*/, NvU32);  // pure virtual
    void (*__kcrashcatEngineRegWrite__)(struct OBJGPU *, struct KernelCrashCatEngine * /*this*/, NvU32, NvU32);  // pure virtual
    NvU32 (*__kcrashcatEngineMaskDmemAddr__)(struct OBJGPU *, struct KernelCrashCatEngine * /*this*/, NvU32);  // pure virtual
    NvU32 (*__kcrashcatEnginePriRead__)(struct KernelCrashCatEngine * /*this*/, NvU32);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEnginePriWrite__)(struct KernelCrashCatEngine * /*this*/, NvU32, NvU32);  // virtual override (crashcatEngine) base (crashcatEngine)
    void * (*__kcrashcatEngineMapBufferDescriptor__)(struct KernelCrashCatEngine * /*this*/, CrashCatBufferDescriptor *);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEngineUnmapBufferDescriptor__)(struct KernelCrashCatEngine * /*this*/, CrashCatBufferDescriptor *);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEngineSyncBufferDescriptor__)(struct KernelCrashCatEngine * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // virtual override (crashcatEngine) base (crashcatEngine)
    void (*__kcrashcatEngineReadDmem__)(struct KernelCrashCatEngine * /*this*/, NvU32, NvU32, void *);  // virtual halified (singleton optimized)
    void (*__kcrashcatEngineReadEmem__)(struct KernelCrashCatEngine * /*this*/, NvU64, NvU64, void *);  // virtual halified (singleton optimized)
    const NvU32 * (*__kcrashcatEngineGetScratchOffsets__)(struct KernelCrashCatEngine * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    NvU32 (*__kcrashcatEngineGetWFL0Offset__)(struct KernelCrashCatEngine * /*this*/);  // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)

    // Data members
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

// Casting support
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


// Wrapper macros
#define kcrashcatEngineConfigured_FNPTR(arg_this) arg_this->__kcrashcatEngineConfigured__
#define kcrashcatEngineConfigured(arg_this) kcrashcatEngineConfigured_DISPATCH(arg_this)
#define kcrashcatEngineUnload_FNPTR(arg_this) arg_this->__kcrashcatEngineUnload__
#define kcrashcatEngineUnload(arg_this) kcrashcatEngineUnload_DISPATCH(arg_this)
#define kcrashcatEngineVprintf_FNPTR(arg_this) arg_this->__kcrashcatEngineVprintf__
#define kcrashcatEngineVprintf(arg_this, bReportStart, fmt, args) kcrashcatEngineVprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define kcrashcatEngineRegRead_FNPTR(arg_this) arg_this->__kcrashcatEngineRegRead__
#define kcrashcatEngineRegRead(pGpu, arg_this, offset) kcrashcatEngineRegRead_DISPATCH(pGpu, arg_this, offset)
#define kcrashcatEngineRegWrite_FNPTR(arg_this) arg_this->__kcrashcatEngineRegWrite__
#define kcrashcatEngineRegWrite(pGpu, arg_this, offset, data) kcrashcatEngineRegWrite_DISPATCH(pGpu, arg_this, offset, data)
#define kcrashcatEngineMaskDmemAddr_FNPTR(arg_this) arg_this->__kcrashcatEngineMaskDmemAddr__
#define kcrashcatEngineMaskDmemAddr(pGpu, arg_this, addr) kcrashcatEngineMaskDmemAddr_DISPATCH(pGpu, arg_this, addr)
#define kcrashcatEnginePriRead_FNPTR(arg_this) arg_this->__kcrashcatEnginePriRead__
#define kcrashcatEnginePriRead(arg_this, offset) kcrashcatEnginePriRead_DISPATCH(arg_this, offset)
#define kcrashcatEnginePriWrite_FNPTR(arg_this) arg_this->__kcrashcatEnginePriWrite__
#define kcrashcatEnginePriWrite(arg_this, offset, data) kcrashcatEnginePriWrite_DISPATCH(arg_this, offset, data)
#define kcrashcatEngineMapBufferDescriptor_FNPTR(arg_this) arg_this->__kcrashcatEngineMapBufferDescriptor__
#define kcrashcatEngineMapBufferDescriptor(arg_this, pBufDesc) kcrashcatEngineMapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kcrashcatEngineUnmapBufferDescriptor_FNPTR(arg_this) arg_this->__kcrashcatEngineUnmapBufferDescriptor__
#define kcrashcatEngineUnmapBufferDescriptor(arg_this, pBufDesc) kcrashcatEngineUnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kcrashcatEngineSyncBufferDescriptor_FNPTR(arg_this) arg_this->__kcrashcatEngineSyncBufferDescriptor__
#define kcrashcatEngineSyncBufferDescriptor(arg_this, pBufDesc, offset, size) kcrashcatEngineSyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define kcrashcatEngineReadDmem_FNPTR(arg_this) arg_this->__kcrashcatEngineReadDmem__
#define kcrashcatEngineReadDmem(arg_this, offset, size, pBuf) kcrashcatEngineReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kcrashcatEngineReadDmem_HAL(arg_this, offset, size, pBuf) kcrashcatEngineReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kcrashcatEngineReadEmem_FNPTR(arg_this) arg_this->__kcrashcatEngineReadEmem__
#define kcrashcatEngineReadEmem(arg_this, offset, size, pBuf) kcrashcatEngineReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define kcrashcatEngineReadEmem_HAL(arg_this, offset, size, pBuf) kcrashcatEngineReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define kcrashcatEngineGetScratchOffsets_FNPTR(arg_this) arg_this->__kcrashcatEngineGetScratchOffsets__
#define kcrashcatEngineGetScratchOffsets(arg_this, scratchGroupId) kcrashcatEngineGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kcrashcatEngineGetScratchOffsets_HAL(arg_this, scratchGroupId) kcrashcatEngineGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kcrashcatEngineGetWFL0Offset_FNPTR(arg_this) arg_this->__kcrashcatEngineGetWFL0Offset__
#define kcrashcatEngineGetWFL0Offset(arg_this) kcrashcatEngineGetWFL0Offset_DISPATCH(arg_this)
#define kcrashcatEngineGetWFL0Offset_HAL(arg_this) kcrashcatEngineGetWFL0Offset_DISPATCH(arg_this)

// Dispatch functions
static inline NvBool kcrashcatEngineConfigured_DISPATCH(struct KernelCrashCatEngine *arg_this) {
    return arg_this->__kcrashcatEngineConfigured__(arg_this);
}

static inline void kcrashcatEngineUnload_DISPATCH(struct KernelCrashCatEngine *arg_this) {
    arg_this->__kcrashcatEngineUnload__(arg_this);
}

static inline void kcrashcatEngineVprintf_DISPATCH(struct KernelCrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__kcrashcatEngineVprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 kcrashcatEngineRegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg_this, NvU32 offset) {
    return arg_this->__kcrashcatEngineRegRead__(pGpu, arg_this, offset);
}

static inline void kcrashcatEngineRegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__kcrashcatEngineRegWrite__(pGpu, arg_this, offset, data);
}

static inline NvU32 kcrashcatEngineMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelCrashCatEngine *arg_this, NvU32 addr) {
    return arg_this->__kcrashcatEngineMaskDmemAddr__(pGpu, arg_this, addr);
}

static inline NvU32 kcrashcatEnginePriRead_DISPATCH(struct KernelCrashCatEngine *arg_this, NvU32 offset) {
    return arg_this->__kcrashcatEnginePriRead__(arg_this, offset);
}

static inline void kcrashcatEnginePriWrite_DISPATCH(struct KernelCrashCatEngine *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__kcrashcatEnginePriWrite__(arg_this, offset, data);
}

static inline void * kcrashcatEngineMapBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__kcrashcatEngineMapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kcrashcatEngineUnmapBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__kcrashcatEngineUnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kcrashcatEngineSyncBufferDescriptor_DISPATCH(struct KernelCrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__kcrashcatEngineSyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline void kcrashcatEngineReadDmem_DISPATCH(struct KernelCrashCatEngine *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    arg_this->__kcrashcatEngineReadDmem__(arg_this, offset, size, pBuf);
}

static inline void kcrashcatEngineReadEmem_DISPATCH(struct KernelCrashCatEngine *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    arg_this->__kcrashcatEngineReadEmem__(arg_this, offset, size, pBuf);
}

static inline const NvU32 * kcrashcatEngineGetScratchOffsets_DISPATCH(struct KernelCrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg_this->__kcrashcatEngineGetScratchOffsets__(arg_this, scratchGroupId);
}

static inline NvU32 kcrashcatEngineGetWFL0Offset_DISPATCH(struct KernelCrashCatEngine *arg_this) {
    return arg_this->__kcrashcatEngineGetWFL0Offset__(arg_this);
}

NvBool kcrashcatEngineConfigured_IMPL(struct KernelCrashCatEngine *arg1);

void kcrashcatEngineUnload_IMPL(struct KernelCrashCatEngine *arg1);

void kcrashcatEngineVprintf_IMPL(struct KernelCrashCatEngine *arg1, NvBool bReportStart, const char *fmt, va_list args);

NvU32 kcrashcatEnginePriRead_IMPL(struct KernelCrashCatEngine *arg1, NvU32 offset);

void kcrashcatEnginePriWrite_IMPL(struct KernelCrashCatEngine *arg1, NvU32 offset, NvU32 data);

void *kcrashcatEngineMapBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg1, CrashCatBufferDescriptor *pBufDesc);

void kcrashcatEngineUnmapBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg1, CrashCatBufferDescriptor *pBufDesc);

void kcrashcatEngineSyncBufferDescriptor_IMPL(struct KernelCrashCatEngine *arg1, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);

void kcrashcatEngineReadDmem_TU102(struct KernelCrashCatEngine *arg1, NvU32 offset, NvU32 size, void *pBuf);

static inline void kcrashcatEngineReadEmem_2fced3(struct KernelCrashCatEngine *arg1, NvU64 offset, NvU64 size, void *pBuf) {
    NV_ASSERT_PRECOMP(0);
}

const NvU32 *kcrashcatEngineGetScratchOffsets_TU102(struct KernelCrashCatEngine *arg1, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);

NvU32 kcrashcatEngineGetWFL0Offset_TU102(struct KernelCrashCatEngine *arg1);

NV_STATUS kcrashcatEngineConfigure_IMPL(struct KernelCrashCatEngine *arg1, KernelCrashCatEngineConfig *pEngConfig);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline NV_STATUS kcrashcatEngineConfigure(struct KernelCrashCatEngine *arg1, KernelCrashCatEngineConfig *pEngConfig) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineConfigure(arg1, pEngConfig) kcrashcatEngineConfigure_IMPL(arg1, pEngConfig)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

MEMORY_DESCRIPTOR *kcrashcatEngineGetQueueMemDesc_IMPL(struct KernelCrashCatEngine *arg1);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline MEMORY_DESCRIPTOR *kcrashcatEngineGetQueueMemDesc(struct KernelCrashCatEngine *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineGetQueueMemDesc(arg1) kcrashcatEngineGetQueueMemDesc_IMPL(arg1)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

NV_STATUS kcrashcatEngineRegisterCrashBuffer_IMPL(struct KernelCrashCatEngine *arg1, MEMORY_DESCRIPTOR *arg2);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline NV_STATUS kcrashcatEngineRegisterCrashBuffer(struct KernelCrashCatEngine *arg1, MEMORY_DESCRIPTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineRegisterCrashBuffer(arg1, arg2) kcrashcatEngineRegisterCrashBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

void kcrashcatEngineUnregisterCrashBuffer_IMPL(struct KernelCrashCatEngine *arg1, MEMORY_DESCRIPTOR *arg2);

#ifdef __nvoc_kernel_crashcat_engine_h_disabled
static inline void kcrashcatEngineUnregisterCrashBuffer(struct KernelCrashCatEngine *arg1, MEMORY_DESCRIPTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCrashCatEngine was disabled!");
}
#else //__nvoc_kernel_crashcat_engine_h_disabled
#define kcrashcatEngineUnregisterCrashBuffer(arg1, arg2) kcrashcatEngineUnregisterCrashBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_crashcat_engine_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_CRASHCAT_ENGINE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CRASHCAT_ENGINE_NVOC_H_
