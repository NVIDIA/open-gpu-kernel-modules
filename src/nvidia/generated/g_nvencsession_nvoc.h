
#ifndef _G_NVENCSESSION_NVOC_H_
#define _G_NVENCSESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_nvencsession_nvoc.h"

#ifndef _NVENCSESSION_H_
#define _NVENCSESSION_H_

#include "core/core.h"
#include "rmapi/client.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu_resource.h"
#include "class/cla0bc.h"
#include "ctrl/ctrla0bc.h"


struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



typedef struct NvencSession NvencSession;
typedef struct NVENC_SESSION_LIST_ITEM
{
     NvHandle        hClient;
     NvencSession   *sessionPtr;
} NVENC_SESSION_LIST_ITEM, *PNVENC_SESSION_LIST_ITEM;
MAKE_LIST(NVENC_SESSION_LIST, NVENC_SESSION_LIST_ITEM);

typedef struct NVENC_SESSION_ENTRY
{
    NvU32               processId;
    NvU32               subProcessId;
    NvU32               sessionId;
    NvU32               codecType;
    NvU32               hResolution;
    NvU32               vResolution;
    NvU32               averageEncodeFps;
    NvU32               averageEncodeLatency; // average over last 60 samples if FPS exceeds 60
} NVENC_SESSION_ENTRY, *PNVENC_SESSION_ENTRY;



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVENCSESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvencSession;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__NvencSession;


struct NvencSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvencSession *__nvoc_metadata_ptr;
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
    struct NvencSession *__nvoc_pbase_NvencSession;    // nvencsession

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__)(struct NvencSession * /*this*/, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *);  // exported (id=0xa0bc0101)
    NV_STATUS (*__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__)(struct NvencSession * /*this*/, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *);  // exported (id=0xa0bc0102)

    // Data members
    NvHandle handle;
    NVENC_SESSION_ENTRY nvencSessionEntry;
    NvU32 version;
    struct Memory *pMemory;
    NvP64 pSessionStatsBuffer;
    NvP64 pPriv;
    NvU32 lastProcessedIndex;
    NvU32 lastProcessedFrameId;
    NvU64 lastProcessedFrameTS;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__NvencSession {
    NV_STATUS (*__nvencsessionControl__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvencsessionMap__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvencsessionUnmap__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__nvencsessionShareCallback__)(struct NvencSession * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvencsessionGetRegBaseOffsetAndSize__)(struct NvencSession * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvencsessionGetMapAddrSpace__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvencsessionInternalControlForward__)(struct NvencSession * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__nvencsessionGetInternalObjectHandle__)(struct NvencSession * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__nvencsessionAccessCallback__)(struct NvencSession * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvencsessionGetMemInterMapParams__)(struct NvencSession * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvencsessionCheckMemInterUnmap__)(struct NvencSession * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvencsessionGetMemoryMappingDescriptor__)(struct NvencSession * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvencsessionControlSerialization_Prologue__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__nvencsessionControlSerialization_Epilogue__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvencsessionControl_Prologue__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__nvencsessionControl_Epilogue__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__nvencsessionCanCopy__)(struct NvencSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvencsessionIsDuplicate__)(struct NvencSession * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__nvencsessionPreDestruct__)(struct NvencSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvencsessionControlFilter__)(struct NvencSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__nvencsessionIsPartialUnmapSupported__)(struct NvencSession * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__nvencsessionMapTo__)(struct NvencSession * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvencsessionUnmapFrom__)(struct NvencSession * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__nvencsessionGetRefCount__)(struct NvencSession * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__nvencsessionAddAdditionalDependants__)(struct RsClient *, struct NvencSession * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvencSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__NvencSession vtable;
};

#ifndef __NVOC_CLASS_NvencSession_TYPEDEF__
#define __NVOC_CLASS_NvencSession_TYPEDEF__
typedef struct NvencSession NvencSession;
#endif /* __NVOC_CLASS_NvencSession_TYPEDEF__ */

#ifndef __nvoc_class_id_NvencSession
#define __nvoc_class_id_NvencSession 0x3434af
#endif /* __nvoc_class_id_NvencSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession;

#define __staticCast_NvencSession(pThis) \
    ((pThis)->__nvoc_pbase_NvencSession)

#ifdef __nvoc_nvencsession_h_disabled
#define __dynamicCast_NvencSession(pThis) ((NvencSession*) NULL)
#else //__nvoc_nvencsession_h_disabled
#define __dynamicCast_NvencSession(pThis) \
    ((NvencSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvencSession)))
#endif //__nvoc_nvencsession_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvencSession(NvencSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvencSession(NvencSession**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_NvencSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvencSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfo_FNPTR(pNvencSession) pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfo(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfo_DISPATCH(pNvencSession, pParams)
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_FNPTR(pNvencSession) pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_DISPATCH(pNvencSession, pParams)
#define nvencsessionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define nvencsessionControl(pGpuResource, pCallContext, pParams) nvencsessionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvencsessionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define nvencsessionMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvencsessionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvencsessionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define nvencsessionUnmap(pGpuResource, pCallContext, pCpuMapping) nvencsessionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvencsessionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define nvencsessionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvencsessionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvencsessionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define nvencsessionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvencsessionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvencsessionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define nvencsessionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvencsessionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvencsessionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define nvencsessionInternalControlForward(pGpuResource, command, pParams, size) nvencsessionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvencsessionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define nvencsessionGetInternalObjectHandle(pGpuResource) nvencsessionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvencsessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define nvencsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvencsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvencsessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nvencsessionGetMemInterMapParams(pRmResource, pParams) nvencsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvencsessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nvencsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvencsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvencsessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nvencsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvencsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvencsessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define nvencsessionControlSerialization_Prologue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define nvencsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define nvencsessionControl_Prologue(pResource, pCallContext, pParams) nvencsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define nvencsessionControl_Epilogue(pResource, pCallContext, pParams) nvencsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define nvencsessionCanCopy(pResource) nvencsessionCanCopy_DISPATCH(pResource)
#define nvencsessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define nvencsessionIsDuplicate(pResource, hMemory, pDuplicate) nvencsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvencsessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define nvencsessionPreDestruct(pResource) nvencsessionPreDestruct_DISPATCH(pResource)
#define nvencsessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define nvencsessionControlFilter(pResource, pCallContext, pParams) nvencsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define nvencsessionIsPartialUnmapSupported(pResource) nvencsessionIsPartialUnmapSupported_DISPATCH(pResource)
#define nvencsessionMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define nvencsessionMapTo(pResource, pParams) nvencsessionMapTo_DISPATCH(pResource, pParams)
#define nvencsessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define nvencsessionUnmapFrom(pResource, pParams) nvencsessionUnmapFrom_DISPATCH(pResource, pParams)
#define nvencsessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define nvencsessionGetRefCount(pResource) nvencsessionGetRefCount_DISPATCH(pResource)
#define nvencsessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define nvencsessionAddAdditionalDependants(pClient, pResource, pReference) nvencsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_DISPATCH(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams) {
    return pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__(pNvencSession, pParams);
}

static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_DISPATCH(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams) {
    return pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__(pNvencSession, pParams);
}

static inline NV_STATUS nvencsessionControl_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionMap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvencsessionUnmap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool nvencsessionShareCallback_DISPATCH(struct NvencSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvencsessionGetRegBaseOffsetAndSize_DISPATCH(struct NvencSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS nvencsessionGetMapAddrSpace_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nvencsessionInternalControlForward_DISPATCH(struct NvencSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle nvencsessionGetInternalObjectHandle_DISPATCH(struct NvencSession *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool nvencsessionAccessCallback_DISPATCH(struct NvencSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvencsessionGetMemInterMapParams_DISPATCH(struct NvencSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvencsessionCheckMemInterUnmap_DISPATCH(struct NvencSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvencsessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS nvencsessionGetMemoryMappingDescriptor_DISPATCH(struct NvencSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS nvencsessionControlSerialization_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void nvencsessionControlSerialization_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__nvencsessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionControl_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void nvencsessionControl_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__nvencsessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool nvencsessionCanCopy_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionCanCopy__(pResource);
}

static inline NV_STATUS nvencsessionIsDuplicate_DISPATCH(struct NvencSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvencsessionPreDestruct_DISPATCH(struct NvencSession *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__nvencsessionPreDestruct__(pResource);
}

static inline NV_STATUS nvencsessionControlFilter_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool nvencsessionIsPartialUnmapSupported_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS nvencsessionMapTo_DISPATCH(struct NvencSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionMapTo__(pResource, pParams);
}

static inline NV_STATUS nvencsessionUnmapFrom_DISPATCH(struct NvencSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionUnmapFrom__(pResource, pParams);
}

static inline NvU32 nvencsessionGetRefCount_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvencsessionGetRefCount__(pResource);
}

static inline void nvencsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvencSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__nvencsessionAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams);

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams);

NV_STATUS nvencsessionConstruct_IMPL(struct NvencSession *arg_pNvencSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_nvencsessionConstruct(arg_pNvencSession, arg_pCallContext, arg_pParams) nvencsessionConstruct_IMPL(arg_pNvencSession, arg_pCallContext, arg_pParams)
void nvencsessionDestruct_IMPL(struct NvencSession *pNvencSession);

#define __nvoc_nvencsessionDestruct(pNvencSession) nvencsessionDestruct_IMPL(pNvencSession)
#undef PRIVATE_FIELD


#endif // _NVENCSESSION_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NVENCSESSION_NVOC_H_
