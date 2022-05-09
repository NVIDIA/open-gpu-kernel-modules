#ifndef _G_DISP_CAPABILITIES_NVOC_H_
#define _G_DISP_CAPABILITIES_NVOC_H_
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

/******************************************************************************
*
*   Description:
*       This file contains functions managing DispCapabilities class.
*
******************************************************************************/

#include "g_disp_capabilities_nvoc.h"

#ifndef DISP_CAPABILITIES_H
#define DISP_CAPABILITIES_H

#include "gpu/gpu_resource.h"

/*!
 * RM internal class representing NVXXXX_DISP_CAPABILITIES
 */
#ifdef NVOC_DISP_CAPABILITIES_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispCapabilities {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct DispCapabilities *__nvoc_pbase_DispCapabilities;
    NV_STATUS (*__dispcapGetRegBaseOffsetAndSize__)(struct DispCapabilities *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__dispcapShareCallback__)(struct DispCapabilities *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispcapControl__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcapUnmap__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispcapGetMemInterMapParams__)(struct DispCapabilities *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispcapGetMemoryMappingDescriptor__)(struct DispCapabilities *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispcapGetMapAddrSpace__)(struct DispCapabilities *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvHandle (*__dispcapGetInternalObjectHandle__)(struct DispCapabilities *);
    NV_STATUS (*__dispcapControlFilter__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__dispcapAddAdditionalDependants__)(struct RsClient *, struct DispCapabilities *, RsResourceRef *);
    NvU32 (*__dispcapGetRefCount__)(struct DispCapabilities *);
    NV_STATUS (*__dispcapCheckMemInterUnmap__)(struct DispCapabilities *, NvBool);
    NV_STATUS (*__dispcapMapTo__)(struct DispCapabilities *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispcapControl_Prologue__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispcapCanCopy__)(struct DispCapabilities *);
    NV_STATUS (*__dispcapInternalControlForward__)(struct DispCapabilities *, NvU32, void *, NvU32);
    void (*__dispcapPreDestruct__)(struct DispCapabilities *);
    NV_STATUS (*__dispcapUnmapFrom__)(struct DispCapabilities *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispcapControl_Epilogue__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcapControlLookup__)(struct DispCapabilities *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispcapMap__)(struct DispCapabilities *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__dispcapAccessCallback__)(struct DispCapabilities *, struct RsClient *, void *, RsAccessRight);
    NvU32 ControlOffset;
    NvU32 ControlLength;
};

#ifndef __NVOC_CLASS_DispCapabilities_TYPEDEF__
#define __NVOC_CLASS_DispCapabilities_TYPEDEF__
typedef struct DispCapabilities DispCapabilities;
#endif /* __NVOC_CLASS_DispCapabilities_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCapabilities
#define __nvoc_class_id_DispCapabilities 0x99db3e
#endif /* __nvoc_class_id_DispCapabilities */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispCapabilities;

#define __staticCast_DispCapabilities(pThis) \
    ((pThis)->__nvoc_pbase_DispCapabilities)

#ifdef __nvoc_disp_capabilities_h_disabled
#define __dynamicCast_DispCapabilities(pThis) ((DispCapabilities*)NULL)
#else //__nvoc_disp_capabilities_h_disabled
#define __dynamicCast_DispCapabilities(pThis) \
    ((DispCapabilities*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispCapabilities)))
#endif //__nvoc_disp_capabilities_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DispCapabilities(DispCapabilities**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispCapabilities(DispCapabilities**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispCapabilities(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispCapabilities((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define dispcapGetRegBaseOffsetAndSize(pDispCapabilities, pGpu, pOffset, pSize) dispcapGetRegBaseOffsetAndSize_DISPATCH(pDispCapabilities, pGpu, pOffset, pSize)
#define dispcapShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispcapShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispcapControl(pGpuResource, pCallContext, pParams) dispcapControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispcapUnmap(pGpuResource, pCallContext, pCpuMapping) dispcapUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispcapGetMemInterMapParams(pRmResource, pParams) dispcapGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispcapGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispcapGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispcapGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispcapGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispcapGetInternalObjectHandle(pGpuResource) dispcapGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispcapControlFilter(pResource, pCallContext, pParams) dispcapControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispcapAddAdditionalDependants(pClient, pResource, pReference) dispcapAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispcapGetRefCount(pResource) dispcapGetRefCount_DISPATCH(pResource)
#define dispcapCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispcapCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispcapMapTo(pResource, pParams) dispcapMapTo_DISPATCH(pResource, pParams)
#define dispcapControl_Prologue(pResource, pCallContext, pParams) dispcapControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapCanCopy(pResource) dispcapCanCopy_DISPATCH(pResource)
#define dispcapInternalControlForward(pGpuResource, command, pParams, size) dispcapInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispcapPreDestruct(pResource) dispcapPreDestruct_DISPATCH(pResource)
#define dispcapUnmapFrom(pResource, pParams) dispcapUnmapFrom_DISPATCH(pResource, pParams)
#define dispcapControl_Epilogue(pResource, pCallContext, pParams) dispcapControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapControlLookup(pResource, pParams, ppEntry) dispcapControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispcapMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispcapMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispcapAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispcapAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS dispcapGetRegBaseOffsetAndSize_IMPL(struct DispCapabilities *pDispCapabilities, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS dispcapGetRegBaseOffsetAndSize_DISPATCH(struct DispCapabilities *pDispCapabilities, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispCapabilities->__dispcapGetRegBaseOffsetAndSize__(pDispCapabilities, pGpu, pOffset, pSize);
}

static inline NvBool dispcapShareCallback_DISPATCH(struct DispCapabilities *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispcapShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispcapControl_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispcapControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispcapUnmap_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispcapUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispcapGetMemInterMapParams_DISPATCH(struct DispCapabilities *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispcapGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispcapGetMemoryMappingDescriptor_DISPATCH(struct DispCapabilities *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispcapGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispcapGetMapAddrSpace_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispcapGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle dispcapGetInternalObjectHandle_DISPATCH(struct DispCapabilities *pGpuResource) {
    return pGpuResource->__dispcapGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispcapControlFilter_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcapControlFilter__(pResource, pCallContext, pParams);
}

static inline void dispcapAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispCapabilities *pResource, RsResourceRef *pReference) {
    pResource->__dispcapAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 dispcapGetRefCount_DISPATCH(struct DispCapabilities *pResource) {
    return pResource->__dispcapGetRefCount__(pResource);
}

static inline NV_STATUS dispcapCheckMemInterUnmap_DISPATCH(struct DispCapabilities *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispcapCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispcapMapTo_DISPATCH(struct DispCapabilities *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispcapMapTo__(pResource, pParams);
}

static inline NV_STATUS dispcapControl_Prologue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcapControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispcapCanCopy_DISPATCH(struct DispCapabilities *pResource) {
    return pResource->__dispcapCanCopy__(pResource);
}

static inline NV_STATUS dispcapInternalControlForward_DISPATCH(struct DispCapabilities *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispcapInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline void dispcapPreDestruct_DISPATCH(struct DispCapabilities *pResource) {
    pResource->__dispcapPreDestruct__(pResource);
}

static inline NV_STATUS dispcapUnmapFrom_DISPATCH(struct DispCapabilities *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispcapUnmapFrom__(pResource, pParams);
}

static inline void dispcapControl_Epilogue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispcapControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispcapControlLookup_DISPATCH(struct DispCapabilities *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispcapControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispcapMap_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispcapMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool dispcapAccessCallback_DISPATCH(struct DispCapabilities *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispcapAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dispcapConstruct_IMPL(struct DispCapabilities *arg_pDispCapabilities, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_dispcapConstruct(arg_pDispCapabilities, arg_pCallContext, arg_pParams) dispcapConstruct_IMPL(arg_pDispCapabilities, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_CAPABILITIES_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_DISP_CAPABILITIES_NVOC_H_
