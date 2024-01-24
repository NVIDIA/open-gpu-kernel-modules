#ifndef _G_VGPUAPI_NVOC_H_
#define _G_VGPUAPI_NVOC_H_
#include "nvoc/runtime.h"

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

struct VgpuApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct VgpuApi *__nvoc_pbase_VgpuApi;
    NV_STATUS (*__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__)(struct VgpuApi *, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *);
    NV_STATUS (*__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__)(struct VgpuApi *, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *);
    NV_STATUS (*__vgpuapiCtrlCmdVGpuGetConfig__)(struct VgpuApi *, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *);
    NvBool (*__vgpuapiShareCallback__)(struct VgpuApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__vgpuapiCheckMemInterUnmap__)(struct VgpuApi *, NvBool);
    NV_STATUS (*__vgpuapiMapTo__)(struct VgpuApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__vgpuapiGetMapAddrSpace__)(struct VgpuApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__vgpuapiGetRefCount__)(struct VgpuApi *);
    void (*__vgpuapiAddAdditionalDependants__)(struct RsClient *, struct VgpuApi *, RsResourceRef *);
    NV_STATUS (*__vgpuapiControl_Prologue__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuapiGetRegBaseOffsetAndSize__)(struct VgpuApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__vgpuapiInternalControlForward__)(struct VgpuApi *, NvU32, void *, NvU32);
    NV_STATUS (*__vgpuapiUnmapFrom__)(struct VgpuApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__vgpuapiControl_Epilogue__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvHandle (*__vgpuapiGetInternalObjectHandle__)(struct VgpuApi *);
    NV_STATUS (*__vgpuapiControl__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuapiUnmap__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__vgpuapiGetMemInterMapParams__)(struct VgpuApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__vgpuapiGetMemoryMappingDescriptor__)(struct VgpuApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__vgpuapiControlFilter__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuapiControlSerialization_Prologue__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__vgpuapiCanCopy__)(struct VgpuApi *);
    NvBool (*__vgpuapiIsPartialUnmapSupported__)(struct VgpuApi *);
    void (*__vgpuapiPreDestruct__)(struct VgpuApi *);
    NV_STATUS (*__vgpuapiIsDuplicate__)(struct VgpuApi *, NvHandle, NvBool *);
    void (*__vgpuapiControlSerialization_Epilogue__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuapiMap__)(struct VgpuApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__vgpuapiAccessCallback__)(struct VgpuApi *, struct RsClient *, void *, RsAccessRight);
    NODE node;
    NvHandle handle;
    NvHandle hDevice;
};

#ifndef __NVOC_CLASS_VgpuApi_TYPEDEF__
#define __NVOC_CLASS_VgpuApi_TYPEDEF__
typedef struct VgpuApi VgpuApi;
#endif /* __NVOC_CLASS_VgpuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuApi
#define __nvoc_class_id_VgpuApi 0x7774f5
#endif /* __nvoc_class_id_VgpuApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuApi;

#define __staticCast_VgpuApi(pThis) \
    ((pThis)->__nvoc_pbase_VgpuApi)

#ifdef __nvoc_vgpuapi_h_disabled
#define __dynamicCast_VgpuApi(pThis) ((VgpuApi*)NULL)
#else //__nvoc_vgpuapi_h_disabled
#define __dynamicCast_VgpuApi(pThis) \
    ((VgpuApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VgpuApi)))
#endif //__nvoc_vgpuapi_h_disabled


NV_STATUS __nvoc_objCreateDynamic_VgpuApi(VgpuApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VgpuApi(VgpuApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VgpuApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VgpuApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties(pVgpuApi, pParams) vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_DISPATCH(pVgpuApi, pParams)
#define vgpuapiCtrlCmdVgpuDisplayCleanupSurface(pVgpuApi, pParams) vgpuapiCtrlCmdVgpuDisplayCleanupSurface_DISPATCH(pVgpuApi, pParams)
#define vgpuapiCtrlCmdVGpuGetConfig(pVgpuApi, pParams) vgpuapiCtrlCmdVGpuGetConfig_DISPATCH(pVgpuApi, pParams)
#define vgpuapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vgpuapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vgpuapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vgpuapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vgpuapiMapTo(pResource, pParams) vgpuapiMapTo_DISPATCH(pResource, pParams)
#define vgpuapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vgpuapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vgpuapiGetRefCount(pResource) vgpuapiGetRefCount_DISPATCH(pResource)
#define vgpuapiAddAdditionalDependants(pClient, pResource, pReference) vgpuapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vgpuapiControl_Prologue(pResource, pCallContext, pParams) vgpuapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vgpuapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vgpuapiInternalControlForward(pGpuResource, command, pParams, size) vgpuapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vgpuapiUnmapFrom(pResource, pParams) vgpuapiUnmapFrom_DISPATCH(pResource, pParams)
#define vgpuapiControl_Epilogue(pResource, pCallContext, pParams) vgpuapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiGetInternalObjectHandle(pGpuResource) vgpuapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vgpuapiControl(pGpuResource, pCallContext, pParams) vgpuapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vgpuapiUnmap(pGpuResource, pCallContext, pCpuMapping) vgpuapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vgpuapiGetMemInterMapParams(pRmResource, pParams) vgpuapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vgpuapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vgpuapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vgpuapiControlFilter(pResource, pCallContext, pParams) vgpuapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiControlSerialization_Prologue(pResource, pCallContext, pParams) vgpuapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiCanCopy(pResource) vgpuapiCanCopy_DISPATCH(pResource)
#define vgpuapiIsPartialUnmapSupported(pResource) vgpuapiIsPartialUnmapSupported_DISPATCH(pResource)
#define vgpuapiPreDestruct(pResource) vgpuapiPreDestruct_DISPATCH(pResource)
#define vgpuapiIsDuplicate(pResource, hMemory, pDuplicate) vgpuapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vgpuapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vgpuapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vgpuapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vgpuapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vgpuapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams);

static inline NV_STATUS vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__(pVgpuApi, pParams);
}

NV_STATUS vgpuapiCtrlCmdVgpuDisplayCleanupSurface_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams);

static inline NV_STATUS vgpuapiCtrlCmdVgpuDisplayCleanupSurface_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__(pVgpuApi, pParams);
}

NV_STATUS vgpuapiCtrlCmdVGpuGetConfig_IMPL(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams);

static inline NV_STATUS vgpuapiCtrlCmdVGpuGetConfig_DISPATCH(struct VgpuApi *pVgpuApi, NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *pParams) {
    return pVgpuApi->__vgpuapiCtrlCmdVGpuGetConfig__(pVgpuApi, pParams);
}

static inline NvBool vgpuapiShareCallback_DISPATCH(struct VgpuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vgpuapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vgpuapiCheckMemInterUnmap_DISPATCH(struct VgpuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vgpuapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vgpuapiMapTo_DISPATCH(struct VgpuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vgpuapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vgpuapiGetMapAddrSpace_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vgpuapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 vgpuapiGetRefCount_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__vgpuapiGetRefCount__(pResource);
}

static inline void vgpuapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VgpuApi *pResource, RsResourceRef *pReference) {
    pResource->__vgpuapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS vgpuapiControl_Prologue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiGetRegBaseOffsetAndSize_DISPATCH(struct VgpuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vgpuapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vgpuapiInternalControlForward_DISPATCH(struct VgpuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vgpuapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS vgpuapiUnmapFrom_DISPATCH(struct VgpuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vgpuapiUnmapFrom__(pResource, pParams);
}

static inline void vgpuapiControl_Epilogue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvHandle vgpuapiGetInternalObjectHandle_DISPATCH(struct VgpuApi *pGpuResource) {
    return pGpuResource->__vgpuapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS vgpuapiControl_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vgpuapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiUnmap_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS vgpuapiGetMemInterMapParams_DISPATCH(struct VgpuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vgpuapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vgpuapiGetMemoryMappingDescriptor_DISPATCH(struct VgpuApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vgpuapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vgpuapiControlFilter_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiControlSerialization_Prologue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuapiCanCopy_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__vgpuapiCanCopy__(pResource);
}

static inline NvBool vgpuapiIsPartialUnmapSupported_DISPATCH(struct VgpuApi *pResource) {
    return pResource->__vgpuapiIsPartialUnmapSupported__(pResource);
}

static inline void vgpuapiPreDestruct_DISPATCH(struct VgpuApi *pResource) {
    pResource->__vgpuapiPreDestruct__(pResource);
}

static inline NV_STATUS vgpuapiIsDuplicate_DISPATCH(struct VgpuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vgpuapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vgpuapiControlSerialization_Epilogue_DISPATCH(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuapiMap_DISPATCH(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool vgpuapiAccessCallback_DISPATCH(struct VgpuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vgpuapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
