
#ifndef _G_HDA_CODEC_API_NVOC_H_
#define _G_HDA_CODEC_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_hda_codec_api_nvoc.h"

#ifndef HDA_CODEC_API_H
#define HDA_CODEC_API_H

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "ctrl/ctrl90ec.h"
#include "gpu/gpu_resource.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HDA_CODEC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Hdacodec;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__Hdacodec;


struct Hdacodec {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Hdacodec *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct Hdacodec *__nvoc_pbase_Hdacodec;    // hdacodec
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__Hdacodec {
    NV_STATUS (*__hdacodecControl__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__hdacodecMap__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__hdacodecUnmap__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__hdacodecShareCallback__)(struct Hdacodec * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__hdacodecGetRegBaseOffsetAndSize__)(struct Hdacodec * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__hdacodecGetMapAddrSpace__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__hdacodecInternalControlForward__)(struct Hdacodec * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__hdacodecGetInternalObjectHandle__)(struct Hdacodec * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__hdacodecAccessCallback__)(struct Hdacodec * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__hdacodecGetMemInterMapParams__)(struct Hdacodec * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__hdacodecCheckMemInterUnmap__)(struct Hdacodec * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__hdacodecGetMemoryMappingDescriptor__)(struct Hdacodec * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__hdacodecControlSerialization_Prologue__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__hdacodecControlSerialization_Epilogue__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__hdacodecControl_Prologue__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__hdacodecControl_Epilogue__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__hdacodecCanCopy__)(struct Hdacodec * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__hdacodecIsDuplicate__)(struct Hdacodec * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__hdacodecPreDestruct__)(struct Hdacodec * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__hdacodecControlFilter__)(struct Hdacodec * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__hdacodecIsPartialUnmapSupported__)(struct Hdacodec * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__hdacodecMapTo__)(struct Hdacodec * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__hdacodecUnmapFrom__)(struct Hdacodec * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__hdacodecGetRefCount__)(struct Hdacodec * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__hdacodecAddAdditionalDependants__)(struct RsClient *, struct Hdacodec * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Hdacodec {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__Hdacodec vtable;
};

#ifndef __NVOC_CLASS_Hdacodec_TYPEDEF__
#define __NVOC_CLASS_Hdacodec_TYPEDEF__
typedef struct Hdacodec Hdacodec;
#endif /* __NVOC_CLASS_Hdacodec_TYPEDEF__ */

#ifndef __nvoc_class_id_Hdacodec
#define __nvoc_class_id_Hdacodec 0xf59a20
#endif /* __nvoc_class_id_Hdacodec */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Hdacodec;

#define __staticCast_Hdacodec(pThis) \
    ((pThis)->__nvoc_pbase_Hdacodec)

#ifdef __nvoc_hda_codec_api_h_disabled
#define __dynamicCast_Hdacodec(pThis) ((Hdacodec*) NULL)
#else //__nvoc_hda_codec_api_h_disabled
#define __dynamicCast_Hdacodec(pThis) \
    ((Hdacodec*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Hdacodec)))
#endif //__nvoc_hda_codec_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Hdacodec(Hdacodec**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Hdacodec(Hdacodec**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_Hdacodec(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Hdacodec((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define hdacodecControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define hdacodecControl(pGpuResource, pCallContext, pParams) hdacodecControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define hdacodecMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define hdacodecMap(pGpuResource, pCallContext, pParams, pCpuMapping) hdacodecMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define hdacodecUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define hdacodecUnmap(pGpuResource, pCallContext, pCpuMapping) hdacodecUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define hdacodecShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define hdacodecShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) hdacodecShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define hdacodecGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define hdacodecGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) hdacodecGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define hdacodecGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define hdacodecGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) hdacodecGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define hdacodecInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define hdacodecInternalControlForward(pGpuResource, command, pParams, size) hdacodecInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define hdacodecGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define hdacodecGetInternalObjectHandle(pGpuResource) hdacodecGetInternalObjectHandle_DISPATCH(pGpuResource)
#define hdacodecAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define hdacodecAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) hdacodecAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define hdacodecGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define hdacodecGetMemInterMapParams(pRmResource, pParams) hdacodecGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define hdacodecCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define hdacodecCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) hdacodecCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define hdacodecGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define hdacodecGetMemoryMappingDescriptor(pRmResource, ppMemDesc) hdacodecGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define hdacodecControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define hdacodecControlSerialization_Prologue(pResource, pCallContext, pParams) hdacodecControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define hdacodecControlSerialization_Epilogue(pResource, pCallContext, pParams) hdacodecControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define hdacodecControl_Prologue(pResource, pCallContext, pParams) hdacodecControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define hdacodecControl_Epilogue(pResource, pCallContext, pParams) hdacodecControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define hdacodecCanCopy(pResource) hdacodecCanCopy_DISPATCH(pResource)
#define hdacodecIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define hdacodecIsDuplicate(pResource, hMemory, pDuplicate) hdacodecIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define hdacodecPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define hdacodecPreDestruct(pResource) hdacodecPreDestruct_DISPATCH(pResource)
#define hdacodecControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define hdacodecControlFilter(pResource, pCallContext, pParams) hdacodecControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define hdacodecIsPartialUnmapSupported(pResource) hdacodecIsPartialUnmapSupported_DISPATCH(pResource)
#define hdacodecMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define hdacodecMapTo(pResource, pParams) hdacodecMapTo_DISPATCH(pResource, pParams)
#define hdacodecUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define hdacodecUnmapFrom(pResource, pParams) hdacodecUnmapFrom_DISPATCH(pResource, pParams)
#define hdacodecGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define hdacodecGetRefCount(pResource) hdacodecGetRefCount_DISPATCH(pResource)
#define hdacodecAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define hdacodecAddAdditionalDependants(pClient, pResource, pReference) hdacodecAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS hdacodecControl_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS hdacodecMap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS hdacodecUnmap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool hdacodecShareCallback_DISPATCH(struct Hdacodec *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS hdacodecGetRegBaseOffsetAndSize_DISPATCH(struct Hdacodec *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS hdacodecGetMapAddrSpace_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS hdacodecInternalControlForward_DISPATCH(struct Hdacodec *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle hdacodecGetInternalObjectHandle_DISPATCH(struct Hdacodec *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__hdacodecGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool hdacodecAccessCallback_DISPATCH(struct Hdacodec *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS hdacodecGetMemInterMapParams_DISPATCH(struct Hdacodec *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__hdacodecGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS hdacodecCheckMemInterUnmap_DISPATCH(struct Hdacodec *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__hdacodecCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS hdacodecGetMemoryMappingDescriptor_DISPATCH(struct Hdacodec *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__hdacodecGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS hdacodecControlSerialization_Prologue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void hdacodecControlSerialization_Epilogue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__hdacodecControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS hdacodecControl_Prologue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void hdacodecControl_Epilogue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__hdacodecControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool hdacodecCanCopy_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecCanCopy__(pResource);
}

static inline NV_STATUS hdacodecIsDuplicate_DISPATCH(struct Hdacodec *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void hdacodecPreDestruct_DISPATCH(struct Hdacodec *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__hdacodecPreDestruct__(pResource);
}

static inline NV_STATUS hdacodecControlFilter_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool hdacodecIsPartialUnmapSupported_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS hdacodecMapTo_DISPATCH(struct Hdacodec *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecMapTo__(pResource, pParams);
}

static inline NV_STATUS hdacodecUnmapFrom_DISPATCH(struct Hdacodec *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecUnmapFrom__(pResource, pParams);
}

static inline NvU32 hdacodecGetRefCount_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__hdacodecGetRefCount__(pResource);
}

static inline void hdacodecAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Hdacodec *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__hdacodecAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS hdacodecConstruct_IMPL(struct Hdacodec *arg_pHdacodecApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_hdacodecConstruct(arg_pHdacodecApi, arg_pCallContext, arg_pParams) hdacodecConstruct_IMPL(arg_pHdacodecApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HDA_CODEC_API_NVOC_H_
