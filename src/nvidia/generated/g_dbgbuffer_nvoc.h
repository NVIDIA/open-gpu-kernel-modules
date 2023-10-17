#ifndef _G_DBGBUFFER_NVOC_H_
#define _G_DBGBUFFER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_dbgbuffer_nvoc.h"

#ifndef _DBGBUFFER_H_
#define _DBGBUFFER_H_

#include "core/core.h"
#include "gpu/gpu_resource.h"
#include "class/cl00db.h"
#include "diagnostics/nv_debug_dump.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************


/*!
 * RM internal class representing NV40_DEBUG_BUFFER
 */
#ifdef NVOC_DBGBUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DebugBufferApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct DebugBufferApi *__nvoc_pbase_DebugBufferApi;
    NV_STATUS (*__dbgbufMap__)(struct DebugBufferApi *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__dbgbufUnmap__)(struct DebugBufferApi *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__dbgbufGetMapAddrSpace__)(struct DebugBufferApi *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__dbgbufGetMemoryMappingDescriptor__)(struct DebugBufferApi *, MEMORY_DESCRIPTOR **);
    NvBool (*__dbgbufShareCallback__)(struct DebugBufferApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dbgbufCheckMemInterUnmap__)(struct DebugBufferApi *, NvBool);
    NV_STATUS (*__dbgbufMapTo__)(struct DebugBufferApi *, RS_RES_MAP_TO_PARAMS *);
    NvU32 (*__dbgbufGetRefCount__)(struct DebugBufferApi *);
    void (*__dbgbufAddAdditionalDependants__)(struct RsClient *, struct DebugBufferApi *, RsResourceRef *);
    NV_STATUS (*__dbgbufControl_Prologue__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dbgbufGetRegBaseOffsetAndSize__)(struct DebugBufferApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__dbgbufInternalControlForward__)(struct DebugBufferApi *, NvU32, void *, NvU32);
    NV_STATUS (*__dbgbufUnmapFrom__)(struct DebugBufferApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dbgbufControl_Epilogue__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dbgbufControlLookup__)(struct DebugBufferApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__dbgbufGetInternalObjectHandle__)(struct DebugBufferApi *);
    NV_STATUS (*__dbgbufControl__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dbgbufGetMemInterMapParams__)(struct DebugBufferApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dbgbufControlFilter__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dbgbufControlSerialization_Prologue__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dbgbufCanCopy__)(struct DebugBufferApi *);
    void (*__dbgbufPreDestruct__)(struct DebugBufferApi *);
    NV_STATUS (*__dbgbufIsDuplicate__)(struct DebugBufferApi *, NvHandle, NvBool *);
    void (*__dbgbufControlSerialization_Epilogue__)(struct DebugBufferApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dbgbufAccessCallback__)(struct DebugBufferApi *, struct RsClient *, void *, RsAccessRight);
    MEMORY_DESCRIPTOR *pMemDesc;
};

#ifndef __NVOC_CLASS_DebugBufferApi_TYPEDEF__
#define __NVOC_CLASS_DebugBufferApi_TYPEDEF__
typedef struct DebugBufferApi DebugBufferApi;
#endif /* __NVOC_CLASS_DebugBufferApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DebugBufferApi
#define __nvoc_class_id_DebugBufferApi 0x5e7a1b
#endif /* __nvoc_class_id_DebugBufferApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DebugBufferApi;

#define __staticCast_DebugBufferApi(pThis) \
    ((pThis)->__nvoc_pbase_DebugBufferApi)

#ifdef __nvoc_dbgbuffer_h_disabled
#define __dynamicCast_DebugBufferApi(pThis) ((DebugBufferApi*)NULL)
#else //__nvoc_dbgbuffer_h_disabled
#define __dynamicCast_DebugBufferApi(pThis) \
    ((DebugBufferApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DebugBufferApi)))
#endif //__nvoc_dbgbuffer_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DebugBufferApi(DebugBufferApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DebugBufferApi(DebugBufferApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DebugBufferApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DebugBufferApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define dbgbufMap(pDebugBufferApi, pCallContext, pParams, pCpuMapping) dbgbufMap_DISPATCH(pDebugBufferApi, pCallContext, pParams, pCpuMapping)
#define dbgbufUnmap(pDebugBufferApi, pCallContext, pCpuMapping) dbgbufUnmap_DISPATCH(pDebugBufferApi, pCallContext, pCpuMapping)
#define dbgbufGetMapAddrSpace(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace) dbgbufGetMapAddrSpace_DISPATCH(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace)
#define dbgbufGetMemoryMappingDescriptor(pDebugBufferApi, ppMemDesc) dbgbufGetMemoryMappingDescriptor_DISPATCH(pDebugBufferApi, ppMemDesc)
#define dbgbufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dbgbufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dbgbufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dbgbufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dbgbufMapTo(pResource, pParams) dbgbufMapTo_DISPATCH(pResource, pParams)
#define dbgbufGetRefCount(pResource) dbgbufGetRefCount_DISPATCH(pResource)
#define dbgbufAddAdditionalDependants(pClient, pResource, pReference) dbgbufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dbgbufControl_Prologue(pResource, pCallContext, pParams) dbgbufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) dbgbufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define dbgbufInternalControlForward(pGpuResource, command, pParams, size) dbgbufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dbgbufUnmapFrom(pResource, pParams) dbgbufUnmapFrom_DISPATCH(pResource, pParams)
#define dbgbufControl_Epilogue(pResource, pCallContext, pParams) dbgbufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufControlLookup(pResource, pParams, ppEntry) dbgbufControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dbgbufGetInternalObjectHandle(pGpuResource) dbgbufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dbgbufControl(pGpuResource, pCallContext, pParams) dbgbufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dbgbufGetMemInterMapParams(pRmResource, pParams) dbgbufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dbgbufControlFilter(pResource, pCallContext, pParams) dbgbufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufControlSerialization_Prologue(pResource, pCallContext, pParams) dbgbufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufCanCopy(pResource) dbgbufCanCopy_DISPATCH(pResource)
#define dbgbufPreDestruct(pResource) dbgbufPreDestruct_DISPATCH(pResource)
#define dbgbufIsDuplicate(pResource, hMemory, pDuplicate) dbgbufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dbgbufControlSerialization_Epilogue(pResource, pCallContext, pParams) dbgbufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dbgbufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS dbgbufMap_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

static inline NV_STATUS dbgbufMap_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pDebugBufferApi->__dbgbufMap__(pDebugBufferApi, pCallContext, pParams, pCpuMapping);
}

NV_STATUS dbgbufUnmap_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

static inline NV_STATUS dbgbufUnmap_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pDebugBufferApi->__dbgbufUnmap__(pDebugBufferApi, pCallContext, pCpuMapping);
}

NV_STATUS dbgbufGetMapAddrSpace_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS dbgbufGetMapAddrSpace_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pDebugBufferApi->__dbgbufGetMapAddrSpace__(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS dbgbufGetMemoryMappingDescriptor_IMPL(struct DebugBufferApi *pDebugBufferApi, MEMORY_DESCRIPTOR **ppMemDesc);

static inline NV_STATUS dbgbufGetMemoryMappingDescriptor_DISPATCH(struct DebugBufferApi *pDebugBufferApi, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pDebugBufferApi->__dbgbufGetMemoryMappingDescriptor__(pDebugBufferApi, ppMemDesc);
}

static inline NvBool dbgbufShareCallback_DISPATCH(struct DebugBufferApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dbgbufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dbgbufCheckMemInterUnmap_DISPATCH(struct DebugBufferApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dbgbufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dbgbufMapTo_DISPATCH(struct DebugBufferApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dbgbufMapTo__(pResource, pParams);
}

static inline NvU32 dbgbufGetRefCount_DISPATCH(struct DebugBufferApi *pResource) {
    return pResource->__dbgbufGetRefCount__(pResource);
}

static inline void dbgbufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DebugBufferApi *pResource, RsResourceRef *pReference) {
    pResource->__dbgbufAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dbgbufControl_Prologue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dbgbufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dbgbufGetRegBaseOffsetAndSize_DISPATCH(struct DebugBufferApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__dbgbufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS dbgbufInternalControlForward_DISPATCH(struct DebugBufferApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dbgbufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS dbgbufUnmapFrom_DISPATCH(struct DebugBufferApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dbgbufUnmapFrom__(pResource, pParams);
}

static inline void dbgbufControl_Epilogue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dbgbufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dbgbufControlLookup_DISPATCH(struct DebugBufferApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dbgbufControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle dbgbufGetInternalObjectHandle_DISPATCH(struct DebugBufferApi *pGpuResource) {
    return pGpuResource->__dbgbufGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dbgbufControl_DISPATCH(struct DebugBufferApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dbgbufControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dbgbufGetMemInterMapParams_DISPATCH(struct DebugBufferApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dbgbufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dbgbufControlFilter_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dbgbufControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dbgbufControlSerialization_Prologue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dbgbufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dbgbufCanCopy_DISPATCH(struct DebugBufferApi *pResource) {
    return pResource->__dbgbufCanCopy__(pResource);
}

static inline void dbgbufPreDestruct_DISPATCH(struct DebugBufferApi *pResource) {
    pResource->__dbgbufPreDestruct__(pResource);
}

static inline NV_STATUS dbgbufIsDuplicate_DISPATCH(struct DebugBufferApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dbgbufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dbgbufControlSerialization_Epilogue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dbgbufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dbgbufAccessCallback_DISPATCH(struct DebugBufferApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dbgbufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dbgbufConstruct_IMPL(struct DebugBufferApi *arg_pDebugBufferApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dbgbufConstruct(arg_pDebugBufferApi, arg_pCallContext, arg_pParams) dbgbufConstruct_IMPL(arg_pDebugBufferApi, arg_pCallContext, arg_pParams)
void dbgbufDestruct_IMPL(struct DebugBufferApi *pDebugBufferApi);

#define __nvoc_dbgbufDestruct(pDebugBufferApi) dbgbufDestruct_IMPL(pDebugBufferApi)
#undef PRIVATE_FIELD


#endif // _DBGBUFFER_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DBGBUFFER_NVOC_H_
