/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_SYSTEM_INFO_H
#define GMCAPI_SYSTEM_INFO_H

/* Key/Value definitions for GMCAPI_SET_SYSTEM_INFO (CPU -> GSP) */

/*
 * 32-bit key/value pairs
 */

/* PCI identification (sequential group) */
#define NVGMC_SI_PCI_DEVICE_ID                                                  0x0001
#define NVGMC_SI_PCI_SUB_DEVICE_ID                                              0x0002
#define NVGMC_SI_PCI_REVISION_ID                                                0x0003

/* PCI config space mirror (sequential group) */
#define NVGMC_SI_PCI_CONFIG_MIRROR_BASE                                         0x0010
#define NVGMC_SI_PCI_CONFIG_MIRROR_SIZE                                         0x0011

/* PCIe atomics completer device capability mask */
#define NVGMC_SI_PCIE_ATOMICS_CPL_DEV_CAP_MASK                                  0x0020

/* Chipset identification (sequential group) */
#define NVGMC_SI_CHIPSET                                                        0x0030
#define NVGMC_SI_CHIPSET_L1SS_ENABLE                                            0x0031

/* Hypervisor (sequential group) */
#define NVGMC_SI_HYPERVISOR_TYPE                                                0x0040
#define NVGMC_SI_VIRTUAL_CONFIG_BITS                                            0x0041

/* PCIe power/control registers (sequential group) */
#define NVGMC_SI_PCIE_POWER_CONTROL_VALUE                                       0x0050
#define NVGMC_SI_PF0_DEVICE_CONTROL2_REG                                        0x0051
#define NVGMC_SI_PCIE_LINK_CAP                                                  0x0052

/* Grid build CSP */
#define NVGMC_SI_GRID_BUILD_CSP                                                 0x0060

/* Host CPU architecture (used to program the SM's out-of-range/canonical-VA check) */
#define NVGMC_SI_HOST_ARCH                                                      0x0070
#define NVGMC_SI_HOST_ARCH_NONE                                                 0
#define NVGMC_SI_HOST_ARCH_X86_64                                               1
#define NVGMC_SI_HOST_ARCH_PPC64LE                                              2
#define NVGMC_SI_HOST_ARCH_ARM                                                  3
#define NVGMC_SI_HOST_ARCH_AARCH64                                              4
#define NVGMC_SI_HOST_ARCH_RISCV64                                              5

#define NVGMC_SI_HOST_OS_TYPE                                                   0x0071
#define NVGMC_SI_HOST_OS_TYPE_OTHER                                             0
#define NVGMC_SI_HOST_OS_TYPE_UNIX                                              1
#define NVGMC_SI_HOST_OS_TYPE_WINDOWS                                           2

/* Host page size (fits in 32 bits) */
#define NVGMC_SI_HOST_PAGE_SIZE                                                 0x0078

/* VF 32-bit values (sequential group) */
#define NVGMC_SI_VF_TOTAL_VFS                                                   0x0080
#define NVGMC_SI_VF_FIRST_VF_OFFSET                                             0x0081

#define NVGMC_SI_FHB_BUSINFO_REVISION_ID                                        0x0090
#define NVGMC_SI_CHIPSET_BUSINFO_REVISION_ID                                    0x0091

/*
 * ACPI Method Data Keys.  All are 32-bit values.
 */
#define NVGMC_SI_ACPI_DOD_STATUS                                                0x0200

/* ACPI DOD ACPI ID list as an array of 32-bit values */
#define NVGMC_SI_ACPI_DOD_ACPI_IDS                                              0x0210

#define NVGMC_SI_ACPI_JT_STATUS                                                 0x0220
#define NVGMC_SI_ACPI_JT_CAPS                                                   0x0221
#define NVGMC_SI_ACPI_JT_REV_ID                                                 0x0222
#define NVGMC_SI_ACPI_JT_B_SBIOS_CAPS                                           0x0223

#define NVGMC_SI_ACPI_MUX_TABLE_LEN                                             0x0230

#define NVGMC_SI_ACPI_MUX_MODE_ACPI_ID                                          0x0240
#define NVGMC_SI_ACPI_MUX_MODE_MODE                                             0x0241
#define NVGMC_SI_ACPI_MUX_MODE_STATUS                                           0x0242

#define NVGMC_SI_ACPI_MUX_PART_ACPI_ID                                          0x0250
#define NVGMC_SI_ACPI_MUX_PART_MODE                                             0x0251
#define NVGMC_SI_ACPI_MUX_PART_STATUS                                           0x0252

#define NVGMC_SI_ACPI_MUX_STATE_ACPI_ID                                         0x0260
#define NVGMC_SI_ACPI_MUX_STATE_MODE                                            0x0261
#define NVGMC_SI_ACPI_MUX_STATE_STATUS                                          0x0262

#define NVGMC_SI_ACPI_CAPS_STATUS                                               0x0270
#define NVGMC_SI_ACPI_CAPS_OPTIMUS_CAPS                                         0x0271

/*
 * 64-bit flag values — booleans packed as single-bit fields
 */

#define NVGMC_SI_PCIE_FLAGS                                                     0x1000
#define NVGMC_SI_PCIE_FLAGS_GPU_BEHIND_BRIDGE                                   0:0
#define NVGMC_SI_PCIE_FLAGS_FLR_SUPPORTED                                       1:1
#define NVGMC_SI_PCIE_FLAGS_64B_BAR0_SUPPORTED                                  2:2
#define NVGMC_SI_PCIE_FLAGS_MNOC_AVAILABLE                                      3:3
#define NVGMC_SI_PCIE_FLAGS_UPSTREAM_L0S_UNSUPPORTED                            4:4
#define NVGMC_SI_PCIE_FLAGS_UPSTREAM_L1_UNSUPPORTED                             5:5
#define NVGMC_SI_PCIE_FLAGS_UPSTREAM_L1_POR_SUPPORTED                           6:6
#define NVGMC_SI_PCIE_FLAGS_UPSTREAM_L1_POR_MOBILE_ONLY                         7:7
#define NVGMC_SI_PCIE_FLAGS_PCIE_POWER_CONTROL_PRESENT                          8:8
#define NVGMC_SI_PCIE_FLAGS_IS_CXL_DEVICE                                       9:9
#define NVGMC_SI_PCIE_FLAGS_UPSTREAM_ADDRESS_VALID                              10:10

#define NVGMC_SI_PLATFORM_FLAGS                                                 0x1001
#define NVGMC_SI_PLATFORM_FLAGS_SYSTEM_HAS_MUX                                  0:0
#define NVGMC_SI_PLATFORM_FLAGS_IS_PASSTHRU                                     1:1
#define NVGMC_SI_PLATFORM_FLAGS_IS_PRIMARY                                      2:2
#define NVGMC_SI_PLATFORM_FLAGS_IS_GRID_BUILD                                   3:3
#define NVGMC_SI_PLATFORM_FLAGS_S0IX_SUPPORT                                    4:4
#define NVGMC_SI_PLATFORM_FLAGS_GSP_NOCAT_ENABLED                               5:5
#define NVGMC_SI_PLATFORM_FLAGS_ENABLE_DYN_GRAN_PAGE_ARRAYS                     6:6
#define NVGMC_SI_PLATFORM_FLAGS_RESERVE_ZERO_FB_ADDR_AS_REGION                  7:7

#define NVGMC_SI_FEATURE_FLAGS                                                  0x1002
#define NVGMC_SI_FEATURE_FLAGS_UNIX_HDMI_FRL_COMPLIANCE                         0:0
#define NVGMC_SI_FEATURE_FLAGS_PRESERVE_VIDEO_MEM_ALLOCS                        1:1
#define NVGMC_SI_FEATURE_FLAGS_TDR_EVENT_SUPPORTED                              2:2
#define NVGMC_SI_FEATURE_FLAGS_STRETCH_VBLANK_CAPABLE                           3:3
#define NVGMC_SI_FEATURE_FLAGS_CLOCK_BOOST_SUPPORTED                            4:4
#define NVGMC_SI_FEATURE_FLAGS_IS_CMC_BASED_HWS                                 5:5
#define NVGMC_SI_FEATURE_FLAGS_WINDOW_CHANNEL_ALWAYS_MAPPED                     6:6

#define NVGMC_SI_VF_FLAGS                                                       0x1003
#define NVGMC_SI_VF_FLAGS_64BIT_BAR0                                            0:0
#define NVGMC_SI_VF_FLAGS_64BIT_BAR1                                            1:1
#define NVGMC_SI_VF_FLAGS_64BIT_BAR2                                            2:2

/* GPU physical addresses (sequential group) */
#define NVGMC_SI_GPU_PHYS_ADDR                                                  0x1010
#define NVGMC_SI_GPU_PHYS_FB_ADDR                                               0x1011
#define NVGMC_SI_GPU_PHYS_INST_ADDR                                             0x1012
#define NVGMC_SI_GPU_PHYS_IO_ADDR                                               0x1013

/* Domain/bus/device, simulation, and notify surface addresses (sequential group) */
#define NVGMC_SI_NV_DOMAIN_BUS_DEVICE_FUNC                                      0x1020
#define NVGMC_SI_SIM_ACCESS_BUF_PHYS_ADDR                                       0x1021
#define NVGMC_SI_NOTIFY_OP_SHARED_SURF_PHYS_ADDR                                0x1022

/* PCIe atomics, console, user VA, chipset PDB properties (sequential group) */
#define NVGMC_SI_PCIE_ATOMICS_OP_MASK                                           0x1030
#define NVGMC_SI_CONSOLE_MEM_SIZE                                               0x1031
#define NVGMC_SI_MAX_USER_VA                                                    0x1032

/* System timer offset */
#define NVGMC_SI_SYS_TIMER_OFFSET_NS                                            0x1040

/* VF BAR addresses (sequential group) */
#define NVGMC_SI_VF_FIRST_BAR0_ADDRESS                                          0x1050
#define NVGMC_SI_VF_FIRST_BAR1_ADDRESS                                          0x1051
#define NVGMC_SI_VF_FIRST_BAR2_ADDRESS                                          0x1052

/*
 * BUSINFO — four 16-bit IDs packed into a single 64-bit value,
 * revision IDs are sent as a separate 32-bit key (up above).
 */
#define NVGMC_SI_FHB_BUSINFO                                                    0x1090
#define NVGMC_SI_CHIPSET_BUSINFO                                                0x1091

#define NVGMC_SI_BUSINFO_DEVICE_ID                                              15:0
#define NVGMC_SI_BUSINFO_VENDOR_ID                                              31:16
#define NVGMC_SI_BUSINFO_SUBDEVICE_ID                                           47:32
#define NVGMC_SI_BUSINFO_SUBVENDOR_ID                                           63:48

/*
 * Kernel driver type (e.g. "NVRM", "Nova") and version info
 * GSP will only use these as a last resort, for bug and stability workarounds.
 */
#define NVGMC_SI_KERNEL_DRIVER_NAME                                             0x1100
#define NVGMC_SI_KERNEL_DRIVER_VERSION_STRING                                   0x1101
#define NVGMC_SI_KERNEL_DRIVER_CHANGELIST                                       0x1102

/* Opt in to use unstable interfaces to GSP */
#define NVGMC_SI_UNSTABLE_INTERFACE_SUPPORT                                     0x1110
#define NVGMC_SI_UNSTABLE_INTERFACE_SUPPORT_RPC                                 0:0
#define NVGMC_SI_UNSTABLE_INTERFACE_SUPPORT_EVENTS                              1:1

/*
 * Special system info keys used for passing registry entries.  The first key will set
 * the name of the registry value, followed by another key that sets the value of the
 * registry key, with different keys representing different data types.  Registry
 * key names are limited to 256 bytes including the null terminator.
 */
#define NVGMC_SI_REGKEY_NAME                                                    0x3070
#define NVGMC_SI_REGKEY_VALUE_U32                                               0x3071
#define NVGMC_SI_REGKEY_VALUE_STRING                                            0x3072
#define NVGMC_SI_REGKEY_VALUE_BINARY                                            0x3073

#define NVGMC_SI_REGKEY_NAME_MAX_LEN                                            256

#endif /* GMCAPI_SYSTEM_INFO_H */
