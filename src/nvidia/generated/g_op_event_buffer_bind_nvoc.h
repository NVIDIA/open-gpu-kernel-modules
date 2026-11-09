
#ifndef _G_OP_EVENT_BUFFER_BIND_NVOC_H_
#define _G_OP_EVENT_BUFFER_BIND_NVOC_H_

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

#pragma once
#include "g_op_event_buffer_bind_nvoc.h"

#ifndef OP_EVENT_BUFFER_BIND_H
#define OP_EVENT_BUFFER_BIND_H

#include "nvoc/prelude.h"
#include "nvstatus.h"
#include "nvtypes.h"
#include "resserv/resserv.h"
#include "rmapi/resource.h"
#include "containers/list.h"
#include "gpu/gpu_uuid.h"

struct EventBuffer;

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502bu
typedef struct EventBuffer EventBuffer;
#endif /* __nvoc_class_id_EventBuffer */



typedef struct OpEventBufferProducerState OpEventBufferProducerState;

//
// Install a Physical-RM notify listener on pBuf. Lazily creates op-event
// producer state on the buffer if not present, then adds one bClientRM
// EVENTNOTIFICATION + enables live-ring threshold gating for the no-drop
// publish path (eventbufferTryAddNotify).
//
// Incompatible with KEEP_NEWEST: rejects with NV_ERR_INVALID_STATE.
//
// On failure, any state freshly created by this call is torn down; any
// state that already had bind references is left untouched.
//
NV_STATUS opEventBufferInstallClientRmNotify(
    struct EventBuffer *pBuf,
    NvHandle hNotifierClient,
    NvHandle hCallbackEvent);

//
// NV_OPERATIONAL_EVENT_BUFFER_BIND resource.
// Per-client subscription handle on an existing EventBuffer. The target
// pointer is weak; producer-state destroy nulls it if the EventBuffer dies
// before the bind resource.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OP_EVENT_BUFFER_BIND_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OpEventBufferBind;
struct NVOC_METADATA__RmResource;


struct OpEventBufferBind {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OpEventBufferBind *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct OpEventBufferBind *__nvoc_pbase_OpEventBufferBind;    // opevtbufbind

    // Data members
    struct EventBuffer *pTargetEventBuffer;
    NvU8 recordFormat;
    NvU8 bindId;
    NvU8 scope;
    NvU8 minLogLevel;
    NvU8 minSeverity;
    NvU8 resourceUuid[16];
    struct ListNode bindListNode;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OpEventBufferBind {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_OpEventBufferBind
#define __nvoc_class_id_OpEventBufferBind 0x0aaa53u
typedef struct OpEventBufferBind OpEventBufferBind;
#endif /* __nvoc_class_id_OpEventBufferBind */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OpEventBufferBind;

#define __staticCast_OpEventBufferBind(pThis) \
    ((pThis)->__nvoc_pbase_OpEventBufferBind)

#ifdef __nvoc_op_event_buffer_bind_h_disabled
#define __dynamicCast_OpEventBufferBind(pThis) ((OpEventBufferBind*) NULL)
#else //__nvoc_op_event_buffer_bind_h_disabled
#define __dynamicCast_OpEventBufferBind(pThis) \
    ((OpEventBufferBind*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OpEventBufferBind)))
#endif //__nvoc_op_event_buffer_bind_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OpEventBufferBind(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OpEventBufferBind(OpEventBufferBind**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_OpEventBufferBind(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_OpEventBufferBind((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS opevtbufbindConstruct_IMPL(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_opevtbufbindConstruct(pResource, pCallContext, pParams) opevtbufbindConstruct_IMPL(pResource, pCallContext, pParams)

void opevtbufbindDestruct_IMPL(struct OpEventBufferBind *pResource);
#define __nvoc_opevtbufbindDestruct(pResource) opevtbufbindDestruct_IMPL(pResource)


// Wrapper macros for halified functions
#define opevtbufbindAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define opevtbufbindAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) opevtbufbindAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define opevtbufbindShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define opevtbufbindShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) opevtbufbindShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define opevtbufbindGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define opevtbufbindGetMemInterMapParams(pRmResource, pParams) opevtbufbindGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define opevtbufbindCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define opevtbufbindCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) opevtbufbindCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define opevtbufbindGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define opevtbufbindGetMemoryMappingDescriptor(pRmResource, ppMemDesc) opevtbufbindGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define opevtbufbindControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define opevtbufbindControlSerialization_Prologue(pResource, pCallContext, pParams) opevtbufbindControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define opevtbufbindControlSerialization_Epilogue(pResource, pCallContext, pParams) opevtbufbindControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define opevtbufbindControl_Prologue(pResource, pCallContext, pParams) opevtbufbindControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define opevtbufbindControl_Epilogue(pResource, pCallContext, pParams) opevtbufbindControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define opevtbufbindCanCopy(pResource) opevtbufbindCanCopy_DISPATCH(pResource)
#define opevtbufbindIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define opevtbufbindIsDuplicate(pResource, hMemory, pDuplicate) opevtbufbindIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define opevtbufbindPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define opevtbufbindPreDestruct(pResource) opevtbufbindPreDestruct_DISPATCH(pResource)
#define opevtbufbindControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define opevtbufbindControl(pResource, pCallContext, pParams) opevtbufbindControl_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define opevtbufbindControlFilter(pResource, pCallContext, pParams) opevtbufbindControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define opevtbufbindMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define opevtbufbindMap(pResource, pCallContext, pParams, pCpuMapping) opevtbufbindMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define opevtbufbindUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define opevtbufbindUnmap(pResource, pCallContext, pCpuMapping) opevtbufbindUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define opevtbufbindIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define opevtbufbindIsPartialUnmapSupported(pResource) opevtbufbindIsPartialUnmapSupported_DISPATCH(pResource)
#define opevtbufbindMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define opevtbufbindMapTo(pResource, pParams) opevtbufbindMapTo_DISPATCH(pResource, pParams)
#define opevtbufbindUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define opevtbufbindUnmapFrom(pResource, pParams) opevtbufbindUnmapFrom_DISPATCH(pResource, pParams)
#define opevtbufbindGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define opevtbufbindGetRefCount(pResource) opevtbufbindGetRefCount_DISPATCH(pResource)
#define opevtbufbindAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define opevtbufbindAddAdditionalDependants(pClient, pResource, pReference) opevtbufbindAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool opevtbufbindAccessCallback_DISPATCH(struct OpEventBufferBind *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool opevtbufbindShareCallback_DISPATCH(struct OpEventBufferBind *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS opevtbufbindGetMemInterMapParams_DISPATCH(struct OpEventBufferBind *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS opevtbufbindCheckMemInterUnmap_DISPATCH(struct OpEventBufferBind *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS opevtbufbindGetMemoryMappingDescriptor_DISPATCH(struct OpEventBufferBind *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS opevtbufbindControlSerialization_Prologue_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void opevtbufbindControlSerialization_Epilogue_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS opevtbufbindControl_Prologue_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void opevtbufbindControl_Epilogue_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool opevtbufbindCanCopy_DISPATCH(struct OpEventBufferBind *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS opevtbufbindIsDuplicate_DISPATCH(struct OpEventBufferBind *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void opevtbufbindPreDestruct_DISPATCH(struct OpEventBufferBind *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS opevtbufbindControl_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS opevtbufbindControlFilter_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS opevtbufbindMap_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS opevtbufbindUnmap_DISPATCH(struct OpEventBufferBind *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool opevtbufbindIsPartialUnmapSupported_DISPATCH(struct OpEventBufferBind *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS opevtbufbindMapTo_DISPATCH(struct OpEventBufferBind *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS opevtbufbindUnmapFrom_DISPATCH(struct OpEventBufferBind *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 opevtbufbindGetRefCount_DISPATCH(struct OpEventBufferBind *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void opevtbufbindAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct OpEventBufferBind *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // OP_EVENT_BUFFER_BIND_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OP_EVENT_BUFFER_BIND_NVOC_H_
