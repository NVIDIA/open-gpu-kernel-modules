
#ifndef _G_VGPUCONFIGAPI_NVOC_H_
#define _G_VGPUCONFIGAPI_NVOC_H_
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
#include "g_vgpuconfigapi_nvoc.h"

#ifndef _VGPUCONFIGAPI_H_
#define _VGPUCONFIGAPI_H_

#include "core/core.h"
#include "class/cla081.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"

#include "ctrl/ctrla081.h" // rmcontrol params

//
// Virtual GPU configuration information
//


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VGPUCONFIGAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct VgpuConfigApi {

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
    struct VgpuConfigApi *__nvoc_pbase_VgpuConfigApi;    // vgpuconfigapi

    // Vtable with 54 per-object function pointers
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *);  // exported (id=0xa0810101)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *);  // exported (id=0xa0810102)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *);  // exported (id=0xa0810103)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *);  // exported (id=0xa0810104)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *);  // exported (id=0xa0810105)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *);  // exported (id=0xa0810106)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetCapability__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *);  // exported (id=0xa081011e)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetCapability__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *);  // exported (id=0xa081011f)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *);  // exported (id=0xa081011c)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__)(struct VgpuConfigApi * /*this*/);  // exported (id=0xa0810109)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *);  // exported (id=0xa0810110)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *);  // exported (id=0xa0810111)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *);  // exported (id=0xa0810112)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *);  // exported (id=0xa0810113)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *);  // exported (id=0xa0810114)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *);  // exported (id=0xa0810115)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *);  // exported (id=0xa0810116)
    NV_STATUS (*__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *);  // exported (id=0xa0810117)
    NV_STATUS (*__vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *);  // exported (id=0xa081011d)
    NV_STATUS (*__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *);  // exported (id=0xa0810118)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *);  // exported (id=0xa0810119)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *);  // exported (id=0xa081011a)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_CONFIG_UPDATE_HETEROGENEOUS_INFO_PARAMS *);  // exported (id=0xa081011b)
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuSetVmName__)(struct VgpuConfigApi * /*this*/, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *);  // exported (id=0xa0810120)
    NV_STATUS (*__vgpuconfigapiControl__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuconfigapiMap__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuconfigapiUnmap__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vgpuconfigapiShareCallback__)(struct VgpuConfigApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuconfigapiGetRegBaseOffsetAndSize__)(struct VgpuConfigApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuconfigapiGetMapAddrSpace__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vgpuconfigapiInternalControlForward__)(struct VgpuConfigApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__vgpuconfigapiGetInternalObjectHandle__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vgpuconfigapiAccessCallback__)(struct VgpuConfigApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuconfigapiGetMemInterMapParams__)(struct VgpuConfigApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuconfigapiCheckMemInterUnmap__)(struct VgpuConfigApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuconfigapiGetMemoryMappingDescriptor__)(struct VgpuConfigApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuconfigapiControlSerialization_Prologue__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vgpuconfigapiControlSerialization_Epilogue__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vgpuconfigapiControl_Prologue__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vgpuconfigapiControl_Epilogue__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__vgpuconfigapiCanCopy__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuconfigapiIsDuplicate__)(struct VgpuConfigApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__vgpuconfigapiPreDestruct__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuconfigapiControlFilter__)(struct VgpuConfigApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__vgpuconfigapiIsPartialUnmapSupported__)(struct VgpuConfigApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__vgpuconfigapiMapTo__)(struct VgpuConfigApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vgpuconfigapiUnmapFrom__)(struct VgpuConfigApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__vgpuconfigapiGetRefCount__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__vgpuconfigapiAddAdditionalDependants__)(struct RsClient *, struct VgpuConfigApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__vgpuconfigapiGetNotificationListPtr__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__vgpuconfigapiGetNotificationShare__)(struct VgpuConfigApi * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__vgpuconfigapiSetNotificationShare__)(struct VgpuConfigApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__vgpuconfigapiUnregisterEvent__)(struct VgpuConfigApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__vgpuconfigapiGetOrAllocNotifShare__)(struct VgpuConfigApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    NvU32 notifyActions[5];
};

#ifndef __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
#define __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __NVOC_CLASS_VgpuConfigApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560a
#endif /* __nvoc_class_id_VgpuConfigApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi;

#define __staticCast_VgpuConfigApi(pThis) \
    ((pThis)->__nvoc_pbase_VgpuConfigApi)

#ifdef __nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) ((VgpuConfigApi*)NULL)
#else //__nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) \
    ((VgpuConfigApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VgpuConfigApi)))
#endif //__nvoc_vgpuconfigapi_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(VgpuConfigApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VgpuConfigApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VgpuConfigApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vgpuconfigapiCtrlCmdVgpuConfigSetInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__
#define vgpuconfigapiCtrlCmdVgpuConfigSetInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__
#define vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__
#define vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__
#define vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification(pVgpuConfigApi, pSetEventParams) vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_DISPATCH(pVgpuConfigApi, pSetEventParams)
#define vgpuconfigapiCtrlCmdVgpuConfigSetCapability_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetCapability__
#define vgpuconfigapiCtrlCmdVgpuConfigSetCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetCapability_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetCapability_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCapability__
#define vgpuconfigapiCtrlCmdVgpuConfigGetCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCapability_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__
#define vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo(pVgpuConfigApi) vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_DISPATCH(pVgpuConfigApi)
#define vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__
#define vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity(pVgpuConfigApi, pEncoderParams) vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_DISPATCH(pVgpuConfigApi, pEncoderParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__
#define vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__
#define vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__
#define vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString(pVgpuConfigApi, pGpuMetadataStringParams) vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_DISPATCH(pVgpuConfigApi, pGpuMetadataStringParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__
#define vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__
#define vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__
#define vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__
#define vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdGetVgpuDriversCaps_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__
#define vgpuconfigapiCtrlCmdGetVgpuDriversCaps(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdGetVgpuDriversCaps_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__
#define vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__
#define vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo__
#define vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuSetVmName_FNPTR(pVgpuConfigApi) pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuSetVmName__
#define vgpuconfigapiCtrlCmdVgpuSetVmName(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuSetVmName_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define vgpuconfigapiControl(pGpuResource, pCallContext, pParams) vgpuconfigapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vgpuconfigapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define vgpuconfigapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vgpuconfigapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vgpuconfigapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define vgpuconfigapiUnmap(pGpuResource, pCallContext, pCpuMapping) vgpuconfigapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vgpuconfigapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define vgpuconfigapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vgpuconfigapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vgpuconfigapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define vgpuconfigapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vgpuconfigapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define vgpuconfigapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vgpuconfigapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vgpuconfigapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define vgpuconfigapiInternalControlForward(pGpuResource, command, pParams, size) vgpuconfigapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vgpuconfigapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define vgpuconfigapiGetInternalObjectHandle(pGpuResource) vgpuconfigapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vgpuconfigapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define vgpuconfigapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vgpuconfigapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vgpuconfigapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define vgpuconfigapiGetMemInterMapParams(pRmResource, pParams) vgpuconfigapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vgpuconfigapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define vgpuconfigapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vgpuconfigapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vgpuconfigapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define vgpuconfigapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vgpuconfigapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define vgpuconfigapiControlSerialization_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define vgpuconfigapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define vgpuconfigapiControl_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define vgpuconfigapiControl_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define vgpuconfigapiCanCopy(pResource) vgpuconfigapiCanCopy_DISPATCH(pResource)
#define vgpuconfigapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define vgpuconfigapiIsDuplicate(pResource, hMemory, pDuplicate) vgpuconfigapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vgpuconfigapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define vgpuconfigapiPreDestruct(pResource) vgpuconfigapiPreDestruct_DISPATCH(pResource)
#define vgpuconfigapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define vgpuconfigapiControlFilter(pResource, pCallContext, pParams) vgpuconfigapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define vgpuconfigapiIsPartialUnmapSupported(pResource) vgpuconfigapiIsPartialUnmapSupported_DISPATCH(pResource)
#define vgpuconfigapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define vgpuconfigapiMapTo(pResource, pParams) vgpuconfigapiMapTo_DISPATCH(pResource, pParams)
#define vgpuconfigapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define vgpuconfigapiUnmapFrom(pResource, pParams) vgpuconfigapiUnmapFrom_DISPATCH(pResource, pParams)
#define vgpuconfigapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define vgpuconfigapiGetRefCount(pResource) vgpuconfigapiGetRefCount_DISPATCH(pResource)
#define vgpuconfigapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define vgpuconfigapiAddAdditionalDependants(pClient, pResource, pReference) vgpuconfigapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vgpuconfigapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define vgpuconfigapiGetNotificationListPtr(pNotifier) vgpuconfigapiGetNotificationListPtr_DISPATCH(pNotifier)
#define vgpuconfigapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define vgpuconfigapiGetNotificationShare(pNotifier) vgpuconfigapiGetNotificationShare_DISPATCH(pNotifier)
#define vgpuconfigapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define vgpuconfigapiSetNotificationShare(pNotifier, pNotifShare) vgpuconfigapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define vgpuconfigapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define vgpuconfigapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) vgpuconfigapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define vgpuconfigapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define vgpuconfigapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) vgpuconfigapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__(pVgpuConfigApi, pSetEventParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetCapability_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetCapability__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCapability_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCapability__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__(pVgpuConfigApi);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__(pVgpuConfigApi, pEncoderParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__(pVgpuConfigApi, pGpuMetadataStringParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_UPDATE_HETEROGENEOUS_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuSetVmName__(pVgpuConfigApi, pParams);
}

static inline NV_STATUS vgpuconfigapiControl_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vgpuconfigapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiMap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuconfigapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vgpuconfigapiUnmap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuconfigapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool vgpuconfigapiShareCallback_DISPATCH(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vgpuconfigapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vgpuconfigapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vgpuconfigapiGetMapAddrSpace_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vgpuconfigapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vgpuconfigapiInternalControlForward_DISPATCH(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vgpuconfigapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle vgpuconfigapiGetInternalObjectHandle_DISPATCH(struct VgpuConfigApi *pGpuResource) {
    return pGpuResource->__vgpuconfigapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool vgpuconfigapiAccessCallback_DISPATCH(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vgpuconfigapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vgpuconfigapiGetMemInterMapParams_DISPATCH(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vgpuconfigapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vgpuconfigapiCheckMemInterUnmap_DISPATCH(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vgpuconfigapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vgpuconfigapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vgpuconfigapiControlSerialization_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vgpuconfigapiControlSerialization_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuconfigapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiControl_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vgpuconfigapiControl_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuconfigapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuconfigapiCanCopy_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__vgpuconfigapiCanCopy__(pResource);
}

static inline NV_STATUS vgpuconfigapiIsDuplicate_DISPATCH(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vgpuconfigapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vgpuconfigapiPreDestruct_DISPATCH(struct VgpuConfigApi *pResource) {
    pResource->__vgpuconfigapiPreDestruct__(pResource);
}

static inline NV_STATUS vgpuconfigapiControlFilter_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuconfigapiIsPartialUnmapSupported_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__vgpuconfigapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vgpuconfigapiMapTo_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vgpuconfigapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vgpuconfigapiUnmapFrom_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vgpuconfigapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 vgpuconfigapiGetRefCount_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__vgpuconfigapiGetRefCount__(pResource);
}

static inline void vgpuconfigapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference) {
    pResource->__vgpuconfigapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * vgpuconfigapiGetNotificationListPtr_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__vgpuconfigapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * vgpuconfigapiGetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__vgpuconfigapiGetNotificationShare__(pNotifier);
}

static inline void vgpuconfigapiSetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__vgpuconfigapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS vgpuconfigapiUnregisterEvent_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__vgpuconfigapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS vgpuconfigapiGetOrAllocNotifShare_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__vgpuconfigapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdateHeterogeneousInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_UPDATE_HETEROGENEOUS_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams);

NV_STATUS vgpuconfigapiConstruct_IMPL(struct VgpuConfigApi *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vgpuconfigapiConstruct(arg_pResource, arg_pCallContext, arg_pParams) vgpuconfigapiConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void vgpuconfigapiDestruct_IMPL(struct VgpuConfigApi *pResource);

#define __nvoc_vgpuconfigapiDestruct(pResource) vgpuconfigapiDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


void                CliNotifyVgpuConfigEvent    (OBJGPU *,  NvU32);

#endif // _VGPUCONFIGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VGPUCONFIGAPI_NVOC_H_
