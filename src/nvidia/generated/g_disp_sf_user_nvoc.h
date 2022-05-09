#ifndef _G_DISP_SF_USER_NVOC_H_
#define _G_DISP_SF_USER_NVOC_H_
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
*       This file contains functions managing DispSfUser class.
*
******************************************************************************/

#include "g_disp_sf_user_nvoc.h"

#ifndef DISP_SF_USER_H
#define DISP_SF_USER_H

#include "gpu/gpu_resource.h"

/*!
 * RM internal class representing NVXXXX_DISP_SF_USER
 */
#ifdef NVOC_DISP_SF_USER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispSfUser {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct DispSfUser *__nvoc_pbase_DispSfUser;
    NV_STATUS (*__dispsfGetRegBaseOffsetAndSize__)(struct DispSfUser *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__dispsfShareCallback__)(struct DispSfUser *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispsfControl__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispsfUnmap__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispsfGetMemInterMapParams__)(struct DispSfUser *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispsfGetMemoryMappingDescriptor__)(struct DispSfUser *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispsfGetMapAddrSpace__)(struct DispSfUser *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvHandle (*__dispsfGetInternalObjectHandle__)(struct DispSfUser *);
    NV_STATUS (*__dispsfControlFilter__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__dispsfAddAdditionalDependants__)(struct RsClient *, struct DispSfUser *, RsResourceRef *);
    NvU32 (*__dispsfGetRefCount__)(struct DispSfUser *);
    NV_STATUS (*__dispsfCheckMemInterUnmap__)(struct DispSfUser *, NvBool);
    NV_STATUS (*__dispsfMapTo__)(struct DispSfUser *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispsfControl_Prologue__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispsfCanCopy__)(struct DispSfUser *);
    NV_STATUS (*__dispsfInternalControlForward__)(struct DispSfUser *, NvU32, void *, NvU32);
    void (*__dispsfPreDestruct__)(struct DispSfUser *);
    NV_STATUS (*__dispsfUnmapFrom__)(struct DispSfUser *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispsfControl_Epilogue__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispsfControlLookup__)(struct DispSfUser *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispsfMap__)(struct DispSfUser *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__dispsfAccessCallback__)(struct DispSfUser *, struct RsClient *, void *, RsAccessRight);
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

#define dispsfGetRegBaseOffsetAndSize(pDispSfUser, pGpu, pOffset, pSize) dispsfGetRegBaseOffsetAndSize_DISPATCH(pDispSfUser, pGpu, pOffset, pSize)
#define dispsfShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispsfShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispsfControl(pGpuResource, pCallContext, pParams) dispsfControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispsfUnmap(pGpuResource, pCallContext, pCpuMapping) dispsfUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispsfGetMemInterMapParams(pRmResource, pParams) dispsfGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispsfGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispsfGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispsfGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispsfGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispsfGetInternalObjectHandle(pGpuResource) dispsfGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispsfControlFilter(pResource, pCallContext, pParams) dispsfControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispsfAddAdditionalDependants(pClient, pResource, pReference) dispsfAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispsfGetRefCount(pResource) dispsfGetRefCount_DISPATCH(pResource)
#define dispsfCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispsfCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispsfMapTo(pResource, pParams) dispsfMapTo_DISPATCH(pResource, pParams)
#define dispsfControl_Prologue(pResource, pCallContext, pParams) dispsfControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfCanCopy(pResource) dispsfCanCopy_DISPATCH(pResource)
#define dispsfInternalControlForward(pGpuResource, command, pParams, size) dispsfInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispsfPreDestruct(pResource) dispsfPreDestruct_DISPATCH(pResource)
#define dispsfUnmapFrom(pResource, pParams) dispsfUnmapFrom_DISPATCH(pResource, pParams)
#define dispsfControl_Epilogue(pResource, pCallContext, pParams) dispsfControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispsfControlLookup(pResource, pParams, ppEntry) dispsfControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispsfMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispsfMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispsfAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispsfAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS dispsfGetRegBaseOffsetAndSize_IMPL(struct DispSfUser *pDispSfUser, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS dispsfGetRegBaseOffsetAndSize_DISPATCH(struct DispSfUser *pDispSfUser, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispSfUser->__dispsfGetRegBaseOffsetAndSize__(pDispSfUser, pGpu, pOffset, pSize);
}

static inline NvBool dispsfShareCallback_DISPATCH(struct DispSfUser *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispsfShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispsfControl_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispsfControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispsfUnmap_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispsfUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispsfGetMemInterMapParams_DISPATCH(struct DispSfUser *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispsfGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispsfGetMemoryMappingDescriptor_DISPATCH(struct DispSfUser *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispsfGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispsfGetMapAddrSpace_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispsfGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle dispsfGetInternalObjectHandle_DISPATCH(struct DispSfUser *pGpuResource) {
    return pGpuResource->__dispsfGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispsfControlFilter_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispsfControlFilter__(pResource, pCallContext, pParams);
}

static inline void dispsfAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSfUser *pResource, RsResourceRef *pReference) {
    pResource->__dispsfAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 dispsfGetRefCount_DISPATCH(struct DispSfUser *pResource) {
    return pResource->__dispsfGetRefCount__(pResource);
}

static inline NV_STATUS dispsfCheckMemInterUnmap_DISPATCH(struct DispSfUser *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispsfCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispsfMapTo_DISPATCH(struct DispSfUser *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispsfMapTo__(pResource, pParams);
}

static inline NV_STATUS dispsfControl_Prologue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispsfControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispsfCanCopy_DISPATCH(struct DispSfUser *pResource) {
    return pResource->__dispsfCanCopy__(pResource);
}

static inline NV_STATUS dispsfInternalControlForward_DISPATCH(struct DispSfUser *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispsfInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline void dispsfPreDestruct_DISPATCH(struct DispSfUser *pResource) {
    pResource->__dispsfPreDestruct__(pResource);
}

static inline NV_STATUS dispsfUnmapFrom_DISPATCH(struct DispSfUser *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispsfUnmapFrom__(pResource, pParams);
}

static inline void dispsfControl_Epilogue_DISPATCH(struct DispSfUser *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispsfControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispsfControlLookup_DISPATCH(struct DispSfUser *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispsfControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispsfMap_DISPATCH(struct DispSfUser *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispsfMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool dispsfAccessCallback_DISPATCH(struct DispSfUser *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispsfAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dispsfConstruct_IMPL(struct DispSfUser *arg_pDispSfUser, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_dispsfConstruct(arg_pDispSfUser, arg_pCallContext, arg_pParams) dispsfConstruct_IMPL(arg_pDispSfUser, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_SF_USER_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_DISP_SF_USER_NVOC_H_
