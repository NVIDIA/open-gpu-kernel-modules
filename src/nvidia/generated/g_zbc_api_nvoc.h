
#ifndef _G_ZBC_API_NVOC_H_
#define _G_ZBC_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_zbc_api_nvoc.h"

#ifndef _ZBCAPI_H_
#define _ZBCAPI_H_

#include "gpu/gpu_resource.h"
#include "gpu/gpu_halspec.h"
#include "rmapi/control.h"
#include "ctrl/ctrl9096.h"

/*!
 * RM internal class representing GF100_ZBC_CLEAR
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_ZBC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ZbcApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__ZbcApi;


struct ZbcApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ZbcApi *__nvoc_metadata_ptr;
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
    struct ZbcApi *__nvoc_pbase_ZbcApi;    // zbcapi

    // Vtable with 7 per-object function pointers
    NV_STATUS (*__zbcapiCtrlCmdSetZbcColorClear__)(struct ZbcApi * /*this*/, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *);  // exported (id=0x90960101)
    NV_STATUS (*__zbcapiCtrlCmdSetZbcDepthClear__)(struct ZbcApi * /*this*/, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *);  // exported (id=0x90960102)
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTable__)(struct ZbcApi * /*this*/, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *);  // exported (id=0x90960103)
    NV_STATUS (*__zbcapiCtrlCmdSetZbcClearTable__)(struct ZbcApi * /*this*/, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *);  // exported (id=0x90960104)
    NV_STATUS (*__zbcapiCtrlCmdSetZbcStencilClear__)(struct ZbcApi * /*this*/, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *);  // exported (id=0x90960105)
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTableSize__)(struct ZbcApi * /*this*/, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *);  // halified (2 hals) exported (id=0x90960106) body
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTableEntry__)(struct ZbcApi * /*this*/, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *);  // exported (id=0x90960107)

    // Data members
    NvBool bZbcUsed;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__ZbcApi {
    NV_STATUS (*__zbcapiControl__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__zbcapiMap__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__zbcapiUnmap__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__zbcapiShareCallback__)(struct ZbcApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__zbcapiGetRegBaseOffsetAndSize__)(struct ZbcApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__zbcapiGetMapAddrSpace__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__zbcapiInternalControlForward__)(struct ZbcApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__zbcapiGetInternalObjectHandle__)(struct ZbcApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__zbcapiAccessCallback__)(struct ZbcApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__zbcapiGetMemInterMapParams__)(struct ZbcApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__zbcapiCheckMemInterUnmap__)(struct ZbcApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__zbcapiGetMemoryMappingDescriptor__)(struct ZbcApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__zbcapiControlSerialization_Prologue__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__zbcapiControlSerialization_Epilogue__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__zbcapiControl_Prologue__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__zbcapiControl_Epilogue__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__zbcapiCanCopy__)(struct ZbcApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__zbcapiIsDuplicate__)(struct ZbcApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__zbcapiPreDestruct__)(struct ZbcApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__zbcapiControlFilter__)(struct ZbcApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__zbcapiIsPartialUnmapSupported__)(struct ZbcApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__zbcapiMapTo__)(struct ZbcApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__zbcapiUnmapFrom__)(struct ZbcApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__zbcapiGetRefCount__)(struct ZbcApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__zbcapiAddAdditionalDependants__)(struct RsClient *, struct ZbcApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ZbcApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__ZbcApi vtable;
};

#ifndef __NVOC_CLASS_ZbcApi_TYPEDEF__
#define __NVOC_CLASS_ZbcApi_TYPEDEF__
typedef struct ZbcApi ZbcApi;
#endif /* __NVOC_CLASS_ZbcApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ZbcApi
#define __nvoc_class_id_ZbcApi 0x397ee3
#endif /* __nvoc_class_id_ZbcApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ZbcApi;

#define __staticCast_ZbcApi(pThis) \
    ((pThis)->__nvoc_pbase_ZbcApi)

#ifdef __nvoc_zbc_api_h_disabled
#define __dynamicCast_ZbcApi(pThis) ((ZbcApi*) NULL)
#else //__nvoc_zbc_api_h_disabled
#define __dynamicCast_ZbcApi(pThis) \
    ((ZbcApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ZbcApi)))
#endif //__nvoc_zbc_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ZbcApi(ZbcApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ZbcApi(ZbcApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_ZbcApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ZbcApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define zbcapiCtrlCmdSetZbcColorClear_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdSetZbcColorClear__
#define zbcapiCtrlCmdSetZbcColorClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcColorClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdSetZbcDepthClear_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdSetZbcDepthClear__
#define zbcapiCtrlCmdSetZbcDepthClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcDepthClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdGetZbcClearTable_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdGetZbcClearTable__
#define zbcapiCtrlCmdGetZbcClearTable(pZbcApi, pGetZBCClearTableParams) zbcapiCtrlCmdGetZbcClearTable_DISPATCH(pZbcApi, pGetZBCClearTableParams)
#define zbcapiCtrlCmdSetZbcClearTable_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdSetZbcClearTable__
#define zbcapiCtrlCmdSetZbcClearTable(pZbcApi, pSetZBCClearTableParams) zbcapiCtrlCmdSetZbcClearTable_DISPATCH(pZbcApi, pSetZBCClearTableParams)
#define zbcapiCtrlCmdSetZbcStencilClear_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdSetZbcStencilClear__
#define zbcapiCtrlCmdSetZbcStencilClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcStencilClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdGetZbcClearTableSize_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdGetZbcClearTableSize__
#define zbcapiCtrlCmdGetZbcClearTableSize(pZbcApi, pGetZBCClearTableSizeParams) zbcapiCtrlCmdGetZbcClearTableSize_DISPATCH(pZbcApi, pGetZBCClearTableSizeParams)
#define zbcapiCtrlCmdGetZbcClearTableSize_HAL(pZbcApi, pGetZBCClearTableSizeParams) zbcapiCtrlCmdGetZbcClearTableSize_DISPATCH(pZbcApi, pGetZBCClearTableSizeParams)
#define zbcapiCtrlCmdGetZbcClearTableEntry_FNPTR(pZbcApi) pZbcApi->__zbcapiCtrlCmdGetZbcClearTableEntry__
#define zbcapiCtrlCmdGetZbcClearTableEntry(pZbcApi, pGetZBCClearTableEntryParams) zbcapiCtrlCmdGetZbcClearTableEntry_DISPATCH(pZbcApi, pGetZBCClearTableEntryParams)
#define zbcapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define zbcapiControl(pGpuResource, pCallContext, pParams) zbcapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define zbcapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define zbcapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) zbcapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define zbcapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define zbcapiUnmap(pGpuResource, pCallContext, pCpuMapping) zbcapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define zbcapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define zbcapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) zbcapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define zbcapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define zbcapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) zbcapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define zbcapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define zbcapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) zbcapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define zbcapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define zbcapiInternalControlForward(pGpuResource, command, pParams, size) zbcapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define zbcapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define zbcapiGetInternalObjectHandle(pGpuResource) zbcapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define zbcapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define zbcapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) zbcapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define zbcapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define zbcapiGetMemInterMapParams(pRmResource, pParams) zbcapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define zbcapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define zbcapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) zbcapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define zbcapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define zbcapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) zbcapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define zbcapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define zbcapiControlSerialization_Prologue(pResource, pCallContext, pParams) zbcapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define zbcapiControlSerialization_Epilogue(pResource, pCallContext, pParams) zbcapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define zbcapiControl_Prologue(pResource, pCallContext, pParams) zbcapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define zbcapiControl_Epilogue(pResource, pCallContext, pParams) zbcapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define zbcapiCanCopy(pResource) zbcapiCanCopy_DISPATCH(pResource)
#define zbcapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define zbcapiIsDuplicate(pResource, hMemory, pDuplicate) zbcapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define zbcapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define zbcapiPreDestruct(pResource) zbcapiPreDestruct_DISPATCH(pResource)
#define zbcapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define zbcapiControlFilter(pResource, pCallContext, pParams) zbcapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define zbcapiIsPartialUnmapSupported(pResource) zbcapiIsPartialUnmapSupported_DISPATCH(pResource)
#define zbcapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define zbcapiMapTo(pResource, pParams) zbcapiMapTo_DISPATCH(pResource, pParams)
#define zbcapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define zbcapiUnmapFrom(pResource, pParams) zbcapiUnmapFrom_DISPATCH(pResource, pParams)
#define zbcapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define zbcapiGetRefCount(pResource) zbcapiGetRefCount_DISPATCH(pResource)
#define zbcapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define zbcapiAddAdditionalDependants(pClient, pResource, pReference) zbcapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS zbcapiCtrlCmdSetZbcColorClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcColorClear__(pZbcApi, pSetZBCClearParams);
}

static inline NV_STATUS zbcapiCtrlCmdSetZbcDepthClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcDepthClear__(pZbcApi, pSetZBCClearParams);
}

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTable_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pGetZBCClearTableParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTable__(pZbcApi, pGetZBCClearTableParams);
}

static inline NV_STATUS zbcapiCtrlCmdSetZbcClearTable_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *pSetZBCClearTableParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcClearTable__(pZbcApi, pSetZBCClearTableParams);
}

static inline NV_STATUS zbcapiCtrlCmdSetZbcStencilClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcStencilClear__(pZbcApi, pSetZBCClearParams);
}

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTableSize_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTableSize__(pZbcApi, pGetZBCClearTableSizeParams);
}

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTableEntry_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pGetZBCClearTableEntryParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTableEntry__(pZbcApi, pGetZBCClearTableEntryParams);
}

static inline NV_STATUS zbcapiControl_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiMap_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS zbcapiUnmap_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool zbcapiShareCallback_DISPATCH(struct ZbcApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS zbcapiGetRegBaseOffsetAndSize_DISPATCH(struct ZbcApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS zbcapiGetMapAddrSpace_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS zbcapiInternalControlForward_DISPATCH(struct ZbcApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle zbcapiGetInternalObjectHandle_DISPATCH(struct ZbcApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__zbcapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool zbcapiAccessCallback_DISPATCH(struct ZbcApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS zbcapiGetMemInterMapParams_DISPATCH(struct ZbcApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__zbcapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS zbcapiCheckMemInterUnmap_DISPATCH(struct ZbcApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__zbcapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS zbcapiGetMemoryMappingDescriptor_DISPATCH(struct ZbcApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__zbcapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS zbcapiControlSerialization_Prologue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void zbcapiControlSerialization_Epilogue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__zbcapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiControl_Prologue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void zbcapiControl_Epilogue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__zbcapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool zbcapiCanCopy_DISPATCH(struct ZbcApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiCanCopy__(pResource);
}

static inline NV_STATUS zbcapiIsDuplicate_DISPATCH(struct ZbcApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void zbcapiPreDestruct_DISPATCH(struct ZbcApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__zbcapiPreDestruct__(pResource);
}

static inline NV_STATUS zbcapiControlFilter_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool zbcapiIsPartialUnmapSupported_DISPATCH(struct ZbcApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS zbcapiMapTo_DISPATCH(struct ZbcApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiMapTo__(pResource, pParams);
}

static inline NV_STATUS zbcapiUnmapFrom_DISPATCH(struct ZbcApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 zbcapiGetRefCount_DISPATCH(struct ZbcApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__zbcapiGetRefCount__(pResource);
}

static inline void zbcapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ZbcApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__zbcapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS zbcapiConstructHal_56cd7a(struct ZbcApi *pZbcApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}

NV_STATUS zbcapiConstructHal_IMPL(struct ZbcApi *pZbcApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_zbc_api_h_disabled
static inline NV_STATUS zbcapiConstructHal(struct ZbcApi *pZbcApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ZbcApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_zbc_api_h_disabled
#define zbcapiConstructHal(pZbcApi, pCallContext, pParams) zbcapiConstructHal_56cd7a(pZbcApi, pCallContext, pParams)
#endif //__nvoc_zbc_api_h_disabled

#define zbcapiConstructHal_HAL(pZbcApi, pCallContext, pParams) zbcapiConstructHal(pZbcApi, pCallContext, pParams)

static inline void zbcapiDestruct_b3696a(struct ZbcApi *pZbcApi) {
    return;
}

void zbcapiDestruct_IMPL(struct ZbcApi *pZbcApi);


#define __nvoc_zbcapiDestruct(pZbcApi) zbcapiDestruct_b3696a(pZbcApi)
NV_STATUS zbcapiCtrlCmdSetZbcColorClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pSetZBCClearParams);

NV_STATUS zbcapiCtrlCmdSetZbcDepthClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pSetZBCClearParams);

NV_STATUS zbcapiCtrlCmdGetZbcClearTable_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pGetZBCClearTableParams);

NV_STATUS zbcapiCtrlCmdSetZbcClearTable_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *pSetZBCClearTableParams);

NV_STATUS zbcapiCtrlCmdSetZbcStencilClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pSetZBCClearParams);

NV_STATUS zbcapiCtrlCmdGetZbcClearTableSize_VF(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams);

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTableSize_5baef9(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS zbcapiCtrlCmdGetZbcClearTableEntry_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pGetZBCClearTableEntryParams);

static inline NV_STATUS __nvoc_zbcapiConstruct(struct ZbcApi *arg_pZbcApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return zbcapiConstructHal(arg_pZbcApi, arg_pCallContext, arg_pParams);
}

#undef PRIVATE_FIELD


#endif // _ZBCAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ZBC_API_NVOC_H_
