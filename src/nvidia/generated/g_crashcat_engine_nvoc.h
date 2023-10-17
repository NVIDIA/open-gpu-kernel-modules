#ifndef _G_CRASHCAT_ENGINE_NVOC_H_
#define _G_CRASHCAT_ENGINE_NVOC_H_
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
#ifdef NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct CrashCatEngine {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;
    void (*__crashcatEngineUnload__)(struct CrashCatEngine *);
    NvBool (*__crashcatEngineConfigured__)(struct CrashCatEngine *);
    void (*__crashcatEngineVprintf__)(struct CrashCatEngine *, NvBool, const char *, va_list);
    NvU32 (*__crashcatEnginePriRead__)(struct CrashCatEngine *, NvU32);
    void (*__crashcatEnginePriWrite__)(struct CrashCatEngine *, NvU32, NvU32);
    void *(*__crashcatEngineMapBufferDescriptor__)(struct CrashCatEngine *, CrashCatBufferDescriptor *);
    void (*__crashcatEngineUnmapBufferDescriptor__)(struct CrashCatEngine *, CrashCatBufferDescriptor *);
    void (*__crashcatEngineSyncBufferDescriptor__)(struct CrashCatEngine *, CrashCatBufferDescriptor *, NvU32, NvU32);
    const NvU32 *(*__crashcatEngineGetScratchOffsets__)(struct CrashCatEngine *, NV_CRASHCAT_SCRATCH_GROUP_ID);
    NvU32 (*__crashcatEngineGetWFL0Offset__)(struct CrashCatEngine *);
    NvBool PRIVATE_FIELD(bEnabled);
    struct CrashCatWayfinder *PRIVATE_FIELD(pWayfinder);
    CrashCatRegisteredBufferMap PRIVATE_FIELD(registeredCrashBuffers);
    CrashCatMappedBufferMap PRIVATE_FIELD(mappedCrashBuffers);
};

#ifndef __NVOC_CLASS_CrashCatEngine_TYPEDEF__
#define __NVOC_CLASS_CrashCatEngine_TYPEDEF__
typedef struct CrashCatEngine CrashCatEngine;
#endif /* __NVOC_CLASS_CrashCatEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatEngine
#define __nvoc_class_id_CrashCatEngine 0x654166
#endif /* __nvoc_class_id_CrashCatEngine */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

#define __staticCast_CrashCatEngine(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatEngine)

#ifdef __nvoc_crashcat_engine_h_disabled
#define __dynamicCast_CrashCatEngine(pThis) ((CrashCatEngine*)NULL)
#else //__nvoc_crashcat_engine_h_disabled
#define __dynamicCast_CrashCatEngine(pThis) \
    ((CrashCatEngine*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatEngine)))
#endif //__nvoc_crashcat_engine_h_disabled


NV_STATUS __nvoc_objCreateDynamic_CrashCatEngine(CrashCatEngine**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatEngine(CrashCatEngine**, Dynamic*, NvU32);
#define __objCreate_CrashCatEngine(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_CrashCatEngine((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define crashcatEngineUnload(arg0) crashcatEngineUnload_DISPATCH(arg0)
#define crashcatEngineConfigured(arg0) crashcatEngineConfigured_DISPATCH(arg0)
#define crashcatEngineVprintf(arg0, bReportStart, fmt, args) crashcatEngineVprintf_DISPATCH(arg0, bReportStart, fmt, args)
#define crashcatEnginePriRead(arg0, offset) crashcatEnginePriRead_DISPATCH(arg0, offset)
#define crashcatEnginePriWrite(arg0, offset, data) crashcatEnginePriWrite_DISPATCH(arg0, offset, data)
#define crashcatEngineMapBufferDescriptor(arg0, pBufDesc) crashcatEngineMapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define crashcatEngineUnmapBufferDescriptor(arg0, pBufDesc) crashcatEngineUnmapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define crashcatEngineSyncBufferDescriptor(arg0, pBufDesc, offset, size) crashcatEngineSyncBufferDescriptor_DISPATCH(arg0, pBufDesc, offset, size)
#define crashcatEngineGetScratchOffsets(arg0, scratchId) crashcatEngineGetScratchOffsets_DISPATCH(arg0, scratchId)
#define crashcatEngineGetWFL0Offset(arg0) crashcatEngineGetWFL0Offset_DISPATCH(arg0)
void crashcatEngineUnload_IMPL(struct CrashCatEngine *arg0);

static inline void crashcatEngineUnload_DISPATCH(struct CrashCatEngine *arg0) {
    arg0->__crashcatEngineUnload__(arg0);
}

static inline NvBool crashcatEngineConfigured_DISPATCH(struct CrashCatEngine *arg0) {
    return arg0->__crashcatEngineConfigured__(arg0);
}

static inline void crashcatEngineVprintf_DISPATCH(struct CrashCatEngine *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    arg0->__crashcatEngineVprintf__(arg0, bReportStart, fmt, args);
}

static inline NvU32 crashcatEnginePriRead_DISPATCH(struct CrashCatEngine *arg0, NvU32 offset) {
    return arg0->__crashcatEnginePriRead__(arg0, offset);
}

static inline void crashcatEnginePriWrite_DISPATCH(struct CrashCatEngine *arg0, NvU32 offset, NvU32 data) {
    arg0->__crashcatEnginePriWrite__(arg0, offset, data);
}

static inline void *crashcatEngineMapBufferDescriptor_DISPATCH(struct CrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return arg0->__crashcatEngineMapBufferDescriptor__(arg0, pBufDesc);
}

static inline void crashcatEngineUnmapBufferDescriptor_DISPATCH(struct CrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc) {
    arg0->__crashcatEngineUnmapBufferDescriptor__(arg0, pBufDesc);
}

static inline void crashcatEngineSyncBufferDescriptor_DISPATCH(struct CrashCatEngine *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg0->__crashcatEngineSyncBufferDescriptor__(arg0, pBufDesc, offset, size);
}

static inline const NvU32 *crashcatEngineGetScratchOffsets_DISPATCH(struct CrashCatEngine *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchId) {
    return arg0->__crashcatEngineGetScratchOffsets__(arg0, scratchId);
}

static inline NvU32 crashcatEngineGetWFL0Offset_DISPATCH(struct CrashCatEngine *arg0) {
    return arg0->__crashcatEngineGetWFL0Offset__(arg0);
}

NV_STATUS crashcatEngineConstruct_IMPL(struct CrashCatEngine *arg_);

#define __nvoc_crashcatEngineConstruct(arg_) crashcatEngineConstruct_IMPL(arg_)
void crashcatEngineDestruct_IMPL(struct CrashCatEngine *arg0);

#define __nvoc_crashcatEngineDestruct(arg0) crashcatEngineDestruct_IMPL(arg0)
struct CrashCatReport *crashcatEngineGetNextCrashReport_IMPL(struct CrashCatEngine *arg0);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline struct CrashCatReport *crashcatEngineGetNextCrashReport(struct CrashCatEngine *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineGetNextCrashReport(arg0) crashcatEngineGetNextCrashReport_IMPL(arg0)
#endif //__nvoc_crashcat_engine_h_disabled

NV_STATUS crashcatEngineRegisterCrashBuffer_IMPL(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size, void *pEngPriv);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline NV_STATUS crashcatEngineRegisterCrashBuffer(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size, void *pEngPriv) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineRegisterCrashBuffer(arg0, aperture, offset, size, pEngPriv) crashcatEngineRegisterCrashBuffer_IMPL(arg0, aperture, offset, size, pEngPriv)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineUnregisterCrashBuffer_IMPL(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineUnregisterCrashBuffer(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineUnregisterCrashBuffer(arg0, aperture, offset, size) crashcatEngineUnregisterCrashBuffer_IMPL(arg0, aperture, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

void *crashcatEngineMapCrashBuffer_IMPL(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void *crashcatEngineMapCrashBuffer(struct CrashCatEngine *arg0, NV_CRASHCAT_MEM_APERTURE aperture, NvU64 offset, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineMapCrashBuffer(arg0, aperture, offset, size) crashcatEngineMapCrashBuffer_IMPL(arg0, aperture, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineUnmapCrashBuffer_IMPL(struct CrashCatEngine *arg0, void *ptr);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineUnmapCrashBuffer(struct CrashCatEngine *arg0, void *ptr) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineUnmapCrashBuffer(arg0, ptr) crashcatEngineUnmapCrashBuffer_IMPL(arg0, ptr)
#endif //__nvoc_crashcat_engine_h_disabled

void crashcatEngineSyncCrashBuffer_IMPL(struct CrashCatEngine *arg0, void *ptr, NvU32 offset, NvU32 size);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline void crashcatEngineSyncCrashBuffer(struct CrashCatEngine *arg0, void *ptr, NvU32 offset, NvU32 size) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineSyncCrashBuffer(arg0, ptr, offset, size) crashcatEngineSyncCrashBuffer_IMPL(arg0, ptr, offset, size)
#endif //__nvoc_crashcat_engine_h_disabled

NV_STATUS crashcatEngineLoadWayfinder_IMPL(struct CrashCatEngine *arg0);

#ifdef __nvoc_crashcat_engine_h_disabled
static inline NV_STATUS crashcatEngineLoadWayfinder(struct CrashCatEngine *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatEngine was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_crashcat_engine_h_disabled
#define crashcatEngineLoadWayfinder(arg0) crashcatEngineLoadWayfinder_IMPL(arg0)
#endif //__nvoc_crashcat_engine_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#ifndef __nvoc_crashcat_engine_h_disabled
#undef crashcatEngineLoadWayfinder
NV_STATUS NVOC_PRIVATE_FUNCTION(crashcatEngineLoadWayfinder)(struct CrashCatEngine *arg0);
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
