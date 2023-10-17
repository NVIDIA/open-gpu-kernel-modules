#ifndef _G_MEM_LIST_NVOC_H_
#define _G_MEM_LIST_NVOC_H_
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

#include "g_mem_list_nvoc.h"

#ifndef _MEMORY_LIST_H_
#define _MEMORY_LIST_H_

#include "mem_mgr/mem.h"

/*!
 * These classes are used by the vGPU support to create memory objects for memory
 * assigned to a guest VM.
 */
#ifdef NVOC_MEM_LIST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MemoryList {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct MemoryList *__nvoc_pbase_MemoryList;
    NvBool (*__memlistCanCopy__)(struct MemoryList *);
    NV_STATUS (*__memlistCheckMemInterUnmap__)(struct MemoryList *, NvBool);
    NvBool (*__memlistShareCallback__)(struct MemoryList *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memlistMapTo__)(struct MemoryList *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memlistGetMapAddrSpace__)(struct MemoryList *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__memlistIsExportAllowed__)(struct MemoryList *);
    NvU32 (*__memlistGetRefCount__)(struct MemoryList *);
    void (*__memlistAddAdditionalDependants__)(struct RsClient *, struct MemoryList *, RsResourceRef *);
    NV_STATUS (*__memlistControl_Prologue__)(struct MemoryList *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__memlistIsGpuMapAllowed__)(struct MemoryList *, struct OBJGPU *);
    NV_STATUS (*__memlistUnmapFrom__)(struct MemoryList *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__memlistControl_Epilogue__)(struct MemoryList *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memlistControlLookup__)(struct MemoryList *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__memlistControl__)(struct MemoryList *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memlistUnmap__)(struct MemoryList *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__memlistGetMemInterMapParams__)(struct MemoryList *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memlistGetMemoryMappingDescriptor__)(struct MemoryList *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__memlistControlFilter__)(struct MemoryList *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memlistControlSerialization_Prologue__)(struct MemoryList *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memlistIsReady__)(struct MemoryList *, NvBool);
    NV_STATUS (*__memlistCheckCopyPermissions__)(struct MemoryList *, struct OBJGPU *, struct Device *);
    void (*__memlistPreDestruct__)(struct MemoryList *);
    NV_STATUS (*__memlistIsDuplicate__)(struct MemoryList *, NvHandle, NvBool *);
    void (*__memlistControlSerialization_Epilogue__)(struct MemoryList *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memlistMap__)(struct MemoryList *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__memlistAccessCallback__)(struct MemoryList *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_MemoryList_TYPEDEF__
#define __NVOC_CLASS_MemoryList_TYPEDEF__
typedef struct MemoryList MemoryList;
#endif /* __NVOC_CLASS_MemoryList_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryList
#define __nvoc_class_id_MemoryList 0x298f78
#endif /* __nvoc_class_id_MemoryList */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList;

#define __staticCast_MemoryList(pThis) \
    ((pThis)->__nvoc_pbase_MemoryList)

#ifdef __nvoc_mem_list_h_disabled
#define __dynamicCast_MemoryList(pThis) ((MemoryList*)NULL)
#else //__nvoc_mem_list_h_disabled
#define __dynamicCast_MemoryList(pThis) \
    ((MemoryList*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryList)))
#endif //__nvoc_mem_list_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryList(MemoryList**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryList(MemoryList**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryList(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryList((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memlistCanCopy(pMemoryList) memlistCanCopy_DISPATCH(pMemoryList)
#define memlistCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memlistCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memlistShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memlistShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memlistMapTo(pResource, pParams) memlistMapTo_DISPATCH(pResource, pParams)
#define memlistGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) memlistGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define memlistIsExportAllowed(pMemory) memlistIsExportAllowed_DISPATCH(pMemory)
#define memlistGetRefCount(pResource) memlistGetRefCount_DISPATCH(pResource)
#define memlistAddAdditionalDependants(pClient, pResource, pReference) memlistAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memlistControl_Prologue(pResource, pCallContext, pParams) memlistControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memlistIsGpuMapAllowed(pMemory, pGpu) memlistIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define memlistUnmapFrom(pResource, pParams) memlistUnmapFrom_DISPATCH(pResource, pParams)
#define memlistControl_Epilogue(pResource, pCallContext, pParams) memlistControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memlistControlLookup(pResource, pParams, ppEntry) memlistControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define memlistControl(pMemory, pCallContext, pParams) memlistControl_DISPATCH(pMemory, pCallContext, pParams)
#define memlistUnmap(pMemory, pCallContext, pCpuMapping) memlistUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memlistGetMemInterMapParams(pMemory, pParams) memlistGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memlistGetMemoryMappingDescriptor(pMemory, ppMemDesc) memlistGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memlistControlFilter(pResource, pCallContext, pParams) memlistControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memlistControlSerialization_Prologue(pResource, pCallContext, pParams) memlistControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memlistIsReady(pMemory, bCopyConstructorContext) memlistIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define memlistCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memlistCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memlistPreDestruct(pResource) memlistPreDestruct_DISPATCH(pResource)
#define memlistIsDuplicate(pMemory, hMemory, pDuplicate) memlistIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memlistControlSerialization_Epilogue(pResource, pCallContext, pParams) memlistControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memlistMap(pMemory, pCallContext, pParams, pCpuMapping) memlistMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memlistAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memlistAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool memlistCanCopy_IMPL(struct MemoryList *pMemoryList);

static inline NvBool memlistCanCopy_DISPATCH(struct MemoryList *pMemoryList) {
    return pMemoryList->__memlistCanCopy__(pMemoryList);
}

static inline NV_STATUS memlistCheckMemInterUnmap_DISPATCH(struct MemoryList *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__memlistCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool memlistShareCallback_DISPATCH(struct MemoryList *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memlistShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memlistMapTo_DISPATCH(struct MemoryList *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memlistMapTo__(pResource, pParams);
}

static inline NV_STATUS memlistGetMapAddrSpace_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__memlistGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool memlistIsExportAllowed_DISPATCH(struct MemoryList *pMemory) {
    return pMemory->__memlistIsExportAllowed__(pMemory);
}

static inline NvU32 memlistGetRefCount_DISPATCH(struct MemoryList *pResource) {
    return pResource->__memlistGetRefCount__(pResource);
}

static inline void memlistAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryList *pResource, RsResourceRef *pReference) {
    pResource->__memlistAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS memlistControl_Prologue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool memlistIsGpuMapAllowed_DISPATCH(struct MemoryList *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__memlistIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS memlistUnmapFrom_DISPATCH(struct MemoryList *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memlistUnmapFrom__(pResource, pParams);
}

static inline void memlistControl_Epilogue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memlistControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memlistControlLookup_DISPATCH(struct MemoryList *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__memlistControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS memlistControl_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__memlistControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS memlistUnmap_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__memlistUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memlistGetMemInterMapParams_DISPATCH(struct MemoryList *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__memlistGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memlistGetMemoryMappingDescriptor_DISPATCH(struct MemoryList *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__memlistGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS memlistControlFilter_DISPATCH(struct MemoryList *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memlistControlSerialization_Prologue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memlistIsReady_DISPATCH(struct MemoryList *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__memlistIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS memlistCheckCopyPermissions_DISPATCH(struct MemoryList *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__memlistCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void memlistPreDestruct_DISPATCH(struct MemoryList *pResource) {
    pResource->__memlistPreDestruct__(pResource);
}

static inline NV_STATUS memlistIsDuplicate_DISPATCH(struct MemoryList *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__memlistIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void memlistControlSerialization_Epilogue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memlistControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memlistMap_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__memlistMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memlistAccessCallback_DISPATCH(struct MemoryList *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memlistAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS memlistConstruct_IMPL(struct MemoryList *arg_pMemoryList, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memlistConstruct(arg_pMemoryList, arg_pCallContext, arg_pParams) memlistConstruct_IMPL(arg_pMemoryList, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_LIST_NVOC_H_
