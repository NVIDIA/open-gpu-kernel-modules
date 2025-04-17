
#ifndef _G_CRASHCAT_ENGINE_NVOC_H_
#define _G_CRASHCAT_ENGINE_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

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
#include "g_crashcat_engine_nvoc.h"

#ifndef CRASHCAT_ENGINE_H
#define CRASHCAT_ENGINE_H

#include "containers/map.h"
#include "nvoc/object.h"
#include "nvport/inline/util_valist.h"
#include "nv-crashcat.h"


struct CrashCatReport;

#ifndef __NVOC_CLASS_CrashCatReport_TYPEDEF__
#define __NVOC_CLASS_CrashCatReport_TYPEDEF__
typedef struct CrashCatReport CrashCatReport;
#endif /* __NVOC_CLASS_CrashCatReport_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatReport
#define __nvoc_class_id_CrashCatReport 0xde4777
#endif /* __nvoc_class_id_CrashCatReport */



struct CrashCatWayfinder;

#ifndef __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__
#define __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__
typedef struct CrashCatWayfinder CrashCatWayfinder;
#endif /* __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatWayfinder
#define __nvoc_class_id_CrashCatWayfinder 0x085e32
#endif /* __nvoc_class_id_CrashCatWayfinder */



typedef struct {
    void *pEngPriv;
    void *pMapping;
    NvBool bRegistered;
    NV_CRASHCAT_MEM_APERTURE aperture;
    NvU64 physOffset;
    NvLength size;
    MapNode registeredBufferMapNode;
    MapNode mappedBufferMapNode;
} CrashCatBufferDescriptor;

MAKE_INTRUSIVE_MAP(CrashCatRegisteredBufferMap, CrashCatBufferDescriptor, registeredBufferMapNode);
MAKE_INTRUSIVE_MAP(CrashCatMappedBufferMap, CrashCatBufferDescriptor, mappedBufferMapNode);

// Base class for engine-specific accessors - must be implemented by the host codebase.

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__CrashCatEngine;
struct NVOC_VTABLE__CrashCatEngine;


struct CrashCatEngine {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CrashCatEngine *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine

    // Data members
    NvBool PRIVATE_FIELD(bEnabled);
    struct CrashCatWayfinder *PRIVATE_FIELD(pWayfinder);
    CrashCatRegisteredBufferMap PRIVATE_FIELD(registeredCrashBuffers);
    CrashCatMappedBufferMap PRIVATE_FIELD(mappedCrashBuffers);
};


// Vtable with 10 per-class function pointers
struct NVOC_VTABLE__CrashCatEngine {
    void (*__crashcatEngineUnload__)(struct CrashCatEngine * /*this*/);  // virtual
    NvBool (*__crashcatEngineConfigured__)(struct CrashCatEngine * /*this*/);  // pure virtual
    void (*__crashcatEngineVprintf__)(struct CrashCatEngine * /*this*/, NvBool, const char *, va_list);  // pure virtual
    NvU32 (*__crashcatEnginePriRead__)(struct CrashCatEngine * /*this*/, NvU32);  // pure virtual
    void (*__crashcatEnginePriWrite__)(struct CrashCatEngine * /*this*/, NvU32, NvU32);  // pure virtual
    void * (*__crashcatEngineMapBufferDescriptor__)(struct CrashCatEngine * /*this*/, CrashCatBufferDescriptor *);  // pure virtual
    void (*__crashcatEngineUnmapBufferDescriptor__)(struct CrashCatEngine * /*this*/, CrashCatBufferDescriptor *);  // pure virtual
    void (*__crashcatEngineSyncBufferDescriptor__)(struct CrashCatEngine * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // pure virtual
    const NvU32 * (*__crashcatEngineGetScratchOffsets__)(struct CrashCatEngine * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // pure virtual
    NvU32 (*__crashcatEngineGetWFL0Offset__)(struct CrashCatEngine * /*this*/);  // pure virtual
};

// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__CrashCatEngine {
    const struct NVOC_RTTI rtti;
    const struct NVOC_VTABLE__CrashCatEngine vtable;
};

#ifndef __NVOC_CLASS_CrashCatEngine_TYPEDEF__
#define __NVOC_CLASS_CrashCatEngine_TYPEDEF__
typedef struct CrashCatEngine CrashCatEngine;
#endif /* __NVOC_CLASS_CrashCatEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatEngine
#define __nvoc_class_id_CrashCatEngine 0x654166
#endif /* __nvoc_class_id_CrashCatEngine */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

#define __staticCast_CrashCatEngine(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatEngine)

#ifdef __nvoc_crashcat_engine_h_disabled
#define __dynamicCast_CrashCatEngine(pThis) ((CrashCatEngine*) NULL)
#else //__nvoc_crashcat_engine_h_disabled
#define __dynamicCast_CrashCatEngine(pThis) \
    ((CrashCatEngine*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatEngine)))
#endif //__nvoc_crashcat_engine_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CrashCatEngine(CrashCatEngine**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatEngine(CrashCatEngine**, Dynamic*, NvU32);
#define __objCreate_CrashCatEngine(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_CrashCatEngine((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define crashcatEngineUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineUnload__
#define crashcatEngineUnload(arg_this) crashcatEngineUnload_DISPATCH(arg_this)
#define crashcatEngineConfigured_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineConfigured__
#define crashcatEngineConfigured(arg_this) crashcatEngineConfigured_DISPATCH(arg_this)
#define crashcatEngineVprintf_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineVprintf__
#define crashcatEngineVprintf(arg_this, bReportStart, fmt, args) crashcatEngineVprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define crashcatEnginePriRead_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEnginePriRead__
#define crashcatEnginePriRead(arg_this, offset) crashcatEnginePriRead_DISPATCH(arg_this, offset)
#define crashcatEnginePriWrite_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEnginePriWrite__
#define crashcatEnginePriWrite(arg_this, offset, data) crashcatEnginePriWrite_DISPATCH(arg_this, offset, data)
#define crashcatEngineMapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineMapBufferDescriptor__
#define crashcatEngineMapBufferDescriptor(arg_this, pBufDesc) crashcatEngineMapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define crashcatEngineUnmapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineUnmapBufferDescriptor__
#define crashcatEngineUnmapBufferDescriptor(arg_this, pBufDesc) crashcatEngineUnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define crashcatEngineSyncBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineSyncBufferDescriptor__
#define crashcatEngineSyncBufferDescriptor(arg_this, pBufDesc, offset, size) crashcatEngineSyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define crashcatEngineGetScratchOffsets_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineGetScratchOffsets__
#define crashcatEngineGetScratchOffsets(arg_this, scratchId) crashcatEngineGetScratchOffsets_DISPATCH(arg_this, scratchId)
#define crashcatEngineGetWFL0Offset_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineGetWFL0Offset__
#define crashcatEngineGetWFL0Offset(arg_this) crashcatEngineGetWFL0Offset_DISPATCH(arg_this)

// Dispatch functions
static inline void crashcatEngineUnload_DISPATCH(struct CrashCatEngine *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineUnload__(arg_this);
}

static inline NvBool crashcatEngineConfigured_DISPATCH(struct CrashCatEngine *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineConfigured__(arg_this);
}

static inline void crashcatEngineVprintf_DISPATCH(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineVprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 crashcatEnginePriRead_DISPATCH(struct CrashCatEngine *arg_this, NvU32 offset) {
    return arg_this->__nvoc_metadata_ptr->vtable.__crashcatEnginePriRead__(arg_this, offset);
}

static inline void crashcatEnginePriWrite_DISPATCH(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__nvoc_metadata_ptr->vtable.__crashcatEnginePriWrite__(arg_this, offset, data);
}

static inline void * crashcatEngineMapBufferDescriptor_DISPATCH(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineMapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void crashcatEngineUnmapBufferDescriptor_DISPATCH(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineUnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void crashcatEngineSyncBufferDescriptor_DISPATCH(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineSyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline const NvU32 * crashcatEngineGetScratchOffsets_DISPATCH(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchId) {
    return arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineGetScratchOffsets__(arg_this, scratchId);
}

static inline NvU32 crashcatEngineGetWFL0Offset_DISPATCH(struct CrashCatEngine *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__crashcatEngineGetWFL0Offset__(arg_this);
}

void crashcatEngineUnload_IMPL(struct CrashCatEngine *arg1);

NV_STATUS crashcatEngineConstruct_IMPL(struct CrashCatEngine *arg_);

#define __nvoc_crashcatEngineConstruct(arg_) crashcatEngineConstruct_IMPL(arg_)
void crashcatEngineDestruct_IMPL(struct CrashCatEngine *arg1);

#define __nvoc_crashcatEngineDestruct(arg1) crashcatEngineDestruct_IMPL(arg1)
struct CrashCatReport *crashcatEngineGetNextCrashReport_IMPL(struct CrashCatEngine *arg1);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline struct CrashCatReport *crashcatEngineGetNextCrashReport(struct CrashCatEngine *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineGetNextCrashReport(arg1) crashcatEngineGetNextCrashReport_IMPL(arg1)
#endif //__nvoc_crashcat_engine_h_disabled

NV_STATUS crashcatEngineRegisterCrashBuffer_IMPL(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size, void *pEngPriv);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline NV_STATUS crashcatEngineRegisterCrashBuffer(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size, void *pEngPriv) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineRegisterCrashBuffer(arg1, aperture, offset, size, pEngPriv) crashcatEngineRegisterCrashBuffer_IMPL(arg1, aperture, offset, size, pEngPriv)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineUnregisterCrashBuffer_IMPL(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineUnregisterCrashBuffer(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineUnregisterCrashBuffer(arg1, aperture, offset, size) crashcatEngineUnregisterCrashBuffer_IMPL(arg1, aperture, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

void *crashcatEngineMapCrashBuffer_IMPL(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void *crashcatEngineMapCrashBuffer(struct CrashCatEngine *arg1, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineMapCrashBuffer(arg1, aperture, offset, size) crashcatEngineMapCrashBuffer_IMPL(arg1, aperture, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineUnmapCrashBuffer_IMPL(struct CrashCatEngine *arg1, void *ptr);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineUnmapCrashBuffer(struct CrashCatEngine *arg1, void *ptr) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineUnmapCrashBuffer(arg1, ptr) crashcatEngineUnmapCrashBuffer_IMPL(arg1, ptr)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineSyncCrashBuffer_IMPL(struct CrashCatEngine *arg1, void *ptr, NvU32 offset, NvU32 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineSyncCrashBuffer(struct CrashCatEngine *arg1, void *ptr, NvU32 offset, NvU32 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineSyncCrashBuffer(arg1, ptr, offset, size) crashcatEngineSyncCrashBuffer_IMPL(arg1, ptr, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

NV_STATUS crashcatEngineLoadWayfinder_IMPL(struct CrashCatEngine *arg1);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline NV_STATUS crashcatEngineLoadWayfinder(struct CrashCatEngine *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineLoadWayfinder(arg1) crashcatEngineLoadWayfinder_IMPL(arg1)
#endif //__nvoc_crashcat_engine_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#ifndef __nvoc_crashcat_engine_h_disabled
#undef crashcatEngineLoadWayfinder
NV_STATUS NVOC_PRIVATE_FUNCTION(crashcatEngineLoadWayfinder)(struct CrashCatEngine *arg1);
#endif //__nvoc_crashcat_engine_h_disabled

#endif // NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED


#define CRASHCAT_GET_ENGINE(pCrashCatObj) objFindAncestorOfType(CrashCatEngine, pCrashCatObj)

// Non-NVOC wrapper for handling variadic arguments
void crashcatEnginePrintf(struct CrashCatEngine *, NvBool, const char *, ...);

#endif // CRASHCAT_ENGINE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CRASHCAT_ENGINE_NVOC_H_
