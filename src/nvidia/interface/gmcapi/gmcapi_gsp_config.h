/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_GSP_CONFIG_H
#define GMCAPI_GSP_CONFIG_H

/* Key/Value definitions for GMCAPI_GSP_INIT static info response */

/* 32-bit key/value pairs */
#define NVGMC_SC_SKU_BOARD_ID                                                  0x0001
#define NVGMC_SC_SKU_CONFIG_VERSION                                            0x0002
#define NVGMC_SC_SKU_BUSINESS_CYCLE                                            0x0003
#define NVGMC_SC_CHIP_SUB_REV                                                  0x0004
#define NVGMC_SC_EMULATION_REV1                                                0x0005

/* Encoding style open.  When sending a series of indexed key/value pairs, should we send
   a separate key/value pair that indicates how many total indexed values are being sent?
   In theory this can be inferred by tracking the maximum sent index across all related
   key/value pairs.  But, it's a bit cumbersome to do that (and have to ensure that they
   all line up.  If a separate key is sent up front, then that value can be verified to be
   in an expected range just once and then stored into another variable that's checked
   against later as the individual key/values are decoded.  It would also allow for more
   dynamic allocations on the receiver side. */
#define NVGMC_SC_FB_REGION_COUNT                                               0x0010
#define NVGMC_SC_FB_REGION_PERFORMANCE                                         0x0011

#define NVGMC_SC_FB_REGION_FLAGS                                               0x0012
#define NVGMC_SC_FB_REGION_FLAGS_IS_COMPRESSION_SUPPORTED                      0:0
#define NVGMC_SC_FB_REGION_FLAGS_IS_ISO_SUPPORTED                              1:1
#define NVGMC_SC_FB_REGION_FLAGS_IS_PROTECTED                                  2:2

#define NVGMC_SC_FB_REGION_TAG                                                 0x0013
#define NVGMC_SC_FB_REGION_TAG_NONE                                            0x00000000
#define NVGMC_SC_FB_REGION_TAG_GSP_CARVEOUT                                    0x00000001
#define NVGMC_SC_FB_REGION_TAG_CPU_RM_RESERVED                                 0x00000002
#define NVGMC_SC_FB_REGION_TAG_CPU_RM_RESERVED_HEAP                            0x00000003
#define NVGMC_SC_FB_REGION_TAG_GSP_RM_RESERVED                                 0x00000004
#define NVGMC_SC_FB_REGION_TAG_GSP_RM_RESERVED_HEAP                            0x00000005

#define NVGMC_SC_PDI                                                           0x0020

#define NVGMC_SC_GC6_RTD3_TOTAL_BOARD_POWER                                    0x0030
#define NVGMC_SC_GC6_RTD3_PERST_DELAY                                          0x0031

#define NVGMC_SC_VBIOS_SUB_VENDOR                                              0x0050
#define NVGMC_SC_VBIOS_SUB_DEVICE                                              0x0051
#define NVGMC_SC_VBIOS_REVISION                                                0x0052
#define NVGMC_SC_VBIOS_OEM_REVISION                                            0x0053

#define NVGMC_SC_SRIOV_MAX_GFID                                                0x0200

#define NVGMC_SC_VGX_VERSION                                                   0x0210

/* Note: These are only valid if using RMAPI RPCs with GSP and should be avoided/ignored
   as any RPCs that rely on them are considered ABI unstable. */
#define NVGMC_SC_INTERNAL_CLIENT_HANDLE                                        0x0FF0
#define NVGMC_SC_INTERNAL_DEVICE_HANDLE                                        0x0FF1
#define NVGMC_SC_INTERNAL_SUBDEVICE_HANDLE                                     0x0FF2

/* 64-bit key/value pairs */
#define NVGMC_SC_BRANDING                                                      0x1000
#define NVGMC_SC_BRANDING_IS_QUADRO_GENERIC                                    0:0
#define NVGMC_SC_BRANDING_IS_QUADRO_AD                                         1:1
#define NVGMC_SC_BRANDING_IS_NVIDIA_NVS                                        2:2
#define NVGMC_SC_BRANDING_IS_VGX                                               3:3
#define NVGMC_SC_BRANDING_IS_TESLA                                             4:4
#define NVGMC_SC_BRANDING_IS_TITAN                                             5:5
#define NVGMC_SC_BRANDING_IS_MOBILE                                            6:6
#define NVGMC_SC_BRANDING_IS_GEFORCE_SMB                                       7:7
#define NVGMC_SC_BRANDING_IS_CMP_SKU                                           8:8

#define NVGMC_SC_MISC_FEATURES                                                 0x1001
#define NVGMC_SC_MISC_FEATURES_IS_MIG_SUPPORTED                                0:0
#define NVGMC_SC_MISC_FEATURES_IS_SELF_HOSTED                                  1:1
#define NVGMC_SC_MISC_FEATURES_IS_ATS_SUPPORTED                                2:2
#define NVGMC_SC_MISC_FEATURES_IS_GPU_UEFI                                     3:3
#define NVGMC_SC_MISC_FEATURES_IS_EFI_INIT                                     4:4
#define NVGMC_SC_MISC_FEATURES_IS_BUS_RESET_REQUIRED                           5:5
#define NVGMC_SC_MISC_FEATURES_IS_PAGE_RETIREMENT_SUPPORTED                    6:6
#define NVGMC_SC_MISC_FEATURES_IS_SPLIT_VAS_BETWEEN_SERVER_CLIENT_RM           7:7
#define NVGMC_SC_MISC_FEATURES_IS_CL_ROOTPORT_NEEDS_NOSNOOP_WAR                8:8
#define NVGMC_SC_MISC_FEATURES_IS_POISON_FUSE_ENABLED                          9:9
#define NVGMC_SC_MISC_FEATURES_IS_SYS_L2_CACHE_COHERENT                        10:10

#define NVGMC_SC_POWER_FEATURES                                                0x1002
#define NVGMC_SC_POWER_FEATURES_IS_GC6_RTD3_ALLOWED                            0:0
#define NVGMC_SC_POWER_FEATURES_IS_GC8_RTD3_ALLOWED                            1:1
#define NVGMC_SC_POWER_FEATURES_IS_GCOFF_RTD3_ALLOWED                          2:2
#define NVGMC_SC_POWER_FEATURES_IS_GCOFF_LEGACY_ALLOWED                        3:3

#define NVGMC_SC_FB_LENGTH                                                     0x1010
#define NVGMC_SC_FB_REGION_BASE                                                0x1011
#define NVGMC_SC_FB_REGION_LIMIT                                               0x1012
#define NVGMC_SC_FB_REGION_RESERVED                                            0x1013

// ECID info is transmitted as an array of 2 64-bit values
#define NVGMC_SC_ECID_INFO                                                     0x1040

#define NVGMC_SC_VMMU_SEGMENT_SIZE                                             0x1050

/* NVGMC_SC_ENGINE_MASK encodes the set of available engines of a given type,
   indexed by NVGMC_ENGINE_TYPE. The starting engine type index is not guaranteed
   to be 0, so care should be taken when processing data for this key. */
#define NVGMC_SC_ENGINE_MASK                                                   0x1100

/* Use separate keys (rather than key indices) because BARs really are not
   orthogonal/indexable in the normal sense and have specific uses/meanings. */
#define NVGMC_SC_BAR1_PDE_BASE                                                 0x1020
#define NVGMC_SC_BAR2_PDE_BASE                                                 0x1021

#define NVGMC_SC_WPR_LAYOUT_NON_WPR_HEAP_OFFSET                                0x1080
#define NVGMC_SC_WPR_LAYOUT_FRTS_OFFSET                                        0x1081

/* Similar to the PDE_BASE keys above, we do not use indexed keys for these */
#define NVGMC_SC_SRIOV_BAR0_SIZE                                               0x1200
#define NVGMC_SC_SRIOV_BAR1_SIZE                                               0x1201
#define NVGMC_SC_SRIOV_BAR2_SIZE                                               0x1202

/* Byte array key/value pairs*/
#define NVGMC_SC_GPU_NAME_STRING                                               0x2000
#define NVGMC_SC_GPU_SHORT_NAME_STRING                                         0x2001
#define NVGMC_SC_GPU_GID_SHA1                                                  0x2002 /* 16-byte SHA1 hash of the GPU's GID . */
#define NVGMC_SC_CHIP_SKU                                                      0x2003
#define NVGMC_SC_CHIP_SKU_MODIFIER                                             0x2004
#define NVGMC_SC_PROJECT                                                       0x2005
#define NVGMC_SC_PROJECT_SKU                                                   0x2006
#define NVGMC_SC_PROJECT_SKU_MODIFIER                                          0x2007
#define NVGMC_SC_CDP                                                           0x2008

#endif // GMCAPI_GSP_CONFIG_H
