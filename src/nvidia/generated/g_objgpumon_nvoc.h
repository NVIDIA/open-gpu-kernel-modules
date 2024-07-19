
#ifndef _G_OBJGPUMON_NVOC_H_
#define _G_OBJGPUMON_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_objgpumon_nvoc.h"

#ifndef OBJGPUMON_H
#define OBJGPUMON_H

/****************** Resource Manager Defines and Structures *****************\
*                                                                            *
*       Defines and structures used for the Gpumon Object.                   *
*                                                                            *
\****************************************************************************/

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

#include "ctrl/ctrl2080/ctrl2080perf.h"



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OBJGPUMON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJGPUMON {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct OBJGPUMON *__nvoc_pbase_OBJGPUMON;    // gpumon

    // Vtable with 14 per-object function pointers
    NV_STATUS (*__gpumonConstructEngine__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, ENGDESCRIPTOR);  // virtual inherited (engstate) base (engstate)
    void (*__gpumonInitMissing__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePreInitLocked__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePreInitUnlocked__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStateInitLocked__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStateInitUnlocked__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePreLoad__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStateLoad__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePostLoad__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePreUnload__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStateUnload__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__gpumonStatePostUnload__)(struct OBJGPU *, struct OBJGPUMON * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__gpumonStateDestroy__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__gpumonIsPresent__)(struct OBJGPU *, struct OBJGPUMON * /*this*/);  // virtual inherited (engstate) base (engstate)
};

#ifndef __NVOC_CLASS_OBJGPUMON_TYPEDEF__
#define __NVOC_CLASS_OBJGPUMON_TYPEDEF__
typedef struct OBJGPUMON OBJGPUMON;
#endif /* __NVOC_CLASS_OBJGPUMON_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUMON
#define __nvoc_class_id_OBJGPUMON 0x2b424b
#endif /* __nvoc_class_id_OBJGPUMON */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON;

#define __staticCast_OBJGPUMON(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPUMON)

#ifdef __nvoc_objgpumon_h_disabled
#define __dynamicCast_OBJGPUMON(pThis) ((OBJGPUMON*)NULL)
#else //__nvoc_objgpumon_h_disabled
#define __dynamicCast_OBJGPUMON(pThis) \
    ((OBJGPUMON*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPUMON)))
#endif //__nvoc_objgpumon_h_disabled

// Property macros
#define PDB_PROP_GPUMON_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_GPUMON_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMON(OBJGPUMON**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPUMON(OBJGPUMON**, Dynamic*, NvU32);
#define __objCreate_OBJGPUMON(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGPUMON((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define gpumonConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__
#define gpumonConstructEngine(pGpu, pEngstate, arg3) gpumonConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define gpumonInitMissing(pGpu, pEngstate) gpumonInitMissing_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define gpumonStatePreInitLocked(pGpu, pEngstate) gpumonStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define gpumonStatePreInitUnlocked(pGpu, pEngstate) gpumonStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define gpumonStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__
#define gpumonStateInitLocked(pGpu, pEngstate) gpumonStateInitLocked_DISPATCH(pGpu, pEngstate)
#define gpumonStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define gpumonStateInitUnlocked(pGpu, pEngstate) gpumonStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define gpumonStatePreLoad(pGpu, pEngstate, arg3) gpumonStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateLoad__
#define gpumonStateLoad(pGpu, pEngstate, arg3) gpumonStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define gpumonStatePostLoad(pGpu, pEngstate, arg3) gpumonStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define gpumonStatePreUnload(pGpu, pEngstate, arg3) gpumonStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateUnload__
#define gpumonStateUnload(pGpu, pEngstate, arg3) gpumonStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define gpumonStatePostUnload(pGpu, pEngstate, arg3) gpumonStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define gpumonStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__
#define gpumonStateDestroy(pGpu, pEngstate) gpumonStateDestroy_DISPATCH(pGpu, pEngstate)
#define gpumonIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define gpumonIsPresent(pGpu, pEngstate) gpumonIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS gpumonConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__gpumonConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void gpumonInitMissing_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    pEngstate->__gpumonInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS gpumonStateLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS gpumonStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS gpumonStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS gpumonStateUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS gpumonStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return pEngstate->__gpumonStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void gpumonStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    pEngstate->__gpumonStateDestroy__(pGpu, pEngstate);
}

static inline NvBool gpumonIsPresent_DISPATCH(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonIsPresent__(pGpu, pEngstate);
}

void gpumonGetContextProcessInfo_GM107(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, RM_ENGINE_TYPE rmEngineTypeBegin, RM_ENGINE_TYPE rmEngineTypeEnd, NvU32 context, NvU32 *pProcID, NvU32 *pSubProcessID, const char **pSubProcessName);


#ifdef __nvoc_objgpumon_h_disabled
static inline void gpumonGetContextProcessInfo(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, RM_ENGINE_TYPE rmEngineTypeBegin, RM_ENGINE_TYPE rmEngineTypeEnd, NvU32 context, NvU32 *pProcID, NvU32 *pSubProcessID, const char **pSubProcessName) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUMON was disabled!");
}
#else //__nvoc_objgpumon_h_disabled
#define gpumonGetContextProcessInfo(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName) gpumonGetContextProcessInfo_GM107(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName)
#endif //__nvoc_objgpumon_h_disabled

#define gpumonGetContextProcessInfo_HAL(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName) gpumonGetContextProcessInfo(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName)

NV_STATUS gpumonGetPerfmonUtilSamples_IMPL(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *arg3, NvU32 arg4, NvU32 *arg5);

#ifdef __nvoc_objgpumon_h_disabled
static inline NV_STATUS gpumonGetPerfmonUtilSamples(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *arg3, NvU32 arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUMON was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objgpumon_h_disabled
#define gpumonGetPerfmonUtilSamples(pGpu, pGpumon, arg3, arg4, arg5) gpumonGetPerfmonUtilSamples_IMPL(pGpu, pGpumon, arg3, arg4, arg5)
#endif //__nvoc_objgpumon_h_disabled

#undef PRIVATE_FIELD


#endif //OBJGPUMON_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJGPUMON_NVOC_H_
