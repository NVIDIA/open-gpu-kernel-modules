
#ifndef _G_WPPS_CONFIG_SESSION_NVOC_H_
#define _G_WPPS_CONFIG_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      WPPS Config Session - capability acquisition and control class for
 *      Workload Power Profile Settings.
 *
 *   Allocating this class validates NV_RM_CAP_SYS_WPPS_ACCESS.  Once
 *   allocated, the client can invoke the NVD0CD_CTRL_CMD_SET_PROFILES
 *   control to change WPPS profiles.  Admin callers always succeed.
 *
 *   Key attributes:
 *   - Parented under Subdevice.
 *   - Non-admin callers without a valid capability FD are rejected at alloc.
 *   - RmApi lock must be held.
 *****************************************************************************/

#pragma once
#include "g_wpps_config_session_nvoc.h"

#ifndef WPPS_CONFIG_SESSION_H
#define WPPS_CONFIG_SESSION_H


#include "gpu/gpu_resource.h"
#include "ctrl/ctrld0cd.h"


// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_WPPS_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__WppsConfigSession;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__WppsConfigSession;


struct WppsConfigSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__WppsConfigSession *__nvoc_metadata_ptr;
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
    struct WppsConfigSession *__nvoc_pbase_WppsConfigSession;    // wppsConfigSession

    // Data members
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__WppsConfigSession {
    NvBool (*__wppsConfigSessionCanCopy__)(struct WppsConfigSession * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__wppsConfigSessionControl__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__wppsConfigSessionMap__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__wppsConfigSessionUnmap__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__wppsConfigSessionShareCallback__)(struct WppsConfigSession * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__wppsConfigSessionGetRegBaseOffsetAndSize__)(struct WppsConfigSession * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__wppsConfigSessionGetMapAddrSpace__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__wppsConfigSessionInternalControlForward__)(struct WppsConfigSession * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__wppsConfigSessionGetInternalObjectHandle__)(struct WppsConfigSession * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__wppsConfigSessionAccessCallback__)(struct WppsConfigSession * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionGetMemInterMapParams__)(struct WppsConfigSession * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionCheckMemInterUnmap__)(struct WppsConfigSession * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionGetMemoryMappingDescriptor__)(struct WppsConfigSession * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionControlSerialization_Prologue__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__wppsConfigSessionControlSerialization_Epilogue__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionControl_Prologue__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__wppsConfigSessionControl_Epilogue__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__wppsConfigSessionIsDuplicate__)(struct WppsConfigSession * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__wppsConfigSessionPreDestruct__)(struct WppsConfigSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__wppsConfigSessionControlFilter__)(struct WppsConfigSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__wppsConfigSessionIsPartialUnmapSupported__)(struct WppsConfigSession * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__wppsConfigSessionMapTo__)(struct WppsConfigSession * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__wppsConfigSessionUnmapFrom__)(struct WppsConfigSession * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__wppsConfigSessionGetRefCount__)(struct WppsConfigSession * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__wppsConfigSessionAddAdditionalDependants__)(struct RsClient *, struct WppsConfigSession * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__WppsConfigSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__WppsConfigSession vtable;
};

#ifndef __nvoc_class_id_WppsConfigSession
#define __nvoc_class_id_WppsConfigSession 0x853c55u
typedef struct WppsConfigSession WppsConfigSession;
#endif /* __nvoc_class_id_WppsConfigSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_WppsConfigSession;

#define __staticCast_WppsConfigSession(pThis) \
    ((pThis)->__nvoc_pbase_WppsConfigSession)

#ifdef __nvoc_wpps_config_session_h_disabled
#define __dynamicCast_WppsConfigSession(pThis) ((WppsConfigSession*) NULL)
#else //__nvoc_wpps_config_session_h_disabled
#define __dynamicCast_WppsConfigSession(pThis) \
    ((WppsConfigSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(WppsConfigSession)))
#endif //__nvoc_wpps_config_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_WppsConfigSession(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_WppsConfigSession(WppsConfigSession**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_WppsConfigSession(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_WppsConfigSession((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS wppsConfigSessionConstruct_IMPL(struct WppsConfigSession *pWppsConfigSession, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_wppsConfigSessionConstruct(pWppsConfigSession, pCallContext, pParams) wppsConfigSessionConstruct_IMPL(pWppsConfigSession, pCallContext, pParams)

void wppsConfigSessionDestruct_IMPL(struct WppsConfigSession *pWppsConfigSession);
#define __nvoc_wppsConfigSessionDestruct(pWppsConfigSession) wppsConfigSessionDestruct_IMPL(pWppsConfigSession)

NV_STATUS wppsConfigSessionCtrlCmdSetProfiles_IMPL(struct WppsConfigSession *pWppsConfigSession, NVD0CD_CTRL_SET_PROFILES_PARAMS *pParams);
#ifdef __nvoc_wpps_config_session_h_disabled
static inline NV_STATUS wppsConfigSessionCtrlCmdSetProfiles(struct WppsConfigSession *pWppsConfigSession, NVD0CD_CTRL_SET_PROFILES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("WppsConfigSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_wpps_config_session_h_disabled
#define wppsConfigSessionCtrlCmdSetProfiles(pWppsConfigSession, pParams) wppsConfigSessionCtrlCmdSetProfiles_IMPL(pWppsConfigSession, pParams)
#endif // __nvoc_wpps_config_session_h_disabled


// Wrapper macros for halified functions
#define wppsConfigSessionCanCopy_FNPTR(pWppsConfigSession) pWppsConfigSession->__nvoc_metadata_ptr->vtable.__wppsConfigSessionCanCopy__
#define wppsConfigSessionCanCopy(pWppsConfigSession) wppsConfigSessionCanCopy_DISPATCH(pWppsConfigSession)
#define wppsConfigSessionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define wppsConfigSessionControl(pGpuResource, pCallContext, pParams) wppsConfigSessionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define wppsConfigSessionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define wppsConfigSessionMap(pGpuResource, pCallContext, pParams, pCpuMapping) wppsConfigSessionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define wppsConfigSessionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define wppsConfigSessionUnmap(pGpuResource, pCallContext, pCpuMapping) wppsConfigSessionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define wppsConfigSessionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define wppsConfigSessionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) wppsConfigSessionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define wppsConfigSessionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define wppsConfigSessionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) wppsConfigSessionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define wppsConfigSessionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define wppsConfigSessionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) wppsConfigSessionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define wppsConfigSessionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define wppsConfigSessionInternalControlForward(pGpuResource, command, pParams, size) wppsConfigSessionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define wppsConfigSessionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define wppsConfigSessionGetInternalObjectHandle(pGpuResource) wppsConfigSessionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define wppsConfigSessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define wppsConfigSessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) wppsConfigSessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define wppsConfigSessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define wppsConfigSessionGetMemInterMapParams(pRmResource, pParams) wppsConfigSessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define wppsConfigSessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define wppsConfigSessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) wppsConfigSessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define wppsConfigSessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define wppsConfigSessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) wppsConfigSessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define wppsConfigSessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define wppsConfigSessionControlSerialization_Prologue(pResource, pCallContext, pParams) wppsConfigSessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define wppsConfigSessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define wppsConfigSessionControlSerialization_Epilogue(pResource, pCallContext, pParams) wppsConfigSessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define wppsConfigSessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define wppsConfigSessionControl_Prologue(pResource, pCallContext, pParams) wppsConfigSessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define wppsConfigSessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define wppsConfigSessionControl_Epilogue(pResource, pCallContext, pParams) wppsConfigSessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define wppsConfigSessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define wppsConfigSessionIsDuplicate(pResource, hMemory, pDuplicate) wppsConfigSessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define wppsConfigSessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define wppsConfigSessionPreDestruct(pResource) wppsConfigSessionPreDestruct_DISPATCH(pResource)
#define wppsConfigSessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define wppsConfigSessionControlFilter(pResource, pCallContext, pParams) wppsConfigSessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define wppsConfigSessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define wppsConfigSessionIsPartialUnmapSupported(pResource) wppsConfigSessionIsPartialUnmapSupported_DISPATCH(pResource)
#define wppsConfigSessionMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define wppsConfigSessionMapTo(pResource, pParams) wppsConfigSessionMapTo_DISPATCH(pResource, pParams)
#define wppsConfigSessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define wppsConfigSessionUnmapFrom(pResource, pParams) wppsConfigSessionUnmapFrom_DISPATCH(pResource, pParams)
#define wppsConfigSessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define wppsConfigSessionGetRefCount(pResource) wppsConfigSessionGetRefCount_DISPATCH(pResource)
#define wppsConfigSessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define wppsConfigSessionAddAdditionalDependants(pClient, pResource, pReference) wppsConfigSessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool wppsConfigSessionCanCopy_DISPATCH(struct WppsConfigSession *pWppsConfigSession) {
    return pWppsConfigSession->__nvoc_metadata_ptr->vtable.__wppsConfigSessionCanCopy__(pWppsConfigSession);
}

static inline NV_STATUS wppsConfigSessionControl_DISPATCH(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS wppsConfigSessionMap_DISPATCH(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS wppsConfigSessionUnmap_DISPATCH(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool wppsConfigSessionShareCallback_DISPATCH(struct WppsConfigSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS wppsConfigSessionGetRegBaseOffsetAndSize_DISPATCH(struct WppsConfigSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS wppsConfigSessionGetMapAddrSpace_DISPATCH(struct WppsConfigSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS wppsConfigSessionInternalControlForward_DISPATCH(struct WppsConfigSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle wppsConfigSessionGetInternalObjectHandle_DISPATCH(struct WppsConfigSession *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool wppsConfigSessionAccessCallback_DISPATCH(struct WppsConfigSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS wppsConfigSessionGetMemInterMapParams_DISPATCH(struct WppsConfigSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS wppsConfigSessionCheckMemInterUnmap_DISPATCH(struct WppsConfigSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS wppsConfigSessionGetMemoryMappingDescriptor_DISPATCH(struct WppsConfigSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS wppsConfigSessionControlSerialization_Prologue_DISPATCH(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void wppsConfigSessionControlSerialization_Epilogue_DISPATCH(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS wppsConfigSessionControl_Prologue_DISPATCH(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void wppsConfigSessionControl_Epilogue_DISPATCH(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS wppsConfigSessionIsDuplicate_DISPATCH(struct WppsConfigSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void wppsConfigSessionPreDestruct_DISPATCH(struct WppsConfigSession *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionPreDestruct__(pResource);
}

static inline NV_STATUS wppsConfigSessionControlFilter_DISPATCH(struct WppsConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool wppsConfigSessionIsPartialUnmapSupported_DISPATCH(struct WppsConfigSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS wppsConfigSessionMapTo_DISPATCH(struct WppsConfigSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionMapTo__(pResource, pParams);
}

static inline NV_STATUS wppsConfigSessionUnmapFrom_DISPATCH(struct WppsConfigSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionUnmapFrom__(pResource, pParams);
}

static inline NvU32 wppsConfigSessionGetRefCount_DISPATCH(struct WppsConfigSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionGetRefCount__(pResource);
}

static inline void wppsConfigSessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct WppsConfigSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__wppsConfigSessionAddAdditionalDependants__(pClient, pResource, pReference);
}

// Virtual method declarations and/or inline definitions
NvBool wppsConfigSessionCanCopy_IMPL(struct WppsConfigSession *pWppsConfigSession);

// Exported method declarations and/or inline definitions
NV_STATUS wppsConfigSessionCtrlCmdSetProfiles_IMPL(struct WppsConfigSession *pWppsConfigSession, NVD0CD_CTRL_SET_PROFILES_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



#endif // WPPS_CONFIG_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_WPPS_CONFIG_SESSION_NVOC_H_
