
#ifndef _G_I2C_API_NVOC_H_
#define _G_I2C_API_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_i2c_api_nvoc.h"

#ifndef _I2CAPI_H_
#define _I2CAPI_H_

#include "gpu/gpu_resource.h"

#include "ctrl/ctrl402c.h"

/*!
 * RM internal class representing NV40_I2C (child of SubDevice)
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_I2C_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct I2cApi {

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
    struct I2cApi *__nvoc_pbase_I2cApi;    // i2capi

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__i2capiCtrlCmdI2cGetPortInfo__)(struct I2cApi * /*this*/, NV402C_CTRL_I2C_GET_PORT_INFO_PARAMS *);  // exported (id=0x402c0101)
    NV_STATUS (*__i2capiCtrlCmdI2cIndexed__)(struct I2cApi * /*this*/, NV402C_CTRL_I2C_INDEXED_PARAMS *);  // exported (id=0x402c0102)
    NV_STATUS (*__i2capiCtrlCmdI2cGetPortSpeed__)(struct I2cApi * /*this*/, NV402C_CTRL_I2C_GET_PORT_SPEED_PARAMS *);  // exported (id=0x402c0103)
    NV_STATUS (*__i2capiCtrlCmdI2cTableGetDevInfo__)(struct I2cApi * /*this*/, NV402C_CTRL_I2C_TABLE_GET_DEV_INFO_PARAMS *);  // exported (id=0x402c0104)
    NV_STATUS (*__i2capiCtrlCmdI2cTransaction__)(struct I2cApi * /*this*/, NV402C_CTRL_I2C_TRANSACTION_PARAMS *);  // exported (id=0x402c0105)
    NV_STATUS (*__i2capiControl__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__i2capiMap__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__i2capiUnmap__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__i2capiShareCallback__)(struct I2cApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__i2capiGetRegBaseOffsetAndSize__)(struct I2cApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__i2capiGetMapAddrSpace__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__i2capiInternalControlForward__)(struct I2cApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__i2capiGetInternalObjectHandle__)(struct I2cApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__i2capiAccessCallback__)(struct I2cApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__i2capiGetMemInterMapParams__)(struct I2cApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__i2capiCheckMemInterUnmap__)(struct I2cApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__i2capiGetMemoryMappingDescriptor__)(struct I2cApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__i2capiControlSerialization_Prologue__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__i2capiControlSerialization_Epilogue__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__i2capiControl_Prologue__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__i2capiControl_Epilogue__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__i2capiCanCopy__)(struct I2cApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__i2capiIsDuplicate__)(struct I2cApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__i2capiPreDestruct__)(struct I2cApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__i2capiControlFilter__)(struct I2cApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__i2capiIsPartialUnmapSupported__)(struct I2cApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__i2capiMapTo__)(struct I2cApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__i2capiUnmapFrom__)(struct I2cApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__i2capiGetRefCount__)(struct I2cApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__i2capiAddAdditionalDependants__)(struct RsClient *, struct I2cApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

#ifndef __NVOC_CLASS_I2cApi_TYPEDEF__
#define __NVOC_CLASS_I2cApi_TYPEDEF__
typedef struct I2cApi I2cApi;
#endif /* __NVOC_CLASS_I2cApi_TYPEDEF__ */

#ifndef __nvoc_class_id_I2cApi
#define __nvoc_class_id_I2cApi 0xceb8f6
#endif /* __nvoc_class_id_I2cApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_I2cApi;

#define __staticCast_I2cApi(pThis) \
    ((pThis)->__nvoc_pbase_I2cApi)

#ifdef __nvoc_i2c_api_h_disabled
#define __dynamicCast_I2cApi(pThis) ((I2cApi*)NULL)
#else //__nvoc_i2c_api_h_disabled
#define __dynamicCast_I2cApi(pThis) \
    ((I2cApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(I2cApi)))
#endif //__nvoc_i2c_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_I2cApi(I2cApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_I2cApi(I2cApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_I2cApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_I2cApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define i2capiCtrlCmdI2cGetPortInfo_FNPTR(pI2cApi) pI2cApi->__i2capiCtrlCmdI2cGetPortInfo__
#define i2capiCtrlCmdI2cGetPortInfo(pI2cApi, pParams) i2capiCtrlCmdI2cGetPortInfo_DISPATCH(pI2cApi, pParams)
#define i2capiCtrlCmdI2cIndexed_FNPTR(pI2cApi) pI2cApi->__i2capiCtrlCmdI2cIndexed__
#define i2capiCtrlCmdI2cIndexed(pI2cApi, pParams) i2capiCtrlCmdI2cIndexed_DISPATCH(pI2cApi, pParams)
#define i2capiCtrlCmdI2cGetPortSpeed_FNPTR(pI2cApi) pI2cApi->__i2capiCtrlCmdI2cGetPortSpeed__
#define i2capiCtrlCmdI2cGetPortSpeed(pI2cApi, pParams) i2capiCtrlCmdI2cGetPortSpeed_DISPATCH(pI2cApi, pParams)
#define i2capiCtrlCmdI2cTableGetDevInfo_FNPTR(pI2cApi) pI2cApi->__i2capiCtrlCmdI2cTableGetDevInfo__
#define i2capiCtrlCmdI2cTableGetDevInfo(pI2cApi, pParams) i2capiCtrlCmdI2cTableGetDevInfo_DISPATCH(pI2cApi, pParams)
#define i2capiCtrlCmdI2cTransaction_FNPTR(pI2cApi) pI2cApi->__i2capiCtrlCmdI2cTransaction__
#define i2capiCtrlCmdI2cTransaction(pI2cApi, pParams) i2capiCtrlCmdI2cTransaction_DISPATCH(pI2cApi, pParams)
#define i2capiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define i2capiControl(pGpuResource, pCallContext, pParams) i2capiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define i2capiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define i2capiMap(pGpuResource, pCallContext, pParams, pCpuMapping) i2capiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define i2capiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define i2capiUnmap(pGpuResource, pCallContext, pCpuMapping) i2capiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define i2capiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define i2capiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) i2capiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define i2capiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define i2capiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) i2capiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define i2capiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define i2capiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) i2capiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define i2capiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define i2capiInternalControlForward(pGpuResource, command, pParams, size) i2capiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define i2capiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define i2capiGetInternalObjectHandle(pGpuResource) i2capiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define i2capiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define i2capiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) i2capiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define i2capiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define i2capiGetMemInterMapParams(pRmResource, pParams) i2capiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define i2capiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define i2capiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) i2capiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define i2capiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define i2capiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) i2capiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define i2capiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define i2capiControlSerialization_Prologue(pResource, pCallContext, pParams) i2capiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define i2capiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define i2capiControlSerialization_Epilogue(pResource, pCallContext, pParams) i2capiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define i2capiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define i2capiControl_Prologue(pResource, pCallContext, pParams) i2capiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define i2capiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define i2capiControl_Epilogue(pResource, pCallContext, pParams) i2capiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define i2capiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define i2capiCanCopy(pResource) i2capiCanCopy_DISPATCH(pResource)
#define i2capiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define i2capiIsDuplicate(pResource, hMemory, pDuplicate) i2capiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define i2capiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define i2capiPreDestruct(pResource) i2capiPreDestruct_DISPATCH(pResource)
#define i2capiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define i2capiControlFilter(pResource, pCallContext, pParams) i2capiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define i2capiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define i2capiIsPartialUnmapSupported(pResource) i2capiIsPartialUnmapSupported_DISPATCH(pResource)
#define i2capiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define i2capiMapTo(pResource, pParams) i2capiMapTo_DISPATCH(pResource, pParams)
#define i2capiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define i2capiUnmapFrom(pResource, pParams) i2capiUnmapFrom_DISPATCH(pResource, pParams)
#define i2capiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define i2capiGetRefCount(pResource) i2capiGetRefCount_DISPATCH(pResource)
#define i2capiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define i2capiAddAdditionalDependants(pClient, pResource, pReference) i2capiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS i2capiCtrlCmdI2cGetPortInfo_DISPATCH(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_GET_PORT_INFO_PARAMS *pParams) {
    return pI2cApi->__i2capiCtrlCmdI2cGetPortInfo__(pI2cApi, pParams);
}

static inline NV_STATUS i2capiCtrlCmdI2cIndexed_DISPATCH(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_INDEXED_PARAMS *pParams) {
    return pI2cApi->__i2capiCtrlCmdI2cIndexed__(pI2cApi, pParams);
}

static inline NV_STATUS i2capiCtrlCmdI2cGetPortSpeed_DISPATCH(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_GET_PORT_SPEED_PARAMS *pParams) {
    return pI2cApi->__i2capiCtrlCmdI2cGetPortSpeed__(pI2cApi, pParams);
}

static inline NV_STATUS i2capiCtrlCmdI2cTableGetDevInfo_DISPATCH(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_TABLE_GET_DEV_INFO_PARAMS *pParams) {
    return pI2cApi->__i2capiCtrlCmdI2cTableGetDevInfo__(pI2cApi, pParams);
}

static inline NV_STATUS i2capiCtrlCmdI2cTransaction_DISPATCH(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_TRANSACTION_PARAMS *pParams) {
    return pI2cApi->__i2capiCtrlCmdI2cTransaction__(pI2cApi, pParams);
}

static inline NV_STATUS i2capiControl_DISPATCH(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__i2capiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS i2capiMap_DISPATCH(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__i2capiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS i2capiUnmap_DISPATCH(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__i2capiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool i2capiShareCallback_DISPATCH(struct I2cApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__i2capiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS i2capiGetRegBaseOffsetAndSize_DISPATCH(struct I2cApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__i2capiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS i2capiGetMapAddrSpace_DISPATCH(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__i2capiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS i2capiInternalControlForward_DISPATCH(struct I2cApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__i2capiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle i2capiGetInternalObjectHandle_DISPATCH(struct I2cApi *pGpuResource) {
    return pGpuResource->__i2capiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool i2capiAccessCallback_DISPATCH(struct I2cApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__i2capiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS i2capiGetMemInterMapParams_DISPATCH(struct I2cApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__i2capiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS i2capiCheckMemInterUnmap_DISPATCH(struct I2cApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__i2capiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS i2capiGetMemoryMappingDescriptor_DISPATCH(struct I2cApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__i2capiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS i2capiControlSerialization_Prologue_DISPATCH(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__i2capiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void i2capiControlSerialization_Epilogue_DISPATCH(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__i2capiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS i2capiControl_Prologue_DISPATCH(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__i2capiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void i2capiControl_Epilogue_DISPATCH(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__i2capiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool i2capiCanCopy_DISPATCH(struct I2cApi *pResource) {
    return pResource->__i2capiCanCopy__(pResource);
}

static inline NV_STATUS i2capiIsDuplicate_DISPATCH(struct I2cApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__i2capiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void i2capiPreDestruct_DISPATCH(struct I2cApi *pResource) {
    pResource->__i2capiPreDestruct__(pResource);
}

static inline NV_STATUS i2capiControlFilter_DISPATCH(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__i2capiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool i2capiIsPartialUnmapSupported_DISPATCH(struct I2cApi *pResource) {
    return pResource->__i2capiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS i2capiMapTo_DISPATCH(struct I2cApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__i2capiMapTo__(pResource, pParams);
}

static inline NV_STATUS i2capiUnmapFrom_DISPATCH(struct I2cApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__i2capiUnmapFrom__(pResource, pParams);
}

static inline NvU32 i2capiGetRefCount_DISPATCH(struct I2cApi *pResource) {
    return pResource->__i2capiGetRefCount__(pResource);
}

static inline void i2capiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct I2cApi *pResource, RsResourceRef *pReference) {
    pResource->__i2capiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS i2capiCtrlCmdI2cGetPortInfo_IMPL(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_GET_PORT_INFO_PARAMS *pParams);

NV_STATUS i2capiCtrlCmdI2cIndexed_IMPL(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_INDEXED_PARAMS *pParams);

NV_STATUS i2capiCtrlCmdI2cGetPortSpeed_IMPL(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_GET_PORT_SPEED_PARAMS *pParams);

NV_STATUS i2capiCtrlCmdI2cTableGetDevInfo_IMPL(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_TABLE_GET_DEV_INFO_PARAMS *pParams);

NV_STATUS i2capiCtrlCmdI2cTransaction_IMPL(struct I2cApi *pI2cApi, NV402C_CTRL_I2C_TRANSACTION_PARAMS *pParams);

NV_STATUS i2capiConstruct_IMPL(struct I2cApi *arg_pI2cApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_i2capiConstruct(arg_pI2cApi, arg_pCallContext, arg_pParams) i2capiConstruct_IMPL(arg_pI2cApi, arg_pCallContext, arg_pParams)
void i2capiDestruct_IMPL(struct I2cApi *pI2cApi);

#define __nvoc_i2capiDestruct(pI2cApi) i2capiDestruct_IMPL(pI2cApi)
#undef PRIVATE_FIELD

#endif // _I2CAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_I2C_API_NVOC_H_
