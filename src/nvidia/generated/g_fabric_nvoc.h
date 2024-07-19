
#ifndef _G_FABRIC_NVOC_H_
#define _G_FABRIC_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions managing the NVLink fabric
 *
 *****************************************************************************/

#pragma once
#include "g_fabric_nvoc.h"

#ifndef _FABRIC_H_
#define _FABRIC_H_

#include "core/core.h"
#include "core/system.h"
#include "containers/multimap.h"
#include "containers/map.h"
#include "class/cl000f.h"
#include "ctrl/ctrl000f.h"

#include "core/thread_state.h"
#include "rmapi/event.h"
#include "rmapi/resource.h"
#include "containers/list.h"
#include "nvCpuUuid.h"
#include "ctrl/ctrl00f1.h"



// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

typedef struct
{
    void *pData;
} FabricCacheMapEntry;

MAKE_MAP(FabricCacheMap, FabricCacheMapEntry);

typedef struct
{
    FabricCacheMap map;
} FabricCacheEntry;

MAKE_MULTIMAP(FabricCache, FabricCacheEntry);

#define NV_FABRIC_INVALID_NODE_ID ((NvU16) 0xFFFF)

MAKE_LIST(FabricEventListV2, NV00F1_CTRL_FABRIC_EVENT);

// Callbacks
void fabricWakeUpThreadCallback(void *pData);

#include "nvlink_inband_msg.h"

void fabricMulticastWaitOnTeamCleanupCallback(void *pCbData);

//
// The Fabric object is used to encapsulate the NVLink fabric
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct Fabric {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Fabric *__nvoc_pbase_Fabric;    // fabric

    // Data members
    PORT_RWLOCK *pFabricImportModuleLock;
    PORT_RWLOCK *pMulticastFabricModuleLock;
    NvU32 PRIVATE_FIELD(flags);
    FabricEventListV2 PRIVATE_FIELD(fabricEventListV2);
    PORT_RWLOCK *PRIVATE_FIELD(pListLock);
    FabricCache PRIVATE_FIELD(importCache);
    PORT_RWLOCK *PRIVATE_FIELD(pImportCacheLock);
    FabricCache PRIVATE_FIELD(unimportCache);
    PORT_RWLOCK *PRIVATE_FIELD(pUnimportCacheLock);
    NvP64 PRIVATE_FIELD(pOsImexEvent);
    NvU16 PRIVATE_FIELD(nodeId);
    volatile NvU64 PRIVATE_FIELD(eventId);
    NvBool PRIVATE_FIELD(bAllowFabricMemAlloc);
    FabricCache PRIVATE_FIELD(fabricMulticastCache);
    PORT_RWLOCK *PRIVATE_FIELD(pMulticastFabriCacheLock);
};

#ifndef __NVOC_CLASS_Fabric_TYPEDEF__
#define __NVOC_CLASS_Fabric_TYPEDEF__
typedef struct Fabric Fabric;
#endif /* __NVOC_CLASS_Fabric_TYPEDEF__ */

#ifndef __nvoc_class_id_Fabric
#define __nvoc_class_id_Fabric 0x0ac791
#endif /* __nvoc_class_id_Fabric */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Fabric;

#define __staticCast_Fabric(pThis) \
    ((pThis)->__nvoc_pbase_Fabric)

#ifdef __nvoc_fabric_h_disabled
#define __dynamicCast_Fabric(pThis) ((Fabric*)NULL)
#else //__nvoc_fabric_h_disabled
#define __dynamicCast_Fabric(pThis) \
    ((Fabric*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Fabric)))
#endif //__nvoc_fabric_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Fabric(Fabric**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Fabric(Fabric**, Dynamic*, NvU32);
#define __objCreate_Fabric(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Fabric((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS fabricConstruct_IMPL(struct Fabric *arg_pFabric);

#define __nvoc_fabricConstruct(arg_pFabric) fabricConstruct_IMPL(arg_pFabric)
void fabricDestruct_IMPL(struct Fabric *pFabric);

#define __nvoc_fabricDestruct(pFabric) fabricDestruct_IMPL(pFabric)
void fabricSetFmSessionFlags_IMPL(struct Fabric *pFabric, NvU32 flags);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricSetFmSessionFlags(struct Fabric *pFabric, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricSetFmSessionFlags(pFabric, flags) fabricSetFmSessionFlags_IMPL(pFabric, flags)
#endif //__nvoc_fabric_h_disabled

NvU32 fabricGetFmSessionFlags_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline NvU32 fabricGetFmSessionFlags(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return 0;
}
#else //__nvoc_fabric_h_disabled
#define fabricGetFmSessionFlags(pFabric) fabricGetFmSessionFlags_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NvBool fabricIsMemAllocDisabled_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline NvBool fabricIsMemAllocDisabled(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_FALSE;
}
#else //__nvoc_fabric_h_disabled
#define fabricIsMemAllocDisabled(pFabric) fabricIsMemAllocDisabled_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

void fabricDisableMemAlloc_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricDisableMemAlloc(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricDisableMemAlloc(pFabric) fabricDisableMemAlloc_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

void fabricEnableMemAlloc_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricEnableMemAlloc(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricEnableMemAlloc(pFabric) fabricEnableMemAlloc_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricSetImexEvent_IMPL(struct Fabric *pFabric, NvP64 pOsEvent);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricSetImexEvent(struct Fabric *pFabric, NvP64 pOsEvent) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricSetImexEvent(pFabric, pOsEvent) fabricSetImexEvent_IMPL(pFabric, pOsEvent)
#endif //__nvoc_fabric_h_disabled

NvP64 fabricGetImexEvent_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline NvP64 fabricGetImexEvent(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return 0;
}
#else //__nvoc_fabric_h_disabled
#define fabricGetImexEvent(pFabric) fabricGetImexEvent_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

void fabricSetNodeId_IMPL(struct Fabric *pFabric, NvU16 nodeId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricSetNodeId(struct Fabric *pFabric, NvU16 nodeId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricSetNodeId(pFabric, nodeId) fabricSetNodeId_IMPL(pFabric, nodeId)
#endif //__nvoc_fabric_h_disabled

NvU16 fabricGetNodeId_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline NvU16 fabricGetNodeId(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return 0;
}
#else //__nvoc_fabric_h_disabled
#define fabricGetNodeId(pFabric) fabricGetNodeId_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricPostEventsV2_IMPL(struct Fabric *pFabric, NV00F1_CTRL_FABRIC_EVENT *pEvents, NvU32 numEvents);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricPostEventsV2(struct Fabric *pFabric, NV00F1_CTRL_FABRIC_EVENT *pEvents, NvU32 numEvents) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricPostEventsV2(pFabric, pEvents, numEvents) fabricPostEventsV2_IMPL(pFabric, pEvents, numEvents)
#endif //__nvoc_fabric_h_disabled

NvBool fabricExtractEventsV2_IMPL(struct Fabric *pFabric, NV00F1_CTRL_FABRIC_EVENT *pEventArray, NvU32 *pNumEvents);

#ifdef __nvoc_fabric_h_disabled
static inline NvBool fabricExtractEventsV2(struct Fabric *pFabric, NV00F1_CTRL_FABRIC_EVENT *pEventArray, NvU32 *pNumEvents) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_FALSE;
}
#else //__nvoc_fabric_h_disabled
#define fabricExtractEventsV2(pFabric, pEventArray, pNumEvents) fabricExtractEventsV2_IMPL(pFabric, pEventArray, pNumEvents)
#endif //__nvoc_fabric_h_disabled

void fabricFlushUnhandledEvents_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricFlushUnhandledEvents(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricFlushUnhandledEvents(pFabric) fabricFlushUnhandledEvents_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NvU64 fabricGenerateEventId_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline NvU64 fabricGenerateEventId(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return 0;
}
#else //__nvoc_fabric_h_disabled
#define fabricGenerateEventId(pFabric) fabricGenerateEventId_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricImportCacheInsert_IMPL(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricImportCacheInsert(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricImportCacheInsert(pFabric, pExportUuid, key, pData) fabricImportCacheInsert_IMPL(pFabric, pExportUuid, key, pData)
#endif //__nvoc_fabric_h_disabled

void fabricImportCacheDelete_IMPL(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricImportCacheDelete(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricImportCacheDelete(pFabric, pExportUuid, key) fabricImportCacheDelete_IMPL(pFabric, pExportUuid, key)
#endif //__nvoc_fabric_h_disabled

void *fabricImportCacheGet_IMPL(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricImportCacheGet(struct Fabric *pFabric, const NvUuid *pExportUuid, NvU64 key) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricImportCacheGet(pFabric, pExportUuid, key) fabricImportCacheGet_IMPL(pFabric, pExportUuid, key)
#endif //__nvoc_fabric_h_disabled

void fabricImportCacheClear_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricImportCacheClear(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricImportCacheClear(pFabric) fabricImportCacheClear_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricUnimportCacheInsert_IMPL(struct Fabric *pFabric, NvU64 unimportEventId, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricUnimportCacheInsert(struct Fabric *pFabric, NvU64 unimportEventId, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricUnimportCacheInsert(pFabric, unimportEventId, pData) fabricUnimportCacheInsert_IMPL(pFabric, unimportEventId, pData)
#endif //__nvoc_fabric_h_disabled

void fabricUnimportCacheDelete_IMPL(struct Fabric *pFabric, NvU64 unimportEventId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricUnimportCacheDelete(struct Fabric *pFabric, NvU64 unimportEventId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricUnimportCacheDelete(pFabric, unimportEventId) fabricUnimportCacheDelete_IMPL(pFabric, unimportEventId)
#endif //__nvoc_fabric_h_disabled

void *fabricUnimportCacheGet_IMPL(struct Fabric *pFabric, NvU64 unimportEventId);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricUnimportCacheGet(struct Fabric *pFabric, NvU64 unimportEventId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricUnimportCacheGet(pFabric, unimportEventId) fabricUnimportCacheGet_IMPL(pFabric, unimportEventId)
#endif //__nvoc_fabric_h_disabled

void fabricUnimportCacheIterateAll_IMPL(struct Fabric *pFabric, void (*pCb)(void *));

#ifdef __nvoc_fabric_h_disabled
static inline void fabricUnimportCacheIterateAll(struct Fabric *pFabric, void (*pCb)(void *)) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricUnimportCacheIterateAll(pFabric, pCb) fabricUnimportCacheIterateAll_IMPL(pFabric, pCb)
#endif //__nvoc_fabric_h_disabled

NvBool fabricUnimportCacheInvokeCallback_IMPL(struct Fabric *pFabric, NvU64 unimportEventId, void (*pCb)(void *));

#ifdef __nvoc_fabric_h_disabled
static inline NvBool fabricUnimportCacheInvokeCallback(struct Fabric *pFabric, NvU64 unimportEventId, void (*pCb)(void *)) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_FALSE;
}
#else //__nvoc_fabric_h_disabled
#define fabricUnimportCacheInvokeCallback(pFabric, unimportEventId, pCb) fabricUnimportCacheInvokeCallback_IMPL(pFabric, unimportEventId, pCb)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricMulticastSetupCacheInsert_IMPL(struct Fabric *pFabric, NvU64 requesId, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricMulticastSetupCacheInsert(struct Fabric *pFabric, NvU64 requesId, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheInsert(pFabric, requesId, pData) fabricMulticastSetupCacheInsert_IMPL(pFabric, requesId, pData)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastSetupCacheDelete_IMPL(struct Fabric *pFabric, NvU64 requesId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastSetupCacheDelete(struct Fabric *pFabric, NvU64 requesId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheDelete(pFabric, requesId) fabricMulticastSetupCacheDelete_IMPL(pFabric, requesId)
#endif //__nvoc_fabric_h_disabled

void *fabricMulticastSetupCacheGet_IMPL(struct Fabric *pFabric, NvU64 requestId);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricMulticastSetupCacheGet(struct Fabric *pFabric, NvU64 requestId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheGet(pFabric, requestId) fabricMulticastSetupCacheGet_IMPL(pFabric, requestId)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricMulticastCleanupCacheInsert_IMPL(struct Fabric *pFabric, NvU64 requesId, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricMulticastCleanupCacheInsert(struct Fabric *pFabric, NvU64 requesId, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheInsert(pFabric, requesId, pData) fabricMulticastCleanupCacheInsert_IMPL(pFabric, requesId, pData)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastCleanupCacheDelete_IMPL(struct Fabric *pFabric, NvU64 requesId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastCleanupCacheDelete(struct Fabric *pFabric, NvU64 requesId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheDelete(pFabric, requesId) fabricMulticastCleanupCacheDelete_IMPL(pFabric, requesId)
#endif //__nvoc_fabric_h_disabled

void *fabricMulticastCleanupCacheGet_IMPL(struct Fabric *pFabric, NvU64 requestId);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricMulticastCleanupCacheGet(struct Fabric *pFabric, NvU64 requestId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheGet(pFabric, requestId) fabricMulticastCleanupCacheGet_IMPL(pFabric, requestId)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastCleanupCacheInvokeCallback_IMPL(struct Fabric *pFabric, NvU64 requestId, void (*pCb)(void *));

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastCleanupCacheInvokeCallback(struct Fabric *pFabric, NvU64 requestId, void (*pCb)(void *)) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheInvokeCallback(pFabric, requestId, pCb) fabricMulticastCleanupCacheInvokeCallback_IMPL(pFabric, requestId, pCb)
#endif //__nvoc_fabric_h_disabled

#undef PRIVATE_FIELD


#endif // _FABRIC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FABRIC_NVOC_H_
