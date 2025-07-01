
#ifndef _G_CHIPSET_NVOC_H_
#define _G_CHIPSET_NVOC_H_

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
#include "g_chipset_nvoc.h"

#ifndef CHIPSET_H
#define CHIPSET_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Defines and structures used for the Core Logic Object.              *
*                                                                           *
\***************************************************************************/

#include "platform/hwbc.h"

// forward declare PcieAerCapability struct
struct PcieAerCapability;

// forward declaration of PexL1SubstateCapability
struct PexL1SubstateCapability;

// PCIe Enhanced Configuration space structure
typedef struct PCIECONFIGSPACEBASE PCIECONFIGSPACEBASE;
typedef struct PCIECONFIGSPACEBASE *PPCIECONFIGSPACEBASE;
struct PCIECONFIGSPACEBASE
{
    RmPhysAddr baseAddress;
    NvU32 domain;
    NvU8 startBusNumber;
    NvU8 endBusNumber;
    PPCIECONFIGSPACEBASE next;
};

// Seen in both nvagp.c and elsewhere
#define PCI_MAX_DOMAINS         65536
#define PCI_MAX_BUSES           256
#define PCI_MAX_DEVICES         32
#define PCI_CLASS_DISPLAY_DEV   0x03
#define PCI_COMMAND             0x04
#define PCI_BASE_ADDRESS_0      0x10   /* Aperture Base */
#define PCI_BASE_ADDRESS_0_VALID_MASK     0xFFFFFFF0


#define PCI_VENDOR_ID_AMD       0x1022
#define PCI_VENDOR_ID_ALI       0x10B9
#define PCI_VENDOR_ID_NVIDIA    0x10DE

#define CL_MAX_LINK_WIDTH(p)    ((p & 0x3f0) >> 4)  // Max Link width is 9:4
// CL_IS_ROOT_PORT() returns NV_TRUE if root port of PCI-E Root Complex
// Device/Port type is 23:20
// Root port is 0100b
#define CL_IS_ROOT_PORT(p)       (((p & 0xf00000) >> 20) == 0x4)
#define CL_IS_UPSTREAM_PORT(p)   (((p & 0xf00000) >> 20) == 0x5)
#define CL_IS_DOWNSTREAM_PORT(p) (((p & 0xf00000) >> 20) == 0x6)
#define CL_AVG_LINK_WIDTH       8
#define CL_DAGWOOD_LINK_WIDTH   8
#define CL_SINGLE_DW_LINK_WIDTH 8

//
// Offset arguments to Pcie[Read|WriteRoot]PortConfigReg
//

// PCI Express capability
#define CL_PCIE_BEGIN                               0x0100
#define CL_PCIE_CAP                                 (CL_PCIE_BEGIN + 0x00)
#define CL_PCIE_DEV_CAP                             (CL_PCIE_BEGIN + 0x04)
#define CL_PCIE_DEV_CTRL_STATUS                     (CL_PCIE_BEGIN + 0x08)
#define CL_PCIE_LINK_CAP                            (CL_PCIE_BEGIN + 0x0C)
#define CL_PCIE_LINK_CTRL_STATUS                    (CL_PCIE_BEGIN + 0x10)
#define CL_PCIE_SLOT_CAP                            (CL_PCIE_BEGIN + 0x14)
#define CL_PCIE_SLOT_CTRL_STATUS                    (CL_PCIE_BEGIN + 0x18)
#define CL_PCIE_ROOT_CTRL_RSVDP                     (CL_PCIE_BEGIN + 0x1C)
#define CL_PCIE_ROOT_STATUS                         (CL_PCIE_BEGIN + 0x20)
#define CL_PCIE_DEV_CAP_2                           (CL_PCIE_BEGIN + 0x24)
#define CL_PCIE_DEV_CTRL_2                          (CL_PCIE_BEGIN + 0x28)
#define CL_PCIE_LINK_CAP_2                          (CL_PCIE_BEGIN + 0x2C)
#define CL_PCIE_END                                 CL_PCIE_LINK_CAP_2

// PCI Express Capabilities
#define CL_PCIE_CAP_SLOT                            NVBIT(24)

// PCI Express Link Control ASPM Control Bits
#define CL_PCIE_LINK_CTRL_STATUS_ASPM_L0S_BIT       NVBIT(0)
#define CL_PCIE_LINK_CTRL_STATUS_ASPM_L1_BIT        NVBIT(1)
#define CL_PCIE_LINK_CTRL_STATUS_ASPM_MASK \
    (CL_PCIE_LINK_CTRL_STATUS_ASPM_L0S_BIT|CL_PCIE_LINK_CTRL_STATUS_ASPM_L1_BIT)

// PCI Express Link control ASPM capability Bits
#define CL_PCIE_LINK_CAP_ASPM_L0S_BIT              NVBIT(10)
#define CL_PCIE_LINK_CAP_ASPM_L1_BIT               NVBIT(11)
#define CL_PCIE_LINK_CAP_CLOCK_PM_BIT              NVBIT(18)

// PCI Express Slot Capabilities
#define CL_PCIE_SLOT_CAP_HOTPLUG_SURPRISE           NVBIT(5)
#define CL_PCIE_SLOT_CAP_HOTPLUG_CAPABLE            NVBIT(6)

//
// CL_DEVICE_CONTROL_STATUS bits for hal
//
// From PCI-E manual
#define CL_PCIE_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED          NVBIT(16)
#define CL_PCIE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED     NVBIT(17)
#define CL_PCIE_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED         NVBIT(18)
#define CL_PCIE_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED      NVBIT(19)

// PCI Express Latency Tolerance Reporting Capability Bit
#define CL_PCIE_DEV_CAP_2_LTR_SUPPORTED_BIT         NVBIT(11)
#define CL_PCIE_DEV_CTRL_2_LTR_ENABLED_BIT          NVBIT(10)

// Advanced Error Reporting capability
#define CL_AER_BEGIN                                0x0200
#define CL_AER_CAP                                  (CL_AER_BEGIN + 0x00)
#define CL_AER_UNCORRECTABLE_STATUS                 (CL_AER_BEGIN + 0x04)
#define CL_AER_UNCORRECTABLE_MASK                   (CL_AER_BEGIN + 0x08)
#define CL_AER_UNCORRECTABLE_SEVERITY               (CL_AER_BEGIN + 0x0C)
#define CL_AER_CORRECTABLE_STATUS                   (CL_AER_BEGIN + 0x10)
#define CL_AER_CORRECTABLE_MASK                     (CL_AER_BEGIN + 0x14)
#define CL_AER_ADVANCED_CAP_CONTROL                 (CL_AER_BEGIN + 0x18)
#define CL_AER_HEADER_LOG                           (CL_AER_BEGIN + 0x1C)
#define CL_AER_ROOT_ERROR_COMMAND                   (CL_AER_BEGIN + 0x2C)
#define CL_AER_ROOT_ERROR_STATUS                    (CL_AER_BEGIN + 0x30)
#define CL_AER_ERROR_SOURCE                         (CL_AER_BEGIN + 0x34)
#define CL_AER_END                                  (CL_AER_BEGIN + 0x34)

// Advanced Error Reporting Root Error Status ERR_COR Subclass Capable Mask
#define CL_AER_ROOT_ERROR_STATUS_ERR_COR_SUBCLASS_MASK        (NVBIT32(7) | NVBIT32(8))

// PCI Express Device Capabilities 2
#define CL_PCIE_DEV_CAP_2_ATOMICS_SUPPORTED_BIT     NVBIT(6)
#define CL_PCIE_DEV_CAP_2_ATOMIC_32BIT              NVBIT(7)
#define CL_PCIE_DEV_CAP_2_ATOMIC_64BIT              NVBIT(8)
#define CL_PCIE_DEV_CAP_2_ATOMIC_128BIT             NVBIT(9)

// PCI Express Device Control & Status 2
#define CL_PCIE_DEV_CTRL_2_ATOMICS_EGRESS_BLOCK_BIT NVBIT(7)

// Defines for Atomic Mask
#define CL_ATOMIC_32BIT                             NVBIT(0)
#define CL_ATOMIC_64BIT                             NVBIT(1)
#define CL_ATOMIC_128BIT                            NVBIT(2)

#define CL_IS_L0_SUPPORTED(p) (((p) & CL_PCIE_LINK_CAP_ASPM_L0S_BIT))
#define CL_IS_L1_SUPPORTED(p) (((p) & CL_PCIE_LINK_CAP_ASPM_L1_BIT))

#define CL_IS_LTR_PORT_SUPPORTED(p)    (((p) & CL_PCIE_DEV_CAP_2_LTR_SUPPORTED_BIT))
#define CL_IS_LTR_PORT_ENABLED(p)      (((p) & CL_PCIE_DEV_CTRL_2_LTR_ENABLED_BIT))

#define CL_IS_ATOMICS_SUPPORTED(p)        (((p) & CL_PCIE_DEV_CAP_2_ATOMICS_SUPPORTED_BIT))
#define CL_IS_ATOMICS_EGRESS_BLOCKED(p)   (((p) & CL_PCIE_DEV_CTRL_2_ATOMICS_EGRESS_BLOCK_BIT))

#define CL_IS_32BIT_ATOMICS_SUPPORTED(p)  (((p) & CL_PCIE_DEV_CAP_2_ATOMIC_32BIT))
#define CL_IS_64BIT_ATOMICS_SUPPORTED(p)  (((p) & CL_PCIE_DEV_CAP_2_ATOMIC_64BIT))
#define CL_IS_128BIT_ATOMICS_SUPPORTED(p) (((p) & CL_PCIE_DEV_CAP_2_ATOMIC_128BIT))

//
// This defines PCI-E Advanced Error Reporting Capability structure per PCI-E manual
// (refer to section 7.10 of PCI Express Base Specification, v1.1)
//
typedef struct PcieAerCapability
{
    NvU32       PexEnhCapHeader;            // (+0x00) PCI-E Enhanced Capability Header
    NvU32       UncorrErrStatusReg;         // (+0x04) Uncorrectable Error Status Register
    NvU32       UncorrErrMaskReg;           // (+0x08) Uncorrectable Error Mask Register
    NvU32       UncorrErrSeverityReg;       // (+0x0C) Uncorrectable Error Severity Register
    NvU32       CorrErrStatusReg;           // (+0x10) Correctable Error Status Register
    NvU32       CorrErrMaskReg;             // (+0x14) Correctable Error Mask Register
    NvU32       AEcapCrtlReg;               // (+0x18) Advanced Error Capability and Control Register
    struct {                                // (+0x1C) Header Log Register
        NvU32   Header[4];                  // (+0x1C-0x2B)
    } HeaderLogReg;
    NvU32       RootErrCmd;                 // (+0x2C) Root Error Command
    NvU32       RooErrStatus;               // (+0x30) Root Error Status
    NvU32       ErrSrcReg;                  // (+0x34) Error Source Register (Correctable Err Src Id + Err Src Id)
} PcieAerCapability, *PPcieAerCapability;

// Virtual Channel Capability
#define CL_VC_BEGIN                                 0x0300
#define CL_VC_RESOURCE_CTRL_0                       (CL_VC_BEGIN + 0x14)
#define CL_VC_END                                   (CL_VC_BEGIN + 0x1C)

typedef struct
{
    NvU16               deviceID;           // deviceID
    NvU16               vendorID;           // vendorID
    NvU16               subdeviceID;        // subsystem deviceID
    NvU16               subvendorID;        // subsystem vendorID
    NvU8                revisionID;         // revision ID
} BUSINFO;

// L1 PM substates Capability
#define CL_L1_SS_BEGIN                              0x0400
#define CL_L1_SS_CAP_HDR                            (CL_L1_SS_BEGIN + 0x00)
#define CL_L1_SS_CAP_REG                            (CL_L1_SS_BEGIN + 0x04)
#define CL_L1_SS_CTRL1_REG                          (CL_L1_SS_BEGIN + 0x08)
#define CL_L1_SS_CTRL2_REG                          (CL_L1_SS_BEGIN + 0x0C)
#define CL_L1_SS_END                                CL_L1_SS_CTRL2_REG

// ACS redirect Configuration
#define CL_ACS_BEGIN                                0x0500
#define CL_ACS_CAP                                 (CL_ACS_BEGIN + 0x04)
#define CL_ACS_CTRL                                (CL_ACS_BEGIN + 0x06)
#define CL_ACS_EGRESS_CTL_V                        (CL_ACS_BEGIN + 0x08)
#define CL_ACS_END                                 CL_ACS_EGRESS_CTL_V

#define CL_ACS_SV                                  NVBIT32(0)      // Source Validation
#define CL_ACS_TB                                  NVBIT32(1)      // Translation Blocking
#define CL_ACS_RR                                  NVBIT32(2)      // P2P Req Redirect
#define CL_ACS_CR                                  NVBIT32(3)      // P2P Cpl Redirect
#define CL_ACS_UF                                  NVBIT32(4)      // Upstream forwarding
#define CL_ACS_EC                                  NVBIT32(5)      // P2P Egress Ctrl
#define CL_ACS_DT                                  NVBIT32(6)      // Direct Translated P2P
//
// This defines PCI-E L1 PM Substates Extended Capability structure per PCI-E manual
// (refer to section 7.xx of ECN_L1_PM_Substates_with_CLKREQ_31_May_2013_Rev10a.pdf
//
typedef struct PexL1SubstateCapability
{
    NvU32       PexEnhCapHeader;            // (+0x00) PCI-E Enhanced Capability Header
    NvU32       Capabilities;               // (+0x04) L1 PM Substates capabilities Register
    NvU32       Control1Reg;                // (+0x08) L1 PM Substates Control1 Register
    NvU32       Control2Reg;                // (+0x0C) L1 PM Substates Control2 Register
} PexL1SubstateCapability, *PPexL1SubstateCapability;

typedef struct BUSTOPOLOGYINFO BUSTOPOLOGYINFO;
typedef struct BUSTOPOLOGYINFO *PBUSTOPOLOGYINFO;
struct BUSTOPOLOGYINFO
{
    PBUSTOPOLOGYINFO next;
    void *handle;
    BUSINFO busInfo;
    NvU32 domain;
    NvU8 bus, device, func, secBus;
    NvU16 pciSubBaseClass;
    NvBool bVgaAdapter;
};

typedef struct GspSystemInfo GspSystemInfo;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CHIPSET_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJCL;
struct NVOC_METADATA__Object;


struct OBJCL {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJCL *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJCL *__nvoc_pbase_OBJCL;    // cl

    // 37 PDB properties
    NvBool PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE;
    NvBool PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL;
    NvBool PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE;
    NvBool PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE;
    NvBool PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET;
    NvBool PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE;
    NvBool PDB_PROP_CL_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390;
    NvBool PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST;
    NvBool PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED;
    NvBool PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED;
    NvBool PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY;
    NvBool PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY;
    NvBool PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED;
    NvBool PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED;
    NvBool PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE;
    NvBool PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE;
    NvBool PDB_PROP_CL_NOSNOOP_NOT_CAPABLE;
    NvBool PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE;
    NvBool PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE;
    NvBool PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED;
    NvBool PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR;
    NvBool PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR;
    NvBool PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ;
    NvBool PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG;
    NvBool PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU;
    NvBool PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR;
    NvBool PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED;
    NvBool PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3;
    NvBool PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG;
    NvBool PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY;
    NvBool PDB_PROP_CL_UNSUPPORTED_CHIPSET;
    NvBool PDB_PROP_CL_IS_CHIPSET_IO_COHERENT;
    NvBool PDB_PROP_CL_DISABLE_IOMAP_WC;
    NvBool PDB_PROP_CL_WAR_AMD_5107271;
    NvBool PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE;
    NvBool PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR;
    NvBool PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS;

    // Data members
    NBADDR NBAddr;
    NvBool EnteredRecoverySinceErrorsLastChecked;
    struct OBJHWBC *pHWBC;
    NvU32 br04HwbcCount;
    NBADDR FHBAddr;
    BUSINFO FHBBusInfo;
    NvU32 Chipset;
    NvU32 ChipsetSliBondType;
    NvBool ChipsetInitialized;
    PPCIECONFIGSPACEBASE pPcieConfigSpaceBase;
    NBADDR chipsetIDBusAddr;
    BUSINFO chipsetIDInfo;
    PBUSTOPOLOGYINFO pBusTopologyInfo;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJCL {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_OBJCL_TYPEDEF__
#define __NVOC_CLASS_OBJCL_TYPEDEF__
typedef struct OBJCL OBJCL;
#endif /* __NVOC_CLASS_OBJCL_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJCL
#define __nvoc_class_id_OBJCL 0x547dbb
#endif /* __nvoc_class_id_OBJCL */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJCL;

#define __staticCast_OBJCL(pThis) \
    ((pThis)->__nvoc_pbase_OBJCL)

#ifdef __nvoc_chipset_h_disabled
#define __dynamicCast_OBJCL(pThis) ((OBJCL*) NULL)
#else //__nvoc_chipset_h_disabled
#define __dynamicCast_OBJCL(pThis) \
    ((OBJCL*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJCL)))
#endif //__nvoc_chipset_h_disabled

// Property macros
#define PDB_PROP_CL_WAR_AMD_5107271_BASE_CAST
#define PDB_PROP_CL_WAR_AMD_5107271_BASE_NAME PDB_PROP_CL_WAR_AMD_5107271
#define PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ_BASE_CAST
#define PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ_BASE_NAME PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ
#define PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE_BASE_CAST
#define PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE_BASE_NAME PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE
#define PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE_BASE_CAST
#define PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE_BASE_NAME PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE
#define PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED_BASE_CAST
#define PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED_BASE_NAME PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED
#define PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS_BASE_CAST
#define PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS_BASE_NAME PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS
#define PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG_BASE_CAST
#define PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG_BASE_NAME PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG
#define PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG_BASE_CAST
#define PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG_BASE_NAME PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG
#define PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE_BASE_CAST
#define PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE_BASE_NAME PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE
#define PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED_BASE_CAST
#define PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED_BASE_NAME PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED
#define PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE_BASE_CAST
#define PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE_BASE_NAME PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE
#define PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED_BASE_CAST
#define PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED_BASE_NAME PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED
#define PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU_BASE_CAST
#define PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU_BASE_NAME PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU
#define PDB_PROP_CL_UNSUPPORTED_CHIPSET_BASE_CAST
#define PDB_PROP_CL_UNSUPPORTED_CHIPSET_BASE_NAME PDB_PROP_CL_UNSUPPORTED_CHIPSET
#define PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE_BASE_CAST
#define PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE_BASE_NAME PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE
#define PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST_BASE_CAST
#define PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST_BASE_NAME PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST
#define PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR_BASE_CAST
#define PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR_BASE_NAME PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR
#define PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY_BASE_CAST
#define PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY_BASE_NAME PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY
#define PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE_BASE_CAST
#define PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE_BASE_NAME PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE
#define PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL_BASE_CAST
#define PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL_BASE_NAME PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL
#define PDB_PROP_CL_DISABLE_IOMAP_WC_BASE_CAST
#define PDB_PROP_CL_DISABLE_IOMAP_WC_BASE_NAME PDB_PROP_CL_DISABLE_IOMAP_WC
#define PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE_BASE_CAST
#define PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE_BASE_NAME PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE
#define PDB_PROP_CL_NOSNOOP_NOT_CAPABLE_BASE_CAST
#define PDB_PROP_CL_NOSNOOP_NOT_CAPABLE_BASE_NAME PDB_PROP_CL_NOSNOOP_NOT_CAPABLE
#define PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED_BASE_CAST
#define PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED_BASE_NAME PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED
#define PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED_BASE_CAST
#define PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED_BASE_NAME PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED
#define PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR_BASE_CAST
#define PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR_BASE_NAME PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR
#define PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET_BASE_CAST
#define PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET_BASE_NAME PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET
#define PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3_BASE_CAST
#define PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3_BASE_NAME PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3
#define PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR_BASE_CAST
#define PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR_BASE_NAME PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR
#define PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_BASE_CAST
#define PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_BASE_NAME PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED
#define PDB_PROP_CL_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390_BASE_CAST
#define PDB_PROP_CL_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390_BASE_NAME PDB_PROP_CL_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390
#define PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY_BASE_CAST
#define PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY_BASE_NAME PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY
#define PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR_BASE_CAST
#define PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR_BASE_NAME PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR
#define PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY_BASE_CAST
#define PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY_BASE_NAME PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY
#define PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE_BASE_CAST
#define PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE_BASE_NAME PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE
#define PDB_PROP_CL_IS_CHIPSET_IO_COHERENT_BASE_CAST
#define PDB_PROP_CL_IS_CHIPSET_IO_COHERENT_BASE_NAME PDB_PROP_CL_IS_CHIPSET_IO_COHERENT
#define PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE_BASE_CAST
#define PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE_BASE_NAME PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE

NV_STATUS __nvoc_objCreateDynamic_OBJCL(OBJCL**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJCL(OBJCL**, Dynamic*, NvU32);
#define __objCreate_OBJCL(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJCL((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS clInit_IMPL(struct OBJGPU *arg1, struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clInit(struct OBJGPU *arg1, struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clInit(arg1, pCl) clInit_IMPL(arg1, pCl)
#endif //__nvoc_chipset_h_disabled

#define clInit_HAL(arg1, pCl) clInit(arg1, pCl)

void clUpdateConfig_IMPL(struct OBJGPU *arg1, struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline void clUpdateConfig(struct OBJGPU *arg1, struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clUpdateConfig(arg1, pCl) clUpdateConfig_IMPL(arg1, pCl)
#endif //__nvoc_chipset_h_disabled

#define clUpdateConfig_HAL(arg1, pCl) clUpdateConfig(arg1, pCl)

NV_STATUS clTeardown_IMPL(struct OBJGPU *arg1, struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clTeardown(struct OBJGPU *arg1, struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clTeardown(arg1, pCl) clTeardown_IMPL(arg1, pCl)
#endif //__nvoc_chipset_h_disabled

#define clTeardown_HAL(arg1, pCl) clTeardown(arg1, pCl)

void clInitPropertiesFromRegistry_IMPL(struct OBJGPU *arg1, struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline void clInitPropertiesFromRegistry(struct OBJGPU *arg1, struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clInitPropertiesFromRegistry(arg1, pCl) clInitPropertiesFromRegistry_IMPL(arg1, pCl)
#endif //__nvoc_chipset_h_disabled

#define clInitPropertiesFromRegistry_HAL(arg1, pCl) clInitPropertiesFromRegistry(arg1, pCl)

NvU32 clInitMappingPciBusDevice_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NvU32 clInitMappingPciBusDevice(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return 0;
}
#else //__nvoc_chipset_h_disabled
#define clInitMappingPciBusDevice(arg1, arg2) clInitMappingPciBusDevice_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clInitMappingPciBusDevice_HAL(arg1, arg2) clInitMappingPciBusDevice(arg1, arg2)

NV_STATUS clFindFHBAndGetChipsetInfoIndex_IMPL(struct OBJCL *arg1, NvU16 *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clFindFHBAndGetChipsetInfoIndex(struct OBJCL *arg1, NvU16 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clFindFHBAndGetChipsetInfoIndex(arg1, arg2) clFindFHBAndGetChipsetInfoIndex_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clFindFHBAndGetChipsetInfoIndex_HAL(arg1, arg2) clFindFHBAndGetChipsetInfoIndex(arg1, arg2)

NV_STATUS clInitPcie_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clInitPcie(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clInitPcie(arg1, arg2) clInitPcie_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clInitPcie_HAL(arg1, arg2) clInitPcie(arg1, arg2)

void clUpdatePcieConfig_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline void clUpdatePcieConfig(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clUpdatePcieConfig(arg1, arg2) clUpdatePcieConfig_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clUpdatePcieConfig_HAL(arg1, arg2) clUpdatePcieConfig(arg1, arg2)

NV_STATUS clTeardownPcie_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clTeardownPcie(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clTeardownPcie(arg1, arg2) clTeardownPcie_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clTeardownPcie_HAL(arg1, arg2) clTeardownPcie(arg1, arg2)

NV_STATUS clPcieReadPortConfigReg_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, PORTDATA *arg3, NvU32 arg4, NvU32 *arg5);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieReadPortConfigReg(struct OBJGPU *arg1, struct OBJCL *arg2, PORTDATA *arg3, NvU32 arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadPortConfigReg(arg1, arg2, arg3, arg4, arg5) clPcieReadPortConfigReg_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadPortConfigReg_HAL(arg1, arg2, arg3, arg4, arg5) clPcieReadPortConfigReg(arg1, arg2, arg3, arg4, arg5)

NV_STATUS clPcieWriteRootPortConfigReg_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3, NvU32 arg4);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieWriteRootPortConfigReg(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieWriteRootPortConfigReg(arg1, arg2, arg3, arg4) clPcieWriteRootPortConfigReg_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_chipset_h_disabled

#define clPcieWriteRootPortConfigReg_HAL(arg1, arg2, arg3, arg4) clPcieWriteRootPortConfigReg(arg1, arg2, arg3, arg4)

NV_STATUS clPcieReadAerCapability_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, struct PcieAerCapability *arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieReadAerCapability(struct OBJGPU *arg1, struct OBJCL *arg2, struct PcieAerCapability *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadAerCapability(arg1, arg2, arg3) clPcieReadAerCapability_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadAerCapability_HAL(arg1, arg2, arg3) clPcieReadAerCapability(arg1, arg2, arg3)

NV_STATUS clPcieReadL1SsCapability_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, struct PexL1SubstateCapability *arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieReadL1SsCapability(struct OBJGPU *arg1, struct OBJCL *arg2, struct PexL1SubstateCapability *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadL1SsCapability(arg1, arg2, arg3) clPcieReadL1SsCapability_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadL1SsCapability_HAL(arg1, arg2, arg3) clPcieReadL1SsCapability(arg1, arg2, arg3)

NV_STATUS clPcieReadDevCtrlStatus_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3, NvU32 *arg4);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieReadDevCtrlStatus(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadDevCtrlStatus(arg1, arg2, arg3, arg4) clPcieReadDevCtrlStatus_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadDevCtrlStatus_HAL(arg1, arg2, arg3, arg4) clPcieReadDevCtrlStatus(arg1, arg2, arg3, arg4)

NV_STATUS clPcieClearDevCtrlStatus_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieClearDevCtrlStatus(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieClearDevCtrlStatus(arg1, arg2, arg3) clPcieClearDevCtrlStatus_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clPcieClearDevCtrlStatus_HAL(arg1, arg2, arg3) clPcieClearDevCtrlStatus(arg1, arg2, arg3)

NvU16 clPcieReadWord_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6);


#ifdef __nvoc_chipset_h_disabled
static inline NvU16 clPcieReadWord(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return 0;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadWord(arg1, arg2, arg3, arg4, arg5, arg6) clPcieReadWord_IMPL(arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadWord_HAL(arg1, arg2, arg3, arg4, arg5, arg6) clPcieReadWord(arg1, arg2, arg3, arg4, arg5, arg6)

NvU32 clPcieReadDword_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6);


#ifdef __nvoc_chipset_h_disabled
static inline NvU32 clPcieReadDword(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return 0;
}
#else //__nvoc_chipset_h_disabled
#define clPcieReadDword(arg1, arg2, arg3, arg4, arg5, arg6) clPcieReadDword_IMPL(arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_chipset_h_disabled

#define clPcieReadDword_HAL(arg1, arg2, arg3, arg4, arg5, arg6) clPcieReadDword(arg1, arg2, arg3, arg4, arg5, arg6)

void clPcieWriteWord_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6, NvU16 arg7);


#ifdef __nvoc_chipset_h_disabled
static inline void clPcieWriteWord(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6, NvU16 arg7) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clPcieWriteWord(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clPcieWriteWord_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_chipset_h_disabled

#define clPcieWriteWord_HAL(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clPcieWriteWord(arg1, arg2, arg3, arg4, arg5, arg6, arg7)

void clPcieWriteDword_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6, NvU32 arg7);


#ifdef __nvoc_chipset_h_disabled
static inline void clPcieWriteDword(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 arg4, NvU8 arg5, NvU32 arg6, NvU32 arg7) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clPcieWriteDword(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clPcieWriteDword_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_chipset_h_disabled

#define clPcieWriteDword_HAL(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clPcieWriteDword(arg1, arg2, arg3, arg4, arg5, arg6, arg7)

NvBool clFindBR04_IMPL(POBJGPU *pGpus, NvU32 NumGpus, NvBool flat, NvU32 devId, struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline NvBool clFindBR04(POBJGPU *pGpus, NvU32 NumGpus, NvBool flat, NvU32 devId, struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_FALSE;
}
#else //__nvoc_chipset_h_disabled
#define clFindBR04(pGpus, NumGpus, flat, devId, pCl) clFindBR04_IMPL(pGpus, NumGpus, flat, devId, pCl)
#endif //__nvoc_chipset_h_disabled

#define clFindBR04_HAL(pGpus, NumGpus, flat, devId, pCl) clFindBR04(pGpus, NumGpus, flat, devId, pCl)

NV_STATUS clResumeBridge_IMPL(struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clResumeBridge(struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clResumeBridge(pCl) clResumeBridge_IMPL(pCl)
#endif //__nvoc_chipset_h_disabled

#define clResumeBridge_HAL(pCl) clResumeBridge(pCl)

void *clFindP2PBrdg_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 *arg4, NvU8 *arg5, NvU8 *arg6, NvU16 *arg7, NvU16 *arg8);


#ifdef __nvoc_chipset_h_disabled
static inline void *clFindP2PBrdg(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3, NvU8 *arg4, NvU8 *arg5, NvU8 *arg6, NvU16 *arg7, NvU16 *arg8) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NULL;
}
#else //__nvoc_chipset_h_disabled
#define clFindP2PBrdg(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) clFindP2PBrdg_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#endif //__nvoc_chipset_h_disabled

#define clFindP2PBrdg_HAL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) clFindP2PBrdg(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)

void *clFindBrdgUpstreamPort_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool arg3, NvU8 *arg4, NvU8 *arg5, NvU8 *arg6, NvU16 *arg7, NvU16 *arg8, NvU8 *arg9);


#ifdef __nvoc_chipset_h_disabled
static inline void *clFindBrdgUpstreamPort(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool arg3, NvU8 *arg4, NvU8 *arg5, NvU8 *arg6, NvU16 *arg7, NvU16 *arg8, NvU8 *arg9) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NULL;
}
#else //__nvoc_chipset_h_disabled
#define clFindBrdgUpstreamPort(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) clFindBrdgUpstreamPort_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
#endif //__nvoc_chipset_h_disabled

#define clFindBrdgUpstreamPort_HAL(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) clFindBrdgUpstreamPort(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)

NV_STATUS clSetPortPcieCapOffset_IMPL(struct OBJCL *arg1, void *arg2, NvU32 *arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clSetPortPcieCapOffset(struct OBJCL *arg1, void *arg2, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clSetPortPcieCapOffset(arg1, arg2, arg3) clSetPortPcieCapOffset_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clSetPortPcieCapOffset_HAL(arg1, arg2, arg3) clSetPortPcieCapOffset(arg1, arg2, arg3)

NV_STATUS clStorePcieConfigSpaceBaseFromMcfg_IMPL(struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clStorePcieConfigSpaceBaseFromMcfg(struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clStorePcieConfigSpaceBaseFromMcfg(pCl) clStorePcieConfigSpaceBaseFromMcfg_IMPL(pCl)
#endif //__nvoc_chipset_h_disabled

#define clStorePcieConfigSpaceBaseFromMcfg_HAL(pCl) clStorePcieConfigSpaceBaseFromMcfg(pCl)

NV_STATUS clInsertPcieConfigSpaceBase_IMPL(struct OBJCL *arg1, RmPhysAddr arg2, NvU32 arg3, NvU8 arg4, NvU8 arg5);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clInsertPcieConfigSpaceBase(struct OBJCL *arg1, RmPhysAddr arg2, NvU32 arg3, NvU8 arg4, NvU8 arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clInsertPcieConfigSpaceBase(arg1, arg2, arg3, arg4, arg5) clInsertPcieConfigSpaceBase_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_chipset_h_disabled

#define clInsertPcieConfigSpaceBase_HAL(arg1, arg2, arg3, arg4, arg5) clInsertPcieConfigSpaceBase(arg1, arg2, arg3, arg4, arg5)

RmPhysAddr clFindPcieConfigSpaceBase_IMPL(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3);


#ifdef __nvoc_chipset_h_disabled
static inline RmPhysAddr clFindPcieConfigSpaceBase(struct OBJCL *arg1, NvU32 arg2, NvU8 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    RmPhysAddr ret;
    portMemSet(&ret, 0, sizeof(RmPhysAddr));
    return ret;
}
#else //__nvoc_chipset_h_disabled
#define clFindPcieConfigSpaceBase(arg1, arg2, arg3) clFindPcieConfigSpaceBase_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clFindPcieConfigSpaceBase_HAL(arg1, arg2, arg3) clFindPcieConfigSpaceBase(arg1, arg2, arg3)

void clFreePcieConfigSpaceBase_IMPL(struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline void clFreePcieConfigSpaceBase(struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clFreePcieConfigSpaceBase(pCl) clFreePcieConfigSpaceBase_IMPL(pCl)
#endif //__nvoc_chipset_h_disabled

#define clFreePcieConfigSpaceBase_HAL(pCl) clFreePcieConfigSpaceBase(pCl)

NV_STATUS clInitDeviceInfo_IMPL(struct OBJCL *arg1, struct OBJGPU *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clInitDeviceInfo(struct OBJCL *arg1, struct OBJGPU *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clInitDeviceInfo(arg1, arg2) clInitDeviceInfo_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clInitDeviceInfo_HAL(arg1, arg2) clInitDeviceInfo(arg1, arg2)

void clCountBR_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU8 *arg3);


#ifdef __nvoc_chipset_h_disabled
static inline void clCountBR(struct OBJGPU *arg1, struct OBJCL *arg2, NvU8 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clCountBR(arg1, arg2, arg3) clCountBR_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clCountBR_HAL(arg1, arg2, arg3) clCountBR(arg1, arg2, arg3)

void clFindCommonBR_IMPL(struct OBJGPU *pGpu1, struct OBJGPU *pGpu2, struct OBJCL *pCl, NvU8 *pBR04Bus, NvBool bScanAll);


#ifdef __nvoc_chipset_h_disabled
static inline void clFindCommonBR(struct OBJGPU *pGpu1, struct OBJGPU *pGpu2, struct OBJCL *pCl, NvU8 *pBR04Bus, NvBool bScanAll) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clFindCommonBR(pGpu1, pGpu2, pCl, pBR04Bus, bScanAll) clFindCommonBR_IMPL(pGpu1, pGpu2, pCl, pBR04Bus, bScanAll)
#endif //__nvoc_chipset_h_disabled

#define clFindCommonBR_HAL(pGpu1, pGpu2, pCl, pBR04Bus, bScanAll) clFindCommonBR(pGpu1, pGpu2, pCl, pBR04Bus, bScanAll)

void clFindCommonDownstreamBR_IMPL(struct OBJGPU *pGpu1, struct OBJGPU *pGpu2, struct OBJCL *pCl, NvU8 *pPciSwitchBus);


#ifdef __nvoc_chipset_h_disabled
static inline void clFindCommonDownstreamBR(struct OBJGPU *pGpu1, struct OBJGPU *pGpu2, struct OBJCL *pCl, NvU8 *pPciSwitchBus) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clFindCommonDownstreamBR(pGpu1, pGpu2, pCl, pPciSwitchBus) clFindCommonDownstreamBR_IMPL(pGpu1, pGpu2, pCl, pPciSwitchBus)
#endif //__nvoc_chipset_h_disabled

#define clFindCommonDownstreamBR_HAL(pGpu1, pGpu2, pCl, pPciSwitchBus) clFindCommonDownstreamBR(pGpu1, pGpu2, pCl, pPciSwitchBus)

void clFindBR_IMPL(struct OBJGPU *pGpu, struct OBJCL *pCl, NvU8 *pBR03Bus, NvU8 *pBR04Bus, NvBool *pBRNotBR04A03, NvBool *pNoUnsupportedBRFound, NvBool *pNoOnboardBR04);


#ifdef __nvoc_chipset_h_disabled
static inline void clFindBR(struct OBJGPU *pGpu, struct OBJCL *pCl, NvU8 *pBR03Bus, NvU8 *pBR04Bus, NvBool *pBRNotBR04A03, NvBool *pNoUnsupportedBRFound, NvBool *pNoOnboardBR04) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clFindBR(pGpu, pCl, pBR03Bus, pBR04Bus, pBRNotBR04A03, pNoUnsupportedBRFound, pNoOnboardBR04) clFindBR_IMPL(pGpu, pCl, pBR03Bus, pBR04Bus, pBRNotBR04A03, pNoUnsupportedBRFound, pNoOnboardBR04)
#endif //__nvoc_chipset_h_disabled

#define clFindBR_HAL(pGpu, pCl, pBR03Bus, pBR04Bus, pBRNotBR04A03, pNoUnsupportedBRFound, pNoOnboardBR04) clFindBR(pGpu, pCl, pBR03Bus, pBR04Bus, pBRNotBR04A03, pNoUnsupportedBRFound, pNoOnboardBR04)

void clSearchBR04_IMPL(struct OBJCL *pCl, NvU8 *pBR04BusArray, NvU8 *pBR04RevArray, NvU8 *pBR04Count);


#ifdef __nvoc_chipset_h_disabled
static inline void clSearchBR04(struct OBJCL *pCl, NvU8 *pBR04BusArray, NvU8 *pBR04RevArray, NvU8 *pBR04Count) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clSearchBR04(pCl, pBR04BusArray, pBR04RevArray, pBR04Count) clSearchBR04_IMPL(pCl, pBR04BusArray, pBR04RevArray, pBR04Count)
#endif //__nvoc_chipset_h_disabled

#define clSearchBR04_HAL(pCl, pBR04BusArray, pBR04RevArray, pBR04Count) clSearchBR04(pCl, pBR04BusArray, pBR04RevArray, pBR04Count)

NV_STATUS clPcieGetMaxCapableLinkWidth_IMPL(struct OBJCL *pCl, struct OBJGPU *pGpu, NvU32 *maxCapableLinkWidth);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieGetMaxCapableLinkWidth(struct OBJCL *pCl, struct OBJGPU *pGpu, NvU32 *maxCapableLinkWidth) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieGetMaxCapableLinkWidth(pCl, pGpu, maxCapableLinkWidth) clPcieGetMaxCapableLinkWidth_IMPL(pCl, pGpu, maxCapableLinkWidth)
#endif //__nvoc_chipset_h_disabled

#define clPcieGetMaxCapableLinkWidth_HAL(pCl, pGpu, maxCapableLinkWidth) clPcieGetMaxCapableLinkWidth(pCl, pGpu, maxCapableLinkWidth)

NV_STATUS clPcieIsRelaxedOrderingSafe_IMPL(struct OBJCL *pCl, struct OBJGPU *pGpu, NvBool *result);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieIsRelaxedOrderingSafe(struct OBJCL *pCl, struct OBJGPU *pGpu, NvBool *result) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieIsRelaxedOrderingSafe(pCl, pGpu, result) clPcieIsRelaxedOrderingSafe_IMPL(pCl, pGpu, result)
#endif //__nvoc_chipset_h_disabled

#define clPcieIsRelaxedOrderingSafe_HAL(pCl, pGpu, result) clPcieIsRelaxedOrderingSafe(pCl, pGpu, result)

NV_STATUS clStoreBusTopologyCache_IMPL(struct OBJCL *pCl, NvU32 secDomain, NvU16 secBus);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clStoreBusTopologyCache(struct OBJCL *pCl, NvU32 secDomain, NvU16 secBus) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clStoreBusTopologyCache(pCl, secDomain, secBus) clStoreBusTopologyCache_IMPL(pCl, secDomain, secBus)
#endif //__nvoc_chipset_h_disabled

#define clStoreBusTopologyCache_HAL(pCl, secDomain, secBus) clStoreBusTopologyCache(pCl, secDomain, secBus)

void clFreeBusTopologyCache_IMPL(struct OBJCL *pCl);


#ifdef __nvoc_chipset_h_disabled
static inline void clFreeBusTopologyCache(struct OBJCL *pCl) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clFreeBusTopologyCache(pCl) clFreeBusTopologyCache_IMPL(pCl)
#endif //__nvoc_chipset_h_disabled

#define clFreeBusTopologyCache_HAL(pCl) clFreeBusTopologyCache(pCl)

NvBool clIsL1SupportedForUpstreamPort_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NvBool clIsL1SupportedForUpstreamPort(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_FALSE;
}
#else //__nvoc_chipset_h_disabled
#define clIsL1SupportedForUpstreamPort(arg1, arg2) clIsL1SupportedForUpstreamPort_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clIsL1SupportedForUpstreamPort_HAL(arg1, arg2) clIsL1SupportedForUpstreamPort(arg1, arg2)

NvBool clIsL0sMaskEnabledForUpstreamPort_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NvBool clIsL0sMaskEnabledForUpstreamPort(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_FALSE;
}
#else //__nvoc_chipset_h_disabled
#define clIsL0sMaskEnabledForUpstreamPort(arg1, arg2) clIsL0sMaskEnabledForUpstreamPort_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clIsL0sMaskEnabledForUpstreamPort_HAL(arg1, arg2) clIsL0sMaskEnabledForUpstreamPort(arg1, arg2)

NV_STATUS clControlL0sL1LinkControlUpstreamPort_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clControlL0sL1LinkControlUpstreamPort(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clControlL0sL1LinkControlUpstreamPort(arg1, arg2, arg3) clControlL0sL1LinkControlUpstreamPort_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clControlL0sL1LinkControlUpstreamPort_HAL(arg1, arg2, arg3) clControlL0sL1LinkControlUpstreamPort(arg1, arg2, arg3)

NV_STATUS clChipsetAspmPublicControl_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clChipsetAspmPublicControl(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clChipsetAspmPublicControl(arg1, arg2, arg3) clChipsetAspmPublicControl_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

#define clChipsetAspmPublicControl_HAL(arg1, arg2, arg3) clChipsetAspmPublicControl(arg1, arg2, arg3)

NvBool clRootportNeedsNosnoopWAR_FWCLIENT(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NvBool clRootportNeedsNosnoopWAR(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_FALSE;
}
#else //__nvoc_chipset_h_disabled
#define clRootportNeedsNosnoopWAR(arg1, arg2) clRootportNeedsNosnoopWAR_FWCLIENT(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clRootportNeedsNosnoopWAR_HAL(arg1, arg2) clRootportNeedsNosnoopWAR(arg1, arg2)

NvU16 clPcieGetGpuLostDiagnosticData_IMPL(struct OBJGPU *pGpu, struct OBJCL *arg2, NvU8 *pBuffer, NvU32 size);


#ifdef __nvoc_chipset_h_disabled
static inline NvU16 clPcieGetGpuLostDiagnosticData(struct OBJGPU *pGpu, struct OBJCL *arg2, NvU8 *pBuffer, NvU32 size) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return 0;
}
#else //__nvoc_chipset_h_disabled
#define clPcieGetGpuLostDiagnosticData(pGpu, arg2, pBuffer, size) clPcieGetGpuLostDiagnosticData_IMPL(pGpu, arg2, pBuffer, size)
#endif //__nvoc_chipset_h_disabled

#define clPcieGetGpuLostDiagnosticData_HAL(pGpu, arg2, pBuffer, size) clPcieGetGpuLostDiagnosticData(pGpu, arg2, pBuffer, size)

NvU32 clGetChipsetL1ClockPMSupport_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);


#ifdef __nvoc_chipset_h_disabled
static inline NvU32 clGetChipsetL1ClockPMSupport(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return 0;
}
#else //__nvoc_chipset_h_disabled
#define clGetChipsetL1ClockPMSupport(arg1, arg2) clGetChipsetL1ClockPMSupport_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#define clGetChipsetL1ClockPMSupport_HAL(arg1, arg2) clGetChipsetL1ClockPMSupport(arg1, arg2)

NV_STATUS clGetPortAcsRedirectConfig_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3, NvU8 arg4, NvU8 arg5, NvU8 arg6, NvU32 *arg7);


#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clGetPortAcsRedirectConfig(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 arg3, NvU8 arg4, NvU8 arg5, NvU8 arg6, NvU32 *arg7) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clGetPortAcsRedirectConfig(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clGetPortAcsRedirectConfig_IMPL(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_chipset_h_disabled

#define clGetPortAcsRedirectConfig_HAL(arg1, arg2, arg3, arg4, arg5, arg6, arg7) clGetPortAcsRedirectConfig(arg1, arg2, arg3, arg4, arg5, arg6, arg7)

NV_STATUS clConstruct_IMPL(struct OBJCL *arg_pCl);

#define __nvoc_clConstruct(arg_pCl) clConstruct_IMPL(arg_pCl)
void clDestruct_IMPL(struct OBJCL *pCl);

#define __nvoc_clDestruct(pCl) clDestruct_IMPL(pCl)
NvBool clUpstreamVgaDecodeEnabled_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2);

#ifdef __nvoc_chipset_h_disabled
static inline NvBool clUpstreamVgaDecodeEnabled(struct OBJGPU *arg1, struct OBJCL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_FALSE;
}
#else //__nvoc_chipset_h_disabled
#define clUpstreamVgaDecodeEnabled(arg1, arg2) clUpstreamVgaDecodeEnabled_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

NV_STATUS clPcieGetRootGenSpeed_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU8 *arg3);

#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieGetRootGenSpeed(struct OBJGPU *arg1, struct OBJCL *arg2, NvU8 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieGetRootGenSpeed(arg1, arg2, arg3) clPcieGetRootGenSpeed_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

NV_STATUS clPcieGetDownstreamPortLinkCap2_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3);

#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clPcieGetDownstreamPortLinkCap2(struct OBJGPU *arg1, struct OBJCL *arg2, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clPcieGetDownstreamPortLinkCap2(arg1, arg2, arg3) clPcieGetDownstreamPortLinkCap2_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

NV_STATUS clCheckUpstreamLtrSupport_IMPL(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool *arg3);

#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clCheckUpstreamLtrSupport(struct OBJGPU *arg1, struct OBJCL *arg2, NvBool *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clCheckUpstreamLtrSupport(arg1, arg2, arg3) clCheckUpstreamLtrSupport_IMPL(arg1, arg2, arg3)
#endif //__nvoc_chipset_h_disabled

NV_STATUS clGetAtomicTypesSupported_IMPL(NvU32 arg1, NvU8 arg2, struct OBJCL *arg3, NvU32 *arg4);

#ifdef __nvoc_chipset_h_disabled
static inline NV_STATUS clGetAtomicTypesSupported(NvU32 arg1, NvU8 arg2, struct OBJCL *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_chipset_h_disabled
#define clGetAtomicTypesSupported(arg1, arg2, arg3, arg4) clGetAtomicTypesSupported_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_chipset_h_disabled

void clSyncWithGsp_IMPL(struct OBJCL *arg1, GspSystemInfo *arg2);

#ifdef __nvoc_chipset_h_disabled
static inline void clSyncWithGsp(struct OBJCL *arg1, GspSystemInfo *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJCL was disabled!");
}
#else //__nvoc_chipset_h_disabled
#define clSyncWithGsp(arg1, arg2) clSyncWithGsp_IMPL(arg1, arg2)
#endif //__nvoc_chipset_h_disabled

#undef PRIVATE_FIELD


#endif // CHIPSET_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CHIPSET_NVOC_H_
