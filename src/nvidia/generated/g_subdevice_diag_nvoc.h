#ifndef _G_SUBDEVICE_DIAG_NVOC_H_
#define _G_SUBDEVICE_DIAG_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_subdevice_diag_nvoc.h"

#ifndef _DIAGAPI_H_
#define _DIAGAPI_H_

#include "class/cl208f.h" // NV208F_NOTIFIERS_MAXCOUNT
#include "ctrl/ctrl208f.h" // rmcontrol params

#include "gpu/gpu_resource.h"
#include "rmapi/event.h"
#include "rmapi/control.h"

/*!
 * RM internal class representing NV20_SUBDEVICE_DIAG
 */
#ifdef NVOC_SUBDEVICE_DIAG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DiagApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DiagApi *__nvoc_pbase_DiagApi;
    NV_STATUS (*__diagapiControl__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__diagapiControlFilter__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__diagapiCtrlCmdFifoCheckEngineContext__)(struct DiagApi *, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdFifoEnableVirtualContext__)(struct DiagApi *, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdFifoGetChannelState__)(struct DiagApi *, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdFbCtrlGpuCache__)(struct DiagApi *, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdFbEccSetKillPtr__)(struct DiagApi *, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdFbClearRemappedRows__)(struct DiagApi *, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdGpuGetRamSvopValues__)(struct DiagApi *, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdGpuSetRamSvopValues__)(struct DiagApi *, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdGpuVerifyInforom__)(struct DiagApi *, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdBifPBIWriteCommand__)(struct DiagApi *, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdBifConfigRegRead__)(struct DiagApi *, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdBifConfigRegWrite__)(struct DiagApi *, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *);
    NV_STATUS (*__diagapiCtrlCmdBifInfo__)(struct DiagApi *, NV208F_CTRL_BIF_INFO_PARAMS *);
    NvBool (*__diagapiShareCallback__)(struct DiagApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__diagapiCheckMemInterUnmap__)(struct DiagApi *, NvBool);
    NV_STATUS (*__diagapiGetOrAllocNotifShare__)(struct DiagApi *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__diagapiMapTo__)(struct DiagApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__diagapiGetMapAddrSpace__)(struct DiagApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__diagapiSetNotificationShare__)(struct DiagApi *, struct NotifShare *);
    NvU32 (*__diagapiGetRefCount__)(struct DiagApi *);
    void (*__diagapiAddAdditionalDependants__)(struct RsClient *, struct DiagApi *, RsResourceRef *);
    NV_STATUS (*__diagapiControl_Prologue__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__diagapiGetRegBaseOffsetAndSize__)(struct DiagApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__diagapiInternalControlForward__)(struct DiagApi *, NvU32, void *, NvU32);
    NV_STATUS (*__diagapiUnmapFrom__)(struct DiagApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__diagapiControl_Epilogue__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__diagapiControlLookup__)(struct DiagApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__diagapiGetInternalObjectHandle__)(struct DiagApi *);
    NV_STATUS (*__diagapiUnmap__)(struct DiagApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__diagapiGetMemInterMapParams__)(struct DiagApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__diagapiGetMemoryMappingDescriptor__)(struct DiagApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__diagapiUnregisterEvent__)(struct DiagApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__diagapiControlSerialization_Prologue__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__diagapiCanCopy__)(struct DiagApi *);
    void (*__diagapiPreDestruct__)(struct DiagApi *);
    NV_STATUS (*__diagapiIsDuplicate__)(struct DiagApi *, NvHandle, NvBool *);
    void (*__diagapiControlSerialization_Epilogue__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__diagapiGetNotificationListPtr__)(struct DiagApi *);
    struct NotifShare *(*__diagapiGetNotificationShare__)(struct DiagApi *);
    NV_STATUS (*__diagapiMap__)(struct DiagApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__diagapiAccessCallback__)(struct DiagApi *, struct RsClient *, void *, RsAccessRight);
    NvU32 notifyActions[1];
};

#ifndef __NVOC_CLASS_DiagApi_TYPEDEF__
#define __NVOC_CLASS_DiagApi_TYPEDEF__
typedef struct DiagApi DiagApi;
#endif /* __NVOC_CLASS_DiagApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DiagApi
#define __nvoc_class_id_DiagApi 0xaa3066
#endif /* __nvoc_class_id_DiagApi */

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

#define diagapiControl(pDiagApi, pCallContext, pParams) diagapiControl_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiControlFilter(pDiagApi, pCallContext, pParams) diagapiControlFilter_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiCtrlCmdFifoCheckEngineContext(pDiagApi, pCheckEngineContextParams) diagapiCtrlCmdFifoCheckEngineContext_DISPATCH(pDiagApi, pCheckEngineContextParams)
#define diagapiCtrlCmdFifoEnableVirtualContext(pDiagApi, pEnableVCParams) diagapiCtrlCmdFifoEnableVirtualContext_DISPATCH(pDiagApi, pEnableVCParams)
#define diagapiCtrlCmdFifoGetChannelState(pDiagApi, pChannelStateParams) diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams)
#define diagapiCtrlCmdFbCtrlGpuCache(pDiagApi, pGpuCacheParams) diagapiCtrlCmdFbCtrlGpuCache_DISPATCH(pDiagApi, pGpuCacheParams)
#define diagapiCtrlCmdFbEccSetKillPtr(pDiagApi, pParams) diagapiCtrlCmdFbEccSetKillPtr_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdFbClearRemappedRows(pDiagApi, pRemappedRowsParams) diagapiCtrlCmdFbClearRemappedRows_DISPATCH(pDiagApi, pRemappedRowsParams)
#define diagapiCtrlCmdGpuGetRamSvopValues(pDiagApi, pGetRamSvopParams) diagapiCtrlCmdGpuGetRamSvopValues_DISPATCH(pDiagApi, pGetRamSvopParams)
#define diagapiCtrlCmdGpuSetRamSvopValues(pDiagApi, pSetRamSvopParams) diagapiCtrlCmdGpuSetRamSvopValues_DISPATCH(pDiagApi, pSetRamSvopParams)
#define diagapiCtrlCmdGpuVerifyInforom(pDiagApi, pParams) diagapiCtrlCmdGpuVerifyInforom_DISPATCH(pDiagApi, pParams)
#define diagapiCtrlCmdBifPBIWriteCommand(pDiagApi, pWritePbiParams) diagapiCtrlCmdBifPBIWriteCommand_DISPATCH(pDiagApi, pWritePbiParams)
#define diagapiCtrlCmdBifConfigRegRead(pDiagApi, pReadConfigReg) diagapiCtrlCmdBifConfigRegRead_DISPATCH(pDiagApi, pReadConfigReg)
#define diagapiCtrlCmdBifConfigRegWrite(pDiagApi, pWriteConfigReg) diagapiCtrlCmdBifConfigRegWrite_DISPATCH(pDiagApi, pWriteConfigReg)
#define diagapiCtrlCmdBifInfo(pDiagApi, pInfo) diagapiCtrlCmdBifInfo_DISPATCH(pDiagApi, pInfo)
#define diagapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) diagapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define diagapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) diagapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define diagapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) diagapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define diagapiMapTo(pResource, pParams) diagapiMapTo_DISPATCH(pResource, pParams)
#define diagapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) diagapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define diagapiSetNotificationShare(pNotifier, pNotifShare) diagapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define diagapiGetRefCount(pResource) diagapiGetRefCount_DISPATCH(pResource)
#define diagapiAddAdditionalDependants(pClient, pResource, pReference) diagapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define diagapiControl_Prologue(pResource, pCallContext, pParams) diagapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) diagapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define diagapiInternalControlForward(pGpuResource, command, pParams, size) diagapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define diagapiUnmapFrom(pResource, pParams) diagapiUnmapFrom_DISPATCH(pResource, pParams)
#define diagapiControl_Epilogue(pResource, pCallContext, pParams) diagapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControlLookup(pResource, pParams, ppEntry) diagapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define diagapiGetInternalObjectHandle(pGpuResource) diagapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define diagapiUnmap(pGpuResource, pCallContext, pCpuMapping) diagapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define diagapiGetMemInterMapParams(pRmResource, pParams) diagapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define diagapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) diagapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define diagapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) diagapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define diagapiControlSerialization_Prologue(pResource, pCallContext, pParams) diagapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiCanCopy(pResource) diagapiCanCopy_DISPATCH(pResource)
#define diagapiPreDestruct(pResource) diagapiPreDestruct_DISPATCH(pResource)
#define diagapiIsDuplicate(pResource, hMemory, pDuplicate) diagapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define diagapiControlSerialization_Epilogue(pResource, pCallContext, pParams) diagapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiGetNotificationListPtr(pNotifier) diagapiGetNotificationListPtr_DISPATCH(pNotifier)
#define diagapiGetNotificationShare(pNotifier) diagapiGetNotificationShare_DISPATCH(pNotifier)
#define diagapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) diagapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define diagapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) diagapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS diagapiControl_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS diagapiControl_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__diagapiControl__(pDiagApi, pCallContext, pParams);
}

NV_STATUS diagapiControlFilter_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS diagapiControlFilter_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__diagapiControlFilter__(pDiagApi, pCallContext, pParams);
}

NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams);

static inline NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams) {
    return pDiagApi->__diagapiCtrlCmdFifoCheckEngineContext__(pDiagApi, pCheckEngineContextParams);
}

NV_STATUS diagapiCtrlCmdFifoEnableVirtualContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *pEnableVCParams);

static inline NV_STATUS diagapiCtrlCmdFifoEnableVirtualContext_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS *pEnableVCParams) {
    return pDiagApi->__diagapiCtrlCmdFifoEnableVirtualContext__(pDiagApi, pEnableVCParams);
}

NV_STATUS diagapiCtrlCmdFifoGetChannelState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams);

static inline NV_STATUS diagapiCtrlCmdFifoGetChannelState_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    return pDiagApi->__diagapiCtrlCmdFifoGetChannelState__(pDiagApi, pChannelStateParams);
}

NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams);

static inline NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams) {
    return pDiagApi->__diagapiCtrlCmdFbCtrlGpuCache__(pDiagApi, pGpuCacheParams);
}

NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams);

static inline NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdFbEccSetKillPtr__(pDiagApi, pParams);
}

NV_STATUS diagapiCtrlCmdFbClearRemappedRows_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);

static inline NV_STATUS diagapiCtrlCmdFbClearRemappedRows_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    return pDiagApi->__diagapiCtrlCmdFbClearRemappedRows__(pDiagApi, pRemappedRowsParams);
}

NV_STATUS diagapiCtrlCmdGpuGetRamSvopValues_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pGetRamSvopParams);

static inline NV_STATUS diagapiCtrlCmdGpuGetRamSvopValues_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pGetRamSvopParams) {
    return pDiagApi->__diagapiCtrlCmdGpuGetRamSvopValues__(pDiagApi, pGetRamSvopParams);
}

NV_STATUS diagapiCtrlCmdGpuSetRamSvopValues_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pSetRamSvopParams);

static inline NV_STATUS diagapiCtrlCmdGpuSetRamSvopValues_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS *pSetRamSvopParams) {
    return pDiagApi->__diagapiCtrlCmdGpuSetRamSvopValues__(pDiagApi, pSetRamSvopParams);
}

NV_STATUS diagapiCtrlCmdGpuVerifyInforom_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams);

static inline NV_STATUS diagapiCtrlCmdGpuVerifyInforom_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams) {
    return pDiagApi->__diagapiCtrlCmdGpuVerifyInforom__(pDiagApi, pParams);
}

NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams);

static inline NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams) {
    return pDiagApi->__diagapiCtrlCmdBifPBIWriteCommand__(pDiagApi, pWritePbiParams);
}

NV_STATUS diagapiCtrlCmdBifConfigRegRead_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg);

static inline NV_STATUS diagapiCtrlCmdBifConfigRegRead_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg) {
    return pDiagApi->__diagapiCtrlCmdBifConfigRegRead__(pDiagApi, pReadConfigReg);
}

NV_STATUS diagapiCtrlCmdBifConfigRegWrite_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg);

static inline NV_STATUS diagapiCtrlCmdBifConfigRegWrite_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg) {
    return pDiagApi->__diagapiCtrlCmdBifConfigRegWrite__(pDiagApi, pWriteConfigReg);
}

NV_STATUS diagapiCtrlCmdBifInfo_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo);

static inline NV_STATUS diagapiCtrlCmdBifInfo_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo) {
    return pDiagApi->__diagapiCtrlCmdBifInfo__(pDiagApi, pInfo);
}

static inline NvBool diagapiShareCallback_DISPATCH(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__diagapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS diagapiCheckMemInterUnmap_DISPATCH(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__diagapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS diagapiGetOrAllocNotifShare_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__diagapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS diagapiMapTo_DISPATCH(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__diagapiMapTo__(pResource, pParams);
}

static inline NV_STATUS diagapiGetMapAddrSpace_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__diagapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void diagapiSetNotificationShare_DISPATCH(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__diagapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 diagapiGetRefCount_DISPATCH(struct DiagApi *pResource) {
    return pResource->__diagapiGetRefCount__(pResource);
}

static inline void diagapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    pResource->__diagapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS diagapiControl_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__diagapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS diagapiGetRegBaseOffsetAndSize_DISPATCH(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__diagapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS diagapiInternalControlForward_DISPATCH(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__diagapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS diagapiUnmapFrom_DISPATCH(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__diagapiUnmapFrom__(pResource, pParams);
}

static inline void diagapiControl_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__diagapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS diagapiControlLookup_DISPATCH(struct DiagApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__diagapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle diagapiGetInternalObjectHandle_DISPATCH(struct DiagApi *pGpuResource) {
    return pGpuResource->__diagapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS diagapiUnmap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__diagapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS diagapiGetMemInterMapParams_DISPATCH(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__diagapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS diagapiGetMemoryMappingDescriptor_DISPATCH(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__diagapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS diagapiUnregisterEvent_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__diagapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS diagapiControlSerialization_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__diagapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool diagapiCanCopy_DISPATCH(struct DiagApi *pResource) {
    return pResource->__diagapiCanCopy__(pResource);
}

static inline void diagapiPreDestruct_DISPATCH(struct DiagApi *pResource) {
    pResource->__diagapiPreDestruct__(pResource);
}

static inline NV_STATUS diagapiIsDuplicate_DISPATCH(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__diagapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void diagapiControlSerialization_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__diagapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *diagapiGetNotificationListPtr_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__diagapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *diagapiGetNotificationShare_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__diagapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS diagapiMap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__diagapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool diagapiAccessCallback_DISPATCH(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__diagapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS diagapiConstruct_IMPL(struct DiagApi *arg_pDiagApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_diagapiConstruct(arg_pDiagApi, arg_pCallContext, arg_pParams) diagapiConstruct_IMPL(arg_pDiagApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // _DIAGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SUBDEVICE_DIAG_NVOC_H_
