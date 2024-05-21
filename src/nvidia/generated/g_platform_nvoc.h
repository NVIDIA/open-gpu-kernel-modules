
#ifndef _G_PLATFORM_NVOC_H_
#define _G_PLATFORM_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_platform_nvoc.h"

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

typedef struct OBJPFM *POBJPFM;

#ifndef __NVOC_CLASS_OBJPFM_TYPEDEF__
#define __NVOC_CLASS_OBJPFM_TYPEDEF__
typedef struct OBJPFM OBJPFM;
#endif /* __NVOC_CLASS_OBJPFM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPFM
#define __nvoc_class_id_OBJPFM 0xb543ae
#endif /* __nvoc_class_id_OBJPFM */



#include "nvoc/runtime.h"
#include "nvlimits.h" // NV_MAX_DEVICES
#include "ctrl/ctrl0073/ctrl0073specific.h" // NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES

#include "core/core.h"

#include "nvCpuUuid.h"
#include "platform/nbsi/nbsi_table.h"

/*!
 * Data structure representing single BlobData entry.
 */
typedef struct
{
    NvU16   identifier;
    NvU32   argument;
} PFM_BLOB_DATA_ENTRY, *PPFM_BLOB_DATA_ENTRY;

typedef struct
{
    NvU32                   entryCount;
    PFM_BLOB_DATA_ENTRY    *pEntry;
} PFM_BLOB_DATA;

typedef struct
{
    NvU32  acpiId;
    NvU32  displayId;
    NvU32  dodIndex;
} ACPI_ID_MAPPING;

/**************** Resource Manager Defines and Structures ******************\
*       Defines and structures used for the Platform object.                *
\***************************************************************************/


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PLATFORM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJPFM {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJPFM *__nvoc_pbase_OBJPFM;    // pfm

    // 9 PDB properties
    NvBool PDB_PROP_PFM_SUPPORTS_ACPI;
    NvBool PDB_PROP_PFM_IS_MOBILE;
    NvBool PDB_PROP_PFM_IS_TOSHIBA_MOBILE;
    NvBool PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT;
    NvBool PDB_PROP_PFM_ENABLE_PERF_WITHOUT_MXM;
    NvBool PDB_PROP_PFM_SKIP_DP_IRQ_HANDLE;
    NvBool PDB_PROP_PFM_MODS_USE_TWO_STAGE_RC_RECOVER;
    NvBool PDB_PROP_PFM_POSSIBLE_HIGHRES_BOOT;
    NvBool PDB_PROP_PFM_APPLE_EDP_SUPPORTED;

    // Data members
    NBSI_OBJ nbsi;
    ACPI_ID_MAPPING acpiIdMapping[32][16];
};

#ifndef __NVOC_CLASS_OBJPFM_TYPEDEF__
#define __NVOC_CLASS_OBJPFM_TYPEDEF__
typedef struct OBJPFM OBJPFM;
#endif /* __NVOC_CLASS_OBJPFM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPFM
#define __nvoc_class_id_OBJPFM 0xb543ae
#endif /* __nvoc_class_id_OBJPFM */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJPFM;

#define __staticCast_OBJPFM(pThis) \
    ((pThis)->__nvoc_pbase_OBJPFM)

#ifdef __nvoc_platform_h_disabled
#define __dynamicCast_OBJPFM(pThis) ((OBJPFM*)NULL)
#else //__nvoc_platform_h_disabled
#define __dynamicCast_OBJPFM(pThis) \
    ((OBJPFM*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJPFM)))
#endif //__nvoc_platform_h_disabled

// Property macros
#define PDB_PROP_PFM_SKIP_DP_IRQ_HANDLE_BASE_CAST
#define PDB_PROP_PFM_SKIP_DP_IRQ_HANDLE_BASE_NAME PDB_PROP_PFM_SKIP_DP_IRQ_HANDLE
#define PDB_PROP_PFM_APPLE_EDP_SUPPORTED_BASE_CAST
#define PDB_PROP_PFM_APPLE_EDP_SUPPORTED_BASE_NAME PDB_PROP_PFM_APPLE_EDP_SUPPORTED
#define PDB_PROP_PFM_IS_MOBILE_BASE_CAST
#define PDB_PROP_PFM_IS_MOBILE_BASE_NAME PDB_PROP_PFM_IS_MOBILE
#define PDB_PROP_PFM_MODS_USE_TWO_STAGE_RC_RECOVER_BASE_CAST
#define PDB_PROP_PFM_MODS_USE_TWO_STAGE_RC_RECOVER_BASE_NAME PDB_PROP_PFM_MODS_USE_TWO_STAGE_RC_RECOVER
#define PDB_PROP_PFM_ENABLE_PERF_WITHOUT_MXM_BASE_CAST
#define PDB_PROP_PFM_ENABLE_PERF_WITHOUT_MXM_BASE_NAME PDB_PROP_PFM_ENABLE_PERF_WITHOUT_MXM
#define PDB_PROP_PFM_SUPPORTS_ACPI_BASE_CAST
#define PDB_PROP_PFM_SUPPORTS_ACPI_BASE_NAME PDB_PROP_PFM_SUPPORTS_ACPI
#define PDB_PROP_PFM_IS_TOSHIBA_MOBILE_BASE_CAST
#define PDB_PROP_PFM_IS_TOSHIBA_MOBILE_BASE_NAME PDB_PROP_PFM_IS_TOSHIBA_MOBILE
#define PDB_PROP_PFM_POSSIBLE_HIGHRES_BOOT_BASE_CAST
#define PDB_PROP_PFM_POSSIBLE_HIGHRES_BOOT_BASE_NAME PDB_PROP_PFM_POSSIBLE_HIGHRES_BOOT
#define PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT_BASE_CAST
#define PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT_BASE_NAME PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT

NV_STATUS __nvoc_objCreateDynamic_OBJPFM(OBJPFM**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJPFM(OBJPFM**, Dynamic*, NvU32);
#define __objCreate_OBJPFM(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJPFM((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS pfmConstruct_IMPL(struct OBJPFM *arg_pPfm);

#define __nvoc_pfmConstruct(arg_pPfm) pfmConstruct_IMPL(arg_pPfm)
void pfmUpdateAcpiIdMapping_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5, NvU32 arg6);

#ifdef __nvoc_platform_h_disabled
static inline void pfmUpdateAcpiIdMapping(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5, NvU32 arg6) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
}
#else //__nvoc_platform_h_disabled
#define pfmUpdateAcpiIdMapping(arg1, arg2, arg3, arg4, arg5, arg6) pfmUpdateAcpiIdMapping_IMPL(arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_platform_h_disabled

NvU32 pfmFindAcpiId_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3);

#ifdef __nvoc_platform_h_disabled
static inline NvU32 pfmFindAcpiId(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
    return 0;
}
#else //__nvoc_platform_h_disabled
#define pfmFindAcpiId(arg1, arg2, arg3) pfmFindAcpiId_IMPL(arg1, arg2, arg3)
#endif //__nvoc_platform_h_disabled

NvU32 pfmFindDodIndex_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3);

#ifdef __nvoc_platform_h_disabled
static inline NvU32 pfmFindDodIndex(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
    return 0;
}
#else //__nvoc_platform_h_disabled
#define pfmFindDodIndex(arg1, arg2, arg3) pfmFindDodIndex_IMPL(arg1, arg2, arg3)
#endif //__nvoc_platform_h_disabled

NvU32 pfmFindDevMaskFromDodIndex_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3);

#ifdef __nvoc_platform_h_disabled
static inline NvU32 pfmFindDevMaskFromDodIndex(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
    return 0;
}
#else //__nvoc_platform_h_disabled
#define pfmFindDevMaskFromDodIndex(arg1, arg2, arg3) pfmFindDevMaskFromDodIndex_IMPL(arg1, arg2, arg3)
#endif //__nvoc_platform_h_disabled

NvU32 pfmFindDevMaskFromAcpiId_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3);

#ifdef __nvoc_platform_h_disabled
static inline NvU32 pfmFindDevMaskFromAcpiId(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
    return 0;
}
#else //__nvoc_platform_h_disabled
#define pfmFindDevMaskFromAcpiId(arg1, arg2, arg3) pfmFindDevMaskFromAcpiId_IMPL(arg1, arg2, arg3)
#endif //__nvoc_platform_h_disabled

void pfmUpdateDeviceAcpiId_IMPL(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_platform_h_disabled
static inline void pfmUpdateDeviceAcpiId(struct OBJPFM *arg1, OBJGPU *arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJPFM was disabled!");
}
#else //__nvoc_platform_h_disabled
#define pfmUpdateDeviceAcpiId(arg1, arg2, arg3, arg4) pfmUpdateDeviceAcpiId_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_platform_h_disabled

#undef PRIVATE_FIELD


#endif // _PLATFORM_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PLATFORM_NVOC_H_
