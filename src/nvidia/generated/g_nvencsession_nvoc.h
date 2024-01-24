#ifndef _G_NVENCSESSION_NVOC_H_
#define _G_NVENCSESSION_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_nvencsession_nvoc.h"

#ifndef _NVENCSESSION_H_
#define _NVENCSESSION_H_

#include "core/core.h"
#include "rmapi/client.h"
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

struct NvencSession {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct NvencSession *__nvoc_pbase_NvencSession;
    NV_STATUS (*__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__)(struct NvencSession *, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *);
    NV_STATUS (*__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__)(struct NvencSession *, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *);
    NvBool (*__nvencsessionShareCallback__)(struct NvencSession *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__nvencsessionCheckMemInterUnmap__)(struct NvencSession *, NvBool);
    NV_STATUS (*__nvencsessionMapTo__)(struct NvencSession *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__nvencsessionGetMapAddrSpace__)(struct NvencSession *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__nvencsessionGetRefCount__)(struct NvencSession *);
    void (*__nvencsessionAddAdditionalDependants__)(struct RsClient *, struct NvencSession *, RsResourceRef *);
    NV_STATUS (*__nvencsessionControl_Prologue__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvencsessionGetRegBaseOffsetAndSize__)(struct NvencSession *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__nvencsessionInternalControlForward__)(struct NvencSession *, NvU32, void *, NvU32);
    NV_STATUS (*__nvencsessionUnmapFrom__)(struct NvencSession *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__nvencsessionControl_Epilogue__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvHandle (*__nvencsessionGetInternalObjectHandle__)(struct NvencSession *);
    NV_STATUS (*__nvencsessionControl__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvencsessionUnmap__)(struct NvencSession *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__nvencsessionGetMemInterMapParams__)(struct NvencSession *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__nvencsessionGetMemoryMappingDescriptor__)(struct NvencSession *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__nvencsessionControlFilter__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvencsessionControlSerialization_Prologue__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__nvencsessionCanCopy__)(struct NvencSession *);
    NvBool (*__nvencsessionIsPartialUnmapSupported__)(struct NvencSession *);
    void (*__nvencsessionPreDestruct__)(struct NvencSession *);
    NV_STATUS (*__nvencsessionIsDuplicate__)(struct NvencSession *, NvHandle, NvBool *);
    void (*__nvencsessionControlSerialization_Epilogue__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvencsessionMap__)(struct NvencSession *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__nvencsessionAccessCallback__)(struct NvencSession *, struct RsClient *, void *, RsAccessRight);
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

#ifndef __NVOC_CLASS_NvencSession_TYPEDEF__
#define __NVOC_CLASS_NvencSession_TYPEDEF__
typedef struct NvencSession NvencSession;
#endif /* __NVOC_CLASS_NvencSession_TYPEDEF__ */

#ifndef __nvoc_class_id_NvencSession
#define __nvoc_class_id_NvencSession 0x3434af
#endif /* __nvoc_class_id_NvencSession */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession;

#define __staticCast_NvencSession(pThis) \
    ((pThis)->__nvoc_pbase_NvencSession)

#ifdef __nvoc_nvencsession_h_disabled
#define __dynamicCast_NvencSession(pThis) ((NvencSession*)NULL)
#else //__nvoc_nvencsession_h_disabled
#define __dynamicCast_NvencSession(pThis) \
    ((NvencSession*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvencSession)))
#endif //__nvoc_nvencsession_h_disabled


NV_STATUS __nvoc_objCreateDynamic_NvencSession(NvencSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvencSession(NvencSession**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_NvencSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvencSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define nvencsessionCtrlCmdNvencSwSessionUpdateInfo(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfo_DISPATCH(pNvencSession, pParams)
#define nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2(pNvencSession, pParams) nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_DISPATCH(pNvencSession, pParams)
#define nvencsessionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvencsessionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvencsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvencsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvencsessionMapTo(pResource, pParams) nvencsessionMapTo_DISPATCH(pResource, pParams)
#define nvencsessionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvencsessionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvencsessionGetRefCount(pResource) nvencsessionGetRefCount_DISPATCH(pResource)
#define nvencsessionAddAdditionalDependants(pClient, pResource, pReference) nvencsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvencsessionControl_Prologue(pResource, pCallContext, pParams) nvencsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvencsessionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvencsessionInternalControlForward(pGpuResource, command, pParams, size) nvencsessionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvencsessionUnmapFrom(pResource, pParams) nvencsessionUnmapFrom_DISPATCH(pResource, pParams)
#define nvencsessionControl_Epilogue(pResource, pCallContext, pParams) nvencsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionGetInternalObjectHandle(pGpuResource) nvencsessionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvencsessionControl(pGpuResource, pCallContext, pParams) nvencsessionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvencsessionUnmap(pGpuResource, pCallContext, pCpuMapping) nvencsessionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvencsessionGetMemInterMapParams(pRmResource, pParams) nvencsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvencsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvencsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvencsessionControlFilter(pResource, pCallContext, pParams) nvencsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionControlSerialization_Prologue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionCanCopy(pResource) nvencsessionCanCopy_DISPATCH(pResource)
#define nvencsessionIsPartialUnmapSupported(pResource) nvencsessionIsPartialUnmapSupported_DISPATCH(pResource)
#define nvencsessionPreDestruct(pResource) nvencsessionPreDestruct_DISPATCH(pResource)
#define nvencsessionIsDuplicate(pResource, hMemory, pDuplicate) nvencsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvencsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) nvencsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvencsessionMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvencsessionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvencsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvencsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams);

static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfo_DISPATCH(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams) {
    return pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__(pNvencSession, pParams);
}

NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams);

static inline NV_STATUS nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_DISPATCH(struct NvencSession *pNvencSession, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams) {
    return pNvencSession->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__(pNvencSession, pParams);
}

static inline NvBool nvencsessionShareCallback_DISPATCH(struct NvencSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvencsessionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvencsessionCheckMemInterUnmap_DISPATCH(struct NvencSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvencsessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS nvencsessionMapTo_DISPATCH(struct NvencSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvencsessionMapTo__(pResource, pParams);
}

static inline NV_STATUS nvencsessionGetMapAddrSpace_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvencsessionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 nvencsessionGetRefCount_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvencsessionGetRefCount__(pResource);
}

static inline void nvencsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvencSession *pResource, RsResourceRef *pReference) {
    pResource->__nvencsessionAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS nvencsessionControl_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvencsessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionGetRegBaseOffsetAndSize_DISPATCH(struct NvencSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvencsessionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS nvencsessionInternalControlForward_DISPATCH(struct NvencSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvencsessionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS nvencsessionUnmapFrom_DISPATCH(struct NvencSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvencsessionUnmapFrom__(pResource, pParams);
}

static inline void nvencsessionControl_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvencsessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvHandle nvencsessionGetInternalObjectHandle_DISPATCH(struct NvencSession *pGpuResource) {
    return pGpuResource->__nvencsessionGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS nvencsessionControl_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvencsessionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionUnmap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvencsessionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS nvencsessionGetMemInterMapParams_DISPATCH(struct NvencSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvencsessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvencsessionGetMemoryMappingDescriptor_DISPATCH(struct NvencSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvencsessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS nvencsessionControlFilter_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvencsessionControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionControlSerialization_Prologue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvencsessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool nvencsessionCanCopy_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvencsessionCanCopy__(pResource);
}

static inline NvBool nvencsessionIsPartialUnmapSupported_DISPATCH(struct NvencSession *pResource) {
    return pResource->__nvencsessionIsPartialUnmapSupported__(pResource);
}

static inline void nvencsessionPreDestruct_DISPATCH(struct NvencSession *pResource) {
    pResource->__nvencsessionPreDestruct__(pResource);
}

static inline NV_STATUS nvencsessionIsDuplicate_DISPATCH(struct NvencSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvencsessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvencsessionControlSerialization_Epilogue_DISPATCH(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvencsessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvencsessionMap_DISPATCH(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvencsessionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool nvencsessionAccessCallback_DISPATCH(struct NvencSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvencsessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
