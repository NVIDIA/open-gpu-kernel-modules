
#ifndef _G_OBJGRIDDISPLAYLESS_NVOC_H_
#define _G_OBJGRIDDISPLAYLESS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_objgriddisplayless_nvoc.h"

#ifndef _OBJGRIDDISPLAYLESS_H_
#define _OBJGRIDDISPLAYLESS_H_

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "ctrl/ctrla083.h"

#define GRID_DISPLAYLESS_MAX_NUM_HEADS                      4


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OBJGRIDDISPLAYLESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJGRIDDISPLAYLESS;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__OBJGRIDDISPLAYLESS;


struct OBJGRIDDISPLAYLESS {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJGRIDDISPLAYLESS *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct OBJGRIDDISPLAYLESS *__nvoc_pbase_OBJGRIDDISPLAYLESS;    // griddisplayless

    // 1 PDB property
//  NvBool PDB_PROP_GRIDDISPLAYLESS_IS_MISSING inherited from OBJENGSTATE

    // Data members
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS heads;
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS maxResolution;
    NvBool displayActive[4];
    NvU64 maxPixels;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__OBJGRIDDISPLAYLESS {
    NV_STATUS (*__griddisplaylessConstructEngine__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__griddisplaylessInitMissing__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePreInitLocked__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePreInitUnlocked__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStateInitLocked__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStateInitUnlocked__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePreLoad__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStateLoad__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePostLoad__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePreUnload__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStateUnload__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__griddisplaylessStatePostUnload__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__griddisplaylessStateDestroy__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__griddisplaylessIsPresent__)(struct OBJGPU *, struct OBJGRIDDISPLAYLESS * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJGRIDDISPLAYLESS {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__OBJGRIDDISPLAYLESS vtable;
};

#ifndef __nvoc_class_id_OBJGRIDDISPLAYLESS
#define __nvoc_class_id_OBJGRIDDISPLAYLESS 0x20fd5au
typedef struct OBJGRIDDISPLAYLESS OBJGRIDDISPLAYLESS;
#endif /* __nvoc_class_id_OBJGRIDDISPLAYLESS */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGRIDDISPLAYLESS;

#define __staticCast_OBJGRIDDISPLAYLESS(pThis) \
    ((pThis)->__nvoc_pbase_OBJGRIDDISPLAYLESS)

#ifdef __nvoc_objgriddisplayless_h_disabled
#define __dynamicCast_OBJGRIDDISPLAYLESS(pThis) ((OBJGRIDDISPLAYLESS*) NULL)
#else //__nvoc_objgriddisplayless_h_disabled
#define __dynamicCast_OBJGRIDDISPLAYLESS(pThis) \
    ((OBJGRIDDISPLAYLESS*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGRIDDISPLAYLESS)))
#endif //__nvoc_objgriddisplayless_h_disabled

// Property macros
#define PDB_PROP_GRIDDISPLAYLESS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_GRIDDISPLAYLESS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS**, Dynamic*, NvU32);
#define __objCreate_OBJGRIDDISPLAYLESS(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_OBJGRIDDISPLAYLESS((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NvU32 griddisplaylessGetNumHeads_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvU32 griddisplaylessGetNumHeads(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return 0;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetNumHeads(pGriddisplayless) griddisplaylessGetNumHeads_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvU32 griddisplaylessGetMaxNumHeads_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvU32 griddisplaylessGetMaxNumHeads(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return 0;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetMaxNumHeads(pGriddisplayless) griddisplaylessGetMaxNumHeads_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvU32 griddisplaylessGetMaxHResolution_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvU32 griddisplaylessGetMaxHResolution(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return 0;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetMaxHResolution(pGriddisplayless) griddisplaylessGetMaxHResolution_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvU32 griddisplaylessGetMaxVResolution_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvU32 griddisplaylessGetMaxVResolution(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return 0;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetMaxVResolution(pGriddisplayless) griddisplaylessGetMaxVResolution_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvBool griddisplaylessIsDisplayActive_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU32 arg2);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvBool griddisplaylessIsDisplayActive(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_FALSE;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessIsDisplayActive(pGriddisplayless, arg2) griddisplaylessIsDisplayActive_IMPL(pGriddisplayless, arg2)
#endif // __nvoc_objgriddisplayless_h_disabled

NvU64 griddisplaylessGetMaxPixels_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvU64 griddisplaylessGetMaxPixels(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return 0;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetMaxPixels(pGriddisplayless) griddisplaylessGetMaxPixels_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvBool griddisplaylessVirtualDisplayIsActive_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvBool griddisplaylessVirtualDisplayIsActive(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_FALSE;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessVirtualDisplayIsActive(pGriddisplayless) griddisplaylessVirtualDisplayIsActive_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NvBool griddisplaylessVirtualDisplayIsConnected_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NvBool griddisplaylessVirtualDisplayIsConnected(struct OBJGRIDDISPLAYLESS *pGriddisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_FALSE;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessVirtualDisplayIsConnected(pGriddisplayless) griddisplaylessVirtualDisplayIsConnected_IMPL(pGriddisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

NV_STATUS griddisplaylessGetDefaultEDID_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU8 connectorType, NvU32 *pEdidSize, NvU8 *pEdidBuffer);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NV_STATUS griddisplaylessGetDefaultEDID(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU8 connectorType, NvU32 *pEdidSize, NvU8 *pEdidBuffer) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessGetDefaultEDID(pGriddisplayless, connectorType, pEdidSize, pEdidBuffer) griddisplaylessGetDefaultEDID_IMPL(pGriddisplayless, connectorType, pEdidSize, pEdidBuffer)
#endif // __nvoc_objgriddisplayless_h_disabled

NV_STATUS griddisplaylessUpdateHeadNumInfo_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *arg2);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NV_STATUS griddisplaylessUpdateHeadNumInfo(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessUpdateHeadNumInfo(pGriddisplayless, arg2) griddisplaylessUpdateHeadNumInfo_IMPL(pGriddisplayless, arg2)
#endif // __nvoc_objgriddisplayless_h_disabled

NV_STATUS griddisplaylessUpdateMaxResInfo_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *arg2);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NV_STATUS griddisplaylessUpdateMaxResInfo(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessUpdateMaxResInfo(pGriddisplayless, arg2) griddisplaylessUpdateMaxResInfo_IMPL(pGriddisplayless, arg2)
#endif // __nvoc_objgriddisplayless_h_disabled

NV_STATUS griddisplaylessUpdateDisplayActive_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU32 arg2, NvBool arg3);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NV_STATUS griddisplaylessUpdateDisplayActive(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU32 arg2, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessUpdateDisplayActive(pGriddisplayless, arg2, arg3) griddisplaylessUpdateDisplayActive_IMPL(pGriddisplayless, arg2, arg3)
#endif // __nvoc_objgriddisplayless_h_disabled

NV_STATUS griddisplaylessUpdateMaxPixels_IMPL(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU64 arg2);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline NV_STATUS griddisplaylessUpdateMaxPixels(struct OBJGRIDDISPLAYLESS *pGriddisplayless, NvU64 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessUpdateMaxPixels(pGriddisplayless, arg2) griddisplaylessUpdateMaxPixels_IMPL(pGriddisplayless, arg2)
#endif // __nvoc_objgriddisplayless_h_disabled

void griddisplaylessSetNumHeads_IMPL(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline void griddisplaylessSetNumHeads(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessSetNumHeads(pGpu, pGridDisplayless) griddisplaylessSetNumHeads_IMPL(pGpu, pGridDisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

void griddisplaylessSetMaxResolution_IMPL(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline void griddisplaylessSetMaxResolution(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessSetMaxResolution(pGpu, pGridDisplayless) griddisplaylessSetMaxResolution_IMPL(pGpu, pGridDisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled

void griddisplaylessSetMaxPixels_IMPL(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless);
#ifdef __nvoc_objgriddisplayless_h_disabled
static inline void griddisplaylessSetMaxPixels(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGridDisplayless) {
    NV_ASSERT_FAILED_PRECOMP("OBJGRIDDISPLAYLESS was disabled!");
}
#else // __nvoc_objgriddisplayless_h_disabled
#define griddisplaylessSetMaxPixels(pGpu, pGridDisplayless) griddisplaylessSetMaxPixels_IMPL(pGpu, pGridDisplayless)
#endif // __nvoc_objgriddisplayless_h_disabled


// Wrapper macros for halified functions
#define griddisplaylessConstructEngine_FNPTR(pGriddisplayless) pGriddisplayless->__nvoc_metadata_ptr->vtable.__griddisplaylessConstructEngine__
#define griddisplaylessConstructEngine(pGpu, pGriddisplayless, arg3) griddisplaylessConstructEngine_DISPATCH(pGpu, pGriddisplayless, arg3)
#define griddisplaylessInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define griddisplaylessInitMissing(pGpu, pEngstate) griddisplaylessInitMissing_DISPATCH(pGpu, pEngstate)
#define griddisplaylessStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define griddisplaylessStatePreInitLocked(pGpu, pEngstate) griddisplaylessStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define griddisplaylessStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define griddisplaylessStatePreInitUnlocked(pGpu, pEngstate) griddisplaylessStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define griddisplaylessStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define griddisplaylessStateInitLocked(pGpu, pEngstate) griddisplaylessStateInitLocked_DISPATCH(pGpu, pEngstate)
#define griddisplaylessStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define griddisplaylessStateInitUnlocked(pGpu, pEngstate) griddisplaylessStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define griddisplaylessStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define griddisplaylessStatePreLoad(pGpu, pEngstate, arg3) griddisplaylessStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define griddisplaylessStateLoad(pGpu, pEngstate, arg3) griddisplaylessStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define griddisplaylessStatePostLoad(pGpu, pEngstate, arg3) griddisplaylessStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define griddisplaylessStatePreUnload(pGpu, pEngstate, arg3) griddisplaylessStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define griddisplaylessStateUnload(pGpu, pEngstate, arg3) griddisplaylessStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define griddisplaylessStatePostUnload(pGpu, pEngstate, arg3) griddisplaylessStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define griddisplaylessStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define griddisplaylessStateDestroy(pGpu, pEngstate) griddisplaylessStateDestroy_DISPATCH(pGpu, pEngstate)
#define griddisplaylessIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define griddisplaylessIsPresent(pGpu, pEngstate) griddisplaylessIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS griddisplaylessConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGriddisplayless, ENGDESCRIPTOR arg3) {
    return pGriddisplayless->__nvoc_metadata_ptr->vtable.__griddisplaylessConstructEngine__(pGpu, pGriddisplayless, arg3);
}

static inline void griddisplaylessInitMissing_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS griddisplaylessStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS griddisplaylessStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS griddisplaylessStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS griddisplaylessStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS griddisplaylessStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS griddisplaylessStateLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS griddisplaylessStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS griddisplaylessStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS griddisplaylessStateUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS griddisplaylessStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void griddisplaylessStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessStateDestroy__(pGpu, pEngstate);
}

static inline NvBool griddisplaylessIsPresent_DISPATCH(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__griddisplaylessIsPresent__(pGpu, pEngstate);
}

// Virtual method declarations and/or inline definitions
NV_STATUS griddisplaylessConstructEngine_IMPL(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pGriddisplayless, ENGDESCRIPTOR arg3);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _OBJGRIDDISPLAYLESS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJGRIDDISPLAYLESS_NVOC_H_
