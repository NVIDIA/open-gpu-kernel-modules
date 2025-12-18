
#ifndef _G_SUBDEVICE_NVOC_H_
#define _G_SUBDEVICE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_subdevice_nvoc.h"

#ifndef _SUBDEVICE_H_
#define _SUBDEVICE_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"
#include "containers/btree.h"
#include "nvoc/utility.h"
#include "gpu/gpu_halspec.h"

#include "class/cl2080.h"
#include "ctrl/ctrl2080.h" // rmcontrol parameters

typedef struct TMR_EVENT TMR_EVENT;


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



struct P2PApi;

#ifndef __NVOC_CLASS_P2PApi_TYPEDEF__
#define __NVOC_CLASS_P2PApi_TYPEDEF__
typedef struct P2PApi P2PApi;
#endif /* __NVOC_CLASS_P2PApi_TYPEDEF__ */

#ifndef __nvoc_class_id_P2PApi
#define __nvoc_class_id_P2PApi 0x3982b7
#endif /* __nvoc_class_id_P2PApi */



/**
 * A subdevice represents a single GPU within a device. Subdevice provide
 * unicast semantics; that is, operations involving a subdevice are applied to
 * the associated GPU only.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SUBDEVICE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Subdevice;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__Subdevice;


struct Subdevice {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Subdevice *__nvoc_metadata_ptr;
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
    struct Subdevice *__nvoc_pbase_Subdevice;    // subdevice

    // Vtable with 50 per-object function pointers
    NV_STATUS (*__subdeviceCtrlCmdBiosGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *);  // halified (2 hals) exported (id=0x20800810) body
    NV_STATUS (*__subdeviceCtrlCmdBiosGetSKUInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800808)
    NV_STATUS (*__subdeviceCtrlCmdBiosGetPostTime__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *);  // halified (2 hals) exported (id=0x20800809) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801829) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *);  // halified (2 hals) exported (id=0x2080182a) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801830) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetC2CInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *);  // halified (2 hals) exported (id=0x2080182b) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetC2CLpwrStats__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *);  // halified (2 hals) exported (id=0x20801831) body
    NV_STATUS (*__subdeviceCtrlCmdBusSetC2CLpwrStateVote__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *);  // halified (2 hals) exported (id=0x20801832) body
    NV_STATUS (*__subdeviceCtrlCmdBusSetP2pMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *);  // halified (2 hals) exported (id=0x2080182e) body
    NV_STATUS (*__subdeviceCtrlCmdBusUnsetP2pMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *);  // halified (2 hals) exported (id=0x2080182f) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20803001)
    NV_STATUS (*__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *);  // halified (2 hals) exported (id=0x20802096)
    NV_STATUS (*__subdeviceCtrlCmdPerfReservePerfmonHw__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *);  // halified (2 hals) exported (id=0x20802093) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetLevelInfo_V2__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *);  // halified (2 hals) exported (id=0x2080200b) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetCurrentPstate__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *);  // halified (2 hals) exported (id=0x20802068) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *);  // halified (2 hals) exported (id=0x20802087) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetPowerstate__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *);  // halified (2 hals) exported (id=0x2080205a) body
    NV_STATUS (*__subdeviceCtrlCmdPerfNotifyVideoevent__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS *);  // halified (2 hals) exported (id=0x2080205d) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetOfflinedPages__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *);  // halified (2 hals) exported (id=0x20801322) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetLTCInfoForFBP__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *);  // halified (2 hals) exported (id=0x20801328) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetDynamicOfflinedPages__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *);  // halified (2 hals) exported (id=0x20801348) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetStaticConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800a1c) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *);  // halified (2 hals) exported (id=0x20800a6b) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20801355) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20801356) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x20801358) body
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *);  // halified (2 hals) exported (id=0x2080111a) body
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *);  // halified (2 hals) exported (id=0x2080111b) body
    NV_STATUS (*__subdeviceCtrlCmdFifoObjschedGetCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801122) body
    NV_STATUS (*__subdeviceCtrlCmdFifoConfigCtxswTimeout__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_CONFIG_CTXSW_TIMEOUT_PARAMS *);  // halified (2 hals) exported (id=0x20801110) body
    NV_STATUS (*__subdeviceCtrlCmdFifoGetDeviceInfoTable__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *);  // halified (2 hals) exported (id=0x20801112) body
    NV_STATUS (*__subdeviceCtrlCmdFifoUpdateChannelInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20801116) body
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPmMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *);  // halified (2 hals) exported (id=0x20801207) body
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x2080012f) body
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryIllumSupport__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *);  // halified (2 hals) exported (id=0x20800153) body
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryScrubberStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x2080015f) body
    NV_STATUS (*__subdeviceCtrlCmdSetRcRecovery__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);  // halified (2 hals) exported (id=0x2080220d) body
    NV_STATUS (*__subdeviceCtrlCmdGetRcRecovery__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);  // halified (2 hals) exported (id=0x2080220e) body
    NV_STATUS (*__subdeviceCtrlCmdCeGetCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20802a01)
    NV_STATUS (*__subdeviceCtrlCmdCeGetCapsV2__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x20802a03)
    NV_STATUS (*__subdeviceCtrlCmdCeGetAllCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20802a0a)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20800133) body
    NV_STATUS (*__subdeviceCtrlCmdGpuSetEccConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20800134) body
    NV_STATUS (*__subdeviceCtrlCmdGpuResetEccErrorStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x20800136) body
    NV_STATUS (*__subdeviceCtrlCmdGspGetFeatures__)(struct Subdevice * /*this*/, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *);  // halified (2 hals) exported (id=0x20803601) body
    NV_STATUS (*__subdeviceCtrlCmdBifGetStaticInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800aac) body
    NV_STATUS (*__subdeviceCtrlCmdInternalGetLocalAtsConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800afb) body
    NV_STATUS (*__subdeviceCtrlCmdInternalSetPeerAtsConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800afc) body
    NV_STATUS (*__subdeviceCtrlCmdCcuGetSampleInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800ab2) body

    // Data members
    NvU32 deviceInst;
    NvU32 subDeviceInst;
    struct Device *pDevice;
    NvBool bMaxGrTickFreqRequested;
    NvU64 P2PfbMappedBytes;
    NvU32 notifyActions[198];
    NvHandle hNotifierMemory;
    struct Memory *pNotifierMemory;
    NvHandle hSemMemory;
    NvU32 videoStream4KCount;
    NvU32 videoStreamHDCount;
    NvU32 videoStreamSDCount;
    NvU32 videoStreamLinearCount;
    NvU32 ofaCount;
    NvBool bGpuDebugModeEnabled;
    NvBool bRcWatchdogEnableRequested;
    NvBool bRcWatchdogDisableRequested;
    NvBool bRcWatchdogSoftDisableRequested;
    NvBool bReservePerfMon;
    NvU32 perfBoostIndex;
    NvU32 perfBoostHighRefCount;
    NvU32 perfBoostLowRefCount;
    NvBool perfBoostEntryExists;
    NvBool bLockedClockModeRequested;
    NvU32 bNvlinkErrorInjectionModeRequested;
    NvBool bSchedPolicySet;
    NvBool bGcoffDisallowed;
    NvBool bUpdateTGP;
    NvBool bVFRefCntIncRequested;
    TMR_EVENT *pTimerEvent;
};


// Vtable with 30 per-class function pointers
struct NVOC_VTABLE__Subdevice {
    void (*__subdevicePreDestruct__)(struct Subdevice * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__subdeviceInternalControlForward__)(struct Subdevice * /*this*/, NvU32, void *, NvU32);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__subdeviceControl__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__subdeviceMap__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__subdeviceUnmap__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__subdeviceShareCallback__)(struct Subdevice * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__subdeviceGetRegBaseOffsetAndSize__)(struct Subdevice * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__subdeviceGetMapAddrSpace__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__subdeviceGetInternalObjectHandle__)(struct Subdevice * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__subdeviceAccessCallback__)(struct Subdevice * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__subdeviceGetMemInterMapParams__)(struct Subdevice * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__subdeviceCheckMemInterUnmap__)(struct Subdevice * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__subdeviceGetMemoryMappingDescriptor__)(struct Subdevice * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__subdeviceControlSerialization_Prologue__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__subdeviceControlSerialization_Epilogue__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__subdeviceControl_Prologue__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__subdeviceControl_Epilogue__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__subdeviceCanCopy__)(struct Subdevice * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__subdeviceIsDuplicate__)(struct Subdevice * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__subdeviceControlFilter__)(struct Subdevice * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__subdeviceIsPartialUnmapSupported__)(struct Subdevice * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__subdeviceMapTo__)(struct Subdevice * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__subdeviceUnmapFrom__)(struct Subdevice * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__subdeviceGetRefCount__)(struct Subdevice * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__subdeviceAddAdditionalDependants__)(struct RsClient *, struct Subdevice * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__subdeviceGetNotificationListPtr__)(struct Subdevice * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__subdeviceGetNotificationShare__)(struct Subdevice * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__subdeviceSetNotificationShare__)(struct Subdevice * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__subdeviceUnregisterEvent__)(struct Subdevice * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__subdeviceGetOrAllocNotifShare__)(struct Subdevice * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Subdevice {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__Subdevice vtable;
};

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice;

#define __staticCast_Subdevice(pThis) \
    ((pThis)->__nvoc_pbase_Subdevice)

#ifdef __nvoc_subdevice_h_disabled
#define __dynamicCast_Subdevice(pThis) ((Subdevice*) NULL)
#else //__nvoc_subdevice_h_disabled
#define __dynamicCast_Subdevice(pThis) \
    ((Subdevice*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Subdevice)))
#endif //__nvoc_subdevice_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Subdevice(Subdevice**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Subdevice(Subdevice**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_Subdevice(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Subdevice((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros for implementation functions
NV_STATUS subdeviceConstruct_IMPL(struct Subdevice *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_subdeviceConstruct(arg_pResource, arg_pCallContext, arg_pParams) subdeviceConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)

void subdeviceDestruct_IMPL(struct Subdevice *pResource);
#define __nvoc_subdeviceDestruct(pResource) subdeviceDestruct_IMPL(pResource)

NV_STATUS subdeviceUnsetDynamicBoostLimit_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceUnsetDynamicBoostLimit(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceUnsetDynamicBoostLimit(pSubdevice) subdeviceUnsetDynamicBoostLimit_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

void subdeviceRestoreGrTickFreq_IMPL(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext);
#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceRestoreGrTickFreq(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceRestoreGrTickFreq(pSubdevice, pCallContext) subdeviceRestoreGrTickFreq_IMPL(pSubdevice, pCallContext)
#endif // __nvoc_subdevice_h_disabled

void subdeviceRestoreWatchdog_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceRestoreWatchdog(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceRestoreWatchdog(pSubdevice) subdeviceRestoreWatchdog_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

void subdeviceUnsetGpuDebugMode_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceUnsetGpuDebugMode(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceUnsetGpuDebugMode(pSubdevice) subdeviceUnsetGpuDebugMode_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

void subdeviceReleaseComputeModeReservation_IMPL(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext);
#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceReleaseComputeModeReservation(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceReleaseComputeModeReservation(pSubdevice, pCallContext) subdeviceReleaseComputeModeReservation_IMPL(pSubdevice, pCallContext)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceGetByHandle_IMPL(struct RsClient *pClient, NvHandle hSubdevice, struct Subdevice **ppSubdevice);
#define subdeviceGetByHandle(pClient, hSubdevice, ppSubdevice) subdeviceGetByHandle_IMPL(pClient, hSubdevice, ppSubdevice)

NV_STATUS subdeviceGetByGpu_IMPL(struct RsClient *pClient, struct OBJGPU *pGpu, struct Subdevice **ppSubdevice);
#define subdeviceGetByGpu(pClient, pGpu, ppSubdevice) subdeviceGetByGpu_IMPL(pClient, pGpu, ppSubdevice)

NV_STATUS subdeviceGetByDeviceAndGpu_IMPL(struct RsClient *pClient, struct Device *pDevice, struct OBJGPU *pGpu, struct Subdevice **ppSubdevice);
#define subdeviceGetByDeviceAndGpu(pClient, pDevice, pGpu, ppSubdevice) subdeviceGetByDeviceAndGpu_IMPL(pClient, pDevice, pGpu, ppSubdevice)

NV_STATUS subdeviceGetByInstance_IMPL(struct RsClient *pClient, NvHandle hDevice, NvU32 subDeviceInst, struct Subdevice **ppSubdevice);
#define subdeviceGetByInstance(pClient, hDevice, subDeviceInst, ppSubdevice) subdeviceGetByInstance_IMPL(pClient, hDevice, subDeviceInst, ppSubdevice)

NV_STATUS subdeviceCtrlCmdBiosGetNbsiV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS *pNbsiParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBiosGetNbsiV2(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS *pNbsiParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBiosGetNbsiV2(pSubdevice, pNbsiParams) subdeviceCtrlCmdBiosGetNbsiV2_IMPL(pSubdevice, pNbsiParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBiosGetUefiSupport(pSubdevice, pUEFIParams) subdeviceCtrlCmdBiosGetUefiSupport_IMPL(pSubdevice, pUEFIParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcGetArchInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcGetArchInfo(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcGetArchInfo(pSubdevice, pArchInfoParams) subdeviceCtrlCmdMcGetArchInfo_IMPL(pSubdevice, pArchInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcGetManufacturer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcGetManufacturer(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcGetManufacturer(pSubdevice, pManufacturerParams) subdeviceCtrlCmdMcGetManufacturer_IMPL(pSubdevice, pManufacturerParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcChangeReplayableFaultOwnership(pSubdevice, pReplayableFaultOwnrshpParams) subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL(pSubdevice, pReplayableFaultOwnrshpParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcServiceInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcServiceInterrupts(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcServiceInterrupts(pSubdevice, pServiceInterruptParams) subdeviceCtrlCmdMcServiceInterrupts_IMPL(pSubdevice, pServiceInterruptParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcGetEngineNotificationIntrVectors(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcGetEngineNotificationIntrVectors(pSubdevice, pParams) subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcGetIntrCategorySubtreeMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_INTR_CATEGORY_SUBTREE_MAP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcGetIntrCategorySubtreeMap(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_INTR_CATEGORY_SUBTREE_MAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcGetIntrCategorySubtreeMap(pSubdevice, pParams) subdeviceCtrlCmdMcGetIntrCategorySubtreeMap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMcGetStaticIntrTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMcGetStaticIntrTable(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMcGetStaticIntrTable(pSubdevice, pParams) subdeviceCtrlCmdMcGetStaticIntrTable_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDmaInvalidateTLB(pSubdevice, pParams) subdeviceCtrlCmdDmaInvalidateTLB_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDmaGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDmaGetInfo(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDmaGetInfo(pSubdevice, pDmaInfoParams) subdeviceCtrlCmdDmaGetInfo_IMPL(pSubdevice, pDmaInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPciInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPciInfo(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPciInfo(pSubdevice, pPciInfoParams) subdeviceCtrlCmdBusGetPciInfo_IMPL(pSubdevice, pPciInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetInfoV2(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetInfoV2(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusGetInfoV2_IMPL(pSubdevice, pBusInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPciBarInfo(pSubdevice, pBarInfoParams) subdeviceCtrlCmdBusGetPciBarInfo_IMPL(pSubdevice, pBarInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusSetPcieSpeed(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusSetPcieSpeed_IMPL(pSubdevice, pBusInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusSetPcieLinkWidth(pSubdevice, pLinkWidthParams) subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL(pSubdevice, pLinkWidthParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusServiceGpuMultifunctionState(pSubdevice, pParams) subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPexCounters(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetBFD_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetBFD(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetBFD(pSubdevice, pBusGetBFDParams) subdeviceCtrlCmdBusGetBFD_IMPL(pSubdevice, pBusGetBFDParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetAspmDisableFlags(pSubdevice, pParams) subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusControlPublicAspmBits(pSubdevice, pParams) subdeviceCtrlCmdBusControlPublicAspmBits_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusClearPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusClearPexCounters(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusClearPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexUtilCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusClearPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexUtilCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusFreezePexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusFreezePexCounters(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusFreezePexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusFreezePexCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPexLaneCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexLaneCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusSetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetNvlinkPeerIdMask(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusSetEomParameters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusSetEomParameters(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusSetEomParameters(pSubdevice, pParams) subdeviceCtrlCmdBusSetEomParameters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetUphyDlnCfgSpace(pSubdevice, pParams) subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetEomStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetEomStatus(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetEomStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetEomStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetC2CErrorInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetC2CErrorInfo(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetC2CErrorInfo(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CErrorInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusSysmemAccess_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusSysmemAccess(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusSysmemAccess(pSubdevice, pParams) subdeviceCtrlCmdBusSysmemAccess_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedCounters(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetSupportedCounters(arg_this, arg2) subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedBWMode(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetSupportedBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetBWMode_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetBWMode(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkGetBWMode_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSetBWMode_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSetBWMode(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSetBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkSetBWMode_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLocalDeviceInfo(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetLocalDeviceInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetNvlinkCountersV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetNvlinkCountersV2(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetNvlinkCountersV2(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkCountersV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdClearNvlinkCountersV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdClearNvlinkCountersV2(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdClearNvlinkCountersV2(pSubdevice, pParams) subdeviceCtrlCmdClearNvlinkCountersV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetNvlinkCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdClearNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdClearNvlinkCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdClearNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdClearNvlinkCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetNvlinkStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkErrInfo(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBusGetNvlinkErrInfo(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings(pSubdevice, pParams) subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkInjectSWError_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkInjectSWError(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkInjectSWError(pSubdevice, pParams) subdeviceCtrlCmdNvlinkInjectSWError_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkConfigureL1Toggle_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkConfigureL1Toggle(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkConfigureL1Toggle(pSubdevice, pParams) subdeviceCtrlCmdNvlinkConfigureL1Toggle_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlNvlinkGetL1Toggle_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlNvlinkGetL1Toggle(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlNvlinkGetL1Toggle(pSubdevice, pParams) subdeviceCtrlNvlinkGetL1Toggle_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinGetLinkFomValues(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinGetLinkFomValues(pSubdevice, pParams) subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkEccErrors(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetNvlinkEccErrors(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSetupEom_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSetupEom(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSetupEom(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetupEom_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetPowerState(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetPowerState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkReadTpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkReadTpCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetLpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLpCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkClearLpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkClearLpCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkClearLpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkClearLpCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSetLoopbackMode(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkClearRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkEnableLinks(pSubdevice) subdeviceCtrlCmdNvlinkEnableLinks_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkProcessInitDisabledLinks(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkInbandSendData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkInbandSendData(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkInbandSendData(pSubdevice, pParams) subdeviceCtrlCmdNvlinkInbandSendData_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPostFaultUp_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPostFaultUp(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPostFaultUp(pSubdevice, pParams) subdeviceCtrlCmdNvlinkPostFaultUp_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPostLazyErrorRecovery(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPostLazyErrorRecovery(pSubdevice) subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkEomControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkEomControl(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkEomControl(pSubdevice, pParams) subdeviceCtrlCmdNvlinkEomControl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSetL1Threshold(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSetL1Threshold(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkDirectConnectCheck(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkDirectConnectCheck(pSubdevice, pParams) subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetL1Threshold(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetL1Threshold(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdIsNvlinkReducedConfig(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdIsNvlinkReducedConfig(pSubdevice, pParams) subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetPortEvents_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPortEvents(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetPortEvents(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetPortEvents_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkIsGpuDegraded_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkIsGpuDegraded(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkIsGpuDegraded(pSubdevice, pParams) subdeviceCtrlCmdNvlinkIsGpuDegraded_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSetNvleEnabledState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSetNvleEnabledState(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSetNvleEnabledState(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetNvleEnabledState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPAOS_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPAOS(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPAOS_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLTC_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLTC(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPLTC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPLTC_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPLM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPLM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLC_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLC(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPSLC_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCAM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCAM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMCAM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMCAM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTECR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTECR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTECR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTECR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEWE_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEWE(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTEWE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTEWE_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSDE_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSDE(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTSDE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTSDE_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTCAP_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTCAP(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTCAP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTCAP_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMTU_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMTU(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPMTU(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMTU_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMLP_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMLP(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPMLP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMLP_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessGHPKT_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessGHPKT(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessGHPKT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessGHPKT_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPDDR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPDDR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPDDR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPDDR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPTT_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPTT(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPTT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPTT_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPCNT_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPCNT(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPCNT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPCNT_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMGIR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMGIR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMGIR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMGIR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPAOS_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPAOS(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPAOS_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPHCR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPHCR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPHCR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPHCR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLTP_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLTP(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessSLTP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLTP_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPGUID_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPGUID(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPGUID(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPGUID_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRT_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRT(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPRT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPRT_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTYS_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTYS(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPTYS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPTYS_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLRG_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLRG(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessSLRG(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLRG_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMAOS_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMAOS(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPMAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMAOS_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPLR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPLR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMORD_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMORD(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMORD(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMORD_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEIM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEIM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTEIM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTEIM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIE_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIE(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTIE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTIE_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTIM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTIM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMPSCR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMPSCR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMPSCR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMPSCR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMTSR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTSR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLS_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLS(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPSLS_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMLPC_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMLPC(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMLPC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMLPC_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLIB_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLIB(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPLIB(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPLIB_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCSR_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MCSR_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCSR(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MCSR_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessMCSR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMCSR_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkUpdateNvleTopology(arg_this, arg2) subdeviceCtrlCmdNvlinkUpdateNvleTopology_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleLids_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvleLids(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetNvleLids(arg_this, arg2) subdeviceCtrlCmdNvlinkGetNvleLids_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTASV2_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTASV2(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPTASV2(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPTASV2_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRM_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRM(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccessPPRM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPRM_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkPRMAccess(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccess_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetPlatformInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetPlatformInfo_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS *arg2);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo_IMPL(arg_this, arg2)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL(arg_this, pParams) subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL_IMPL(arg_this, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL(arg_this, pParams) subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL_IMPL(arg_this, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdI2cReadBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdI2cReadBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdI2cReadBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadBuffer_IMPL(pSubdevice, pI2cParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdI2cWriteBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdI2cWriteBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdI2cWriteBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteBuffer_IMPL(pSubdevice, pI2cParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdI2cReadReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdI2cReadReg(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdI2cReadReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadReg_IMPL(pSubdevice, pI2cParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdI2cWriteReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdI2cWriteReg(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdI2cWriteReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteReg_IMPL(pSubdevice, pI2cParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdThermalSystemExecuteV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *pSystemExecuteParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdThermalSystemExecuteV2(struct Subdevice *pSubdevice, NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *pSystemExecuteParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdThermalSystemExecuteV2(pSubdevice, pSystemExecuteParams) subdeviceCtrlCmdThermalSystemExecuteV2_IMPL(pSubdevice, pSystemExecuteParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfRatedTdpGetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL(pSubdevice, pControlParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfRatedTdpSetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL(pSubdevice, pControlParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfSetAuxPowerState(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS *pPowerStateParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfSetAuxPowerState(pSubdevice, pPowerStateParams) subdeviceCtrlCmdPerfSetAuxPowerState_KERNEL(pSubdevice, pPowerStateParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfSetPowerstate(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS *pPowerInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfSetPowerstate(pSubdevice, pPowerInfoParams) subdeviceCtrlCmdPerfSetPowerstate_KERNEL(pSubdevice, pPowerInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdPerfGetTegraPerfmonSample_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_TEGRA_PERFMON_SAMPLE_PARAMS *pSampleParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfGetTegraPerfmonSample(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_TEGRA_PERFMON_SAMPLE_PARAMS *pSampleParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfGetTegraPerfmonSample(pSubdevice, pSampleParams) subdeviceCtrlCmdPerfGetTegraPerfmonSample_IMPL(pSubdevice, pSampleParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKPerfBoost_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKPerfBoost(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKPerfBoost(pSubdevice, pBoostParams) subdeviceCtrlCmdKPerfBoost_IMPL(pSubdevice, pBoostParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetFBRegionInfo(pSubdevice, pGFBRIParams) subdeviceCtrlCmdFbGetFBRegionInfo_IMPL(pSubdevice, pGFBRIParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetBar1Offset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetBar1Offset(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetBar1Offset(pSubdevice, pFbMemParams) subdeviceCtrlCmdFbGetBar1Offset_IMPL(pSubdevice, pFbMemParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbIsKind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbIsKind(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbIsKind(pSubdevice, pIsKindParams) subdeviceCtrlCmdFbIsKind_IMPL(pSubdevice, pIsKindParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetMemAlignment_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetMemAlignment(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetMemAlignment(pSubdevice, pParams) subdeviceCtrlCmdFbGetMemAlignment_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetHeapReservationSize(pSubdevice, pParams) subdeviceCtrlCmdFbGetHeapReservationSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout(pSubdevice, pParams) subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_46f6a7(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetCarveoutRegionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetCarveoutRegionInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetCarveoutRegionInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetCarveoutRegionInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetInfoV2(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetInfoV2(pSubdevice, pFbInfoParams) subdeviceCtrlCmdFbGetInfoV2_IMPL(pSubdevice, pFbInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetCalibrationLockFailed(pSubdevice, pGCLFParams) subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL(pSubdevice, pGCLFParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbFlushGpuCache_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbFlushGpuCache(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbFlushGpuCache(pSubdevice, pCacheFlushParams) subdeviceCtrlCmdFbFlushGpuCache_IMPL(pSubdevice, pCacheFlushParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetGpuCacheInfo(pSubdevice, pGpuCacheParams) subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL(pSubdevice, pGpuCacheParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetCliManagedOfflinedPages(pSubdevice, pOsOfflinedParams) subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL(pSubdevice, pOsOfflinedParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbSetupVprRegion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbSetupVprRegion(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbSetupVprRegion(pSubdevice, pCliReqParams) subdeviceCtrlCmdFbSetupVprRegion_IMPL(pSubdevice, pCliReqParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetCompBitCopyConstructInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbPatchPbrForMining(pSubdevice, pParams) subdeviceCtrlCmdFbPatchPbrForMining_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetRemappedRows_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetRemappedRows(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetRemappedRows(pSubdevice, pRemappedRowsParams) subdeviceCtrlCmdFbGetRemappedRows_IMPL(pSubdevice, pRemappedRowsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetFsInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetFsInfo(pSubdevice, pInfoParams) subdeviceCtrlCmdFbGetFsInfo_IMPL(pSubdevice, pInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetRowRemapperHistogram(pSubdevice, pParams) subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbUpdateNumaStatus(pSubdevice, pParams) subdeviceCtrlCmdFbUpdateNumaStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetNumaInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetNumaInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetNumaInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetNumaInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysSetPartitionableMem(pSubdevice, pParams) subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbSetZbcReferenced(pSubdevice, pParams) subdeviceCtrlCmdFbSetZbcReferenced_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysL2InvalidateEvict(pSubdevice, pParams) subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches(pSubdevice) subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysDisableNvlinkPeers(pSubdevice) subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysProgramRawCompressionMode(pSubdevice, pParams) subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetCtagsForCbcEviction(pSubdevice, pParams) subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbCBCOp_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbCBCOp(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbCBCOp(pSubdevice, pParams) subdeviceCtrlCmdFbCBCOp_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbSetRrd_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbSetRrd(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbSetRrd(pSubdevice, pParams) subdeviceCtrlCmdFbSetRrd_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbSetReadLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbSetReadLimit(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbSetReadLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetReadLimit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbSetWriteLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbSetWriteLimit(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbSetWriteLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetWriteLimit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGmmuCommitTlbInvalidate(struct Subdevice *pSubdevice, NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGmmuCommitTlbInvalidate(pSubdevice, pParams) subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFbGetStaticBar1Info_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFbGetStaticBar1Info(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFbGetStaticBar1Info(pSubdevice, pParams) subdeviceCtrlCmdFbGetStaticBar1Info_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS *pConfig);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport(struct Subdevice *pSubdevice, NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS *pConfig) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_IMPL(pSubdevice, pConfig)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdSetGpfifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdSetGpfifo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdSetGpfifo(pSubdevice, pSetGpFifoParams) subdeviceCtrlCmdSetGpfifo_IMPL(pSubdevice, pSetGpFifoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdSetOperationalProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdSetOperationalProperties(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdSetOperationalProperties(pSubdevice, pSetOperationalProperties) subdeviceCtrlCmdSetOperationalProperties_IMPL(pSubdevice, pSetOperationalProperties)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoBindEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoBindEngines(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoBindEngines(pSubdevice, pBindParams) subdeviceCtrlCmdFifoBindEngines_IMPL(pSubdevice, pBindParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetPhysicalChannelCount(pSubdevice, pParams) subdeviceCtrlCmdGetPhysicalChannelCount_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoGetInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoGetInfo(pSubdevice, pFifoInfoParams) subdeviceCtrlCmdFifoGetInfo_IMPL(pSubdevice, pFifoInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoDisableChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannels(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoDisableChannels(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannels_IMPL(pSubdevice, pDisableChannelParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoDisableUsermodeChannels(pSubdevice, pParams) subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoGetChannelMemInfo(pSubdevice, pChannelMemParams) subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL(pSubdevice, pChannelMemParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoGetUserdLocation(pSubdevice, pUserdLocationParams) subdeviceCtrlCmdFifoGetUserdLocation_IMPL(pSubdevice, pUserdLocationParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoObjschedSwGetLog(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoObjschedSwGetLog(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoObjschedGetState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetState(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoObjschedGetState(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoObjschedSetState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoObjschedSetState(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoObjschedSetState(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedSetState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS *pGetChannelGrpUidParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS *pGetChannelGrpUidParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo(pSubdevice, pGetChannelGrpUidParams) subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_IMPL(pSubdevice, pGetChannelGrpUidParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoQueryChannelUniqueId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS *pQueryChannelUidParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoQueryChannelUniqueId(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS *pQueryChannelUidParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoQueryChannelUniqueId(pSubdevice, pQueryChannelUidParams) subdeviceCtrlCmdFifoQueryChannelUniqueId_IMPL(pSubdevice, pQueryChannelUidParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb(pSubdevice, pParams) subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoClearFaultedBit(pSubdevice, pParams) subdeviceCtrlCmdFifoClearFaultedBit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoRunlistSetSchedPolicy(pSubdevice, pSchedPolicyParams) subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL(pSubdevice, pSchedPolicyParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers(pSubdevice, pParams) subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalFifoGetNumChannels(pSubdevice, pNumChannelsParams) subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL(pSubdevice, pNumChannelsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFifoGetAllocatedChannels(pSubdevice, pParams) subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS *pNumSecureChannelsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalFifoGetNumSecureChannels(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS *pNumSecureChannelsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalFifoGetNumSecureChannels(pSubdevice, pNumSecureChannelsParams) subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL(pSubdevice, pNumSecureChannelsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *pToggleActiveChannelSchedulingParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *pToggleActiveChannelSchedulingParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling(pSubdevice, pToggleActiveChannelSchedulingParams) subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL(pSubdevice, pToggleActiveChannelSchedulingParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetInfoV2(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetInfoV2(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfoV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCapsV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCapsV2(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCapsV2(pSubdevice, pGrCapsParams) subdeviceCtrlCmdKGrGetCapsV2_IMPL(pSubdevice, pGrCapsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCtxswModes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswModes_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswZcullMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswZcullBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullBind_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetZcullInfo(pSubdevice, pZcullInfoParams) subdeviceCtrlCmdKGrGetZcullInfo_IMPL(pSubdevice, pZcullInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswPmBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmBind_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswSetupBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswSetupBind(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswSetupBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswSetupBind_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrSetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrSetGpcTileMap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswSmpcMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrPcSamplingMode(pSubdevice, pParams) subdeviceCtrlCmdKGrPcSamplingMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetSmToGpcTpcMappings(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrSetCtxswPreemptionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrCtxswPreemptionBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetROPInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetROPInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetROPInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetROPInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCtxswStats(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswStats_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCtxBufferPtes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetCurrentResidentChannel(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetVatAlarmData(pSubdevice, pParams) subdeviceCtrlCmdKGrGetVatAlarmData_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetAttributeBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGfxPoolQuerySize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGfxPoolInitialize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGfxPoolAddSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGfxPoolRemoveSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetPpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetPpcMask(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetPpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPpcMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrSetTpcPartitionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifierV2(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetSmIssueRateModifierV2(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUid(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetPhysGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcMask(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetTpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetTpcMask(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetTpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetTpcMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetEngineContextProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetNumTpcsForGpc(pSubdevice, pParams) subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcTileMap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetZcullMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullMask(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetZcullMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetZcullMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetCaps(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetPpcMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetZcullInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetRopInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdKGrInternalStaticGetPdbProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable(pSubdevice, pParams) subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrStaticGetFecsTraceDefines(pSubdevice, pParams) subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetCachedInfo(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetCachedInfo_IMPL(pSubdevice, pGpuInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuForceGspUnload_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_FORCE_GSP_UNLOAD_PARAMS *pGpuInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuForceGspUnload(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_FORCE_GSP_UNLOAD_PARAMS *pGpuInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuForceGspUnload(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuForceGspUnload_IMPL(pSubdevice, pGpuInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetInfoV2(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetInfoV2(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetInfoV2_IMPL(pSubdevice, pGpuInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetIpVersion(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetIpVersion(pSubdevice, pGpuIpVersionParams) subdeviceCtrlCmdGpuGetIpVersion_IMPL(pSubdevice, pGpuIpVersionParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL(pSubdevice, pBridgeInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL(pSubdevice, pBridgeInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetOptimusInfo(pSubdevice, pGpuOptimusInfoParams) subdeviceCtrlCmdGpuSetOptimusInfo_IMPL(pSubdevice, pGpuOptimusInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNameString(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNameString(pSubdevice, pNameStringParams) subdeviceCtrlCmdGpuGetNameString_IMPL(pSubdevice, pNameStringParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetShortNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetShortNameString(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetShortNameString(pSubdevice, pShortNameStringParams) subdeviceCtrlCmdGpuGetShortNameString_IMPL(pSubdevice, pShortNameStringParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEncoderCapacity(pSubdevice, pEncoderCapacityParams) subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL(pSubdevice, pEncoderCapacityParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNvencSwSessionStats(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionStats(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL(pSubdevice, params)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL(pSubdevice, params)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdGpuSetFabricAddr_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetPower(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetPower(pSubdevice, pSetPowerParams) subdeviceCtrlCmdGpuSetPower_IMPL(pSubdevice, pSetPowerParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetSdm_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetSdm(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetSdm(pSubdevice, pSdmParams) subdeviceCtrlCmdGpuGetSdm_IMPL(pSubdevice, pSdmParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetSimulationInfo(pSubdevice, pGpuSimulationInfoParams) subdeviceCtrlCmdGpuGetSimulationInfo_IMPL(pSubdevice, pGpuSimulationInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEngines(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEngines(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngines_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEnginesV2(pSubdevice, pEngineParams) subdeviceCtrlCmdGpuGetEnginesV2_IMPL(pSubdevice, pEngineParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEngineClasslist(pSubdevice, pClassParams) subdeviceCtrlCmdGpuGetEngineClasslist_IMPL(pSubdevice, pClassParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEnginePartnerList(pSubdevice, pPartnerListParams) subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL(pSubdevice, pPartnerListParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetFermiGpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetFermiTpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetFermiZcullInfo(pSubdevice, pGpuFermiZcullInfoParams) subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL(pSubdevice, pGpuFermiZcullInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPesInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPesInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPesInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPesInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuExecRegOps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuExecRegOps(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuExecRegOps(pSubdevice, pRegParams) subdeviceCtrlCmdGpuExecRegOps_IMPL(pSubdevice, pRegParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuMigratableOps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pRegParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuMigratableOps(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pRegParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuMigratableOps(pSubdevice, pRegParams) subdeviceCtrlCmdGpuMigratableOps_IMPL(pSubdevice, pRegParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuQueryMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuQueryMode(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuQueryMode(pSubdevice, pQueryMode) subdeviceCtrlCmdGpuQueryMode_IMPL(pSubdevice, pQueryMode)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetInforomImageVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL(pSubdevice, pVersionInfo)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetInforomObjectVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL(pSubdevice, pVersionInfo)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuQueryInforomEccSupport(pSubdevice) subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetChipDetails_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetChipDetails(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetChipDetails(pSubdevice, pParams) subdeviceCtrlCmdGpuGetChipDetails_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetOEMBoardInfo(pSubdevice, pBoardInfo) subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL(pSubdevice, pBoardInfo)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetOEMInfo(pSubdevice, pOemInfo) subdeviceCtrlCmdGpuGetOEMInfo_IMPL(pSubdevice, pOemInfo)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuHandleGpuSR(pSubdevice) subdeviceCtrlCmdGpuHandleGpuSR_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetComputeModeRules(pSubdevice, pSetRulesParams) subdeviceCtrlCmdGpuSetComputeModeRules_IMPL(pSubdevice, pSetRulesParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuQueryComputeModeRules(pSubdevice, pQueryRulesParams) subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL(pSubdevice, pQueryRulesParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuAcquireComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuReleaseComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuInitializeCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuInitializeCtx(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuInitializeCtx(pSubdevice, pInitializeCtxParams) subdeviceCtrlCmdGpuInitializeCtx_IMPL(pSubdevice, pInitializeCtxParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuPromoteCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuPromoteCtx(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuPromoteCtx(pSubdevice, pPromoteCtxParams) subdeviceCtrlCmdGpuPromoteCtx_IMPL(pSubdevice, pPromoteCtxParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuEvictCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuEvictCtx(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuEvictCtx(pSubdevice, pEvictCtxParams) subdeviceCtrlCmdGpuEvictCtx_IMPL(pSubdevice, pEvictCtxParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetId(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetId(pSubdevice, pIdParams) subdeviceCtrlCmdGpuGetId_IMPL(pSubdevice, pIdParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetGidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetGidInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetGidInfo(pSubdevice, pGidInfoParams) subdeviceCtrlCmdGpuGetGidInfo_IMPL(pSubdevice, pGidInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetIllum(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuGetIllum_IMPL(pSubdevice, pConfigParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetIllum(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuSetIllum_IMPL(pSubdevice, pConfigParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetVprCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetVprCaps(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetVprCaps(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetVprInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetVprInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetVprInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPids_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPids(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPids(pSubdevice, pGetPidsParams) subdeviceCtrlCmdGpuGetPids_IMPL(pSubdevice, pGetPidsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPidInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPidInfo(pSubdevice, pGetPidInfoParams) subdeviceCtrlCmdGpuGetPidInfo_IMPL(pSubdevice, pGetPidInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuQueryFunctionStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetSkylineInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SKYLINE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetSkylineInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SKYLINE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetSkylineInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetSkylineInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuReportNonReplayableFault(pSubdevice, pParams) subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEngineFaultInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEngineRunlistPriBase(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetHwEngineId(pSubdevice, pParams) subdeviceCtrlCmdGpuGetHwEngineId_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetFirstAsyncCEIdx(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetFirstAsyncCEIdx(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetVmmuSegmentSize(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetVmmuSegmentSize(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetMaxSupportedPageSize(pSubdevice, pParams) subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuHandleVfPriFault_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuHandleVfPriFault(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuHandleVfPriFault(pSubdevice, pParams) subdeviceCtrlCmdGpuHandleVfPriFault_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdValidateMemMapRequest(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdValidateMemMapRequest(pSubdevice, pParams) subdeviceCtrlCmdValidateMemMapRequest_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetGfid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GFID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetGfid(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GFID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetGfid(pSubdevice, pParams) subdeviceCtrlCmdGpuGetGfid_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdUpdateGfidP2pCapability(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdUpdateGfidP2pCapability(pSubdevice, pParams) subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetEngineLoadTimes(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetP2pCaps(struct Subdevice *pSubdevice, NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdGetP2pCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetGpuFabricProbeInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetGpuFabricProbeInfo(pSubdevice, pParams) subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForReset_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForReset(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuMarkDeviceForReset(pSubdevice) subdeviceCtrlCmdGpuMarkDeviceForReset_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForReset_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForReset(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuUnmarkDeviceForReset(pSubdevice) subdeviceCtrlCmdGpuUnmarkDeviceForReset_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset(pSubdevice) subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset(pSubdevice) subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetResetStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetResetStatus(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetResetStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuGetResetStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetDrainAndResetStatus(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetDrainAndResetStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetConstructedFalconInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetConstructedFalconInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlGpuGetFipsStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlGpuGetFipsStatus(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlGpuGetFipsStatus(pSubdevice, pParams) subdeviceCtrlGpuGetFipsStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetVfCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetVfCaps(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetVfCaps(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVfCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetRecoveryAction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetRecoveryAction(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetRecoveryAction(pSubdevice, pParams) subdeviceCtrlCmdGpuGetRecoveryAction_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuRpcGspTest_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuRpcGspTest(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuRpcGspTest(pSubdevice, pParams) subdeviceCtrlCmdGpuRpcGspTest_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuRpcGspQuerySizes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuRpcGspQuerySizes(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuRpcGspQuerySizes(pSubdevice, pParams) subdeviceCtrlCmdGpuRpcGspQuerySizes_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRusdGetSupportedFeatures(struct Subdevice *pSubdevice, NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRusdGetSupportedFeatures(pSubdevice, pParams) subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetTrigger_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetTrigger(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetTrigger(pSubdevice) subdeviceCtrlCmdEventSetTrigger_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetTriggerFifo(pSubdevice, pTriggerFifoParams) subdeviceCtrlCmdEventSetTriggerFifo_IMPL(pSubdevice, pTriggerFifoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetNotification_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetNotification(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetNotification(pSubdevice, pSetEventParams) subdeviceCtrlCmdEventSetNotification_IMPL(pSubdevice, pSetEventParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetMemoryNotifies(pSubdevice, pSetMemoryNotifiesParams) subdeviceCtrlCmdEventSetMemoryNotifies_IMPL(pSubdevice, pSetMemoryNotifiesParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetSemaphoreMemory(pSubdevice, pSetSemMemoryParams) subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL(pSubdevice, pSetSemMemoryParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventSetSemaMemValidation(pSubdevice, pSetSemaMemValidationParams) subdeviceCtrlCmdEventSetSemaMemValidation_IMPL(pSubdevice, pSetSemaMemValidationParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pBindParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pBindParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventVideoBindEvtbuf(pSubdevice, pBindParams) subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL(pSubdevice, pBindParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *pBindParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *pBindParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf(pSubdevice, pBindParams) subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL(pSubdevice, pBindParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerCancel_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerCancel(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerCancel(pSubdevice) subdeviceCtrlCmdTimerCancel_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerSchedule_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerSchedule(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerSchedule(pSubdevice, pParams) subdeviceCtrlCmdTimerSchedule_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerGetTime_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerGetTime(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerGetTime(pSubdevice, pParams) subdeviceCtrlCmdTimerGetTime_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerGetRegisterOffset(pSubdevice, pTimerRegOffsetParams) subdeviceCtrlCmdTimerGetRegisterOffset_IMPL(pSubdevice, pTimerRegOffsetParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo(pSubdevice, pParams) subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdTimerSetGrTickFreq(pSubdevice, pParams) subdeviceCtrlCmdTimerSetGrTickFreq_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcReadVirtualMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcReadVirtualMem(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcReadVirtualMem(pSubdevice, pReadVirtMemParam) subdeviceCtrlCmdRcReadVirtualMem_IMPL(pSubdevice, pReadVirtMemParam)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcGetErrorCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcGetErrorCount(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcGetErrorCount(pSubdevice, pErrorCount) subdeviceCtrlCmdRcGetErrorCount_IMPL(pSubdevice, pErrorCount)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcGetErrorV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcGetErrorV2(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcGetErrorV2(pSubdevice, pErrorParams) subdeviceCtrlCmdRcGetErrorV2_IMPL(pSubdevice, pErrorParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcSetCleanErrorHistory(pSubdevice) subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcGetWatchdogInfo(pSubdevice, pWatchdogInfoParams) subdeviceCtrlCmdRcGetWatchdogInfo_IMPL(pSubdevice, pWatchdogInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcDisableWatchdog_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcDisableWatchdog(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcDisableWatchdog_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcSoftDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcEnableWatchdog_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcEnableWatchdog(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcEnableWatchdog(pSubdevice) subdeviceCtrlCmdRcEnableWatchdog_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdRcReleaseWatchdogRequests(pSubdevice) subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalRcWatchdogTimeout(pSubdevice) subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetRcInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdGetRcInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdSetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdSetRcInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdSetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdSetRcInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvdGetDumpSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvdGetDumpSize(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvdGetDumpSize(pSubdevice, pDumpSizeParams) subdeviceCtrlCmdNvdGetDumpSize_IMPL(pSubdevice, pDumpSizeParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvdGetDump_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvdGetDump(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvdGetDump(pSubdevice, pDumpParams) subdeviceCtrlCmdNvdGetDump_IMPL(pSubdevice, pDumpParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvdGetNocatJournalRpt(pSubdevice, pReportParams) subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL(pSubdevice, pReportParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvdSetNocatJournalData(pSubdevice, pReportParams) subdeviceCtrlCmdNvdSetNocatJournalData_IMPL(pSubdevice, pReportParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS *pReportParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdNvdInsertNocatJournalRecord(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS *pReportParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdNvdInsertNocatJournalRecord(pSubdevice, pReportParams) subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL(pSubdevice, pReportParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdPmgrGetModuleInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PMGR_MODULE_INFO_PARAMS *pModuleInfoParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPmgrGetModuleInfo(struct Subdevice *pSubdevice, NV2080_CTRL_PMGR_MODULE_INFO_PARAMS *pModuleInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPmgrGetModuleInfo(pSubdevice, pModuleInfoParams) subdeviceCtrlCmdPmgrGetModuleInfo_IMPL(pSubdevice, pModuleInfoParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGc6Entry_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGc6Entry(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGc6Entry(pSubdevice, pParams) subdeviceCtrlCmdGc6Entry_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGc6Exit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_EXIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGc6Exit(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_EXIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGc6Exit(pSubdevice, pParams) subdeviceCtrlCmdGc6Exit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdLpwrDifrPrefetchResponse(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdLpwrDifrPrefetchResponse(pSubdevice, pParams) subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdLpwrDifrCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdLpwrDifrCtrl(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdLpwrDifrCtrl(pSubdevice, pParams) subdeviceCtrlCmdLpwrDifrCtrl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetCePceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetCePceMask(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetCePceMask(pSubdevice, pCePceMaskParams) subdeviceCtrlCmdCeGetCePceMask_IMPL(pSubdevice, pCePceMaskParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *pCeUpdatePceLceMappingsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeUpdatePceLceMappings(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappings_KERNEL(pSubdevice, pCeUpdatePceLceMappingsParams)
#endif // __nvoc_subdevice_h_disabled

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappingsV2(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS *pCeUpdatePceLceMappingsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeUpdatePceLceMappingsV2(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappingsV2_KERNEL(pSubdevice, pCeUpdatePceLceMappingsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetLceShimInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetLceShimInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetLceShimInfo(pSubdevice, pParams) subdeviceCtrlCmdCeGetLceShimInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceType(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetPceConfigForLceType(pSubdevice, pParams) subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetDecompLceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetDecompLceMask(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetDecompLceMask(pSubdevice, pParams) subdeviceCtrlCmdCeGetDecompLceMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeIsDecompLceEnabled_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeIsDecompLceEnabled(struct Subdevice *pSubdevice, NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeIsDecompLceEnabled(pSubdevice, pParams) subdeviceCtrlCmdCeIsDecompLceEnabled_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnGetDmemUsage(pSubdevice, pFlcnDmemUsageParams) subdeviceCtrlCmdFlcnGetDmemUsage_IMPL(pSubdevice, pFlcnDmemUsageParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnGetEngineArch(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetEngineArch_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnUstreamerQueueInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnUstreamerControlGet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnUstreamerControlSet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlcnGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEccGetClientExposedCounters(pSubdevice, pParams) subdeviceCtrlCmdEccGetClientExposedCounters_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEccGetVolatileCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEccGetVolatileCounts(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEccGetVolatileCounts(pSubdevice, pParams) subdeviceCtrlCmdEccGetVolatileCounts_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEccInjectError_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_INJECT_ERROR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEccInjectError(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_INJECT_ERROR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEccInjectError(pSubdevice, pParams) subdeviceCtrlCmdEccInjectError_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEccGetRepairStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEccGetRepairStatus(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEccGetRepairStatus(pSubdevice, pParams) subdeviceCtrlCmdEccGetRepairStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdEccGetSramUniqueUncorrCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdEccGetSramUniqueUncorrCounts(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdEccGetSramUniqueUncorrCounts(pSubdevice, pParams) subdeviceCtrlCmdEccGetSramUniqueUncorrCounts_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlaRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlaRange(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlaRange(pSubdevice, pParams) subdeviceCtrlCmdFlaRange_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlaSetupInstanceMemBlock(pSubdevice, pParams) subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlaGetRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlaGetRange(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlaGetRange(pSubdevice, pParams) subdeviceCtrlCmdFlaGetRange_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdFlaGetFabricMemStats(pSubdevice, pParams) subdeviceCtrlCmdFlaGetFabricMemStats_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGspGetRmHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS *pGspRmHeapStatsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGspGetRmHeapStats(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS *pGspRmHeapStatsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGspGetRmHeapStats(pSubdevice, pGspRmHeapStatsParams) subdeviceCtrlCmdGspGetRmHeapStats_IMPL(pSubdevice, pGspRmHeapStatsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetVgpuHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetVgpuHeapStats(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetVgpuHeapStats(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVgpuHeapStats_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdLibosGetHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *pGspLibosHeapStatsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdLibosGetHeapStats(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *pGspLibosHeapStatsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdLibosGetHeapStats(pSubdevice, pGspLibosHeapStatsParams) subdeviceCtrlCmdLibosGetHeapStats_IMPL(pSubdevice, pGspLibosHeapStatsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetActivePartitionIds(pSubdevice, pParams) subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPartitionCapacity(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuDescribePartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuDescribePartitions(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuDescribePartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuDescribePartitions_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetPartitioningMode(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitioningMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGrmgrGetGrFsInfo(pSubdevice, pParams) subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuSetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitions(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuSetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitions_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitions(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitions_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetComputeProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetComputeProfiles(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetComputeProfiles(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputeProfiles_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGpuGetComputeProfileCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGpuGetComputeProfileCapacity(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGpuGetComputeProfileCapacity(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputeProfileCapacity_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdOsUnixGc6BlockerRefCnt(pSubdevice, pParams) subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdOsUnixAllowDisallowGcoff(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdOsUnixAudioDynamicPower(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdOsUnixVidmemPersistenceStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdOsUnixUpdateTgpStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayGetIpVersion(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetIpVersion_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetStaticInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplaySetChannelPushbuffer(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayWriteInstMem(pSubdevice, pParams) subdeviceCtrlCmdDisplayWriteInstMem_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplaySetupRgLineIntr(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplaySetImportedImpData(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetImportedImpData_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayGetDisplayMask(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetDisplayMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayPinsetsToLockpins(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayPinsetsToLockpins(pSubdevice, pParams) subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpioProgramDirection_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpioProgramDirection(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpioProgramDirection(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioProgramDirection_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpioProgramOutput_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpioProgramOutput(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpioProgramOutput(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioProgramOutput_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpioReadInput_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpioReadInput(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpioReadInput(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioReadInput_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpioActivateHwFunction(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpioActivateHwFunction(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated(pSubdevice) subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalDisplayPreModeSet(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalDisplayPreModeSet(pSubdevice) subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalDisplayPostModeSet(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalDisplayPostModeSet(pSubdevice) subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayPreUnixConsole_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayPreUnixConsole(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayPreUnixConsole(pSubdevice, pParams) subdeviceCtrlCmdDisplayPreUnixConsole_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdDisplayPostUnixConsole_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdDisplayPostUnixConsole(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdDisplayPostUnixConsole(pSubdevice, pParams) subdeviceCtrlCmdDisplayPostUnixConsole_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGetChipInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGetChipInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGetChipInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalGetChipInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGetUserRegisterAccessMap(pSubdevice, pParams) subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalRecoverAllComputeContexts(pSubdevice) subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGetSmcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGetSmcMode(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGetSmcMode(pSubdevice, pParams) subdeviceCtrlCmdInternalGetSmcMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalCheckCtsIdValid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CHECK_CTS_ID_VALID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalCheckCtsIdValid(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CHECK_CTS_ID_VALID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalCheckCtsIdValid(pSubdevice, pParams) subdeviceCtrlCmdInternalCheckCtsIdValid_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdIsEgpuBridge_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdIsEgpuBridge(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdIsEgpuBridge(pSubdevice, pParams) subdeviceCtrlCmdIsEgpuBridge_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap(pSubdevice, pParams) subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusFlushWithSysmembar(pSubdevice) subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusDestroyP2pMailbox(pSubdevice, pParams) subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusCreateC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpuGetPFBar1Spa_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS *pConfigParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpuGetPFBar1Spa(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS *pConfigParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpuGetPFBar1Spa(pSubdevice, pConfigParams) subdeviceCtrlCmdInternalGpuGetPFBar1Spa_IMPL(pSubdevice, pConfigParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries(pSubdevice, pParams) subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGmmuGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdGmmuGetStaticInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer(pSubdevice) subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS *pCopyServerReservedPdesParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS *pCopyServerReservedPdesParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer(pSubdevice, pCopyServerReservedPdesParams) subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL(pSubdevice, pCopyServerReservedPdesParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetPhysicalCaps_IMPL(pSubdevice, pCeCapsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetAllPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL(pSubdevice, pCeCapsParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeUpdateClassDB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeUpdateClassDB(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeUpdateClassDB(pSubdevice, params) subdeviceCtrlCmdCeUpdateClassDB_IMPL(pSubdevice, params)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetFaultMethodBufferSize(pSubdevice, params) subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL(pSubdevice, params)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetHubPceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetHubPceMask(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetHubPceMask(pSubdevice, pParams) subdeviceCtrlCmdCeGetHubPceMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCeGetHubPceMaskV2(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCeGetHubPceMaskV2(pSubdevice, pParams) subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdIntrGetKernelTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdIntrGetKernelTable(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdIntrGetKernelTable(pSubdevice, pParams) subdeviceCtrlCmdIntrGetKernelTable_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfBoostSet_2x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfBoostSet_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfBoostClear_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfGetAuxPowerState(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfGetAuxPowerState(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS *pGcxEntryPrerequisite);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGcxEntryPrerequisite(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS *pGcxEntryPrerequisite) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGcxEntryPrerequisite(pSubdevice, pGcxEntryPrerequisite) subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL(pSubdevice, pGcxEntryPrerequisite)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBifGetAspmL1Flags(pSubdevice, pParams) subdeviceCtrlCmdBifGetAspmL1Flags_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBifSetPcieRo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBifSetPcieRo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBifSetPcieRo(pSubdevice, pParams) subdeviceCtrlCmdBifSetPcieRo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdBifDisableSystemMemoryAccess(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdBifDisableSystemMemoryAccess(pSubdevice, pParams) subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubPeerConnConfig(pSubdevice, pParams) subdeviceCtrlCmdHshubPeerConnConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubGetHshubIdForLinks(pSubdevice, pParams) subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubGetNumUnits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubGetNumUnits(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubGetNumUnits(pSubdevice, pParams) subdeviceCtrlCmdHshubGetNumUnits_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubNextHshubId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubNextHshubId(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubNextHshubId(pSubdevice, pParams) subdeviceCtrlCmdHshubNextHshubId_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubGetMaxHshubsPerShim(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubGetMaxHshubsPerShim(pSubdevice, pParams) subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdHshubEgmConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdHshubEgmConfig(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdHshubEgmConfig(pSubdevice, pParams) subdeviceCtrlCmdHshubEgmConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr(pSubdevice) subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkCoreCallback_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkCoreCallback(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkCoreCallback(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkCoreCallback_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetAliEnabled_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetAliEnabled(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetAliEnabled(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetAliEnabled_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramBufferready_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramBufferready(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkProgramBufferready(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProgramBufferready_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors(pSubdevice) subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateHshubMux(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkUpdateHshubMux(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkLinkTrainAli_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkLinkTrainAli(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkLinkTrainAli(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkLinkTrainAli_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkAreLinksTrained_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkAreLinksTrained(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkAreLinksTrained(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkAreLinksTrained_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkResetLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkResetLinks(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkResetLinks(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkResetLinks_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalNvlinkRCUserModeChannels_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkRCUserModeChannels(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalNvlinkRCUserModeChannels(pSubdevice) subdeviceCtrlCmdInternalNvlinkRCUserModeChannels_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalSetP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalSetP2pCaps(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalSetP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalSetP2pCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalRemoveP2pCaps(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalRemoveP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGetPcieP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalInitGpuIntr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalInitGpuIntr(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalInitGpuIntr(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalInitGpuIntr_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncOptimizeTiming(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncOptimizeTiming(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetDisplayIds(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncGetDisplayIds(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncSetStereoSync(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncSetStereoSync(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetVactiveLines(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncGetVactiveLines(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncIsDisplayIdValid(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncIsDisplayIdValid(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS *pAttachParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS *pAttachParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_IMPL(pSubdevice, pAttachParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode(pSubdevice, pParams) subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalFbsrInit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalFbsrInit(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalFbsrInit(pSubdevice, pParams) subdeviceCtrlCmdInternalFbsrInit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPostInitBrightcStateLoad(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPostInitBrightcStateLoad(pSubdevice, pParams) subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalSetStaticEdidData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalSetStaticEdidData(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalSetStaticEdidData(pSubdevice, pParams) subdeviceCtrlCmdInternalSetStaticEdidData_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate(pSubdevice, pParams) subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync(pSubdevice, pParams) subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit(pSubdevice, pParams) subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck(pSubdevice) subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit(pSubdevice) subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL(struct Subdevice *pSubdevice);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalDetectHsVideoBridge(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalDetectHsVideoBridge(pSubdevice) subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL(pSubdevice)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeGetStaticInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveLceKeys(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeDeriveLceKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeRotateKeys(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeRotateKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeSetGpuState(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeSetGpuState(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalInitUserSharedData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalInitUserSharedData(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalInitUserSharedData(pSubdevice, pParams) subdeviceCtrlCmdInternalInitUserSharedData_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalUserSharedDataSetDataPoll(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalUserSharedDataSetDataPoll(pSubdevice, pParams) subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalControlGspTrace_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalControlGspTrace(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalControlGspTrace(pSubdevice, pParams) subdeviceCtrlCmdInternalControlGspTrace_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter(pSubdevice, pParams) subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpuSetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS *pConfigParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpuSetIllum(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS *pConfigParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpuSetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdInternalGpuSetIllum_IMPL(pSubdevice, pConfigParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalLogOobXid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalLogOobXid(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalLogOobXid(pSubdevice, pParams) subdeviceCtrlCmdInternalLogOobXid_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdInternalGpuGetHfrpInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_HFRP_INFO_PARAMS *pHfrpParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdInternalGpuGetHfrpInfo(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_HFRP_INFO_PARAMS *pHfrpParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdInternalGpuGetHfrpInfo(pSubdevice, pHfrpParams) subdeviceCtrlCmdInternalGpuGetHfrpInfo_IMPL(pSubdevice, pHfrpParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeStates(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalFreeStates(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdGetAvailableHshubMask(pSubdevice, pParams) subdeviceCtrlCmdGetAvailableHshubMask_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlSetEcThrottleMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlSetEcThrottleMode(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlSetEcThrottleMode(pSubdevice, pParams) subdeviceCtrlSetEcThrottleMode_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCcuMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCcuMap(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCcuMap(pSubdevice, pParams) subdeviceCtrlCmdCcuMap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCcuUnmap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCcuUnmap(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCcuUnmap(pSubdevice, pParams) subdeviceCtrlCmdCcuUnmap_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdCcuSetStreamState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *pParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdCcuSetStreamState(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdCcuSetStreamState(pSubdevice, pParams) subdeviceCtrlCmdCcuSetStreamState_IMPL(pSubdevice, pParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceCtrlCmdSpdmPartition_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdSpdmPartition(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdSpdmPartition(pSubdevice, pSpdmPartitionParams) subdeviceCtrlCmdSpdmPartition_IMPL(pSubdevice, pSpdmPartitionParams)
#endif // __nvoc_subdevice_h_disabled

NV_STATUS subdeviceSpdmRetrieveTranscript_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *pSpdmRetrieveSessionTranscriptParams);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceSpdmRetrieveTranscript(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *pSpdmRetrieveSessionTranscriptParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_subdevice_h_disabled
#define subdeviceSpdmRetrieveTranscript(pSubdevice, pSpdmRetrieveSessionTranscriptParams) subdeviceSpdmRetrieveTranscript_IMPL(pSubdevice, pSpdmRetrieveSessionTranscriptParams)
#endif // __nvoc_subdevice_h_disabled


// Wrapper macros for halified functions
#define subdevicePreDestruct_FNPTR(pResource) pResource->__nvoc_metadata_ptr->vtable.__subdevicePreDestruct__
#define subdevicePreDestruct(pResource) subdevicePreDestruct_DISPATCH(pResource)
#define subdeviceInternalControlForward_FNPTR(pSubdevice) pSubdevice->__nvoc_metadata_ptr->vtable.__subdeviceInternalControlForward__
#define subdeviceInternalControlForward(pSubdevice, command, pParams, size) subdeviceInternalControlForward_DISPATCH(pSubdevice, command, pParams, size)
#define subdeviceCtrlCmdBiosGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetInfoV2__
#define subdeviceCtrlCmdBiosGetInfoV2(pSubdevice, pBiosInfoParams) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams)
#define subdeviceCtrlCmdBiosGetInfoV2_HAL(pSubdevice, pBiosInfoParams) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams)
#define subdeviceCtrlCmdBiosGetSKUInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetSKUInfo__
#define subdeviceCtrlCmdBiosGetSKUInfo(pSubdevice, pBiosGetSKUInfoParams) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams)
#define subdeviceCtrlCmdBiosGetSKUInfo_HAL(pSubdevice, pBiosGetSKUInfoParams) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams)
#define subdeviceCtrlCmdBiosGetPostTime_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetPostTime__
#define subdeviceCtrlCmdBiosGetPostTime(pSubdevice, pBiosPostTime) subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime)
#define subdeviceCtrlCmdBiosGetPostTime_HAL(pSubdevice, pBiosPostTime) subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime)
#define subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__
#define subdeviceCtrlCmdBusGetPcieReqAtomicsCaps(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__
#define subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__
#define subdeviceCtrlCmdBusGetPcieCplAtomicsCaps(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetC2CInfo__
#define subdeviceCtrlCmdBusGetC2CInfo(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CInfo_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CLpwrStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetC2CLpwrStats__
#define subdeviceCtrlCmdBusGetC2CLpwrStats(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CLpwrStats_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote(pSubdevice, pParams) subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetP2pMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetP2pMapping__
#define subdeviceCtrlCmdBusSetP2pMapping(pSubdevice, pParams) subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetP2pMapping_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusUnsetP2pMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusUnsetP2pMapping__
#define subdeviceCtrlCmdBusUnsetP2pMapping(pSubdevice, pParams) subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusUnsetP2pMapping_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetNvlinkCaps__
#define subdeviceCtrlCmdBusGetNvlinkCaps(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_HAL(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfRatedTdpSetControl_HAL(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfReservePerfmonHw_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfReservePerfmonHw__
#define subdeviceCtrlCmdPerfReservePerfmonHw(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams)
#define subdeviceCtrlCmdPerfReservePerfmonHw_HAL(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams)
#define subdeviceCtrlCmdPerfSetAuxPowerState_HAL(pSubdevice, pPowerStateParams) subdeviceCtrlCmdPerfSetAuxPowerState(pSubdevice, pPowerStateParams)
#define subdeviceCtrlCmdPerfSetPowerstate_HAL(pSubdevice, pPowerInfoParams) subdeviceCtrlCmdPerfSetPowerstate(pSubdevice, pPowerInfoParams)
#define subdeviceCtrlCmdPerfGetLevelInfo_V2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetLevelInfo_V2__
#define subdeviceCtrlCmdPerfGetLevelInfo_V2(pSubdevice, pLevelInfoParams) subdeviceCtrlCmdPerfGetLevelInfo_V2_DISPATCH(pSubdevice, pLevelInfoParams)
#define subdeviceCtrlCmdPerfGetLevelInfo_V2_HAL(pSubdevice, pLevelInfoParams) subdeviceCtrlCmdPerfGetLevelInfo_V2_DISPATCH(pSubdevice, pLevelInfoParams)
#define subdeviceCtrlCmdPerfGetCurrentPstate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetCurrentPstate__
#define subdeviceCtrlCmdPerfGetCurrentPstate(pSubdevice, pInfoParams) subdeviceCtrlCmdPerfGetCurrentPstate_DISPATCH(pSubdevice, pInfoParams)
#define subdeviceCtrlCmdPerfGetCurrentPstate_HAL(pSubdevice, pInfoParams) subdeviceCtrlCmdPerfGetCurrentPstate_DISPATCH(pSubdevice, pInfoParams)
#define subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__
#define subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample(pSubdevice, pSampleParams) subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_DISPATCH(pSubdevice, pSampleParams)
#define subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_HAL(pSubdevice, pSampleParams) subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_DISPATCH(pSubdevice, pSampleParams)
#define subdeviceCtrlCmdPerfGetPowerstate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetPowerstate__
#define subdeviceCtrlCmdPerfGetPowerstate(pSubdevice, powerInfoParams) subdeviceCtrlCmdPerfGetPowerstate_DISPATCH(pSubdevice, powerInfoParams)
#define subdeviceCtrlCmdPerfGetPowerstate_HAL(pSubdevice, powerInfoParams) subdeviceCtrlCmdPerfGetPowerstate_DISPATCH(pSubdevice, powerInfoParams)
#define subdeviceCtrlCmdPerfNotifyVideoevent_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfNotifyVideoevent__
#define subdeviceCtrlCmdPerfNotifyVideoevent(pSubdevice, pVideoEventParams) subdeviceCtrlCmdPerfNotifyVideoevent_DISPATCH(pSubdevice, pVideoEventParams)
#define subdeviceCtrlCmdPerfNotifyVideoevent_HAL(pSubdevice, pVideoEventParams) subdeviceCtrlCmdPerfNotifyVideoevent_DISPATCH(pSubdevice, pVideoEventParams)
#define subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetOfflinedPages_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetOfflinedPages__
#define subdeviceCtrlCmdFbGetOfflinedPages(pSubdevice, pBlackListParams) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams)
#define subdeviceCtrlCmdFbGetOfflinedPages_HAL(pSubdevice, pBlackListParams) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams)
#define subdeviceCtrlCmdFbGetLTCInfoForFBP_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetLTCInfoForFBP__
#define subdeviceCtrlCmdFbGetLTCInfoForFBP(pSubdevice, pParams) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetLTCInfoForFBP_HAL(pSubdevice, pParams) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages(pSubdevice, pParams) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages_HAL(pSubdevice, pParams) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetStaticConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysGetStaticConfig__
#define subdeviceCtrlCmdMemSysGetStaticConfig(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetStaticConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_HAL(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_HAL(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_HAL(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoObjschedGetCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoObjschedGetCaps__
#define subdeviceCtrlCmdFifoObjschedGetCaps(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoObjschedGetCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoConfigCtxswTimeout_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoConfigCtxswTimeout__
#define subdeviceCtrlCmdFifoConfigCtxswTimeout(pSubdevice, pParams) subdeviceCtrlCmdFifoConfigCtxswTimeout_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoConfigCtxswTimeout_HAL(pSubdevice, pParams) subdeviceCtrlCmdFifoConfigCtxswTimeout_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetDeviceInfoTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetDeviceInfoTable__
#define subdeviceCtrlCmdFifoGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetDeviceInfoTable_HAL(pSubdevice, pParams) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoUpdateChannelInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoUpdateChannelInfo__
#define subdeviceCtrlCmdFifoUpdateChannelInfo(pSubdevice, pChannelInfo) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo)
#define subdeviceCtrlCmdFifoUpdateChannelInfo_HAL(pSubdevice, pChannelInfo) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo)
#define subdeviceCtrlCmdKGrCtxswPmMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswPmMode__
#define subdeviceCtrlCmdKGrCtxswPmMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPmMode_HAL(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryEccStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryEccStatus__
#define subdeviceCtrlCmdGpuQueryEccStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryEccStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryIllumSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryIllumSupport__
#define subdeviceCtrlCmdGpuQueryIllumSupport(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuQueryIllumSupport_HAL(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuQueryScrubberStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryScrubberStatus__
#define subdeviceCtrlCmdGpuQueryScrubberStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryScrubberStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSetRcRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetRcRecovery__
#define subdeviceCtrlCmdSetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdSetRcRecovery_HAL(pSubdevice, pRcRecovery) subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetRcRecovery__
#define subdeviceCtrlCmdGetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcRecovery_HAL(pSubdevice, pRcRecovery) subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdCeGetCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetCaps__
#define subdeviceCtrlCmdCeGetCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCaps_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappings_HAL(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappings(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappingsV2_HAL(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappingsV2(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeGetCapsV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetCapsV2__
#define subdeviceCtrlCmdCeGetCapsV2(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCapsV2_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetAllCaps__
#define subdeviceCtrlCmdCeGetAllCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllCaps_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdGpuQueryEccConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryEccConfiguration__
#define subdeviceCtrlCmdGpuQueryEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuQueryEccConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuSetEccConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetEccConfiguration__
#define subdeviceCtrlCmdGpuSetEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuSetEccConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuResetEccErrorStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuResetEccErrorStatus__
#define subdeviceCtrlCmdGpuResetEccErrorStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuResetEccErrorStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGspGetFeatures_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGspGetFeatures__
#define subdeviceCtrlCmdGspGetFeatures(pSubdevice, pGspFeaturesParams) subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams)
#define subdeviceCtrlCmdGspGetFeatures_HAL(pSubdevice, pGspFeaturesParams) subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams)
#define subdeviceCtrlCmdBifGetStaticInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBifGetStaticInfo__
#define subdeviceCtrlCmdBifGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifGetStaticInfo_HAL(pSubdevice, pParams) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetLocalAtsConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetLocalAtsConfig__
#define subdeviceCtrlCmdInternalGetLocalAtsConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetLocalAtsConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetPeerAtsConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalSetPeerAtsConfig__
#define subdeviceCtrlCmdInternalSetPeerAtsConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetPeerAtsConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuGetSampleInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCcuGetSampleInfo__
#define subdeviceCtrlCmdCcuGetSampleInfo(pSubdevice, pParams) subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuGetSampleInfo_HAL(pSubdevice, pParams) subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define subdeviceControl(pGpuResource, pCallContext, pParams) subdeviceControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define subdeviceMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define subdeviceMap(pGpuResource, pCallContext, pParams, pCpuMapping) subdeviceMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define subdeviceUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define subdeviceUnmap(pGpuResource, pCallContext, pCpuMapping) subdeviceUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define subdeviceShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define subdeviceShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) subdeviceShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define subdeviceGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define subdeviceGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) subdeviceGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define subdeviceGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define subdeviceGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) subdeviceGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define subdeviceGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define subdeviceGetInternalObjectHandle(pGpuResource) subdeviceGetInternalObjectHandle_DISPATCH(pGpuResource)
#define subdeviceAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define subdeviceAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) subdeviceAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define subdeviceGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define subdeviceGetMemInterMapParams(pRmResource, pParams) subdeviceGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define subdeviceCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define subdeviceCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) subdeviceCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define subdeviceGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define subdeviceGetMemoryMappingDescriptor(pRmResource, ppMemDesc) subdeviceGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define subdeviceControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define subdeviceControlSerialization_Prologue(pResource, pCallContext, pParams) subdeviceControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define subdeviceControlSerialization_Epilogue(pResource, pCallContext, pParams) subdeviceControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define subdeviceControl_Prologue(pResource, pCallContext, pParams) subdeviceControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define subdeviceControl_Epilogue(pResource, pCallContext, pParams) subdeviceControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define subdeviceCanCopy(pResource) subdeviceCanCopy_DISPATCH(pResource)
#define subdeviceIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define subdeviceIsDuplicate(pResource, hMemory, pDuplicate) subdeviceIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define subdeviceControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define subdeviceControlFilter(pResource, pCallContext, pParams) subdeviceControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define subdeviceIsPartialUnmapSupported(pResource) subdeviceIsPartialUnmapSupported_DISPATCH(pResource)
#define subdeviceMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define subdeviceMapTo(pResource, pParams) subdeviceMapTo_DISPATCH(pResource, pParams)
#define subdeviceUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define subdeviceUnmapFrom(pResource, pParams) subdeviceUnmapFrom_DISPATCH(pResource, pParams)
#define subdeviceGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define subdeviceGetRefCount(pResource) subdeviceGetRefCount_DISPATCH(pResource)
#define subdeviceAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define subdeviceAddAdditionalDependants(pClient, pResource, pReference) subdeviceAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define subdeviceGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define subdeviceGetNotificationListPtr(pNotifier) subdeviceGetNotificationListPtr_DISPATCH(pNotifier)
#define subdeviceGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define subdeviceGetNotificationShare(pNotifier) subdeviceGetNotificationShare_DISPATCH(pNotifier)
#define subdeviceSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define subdeviceSetNotificationShare(pNotifier, pNotifShare) subdeviceSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define subdeviceUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define subdeviceUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) subdeviceUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define subdeviceGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define subdeviceGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) subdeviceGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline void subdevicePreDestruct_DISPATCH(struct Subdevice *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__subdevicePreDestruct__(pResource);
}

static inline NV_STATUS subdeviceInternalControlForward_DISPATCH(struct Subdevice *pSubdevice, NvU32 command, void *pParams, NvU32 size) {
    return pSubdevice->__nvoc_metadata_ptr->vtable.__subdeviceInternalControlForward__(pSubdevice, command, pParams, size);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetInfoV2__(pSubdevice, pBiosInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetSKUInfo__(pSubdevice, pBiosGetSKUInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetPostTime_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetPostTime__(pSubdevice, pBiosPostTime);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetC2CInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetC2CLpwrStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetP2pMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusUnsetP2pMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfReservePerfmonHw__(pSubdevice, pPerfmonParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetLevelInfo_V2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *pLevelInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetLevelInfo_V2__(pSubdevice, pLevelInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetCurrentPstate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *pInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetCurrentPstate__(pSubdevice, pInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *pSampleParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__(pSubdevice, pSampleParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetPowerstate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *powerInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetPowerstate__(pSubdevice, powerInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfNotifyVideoevent_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS *pVideoEventParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfNotifyVideoevent__(pSubdevice, pVideoEventParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetOfflinedPages__(pSubdevice, pBlackListParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetLTCInfoForFBP__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetStaticConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__(pSubdevice, pDisableChannelParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__(pSubdevice, pDisableChannelParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoObjschedGetCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoConfigCtxswTimeout_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_CONFIG_CTXSW_TIMEOUT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoConfigCtxswTimeout__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetDeviceInfoTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo) {
    return pSubdevice->__subdeviceCtrlCmdFifoUpdateChannelInfo__(pSubdevice, pChannelInfo);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPmMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryEccStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryIllumSupport__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryScrubberStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdSetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdSetRcRecovery__(pSubdevice, pRcRecovery);
}

static inline NV_STATUS subdeviceCtrlCmdGetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdGetRcRecovery__(pSubdevice, pRcRecovery);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetCapsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCapsV2__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetAllCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetAllCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryEccConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetEccConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuResetEccErrorStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGspGetFeatures_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams) {
    return pSubdevice->__subdeviceCtrlCmdGspGetFeatures__(pSubdevice, pGspFeaturesParams);
}

static inline NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifGetStaticInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetLocalAtsConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalSetPeerAtsConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCcuGetSampleInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceControl_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS subdeviceMap_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS subdeviceUnmap_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool subdeviceShareCallback_DISPATCH(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS subdeviceGetRegBaseOffsetAndSize_DISPATCH(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS subdeviceGetMapAddrSpace_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle subdeviceGetInternalObjectHandle_DISPATCH(struct Subdevice *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__subdeviceGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool subdeviceAccessCallback_DISPATCH(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS subdeviceGetMemInterMapParams_DISPATCH(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__subdeviceGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS subdeviceCheckMemInterUnmap_DISPATCH(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__subdeviceCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS subdeviceGetMemoryMappingDescriptor_DISPATCH(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__subdeviceGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS subdeviceControlSerialization_Prologue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void subdeviceControlSerialization_Epilogue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__subdeviceControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS subdeviceControl_Prologue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void subdeviceControl_Epilogue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__subdeviceControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool subdeviceCanCopy_DISPATCH(struct Subdevice *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceCanCopy__(pResource);
}

static inline NV_STATUS subdeviceIsDuplicate_DISPATCH(struct Subdevice *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline NV_STATUS subdeviceControlFilter_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool subdeviceIsPartialUnmapSupported_DISPATCH(struct Subdevice *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS subdeviceMapTo_DISPATCH(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceMapTo__(pResource, pParams);
}

static inline NV_STATUS subdeviceUnmapFrom_DISPATCH(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceUnmapFrom__(pResource, pParams);
}

static inline NvU32 subdeviceGetRefCount_DISPATCH(struct Subdevice *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__subdeviceGetRefCount__(pResource);
}

static inline void subdeviceAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__subdeviceAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * subdeviceGetNotificationListPtr_DISPATCH(struct Subdevice *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__subdeviceGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * subdeviceGetNotificationShare_DISPATCH(struct Subdevice *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__subdeviceGetNotificationShare__(pNotifier);
}

static inline void subdeviceSetNotificationShare_DISPATCH(struct Subdevice *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__subdeviceSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS subdeviceUnregisterEvent_DISPATCH(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__subdeviceUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS subdeviceGetOrAllocNotifShare_DISPATCH(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__subdeviceGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

void subdevicePreDestruct_IMPL(struct Subdevice *pResource);

NV_STATUS subdeviceInternalControlForward_IMPL(struct Subdevice *pSubdevice, NvU32 command, void *pParams, NvU32 size);

NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_VF(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBiosGetNbsiV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS *pNbsiParams);

NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_VF(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams);

NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams);

NV_STATUS subdeviceCtrlCmdBiosGetPostTime_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime);

static inline NV_STATUS subdeviceCtrlCmdBiosGetPostTime_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams);

NV_STATUS subdeviceCtrlCmdMcGetArchInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams);

NV_STATUS subdeviceCtrlCmdMcGetManufacturer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams);

NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams);

NV_STATUS subdeviceCtrlCmdMcServiceInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams);

NV_STATUS subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMcGetIntrCategorySubtreeMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_INTR_CATEGORY_SUBTREE_MAP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMcGetStaticIntrTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDmaGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams);

NV_STATUS subdeviceCtrlCmdBusGetPciInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams);

NV_STATUS subdeviceCtrlCmdBusGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams);

NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams);

NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams);

NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams);

NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetBFD_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams);

NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusClearPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusFreezePexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusSetEomParameters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetEomStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieReqAtomicsCaps_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieCplAtomicsCaps_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusGetC2CInfo_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CInfo_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusGetC2CErrorInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetC2CLpwrStats_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CLpwrStats_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusSetC2CLpwrStateVote_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetC2CLpwrStateVote_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBusSysmemAccess_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusSetP2pMapping_VF(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetP2pMapping_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdBusUnsetP2pMapping_VF(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusUnsetP2pMapping_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetBWMode_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkSetBWMode_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdGetNvlinkCountersV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdClearNvlinkCountersV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGetNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdClearNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBusGetNvlinkErrInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkInjectSWError_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkConfigureL1Toggle_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlNvlinkGetL1Toggle_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinGetLinkFomValues_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkSetupEom_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkClearLpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkInbandSendData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkPostFaultUp_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdNvlinkEomControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkSetL1Threshold_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkDirectConnectCheck_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetL1Threshold_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdIsNvlinkReducedConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetPortEvents_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkIsGpuDegraded_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkSetNvleEnabledState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPAOS_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLTC_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLC_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCAM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTECR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEWE_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSDE_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTCAP_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMTU_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMLP_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessGHPKT_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPDDR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPTT_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPCNT_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMGIR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPAOS_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPHCR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLTP_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPGUID_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRT_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTYS_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLRG_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMAOS_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMORD_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEIM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIE_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMPSCR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLS_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMLPC_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLIB_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCSR_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_MCSR_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleLids_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTASV2_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetFirmwareVersionInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkSaveNodeHostname_IMPL_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvlinkGetSavedNodeHostname_IMPL_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdI2cReadBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams);

NV_STATUS subdeviceCtrlCmdI2cWriteBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams);

NV_STATUS subdeviceCtrlCmdI2cReadReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);

NV_STATUS subdeviceCtrlCmdI2cWriteReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);

NV_STATUS subdeviceCtrlCmdThermalSystemExecuteV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *pSystemExecuteParams);

NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_VF(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams);

NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams);

NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams);

static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdPerfSetAuxPowerState_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS *pPowerStateParams);

NV_STATUS subdeviceCtrlCmdPerfSetPowerstate_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS *pPowerInfoParams);

NV_STATUS subdeviceCtrlCmdPerfGetLevelInfo_V2_VF(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *pLevelInfoParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetLevelInfo_V2_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *pLevelInfoParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdPerfGetCurrentPstate_VF(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *pInfoParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetCurrentPstate_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *pInfoParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_VF(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *pSampleParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *pSampleParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdPerfGetPowerstate_VF(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *powerInfoParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetPowerstate_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *powerInfoParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS subdeviceCtrlCmdPerfNotifyVideoevent_ac1694(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS *pVideoEventParams) {
    return NV_OK;
}

static inline NV_STATUS subdeviceCtrlCmdPerfNotifyVideoevent_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS *pVideoEventParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdPerfGetTegraPerfmonSample_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_TEGRA_PERFMON_SAMPLE_PARAMS *pSampleParams);

NV_STATUS subdeviceCtrlCmdKPerfBoost_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams);

NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams);

NV_STATUS subdeviceCtrlCmdFbGetBar1Offset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams);

NV_STATUS subdeviceCtrlCmdFbIsKind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams);

NV_STATUS subdeviceCtrlCmdFbGetMemAlignment_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFbGetCarveoutRegionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CARVEOUT_REGION_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams);

NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams);

NV_STATUS subdeviceCtrlCmdFbFlushGpuCache_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams);

NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams);

NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams);

NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFbSetupVprRegion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams);

NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetRemappedRows_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);

NV_STATUS subdeviceCtrlCmdFbGetFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams);

NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetNumaInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_VF(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *pParams) {
    return subdeviceInternalControlForward(pSubdevice, (545262184), pParams, sizeof (*pParams));
}

NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbCBCOp_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbSetRrd_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbSetReadLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbSetWriteLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGmmuCommitTlbInvalidate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFbGetStaticBar1Info_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *pConfig) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *pConfig) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS *pConfig);

NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *pConfig) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdSetGpfifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams);

NV_STATUS subdeviceCtrlCmdSetOperationalProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties);

NV_STATUS subdeviceCtrlCmdFifoBindEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams);

NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams);

NV_STATUS subdeviceCtrlCmdFifoDisableChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams);

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *pDisableChannelParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *pDisableChannelParams);

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *pDisableChannelParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *pDisableChannelParams);

NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams);

NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams);

NV_STATUS subdeviceCtrlCmdFifoObjschedSwGetLog_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoObjschedGetState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoObjschedSetState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetCaps_ac1694(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *pParams) {
    return NV_OK;
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetCaps_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS *pGetChannelGrpUidParams);

NV_STATUS subdeviceCtrlCmdFifoQueryChannelUniqueId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS *pQueryChannelUidParams);

static inline NV_STATUS subdeviceCtrlCmdFifoConfigCtxswTimeout_56cd7a(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_CONFIG_CTXSW_TIMEOUT_PARAMS *pParams) {
    return NV_OK;
}

static inline NV_STATUS subdeviceCtrlCmdFifoConfigCtxswTimeout_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_CONFIG_CTXSW_TIMEOUT_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_VF(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams);

static inline NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo);

NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams);

NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalFifoGetNumSecureChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS *pNumSecureChannelsParams);

NV_STATUS subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *pToggleActiveChannelSchedulingParams);

NV_STATUS subdeviceCtrlCmdKGrGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCapsV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams);

NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswSetupBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetROPInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetPpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueThrottleCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetTpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetZcullMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueThrottleCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_THROTTLE_CTRL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);

NV_STATUS subdeviceCtrlCmdGpuForceGspUnload_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_FORCE_GSP_UNLOAD_PARAMS *pGpuInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams);

NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams);

NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams);

NV_STATUS subdeviceCtrlCmdGpuGetShortNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams);

NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams);

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params);

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params);

NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuSetPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams);

NV_STATUS subdeviceCtrlCmdGpuGetSdm_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams);

NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams);

NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams);

NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams);

NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams);

NV_STATUS subdeviceCtrlCmdGpuGetPesInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuExecRegOps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams);

NV_STATUS subdeviceCtrlCmdGpuMigratableOps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pRegParams);

NV_STATUS subdeviceCtrlCmdGpuQueryMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode);

NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo);

NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo);

NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_VF(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdGpuGetChipDetails_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo);

NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo);

NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams);

NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams);

NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuInitializeCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams);

NV_STATUS subdeviceCtrlCmdGpuPromoteCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams);

NV_STATUS subdeviceCtrlCmdGpuEvictCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams);

NV_STATUS subdeviceCtrlCmdGpuGetId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams);

NV_STATUS subdeviceCtrlCmdGpuGetGidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams);

NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdGpuGetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);

NV_STATUS subdeviceCtrlCmdGpuSetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);

NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_VF(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdGpuGetVprCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetVprInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetPids_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams);

NV_STATUS subdeviceCtrlCmdGpuGetPidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams);

NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetSkylineInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SKYLINE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetVmmuSegmentSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuHandleVfPriFault_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetGfid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GFID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGetP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForReset_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForReset_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdGpuGetResetStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetDrainAndResetStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetConstructedFalconInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlGpuGetFipsStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetVfCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetRecoveryAction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuRpcGspTest_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuRpcGspQuerySizes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdRusdGetSupportedFeatures_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdEventSetTrigger_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams);

NV_STATUS subdeviceCtrlCmdEventSetNotification_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);

NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams);

NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams);

NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pBindParams);

NV_STATUS subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *pBindParams);

NV_STATUS subdeviceCtrlCmdTimerCancel_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdTimerSchedule_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdTimerGetTime_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams);

NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdRcReadVirtualMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam);

NV_STATUS subdeviceCtrlCmdRcGetErrorCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount);

NV_STATUS subdeviceCtrlCmdRcGetErrorV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams);

NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams);

NV_STATUS subdeviceCtrlCmdRcDisableWatchdog_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdRcEnableWatchdog_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdSetRcRecovery_56cd7a(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return NV_OK;
}

static inline NV_STATUS subdeviceCtrlCmdSetRcRecovery_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdGetRcRecovery_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery);

static inline NV_STATUS subdeviceCtrlCmdGetRcRecovery_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdGetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdSetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdNvdGetDumpSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams);

NV_STATUS subdeviceCtrlCmdNvdGetDump_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams);

NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams);

NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams);

NV_STATUS subdeviceCtrlCmdNvdInsertNocatJournalRecord_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS *pReportParams);

NV_STATUS subdeviceCtrlCmdPmgrGetModuleInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PMGR_MODULE_INFO_PARAMS *pModuleInfoParams);

NV_STATUS subdeviceCtrlCmdGc6Entry_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGc6Exit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_EXIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdLpwrDifrPrefetchResponse_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdLpwrDifrCtrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCeGetCaps_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetCePceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams);

NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *pCeUpdatePceLceMappingsParams);

NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappingsV2_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS *pCeUpdatePceLceMappingsParams);

NV_STATUS subdeviceCtrlCmdCeGetLceShimInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceType_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCeGetCapsV2_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetCapsV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetAllCaps_VF(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetAllCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetDecompLceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCeIsDecompLceEnabled_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams);

NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdEccGetVolatileCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdEccInjectError_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_INJECT_ERROR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdEccGetRepairStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration_VF(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration_VF(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus_VF(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdEccGetSramUniqueUncorrCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlaRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlaGetRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGspGetFeatures_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams);

static inline NV_STATUS subdeviceCtrlCmdGspGetFeatures_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdGspGetRmHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS *pGspRmHeapStatsParams);

NV_STATUS subdeviceCtrlCmdGpuGetVgpuHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdLibosGetHeapStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *pGspLibosHeapStatsParams);

NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuDescribePartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuSetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetComputeProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetComputeProfileCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayPinsetsToLockpins_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpioProgramDirection_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpioProgramOutput_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpioReadInput_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpioActivateHwFunction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalDisplayPreModeSet_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalDisplayPostModeSet_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdDisplayPreUnixConsole_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdDisplayPostUnixConsole_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGetChipInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalGetSmcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalCheckCtsIdValid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CHECK_CTS_ID_VALID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdIsEgpuBridge_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpuGetPFBar1Spa_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS *pConfigParams);

NV_STATUS subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS *pCopyServerReservedPdesParams);

NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);

NV_STATUS subdeviceCtrlCmdCeUpdateClassDB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params);

NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params);

NV_STATUS subdeviceCtrlCmdCeGetHubPceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCeGetHubPceMaskV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdIntrGetKernelTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfGetAuxPowerState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGcxEntryPrerequisite_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS *pGcxEntryPrerequisite);

NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_VF(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBifSetPcieRo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdBifDisableSystemMemoryAccess_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubGetNumUnits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubNextHshubId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubGetMaxHshubsPerShim_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdHshubEgmConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkCoreCallback_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetAliEnabled_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramBufferready_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkLinkTrainAli_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkAreLinksTrained_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkResetLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalNvlinkRCUserModeChannels_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalSetP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalRemoveP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetLocalAtsConfig_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetLocalAtsConfig_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetPeerAtsConfig_46f6a7(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetPeerAtsConfig_92bfc3(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS subdeviceCtrlCmdInternalInitGpuIntr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncOptimizeTiming_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncGetDisplayIds_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncSetStereoSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncGetVactiveLines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS *pAttachParams);

NV_STATUS subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalFbsrInit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPostInitBrightcStateLoad_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalSetStaticEdidData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalDetectHsVideoBridge_IMPL(struct Subdevice *pSubdevice);

NV_STATUS subdeviceCtrlCmdInternalConfComputeGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeRotateKeys_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeSetGpuState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalInitUserSharedData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalControlGspTrace_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpuSetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS *pConfigParams);

NV_STATUS subdeviceCtrlCmdInternalLogOobXid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdInternalGpuGetHfrpInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_HFRP_INFO_PARAMS *pHfrpParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeStates_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams);

NV_STATUS subdeviceCtrlSetEcThrottleMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCcuMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCcuUnmap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCcuSetStreamState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdCcuGetSampleInfo_VF(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdCcuGetSampleInfo_5baef9(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS subdeviceCtrlCmdSpdmPartition_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams);

NV_STATUS subdeviceSpdmRetrieveTranscript_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *pSpdmRetrieveSessionTranscriptParams);

static inline NV_STATUS subdeviceSetPerfmonReservation(struct Subdevice *pSubdevice, NvBool bReservation, NvBool bClientHandlesGrGating, NvBool bRmHandlesIdleSlow) {
    return NV_OK;
}

static inline NV_STATUS subdeviceReleaseVideoStreams(struct Subdevice *pSubdevice) {
    return NV_OK;
}

static inline void subdeviceRestoreLockedClock(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    return;
}

static inline void subdeviceRestoreVF(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    return;
}

static inline void subdeviceReleaseNvlinkErrorInjectionMode(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    return;
}

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SUBDEVICE_NVOC_H_
