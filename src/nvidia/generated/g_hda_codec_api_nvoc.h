#ifndef _G_HDA_CODEC_API_NVOC_H_
#define _G_HDA_CODEC_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_hda_codec_api_nvoc.h"

#ifndef HDA_CODEC_API_H
#define HDA_CODEC_API_H

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "ctrl/ctrl90ec.h"
#include "gpu/gpu_resource.h"

#ifdef NVOC_HDA_CODEC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Hdacodec {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct Hdacodec *__nvoc_pbase_Hdacodec;
    NvBool (*__hdacodecShareCallback__)(struct Hdacodec *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__hdacodecControl__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__hdacodecUnmap__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__hdacodecGetMemInterMapParams__)(struct Hdacodec *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__hdacodecGetMemoryMappingDescriptor__)(struct Hdacodec *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__hdacodecGetMapAddrSpace__)(struct Hdacodec *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvHandle (*__hdacodecGetInternalObjectHandle__)(struct Hdacodec *);
    NV_STATUS (*__hdacodecControlFilter__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__hdacodecAddAdditionalDependants__)(struct RsClient *, struct Hdacodec *, RsResourceRef *);
    NvU32 (*__hdacodecGetRefCount__)(struct Hdacodec *);
    NV_STATUS (*__hdacodecCheckMemInterUnmap__)(struct Hdacodec *, NvBool);
    NV_STATUS (*__hdacodecMapTo__)(struct Hdacodec *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__hdacodecControl_Prologue__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__hdacodecGetRegBaseOffsetAndSize__)(struct Hdacodec *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__hdacodecCanCopy__)(struct Hdacodec *);
    NV_STATUS (*__hdacodecInternalControlForward__)(struct Hdacodec *, NvU32, void *, NvU32);
    void (*__hdacodecPreDestruct__)(struct Hdacodec *);
    NV_STATUS (*__hdacodecUnmapFrom__)(struct Hdacodec *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__hdacodecControl_Epilogue__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__hdacodecControlLookup__)(struct Hdacodec *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__hdacodecMap__)(struct Hdacodec *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__hdacodecAccessCallback__)(struct Hdacodec *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_Hdacodec_TYPEDEF__
#define __NVOC_CLASS_Hdacodec_TYPEDEF__
typedef struct Hdacodec Hdacodec;
#endif /* __NVOC_CLASS_Hdacodec_TYPEDEF__ */

#ifndef __nvoc_class_id_Hdacodec
#define __nvoc_class_id_Hdacodec 0xf59a20
#endif /* __nvoc_class_id_Hdacodec */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Hdacodec;

#define __staticCast_Hdacodec(pThis) \
    ((pThis)->__nvoc_pbase_Hdacodec)

#ifdef __nvoc_hda_codec_api_h_disabled
#define __dynamicCast_Hdacodec(pThis) ((Hdacodec*)NULL)
#else //__nvoc_hda_codec_api_h_disabled
#define __dynamicCast_Hdacodec(pThis) \
    ((Hdacodec*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Hdacodec)))
#endif //__nvoc_hda_codec_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Hdacodec(Hdacodec**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Hdacodec(Hdacodec**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Hdacodec(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Hdacodec((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define hdacodecShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) hdacodecShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define hdacodecControl(pGpuResource, pCallContext, pParams) hdacodecControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define hdacodecUnmap(pGpuResource, pCallContext, pCpuMapping) hdacodecUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define hdacodecGetMemInterMapParams(pRmResource, pParams) hdacodecGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define hdacodecGetMemoryMappingDescriptor(pRmResource, ppMemDesc) hdacodecGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define hdacodecGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) hdacodecGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define hdacodecGetInternalObjectHandle(pGpuResource) hdacodecGetInternalObjectHandle_DISPATCH(pGpuResource)
#define hdacodecControlFilter(pResource, pCallContext, pParams) hdacodecControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecAddAdditionalDependants(pClient, pResource, pReference) hdacodecAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define hdacodecGetRefCount(pResource) hdacodecGetRefCount_DISPATCH(pResource)
#define hdacodecCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) hdacodecCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define hdacodecMapTo(pResource, pParams) hdacodecMapTo_DISPATCH(pResource, pParams)
#define hdacodecControl_Prologue(pResource, pCallContext, pParams) hdacodecControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) hdacodecGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define hdacodecCanCopy(pResource) hdacodecCanCopy_DISPATCH(pResource)
#define hdacodecInternalControlForward(pGpuResource, command, pParams, size) hdacodecInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define hdacodecPreDestruct(pResource) hdacodecPreDestruct_DISPATCH(pResource)
#define hdacodecUnmapFrom(pResource, pParams) hdacodecUnmapFrom_DISPATCH(pResource, pParams)
#define hdacodecControl_Epilogue(pResource, pCallContext, pParams) hdacodecControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControlLookup(pResource, pParams, ppEntry) hdacodecControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define hdacodecMap(pGpuResource, pCallContext, pParams, pCpuMapping) hdacodecMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define hdacodecAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) hdacodecAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool hdacodecShareCallback_DISPATCH(struct Hdacodec *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__hdacodecShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS hdacodecControl_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__hdacodecControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS hdacodecUnmap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__hdacodecUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS hdacodecGetMemInterMapParams_DISPATCH(struct Hdacodec *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__hdacodecGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS hdacodecGetMemoryMappingDescriptor_DISPATCH(struct Hdacodec *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__hdacodecGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS hdacodecGetMapAddrSpace_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__hdacodecGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle hdacodecGetInternalObjectHandle_DISPATCH(struct Hdacodec *pGpuResource) {
    return pGpuResource->__hdacodecGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS hdacodecControlFilter_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__hdacodecControlFilter__(pResource, pCallContext, pParams);
}

static inline void hdacodecAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Hdacodec *pResource, RsResourceRef *pReference) {
    pResource->__hdacodecAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 hdacodecGetRefCount_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__hdacodecGetRefCount__(pResource);
}

static inline NV_STATUS hdacodecCheckMemInterUnmap_DISPATCH(struct Hdacodec *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__hdacodecCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS hdacodecMapTo_DISPATCH(struct Hdacodec *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__hdacodecMapTo__(pResource, pParams);
}

static inline NV_STATUS hdacodecControl_Prologue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__hdacodecControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS hdacodecGetRegBaseOffsetAndSize_DISPATCH(struct Hdacodec *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__hdacodecGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NvBool hdacodecCanCopy_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__hdacodecCanCopy__(pResource);
}

static inline NV_STATUS hdacodecInternalControlForward_DISPATCH(struct Hdacodec *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__hdacodecInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline void hdacodecPreDestruct_DISPATCH(struct Hdacodec *pResource) {
    pResource->__hdacodecPreDestruct__(pResource);
}

static inline NV_STATUS hdacodecUnmapFrom_DISPATCH(struct Hdacodec *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__hdacodecUnmapFrom__(pResource, pParams);
}

static inline void hdacodecControl_Epilogue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__hdacodecControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS hdacodecControlLookup_DISPATCH(struct Hdacodec *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__hdacodecControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS hdacodecMap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__hdacodecMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool hdacodecAccessCallback_DISPATCH(struct Hdacodec *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__hdacodecAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS hdacodecConstruct_IMPL(struct Hdacodec *arg_pHdacodecApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_hdacodecConstruct(arg_pHdacodecApi, arg_pCallContext, arg_pParams) hdacodecConstruct_IMPL(arg_pHdacodecApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_HDA_CODEC_API_NVOC_H_
