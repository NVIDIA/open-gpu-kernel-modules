
#ifndef _G_TDISP_MGR_NVOC_H_
#define _G_TDISP_MGR_NVOC_H_

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

#pragma once
#include "g_tdisp_mgr_nvoc.h"

#ifndef TDISP_MGR_H
#define TDISP_MGR_H
#include "gpu/gpu.h"
#include "gpu/eng_state.h"

#define TDISP_BAR_INDEX_REGISTER_SPACE  (0)

#define TDISP_BAR_INDEX_C2C_COHERENT    4

/****************************************************************************\
 *                                                                           *
 *      TDISP_MGR module header.                    *
 *                                                                           *
 ****************************************************************************/


/* ------------------------- Macros and Defines ----------------------------- */


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_TDISP_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TdispMgr;
struct NVOC_METADATA__OBJENGSTATE;


struct TdispMgr {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TdispMgr *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct TdispMgr *__nvoc_pbase_TdispMgr;    // tdispmgr

    // Vtable with 3 per-object function pointers
    NV_STATUS (*__tdispmgrTriggerTdispUnload__)(struct OBJGPU *, struct TdispMgr * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__tdispmgrProceedTdispUnload__)(struct OBJGPU *, struct TdispMgr * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__tdispmgrWaitForTdispUnload__)(struct OBJGPU *, struct TdispMgr * /*this*/);  // halified (2 hals) body

    // 6 PDB properties
    NvBool PDB_PROP_TDISPMGR_TDISP_SUPPORTED;
    NvBool PDB_PROP_TDISPMGR_TDISP_ENABLED;
    NvBool PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED;
    NvBool PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED;
    NvBool PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED;
//  NvBool PDB_PROP_TDISPMGR_IS_MISSING inherited from OBJENGSTATE

    // Data members
    NvU32 gspProxyRegkeys;
    NvBool bRunState;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TdispMgr {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
};

#ifndef __nvoc_class_id_TdispMgr
#define __nvoc_class_id_TdispMgr 0x78baa3u
typedef struct TdispMgr TdispMgr;
#endif /* __nvoc_class_id_TdispMgr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TdispMgr;

#define __staticCast_TdispMgr(pThis) \
    ((pThis)->__nvoc_pbase_TdispMgr)

#ifdef __nvoc_tdisp_mgr_h_disabled
#define __dynamicCast_TdispMgr(pThis) ((TdispMgr*) NULL)
#else //__nvoc_tdisp_mgr_h_disabled
#define __dynamicCast_TdispMgr(pThis) \
    ((TdispMgr*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TdispMgr)))
#endif //__nvoc_tdisp_mgr_h_disabled

// Property macros
#define PDB_PROP_TDISPMGR_TDISP_SUPPORTED_BASE_CAST
#define PDB_PROP_TDISPMGR_TDISP_SUPPORTED_BASE_NAME PDB_PROP_TDISPMGR_TDISP_SUPPORTED
#define PDB_PROP_TDISPMGR_TDISP_ENABLED_BASE_CAST
#define PDB_PROP_TDISPMGR_TDISP_ENABLED_BASE_NAME PDB_PROP_TDISPMGR_TDISP_ENABLED
#define PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED_BASE_CAST
#define PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED_BASE_NAME PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED
#define PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED_BASE_CAST
#define PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED_BASE_NAME PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED
#define PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED_BASE_CAST
#define PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED_BASE_NAME PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED
#define PDB_PROP_TDISPMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_TDISPMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_TdispMgr(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TdispMgr(TdispMgr**, Dynamic*, NvU32);
#define __objCreate_TdispMgr(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_TdispMgr((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NvBool tdispmgrIsTdispIdeTrafficDisabled_IMPL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);
#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsTdispIdeTrafficDisabled(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsTdispIdeTrafficDisabled(pGpu, pTdispMgr) tdispmgrIsTdispIdeTrafficDisabled_IMPL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

NvBool tdispmgrIsTdispBar1ValidationSkipped_IMPL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);
#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsTdispBar1ValidationSkipped(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsTdispBar1ValidationSkipped(pGpu, pTdispMgr) tdispmgrIsTdispBar1ValidationSkipped_IMPL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

NvBool tdispmgrIsTdispQualModeEnabled_IMPL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);
#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsTdispQualModeEnabled(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsTdispQualModeEnabled(pGpu, pTdispMgr) tdispmgrIsTdispQualModeEnabled_IMPL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NV_STATUS tdispmgrMnocMctpConfiguration(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrMnocMctpConfiguration(pGpu, pTdispMgr) tdispmgrMnocMctpConfiguration_f4f420(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsGpuTrusted(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsGpuTrusted(pGpu, pTdispMgr) tdispmgrIsGpuTrusted_KERNEL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsTsmFspOnlyBoot(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsTsmFspOnlyBoot(pGpu, pTdispMgr) tdispmgrIsTsmFspOnlyBoot_KERNEL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NvBool tdispmgrIsTsmGspRmBoot(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrIsTsmGspRmBoot(pGpu, pTdispMgr) tdispmgrIsTsmGspRmBoot_KERNEL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled

#ifdef __nvoc_tdisp_mgr_h_disabled
static inline NV_STATUS tdispmgrVerifyDeviceCert(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    NV_ASSERT_FAILED_PRECOMP("TdispMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_tdisp_mgr_h_disabled
#define tdispmgrVerifyDeviceCert(pGpu, pTdispMgr) tdispmgrVerifyDeviceCert_KERNEL(pGpu, pTdispMgr)
#endif // __nvoc_tdisp_mgr_h_disabled


// Wrapper macros for halified functions
#define tdispmgrConstructEngine_FNPTR(pTdispMgr) pTdispMgr->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define tdispmgrConstructEngine(pGpu, pTdispMgr, engDesc) tdispmgrConstructEngine_DISPATCH(pGpu, pTdispMgr, engDesc)
#define tdispmgrMnocMctpConfiguration_HAL(pGpu, pTdispMgr) tdispmgrMnocMctpConfiguration(pGpu, pTdispMgr)
#define tdispmgrIsGpuTrusted_HAL(pGpu, pTdispMgr) tdispmgrIsGpuTrusted(pGpu, pTdispMgr)
#define tdispmgrIsTsmFspOnlyBoot_HAL(pGpu, pTdispMgr) tdispmgrIsTsmFspOnlyBoot(pGpu, pTdispMgr)
#define tdispmgrIsTsmGspRmBoot_HAL(pGpu, pTdispMgr) tdispmgrIsTsmGspRmBoot(pGpu, pTdispMgr)
#define tdispmgrTriggerTdispUnload_FNPTR(pTdispMgr) pTdispMgr->__tdispmgrTriggerTdispUnload__
#define tdispmgrTriggerTdispUnload(pGpu, pTdispMgr) tdispmgrTriggerTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrTriggerTdispUnload_HAL(pGpu, pTdispMgr) tdispmgrTriggerTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrProceedTdispUnload_FNPTR(pTdispMgr) pTdispMgr->__tdispmgrProceedTdispUnload__
#define tdispmgrProceedTdispUnload(pGpu, pTdispMgr) tdispmgrProceedTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrProceedTdispUnload_HAL(pGpu, pTdispMgr) tdispmgrProceedTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrWaitForTdispUnload_FNPTR(pTdispMgr) pTdispMgr->__tdispmgrWaitForTdispUnload__
#define tdispmgrWaitForTdispUnload(pGpu, pTdispMgr) tdispmgrWaitForTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrWaitForTdispUnload_HAL(pGpu, pTdispMgr) tdispmgrWaitForTdispUnload_DISPATCH(pGpu, pTdispMgr)
#define tdispmgrVerifyDeviceCert_HAL(pGpu, pTdispMgr) tdispmgrVerifyDeviceCert(pGpu, pTdispMgr)
#define tdispmgrConstructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__
#define tdispmgrConstructApertures(pGpu, pEngstate) tdispmgrConstructApertures_DISPATCH(pGpu, pEngstate)
#define tdispmgrDestructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__
#define tdispmgrDestructApertures(pGpu, pEngstate) tdispmgrDestructApertures_DISPATCH(pGpu, pEngstate)
#define tdispmgrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define tdispmgrInitMissing(pGpu, pEngstate) tdispmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define tdispmgrStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define tdispmgrStatePreInitLocked(pGpu, pEngstate) tdispmgrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define tdispmgrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define tdispmgrStatePreInitUnlocked(pGpu, pEngstate) tdispmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define tdispmgrStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define tdispmgrStateInitLocked(pGpu, pEngstate) tdispmgrStateInitLocked_DISPATCH(pGpu, pEngstate)
#define tdispmgrStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define tdispmgrStateInitUnlocked(pGpu, pEngstate) tdispmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define tdispmgrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define tdispmgrStatePreLoad(pGpu, pEngstate, arg3) tdispmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define tdispmgrStateLoad(pGpu, pEngstate, arg3) tdispmgrStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define tdispmgrStatePostLoad(pGpu, pEngstate, arg3) tdispmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define tdispmgrStatePreUnload(pGpu, pEngstate, arg3) tdispmgrStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define tdispmgrStateUnload(pGpu, pEngstate, arg3) tdispmgrStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define tdispmgrStatePostUnload(pGpu, pEngstate, arg3) tdispmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define tdispmgrStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define tdispmgrStateDestroy(pGpu, pEngstate) tdispmgrStateDestroy_DISPATCH(pGpu, pEngstate)
#define tdispmgrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define tdispmgrIsPresent(pGpu, pEngstate) tdispmgrIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS tdispmgrConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr, ENGDESCRIPTOR engDesc) {
    return pTdispMgr->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(pGpu, staticCast(pTdispMgr, OBJENGSTATE), engDesc);
}

static inline NV_STATUS tdispmgrTriggerTdispUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    return pTdispMgr->__tdispmgrTriggerTdispUnload__(pGpu, pTdispMgr);
}

static inline NV_STATUS tdispmgrProceedTdispUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    return pTdispMgr->__tdispmgrProceedTdispUnload__(pGpu, pTdispMgr);
}

static inline NV_STATUS tdispmgrWaitForTdispUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr) {
    return pTdispMgr->__tdispmgrWaitForTdispUnload__(pGpu, pTdispMgr);
}

static inline NV_STATUS tdispmgrConstructApertures_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void tdispmgrDestructApertures_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void tdispmgrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS tdispmgrStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS tdispmgrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS tdispmgrStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS tdispmgrStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS tdispmgrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS tdispmgrStateLoad_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS tdispmgrStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS tdispmgrStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS tdispmgrStateUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS tdispmgrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline void tdispmgrStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NvBool tdispmgrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct TdispMgr *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

// Virtual method declarations and/or inline definitions
NV_STATUS tdispmgrConstructEngine_IMPL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr, ENGDESCRIPTOR engDesc);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS tdispmgrMnocMctpConfiguration_GB100(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NvBool tdispmgrIsGpuTrusted_KERNEL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NvBool tdispmgrIsTsmFspOnlyBoot_KERNEL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NvBool tdispmgrIsTsmGspRmBoot_KERNEL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NV_STATUS tdispmgrTriggerTdispUnload_GR100(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NV_STATUS tdispmgrProceedTdispUnload_GR100(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NV_STATUS tdispmgrWaitForTdispUnload_GR100(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

NV_STATUS tdispmgrVerifyDeviceCert_KERNEL(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr);

// Inline HAL method definitions
static inline NV_STATUS tdispmgrMnocMctpConfiguration_f4f420(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr){
    __builtin_unreachable();
}

static inline NV_STATUS tdispmgrTriggerTdispUnload_395e98(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS tdispmgrProceedTdispUnload_395e98(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS tdispmgrWaitForTdispUnload_395e98(struct OBJGPU *pGpu, struct TdispMgr *pTdispMgr){
    return NV_ERR_NOT_SUPPORTED;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // TDISP_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_TDISP_MGR_NVOC_H_
