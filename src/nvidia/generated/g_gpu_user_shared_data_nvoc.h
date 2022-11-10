#ifndef _G_GPU_USER_SHARED_DATA_NVOC_H_
#define _G_GPU_USER_SHARED_DATA_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_gpu_user_shared_data_nvoc.h"

#ifndef GPU_USER_SHARED_DATA_H
#define GPU_USER_SHARED_DATA_H

#include "core/core.h"
#include "gpu/gpu_resource.h"
#include "gpu/mem_mgr/mem_desc.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

/*!
 * RM internal class representing RM_USER_SHARED_DATA
 */
#ifdef NVOC_GPU_USER_SHARED_DATA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GpuUserSharedData {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct GpuUserSharedData *__nvoc_pbase_GpuUserSharedData;
    NV_STATUS (*__gpushareddataMap__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__gpushareddataUnmap__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__gpushareddataGetMapAddrSpace__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__gpushareddataGetMemoryMappingDescriptor__)(struct GpuUserSharedData *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__gpushareddataShareCallback__)(struct GpuUserSharedData *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gpushareddataControl__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataGetMemInterMapParams__)(struct GpuUserSharedData *, RMRES_MEM_INTER_MAP_PARAMS *);
    NvHandle (*__gpushareddataGetInternalObjectHandle__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataControlFilter__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__gpushareddataAddAdditionalDependants__)(struct RsClient *, struct GpuUserSharedData *, RsResourceRef *);
    NvU32 (*__gpushareddataGetRefCount__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataCheckMemInterUnmap__)(struct GpuUserSharedData *, NvBool);
    NV_STATUS (*__gpushareddataMapTo__)(struct GpuUserSharedData *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__gpushareddataControl_Prologue__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataGetRegBaseOffsetAndSize__)(struct GpuUserSharedData *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__gpushareddataCanCopy__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataInternalControlForward__)(struct GpuUserSharedData *, NvU32, void *, NvU32);
    void (*__gpushareddataPreDestruct__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataUnmapFrom__)(struct GpuUserSharedData *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__gpushareddataIsDuplicate__)(struct GpuUserSharedData *, NvHandle, NvBool *);
    void (*__gpushareddataControl_Epilogue__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataControlLookup__)(struct GpuUserSharedData *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvBool (*__gpushareddataAccessCallback__)(struct GpuUserSharedData *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
#define __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
typedef struct GpuUserSharedData GpuUserSharedData;
#endif /* __NVOC_CLASS_GpuUserSharedData_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuUserSharedData
#define __nvoc_class_id_GpuUserSharedData 0x5e7d1f
#endif /* __nvoc_class_id_GpuUserSharedData */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData;

#define __staticCast_GpuUserSharedData(pThis) \
    ((pThis)->__nvoc_pbase_GpuUserSharedData)

#ifdef __nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) ((GpuUserSharedData*)NULL)
#else //__nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) \
    ((GpuUserSharedData*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuUserSharedData)))
#endif //__nvoc_gpu_user_shared_data_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GpuUserSharedData(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuUserSharedData((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gpushareddataMap(pData, pCallContext, pParams, pCpuMapping) gpushareddataMap_DISPATCH(pData, pCallContext, pParams, pCpuMapping)
#define gpushareddataUnmap(pData, pCallContext, pCpuMapping) gpushareddataUnmap_DISPATCH(pData, pCallContext, pCpuMapping)
#define gpushareddataGetMapAddrSpace(pData, pCallContext, mapFlags, pAddrSpace) gpushareddataGetMapAddrSpace_DISPATCH(pData, pCallContext, mapFlags, pAddrSpace)
#define gpushareddataGetMemoryMappingDescriptor(pData, ppMemDesc) gpushareddataGetMemoryMappingDescriptor_DISPATCH(pData, ppMemDesc)
#define gpushareddataShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) gpushareddataShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpushareddataControl(pGpuResource, pCallContext, pParams) gpushareddataControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define gpushareddataGetMemInterMapParams(pRmResource, pParams) gpushareddataGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gpushareddataGetInternalObjectHandle(pGpuResource) gpushareddataGetInternalObjectHandle_DISPATCH(pGpuResource)
#define gpushareddataControlFilter(pResource, pCallContext, pParams) gpushareddataControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataAddAdditionalDependants(pClient, pResource, pReference) gpushareddataAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gpushareddataGetRefCount(pResource) gpushareddataGetRefCount_DISPATCH(pResource)
#define gpushareddataCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gpushareddataCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gpushareddataMapTo(pResource, pParams) gpushareddataMapTo_DISPATCH(pResource, pParams)
#define gpushareddataControl_Prologue(pResource, pCallContext, pParams) gpushareddataControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) gpushareddataGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define gpushareddataCanCopy(pResource) gpushareddataCanCopy_DISPATCH(pResource)
#define gpushareddataInternalControlForward(pGpuResource, command, pParams, size) gpushareddataInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define gpushareddataPreDestruct(pResource) gpushareddataPreDestruct_DISPATCH(pResource)
#define gpushareddataUnmapFrom(pResource, pParams) gpushareddataUnmapFrom_DISPATCH(pResource, pParams)
#define gpushareddataIsDuplicate(pResource, hMemory, pDuplicate) gpushareddataIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gpushareddataControl_Epilogue(pResource, pCallContext, pParams) gpushareddataControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControlLookup(pResource, pParams, ppEntry) gpushareddataControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gpushareddataAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpushareddataAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS gpushareddataMap_IMPL(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS gpushareddataMap_DISPATCH(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pData->__gpushareddataMap__(pData, pCallContext, pParams, pCpuMapping);
}

NV_STATUS gpushareddataUnmap_IMPL(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS gpushareddataUnmap_DISPATCH(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pData->__gpushareddataUnmap__(pData, pCallContext, pCpuMapping);
}

NV_STATUS gpushareddataGetMapAddrSpace_IMPL(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS gpushareddataGetMapAddrSpace_DISPATCH(struct GpuUserSharedData *pData, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pData->__gpushareddataGetMapAddrSpace__(pData, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS gpushareddataGetMemoryMappingDescriptor_IMPL(struct GpuUserSharedData *pData, struct MEMORY_DESCRIPTOR **ppMemDesc);

static inline NV_STATUS gpushareddataGetMemoryMappingDescriptor_DISPATCH(struct GpuUserSharedData *pData, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pData->__gpushareddataGetMemoryMappingDescriptor__(pData, ppMemDesc);
}

static inline NvBool gpushareddataShareCallback_DISPATCH(struct GpuUserSharedData *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__gpushareddataShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpushareddataControl_DISPATCH(struct GpuUserSharedData *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__gpushareddataControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataGetMemInterMapParams_DISPATCH(struct GpuUserSharedData *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__gpushareddataGetMemInterMapParams__(pRmResource, pParams);
}

static inline NvHandle gpushareddataGetInternalObjectHandle_DISPATCH(struct GpuUserSharedData *pGpuResource) {
    return pGpuResource->__gpushareddataGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS gpushareddataControlFilter_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpushareddataControlFilter__(pResource, pCallContext, pParams);
}

static inline void gpushareddataAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    pResource->__gpushareddataAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 gpushareddataGetRefCount_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__gpushareddataGetRefCount__(pResource);
}

static inline NV_STATUS gpushareddataCheckMemInterUnmap_DISPATCH(struct GpuUserSharedData *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__gpushareddataCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpushareddataMapTo_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gpushareddataMapTo__(pResource, pParams);
}

static inline NV_STATUS gpushareddataControl_Prologue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpushareddataControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataGetRegBaseOffsetAndSize_DISPATCH(struct GpuUserSharedData *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__gpushareddataGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NvBool gpushareddataCanCopy_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__gpushareddataCanCopy__(pResource);
}

static inline NV_STATUS gpushareddataInternalControlForward_DISPATCH(struct GpuUserSharedData *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__gpushareddataInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline void gpushareddataPreDestruct_DISPATCH(struct GpuUserSharedData *pResource) {
    pResource->__gpushareddataPreDestruct__(pResource);
}

static inline NV_STATUS gpushareddataUnmapFrom_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gpushareddataUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS gpushareddataIsDuplicate_DISPATCH(struct GpuUserSharedData *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__gpushareddataIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gpushareddataControl_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gpushareddataControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataControlLookup_DISPATCH(struct GpuUserSharedData *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gpushareddataControlLookup__(pResource, pParams, ppEntry);
}

static inline NvBool gpushareddataAccessCallback_DISPATCH(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gpushareddataAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS gpushareddataConstruct_IMPL(struct GpuUserSharedData *arg_pData, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gpushareddataConstruct(arg_pData, arg_pCallContext, arg_pParams) gpushareddataConstruct_IMPL(arg_pData, arg_pCallContext, arg_pParams)
void gpushareddataDestruct_IMPL(struct GpuUserSharedData *pData);

#define __nvoc_gpushareddataDestruct(pData) gpushareddataDestruct_IMPL(pData)
#undef PRIVATE_FIELD


#endif // GPU_USER_SHARED_DATA_H


#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_GPU_USER_SHARED_DATA_NVOC_H_
