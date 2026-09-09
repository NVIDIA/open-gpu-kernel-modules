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

#ifndef GMCAPI_SYSTEM_INFO_UNSTABLE_H
#define GMCAPI_SYSTEM_INFO_UNSTABLE_H

/* Keys that are still under development (or can change depending on build configuration)
   live in this file until they get promoted to gmcapi_system_info.h. */

/* 64-bit flags for Core Logic Chipset features/workarounds */
#define NVGMC_SI_CL_FLAGS                                                      0x1033
#define NVGMC_SI_CL_FLAGS_PCIE_CONFIG_ACCESSIBLE                               0:0
#define NVGMC_SI_CL_FLAGS_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390          1:1
#define NVGMC_SI_CL_FLAGS_DISABLE_BR03_FLOW_CONTROL                            2:2
#define NVGMC_SI_CL_FLAGS_ASLM_SUPPORTS_NV_LINK_UPGRADE                        3:3
#define NVGMC_SI_CL_FLAGS_ASLM_SUPPORTS_FAST_LINK_UPGRADE                      4:4
#define NVGMC_SI_CL_FLAGS_ASLM_SUPPORTS_HOT_RESET                              5:5
#define NVGMC_SI_CL_FLAGS_ASLM_SUPPORTS_GEN2_LINK_UPGRADE                      6:6
#define NVGMC_SI_CL_FLAGS_IS_CHIPSET_IN_ASPM_POR_LIST                          7:7
#define NVGMC_SI_CL_FLAGS_ASPM_L0S_CHIPSET_DISABLED                            8:8
#define NVGMC_SI_CL_FLAGS_ASPM_L1_CHIPSET_DISABLED                             9:9
#define NVGMC_SI_CL_FLAGS_WAR_4802761_ENABLED                                  10:10
#define NVGMC_SI_CL_FLAGS_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY                 11:11
#define NVGMC_SI_CL_FLAGS_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY                  12:12
#define NVGMC_SI_CL_FLAGS_ASPM_L1_UPSTREAM_PORT_SUPPORTED                      13:13
#define NVGMC_SI_CL_FLAGS_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED               14:14
#define NVGMC_SI_CL_FLAGS_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE       15:15
#define NVGMC_SI_CL_FLAGS_EXTENDED_TAG_FIELD_NOT_CAPABLE                       16:16
#define NVGMC_SI_CL_FLAGS_NOSNOOP_NOT_CAPABLE                                  17:17
#define NVGMC_SI_CL_FLAGS_RELAXED_ORDERING_NOT_CAPABLE                         18:18
#define NVGMC_SI_CL_FLAGS_PCIE_FORCE_GEN2_ENABLE                               19:19
#define NVGMC_SI_CL_FLAGS_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED                  20:20
#define NVGMC_SI_CL_FLAGS_ROOTPORT_NEEDS_NOSNOOP_WAR                           21:21
#define NVGMC_SI_CL_FLAGS_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR                    22:22
#define NVGMC_SI_CL_FLAGS_PCIE_CONFIG_SKIP_MCFG_READ                           23:23
#define NVGMC_SI_CL_FLAGS_ON_PCIE_GEN3_PATSBURG                                24:24
#define NVGMC_SI_CL_FLAGS_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU            25:25
#define NVGMC_SI_CL_FLAGS_BUG_999673_P2P_ARBITRARY_SPLIT_WAR                   26:26
#define NVGMC_SI_CL_FLAGS_UPSTREAM_LTR_SUPPORTED                               27:27
#define NVGMC_SI_CL_FLAGS_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3        28:28
#define NVGMC_SI_CL_FLAGS_BUG_1681803_WAR_DISABLE_MSCG                         29:29
#define NVGMC_SI_CL_FLAGS_PCIE_NON_COHERENT_USE_TC0_ONLY                       30:30
#define NVGMC_SI_CL_FLAGS_UNSUPPORTED_CHIPSET                                  31:31
#define NVGMC_SI_CL_FLAGS_IS_CHIPSET_IO_COHERENT                               32:32
#define NVGMC_SI_CL_FLAGS_DISABLE_IOMAP_WC                                     33:33
#define NVGMC_SI_CL_FLAGS_HAS_RESIZABLE_BAR_ISSUE                              34:34
#define NVGMC_SI_CL_FLAGS_BUG_3751839_GEN_SPEED_WAR                            35:35
#define NVGMC_SI_CL_FLAGS_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS                   36:36
#define NVGMC_SI_CL_FLAGS_WAR_AMD_5107271                                      37:37

#endif /* GMCAPI_SYSTEM_INFO_UNSTABLE_H */
