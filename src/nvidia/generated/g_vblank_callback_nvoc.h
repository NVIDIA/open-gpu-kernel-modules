
#ifndef _G_VBLANK_CALLBACK_NVOC_H_
#define _G_VBLANK_CALLBACK_NVOC_H_
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


struct VblankCallback {

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
    struct VblankCallback *__nvoc_pbase_VblankCallback;    // vblcb

    // Vtable with 26 per-object function pointers
    NV_STATUS (*__vblcbCtrlSetVBlankNotification__)(struct VblankCallback * /*this*/, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *);  // exported (id=0x90100101)
    NV_STATUS (*__vblcbControl__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vblcbMap__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vblcbUnmap__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vblcbShareCallback__)(struct VblankCallback * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vblcbGetRegBaseOffsetAndSize__)(struct VblankCallback * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vblcbGetMapAddrSpace__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vblcbInternalControlForward__)(struct VblankCallback * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__vblcbGetInternalObjectHandle__)(struct VblankCallback * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vblcbAccessCallback__)(struct VblankCallback * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vblcbGetMemInterMapParams__)(struct VblankCallback * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vblcbCheckMemInterUnmap__)(struct VblankCallback * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vblcbGetMemoryMappingDescriptor__)(struct VblankCallback * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vblcbControlSerialization_Prologue__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vblcbControlSerialization_Epilogue__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vblcbControl_Prologue__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vblcbControl_Epilogue__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__vblcbCanCopy__)(struct VblankCallback * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vblcbIsDuplicate__)(struct VblankCallback * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__vblcbPreDestruct__)(struct VblankCallback * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vblcbControlFilter__)(struct VblankCallback * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__vblcbIsPartialUnmapSupported__)(struct VblankCallback * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__vblcbMapTo__)(struct VblankCallback * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vblcbUnmapFrom__)(struct VblankCallback * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__vblcbGetRefCount__)(struct VblankCallback * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__vblcbAddAdditionalDependants__)(struct RsClient *, struct VblankCallback * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    VBLANKCALLBACK CallBack;
    OSVBLANKCALLBACKPROC pProc;
    NvP64 pParm1;
    NvP64 pParm2;
    NvU32 LogicalHead;
};

#ifndef __NVOC_CLASS_VblankCallback_TYPEDEF__
#define __NVOC_CLASS_VblankCallback_TYPEDEF__
typedef struct VblankCallback VblankCallback;
#endif /* __NVOC_CLASS_VblankCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_VblankCallback
#define __nvoc_class_id_VblankCallback 0x4c1997
#endif /* __nvoc_class_id_VblankCallback */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VblankCallback;

#define __staticCast_VblankCallback(pThis) \
    ((pThis)->__nvoc_pbase_VblankCallback)

#ifdef __nvoc_vblank_callback_h_disabled
#define __dynamicCast_VblankCallback(pThis) ((VblankCallback*)NULL)
#else //__nvoc_vblank_callback_h_disabled
#define __dynamicCast_VblankCallback(pThis) \
    ((VblankCallback*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VblankCallback)))
#endif //__nvoc_vblank_callback_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VblankCallback(VblankCallback**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VblankCallback(VblankCallback**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VblankCallback(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VblankCallback((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vblcbCtrlSetVBlankNotification_FNPTR(pVblankCallback) pVblankCallback->__vblcbCtrlSetVBlankNotification__
#define vblcbCtrlSetVBlankNotification(pVblankCallback, pParams) vblcbCtrlSetVBlankNotification_DISPATCH(pVblankCallback, pParams)
#define vblcbControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define vblcbControl(pGpuResource, pCallContext, pParams) vblcbControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vblcbMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define vblcbMap(pGpuResource, pCallContext, pParams, pCpuMapping) vblcbMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vblcbUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define vblcbUnmap(pGpuResource, pCallContext, pCpuMapping) vblcbUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vblcbShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define vblcbShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vblcbShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vblcbGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define vblcbGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vblcbGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vblcbGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define vblcbGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vblcbGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vblcbInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define vblcbInternalControlForward(pGpuResource, command, pParams, size) vblcbInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vblcbGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define vblcbGetInternalObjectHandle(pGpuResource) vblcbGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vblcbAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define vblcbAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vblcbAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vblcbGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define vblcbGetMemInterMapParams(pRmResource, pParams) vblcbGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vblcbCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define vblcbCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vblcbCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vblcbGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define vblcbGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vblcbGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vblcbControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define vblcbControlSerialization_Prologue(pResource, pCallContext, pParams) vblcbControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define vblcbControlSerialization_Epilogue(pResource, pCallContext, pParams) vblcbControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define vblcbControl_Prologue(pResource, pCallContext, pParams) vblcbControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define vblcbControl_Epilogue(pResource, pCallContext, pParams) vblcbControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vblcbCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define vblcbCanCopy(pResource) vblcbCanCopy_DISPATCH(pResource)
#define vblcbIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define vblcbIsDuplicate(pResource, hMemory, pDuplicate) vblcbIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vblcbPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define vblcbPreDestruct(pResource) vblcbPreDestruct_DISPATCH(pResource)
#define vblcbControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define vblcbControlFilter(pResource, pCallContext, pParams) vblcbControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vblcbIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define vblcbIsPartialUnmapSupported(pResource) vblcbIsPartialUnmapSupported_DISPATCH(pResource)
#define vblcbMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define vblcbMapTo(pResource, pParams) vblcbMapTo_DISPATCH(pResource, pParams)
#define vblcbUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define vblcbUnmapFrom(pResource, pParams) vblcbUnmapFrom_DISPATCH(pResource, pParams)
#define vblcbGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define vblcbGetRefCount(pResource) vblcbGetRefCount_DISPATCH(pResource)
#define vblcbAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define vblcbAddAdditionalDependants(pClient, pResource, pReference) vblcbAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vblcbCtrlSetVBlankNotification_DISPATCH(struct VblankCallback *pVblankCallback, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams) {
    return pVblankCallback->__vblcbCtrlSetVBlankNotification__(pVblankCallback, pParams);
}

static inline NV_STATUS vblcbControl_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vblcbControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vblcbMap_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vblcbMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vblcbUnmap_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vblcbUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool vblcbShareCallback_DISPATCH(struct VblankCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vblcbShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vblcbGetRegBaseOffsetAndSize_DISPATCH(struct VblankCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vblcbGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vblcbGetMapAddrSpace_DISPATCH(struct VblankCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vblcbGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vblcbInternalControlForward_DISPATCH(struct VblankCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vblcbInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle vblcbGetInternalObjectHandle_DISPATCH(struct VblankCallback *pGpuResource) {
    return pGpuResource->__vblcbGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool vblcbAccessCallback_DISPATCH(struct VblankCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vblcbAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vblcbGetMemInterMapParams_DISPATCH(struct VblankCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vblcbGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vblcbCheckMemInterUnmap_DISPATCH(struct VblankCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vblcbCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vblcbGetMemoryMappingDescriptor_DISPATCH(struct VblankCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vblcbGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vblcbControlSerialization_Prologue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vblcbControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vblcbControlSerialization_Epilogue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vblcbControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vblcbControl_Prologue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vblcbControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vblcbControl_Epilogue_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vblcbControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool vblcbCanCopy_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__vblcbCanCopy__(pResource);
}

static inline NV_STATUS vblcbIsDuplicate_DISPATCH(struct VblankCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vblcbIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vblcbPreDestruct_DISPATCH(struct VblankCallback *pResource) {
    pResource->__vblcbPreDestruct__(pResource);
}

static inline NV_STATUS vblcbControlFilter_DISPATCH(struct VblankCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vblcbControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vblcbIsPartialUnmapSupported_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__vblcbIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vblcbMapTo_DISPATCH(struct VblankCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vblcbMapTo__(pResource, pParams);
}

static inline NV_STATUS vblcbUnmapFrom_DISPATCH(struct VblankCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vblcbUnmapFrom__(pResource, pParams);
}

static inline NvU32 vblcbGetRefCount_DISPATCH(struct VblankCallback *pResource) {
    return pResource->__vblcbGetRefCount__(pResource);
}

static inline void vblcbAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VblankCallback *pResource, RsResourceRef *pReference) {
    pResource->__vblcbAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS vblcbCtrlSetVBlankNotification_IMPL(struct VblankCallback *pVblankCallback, NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams);

NV_STATUS vblcbConstruct_IMPL(struct VblankCallback *arg_pVblankCallback, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vblcbConstruct(arg_pVblankCallback, arg_pCallContext, arg_pParams) vblcbConstruct_IMPL(arg_pVblankCallback, arg_pCallContext, arg_pParams)
void vblcbDestruct_IMPL(struct VblankCallback *pVblankCallback);

#define __nvoc_vblcbDestruct(pVblankCallback) vblcbDestruct_IMPL(pVblankCallback)
#undef PRIVATE_FIELD


#endif // VBLANK_CALLBACK_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VBLANK_CALLBACK_NVOC_H_
