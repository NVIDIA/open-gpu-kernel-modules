#ifndef _G_MIG_MONITOR_SESSION_NVOC_H_
#define _G_MIG_MONITOR_SESSION_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      This file contains functions to grant MIG monitor capability
 *
 *   Key attributes of MIGMonitorSession class:
 *   - hClient is parent of MIGMonitorSession.
 *   - As the MIG monitor capability is world accessible by default,
 *     MIGMonitorSession allocation requires a privileged client only
 *     if the platform doesn't implement this capability
 *   - RmApi lock must be held.
 *****************************************************************************/

#include "g_mig_monitor_session_nvoc.h"

#ifndef MIG_MONITOR_SESSION_H
#define MIG_MONITOR_SESSION_H

#include "rmapi/resource.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

#ifdef NVOC_MIG_MONITOR_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MIGMonitorSession {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct MIGMonitorSession *__nvoc_pbase_MIGMonitorSession;
    NvBool (*__migmonitorsessionShareCallback__)(struct MIGMonitorSession *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__migmonitorsessionCheckMemInterUnmap__)(struct MIGMonitorSession *, NvBool);
    NV_STATUS (*__migmonitorsessionControl__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__migmonitorsessionGetMemInterMapParams__)(struct MIGMonitorSession *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__migmonitorsessionGetMemoryMappingDescriptor__)(struct MIGMonitorSession *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__migmonitorsessionGetRefCount__)(struct MIGMonitorSession *);
    NV_STATUS (*__migmonitorsessionControlFilter__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__migmonitorsessionAddAdditionalDependants__)(struct RsClient *, struct MIGMonitorSession *, RsResourceRef *);
    NV_STATUS (*__migmonitorsessionUnmapFrom__)(struct MIGMonitorSession *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__migmonitorsessionControlSerialization_Prologue__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__migmonitorsessionControl_Prologue__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__migmonitorsessionCanCopy__)(struct MIGMonitorSession *);
    NV_STATUS (*__migmonitorsessionUnmap__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__migmonitorsessionPreDestruct__)(struct MIGMonitorSession *);
    NV_STATUS (*__migmonitorsessionMapTo__)(struct MIGMonitorSession *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__migmonitorsessionIsDuplicate__)(struct MIGMonitorSession *, NvHandle, NvBool *);
    void (*__migmonitorsessionControlSerialization_Epilogue__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__migmonitorsessionControl_Epilogue__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__migmonitorsessionControlLookup__)(struct MIGMonitorSession *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__migmonitorsessionMap__)(struct MIGMonitorSession *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__migmonitorsessionAccessCallback__)(struct MIGMonitorSession *, struct RsClient *, void *, RsAccessRight);
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};

#ifndef __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
#define __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
typedef struct MIGMonitorSession MIGMonitorSession;
#endif /* __NVOC_CLASS_MIGMonitorSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGMonitorSession
#define __nvoc_class_id_MIGMonitorSession 0x29e15c
#endif /* __nvoc_class_id_MIGMonitorSession */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession;

#define __staticCast_MIGMonitorSession(pThis) \
    ((pThis)->__nvoc_pbase_MIGMonitorSession)

#ifdef __nvoc_mig_monitor_session_h_disabled
#define __dynamicCast_MIGMonitorSession(pThis) ((MIGMonitorSession*)NULL)
#else //__nvoc_mig_monitor_session_h_disabled
#define __dynamicCast_MIGMonitorSession(pThis) \
    ((MIGMonitorSession*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MIGMonitorSession)))
#endif //__nvoc_mig_monitor_session_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MIGMonitorSession(MIGMonitorSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MIGMonitorSession(MIGMonitorSession**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MIGMonitorSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MIGMonitorSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define migmonitorsessionShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) migmonitorsessionShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define migmonitorsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) migmonitorsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define migmonitorsessionControl(pResource, pCallContext, pParams) migmonitorsessionControl_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionGetMemInterMapParams(pRmResource, pParams) migmonitorsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define migmonitorsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) migmonitorsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define migmonitorsessionGetRefCount(pResource) migmonitorsessionGetRefCount_DISPATCH(pResource)
#define migmonitorsessionControlFilter(pResource, pCallContext, pParams) migmonitorsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionAddAdditionalDependants(pClient, pResource, pReference) migmonitorsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define migmonitorsessionUnmapFrom(pResource, pParams) migmonitorsessionUnmapFrom_DISPATCH(pResource, pParams)
#define migmonitorsessionControlSerialization_Prologue(pResource, pCallContext, pParams) migmonitorsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControl_Prologue(pResource, pCallContext, pParams) migmonitorsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionCanCopy(pResource) migmonitorsessionCanCopy_DISPATCH(pResource)
#define migmonitorsessionUnmap(pResource, pCallContext, pCpuMapping) migmonitorsessionUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define migmonitorsessionPreDestruct(pResource) migmonitorsessionPreDestruct_DISPATCH(pResource)
#define migmonitorsessionMapTo(pResource, pParams) migmonitorsessionMapTo_DISPATCH(pResource, pParams)
#define migmonitorsessionIsDuplicate(pResource, hMemory, pDuplicate) migmonitorsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define migmonitorsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) migmonitorsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControl_Epilogue(pResource, pCallContext, pParams) migmonitorsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControlLookup(pResource, pParams, ppEntry) migmonitorsessionControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define migmonitorsessionMap(pResource, pCallContext, pParams, pCpuMapping) migmonitorsessionMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define migmonitorsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) migmonitorsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool migmonitorsessionShareCallback_DISPATCH(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__migmonitorsessionShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS migmonitorsessionCheckMemInterUnmap_DISPATCH(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__migmonitorsessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS migmonitorsessionControl_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__migmonitorsessionControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionGetMemInterMapParams_DISPATCH(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__migmonitorsessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS migmonitorsessionGetMemoryMappingDescriptor_DISPATCH(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__migmonitorsessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 migmonitorsessionGetRefCount_DISPATCH(struct MIGMonitorSession *pResource) {
    return pResource->__migmonitorsessionGetRefCount__(pResource);
}

static inline NV_STATUS migmonitorsessionControlFilter_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__migmonitorsessionControlFilter__(pResource, pCallContext, pParams);
}

static inline void migmonitorsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference) {
    pResource->__migmonitorsessionAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS migmonitorsessionUnmapFrom_DISPATCH(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__migmonitorsessionUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS migmonitorsessionControlSerialization_Prologue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__migmonitorsessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionControl_Prologue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__migmonitorsessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool migmonitorsessionCanCopy_DISPATCH(struct MIGMonitorSession *pResource) {
    return pResource->__migmonitorsessionCanCopy__(pResource);
}

static inline NV_STATUS migmonitorsessionUnmap_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__migmonitorsessionUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void migmonitorsessionPreDestruct_DISPATCH(struct MIGMonitorSession *pResource) {
    pResource->__migmonitorsessionPreDestruct__(pResource);
}

static inline NV_STATUS migmonitorsessionMapTo_DISPATCH(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__migmonitorsessionMapTo__(pResource, pParams);
}

static inline NV_STATUS migmonitorsessionIsDuplicate_DISPATCH(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__migmonitorsessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void migmonitorsessionControlSerialization_Epilogue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__migmonitorsessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void migmonitorsessionControl_Epilogue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__migmonitorsessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionControlLookup_DISPATCH(struct MIGMonitorSession *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__migmonitorsessionControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS migmonitorsessionMap_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__migmonitorsessionMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool migmonitorsessionAccessCallback_DISPATCH(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__migmonitorsessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS migmonitorsessionConstruct_IMPL(struct MIGMonitorSession *arg_pMIGMonitorSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_migmonitorsessionConstruct(arg_pMIGMonitorSession, arg_pCallContext, arg_pParams) migmonitorsessionConstruct_IMPL(arg_pMIGMonitorSession, arg_pCallContext, arg_pParams)
void migmonitorsessionDestruct_IMPL(struct MIGMonitorSession *pMIGMonitorSession);

#define __nvoc_migmonitorsessionDestruct(pMIGMonitorSession) migmonitorsessionDestruct_IMPL(pMIGMonitorSession)
#undef PRIVATE_FIELD


#endif // MIG_MONITOR_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MIG_MONITOR_SESSION_NVOC_H_
