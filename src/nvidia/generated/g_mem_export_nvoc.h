#ifndef _G_MEM_EXPORT_NVOC_H_
#define _G_MEM_EXPORT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions for exporting memory
 *
 *****************************************************************************/

#include "g_mem_export_nvoc.h"

#ifndef _MEMORYEXPORT_H_
#define _MEMORYEXPORT_H_

#include "core/core.h"
#include "rmapi/resource.h"

#include "class/cl00e0.h"
#include "ctrl/ctrl00e0.h"

// ****************************************************************************
//  Type Definitions
// ****************************************************************************

typedef struct mem_export_info MEM_EXPORT_INFO;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_EXPORT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif

struct MemoryExport {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct MemoryExport *__nvoc_pbase_MemoryExport;
    NvBool (*__memoryexportCanCopy__)(struct MemoryExport *);
    NV_STATUS (*__memoryexportControl__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryexportCtrlExportMem__)(struct MemoryExport *, NV00E0_CTRL_EXPORT_MEM_PARAMS *);
    NV_STATUS (*__memoryexportCtrlImportMem__)(struct MemoryExport *, NV00E0_CTRL_IMPORT_MEM_PARAMS *);
    NV_STATUS (*__memoryexportCtrlGetInfo__)(struct MemoryExport *, NV00E0_CTRL_GET_INFO_PARAMS *);
    NvBool (*__memoryexportShareCallback__)(struct MemoryExport *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memoryexportCheckMemInterUnmap__)(struct MemoryExport *, NvBool);
    NV_STATUS (*__memoryexportGetMemInterMapParams__)(struct MemoryExport *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memoryexportGetMemoryMappingDescriptor__)(struct MemoryExport *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__memoryexportGetRefCount__)(struct MemoryExport *);
    NV_STATUS (*__memoryexportControlFilter__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__memoryexportAddAdditionalDependants__)(struct RsClient *, struct MemoryExport *, RsResourceRef *);
    NV_STATUS (*__memoryexportControlSerialization_Prologue__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryexportControl_Prologue__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryexportUnmap__)(struct MemoryExport *, struct CALL_CONTEXT *, RsCpuMapping *);
    NvBool (*__memoryexportIsPartialUnmapSupported__)(struct MemoryExport *);
    void (*__memoryexportPreDestruct__)(struct MemoryExport *);
    NV_STATUS (*__memoryexportMapTo__)(struct MemoryExport *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memoryexportIsDuplicate__)(struct MemoryExport *, NvHandle, NvBool *);
    void (*__memoryexportControlSerialization_Epilogue__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__memoryexportControl_Epilogue__)(struct MemoryExport *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryexportUnmapFrom__)(struct MemoryExport *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__memoryexportMap__)(struct MemoryExport *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__memoryexportAccessCallback__)(struct MemoryExport *, struct RsClient *, void *, RsAccessRight);
    MEM_EXPORT_INFO *PRIVATE_FIELD(pExportInfo);
};

#ifndef __NVOC_CLASS_MemoryExport_TYPEDEF__
#define __NVOC_CLASS_MemoryExport_TYPEDEF__
typedef struct MemoryExport MemoryExport;
#endif /* __NVOC_CLASS_MemoryExport_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryExport
#define __nvoc_class_id_MemoryExport 0xe7ac53
#endif /* __nvoc_class_id_MemoryExport */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryExport;

#define __staticCast_MemoryExport(pThis) \
    ((pThis)->__nvoc_pbase_MemoryExport)

#ifdef __nvoc_mem_export_h_disabled
#define __dynamicCast_MemoryExport(pThis) ((MemoryExport*)NULL)
#else //__nvoc_mem_export_h_disabled
#define __dynamicCast_MemoryExport(pThis) \
    ((MemoryExport*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryExport)))
#endif //__nvoc_mem_export_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryExport(MemoryExport**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryExport(MemoryExport**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryExport(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryExport((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memoryexportCanCopy(pMemoryExport) memoryexportCanCopy_DISPATCH(pMemoryExport)
#define memoryexportControl(pMemoryExport, pCallContext, pParams) memoryexportControl_DISPATCH(pMemoryExport, pCallContext, pParams)
#define memoryexportCtrlExportMem(pMemoryExport, pParams) memoryexportCtrlExportMem_DISPATCH(pMemoryExport, pParams)
#define memoryexportCtrlImportMem(pMemoryExport, pParams) memoryexportCtrlImportMem_DISPATCH(pMemoryExport, pParams)
#define memoryexportCtrlGetInfo(pMemoryExport, pParams) memoryexportCtrlGetInfo_DISPATCH(pMemoryExport, pParams)
#define memoryexportShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryexportShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryexportCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memoryexportCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memoryexportGetMemInterMapParams(pRmResource, pParams) memoryexportGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memoryexportGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memoryexportGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memoryexportGetRefCount(pResource) memoryexportGetRefCount_DISPATCH(pResource)
#define memoryexportControlFilter(pResource, pCallContext, pParams) memoryexportControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memoryexportAddAdditionalDependants(pClient, pResource, pReference) memoryexportAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memoryexportControlSerialization_Prologue(pResource, pCallContext, pParams) memoryexportControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryexportControl_Prologue(pResource, pCallContext, pParams) memoryexportControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryexportUnmap(pResource, pCallContext, pCpuMapping) memoryexportUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define memoryexportIsPartialUnmapSupported(pResource) memoryexportIsPartialUnmapSupported_DISPATCH(pResource)
#define memoryexportPreDestruct(pResource) memoryexportPreDestruct_DISPATCH(pResource)
#define memoryexportMapTo(pResource, pParams) memoryexportMapTo_DISPATCH(pResource, pParams)
#define memoryexportIsDuplicate(pResource, hMemory, pDuplicate) memoryexportIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define memoryexportControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryexportControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryexportControl_Epilogue(pResource, pCallContext, pParams) memoryexportControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryexportUnmapFrom(pResource, pParams) memoryexportUnmapFrom_DISPATCH(pResource, pParams)
#define memoryexportMap(pResource, pCallContext, pParams, pCpuMapping) memoryexportMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define memoryexportAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryexportAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool memoryexportCanCopy_IMPL(struct MemoryExport *pMemoryExport);

static inline NvBool memoryexportCanCopy_DISPATCH(struct MemoryExport *pMemoryExport) {
    return pMemoryExport->__memoryexportCanCopy__(pMemoryExport);
}

NV_STATUS memoryexportControl_IMPL(struct MemoryExport *pMemoryExport, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memoryexportControl_DISPATCH(struct MemoryExport *pMemoryExport, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryExport->__memoryexportControl__(pMemoryExport, pCallContext, pParams);
}

NV_STATUS memoryexportCtrlExportMem_IMPL(struct MemoryExport *pMemoryExport, NV00E0_CTRL_EXPORT_MEM_PARAMS *pParams);

static inline NV_STATUS memoryexportCtrlExportMem_DISPATCH(struct MemoryExport *pMemoryExport, NV00E0_CTRL_EXPORT_MEM_PARAMS *pParams) {
    return pMemoryExport->__memoryexportCtrlExportMem__(pMemoryExport, pParams);
}

NV_STATUS memoryexportCtrlImportMem_IMPL(struct MemoryExport *pMemoryExport, NV00E0_CTRL_IMPORT_MEM_PARAMS *pParams);

static inline NV_STATUS memoryexportCtrlImportMem_DISPATCH(struct MemoryExport *pMemoryExport, NV00E0_CTRL_IMPORT_MEM_PARAMS *pParams) {
    return pMemoryExport->__memoryexportCtrlImportMem__(pMemoryExport, pParams);
}

NV_STATUS memoryexportCtrlGetInfo_IMPL(struct MemoryExport *pMemoryExport, NV00E0_CTRL_GET_INFO_PARAMS *pParams);

static inline NV_STATUS memoryexportCtrlGetInfo_DISPATCH(struct MemoryExport *pMemoryExport, NV00E0_CTRL_GET_INFO_PARAMS *pParams) {
    return pMemoryExport->__memoryexportCtrlGetInfo__(pMemoryExport, pParams);
}

static inline NvBool memoryexportShareCallback_DISPATCH(struct MemoryExport *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memoryexportShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryexportCheckMemInterUnmap_DISPATCH(struct MemoryExport *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__memoryexportCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS memoryexportGetMemInterMapParams_DISPATCH(struct MemoryExport *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__memoryexportGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS memoryexportGetMemoryMappingDescriptor_DISPATCH(struct MemoryExport *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__memoryexportGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 memoryexportGetRefCount_DISPATCH(struct MemoryExport *pResource) {
    return pResource->__memoryexportGetRefCount__(pResource);
}

static inline NV_STATUS memoryexportControlFilter_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryexportControlFilter__(pResource, pCallContext, pParams);
}

static inline void memoryexportAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryExport *pResource, RsResourceRef *pReference) {
    pResource->__memoryexportAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS memoryexportControlSerialization_Prologue_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryexportControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryexportControl_Prologue_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryexportControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryexportUnmap_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__memoryexportUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool memoryexportIsPartialUnmapSupported_DISPATCH(struct MemoryExport *pResource) {
    return pResource->__memoryexportIsPartialUnmapSupported__(pResource);
}

static inline void memoryexportPreDestruct_DISPATCH(struct MemoryExport *pResource) {
    pResource->__memoryexportPreDestruct__(pResource);
}

static inline NV_STATUS memoryexportMapTo_DISPATCH(struct MemoryExport *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memoryexportMapTo__(pResource, pParams);
}

static inline NV_STATUS memoryexportIsDuplicate_DISPATCH(struct MemoryExport *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__memoryexportIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void memoryexportControlSerialization_Epilogue_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryexportControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void memoryexportControl_Epilogue_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryexportControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryexportUnmapFrom_DISPATCH(struct MemoryExport *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memoryexportUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS memoryexportMap_DISPATCH(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__memoryexportMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memoryexportAccessCallback_DISPATCH(struct MemoryExport *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memoryexportAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS memoryexportConstruct_IMPL(struct MemoryExport *arg_pMemoryExport, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memoryexportConstruct(arg_pMemoryExport, arg_pCallContext, arg_pParams) memoryexportConstruct_IMPL(arg_pMemoryExport, arg_pCallContext, arg_pParams)
void memoryexportDestruct_IMPL(struct MemoryExport *pMemoryExport);

#define __nvoc_memoryexportDestruct(pMemoryExport) memoryexportDestruct_IMPL(pMemoryExport)
#undef PRIVATE_FIELD


void memoryexportClearCache(NvU16 nodeId);
NvU16 memoryExportGetNodeId(NV_EXPORT_MEM_PACKET *pExportPacket);

#endif /* _MEMORYEXPORT_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_EXPORT_NVOC_H_
