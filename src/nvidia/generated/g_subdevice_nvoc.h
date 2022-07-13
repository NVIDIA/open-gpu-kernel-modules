#ifndef _G_SUBDEVICE_NVOC_H_
#define _G_SUBDEVICE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl2080.h" // rmcontrol parameters

#ifndef NV2080_NOTIFIERS_CE_IDX
// TODO these need to be moved to cl2080.h
#define NV2080_NOTIFIERS_CE_IDX(i) ((i) - NV2080_NOTIFIERS_CE0)
#define NV2080_NOTIFIERS_NVENC_IDX(i) ((i) - NV2080_NOTIFIERS_NVENC0)
#define NV2080_NOTIFIERS_NVDEC_IDX(i) ((i) - NV2080_NOTIFIERS_NVDEC0)
#define NV2080_NOTIFIERS_GR_IDX(i) ((i) - NV2080_NOTIFIERS_GR0)
#endif

#define NV2080_ENGINE_RANGE_GR()    rangeMake(NV2080_ENGINE_TYPE_GR(0), NV2080_ENGINE_TYPE_GR(NV2080_ENGINE_TYPE_GR_SIZE - 1))
#define NV2080_ENGINE_RANGE_COPY()  rangeMake(NV2080_ENGINE_TYPE_COPY(0), NV2080_ENGINE_TYPE_COPY(NV2080_ENGINE_TYPE_COPY_SIZE - 1))
#define NV2080_ENGINE_RANGE_NVDEC() rangeMake(NV2080_ENGINE_TYPE_NVDEC(0), NV2080_ENGINE_TYPE_NVDEC(NV2080_ENGINE_TYPE_NVDEC_SIZE - 1))
#define NV2080_ENGINE_RANGE_NVENC() rangeMake(NV2080_ENGINE_TYPE_NVENC(0), NV2080_ENGINE_TYPE_NVENC(NV2080_ENGINE_TYPE_NVENC_SIZE - 1))
#define NV2080_ENGINE_RANGE_NVJPEG() rangeMake(NV2080_ENGINE_TYPE_NVJPEG(0), NV2080_ENGINE_TYPE_NVJPEG(NV2080_ENGINE_TYPE_NVJPEG_SIZE - 1))

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
#ifdef NVOC_SUBDEVICE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Subdevice {
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
    struct Subdevice *__nvoc_pbase_Subdevice;
    void (*__subdevicePreDestruct__)(struct Subdevice *);
    NV_STATUS (*__subdeviceInternalControlForward__)(struct Subdevice *, NvU32, void *, NvU32);
    NV_STATUS (*__subdeviceControlFilter__)(struct Subdevice *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__subdeviceCtrlCmdBiosGetInfoV2__)(struct Subdevice *, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBiosGetSKUInfo__)(struct Subdevice *, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBiosGetPostTime__)(struct Subdevice *, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBiosGetUefiSupport__)(struct Subdevice *, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcGetArchInfo__)(struct Subdevice *, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcGetManufacturer__)(struct Subdevice *, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcQueryHostclkSlowdownStatus__)(struct Subdevice *, NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcSetHostclkSlowdownStatus__)(struct Subdevice *, NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__)(struct Subdevice *, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMcServiceInterrupts__)(struct Subdevice *, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDmaInvalidateTLB__)(struct Subdevice *, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDmaGetInfo__)(struct Subdevice *, NV2080_CTRL_DMA_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPciInfo__)(struct Subdevice *, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetInfo__)(struct Subdevice *, NV2080_CTRL_BUS_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetInfoV2__)(struct Subdevice *, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPciBarInfo__)(struct Subdevice *, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieSpeed__)(struct Subdevice *, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieLinkWidth__)(struct Subdevice *, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed__)(struct Subdevice *, NV2080_CTRL_BUS_SET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed__)(struct Subdevice *, NV2080_CTRL_BUS_GET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusHWBCGetUpstreamBAR0__)(struct Subdevice *, NV2080_CTRL_BUS_HWBC_GET_UPSTREAM_BAR0_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusServiceGpuMultifunctionState__)(struct Subdevice *, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexCounters__)(struct Subdevice *, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetBFD__)(struct Subdevice *, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetAspmDisableFlags__)(struct Subdevice *, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusControlPublicAspmBits__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusClearPexCounters__)(struct Subdevice *, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexUtilCounters__)(struct Subdevice *, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusClearPexUtilCounters__)(struct Subdevice *, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusFreezePexCounters__)(struct Subdevice *, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPexLaneCounters__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetPcieLtrLatency__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSetPcieLtrLatency__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__)(struct Subdevice *, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSetEomParameters__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__)(struct Subdevice *, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetEomStatus__)(struct Subdevice *, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusSysmemAccess__)(struct Subdevice *, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkCaps__)(struct Subdevice *, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBusGetNvlinkStatus__)(struct Subdevice *, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGetNvlinkCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdClearNvlinkCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetupEom__)(struct Subdevice *, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetPowerState__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkReadTpCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLpCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEnableNvlinkPeer__)(struct Subdevice *, NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkCoreCallback__)(struct Subdevice *, NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetLoopbackMode__)(struct Subdevice *, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid__)(struct Subdevice *, NV2080_CTRL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdateHshubMux__)(struct Subdevice *, NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer__)(struct Subdevice *, NV2080_CTRL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer__)(struct Subdevice *, NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkRemoveNvlinkMapping__)(struct Subdevice *, NV2080_CTRL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSaveRestoreHshubState__)(struct Subdevice *, NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkProgramBufferready__)(struct Subdevice *, NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdateCurrentConfig__)(struct Subdevice *, NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdatePeerLinkMask__)(struct Subdevice *, NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkUpdateLinkConnection__)(struct Subdevice *, NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEnableLinksPostTopology__)(struct Subdevice *, NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetRefreshCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkClearRefreshCounters__)(struct Subdevice *, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkProgramLinkSpeed__)(struct Subdevice *, NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkAreLinksTrained__)(struct Subdevice *, NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkResetLinks__)(struct Subdevice *, NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkDisableDlInterrupts__)(struct Subdevice *, NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetLinkAndClockInfo__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSetupNvlinkSysmem__)(struct Subdevice *, NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkProcessForcedConfigs__)(struct Subdevice *, NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSyncLaneShutdownProps__)(struct Subdevice *, NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts__)(struct Subdevice *, NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask__)(struct Subdevice *, NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__)(struct Subdevice *, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__)(struct Subdevice *, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkEnableLinks__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__)(struct Subdevice *, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdI2cReadBuffer__)(struct Subdevice *, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdI2cWriteBuffer__)(struct Subdevice *, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdI2cReadReg__)(struct Subdevice *, NV2080_CTRL_I2C_RW_REG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdI2cWriteReg__)(struct Subdevice *, NV2080_CTRL_I2C_RW_REG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples__)(struct Subdevice *, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *);
    NV_STATUS (*__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__)(struct Subdevice *, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdPerfRatedTdpGetControl__)(struct Subdevice *, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdPerfRatedTdpSetControl__)(struct Subdevice *, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdPerfReservePerfmonHw__)(struct Subdevice *, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKPerfBoost__)(struct Subdevice *, NV2080_CTRL_PERF_BOOST_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetFBRegionInfo__)(struct Subdevice *, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetBar1Offset__)(struct Subdevice *, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbIsKind__)(struct Subdevice *, NV2080_CTRL_FB_IS_KIND_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetMemAlignment__)(struct Subdevice *, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetHeapReservationSize__)(struct Subdevice *, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetInfo__)(struct Subdevice *, NV2080_CTRL_FB_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetInfoV2__)(struct Subdevice *, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetCarveoutAddressInfo__)(struct Subdevice *, NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetCalibrationLockFailed__)(struct Subdevice *, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbFlushGpuCache__)(struct Subdevice *, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetGpuCacheAllocPolicy__)(struct Subdevice *, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetGpuCacheAllocPolicy__)(struct Subdevice *, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2__)(struct Subdevice *, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2__)(struct Subdevice *, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetGpuCacheInfo__)(struct Subdevice *, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__)(struct Subdevice *, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetOfflinedPages__)(struct Subdevice *, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetupVprRegion__)(struct Subdevice *, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetLTCInfoForFBP__)(struct Subdevice *, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__)(struct Subdevice *, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbPatchPbrForMining__)(struct Subdevice *, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetRemappedRows__)(struct Subdevice *, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetFsInfo__)(struct Subdevice *, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetRowRemapperHistogram__)(struct Subdevice *, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetDynamicOfflinedPages__)(struct Subdevice *, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbUpdateNumaStatus__)(struct Subdevice *, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetNumaInfo__)(struct Subdevice *, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetStaticConfig__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysSetPartitionableMem__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetZbcReferenced__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysL2InvalidateEvict__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysDisableNvlinkPeers__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysProgramRawCompressionMode__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbGetCtagsForCbcEviction__)(struct Subdevice *, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbCBCOp__)(struct Subdevice *, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetRrd__)(struct Subdevice *, NV2080_CTRL_FB_SET_RRD_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetReadLimit__)(struct Subdevice *, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFbSetWriteLimit__)(struct Subdevice *, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdSetGpfifo__)(struct Subdevice *, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdSetOperationalProperties__)(struct Subdevice *, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoBindEngines__)(struct Subdevice *, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGetPhysicalChannelCount__)(struct Subdevice *, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoGetInfo__)(struct Subdevice *, NV2080_CTRL_FIFO_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableChannels__)(struct Subdevice *, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoDisableUsermodeChannels__)(struct Subdevice *, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoGetChannelMemInfo__)(struct Subdevice *, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoGetUserdLocation__)(struct Subdevice *, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoGetDeviceInfoTable__)(struct Subdevice *, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoClearFaultedBit__)(struct Subdevice *, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__)(struct Subdevice *, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFifoUpdateChannelInfo__)(struct Subdevice *, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__)(struct Subdevice *, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalFifoGetNumChannels__)(struct Subdevice *, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetInfo__)(struct Subdevice *, NV2080_CTRL_GR_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetInfoV2__)(struct Subdevice *, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCapsV2__)(struct Subdevice *, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxswModes__)(struct Subdevice *, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswZcullMode__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswZcullBind__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetZcullInfo__)(struct Subdevice *, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPmMode__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPmBind__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrSetGpcTileMap__)(struct Subdevice *, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswSmpcMode__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrPcSamplingMode__)(struct Subdevice *, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__)(struct Subdevice *, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGlobalSmOrder__)(struct Subdevice *, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__)(struct Subdevice *, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrCtxswPreemptionBind__)(struct Subdevice *, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetROPInfo__)(struct Subdevice *, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxswStats__)(struct Subdevice *, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferSize__)(struct Subdevice *, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferInfo__)(struct Subdevice *, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCtxBufferPtes__)(struct Subdevice *, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetCurrentResidentChannel__)(struct Subdevice *, NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetVatAlarmData__)(struct Subdevice *, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetAttributeBufferSize__)(struct Subdevice *, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolQuerySize__)(struct Subdevice *, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolInitialize__)(struct Subdevice *, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolAddSlots__)(struct Subdevice *, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__)(struct Subdevice *, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetPpcMask__)(struct Subdevice *, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrSetTpcPartitionMode__)(struct Subdevice *, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetSmIssueRateModifier__)(struct Subdevice *, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__)(struct Subdevice *, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__)(struct Subdevice *, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetPhysGpcMask__)(struct Subdevice *, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGpcMask__)(struct Subdevice *, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetTpcMask__)(struct Subdevice *, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetEngineContextProperties__)(struct Subdevice *, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetNumTpcsForGpc__)(struct Subdevice *, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetGpcTileMap__)(struct Subdevice *, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrGetZcullMask__)(struct Subdevice *, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetCaps__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetCachedInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInfoV2__)(struct Subdevice *, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetIpVersion__)(struct Subdevice *, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetOptimusInfo__)(struct Subdevice *, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNameString__)(struct Subdevice *, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetShortNameString__)(struct Subdevice *, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEncoderCapacity__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvencSwSessionStats__)(struct Subdevice *, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__)(struct Subdevice *, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetFabricAddr__)(struct Subdevice *, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPower__)(struct Subdevice *, NV2080_CTRL_GPU_SET_POWER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetSdm__)(struct Subdevice *, NV2080_CTRL_GPU_GET_SDM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetSdm__)(struct Subdevice *, NV2080_CTRL_GPU_SET_SDM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetSimulationInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngines__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEnginesV2__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineClasslist__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEnginePartnerList__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiGpcInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiTpcInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetFermiZcullInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPesInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuExecRegOps__)(struct Subdevice *, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryMode__)(struct Subdevice *, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInforomImageVersion__)(struct Subdevice *, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetInforomObjectVersion__)(struct Subdevice *, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryInforomEccSupport__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccStatus__)(struct Subdevice *, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetOEMBoardInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetOEMInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuHandleGpuSR__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetComputeModeRules__)(struct Subdevice *, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryComputeModeRules__)(struct Subdevice *, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuAcquireComputeModeReservation__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdGpuReleaseComputeModeReservation__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdGpuInitializeCtx__)(struct Subdevice *, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuPromoteCtx__)(struct Subdevice *, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuEvictCtx__)(struct Subdevice *, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetId__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetGidInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryIllumSupport__)(struct Subdevice *, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetIllum__)(struct Subdevice *, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetIllum__)(struct Subdevice *, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryScrubberStatus__)(struct Subdevice *, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVprCaps__)(struct Subdevice *, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetVprInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPids__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PIDS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPidInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuInterruptFunction__)(struct Subdevice *, NV2080_CTRL_GPU_INTERRUPT_FUNCTION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryFunctionStatus__)(struct Subdevice *, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuReportNonReplayableFault__)(struct Subdevice *, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineFaultInfo__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetHwEngineId__)(struct Subdevice *, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__)(struct Subdevice *, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetComputePolicyConfig__)(struct Subdevice *, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetComputePolicyConfig__)(struct Subdevice *, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdValidateMemMapRequest__)(struct Subdevice *, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetEngineLoadTimes__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetTrigger__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetTriggerFifo__)(struct Subdevice *, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetNotification__)(struct Subdevice *, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetMemoryNotifies__)(struct Subdevice *, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetSemaphoreMemory__)(struct Subdevice *, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEventSetSemaMemValidation__)(struct Subdevice *, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdTimerCancel__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdTimerSchedule__)(struct Subdevice *, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdTimerGetTime__)(struct Subdevice *, NV2080_CTRL_TIMER_GET_TIME_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdTimerGetRegisterOffset__)(struct Subdevice *, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__)(struct Subdevice *, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdTimerSetGrTickFreq__)(struct Subdevice *, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdRcReadVirtualMem__)(struct Subdevice *, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdRcGetErrorCount__)(struct Subdevice *, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdRcGetErrorV2__)(struct Subdevice *, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdRcSetCleanErrorHistory__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdRcGetWatchdogInfo__)(struct Subdevice *, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdRcDisableWatchdog__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdRcSoftDisableWatchdog__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdRcEnableWatchdog__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdRcReleaseWatchdogRequests__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalRcWatchdogTimeout__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdSetRcRecovery__)(struct Subdevice *, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGetRcRecovery__)(struct Subdevice *, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGetRcInfo__)(struct Subdevice *, NV2080_CTRL_CMD_RC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdSetRcInfo__)(struct Subdevice *, NV2080_CTRL_CMD_RC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvdGetDumpSize__)(struct Subdevice *, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvdGetDump__)(struct Subdevice *, NV2080_CTRL_NVD_GET_DUMP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvdGetNocatJournalRpt__)(struct Subdevice *, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdNvdSetNocatJournalData__)(struct Subdevice *, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetCaps__)(struct Subdevice *, NV2080_CTRL_CE_GET_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetCapsV2__)(struct Subdevice *, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetAllCaps__)(struct Subdevice *, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetCePceMask__)(struct Subdevice *, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeUpdatePceLceMappings__)(struct Subdevice *, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetDmemUsage__)(struct Subdevice *, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationMap__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationUnmap__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationGetInfo__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationGetControl__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationSetControl__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnInstrumentationRecalibrate__)(struct Subdevice *, NV2080_CTRL_FLCN_INSTRUMENTATION_RECALIBRATE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetEngineArch__)(struct Subdevice *, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerQueueInfo__)(struct Subdevice *, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerControlGet__)(struct Subdevice *, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnUstreamerControlSet__)(struct Subdevice *, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetCtxBufferInfo__)(struct Subdevice *, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlcnGetCtxBufferSize__)(struct Subdevice *, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdEccGetClientExposedCounters__)(struct Subdevice *, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuQueryEccConfiguration__)(struct Subdevice *, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetEccConfiguration__)(struct Subdevice *, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuResetEccErrorStatus__)(struct Subdevice *, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlaRange__)(struct Subdevice *, NV2080_CTRL_FLA_RANGE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlaSetupInstanceMemBlock__)(struct Subdevice *, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlaGetRange__)(struct Subdevice *, NV2080_CTRL_FLA_GET_RANGE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdFlaGetFabricMemStats__)(struct Subdevice *, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGspGetFeatures__)(struct Subdevice *, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetActivePartitionIds__)(struct Subdevice *, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPartitionCapacity__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuDescribePartitions__)(struct Subdevice *, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPartitioningMode__)(struct Subdevice *, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGrmgrGetGrFsInfo__)(struct Subdevice *, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuSetPartitions__)(struct Subdevice *, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGpuGetPartitions__)(struct Subdevice *, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__)(struct Subdevice *, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__)(struct Subdevice *, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__)(struct Subdevice *, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__)(struct Subdevice *, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__)(struct Subdevice *, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdOsUnixAudioDynamicPower__)(struct Subdevice *, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__)(struct Subdevice *, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdOsUnixUpdateTgpStatus__)(struct Subdevice *, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetIpVersion__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetStaticInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetChannelPushbuffer__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplayWriteInstMem__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetupRgLineIntr__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplaySetImportedImpData__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdDisplayGetDisplayMask__)(struct Subdevice *, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdMsencGetCaps__)(struct Subdevice *, NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__)(struct Subdevice *, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize__)(struct Subdevice *, NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetChipInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetDeviceInfoTable__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetConstructedFalconInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalRecoverAllComputeContexts__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetSmcMode__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusBindLocalGfidForP2p__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusFlushWithSysmembar__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__)(struct Subdevice *, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__)(struct Subdevice *, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGmmuGetStaticInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetPhysicalCaps__)(struct Subdevice *, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetAllPhysicalCaps__)(struct Subdevice *, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeUpdateClassDB__)(struct Subdevice *, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetFaultMethodBufferSize__)(struct Subdevice *, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdCeGetHubPceMask__)(struct Subdevice *, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdIntrGetKernelTable__)(struct Subdevice *, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfCudaLimitDisable__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfOptpCliClear__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostSet_2x__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostSet_3x__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfBoostClear_3x__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__)(struct Subdevice *, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBifGetStaticInfo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBifGetAspmL1Flags__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdBifSetPcieRo__)(struct Subdevice *, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdHshubPeerConnConfig__)(struct Subdevice *, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdHshubFirstLinkPeerId__)(struct Subdevice *, NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdHshubGetHshubIdForLinks__)(struct Subdevice *, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdHshubGetNumUnits__)(struct Subdevice *, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdHshubNextHshubId__)(struct Subdevice *, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__)(struct Subdevice *);
    NV_STATUS (*__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__)(struct Subdevice *, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetPcieP2pCaps__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdInternalGetCoherentFbApertureSize__)(struct Subdevice *, NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS *);
    NV_STATUS (*__subdeviceCtrlCmdGetAvailableHshubMask__)(struct Subdevice *, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *);
    NvBool (*__subdeviceShareCallback__)(struct Subdevice *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__subdeviceMapTo__)(struct Subdevice *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__subdeviceGetOrAllocNotifShare__)(struct Subdevice *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__subdeviceCheckMemInterUnmap__)(struct Subdevice *, NvBool);
    NV_STATUS (*__subdeviceGetMapAddrSpace__)(struct Subdevice *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__subdeviceSetNotificationShare__)(struct Subdevice *, struct NotifShare *);
    NvU32 (*__subdeviceGetRefCount__)(struct Subdevice *);
    void (*__subdeviceAddAdditionalDependants__)(struct RsClient *, struct Subdevice *, RsResourceRef *);
    NV_STATUS (*__subdeviceControl_Prologue__)(struct Subdevice *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__subdeviceGetRegBaseOffsetAndSize__)(struct Subdevice *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__subdeviceUnmapFrom__)(struct Subdevice *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__subdeviceControl_Epilogue__)(struct Subdevice *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__subdeviceControlLookup__)(struct Subdevice *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__subdeviceGetInternalObjectHandle__)(struct Subdevice *);
    NV_STATUS (*__subdeviceControl__)(struct Subdevice *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__subdeviceUnmap__)(struct Subdevice *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__subdeviceGetMemInterMapParams__)(struct Subdevice *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__subdeviceGetMemoryMappingDescriptor__)(struct Subdevice *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__subdeviceUnregisterEvent__)(struct Subdevice *, NvHandle, NvHandle, NvHandle, NvHandle);
    NvBool (*__subdeviceCanCopy__)(struct Subdevice *);
    PEVENTNOTIFICATION *(*__subdeviceGetNotificationListPtr__)(struct Subdevice *);
    struct NotifShare *(*__subdeviceGetNotificationShare__)(struct Subdevice *);
    NV_STATUS (*__subdeviceMap__)(struct Subdevice *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__subdeviceAccessCallback__)(struct Subdevice *, struct RsClient *, void *, RsAccessRight);
    NvU32 deviceInst;
    NvU32 subDeviceInst;
    struct Device *pDevice;
    NvBool bMaxGrTickFreqRequested;
    PNODE pP2PMappingList;
    NvU64 P2PfbMappedBytes;
    NvU32 notifyActions[165];
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
    NvU32 perfBoostRefCount;
    NvBool perfBoostEntryExists;
    NvBool bLockedClockModeRequested;
    NvU32 bNvlinkErrorInjectionModeRequested;
    NvBool bSchedPolicySet;
    NvBool bGcoffDisallowed;
    NvBool bUpdateTGP;
};

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice;

#define __staticCast_Subdevice(pThis) \
    ((pThis)->__nvoc_pbase_Subdevice)

#ifdef __nvoc_subdevice_h_disabled
#define __dynamicCast_Subdevice(pThis) ((Subdevice*)NULL)
#else //__nvoc_subdevice_h_disabled
#define __dynamicCast_Subdevice(pThis) \
    ((Subdevice*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Subdevice)))
#endif //__nvoc_subdevice_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Subdevice(Subdevice**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Subdevice(Subdevice**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Subdevice(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Subdevice((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define subdevicePreDestruct(pResource) subdevicePreDestruct_DISPATCH(pResource)
#define subdeviceInternalControlForward(pSubdevice, command, pParams, size) subdeviceInternalControlForward_DISPATCH(pSubdevice, command, pParams, size)
#define subdeviceControlFilter(pSubdevice, pCallContext, pParams) subdeviceControlFilter_DISPATCH(pSubdevice, pCallContext, pParams)
#define subdeviceCtrlCmdBiosGetInfoV2(pSubdevice, pBiosInfoParams) subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(pSubdevice, pBiosInfoParams)
#define subdeviceCtrlCmdBiosGetSKUInfo(pSubdevice, pBiosGetSKUInfoParams) subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(pSubdevice, pBiosGetSKUInfoParams)
#define subdeviceCtrlCmdBiosGetPostTime(pSubdevice, pBiosPostTime) subdeviceCtrlCmdBiosGetPostTime_DISPATCH(pSubdevice, pBiosPostTime)
#define subdeviceCtrlCmdBiosGetUefiSupport(pSubdevice, pUEFIParams) subdeviceCtrlCmdBiosGetUefiSupport_DISPATCH(pSubdevice, pUEFIParams)
#define subdeviceCtrlCmdMcGetArchInfo(pSubdevice, pArchInfoParams) subdeviceCtrlCmdMcGetArchInfo_DISPATCH(pSubdevice, pArchInfoParams)
#define subdeviceCtrlCmdMcGetManufacturer(pSubdevice, pManufacturerParams) subdeviceCtrlCmdMcGetManufacturer_DISPATCH(pSubdevice, pManufacturerParams)
#define subdeviceCtrlCmdMcQueryHostclkSlowdownStatus(pSubdevice, pGetStatusParams) subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_DISPATCH(pSubdevice, pGetStatusParams)
#define subdeviceCtrlCmdMcSetHostclkSlowdownStatus(pSubdevice, pParams) subdeviceCtrlCmdMcSetHostclkSlowdownStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMcChangeReplayableFaultOwnership(pSubdevice, pReplayableFaultOwnrshpParams) subdeviceCtrlCmdMcChangeReplayableFaultOwnership_DISPATCH(pSubdevice, pReplayableFaultOwnrshpParams)
#define subdeviceCtrlCmdMcServiceInterrupts(pSubdevice, pServiceInterruptParams) subdeviceCtrlCmdMcServiceInterrupts_DISPATCH(pSubdevice, pServiceInterruptParams)
#define subdeviceCtrlCmdDmaInvalidateTLB(pSubdevice, pParams) subdeviceCtrlCmdDmaInvalidateTLB_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDmaGetInfo(pSubdevice, pDmaInfoParams) subdeviceCtrlCmdDmaGetInfo_DISPATCH(pSubdevice, pDmaInfoParams)
#define subdeviceCtrlCmdBusGetPciInfo(pSubdevice, pPciInfoParams) subdeviceCtrlCmdBusGetPciInfo_DISPATCH(pSubdevice, pPciInfoParams)
#define subdeviceCtrlCmdBusGetInfo(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusGetInfo_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusGetInfoV2(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusGetInfoV2_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusGetPciBarInfo(pSubdevice, pBarInfoParams) subdeviceCtrlCmdBusGetPciBarInfo_DISPATCH(pSubdevice, pBarInfoParams)
#define subdeviceCtrlCmdBusSetPcieSpeed(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusSetPcieSpeed_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusSetPcieLinkWidth(pSubdevice, pLinkWidthParams) subdeviceCtrlCmdBusSetPcieLinkWidth_DISPATCH(pSubdevice, pLinkWidthParams)
#define subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusHWBCGetUpstreamBAR0(pSubdevice, pBusInfoParams) subdeviceCtrlCmdBusHWBCGetUpstreamBAR0_DISPATCH(pSubdevice, pBusInfoParams)
#define subdeviceCtrlCmdBusServiceGpuMultifunctionState(pSubdevice, pParams) subdeviceCtrlCmdBusServiceGpuMultifunctionState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetBFD(pSubdevice, pBusGetBFDParams) subdeviceCtrlCmdBusGetBFD_DISPATCH(pSubdevice, pBusGetBFDParams)
#define subdeviceCtrlCmdBusGetAspmDisableFlags(pSubdevice, pParams) subdeviceCtrlCmdBusGetAspmDisableFlags_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusControlPublicAspmBits(pSubdevice, pParams) subdeviceCtrlCmdBusControlPublicAspmBits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusClearPexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexUtilCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusClearPexUtilCounters(pSubdevice, pParams) subdeviceCtrlCmdBusClearPexUtilCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusFreezePexCounters(pSubdevice, pParams) subdeviceCtrlCmdBusFreezePexCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPexLaneCounters(pSubdevice, pParams) subdeviceCtrlCmdBusGetPexLaneCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusGetPcieLtrLatency_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetPcieLtrLatency(pSubdevice, pParams) subdeviceCtrlCmdBusSetPcieLtrLatency_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkPeerIdMask(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkPeerIdMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSetEomParameters(pSubdevice, pParams) subdeviceCtrlCmdBusSetEomParameters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetUphyDlnCfgSpace(pSubdevice, pParams) subdeviceCtrlCmdBusGetUphyDlnCfgSpace_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetEomStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetEomStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusSysmemAccess(pSubdevice, pParams) subdeviceCtrlCmdBusSysmemAccess_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkCaps(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBusGetNvlinkStatus(pSubdevice, pParams) subdeviceCtrlCmdBusGetNvlinkStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdGetNvlinkCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdClearNvlinkCounters(pSubdevice, pParams) subdeviceCtrlCmdClearNvlinkCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetupEom(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetupEom_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetPowerState(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetPowerState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkReadTpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkReadTpCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLpCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLpCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkEnableNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdNvlinkEnableNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkCoreCallback(pSubdevice, pParams) subdeviceCtrlCmdNvlinkCoreCallback_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetLoopbackMode(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetLoopbackMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid(pSubdevice, pParams) subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkUpdateHshubMux(pSubdevice, pParams) subdeviceCtrlCmdNvlinkUpdateHshubMux_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer(pSubdevice, pParams) subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkRemoveNvlinkMapping(pSubdevice, pParams) subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSaveRestoreHshubState(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSaveRestoreHshubState_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkProgramBufferready(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProgramBufferready_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkUpdateCurrentConfig(pSubdevice, pParams) subdeviceCtrlCmdNvlinkUpdateCurrentConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkUpdatePeerLinkMask(pSubdevice, pParams) subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkUpdateLinkConnection(pSubdevice, pParams) subdeviceCtrlCmdNvlinkUpdateLinkConnection_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkEnableLinksPostTopology(pSubdevice, pParams) subdeviceCtrlCmdNvlinkEnableLinksPostTopology_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetRefreshCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkClearRefreshCounters(pSubdevice, pParams) subdeviceCtrlCmdNvlinkClearRefreshCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkProgramLinkSpeed(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProgramLinkSpeed_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkAreLinksTrained(pSubdevice, pParams) subdeviceCtrlCmdNvlinkAreLinksTrained_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkResetLinks(pSubdevice, pParams) subdeviceCtrlCmdNvlinkResetLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkDisableDlInterrupts(pSubdevice, pParams) subdeviceCtrlCmdNvlinkDisableDlInterrupts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetLinkAndClockInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSetupNvlinkSysmem(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkProcessForcedConfigs(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProcessForcedConfigs_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSyncLaneShutdownProps(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts(pSubdevice, pParams) subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask(pSubdevice, pParams) subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr(pSubdevice, pParams) subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo(pSubdevice, pParams) subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvlinkEnableLinks(pSubdevice) subdeviceCtrlCmdNvlinkEnableLinks_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdNvlinkProcessInitDisabledLinks(pSubdevice, pParams) subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdI2cReadBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadBuffer_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cWriteBuffer(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteBuffer_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cReadReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cReadReg_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdI2cWriteReg(pSubdevice, pI2cParams) subdeviceCtrlCmdI2cWriteReg_DISPATCH(pSubdevice, pI2cParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2(pSubdevice, pParams) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdPerfRatedTdpGetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpGetControl_DISPATCH(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfRatedTdpSetControl(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH(pSubdevice, pControlParams)
#define subdeviceCtrlCmdPerfReservePerfmonHw(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(pSubdevice, pPerfmonParams)
#define subdeviceCtrlCmdKPerfBoost(pSubdevice, pBoostParams) subdeviceCtrlCmdKPerfBoost_DISPATCH(pSubdevice, pBoostParams)
#define subdeviceCtrlCmdFbGetFBRegionInfo(pSubdevice, pGFBRIParams) subdeviceCtrlCmdFbGetFBRegionInfo_DISPATCH(pSubdevice, pGFBRIParams)
#define subdeviceCtrlCmdFbGetBar1Offset(pSubdevice, pFbMemParams) subdeviceCtrlCmdFbGetBar1Offset_DISPATCH(pSubdevice, pFbMemParams)
#define subdeviceCtrlCmdFbIsKind(pSubdevice, pIsKindParams) subdeviceCtrlCmdFbIsKind_DISPATCH(pSubdevice, pIsKindParams)
#define subdeviceCtrlCmdFbGetMemAlignment(pSubdevice, pParams) subdeviceCtrlCmdFbGetMemAlignment_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetHeapReservationSize(pSubdevice, pParams) subdeviceCtrlCmdFbGetHeapReservationSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetInfo(pSubdevice, pFbInfoParams) subdeviceCtrlCmdFbGetInfo_DISPATCH(pSubdevice, pFbInfoParams)
#define subdeviceCtrlCmdFbGetInfoV2(pSubdevice, pFbInfoParams) subdeviceCtrlCmdFbGetInfoV2_DISPATCH(pSubdevice, pFbInfoParams)
#define subdeviceCtrlCmdFbGetCarveoutAddressInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetCarveoutAddressInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetCalibrationLockFailed(pSubdevice, pGCLFParams) subdeviceCtrlCmdFbGetCalibrationLockFailed_DISPATCH(pSubdevice, pGCLFParams)
#define subdeviceCtrlCmdFbFlushGpuCache(pSubdevice, pCacheFlushParams) subdeviceCtrlCmdFbFlushGpuCache_DISPATCH(pSubdevice, pCacheFlushParams)
#define subdeviceCtrlCmdFbSetGpuCacheAllocPolicy(pSubdevice, pParams) subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetGpuCacheAllocPolicy(pSubdevice, pGpuCacheAllocPolicyParams) subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_DISPATCH(pSubdevice, pGpuCacheAllocPolicyParams)
#define subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2(pSubdevice, pParams) subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2(pSubdevice, pParams) subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetGpuCacheInfo(pSubdevice, pGpuCacheParams) subdeviceCtrlCmdFbGetGpuCacheInfo_DISPATCH(pSubdevice, pGpuCacheParams)
#define subdeviceCtrlCmdFbGetCliManagedOfflinedPages(pSubdevice, pOsOfflinedParams) subdeviceCtrlCmdFbGetCliManagedOfflinedPages_DISPATCH(pSubdevice, pOsOfflinedParams)
#define subdeviceCtrlCmdFbGetOfflinedPages(pSubdevice, pBlackListParams) subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(pSubdevice, pBlackListParams)
#define subdeviceCtrlCmdFbSetupVprRegion(pSubdevice, pCliReqParams) subdeviceCtrlCmdFbSetupVprRegion_DISPATCH(pSubdevice, pCliReqParams)
#define subdeviceCtrlCmdFbGetLTCInfoForFBP(pSubdevice, pParams) subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetCompBitCopyConstructInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbPatchPbrForMining(pSubdevice, pParams) subdeviceCtrlCmdFbPatchPbrForMining_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetRemappedRows(pSubdevice, pRemappedRowsParams) subdeviceCtrlCmdFbGetRemappedRows_DISPATCH(pSubdevice, pRemappedRowsParams)
#define subdeviceCtrlCmdFbGetFsInfo(pSubdevice, pInfoParams) subdeviceCtrlCmdFbGetFsInfo_DISPATCH(pSubdevice, pInfoParams)
#define subdeviceCtrlCmdFbGetRowRemapperHistogram(pSubdevice, pParams) subdeviceCtrlCmdFbGetRowRemapperHistogram_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetDynamicOfflinedPages(pSubdevice, pParams) subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbUpdateNumaStatus(pSubdevice, pParams) subdeviceCtrlCmdFbUpdateNumaStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetNumaInfo(pSubdevice, pParams) subdeviceCtrlCmdFbGetNumaInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetStaticConfig(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysSetPartitionableMem(pSubdevice, pParams) subdeviceCtrlCmdMemSysSetPartitionableMem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKMemSysGetMIGMemoryConfig(pSubdevice, pParams) subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetZbcReferenced(pSubdevice, pParams) subdeviceCtrlCmdFbSetZbcReferenced_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysL2InvalidateEvict(pSubdevice, pParams) subdeviceCtrlCmdMemSysL2InvalidateEvict_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches(pSubdevice) subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdMemSysDisableNvlinkPeers(pSubdevice) subdeviceCtrlCmdMemSysDisableNvlinkPeers_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdMemSysProgramRawCompressionMode(pSubdevice, pParams) subdeviceCtrlCmdMemSysProgramRawCompressionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable(pSubdevice, pParams) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbGetCtagsForCbcEviction(pSubdevice, pParams) subdeviceCtrlCmdFbGetCtagsForCbcEviction_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbCBCOp(pSubdevice, pParams) subdeviceCtrlCmdFbCBCOp_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetRrd(pSubdevice, pParams) subdeviceCtrlCmdFbSetRrd_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetReadLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetReadLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFbSetWriteLimit(pSubdevice, pParams) subdeviceCtrlCmdFbSetWriteLimit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSetGpfifo(pSubdevice, pSetGpFifoParams) subdeviceCtrlCmdSetGpfifo_DISPATCH(pSubdevice, pSetGpFifoParams)
#define subdeviceCtrlCmdSetOperationalProperties(pSubdevice, pSetOperationalProperties) subdeviceCtrlCmdSetOperationalProperties_DISPATCH(pSubdevice, pSetOperationalProperties)
#define subdeviceCtrlCmdFifoBindEngines(pSubdevice, pBindParams) subdeviceCtrlCmdFifoBindEngines_DISPATCH(pSubdevice, pBindParams)
#define subdeviceCtrlCmdGetPhysicalChannelCount(pSubdevice, pParams) subdeviceCtrlCmdGetPhysicalChannelCount_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetInfo(pSubdevice, pFifoInfoParams) subdeviceCtrlCmdFifoGetInfo_DISPATCH(pSubdevice, pFifoInfoParams)
#define subdeviceCtrlCmdFifoDisableChannels(pSubdevice, pDisableChannelParams) subdeviceCtrlCmdFifoDisableChannels_DISPATCH(pSubdevice, pDisableChannelParams)
#define subdeviceCtrlCmdFifoDisableUsermodeChannels(pSubdevice, pParams) subdeviceCtrlCmdFifoDisableUsermodeChannels_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoGetChannelMemInfo(pSubdevice, pChannelMemParams) subdeviceCtrlCmdFifoGetChannelMemInfo_DISPATCH(pSubdevice, pChannelMemParams)
#define subdeviceCtrlCmdFifoGetUserdLocation(pSubdevice, pUserdLocationParams) subdeviceCtrlCmdFifoGetUserdLocation_DISPATCH(pSubdevice, pUserdLocationParams)
#define subdeviceCtrlCmdFifoGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoClearFaultedBit(pSubdevice, pParams) subdeviceCtrlCmdFifoClearFaultedBit_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFifoRunlistSetSchedPolicy(pSubdevice, pSchedPolicyParams) subdeviceCtrlCmdFifoRunlistSetSchedPolicy_DISPATCH(pSubdevice, pSchedPolicyParams)
#define subdeviceCtrlCmdFifoUpdateChannelInfo(pSubdevice, pChannelInfo) subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(pSubdevice, pChannelInfo)
#define subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers(pSubdevice, pParams) subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalFifoGetNumChannels(pSubdevice, pNumChannelsParams) subdeviceCtrlCmdInternalFifoGetNumChannels_DISPATCH(pSubdevice, pNumChannelsParams)
#define subdeviceCtrlCmdKGrGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetInfoV2(pSubdevice, pParams) subdeviceCtrlCmdKGrGetInfoV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCapsV2(pSubdevice, pGrCapsParams) subdeviceCtrlCmdKGrGetCapsV2_DISPATCH(pSubdevice, pGrCapsParams)
#define subdeviceCtrlCmdKGrGetCtxswModes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswModes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswZcullMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswZcullBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswZcullBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetZcullInfo(pSubdevice, pZcullInfoParams) subdeviceCtrlCmdKGrGetZcullInfo_DISPATCH(pSubdevice, pZcullInfoParams)
#define subdeviceCtrlCmdKGrCtxswPmMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPmBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPmBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrSetGpcTileMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswSmpcMode(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswSmpcMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrPcSamplingMode(pSubdevice, pParams) subdeviceCtrlCmdKGrPcSamplingMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetSmToGpcTpcMappings(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGlobalSmOrder_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetCtxswPreemptionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetCtxswPreemptionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrCtxswPreemptionBind(pSubdevice, pParams) subdeviceCtrlCmdKGrCtxswPreemptionBind_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetROPInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetROPInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxswStats(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxswStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCtxBufferPtes(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCtxBufferPtes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetCurrentResidentChannel(pSubdevice, pParams) subdeviceCtrlCmdKGrGetCurrentResidentChannel_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetVatAlarmData(pSubdevice, pParams) subdeviceCtrlCmdKGrGetVatAlarmData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetAttributeBufferSize(pSubdevice, pParams) subdeviceCtrlCmdKGrGetAttributeBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolQuerySize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolQuerySize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolInitialize(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolInitialize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolAddSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolAddSlots_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGfxPoolRemoveSlots(pSubdevice, pParams) subdeviceCtrlCmdKGrGfxPoolRemoveSlots_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetPpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrSetTpcPartitionMode(pSubdevice, pParams) subdeviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrGetSmIssueRateModifier_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUid(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUid_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2(pSubdevice, pParams) subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetPhysGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetPhysGpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetTpcMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetTpcMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetEngineContextProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrGetEngineContextProperties_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetNumTpcsForGpc(pSubdevice, pParams) subdeviceCtrlCmdKGrGetNumTpcsForGpc_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetGpcTileMap(pSubdevice, pParams) subdeviceCtrlCmdKGrGetGpcTileMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrGetZcullMask(pSubdevice, pParams) subdeviceCtrlCmdKGrGetZcullMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetCaps(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetPpcMasks(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetZcullInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetRopInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetRopInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdKGrInternalStaticGetPdbProperties(pSubdevice, pParams) subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetCachedInfo(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetCachedInfo_DISPATCH(pSubdevice, pGpuInfoParams)
#define subdeviceCtrlCmdGpuGetInfoV2(pSubdevice, pGpuInfoParams) subdeviceCtrlCmdGpuGetInfoV2_DISPATCH(pSubdevice, pGpuInfoParams)
#define subdeviceCtrlCmdGpuGetIpVersion(pSubdevice, pGpuIpVersionParams) subdeviceCtrlCmdGpuGetIpVersion_DISPATCH(pSubdevice, pGpuIpVersionParams)
#define subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_DISPATCH(pSubdevice, pBridgeInfoParams)
#define subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu(pSubdevice, pBridgeInfoParams) subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_DISPATCH(pSubdevice, pBridgeInfoParams)
#define subdeviceCtrlCmdGpuSetOptimusInfo(pSubdevice, pGpuOptimusInfoParams) subdeviceCtrlCmdGpuSetOptimusInfo_DISPATCH(pSubdevice, pGpuOptimusInfoParams)
#define subdeviceCtrlCmdGpuGetNameString(pSubdevice, pNameStringParams) subdeviceCtrlCmdGpuGetNameString_DISPATCH(pSubdevice, pNameStringParams)
#define subdeviceCtrlCmdGpuGetShortNameString(pSubdevice, pShortNameStringParams) subdeviceCtrlCmdGpuGetShortNameString_DISPATCH(pSubdevice, pShortNameStringParams)
#define subdeviceCtrlCmdGpuGetEncoderCapacity(pSubdevice, pEncoderCapacityParams) subdeviceCtrlCmdGpuGetEncoderCapacity_DISPATCH(pSubdevice, pEncoderCapacityParams)
#define subdeviceCtrlCmdGpuGetNvencSwSessionStats(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetNvencSwSessionInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetNvencSwSessionInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionStats(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo(pSubdevice, params) subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdGpuSetFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdGpuSetFabricAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPower(pSubdevice, pSetPowerParams) subdeviceCtrlCmdGpuSetPower_DISPATCH(pSubdevice, pSetPowerParams)
#define subdeviceCtrlCmdGpuGetSdm(pSubdevice, pSdmParams) subdeviceCtrlCmdGpuGetSdm_DISPATCH(pSubdevice, pSdmParams)
#define subdeviceCtrlCmdGpuSetSdm(pSubdevice, pSdmParams) subdeviceCtrlCmdGpuSetSdm_DISPATCH(pSubdevice, pSdmParams)
#define subdeviceCtrlCmdGpuGetSimulationInfo(pSubdevice, pGpuSimulationInfoParams) subdeviceCtrlCmdGpuGetSimulationInfo_DISPATCH(pSubdevice, pGpuSimulationInfoParams)
#define subdeviceCtrlCmdGpuGetEngines(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEnginesV2(pSubdevice, pEngineParams) subdeviceCtrlCmdGpuGetEnginesV2_DISPATCH(pSubdevice, pEngineParams)
#define subdeviceCtrlCmdGpuGetEngineClasslist(pSubdevice, pClassParams) subdeviceCtrlCmdGpuGetEngineClasslist_DISPATCH(pSubdevice, pClassParams)
#define subdeviceCtrlCmdGpuGetEnginePartnerList(pSubdevice, pPartnerListParams) subdeviceCtrlCmdGpuGetEnginePartnerList_DISPATCH(pSubdevice, pPartnerListParams)
#define subdeviceCtrlCmdGpuGetFermiGpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiGpcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetFermiTpcInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetFermiTpcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetFermiZcullInfo(pSubdevice, pGpuFermiZcullInfoParams) subdeviceCtrlCmdGpuGetFermiZcullInfo_DISPATCH(pSubdevice, pGpuFermiZcullInfoParams)
#define subdeviceCtrlCmdGpuGetPesInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPesInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuExecRegOps(pSubdevice, pRegParams) subdeviceCtrlCmdGpuExecRegOps_DISPATCH(pSubdevice, pRegParams)
#define subdeviceCtrlCmdGpuQueryMode(pSubdevice, pQueryMode) subdeviceCtrlCmdGpuQueryMode_DISPATCH(pSubdevice, pQueryMode)
#define subdeviceCtrlCmdGpuGetInforomImageVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomImageVersion_DISPATCH(pSubdevice, pVersionInfo)
#define subdeviceCtrlCmdGpuGetInforomObjectVersion(pSubdevice, pVersionInfo) subdeviceCtrlCmdGpuGetInforomObjectVersion_DISPATCH(pSubdevice, pVersionInfo)
#define subdeviceCtrlCmdGpuQueryInforomEccSupport(pSubdevice) subdeviceCtrlCmdGpuQueryInforomEccSupport_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuQueryEccStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetOEMBoardInfo(pSubdevice, pBoardInfo) subdeviceCtrlCmdGpuGetOEMBoardInfo_DISPATCH(pSubdevice, pBoardInfo)
#define subdeviceCtrlCmdGpuGetOEMInfo(pSubdevice, pOemInfo) subdeviceCtrlCmdGpuGetOEMInfo_DISPATCH(pSubdevice, pOemInfo)
#define subdeviceCtrlCmdGpuHandleGpuSR(pSubdevice) subdeviceCtrlCmdGpuHandleGpuSR_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuSetComputeModeRules(pSubdevice, pSetRulesParams) subdeviceCtrlCmdGpuSetComputeModeRules_DISPATCH(pSubdevice, pSetRulesParams)
#define subdeviceCtrlCmdGpuQueryComputeModeRules(pSubdevice, pQueryRulesParams) subdeviceCtrlCmdGpuQueryComputeModeRules_DISPATCH(pSubdevice, pQueryRulesParams)
#define subdeviceCtrlCmdGpuAcquireComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuAcquireComputeModeReservation_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuReleaseComputeModeReservation(pSubdevice) subdeviceCtrlCmdGpuReleaseComputeModeReservation_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdGpuInitializeCtx(pSubdevice, pInitializeCtxParams) subdeviceCtrlCmdGpuInitializeCtx_DISPATCH(pSubdevice, pInitializeCtxParams)
#define subdeviceCtrlCmdGpuPromoteCtx(pSubdevice, pPromoteCtxParams) subdeviceCtrlCmdGpuPromoteCtx_DISPATCH(pSubdevice, pPromoteCtxParams)
#define subdeviceCtrlCmdGpuEvictCtx(pSubdevice, pEvictCtxParams) subdeviceCtrlCmdGpuEvictCtx_DISPATCH(pSubdevice, pEvictCtxParams)
#define subdeviceCtrlCmdGpuGetId(pSubdevice, pIdParams) subdeviceCtrlCmdGpuGetId_DISPATCH(pSubdevice, pIdParams)
#define subdeviceCtrlCmdGpuGetGidInfo(pSubdevice, pGidInfoParams) subdeviceCtrlCmdGpuGetGidInfo_DISPATCH(pSubdevice, pGidInfoParams)
#define subdeviceCtrlCmdGpuQueryIllumSupport(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuGetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuGetIllum_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuSetIllum(pSubdevice, pConfigParams) subdeviceCtrlCmdGpuSetIllum_DISPATCH(pSubdevice, pConfigParams)
#define subdeviceCtrlCmdGpuQueryScrubberStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVprCaps(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetVprInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetVprInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPids(pSubdevice, pGetPidsParams) subdeviceCtrlCmdGpuGetPids_DISPATCH(pSubdevice, pGetPidsParams)
#define subdeviceCtrlCmdGpuGetPidInfo(pSubdevice, pGetPidInfoParams) subdeviceCtrlCmdGpuGetPidInfo_DISPATCH(pSubdevice, pGetPidInfoParams)
#define subdeviceCtrlCmdGpuInterruptFunction(pSubdevice, pParams) subdeviceCtrlCmdGpuInterruptFunction_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryFunctionStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuQueryFunctionStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuReportNonReplayableFault(pSubdevice, pParams) subdeviceCtrlCmdGpuReportNonReplayableFault_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineFaultInfo(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineFaultInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineRunlistPriBase(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineRunlistPriBase_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetHwEngineId(pSubdevice, pParams) subdeviceCtrlCmdGpuGetHwEngineId_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetMaxSupportedPageSize(pSubdevice, pParams) subdeviceCtrlCmdGpuGetMaxSupportedPageSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuSetComputePolicyConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetComputePolicyConfig(pSubdevice, pParams) subdeviceCtrlCmdGpuGetComputePolicyConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdValidateMemMapRequest(pSubdevice, pParams) subdeviceCtrlCmdValidateMemMapRequest_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetEngineLoadTimes(pSubdevice, pParams) subdeviceCtrlCmdGpuGetEngineLoadTimes_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdEventSetTrigger(pSubdevice) subdeviceCtrlCmdEventSetTrigger_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdEventSetTriggerFifo(pSubdevice, pTriggerFifoParams) subdeviceCtrlCmdEventSetTriggerFifo_DISPATCH(pSubdevice, pTriggerFifoParams)
#define subdeviceCtrlCmdEventSetNotification(pSubdevice, pSetEventParams) subdeviceCtrlCmdEventSetNotification_DISPATCH(pSubdevice, pSetEventParams)
#define subdeviceCtrlCmdEventSetMemoryNotifies(pSubdevice, pSetMemoryNotifiesParams) subdeviceCtrlCmdEventSetMemoryNotifies_DISPATCH(pSubdevice, pSetMemoryNotifiesParams)
#define subdeviceCtrlCmdEventSetSemaphoreMemory(pSubdevice, pSetSemMemoryParams) subdeviceCtrlCmdEventSetSemaphoreMemory_DISPATCH(pSubdevice, pSetSemMemoryParams)
#define subdeviceCtrlCmdEventSetSemaMemValidation(pSubdevice, pSetSemaMemValidationParams) subdeviceCtrlCmdEventSetSemaMemValidation_DISPATCH(pSubdevice, pSetSemaMemValidationParams)
#define subdeviceCtrlCmdTimerCancel(pSubdevice) subdeviceCtrlCmdTimerCancel_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdTimerSchedule(pSubdevice, pParams) subdeviceCtrlCmdTimerSchedule_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerGetTime(pSubdevice, pParams) subdeviceCtrlCmdTimerGetTime_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerGetRegisterOffset(pSubdevice, pTimerRegOffsetParams) subdeviceCtrlCmdTimerGetRegisterOffset_DISPATCH(pSubdevice, pTimerRegOffsetParams)
#define subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo(pSubdevice, pParams) subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdTimerSetGrTickFreq(pSubdevice, pParams) subdeviceCtrlCmdTimerSetGrTickFreq_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdRcReadVirtualMem(pSubdevice, pReadVirtMemParam) subdeviceCtrlCmdRcReadVirtualMem_DISPATCH(pSubdevice, pReadVirtMemParam)
#define subdeviceCtrlCmdRcGetErrorCount(pSubdevice, pErrorCount) subdeviceCtrlCmdRcGetErrorCount_DISPATCH(pSubdevice, pErrorCount)
#define subdeviceCtrlCmdRcGetErrorV2(pSubdevice, pErrorParams) subdeviceCtrlCmdRcGetErrorV2_DISPATCH(pSubdevice, pErrorParams)
#define subdeviceCtrlCmdRcSetCleanErrorHistory(pSubdevice) subdeviceCtrlCmdRcSetCleanErrorHistory_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcGetWatchdogInfo(pSubdevice, pWatchdogInfoParams) subdeviceCtrlCmdRcGetWatchdogInfo_DISPATCH(pSubdevice, pWatchdogInfoParams)
#define subdeviceCtrlCmdRcDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcDisableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcSoftDisableWatchdog(pSubdevice) subdeviceCtrlCmdRcSoftDisableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcEnableWatchdog(pSubdevice) subdeviceCtrlCmdRcEnableWatchdog_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdRcReleaseWatchdogRequests(pSubdevice) subdeviceCtrlCmdRcReleaseWatchdogRequests_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalRcWatchdogTimeout(pSubdevice) subdeviceCtrlCmdInternalRcWatchdogTimeout_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdSetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdSetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcRecovery(pSubdevice, pRcRecovery) subdeviceCtrlCmdGetRcRecovery_DISPATCH(pSubdevice, pRcRecovery)
#define subdeviceCtrlCmdGetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdGetRcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdSetRcInfo(pSubdevice, pParams) subdeviceCtrlCmdSetRcInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdNvdGetDumpSize(pSubdevice, pDumpSizeParams) subdeviceCtrlCmdNvdGetDumpSize_DISPATCH(pSubdevice, pDumpSizeParams)
#define subdeviceCtrlCmdNvdGetDump(pSubdevice, pDumpParams) subdeviceCtrlCmdNvdGetDump_DISPATCH(pSubdevice, pDumpParams)
#define subdeviceCtrlCmdNvdGetNocatJournalRpt(pSubdevice, pReportParams) subdeviceCtrlCmdNvdGetNocatJournalRpt_DISPATCH(pSubdevice, pReportParams)
#define subdeviceCtrlCmdNvdSetNocatJournalData(pSubdevice, pReportParams) subdeviceCtrlCmdNvdSetNocatJournalData_DISPATCH(pSubdevice, pReportParams)
#define subdeviceCtrlCmdCeGetCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCapsV2(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetCapsV2_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetCePceMask(pSubdevice, pCePceMaskParams) subdeviceCtrlCmdCeGetCePceMask_DISPATCH(pSubdevice, pCePceMaskParams)
#define subdeviceCtrlCmdCeUpdatePceLceMappings(pSubdevice, pCeUpdatePceLceMappingsParams) subdeviceCtrlCmdCeUpdatePceLceMappings_DISPATCH(pSubdevice, pCeUpdatePceLceMappingsParams)
#define subdeviceCtrlCmdFlcnGetDmemUsage(pSubdevice, pFlcnDmemUsageParams) subdeviceCtrlCmdFlcnGetDmemUsage_DISPATCH(pSubdevice, pFlcnDmemUsageParams)
#define subdeviceCtrlCmdFlcnInstrumentationMap(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnInstrumentationUnmap(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationUnmap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnInstrumentationGetInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnInstrumentationGetControl(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationGetControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnInstrumentationSetControl(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationSetControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnInstrumentationRecalibrate(pSubdevice, pParams) subdeviceCtrlCmdFlcnInstrumentationRecalibrate_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetEngineArch(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetEngineArch_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerQueueInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerQueueInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerControlGet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlGet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnUstreamerControlSet(pSubdevice, pParams) subdeviceCtrlCmdFlcnUstreamerControlSet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetCtxBufferInfo(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlcnGetCtxBufferSize(pSubdevice, pParams) subdeviceCtrlCmdFlcnGetCtxBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdEccGetClientExposedCounters(pSubdevice, pParams) subdeviceCtrlCmdEccGetClientExposedCounters_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuQueryEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuSetEccConfiguration(pSubdevice, pConfig) subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(pSubdevice, pConfig)
#define subdeviceCtrlCmdGpuResetEccErrorStatus(pSubdevice, pParams) subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaRange(pSubdevice, pParams) subdeviceCtrlCmdFlaRange_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaSetupInstanceMemBlock(pSubdevice, pParams) subdeviceCtrlCmdFlaSetupInstanceMemBlock_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaGetRange(pSubdevice, pParams) subdeviceCtrlCmdFlaGetRange_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdFlaGetFabricMemStats(pSubdevice, pParams) subdeviceCtrlCmdFlaGetFabricMemStats_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGspGetFeatures(pSubdevice, pGspFeaturesParams) subdeviceCtrlCmdGspGetFeatures_DISPATCH(pSubdevice, pGspFeaturesParams)
#define subdeviceCtrlCmdGpuGetActivePartitionIds(pSubdevice, pParams) subdeviceCtrlCmdGpuGetActivePartitionIds_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPartitionCapacity(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitionCapacity_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuDescribePartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuDescribePartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPartitioningMode(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitioningMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGrmgrGetGrFsInfo(pSubdevice, pParams) subdeviceCtrlCmdGrmgrGetGrFsInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuSetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuSetPartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGpuGetPartitions(pSubdevice, pParams) subdeviceCtrlCmdGpuGetPartitions_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges(pSubdevice, pParams) subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance(pSubdevice, pParams) subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixGc6BlockerRefCnt(pSubdevice, pParams) subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixAllowDisallowGcoff(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAllowDisallowGcoff_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixAudioDynamicPower(pSubdevice, pParams) subdeviceCtrlCmdOsUnixAudioDynamicPower_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixVidmemPersistenceStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdOsUnixUpdateTgpStatus(pSubdevice, pParams) subdeviceCtrlCmdOsUnixUpdateTgpStatus_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetIpVersion(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetIpVersion_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetChannelPushbuffer(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetChannelPushbuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayWriteInstMem(pSubdevice, pParams) subdeviceCtrlCmdDisplayWriteInstMem_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetupRgLineIntr(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetupRgLineIntr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplaySetImportedImpData(pSubdevice, pParams) subdeviceCtrlCmdDisplaySetImportedImpData_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdDisplayGetDisplayMask(pSubdevice, pParams) subdeviceCtrlCmdDisplayGetDisplayMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdMsencGetCaps(pSubdevice, pParams) subdeviceCtrlCmdMsencGetCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer(pSubdevice) subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer(pSubdevice) subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize(pSubdevice, pParams) subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetChipInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalGetChipInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetUserRegisterAccessMap(pSubdevice, pParams) subdeviceCtrlCmdInternalGetUserRegisterAccessMap_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetDeviceInfoTable(pSubdevice, pParams) subdeviceCtrlCmdInternalGetDeviceInfoTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetConstructedFalconInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalGetConstructedFalconInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalRecoverAllComputeContexts(pSubdevice) subdeviceCtrlCmdInternalRecoverAllComputeContexts_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalGetSmcMode(pSubdevice, pParams) subdeviceCtrlCmdInternalGetSmcMode_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusBindLocalGfidForP2p(pSubdevice, pParams) subdeviceCtrlCmdInternalBusBindLocalGfidForP2p_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p(pSubdevice, pParams) subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusFlushWithSysmembar(pSubdevice) subdeviceCtrlCmdInternalBusFlushWithSysmembar_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote(pSubdevice, pParams) subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusDestroyP2pMailbox(pSubdevice, pParams) subdeviceCtrlCmdInternalBusDestroyP2pMailbox_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusCreateC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping(pSubdevice, pParams) subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGmmuGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdGmmuGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer(pSubdevice) subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer(pSubdevice, pParams) subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer(pSubdevice) subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdCeGetPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetPhysicalCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeGetAllPhysicalCaps(pSubdevice, pCeCapsParams) subdeviceCtrlCmdCeGetAllPhysicalCaps_DISPATCH(pSubdevice, pCeCapsParams)
#define subdeviceCtrlCmdCeUpdateClassDB(pSubdevice, params) subdeviceCtrlCmdCeUpdateClassDB_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdCeGetFaultMethodBufferSize(pSubdevice, params) subdeviceCtrlCmdCeGetFaultMethodBufferSize_DISPATCH(pSubdevice, params)
#define subdeviceCtrlCmdCeGetHubPceMask(pSubdevice, pParams) subdeviceCtrlCmdCeGetHubPceMask_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdIntrGetKernelTable(pSubdevice, pParams) subdeviceCtrlCmdIntrGetKernelTable_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfCudaLimitDisable(pSubdevice) subdeviceCtrlCmdInternalPerfCudaLimitDisable_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalPerfOptpCliClear(pSubdevice) subdeviceCtrlCmdInternalPerfOptpCliClear_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalPerfBoostSet_2x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_2x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfBoostSet_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostSet_3x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfBoostClear_3x(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfBoostClear_3x_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount(pSubdevice, pParams) subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifGetStaticInfo(pSubdevice, pParams) subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifGetAspmL1Flags(pSubdevice, pParams) subdeviceCtrlCmdBifGetAspmL1Flags_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdBifSetPcieRo(pSubdevice, pParams) subdeviceCtrlCmdBifSetPcieRo_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubPeerConnConfig(pSubdevice, pParams) subdeviceCtrlCmdHshubPeerConnConfig_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubFirstLinkPeerId(pSubdevice, pParams) subdeviceCtrlCmdHshubFirstLinkPeerId_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubGetHshubIdForLinks(pSubdevice, pParams) subdeviceCtrlCmdHshubGetHshubIdForLinks_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubGetNumUnits(pSubdevice, pParams) subdeviceCtrlCmdHshubGetNumUnits_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdHshubNextHshubId(pSubdevice, pParams) subdeviceCtrlCmdHshubNextHshubId_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr(pSubdevice) subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_DISPATCH(pSubdevice)
#define subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr(pSubdevice, pParams) subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetPcieP2pCaps(pSubdevice, pParams) subdeviceCtrlCmdInternalGetPcieP2pCaps_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdInternalGetCoherentFbApertureSize(pSubdevice, pParams) subdeviceCtrlCmdInternalGetCoherentFbApertureSize_DISPATCH(pSubdevice, pParams)
#define subdeviceCtrlCmdGetAvailableHshubMask(pSubdevice, pParams) subdeviceCtrlCmdGetAvailableHshubMask_DISPATCH(pSubdevice, pParams)
#define subdeviceShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) subdeviceShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define subdeviceMapTo(pResource, pParams) subdeviceMapTo_DISPATCH(pResource, pParams)
#define subdeviceGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) subdeviceGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define subdeviceCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) subdeviceCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define subdeviceGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) subdeviceGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define subdeviceSetNotificationShare(pNotifier, pNotifShare) subdeviceSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define subdeviceGetRefCount(pResource) subdeviceGetRefCount_DISPATCH(pResource)
#define subdeviceAddAdditionalDependants(pClient, pResource, pReference) subdeviceAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define subdeviceControl_Prologue(pResource, pCallContext, pParams) subdeviceControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) subdeviceGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define subdeviceUnmapFrom(pResource, pParams) subdeviceUnmapFrom_DISPATCH(pResource, pParams)
#define subdeviceControl_Epilogue(pResource, pCallContext, pParams) subdeviceControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define subdeviceControlLookup(pResource, pParams, ppEntry) subdeviceControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define subdeviceGetInternalObjectHandle(pGpuResource) subdeviceGetInternalObjectHandle_DISPATCH(pGpuResource)
#define subdeviceControl(pGpuResource, pCallContext, pParams) subdeviceControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define subdeviceUnmap(pGpuResource, pCallContext, pCpuMapping) subdeviceUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define subdeviceGetMemInterMapParams(pRmResource, pParams) subdeviceGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define subdeviceGetMemoryMappingDescriptor(pRmResource, ppMemDesc) subdeviceGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define subdeviceUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) subdeviceUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define subdeviceCanCopy(pResource) subdeviceCanCopy_DISPATCH(pResource)
#define subdeviceGetNotificationListPtr(pNotifier) subdeviceGetNotificationListPtr_DISPATCH(pNotifier)
#define subdeviceGetNotificationShare(pNotifier) subdeviceGetNotificationShare_DISPATCH(pNotifier)
#define subdeviceMap(pGpuResource, pCallContext, pParams, pCpuMapping) subdeviceMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define subdeviceAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) subdeviceAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams);

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_internal(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfReservePerfmonHw_internal(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL(pSubdevice, pPerfmonParams)
#endif //__nvoc_subdevice_h_disabled

#define subdeviceCtrlCmdPerfReservePerfmonHw_internal_HAL(pSubdevice, pPerfmonParams) subdeviceCtrlCmdPerfReservePerfmonHw_internal(pSubdevice, pPerfmonParams)

NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams);

#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_internal(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceCtrlCmdPerfRatedTdpSetControl_internal(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL(pSubdevice, pControlParams)
#endif //__nvoc_subdevice_h_disabled

#define subdeviceCtrlCmdPerfRatedTdpSetControl_internal_HAL(pSubdevice, pControlParams) subdeviceCtrlCmdPerfRatedTdpSetControl_internal(pSubdevice, pControlParams)

void subdevicePreDestruct_IMPL(struct Subdevice *pResource);

static inline void subdevicePreDestruct_DISPATCH(struct Subdevice *pResource) {
    pResource->__subdevicePreDestruct__(pResource);
}

NV_STATUS subdeviceInternalControlForward_IMPL(struct Subdevice *pSubdevice, NvU32 command, void *pParams, NvU32 size);

static inline NV_STATUS subdeviceInternalControlForward_DISPATCH(struct Subdevice *pSubdevice, NvU32 command, void *pParams, NvU32 size) {
    return pSubdevice->__subdeviceInternalControlForward__(pSubdevice, command, pParams, size);
}

NV_STATUS subdeviceControlFilter_IMPL(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS subdeviceControlFilter_DISPATCH(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pSubdevice->__subdeviceControlFilter__(pSubdevice, pCallContext, pParams);
}

NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBiosGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *pBiosInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetInfoV2__(pSubdevice, pBiosInfoParams);
}

NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBiosGetSKUInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *pBiosGetSKUInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetSKUInfo__(pSubdevice, pBiosGetSKUInfoParams);
}

NV_STATUS subdeviceCtrlCmdBiosGetPostTime_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime);

static inline NV_STATUS subdeviceCtrlCmdBiosGetPostTime_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS *pBiosPostTime) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetPostTime__(pSubdevice, pBiosPostTime);
}

NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams);

static inline NV_STATUS subdeviceCtrlCmdBiosGetUefiSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS *pUEFIParams) {
    return pSubdevice->__subdeviceCtrlCmdBiosGetUefiSupport__(pSubdevice, pUEFIParams);
}

NV_STATUS subdeviceCtrlCmdMcGetArchInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams);

static inline NV_STATUS subdeviceCtrlCmdMcGetArchInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS *pArchInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetArchInfo__(pSubdevice, pArchInfoParams);
}

NV_STATUS subdeviceCtrlCmdMcGetManufacturer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams);

static inline NV_STATUS subdeviceCtrlCmdMcGetManufacturer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS *pManufacturerParams) {
    return pSubdevice->__subdeviceCtrlCmdMcGetManufacturer__(pSubdevice, pManufacturerParams);
}

NV_STATUS subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pGetStatusParams);

static inline NV_STATUS subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pGetStatusParams) {
    return pSubdevice->__subdeviceCtrlCmdMcQueryHostclkSlowdownStatus__(pSubdevice, pGetStatusParams);
}

NV_STATUS subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMcSetHostclkSlowdownStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMcSetHostclkSlowdownStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams);

static inline NV_STATUS subdeviceCtrlCmdMcChangeReplayableFaultOwnership_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS *pReplayableFaultOwnrshpParams) {
    return pSubdevice->__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__(pSubdevice, pReplayableFaultOwnrshpParams);
}

NV_STATUS subdeviceCtrlCmdMcServiceInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams);

static inline NV_STATUS subdeviceCtrlCmdMcServiceInterrupts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pServiceInterruptParams) {
    return pSubdevice->__subdeviceCtrlCmdMcServiceInterrupts__(pSubdevice, pServiceInterruptParams);
}

NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDmaInvalidateTLB_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDmaInvalidateTLB__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDmaGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams);

static inline NV_STATUS subdeviceCtrlCmdDmaGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdDmaGetInfo__(pSubdevice, pDmaInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPciInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPciInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPciInfo__(pSubdevice, pPciInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetInfo__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetInfoV2__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPciBarInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPciBarInfo__(pSubdevice, pBarInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieSpeed__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLinkWidth_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS *pLinkWidthParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieLinkWidth__(pSubdevice, pLinkWidthParams);
}

NV_STATUS subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusHWBCGetUpstreamBAR0_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_HWBC_GET_UPSTREAM_BAR0_PARAMS *pBusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdBusHWBCGetUpstreamBAR0_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_HWBC_GET_UPSTREAM_BAR0_PARAMS *pBusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdBusHWBCGetUpstreamBAR0__(pSubdevice, pBusInfoParams);
}

NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusServiceGpuMultifunctionState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusServiceGpuMultifunctionState__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetBFD_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetBFD_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetBFD__(pSubdevice, pBusGetBFDParams);
}

NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetAspmDisableFlags_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetAspmDisableFlags__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusControlPublicAspmBits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusControlPublicAspmBits__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusClearPexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusClearPexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusClearPexCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPexUtilCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexUtilCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusClearPexUtilCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusClearPexUtilCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusFreezePexCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusFreezePexCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusFreezePexCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPexLaneCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPexLaneCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetPcieLtrLatency_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetPcieLtrLatency__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetPcieLtrLatency_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetPcieLtrLatency__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkPeerIdMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusSetEomParameters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusSetEomParameters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSetEomParameters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetUphyDlnCfgSpace_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetEomStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetEomStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetEomStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusSysmemAccess_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusSysmemAccess_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusSysmemAccess__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkCaps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBusGetNvlinkStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBusGetNvlinkStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGetNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGetNvlinkCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetNvlinkCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdClearNvlinkCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdClearNvlinkCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdClearNvlinkCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSetupEom_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetupEom_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetupEom__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetPowerState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetPowerState__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkReadTpCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkReadTpCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLpCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLpCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkEnableNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEnableNvlinkPeer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkCoreCallback_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkCoreCallback_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkCoreCallback__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetLoopbackMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetLoopbackMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkUpdateHshubMux_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateHshubMux_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkUpdateHshubMux__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkRemoveNvlinkMapping__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSaveRestoreHshubState_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSaveRestoreHshubState_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSaveRestoreHshubState__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkProgramBufferready_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkProgramBufferready_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkProgramBufferready__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkUpdateCurrentConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateCurrentConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkUpdateCurrentConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkUpdatePeerLinkMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkUpdateLinkConnection_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkUpdateLinkConnection_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkUpdateLinkConnection__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkEnableLinksPostTopology_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableLinksPostTopology_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEnableLinksPostTopology__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetRefreshCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetRefreshCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkClearRefreshCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkClearRefreshCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkProgramLinkSpeed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkProgramLinkSpeed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkProgramLinkSpeed__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkAreLinksTrained_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkAreLinksTrained_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkAreLinksTrained__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkResetLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkResetLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkResetLinks__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkDisableDlInterrupts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkDisableDlInterrupts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkDisableDlInterrupts__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetLinkAndClockInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSetupNvlinkSysmem__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkProcessForcedConfigs_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkProcessForcedConfigs_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkProcessForcedConfigs__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSyncLaneShutdownProps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdNvlinkEnableLinks_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkEnableLinks__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdI2cReadBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams);

static inline NV_STATUS subdeviceCtrlCmdI2cReadBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_READ_BUFFER_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cReadBuffer__(pSubdevice, pI2cParams);
}

NV_STATUS subdeviceCtrlCmdI2cWriteBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams);

static inline NV_STATUS subdeviceCtrlCmdI2cWriteBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cWriteBuffer__(pSubdevice, pI2cParams);
}

NV_STATUS subdeviceCtrlCmdI2cReadReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);

static inline NV_STATUS subdeviceCtrlCmdI2cReadReg_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cReadReg__(pSubdevice, pI2cParams);
}

NV_STATUS subdeviceCtrlCmdI2cWriteReg_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams);

static inline NV_STATUS subdeviceCtrlCmdI2cWriteReg_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_I2C_RW_REG_PARAMS *pI2cParams) {
    return pSubdevice->__subdeviceCtrlCmdI2cWriteReg__(pSubdevice, pI2cParams);
}

NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *pParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM *pParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams);

static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpGetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfRatedTdpGetControl__(pSubdevice, pControlParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_a2e9a2(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    return subdeviceCtrlCmdPerfRatedTdpSetControl_internal(pSubdevice, pControlParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfRatedTdpSetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfRatedTdpSetControl__(pSubdevice, pControlParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_3f0664(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    return subdeviceCtrlCmdPerfReservePerfmonHw_internal(pSubdevice, pPerfmonParams);
}

static inline NV_STATUS subdeviceCtrlCmdPerfReservePerfmonHw_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams) {
    return pSubdevice->__subdeviceCtrlCmdPerfReservePerfmonHw__(pSubdevice, pPerfmonParams);
}

NV_STATUS subdeviceCtrlCmdKPerfBoost_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams);

static inline NV_STATUS subdeviceCtrlCmdKPerfBoost_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams) {
    return pSubdevice->__subdeviceCtrlCmdKPerfBoost__(pSubdevice, pBoostParams);
}

NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetFBRegionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetFBRegionInfo__(pSubdevice, pGFBRIParams);
}

NV_STATUS subdeviceCtrlCmdFbGetBar1Offset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetBar1Offset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetBar1Offset__(pSubdevice, pFbMemParams);
}

NV_STATUS subdeviceCtrlCmdFbIsKind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams);

static inline NV_STATUS subdeviceCtrlCmdFbIsKind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams) {
    return pSubdevice->__subdeviceCtrlCmdFbIsKind__(pSubdevice, pIsKindParams);
}

NV_STATUS subdeviceCtrlCmdFbGetMemAlignment_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetMemAlignment_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetMemAlignment__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetHeapReservationSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetHeapReservationSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_PARAMS *pFbInfoParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_PARAMS *pFbInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetInfo__(pSubdevice, pFbInfoParams);
}

NV_STATUS subdeviceCtrlCmdFbGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetInfoV2__(pSubdevice, pFbInfoParams);
}

NV_STATUS subdeviceCtrlCmdFbGetCarveoutAddressInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetCarveoutAddressInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCarveoutAddressInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetCalibrationLockFailed_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS *pGCLFParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCalibrationLockFailed__(pSubdevice, pGCLFParams);
}

NV_STATUS subdeviceCtrlCmdFbFlushGpuCache_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams);

static inline NV_STATUS subdeviceCtrlCmdFbFlushGpuCache_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams) {
    return pSubdevice->__subdeviceCtrlCmdFbFlushGpuCache__(pSubdevice, pCacheFlushParams);
}

NV_STATUS subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicy__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *pGpuCacheAllocPolicyParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS *pGpuCacheAllocPolicyParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicy__(pSubdevice, pGpuCacheAllocPolicyParams);
}

NV_STATUS subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetGpuCacheInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetGpuCacheInfo__(pSubdevice, pGpuCacheParams);
}

NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetCliManagedOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__(pSubdevice, pOsOfflinedParams);
}

NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pBlackListParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetOfflinedPages__(pSubdevice, pBlackListParams);
}

NV_STATUS subdeviceCtrlCmdFbSetupVprRegion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetupVprRegion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *pCliReqParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetupVprRegion__(pSubdevice, pCliReqParams);
}

NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetLTCInfoForFBP_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetLTCInfoForFBP__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbPatchPbrForMining_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbPatchPbrForMining__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetRemappedRows_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetRemappedRows_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS *pRemappedRowsParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetRemappedRows__(pSubdevice, pRemappedRowsParams);
}

NV_STATUS subdeviceCtrlCmdFbGetFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetFsInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetFsInfo__(pSubdevice, pInfoParams);
}

NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetRowRemapperHistogram_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetRowRemapperHistogram__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetDynamicOfflinedPages_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbUpdateNumaStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbUpdateNumaStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetNumaInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetNumaInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetNumaInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetStaticConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysSetPartitionableMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysSetPartitionableMem__(pSubdevice, pParams);
}

static inline NV_STATUS subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *pParams) {
    return subdeviceInternalControlForward(pSubdevice, (545262184), pParams, sizeof (*pParams));
}

static inline NV_STATUS subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetZbcReferenced_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetZbcReferenced__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysL2InvalidateEvict_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysL2InvalidateEvict__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdMemSysDisableNvlinkPeers_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdMemSysDisableNvlinkPeers__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysProgramRawCompressionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysProgramRawCompressionMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbGetCtagsForCbcEviction_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbGetCtagsForCbcEviction__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbCBCOp_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbCBCOp_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FB_CBC_OP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbCBCOp__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbSetRrd_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetRrd_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_RRD_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetRrd__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbSetReadLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetReadLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetReadLimit__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFbSetWriteLimit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFbSetWriteLimit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFbSetWriteLimit__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdSetGpfifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams);

static inline NV_STATUS subdeviceCtrlCmdSetGpfifo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_GPFIFO_PARAMS *pSetGpFifoParams) {
    return pSubdevice->__subdeviceCtrlCmdSetGpfifo__(pSubdevice, pSetGpFifoParams);
}

NV_STATUS subdeviceCtrlCmdSetOperationalProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties);

static inline NV_STATUS subdeviceCtrlCmdSetOperationalProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS *pSetOperationalProperties) {
    return pSubdevice->__subdeviceCtrlCmdSetOperationalProperties__(pSubdevice, pSetOperationalProperties);
}

NV_STATUS subdeviceCtrlCmdFifoBindEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams);

static inline NV_STATUS subdeviceCtrlCmdFifoBindEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS *pBindParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoBindEngines__(pSubdevice, pBindParams);
}

NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGetPhysicalChannelCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetPhysicalChannelCount__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFifoGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams);

static inline NV_STATUS subdeviceCtrlCmdFifoGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_INFO_PARAMS *pFifoInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetInfo__(pSubdevice, pFifoInfoParams);
}

NV_STATUS subdeviceCtrlCmdFifoDisableChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams);

static inline NV_STATUS subdeviceCtrlCmdFifoDisableChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pDisableChannelParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableChannels__(pSubdevice, pDisableChannelParams);
}

NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFifoDisableUsermodeChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoDisableUsermodeChannels__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams);

static inline NV_STATUS subdeviceCtrlCmdFifoGetChannelMemInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS *pChannelMemParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetChannelMemInfo__(pSubdevice, pChannelMemParams);
}

NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams);

static inline NV_STATUS subdeviceCtrlCmdFifoGetUserdLocation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS *pUserdLocationParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetUserdLocation__(pSubdevice, pUserdLocationParams);
}

NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFifoGetDeviceInfoTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoGetDeviceInfoTable__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFifoClearFaultedBit_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoClearFaultedBit__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams);

static inline NV_STATUS subdeviceCtrlCmdFifoRunlistSetSchedPolicy_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS *pSchedPolicyParams) {
    return pSubdevice->__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__(pSubdevice, pSchedPolicyParams);
}

NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo);

static inline NV_STATUS subdeviceCtrlCmdFifoUpdateChannelInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pChannelInfo) {
    return pSubdevice->__subdeviceCtrlCmdFifoUpdateChannelInfo__(pSubdevice, pChannelInfo);
}

NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams);

static inline NV_STATUS subdeviceCtrlCmdInternalFifoGetNumChannels_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS *pNumChannelsParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalFifoGetNumChannels__(pSubdevice, pNumChannelsParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_INFO_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetInfoV2__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCapsV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCapsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CAPS_V2_PARAMS *pGrCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCapsV2__(pSubdevice, pGrCapsParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswModes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxswModes__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswZcullBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswZcullBind__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *pZcullInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetZcullInfo__(pSubdevice, pZcullInfoParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPmMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPmBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPmBind__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrSetGpcTileMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetGpcTileMap__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswSmpcMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswSmpcMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrPcSamplingMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrPcSamplingMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetGlobalSmOrder_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGlobalSmOrder__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrSetCtxswPreemptionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrCtxswPreemptionBind_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrCtxswPreemptionBind__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetROPInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetROPInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ROP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetROPInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxswStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxswStats__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCtxBufferPtes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCtxBufferPtes__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetCurrentResidentChannel_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetCurrentResidentChannel__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetVatAlarmData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetVatAlarmData__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetAttributeBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetAttributeBufferSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolQuerySize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolQuerySize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolInitialize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolInitialize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolAddSlots_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolAddSlots__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGfxPoolRemoveSlots_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetPpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetPpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetPpcMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrSetTpcPartitionMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetSmIssueRateModifier_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetSmIssueRateModifier__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUid_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetPhysGpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetPhysGpcMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetGpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGpcMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetTpcMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetTpcMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_TPC_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetTpcMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetEngineContextProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetEngineContextProperties__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetNumTpcsForGpc_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetNumTpcsForGpc__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetGpcTileMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetGpcTileMap__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrGetZcullMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrGetZcullMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrGetZcullMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetCaps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetRopInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetCachedInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetCachedInfo__(pSubdevice, pGpuInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetInfoV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetInfoV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInfoV2__(pSubdevice, pGpuInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetIpVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS *pGpuIpVersionParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetIpVersion__(pSubdevice, pGpuIpVersionParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__(pSubdevice, pBridgeInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__(pSubdevice, pBridgeInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetOptimusInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetOptimusInfo__(pSubdevice, pGpuOptimusInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetNameString_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS *pNameStringParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNameString__(pSubdevice, pNameStringParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetShortNameString_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetShortNameString_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS *pShortNameStringParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetShortNameString__(pSubdevice, pShortNameStringParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEncoderCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEncoderCapacity__(pSubdevice, pEncoderCapacityParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionStats__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvencSwSessionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params);

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__(pSubdevice, params);
}

NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params);

static inline NV_STATUS subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__(pSubdevice, params);
}

NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetFabricAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetFabricAddr__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetPower_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_POWER_PARAMS *pSetPowerParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPower__(pSubdevice, pSetPowerParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetSdm_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetSdm_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetSdm__(pSubdevice, pSdmParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetSdm_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_SDM_PARAMS *pSdmParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetSdm_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_SDM_PARAMS *pSdmParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetSdm__(pSubdevice, pSdmParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetSimulationInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetSimulationInfo__(pSubdevice, pGpuSimulationInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngines__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginesV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEnginesV2__(pSubdevice, pEngineParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineClasslist_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineClasslist__(pSubdevice, pClassParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEnginePartnerList_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEnginePartnerList__(pSubdevice, pPartnerListParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiGpcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiGpcInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiTpcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiTpcInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetFermiZcullInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pGpuFermiZcullInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetFermiZcullInfo__(pSubdevice, pGpuFermiZcullInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPesInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPesInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPesInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuExecRegOps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams);

static inline NV_STATUS subdeviceCtrlCmdGpuExecRegOps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pRegParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuExecRegOps__(pSubdevice, pRegParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryMode__(pSubdevice, pQueryMode);
}

NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo);

static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomImageVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS *pVersionInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInforomImageVersion__(pSubdevice, pVersionInfo);
}

NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo);

static inline NV_STATUS subdeviceCtrlCmdGpuGetInforomObjectVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS *pVersionInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetInforomObjectVersion__(pSubdevice, pVersionInfo);
}

NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryInforomEccSupport_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryInforomEccSupport__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryEccStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo);

static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMBoardInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *pBoardInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetOEMBoardInfo__(pSubdevice, pBoardInfo);
}

NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo);

static inline NV_STATUS subdeviceCtrlCmdGpuGetOEMInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS *pOemInfo) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetOEMInfo__(pSubdevice, pOemInfo);
}

NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdGpuHandleGpuSR_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuHandleGpuSR__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetComputeModeRules_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetComputeModeRules__(pSubdevice, pSetRulesParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryComputeModeRules_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryComputeModeRules__(pSubdevice, pQueryRulesParams);
}

NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdGpuAcquireComputeModeReservation_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuAcquireComputeModeReservation__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdGpuReleaseComputeModeReservation_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdGpuReleaseComputeModeReservation__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdGpuInitializeCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams);

static inline NV_STATUS subdeviceCtrlCmdGpuInitializeCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pInitializeCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuInitializeCtx__(pSubdevice, pInitializeCtxParams);
}

NV_STATUS subdeviceCtrlCmdGpuPromoteCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams);

static inline NV_STATUS subdeviceCtrlCmdGpuPromoteCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pPromoteCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuPromoteCtx__(pSubdevice, pPromoteCtxParams);
}

NV_STATUS subdeviceCtrlCmdGpuEvictCtx_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams);

static inline NV_STATUS subdeviceCtrlCmdGpuEvictCtx_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pEvictCtxParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuEvictCtx__(pSubdevice, pEvictCtxParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetId__(pSubdevice, pIdParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetGidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetGidInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetGidInfo__(pSubdevice, pGidInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryIllumSupport_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryIllumSupport__(pSubdevice, pConfigParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetIllum_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetIllum__(pSubdevice, pConfigParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetIllum_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetIllum_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_ILLUM_PARAMS *pConfigParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetIllum__(pSubdevice, pConfigParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryScrubberStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryScrubberStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetVprCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetVprCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVprCaps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetVprInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetVprInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetVprInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPids_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPids_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPids__(pSubdevice, pGetPidsParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPidInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPidInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPidInfo__(pSubdevice, pGetPidInfoParams);
}

NV_STATUS subdeviceCtrlCmdGpuInterruptFunction_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INTERRUPT_FUNCTION_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuInterruptFunction_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_INTERRUPT_FUNCTION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuInterruptFunction__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryFunctionStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryFunctionStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuReportNonReplayableFault_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuReportNonReplayableFault__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineFaultInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineFaultInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineRunlistPriBase_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetHwEngineId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetHwEngineId__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetMaxSupportedPageSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetComputePolicyConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetComputePolicyConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetComputePolicyConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetComputePolicyConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdValidateMemMapRequest__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetEngineLoadTimes_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetEngineLoadTimes__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdEventSetTrigger_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdEventSetTrigger_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdEventSetTrigger__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams);

static inline NV_STATUS subdeviceCtrlCmdEventSetTriggerFifo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS *pTriggerFifoParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetTriggerFifo__(pSubdevice, pTriggerFifoParams);
}

NV_STATUS subdeviceCtrlCmdEventSetNotification_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

static inline NV_STATUS subdeviceCtrlCmdEventSetNotification_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetNotification__(pSubdevice, pSetEventParams);
}

NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);

static inline NV_STATUS subdeviceCtrlCmdEventSetMemoryNotifies_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetMemoryNotifies__(pSubdevice, pSetMemoryNotifiesParams);
}

NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams);

static inline NV_STATUS subdeviceCtrlCmdEventSetSemaphoreMemory_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS *pSetSemMemoryParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetSemaphoreMemory__(pSubdevice, pSetSemMemoryParams);
}

NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams);

static inline NV_STATUS subdeviceCtrlCmdEventSetSemaMemValidation_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS *pSetSemaMemValidationParams) {
    return pSubdevice->__subdeviceCtrlCmdEventSetSemaMemValidation__(pSubdevice, pSetSemaMemValidationParams);
}

NV_STATUS subdeviceCtrlCmdTimerCancel_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdTimerCancel_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdTimerCancel__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdTimerSchedule_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdTimerSchedule_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerSchedule__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdTimerGetTime_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdTimerGetTime_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetTime__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams);

static inline NV_STATUS subdeviceCtrlCmdTimerGetRegisterOffset_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetRegisterOffset__(pSubdevice, pTimerRegOffsetParams);
}

NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdTimerSetGrTickFreq_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdTimerSetGrTickFreq__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdRcReadVirtualMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam);

static inline NV_STATUS subdeviceCtrlCmdRcReadVirtualMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam) {
    return pSubdevice->__subdeviceCtrlCmdRcReadVirtualMem__(pSubdevice, pReadVirtMemParam);
}

NV_STATUS subdeviceCtrlCmdRcGetErrorCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount);

static inline NV_STATUS subdeviceCtrlCmdRcGetErrorCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount) {
    return pSubdevice->__subdeviceCtrlCmdRcGetErrorCount__(pSubdevice, pErrorCount);
}

NV_STATUS subdeviceCtrlCmdRcGetErrorV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams);

static inline NV_STATUS subdeviceCtrlCmdRcGetErrorV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams) {
    return pSubdevice->__subdeviceCtrlCmdRcGetErrorV2__(pSubdevice, pErrorParams);
}

NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcSetCleanErrorHistory__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams);

static inline NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams) {
    return pSubdevice->__subdeviceCtrlCmdRcGetWatchdogInfo__(pSubdevice, pWatchdogInfoParams);
}

NV_STATUS subdeviceCtrlCmdRcDisableWatchdog_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdRcDisableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcDisableWatchdog__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcSoftDisableWatchdog__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdRcEnableWatchdog_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdRcEnableWatchdog_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcEnableWatchdog__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdRcReleaseWatchdogRequests__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalRcWatchdogTimeout__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdSetRcRecovery_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery);

static inline NV_STATUS subdeviceCtrlCmdSetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdSetRcRecovery__(pSubdevice, pRcRecovery);
}

NV_STATUS subdeviceCtrlCmdGetRcRecovery_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery);

static inline NV_STATUS subdeviceCtrlCmdGetRcRecovery_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery) {
    return pSubdevice->__subdeviceCtrlCmdGetRcRecovery__(pSubdevice, pRcRecovery);
}

NV_STATUS subdeviceCtrlCmdGetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGetRcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetRcInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdSetRcInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdSetRcInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdSetRcInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdNvdGetDumpSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams);

static inline NV_STATUS subdeviceCtrlCmdNvdGetDumpSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS *pDumpSizeParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetDumpSize__(pSubdevice, pDumpSizeParams);
}

NV_STATUS subdeviceCtrlCmdNvdGetDump_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams);

static inline NV_STATUS subdeviceCtrlCmdNvdGetDump_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_DUMP_PARAMS *pDumpParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetDump__(pSubdevice, pDumpParams);
}

NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams);

static inline NV_STATUS subdeviceCtrlCmdNvdGetNocatJournalRpt_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS *pReportParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdGetNocatJournalRpt__(pSubdevice, pReportParams);
}

NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams);

static inline NV_STATUS subdeviceCtrlCmdNvdSetNocatJournalData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS *pReportParams) {
    return pSubdevice->__subdeviceCtrlCmdNvdSetNocatJournalData__(pSubdevice, pReportParams);
}

NV_STATUS subdeviceCtrlCmdCeGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCaps__(pSubdevice, pCeCapsParams);
}

NV_STATUS subdeviceCtrlCmdCeGetCapsV2_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetCapsV2_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCapsV2__(pSubdevice, pCeCapsParams);
}

NV_STATUS subdeviceCtrlCmdCeGetAllCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetAllCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetAllCaps__(pSubdevice, pCeCapsParams);
}

NV_STATUS subdeviceCtrlCmdCeGetCePceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetCePceMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pCePceMaskParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetCePceMask__(pSubdevice, pCePceMaskParams);
}

NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *pCeUpdatePceLceMappingsParams);

static inline NV_STATUS subdeviceCtrlCmdCeUpdatePceLceMappings_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS *pCeUpdatePceLceMappingsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeUpdatePceLceMappings__(pSubdevice, pCeUpdatePceLceMappingsParams);
}

NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnGetDmemUsage_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS *pFlcnDmemUsageParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetDmemUsage__(pSubdevice, pFlcnDmemUsageParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationMap__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationUnmap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationUnmap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationUnmap__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_GET_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationGetInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationGetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationGetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationGetControl__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationSetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationSetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationSetControl__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnInstrumentationRecalibrate_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_RECALIBRATE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnInstrumentationRecalibrate_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_INSTRUMENTATION_RECALIBRATE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnInstrumentationRecalibrate__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnGetEngineArch_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetEngineArch__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerQueueInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerQueueInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlGet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlGet__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnUstreamerControlSet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnUstreamerControlSet__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlcnGetCtxBufferSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdEccGetClientExposedCounters_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdEccGetClientExposedCounters__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdGpuQueryEccConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdGpuQueryEccConfiguration__(pSubdevice, pConfig);
}

NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig);

static inline NV_STATUS subdeviceCtrlCmdGpuSetEccConfiguration_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS *pConfig) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetEccConfiguration__(pSubdevice, pConfig);
}

NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuResetEccErrorStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuResetEccErrorStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlaRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlaRange_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_RANGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaRange__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlaSetupInstanceMemBlock_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaSetupInstanceMemBlock__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlaGetRange_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlaGetRange_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaGetRange__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdFlaGetFabricMemStats_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdFlaGetFabricMemStats__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGspGetFeatures_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams);

static inline NV_STATUS subdeviceCtrlCmdGspGetFeatures_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GSP_GET_FEATURES_PARAMS *pGspFeaturesParams) {
    return pSubdevice->__subdeviceCtrlCmdGspGetFeatures__(pSubdevice, pGspFeaturesParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetActivePartitionIds_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetActivePartitionIds__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitionCapacity_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPartitionCapacity__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuDescribePartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuDescribePartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuDescribePartitions__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitioningMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPartitioningMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGrmgrGetGrFsInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGrmgrGetGrFsInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuSetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuSetPartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuSetPartitions__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGpuGetPartitions_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGpuGetPartitions_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGpuGetPartitions__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdOsUnixAllowDisallowGcoff_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdOsUnixAudioDynamicPower_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixAudioDynamicPower__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdOsUnixUpdateTgpStatus_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdOsUnixUpdateTgpStatus__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplayGetIpVersion_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetIpVersion__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplayGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetStaticInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplaySetChannelPushbuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetChannelPushbuffer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplayWriteInstMem_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayWriteInstMem__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplaySetupRgLineIntr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetupRgLineIntr__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplaySetImportedImpData_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplaySetImportedImpData__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdDisplayGetDisplayMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdDisplayGetDisplayMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdMsencGetCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdMsencGetCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdMsencGetCaps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetChipInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetChipInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetChipInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetUserRegisterAccessMap_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetDeviceInfoTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetDeviceInfoTable__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetConstructedFalconInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetConstructedFalconInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetConstructedFalconInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalRecoverAllComputeContexts_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalRecoverAllComputeContexts__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalGetSmcMode_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetSmcMode_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetSmcMode__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusBindLocalGfidForP2p_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusBindLocalGfidForP2p_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusBindLocalGfidForP2p__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalBusFlushWithSysmembar_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusFlushWithSysmembar__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusDestroyP2pMailbox_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGmmuGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGmmuGetStaticInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetPhysicalCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetPhysicalCaps__(pSubdevice, pCeCapsParams);
}

NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetAllPhysicalCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetAllPhysicalCaps__(pSubdevice, pCeCapsParams);
}

NV_STATUS subdeviceCtrlCmdCeUpdateClassDB_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params);

static inline NV_STATUS subdeviceCtrlCmdCeUpdateClassDB_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdCeUpdateClassDB__(pSubdevice, params);
}

NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params);

static inline NV_STATUS subdeviceCtrlCmdCeGetFaultMethodBufferSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS *params) {
    return pSubdevice->__subdeviceCtrlCmdCeGetFaultMethodBufferSize__(pSubdevice, params);
}

NV_STATUS subdeviceCtrlCmdCeGetHubPceMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdCeGetHubPceMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdCeGetHubPceMask__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdIntrGetKernelTable_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdIntrGetKernelTable_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdIntrGetKernelTable__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfCudaLimitDisable_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfCudaLimitDisable_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfCudaLimitDisable__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalPerfOptpCliClear_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfOptpCliClear_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfOptpCliClear__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_2x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_2x__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostSet_3x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostSet_3x__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfBoostClear_3x_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfBoostClear_3x__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBifGetStaticInfo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifGetStaticInfo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBifGetAspmL1Flags_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifGetAspmL1Flags__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdBifSetPcieRo_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdBifSetPcieRo_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdBifSetPcieRo__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdHshubPeerConnConfig_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubPeerConnConfig__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdHshubFirstLinkPeerId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdHshubFirstLinkPeerId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubFirstLinkPeerId__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdHshubGetHshubIdForLinks_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubGetHshubIdForLinks__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdHshubGetNumUnits_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdHshubGetNumUnits_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubGetNumUnits__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdHshubNextHshubId_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdHshubNextHshubId_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdHshubNextHshubId__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL(struct Subdevice *pSubdevice);

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_DISPATCH(struct Subdevice *pSubdevice) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__(pSubdevice);
}

NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetPcieP2pCaps_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetPcieP2pCaps__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdInternalGetCoherentFbApertureSize_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdInternalGetCoherentFbApertureSize_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdInternalGetCoherentFbApertureSize__(pSubdevice, pParams);
}

NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask_IMPL(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams);

static inline NV_STATUS subdeviceCtrlCmdGetAvailableHshubMask_DISPATCH(struct Subdevice *pSubdevice, NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS *pParams) {
    return pSubdevice->__subdeviceCtrlCmdGetAvailableHshubMask__(pSubdevice, pParams);
}

static inline NvBool subdeviceShareCallback_DISPATCH(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__subdeviceShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS subdeviceMapTo_DISPATCH(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__subdeviceMapTo__(pResource, pParams);
}

static inline NV_STATUS subdeviceGetOrAllocNotifShare_DISPATCH(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__subdeviceGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS subdeviceCheckMemInterUnmap_DISPATCH(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__subdeviceCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS subdeviceGetMapAddrSpace_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__subdeviceGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void subdeviceSetNotificationShare_DISPATCH(struct Subdevice *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__subdeviceSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 subdeviceGetRefCount_DISPATCH(struct Subdevice *pResource) {
    return pResource->__subdeviceGetRefCount__(pResource);
}

static inline void subdeviceAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference) {
    pResource->__subdeviceAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS subdeviceControl_Prologue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__subdeviceControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS subdeviceGetRegBaseOffsetAndSize_DISPATCH(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__subdeviceGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS subdeviceUnmapFrom_DISPATCH(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__subdeviceUnmapFrom__(pResource, pParams);
}

static inline void subdeviceControl_Epilogue_DISPATCH(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__subdeviceControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS subdeviceControlLookup_DISPATCH(struct Subdevice *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__subdeviceControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle subdeviceGetInternalObjectHandle_DISPATCH(struct Subdevice *pGpuResource) {
    return pGpuResource->__subdeviceGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS subdeviceControl_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__subdeviceControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS subdeviceUnmap_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__subdeviceUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS subdeviceGetMemInterMapParams_DISPATCH(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__subdeviceGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS subdeviceGetMemoryMappingDescriptor_DISPATCH(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__subdeviceGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS subdeviceUnregisterEvent_DISPATCH(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__subdeviceUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NvBool subdeviceCanCopy_DISPATCH(struct Subdevice *pResource) {
    return pResource->__subdeviceCanCopy__(pResource);
}

static inline PEVENTNOTIFICATION *subdeviceGetNotificationListPtr_DISPATCH(struct Subdevice *pNotifier) {
    return pNotifier->__subdeviceGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *subdeviceGetNotificationShare_DISPATCH(struct Subdevice *pNotifier) {
    return pNotifier->__subdeviceGetNotificationShare__(pNotifier);
}

static inline NV_STATUS subdeviceMap_DISPATCH(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__subdeviceMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool subdeviceAccessCallback_DISPATCH(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__subdeviceAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS subdeviceSetPerfmonReservation(struct Subdevice *pSubdevice, NvBool bReservation, NvBool bClientHandlesGrGating, NvBool bRmHandlesIdleSlow) {
    return NV_OK;
}

static inline NV_STATUS subdeviceResetTGP(struct Subdevice *pSubdevice) {
    return NV_OK;
}

static inline NV_STATUS subdeviceReleaseVideoStreams(struct Subdevice *pSubdevice) {
    return NV_OK;
}

static inline void subdeviceRestoreLockedClock(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    return;
}

static inline void subdeviceReleaseNvlinkErrorInjectionMode(struct Subdevice *pSubdevice, struct CALL_CONTEXT *pCallContext) {
    return;
}

NV_STATUS subdeviceConstruct_IMPL(struct Subdevice *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_subdeviceConstruct(arg_pResource, arg_pCallContext, arg_pParams) subdeviceConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void subdeviceDestruct_IMPL(struct Subdevice *pResource);
#define __nvoc_subdeviceDestruct(pResource) subdeviceDestruct_IMPL(pResource)
NV_STATUS subdeviceAddP2PApi_IMPL(struct Subdevice *pSubdevice, struct P2PApi *pP2PApi);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceAddP2PApi(struct Subdevice *pSubdevice, struct P2PApi *pP2PApi) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceAddP2PApi(pSubdevice, pP2PApi) subdeviceAddP2PApi_IMPL(pSubdevice, pP2PApi)
#endif //__nvoc_subdevice_h_disabled

NV_STATUS subdeviceDelP2PApi_IMPL(struct Subdevice *pSubdevice, struct P2PApi *pP2PApi);
#ifdef __nvoc_subdevice_h_disabled
static inline NV_STATUS subdeviceDelP2PApi(struct Subdevice *pSubdevice, struct P2PApi *pP2PApi) {
    NV_ASSERT_FAILED_PRECOMP("Subdevice was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_subdevice_h_disabled
#define subdeviceDelP2PApi(pSubdevice, pP2PApi) subdeviceDelP2PApi_IMPL(pSubdevice, pP2PApi)
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
NV_STATUS subdeviceGetByInstance_IMPL(struct RsClient *pClient, NvHandle hDevice, NvU32 subDeviceInst, struct Subdevice **ppSubdevice);
#define subdeviceGetByInstance(pClient, hDevice, subDeviceInst, ppSubdevice) subdeviceGetByInstance_IMPL(pClient, hDevice, subDeviceInst, ppSubdevice)
#undef PRIVATE_FIELD


// ****************************************************************************
//                            Deprecated Definitions
// ****************************************************************************

/**
 * WARNING: This function is deprecated! Please use subdeviceGetByGpu and
 * GPU_RES_SET_THREAD_BC_STATE (if needed to set thread UC state for SLI)
 */
struct Subdevice *CliGetSubDeviceInfoFromGpu(NvHandle, struct OBJGPU*);

/**
 * WARNING: This function is deprecated! Please use subdeviceGetByGpu and
 * RES_GET_HANDLE
 */
NV_STATUS CliGetSubDeviceHandleFromGpu(NvHandle, struct OBJGPU*, NvHandle *);

/**
 * WARNING: This function is deprecated and use is *strongly* discouraged
 * (especially for new code!)
 *
 * From the function name (CliSetSubDeviceContext) it appears as a simple
 * accessor but violates expectations by modifying the SLI BC threadstate (calls
 * to GPU_RES_SET_THREAD_BC_STATE). This can be dangerous if not carefully
 * managed by the caller.
 *
 * Instead of using this routine, please use subdeviceGetByHandle then call
 * GPU_RES_GET_GPU, RES_GET_HANDLE, GPU_RES_SET_THREAD_BC_STATE as needed.
 *
 * Note that GPU_RES_GET_GPU supports returning a pGpu for both pDevice,
 * pSubdevice, the base pResource type, and any resource that inherits from
 * GpuResource. That is, instead of using CliSetGpuContext or
 * CliSetSubDeviceContext, please use following pattern to look up the pGpu:
 *
 * OBJGPU *pGpu = GPU_RES_GET_GPU(pResource or pResourceRef->pResource)
 *
 * To set the threadstate, please use:
 *
 * GPU_RES_SET_THREAD_BC_STATE(pResource or pResourceRef->pResource);
 */
NV_STATUS CliSetSubDeviceContext(NvHandle hClient, NvHandle hSubdevice, NvHandle *phDevice,
                                 struct OBJGPU **ppGpu);

#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_SUBDEVICE_NVOC_H_
