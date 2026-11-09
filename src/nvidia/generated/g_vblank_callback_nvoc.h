
#ifndef _G_VBLANK_CALLBACK_NVOC_H_
#define _G_VBLANK_CALLBACK_NVOC_H_

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

/******************************************************************************
*
*       VblankCallback Module Header
*       This file contains functions managing vblank callback.
*
******************************************************************************/

#pragma once
#include "g_vblank_callback_nvoc.h"

#ifndef VBLANK_CALLBACK_H
#define VBLANK_CALLBACK_H

#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"
#include "gpu/disp/vblank_callback/vblank.h"
#include "ctrl/ctrl9010.h"
/*!
 * RM internal class representing NV9010_VBLANK_CALLBACK
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VBLANK_CALLBACK_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VblankCallback;
struct NVOC_METADATA__GpuResource;


struct VblankCallback {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VblankCallback *__nvoc_metadata_ptr;
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
    struct VblankCallback *__nvoc_pbase_VblankCallback;    // vblcb

    // Data members
    VBLANKCALLBACK CallBack;
    OSVBLANKCALLBACKPROC pProc;
    NvP64 pParm1;
    NvP64 pParm2;
    NvU32 LogicalHead;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VblankCallback {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_VblankCallback
#define __nvoc_class_id_VblankCallback 0x4c1997u
typedef struct VblankCallback VblankCallback;
#endif /* __nvoc_class_id_VblankCallback */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VblankCallback;

#define __staticCast_VblankCallback(pThis) \
    ((pThis)->__nvoc_pbase_VblankCallback)

#ifdef __nvoc_vblank_callback_h_disabled
#define __dynamicCast_VblankCallback(pThis) ((VblankCallback*) NULL)
#else //__nvoc_vblank_callback_h_disabled
#define __dynamicCast_VblankCallback(pThis) \
    ((VblankCallback*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VblankCallback)))
#endif //__nvoc_vblank_callback_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VblankCallback(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VblankCallback(VblankCallback**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_VblankCallback(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_VblankCallback((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS vblcbConstruct_IMPL(struct VblankCallback *pVblankCallback, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_vblcbConstruct(pVblankCallback, pCallContext, pParams) vblcbConstruct_IMPL(pVblankCallback, pCallContext, pParams)

void vblcbDestruct_IMPL(struct VblankCallback *pVblankCallback);
#define __nvoc_vblcbDestruct(pVblankCallback) vblcbDestruct_IMPL(pVblankCallback)

NV_STATUS vblcbCtrlSetVBlankNotification_IMPL(struct VblankCallback *pVblankCallback, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams);
#ifdef __nvoc_vblank_callback_h_disabled
static inline NV_STATUS vblcbCtrlSetVBlankNotification(struct VblankCallback *pVblankCallback, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VblankCallback was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vblank_callback_h_disabled
#define vblcbCtrlSetVBlankNotification(pVblankCallback, pParams) vblcbCtrlSetVBlankNotification_IMPL(pVblankCallback, pParams)
#endif // __nvoc_vblank_callback_h_disabled


// Wrapper macros for halified functions
#define vblcbControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define vblcbControl(pGpuResource, pCallContext, pParams) vblcbControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vblcbMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define vblcbMap(pGpuResource, pCallContext, pParams, pCpuMapping) vblcbMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vblcbUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define vblcbUnmap(pGpuResource, pCallContext, pCpuMapping) vblcbUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vblcbShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define vblcbShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vblcbShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vblcbGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define vblcbGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vblcbGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vblcbGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define vblcbGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vblcbGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vblcbInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define vblcbInternalControlForward(pGpuResource, command, pParams, size) vblcbInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vblcbGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define vblcbGetInternalObjectHandle(pGpuResource) vblcbGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vblcbAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define vblcbAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vblcbAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vblcbGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vblcbGetMemInterMapParams(pRmResource, pParams) vblcbGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vblcbCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vblcbCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vblcbCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vblcbGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vblcbGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vblcbGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vblcbControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define vblcbControlSerialization_Prologue(pResource, pCallContext, pParams) vblcbControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define vblcbControlSerialization_Epilogue(pResource, pCallContext, pParams) vblcbControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define vblcbControl_Prologue(pResource, pCallContext, pParams) vblcbControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define vblcbControl_Epilogue(pResource, pCallContext, pParams) vblcbControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define vblcbCanCopy(pResource) vblcbCanCopy_DISPATCH(pResource)
#define vblcbIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define vblcbIsDuplicate(pResource, hMemory, pDuplicate) vblcbIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vblcbPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define vblcbPreDestruct(pResource) vblcbPreDestruct_DISPATCH(pResource)
#define vblcbControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define vblcbControlFilter(pResource, pCallContext, pParams) vblcbControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vblcbIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define vblcbIsPartialUnmapSupported(pResource) vblcbIsPartialUnmapSupported_DISPATCH(pResource)
#define vblcbMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define vblcbMapTo(pResource, pParams) vblcbMapTo_DISPATCH(pResource, pParams)
#define vblcbUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define vblcbUnmapFrom(pResource, pParams) vblcbUnmapFrom_DISPATCH(pResource, pParams)
#define vblcbGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define vblcbGetRefCount(pResource) vblcbGetRefCount_DISPATCH(pResource)
#define vblcbAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define vblcbAddAdditionalDependants(pClient, pResource, pReference) vblcbAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vblcbControl_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vblcbMap_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vblcbUnmap_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool vblcbShareCallback_DISPATCH(struct VblankCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vblcbGetRegBaseOffsetAndSize_DISPATCH(struct VblankCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS vblcbGetMapAddrSpace_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vblcbInternalControlForward_DISPATCH(struct VblankCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle vblcbGetInternalObjectHandle_DISPATCH(struct VblankCallback *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool vblcbAccessCallback_DISPATCH(struct VblankCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vblcbGetMemInterMapParams_DISPATCH(struct VblankCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS vblcbCheckMemInterUnmap_DISPATCH(struct VblankCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS vblcbGetMemoryMappingDescriptor_DISPATCH(struct VblankCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS vblcbControlSerialization_Prologue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vblcbControlSerialization_Epilogue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vblcbControl_Prologue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vblcbControl_Epilogue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vblcbCanCopy_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vblcbIsDuplicate_DISPATCH(struct VblankCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void vblcbPreDestruct_DISPATCH(struct VblankCallback *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vblcbControlFilter_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vblcbIsPartialUnmapSupported_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vblcbMapTo_DISPATCH(struct VblankCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS vblcbUnmapFrom_DISPATCH(struct VblankCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 vblcbGetRefCount_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void vblcbAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VblankCallback *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS vblcbCtrlSetVBlankNotification_IMPL(struct VblankCallback *pVblankCallback, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // VBLANK_CALLBACK_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VBLANK_CALLBACK_NVOC_H_
