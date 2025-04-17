
#ifndef _G_CONSOLE_MEM_NVOC_H_
#define _G_CONSOLE_MEM_NVOC_H_

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

#pragma once
#include "g_console_mem_nvoc.h"

#ifndef _CONSOLE_MEMORY_H_
#define _CONSOLE_MEMORY_H_

#include "mem_mgr/mem.h"
#include "ctrl/ctrl0076.h"

/*!
 * This class is used to create hMemory referencing reserved console memory
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CONSOLE_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ConsoleMemory;
struct NVOC_METADATA__Memory;
struct NVOC_VTABLE__ConsoleMemory;


struct ConsoleMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ConsoleMemory *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Memory __nvoc_base_Memory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct Memory *__nvoc_pbase_Memory;    // mem super
    struct ConsoleMemory *__nvoc_pbase_ConsoleMemory;    // conmem

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__conmemCtrlCmdNotifyConsoleDisabled__)(struct ConsoleMemory * /*this*/);  // exported (id=0x760101)
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__ConsoleMemory {
    NvBool (*__conmemCanCopy__)(struct ConsoleMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__conmemIsDuplicate__)(struct ConsoleMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemGetMapAddrSpace__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemControl__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemMap__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemUnmap__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemGetMemInterMapParams__)(struct ConsoleMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemCheckMemInterUnmap__)(struct ConsoleMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__conmemGetMemoryMappingDescriptor__)(struct ConsoleMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__conmemCheckCopyPermissions__)(struct ConsoleMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__conmemIsReady__)(struct ConsoleMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__conmemIsGpuMapAllowed__)(struct ConsoleMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__conmemIsExportAllowed__)(struct ConsoleMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__conmemAccessCallback__)(struct ConsoleMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__conmemShareCallback__)(struct ConsoleMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__conmemControlSerialization_Prologue__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__conmemControlSerialization_Epilogue__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__conmemControl_Prologue__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__conmemControl_Epilogue__)(struct ConsoleMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__conmemPreDestruct__)(struct ConsoleMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__conmemControlFilter__)(struct ConsoleMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__conmemIsPartialUnmapSupported__)(struct ConsoleMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__conmemMapTo__)(struct ConsoleMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__conmemUnmapFrom__)(struct ConsoleMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__conmemGetRefCount__)(struct ConsoleMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__conmemAddAdditionalDependants__)(struct RsClient *, struct ConsoleMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ConsoleMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
    const struct NVOC_VTABLE__ConsoleMemory vtable;
};

#ifndef __NVOC_CLASS_ConsoleMemory_TYPEDEF__
#define __NVOC_CLASS_ConsoleMemory_TYPEDEF__
typedef struct ConsoleMemory ConsoleMemory;
#endif /* __NVOC_CLASS_ConsoleMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ConsoleMemory
#define __nvoc_class_id_ConsoleMemory 0xaac69e
#endif /* __nvoc_class_id_ConsoleMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory;

#define __staticCast_ConsoleMemory(pThis) \
    ((pThis)->__nvoc_pbase_ConsoleMemory)

#ifdef __nvoc_console_mem_h_disabled
#define __dynamicCast_ConsoleMemory(pThis) ((ConsoleMemory*) NULL)
#else //__nvoc_console_mem_h_disabled
#define __dynamicCast_ConsoleMemory(pThis) \
    ((ConsoleMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ConsoleMemory)))
#endif //__nvoc_console_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ConsoleMemory(ConsoleMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ConsoleMemory(ConsoleMemory**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_ConsoleMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ConsoleMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define conmemCtrlCmdNotifyConsoleDisabled_FNPTR(pConsoleMemory) pConsoleMemory->__conmemCtrlCmdNotifyConsoleDisabled__
#define conmemCtrlCmdNotifyConsoleDisabled(pConsoleMemory) conmemCtrlCmdNotifyConsoleDisabled_DISPATCH(pConsoleMemory)
#define conmemCanCopy_FNPTR(pConsoleMemory) pConsoleMemory->__nvoc_metadata_ptr->vtable.__conmemCanCopy__
#define conmemCanCopy(pConsoleMemory) conmemCanCopy_DISPATCH(pConsoleMemory)
#define conmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define conmemIsDuplicate(pMemory, hMemory, pDuplicate) conmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define conmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define conmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) conmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define conmemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define conmemControl(pMemory, pCallContext, pParams) conmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define conmemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define conmemMap(pMemory, pCallContext, pParams, pCpuMapping) conmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define conmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define conmemUnmap(pMemory, pCallContext, pCpuMapping) conmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define conmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define conmemGetMemInterMapParams(pMemory, pParams) conmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define conmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define conmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) conmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define conmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define conmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) conmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define conmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define conmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) conmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define conmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define conmemIsReady(pMemory, bCopyConstructorContext) conmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define conmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define conmemIsGpuMapAllowed(pMemory, pGpu) conmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define conmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define conmemIsExportAllowed(pMemory) conmemIsExportAllowed_DISPATCH(pMemory)
#define conmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define conmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) conmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define conmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define conmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) conmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define conmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define conmemControlSerialization_Prologue(pResource, pCallContext, pParams) conmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define conmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define conmemControlSerialization_Epilogue(pResource, pCallContext, pParams) conmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define conmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define conmemControl_Prologue(pResource, pCallContext, pParams) conmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define conmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define conmemControl_Epilogue(pResource, pCallContext, pParams) conmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define conmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define conmemPreDestruct(pResource) conmemPreDestruct_DISPATCH(pResource)
#define conmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define conmemControlFilter(pResource, pCallContext, pParams) conmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define conmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define conmemIsPartialUnmapSupported(pResource) conmemIsPartialUnmapSupported_DISPATCH(pResource)
#define conmemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define conmemMapTo(pResource, pParams) conmemMapTo_DISPATCH(pResource, pParams)
#define conmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define conmemUnmapFrom(pResource, pParams) conmemUnmapFrom_DISPATCH(pResource, pParams)
#define conmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define conmemGetRefCount(pResource) conmemGetRefCount_DISPATCH(pResource)
#define conmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define conmemAddAdditionalDependants(pClient, pResource, pReference) conmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS conmemCtrlCmdNotifyConsoleDisabled_DISPATCH(struct ConsoleMemory *pConsoleMemory) {
    return pConsoleMemory->__conmemCtrlCmdNotifyConsoleDisabled__(pConsoleMemory);
}

static inline NvBool conmemCanCopy_DISPATCH(struct ConsoleMemory *pConsoleMemory) {
    return pConsoleMemory->__nvoc_metadata_ptr->vtable.__conmemCanCopy__(pConsoleMemory);
}

static inline NV_STATUS conmemIsDuplicate_DISPATCH(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS conmemGetMapAddrSpace_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS conmemControl_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS conmemMap_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS conmemUnmap_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS conmemGetMemInterMapParams_DISPATCH(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS conmemCheckMemInterUnmap_DISPATCH(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS conmemGetMemoryMappingDescriptor_DISPATCH(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS conmemCheckCopyPermissions_DISPATCH(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS conmemIsReady_DISPATCH(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool conmemIsGpuMapAllowed_DISPATCH(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool conmemIsExportAllowed_DISPATCH(struct ConsoleMemory *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__conmemIsExportAllowed__(pMemory);
}

static inline NvBool conmemAccessCallback_DISPATCH(struct ConsoleMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool conmemShareCallback_DISPATCH(struct ConsoleMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS conmemControlSerialization_Prologue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void conmemControlSerialization_Epilogue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__conmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS conmemControl_Prologue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void conmemControl_Epilogue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__conmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void conmemPreDestruct_DISPATCH(struct ConsoleMemory *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__conmemPreDestruct__(pResource);
}

static inline NV_STATUS conmemControlFilter_DISPATCH(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool conmemIsPartialUnmapSupported_DISPATCH(struct ConsoleMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS conmemMapTo_DISPATCH(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemMapTo__(pResource, pParams);
}

static inline NV_STATUS conmemUnmapFrom_DISPATCH(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 conmemGetRefCount_DISPATCH(struct ConsoleMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__conmemGetRefCount__(pResource);
}

static inline void conmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__conmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS conmemCtrlCmdNotifyConsoleDisabled_IMPL(struct ConsoleMemory *pConsoleMemory);

NvBool conmemCanCopy_IMPL(struct ConsoleMemory *pConsoleMemory);

NV_STATUS conmemConstruct_IMPL(struct ConsoleMemory *arg_pConsoleMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_conmemConstruct(arg_pConsoleMemory, arg_pCallContext, arg_pParams) conmemConstruct_IMPL(arg_pConsoleMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONSOLE_MEM_NVOC_H_
