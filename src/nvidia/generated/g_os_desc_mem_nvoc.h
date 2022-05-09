#ifndef _G_OS_DESC_MEM_NVOC_H_
#define _G_OS_DESC_MEM_NVOC_H_
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

#include "g_os_desc_mem_nvoc.h"

#ifndef _OS_DESC_MEMORY_H_
#define _OS_DESC_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Bind memory allocated through os descriptor
 */
#ifdef NVOC_OS_DESC_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OsDescMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct OsDescMemory *__nvoc_pbase_OsDescMemory;
    NvBool (*__osdescCanCopy__)(struct OsDescMemory *);
    NV_STATUS (*__osdescCheckMemInterUnmap__)(struct OsDescMemory *, NvBool);
    NV_STATUS (*__osdescControl__)(struct OsDescMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__osdescUnmap__)(struct OsDescMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__osdescGetMemInterMapParams__)(struct OsDescMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__osdescGetMemoryMappingDescriptor__)(struct OsDescMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__osdescGetMapAddrSpace__)(struct OsDescMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__osdescShareCallback__)(struct OsDescMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__osdescControlFilter__)(struct OsDescMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__osdescAddAdditionalDependants__)(struct RsClient *, struct OsDescMemory *, RsResourceRef *);
    NvU32 (*__osdescGetRefCount__)(struct OsDescMemory *);
    NV_STATUS (*__osdescMapTo__)(struct OsDescMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__osdescControl_Prologue__)(struct OsDescMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__osdescIsReady__)(struct OsDescMemory *);
    NV_STATUS (*__osdescCheckCopyPermissions__)(struct OsDescMemory *, struct OBJGPU *, NvHandle);
    void (*__osdescPreDestruct__)(struct OsDescMemory *);
    NV_STATUS (*__osdescUnmapFrom__)(struct OsDescMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__osdescControl_Epilogue__)(struct OsDescMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__osdescControlLookup__)(struct OsDescMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__osdescMap__)(struct OsDescMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__osdescAccessCallback__)(struct OsDescMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_OsDescMemory_TYPEDEF__
#define __NVOC_CLASS_OsDescMemory_TYPEDEF__
typedef struct OsDescMemory OsDescMemory;
#endif /* __NVOC_CLASS_OsDescMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_OsDescMemory
#define __nvoc_class_id_OsDescMemory 0xb3dacd
#endif /* __nvoc_class_id_OsDescMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OsDescMemory;

#define __staticCast_OsDescMemory(pThis) \
    ((pThis)->__nvoc_pbase_OsDescMemory)

#ifdef __nvoc_os_desc_mem_h_disabled
#define __dynamicCast_OsDescMemory(pThis) ((OsDescMemory*)NULL)
#else //__nvoc_os_desc_mem_h_disabled
#define __dynamicCast_OsDescMemory(pThis) \
    ((OsDescMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OsDescMemory)))
#endif //__nvoc_os_desc_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OsDescMemory(OsDescMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OsDescMemory(OsDescMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_OsDescMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_OsDescMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define osdescCanCopy(pOsDescMemory) osdescCanCopy_DISPATCH(pOsDescMemory)
#define osdescCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) osdescCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define osdescControl(pMemory, pCallContext, pParams) osdescControl_DISPATCH(pMemory, pCallContext, pParams)
#define osdescUnmap(pMemory, pCallContext, pCpuMapping) osdescUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define osdescGetMemInterMapParams(pMemory, pParams) osdescGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define osdescGetMemoryMappingDescriptor(pMemory, ppMemDesc) osdescGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define osdescGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) osdescGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define osdescShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) osdescShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define osdescControlFilter(pResource, pCallContext, pParams) osdescControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define osdescAddAdditionalDependants(pClient, pResource, pReference) osdescAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define osdescGetRefCount(pResource) osdescGetRefCount_DISPATCH(pResource)
#define osdescMapTo(pResource, pParams) osdescMapTo_DISPATCH(pResource, pParams)
#define osdescControl_Prologue(pResource, pCallContext, pParams) osdescControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define osdescIsReady(pMemory) osdescIsReady_DISPATCH(pMemory)
#define osdescCheckCopyPermissions(pMemory, pDstGpu, hDstClientNvBool) osdescCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, hDstClientNvBool)
#define osdescPreDestruct(pResource) osdescPreDestruct_DISPATCH(pResource)
#define osdescUnmapFrom(pResource, pParams) osdescUnmapFrom_DISPATCH(pResource, pParams)
#define osdescControl_Epilogue(pResource, pCallContext, pParams) osdescControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define osdescControlLookup(pResource, pParams, ppEntry) osdescControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define osdescMap(pMemory, pCallContext, pParams, pCpuMapping) osdescMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define osdescAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) osdescAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool osdescCanCopy_IMPL(struct OsDescMemory *pOsDescMemory);

static inline NvBool osdescCanCopy_DISPATCH(struct OsDescMemory *pOsDescMemory) {
    return pOsDescMemory->__osdescCanCopy__(pOsDescMemory);
}

static inline NV_STATUS osdescCheckMemInterUnmap_DISPATCH(struct OsDescMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__osdescCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS osdescControl_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__osdescControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS osdescUnmap_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__osdescUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS osdescGetMemInterMapParams_DISPATCH(struct OsDescMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__osdescGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS osdescGetMemoryMappingDescriptor_DISPATCH(struct OsDescMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__osdescGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS osdescGetMapAddrSpace_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__osdescGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool osdescShareCallback_DISPATCH(struct OsDescMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__osdescShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS osdescControlFilter_DISPATCH(struct OsDescMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__osdescControlFilter__(pResource, pCallContext, pParams);
}

static inline void osdescAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct OsDescMemory *pResource, RsResourceRef *pReference) {
    pResource->__osdescAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 osdescGetRefCount_DISPATCH(struct OsDescMemory *pResource) {
    return pResource->__osdescGetRefCount__(pResource);
}

static inline NV_STATUS osdescMapTo_DISPATCH(struct OsDescMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__osdescMapTo__(pResource, pParams);
}

static inline NV_STATUS osdescControl_Prologue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__osdescControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS osdescIsReady_DISPATCH(struct OsDescMemory *pMemory) {
    return pMemory->__osdescIsReady__(pMemory);
}

static inline NV_STATUS osdescCheckCopyPermissions_DISPATCH(struct OsDescMemory *pMemory, struct OBJGPU *pDstGpu, NvHandle hDstClientNvBool) {
    return pMemory->__osdescCheckCopyPermissions__(pMemory, pDstGpu, hDstClientNvBool);
}

static inline void osdescPreDestruct_DISPATCH(struct OsDescMemory *pResource) {
    pResource->__osdescPreDestruct__(pResource);
}

static inline NV_STATUS osdescUnmapFrom_DISPATCH(struct OsDescMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__osdescUnmapFrom__(pResource, pParams);
}

static inline void osdescControl_Epilogue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__osdescControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS osdescControlLookup_DISPATCH(struct OsDescMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__osdescControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS osdescMap_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__osdescMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool osdescAccessCallback_DISPATCH(struct OsDescMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__osdescAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS osdescConstruct_IMPL(struct OsDescMemory *arg_pOsDescMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_osdescConstruct(arg_pOsDescMemory, arg_pCallContext, arg_pParams) osdescConstruct_IMPL(arg_pOsDescMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_OS_DESC_MEM_NVOC_H_
