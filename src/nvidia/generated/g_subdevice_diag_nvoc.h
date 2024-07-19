
#ifndef _G_SUBDEVICE_DIAG_NVOC_H_
#define _G_SUBDEVICE_DIAG_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_subdevice_diag_nvoc.h"

#ifndef _DIAGAPI_H_
#define _DIAGAPI_H_

#include "class/cl208f.h" // NV208F_NOTIFIERS_MAXCOUNT
#include "ctrl/ctrl208f.h" // rmcontrol params

#include "gpu/gpu_resource.h"
#include "rmapi/event.h"
#include "rmapi/control.h"
#include "gpu/gpu_halspec.h"

/*!
 * RM internal class representing NV20_SUBDEVICE_DIAG
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SUBDEVICE_DIAG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DiagApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct DiagApi *__nvoc_pbase_DiagApi;    // diagapi

    // Vtable with 46 per-object function pointers
    NV_STATUS (*__diagapiControl__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__diagapiControlFilter__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__diagapiCtrlCmdFifoCheckEngineContext__)(struct DiagApi * /*this*/, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *);  // exported (id=0x208f0401)
    NV_STATUS (*__diagapiCtrlCmdFifoEnableVirtualContext__)(struct DiagApi * /*this*/, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *);  // exported (id=0x208f0402)
    NV_STATUS (*__diagapiCtrlCmdFifoGetChannelState__)(struct DiagApi * /*this*/, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *);  // halified (2 hals) exported (id=0x208f0403) body
    NV_STATUS (*__diagapiCtrlCmdFbCtrlGpuCache__)(struct DiagApi * /*this*/, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *);  // exported (id=0x208f0506)
    NV_STATUS (*__diagapiCtrlCmdFbEccSetKillPtr__)(struct DiagApi * /*this*/, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *);  // exported (id=0x208f050e)
    NV_STATUS (*__diagapiCtrlCmdFbClearRemappedRows__)(struct DiagApi * /*this*/, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *);  // exported (id=0x208f0515)
    NV_STATUS (*__diagapiCtrlCmdGpuGetRamSvopValues__)(struct DiagApi * /*this*/, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *);  // exported (id=0x208f1101)
    NV_STATUS (*__diagapiCtrlCmdGpuSetRamSvopValues__)(struct DiagApi * /*this*/, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *);  // exported (id=0x208f1102)
    NV_STATUS (*__diagapiCtrlCmdGpuVerifyInforom__)(struct DiagApi * /*this*/, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *);  // exported (id=0x208f1105)
    NV_STATUS (*__diagapiCtrlCmdBifPBIWriteCommand__)(struct DiagApi * /*this*/, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *);  // exported (id=0x208f0701)
    NV_STATUS (*__diagapiCtrlCmdBifConfigRegRead__)(struct DiagApi * /*this*/, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *);  // exported (id=0x208f0702)
    NV_STATUS (*__diagapiCtrlCmdBifConfigRegWrite__)(struct DiagApi * /*this*/, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *);  // exported (id=0x208f0703)
    NV_STATUS (*__diagapiCtrlCmdBifInfo__)(struct DiagApi * /*this*/, NV208F_CTRL_BIF_INFO_PARAMS *);  // exported (id=0x208f0704)
    NV_STATUS (*__diagapiCtrlCmdUcodeCoverageGetState__)(struct DiagApi * /*this*/, NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *);  // exported (id=0x208f1901)
    NV_STATUS (*__diagapiCtrlCmdUcodeCoverageSetState__)(struct DiagApi * /*this*/, NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS *);  // exported (id=0x208f1902)
    NV_STATUS (*__diagapiCtrlCmdUcodeCoverageGetData__)(struct DiagApi * /*this*/, NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS *);  // exported (id=0x208f1903)
    NV_STATUS (*__diagapiMap__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__diagapiUnmap__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__diagapiShareCallback__)(struct DiagApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__diagapiGetRegBaseOffsetAndSize__)(struct DiagApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__diagapiGetMapAddrSpace__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__diagapiInternalControlForward__)(struct DiagApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__diagapiGetInternalObjectHandle__)(struct DiagApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__diagapiAccessCallback__)(struct DiagApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__diagapiGetMemInterMapParams__)(struct DiagApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__diagapiCheckMemInterUnmap__)(struct DiagApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__diagapiGetMemoryMappingDescriptor__)(struct DiagApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__diagapiControlSerialization_Prologue__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__diagapiControlSerialization_Epilogue__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__diagapiControl_Prologue__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__diagapiControl_Epilogue__)(struct DiagApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__diagapiCanCopy__)(struct DiagApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__diagapiIsDuplicate__)(struct DiagApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__diagapiPreDestruct__)(struct DiagApi * /*this*/);  // virtual inherited (res) base (gpures)
    NvBool (*__diagapiIsPartialUnmapSupported__)(struct DiagApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__diagapiMapTo__)(struct DiagApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__diagapiUnmapFrom__)(struct DiagApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__diagapiGetRefCount__)(struct DiagApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__diagapiAddAdditionalDependants__)(struct RsClient *, struct DiagApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__diagapiGetNotificationListPtr__)(struct DiagApi * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__diagapiGetNotificationShare__)(struct DiagApi * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__diagapiSetNotificationShare__)(struct DiagApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__diagapiUnregisterEvent__)(struct DiagApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__diagapiGetOrAllocNotifShare__)(struct DiagApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    NvU32 notifyActions[1];
};

#ifndef __NVOC_CLASS_DiagApi_TYPEDEF__
#define __NVOC_CLASS_DiagApi_TYPEDEF__
typedef struct DiagApi DiagApi;
#endif /* __NVOC_CLASS_DiagApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DiagApi
#define __nvoc_class_id_DiagApi 0xaa3066
#endif /* __nvoc_class_id_DiagApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi;

#define __staticCast_DiagApi(pThis) \
    ((pThis)->__nvoc_pbase_DiagApi)

#ifdef __nvoc_subdevice_diag_h_disabled
#define __dynamicCast_DiagApi(pThis) ((DiagApi*)NULL)
#else //__nvoc_subdevice_diag_h_disabled
#define __dynamicCast_DiagApi(pThis) \
    ((DiagApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DiagApi)))
#endif //__nvoc_subdevice_diag_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DiagApi(DiagApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DiagApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DiagApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define diagapiControl_FNPTR(pDiagApi) pDiagApi->__diagapiControl__
#define diagapiControl(pDiagApi, pCallContext, pParams) diagapiControl_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiControlFilter_FNPTR(pDiagApi) pDiagApi->__diagapiControlFilter__
#define diagapiControlFilter(pDiagApi, pCallContext, pParams) diagapiControlFilter_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiCtrlCmdFifoCheckEngineContext_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFifoCheckEngineContext__
#define diagapiCtrlCmdFifoCheckEngineContext(pDiagApi, pCheckEngineContextParams) diagapiCtrlCmdFifoCheckEngineContext_DISPATCH(pDiagApi, pCheckEngineContextParams)
#define diagapiCtrlCmdFifoEnableVirtualContext_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFifoEnableVirtualContext__
#define diagapiCtrlCmdFifoEnableVirtualContext(pDiagApi, pEnableVCParams) diagapiCtrlCmdFifoEnableVirtualContext_DISPATCH(pDiagApi, pEnableVCParams)
#define diagapiCtrlCmdFifoGetChannelState_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFifoGetChannelState__
#define diagapiCtrlCmdFifoGetChannelState(pDiagApi, pChannelStateParams) diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams)
#define diagapiCtrlCmdFifoGetChannelState_HAL(pDiagApi, pChannelStateParams) diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams)
#define diagapiCtrlCmdFbCtrlGpuCache_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFbCtrlGpuCache__
#define diagapiCtrlCmdFbCtrlGpuCache(pDiagApi, pGpuCacheParams) diagapiCtrlCmdFbCtrlGpuCache_DISPATCH(pDiagApi, pGpuCacheParams)
#define diagapiCtrlCmdFbEccSetKillPtr_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFbEccSetKillPtr__
#define diagapiCtrlCmdFbEccSetKillPtr(pDiagApi, pParams) diagapiCtrlCmdFbEccSetKillPtr_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdFbClearRemappedRows_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFbClearRemappedRows__
#define diagapiCtrlCmdFbClearRemappedRows(pDiagApi, pRemappedRowsParams) diagapiCtrlCmdFbClearRemappedRows_DISPATCH(pDiagApi, pRemappedRowsParams)
#define diagapiCtrlCmdGpuGetRamSvopValues_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdGpuGetRamSvopValues__
#define diagapiCtrlCmdGpuGetRamSvopValues(pDiagApi, pGetRamSvopParams) diagapiCtrlCmdGpuGetRamSvopValues_DISPATCH(pDiagApi, pGetRamSvopParams)
#define diagapiCtrlCmdGpuSetRamSvopValues_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdGpuSetRamSvopValues__
#define diagapiCtrlCmdGpuSetRamSvopValues(pDiagApi, pSetRamSvopParams) diagapiCtrlCmdGpuSetRamSvopValues_DISPATCH(pDiagApi, pSetRamSvopParams)
#define diagapiCtrlCmdGpuVerifyInforom_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdGpuVerifyInforom__
#define diagapiCtrlCmdGpuVerifyInforom(pDiagApi, pParams) diagapiCtrlCmdGpuVerifyInforom_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdBifPBIWriteCommand_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdBifPBIWriteCommand__
#define diagapiCtrlCmdBifPBIWriteCommand(pDiagApi, pWritePbiParams) diagapiCtrlCmdBifPBIWriteCommand_DISPATCH(pDiagApi, pWritePbiParams)
#define diagapiCtrlCmdBifConfigRegRead_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdBifConfigRegRead__
#define diagapiCtrlCmdBifConfigRegRead(pDiagApi, pReadConfigReg) diagapiCtrlCmdBifConfigRegRead_DISPATCH(pDiagApi, pReadConfigReg)
#define diagapiCtrlCmdBifConfigRegWrite_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdBifConfigRegWrite__
#define diagapiCtrlCmdBifConfigRegWrite(pDiagApi, pWriteConfigReg) diagapiCtrlCmdBifConfigRegWrite_DISPATCH(pDiagApi, pWriteConfigReg)
#define diagapiCtrlCmdBifInfo_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdBifInfo__
#define diagapiCtrlCmdBifInfo(pDiagApi, pInfo) diagapiCtrlCmdBifInfo_DISPATCH(pDiagApi, pInfo)
#define diagapiCtrlCmdUcodeCoverageGetState_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdUcodeCoverageGetState__
#define diagapiCtrlCmdUcodeCoverageGetState(pDiagApi, pParams) diagapiCtrlCmdUcodeCoverageGetState_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdUcodeCoverageSetState_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdUcodeCoverageSetState__
#define diagapiCtrlCmdUcodeCoverageSetState(pDiagApi, pParams) diagapiCtrlCmdUcodeCoverageSetState_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdUcodeCoverageGetData_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdUcodeCoverageGetData__
#define diagapiCtrlCmdUcodeCoverageGetData(pDiagApi, pParams) diagapiCtrlCmdUcodeCoverageGetData_DISPATCH(pDiagApi, pParams)
#define diagapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define diagapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) diagapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define diagapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define diagapiUnmap(pGpuResource, pCallContext, pCpuMapping) diagapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define diagapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define diagapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) diagapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define diagapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define diagapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) diagapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define diagapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define diagapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) diagapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define diagapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define diagapiInternalControlForward(pGpuResource, command, pParams, size) diagapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define diagapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define diagapiGetInternalObjectHandle(pGpuResource) diagapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define diagapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define diagapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) diagapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define diagapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define diagapiGetMemInterMapParams(pRmResource, pParams) diagapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define diagapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define diagapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) diagapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define diagapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define diagapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) diagapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define diagapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define diagapiControlSerialization_Prologue(pResource, pCallContext, pParams) diagapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define diagapiControlSerialization_Epilogue(pResource, pCallContext, pParams) diagapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define diagapiControl_Prologue(pResource, pCallContext, pParams) diagapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define diagapiControl_Epilogue(pResource, pCallContext, pParams) diagapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define diagapiCanCopy(pResource) diagapiCanCopy_DISPATCH(pResource)
#define diagapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define diagapiIsDuplicate(pResource, hMemory, pDuplicate) diagapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define diagapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define diagapiPreDestruct(pResource) diagapiPreDestruct_DISPATCH(pResource)
#define diagapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define diagapiIsPartialUnmapSupported(pResource) diagapiIsPartialUnmapSupported_DISPATCH(pResource)
#define diagapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define diagapiMapTo(pResource, pParams) diagapiMapTo_DISPATCH(pResource, pParams)
#define diagapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define diagapiUnmapFrom(pResource, pParams) diagapiUnmapFrom_DISPATCH(pResource, pParams)
#define diagapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define diagapiGetRefCount(pResource) diagapiGetRefCount_DISPATCH(pResource)
#define diagapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define diagapiAddAdditionalDependants(pClient, pResource, pReference) diagapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define diagapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define diagapiGetNotificationListPtr(pNotifier) diagapiGetNotificationListPtr_DISPATCH(pNotifier)
#define diagapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define diagapiGetNotificationShare(pNotifier) diagapiGetNotificationShare_DISPATCH(pNotifier)
#define diagapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define diagapiSetNotificationShare(pNotifier, pNotifShare) diagapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define diagapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define diagapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) diagapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define diagapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define diagapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) diagapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS diagapiControl_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__diagapiControl__(pDiagApi, pCallContext, pParams);
}

static inline NV_STATUS diagapiControlFilter_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__diagapiControlFilter__(pDiagApi, pCallContext, pParams);
}

static inline NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams) {
    return pDiagApi->__diagapiCtrlCmdFifoCheckEngineContext__(pDiagApi, pCheckEngineContextParams);
}

static inline NV_STATUS diagapiCtrlCmdFifoEnableVirtualContext_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *pEnableVCParams) {
    return pDiagApi->__diagapiCtrlCmdFifoEnableVirtualContext__(pDiagApi, pEnableVCParams);
}

static inline NV_STATUS diagapiCtrlCmdFifoGetChannelState_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    return pDiagApi->__diagapiCtrlCmdFifoGetChannelState__(pDiagApi, pChannelStateParams);
}

static inline NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams) {
    return pDiagApi->__diagapiCtrlCmdFbCtrlGpuCache__(pDiagApi, pGpuCacheParams);
}

static inline NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdFbEccSetKillPtr__(pDiagApi, pParams);
}

static inline NV_STATUS diagapiCtrlCmdFbClearRemappedRows_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    return pDiagApi->__diagapiCtrlCmdFbClearRemappedRows__(pDiagApi, pRemappedRowsParams);
}

static inline NV_STATUS diagapiCtrlCmdGpuGetRamSvopValues_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pGetRamSvopParams) {
    return pDiagApi->__diagapiCtrlCmdGpuGetRamSvopValues__(pDiagApi, pGetRamSvopParams);
}

static inline NV_STATUS diagapiCtrlCmdGpuSetRamSvopValues_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pSetRamSvopParams) {
    return pDiagApi->__diagapiCtrlCmdGpuSetRamSvopValues__(pDiagApi, pSetRamSvopParams);
}

static inline NV_STATUS diagapiCtrlCmdGpuVerifyInforom_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdGpuVerifyInforom__(pDiagApi, pParams);
}

static inline NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams) {
    return pDiagApi->__diagapiCtrlCmdBifPBIWriteCommand__(pDiagApi, pWritePbiParams);
}

static inline NV_STATUS diagapiCtrlCmdBifConfigRegRead_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg) {
    return pDiagApi->__diagapiCtrlCmdBifConfigRegRead__(pDiagApi, pReadConfigReg);
}

static inline NV_STATUS diagapiCtrlCmdBifConfigRegWrite_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg) {
    return pDiagApi->__diagapiCtrlCmdBifConfigRegWrite__(pDiagApi, pWriteConfigReg);
}

static inline NV_STATUS diagapiCtrlCmdBifInfo_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo) {
    return pDiagApi->__diagapiCtrlCmdBifInfo__(pDiagApi, pInfo);
}

static inline NV_STATUS diagapiCtrlCmdUcodeCoverageGetState_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdUcodeCoverageGetState__(pDiagApi, pParams);
}

static inline NV_STATUS diagapiCtrlCmdUcodeCoverageSetState_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdUcodeCoverageSetState__(pDiagApi, pParams);
}

static inline NV_STATUS diagapiCtrlCmdUcodeCoverageGetData_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdUcodeCoverageGetData__(pDiagApi, pParams);
}

static inline NV_STATUS diagapiMap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__diagapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS diagapiUnmap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__diagapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool diagapiShareCallback_DISPATCH(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__diagapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS diagapiGetRegBaseOffsetAndSize_DISPATCH(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__diagapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS diagapiGetMapAddrSpace_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__diagapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS diagapiInternalControlForward_DISPATCH(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__diagapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle diagapiGetInternalObjectHandle_DISPATCH(struct DiagApi *pGpuResource) {
    return pGpuResource->__diagapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool diagapiAccessCallback_DISPATCH(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__diagapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS diagapiGetMemInterMapParams_DISPATCH(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__diagapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS diagapiCheckMemInterUnmap_DISPATCH(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__diagapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS diagapiGetMemoryMappingDescriptor_DISPATCH(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__diagapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS diagapiControlSerialization_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__diagapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void diagapiControlSerialization_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__diagapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS diagapiControl_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__diagapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void diagapiControl_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__diagapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool diagapiCanCopy_DISPATCH(struct DiagApi *pResource) {
    return pResource->__diagapiCanCopy__(pResource);
}

static inline NV_STATUS diagapiIsDuplicate_DISPATCH(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__diagapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void diagapiPreDestruct_DISPATCH(struct DiagApi *pResource) {
    pResource->__diagapiPreDestruct__(pResource);
}

static inline NvBool diagapiIsPartialUnmapSupported_DISPATCH(struct DiagApi *pResource) {
    return pResource->__diagapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS diagapiMapTo_DISPATCH(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__diagapiMapTo__(pResource, pParams);
}

static inline NV_STATUS diagapiUnmapFrom_DISPATCH(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__diagapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 diagapiGetRefCount_DISPATCH(struct DiagApi *pResource) {
    return pResource->__diagapiGetRefCount__(pResource);
}

static inline void diagapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    pResource->__diagapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * diagapiGetNotificationListPtr_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__diagapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * diagapiGetNotificationShare_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__diagapiGetNotificationShare__(pNotifier);
}

static inline void diagapiSetNotificationShare_DISPATCH(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__diagapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS diagapiUnregisterEvent_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__diagapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS diagapiGetOrAllocNotifShare_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__diagapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS diagapiControl_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS diagapiControlFilter_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams);

NV_STATUS diagapiCtrlCmdFifoEnableVirtualContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *pEnableVCParams);

static inline NV_STATUS diagapiCtrlCmdFifoGetChannelState_46f6a7(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS diagapiCtrlCmdFifoGetChannelState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams);

NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams);

NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdFbClearRemappedRows_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);

NV_STATUS diagapiCtrlCmdGpuGetRamSvopValues_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pGetRamSvopParams);

NV_STATUS diagapiCtrlCmdGpuSetRamSvopValues_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pSetRamSvopParams);

NV_STATUS diagapiCtrlCmdGpuVerifyInforom_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams);

NV_STATUS diagapiCtrlCmdBifConfigRegRead_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg);

NV_STATUS diagapiCtrlCmdBifConfigRegWrite_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg);

NV_STATUS diagapiCtrlCmdBifInfo_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo);

NV_STATUS diagapiCtrlCmdUcodeCoverageGetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdUcodeCoverageSetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdUcodeCoverageGetData_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS *pParams);

NV_STATUS diagapiConstruct_IMPL(struct DiagApi *arg_pDiagApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_diagapiConstruct(arg_pDiagApi, arg_pCallContext, arg_pParams) diagapiConstruct_IMPL(arg_pDiagApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // _DIAGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SUBDEVICE_DIAG_NVOC_H_
