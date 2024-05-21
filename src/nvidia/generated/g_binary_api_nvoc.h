
#ifndef _G_BINARY_API_NVOC_H_
#define _G_BINARY_API_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_binary_api_nvoc.h"

#ifndef BINARY_API_H
#define BINARY_API_H

#include "core/core.h"
#include "rmapi/resource.h"
#include "gpu/gpu_resource.h"
#include "resserv/rs_resource.h"
#include "rmapi/control.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct BinaryApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct BinaryApi *__nvoc_pbase_BinaryApi;    // binapi

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__binapiControl__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__binapiMap__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__binapiUnmap__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__binapiShareCallback__)(struct BinaryApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__binapiGetRegBaseOffsetAndSize__)(struct BinaryApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__binapiGetMapAddrSpace__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__binapiInternalControlForward__)(struct BinaryApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__binapiGetInternalObjectHandle__)(struct BinaryApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__binapiAccessCallback__)(struct BinaryApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__binapiGetMemInterMapParams__)(struct BinaryApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__binapiCheckMemInterUnmap__)(struct BinaryApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__binapiGetMemoryMappingDescriptor__)(struct BinaryApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__binapiControlSerialization_Prologue__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__binapiControlSerialization_Epilogue__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__binapiControl_Prologue__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__binapiControl_Epilogue__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__binapiCanCopy__)(struct BinaryApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__binapiIsDuplicate__)(struct BinaryApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__binapiPreDestruct__)(struct BinaryApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__binapiControlFilter__)(struct BinaryApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__binapiIsPartialUnmapSupported__)(struct BinaryApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__binapiMapTo__)(struct BinaryApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__binapiUnmapFrom__)(struct BinaryApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__binapiGetRefCount__)(struct BinaryApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__binapiAddAdditionalDependants__)(struct RsClient *, struct BinaryApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

#ifndef __NVOC_CLASS_BinaryApi_TYPEDEF__
#define __NVOC_CLASS_BinaryApi_TYPEDEF__
typedef struct BinaryApi BinaryApi;
#endif /* __NVOC_CLASS_BinaryApi_TYPEDEF__ */

#ifndef __nvoc_class_id_BinaryApi
#define __nvoc_class_id_BinaryApi 0xb7a47c
#endif /* __nvoc_class_id_BinaryApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;

#define __staticCast_BinaryApi(pThis) \
    ((pThis)->__nvoc_pbase_BinaryApi)

#ifdef __nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApi(pThis) ((BinaryApi*)NULL)
#else //__nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApi(pThis) \
    ((BinaryApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(BinaryApi)))
#endif //__nvoc_binary_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_BinaryApi(BinaryApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_BinaryApi(BinaryApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_BinaryApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_BinaryApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define binapiControl_FNPTR(pResource) pResource->__binapiControl__
#define binapiControl(pResource, pCallContext, pParams) binapiControl_DISPATCH(pResource, pCallContext, pParams)
#define binapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define binapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) binapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define binapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define binapiUnmap(pGpuResource, pCallContext, pCpuMapping) binapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define binapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define binapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) binapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define binapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define binapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) binapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define binapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define binapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) binapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define binapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define binapiInternalControlForward(pGpuResource, command, pParams, size) binapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define binapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define binapiGetInternalObjectHandle(pGpuResource) binapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define binapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define binapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) binapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define binapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define binapiGetMemInterMapParams(pRmResource, pParams) binapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define binapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define binapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) binapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define binapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define binapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) binapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define binapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define binapiControlSerialization_Prologue(pResource, pCallContext, pParams) binapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define binapiControlSerialization_Epilogue(pResource, pCallContext, pParams) binapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define binapiControl_Prologue(pResource, pCallContext, pParams) binapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define binapiControl_Epilogue(pResource, pCallContext, pParams) binapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define binapiCanCopy(pResource) binapiCanCopy_DISPATCH(pResource)
#define binapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define binapiIsDuplicate(pResource, hMemory, pDuplicate) binapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define binapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define binapiPreDestruct(pResource) binapiPreDestruct_DISPATCH(pResource)
#define binapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define binapiControlFilter(pResource, pCallContext, pParams) binapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define binapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define binapiIsPartialUnmapSupported(pResource) binapiIsPartialUnmapSupported_DISPATCH(pResource)
#define binapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define binapiMapTo(pResource, pParams) binapiMapTo_DISPATCH(pResource, pParams)
#define binapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define binapiUnmapFrom(pResource, pParams) binapiUnmapFrom_DISPATCH(pResource, pParams)
#define binapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define binapiGetRefCount(pResource) binapiGetRefCount_DISPATCH(pResource)
#define binapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define binapiAddAdditionalDependants(pClient, pResource, pReference) binapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS binapiControl_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS binapiMap_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__binapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS binapiUnmap_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__binapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool binapiShareCallback_DISPATCH(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__binapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS binapiGetRegBaseOffsetAndSize_DISPATCH(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__binapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS binapiGetMapAddrSpace_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__binapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS binapiInternalControlForward_DISPATCH(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__binapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle binapiGetInternalObjectHandle_DISPATCH(struct BinaryApi *pGpuResource) {
    return pGpuResource->__binapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool binapiAccessCallback_DISPATCH(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__binapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS binapiGetMemInterMapParams_DISPATCH(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__binapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS binapiCheckMemInterUnmap_DISPATCH(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__binapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS binapiGetMemoryMappingDescriptor_DISPATCH(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__binapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS binapiControlSerialization_Prologue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void binapiControlSerialization_Epilogue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__binapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS binapiControl_Prologue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void binapiControl_Epilogue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__binapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool binapiCanCopy_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__binapiCanCopy__(pResource);
}

static inline NV_STATUS binapiIsDuplicate_DISPATCH(struct BinaryApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__binapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void binapiPreDestruct_DISPATCH(struct BinaryApi *pResource) {
    pResource->__binapiPreDestruct__(pResource);
}

static inline NV_STATUS binapiControlFilter_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool binapiIsPartialUnmapSupported_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__binapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS binapiMapTo_DISPATCH(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__binapiMapTo__(pResource, pParams);
}

static inline NV_STATUS binapiUnmapFrom_DISPATCH(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__binapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 binapiGetRefCount_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__binapiGetRefCount__(pResource);
}

static inline void binapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference) {
    pResource->__binapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS binapiControl_IMPL(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS binapiConstruct_IMPL(struct BinaryApi *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_binapiConstruct(arg_pResource, arg_pCallContext, arg_pParams) binapiConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct BinaryApiPrivileged {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct BinaryApi __nvoc_base_BinaryApi;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct BinaryApi *__nvoc_pbase_BinaryApi;    // binapi super
    struct BinaryApiPrivileged *__nvoc_pbase_BinaryApiPrivileged;    // binapipriv

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__binapiprivControl__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (binapi)
    NV_STATUS (*__binapiprivMap__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (binapi)
    NV_STATUS (*__binapiprivUnmap__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (binapi)
    NvBool (*__binapiprivShareCallback__)(struct BinaryApiPrivileged * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (binapi)
    NV_STATUS (*__binapiprivGetRegBaseOffsetAndSize__)(struct BinaryApiPrivileged * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (binapi)
    NV_STATUS (*__binapiprivGetMapAddrSpace__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (binapi)
    NV_STATUS (*__binapiprivInternalControlForward__)(struct BinaryApiPrivileged * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (binapi)
    NvHandle (*__binapiprivGetInternalObjectHandle__)(struct BinaryApiPrivileged * /*this*/);  // virtual inherited (gpures) base (binapi)
    NvBool (*__binapiprivAccessCallback__)(struct BinaryApiPrivileged * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (binapi)
    NV_STATUS (*__binapiprivGetMemInterMapParams__)(struct BinaryApiPrivileged * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (binapi)
    NV_STATUS (*__binapiprivCheckMemInterUnmap__)(struct BinaryApiPrivileged * /*this*/, NvBool);  // virtual inherited (rmres) base (binapi)
    NV_STATUS (*__binapiprivGetMemoryMappingDescriptor__)(struct BinaryApiPrivileged * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (binapi)
    NV_STATUS (*__binapiprivControlSerialization_Prologue__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (binapi)
    void (*__binapiprivControlSerialization_Epilogue__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (binapi)
    NV_STATUS (*__binapiprivControl_Prologue__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (binapi)
    void (*__binapiprivControl_Epilogue__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (binapi)
    NvBool (*__binapiprivCanCopy__)(struct BinaryApiPrivileged * /*this*/);  // virtual inherited (res) base (binapi)
    NV_STATUS (*__binapiprivIsDuplicate__)(struct BinaryApiPrivileged * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (binapi)
    void (*__binapiprivPreDestruct__)(struct BinaryApiPrivileged * /*this*/);  // virtual inherited (res) base (binapi)
    NV_STATUS (*__binapiprivControlFilter__)(struct BinaryApiPrivileged * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (binapi)
    NvBool (*__binapiprivIsPartialUnmapSupported__)(struct BinaryApiPrivileged * /*this*/);  // inline virtual inherited (res) base (binapi) body
    NV_STATUS (*__binapiprivMapTo__)(struct BinaryApiPrivileged * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (binapi)
    NV_STATUS (*__binapiprivUnmapFrom__)(struct BinaryApiPrivileged * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (binapi)
    NvU32 (*__binapiprivGetRefCount__)(struct BinaryApiPrivileged * /*this*/);  // virtual inherited (res) base (binapi)
    void (*__binapiprivAddAdditionalDependants__)(struct RsClient *, struct BinaryApiPrivileged * /*this*/, RsResourceRef *);  // virtual inherited (res) base (binapi)
};

#ifndef __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__
#define __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__
typedef struct BinaryApiPrivileged BinaryApiPrivileged;
#endif /* __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__ */

#ifndef __nvoc_class_id_BinaryApiPrivileged
#define __nvoc_class_id_BinaryApiPrivileged 0x1c0579
#endif /* __nvoc_class_id_BinaryApiPrivileged */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged;

#define __staticCast_BinaryApiPrivileged(pThis) \
    ((pThis)->__nvoc_pbase_BinaryApiPrivileged)

#ifdef __nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApiPrivileged(pThis) ((BinaryApiPrivileged*)NULL)
#else //__nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApiPrivileged(pThis) \
    ((BinaryApiPrivileged*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(BinaryApiPrivileged)))
#endif //__nvoc_binary_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_BinaryApiPrivileged(BinaryApiPrivileged**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_BinaryApiPrivileged(BinaryApiPrivileged**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_BinaryApiPrivileged(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_BinaryApiPrivileged((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define binapiprivControl_FNPTR(pResource) pResource->__binapiprivControl__
#define binapiprivControl(pResource, pCallContext, pParams) binapiprivControl_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresMap__
#define binapiprivMap(pGpuResource, pCallContext, pParams, pCpuMapping) binapiprivMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define binapiprivUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresUnmap__
#define binapiprivUnmap(pGpuResource, pCallContext, pCpuMapping) binapiprivUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define binapiprivShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresShareCallback__
#define binapiprivShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) binapiprivShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define binapiprivGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define binapiprivGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) binapiprivGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define binapiprivGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define binapiprivGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) binapiprivGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define binapiprivInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define binapiprivInternalControlForward(pGpuResource, command, pParams, size) binapiprivInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define binapiprivGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define binapiprivGetInternalObjectHandle(pGpuResource) binapiprivGetInternalObjectHandle_DISPATCH(pGpuResource)
#define binapiprivAccessCallback_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define binapiprivAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) binapiprivAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define binapiprivGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define binapiprivGetMemInterMapParams(pRmResource, pParams) binapiprivGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define binapiprivCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define binapiprivCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) binapiprivCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define binapiprivGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define binapiprivGetMemoryMappingDescriptor(pRmResource, ppMemDesc) binapiprivGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define binapiprivControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define binapiprivControlSerialization_Prologue(pResource, pCallContext, pParams) binapiprivControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define binapiprivControlSerialization_Epilogue(pResource, pCallContext, pParams) binapiprivControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define binapiprivControl_Prologue(pResource, pCallContext, pParams) binapiprivControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define binapiprivControl_Epilogue(pResource, pCallContext, pParams) binapiprivControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivCanCopy_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define binapiprivCanCopy(pResource) binapiprivCanCopy_DISPATCH(pResource)
#define binapiprivIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define binapiprivIsDuplicate(pResource, hMemory, pDuplicate) binapiprivIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define binapiprivPreDestruct_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define binapiprivPreDestruct(pResource) binapiprivPreDestruct_DISPATCH(pResource)
#define binapiprivControlFilter_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define binapiprivControlFilter(pResource, pCallContext, pParams) binapiprivControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define binapiprivIsPartialUnmapSupported(pResource) binapiprivIsPartialUnmapSupported_DISPATCH(pResource)
#define binapiprivMapTo_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define binapiprivMapTo(pResource, pParams) binapiprivMapTo_DISPATCH(pResource, pParams)
#define binapiprivUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define binapiprivUnmapFrom(pResource, pParams) binapiprivUnmapFrom_DISPATCH(pResource, pParams)
#define binapiprivGetRefCount_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define binapiprivGetRefCount(pResource) binapiprivGetRefCount_DISPATCH(pResource)
#define binapiprivAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define binapiprivAddAdditionalDependants(pClient, pResource, pReference) binapiprivAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS binapiprivControl_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiprivControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS binapiprivMap_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__binapiprivMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS binapiprivUnmap_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__binapiprivUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool binapiprivShareCallback_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__binapiprivShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS binapiprivGetRegBaseOffsetAndSize_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__binapiprivGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS binapiprivGetMapAddrSpace_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__binapiprivGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS binapiprivInternalControlForward_DISPATCH(struct BinaryApiPrivileged *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__binapiprivInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle binapiprivGetInternalObjectHandle_DISPATCH(struct BinaryApiPrivileged *pGpuResource) {
    return pGpuResource->__binapiprivGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool binapiprivAccessCallback_DISPATCH(struct BinaryApiPrivileged *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__binapiprivAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS binapiprivGetMemInterMapParams_DISPATCH(struct BinaryApiPrivileged *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__binapiprivGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS binapiprivCheckMemInterUnmap_DISPATCH(struct BinaryApiPrivileged *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__binapiprivCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS binapiprivGetMemoryMappingDescriptor_DISPATCH(struct BinaryApiPrivileged *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__binapiprivGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS binapiprivControlSerialization_Prologue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiprivControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void binapiprivControlSerialization_Epilogue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__binapiprivControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS binapiprivControl_Prologue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiprivControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void binapiprivControl_Epilogue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__binapiprivControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool binapiprivCanCopy_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__binapiprivCanCopy__(pResource);
}

static inline NV_STATUS binapiprivIsDuplicate_DISPATCH(struct BinaryApiPrivileged *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__binapiprivIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void binapiprivPreDestruct_DISPATCH(struct BinaryApiPrivileged *pResource) {
    pResource->__binapiprivPreDestruct__(pResource);
}

static inline NV_STATUS binapiprivControlFilter_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__binapiprivControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool binapiprivIsPartialUnmapSupported_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__binapiprivIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS binapiprivMapTo_DISPATCH(struct BinaryApiPrivileged *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__binapiprivMapTo__(pResource, pParams);
}

static inline NV_STATUS binapiprivUnmapFrom_DISPATCH(struct BinaryApiPrivileged *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__binapiprivUnmapFrom__(pResource, pParams);
}

static inline NvU32 binapiprivGetRefCount_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__binapiprivGetRefCount__(pResource);
}

static inline void binapiprivAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct BinaryApiPrivileged *pResource, RsResourceRef *pReference) {
    pResource->__binapiprivAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS binapiprivControl_IMPL(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS binapiprivConstruct_IMPL(struct BinaryApiPrivileged *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_binapiprivConstruct(arg_pResource, arg_pCallContext, arg_pParams) binapiprivConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_BINARY_API_NVOC_H_
