
#ifndef _G_DISP_SF_USER_NVOC_H_
#define _G_DISP_SF_USER_NVOC_H_
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
*       This file contains functions managing DispSfUser class.
*
******************************************************************************/

#pragma once
#include "g_disp_sf_user_nvoc.h"

#ifndef DISP_SF_USER_H
#define DISP_SF_USER_H

#include "gpu/gpu_resource.h"

/*!
 * RM internal class representing NVXXXX_DISP_SF_USER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_SF_USER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispSfUser {

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
    struct DispSfUser *__nvoc_pbase_DispSfUser;    // dispsf

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__dispsfGetRegBaseOffsetAndSize__)(struct DispSfUser * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__dispsfControl__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispsfMap__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispsfUnmap__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispsfShareCallback__)(struct DispSfUser * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispsfGetMapAddrSpace__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispsfInternalControlForward__)(struct DispSfUser * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__dispsfGetInternalObjectHandle__)(struct DispSfUser * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispsfAccessCallback__)(struct DispSfUser * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispsfGetMemInterMapParams__)(struct DispSfUser * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispsfCheckMemInterUnmap__)(struct DispSfUser * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispsfGetMemoryMappingDescriptor__)(struct DispSfUser * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispsfControlSerialization_Prologue__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispsfControlSerialization_Epilogue__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispsfControl_Prologue__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispsfControl_Epilogue__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__dispsfCanCopy__)(struct DispSfUser * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispsfIsDuplicate__)(struct DispSfUser * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__dispsfPreDestruct__)(struct DispSfUser * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispsfControlFilter__)(struct DispSfUser * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__dispsfIsPartialUnmapSupported__)(struct DispSfUser * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__dispsfMapTo__)(struct DispSfUser * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispsfUnmapFrom__)(struct DispSfUser * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__dispsfGetRefCount__)(struct DispSfUser * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__dispsfAddAdditionalDependants__)(struct RsClient *, struct DispSfUser * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    NvU32 ControlOffset;
    NvU32 ControlLength;
};

#ifndef __NVOC_CLASS_DispSfUser_TYPEDEF__
#define __NVOC_CLASS_DispSfUser_TYPEDEF__
typedef struct DispSfUser DispSfUser;
#endif /* __NVOC_CLASS_DispSfUser_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSfUser
#define __nvoc_class_id_DispSfUser 0xba7439
#endif /* __nvoc_class_id_DispSfUser */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSfUser;

#define __staticCast_DispSfUser(pThis) \
    ((pThis)->__nvoc_pbase_DispSfUser)

#ifdef __nvoc_disp_sf_user_h_disabled
#define __dynamicCast_DispSfUser(pThis) ((DispSfUser*)NULL)
#else //__nvoc_disp_sf_user_h_disabled
#define __dynamicCast_DispSfUser(pThis) \
    ((DispSfUser*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispSfUser)))
#endif //__nvoc_disp_sf_user_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispSfUser(DispSfUser**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispSfUser(DispSfUser**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispSfUser(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispSfUser((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispsfGetRegBaseOffsetAndSize_FNPTR(pDispSfUser) pDispSfUser->__dispsfGetRegBaseOffsetAndSize__
#define dispsfGetRegBaseOffsetAndSize(pDispSfUser, pGpu, pOffset, pSize) dispsfGetRegBaseOffsetAndSize_DISPATCH(pDispSfUser, pGpu, pOffset, pSize)
#define dispsfControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define dispsfControl(pGpuResource, pCallContext, pParams) dispsfControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispsfMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define dispsfMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispsfMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispsfUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define dispsfUnmap(pGpuResource, pCallContext, pCpuMapping) dispsfUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispsfShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispsfShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispsfShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispsfGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispsfGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispsfGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispsfInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispsfInternalControlForward(pGpuResource, command, pParams, size) dispsfInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispsfGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispsfGetInternalObjectHandle(pGpuResource) dispsfGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispsfAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispsfAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispsfAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispsfGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispsfGetMemInterMapParams(pRmResource, pParams) dispsfGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispsfCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispsfCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispsfCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispsfGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispsfGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispsfGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispsfControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispsfControlSerialization_Prologue(pResource, pCallContext, pParams) dispsfControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispsfControlSerialization_Epilogue(pResource, pCallContext, pParams) dispsfControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispsfControl_Prologue(pResource, pCallContext, pParams) dispsfControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispsfControl_Epilogue(pResource, pCallContext, pParams) dispsfControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispsfCanCopy(pResource) dispsfCanCopy_DISPATCH(pResource)
#define dispsfIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispsfIsDuplicate(pResource, hMemory, pDuplicate) dispsfIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispsfPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispsfPreDestruct(pResource) dispsfPreDestruct_DISPATCH(pResource)
#define dispsfControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispsfControlFilter(pResource, pCallContext, pParams) dispsfControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispsfIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispsfIsPartialUnmapSupported(pResource) dispsfIsPartialUnmapSupported_DISPATCH(pResource)
#define dispsfMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispsfMapTo(pResource, pParams) dispsfMapTo_DISPATCH(pResource, pParams)
#define dispsfUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispsfUnmapFrom(pResource, pParams) dispsfUnmapFrom_DISPATCH(pResource, pParams)
#define dispsfGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispsfGetRefCount(pResource) dispsfGetRefCount_DISPATCH(pResource)
#define dispsfAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispsfAddAdditionalDependants(pClient, pResource, pReference) dispsfAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS dispsfGetRegBaseOffsetAndSize_DISPATCH(struct DispSfUser *pDispSfUser, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispSfUser->__dispsfGetRegBaseOffsetAndSize__(pDispSfUser, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispsfControl_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispsfControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispsfMap_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispsfMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispsfUnmap_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispsfUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispsfShareCallback_DISPATCH(struct DispSfUser *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispsfShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispsfGetMapAddrSpace_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispsfGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispsfInternalControlForward_DISPATCH(struct DispSfUser *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispsfInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispsfGetInternalObjectHandle_DISPATCH(struct DispSfUser *pGpuResource) {
    return pGpuResource->__dispsfGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispsfAccessCallback_DISPATCH(struct DispSfUser *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispsfAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispsfGetMemInterMapParams_DISPATCH(struct DispSfUser *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispsfGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispsfCheckMemInterUnmap_DISPATCH(struct DispSfUser *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispsfCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispsfGetMemoryMappingDescriptor_DISPATCH(struct DispSfUser *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispsfGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispsfControlSerialization_Prologue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispsfControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispsfControlSerialization_Epilogue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispsfControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispsfControl_Prologue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispsfControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispsfControl_Epilogue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispsfControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispsfCanCopy_DISPATCH(struct DispSfUser *pResource) {
    return pResource->__dispsfCanCopy__(pResource);
}

static inline NV_STATUS dispsfIsDuplicate_DISPATCH(struct DispSfUser *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispsfIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispsfPreDestruct_DISPATCH(struct DispSfUser *pResource) {
    pResource->__dispsfPreDestruct__(pResource);
}

static inline NV_STATUS dispsfControlFilter_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispsfControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispsfIsPartialUnmapSupported_DISPATCH(struct DispSfUser *pResource) {
    return pResource->__dispsfIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispsfMapTo_DISPATCH(struct DispSfUser *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispsfMapTo__(pResource, pParams);
}

static inline NV_STATUS dispsfUnmapFrom_DISPATCH(struct DispSfUser *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispsfUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispsfGetRefCount_DISPATCH(struct DispSfUser *pResource) {
    return pResource->__dispsfGetRefCount__(pResource);
}

static inline void dispsfAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSfUser *pResource, RsResourceRef *pReference) {
    pResource->__dispsfAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS dispsfGetRegBaseOffsetAndSize_IMPL(struct DispSfUser *pDispSfUser, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

NV_STATUS dispsfConstruct_IMPL(struct DispSfUser *arg_pDispSfUser, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispsfConstruct(arg_pDispSfUser, arg_pCallContext, arg_pParams) dispsfConstruct_IMPL(arg_pDispSfUser, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_SF_USER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_SF_USER_NVOC_H_
