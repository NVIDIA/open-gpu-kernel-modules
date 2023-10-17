#ifndef _G_FABRIC_NVOC_H_
#define _G_FABRIC_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_fabric_nvoc.h"

#ifndef _FABRIC_H_
#define _FABRIC_H_

#include "core/core.h"
#include "core/system.h"
#include "containers/multimap.h"
#include "containers/map.h"
#include "class/cl000f.h"
#include "ctrl/ctrl000f.h"



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

#include "nvlink_inband_msg.h"

void fabricMulticastWaitOnTeamCleanupCallback(void *pCbData);

//
// The Fabric object is used to encapsulate the NVLink fabric
//
#ifdef NVOC_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Fabric {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct Fabric *__nvoc_pbase_Fabric;
    NvU32 flags;
    PORT_MUTEX *pMulticastFabricOpsMutex;
    FabricCache fabricMulticastCache;
};

#ifndef __NVOC_CLASS_Fabric_TYPEDEF__
#define __NVOC_CLASS_Fabric_TYPEDEF__
typedef struct Fabric Fabric;
#endif /* __NVOC_CLASS_Fabric_TYPEDEF__ */

#ifndef __nvoc_class_id_Fabric
#define __nvoc_class_id_Fabric 0x0ac791
#endif /* __nvoc_class_id_Fabric */

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

void fabricMulticastFabricOpsMutexAcquire_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastFabricOpsMutexAcquire(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastFabricOpsMutexAcquire(pFabric) fabricMulticastFabricOpsMutexAcquire_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastFabricOpsMutexRelease_IMPL(struct Fabric *pFabric);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastFabricOpsMutexRelease(struct Fabric *pFabric) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastFabricOpsMutexRelease(pFabric) fabricMulticastFabricOpsMutexRelease_IMPL(pFabric)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricMulticastSetupCacheInsertUnderLock_IMPL(struct Fabric *pFabric, NvU64 requesId, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricMulticastSetupCacheInsertUnderLock(struct Fabric *pFabric, NvU64 requesId, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheInsertUnderLock(pFabric, requesId, pData) fabricMulticastSetupCacheInsertUnderLock_IMPL(pFabric, requesId, pData)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastSetupCacheDeleteUnderLock_IMPL(struct Fabric *pFabric, NvU64 requesId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastSetupCacheDeleteUnderLock(struct Fabric *pFabric, NvU64 requesId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheDeleteUnderLock(pFabric, requesId) fabricMulticastSetupCacheDeleteUnderLock_IMPL(pFabric, requesId)
#endif //__nvoc_fabric_h_disabled

void *fabricMulticastSetupCacheGetUnderLock_IMPL(struct Fabric *pFabric, NvU64 requestId);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricMulticastSetupCacheGetUnderLock(struct Fabric *pFabric, NvU64 requestId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastSetupCacheGetUnderLock(pFabric, requestId) fabricMulticastSetupCacheGetUnderLock_IMPL(pFabric, requestId)
#endif //__nvoc_fabric_h_disabled

NV_STATUS fabricMulticastCleanupCacheInsertUnderLock_IMPL(struct Fabric *pFabric, NvU64 requesId, void *pData);

#ifdef __nvoc_fabric_h_disabled
static inline NV_STATUS fabricMulticastCleanupCacheInsertUnderLock(struct Fabric *pFabric, NvU64 requesId, void *pData) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheInsertUnderLock(pFabric, requesId, pData) fabricMulticastCleanupCacheInsertUnderLock_IMPL(pFabric, requesId, pData)
#endif //__nvoc_fabric_h_disabled

void fabricMulticastCleanupCacheDeleteUnderLock_IMPL(struct Fabric *pFabric, NvU64 requesId);

#ifdef __nvoc_fabric_h_disabled
static inline void fabricMulticastCleanupCacheDeleteUnderLock(struct Fabric *pFabric, NvU64 requesId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheDeleteUnderLock(pFabric, requesId) fabricMulticastCleanupCacheDeleteUnderLock_IMPL(pFabric, requesId)
#endif //__nvoc_fabric_h_disabled

void *fabricMulticastCleanupCacheGetUnderLock_IMPL(struct Fabric *pFabric, NvU64 requestId);

#ifdef __nvoc_fabric_h_disabled
static inline void *fabricMulticastCleanupCacheGetUnderLock(struct Fabric *pFabric, NvU64 requestId) {
    NV_ASSERT_FAILED_PRECOMP("Fabric was disabled!");
    return NULL;
}
#else //__nvoc_fabric_h_disabled
#define fabricMulticastCleanupCacheGetUnderLock(pFabric, requestId) fabricMulticastCleanupCacheGetUnderLock_IMPL(pFabric, requestId)
#endif //__nvoc_fabric_h_disabled

#undef PRIVATE_FIELD


#endif // _FABRIC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FABRIC_NVOC_H_
