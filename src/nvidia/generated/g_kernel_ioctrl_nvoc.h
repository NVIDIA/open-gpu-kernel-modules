
#ifndef _G_KERNEL_IOCTRL_NVOC_H_
#define _G_KERNEL_IOCTRL_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_ioctrl_nvoc.h"

#ifndef _KERNEL_IOCTRL_H_
#define _KERNEL_IOCTRL_H_

#include "core/core.h"
#include "gpu/eng_state.h"
#include "lib/ref_count.h"
#include "gpu/gpu.h"
#include "nvCpuUuid.h"

#if defined(INCLUDE_NVLINK_LIB)
#include "nvlink.h"
#include "nvlink_export.h"
#endif

/*!
 * KernelIoctrl is a logical abstraction of the GPU Ioctrl Engine. The
 * Public API of the Ioctrl Engine is exposed through this object, and
 * any interfaces which do not manage the underlying Ioctrl hardware
 * can be managed by this object.
 */

 // Link Conversion Macros
#define KIOCTRL_LINK_GLOBAL_TO_LOCAL_MASK(mask)  (mask  >> pKernelIoctrl->localGlobalLinkOffset)


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelIoctrl;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelIoctrl;


struct KernelIoctrl {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelIoctrl *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelIoctrl *__nvoc_pbase_KernelIoctrl;    // kioctrl

    // Vtable with 2 per-object function pointers
    NvBool (*__kioctrlGetMinionEnableDefault__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kioctrlMinionConstruct__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // halified (2 hals) body

    // 4 PDB properties
    NvBool PDB_PROP_KIOCTRL_MINION_AVAILABLE;
    NvBool PDB_PROP_KIOCTRL_MINION_FORCE_BOOT;
    NvBool PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS;

    // Data members
    NvU32 PRIVATE_FIELD(PublicId);
    NvU32 PRIVATE_FIELD(localDiscoveredLinks);
    NvU32 PRIVATE_FIELD(localGlobalLinkOffset);
    NvU32 PRIVATE_FIELD(ipVerIoctrl);
    NvU32 PRIVATE_FIELD(ipVerMinion);
    NvU32 PRIVATE_FIELD(ioctrlDiscoverySize);
    NvU8 PRIVATE_FIELD(numDevices);
};


struct KernelIoctrl_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelIoctrl *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelIoctrl *__nvoc_pbase_KernelIoctrl;    // kioctrl

    // Vtable with 2 per-object function pointers
    NvBool (*__kioctrlGetMinionEnableDefault__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kioctrlMinionConstruct__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // halified (2 hals) body

    // 4 PDB properties
    NvBool PDB_PROP_KIOCTRL_MINION_AVAILABLE;
    NvBool PDB_PROP_KIOCTRL_MINION_FORCE_BOOT;
    NvBool PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS;

    // Data members
    NvU32 PublicId;
    NvU32 localDiscoveredLinks;
    NvU32 localGlobalLinkOffset;
    NvU32 ipVerIoctrl;
    NvU32 ipVerMinion;
    NvU32 ioctrlDiscoverySize;
    NvU8 numDevices;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelIoctrl {
    NV_STATUS (*__kioctrlConstructEngine__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kioctrlInitMissing__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePreInitLocked__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePreInitUnlocked__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStateInitLocked__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStateInitUnlocked__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePreLoad__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStateLoad__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePostLoad__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePreUnload__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStateUnload__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kioctrlStatePostUnload__)(struct OBJGPU *, struct KernelIoctrl * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kioctrlStateDestroy__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kioctrlIsPresent__)(struct OBJGPU *, struct KernelIoctrl * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelIoctrl {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelIoctrl vtable;
};

#ifndef __NVOC_CLASS_KernelIoctrl_TYPEDEF__
#define __NVOC_CLASS_KernelIoctrl_TYPEDEF__
typedef struct KernelIoctrl KernelIoctrl;
#endif /* __NVOC_CLASS_KernelIoctrl_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelIoctrl
#define __nvoc_class_id_KernelIoctrl 0x880c7d
#endif /* __nvoc_class_id_KernelIoctrl */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl;

#define __staticCast_KernelIoctrl(pThis) \
    ((pThis)->__nvoc_pbase_KernelIoctrl)

#ifdef __nvoc_kernel_ioctrl_h_disabled
#define __dynamicCast_KernelIoctrl(pThis) ((KernelIoctrl*) NULL)
#else //__nvoc_kernel_ioctrl_h_disabled
#define __dynamicCast_KernelIoctrl(pThis) \
    ((KernelIoctrl*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelIoctrl)))
#endif //__nvoc_kernel_ioctrl_h_disabled

// Property macros
#define PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS_BASE_CAST
#define PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS_BASE_NAME PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS
#define PDB_PROP_KIOCTRL_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KIOCTRL_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KIOCTRL_MINION_AVAILABLE_BASE_CAST
#define PDB_PROP_KIOCTRL_MINION_AVAILABLE_BASE_NAME PDB_PROP_KIOCTRL_MINION_AVAILABLE
#define PDB_PROP_KIOCTRL_MINION_FORCE_BOOT_BASE_CAST
#define PDB_PROP_KIOCTRL_MINION_FORCE_BOOT_BASE_NAME PDB_PROP_KIOCTRL_MINION_FORCE_BOOT

NV_STATUS __nvoc_objCreateDynamic_KernelIoctrl(KernelIoctrl**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelIoctrl(KernelIoctrl**, Dynamic*, NvU32);
#define __objCreate_KernelIoctrl(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelIoctrl((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kioctrlConstructEngine_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kioctrlConstructEngine__
#define kioctrlConstructEngine(arg1, arg_this, arg3) kioctrlConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define kioctrlGetMinionEnableDefault_FNPTR(pKernelIoctrl) pKernelIoctrl->__kioctrlGetMinionEnableDefault__
#define kioctrlGetMinionEnableDefault(pGpu, pKernelIoctrl) kioctrlGetMinionEnableDefault_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlGetMinionEnableDefault_HAL(pGpu, pKernelIoctrl) kioctrlGetMinionEnableDefault_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlMinionConstruct_FNPTR(pKernelIoctrl) pKernelIoctrl->__kioctrlMinionConstruct__
#define kioctrlMinionConstruct(pGpu, pKernelIoctrl) kioctrlMinionConstruct_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlMinionConstruct_HAL(pGpu, pKernelIoctrl) kioctrlMinionConstruct_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kioctrlInitMissing(pGpu, pEngstate) kioctrlInitMissing_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kioctrlStatePreInitLocked(pGpu, pEngstate) kioctrlStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kioctrlStatePreInitUnlocked(pGpu, pEngstate) kioctrlStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kioctrlStateInitLocked(pGpu, pEngstate) kioctrlStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kioctrlStateInitUnlocked(pGpu, pEngstate) kioctrlStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kioctrlStatePreLoad(pGpu, pEngstate, arg3) kioctrlStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kioctrlStateLoad(pGpu, pEngstate, arg3) kioctrlStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kioctrlStatePostLoad(pGpu, pEngstate, arg3) kioctrlStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kioctrlStatePreUnload(pGpu, pEngstate, arg3) kioctrlStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kioctrlStateUnload(pGpu, pEngstate, arg3) kioctrlStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kioctrlStatePostUnload(pGpu, pEngstate, arg3) kioctrlStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kioctrlStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kioctrlStateDestroy(pGpu, pEngstate) kioctrlStateDestroy_DISPATCH(pGpu, pEngstate)
#define kioctrlIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kioctrlIsPresent(pGpu, pEngstate) kioctrlIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kioctrlConstructEngine_DISPATCH(struct OBJGPU *arg1, struct KernelIoctrl *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kioctrlConstructEngine__(arg1, arg_this, arg3);
}

static inline NvBool kioctrlGetMinionEnableDefault_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return pKernelIoctrl->__kioctrlGetMinionEnableDefault__(pGpu, pKernelIoctrl);
}

static inline NV_STATUS kioctrlMinionConstruct_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return pKernelIoctrl->__kioctrlMinionConstruct__(pGpu, pKernelIoctrl);
}

static inline void kioctrlInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kioctrlStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kioctrlStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kioctrlStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kioctrlStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kioctrlStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kioctrlStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kioctrlIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kioctrlIsPresent__(pGpu, pEngstate);
}

NV_STATUS kioctrlConstructEngine_IMPL(struct OBJGPU *arg1, struct KernelIoctrl *arg2, NvU32 arg3);

static inline NvBool kioctrlGetMinionEnableDefault_d69453(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return NV_FALSE;
}

NvBool kioctrlGetMinionEnableDefault_GV100(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl);

static inline NV_STATUS kioctrlMinionConstruct_ac1694(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return NV_OK;
}

NV_STATUS kioctrlMinionConstruct_GV100(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl);

static inline NvU32 kioctrlGetLocalDiscoveredLinks(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    struct KernelIoctrl_PRIVATE *pKernelIoctrl_PRIVATE = (struct KernelIoctrl_PRIVATE *)pKernelIoctrl;
    return pKernelIoctrl_PRIVATE->localDiscoveredLinks;
}

static inline NvU32 kioctrlGetGlobalToLocalMask(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl, NvU32 mask) {
    struct KernelIoctrl_PRIVATE *pKernelIoctrl_PRIVATE = (struct KernelIoctrl_PRIVATE *)pKernelIoctrl;
    return (mask >> pKernelIoctrl_PRIVATE->localGlobalLinkOffset);
}

static inline NvU32 kioctrlGetPublicId(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    struct KernelIoctrl_PRIVATE *pKernelIoctrl_PRIVATE = (struct KernelIoctrl_PRIVATE *)pKernelIoctrl;
    return pKernelIoctrl_PRIVATE->PublicId;
}

void kioctrlDestructEngine_IMPL(struct KernelIoctrl *arg1);

#ifdef __nvoc_kernel_ioctrl_h_disabled
static inline void kioctrlDestructEngine(struct KernelIoctrl *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelIoctrl was disabled!");
}
#else //__nvoc_kernel_ioctrl_h_disabled
#define kioctrlDestructEngine(arg1) kioctrlDestructEngine_IMPL(arg1)
#endif //__nvoc_kernel_ioctrl_h_disabled

#undef PRIVATE_FIELD


#endif // _KERNEL_IOCTRL_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_IOCTRL_NVOC_H_
