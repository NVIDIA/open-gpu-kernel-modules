#ifndef _G_OBJGPUMON_NVOC_H_
#define _G_OBJGPUMON_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_objgpumon_nvoc.h"

#ifndef _OBJGPUMON_H_
#define _OBJGPUMON_H_

/****************** Resource Manager Defines and Structures *****************\
*                                                                            *
*       Defines and structures used for the Gpumon Object.                   *
*                                                                            *
\****************************************************************************/

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

#include "ctrl/ctrl2080/ctrl2080perf.h"

typedef struct OBJGPUMON *POBJGPUMON;

#ifdef NVOC_OBJGPUMON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJGPUMON {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct OBJGPUMON *__nvoc_pbase_OBJGPUMON;
    NV_STATUS (*__gpumonStateLoad__)(POBJGPU, struct OBJGPUMON *, NvU32);
    NV_STATUS (*__gpumonStateUnload__)(POBJGPU, struct OBJGPUMON *, NvU32);
    NV_STATUS (*__gpumonStateInitLocked__)(POBJGPU, struct OBJGPUMON *);
    NV_STATUS (*__gpumonStatePreLoad__)(POBJGPU, struct OBJGPUMON *, NvU32);
    NV_STATUS (*__gpumonStatePostUnload__)(POBJGPU, struct OBJGPUMON *, NvU32);
    void (*__gpumonStateDestroy__)(POBJGPU, struct OBJGPUMON *);
    NV_STATUS (*__gpumonStatePreUnload__)(POBJGPU, struct OBJGPUMON *, NvU32);
    NV_STATUS (*__gpumonStateInitUnlocked__)(POBJGPU, struct OBJGPUMON *);
    void (*__gpumonInitMissing__)(POBJGPU, struct OBJGPUMON *);
    NV_STATUS (*__gpumonStatePreInitLocked__)(POBJGPU, struct OBJGPUMON *);
    NV_STATUS (*__gpumonStatePreInitUnlocked__)(POBJGPU, struct OBJGPUMON *);
    NV_STATUS (*__gpumonStatePostLoad__)(POBJGPU, struct OBJGPUMON *, NvU32);
    NV_STATUS (*__gpumonConstructEngine__)(POBJGPU, struct OBJGPUMON *, ENGDESCRIPTOR);
    NvBool (*__gpumonIsPresent__)(POBJGPU, struct OBJGPUMON *);
};

#ifndef __NVOC_CLASS_OBJGPUMON_TYPEDEF__
#define __NVOC_CLASS_OBJGPUMON_TYPEDEF__
typedef struct OBJGPUMON OBJGPUMON;
#endif /* __NVOC_CLASS_OBJGPUMON_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUMON
#define __nvoc_class_id_OBJGPUMON 0x2b424b
#endif /* __nvoc_class_id_OBJGPUMON */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON;

#define __staticCast_OBJGPUMON(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPUMON)

#ifdef __nvoc_objgpumon_h_disabled
#define __dynamicCast_OBJGPUMON(pThis) ((OBJGPUMON*)NULL)
#else //__nvoc_objgpumon_h_disabled
#define __dynamicCast_OBJGPUMON(pThis) \
    ((OBJGPUMON*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPUMON)))
#endif //__nvoc_objgpumon_h_disabled

#define PDB_PROP_GPUMON_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_GPUMON_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMON(OBJGPUMON**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPUMON(OBJGPUMON**, Dynamic*, NvU32);
#define __objCreate_OBJGPUMON(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGPUMON((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define gpumonStateLoad(pGpu, pEngstate, arg0) gpumonStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonStateUnload(pGpu, pEngstate, arg0) gpumonStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonStateInitLocked(pGpu, pEngstate) gpumonStateInitLocked_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreLoad(pGpu, pEngstate, arg0) gpumonStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonStatePostUnload(pGpu, pEngstate, arg0) gpumonStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonStateDestroy(pGpu, pEngstate) gpumonStateDestroy_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreUnload(pGpu, pEngstate, arg0) gpumonStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonStateInitUnlocked(pGpu, pEngstate) gpumonStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define gpumonInitMissing(pGpu, pEngstate) gpumonInitMissing_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreInitLocked(pGpu, pEngstate) gpumonStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define gpumonStatePreInitUnlocked(pGpu, pEngstate) gpumonStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define gpumonStatePostLoad(pGpu, pEngstate, arg0) gpumonStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonConstructEngine(pGpu, pEngstate, arg0) gpumonConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
#define gpumonIsPresent(pGpu, pEngstate) gpumonIsPresent_DISPATCH(pGpu, pEngstate)
void gpumonGetContextProcessInfo_GM107(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, RM_ENGINE_TYPE rmEngineTypeBegin, RM_ENGINE_TYPE rmEngineTypeEnd, NvU32 context, NvU32 *pProcID, NvU32 *pSubProcessID, const char **pSubProcessName);


#ifdef __nvoc_objgpumon_h_disabled
static inline void gpumonGetContextProcessInfo(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, RM_ENGINE_TYPE rmEngineTypeBegin, RM_ENGINE_TYPE rmEngineTypeEnd, NvU32 context, NvU32 *pProcID, NvU32 *pSubProcessID, const char **pSubProcessName) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUMON was disabled!");
}
#else //__nvoc_objgpumon_h_disabled
#define gpumonGetContextProcessInfo(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName) gpumonGetContextProcessInfo_GM107(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName)
#endif //__nvoc_objgpumon_h_disabled

#define gpumonGetContextProcessInfo_HAL(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName) gpumonGetContextProcessInfo(pGpu, pGpumon, rmEngineTypeBegin, rmEngineTypeEnd, context, pProcID, pSubProcessID, pSubProcessName)

static inline NV_STATUS gpumonStateLoad_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS gpumonStateUnload_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS gpumonStateInitLocked_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreLoad_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS gpumonStatePostUnload_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void gpumonStateDestroy_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    pEngstate->__gpumonStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreUnload_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS gpumonStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStateInitUnlocked__(pGpu, pEngstate);
}

static inline void gpumonInitMissing_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    pEngstate->__gpumonInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS gpumonStatePostLoad_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return pEngstate->__gpumonStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS gpumonConstructEngine_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__gpumonConstructEngine__(pGpu, pEngstate, arg0);
}

static inline NvBool gpumonIsPresent_DISPATCH(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return pEngstate->__gpumonIsPresent__(pGpu, pEngstate);
}

NV_STATUS gpumonGetPerfmonUtilSamples_IMPL(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *arg0, NvU32 arg1, NvU32 *arg2);

#ifdef __nvoc_objgpumon_h_disabled
static inline NV_STATUS gpumonGetPerfmonUtilSamples(struct OBJGPU *pGpu, struct OBJGPUMON *pGpumon, NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *arg0, NvU32 arg1, NvU32 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUMON was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objgpumon_h_disabled
#define gpumonGetPerfmonUtilSamples(pGpu, pGpumon, arg0, arg1, arg2) gpumonGetPerfmonUtilSamples_IMPL(pGpu, pGpumon, arg0, arg1, arg2)
#endif //__nvoc_objgpumon_h_disabled

#undef PRIVATE_FIELD


#endif //_OBJGPUMON_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJGPUMON_NVOC_H_
