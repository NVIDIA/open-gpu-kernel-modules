
#ifndef _G_DISP_CAPABILITIES_NVOC_H_
#define _G_DISP_CAPABILITIES_NVOC_H_
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

#pragma once
#include "g_disp_capabilities_nvoc.h"

#ifndef DISP_CAPABILITIES_H
#define DISP_CAPABILITIES_H

#include "gpu/gpu_resource.h"

/*!
 * RM internal class representing NVXXXX_DISP_CAPABILITIES
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CAPABILITIES_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispCapabilities {

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
    struct DispCapabilities *__nvoc_pbase_DispCapabilities;    // dispcap

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__dispcapGetRegBaseOffsetAndSize__)(struct DispCapabilities * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__dispcapControl__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispcapMap__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispcapUnmap__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispcapShareCallback__)(struct DispCapabilities * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispcapGetMapAddrSpace__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispcapInternalControlForward__)(struct DispCapabilities * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__dispcapGetInternalObjectHandle__)(struct DispCapabilities * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispcapAccessCallback__)(struct DispCapabilities * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispcapGetMemInterMapParams__)(struct DispCapabilities * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispcapCheckMemInterUnmap__)(struct DispCapabilities * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispcapGetMemoryMappingDescriptor__)(struct DispCapabilities * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispcapControlSerialization_Prologue__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispcapControlSerialization_Epilogue__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispcapControl_Prologue__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispcapControl_Epilogue__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__dispcapCanCopy__)(struct DispCapabilities * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispcapIsDuplicate__)(struct DispCapabilities * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__dispcapPreDestruct__)(struct DispCapabilities * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispcapControlFilter__)(struct DispCapabilities * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__dispcapIsPartialUnmapSupported__)(struct DispCapabilities * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__dispcapMapTo__)(struct DispCapabilities * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispcapUnmapFrom__)(struct DispCapabilities * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__dispcapGetRefCount__)(struct DispCapabilities * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__dispcapAddAdditionalDependants__)(struct RsClient *, struct DispCapabilities * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
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

// Casting support
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


// Wrapper macros
#define dispcapGetRegBaseOffsetAndSize_FNPTR(pDispCapabilities) pDispCapabilities->__dispcapGetRegBaseOffsetAndSize__
#define dispcapGetRegBaseOffsetAndSize(pDispCapabilities, pGpu, pOffset, pSize) dispcapGetRegBaseOffsetAndSize_DISPATCH(pDispCapabilities, pGpu, pOffset, pSize)
#define dispcapControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define dispcapControl(pGpuResource, pCallContext, pParams) dispcapControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispcapMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define dispcapMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispcapMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispcapUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define dispcapUnmap(pGpuResource, pCallContext, pCpuMapping) dispcapUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispcapShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispcapShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispcapShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispcapGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispcapGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispcapGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispcapInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispcapInternalControlForward(pGpuResource, command, pParams, size) dispcapInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispcapGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispcapGetInternalObjectHandle(pGpuResource) dispcapGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispcapAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispcapAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispcapAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispcapGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispcapGetMemInterMapParams(pRmResource, pParams) dispcapGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispcapCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispcapCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispcapCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispcapGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispcapGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispcapGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispcapControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispcapControlSerialization_Prologue(pResource, pCallContext, pParams) dispcapControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispcapControlSerialization_Epilogue(pResource, pCallContext, pParams) dispcapControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispcapControl_Prologue(pResource, pCallContext, pParams) dispcapControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispcapControl_Epilogue(pResource, pCallContext, pParams) dispcapControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispcapCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispcapCanCopy(pResource) dispcapCanCopy_DISPATCH(pResource)
#define dispcapIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispcapIsDuplicate(pResource, hMemory, pDuplicate) dispcapIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispcapPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispcapPreDestruct(pResource) dispcapPreDestruct_DISPATCH(pResource)
#define dispcapControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispcapControlFilter(pResource, pCallContext, pParams) dispcapControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispcapIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispcapIsPartialUnmapSupported(pResource) dispcapIsPartialUnmapSupported_DISPATCH(pResource)
#define dispcapMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispcapMapTo(pResource, pParams) dispcapMapTo_DISPATCH(pResource, pParams)
#define dispcapUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispcapUnmapFrom(pResource, pParams) dispcapUnmapFrom_DISPATCH(pResource, pParams)
#define dispcapGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispcapGetRefCount(pResource) dispcapGetRefCount_DISPATCH(pResource)
#define dispcapAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispcapAddAdditionalDependants(pClient, pResource, pReference) dispcapAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS dispcapGetRegBaseOffsetAndSize_DISPATCH(struct DispCapabilities *pDispCapabilities, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispCapabilities->__dispcapGetRegBaseOffsetAndSize__(pDispCapabilities, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispcapControl_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispcapControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispcapMap_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispcapMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispcapUnmap_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispcapUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispcapShareCallback_DISPATCH(struct DispCapabilities *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispcapShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispcapGetMapAddrSpace_DISPATCH(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispcapGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispcapInternalControlForward_DISPATCH(struct DispCapabilities *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispcapInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispcapGetInternalObjectHandle_DISPATCH(struct DispCapabilities *pGpuResource) {
    return pGpuResource->__dispcapGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispcapAccessCallback_DISPATCH(struct DispCapabilities *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispcapAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispcapGetMemInterMapParams_DISPATCH(struct DispCapabilities *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispcapGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispcapCheckMemInterUnmap_DISPATCH(struct DispCapabilities *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispcapCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispcapGetMemoryMappingDescriptor_DISPATCH(struct DispCapabilities *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispcapGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispcapControlSerialization_Prologue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcapControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispcapControlSerialization_Epilogue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispcapControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispcapControl_Prologue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcapControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispcapControl_Epilogue_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispcapControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispcapCanCopy_DISPATCH(struct DispCapabilities *pResource) {
    return pResource->__dispcapCanCopy__(pResource);
}

static inline NV_STATUS dispcapIsDuplicate_DISPATCH(struct DispCapabilities *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispcapIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispcapPreDestruct_DISPATCH(struct DispCapabilities *pResource) {
    pResource->__dispcapPreDestruct__(pResource);
}

static inline NV_STATUS dispcapControlFilter_DISPATCH(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcapControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispcapIsPartialUnmapSupported_DISPATCH(struct DispCapabilities *pResource) {
    return pResource->__dispcapIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispcapMapTo_DISPATCH(struct DispCapabilities *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispcapMapTo__(pResource, pParams);
}

static inline NV_STATUS dispcapUnmapFrom_DISPATCH(struct DispCapabilities *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispcapUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispcapGetRefCount_DISPATCH(struct DispCapabilities *pResource) {
    return pResource->__dispcapGetRefCount__(pResource);
}

static inline void dispcapAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispCapabilities *pResource, RsResourceRef *pReference) {
    pResource->__dispcapAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS dispcapGetRegBaseOffsetAndSize_IMPL(struct DispCapabilities *pDispCapabilities, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

NV_STATUS dispcapConstruct_IMPL(struct DispCapabilities *arg_pDispCapabilities, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispcapConstruct(arg_pDispCapabilities, arg_pCallContext, arg_pParams) dispcapConstruct_IMPL(arg_pDispCapabilities, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_CAPABILITIES_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_CAPABILITIES_NVOC_H_
