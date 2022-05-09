#ifndef _G_ENG_STATE_NVOC_H_
#define _G_ENG_STATE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_eng_state_nvoc.h"

#ifndef _ENG_STATE_H_
#define _ENG_STATE_H_

/*!
 * @file   eng_state.h
 * @brief  Provides definitions for all OBJENGSTATE data structures and interfaces.
 */

#include "core/core.h"
#include "gpu/eng_desc.h"

typedef enum ENGSTATE_STATE
{
    ENGSTATE_STATE_UNDEFINED = 0,
    ENGSTATE_STATE_CONSTRUCT,
    ENGSTATE_STATE_PRE_INIT,
    ENGSTATE_STATE_INIT,
    ENGSTATE_STATE_PRE_LOAD,
    ENGSTATE_STATE_LOAD,
    ENGSTATE_STATE_POST_LOAD,
    ENGSTATE_STATE_PRE_UNLOAD,
    ENGSTATE_STATE_UNLOAD,
    ENGSTATE_STATE_POST_UNLOAD,
    ENGSTATE_STATE_DESTROY,
    ENGSTATE_STATE_COUNT // Keep this last
} ENGSTATE_STATE;

// Stats data stored for every state transition
typedef struct ENGSTATE_STATS
{
    NvS32 memoryAllocCount;
    NvS32 memoryAllocSize;
    NvU32 transitionTimeUs;
} ENGSTATE_STATS;

// Temporary transition data, not stored
typedef struct ENGSTATE_TRANSITION_DATA
{
    NvS64 memoryAllocCount;
    NvS64 memoryAllocSize;
    NvU64 transitionStartTimeNs;
} ENGSTATE_TRANSITION_DATA;

typedef struct OBJENGSTATE *POBJENGSTATE;

#define ENG_GET_FIFO(p)                 (engstateGetFifo(staticCast((p), OBJENGSTATE)))
#define ENG_GET_ENG_DESC(p)             (staticCast((p), OBJENGSTATE)->engDesc)


/*!
 * Defines the structure used to contain all generic information related to
 * the OBJENGSTATE.
 */
#ifdef NVOC_ENG_STATE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJENGSTATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    NV_STATUS (*__engstateConstructEngine__)(POBJGPU, POBJENGSTATE, ENGDESCRIPTOR);
    void (*__engstateInitMissing__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateStatePreInitLocked__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateStatePreInitUnlocked__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateStateInitLocked__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateStateInitUnlocked__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateStatePreLoad__)(POBJGPU, POBJENGSTATE, NvU32);
    NV_STATUS (*__engstateStateLoad__)(POBJGPU, POBJENGSTATE, NvU32);
    NV_STATUS (*__engstateStatePostLoad__)(POBJGPU, POBJENGSTATE, NvU32);
    NV_STATUS (*__engstateStatePreUnload__)(POBJGPU, POBJENGSTATE, NvU32);
    NV_STATUS (*__engstateStateUnload__)(POBJGPU, POBJENGSTATE, NvU32);
    NV_STATUS (*__engstateStatePostUnload__)(POBJGPU, POBJENGSTATE, NvU32);
    void (*__engstateStateDestroy__)(POBJGPU, POBJENGSTATE);
    NV_STATUS (*__engstateAllocTunableState__)(POBJGPU, POBJENGSTATE, void **);
    void (*__engstateFreeTunableState__)(POBJGPU, POBJENGSTATE, void *);
    NV_STATUS (*__engstateGetTunableState__)(POBJGPU, POBJENGSTATE, void *);
    NV_STATUS (*__engstateSetTunableState__)(POBJGPU, POBJENGSTATE, void *);
    NV_STATUS (*__engstateReconcileTunableState__)(POBJGPU, POBJENGSTATE, void *);
    NV_STATUS (*__engstateCompareTunableState__)(POBJGPU, POBJENGSTATE, void *, void *);
    NvBool (*__engstateIsPresent__)(POBJGPU, POBJENGSTATE);
    NvBool PDB_PROP_ENGSTATE_IS_MISSING;
    ENGDESCRIPTOR engDesc;
    void *pOriginalTunableState;
    struct OBJGPU *pGpu;
    ENGSTATE_STATE currentState;
    ENGSTATE_STATS stats[11];
    char name[100];
};

#ifndef __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
#define __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
typedef struct OBJENGSTATE OBJENGSTATE;
#endif /* __NVOC_CLASS_OBJENGSTATE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJENGSTATE
#define __nvoc_class_id_OBJENGSTATE 0x7a7ed6
#endif /* __nvoc_class_id_OBJENGSTATE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

#define __staticCast_OBJENGSTATE(pThis) \
    ((pThis)->__nvoc_pbase_OBJENGSTATE)

#ifdef __nvoc_eng_state_h_disabled
#define __dynamicCast_OBJENGSTATE(pThis) ((OBJENGSTATE*)NULL)
#else //__nvoc_eng_state_h_disabled
#define __dynamicCast_OBJENGSTATE(pThis) \
    ((OBJENGSTATE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJENGSTATE)))
#endif //__nvoc_eng_state_h_disabled

#define PDB_PROP_ENGSTATE_IS_MISSING_BASE_CAST
#define PDB_PROP_ENGSTATE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJENGSTATE(OBJENGSTATE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJENGSTATE(OBJENGSTATE**, Dynamic*, NvU32);
#define __objCreate_OBJENGSTATE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJENGSTATE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define engstateConstructEngine(pGpu, pEngstate, arg0) engstateConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
#define engstateInitMissing(pGpu, pEngstate) engstateInitMissing_DISPATCH(pGpu, pEngstate)
#define engstateStatePreInitLocked(pGpu, pEngstate) engstateStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define engstateStatePreInitUnlocked(pGpu, pEngstate) engstateStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define engstateStateInitLocked(pGpu, pEngstate) engstateStateInitLocked_DISPATCH(pGpu, pEngstate)
#define engstateStateInitUnlocked(pGpu, pEngstate) engstateStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define engstateStatePreLoad(pGpu, pEngstate, arg0) engstateStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStateLoad(pGpu, pEngstate, arg0) engstateStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStatePostLoad(pGpu, pEngstate, arg0) engstateStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStatePreUnload(pGpu, pEngstate, arg0) engstateStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStateUnload(pGpu, pEngstate, arg0) engstateStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStatePostUnload(pGpu, pEngstate, arg0) engstateStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define engstateStateDestroy(pGpu, pEngstate) engstateStateDestroy_DISPATCH(pGpu, pEngstate)
#define engstateAllocTunableState(pGpu, pEngstate, ppTunableState) engstateAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define engstateFreeTunableState(pGpu, pEngstate, pTunableState) engstateFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define engstateGetTunableState(pGpu, pEngstate, pTunableState) engstateGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define engstateSetTunableState(pGpu, pEngstate, pTunableState) engstateSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define engstateReconcileTunableState(pGpu, pEngstate, pTunableState) engstateReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define engstateCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) engstateCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define engstateIsPresent(pGpu, pEngstate) engstateIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS engstateConstructEngine_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, ENGDESCRIPTOR arg0);

static inline NV_STATUS engstateConstructEngine_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__engstateConstructEngine__(pGpu, pEngstate, arg0);
}

void engstateInitMissing_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline void engstateInitMissing_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    pEngstate->__engstateInitMissing__(pGpu, pEngstate);
}

NV_STATUS engstateStatePreInitLocked_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline NV_STATUS engstateStatePreInitLocked_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    return pEngstate->__engstateStatePreInitLocked__(pGpu, pEngstate);
}

NV_STATUS engstateStatePreInitUnlocked_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline NV_STATUS engstateStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    return pEngstate->__engstateStatePreInitUnlocked__(pGpu, pEngstate);
}

NV_STATUS engstateStateInitLocked_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline NV_STATUS engstateStateInitLocked_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    return pEngstate->__engstateStateInitLocked__(pGpu, pEngstate);
}

NV_STATUS engstateStateInitUnlocked_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline NV_STATUS engstateStateInitUnlocked_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    return pEngstate->__engstateStateInitUnlocked__(pGpu, pEngstate);
}

NV_STATUS engstateStatePreLoad_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStatePreLoad_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStatePreLoad__(pGpu, pEngstate, arg0);
}

NV_STATUS engstateStateLoad_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStateLoad_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStateLoad__(pGpu, pEngstate, arg0);
}

NV_STATUS engstateStatePostLoad_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStatePostLoad_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStatePostLoad__(pGpu, pEngstate, arg0);
}

NV_STATUS engstateStatePreUnload_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStatePreUnload_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStatePreUnload__(pGpu, pEngstate, arg0);
}

NV_STATUS engstateStateUnload_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStateUnload_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStateUnload__(pGpu, pEngstate, arg0);
}

NV_STATUS engstateStatePostUnload_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0);

static inline NV_STATUS engstateStatePostUnload_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, NvU32 arg0) {
    return pEngstate->__engstateStatePostUnload__(pGpu, pEngstate, arg0);
}

void engstateStateDestroy_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline void engstateStateDestroy_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    pEngstate->__engstateStateDestroy__(pGpu, pEngstate);
}

NV_STATUS engstateAllocTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void **ppTunableState);

static inline NV_STATUS engstateAllocTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void **ppTunableState) {
    return pEngstate->__engstateAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

void engstateFreeTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState);

static inline void engstateFreeTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState) {
    pEngstate->__engstateFreeTunableState__(pGpu, pEngstate, pTunableState);
}

NV_STATUS engstateGetTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState);

static inline NV_STATUS engstateGetTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState) {
    return pEngstate->__engstateGetTunableState__(pGpu, pEngstate, pTunableState);
}

NV_STATUS engstateSetTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState);

static inline NV_STATUS engstateSetTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState) {
    return pEngstate->__engstateSetTunableState__(pGpu, pEngstate, pTunableState);
}

NV_STATUS engstateReconcileTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState);

static inline NV_STATUS engstateReconcileTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunableState) {
    return pEngstate->__engstateReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

NV_STATUS engstateCompareTunableState_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunables1, void *pTunables2);

static inline NV_STATUS engstateCompareTunableState_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__engstateCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

NvBool engstateIsPresent_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);

static inline NvBool engstateIsPresent_DISPATCH(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    return pEngstate->__engstateIsPresent__(pGpu, pEngstate);
}

NV_STATUS engstateConstructBase_IMPL(struct OBJENGSTATE *arg0, struct OBJGPU *arg1, ENGDESCRIPTOR arg2);
#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateConstructBase(struct OBJENGSTATE *arg0, struct OBJGPU *arg1, ENGDESCRIPTOR arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateConstructBase(arg0, arg1, arg2) engstateConstructBase_IMPL(arg0, arg1, arg2)
#endif //__nvoc_eng_state_h_disabled

void engstateLogStateTransitionPre_IMPL(struct OBJENGSTATE *arg0, ENGSTATE_STATE arg1, ENGSTATE_TRANSITION_DATA *arg2);
#ifdef __nvoc_eng_state_h_disabled
static inline void engstateLogStateTransitionPre(struct OBJENGSTATE *arg0, ENGSTATE_STATE arg1, ENGSTATE_TRANSITION_DATA *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
}
#else //__nvoc_eng_state_h_disabled
#define engstateLogStateTransitionPre(arg0, arg1, arg2) engstateLogStateTransitionPre_IMPL(arg0, arg1, arg2)
#endif //__nvoc_eng_state_h_disabled

void engstateLogStateTransitionPost_IMPL(struct OBJENGSTATE *arg0, ENGSTATE_STATE arg1, ENGSTATE_TRANSITION_DATA *arg2);
#ifdef __nvoc_eng_state_h_disabled
static inline void engstateLogStateTransitionPost(struct OBJENGSTATE *arg0, ENGSTATE_STATE arg1, ENGSTATE_TRANSITION_DATA *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
}
#else //__nvoc_eng_state_h_disabled
#define engstateLogStateTransitionPost(arg0, arg1, arg2) engstateLogStateTransitionPost_IMPL(arg0, arg1, arg2)
#endif //__nvoc_eng_state_h_disabled

const char *engstateGetName_IMPL(struct OBJENGSTATE *arg0);
#ifdef __nvoc_eng_state_h_disabled
static inline const char *engstateGetName(struct OBJENGSTATE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NULL;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetName(arg0) engstateGetName_IMPL(arg0)
#endif //__nvoc_eng_state_h_disabled

void engstateDestruct_IMPL(POBJENGSTATE pEngstate);
#define __nvoc_engstateDestruct(pEngstate) engstateDestruct_IMPL(pEngstate)
NV_STATUS engstateStatePreInit_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);
#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateStatePreInit(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateStatePreInit(pGpu, pEngstate) engstateStatePreInit_IMPL(pGpu, pEngstate)
#endif //__nvoc_eng_state_h_disabled

NV_STATUS engstateStateInit_IMPL(POBJGPU pGpu, POBJENGSTATE pEngstate);
#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateStateInit(POBJGPU pGpu, POBJENGSTATE pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateStateInit(pGpu, pEngstate) engstateStateInit_IMPL(pGpu, pEngstate)
#endif //__nvoc_eng_state_h_disabled

ENGDESCRIPTOR engstateGetDescriptor_IMPL(POBJENGSTATE pEngstate);
#ifdef __nvoc_eng_state_h_disabled
static inline ENGDESCRIPTOR engstateGetDescriptor(POBJENGSTATE pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    ENGDESCRIPTOR ret;
    portMemSet(&ret, 0, sizeof(ENGDESCRIPTOR));
    return ret;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetDescriptor(pEngstate) engstateGetDescriptor_IMPL(pEngstate)
#endif //__nvoc_eng_state_h_disabled

struct OBJFIFO *engstateGetFifo_IMPL(POBJENGSTATE pEngstate);
#ifdef __nvoc_eng_state_h_disabled
static inline struct OBJFIFO *engstateGetFifo(POBJENGSTATE pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NULL;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetFifo(pEngstate) engstateGetFifo_IMPL(pEngstate)
#endif //__nvoc_eng_state_h_disabled

#undef PRIVATE_FIELD


#endif // _ENG_STATE_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_ENG_STATE_NVOC_H_
