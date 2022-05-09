#ifndef _G_KERNEL_NVLINK_NVOC_H_
#define _G_KERNEL_NVLINK_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_nvlink_nvoc.h"

#ifndef _KERNEL_NVLINK_H_
#define _KERNEL_NVLINK_H_

#include "core/core.h"
#include "core/locks.h"
#include "gpu/eng_state.h"
#include "lib/ref_count.h"
#include "nvCpuUuid.h"
#include "gpu/bus/kern_bus.h"

#if defined(INCLUDE_NVLINK_LIB)
#include "nvlink.h"
#include "nvlink_export.h"
#endif

#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#include "ctrl/ctrl2080/ctrl2080nvlink.h" // rmcontrol params

#if defined(INCLUDE_NVLINK_LIB)

typedef struct _def_knvlink_conn_info
{
    NvU32  domain;
    NvU16  bus;
    NvU16  device;
    NvU16  function;
    NvU32  pciDeviceId;
    NvU8   devUuid[NV_UUID_LEN];
    NvU64  deviceType;
    NvU32  linkNumber;
    NvU32  ipVerDlPl;
    NvBool bConnected;
    NvU64  chipSid;
} KNVLINK_CONN_INFO, *PKNVLINK_CONN_INFO;

#endif

// Known versions (taken from nvlinkip_discovery.h NV_NVLINKIP_DISCOVERY_COMMON_VERSION)
#define NVLINK_VERSION_10     0x00000001
#define NVLINK_VERSION_20     0x00000002
#define NVLINK_VERSION_22     0x00000004
#define NVLINK_VERSION_30     0x00000005
#define NVLINK_VERSION_31     0x00000006

// Maximum links the GPU NVLink SW can currently support
#define NVLINK_MAX_LINKS_SW             12

// Maximum IOCTRLs supported in SW
#define NVLINK_MAX_IOCTRLS_SW           3

// NvLink Phase Identifiers
#define NVLINK_PHASE_STATE_LOAD         0xFF000001
#define NVLINK_PHASE_STATE_POST_LOAD    0xFF000002

/******** NVLink associated timeouts and delays ***********/

// INITOPTIMIZE timeout = 10s
#define NVLINK_INITOPTIMIZE_POLL_TIMEOUT                10000000
#define NVLINK_INITOPTIMIZE_POLL_TIMEOUT_EMU            20000000
#define NVLINK_INITOPTIMIZE_POLL_COUNT_DELAY_MS         1000

/**********************************************************/

// NvGpu identifier in nvlink core library
#define NVLINK_NVIDIA_DRIVER            "NVIDIA GPU DRIVER"

#define NVLINK_DRIVER_NAME_LENGTH       0x0000040
#define NVLINK_DEVICE_NAME_LENGTH       0x0000040
#define NVLINK_LINK_NAME_LENGTH         0x0000040

//
// Arch CONNECTION defines, replaces forceconfig. See Bugs 1665737,
// 1665734 and 1734252.
// This per link connection state is passed up from chiplib
// and can be controlled on the command line.
// The max number of connections is speced in __SIZE_1.
//
#define NV_NVLINK_ARCH_CONNECTION                         31:0
#define NV_NVLINK_ARCH_CONNECTION__SIZE_1                   32
#define NV_NVLINK_ARCH_CONNECTION_DISABLED          0x00000000
#define NV_NVLINK_ARCH_CONNECTION_PEER_MASK                7:0
#define NV_NVLINK_ARCH_CONNECTION_ENABLED                  8:8
#define NV_NVLINK_ARCH_CONNECTION_PHYSICAL_LINK          20:16
#define NV_NVLINK_ARCH_CONNECTION_RESERVED               29:21
#define NV_NVLINK_ARCH_CONNECTION_PEERS_COMPUTE_ONLY     30:30
#define NV_NVLINK_ARCH_CONNECTION_CPU                    31:31

// Invalid fabric address
#define NVLINK_INVALID_FABRIC_ADDR  NV_U64_MAX

#define NVLINK_MAX_PEERS_SW 8

// PCI Device IDs and types used for ForceConfig

// FORCED_SYSMEM uses the ebridge device from NVLink1
#define FORCED_SYSMEM_PCI_BUS     0xe
#define FORCED_SYSMEM_DEVICE_ID   0x10ec
#define FORCED_SYSMEM_DEVICE_TYPE NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_EBRIDGE

//
// FORCED_SWITCH uses the first Switch device (Willow)
// Willow device ID is 0x10F5 or 0x1AC0..0x1ACF
//
#define FORCED_SWITCH_PCI_BUS     0xe
#define FORCED_SWITCH_DEVICE_ID   0x10F5
#define FORCED_SWITCH_DEVICE_TYPE NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH

//
// Structure representing per link information
//
typedef struct _def_knvlink_link
{
#if defined(INCLUDE_NVLINK_LIB)

    // Reference to link object registered with core lib
    nvlink_link *core_link;

    // Nvlink connection information
    KNVLINK_CONN_INFO remoteEndInfo;

    // OS-specific data associated with the link
    void   *pOsInfo;

#endif

    OBJGPU   *pGpu;
    NvU8      linkId;
    NvBool    bValid;

    // IOCTRL id that this link is associated with
    NvU32     ioctrlId;

    // DLPL IP version for the link
    NvU32     ipVerDlPl;

    // PLL sharing information
    NvU8      pllMasterLinkId;
    NvU8      pllSlaveLinkId;

    // RXDET per-lane status
    NvU32     laneRxdetStatusMask;

} KNVLINK_RM_LINK, *PKNVLINK_RM_LINK;


/*!
 * KernelNvlink is a logical abstraction of the GPU Nvlink Engine. The
 * Public API of the Nvlink Engine is exposed through this object, and
 * any interfaces which do not manage the underlying Nvlink hardware
 * can be managed by this object.
 */
#ifdef NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelNvlink {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelNvlink *__nvoc_pbase_KernelNvlink;
    NV_STATUS (*__knvlinkConstructEngine__)(OBJGPU *, struct KernelNvlink *, ENGDESCRIPTOR);
    NV_STATUS (*__knvlinkStatePreInitLocked__)(OBJGPU *, struct KernelNvlink *);
    NV_STATUS (*__knvlinkStateLoad__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkStatePostLoad__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkStateUnload__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkStatePostUnload__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NvBool (*__knvlinkIsPresent__)(OBJGPU *, struct KernelNvlink *);
    NV_STATUS (*__knvlinkValidateFabricBaseAddress__)(OBJGPU *, struct KernelNvlink *, NvU64);
    NvU32 (*__knvlinkGetConnectedLinksMask__)(OBJGPU *, struct KernelNvlink *);
    NV_STATUS (*__knvlinkEnableLinksPostTopology__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkOverrideConfig__)(OBJGPU *, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkFilterBridgeLinks__)(OBJGPU *, struct KernelNvlink *);
    NvU32 (*__knvlinkGetUniquePeerIdMask__)(OBJGPU *, struct KernelNvlink *);
    NvU32 (*__knvlinkGetUniquePeerId__)(OBJGPU *, struct KernelNvlink *, OBJGPU *);
    NV_STATUS (*__knvlinkRemoveMapping__)(OBJGPU *, struct KernelNvlink *, NvBool, NvU32, NvBool);
    NV_STATUS (*__knvlinkGetP2POptimalCEs__)(OBJGPU *, struct KernelNvlink *, NvU32, NvU32 *, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__knvlinkConstructHal__)(OBJGPU *, struct KernelNvlink *);
    void (*__knvlinkSetupPeerMapping__)(OBJGPU *, struct KernelNvlink *, OBJGPU *, NvU32);
    NV_STATUS (*__knvlinkProgramLinkSpeed__)(OBJGPU *, struct KernelNvlink *);
    NvBool (*__knvlinkPoweredUpForD3__)(OBJGPU *, struct KernelNvlink *);
    NV_STATUS (*__knvlinkReconcileTunableState__)(POBJGPU, struct KernelNvlink *, void *);
    NV_STATUS (*__knvlinkStateInitLocked__)(POBJGPU, struct KernelNvlink *);
    NV_STATUS (*__knvlinkStatePreLoad__)(POBJGPU, struct KernelNvlink *, NvU32);
    void (*__knvlinkStateDestroy__)(POBJGPU, struct KernelNvlink *);
    NV_STATUS (*__knvlinkStatePreUnload__)(POBJGPU, struct KernelNvlink *, NvU32);
    NV_STATUS (*__knvlinkStateInitUnlocked__)(POBJGPU, struct KernelNvlink *);
    void (*__knvlinkInitMissing__)(POBJGPU, struct KernelNvlink *);
    NV_STATUS (*__knvlinkStatePreInitUnlocked__)(POBJGPU, struct KernelNvlink *);
    NV_STATUS (*__knvlinkGetTunableState__)(POBJGPU, struct KernelNvlink *, void *);
    NV_STATUS (*__knvlinkCompareTunableState__)(POBJGPU, struct KernelNvlink *, void *, void *);
    void (*__knvlinkFreeTunableState__)(POBJGPU, struct KernelNvlink *, void *);
    NV_STATUS (*__knvlinkAllocTunableState__)(POBJGPU, struct KernelNvlink *, void **);
    NV_STATUS (*__knvlinkSetTunableState__)(POBJGPU, struct KernelNvlink *, void *);
    NvBool PDB_PROP_KNVLINK_ENABLED;
    NvBool PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED;
    NvBool PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED;
    NvBool PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD;
    NvBool PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN;
    NvBool PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE;
    NvBool PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING;
    NvBool PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED;
    struct KernelIoctrl *pKernelIoctrl[3];
    NvU32 ioctrlMask;
    NvU32 ipVerNvlink;
    NvU8 ioctrlNumEntries;
    NvU32 ioctrlSize;
    NvU32 registryControl;
    NvU32 minionControl;
    NvU32 verboseMask;
    NvU32 *pLinkConnection;
    NvBool bChiplibConfig;
    NvBool bRegistryLinkOverride;
    NvU32 registryLinkMask;
    NvBool bOverrideComputePeerMode;
    NvU32 discoveredLinks;
    NvU32 vbiosDisabledLinkMask;
    NvU32 regkeyDisabledLinksMask;
    NvU32 initDisabledLinksMask;
    NvU32 connectedLinksMask;
    NvU32 bridgeSensableLinks;
    NvU32 bridgedLinks;
    NvU32 enabledLinks;
    NvU32 initializedLinks;
    KNVLINK_RM_LINK nvlinkLinks[12];
    NvU32 postRxDetLinkMask;
    NvU32 disconnectedLinkMask;
    NvU32 sysmemLinkMask;
    NvU32 peerLinkMasks[32];
    NvU32 forcedSysmemDeviceType;
    nvlink_device *pNvlinkDev;
    NvU32 deviceLockRefcount;
    NvBool bVerifTrainingEnable;
    NvBool bL2Entry;
    NvBool bSkipLinkTraining;
    NvBool bForceAutoconfig;
    NvBool bForceEnableCoreLibRtlsims;
    NvBool bEnableTrainingAtLoad;
    NvBool bEnableSafeModeAtLoad;
    NvBool bDisableSingleLaneMode;
    NvBool bDisableL2Mode;
    NvU32 nvlinkLinkSpeed;
    NvU32 errorRecoveries[12];
    NvBool bNvswitchProxy;
    NvU64 fabricBaseAddr;
};

#ifndef __NVOC_CLASS_KernelNvlink_TYPEDEF__
#define __NVOC_CLASS_KernelNvlink_TYPEDEF__
typedef struct KernelNvlink KernelNvlink;
#endif /* __NVOC_CLASS_KernelNvlink_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelNvlink
#define __nvoc_class_id_KernelNvlink 0xce6818
#endif /* __nvoc_class_id_KernelNvlink */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvlink;

#define __staticCast_KernelNvlink(pThis) \
    ((pThis)->__nvoc_pbase_KernelNvlink)

#ifdef __nvoc_kernel_nvlink_h_disabled
#define __dynamicCast_KernelNvlink(pThis) ((KernelNvlink*)NULL)
#else //__nvoc_kernel_nvlink_h_disabled
#define __dynamicCast_KernelNvlink(pThis) \
    ((KernelNvlink*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelNvlink)))
#endif //__nvoc_kernel_nvlink_h_disabled

#define PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED_BASE_NAME PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED_BASE_NAME PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED
#define PDB_PROP_KNVLINK_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_ENABLED_BASE_NAME PDB_PROP_KNVLINK_ENABLED
#define PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED_BASE_CAST
#define PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED_BASE_NAME PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED
#define PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED_BASE_NAME PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED
#define PDB_PROP_KNVLINK_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KNVLINK_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING_BASE_CAST
#define PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING_BASE_NAME PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING
#define PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED_BASE_NAME PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED
#define PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6_BASE_CAST
#define PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6_BASE_NAME PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD_BASE_CAST
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD_BASE_NAME PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD
#define PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING_BASE_CAST
#define PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING_BASE_NAME PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING
#define PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE_BASE_CAST
#define PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE_BASE_NAME PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE
#define PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN_BASE_CAST
#define PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN_BASE_NAME PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN

NV_STATUS __nvoc_objCreateDynamic_KernelNvlink(KernelNvlink**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelNvlink(KernelNvlink**, Dynamic*, NvU32);
#define __objCreate_KernelNvlink(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelNvlink((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define knvlinkConstructEngine(arg0, arg1, arg2) knvlinkConstructEngine_DISPATCH(arg0, arg1, arg2)
#define knvlinkStatePreInitLocked(arg0, arg1) knvlinkStatePreInitLocked_DISPATCH(arg0, arg1)
#define knvlinkStateLoad(arg0, arg1, arg2) knvlinkStateLoad_DISPATCH(arg0, arg1, arg2)
#define knvlinkStatePostLoad(arg0, arg1, arg2) knvlinkStatePostLoad_DISPATCH(arg0, arg1, arg2)
#define knvlinkStateUnload(arg0, arg1, arg2) knvlinkStateUnload_DISPATCH(arg0, arg1, arg2)
#define knvlinkStatePostUnload(arg0, arg1, arg2) knvlinkStatePostUnload_DISPATCH(arg0, arg1, arg2)
#define knvlinkIsPresent(arg0, arg1) knvlinkIsPresent_DISPATCH(arg0, arg1)
#define knvlinkValidateFabricBaseAddress(pGpu, pKernelNvlink, arg0) knvlinkValidateFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkValidateFabricBaseAddress_HAL(pGpu, pKernelNvlink, arg0) knvlinkValidateFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkGetConnectedLinksMask(pGpu, pKernelNvlink) knvlinkGetConnectedLinksMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetConnectedLinksMask_HAL(pGpu, pKernelNvlink) knvlinkGetConnectedLinksMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkEnableLinksPostTopology(pGpu, pKernelNvlink, arg0) knvlinkEnableLinksPostTopology_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, arg0) knvlinkEnableLinksPostTopology_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkOverrideConfig(pGpu, pKernelNvlink, arg0) knvlinkOverrideConfig_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkOverrideConfig_HAL(pGpu, pKernelNvlink, arg0) knvlinkOverrideConfig_DISPATCH(pGpu, pKernelNvlink, arg0)
#define knvlinkFilterBridgeLinks(pGpu, pKernelNvlink) knvlinkFilterBridgeLinks_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink) knvlinkFilterBridgeLinks_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerIdMask(pGpu, pKernelNvlink) knvlinkGetUniquePeerIdMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerIdMask_HAL(pGpu, pKernelNvlink) knvlinkGetUniquePeerIdMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerId(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetUniquePeerId_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu)
#define knvlinkGetUniquePeerId_HAL(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetUniquePeerId_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu)
#define knvlinkRemoveMapping(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry) knvlinkRemoveMapping_DISPATCH(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry)
#define knvlinkRemoveMapping_HAL(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry) knvlinkRemoveMapping_DISPATCH(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry)
#define knvlinkGetP2POptimalCEs(pGpu, pKernelNvlink, arg0, arg1, arg2, arg3, arg4) knvlinkGetP2POptimalCEs_DISPATCH(pGpu, pKernelNvlink, arg0, arg1, arg2, arg3, arg4)
#define knvlinkGetP2POptimalCEs_HAL(pGpu, pKernelNvlink, arg0, arg1, arg2, arg3, arg4) knvlinkGetP2POptimalCEs_DISPATCH(pGpu, pKernelNvlink, arg0, arg1, arg2, arg3, arg4)
#define knvlinkConstructHal(pGpu, pKernelNvlink) knvlinkConstructHal_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkConstructHal_HAL(pGpu, pKernelNvlink) knvlinkConstructHal_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkSetupPeerMapping(pGpu, pKernelNvlink, pRemoteGpu, peerId) knvlinkSetupPeerMapping_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, peerId)
#define knvlinkSetupPeerMapping_HAL(pGpu, pKernelNvlink, pRemoteGpu, peerId) knvlinkSetupPeerMapping_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, peerId)
#define knvlinkProgramLinkSpeed(pGpu, pKernelNvlink) knvlinkProgramLinkSpeed_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkProgramLinkSpeed_HAL(pGpu, pKernelNvlink) knvlinkProgramLinkSpeed_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPoweredUpForD3(pGpu, pKernelNvlink) knvlinkPoweredUpForD3_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink) knvlinkPoweredUpForD3_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkReconcileTunableState(pGpu, pEngstate, pTunableState) knvlinkReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvlinkStateInitLocked(pGpu, pEngstate) knvlinkStateInitLocked_DISPATCH(pGpu, pEngstate)
#define knvlinkStatePreLoad(pGpu, pEngstate, arg0) knvlinkStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define knvlinkStateDestroy(pGpu, pEngstate) knvlinkStateDestroy_DISPATCH(pGpu, pEngstate)
#define knvlinkStatePreUnload(pGpu, pEngstate, arg0) knvlinkStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define knvlinkStateInitUnlocked(pGpu, pEngstate) knvlinkStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvlinkInitMissing(pGpu, pEngstate) knvlinkInitMissing_DISPATCH(pGpu, pEngstate)
#define knvlinkStatePreInitUnlocked(pGpu, pEngstate) knvlinkStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvlinkGetTunableState(pGpu, pEngstate, pTunableState) knvlinkGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvlinkCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) knvlinkCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define knvlinkFreeTunableState(pGpu, pEngstate, pTunableState) knvlinkFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvlinkAllocTunableState(pGpu, pEngstate, ppTunableState) knvlinkAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define knvlinkSetTunableState(pGpu, pEngstate, pTunableState) knvlinkSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
NvBool knvlinkIsForcedConfig_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsForcedConfig(OBJGPU *arg0, struct KernelNvlink *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsForcedConfig(arg0, arg1) knvlinkIsForcedConfig_IMPL(arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsForcedConfig_HAL(arg0, arg1) knvlinkIsForcedConfig(arg0, arg1)

NV_STATUS knvlinkApplyRegkeyOverrides_IMPL(OBJGPU *pGpu, struct KernelNvlink *arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkApplyRegkeyOverrides(OBJGPU *pGpu, struct KernelNvlink *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkApplyRegkeyOverrides(pGpu, arg0) knvlinkApplyRegkeyOverrides_IMPL(pGpu, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkApplyRegkeyOverrides_HAL(pGpu, arg0) knvlinkApplyRegkeyOverrides(pGpu, arg0)

NvBool knvlinkIsNvlinkDefaultEnabled_IMPL(OBJGPU *pGpu, struct KernelNvlink *arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvlinkDefaultEnabled(OBJGPU *pGpu, struct KernelNvlink *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvlinkDefaultEnabled(pGpu, arg0) knvlinkIsNvlinkDefaultEnabled_IMPL(pGpu, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvlinkDefaultEnabled_HAL(pGpu, arg0) knvlinkIsNvlinkDefaultEnabled(pGpu, arg0)

NvBool knvlinkIsP2pLoopbackSupported_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsP2pLoopbackSupported(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsP2pLoopbackSupported(pGpu, pKernelNvlink) knvlinkIsP2pLoopbackSupported_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsP2pLoopbackSupported_HAL(pGpu, pKernelNvlink) knvlinkIsP2pLoopbackSupported(pGpu, pKernelNvlink)

NvBool knvlinkIsP2pLoopbackSupportedPerLink_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsP2pLoopbackSupportedPerLink(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, arg0) knvlinkIsP2pLoopbackSupportedPerLink_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsP2pLoopbackSupportedPerLink_HAL(pGpu, pKernelNvlink, arg0) knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, arg0)

NvBool knvlinkIsNvlinkP2pSupported_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvlinkP2pSupported(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvlinkP2pSupported(pGpu, pKernelNvlink, pPeerGpu) knvlinkIsNvlinkP2pSupported_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvlinkP2pSupported_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkIsNvlinkP2pSupported(pGpu, pKernelNvlink, pPeerGpu)

NvBool knvlinkCheckNvswitchP2pConfig_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkCheckNvswitchP2pConfig(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCheckNvswitchP2pConfig(pGpu, pKernelNvlink, pPeerGpu) knvlinkCheckNvswitchP2pConfig_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCheckNvswitchP2pConfig_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkCheckNvswitchP2pConfig(pGpu, pKernelNvlink, pPeerGpu)

NV_STATUS knvlinkGetP2pConnectionStatus_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkGetP2pConnectionStatus(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu) knvlinkGetP2pConnectionStatus_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetP2pConnectionStatus_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu)

NV_STATUS knvlinkUpdateCurrentConfig_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdateCurrentConfig(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink) knvlinkUpdateCurrentConfig_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdateCurrentConfig_HAL(pGpu, pKernelNvlink) knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink)

void knvlinkCoreDriverLoadWar_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkCoreDriverLoadWar(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreDriverLoadWar(pGpu, pKernelNvlink) knvlinkCoreDriverLoadWar_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreDriverLoadWar_HAL(pGpu, pKernelNvlink) knvlinkCoreDriverLoadWar(pGpu, pKernelNvlink)

void knvlinkCoreDriverUnloadWar_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkCoreDriverUnloadWar(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreDriverUnloadWar(pGpu, pKernelNvlink) knvlinkCoreDriverUnloadWar_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreDriverUnloadWar_HAL(pGpu, pKernelNvlink) knvlinkCoreDriverUnloadWar(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreIsDriverSupported_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreIsDriverSupported(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreIsDriverSupported(pGpu, pKernelNvlink) knvlinkCoreIsDriverSupported_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreIsDriverSupported_HAL(pGpu, pKernelNvlink) knvlinkCoreIsDriverSupported(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreAddDevice_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreAddDevice(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreAddDevice(pGpu, pKernelNvlink) knvlinkCoreAddDevice_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreAddDevice_HAL(pGpu, pKernelNvlink) knvlinkCoreAddDevice(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreAddLink_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreAddLink(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreAddLink(pGpu, pKernelNvlink, arg0) knvlinkCoreAddLink_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreAddLink_HAL(pGpu, pKernelNvlink, arg0) knvlinkCoreAddLink(pGpu, pKernelNvlink, arg0)

NV_STATUS knvlinkCoreRemoveDevice_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreRemoveDevice(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreRemoveDevice(pGpu, pKernelNvlink) knvlinkCoreRemoveDevice_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreRemoveDevice_HAL(pGpu, pKernelNvlink) knvlinkCoreRemoveDevice(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreRemoveLink_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreRemoveLink(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreRemoveLink(pGpu, pKernelNvlink, arg0) knvlinkCoreRemoveLink_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreRemoveLink_HAL(pGpu, pKernelNvlink, arg0) knvlinkCoreRemoveLink(pGpu, pKernelNvlink, arg0)

NV_STATUS knvlinkCoreShutdownDeviceLinks_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreShutdownDeviceLinks(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink) knvlinkCoreShutdownDeviceLinks_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreShutdownDeviceLinks_HAL(pGpu, pKernelNvlink) knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreResetDeviceLinks_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreResetDeviceLinks(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreResetDeviceLinks(pGpu, pKernelNvlink) knvlinkCoreResetDeviceLinks_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreResetDeviceLinks_HAL(pGpu, pKernelNvlink) knvlinkCoreResetDeviceLinks(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreUpdateDeviceUUID_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreUpdateDeviceUUID(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreUpdateDeviceUUID(pGpu, pKernelNvlink) knvlinkCoreUpdateDeviceUUID_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreUpdateDeviceUUID_HAL(pGpu, pKernelNvlink) knvlinkCoreUpdateDeviceUUID(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreGetRemoteDeviceInfo_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreGetRemoteDeviceInfo(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink) knvlinkCoreGetRemoteDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreGetRemoteDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink)

NvBool knvlinkIsGpuConnectedToNvswitch_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsGpuConnectedToNvswitch(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink) knvlinkIsGpuConnectedToNvswitch_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsGpuConnectedToNvswitch_HAL(pGpu, pKernelNvlink) knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink)

NvBool knvlinkIsLinkConnected_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsLinkConnected(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsLinkConnected(pGpu, pKernelNvlink, arg0) knvlinkIsLinkConnected_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsLinkConnected_HAL(pGpu, pKernelNvlink, arg0) knvlinkIsLinkConnected(pGpu, pKernelNvlink, arg0)

NV_STATUS knvlinkTrainSysmemLinksToActive_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainSysmemLinksToActive(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainSysmemLinksToActive(pGpu, pKernelNvlink) knvlinkTrainSysmemLinksToActive_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainSysmemLinksToActive_HAL(pGpu, pKernelNvlink) knvlinkTrainSysmemLinksToActive(pGpu, pKernelNvlink)

NV_STATUS knvlinkTrainP2pLinksToActive_IMPL(OBJGPU *pGpu0, OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainP2pLinksToActive(OBJGPU *pGpu0, OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainP2pLinksToActive(pGpu0, pGpu1, pKernelNvlink) knvlinkTrainP2pLinksToActive_IMPL(pGpu0, pGpu1, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainP2pLinksToActive_HAL(pGpu0, pGpu1, pKernelNvlink) knvlinkTrainP2pLinksToActive(pGpu0, pGpu1, pKernelNvlink)

NV_STATUS knvlinkCheckTrainingIsComplete_IMPL(OBJGPU *pGpu0, OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCheckTrainingIsComplete(OBJGPU *pGpu0, OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCheckTrainingIsComplete(pGpu0, pGpu1, pKernelNvlink) knvlinkCheckTrainingIsComplete_IMPL(pGpu0, pGpu1, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCheckTrainingIsComplete_HAL(pGpu0, pGpu1, pKernelNvlink) knvlinkCheckTrainingIsComplete(pGpu0, pGpu1, pKernelNvlink)

NV_STATUS knvlinkTrainFabricLinksToActive_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainFabricLinksToActive(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainFabricLinksToActive(pGpu, pKernelNvlink) knvlinkTrainFabricLinksToActive_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainFabricLinksToActive_HAL(pGpu, pKernelNvlink) knvlinkTrainFabricLinksToActive(pGpu, pKernelNvlink)

NV_STATUS knvlinkRetrainLink_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId, NvBool bFromOff);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkRetrainLink(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId, NvBool bFromOff) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkRetrainLink(pGpu, pKernelNvlink, linkId, bFromOff) knvlinkRetrainLink_IMPL(pGpu, pKernelNvlink, linkId, bFromOff)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkRetrainLink_HAL(pGpu, pKernelNvlink, linkId, bFromOff) knvlinkRetrainLink(pGpu, pKernelNvlink, linkId, bFromOff)

NvU32 knvlinkGetEnabledLinkMask_IMPL(OBJGPU *pGpu, struct KernelNvlink *arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetEnabledLinkMask(OBJGPU *pGpu, struct KernelNvlink *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetEnabledLinkMask(pGpu, arg0) knvlinkGetEnabledLinkMask_IMPL(pGpu, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetEnabledLinkMask_HAL(pGpu, arg0) knvlinkGetEnabledLinkMask(pGpu, arg0)

NvU32 knvlinkGetDiscoveredLinkMask_IMPL(OBJGPU *pGpu, struct KernelNvlink *arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetDiscoveredLinkMask(OBJGPU *pGpu, struct KernelNvlink *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetDiscoveredLinkMask(pGpu, arg0) knvlinkGetDiscoveredLinkMask_IMPL(pGpu, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetDiscoveredLinkMask_HAL(pGpu, arg0) knvlinkGetDiscoveredLinkMask(pGpu, arg0)

NV_STATUS knvlinkProcessInitDisabledLinks_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkProcessInitDisabledLinks(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkProcessInitDisabledLinks(pGpu, pKernelNvlink) knvlinkProcessInitDisabledLinks_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkProcessInitDisabledLinks_HAL(pGpu, pKernelNvlink) knvlinkProcessInitDisabledLinks(pGpu, pKernelNvlink)

NvU32 knvlinkGetNumLinksToSystem_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetNumLinksToSystem(OBJGPU *arg0, struct KernelNvlink *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetNumLinksToSystem(arg0, arg1) knvlinkGetNumLinksToSystem_IMPL(arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetNumLinksToSystem_HAL(arg0, arg1) knvlinkGetNumLinksToSystem(arg0, arg1)

NvU32 knvlinkGetNumLinksToPeer_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetNumLinksToPeer(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetNumLinksToPeer_IMPL(pGpu, pKernelNvlink, pRemoteGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetNumLinksToPeer_HAL(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu)

NvU32 knvlinkGetLinkMaskToPeer_IMPL(OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, OBJGPU *pGpu1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetLinkMaskToPeer(OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, OBJGPU *pGpu1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1) knvlinkGetLinkMaskToPeer_IMPL(pGpu0, pKernelNvlink0, pGpu1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetLinkMaskToPeer_HAL(pGpu0, pKernelNvlink0, pGpu1) knvlinkGetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1)

NV_STATUS knvlinkSetLinkMaskToPeer_IMPL(OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, OBJGPU *pGpu1, NvU32 peerLinkMask);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetLinkMaskToPeer(OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, OBJGPU *pGpu1, NvU32 peerLinkMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask) knvlinkSetLinkMaskToPeer_IMPL(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetLinkMaskToPeer_HAL(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask) knvlinkSetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask)

NvU32 knvlinkGetPeersNvlinkMaskFromHshub_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetPeersNvlinkMaskFromHshub(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetPeersNvlinkMaskFromHshub(pGpu, pKernelNvlink) knvlinkGetPeersNvlinkMaskFromHshub_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetPeersNvlinkMaskFromHshub_HAL(pGpu, pKernelNvlink) knvlinkGetPeersNvlinkMaskFromHshub(pGpu, pKernelNvlink)

NV_STATUS knvlinkPrepareForXVEReset_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkPrepareForXVEReset(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkPrepareForXVEReset(pGpu, pKernelNvlink) knvlinkPrepareForXVEReset_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkPrepareForXVEReset_HAL(pGpu, pKernelNvlink) knvlinkPrepareForXVEReset(pGpu, pKernelNvlink)

NV_STATUS knvlinkEnterExitSleep_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkEnterExitSleep(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkEnterExitSleep(pGpu, pKernelNvlink, arg0, arg1) knvlinkEnterExitSleep_IMPL(pGpu, pKernelNvlink, arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkEnterExitSleep_HAL(pGpu, pKernelNvlink, arg0, arg1) knvlinkEnterExitSleep(pGpu, pKernelNvlink, arg0, arg1)

NV_STATUS knvlinkSyncLinkMasksAndVbiosInfo_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSyncLinkMasksAndVbiosInfo(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink) knvlinkSyncLinkMasksAndVbiosInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSyncLinkMasksAndVbiosInfo_HAL(pGpu, pKernelNvlink) knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkUpdateLinkConnectionStatus_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdateLinkConnectionStatus(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, arg0) knvlinkUpdateLinkConnectionStatus_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdateLinkConnectionStatus_HAL(pGpu, pKernelNvlink, arg0) knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, arg0)

NV_STATUS knvlinkPreTrainLinksToActiveAli_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkPreTrainLinksToActiveAli(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkPreTrainLinksToActiveAli(pGpu, pKernelNvlink, arg0, arg1) knvlinkPreTrainLinksToActiveAli_IMPL(pGpu, pKernelNvlink, arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkPreTrainLinksToActiveAli_HAL(pGpu, pKernelNvlink, arg0, arg1) knvlinkPreTrainLinksToActiveAli(pGpu, pKernelNvlink, arg0, arg1)

NV_STATUS knvlinkTrainLinksToActiveAli_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainLinksToActiveAli(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvBool arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainLinksToActiveAli(pGpu, pKernelNvlink, arg0, arg1) knvlinkTrainLinksToActiveAli_IMPL(pGpu, pKernelNvlink, arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainLinksToActiveAli_HAL(pGpu, pKernelNvlink, arg0, arg1) knvlinkTrainLinksToActiveAli(pGpu, pKernelNvlink, arg0, arg1)

NV_STATUS knvlinkUpdatePostRxDetectLinkMask_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdatePostRxDetectLinkMask(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdatePostRxDetectLinkMask(pGpu, pKernelNvlink) knvlinkUpdatePostRxDetectLinkMask_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdatePostRxDetectLinkMask_HAL(pGpu, pKernelNvlink) knvlinkUpdatePostRxDetectLinkMask(pGpu, pKernelNvlink)

NV_STATUS knvlinkCopyNvlinkDeviceInfo_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCopyNvlinkDeviceInfo(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink) knvlinkCopyNvlinkDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCopyNvlinkDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkCopyIoctrlDeviceInfo_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCopyIoctrlDeviceInfo(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCopyIoctrlDeviceInfo(pGpu, pKernelNvlink) knvlinkCopyIoctrlDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCopyIoctrlDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCopyIoctrlDeviceInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkSetupTopologyForForcedConfig_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetupTopologyForForcedConfig(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetupTopologyForForcedConfig(pGpu, pKernelNvlink) knvlinkSetupTopologyForForcedConfig_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetupTopologyForForcedConfig_HAL(pGpu, pKernelNvlink) knvlinkSetupTopologyForForcedConfig(pGpu, pKernelNvlink)

NV_STATUS knvlinkSyncLaneShutdownProps_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSyncLaneShutdownProps(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSyncLaneShutdownProps(pGpu, pKernelNvlink) knvlinkSyncLaneShutdownProps_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSyncLaneShutdownProps_HAL(pGpu, pKernelNvlink) knvlinkSyncLaneShutdownProps(pGpu, pKernelNvlink)

void knvlinkSetPowerFeatures_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkSetPowerFeatures(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetPowerFeatures(pGpu, pKernelNvlink) knvlinkSetPowerFeatures_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetPowerFeatures_HAL(pGpu, pKernelNvlink) knvlinkSetPowerFeatures(pGpu, pKernelNvlink)

NV_STATUS knvlinkExecGspRmRpc_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, void *arg1, NvU32 arg2);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkExecGspRmRpc(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, void *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkExecGspRmRpc(pGpu, pKernelNvlink, arg0, arg1, arg2) knvlinkExecGspRmRpc_IMPL(pGpu, pKernelNvlink, arg0, arg1, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkExecGspRmRpc_HAL(pGpu, pKernelNvlink, arg0, arg1, arg2) knvlinkExecGspRmRpc(pGpu, pKernelNvlink, arg0, arg1, arg2)

NvBool knvlinkIsNvswitchProxyPresent_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvswitchProxyPresent(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink) knvlinkIsNvswitchProxyPresent_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvswitchProxyPresent_HAL(pGpu, pKernelNvlink) knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink)

void knvlinkDetectNvswitchProxy_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkDetectNvswitchProxy(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkDetectNvswitchProxy(pGpu, pKernelNvlink) knvlinkDetectNvswitchProxy_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkDetectNvswitchProxy_HAL(pGpu, pKernelNvlink) knvlinkDetectNvswitchProxy(pGpu, pKernelNvlink)

NV_STATUS knvlinkSetUniqueFlaBaseAddress_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetUniqueFlaBaseAddress(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, arg0) knvlinkSetUniqueFlaBaseAddress_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetUniqueFlaBaseAddress_HAL(pGpu, pKernelNvlink, arg0) knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, arg0)

static inline NvU64 knvlinkGetUniqueFabricBaseAddress_72249a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->fabricBaseAddr;
}

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetUniqueFabricBaseAddress(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink) knvlinkGetUniqueFabricBaseAddress_72249a(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink) knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink)

NV_STATUS knvlinkSetUniqueFabricBaseAddress_IMPL(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetUniqueFabricBaseAddress(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetUniqueFabricBaseAddress(pGpu, pKernelNvlink, arg0) knvlinkSetUniqueFabricBaseAddress_IMPL(pGpu, pKernelNvlink, arg0)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink, arg0) knvlinkSetUniqueFabricBaseAddress(pGpu, pKernelNvlink, arg0)

NV_STATUS knvlinkStatePostLoadHal_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkStatePostLoadHal(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkStatePostLoadHal(pGpu, pKernelNvlink) knvlinkStatePostLoadHal_GV100(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkStatePostLoadHal_HAL(pGpu, pKernelNvlink) knvlinkStatePostLoadHal(pGpu, pKernelNvlink)

NV_STATUS knvlinkApplyNvswitchDegradedModeSettings_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 *switchLinkMasks);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkApplyNvswitchDegradedModeSettings(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 *switchLinkMasks) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkApplyNvswitchDegradedModeSettings(pGpu, pKernelNvlink, switchLinkMasks) knvlinkApplyNvswitchDegradedModeSettings_GV100(pGpu, pKernelNvlink, switchLinkMasks)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkApplyNvswitchDegradedModeSettings_HAL(pGpu, pKernelNvlink, switchLinkMasks) knvlinkApplyNvswitchDegradedModeSettings(pGpu, pKernelNvlink, switchLinkMasks)

static inline NV_STATUS knvlinkDiscoverPostRxDetLinks_46f6a7(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu) {
    return NV_ERR_NOT_SUPPORTED;
}

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkDiscoverPostRxDetLinks(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkDiscoverPostRxDetLinks(pGpu, pKernelNvlink, pPeerGpu) knvlinkDiscoverPostRxDetLinks_46f6a7(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkDiscoverPostRxDetLinks_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkDiscoverPostRxDetLinks(pGpu, pKernelNvlink, pPeerGpu)

NV_STATUS knvlinkConstructEngine_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS knvlinkConstructEngine_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__knvlinkConstructEngine__(arg0, arg1, arg2);
}

NV_STATUS knvlinkStatePreInitLocked_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1);

static inline NV_STATUS knvlinkStatePreInitLocked_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1) {
    return arg1->__knvlinkStatePreInitLocked__(arg0, arg1);
}

NV_STATUS knvlinkStateLoad_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2);

static inline NV_STATUS knvlinkStateLoad_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2) {
    return arg1->__knvlinkStateLoad__(arg0, arg1, arg2);
}

NV_STATUS knvlinkStatePostLoad_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2);

static inline NV_STATUS knvlinkStatePostLoad_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2) {
    return arg1->__knvlinkStatePostLoad__(arg0, arg1, arg2);
}

NV_STATUS knvlinkStateUnload_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2);

static inline NV_STATUS knvlinkStateUnload_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2) {
    return arg1->__knvlinkStateUnload__(arg0, arg1, arg2);
}

NV_STATUS knvlinkStatePostUnload_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2);

static inline NV_STATUS knvlinkStatePostUnload_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1, NvU32 arg2) {
    return arg1->__knvlinkStatePostUnload__(arg0, arg1, arg2);
}

NvBool knvlinkIsPresent_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1);

static inline NvBool knvlinkIsPresent_DISPATCH(OBJGPU *arg0, struct KernelNvlink *arg1) {
    return arg1->__knvlinkIsPresent__(arg0, arg1);
}

static inline NV_STATUS knvlinkValidateFabricBaseAddress_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0) {
    return NV_OK;
}

NV_STATUS knvlinkValidateFabricBaseAddress_GA100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0);

static inline NV_STATUS knvlinkValidateFabricBaseAddress_46f6a7(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS knvlinkValidateFabricBaseAddress_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg0) {
    return pKernelNvlink->__knvlinkValidateFabricBaseAddress__(pGpu, pKernelNvlink, arg0);
}

static inline NvU32 knvlinkGetConnectedLinksMask_15a734(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0U;
}

NvU32 knvlinkGetConnectedLinksMask_TU102(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvU32 knvlinkGetConnectedLinksMask_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetConnectedLinksMask__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkEnableLinksPostTopology_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS knvlinkEnableLinksPostTopology_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

static inline NV_STATUS knvlinkEnableLinksPostTopology_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    return pKernelNvlink->__knvlinkEnableLinksPostTopology__(pGpu, pKernelNvlink, arg0);
}

static inline NV_STATUS knvlinkOverrideConfig_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS knvlinkOverrideConfig_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

NV_STATUS knvlinkOverrideConfig_GA100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0);

static inline NV_STATUS knvlinkOverrideConfig_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0) {
    return pKernelNvlink->__knvlinkOverrideConfig__(pGpu, pKernelNvlink, arg0);
}

static inline NV_STATUS knvlinkFilterBridgeLinks_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkFilterBridgeLinks_TU102(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkFilterBridgeLinks_46f6a7(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS knvlinkFilterBridgeLinks_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkFilterBridgeLinks__(pGpu, pKernelNvlink);
}

static inline NvU32 knvlinkGetUniquePeerIdMask_4a4dee(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0;
}

NvU32 knvlinkGetUniquePeerIdMask_GP100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvU32 knvlinkGetUniquePeerIdMask_15a734(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0U;
}

static inline NvU32 knvlinkGetUniquePeerIdMask_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetUniquePeerIdMask__(pGpu, pKernelNvlink);
}

static inline NvU32 knvlinkGetUniquePeerId_c732fb(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu) {
    return 4294967295U;
}

NvU32 knvlinkGetUniquePeerId_GP100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu);

static inline NvU32 knvlinkGetUniquePeerId_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu) {
    return pKernelNvlink->__knvlinkGetUniquePeerId__(pGpu, pKernelNvlink, pRemoteGpu);
}

static inline NV_STATUS knvlinkRemoveMapping_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry) {
    return NV_OK;
}

NV_STATUS knvlinkRemoveMapping_GP100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry);

NV_STATUS knvlinkRemoveMapping_GA100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry);

static inline NV_STATUS knvlinkRemoveMapping_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry) {
    return pKernelNvlink->__knvlinkRemoveMapping__(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry);
}

static inline NV_STATUS knvlinkGetP2POptimalCEs_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 *arg4) {
    return NV_OK;
}

NV_STATUS knvlinkGetP2POptimalCEs_GP100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 *arg4);

static inline NV_STATUS knvlinkGetP2POptimalCEs_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 *arg4) {
    return pKernelNvlink->__knvlinkGetP2POptimalCEs__(pGpu, pKernelNvlink, arg0, arg1, arg2, arg3, arg4);
}

static inline NV_STATUS knvlinkConstructHal_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkConstructHal_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkConstructHal_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkConstructHal__(pGpu, pKernelNvlink);
}

static inline void knvlinkSetupPeerMapping_b3696a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu, NvU32 peerId) {
    return;
}

void knvlinkSetupPeerMapping_GP100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu, NvU32 peerId);

static inline void knvlinkSetupPeerMapping_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, OBJGPU *pRemoteGpu, NvU32 peerId) {
    pKernelNvlink->__knvlinkSetupPeerMapping__(pGpu, pKernelNvlink, pRemoteGpu, peerId);
}

static inline NV_STATUS knvlinkProgramLinkSpeed_56cd7a(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkProgramLinkSpeed_GV100(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkProgramLinkSpeed_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkProgramLinkSpeed__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkPoweredUpForD3_491d52(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return ((NvBool)(0 != 0));
}

NvBool knvlinkPoweredUpForD3_TU102(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkPoweredUpForD3_DISPATCH(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkPoweredUpForD3__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return pEngstate->__knvlinkReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvlinkStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__knvlinkStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, NvU32 arg0) {
    return pEngstate->__knvlinkStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline void knvlinkStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    pEngstate->__knvlinkStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, NvU32 arg0) {
    return pEngstate->__knvlinkStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvlinkStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__knvlinkStateInitUnlocked__(pGpu, pEngstate);
}

static inline void knvlinkInitMissing_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    pEngstate->__knvlinkInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__knvlinkStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return pEngstate->__knvlinkGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvlinkCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__knvlinkCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline void knvlinkFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    pEngstate->__knvlinkFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvlinkAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void **ppTunableState) {
    return pEngstate->__knvlinkAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS knvlinkSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return pEngstate->__knvlinkSetTunableState__(pGpu, pEngstate, pTunableState);
}

void knvlinkDestruct_IMPL(struct KernelNvlink *arg0);
#define __nvoc_knvlinkDestruct(arg0) knvlinkDestruct_IMPL(arg0)
NV_STATUS knvlinkRemoveMissingIoctrlObjects_IMPL(OBJGPU *arg0, struct KernelNvlink *arg1);
#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkRemoveMissingIoctrlObjects(OBJGPU *arg0, struct KernelNvlink *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkRemoveMissingIoctrlObjects(arg0, arg1) knvlinkRemoveMissingIoctrlObjects_IMPL(arg0, arg1)
#endif //__nvoc_kernel_nvlink_h_disabled

#undef PRIVATE_FIELD



// IOCTRL GET defines
#define KNVLINK_GET_IOCTRL(pKernelNvlink, PublicId)    (pKernelNvlink->pKernelIoctrl[PublicId])
#define KNVLINK_LINK_GET_IOCTRL(pKernelNvlink, linkId)          \
               (KNVLINK_GET_IOCTRL(pKernelNvlink, pKernelNvlink->nvlinkLinks[linkId].ioctrlId))

#define KNVLINK_IS_LINK_CONNECTED_TO_GPU(pKernelNvlink, linkId, pRemoteGpu)                     \
    ((pKernelNvlink != NULL) && (pRemoteGpu != NULL) &&                                         \
     (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected) &&                           \
     (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain   == gpuGetDomain(pRemoteGpu)) && \
     (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus      == gpuGetBus(pRemoteGpu))    && \
     (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device   == gpuGetDevice(pRemoteGpu)) && \
     (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.function == 0))

//
// NVLINK internal functions
//
NV_STATUS knvlinkRetrainLinkFromOff (OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId);
NV_STATUS knvlinkRetrainLinkFromSafe(OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId);

//
// NVLINK Callback functions from core library
//
#if defined(INCLUDE_NVLINK_LIB)
     
// Device callback functions
     
NvlStatus knvlinkCoreAddDeviceCallback   (nvlink_device *dev);
NvlStatus knvlinkCoreRemoveDeviceCallback(nvlink_device *dev);
     
// Link callback functions
     
NvlStatus knvlinkCoreAddLinkCallback                 (nvlink_link *link);
NvlStatus knvlinkCoreRemoveLinkCallback              (nvlink_link *link);
NvlStatus knvlinkCoreLockLinkCallback                (nvlink_link *link);
void      knvlinkCoreUnlockLinkCallback              (nvlink_link *link);
NvlStatus knvlinkCoreQueueLinkChangeCallback         (nvlink_link_change *link_change);
NvlStatus knvlinkCoreSetDlLinkModeCallback           (nvlink_link *link, NvU64  mode, NvU32 flags);
NvlStatus knvlinkCoreGetDlLinkModeCallback           (nvlink_link *link, NvU64 *mode);
NvlStatus knvlinkCoreSetTlLinkModeCallback           (nvlink_link *link, NvU64  mode, NvU32 flags);
NvlStatus knvlinkCoreGetTlLinkModeCallback           (nvlink_link *link, NvU64 *mode);
NvlStatus knvlinkCoreWriteDiscoveryTokenCallback     (nvlink_link *link, NvU64  token);
NvlStatus knvlinkCoreReadDiscoveryTokenCallback      (nvlink_link *link, NvU64 *token);
NvlStatus knvlinkCoreSetTxSublinkModeCallback        (nvlink_link *link, NvU64  mode, NvU32 flags);
NvlStatus knvlinkCoreSetRxSublinkModeCallback        (nvlink_link *link, NvU64  mode, NvU32 flags);
NvlStatus knvlinkCoreGetTxSublinkModeCallback        (nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus knvlinkCoreGetRxSublinkModeCallback        (nvlink_link *link, NvU64 *mode, NvU32 *subMode);
NvlStatus knvlinkCoreSetRxSublinkDetectCallback      (nvlink_link *link, NvU32  flags);
NvlStatus knvlinkCoreGetRxSublinkDetectCallback      (nvlink_link *link);
void      knvlinkCoreTrainingCompleteCallback        (nvlink_link *link);
void      knvlinkCoreGetUphyLoadCallback             (nvlink_link *link, NvBool *bUnlocked);
     
#endif

// NVLINK Utility Functions
void knvlinkUtoa(NvU8 *, NvU64, NvU64);

NV_STATUS knvlinkCtrlCmdBusGetNvlinkCaps(OBJGPU *pGpu, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams);

#endif // _KERNEL_NVLINK_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_NVLINK_NVOC_H_
