#ifndef _G_KERNEL_IOCTRL_NVOC_H_
#define _G_KERNEL_IOCTRL_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelIoctrl {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelIoctrl *__nvoc_pbase_KernelIoctrl;
    NV_STATUS (*__kioctrlConstructEngine__)(struct OBJGPU *, struct KernelIoctrl *, NvU32);
    NvBool (*__kioctrlGetMinionEnableDefault__)(struct OBJGPU *, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlMinionConstruct__)(struct OBJGPU *, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStateLoad__)(POBJGPU, struct KernelIoctrl *, NvU32);
    NV_STATUS (*__kioctrlStateUnload__)(POBJGPU, struct KernelIoctrl *, NvU32);
    NV_STATUS (*__kioctrlStateInitLocked__)(POBJGPU, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStatePreLoad__)(POBJGPU, struct KernelIoctrl *, NvU32);
    NV_STATUS (*__kioctrlStatePostUnload__)(POBJGPU, struct KernelIoctrl *, NvU32);
    void (*__kioctrlStateDestroy__)(POBJGPU, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStatePreUnload__)(POBJGPU, struct KernelIoctrl *, NvU32);
    NV_STATUS (*__kioctrlStateInitUnlocked__)(POBJGPU, struct KernelIoctrl *);
    void (*__kioctrlInitMissing__)(POBJGPU, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStatePreInitLocked__)(POBJGPU, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStatePreInitUnlocked__)(POBJGPU, struct KernelIoctrl *);
    NV_STATUS (*__kioctrlStatePostLoad__)(POBJGPU, struct KernelIoctrl *, NvU32);
    NvBool (*__kioctrlIsPresent__)(POBJGPU, struct KernelIoctrl *);
    NvBool PDB_PROP_KIOCTRL_MINION_AVAILABLE;
    NvBool PDB_PROP_KIOCTRL_MINION_FORCE_BOOT;
    NvBool PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS;
    NvU32 PRIVATE_FIELD(PublicId);
    NvU32 PRIVATE_FIELD(localDiscoveredLinks);
    NvU32 PRIVATE_FIELD(localGlobalLinkOffset);
    NvU32 PRIVATE_FIELD(ipVerIoctrl);
    NvU32 PRIVATE_FIELD(ipVerMinion);
    NvU32 PRIVATE_FIELD(ioctrlDiscoverySize);
    NvU8 PRIVATE_FIELD(numDevices);
};

#ifndef __NVOC_CLASS_KernelIoctrl_TYPEDEF__
#define __NVOC_CLASS_KernelIoctrl_TYPEDEF__
typedef struct KernelIoctrl KernelIoctrl;
#endif /* __NVOC_CLASS_KernelIoctrl_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelIoctrl
#define __nvoc_class_id_KernelIoctrl 0x880c7d
#endif /* __nvoc_class_id_KernelIoctrl */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl;

#define __staticCast_KernelIoctrl(pThis) \
    ((pThis)->__nvoc_pbase_KernelIoctrl)

#ifdef __nvoc_kernel_ioctrl_h_disabled
#define __dynamicCast_KernelIoctrl(pThis) ((KernelIoctrl*)NULL)
#else //__nvoc_kernel_ioctrl_h_disabled
#define __dynamicCast_KernelIoctrl(pThis) \
    ((KernelIoctrl*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelIoctrl)))
#endif //__nvoc_kernel_ioctrl_h_disabled

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

#define kioctrlConstructEngine(arg0, arg1, arg2) kioctrlConstructEngine_DISPATCH(arg0, arg1, arg2)
#define kioctrlGetMinionEnableDefault(pGpu, pKernelIoctrl) kioctrlGetMinionEnableDefault_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlGetMinionEnableDefault_HAL(pGpu, pKernelIoctrl) kioctrlGetMinionEnableDefault_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlMinionConstruct(pGpu, pKernelIoctrl) kioctrlMinionConstruct_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlMinionConstruct_HAL(pGpu, pKernelIoctrl) kioctrlMinionConstruct_DISPATCH(pGpu, pKernelIoctrl)
#define kioctrlStateLoad(pGpu, pEngstate, arg0) kioctrlStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlStateUnload(pGpu, pEngstate, arg0) kioctrlStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlStateInitLocked(pGpu, pEngstate) kioctrlStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreLoad(pGpu, pEngstate, arg0) kioctrlStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlStatePostUnload(pGpu, pEngstate, arg0) kioctrlStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlStateDestroy(pGpu, pEngstate) kioctrlStateDestroy_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreUnload(pGpu, pEngstate, arg0) kioctrlStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlStateInitUnlocked(pGpu, pEngstate) kioctrlStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kioctrlInitMissing(pGpu, pEngstate) kioctrlInitMissing_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreInitLocked(pGpu, pEngstate) kioctrlStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePreInitUnlocked(pGpu, pEngstate) kioctrlStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kioctrlStatePostLoad(pGpu, pEngstate, arg0) kioctrlStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kioctrlIsPresent(pGpu, pEngstate) kioctrlIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kioctrlConstructEngine_IMPL(struct OBJGPU *arg0, struct KernelIoctrl *arg1, NvU32 arg2);

static inline NV_STATUS kioctrlConstructEngine_DISPATCH(struct OBJGPU *arg0, struct KernelIoctrl *arg1, NvU32 arg2) {
    return arg1->__kioctrlConstructEngine__(arg0, arg1, arg2);
}

static inline NvBool kioctrlGetMinionEnableDefault_bf6dfa(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return ((NvBool)(0 != 0));
}

NvBool kioctrlGetMinionEnableDefault_GV100(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl);

static inline NvBool kioctrlGetMinionEnableDefault_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return pKernelIoctrl->__kioctrlGetMinionEnableDefault__(pGpu, pKernelIoctrl);
}

static inline NV_STATUS kioctrlMinionConstruct_ac1694(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return NV_OK;
}

NV_STATUS kioctrlMinionConstruct_GV100(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl);

static inline NV_STATUS kioctrlMinionConstruct_DISPATCH(struct OBJGPU *pGpu, struct KernelIoctrl *pKernelIoctrl) {
    return pKernelIoctrl->__kioctrlMinionConstruct__(pGpu, pKernelIoctrl);
}

static inline NV_STATUS kioctrlStateLoad_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kioctrlStateUnload_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kioctrlStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__kioctrlStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kioctrlStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kioctrlStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    pEngstate->__kioctrlStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kioctrlStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__kioctrlStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kioctrlInitMissing_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    pEngstate->__kioctrlInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__kioctrlStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__kioctrlStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kioctrlStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return pEngstate->__kioctrlStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kioctrlIsPresent_DISPATCH(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return pEngstate->__kioctrlIsPresent__(pGpu, pEngstate);
}

void kioctrlDestructEngine_IMPL(struct KernelIoctrl *arg0);

#ifdef __nvoc_kernel_ioctrl_h_disabled
static inline void kioctrlDestructEngine(struct KernelIoctrl *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelIoctrl was disabled!");
}
#else //__nvoc_kernel_ioctrl_h_disabled
#define kioctrlDestructEngine(arg0) kioctrlDestructEngine_IMPL(arg0)
#endif //__nvoc_kernel_ioctrl_h_disabled

#undef PRIVATE_FIELD


// Link Conversion Macros
#define KIOCTRL_LINK_GLOBAL_TO_LOCAL_MASK(mask)  (mask  >> pKernelIoctrl->localGlobalLinkOffset)

#endif // _KERNEL_IOCTRL_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_IOCTRL_NVOC_H_
