
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Hdacodec;
struct NVOC_METADATA__GpuResource;


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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Hdacodec {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_Hdacodec
#define __nvoc_class_id_Hdacodec 0xf59a20u
typedef struct Hdacodec Hdacodec;
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

NV_STATUS __nvoc_objCreateDynamic_Hdacodec(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Hdacodec(Hdacodec**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_Hdacodec(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_Hdacodec((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS hdacodecConstruct_IMPL(struct Hdacodec *pHdacodecApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_hdacodecConstruct(pHdacodecApi, pCallContext, pParams) hdacodecConstruct_IMPL(pHdacodecApi, pCallContext, pParams)


// Wrapper macros for halified functions
#define hdacodecControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define hdacodecControl(pGpuResource, pCallContext, pParams) hdacodecControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define hdacodecMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define hdacodecMap(pGpuResource, pCallContext, pParams, pCpuMapping) hdacodecMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define hdacodecUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define hdacodecUnmap(pGpuResource, pCallContext, pCpuMapping) hdacodecUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define hdacodecShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define hdacodecShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) hdacodecShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define hdacodecGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define hdacodecGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) hdacodecGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define hdacodecGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define hdacodecGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) hdacodecGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define hdacodecInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define hdacodecInternalControlForward(pGpuResource, command, pParams, size) hdacodecInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define hdacodecGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define hdacodecGetInternalObjectHandle(pGpuResource) hdacodecGetInternalObjectHandle_DISPATCH(pGpuResource)
#define hdacodecAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define hdacodecAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) hdacodecAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define hdacodecGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define hdacodecGetMemInterMapParams(pRmResource, pParams) hdacodecGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define hdacodecCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define hdacodecCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) hdacodecCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define hdacodecGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define hdacodecGetMemoryMappingDescriptor(pRmResource, ppMemDesc) hdacodecGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define hdacodecControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define hdacodecControlSerialization_Prologue(pResource, pCallContext, pParams) hdacodecControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define hdacodecControlSerialization_Epilogue(pResource, pCallContext, pParams) hdacodecControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define hdacodecControl_Prologue(pResource, pCallContext, pParams) hdacodecControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hdacodecControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS hdacodecMap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS hdacodecUnmap_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool hdacodecShareCallback_DISPATCH(struct Hdacodec *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS hdacodecGetRegBaseOffsetAndSize_DISPATCH(struct Hdacodec *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS hdacodecGetMapAddrSpace_DISPATCH(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS hdacodecInternalControlForward_DISPATCH(struct Hdacodec *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle hdacodecGetInternalObjectHandle_DISPATCH(struct Hdacodec *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool hdacodecAccessCallback_DISPATCH(struct Hdacodec *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS hdacodecGetMemInterMapParams_DISPATCH(struct Hdacodec *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS hdacodecCheckMemInterUnmap_DISPATCH(struct Hdacodec *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS hdacodecGetMemoryMappingDescriptor_DISPATCH(struct Hdacodec *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS hdacodecControlSerialization_Prologue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void hdacodecControlSerialization_Epilogue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS hdacodecControl_Prologue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void hdacodecControl_Epilogue_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool hdacodecCanCopy_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS hdacodecIsDuplicate_DISPATCH(struct Hdacodec *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void hdacodecPreDestruct_DISPATCH(struct Hdacodec *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS hdacodecControlFilter_DISPATCH(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool hdacodecIsPartialUnmapSupported_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS hdacodecMapTo_DISPATCH(struct Hdacodec *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS hdacodecUnmapFrom_DISPATCH(struct Hdacodec *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 hdacodecGetRefCount_DISPATCH(struct Hdacodec *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void hdacodecAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Hdacodec *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HDA_CODEC_API_NVOC_H_
