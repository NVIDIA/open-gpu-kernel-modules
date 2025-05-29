/*
 * SPDX-FileCopyrightText: Copyright (c) 1997-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file holds NVIDIA Resource Manager registry key definitions that are
// shared between Windows and Unix
//

#ifndef NVRM_REGISTRY_H
#define NVRM_REGISTRY_H

#include "nvtypes.h"

//
// Some shared defines with nvReg.h
//
#if defined(NV_UNIX)
#define NV4_REG_GLOBAL_BASE_KEY     ""
#define NV4_REG_GLOBAL_BASE_PATH    "_NV_"
#else
#define NV4_REG_GLOBAL_BASE_KEY     HKEY_LOCAL_MACHINE
#define NV4_REG_GLOBAL_BASE_PATH    "SOFTWARE\\NVIDIA Corporation\\Global"
#endif
#define NV4_REG_SUBKEY                    "NVidia"
#define NV4_REG_DISPLAY_DRIVER_SUBKEY     "Display"
#define NV4_REG_RESOURCE_MANAGER_SUBKEY   "System"

//
// Globally overrides the memory type used to store surfaces.
// Used by all parts of the driver and stored in the hardware-specific key.
// Mirrored from nvReg.h
//
#define NV_REG_STR_GLOBAL_SURFACE_OVERRIDE               "GlobalSurfaceOverrides"
#define NV_REG_STR_GLOBAL_SURFACE_OVERRIDE_DISABLE       (0x00000000) // Do not use global surface overrides
#define NV_REG_STR_GLOBAL_SURFACE_OVERRIDE_ENABLE        (0x00000001)
#define NV_REG_STR_GLOBAL_SURFACE_OVERRIDE_RM_VALUE      1:0
#define NV_REG_STR_GLOBAL_SURFACE_OVERRIDE_RM_ENABLE     3:3


#define NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT         "RmDefaultTimeout"
// Type Dword
// Override default RM timeout.  Measured in milliseconds.
// Not scaled for emulation

#define NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS    "RmDefaultTimeoutFlags"
#define NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS_OSTIMER    4
#define NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS_OSDELAY    8
// Type Dword
// Override default RM timeout flags to either OSDELAY or OSTIMER.


#define NV_REG_STR_SUPPRESS_CLASS_LIST "SuppressClassList"
// Type String
// A list of comma separated classes to suppress
// examples:
// 5097
// 4097, 5097
// etc


//
// Allow instance memory overrides.  Some fields are chip specific
// and may not apply to all chips.  Since there are many fields,
// this is spread across several DWORD registry keys.
//
// Type DWORD
// Encoding:
//    DEFAULT   RM determines
//    COH       Coherent system memory
//    NCOH      Non-coherent system memory
//    VID       Local video memory
//
#define NV_REG_STR_RM_INST_LOC                              "RMInstLoc"
#define NV_REG_STR_RM_INST_LOC_2                            "RMInstLoc2"
#define NV_REG_STR_RM_INST_LOC_3                            "RMInstLoc3"
#define NV_REG_STR_RM_INST_LOC_4                            "RMInstLoc4"

#define NV_REG_STR_RM_INST_LOC_DEFAULT                      (0x00000000)
#define NV_REG_STR_RM_INST_LOC_COH                          (0x00000001)
#define NV_REG_STR_RM_INST_LOC_NCOH                         (0x00000002)
#define NV_REG_STR_RM_INST_LOC_VID                          (0x00000003)

#define NV_REG_STR_RM_INST_LOC_ALL_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_INST_LOC_ALL_COH                      (0x55555555)
#define NV_REG_STR_RM_INST_LOC_ALL_NCOH                     (0xAAAAAAAA)
#define NV_REG_STR_RM_INST_LOC_ALL_VID                      (0xFFFFFFFF)

//
// Allow instance memory overrides.  Some fields are chip specific
// and may not apply to all chips.  Since there are many fields,
// this is spread across several DWORD registry keys.
//
// The registry keys are defined in nvrm_registry.
// Specific overrrides are defined here.
//
// Type DWORD
// Encoding:
//    DEFAULT   RM determines
//    COH       Coherent system memory
//    NCOH      Non-coherent system memory
//    VID       Local video memory
//
#define NV_REG_STR_RM_INST_LOC_PTE                          1:0             // Context PTE
#define NV_REG_STR_RM_INST_LOC_PTE_DEFAULT                  NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_PTE_COH                      NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_PTE_NCOH                     NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_PTE_VID                      NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_BAR_PTE                      3:2             // BAR PTE
#define NV_REG_STR_RM_INST_LOC_BAR_PTE_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_BAR_PTE_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_BAR_PTE_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_BAR_PTE_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_INSTBLK                      5:4             // Instance block
#define NV_REG_STR_RM_INST_LOC_INSTBLK_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_INSTBLK_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_INSTBLK_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_INSTBLK_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_RAMFC                        7:6             // RAMFC save area
#define NV_REG_STR_RM_INST_LOC_RAMFC_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_RAMFC_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_RAMFC_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_RAMFC_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_CACHE1                       9:8             // CACHE1 save area
#define NV_REG_STR_RM_INST_LOC_CACHE1_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_CACHE1_COH                   NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_CACHE1_NCOH                  NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_CACHE1_VID                   NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_GRCTX                        11:10           // Graphics contxt
#define NV_REG_STR_RM_INST_LOC_GRCTX_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_GRCTX_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_GRCTX_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_GRCTX_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_RUNLIST                      13:12           // Runlist
#define NV_REG_STR_RM_INST_LOC_RUNLIST_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_RUNLIST_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_RUNLIST_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_RUNLIST_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_DISPLAY                      15:14           // Display
#define NV_REG_STR_RM_INST_LOC_DISPLAY_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_DISPLAY_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_DISPLAY_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_DISPLAY_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_USERD                        17:16           // USERD
#define NV_REG_STR_RM_INST_LOC_USERD_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_USERD_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_USERD_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_USERD_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_EVENTBUFFER                  19:18           // EVENTBUFFER
#define NV_REG_STR_RM_INST_LOC_EVENTBUFFER_DEFAULT          NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_EVENTBUFFER_COH              NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_EVENTBUFFER_NCOH             NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_EVENTBUFFER_VID              NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_UNUSED                       21:20           // UNUSED
#define NV_REG_STR_RM_INST_LOC_UNUSED_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_CIPHER_XCHG                  23:22           // Cipher exchange memory resources
#define NV_REG_STR_RM_INST_LOC_CIPHER_XCHG_DEFAULT          NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_CIPHER_XCHG_COH              NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_CIPHER_XCHG_NCOH             NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_CIPHER_XCHG_VID              NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_PDE                          25:24           // Context PDE
#define NV_REG_STR_RM_INST_LOC_PDE_DEFAULT                  NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_PDE_COH                      NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_PDE_NCOH                     NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_PDE_VID                      NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_BAR_PDE                      27:26           // BAR PDE
#define NV_REG_STR_RM_INST_LOC_BAR_PDE_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_BAR_PDE_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_BAR_PDE_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_BAR_PDE_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_PMUINST                      29:28           // PMUINST
#define NV_REG_STR_RM_INST_LOC_PMUINST_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_PMUINST_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_PMUINST_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_PMUINST_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_PMUUCODE                     31:30           // PMU UCODE
#define NV_REG_STR_RM_INST_LOC_PMUUCODE_DEFAULT             NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_PMUUCODE_COH                 NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_PMUUCODE_NCOH                NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_PMUUCODE_VID                 NV_REG_STR_RM_INST_LOC_VID

#define NV_REG_STR_RM_INST_LOC_2_COMPTAG_STORE                   1:0           // Compbit backing store
#define NV_REG_STR_RM_INST_LOC_2_COMPTAG_STORE_DEFAULT           NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_COMPTAG_STORE_COH               NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_COMPTAG_STORE_NCOH              NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_COMPTAG_STORE_VID               NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_ATTR_CB                         3:2           // Attribute Circular Buffer
#define NV_REG_STR_RM_INST_LOC_2_ATTR_CB_DEFAULT                 NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_ATTR_CB_COH                     NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_ATTR_CB_NCOH                    NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_ATTR_CB_VID                     NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_BUNDLE_CB                       5:4           // Bundle Circular Buffer
#define NV_REG_STR_RM_INST_LOC_2_BUNDLE_CB_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_BUNDLE_CB_COH                   NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_BUNDLE_CB_NCOH                  NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_BUNDLE_CB_VID                   NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_PAGEPOOL                        7:6           // Pagepool Buffer
#define NV_REG_STR_RM_INST_LOC_2_PAGEPOOL_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_PAGEPOOL_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_PAGEPOOL_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_PAGEPOOL_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_GOLD_CTX                        9:8           // Golden Context Image
#define NV_REG_STR_RM_INST_LOC_2_GOLD_CTX_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_GOLD_CTX_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_GOLD_CTX_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_GOLD_CTX_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_BAR_CTX                         11:10         // Bar context aperture
#define NV_REG_STR_RM_INST_LOC_2_BAR_CTX_DEFAULT                 NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_BAR_CTX_COH                     NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_BAR_CTX_NCOH                    NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_BAR_CTX_VID                     NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_PMU_PWR_RAIL_VIDEO_PRED_BUFFER_SURFACE            13:12 // Power Rail Video Prediction
#define NV_REG_STR_RM_INST_LOC_2_PMU_PWR_RAIL_VIDEO_PRED_BUFFER_SURFACE_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_PMU_PWR_RAIL_VIDEO_PRED_BUFFER_SURFACE_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_PMU_PWR_RAIL_VIDEO_PRED_BUFFER_SURFACE_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_PMU_PWR_RAIL_VIDEO_PRED_BUFFER_SURFACE_VID        NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_CTX_PATCH                       15:14         // context patch
#define NV_REG_STR_RM_INST_LOC_2_CTX_PATCH_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_CTX_PATCH_COH                   NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_CTX_PATCH_NCOH                  NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_CTX_PATCH_VID                   NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_MMU_READ                        17:16         // MMU Read
#define NV_REG_STR_RM_INST_LOC_2_MMU_READ_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_MMU_READ_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_MMU_READ_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_MMU_READ_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_MMU_WRITE                       19:18         // MMU Write
#define NV_REG_STR_RM_INST_LOC_2_MMU_WRITE_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_MMU_WRITE_COH                   NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_MMU_WRITE_NCOH                  NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_MMU_WRITE_VID                   NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_UNUSED                          21:20         // Unused
#define NV_REG_STR_RM_INST_LOC_2_ZCULLCTX                        23:22         // zcull context buffer
#define NV_REG_STR_RM_INST_LOC_2_ZCULLCTX_DEFAULT                NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_ZCULLCTX_COH                    NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_ZCULLCTX_NCOH                   NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_ZCULLCTX_VID                    NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_PMCTX                           25:24         // PM context buffer
#define NV_REG_STR_RM_INST_LOC_2_PMCTX_DEFAULT                   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_PMCTX_COH                       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_PMCTX_NCOH                      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_PMCTX_VID                       NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_DPUDBG                          27:26         // DPU Debug/Falctrace Buffer
#define NV_REG_STR_RM_INST_LOC_2_DPUDBG_DEFAULT                  NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_DPUDBG_COH                      NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_DPUDBG_NCOH                     NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_DPUDBG_VID                      NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_2_PMUPG                           29:28         // PMU PG buffer
#define NV_REG_STR_RM_INST_LOC_2_PMUPG_DEFAULT                   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_PMUPG_COH                       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_PMUPG_NCOH                      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_PMUPG_VID                       NV_REG_STR_RM_INST_LOC_VID
//
// Note: on Blackwell-and-later, the location of the PMU_LOGGER cannot be
// independently controlled. If the regkey is set and the PMU_LOGGER is not
// already being placed in the specified surface, the driver will fail to load.
//
#define NV_REG_STR_RM_INST_LOC_2_PMU_LOGGER                      31:30
#define NV_REG_STR_RM_INST_LOC_2_PMU_LOGGER_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_2_PMU_LOGGER_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_2_PMU_LOGGER_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_2_PMU_LOGGER_VID                  NV_REG_STR_RM_INST_LOC_VID

#define NV_REG_STR_RM_INST_LOC_3_PG_LOG_SURFACE               1:0             // PG log surface
#define NV_REG_STR_RM_INST_LOC_3_PG_LOG_SURFACE_DEFAULT       NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_PG_LOG_SURFACE_COH           NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_PG_LOG_SURFACE_NCOH          NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_PG_LOG_SURFACE_VID           NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_PREEMPT_BUFFER               3:2             // Preemption buffer
#define NV_REG_STR_RM_INST_LOC_3_PREEMPT_BUFFER_DEFAULT       NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_PREEMPT_BUFFER_COH           NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_PREEMPT_BUFFER_NCOH          NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_PREEMPT_BUFFER_VID           NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_GFXP_BETACB_BUFFER           5:4             // GFXP BetaCB buffer
#define NV_REG_STR_RM_INST_LOC_3_GFXP_BETACB_BUFFER_DEFAULT   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_GFXP_BETACB_BUFFER_COH       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_BETACB_BUFFER_NCOH      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_BETACB_BUFFER_VID       NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_GFXP_PAGEPOOL_BUFFER         7:6             // GFXP Pagepool buffer
#define NV_REG_STR_RM_INST_LOC_3_GFXP_PAGEPOOL_BUFFER_DEFAULT NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_GFXP_PAGEPOOL_BUFFER_COH     NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_PAGEPOOL_BUFFER_NCOH    NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_PAGEPOOL_BUFFER_VID     NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_BSI_IMAGE                    9:8             // BSI RAM image
#define NV_REG_STR_RM_INST_LOC_3_BSI_IMAGE_DEFAULT            NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_BSI_IMAGE_COH                NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_BSI_IMAGE_NCOH               NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_BSI_IMAGE_VID                NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_PRIV_ACCESS_MAP              11:10           // Priv whitelist buffer
#define NV_REG_STR_RM_INST_LOC_3_PRIV_ACCESS_MAP_DEFAULT      NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_PRIV_ACCESS_MAP_COH          NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_PRIV_ACCESS_MAP_NCOH         NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_PRIV_ACCESS_MAP_VID          NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_SEC2DBG                      13:12           // SEC2 Debug/Falctrace Buffer
#define NV_REG_STR_RM_INST_LOC_3_SEC2DBG_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_SEC2DBG_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_SEC2DBG_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_SEC2DBG_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_FECS_UCODE                   15:14           // FECS UCODE
#define NV_REG_STR_RM_INST_LOC_3_FECS_UCODE_DEFAULT           NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_FECS_UCODE_COH               NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_FECS_UCODE_NCOH              NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_FECS_UCODE_VID               NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_GFXP_SPILL_BUFFER            17:16           // GFXP Pagepool buffer
#define NV_REG_STR_RM_INST_LOC_3_GFXP_SPILL_BUFFER_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_GFXP_SPILL_BUFFER_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_SPILL_BUFFER_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_SPILL_BUFFER_VID        NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_UVM_FAULT_BUFFER_NONREPLAYABLE            19:18           // UVM Non-Replayable fault buffer
#define NV_REG_STR_RM_INST_LOC_3_UVM_FAULT_BUFFER_NONREPLAYABLE_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_UVM_FAULT_BUFFER_NONREPLAYABLE_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_UVM_FAULT_BUFFER_NONREPLAYABLE_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_UVM_FAULT_BUFFER_NONREPLAYABLE_VID        NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_BAR_SCRATCH_PAGE             21:20           // BAR scratch pages
#define NV_REG_STR_RM_INST_LOC_3_BAR_SCRATCH_PAGE_DEFAULT     NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_BAR_SCRATCH_PAGE_COH         NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_BAR_SCRATCH_PAGE_NCOH        NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_BAR_SCRATCH_PAGE_VID         NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_FLCNINST                     23:22           // FLCNINST
#define NV_REG_STR_RM_INST_LOC_3_FLCNINST_DEFAULT             NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_FLCNINST_COH                 NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_FLCNINST_NCOH                NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_FLCNINST_VID                 NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_RTVCB_BUFFER                 25:24           // RTVCB buffer
#define NV_REG_STR_RM_INST_LOC_3_RTVCB_BUFFER_DEFAULT         NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_RTVCB_BUFFER_COH             NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_RTVCB_BUFFER_NCOH            NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_RTVCB_BUFFER_VID             NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_GFXP_RTVCB_BUFFER            27:26           // GFXP RTVCB buffer
#define NV_REG_STR_RM_INST_LOC_3_GFXP_RTVCB_BUFFER_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_GFXP_RTVCB_BUFFER_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_RTVCB_BUFFER_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_GFXP_RTVCB_BUFFER_VID        NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER          29:28           // Fault method buffer
#define NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER_DEFAULT  NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER_COH      NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER_NCOH     NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_FAULT_METHOD_BUFFER_VID      NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_3_PMU_DPU_DMA                  31:30           // PMU/DPU DMA transfers
#define NV_REG_STR_RM_INST_LOC_3_PMU_DPU_DMA_DEFAULT          NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_3_PMU_DPU_DMA_COH              NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_3_PMU_DPU_DMA_NCOH             NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_3_PMU_DPU_DMA_VID              NV_REG_STR_RM_INST_LOC_VID

#define NV_REG_STR_RM_INST_LOC_4_DISP_SC                      1:0             // Display state cache buffer
#define NV_REG_STR_RM_INST_LOC_4_DISP_SC_DEFAULT              NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_DISP_SC_COH                  NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_DISP_SC_NCOH                 NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_DISP_SC_VID                  NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER           3:2             // FIFO channel push buffer
#define NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_DEFAULT   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_COH       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_NCOH      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_VID       NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_4_FW_SEC_LIC_COMMAND           5:4             // Firmware security license command
#define NV_REG_STR_RM_INST_LOC_4_FW_SEC_LIC_COMMAND_DEFAULT   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_FW_SEC_LIC_COMMAND_COH       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_FW_SEC_LIC_COMMAND_NCOH      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_FW_SEC_LIC_COMMAND_VID       NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_4_VRDS                         7:6             // VBIOS runtime data security
#define NV_REG_STR_RM_INST_LOC_4_VRDS_DEFAULT                 NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_VRDS_COH                     NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_VRDS_NCOH                    NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_VRDS_VID                     NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_4_FLCN_UCODE_BUFFERS           9:8             // Falcon uCode buffers
#define NV_REG_STR_RM_INST_LOC_4_FLCN_UCODE_BUFFERS_DEFAULT   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_FLCN_UCODE_BUFFERS_COH       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_FLCN_UCODE_BUFFERS_NCOH      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_FLCN_UCODE_BUFFERS_VID       NV_REG_STR_RM_INST_LOC_VID
#define NV_REG_STR_RM_INST_LOC_4_UVM_FAULT_BUFFER_REPLAYABLE            11:10           // UVM Replayable fault buffer
#define NV_REG_STR_RM_INST_LOC_4_UVM_FAULT_BUFFER_REPLAYABLE_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_UVM_FAULT_BUFFER_REPLAYABLE_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_UVM_FAULT_BUFFER_REPLAYABLE_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_UVM_FAULT_BUFFER_REPLAYABLE_VID        NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of BARs. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_BAR                          13:12             // BAR Bind location
#define NV_REG_STR_RM_INST_LOC_4_BAR_DEFAULT                  NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_BAR_COH                      NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_BAR_NCOH                     NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_BAR_VID                      NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of async CEs. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_CE                           15:14             // Async CE Bind location
#define NV_REG_STR_RM_INST_LOC_4_CE_DEFAULT                   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_CE_COH                       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_CE_NCOH                      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_CE_VID                       NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of GR/GRCE. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_GR                           17:16             // GR/GRCE Bind location
#define NV_REG_STR_RM_INST_LOC_4_GR_DEFAULT                   NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_GR_COH                       NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_GR_NCOH                      NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_GR_VID                       NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of VEs. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_FALCON                       19:18             // FALCON Bind location
#define NV_REG_STR_RM_INST_LOC_4_FALCON_DEFAULT               NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_FALCON_COH                   NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_FALCON_NCOH                  NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_FALCON_VID                   NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of HWPM PMA. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_HWPM_PMA                     21:20             // HWPM PMA Bind location
#define NV_REG_STR_RM_INST_LOC_4_HWPM_PMA_DEFAULT             NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_HWPM_PMA_COH                 NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_HWPM_PMA_NCOH                NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_HWPM_PMA_VID                 NV_REG_STR_RM_INST_LOC_VID

//
// Separately define instance block location of HWPM PMA. Default Setting
// reverts to NV_REG_STR_RM_INST_LOC_INSTBLK
//
#define NV_REG_STR_RM_INST_LOC_4_FECS_EVENT_BUF               23:22             // FECS EVENT buffer location
#define NV_REG_STR_RM_INST_LOC_4_FECS_EVENT_BUF_DEFAULT       NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_FECS_EVENT_BUF_COH           NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_FECS_EVENT_BUF_NCOH          NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_FECS_EVENT_BUF_VID           NV_REG_STR_RM_INST_LOC_VID

//
// Overrides for the GFXP SETUP buffer
//
#define NV_REG_STR_RM_INST_LOC_4_GFXP_SETUP_BUFFER            25:24           // GFXP SETUP buffer
#define NV_REG_STR_RM_INST_LOC_4_GFXP_SETUP_BUFFER_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_GFXP_SETUP_BUFFER_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_GFXP_SETUP_BUFFER_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_GFXP_SETUP_BUFFER_VID        NV_REG_STR_RM_INST_LOC_VID

//
// Overrides for the VIDEO ENGINE BOOTARGS buffer
//
#define NV_REG_STR_RM_INST_LOC_4_VIDEO_ENGINE_BOOTARGS            27:26           // VIDEO_ENGINE BOOTARGS buffer
#define NV_REG_STR_RM_INST_LOC_4_VIDEO_ENGINE_BOOTARGS_DEFAULT    NV_REG_STR_RM_INST_LOC_DEFAULT
#define NV_REG_STR_RM_INST_LOC_4_VIDEO_ENGINE_BOOTARGS_COH        NV_REG_STR_RM_INST_LOC_COH
#define NV_REG_STR_RM_INST_LOC_4_VIDEO_ENGINE_BOOTARGS_NCOH       NV_REG_STR_RM_INST_LOC_NCOH
#define NV_REG_STR_RM_INST_LOC_4_VIDEO_ENGINE_BOOTARGS_VID        NV_REG_STR_RM_INST_LOC_VID

#define NV_REG_STR_RM_GSP_STATUS_QUEUE_SIZE         "RmGspStatusQueueSize"
// TYPE DWORD
// Set the GSP status queue size in KB (for GSP to CPU RPC status and event communication)

#define NV_REG_STR_RM_MSG                                   "RmMsg"
// Type String: Set parameters for RM DBG_PRINTF.  Only for builds with printfs enabled.
// Encoding:
//    rule = [!][filename|function][:startline][-endline]
//    Format = rule[,rule]


#define NV_REG_STR_RM_THREAD_STATE_SETUP_FLAGS      "RmThreadStateSetupFlags"
// Type DWORD
// Enables or disables various ThreadState features
// See resman/inc/kernel/core/thread_state.h for
// THREAD_STATE_SETUP_FLAGS values.


#define NV_REG_STR_RM_ENABLE_EVENT_TRACER                  "RMEnableEventTracer"
#define NV_REG_STR_RM_ENABLE_EVENT_TRACER_DISABLE          0
#define NV_REG_STR_RM_ENABLE_EVENT_TRACER_ENABLE           1
#define NV_REG_STR_RM_ENABLE_EVENT_TRACER_DEFAULT          NV_REG_STR_RM_ENABLE_EVENT_TRACER_DISABLE
// Type DWORD
// Encoding boolean
// Enable/Disable RM event tracing
// 0 - Disable RM event tracing
// 1 - Enable RM event tracing


#define NV_REG_STR_RM_COMPUTE_MODE_RULES "RmComputeModeRules"
// Type DWORD
// Saves the last compute mode rule set by the client.
// Encoding:
// Bits 31:0 : Last compute mode rule set by the client

#define NV_REG_STR_ILLUM_ATTRIB_LOGO_BRIGHTNESS         "RmIllumLogoBrightness"
// Type DWORD:
// contains Logo Illumination Brightness in percent to be used on driver load.

#define NV_REG_STR_ILLUM_ATTRIB_SLI_BRIGHTNESS          "RmIllumSLIBrightness"
// Type DWORD:
// contains SLI Illumination Brightness in percent to be used on driver load.


#define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_1        "RMNvLogExtraBuffer1"
// #define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_2        "RMNvLogExtraBuffer2"
// #define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_3        "RMNvLogExtraBuffer3"
// #define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_4        "RMNvLogExtraBuffer4"
// #define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_5        "RMNvLogExtraBuffer5"
// #define NV_REG_STR_RM_NVLOG_EXTRA_BUFFER_6        "RMNvLogExtraBuffer6"
// Type DWORD
// Used to specify up to 6 additional logging buffers
// Encoding:
//   _BUFFER_FLAGS
//     x: uses NVLOG_BUFFER_FLAGS fields, for main nvlog buffer
//   _BUFFER_SIZE
//     n: Size of main buffer, in kilobytes


// Type DWORD
// This can be used for dumping NvLog buffers (in /var/log/vmkernel.log ), when
// we hit critical XIDs e.g 31/79.
#define NV_REG_STR_RM_DUMP_NVLOG                    "RMDumpNvLog"
#define NV_REG_STR_RM_DUMP_NVLOG_DEFAULT            (0x00000000)
#define NV_REG_STR_RM_DUMP_NVLOG_DISABLE            (0x00000000)
#define NV_REG_STR_RM_DUMP_NVLOG_ENABLE             (0x00000001)

// Type: Binary
// Comma separated list of XID values to suppress from the kernel log
// example:
// RmSuppressXidDump="43,31"
#define NV_REG_SUPPRESS_XID_DUMP                              "RmSuppressXidDump"
#define MAX_XID_SUPPRESS_KEY_LENGTH                           64

//
// Type DWORD
// RM external fabric management.
//
// RM currently uses nvlink core driver APIs which internally trigger
// link initialization and training. However, nvlink core driver now exposes a
// set of APIs for managing nvlink fabric externally (from user mode).
//
// When the regkey is enabled, RM will skip use of APIs which trigger
// link initialization and training. In that case, link training needs to be
// triggered externally.
//
#define NV_REG_STR_RM_EXTERNAL_FABRIC_MGMT               "RMExternalFabricMgmt"
#define NV_REG_STR_RM_EXTERNAL_FABRIC_MGMT_MODE          0:0
#define NV_REG_STR_RM_EXTERNAL_FABRIC_MGMT_MODE_ENABLE   (0x00000001)
#define NV_REG_STR_RM_EXTERNAL_FABRIC_MGMT_MODE_DISABLE  (0x00000000)


//
// Type DWORD
// BIT 1:0: All Data validation
// 0 - Default
// 1 - Validate the kernel data - enable all below
// 2 - Do not validate the kernel data - disable all below
// BIT 3:2: Buffer validation
// 0 - Default
// 1 - Validate the kernel buffers
// 2 - Do not validate the kernel buffers
// BIT 5:4: Handle validation
// 0 - Default
// 1 - Validate the handles
// 2 - Do not validate the handles
// BIT 7:6: Strict client validation
// 0 - Default
// 1 - Enable strict client validation
// 2 - Do not enable strict client validation
//
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION              "RmValidateClientData"
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_ALL                             1:0
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_ALL_DEFAULT              0x00000000
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_ALL_ENABLED              0x00000001
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_ALL_DISABLED             0x00000002
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_KERNEL_BUFFERS                  3:2
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_KERNEL_BUFFERS_DEFAULT   0x00000000
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_KERNEL_BUFFERS_ENABLED   0x00000001
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_KERNEL_BUFFERS_DISABLED  0x00000002
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_HANDLE                          5:4
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_HANDLE_DEFAULT           0x00000000
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_HANDLE_ENABLED           0x00000001
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_HANDLE_DISABLED          0x00000002
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_STRICT_CLIENT                   7:6
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_STRICT_CLIENT_DEFAULT    0x00000000
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_STRICT_CLIENT_ENABLED    0x00000001
#define NV_REG_STR_RM_CLIENT_DATA_VALIDATION_STRICT_CLIENT_DISABLED   0x00000002


//
// Type: DWORD
//
// This regkey configures thread priority boosting whenever
// the thread is holding a GPU lock.
//
#define NV_REG_STR_RM_PRIORITY_BOOST                          "RMPriorityBoost"
#define NV_REG_STR_RM_PRIORITY_BOOST_DISABLE                  0x00000000
#define NV_REG_STR_RM_PRIORITY_BOOST_ENABLE                   0x00000001
#define NV_REG_STR_RM_PRIORITY_BOOST_DEFAULT                  NV_REG_STR_RM_PRIORITY_BOOST_DISABLE


//
// Type: DWORD
//
// This regkey configures the delay (us) before a boosted thread is throttled
// down.
//
// Default value: 0 (Disable)
//
#define NV_REG_STR_RM_PRIORITY_THROTTLE_DELAY                 "RMPriorityThrottleDelay"
#define NV_REG_STR_RM_PRIORITY_THROTTLE_DELAY_DISABLE          0x00000000


//
// Type DWORD
// Enable support for CUDA Stream Memory Operations in user-mode applications.
//
// BIT 0:0 - Feature enablement
//  0 - disable feature (default)
//  1 - enable feature
//
#define NV_REG_STR_RM_STREAM_MEMOPS                 "RmStreamMemOps"
#define NV_REG_STR_RM_STREAM_MEMOPS_ENABLE          0:0
#define NV_REG_STR_RM_STREAM_MEMOPS_ENABLE_YES      1
#define NV_REG_STR_RM_STREAM_MEMOPS_ENABLE_NO       0


//
// Type DWORD: Enable read-only RMAPI locks for select interfaces
//
// Setting an interface to 0 will disable read-only API locks for that interface
// Setting an interface to 1 will enable read-only API locks for that interface,
// however, RM may still choose to take a read-write lock if it needs to.
//
#define NV_REG_STR_RM_READONLY_API_LOCK                            "RmRoApiLock"
#define NV_REG_STR_RM_READONLY_API_LOCK_ALLOC_RESOURCE             1:1
#define NV_REG_STR_RM_READONLY_API_LOCK_ALLOC_RESOURCE_DEFAULT    (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_ALLOC_RESOURCE_DISABLE    (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_ALLOC_RESOURCE_ENABLE     (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_FREE_RESOURCE              2:2
#define NV_REG_STR_RM_READONLY_API_LOCK_FREE_RESOURCE_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_FREE_RESOURCE_DISABLE     (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_FREE_RESOURCE_ENABLE      (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_MAP                        3:3
#define NV_REG_STR_RM_READONLY_API_LOCK_MAP_DEFAULT               (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MAP_DISABLE               (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MAP_ENABLE                (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_UNMAP                      4:4
#define NV_REG_STR_RM_READONLY_API_LOCK_UNMAP_DEFAULT             (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_UNMAP_DISABLE             (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_UNMAP_ENABLE              (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_MAP                  5:5
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_MAP_DEFAULT         (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_MAP_DISABLE         (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_MAP_ENABLE          (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_UNMAP                6:6
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_UNMAP_DEFAULT       (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_UNMAP_DISABLE       (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_INTER_UNMAP_ENABLE        (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_COPY                       7:7
#define NV_REG_STR_RM_READONLY_API_LOCK_COPY_DEFAULT              (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_COPY_DISABLE              (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_COPY_ENABLE               (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_SHARE                      8:8
#define NV_REG_STR_RM_READONLY_API_LOCK_SHARE_DEFAULT             (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_SHARE_DISABLE             (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_SHARE_ENABLE              (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_CTRL                       9:9
#define NV_REG_STR_RM_READONLY_API_LOCK_CTRL_DEFAULT              (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_CTRL_DISABLE              (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_CTRL_ENABLE               (0x00000001)


//
// Type DWORD: Enable read-only RMAPI locks for select modules
//
// Setting an interface to 0 will disable read-only API locks for that module
// Setting an interface to 1 will enable read-only API locks for that module,
// however, RM may still choose to take a read-write lock if it needs to.
//
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE                     "RmRoApiLockModule"
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_GPU_OPS              0:0
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_GPU_OPS_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_GPU_OPS_DISABLE     (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_GPU_OPS_ENABLE      (0x00000001)
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_WORKITEM             1:1
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_WORKITEM_DEFAULT    (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_WORKITEM_DISABLE    (0x00000000)
#define NV_REG_STR_RM_READONLY_API_LOCK_MODULE_WORKITEM_ENABLE     (0x00000001)


//
// Type DWORD: Enable read-only GPU locks for select modules
//
// Setting an interface to 0 will disable read-only GPU locks for that module
// Setting an interface to 1 will enable read-only GPU locks for that module,
// however, RM may still choose to take a read-write lock if it needs to.
//
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE                     "RmRoGpuLockModule"
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_GPU_OPS              0:0
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_GPU_OPS_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_GPU_OPS_DISABLE     (0x00000000)
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_GPU_OPS_ENABLE      (0x00000001)
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_WORKITEM             1:1
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_WORKITEM_DEFAULT    (0x00000000)
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_WORKITEM_DISABLE    (0x00000000)
#define NV_REG_STR_RM_READONLY_GPU_LOCK_MODULE_WORKITEM_ENABLE     (0x00000001)


// Mode for CACHEABLE rmapi control
// RMCTRL cache mode defined in ctrl0000system.h
#define NV_REG_STR_RM_CACHEABLE_CONTROLS             "RmEnableCacheableControls"

// Type DWORD
// This regkey forces for Maxwell+ that on FB Unload we wait for FB pull before issuing the
// L2 clean. WAR for bug 1032432
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL                                    "RmL2CleanFbPull"
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_ENABLED                            (0x00000000)
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_DISABLED                           (0x00000001)
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_DEFAULT                            (0x00000000)

//
// Type: DWORD
// This regkey overrides BL8, 16, and 24 kinds to only be of GENERIC_MEMORY or
// GENERIC_MEMORY_COMPRESSIBLE kinds.
// 0 - No override
// > 0 - Override memkind to GMK
//       bit 0: override BL8 type
//       bit 1: override BL16 type
//       bit 2: override BL24 type
// ex. 0x00001000 means override all types
#define NV_REG_STR_RM_OVERRIDE_TO_GMK               "RMOverrideToGMK"
#define NV_REG_STR_RM_OVERRIDE_TO_GMK_DISABLED      (0x00000000)
#define NV_REG_STR_RM_OVERRIDE_TO_GMK_BL8           (0x00000001)
#define NV_REG_STR_RM_OVERRIDE_TO_GMK_BL16          (0x00000002)
#define NV_REG_STR_RM_OVERRIDE_TO_GMK_BL24          (0x00000004)
#define NV_REG_STR_RM_OVERRIDE_TO_GMK_ALL           (0x00000007)

// Enable backtrace dumping at assertion failure.
// If physical RM or RCDB is unavailable, then this regkey controls the behaviour of backtrace
// printing.
// 0: disable
// 1 (default): only print unique backtraces, identified by instruction pointer of the failed assert
// 2: print all
#define NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE         "RmPrintAssertBacktrace"
#define NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_DISABLE 0
#define NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_UNIQUE  1
#define NV_REG_STR_RM_PRINT_ASSERT_BACKTRACE_ENABLE  2


//
// Type DWORD
// Used to enable no locking on copy
//
#define NV_REG_STR_RM_PARAM_COPY_NO_LOCK                      "RMParamCopyNoLock"

//
// Type DWORD
// Used to control RM API lock aging for low priority acquires.
// If 0, low priority acquires (e.g. from cleanup when a process dies)
// are disabled and treated like regular ones.
// Otherwise, they will yield the lock this many times to the higher priority
// threads before proceeding.
// Off by default; 3 would be a good starting value if the feature is desired.
//
#define NV_REG_STR_RM_LOCKING_LOW_PRIORITY_AGING              "RMLockingLowPriorityAging"

//
// Type DWORD
// This regkey restricts profiling capabilities (creation of profiling objects
// and access to profiling-related registers) to admin only.
// 0 - (default - disabled)
// 1 - Enables admin check
//
#define NV_REG_STR_RM_PROFILING_ADMIN_ONLY              "RmProfilingAdminOnly"
#define NV_REG_STR_RM_PROFILING_ADMIN_ONLY_FALSE        0x00000000
#define NV_REG_STR_RM_PROFILING_ADMIN_ONLY_TRUE         0x00000001


#define NV_REG_STR_GPU_BROKEN_FB              "nvBrokenFb"
#define NV_REG_STR_GPU_BROKEN_FB_ALL_OKAY                       0x00000000
#define NV_REG_STR_GPU_BROKEN_FB_ALL_BROKEN                     0xffffffff
#define NV_REG_STR_GPU_BROKEN_FB_DEFAULT                        NV_REG_STR_GPU_BROKEN_FB_ALL_OKAY
#define NV_REG_STR_GPU_BROKEN_FB_DEFAULT_GF100_A01              NV_REG_STR_GPU_BROKEN_FB_MEMORY_BROKEN
#define NV_REG_STR_GPU_BROKEN_FB_MEMORY                         0:0
#define NV_REG_STR_GPU_BROKEN_FB_MEMORY_OKAY                    0x00000000
#define NV_REG_STR_GPU_BROKEN_FB_MEMORY_BROKEN                  0x00000001
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_CPU                    1:1
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_CPU_OKAY               0x00000000
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_CPU_BROKEN             0x00000001
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_PMU                    2:2
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_PMU_OKAY               0x00000000
#define NV_REG_STR_GPU_BROKEN_FB_REG_VIA_PMU_BROKEN             0x00000001
// Type DWORD
// _ALL_OKAY:       FB is not broken.  All is okay.
// _ALL_BROKEN:     FB is broken and no software will try to use it.
// _MEMORY:         Memory itself can/cannot be accessed.  (PDB_PROP_GPU_BROKEN_FB property)
// _REG_VIA_CPU:    CPU can/cannot access FBPA/FBIO registers.  (PDB_PROP_GPU_BROKEN_FB_REG_VIA_CPU property)
// _REG_VIA_PMU:    PMU can/cannot access FBPA/FBIO registers.  (PDB_PROP_GPU_BROKEN_FB_REG_VIA_PMU property)
// FBPA/FBIO register addresses are defined by gpuIsBrokenFbReg().
// Note that if the CPU and the PMU can't access registers, then memory isn't going to work either.
// In other words, the only even number that makes sense for this regkey is zero.
// Default depends on the chip and mask revision.

#define NV_REG_STR_OVERRIDE_FB_SIZE                                 "OverrideFbSize"
// Type Dword
// Encoding Numeric Value
// Size in MB
// Used to reduce FB for testing memory management
//
#define NV_REG_STR_OVERRIDE_FB_SIZE_DEFAULT                         0

//
// TYPE DWORD
// This regkey helps increase the size of RM reserved region.
// Exposed to clients for bug 2404337.
// Note: In GSP builds this key applies to the kernel (CPU) RM only.
//
#define NV_REG_STR_RM_INCREASE_RSVD_MEMORY_SIZE_MB             "RMIncreaseRsvdMemorySizeMB"
#define NV_REG_STR_RM_INCREASE_RSVD_MEMORY_SIZE_MB_DEFAULT     0x0

// TYPE Dword
// Determines whether or not RM reserved space should be increased.
// 1 - Increases RM reserved space
// 0 - (default) Keeps RM reserved space as it is.

#define  NV_REG_STR_RM_DISABLE_SCRUB_ON_FREE          "RMDisableScrubOnFree"
// Type DWORD
// Encoding 0 (default) - Scrub on free
//          1           - Disable Scrub on Free

#define  NV_REG_STR_RM_DISABLE_FAST_SCRUBBER          "RMDisableFastScrubber"
// Type DWORD
// Encoding 0 (default) - Enable Fast Scrubber
//          1           - Disable Fast Scrubber

//
// Type DWORD
// Controls enable of PMA memory management instead of existing legacy
// RM FB heap manager.
//
#define NV_REG_STR_RM_ENABLE_PMA                "RMEnablePMA"
#define NV_REG_STR_RM_ENABLE_PMA_YES            (0x00000001)
#define NV_REG_STR_RM_ENABLE_PMA_NO             (0x00000000)

//
// Type DWORD
// Controls management of client page tables by PMA on MODS.
// Default enable. MODS will use regkey to override to disable feature.
//
#define NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES           "RMEnablePmaManagedPtables"
#define NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES_YES       (0x00000001)
#define NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES_NO        (0x00000000)
#define NV_REG_STR_RM_ENABLE_PMA_MANAGED_PTABLES_DEFAULT   (0x00000001)

//
// TYPE DWORD
// This regkey enables localized memory changes
// 0 - (default disabled)
// 1 - Enables localized memory changes
//
#define NV_REG_STR_RM_LOCALIZED_MEMORY                 "RmEnableLocalizedMemory"
#define NV_REG_STR_RM_LOCALIZED_MEMORY_DISABLE         0x00000000
#define NV_REG_STR_RM_LOCALIZED_MEMORY_ENABLE          0x00000001
#define NV_REG_STR_RM_LOCALIZED_MEMORY_DEFAULT         NV_REG_STR_RM_LOCALIZED_MEMORY_ENABLE

//
// Type DWORD
// Disable global CeUtils instance creation after fifo scheduling enablement
//
#define NV_REG_STR_DISABLE_GLOBAL_CE_UTILS             "RmDisableGlobalCeUtils"
#define NV_REG_STR_DISABLE_GLOBAL_CE_UTILS_YES         (0x00000001)
#define NV_REG_STR_DISABLE_GLOBAL_CE_UTILS_NO          (0x00000000)

#define  NV_REG_STR_RM_SCRUB_BLOCK_SHIFT               "RMScrubBlockShift"
// Type DWORD
// Encoding Numeric Value
// A value in the range 12 to 20 represents logbase2 of maxBlockSize for heap
// scrubber. Any other value will be defaulted to 16 i.e. maxBlockSize = 64KB.

#define NV_REG_STR_RM_INST_VPR                              "RMInstVPR"
// Type DWORD
// Encoding: takes effect for allocations in VIDEO memory
//    TRUE       Make allocation in protected region
//    FALSE      Make allocation in non-protected region (default)
//
#define NV_REG_STR_RM_INST_VPR_INSTBLK                      0:0             // Instance block
#define NV_REG_STR_RM_INST_VPR_INSTBLK_FALSE                (0x00000000)
#define NV_REG_STR_RM_INST_VPR_INSTBLK_TRUE                 (0x00000001)
#define NV_REG_STR_RM_INST_VPR_RAMFC                        1:1             // RAMFC save area
#define NV_REG_STR_RM_INST_VPR_RAMFC_FALSE                  (0x00000000)
#define NV_REG_STR_RM_INST_VPR_RAMFC_TRUE                   (0x00000001)
#define NV_REG_STR_RM_INST_VPR_RUNLIST                      2:2             // Runlist
#define NV_REG_STR_RM_INST_VPR_RUNLIST_FALSE                (0x00000000)
#define NV_REG_STR_RM_INST_VPR_RUNLIST_TRUE                 (0x00000001)
#define NV_REG_STR_RM_INST_VPR_MMU_READ                     3:3             // MMU Debug Read
#define NV_REG_STR_RM_INST_VPR_MMU_READ_FALSE               (0x00000000)
#define NV_REG_STR_RM_INST_VPR_MMU_READ_TRUE                (0x00000001)
#define NV_REG_STR_RM_INST_VPR_MMU_WRITE                    4:4             // MMU Debug Read
#define NV_REG_STR_RM_INST_VPR_MMU_WRITE_FALSE              (0x00000000)
#define NV_REG_STR_RM_INST_VPR_MMU_WRITE_TRUE               (0x00000001)

#define NV_REG_STR_RM_GPU_SURPRISE_REMOVAL                   "RMGpuSurpriseRemoval"
// Type DWORD
// Encoding boolean
// If set, this will cause RM mark GPU as lost when it detects 0xFF from register
// access.

#define NV_REG_STR_RM_BLACKLIST_ADDRESSES                "RmBlackListAddresses"
// Type BINARY:
// struct
// {
//     NvU64 addresses[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];
// };

#define NV_REG_STR_RM_NUM_FIFOS                        "RmNumFifos"
// Type Dword
// Override number of fifos (channels) on NV4X
#define NV_REG_STR_RM_NUM_FIFOS_COMPAT                 0x00000020
#define NV_REG_STR_RM_NUM_FIFOS_EXTENDED               0x00000200

#define NV_REG_STR_RM_SUPPORT_USERD_MAP_DMA     "RMSupportUserdMapDma"
// Type DWORD
// Encoding: Boolean
// If set, allow MapMemoryDma calls to be made on channel objects

#define NV_REG_STR_SECONDARY_BUS_RESET_ENABLED          "RMSecBusResetEnable"
// Type DWORD
// Encoding boolean
// Default FALSE

#define NV_REG_STR_FORCE_PCIE_CONFIG_SAVE               "RMForcePcieConfigSave"
// Type DWORD
// Encoding boolean
// Default FALSE

#define NV_REG_STR_RM_PCIE_FLR_DEVINIT_TIMEOUT_SCALE             "RMPcieFlrDevinitTimeout"
#define NV_REG_STR_RM_PCIE_FLR_DEVINIT_TIMEOUT_SCALE_MIN_ALLOWED  1
#define NV_REG_STR_RM_PCIE_FLR_DEVINIT_TIMEOUT_SCALE_MAX_ALLOWED  4
// Type DWORD
// Regkey to change FLR devinit timeout value. Increase in scale value increases
// the timeout value and vice versa.
// Scale value has to be greater than 0 since flr devinit timeout can't be 0
// Scale value for now is limited to 4 which translates to maximum of
// 3.6seconds(900ms*4) timeout value.
//

#define NV_REG_STR_RM_PCIE_FLR_POLICY                  "RMPcieFLRPolicy"
#define NV_REG_STR_RM_PCIE_FLR_POLICY_DEFAULT          0
#define NV_REG_STR_RM_PCIE_FLR_POLICY_FORCE_DISABLE    1
// Type DWORD
// Regkey to force disable Function Level Reset
// Note that we don't want to provision for force enabling FLR since as per current design -
// For Pre-Turing boards, FLR will be strictly disabled since it's not supported in HW
// Default policy could be different for different boards though

// Type DWORD
// Encoding Numeric Value
// Overrides chipset-based P2P configurations.
// Only be used to test on internal issues
//
// P2P reads:
//  0 - Do not allow P2P reads
//  1 - Allow P2P reads
//  2 - Do not override chipset-selected config (default)
// P2P writes:
//  0 - Do not allow P2P writes
//  1 - Allow P2P writes
//  2 - Do not override chipset-selected config (default)
// P2P atomics:
//  0 - Do not allow P2P atomics
//  1 - Allow P2P atomics
//  2 - Do not override chipset-selected config (default)
//
#define NV_REG_STR_CL_FORCE_P2P                              "ForceP2P"
#define NV_REG_STR_CL_FORCE_P2P_READ                         1:0
#define NV_REG_STR_CL_FORCE_P2P_READ_DISABLE                 0x00000000
#define NV_REG_STR_CL_FORCE_P2P_READ_ENABLE                  0x00000001
#define NV_REG_STR_CL_FORCE_P2P_READ_DEFAULT                 0x00000002
#define NV_REG_STR_CL_FORCE_P2P_WRITE                        5:4
#define NV_REG_STR_CL_FORCE_P2P_WRITE_DISABLE                0x00000000
#define NV_REG_STR_CL_FORCE_P2P_WRITE_ENABLE                 0x00000001
#define NV_REG_STR_CL_FORCE_P2P_WRITE_DEFAULT                0x00000002
#define NV_REG_STR_CL_FORCE_P2P_ATOMICS                      9:8
#define NV_REG_STR_CL_FORCE_P2P_ATOMICS_DISABLE              0x00000000
#define NV_REG_STR_CL_FORCE_P2P_ATOMICS_ENABLE               0x00000001
#define NV_REG_STR_CL_FORCE_P2P_ATOMICS_DEFAULT              0x00000002

//
// Type DWORD
// Use this regkey to force RM to pick a P2P type. HW has to support the picked TYPE to take effect.
// e.g., TYPE_BAR1P2P will not work if HW does not support it. A call to create NV50_P2P object will
// will fail in such a case.
//
// TYPE_DEFAULT let RM to choose a P2P type. The priority is:
//              C2C > NVLINK > mailbox P2P > BAR1P2P
//
// TYPE_C2C to use C2C P2P if it supports
// TYPE_NVLINK to use NVLINK P2P, including INDIRECT_NVLINK_P2P if it supports
// TYPE_PCIEP2P to use PCIE P2P if it supports. To be used in conjunction with
//              NV_REG_STR_RM_PCIEP2P_TYPE.
//
#define NV_REG_STR_RM_FORCE_P2P_TYPE                           "RMForceP2PType"
#define NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT                   (0x00000000)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_PCIEP2P                   (0x00000001)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_NVLINK                    (0x00000002)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_C2C                       (0x00000003)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_MAX                       NV_REG_STR_RM_FORCE_P2P_TYPE_C2C

//
// Type: DWORD
// Allows the choice of which PCIe P2P method to use
//
// TYPE_DEFAULT   mailbox P2P is to be used by default until UVM supports BAR1 P2P
//
// TYPE_MAILBOX   forces mailbox P2P to be used if supported
// TYPE_BAR1      forces BAR1 P2P to be used if supported
// TYPE_AUTO      allows automatic selection of BAR1/mailbox depending on if BAR1P2P is supported
//
#define NV_REG_STR_RM_PCIEP2P_TYPE                          "RMPcieP2PType"
#define NV_REG_STR_RM_PCIEP2P_TYPE_MAILBOX                  (0x00000000)
#define NV_REG_STR_RM_PCIEP2P_TYPE_BAR1                     (0x00000001)
#define NV_REG_STR_RM_PCIEP2P_TYPE_AUTO                     (0x00000002)
#define NV_REG_STR_RM_PCIEP2P_TYPE_DEFAULT                  NV_REG_STR_RM_PCIEP2P_TYPE_MAILBOX

//
// Type: DWORD
// Enables/Disables the WAR for bug 1630288 where we disable 3rd-party peer mappings
// Disabled by default
//
#define NV_REG_STR_PEERMAPPING_OVERRIDE                         "PeerMappingOverride"
#define NV_REG_STR_PEERMAPPING_OVERRIDE_DEFAULT                 0

#define NV_REG_STR_P2P_MAILBOX_CLIENT_ALLOCATED                     "P2PMailboxClientAllocated"
#define NV_REG_STR_P2P_MAILBOX_CLIENT_ALLOCATED_FALSE               0
#define NV_REG_STR_P2P_MAILBOX_CLIENT_ALLOCATED_TRUE                1
// Type Dword
// Overrides the P2P Mailbox allocation policy
// For testing only
// 0 - P2P Mailbox area is allocated by RM
// 1 - P2P Mailbox area is not allocated by RM, but by the client.

#define NV_REG_STR_RM_MAP_P2P_PEER_ID               "RMP2PPeerId"
// Type DWORD
// Encoding:
//      Peer ID to use when mapping p2p to peer subdevice in p2p loopback mode
// Default: RM takes care of assigning peer ID.

#define NV_REG_STR_OVERRIDE_GPU_NUMA_NODE_ID    "RMOverrideGpuNumaNodeId"
// Type DWORD:
// Encoding -- NvS32
// Override GPU NUMA Node ID assigned by OS

#define NV_REG_STR_RESTORE_BAR1_SIZE_BUG_3249028_WAR         "RMBar1RestoreSize"
#define NV_REG_STR_RESTORE_BAR1_SIZE_BUG_3249028_TRUE        (0x00000001)
#define NV_REG_STR_RESTORE_BAR1_SIZE_BUG_3249028_FALSE       (0x00000000)
// Type DWORD:
// Encoding -- Boolean
// Check if BAR1 size has been restored correctly by SBIOS across power transitions
// Default: enabled for Ampere and up
//

//
// Type DWORD
// Numa allocations allow for skipping reclaim less than a specified memory occupancy threshold.
// This override allows for its tuning, value supplied here shall indicate a percent of free memory
// less than which GFP_RECLAIM flag will be dropped.
//
#define NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE     "RmNumaAllocSkipReclaimPercent"
#define NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_DEFAULT    4
#define NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_DISABLED   0
#define NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_MIN        0
#define NV_REG_STR_RM_NUMA_ALLOC_SKIP_RECLAIM_PERCENTAGE_MAX      100

//
// Disable 64KB BAR1 mappings
// 0 - Disable 64KB BAR1 mappings
// 1 - Force/Enable 64KB BAR1 mappings
//
#define NV_REG_STR_RM_64KB_BAR1_MAPPINGS                            "RM64KBBAR1Mappings"
#define NV_REG_STR_RM_64KB_BAR1_MAPPINGS_ENABLED                    0x00000001
#define NV_REG_STR_RM_64KB_BAR1_MAPPINGS_DISABLED                   0x00000000

#define NV_REG_STR_RM_BAR1_APERTURE_SIZE_MB                  "RMBar1ApertureSizeMB"
// Type DWORD
// Encoding Numeric Value
// Overrides the size of the BAR1 aperture. Used to shrink BAR1. It cannot be
// greater than the physical size of BAR1.

// Type DWORD
// Encoding Numeric Value
// Forces the entire BAR1 to be statically allocated.
//
// DISABLE will force disable static BAR1
// ENABLE will force the static BAR1 to be enabled if there is sufficient BAR1
//        to map all of FB once or fail initialization otherwise. This mode does
//        not take into account other expected BAR1 mappings and may lead to
//        BAR1 exhaustion later. Use with caution.
// AUTO   will only map static BAR1 if static BAR1 size is calculated to be big enough
//        to map all of FB once plus a calculated amount for other expected BAR1 mappings
//
#define NV_REG_STR_RM_FORCE_STATIC_BAR1                          "RMForceStaticBar1"
#define NV_REG_STR_RM_FORCE_STATIC_BAR1_DISABLE                   0x00000000
#define NV_REG_STR_RM_FORCE_STATIC_BAR1_ENABLE                    0x00000001
#define NV_REG_STR_RM_FORCE_STATIC_BAR1_AUTO                      0x00000002
#define NV_REG_STR_RM_FORCE_STATIC_BAR1_MAX                       0x00000003
#define NV_REG_STR_RM_FORCE_STATIC_BAR1_DEFAULT                   NV_REG_STR_RM_FORCE_STATIC_BAR1_DISABLE

#define NV_REG_STR_RM_BAR2_APERTURE_SIZE_MB                  "RMBar2ApertureSizeMB"
// Type DWORD
// Encoding Numeric Value
// Overrides the size of the BAR2 aperture.  Cannot be greater than the
// physical size of BAR2 available to RM (which may be less than the total size
// of BAR2).  When this regkey is present we cap the total aperture size to the
// RM aperture size. This can result in undefined beahvior in environments that
// rely on a virtual bar2 aperture shared between RM and VBIOS for VESA support.

//
// This is used to control C2C low power features.
//
#define NV_REG_STR_RM_LPWR_C2C_STATE                         "RmLpwrC2CState"
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL3            0:0
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL3_DISABLE    0
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL3_ENABLE     1
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL3_DEFAULT    NV_REG_STR_RM_LPWR_C2C_STATE_CL3_DISABLE
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL4            1:1
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL4_DISABLE    0
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL4_ENABLE     1
#define NV_REG_STR_RM_LPWR_C2C_STATE_CL4_DEFAULT    NV_REG_STR_RM_LPWR_C2C_STATE_CL4_DISABLE

//
// Type DWORD:
// This regkey overrides C2C CL3/CL4 idle thresholds set by vbios.
// Encoding - Value of CL3/CL4 idle threshold in microseconds. Zero means that vbios values will be used.
//
#define NV_REG_STR_RM_LPWR_C2C_CL3_IDLE_THRESHOLD_US          "RmLpwrC2cCl3IdleThresholdUs"
#define NV_REG_STR_RM_LPWR_C2C_CL4_IDLE_THRESHOLD_US          "RmLpwrC2cCl4IdleThresholdUs"

#if defined(DEVELOP) || defined(DEBUG) || (defined(RMCFG_FEATURE_MODS_FEATURES) && RMCFG_FEATURE_MODS_FEATURES)
//
// TYPE DWORD
// This setting will override the BAR1 Big page size
// This is used for interop testing for MODS
//
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE                  "RMSetBAR1AddressSpaceBigPageSize"
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE_64k              (64 * 1024)
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE_128k             (128 * 1024)
#endif //DEVELOP || DEBUG || MODS_FEATURES

// This regkey is to disable coherent path CPU->Nvlink/C2C->FB and force BAR path.
#define NV_REG_STR_RM_FORCE_BAR_PATH            "RMForceBarPath"
// Type DWORD
// Encoding 0 (default) - Enable Coherent C2C/NvLink Path
//          1           - Force BAR Path

//
// Type: Dword
// Encoding:
// 0 - client RM allocated context buffer feature is disabled
// 1 - client RM allocated context buffer feature is enabled
//
#define NV_REG_STR_RM_CLIENT_RM_ALLOCATED_CTX_BUFFER            "RMSetClientRMAllocatedCtxBuffer"
#define NV_REG_STR_RM_CLIENT_RM_ALLOCATED_CTX_BUFFER_DISABLED   0x00000000
#define NV_REG_STR_RM_CLIENT_RM_ALLOCATED_CTX_BUFFER_ENABLED    0x00000001

//
// Type: Dword
// Encoding:
// 0 - Split VA space management between server/client RM is disabled
// 1 - Split VA space management between server/client RM is enabled
//
#define NV_REG_STR_RM_SPLIT_VAS_MGMT_SERVER_CLIENT_RM              "RMSplitVasMgmtServerClientRm"
#define NV_REG_STR_RM_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_DISABLED                         0x00000000
#define NV_REG_STR_RM_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_ENABLED                          0x00000001

//
// Type: Dword
// Encoding:
// 0 - Disable PE FIRO Buffer
// 1 - Enable PE FIRO Buffer
// Default: disabled (0x0)
//
#define NV_REG_STR_RM_ENABLE_PE_FIRO_BUFFER                      "RMEnablePeFiroBuffer"
#define NV_REG_STR_RM_ENABLE_PE_FIRO_BUFFER_DISABLED                         0x00000000
#define NV_REG_STR_RM_ENABLE_PE_FIRO_BUFFER_ENABLED                          0x00000001

//
// Restrict the VA range to be <= @ref VASPACE_SIZE_FERMI.
// Used in cases where some engines support 49 bit VA and some don't.
// Ignored if NVOS32_ALLOC_FLAGS_USE_BEGIN_END (DDMA_ALLOC_VASPACE_USE_RANGE)  or
// NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE (DMA_ALLOC_VASPACE_VA_FIXED) is set.
// Default: OFF (0x0)
// Type:    DWORD
//
#define NV_REG_STR_RM_RESTRICT_VA_RANGE                     "RMRestrictVARange"
#define NV_REG_STR_RM_RESTRICT_VA_RANGE_DEFAULT             (0x0)
#define NV_REG_STR_RM_RESTRICT_VA_RANGE_ON                  (0x1)

#define NV_REG_STR_RESERVE_PTE_SYSMEM_MB                "RmReservePteSysmemMB"
// Type DWORD:
// Encoding -- Value = 0 -> Do not reserve sysmem for PTEs (default)
//             Value > 0 -> Reserve ValueMB for PTEs when we run out of video and system memory
//

// Type DWORD
// Contains the sysmem page size.
#define NV_REG_STR_RM_SYSMEM_PAGE_SIZE                  "RMSysmemPageSize"

//
// Allows pages that are aligned to large page boundaries to be mapped as large
// pages.
//
#define NV_REG_STR_RM_ALLOW_SYSMEM_LARGE_PAGES "RMAllowSysmemLargePages"

#define NV_REG_STR_FERMI_BIG_PAGE_SIZE                  "RMFermiBigPageSize"
#define NV_REG_STR_FERMI_BIG_PAGE_SIZE_64KB             (64 * 1024)
#define NV_REG_STR_FERMI_BIG_PAGE_SIZE_128KB            (128 * 1024)

//
// TYPE DWORD
// This setting will disable big page size per address space
//
#define NV_REG_STR_RM_DISABLE_BIG_PAGE_PER_ADDRESS_SPACE                     "RmDisableBigPagePerAddressSpace"
#define NV_REG_STR_RM_DISABLE_BIG_PAGE_PER_ADDRESS_SPACE_FALSE               (0x00000000)
#define NV_REG_STR_RM_DISABLE_BIG_PAGE_PER_ADDRESS_SPACE_TRUE                (0x00000001)

#define NV_REG_STR_RM_DISABLE_NONCONTIGUOUS_ALLOCATION          "RMDisableNoncontigAlloc"
#define NV_REG_STR_RM_DISABLE_NONCONTIGUOUS_ALLOCATION_FALSE    (0x00000000)
#define NV_REG_STR_RM_DISABLE_NONCONTIGUOUS_ALLOCATION_TRUE     (0x00000001)
// Type DWORD:
// Encoding -- Boolean
// Disable noncontig vidmem allocation
//

#define NV_REG_STR_RM_FBSR_PAGED_DMA                         "RmFbsrPagedDMA"
#define NV_REG_STR_RM_FBSR_PAGED_DMA_ENABLE                  1
#define NV_REG_STR_RM_FBSR_PAGED_DMA_DISABLE                 0
#define NV_REG_STR_RM_FBSR_PAGED_DMA_DEFAULT                 NV_REG_STR_RM_FBSR_PAGED_DMA_DISABLE
// Type Dword
// Encoding Numeric Value
// Enable the Paged DMA mode for FBSR
// 0 - Disable (default)
// 1 - Enable

#define NV_REG_STR_RM_FBSR_FILE_MODE                   "RmFbsrFileMode"
#define NV_REG_STR_RM_FBSR_FILE_MODE_ENABLE            1
#define NV_REG_STR_RM_FBSR_FILE_MODE_DISABLE           0
#define NV_REG_STR_RM_FBSR_FILE_MODE_DEFAULT           NV_REG_STR_RM_FBSR_FILE_MODE_DISABLE
// Type Dword
// Encoding Numeric Value
// Enable the File based power saving mode for Linux
// 0 - Disable (default)
// 1 - Enable

#define NV_REG_STR_RM_FBSR_WDDM_MODE                         "RmFbsrWDDMMode"
#define NV_REG_STR_RM_FBSR_WDDM_MODE_ENABLE                  1
#define NV_REG_STR_RM_FBSR_WDDM_MODE_DISABLE                 0
#define NV_REG_STR_RM_FBSR_WDDM_MODE_DEFAULT                 NV_REG_STR_RM_FBSR_WDDM_MODE_DISABLE
// Type Dword
// Encoding Numeric Value
// Enable the WDDM power saving mode for FBSR
// 0 - Disable (default)
// 1 - Enable

// Type DWORD: Disables HW fault buffers on Pascal+ chips
// Encoding : 1 -- TRUE
//          : 0 -- False
//          : Default -- False
#define NV_REG_STR_RM_DISABLE_HW_FAULT_BUFFER            "RmDisableHwFaultBuffer"
#define NV_REG_STR_RM_DISABLE_HW_FAULT_BUFFER_TRUE        0x00000001
#define NV_REG_STR_RM_DISABLE_HW_FAULT_BUFFER_FALSE       0x00000000
#define NV_REG_STR_RM_DISABLE_HW_FAULT_BUFFER_DEFAULT     0x00000000

//
// Type: DWORD
// Encoding:
// 3 - Enable interrupt-based FECS context switch logging with bottom-half/APC fall-back
// 2 - Enable interrupt-based FECS context switch logging without bottom-half/APC fall-back
// 1 - Enable periodic FECS context switch logging
// 0 - Disable FECS context switch logging
//
// Note: Interrupt-based logging and periodic logging are mutually exclusive
//
#define NV_REG_STR_RM_CTXSW_LOG                               "RMCtxswLog"
#define NV_REG_STR_RM_CTXSW_LOG_DISABLE                       0x00000000
#define NV_REG_STR_RM_CTXSW_LOG_ENABLE                        0x00000001
#define NV_REG_STR_RM_CTXSW_LOG_ENABLE_INTR                   0x00000002
#define NV_REG_STR_RM_CTXSW_LOG_ENABLE_INTR_APC               0x00000003
#define NV_REG_STR_RM_CTXSW_LOG_DEFAULT                       NV_REG_STR_RM_CTXSW_LOG_DISABLE

// Type DWORD: Indicates if enabling video event tracing
//
// 0    - Disables Video event trace usage (default)
// > 0  - Enable video event trace and define sizes for different buffers
//        bit 16 - 30: sizes of the event buffer in 4K pages
//        bit 31 - 31: Enable always logging:
//                     By default, video engines only log video events when there is
//                     at least one eventbuffer bound and enabled. If this flag is set,
//                     video engines will always log events even without a consumer. This
//                     is helpful for debugging purposes.
//        Example: 0x01000000 means 1MB event buffer.
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE                                 "RmVideoEventTrace"
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE_DISABLED                        (0x00000000)
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE_EVENT_BUFFER_SIZE_IN_4k         30:16
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE_ALWAYS_LOG                      31:31
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE_ALWAYS_LOG_DISABLED             0x00000000
#define NV_REG_STR_RM_VIDEO_EVENT_TRACE_ALWAYS_LOG_ENABLED              0x00000001

//
// Type: DWORD
//
// This regkey configures the maximum number of records that can be
// processed per DPC when using interrupt-based ctxsw logging
#define NV_REG_STR_RM_CTXSW_LOG_RECORDS_PER_INTR              "RMCtxswLogMaxRecordsPerIntr"
#define NV_REG_STR_RM_CTXSW_LOG_RECORDS_PER_INTR_DEFAULT      0x30

//
// Type: DWORD
// Encoding:
// 0 - Disable more detailed debug INTR logs
// 1 - Enable more detailed debug INTR logs
//
#define NV_REG_STR_RM_INTR_DETAILED_LOGS                      "RMIntrDetailedLogs"
#define NV_REG_STR_RM_INTR_DETAILED_LOGS_DISABLE              0x00000000
#define NV_REG_STR_RM_INTR_DETAILED_LOGS_ENABLE               0x00000001

#define NV_REG_STR_RM_INTR_LOCKING_MODE              "RMIntrLockingMode"
// Type DWORD
// Encoding enum
// Overrides what INTR Locking Mode is in use.
// Default 0
#define NV_REG_STR_RM_INTR_LOCKING_MODE_DEFAULT               (0x00000000)
#define NV_REG_STR_RM_INTR_LOCKING_MODE_INTR_MASK             (0x00000001)

#define NV_REG_INTERNAL_PANEL_DISCONNECTED         "RMInternalPanelDisconnected"
#define NV_REG_INTERNAL_PANEL_DISCONNECTED_DISABLE  0x00000000
#define NV_REG_INTERNAL_PANEL_DISCONNECTED_ENABLE   0x00000001
#define NV_REG_INTERNAL_PANEL_DISCONNECTED_DEFAULT  RM_REG_INTERNAL_PANEL_DISCONNECTED_DISABLE

#define NV_REG_STR_RM_PER_INTR_DPC_QUEUING        "RMDisablePerIntrDPCQueueing"
// Type DWORD
// This regkey is used to disable per interrupt DPC queuing.
// 0: Enable Per interrupt DPC Queuing
// 1: Disable Per interrupt DPC Queuing

#define NV_REG_STR_INTR_STUCK_THRESHOLD         "RM654663"
// Type DWORD
// Encoding NvU32
// Number of iterations to see an interrupt in succession before considering it
// "stuck."
// Default - See INTR_STUCK_THRESHOLD


#define NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR  "RMProcessNonStallIntrInLocklessIsr"

// Type: DWORD
// Enables/Disables processing of non-stall interrupts in lockless ISR for
// Linux only.
// Non-stall interrupts are processed by the function
// intrServiceNonStall_HAL(pIntr,pGpu, TRUE /* bProcess*/); where bProcess is TRUE which
// means that event list will be traversed to notify clients registered for it.
// Disabled by default
//

#define NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR_DISABLE      0x00000000
#define NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR_ENABLE       0x00000001

#define NV_REG_STR_RM_ROBUST_CHANNELS                       "RmRobustChannels"
#define NV_REG_STR_RM_ROBUST_CHANNELS_ENABLE                 0x00000001
#define NV_REG_STR_RM_ROBUST_CHANNELS_DISABLE                0x00000000
#define NV_REG_STR_RM_ROBUST_CHANNELS_DEFAULT                NV_REG_STR_RM_ROBUST_CHANNELS_DISABLE

#define NV_REG_STR_RM_RC_WATCHDOG                           "RmRcWatchdog"
#define NV_REG_STR_RM_RC_WATCHDOG_ENABLE                    0x00000001
#define NV_REG_STR_RM_RC_WATCHDOG_DISABLE                   0x00000000
#define NV_REG_STR_RM_RC_WATCHDOG_DEFAULT                   NV_REG_STR_RM_RC_WATCHDOG_ENABLE

#define NV_REG_STR_RM_WATCHDOG_TIMEOUT                      "RmWatchDogTimeOut"
#define NV_REG_STR_RM_WATCHDOG_TIMEOUT_LOW                   0x00000007
#define NV_REG_STR_RM_WATCHDOG_TIMEOUT_HI                    0x0000000C
#define NV_REG_STR_RM_WATCHDOG_TIMEOUT_DEFAULT               NV_REG_STR_RM_WATCHDOG_TIMEOUT_LOW

#define NV_REG_STR_RM_WATCHDOG_INTERVAL                      "RmWatchDogInterval"
#define NV_REG_STR_RM_WATCHDOG_INTERVAL_LOW                   0x00000007
#define NV_REG_STR_RM_WATCHDOG_INTERVAL_HI                    0x0000000C
#define NV_REG_STR_RM_WATCHDOG_INTERVAL_DEFAULT               NV_REG_STR_RM_WATCHDOG_INTERVAL_LOW

// Enable/Disable watchcat in GSP-RM partition
// Default is Enabled
#define NV_REG_STR_TASK_RM_WATCHCAT                          "RmEnableRmTaskWatchcat"
#define NV_REG_STR_TASK_RM_WATCHCAT_ENABLE                    0x00000001
#define NV_REG_STR_TASK_RM_WATCHCAT_DISABLE                   0x00000000
#define NV_REG_STR_TASK_RM_WATCHCAT_DEFAULT                   NV_REG_STR_TASK_RM_WATCHCAT_ENABLE

// Enable/Disable watchcat in GSP-Plugin for Guest RPC
// Default is Enabled
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT                     "RmEnableGspPluginWatchcat"
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_ENABLE              0x00000001
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_DISABLE             0x00000000
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_DEFAULT             NV_REG_STR_RM_GSP_VGPU_WATCHCAT_ENABLE

// Set watchcat timeout value in GSP-Plugin for Guest RPC
// Default is 10 seconds
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_TIMEOUT             "RmGspPluginWatchcatTimeOut"
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_TIMEOUT_MIN         0x0000000A
#define NV_REG_STR_RM_GSP_VGPU_WATCHCAT_TIMEOUT_DEFAULT     NV_REG_STR_RM_GSP_VGPU_WATCHCAT_TIMEOUT_MIN

// Set watchdog timeout value for the libos user task watchdog
#define NV_REG_STR_RM_GSP_LIBOS_WATCHDOG_TIMEOUT             "RmGspLibosWatchdogTimeOut"
#define NV_REG_STR_RM_GSP_LIBOS_WATCHDOG_TIMEOUT_MIN         0x00000000
#define NV_REG_STR_RM_GSP_LIBOS_WATCHDOG_TIMEOUT_DEFAULT     0x00000005

#define NV_REG_STR_RM_DO_LOG_RC_EVENTS                      "RmLogonRC"
// Type Dword
// Encoding : 0 --> Skip Logging
//            1 --> Do log
// Enable/Disable Event Logging on RC errors
// Default is Disabled
#define NV_REG_STR_RM_DO_LOG_RC_ENABLE                 0x00000001
#define NV_REG_STR_RM_DO_LOG_RC_DISABLE                0x00000000
#define NV_REG_STR_RM_DO_LOG_RC_DEFAULT                NV_REG_STR_RM_DO_LOG_RC_DISABLE

// Type Dword
// Encoding : 0       --> Skip Breakpoint
//            nonzero --> Do Breakpoint
// Enable/Disable breakpoint on DEBUG resource manager on RC errors

#define NV_REG_STR_RM_BREAK_ON_RC                      "RmBreakonRC"
#define NV_REG_STR_RM_BREAK_ON_RC_DISABLE              0x00000000
#define NV_REG_STR_RM_BREAK_ON_RC_ENABLE               0x00000001

// Explicitly disable RmBreakOnRC for Retail <enable via registry> and
// RMCFG_FEATURE_PLATFORM_GSP builds
#if ((defined(DEBUG) || defined(QA_BUILD)) &&  \
     (!defined(RMCFG_FEATURE_PLATFORM_GSP) ||  \
      (defined(RMCFG_FEATURE_PLATFORM_GSP) && !RMCFG_FEATURE_PLATFORM_GSP)))
#define NV_REG_STR_RM_BREAK_ON_RC_DEFAULT              NV_REG_STR_RM_BREAK_ON_RC_ENABLE
#else
#define NV_REG_STR_RM_BREAK_ON_RC_DEFAULT              NV_REG_STR_RM_BREAK_ON_RC_DISABLE
#endif

// Volatile registry entries for previous driver version.
// Used to record driver unload/reload for debugging purposes.
#define NV_REG_STR_RM_RC_PREV_DRIVER_VERSION        "RmRCPrevDriverVersion"
#define NV_REG_STR_RM_RC_PREV_DRIVER_BRANCH         "RmRCPrevDriverBranch"
#define NV_REG_STR_RM_RC_PREV_DRIVER_CHANGELIST     "RmRCPrevDriverChangelist"
#define NV_REG_STR_RM_RC_PREV_DRIVER_LOAD_COUNT     "RmRCPrevDriverLoadCount"

#define NV_REG_STR_USE_UNCACHED_PCI_MAPPINGS            "UseUncachedPCIMappings"
// Type DWORD
// Encode -- Numeric Value
// Check to see if we are converting PCI mappings

#define NV_REG_STR_RM_CE_USE_GEN4_MAPPING                "RmCeUseGen4Mapping"
#define NV_REG_STR_RM_CE_USE_GEN4_MAPPING_TRUE           0x1
#define NV_REG_STR_RM_CE_USE_GEN4_MAPPING_FALSE          0x0
// Type Dword (Boolean)
// Encoding Numeric Value
// Use gen4 mapping that uses a HSHUB CE, if available
// Else, continue using FBHUB PCEs

// Type Dword
// Enable PCE LCE auto config
#define NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG                "RmCeEnableAutoConfig"
#define NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG_TRUE           0x1
#define NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG_FALSE          0x0

//
// Type DWORD
// NVLINK control overrides.
//
// FORCE_DISABLE: Force disable NVLINK when the current default is ON (POR)
//
// TRAIN_AT_LOAD : Force train links during driver load
//
// FORCE_AUTOCONFIG : Force autoconfig training regardless of chiplib forced config links
//
// FORCE_ENABLE: Force enable NVLINK when the current default is OFF (bringup etc.)
//
// PARALLEL_TRAINING: Have the GPU endpoint parallelize link training
#define NV_REG_STR_RM_NVLINK_CONTROL                                "RMNvLinkControl"
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_DISABLE                  0:0
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_DISABLE_NO               (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_DISABLE_YES              (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_DISABLE_DEFAULT          (NV_REG_STR_RM_NVLINK_CONTROL_FORCE_DISABLE_NO)
#define NV_REG_STR_RM_NVLINK_CONTROL_TRAIN_AT_LOAD                  1:1
#define NV_REG_STR_RM_NVLINK_CONTROL_TRAIN_AT_LOAD_NO               (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_TRAIN_AT_LOAD_YES              (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_TRAIN_AT_LOAD_DEFAULT          (NV_REG_STR_RM_NVLINK_CONTROL_TRAIN_AT_LOAD_NO)
#define NV_REG_STR_RM_NVLINK_CONTROL_SKIP_TRAIN                     2:2
#define NV_REG_STR_RM_NVLINK_CONTROL_SKIP_TRAIN_NO                  (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_SKIP_TRAIN_YES                 (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_SKIP_TRAIN_DEFAULT             (NV_REG_STR_RM_NVLINK_CONTROL_SKIP_TRAIN_NO)
#define NV_REG_STR_RM_NVLINK_CONTROL_RESERVED_0                     6:3
#define NV_REG_STR_RM_NVLINK_CONTROL_LINK_TRAINING_DEBUG_SPEW       7:7
#define NV_REG_STR_RM_NVLINK_CONTROL_LINK_TRAINING_DEBUG_SPEW_OFF   (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_LINK_TRAINING_DEBUG_SPEW_ON    (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_AUTOCONFIG               8:8
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_AUTOCONFIG_NO            (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_AUTOCONFIG_YES           (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_AUTOCONFIG_DEFAULT       (NV_REG_STR_RM_NVLINK_CONTROL_FORCE_AUTOCONFIG_NO)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_ENABLE                   31:31
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_ENABLE_NO                (0x00000000)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_ENABLE_YES               (0x00000001)
#define NV_REG_STR_RM_NVLINK_CONTROL_FORCE_ENABLE_DEFAULT           (NV_REG_STR_RM_NVLINK_CONTROL_FORCE_ENABLE_NO)

//
// Type DWORD
// Knob to control NVLink MINION
//
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL                          "RMNvLinkMinionControl"
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE                   3:0
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE_DEFAULT           (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE_FORCE_ON          (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE_FORCE_OFF         (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_PHY_CONFIG               7:4
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_PHY_CONFIG_DEFAULT       (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_PHY_CONFIG_ENABLE        (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_PHY_CONFIG_DISABLE       (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_DL_STATUS                11:8
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_DL_STATUS_DEFAULT        (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_DL_STATUS_ENABLE         (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_DL_STATUS_DISABLE        (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITNEGOTIATE            15:12
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITNEGOTIATE_DEFAULT    (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITNEGOTIATE_ENABLE     (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITNEGOTIATE_DISABLE    (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITOPTIMIZE             19:16
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITOPTIMIZE_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITOPTIMIZE_ENABLE      (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_INITOPTIMIZE_DISABLE     (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS              23:20
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS_DEFAULT      (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS_ENABLE       (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS_DISABLE      (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE                27:24
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE_DEFAULT        (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE_RISCV          (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE_FALCON         (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE_RISCV_MANIFEST (0x00000003)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_BOOT_CORE_NO_MANIFEST    (0x00000004)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING             30:28
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING_ENABLE      (0x00000001)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING_DISABLE     (0x00000002)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_GFW_BOOT_DISABLE         31:31
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_GFW_BOOT_DISABLE_DEFAULT (0x00000000)
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_GFW_BOOT_DISABLE_DISABLE (0x00000001)

//
// Type DWORD
// Knob to change NVLink link speed
// __LAST is same as highest supported speed
// NOTE:
//   NVLINK_SPEED_CONTROL_SPEED_25G is exactly 25.00000Gbps on Pascal
//   NVLINK_SPEED_CONTROL_SPEED_25G is exactly 25.78125Gbps on Volta and later
//   NVLINK_SPEED_CONTROL_SPEED_2500000G is exactly 25.00000Gbps on all chips
//
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL                          "RMNvLinkSpeedControl"
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED                     4:0
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_DEFAULT             (0x00000000)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_16G                 (0x00000001)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_19_2G               (0x00000002)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_20G                 (0x00000003)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_21G                 (0x00000004)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_22G                 (0x00000005)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_23G                 (0x00000006)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_24G                 (0x00000007)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_25G                 (0x00000008)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_25_78125G           (0x00000008)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_25_00000G           (0x00000009)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_16_14583G           (0x0000000A)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_26_56250G           (0x0000000B)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_27_34375G           (0x0000000C)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_28_12500G           (0x0000000D)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_32G                 (0x0000000E)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_40G                 (0x0000000F)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_50_00000G           (0x00000010)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_53_12500G           (0x00000011)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_100_00000G          (0x00000012)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_106_25000G          (0x00000013)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_FAULT               (0x00000014)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED__LAST               (0x00000014)

//
// Type DWORD
// P2P Loopback over NVLINK will be enabled by default if RM
// detects loopback links. For P2P over PCIE, force disable
// P2P loopback over NVLINK using the following regkey
//
#define NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK                   "RMNvLinkDisableP2PLoopback"
#define NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK_DEFAULT           (0x00000000)
#define NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK_TRUE              (0x00000001)
#define NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK_FALSE             (0x00000000)

//
// Type DWORD
// Knob to control NVLink Link Power States
//
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL                            "RMNvLinkControlLinkPM"
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_SINGLE_LANE_MODE           1:0
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_SINGLE_LANE_MODE_DEFAULT   (0x00000000)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_SINGLE_LANE_MODE_ENABLE    (0x00000001)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_SINGLE_LANE_MODE_DISABLE   (0x00000002)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_PROD_WRITES                3:2
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_PROD_WRITES_DEFAULT        (0x00000000)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_PROD_WRITES_ENABLE         (0x00000001)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_PROD_WRITES_DISABLE        (0x00000002)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L1_MODE                    5:4
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L1_MODE_DEFAULT            (0x00000000)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L1_MODE_ENABLE             (0x00000001)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L1_MODE_DISABLE            (0x00000002)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L2_MODE                    7:6
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L2_MODE_DEFAULT            (0x00000000)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L2_MODE_ENABLE             (0x00000001)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_L2_MODE_DISABLE            (0x00000002)
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_RESERVED                   31:8

//
// Type DWORD
// Knob to force lane disable and shutdown during driver unload
//     The regkey will also cause a toggle of link reset on driver load
//     The regkey should not be used in S/R paths
//
#define NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN                        "RMNvLinkForceLaneshutdown"
#define NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN_TRUE                   (0x00000001)
#define NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN_FALSE                  (0x00000000)
#define NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN_DEFAULT                (NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN_FALSE)

//
// Type DWORD
// For links that are SYSMEM, use this device type for force configs
// Choose the value from NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_*
//
#define NV_REG_STR_RM_NVLINK_SYSMEM_DEVICE_TYPE                         "RMNvLinkForcedSysmemDeviceType"

//
// Type DWORD
// NVLink Disable Link Overrides
// The supplied value is ANDed with the set of discovered
// (not necessarily connected) links to remove unwanted links.
//    A value of DISABLE_ALL removes/disables all links on this device.
//    A value of DISABLE_NONE removes no links.
//    If not present, this regkey has no effect.
//
#define NV_REG_STR_RM_NVLINK_DISABLE_LINKS                    "RMNvLinkDisableLinks"
#define NV_REG_STR_RM_NVLINK_DISABLE_LINKS_DISABLE_ALL        (0xFFFFFFFF)
#define NV_REG_STR_RM_NVLINK_DISABLE_LINKS_DISABLE_NONE       (0x00000000)

//
// Type DWORD
// NVLINK Enable Links Overrides
// Note that this control does not force enable links, rather, it should be
// used to disable or mask off SW discovered links supported by the HW.
//
// NOTE: THIS REGKEY HAS BEEN DEPRECATED IN RM, since it does NOT work
//       with NVLink auto-configuration. Instead, please move to using
//       the new regkey NV_REG_STR_RM_NVLINK_DISABLE_LINKS
//
#define NV_REG_STR_RM_NVLINK_ENABLE                          "RMNvLinkEnable"
#define NV_REG_STR_RM_NVLINK_ENABLE_IDX(i)                   (i):(i)
#define NV_REG_STR_RM_NVLINK_ENABLE_IDX__SIZE                32
#define NV_REG_STR_RM_NVLINK_ENABLE_IDX_TRUE                 (0x00000001)
#define NV_REG_STR_RM_NVLINK_ENABLE_IDX_FALSE                (0x00000000)

//
// Type DWORD
// Knob to control NVLink Verbose Prints
//
#define NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL                   "RMNvLinkverboseControlMask"
#define NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL_REGINIT           0:0
#define NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL_REGINIT_DEFAULT   (0x00000000)
#define NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL_REGINIT_ON        (0x00000001)
#define NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL_REGINIT_OFF       (0x00000000)

// Type DWORD:
#define NV_REG_STR_RM_PCIE_LINK_SPEED                                     "RMPcieLinkSpeed"
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN2                          1:0
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN2_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN2_ENABLE                   (0x00000001)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN2_DISABLE                  (0x00000002)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN3                          3:2
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN3_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN3_ENABLE                   (0x00000001)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN3_DISABLE                  (0x00000002)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN4                          5:4
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN4_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN4_ENABLE                   (0x00000001)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN4_DISABLE                  (0x00000002)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN5                          7:6
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN5_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN5_ENABLE                   (0x00000001)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN5_DISABLE                  (0x00000002)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN6                          9:8
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN6_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN6_ENABLE                   (0x00000001)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_ALLOW_GEN6_DISABLE                  (0x00000002)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_LOCK_AT_LOAD                        31:31
#define NV_REG_STR_RM_PCIE_LINK_SPEED_LOCK_AT_LOAD_DISABLE                (0x00000000)
#define NV_REG_STR_RM_PCIE_LINK_SPEED_LOCK_AT_LOAD_ENABLE                 (0x00000001)

//
// Type DWORD
//
//
// This can be used as a per-device regkey or not, in which case the setting
// will apply to all devices. If this key is supplied as both a per-device and
// non-per-device regkey, the non-per-device option will apply first to all
// devices, and then the per-device key settings will apply, overriding the
// settings for the relevant devices.
//
// Encoding : 0 - Disable PCIe Relaxed Ordering TLP header bit setting. This is
//                the default option.
//            1 - Try to enable PCIe Relaxed Ordering TLP header bit setting.
//                Traverses the PCIe topology and only enables the header bit if
//                it is safe to do so, with regard to all devices that could be
//                affected.
//            2 - Forcibly enable PCIe Relaxed Ordering TLP header bit setting.
//                Explicitly ignores the compatibility of the PCIe topology
//                around the device or devices in question.
//
#define NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING               "RmSetPCIERelaxedOrdering"
#define NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_DEFAULT       0x00000000
#define NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_DISABLE       0x00000000
#define NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_ENABLE        0x00000001
#define NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_FORCE_ENABLE  0x00000002

// Type DWORD
// This regkey overrides the default use case to optimize the GPU for.
// This regkey should not be used with the RMFermiBigPageSize regkey.
// This regkey should only be set by the RM.
#define NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX    "RMOptimizeComputeOrSparseTex"
#define NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT              (0x00000000)
#define NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_COMPUTE              (0x00000001)
#define NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_SPARSE_TEX           (0x00000002)

#define NV_REG_STR_CL_ASLM_CFG                                      "AslmCfg"
#define NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE                      1:0
#define NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE_NO                   0x00000000
#define NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE_YES                  0x00000001
#define NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE_DEFAULT              0x00000002
#define NV_REG_STR_CL_ASLM_CFG_HOT_RESET                            5:4
#define NV_REG_STR_CL_ASLM_CFG_HOT_RESET_NO                         0x00000000
#define NV_REG_STR_CL_ASLM_CFG_HOT_RESET_YES                        0x00000001
#define NV_REG_STR_CL_ASLM_CFG_HOT_RESET_DEFAULT                    0x00000002
#define NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE                         9:8
#define NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE_NO                      0x00000000
#define NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE_YES                     0x00000001
#define NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE_DEFAULT                 0x00000002
#define NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE                    11:10
#define NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE_NO                 0x00000000
#define NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE_YES                0x00000001
#define NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE_DEFAULT            0x00000002
// Type Dword
// Encoding Numeric Value
// Overrides chipset-based ASLM configurations.
//
// NV link upgrade:
//  0 - Do not use NV link upgrade for ASLM
//  1 - Use NV link upgrade for ASLM
//  2 - Do not override chipset-selected config (default)
// Hot reset:
//  0 - Do not use hot reset for ASLM
//  1 - Use hot reset for ASLM
//  2 - Do not override chipset-selected config (default)
// Fast link upgrade:
//  0 - Do not use fast link upgrade for ASLM
//  1 - Use fast link upgrade for ASLM
//  2 - Do not override chipset-selected config (default)
// Gen2 link width upgrade:
//  0 - Do not use Gen2 link upgrade for ASLM
//  1 - Use Gen2 link upgrade for ASLM
//  2 - Do not override chipset-selected config (default)

#define NV_REG_STR_RM_DISABLE_BR03_FLOW_CONTROL         "MB_DisableBr03FlowControl"
// Type DWORD
// Encoding 1 -> Do not program BR03 flow control registers
//          0 -> Setup BR03 flow control registers
// Determine whether we need to program BR03 flow control registers, in objcl.c

#define NV_REG_STR_RM_FORCE_ENABLE_GEN2                     "RmForceEnableGen2"
#define NV_REG_STR_RM_FORCE_ENABLE_GEN2_NO                  0
#define NV_REG_STR_RM_FORCE_ENABLE_GEN2_YES                 1
#define NV_REG_STR_RM_FORCE_ENABLE_GEN2_DEFAULT             NV_REG_STR_RM_FORCE_ENABLE_GEN2_NO
// Type DWORD: On some platform, Gen2 is disabled to work around system problems.
//             This key is to force enabling Gen2 for testing or other purpose.  It is
//             ineffective on platforms not Gen2 capable.
// Encoding boolean:
// 0 - Do Nothing
// 1 - Force Enable Gen2 (to invalidate PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED)
//

#define NV_REG_STR_RM_D3_FEATURE                                    "RMD3Feature"
// Type DWORD
// This regkey controls D3 related features
#define NV_REG_STR_RM_D3_FEATURE_DRIVER_CFG_SPACE_RESTORE           1:0
#define NV_REG_STR_RM_D3_FEATURE_DRIVER_CFG_SPACE_RESTORE_DEFAULT   (0x00000000)
#define NV_REG_STR_RM_D3_FEATURE_DRIVER_CFG_SPACE_RESTORE_ENABLED   (0x00000001)
#define NV_REG_STR_RM_D3_FEATURE_DRIVER_CFG_SPACE_RESTORE_DISABLED  (0x00000002)
#define NV_REG_STR_RM_D3_FEATURE_DRIVER_CFG_SPACE_RESTORE_UNUSED    (0x00000003)

#define NV_REG_STR_EMULATED_NBSI_TABLE                      "RMemNBSItable"
// The emulated NBSI table

#define NV_REG_STR_RM_DISABLE_FSP                           "RmDisableFsp"
#define NV_REG_STR_RM_DISABLE_FSP_NO                        (0x00000000)
#define NV_REG_STR_RM_DISABLE_FSP_YES                       (0x00000001)
// Type DWORD (Boolean)
// Override any other settings and disable FSP

#define NV_REG_STR_RM_DISABLE_COT_CMD                       "RmDisableCotCmd"
#define NV_REG_STR_RM_DISABLE_COT_CMD_FRTS_SYSMEM           1:0
#define NV_REG_STR_RM_DISABLE_COT_CMD_FRTS_VIDMEM           3:2
#define NV_REG_STR_RM_DISABLE_COT_CMD_GSPFMC                5:4
#define NV_REG_STR_RM_DISABLE_COT_CMD_DEFAULT               (0x00000000)
#define NV_REG_STR_RM_DISABLE_COT_CMD_YES                   (0x00000001)
// Type DWORD (Boolean)
// Disable the specified commands as part of Chain-Of-Trust feature

#define NV_REG_STR_RM_FSP_USE_MNOC                          "RmFspUseMnoc"
#define NV_REG_STR_RM_FSP_USE_MNOC_DEFAULT                  (0x00000000)
#define NV_REG_STR_RM_FSP_USE_MNOC_CPU                      (0x00000001)
#define NV_REG_STR_RM_FSP_USE_MNOC_GSP                      (0x00000002)
#define NV_REG_STR_RM_FSP_USE_MNOC_BOTH                     (0x00000003)
// Type DWORD
// Use MNOC (mailbox on CPU / MCTP on GSP) interface to communicate with FSP

#define NV_REG_STR_PCI_LATENCY_TIMER_CONTROL                "PciLatencyTimerControl"
// Type Dword
// Encoding Numeric Value
// Override to control setting/not setting of pci timer latency value.
// Not present suggests default value. A value 0xFFFFFFFF will leave the value unmodified (ie bios value).
// All other values must be multiples of 8

#define NV_REG_STR_RM_ENABLE_ROUTE_TO_PHYSICAL_LOCK_BYPASS  "RmRouteToPhyiscalLockBypass"
// Type Bool
// Enable optimisation to only take API READ (not WRITE) lock when forwarding ROUTE_TO_PHYSICAL
// control calls to GSP-enabled GPUs.
// This will heavily improve multi-gpu-multi-process control call latency and throughput.
// This optimisation will only work when *all* GPUs in the system are in offload mode (GSP mode).

#define NV_REG_STR_RM_GPU_FABRIC_PROBE "RmGpuFabricProbe"
#define NV_REG_STR_RM_GPU_FABRIC_PROBE_DELAY               7:0
#define NV_REG_STR_RM_GPU_FABRIC_PROBE_SLOWDOWN_THRESHOLD 15:8
#define NV_REG_STR_RM_GPU_FABRIC_PROBE_OVERRIDE           31:31
// Type DWORD
// Enable GPU fabric probe
//
// When this option is enabled, the GPU will probe its fabric state over the
// NVLink inband channel. The fabric state includes the attributes to allow
// the GPU to participate in P2P over the NVLink fabric.
//
// This option is only honored on NVSwitch based systems.
//
// Encoding:
//    _DELAY              : Delay between consecutive probe retries (in sec)
//                           before the slowdown starts. (Default: 5 sec)
//    _SLOWDOWN_THRESHOLD : Number of probes retries before the slowdown starts
//                          (Default: 10). The slowdown doubles the delay
//                          between every consecutive probe retries until success.
//

// Enable plugin logs in ftrace buffer.
// 0 - Default
// 0 - Disabled
// 1 - Enabled
#define NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER                "RmEnablePluginFtrace"
#define NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER_ENABLED        0x00000001
#define NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER_DISABLED       0x00000000
#define NV_REG_STR_RM_ENABLE_PLUGIN_IN_FTRACE_BUFFER_DEFAULT        0x00000000

// TYPE Dword
// Enable vGPU migration on KVM hyperivsor.
// 1 - (Default) Enable vGPU migration on KVM
// 0 - Disable vGPU migration on KVM hypervisor
//
#define NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION           "RmEnableKvmVgpuMigration"
#define NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_TRUE      0x00000001
#define NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_FALSE     0x00000000
#define NV_REG_STR_RM_ENABLE_KVM_VGPU_MIGRATION_DEFAULT   0x00000001

#define NV_REG_STR_RM_QSYNC_FW_REV_CHECK                    "QuadroSyncFirmwareRevisionCheckDisable"
#define NV_REG_STR_RM_QSYNC_FW_REV_CHECK_DEFAULT            0x00000000
#define NV_REG_STR_RM_QSYNC_FW_REV_CHECK_ENABLE             0x00000000
#define NV_REG_STR_RM_QSYNC_FW_REV_CHECK_DISABLE            0x00000001

// Type DWORD
// Disable Quadro Sync Firmware Revision Checking, for testing new versions.
//

//
// Type: Dword
// Encoding:
// 1 - SRIOV Enabled on supported GPU
// 0 - SRIOV Disabled on specific GPU
//
#define NV_REG_STR_RM_SET_SRIOV_MODE                    "RMSetSriovMode"
#define NV_REG_STR_RM_SET_SRIOV_MODE_DISABLED             0x00000000
#define NV_REG_STR_RM_SET_SRIOV_MODE_ENABLED              0x00000001

#define NV_REG_STR_RM_SET_VGPU_VERSION_MIN                       "RMSetVGPUVersionMin"
//
// TYPE DWORD
// Set the minimum vGPU version enforced to support

#define NV_REG_STR_RM_SET_VGPU_VERSION_MAX                       "RMSetVGPUVersionMax"
//
// TYPE DWORD
// Set the maximum vGPU version enforced to support

#define NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME             "TSwapRdyHiLsrMinTime"
#define NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME_DEFAULT     250 // 250 micro seconds
// Type: DWORD
// Encoding:
// To modify LSR_MIN_TIME parameter according to the time
// period for which swap lock window will remain HIGH for QSYNC III
// i.e. P2060 during swap barrier.

#define NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_SWAP_LOCKOUT_START           "TSwapRdyHiSwapLockoutStart"
#define NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_SWAP_LOCKOUT_START_DEFAULT   250 // 250 micro seconds
// Type: DWORD
// Encoding:
// To modify SWAP_LOCKOUT_START parameter according to the time
// period for which swap lock window will remain HIGH for QSYNC III.
//

#define NV_REG_STR_RM_NVLINK_BW                     "RmNvlinkBandwidth"
// Type String
// The option is in the string format.
//
// Possible string values:
//   OFF:      0% bandwidth
//   MIN:      15%-25% bandwidth depending on the system's NVLink topology
//   HALF:     50% bandwidth
//   3QUARTER: 75% bandwidth
//   FULL:     100% bandwidth (default)
//
// This option is only for Hopper+ GPU with NVLINK version 4.0.

#define NV_REG_STR_RM_NVLINK_BW_LINK_COUNT                     "RmNvlinkBandwidthLinkCount"
// Type: DWORD
//
// Link count RBM (Reduced Bandwidth Mode) requested.
//
// This option is only for Blackwell+ GPU with NVLINK version 5.0.

//
// Type DWORD (Boolean)
// 1 - Measure API and GPU lock hold/wait times which can be retrieved with the
//     NV0000_CTRL_CMD_SYSTEM_GET_LOCK_TIMES control call
// 0 - (Default) Don't measure lock hold/wait times
//
#define NV_REG_STR_RM_LOCK_TIME_COLLECT                            "RmLockTimeCollect"

//
// Type: DWORD (Boolean)
//
// 1 - Only invalidate and free CPU mappings immediatelly, then collect GPU resources
//     from individual clients under separate lock acquire/release sequences.
// 0 - (Default) Immediately free all clients resources when freeing a client list
#define NV_REG_STR_RM_CLIENT_LIST_DEFERRED_FREE             "RMClientListDeferredFree"

//
// Type: DWORD
//
// Number of clients to free in a single chunk before yielding and scheduling
// a work item to handle the rest.
//
// Only valid if NV_REG_STR_RM_CLIENT_LIST_DEFERRED_FREE is set.
//
// Value of 0 (default) means there is no limit and all clients will be freed
// at once before the process terminates.
//
#define NV_REG_STR_RM_CLIENT_LIST_DEFERRED_FREE_LIMIT      "RMClientListDeferredFreeLimit"

//
// TYPE Dword
// Determines whether or not to emulate VF MMU TLB Invalidation register range
// Encoding : 1 - Emulate register range (Default)
//          : 0 - Do not emulate register range.
//
#define NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE            "RMBug3007008EmulateVfMmuTlbInvalidate"
#define NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_ENABLE     0x00000001
#define NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_DISABLE    0x00000000
#define NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_DEFAULT    NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_ENABLE

#define NV_REG_STR_RM_POWER_FEATURES                        "RMPowerFeature"

// Type DWORD
// This Regkey controls inforom black box data recording. This can be used to
// restrict access to BBX.
// 0               - Enable BBX. (Default)
// COMPLETELY      - Enable/Disable BBX access (read/write).
// WRITE_BY_RM     - Enable/Disable writes by RM itself.
// WRITE_BY_CLIENT - Enable/Disable writes by clients to RM.
// PERIODIC FLUSH  - Enable/Disable periodic flush to inforom (Also enables/disables Power data collection)
//
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX                  "RmDisableInforomBBX"
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_NO                        (0x00000000)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_YES                       (0x00000001)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_COMPLETELY                         0:0
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_COMPLETELY_NO             (0x00000000)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_COMPLETELY_YES            (0x00000001)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_RM                        1:1
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_RM_NO            (0x00000000)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_RM_YES           (0x00000001)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_CLIENT                    2:2
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_CLIENT_NO        (0x00000000)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_WRITE_BY_CLIENT_YES       (0x00000001)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_PERIODIC_FLUSH                     3:3
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_PERIODIC_FLUSH_YES        (0x00000000)
#define NV_REG_STR_RM_INFOROM_DISABLE_BBX_PERIODIC_FLUSH_NO         (0x00000001)

//
// Type DWORD (Boolean)
// RmNvlinkEnablePrivErrorRc
//
// 0 - (default) does not do RC recovery when PRIV_ERROR
// 1 - enable FLA PRIV_ERROR RC recovery
//
#define NV_REG_STR_RM_NVLINK_ENABLE_PRIV_ERROR_RC                 "RmNvlinkEnablePrivErrorRc"
#define NV_REG_STR_RM_NVLINK_ENABLE_PRIV_ERROR_RC_NO              0
#define NV_REG_STR_RM_NVLINK_ENABLE_PRIV_ERROR_RC_YES             1

//
// Add the conditions to exclude these macros from Orin build, as CONFIDENTIAL_COMPUTE
// is a guardword. The #if could be removed when nvRmReg.h file is trimmed from Orin build.
//
// Enable Disable Confidential Compute and control its various modes of operation
// 0 - Feature Disable
// 1 - Feature Enable
//
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE                              "RmConfidentialCompute"
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_ENABLED                      0:0
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_ENABLED_NO                   0x00000000
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_ENABLED_YES                  0x00000001
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_DEV_MODE_ENABLED             1:1
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_DEV_MODE_ENABLED_NO          0x00000000
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_DEV_MODE_ENABLED_YES         0x00000001
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_GPUS_READY_CHECK             2:2
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_GPUS_READY_CHECK_DISABLED    0x00000000
#define NV_REG_STR_RM_CONFIDENTIAL_COMPUTE_GPUS_READY_CHECK_ENABLED     0x00000001

//
// Enable/disable SPDM feature in Confidential Compute. SPDM-capable profiles
// may not be loaded by default. This regkey allows us to override the default
// behavior and force SPDM to enabled/disabled.
//
// 0 - Feature disable
// 1 - Feature enable
//
#define NV_REG_STR_RM_CONF_COMPUTE_SPDM_POLICY                            "RmConfComputeSpdmPolicy"
#define NV_REG_STR_RM_CONF_COMPUTE_SPDM_POLICY_ENABLED                    0:0
#define NV_REG_STR_RM_CONF_COMPUTE_SPDM_POLICY_ENABLED_NO                 0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_SPDM_POLICY_ENABLED_YES                0x00000001

//
// Enable/disable key rotation in Confidential Compute.
//
// 0 - Feature disabled
// 1 - Feature enabled
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION                    "RmConfComputeKeyRotation"
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED            0:0
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED_NO         0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED_YES        0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_SEC2_KEYS          1:1
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_SEC2_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_SEC2_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE2_KEYS          2:2
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE2_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE2_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE3_KEYS          3:3
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE3_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE3_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE4_KEYS          4:4
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE4_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE4_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE5_KEYS          5:5
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE5_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE5_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE6_KEYS          6:6
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE6_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE6_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE7_KEYS          7:7
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE7_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE7_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE8_KEYS          8:8
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE8_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE8_KEYS_YES      0x00000001
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE9_KEYS          9:9
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE9_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LCE9_KEYS_YES      0x00000001

// if all kernel keys should be considered for key rotation
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_KERNEL_KEYS        10:10
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_KERNEL_KEYS_NO     0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_KERNEL_KEYS_YES    0x00000001

// if all user keys should be considered for key rotation
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_USER_KEYS          11:11
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_USER_KEYS_NO       0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_USER_KEYS_YES      0x00000001

// if internal RM keys should be considered for key rotation
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_INTERNAL_KEYS      12:12
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_INTERNAL_KEYS_NO   0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_INTERNAL_KEYS_YES  0x00000001

#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED_ALL        12:0
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED_ALL_NO     0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_ENABLED_ALL_YES    0x00001fff

//
// Set period for "keep-alive" heartbeat message sent between SPDM Requester and Responder.
// This will sent a keep-alive message every period to GPU. GPU will set timeout to 2 * period.
// If GPU doesn't receive message within 2 * period, it is fatal error and GPU will require reset.
// Minimum period is 4 seconds, maximum period is 255 seconds. Setting period to 0 will disable heartbeat.
//
// 0 - Disable feature (no heartbeat sending)
// x - Period value in seconds
//
#define NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT                             "RmConfComputeHeartbeatPeriod"
#define NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS              31:0
#define NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS_DISABLE      0x00000000
#define NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS_MIN          0x00000004
#define NV_REG_STR_RM_CONF_COMPUTE_HEARTBEAT_PERIOD_SECONDS_MAX          0x000000FF

//
// Set the key rotation timeout value for user-mode clients.
// This is the amount of time in seconds, after the threshold has been crossed, that user-mode clients
// have to idle their channels before RM RCs the channels for key rotation.
// This value must be greater than or equal to 2.
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_TIMEOUT_IN_SEC "RmKeyRotationTimeout"

//
// Set the difference between the lower and upper thresholds.
// Value is in units of (amount of data encrypted in units of 16 bytes + number of encryption invocations)
// If this is set then NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LOWER_THRESHOLD and
// NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_UPPER_THRESHOLD are ignored.
// This value must be greater than 0.
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_THRESHOLD_DELTA "RmKeyRotationThresholdDelta"

//
// Set lower threshold for key rotation.
// Value is in units of (amount of data encrypted in units of 16 bytes + number of encryption invocations)
// If this is set then RmKeyRotationUpperThreshold must also be set.
// This value must be less than RmKeyRotationUpperThreshold.
// Note that setting the attacker advantage via SMI/NVML will overwrite this value.
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_LOWER_THRESHOLD "RmKeyRotationLowerThreshold"

//
// Set upper threshold for key rotation.
// Value is in units of (amount of data encrypted in units of 16 bytes + number of encryption invocations)
// If this is set then RmKeyRotationLowerThreshold must also be set.
// This value must be greater than RmKeyRotationLowerThreshold.
// Note that setting the attacker advantage via SMI/NVML will overwrite this value.
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_UPPER_THRESHOLD "RmKeyRotationUpperThreshold"

//
// Set threshold for rotation of internal (RM only) keys.
// Value is in units of (amount of data encrypted in units of 16 bytes + number of encryption invocations)
// Value must be greater than minimum of (0x7FFFFFF).
// This value cannot be changed at runtime, only via this registry key at boot time.
//
#define NV_REG_STR_RM_CONF_COMPUTE_KEY_ROTATION_INTERNAL_THRESHOLD "RmKeyRotationInternalThreshold"

//
// Controls whether GSP-RM profiling is enabled.
// 0 (default): disabled
// 1: enabled
//
#define NV_REG_STR_RM_GSPRM_PROFILING "RmGspRmProfiling"
#define NV_REG_STR_RM_GSPRM_PROFILING_DISABLE 0
#define NV_REG_STR_RM_GSPRM_PROFILING_ENABLE  1

//
// Controls, GSP-RM start with boost clocks.
// 0 : disabled
// 1 : enabled (default)
//
#define NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS "RmBootGspRmWithBoostClocks"
#define NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS_DISABLED 0
#define NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS_ENABLED  1

//
// Enable Local EGM HW verification using RM/SW stack.
// Must be specified with a peerID corresponding to local EGM
//
#define NV_REG_STR_RM_ENABLE_LOCAL_EGM_PEER_ID            "RMEnableLocalEgmPeerId"

//
// Overrides the size of the GSP-RM firmware heap in GPU memory.
// The GSP-RM firmware heap is reserved for system use and is not available to
// applications. This regkey can be used to optimize the amount of memory
// reserved for system use for targeted use cases. The default value for this
// regkey is determined to support certain worst case resource allocation
// patterns, but many use cases do not exhibit such resource allocation patterns
// and could benefit from the lesser reserved GPU memory. Other use cases may
// exhibit an even more pathological/stressful resource allocation pattern,
// which can be enabled (up to a limit) with this regkey.
//
// However, NVIDIA does not support setting this registry key, and will require
// that any bugs observed with it set be reproducible with the default setting
// as well.
//
// The value of this regkey is specified in megabytes. A value of 0 indicates to
// use the default value. Values less than the minimum or greater than the
// maximum will be clamped to the nearest optimum. The default values are
// are dynamically computed for each GPU prior to booting GSP-RM.
//
#define NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB          "RmGspFirmwareHeapSizeMB"
#define NV_REG_STR_GSP_FIRMWARE_HEAP_SIZE_MB_DEFAULT  0

//
// Sets the size of the sysmem heap for GSP-RM, only for use with 0FB chips.
// On 0FB chips, a segment of sysmem is allocated for use by GSP-RM in place of
// where FB would be used.
//
// NOTE: This is different from the above "RmGspFirmwareHeapSizeMB", which
// is used for the WPR heap. The sysmem heap is used for memdescAlloc() calls
//
#define NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB          "RmGspSysmemHeapSizeMB"
#define NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB_DEFAULT  512

//
// Type DWORD
// This regkey can be used to enable GSP owned fault buffers
//
#define NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE      "RmGspOwnedFaultBuffersEnable"
#define NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE_NO    0x00000000
#define NV_REG_STR_RM_GSP_OWNED_FAULT_BUFFERS_ENABLE_YES   0x00000001

//
// WAR for BlueField3: Bug 4040336
// BF3's PCI MMIO bus address 0x800000000000 is too high for Ampere to address.
// Due to this, BF3's bus address is now moved to < 4GB. So, the CPU PA is no longer
// the same as the bus address and this regkey adjusts the CPU PA passed in to the
// correct bus address.
//
#define NV_REG_STR_RM_DMA_ADJUST_PEER_MMIO_BF3 "RmDmaAdjustPeerMmioBF3"
#define NV_REG_STR_RM_DMA_ADJUST_PEER_MMIO_BF3_DISABLE 0
#define NV_REG_STR_RM_DMA_ADJUST_PEER_MMIO_BF3_ENABLE  1

#define NV_REG_STR_RM_NVLINK_FORCED_LOOPBACK_ON_SWITCH                  "RMNvLinkForcedLoopbackOnSwitch"
#define NV_REG_STR_RM_NVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE             0:0
#define NV_REG_STR_RM_NVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_DEFAULT     (0x00000000)
#define NV_REG_STR_RM_NVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED     (0x00000001)

//
// Type DWORD
// This set of MIG regkeys specifies a set of allocation requests to be issued to the GPU on boot.
// MIG configuration contained within GPUMGR always supersedes these regkeys, if present and valid.
// The entire configuration specified by these regkeys is validated before being applied. An error
// reflected in whole or in part on these regkeys will cause them to be discarded entirely.
//
// RmMIGBootConfigurationGI is used to encode a series of GPU instance allocations. These are applied in order.
// RmMIGBootConfigurationCI is used to encode a series of CI instance allocations.
//   The GI associated with each CI allocation entry is specified by RmMIGBootConfigurationCIAssignment.
//   It is an error to specify a CI via RmMIGBootConfigurationCI without specifying the associated GPU
//   instance entry via RmMIGBootConfigurationCIAssignment. The values for any CI assignment for a CI
//   entry left unspecified must be 0.
//
// RmMIGBootConfigurationGI_N
//                           _FLAGS               - determines granularity of GPU partitioning. See NV2080_CTRL_CMD_GPU_SET_PARTITIONS
//                           _PLACEMENT_LO        - Optional placement span to allocate the partition into. Unused if HI<LO
//                           _PLACEMENT_HI        - Optional placement span to allocate the partition into. Unused if HI<LO
//                           _REQ_DEC_JPG_OFA     - For single slice instances, request at least 1 video decode, jpeg, and optical flow engine
//
// RmMIGBootConfigurationCI_N
//                           _FLAGS               - determines granularity of GPU partitioning. Subset of the GI flags - only the compute size flags are used here. See NV2080_CTRL_CMD_GPU_SET_PARTITIONS
//                           _PLACEMENT_LO        - Starting slice in GPU instance
//                           _CES                 - # Copy engines to share with other CIs in this GI. 0 denotes all available.
//                           _DECS                - # video decoder engines to share with other CIs in this GI. 0 denotes all available.
//                           _ENCS                - # video decoder engines to share with other CIs in this GI. 0 denotes all available.
//                           _JPGS                - # jpeg engines to share with other CIs in this GI. 0 denotes all available.
//                           _OFAS                - # optical flow engines to share with other CIs in this GI. 0 denotes all available.
//
// RmMIGBootConfigurationCIAssignment
//                                   _GI(n)       - Assign Compute instance n to GPU instance entry according to this value
//
// RmMIGBootConfigurationFeatureFlags
//                                   _AUTO_UPDATE - Windows only. RM updates the regkeys at runtime as MIG configuration changes.
//

#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI(n)                               "RmMIGBootConfigurationGI_" #n
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI__SIZE                            8
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI_FLAGS                            7:0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI_PLACEMENT_LO                     15:8
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI_PLACEMENT_HI                     23:16
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_GI_REQ_DEC_JPG_OFA                  31:31

#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI(n)                               "RmMIGBootConfigurationCI_" #n
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI__SIZE                            8
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_FLAGS                            7:0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_PLACEMENT_LO                     11:8
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_CES                              15:12
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_DECS                             19:16
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ENCS                             23:20
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_JPGS                             27:24
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_OFAS                             31:28

#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT                       "RmMIGBootConfigurationCIAssignment"
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT_GI(n)                 ((4 * ((n) + 1)) - 1):(4 * (n))
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_CI_ASSIGNMENT_GI__SIZE              8

#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS                       "RmMIGBootConfigurationFeatureFlags"
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_SUPPORTED             0:0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_SUPPORTED_DEFAULT     0x0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_SUPPORTED_FALSE       0x0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_SUPPORTED_TRUE        0x1
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_AUTO_UPDATE           1:1
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_AUTO_UPDATE_DEFAULT   0x0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_AUTO_UPDATE_DISABLED  0x0
#define NV_REG_STR_RM_MIG_BOOT_CONFIGURATION_FEATURE_FLAGS_AUTO_UPDATE_ENABLED   0x1

//
// Type DWORD
// This regkey toggles whether to enable support for MIG With GFX (SMG)
//
// 0 - Disable MIG with GFX (SMG)
// 1 - Enable MIG with GFX (SMG)
//
#define NV_REG_STR_RM_ENABLE_MIG_GFX                  "RmEnableMIGGfx"
#define NV_REG_STR_RM_ENABLE_MIG_GFX_DISABLED          0
#define NV_REG_STR_RM_ENABLE_MIG_GFX_ENABLED           1

//
// Type: DWORD
//
// If the midpath spinning feature of the GPU lock is enabled.
//
// 0 (default) - Midpath Spinning disabled
// 1 - Midpath spinning enabled
//
#define NV_REG_STR_RM_GPU_LOCK_MIDPATH                "RMGpuLockMidpath"
#define NV_REG_STR_RM_GPU_LOCK_MIDPATH_DISABLED       0x00000000
#define NV_REG_STR_RM_GPU_LOCK_MIDPATH_ENABLED        0x00000001

//
// Type: DWORD (Boolean)
//
// If the forced shared (read) lock feature is enabled.
//
// 0 - The forced shared (read) lock is not enabled.
// 1 (default) - The forced shared (read) lock is enabled.
//
#define NV_REG_STR_RM_ENABLE_FORCE_SHARED_LOCK        "RMEnableForceSharedLock"
#define NV_REG_STR_RM_ENABLE_FORCE_SHARED_LOCK_NO     0x00000000
#define NV_REG_STR_RM_ENABLE_FORCE_SHARED_LOCK_YES    0x00000001

//
// This regkey controls the GPU load failure test.
// Supported only on DEBUG, DEVELOP, or RELEASE drivers built with the parameter INSTRUMENT_RM=true
// This is an input/output registry key.
// NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_START: input - start the text at the specified stage and engine index.
//   Typically when the test starts, it is with NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PREINIT + ENGINEINDEX = 0
// NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_RUNNING: output - the test is running.
//   The next stage and engine index are specified
//   The test executable just needs to change NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_NEXT to _START for the next step
// NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_FINISHED: output - there is no morre stages and engines to test
//
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST                    "RmGpuLoadFailureTest"
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS             1:0
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_NONE        0x00000000
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_START       0x00000001
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_RUNNING     0x00000002
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STATUS_FINISHED    0x00000003
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE              4:2
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PREINIT      0x00000000
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_INIT         0x00000001
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PRELOAD      0x00000002
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_LOAD         0x00000003
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_POSTLOAD     0x00000004
#define NV_REG_STR_GPU_LOAD_FAILURE_TEST_ENGINEINDEX        31:5

//
// Type: DWORD
// Encoding:
// 0 - Disable MIG auto online mode on driver load
// 1 - Enable MIG auto online mode on driver load
//
#define NV_REG_STR_RM_SET_MIG_AUTO_ONLINE_MODE              "RMSetMIGAutoOnlineMode"
#define NV_REG_STR_RM_SET_MIG_AUTO_ONLINE_MODE_DISABLED     0
#define NV_REG_STR_RM_SET_MIG_AUTO_ONLINE_MODE_ENABLED      1

//
// Type: DWORD
// Encoding:
// 0 - Disable multi gpu mode
// 1 - Enable protected pcie
//
#define NV_REG_STR_RM_PPCIE_ENABLED                         "RmEnableProtectedPcie"
#define NV_REG_STR_RM_PPCIE_ENABLED_NO                      0x00000000
#define NV_REG_STR_RM_PPCIE_ENABLED_YES                     0x00000001

//
// Type: DWORD
// Encoding:
// 0 - Disable CC multi gpu mode with NVLE
// 1 - Enable  CC multi gpu mode with NVLE
//
#define NV_REG_STR_RM_CC_MULTI_GPU_NVLE_MODE_ENABLED                        "RmCCMultiGpuNvle"
#define NV_REG_STR_RM_CC_MULTI_GPU_NVLE_MODE_ENABLED_NO                      0x00000000
#define NV_REG_STR_RM_CC_MULTI_GPU_NVLE_MODE_ENABLED_YES                     0x00000001

// This regkey allows RM to access CPR vidmem over BARs when HCC devtools mode is ON
#define NV_REG_STR_RM_FORCE_BAR_ACCESS_ON_HCC               "RmForceBarAccessOnHcc"
#define NV_REG_STR_RM_FORCE_BAR_ACCESS_ON_HCC_NO            0x00000000
#define NV_REG_STR_RM_FORCE_BAR_ACCESS_ON_HCC_YES           0x00000001

//
// TYPE DWORD
// This regkey allows to change the state of NVENC sessions stats reporting.
// Note : Currently only used and works for Grid.
// 0 - Disable NVENC session stats reporting.
// 1 - Enable NVENC session stats reporting.
//
#define NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE                  "EncSessionStatsReportingState"
#define NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_DISABLED         0x00000000
#define NV_REG_STR_RM_NVENC_SESSION_STATS_REPORTING_STATE_ENABLED          0x00000001

// TYPE DWORD
// Set to provide ECC state in guest
// Used for vGPU
// The value default is set if ECC is enabled in USM profile.
//
#define NV_REG_STR_RM_GUEST_ECC_STATE                               "RMGuestECCState"
#define NV_REG_STR_RM_GUEST_ECC_STATE_DISABLED                             0x00000000
#define NV_REG_STR_RM_GUEST_ECC_STATE_ENABLED                              0x00000001
#define NV_REG_STR_RM_GUEST_ECC_STATE_DEFAULT                              0x00000001

//
// TYPE DWORD
// This regkey allows to enable RM test only code paths
// 0 - Disable RM test only code
// 1 - Enable RM test only code
//
#define NV_REG_STR_RM_ENABLE_RM_TEST_ONLY_CODE                  "EnableRmTestOnlyCode"
#define NV_REG_STR_RM_ENABLE_RM_TEST_ONLY_CODE_DISABLED         0x00000000
#define NV_REG_STR_RM_ENABLE_RM_TEST_ONLY_CODE_ENABLED          0x00000001

//
// Type DWORD
// This regkey force-disables write-combine iomap allocations, used for chipsets where
// write-combine is broken.
//
#define NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC             "RmForceDisableIomapWC"
#define NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC_YES         0x00000001
#define NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC_NO          0x00000000
#define NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC_DEFAULT     NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC_NO

//
// TYPE DWORD
// This regkey will increase the margin after the end of WPR2 when booting GSP-RM.
//
// This margin can be used to help GSP firmware boot in the presence of ECC
// errors which might affect the default GSP firmware image location in the GPU
// framebuffer. If GPU firmware is able to successfully boot with this registry
// key enabled, it should scan the margin area to attempt to handle ECC errors in
// the region, so that the region can be safely used in a subsequent boot.
//
// NV_REG_RM_GSP_WPR_END_MARGIN_MB
// Possible values:
//  0  - (Default) use the default calculated GSP WPR size
//  1+ - size of the end margin in megabytes
//
// NV_REG_RM_GSP_WPR_END_MARGIN_APPLY
// Possible values:
//  _ON_RETRY (0) - (Default) only increase the margin to the requested size
//                  when retrying GSP firmware boot after a failed boot attempt
//  _ALWAYS   (1) - increase the margin to the requested size for all GSP
//                  firmware boot attempts, including the first
//
#define NV_REG_STR_RM_GSP_WPR_END_MARGIN                    "RmGspWprEndMargin"
#define NV_REG_RM_GSP_WPR_END_MARGIN_MB                     30:0
#define NV_REG_RM_GSP_WPR_END_MARGIN_APPLY                  31:31
#define NV_REG_RM_GSP_WPR_END_MARGIN_APPLY_ON_RETRY         0x00000000
#define NV_REG_RM_GSP_WPR_END_MARGIN_APPLY_ALWAYS           0x00000001

//
// Type DWORD
// Regkey to set the initial number of firmware shifts for GSP-RM boot.
//
// Depending on where and when GSP-RM bootup fails, re-attempts may fail.
// This regkey allows the driver to pre-emptively shift GSP-RM firmware
// on the first boot, breaking a cycle of: attempting boot, failing,
// re-attempts barred, rebooting system, attempting boot, etc...
//
// The value, n, of this this regkey represents n shifts of size RmGspWprEndMargin.
//
// On Windows, the driver increments this key after each failed attempt and
//             the value rolls over to default after hitting max.
// On Linux, the user must manually set this regkey when re-attempts fail.
//
// Note, this regkey is still bounded by NV_REG_STR_RM_GSP_BOOT_RETRY_ATTEMPTS.
// Any value greater than max attempts will be modulo-ed.
//
#define NV_REG_STR_RM_GSP_BOOT_INITIAL_SHIFT              "RmGspBootInitialShift"
#define NV_REG_STR_RM_GSP_BOOT_INITIAL_SHIFT_DEFAULT      0

//
// Type DWORD
// Regkey to toggle memory integrity scan during GSP-RM bootup
// for GeForce/non-ECC config.
//
#define NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN             "RmGspScanWprEndMargin"
#define NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN_DEFAULT     0x00000001
#define NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN_ENABLE      0x00000001
#define NV_REG_STR_RM_GSP_SCAN_WPR_END_MARGIN_DISABLE     0x00000000

//
// Type DWORD
// Regkey to set the number of retry attempts at GSP bootstrap
//
#define NV_REG_STR_RM_GSP_BOOT_RETRY_ATTEMPTS             "RmGspBootRetryAttempts"
#define NV_REG_STR_RM_GSP_BOOT_RETRY_ATTEMPTS_DEFAULT     4

//
// Type DWORD
// Regkey to control placement of GSP firmawre's stack
// 0 - let RM decide (default)
// 1 - force use of libos task stack
// 2 - force use of dmem stack
//
#define NV_REG_STR_RM_GSP_STACK_PLACEMENT           "RmGspStackPlacement"
#define NV_REG_STR_RM_GSP_STACK_PLACEMENT_DEFAULT   0
#define NV_REG_STR_RM_GSP_STACK_PLACEMENT_LIBOS     1
#define NV_REG_STR_RM_GSP_STACK_PLACEMENT_DMEM      2

//
// Type: Dword
// This regkey toggles whether to release API lock during initialization to
// allow multiple GPUS to initialize in parallel
// 0 - API lock will not be released
// 1 - API lock will be released
// 2 - API lock release determined by platform (default)
//
#define NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING              "RmRelaxedGspInitLocking"
#define NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_DISABLE      0x00000000
#define NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_ENABLE       0x00000001
#define NV_REG_STR_RM_RELAXED_GSP_INIT_LOCKING_DEFAULT      0x00000002

//
// Regkey to configure Per VM RunList.
// Type Dword
//  BIT 0:0 - Overall PVMRL enable/disable.
//   0 - Disable / Default - 1 HW runlist per engine.
//   1 - Enable            - 1 SW runlist per VM for some engines.
//  BIT 1:1 - Adaptive Round Robin Scheduler
//   0 - Enable / Default - Use Adaptive Round Robin Scheduler
//   1 - Disable          - Use Legacy PVMRL
//  BIT 7:4 - PVMRL scheduler to run.
//   0 - equal share / Default - equal share amongst running vGPUs.
//   1 - fixed share           - fixed share of the physical GPU.
//  BIT 21:12 - PVMRL Scheduling frequency.
//   0 - Default timeslice.
//   F - Timeslice = 1000 / F.
//  BIT 23:16 - PVMRL timeslice in ms (Milli-seconds).
//   0 - Default timeslice.
//   T - Timeslice of T ms.
//  BIT 31:24 - ARR Average Factor
//   0 - Default Average Factor
//   F - Average Factor = F
//
#define NV_REG_STR_RM_PVMRL                                       "RmPVMRL"
#define NV_REG_STR_RM_PVMRL_ENABLE                                0:0
#define NV_REG_STR_RM_PVMRL_ENABLE_DEFAULT                        0x00000000
#define NV_REG_STR_RM_PVMRL_ENABLE_NO                             0x00000000
#define NV_REG_STR_RM_PVMRL_ENABLE_YES                            0x00000001
#define NV_REG_STR_RM_PVMRL_ARR_DISABLE                           1:1
#define NV_REG_STR_RM_PVMRL_ARR_DISABLE_DEFAULT                   0x00000000
#define NV_REG_STR_RM_PVMRL_ARR_DISABLE_NO                        0x00000000
#define NV_REG_STR_RM_PVMRL_ARR_DISABLE_YES                       0x00000001
#define NV_REG_STR_RM_PVMRL_SCHED_POLICY                          7:4
#define NV_REG_STR_RM_PVMRL_SCHED_POLICY_DEFAULT                  0x00000000
#define NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_EQUAL_SHARE         0x00000000
#define NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_FIXED_SHARE         0x00000001
#define NV_REG_STR_RM_PVMRL_FREQUENCY                             21:12
#define NV_REG_STR_RM_PVMRL_TIMESLICE                             23:16
#define NV_REG_STR_RM_PVMRL_AVERAGE_FACTOR                        31:24

//
// Type: Dword
// This regkey is used to enable Nvlink Encryption. By default it is disabled
//
#define NV_REG_STR_RM_NVLINK_ENCRYPTION                   "RmNvlinkEncryption"
#define NV_REG_STR_RM_NVLINK_ENCRYPTION_MODE              0:0
#define NV_REG_STR_RM_NVLINK_ENCRYPTION_MODE_DEFAULT      0x00000000
#define NV_REG_STR_RM_NVLINK_ENCRYPTION_MODE_ENABLE       0x00000001

//
// Type: Dword
// This regkey overrides the state of the GR scrubber channel and determines
// whether it should be created or not.
//
#define NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL             "RmForceGrScrubberChannel"
#define NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL_DISABLE     0x00000000
#define NV_REG_STR_RM_FORCE_GR_SCRUBBER_CHANNEL_ENABLE      0x00000001

//
// Type: Dword
//
// This regkey controls the per-runlist channel ID feature, which is only
// supported on Ampere+ HW
//
// On platforms or chips where this feature is supported in HW but not enabled
// by default, this regkey can override the default to enable it. It also
// allows turning off the feature on platforms or chips where it is already
// enabled by default.
//
// 0 - Force disable per-runlist channel IDs
// 1 - Force enable per-runlist channel IDs
//
#define NV_REG_STR_RM_DEBUG_OVERRIDE_PER_RUNLIST_CHANNEL_RAM         "RMDebugOverridePerRunlistChannelRam"
#define NV_REG_STR_RM_DEBUG_OVERRIDE_PER_RUNLIST_CHANNEL_RAM_DISABLE 0
#define NV_REG_STR_RM_DEBUG_OVERRIDE_PER_RUNLIST_CHANNEL_RAM_ENABLE  1

//
// Type: Dword
//
// This regkey controls RM User Shared Data polling for testing and debugging.
// supported on Ampere+ HW
//
// By default, RUSD polling is controlled through the allocation of the 00DE class
// with specified poll masks for specific data.
// Setting this key will force RUSD to poll all data, or no data.
//
// 0 - Default behavior
// 1 - Force disable all RUSD polling
// 2 - Force enable all RUSD polling
//
#define NV_REG_STR_RM_DEBUG_RUSD_POLLING                  "RMDebugRusdPolling"
#define NV_REG_STR_RM_DEBUG_RUSD_POLLING_DEFAULT          0
#define NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_DISABLE    1
#define NV_REG_STR_RM_DEBUG_RUSD_POLLING_FORCE_ENABLE     2

//
// Type: Dword
//
// This regkey controls RM User Shared Data polling frequency
//
// By default, RUSD polls requested data once every 100ms.
// Decreasing this interval will make RUSD update data more often, providing more recent data,
// but may result in increased GPU power usage.
//
// Interval is specified in milliseconds
// Default - 100
// Minimum - 100
// Maximum - 1000
//
#define NV_REG_STR_RM_RUSD_POLLING_INTERVAL                  "RMRusdPollingInterval"
#define NV_REG_STR_RM_RUSD_POLLING_INTERVAL_DEFAULT          500
#define NV_REG_STR_RM_RUSD_POLLING_INTERVAL_TESLA            100
#define NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MIN              100
#define NV_REG_STR_RM_RUSD_POLLING_INTERVAL_MAX              1000

//
// Type: DWORD
// This regkey enables some path to reuse the initially allocated continguous memory
// instead of freeing it dynamically which causes memory fragmentation and
// under low available memory configuration the allocation could fail and leads to BSOD
// Enable the key to reuse the initially allocated mem withoug free and reallocate
//   - Default for Blackwell and up
// Disable the key to free and allocate dynamically
//
#define NV_REG_STR_RM_INIT_MEM_REUSE                           "RmInitMemReuse"
#define NV_REG_STR_RM_INIT_MEM_REUSE_ENABLE                     0x00000001
#define NV_REG_STR_RM_INIT_MEM_REUSE_DISABLE                    0x00000000

//
// Type DWORD (Boolean)
// This regkey controls the use of BAR1 SPA instead of GPA for p2p subsystems
// like dma-buf and nv-p2p for coherent systems with a direct PCIe link between
// the GPU and another device in virtualized environments.
// By default, the regkey shall be set to false to always use GPA.
//
#define NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA          "RmGpuDirectRdmaForceSPA"
#define NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_YES      (0x00000001)
#define NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_NO       (0x00000000)
#define NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_DEFAULT  NV_REG_STR_RM_GPUDIRECT_RDMA_FORCE_SPA_NO

// Type DWORD (Boolean)
// Disable the check for FSP's fuse error detection status during boot.
// By default, the check would be enabled and we would bail out during boot on error.
#define NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK           "RmDisableFspFuseErrorCheck"
#define NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK_YES       (0x00000001)
#define NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK_NO        (0x00000000)
#define NV_REG_STR_RM_DISABLE_FSP_FUSE_ERROR_CHECK_DEFAULT   (0x00000000)

//
// TYPE Dword
// Regkey to set the simulated path for TDR escape.
// With corresponding bit set the return status will be set to error for simulation.
// The error resume sequence is bus , ucode and power call
//
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE                      "RMTdrSimulatedEscape"
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_NONE                  0
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_POWER_ERROR           0:0
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_POWER_ERROR_TRUE      0x00000001
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_POWER_ERROR_FALSE     0x00000000
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_BUS_ERROR             1:1
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_BUS_ERROR_TRUE        0x00000001
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_BUS_ERROR_FALSE       0x00000000
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_UCODE_ERROR           2:2
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_UCODE_ERROR_TRUE      0x00000001
#define NV_REG_STR_RM_TDR_SIMULATED_ESCAPE_UCODE_ERROR_FALSE     0x00000000

// TYPE DWORD
// Enable mask for GSPLITE engines
//
#define NV_REG_STR_RM_GSPLITE_ENABLE_MASK                         "RmGspliteEnableMask"
#define NV_REG_STR_RM_GSPLITE_DISABLE_ALL                         (0x00000000)
#define NV_REG_STR_RM_GSPLITE_ENABLE_ALL                          (0xFFFFFFFF)
#define NV_REG_STR_RM_GSPLITE_ENABLE_MASK_DEFAULT                 NV_REG_STR_RM_GSPLITE_DISABLE_ALL

// Type DWORD (Boolean)
// Forcefully enables the HWPM VA space multiple mapping for MODS testing purpose.
#define NV_REG_STR_RM_HWPM_MUTI_VA_MAPPING                     "RmHwpmForceMultiMapping"
#define NV_REG_STR_RM_HWPM_MUTI_VA_MAPPING_TRUE                 (0x00000001)
#define NV_REG_STR_RM_HWPM_MUTI_VA_MAPPING_FALSE                (0x00000000)

// Type DWORD (Boolean)
// Enable the extended buffer for HWPM PMA channel.
#define NV_REG_STR_RM_HWPM_EXTENDED_BUFFER                     "RmHwpmExtendedBuffer"
#define NV_REG_STR_RM_HWPM_EXTENDED_BUFFER_TRUE                 (0x00000001)
#define NV_REG_STR_RM_HWPM_EXTENDED_BUFFER_FALSE                (0x00000000)


//
// Type DWORD:
// Secure bootflow is needed to bootstrap DEVINIT in LS mode where IFR is suppressed
// This is achieved by executing DEVINIT by SECURE_BOOT command of FWSECLIC
// _ENABLE: Enable Secure Boot flow
// _DISABLE: Disable Secure Boot flow
// default is _DISABLE
#define NV_REG_STR_RM_DEVINIT_BY_SECURE_BOOT                     "RMDevinitBySecureBoot"
#define NV_REG_STR_RM_DEVINIT_BY_SECURE_BOOT_ENABLE              1
#define NV_REG_STR_RM_DEVINIT_BY_SECURE_BOOT_DISABLE             0

//
// Type: DWORD
// Regkey to enable/disable FB sanity check after FSP secure boot complete
// _ENABLE: Enable FB sanity check
// _DISABLE: Disable FB sanity check
// default is _DISABLE
//
#define NV_REG_STR_RM_FB_SANITY_CHECK                             "RmFbSanityCheck"
#define NV_REG_STR_RM_FB_SANITY_CHECK_ENABLE                      (0x00000001)
#define NV_REG_STR_RM_FB_SANITY_CHECK_DISABLE                     (0x00000000)
#define NV_REG_STR_RM_FB_SANITY_CHECK_DEFAULT                     NV_REG_STR_RM_FB_SANITY_CHECK_DISABLE

// Type DWORD
// If set, RM will Align the circular buffer size up to 64k
#define NV_REG_STR_RM_64K_BUG_5123775_WAR                         "RM64kBug5123775War"

#endif // NVRM_REGISTRY_H
