
#ifndef _G_RG_LINE_CALLBACK_NVOC_H_
#define _G_RG_LINE_CALLBACK_NVOC_H_
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

#pragma once
#include "g_rg_line_callback_nvoc.h"

#ifndef RG_LINE_CALLBACK_H
#define RG_LINE_CALLBACK_H

#include "class/cl0092.h"
#include "gpu/gpu_resource.h"


struct DispCommon;

#ifndef __NVOC_CLASS_DispCommon_TYPEDEF__
#define __NVOC_CLASS_DispCommon_TYPEDEF__
typedef struct DispCommon DispCommon;
#endif /* __NVOC_CLASS_DispCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCommon
#define __nvoc_class_id_DispCommon 0x41f4f2
#endif /* __nvoc_class_id_DispCommon */



/*!
 * RM internal class representing NV0092_RG_LINE_CALLBACK
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RG_LINE_CALLBACK_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RgLineCallback {

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
    struct RgLineCallback *__nvoc_pbase_RgLineCallback;    // rglcb

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__rglcbControl__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__rglcbMap__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__rglcbUnmap__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__rglcbShareCallback__)(struct RgLineCallback * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__rglcbGetRegBaseOffsetAndSize__)(struct RgLineCallback * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__rglcbGetMapAddrSpace__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__rglcbInternalControlForward__)(struct RgLineCallback * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__rglcbGetInternalObjectHandle__)(struct RgLineCallback * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__rglcbAccessCallback__)(struct RgLineCallback * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__rglcbGetMemInterMapParams__)(struct RgLineCallback * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__rglcbCheckMemInterUnmap__)(struct RgLineCallback * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__rglcbGetMemoryMappingDescriptor__)(struct RgLineCallback * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__rglcbControlSerialization_Prologue__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__rglcbControlSerialization_Epilogue__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__rglcbControl_Prologue__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__rglcbControl_Epilogue__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__rglcbCanCopy__)(struct RgLineCallback * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__rglcbIsDuplicate__)(struct RgLineCallback * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__rglcbPreDestruct__)(struct RgLineCallback * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__rglcbControlFilter__)(struct RgLineCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__rglcbIsPartialUnmapSupported__)(struct RgLineCallback * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__rglcbMapTo__)(struct RgLineCallback * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__rglcbUnmapFrom__)(struct RgLineCallback * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__rglcbGetRefCount__)(struct RgLineCallback * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__rglcbAddAdditionalDependants__)(struct RsClient *, struct RgLineCallback * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 rgLineNum;
    NV0092_REGISTER_RG_LINE_CALLBACK_FN pCallbkFn;
    NvP64 pCallbkParams;
    NvU32 rgIntrLine;
    struct DispCommon *pDispCommon;
};

#ifndef __NVOC_CLASS_RgLineCallback_TYPEDEF__
#define __NVOC_CLASS_RgLineCallback_TYPEDEF__
typedef struct RgLineCallback RgLineCallback;
#endif /* __NVOC_CLASS_RgLineCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1c
#endif /* __nvoc_class_id_RgLineCallback */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RgLineCallback;

#define __staticCast_RgLineCallback(pThis) \
    ((pThis)->__nvoc_pbase_RgLineCallback)

#ifdef __nvoc_rg_line_callback_h_disabled
#define __dynamicCast_RgLineCallback(pThis) ((RgLineCallback*)NULL)
#else //__nvoc_rg_line_callback_h_disabled
#define __dynamicCast_RgLineCallback(pThis) \
    ((RgLineCallback*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RgLineCallback)))
#endif //__nvoc_rg_line_callback_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RgLineCallback(RgLineCallback**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RgLineCallback(RgLineCallback**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RgLineCallback(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RgLineCallback((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define rglcbControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define rglcbControl(pGpuResource, pCallContext, pParams) rglcbControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define rglcbMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define rglcbMap(pGpuResource, pCallContext, pParams, pCpuMapping) rglcbMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define rglcbUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define rglcbUnmap(pGpuResource, pCallContext, pCpuMapping) rglcbUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define rglcbShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define rglcbShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) rglcbShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define rglcbGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define rglcbGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) rglcbGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define rglcbGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define rglcbGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) rglcbGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define rglcbInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define rglcbInternalControlForward(pGpuResource, command, pParams, size) rglcbInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define rglcbGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define rglcbGetInternalObjectHandle(pGpuResource) rglcbGetInternalObjectHandle_DISPATCH(pGpuResource)
#define rglcbAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define rglcbAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) rglcbAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define rglcbGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define rglcbGetMemInterMapParams(pRmResource, pParams) rglcbGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define rglcbCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define rglcbCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) rglcbCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define rglcbGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define rglcbGetMemoryMappingDescriptor(pRmResource, ppMemDesc) rglcbGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define rglcbControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define rglcbControlSerialization_Prologue(pResource, pCallContext, pParams) rglcbControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define rglcbControlSerialization_Epilogue(pResource, pCallContext, pParams) rglcbControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define rglcbControl_Prologue(pResource, pCallContext, pParams) rglcbControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define rglcbControl_Epilogue(pResource, pCallContext, pParams) rglcbControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define rglcbCanCopy(pResource) rglcbCanCopy_DISPATCH(pResource)
#define rglcbIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define rglcbIsDuplicate(pResource, hMemory, pDuplicate) rglcbIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define rglcbPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define rglcbPreDestruct(pResource) rglcbPreDestruct_DISPATCH(pResource)
#define rglcbControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define rglcbControlFilter(pResource, pCallContext, pParams) rglcbControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define rglcbIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define rglcbIsPartialUnmapSupported(pResource) rglcbIsPartialUnmapSupported_DISPATCH(pResource)
#define rglcbMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define rglcbMapTo(pResource, pParams) rglcbMapTo_DISPATCH(pResource, pParams)
#define rglcbUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define rglcbUnmapFrom(pResource, pParams) rglcbUnmapFrom_DISPATCH(pResource, pParams)
#define rglcbGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define rglcbGetRefCount(pResource) rglcbGetRefCount_DISPATCH(pResource)
#define rglcbAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define rglcbAddAdditionalDependants(pClient, pResource, pReference) rglcbAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS rglcbControl_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__rglcbControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS rglcbMap_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__rglcbMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS rglcbUnmap_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__rglcbUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool rglcbShareCallback_DISPATCH(struct RgLineCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__rglcbShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS rglcbGetRegBaseOffsetAndSize_DISPATCH(struct RgLineCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__rglcbGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS rglcbGetMapAddrSpace_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__rglcbGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS rglcbInternalControlForward_DISPATCH(struct RgLineCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__rglcbInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle rglcbGetInternalObjectHandle_DISPATCH(struct RgLineCallback *pGpuResource) {
    return pGpuResource->__rglcbGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool rglcbAccessCallback_DISPATCH(struct RgLineCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__rglcbAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS rglcbGetMemInterMapParams_DISPATCH(struct RgLineCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__rglcbGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS rglcbCheckMemInterUnmap_DISPATCH(struct RgLineCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__rglcbCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS rglcbGetMemoryMappingDescriptor_DISPATCH(struct RgLineCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__rglcbGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS rglcbControlSerialization_Prologue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rglcbControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void rglcbControlSerialization_Epilogue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__rglcbControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS rglcbControl_Prologue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rglcbControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void rglcbControl_Epilogue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__rglcbControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool rglcbCanCopy_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__rglcbCanCopy__(pResource);
}

static inline NV_STATUS rglcbIsDuplicate_DISPATCH(struct RgLineCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__rglcbIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void rglcbPreDestruct_DISPATCH(struct RgLineCallback *pResource) {
    pResource->__rglcbPreDestruct__(pResource);
}

static inline NV_STATUS rglcbControlFilter_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rglcbControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool rglcbIsPartialUnmapSupported_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__rglcbIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS rglcbMapTo_DISPATCH(struct RgLineCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__rglcbMapTo__(pResource, pParams);
}

static inline NV_STATUS rglcbUnmapFrom_DISPATCH(struct RgLineCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__rglcbUnmapFrom__(pResource, pParams);
}

static inline NvU32 rglcbGetRefCount_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__rglcbGetRefCount__(pResource);
}

static inline void rglcbAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RgLineCallback *pResource, RsResourceRef *pReference) {
    pResource->__rglcbAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS rglcbConstruct_IMPL(struct RgLineCallback *arg_pRgLineCallback, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_rglcbConstruct(arg_pRgLineCallback, arg_pCallContext, arg_pParams) rglcbConstruct_IMPL(arg_pRgLineCallback, arg_pCallContext, arg_pParams)
void rglcbDestruct_IMPL(struct RgLineCallback *pRgLineCallback);

#define __nvoc_rglcbDestruct(pRgLineCallback) rglcbDestruct_IMPL(pRgLineCallback)
void rglcbInvoke_IMPL(struct RgLineCallback *pRgLineCallback, NvBool bIsIrqlIsr);

#ifdef __nvoc_rg_line_callback_h_disabled
static inline void rglcbInvoke(struct RgLineCallback *pRgLineCallback, NvBool bIsIrqlIsr) {
    NV_ASSERT_FAILED_PRECOMP("RgLineCallback was disabled!");
}
#else //__nvoc_rg_line_callback_h_disabled
#define rglcbInvoke(pRgLineCallback, bIsIrqlIsr) rglcbInvoke_IMPL(pRgLineCallback, bIsIrqlIsr)
#endif //__nvoc_rg_line_callback_h_disabled

#undef PRIVATE_FIELD


#endif // RG_LINE_CALLBACK_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RG_LINE_CALLBACK_NVOC_H_
