
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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2u
typedef struct Memory Memory;
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvencSession;
struct NVOC_METADATA__GpuResource;


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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvencSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_NvencSession
#define __nvoc_class_id_NvencSession 0x3434afu
typedef struct NvencSession NvencSession;
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

NV_STATUS __nvoc_objCreateDynamic_NvencSession(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvencSession(NvencSession**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_NvencSession(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_NvencSession((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS nvencsessionConstruct_IMPL(struct NvencSession *pNvencSession, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_nvencsessionConstruct(pNvencSession, pCallContext, pParams) nvencsessionConstruct_IMPL(pNvencSession, pCallContext, pParams)

void nvencsessionDestruct_IMPL(struct NvencSession *pNvencSession);
#define __nvoc_nvencsessionDestruct(pNvencSession) nvencsessionDestruct_IMPL(pNvencSession)

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams);
#ifdef __nvoc_nvencsession_h_disabled
static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvencSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_nvencsession_h_disabled
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfo(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL(pNvencSession, pParams)
#endif // __nvoc_nvencsession_h_disabled

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams);
#ifdef __nvoc_nvencsession_h_disabled
static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvencSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_nvencsession_h_disabled
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL(pNvencSession, pParams)
#endif // __nvoc_nvencsession_h_disabled


// Wrapper macros for halified functions
#define nvencsessionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define nvencsessionControl(pGpuResource, pCallContext, pParams) nvencsessionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvencsessionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define nvencsessionMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvencsessionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvencsessionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define nvencsessionUnmap(pGpuResource, pCallContext, pCpuMapping) nvencsessionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvencsessionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define nvencsessionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvencsessionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvencsessionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define nvencsessionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvencsessionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvencsessionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define nvencsessionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvencsessionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvencsessionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define nvencsessionInternalControlForward(pGpuResource, command, pParams, size) nvencsessionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvencsessionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define nvencsessionGetInternalObjectHandle(pGpuResource) nvencsessionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvencsessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define nvencsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvencsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvencsessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nvencsessionGetMemInterMapParams(pRmResource, pParams) nvencsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvencsessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nvencsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvencsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvencsessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nvencsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvencsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvencsessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define nvencsessionControlSerialization_Prologue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define nvencsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define nvencsessionControl_Prologue(pResource, pCallContext, pParams) nvencsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
static inline NV_STATUS nvencsessionControl_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvencsessionMap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvencsessionUnmap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool nvencsessionShareCallback_DISPATCH(struct NvencSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvencsessionGetRegBaseOffsetAndSize_DISPATCH(struct NvencSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS nvencsessionGetMapAddrSpace_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nvencsessionInternalControlForward_DISPATCH(struct NvencSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle nvencsessionGetInternalObjectHandle_DISPATCH(struct NvencSession *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool nvencsessionAccessCallback_DISPATCH(struct NvencSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvencsessionGetMemInterMapParams_DISPATCH(struct NvencSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS nvencsessionCheckMemInterUnmap_DISPATCH(struct NvencSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS nvencsessionGetMemoryMappingDescriptor_DISPATCH(struct NvencSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS nvencsessionControlSerialization_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvencsessionControlSerialization_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvencsessionControl_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvencsessionControl_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvencsessionCanCopy_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvencsessionIsDuplicate_DISPATCH(struct NvencSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void nvencsessionPreDestruct_DISPATCH(struct NvencSession *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvencsessionControlFilter_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvencsessionIsPartialUnmapSupported_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvencsessionMapTo_DISPATCH(struct NvencSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS nvencsessionUnmapFrom_DISPATCH(struct NvencSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 nvencsessionGetRefCount_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void nvencsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvencSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams);

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _NVENCSESSION_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NVENCSESSION_NVOC_H_
