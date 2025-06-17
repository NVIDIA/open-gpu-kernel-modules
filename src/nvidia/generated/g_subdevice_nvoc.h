
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

    // Vtable with 630 per-object function pointers
    NV_STATUS (*__subdeviceCtrlCmdBiosGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *);  // halified (2 hals) exported (id=0x20800810) body
    NV_STATUS (*__subdeviceCtrlCmdBiosGetNbsiV2__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS *);  // exported (id=0x2080080e)
    NV_STATUS (*__subdeviceCtrlCmdBiosGetSKUInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800808)
    NV_STATUS (*__subdeviceCtrlCmdBiosGetPostTime__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *);  // halified (2 hals) exported (id=0x20800809) body
    NV_STATUS (*__subdeviceCtrlCmdBiosGetUefiSupport__)(struct Subdevice * /*this*/, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *);  // exported (id=0x2080080b)
    NV_STATUS (*__subdeviceCtrlCmdMcGetArchInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *);  // exported (id=0x20801701)
    NV_STATUS (*__subdeviceCtrlCmdMcGetManufacturer__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *);  // exported (id=0x20801703)
    NV_STATUS (*__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *);  // exported (id=0x2080170c)
    NV_STATUS (*__subdeviceCtrlCmdMcServiceInterrupts__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *);  // exported (id=0x20801702)
    NV_STATUS (*__subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *);  // exported (id=0x2080170d)
    NV_STATUS (*__subdeviceCtrlCmdMcGetStaticIntrTable__)(struct Subdevice * /*this*/, NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *);  // exported (id=0x2080170e)
    NV_STATUS (*__subdeviceCtrlCmdDmaInvalidateTLB__)(struct Subdevice * /*this*/, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *);  // exported (id=0x20802502)
    NV_STATUS (*__subdeviceCtrlCmdDmaGetInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_DMA_GET_INFO_PARAMS *);  // exported (id=0x20802503)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPciInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *);  // exported (id=0x20801801)
    NV_STATUS (*__subdeviceCtrlCmdBusGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *);  // exported (id=0x20801823)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPciBarInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *);  // exported (id=0x20801803)
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieSpeed__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *);  // exported (id=0x20801805)
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieLinkWidth__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *);  // exported (id=0x20801804)
    NV_STATUS (*__subdeviceCtrlCmdBusServiceGpuMultifunctionState__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *);  // exported (id=0x20801812)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *);  // exported (id=0x20801813)
    NV_STATUS (*__subdeviceCtrlCmdBusGetBFD__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *);  // exported (id=0x20801821)
    NV_STATUS (*__subdeviceCtrlCmdBusGetAspmDisableFlags__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *);  // exported (id=0x20801822)
    NV_STATUS (*__subdeviceCtrlCmdBusControlPublicAspmBits__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *);  // exported (id=0x20801824)
    NV_STATUS (*__subdeviceCtrlCmdBusClearPexCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *);  // exported (id=0x20801814)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexUtilCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *);  // exported (id=0x20801819)
    NV_STATUS (*__subdeviceCtrlCmdBusClearPexUtilCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *);  // exported (id=0x20801820)
    NV_STATUS (*__subdeviceCtrlCmdBusFreezePexCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *);  // exported (id=0x20801815)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexLaneCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *);  // exported (id=0x20801816)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieLtrLatency__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *);  // exported (id=0x20801817)
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieLtrLatency__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *);  // exported (id=0x20801818)
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *);  // exported (id=0x20801825)
    NV_STATUS (*__subdeviceCtrlCmdBusSetEomParameters__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *);  // exported (id=0x20801826)
    NV_STATUS (*__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *);  // exported (id=0x20801827)
    NV_STATUS (*__subdeviceCtrlCmdBusGetEomStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *);  // exported (id=0x20801828)
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieReqAtomicsCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801829) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *);  // halified (2 hals) exported (id=0x2080182a) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieCplAtomicsCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801830) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetC2CInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *);  // halified (2 hals) exported (id=0x2080182b) body
    NV_STATUS (*__subdeviceCtrlCmdBusGetC2CErrorInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS *);  // exported (id=0x2080182d)
    NV_STATUS (*__subdeviceCtrlCmdBusGetC2CLpwrStats__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *);  // halified (2 hals) exported (id=0x20801831) body
    NV_STATUS (*__subdeviceCtrlCmdBusSetC2CLpwrStateVote__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *);  // halified (2 hals) exported (id=0x20801832) body
    NV_STATUS (*__subdeviceCtrlCmdBusSysmemAccess__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *);  // exported (id=0x2080182c)
    NV_STATUS (*__subdeviceCtrlCmdBusSetP2pMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *);  // halified (2 hals) exported (id=0x2080182e) body
    NV_STATUS (*__subdeviceCtrlCmdBusUnsetP2pMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *);  // halified (2 hals) exported (id=0x2080182f) body
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetSupportedCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *);  // exported (id=0x20803074)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetSupportedBWMode__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *);  // exported (id=0x20803085)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetBWMode__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *);  // exported (id=0x20803087)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetBWMode__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *);  // exported (id=0x20803086)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLocalDeviceInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *);  // exported (id=0x20803088)
    NV_STATUS (*__subdeviceCtrlCmdGetNvlinkCountersV2__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS *);  // exported (id=0x20803050)
    NV_STATUS (*__subdeviceCtrlCmdClearNvlinkCountersV2__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS *);  // exported (id=0x20803051)
    NV_STATUS (*__subdeviceCtrlCmdGetNvlinkCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *);  // exported (id=0x20803004)
    NV_STATUS (*__subdeviceCtrlCmdClearNvlinkCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *);  // exported (id=0x20803005)
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20803001)
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *);  // exported (id=0x20803002)
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkErrInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *);  // exported (id=0x20803003)
    NV_STATUS (*__subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS *);  // exported (id=0x20803081)
    NV_STATUS (*__subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS *);  // exported (id=0x20803082)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkInjectSWError__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS *);  // exported (id=0x20803089)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkConfigureL1Toggle__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS *);  // exported (id=0x2080308e)
    NV_STATUS (*__subdeviceCtrlNvlinkGetL1Toggle__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS *);  // exported (id=0x2080308f)
    NV_STATUS (*__subdeviceCtrlCmdNvlinGetLinkFomValues__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS *);  // exported (id=0x20803011)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS *);  // exported (id=0x20803014)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *);  // exported (id=0x20803009)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetupEom__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *);  // exported (id=0x2080300c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetPowerState__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *);  // exported (id=0x2080300e)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkReadTpCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *);  // exported (id=0x20803015)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLpCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *);  // exported (id=0x20803018)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkClearLpCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *);  // exported (id=0x20803052)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetLoopbackMode__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *);  // exported (id=0x20803023)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetRefreshCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *);  // exported (id=0x20803028)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkClearRefreshCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *);  // exported (id=0x20803029)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *);  // exported (id=0x20803038)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *);  // exported (id=0x20803039)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEnableLinks__)(struct Subdevice * /*this*/);  // exported (id=0x2080303a)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *);  // exported (id=0x2080303b)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkInbandSendData__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *);  // exported (id=0x20803040)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPostFaultUp__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *);  // exported (id=0x20803043)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPostLazyErrorRecovery__)(struct Subdevice * /*this*/);  // exported (id=0x2080308a)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEomControl__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS *);  // exported (id=0x2080303c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetL1Threshold__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS *);  // exported (id=0x2080303e)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkDirectConnectCheck__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS *);  // exported (id=0x20803042)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetL1Threshold__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS *);  // exported (id=0x2080303f)
    NV_STATUS (*__subdeviceCtrlCmdIsNvlinkReducedConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS *);  // exported (id=0x20803046)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetPortEvents__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS *);  // exported (id=0x20803044)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkIsGpuDegraded__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS *);  // exported (id=0x20803041)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPAOS__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS *);  // exported (id=0x20803047)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPLTC__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS *);  // exported (id=0x20803053)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPLM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS *);  // exported (id=0x20803054)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPSLC__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS *);  // exported (id=0x20803055)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMCAM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS *);  // exported (id=0x20803056)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTECR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS *);  // exported (id=0x2080305c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTEWE__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS *);  // exported (id=0x2080305e)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTSDE__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS *);  // exported (id=0x2080305f)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTCAP__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS *);  // exported (id=0x20803061)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPMTU__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS *);  // exported (id=0x20803062)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPMLP__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS *);  // exported (id=0x20803064)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessGHPKT__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS *);  // exported (id=0x20803065)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPDDR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS *);  // exported (id=0x20803066)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPTT__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS *);  // exported (id=0x20803068)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPCNT__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS *);  // exported (id=0x20803069)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMGIR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS *);  // exported (id=0x2080306a)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPAOS__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS *);  // exported (id=0x2080306b)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPHCR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS *);  // exported (id=0x2080306c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessSLTP__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS *);  // exported (id=0x2080306d)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPGUID__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS *);  // exported (id=0x2080306e)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPRT__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS *);  // exported (id=0x2080306f)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPTYS__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS *);  // exported (id=0x20803070)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessSLRG__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS *);  // exported (id=0x20803071)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPMAOS__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS *);  // exported (id=0x20803072)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPLR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS *);  // exported (id=0x20803073)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMORD__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS *);  // exported (id=0x20803075)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS *);  // exported (id=0x20803076)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS *);  // exported (id=0x20803077)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS *);  // exported (id=0x20803078)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTEIM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS *);  // exported (id=0x20803079)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTIE__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS *);  // exported (id=0x2080307a)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTIM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS *);  // exported (id=0x2080307b)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMPSCR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS *);  // exported (id=0x2080307c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMTSR__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS *);  // exported (id=0x2080307d)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPSLS__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS *);  // exported (id=0x2080307e)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessMLPC__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS *);  // exported (id=0x2080307f)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPLIB__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS *);  // exported (id=0x20803080)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdateNvleTopology__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *);  // exported (id=0x2080308c)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetNvleLids__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *);  // exported (id=0x2080308d)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPTASV2__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *);  // exported (id=0x20803093)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *);  // exported (id=0x20803094)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccessPPRM__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *);  // exported (id=0x20803091)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPRMAccess__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *);  // exported (id=0x20803090)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetPlatformInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *);  // exported (id=0x20803083)
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *);  // exported (id=0x2080308b)
    NV_STATUS (*__subdeviceCtrlCmdI2cReadBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *);  // exported (id=0x20800601)
    NV_STATUS (*__subdeviceCtrlCmdI2cWriteBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *);  // exported (id=0x20800602)
    NV_STATUS (*__subdeviceCtrlCmdI2cReadReg__)(struct Subdevice * /*this*/, NV2080_CTRL_I2C_RW_REG_PARAMS *);  // exported (id=0x20800603)
    NV_STATUS (*__subdeviceCtrlCmdI2cWriteReg__)(struct Subdevice * /*this*/, NV2080_CTRL_I2C_RW_REG_PARAMS *);  // exported (id=0x20800604)
    NV_STATUS (*__subdeviceCtrlCmdThermalSystemExecuteV2__)(struct Subdevice * /*this*/, NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *);  // exported (id=0x20800513)
    NV_STATUS (*__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *);  // halified (2 hals) exported (id=0x20802096)
    NV_STATUS (*__subdeviceCtrlCmdPerfRatedTdpGetControl__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *);  // exported (id=0x2080206e)
    NV_STATUS (*__subdeviceCtrlCmdPerfRatedTdpSetControl__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *);  // halified (singleton optimized) exported (id=0x2080206f)
    NV_STATUS (*__subdeviceCtrlCmdPerfReservePerfmonHw__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *);  // halified (2 hals) exported (id=0x20802093) body
    NV_STATUS (*__subdeviceCtrlCmdPerfSetAuxPowerState__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS *);  // halified (singleton optimized) exported (id=0x20802092)
    NV_STATUS (*__subdeviceCtrlCmdPerfSetPowerstate__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS *);  // halified (singleton optimized) exported (id=0x2080205b)
    NV_STATUS (*__subdeviceCtrlCmdPerfGetLevelInfo_V2__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *);  // halified (2 hals) exported (id=0x2080200b) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetCurrentPstate__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *);  // halified (2 hals) exported (id=0x20802068) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *);  // halified (2 hals) exported (id=0x20802087) body
    NV_STATUS (*__subdeviceCtrlCmdPerfGetPowerstate__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *);  // halified (2 hals) exported (id=0x2080205a) body
    NV_STATUS (*__subdeviceCtrlCmdPerfNotifyVideoevent__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS *);  // halified (2 hals) exported (id=0x2080205d) body
    NV_STATUS (*__subdeviceCtrlCmdKPerfBoost__)(struct Subdevice * /*this*/, NV2080_CTRL_PERF_BOOST_PARAMS *);  // exported (id=0x2080200a)
    NV_STATUS (*__subdeviceCtrlCmdFbGetFBRegionInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *);  // exported (id=0x20801320)
    NV_STATUS (*__subdeviceCtrlCmdFbGetBar1Offset__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *);  // exported (id=0x20801310)
    NV_STATUS (*__subdeviceCtrlCmdFbIsKind__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_IS_KIND_PARAMS *);  // exported (id=0x20801313)
    NV_STATUS (*__subdeviceCtrlCmdFbGetMemAlignment__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *);  // exported (id=0x20801342)
    NV_STATUS (*__subdeviceCtrlCmdFbGetHeapReservationSize__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *);  // exported (id=0x20800a5b)
    NV_STATUS (*__subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS *);  // exported (id=0x20800ac5)
    NV_STATUS (*__subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *);  // exported (id=0x20801352)
    NV_STATUS (*__subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS *);  // halified (singleton optimized) exported (id=0x20800afa) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *);  // exported (id=0x20801303)
    NV_STATUS (*__subdeviceCtrlCmdFbGetCalibrationLockFailed__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *);  // exported (id=0x2080130c)
    NV_STATUS (*__subdeviceCtrlCmdFbFlushGpuCache__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *);  // exported (id=0x2080130e)
    NV_STATUS (*__subdeviceCtrlCmdFbGetGpuCacheInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *);  // exported (id=0x20801315)
    NV_STATUS (*__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *);  // exported (id=0x2080133c)
    NV_STATUS (*__subdeviceCtrlCmdFbGetOfflinedPages__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *);  // halified (2 hals) exported (id=0x20801322) body
    NV_STATUS (*__subdeviceCtrlCmdFbSetupVprRegion__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *);  // exported (id=0x2080133b)
    NV_STATUS (*__subdeviceCtrlCmdFbGetLTCInfoForFBP__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *);  // halified (2 hals) exported (id=0x20801328) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *);  // exported (id=0x2080133d)
    NV_STATUS (*__subdeviceCtrlCmdFbPatchPbrForMining__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *);  // exported (id=0x20801341)
    NV_STATUS (*__subdeviceCtrlCmdFbGetRemappedRows__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *);  // exported (id=0x20801344)
    NV_STATUS (*__subdeviceCtrlCmdFbGetFsInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *);  // exported (id=0x20801346)
    NV_STATUS (*__subdeviceCtrlCmdFbGetRowRemapperHistogram__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *);  // exported (id=0x20801347)
    NV_STATUS (*__subdeviceCtrlCmdFbGetDynamicOfflinedPages__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *);  // halified (2 hals) exported (id=0x20801348) body
    NV_STATUS (*__subdeviceCtrlCmdFbUpdateNumaStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *);  // exported (id=0x20801350)
    NV_STATUS (*__subdeviceCtrlCmdFbGetNumaInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *);  // exported (id=0x20801351)
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetStaticConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800a1c) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysSetPartitionableMem__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *);  // exported (id=0x20800a51)
    NV_STATUS (*__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *);  // inline exported (id=0x20800a67) body
    NV_STATUS (*__subdeviceCtrlCmdFbSetZbcReferenced__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *);  // exported (id=0x20800a69)
    NV_STATUS (*__subdeviceCtrlCmdMemSysL2InvalidateEvict__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *);  // exported (id=0x20800a6c)
    NV_STATUS (*__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__)(struct Subdevice * /*this*/);  // exported (id=0x20800a6d)
    NV_STATUS (*__subdeviceCtrlCmdMemSysDisableNvlinkPeers__)(struct Subdevice * /*this*/);  // exported (id=0x20800a6e)
    NV_STATUS (*__subdeviceCtrlCmdMemSysProgramRawCompressionMode__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *);  // exported (id=0x20800a6f)
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *);  // halified (2 hals) exported (id=0x20800a6b) body
    NV_STATUS (*__subdeviceCtrlCmdFbGetCtagsForCbcEviction__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *);  // exported (id=0x20801338)
    NV_STATUS (*__subdeviceCtrlCmdFbCBCOp__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *);  // exported (id=0x20801337)
    NV_STATUS (*__subdeviceCtrlCmdFbSetRrd__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_SET_RRD_PARAMS *);  // exported (id=0x2080133e)
    NV_STATUS (*__subdeviceCtrlCmdFbSetReadLimit__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *);  // exported (id=0x2080133f)
    NV_STATUS (*__subdeviceCtrlCmdFbSetWriteLimit__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *);  // exported (id=0x20801340)
    NV_STATUS (*__subdeviceCtrlCmdGmmuCommitTlbInvalidate__)(struct Subdevice * /*this*/, NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS *);  // exported (id=0x20801353)
    NV_STATUS (*__subdeviceCtrlCmdFbGetStaticBar1Info__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *);  // exported (id=0x20801354)
    NV_STATUS (*__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20801355) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20801356) body
    NV_STATUS (*__subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS *);  // exported (id=0x20801357)
    NV_STATUS (*__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x20801358) body
    NV_STATUS (*__subdeviceCtrlCmdSetGpfifo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *);  // exported (id=0x20801102)
    NV_STATUS (*__subdeviceCtrlCmdSetOperationalProperties__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *);  // exported (id=0x20801104)
    NV_STATUS (*__subdeviceCtrlCmdFifoBindEngines__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *);  // exported (id=0x20801103)
    NV_STATUS (*__subdeviceCtrlCmdGetPhysicalChannelCount__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *);  // exported (id=0x20801108)
    NV_STATUS (*__subdeviceCtrlCmdFifoGetInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_INFO_PARAMS *);  // exported (id=0x20801109)
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannels__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *);  // exported (id=0x2080110b)
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *);  // halified (2 hals) exported (id=0x2080111a) body
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *);  // halified (2 hals) exported (id=0x2080111b) body
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableUsermodeChannels__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *);  // exported (id=0x20801117)
    NV_STATUS (*__subdeviceCtrlCmdFifoGetChannelMemInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *);  // exported (id=0x2080110c)
    NV_STATUS (*__subdeviceCtrlCmdFifoGetUserdLocation__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *);  // exported (id=0x2080110d)
    NV_STATUS (*__subdeviceCtrlCmdFifoObjschedSwGetLog__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS *);  // exported (id=0x2080110e)
    NV_STATUS (*__subdeviceCtrlCmdFifoObjschedGetState__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS *);  // exported (id=0x20801120)
    NV_STATUS (*__subdeviceCtrlCmdFifoObjschedSetState__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS *);  // exported (id=0x20801121)
    NV_STATUS (*__subdeviceCtrlCmdFifoObjschedGetCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20801122) body
    NV_STATUS (*__subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS *);  // exported (id=0x20801123)
    NV_STATUS (*__subdeviceCtrlCmdFifoQueryChannelUniqueId__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS *);  // exported (id=0x20801124)
    NV_STATUS (*__subdeviceCtrlCmdFifoGetDeviceInfoTable__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *);  // halified (2 hals) exported (id=0x20801112) body
    NV_STATUS (*__subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *);  // exported (id=0x20801118)
    NV_STATUS (*__subdeviceCtrlCmdFifoClearFaultedBit__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *);  // exported (id=0x20801113)
    NV_STATUS (*__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *);  // exported (id=0x20801115)
    NV_STATUS (*__subdeviceCtrlCmdFifoUpdateChannelInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20801116) body
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *);  // exported (id=0x20800a53)
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoGetNumChannels__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *);  // exported (id=0x20800a61)
    NV_STATUS (*__subdeviceCtrlCmdFifoGetAllocatedChannels__)(struct Subdevice * /*this*/, NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *);  // exported (id=0x20801119)
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoGetNumSecureChannels__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS *);  // exported (id=0x20800ad8)
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *);  // exported (id=0x20800ac3)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_INFO_PARAMS *);  // exported (id=0x20801201)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *);  // exported (id=0x20801228)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCapsV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *);  // exported (id=0x20801227)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxswModes__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *);  // exported (id=0x20801235)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswZcullMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *);  // exported (id=0x20801205)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswZcullBind__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *);  // exported (id=0x20801208)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetZcullInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *);  // exported (id=0x20801206)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPmMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *);  // halified (2 hals) exported (id=0x20801207) body
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPmBind__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *);  // exported (id=0x20801209)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswSetupBind__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS *);  // exported (id=0x2080123a)
    NV_STATUS (*__subdeviceCtrlCmdKGrSetGpcTileMap__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *);  // exported (id=0x2080120a)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswSmpcMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *);  // exported (id=0x2080120e)
    NV_STATUS (*__subdeviceCtrlCmdKGrPcSamplingMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *);  // exported (id=0x20801212)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *);  // exported (id=0x2080120f)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGlobalSmOrder__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *);  // exported (id=0x2080121b)
    NV_STATUS (*__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *);  // exported (id=0x20801210)
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPreemptionBind__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *);  // exported (id=0x20801211)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetROPInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *);  // exported (id=0x20801213)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxswStats__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *);  // exported (id=0x20801215)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferSize__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *);  // exported (id=0x20801218)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *);  // exported (id=0x20801219)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferPtes__)(struct Subdevice * /*this*/, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *);  // exported (id=0x20800a28)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCurrentResidentChannel__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *);  // exported (id=0x2080121c)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetVatAlarmData__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *);  // exported (id=0x2080121d)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetAttributeBufferSize__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *);  // exported (id=0x2080121e)
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolQuerySize__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *);  // exported (id=0x2080121f)
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolInitialize__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *);  // exported (id=0x20801220)
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolAddSlots__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *);  // exported (id=0x20801221)
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *);  // exported (id=0x20801222)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetPpcMask__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *);  // exported (id=0x20801233)
    NV_STATUS (*__subdeviceCtrlCmdKGrSetTpcPartitionMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x2080122c)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetSmIssueRateModifier__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *);  // exported (id=0x20801230)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetSmIssueRateModifierV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *);  // exported (id=0x2080123c)
    NV_STATUS (*__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *);  // exported (id=0x20801231)
    NV_STATUS (*__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *);  // exported (id=0x20801238)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetPhysGpcMask__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *);  // exported (id=0x20801232)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGpcMask__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *);  // exported (id=0x2080122a)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetTpcMask__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *);  // exported (id=0x2080122b)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetEngineContextProperties__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *);  // exported (id=0x2080122d)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetNumTpcsForGpc__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *);  // exported (id=0x20801234)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGpcTileMap__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *);  // exported (id=0x20801236)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetZcullMask__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *);  // exported (id=0x20801237)
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *);  // exported (id=0x20801239)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *);  // exported (id=0x20800a2a)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *);  // exported (id=0x20800a1f)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *);  // exported (id=0x20800a22)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *);  // exported (id=0x20800a26)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *);  // exported (id=0x20800a30)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *);  // exported (id=0x20800a2c)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *);  // exported (id=0x20800a2e)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *);  // exported (id=0x20800a32)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *);  // exported (id=0x20800a34)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *);  // exported (id=0x20800b03)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *);  // exported (id=0x20800a3d)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *);  // exported (id=0x20800a3f)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *);  // exported (id=0x20800a48)
    NV_STATUS (*__subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *);  // exported (id=0x20800a37)
    NV_STATUS (*__subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *);  // exported (id=0x20800a38)
    NV_STATUS (*__subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *);  // exported (id=0x20800a39)
    NV_STATUS (*__subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *);  // exported (id=0x20800a3b)
    NV_STATUS (*__subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *);  // exported (id=0x20800a3a)
    NV_STATUS (*__subdeviceCtrlCmdGrStaticGetFecsTraceDefines__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *);  // exported (id=0x20800a3e)
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalInitBug4208224War__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *);  // exported (id=0x20800a46)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetCachedInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *);  // exported (id=0x20800182)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *);  // exported (id=0x20800102)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetIpVersion__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *);  // exported (id=0x2080014d)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *);  // exported (id=0x2080015a)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *);  // exported (id=0x2080015b)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetOptimusInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *);  // exported (id=0x2080014c)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNameString__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *);  // exported (id=0x20800110)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetShortNameString__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *);  // exported (id=0x20800111)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEncoderCapacity__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *);  // exported (id=0x2080016c)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvencSwSessionStats__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *);  // exported (id=0x2080016d)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *);  // exported (id=0x2080016e)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *);  // exported (id=0x208001af)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *);  // exported (id=0x2080017b)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *);  // exported (id=0x2080017c)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetFabricAddr__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *);  // exported (id=0x2080016f)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetEgmGpaFabricAddr__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS *);  // exported (id=0x20800199)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPower__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_POWER_PARAMS *);  // exported (id=0x20800112)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetSdm__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_SDM_PARAMS *);  // exported (id=0x20800118)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetSimulationInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *);  // exported (id=0x20800119)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngines__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *);  // exported (id=0x20800123)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEnginesV2__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *);  // exported (id=0x20800170)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineClasslist__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *);  // exported (id=0x20800124)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEnginePartnerList__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *);  // exported (id=0x20800147)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiGpcInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *);  // exported (id=0x20800137)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiTpcInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *);  // exported (id=0x20800138)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiZcullInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *);  // exported (id=0x20800139)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPesInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *);  // exported (id=0x20800168)
    NV_STATUS (*__subdeviceCtrlCmdGpuExecRegOps__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *);  // exported (id=0x20800122)
    NV_STATUS (*__subdeviceCtrlCmdGpuMigratableOps__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *);  // exported (id=0x208001a6)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *);  // exported (id=0x20800128)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInforomImageVersion__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *);  // exported (id=0x20800156)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInforomObjectVersion__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *);  // exported (id=0x2080014b)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryInforomEccSupport__)(struct Subdevice * /*this*/);  // exported (id=0x20800157)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x2080012f) body
    NV_STATUS (*__subdeviceCtrlCmdGpuGetChipDetails__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *);  // exported (id=0x208001a4)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetOEMBoardInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *);  // exported (id=0x2080013f)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetOEMInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *);  // exported (id=0x20800169)
    NV_STATUS (*__subdeviceCtrlCmdGpuHandleGpuSR__)(struct Subdevice * /*this*/);  // exported (id=0x20800167)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetComputeModeRules__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *);  // exported (id=0x20800130)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryComputeModeRules__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *);  // exported (id=0x20800131)
    NV_STATUS (*__subdeviceCtrlCmdGpuAcquireComputeModeReservation__)(struct Subdevice * /*this*/);  // exported (id=0x20800145)
    NV_STATUS (*__subdeviceCtrlCmdGpuReleaseComputeModeReservation__)(struct Subdevice * /*this*/);  // exported (id=0x20800146)
    NV_STATUS (*__subdeviceCtrlCmdGpuInitializeCtx__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *);  // exported (id=0x2080012d)
    NV_STATUS (*__subdeviceCtrlCmdGpuPromoteCtx__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *);  // exported (id=0x2080012b)
    NV_STATUS (*__subdeviceCtrlCmdGpuEvictCtx__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *);  // exported (id=0x2080012c)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetId__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ID_PARAMS *);  // exported (id=0x20800142)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetGidInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *);  // exported (id=0x2080014a)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryIllumSupport__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *);  // halified (2 hals) exported (id=0x20800153) body
    NV_STATUS (*__subdeviceCtrlCmdGpuGetIllum__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *);  // exported (id=0x20800154)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetIllum__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *);  // exported (id=0x20800155)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryScrubberStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x2080015f) body
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVprCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *);  // exported (id=0x20800160)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVprInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *);  // exported (id=0x2080016b)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPids__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PIDS_PARAMS *);  // exported (id=0x2080018d)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPidInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *);  // exported (id=0x2080018e)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryFunctionStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *);  // exported (id=0x20800173)
    NV_STATUS (*__subdeviceCtrlCmdGpuReportNonReplayableFault__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *);  // exported (id=0x20800177)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineFaultInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *);  // exported (id=0x20800125)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *);  // exported (id=0x20800179)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetHwEngineId__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *);  // exported (id=0x2080017a)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFirstAsyncCEIdx__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS *);  // exported (id=0x208001e6)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVmmuSegmentSize__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS *);  // exported (id=0x2080017e)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *);  // exported (id=0x20800188)
    NV_STATUS (*__subdeviceCtrlCmdGpuHandleVfPriFault__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *);  // exported (id=0x20800192)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetComputePolicyConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *);  // exported (id=0x20800194)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetComputePolicyConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *);  // exported (id=0x20800195)
    NV_STATUS (*__subdeviceCtrlCmdValidateMemMapRequest__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *);  // exported (id=0x20800198)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetGfid__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_GFID_PARAMS *);  // exported (id=0x20800196)
    NV_STATUS (*__subdeviceCtrlCmdUpdateGfidP2pCapability__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS *);  // exported (id=0x20800197)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineLoadTimes__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *);  // exported (id=0x2080019b)
    NV_STATUS (*__subdeviceCtrlCmdGetP2pCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_GET_P2P_CAPS_PARAMS *);  // exported (id=0x208001a0)
    NV_STATUS (*__subdeviceCtrlCmdGetGpuFabricProbeInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *);  // exported (id=0x208001a3)
    NV_STATUS (*__subdeviceCtrlCmdGpuMarkDeviceForReset__)(struct Subdevice * /*this*/);  // exported (id=0x208001a9)
    NV_STATUS (*__subdeviceCtrlCmdGpuUnmarkDeviceForReset__)(struct Subdevice * /*this*/);  // exported (id=0x208001aa)
    NV_STATUS (*__subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset__)(struct Subdevice * /*this*/);  // exported (id=0x208001ac)
    NV_STATUS (*__subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset__)(struct Subdevice * /*this*/);  // exported (id=0x208001ad)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetResetStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS *);  // exported (id=0x208001ab)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetDrainAndResetStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS *);  // exported (id=0x208001ae)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetConstructedFalconInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *);  // exported (id=0x208001b0)
    NV_STATUS (*__subdeviceCtrlGpuGetFipsStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS *);  // exported (id=0x208001e4)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVfCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS *);  // exported (id=0x208001b1)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetRecoveryAction__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS *);  // exported (id=0x208001b2)
    NV_STATUS (*__subdeviceCtrlCmdGpuRpcGspTest__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS *);  // exported (id=0x208001e8)
    NV_STATUS (*__subdeviceCtrlCmdGpuRpcGspQuerySizes__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS *);  // exported (id=0x208001e9)
    NV_STATUS (*__subdeviceCtrlCmdRusdGetSupportedFeatures__)(struct Subdevice * /*this*/, NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *);  // exported (id=0x208081ea)
    NV_STATUS (*__subdeviceCtrlCmdEventSetTrigger__)(struct Subdevice * /*this*/);  // exported (id=0x20800302)
    NV_STATUS (*__subdeviceCtrlCmdEventSetTriggerFifo__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *);  // exported (id=0x20800308)
    NV_STATUS (*__subdeviceCtrlCmdEventSetNotification__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *);  // exported (id=0x20800301)
    NV_STATUS (*__subdeviceCtrlCmdEventSetMemoryNotifies__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *);  // exported (id=0x20800303)
    NV_STATUS (*__subdeviceCtrlCmdEventSetSemaphoreMemory__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *);  // exported (id=0x20800304)
    NV_STATUS (*__subdeviceCtrlCmdEventSetSemaMemValidation__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *);  // exported (id=0x20800306)
    NV_STATUS (*__subdeviceCtrlCmdEventVideoBindEvtbuf__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *);  // exported (id=0x20800309)
    NV_STATUS (*__subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__)(struct Subdevice * /*this*/, NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *);  // exported (id=0x2080030a)
    NV_STATUS (*__subdeviceCtrlCmdTimerCancel__)(struct Subdevice * /*this*/);  // exported (id=0x20800402)
    NV_STATUS (*__subdeviceCtrlCmdTimerSchedule__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *);  // exported (id=0x20800401)
    NV_STATUS (*__subdeviceCtrlCmdTimerGetTime__)(struct Subdevice * /*this*/, NV2080_CTRL_TIMER_GET_TIME_PARAMS *);  // exported (id=0x20800403)
    NV_STATUS (*__subdeviceCtrlCmdTimerGetRegisterOffset__)(struct Subdevice * /*this*/, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *);  // exported (id=0x20800404)
    NV_STATUS (*__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *);  // exported (id=0x20800406)
    NV_STATUS (*__subdeviceCtrlCmdTimerSetGrTickFreq__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *);  // exported (id=0x20800407)
    NV_STATUS (*__subdeviceCtrlCmdRcReadVirtualMem__)(struct Subdevice * /*this*/, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *);  // exported (id=0x20802204)
    NV_STATUS (*__subdeviceCtrlCmdRcGetErrorCount__)(struct Subdevice * /*this*/, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *);  // exported (id=0x20802205)
    NV_STATUS (*__subdeviceCtrlCmdRcGetErrorV2__)(struct Subdevice * /*this*/, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *);  // exported (id=0x20802213)
    NV_STATUS (*__subdeviceCtrlCmdRcSetCleanErrorHistory__)(struct Subdevice * /*this*/);  // exported (id=0x20802207)
    NV_STATUS (*__subdeviceCtrlCmdRcGetWatchdogInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *);  // exported (id=0x20802209)
    NV_STATUS (*__subdeviceCtrlCmdRcDisableWatchdog__)(struct Subdevice * /*this*/);  // exported (id=0x2080220a)
    NV_STATUS (*__subdeviceCtrlCmdRcSoftDisableWatchdog__)(struct Subdevice * /*this*/);  // exported (id=0x20802210)
    NV_STATUS (*__subdeviceCtrlCmdRcEnableWatchdog__)(struct Subdevice * /*this*/);  // exported (id=0x2080220b)
    NV_STATUS (*__subdeviceCtrlCmdRcReleaseWatchdogRequests__)(struct Subdevice * /*this*/);  // exported (id=0x2080220c)
    NV_STATUS (*__subdeviceCtrlCmdInternalRcWatchdogTimeout__)(struct Subdevice * /*this*/);  // exported (id=0x20800a6a)
    NV_STATUS (*__subdeviceCtrlCmdSetRcRecovery__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);  // halified (2 hals) exported (id=0x2080220d) body
    NV_STATUS (*__subdeviceCtrlCmdGetRcRecovery__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);  // halified (2 hals) exported (id=0x2080220e) body
    NV_STATUS (*__subdeviceCtrlCmdGetRcInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_INFO_PARAMS *);  // exported (id=0x20802212)
    NV_STATUS (*__subdeviceCtrlCmdSetRcInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_RC_INFO_PARAMS *);  // exported (id=0x20802211)
    NV_STATUS (*__subdeviceCtrlCmdNvdGetDumpSize__)(struct Subdevice * /*this*/, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *);  // exported (id=0x20802401)
    NV_STATUS (*__subdeviceCtrlCmdNvdGetDump__)(struct Subdevice * /*this*/, NV2080_CTRL_NVD_GET_DUMP_PARAMS *);  // exported (id=0x20802402)
    NV_STATUS (*__subdeviceCtrlCmdNvdGetNocatJournalRpt__)(struct Subdevice * /*this*/, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *);  // exported (id=0x20802409)
    NV_STATUS (*__subdeviceCtrlCmdNvdSetNocatJournalData__)(struct Subdevice * /*this*/, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *);  // exported (id=0x2080240b)
    NV_STATUS (*__subdeviceCtrlCmdNvdInsertNocatJournalRecord__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS *);  // exported (id=0x2080240c)
    NV_STATUS (*__subdeviceCtrlCmdPmgrGetModuleInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_PMGR_MODULE_INFO_PARAMS *);  // exported (id=0x20802609)
    NV_STATUS (*__subdeviceCtrlCmdGpuProcessPostGc6ExitTasks__)(struct Subdevice * /*this*/);  // exported (id=0x2080270c)
    NV_STATUS (*__subdeviceCtrlCmdGc6Entry__)(struct Subdevice * /*this*/, NV2080_CTRL_GC6_ENTRY_PARAMS *);  // exported (id=0x2080270d)
    NV_STATUS (*__subdeviceCtrlCmdGc6Exit__)(struct Subdevice * /*this*/, NV2080_CTRL_GC6_EXIT_PARAMS *);  // exported (id=0x2080270e)
    NV_STATUS (*__subdeviceCtrlCmdLpwrDifrPrefetchResponse__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS *);  // exported (id=0x20802802)
    NV_STATUS (*__subdeviceCtrlCmdLpwrDifrCtrl__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS *);  // exported (id=0x20802801)
    NV_STATUS (*__subdeviceCtrlCmdCeGetCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20802a01)
    NV_STATUS (*__subdeviceCtrlCmdCeGetCePceMask__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *);  // exported (id=0x20802a02)
    NV_STATUS (*__subdeviceCtrlCmdCeUpdatePceLceMappings__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *);  // halified (singleton optimized) exported (id=0x20802a05)
    NV_STATUS (*__subdeviceCtrlCmdCeUpdatePceLceMappingsV2__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS *);  // halified (singleton optimized) exported (id=0x20802a0d)
    NV_STATUS (*__subdeviceCtrlCmdCeGetLceShimInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS *);  // exported (id=0x20802a0c)
    NV_STATUS (*__subdeviceCtrlCmdCeGetPceConfigForLceType__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS *);  // exported (id=0x20802a0f)
    NV_STATUS (*__subdeviceCtrlCmdCeGetCapsV2__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x20802a03)
    NV_STATUS (*__subdeviceCtrlCmdCeGetAllCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x20802a0a)
    NV_STATUS (*__subdeviceCtrlCmdCeGetDecompLceMask__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS *);  // exported (id=0x20802a11)
    NV_STATUS (*__subdeviceCtrlCmdCeIsDecompLceEnabled__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS *);  // exported (id=0x20802a12)
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetDmemUsage__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *);  // exported (id=0x20803101)
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetEngineArch__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *);  // exported (id=0x20803118)
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerQueueInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *);  // exported (id=0x20803120)
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerControlGet__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *);  // exported (id=0x20803122)
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerControlSet__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *);  // exported (id=0x20803123)
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetCtxBufferInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *);  // exported (id=0x20803124)
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetCtxBufferSize__)(struct Subdevice * /*this*/, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *);  // exported (id=0x20803125)
    NV_STATUS (*__subdeviceCtrlCmdEccGetClientExposedCounters__)(struct Subdevice * /*this*/, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *);  // exported (id=0x20803400)
    NV_STATUS (*__subdeviceCtrlCmdEccGetVolatileCounts__)(struct Subdevice * /*this*/, NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS *);  // exported (id=0x20803401)
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20800133) body
    NV_STATUS (*__subdeviceCtrlCmdGpuSetEccConfiguration__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *);  // halified (2 hals) exported (id=0x20800134) body
    NV_STATUS (*__subdeviceCtrlCmdGpuResetEccErrorStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *);  // halified (2 hals) exported (id=0x20800136) body
    NV_STATUS (*__subdeviceCtrlCmdFlaRange__)(struct Subdevice * /*this*/, NV2080_CTRL_FLA_RANGE_PARAMS *);  // exported (id=0x20803501)
    NV_STATUS (*__subdeviceCtrlCmdFlaSetupInstanceMemBlock__)(struct Subdevice * /*this*/, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *);  // exported (id=0x20803502)
    NV_STATUS (*__subdeviceCtrlCmdFlaGetRange__)(struct Subdevice * /*this*/, NV2080_CTRL_FLA_GET_RANGE_PARAMS *);  // exported (id=0x20803503)
    NV_STATUS (*__subdeviceCtrlCmdFlaGetFabricMemStats__)(struct Subdevice * /*this*/, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *);  // exported (id=0x20803504)
    NV_STATUS (*__subdeviceCtrlCmdGspGetFeatures__)(struct Subdevice * /*this*/, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *);  // halified (2 hals) exported (id=0x20803601) body
    NV_STATUS (*__subdeviceCtrlCmdGspGetRmHeapStats__)(struct Subdevice * /*this*/, NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS *);  // exported (id=0x20803602)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVgpuHeapStats__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *);  // exported (id=0x20803603)
    NV_STATUS (*__subdeviceCtrlCmdLibosGetHeapStats__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *);  // exported (id=0x20803604)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetActivePartitionIds__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *);  // exported (id=0x2080018b)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPartitionCapacity__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *);  // exported (id=0x20800181)
    NV_STATUS (*__subdeviceCtrlCmdGpuDescribePartitions__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *);  // exported (id=0x20800185)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPartitioningMode__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *);  // exported (id=0x20800183)
    NV_STATUS (*__subdeviceCtrlCmdGrmgrGetGrFsInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *);  // exported (id=0x20803801)
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPartitions__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *);  // exported (id=0x20800174)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPartitions__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *);  // exported (id=0x20800175)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetComputeProfiles__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *);  // exported (id=0x208001a2)
    NV_STATUS (*__subdeviceCtrlCmdGpuGetComputeProfileCapacity__)(struct Subdevice * /*this*/, NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS *);  // exported (id=0x208001e5)
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *);  // exported (id=0x20800a63)
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *);  // exported (id=0x20800a65)
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *);  // exported (id=0x20800a66)
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *);  // exported (id=0x20800aba)
    NV_STATUS (*__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *);  // exported (id=0x20800aa7)
    NV_STATUS (*__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *);  // exported (id=0x20800aa9)
    NV_STATUS (*__subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS *);  // exported (id=0x20800a44)
    NV_STATUS (*__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__)(struct Subdevice * /*this*/, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *);  // exported (id=0x20803d01)
    NV_STATUS (*__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__)(struct Subdevice * /*this*/, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *);  // exported (id=0x20803d02)
    NV_STATUS (*__subdeviceCtrlCmdOsUnixAudioDynamicPower__)(struct Subdevice * /*this*/, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *);  // exported (id=0x20803d03)
    NV_STATUS (*__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *);  // exported (id=0x20803d07)
    NV_STATUS (*__subdeviceCtrlCmdOsUnixUpdateTgpStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *);  // exported (id=0x20803d08)
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetIpVersion__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *);  // exported (id=0x20800a4b)
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetStaticInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *);  // exported (id=0x20800a01)
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetChannelPushbuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *);  // exported (id=0x20800a58)
    NV_STATUS (*__subdeviceCtrlCmdDisplayWriteInstMem__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *);  // exported (id=0x20800a49)
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetupRgLineIntr__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *);  // exported (id=0x20800a4d)
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetImportedImpData__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *);  // exported (id=0x20800a54)
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetDisplayMask__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *);  // exported (id=0x20800a5d)
    NV_STATUS (*__subdeviceCtrlCmdDisplayPinsetsToLockpins__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS *);  // exported (id=0x20800adc)
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS *);  // exported (id=0x20800ade)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpioProgramDirection__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS *);  // exported (id=0x20802300)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpioProgramOutput__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS *);  // exported (id=0x20802301)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpioReadInput__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS *);  // exported (id=0x20802302)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpioActivateHwFunction__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS *);  // exported (id=0x20802303)
    NV_STATUS (*__subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__)(struct Subdevice * /*this*/);  // exported (id=0x20800af0)
    NV_STATUS (*__subdeviceCtrlCmdInternalDisplayPreModeSet__)(struct Subdevice * /*this*/);  // exported (id=0x20800af1)
    NV_STATUS (*__subdeviceCtrlCmdInternalDisplayPostModeSet__)(struct Subdevice * /*this*/);  // exported (id=0x20800af2)
    NV_STATUS (*__subdeviceCtrlCmdDisplayPreUnixConsole__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS *);  // exported (id=0x20800a76)
    NV_STATUS (*__subdeviceCtrlCmdDisplayPostUnixConsole__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS *);  // exported (id=0x20800a77)
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *);  // exported (id=0x20800a1d)
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *);  // exported (id=0x20800a1e)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetChipInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *);  // exported (id=0x20800a36)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *);  // exported (id=0x20800a41)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetDeviceInfoTable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *);  // exported (id=0x20800a40)
    NV_STATUS (*__subdeviceCtrlCmdInternalRecoverAllComputeContexts__)(struct Subdevice * /*this*/);  // exported (id=0x20800a4a)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetSmcMode__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *);  // exported (id=0x20800a4c)
    NV_STATUS (*__subdeviceCtrlCmdIsEgpuBridge__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS *);  // exported (id=0x20800a55)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS *);  // exported (id=0x20800aeb)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusFlushWithSysmembar__)(struct Subdevice * /*this*/);  // exported (id=0x20800a70)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *);  // exported (id=0x20800a71)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *);  // exported (id=0x20800a72)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *);  // exported (id=0x20800a73)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *);  // exported (id=0x20800a74)
    NV_STATUS (*__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *);  // exported (id=0x20800a75)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpuGetPFBar1Spa__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS *);  // exported (id=0x20800aee)
    NV_STATUS (*__subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *);  // exported (id=0x20800a57)
    NV_STATUS (*__subdeviceCtrlCmdGmmuGetStaticInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *);  // exported (id=0x20800a59)
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *);  // exported (id=0x20800a9b)
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__)(struct Subdevice * /*this*/);  // exported (id=0x20800a9c)
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *);  // exported (id=0x20800a9d)
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *);  // exported (id=0x20800a9e)
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS *);  // exported (id=0x20800a9f)
    NV_STATUS (*__subdeviceCtrlCmdCeGetPhysicalCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *);  // exported (id=0x20802a07)
    NV_STATUS (*__subdeviceCtrlCmdCeGetAllPhysicalCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *);  // exported (id=0x20802a0b)
    NV_STATUS (*__subdeviceCtrlCmdCeUpdateClassDB__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *);  // exported (id=0x20802a06)
    NV_STATUS (*__subdeviceCtrlCmdCeGetFaultMethodBufferSize__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *);  // exported (id=0x20802a08)
    NV_STATUS (*__subdeviceCtrlCmdCeGetHubPceMask__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *);  // exported (id=0x20802a09)
    NV_STATUS (*__subdeviceCtrlCmdCeGetHubPceMaskV2__)(struct Subdevice * /*this*/, NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS *);  // exported (id=0x20802a0e)
    NV_STATUS (*__subdeviceCtrlCmdIntrGetKernelTable__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *);  // exported (id=0x20800a5c)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostSet_2x__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *);  // exported (id=0x20800a9a)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostSet_3x__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *);  // exported (id=0x20800aa0)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostClear_3x__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *);  // exported (id=0x20800aa1)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *);  // exported (id=0x20800a7e)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *);  // exported (id=0x20800a80)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *);  // exported (id=0x20800a7f)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *);  // exported (id=0x20800a98)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *);  // exported (id=0x20800a99)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *);  // exported (id=0x20800ab1)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfGetAuxPowerState__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS *);  // exported (id=0x20800a81)
    NV_STATUS (*__subdeviceCtrlCmdInternalGcxEntryPrerequisite__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS *);  // exported (id=0x2080a7d7)
    NV_STATUS (*__subdeviceCtrlCmdBifGetStaticInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800aac) body
    NV_STATUS (*__subdeviceCtrlCmdBifGetAspmL1Flags__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *);  // exported (id=0x20800ab0)
    NV_STATUS (*__subdeviceCtrlCmdBifSetPcieRo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *);  // exported (id=0x20800ab9)
    NV_STATUS (*__subdeviceCtrlCmdBifDisableSystemMemoryAccess__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS *);  // exported (id=0x20800adb)
    NV_STATUS (*__subdeviceCtrlCmdHshubPeerConnConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *);  // exported (id=0x20800a88)
    NV_STATUS (*__subdeviceCtrlCmdHshubGetHshubIdForLinks__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *);  // exported (id=0x20800a8a)
    NV_STATUS (*__subdeviceCtrlCmdHshubGetNumUnits__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *);  // exported (id=0x20800a8b)
    NV_STATUS (*__subdeviceCtrlCmdHshubNextHshubId__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *);  // exported (id=0x20800a8c)
    NV_STATUS (*__subdeviceCtrlCmdHshubGetMaxHshubsPerShim__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS *);  // exported (id=0x20800a79)
    NV_STATUS (*__subdeviceCtrlCmdHshubEgmConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS *);  // exported (id=0x20800a8d)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__)(struct Subdevice * /*this*/);  // exported (id=0x20800aad)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *);  // exported (id=0x20800aae)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS *);  // exported (id=0x20800ac7)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS *);  // exported (id=0x20800ac8)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS *);  // exported (id=0x20800aea)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *);  // exported (id=0x20800a21)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkCoreCallback__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS *);  // exported (id=0x20800a24)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *);  // exported (id=0x20800a25)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetAliEnabled__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS *);  // exported (id=0x20800a29)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *);  // exported (id=0x20800a62)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkProgramBufferready__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *);  // exported (id=0x20800a64)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors__)(struct Subdevice * /*this*/);  // exported (id=0x20800b01)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *);  // exported (id=0x20800a78)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *);  // exported (id=0x20800a7d)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *);  // exported (id=0x20800a82)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *);  // exported (id=0x20800a4e)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *);  // exported (id=0x20800a50)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkUpdateHshubMux__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *);  // exported (id=0x20800a42)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *);  // exported (id=0x20800a5f)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *);  // exported (id=0x20800a83)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS *);  // exported (id=0x20800a84)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *);  // exported (id=0x20800a85)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkLinkTrainAli__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS *);  // exported (id=0x20800a86)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *);  // exported (id=0x20800a87)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *);  // exported (id=0x20800a8e)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *);  // exported (id=0x20800a8f)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkAreLinksTrained__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS *);  // exported (id=0x20800a90)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkResetLinks__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS *);  // exported (id=0x20800a91)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *);  // exported (id=0x20800a92)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *);  // exported (id=0x20800a93)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *);  // exported (id=0x20800a94)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *);  // exported (id=0x20800a95)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *);  // exported (id=0x20800a96)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *);  // exported (id=0x20800a97)
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *);  // exported (id=0x20800aab)
    NV_STATUS (*__subdeviceCtrlCmdInternalSetP2pCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *);  // exported (id=0x20800ab5)
    NV_STATUS (*__subdeviceCtrlCmdInternalRemoveP2pCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS *);  // exported (id=0x20800ab6)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetPcieP2pCaps__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *);  // exported (id=0x20800ab8)
    NV_STATUS (*__subdeviceCtrlCmdInternalGetLocalAtsConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800afb) body
    NV_STATUS (*__subdeviceCtrlCmdInternalSetPeerAtsConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *);  // halified (2 hals) exported (id=0x20800afc) body
    NV_STATUS (*__subdeviceCtrlCmdInternalInitGpuIntr__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS *);  // exported (id=0x20800abe)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncOptimizeTiming__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS *);  // exported (id=0x20800abf)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncGetDisplayIds__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS *);  // exported (id=0x20800ac0)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncSetStereoSync__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS *);  // exported (id=0x20800ac1)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncGetVactiveLines__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS *);  // exported (id=0x20800ac4)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS *);  // exported (id=0x20800ac9)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS *);  // exported (id=0x20800aca)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS *);  // exported (id=0x20800aed)
    NV_STATUS (*__subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS *);  // exported (id=0x20800a14)
    NV_STATUS (*__subdeviceCtrlCmdInternalFbsrInit__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS *);  // exported (id=0x20800ac2)
    NV_STATUS (*__subdeviceCtrlCmdInternalPostInitBrightcStateLoad__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *);  // exported (id=0x20800ac6)
    NV_STATUS (*__subdeviceCtrlCmdInternalSetStaticEdidData__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *);  // exported (id=0x20800adf)
    NV_STATUS (*__subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS *);  // exported (id=0x20800acb)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *);  // exported (id=0x20800acc)
    NV_STATUS (*__subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *);  // exported (id=0x20800acd)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS *);  // exported (id=0x20800ace)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS *);  // exported (id=0x20800acf)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS *);  // exported (id=0x20800ad0)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS *);  // exported (id=0x20800afd)
    NV_STATUS (*__subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS *);  // exported (id=0x20800ad1)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS *);  // exported (id=0x20800ad2)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS *);  // exported (id=0x20800ad3)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS *);  // exported (id=0x20800ad4)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS *);  // exported (id=0x20800ad5)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS *);  // exported (id=0x20800ad6)
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck__)(struct Subdevice * /*this*/);  // exported (id=0x20800a7a)
    NV_STATUS (*__subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__)(struct Subdevice * /*this*/);  // exported (id=0x20800a7b)
    NV_STATUS (*__subdeviceCtrlCmdInternalDetectHsVideoBridge__)(struct Subdevice * /*this*/);  // exported (id=0x20800add)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeGetStaticInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS *);  // exported (id=0x20800af3)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS *);  // exported (id=0x20800ae1)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS *);  // exported (id=0x20800ae2)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeRotateKeys__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS *);  // exported (id=0x20800ae5)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS *);  // exported (id=0x20800ae6)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeSetGpuState__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS *);  // exported (id=0x20800ae7)
    NV_STATUS (*__subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS *);  // exported (id=0x20800ae8)
    NV_STATUS (*__subdeviceCtrlCmdInternalInitUserSharedData__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS *);  // exported (id=0x20800afe)
    NV_STATUS (*__subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS *);  // exported (id=0x20800aff)
    NV_STATUS (*__subdeviceCtrlCmdInternalControlGspTrace__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *);  // exported (id=0x208001e3)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS *);  // exported (id=0x20800ae9)
    NV_STATUS (*__subdeviceCtrlCmdInternalGpuSetIllum__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS *);  // exported (id=0x20800aec)
    NV_STATUS (*__subdeviceCtrlCmdInternalLogOobXid__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS *);  // exported (id=0x20800a56)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *);  // exported (id=0x20804001)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS *);  // exported (id=0x20804002)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *);  // exported (id=0x20804003)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *);  // exported (id=0x20804004)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS *);  // exported (id=0x20804005)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS *);  // exported (id=0x20804006)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS *);  // exported (id=0x20804007)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS *);  // exported (id=0x20804008)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS *);  // exported (id=0x20804009)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS *);  // exported (id=0x2080400a)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS *);  // exported (id=0x2080400b)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalFreeStates__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS *);  // exported (id=0x2080400c)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS *);  // exported (id=0x2080400d)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *);  // exported (id=0x2080400e)
    NV_STATUS (*__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode__)(struct Subdevice * /*this*/, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS *);  // exported (id=0x2080400f)
    NV_STATUS (*__subdeviceCtrlCmdGetAvailableHshubMask__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *);  // exported (id=0x20804101)
    NV_STATUS (*__subdeviceCtrlSetEcThrottleMode__)(struct Subdevice * /*this*/, NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS *);  // exported (id=0x20804102)
    NV_STATUS (*__subdeviceCtrlCmdCcuMap__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *);  // exported (id=0x20800ab3)
    NV_STATUS (*__subdeviceCtrlCmdCcuUnmap__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *);  // exported (id=0x20800ab4)
    NV_STATUS (*__subdeviceCtrlCmdCcuSetStreamState__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *);  // exported (id=0x20800abd)
    NV_STATUS (*__subdeviceCtrlCmdCcuGetSampleInfo__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *);  // halified (2 hals) exported (id=0x20800ab2) body
    NV_STATUS (*__subdeviceCtrlCmdSpdmPartition__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *);  // exported (id=0x20800ad9)
    NV_STATUS (*__subdeviceSpdmRetrieveTranscript__)(struct Subdevice * /*this*/, NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *);  // exported (id=0x20800ada)

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


// Wrapper macros
#define subdevicePreDestruct_FNPTR(pResource) pResource->__nvoc_metadata_ptr->vtable.__subdevicePreDestruct__
#define subdevicePreDestruct(pResource) subdevicePreDestruct_DISPATCH(pResource)
#define subdeviceInternalControlForward_FNPTR(pSubdevice) pSubdevice->__nvoc_metadata_ptr->vtable.__subdeviceInternalControlForward__
#define subdeviceInternalControlForward(pSubdevice, command, pParams, size) subdeviceInternalControlForward_DISPATCH(pSubdevice, command, pParams, size)
#define subdeviceCtrlCmdBiosGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetInfoV2__
#define subdeviceCtrlCmdBiosGetInfoV2(pSubdevice, pBiosInfoParams) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams)
#define subdeviceCtrlCmdBiosGetInfoV2_HAL(pSubdevice, pBiosInfoParams) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams)
#define subdeviceCtrlCmdBiosGetNbsiV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetNbsiV2__
#define subdeviceCtrlCmdBiosGetNbsiV2(pSubdevice, pNbsiParams) subdeviceCtrlCmdBiosGetNbsiV2_DISPATCH(pSubdevice, pNbsiParams)
#define subdeviceCtrlCmdBiosGetSKUInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetSKUInfo__
#define subdeviceCtrlCmdBiosGetSKUInfo(pSubdevice, pBiosGetSKUInfoParams) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams)
#define subdeviceCtrlCmdBiosGetSKUInfo_HAL(pSubdevice, pBiosGetSKUInfoParams) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams)
#define subdeviceCtrlCmdBiosGetPostTime_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetPostTime__
#define subdeviceCtrlCmdBiosGetPostTime(pSubdevice, pBiosPostTime) subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime)
#define subdeviceCtrlCmdBiosGetPostTime_HAL(pSubdevice, pBiosPostTime) subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime)
#define subdeviceCtrlCmdBiosGetUefiSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBiosGetUefiSupport__
#define subdeviceCtrlCmdBiosGetUefiSupport(pSubdevice, pUEFIParams) subdeviceCtrlCmdBiosGetUefiSupport_DISPATCH(pSubdevice, pUEFIParams)
#define subdeviceCtrlCmdMcGetArchInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcGetArchInfo__
#define subdeviceCtrlCmdMcGetArchInfo(pSubdevice, pArchInfoParams) subdeviceCtrlCmdMcGetArchInfo_DISPATCH(pSubdevice, pArchInfoParams)
#define subdeviceCtrlCmdMcGetManufacturer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcGetManufacturer__
#define subdeviceCtrlCmdMcGetManufacturer(pSubdevice, pManufacturerParams) subdeviceCtrlCmdMcGetManufacturer_DISPATCH(pSubdevice, pManufacturerParams)
#define subdeviceCtrlCmdMcChangeReplayableFaultOwnership_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__
#define subdeviceCtrlCmdMcChangeReplayableFaultOwnership(pSubdevice, pReplayableFaultOwnrshpParams) subdeviceCtrlCmdMcChangeReplayableFaultOwnership_DISPATCH(pSubdevice, pReplayableFaultOwnrshpParams)
#define subdeviceCtrlCmdMcServiceInterrupts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcServiceInterrupts__
#define subdeviceCtrlCmdMcServiceInterrupts(pSubdevice, pServiceInterruptParams) subdeviceCtrlCmdMcServiceInterrupts_DISPATCH(pSubdevice, pServiceInterruptParams)
#define subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__
#define subdeviceCtrlCmdMcGetEngineNotificationIntrVectors(pSubdevice, pParams) subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMcGetStaticIntrTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMcGetStaticIntrTable__
#define subdeviceCtrlCmdMcGetStaticIntrTable(pSubdevice, pParams) subdeviceCtrlCmdMcGetStaticIntrTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDmaInvalidateTLB_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDmaInvalidateTLB__
#define subdeviceCtrlCmdDmaInvalidateTLB(pSubdevice, pParams) subdeviceCtrlCmdDmaInvalidateTLB_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDmaGetInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDmaGetInfo__
#define subdeviceCtrlCmdDmaGetInfo(pSubdevice, pDmaInfoParams) subdeviceCtrlCmdDmaGetInfo_DISPATCH(pSubdevice, pDmaInfoParams)
#define subdeviceCtrlCmdBusGetPciInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPciInfo__
#define subdeviceCtrlCmdBusGetPciInfo(pSubdevice, pPciInfoParams) subdeviceCtrlCmdBusGetPciInfo_DISPATCH(pSubdevice, pPciInfoParams)
#define subdeviceCtrlCmdBusGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetInfoV2__
#define subdeviceCtrlCmdBusGetInfoV2(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusGetInfoV2_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusGetPciBarInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPciBarInfo__
#define subdeviceCtrlCmdBusGetPciBarInfo(pSubdevice, pBarInfoParams) subdeviceCtrlCmdBusGetPciBarInfo_DISPATCH(pSubdevice, pBarInfoParams)
#define subdeviceCtrlCmdBusSetPcieSpeed_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetPcieSpeed__
#define subdeviceCtrlCmdBusSetPcieSpeed(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusSetPcieSpeed_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusSetPcieLinkWidth_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetPcieLinkWidth__
#define subdeviceCtrlCmdBusSetPcieLinkWidth(pSubdevice, pLinkWidthParams) subdeviceCtrlCmdBusSetPcieLinkWidth_DISPATCH(pSubdevice, pLinkWidthParams)
#define subdeviceCtrlCmdBusServiceGpuMultifunctionState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusServiceGpuMultifunctionState__
#define subdeviceCtrlCmdBusServiceGpuMultifunctionState(pSubdevice, pParams) subdeviceCtrlCmdBusServiceGpuMultifunctionState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPexCounters__
#define subdeviceCtrlCmdBusGetPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetBFD_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetBFD__
#define subdeviceCtrlCmdBusGetBFD(pSubdevice, pBusGetBFDParams) subdeviceCtrlCmdBusGetBFD_DISPATCH(pSubdevice, pBusGetBFDParams)
#define subdeviceCtrlCmdBusGetAspmDisableFlags_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetAspmDisableFlags__
#define subdeviceCtrlCmdBusGetAspmDisableFlags(pSubdevice, pParams) subdeviceCtrlCmdBusGetAspmDisableFlags_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusControlPublicAspmBits_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusControlPublicAspmBits__
#define subdeviceCtrlCmdBusControlPublicAspmBits(pSubdevice, pParams) subdeviceCtrlCmdBusControlPublicAspmBits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusClearPexCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusClearPexCounters__
#define subdeviceCtrlCmdBusClearPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexUtilCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPexUtilCounters__
#define subdeviceCtrlCmdBusGetPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexUtilCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusClearPexUtilCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusClearPexUtilCounters__
#define subdeviceCtrlCmdBusClearPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexUtilCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusFreezePexCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusFreezePexCounters__
#define subdeviceCtrlCmdBusFreezePexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusFreezePexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexLaneCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPexLaneCounters__
#define subdeviceCtrlCmdBusGetPexLaneCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexLaneCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieLtrLatency_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetPcieLtrLatency__
#define subdeviceCtrlCmdBusGetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieLtrLatency_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetPcieLtrLatency_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetPcieLtrLatency__
#define subdeviceCtrlCmdBusSetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusSetPcieLtrLatency_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkPeerIdMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__
#define subdeviceCtrlCmdBusGetNvlinkPeerIdMask(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkPeerIdMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetEomParameters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetEomParameters__
#define subdeviceCtrlCmdBusSetEomParameters(pSubdevice, pParams) subdeviceCtrlCmdBusSetEomParameters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetUphyDlnCfgSpace_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__
#define subdeviceCtrlCmdBusGetUphyDlnCfgSpace(pSubdevice, pParams) subdeviceCtrlCmdBusGetUphyDlnCfgSpace_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetEomStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetEomStatus__
#define subdeviceCtrlCmdBusGetEomStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetEomStatus_DISPATCH(pSubdevice, pParams)
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
#define subdeviceCtrlCmdBusGetC2CErrorInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetC2CErrorInfo__
#define subdeviceCtrlCmdBusGetC2CErrorInfo(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CErrorInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CLpwrStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetC2CLpwrStats__
#define subdeviceCtrlCmdBusGetC2CLpwrStats(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetC2CLpwrStats_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote(pSubdevice, pParams) subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetC2CLpwrStateVote_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSysmemAccess_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSysmemAccess__
#define subdeviceCtrlCmdBusSysmemAccess(pSubdevice, pParams) subdeviceCtrlCmdBusSysmemAccess_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetP2pMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusSetP2pMapping__
#define subdeviceCtrlCmdBusSetP2pMapping(pSubdevice, pParams) subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetP2pMapping_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusUnsetP2pMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusUnsetP2pMapping__
#define subdeviceCtrlCmdBusUnsetP2pMapping(pSubdevice, pParams) subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusUnsetP2pMapping_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetSupportedCounters_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetSupportedCounters__
#define subdeviceCtrlCmdNvlinkGetSupportedCounters(arg_this, arg2) subdeviceCtrlCmdNvlinkGetSupportedCounters_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetSupportedBWMode_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetSupportedBWMode__
#define subdeviceCtrlCmdNvlinkGetSupportedBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkGetSupportedBWMode_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetBWMode_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetBWMode__
#define subdeviceCtrlCmdNvlinkGetBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkGetBWMode_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkSetBWMode_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkSetBWMode__
#define subdeviceCtrlCmdNvlinkSetBWMode(arg_this, arg2) subdeviceCtrlCmdNvlinkSetBWMode_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetLocalDeviceInfo__
#define subdeviceCtrlCmdNvlinkGetLocalDeviceInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdGetNvlinkCountersV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetNvlinkCountersV2__
#define subdeviceCtrlCmdGetNvlinkCountersV2(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkCountersV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdClearNvlinkCountersV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdClearNvlinkCountersV2__
#define subdeviceCtrlCmdClearNvlinkCountersV2(pSubdevice, pParams) subdeviceCtrlCmdClearNvlinkCountersV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetNvlinkCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetNvlinkCounters__
#define subdeviceCtrlCmdGetNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdClearNvlinkCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdClearNvlinkCounters__
#define subdeviceCtrlCmdClearNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdClearNvlinkCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetNvlinkCaps__
#define subdeviceCtrlCmdBusGetNvlinkCaps(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetNvlinkStatus__
#define subdeviceCtrlCmdBusGetNvlinkStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkErrInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBusGetNvlinkErrInfo__
#define subdeviceCtrlCmdBusGetNvlinkErrInfo(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkErrInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__
#define subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings(pSubdevice, pParams) subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__
#define subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkInjectSWError_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkInjectSWError__
#define subdeviceCtrlCmdNvlinkInjectSWError(pSubdevice, pParams) subdeviceCtrlCmdNvlinkInjectSWError_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkConfigureL1Toggle_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkConfigureL1Toggle__
#define subdeviceCtrlCmdNvlinkConfigureL1Toggle(pSubdevice, pParams) subdeviceCtrlCmdNvlinkConfigureL1Toggle_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlNvlinkGetL1Toggle_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlNvlinkGetL1Toggle__
#define subdeviceCtrlNvlinkGetL1Toggle(pSubdevice, pParams) subdeviceCtrlNvlinkGetL1Toggle_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinGetLinkFomValues_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinGetLinkFomValues__
#define subdeviceCtrlCmdNvlinGetLinkFomValues(pSubdevice, pParams) subdeviceCtrlCmdNvlinGetLinkFomValues_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__
#define subdeviceCtrlCmdNvlinkGetNvlinkEccErrors(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__
#define subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetupEom_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkSetupEom__
#define subdeviceCtrlCmdNvlinkSetupEom(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetupEom_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetPowerState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetPowerState__
#define subdeviceCtrlCmdNvlinkGetPowerState(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetPowerState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkReadTpCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkReadTpCounters__
#define subdeviceCtrlCmdNvlinkReadTpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkReadTpCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLpCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetLpCounters__
#define subdeviceCtrlCmdNvlinkGetLpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLpCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkClearLpCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkClearLpCounters__
#define subdeviceCtrlCmdNvlinkClearLpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkClearLpCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetLoopbackMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkSetLoopbackMode__
#define subdeviceCtrlCmdNvlinkSetLoopbackMode(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetLoopbackMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetRefreshCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetRefreshCounters__
#define subdeviceCtrlCmdNvlinkGetRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetRefreshCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkClearRefreshCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkClearRefreshCounters__
#define subdeviceCtrlCmdNvlinkClearRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkClearRefreshCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__
#define subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__
#define subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkEnableLinks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkEnableLinks__
#define subdeviceCtrlCmdNvlinkEnableLinks(pSubdevice) subdeviceCtrlCmdNvlinkEnableLinks_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__
#define subdeviceCtrlCmdNvlinkProcessInitDisabledLinks(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkInbandSendData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkInbandSendData__
#define subdeviceCtrlCmdNvlinkInbandSendData(pSubdevice, pParams) subdeviceCtrlCmdNvlinkInbandSendData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkPostFaultUp_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkPostFaultUp__
#define subdeviceCtrlCmdNvlinkPostFaultUp(pSubdevice, pParams) subdeviceCtrlCmdNvlinkPostFaultUp_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkPostLazyErrorRecovery__
#define subdeviceCtrlCmdNvlinkPostLazyErrorRecovery(pSubdevice) subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdNvlinkEomControl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkEomControl__
#define subdeviceCtrlCmdNvlinkEomControl(pSubdevice, pParams) subdeviceCtrlCmdNvlinkEomControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetL1Threshold_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkSetL1Threshold__
#define subdeviceCtrlCmdNvlinkSetL1Threshold(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetL1Threshold_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkDirectConnectCheck_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkDirectConnectCheck__
#define subdeviceCtrlCmdNvlinkDirectConnectCheck(pSubdevice, pParams) subdeviceCtrlCmdNvlinkDirectConnectCheck_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetL1Threshold_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetL1Threshold__
#define subdeviceCtrlCmdNvlinkGetL1Threshold(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetL1Threshold_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdIsNvlinkReducedConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdIsNvlinkReducedConfig__
#define subdeviceCtrlCmdIsNvlinkReducedConfig(pSubdevice, pParams) subdeviceCtrlCmdIsNvlinkReducedConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetPortEvents_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkGetPortEvents__
#define subdeviceCtrlCmdNvlinkGetPortEvents(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetPortEvents_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkIsGpuDegraded_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvlinkIsGpuDegraded__
#define subdeviceCtrlCmdNvlinkIsGpuDegraded(pSubdevice, pParams) subdeviceCtrlCmdNvlinkIsGpuDegraded_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkPRMAccessPAOS_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPAOS__
#define subdeviceCtrlCmdNvlinkPRMAccessPAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPAOS_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPLTC_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPLTC__
#define subdeviceCtrlCmdNvlinkPRMAccessPLTC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPLTC_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPLM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPLM__
#define subdeviceCtrlCmdNvlinkPRMAccessPPLM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPLM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLC_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPSLC__
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPSLC_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMCAM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMCAM__
#define subdeviceCtrlCmdNvlinkPRMAccessMCAM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMCAM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTECR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTECR__
#define subdeviceCtrlCmdNvlinkPRMAccessMTECR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTECR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTEWE_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTEWE__
#define subdeviceCtrlCmdNvlinkPRMAccessMTEWE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTEWE_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTSDE_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTSDE__
#define subdeviceCtrlCmdNvlinkPRMAccessMTSDE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTSDE_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTCAP_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTCAP__
#define subdeviceCtrlCmdNvlinkPRMAccessMTCAP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTCAP_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPMTU_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMTU__
#define subdeviceCtrlCmdNvlinkPRMAccessPMTU(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMTU_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPMLP_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMLP__
#define subdeviceCtrlCmdNvlinkPRMAccessPMLP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMLP_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessGHPKT_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessGHPKT__
#define subdeviceCtrlCmdNvlinkPRMAccessGHPKT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessGHPKT_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPDDR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPDDR__
#define subdeviceCtrlCmdNvlinkPRMAccessPDDR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPDDR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPTT_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPTT__
#define subdeviceCtrlCmdNvlinkPRMAccessPPTT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPTT_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPCNT_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPCNT__
#define subdeviceCtrlCmdNvlinkPRMAccessPPCNT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPCNT_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMGIR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMGIR__
#define subdeviceCtrlCmdNvlinkPRMAccessMGIR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMGIR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPAOS_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPAOS__
#define subdeviceCtrlCmdNvlinkPRMAccessPPAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPAOS_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPHCR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPHCR__
#define subdeviceCtrlCmdNvlinkPRMAccessPPHCR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPHCR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessSLTP_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLTP__
#define subdeviceCtrlCmdNvlinkPRMAccessSLTP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLTP_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPGUID_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPGUID__
#define subdeviceCtrlCmdNvlinkPRMAccessPGUID(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPGUID_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPRT_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPRT__
#define subdeviceCtrlCmdNvlinkPRMAccessPPRT(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPRT_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPTYS_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPTYS__
#define subdeviceCtrlCmdNvlinkPRMAccessPTYS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPTYS_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessSLRG_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLRG__
#define subdeviceCtrlCmdNvlinkPRMAccessSLRG(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLRG_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPMAOS_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMAOS__
#define subdeviceCtrlCmdNvlinkPRMAccessPMAOS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPMAOS_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPLR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPLR__
#define subdeviceCtrlCmdNvlinkPRMAccessPPLR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPLR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMORD_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMORD__
#define subdeviceCtrlCmdNvlinkPRMAccessMORD(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMORD_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP__
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF__
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL__
#define subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTEIM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTEIM__
#define subdeviceCtrlCmdNvlinkPRMAccessMTEIM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTEIM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTIE_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTIE__
#define subdeviceCtrlCmdNvlinkPRMAccessMTIE(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTIE_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTIM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTIM__
#define subdeviceCtrlCmdNvlinkPRMAccessMTIM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTIM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMPSCR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMPSCR__
#define subdeviceCtrlCmdNvlinkPRMAccessMPSCR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMPSCR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMTSR_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTSR__
#define subdeviceCtrlCmdNvlinkPRMAccessMTSR(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMTSR_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLS_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPSLS__
#define subdeviceCtrlCmdNvlinkPRMAccessPPSLS(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPSLS_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessMLPC_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMLPC__
#define subdeviceCtrlCmdNvlinkPRMAccessMLPC(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessMLPC_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPLIB_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPLIB__
#define subdeviceCtrlCmdNvlinkPRMAccessPLIB(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPLIB_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkUpdateNvleTopology_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkUpdateNvleTopology__
#define subdeviceCtrlCmdNvlinkUpdateNvleTopology(arg_this, arg2) subdeviceCtrlCmdNvlinkUpdateNvleTopology_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetNvleLids_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetNvleLids__
#define subdeviceCtrlCmdNvlinkGetNvleLids(arg_this, arg2) subdeviceCtrlCmdNvlinkGetNvleLids_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPTASV2_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPTASV2__
#define subdeviceCtrlCmdNvlinkPRMAccessPTASV2(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPTASV2_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM__
#define subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccessPPRM_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPRM__
#define subdeviceCtrlCmdNvlinkPRMAccessPPRM(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccessPPRM_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkPRMAccess_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkPRMAccess__
#define subdeviceCtrlCmdNvlinkPRMAccess(arg_this, arg2) subdeviceCtrlCmdNvlinkPRMAccess_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetPlatformInfo_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetPlatformInfo__
#define subdeviceCtrlCmdNvlinkGetPlatformInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetPlatformInfo_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_FNPTR(arg_this) arg_this->__subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo__
#define subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo(arg_this, arg2) subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_DISPATCH(arg_this, arg2)
#define subdeviceCtrlCmdI2cReadBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdI2cReadBuffer__
#define subdeviceCtrlCmdI2cReadBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadBuffer_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cWriteBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdI2cWriteBuffer__
#define subdeviceCtrlCmdI2cWriteBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteBuffer_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cReadReg_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdI2cReadReg__
#define subdeviceCtrlCmdI2cReadReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadReg_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cWriteReg_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdI2cWriteReg__
#define subdeviceCtrlCmdI2cWriteReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteReg_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdThermalSystemExecuteV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdThermalSystemExecuteV2__
#define subdeviceCtrlCmdThermalSystemExecuteV2(pSubdevice, pSystemExecuteParams) subdeviceCtrlCmdThermalSystemExecuteV2_DISPATCH(pSubdevice, pSystemExecuteParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_HAL(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfRatedTdpGetControl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfRatedTdpGetControl__
#define subdeviceCtrlCmdPerfRatedTdpGetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpGetControl_DISPATCH(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfRatedTdpSetControl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfRatedTdpSetControl__
#define subdeviceCtrlCmdPerfRatedTdpSetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfRatedTdpSetControl_HAL(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfReservePerfmonHw_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfReservePerfmonHw__
#define subdeviceCtrlCmdPerfReservePerfmonHw(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams)
#define subdeviceCtrlCmdPerfReservePerfmonHw_HAL(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams)
#define subdeviceCtrlCmdPerfSetAuxPowerState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfSetAuxPowerState__
#define subdeviceCtrlCmdPerfSetAuxPowerState(pSubdevice, pPowerStateParams) subdeviceCtrlCmdPerfSetAuxPowerState_DISPATCH(pSubdevice, pPowerStateParams)
#define subdeviceCtrlCmdPerfSetAuxPowerState_HAL(pSubdevice, pPowerStateParams) subdeviceCtrlCmdPerfSetAuxPowerState_DISPATCH(pSubdevice, pPowerStateParams)
#define subdeviceCtrlCmdPerfSetPowerstate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPerfSetPowerstate__
#define subdeviceCtrlCmdPerfSetPowerstate(pSubdevice, pPowerInfoParams) subdeviceCtrlCmdPerfSetPowerstate_DISPATCH(pSubdevice, pPowerInfoParams)
#define subdeviceCtrlCmdPerfSetPowerstate_HAL(pSubdevice, pPowerInfoParams) subdeviceCtrlCmdPerfSetPowerstate_DISPATCH(pSubdevice, pPowerInfoParams)
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
#define subdeviceCtrlCmdKPerfBoost_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKPerfBoost__
#define subdeviceCtrlCmdKPerfBoost(pSubdevice, pBoostParams) subdeviceCtrlCmdKPerfBoost_DISPATCH(pSubdevice, pBoostParams)
#define subdeviceCtrlCmdFbGetFBRegionInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetFBRegionInfo__
#define subdeviceCtrlCmdFbGetFBRegionInfo(pSubdevice, pGFBRIParams) subdeviceCtrlCmdFbGetFBRegionInfo_DISPATCH(pSubdevice, pGFBRIParams)
#define subdeviceCtrlCmdFbGetBar1Offset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetBar1Offset__
#define subdeviceCtrlCmdFbGetBar1Offset(pSubdevice, pFbMemParams) subdeviceCtrlCmdFbGetBar1Offset_DISPATCH(pSubdevice, pFbMemParams)
#define subdeviceCtrlCmdFbIsKind_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbIsKind__
#define subdeviceCtrlCmdFbIsKind(pSubdevice, pIsKindParams) subdeviceCtrlCmdFbIsKind_DISPATCH(pSubdevice, pIsKindParams)
#define subdeviceCtrlCmdFbGetMemAlignment_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetMemAlignment__
#define subdeviceCtrlCmdFbGetMemAlignment(pSubdevice, pParams) subdeviceCtrlCmdFbGetMemAlignment_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetHeapReservationSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetHeapReservationSize__
#define subdeviceCtrlCmdFbGetHeapReservationSize(pSubdevice, pParams) subdeviceCtrlCmdFbGetHeapReservationSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__
#define subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__
#define subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout(pSubdevice, pParams) subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__
#define subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetInfoV2__
#define subdeviceCtrlCmdFbGetInfoV2(pSubdevice, pFbInfoParams) subdeviceCtrlCmdFbGetInfoV2_DISPATCH(pSubdevice, pFbInfoParams)
#define subdeviceCtrlCmdFbGetCalibrationLockFailed_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetCalibrationLockFailed__
#define subdeviceCtrlCmdFbGetCalibrationLockFailed(pSubdevice, pGCLFParams) subdeviceCtrlCmdFbGetCalibrationLockFailed_DISPATCH(pSubdevice, pGCLFParams)
#define subdeviceCtrlCmdFbFlushGpuCache_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbFlushGpuCache__
#define subdeviceCtrlCmdFbFlushGpuCache(pSubdevice, pCacheFlushParams) subdeviceCtrlCmdFbFlushGpuCache_DISPATCH(pSubdevice, pCacheFlushParams)
#define subdeviceCtrlCmdFbGetGpuCacheInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetGpuCacheInfo__
#define subdeviceCtrlCmdFbGetGpuCacheInfo(pSubdevice, pGpuCacheParams) subdeviceCtrlCmdFbGetGpuCacheInfo_DISPATCH(pSubdevice, pGpuCacheParams)
#define subdeviceCtrlCmdFbGetCliManagedOfflinedPages_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__
#define subdeviceCtrlCmdFbGetCliManagedOfflinedPages(pSubdevice, pOsOfflinedParams) subdeviceCtrlCmdFbGetCliManagedOfflinedPages_DISPATCH(pSubdevice, pOsOfflinedParams)
#define subdeviceCtrlCmdFbGetOfflinedPages_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetOfflinedPages__
#define subdeviceCtrlCmdFbGetOfflinedPages(pSubdevice, pBlackListParams) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams)
#define subdeviceCtrlCmdFbGetOfflinedPages_HAL(pSubdevice, pBlackListParams) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams)
#define subdeviceCtrlCmdFbSetupVprRegion_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbSetupVprRegion__
#define subdeviceCtrlCmdFbSetupVprRegion(pSubdevice, pCliReqParams) subdeviceCtrlCmdFbSetupVprRegion_DISPATCH(pSubdevice, pCliReqParams)
#define subdeviceCtrlCmdFbGetLTCInfoForFBP_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetLTCInfoForFBP__
#define subdeviceCtrlCmdFbGetLTCInfoForFBP(pSubdevice, pParams) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetLTCInfoForFBP_HAL(pSubdevice, pParams) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__
#define subdeviceCtrlCmdFbGetCompBitCopyConstructInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbPatchPbrForMining_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbPatchPbrForMining__
#define subdeviceCtrlCmdFbPatchPbrForMining(pSubdevice, pParams) subdeviceCtrlCmdFbPatchPbrForMining_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetRemappedRows_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetRemappedRows__
#define subdeviceCtrlCmdFbGetRemappedRows(pSubdevice, pRemappedRowsParams) subdeviceCtrlCmdFbGetRemappedRows_DISPATCH(pSubdevice, pRemappedRowsParams)
#define subdeviceCtrlCmdFbGetFsInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetFsInfo__
#define subdeviceCtrlCmdFbGetFsInfo(pSubdevice, pInfoParams) subdeviceCtrlCmdFbGetFsInfo_DISPATCH(pSubdevice, pInfoParams)
#define subdeviceCtrlCmdFbGetRowRemapperHistogram_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetRowRemapperHistogram__
#define subdeviceCtrlCmdFbGetRowRemapperHistogram(pSubdevice, pParams) subdeviceCtrlCmdFbGetRowRemapperHistogram_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages(pSubdevice, pParams) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages_HAL(pSubdevice, pParams) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbUpdateNumaStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbUpdateNumaStatus__
#define subdeviceCtrlCmdFbUpdateNumaStatus(pSubdevice, pParams) subdeviceCtrlCmdFbUpdateNumaStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetNumaInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetNumaInfo__
#define subdeviceCtrlCmdFbGetNumaInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetNumaInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetStaticConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysGetStaticConfig__
#define subdeviceCtrlCmdMemSysGetStaticConfig(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetStaticConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysSetPartitionableMem_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysSetPartitionableMem__
#define subdeviceCtrlCmdMemSysSetPartitionableMem(pSubdevice, pParams) subdeviceCtrlCmdMemSysSetPartitionableMem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__
#define subdeviceCtrlCmdKMemSysGetMIGMemoryConfig(pSubdevice, pParams) subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetZbcReferenced_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbSetZbcReferenced__
#define subdeviceCtrlCmdFbSetZbcReferenced(pSubdevice, pParams) subdeviceCtrlCmdFbSetZbcReferenced_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysL2InvalidateEvict_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysL2InvalidateEvict__
#define subdeviceCtrlCmdMemSysL2InvalidateEvict(pSubdevice, pParams) subdeviceCtrlCmdMemSysL2InvalidateEvict_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__
#define subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches(pSubdevice) subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdMemSysDisableNvlinkPeers_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysDisableNvlinkPeers__
#define subdeviceCtrlCmdMemSysDisableNvlinkPeers(pSubdevice) subdeviceCtrlCmdMemSysDisableNvlinkPeers_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdMemSysProgramRawCompressionMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysProgramRawCompressionMode__
#define subdeviceCtrlCmdMemSysProgramRawCompressionMode(pSubdevice, pParams) subdeviceCtrlCmdMemSysProgramRawCompressionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_HAL(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetCtagsForCbcEviction_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetCtagsForCbcEviction__
#define subdeviceCtrlCmdFbGetCtagsForCbcEviction(pSubdevice, pParams) subdeviceCtrlCmdFbGetCtagsForCbcEviction_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbCBCOp_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbCBCOp__
#define subdeviceCtrlCmdFbCBCOp(pSubdevice, pParams) subdeviceCtrlCmdFbCBCOp_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetRrd_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbSetRrd__
#define subdeviceCtrlCmdFbSetRrd(pSubdevice, pParams) subdeviceCtrlCmdFbSetRrd_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetReadLimit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbSetReadLimit__
#define subdeviceCtrlCmdFbSetReadLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetReadLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetWriteLimit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbSetWriteLimit__
#define subdeviceCtrlCmdFbSetWriteLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetWriteLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGmmuCommitTlbInvalidate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGmmuCommitTlbInvalidate__
#define subdeviceCtrlCmdGmmuCommitTlbInvalidate(pSubdevice, pParams) subdeviceCtrlCmdGmmuCommitTlbInvalidate_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetStaticBar1Info_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFbGetStaticBar1Info__
#define subdeviceCtrlCmdFbGetStaticBar1Info(pSubdevice, pParams) subdeviceCtrlCmdFbGetStaticBar1Info_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport__
#define subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_HAL(pSubdevice, pConfig) subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdSetGpfifo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetGpfifo__
#define subdeviceCtrlCmdSetGpfifo(pSubdevice, pSetGpFifoParams) subdeviceCtrlCmdSetGpfifo_DISPATCH(pSubdevice, pSetGpFifoParams)
#define subdeviceCtrlCmdSetOperationalProperties_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetOperationalProperties__
#define subdeviceCtrlCmdSetOperationalProperties(pSubdevice, pSetOperationalProperties) subdeviceCtrlCmdSetOperationalProperties_DISPATCH(pSubdevice, pSetOperationalProperties)
#define subdeviceCtrlCmdFifoBindEngines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoBindEngines__
#define subdeviceCtrlCmdFifoBindEngines(pSubdevice, pBindParams) subdeviceCtrlCmdFifoBindEngines_DISPATCH(pSubdevice, pBindParams)
#define subdeviceCtrlCmdGetPhysicalChannelCount_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetPhysicalChannelCount__
#define subdeviceCtrlCmdGetPhysicalChannelCount(pSubdevice, pParams) subdeviceCtrlCmdGetPhysicalChannelCount_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetInfo__
#define subdeviceCtrlCmdFifoGetInfo(pSubdevice, pFifoInfoParams) subdeviceCtrlCmdFifoGetInfo_DISPATCH(pSubdevice, pFifoInfoParams)
#define subdeviceCtrlCmdFifoDisableChannels_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableChannels__
#define subdeviceCtrlCmdFifoDisableChannels(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannels_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_HAL(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_HAL(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableUsermodeChannels_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoDisableUsermodeChannels__
#define subdeviceCtrlCmdFifoDisableUsermodeChannels(pSubdevice, pParams) subdeviceCtrlCmdFifoDisableUsermodeChannels_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetChannelMemInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetChannelMemInfo__
#define subdeviceCtrlCmdFifoGetChannelMemInfo(pSubdevice, pChannelMemParams) subdeviceCtrlCmdFifoGetChannelMemInfo_DISPATCH(pSubdevice, pChannelMemParams)
#define subdeviceCtrlCmdFifoGetUserdLocation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetUserdLocation__
#define subdeviceCtrlCmdFifoGetUserdLocation(pSubdevice, pUserdLocationParams) subdeviceCtrlCmdFifoGetUserdLocation_DISPATCH(pSubdevice, pUserdLocationParams)
#define subdeviceCtrlCmdFifoObjschedSwGetLog_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoObjschedSwGetLog__
#define subdeviceCtrlCmdFifoObjschedSwGetLog(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedSwGetLog_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoObjschedGetState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoObjschedGetState__
#define subdeviceCtrlCmdFifoObjschedGetState(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoObjschedSetState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoObjschedSetState__
#define subdeviceCtrlCmdFifoObjschedSetState(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedSetState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoObjschedGetCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoObjschedGetCaps__
#define subdeviceCtrlCmdFifoObjschedGetCaps(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoObjschedGetCaps_HAL(pSubdevice, pParams) subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo__
#define subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo(pSubdevice, pGetChannelGrpUidParams) subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_DISPATCH(pSubdevice, pGetChannelGrpUidParams)
#define subdeviceCtrlCmdFifoQueryChannelUniqueId_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoQueryChannelUniqueId__
#define subdeviceCtrlCmdFifoQueryChannelUniqueId(pSubdevice, pQueryChannelUidParams) subdeviceCtrlCmdFifoQueryChannelUniqueId_DISPATCH(pSubdevice, pQueryChannelUidParams)
#define subdeviceCtrlCmdFifoGetDeviceInfoTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetDeviceInfoTable__
#define subdeviceCtrlCmdFifoGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetDeviceInfoTable_HAL(pSubdevice, pParams) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__
#define subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb(pSubdevice, pParams) subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoClearFaultedBit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoClearFaultedBit__
#define subdeviceCtrlCmdFifoClearFaultedBit(pSubdevice, pParams) subdeviceCtrlCmdFifoClearFaultedBit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoRunlistSetSchedPolicy_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__
#define subdeviceCtrlCmdFifoRunlistSetSchedPolicy(pSubdevice, pSchedPolicyParams) subdeviceCtrlCmdFifoRunlistSetSchedPolicy_DISPATCH(pSubdevice, pSchedPolicyParams)
#define subdeviceCtrlCmdFifoUpdateChannelInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoUpdateChannelInfo__
#define subdeviceCtrlCmdFifoUpdateChannelInfo(pSubdevice, pChannelInfo) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo)
#define subdeviceCtrlCmdFifoUpdateChannelInfo_HAL(pSubdevice, pChannelInfo) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo)
#define subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__
#define subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers(pSubdevice, pParams) subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalFifoGetNumChannels_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalFifoGetNumChannels__
#define subdeviceCtrlCmdInternalFifoGetNumChannels(pSubdevice, pNumChannelsParams) subdeviceCtrlCmdInternalFifoGetNumChannels_DISPATCH(pSubdevice, pNumChannelsParams)
#define subdeviceCtrlCmdFifoGetAllocatedChannels_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFifoGetAllocatedChannels__
#define subdeviceCtrlCmdFifoGetAllocatedChannels(pSubdevice, pParams) subdeviceCtrlCmdFifoGetAllocatedChannels_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalFifoGetNumSecureChannels_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalFifoGetNumSecureChannels__
#define subdeviceCtrlCmdInternalFifoGetNumSecureChannels(pSubdevice, pNumSecureChannelsParams) subdeviceCtrlCmdInternalFifoGetNumSecureChannels_DISPATCH(pSubdevice, pNumSecureChannelsParams)
#define subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__
#define subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling(pSubdevice, pToggleActiveChannelSchedulingParams) subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_DISPATCH(pSubdevice, pToggleActiveChannelSchedulingParams)
#define subdeviceCtrlCmdKGrGetInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetInfo__
#define subdeviceCtrlCmdKGrGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetInfoV2__
#define subdeviceCtrlCmdKGrGetInfoV2(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfoV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCapsV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCapsV2__
#define subdeviceCtrlCmdKGrGetCapsV2(pSubdevice, pGrCapsParams) subdeviceCtrlCmdKGrGetCapsV2_DISPATCH(pSubdevice, pGrCapsParams)
#define subdeviceCtrlCmdKGrGetCtxswModes_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCtxswModes__
#define subdeviceCtrlCmdKGrGetCtxswModes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswModes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswZcullMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullMode__
#define subdeviceCtrlCmdKGrCtxswZcullMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswZcullBind_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullBind__
#define subdeviceCtrlCmdKGrCtxswZcullBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetZcullInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetZcullInfo__
#define subdeviceCtrlCmdKGrGetZcullInfo(pSubdevice, pZcullInfoParams) subdeviceCtrlCmdKGrGetZcullInfo_DISPATCH(pSubdevice, pZcullInfoParams)
#define subdeviceCtrlCmdKGrCtxswPmMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswPmMode__
#define subdeviceCtrlCmdKGrCtxswPmMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPmMode_HAL(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPmBind_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswPmBind__
#define subdeviceCtrlCmdKGrCtxswPmBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswSetupBind_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswSetupBind__
#define subdeviceCtrlCmdKGrCtxswSetupBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswSetupBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetGpcTileMap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrSetGpcTileMap__
#define subdeviceCtrlCmdKGrSetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrSetGpcTileMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswSmpcMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswSmpcMode__
#define subdeviceCtrlCmdKGrCtxswSmpcMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswSmpcMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrPcSamplingMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrPcSamplingMode__
#define subdeviceCtrlCmdKGrPcSamplingMode(pSubdevice, pParams) subdeviceCtrlCmdKGrPcSamplingMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__
#define subdeviceCtrlCmdKGrGetSmToGpcTpcMappings(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGlobalSmOrder_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetGlobalSmOrder__
#define subdeviceCtrlCmdKGrGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGlobalSmOrder_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetCtxswPreemptionMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__
#define subdeviceCtrlCmdKGrSetCtxswPreemptionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetCtxswPreemptionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPreemptionBind_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrCtxswPreemptionBind__
#define subdeviceCtrlCmdKGrCtxswPreemptionBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPreemptionBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetROPInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetROPInfo__
#define subdeviceCtrlCmdKGrGetROPInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetROPInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxswStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCtxswStats__
#define subdeviceCtrlCmdKGrGetCtxswStats(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferSize__
#define subdeviceCtrlCmdKGrGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferInfo__
#define subdeviceCtrlCmdKGrGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferPtes_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferPtes__
#define subdeviceCtrlCmdKGrGetCtxBufferPtes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferPtes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCurrentResidentChannel_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetCurrentResidentChannel__
#define subdeviceCtrlCmdKGrGetCurrentResidentChannel(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCurrentResidentChannel_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetVatAlarmData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetVatAlarmData__
#define subdeviceCtrlCmdKGrGetVatAlarmData(pSubdevice, pParams) subdeviceCtrlCmdKGrGetVatAlarmData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetAttributeBufferSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetAttributeBufferSize__
#define subdeviceCtrlCmdKGrGetAttributeBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetAttributeBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolQuerySize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGfxPoolQuerySize__
#define subdeviceCtrlCmdKGrGfxPoolQuerySize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolQuerySize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolInitialize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGfxPoolInitialize__
#define subdeviceCtrlCmdKGrGfxPoolInitialize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolInitialize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolAddSlots_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGfxPoolAddSlots__
#define subdeviceCtrlCmdKGrGfxPoolAddSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolAddSlots_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolRemoveSlots_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__
#define subdeviceCtrlCmdKGrGfxPoolRemoveSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolRemoveSlots_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetPpcMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetPpcMask__
#define subdeviceCtrlCmdKGrGetPpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetTpcPartitionMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrSetTpcPartitionMode__
#define subdeviceCtrlCmdKGrSetTpcPartitionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetSmIssueRateModifier_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetSmIssueRateModifier__
#define subdeviceCtrlCmdKGrGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueRateModifier_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetSmIssueRateModifierV2__
#define subdeviceCtrlCmdKGrGetSmIssueRateModifierV2(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUid_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUid(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetPhysGpcMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetPhysGpcMask__
#define subdeviceCtrlCmdKGrGetPhysGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPhysGpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGpcMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetGpcMask__
#define subdeviceCtrlCmdKGrGetGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetTpcMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetTpcMask__
#define subdeviceCtrlCmdKGrGetTpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetTpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetEngineContextProperties_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetEngineContextProperties__
#define subdeviceCtrlCmdKGrGetEngineContextProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrGetEngineContextProperties_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetNumTpcsForGpc_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetNumTpcsForGpc__
#define subdeviceCtrlCmdKGrGetNumTpcsForGpc(pSubdevice, pParams) subdeviceCtrlCmdKGrGetNumTpcsForGpc_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGpcTileMap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetGpcTileMap__
#define subdeviceCtrlCmdKGrGetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcTileMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetZcullMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetZcullMask__
#define subdeviceCtrlCmdKGrGetZcullMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetZcullMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__
#define subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetInfo__
#define subdeviceCtrlCmdKGrInternalStaticGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetCaps__
#define subdeviceCtrlCmdKGrInternalStaticGetCaps(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__
#define subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__
#define subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__
#define subdeviceCtrlCmdKGrInternalStaticGetPpcMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__
#define subdeviceCtrlCmdKGrInternalStaticGetZcullInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetRopInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__
#define subdeviceCtrlCmdKGrInternalStaticGetRopInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetRopInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__
#define subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2__
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__
#define subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__
#define subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__
#define subdeviceCtrlCmdKGrInternalStaticGetPdbProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__
#define subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__
#define subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable(pSubdevice, pParams) subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__
#define subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__
#define subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__
#define subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset(pSubdevice, pParams) subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrStaticGetFecsTraceDefines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrStaticGetFecsTraceDefines__
#define subdeviceCtrlCmdGrStaticGetFecsTraceDefines(pSubdevice, pParams) subdeviceCtrlCmdGrStaticGetFecsTraceDefines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalInitBug4208224War_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdKGrInternalInitBug4208224War__
#define subdeviceCtrlCmdKGrInternalInitBug4208224War(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalInitBug4208224War_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetCachedInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetCachedInfo__
#define subdeviceCtrlCmdGpuGetCachedInfo(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetCachedInfo_DISPATCH(pSubdevice, pGpuInfoParams)
#define subdeviceCtrlCmdGpuGetInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetInfoV2__
#define subdeviceCtrlCmdGpuGetInfoV2(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetInfoV2_DISPATCH(pSubdevice, pGpuInfoParams)
#define subdeviceCtrlCmdGpuGetIpVersion_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetIpVersion__
#define subdeviceCtrlCmdGpuGetIpVersion(pSubdevice, pGpuIpVersionParams) subdeviceCtrlCmdGpuGetIpVersion_DISPATCH(pSubdevice, pGpuIpVersionParams)
#define subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__
#define subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_DISPATCH(pSubdevice, pBridgeInfoParams)
#define subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__
#define subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_DISPATCH(pSubdevice, pBridgeInfoParams)
#define subdeviceCtrlCmdGpuSetOptimusInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetOptimusInfo__
#define subdeviceCtrlCmdGpuSetOptimusInfo(pSubdevice, pGpuOptimusInfoParams) subdeviceCtrlCmdGpuSetOptimusInfo_DISPATCH(pSubdevice, pGpuOptimusInfoParams)
#define subdeviceCtrlCmdGpuGetNameString_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNameString__
#define subdeviceCtrlCmdGpuGetNameString(pSubdevice, pNameStringParams) subdeviceCtrlCmdGpuGetNameString_DISPATCH(pSubdevice, pNameStringParams)
#define subdeviceCtrlCmdGpuGetShortNameString_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetShortNameString__
#define subdeviceCtrlCmdGpuGetShortNameString(pSubdevice, pShortNameStringParams) subdeviceCtrlCmdGpuGetShortNameString_DISPATCH(pSubdevice, pShortNameStringParams)
#define subdeviceCtrlCmdGpuGetEncoderCapacity_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEncoderCapacity__
#define subdeviceCtrlCmdGpuGetEncoderCapacity(pSubdevice, pEncoderCapacityParams) subdeviceCtrlCmdGpuGetEncoderCapacity_DISPATCH(pSubdevice, pEncoderCapacityParams)
#define subdeviceCtrlCmdGpuGetNvencSwSessionStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionStats__
#define subdeviceCtrlCmdGpuGetNvencSwSessionStats(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionStats(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdGpuSetFabricAddr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetFabricAddr__
#define subdeviceCtrlCmdGpuSetFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdGpuSetFabricAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetEgmGpaFabricAddr__
#define subdeviceCtrlCmdGpuSetEgmGpaFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPower_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetPower__
#define subdeviceCtrlCmdGpuSetPower(pSubdevice, pSetPowerParams) subdeviceCtrlCmdGpuSetPower_DISPATCH(pSubdevice, pSetPowerParams)
#define subdeviceCtrlCmdGpuGetSdm_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetSdm__
#define subdeviceCtrlCmdGpuGetSdm(pSubdevice, pSdmParams) subdeviceCtrlCmdGpuGetSdm_DISPATCH(pSubdevice, pSdmParams)
#define subdeviceCtrlCmdGpuGetSimulationInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetSimulationInfo__
#define subdeviceCtrlCmdGpuGetSimulationInfo(pSubdevice, pGpuSimulationInfoParams) subdeviceCtrlCmdGpuGetSimulationInfo_DISPATCH(pSubdevice, pGpuSimulationInfoParams)
#define subdeviceCtrlCmdGpuGetEngines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEngines__
#define subdeviceCtrlCmdGpuGetEngines(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEnginesV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEnginesV2__
#define subdeviceCtrlCmdGpuGetEnginesV2(pSubdevice, pEngineParams) subdeviceCtrlCmdGpuGetEnginesV2_DISPATCH(pSubdevice, pEngineParams)
#define subdeviceCtrlCmdGpuGetEngineClasslist_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEngineClasslist__
#define subdeviceCtrlCmdGpuGetEngineClasslist(pSubdevice, pClassParams) subdeviceCtrlCmdGpuGetEngineClasslist_DISPATCH(pSubdevice, pClassParams)
#define subdeviceCtrlCmdGpuGetEnginePartnerList_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEnginePartnerList__
#define subdeviceCtrlCmdGpuGetEnginePartnerList(pSubdevice, pPartnerListParams) subdeviceCtrlCmdGpuGetEnginePartnerList_DISPATCH(pSubdevice, pPartnerListParams)
#define subdeviceCtrlCmdGpuGetFermiGpcInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetFermiGpcInfo__
#define subdeviceCtrlCmdGpuGetFermiGpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiGpcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetFermiTpcInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetFermiTpcInfo__
#define subdeviceCtrlCmdGpuGetFermiTpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiTpcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetFermiZcullInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetFermiZcullInfo__
#define subdeviceCtrlCmdGpuGetFermiZcullInfo(pSubdevice, pGpuFermiZcullInfoParams) subdeviceCtrlCmdGpuGetFermiZcullInfo_DISPATCH(pSubdevice, pGpuFermiZcullInfoParams)
#define subdeviceCtrlCmdGpuGetPesInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPesInfo__
#define subdeviceCtrlCmdGpuGetPesInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPesInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuExecRegOps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuExecRegOps__
#define subdeviceCtrlCmdGpuExecRegOps(pSubdevice, pRegParams) subdeviceCtrlCmdGpuExecRegOps_DISPATCH(pSubdevice, pRegParams)
#define subdeviceCtrlCmdGpuMigratableOps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuMigratableOps__
#define subdeviceCtrlCmdGpuMigratableOps(pSubdevice, pRegParams) subdeviceCtrlCmdGpuMigratableOps_DISPATCH(pSubdevice, pRegParams)
#define subdeviceCtrlCmdGpuQueryMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryMode__
#define subdeviceCtrlCmdGpuQueryMode(pSubdevice, pQueryMode) subdeviceCtrlCmdGpuQueryMode_DISPATCH(pSubdevice, pQueryMode)
#define subdeviceCtrlCmdGpuGetInforomImageVersion_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetInforomImageVersion__
#define subdeviceCtrlCmdGpuGetInforomImageVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomImageVersion_DISPATCH(pSubdevice, pVersionInfo)
#define subdeviceCtrlCmdGpuGetInforomObjectVersion_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetInforomObjectVersion__
#define subdeviceCtrlCmdGpuGetInforomObjectVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomObjectVersion_DISPATCH(pSubdevice, pVersionInfo)
#define subdeviceCtrlCmdGpuQueryInforomEccSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryInforomEccSupport__
#define subdeviceCtrlCmdGpuQueryInforomEccSupport(pSubdevice) subdeviceCtrlCmdGpuQueryInforomEccSupport_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuQueryEccStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryEccStatus__
#define subdeviceCtrlCmdGpuQueryEccStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryEccStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetChipDetails_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetChipDetails__
#define subdeviceCtrlCmdGpuGetChipDetails(pSubdevice, pParams) subdeviceCtrlCmdGpuGetChipDetails_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetOEMBoardInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetOEMBoardInfo__
#define subdeviceCtrlCmdGpuGetOEMBoardInfo(pSubdevice, pBoardInfo) subdeviceCtrlCmdGpuGetOEMBoardInfo_DISPATCH(pSubdevice, pBoardInfo)
#define subdeviceCtrlCmdGpuGetOEMInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetOEMInfo__
#define subdeviceCtrlCmdGpuGetOEMInfo(pSubdevice, pOemInfo) subdeviceCtrlCmdGpuGetOEMInfo_DISPATCH(pSubdevice, pOemInfo)
#define subdeviceCtrlCmdGpuHandleGpuSR_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuHandleGpuSR__
#define subdeviceCtrlCmdGpuHandleGpuSR(pSubdevice) subdeviceCtrlCmdGpuHandleGpuSR_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuSetComputeModeRules_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetComputeModeRules__
#define subdeviceCtrlCmdGpuSetComputeModeRules(pSubdevice, pSetRulesParams) subdeviceCtrlCmdGpuSetComputeModeRules_DISPATCH(pSubdevice, pSetRulesParams)
#define subdeviceCtrlCmdGpuQueryComputeModeRules_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryComputeModeRules__
#define subdeviceCtrlCmdGpuQueryComputeModeRules(pSubdevice, pQueryRulesParams) subdeviceCtrlCmdGpuQueryComputeModeRules_DISPATCH(pSubdevice, pQueryRulesParams)
#define subdeviceCtrlCmdGpuAcquireComputeModeReservation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuAcquireComputeModeReservation__
#define subdeviceCtrlCmdGpuAcquireComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuAcquireComputeModeReservation_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuReleaseComputeModeReservation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuReleaseComputeModeReservation__
#define subdeviceCtrlCmdGpuReleaseComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuReleaseComputeModeReservation_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuInitializeCtx_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuInitializeCtx__
#define subdeviceCtrlCmdGpuInitializeCtx(pSubdevice, pInitializeCtxParams) subdeviceCtrlCmdGpuInitializeCtx_DISPATCH(pSubdevice, pInitializeCtxParams)
#define subdeviceCtrlCmdGpuPromoteCtx_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuPromoteCtx__
#define subdeviceCtrlCmdGpuPromoteCtx(pSubdevice, pPromoteCtxParams) subdeviceCtrlCmdGpuPromoteCtx_DISPATCH(pSubdevice, pPromoteCtxParams)
#define subdeviceCtrlCmdGpuEvictCtx_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuEvictCtx__
#define subdeviceCtrlCmdGpuEvictCtx(pSubdevice, pEvictCtxParams) subdeviceCtrlCmdGpuEvictCtx_DISPATCH(pSubdevice, pEvictCtxParams)
#define subdeviceCtrlCmdGpuGetId_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetId__
#define subdeviceCtrlCmdGpuGetId(pSubdevice, pIdParams) subdeviceCtrlCmdGpuGetId_DISPATCH(pSubdevice, pIdParams)
#define subdeviceCtrlCmdGpuGetGidInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetGidInfo__
#define subdeviceCtrlCmdGpuGetGidInfo(pSubdevice, pGidInfoParams) subdeviceCtrlCmdGpuGetGidInfo_DISPATCH(pSubdevice, pGidInfoParams)
#define subdeviceCtrlCmdGpuQueryIllumSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryIllumSupport__
#define subdeviceCtrlCmdGpuQueryIllumSupport(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuQueryIllumSupport_HAL(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuGetIllum_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetIllum__
#define subdeviceCtrlCmdGpuGetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuGetIllum_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuSetIllum_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetIllum__
#define subdeviceCtrlCmdGpuSetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuSetIllum_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuQueryScrubberStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryScrubberStatus__
#define subdeviceCtrlCmdGpuQueryScrubberStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryScrubberStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVprCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetVprCaps__
#define subdeviceCtrlCmdGpuGetVprCaps(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVprInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetVprInfo__
#define subdeviceCtrlCmdGpuGetVprInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPids_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPids__
#define subdeviceCtrlCmdGpuGetPids(pSubdevice, pGetPidsParams) subdeviceCtrlCmdGpuGetPids_DISPATCH(pSubdevice, pGetPidsParams)
#define subdeviceCtrlCmdGpuGetPidInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPidInfo__
#define subdeviceCtrlCmdGpuGetPidInfo(pSubdevice, pGetPidInfoParams) subdeviceCtrlCmdGpuGetPidInfo_DISPATCH(pSubdevice, pGetPidInfoParams)
#define subdeviceCtrlCmdGpuQueryFunctionStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryFunctionStatus__
#define subdeviceCtrlCmdGpuQueryFunctionStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryFunctionStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuReportNonReplayableFault_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuReportNonReplayableFault__
#define subdeviceCtrlCmdGpuReportNonReplayableFault(pSubdevice, pParams) subdeviceCtrlCmdGpuReportNonReplayableFault_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineFaultInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEngineFaultInfo__
#define subdeviceCtrlCmdGpuGetEngineFaultInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineFaultInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineRunlistPriBase_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__
#define subdeviceCtrlCmdGpuGetEngineRunlistPriBase(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineRunlistPriBase_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetHwEngineId_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetHwEngineId__
#define subdeviceCtrlCmdGpuGetHwEngineId(pSubdevice, pParams) subdeviceCtrlCmdGpuGetHwEngineId_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetFirstAsyncCEIdx__
#define subdeviceCtrlCmdGpuGetFirstAsyncCEIdx(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVmmuSegmentSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetVmmuSegmentSize__
#define subdeviceCtrlCmdGpuGetVmmuSegmentSize(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVmmuSegmentSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetMaxSupportedPageSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__
#define subdeviceCtrlCmdGpuGetMaxSupportedPageSize(pSubdevice, pParams) subdeviceCtrlCmdGpuGetMaxSupportedPageSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuHandleVfPriFault_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuHandleVfPriFault__
#define subdeviceCtrlCmdGpuHandleVfPriFault(pSubdevice, pParams) subdeviceCtrlCmdGpuHandleVfPriFault_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetComputePolicyConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetComputePolicyConfig__
#define subdeviceCtrlCmdGpuSetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuSetComputePolicyConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetComputePolicyConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetComputePolicyConfig__
#define subdeviceCtrlCmdGpuGetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputePolicyConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdValidateMemMapRequest_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdValidateMemMapRequest__
#define subdeviceCtrlCmdValidateMemMapRequest(pSubdevice, pParams) subdeviceCtrlCmdValidateMemMapRequest_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetGfid_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetGfid__
#define subdeviceCtrlCmdGpuGetGfid(pSubdevice, pParams) subdeviceCtrlCmdGpuGetGfid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdUpdateGfidP2pCapability_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdUpdateGfidP2pCapability__
#define subdeviceCtrlCmdUpdateGfidP2pCapability(pSubdevice, pParams) subdeviceCtrlCmdUpdateGfidP2pCapability_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineLoadTimes_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetEngineLoadTimes__
#define subdeviceCtrlCmdGpuGetEngineLoadTimes(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineLoadTimes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetP2pCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetP2pCaps__
#define subdeviceCtrlCmdGetP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdGetP2pCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetGpuFabricProbeInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetGpuFabricProbeInfo__
#define subdeviceCtrlCmdGetGpuFabricProbeInfo(pSubdevice, pParams) subdeviceCtrlCmdGetGpuFabricProbeInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuMarkDeviceForReset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuMarkDeviceForReset__
#define subdeviceCtrlCmdGpuMarkDeviceForReset(pSubdevice) subdeviceCtrlCmdGpuMarkDeviceForReset_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuUnmarkDeviceForReset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuUnmarkDeviceForReset__
#define subdeviceCtrlCmdGpuUnmarkDeviceForReset(pSubdevice) subdeviceCtrlCmdGpuUnmarkDeviceForReset_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset__
#define subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset(pSubdevice) subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset__
#define subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset(pSubdevice) subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuGetResetStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetResetStatus__
#define subdeviceCtrlCmdGpuGetResetStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuGetResetStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetDrainAndResetStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetDrainAndResetStatus__
#define subdeviceCtrlCmdGpuGetDrainAndResetStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuGetDrainAndResetStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetConstructedFalconInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetConstructedFalconInfo__
#define subdeviceCtrlCmdGpuGetConstructedFalconInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetConstructedFalconInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlGpuGetFipsStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlGpuGetFipsStatus__
#define subdeviceCtrlGpuGetFipsStatus(pSubdevice, pParams) subdeviceCtrlGpuGetFipsStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVfCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetVfCaps__
#define subdeviceCtrlCmdGpuGetVfCaps(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVfCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetRecoveryAction_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetRecoveryAction__
#define subdeviceCtrlCmdGpuGetRecoveryAction(pSubdevice, pParams) subdeviceCtrlCmdGpuGetRecoveryAction_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuRpcGspTest_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuRpcGspTest__
#define subdeviceCtrlCmdGpuRpcGspTest(pSubdevice, pParams) subdeviceCtrlCmdGpuRpcGspTest_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuRpcGspQuerySizes_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuRpcGspQuerySizes__
#define subdeviceCtrlCmdGpuRpcGspQuerySizes(pSubdevice, pParams) subdeviceCtrlCmdGpuRpcGspQuerySizes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdRusdGetSupportedFeatures_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRusdGetSupportedFeatures__
#define subdeviceCtrlCmdRusdGetSupportedFeatures(pSubdevice, pParams) subdeviceCtrlCmdRusdGetSupportedFeatures_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdEventSetTrigger_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetTrigger__
#define subdeviceCtrlCmdEventSetTrigger(pSubdevice) subdeviceCtrlCmdEventSetTrigger_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdEventSetTriggerFifo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetTriggerFifo__
#define subdeviceCtrlCmdEventSetTriggerFifo(pSubdevice, pTriggerFifoParams) subdeviceCtrlCmdEventSetTriggerFifo_DISPATCH(pSubdevice, pTriggerFifoParams)
#define subdeviceCtrlCmdEventSetNotification_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetNotification__
#define subdeviceCtrlCmdEventSetNotification(pSubdevice, pSetEventParams) subdeviceCtrlCmdEventSetNotification_DISPATCH(pSubdevice, pSetEventParams)
#define subdeviceCtrlCmdEventSetMemoryNotifies_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetMemoryNotifies__
#define subdeviceCtrlCmdEventSetMemoryNotifies(pSubdevice, pSetMemoryNotifiesParams) subdeviceCtrlCmdEventSetMemoryNotifies_DISPATCH(pSubdevice, pSetMemoryNotifiesParams)
#define subdeviceCtrlCmdEventSetSemaphoreMemory_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetSemaphoreMemory__
#define subdeviceCtrlCmdEventSetSemaphoreMemory(pSubdevice, pSetSemMemoryParams) subdeviceCtrlCmdEventSetSemaphoreMemory_DISPATCH(pSubdevice, pSetSemMemoryParams)
#define subdeviceCtrlCmdEventSetSemaMemValidation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventSetSemaMemValidation__
#define subdeviceCtrlCmdEventSetSemaMemValidation(pSubdevice, pSetSemaMemValidationParams) subdeviceCtrlCmdEventSetSemaMemValidation_DISPATCH(pSubdevice, pSetSemaMemValidationParams)
#define subdeviceCtrlCmdEventVideoBindEvtbuf_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventVideoBindEvtbuf__
#define subdeviceCtrlCmdEventVideoBindEvtbuf(pSubdevice, pBindParams) subdeviceCtrlCmdEventVideoBindEvtbuf_DISPATCH(pSubdevice, pBindParams)
#define subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__
#define subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf(pSubdevice, pBindParams) subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_DISPATCH(pSubdevice, pBindParams)
#define subdeviceCtrlCmdTimerCancel_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerCancel__
#define subdeviceCtrlCmdTimerCancel(pSubdevice) subdeviceCtrlCmdTimerCancel_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdTimerSchedule_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerSchedule__
#define subdeviceCtrlCmdTimerSchedule(pSubdevice, pParams) subdeviceCtrlCmdTimerSchedule_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerGetTime_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerGetTime__
#define subdeviceCtrlCmdTimerGetTime(pSubdevice, pParams) subdeviceCtrlCmdTimerGetTime_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerGetRegisterOffset_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerGetRegisterOffset__
#define subdeviceCtrlCmdTimerGetRegisterOffset(pSubdevice, pTimerRegOffsetParams) subdeviceCtrlCmdTimerGetRegisterOffset_DISPATCH(pSubdevice, pTimerRegOffsetParams)
#define subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__
#define subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo(pSubdevice, pParams) subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerSetGrTickFreq_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdTimerSetGrTickFreq__
#define subdeviceCtrlCmdTimerSetGrTickFreq(pSubdevice, pParams) subdeviceCtrlCmdTimerSetGrTickFreq_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdRcReadVirtualMem_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcReadVirtualMem__
#define subdeviceCtrlCmdRcReadVirtualMem(pSubdevice, pReadVirtMemParam) subdeviceCtrlCmdRcReadVirtualMem_DISPATCH(pSubdevice, pReadVirtMemParam)
#define subdeviceCtrlCmdRcGetErrorCount_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcGetErrorCount__
#define subdeviceCtrlCmdRcGetErrorCount(pSubdevice, pErrorCount) subdeviceCtrlCmdRcGetErrorCount_DISPATCH(pSubdevice, pErrorCount)
#define subdeviceCtrlCmdRcGetErrorV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcGetErrorV2__
#define subdeviceCtrlCmdRcGetErrorV2(pSubdevice, pErrorParams) subdeviceCtrlCmdRcGetErrorV2_DISPATCH(pSubdevice, pErrorParams)
#define subdeviceCtrlCmdRcSetCleanErrorHistory_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcSetCleanErrorHistory__
#define subdeviceCtrlCmdRcSetCleanErrorHistory(pSubdevice) subdeviceCtrlCmdRcSetCleanErrorHistory_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcGetWatchdogInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcGetWatchdogInfo__
#define subdeviceCtrlCmdRcGetWatchdogInfo(pSubdevice, pWatchdogInfoParams) subdeviceCtrlCmdRcGetWatchdogInfo_DISPATCH(pSubdevice, pWatchdogInfoParams)
#define subdeviceCtrlCmdRcDisableWatchdog_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcDisableWatchdog__
#define subdeviceCtrlCmdRcDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcDisableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcSoftDisableWatchdog_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcSoftDisableWatchdog__
#define subdeviceCtrlCmdRcSoftDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcSoftDisableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcEnableWatchdog_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcEnableWatchdog__
#define subdeviceCtrlCmdRcEnableWatchdog(pSubdevice) subdeviceCtrlCmdRcEnableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcReleaseWatchdogRequests_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdRcReleaseWatchdogRequests__
#define subdeviceCtrlCmdRcReleaseWatchdogRequests(pSubdevice) subdeviceCtrlCmdRcReleaseWatchdogRequests_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalRcWatchdogTimeout_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalRcWatchdogTimeout__
#define subdeviceCtrlCmdInternalRcWatchdogTimeout(pSubdevice) subdeviceCtrlCmdInternalRcWatchdogTimeout_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdSetRcRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetRcRecovery__
#define subdeviceCtrlCmdSetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdSetRcRecovery_HAL(pSubdevice, pRcRecovery) subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetRcRecovery__
#define subdeviceCtrlCmdGetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcRecovery_HAL(pSubdevice, pRcRecovery) subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetRcInfo__
#define subdeviceCtrlCmdGetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdGetRcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSetRcInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSetRcInfo__
#define subdeviceCtrlCmdSetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdSetRcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvdGetDumpSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvdGetDumpSize__
#define subdeviceCtrlCmdNvdGetDumpSize(pSubdevice, pDumpSizeParams) subdeviceCtrlCmdNvdGetDumpSize_DISPATCH(pSubdevice, pDumpSizeParams)
#define subdeviceCtrlCmdNvdGetDump_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvdGetDump__
#define subdeviceCtrlCmdNvdGetDump(pSubdevice, pDumpParams) subdeviceCtrlCmdNvdGetDump_DISPATCH(pSubdevice, pDumpParams)
#define subdeviceCtrlCmdNvdGetNocatJournalRpt_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvdGetNocatJournalRpt__
#define subdeviceCtrlCmdNvdGetNocatJournalRpt(pSubdevice, pReportParams) subdeviceCtrlCmdNvdGetNocatJournalRpt_DISPATCH(pSubdevice, pReportParams)
#define subdeviceCtrlCmdNvdSetNocatJournalData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvdSetNocatJournalData__
#define subdeviceCtrlCmdNvdSetNocatJournalData(pSubdevice, pReportParams) subdeviceCtrlCmdNvdSetNocatJournalData_DISPATCH(pSubdevice, pReportParams)
#define subdeviceCtrlCmdNvdInsertNocatJournalRecord_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdNvdInsertNocatJournalRecord__
#define subdeviceCtrlCmdNvdInsertNocatJournalRecord(pSubdevice, pReportParams) subdeviceCtrlCmdNvdInsertNocatJournalRecord_DISPATCH(pSubdevice, pReportParams)
#define subdeviceCtrlCmdPmgrGetModuleInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdPmgrGetModuleInfo__
#define subdeviceCtrlCmdPmgrGetModuleInfo(pSubdevice, pModuleInfoParams) subdeviceCtrlCmdPmgrGetModuleInfo_DISPATCH(pSubdevice, pModuleInfoParams)
#define subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuProcessPostGc6ExitTasks__
#define subdeviceCtrlCmdGpuProcessPostGc6ExitTasks(pSubdevice) subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGc6Entry_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGc6Entry__
#define subdeviceCtrlCmdGc6Entry(pSubdevice, pParams) subdeviceCtrlCmdGc6Entry_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGc6Exit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGc6Exit__
#define subdeviceCtrlCmdGc6Exit(pSubdevice, pParams) subdeviceCtrlCmdGc6Exit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdLpwrDifrPrefetchResponse_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdLpwrDifrPrefetchResponse__
#define subdeviceCtrlCmdLpwrDifrPrefetchResponse(pSubdevice, pParams) subdeviceCtrlCmdLpwrDifrPrefetchResponse_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdLpwrDifrCtrl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdLpwrDifrCtrl__
#define subdeviceCtrlCmdLpwrDifrCtrl(pSubdevice, pParams) subdeviceCtrlCmdLpwrDifrCtrl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCeGetCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetCaps__
#define subdeviceCtrlCmdCeGetCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCaps_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCePceMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetCePceMask__
#define subdeviceCtrlCmdCeGetCePceMask(pSubdevice, pCePceMaskParams) subdeviceCtrlCmdCeGetCePceMask_DISPATCH(pSubdevice, pCePceMaskParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappings_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeUpdatePceLceMappings__
#define subdeviceCtrlCmdCeUpdatePceLceMappings(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappings_DISPATCH(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappings_HAL(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappings_DISPATCH(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappingsV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeUpdatePceLceMappingsV2__
#define subdeviceCtrlCmdCeUpdatePceLceMappingsV2(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappingsV2_DISPATCH(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappingsV2_HAL(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappingsV2_DISPATCH(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdCeGetLceShimInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetLceShimInfo__
#define subdeviceCtrlCmdCeGetLceShimInfo(pSubdevice, pParams) subdeviceCtrlCmdCeGetLceShimInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCeGetPceConfigForLceType_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetPceConfigForLceType__
#define subdeviceCtrlCmdCeGetPceConfigForLceType(pSubdevice, pParams) subdeviceCtrlCmdCeGetPceConfigForLceType_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCeGetCapsV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetCapsV2__
#define subdeviceCtrlCmdCeGetCapsV2(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCapsV2_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetAllCaps__
#define subdeviceCtrlCmdCeGetAllCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllCaps_HAL(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetDecompLceMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetDecompLceMask__
#define subdeviceCtrlCmdCeGetDecompLceMask(pSubdevice, pParams) subdeviceCtrlCmdCeGetDecompLceMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCeIsDecompLceEnabled_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeIsDecompLceEnabled__
#define subdeviceCtrlCmdCeIsDecompLceEnabled(pSubdevice, pParams) subdeviceCtrlCmdCeIsDecompLceEnabled_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetDmemUsage_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnGetDmemUsage__
#define subdeviceCtrlCmdFlcnGetDmemUsage(pSubdevice, pFlcnDmemUsageParams) subdeviceCtrlCmdFlcnGetDmemUsage_DISPATCH(pSubdevice, pFlcnDmemUsageParams)
#define subdeviceCtrlCmdFlcnGetEngineArch_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnGetEngineArch__
#define subdeviceCtrlCmdFlcnGetEngineArch(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetEngineArch_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerQueueInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnUstreamerQueueInfo__
#define subdeviceCtrlCmdFlcnUstreamerQueueInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerQueueInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerControlGet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlGet__
#define subdeviceCtrlCmdFlcnUstreamerControlGet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlGet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerControlSet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlSet__
#define subdeviceCtrlCmdFlcnUstreamerControlSet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlSet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetCtxBufferInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferInfo__
#define subdeviceCtrlCmdFlcnGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetCtxBufferSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferSize__
#define subdeviceCtrlCmdFlcnGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdEccGetClientExposedCounters_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEccGetClientExposedCounters__
#define subdeviceCtrlCmdEccGetClientExposedCounters(pSubdevice, pParams) subdeviceCtrlCmdEccGetClientExposedCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdEccGetVolatileCounts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdEccGetVolatileCounts__
#define subdeviceCtrlCmdEccGetVolatileCounts(pSubdevice, pParams) subdeviceCtrlCmdEccGetVolatileCounts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryEccConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuQueryEccConfiguration__
#define subdeviceCtrlCmdGpuQueryEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuQueryEccConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuSetEccConfiguration_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetEccConfiguration__
#define subdeviceCtrlCmdGpuSetEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuSetEccConfiguration_HAL(pSubdevice, pConfig) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuResetEccErrorStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuResetEccErrorStatus__
#define subdeviceCtrlCmdGpuResetEccErrorStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuResetEccErrorStatus_HAL(pSubdevice, pParams) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaRange_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlaRange__
#define subdeviceCtrlCmdFlaRange(pSubdevice, pParams) subdeviceCtrlCmdFlaRange_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaSetupInstanceMemBlock_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlaSetupInstanceMemBlock__
#define subdeviceCtrlCmdFlaSetupInstanceMemBlock(pSubdevice, pParams) subdeviceCtrlCmdFlaSetupInstanceMemBlock_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaGetRange_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlaGetRange__
#define subdeviceCtrlCmdFlaGetRange(pSubdevice, pParams) subdeviceCtrlCmdFlaGetRange_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaGetFabricMemStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdFlaGetFabricMemStats__
#define subdeviceCtrlCmdFlaGetFabricMemStats(pSubdevice, pParams) subdeviceCtrlCmdFlaGetFabricMemStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGspGetFeatures_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGspGetFeatures__
#define subdeviceCtrlCmdGspGetFeatures(pSubdevice, pGspFeaturesParams) subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams)
#define subdeviceCtrlCmdGspGetFeatures_HAL(pSubdevice, pGspFeaturesParams) subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams)
#define subdeviceCtrlCmdGspGetRmHeapStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGspGetRmHeapStats__
#define subdeviceCtrlCmdGspGetRmHeapStats(pSubdevice, pGspRmHeapStatsParams) subdeviceCtrlCmdGspGetRmHeapStats_DISPATCH(pSubdevice, pGspRmHeapStatsParams)
#define subdeviceCtrlCmdGpuGetVgpuHeapStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetVgpuHeapStats__
#define subdeviceCtrlCmdGpuGetVgpuHeapStats(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVgpuHeapStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdLibosGetHeapStats_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdLibosGetHeapStats__
#define subdeviceCtrlCmdLibosGetHeapStats(pSubdevice, pGspLibosHeapStatsParams) subdeviceCtrlCmdLibosGetHeapStats_DISPATCH(pSubdevice, pGspLibosHeapStatsParams)
#define subdeviceCtrlCmdGpuGetActivePartitionIds_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetActivePartitionIds__
#define subdeviceCtrlCmdGpuGetActivePartitionIds(pSubdevice, pParams) subdeviceCtrlCmdGpuGetActivePartitionIds_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPartitionCapacity_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPartitionCapacity__
#define subdeviceCtrlCmdGpuGetPartitionCapacity(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitionCapacity_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuDescribePartitions_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuDescribePartitions__
#define subdeviceCtrlCmdGpuDescribePartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuDescribePartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPartitioningMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetPartitioningMode__
#define subdeviceCtrlCmdGpuSetPartitioningMode(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitioningMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrmgrGetGrFsInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGrmgrGetGrFsInfo__
#define subdeviceCtrlCmdGrmgrGetGrFsInfo(pSubdevice, pParams) subdeviceCtrlCmdGrmgrGetGrFsInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPartitions_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuSetPartitions__
#define subdeviceCtrlCmdGpuSetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPartitions_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetPartitions__
#define subdeviceCtrlCmdGpuGetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetComputeProfiles_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetComputeProfiles__
#define subdeviceCtrlCmdGpuGetComputeProfiles(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputeProfiles_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetComputeProfileCapacity_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGpuGetComputeProfileCapacity__
#define subdeviceCtrlCmdGpuGetComputeProfileCapacity(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputeProfileCapacity_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__
#define subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__
#define subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__
#define subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__
#define subdeviceCtrlCmdOsUnixGc6BlockerRefCnt(pSubdevice, pParams) subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixAllowDisallowGcoff_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__
#define subdeviceCtrlCmdOsUnixAllowDisallowGcoff(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAllowDisallowGcoff_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixAudioDynamicPower_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdOsUnixAudioDynamicPower__
#define subdeviceCtrlCmdOsUnixAudioDynamicPower(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAudioDynamicPower_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__
#define subdeviceCtrlCmdOsUnixVidmemPersistenceStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixUpdateTgpStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdOsUnixUpdateTgpStatus__
#define subdeviceCtrlCmdOsUnixUpdateTgpStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixUpdateTgpStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetIpVersion_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayGetIpVersion__
#define subdeviceCtrlCmdDisplayGetIpVersion(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetIpVersion_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetStaticInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayGetStaticInfo__
#define subdeviceCtrlCmdDisplayGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetChannelPushbuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplaySetChannelPushbuffer__
#define subdeviceCtrlCmdDisplaySetChannelPushbuffer(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetChannelPushbuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayWriteInstMem_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayWriteInstMem__
#define subdeviceCtrlCmdDisplayWriteInstMem(pSubdevice, pParams) subdeviceCtrlCmdDisplayWriteInstMem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetupRgLineIntr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplaySetupRgLineIntr__
#define subdeviceCtrlCmdDisplaySetupRgLineIntr(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetupRgLineIntr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetImportedImpData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplaySetImportedImpData__
#define subdeviceCtrlCmdDisplaySetImportedImpData(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetImportedImpData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetDisplayMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayGetDisplayMask__
#define subdeviceCtrlCmdDisplayGetDisplayMask(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetDisplayMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayPinsetsToLockpins_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayPinsetsToLockpins__
#define subdeviceCtrlCmdDisplayPinsetsToLockpins(pSubdevice, pParams) subdeviceCtrlCmdDisplayPinsetsToLockpins_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__
#define subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpioProgramDirection_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpioProgramDirection__
#define subdeviceCtrlCmdInternalGpioProgramDirection(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioProgramDirection_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpioProgramOutput_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpioProgramOutput__
#define subdeviceCtrlCmdInternalGpioProgramOutput(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioProgramOutput_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpioReadInput_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpioReadInput__
#define subdeviceCtrlCmdInternalGpioReadInput(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioReadInput_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpioActivateHwFunction_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpioActivateHwFunction__
#define subdeviceCtrlCmdInternalGpioActivateHwFunction(pSubdevice, pParams) subdeviceCtrlCmdInternalGpioActivateHwFunction_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__
#define subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated(pSubdevice) subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalDisplayPreModeSet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalDisplayPreModeSet__
#define subdeviceCtrlCmdInternalDisplayPreModeSet(pSubdevice) subdeviceCtrlCmdInternalDisplayPreModeSet_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalDisplayPostModeSet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalDisplayPostModeSet__
#define subdeviceCtrlCmdInternalDisplayPostModeSet(pSubdevice) subdeviceCtrlCmdInternalDisplayPostModeSet_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdDisplayPreUnixConsole_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayPreUnixConsole__
#define subdeviceCtrlCmdDisplayPreUnixConsole(pSubdevice, pParams) subdeviceCtrlCmdDisplayPreUnixConsole_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayPostUnixConsole_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdDisplayPostUnixConsole__
#define subdeviceCtrlCmdDisplayPostUnixConsole(pSubdevice, pParams) subdeviceCtrlCmdDisplayPostUnixConsole_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__
#define subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__
#define subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetChipInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetChipInfo__
#define subdeviceCtrlCmdInternalGetChipInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalGetChipInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetUserRegisterAccessMap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__
#define subdeviceCtrlCmdInternalGetUserRegisterAccessMap(pSubdevice, pParams) subdeviceCtrlCmdInternalGetUserRegisterAccessMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetDeviceInfoTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetDeviceInfoTable__
#define subdeviceCtrlCmdInternalGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdInternalGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalRecoverAllComputeContexts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalRecoverAllComputeContexts__
#define subdeviceCtrlCmdInternalRecoverAllComputeContexts(pSubdevice) subdeviceCtrlCmdInternalRecoverAllComputeContexts_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalGetSmcMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetSmcMode__
#define subdeviceCtrlCmdInternalGetSmcMode(pSubdevice, pParams) subdeviceCtrlCmdInternalGetSmcMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdIsEgpuBridge_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdIsEgpuBridge__
#define subdeviceCtrlCmdIsEgpuBridge(pSubdevice, pParams) subdeviceCtrlCmdIsEgpuBridge_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__
#define subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap(pSubdevice, pParams) subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusFlushWithSysmembar_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusFlushWithSysmembar__
#define subdeviceCtrlCmdInternalBusFlushWithSysmembar(pSubdevice) subdeviceCtrlCmdInternalBusFlushWithSysmembar_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusDestroyP2pMailbox_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__
#define subdeviceCtrlCmdInternalBusDestroyP2pMailbox(pSubdevice, pParams) subdeviceCtrlCmdInternalBusDestroyP2pMailbox_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__
#define subdeviceCtrlCmdInternalBusCreateC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__
#define subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpuGetPFBar1Spa_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpuGetPFBar1Spa__
#define subdeviceCtrlCmdInternalGpuGetPFBar1Spa(pSubdevice, pConfigParams) subdeviceCtrlCmdInternalGpuGetPFBar1Spa_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__
#define subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries(pSubdevice, pParams) subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGmmuGetStaticInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGmmuGetStaticInfo__
#define subdeviceCtrlCmdGmmuGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdGmmuGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__
#define subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__
#define subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer(pSubdevice) subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__
#define subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__
#define subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__
#define subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer(pSubdevice, pCopyServerReservedPdesParams) subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_DISPATCH(pSubdevice, pCopyServerReservedPdesParams)
#define subdeviceCtrlCmdCeGetPhysicalCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetPhysicalCaps__
#define subdeviceCtrlCmdCeGetPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetPhysicalCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllPhysicalCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetAllPhysicalCaps__
#define subdeviceCtrlCmdCeGetAllPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllPhysicalCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeUpdateClassDB_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeUpdateClassDB__
#define subdeviceCtrlCmdCeUpdateClassDB(pSubdevice, params) subdeviceCtrlCmdCeUpdateClassDB_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdCeGetFaultMethodBufferSize_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetFaultMethodBufferSize__
#define subdeviceCtrlCmdCeGetFaultMethodBufferSize(pSubdevice, params) subdeviceCtrlCmdCeGetFaultMethodBufferSize_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdCeGetHubPceMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetHubPceMask__
#define subdeviceCtrlCmdCeGetHubPceMask(pSubdevice, pParams) subdeviceCtrlCmdCeGetHubPceMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCeGetHubPceMaskV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCeGetHubPceMaskV2__
#define subdeviceCtrlCmdCeGetHubPceMaskV2(pSubdevice, pParams) subdeviceCtrlCmdCeGetHubPceMaskV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdIntrGetKernelTable_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdIntrGetKernelTable__
#define subdeviceCtrlCmdIntrGetKernelTable(pSubdevice, pParams) subdeviceCtrlCmdIntrGetKernelTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfBoostSet_2x_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_2x__
#define subdeviceCtrlCmdInternalPerfBoostSet_2x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_2x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfBoostSet_3x_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_3x__
#define subdeviceCtrlCmdInternalPerfBoostSet_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_3x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfBoostClear_3x_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfBoostClear_3x__
#define subdeviceCtrlCmdInternalPerfBoostClear_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostClear_3x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__
#define subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__
#define subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfGetAuxPowerState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfGetAuxPowerState__
#define subdeviceCtrlCmdInternalPerfGetAuxPowerState(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGetAuxPowerState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGcxEntryPrerequisite_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGcxEntryPrerequisite__
#define subdeviceCtrlCmdInternalGcxEntryPrerequisite(pSubdevice, pGcxEntryPrerequisite) subdeviceCtrlCmdInternalGcxEntryPrerequisite_DISPATCH(pSubdevice, pGcxEntryPrerequisite)
#define subdeviceCtrlCmdBifGetStaticInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBifGetStaticInfo__
#define subdeviceCtrlCmdBifGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifGetStaticInfo_HAL(pSubdevice, pParams) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifGetAspmL1Flags_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBifGetAspmL1Flags__
#define subdeviceCtrlCmdBifGetAspmL1Flags(pSubdevice, pParams) subdeviceCtrlCmdBifGetAspmL1Flags_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifSetPcieRo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBifSetPcieRo__
#define subdeviceCtrlCmdBifSetPcieRo(pSubdevice, pParams) subdeviceCtrlCmdBifSetPcieRo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifDisableSystemMemoryAccess_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdBifDisableSystemMemoryAccess__
#define subdeviceCtrlCmdBifDisableSystemMemoryAccess(pSubdevice, pParams) subdeviceCtrlCmdBifDisableSystemMemoryAccess_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubPeerConnConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubPeerConnConfig__
#define subdeviceCtrlCmdHshubPeerConnConfig(pSubdevice, pParams) subdeviceCtrlCmdHshubPeerConnConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubGetHshubIdForLinks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubGetHshubIdForLinks__
#define subdeviceCtrlCmdHshubGetHshubIdForLinks(pSubdevice, pParams) subdeviceCtrlCmdHshubGetHshubIdForLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubGetNumUnits_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubGetNumUnits__
#define subdeviceCtrlCmdHshubGetNumUnits(pSubdevice, pParams) subdeviceCtrlCmdHshubGetNumUnits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubNextHshubId_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubNextHshubId__
#define subdeviceCtrlCmdHshubNextHshubId(pSubdevice, pParams) subdeviceCtrlCmdHshubNextHshubId_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubGetMaxHshubsPerShim_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubGetMaxHshubsPerShim__
#define subdeviceCtrlCmdHshubGetMaxHshubsPerShim(pSubdevice, pParams) subdeviceCtrlCmdHshubGetMaxHshubsPerShim_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubEgmConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdHshubEgmConfig__
#define subdeviceCtrlCmdHshubEgmConfig(pSubdevice, pParams) subdeviceCtrlCmdHshubEgmConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__
#define subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr(pSubdevice) subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__
#define subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__
#define subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__
#define subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__
#define subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer__
#define subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkCoreCallback_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkCoreCallback__
#define subdeviceCtrlCmdInternalNvlinkCoreCallback(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkCoreCallback_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid__
#define subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetAliEnabled_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetAliEnabled__
#define subdeviceCtrlCmdInternalNvlinkGetAliEnabled(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetAliEnabled_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState__
#define subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkProgramBufferready_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkProgramBufferready__
#define subdeviceCtrlCmdInternalNvlinkProgramBufferready(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProgramBufferready_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors__
#define subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors(pSubdevice) subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig__
#define subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask__
#define subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection__
#define subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer__
#define subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer__
#define subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateHshubMux__
#define subdeviceCtrlCmdInternalNvlinkUpdateHshubMux(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping__
#define subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology__
#define subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli__
#define subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet__
#define subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkLinkTrainAli_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkLinkTrainAli__
#define subdeviceCtrlCmdInternalNvlinkLinkTrainAli(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkLinkTrainAli_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo__
#define subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo__
#define subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed__
#define subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkAreLinksTrained_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkAreLinksTrained__
#define subdeviceCtrlCmdInternalNvlinkAreLinksTrained(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkAreLinksTrained_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkResetLinks_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkResetLinks__
#define subdeviceCtrlCmdInternalNvlinkResetLinks(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkResetLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts__
#define subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo__
#define subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem__
#define subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs__
#define subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps__
#define subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts__
#define subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask__
#define subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetP2pCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalSetP2pCaps__
#define subdeviceCtrlCmdInternalSetP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalSetP2pCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalRemoveP2pCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalRemoveP2pCaps__
#define subdeviceCtrlCmdInternalRemoveP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalRemoveP2pCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetPcieP2pCaps_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetPcieP2pCaps__
#define subdeviceCtrlCmdInternalGetPcieP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalGetPcieP2pCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetLocalAtsConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGetLocalAtsConfig__
#define subdeviceCtrlCmdInternalGetLocalAtsConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetLocalAtsConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetPeerAtsConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalSetPeerAtsConfig__
#define subdeviceCtrlCmdInternalSetPeerAtsConfig(pSubdevice, pParams) subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetPeerAtsConfig_HAL(pSubdevice, pParams) subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalInitGpuIntr_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalInitGpuIntr__
#define subdeviceCtrlCmdInternalInitGpuIntr(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalInitGpuIntr_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncOptimizeTiming_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncOptimizeTiming__
#define subdeviceCtrlCmdInternalGsyncOptimizeTiming(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncOptimizeTiming_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncGetDisplayIds_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncGetDisplayIds__
#define subdeviceCtrlCmdInternalGsyncGetDisplayIds(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncGetDisplayIds_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncSetStereoSync_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncSetStereoSync__
#define subdeviceCtrlCmdInternalGsyncSetStereoSync(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncSetStereoSync_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncGetVactiveLines_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncGetVactiveLines__
#define subdeviceCtrlCmdInternalGsyncGetVactiveLines(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncGetVactiveLines_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__
#define subdeviceCtrlCmdInternalGsyncIsDisplayIdValid(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__
#define subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar__
#define subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar(pSubdevice, pAttachParams) subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_DISPATCH(pSubdevice, pAttachParams)
#define subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode__
#define subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode(pSubdevice, pParams) subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalFbsrInit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalFbsrInit__
#define subdeviceCtrlCmdInternalFbsrInit(pSubdevice, pParams) subdeviceCtrlCmdInternalFbsrInit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPostInitBrightcStateLoad_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPostInitBrightcStateLoad__
#define subdeviceCtrlCmdInternalPostInitBrightcStateLoad(pSubdevice, pParams) subdeviceCtrlCmdInternalPostInitBrightcStateLoad_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSetStaticEdidData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalSetStaticEdidData__
#define subdeviceCtrlCmdInternalSetStaticEdidData(pSubdevice, pParams) subdeviceCtrlCmdInternalSetStaticEdidData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__
#define subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate(pSubdevice, pParams) subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__
#define subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync(pSubdevice, pParams) subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__
#define subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit(pSubdevice, pParams) subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__
#define subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2(pSubdevice, pParams) subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck__
#define subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck(pSubdevice) subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__
#define subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit(pSubdevice) subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalDetectHsVideoBridge_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalDetectHsVideoBridge__
#define subdeviceCtrlCmdInternalDetectHsVideoBridge(pSubdevice) subdeviceCtrlCmdInternalDetectHsVideoBridge_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalConfComputeGetStaticInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeGetStaticInfo__
#define subdeviceCtrlCmdInternalConfComputeGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__
#define subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__
#define subdeviceCtrlCmdInternalConfComputeDeriveLceKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeRotateKeys_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeRotateKeys__
#define subdeviceCtrlCmdInternalConfComputeRotateKeys(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeRotateKeys_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__
#define subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeSetGpuState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeSetGpuState__
#define subdeviceCtrlCmdInternalConfComputeSetGpuState(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeSetGpuState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__
#define subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy(pSubdevice, pParams) subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalInitUserSharedData_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalInitUserSharedData__
#define subdeviceCtrlCmdInternalInitUserSharedData(pSubdevice, pParams) subdeviceCtrlCmdInternalInitUserSharedData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__
#define subdeviceCtrlCmdInternalUserSharedDataSetDataPoll(pSubdevice, pParams) subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalControlGspTrace_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalControlGspTrace__
#define subdeviceCtrlCmdInternalControlGspTrace(pSubdevice, pParams) subdeviceCtrlCmdInternalControlGspTrace_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__
#define subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter(pSubdevice, pParams) subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGpuSetIllum_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalGpuSetIllum__
#define subdeviceCtrlCmdInternalGpuSetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdInternalGpuSetIllum_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdInternalLogOobXid_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdInternalLogOobXid__
#define subdeviceCtrlCmdInternalLogOobXid(pSubdevice, pParams) subdeviceCtrlCmdInternalLogOobXid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__
#define subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__
#define subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__
#define subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__
#define subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__
#define subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__
#define subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__
#define subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__
#define subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalFreeStates_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalFreeStates__
#define subdeviceCtrlCmdVgpuMgrInternalFreeStates(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalFreeStates_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus__
#define subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode__
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode__
#define subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode(pSubdevice, pParams) subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetAvailableHshubMask_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdGetAvailableHshubMask__
#define subdeviceCtrlCmdGetAvailableHshubMask(pSubdevice, pParams) subdeviceCtrlCmdGetAvailableHshubMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlSetEcThrottleMode_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlSetEcThrottleMode__
#define subdeviceCtrlSetEcThrottleMode(pSubdevice, pParams) subdeviceCtrlSetEcThrottleMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuMap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCcuMap__
#define subdeviceCtrlCmdCcuMap(pSubdevice, pParams) subdeviceCtrlCmdCcuMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuUnmap_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCcuUnmap__
#define subdeviceCtrlCmdCcuUnmap(pSubdevice, pParams) subdeviceCtrlCmdCcuUnmap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuSetStreamState_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCcuSetStreamState__
#define subdeviceCtrlCmdCcuSetStreamState(pSubdevice, pParams) subdeviceCtrlCmdCcuSetStreamState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuGetSampleInfo_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdCcuGetSampleInfo__
#define subdeviceCtrlCmdCcuGetSampleInfo(pSubdevice, pParams) subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdCcuGetSampleInfo_HAL(pSubdevice, pParams) subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSpdmPartition_FNPTR(pSubdevice) pSubdevice->__subdeviceCtrlCmdSpdmPartition__
#define subdeviceCtrlCmdSpdmPartition(pSubdevice, pSpdmPartitionParams) subdeviceCtrlCmdSpdmPartition_DISPATCH(pSubdevice, pSpdmPartitionParams)
#define subdeviceSpdmRetrieveTranscript_FNPTR(pSubdevice) pSubdevice->__subdeviceSpdmRetrieveTranscript__
#define subdeviceSpdmRetrieveTranscript(pSubdevice, pSpdmRetrieveSessionTranscriptParams) subdeviceSpdmRetrieveTranscript_DISPATCH(pSubdevice, pSpdmRetrieveSessionTranscriptParams)
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

static inline NV_STATUS subdeviceCtrlCmdBiosGetNbsiV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS *pNbsiParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetNbsiV2__(pSubdevice, pNbsiParams);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetSKUInfo__(pSubdevice, pBiosGetSKUInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetPostTime_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetPostTime__(pSubdevice, pBiosPostTime);
}

static inline NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetUefiSupport__(pSubdevice, pUEFIParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcGetArchInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetArchInfo__(pSubdevice, pArchInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcGetManufacturer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetManufacturer__(pSubdevice, pManufacturerParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams) {
    return pSubdevice->__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__(pSubdevice, pReplayableFaultOwnrshpParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcServiceInterrupts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams) {
    return pSubdevice->__subdeviceCtrlCmdMcServiceInterrupts__(pSubdevice, pServiceInterruptParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcGetEngineNotificationIntrVectors_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetEngineNotificationIntrVectors__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMcGetStaticIntrTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetStaticIntrTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDmaInvalidateTLB__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDmaGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdDmaGetInfo__(pSubdevice, pDmaInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPciInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPciInfo__(pSubdevice, pPciInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetInfoV2__(pSubdevice, pBusInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPciBarInfo__(pSubdevice, pBarInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieSpeed__(pSubdevice, pBusInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieLinkWidth__(pSubdevice, pLinkWidthParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusServiceGpuMultifunctionState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetBFD_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetBFD__(pSubdevice, pBusGetBFDParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetAspmDisableFlags__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusControlPublicAspmBits__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusClearPexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusClearPexCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexUtilCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusClearPexUtilCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusFreezePexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusFreezePexCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexLaneCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPcieLtrLatency__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieLtrLatency__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetEomParameters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetEomParameters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetEomStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetEomStatus__(pSubdevice, pParams);
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

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CErrorInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_C2C_ERR_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetC2CErrorInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetC2CLpwrStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetC2CLpwrStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetC2CLpwrStateVote_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetC2CLpwrStateVote__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSysmemAccess_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSysmemAccess__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusSetP2pMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetP2pMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusUnsetP2pMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusUnsetP2pMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedCounters_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetSupportedCounters__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSupportedBWMode_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetSupportedBWMode__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetBWMode_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetBWMode__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetBWMode_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkSetBWMode__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetLocalDeviceInfo__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdGetNvlinkCountersV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetNvlinkCountersV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdClearNvlinkCountersV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdClearNvlinkCountersV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetNvlinkCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetNvlinkCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdClearNvlinkCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdClearNvlinkCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkErrInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkErrInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdSetNvlinkHwErrorInjectSettings__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetNvlinkHwErrorInjectSettings__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkInjectSWError_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkInjectSWError__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkConfigureL1Toggle_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkConfigureL1Toggle__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlNvlinkGetL1Toggle_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlNvlinkGetL1Toggle__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinGetLinkFomValues_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinGetLinkFomValues__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkEccErrors_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetNvlinkEccErrors__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetupEom_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetupEom__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetPowerState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkReadTpCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLpCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkClearLpCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkClearLpCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetLoopbackMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetRefreshCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkClearRefreshCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEnableLinks__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkInbandSendData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkInbandSendData__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPostFaultUp_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkPostFaultUp__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPostLazyErrorRecovery_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkPostLazyErrorRecovery__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkEomControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEomControl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetL1Threshold_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetL1Threshold__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkDirectConnectCheck_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkDirectConnectCheck__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetL1Threshold_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetL1Threshold__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdIsNvlinkReducedConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdIsNvlinkReducedConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPortEvents_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetPortEvents__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkIsGpuDegraded_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkIsGpuDegraded__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPAOS_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PAOS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPAOS__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLTC_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLTC_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPLTC__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPLM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLC_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLC_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPSLC__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMCAM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MCAM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMCAM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTECR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTECR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTECR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEWE_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEWE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTEWE__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSDE_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSDE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTSDE__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTCAP_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTCAP_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTCAP__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMTU_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMTU_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMTU__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMLP_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMLP_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMLP__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessGHPKT_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_GHPKT_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessGHPKT__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPDDR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PDDR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPDDR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPTT_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPTT_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPTT__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPCNT_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPCNT_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPCNT__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMGIR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MGIR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMGIR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPAOS_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPAOS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPAOS__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPHCR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPHCR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPHCR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLTP_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLTP_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLTP__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPGUID_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PGUID_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPGUID__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRT_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRT_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPRT__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTYS_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTYS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPTYS__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLRG_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLRG_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLRG__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPMAOS_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PMAOS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPMAOS__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPLR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPLR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPLR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMORD_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MORD_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMORD__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CAP_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CAP__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CONF_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CONF__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTRC_CTRL_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTRC_CTRL__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTEIM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTEIM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTEIM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIE_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIE_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTIE__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTIM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTIM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTIM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMPSCR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MPSCR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMPSCR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMTSR_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MTSR_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMTSR__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPSLS_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPSLS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPSLS__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessMLPC_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_MLPC_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessMLPC__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPLIB_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PLIB_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPLIB__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkUpdateNvleTopology__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvleLids_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetNvleLids__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTASV2_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPTASV2__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRM_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccessPPRM__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkPRMAccess__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetPlatformInfo__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_DISPATCH(struct Subdevice *arg_this, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *arg2) {
    return arg_this->__subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo__(arg_this, arg2);
}

static inline NV_STATUS subdeviceCtrlCmdI2cReadBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cReadBuffer__(pSubdevice, pI2cParams);
}

static inline NV_STATUS subdeviceCtrlCmdI2cWriteBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cWriteBuffer__(pSubdevice, pI2cParams);
}

static inline NV_STATUS subdeviceCtrlCmdI2cReadReg_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cReadReg__(pSubdevice, pI2cParams);
}

static inline NV_STATUS subdeviceCtrlCmdI2cWriteReg_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cWriteReg__(pSubdevice, pI2cParams);
}

static inline NV_STATUS subdeviceCtrlCmdThermalSystemExecuteV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *pSystemExecuteParams) {
    return pSubdevice->__subdeviceCtrlCmdThermalSystemExecuteV2__(pSubdevice, pSystemExecuteParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfRatedTdpGetControl__(pSubdevice, pControlParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfRatedTdpSetControl__(pSubdevice, pControlParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfReservePerfmonHw__(pSubdevice, pPerfmonParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfSetAuxPowerState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS *pPowerStateParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfSetAuxPowerState__(pSubdevice, pPowerStateParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfSetPowerstate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS *pPowerInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfSetPowerstate__(pSubdevice, pPowerInfoParams);
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

static inline NV_STATUS subdeviceCtrlCmdKPerfBoost_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams) {
    return pSubdevice->__subdeviceCtrlCmdKPerfBoost__(pSubdevice, pBoostParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetFBRegionInfo__(pSubdevice, pGFBRIParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetBar1Offset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetBar1Offset__(pSubdevice, pFbMemParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbIsKind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams) {
    return pSubdevice->__subdeviceCtrlCmdFbIsKind__(pSubdevice, pIsKindParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetMemAlignment_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetMemAlignment__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetHeapReservationSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_GET_VGPU_CONFIG_HOST_RESERVED_FB_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalMemmgrGetVgpuHostRmReservedFb__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMMGR_MEMORY_TRANSFER_WITH_GSP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalMemmgrMemoryTransferWithGsp__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetInfoV2__(pSubdevice, pFbInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCalibrationLockFailed__(pSubdevice, pGCLFParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbFlushGpuCache_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams) {
    return pSubdevice->__subdeviceCtrlCmdFbFlushGpuCache__(pSubdevice, pCacheFlushParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetGpuCacheInfo__(pSubdevice, pGpuCacheParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__(pSubdevice, pOsOfflinedParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetOfflinedPages__(pSubdevice, pBlackListParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbSetupVprRegion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetupVprRegion__(pSubdevice, pCliReqParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetLTCInfoForFBP__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbPatchPbrForMining__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetRemappedRows_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetRemappedRows__(pSubdevice, pRemappedRowsParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetFsInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetFsInfo__(pSubdevice, pInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetRowRemapperHistogram__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbUpdateNumaStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetNumaInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetNumaInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetStaticConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysSetPartitionableMem__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetZbcReferenced__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysL2InvalidateEvict__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdMemSysDisableNvlinkPeers__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysProgramRawCompressionMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCtagsForCbcEviction__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbCBCOp_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbCBCOp__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbSetRrd_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetRrd__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbSetReadLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetReadLimit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbSetWriteLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetWriteLimit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGmmuCommitTlbInvalidate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGmmuCommitTlbInvalidate__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFbGetStaticBar1Info_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetStaticBar1Info__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_PENDING_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionPendingConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_DRAM_ENCRYPTION_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysSetDramEncryptionConfiguration__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_DRAM_ENCRYPTION_INFOROM_SUPPORT_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionInforomSupport__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdMemSysQueryDramEncryptionStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_QUERY_DRAM_ENCRYPTION_STATUS_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdMemSysQueryDramEncryptionStatus__(pSubdevice, pConfig);
}

static inline NV_STATUS subdeviceCtrlCmdSetGpfifo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams) {
    return pSubdevice->__subdeviceCtrlCmdSetGpfifo__(pSubdevice, pSetGpFifoParams);
}

static inline NV_STATUS subdeviceCtrlCmdSetOperationalProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties) {
    return pSubdevice->__subdeviceCtrlCmdSetOperationalProperties__(pSubdevice, pSetOperationalProperties);
}

static inline NV_STATUS subdeviceCtrlCmdFifoBindEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoBindEngines__(pSubdevice, pBindParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetPhysicalChannelCount__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetInfo__(pSubdevice, pFifoInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannels__(pSubdevice, pDisableChannelParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotation__(pSubdevice, pDisableChannelParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_FOR_KEY_ROTATION_V2_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannelsForKeyRotationV2__(pSubdevice, pDisableChannelParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableUsermodeChannels__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetChannelMemInfo__(pSubdevice, pChannelMemParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetUserdLocation__(pSubdevice, pUserdLocationParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedSwGetLog_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SW_GET_LOG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoObjschedSwGetLog__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoObjschedGetState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedSetState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_SET_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoObjschedSetState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoObjschedGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_OBJSCHED_GET_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoObjschedGetCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_CHANNEL_GROUP_UNIQUE_ID_INFO_PARAMS *pGetChannelGrpUidParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetChannelGroupUniqueIdInfo__(pSubdevice, pGetChannelGrpUidParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoQueryChannelUniqueId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_QUERY_CHANNEL_UNIQUE_ID_PARAMS *pQueryChannelUidParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoQueryChannelUniqueId__(pSubdevice, pQueryChannelUidParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetDeviceInfoTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoSetupVfZombieSubctxPdb__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoClearFaultedBit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__(pSubdevice, pSchedPolicyParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo) {
    return pSubdevice->__subdeviceCtrlCmdFifoUpdateChannelInfo__(pSubdevice, pChannelInfo);
}

static inline NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoGetNumChannels__(pSubdevice, pNumChannelsParams);
}

static inline NV_STATUS subdeviceCtrlCmdFifoGetAllocatedChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_ALLOCATED_CHANNELS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetAllocatedChannels__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalFifoGetNumSecureChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_SECURE_CHANNELS_PARAMS *pNumSecureChannelsParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoGetNumSecureChannels__(pSubdevice, pNumSecureChannelsParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *pToggleActiveChannelSchedulingParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoToggleActiveChannelScheduling__(pSubdevice, pToggleActiveChannelSchedulingParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetInfoV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCapsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCapsV2__(pSubdevice, pGrCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxswModes__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullBind__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetZcullInfo__(pSubdevice, pZcullInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPmMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPmBind__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswSetupBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SETUP_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswSetupBind__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetGpcTileMap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswSmpcMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrPcSamplingMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGlobalSmOrder__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPreemptionBind__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetROPInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetROPInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxswStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferPtes__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCurrentResidentChannel__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetVatAlarmData__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetAttributeBufferSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolQuerySize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolInitialize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolAddSlots__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetPpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetPpcMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetTpcPartitionMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetSmIssueRateModifier__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifierV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetSmIssueRateModifierV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetPhysGpcMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGpcMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetTpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetTpcMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetEngineContextProperties__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetNumTpcsForGpc__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGpcTileMap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetZcullMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GFX_GPC_AND_TPC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGfxGpcAndTpcInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifierV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrStaticGetFecsTraceDefines__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKGrInternalInitBug4208224War_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalInitBug4208224War__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetCachedInfo__(pSubdevice, pGpuInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInfoV2__(pSubdevice, pGpuInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetIpVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetIpVersion__(pSubdevice, pGpuIpVersionParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__(pSubdevice, pBridgeInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__(pSubdevice, pBridgeInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetOptimusInfo__(pSubdevice, pGpuOptimusInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNameString_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNameString__(pSubdevice, pNameStringParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetShortNameString_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetShortNameString__(pSubdevice, pShortNameStringParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEncoderCapacity__(pSubdevice, pEncoderCapacityParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__(pSubdevice, params);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__(pSubdevice, params);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetFabricAddr__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetEgmGpaFabricAddr__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetPower_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPower__(pSubdevice, pSetPowerParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetSdm_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetSdm__(pSubdevice, pSdmParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetSimulationInfo__(pSubdevice, pGpuSimulationInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngines__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEnginesV2__(pSubdevice, pEngineParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineClasslist__(pSubdevice, pClassParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEnginePartnerList__(pSubdevice, pPartnerListParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiGpcInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiTpcInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiZcullInfo__(pSubdevice, pGpuFermiZcullInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPesInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPesInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuExecRegOps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuExecRegOps__(pSubdevice, pRegParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuMigratableOps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pRegParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuMigratableOps__(pSubdevice, pRegParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryMode__(pSubdevice, pQueryMode);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInforomImageVersion__(pSubdevice, pVersionInfo);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInforomObjectVersion__(pSubdevice, pVersionInfo);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryInforomEccSupport__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryEccStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetChipDetails_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetChipDetails__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetOEMBoardInfo__(pSubdevice, pBoardInfo);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetOEMInfo__(pSubdevice, pOemInfo);
}

static inline NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuHandleGpuSR__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetComputeModeRules__(pSubdevice, pSetRulesParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryComputeModeRules__(pSubdevice, pQueryRulesParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuAcquireComputeModeReservation__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuReleaseComputeModeReservation__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuInitializeCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuInitializeCtx__(pSubdevice, pInitializeCtxParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuPromoteCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuPromoteCtx__(pSubdevice, pPromoteCtxParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuEvictCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuEvictCtx__(pSubdevice, pEvictCtxParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetId__(pSubdevice, pIdParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetGidInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetGidInfo__(pSubdevice, pGidInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryIllumSupport__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetIllum_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetIllum__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetIllum_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetIllum__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryScrubberStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetVprCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVprCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetVprInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVprInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPids_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPids__(pSubdevice, pGetPidsParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPidInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPidInfo__(pSubdevice, pGetPidInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryFunctionStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuReportNonReplayableFault__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineFaultInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetHwEngineId__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetFirstAsyncCEIdx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIRST_ASYNC_CE_IDX_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFirstAsyncCEIdx__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetVmmuSegmentSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVmmuSegmentSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuHandleVfPriFault_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuHandleVfPriFault__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetComputePolicyConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetComputePolicyConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdValidateMemMapRequest__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetGfid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GFID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetGfid__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdUpdateGfidP2pCapability_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdUpdateGfidP2pCapability__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineLoadTimes__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetP2pCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetP2pCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetGpuFabricProbeInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetGpuFabricProbeInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForReset_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuMarkDeviceForReset__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForReset_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuUnmarkDeviceForReset__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuMarkDeviceForDrainAndReset__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuUnmarkDeviceForDrainAndReset__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetResetStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RESET_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetResetStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetDrainAndResetStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_DRAIN_AND_RESET_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetDrainAndResetStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetConstructedFalconInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetConstructedFalconInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlGpuGetFipsStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FIPS_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlGpuGetFipsStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetVfCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VF_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVfCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetRecoveryAction_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetRecoveryAction__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuRpcGspTest_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuRpcGspTest__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuRpcGspQuerySizes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RPC_GSP_QUERY_SIZES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuRpcGspQuerySizes__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdRusdGetSupportedFeatures_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RUSD_GET_SUPPORTED_FEATURES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdRusdGetSupportedFeatures__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetTrigger_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdEventSetTrigger__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetTriggerFifo__(pSubdevice, pTriggerFifoParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetNotification_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetNotification__(pSubdevice, pSetEventParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetMemoryNotifies__(pSubdevice, pSetMemoryNotifiesParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetSemaphoreMemory__(pSubdevice, pSetSemMemoryParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetSemaMemValidation__(pSubdevice, pSetSemaMemValidationParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventVideoBindEvtbuf_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_PARAMS *pBindParams) {
    return pSubdevice->__subdeviceCtrlCmdEventVideoBindEvtbuf__(pSubdevice, pBindParams);
}

static inline NV_STATUS subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_RATS_GSP_TRACE_BIND_EVTBUF_PARAMS *pBindParams) {
    return pSubdevice->__subdeviceCtrlCmdEventGspTraceRatsBindEvtbuf__(pSubdevice, pBindParams);
}

static inline NV_STATUS subdeviceCtrlCmdTimerCancel_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdTimerCancel__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdTimerSchedule_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerSchedule__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdTimerGetTime_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetTime__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetRegisterOffset__(pSubdevice, pTimerRegOffsetParams);
}

static inline NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerSetGrTickFreq__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdRcReadVirtualMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam) {
    return pSubdevice->__subdeviceCtrlCmdRcReadVirtualMem__(pSubdevice, pReadVirtMemParam);
}

static inline NV_STATUS subdeviceCtrlCmdRcGetErrorCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount) {
    return pSubdevice->__subdeviceCtrlCmdRcGetErrorCount__(pSubdevice, pErrorCount);
}

static inline NV_STATUS subdeviceCtrlCmdRcGetErrorV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams) {
    return pSubdevice->__subdeviceCtrlCmdRcGetErrorV2__(pSubdevice, pErrorParams);
}

static inline NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcSetCleanErrorHistory__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdRcGetWatchdogInfo__(pSubdevice, pWatchdogInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdRcDisableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcDisableWatchdog__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcSoftDisableWatchdog__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdRcEnableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcEnableWatchdog__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcReleaseWatchdogRequests__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalRcWatchdogTimeout__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdSetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdSetRcRecovery__(pSubdevice, pRcRecovery);
}

static inline NV_STATUS subdeviceCtrlCmdGetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdGetRcRecovery__(pSubdevice, pRcRecovery);
}

static inline NV_STATUS subdeviceCtrlCmdGetRcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetRcInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdSetRcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdSetRcInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvdGetDumpSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetDumpSize__(pSubdevice, pDumpSizeParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvdGetDump_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetDump__(pSubdevice, pDumpParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetNocatJournalRpt__(pSubdevice, pReportParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdSetNocatJournalData__(pSubdevice, pReportParams);
}

static inline NV_STATUS subdeviceCtrlCmdNvdInsertNocatJournalRecord_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS *pReportParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdInsertNocatJournalRecord__(pSubdevice, pReportParams);
}

static inline NV_STATUS subdeviceCtrlCmdPmgrGetModuleInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PMGR_MODULE_INFO_PARAMS *pModuleInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdPmgrGetModuleInfo__(pSubdevice, pModuleInfoParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuProcessPostGc6ExitTasks__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdGc6Entry_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGc6Entry__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGc6Exit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GC6_EXIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGc6Exit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdLpwrDifrPrefetchResponse_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdLpwrDifrPrefetchResponse__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdLpwrDifrCtrl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdLpwrDifrCtrl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetCePceMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCePceMask__(pSubdevice, pCePceMaskParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *pCeUpdatePceLceMappingsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeUpdatePceLceMappings__(pSubdevice, pCeUpdatePceLceMappingsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappingsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS *pCeUpdatePceLceMappingsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeUpdatePceLceMappingsV2__(pSubdevice, pCeUpdatePceLceMappingsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetLceShimInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetLceShimInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetPceConfigForLceType_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetPceConfigForLceType__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetCapsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCapsV2__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetAllCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetAllCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetDecompLceMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetDecompLceMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeIsDecompLceEnabled_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeIsDecompLceEnabled__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetDmemUsage__(pSubdevice, pFlcnDmemUsageParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetEngineArch__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerQueueInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlGet__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlSet__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferSize__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdEccGetClientExposedCounters__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdEccGetVolatileCounts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdEccGetVolatileCounts__(pSubdevice, pParams);
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

static inline NV_STATUS subdeviceCtrlCmdFlaRange_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaRange__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaSetupInstanceMemBlock__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlaGetRange_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaGetRange__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaGetFabricMemStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGspGetFeatures_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams) {
    return pSubdevice->__subdeviceCtrlCmdGspGetFeatures__(pSubdevice, pGspFeaturesParams);
}

static inline NV_STATUS subdeviceCtrlCmdGspGetRmHeapStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_RM_HEAP_STATS_PARAMS *pGspRmHeapStatsParams) {
    return pSubdevice->__subdeviceCtrlCmdGspGetRmHeapStats__(pSubdevice, pGspRmHeapStatsParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetVgpuHeapStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVgpuHeapStats__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdLibosGetHeapStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *pGspLibosHeapStatsParams) {
    return pSubdevice->__subdeviceCtrlCmdLibosGetHeapStats__(pSubdevice, pGspLibosHeapStatsParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetActivePartitionIds__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPartitionCapacity__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuDescribePartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuDescribePartitions__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPartitioningMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrmgrGetGrFsInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPartitions__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPartitions__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetComputeProfiles_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetComputeProfiles__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGpuGetComputeProfileCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_PROFILE_CAPACITY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetComputeProfileCapacity__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_COMPUTE_PROFILES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetComputeInstanceProfiles__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_PROMOTE_GPU_INSTANCE_MEM_RANGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrPromoteGpuInstanceMemRange__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixAudioDynamicPower__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixUpdateTgpStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetIpVersion__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetStaticInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetChannelPushbuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayWriteInstMem__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetupRgLineIntr__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetImportedImpData__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetDisplayMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayPinsetsToLockpins_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayPinsetsToLockpins__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetSliLinkGpioSwControl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpioProgramDirection_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpioProgramDirection__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpioProgramOutput_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpioProgramOutput__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpioReadInput_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpioReadInput__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpioActivateHwFunction_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpioActivateHwFunction__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalDisplayAcpiSubsytemActivated__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalDisplayPreModeSet_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalDisplayPreModeSet__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalDisplayPostModeSet_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalDisplayPostModeSet__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayPreUnixConsole_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_PRE_UNIX_CONSOLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayPreUnixConsole__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdDisplayPostUnixConsole_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_DISPLAY_POST_UNIX_CONSOLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayPostUnixConsole__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetChipInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetChipInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetDeviceInfoTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalRecoverAllComputeContexts__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetSmcMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetSmcMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdIsEgpuBridge_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_EGPU_BRIDGE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdIsEgpuBridge__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_GSP_RM_FREE_HEAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpuGetGspRmFreeHeap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusFlushWithSysmembar__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpuGetPFBar1Spa_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_PF_BAR1_SPA_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpuGetPFBar1Spa__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_VMMU_GET_SPA_FOR_GPA_ENTRIES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalVmmuGetSpaForGpaEntries__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGmmuGetStaticInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_UNREGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_COPY_RESERVED_SPLIT_GVASPACE_PDES_TO_SERVER_PARAMS *pCopyServerReservedPdesParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuCopyReservedSplitGVASpacePdesServer__(pSubdevice, pCopyServerReservedPdesParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetPhysicalCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetAllPhysicalCaps__(pSubdevice, pCeCapsParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeUpdateClassDB_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdCeUpdateClassDB__(pSubdevice, params);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdCeGetFaultMethodBufferSize__(pSubdevice, params);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetHubPceMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetHubPceMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCeGetHubPceMaskV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetHubPceMaskV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdIntrGetKernelTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdIntrGetKernelTable__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_2x__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_3x__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostClear_3x__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfGetAuxPowerState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfGetAuxPowerState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGcxEntryPrerequisite_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GCX_ENTRY_PREREQUISITE_PARAMS *pGcxEntryPrerequisite) {
    return pSubdevice->__subdeviceCtrlCmdInternalGcxEntryPrerequisite__(pSubdevice, pGcxEntryPrerequisite);
}

static inline NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifGetStaticInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifGetAspmL1Flags__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBifSetPcieRo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifSetPcieRo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdBifDisableSystemMemoryAccess_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifDisableSystemMemoryAccess__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubPeerConnConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubGetHshubIdForLinks__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubGetNumUnits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubGetNumUnits__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubNextHshubId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubNextHshubId__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubGetMaxHshubsPerShim_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubGetMaxHshubsPerShim__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdHshubEgmConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_EGM_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubEgmConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetNumActiveLinksPerIoctrl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetTotalNumLinksPerIoctrl__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkPostFatalErrorRecovery__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableNvlinkPeer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkCoreCallback_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_CORE_CALLBACK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkCoreCallback__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateRemoteLocalSid__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetAliEnabled_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_ALI_ENABLED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetAliEnabled__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkSaveRestoreHshubState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramBufferready_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkProgramBufferready__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkReplaySuppressedErrors__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateCurrentConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdatePeerLinkMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateLinkConnection__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkPreSetupNvlinkPeer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkPostSetupNvlinkPeer__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkUpdateHshubMux_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkUpdateHshubMux__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkRemoveNvlinkMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableLinksPostTopology__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkPreLinkTrainAli__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetLinkMaskPostRxDet__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkLinkTrainAli_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkLinkTrainAli__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetNvlinkDeviceInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetIoctrlDeviceInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkProgramLinkSpeed__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkAreLinksTrained_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkAreLinksTrained__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkResetLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkResetLinks__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkDisableDlInterrupts__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetLinkAndClockInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkSetupNvlinkSysmem__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkProcessForcedConfigs__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkSyncLaneShutdownProps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableSysmemNvlinkAts__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkHshubGetSysmemNvlinkMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetP2pCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalSetP2pCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalRemoveP2pCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalRemoveP2pCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetPcieP2pCaps__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGetLocalAtsConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetLocalAtsConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetPeerAtsConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalSetPeerAtsConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalInitGpuIntr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalInitGpuIntr__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncOptimizeTiming_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncOptimizeTiming__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetDisplayIds_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncGetDisplayIds__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncSetStereoSync_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncSetStereoSync__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetVactiveLines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncGetVactiveLines__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncIsDisplayIdValid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncIsDisplayIdValid__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncSetOrRestoreGpioRasterSync__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_APPLY_STEREO_PIN_ALWAYS_HI_WAR_PARAMS *pAttachParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncApplyStereoPinAlwaysHiWar__(pSubdevice, pAttachParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGsyncGetRasterSyncDecodeMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalFbsrInit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FBSR_INIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFbsrInit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPostInitBrightcStateLoad_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPostInitBrightcStateLoad__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalSetStaticEdidData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalSetStaticEdidData__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalSmbpbiPfmReqHndlrCapUpdate__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrStateLoadSync__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalThermPfmReqHndlrStateInitSync__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetPm1State__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetPm1State__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrUpdateEdppLimit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrGetEdppLimitInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalThermPfmReqHndlrUpdateTgpuLimit__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTgpMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrPfmReqHndlrConfigureTurboV2__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrGetVpstateMapping__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrSetVpstate__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPfmReqHndlrDependencyCheck__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalPmgrUnsetDynamicBoostLimit__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalDetectHsVideoBridge_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalDetectHsVideoBridge__(pSubdevice);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeGetStaticInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeDeriveSwlKeys__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeDeriveLceKeys_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeDeriveLceKeys__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeRotateKeys_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_ROTATE_KEYS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeRotateKeys__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CONF_COMPUTE_RC_CHANNELS_FOR_KEY_ROTATION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeRCChannelsForKeyRotation__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeSetGpuState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_GPU_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeSetGpuState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_SET_SECURITY_POLICY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalConfComputeSetSecurityPolicy__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalInitUserSharedData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INIT_USER_SHARED_DATA_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalInitUserSharedData__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalUserSharedDataSetDataPoll_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_USER_SHARED_DATA_SET_DATA_POLL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalUserSharedDataSetDataPoll__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalControlGspTrace_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalControlGspTrace__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpuClientLowPowerModeEnter__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalGpuSetIllum_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_SET_ILLUM_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGpuSetIllum__(pSubdevice, pConfigParams);
}

static inline NV_STATUS subdeviceCtrlCmdInternalLogOobXid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalLogOobXid__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalBootloadGspVgpuPluginTask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalShutdownGspVgpuPluginTask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalPgpuAddVgpuType__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalEnumerateVgpuPerPgpu__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalClearGuestVmInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetVgpuFbUsage__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuEncoderCapacity__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalCleanupGspVgpuPluginResources__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuFsEncoding__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetPgpuMigrationSupport__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMgrConfig__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalFreeStates_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalFreeStates__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_GET_FRAME_RATE_LIMITER_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalGetFrameRateLimiterStatus__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuHeterogeneousMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdVgpuMgrInternalSetVgpuMigTimesliceMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetAvailableHshubMask__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlSetEcThrottleMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_SET_EC_THROTTLE_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlSetEcThrottleMode__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCcuMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCcuMap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCcuUnmap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCcuUnmap__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCcuSetStreamState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCcuSetStreamState__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdCcuGetSampleInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCcuGetSampleInfo__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdSpdmPartition_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams) {
    return pSubdevice->__subdeviceCtrlCmdSpdmPartition__(pSubdevice, pSpdmPartitionParams);
}

static inline NV_STATUS subdeviceSpdmRetrieveTranscript_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *pSpdmRetrieveSessionTranscriptParams) {
    return pSubdevice->__subdeviceSpdmRetrieveTranscript__(pSubdevice, pSpdmRetrieveSessionTranscriptParams);
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

NV_STATUS subdeviceCtrlCmdNvlinkUpdateNvleTopology_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleLids_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPTASV2_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PTASV2_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessSLLM_5NM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_SLLM_5NM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccessPPRM_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PPRM_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkPRMAccess_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetPlatformInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *arg2);

NV_STATUS subdeviceCtrlCmdNvlinkGetNvleEncryptEnInfo_IMPL(struct Subdevice *arg1, NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS *arg2);

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

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceHwEnable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalGetFecsTraceRdOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrInternalSetFecsTraceWrOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGrStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdKGrInternalInitBug4208224War_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *pParams);

NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);

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

NV_STATUS subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS *pParams);

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

NV_STATUS subdeviceCtrlCmdGpuProcessPostGc6ExitTasks_IMPL(struct Subdevice *pSubdevice);

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
#else //__nvoc_subdevice_h_disabled
#define subdeviceUnsetDynamicBoostLimit(pSubdevice) subdeviceUnsetDynamicBoostLimit_IMPL(pSubdevice)
#endif //__nvoc_subdevice_h_disabled

void subdeviceRestoreGrTickFreq_IMPL(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext);

#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceRestoreGrTickFreq(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceRestoreGrTickFreq(pSubdevice, pCallContext) subdeviceRestoreGrTickFreq_IMPL(pSubdevice, pCallContext)
#endif //__nvoc_subdevice_h_disabled

void subdeviceRestoreWatchdog_IMPL(struct Subdevice *pSubdevice);

#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceRestoreWatchdog(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceRestoreWatchdog(pSubdevice) subdeviceRestoreWatchdog_IMPL(pSubdevice)
#endif //__nvoc_subdevice_h_disabled

void subdeviceUnsetGpuDebugMode_IMPL(struct Subdevice *pSubdevice);

#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceUnsetGpuDebugMode(struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceUnsetGpuDebugMode(pSubdevice) subdeviceUnsetGpuDebugMode_IMPL(pSubdevice)
#endif //__nvoc_subdevice_h_disabled

void subdeviceReleaseComputeModeReservation_IMPL(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext);

#ifdef __nvoc_subdevice_h_disabled
static inline void subdeviceReleaseComputeModeReservation(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceReleaseComputeModeReservation(pSubdevice, pCallContext) subdeviceReleaseComputeModeReservation_IMPL(pSubdevice, pCallContext)
#endif //__nvoc_subdevice_h_disabled

NV_STATUS subdeviceGetByHandle_IMPL(struct RsClient *pClient, NvHandle hSubdevice, struct Subdevice **ppSubdevice);

#define subdeviceGetByHandle(pClient, hSubdevice, ppSubdevice) subdeviceGetByHandle_IMPL(pClient, hSubdevice, ppSubdevice)
NV_STATUS subdeviceGetByGpu_IMPL(struct RsClient *pClient, struct OBJGPU *pGpu, struct Subdevice **ppSubdevice);

#define subdeviceGetByGpu(pClient, pGpu, ppSubdevice) subdeviceGetByGpu_IMPL(pClient, pGpu, ppSubdevice)
NV_STATUS subdeviceGetByDeviceAndGpu_IMPL(struct RsClient *pClient, struct Device *pDevice, struct OBJGPU *pGpu, struct Subdevice **ppSubdevice);

#define subdeviceGetByDeviceAndGpu(pClient, pDevice, pGpu, ppSubdevice) subdeviceGetByDeviceAndGpu_IMPL(pClient, pDevice, pGpu, ppSubdevice)
NV_STATUS subdeviceGetByInstance_IMPL(struct RsClient *pClient, NvHandle hDevice, NvU32 subDeviceInst, struct Subdevice **ppSubdevice);

#define subdeviceGetByInstance(pClient, hDevice, subDeviceInst, ppSubdevice) subdeviceGetByInstance_IMPL(pClient, hDevice, subDeviceInst, ppSubdevice)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SUBDEVICE_NVOC_H_
