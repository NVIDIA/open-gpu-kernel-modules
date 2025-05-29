/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000system.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl0000/ctrl0000base.h"

/* NV01_ROOT (client) system control commands and parameters */

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_FEATURES
 *
 * This command returns a mask of supported features for the SYSTEM category
 * of the 0000 class.
 *
 *     Valid features include:
 *         
 *       NV0000_CTRL_GET_FEATURES_SLI
 *         When this bit is set, SLI is supported.
 *       NV0000_CTRL_SYSTEM_GET_FEATURES_IS_EFI_INIT
 *         When this bit is set, EFI has initialized core channel 
 *       NV0000_CTRL_SYSTEM_GET_FEATURES_RM_TEST_ONLY_CODE_ENABLED
 *         When this bit is set, RM test only code is supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_FEATURES (0x1f0U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS_MESSAGE_ID (0xF0U)

typedef struct NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS {
    NvU32 featuresMask;
} NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS;



/* Valid feature values */
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI                                 0:0
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI_FALSE                       (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI_TRUE                        (0x00000001U)

#define NV0000_CTRL_SYSTEM_GET_FEATURES_UUID_BASED_MEM_SHARING              3:3
#define NV0000_CTRL_SYSTEM_GET_FEATURES_UUID_BASED_MEM_SHARING_FALSE    (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_UUID_BASED_MEM_SHARING_TRUE     (0x00000001U)

#define NV0000_CTRL_SYSTEM_GET_FEATURES_RM_TEST_ONLY_CODE_ENABLED              4:4
#define NV0000_CTRL_SYSTEM_GET_FEATURES_RM_TEST_ONLY_CODE_ENABLED_FALSE (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_RM_TEST_ONLY_CODE_ENABLED_TRUE  (0x00000001U)
/*
 * NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION
 *
 * This command returns the current driver information.
 * The first time this is called the size of strings is
 * set with the greater of NV_BUILD_BRANCH_VERSION and
 * NV_DISPLAY_DRIVER_TITLE. The client then allocates memory
 * of size sizeOfStrings for pVersionBuffer and pTitleBuffer
 * and calls the command again to receive driver info.
 *
 *   sizeOfStrings
 *       This field returns the size in bytes of the pVersionBuffer and
 *       pTitleBuffer strings.
 *   pDriverVersionBuffer
 *       This field returns the version (NV_VERSION_STRING).
 *   pVersionBuffer
 *       This field returns the version (NV_BUILD_BRANCH_VERSION).
 *   pTitleBuffer
 *       This field returns the title (NV_DISPLAY_DRIVER_TITLE).
 *   changelistNumber
 *       This field returns the changelist value (NV_BUILD_CHANGELIST_NUM).
 *   officialChangelistNumber
 *       This field returns the last official changelist value
 *       (NV_LAST_OFFICIAL_CHANGELIST_NUM).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION                        (0x101U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS {
    NvU32 sizeOfStrings;
    NV_DECLARE_ALIGNED(NvP64 pDriverVersionBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pVersionBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pTitleBuffer, 8);
    NvU32 changelistNumber;
    NvU32 officialChangelistNumber;
} NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS;

typedef enum NV0000_CTRL_SYSTEM_SH_SOC_TYPE {
    NV0000_CTRL_SYSTEM_SH_SOC_TYPE_NA = 0,
    NV0000_CTRL_SYSTEM_SH_SOC_TYPE_NV_GRACE = 1,

} NV0000_CTRL_SYSTEM_SH_SOC_TYPE;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_CPU_INFO
 *
 * This command returns system CPU information.
 *
 *   type
 *     This field returns the processor type.
 *     Legal processor types include:
 *       Intel processors:
 *         P55       : P55C - MMX
 *         P6        : PPro
 *         P2        : PentiumII
 *         P2XC      : Xeon & Celeron
 *         CELA      : Celeron-A
 *         P3        : Pentium-III
 *         P3_INTL2  : Pentium-III w/integrated L2 (fullspeed, on die, 256K)
 *         P4        : Pentium 4
 *         CORE2     : Core2 Duo Conroe
 *       AMD processors
 *         K62       : K6-2 w/ 3DNow
 *       IDT/Centaur processors
 *         C6        : WinChip C6
 *         C62       : WinChip 2 w/ 3DNow
 *       Cyrix processors
 *         GX        : MediaGX
 *         M1        : 6x86
 *         M2        : M2
 *         MGX       : MediaGX w/ MMX
 *       Transmeta processors
 *         TM_CRUSOE : Transmeta Crusoe(tm)
 *       PowerPC processors
 *         PPC603    : PowerPC 603
 *         PPC604    : PowerPC 604
 *         PPC750    : PowerPC 750
 *
 *   capabilities
 *     This field returns the capabilities of the processor.
 *     Legal processor capabilities include:
 *       MMX                 : supports MMX
 *       SSE                 : supports SSE
 *       3DNOW               : supports 3DNow
 *       SSE2                : supports SSE2
 *       SFENCE              : supports SFENCE
 *       WRITE_COMBINING     : supports write-combining
 *       ALTIVEC             : supports ALTIVEC
 *       PUT_NEEDS_IO        : requires OUT inst w/PUT updates
 *       NEEDS_WC_WORKAROUND : requires workaround for P4 write-combining bug
 *       3DNOW_EXT           : supports 3DNow Extensions
 *       MMX_EXT             : supports MMX Extensions
 *       CMOV                : supports CMOV
 *       CLFLUSH             : supports CLFLUSH
 *       SSE3                : supports SSE3
 *       NEEDS_WAR_124888    : requires write to GPU while spinning on
 *                           : GPU value
 *       HT                  : support hyper-threading
 *   clock
 *     This field returns the processor speed in MHz.
 *   L1DataCacheSize
 *     This field returns the level 1 data (or unified) cache size
 *     in kilobytes.
 *   L2DataCacheSize
 *     This field returns the level 2 data (or unified) cache size
 *     in kilobytes.
 *   dataCacheLineSize
 *     This field returns the bytes per line in the level 1 data cache.
 *   numLogicalCpus
 *     This field returns the number of logical processors.  On Intel x86
 *     systems that support it, this value will incorporate the current state
 *     of HyperThreading.
 *   numPhysicalCpus
 *     This field returns the number of physical processors.
 *   name
 *     This field returns the CPU name in ASCII string format.
 *   family
 *     Vendor defined Family and Extended Family combined
 *   model
 *     Vendor defined Model and Extended Model combined
 *   stepping
 *     Silicon stepping
 *   bCCEnabled
 *     Confidentail compute enabled/disabled state
 *   selfHostedSocType
 *     SoC type NV0000_CTRL_SYSTEM_SH_SOC_TYPE* in case of self hosted systems
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_CPU_INFO (0x102U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS {
    NvU32                          type;                               /* processor type        */
    NvU32                          capabilities;                       /* processor caps        */
    NvU32                          clock;                              /* processor speed (MHz) */
    NvU32                          L1DataCacheSize;                    /* L1 dcache size (KB)   */
    NvU32                          L2DataCacheSize;                    /* L2 dcache size (KB)   */
    NvU32                          dataCacheLineSize;                  /* L1 dcache bytes/line  */
    NvU32                          numLogicalCpus;                     /* logial processor cnt  */
    NvU32                          numPhysicalCpus;                    /* physical processor cnt*/
    NvU8                           name[52];                           /* embedded cpu name     */
    NvU32                          family;                             /* Vendor defined Family and Extended Family combined */
    NvU32                          model;                              /* Vendor defined Model and Extended Model combined   */
    NvU8                           stepping;                           /* Silicon stepping      */
    NvU32                          coresOnDie;                         /* cpu cores per die     */
    NvBool                         bCCEnabled;                        /* CC enabled on cpu    */
    NV0000_CTRL_SYSTEM_SH_SOC_TYPE selfHostedSocType;                  /* SoC type in case of self hosted systems    */
} NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS;

// Macros for CPU family information
#define NV0000_CTRL_SYSTEM_CPU_FAMILY                         3:0
#define NV0000_CTRL_SYSTEM_CPU_EXTENDED_FAMILY               11:4

// Macros for CPU model information
#define NV0000_CTRL_SYSTEM_CPU_MODEL                          3:0
#define NV0000_CTRL_SYSTEM_CPU_EXTENDED_MODEL                 7:4

// Macros for AMD CPU information
#define NV0000_CTRL_SYSTEM_CPU_ID_AMD_FAMILY                  0xF
#define NV0000_CTRL_SYSTEM_CPU_ID_AMD_EXTENDED_FAMILY         0xA
#define NV0000_CTRL_SYSTEM_CPU_ID_AMD_MODEL                   0x0
#define NV0000_CTRL_SYSTEM_CPU_ID_AMD_EXTENDED_MODEL          0x4

// Macros for Intel CPU information
#define NV0000_CTRL_SYSTEM_CPU_ID_INTEL_FAMILY                0x6
#define NV0000_CTRL_SYSTEM_CPU_ID_INTEL_EXTENDED_FAMILY       0x0
#define NV0000_CTRL_SYSTEM_CPU_ID_INTEL_CORE_S_MODEL          0x7
#define NV0000_CTRL_SYSTEM_CPU_ID_INTEL_CORE_P_MODEL          0xA
#define NV0000_CTRL_SYSTEM_CPU_ID_INTEL_EXTENDED_MODEL        0x9

/*  processor type values */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN            (0x00000000U)
/* Intel types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P5                 (0x00000001U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P55                (0x00000002U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P6                 (0x00000003U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P2                 (0x00000004U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P2XC               (0x00000005U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CELA               (0x00000006U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P3                 (0x00000007U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P3_INTL2           (0x00000008U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P4                 (0x00000009U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2              (0x00000010U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CELN_M16H          (0x00000011U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2_EXTRM        (0x00000012U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ATOM               (0x00000013U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_XEON_SPR           (0x00000014U)
/* AMD types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K5                 (0x00000030U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K6                 (0x00000031U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K62                (0x00000032U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K63                (0x00000033U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K7                 (0x00000034U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K8                 (0x00000035U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K10                (0x00000036U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K11                (0x00000037U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_RYZEN              (0x00000038U)
/* IDT/Centaur types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_C6                 (0x00000060U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_C62                (0x00000061U)
/* Cyrix types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_GX                 (0x00000070U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_M1                 (0x00000071U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_M2                 (0x00000072U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_MGX                (0x00000073U)
/* Transmeta types  */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_TM_CRUSOE          (0x00000080U)
/* IBM types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC603             (0x00000090U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC604             (0x00000091U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC750             (0x00000092U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_POWERN             (0x00000093U)
/* Unknown ARM architecture CPU type */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_UNKNOWN        (0xA0000000U)
/* ARM Ltd types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A9             (0xA0000009U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A15            (0xA000000FU)
/* NVIDIA types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_1_0      (0xA0001000U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_2_0      (0xA0002000U)

/* Generic types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV8A_GENERIC     (0xA00FF000U)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV9A_GENERIC     (0xA00FF001U)

/* processor capabilities */
#define NV0000_CTRL_SYSTEM_CPU_CAP_MMX                 (0x00000001U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE                 (0x00000002U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW               (0x00000004U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE2                (0x00000008U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE              (0x00000010U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_WRITE_COMBINING     (0x00000020U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_ALTIVEC             (0x00000040U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_PUT_NEEDS_IO        (0x00000080U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WC_WORKAROUND (0x00000100U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW_EXT           (0x00000200U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_MMX_EXT             (0x00000400U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_CMOV                (0x00000800U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_CLFLUSH             (0x00001000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WAR_190854    (0x00002000U) /* deprecated */
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE3                (0x00004000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WAR_124888    (0x00008000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_HT_CAPABLE          (0x00010000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE41               (0x00020000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE42               (0x00040000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_AVX                 (0x00080000U)
#define NV0000_CTRL_SYSTEM_CPU_CAP_ERMS                (0x00100000U)

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_CHIPSET_INFO
 *
 * This command returns system chipset information.
 *
 *   vendorId
 *     This parameter returns the vendor identification for the chipset.
 *     A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates the chipset
 *     cannot be identified.
 *   deviceId
 *     This parameter returns the device identification for the chipset.
 *     A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates the chipset
 *     cannot be identified.
 *   subSysVendorId
 *     This parameter returns the subsystem vendor identification for the
 *     chipset.  A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates the
 *     chipset cannot be identified.
 *   subSysDeviceId
 *     This parameter returns the subsystem device identification for the 
 *     chipset. A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates the 
 *     chipset cannot be identified.
 *   HBvendorId
 *     This parameter returns the vendor identification for the chipset's
 *     host bridge. A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates
 *     the chipset's host bridge cannot be identified.
 *   HBdeviceId
 *     This parameter returns the device identification for the chipset's
 *     host bridge. A value of NV0000_SYSTEM_CHIPSET_INVALID_ID indicates
 *     the chipset's host bridge cannot be identified.
 *   HBsubSysVendorId
 *     This parameter returns the subsystem vendor identification for the
 *     chipset's host bridge. A value of NV0000_SYSTEM_CHIPSET_INVALID_ID
 *     indicates the chipset's host bridge cannot be identified.
 *   HBsubSysDeviceId
 *     This parameter returns the subsystem device identification for the
 *     chipset's host bridge. A value of NV0000_SYSTEM_CHIPSET_INVALID_ID
 *     indicates the chipset's host bridge cannot be identified.
 *   sliBondId
 *     This parameter returns the SLI bond identification for the chipset.
 *   vendorNameString
 *     This parameter returns the vendor name string.
 *   chipsetNameString
 *     This parameter returns the vendor name string.
 *   sliBondNameString
 *     This parameter returns the SLI bond name string.
 *   flag
 *     This parameter specifies NV0000_CTRL_SYSTEM_CHIPSET_FLAG_XXX flags:
 *     _HAS_RESIZABLE_BAR_ISSUE_YES: Chipset where the use of resizable BAR1
 *     should be disabled - bug 3440153
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_CHIPSET_INFO        (0x104U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS_MESSAGE_ID" */

/* maximum name string length */
#define NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH        (0x0000020U)

/* invalid id */
#define NV0000_SYSTEM_CHIPSET_INVALID_ID               (0xffffU)

#define NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS {
    NvU16 vendorId;
    NvU16 deviceId;
    NvU16 subSysVendorId;
    NvU16 subSysDeviceId;
    NvU16 HBvendorId;
    NvU16 HBdeviceId;
    NvU16 HBsubSysVendorId;
    NvU16 HBsubSysDeviceId;
    NvU32 sliBondId;
    NvU8  vendorNameString[NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH];
    NvU8  subSysVendorNameString[NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH];
    NvU8  chipsetNameString[NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH];
    NvU8  sliBondNameString[NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH];
    NvU32 flags;
} NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS;

#define NV0000_CTRL_SYSTEM_CHIPSET_FLAG_HAS_RESIZABLE_BAR_ISSUE                  0:0
#define NV0000_CTRL_SYSTEM_CHIPSET_FLAG_HAS_RESIZABLE_BAR_ISSUE_NO  (0x00000000U)
#define NV0000_CTRL_SYSTEM_CHIPSET_FLAG_HAS_RESIZABLE_BAR_ISSUE_YES (0x00000001U)



/*
 * NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT
 *
 * This command returns whether the VRR cookie is present in the SBIOS.
 *
 * bIsPresent (out)
 *     This parameter contains whether the VRR cookie is present in the SBIOS.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT                                      (0x107U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS {
    NvBool bIsPresent;
} NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_LOCK_TIMES
 *
 * This command is used to retrieve the measured times spent holding and waiting for
 * the main RM locks (API and GPU).
 *
 *   waitApiLock
 *     Total time spent by RM API's waiting to acquire the API lock
 *
 *   holdRoApiLock
 *     Total time spent by RM API's holding the API lock in RO mode.
 *
 *   holdRwApiLock
 *     Total time spent by RM API's holding the API lock in RW mode.
 *
 *   waitGpuLock
 *     Total time spent by RM API's waiting to acquire one or more GPU locks.
 *
 *   holdGpuLock
 *     Total time spent by RM API's holding one or more GPU locks.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_GET_LOCK_TIMES (0x109U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 waitApiLock, 8);
    NV_DECLARE_ALIGNED(NvU64 holdRoApiLock, 8);
    NV_DECLARE_ALIGNED(NvU64 holdRwApiLock, 8);
    NV_DECLARE_ALIGNED(NvU64 waitGpuLock, 8);
    NV_DECLARE_ALIGNED(NvU64 holdGpuLock, 8);
} NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_CLASSLIST
 *
 * This command is used to retrieve the set of system-level classes
 * supported by the platform.
 *
 *   numClasses
 *     This parameter returns the number of valid entries in the returned
 *     classes[] list.  This parameter will not exceed
 *     Nv0000_CTRL_SYSTEM_MAX_CLASSLIST_SIZE.
 *   classes
 *     This parameter returns the list of supported classes
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV0000_CTRL_CMD_SYSTEM_GET_CLASSLIST  (0x108U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS_MESSAGE_ID" */

/* maximum number of classes returned in classes[] array */
#define NV0000_CTRL_SYSTEM_MAX_CLASSLIST_SIZE (32U)

#define NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS {
    NvU32 numClasses;
    NvU32 classes[NV0000_CTRL_SYSTEM_MAX_CLASSLIST_SIZE];
} NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_NOTIFY_EVENT
 *
 * This command is used to send triggered mobile related system events
 * to the RM.
 *
 *   eventType
 *     This parameter indicates the triggered event type.  This parameter
 *     should specify a valid NV0000_CTRL_SYSTEM_EVENT_TYPE value.
 *   eventData
 *     This parameter specifies the type-dependent event data associated
 *     with EventType.  This parameter should specify a valid
 *     NV0000_CTRL_SYSTEM_EVENT_DATA value.
 *   bEventDataForced
 *     This parameter specifies what we have to do, Whether trust current
 *     Lid/Dock state or not. This parameter should specify a valid
 *     NV0000_CTRL_SYSTEM_EVENT_DATA_FORCED value.

 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 * Sync this up (#defines) with one in nvapi.spec!
 * (NV_ACPI_EVENT_TYPE & NV_ACPI_EVENT_DATA)
 */
#define NV0000_CTRL_CMD_SYSTEM_NOTIFY_EVENT (0x110U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS {
    NvU32  eventType;
    NvU32  eventData;
    NvBool bEventDataForced;
} NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS;

/* valid eventType values */
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_LID_STATE        (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_POWER_SOURCE     (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_DOCK_STATE       (0x00000002U)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_LID        (0x00000003U)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_DOCK       (0x00000004U)

/* valid eventData values */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_LID_OPEN         (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_LID_CLOSED       (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_BATTERY    (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_AC         (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_UNDOCKED         (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_DOCKED           (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_DSM    (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_DCS    (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_NVIF   (0x00000002U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_ACPI   (0x00000003U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_POLL   (0x00000004U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_COUNT  (0x5U) /* finn: Evaluated from "(NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_POLL + 1)" */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_DSM   (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_DCS   (0x00000001U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_NVIF  (0x00000002U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_ACPI  (0x00000003U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_POLL  (0x00000004U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_COUNT (0x5U) /* finn: Evaluated from "(NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_POLL + 1)" */

/* valid bEventDataForced values */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_FORCED_FALSE     (0x00000000U)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_FORCED_TRUE      (0x00000001U)

/*
 * NV000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE
 *
 * This command is used to query the platform type.
 *
 *   systemType
 *     This parameter returns the type of the system.
 *     Legal values for this parameter include:
 *       NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_DESKTOP
 *         The system is a desktop platform.
 *       NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_GENERIC
 *         The system is a mobile (non-Toshiba) platform.
 *       NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_DESKTOP
 *         The system is a mobile Toshiba platform.
 *       NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_SOC
 *         The system is a system-on-a-chip (SOC) platform.
 *

 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE       (0x111U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS {
    NvU32 systemType;
} NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS;

/* valid systemType values */
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_DESKTOP        (0x000000U)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_GENERIC (0x000001U)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_TOSHIBA (0x000002U)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_SOC            (0x000003U)




/*
 * NV0000_CTRL_CMD_SYSTEM_DEBUG_RMMSG_CTRL
 *
 * This command controls the current RmMsg filters. 
 *
 * It is only supported if RmMsg is enabled (e.g. debug builds).
 *
 *   cmd
 *     GET - Gets the current RmMsg filter string.
 *     SET - Sets the current RmMsg filter string.
 *
 *   count
 *     The length of the RmMsg filter string.
 *
 *   data
 *     The RmMsg filter string.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_DEBUG_RMMSG_CTRL     (0x121U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE         512U

#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_GET (0x00000000U)
#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_SET (0x00000001U)

#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS {
    NvU32 cmd;
    NvU32 count;
    NvU8  data[NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE];
} NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS;

/*
 * NV0000_CTRL_SYSTEM_HWBC_INFO
 *
 * This structure contains information about the HWBC (BR04) specified by 
 * hwbcId.
 *   
 *   hwbcId
 *     This field specifies the HWBC ID.
 *   firmwareVersion
 *     This field returns the version of the firmware on the HWBC (BR04), if
 *     present. This is a packed binary number of the form 0x12345678, which
 *     corresponds to a firmware version of 12.34.56.78.
 *   subordinateBus
 *     This field returns the subordinate bus number of the HWBC (BR04).
 *   secondaryBus
 *     This field returns the secondary bus number of the HWBC (BR04).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

typedef struct NV0000_CTRL_SYSTEM_HWBC_INFO {
    NvU32 hwbcId;
    NvU32 firmwareVersion;
    NvU32 subordinateBus;
    NvU32 secondaryBus;
} NV0000_CTRL_SYSTEM_HWBC_INFO;

#define NV0000_CTRL_SYSTEM_HWBC_INVALID_ID   (0xFFFFFFFFU)

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_HWBC_INFO
 *
 * This command returns information about all Hardware Broadcast (HWBC) 
 * devices present in the system that are BR04s. To get the complete
 * list of HWBCs in the system, all GPUs present in the system must be 
 * initialized. See the description of NV0000_CTRL_CMD_GPU_ATTACH_IDS to 
 * accomplish this.
 *   
 *   hwbcInfo
 *     This field is an array of NV0000_CTRL_SYSTEM_HWBC_INFO structures into
 *     which HWBC information is placed. There is one entry for each HWBC
 *     present in the system. Valid entries are contiguous, invalid entries 
 *     have the hwbcId equal to NV0000_CTRL_SYSTEM_HWBC_INVALID_ID. If no HWBC
 *     is present in the system, all the entries would be marked invalid, but
 *     the return value would still be SUCCESS.
 *     
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_HWBC_INFO (0x124U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_MAX_HWBCS         (0x00000080U)

#define NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS {
    NV0000_CTRL_SYSTEM_HWBC_INFO hwbcInfo[NV0000_CTRL_SYSTEM_MAX_HWBCS];
} NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GPS_CONTROL
 *
 * This command is used to control GPS functionality.  It allows control of
 * GPU Performance Scaling (GPS), changing its operational parameters and read
 * most GPS dynamic parameters.
 *
 *   command
 *     This parameter specifies the command to execute.  Invalid commands
 *     result in the return of an NV_ERR_INVALID_ARGUMENT status.
 *   locale
 *     This parameter indicates the specific locale to which the command
 *     'command' is to be applied.
 *     Supported range of CPU/GPU {i = 0, ..., 255}
 *   data
 *     This parameter contains a command-specific data payload.  It can
 *     be used to input data as well as output data.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_COMMAND
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_DATA
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_GPS_CONTROL (0x122U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS {
    NvU16 command;
    NvU16 locale;
    NvU32 data;
} NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS;

/* 
 *  Valid command values :
 *  
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_INIT
 *    Is used to check if GPS was correctly initialized.
 *    Possible return (OUT) values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INIT_NO
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INIT_YES
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_EXEC
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_EXEC
 *    Are used to stop/start GPS functionality and to get current status.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_EXEC_STOP
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_EXEC_START
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_ACTIONS
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_ACTIONS
 *    Are used to control execution of GPS actions and to get current status.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_ACTIONS_OFF
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_ACTIONS_ON
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_LOGIC
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_LOGIC
 *    Are used to switch current GPS logic and to retrieve current logic.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_OFF
 *        Will cause that all GPS actions will be NULL.
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_FUZZY
 *        Fuzzy logic will determine GPS actions based on current ruleset.
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_DETERMINISTIC
 *        Deterministic logic will define GPS actions based on current ruleset.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_PREFERENCE
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_PREFERENCE
 *    Are used to set/retrieve system control preference.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_CPU
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_GPU
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_BOTH
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_GPU2CPU_LIMIT
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_GPU2CPU_LIMIT
 *    Are used to set/retrieve GPU2CPU pstate limits.
 *    IN/OUT values are four bytes packed into a 32-bit data field.
 *    The CPU cap index for GPU pstate 0 is in the lowest byte, the CPU cap
 *    index for the GPU pstate 3 is in the highest byte, etc.  One
 *    special value is to disable the override to the GPU2CPU map:
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_PMU_GPS_STATE
 *    Is used to stop/start GPS PMU functionality.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_PMU_GPS_STATE
 *    Is used to get the current status of PMU GPS.
 *      NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_NO_MAP_OVERRIDE
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_MAX_POWER
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_MAX_POWER
 *    Are used to set/retrieve max power [mW] that system can provide.
 *    This is hardcoded GPS safety feature and logic/rules does not apply
 *    to this threshold.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_COOLING_BUDGET
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_COOLING_BUDGET
 *    Are used to set/retrieve current system cooling budget [mW].
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_INTEGRAL_PERIOD
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_INTEGRAL_PERIOD
 *    Are used to set/retrieve integration interval [sec].
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_RULESET
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_RULESET
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_RULE_COUNT
 *    Are used to set/retrieve used ruleset [#].  Value is checked
 *    against MAX number of rules for currently used GPS logic. Also COUNT
 *    provides a way to find out how many rules exist for the current control 
 *    system.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_APP_BOOST
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_APP_BOOST
 *    Is used to set/get a delay relative to now during which to allow unbound 
 *    CPU performance.  Units are seconds.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_PWR_SUPPLY_MODE
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_PWR_SUPPLY_MODE
 *    Is used to override/get the actual power supply mode (AC/Battery).
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_REAL
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_FAKE_AC
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_FAKE_BATT
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_VCT_SUPPORT_INFO
 *    Is used to get the Ventura system information for VCT tool
 *    Returned 32bit value should be treated as bitmask and decoded in
 *    following way:
 *    Encoding details are defined in objgps.h refer to
 *    NV_GPS_SYS_SUPPORT_INFO and corresponding bit defines.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_SUPPORTED_FUNCTION
 *    Is used to get the supported sub-functions defined in SBIOS.  Returned
 *    value is a bitmask where each bit corresponds to different function:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SUPPORT
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_VENTURASTATUS
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_GETPSS
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SETPPC
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_GETPPC
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_VENTURACB
 *      NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SYSPARAMS
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_DELTA
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_FUTURE
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_LTMAVG
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_INTEGRAL
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_BURDEN
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_INTERMEDIATE
 *    Are used to retrieve appropriate power measurements and their derivatives
 *    in [mW] for required locale.  _BURDEN is defined only for _LOCALE_SYSTEM.
 *    _INTERMEDIATE is not defined for _LOCALE_SYSTEM, and takes an In value as
 *    index.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_SENSOR_PARAMETERS
 *    Is used to retrieve parameters when adjusting raw sensor power reading.
 *    The values may come from SBIOS, VBIOS, registry or driver default.
 *    Possible IN value is the index of interested parameter.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP_DELTA
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP_FUTURE
 *    Are used to retrieve appropriate temperature measurements and their
 *    derivatives in [1/1000 Celsius].
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_CAP
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_MIN
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_MAX
 *    Are used to retrieve CPU(x)/GPU(x) p-state or it's limits.
 *    Not applicable to _LOCALE_SYSTEM.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_ACTION
 *    Is used to retrieve last GPS action for given domain.
 *    Not applicable to _LOCALE_SYSTEM.
 *    Possible return (OUT) values are:
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DEC_TO_P0
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DEC_BY_1
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DO_NOTHING
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_SET_CURRENT
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_BY_1
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_BY_2
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_TO_LFM
 *      NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_TO_SLFM
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_SET_POWER_SIM_STATE
 *    Is used to set the power sensor simulator state.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_SIM_STATE
 *    Is used to get the power simulator sensor simulator state.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_SET_POWER_SIM_DATA
 *    Is used to set power sensor simulator data
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_SIM_DATA
 *    Is used to get power sensor simulator data
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_INIT_USING_SBIOS_AND_ACK
 *    Is used to respond to the ACPI event triggered by SBIOS.  RM will
 *    request value for budget and status, validate them, apply them
 *    and send ACK back to SBIOS.
 *  NV0000_CTRL_CMD_SYSTEM_GPS_DATA_PING_SBIOS_FOR_EVENT
 *    Is a test cmd that should notify SBIOS to send ACPI event requesting
 *    budget and status change.
 */
#define NV0000_CTRL_CMD_SYSTEM_GPS_INVALID                       (0xFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_INIT                  (0x0000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_EXEC                  (0x0001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_EXEC                  (0x0002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_ACTIONS               (0x0003U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_ACTIONS               (0x0004U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_LOGIC                 (0x0005U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_LOGIC                 (0x0006U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_PREFERENCE            (0x0007U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_PREFERENCE            (0x0008U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_GPU2CPU_LIMIT         (0x0009U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_GPU2CPU_LIMIT         (0x000AU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_SET_PMU_GPS_STATE         (0x000BU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_GET_PMU_GPS_STATE         (0x000CU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_MAX_POWER             (0x0100U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_MAX_POWER             (0x0101U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_COOLING_BUDGET        (0x0102U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_COOLING_BUDGET        (0x0103U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_INTEGRAL_PERIOD       (0x0104U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_INTEGRAL_PERIOD       (0x0105U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_RULESET               (0x0106U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_RULESET               (0x0107U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_RULE_COUNT            (0x0108U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_APP_BOOST             (0x0109U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_APP_BOOST             (0x010AU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_SET_PWR_SUPPLY_MODE       (0x010BU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_PWR_SUPPLY_MODE       (0x010CU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_VCT_SUPPORT_INFO      (0x010DU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_GET_SUPPORTED_FUNCTIONS   (0x010EU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER                (0x0200U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_DELTA          (0x0201U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_FUTURE         (0x0202U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_LTMAVG         (0x0203U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_INTEGRAL       (0x0204U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_BURDEN         (0x0205U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_INTERMEDIATE   (0x0206U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_SENSOR_PARAMETERS    (0x0210U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP                 (0x0220U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP_DELTA           (0x0221U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_TEMP_FUTURE          (0x0222U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE               (0x0240U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_CAP           (0x0241U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_MIN           (0x0242U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_MAX           (0x0243U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_ACTION        (0x0244U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_PSTATE_SLFM_PRESENT  (0x0245U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_SET_POWER_SIM_STATE      (0x0250U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_SIM_STATE      (0x0251U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_SET_POWER_SIM_DATA       (0x0252U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_GET_POWER_SIM_DATA       (0x0253U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_INIT_USING_SBIOS_AND_ACK (0x0320U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_PING_SBIOS_FOR_EVENT     (0x0321U)

/* valid LOCALE values */
#define NV0000_CTRL_CMD_SYSTEM_GPS_LOCALE_INVALID                (0xFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_LOCALE_SYSTEM                 (0x0000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_LOCALE_CPU(i)           (0x0100+((i)%0x100))
#define NV0000_CTRL_CMD_SYSTEM_GPS_LOCALE_GPU(i)           (0x0200+((i)%0x100))

/* valid data values for enums */
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INVALID               (0x80000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INIT_NO               (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INIT_YES              (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_EXEC_STOP             (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_EXEC_START            (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_ACTIONS_OFF           (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_ACTIONS_ON            (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_OFF             (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_FUZZY           (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_LOGIC_DETERMINISTIC   (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_CPU        (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_GPU        (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PREFERENCE_BOTH       (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_NO_MAP_OVERRIDE       (0xFFFFFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PMU_GPS_STATE_OFF     (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_PMU_GPS_STATE_ON      (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_REAL       (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_FAKE_AC    (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_PWR_SUPPLY_FAKE_BATT  (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SUPPORT          (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_VENTURASTATUS    (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_GETPSS           (0x00000004U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SETPPC           (0x00000008U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_GETPPC           (0x00000010U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_VENTURACB        (0x00000020U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_SYS_DEF_FUNC_SYSPARAMS        (0x00000040U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DEC_TO_P0     (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DEC_BY_1      (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_DO_NOTHING    (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_SET_CURRENT   (0x00000003U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_BY_1      (0x00000004U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_BY_2      (0x00000005U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_TO_LFM    (0x00000006U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_ACTION_INC_TO_SLFM   (0x00000007U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_SLFM_PRESENT_NO      (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_SLFM_PRESENT_YES     (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_POWER_SIM_STATE_OFF  (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_GPS_DATA_DEF_POWER_SIM_STATE_ON   (0x00000001U)

/*
 *  NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_CONTROL
 *
 *  This command allows execution of multiple GpsControl commands within one
 *  RmControl call.  For practical reasons # of commands is limited to 16.
 *  This command shares defines with NV0000_CTRL_CMD_SYSTEM_GPS_CONTROL.
 *
 *    cmdCount
 *      Number of commands that should be executed.
 *      Less or equal to NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_COMMAND_MAX.
 * 
 *    succeeded
 *      Number of commands that were succesully executed.
 *      Less or equal to NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_COMMAND_MAX.
 *      Failing commands return NV0000_CTRL_CMD_SYSTEM_GPS_CMD_DEF_INVALID
 *      in their data field.
 *
 *    cmdData
 *      Array of commands with following structure:
 *        command
 *          This parameter specifies the command to execute.
 *          Invalid commands result in the return of an
 *          NV_ERR_INVALID_ARGUMENT status.
 *        locale
 *          This parameter indicates the specific locale to which
 *          the command 'command' is to be applied.
 *          Supported range of CPU/GPU {i = 0, ..., 255}
 *        data
 *          This parameter contains a command-specific data payload.
 *          It is used both to input data as well as to output data.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_REQUEST
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_CONTROL                 (0x123U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_COMMAND_MAX             (16U)
#define NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS {
    NvU32 cmdCount;
    NvU32 succeeded;

    struct {
        NvU16 command;
        NvU16 locale;
        NvU32 data;
    } cmdData[NV0000_CTRL_CMD_SYSTEM_GPS_BATCH_COMMAND_MAX];
} NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS;


/*
 * Deprecated. Please use NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 instead.
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS          (0x127U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED must remain equal to the square of
 * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS due to Check RM parsing issues.
 * NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS is the maximum size of GPU groups
 * allowed for batched P2P caps queries provided by the RM control
 * NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX.
 */
#define NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS         32U
#define NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED 1024U
#define NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS        8U
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER 0xffffffffU

/* P2P capabilities status index values */
#define NV0000_CTRL_P2P_CAPS_INDEX_READ              0U
#define NV0000_CTRL_P2P_CAPS_INDEX_WRITE             1U
#define NV0000_CTRL_P2P_CAPS_INDEX_NVLINK            2U
#define NV0000_CTRL_P2P_CAPS_INDEX_ATOMICS           3U
#define NV0000_CTRL_P2P_CAPS_INDEX_PROP              4U
#define NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK          5U
#define NV0000_CTRL_P2P_CAPS_INDEX_PCI               6U
#define NV0000_CTRL_P2P_CAPS_INDEX_C2C               7U
#define NV0000_CTRL_P2P_CAPS_INDEX_PCI_BAR1          8U

#define NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE        9U


#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_MESSAGE_ID (0x27U)

typedef struct NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU32 gpuCount;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NV_DECLARE_ALIGNED(NvP64 busPeerIds, 8);
    NV_DECLARE_ALIGNED(NvP64 busEgmPeerIds, 8);
} NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS;

/* valid p2pCaps values */
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED                    0:0
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED_FALSE           (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED_TRUE            (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED                     1:1
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED_FALSE            (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED_TRUE             (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED                      2:2
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED_FALSE             (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED_TRUE              (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED                    3:3
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED_FALSE           (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED_TRUE            (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED                   4:4
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED_FALSE          (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED_TRUE           (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED                  5:5
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED_FALSE         (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED_TRUE          (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED                       6:6
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED_FALSE              (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED_TRUE               (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED           7:7
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED_FALSE  (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED_TRUE   (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED            8:8
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED_FALSE   (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED_TRUE    (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED          9:9
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED_FALSE (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED_TRUE  (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED           10:10
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED_FALSE  (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED_TRUE   (0x00000001U)


#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED                       12:12
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED_FALSE              (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED_TRUE               (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED                 13:13
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED_FALSE         (0x00000000U)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED_TRUE          (0x00000001U)

/* P2P status codes */
#define NV0000_P2P_CAPS_STATUS_OK                                        (0x00U)
#define NV0000_P2P_CAPS_STATUS_CHIPSET_NOT_SUPPORTED                     (0x01U)
#define NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED                         (0x02U)
#define NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED                (0x03U)
#define NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY                        (0x04U)
#define NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED                             (0x05U)

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2
 *
 * This command returns peer to peer capabilities present between GPUs.  
 * Valid requests must present a list of GPU Ids.
 *
 *   [in] gpuIds
 *     This member contains the array of GPU IDs for which we query the P2P
 *     capabilities. Valid entries are contiguous, beginning with the first 
 *     entry in the list.
 *   [in] gpuCount
 *     This member contains the number of GPU IDs stored in the gpuIds[] array.
 *   [out] p2pCaps
 *     This member returns the peer to peer capabilities discovered between the
 *     GPUs. Valid p2pCaps values include:
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED
 *         When this bit is set, peer to peer writes between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED
 *         When this bit is set, peer to peer reads between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED
 *         When this bit is set, peer to peer PROP between subdevices owned
 *         by this device are supported. This is enabled by default
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED
 *         When this bit is set, PCI is supported for all P2P between subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED
 *         When this bit is set, NVLINK is supported for all P2P between subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED
 *         When this bit is set, peer to peer atomics between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED
 *         When this bit is set, peer to peer loopback is supported for subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED
 *         When this bit is set, indirect peer to peer writes between subdevices
 *         owned by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED
 *         When this bit is set, indirect peer to peer reads between subdevices
 *         owned by this device are supported.
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED
 *         When this bit is set, indirect peer to peer atomics between
 *         subdevices owned by this device are supported.
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED
 *         When this bit is set, indirect NVLINK is supported for subdevices
 *         owned by this device.
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED
 *         When this bit is set, C2C P2P is supported between the GPUs
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_BAR1_SUPPORTED
 *         When this bit is set, BAR1 P2P is supported between the GPUs
 *         mentioned in @ref gpuIds
 *   [out] p2pOptimalReadCEs
 *      For a pair of GPUs, return mask of CEs to use for p2p reads over Nvlink
 *   [out] p2pOptimalWriteCEs
 *      For a pair of GPUs, return mask of CEs to use for p2p writes over Nvlink
 *   [out] p2pCapsStatus
 *     This member returns status of all supported p2p capabilities. Valid
 *     status values include:
 *       NV0000_P2P_CAPS_STATUS_OK
 *         P2P capability is supported.
 *       NV0000_P2P_CAPS_STATUS_CHIPSET_NOT_SUPPORTED
 *         Chipset doesn't support p2p capability.
 *       NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED
 *         GPU doesn't support p2p capability.
 *       NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED
 *         IOH topology isn't supported. For e.g. root ports are on different
 *         IOH.
 *       NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY
 *         P2P Capability is disabled by a regkey.
 *       NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED
 *         P2P Capability is not supported.
 *       NV0000_P2P_CAPS_STATUS_NVLINK_SETUP_FAILED
 *         Indicates that NvLink P2P link setup failed.
 *    [out] busPeerIds
 *        Peer ID matrix. It is a one-dimentional array.
 *        busPeerIds[X * gpuCount + Y] maps from index X to index Y in
 *        the gpuIds[] table. For invalid or non-existent peer busPeerIds[]
 *        has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 *    [out] busEgmPeerIds
 *        EGM Peer ID matrix. It is a one-dimentional array.
 *        busEgmPeerIds[X * gpuCount + Y] maps from index X to index Y in
 *        the gpuIds[] table. For invalid or non-existent peer busEgmPeerIds[]
 *        has the value NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */



#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2                           (0x12bU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU32 gpuCount;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NvU32 busPeerIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED];
    NvU32 busEgmPeerIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED];
} NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX
 *
 * This command returns peer to peer capabilities present between all pairs of
 * GPU IDs {(a, b) : a in gpuIdGrpA and b in gpuIdGrpB}. This can be used to
 * collect all P2P capabilities in the system - see the SRT:
 *     NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX_TEST
 * for a demonstration.
 *
 * The call will query for all pairs between set A and set B, and returns
 * results in both link directions. The results are two-dimensional arrays where
 * the first dimension is the index within the set-A array of one GPU ID under
 * consideration, and the second dimension is the index within the set-B array
 * of the other GPU ID under consideration.
 *
 * That is, the result arrays are *ALWAYS* to be indexed first with the set-A
 * index, then with the set-B index. The B-to-A direction of results are put in
 * the b2aOptimal(Read|Write)CEs. This makes it unnecessary to call the query
 * twice, since the usual use case requires both directions.
 *
 * If a set is being compared against itself (by setting grpBCount to 0), then
 * the result matrices are symmetric - it doesn't matter which index is first.
 * However, the choice of indices is effectively a choice of which ID is "B" and
 * which is "A" for the "a2b" and "b2a" directional results.
 *
 *   [in] grpACount
 *     This member contains the number of GPU IDs stored in the gpuIdGrpA[]
 *     array. Must be >= 0.
 *   [in] grpBCount
 *     This member contains the number of GPU IDs stored in the gpuIdGrpB[]
 *     array. Can be == 0 to specify a check of group A against itself.
 *   [in] gpuIdGrpA
 *     This member contains the array of GPU IDs in "group A", each of which
 *     will have its P2P capabilities returned with respect to each GPU ID in
 *     "group B". Valid entries are contiguous, beginning with the first entry
 *     in the list.
 *   [in] gpuIdGrpB
 *     This member contains the array of GPU IDs in "group B", each of which
 *     will have its P2P capabilities returned with respect to each GPU ID in
 *     "group A". Valid entries are contiguous, beginning with the first entry
 *     in the list. May be equal to gpuIdGrpA, but best performance requires
 *     that the caller specifies grpBCount = 0 in this case, and ignores this.
 *   [out] p2pCaps
 *     This member returns the peer to peer capabilities discovered between the
 *     pairs of input GPUs between the groups, indexed by [A_index][B_index].
 *     Valid p2pCaps values include:
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED
 *         When this bit is set, peer to peer writes between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED
 *         When this bit is set, peer to peer reads between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED
 *         When this bit is set, peer to peer PROP between subdevices owned
 *         by this device are supported. This is enabled by default
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED
 *         When this bit is set, PCI is supported for all P2P between subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED
 *         When this bit is set, NVLINK is supported for all P2P between subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED
 *         When this bit is set, peer to peer atomics between subdevices owned
 *         by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED
 *         When this bit is set, peer to peer loopback is supported for subdevices
 *         owned by this device.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED
 *         When this bit is set, indirect peer to peer writes between subdevices
 *         owned by this device are supported.
 *       NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED
 *         When this bit is set, indirect peer to peer reads between subdevices
 *         owned by this device are supported.
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED
 *         When this bit is set, indirect peer to peer atomics between
 *         subdevices owned by this device are supported.
 *      NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED
 *         When this bit is set, indirect NVLINK is supported for subdevices
 *         owned by this device.
 *   [out] a2bOptimalReadCes
 *      For a pair of GPUs, return mask of CEs to use for p2p reads over Nvlink
 *      in the A-to-B direction.
 *   [out] a2bOptimalWriteCes
 *      For a pair of GPUs, return mask of CEs to use for p2p writes over Nvlink
 *      in the A-to-B direction.
 *   [out] b2aOptimalReadCes
 *      For a pair of GPUs, return mask of CEs to use for p2p reads over Nvlink
 *      in the B-to-A direction.
 *   [out] b2aOptimalWriteCes
 *      For a pair of GPUs, return mask of CEs to use for p2p writes over Nvlink
 *      in the B-to-A direction.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */



#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX (0x13aU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_MESSAGE_ID" */

typedef NvU32 NV0000_CTRL_P2P_CAPS_MATRIX_ROW[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_MESSAGE_ID (0x3AU)

typedef struct NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS {
    NvU32                           grpACount;
    NvU32                           grpBCount;
    NvU32                           gpuIdGrpA[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NvU32                           gpuIdGrpB[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NV0000_CTRL_P2P_CAPS_MATRIX_ROW p2pCaps[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NV0000_CTRL_P2P_CAPS_MATRIX_ROW a2bOptimalReadCes[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NV0000_CTRL_P2P_CAPS_MATRIX_ROW a2bOptimalWriteCes[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NV0000_CTRL_P2P_CAPS_MATRIX_ROW b2aOptimalReadCes[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
    NV0000_CTRL_P2P_CAPS_MATRIX_ROW b2aOptimalWriteCes[NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS];
} NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GPS_CTRL
 *
 * This command is used to execute general GPS Functions, most dealing with
 * calling SBIOS, or retrieving cached sensor and GPS state data.
 *
 *   version
 *     This parameter specifies the version of the interface.  Legal values
 *     for this parameter are 1.
 *   cmd
 *     This parameter specifies the GPS API to be invoked. 
 *     Valid values for this parameter are:
 *       NV0000_CTRL_GPS_CMD_GET_THERM_LIMIT
 *         This command gets the temperature limit for thermal controller. When
 *         this command is specified the input parameter contains ???.
 *      NV0000_CTRL_GPS_CMD_SET_THERM_LIMIT
 *         This command set the temperature limit for thermal controller.  When
 *         this command is specified the input parameter contains ???.
 *   input
 *     This parameter specifies the cmd-specific input value.
 *   result
 *     This parameter returns the cmd-specific output value.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_SYSTEM_GPS_CTRL (0x12aU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS_MESSAGE_ID (0x2AU)

typedef struct NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS {
    NvU32 cmd;
    NvS32 input[2];
    NvS32 result[4];
} NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS;

/* valid version values */
#define NV0000_CTRL_GPS_PSHARE_PARAMS_PSP_CURRENT_VERSION      (0x00010000U)

/* valid cmd values */
#define NV0000_CTRL_GPS_CMD_TYPE_GET_THERM_LIMIT               (0x00000002U)
#define NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                     (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_THERMAL_LIMIT                   (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_MIN_LIMIT                       (0x00000001U)
#define NV0000_CTRL_GPS_RESULT_MAX_LIMIT                       (0x00000002U)
#define NV0000_CTRL_GPS_RESULT_LIMIT_SOURCE                    (0x00000003U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_THERM_LIMIT               (0x00000003U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_THERMAL_LIMIT                    (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_DOWN_N_DELTA    (0x00000004U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_DOWN_N_DELTA          (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_DOWN_N_DELTA    (0x00000005U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_DOWN_N_DELTA           (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_HOLD_DELTA      (0x00000006U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_HOLD_DELTA            (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_HOLD_DELTA      (0x00000007U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_HOLD_DELTA             (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_UP_DELTA        (0x00000008U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_UP_DELTA              (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_UP_DELTA        (0x00000009U)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_UP_DELTA               (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_ENGAGE_DELTA    (0x0000000AU)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_ENGAGE_DELTA          (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_ENGAGE_DELTA    (0x0000000BU)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_ENGAGE_DELTA           (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_DISENGAGE_DELTA (0x0000000CU)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_DISENGAGE_DELTA       (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_DISENGAGE_DELTA (0x0000000DU)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_DISENGAGE_DELTA        (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_CTRL_STATUS          (0x00000016U)
#define NV0000_CTRL_GPS_RESULT_TEMP_CTRL_STATUS                (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_CTRL_STATUS          (0x00000017U)
#define NV0000_CTRL_GPS_INPUT_TEMP_CTRL_STATUS                 (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_GET_UTIL_AVG_NUM      (0x00000018U)
#define NV0000_CTRL_GPS_RESULT_CPU_SET_UTIL_AVG_NUM            (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_CPU_SET_UTIL_AVG_NUM      (0x00000019U)
#define NV0000_CTRL_GPS_INPUT_CPU_GET_UTIL_AVG_NUM             (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_PERF_SENSOR               (0x0000001AU)
//      NV0000_CTRL_GPS_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_GPS_INPUT_NEXT_EXPECTED_POLL               (0x00000001U)
#define NV0000_CTRL_GPS_RESULT_PERF_SENSOR_VALUE               (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_PERF_SENSOR_AVAILABLE           (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_CALL_ACPI                     (0x0000001BU)
#define NV0000_CTRL_GPS_INPUT_ACPI_CMD                         (0x00000000U)
#define NV0000_CTRL_GPS_INPUT_ACPI_PARAM_IN                    (0x00000001U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_RESULT_1                   (0x00000000U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_RESULT_2                   (0x00000001U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_PSHAREPARAM_STATUS         (0x00000000U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_PSHAREPARAM_VERSION        (0x00000001U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_PSHAREPARAM_SZ             (0x00000002U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_PSS_SZ                     (0x00000000U)
#define NV0000_CTRL_GPS_OUTPUT_ACPI_PSS_COUNT                  (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_IGPU_TURBO                (0x0000001CU)
#define NV0000_CTRL_GPS_INPUT_SET_IGPU_TURBO                   (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_PERIOD               (0x00000026U)
#define NV0000_CTRL_GPS_INPUT_TEMP_PERIOD                      (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_PERIOD               (0x00000027U)
#define NV0000_CTRL_GPS_RESULT_TEMP_PERIOD                     (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_NUDGE_FACTOR         (0x00000028U)
#define NV0000_CTRL_GPS_INPUT_TEMP_NUDGE_UP                    (0x00000000U)
#define NV0000_CTRL_GPS_INPUT_TEMP_NUDGE_DOWN                  (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_NUDGE_FACTOR         (0x00000029U)
#define NV0000_CTRL_GPS_RESULT_TEMP_NUDGE_UP                   (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_TEMP_NUDGE_DOWN                 (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_THRESHOLD_SAMPLES    (0x0000002AU)
#define NV0000_CTRL_GPS_INPUT_TEMP_THRESHOLD_SAMPLE_HOLD       (0x00000000U)
#define NV0000_CTRL_GPS_INPUT_TEMP_THRESHOLD_SAMPLE_STEP       (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_THRESHOLD_SAMPLES    (0x0000002BU)
#define NV0000_CTRL_GPS_RESULT_TEMP_THRESHOLD_SAMPLE_HOLD      (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_TEMP_THRESHOLD_SAMPLE_STEP      (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_TEMP_PERF_LIMITS          (0x0000002CU)
#define NV0000_CTRL_GPS_INPUT_TEMP_PERF_LIMIT_UPPER            (0x00000000U)
#define NV0000_CTRL_GPS_INPUT_TEMP_PERF_LIMIT_LOWER            (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_TEMP_PERF_LIMITS          (0x0000002DU)
#define NV0000_CTRL_GPS_RESULT_TEMP_PERF_LIMIT_UPPER           (0x00000000U)
#define NV0000_CTRL_GPS_RESULT_TEMP_PERF_LIMIT_LOWER           (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_PM1_AVAILABLE             (0x0000002EU)
#define NV0000_CTRL_GPS_INPUT_PM1_AVAILABLE                    (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_PM1_AVAILABLE             (0x0000002FU)
#define NV0000_CTRL_GPS_OUTPUT_PM1_AVAILABLE                   (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_PACKAGE_LIMITS        (0x00000044U)
#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL1    (0x00000000U)
#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL2    (0x00000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_CPU_PACKAGE_LIMITS        (0x00000045U)
#define NV0000_CTRL_GPS_CMD_TYPE_SET_CPU_PACKAGE_LIMITS_PL1    (0x00000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_FREQ_LIMIT            (0x00000046U)
#define NV0000_CTRL_GPS_CMD_TYPE_GET_CPU_FREQ_LIMIT_MHZ        (0000000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_CPU_FREQ_LIMIT            (0x00000047U)
#define NV0000_CTRL_GPS_CMD_TYPE_SET_CPU_FREQ_LIMIT_MHZ        (0000000000U)

#define NV0000_CTRL_GPS_CMD_TYPE_GET_PPM                       (0x00000048U)
#define NV0000_CTRL_GPS_CMD_TYPE_GET_PPM_INDEX                 (0000000000U)
#define NV0000_CTRL_GPS_CMD_TYPE_GET_PPM_AVAILABLE_MASK        (0000000001U)

#define NV0000_CTRL_GPS_CMD_TYPE_SET_PPM                       (0x00000049U)
#define NV0000_CTRL_GPS_CMD_TYPE_SET_PPM_INDEX                 (0000000000U)
#define NV0000_CTRL_GPS_CMD_TYPE_SET_PPM_INDEX_MAX             (2U)

#define NV0000_CTRL_GPS_PPM_INDEX                                   7:0
#define NV0000_CTRL_GPS_PPM_INDEX_MAXPERF                      (0U)
#define NV0000_CTRL_GPS_PPM_INDEX_BALANCED                     (1U)
#define NV0000_CTRL_GPS_PPM_INDEX_QUIET                        (2U)
#define NV0000_CTRL_GPS_PPM_INDEX_INVALID                      (0xFFU)
#define NV0000_CTRL_GPS_PPM_MASK                                    15:8
#define NV0000_CTRL_GPS_PPM_MASK_INVALID                       (0U)

/* valid PS_STATUS result values */
#define NV0000_CTRL_GPS_CMD_PS_STATUS_OFF                      (0U)
#define NV0000_CTRL_GPS_CMD_PS_STATUS_ON                       (1U)


/*
 * NV0000_CTRL_CMD_SYSTEM_SET_SECURITY_SETTINGS
 *
 * This command allows privileged users to update the values of
 * security settings governing RM behavior.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT,
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *
 * Please note: as implied above, administrator privileges are
 * required to modify security settings.
 */
#define NV0000_CTRL_CMD_SYSTEM_SET_SECURITY_SETTINGS           (0x129U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS_MESSAGE_ID" */

#define GPS_MAX_COUNTERS_PER_BLOCK                             32U
#define NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS_MESSAGE_ID (0x29U)

typedef struct NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS {
    NvU32 objHndl;
    NvU32 blockId;
    NvU32 nextExpectedSampleTimems;
    NvU32 countersReq;
    NvU32 countersReturned;
    NvU32 counterBlock[GPS_MAX_COUNTERS_PER_BLOCK];
} NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_GPS_GET_PERF_SENSORS (0x12cU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSORS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSORS_PARAMS_MESSAGE_ID (0x2CU)

typedef NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSORS_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_GPS_GET_EXTENDED_PERF_SENSORS (0x12eU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_GET_EXTENDED_PERF_SENSORS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_GET_EXTENDED_PERF_SENSORS_PARAMS_MESSAGE_ID (0x2EU)

typedef NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS NV0000_CTRL_SYSTEM_GPS_GET_EXTENDED_PERF_SENSORS_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GPS_CALL_ACPI
 *
 * This command allows users to call GPS ACPI commands for testing purposes.
 *
 *   cmd
 *      This parameter specifies the GPS ACPI command to execute.
 * 
 *   input
 *      This parameter specified the cmd-dependent input value. 
 *
 *   resultSz
 *      This parameter returns the size (in bytes) of the valid data
 *      returned in  the result parameter.
 *
 *   result
 *      This parameter returns the results of the specified cmd.
 *      The maximum size (in bytes) of this returned data will
 *      not exceed GPS_MAX_ACPI_OUTPUT_BUFFER_SIZE
 *
 *   GPS_MAX_ACPI_OUTPUT_BUFFER_SIZE
 *      The size of buffer (result) in unit of NvU32.
 *      The smallest value is sizeof(PSS_ENTRY)*ACPI_PSS_ENTRY_MAX. 
 *      Since the prior one is 24 bytes, and the later one is 48, 
 *      this value cannot be smaller than 288.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT,
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *
 */
#define GPS_MAX_ACPI_OUTPUT_BUFFER_SIZE 288U
#define NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS_MESSAGE_ID (0x2DU)

typedef struct NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS {
    NvU32 cmd;
    NvU32 input;
    NvU32 resultSz;
    NvU32 result[GPS_MAX_ACPI_OUTPUT_BUFFER_SIZE];
} NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_GPS_CALL_ACPI       (0x12dU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_SYSTEM_PARAM_*
 *
 * The following is a list of system-level parameters (often sensors) that the
 * driver can be made aware of. They are primarily intended to be used by system
 * power-balancing algorithms that require system-wide visibility in order to
 * function. The names and values used here are established and specified in
 * several different NVIDIA documents that are made externally available. Thus,
 * updates to this list must be made with great caution. The only permissible
 * change is to append new parameters. Reordering is strictly prohibited.
 *
 * Brief Parameter Summary:
 *     TGPU - GPU temperature                   (NvTemp)
 *     PDTS - CPU package temperature           (NvTemp)
 *     SFAN - System fan speed                  (% of maximum fan speed)
 *     SKNT - Skin temperature                  (NvTemp)
 *     CPUE - CPU energy counter                (NvU32)
 *     TMP1 - Additional temperature sensor 1   (NvTemp)
 *     TMP2 - Additional temperature sensor 2   (NvTemp)
 *     CTGP - Mode 2 power limit offset         (NvU32)
 *     PPMD - Power mode data                   (NvU32)
 */
#define NV0000_CTRL_SYSTEM_PARAM_TGPU              (0x00000000U)
#define NV0000_CTRL_SYSTEM_PARAM_PDTS              (0x00000001U)
#define NV0000_CTRL_SYSTEM_PARAM_SFAN              (0x00000002U)
#define NV0000_CTRL_SYSTEM_PARAM_SKNT              (0x00000003U)
#define NV0000_CTRL_SYSTEM_PARAM_CPUE              (0x00000004U)
#define NV0000_CTRL_SYSTEM_PARAM_TMP1              (0x00000005U)
#define NV0000_CTRL_SYSTEM_PARAM_TMP2              (0x00000006U)
#define NV0000_CTRL_SYSTEM_PARAM_CTGP              (0x00000007U)
#define NV0000_CTRL_SYSTEM_PARAM_PPMD              (0x00000008U)
#define NV0000_CTRL_SYSTEM_PARAM_COUNT             (0x00000009U)

/*
 * NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD
 *
 * This command is used to execute general ACPI methods.
 *
 *  method
 *    This parameter identifies the MXM ACPI API to be invoked. 
 *    Valid values for this parameter are:
 *      NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSCAPS
 *        This value specifies that the DSM NVOP subfunction OPTIMUSCAPS
 *        API is to be invoked.
 *      NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSFLAG
 *        This value specifies that the DSM NVOP subfunction OPTIMUSFLAG
 *        API is to be invoked. This API will set a Flag in sbios to Indicate 
 *        that HD Audio Controller is disable/Enabled from GPU Config space.
 *        This flag will be used by sbios to restore Audio state after resuming
 *        from s3/s4.
 *      NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_CAPS
 *        This value specifies that the DSM JT subfunction FUNC_CAPS is to
 *        to be invoked to get the SBIOS capabilities
 *      NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_PLATPOLICY
 *        This value specifies that the DSM JT subfunction FUNC_PLATPOLICY is
 *        to be invoked to set and get the various platform policies for JT.
 *        Refer to the JT spec in more detail on various policies.
 *  inData
 *    This parameter specifies the method-specific input buffer.  Data is
 *    passed to the specified API using this buffer.
 *  inDataSize
 *    This parameter specifies the size of the inData buffer in bytes.
 *  outStatus
 *    This parameter returns the status code from the associated ACPI call.
 *  outData
 *    This parameter specifies the method-specific output buffer.  Data
 *    is returned by the specified API using this buffer.
 *  outDataSize
 *    This parameter specifies the size of the outData buffer in bytes. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD (0x130U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS {
    NvU32 method;
    NV_DECLARE_ALIGNED(NvP64 inData, 8);
    NvU16 inDataSize;
    NvU32 outStatus;
    NV_DECLARE_ALIGNED(NvP64 outData, 8);
    NvU16 outDataSize;
} NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS;

/* valid method parameter values */
#define NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSCAPS (0x00000000U)
#define NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NVOP_OPTIMUSFLAG (0x00000001U)
#define NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_CAPS          (0x00000002U)
#define NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_JT_PLATPOLICY    (0x00000003U)
/*
 * NV0000_CTRL_CMD_SYSTEM_ENABLE_ETW_EVENTS
 *
 * This command can be used to instruct the RM to enable/disable specific module
 * of ETW events.
 *
 *   moduleMask
 *     This parameter specifies the module of events we would like to 
 *     enable/disable.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_SYSTEM_ENABLE_ETW_EVENTS                    (0x131U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_ENABLE_ETW_EVENTS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_ENABLE_ETW_EVENTS_PARAMS_MESSAGE_ID (0x31U)

typedef struct NV0000_CTRL_SYSTEM_ENABLE_ETW_EVENTS_PARAMS {
    NvU32 moduleMask;
} NV0000_CTRL_SYSTEM_ENABLE_ETW_EVENTS_PARAMS;

#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_ALL         (0x00000001U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_NOFREQ      (0x00000002U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_FLUSH       (0x00000004U)

#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_PERF        (0x00000010U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_ELPG        (0x00000020U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_NVDPS       (0x00000040U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_POWER       (0x00000080U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_DISP        (0x00000100U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_RMAPI       (0x00000200U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_INTR        (0x00000400U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_LOCK        (0x00000800U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_RCJOURNAL   (0x00001000U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_GENERIC     (0x00002000U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_THERM       (0x00004000U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_GPS         (0x00008000U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_PCIE        (0x00010000U)
#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_NVTELEMETRY (0x00020000U)

/*
 * NV0000_CTRL_CMD_SYSTEM_GPS_GET_FRM_DATA
 *
 * This command is used to read FRL data based on need.
 *
 *   nextSampleNumber
 *     This parameter returns the counter of next sample which is being filled.
 *   samples
 *     This parameter returns the frame time, render time, target time, client ID 
 *     with one reserve bit for future use.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_GPS_GET_FRM_DATA       (0x12fU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE_SIZE   64U

typedef struct NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE {
    NvU16 frameTime;
    NvU16 renderTime;
    NvU16 targetTime;
    NvU8  sleepTime;
    NvU8  sampleNumber;
} NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE;

#define NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS_MESSAGE_ID (0x2FU)

typedef struct NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS {
    NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE samples[NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE_SIZE];
    NvU8                                   nextSampleNumber;
} NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GPS_SET_FRM_DATA
 *
 * This command is used to write FRM data based on need.
 *
 *   frameTime
 *     This parameter contains the frame time of current frame.
 *   renderTime
 *     This parameter contains the render time of current frame.
 *   targetTime
 *     This parameter contains the target time of current frame.
 *   sleepTime
 *     This parameter contains the sleep duration inserted by FRL for the latest frame.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_GPS_SET_FRM_DATA (0x132U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS_MESSAGE_ID (0x32U)

typedef struct NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS {
    NV0000_CTRL_SYSTEM_GPS_FRM_DATA_SAMPLE sampleData;
} NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO
 *
 * This command returns the current host driver, host OS and 
 * plugin information. It is only valid when VGX is setup.
 *   szHostDriverVersionBuffer
 *       This field returns the host driver version (NV_VERSION_STRING).
 *   szHostVersionBuffer
 *       This field returns the host driver version (NV_BUILD_BRANCH_VERSION).
 *   szHostTitleBuffer
 *       This field returns the host driver title (NV_DISPLAY_DRIVER_TITLE).
 *   szPluginTitleBuffer
 *       This field returns the plugin build title (NV_DISPLAY_DRIVER_TITLE).
 *   szHostUnameBuffer
 *       This field returns the call of 'uname' on the host OS.
 *   iHostChangelistNumber
 *       This field returns the changelist value of the host driver (NV_BUILD_CHANGELIST_NUM).
 *   iPluginChangelistNumber
 *       This field returns the changelist value of the plugin (NV_BUILD_CHANGELIST_NUM).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE 256U
#define NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO             (0x133U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_MESSAGE_ID (0x33U)

typedef struct NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS {
    char  szHostDriverVersionBuffer[NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE];
    char  szHostVersionBuffer[NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE];
    char  szHostTitleBuffer[NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE];
    char  szPluginTitleBuffer[NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE];
    char  szHostUnameBuffer[NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE];
    NvU32 iHostChangelistNumber;
    NvU32 iPluginChangelistNumber;
} NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_GPUS_POWER_STATUS
 *
 * This command returns the power status of the GPUs in the system, successfully attached or not because of
 * insufficient power. It is supported on Kepler and up only.
 *   gpuCount
 *       This field returns the count into the following arrays.
 *   busNumber
 *       This field returns the busNumber of a GPU.
 *   gpuExternalPowerStatus
 *       This field returns the corresponding external power status:
 *          NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_CONNECTED
 *          NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_NOT_CONNECTED
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_GET_GPUS_POWER_STATUS (0x134U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS_MESSAGE_ID (0x34U)

typedef struct NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS {
    NvU8 gpuCount;
    NvU8 gpuBus[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU8 gpuExternalPowerStatus[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
} NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS;

/* Valid gpuExternalPowerStatus values */
#define NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_CONNECTED     0U
#define NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_NOT_CONNECTED 1U

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_PRIVILEGED_STATUS
 * 
 * This command returns the caller's API access privileges using
 * this client handle.
 *
 *   privStatus
 *     This parameter returns a mask of possible access privileges:
 *       NV0000_CTRL_SYSTEM_PRIVILEGED_STATUS_PRIV_USER_FLAG
 *         The caller is running with elevated privileges
 *       NV0000_CTRL_SYSTEM_PRIVILEGED_STATUS_ROOT_HANDLE_FLAG
 *         Client is of NV01_ROOT class.
 *       NV0000_CTRL_SYSTEM_PRIVILEGED_STATUS_PRIV_HANDLE_FLAG
 *         Client has PRIV bit set.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */


#define NV0000_CTRL_CMD_SYSTEM_GET_PRIVILEGED_STATUS               (0x135U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS_MESSAGE_ID (0x35U)

typedef struct NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS {
    NvU8 privStatusFlags;
} NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS;


/* Valid privStatus values */
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_USER_FLAG     (0x00000001U)
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_KERNEL_HANDLE_FLAG (0x00000002U)
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_HANDLE_FLAG   (0x00000004U)

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_FABRIC_STATUS
 *
 * The fabric manager (FM) notifies RM that fabric (system) is ready for peer to
 * peer (P2P) use or still initializing the fabric. This command allows clients
 * to query fabric status to allow P2P operations.
 *
 * Note, on systems where FM isn't used, RM just returns _SKIP.
 *
 * fabricStatus
 *     This parameter returns current fabric status:
 *          NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_SKIP
 *          NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_UNINITIALIZED
 *          NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_IN_PROGRESS
 *          NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_INITIALIZED
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

typedef enum NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS {
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_SKIP = 1,
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_UNINITIALIZED = 2,
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_IN_PROGRESS = 3,
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS_INITIALIZED = 4,
} NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS;

#define NV0000_CTRL_CMD_SYSTEM_GET_FABRIC_STATUS (0x136U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS_MESSAGE_ID (0x36U)

typedef struct NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS {
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS fabricStatus;
} NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS;

/*
 * NV0000_CTRL_VGPU_GET_VGPU_VERSION_INFO
 *
 * This command is used to query the range of VGX version supported.
 *
 *  host_min_supported_version
 *     The minimum vGPU version supported by host driver
 *  host_max_supported_version
 *     The maximum vGPU version supported by host driver
 *  user_min_supported_version
 *     The minimum vGPU version set by user for vGPU support
 *  user_max_supported_version
 *     The maximum vGPU version set by user for vGPU support
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 */
#define NV0000_CTRL_VGPU_GET_VGPU_VERSION (0x137U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_VGPU_GET_VGPU_VERSION
 */
#define NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS_MESSAGE_ID (0x37U)

typedef struct NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS {
    NvU32 host_min_supported_version;
    NvU32 host_max_supported_version;
    NvU32 user_min_supported_version;
    NvU32 user_max_supported_version;
} NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS;

/*
 * NV0000_CTRL_VGPU_SET_VGPU_VERSION
 *
 * This command is used to query whether pGPU is live migration capable or not.
 *
 *  min_version
 *      The minimum vGPU version to be supported being set
 *  max_version
 *      The maximum vGPU version to be supported being set
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 */
#define NV0000_CTRL_VGPU_SET_VGPU_VERSION (0x138U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS
 */
#define NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS_MESSAGE_ID (0x38U)

typedef struct NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS {
    NvU32 min_version;
    NvU32 max_version;
} NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS;

/*
 * NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID
 *
 * This command is used to get a unique identifier for the instance of RM.
 * The returned value will only change when the driver is reloaded. A previous
 * value will never be reused on a given machine.
 *
 *  rm_instance_id;
 *      The instance ID of the current RM instance
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_RM_INSTANCE_ID (0x139U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS
 */
#define NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 rm_instance_id, 8);
} NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO
 *
 * This API is used to get the TPP(total processing power) and 
 * the rated TGP(total GPU power) from SBIOS.
 *
 * NVPCF is an acronym for Nvidia Platform Controllers and Framework 
 * which implements platform level policies. NVPCF is implemented in
 * a kernel driver on windows. It is implemented in a user mode app
 * called nvidia-powerd on Linux.
 *
 *   Valid subFunc ids for NVPCF 1x include :
 *   NVPCF0100_CTRL_CONFIG_DSM_1X_FUNC_GET_SUPPORTED
 *   NVPCF0100_CTRL_CONFIG_DSM_1X_FUNC_GET_DYNAMIC_PARAMS
 *
 *   Valid subFunc ids for NVPCF 2x include :
 *   NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED
 *   NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_PARAMS
 *   NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CONFIG_TABLES
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO (0x13bU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS_MESSAGE_ID" */
#define NVPCF_CTRL_SYSPWRLIMIT_TYPE_BASE                 1U
#define NV0000_CTRL_SYSTEM_POWER_INFO_INDEX_MAX_SIZE     32U

#define NV0000_CTRL_CMD_SYSTEM_GET_SYSTEM_POWER_LIMIT_MESSAGE_ID (0x48U)

typedef struct NV0000_CTRL_CMD_SYSTEM_GET_SYSTEM_POWER_LIMIT {

    /* Battery state of charge threshold (percent 0-100) */
    NvU8  batteryStateOfChargePercent;

    /* Long Timescale Battery current limit (milliamps) */
    NvU32 batteryCurrentLimitmA;

    /* Rest of system reserved power (milliwatts) */
    NvU32 restOfSytemReservedPowermW;

    /* Min CPU TDP (milliwatts) */
    NvU32 minCpuTdpmW;

    /* Max CPU TDP (milliwatts) */
    NvU32 maxCpuTdpmW;

    /* Short Timescale Battery current limit (milliamps) */
    NvU32 shortTimescaleBatteryCurrentLimitmA;
} NV0000_CTRL_CMD_SYSTEM_GET_SYSTEM_POWER_LIMIT;

/*!
 * States for the Battery CPU TDP Control ability.
 * _CPU_TDP_CONTROL_TYPE_DC_ONLY :==> Legacy setting for DC only CPU TDP Control
 * _CPU_TDP_CONTROL_TYPE_DC_AC   :==> AC and DC both support CPU TDP Control
 */
typedef enum QBOOST_CPU_TDP_CONTROL_TYPE {
    QBOOST_CPU_TDP_CONTROL_TYPE_DC_ONLY = 0,
    QBOOST_CPU_TDP_CONTROL_TYPE_DC_AC = 1,
} QBOOST_CPU_TDP_CONTROL_TYPE;

#define NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS_MESSAGE_ID (0x3BU)

typedef struct NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS {
    /*Buffer to get all the supported functions*/
    NvU32  supportedFuncs;

    /* GPU ID */
    NvU32  gpuId;

    /* Total processing power including CPU and GPU */
    NvU32  tpp;

    /* Rated total GPU Power */
    NvU32  ratedTgp;

    /* NVPCF subfunction id */
    NvU32  subFunc;

    /* Configurable TGP offset, in mW */
    NvS32  ctgpOffsetmW;

    /* TPP, as offset in mW */
    NvS32  targetTppOffsetmW;

    /* Maximum allowed output, as offset in mW */
    NvS32  maxOutputOffsetmW;

    /* Minimum allowed output, as offset in mW */
    NvS32  minOutputOffsetmW;

    /* Configurable TGP offset, on battery, in milli-Watts. */
    NvS32  ctgpBattOffsetmW;

    /* Target total processing power on battery, offset, in milli-Watts. */
    NvS32  targetTppBattOffsetmW;

    /*
     * Maximum allowed output on battery, offset, in milli-Watts.
     */
    NvS32  maxOutputBattOffsetmW;

    /*
     * Minimum allowed output on battery, offset, in milli-Watts.
     */
    NvS32  minOutputBattOffsetmW;

    /*
     * If value specified is larger than the statically assigned ROS reserve in
     * the system power limits table, this will take affect.
     *
     * A value of zero naturally works as a clear as it will be lesser than the
     * statically assigned value.
     */
    NvU32  dcRosReserveOverridemW;

    /*
     * This is the active arbitrated long timescale limit provided by Qboost and
     * honored by JPAC/JPPC
     */
    NvU32  dcTspLongTimescaleLimitmA;

   /*
    * This is the active arbitrated short timescale limit provided by Qboost and
    * honored by RM/PMU
    */
    NvU32  dcTspShortTimescaleLimitmA;

    /* Dynamic Boost AC support */
    NvBool bEnableForAC;

    /* Dynamic Boost DC support */
    NvBool bEnableForDC;

    /* The System Controller Table Version */
    NvU8   version;

    /* Base sampling period */
    NvU16  samplingPeriodmS;

    /* Sampling Multiplier */
    NvU16  samplingMulti;

    /* Fitler function type */
    NvU8   filterType;

    union {

        /* weight */
        NvU8 weight;

        /* windowSize */
        NvU8 windowSize;
    } filterParam;

    /* Reserved */
    NvU16                                         filterReserved;

    /* Controller Type Dynamic Boost Controller */
    NvBool                                        bIsBoostController;

    /* Increase power limit ratio */
    NvU16                                         incRatio;

    /* Decrease power limit ratio */
    NvU16                                         decRatio;

    /* Dynamic Boost Controller DC Support */
    NvBool                                        bSupportBatt;

    /* CPU type(Intel/AMD) */
    NvU8                                          cpuType;

    /* GPU type(Nvidia) */
    NvU8                                          gpuType;

    /* System Power Table info index */
    NvU32                                         sysPwrIndex;

    /* System Power Table get table limits */
    NV0000_CTRL_CMD_SYSTEM_GET_SYSTEM_POWER_LIMIT sysPwrGetInfo[NV0000_CTRL_SYSTEM_POWER_INFO_INDEX_MAX_SIZE];

    /*
     * Does this version of the system power limits table support TSP -> table
     * version 2.0 and later should set this to true
     */
    NvBool                                        bIsTspSupported;

    /*
     * Stores the System Power Limits (Battery State of Charge aka BSOC) table version implemented by the SBIOS
     *
     */
    NvU8                                          sysPwrLimitsTableVersion;

    /* SYSPWRLIMIT class types */
    NvU32                                         type;

    /* CPU TDP Limit to be set (milliwatts) */
    NvU32                                         cpuTdpmw;

    /* CPU TDP Control Support */
    QBOOST_CPU_TDP_CONTROL_TYPE                   cpuTdpControlType;
} NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS;

/* Define the filter types */
#define CONTROLLER_FILTER_TYPE_EMWA                                        0U
#define CONTROLLER_FILTER_TYPE_MOVING_MAX                                  1U

/* Valid NVPCF subfunction case */
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED_CASE               2U
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_CASE                 3U

/* NVPCF subfunction to get the static data tables */
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CASE                  4U

/* NVPCF subfunction to get the system power limits table */
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DC_SYSTEM_POWER_LIMITS_CASE  5U

/* NVPCF subfunction to change the CPU's TDP limit */
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_CPU_TDP_LIMIT_CONTROL_CASE       6U

/* Valid NVPCF subfunction ids */
#define NVPCF0100_CTRL_CONFIG_DSM_1X_FUNC_GET_SUPPORTED                    (0x00000000)
#define NVPCF0100_CTRL_CONFIG_DSM_1X_FUNC_GET_DYNAMIC_PARAMS               (0x00000002)

/*
 *  Defines for get supported sub functions bit fields
 */
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_SUPPORTED_IS_SUPPORTED                     0:0
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_SUPPORTED_IS_SUPPORTED_YES                 1
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_SUPPORTED_IS_SUPPORTED_NO                  0
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_DC_SYSTEM_POWER_LIMITS_IS_SUPPORTED        8:8
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_DC_SYSTEM_POWER_LIMITS_IS_SUPPORTED_YES    1
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_GET_DC_SYSTEM_POWER_LIMITS_IS_SUPPORTED_NO     0
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_CPU_TDP_LIMIT_CONTROL_IS_SUPPORTED             9:9
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_CPU_TDP_LIMIT_CONTROL_IS_SUPPORTED_YES         1
#define NVPCF0100_CTRL_CONFIG_DSM_FUNC_CPU_TDP_LIMIT_CONTROL_IS_SUPPORTED_NO          0


/*!
 * Config DSM 2x version specific defines
 */
#define NVPCF0100_CTRL_CONFIG_DSM_2X_VERSION                               (0x00000200)
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_SUPPORTED                    (0x00000000)
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CONFIG_TABLES         (0x00000001)
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_PARAMS               (0x00000002)
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DC_SYSTEM_POWER_LIMITS_TABLE (0x00000008)
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_CPU_TDP_LIMIT_CONTROL            (0x00000009)

/*!
 * Defines the max buffer size for config
 */
#define NVPCF0100_CTRL_CONFIG_2X_BUFF_SIZE_MAX                            (255)

/*
 * NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT
 *
 * This API is used to sync the external fabric management status with
 * GSP-RM
 *
 *  bExternalFabricMgmt
 *      Whether fabric is externally managed
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT                   (0x13cU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS_MESSAGE_ID (0x3CU)

typedef struct NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS {
    NvBool bExternalFabricMgmt;
} NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS;

/*
 * NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO
 *
 * This API is used to get information about the RM client
 * database.
 *
 * clientCount [OUT]
 *  This field indicates the number of clients currently allocated.
 *
 * resourceCount [OUT]
 *  This field indicates the number of resources currently allocated
 *  across all clients.
 *
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_CLIENT_DATABASE_INFO (0x13dU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS_MESSAGE_ID (0x3DU)

typedef struct NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS {
    NvU32 clientCount;
    NV_DECLARE_ALIGNED(NvU64 resourceCount, 8);
} NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION_V2
 *
 * This command returns the current driver information in
 * statically sized character arrays.
 *
 *   driverVersionBuffer
 *       This field returns the version (NV_VERSION_STRING).
 *   versionBuffer
 *       This field returns the version (NV_BUILD_BRANCH_VERSION).
 *   driverBranch
 *       This field returns the branch (NV_BUILD_BRANCH).
 *   titleBuffer
 *       This field returns the title (NV_DISPLAY_DRIVER_TITLE).
 *   changelistNumber
 *       This field returns the changelist value (NV_BUILD_CHANGELIST_NUM).
 *   officialChangelistNumber
 *       This field returns the last official changelist value
 *       (NV_LAST_OFFICIAL_CHANGELIST_NUM).
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE 256U
#define NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION_V2             (0x13eU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS_MESSAGE_ID (0x3EU)

typedef struct NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS {
    char  driverVersionBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    char  versionBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    char  driverBranch[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    char  titleBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    NvU32 changelistNumber;
    NvU32 officialChangelistNumber;
} NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_RMCTRL_CACHE_MODE_CTRL
 *
 * This API is used to get/set RMCTRL cache mode
 *
 * cmd [IN]
 *   GET - Gets RMCTRL cache mode
 *   SET - Sets RMCTRL cache mode
 *
 * mode [IN/OUT]
 *  On GET, this field is the output of current RMCTRL cache mode
 *  On SET, this field indicates the mode to set RMCTRL cache to
 *  Valid values for this parameter are:
 *    NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_DISABLE
 *      No get/set action to cache.
 *    NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_ENABLE
 *      Try to get from cache at the beginning of the control.
 *      Set cache after control finished if the control has not been cached.
 *    NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_VERIFY_ONLY
 *      Do not get from cache. Set cache when control call finished.
 *      When setting the cache, verify the value in the cache is the same
 *      with the current control value if the control is already cached.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_SYSTEM_RMCTRL_CACHE_MODE_CTRL (0x13fU) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS_MESSAGE_ID (0x3FU)

typedef struct NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS {
    NvU32 cmd;
    NvU32 mode;
} NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS;

#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_CMD_GET          (0x00000000U)
#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_CMD_SET          (0x00000001U)

#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_DISABLE     (0x00000000U)
#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_ENABLE      (0x00000001U)
#define NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_VERIFY_ONLY (0x00000002U)

/*
 * NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CONTROL
 *
 * This command is used to control PFM_REQ_HNDLR functionality.  It allows control of
 * GPU Performance Scaling (PFM_REQ_HNDLR), changing its operational parameters and read
 * most PFM_REQ_HNDLR dynamic parameters.
 *
 *   command
 *     This parameter specifies the command to execute.  Invalid commands
 *     result in the return of an NV_ERR_INVALID_ARGUMENT status.
 *   locale
 *     This parameter indicates the specific locale to which the command
 *     'command' is to be applied.
*     Supported range of CPU/GPU {i = 0, ..., 255}
 *   data
 *     This parameter contains a command-specific data payload.  It can
 *     be used to input data as well as output data.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_COMMAND
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_DATA
 *   NV_ERR_INVALID_REQUEST
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CONTROL               (0x140U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS_MESSAGE_ID (0x40U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS {
    NvU16 command;
    NvU16 locale;
    NvU32 data;
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS;

/* 
 *  Valid command values :
 *  
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_INIT
 *    Is used to check if PFM_REQ_HNDLR was correctly initialized.
 *    Possible return (OUT) values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INIT_NO
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INIT_YES
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_EXEC
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_EXEC
 *    Are used to stop/start PFM_REQ_HNDLR functionality and to get current status.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_EXEC_STOP
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_EXEC_START
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_ACTIONS
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_ACTIONS
 *    Are used to control execution of PFM_REQ_HNDLR actions and to get current status.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_ACTIONS_OFF
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_ACTIONS_ON
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_LOGIC
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_LOGIC
 *    Are used to switch current PFM_REQ_HNDLR logic and to retrieve current logic.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_OFF
 *        Will cause that all PFM_REQ_HNDLR actions will be NULL.
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_FUZZY
 *        Fuzzy logic will determine PFM_REQ_HNDLR actions based on current ruleset.
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_DETERMINISTIC
 *        Deterministic logic will define PFM_REQ_HNDLR actions based on current ruleset.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_PREFERENCE
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_PREFERENCE
 *    Are used to set/retrieve system control preference.
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_CPU
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_GPU
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_BOTH
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_GPU2CPU_LIMIT
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_GPU2CPU_LIMIT
 *    Are used to set/retrieve GPU2CPU pstate limits.
 *    IN/OUT values are four bytes packed into a 32-bit data field.
 *    The CPU cap index for GPU pstate 0 is in the lowest byte, the CPU cap
 *    index for the GPU pstate 3 is in the highest byte, etc.  One
 *    special value is to disable the override to the GPU2CPU map:
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_PMU_PFM_REQ_HNDLR_STATE
 *    Is used to stop/start PFM_REQ_HNDLR PMU functionality.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_PMU_PFM_REQ_HNDLR_STATE
 *    Is used to get the current status of PMU PFM_REQ_HNDLR.
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_NO_MAP_OVERRIDE
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_MAX_POWER
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_MAX_POWER
 *    Are used to set/retrieve max power [mW] that system can provide.
 *    This is hardcoded PFM_REQ_HNDLR safety feature and logic/rules does not apply
 *    to this threshold.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_COOLING_BUDGET
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_COOLING_BUDGET
 *    Are used to set/retrieve current system cooling budget [mW].
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_INTEGRAL_PERIOD
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_INTEGRAL_PERIOD
 *    Are used to set/retrieve integration interval [sec].
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_RULESET
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_RULESET
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_RULE_COUNT
 *    Are used to set/retrieve used ruleset [#].  Value is checked
 *    against MAX number of rules for currently used PFM_REQ_HNDLR logic. Also COUNT
 *    provides a way to find out how many rules exist for the current control 
 *    system.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_APP_BOOST
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_APP_BOOST
 *    Is used to set/get a delay relative to now during which to allow unbound 
 *    CPU performance.  Units are seconds.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_PWR_SUPPLY_MODE
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_PWR_SUPPLY_MODE
 *    Is used to override/get the actual power supply mode (AC/Battery).
 *    Possible IN/OUT values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_REAL
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_FAKE_AC
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_FAKE_BATT
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_VCT_SUPPORT_INFO
 *    Is used to get the Ventura system information for VCT tool
 *    Returned 32bit value should be treated as bitmask and decoded in
 *    following way:
 *    Encoding details are defined in objPFM_REQ_HNDLR.h refer to
 *    NV_PFM_REQ_HNDLR_SYS_SUPPORT_INFO and corresponding bit defines.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_SUPPORTED_FUNCTION
 *    Is used to get the supported sub-functions defined in SBIOS.  Returned
 *    value is a bitmask where each bit corresponds to different function:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SUPPORT
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_VENTURASTATUS
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_GETPSS
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SETPPC
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_GETPPC
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_VENTURACB
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SYSPARAMS
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_DELTA
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_FUTURE
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_LTMAVG
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_INTEGRAL
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_BURDEN
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_INTERMEDIATE
 *    Are used to retrieve appropriate power measurements and their derivatives
 *    in [mW] for required locale.  _BURDEN is defined only for _LOCALE_SYSTEM.
 *    _INTERMEDIATE is not defined for _LOCALE_SYSTEM, and takes an In value as
 *    index.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_SENSOR_PARAMETERS
 *    Is used to retrieve parameters when adjusting raw sensor power reading.
 *    The values may come from SBIOS, VBIOS, registry or driver default.
 *    Possible IN value is the index of interested parameter.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP_DELTA
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP_FUTURE
 *    Are used to retrieve appropriate temperature measurements and their
 *    derivatives in [1/1000 Celsius].
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_CAP
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_MIN
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_MAX
 *    Are used to retrieve CPU(x)/GPU(x) p-state or it's limits.
 *    Not applicable to _LOCALE_SYSTEM.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_ACTION
 *    Is used to retrieve last PFM_REQ_HNDLR action for given domain.
 *    Not applicable to _LOCALE_SYSTEM.
 *    Possible return (OUT) values are:
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DEC_TO_P0
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DEC_BY_1
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DO_NOTHING
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_SET_CURRENT
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_BY_1
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_BY_2
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_TO_LFM
 *      NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_TO_SLFM
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_SET_POWER_SIM_STATE
 *    Is used to set the power sensor simulator state.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_SIM_STATE
 *    Is used to get the power simulator sensor simulator state.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_SET_POWER_SIM_DATA
 *    Is used to set power sensor simulator data
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_SIM_DATA
 *    Is used to get power sensor simulator data
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_INIT_USING_SBIOS_AND_ACK
 *    Is used to respond to the ACPI event triggered by SBIOS.  RM will
 *    request value for budget and status, validate them, apply them
 *    and send ACK back to SBIOS.
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_PING_SBIOS_FOR_EVENT
 *    Is a test cmd that should notify SBIOS to send ACPI event requesting
 *    budget and status change.
 */
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_INVALID                             (0xFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_INIT                        (0x0000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_EXEC                        (0x0001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_EXEC                        (0x0002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_ACTIONS                     (0x0003U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_ACTIONS                     (0x0004U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_LOGIC                       (0x0005U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_LOGIC                       (0x0006U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_PREFERENCE                  (0x0007U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_PREFERENCE                  (0x0008U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_GPU2CPU_LIMIT               (0x0009U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_GPU2CPU_LIMIT               (0x000AU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_SET_PMU_PFM_REQ_HNDLR_STATE     (0x000BU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_GET_PMU_PFM_REQ_HNDLR_STATE     (0x000CU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_MAX_POWER                   (0x0100U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_MAX_POWER                   (0x0101U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_COOLING_BUDGET              (0x0102U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_COOLING_BUDGET              (0x0103U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_INTEGRAL_PERIOD             (0x0104U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_INTEGRAL_PERIOD             (0x0105U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_RULESET                     (0x0106U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_RULESET                     (0x0107U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_RULE_COUNT                  (0x0108U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_APP_BOOST                   (0x0109U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_APP_BOOST                   (0x010AU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_SET_PWR_SUPPLY_MODE             (0x010BU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_PWR_SUPPLY_MODE             (0x010CU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_VCT_SUPPORT_INFO            (0x010DU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_SUPPORTED_FUNCTIONS         (0x010EU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER                      (0x0200U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_DELTA                (0x0201U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_FUTURE               (0x0202U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_LTMAVG               (0x0203U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_INTEGRAL             (0x0204U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_BURDEN               (0x0205U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_INTERMEDIATE         (0x0206U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_SENSOR_PARAMETERS          (0x0210U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP                       (0x0220U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP_DELTA                 (0x0221U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_TEMP_FUTURE                (0x0222U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE                     (0x0240U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_CAP                 (0x0241U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_MIN                 (0x0242U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_MAX                 (0x0243U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_ACTION              (0x0244U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_PSTATE_SLFM_PRESENT        (0x0245U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_SET_POWER_SIM_STATE            (0x0250U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_SIM_STATE            (0x0251U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_SET_POWER_SIM_DATA             (0x0252U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_GET_POWER_SIM_DATA             (0x0253U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_INIT_USING_SBIOS_AND_ACK       (0x0320U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_PING_SBIOS_FOR_EVENT           (0x0321U)

/* valid LOCALE values */
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_INVALID                      (0xFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_SYSTEM                       (0x0000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_CPU(i)           (0x0100+((i)%0x100))
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_GPU(i)           (0x0200+((i)%0x100))

/* valid data values for enums */
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INVALID                     (0x80000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INIT_NO                     (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INIT_YES                    (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_EXEC_STOP                   (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_EXEC_START                  (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_ACTIONS_OFF                 (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_ACTIONS_ON                  (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_OFF                   (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_FUZZY                 (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_LOGIC_DETERMINISTIC         (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_CPU              (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_GPU              (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PREFERENCE_BOTH             (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_NO_MAP_OVERRIDE             (0xFFFFFFFFU)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PMU_PFM_REQ_HNDLR_STATE_OFF (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_PMU_PFM_REQ_HNDLR_STATE_ON  (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_REAL             (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_FAKE_AC          (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_PWR_SUPPLY_FAKE_BATT        (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SUPPORT                (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_VENTURASTATUS          (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_GETPSS                 (0x00000004U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SETPPC                 (0x00000008U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_GETPPC                 (0x00000010U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_VENTURACB              (0x00000020U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SYSPARAMS              (0x00000040U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DEC_TO_P0           (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DEC_BY_1            (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_DO_NOTHING          (0x00000002U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_SET_CURRENT         (0x00000003U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_BY_1            (0x00000004U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_BY_2            (0x00000005U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_TO_LFM          (0x00000006U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_ACTION_INC_TO_SLFM         (0x00000007U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_SLFM_PRESENT_NO            (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_SLFM_PRESENT_YES           (0x00000001U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_POWER_SIM_STATE_OFF        (0x00000000U)
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_DEF_POWER_SIM_STATE_ON         (0x00000001U)

/*
 *  NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL
 *
 *  This command allows execution of multiple PFM_REQ_HNDLRControl commands within one
 *  RmControl call.  For practical reasons # of commands is limited to 16.
 *  This command shares defines with NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CONTROL.
 *
 *    cmdCount
 *      Number of commands that should be executed.
 *      Less or equal to NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_COMMAND_MAX.
 * 
 *    succeeded
 *      Number of commands that were succesully executed.
 *      Less or equal to NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_COMMAND_MAX.
 *      Failing commands return NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CMD_DEF_INVALID
 *      in their data field.
 *
 *    cmdData
 *      Array of commands with following structure:
 *        command
 *          This parameter specifies the command to execute.
 *          Invalid commands result in the return of an
 *          NV_ERR_INVALID_ARGUMENT status.
 *        locale
 *          This parameter indicates the specific locale to which
 *          the command 'command' is to be applied.
 *          Supported range of CPU/GPU {i = 0, ..., 255}
 *        data
 *          This parameter contains a command-specific data payload.
 *          It is used both to input data as well as to output data.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_REQUEST
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL                       (0x141U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_COMMAND_MAX                   (16U)
#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS {
    NvU32 cmdCount;
    NvU32 succeeded;

    struct {
        NvU16 command;
        NvU16 locale;
        NvU32 data;
    } cmdData[NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_BATCH_COMMAND_MAX];
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CTRL
 *
 * This command is used to execute general PFM_REQ_HNDLR Functions, most dealing with
 * calling SBIOS, or retrieving cached sensor and PFM_REQ_HNDLR state data.
 *
 *   version
 *     This parameter specifies the version of the interface.  Legal values
 *     for this parameter are 1.
 *   cmd
 *     This parameter specifies the PFM_REQ_HNDLR API to be invoked. 
 *     Valid values for this parameter are:
 *       NV0000_CTRL_PFM_REQ_HNDLR_CMD_GET_THERM_LIMIT
 *         This command gets the temperature limit for thermal controller. When
 *         this command is specified the input parameter contains ???.
 *      NV0000_CTRL_PFM_REQ_HNDLR_CMD_SET_THERM_LIMIT
 *         This command set the temperature limit for thermal controller.  When
 *         this command is specified the input parameter contains ???.
 *   input
 *     This parameter specifies the cmd-specific input value.
 *   result
 *     This parameter returns the cmd-specific output value.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CTRL (0x142U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS {
    NvU32 cmd;
    NvS32 input[2];
    NvS32 result[4];
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS;

/* valid version values */
#define NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_PSP_CURRENT_VERSION      (0x00010000U)

/* valid cmd values */
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_THERM_LIMIT               (0x00000002U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                     (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_THERMAL_LIMIT                   (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_MIN_LIMIT                       (0x00000001U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_MAX_LIMIT                       (0x00000002U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_LIMIT_SOURCE                    (0x00000003U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_THERM_LIMIT               (0x00000003U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_THERMAL_LIMIT                    (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_DOWN_N_DELTA    (0x00000004U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_DOWN_N_DELTA          (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_DOWN_N_DELTA    (0x00000005U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_DOWN_N_DELTA           (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_HOLD_DELTA      (0x00000006U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_HOLD_DELTA            (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_HOLD_DELTA      (0x00000007U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_HOLD_DELTA             (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_UP_DELTA        (0x00000008U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_UP_DELTA              (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_UP_DELTA        (0x00000009U)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_UP_DELTA               (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_ENGAGE_DELTA    (0x0000000AU)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_ENGAGE_DELTA          (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_ENGAGE_DELTA    (0x0000000BU)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_ENGAGE_DELTA           (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_DISENGAGE_DELTA (0x0000000CU)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_DISENGAGE_DELTA       (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_DISENGAGE_DELTA (0x0000000DU)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_DISENGAGE_DELTA        (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_STATUS          (0x00000016U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_CTRL_STATUS                (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_STATUS          (0x00000017U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_CTRL_STATUS                 (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_GET_UTIL_AVG_NUM      (0x00000018U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_CPU_SET_UTIL_AVG_NUM            (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_SET_UTIL_AVG_NUM      (0x00000019U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_CPU_GET_UTIL_AVG_NUM             (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PERF_SENSOR               (0x0000001AU)
//      NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX                          (0x00000000)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_NEXT_EXPECTED_POLL               (0x00000001U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_VALUE               (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_AVAILABLE           (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_CALL_ACPI                     (0x0000001BU)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_ACPI_CMD                         (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_ACPI_PARAM_IN                    (0x00000001U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_RESULT_1                   (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_RESULT_2                   (0x00000001U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_PSHAREPARAM_STATUS         (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_PSHAREPARAM_VERSION        (0x00000001U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_PSHAREPARAM_SZ             (0x00000002U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_PSS_SZ                     (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_ACPI_PSS_COUNT                  (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_IGPU_TURBO                (0x0000001CU)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SET_IGPU_TURBO                   (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_PERIOD               (0x00000026U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_PERIOD                      (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_PERIOD               (0x00000027U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_PERIOD                     (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_NUDGE_FACTOR         (0x00000028U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_NUDGE_UP                    (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_NUDGE_DOWN                  (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_NUDGE_FACTOR         (0x00000029U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_NUDGE_UP                   (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_NUDGE_DOWN                 (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_THRESHOLD_SAMPLES    (0x0000002AU)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_THRESHOLD_SAMPLE_HOLD       (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_THRESHOLD_SAMPLE_STEP       (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_THRESHOLD_SAMPLES    (0x0000002BU)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_THRESHOLD_SAMPLE_HOLD      (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_THRESHOLD_SAMPLE_STEP      (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_PERF_LIMITS          (0x0000002CU)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_PERF_LIMIT_UPPER            (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_TEMP_PERF_LIMIT_LOWER            (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_PERF_LIMITS          (0x0000002DU)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_PERF_LIMIT_UPPER           (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_RESULT_TEMP_PERF_LIMIT_LOWER           (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PM1_AVAILABLE             (0x0000002EU)
#define NV0000_CTRL_PFM_REQ_HNDLR_INPUT_PM1_AVAILABLE                    (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PM1_AVAILABLE             (0x0000002FU)
#define NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_PM1_AVAILABLE                   (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS        (0x00000044U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL1    (0x00000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL2    (0x00000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS        (0x00000045U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS_PL1    (0x00000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_FREQ_LIMIT            (0x00000046U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_FREQ_LIMIT_MHZ        (0000000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_FREQ_LIMIT            (0x00000047U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_FREQ_LIMIT_MHZ        (0000000000U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM                       (0x00000048U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM_INDEX                 (0000000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM_AVAILABLE_MASK        (0000000001U)

#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PPM                       (0x00000049U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PPM_INDEX                 (0000000000U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PPM_INDEX_MAX             (2U)

#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX                                   7:0
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_MAXPERF                      (0U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_BALANCED                     (1U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_QUIET                        (2U)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_INVALID                      (0xFFU)
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_MASK                                    15:8
#define NV0000_CTRL_PFM_REQ_HNDLR_PPM_MASK_INVALID                       (0U)

/* valid PS_STATUS result values */
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_PS_STATUS_OFF                      (0U)
#define NV0000_CTRL_PFM_REQ_HNDLR_CMD_PS_STATUS_ON                       (1U)

#define PFM_REQ_HNDLR_MAX_COUNTERS_PER_BLOCK                             32U
typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS {
    NvU32 objHndl;
    NvU32 blockId;
    NvU32 nextExpectedSampleTimems;
    NvU32 countersReq;
    NvU32 countersReturned;
    NvU32 counterBlock[PFM_REQ_HNDLR_MAX_COUNTERS_PER_BLOCK];
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSORS (0x146U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSORS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSORS_PARAMS_MESSAGE_ID (0x46U)

typedef NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSORS_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_GET_EXTENDED_PERF_SENSORS (0x147U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_EXTENDED_PERF_SENSORS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_EXTENDED_PERF_SENSORS_PARAMS_MESSAGE_ID (0x47U)

typedef NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_EXTENDED_PERF_SENSORS_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI
 *
 * This command allows users to call PFM_REQ_HNDLR ACPI commands for testing purposes.
 *
 *   cmd
 *      This parameter specifies the PFM_REQ_HNDLR ACPI command to execute.
 * 
 *   input
 *      This parameter specified the cmd-dependent input value. 
 *
 *   resultSz
 *      This parameter returns the size (in bytes) of the valid data
 *      returned in  the result parameter.
 *
 *   result
 *      This parameter returns the results of the specified cmd.
 *      The maximum size (in bytes) of this returned data will
 *      not exceed PFM_REQ_HNDLR_MAX_ACPI_OUTPUT_BUFFER_SIZE
 *
 *   PFM_REQ_HNDLR_MAX_ACPI_OUTPUT_BUFFER_SIZE
 *      The size of buffer (result) in unit of NvU32.
 *      The smallest value is sizeof(PSS_ENTRY)*ACPI_PSS_ENTRY_MAX. 
 *      Since the prior one is 24 bytes, and the later one is 48, 
 *      this value cannot be smaller than 288.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT,
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 *
 */
#define PFM_REQ_HNDLR_MAX_ACPI_OUTPUT_BUFFER_SIZE 288U
#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS {
    NvU32 cmd;
    NvU32 input;
    NvU32 resultSz;
    NvU32 result[PFM_REQ_HNDLR_MAX_ACPI_OUTPUT_BUFFER_SIZE];
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI        (0x143U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_RMTRACE_MODULE_PFM_REQ_HNDLR       (0x00008000U)

/*
 * NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA
 *
 * This command is used to read FRL data based on need.
 *
 *   nextSampleNumber
 *     This parameter returns the counter of next sample which is being filled.
 *   samples
 *     This parameter returns the frame time, render time, target time, client ID 
 *     with one reserve bit for future use.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA     (0x144U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE_SIZE 64U

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE {
    NvU16 frameTime;
    NvU16 renderTime;
    NvU16 targetTime;
    NvU8  sleepTime;
    NvU8  sampleNumber;
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE;

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS_MESSAGE_ID (0x44U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS {
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE samples[NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE_SIZE];
    NvU8                                             nextSampleNumber;
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS;

/*
 * NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA
 *
 * This command is used to write FRM data based on need.
 *
 *   frameTime
 *     This parameter contains the frame time of current frame.
 *   renderTime
 *     This parameter contains the render time of current frame.
 *   targetTime
 *     This parameter contains the target time of current frame.
 *   sleepTime
 *     This parameter contains the sleep duration inserted by FRL for the latest frame.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA (0x145U) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS_MESSAGE_ID (0x45U)

typedef struct NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS {
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE sampleData;
} NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS;

/* _ctrl0000system_h_ */
