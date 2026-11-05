
#ifndef _G_SUBDEVICE_DIAG_NVOC_H_
#define _G_SUBDEVICE_DIAG_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "ctrl/ctrl208f.h" // rmcontrol params
#include "ctrl/ctrl208f/ctrl208fgr.h" // rmcontrol params

#include "gpu/gpu_resource.h"
#include "rmapi/event_api.h"
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DiagApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct DiagApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DiagApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
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

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__diagapiCtrlCmdFifoGetChannelState__)(struct DiagApi * /*this*/, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *);  // halified (2 hals) exported (id=0x208f0403) body
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DiagApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_DiagApi
#define __nvoc_class_id_DiagApi 0xaa3066u
typedef struct DiagApi DiagApi;
#endif /* __nvoc_class_id_DiagApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi;

#define __staticCast_DiagApi(pThis) \
    ((pThis)->__nvoc_pbase_DiagApi)

#ifdef __nvoc_subdevice_diag_h_disabled
#define __dynamicCast_DiagApi(pThis) ((DiagApi*) NULL)
#else //__nvoc_subdevice_diag_h_disabled
#define __dynamicCast_DiagApi(pThis) \
    ((DiagApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DiagApi)))
#endif //__nvoc_subdevice_diag_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DiagApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_DiagApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_DiagApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS diagapiConstruct_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_diagapiConstruct(pDiagApi, pCallContext, pParams) diagapiConstruct_IMPL(pDiagApi, pCallContext, pParams)

NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFifoCheckEngineContext(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFifoCheckEngineContext(pDiagApi, pCheckEngineContextParams) diagapiCtrlCmdFifoCheckEngineContext_IMPL(pDiagApi, pCheckEngineContextParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbCtrlGpuCache(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbCtrlGpuCache(pDiagApi, pGpuCacheParams) diagapiCtrlCmdFbCtrlGpuCache_IMPL(pDiagApi, pGpuCacheParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbEccSetKillPtr(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbEccSetKillPtr(pDiagApi, pParams) diagapiCtrlCmdFbEccSetKillPtr_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbEccSetWriteKill_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbEccSetWriteKill(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbEccSetWriteKill(pDiagApi, pParams) diagapiCtrlCmdFbEccSetWriteKill_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbEccInjectionSupported_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbEccInjectionSupported(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbEccInjectionSupported(pDiagApi, pParams) diagapiCtrlCmdFbEccInjectionSupported_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbRemapRow_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_ROW_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbRemapRow(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_ROW_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbRemapRow(pDiagApi, pParams) diagapiCtrlCmdFbRemapRow_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbRemapBank_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_BANK_PARAMS *pRemapBankParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbRemapBank(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_BANK_PARAMS *pRemapBankParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbRemapBank(pDiagApi, pRemapBankParams) diagapiCtrlCmdFbRemapBank_IMPL(pDiagApi, pRemapBankParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbClearRemappedRows_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbClearRemappedRows(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbClearRemappedRows(pDiagApi, pRemappedRowsParams) diagapiCtrlCmdFbClearRemappedRows_IMPL(pDiagApi, pRemappedRowsParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdFbClearRemappedBanks_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_BANKS_PARAMS *pRemappedBanksParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdFbClearRemappedBanks(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_BANKS_PARAMS *pRemappedBanksParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdFbClearRemappedBanks(pDiagApi, pRemappedBanksParams) diagapiCtrlCmdFbClearRemappedBanks_IMPL(pDiagApi, pRemappedBanksParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdGpuVerifyInforom_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdGpuVerifyInforom(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdGpuVerifyInforom(pDiagApi, pParams) diagapiCtrlCmdGpuVerifyInforom_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdGrInjectCtxswUcodeError(struct DiagApi *pDiagApi, NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdGrInjectCtxswUcodeError(pDiagApi, pParams) diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdBifPBIWriteCommand(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdBifPBIWriteCommand(pDiagApi, pWritePbiParams) diagapiCtrlCmdBifPBIWriteCommand_IMPL(pDiagApi, pWritePbiParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdBifConfigRegRead_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdBifConfigRegRead(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdBifConfigRegRead(pDiagApi, pReadConfigReg) diagapiCtrlCmdBifConfigRegRead_IMPL(pDiagApi, pReadConfigReg)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdBifConfigRegWrite_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdBifConfigRegWrite(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdBifConfigRegWrite(pDiagApi, pWriteConfigReg) diagapiCtrlCmdBifConfigRegWrite_IMPL(pDiagApi, pWriteConfigReg)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdBifInfo_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdBifInfo(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdBifInfo(pDiagApi, pInfo) diagapiCtrlCmdBifInfo_IMPL(pDiagApi, pInfo)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetState(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdUcodeInstrumentationGetState(pDiagApi, pParams) diagapiCtrlCmdUcodeInstrumentationGetState_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdUcodeInstrumentationSetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdUcodeInstrumentationSetState(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdUcodeInstrumentationSetState(pDiagApi, pParams) diagapiCtrlCmdUcodeInstrumentationSetState_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled

NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetData_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pParams);
#ifdef __nvoc_subdevice_diag_h_disabled
static inline NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetData(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DiagApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_diag_h_disabled
#define diagapiCtrlCmdUcodeInstrumentationGetData(pDiagApi, pParams) diagapiCtrlCmdUcodeInstrumentationGetData_IMPL(pDiagApi, pParams)
#endif // __nvoc_subdevice_diag_h_disabled


// Wrapper macros for halified functions
#define diagapiControl_FNPTR(pDiagApi) pDiagApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define diagapiControl(pDiagApi, pCallContext, pParams) diagapiControl_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiControlFilter_FNPTR(pDiagApi) pDiagApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define diagapiControlFilter(pDiagApi, pCallContext, pParams) diagapiControlFilter_DISPATCH(pDiagApi, pCallContext, pParams)
#define diagapiCtrlCmdFifoGetChannelState_FNPTR(pDiagApi) pDiagApi->__diagapiCtrlCmdFifoGetChannelState__
#define diagapiCtrlCmdFifoGetChannelState(pDiagApi, pChannelStateParams) diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams)
#define diagapiCtrlCmdFifoGetChannelState_HAL(pDiagApi, pChannelStateParams) diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams)
#define diagapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define diagapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) diagapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define diagapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define diagapiUnmap(pGpuResource, pCallContext, pCpuMapping) diagapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define diagapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define diagapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) diagapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define diagapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define diagapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) diagapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define diagapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define diagapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) diagapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define diagapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define diagapiInternalControlForward(pGpuResource, command, pParams, size) diagapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define diagapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define diagapiGetInternalObjectHandle(pGpuResource) diagapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define diagapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define diagapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) diagapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define diagapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define diagapiGetMemInterMapParams(pRmResource, pParams) diagapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define diagapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define diagapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) diagapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define diagapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define diagapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) diagapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define diagapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define diagapiControlSerialization_Prologue(pResource, pCallContext, pParams) diagapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define diagapiControlSerialization_Epilogue(pResource, pCallContext, pParams) diagapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define diagapiControl_Prologue(pResource, pCallContext, pParams) diagapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define diagapiControl_Epilogue(pResource, pCallContext, pParams) diagapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define diagapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define diagapiCanCopy(pResource) diagapiCanCopy_DISPATCH(pResource)
#define diagapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define diagapiIsDuplicate(pResource, hMemory, pDuplicate) diagapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define diagapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define diagapiPreDestruct(pResource) diagapiPreDestruct_DISPATCH(pResource)
#define diagapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define diagapiIsPartialUnmapSupported(pResource) diagapiIsPartialUnmapSupported_DISPATCH(pResource)
#define diagapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define diagapiMapTo(pResource, pParams) diagapiMapTo_DISPATCH(pResource, pParams)
#define diagapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define diagapiUnmapFrom(pResource, pParams) diagapiUnmapFrom_DISPATCH(pResource, pParams)
#define diagapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define diagapiGetRefCount(pResource) diagapiGetRefCount_DISPATCH(pResource)
#define diagapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define diagapiAddAdditionalDependants(pClient, pResource, pReference) diagapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define diagapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define diagapiGetNotificationListPtr(pNotifier) diagapiGetNotificationListPtr_DISPATCH(pNotifier)
#define diagapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define diagapiGetNotificationShare(pNotifier) diagapiGetNotificationShare_DISPATCH(pNotifier)
#define diagapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define diagapiSetNotificationShare(pNotifier, pNotifShare) diagapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define diagapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define diagapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) diagapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define diagapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define diagapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) diagapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS diagapiControl_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pDiagApi, RsResource), pCallContext, pParams);
}

static inline NV_STATUS diagapiControlFilter_DISPATCH(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDiagApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pDiagApi, RsResource), pCallContext, pParams);
}

static inline NV_STATUS diagapiCtrlCmdFifoGetChannelState_DISPATCH(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    return pDiagApi->__diagapiCtrlCmdFifoGetChannelState__(pDiagApi, pChannelStateParams);
}

static inline NV_STATUS diagapiMap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS diagapiUnmap_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool diagapiShareCallback_DISPATCH(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS diagapiGetRegBaseOffsetAndSize_DISPATCH(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS diagapiGetMapAddrSpace_DISPATCH(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS diagapiInternalControlForward_DISPATCH(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle diagapiGetInternalObjectHandle_DISPATCH(struct DiagApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool diagapiAccessCallback_DISPATCH(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS diagapiGetMemInterMapParams_DISPATCH(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS diagapiCheckMemInterUnmap_DISPATCH(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS diagapiGetMemoryMappingDescriptor_DISPATCH(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS diagapiControlSerialization_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void diagapiControlSerialization_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS diagapiControl_Prologue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void diagapiControl_Epilogue_DISPATCH(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool diagapiCanCopy_DISPATCH(struct DiagApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS diagapiIsDuplicate_DISPATCH(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void diagapiPreDestruct_DISPATCH(struct DiagApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NvBool diagapiIsPartialUnmapSupported_DISPATCH(struct DiagApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS diagapiMapTo_DISPATCH(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS diagapiUnmapFrom_DISPATCH(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 diagapiGetRefCount_DISPATCH(struct DiagApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void diagapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * diagapiGetNotificationListPtr_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * diagapiGetNotificationShare_DISPATCH(struct DiagApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void diagapiSetNotificationShare_DISPATCH(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS diagapiUnregisterEvent_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS diagapiGetOrAllocNotifShare_DISPATCH(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS diagapiControl_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS diagapiControlFilter_IMPL(struct DiagApi *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

// Exported method declarations and/or inline definitions
NV_STATUS diagapiCtrlCmdFifoCheckEngineContext_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS *pCheckEngineContextParams);

NV_STATUS diagapiCtrlCmdFbCtrlGpuCache_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS *pGpuCacheParams);

NV_STATUS diagapiCtrlCmdFbEccSetKillPtr_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdFbEccSetWriteKill_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdFbEccInjectionSupported_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdFbRemapRow_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_ROW_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdFbRemapBank_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_REMAP_BANK_PARAMS *pRemapBankParams);

NV_STATUS diagapiCtrlCmdFbClearRemappedRows_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);

NV_STATUS diagapiCtrlCmdFbClearRemappedBanks_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FB_CLEAR_REMAPPED_BANKS_PARAMS *pRemappedBanksParams);

NV_STATUS diagapiCtrlCmdGpuVerifyInforom_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdBifPBIWriteCommand_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS *pWritePbiParams);

NV_STATUS diagapiCtrlCmdBifConfigRegRead_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS *pReadConfigReg);

NV_STATUS diagapiCtrlCmdBifConfigRegWrite_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS *pWriteConfigReg);

NV_STATUS diagapiCtrlCmdBifInfo_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_BIF_INFO_PARAMS *pInfo);

NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdUcodeInstrumentationSetState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS *pParams);

NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetData_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS *pParams);

// HAL method declarations without bodies
NV_STATUS diagapiCtrlCmdFifoGetChannelState_IMPL(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams);

// Inline HAL method definitions
static inline NV_STATUS diagapiCtrlCmdFifoGetChannelState_395e98(struct DiagApi *pDiagApi, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams){
    return NV_ERR_NOT_SUPPORTED;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _DIAGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SUBDEVICE_DIAG_NVOC_H_
