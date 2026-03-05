
#ifndef _G_PREREQ_TRACKER_NVOC_H_
#define _G_PREREQ_TRACKER_NVOC_H_

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

/*!
 * @file    prereq_tracker.h
 * @brief   Holds interfaces and data structures required by the prerequisite
 *          tracking feature/code.
 *
 *   Code depending on multiple other features should use prereqComposeEntry() to create
 * a prerequisite tracking structure with a provided bitVector of all necessary
 * dependencies, which will arm the prereq to start watching those dependencies.
 *   Once those dependencies are fulfilled they should issue prereqSatisfy() (one-by-one)
 * This common code should broadcast those to all prerequisite tracking structures
 * and once all respective dependencies are satisfied, will issue the
 * registered callback.
 *   Similarly, dependencies should issue prereqRetract() before they change
 * their state and common code will broadcast that to all tracking structures
 * and issue callbacks again with bSatisfied=false, if all dependencies
 * for that prereq were previously satisfied.
 *   Prerequisites can also be added as deferrable. In this case, when all dependencies
 * are satisfied (or a satisfied PREREQ_ENTRY is unsatisfied), the associated callback
 * will not be invoked directly inline. Instead, if deferment conditions are met,
 * a work-item will be scheduled to execute the callback asynchronously.
 * This allows the callback to run outside of contexts where direct
 * invocation is undesirable (e.g., high IRQL or blocking RPC in progress).
 *
 * @note      Feature is designed to prevent creating new prerequisites once
 *          dependencies start issuing Satisfy()/Retract() notifications.
 *            Therefore, ComposeEntry all prerequisites during
 *          stateInit() and allow code to issue Satisfy()/Retract() only in
 *          stateLoad() or later.
 */

#pragma once
#include "g_prereq_tracker_nvoc.h"

#ifndef __PREREQUISITE_TRACKER_H__
#define __PREREQUISITE_TRACKER_H__

/* ------------------------ Includes ---------------------------------------- */
#include "containers/list.h"
#include "utils/nvbitvector.h"

#include "nvoc/object.h"

/* ------------------------ Macros ------------------------------------------ */

#define PREREQ_ID_VECTOR_SIZE 64

/*!
 * Performs check if all dependencies of the given prerequisite tracking
 * structure has been satisfied.
 *
 * @param[in]   _pPrereq    PREREQ_ENTRY pointer
 *
 * @return  boolean if prerequisite has been satisfied.
 */
#define PREREQ_IS_SATISFIED(_pPrereq)                                      \
    ((_pPrereq)->countRequested == (_pPrereq)->countSatisfied)

/* ------------------------ Datatypes --------------------------------------- */

struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */



/*!
 * @brief   Callback prototype.
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   bSatisfied
 *      Indicates if dependencies were just satisfied or about to be retracted.
 *
 * @return  NV_OK   if callback successfully executed
 * @return  status  failure specific error code
 */
typedef NV_STATUS GpuPrereqCallback(struct OBJGPU *pGpu, NvBool bSatisfied);

typedef NvU16 PREREQ_ID;

/*!
 * Bitvector for storing prereq IDs required for another prereq struct
 * Limited to size defined above, set to largest required by users
 */
MAKE_BITVECTOR(PREREQ_ID_BIT_VECTOR, PREREQ_ID_VECTOR_SIZE);

/*!
 * An individual prerequisite tracking entry structure.
 */
typedef struct
{
    /*!
     * Mask of the dependencies (prerequisites that have to be satisfied before
     * callback can be issues).
     */
    PREREQ_ID_BIT_VECTOR requested;

    /*!
     * Counter of all dependencies (prerequisites) tracked by this structure.
     */
    NvS32                countRequested;
    /*!
     * Counter of currently satisfied dependencies (prerequisites) tracked by
     * this structure. Once equal to @ref countRequested, callback can be issued.
     */
    NvS32                countSatisfied;

    /*!
     * Boolean indicating that the given PREREQ_ENTRY is armed and ready to fire @ref
     * callback whenever all PREREQ_IDs specified in @ref requested are satisfied.
     *
     * This bit is set during @ref prereqComposeEntry_IMPL(), which will also do an
     * initial satisfaction check of all @ref requested PREREQ_IDs
     * and fire the @ref callback if necessary.
     */
    NvBool               bArmed;

    /*!
     * Boolean indicating whether we should try to defer executing the callback
     * associated with this entry.
     */
    NvBool               bDeferrable;

    /*!
     * Boolean indicating whether a work-item has already been scheduled to
     * execute this entry's callback, used to prevent redundant work-item
     * scheduling.
     */
    NvBool               bWorkItemScheduled;

    /*!
     *  Boolean indicating value of the flag `bSatisfied` passed into this
     * entry's callback during its last invocation, so that we know
     * (1) what value to pass in for next invocation and
     * (2) which condition work-item should double check before next executing
     * the callback.
     */
    NvBool               bLastCallbackWasSatisfy;

    /*!
     * @copydoc GpuPrereqCallback
     */
    GpuPrereqCallback   *callback;
} PREREQ_ENTRY;
MAKE_LIST(PrereqList, PREREQ_ENTRY);

struct PrereqTracker;

#ifndef __nvoc_class_id_PrereqTracker
#define __nvoc_class_id_PrereqTracker 0x0e171bu
typedef struct PrereqTracker PrereqTracker;
#endif /* __nvoc_class_id_PrereqTracker */



typedef NvBool GpuPrereqDeferralCheckCallback(struct PrereqTracker *pPrereqTracker, PREREQ_ENTRY *pPrereqEntry);

/*!
 * Holds common prerequisite tracking information.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PREREQ_TRACKER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PrereqTracker;
struct NVOC_METADATA__Object;


struct PrereqTracker {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PrereqTracker *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct PrereqTracker *__nvoc_pbase_PrereqTracker;    // prereq

    // Data members
    union PREREQ_ID_BIT_VECTOR satisfied;
    NvBool bInitialized;
    PrereqList prereqList;
    struct OBJGPU *pParent;
    GpuPrereqDeferralCheckCallback *pDeferralCheckCallback;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PrereqTracker {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_PrereqTracker
#define __nvoc_class_id_PrereqTracker 0x0e171bu
typedef struct PrereqTracker PrereqTracker;
#endif /* __nvoc_class_id_PrereqTracker */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PrereqTracker;

#define __staticCast_PrereqTracker(pThis) \
    ((pThis)->__nvoc_pbase_PrereqTracker)

#ifdef __nvoc_prereq_tracker_h_disabled
#define __dynamicCast_PrereqTracker(pThis) ((PrereqTracker*) NULL)
#else //__nvoc_prereq_tracker_h_disabled
#define __dynamicCast_PrereqTracker(pThis) \
    ((PrereqTracker*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PrereqTracker)))
#endif //__nvoc_prereq_tracker_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PrereqTracker(PrereqTracker**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PrereqTracker(PrereqTracker**, Dynamic*, NvU32, struct OBJGPU *pParent, GpuPrereqDeferralCheckCallback *pCallback);
#define __objCreate_PrereqTracker(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pParent, pCallback) \
    __nvoc_objCreate_PrereqTracker((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pParent, pCallback)


// Wrapper macros for implementation functions
NV_STATUS prereqConstruct_IMPL(struct PrereqTracker *pTracker, struct OBJGPU *pParent, GpuPrereqDeferralCheckCallback *pCallback);
#define __nvoc_prereqConstruct(pTracker, pParent, pCallback) prereqConstruct_IMPL(pTracker, pParent, pCallback)

void prereqDestruct_IMPL(struct PrereqTracker *pTracker);
#define __nvoc_prereqDestruct(pTracker) prereqDestruct_IMPL(pTracker)

NV_STATUS prereqSatisfy_IMPL(struct PrereqTracker *pTracker, PREREQ_ID prereqId);
#ifdef __nvoc_prereq_tracker_h_disabled
static inline NV_STATUS prereqSatisfy(struct PrereqTracker *pTracker, PREREQ_ID prereqId) {
    NV_ASSERT_FAILED_PRECOMP("PrereqTracker was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_prereq_tracker_h_disabled
#define prereqSatisfy(pTracker, prereqId) prereqSatisfy_IMPL(pTracker, prereqId)
#endif // __nvoc_prereq_tracker_h_disabled

NV_STATUS prereqRetract_IMPL(struct PrereqTracker *pTracker, PREREQ_ID prereqId);
#ifdef __nvoc_prereq_tracker_h_disabled
static inline NV_STATUS prereqRetract(struct PrereqTracker *pTracker, PREREQ_ID prereqId) {
    NV_ASSERT_FAILED_PRECOMP("PrereqTracker was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_prereq_tracker_h_disabled
#define prereqRetract(pTracker, prereqId) prereqRetract_IMPL(pTracker, prereqId)
#endif // __nvoc_prereq_tracker_h_disabled

NvBool prereqIdIsSatisfied_IMPL(struct PrereqTracker *pTracker, PREREQ_ID prereqId);
#ifdef __nvoc_prereq_tracker_h_disabled
static inline NvBool prereqIdIsSatisfied(struct PrereqTracker *pTracker, PREREQ_ID prereqId) {
    NV_ASSERT_FAILED_PRECOMP("PrereqTracker was disabled!");
    return NV_FALSE;
}
#else // __nvoc_prereq_tracker_h_disabled
#define prereqIdIsSatisfied(pTracker, prereqId) prereqIdIsSatisfied_IMPL(pTracker, prereqId)
#endif // __nvoc_prereq_tracker_h_disabled

NV_STATUS prereqComposeEntry_IMPL(struct PrereqTracker *pTracker, GpuPrereqCallback *callback, union PREREQ_ID_BIT_VECTOR *pDepends, PREREQ_ENTRY **ppPrereq);
#ifdef __nvoc_prereq_tracker_h_disabled
static inline NV_STATUS prereqComposeEntry(struct PrereqTracker *pTracker, GpuPrereqCallback *callback, union PREREQ_ID_BIT_VECTOR *pDepends, PREREQ_ENTRY **ppPrereq) {
    NV_ASSERT_FAILED_PRECOMP("PrereqTracker was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_prereq_tracker_h_disabled
#define prereqComposeEntry(pTracker, callback, pDepends, ppPrereq) prereqComposeEntry_IMPL(pTracker, callback, pDepends, ppPrereq)
#endif // __nvoc_prereq_tracker_h_disabled

NV_STATUS prereqComposeEntryDeferrable_IMPL(struct PrereqTracker *pTracker, GpuPrereqCallback *callback, union PREREQ_ID_BIT_VECTOR *pDepends, PREREQ_ENTRY **ppPrereq);
#ifdef __nvoc_prereq_tracker_h_disabled
static inline NV_STATUS prereqComposeEntryDeferrable(struct PrereqTracker *pTracker, GpuPrereqCallback *callback, union PREREQ_ID_BIT_VECTOR *pDepends, PREREQ_ENTRY **ppPrereq) {
    NV_ASSERT_FAILED_PRECOMP("PrereqTracker was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_prereq_tracker_h_disabled
#define prereqComposeEntryDeferrable(pTracker, callback, pDepends, ppPrereq) prereqComposeEntryDeferrable_IMPL(pTracker, callback, pDepends, ppPrereq)
#endif // __nvoc_prereq_tracker_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // __PREREQUISITE_TRACKER_H__

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PREREQ_TRACKER_NVOC_H_
