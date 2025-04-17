
#ifndef _G_ENG_STATE_NVOC_H_
#define _G_ENG_STATE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_eng_state_nvoc.h"

#ifndef _ENG_STATE_H_
#define _ENG_STATE_H_

/*!
 * @file   eng_state.h
 * @brief  Provides definitions for all OBJENGSTATE data structures and interfaces.
 */

#include "core/core.h"
#include "nvoc/object.h"
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


#define ENG_GET_FIFO(p)                 (engstateGetFifo(staticCast((p), OBJENGSTATE)))
#define ENG_GET_ENG_DESC(p)             (staticCast((p), OBJENGSTATE)->engDesc)


/*!
 * Defines the structure used to contain all generic information related to
 * the OBJENGSTATE.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_ENG_STATE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__Object;
struct NVOC_VTABLE__OBJENGSTATE;


struct OBJENGSTATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJENGSTATE *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate

    // 1 PDB property
    NvBool PDB_PROP_ENGSTATE_IS_MISSING;

    // Data members
    ENGDESCRIPTOR engDesc;
    struct OBJGPU *pGpu;
    ENGSTATE_STATE currentState;
    ENGSTATE_STATS stats[11];
    char name[100];
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__OBJENGSTATE {
    NV_STATUS (*__engstateConstructEngine__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, ENGDESCRIPTOR);  // virtual
    void (*__engstateInitMissing__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NV_STATUS (*__engstateStatePreInitLocked__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NV_STATUS (*__engstateStatePreInitUnlocked__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NV_STATUS (*__engstateStateInitLocked__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NV_STATUS (*__engstateStateInitUnlocked__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NV_STATUS (*__engstateStatePreLoad__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    NV_STATUS (*__engstateStateLoad__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    NV_STATUS (*__engstateStatePostLoad__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    NV_STATUS (*__engstateStatePreUnload__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    NV_STATUS (*__engstateStateUnload__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    NV_STATUS (*__engstateStatePostUnload__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/, NvU32);  // virtual
    void (*__engstateStateDestroy__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
    NvBool (*__engstateIsPresent__)(struct OBJGPU *, struct OBJENGSTATE * /*this*/);  // virtual
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJENGSTATE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
    const struct NVOC_VTABLE__OBJENGSTATE vtable;
};

#ifndef __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
#define __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
typedef struct OBJENGSTATE OBJENGSTATE;
#endif /* __NVOC_CLASS_OBJENGSTATE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJENGSTATE
#define __nvoc_class_id_OBJENGSTATE 0x7a7ed6
#endif /* __nvoc_class_id_OBJENGSTATE */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

#define __staticCast_OBJENGSTATE(pThis) \
    ((pThis)->__nvoc_pbase_OBJENGSTATE)

#ifdef __nvoc_eng_state_h_disabled
#define __dynamicCast_OBJENGSTATE(pThis) ((OBJENGSTATE*) NULL)
#else //__nvoc_eng_state_h_disabled
#define __dynamicCast_OBJENGSTATE(pThis) \
    ((OBJENGSTATE*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJENGSTATE)))
#endif //__nvoc_eng_state_h_disabled

// Property macros
#define PDB_PROP_ENGSTATE_IS_MISSING_BASE_CAST
#define PDB_PROP_ENGSTATE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJENGSTATE(OBJENGSTATE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJENGSTATE(OBJENGSTATE**, Dynamic*, NvU32);
#define __objCreate_OBJENGSTATE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJENGSTATE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define engstateConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define engstateConstructEngine(pGpu, pEngstate, arg3) engstateConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define engstateInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define engstateInitMissing(pGpu, pEngstate) engstateInitMissing_DISPATCH(pGpu, pEngstate)
#define engstateStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define engstateStatePreInitLocked(pGpu, pEngstate) engstateStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define engstateStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define engstateStatePreInitUnlocked(pGpu, pEngstate) engstateStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define engstateStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define engstateStateInitLocked(pGpu, pEngstate) engstateStateInitLocked_DISPATCH(pGpu, pEngstate)
#define engstateStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define engstateStateInitUnlocked(pGpu, pEngstate) engstateStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define engstateStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define engstateStatePreLoad(pGpu, pEngstate, arg3) engstateStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define engstateStateLoad(pGpu, pEngstate, arg3) engstateStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define engstateStatePostLoad(pGpu, pEngstate, arg3) engstateStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define engstateStatePreUnload(pGpu, pEngstate, arg3) engstateStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define engstateStateUnload(pGpu, pEngstate, arg3) engstateStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define engstateStatePostUnload(pGpu, pEngstate, arg3) engstateStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define engstateStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define engstateStateDestroy(pGpu, pEngstate) engstateStateDestroy_DISPATCH(pGpu, pEngstate)
#define engstateIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define engstateIsPresent(pGpu, pEngstate) engstateIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS engstateConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void engstateInitMissing_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS engstateStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS engstateStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS engstateStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS engstateStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS engstateStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS engstateStateLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS engstateStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS engstateStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS engstateStateUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS engstateStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void engstateStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(pGpu, pEngstate);
}

static inline NvBool engstateIsPresent_DISPATCH(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, pEngstate);
}

NV_STATUS engstateConstructEngine_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, ENGDESCRIPTOR arg3);

void engstateInitMissing_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NV_STATUS engstateStatePreInitLocked_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NV_STATUS engstateStatePreInitUnlocked_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NV_STATUS engstateStateInitLocked_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NV_STATUS engstateStateInitUnlocked_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NV_STATUS engstateStatePreLoad_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

NV_STATUS engstateStateLoad_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

NV_STATUS engstateStatePostLoad_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

NV_STATUS engstateStatePreUnload_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

NV_STATUS engstateStateUnload_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

NV_STATUS engstateStatePostUnload_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate, NvU32 arg3);

void engstateStateDestroy_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

NvBool engstateIsPresent_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

void engstateDestruct_IMPL(struct OBJENGSTATE *pEngstate);

#define __nvoc_engstateDestruct(pEngstate) engstateDestruct_IMPL(pEngstate)
NV_STATUS engstateStatePreInit_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateStatePreInit(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateStatePreInit(pGpu, pEngstate) engstateStatePreInit_IMPL(pGpu, pEngstate)
#endif //__nvoc_eng_state_h_disabled

NV_STATUS engstateStateInit_IMPL(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate);

#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateStateInit(struct OBJGPU *pGpu, struct OBJENGSTATE *pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateStateInit(pGpu, pEngstate) engstateStateInit_IMPL(pGpu, pEngstate)
#endif //__nvoc_eng_state_h_disabled

ENGDESCRIPTOR engstateGetDescriptor_IMPL(struct OBJENGSTATE *pEngstate);

#ifdef __nvoc_eng_state_h_disabled
static inline ENGDESCRIPTOR engstateGetDescriptor(struct OBJENGSTATE *pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    ENGDESCRIPTOR ret;
    portMemSet(&ret, 0, sizeof(ENGDESCRIPTOR));
    return ret;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetDescriptor(pEngstate) engstateGetDescriptor_IMPL(pEngstate)
#endif //__nvoc_eng_state_h_disabled

struct OBJFIFO *engstateGetFifo_IMPL(struct OBJENGSTATE *pEngstate);

#ifdef __nvoc_eng_state_h_disabled
static inline struct OBJFIFO *engstateGetFifo(struct OBJENGSTATE *pEngstate) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NULL;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetFifo(pEngstate) engstateGetFifo_IMPL(pEngstate)
#endif //__nvoc_eng_state_h_disabled

NV_STATUS engstateConstructBase_IMPL(struct OBJENGSTATE *arg1, struct OBJGPU *arg2, ENGDESCRIPTOR arg3);

#ifdef __nvoc_eng_state_h_disabled
static inline NV_STATUS engstateConstructBase(struct OBJENGSTATE *arg1, struct OBJGPU *arg2, ENGDESCRIPTOR arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_eng_state_h_disabled
#define engstateConstructBase(arg1, arg2, arg3) engstateConstructBase_IMPL(arg1, arg2, arg3)
#endif //__nvoc_eng_state_h_disabled

void engstateLogStateTransitionPre_IMPL(struct OBJENGSTATE *arg1, ENGSTATE_STATE arg2, ENGSTATE_TRANSITION_DATA *arg3);

#ifdef __nvoc_eng_state_h_disabled
static inline void engstateLogStateTransitionPre(struct OBJENGSTATE *arg1, ENGSTATE_STATE arg2, ENGSTATE_TRANSITION_DATA *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
}
#else //__nvoc_eng_state_h_disabled
#define engstateLogStateTransitionPre(arg1, arg2, arg3) engstateLogStateTransitionPre_IMPL(arg1, arg2, arg3)
#endif //__nvoc_eng_state_h_disabled

void engstateLogStateTransitionPost_IMPL(struct OBJENGSTATE *arg1, ENGSTATE_STATE arg2, ENGSTATE_TRANSITION_DATA *arg3);

#ifdef __nvoc_eng_state_h_disabled
static inline void engstateLogStateTransitionPost(struct OBJENGSTATE *arg1, ENGSTATE_STATE arg2, ENGSTATE_TRANSITION_DATA *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
}
#else //__nvoc_eng_state_h_disabled
#define engstateLogStateTransitionPost(arg1, arg2, arg3) engstateLogStateTransitionPost_IMPL(arg1, arg2, arg3)
#endif //__nvoc_eng_state_h_disabled

const char *engstateGetName_IMPL(struct OBJENGSTATE *arg1);

#ifdef __nvoc_eng_state_h_disabled
static inline const char *engstateGetName(struct OBJENGSTATE *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJENGSTATE was disabled!");
    return NULL;
}
#else //__nvoc_eng_state_h_disabled
#define engstateGetName(arg1) engstateGetName_IMPL(arg1)
#endif //__nvoc_eng_state_h_disabled

#undef PRIVATE_FIELD


#endif // _ENG_STATE_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ENG_STATE_NVOC_H_
