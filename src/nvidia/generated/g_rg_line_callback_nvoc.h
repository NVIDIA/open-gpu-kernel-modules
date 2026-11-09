
#ifndef _G_RG_LINE_CALLBACK_NVOC_H_
#define _G_RG_LINE_CALLBACK_NVOC_H_

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

#ifndef __nvoc_class_id_DispCommon
#define __nvoc_class_id_DispCommon 0x41f4f2u
typedef struct DispCommon DispCommon;
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RgLineCallback;
struct NVOC_METADATA__GpuResource;


struct RgLineCallback {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RgLineCallback *__nvoc_metadata_ptr;
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
    struct RgLineCallback *__nvoc_pbase_RgLineCallback;    // rglcb

    // Data members
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 rgLineNum;
    NV0092_REGISTER_RG_LINE_CALLBACK_FN pCallbkFn;
    NvP64 pCallbkParams;
    NvU32 rgIntrLine;
    struct DispCommon *pDispCommon;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RgLineCallback {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1cu
typedef struct RgLineCallback RgLineCallback;
#endif /* __nvoc_class_id_RgLineCallback */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RgLineCallback;

#define __staticCast_RgLineCallback(pThis) \
    ((pThis)->__nvoc_pbase_RgLineCallback)

#ifdef __nvoc_rg_line_callback_h_disabled
#define __dynamicCast_RgLineCallback(pThis) ((RgLineCallback*) NULL)
#else //__nvoc_rg_line_callback_h_disabled
#define __dynamicCast_RgLineCallback(pThis) \
    ((RgLineCallback*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RgLineCallback)))
#endif //__nvoc_rg_line_callback_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RgLineCallback(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RgLineCallback(RgLineCallback**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_RgLineCallback(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_RgLineCallback((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS rglcbConstruct_IMPL(struct RgLineCallback *pRgLineCallback, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_rglcbConstruct(pRgLineCallback, pCallContext, pParams) rglcbConstruct_IMPL(pRgLineCallback, pCallContext, pParams)

void rglcbDestruct_IMPL(struct RgLineCallback *pRgLineCallback);
#define __nvoc_rglcbDestruct(pRgLineCallback) rglcbDestruct_IMPL(pRgLineCallback)

void rglcbInvoke_IMPL(struct RgLineCallback *pRgLineCallback, NvBool bIsIrqlIsr);
#ifdef __nvoc_rg_line_callback_h_disabled
static inline void rglcbInvoke(struct RgLineCallback *pRgLineCallback, NvBool bIsIrqlIsr) {
    NV_ASSERT_FAILED_PRECOMP("RgLineCallback was disabled!");
}
#else // __nvoc_rg_line_callback_h_disabled
#define rglcbInvoke(pRgLineCallback, bIsIrqlIsr) rglcbInvoke_IMPL(pRgLineCallback, bIsIrqlIsr)
#endif // __nvoc_rg_line_callback_h_disabled


// Wrapper macros for halified functions
#define rglcbControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define rglcbControl(pGpuResource, pCallContext, pParams) rglcbControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define rglcbMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define rglcbMap(pGpuResource, pCallContext, pParams, pCpuMapping) rglcbMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define rglcbUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define rglcbUnmap(pGpuResource, pCallContext, pCpuMapping) rglcbUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define rglcbShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define rglcbShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) rglcbShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define rglcbGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define rglcbGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) rglcbGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define rglcbGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define rglcbGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) rglcbGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define rglcbInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define rglcbInternalControlForward(pGpuResource, command, pParams, size) rglcbInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define rglcbGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define rglcbGetInternalObjectHandle(pGpuResource) rglcbGetInternalObjectHandle_DISPATCH(pGpuResource)
#define rglcbAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define rglcbAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) rglcbAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define rglcbGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define rglcbGetMemInterMapParams(pRmResource, pParams) rglcbGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define rglcbCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define rglcbCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) rglcbCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define rglcbGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define rglcbGetMemoryMappingDescriptor(pRmResource, ppMemDesc) rglcbGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define rglcbControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define rglcbControlSerialization_Prologue(pResource, pCallContext, pParams) rglcbControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define rglcbControlSerialization_Epilogue(pResource, pCallContext, pParams) rglcbControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define rglcbControl_Prologue(pResource, pCallContext, pParams) rglcbControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define rglcbControl_Epilogue(pResource, pCallContext, pParams) rglcbControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rglcbCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define rglcbCanCopy(pResource) rglcbCanCopy_DISPATCH(pResource)
#define rglcbIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define rglcbIsDuplicate(pResource, hMemory, pDuplicate) rglcbIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define rglcbPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define rglcbPreDestruct(pResource) rglcbPreDestruct_DISPATCH(pResource)
#define rglcbControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define rglcbControlFilter(pResource, pCallContext, pParams) rglcbControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define rglcbIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define rglcbIsPartialUnmapSupported(pResource) rglcbIsPartialUnmapSupported_DISPATCH(pResource)
#define rglcbMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define rglcbMapTo(pResource, pParams) rglcbMapTo_DISPATCH(pResource, pParams)
#define rglcbUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define rglcbUnmapFrom(pResource, pParams) rglcbUnmapFrom_DISPATCH(pResource, pParams)
#define rglcbGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define rglcbGetRefCount(pResource) rglcbGetRefCount_DISPATCH(pResource)
#define rglcbAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define rglcbAddAdditionalDependants(pClient, pResource, pReference) rglcbAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS rglcbControl_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS rglcbMap_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS rglcbUnmap_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool rglcbShareCallback_DISPATCH(struct RgLineCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS rglcbGetRegBaseOffsetAndSize_DISPATCH(struct RgLineCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS rglcbGetMapAddrSpace_DISPATCH(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS rglcbInternalControlForward_DISPATCH(struct RgLineCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle rglcbGetInternalObjectHandle_DISPATCH(struct RgLineCallback *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool rglcbAccessCallback_DISPATCH(struct RgLineCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS rglcbGetMemInterMapParams_DISPATCH(struct RgLineCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS rglcbCheckMemInterUnmap_DISPATCH(struct RgLineCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS rglcbGetMemoryMappingDescriptor_DISPATCH(struct RgLineCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS rglcbControlSerialization_Prologue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void rglcbControlSerialization_Epilogue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS rglcbControl_Prologue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void rglcbControl_Epilogue_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool rglcbCanCopy_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS rglcbIsDuplicate_DISPATCH(struct RgLineCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void rglcbPreDestruct_DISPATCH(struct RgLineCallback *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS rglcbControlFilter_DISPATCH(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool rglcbIsPartialUnmapSupported_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS rglcbMapTo_DISPATCH(struct RgLineCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS rglcbUnmapFrom_DISPATCH(struct RgLineCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 rglcbGetRefCount_DISPATCH(struct RgLineCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void rglcbAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RgLineCallback *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // RG_LINE_CALLBACK_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RG_LINE_CALLBACK_NVOC_H_
