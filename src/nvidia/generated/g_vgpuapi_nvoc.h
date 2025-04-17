
#ifndef _G_VGPUAPI_NVOC_H_
#define _G_VGPUAPI_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_vgpuapi_nvoc.h"

#ifndef _VGPUAPI_H_
#define _VGPUAPI_H_

#include "core/core.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "rmapi/control.h"
#include "ctrl/ctrla080.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VGPUAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VgpuApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__VgpuApi;


struct VgpuApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VgpuApi *__nvoc_metadata_ptr;
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
    struct VgpuApi *__nvoc_pbase_VgpuApi;    // vgpuapi

    // Vtable with 3 per-object function pointers
    NV_STATUS (*__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__)(struct VgpuApi * /*this*/, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *);  // exported (id=0xa0800103)
    NV_STATUS (*__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__)(struct VgpuApi * /*this*/, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *);  // exported (id=0xa0800104)
    NV_STATUS (*__vgpuapiCtrlCmdVGpuGetConfig__)(struct VgpuApi * /*this*/, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *);  // exported (id=0xa0800301)

    // Data members
    NODE node;
    NvHandle handle;
    NvHandle hDevice;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__VgpuApi {
    NV_STATUS (*__vgpuapiControl__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuapiMap__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuapiUnmap__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vgpuapiShareCallback__)(struct VgpuApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuapiGetRegBaseOffsetAndSize__)(struct VgpuApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuapiGetMapAddrSpace__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuapiInternalControlForward__)(struct VgpuApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__vgpuapiGetInternalObjectHandle__)(struct VgpuApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vgpuapiAccessCallback__)(struct VgpuApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuapiGetMemInterMapParams__)(struct VgpuApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuapiCheckMemInterUnmap__)(struct VgpuApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuapiGetMemoryMappingDescriptor__)(struct VgpuApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuapiControlSerialization_Prologue__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vgpuapiControlSerialization_Epilogue__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuapiControl_Prologue__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vgpuapiControl_Epilogue__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__vgpuapiCanCopy__)(struct VgpuApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuapiIsDuplicate__)(struct VgpuApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__vgpuapiPreDestruct__)(struct VgpuApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuapiControlFilter__)(struct VgpuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__vgpuapiIsPartialUnmapSupported__)(struct VgpuApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__vgpuapiMapTo__)(struct VgpuApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuapiUnmapFrom__)(struct VgpuApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__vgpuapiGetRefCount__)(struct VgpuApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__vgpuapiAddAdditionalDependants__)(struct RsClient *, struct VgpuApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VgpuApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__VgpuApi vtable;
};

#ifndef __NVOC_CLASS_VgpuApi_TYPEDEF__
#define __NVOC_CLASS_VgpuApi_TYPEDEF__
typedef struct VgpuApi VgpuApi;
#endif /* __NVOC_CLASS_VgpuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuApi
#define __nvoc_class_id_VgpuApi 0x7774f5
#endif /* __nvoc_class_id_VgpuApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuApi;

#define __staticCast_VgpuApi(pThis) \
    ((pThis)->__nvoc_pbase_VgpuApi)

#ifdef __nvoc_vgpuapi_h_disabled
#define __dynamicCast_VgpuApi(pThis) ((VgpuApi*) NULL)
#else //__nvoc_vgpuapi_h_disabled
#define __dynamicCast_VgpuApi(pThis) \
    ((VgpuApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VgpuApi)))
#endif //__nvoc_vgpuapi_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VgpuApi(VgpuApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VgpuApi(VgpuApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_VgpuApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VgpuApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_FNPTR(pVgpuApi) pVgpuApi->__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__
#define vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties(pVgpuApi, pParams) vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_DISPATCH(pVgpuApi, pParams)
#define vgpuapiCtrlCmdVgpuDisplayCleanupSurface_FNPTR(pVgpuApi) pVgpuApi->__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__
#define vgpuapiCtrlCmdVgpuDisplayCleanupSurface(pVgpuApi, pParams) vgpuapiCtrlCmdVgpuDisplayCleanupSurface_DISPATCH(pVgpuApi, pParams)
#define vgpuapiCtrlCmdVGpuGetConfig_FNPTR(pVgpuApi) pVgpuApi->__vgpuapiCtrlCmdVGpuGetConfig__
#define vgpuapiCtrlCmdVGpuGetConfig(pVgpuApi, pParams) vgpuapiCtrlCmdVGpuGetConfig_DISPATCH(pVgpuApi, pParams)
#define vgpuapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define vgpuapiControl(pGpuResource, pCallContext, pParams) vgpuapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vgpuapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define vgpuapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vgpuapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vgpuapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define vgpuapiUnmap(pGpuResource, pCallContext, pCpuMapping) vgpuapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vgpuapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define vgpuapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vgpuapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vgpuapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define vgpuapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vgpuapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vgpuapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define vgpuapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vgpuapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vgpuapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define vgpuapiInternalControlForward(pGpuResource, command, pParams, size) vgpuapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vgpuapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define vgpuapiGetInternalObjectHandle(pGpuResource) vgpuapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vgpuapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define vgpuapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vgpuapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vgpuapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vgpuapiGetMemInterMapParams(pRmResource, pParams) vgpuapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vgpuapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vgpuapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vgpuapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vgpuapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vgpuapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vgpuapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vgpuapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define vgpuapiControlSerialization_Prologue(pResource, pCallContext, pParams) vgpuapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define vgpuapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vgpuapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define vgpuapiControl_Prologue(pResource, pCallContext, pParams) vgpuapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define vgpuapiControl_Epilogue(pResource, pCallContext, pParams) vgpuapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define vgpuapiCanCopy(pResource) vgpuapiCanCopy_DISPATCH(pResource)
#define vgpuapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define vgpuapiIsDuplicate(pResource, hMemory, pDuplicate) vgpuapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vgpuapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define vgpuapiPreDestruct(pResource) vgpuapiPreDestruct_DISPATCH(pResource)
#define vgpuapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define vgpuapiControlFilter(pResource, pCallContext, pParams) vgpuapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define vgpuapiIsPartialUnmapSupported(pResource) vgpuapiIsPartialUnmapSupported_DISPATCH(pResource)
#define vgpuapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define vgpuapiMapTo(pResource, pParams) vgpuapiMapTo_DISPATCH(pResource, pParams)
#define vgpuapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define vgpuapiUnmapFrom(pResource, pParams) vgpuapiUnmapFrom_DISPATCH(pResource, pParams)
#define vgpuapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define vgpuapiGetRefCount(pResource) vgpuapiGetRefCount_DISPATCH(pResource)
#define vgpuapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define vgpuapiAddAdditionalDependants(pClient, pResource, pReference) vgpuapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__(pVgpuApi, pParams);
}

static inline NV_STATUS vgpuapiCtrlCmdVgpuDisplayCleanupSurface_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__(pVgpuApi, pParams);
}

static inline NV_STATUS vgpuapiCtrlCmdVGpuGetConfig_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVGpuGetConfig__(pVgpuApi, pParams);
}

static inline NV_STATUS vgpuapiControl_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiMap_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vgpuapiUnmap_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool vgpuapiShareCallback_DISPATCH(struct VgpuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vgpuapiGetRegBaseOffsetAndSize_DISPATCH(struct VgpuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vgpuapiGetMapAddrSpace_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vgpuapiInternalControlForward_DISPATCH(struct VgpuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle vgpuapiGetInternalObjectHandle_DISPATCH(struct VgpuApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool vgpuapiAccessCallback_DISPATCH(struct VgpuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vgpuapiGetMemInterMapParams_DISPATCH(struct VgpuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vgpuapiCheckMemInterUnmap_DISPATCH(struct VgpuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vgpuapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vgpuapiGetMemoryMappingDescriptor_DISPATCH(struct VgpuApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vgpuapiControlSerialization_Prologue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vgpuapiControlSerialization_Epilogue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__vgpuapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiControl_Prologue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vgpuapiControl_Epilogue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__vgpuapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuapiCanCopy_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiCanCopy__(pResource);
}

static inline NV_STATUS vgpuapiIsDuplicate_DISPATCH(struct VgpuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vgpuapiPreDestruct_DISPATCH(struct VgpuApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__vgpuapiPreDestruct__(pResource);
}

static inline NV_STATUS vgpuapiControlFilter_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuapiIsPartialUnmapSupported_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vgpuapiMapTo_DISPATCH(struct VgpuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vgpuapiUnmapFrom_DISPATCH(struct VgpuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 vgpuapiGetRefCount_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__vgpuapiGetRefCount__(pResource);
}

static inline void vgpuapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VgpuApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__vgpuapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams);

NV_STATUS vgpuapiCtrlCmdVgpuDisplayCleanupSurface_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams);

NV_STATUS vgpuapiCtrlCmdVGpuGetConfig_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams);

NV_STATUS vgpuapiConstruct_IMPL(struct VgpuApi *arg_pVgpuApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vgpuapiConstruct(arg_pVgpuApi, arg_pCallContext, arg_pParams) vgpuapiConstruct_IMPL(arg_pVgpuApi, arg_pCallContext, arg_pParams)
void vgpuapiDestruct_IMPL(struct VgpuApi *pVgpuApi);

#define __nvoc_vgpuapiDestruct(pVgpuApi) vgpuapiDestruct_IMPL(pVgpuApi)
#undef PRIVATE_FIELD


#endif // _VGPUAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VGPUAPI_NVOC_H_
