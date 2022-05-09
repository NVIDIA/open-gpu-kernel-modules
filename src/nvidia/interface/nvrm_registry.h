/*
 * SPDX-FileCopyrightText: Copyright (c) 1997-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NV_REG_STR_RM_DISABLE_GSP_OFFLOAD                   "RmDisableGspOffload"
#define NV_REG_STR_RM_DISABLE_GSP_OFFLOAD_FALSE             (0x00000000)
#define NV_REG_STR_RM_DISABLE_GSP_OFFLOAD_TRUE              (0x00000001)
// Type DWORD (Boolean)
// Override any other settings and disable GSP-RM offload.


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
// Type: Dword
// Encoding:
// 1 - Enable remote GPU
// 0 - Disable remote GPU
//
#define NV_REG_STR_RM_REMOTE_GPU                           "RMRemoteGpu"
#define NV_REG_STR_RM_REMOTE_GPU_ENABLE                    0x00000001
#define NV_REG_STR_RM_REMOTE_GPU_DISABLE                   0x00000000
#define NV_REG_STR_RM_REMOTE_GPU_DEFAULT                   NV_REG_STR_RM_REMOTE_GPU_DISABLE


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


// Enable support for CACHEABLE rmapi control flag
// 0: never cache any controls
// 1 (default): cache only ROUTE_TO_PHYSICAL controls, and only if GSP-RM is running
// 2: cache all controls
#define NV_REG_STR_RM_CACHEABLE_CONTROLS             "RmEnableCacheableControls"
#define NV_REG_STR_RM_CACHEABLE_CONTROLS_DISABLE     0
#define NV_REG_STR_RM_CACHEABLE_CONTROLS_GSP_ONLY    1
#define NV_REG_STR_RM_CACHEABLE_CONTROLS_ENABLE      2

// Type DWORD
// This regkey forces for Maxwell+ that on FB Unload we wait for FB pull before issuing the
// L2 clean. WAR for bug 1032432
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL                                    "RmL2CleanFbPull"
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_ENABLED                            (0x00000000)
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_DISABLED                           (0x00000001)
#define NV_REG_STR_RM_L2_CLEAN_FB_PULL_DEFAULT                            (0x00000000)

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

#define NV_REG_STR_BUG_1698088_WAR                            "RMBug1698088War"
#define NV_REG_STR_BUG_1698088_WAR_ENABLE                     0x00000001
#define NV_REG_STR_BUG_1698088_WAR_DISABLE                    0x00000000
#define NV_REG_STR_BUG_1698088_WAR_DEFAULT                    NV_REG_STR_BUG_1698088_WAR_DISABLE

//
// TYPE DWORD
// This regkey can be used to ignore upper memory on GM20X and later. If there
// is upper memory but this regkey is set to _YES, then RM will only expose the
// lower memory to clients.
//
// DEFAULT - Use the default setting of upper memory on GM20X-and-later.
// YES     - Ignore upper memory on GM20X-and-later.
//
#define NV_REG_STR_RM_IGNORE_UPPER_MEMORY           "RMIgnoreUpperMemory"
#define NV_REG_STR_RM_IGNORE_UPPER_MEMORY_DEFAULT   (0x00000000)
#define NV_REG_STR_RM_IGNORE_UPPER_MEMORY_YES       (0x00000001)

#define NV_REG_STR_RM_NO_ECC_FB_SCRUB                        "RMNoECCFBScrub"

#define  NV_REG_STR_RM_DISABLE_SCRUB_ON_FREE          "RMDisableScrubOnFree"
// Type DWORD
// Encoding 0 (default) - Scrub on free
//          1           - Disable Scrub on Free

#define NV_REG_STR_RM_INIT_SCRUB                          "RMInitScrub"
// Type DWORD
// Encoding 1 - Scrub Fb during rminit irrespective of ECC capability

#define NV_REG_STR_RM_DISABLE_ASYNC_MEM_SCRUB         "RMDisableAsyncMemScrub"
// Type DWORD
// Encoding 0 (default) - Async memory scrubbing is enabled
//          1           - Async memory scrubbing is disabled

#define NV_REG_STR_RM_INCREASE_ECC_SCRUB_TIMEOUT         "RM1441072"
// Type DWORD
// Encoding 0 (default) - Use default ECC Scrub Timeout
//          1           - Increase ECC Scrub Timeout

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
// Type DWORD
// Controls enable of Address Tree memory tracking instead of regmap
// for the PMA memory manager.
//
#define NV_REG_STR_RM_ENABLE_ADDRTREE       "RMEnableAddrtree"
#define NV_REG_STR_RM_ENABLE_ADDRTREE_YES   (0x00000001)
#define NV_REG_STR_RM_ENABLE_ADDRTREE_NO    (0x00000000)

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

#define NV_REG_STR_RM_NUM_FIFOS                        "RmNumFifos"
// Type Dword
// Override number of fifos (channels) on NV4X
#define NV_REG_STR_RM_NUM_FIFOS_COMPAT                 0x00000020
#define NV_REG_STR_RM_NUM_FIFOS_EXTENDED               0x00000200

#define NV_REG_STR_RM_SUPPORT_USERD_MAP_DMA     "RMSupportUserdMapDma"
// Type DWORD
// Encoding: Boolean
// If set, allow MapMemoryDma calls to be made on channel objects

//
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

//
// Type DWORD
// Use this regkey to force RM to pick a P2P type. HW has to support the picked TYPE to take effect.
// e.g., TYPE_BAR1P2P will not work if HW does not support it. A call to create NV50_P2P object will
// will fail in such a case.
//
// TYPE_DEFAULT let RM to choose a P2P type. The priority is:
//              C2C > NVLINK > BAR1P2P > mailbox P2P
//
// TYPE_C2C to use C2C P2P if it supports
// TYPE_NVLINK to use NVLINK P2P, including INDIRECT_NVLINK_P2P if it supports
// TYPE_BAR1P2P to use BAR1 P2P if it supports
// TYPE_MAILBOXP2P to use mailbox p2p if it supports
//
#define NV_REG_STR_RM_FORCE_P2P_TYPE                           "RMForceP2PType"
#define NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT                   (0x00000000)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_MAILBOXP2P                (0x00000001)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_BAR1P2P                   (0x00000002)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_NVLINK                    (0x00000003)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_C2C                       (0x00000004)
#define NV_REG_STR_RM_FORCE_P2P_TYPE_MAX                       NV_REG_STR_RM_FORCE_P2P_TYPE_C2C

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

#define NV_REG_STR_RM_BAR2_APERTURE_SIZE_MB                  "RMBar2ApertureSizeMB"
// Type DWORD
// Encoding Numeric Value
// Overrides the size of the BAR2 aperture.  Cannot be greater than the
// physical size of BAR2 available to RM (which may be less than the total size
// of BAR2).  When this regkey is present we cap the total aperture size to the
// RM aperture size. This can result in undefined beahvior in environments that
// rely on a virtual bar2 aperture shared between RM and VBIOS for VESA support.

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
//
// TYPE DWORD
// This setting will override the BAR1 Big page size
// This is used for interop testing for MODS
//
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE                  "RMSetBAR1AddressSpaceBigPageSize"
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE_64k              (64 * 1024)
#define NV_REG_STR_RM_SET_BAR1_ADDRESS_SPACE_BIG_PAGE_SIZE_128k             (128 * 1024)
#endif //DEVELOP || DEBUG || NV_MODS

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

#define NV_REG_STR_RM_LOCKING_MODE              "RMLockingMode"
// Type DWORD
// Encoding enum
// Overrides what Locking Mode is in use.
// Default 0
#define NV_REG_STR_RM_LOCKING_MODE_DEFAULT               (0x00000000)
#define NV_REG_STR_RM_LOCKING_MODE_INTR_MASK             (0x00000001)
#define NV_REG_STR_RM_LOCKING_MODE_LAZY_INTR_DISABLE     (0x00000002)

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
#define NV_REG_STR_RM_NVLINK_CONTROL_RESERVED_0                     7:3
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
#define NV_REG_STR_RM_NVLINK_MINION_CONTROL_RESERVED_0                31:20

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
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_FAULT               (0x00000013)
#define NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED__LAST               (0x00000013)

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
#define NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL_RESERVED_0                 5:4
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

#endif // NVRM_REGISTRY_H
