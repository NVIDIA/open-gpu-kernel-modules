/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080nvlink.finn
//

#include "nvlimits.h"
#include "nvfixedtypes.h"
#include "ctrl/ctrl2080/ctrl2080base.h"
#include "ctrl/ctrl2080/ctrl2080nvlink_common.h"
#include "nvcfg_sdk.h"

/* NV20_SUBDEVICE_XX bus control commands and parameters */

/*
 * NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS
 *
 * This command returns the NVLink capabilities supported by the subdevice.
 *
 *   capsTbl
 *     This is bit field for getting different global caps. The individual bitfields are specified by NV2080_CTRL_NVLINK_CAPS_* 
 *   lowestNvlinkVersion
 *     This field specifies the lowest supported NVLink version for this subdevice.
 *   highestNvlinkVersion
 *     This field specifies the highest supported NVLink version for this subdevice.
 *   lowestNciVersion
 *     This field specifies the lowest supported NCI version for this subdevice.
 *   highestNciVersion
 *     This field specifies the highest supported NCI version for this subdevice.
 *   discoveredLinkMask // deprecated use discovered links
 *     This field provides a bitfield mask of NVLink links discovered on this subdevice.
 *   enabledLinkMask // deprecated use enabled Links
 *     This field provides a bitfield mask of NVLink links enabled on this subdevice.
 *   discoveredLinks
 *     This field provides a bitfield mask of NVLink links discovered on this subdevice.
 *   enabledLinks
 *     This field provides a bitfield mask of NVLink links enabled on this subdevice.
 *
 */
#define NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS {
    NvU32 capsTbl;

    NvU8  lowestNvlinkVersion;
    NvU8  highestNvlinkVersion;
    NvU8  lowestNciVersion;
    NvU8  highestNciVersion;

    NvU32 discoveredLinkMask;
    NvU32 enabledLinkMask;

    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK discoveredLinks, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK enabledLinks, 8);
} NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV2080_CTRL_NVLINK_GET_CAP(tbl,c)              (((NvU8)tbl[(1?c)]) & (0?c))

/*
 * NV2080_CTRL_NVLINK_CAPS
 *
 *   SUPPORTED
 *     Set if NVLink is present and supported on this subdevice, NV_FALSE otherwise. This field is used for *global* caps only and NOT for per-link caps
 *   P2P_SUPPORTED
 *     Set if P2P over NVLink is supported on this subdevice, NV_FALSE otherwise.
 *   SYSMEM_ACCESS
 *     Set if sysmem can be accessed over NVLink on this subdevice, NV_FALSE otherwise.
 *   PEER_ATOMICS
 *     Set if P2P atomics are supported over NVLink on this subdevice, NV_FALSE otherwise.
 *   SYSMEM_ATOMICS
 *     Set if sysmem atomic transcations are supported over NVLink on this subdevice, NV_FALSE otherwise.
 *   PEX_TUNNELING
 *     Set if PEX tunneling over NVLink is supported on this subdevice, NV_FALSE otherwise.
 *   SLI_BRIDGE
 *     GLOBAL: Set if SLI over NVLink is supported on this subdevice, NV_FALSE otherwise.
 *     LINK:   Set if SLI over NVLink is supported on a link, NV_FALSE otherwise.
 *   SLI_BRIDGE_SENSABLE
 *     GLOBAL: Set if the subdevice is capable of sensing SLI bridges, NV_FALSE otherwise.
 *     LINK:   Set if the link is capable of sensing an SLI bridge, NV_FALSE otherwise.
 *   POWER_STATE_L0
 *     Set if L0 is a supported power state on this subdevice/link, NV_FALSE otherwise.
 *   POWER_STATE_L1
 *     Set if L1 is a supported power state on this subdevice/link, NV_FALSE otherwise.
 *   POWER_STATE_L2
 *     Set if L2 is a supported power state on this subdevice/link, NV_FALSE otherwise.
 *   POWER_STATE_L3
 *     Set if L3 is a supported power state on this subdevice/link, NV_FALSE otherwise.
 *   VALID
 *     Set if this link is supported on this subdevice, NV_FALSE otherwise. This field is used for *per-link* caps only and NOT for global caps.
 *   UNCONTAINED_ERROR_RECOVERY
 *     Set if this GPU supports resetless recovery from uncontained packet errors.
 *
 */

/* caps format is byte_index:bit_mask */
#define NV2080_CTRL_NVLINK_CAPS_SUPPORTED                          0:0x01
#define NV2080_CTRL_NVLINK_CAPS_P2P_SUPPORTED                      0:0x02
#define NV2080_CTRL_NVLINK_CAPS_SYSMEM_ACCESS                      0:0x04
#define NV2080_CTRL_NVLINK_CAPS_P2P_ATOMICS                        0:0x08
#define NV2080_CTRL_NVLINK_CAPS_SYSMEM_ATOMICS                     0:0x10
#define NV2080_CTRL_NVLINK_CAPS_PEX_TUNNELING                      0:0x20
#define NV2080_CTRL_NVLINK_CAPS_SLI_BRIDGE                         0:0x40
#define NV2080_CTRL_NVLINK_CAPS_SLI_BRIDGE_SENSABLE                0:0x80
#define NV2080_CTRL_NVLINK_CAPS_POWER_STATE_L0                     1:0x01
#define NV2080_CTRL_NVLINK_CAPS_POWER_STATE_L1                     1:0x02
#define NV2080_CTRL_NVLINK_CAPS_POWER_STATE_L2                     1:0x04
#define NV2080_CTRL_NVLINK_CAPS_POWER_STATE_L3                     1:0x08
#define NV2080_CTRL_NVLINK_CAPS_VALID                              1:0x10
#define NV2080_CTRL_NVLINK_CAPS_UNCONTAINED_ERROR_RECOVERY         1:0x20

/*
 * Size in bytes of nvlink caps table.  This value should be one greater
 * than the largest byte_index value above.
 */
#define NV2080_CTRL_NVLINK_CAPS_TBL_SIZE               2U

#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_INVALID (0x00000000U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0     (0x00000001U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0     (0x00000002U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2     (0x00000004U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0     (0x00000005U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1     (0x00000006U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0     (0x00000007U)
#define NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0     (0x00000008U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_INVALID    (0x00000000U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_1_0        (0x00000001U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_0        (0x00000002U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_2        (0x00000004U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_0        (0x00000005U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_1        (0x00000006U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_4_0        (0x00000007U)
#define NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_5_0        (0x00000008U)
/*
 * NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS         (0x20803001U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_MESSAGE_ID" */

/* 
 * NV2080_CTRL_NVLINK_DEVICE_INFO
 *
 * This structure stores information about the device to which this link is associated
 *
 *   deviceIdFlags
 *      Bitmask that specifies which IDs are valid for the device
 *      Refer NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_* for possible values
 *      If NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI is set, PCI information is valid
 *      If NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_UUID is set, UUID is valid
 *   domain, bus, device, function, pciDeviceId
 *      PCI information for the device
 *   deviceType
 *      Type of the device
 *      See NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_* for possible values
 *   deviceUUID
 *      This field specifies the device UUID of the device. Useful for identifying the device (or version)
 *   fabricRecoveryStatusMask
 *      This field contains flags which advertise the current GPU-centric health status of the NVLINKs
 */
typedef struct NV2080_CTRL_NVLINK_DEVICE_INFO {
    // ID Flags
    NvU32 deviceIdFlags;

    // PCI Information
    NvU32 domain;
    NvU16 bus;
    NvU16 device;
    NvU16 function;
    NvU32 pciDeviceId;

    // Device Type
    NV_DECLARE_ALIGNED(NvU64 deviceType, 8);

    // Device UUID
    NvU8  deviceUUID[16];

    // GPU-centric fabric health
    NvU32 fabricRecoveryStatusMask;
} NV2080_CTRL_NVLINK_DEVICE_INFO;

#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS        31:0 
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_NONE                                            (0x00000000U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI                                             (0x00000001U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_UUID                                            (0x00000002U)

#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_EBRIDGE                                             (0x00000000U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NPU                                                 (0x00000001U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU                                                 (0x00000002U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH                                              (0x00000003U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_TEGRA                                               (0x00000004U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE                                                (0x000000FFU)

#define NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_UUID_INVALID                                             (0xFFFFFFFFU)

#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_GPU_DEGRADED                      0:0
#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_GPU_DEGRADED_FALSE                  (0x00000000U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_GPU_DEGRADED_TRUE                   (0x00000001U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_UNCONTAINED_ERROR_RECOVERY        1:1
#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_UNCONTAINED_ERROR_RECOVERY_INACTIVE (0x00000000U)
#define NV2080_CTRL_NVLINK_DEVICE_INFO_FABRIC_RECOVERY_STATUS_MASK_UNCONTAINED_ERROR_RECOVERY_ACTIVE   (0x00000001U)

/*
 * NV2080_CTRL_NVLINK_NVLINK_LINK_STATUS_INFO
 *
 * This structure stores the per-link status of different NVLink parameters.
 *
 *   capsTbl
 *     This is bit field for getting different global caps. The individual bitfields
 *     are specified by NV2080_CTRL_NVLINK_CAPS_*
 *   phyType
 *     This field specifies the type of PHY (NVHS or GRS) being used for this link.
 *   subLinkWidth
 *     This field specifies the no. of lanes per sublink.
 *   linkState
 *     This field specifies the current state of the link.
 *     See NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_LINK_STATE_* for possible values.
 *   rxSublinkStatus
 *     This field specifies the current state of RX sublink.
 *     See NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_SUBLINK_RX_STATE_* for possible values.
 *   txSublinkStatus
 *     This field specifies the current state of TX sublink.
 *     See NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_SUBLINK_TX_STATE_* for possible values.
 *   bLaneReversal
 *     This field indicates that lane reversal is in effect on this link.
 *   nvlinkVersion
 *     This field specifies the NVLink version supported by the link.
 *   nciVersion
 *     This field specifies the NCI version supported by the link.
 *   phyVersion
 *     This field specifies the version of PHY being used by the link.
 *   nvlinkLineRateMbps
 *      Bit rate at which bits toggle on wires in megabits per second.
 *      NOTE: This value is the full speed line rate, not the instantaneous line rate of the link.
 *   nvlinkLinkClockMhz
 *      Clock corresponding to link logic in mega hertz
 *   nvlinkRefClkType
 *      This field specifies whether the link clock is taken from NVHS reflck
 *      or PEX refclk for the current GPU.
 *   nvlinkLinkDataRateKiBps
 *      Effective rate available for transactions after subtracting overhead,
 *      as seen at Data Layer in kibibytes (1024 bytes) per second.
 *      Only valid in GA100+, reported as 0 otherwise
 *      NOTE: Because minion calculates these values, it will only be valid if
 *            links are in ACTIVE state
 *   nvlinkRefClkSpeedMhz
 *      The input reference frequency to the PLL
 *   connected
 *     This field specifies if any device is connected on the other end of the link
 *   loopProperty
 *     This field specifies if the link is a loopback/loopout link. See NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_* for possible values.
 *   remoteDeviceLinkNumber
 *     This field specifies the link number on the remote end of the link 
 *   remoteDeviceInfo
 *     This field stores the device information for the remote end of the link
 *   nvlinkMinL1Threshold
 *     This field stores the Min L1 Thresohld of the link
 *   nvlinkMaxL1Threshold
 *     This field stores the Max L1 Threshold of the link
 *   nvlinkL1ThresholdUnits
 *     This field stores the L1 Threshold Units of the link
 *   nvlpwIdx
 *     This field stores the nvlpw index in use
 *
 */
typedef struct NV2080_CTRL_NVLINK_LINK_STATUS_INFO {
    // Top level capablilites
    NvU32  capsTbl;

    NvU8   phyType;
    NvU8   subLinkWidth;

    // Link and sublink states
    NvU32  linkState;
    NvU8   rxSublinkStatus;
    NvU8   txSublinkStatus;

    // Indicates that lane reversal is in effect on this link.
    NvBool bLaneReversal;

    NvU8   nvlinkVersion;
    NvU8   nciVersion;
    NvU8   phyVersion;

    // Legacy clock information (to be deprecated)
    NvU32  nvlinkLinkClockKHz;
    NvU32  nvlinkCommonClockSpeedKHz;
    NvU32  nvlinkRefClkSpeedKHz;

    NvU32  nvlinkCommonClockSpeedMhz;

    // Clock Speed and Data Rate Reporting
    NvU32  nvlinkLineRateMbps;
    NvU32  nvlinkLinkClockMhz;
    NvU8   nvlinkRefClkType;
    NvU32  nvlinkLinkDataRateKiBps;
    NvU32  nvlinkRefClkSpeedMhz;

    // Connection information
    NvBool connected;
    NvU8   loopProperty;
    NvU8   remoteDeviceLinkNumber;
    NvU8   localDeviceLinkNumber;

    //
    // Added as part of NvLink 3.0
    // Note: SID has link info appended to it when provided by minion
    //
    NV_DECLARE_ALIGNED(NvU64 remoteLinkSid, 8);
    NV_DECLARE_ALIGNED(NvU64 localLinkSid, 8);

    // Ampere+ only
    NvU32  laneRxdetStatusMask;

    // L1 Threshold Info
    NvU32  nvlinkMinL1Threshold;
    NvU32  nvlinkMaxL1Threshold;
    NvU32  nvlinkL1ThresholdUnits;

    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_DEVICE_INFO remoteDeviceInfo, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_DEVICE_INFO localDeviceInfo, 8);

    NvU32  nvlpwIdx;
} NV2080_CTRL_NVLINK_LINK_STATUS_INFO;

// NVLink link states
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_INIT               (0x00000000U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_HWCFG              (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_SWCFG              (0x00000002U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_ACTIVE             (0x00000003U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_FAULT              (0x00000004U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_SLEEP              (0x00000005U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_RECOVERY           (0x00000006U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_RECOVERY_AC        (0x00000008U)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_RECOVERY_RX        (0x0000000aU)
#define NV2080_CTRL_NVLINK_STATUS_LINK_STATE_INVALID            (0xFFFFFFFFU)

// NVLink Rx sublink states
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_HIGH_SPEED_1 (0x00000000U)
// TODO: @achaudhry remove SINGLE_LANE define once references switch to LOW_POWER
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_SINGLE_LANE  (0x00000004) // Deprecated
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_LOW_POWER    (0x00000004)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_TRAINING     (0x00000005U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_SAFE_MODE    (0x00000006U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_OFF          (0x00000007U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_TEST         (0x00000008U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_FAULT        (0x0000000eU)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_INVALID      (0x000000FFU)

// NVLink Tx sublink states
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_HIGH_SPEED_1 (0x00000000U)
// TODO: @achaudhry remove SINGLE_LANE define once references switch to LOW_POWER
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_SINGLE_LANE  (0x00000004) // Deprecated
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_LOW_POWER    (0x00000004)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_TRAINING     (0x00000005U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_SAFE_MODE    (0x00000006U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_OFF          (0x00000007U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_TEST         (0x00000008U)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_FAULT        (0x0000000eU)
#define NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_INVALID      (0x000000FFU)

#define NV2080_CTRL_NVLINK_STATUS_PHY_NVHS                      (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_PHY_GRS                       (0x00000002U)
#define NV2080_CTRL_NVLINK_STATUS_PHY_INVALID                   (0x000000FFU)

// Version information
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0            (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_0            (0x00000002U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_2            (0x00000004U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_0            (0x00000005U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_1            (0x00000006U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_4_0            (0x00000007U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_5_0            (0x00000008U)
#define NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID        (0x000000FFU)

#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_1_0               (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_2_0               (0x00000002U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_2_2               (0x00000004U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_3_0               (0x00000005U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_3_1               (0x00000006U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_4_0               (0x00000007U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_5_0               (0x00000008U)
#define NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_INVALID           (0x000000FFU)

#define NV2080_CTRL_NVLINK_STATUS_NVHS_VERSION_1_0              (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_NVHS_VERSION_INVALID          (0x000000FFU)

#define NV2080_CTRL_NVLINK_STATUS_GRS_VERSION_1_0               (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_GRS_VERSION_INVALID           (0x000000FFU)

// Connection properties
#define NV2080_CTRL_NVLINK_STATUS_CONNECTED_TRUE                (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_CONNECTED_FALSE               (0x00000000U)

#define NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPBACK        (0x00000001U)
#define NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPOUT         (0x00000002U)
#define NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE            (0x00000000U)

#define NV2080_CTRL_NVLINK_STATUS_REMOTE_LINK_NUMBER_INVALID    (0x000000FFU)

// L1 Threshold Units
typedef enum NV2080_CTRL_NVLINK_STATUS_L1_THRESHOLD_UNIT {
    NV2080_CTRL_NVLINK_STATUS_L1_THRESHOLD_UNIT_100US = 0,
    NV2080_CTRL_NVLINK_STATUS_L1_THRESHOLD_UNIT_50US = 1,
} NV2080_CTRL_NVLINK_STATUS_L1_THRESHOLD_UNIT;

// NVLink REFCLK types
#define NV2080_CTRL_NVLINK_REFCLK_TYPE_INVALID (0x00U)
#define NV2080_CTRL_NVLINK_REFCLK_TYPE_NVHS    (0x01U)
#define NV2080_CTRL_NVLINK_REFCLK_TYPE_PEX     (0x02U)

#define NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS {
    NvU32  enabledLinkMask; // (This field will be deprecated in the future, use enabledLinks)
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK enabledLinks, 8);
    NvBool bSublinkStateInst; // whether instantaneous sublink state is needed
    NvBool bNvleModeEnabled; // whether Nvlink Encryption is enabled or not
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_STATUS_INFO linkInfo[NV2080_CTRL_NVLINK_MAX_ARR_SIZE], 8);
    NV_DECLARE_ALIGNED(NvU64 enabledNvlpwMask, 8);
} NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS
 *
 *   enabledLinkMask
 *     This field specifies the mask of available links on this subdevice.
 *   bNvleModeEnabled
 *     This field indicates if Nvlink Encryption is enabled or not.
 *   linkInfo
 *     This structure stores the per-link status of different NVLink parameters. The link is identified using an index.
 *   enabledNvlpwMask
 *     This field specifics mask of Nvlpw indices available
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS (0x20803002U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_NVLINK_ERR_INFO
 *   Error information per link
 *
 *   TLErrlog
 *     Returns the error mask for NVLINK TL errors
 *     Used in Pascal
 *
 *   TLIntrEn
 *     Returns the intr enable mask for NVLINK TL errors
 *     Used in Pascal
 *
 *   TLCTxErrStatus0
 *     Returns the TLC Tx Error Mask 0
 *     Used in Volta and later
 *
 *   TLCTxErrStatus1
 *     Returns the TLC Tx Error Mask 1
 *     Used in Ampere and later
 *
 *   TLCTxSysErrStatus0
 *     Returns the TLC Tx Sys Error Mask 0
 *     Used in Ampere and later.
 *
 *   TLCRxErrStatus0
 *     Returns the TLC Rx Error Mask 0
 *     Used in Volta and later
 *
 *   TLCRxErrStatus1
 *     Returns the TLC Rx Error Mask 1
 *     Used in Volta and later
 *
 *   TLCRxSysErrStatus0
 *     Returns the TLC Rx Sys Error Mask 0
 *     Used in Ampere and later.
 *
 *   TLCTxErrLogEn0
 *     Returns the TLC Tx Error Log En 0
 *     Used in Volta and later
 *
 *   TLCTxErrLogEn1
 *     Returns the TLC Tx Error Log En 1
 *     Used in Ampere and later
 *
 *   TLCTxSysErrLogEn0
 *     Returns the TLC Tx Sys Error Log En 0
 *     Used in Ampere and later
 *
 *   TLCRxErrLogEn0
 *     Returns the TLC Rx Error Log En 0
 *     Used in Volta and later
 *
 *   TLCRxErrLogEn1
 *     Returns the TLC Rx Error Log En 1
 *     Used in Volta and later
 *
 *   TLCRxSysErrLogEn0
 *     Returns the TLC Rx Sys Error Log En 0
 *     Used in Ampere and later
 *
 *   MIFTxErrStatus0
 *     Returns the MIF Rx Error Mask 0
 *     Used in Volta and Turing
 *
 *   MIFRxErrStatus0
 *     Returns the MIF Tx Error Mask 0
 *     Used in Volta and Turing
 *
 *   NVLIPTLnkErrStatus0
 *     Returns the NVLIPT_LNK Error Mask 0
 *     Used in Ampere and later
 *
 *   NVLIPTLnkErrLogEn0
 *     Returns the NVLIPT_LNK Log En Mask 0
 *     Used in Ampere and later
 *
 *   NVLIPTLnkCtrlLinkStateRequest
 *     Returns the NVLIPT_LNK Control Link State Request value
 *     Used in Ampere and later
 *
 *   DLSpeedStatusTx
 *     Returns the NVLINK DL speed status for sublink Tx
 *
 *   DLSpeedStatusRx
 *     Returns the NVLINK DL speed status for sublink Rx
 *
 *   NVLDLRxSlsmErrCntl
 *     Returns the NVLDL_RXSLSM_ERR_CNTL value
 *     Used in Hopper and later
 *
 *   NVLDLTopLinkState
 *     Returns the NVLDL_TOP_LINK_STATE value
 *     Used in Hopper and later
 *
 *   NVLDLTopIntr
 *     Returns the NVLDL_TOP_INTR value
 *     Used in Hopper and later
 *
 *   DLStatMN00
 *     Returns the DLSTAT MN00 Code and subcode
 *     Used in Hopper and later
 *
 *   DLStatUC01
 *     Returns the DLSTAT UC01 value
 *     Used in Hopper and later
 *
 *   MinionNvlinkLinkIntr
 *     Returns the MINION_NVLINK_LINK_INTR code and subcode
 *     Used in Hopper and later
 *
 *   bExcessErrorDL
 *     Returns true for excessive error rate interrupt from DL
 */
typedef struct NV2080_CTRL_NVLINK_ERR_INFO {
    NvU32  TLErrlog;
    NvU32  TLIntrEn;
    NvU32  TLCTxErrStatus0;
    NvU32  TLCTxErrStatus1;
    NvU32  TLCTxSysErrStatus0;
    NvU32  TLCRxErrStatus0;
    NvU32  TLCRxErrStatus1;
    NvU32  TLCRxSysErrStatus0;
    NvU32  TLCTxErrLogEn0;
    NvU32  TLCTxErrLogEn1;
    NvU32  TLCTxSysErrLogEn0;
    NvU32  TLCRxErrLogEn0;
    NvU32  TLCRxErrLogEn1;
    NvU32  TLCRxSysErrLogEn0;
    NvU32  MIFTxErrStatus0;
    NvU32  MIFRxErrStatus0;
    NvU32  NVLIPTLnkErrStatus0;
    NvU32  NVLIPTLnkErrLogEn0;
    NvU32  NVLIPTLnkCtrlLinkStateRequest;
    NvU32  DLSpeedStatusTx;
    NvU32  DLSpeedStatusRx;
    NvU32  NVLDLRxSlsmErrCntl;
    NvU32  NVLDLTopLinkState;
    NvU32  NVLDLTopIntr;
    NvU32  DLStatMN00;
    NvU32  DLStatUC01;
    NvU32  MinionNvlinkLinkIntr;
    NvBool bExcessErrorDL;
} NV2080_CTRL_NVLINK_ERR_INFO;

/*
 * NV2080_CTRL_NVLINK_COMMON_ERR_INFO
 *   Error information per IOCTRL
 *
 *   NVLIPTErrStatus0
 *     Returns the NVLIPT_COMMON Error Mask 0
 *     Used in Ampere and later
 *
 *   NVLIPTErrLogEn0
 *     Returns the NVLIPT_COMMON Log En Mask 0
 *     Used in Ampere and later
 */
typedef struct NV2080_CTRL_NVLINK_COMMON_ERR_INFO {
    NvU32 NVLIPTErrStatus0;
    NvU32 NVLIPTErrLogEn0;
} NV2080_CTRL_NVLINK_COMMON_ERR_INFO;

/* Extract the error status bit for a given TL error index i */
#define NV2080_CTRL_NVLINK_GET_TL_ERRLOG_BIT(intr, i)       (((1U << i) & (intr)) >> i)

/* Extract the intr enable bit for a given TL error index i */
#define NV2080_CTRL_NVLINK_GET_TL_INTEN_BIT(intr, i)        NV2080_CTRL_NVLINK_GET_TL_ERRLOG_BIT(intr, i)

/* Error status values for a given NVLINK TL error */
#define NV2080_CTRL_NVLINK_TL_ERRLOG_TRUE                               (0x00000001U)
#define NV2080_CTRL_NVLINK_TL_ERRLOG_FALSE                              (0x00000000U)

/* Intr enable/disable for a given NVLINK TL error */
#define NV2080_CTRL_NVLINK_TL_INTEN_TRUE                                (0x00000001U)
#define NV2080_CTRL_NVLINK_TL_INTEN_FALSE                               (0x00000000U)

/* NVLINK TL interrupt enable fields for errors */
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXDLDATAPARITYEN                0U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXDLCTRLPARITYEN                1U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXPROTOCOLEN                    2U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXOVERFLOWEN                    3U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXRAMDATAPARITYEN               4U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXRAMHDRPARITYEN                5U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXRESPEN                        6U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_RXPOISONEN                      7U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_TXRAMDATAPARITYEN               8U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_TXRAMHDRPARITYEN                9U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_DLFLOWPARITYEN                  10U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_DLHDRPARITYEN                   12U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_TXCREDITEN                      13U
#define NV2080_CTRL_NVLINK_TL_INTEN_IDX_MAX                             14U

/* NVLINK TL error fields */
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXDLDATAPARITYERR              0U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXDLCTRLPARITYERR              1U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXPROTOCOLERR                  2U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXOVERFLOWERR                  3U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXRAMDATAPARITYERR             4U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXRAMHDRPARITYERR              5U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXRESPERR                      6U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_RXPOISONERR                    7U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_TXRAMDATAPARITYERR             8U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_TXRAMHDRPARITYERR              9U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_DLFLOWPARITYERR                10U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_DLHDRPARITYERR                 12U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_TXCREDITERR                    13U
#define NV2080_CTRL_NVLINK_TL_ERRLOG_IDX_MAX                            14U

/* NVLINK DL speed status for sublink Tx*/
#define NV2080_CTRL_NVLINK_SL0_SLSM_STATUS_TX_PRIMARY_STATE_HS          (0x00000000U)
#define NV2080_CTRL_NVLINK_SL0_SLSM_STATUS_TX_PRIMARY_STATE_SINGLE_LANE (0x00000004U)
#define NV2080_CTRL_NVLINK_SL0_SLSM_STATUS_TX_PRIMARY_STATE_TRAIN       (0x00000005U)
#define NV2080_CTRL_NVLINK_SL0_SLSM_STATUS_TX_PRIMARY_STATE_SAFE        (0x00000006U)
#define NV2080_CTRL_NVLINK_SL0_SLSM_STATUS_TX_PRIMARY_STATE_OFF         (0x00000007U)

/* NVLINK DL speed status for sublink Rx*/
#define NV2080_CTRL_NVLINK_SL1_SLSM_STATUS_RX_PRIMARY_STATE_HS          (0x00000000U)
#define NV2080_CTRL_NVLINK_SL1_SLSM_STATUS_RX_PRIMARY_STATE_SINGLE_LANE (0x00000004U)
#define NV2080_CTRL_NVLINK_SL1_SLSM_STATUS_RX_PRIMARY_STATE_TRAIN       (0x00000005U)
#define NV2080_CTRL_NVLINK_SL1_SLSM_STATUS_RX_PRIMARY_STATE_SAFE        (0x00000006U)
#define NV2080_CTRL_NVLINK_SL1_SLSM_STATUS_RX_PRIMARY_STATE_OFF         (0x00000007U)

/* Flags to query different debug registers */
#define NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_DEFAULT                       (0x0U)
#define NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_INTR_STATUS                   (0x1U)
#define NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_ALI_STATUS                    (0x2U)

#define NV2080_CTRL_NVLINK_MAX_IOCTRLS                                  3U
/*
 *   NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS
 *
 *   linkMask (This field will be deprecated in the future, please use links)
 *     Returns the mask of links enabled
 *
 *   links
 *     Returns the mask of links enabled
 *
 *   linkErrInfo
 *     Returns the error information for all the links
 *
 *   ioctrlMask
 *     Returns the mask of ioctrls
 *
 *   commonErrInfo
 *     Returns the error information common to each IOCTRL
 *
 *   ErrInfoFlags
 *     Input for determining which values to query. Possible values:
 *     NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_INTR_STATUS
 *     NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_ALI_STATUS
 *
 */
#define NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS {
    NvU32                              linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_ERR_INFO        linkErrInfo[NV2080_CTRL_NVLINK_MAX_ARR_SIZE];
    NvU32                              ioctrlMask;
    NV2080_CTRL_NVLINK_COMMON_ERR_INFO commonErrInfo[NV2080_CTRL_NVLINK_MAX_IOCTRLS];
    NvU8                               ErrInfoFlags;
} NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_ERR_INFO
 *     This command is used to query the NVLINK error information
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_NVLINK_GET_ERR_INFO                 (0x20803003U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS_MESSAGE_ID" */

/*
 * APIs for getting NVLink counters
 */

// These are the bitmask definitions for different counter types

#define NV2080_CTRL_NVLINK_COUNTER_INVALID                  0x00000000U

#define NV2080_CTRL_NVLINK_COUNTER_TL_TX0                   0x00000001U
#define NV2080_CTRL_NVLINK_COUNTER_TL_TX1                   0x00000002U
#define NV2080_CTRL_NVLINK_COUNTER_TL_RX0                   0x00000004U
#define NV2080_CTRL_NVLINK_COUNTER_TL_RX1                   0x00000008U

#define NV2080_CTRL_NVLINK_LP_COUNTERS_DL                   0x00000010U

#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE_L(i)      (1 << (i + 8))
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE__SIZE 4U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE_L0    0x00000100U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE_L1    0x00000200U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE_L2    0x00000400U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_ECC_LANE_L3    0x00000800U

#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT       0x00010000U

#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(i)      (1 << (i + 17))
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE 8U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0    0x00020000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1    0x00040000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2    0x00080000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3    0x00100000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4    0x00200000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5    0x00400000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6    0x00800000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7    0x01000000U

#define NV2080_CTRL_NVLINK_COUNTER_DL_TX_ERR_REPLAY         0x02000000U
#define NV2080_CTRL_NVLINK_COUNTER_DL_TX_ERR_RECOVERY       0x04000000U

#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_REPLAY         0x08000000U

#define NV2080_CTRL_NVLINK_COUNTER_DL_RX_ERR_CRC_MASKED     0x10000000U

/*
 * Note that COUNTER_MAX_TYPES will need to be updated each time
 * a new counter type gets added to the list above.
 *
 */
#define NV2080_CTRL_NVLINK_COUNTER_MAX_TYPES                32U

/*
 * NV2080_CTRL_CMD_NVLINK_GET_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * [in] counterMask
 *     Mask of counter types to be queried
 *     One of NV2080_CTRL_NVLINK_COUNTERS_TYPE_* macros
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *     Mask of links to be queried
 *
 * [in] links
 *     Mask of links to be queried
 *
 * [out] counters
 *     Counter value returned
 *
 * [out] bTx0TlCounterOverflow
 *  This boolean is set to NV_TRUE if TX Counter 0 has rolled over.
 *
 * [out] bTx1TlCounterOverflow
 *  This boolean is set to NV_TRUE if TX Counter 1 has rolled over.
 *
 * [out] bRx0TlCounterOverflow
 *  This boolean is set to NV_TRUE if RX Counter 0 has rolled over.
 *
 * [out] bRx1TlCounterOverflow
 *  This boolean is set to NV_TRUE if RX Counter 1 has rolled over.
 *
 * [out] value 
 *  This array contains the error counts for each error type as requested from
 *  the counterMask. The array indexes correspond to the mask bits one-to-one.
 */
typedef struct NV2080_CTRL_NVLINK_GET_COUNTERS_VALUES {
    NvBool bTx0TlCounterOverflow;
    NvBool bTx1TlCounterOverflow;
    NvBool bRx0TlCounterOverflow;
    NvBool bRx1TlCounterOverflow;
    NV_DECLARE_ALIGNED(NvU64 value[NV2080_CTRL_NVLINK_COUNTER_MAX_TYPES], 8);
} NV2080_CTRL_NVLINK_GET_COUNTERS_VALUES;

#define NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS {
    NvU32 counterMask;
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_GET_COUNTERS_VALUES counters[NV2080_CTRL_NVLINK_MAX_ARR_SIZE], 8);
} NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_COUNTERS   (0x20803004U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS_MESSAGE_ID" */


/*
 * NV2080_CTRL_CMD_NVLINK_CLEAR_COUNTERS
 *  This command clears/resets the counters for the specified types.
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *  This parameter specifies for which links we want to clear the 
 *  counters.
 *
 * [in] links
 *  This parameter specifies for which links we want to clear the 
 *  counters.
 *
 * [in] counterMask
 *  This parameter specifies the input mask for desired counters to be
 *  cleared. Note that all counters cannot be cleared.
 *
 *  NOTE: Bug# 2098529: On Turing all DL errors and LP counters are cleared
 *        together. They cannot be cleared individually per error type. RM
 *        would possibly move to a new API on Ampere and beyond
 */

#define NV2080_CTRL_CMD_NVLINK_CLEAR_COUNTERS (0x20803005U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS {
    NvU32 counterMask;
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
} NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS;


#define NV2080_CTRL_NVLINK_COUNTER_TP_TL_TX0                                  0U
#define NV2080_CTRL_NVLINK_COUNTER_TP_TL_TX1                                  1U
#define NV2080_CTRL_NVLINK_COUNTER_TP_TL_RX0                                  2U
#define NV2080_CTRL_NVLINK_COUNTER_TP_TL_RX1                                  3U

#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_ECC_LANE_SIZE                4U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_ECC_LANE_L0                  4U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_ECC_LANE_L1                  5U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_ECC_LANE_L2                  6U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_ECC_LANE_L3                  7U

#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_SIZE                8U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L0                  8U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L1                  9U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L2                  10U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L3                  11U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L4                  12U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L5                  13U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L6                  14U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_LANE_L7                  15U

#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_TX_ERR_RECOVERY                     16U

#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_TX_ERR_REPLAY                       17U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_REPLAY                       18U

#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_MASKED                   19U
#define NV2080_CTRL_NVLINK_COUNTER_ERR_DL_RX_ERR_CRC_FLIT                     20U

#define NV2080_CTRL_NVLINK_COUNTER_LP_DL                                      21U

#define NV2080_CTRL_NVLINK_COUNTER_V1_MAX_COUNTER NV2080_CTRL_NVLINK_COUNTER_LP_DL

/* Transmit Counters */
#define NV2080_CTRL_NVLINK_COUNTER_XMIT_PACKETS                               22U
#define NV2080_CTRL_NVLINK_COUNTER_XMIT_BYTES                                 23U
/* Received Counters */
#define NV2080_CTRL_NVLINK_COUNTER_RCV_PACKETS                                24U
#define NV2080_CTRL_NVLINK_COUNTER_RCV_BYTES                                  25U
/* Link Events */
#define NV2080_CTRL_NVLINK_COUNTER_LINK_ERROR_RECOVERY_COUNTER                26U
#define NV2080_CTRL_NVLINK_COUNTER_LINK_DOWNED_COUNTER                        27U
#define NV2080_CTRL_NVLINK_COUNTER_LINK_RECOVERY_SUCCESSFUL_COUNTER           28U
/* Link Receive Errors */
#define NV2080_CTRL_NVLINK_COUNTER_RCV_ERRORS                                 29U
#define NV2080_CTRL_NVLINK_COUNTER_RCV_REMOTE_ERRORS                          30U
#define NV2080_CTRL_NVLINK_COUNTER_RCV_GENERAL_ERRORS                         31U
/* Link Receive Errors Detail */
#define NV2080_CTRL_NVLINK_COUNTER_RCV_MALFORMED_PKT_ERROR                    32U
#define NV2080_CTRL_NVLINK_COUNTER_RCV_BUFFER_OVERRUN_ERROR                   33U
#define NV2080_CTRL_NVLINK_COUNTER_RCV_VL15DROPPED_ERROR                      34U
/* Link Other Errors Detail */
#define NV2080_CTRL_NVLINK_COUNTER_LINK_INTEGRITY_ERRORS                      35U
#define NV2080_CTRL_NVLINK_COUNTER_BUFFER_OVERRUN_ERRORS                      36U
/* Link Transmit Errors */
#define NV2080_CTRL_NVLINK_COUNTER_XMIT_WAIT_TIME                             37U
#define NV2080_CTRL_NVLINK_COUNTER_XMIT_ERRORS                                38U
/* FEC Block Counters */
#define NV2080_CTRL_NVLINK_COUNTER_SINGLE_ERROR_BLOCKS                        39U
#define NV2080_CTRL_NVLINK_COUNTER_CORRECTED_BLOCKS                           40U
#define NV2080_CTRL_NVLINK_COUNTER_UNCORRECTED_BLOCKS                         41U
/* FEC Symbol Counters */
#define NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_LANE_0                   42U
#define NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_LANE_1                   43U
#define NV2080_CTRL_NVLINK_COUNTER_CORRECTED_SYMBOLS_TOTAL                    44U
/* FEC Raw Error Counters */
#define NV2080_CTRL_NVLINK_COUNTER_RAW_ERRORS_LANE_0                          45U
#define NV2080_CTRL_NVLINK_COUNTER_RAW_ERRORS_LANE_1                          46U
#define NV2080_CTRL_NVLINK_COUNTER_CORRECTED_BITS                             47U
/* FEC Raw BER */
#define NV2080_CTRL_NVLINK_COUNTER_RAW_BER_LANE_0                             48U
#define NV2080_CTRL_NVLINK_COUNTER_RAW_BER_LANE_1                             49U
#define NV2080_CTRL_NVLINK_COUNTER_RAW_BER_TOTAL                              50U
/* FEC Effective BER */
#define NV2080_CTRL_NVLINK_COUNTER_NO_ERROR_BLOCKS                            51U
#define NV2080_CTRL_NVLINK_COUNTER_EFFECTIVE_ERRORS                           52U
#define NV2080_CTRL_NVLINK_COUNTER_EFFECTIVE_BER                              53U
/* Phy Symbol Errors Counters */
#define NV2080_CTRL_NVLINK_COUNTER_SYMBOL_ERRORS                              54U
#define NV2080_CTRL_NVLINK_COUNTER_SYMBOL_BER                                 55U
#define NV2080_CTRL_NVLINK_COUNTER_RECEIVED_BITS                              56U
/* Phy Other Errors Counters */
#define NV2080_CTRL_NVLINK_COUNTER_SYNC_HEADER_ERRORS                         57U
#define NV2080_CTRL_NVLINK_COUNTER_TIME_SINCE_LAST_CLEAR                      58U
/* PLR Receive Counters */
#define NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS                             59U
#define NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS_WITH_UNCORRECTABLE_ERRORS   60U
#define NV2080_CTRL_NVLINK_COUNTER_PLR_RCV_BLOCKS_WITH_ERRORS                 61U
/* PLR Transmit Counters */
#define NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_BLOCKS                            62U
#define NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_RETRY_BLOCKS                      63U
#define NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_RETRY_EVENTS                      64U
/* PLR BW Loss Counters */
#define NV2080_CTRL_NVLINK_COUNTER_PLR_BW_LOSS                                65U
/* NVLE Rx counters */
#define NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_GOOD                               66U
#define NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_ERROR                              67U
#define NV2080_CTRL_NVLINK_COUNTER_NVLE_RX_AUTH                               68U
/* NVLE Tx Counters */
#define NV2080_CTRL_NVLINK_COUNTER_NVLE_TX_GOOD                               69U
#define NV2080_CTRL_NVLINK_COUNTER_NVLE_TX_ERROR                              70U
/* FEC Histogram */
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_0                                  71U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_1                                  72U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_2                                  73U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_3                                  74U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_4                                  75U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_5                                  76U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_6                                  77U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_7                                  78U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_8                                  79U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_9                                  80U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_10                                 81U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_11                                 82U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_12                                 83U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_13                                 84U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_14                                 85U
#define NV2080_CTRL_NVLINK_COUNTER_HISTORY_15                                 86U

/* Throughput counters */
#define NV2080_CTRL_NVLINK_COUNTER_TP_RX_DATA                                 87U
#define NV2080_CTRL_NVLINK_COUNTER_TP_TX_DATA                                 88U
#define NV2080_CTRL_NVLINK_COUNTER_TP_RX_RAW                                  89U
#define NV2080_CTRL_NVLINK_COUNTER_TP_TX_RAW                                  90U

/* Low power counters */
#define NV2080_CTRL_NVLINK_COUNTER_L1_ENTRY                                   91U
#define NV2080_CTRL_NVLINK_COUNTER_L1_ENTRY_FORCE                             92U
#define NV2080_CTRL_NVLINK_COUNTER_L1_EXIT                                    93U
#define NV2080_CTRL_NVLINK_COUNTER_L1_EXIT_RECAL                              94U
#define NV2080_CTRL_NVLINK_COUNTER_L1_EXIT_REMOTE                             95U
#define NV2080_CTRL_NVLINK_COUNTER_L1_LP_STEADY_STATE_TIME                    96U
#define NV2080_CTRL_NVLINK_COUNTER_L1_HIGH_SPEED_STEADY_STATE_TIME            97U
#define NV2080_CTRL_NVLINK_COUNTER_L1_OTHER_STATE_TIME                        98U
#define NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_ENTRY_TIME                        99U
#define NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_EXIT_TIME                         100U
#define NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_FULL_BW_ENTRY_TIME                101U
#define NV2080_CTRL_NVLINK_COUNTER_LP_LOCAL_FULL_BW_EXIT_TIME                 102U
#define NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_ENTRY_TIME                       103U
#define NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_EXIT_TIME                        104U
#define NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_FULL_BW_ENTRY_TIME               105U
#define NV2080_CTRL_NVLINK_COUNTER_LP_REMOTE_FULL_BW_EXIT_TIME                106U

#define NV2080_CTRL_NVLINK_COUNTER_PLR_LAST_RAW_BER                           107U
#define NV2080_CTRL_NVLINK_COUNTER_PLR_XMIT_RETRY_EVENTS_WITHIN_T_SEC_MAX_LOW 108U

#define NV2080_CTRL_NVLINK_COUNTERS_MAX                                       109U

#define NV2080_CTRL_NVLINK_COUNTER_MAX_GROUPS                                 2U
#define NV2080_CTRL_NVLINK_COUNTER_MAX_COUNTERS_PER_LINK_IN_REQ               28
#define NV2080_CTRL_NVLINK_COUNTER_V2_GROUP(i)  ((i) / 64)
#define NV2080_CTRL_NVLINK_COUNTER_V2_COUNTER_MASK(i) ((NvU64)1 << ((i) % 64))

/*
 * NV2080_CTRL_CMD_NVLINK_GET_COUNTERS_V2
 *  This command gets the counts for different counter types.
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *     Mask of links to be queried
 *
 * [in] links
 *     Mask of links to be queried
 *
 * [in] counterMask
 *     Mask of counter types to be queried
 *     One of NV2080_CTRL_NVLINK_COUNTERS_TYPE_* macros
 *
 * [out] counter
 *     This array contains the error counts for each error type as requested from
 *     the counterMask. The array indexes correspond to the mask bits one-to-one.
 */

typedef struct NV2080_CTRL_NVLINK_COUNTERS_V2_VALUES {
    NvBool overFlow;
    NV_DECLARE_ALIGNED(NvU64 value, 8);
} NV2080_CTRL_NVLINK_COUNTERS_V2_VALUES;

#define NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV_DECLARE_ALIGNED(NvU64 counterMask[NV2080_CTRL_NVLINK_COUNTER_MAX_GROUPS], 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_COUNTERS_V2_VALUES counter[NV2080_CTRL_NVLINK_MAX_ARR_SIZE][NV2080_CTRL_NVLINK_COUNTER_MAX_COUNTERS_PER_LINK_IN_REQ], 8);
} NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS;
#define NV2080_CTRL_CMD_NVLINK_GET_COUNTERS_V2 (0x20803050U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8 | NV2080_CTRL_NVLINK_GET_COUNTERS_V2_PARAMS_MESSAGE_ID)" */

/*
 * NV2080_CTRL_CMD_NVLINK_CLEAR_COUNTERS_V2
 *  This command clears/resets the counters for the specified types.
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *  This parameter specifies for which links we want to clear the
 *  counters.
 *
 * [in] links
 *  This parameter specifies for which links we want to clear the
 *  counters.
 *
 * [in] counterMask
 *  This parameter specifies the input mask for desired counters to be
 *  cleared. Note that all counters cannot be cleared.
*/

#define NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV_DECLARE_ALIGNED(NvU64 counterMask[NV2080_CTRL_NVLINK_COUNTER_MAX_GROUPS], 8);
} NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS;
#define NV2080_CTRL_CMD_NVLINK_CLEAR_COUNTERS_V2 (0x20803051U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8 | NV2080_CTRL_NVLINK_CLEAR_COUNTERS_V2_PARAMS_MESSAGE_ID)" */

/*
 * NV2080_CTRL_CMD_NVLINK_INJECT_ERROR
 *  This command causes all the same actions to occur as if the related
 *  error were to occur, either fatal or recoverable.
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *  Controls which links to apply error injection to.
 *
 * [in] links
 *  Controls which links to apply error injection to.
 *
 * [in] bFatal
 *  This parameter specifies that the error should be fatal.
 *
 */
#define NV2080_CTRL_CMD_NVLINK_INJECT_ERROR      (0x20803006U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_INJECT_ERROR_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_INJECT_ERROR_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_NVLINK_INJECT_ERROR_PARAMS {
    NvU32  linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NvBool bFatalError;
} NV2080_CTRL_NVLINK_INJECT_ERROR_PARAMS;

/* NVLINK unit list - to be used with error notifiers */
#define NV2080_CTRL_NVLINK_UNIT_DL       0x01U
#define NV2080_CTRL_NVLINK_UNIT_TL       0x02U
#define NV2080_CTRL_NVLINK_UNIT_TLC_RX_0 0x03U
#define NV2080_CTRL_NVLINK_UNIT_TLC_RX_1 0x04U
#define NV2080_CTRL_NVLINK_UNIT_TLC_TX_0 0x05U
#define NV2080_CTRL_NVLINK_UNIT_MIF_RX_0 0x06U
#define NV2080_CTRL_NVLINK_UNIT_MIF_TX_0 0x07U
#define NV2080_CTRL_NVLINK_UNIT_MINION   0x08U

/*
 * NV2080_CTRL_NVLINK_ERROR_INJECT_CFG
 *
 * [in] errType
 *  This parameter specifies the type of error injection settings
 * [in] errSettings
 *  This parameter specifies the settings for the error type in NVL5
 */

typedef enum NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE {
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE_TX_ERR = 1,
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE_PKT_ERR = 2,
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE_AUTH_TAG_ERR = 3,
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE_LINK_ERR = 4,
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE_MAX = 5,
} NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE;

typedef struct NV2080_CTRL_NVLINK_HW_ERROR_INJECT_CFG {
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_ERR_TYPE errType;
    NV_DECLARE_ALIGNED(NvU64 errSettings, 8);
} NV2080_CTRL_NVLINK_HW_ERROR_INJECT_CFG;

/*
 * Tx error type settings
 */
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_TYPE                                  31:28
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_TYPE_NO_ERROR                   0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_TYPE_RAW_BER                    0x00000001U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_TYPE_EFFECTIVE_BER              0x00000002U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_ERR_INJECT_DURATION                   27:12 // Error Injection Duration, in 10ms units.
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_BER_MANTISSA                           11:8
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_TX_ERR_BER_EXPONENT                            7:0

/*
 * Packet error type settings
 */
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_INJECT_COUNT                          15:0
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_STOMP                                16:16
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_STOMP_DIS                      0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_STOMP_EN                       0x00000001U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_POISON                               17:17
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_POISON_DIS                     0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_POISON_EN                      0x00000001U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_PKT_ERR_CLEAR_COUNTERS                       18:18

/*
 * Authentication error type settings
 */
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_AUTH_TAG_ERR_PIPE_INDEX                        3:0
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_AUTH_TAG_ERR_AUTH_ERR                          4:4
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_AUTH_TAG_ERR_AUTH_ERR_DIS              0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_AUTH_TAG_ERR_AUTH_ERR_EN               0x00000001U

/*
 * Link Error type settings
 */
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_LINK_ERR_FORCE_LINK_DOWN                       0:0
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_LINK_ERR_FORCE_LINK_DOWN_DIS           0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_LINK_ERR_FORCE_LINK_DOWN_EN            0x00000001U

/*
 * NV2080_CTRL_CMD_NVLINK_SET_HW_ERROR_INJECT
 *  This command causes all the same actions to occur as if the related
 *  error were to occur, either fatal or recoverable.
 *
 * [in] linkMask        size: 64 bits (This field will be deprecated in the future, please use links)
 *  Mask of the links to be configured.
 *
 * [in] links
 *  Mask of the links to be configured.
 *
 * [in] errCfg
 *  This parameter specifies that the error configurations.
 */

#define NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_HW_ERROR_INJECT_CFG errCfg[NV2080_CTRL_NVLINK_MAX_ARR_SIZE], 8);
} NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_SET_HW_ERROR_INJECT (0x20803081U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_HW_ERROR_INJECT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_LINK_STATE                             1:0
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_LINK_STATE_UP                  0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_LINK_STATE_DOWN_BY_REQUEST     0x00000001U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_LINK_STATE_DOWN_BY_HW_ERR      0x00000002U

#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_OPER_STS                               0:0
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_OPER_STS_NO_ERR_INJECT         0x00000000U
#define NV2080_CTRL_NVLINK_HW_ERROR_INJECT_STS_OPER_STS_PERFORMING_ERR_INJECT 0x00000001U

/*
 * NV2080_CTRL_NVLINK_HW_ERROR_INJECT_INFO
 *
 * [out] txErrInfo
 *  This info specifies the settings for Tx errs in NVL5
 * [out] packetErrSettings
 *  This info specifies the settings for Pkt errs in NVL5
 * [out] authErrSettings
 *  This info specifies the settings for NVLE errs in NVL5
 * [out] linkStatus
 *  This specifies the status of the link in NVL5
 * [out] errInjectStatus
 *  This specifies the status of error injection
 */

typedef struct NV2080_CTRL_NVLINK_HW_ERROR_INJECT_INFO {
    NvU32 txErrInfo;
    NvU32 packetErrInfo;
    NvU32 authErrInfo;
    NvU32 linkStatus;
    NvU32 errInjectStatus;
} NV2080_CTRL_NVLINK_HW_ERROR_INJECT_INFO;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_HW_ERROR_INJECT
 *  This command get all the current nvlink error config
 *
 * [in] linkMask        size: 64 bits (This field will be deprecated in the future, please use links)
 *  Mask of the links to be configured.
 *
 * [in] links
 *  Mask of the links to be configured.
 *
 * [in] errCfg
 *  This parameter specifies that the error configurations.
 */

#define NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS_MESSAGE_ID (0x82U)

typedef struct NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_HW_ERROR_INJECT_INFO errInfo[NV2080_CTRL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_HW_ERROR_INJECT  (0x20803082U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_HW_ERROR_INJECT_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES
 *  This command gets the number of successful error recoveries
 *
 * [in]  linkMask        size: 32 bits (This field will be deprecated in the future, please use links)
 *    This parameter controls which links to get recoveries for.
 *
 * [in]  links
 *    This parameter controls which links to get recoveries for.
 *
 * [out] numRecoveries
 *    This parameter specifies the number of successful per link error recoveries
 */
#define NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES (0x20803007U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES_PARAMS {
    NvU32 linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NvU32 numRecoveries[NV2080_CTRL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE
 *
 * This command queries the remote endpoint type of the link recorded at the
 * time the last error occurred on the link.
 *
 *   [in] linkId
 *     This parameter specifies the link to get the last remote endpoint type
 *     recorded for.
 *
 *   [out] remoteType
 *     This parameter returns the remote endpoint type of the link recorded at
 *     the time the last error occurred on the link. Possible values are:
 *       NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE
 *         The link is not connected to an active remote endpoint.
 *       NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU
 *         The remote endpoint of the link is a peer GPU.
 *       NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NPU
 *         The remote endpoint of the link is the host system (e.g., an NPU
 *         on IBM POWER platforms).
 *       NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_TEGRA
 *         The remote endpoint of the link a tegra device
 *
 * Possible return status values are:
 *   NV_OK
 *     If the remoteType parameter value is valid upon return.
 *   NV_ERR_INVALID_ARGUMENT
 *     If the linkId parameter does not specify a valid link.
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on this GPU or the remote endpoint type is
 *     not recorded in non-volatile storage.
 */
#define NV2080_CTRL_CMD_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE (0x20803008U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE_PARAMS {
    NvU32 linkId;
    NvU32 remoteType;
} NV2080_CTRL_NVLINK_GET_LINK_LAST_ERROR_REMOTE_TYPE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_LINK_FATAL_ERROR_COUNTS
 *
 * This command queries the number of each type of fatal errors that have
 * occurred on the given link.
 *
 *   [in] linkId
 *     This parameter specifies the link to get the fatal error information
 *     for.
 *
 *   [out] supportedCounts
 *     This parameter identifies which counts in the fatalErrorCounts array
 *     are valid for the given link. A bit set in this field means that the
 *     corresponding index is valid in the fatalErrorCounts array.
 *
 *   [out] fatalErrorCounts
 *     This parameter returns an array of 8-bit counts, one for each type of
 *     fatal error that can occur on the link. The valid indices of this array
 *     are:
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_RX_DL_DATA_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_RX_DL_CTRL_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_PROTOCOL
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_RX_RAM_DATA_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_RX_RAM_HDR_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_RESP
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_POISON
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DATA_POISONED_PKT_RCVD
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_TX_RAM_DATA_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL(C)_TX_RAM_HDR_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_TX_CREDIT
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_DL_FLOW_CTRL_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DL_FLOW_CTRL_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_DL_HDR_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_RECOVERY_LONG
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_RAM
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_INTERFACE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_SUBLINK_CHANGE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_RX_FAULT_SUBLINK_CHANGE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_RX_FAULT_DL_PROTOCOL
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_LTSSM_FAULT
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DL_HDR_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_AE_FLIT_RCVD
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_BE_FLIT_RCVD
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_ADDR_ALIGN
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_PKT_LEN
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CMD_ENC
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_DAT_LEN_ENC
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_ADDR_TYPE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_RSP_STATUS
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_PKT_STATUS
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_REQ
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_RESP
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_GT_ATOMIC_REQ_MAX_SIZE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_GT_RMW_REQ_MAX_SIZE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_LT_ATR_RESP_MIN_SIZE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_PO_FOR_CACHE_ATTR
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_COMPRESSED_RESP
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RESP_STATUS_TARGET
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RESP_STATUS_UNSUPPORTED_REQUEST
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_HDR_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DATA_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_STOMPED_PKT_RCVD
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_CORRECTABLE_INTERNAL
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_VC_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_NVLINK_CREDIT_RELEASE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_NCISOC_CREDIT_RELEASE
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_HDR_CREDIT_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DATA_CREDIT_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DL_REPLAY_CREDIT_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_UNSUPPORTED_VC_OVERFLOW
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_STOMPED_PKT_SENT
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DATA_POISONED_PKT_SENT
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RESP_STATUS_TARGET
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RESP_STATUS_UNSUPPORTED_REQUEST
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_RX_RAM_DATA_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_RX_RAM_HDR_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_TX_RAM_DATA_PARITY
 *       NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_TX_RAM_HDR_PARITY
 *
 * Possible return status values are:
 *   NV_OK
 *     If the values in the fatalErrorCounts array are valid upon return.
 *   NV_ERR_INVALID_ARGUMENT
 *     If the linkId parameter does not specify a valid link.
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on this GPU or aggregate NVLINK fatal error
 *     counts are not recorded in non-volatile storage.
 */
#define NV2080_CTRL_CMD_NVLINK_GET_LINK_FATAL_ERROR_COUNTS                           (0x20803009U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS_MESSAGE_ID" */

/*
 * NVLink 1 Fatal Error Types
 */
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_DL_DATA_PARITY                     0U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_DL_CTRL_PARITY                     1U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_PROTOCOL                           2U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_OVERFLOW                           3U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_RAM_DATA_PARITY                    4U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_RAM_HDR_PARITY                     5U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_RESP                               6U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_RX_POISON                             7U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_TX_RAM_DATA_PARITY                    8U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_TX_RAM_HDR_PARITY                     9U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_TX_CREDIT                             10U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_DL_FLOW_CTRL_PARITY                   11U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TL_DL_HDR_PARITY                         12U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_RECOVERY_LONG                      13U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_RAM                          14U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_INTERFACE                    15U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_TX_FAULT_SUBLINK_CHANGE               16U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_RX_FAULT_SUBLINK_CHANGE               17U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_RX_FAULT_DL_PROTOCOL                  18U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_LTSSM_FAULT                           19U

/*
 * NVLink 2 Fatal Error Types
 */
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DL_DATA_PARITY                    0U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DL_CTRL_PARITY                    1U
// No direct equivalent to:                 TL_RX_PROTOCOL                            2
// No direct equivalent to:                 TL_RX_OVERFLOW                            3
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RAM_DATA_PARITY                   4U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RAM_HDR_PARITY                    5U
// No direct equivalent to:                 TL_RX_RESP                                6
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DATA_POISONED_PKT_RCVD            7U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RAM_DATA_PARITY                   8U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RAM_HDR_PARITY                    9U
// No direct equivalent to:                 TL_TX_CREDIT                             10
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DL_FLOW_CONTROL_PARITY            11U
// No direct equivalent to:                 TL_DL_HDR_PARITY                         12
// Identical to NVLink 1:                   DL_TX_RECOVERY_LONG                      13
// Identical to NVLink 1:                   DL_TX_FAULT_RAM                          14
// Identical to NVLink 1:                   DL_TX_FAULT_INTERFACE                    15
// Identical to NVLink 1:                   DL_TX_FAULT_SUBLINK_CHANGE               16
// Identical to NVLink 1:                   DL_RX_FAULT_SUBLINK_CHANGE               17
// Identical to NVLink 1:                   DL_RX_FAULT_DL_PROTOCOL                  18
// Identical to NVLink 1:                   DL_LTSSM_FAULT                           19
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DL_HDR_PARITY                     20U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_AE_FLIT_RCVD              21U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_BE_FLIT_RCVD              22U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_ADDR_ALIGN                23U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_PKT_LEN                           24U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CMD_ENC                      25U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_DAT_LEN_ENC                  26U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_ADDR_TYPE                    27U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_RSP_STATUS                   28U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_PKT_STATUS                   29U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_REQ  30U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_RESP 31U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_GT_ATOMIC_REQ_MAX_SIZE    32U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_GT_RMW_REQ_MAX_SIZE       33U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DAT_LEN_LT_ATR_RESP_MIN_SIZE      34U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_PO_FOR_CACHE_ATTR         35U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_COMPRESSED_RESP           36U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RESP_STATUS_TARGET                37U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_RESP_STATUS_UNSUPPORTED_REQUEST   38U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_HDR_OVERFLOW                      39U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_DATA_OVERFLOW                     40U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_STOMPED_PKT_RCVD                  41U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_CORRECTABLE_INTERNAL              42U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_VC_OVERFLOW           43U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_NVLINK_CREDIT_RELEASE 44U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_UNSUPPORTED_NCISOC_CREDIT_RELEASE 45U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_HDR_CREDIT_OVERFLOW               46U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DATA_CREDIT_OVERFLOW              47U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DL_REPLAY_CREDIT_OVERFLOW         48U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_UNSUPPORTED_VC_OVERFLOW           49U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_STOMPED_PKT_SENT                  50U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_DATA_POISONED_PKT_SENT            51U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RESP_STATUS_TARGET                52U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_RESP_STATUS_UNSUPPORTED_REQUEST   53U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_RX_RAM_DATA_PARITY                   54U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_RX_RAM_HDR_PARITY                    55U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_TX_RAM_DATA_PARITY                   56U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_MIF_TX_RAM_HDR_PARITY                    57U

/*
 * NVLink 3 Fatal Error Types
 */
// Identical to NVLink 2:                   TLC_RX_DL_DATA_PARITY                     0
// Identical to NVLink 2:                   TLC_RX_DL_CTRL_PARITY                     1
// No direct equivalent to:                 TL_RX_PROTOCOL                            2
// No direct equivalent to:                 TL_RX_OVERFLOW                            3
// No direct equivalent to:                 TLC_RX_RAM_DATA_PARITY                    4
// No direct equivalent to:                 RX_RAM_HDR_PARITY                         5
// No direct equivalent to:                 TL_RX_RESP                                6
// No direct equivalent to:                 TLC_RX_DATA_POISONED_PKT_RCVD             7
// No direct equivalent to:                 TLC_TX_RAM_DATA_PARITY                    8
// No direct equivalent to:                 TLC_TX_RAM_HDR_PARITY                     9
// No direct equivalent to:                 TL_TX_CREDIT                             10
// Identical to NVLink 2:                   TLC_TX_DL_FLOW_CONTROL_PARITY            11
// No direct equivalent to:                 TL_DL_HDR_PARITY                         12
// No direct equivalent to:                 DL_TX_RECOVERY_LONG                      13
// Identical to NVLink 1:                   DL_TX_FAULT_RAM                          14
// Identical to NVLink 1:                   DL_TX_FAULT_INTERFACE                    15
// Identical to NVLink 1:                   DL_TX_FAULT_SUBLINK_CHANGE               16
// Identical to NVLink 1:                   DL_RX_FAULT_SUBLINK_CHANGE               17
// Identical to NVLink 1:                   DL_RX_FAULT_DL_PROTOCOL                  18
// No direct equivalent to:                 DL_LTSSM_FAULT                           19
// Identical to NVLink 2:                   TLC_RX_DL_HDR_PARITY                     20
// Identical to NVLink 2:                   TLC_RX_INVALID_AE_FLIT_RCVD              21
// Identical to NVLink 2:                   TLC_RX_INVALID_BE_FLIT_RCVD              22
// Identical to NVLink 2:                   TLC_RX_INVALID_ADDR_ALIGN                23
// Identical to NVLink 2:                   TLC_RX_PKT_LEN                           24
// Identical to NVLink 2:                   TLC_RX_RSVD_CMD_ENC                      25
// Identical to NVLink 2:                   TLC_RX_RSVD_DAT_LEN_ENC                  26
// No direct equivalent to:                 TLC_RX_RSVD_ADDR_TYPE                    27
// No direct equivalent to:                 TLC_RX_RSVD_RSP_STATUS                   28
// Identical to NVLink 2:                   TLC_RX_RSVD_PKT_STATUS                   29
// Identical to NVLink 2:                   TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_REQ  30
// Identical to NVLink 2:                   TLC_RX_RSVD_CACHE_ATTR_ENC_IN_PROBE_RESP 31
// No direct equivalent to:                 TLC_RX_DAT_LEN_GT_ATOMIC_REQ_MAX_SIZE    32
// Identical to NVLink 2:                   TLC_RX_DAT_LEN_GT_RMW_REQ_MAX_SIZE       33
// Identical to NVLink 2:                   TLC_RX_DAT_LEN_LT_ATR_RESP_MIN_SIZE      34
// Identical to NVLink 2:                   TLC_RX_INVALID_PO_FOR_CACHE_ATTR         35
// Identical to NVLink 2:                   TLC_RX_INVALID_COMPRESSED_RESP           36
// No direct equivalent to:                 TLC_RX_RESP_STATUS_TARGET                37
// No direct equivalent to:                 TLC_RX_RESP_STATUS_UNSUPPORTED_REQUEST   38
// Identical to NVLink 2:                   TLC_RX_HDR_OVERFLOW                      39
// Identical to NVLink 2:                   TLC_RX_DATA_OVERFLOW                     40
// Identical to NVLink 2:                   TLC_RX_STOMPED_PKT_RCVD                  41
// No direct equivalent to:                 TLC_RX_CORRECTABLE_INTERNAL              42
// No direct equivalent to:                 TLC_RX_UNSUPPORTED_VC_OVERFLOW           43
// No direct equivalent to:                 TLC_RX_UNSUPPORTED_NVLINK_CREDIT_RELEASE 44
// No direct equivalent to:                 TLC_RX_UNSUPPORTED_NCISOC_CREDIT_RELEASE 45
// No direct equivalent to:                 TLC_TX_HDR_CREDIT_OVERFLOW               46
// No direct equivalent to:                 TLC_TX_DATA_CREDIT_OVERFLOW              47
// No direct equivalent to:                 TLC_TX_DL_REPLAY_CREDIT_OVERFLOW         48
// No direct equivalent to:                 TLC_TX_UNSUPPORTED_VC_OVERFLOW           49
// No direct equivalent to:                 TLC_TX_STOMPED_PKT_SENT                  50
// No direct equivalent to:                 TLC_TX_DATA_POISONED_PKT_SENT            51
// No direct equivalent to:                 TLC_TX_RESP_STATUS_TARGET                52
// No direct equivalent to:                 TLC_TX_RESP_STATUS_UNSUPPORTED_REQUEST   53
// No direct equivalent to:                 MIF_RX_RAM_DATA_PARITY                   54
// No direct equivalent to:                 MIF_RX_RAM_HDR_PARITY                    55
// No direct equivalent to:                 MIF_TX_RAM_DATA_PARITY                   56
// No direct equivalent to:                 MIF_TX_RAM_HDR_PARITY                    57
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_RX_INVALID_COLLAPSED_RESPONSE        58U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_NCISOC_HDR_ECC_DBE                59U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_TLC_TX_NCISOC_PARITY                     60U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_LTSSM_FAULT_UP                        61U
#define NV2080_CTRL_NVLINK_FATAL_ERROR_TYPE_DL_LTSSM_FAULT_DOWN                      62U

#define NV2080_CTRL_NVLINK_NUM_FATAL_ERROR_TYPES                                     63U

#define NV2080_CTRL_NVLINK_IS_FATAL_ERROR_COUNT_VALID(count, supportedCounts)    \
    (!!((supportedCounts) & NVBIT64(count)))

#define NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS {
    NvU32 linkId;
    NV_DECLARE_ALIGNED(NvU64 supportedCounts, 8);
    NvU8  fatalErrorCounts[NV2080_CTRL_NVLINK_NUM_FATAL_ERROR_TYPES];
} NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_LINK_NONFATAL_ERROR_RATES
 *
 * This command queries recent non-fatal error rates for the given link.
 *
 * The error rates specify the maximum number of errors per minute recorded
 * for the given link within a 24-hour period for daily maximums or a 30-day
 * period for monthly maximums.
 *
 *   [in] linkId
 *     This parameter specifies the link to get the nonfatal error information
 *     for.
 *
 *   [out] numDailyMaxNonfatalErrorRates
 *     This parameter returns the number of valid nonfatal error rate entries
 *     in the dailyMaxNonfatalErrorRates parameter.
 *
 *   [out] dailyMaxNonfatalErrorRates
 *     This parameter returns maximum nonfatal error rate entries recorded
 *     over the last few 24-hour periods. For example, index 0 contains the
 *     maximum nonfatal error rate recorded in the current day, index 1
 *     contains the maximum nonfatal error rate recorded yesterday ago, etc.
 *
 *   [out] numMonthlyMaxNonfatalErrorRates
 *     This parameter returns the number of valid nonfatal error rate entries
 *     in the monthlyMaxNonfatalErrorRates parameter.
 *
 *   [out] monthlyMaxNonfatalErrorRates
 *     THis parameter returns maximum nonfatal error rate entries recorded
 *     over the last few 30-day periods. For example, index 0 contains the
 *     maximum nonfatal error rate recorded in the current month, index 1
 *     contains the maximum nonfatal error recorded last month, etc.
 *
 * Possible status values returned are:
 *   NV_OK
 *     If any nonfatal error rates are valid upon return.
 *   NV_ERR_INVALID_ARGUMENT
 *     If the linkId parameter does not specify a valid link.
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on this GPU or NVLINK nonfatal error rates
 *     are not recorded in non-volatile storage.
 */
#define NV2080_CTRL_CMD_NVLINK_GET_LINK_NONFATAL_ERROR_RATES (0x2080300aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LINK_NONFATAL_ERROR_RATES_PARAMS_MESSAGE_ID" */

typedef struct NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE {
    NvU32 errorsPerMinute;
    NvU32 timestamp;
} NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE;

#define NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE_ENTRIES 5U

#define NV2080_CTRL_NVLINK_GET_LINK_NONFATAL_ERROR_RATES_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_NVLINK_GET_LINK_NONFATAL_ERROR_RATES_PARAMS {
    NvU32                                  linkId;
    NvU32                                  numDailyMaxNonfatalErrorRates;
    NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE dailyMaxNonfatalErrorRates[NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE_ENTRIES];
    NvU32                                  numMonthlyMaxNonfatalErrorRates;
    NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE monthlyMaxNonfatalErrorRates[NV2080_CTRL_NVLINK_NONFATAL_ERROR_RATE_ENTRIES];
} NV2080_CTRL_NVLINK_GET_LINK_NONFATAL_ERROR_RATES_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_SET_ERROR_INJECTION_MODE
 *
 * This command sets the injection mode so that error handling and error
 * logging software can be aware that errors cropping up on links are
 * intentional and not due to HW failures.
 *
 *   [in] bEnabled
 *     This parameter specifies whether injection mode should be enabled or
 *     disabled.
 *
 * Possible status values returned are:
 *   NV_OK
 *     If injection mode is enabled or disabled according to the parameters.
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on this GPU.
 */
#define NV2080_CTRL_CMD_NVLINK_SET_ERROR_INJECTION_MODE (0x2080300bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_ERROR_INJECTION_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_SET_ERROR_INJECTION_MODE_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_NVLINK_SET_ERROR_INJECTION_MODE_PARAMS {
    NvBool bEnabled;
} NV2080_CTRL_NVLINK_SET_ERROR_INJECTION_MODE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_SETUP_EOM
 *
 * This command passes a packed 32bit params value to NV_PMINION_MISC_0_SCRATCH_SWRW_0
 * and then issues an EOM DLCMD to minion for the desired link. Only one DLCMD 
 * at a time can be issued to any given link.
 *
 * Params Packing is specified in Minion IAS
 */
#define NV2080_CTRL_CMD_NVLINK_SETUP_EOM (0x2080300cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS {
    NvU8  linkId;
    NvU32 params;
} NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_SET_POWER_STATE
 *
 * This command sets the mask of links associated with the GPU
 * to a target power state
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *     Mask of links that will be put to desired power state
 *     Note: In Turing RM supports only tansitions into/out of L2
 *
 * [in] links
 *     Mask of links that will be put to desired power state
 *     Note: In Turing RM supports only tansitions into/out of L2
 *
 * [in] powerState
 *     Target power state to which the links will transition
 *     This can be any one of NV2080_CTRL_NVLINK_POWER_STATE_* states
 *
 * Possible status values returned are:
 *   NV_OK
 *     If all links transitioned successfully to the target state
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip or if the power state
 *     is not enabled on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the any of the links in the mask is not enabled
 *   NV_ERR_INVALID_REQUEST
 *     If the power state transition is not supported
 *   NV_WARN_MORE_PROCESSING_REQUIRED
 *      Link has received the request for the power transition
 *      The transition will happen when the remote end also agrees
 *
 *  Note: Currently only L0->L2 and L2->L0 is supported
 */
#define NV2080_CTRL_CMD_NVLINK_SET_POWER_STATE (0x2080300dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_POWER_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_SET_POWER_STATE_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV2080_CTRL_NVLINK_SET_POWER_STATE_PARAMS {
    NvU32 linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NvU32 powerState;
} NV2080_CTRL_NVLINK_SET_POWER_STATE_PARAMS;

// NVLink Power States
#define NV2080_CTRL_NVLINK_POWER_STATE_L0      (0x00U)
#define NV2080_CTRL_NVLINK_POWER_STATE_L1      (0x01U)
#define NV2080_CTRL_NVLINK_POWER_STATE_L2      (0x02U)
#define NV2080_CTRL_NVLINK_POWER_STATE_L3      (0x03U)

/*
 * NV2080_CTRL_CMD_NVLINK_GET_POWER_STATE
 *
 * This command gets the power state of a link associated
 * with the GPU
 *
 * [in] linkId
 *     Link whose power state is being requested
 * [out] powerState
 *     Current power state of the link
 *     Is any one the NV2080_CTRL_NVLINK_POWER_STATE_* states
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the power state is retrieved successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU
 *   NV_ERR_INVALID_STATE
 *     If the link is in an invalid state
 */
#define NV2080_CTRL_CMD_NVLINK_GET_POWER_STATE (0x2080300eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS {
    NvU32 linkId;
    NvU32 powerState;
} NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_INJECT_TLC_ERROR
 *
 * This command injects TLC_*_REPORT_INJECT error. An RM interrupt
 * will be triggered after injection. Currently the injection call
 * only deals with HW_ERR, UR_ERR, PRIV_ERR in TX_SYS and RX_LNK devices
 *
 * [in] linkId
 *     Link whose power state is being requested.
 * [in] errorType
 *     error type that needs to be injected.
 * [in] device
 *     The device this injection is intended for.
 * [in] bBroadcast
 *     Whether the link report error should be fired in multiple links.

 * Possible status values returned are:
 *   NV_OK
 *     If the injection succeeds.
 *   NV_ERR_NOT_SUPPORTED
 *     If the error type of NVLINK is not supported on the chip
 */
#define NV2080_CTRL_CMD_NVLINK_INJECT_TLC_ERROR (0x2080300fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_DEVICE {
    TLC_RX_LNK = 0,
    TLC_TX_SYS = 1,
} NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_DEVICE;

typedef enum NV2080_CTRL_NVLINK_INJECT_TLC_TX_SYS_REPORT_ERROR_TYPE {
    TX_SYS_TX_RSP_STATUS_HW_ERR = 0,
    TX_SYS_TX_RSP_STATUS_UR_ERR = 1,
    TX_SYS_TX_RSP_STATUS_PRIV_ERR = 2,
} NV2080_CTRL_NVLINK_INJECT_TLC_TX_SYS_REPORT_ERROR_TYPE;

typedef enum NV2080_CTRL_NVLINK_INJECT_TLC_RX_LNK_REPORT_ERROR_TYPE {
    RX_LNK_RX_RSP_STATUS_HW_ERR = 0,
    RX_LNK_RX_RSP_STATUS_UR_ERR = 1,
    RX_LNK_RX_RSP_STATUS_PRIV_ERR = 2,
} NV2080_CTRL_NVLINK_INJECT_TLC_RX_LNK_REPORT_ERROR_TYPE;

typedef union NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_TYPE {
    NV2080_CTRL_NVLINK_INJECT_TLC_TX_SYS_REPORT_ERROR_TYPE txSysErrorType;
    NV2080_CTRL_NVLINK_INJECT_TLC_RX_LNK_REPORT_ERROR_TYPE rxLnkErrorType;
} NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_TYPE;


#define NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_PARAMS_MESSAGE_ID (0xFU)

typedef struct NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_PARAMS {
    NvU32                                      linkId;
    NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_DEVICE device;
    NvBool                                     bBroadcast;
    NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_TYPE   errorType;
} NV2080_CTRL_NVLINK_INJECT_TLC_ERROR_PARAMS;



/*
 * NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES
 *
 * This command returns the per-lane Figure Of Merit (FOM) Values from a link
 *
 * [in]  linkId
 *     The NVLink link ID to report FOM values for
 * [out] numLanes
 *     This field specifies the no. of lanes per link
 * [out] figureOfMeritValues
 *     This field contains the FOM values per lane
 *
 */
#define NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES (0x20803011U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_MAX_LANES               4U

#define NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS {
    NvU32 linkId;
    NvU8  numLanes;
    NvU16 figureOfMeritValues[NV2080_CTRL_NVLINK_MAX_LANES];
} NV2080_CTRL_CMD_NVLINK_GET_LINK_FOM_VALUES_PARAMS;

/*
 * NV2080_CTRL_NVLINK_SET_NVLINK_PEER
 *
 * This command sets/unsets the USE_NVLINK_PEER bit for a given
 *     mask of peers
 *
 * [in] peerMask
 *     Mask of Peer IDs for which USE_NVLINK_PEER needs to be updated
 * [in] bEnable
 *     Whether the bit needs to be set or unset
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the USE_NVLINK_PEER bit was updated successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip, or
 *     If unsetting USE_NVLINK_PEER bit is not supported
 *
 * NOTE: This is only supported on Windows
 *
 */
#define NV2080_CTRL_CMD_NVLINK_SET_NVLINK_PEER (0x20803012U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_NVLINK_PEER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_SET_NVLINK_PEER_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV2080_CTRL_NVLINK_SET_NVLINK_PEER_PARAMS {
    NvU32  peerMask;
    NvBool bEnable;
} NV2080_CTRL_NVLINK_SET_NVLINK_PEER_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_READ_UPHY_PAD_LANE_REG
 *
 * This command packs the lane and addr values into NV_PMINION_MISC_0_SCRATCH_SWRW_0
 * and then issues a READPADLANEREG DLCMD to minion for the desired link. Only one DLCMD 
 * at a time can be issued to any given link.
 * 
 * After this command completes it is necessary to read the appropriate
 * NV_PNVL_BR0_PAD_CTL_7_CFG_RDATA register to retrieve the results of the read
 * Only GV100 should read NV_PNVL_BR0_PAD_CTL_7_CFG_RDATA.
 * From TU102+ the ctrl the required data would be updated in phyConfigData.
 *
 * [in] linkId
 *     Link whose pad lane register is being read
 * [in] lane
 *     Lane whose pad lane register is being read
 * [in] addr
 *     Address of the pad lane register to read
 * [out] phyConfigData
 *     Provides phyconfigaddr and landid
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the minion command completed successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU or the lane is invalid
 *   NV_ERR_TIMEOUT
 *     If a timeout occurred waiting for minion response
 */
#define NV2080_CTRL_CMD_NVLINK_READ_UPHY_PAD_LANE_REG (0x20803013U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_READ_UPHY_PAD_LANE_REG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_READ_UPHY_PAD_LANE_REG_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_NVLINK_READ_UPHY_PAD_LANE_REG_PARAMS {
    NvU8  linkId;
    NvU8  lane;
    NvU16 addr;
    NvU32 phyConfigData;
} NV2080_CTRL_NVLINK_READ_UPHY_PAD_LANE_REG_PARAMS;

/*
 * Structure to store the ECC error data.
 * valid
 *     Is the lane valid or not
 * eccErrorValue
 *     Value of the Error.
 * overflowed
 *     If the error overflowed or not
 */
typedef struct NV2080_CTRL_NVLINK_LANE_ERROR {
    NvBool bValid;
    NvU32  eccErrorValue;
    NvBool overflowed;
} NV2080_CTRL_NVLINK_LANE_ERROR;

/*
 * Structure to store ECC error data for Links
 * errorLane array index corresponds to the lane number.
 *
 * errorLane[]
 *    Stores the ECC error data per lane.
 */
typedef struct NV2080_CTRL_NVLINK_LINK_ECC_ERROR {
    NV2080_CTRL_NVLINK_LANE_ERROR errorLane[NV2080_CTRL_NVLINK_MAX_LANES];
    NvU32                         eccDecFailed;
    NvBool                        eccDecFailedOverflowed;
} NV2080_CTRL_NVLINK_LINK_ECC_ERROR;

/*
 * NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS
 *
 * Control to get the values of ECC ERRORS
 *
 * Parameters:
 *    linkMask [IN] (This field will be deprecated in the future, please use links)
 *      Links on which the ECC error data requested
 *      A valid link/port mask returned by the port masks returned by
 *      NVSWITCH_GET_INFO
 *    links [IN]
 *      Links on which the ECC error data requested
 *      A valid link/port mask returned by the port masks returned by
 *      NVSWITCH_GET_INFO
 *    errorLink[] [OUT]
 *      Stores the ECC error related information for each link.
 *      errorLink array index corresponds to the link Number.   
 */

#define NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS {
    NvU32                             linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_LINK_ECC_ERROR errorLink[NV2080_CTRL_NVLINK_MAX_ARR_SIZE];
} NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS;


#define NV2080_CTRL_CMD_NVLINK_GET_NVLINK_ECC_ERRORS     (0x20803014U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_NVLINK_ECC_ERRORS_PARAMS_MESSAGE_ID" */

// Nvlink throughput counters reading data flits in TX
#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_DATA_TX 0U

// Nvlink throughput counters reading data flits in RX
#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_DATA_RX 1U

// Nvlink throughput counters reading all flits in TX
#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_RAW_TX  2U

// Nvlink throughput counters reading all flits in RX
#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_RAW_RX  3U

#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_MAX     4U

/*
 * NV2080_CTRL_CMD_NVLINK_READ_TP_COUNTERS
 *
 * Reads reserved monotonically increasing NVLINK throughput counters for given linkIds
 *
 * [in] counterMask
 *     Mask of counter types to be queried
 *     One of NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_* macros
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *     Mask of links to be queried
 * [in] links
 *     Mask of links to be queried
 * [out] value
 *     Throughput counter value returned
 *
 * Possible status values returned are:
 *   NV_OK
 *     If command completed successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If numLinks is out-of-range or requested link is inactive
 *
 * Note:
 * The following commands will be deprecated in favor of NV2080_CTRL_CMD_NVLINK_READ_TP_COUNTERS:
 *     NV90CC_CTRL_CMD_NVLINK_GET_COUNTERS
 *     NV2080_CTRL_CMD_NVLINK_GET_COUNTERS
 * Other commands that will be deprecated due to the change in design:
 *     NV90CC_CTRL_CMD_NVLINK_RESERVE_COUNTERS
 *     NV90CC_CTRL_CMD_NVLINK_RELEASE_COUNTERS
 *     NV90CC_CTRL_CMD_NVLINK_SET_COUNTERS_FROZEN
 *     NV90CC_CTRL_CMD_NVLINK_GET_TL_COUNTER_CFG
 *     NV90CC_CTRL_CMD_NVLINK_SET_TL_COUNTER_CFG
 *     NV90CC_CTRL_CMD_NVLINK_CLEAR_COUNTERS
 *
 * Also, note that there is no counter overflow handling for these calls.
 * These counters would be counting in flits and assuming 25GB/s bandwidth per link,
 * with traffic flowing continuously, it would take 174 years for overflow to happen.
 * It is reasonable to assume an overflow will not occur within the GPU operation,
 * given that the counters get reset at system reboot or GPU reset. Counters are 63-bit.
 */

typedef struct NV2080_CTRL_NVLINK_READ_TP_COUNTERS_VALUES {
    NV_DECLARE_ALIGNED(NvU64 value[NV2080_CTRL_NVLINK_READ_TP_COUNTERS_TYPE_MAX], 8);
} NV2080_CTRL_NVLINK_READ_TP_COUNTERS_VALUES;

#define NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS {
    NvU16 counterMask;
    NV_DECLARE_ALIGNED(NvU64 linkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_READ_TP_COUNTERS_VALUES counters[NV2080_CTRL_NVLINK_MAX_ARR_SIZE], 8);
} NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_READ_TP_COUNTERS      (0x20803015U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE
 *
 * This command locks the link power state so that RM doesn't modify the state
 * of the link during pstate switch.
 *
 *   [in] linkMask        Links for which power mode needs to be locked.
 */
#define NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE (0x20803016U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE_PARAMS {
    NvBool bLockPowerMode;
} NV2080_CTRL_CMD_NVLINK_LOCK_LINK_POWER_STATE_PARAMS;

#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_COUNT_TX_NVHS             0U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_COUNT_TX_EIGHTH           1U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_COUNT_TX_OTHER            2U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_NUM_TX_LP_ENTER           3U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_NUM_TX_LP_EXIT            4U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_COUNT_TX_SLEEP            5U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_HS_TIME                   6U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_LOCAL_FULL_BW_EXIT_TIME   7U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_LOCAL_LP_ENTRY_TIME       8U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_LOCAL_LP_EXIT_TIME        9U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_LOCAL_FULL_BW_ENTRY_TIME  10U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_REMOTE_FULL_BW_EXIT_TIME  11U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_REMOTE_LP_ENTRY_TIME      12U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_REMOTE_LP_EXIT_TIME       13U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_REMOTE_FULL_BW_ENTRY_TIME 14U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_OTHER_STATE_TIME          15U
#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_MAX_COUNTERS              16U

/*
 * NV2080_CTRL_CMD_NVLINK_GET_LP_COUNTERS
 *
 * Reads NVLINK low power counters for given linkId
 *
 * [in] linkId
 *     ID of the link to be queried
 * [in,out] counterValidMask
 *     Mask of valid counters
 * [out] counterValues
 *     Low power counter values returned
 *
 * Possible status values returned are:
 *   NV_OK
 *     If command completed successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If linkId is out-of-range or requested link is inactive
 */

#define NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS {
    NvU32 linkId;
    NvU32 counterValidMask;
    NvU32 counterValues[NV2080_CTRL_NVLINK_GET_LP_COUNTERS_MAX_COUNTERS];
} NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_LP_COUNTERS   (0x20803018U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_NVLINK_CLEAR_LP_COUNTERS (0x20803052U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8 | 0x52)" */


//
// Set the near end loopback mode using the following
// Currently, three modes - NEA, NEDR, NEW
//
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_DEFAULT              (0x00000000)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_NEA                  (0x00000001)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_NEDR                 (0x00000002)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_NEDW                 (0x00000003)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PHY_REMOTE           (0x00000004)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PHY_LOCAL            (0x00000005)
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_EXT_LOCAL            (0x00000006)

/*
 * NV2080_CTRL_CMD_NVLINK_SET_LOOPBACK_MODE
 *
 * Generic NvLink callback for MODS
 *
 * [In] linkdId
 *     ID of the link to be used
 * [In] loopbackMode
 *     This value will decide which loopback mode need to
 *     set on the specified link.
 *     Modes are NEA / NEDR / NEDW
 */
#define NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS {
    NvU32 linkId;
    NvU8  loopbackMode;
} NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_SET_LOOPBACK_MODE (0x20803023U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS_MESSAGE_ID" */

//
// Read Refresh counter - the pass/fail occurrences
//

typedef struct NV2080_CTRL_NVLINK_PHY_REFRESH_STATUS_INFO {
    // requested links or not
    NvBool bValid;

    // counters
    NvU16  passCount;
    NvU16  failCount;
} NV2080_CTRL_NVLINK_PHY_REFRESH_STATUS_INFO;

#define NV2080_CTRL_NVLINK_MAX_LINK_COUNT 32

/*
 * NV2080_CTRL_CMD_NVLINK_GET_REFRESH_COUNTERS
 *
 *
 * [In] linkMask (This field will be deprecated in the future, please use links)
 *     Specifies for which links we want to read the counters
 * [In] links
 *     Specifies for which links we want to read the counters
 * [Out] refreshCountPass
 *     Count of number of times PHY refresh pass
 * [Out] refreshCountFail
 *     Count of number of times PHY refresh fail
 */
#define NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS_MESSAGE_ID (0x28U)

typedef struct NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS {
    NvU32                                      linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_PHY_REFRESH_STATUS_INFO refreshCount[NV2080_CTRL_NVLINK_MAX_LINK_COUNT];
} NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_REFRESH_COUNTERS (0x20803028U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS_MESSAGE_ID" */

//
// Clear Refresh counter - the pass/fail occurrences
//

/*
 * NV2080_CTRL_CMD_NVLINK_CLEAR_REFRESH_COUNTERS
 *
 *
 * [In] linkMask (This field will be deprecated in the future, please use links)
 *     Specifies for which links we want to clear the counters
 * [In] links
 *     Specifies for which links we want to clear the counters
 */
#define NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS_MESSAGE_ID (0x29U)

typedef struct NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS {
    NvU32 linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
} NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_CLEAR_REFRESH_COUNTERS (0x20803029U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_GET_SET_NVSWITCH_FLA_ADDR
 *
 * Get/Set NVSwitch FLA address
 *
 * [In] bGet
 *     Whether to get or set the NVSwitch FLA address
 * [In/Out] addr
 *     Address that is to be set or retrieved.
 */
#define NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS_MESSAGE_ID (0x38U)

typedef struct NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS {
    NvBool bGet;
    NV_DECLARE_ALIGNED(NvU64 addr, 8);
} NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_SET_NVSWITCH_FLA_ADDR (0x20803038) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO
 *
 * Syncs the different link masks and vbios defined values between CPU-RM and GSP-RM
 *
 * [in]  discoveredLinks (This field will be deprecated in the future, please use discoveredLinkMask)
 *     Mask of links discovered from IOCTRLs
 *
 * [in]  discoveredLinkMask
 *     Mask of links discovered from IOCTRLs
 *
 * [in]  connectedLinksMask (This field will be deprecated in the future, please use connectedLinks)
 *     Mask of links which are connected (remote present)
 *
 * [in]  connectedLinks
 *     Mask of links which are connected (remote present)
 *
 * [in]  bridgeSensableLinks (This field will be deprecated in the future, please use bridgeSensableLinkMask)
 *     Mask of links whose remote endpoint presence can be sensed
 *
 * [in]  bridgeSensableLinkMask
 *     Mask of links whose remote endpoint presence can be sensed
 *
 * [in]  bridgedLinks (This field will be deprecated in the future, please use bridgedLinkMask)
 *    Mask of links which are connected (remote present)
 *    Same as connectedLinksMask, but also tracks the case where link
 *    is connected but marginal and could not initialize
 *
 * [in]  bridgedLinkMask
 *    Mask of links which are connected (remote present)
 *    Same as connectedLinks, but also tracks the case where link
 *    is connected but marginal and could not initialize
 *
 * [out] initDisabledLinksMask (This field will be deprecated in the future, please use initDisabledLinks)
 *      Mask of links for which initialization is disabled
 *
 * [out] initDisabledLinks
 *      Mask of links for which initialization is disabled
 *
 * [out] vbiosDisabledLinkMask (This field will be deprecated in the future, please use vbiosDisabledLinks)
 *      Mask of links disabled in the VBIOS
 *
 * [out] vbiosDisabledLinks
 *      Mask of links disabled in the VBIOS
 *
 * [out] initializedLinks (This field will be deprecated in the future, please use initializedLinkMask)
 *      Mask of initialized links
 *
 * [out] initializedLinkMask
 *      Mask of initialized links
 *
 * [out] bEnableTrainingAtLoad
 *      Whether the links should be trained to active during driver load
 *
 * [out] bEnableSafeModeAtLoad
 *      Whether the links should be initialized to swcfg during driver load
 */

#define NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 discoveredLinks, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK discoveredLinkMasks, 8);
    NvU32  connectedLinksMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK connectedLinks, 8);
    NV_DECLARE_ALIGNED(NvU64 bridgeSensableLinks, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK bridgeSensableLinkMasks, 8);
    NvU32  bridgedLinks;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK bridgedLinkMasks, 8);
    NvU32  initDisabledLinksMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK initDisabledLinks, 8);
    NV_DECLARE_ALIGNED(NvU64 vbiosDisabledLinkMask, 8);
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK vbiosDisabledLinks, 8);
    NvU32  initializedLinks;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK initializedLinkMasks, 8);
    NvBool bEnableTrainingAtLoad;
    NvBool bEnableSafeModeAtLoad;
} NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO (0x20803039U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_ENABLE_LINKS
 *
 * Enable pre-topology setup on the mask of enabled links
 * This command accepts no parameters.
 */

#define NV2080_CTRL_CMD_NVLINK_ENABLE_LINKS                   (0x2080303aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | 0x3a" */

/*
 * NV2080_CTRL_CMD_NVLINK_PROCESS_INIT_DISABLED_LINKS
 *
 * Process the init disabled NVLinks and filter those out
 *
 * [in/out]  initDisabledLinksMask (This field will be deprecated in the future, please use initDisabledLinks)
 *     Mask of links initdisabled on a given GPU
 *
 * [in/out]  initDisabledLinks
 *     Mask of links initdisabled on a given GPU
 *
 * [in] bSkipHwNvlinkDisable
 *     Whether to consider skipping the HW initdisable links
 */

#define NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS_MESSAGE_ID (0x3bU)

typedef struct NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS {
    NvU32  initDisabledLinksMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK initDisabledLinks, 8);
    NvBool bSkipHwNvlinkDisable;
} NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_PROCESS_INIT_DISABLED_LINKS (0x2080303bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_EOM_CONTROL
 *
 * cmd [IN] enum identifying the EOM related command for the driver to process
 * link [IN] linkId
 * params [IN] NvU32 word that is written into NV_PMINION_SCRATCH_SWRW_0 before calling CONFIGEOM dlcmd
 * measurements [OUT] output of EOM
 *
 * Params Packing is specified in Minion IAS
 */

typedef enum NV2080_CTRL_CMD_NVLINK_EOM_CONTROL_PARAMS_COMMAND {
    NVLINK_EOM_CONTROL_START_EOM = 0,
    NVLINK_EOM_CONTROL_END_EOM = 1,
    NVLINK_EOM_CONTROL_CONFIG_EOM = 2,
    NVLINK_EOM_CONTROL_FULL_EOM_SEQUENCE = 3,
} NV2080_CTRL_CMD_NVLINK_EOM_CONTROL_PARAMS_COMMAND;

typedef struct NV2080_CTRL_NVLINK_EOM_MEASUREMENT {
    NvU8 upper;
    NvU8 middle;
    NvU8 lower;
    NvU8 composite;
} NV2080_CTRL_NVLINK_EOM_MEASUREMENT;

#define NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS_MESSAGE_ID (0x3cU)

typedef struct NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS {
    NV2080_CTRL_CMD_NVLINK_EOM_CONTROL_PARAMS_COMMAND cmd;
    NvU32                                             linkId;
    NvU32                                             params;
    NV2080_CTRL_NVLINK_EOM_MEASUREMENT                measurements[NV2080_CTRL_NVLINK_MAX_LANES];
} NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_EOM_CONTROL      (0x2080303c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_EOM_CONTROL_PARAMS_MESSAGE_ID" */

/*!
 * Inband Received Data
 */
#define NV2080_CTRL_NVLINK_INBAND_MAX_DATA_SIZE 5120
#define NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS_MESSAGE_ID (0x3dU)

typedef struct NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS {
    NvU8  data[NV2080_CTRL_NVLINK_INBAND_MAX_DATA_SIZE];
    NvU32 dataSize;
} NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS;

#define NV2080_CTRL_CMD_READ_NVLINK_INBAND_RESPONSE (0x2080303d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_L1_THRESHOLD_VALUE_DEFAULT (0xFFFFFFFF)

/*
 * NV2080_CTRL_CMD_NVLINK_SET_L1_THRESHOLD
 *
 * This command is used to set the L1 threshold value.
 * A value of NV2080_CTRL_NVLINK_L1_THRESHOLD_VALUE_DEFAULT
 * will reset the L1 Threshold to the default values.
 *
 * [in] l1Threshold
 *     Used to set the L1 threshold value
 *
 * [in] l1ExitThreshold
 *     Used to set the L1 Exit threshold value
 */
#define NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS_MESSAGE_ID (0x3eU)

typedef struct NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS {
    NvU32 l1Threshold;
    NvU32 l1ExitThreshold;
} NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_SET_L1_THRESHOLD (0x2080303eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_L1_THRESHOLD_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_GET_L1_THRESHOLD
 *
 * This command is used to get the L1 threshold value
 *
 * [out] l1Threshold
 *     Used to get the L1 threshold value
 *
 * [out] l1ExitThreshold
 *     Used to get the L1 Exit Thrshold value
 */
#define NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS_MESSAGE_ID (0x3fU)

typedef struct NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS {
    NvU32 l1Threshold;
    NvU32 l1ExitThreshold;
} NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_L1_THRESHOLD (0x2080303fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_L1_THRESHOLD_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_INBAND_SEND_DATA
 *
 * RPC  for sending Inband data
 *
 * [In] data[]
 *     data to be sent over inband
 * [In] dataSize
 *     Size of valid data in data array
 */
#define NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS {
    NvU8  buffer[NV2080_CTRL_NVLINK_INBAND_MAX_DATA_SIZE];
    NvU32 dataSize;
} NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_INBAND_SEND_DATA (0x20803040U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_IS_GPU_DEGRADED
 *
 * RPC for Getting GPU degraded status upon link error
 *
 * [In] linkId
 *     Id of the link on which error occured
 * [Out] bIsGpuDegraded
 *     Boolean to track corresponding GPU is degraded or not
 */
#define NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS {
    NvU32  linkId;
    NvBool bIsGpuDegraded;
} NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_IS_GPU_DEGRADED (0x20803041U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_IS_GPU_DEGRADED_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_DIRECT_CONNECT_CHECK
 *
 * This command is used to check for missing
 * bridge/nvlink for direct connect GPU
 *
 * [out] bIsEnoughNvLink
 *     Check if number of active nvlink meet the minimum requirements.
 * [out] numBridge
 *     Number of NVLink bridge
 * [out] bridgePresenceMask
 *     Bit mask of NVLink bridges's presence
 */
#define NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS {
    NvBool bIsEnoughNvLink;
    NvU32  numBridge;
    NvU32  bridgePresenceMask;
} NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_DIRECT_CONNECT_CHECK (0x20803042U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_POST_FAULT_UP
 *
 * This command is to trigger the next sequence after the 10 sec delay
 *
 * [out] linkId
 *     Link number which the sequence should be triggered
 */
#define NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS {
    NvU32 linkId;
} NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_POST_FAULT_UP     (0x20803043U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_PORT_EVENT_COUNT_SIZE 64U

/*
* Structure to store port event information
*
*   portEventType
*       Type of port even that occurred: NVLINK_PORT_EVENT_TYPE*
*
*   gpuId
*       Gpu that port event occurred on
*
*   linkId
*       Link id that port event occurred on
*
*   time
*       Platform time (nsec) when event occurred
*/
typedef struct NV2080_CTRL_NVLINK_PORT_EVENT {
    NvU32 portEventType;
    NvU32 gpuId;
    NvU32 linkId;
    NV_DECLARE_ALIGNED(NvU64 time, 8);
} NV2080_CTRL_NVLINK_PORT_EVENT;

/*
* NV2080_CTRL_CMD_NVLINK_GET_PORT_EVENTS
*
* This command returns the port up and port down events that have occurred
*
* Parameters:
*
*   portEventIndex [IN/OUT]
*      On input: The index of the first port event at which to start reading out of the driver.
*
*      On output: The index of the first port event that wasn't reported through the 'port event' array
*                 in this call to NV2080_CTRL_CMD_NVLINK_GET_PORT_EVENTS. 
*
*   nextPortEventIndex[OUT]
*      The index that will be assigned to the next port event that occurs.
*      Users of the GET_PORT_EVENTS control call may set 'portEventIndex' to this field on initialization
*      to bypass port events that have already occurred without making multiple control calls.
*
*   portEventCount [OUT]
*      Number of port events returned by the call. Currently, portEventCount is limited
*      by NV2080_CTRL_NVLINK_PORT_EVENT_COUNT_SIZE. In order to query all the port events, a
*      client needs to keep calling the control till portEventCount is zero.
*
*   bOverflow [OUT]
*       True when the port event log is overflowed and no longer contains all the port
*       events that have occurred, false otherwise.
*
*   portEvent [OUT]
*      The port event entires.
*/
#define NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS_MESSAGE_ID (0x44U)

typedef struct NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 portEventIndex, 8);
    NV_DECLARE_ALIGNED(NvU64 nextPortEventIndex, 8);
    NvU32  portEventCount;
    NvBool bOverflow;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_PORT_EVENT portEvent[NV2080_CTRL_NVLINK_PORT_EVENT_COUNT_SIZE], 8);
} NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_PORT_EVENTS (0x20803044U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_PORT_EVENTS_PARAMS_MESSAGE_ID" */

/*
* NV2080_CTRL_CMD_NVLINK_CYCLE_LINK
*
* This command cycles a link by faulting it and then retraining the link
*
* Parameters:
*
*   linkId [IN]
*      The link id of the link to be cycled
*/
#define NV2080_CTRL_NVLINK_CYCLE_LINK_PARAMS_MESSAGE_ID (0x45U)

typedef struct NV2080_CTRL_NVLINK_CYCLE_LINK_PARAMS {
    NvU32 linkId;
} NV2080_CTRL_NVLINK_CYCLE_LINK_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_CYCLE_LINK (0x20803045U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_CYCLE_LINK_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_NVLINK_IS_REDUCED_CONFIG
 *
 * This command is to check if a GPU has a reduced nvlink configuration
 *
 * [out] bReducedNvlinkConfig
 *     Link number which the sequence should be triggered
 */
#define NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS_MESSAGE_ID (0x46U)

typedef struct NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS {
    NvBool bReducedNvlinkConfig;
} NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_IS_REDUCED_CONFIG (0x20803046U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS_MESSAGE_ID" */


#define NV2080_CTRL_NVLINK_PRM_ACCESS_MAX_LENGTH 496U

typedef struct NV2080_CTRL_NVLINK_PRM_DATA {
    NvU8 data[NV2080_CTRL_NVLINK_PRM_ACCESS_MAX_LENGTH];
} NV2080_CTRL_NVLINK_PRM_DATA;

#define NV2080_CTRL_CMD_NVLINK_GET_SUPPORTED_COUNTERS (0x20803074U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS_MESSAGE_ID (0x74U)

typedef struct NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 counterMask[NV2080_CTRL_NVLINK_COUNTER_MAX_GROUPS], 8);
} NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS;
/*
 * NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS
 *
 * This command returns platform-specific information related to the GPU's NVLink setup.
 *
 *   ibGuid
 *     Infiniband GUID reported by platform (for Blackwell, ibGuid is 8 bytes so indices 8-15 are zero)
 *   rackGuid
 *     GUID of the rack containing this GPU (for Blackwell rackGuid is 13 bytes so indices 13-15 are zero)
 *   chassisPhysicalSlotNumber
 *     The slot number in the rack containing this GPU (includes switches)
 *   computeSlotIndex
 *     The index within the compute slots in the rack containing this GPU (does not include switches)
 *   nodeIndex
 *     Index of the node within the slot containing this GPU
 *   peerType
 *     Platform indicated NVLink-peer type (e.g. switch present or not)
 *   moduleId
 *     ID of this GPU within the node
 */

#define NV2080_CTRL_CMD_NVLINK_GET_PLATFORM_INFO (0x20803083U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS_MESSAGE_ID (0x83U)

typedef struct NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS {
    NvU8 ibGuid[16];
    NvU8 rackGuid[16];
    NvU8 chassisPhysicalSlotNumber;
    NvU8 computeSlotIndex;
    NvU8 nodeIndex;
    NvU8 peerType;
    NvU8 moduleId;
    NvU8 nvlinkSignalingProtocol;
} NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS;



/*
 * Structure to store UPHY cmd data.
 * laneId
 *     Lane ID for specified link
 * address
 *     Desired address for read
 */
typedef struct NV2080_CTRL_NVLINK_UPHY_CLN_CMD {
    NvU8  pllIndex;
    NvU16 address;
} NV2080_CTRL_NVLINK_UPHY_CLN_CMD;

/*
 * NV2080_CTRL_CMD_NVLINK_READ_UPHY_CLN
 *
 *
 *  This command retrieves the land id cln select, lane id, and pll index.
 *
 * [in] linkMask (This field will be deprecated in the future, please use links)
 *     Mask of links whose uphy should be read
 * [in] links
 *     Mask of links whose uphy should be read
 * [in] uphyCmd
 *     Array of input data (pll index and address) for each link, 
 *      where index 0 represents link 0's pll index
 *      and index 16 represents link 16's pll index.
 * [out] data
 *     Data from uphy cln for each link where index 0 represents link 0's pll index
 *      and index 16 represents link 16's pll index.
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the minion command completed successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU or the lane is invalid
 *   NV_ERR_TIMEOUT
 *     If a timeout occurred waiting for minion response
 */
#define NV2080_CTRL_CMD_NVLINK_READ_UPHY_CLN (0x20803084U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_READ_UPHY_CLN_REG_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_READ_UPHY_CLN_REG_PARAMS_MESSAGE_ID (0x84U)

typedef struct NV2080_CTRL_NVLINK_READ_UPHY_CLN_REG_PARAMS {
    NvU32                           linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_UPHY_CLN_CMD uphyCmd[NV2080_CTRL_NVLINK_MAX_LINKS];
    NvU32                           data[NV2080_CTRL_NVLINK_MAX_LINKS];
} NV2080_CTRL_NVLINK_READ_UPHY_CLN_REG_PARAMS;

#define NV2080_CTRL_NVLINK_SUPPORTED_MAX_BW_MODE_COUNT 23U

/*
 * NV2080_CTRL_CMD_NVLINK_GET_SUPPORTED_BW_MODE
 *
 * This command gets the supported RBMs of the GPU
 *
 * [out] rbmModesList
 *     List of supported RBM modes
 * [out] rbmTotalModes
 *     Total RBM modes supported
 * Possible status values returned are: TODO: Update this
 *   NV_OK
 *     If the BW mode is retrieved successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU
 *   NV_ERR_INVALID_STATE
 *     If the link is in an invalid state
 */
#define NV2080_CTRL_CMD_NVLINK_GET_SUPPORTED_BW_MODE   (0x20803085U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS {
    NvU8 rbmModesList[NV2080_CTRL_NVLINK_SUPPORTED_MAX_BW_MODE_COUNT];
    NvU8 rbmTotalModes;
} NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_SET_BW_MODE
 *
 * This command sets the requested RBM of the GPU
 *
 * [in] rbmMode
 *     Requested RBM mode
 *
 * Possible status values returned are: TODO: Update this
 *   NV_OK
 *     If the BW mode is set successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU
 *   NV_ERR_INVALID_STATE
 *     If the link is in an invalid state
 */
#define NV2080_CTRL_CMD_NVLINK_SET_BW_MODE (0x20803086U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS_MESSAGE_ID (0x86U)

typedef struct NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS {
    NvU8 rbmMode;
} NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_BW_MODE
 *
 * This command gets the set RBM of the GPU
 *
 * [out] rbmMode
 *     RBM mode currently set
 *
 * Possible status values returned are: TODO: Update this
 *   NV_OK
 *     If the BW mode is set successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU
 *   NV_ERR_INVALID_STATE
 *     If the link is in an invalid state
 */
#define NV2080_CTRL_CMD_NVLINK_GET_BW_MODE (0x20803087U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS {
    NvU8 rbmMode;
} NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_LOCAL_DEVICE_INFO
 *
 *   localDeviceInfo
 *      NVLINK-relevant information about the device
 */

#define NV2080_CTRL_CMD_NVLINK_GET_LOCAL_DEVICE_INFO (0x20803088U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_DEVICE_INFO localDeviceInfo, 8);
} NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_INJECT_SW_ERROR
 *
 * This command is used to inject NVL5 ERROR_INJECT_V2 commands
 *
 * [in] links
 *          link mask of which links to inject the error on
 * [out]
 *     Error Types to be injected
 *
 * Possible status values returned are:
 *   NV_OK
 *     If the Error is injected successfully
 *   NV_ERR_NOT_SUPPORTED
 *     If NVLINK is not supported on the chip
 *   NV_ERR_INVALID_ARGUMENT
 *     If the link is not enabled on the GPU
 *   NV_ERR_INVALID_STATE
 *     If the link is in an invalid state
 */
#define NV2080_CTRL_CMD_NVLINK_INJECT_SW_ERROR (0x20803089U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS_MESSAGE_ID" */
typedef enum NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY {
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_NONFATAL = 0,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_APP_FATAL = 1,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_FATAL = 2,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_DEGRADATION = 3,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_WATCHDOG_TIMEOUT = 4,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_SAW_MVB_NON_FATAL = 5,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_SAW_MSE = 6,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_RLW_NON_FATAL = 7,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_RLW_FATAL = 8,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_RLW_PRIV_ERR = 9,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_TLW_NON_FATAL = 10,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_TLW_FATAL = 11,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_TREX_NON_FATAL = 12,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_NETIR_NON_FATAL = 13,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_NETIR_FATAL = 14,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_SAW_MVB_NON_FATAL = 15,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_SAW_MVB_FATAL = 16,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_RLW_NON_FATAL = 17,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_RLW_FATAL = 18,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_TLW_NON_FATAL = 19,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_ECC_INJECT_TLW_FATAL = 20,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MSE_GIN_SAW_MVB_FATAL = 21,
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY_MAX = 22,
} NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY;

#define NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS {
    NvU32                                       linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_INJECT_SW_ERROR_SEVERITY severity;
} NV2080_CTRL_NVLINK_INJECT_SW_ERROR_PARAMS;

/*
 * NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY
 *
 *  This command is used to update the NVLE topology in GSP RM
 *
 *  [in] localGpuAlid
 *      ALID of local GPU in P2P object
 *  [in] localGpuClid
 *      CLID of local GPU in P2P object
 *  [in] remoteGpuAlid
 *      ALID of remote GPU in P2P object
 *  [in] remoteGpuClid
 *      CLID of remote GPU in P2P object
 *  [out] flaRemapTabAddr
 *      FLA remap table addr for all GPU CLIDs
 *  [out] gpaRemapTabAddr
 *      SPA/GPA remap table addr for all GPU CLIDs
 */
#define NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY (0x2080308cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS_MESSAGE_ID (0x8cU)

typedef struct NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS {
    NvU32 localGpuAlid;
    NvU32 localGpuClid;
    NvU32 remoteGpuAlid;
    NvU32 remoteGpuClid;
    NvU32 flaRemapTabAddr[NV_MAX_DEVICES];
    NvU32 gpaRemapTabAddr[NV_MAX_DEVICES];
} NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS;

/*
 * Structure to store the NVLE ALIDs
 *
 *  [in/out] alid
 *      Represents the ALID
 *  [in/out] bValid
 *      Whether this ALID is a valid one  
 */
typedef struct NV2080_CTRL_NVLINK_NVLE_ALID {
    NvU32  alid;
    NvBool bValid;
} NV2080_CTRL_NVLINK_NVLE_ALID;

#define NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES 32U

/*
 * NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS
 *
 *  This command is used for the following operations.
 *    If getting the LID information in the remap table, the command gets
 *      a. ALID of the current GPU for which ctrl call is invoked
 *      b. All the ALIDs in the current GPU's remap table
 *    If setting the LID information in the remap table, the command sets
 *      a. All the clids corresponding to these ALIDs in the GPU's remap table
 *
 *  [in] bGet
 *      Whether to get the ALIDs or set the CLIDs in the remap table
 *  [in] bGpa
 *      Whether to select FLA or SPA/GPA remap table
 *  [out] alid
 *      ALID of the given GPU
 *  [in/out] alidList
 *      List of ALIDs that were retrieved, or list of ALIDs for which corresponding CLIDs needs to be programmed
 */
#define NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS    (0x2080308dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS_MESSAGE_ID (0x8dU)

typedef struct NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS {
    NvBool                       bGet;
    NvBool                       bGpa;
    NvU32                        alid;
    NV2080_CTRL_NVLINK_NVLE_ALID alidList[NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES];
} NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_POST_LAZY_ERROR_RECOVERY
 *
 * Signal to GSP that lazy error recovery can proceed.
 */

#define NV2080_CTRL_CMD_NVLINK_POST_LAZY_ERROR_RECOVERY (0x2080308aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | 0x8A" */

/*
 * NV2080_CTRL_CMD_SET_NVLINK_CONFIGURE_L1_TOGGLE
 *
 * This command configures the nvlink L1 toggle pattern
 *
 * Commands returns SUCCESS only when it successfully sets value of all
 * parameter in the list.
 *
 * Possible status return values are:
 *  NV_OK
 *
 * Reference:
 *
 */
#define NV2080_CTRL_CMD_NVLINK_CONFIGURE_L1_TOGGLE      (0x2080308eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE {
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE_DISABLED = 0,
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE_TRIGGER_ONCE = 1,
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE_FORCE_EXITED = 2,
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE_FORCE_ENTERED = 3,
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE_DUTY_CYCLE = 4,
} NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE;

typedef struct NV2080_CTRL_NVLINK_L1_FORCE_CONFIG {
    NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_MODE mode;
    NvU8                                        toggleActiveTime;
    NvU8                                        toggleInactiveTime;
    NvBool                                      bTrigger;
} NV2080_CTRL_NVLINK_L1_FORCE_CONFIG;

#define NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS_MESSAGE_ID (0x8EU)

typedef struct NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS {
    NvU32                              linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_L1_FORCE_CONFIG config;
} NV2080_CTRL_NVLINK_CONFIGURE_L1_TOGGLE_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_GET_L1_TOGGLE (0x2080308fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS_MESSAGE_ID (0x8FU)

typedef struct NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS {
    NvU32                              linkMask;
    NV_DECLARE_ALIGNED(NV2080_CTRL_NVLINK_LINK_MASK links, 8);
    NV2080_CTRL_NVLINK_L1_FORCE_CONFIG config[NV2080_CTRL_NVLINK_MAX_LINKS];
} NV2080_CTRL_NVLINK_GET_L1_TOGGLE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_GET_NVLE_ENCRYPT_EN_INFO
 *
 *  This command is used to get the ENCRYPT_EN register info
 *
 *  [out] bMmuNvlinkEncryptEn
 *      Boolean that shows if ENCRYPTION_ENABLED bit in MMU is enabled or not.
 *  [out] bNvlinkTlwEncryptEn
 *      Boolean that shows if ENCRYPT_EN bit in NVLPLW TLW is enabled or not.
 */

#define NV2080_CTRL_CMD_NVLINK_GET_NVLE_ENCRYPT_EN_INFO (0x2080308bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS_MESSAGE_ID (0x8bU)

typedef struct NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS {
    NvBool bMmuNvlinkEncryptEn;
    NvBool bNvlinkTlwEncryptEn;
} NV2080_CTRL_NVLINK_GET_NVLE_ENCRYPT_EN_INFO_PARAMS;

/*
 * NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO
 *
 *  This command is used to get the firmware version info
 *
 *  [out] firmwareVersion
 *      Array of firmware versions specifying their ucode type, and major/minor/subminor version
 *  [out] chipTypeArch
 *      Chip type arch
 *  [out] numValidEntries
 *      Number of valid entries in firmwareVersion array
 */

#define NV2080_CTRL_NVLINK_FIRMWARE_VERSION_LENGTH                (0x10U)
#define NV2080_CTRL_NVLINK_SEMANTIC_VERSION_UCODE_TYPE_MSE        (0x01U)
#define NV2080_CTRL_NVLINK_SEMANTIC_VERSION_UCODE_TYPE_NETIR      (0x02U)
#define NV2080_CTRL_NVLINK_SEMANTIC_VERSION_UCODE_TYPE_NETIR_UPHY (0x03U)
#define NV2080_CTRL_NVLINK_SEMANTIC_VERSION_UCODE_TYPE_NETIR_CLN  (0x04U)
#define NV2080_CTRL_NVLINK_SEMANTIC_VERSION_UCODE_TYPE_NETIR_DLN  (0x05U)

typedef struct NV2080_CTRL_NVLINK_SEMANTIC_VERSION {
    NvU8  ucodeType;
    NvU32 major;
    NvU32 minor;
    NvU32 subMinor;
} NV2080_CTRL_NVLINK_SEMANTIC_VERSION;

#define NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO (0x20803091U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS_MESSAGE_ID (0x91U)

typedef struct NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS {
    NV2080_CTRL_NVLINK_SEMANTIC_VERSION firmwareVersion[NV2080_CTRL_NVLINK_FIRMWARE_VERSION_LENGTH];
    NvU8                                chipTypeArch;
    NvU32                               numValidEntries;
} NV2080_CTRL_NVLINK_GET_FIRMWARE_VERSION_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_NVLINK_SET_NVLE_ENABLED_STATE
 *
 * This command is used to set the NVLE enablement status in GSP-RM
 *
 * [Out] bIsNvleEnabled
 *     Boolean to determine if Nvlink Encryption is enabled or not
 */

#define NV2080_CTRL_CMD_NVLINK_SET_NVLE_ENABLED_STATE (0x20803092U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS_MESSAGE_ID (0x92U)

typedef struct NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS {
    NvBool bIsNvleEnabled;
} NV2080_CTRL_NVLINK_SET_NVLE_ENABLED_STATE_PARAMS;

#define NV2080_CTRL_CMD_NVLINK_PRM_ACCESS (0x20803095U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS_MESSAGE_ID (0x95U)

typedef struct NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS {
    NV2080_CTRL_NVLINK_PRM_DATA prm;
} NV2080_CTRL_NVLINK_PRM_ACCESS_PARAMS;

#define NV2080_CTRL_NVLINK_FIXED_POINT_HISTOGRAM_MAX_SIZE 32U

typedef struct NV2080_CTRL_NVLINK_FIXED_POINT_HISTOGRAM {
    NvUFXP0_8 data[NV2080_CTRL_NVLINK_FIXED_POINT_HISTOGRAM_MAX_SIZE];
    NvU32     size;
} NV2080_CTRL_NVLINK_FIXED_POINT_HISTOGRAM;

/*
 * NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA
 *
 *  This command is used to retrieve persistent per-link metrics from beginning of life.
 *    This command is supported for NVL IP version 5.0
 *
 *  [in]  linkId
 *      link ID to query
 *  [out] globalMaxRawBERExp
 *      Largest lifetime RAW BER (exponent only)
 *  [out] globalMaxEffBERExp
 *      Largest lifetime EFF BER (exponent only)
 *  [out] linkDownCount
 *      Lifetime count of link down events (saturates)
 *  [out] downReasonHistogram
 *      Fixed-point histogram of unintentional link down reason codes with
 *      recency bias
 */

#define NV2080_CTRL_CMD_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA (0x20803097U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA_PARAMS_MESSAGE_ID (0x97U)

typedef struct NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA_PARAMS {
    NvU32                                    linkId;
    NvU8                                     globalMaxRawBERExp;
    NvU8                                     globalMaxEffBERExp;
    NvU32                                    linkDownCount;
    NV2080_CTRL_NVLINK_FIXED_POINT_HISTOGRAM downReasonHistogram;
} NV2080_CTRL_NVLINK_GET_LINK_ACCUMULATIVE_METRIC_DATA_PARAMS;

/*
 * NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA
 *
 *  This command is used to retrieve most recent persistent per-link metrics.
 *    This command is supported for NVL IP version 5.0
 *
 *  [in]  linkId
 *      link ID to query
 *  [out] initialRawBERExp
 *      Raw BER from most recent linkup (exponent only)
 *  [out] initialEffBERExp
 *      Eff BER from most recent linkup (exponent only)
 *  [out] lastDownReason
 *      Port down reason code from most recent linkdown
 *  [out] uptime
 *      Total time port has been up since most recent linkup (floating point msec)
 *  [out] localMaxRawBERExp
 *      Largest Raw BER reading since most recent linkup (exponent only)
 *  [out] localMaxEffBERExp
 *      Largest Eff BER reading since most recent linkup (exponent only)
 *  [out] currEffBERMonitor
 *      Current Eff BER monitor from most recent linkup
 *  [out] plrXmitRetryWithinTSecMaxLo
 *      PLR retransmissions in time window
 *  [out] totalSuccessfulRecoveryEvents
 *      Total number of times the Port Training state machine has
 *        successfullycompleted the link error recovery process
 */
#define NV2080_CTRL_CMD_NVLINK_GET_LINK_RECORD_METRIC_DATA (0x20803098U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA_PARAMS_MESSAGE_ID (0x98U)

typedef struct NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA_PARAMS {
    NvU32 linkId;
    NvU8  initialRawBERExp;
    NvU8  initialEffBERExp;
    NvU8  lastDownReason;
    NvU32 uptime;
    NvU8  localMaxRawBERExp;
    NvU8  localMaxEffBERExp;
    NvU16 currEffBERMonitor;
    NvU32 plrXmitRetryWithinTSecMaxLo;
    NvU32 totalSuccessfulRecoveryEvents;
} NV2080_CTRL_NVLINK_GET_LINK_RECORD_METRIC_DATA_PARAMS;

/*
 * NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA
 *
 *  This command is used to retrieve most recent persistent per-device metrics.
 *    This command is supported for NVL IP version 5.0
 *
 *  [out] MFDE
 *      PRM payload of most recent NETIR_MFDE_EVENT error
 */

#define NV2080_CTRL_CMD_NVLINK_GET_DEVICE_RECORD_METRIC_DATA (0x20803099U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA_PARAMS_MESSAGE_ID (0x99U)

typedef struct NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA_PARAMS {
    NV2080_CTRL_NVLINK_PRM_DATA MFDE;
} NV2080_CTRL_NVLINK_GET_DEVICE_RECORD_METRIC_DATA_PARAMS;


/*
 * NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME
 * NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME
 *
 * These commands store/retrieve a 64 char null-terminated ASCII string which is
 * treated by the NVLINK firmware as the node's hostname for fabric reporting
 * purposes.
 */
typedef struct NV2080_CTRL_NVLINK_NODE_HOSTNAME {
    NvU8 name[64];
} NV2080_CTRL_NVLINK_NODE_HOSTNAME;

#define NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS_MESSAGE_ID (0x9AU)

typedef struct NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS {
    NV2080_CTRL_NVLINK_NODE_HOSTNAME hostname;
} NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS;
#define NV2080_CTRL_CMD_NVLINK_SAVE_NODE_HOSTNAME (0x2080309aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_SAVE_NODE_HOSTNAME_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS_MESSAGE_ID (0x9BU)

typedef struct NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS {
    NV2080_CTRL_NVLINK_NODE_HOSTNAME hostname;
} NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS;
#define NV2080_CTRL_CMD_NVLINK_GET_SAVED_NODE_HOSTNAME (0x2080309bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVLINK_INTERFACE_ID << 8) | NV2080_CTRL_NVLINK_GET_SAVED_NODE_HOSTNAME_PARAMS_MESSAGE_ID" */


/* _ctrl2080nvlink_h_ */

