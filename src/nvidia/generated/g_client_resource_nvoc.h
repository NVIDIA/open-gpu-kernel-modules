#ifndef _G_CLIENT_RESOURCE_NVOC_H_
#define _G_CLIENT_RESOURCE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_client_resource_nvoc.h"


#ifndef _CLIENT_RESOURCE_H_
#define _CLIENT_RESOURCE_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_client.h"
#include "rmapi/resource.h"
#include "rmapi/event.h"
#include "rmapi/control.h"

#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "ctrl/ctrl0000/ctrl0000gpuacct.h"
#include "ctrl/ctrl0000/ctrl0000gsync.h"
#include "ctrl/ctrl0000/ctrl0000diag.h"
#include "ctrl/ctrl0000/ctrl0000event.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "ctrl/ctrl0000/ctrl0000proc.h"
#include "ctrl/ctrl0000/ctrl0000syncgpuboost.h"
#include "ctrl/ctrl0000/ctrl0000vgpu.h"
#include "ctrl/ctrl0000/ctrl0000client.h"

/* include appropriate os-specific command header */
#if defined(NV_UNIX) || defined(NV_QNX)
#include "ctrl/ctrl0000/ctrl0000unix.h"
#endif

#ifdef NVOC_CLIENT_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RmClientResource {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsClientResource __nvoc_base_RsClientResource;
    struct RmResourceCommon __nvoc_base_RmResourceCommon;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RsClientResource *__nvoc_pbase_RsClientResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct RmClientResource *__nvoc_pbase_RmClientResource;
    NvBool (*__cliresAccessCallback__)(struct RmClientResource *, struct RsClient *, void *, RsAccessRight);
    NvBool (*__cliresShareCallback__)(struct RmClientResource *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__cliresControl_Prologue__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__cliresControl_Epilogue__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cliresCtrlCmdSystemGetCpuInfo__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetFeatures__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetBuildVersionV2__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemExecuteAcpiMethod__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetChipsetInfo__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemSetMemorySize__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetLockTimes__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetClassList__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemNotifyEvent__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetPlatformType__)(struct RmClientResource *, NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemDebugCtrlRmMsg__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsControl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsBatchControl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetHwbcInfo__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetP2pCaps__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetP2pCapsV2__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetP2pCapsMatrix__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsCtrl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsGetFrmData__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsSetFrmData__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGpsCallAcpi__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetPerfSensorCounters__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetExtendedPerfSensorCounters__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetGpusPowerStatus__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetPrivilegedStatus__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetFabricStatus__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetRmInstanceId__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemGetClientDatabaseInfo__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemRmctrlCacheModeCtrl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetDumpSize__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetDump__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_DUMP_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetTimestamp__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetNvlogInfo__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetNvlogBufferInfo__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetNvlog__)(struct RmClientResource *, NV0000_CTRL_NVD_GET_NVLOG_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdNvdGetRcerrRpt__)(struct RmClientResource *, NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientGetAddrSpaceType__)(struct RmClientResource *, NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientGetHandleInfo__)(struct RmClientResource *, NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientGetAccessRights__)(struct RmClientResource *, NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientSetInheritedSharePolicy__)(struct RmClientResource *, NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientShareObject__)(struct RmClientResource *, NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdClientGetChildHandle__)(struct RmClientResource *, NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdObjectsAreDuplicates__)(struct RmClientResource *, NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetAttachedIds__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetIdInfo__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetIdInfoV2__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetInitStatus__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetDeviceIds__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetActiveDeviceIds__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetProbedIds__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAttachIds__)(struct RmClientResource *, NV0000_CTRL_GPU_ATTACH_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuDetachIds__)(struct RmClientResource *, NV0000_CTRL_GPU_DETACH_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetSvmSize__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetPciInfo__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetUuidInfo__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetUuidFromGpuId__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuModifyGpuDrainState__)(struct RmClientResource *, NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuQueryGpuDrainState__)(struct RmClientResource *, NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetMemOpEnable__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuDisableNvlinkInit__)(struct RmClientResource *, NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuSetNvlinkBwMode__)(struct RmClientResource *, NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetNvlinkBwMode__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdLegacyConfig__)(struct RmClientResource *, NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdIdleChannels__)(struct RmClientResource *, NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdPushGspUcode__)(struct RmClientResource *, NV0000_CTRL_GPU_PUSH_GSP_UCODE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuGetVideoLinks__)(struct RmClientResource *, NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGsyncGetAttachedIds__)(struct RmClientResource *, NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGsyncGetIdInfo__)(struct RmClientResource *, NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdEventSetNotification__)(struct RmClientResource *, NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdEventGetSystemEventStatus__)(struct RmClientResource *, NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixExportObjectToFd__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixImportObjectFromFd__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixGetExportObjectInfo__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixCreateExportObjectFd__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixExportObjectsToFd__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixImportObjectsFromFd__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdOsUnixFlushUserCache__)(struct RmClientResource *, NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAcctSetAccountingState__)(struct RmClientResource *, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAcctGetAccountingState__)(struct RmClientResource *, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAcctGetProcAccountingInfo__)(struct RmClientResource *, NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAcctGetAccountingPids__)(struct RmClientResource *, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdGpuAcctClearAccountingData__)(struct RmClientResource *, NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSetSubProcessID__)(struct RmClientResource *, NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdDisableSubProcessUserdIsolation__)(struct RmClientResource *, NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSyncGpuBoostInfo__)(struct RmClientResource *, NV0000_SYNC_GPU_BOOST_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSyncGpuBoostGroupCreate__)(struct RmClientResource *, NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSyncGpuBoostGroupDestroy__)(struct RmClientResource *, NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSyncGpuBoostGroupInfo__)(struct RmClientResource *, NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdVgpuGetStartData__)(struct RmClientResource *, NV0000_CTRL_VGPU_GET_START_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdVgpuGetVgpuVersion__)(struct RmClientResource *, NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdVgpuSetVgpuVersion__)(struct RmClientResource *, NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemNVPCFGetPowerModeInfo__)(struct RmClientResource *, NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemSyncExternalFabricMgmt__)(struct RmClientResource *, NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrCtrl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrGetFrmData__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrSetFrmData__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrCallAcpi__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrControl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrBatchControl__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *);
    NV_STATUS (*__cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters__)(struct RmClientResource *, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *);
    NV_STATUS (*__cliresControl__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cliresUnmap__)(struct RmClientResource *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__cliresMapTo__)(struct RmClientResource *, RS_RES_MAP_TO_PARAMS *);
    void (*__cliresSetNotificationShare__)(struct RmClientResource *, struct NotifShare *);
    NV_STATUS (*__cliresControlFilter__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__cliresAddAdditionalDependants__)(struct RsClient *, struct RmClientResource *, RsResourceRef *);
    NvU32 (*__cliresGetRefCount__)(struct RmClientResource *);
    NV_STATUS (*__cliresUnregisterEvent__)(struct RmClientResource *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__cliresControlSerialization_Prologue__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__cliresCanCopy__)(struct RmClientResource *);
    void (*__cliresPreDestruct__)(struct RmClientResource *);
    NV_STATUS (*__cliresUnmapFrom__)(struct RmClientResource *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__cliresIsDuplicate__)(struct RmClientResource *, NvHandle, NvBool *);
    void (*__cliresControlSerialization_Epilogue__)(struct RmClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__cliresGetNotificationListPtr__)(struct RmClientResource *);
    struct NotifShare *(*__cliresGetNotificationShare__)(struct RmClientResource *);
    NV_STATUS (*__cliresControlLookup__)(struct RmClientResource *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__cliresMap__)(struct RmClientResource *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__cliresGetOrAllocNotifShare__)(struct RmClientResource *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_RmClientResource_TYPEDEF__
#define __NVOC_CLASS_RmClientResource_TYPEDEF__
typedef struct RmClientResource RmClientResource;
#endif /* __NVOC_CLASS_RmClientResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RmClientResource
#define __nvoc_class_id_RmClientResource 0x37a701
#endif /* __nvoc_class_id_RmClientResource */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmClientResource;

#define __staticCast_RmClientResource(pThis) \
    ((pThis)->__nvoc_pbase_RmClientResource)

#ifdef __nvoc_client_resource_h_disabled
#define __dynamicCast_RmClientResource(pThis) ((RmClientResource*)NULL)
#else //__nvoc_client_resource_h_disabled
#define __dynamicCast_RmClientResource(pThis) \
    ((RmClientResource*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmClientResource)))
#endif //__nvoc_client_resource_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RmClientResource(RmClientResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmClientResource(RmClientResource**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RmClientResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RmClientResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define cliresAccessCallback(pRmCliRes, pInvokingClient, pAllocParams, accessRight) cliresAccessCallback_DISPATCH(pRmCliRes, pInvokingClient, pAllocParams, accessRight)
#define cliresShareCallback(pRmCliRes, pInvokingClient, pParentRef, pSharePolicy) cliresShareCallback_DISPATCH(pRmCliRes, pInvokingClient, pParentRef, pSharePolicy)
#define cliresControl_Prologue(pRmCliRes, pCallContext, pParams) cliresControl_Prologue_DISPATCH(pRmCliRes, pCallContext, pParams)
#define cliresControl_Epilogue(pRmCliRes, pCallContext, pParams) cliresControl_Epilogue_DISPATCH(pRmCliRes, pCallContext, pParams)
#define cliresCtrlCmdSystemGetCpuInfo(pRmCliRes, pCpuInfoParams) cliresCtrlCmdSystemGetCpuInfo_DISPATCH(pRmCliRes, pCpuInfoParams)
#define cliresCtrlCmdSystemGetFeatures(pRmCliRes, pParams) cliresCtrlCmdSystemGetFeatures_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetBuildVersionV2(pRmCliRes, pParams) cliresCtrlCmdSystemGetBuildVersionV2_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemExecuteAcpiMethod(pRmCliRes, pAcpiMethodParams) cliresCtrlCmdSystemExecuteAcpiMethod_DISPATCH(pRmCliRes, pAcpiMethodParams)
#define cliresCtrlCmdSystemGetChipsetInfo(pRmCliRes, pChipsetInfo) cliresCtrlCmdSystemGetChipsetInfo_DISPATCH(pRmCliRes, pChipsetInfo)
#define cliresCtrlCmdSystemSetMemorySize(pRmCliRes, pParams) cliresCtrlCmdSystemSetMemorySize_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetLockTimes(pRmCliRes, pParams) cliresCtrlCmdSystemGetLockTimes_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetClassList(pRmCliRes, pParams) cliresCtrlCmdSystemGetClassList_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemNotifyEvent(pRmCliRes, pParams) cliresCtrlCmdSystemNotifyEvent_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetPlatformType(pRmCliRes, pSysParams) cliresCtrlCmdSystemGetPlatformType_DISPATCH(pRmCliRes, pSysParams)
#define cliresCtrlCmdSystemDebugCtrlRmMsg(pRmCliRes, pParams) cliresCtrlCmdSystemDebugCtrlRmMsg_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGpsControl(pRmCliRes, controlParams) cliresCtrlCmdSystemGpsControl_DISPATCH(pRmCliRes, controlParams)
#define cliresCtrlCmdSystemGpsBatchControl(pRmCliRes, controlParams) cliresCtrlCmdSystemGpsBatchControl_DISPATCH(pRmCliRes, controlParams)
#define cliresCtrlCmdSystemGetHwbcInfo(pRmCliRes, pParams) cliresCtrlCmdSystemGetHwbcInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetP2pCaps(pRmCliRes, pP2PParams) cliresCtrlCmdSystemGetP2pCaps_DISPATCH(pRmCliRes, pP2PParams)
#define cliresCtrlCmdSystemGetP2pCapsV2(pRmCliRes, pP2PParams) cliresCtrlCmdSystemGetP2pCapsV2_DISPATCH(pRmCliRes, pP2PParams)
#define cliresCtrlCmdSystemGetP2pCapsMatrix(pRmCliRes, pP2PParams) cliresCtrlCmdSystemGetP2pCapsMatrix_DISPATCH(pRmCliRes, pP2PParams)
#define cliresCtrlCmdSystemGpsCtrl(pRmCliRes, pParams) cliresCtrlCmdSystemGpsCtrl_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGpsGetFrmData(pRmCliRes, pParams) cliresCtrlCmdSystemGpsGetFrmData_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGpsSetFrmData(pRmCliRes, pParams) cliresCtrlCmdSystemGpsSetFrmData_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGpsCallAcpi(pRmCliRes, pParams) cliresCtrlCmdSystemGpsCallAcpi_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetPerfSensorCounters(pRmCliRes, pParams) cliresCtrlCmdSystemGetPerfSensorCounters_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetExtendedPerfSensorCounters(pRmCliRes, pParams) cliresCtrlCmdSystemGetExtendedPerfSensorCounters_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetGpusPowerStatus(pRmCliRes, pGpusPowerStatus) cliresCtrlCmdSystemGetGpusPowerStatus_DISPATCH(pRmCliRes, pGpusPowerStatus)
#define cliresCtrlCmdSystemGetPrivilegedStatus(pRmCliRes, pParams) cliresCtrlCmdSystemGetPrivilegedStatus_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetFabricStatus(pRmCliRes, pParams) cliresCtrlCmdSystemGetFabricStatus_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemGetRmInstanceId(pRmCliRes, pRmInstanceIdParams) cliresCtrlCmdSystemGetRmInstanceId_DISPATCH(pRmCliRes, pRmInstanceIdParams)
#define cliresCtrlCmdSystemGetClientDatabaseInfo(pRmCliRes, pParams) cliresCtrlCmdSystemGetClientDatabaseInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemRmctrlCacheModeCtrl(pRmCliRes, pParams) cliresCtrlCmdSystemRmctrlCacheModeCtrl_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdNvdGetDumpSize(pRmCliRes, pDumpSizeParams) cliresCtrlCmdNvdGetDumpSize_DISPATCH(pRmCliRes, pDumpSizeParams)
#define cliresCtrlCmdNvdGetDump(pRmCliRes, pDumpParams) cliresCtrlCmdNvdGetDump_DISPATCH(pRmCliRes, pDumpParams)
#define cliresCtrlCmdNvdGetTimestamp(pRmCliRes, pTimestampParams) cliresCtrlCmdNvdGetTimestamp_DISPATCH(pRmCliRes, pTimestampParams)
#define cliresCtrlCmdNvdGetNvlogInfo(pRmCliRes, pParams) cliresCtrlCmdNvdGetNvlogInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdNvdGetNvlogBufferInfo(pRmCliRes, pParams) cliresCtrlCmdNvdGetNvlogBufferInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdNvdGetNvlog(pRmCliRes, pParams) cliresCtrlCmdNvdGetNvlog_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdNvdGetRcerrRpt(pRmCliRes, pReportParams) cliresCtrlCmdNvdGetRcerrRpt_DISPATCH(pRmCliRes, pReportParams)
#define cliresCtrlCmdClientGetAddrSpaceType(pRmCliRes, pParams) cliresCtrlCmdClientGetAddrSpaceType_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdClientGetHandleInfo(pRmCliRes, pParams) cliresCtrlCmdClientGetHandleInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdClientGetAccessRights(pRmCliRes, pParams) cliresCtrlCmdClientGetAccessRights_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdClientSetInheritedSharePolicy(pRmCliRes, pParams) cliresCtrlCmdClientSetInheritedSharePolicy_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdClientShareObject(pRmCliRes, pParams) cliresCtrlCmdClientShareObject_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdClientGetChildHandle(pRmCliRes, pParams) cliresCtrlCmdClientGetChildHandle_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdObjectsAreDuplicates(pRmCliRes, pParams) cliresCtrlCmdObjectsAreDuplicates_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuGetAttachedIds(pRmCliRes, pGpuAttachedIds) cliresCtrlCmdGpuGetAttachedIds_DISPATCH(pRmCliRes, pGpuAttachedIds)
#define cliresCtrlCmdGpuGetIdInfo(pRmCliRes, pGpuIdInfoParams) cliresCtrlCmdGpuGetIdInfo_DISPATCH(pRmCliRes, pGpuIdInfoParams)
#define cliresCtrlCmdGpuGetIdInfoV2(pRmCliRes, pGpuIdInfoParams) cliresCtrlCmdGpuGetIdInfoV2_DISPATCH(pRmCliRes, pGpuIdInfoParams)
#define cliresCtrlCmdGpuGetInitStatus(pRmCliRes, pGpuInitStatusParams) cliresCtrlCmdGpuGetInitStatus_DISPATCH(pRmCliRes, pGpuInitStatusParams)
#define cliresCtrlCmdGpuGetDeviceIds(pRmCliRes, pDeviceIdsParams) cliresCtrlCmdGpuGetDeviceIds_DISPATCH(pRmCliRes, pDeviceIdsParams)
#define cliresCtrlCmdGpuGetActiveDeviceIds(pRmCliRes, pActiveDeviceIdsParams) cliresCtrlCmdGpuGetActiveDeviceIds_DISPATCH(pRmCliRes, pActiveDeviceIdsParams)
#define cliresCtrlCmdGpuGetProbedIds(pRmCliRes, pGpuProbedIds) cliresCtrlCmdGpuGetProbedIds_DISPATCH(pRmCliRes, pGpuProbedIds)
#define cliresCtrlCmdGpuAttachIds(pRmCliRes, pGpuAttachIds) cliresCtrlCmdGpuAttachIds_DISPATCH(pRmCliRes, pGpuAttachIds)
#define cliresCtrlCmdGpuDetachIds(pRmCliRes, pGpuDetachIds) cliresCtrlCmdGpuDetachIds_DISPATCH(pRmCliRes, pGpuDetachIds)
#define cliresCtrlCmdGpuGetSvmSize(pRmCliRes, pSvmSizeGetParams) cliresCtrlCmdGpuGetSvmSize_DISPATCH(pRmCliRes, pSvmSizeGetParams)
#define cliresCtrlCmdGpuGetPciInfo(pRmCliRes, pPciInfoParams) cliresCtrlCmdGpuGetPciInfo_DISPATCH(pRmCliRes, pPciInfoParams)
#define cliresCtrlCmdGpuGetUuidInfo(pRmCliRes, pParams) cliresCtrlCmdGpuGetUuidInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuGetUuidFromGpuId(pRmCliRes, pParams) cliresCtrlCmdGpuGetUuidFromGpuId_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuModifyGpuDrainState(pRmCliRes, pParams) cliresCtrlCmdGpuModifyGpuDrainState_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuQueryGpuDrainState(pRmCliRes, pParams) cliresCtrlCmdGpuQueryGpuDrainState_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuGetMemOpEnable(pRmCliRes, pMemOpEnableParams) cliresCtrlCmdGpuGetMemOpEnable_DISPATCH(pRmCliRes, pMemOpEnableParams)
#define cliresCtrlCmdGpuDisableNvlinkInit(pRmCliRes, pParams) cliresCtrlCmdGpuDisableNvlinkInit_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuSetNvlinkBwMode(pRmCliRes, pParams) cliresCtrlCmdGpuSetNvlinkBwMode_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuGetNvlinkBwMode(pRmCliRes, pParams) cliresCtrlCmdGpuGetNvlinkBwMode_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdLegacyConfig(pRmCliRes, pParams) cliresCtrlCmdLegacyConfig_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdIdleChannels(pRmCliRes, pParams) cliresCtrlCmdIdleChannels_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdPushGspUcode(pRmCliRes, pParams) cliresCtrlCmdPushGspUcode_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuGetVideoLinks(pRmCliRes, pParams) cliresCtrlCmdGpuGetVideoLinks_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGsyncGetAttachedIds(pRmCliRes, pGsyncAttachedIds) cliresCtrlCmdGsyncGetAttachedIds_DISPATCH(pRmCliRes, pGsyncAttachedIds)
#define cliresCtrlCmdGsyncGetIdInfo(pRmCliRes, pGsyncIdInfoParams) cliresCtrlCmdGsyncGetIdInfo_DISPATCH(pRmCliRes, pGsyncIdInfoParams)
#define cliresCtrlCmdEventSetNotification(pRmCliRes, pEventSetNotificationParams) cliresCtrlCmdEventSetNotification_DISPATCH(pRmCliRes, pEventSetNotificationParams)
#define cliresCtrlCmdEventGetSystemEventStatus(pRmCliRes, pSystemEventStatusParams) cliresCtrlCmdEventGetSystemEventStatus_DISPATCH(pRmCliRes, pSystemEventStatusParams)
#define cliresCtrlCmdOsUnixExportObjectToFd(pRmCliRes, pParams) cliresCtrlCmdOsUnixExportObjectToFd_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixImportObjectFromFd(pRmCliRes, pParams) cliresCtrlCmdOsUnixImportObjectFromFd_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixGetExportObjectInfo(pRmCliRes, pParams) cliresCtrlCmdOsUnixGetExportObjectInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixCreateExportObjectFd(pRmCliRes, pParams) cliresCtrlCmdOsUnixCreateExportObjectFd_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixExportObjectsToFd(pRmCliRes, pParams) cliresCtrlCmdOsUnixExportObjectsToFd_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixImportObjectsFromFd(pRmCliRes, pParams) cliresCtrlCmdOsUnixImportObjectsFromFd_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdOsUnixFlushUserCache(pRmCliRes, pAddressSpaceParams) cliresCtrlCmdOsUnixFlushUserCache_DISPATCH(pRmCliRes, pAddressSpaceParams)
#define cliresCtrlCmdGpuAcctSetAccountingState(pRmCliRes, pParams) cliresCtrlCmdGpuAcctSetAccountingState_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuAcctGetAccountingState(pRmCliRes, pParams) cliresCtrlCmdGpuAcctGetAccountingState_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdGpuAcctGetProcAccountingInfo(pRmCliRes, pAcctInfoParams) cliresCtrlCmdGpuAcctGetProcAccountingInfo_DISPATCH(pRmCliRes, pAcctInfoParams)
#define cliresCtrlCmdGpuAcctGetAccountingPids(pRmCliRes, pAcctPidsParams) cliresCtrlCmdGpuAcctGetAccountingPids_DISPATCH(pRmCliRes, pAcctPidsParams)
#define cliresCtrlCmdGpuAcctClearAccountingData(pRmCliRes, pParams) cliresCtrlCmdGpuAcctClearAccountingData_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSetSubProcessID(pRmCliRes, pParams) cliresCtrlCmdSetSubProcessID_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdDisableSubProcessUserdIsolation(pRmCliRes, pParams) cliresCtrlCmdDisableSubProcessUserdIsolation_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSyncGpuBoostInfo(pRmCliRes, pParams) cliresCtrlCmdSyncGpuBoostInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSyncGpuBoostGroupCreate(pRmCliRes, pParams) cliresCtrlCmdSyncGpuBoostGroupCreate_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSyncGpuBoostGroupDestroy(pRmCliRes, pParams) cliresCtrlCmdSyncGpuBoostGroupDestroy_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSyncGpuBoostGroupInfo(pRmCliRes, pParams) cliresCtrlCmdSyncGpuBoostGroupInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdVgpuGetStartData(pRmCliRes, pVgpuStartParams) cliresCtrlCmdVgpuGetStartData_DISPATCH(pRmCliRes, pVgpuStartParams)
#define cliresCtrlCmdVgpuGetVgpuVersion(pRmCliRes, vgpuVersionInfo) cliresCtrlCmdVgpuGetVgpuVersion_DISPATCH(pRmCliRes, vgpuVersionInfo)
#define cliresCtrlCmdVgpuSetVgpuVersion(pRmCliRes, vgpuVersionInfo) cliresCtrlCmdVgpuSetVgpuVersion_DISPATCH(pRmCliRes, vgpuVersionInfo)
#define cliresCtrlCmdSystemNVPCFGetPowerModeInfo(pRmCliRes, pParams) cliresCtrlCmdSystemNVPCFGetPowerModeInfo_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemSyncExternalFabricMgmt(pRmCliRes, pExtFabricMgmtParams) cliresCtrlCmdSystemSyncExternalFabricMgmt_DISPATCH(pRmCliRes, pExtFabricMgmtParams)
#define cliresCtrlCmdSystemPfmreqhndlrCtrl(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrCtrl_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemPfmreqhndlrGetFrmData(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrGetFrmData_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemPfmreqhndlrSetFrmData(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrSetFrmData_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemPfmreqhndlrCallAcpi(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrCallAcpi_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemPfmreqhndlrControl(pRmCliRes, controlParams) cliresCtrlCmdSystemPfmreqhndlrControl_DISPATCH(pRmCliRes, controlParams)
#define cliresCtrlCmdSystemPfmreqhndlrBatchControl(pRmCliRes, controlParams) cliresCtrlCmdSystemPfmreqhndlrBatchControl_DISPATCH(pRmCliRes, controlParams)
#define cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters_DISPATCH(pRmCliRes, pParams)
#define cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters(pRmCliRes, pParams) cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters_DISPATCH(pRmCliRes, pParams)
#define cliresControl(pResource, pCallContext, pParams) cliresControl_DISPATCH(pResource, pCallContext, pParams)
#define cliresUnmap(pResource, pCallContext, pCpuMapping) cliresUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define cliresMapTo(pResource, pParams) cliresMapTo_DISPATCH(pResource, pParams)
#define cliresSetNotificationShare(pNotifier, pNotifShare) cliresSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define cliresControlFilter(pResource, pCallContext, pParams) cliresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define cliresAddAdditionalDependants(pClient, pResource, pReference) cliresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define cliresGetRefCount(pResource) cliresGetRefCount_DISPATCH(pResource)
#define cliresUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) cliresUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define cliresControlSerialization_Prologue(pResource, pCallContext, pParams) cliresControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define cliresCanCopy(pResource) cliresCanCopy_DISPATCH(pResource)
#define cliresPreDestruct(pResource) cliresPreDestruct_DISPATCH(pResource)
#define cliresUnmapFrom(pResource, pParams) cliresUnmapFrom_DISPATCH(pResource, pParams)
#define cliresIsDuplicate(pResource, hMemory, pDuplicate) cliresIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define cliresControlSerialization_Epilogue(pResource, pCallContext, pParams) cliresControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define cliresGetNotificationListPtr(pNotifier) cliresGetNotificationListPtr_DISPATCH(pNotifier)
#define cliresGetNotificationShare(pNotifier) cliresGetNotificationShare_DISPATCH(pNotifier)
#define cliresControlLookup(pResource, pParams, ppEntry) cliresControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define cliresMap(pResource, pCallContext, pParams, pCpuMapping) cliresMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define cliresGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) cliresGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NvBool cliresAccessCallback_IMPL(struct RmClientResource *pRmCliRes, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);

static inline NvBool cliresAccessCallback_DISPATCH(struct RmClientResource *pRmCliRes, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pRmCliRes->__cliresAccessCallback__(pRmCliRes, pInvokingClient, pAllocParams, accessRight);
}

NvBool cliresShareCallback_IMPL(struct RmClientResource *pRmCliRes, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

static inline NvBool cliresShareCallback_DISPATCH(struct RmClientResource *pRmCliRes, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pRmCliRes->__cliresShareCallback__(pRmCliRes, pInvokingClient, pParentRef, pSharePolicy);
}

NV_STATUS cliresControl_Prologue_IMPL(struct RmClientResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS cliresControl_Prologue_DISPATCH(struct RmClientResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pRmCliRes->__cliresControl_Prologue__(pRmCliRes, pCallContext, pParams);
}

void cliresControl_Epilogue_IMPL(struct RmClientResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline void cliresControl_Epilogue_DISPATCH(struct RmClientResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pRmCliRes->__cliresControl_Epilogue__(pRmCliRes, pCallContext, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetCpuInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS *pCpuInfoParams);

static inline NV_STATUS cliresCtrlCmdSystemGetCpuInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS *pCpuInfoParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetCpuInfo__(pRmCliRes, pCpuInfoParams);
}

NV_STATUS cliresCtrlCmdSystemGetFeatures_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetFeatures_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetFeatures__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetBuildVersionV2_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetBuildVersionV2_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetBuildVersionV2__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemExecuteAcpiMethod_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams);

static inline NV_STATUS cliresCtrlCmdSystemExecuteAcpiMethod_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams) {
    return pRmCliRes->__cliresCtrlCmdSystemExecuteAcpiMethod__(pRmCliRes, pAcpiMethodParams);
}

NV_STATUS cliresCtrlCmdSystemGetChipsetInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS *pChipsetInfo);

static inline NV_STATUS cliresCtrlCmdSystemGetChipsetInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS *pChipsetInfo) {
    return pRmCliRes->__cliresCtrlCmdSystemGetChipsetInfo__(pRmCliRes, pChipsetInfo);
}

NV_STATUS cliresCtrlCmdSystemSetMemorySize_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemSetMemorySize_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemSetMemorySize__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetLockTimes_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetLockTimes_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetLockTimes__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetClassList_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetClassList_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetClassList__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemNotifyEvent_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemNotifyEvent_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemNotifyEvent__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetPlatformType_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS *pSysParams);

static inline NV_STATUS cliresCtrlCmdSystemGetPlatformType_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS *pSysParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetPlatformType__(pRmCliRes, pSysParams);
}

NV_STATUS cliresCtrlCmdSystemDebugCtrlRmMsg_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemDebugCtrlRmMsg_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemDebugCtrlRmMsg__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGpsControl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS *controlParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsControl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS *controlParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsControl__(pRmCliRes, controlParams);
}

NV_STATUS cliresCtrlCmdSystemGpsBatchControl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS *controlParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsBatchControl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS *controlParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsBatchControl__(pRmCliRes, controlParams);
}

NV_STATUS cliresCtrlCmdSystemGetHwbcInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetHwbcInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetHwbcInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetP2pCaps_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pP2PParams);

static inline NV_STATUS cliresCtrlCmdSystemGetP2pCaps_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pP2PParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetP2pCaps__(pRmCliRes, pP2PParams);
}

NV_STATUS cliresCtrlCmdSystemGetP2pCapsV2_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pP2PParams);

static inline NV_STATUS cliresCtrlCmdSystemGetP2pCapsV2_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pP2PParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetP2pCapsV2__(pRmCliRes, pP2PParams);
}

NV_STATUS cliresCtrlCmdSystemGetP2pCapsMatrix_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pP2PParams);

static inline NV_STATUS cliresCtrlCmdSystemGetP2pCapsMatrix_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pP2PParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetP2pCapsMatrix__(pRmCliRes, pP2PParams);
}

NV_STATUS cliresCtrlCmdSystemGpsCtrl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsCtrl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsCtrl__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGpsGetFrmData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsGetFrmData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsGetFrmData__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGpsSetFrmData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsSetFrmData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsSetFrmData__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGpsCallAcpi_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGpsCallAcpi_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGpsCallAcpi__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetPerfSensorCounters_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetPerfSensorCounters_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetPerfSensorCounters__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetExtendedPerfSensorCounters_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetExtendedPerfSensorCounters_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetExtendedPerfSensorCounters__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetGpusPowerStatus_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS *pGpusPowerStatus);

static inline NV_STATUS cliresCtrlCmdSystemGetGpusPowerStatus_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS *pGpusPowerStatus) {
    return pRmCliRes->__cliresCtrlCmdSystemGetGpusPowerStatus__(pRmCliRes, pGpusPowerStatus);
}

NV_STATUS cliresCtrlCmdSystemGetPrivilegedStatus_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetPrivilegedStatus_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetPrivilegedStatus__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetFabricStatus_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetFabricStatus_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetFabricStatus__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemGetRmInstanceId_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS *pRmInstanceIdParams);

static inline NV_STATUS cliresCtrlCmdSystemGetRmInstanceId_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS *pRmInstanceIdParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetRmInstanceId__(pRmCliRes, pRmInstanceIdParams);
}

NV_STATUS cliresCtrlCmdSystemGetClientDatabaseInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemGetClientDatabaseInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemGetClientDatabaseInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemRmctrlCacheModeCtrl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemRmctrlCacheModeCtrl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemRmctrlCacheModeCtrl__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdNvdGetDumpSize_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams);

static inline NV_STATUS cliresCtrlCmdNvdGetDumpSize_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetDumpSize__(pRmCliRes, pDumpSizeParams);
}

NV_STATUS cliresCtrlCmdNvdGetDump_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams);

static inline NV_STATUS cliresCtrlCmdNvdGetDump_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetDump__(pRmCliRes, pDumpParams);
}

NV_STATUS cliresCtrlCmdNvdGetTimestamp_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS *pTimestampParams);

static inline NV_STATUS cliresCtrlCmdNvdGetTimestamp_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS *pTimestampParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetTimestamp__(pRmCliRes, pTimestampParams);
}

NV_STATUS cliresCtrlCmdNvdGetNvlogInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdNvdGetNvlogInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetNvlogInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdNvdGetNvlogBufferInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdNvdGetNvlogBufferInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetNvlogBufferInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdNvdGetNvlog_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdNvdGetNvlog_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_NVD_GET_NVLOG_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetNvlog__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdNvdGetRcerrRpt_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pReportParams);

static inline NV_STATUS cliresCtrlCmdNvdGetRcerrRpt_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pReportParams) {
    return pRmCliRes->__cliresCtrlCmdNvdGetRcerrRpt__(pRmCliRes, pReportParams);
}

NV_STATUS cliresCtrlCmdClientGetAddrSpaceType_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientGetAddrSpaceType_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientGetAddrSpaceType__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdClientGetHandleInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientGetHandleInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientGetHandleInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdClientGetAccessRights_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientGetAccessRights_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientGetAccessRights__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdClientSetInheritedSharePolicy_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientSetInheritedSharePolicy_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientSetInheritedSharePolicy__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdClientShareObject_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientShareObject_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientShareObject__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdClientGetChildHandle_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdClientGetChildHandle_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdClientGetChildHandle__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdObjectsAreDuplicates_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdObjectsAreDuplicates_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdObjectsAreDuplicates__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuGetAttachedIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *pGpuAttachedIds);

static inline NV_STATUS cliresCtrlCmdGpuGetAttachedIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *pGpuAttachedIds) {
    return pRmCliRes->__cliresCtrlCmdGpuGetAttachedIds__(pRmCliRes, pGpuAttachedIds);
}

NV_STATUS cliresCtrlCmdGpuGetIdInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *pGpuIdInfoParams);

static inline NV_STATUS cliresCtrlCmdGpuGetIdInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *pGpuIdInfoParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetIdInfo__(pRmCliRes, pGpuIdInfoParams);
}

NV_STATUS cliresCtrlCmdGpuGetIdInfoV2_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *pGpuIdInfoParams);

static inline NV_STATUS cliresCtrlCmdGpuGetIdInfoV2_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *pGpuIdInfoParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetIdInfoV2__(pRmCliRes, pGpuIdInfoParams);
}

NV_STATUS cliresCtrlCmdGpuGetInitStatus_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *pGpuInitStatusParams);

static inline NV_STATUS cliresCtrlCmdGpuGetInitStatus_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *pGpuInitStatusParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetInitStatus__(pRmCliRes, pGpuInitStatusParams);
}

NV_STATUS cliresCtrlCmdGpuGetDeviceIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS *pDeviceIdsParams);

static inline NV_STATUS cliresCtrlCmdGpuGetDeviceIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS *pDeviceIdsParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetDeviceIds__(pRmCliRes, pDeviceIdsParams);
}

NV_STATUS cliresCtrlCmdGpuGetActiveDeviceIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *pActiveDeviceIdsParams);

static inline NV_STATUS cliresCtrlCmdGpuGetActiveDeviceIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *pActiveDeviceIdsParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetActiveDeviceIds__(pRmCliRes, pActiveDeviceIdsParams);
}

NV_STATUS cliresCtrlCmdGpuGetProbedIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds);

static inline NV_STATUS cliresCtrlCmdGpuGetProbedIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *pGpuProbedIds) {
    return pRmCliRes->__cliresCtrlCmdGpuGetProbedIds__(pRmCliRes, pGpuProbedIds);
}

NV_STATUS cliresCtrlCmdGpuAttachIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_ATTACH_IDS_PARAMS *pGpuAttachIds);

static inline NV_STATUS cliresCtrlCmdGpuAttachIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_ATTACH_IDS_PARAMS *pGpuAttachIds) {
    return pRmCliRes->__cliresCtrlCmdGpuAttachIds__(pRmCliRes, pGpuAttachIds);
}

NV_STATUS cliresCtrlCmdGpuDetachIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_DETACH_IDS_PARAMS *pGpuDetachIds);

static inline NV_STATUS cliresCtrlCmdGpuDetachIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_DETACH_IDS_PARAMS *pGpuDetachIds) {
    return pRmCliRes->__cliresCtrlCmdGpuDetachIds__(pRmCliRes, pGpuDetachIds);
}

NV_STATUS cliresCtrlCmdGpuGetSvmSize_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS *pSvmSizeGetParams);

static inline NV_STATUS cliresCtrlCmdGpuGetSvmSize_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_SVM_SIZE_PARAMS *pSvmSizeGetParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetSvmSize__(pRmCliRes, pSvmSizeGetParams);
}

NV_STATUS cliresCtrlCmdGpuGetPciInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS *pPciInfoParams);

static inline NV_STATUS cliresCtrlCmdGpuGetPciInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS *pPciInfoParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetPciInfo__(pRmCliRes, pPciInfoParams);
}

NV_STATUS cliresCtrlCmdGpuGetUuidInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuGetUuidInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetUuidInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuGetUuidFromGpuId_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuGetUuidFromGpuId_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetUuidFromGpuId__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuModifyGpuDrainState_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuModifyGpuDrainState_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuModifyGpuDrainState__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuQueryGpuDrainState_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuQueryGpuDrainState_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuQueryGpuDrainState__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuGetMemOpEnable_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS *pMemOpEnableParams);

static inline NV_STATUS cliresCtrlCmdGpuGetMemOpEnable_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS *pMemOpEnableParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetMemOpEnable__(pRmCliRes, pMemOpEnableParams);
}

NV_STATUS cliresCtrlCmdGpuDisableNvlinkInit_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuDisableNvlinkInit_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_DISABLE_NVLINK_INIT_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuDisableNvlinkInit__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuSetNvlinkBwMode_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuSetNvlinkBwMode_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuSetNvlinkBwMode__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuGetNvlinkBwMode_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuGetNvlinkBwMode_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetNvlinkBwMode__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdLegacyConfig_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdLegacyConfig_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_LEGACY_CONFIG_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdLegacyConfig__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdIdleChannels_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdIdleChannels_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdIdleChannels__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdPushGspUcode_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_PUSH_GSP_UCODE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdPushGspUcode_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_PUSH_GSP_UCODE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdPushGspUcode__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuGetVideoLinks_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuGetVideoLinks_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuGetVideoLinks__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGsyncGetAttachedIds_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *pGsyncAttachedIds);

static inline NV_STATUS cliresCtrlCmdGsyncGetAttachedIds_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *pGsyncAttachedIds) {
    return pRmCliRes->__cliresCtrlCmdGsyncGetAttachedIds__(pRmCliRes, pGsyncAttachedIds);
}

NV_STATUS cliresCtrlCmdGsyncGetIdInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *pGsyncIdInfoParams);

static inline NV_STATUS cliresCtrlCmdGsyncGetIdInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *pGsyncIdInfoParams) {
    return pRmCliRes->__cliresCtrlCmdGsyncGetIdInfo__(pRmCliRes, pGsyncIdInfoParams);
}

NV_STATUS cliresCtrlCmdEventSetNotification_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pEventSetNotificationParams);

static inline NV_STATUS cliresCtrlCmdEventSetNotification_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pEventSetNotificationParams) {
    return pRmCliRes->__cliresCtrlCmdEventSetNotification__(pRmCliRes, pEventSetNotificationParams);
}

NV_STATUS cliresCtrlCmdEventGetSystemEventStatus_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS *pSystemEventStatusParams);

static inline NV_STATUS cliresCtrlCmdEventGetSystemEventStatus_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GET_SYSTEM_EVENT_STATUS_PARAMS *pSystemEventStatusParams) {
    return pRmCliRes->__cliresCtrlCmdEventGetSystemEventStatus__(pRmCliRes, pSystemEventStatusParams);
}

NV_STATUS cliresCtrlCmdOsUnixExportObjectToFd_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixExportObjectToFd_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixExportObjectToFd__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixImportObjectFromFd_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixImportObjectFromFd_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixImportObjectFromFd__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixGetExportObjectInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixGetExportObjectInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixGetExportObjectInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixCreateExportObjectFd_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixCreateExportObjectFd_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixCreateExportObjectFd__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixExportObjectsToFd_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixExportObjectsToFd_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixExportObjectsToFd__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixImportObjectsFromFd_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdOsUnixImportObjectsFromFd_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixImportObjectsFromFd__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdOsUnixFlushUserCache_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS *pAddressSpaceParams);

static inline NV_STATUS cliresCtrlCmdOsUnixFlushUserCache_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS *pAddressSpaceParams) {
    return pRmCliRes->__cliresCtrlCmdOsUnixFlushUserCache__(pRmCliRes, pAddressSpaceParams);
}

NV_STATUS cliresCtrlCmdGpuAcctSetAccountingState_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuAcctSetAccountingState_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuAcctSetAccountingState__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuAcctGetAccountingState_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuAcctGetAccountingState_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuAcctGetAccountingState__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdGpuAcctGetProcAccountingInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *pAcctInfoParams);

static inline NV_STATUS cliresCtrlCmdGpuAcctGetProcAccountingInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *pAcctInfoParams) {
    return pRmCliRes->__cliresCtrlCmdGpuAcctGetProcAccountingInfo__(pRmCliRes, pAcctInfoParams);
}

NV_STATUS cliresCtrlCmdGpuAcctGetAccountingPids_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *pAcctPidsParams);

static inline NV_STATUS cliresCtrlCmdGpuAcctGetAccountingPids_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *pAcctPidsParams) {
    return pRmCliRes->__cliresCtrlCmdGpuAcctGetAccountingPids__(pRmCliRes, pAcctPidsParams);
}

NV_STATUS cliresCtrlCmdGpuAcctClearAccountingData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdGpuAcctClearAccountingData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdGpuAcctClearAccountingData__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSetSubProcessID_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSetSubProcessID_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSetSubProcessID__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdDisableSubProcessUserdIsolation_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdDisableSubProcessUserdIsolation_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdDisableSubProcessUserdIsolation__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSyncGpuBoostInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSyncGpuBoostInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSyncGpuBoostInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSyncGpuBoostGroupCreate_IMPL(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSyncGpuBoostGroupCreate_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSyncGpuBoostGroupCreate__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSyncGpuBoostGroupDestroy_IMPL(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSyncGpuBoostGroupDestroy_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSyncGpuBoostGroupDestroy__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSyncGpuBoostGroupInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSyncGpuBoostGroupInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSyncGpuBoostGroupInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdVgpuGetStartData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_GET_START_DATA_PARAMS *pVgpuStartParams);

static inline NV_STATUS cliresCtrlCmdVgpuGetStartData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_GET_START_DATA_PARAMS *pVgpuStartParams) {
    return pRmCliRes->__cliresCtrlCmdVgpuGetStartData__(pRmCliRes, pVgpuStartParams);
}

NV_STATUS cliresCtrlCmdVgpuGetVgpuVersion_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS *vgpuVersionInfo);

static inline NV_STATUS cliresCtrlCmdVgpuGetVgpuVersion_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS *vgpuVersionInfo) {
    return pRmCliRes->__cliresCtrlCmdVgpuGetVgpuVersion__(pRmCliRes, vgpuVersionInfo);
}

NV_STATUS cliresCtrlCmdVgpuSetVgpuVersion_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS *vgpuVersionInfo);

static inline NV_STATUS cliresCtrlCmdVgpuSetVgpuVersion_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS *vgpuVersionInfo) {
    return pRmCliRes->__cliresCtrlCmdVgpuSetVgpuVersion__(pRmCliRes, vgpuVersionInfo);
}

NV_STATUS cliresCtrlCmdSystemNVPCFGetPowerModeInfo_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemNVPCFGetPowerModeInfo_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemNVPCFGetPowerModeInfo__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemSyncExternalFabricMgmt_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS *pExtFabricMgmtParams);

static inline NV_STATUS cliresCtrlCmdSystemSyncExternalFabricMgmt_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS *pExtFabricMgmtParams) {
    return pRmCliRes->__cliresCtrlCmdSystemSyncExternalFabricMgmt__(pRmCliRes, pExtFabricMgmtParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCtrl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCtrl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrCtrl__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetFrmData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetFrmData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrGetFrmData__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrSetFrmData_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrSetFrmData_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrSetFrmData__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCallAcpi_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCallAcpi_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrCallAcpi__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrControl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS *controlParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrControl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS *controlParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrControl__(pRmCliRes, controlParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrBatchControl_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS *controlParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrBatchControl_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS *controlParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrBatchControl__(pRmCliRes, controlParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters__(pRmCliRes, pParams);
}

NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters_IMPL(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams);

static inline NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters_DISPATCH(struct RmClientResource *pRmCliRes, NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams) {
    return pRmCliRes->__cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters__(pRmCliRes, pParams);
}

static inline NV_STATUS cliresControl_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cliresControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cliresUnmap_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__cliresUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS cliresMapTo_DISPATCH(struct RmClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__cliresMapTo__(pResource, pParams);
}

static inline void cliresSetNotificationShare_DISPATCH(struct RmClientResource *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__cliresSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS cliresControlFilter_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cliresControlFilter__(pResource, pCallContext, pParams);
}

static inline void cliresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RmClientResource *pResource, RsResourceRef *pReference) {
    pResource->__cliresAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 cliresGetRefCount_DISPATCH(struct RmClientResource *pResource) {
    return pResource->__cliresGetRefCount__(pResource);
}

static inline NV_STATUS cliresUnregisterEvent_DISPATCH(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__cliresUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS cliresControlSerialization_Prologue_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cliresControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool cliresCanCopy_DISPATCH(struct RmClientResource *pResource) {
    return pResource->__cliresCanCopy__(pResource);
}

static inline void cliresPreDestruct_DISPATCH(struct RmClientResource *pResource) {
    pResource->__cliresPreDestruct__(pResource);
}

static inline NV_STATUS cliresUnmapFrom_DISPATCH(struct RmClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__cliresUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS cliresIsDuplicate_DISPATCH(struct RmClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__cliresIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void cliresControlSerialization_Epilogue_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__cliresControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *cliresGetNotificationListPtr_DISPATCH(struct RmClientResource *pNotifier) {
    return pNotifier->__cliresGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *cliresGetNotificationShare_DISPATCH(struct RmClientResource *pNotifier) {
    return pNotifier->__cliresGetNotificationShare__(pNotifier);
}

static inline NV_STATUS cliresControlLookup_DISPATCH(struct RmClientResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__cliresControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS cliresMap_DISPATCH(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__cliresMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS cliresGetOrAllocNotifShare_DISPATCH(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__cliresGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS cliresConstruct_IMPL(struct RmClientResource *arg_pRmCliRes, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_cliresConstruct(arg_pRmCliRes, arg_pCallContext, arg_pParams) cliresConstruct_IMPL(arg_pRmCliRes, arg_pCallContext, arg_pParams)
void cliresDestruct_IMPL(struct RmClientResource *pRmCliRes);

#define __nvoc_cliresDestruct(pRmCliRes) cliresDestruct_IMPL(pRmCliRes)
#undef PRIVATE_FIELD


NV_STATUS CliGetSystemP2pCaps(NvU32 *gpuIds,
                              NvU32 gpuCount,
                              NvU32 *p2pCaps,
                              NvU32 *p2pOptimalReadCEs,
                              NvU32 *p2pOptimalWriteCEs,
                              NvU8 *p2pCapsStatus,
                              NvU32 *pBusPeerIds);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CLIENT_RESOURCE_NVOC_H_
