
#ifndef _G_VGPUCONFIGAPI_NVOC_H_
#define _G_VGPUCONFIGAPI_NVOC_H_

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
#include "rmapi/event_api.h"

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VgpuConfigApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct VgpuConfigApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VgpuConfigApi *__nvoc_metadata_ptr;
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
    struct VgpuConfigApi *__nvoc_pbase_VgpuConfigApi;    // vgpuconfigapi

    // Data members
    NvU32 notifyActions[5];
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VgpuConfigApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560au
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __nvoc_class_id_VgpuConfigApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi;

#define __staticCast_VgpuConfigApi(pThis) \
    ((pThis)->__nvoc_pbase_VgpuConfigApi)

#ifdef __nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) ((VgpuConfigApi*) NULL)
#else //__nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) \
    ((VgpuConfigApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VgpuConfigApi)))
#endif //__nvoc_vgpuconfigapi_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_VgpuConfigApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_VgpuConfigApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS vgpuconfigapiConstruct_IMPL(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_vgpuconfigapiConstruct(pResource, pCallContext, pParams) vgpuconfigapiConstruct_IMPL(pResource, pCallContext, pParams)

void vgpuconfigapiDestruct_IMPL(struct VgpuConfigApi *pResource);
#define __nvoc_vgpuconfigapiDestruct(pResource) vgpuconfigapiDestruct_IMPL(pResource)

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigSetInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification(pVgpuConfigApi, pSetEventParams) vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL(pVgpuConfigApi, pSetEventParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetCapability(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigSetCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCapability(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo(struct VgpuConfigApi *pVgpuConfigApi) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo(pVgpuConfigApi) vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL(pVgpuConfigApi)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity(pVgpuConfigApi, pEncoderParams) vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL(pVgpuConfigApi, pEncoderParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString(pVgpuConfigApi, pGpuMetadataStringParams) vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL(pVgpuConfigApi, pGpuMetadataStringParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdGetVgpuDriversCaps(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuSetVmName(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS *pParams);
#ifdef __nvoc_vgpuconfigapi_h_disabled
static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VgpuConfigApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vgpuconfigapi_h_disabled
#define vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL(pVgpuConfigApi, pParams)
#endif // __nvoc_vgpuconfigapi_h_disabled


// Wrapper macros for halified functions
#define vgpuconfigapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define vgpuconfigapiControl(pGpuResource, pCallContext, pParams) vgpuconfigapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vgpuconfigapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define vgpuconfigapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vgpuconfigapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vgpuconfigapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define vgpuconfigapiUnmap(pGpuResource, pCallContext, pCpuMapping) vgpuconfigapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vgpuconfigapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define vgpuconfigapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vgpuconfigapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vgpuconfigapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define vgpuconfigapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vgpuconfigapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define vgpuconfigapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vgpuconfigapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vgpuconfigapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define vgpuconfigapiInternalControlForward(pGpuResource, command, pParams, size) vgpuconfigapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vgpuconfigapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define vgpuconfigapiGetInternalObjectHandle(pGpuResource) vgpuconfigapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vgpuconfigapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define vgpuconfigapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vgpuconfigapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vgpuconfigapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vgpuconfigapiGetMemInterMapParams(pRmResource, pParams) vgpuconfigapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vgpuconfigapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vgpuconfigapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vgpuconfigapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vgpuconfigapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vgpuconfigapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vgpuconfigapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define vgpuconfigapiControlSerialization_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define vgpuconfigapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define vgpuconfigapiControl_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define vgpuconfigapiControl_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define vgpuconfigapiCanCopy(pResource) vgpuconfigapiCanCopy_DISPATCH(pResource)
#define vgpuconfigapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define vgpuconfigapiIsDuplicate(pResource, hMemory, pDuplicate) vgpuconfigapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vgpuconfigapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define vgpuconfigapiPreDestruct(pResource) vgpuconfigapiPreDestruct_DISPATCH(pResource)
#define vgpuconfigapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define vgpuconfigapiControlFilter(pResource, pCallContext, pParams) vgpuconfigapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define vgpuconfigapiIsPartialUnmapSupported(pResource) vgpuconfigapiIsPartialUnmapSupported_DISPATCH(pResource)
#define vgpuconfigapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define vgpuconfigapiMapTo(pResource, pParams) vgpuconfigapiMapTo_DISPATCH(pResource, pParams)
#define vgpuconfigapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define vgpuconfigapiUnmapFrom(pResource, pParams) vgpuconfigapiUnmapFrom_DISPATCH(pResource, pParams)
#define vgpuconfigapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define vgpuconfigapiGetRefCount(pResource) vgpuconfigapiGetRefCount_DISPATCH(pResource)
#define vgpuconfigapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define vgpuconfigapiAddAdditionalDependants(pClient, pResource, pReference) vgpuconfigapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vgpuconfigapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define vgpuconfigapiGetNotificationListPtr(pNotifier) vgpuconfigapiGetNotificationListPtr_DISPATCH(pNotifier)
#define vgpuconfigapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define vgpuconfigapiGetNotificationShare(pNotifier) vgpuconfigapiGetNotificationShare_DISPATCH(pNotifier)
#define vgpuconfigapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define vgpuconfigapiSetNotificationShare(pNotifier, pNotifShare) vgpuconfigapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define vgpuconfigapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define vgpuconfigapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) vgpuconfigapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define vgpuconfigapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define vgpuconfigapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) vgpuconfigapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS vgpuconfigapiControl_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiMap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vgpuconfigapiUnmap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool vgpuconfigapiShareCallback_DISPATCH(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS vgpuconfigapiGetMapAddrSpace_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vgpuconfigapiInternalControlForward_DISPATCH(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle vgpuconfigapiGetInternalObjectHandle_DISPATCH(struct VgpuConfigApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool vgpuconfigapiAccessCallback_DISPATCH(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vgpuconfigapiGetMemInterMapParams_DISPATCH(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS vgpuconfigapiCheckMemInterUnmap_DISPATCH(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS vgpuconfigapiControlSerialization_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vgpuconfigapiControlSerialization_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiControl_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vgpuconfigapiControl_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vgpuconfigapiCanCopy_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vgpuconfigapiIsDuplicate_DISPATCH(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void vgpuconfigapiPreDestruct_DISPATCH(struct VgpuConfigApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vgpuconfigapiControlFilter_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vgpuconfigapiIsPartialUnmapSupported_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vgpuconfigapiMapTo_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS vgpuconfigapiUnmapFrom_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 vgpuconfigapiGetRefCount_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void vgpuconfigapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * vgpuconfigapiGetNotificationListPtr_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * vgpuconfigapiGetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void vgpuconfigapiSetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS vgpuconfigapiUnregisterEvent_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS vgpuconfigapiGetOrAllocNotifShare_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
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

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS *pParams);

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


void                CliNotifyVgpuConfigEvent    (OBJGPU *,  NvU32);

#endif // _VGPUCONFIGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VGPUCONFIGAPI_NVOC_H_
