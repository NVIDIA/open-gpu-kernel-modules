
#ifndef _G_KERNEL_NVLINK_NVOC_H_
#define _G_KERNEL_NVLINK_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_nvlink_nvoc.h"

#ifndef _KERNEL_NVLINK_H_
#define _KERNEL_NVLINK_H_

#include "core/core.h"
#include "core/locks.h"
#include "gpu/eng_state.h"
#include "lib/ref_count.h"
#include "gpu/timer/objtmr.h"
#include "nvCpuUuid.h"
#include "gpu/bus/kern_bus.h"

#if defined(INCLUDE_NVLINK_LIB)
#include "nvlink.h"
#include "nvlink_export.h"
#include "nvlink_lib_ctrl.h"
#endif

#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#include "ctrl/ctrl2080/ctrl2080nvlink.h" // rmcontrol params
#include "ctrl/ctrl2080/ctrl2080pmgr.h"

#include "nvlink_inband_drv_header.h"
#include "nvlink_inband_msg.h"

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
#define NVLINK_VERSION_40     0x00000007
#define NVLINK_VERSION_50     0x00000008

// Maximum links the GPU NVLink SW can currently support
#define NVLINK_MAX_LINKS_SW             36

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

// Link Retrain after reset time = 10s
#define NVLINK_RETRAIN_TIME                             10000000000

// Recovery period allowing orphaned responses to return - 10s
#define NVLINK_UNCONTAINED_ERROR_IDLE_PERIOD_NS         10000000000

// Total recovery period allowed before aborting and triggering degradation - 30s
#define NVLINK_UNCONTAINED_ERROR_ABORT_PERIOD_NS        30000000000

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
#define NV_NVLINK_ARCH_CONNECTION_PHYSICAL_LINK          21:16
#define NV_NVLINK_ARCH_CONNECTION_RESERVED               29:20
#define NV_NVLINK_ARCH_CONNECTION_PEERS_COMPUTE_ONLY     30:30
#define NV_NVLINK_ARCH_CONNECTION_CPU                    31:31

// Invalid fabric address
#define NVLINK_INVALID_FABRIC_ADDR  NV_U64_MAX

// Shift value for node remap entries
#define NVLINK_NODE_REMAP_OFFSET_SHIFT 42

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
// Accessor to get a kernel nvlink mask in a bit format caller desires
// Note: only use this macro if the link mask is something other than a
// NvU32
//
#define KNVLINK_GET_MASK(pNvlink, mask, bits) \
    (NvU##bits)((bits == 32) ? ( NvU64_LO32(pNvlink->mask)) : (pNvlink->mask))

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

    struct OBJGPU   *pGpu;
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

    TMR_EVENT *pTmrEvent;

} KNVLINK_RM_LINK, *PKNVLINK_RM_LINK;

typedef struct NVLINK_INBAND_CALLBACK
{
    NvU32 messageType;
    NV_STATUS (*pCallback)(NvU32 gpuInstance, NvU64 *pNotifyGfIdMask,
                      NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pMessage);
    NvU32 wqItemFlags;
} NVLINK_INBAND_MSG_CALLBACK;

typedef struct
{
    NvU8 linkId;
} NVLINK_ID, *PNVLINK_ID;

MAKE_LIST(FaultUpList, NVLINK_ID);

/*!
 * KernelNvlink is a logical abstraction of the GPU Nvlink Engine. The
 * Public API of the Nvlink Engine is exposed through this object, and
 * any interfaces which do not manage the underlying Nvlink hardware
 * can be managed by this object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelNvlink;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelNvlink;


struct KernelNvlink {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelNvlink *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelNvlink *__nvoc_pbase_KernelNvlink;    // knvlink

    // Vtable with 41 per-object function pointers
    NvBool (*__knvlinkIsPresent__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual halified (2 hals) override (engstate) base (engstate)
    NV_STATUS (*__knvlinkSetUniqueFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    void (*__knvlinkClearUniqueFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkSetUniqueFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (2 hals) body
    void (*__knvlinkClearUniqueFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkHandleFaultUpInterrupt__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkValidateFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    NV_STATUS (*__knvlinkValidateFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    NvU32 (*__knvlinkGetConnectedLinksMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEnableLinksPostTopology__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkOverrideConfig__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__knvlinkFilterBridgeLinks__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvU32 (*__knvlinkGetUniquePeerIdMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvU32 (*__knvlinkGetUniquePeerId__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkRemoveMapping__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvBool, NvU32, NvBool);  // halified (3 hals) body
    NV_STATUS (*__knvlinkGetP2POptimalCEs__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32 *, NvU32 *, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkConstructHal__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkSetupPeerMapping__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkProgramLinkSpeed__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkPoweredUpForD3__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkIsAliSupported__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkPostSetupNvlinkPeer__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkDiscoverPostRxDetLinks__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkLogAliDebugMessages__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvBool);  // halified (3 hals) body
    void (*__knvlinkDumpCallbackRegister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkGetEffectivePeerLinkMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *, NvU64 *);  // halified (2 hals) body
    NvU32 (*__knvlinkGetNumLinksToBeReducedPerIoctrl__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsBandwidthModeOff__)(struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsBwModeSupported__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU8);  // halified (3 hals) body
    NV_STATUS (*__knvlinkGetHshubSupportedRbmModes__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkPostSchedulingEnableCallbackRegister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkPostSchedulingEnableCallbackUnregister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkGetSupportedBwMode__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *);  // halified (3 hals) body
    void (*__knvlinkDirectConnectCheck__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsGpuReducedNvlinkConfig__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsFloorSweepingNeeded__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkGetSupportedCounters__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *);  // halified (2 hals) body
    NvBool (*__knvlinkIsEncryptEnSet__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsNvleEnabled__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEncryptionGetGpuIdentifiers__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEncryptionUpdateTopology__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32);  // halified (2 hals) body

    // 19 PDB properties
    NvBool PDB_PROP_KNVLINK_ENABLED;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED;
    NvBool PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED;
    NvBool PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD;
    NvBool PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN;
    NvBool PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE;
    NvBool PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING;
    NvBool PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING;
    NvBool PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING;
    NvBool PDB_PROP_KNVLINK_MINION_GFW_BOOT;
    NvBool PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED;
    NvBool PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED;
    NvBool PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED;
    NvBool PDB_PROP_KNVLINK_ENCRYPTION_ENABLED;
    NvBool PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED;

    // Data members
    NvU8 nvlinkBwMode;
    NvU8 maxRbmLinks;
    NvU8 hshubSupportedRbmModesList[36];
    NvU8 totalRbmModes;
    NvU32 gspProxyRegkeys;
    NvU32 alid;
    NvU32 clid;
    NvBool bGotNvleIdentifiers;
    struct KernelIoctrl *PRIVATE_FIELD(pKernelIoctrl)[3];
    NvU32 PRIVATE_FIELD(ioctrlMask);
    NvU32 PRIVATE_FIELD(ipVerNvlink);
    NvU8 PRIVATE_FIELD(ioctrlNumEntries);
    NvU32 PRIVATE_FIELD(ioctrlSize);
    NvU32 PRIVATE_FIELD(registryControl);
    NvU32 PRIVATE_FIELD(minionControl);
    NvU32 PRIVATE_FIELD(verboseMask);
    NvU32 *PRIVATE_FIELD(pLinkConnection);
    NvBool PRIVATE_FIELD(bChiplibConfig);
    NvBool PRIVATE_FIELD(bRegistryLinkOverride);
    NvU64 PRIVATE_FIELD(registryLinkMask);
    NvBool PRIVATE_FIELD(bOverrideComputePeerMode);
    NvU64 PRIVATE_FIELD(discoveredLinks);
    NvU64 PRIVATE_FIELD(vbiosDisabledLinkMask);
    NvU32 PRIVATE_FIELD(regkeyDisabledLinksMask);
    NvU32 PRIVATE_FIELD(initDisabledLinksMask);
    NvU32 PRIVATE_FIELD(connectedLinksMask);
    NvU64 PRIVATE_FIELD(bridgeSensableLinks);
    NvU32 PRIVATE_FIELD(bridgedLinks);
    NvU64 PRIVATE_FIELD(enabledLinks);
    FaultUpList PRIVATE_FIELD(faultUpLinks);
    NvU64 PRIVATE_FIELD(initializedLinks);
    NvU32 PRIVATE_FIELD(maxSupportedLinks);
    KNVLINK_RM_LINK PRIVATE_FIELD(nvlinkLinks)[36];
    NvBool PRIVATE_FIELD(bIsGpuDegraded);
    NvU64 PRIVATE_FIELD(postRxDetLinkMask);
    NvU64 PRIVATE_FIELD(disconnectedLinkMask);
    NvU32 PRIVATE_FIELD(sysmemLinkMask);
    NvU64 PRIVATE_FIELD(peerLinkMasks)[32];
    NvU32 PRIVATE_FIELD(forcedSysmemDeviceType);
    nvlink_device *PRIVATE_FIELD(pNvlinkDev);
    NvU32 PRIVATE_FIELD(deviceLockRefcount);
    char *PRIVATE_FIELD(driverName);
    char *PRIVATE_FIELD(deviceName);
    NvU8 *PRIVATE_FIELD(pGidString);
    NvBool PRIVATE_FIELD(bVerifTrainingEnable);
    NvBool PRIVATE_FIELD(bL2Entry);
    NvBool PRIVATE_FIELD(bSkipLinkTraining);
    NvBool PRIVATE_FIELD(bForceAutoconfig);
    NvBool PRIVATE_FIELD(bForceEnableCoreLibRtlsims);
    NvBool PRIVATE_FIELD(bEnableTrainingAtLoad);
    NvBool PRIVATE_FIELD(bEnableSafeModeAtLoad);
    NvBool PRIVATE_FIELD(bEnableAli);
    NvBool PRIVATE_FIELD(bFloorSwept);
    NvU32 PRIVATE_FIELD(numPortEvents);
    NvBool PRIVATE_FIELD(bLinkTrainingDebugSpew);
    NvBool PRIVATE_FIELD(bDisableL2Mode);
    NvU32 PRIVATE_FIELD(nvlinkLinkSpeed);
    NvU32 PRIVATE_FIELD(errorRecoveries)[36];
    NvBool PRIVATE_FIELD(bNvswitchProxy);
    NvU64 PRIVATE_FIELD(fabricBaseAddr);
    NvU64 PRIVATE_FIELD(fabricEgmBaseAddr);
    NvBool PRIVATE_FIELD(bNvleModeRegkey);
};


struct KernelNvlink_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelNvlink *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelNvlink *__nvoc_pbase_KernelNvlink;    // knvlink

    // Vtable with 41 per-object function pointers
    NvBool (*__knvlinkIsPresent__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual halified (2 hals) override (engstate) base (engstate)
    NV_STATUS (*__knvlinkSetUniqueFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    void (*__knvlinkClearUniqueFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkSetUniqueFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (2 hals) body
    void (*__knvlinkClearUniqueFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkHandleFaultUpInterrupt__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkValidateFabricBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    NV_STATUS (*__knvlinkValidateFabricEgmBaseAddress__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU64);  // halified (3 hals) body
    NvU32 (*__knvlinkGetConnectedLinksMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEnableLinksPostTopology__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkOverrideConfig__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__knvlinkFilterBridgeLinks__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvU32 (*__knvlinkGetUniquePeerIdMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvU32 (*__knvlinkGetUniquePeerId__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkRemoveMapping__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvBool, NvU32, NvBool);  // halified (3 hals) body
    NV_STATUS (*__knvlinkGetP2POptimalCEs__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32 *, NvU32 *, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkConstructHal__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkSetupPeerMapping__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkProgramLinkSpeed__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkPoweredUpForD3__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkIsAliSupported__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkPostSetupNvlinkPeer__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkDiscoverPostRxDetLinks__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *);  // halified (2 hals) body
    NV_STATUS (*__knvlinkLogAliDebugMessages__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvBool);  // halified (3 hals) body
    void (*__knvlinkDumpCallbackRegister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkGetEffectivePeerLinkMask__)(struct OBJGPU *, struct KernelNvlink * /*this*/, struct OBJGPU *, NvU64 *);  // halified (2 hals) body
    NvU32 (*__knvlinkGetNumLinksToBeReducedPerIoctrl__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsBandwidthModeOff__)(struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsBwModeSupported__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU8);  // halified (3 hals) body
    NV_STATUS (*__knvlinkGetHshubSupportedRbmModes__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkPostSchedulingEnableCallbackRegister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    void (*__knvlinkPostSchedulingEnableCallbackUnregister__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkGetSupportedBwMode__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *);  // halified (3 hals) body
    void (*__knvlinkDirectConnectCheck__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsGpuReducedNvlinkConfig__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsFloorSweepingNeeded__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__knvlinkGetSupportedCounters__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *);  // halified (2 hals) body
    NvBool (*__knvlinkIsEncryptEnSet__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NvBool (*__knvlinkIsNvleEnabled__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEncryptionGetGpuIdentifiers__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__knvlinkEncryptionUpdateTopology__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32, NvU32);  // halified (2 hals) body

    // 19 PDB properties
    NvBool PDB_PROP_KNVLINK_ENABLED;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED;
    NvBool PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED;
    NvBool PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED;
    NvBool PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD;
    NvBool PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN;
    NvBool PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6;
    NvBool PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE;
    NvBool PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING;
    NvBool PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING;
    NvBool PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING;
    NvBool PDB_PROP_KNVLINK_MINION_GFW_BOOT;
    NvBool PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED;
    NvBool PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED;
    NvBool PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED;
    NvBool PDB_PROP_KNVLINK_ENCRYPTION_ENABLED;
    NvBool PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED;

    // Data members
    NvU8 nvlinkBwMode;
    NvU8 maxRbmLinks;
    NvU8 hshubSupportedRbmModesList[36];
    NvU8 totalRbmModes;
    NvU32 gspProxyRegkeys;
    NvU32 alid;
    NvU32 clid;
    NvBool bGotNvleIdentifiers;
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
    NvU64 registryLinkMask;
    NvBool bOverrideComputePeerMode;
    NvU64 discoveredLinks;
    NvU64 vbiosDisabledLinkMask;
    NvU32 regkeyDisabledLinksMask;
    NvU32 initDisabledLinksMask;
    NvU32 connectedLinksMask;
    NvU64 bridgeSensableLinks;
    NvU32 bridgedLinks;
    NvU64 enabledLinks;
    FaultUpList faultUpLinks;
    NvU64 initializedLinks;
    NvU32 maxSupportedLinks;
    KNVLINK_RM_LINK nvlinkLinks[36];
    NvBool bIsGpuDegraded;
    NvU64 postRxDetLinkMask;
    NvU64 disconnectedLinkMask;
    NvU32 sysmemLinkMask;
    NvU64 peerLinkMasks[32];
    NvU32 forcedSysmemDeviceType;
    nvlink_device *pNvlinkDev;
    NvU32 deviceLockRefcount;
    char *driverName;
    char *deviceName;
    NvU8 *pGidString;
    NvBool bVerifTrainingEnable;
    NvBool bL2Entry;
    NvBool bSkipLinkTraining;
    NvBool bForceAutoconfig;
    NvBool bForceEnableCoreLibRtlsims;
    NvBool bEnableTrainingAtLoad;
    NvBool bEnableSafeModeAtLoad;
    NvBool bEnableAli;
    NvBool bFloorSwept;
    NvU32 numPortEvents;
    NvBool bLinkTrainingDebugSpew;
    NvBool bDisableL2Mode;
    NvU32 nvlinkLinkSpeed;
    NvU32 errorRecoveries[36];
    NvBool bNvswitchProxy;
    NvU64 fabricBaseAddr;
    NvU64 fabricEgmBaseAddr;
    NvBool bNvleModeRegkey;
};


// Vtable with 13 per-class function pointers
struct NVOC_VTABLE__KernelNvlink {
    NV_STATUS (*__knvlinkConstructEngine__)(struct OBJGPU *, struct KernelNvlink * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePreInitLocked__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__knvlinkStateLoad__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePostLoad__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__knvlinkStateUnload__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePostUnload__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__knvlinkInitMissing__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePreInitUnlocked__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__knvlinkStateInitLocked__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__knvlinkStateInitUnlocked__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePreLoad__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__knvlinkStatePreUnload__)(struct OBJGPU *, struct KernelNvlink * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__knvlinkStateDestroy__)(struct OBJGPU *, struct KernelNvlink * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelNvlink {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelNvlink vtable;
};

#ifndef __NVOC_CLASS_KernelNvlink_TYPEDEF__
#define __NVOC_CLASS_KernelNvlink_TYPEDEF__
typedef struct KernelNvlink KernelNvlink;
#endif /* __NVOC_CLASS_KernelNvlink_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelNvlink
#define __nvoc_class_id_KernelNvlink 0xce6818
#endif /* __nvoc_class_id_KernelNvlink */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvlink;

#define __staticCast_KernelNvlink(pThis) \
    ((pThis)->__nvoc_pbase_KernelNvlink)

#ifdef __nvoc_kernel_nvlink_h_disabled
#define __dynamicCast_KernelNvlink(pThis) ((KernelNvlink*) NULL)
#else //__nvoc_kernel_nvlink_h_disabled
#define __dynamicCast_KernelNvlink(pThis) \
    ((KernelNvlink*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelNvlink)))
#endif //__nvoc_kernel_nvlink_h_disabled

// Property macros
#define PDB_PROP_KNVLINK_MINION_GFW_BOOT_BASE_CAST
#define PDB_PROP_KNVLINK_MINION_GFW_BOOT_BASE_NAME PDB_PROP_KNVLINK_MINION_GFW_BOOT
#define PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED_BASE_NAME PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED
#define PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK_BASE_CAST
#define PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK_BASE_NAME PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED_BASE_NAME PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED
#define PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED_BASE_NAME PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED
#define PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING_BASE_CAST
#define PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING_BASE_NAME PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING
#define PDB_PROP_KNVLINK_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_ENABLED_BASE_NAME PDB_PROP_KNVLINK_ENABLED
#define PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED_BASE_CAST
#define PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED_BASE_NAME PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED
#define PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING_BASE_CAST
#define PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING_BASE_NAME PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING
#define PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED_BASE_NAME PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED
#define PDB_PROP_KNVLINK_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KNVLINK_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING_BASE_CAST
#define PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING_BASE_NAME PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING
#define PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED_BASE_CAST
#define PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED_BASE_NAME PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED
#define PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6_BASE_CAST
#define PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6_BASE_NAME PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD_BASE_CAST
#define PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD_BASE_NAME PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD
#define PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED_BASE_NAME PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED
#define PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE_BASE_CAST
#define PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE_BASE_NAME PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE
#define PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN_BASE_CAST
#define PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN_BASE_NAME PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN
#define PDB_PROP_KNVLINK_ENCRYPTION_ENABLED_BASE_CAST
#define PDB_PROP_KNVLINK_ENCRYPTION_ENABLED_BASE_NAME PDB_PROP_KNVLINK_ENCRYPTION_ENABLED

NV_STATUS __nvoc_objCreateDynamic_KernelNvlink(KernelNvlink**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelNvlink(KernelNvlink**, Dynamic*, NvU32);
#define __objCreate_KernelNvlink(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelNvlink((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define knvlinkConstructEngine_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkConstructEngine__
#define knvlinkConstructEngine(arg1, arg_this, arg3) knvlinkConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define knvlinkStatePreInitLocked_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePreInitLocked__
#define knvlinkStatePreInitLocked(arg1, arg_this) knvlinkStatePreInitLocked_DISPATCH(arg1, arg_this)
#define knvlinkStateLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStateLoad__
#define knvlinkStateLoad(arg1, arg_this, arg3) knvlinkStateLoad_DISPATCH(arg1, arg_this, arg3)
#define knvlinkStatePostLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePostLoad__
#define knvlinkStatePostLoad(arg1, arg_this, arg3) knvlinkStatePostLoad_DISPATCH(arg1, arg_this, arg3)
#define knvlinkStateUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStateUnload__
#define knvlinkStateUnload(arg1, arg_this, arg3) knvlinkStateUnload_DISPATCH(arg1, arg_this, arg3)
#define knvlinkStatePostUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePostUnload__
#define knvlinkStatePostUnload(arg1, arg_this, arg3) knvlinkStatePostUnload_DISPATCH(arg1, arg_this, arg3)
#define knvlinkIsPresent_FNPTR(arg_this) arg_this->__knvlinkIsPresent__
#define knvlinkIsPresent(arg1, arg_this) knvlinkIsPresent_DISPATCH(arg1, arg_this)
#define knvlinkIsPresent_HAL(arg1, arg_this) knvlinkIsPresent_DISPATCH(arg1, arg_this)
#define knvlinkSetUniqueFabricBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkSetUniqueFabricBaseAddress__
#define knvlinkSetUniqueFabricBaseAddress(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkSetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkClearUniqueFabricBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkClearUniqueFabricBaseAddress__
#define knvlinkClearUniqueFabricBaseAddress(pGpu, pKernelNvlink) knvlinkClearUniqueFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkClearUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink) knvlinkClearUniqueFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkSetUniqueFabricEgmBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkSetUniqueFabricEgmBaseAddress__
#define knvlinkSetUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkSetUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkClearUniqueFabricEgmBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkClearUniqueFabricEgmBaseAddress__
#define knvlinkClearUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink) knvlinkClearUniqueFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkClearUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink) knvlinkClearUniqueFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkHandleFaultUpInterrupt_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkHandleFaultUpInterrupt__
#define knvlinkHandleFaultUpInterrupt(pGpu, pKernelNvlink, arg3) knvlinkHandleFaultUpInterrupt_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkHandleFaultUpInterrupt_HAL(pGpu, pKernelNvlink, arg3) knvlinkHandleFaultUpInterrupt_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkValidateFabricBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkValidateFabricBaseAddress__
#define knvlinkValidateFabricBaseAddress(pGpu, pKernelNvlink, arg3) knvlinkValidateFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkValidateFabricBaseAddress_HAL(pGpu, pKernelNvlink, arg3) knvlinkValidateFabricBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkValidateFabricEgmBaseAddress_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkValidateFabricEgmBaseAddress__
#define knvlinkValidateFabricEgmBaseAddress(pGpu, pKernelNvlink, arg3) knvlinkValidateFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkValidateFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink, arg3) knvlinkValidateFabricEgmBaseAddress_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkGetConnectedLinksMask_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetConnectedLinksMask__
#define knvlinkGetConnectedLinksMask(pGpu, pKernelNvlink) knvlinkGetConnectedLinksMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetConnectedLinksMask_HAL(pGpu, pKernelNvlink) knvlinkGetConnectedLinksMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkEnableLinksPostTopology_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkEnableLinksPostTopology__
#define knvlinkEnableLinksPostTopology(pGpu, pKernelNvlink, arg3) knvlinkEnableLinksPostTopology_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, arg3) knvlinkEnableLinksPostTopology_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkOverrideConfig_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkOverrideConfig__
#define knvlinkOverrideConfig(pGpu, pKernelNvlink, arg3) knvlinkOverrideConfig_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkOverrideConfig_HAL(pGpu, pKernelNvlink, arg3) knvlinkOverrideConfig_DISPATCH(pGpu, pKernelNvlink, arg3)
#define knvlinkFilterBridgeLinks_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkFilterBridgeLinks__
#define knvlinkFilterBridgeLinks(pGpu, pKernelNvlink) knvlinkFilterBridgeLinks_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink) knvlinkFilterBridgeLinks_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerIdMask_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetUniquePeerIdMask__
#define knvlinkGetUniquePeerIdMask(pGpu, pKernelNvlink) knvlinkGetUniquePeerIdMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerIdMask_HAL(pGpu, pKernelNvlink) knvlinkGetUniquePeerIdMask_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetUniquePeerId_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetUniquePeerId__
#define knvlinkGetUniquePeerId(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetUniquePeerId_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu)
#define knvlinkGetUniquePeerId_HAL(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetUniquePeerId_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu)
#define knvlinkRemoveMapping_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkRemoveMapping__
#define knvlinkRemoveMapping(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry) knvlinkRemoveMapping_DISPATCH(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry)
#define knvlinkRemoveMapping_HAL(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry) knvlinkRemoveMapping_DISPATCH(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry)
#define knvlinkGetP2POptimalCEs_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetP2POptimalCEs__
#define knvlinkGetP2POptimalCEs(pGpu, pKernelNvlink, arg3, arg4, arg5, arg6, arg7) knvlinkGetP2POptimalCEs_DISPATCH(pGpu, pKernelNvlink, arg3, arg4, arg5, arg6, arg7)
#define knvlinkGetP2POptimalCEs_HAL(pGpu, pKernelNvlink, arg3, arg4, arg5, arg6, arg7) knvlinkGetP2POptimalCEs_DISPATCH(pGpu, pKernelNvlink, arg3, arg4, arg5, arg6, arg7)
#define knvlinkConstructHal_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkConstructHal__
#define knvlinkConstructHal(pGpu, pKernelNvlink) knvlinkConstructHal_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkConstructHal_HAL(pGpu, pKernelNvlink) knvlinkConstructHal_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkSetupPeerMapping_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkSetupPeerMapping__
#define knvlinkSetupPeerMapping(pGpu, pKernelNvlink, pRemoteGpu, peerId) knvlinkSetupPeerMapping_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, peerId)
#define knvlinkSetupPeerMapping_HAL(pGpu, pKernelNvlink, pRemoteGpu, peerId) knvlinkSetupPeerMapping_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, peerId)
#define knvlinkProgramLinkSpeed_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkProgramLinkSpeed__
#define knvlinkProgramLinkSpeed(pGpu, pKernelNvlink) knvlinkProgramLinkSpeed_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkProgramLinkSpeed_HAL(pGpu, pKernelNvlink) knvlinkProgramLinkSpeed_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPoweredUpForD3_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkPoweredUpForD3__
#define knvlinkPoweredUpForD3(pGpu, pKernelNvlink) knvlinkPoweredUpForD3_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPoweredUpForD3_HAL(pGpu, pKernelNvlink) knvlinkPoweredUpForD3_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsAliSupported_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsAliSupported__
#define knvlinkIsAliSupported(pGpu, pKernelNvlink) knvlinkIsAliSupported_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsAliSupported_HAL(pGpu, pKernelNvlink) knvlinkIsAliSupported_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSetupNvlinkPeer_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkPostSetupNvlinkPeer__
#define knvlinkPostSetupNvlinkPeer(pGpu, pKernelNvlink) knvlinkPostSetupNvlinkPeer_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSetupNvlinkPeer_HAL(pGpu, pKernelNvlink) knvlinkPostSetupNvlinkPeer_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkDiscoverPostRxDetLinks_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkDiscoverPostRxDetLinks__
#define knvlinkDiscoverPostRxDetLinks(pGpu, pKernelNvlink, pPeerGpu) knvlinkDiscoverPostRxDetLinks_DISPATCH(pGpu, pKernelNvlink, pPeerGpu)
#define knvlinkDiscoverPostRxDetLinks_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkDiscoverPostRxDetLinks_DISPATCH(pGpu, pKernelNvlink, pPeerGpu)
#define knvlinkLogAliDebugMessages_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkLogAliDebugMessages__
#define knvlinkLogAliDebugMessages(pGpu, pKernelNvlink, bFinal) knvlinkLogAliDebugMessages_DISPATCH(pGpu, pKernelNvlink, bFinal)
#define knvlinkLogAliDebugMessages_HAL(pGpu, pKernelNvlink, bFinal) knvlinkLogAliDebugMessages_DISPATCH(pGpu, pKernelNvlink, bFinal)
#define knvlinkDumpCallbackRegister_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkDumpCallbackRegister__
#define knvlinkDumpCallbackRegister(pGpu, pKernelNvlink) knvlinkDumpCallbackRegister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkDumpCallbackRegister_HAL(pGpu, pKernelNvlink) knvlinkDumpCallbackRegister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetEffectivePeerLinkMask_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetEffectivePeerLinkMask__
#define knvlinkGetEffectivePeerLinkMask(pGpu, pKernelNvlink, pRemoteGpu, pPeerLinkMask) knvlinkGetEffectivePeerLinkMask_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, pPeerLinkMask)
#define knvlinkGetEffectivePeerLinkMask_HAL(pGpu, pKernelNvlink, pRemoteGpu, pPeerLinkMask) knvlinkGetEffectivePeerLinkMask_DISPATCH(pGpu, pKernelNvlink, pRemoteGpu, pPeerLinkMask)
#define knvlinkGetNumLinksToBeReducedPerIoctrl_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetNumLinksToBeReducedPerIoctrl__
#define knvlinkGetNumLinksToBeReducedPerIoctrl(pGpu, pKernelNvlink) knvlinkGetNumLinksToBeReducedPerIoctrl_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetNumLinksToBeReducedPerIoctrl_HAL(pGpu, pKernelNvlink) knvlinkGetNumLinksToBeReducedPerIoctrl_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsBandwidthModeOff_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsBandwidthModeOff__
#define knvlinkIsBandwidthModeOff(pKernelNvlink) knvlinkIsBandwidthModeOff_DISPATCH(pKernelNvlink)
#define knvlinkIsBandwidthModeOff_HAL(pKernelNvlink) knvlinkIsBandwidthModeOff_DISPATCH(pKernelNvlink)
#define knvlinkIsBwModeSupported_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsBwModeSupported__
#define knvlinkIsBwModeSupported(pGpu, pKernelNvlink, mode) knvlinkIsBwModeSupported_DISPATCH(pGpu, pKernelNvlink, mode)
#define knvlinkIsBwModeSupported_HAL(pGpu, pKernelNvlink, mode) knvlinkIsBwModeSupported_DISPATCH(pGpu, pKernelNvlink, mode)
#define knvlinkGetHshubSupportedRbmModes_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetHshubSupportedRbmModes__
#define knvlinkGetHshubSupportedRbmModes(pGpu, pKernelNvlink) knvlinkGetHshubSupportedRbmModes_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetHshubSupportedRbmModes_HAL(pGpu, pKernelNvlink) knvlinkGetHshubSupportedRbmModes_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSchedulingEnableCallbackRegister_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkPostSchedulingEnableCallbackRegister__
#define knvlinkPostSchedulingEnableCallbackRegister(pGpu, pKernelNvlink) knvlinkPostSchedulingEnableCallbackRegister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSchedulingEnableCallbackRegister_HAL(pGpu, pKernelNvlink) knvlinkPostSchedulingEnableCallbackRegister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSchedulingEnableCallbackUnregister_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkPostSchedulingEnableCallbackUnregister__
#define knvlinkPostSchedulingEnableCallbackUnregister(pGpu, pKernelNvlink) knvlinkPostSchedulingEnableCallbackUnregister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkPostSchedulingEnableCallbackUnregister_HAL(pGpu, pKernelNvlink) knvlinkPostSchedulingEnableCallbackUnregister_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkGetSupportedBwMode_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetSupportedBwMode__
#define knvlinkGetSupportedBwMode(pGpu, pKernelNvlink, pParams) knvlinkGetSupportedBwMode_DISPATCH(pGpu, pKernelNvlink, pParams)
#define knvlinkGetSupportedBwMode_HAL(pGpu, pKernelNvlink, pParams) knvlinkGetSupportedBwMode_DISPATCH(pGpu, pKernelNvlink, pParams)
#define knvlinkDirectConnectCheck_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkDirectConnectCheck__
#define knvlinkDirectConnectCheck(pGpu, pKernelNvlink) knvlinkDirectConnectCheck_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkDirectConnectCheck_HAL(pGpu, pKernelNvlink) knvlinkDirectConnectCheck_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsGpuReducedNvlinkConfig_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsGpuReducedNvlinkConfig__
#define knvlinkIsGpuReducedNvlinkConfig(pGpu, pKernelNvlink) knvlinkIsGpuReducedNvlinkConfig_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsGpuReducedNvlinkConfig_HAL(pGpu, pKernelNvlink) knvlinkIsGpuReducedNvlinkConfig_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsFloorSweepingNeeded_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsFloorSweepingNeeded__
#define knvlinkIsFloorSweepingNeeded(pGpu, pKernelNvlink, numActiveLinksPerIoctrl, numLinksPerIoctrl) knvlinkIsFloorSweepingNeeded_DISPATCH(pGpu, pKernelNvlink, numActiveLinksPerIoctrl, numLinksPerIoctrl)
#define knvlinkIsFloorSweepingNeeded_HAL(pGpu, pKernelNvlink, numActiveLinksPerIoctrl, numLinksPerIoctrl) knvlinkIsFloorSweepingNeeded_DISPATCH(pGpu, pKernelNvlink, numActiveLinksPerIoctrl, numLinksPerIoctrl)
#define knvlinkGetSupportedCounters_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkGetSupportedCounters__
#define knvlinkGetSupportedCounters(pGpu, pKernelNvlink, pParams) knvlinkGetSupportedCounters_DISPATCH(pGpu, pKernelNvlink, pParams)
#define knvlinkGetSupportedCounters_HAL(pGpu, pKernelNvlink, pParams) knvlinkGetSupportedCounters_DISPATCH(pGpu, pKernelNvlink, pParams)
#define knvlinkIsEncryptEnSet_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsEncryptEnSet__
#define knvlinkIsEncryptEnSet(pGpu, pKernelNvlink) knvlinkIsEncryptEnSet_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsEncryptEnSet_HAL(pGpu, pKernelNvlink) knvlinkIsEncryptEnSet_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsNvleEnabled_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkIsNvleEnabled__
#define knvlinkIsNvleEnabled(pGpu, pKernelNvlink) knvlinkIsNvleEnabled_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkIsNvleEnabled_HAL(pGpu, pKernelNvlink) knvlinkIsNvleEnabled_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkEncryptionGetGpuIdentifiers_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkEncryptionGetGpuIdentifiers__
#define knvlinkEncryptionGetGpuIdentifiers(pGpu, pKernelNvlink) knvlinkEncryptionGetGpuIdentifiers_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkEncryptionGetGpuIdentifiers_HAL(pGpu, pKernelNvlink) knvlinkEncryptionGetGpuIdentifiers_DISPATCH(pGpu, pKernelNvlink)
#define knvlinkEncryptionUpdateTopology_FNPTR(pKernelNvlink) pKernelNvlink->__knvlinkEncryptionUpdateTopology__
#define knvlinkEncryptionUpdateTopology(pGpu, pKernelNvlink, remoteGpuAlid, remoteGpuClid) knvlinkEncryptionUpdateTopology_DISPATCH(pGpu, pKernelNvlink, remoteGpuAlid, remoteGpuClid)
#define knvlinkEncryptionUpdateTopology_HAL(pGpu, pKernelNvlink, remoteGpuAlid, remoteGpuClid) knvlinkEncryptionUpdateTopology_DISPATCH(pGpu, pKernelNvlink, remoteGpuAlid, remoteGpuClid)
#define knvlinkInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define knvlinkInitMissing(pGpu, pEngstate) knvlinkInitMissing_DISPATCH(pGpu, pEngstate)
#define knvlinkStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define knvlinkStatePreInitUnlocked(pGpu, pEngstate) knvlinkStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvlinkStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define knvlinkStateInitLocked(pGpu, pEngstate) knvlinkStateInitLocked_DISPATCH(pGpu, pEngstate)
#define knvlinkStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define knvlinkStateInitUnlocked(pGpu, pEngstate) knvlinkStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvlinkStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define knvlinkStatePreLoad(pGpu, pEngstate, arg3) knvlinkStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define knvlinkStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define knvlinkStatePreUnload(pGpu, pEngstate, arg3) knvlinkStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define knvlinkStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define knvlinkStateDestroy(pGpu, pEngstate) knvlinkStateDestroy_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS knvlinkConstructEngine_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this, ENGDESCRIPTOR arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkConstructEngine__(arg1, arg_this, arg3);
}

static inline NV_STATUS knvlinkStatePreInitLocked_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePreInitLocked__(arg1, arg_this);
}

static inline NV_STATUS knvlinkStateLoad_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStateLoad__(arg1, arg_this, arg3);
}

static inline NV_STATUS knvlinkStatePostLoad_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePostLoad__(arg1, arg_this, arg3);
}

static inline NV_STATUS knvlinkStateUnload_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStateUnload__(arg1, arg_this, arg3);
}

static inline NV_STATUS knvlinkStatePostUnload_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__knvlinkStatePostUnload__(arg1, arg_this, arg3);
}

static inline NvBool knvlinkIsPresent_DISPATCH(struct OBJGPU *arg1, struct KernelNvlink *arg_this) {
    return arg_this->__knvlinkIsPresent__(arg1, arg_this);
}

static inline NV_STATUS knvlinkSetUniqueFabricBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return pKernelNvlink->__knvlinkSetUniqueFabricBaseAddress__(pGpu, pKernelNvlink, arg3);
}

static inline void knvlinkClearUniqueFabricBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkClearUniqueFabricBaseAddress__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkSetUniqueFabricEgmBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return pKernelNvlink->__knvlinkSetUniqueFabricEgmBaseAddress__(pGpu, pKernelNvlink, arg3);
}

static inline void knvlinkClearUniqueFabricEgmBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkClearUniqueFabricEgmBaseAddress__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkHandleFaultUpInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return pKernelNvlink->__knvlinkHandleFaultUpInterrupt__(pGpu, pKernelNvlink, arg3);
}

static inline NV_STATUS knvlinkValidateFabricBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return pKernelNvlink->__knvlinkValidateFabricBaseAddress__(pGpu, pKernelNvlink, arg3);
}

static inline NV_STATUS knvlinkValidateFabricEgmBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return pKernelNvlink->__knvlinkValidateFabricEgmBaseAddress__(pGpu, pKernelNvlink, arg3);
}

static inline NvU32 knvlinkGetConnectedLinksMask_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetConnectedLinksMask__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkEnableLinksPostTopology_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return pKernelNvlink->__knvlinkEnableLinksPostTopology__(pGpu, pKernelNvlink, arg3);
}

static inline NV_STATUS knvlinkOverrideConfig_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return pKernelNvlink->__knvlinkOverrideConfig__(pGpu, pKernelNvlink, arg3);
}

static inline NV_STATUS knvlinkFilterBridgeLinks_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkFilterBridgeLinks__(pGpu, pKernelNvlink);
}

static inline NvU32 knvlinkGetUniquePeerIdMask_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetUniquePeerIdMask__(pGpu, pKernelNvlink);
}

static inline NvU32 knvlinkGetUniquePeerId_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu) {
    return pKernelNvlink->__knvlinkGetUniquePeerId__(pGpu, pKernelNvlink, pRemoteGpu);
}

static inline NV_STATUS knvlinkRemoveMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry) {
    return pKernelNvlink->__knvlinkRemoveMapping__(pGpu, pKernelNvlink, bAllMapping, peerMask, bL2Entry);
}

static inline NV_STATUS knvlinkGetP2POptimalCEs_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 *arg7) {
    return pKernelNvlink->__knvlinkGetP2POptimalCEs__(pGpu, pKernelNvlink, arg3, arg4, arg5, arg6, arg7);
}

static inline NV_STATUS knvlinkConstructHal_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkConstructHal__(pGpu, pKernelNvlink);
}

static inline void knvlinkSetupPeerMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU32 peerId) {
    pKernelNvlink->__knvlinkSetupPeerMapping__(pGpu, pKernelNvlink, pRemoteGpu, peerId);
}

static inline NV_STATUS knvlinkProgramLinkSpeed_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkProgramLinkSpeed__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkPoweredUpForD3_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkPoweredUpForD3__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkIsAliSupported_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkIsAliSupported__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkPostSetupNvlinkPeer_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkPostSetupNvlinkPeer__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkDiscoverPostRxDetLinks_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu) {
    return pKernelNvlink->__knvlinkDiscoverPostRxDetLinks__(pGpu, pKernelNvlink, pPeerGpu);
}

static inline NV_STATUS knvlinkLogAliDebugMessages_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bFinal) {
    return pKernelNvlink->__knvlinkLogAliDebugMessages__(pGpu, pKernelNvlink, bFinal);
}

static inline void knvlinkDumpCallbackRegister_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkDumpCallbackRegister__(pGpu, pKernelNvlink);
}

static inline void knvlinkGetEffectivePeerLinkMask_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU64 *pPeerLinkMask) {
    pKernelNvlink->__knvlinkGetEffectivePeerLinkMask__(pGpu, pKernelNvlink, pRemoteGpu, pPeerLinkMask);
}

static inline NvU32 knvlinkGetNumLinksToBeReducedPerIoctrl_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetNumLinksToBeReducedPerIoctrl__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkIsBandwidthModeOff_DISPATCH(struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkIsBandwidthModeOff__(pKernelNvlink);
}

static inline NvBool knvlinkIsBwModeSupported_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU8 mode) {
    return pKernelNvlink->__knvlinkIsBwModeSupported__(pGpu, pKernelNvlink, mode);
}

static inline NV_STATUS knvlinkGetHshubSupportedRbmModes_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkGetHshubSupportedRbmModes__(pGpu, pKernelNvlink);
}

static inline void knvlinkPostSchedulingEnableCallbackRegister_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkPostSchedulingEnableCallbackRegister__(pGpu, pKernelNvlink);
}

static inline void knvlinkPostSchedulingEnableCallbackUnregister_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkPostSchedulingEnableCallbackUnregister__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkGetSupportedBwMode_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams) {
    return pKernelNvlink->__knvlinkGetSupportedBwMode__(pGpu, pKernelNvlink, pParams);
}

static inline void knvlinkDirectConnectCheck_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    pKernelNvlink->__knvlinkDirectConnectCheck__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkIsGpuReducedNvlinkConfig_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkIsGpuReducedNvlinkConfig__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkIsFloorSweepingNeeded_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 numActiveLinksPerIoctrl, NvU32 numLinksPerIoctrl) {
    return pKernelNvlink->__knvlinkIsFloorSweepingNeeded__(pGpu, pKernelNvlink, numActiveLinksPerIoctrl, numLinksPerIoctrl);
}

static inline NV_STATUS knvlinkGetSupportedCounters_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *pParams) {
    return pKernelNvlink->__knvlinkGetSupportedCounters__(pGpu, pKernelNvlink, pParams);
}

static inline NvBool knvlinkIsEncryptEnSet_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkIsEncryptEnSet__(pGpu, pKernelNvlink);
}

static inline NvBool knvlinkIsNvleEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkIsNvleEnabled__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkEncryptionGetGpuIdentifiers_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return pKernelNvlink->__knvlinkEncryptionGetGpuIdentifiers__(pGpu, pKernelNvlink);
}

static inline NV_STATUS knvlinkEncryptionUpdateTopology_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 remoteGpuAlid, NvU32 remoteGpuClid) {
    return pKernelNvlink->__knvlinkEncryptionUpdateTopology__(pGpu, pKernelNvlink, remoteGpuAlid, remoteGpuClid);
}

static inline void knvlinkInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvlinkStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS knvlinkStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline void knvlinkStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__knvlinkStateDestroy__(pGpu, pEngstate);
}

NvBool knvlinkIsForcedConfig_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsForcedConfig(struct OBJGPU *arg1, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsForcedConfig(arg1, arg2) knvlinkIsForcedConfig_IMPL(arg1, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsForcedConfig_HAL(arg1, arg2) knvlinkIsForcedConfig(arg1, arg2)

NV_STATUS knvlinkApplyRegkeyOverrides_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkApplyRegkeyOverrides(struct OBJGPU *pGpu, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkApplyRegkeyOverrides(pGpu, arg2) knvlinkApplyRegkeyOverrides_IMPL(pGpu, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkApplyRegkeyOverrides_HAL(pGpu, arg2) knvlinkApplyRegkeyOverrides(pGpu, arg2)

NvBool knvlinkIsNvlinkDefaultEnabled_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvlinkDefaultEnabled(struct OBJGPU *pGpu, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvlinkDefaultEnabled(pGpu, arg2) knvlinkIsNvlinkDefaultEnabled_IMPL(pGpu, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvlinkDefaultEnabled_HAL(pGpu, arg2) knvlinkIsNvlinkDefaultEnabled(pGpu, arg2)

NvBool knvlinkIsP2pLoopbackSupported_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsP2pLoopbackSupported(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsP2pLoopbackSupported(pGpu, pKernelNvlink) knvlinkIsP2pLoopbackSupported_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsP2pLoopbackSupported_HAL(pGpu, pKernelNvlink) knvlinkIsP2pLoopbackSupported(pGpu, pKernelNvlink)

NvBool knvlinkIsP2pLoopbackSupportedPerLink_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsP2pLoopbackSupportedPerLink(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, arg3) knvlinkIsP2pLoopbackSupportedPerLink_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsP2pLoopbackSupportedPerLink_HAL(pGpu, pKernelNvlink, arg3) knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, arg3)

NvBool knvlinkIsNvlinkP2pSupported_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvlinkP2pSupported(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvlinkP2pSupported(pGpu, pKernelNvlink, pPeerGpu) knvlinkIsNvlinkP2pSupported_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvlinkP2pSupported_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkIsNvlinkP2pSupported(pGpu, pKernelNvlink, pPeerGpu)

NvBool knvlinkCheckNvswitchP2pConfig_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkCheckNvswitchP2pConfig(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCheckNvswitchP2pConfig(pGpu, pKernelNvlink, pPeerGpu) knvlinkCheckNvswitchP2pConfig_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCheckNvswitchP2pConfig_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkCheckNvswitchP2pConfig(pGpu, pKernelNvlink, pPeerGpu)

NV_STATUS knvlinkGetP2pConnectionStatus_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkGetP2pConnectionStatus(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu) knvlinkGetP2pConnectionStatus_IMPL(pGpu, pKernelNvlink, pPeerGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetP2pConnectionStatus_HAL(pGpu, pKernelNvlink, pPeerGpu) knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu)

NV_STATUS knvlinkUpdateCurrentConfig_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdateCurrentConfig(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink) knvlinkUpdateCurrentConfig_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdateCurrentConfig_HAL(pGpu, pKernelNvlink) knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink)

void knvlinkCoreDriverLoadWar_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkCoreDriverLoadWar(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreDriverLoadWar(pGpu, pKernelNvlink) knvlinkCoreDriverLoadWar_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreDriverLoadWar_HAL(pGpu, pKernelNvlink) knvlinkCoreDriverLoadWar(pGpu, pKernelNvlink)

void knvlinkCoreDriverUnloadWar_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkCoreDriverUnloadWar(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreDriverUnloadWar(pGpu, pKernelNvlink) knvlinkCoreDriverUnloadWar_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreDriverUnloadWar_HAL(pGpu, pKernelNvlink) knvlinkCoreDriverUnloadWar(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreIsDriverSupported_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreIsDriverSupported(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreIsDriverSupported(pGpu, pKernelNvlink) knvlinkCoreIsDriverSupported_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreIsDriverSupported_HAL(pGpu, pKernelNvlink) knvlinkCoreIsDriverSupported(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreAddDevice_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreAddDevice(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreAddDevice(pGpu, pKernelNvlink) knvlinkCoreAddDevice_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreAddDevice_HAL(pGpu, pKernelNvlink) knvlinkCoreAddDevice(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreAddLink_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreAddLink(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreAddLink(pGpu, pKernelNvlink, arg3) knvlinkCoreAddLink_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreAddLink_HAL(pGpu, pKernelNvlink, arg3) knvlinkCoreAddLink(pGpu, pKernelNvlink, arg3)

NV_STATUS knvlinkCoreRemoveDevice_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreRemoveDevice(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreRemoveDevice(pGpu, pKernelNvlink) knvlinkCoreRemoveDevice_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreRemoveDevice_HAL(pGpu, pKernelNvlink) knvlinkCoreRemoveDevice(pGpu, pKernelNvlink)

void knvlinkSetDegradedMode_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvS32 linkId);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkSetDegradedMode(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvS32 linkId) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetDegradedMode(pGpu, pKernelNvlink, linkId) knvlinkSetDegradedMode_IMPL(pGpu, pKernelNvlink, linkId)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetDegradedMode_HAL(pGpu, pKernelNvlink, linkId) knvlinkSetDegradedMode(pGpu, pKernelNvlink, linkId)

NvBool knvlinkGetDegradedMode_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkGetDegradedMode(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetDegradedMode(pGpu, pKernelNvlink) knvlinkGetDegradedMode_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetDegradedMode_HAL(pGpu, pKernelNvlink) knvlinkGetDegradedMode(pGpu, pKernelNvlink)

NvBool knvlinkIsUncontainedErrorRecoveryActive_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsUncontainedErrorRecoveryActive(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsUncontainedErrorRecoveryActive(pGpu, pKernelNvlink) knvlinkIsUncontainedErrorRecoveryActive_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsUncontainedErrorRecoveryActive_HAL(pGpu, pKernelNvlink) knvlinkIsUncontainedErrorRecoveryActive(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreRemoveLink_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreRemoveLink(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreRemoveLink(pGpu, pKernelNvlink, arg3) knvlinkCoreRemoveLink_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreRemoveLink_HAL(pGpu, pKernelNvlink, arg3) knvlinkCoreRemoveLink(pGpu, pKernelNvlink, arg3)

NV_STATUS knvlinkCoreShutdownDeviceLinks_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bForcePowerDown);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreShutdownDeviceLinks(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bForcePowerDown) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink, bForcePowerDown) knvlinkCoreShutdownDeviceLinks_IMPL(pGpu, pKernelNvlink, bForcePowerDown)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreShutdownDeviceLinks_HAL(pGpu, pKernelNvlink, bForcePowerDown) knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink, bForcePowerDown)

NV_STATUS knvlinkCoreResetDeviceLinks_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreResetDeviceLinks(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreResetDeviceLinks(pGpu, pKernelNvlink) knvlinkCoreResetDeviceLinks_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreResetDeviceLinks_HAL(pGpu, pKernelNvlink) knvlinkCoreResetDeviceLinks(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreUpdateDeviceUUID_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreUpdateDeviceUUID(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreUpdateDeviceUUID(pGpu, pKernelNvlink) knvlinkCoreUpdateDeviceUUID_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreUpdateDeviceUUID_HAL(pGpu, pKernelNvlink) knvlinkCoreUpdateDeviceUUID(pGpu, pKernelNvlink)

NV_STATUS knvlinkCoreGetRemoteDeviceInfo_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCoreGetRemoteDeviceInfo(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink) knvlinkCoreGetRemoteDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreGetRemoteDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink)

NvBool knvlinkIsGpuConnectedToNvswitch_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsGpuConnectedToNvswitch(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink) knvlinkIsGpuConnectedToNvswitch_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsGpuConnectedToNvswitch_HAL(pGpu, pKernelNvlink) knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink)

NvBool knvlinkIsLinkConnected_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsLinkConnected(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsLinkConnected(pGpu, pKernelNvlink, arg3) knvlinkIsLinkConnected_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsLinkConnected_HAL(pGpu, pKernelNvlink, arg3) knvlinkIsLinkConnected(pGpu, pKernelNvlink, arg3)

NV_STATUS knvlinkTrainSysmemLinksToActive_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainSysmemLinksToActive(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainSysmemLinksToActive(pGpu, pKernelNvlink) knvlinkTrainSysmemLinksToActive_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainSysmemLinksToActive_HAL(pGpu, pKernelNvlink) knvlinkTrainSysmemLinksToActive(pGpu, pKernelNvlink)

NV_STATUS knvlinkTrainP2pLinksToActive_IMPL(struct OBJGPU *pGpu0, struct OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainP2pLinksToActive(struct OBJGPU *pGpu0, struct OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainP2pLinksToActive(pGpu0, pGpu1, pKernelNvlink) knvlinkTrainP2pLinksToActive_IMPL(pGpu0, pGpu1, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainP2pLinksToActive_HAL(pGpu0, pGpu1, pKernelNvlink) knvlinkTrainP2pLinksToActive(pGpu0, pGpu1, pKernelNvlink)

NV_STATUS knvlinkCheckTrainingIsComplete_IMPL(struct OBJGPU *pGpu0, struct OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCheckTrainingIsComplete(struct OBJGPU *pGpu0, struct OBJGPU *pGpu1, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCheckTrainingIsComplete(pGpu0, pGpu1, pKernelNvlink) knvlinkCheckTrainingIsComplete_IMPL(pGpu0, pGpu1, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCheckTrainingIsComplete_HAL(pGpu0, pGpu1, pKernelNvlink) knvlinkCheckTrainingIsComplete(pGpu0, pGpu1, pKernelNvlink)

NV_STATUS knvlinkTrainFabricLinksToActive_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainFabricLinksToActive(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainFabricLinksToActive(pGpu, pKernelNvlink) knvlinkTrainFabricLinksToActive_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainFabricLinksToActive_HAL(pGpu, pKernelNvlink) knvlinkTrainFabricLinksToActive(pGpu, pKernelNvlink)

NV_STATUS knvlinkRetrainLink_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId, NvBool bFromOff);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkRetrainLink(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId, NvBool bFromOff) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkRetrainLink(pGpu, pKernelNvlink, linkId, bFromOff) knvlinkRetrainLink_IMPL(pGpu, pKernelNvlink, linkId, bFromOff)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkRetrainLink_HAL(pGpu, pKernelNvlink, linkId, bFromOff) knvlinkRetrainLink(pGpu, pKernelNvlink, linkId, bFromOff)

NvU64 knvlinkGetEnabledLinkMask_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetEnabledLinkMask(struct OBJGPU *pGpu, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetEnabledLinkMask(pGpu, arg2) knvlinkGetEnabledLinkMask_IMPL(pGpu, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetEnabledLinkMask_HAL(pGpu, arg2) knvlinkGetEnabledLinkMask(pGpu, arg2)

NvU64 knvlinkGetDiscoveredLinkMask_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetDiscoveredLinkMask(struct OBJGPU *pGpu, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetDiscoveredLinkMask(pGpu, arg2) knvlinkGetDiscoveredLinkMask_IMPL(pGpu, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetDiscoveredLinkMask_HAL(pGpu, arg2) knvlinkGetDiscoveredLinkMask(pGpu, arg2)

NV_STATUS knvlinkProcessInitDisabledLinks_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkProcessInitDisabledLinks(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkProcessInitDisabledLinks(pGpu, pKernelNvlink) knvlinkProcessInitDisabledLinks_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkProcessInitDisabledLinks_HAL(pGpu, pKernelNvlink) knvlinkProcessInitDisabledLinks(pGpu, pKernelNvlink)

NvU32 knvlinkGetNumLinksToSystem_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetNumLinksToSystem(struct OBJGPU *arg1, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetNumLinksToSystem(arg1, arg2) knvlinkGetNumLinksToSystem_IMPL(arg1, arg2)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetNumLinksToSystem_HAL(arg1, arg2) knvlinkGetNumLinksToSystem(arg1, arg2)

NvU32 knvlinkGetNumLinksToPeer_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetNumLinksToPeer(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetNumLinksToPeer_IMPL(pGpu, pKernelNvlink, pRemoteGpu)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetNumLinksToPeer_HAL(pGpu, pKernelNvlink, pRemoteGpu) knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu)

NvU64 knvlinkGetLinkMaskToPeer_IMPL(struct OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, struct OBJGPU *pGpu1);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetLinkMaskToPeer(struct OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, struct OBJGPU *pGpu1) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1) knvlinkGetLinkMaskToPeer_IMPL(pGpu0, pKernelNvlink0, pGpu1)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetLinkMaskToPeer_HAL(pGpu0, pKernelNvlink0, pGpu1) knvlinkGetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1)

NV_STATUS knvlinkSetLinkMaskToPeer_IMPL(struct OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, struct OBJGPU *pGpu1, NvU64 peerLinkMask);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetLinkMaskToPeer(struct OBJGPU *pGpu0, struct KernelNvlink *pKernelNvlink0, struct OBJGPU *pGpu1, NvU64 peerLinkMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask) knvlinkSetLinkMaskToPeer_IMPL(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetLinkMaskToPeer_HAL(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask) knvlinkSetLinkMaskToPeer(pGpu0, pKernelNvlink0, pGpu1, peerLinkMask)

NvU32 knvlinkGetPeersNvlinkMaskFromHshub_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetPeersNvlinkMaskFromHshub(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetPeersNvlinkMaskFromHshub(pGpu, pKernelNvlink) knvlinkGetPeersNvlinkMaskFromHshub_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetPeersNvlinkMaskFromHshub_HAL(pGpu, pKernelNvlink) knvlinkGetPeersNvlinkMaskFromHshub(pGpu, pKernelNvlink)

NV_STATUS knvlinkPrepareForXVEReset_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bForcePowerDown);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkPrepareForXVEReset(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bForcePowerDown) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkPrepareForXVEReset(pGpu, pKernelNvlink, bForcePowerDown) knvlinkPrepareForXVEReset_IMPL(pGpu, pKernelNvlink, bForcePowerDown)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkPrepareForXVEReset_HAL(pGpu, pKernelNvlink, bForcePowerDown) knvlinkPrepareForXVEReset(pGpu, pKernelNvlink, bForcePowerDown)

NV_STATUS knvlinkEnterExitSleep_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkEnterExitSleep(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkEnterExitSleep(pGpu, pKernelNvlink, arg3, arg4) knvlinkEnterExitSleep_IMPL(pGpu, pKernelNvlink, arg3, arg4)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkEnterExitSleep_HAL(pGpu, pKernelNvlink, arg3, arg4) knvlinkEnterExitSleep(pGpu, pKernelNvlink, arg3, arg4)

NV_STATUS knvlinkSyncLinkMasksAndVbiosInfo_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSyncLinkMasksAndVbiosInfo(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink) knvlinkSyncLinkMasksAndVbiosInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSyncLinkMasksAndVbiosInfo_HAL(pGpu, pKernelNvlink) knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkInbandMsgCallbackDispatcher_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvLink, NvU32 dataSize, NvU8 *pMessage);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkInbandMsgCallbackDispatcher(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvLink, NvU32 dataSize, NvU8 *pMessage) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvLink, dataSize, pMessage) knvlinkInbandMsgCallbackDispatcher_IMPL(pGpu, pKernelNvLink, dataSize, pMessage)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkInbandMsgCallbackDispatcher_HAL(pGpu, pKernelNvLink, dataSize, pMessage) knvlinkInbandMsgCallbackDispatcher(pGpu, pKernelNvLink, dataSize, pMessage)

NV_STATUS knvlinkFatalErrorRecovery_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvLink, NvBool bRecoverable, NvBool bLazy);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkFatalErrorRecovery(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvLink, NvBool bRecoverable, NvBool bLazy) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkFatalErrorRecovery(pGpu, pKernelNvLink, bRecoverable, bLazy) knvlinkFatalErrorRecovery_IMPL(pGpu, pKernelNvLink, bRecoverable, bLazy)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkFatalErrorRecovery_HAL(pGpu, pKernelNvLink, bRecoverable, bLazy) knvlinkFatalErrorRecovery(pGpu, pKernelNvLink, bRecoverable, bLazy)

NV_STATUS knvlinkSendInbandData_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSendInbandData(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSendInbandData(pGpu, pKernelNvlink, pParams) knvlinkSendInbandData_IMPL(pGpu, pKernelNvlink, pParams)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSendInbandData_HAL(pGpu, pKernelNvlink, pParams) knvlinkSendInbandData(pGpu, pKernelNvlink, pParams)

NV_STATUS knvlinkUpdateLinkConnectionStatus_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdateLinkConnectionStatus(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, arg3) knvlinkUpdateLinkConnectionStatus_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdateLinkConnectionStatus_HAL(pGpu, pKernelNvlink, arg3) knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, arg3)

NV_STATUS knvlinkPreTrainLinksToActiveAli_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkPreTrainLinksToActiveAli(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkPreTrainLinksToActiveAli(pGpu, pKernelNvlink, arg3, arg4) knvlinkPreTrainLinksToActiveAli_IMPL(pGpu, pKernelNvlink, arg3, arg4)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkPreTrainLinksToActiveAli_HAL(pGpu, pKernelNvlink, arg3, arg4) knvlinkPreTrainLinksToActiveAli(pGpu, pKernelNvlink, arg3, arg4)

NV_STATUS knvlinkTrainLinksToActiveAli_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkTrainLinksToActiveAli(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkTrainLinksToActiveAli(pGpu, pKernelNvlink, arg3, arg4) knvlinkTrainLinksToActiveAli_IMPL(pGpu, pKernelNvlink, arg3, arg4)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkTrainLinksToActiveAli_HAL(pGpu, pKernelNvlink, arg3, arg4) knvlinkTrainLinksToActiveAli(pGpu, pKernelNvlink, arg3, arg4)

NV_STATUS knvlinkUpdatePostRxDetectLinkMask_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkUpdatePostRxDetectLinkMask(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkUpdatePostRxDetectLinkMask(pGpu, pKernelNvlink) knvlinkUpdatePostRxDetectLinkMask_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkUpdatePostRxDetectLinkMask_HAL(pGpu, pKernelNvlink) knvlinkUpdatePostRxDetectLinkMask(pGpu, pKernelNvlink)

NV_STATUS knvlinkCopyNvlinkDeviceInfo_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCopyNvlinkDeviceInfo(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink) knvlinkCopyNvlinkDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCopyNvlinkDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCopyNvlinkDeviceInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkCopyIoctrlDeviceInfo_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkCopyIoctrlDeviceInfo(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCopyIoctrlDeviceInfo(pGpu, pKernelNvlink) knvlinkCopyIoctrlDeviceInfo_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCopyIoctrlDeviceInfo_HAL(pGpu, pKernelNvlink) knvlinkCopyIoctrlDeviceInfo(pGpu, pKernelNvlink)

NV_STATUS knvlinkSetupTopologyForForcedConfig_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetupTopologyForForcedConfig(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetupTopologyForForcedConfig(pGpu, pKernelNvlink) knvlinkSetupTopologyForForcedConfig_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetupTopologyForForcedConfig_HAL(pGpu, pKernelNvlink) knvlinkSetupTopologyForForcedConfig(pGpu, pKernelNvlink)

NV_STATUS knvlinkSyncLaneShutdownProps_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSyncLaneShutdownProps(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSyncLaneShutdownProps(pGpu, pKernelNvlink) knvlinkSyncLaneShutdownProps_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSyncLaneShutdownProps_HAL(pGpu, pKernelNvlink) knvlinkSyncLaneShutdownProps(pGpu, pKernelNvlink)

void knvlinkSetPowerFeatures_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkSetPowerFeatures(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetPowerFeatures(pGpu, pKernelNvlink) knvlinkSetPowerFeatures_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetPowerFeatures_HAL(pGpu, pKernelNvlink) knvlinkSetPowerFeatures(pGpu, pKernelNvlink)

NV_STATUS knvlinkExecGspRmRpc_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, void *arg4, NvU32 arg5);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkExecGspRmRpc(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, void *arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkExecGspRmRpc(pGpu, pKernelNvlink, arg3, arg4, arg5) knvlinkExecGspRmRpc_IMPL(pGpu, pKernelNvlink, arg3, arg4, arg5)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkExecGspRmRpc_HAL(pGpu, pKernelNvlink, arg3, arg4, arg5) knvlinkExecGspRmRpc(pGpu, pKernelNvlink, arg3, arg4, arg5)

NvBool knvlinkIsNvswitchProxyPresent_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvBool knvlinkIsNvswitchProxyPresent(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink) knvlinkIsNvswitchProxyPresent_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkIsNvswitchProxyPresent_HAL(pGpu, pKernelNvlink) knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink)

void knvlinkDetectNvswitchProxy_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkDetectNvswitchProxy(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkDetectNvswitchProxy(pGpu, pKernelNvlink) knvlinkDetectNvswitchProxy_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkDetectNvswitchProxy_HAL(pGpu, pKernelNvlink) knvlinkDetectNvswitchProxy(pGpu, pKernelNvlink)

NV_STATUS knvlinkSetUniqueFlaBaseAddress_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkSetUniqueFlaBaseAddress(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFlaBaseAddress_IMPL(pGpu, pKernelNvlink, arg3)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkSetUniqueFlaBaseAddress_HAL(pGpu, pKernelNvlink, arg3) knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, arg3)

NV_STATUS knvlinkFloorSweep_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 numLinksPerIp, NvU32 *pNumActiveLinks);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkFloorSweep(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 numLinksPerIp, NvU32 *pNumActiveLinks) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkFloorSweep(pGpu, pKernelNvlink, numLinksPerIp, pNumActiveLinks) knvlinkFloorSweep_IMPL(pGpu, pKernelNvlink, numLinksPerIp, pNumActiveLinks)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkFloorSweep_HAL(pGpu, pKernelNvlink, numLinksPerIp, pNumActiveLinks) knvlinkFloorSweep(pGpu, pKernelNvlink, numLinksPerIp, pNumActiveLinks)

static inline NvU64 knvlinkGetUniqueFabricBaseAddress_e203db(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return pKernelNvlink_PRIVATE->fabricBaseAddr;
}


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetUniqueFabricBaseAddress(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink) knvlinkGetUniqueFabricBaseAddress_e203db(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink) knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink)

static inline NvU64 knvlinkGetUniqueFabricEgmBaseAddress_4de472(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return pKernelNvlink_PRIVATE->fabricEgmBaseAddr;
}


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU64 knvlinkGetUniqueFabricEgmBaseAddress(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink) knvlinkGetUniqueFabricEgmBaseAddress_4de472(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink) knvlinkGetUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink)

NV_STATUS knvlinkStatePostLoadHal_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkStatePostLoadHal(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkStatePostLoadHal(pGpu, pKernelNvlink) knvlinkStatePostLoadHal_GV100(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkStatePostLoadHal_HAL(pGpu, pKernelNvlink) knvlinkStatePostLoadHal(pGpu, pKernelNvlink)

NV_STATUS knvlinkApplyNvswitchDegradedModeSettings_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 *switchLinkMasks);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkApplyNvswitchDegradedModeSettings(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 *switchLinkMasks) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkApplyNvswitchDegradedModeSettings(pGpu, pKernelNvlink, switchLinkMasks) knvlinkApplyNvswitchDegradedModeSettings_GV100(pGpu, pKernelNvlink, switchLinkMasks)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkApplyNvswitchDegradedModeSettings_HAL(pGpu, pKernelNvlink, switchLinkMasks) knvlinkApplyNvswitchDegradedModeSettings(pGpu, pKernelNvlink, switchLinkMasks)

NvU32 knvlinkGetNumActiveLinksPerIoctrl_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetNumActiveLinksPerIoctrl(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetNumActiveLinksPerIoctrl(pGpu, pKernelNvlink) knvlinkGetNumActiveLinksPerIoctrl_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetNumActiveLinksPerIoctrl_HAL(pGpu, pKernelNvlink) knvlinkGetNumActiveLinksPerIoctrl(pGpu, pKernelNvlink)

NvU32 knvlinkGetTotalNumLinksPerIoctrl_IMPL(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NvU32 knvlinkGetTotalNumLinksPerIoctrl(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return 0;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkGetTotalNumLinksPerIoctrl(pGpu, pKernelNvlink) knvlinkGetTotalNumLinksPerIoctrl_IMPL(pGpu, pKernelNvlink)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkGetTotalNumLinksPerIoctrl_HAL(pGpu, pKernelNvlink) knvlinkGetTotalNumLinksPerIoctrl(pGpu, pKernelNvlink)

void knvlinkCoreGetDevicePciInfo_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, nvlink_device_info *devInfo);


#ifdef __nvoc_kernel_nvlink_h_disabled
static inline void knvlinkCoreGetDevicePciInfo(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, nvlink_device_info *devInfo) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkCoreGetDevicePciInfo(pGpu, pKernelNvlink, devInfo) knvlinkCoreGetDevicePciInfo_GP100(pGpu, pKernelNvlink, devInfo)
#endif //__nvoc_kernel_nvlink_h_disabled

#define knvlinkCoreGetDevicePciInfo_HAL(pGpu, pKernelNvlink, devInfo) knvlinkCoreGetDevicePciInfo(pGpu, pKernelNvlink, devInfo)

NV_STATUS knvlinkConstructEngine_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2, ENGDESCRIPTOR arg3);

NV_STATUS knvlinkStatePreInitLocked_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2);

NV_STATUS knvlinkStateLoad_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2, NvU32 arg3);

NV_STATUS knvlinkStatePostLoad_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2, NvU32 arg3);

NV_STATUS knvlinkStateUnload_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2, NvU32 arg3);

NV_STATUS knvlinkStatePostUnload_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2, NvU32 arg3);

static inline NvBool knvlinkIsPresent_3dd2c9(struct OBJGPU *arg1, struct KernelNvlink *arg2) {
    return NV_FALSE;
}

NvBool knvlinkIsPresent_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2);

NV_STATUS knvlinkSetUniqueFabricBaseAddress_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

NV_STATUS knvlinkSetUniqueFabricBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

static inline NV_STATUS knvlinkSetUniqueFabricBaseAddress_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void knvlinkClearUniqueFabricBaseAddress_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkClearUniqueFabricBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

NV_STATUS knvlinkSetUniqueFabricEgmBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

static inline NV_STATUS knvlinkSetUniqueFabricEgmBaseAddress_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void knvlinkClearUniqueFabricEgmBaseAddress_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkClearUniqueFabricEgmBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

NV_STATUS knvlinkHandleFaultUpInterrupt_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);

static inline NV_STATUS knvlinkHandleFaultUpInterrupt_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS knvlinkValidateFabricBaseAddress_GA100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

NV_STATUS knvlinkValidateFabricBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

static inline NV_STATUS knvlinkValidateFabricBaseAddress_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS knvlinkValidateFabricEgmBaseAddress_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

NV_STATUS knvlinkValidateFabricEgmBaseAddress_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3);

static inline NV_STATUS knvlinkValidateFabricEgmBaseAddress_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU64 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU32 knvlinkGetConnectedLinksMask_15a734(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0U;
}

NvU32 knvlinkGetConnectedLinksMask_TU102(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkEnableLinksPostTopology_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return NV_OK;
}

NV_STATUS knvlinkEnableLinksPostTopology_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);

static inline NV_STATUS knvlinkOverrideConfig_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3) {
    return NV_OK;
}

NV_STATUS knvlinkOverrideConfig_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);

NV_STATUS knvlinkOverrideConfig_GA100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3);

NV_STATUS knvlinkFilterBridgeLinks_TU102(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkFilterBridgeLinks_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 knvlinkGetUniquePeerIdMask_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvU32 knvlinkGetUniquePeerIdMask_15a734(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0U;
}

static inline NvU32 knvlinkGetUniquePeerId_c732fb(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu) {
    return 4294967295U;
}

NvU32 knvlinkGetUniquePeerId_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu);

static inline NV_STATUS knvlinkRemoveMapping_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry) {
    return NV_OK;
}

NV_STATUS knvlinkRemoveMapping_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry);

NV_STATUS knvlinkRemoveMapping_GA100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bAllMapping, NvU32 peerMask, NvBool bL2Entry);

static inline NV_STATUS knvlinkGetP2POptimalCEs_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 *arg7) {
    return NV_OK;
}

NV_STATUS knvlinkGetP2POptimalCEs_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 *arg7);

static inline NV_STATUS knvlinkConstructHal_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkConstructHal_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline void knvlinkSetupPeerMapping_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU32 peerId) {
    return;
}

void knvlinkSetupPeerMapping_GP100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU32 peerId);

static inline NV_STATUS knvlinkProgramLinkSpeed_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkProgramLinkSpeed_GV100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkPoweredUpForD3_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_FALSE;
}

NvBool knvlinkPoweredUpForD3_TU102(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkIsAliSupported_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkIsAliSupported_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkPostSetupNvlinkPeer_56cd7a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_OK;
}

NV_STATUS knvlinkPostSetupNvlinkPeer_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

NV_STATUS knvlinkDiscoverPostRxDetLinks_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu);

static inline NV_STATUS knvlinkDiscoverPostRxDetLinks_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pPeerGpu) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS knvlinkLogAliDebugMessages_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bFinal);

NV_STATUS knvlinkLogAliDebugMessages_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bFinal);

static inline NV_STATUS knvlinkLogAliDebugMessages_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvBool bFinal) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void knvlinkDumpCallbackRegister_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkDumpCallbackRegister_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline void knvlinkGetEffectivePeerLinkMask_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU64 *pPeerLinkMask) {
    return;
}

void knvlinkGetEffectivePeerLinkMask_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, struct OBJGPU *pRemoteGpu, NvU64 *pPeerLinkMask);

static inline NvU32 knvlinkGetNumLinksToBeReducedPerIoctrl_4a4dee(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return 0;
}

NvU32 knvlinkGetNumLinksToBeReducedPerIoctrl_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkIsBandwidthModeOff_3dd2c9(struct KernelNvlink *pKernelNvlink) {
    return NV_FALSE;
}

NvBool knvlinkIsBandwidthModeOff_GH100(struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkIsBwModeSupported_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU8 mode) {
    return NV_FALSE;
}

NvBool knvlinkIsBwModeSupported_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU8 mode);

NvBool knvlinkIsBwModeSupported_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU8 mode);

NV_STATUS knvlinkGetHshubSupportedRbmModes_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkGetHshubSupportedRbmModes_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void knvlinkPostSchedulingEnableCallbackRegister_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkPostSchedulingEnableCallbackRegister_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline void knvlinkPostSchedulingEnableCallbackUnregister_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkPostSchedulingEnableCallbackUnregister_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

NV_STATUS knvlinkGetSupportedBwMode_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams);

NV_STATUS knvlinkGetSupportedBwMode_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams);

static inline NV_STATUS knvlinkGetSupportedBwMode_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline void knvlinkDirectConnectCheck_b3696a(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return;
}

void knvlinkDirectConnectCheck_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkIsGpuReducedNvlinkConfig_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_FALSE;
}

NvBool knvlinkIsGpuReducedNvlinkConfig_GA100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkIsFloorSweepingNeeded_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 numActiveLinksPerIoctrl, NvU32 numLinksPerIoctrl) {
    return NV_FALSE;
}

NvBool knvlinkIsFloorSweepingNeeded_GH100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 numActiveLinksPerIoctrl, NvU32 numLinksPerIoctrl);

NV_STATUS knvlinkGetSupportedCounters_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *pParams);

static inline NV_STATUS knvlinkGetSupportedCounters_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool knvlinkIsEncryptEnSet_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_FALSE;
}

NvBool knvlinkIsEncryptEnSet_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NvBool knvlinkIsNvleEnabled_3dd2c9(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_FALSE;
}

NvBool knvlinkIsNvleEnabled_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

NV_STATUS knvlinkEncryptionGetGpuIdentifiers_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink);

static inline NV_STATUS knvlinkEncryptionGetGpuIdentifiers_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS knvlinkEncryptionUpdateTopology_GB100(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 remoteGpuAlid, NvU32 remoteGpuClid);

static inline NV_STATUS knvlinkEncryptionUpdateTopology_46f6a7(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 remoteGpuAlid, NvU32 remoteGpuClid) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU64 knvlinkGetInitializedLinkMask(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return (NvU64)((64 == 32) ? (((NvU32)(((NvU64)(pKernelNvlink_PRIVATE->initializedLinks)) & 4294967295U))) : (pKernelNvlink_PRIVATE->initializedLinks));
}

static inline NvBool knvlinkAreLinksRegistryOverriden(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return pKernelNvlink_PRIVATE->bRegistryLinkOverride;
}

static inline NvU64 knvlinkGetPeerLinkMask(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 peerId) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return (peerId < ((sizeof (pKernelNvlink_PRIVATE->peerLinkMasks) / sizeof ((pKernelNvlink_PRIVATE->peerLinkMasks)[0])))) ? (NvU64)((64 == 32) ? (((NvU32)(((NvU64)(pKernelNvlink_PRIVATE->peerLinkMasks[peerId])) & 4294967295U))) : (pKernelNvlink_PRIVATE->peerLinkMasks[peerId])) : 0;
}

static inline KNVLINK_RM_LINK *knvlinkGetLinkData(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return (linkId < ((sizeof (pKernelNvlink_PRIVATE->nvlinkLinks) / sizeof ((pKernelNvlink_PRIVATE->nvlinkLinks)[0])))) ? &pKernelNvlink_PRIVATE->nvlinkLinks[linkId] : ((void *)0);
}

static inline NvU32 knvlinkGetIPVersion(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return pKernelNvlink_PRIVATE->ipVerNvlink;
}

static inline NvU32 knvlinkGetMinionControl(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink) {
    struct KernelNvlink_PRIVATE *pKernelNvlink_PRIVATE = (struct KernelNvlink_PRIVATE *)pKernelNvlink;
    return pKernelNvlink_PRIVATE->minionControl;
}

void knvlinkDestruct_IMPL(struct KernelNvlink *arg1);

#define __nvoc_knvlinkDestruct(arg1) knvlinkDestruct_IMPL(arg1)
NV_STATUS knvlinkRemoveMissingIoctrlObjects_IMPL(struct OBJGPU *arg1, struct KernelNvlink *arg2);

#ifdef __nvoc_kernel_nvlink_h_disabled
static inline NV_STATUS knvlinkRemoveMissingIoctrlObjects(struct OBJGPU *arg1, struct KernelNvlink *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvlink was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvlink_h_disabled
#define knvlinkRemoveMissingIoctrlObjects(arg1, arg2) knvlinkRemoveMissingIoctrlObjects_IMPL(arg1, arg2)
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
NV_STATUS knvlinkRetrainLinkFromOff (struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId);
NV_STATUS knvlinkRetrainLinkFromSafe(struct OBJGPU *pGpu, struct KernelNvlink *pKernelNvlink, NvU32 linkId);

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
NvlStatus knvlinkCoreAliTrainingCallback             (nvlink_link *link);

NvlStatus knvlinkCoreGetCciLinkModeCallback          (nvlink_link *link, NvU64 *mode);

#endif

// NVLINK Utility Functions
void knvlinkUtoa(NvU8 *, NvU64, NvU64);

NV_STATUS ioctrlFaultUpTmrHandler(struct OBJGPU *, struct OBJTMR *,TMR_EVENT *);

#endif // _KERNEL_NVLINK_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVLINK_NVOC_H_
