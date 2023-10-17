#ifndef _G_OBJTMR_NVOC_H_
#define _G_OBJTMR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_objtmr_nvoc.h"

#ifndef _OBJTMR_H_
#define _OBJTMR_H_

/*!
 * @file
 * @brief   Defines and structures used for the Tmr Engine Object.
 */

/* ------------------------ Includes --------------------------------------- */
#include "core/core.h"
#include "core/info_block.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "tmr.h"
#include "lib/ref_count.h"
#include "os/os.h"
#include "nvoc/utility.h"
#include "kernel/gpu/intr/intr_service.h"

/* ------------------------ Macros ----------------------------------------- */
//
// Extent of the timer callback array
//
#define TMR_NUM_CALLBACKS_RM                96
#define TMR_NUM_CALLBACKS_OS                36

// Callback scheduled without any explicit flags set.
#define TMR_FLAGS_NONE              0x00000000
//
// Automatically reschedule the callback, so that it repeats.
// Otherwise, callback is scheduled for one-shot execution.
//
#define TMR_FLAG_RECUR              NVBIT(0)
//
// Indicate that the implementation of the callback function will/can release
// a GPU semaphore. This allows fifoIdleChannels to query this information,
// and hence not bail out early if channels are blocked on semaphores that
// will in fact be released.
// !!NOTE: This is OBSOLETE, it should be moved directly to FIFO, where it's needed
//
#define TMR_FLAG_RELEASE_SEMAPHORE  NVBIT(1)
#define TMR_FLAG_OS_TIMER_QUEUED    NVBIT(2)
//
// Normally, it should not be necessary to use the TMR_FLAG_USE_OS_TIMER flag,
// because the OS timer is selected automatically by the default
// PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS setting.
// Note that the OS timer is not supported in all environments (such as GSP-RM).
//
#define TMR_FLAG_USE_OS_TIMER       NVBIT(3)

#define TMR_GET_GPU(p)   ENG_GET_GPU(p)

/* ------------------------ Function Redefinitions ------------------------- */
#define tmrEventScheduleRelSec(pTmr, pEvent, RelTimeSec) tmrEventScheduleRel(pTmr, pEvent, (NvU64)(RelTimeSec) * 1000000000 )

#define tmrGetInfoBlock(pTmr, pListHead, dataId)         getInfoPtr(pListHead, dataId)
#define tmrAddInfoBlock(pTmr, ppListHead, dataId, size)  addInfoPtr(ppListHead, dataId, size)
#define tmrDeleteInfoBlock(pTmr, ppListHead, dataId)     deleteInfoPtr(ppListHead, dataId)
#define tmrTestInfoBlock(pTmr, pListHead, dataId)        testInfoPtr(pListHead, dataId)

/* ------------------------ Datatypes -------------------------------------- */
TYPEDEF_BITVECTOR(MC_ENGINE_BITVECTOR);

//
// Forward references for timer related structures
//
typedef struct DAYMSECTIME      *PDAYMSECTIME;
typedef struct DAYMSECTIME      DAYMSECTIME;

//
// System time structure
//
struct DAYMSECTIME
{
    NvU32 days;
    NvU32 msecs;
    NvU32 valid;
};

/*!
 * Callback wrapper memory type, used with interfacing all scheduling functions
 * Reveals only partial representation of the event information.
 * User Use only, internal code will not change them.
 */
struct TMR_EVENT
{
    TIMEPROC        pTimeProc;    //<! The callback function for the event.
    void *          pUserData;    //<! Special object used to associate event with.
                                  //<! It will be given to the callback inside this struct.
    void *          pOSTmrCBdata; //<! This parameter holds the data of OS registered timer

    NvU32           flags;
    NvU32           chId;         //<! OBSOLETE, semaphore should be handled by FIFO directly
                                  //<! Used only with TMR_FLAG_RELEASE_SEMAPHORE, this is
                                  //<! soon to be obsoleted, it won't be necessary with a free
                                  //<! object for callbacks to manipulate.
};

/*!
 * Internal representation of the wrapper memory type.
 * Casted from public user datatype, so they have access to some fields only.
 * Internal Use only, will be hidden in time.c when obsolete tasks have migrated.
 */
typedef struct TMR_EVENT_PVT TMR_EVENT_PVT, *PTMR_EVENT_PVT;
struct TMR_EVENT_PVT
{
    // Public interface, must come first, don't declare anything above this field.
    TMR_EVENT           super;

    // Legacy Fields, soon to be obsoleted.
    NvBool              bLegacy;     //<! Used to mark legacy mode, controls which of the
                                     //<! two callbacks will called.
    TIMEPROC_OBSOLETE   pTimeProc_OBSOLETE;

    NvBool              bInUse;      //<! Marks this as currently used
    NvU64               timens;      //<! Absolute time to perform callback
    NvU64               startTimeNs; //<! Store system time at timer callback schedule
    PTMR_EVENT_PVT      pNext;       //<! Next element in the list
};

/*!
 * Struct to pass to event creation and updates
 * as it holds all the relevant data.
 */
typedef struct TMR_EVENT_SET_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 *ppEvent, 8);
    NV_DECLARE_ALIGNED(NvP64 pTimeProc, 8);
    NV_DECLARE_ALIGNED(NvP64 pCallbackData, 8);
    NvU32 flags;
} TMR_EVENT_SET_PARAMS;

/*!
 * Struct to pass to scheduling function.
 *
 * Takes a flag for schedule type based on the above
 * SCHEDULE_FLAGS.
 */
typedef struct TMR_EVENT_SCHEDULE_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pEvent, 8);
    NV_DECLARE_ALIGNED(NvU64 timeNs, 8);
    NvBool bUseTimeAbs;
} TMR_EVENT_SCHEDULE_PARAMS;

/*!
 * Struct to pass to cancel, destroy and
 * get-user-data commands.
 */
typedef struct TMR_EVENT_GENERAL_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pEvent, 8);
    NV_DECLARE_ALIGNED(NvP64 returnVal, 8);
} TMR_EVENT_GENERAL_PARAMS;

/*!
 * Timer object itself
 */
#ifdef NVOC_OBJTMR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJTMR {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct OBJTMR *__nvoc_pbase_OBJTMR;
    void (*__tmrRegisterIntrService__)(OBJGPU *, struct OBJTMR *, IntrServiceRecord *);
    NvBool (*__tmrClearInterrupt__)(OBJGPU *, struct OBJTMR *, IntrServiceClearInterruptArguments *);
    NvU32 (*__tmrServiceInterrupt__)(OBJGPU *, struct OBJTMR *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__tmrConstructEngine__)(OBJGPU *, struct OBJTMR *, ENGDESCRIPTOR);
    NV_STATUS (*__tmrStateInitLocked__)(OBJGPU *, struct OBJTMR *);
    NV_STATUS (*__tmrStateInitUnlocked__)(OBJGPU *, struct OBJTMR *);
    NV_STATUS (*__tmrStateLoad__)(OBJGPU *, struct OBJTMR *, NvU32);
    NV_STATUS (*__tmrStateUnload__)(OBJGPU *, struct OBJTMR *, NvU32);
    void (*__tmrStateDestroy__)(OBJGPU *, struct OBJTMR *);
    NV_STATUS (*__tmrSetCurrentTime__)(OBJGPU *, struct OBJTMR *);
    NvU64 (*__tmrGetTimeEx__)(OBJGPU *, struct OBJTMR *, struct THREAD_STATE_NODE *);
    NV_STATUS (*__tmrSetCountdownIntrDisable__)(OBJGPU *, struct OBJTMR *);
    NV_STATUS (*__tmrSetCountdown__)(OBJGPU *, struct OBJTMR *, NvU32, NvU32, struct THREAD_STATE_NODE *);
    NV_STATUS (*__tmrGrTickFreqChange__)(OBJGPU *, struct OBJTMR *, NvBool);
    NV_STATUS (*__tmrGetGpuPtimerOffset__)(OBJGPU *, struct OBJTMR *, NvU32 *, NvU32 *);
    NV_STATUS (*__tmrServiceNotificationInterrupt__)(OBJGPU *, struct OBJTMR *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__tmrStatePreLoad__)(POBJGPU, struct OBJTMR *, NvU32);
    NV_STATUS (*__tmrStatePostUnload__)(POBJGPU, struct OBJTMR *, NvU32);
    NV_STATUS (*__tmrStatePreUnload__)(POBJGPU, struct OBJTMR *, NvU32);
    void (*__tmrInitMissing__)(POBJGPU, struct OBJTMR *);
    NV_STATUS (*__tmrStatePreInitLocked__)(POBJGPU, struct OBJTMR *);
    NV_STATUS (*__tmrStatePreInitUnlocked__)(POBJGPU, struct OBJTMR *);
    NV_STATUS (*__tmrStatePostLoad__)(POBJGPU, struct OBJTMR *, NvU32);
    NvBool (*__tmrIsPresent__)(POBJGPU, struct OBJTMR *);
    NvBool PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS;
    NvBool PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE;
    NvBool PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS;
    NvBool PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS;
    NvBool PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS;
    NvBool PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL;
    PTMR_EVENT_PVT pRmActiveEventList;
    PTMR_EVENT_PVT pRmActiveOSTimerEventList;
    PTMR_EVENT_PVT pRmCallbackFreeList_OBSOLETE;
    struct TMR_EVENT_PVT rmCallbackTable_OBSOLETE[96];
    POS1HZTIMERENTRY pOs1HzCallbackList;
    POS1HZTIMERENTRY pOs1HzCallbackFreeList;
    struct OS1HZTIMERENTRY os1HzCallbackTable[36];
    struct TMR_EVENT *pOs1HzEvent;
    PORT_SPINLOCK *pTmrSwrlLock;
    TIMEPROC_COUNTDOWN pSwrlCallback;
    NvU32 retryTimes;
    NvU32 errorCount;
    volatile NvS32 tmrChangePending;
    NvBool bInitialized;
    NvBool bAlarmIntrEnabled;
    PENG_INFO_LINK_NODE infoList;
    struct OBJREFCNT *pGrTickFreqRefcnt;
    NvU64 sysTimerOffsetNs;
};

#ifndef __NVOC_CLASS_OBJTMR_TYPEDEF__
#define __NVOC_CLASS_OBJTMR_TYPEDEF__
typedef struct OBJTMR OBJTMR;
#endif /* __NVOC_CLASS_OBJTMR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJTMR
#define __nvoc_class_id_OBJTMR 0x9ddede
#endif /* __nvoc_class_id_OBJTMR */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTMR;

#define __staticCast_OBJTMR(pThis) \
    ((pThis)->__nvoc_pbase_OBJTMR)

#ifdef __nvoc_objtmr_h_disabled
#define __dynamicCast_OBJTMR(pThis) ((OBJTMR*)NULL)
#else //__nvoc_objtmr_h_disabled
#define __dynamicCast_OBJTMR(pThis) \
    ((OBJTMR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJTMR)))
#endif //__nvoc_objtmr_h_disabled

#define PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS_BASE_CAST
#define PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS_BASE_NAME PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS
#define PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS_BASE_CAST
#define PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS_BASE_NAME PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS
#define PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS_BASE_CAST
#define PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS_BASE_NAME PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS
#define PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL_BASE_CAST
#define PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL_BASE_NAME PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL
#define PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS_BASE_CAST
#define PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS_BASE_NAME PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS
#define PDB_PROP_TMR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_TMR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE_BASE_CAST
#define PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE_BASE_NAME PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE

NV_STATUS __nvoc_objCreateDynamic_OBJTMR(OBJTMR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJTMR(OBJTMR**, Dynamic*, NvU32);
#define __objCreate_OBJTMR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJTMR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define tmrRegisterIntrService(pGpu, pTmr, pRecords) tmrRegisterIntrService_DISPATCH(pGpu, pTmr, pRecords)
#define tmrClearInterrupt(pGpu, pTmr, pParams) tmrClearInterrupt_DISPATCH(pGpu, pTmr, pParams)
#define tmrServiceInterrupt(pGpu, pTmr, pParams) tmrServiceInterrupt_DISPATCH(pGpu, pTmr, pParams)
#define tmrServiceInterrupt_HAL(pGpu, pTmr, pParams) tmrServiceInterrupt_DISPATCH(pGpu, pTmr, pParams)
#define tmrConstructEngine(pGpu, pTmr, arg0) tmrConstructEngine_DISPATCH(pGpu, pTmr, arg0)
#define tmrStateInitLocked(pGpu, pTmr) tmrStateInitLocked_DISPATCH(pGpu, pTmr)
#define tmrStateInitUnlocked(pGpu, pTmr) tmrStateInitUnlocked_DISPATCH(pGpu, pTmr)
#define tmrStateLoad(pGpu, pTmr, arg0) tmrStateLoad_DISPATCH(pGpu, pTmr, arg0)
#define tmrStateUnload(pGpu, pTmr, arg0) tmrStateUnload_DISPATCH(pGpu, pTmr, arg0)
#define tmrStateDestroy(pGpu, pTmr) tmrStateDestroy_DISPATCH(pGpu, pTmr)
#define tmrSetCurrentTime(pGpu, pTmr) tmrSetCurrentTime_DISPATCH(pGpu, pTmr)
#define tmrSetCurrentTime_HAL(pGpu, pTmr) tmrSetCurrentTime_DISPATCH(pGpu, pTmr)
#define tmrGetTimeEx(pGpu, pTmr, arg0) tmrGetTimeEx_DISPATCH(pGpu, pTmr, arg0)
#define tmrGetTimeEx_HAL(pGpu, pTmr, arg0) tmrGetTimeEx_DISPATCH(pGpu, pTmr, arg0)
#define tmrSetCountdownIntrDisable(pGpu, pTmr) tmrSetCountdownIntrDisable_DISPATCH(pGpu, pTmr)
#define tmrSetCountdownIntrDisable_HAL(pGpu, pTmr) tmrSetCountdownIntrDisable_DISPATCH(pGpu, pTmr)
#define tmrSetCountdown(pGpu, pTmr, arg0, arg1, arg2) tmrSetCountdown_DISPATCH(pGpu, pTmr, arg0, arg1, arg2)
#define tmrSetCountdown_HAL(pGpu, pTmr, arg0, arg1, arg2) tmrSetCountdown_DISPATCH(pGpu, pTmr, arg0, arg1, arg2)
#define tmrGrTickFreqChange(pGpu, pTmr, arg0) tmrGrTickFreqChange_DISPATCH(pGpu, pTmr, arg0)
#define tmrGrTickFreqChange_HAL(pGpu, pTmr, arg0) tmrGrTickFreqChange_DISPATCH(pGpu, pTmr, arg0)
#define tmrGetGpuPtimerOffset(pGpu, pTmr, arg0, arg1) tmrGetGpuPtimerOffset_DISPATCH(pGpu, pTmr, arg0, arg1)
#define tmrGetGpuPtimerOffset_HAL(pGpu, pTmr, arg0, arg1) tmrGetGpuPtimerOffset_DISPATCH(pGpu, pTmr, arg0, arg1)
#define tmrServiceNotificationInterrupt(pGpu, pIntrService, pParams) tmrServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define tmrStatePreLoad(pGpu, pEngstate, arg0) tmrStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define tmrStatePostUnload(pGpu, pEngstate, arg0) tmrStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define tmrStatePreUnload(pGpu, pEngstate, arg0) tmrStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define tmrInitMissing(pGpu, pEngstate) tmrInitMissing_DISPATCH(pGpu, pEngstate)
#define tmrStatePreInitLocked(pGpu, pEngstate) tmrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define tmrStatePreInitUnlocked(pGpu, pEngstate) tmrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define tmrStatePostLoad(pGpu, pEngstate, arg0) tmrStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define tmrIsPresent(pGpu, pEngstate) tmrIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS tmrGetCurrentTime_IMPL(struct OBJTMR *pTmr, NvU64 *pTime);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetCurrentTime(struct OBJTMR *pTmr, NvU64 *pTime) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetCurrentTime(pTmr, pTime) tmrGetCurrentTime_IMPL(pTmr, pTime)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetCurrentTime_HAL(pTmr, pTime) tmrGetCurrentTime(pTmr, pTime)

NV_STATUS tmrGetCurrentTimeEx_IMPL(struct OBJTMR *pTmr, NvU64 *pTime, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetCurrentTimeEx(struct OBJTMR *pTmr, NvU64 *pTime, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetCurrentTimeEx(pTmr, pTime, arg0) tmrGetCurrentTimeEx_IMPL(pTmr, pTime, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetCurrentTimeEx_HAL(pTmr, pTime, arg0) tmrGetCurrentTimeEx(pTmr, pTime, arg0)

NV_STATUS tmrDelay_OSTIMER(struct OBJTMR *pTmr, NvU32 arg0);

NV_STATUS tmrDelay_PTIMER(struct OBJTMR *pTmr, NvU32 arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrDelay(struct OBJTMR *pTmr, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrDelay(pTmr, arg0) tmrDelay_OSTIMER(pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrDelay_HAL(pTmr, arg0) tmrDelay(pTmr, arg0)

static inline NV_STATUS tmrSetAlarmIntrDisable_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return NV_OK;
}

NV_STATUS tmrSetAlarmIntrDisable_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr);

NV_STATUS tmrSetAlarmIntrDisable_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrSetAlarmIntrDisable_46f6a7(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetAlarmIntrDisable(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetAlarmIntrDisable(pGpu, pTmr) tmrSetAlarmIntrDisable_56cd7a(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetAlarmIntrDisable_HAL(pGpu, pTmr) tmrSetAlarmIntrDisable(pGpu, pTmr)

static inline NV_STATUS tmrSetAlarmIntrEnable_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return NV_OK;
}

NV_STATUS tmrSetAlarmIntrEnable_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr);

NV_STATUS tmrSetAlarmIntrEnable_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrSetAlarmIntrEnable_46f6a7(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetAlarmIntrEnable(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetAlarmIntrEnable(pGpu, pTmr) tmrSetAlarmIntrEnable_56cd7a(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetAlarmIntrEnable_HAL(pGpu, pTmr) tmrSetAlarmIntrEnable(pGpu, pTmr)

static inline NV_STATUS tmrSetAlarmIntrReset_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return NV_OK;
}

NV_STATUS tmrSetAlarmIntrReset_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);

NV_STATUS tmrSetAlarmIntrReset_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);

static inline NV_STATUS tmrSetAlarmIntrReset_46f6a7(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetAlarmIntrReset(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetAlarmIntrReset(pGpu, pTmr, arg0) tmrSetAlarmIntrReset_56cd7a(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetAlarmIntrReset_HAL(pGpu, pTmr, arg0) tmrSetAlarmIntrReset(pGpu, pTmr, arg0)

static inline NV_STATUS tmrGetIntrStatus_cb5ce8(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *pStatus, struct THREAD_STATE_NODE *arg0) {
    *pStatus = 0;
    return NV_OK;
}

NV_STATUS tmrGetIntrStatus_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *pStatus, struct THREAD_STATE_NODE *arg0);

NV_STATUS tmrGetIntrStatus_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *pStatus, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetIntrStatus(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *pStatus, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetIntrStatus(pGpu, pTmr, pStatus, arg0) tmrGetIntrStatus_cb5ce8(pGpu, pTmr, pStatus, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetIntrStatus_HAL(pGpu, pTmr, pStatus, arg0) tmrGetIntrStatus(pGpu, pTmr, pStatus, arg0)

static inline NvU32 tmrGetTimeLo_cf0499(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return ((NvU32)(((NvU64)(osGetTimestamp())) & 4294967295U));
}

NvU32 tmrGetTimeLo_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr);


#ifdef __nvoc_objtmr_h_disabled
static inline NvU32 tmrGetTimeLo(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return 0;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetTimeLo(pGpu, pTmr) tmrGetTimeLo_cf0499(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetTimeLo_HAL(pGpu, pTmr) tmrGetTimeLo(pGpu, pTmr)

static inline NvU64 tmrGetTime_fa6bbe(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return osGetTimestamp();
}

NvU64 tmrGetTime_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr);


#ifdef __nvoc_objtmr_h_disabled
static inline NvU64 tmrGetTime(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return 0;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetTime(pGpu, pTmr) tmrGetTime_fa6bbe(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetTime_HAL(pGpu, pTmr) tmrGetTime(pGpu, pTmr)

NvU32 tmrReadTimeLoReg_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NvU32 tmrReadTimeLoReg(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return 0;
}
#else //__nvoc_objtmr_h_disabled
#define tmrReadTimeLoReg(pGpu, pTmr, arg0) tmrReadTimeLoReg_TU102(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrReadTimeLoReg_HAL(pGpu, pTmr, arg0) tmrReadTimeLoReg(pGpu, pTmr, arg0)

NvU32 tmrReadTimeHiReg_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NvU32 tmrReadTimeHiReg(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return 0;
}
#else //__nvoc_objtmr_h_disabled
#define tmrReadTimeHiReg(pGpu, pTmr, arg0) tmrReadTimeHiReg_TU102(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrReadTimeHiReg_HAL(pGpu, pTmr, arg0) tmrReadTimeHiReg(pGpu, pTmr, arg0)

static inline NV_STATUS tmrSetAlarm_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 alarm, struct THREAD_STATE_NODE *pThreadState) {
    return NV_OK;
}

NV_STATUS tmrSetAlarm_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 alarm, struct THREAD_STATE_NODE *pThreadState);

static inline NV_STATUS tmrSetAlarm_46f6a7(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 alarm, struct THREAD_STATE_NODE *pThreadState) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetAlarm(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 alarm, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetAlarm(pGpu, pTmr, alarm, pThreadState) tmrSetAlarm_56cd7a(pGpu, pTmr, alarm, pThreadState)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetAlarm_HAL(pGpu, pTmr, alarm, pThreadState) tmrSetAlarm(pGpu, pTmr, alarm, pThreadState)

static inline NvBool tmrGetAlarmPending_491d52(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return ((NvBool)(0 != 0));
}

NvBool tmrGetAlarmPending_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);

NvBool tmrGetAlarmPending_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrGetAlarmPending(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetAlarmPending(pGpu, pTmr, arg0) tmrGetAlarmPending_491d52(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetAlarmPending_HAL(pGpu, pTmr, arg0) tmrGetAlarmPending(pGpu, pTmr, arg0)

NV_STATUS tmrSetCountdownIntrEnable_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetCountdownIntrEnable(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetCountdownIntrEnable(pGpu, pTmr) tmrSetCountdownIntrEnable_TU102(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetCountdownIntrEnable_HAL(pGpu, pTmr) tmrSetCountdownIntrEnable(pGpu, pTmr)

NV_STATUS tmrSetCountdownIntrReset_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrSetCountdownIntrReset(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetCountdownIntrReset(pGpu, pTmr, arg0) tmrSetCountdownIntrReset_TU102(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrSetCountdownIntrReset_HAL(pGpu, pTmr, arg0) tmrSetCountdownIntrReset(pGpu, pTmr, arg0)

static inline NvBool tmrGetCountdownPending_491d52(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrGetCountdownPending(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetCountdownPending(pGpu, pTmr, arg0) tmrGetCountdownPending_491d52(pGpu, pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetCountdownPending_HAL(pGpu, pTmr, arg0) tmrGetCountdownPending(pGpu, pTmr, arg0)

NV_STATUS tmrGetTimerBar0MapInfo_PTIMER(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *arg0, NvU32 *arg1);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetTimerBar0MapInfo(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetTimerBar0MapInfo(pGpu, pTmr, arg0, arg1) tmrGetTimerBar0MapInfo_PTIMER(pGpu, pTmr, arg0, arg1)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetTimerBar0MapInfo_HAL(pGpu, pTmr, arg0, arg1) tmrGetTimerBar0MapInfo(pGpu, pTmr, arg0, arg1)

static inline NvU32 tmrGetUtilsClkScaleFactor_4a4dee(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return 0;
}

NvU32 tmrGetUtilsClkScaleFactor_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr);


#ifdef __nvoc_objtmr_h_disabled
static inline NvU32 tmrGetUtilsClkScaleFactor(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return 0;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetUtilsClkScaleFactor(pGpu, pTmr) tmrGetUtilsClkScaleFactor_4a4dee(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetUtilsClkScaleFactor_HAL(pGpu, pTmr) tmrGetUtilsClkScaleFactor(pGpu, pTmr)

NV_STATUS tmrGetGpuAndCpuTimestampPair_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *arg0, NvU64 *arg1);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetGpuAndCpuTimestampPair(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *arg0, NvU64 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetGpuAndCpuTimestampPair(pGpu, pTmr, arg0, arg1) tmrGetGpuAndCpuTimestampPair_GM107(pGpu, pTmr, arg0, arg1)
#endif //__nvoc_objtmr_h_disabled

#define tmrGetGpuAndCpuTimestampPair_HAL(pGpu, pTmr, arg0, arg1) tmrGetGpuAndCpuTimestampPair(pGpu, pTmr, arg0, arg1)

static inline void tmrResetTimerRegistersForVF_b3696a(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 gfid) {
    return;
}

void tmrResetTimerRegistersForVF_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 gfid);

void tmrResetTimerRegistersForVF_GH100(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 gfid);


#ifdef __nvoc_objtmr_h_disabled
static inline void tmrResetTimerRegistersForVF(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrResetTimerRegistersForVF(pGpu, pTmr, gfid) tmrResetTimerRegistersForVF_b3696a(pGpu, pTmr, gfid)
#endif //__nvoc_objtmr_h_disabled

#define tmrResetTimerRegistersForVF_HAL(pGpu, pTmr, gfid) tmrResetTimerRegistersForVF(pGpu, pTmr, gfid)

NV_STATUS tmrEventCreateOSTimer_OSTIMER(struct OBJTMR *pTmr, PTMR_EVENT pEvent);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventCreateOSTimer(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventCreateOSTimer(pTmr, pEvent) tmrEventCreateOSTimer_OSTIMER(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

#define tmrEventCreateOSTimer_HAL(pTmr, pEvent) tmrEventCreateOSTimer(pTmr, pEvent)

NV_STATUS tmrEventScheduleRelOSTimer_OSTIMER(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeRelNs);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventScheduleRelOSTimer(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeRelNs) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventScheduleRelOSTimer(pTmr, pEvent, timeRelNs) tmrEventScheduleRelOSTimer_OSTIMER(pTmr, pEvent, timeRelNs)
#endif //__nvoc_objtmr_h_disabled

#define tmrEventScheduleRelOSTimer_HAL(pTmr, pEvent, timeRelNs) tmrEventScheduleRelOSTimer(pTmr, pEvent, timeRelNs)

NV_STATUS tmrEventServiceOSTimerCallback_OSTIMER(OBJGPU *pGpu, struct OBJTMR *pTmr, PTMR_EVENT pEvent);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventServiceOSTimerCallback(OBJGPU *pGpu, struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventServiceOSTimerCallback(pGpu, pTmr, pEvent) tmrEventServiceOSTimerCallback_OSTIMER(pGpu, pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

#define tmrEventServiceOSTimerCallback_HAL(pGpu, pTmr, pEvent) tmrEventServiceOSTimerCallback(pGpu, pTmr, pEvent)

NV_STATUS tmrEventCancelOSTimer_OSTIMER(struct OBJTMR *pTmr, PTMR_EVENT pEvent);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventCancelOSTimer(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventCancelOSTimer(pTmr, pEvent) tmrEventCancelOSTimer_OSTIMER(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

#define tmrEventCancelOSTimer_HAL(pTmr, pEvent) tmrEventCancelOSTimer(pTmr, pEvent)

NV_STATUS tmrEventDestroyOSTimer_OSTIMER(struct OBJTMR *pTmr, PTMR_EVENT pEvent);


#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventDestroyOSTimer(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventDestroyOSTimer(pTmr, pEvent) tmrEventDestroyOSTimer_OSTIMER(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

#define tmrEventDestroyOSTimer_HAL(pTmr, pEvent) tmrEventDestroyOSTimer(pTmr, pEvent)

void tmrRegisterIntrService_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceRecord pRecords[168]);

static inline void tmrRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceRecord pRecords[168]) {
    pTmr->__tmrRegisterIntrService__(pGpu, pTmr, pRecords);
}

NvBool tmrClearInterrupt_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceClearInterruptArguments *pParams);

static inline NvBool tmrClearInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceClearInterruptArguments *pParams) {
    return pTmr->__tmrClearInterrupt__(pGpu, pTmr, pParams);
}

static inline NvU32 tmrServiceInterrupt_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceServiceInterruptArguments *pParams) {
    return NV_OK;
}

NvU32 tmrServiceInterrupt_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceServiceInterruptArguments *pParams);

NvU32 tmrServiceInterrupt_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceServiceInterruptArguments *pParams);

static inline NvU32 tmrServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, IntrServiceServiceInterruptArguments *pParams) {
    return pTmr->__tmrServiceInterrupt__(pGpu, pTmr, pParams);
}

NV_STATUS tmrConstructEngine_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, ENGDESCRIPTOR arg0);

static inline NV_STATUS tmrConstructEngine_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, ENGDESCRIPTOR arg0) {
    return pTmr->__tmrConstructEngine__(pGpu, pTmr, arg0);
}

NV_STATUS tmrStateInitLocked_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrStateInitLocked_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return pTmr->__tmrStateInitLocked__(pGpu, pTmr);
}

NV_STATUS tmrStateInitUnlocked_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return pTmr->__tmrStateInitUnlocked__(pGpu, pTmr);
}

NV_STATUS tmrStateLoad_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0);

static inline NV_STATUS tmrStateLoad_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0) {
    return pTmr->__tmrStateLoad__(pGpu, pTmr, arg0);
}

NV_STATUS tmrStateUnload_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0);

static inline NV_STATUS tmrStateUnload_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0) {
    return pTmr->__tmrStateUnload__(pGpu, pTmr, arg0);
}

void tmrStateDestroy_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline void tmrStateDestroy_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    pTmr->__tmrStateDestroy__(pGpu, pTmr);
}

NV_STATUS tmrSetCurrentTime_GV100(OBJGPU *pGpu, struct OBJTMR *pTmr);

NV_STATUS tmrSetCurrentTime_GH100(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrSetCurrentTime_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return pTmr->__tmrSetCurrentTime__(pGpu, pTmr);
}

NvU64 tmrGetTimeEx_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);

NvU64 tmrGetTimeEx_GH100(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0);

static inline NvU64 tmrGetTimeEx_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return pTmr->__tmrGetTimeEx__(pGpu, pTmr, arg0);
}

NV_STATUS tmrSetCountdownIntrDisable_GM200(OBJGPU *pGpu, struct OBJTMR *pTmr);

static inline NV_STATUS tmrSetCountdownIntrDisable_56cd7a(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return NV_OK;
}

static inline NV_STATUS tmrSetCountdownIntrDisable_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    return pTmr->__tmrSetCountdownIntrDisable__(pGpu, pTmr);
}

NV_STATUS tmrSetCountdown_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0, NvU32 arg1, struct THREAD_STATE_NODE *arg2);

NV_STATUS tmrSetCountdown_GH100(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0, NvU32 arg1, struct THREAD_STATE_NODE *arg2);

static inline NV_STATUS tmrSetCountdown_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 arg0, NvU32 arg1, struct THREAD_STATE_NODE *arg2) {
    return pTmr->__tmrSetCountdown__(pGpu, pTmr, arg0, arg1, arg2);
}

NV_STATUS tmrGrTickFreqChange_GM107(OBJGPU *pGpu, struct OBJTMR *pTmr, NvBool arg0);

static inline NV_STATUS tmrGrTickFreqChange_46f6a7(OBJGPU *pGpu, struct OBJTMR *pTmr, NvBool arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS tmrGrTickFreqChange_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, NvBool arg0) {
    return pTmr->__tmrGrTickFreqChange__(pGpu, pTmr, arg0);
}

NV_STATUS tmrGetGpuPtimerOffset_TU102(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *arg0, NvU32 *arg1);

NV_STATUS tmrGetGpuPtimerOffset_GA100(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *arg0, NvU32 *arg1);

static inline NV_STATUS tmrGetGpuPtimerOffset_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU32 *arg0, NvU32 *arg1) {
    return pTmr->__tmrGetGpuPtimerOffset__(pGpu, pTmr, arg0, arg1);
}

static inline NV_STATUS tmrServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJTMR *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__tmrServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS tmrStatePreLoad_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return pEngstate->__tmrStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS tmrStatePostUnload_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return pEngstate->__tmrStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS tmrStatePreUnload_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return pEngstate->__tmrStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline void tmrInitMissing_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    pEngstate->__tmrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS tmrStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return pEngstate->__tmrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS tmrStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return pEngstate->__tmrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS tmrStatePostLoad_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return pEngstate->__tmrStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool tmrIsPresent_DISPATCH(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return pEngstate->__tmrIsPresent__(pGpu, pEngstate);
}

static inline NvBool tmrServiceSwrlCallbacksPmcTree(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return ((NvBool)(0 != 0));
}

static inline NvBool tmrClearSwrlCallbacksSemaphore(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return ((NvBool)(0 != 0));
}

static inline void tmrServiceSwrlCallbacks(OBJGPU *pGpu, struct OBJTMR *pTmr, struct THREAD_STATE_NODE *arg0) {
    return;
}

static inline NvBool tmrServiceSwrlWrapper(OBJGPU *pGpu, struct OBJTMR *pTmr, MC_ENGINE_BITVECTOR *arg0, struct THREAD_STATE_NODE *arg1) {
    return ((NvBool)(0 != 0));
}

void tmrDestruct_IMPL(struct OBJTMR *pTmr);

#define __nvoc_tmrDestruct(pTmr) tmrDestruct_IMPL(pTmr)
NV_STATUS tmrEventCreate_IMPL(struct OBJTMR *pTmr, PTMR_EVENT *ppEvent, TIMEPROC callbackFn, void *pUserData, NvU32 flags);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventCreate(struct OBJTMR *pTmr, PTMR_EVENT *ppEvent, TIMEPROC callbackFn, void *pUserData, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventCreate(pTmr, ppEvent, callbackFn, pUserData, flags) tmrEventCreate_IMPL(pTmr, ppEvent, callbackFn, pUserData, flags)
#endif //__nvoc_objtmr_h_disabled

void tmrEventCancel_IMPL(struct OBJTMR *pTmr, PTMR_EVENT pEvent);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrEventCancel(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventCancel(pTmr, pEvent) tmrEventCancel_IMPL(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

void tmrEventDestroy_IMPL(struct OBJTMR *pTmr, PTMR_EVENT pEvent);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrEventDestroy(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventDestroy(pTmr, pEvent) tmrEventDestroy_IMPL(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

void tmrInitCallbacks_IMPL(struct OBJTMR *pTmr);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrInitCallbacks(struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrInitCallbacks(pTmr) tmrInitCallbacks_IMPL(pTmr)
#endif //__nvoc_objtmr_h_disabled

void tmrSetCountdownCallback_IMPL(struct OBJTMR *pTmr, TIMEPROC_COUNTDOWN arg0);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrSetCountdownCallback(struct OBJTMR *pTmr, TIMEPROC_COUNTDOWN arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrSetCountdownCallback(pTmr, arg0) tmrSetCountdownCallback_IMPL(pTmr, arg0)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrCancelCallback_IMPL(struct OBJTMR *pTmr, void *pObject);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrCancelCallback(struct OBJTMR *pTmr, void *pObject) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrCancelCallback(pTmr, pObject) tmrCancelCallback_IMPL(pTmr, pObject)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrGetCurrentDiffTime_IMPL(struct OBJTMR *pTmr, NvU64 arg0, NvU64 *arg1);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrGetCurrentDiffTime(struct OBJTMR *pTmr, NvU64 arg0, NvU64 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetCurrentDiffTime(pTmr, arg0, arg1) tmrGetCurrentDiffTime_IMPL(pTmr, arg0, arg1)
#endif //__nvoc_objtmr_h_disabled

void tmrGetSystemTime_IMPL(struct OBJTMR *pTmr, PDAYMSECTIME pTime);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrGetSystemTime(struct OBJTMR *pTmr, PDAYMSECTIME pTime) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetSystemTime(pTmr, pTime) tmrGetSystemTime_IMPL(pTmr, pTime)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrCheckCallbacksReleaseSem_IMPL(struct OBJTMR *pTmr, NvU32 chId);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrCheckCallbacksReleaseSem(struct OBJTMR *pTmr, NvU32 chId) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrCheckCallbacksReleaseSem(pTmr, chId) tmrCheckCallbacksReleaseSem_IMPL(pTmr, chId)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrDiffExceedsTime_IMPL(struct OBJTMR *pTmr, PDAYMSECTIME pFutureTime, PDAYMSECTIME pPastTime, NvU32 time);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrDiffExceedsTime(struct OBJTMR *pTmr, PDAYMSECTIME pFutureTime, PDAYMSECTIME pPastTime, NvU32 time) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrDiffExceedsTime(pTmr, pFutureTime, pPastTime, time) tmrDiffExceedsTime_IMPL(pTmr, pFutureTime, pPastTime, time)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrEventScheduleAbs_IMPL(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeAbs);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventScheduleAbs(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeAbs) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventScheduleAbs(pTmr, pEvent, timeAbs) tmrEventScheduleAbs_IMPL(pTmr, pEvent, timeAbs)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrScheduleCallbackAbs_IMPL(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU64 arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrScheduleCallbackAbs(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU64 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrScheduleCallbackAbs(pTmr, arg0, arg1, arg2, arg3, arg4) tmrScheduleCallbackAbs_IMPL(pTmr, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrEventScheduleRel_IMPL(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeRel);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventScheduleRel(struct OBJTMR *pTmr, PTMR_EVENT pEvent, NvU64 timeRel) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventScheduleRel(pTmr, pEvent, timeRel) tmrEventScheduleRel_IMPL(pTmr, pEvent, timeRel)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrScheduleCallbackRel_IMPL(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU64 arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrScheduleCallbackRel(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU64 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrScheduleCallbackRel(pTmr, arg0, arg1, arg2, arg3, arg4) tmrScheduleCallbackRel_IMPL(pTmr, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrScheduleCallbackRelSec_IMPL(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrScheduleCallbackRelSec(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrScheduleCallbackRelSec(pTmr, arg0, arg1, arg2, arg3, arg4) tmrScheduleCallbackRelSec_IMPL(pTmr, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrEventOnList_IMPL(struct OBJTMR *pTmr, PTMR_EVENT pEvent);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrEventOnList(struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventOnList(pTmr, pEvent) tmrEventOnList_IMPL(pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrEventServiceTimer_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, PTMR_EVENT pEvent);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrEventServiceTimer(OBJGPU *pGpu, struct OBJTMR *pTmr, PTMR_EVENT pEvent) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrEventServiceTimer(pGpu, pTmr, pEvent) tmrEventServiceTimer_IMPL(pGpu, pTmr, pEvent)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrCallbackOnList_IMPL(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrCallbackOnList(struct OBJTMR *pTmr, TIMEPROC_OBSOLETE arg0, void *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrCallbackOnList(pTmr, arg0, arg1) tmrCallbackOnList_IMPL(pTmr, arg0, arg1)
#endif //__nvoc_objtmr_h_disabled

void tmrRmCallbackIntrEnable_IMPL(struct OBJTMR *pTmr, OBJGPU *pGpu);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrRmCallbackIntrEnable(struct OBJTMR *pTmr, OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrRmCallbackIntrEnable(pTmr, pGpu) tmrRmCallbackIntrEnable_IMPL(pTmr, pGpu)
#endif //__nvoc_objtmr_h_disabled

void tmrRmCallbackIntrDisable_IMPL(struct OBJTMR *pTmr, OBJGPU *pGpu);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrRmCallbackIntrDisable(struct OBJTMR *pTmr, OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrRmCallbackIntrDisable(pTmr, pGpu) tmrRmCallbackIntrDisable_IMPL(pTmr, pGpu)
#endif //__nvoc_objtmr_h_disabled

NV_STATUS tmrTimeUntilNextCallback_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *pTimeUntilCallbackNs);

#ifdef __nvoc_objtmr_h_disabled
static inline NV_STATUS tmrTimeUntilNextCallback(OBJGPU *pGpu, struct OBJTMR *pTmr, NvU64 *pTimeUntilCallbackNs) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_objtmr_h_disabled
#define tmrTimeUntilNextCallback(pGpu, pTmr, pTimeUntilCallbackNs) tmrTimeUntilNextCallback_IMPL(pGpu, pTmr, pTimeUntilCallbackNs)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrCallExpiredCallbacks_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrCallExpiredCallbacks(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrCallExpiredCallbacks(pGpu, pTmr) tmrCallExpiredCallbacks_IMPL(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

void tmrResetCallbackInterrupt_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

#ifdef __nvoc_objtmr_h_disabled
static inline void tmrResetCallbackInterrupt(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
}
#else //__nvoc_objtmr_h_disabled
#define tmrResetCallbackInterrupt(pGpu, pTmr) tmrResetCallbackInterrupt_IMPL(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

NvBool tmrGetCallbackInterruptPending_IMPL(OBJGPU *pGpu, struct OBJTMR *pTmr);

#ifdef __nvoc_objtmr_h_disabled
static inline NvBool tmrGetCallbackInterruptPending(OBJGPU *pGpu, struct OBJTMR *pTmr) {
    NV_ASSERT_FAILED_PRECOMP("OBJTMR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_objtmr_h_disabled
#define tmrGetCallbackInterruptPending(pGpu, pTmr) tmrGetCallbackInterruptPending_IMPL(pGpu, pTmr)
#endif //__nvoc_objtmr_h_disabled

#undef PRIVATE_FIELD


NV_STATUS tmrCtrlCmdEventCreate(OBJGPU *pGpu, TMR_EVENT_SET_PARAMS *pParams);
NV_STATUS tmrCtrlCmdEventSchedule(OBJGPU *pGpu, TMR_EVENT_SCHEDULE_PARAMS *pParams);
NV_STATUS tmrCtrlCmdEventCancel(OBJGPU *pGpu, TMR_EVENT_GENERAL_PARAMS *pParams);
NV_STATUS tmrCtrlCmdEventDestroy(OBJGPU *pGpu, TMR_EVENT_GENERAL_PARAMS *pParams);

#endif // _OBJTMR_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJTMR_NVOC_H_
