/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0000/ctrl0000system.finn
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
 *       NV0000_CTRL_GET_FEATURES_UEFI
 *         When this bit is set, it is a UEFI system.
 *       NV0000_CTRL_SYSTEM_GET_FEATURES_IS_EFI_INIT
 *         When this bit is set, EFI has initialized core channel 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_FEATURES (0x1f0) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS_MESSAGE_ID (0xF0U)

typedef struct NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS {
    NvU32 featuresMask;
} NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS;



/* Valid feature values */
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI                                 0:0
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI_FALSE         (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_SLI_TRUE          (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_UEFI                                1:1
#define NV0000_CTRL_SYSTEM_GET_FEATURES_UEFI_FALSE        (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_UEFI_TRUE         (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_IS_EFI_INIT                         2:2
#define NV0000_CTRL_SYSTEM_GET_FEATURES_IS_EFI_INIT_FALSE (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_FEATURES_IS_EFI_INIT_TRUE  (0x00000001)
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

#define NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION          (0x101) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS {
    NvU32 sizeOfStrings;
    NV_DECLARE_ALIGNED(NvP64 pDriverVersionBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pVersionBuffer, 8);
    NV_DECLARE_ALIGNED(NvP64 pTitleBuffer, 8);
    NvU32 changelistNumber;
    NvU32 officialChangelistNumber;
} NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_PARAMS;

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
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_CPU_INFO (0x102) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS {
    NvU32 type;                               /* processor type        */
    NvU32 capabilities;                       /* processor caps        */
    NvU32 clock;                              /* processor speed (MHz) */
    NvU32 L1DataCacheSize;                    /* L1 dcache size (KB)   */
    NvU32 L2DataCacheSize;                    /* L2 dcache size (KB)   */
    NvU32 dataCacheLineSize;                  /* L1 dcache bytes/line  */
    NvU32 numLogicalCpus;                     /* logial processor cnt  */
    NvU32 numPhysicalCpus;                    /* physical processor cnt*/
    NvU8  name[52];                           /* embedded cpu name     */
    NvU32 family;                             /* Vendor defined Family and Extended Family combined */
    NvU32 model;                              /* Vendor defined Model and Extended Model combined   */
    NvU8  stepping;                           /* Silicon stepping      */
    NvU32 coresOnDie;                         /* cpu cores per die     */
} NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS;

/*  processor type values */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN            (0x00000000)
/* Intel types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P5                 (0x00000001)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P55                (0x00000002)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P6                 (0x00000003)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P2                 (0x00000004)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P2XC               (0x00000005)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CELA               (0x00000006)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P3                 (0x00000007)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P3_INTL2           (0x00000008)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_P4                 (0x00000009)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2              (0x00000010)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CELN_M16H          (0x00000011)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2_EXTRM        (0x00000012)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ATOM               (0x00000013)
/* AMD types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K5                 (0x00000030)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K6                 (0x00000031)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K62                (0x00000032)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K63                (0x00000033)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K7                 (0x00000034)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K8                 (0x00000035)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K10                (0x00000036)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_K11                (0x00000037)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_RYZEN              (0x00000038)
/* IDT/Centaur types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_C6                 (0x00000060)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_C62                (0x00000061)
/* Cyrix types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_GX                 (0x00000070)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_M1                 (0x00000071)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_M2                 (0x00000072)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_MGX                (0x00000073)
/* Transmeta types  */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_TM_CRUSOE          (0x00000080)
/* IBM types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC603             (0x00000090)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC604             (0x00000091)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_PPC750             (0x00000092)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_POWERN             (0x00000093)
/* Unknown ARM architecture CPU type */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_UNKNOWN        (0xA0000000)
/* ARM Ltd types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A9             (0xA0000009)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A15            (0xA000000F)
/* NVIDIA types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_1_0      (0xA0001000)
#define NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_2_0      (0xA0002000)

/* Generic types */
#define NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV8A_GENERIC     (0xA00FF000)

/* processor capabilities */
#define NV0000_CTRL_SYSTEM_CPU_CAP_MMX                 (0x00000001)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE                 (0x00000002)
#define NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW               (0x00000004)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE2                (0x00000008)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE              (0x00000010)
#define NV0000_CTRL_SYSTEM_CPU_CAP_WRITE_COMBINING     (0x00000020)
#define NV0000_CTRL_SYSTEM_CPU_CAP_ALTIVEC             (0x00000040)
#define NV0000_CTRL_SYSTEM_CPU_CAP_PUT_NEEDS_IO        (0x00000080)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WC_WORKAROUND (0x00000100)
#define NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW_EXT           (0x00000200)
#define NV0000_CTRL_SYSTEM_CPU_CAP_MMX_EXT             (0x00000400)
#define NV0000_CTRL_SYSTEM_CPU_CAP_CMOV                (0x00000800)
#define NV0000_CTRL_SYSTEM_CPU_CAP_CLFLUSH             (0x00001000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WAR_190854    (0x00002000) /* deprecated */
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE3                (0x00004000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WAR_124888    (0x00008000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_HT_CAPABLE          (0x00010000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE41               (0x00020000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_SSE42               (0x00040000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_AVX                 (0x00080000)
#define NV0000_CTRL_SYSTEM_CPU_CAP_ERMS                (0x00100000)

/* feature mask (as opposed to bugs, requirements, etc.) */
#define NV0000_CTRL_SYSTEM_CPU_CAP_FEATURE_MASK        (0x1f5e7f) /* finn: Evaluated from "(NV0000_CTRL_SYSTEM_CPU_CAP_MMX | NV0000_CTRL_SYSTEM_CPU_CAP_SSE | NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW | NV0000_CTRL_SYSTEM_CPU_CAP_SSE2 | NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE | NV0000_CTRL_SYSTEM_CPU_CAP_WRITE_COMBINING | NV0000_CTRL_SYSTEM_CPU_CAP_ALTIVEC | NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW_EXT | NV0000_CTRL_SYSTEM_CPU_CAP_MMX_EXT | NV0000_CTRL_SYSTEM_CPU_CAP_CMOV | NV0000_CTRL_SYSTEM_CPU_CAP_CLFLUSH | NV0000_CTRL_SYSTEM_CPU_CAP_SSE3 | NV0000_CTRL_SYSTEM_CPU_CAP_HT_CAPABLE | NV0000_CTRL_SYSTEM_CPU_CAP_SSE41 | NV0000_CTRL_SYSTEM_CPU_CAP_SSE42 | NV0000_CTRL_SYSTEM_CPU_CAP_AVX | NV0000_CTRL_SYSTEM_CPU_CAP_ERMS)" */

/*
 * NV0000_CTRL_CMD_SYSTEM_GET_CAPS
 *
 * This command returns the set of system capabilities in the
 * form of an array of unsigned bytes.  System capabilities include
 * supported features and required workarounds for the system,
 * each represented by a byte offset into the table and a bit
 * position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0000_CTRL_SYSTEM_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the system caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_CAPS                (0x103) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | 0x3" */

typedef struct NV0000_CTRL_SYSTEM_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV0000_CTRL_SYSTEM_GET_CAPS_PARAMS;

/* extract cap bit setting from tbl */
#define NV0000_CTRL_SYSTEM_GET_CAP(tbl,c)           (((NvU8)tbl[(1?c)]) & (0?c))

/* caps format is byte_index:bit_mask */
#define NV0000_CTRL_SYSTEM_CAPS_POWER_SLI_SUPPORTED                 0:0x01

/* size in bytes of system caps table */
#define NV0000_CTRL_SYSTEM_CAPS_TBL_SIZE        1

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
#define NV0000_CTRL_CMD_SYSTEM_GET_CHIPSET_INFO (0x104) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS_MESSAGE_ID" */

/* maximum name string length */
#define NV0000_SYSTEM_MAX_CHIPSET_STRING_LENGTH (0x0000020)

/* invalid id */
#define NV0000_SYSTEM_CHIPSET_INVALID_ID        (0xffff)

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
#define NV0000_CTRL_SYSTEM_CHIPSET_FLAG_HAS_RESIZABLE_BAR_ISSUE_NO  (0x00000000)
#define NV0000_CTRL_SYSTEM_CHIPSET_FLAG_HAS_RESIZABLE_BAR_ISSUE_YES (0x00000001)



/*
 * NV0000_CTRL_CMD_SYSTEM_SET_MEMORY_SIZE
 *
 * This command is used to set the system memory size in pages.
 *
 *   memorySize
 *     This parameter specifies the system memory size in pages.  All values
 *     are considered legal.
 *
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV0000_CTRL_CMD_SYSTEM_SET_MEMORY_SIZE                                         (0x107) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS {
    NvU32 memorySize;
} NV0000_CTRL_SYSTEM_SET_MEMORY_SIZE_PARAMS;

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

#define NV0000_CTRL_CMD_SYSTEM_GET_CLASSLIST  (0x108) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS_MESSAGE_ID" */

/* maximum number of classes returned in classes[] array */
#define NV0000_CTRL_SYSTEM_MAX_CLASSLIST_SIZE (32)

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
#define NV0000_CTRL_CMD_SYSTEM_NOTIFY_EVENT (0x110) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS {
    NvU32  eventType;
    NvU32  eventData;
    NvBool bEventDataForced;
} NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS;

/* valid eventType values */
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_LID_STATE        (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_POWER_SOURCE     (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_DOCK_STATE       (0x00000002)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_LID        (0x00000003)
#define NV0000_CTRL_SYSTEM_EVENT_TYPE_TRUST_DOCK       (0x00000004)

/* valid eventData values */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_LID_OPEN         (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_LID_CLOSED       (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_BATTERY    (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_AC         (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_UNDOCKED         (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_DOCKED           (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_DSM    (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_DCS    (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_NVIF   (0x00000002)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_ACPI   (0x00000003)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_POLL   (0x00000004)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_COUNT  (0x5) /* finn: Evaluated from "(NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_LID_POLL + 1)" */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_DSM   (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_DCS   (0x00000001)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_NVIF  (0x00000002)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_ACPI  (0x00000003)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_POLL  (0x00000004)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_COUNT (0x5) /* finn: Evaluated from "(NV0000_CTRL_SYSTEM_EVENT_DATA_TRUST_DOCK_POLL + 1)" */

/* valid bEventDataForced values */
#define NV0000_CTRL_SYSTEM_EVENT_DATA_FORCED_FALSE     (0x00000000)
#define NV0000_CTRL_SYSTEM_EVENT_DATA_FORCED_TRUE      (0x00000001)

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
#define NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE       (0x111) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS {
    NvU32 systemType;
} NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS;

/* valid systemType values */
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_DESKTOP        (0x000000)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_GENERIC (0x000001)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_MOBILE_TOSHIBA (0x000002)
#define NV0000_CTRL_SYSTEM_GET_PLATFORM_TYPE_SOC            (0x000003)




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
#define NV0000_CTRL_CMD_SYSTEM_DEBUG_RMMSG_CTRL     (0x121) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_SIZE         512

#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_GET (0x00000000)
#define NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_CMD_SET (0x00000001)

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

#define NV0000_CTRL_SYSTEM_HWBC_INVALID_ID   (0xFFFFFFFF)

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
#define NV0000_CTRL_CMD_SYSTEM_GET_HWBC_INFO (0x124) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_MAX_HWBCS         (0x00000080)

#define NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS {
    NV0000_CTRL_SYSTEM_HWBC_INFO hwbcInfo[NV0000_CTRL_SYSTEM_MAX_HWBCS];
} NV0000_CTRL_SYSTEM_GET_HWBC_INFO_PARAMS;



/*
 * Deprecated. Please use NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2 instead.
 */
#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS          (0x127) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED must remain equal to the square of
 * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS due to Check RM parsing issues.
 * NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS is the maximum size of GPU groups
 * allowed for batched P2P caps queries provided by the RM control
 * NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX.
 */
#define NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS         32
#define NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED 1024
#define NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS        8
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INVALID_PEER 0xffffffff

/* P2P capabilities status index values */
#define NV0000_CTRL_P2P_CAPS_INDEX_READ              0
#define NV0000_CTRL_P2P_CAPS_INDEX_WRITE             1
#define NV0000_CTRL_P2P_CAPS_INDEX_NVLINK            2
#define NV0000_CTRL_P2P_CAPS_INDEX_ATOMICS           3
#define NV0000_CTRL_P2P_CAPS_INDEX_PROP              4
#define NV0000_CTRL_P2P_CAPS_INDEX_LOOPBACK          5
#define NV0000_CTRL_P2P_CAPS_INDEX_PCI               6
#define NV0000_CTRL_P2P_CAPS_INDEX_C2C               7
#define NV0000_CTRL_P2P_CAPS_INDEX_PCI_BAR1          8

#define NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE        9


#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_MESSAGE_ID (0x27U)

typedef struct NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU32 gpuCount;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NV_DECLARE_ALIGNED(NvP64 busPeerIds, 8);
} NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS;

/* valid p2pCaps values */
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED                    0:0
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED_FALSE           (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED_TRUE            (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED                     1:1
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED_FALSE            (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED_TRUE             (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED                      2:2
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED_FALSE             (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED_TRUE              (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED                    3:3
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED_FALSE           (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED_TRUE            (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED                   4:4
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED_FALSE          (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED_TRUE           (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED                  5:5
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED_FALSE         (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_LOOPBACK_SUPPORTED_TRUE          (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED                       6:6
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED_FALSE              (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_SUPPORTED_TRUE               (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED           7:7
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED_FALSE  (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_WRITES_SUPPORTED_TRUE   (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED            8:8
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED_FALSE   (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_READS_SUPPORTED_TRUE    (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED          9:9
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED_FALSE (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_ATOMICS_SUPPORTED_TRUE  (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED           10:10
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED_FALSE  (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED_TRUE   (0x00000001)


#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED                       12:12
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED_FALSE              (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED_TRUE               (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED                 13:13
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED_FALSE         (0x00000000)
#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED_TRUE          (0x00000001)

/* P2P status codes */
#define NV0000_P2P_CAPS_STATUS_OK                                        (0x00)
#define NV0000_P2P_CAPS_STATUS_CHIPSET_NOT_SUPPORTED                     (0x01)
#define NV0000_P2P_CAPS_STATUS_GPU_NOT_SUPPORTED                         (0x02)
#define NV0000_P2P_CAPS_STATUS_IOH_TOPOLOGY_NOT_SUPPORTED                (0x03)
#define NV0000_P2P_CAPS_STATUS_DISABLED_BY_REGKEY                        (0x04)
#define NV0000_P2P_CAPS_STATUS_NOT_SUPPORTED                             (0x05)

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
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */


#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2                           (0x12b) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS {
    NvU32 gpuIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU32 gpuCount;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NvU32 busPeerIds[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED];
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



#define NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX (0x13a) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_MESSAGE_ID" */

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



#define GPS_MAX_COUNTERS_PER_BLOCK                                  32
typedef struct NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS {
    NvU32 objHndl;
    NvU32 blockId;
    NvU32 nextExpectedSampleTimems;
    NvU32 countersReq;
    NvU32 countersReturned;
    NvU32 counterBlock[GPS_MAX_COUNTERS_PER_BLOCK];
} NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS;

#define NV0000_CTRL_CMD_SYSTEM_GPS_GET_PERF_SENSORS          (0x12c) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | 0x2C" */

#define NV0000_CTRL_CMD_SYSTEM_GPS_GET_EXTENDED_PERF_SENSORS (0x12e) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | 0x2E" */



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

#define NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE 256
#define NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO             (0x133) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_MESSAGE_ID" */

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

#define NV0000_CTRL_CMD_SYSTEM_GET_GPUS_POWER_STATUS (0x134) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS_MESSAGE_ID (0x34U)

typedef struct NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS {
    NvU8 gpuCount;
    NvU8 gpuBus[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
    NvU8 gpuExternalPowerStatus[NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS];
} NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS;

/* Valid gpuExternalPowerStatus values */
#define NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_CONNECTED     0
#define NV0000_CTRL_SYSTEM_GPU_EXTERNAL_POWER_STATUS_NOT_CONNECTED 1

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


#define NV0000_CTRL_CMD_SYSTEM_GET_PRIVILEGED_STATUS               (0x135) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS_MESSAGE_ID (0x35U)

typedef struct NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS {
    NvU8 privStatusFlags;
} NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS;


/* Valid privStatus values */
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_USER_FLAG     (0x00000001)
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_KERNEL_HANDLE_FLAG (0x00000002)
#define NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PRIV_HANDLE_FLAG   (0x00000004)

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

#define NV0000_CTRL_CMD_SYSTEM_GET_FABRIC_STATUS (0x136) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS_MESSAGE_ID (0x36U)

typedef struct NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS {
    NV0000_CTRL_GET_SYSTEM_FABRIC_STATUS fabricStatus;
} NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS;



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
#define NV0000_CTRL_CMD_SYSTEM_GET_RM_INSTANCE_ID (0x139) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS_MESSAGE_ID" */

/*
 * NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS
 */
#define NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 rm_instance_id, 8);
} NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS;



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
#define NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT     (0x13c) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS_MESSAGE_ID" */

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
#define NV0000_CTRL_CMD_SYSTEM_GET_CLIENT_DATABASE_INFO (0x13d) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS_MESSAGE_ID" */

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

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE 256
#define NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION_V2             (0x13e) /* finn: Evaluated from "(FINN_NV01_ROOT_SYSTEM_INTERFACE_ID << 8) | NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS_MESSAGE_ID (0x3EU)

typedef struct NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS {
    char  driverVersionBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    char  versionBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    char  titleBuffer[NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_MAX_STRING_SIZE];
    NvU32 changelistNumber;
    NvU32 officialChangelistNumber;
} NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS;

/* _ctrl0000system_h_ */
