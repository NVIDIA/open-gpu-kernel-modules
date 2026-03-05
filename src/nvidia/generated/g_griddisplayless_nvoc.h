
#ifndef _G_GRIDDISPLAYLESS_NVOC_H_
#define _G_GRIDDISPLAYLESS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_griddisplayless_nvoc.h"

#ifndef _GRIDDISPLAYLESS_H_
#define _GRIDDISPLAYLESS_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "class/cla083.h"
#include "ctrl/ctrla083.h"
#include "gpu/gpu_resource.h"
#include "rmapi/control.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GRIDDISPLAYLESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Griddisplayless;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__Griddisplayless;


struct Griddisplayless {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Griddisplayless *__nvoc_metadata_ptr;
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
    struct Griddisplayless *__nvoc_pbase_Griddisplayless;    // griddisplayless
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__Griddisplayless {
    NV_STATUS (*__griddisplaylessControl__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__griddisplaylessMap__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__griddisplaylessUnmap__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__griddisplaylessShareCallback__)(struct Griddisplayless * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__griddisplaylessGetRegBaseOffsetAndSize__)(struct Griddisplayless * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__griddisplaylessGetMapAddrSpace__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__griddisplaylessInternalControlForward__)(struct Griddisplayless * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__griddisplaylessGetInternalObjectHandle__)(struct Griddisplayless * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__griddisplaylessAccessCallback__)(struct Griddisplayless * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__griddisplaylessGetMemInterMapParams__)(struct Griddisplayless * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__griddisplaylessCheckMemInterUnmap__)(struct Griddisplayless * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__griddisplaylessGetMemoryMappingDescriptor__)(struct Griddisplayless * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__griddisplaylessControlSerialization_Prologue__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__griddisplaylessControlSerialization_Epilogue__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__griddisplaylessControl_Prologue__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__griddisplaylessControl_Epilogue__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__griddisplaylessCanCopy__)(struct Griddisplayless * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__griddisplaylessIsDuplicate__)(struct Griddisplayless * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__griddisplaylessPreDestruct__)(struct Griddisplayless * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__griddisplaylessControlFilter__)(struct Griddisplayless * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__griddisplaylessIsPartialUnmapSupported__)(struct Griddisplayless * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__griddisplaylessMapTo__)(struct Griddisplayless * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__griddisplaylessUnmapFrom__)(struct Griddisplayless * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__griddisplaylessGetRefCount__)(struct Griddisplayless * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__griddisplaylessAddAdditionalDependants__)(struct RsClient *, struct Griddisplayless * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Griddisplayless {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__Griddisplayless vtable;
};

#ifndef __nvoc_class_id_Griddisplayless
#define __nvoc_class_id_Griddisplayless 0x3d03b2u
typedef struct Griddisplayless Griddisplayless;
#endif /* __nvoc_class_id_Griddisplayless */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Griddisplayless;

#define __staticCast_Griddisplayless(pThis) \
    ((pThis)->__nvoc_pbase_Griddisplayless)

#ifdef __nvoc_griddisplayless_h_disabled
#define __dynamicCast_Griddisplayless(pThis) ((Griddisplayless*) NULL)
#else //__nvoc_griddisplayless_h_disabled
#define __dynamicCast_Griddisplayless(pThis) \
    ((Griddisplayless*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Griddisplayless)))
#endif //__nvoc_griddisplayless_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Griddisplayless(Griddisplayless**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Griddisplayless(Griddisplayless**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_Griddisplayless(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_Griddisplayless((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS griddisplaylessConstruct_IMPL(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_griddisplaylessConstruct(pResource, pCallContext, pParams) griddisplaylessConstruct_IMPL(pResource, pCallContext, pParams)

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetNumHeads_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *pParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetNumHeads(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayGetNumHeads(pGridDisplaylessApi, pParams) griddisplaylessCtrlCmdVirtualDisplayGetNumHeads_IMPL(pGridDisplaylessApi, pParams)
#endif // __nvoc_griddisplayless_h_disabled

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *pParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution(pGridDisplaylessApi, pParams) griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution_IMPL(pGridDisplaylessApi, pParams)
#endif // __nvoc_griddisplayless_h_disabled

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS *pEdidParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS *pEdidParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid(pGridDisplaylessApi, pEdidParams) griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid_IMPL(pGridDisplaylessApi, pEdidParams)
#endif // __nvoc_griddisplayless_h_disabled

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsActive_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS *pParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsActive(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayIsActive(pGridDisplaylessApi, pParams) griddisplaylessCtrlCmdVirtualDisplayIsActive_IMPL(pGridDisplaylessApi, pParams)
#endif // __nvoc_griddisplayless_h_disabled

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsConnected_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS *pParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsConnected(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayIsConnected(pGridDisplaylessApi, pParams) griddisplaylessCtrlCmdVirtualDisplayIsConnected_IMPL(pGridDisplaylessApi, pParams)
#endif // __nvoc_griddisplayless_h_disabled

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS *pParams);
#ifdef __nvoc_griddisplayless_h_disabled
static inline NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Griddisplayless was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_griddisplayless_h_disabled
#define griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels(pGridDisplaylessApi, pParams) griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels_IMPL(pGridDisplaylessApi, pParams)
#endif // __nvoc_griddisplayless_h_disabled


// Wrapper macros for halified functions
#define griddisplaylessControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define griddisplaylessControl(pGpuResource, pCallContext, pParams) griddisplaylessControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define griddisplaylessMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define griddisplaylessMap(pGpuResource, pCallContext, pParams, pCpuMapping) griddisplaylessMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define griddisplaylessUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define griddisplaylessUnmap(pGpuResource, pCallContext, pCpuMapping) griddisplaylessUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define griddisplaylessShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define griddisplaylessShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) griddisplaylessShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define griddisplaylessGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define griddisplaylessGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) griddisplaylessGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define griddisplaylessGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define griddisplaylessGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) griddisplaylessGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define griddisplaylessInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define griddisplaylessInternalControlForward(pGpuResource, command, pParams, size) griddisplaylessInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define griddisplaylessGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define griddisplaylessGetInternalObjectHandle(pGpuResource) griddisplaylessGetInternalObjectHandle_DISPATCH(pGpuResource)
#define griddisplaylessAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define griddisplaylessAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) griddisplaylessAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define griddisplaylessGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define griddisplaylessGetMemInterMapParams(pRmResource, pParams) griddisplaylessGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define griddisplaylessCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define griddisplaylessCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) griddisplaylessCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define griddisplaylessGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define griddisplaylessGetMemoryMappingDescriptor(pRmResource, ppMemDesc) griddisplaylessGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define griddisplaylessControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define griddisplaylessControlSerialization_Prologue(pResource, pCallContext, pParams) griddisplaylessControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define griddisplaylessControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define griddisplaylessControlSerialization_Epilogue(pResource, pCallContext, pParams) griddisplaylessControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define griddisplaylessControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define griddisplaylessControl_Prologue(pResource, pCallContext, pParams) griddisplaylessControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define griddisplaylessControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define griddisplaylessControl_Epilogue(pResource, pCallContext, pParams) griddisplaylessControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define griddisplaylessCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define griddisplaylessCanCopy(pResource) griddisplaylessCanCopy_DISPATCH(pResource)
#define griddisplaylessIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define griddisplaylessIsDuplicate(pResource, hMemory, pDuplicate) griddisplaylessIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define griddisplaylessPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define griddisplaylessPreDestruct(pResource) griddisplaylessPreDestruct_DISPATCH(pResource)
#define griddisplaylessControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define griddisplaylessControlFilter(pResource, pCallContext, pParams) griddisplaylessControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define griddisplaylessIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define griddisplaylessIsPartialUnmapSupported(pResource) griddisplaylessIsPartialUnmapSupported_DISPATCH(pResource)
#define griddisplaylessMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define griddisplaylessMapTo(pResource, pParams) griddisplaylessMapTo_DISPATCH(pResource, pParams)
#define griddisplaylessUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define griddisplaylessUnmapFrom(pResource, pParams) griddisplaylessUnmapFrom_DISPATCH(pResource, pParams)
#define griddisplaylessGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define griddisplaylessGetRefCount(pResource) griddisplaylessGetRefCount_DISPATCH(pResource)
#define griddisplaylessAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define griddisplaylessAddAdditionalDependants(pClient, pResource, pReference) griddisplaylessAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS griddisplaylessControl_DISPATCH(struct Griddisplayless *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS griddisplaylessMap_DISPATCH(struct Griddisplayless *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS griddisplaylessUnmap_DISPATCH(struct Griddisplayless *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool griddisplaylessShareCallback_DISPATCH(struct Griddisplayless *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS griddisplaylessGetRegBaseOffsetAndSize_DISPATCH(struct Griddisplayless *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS griddisplaylessGetMapAddrSpace_DISPATCH(struct Griddisplayless *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS griddisplaylessInternalControlForward_DISPATCH(struct Griddisplayless *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle griddisplaylessGetInternalObjectHandle_DISPATCH(struct Griddisplayless *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool griddisplaylessAccessCallback_DISPATCH(struct Griddisplayless *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS griddisplaylessGetMemInterMapParams_DISPATCH(struct Griddisplayless *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS griddisplaylessCheckMemInterUnmap_DISPATCH(struct Griddisplayless *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__griddisplaylessCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS griddisplaylessGetMemoryMappingDescriptor_DISPATCH(struct Griddisplayless *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS griddisplaylessControlSerialization_Prologue_DISPATCH(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void griddisplaylessControlSerialization_Epilogue_DISPATCH(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS griddisplaylessControl_Prologue_DISPATCH(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void griddisplaylessControl_Epilogue_DISPATCH(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool griddisplaylessCanCopy_DISPATCH(struct Griddisplayless *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessCanCopy__(pResource);
}

static inline NV_STATUS griddisplaylessIsDuplicate_DISPATCH(struct Griddisplayless *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void griddisplaylessPreDestruct_DISPATCH(struct Griddisplayless *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessPreDestruct__(pResource);
}

static inline NV_STATUS griddisplaylessControlFilter_DISPATCH(struct Griddisplayless *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool griddisplaylessIsPartialUnmapSupported_DISPATCH(struct Griddisplayless *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS griddisplaylessMapTo_DISPATCH(struct Griddisplayless *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessMapTo__(pResource, pParams);
}

static inline NV_STATUS griddisplaylessUnmapFrom_DISPATCH(struct Griddisplayless *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessUnmapFrom__(pResource, pParams);
}

static inline NvU32 griddisplaylessGetRefCount_DISPATCH(struct Griddisplayless *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessGetRefCount__(pResource);
}

static inline void griddisplaylessAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Griddisplayless *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__griddisplaylessAddAdditionalDependants__(pClient, pResource, pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetNumHeads_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *pParams);

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *pParams);

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS *pEdidParams);

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsActive_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS *pParams);

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayIsConnected_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS *pParams);

NV_STATUS griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels_IMPL(struct Griddisplayless *pGridDisplaylessApi, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GRIDDISPLAYLESS_NVOC_H_
