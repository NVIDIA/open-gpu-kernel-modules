/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _GPU_TIMEOUT_H_
#define _GPU_TIMEOUT_H_

/* ------------------------ Includes ---------------------------------------- */
#include "core/core.h"


/* ------------------------ Forward Definitions ----------------------------- */
struct OBJGPU;

/* ------------------------ Macros ------------------------------------------ */
/*!
 * @note    GPU_TIMEOUT_DEFAULT is different per platform and can range anywhere
 *          from 2 to 30 secs depending on the GPU Mode and Platform.
 *          By default if GPU_TIMEOUT_DEFAULT is specified, we use the ThreadState
 *          unless explicitly told not to via GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE.
 */
#define GPU_TIMEOUT_DEFAULT                 0

/*!
 * gpuSetTimeout Flags - saved in pTimeout->flags
 */
#define GPU_TIMEOUT_FLAGS_DEFAULT               NVBIT(0)  //!< default timeout mechanism as set by platform
#define GPU_TIMEOUT_FLAGS_USE_THREAD_STATE      NVBIT(1)  //!< default timeout time used - use the ThreadState
#define GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE   NVBIT(2)  //!< even if default time was used - skip the ThreadState
#define GPU_TIMEOUT_FLAGS_OSTIMER               NVBIT(3)  //!< osGetCurrentTime()
#define GPU_TIMEOUT_FLAGS_OSDELAY               NVBIT(4)  //!< osDelay()
#define GPU_TIMEOUT_FLAGS_TMR                   NVBIT(5)  //!< tmrGetCurrentTime()
#define GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG    NVBIT(6)  //!< bypass timeout logging in the RM journal
#define GPU_TIMEOUT_FLAGS_TMRDELAY              NVBIT(7)  //!< tmrDelay()
#define GPU_TIMEOUT_FLAGS_BYPASS_CPU_YIELD      NVBIT(8)  //!< don't explicitly let other threads run first
/*!
 * gpuCheckTimeout Flags set in pTimeout->flags upon NV_ERR_TIMEOUT
 */
#define GPU_TIMEOUT_FLAGS_STATUS_LOCAL_TIMEOUT          NVBIT(30)
#define GPU_TIMEOUT_FLAGS_STATUS_THREAD_STATE_TIMEOUT   NVBIT(31)

/* ------------------------ Datatypes --------------------------------------- */
/*!
 * Timeout support.
 */
typedef struct
{
    NvU64   timeout;
    NvU32   flags;
    OBJGPU *pTmrGpu;        //!< The GPU whose timer is used in SLI mode
                            //   Defined only if flags is set to _TMR or _TMRDELAY
} RMTIMEOUT,
*PRMTIMEOUT;

/*!
 * @brief   GPU timeout related data.
 */
typedef struct
{
    volatile NvBool bDefaultOverridden;
    volatile NvBool bScaled;
    volatile NvU32  defaultus;          //!< Default timeout in us
    volatile NvU32  defaultResetus;     //!< Default timeout reset value in us
    NvU32           defaultFlags;       //!< Default timeout mode
    NvU32           scale;              //!< Emulation/Simulation multiplier
    OBJGPU         *pGpu;
} TIMEOUT_DATA;

/*!
 * @brief   A prototype of the condition evaluation function required by the
 *          @ref gpuTimeoutCondWait_IMPL interface.
 *
 * @note    Function is responsible for evaluation of the encapsulated condition
 *          as well as for triggering of required prerequisites (if any).
 *          For example if condition depends on a PMU issued message function
 *          should assure proper servicing of the PMU interrupts.
 *
 * @param[in] pGpu    OBJGPU pointer for this conditional function
 * @param[in] pVoid
 *     Void parameter pointer which can be used to pass in the
 *     pCondData from @ref gpuTimeoutCondWait_IMPL().
 *
 * @return NV_TRUE
 *     Waited condition has happened and @ref
 *     gpuTimeoutCondWait_IMPL() may return to caller.
 * @return NV_FALSE
 *     Waited condition has not happened and @ref
 *     gpuTimeoutCondWait_IMPL() should continue to wait until this
 *     interface returns NV_TRUE or timeout occurs (whichever occurs
 *     first).
 */
typedef NvBool GpuWaitConditionFunc(OBJGPU *pGpu, void *pVoid);

/* ------------------------ Function Prototypes ----------------------------- */

void timeoutInitializeGpuDefault(TIMEOUT_DATA *pTD, OBJGPU *pGpu);

void timeoutRegistryOverride(TIMEOUT_DATA *pTD, OBJGPU *pGpu);

void timeoutOverride(TIMEOUT_DATA *pTD, NvBool bOverride, NvU32 timeoutMs);

/*! Initialize the RMTIMEOUT structure with the selected timeout scheme. */
void timeoutSet(TIMEOUT_DATA *, RMTIMEOUT *, NvU32 timeoutUs, NvU32 flags);

/*! Check if the passed in RMTIMEOUT struct has expired. */
NV_STATUS timeoutCheck(TIMEOUT_DATA *, RMTIMEOUT *, NvU32);

/*! Wait for the condition to become satisfied while checking for the timeout */
NV_STATUS timeoutCondWait(TIMEOUT_DATA *, RMTIMEOUT *, GpuWaitConditionFunc *, void *pCondData, NvU32);

/*! Scales timeout values depending on the environment we are running in. */
static NV_INLINE NvU32 timeoutApplyScale(TIMEOUT_DATA *pTD, NvU32 timeout)
{
    return timeout * pTD->scale;
}


// Deprecated macros
#define gpuSetTimeout(g,a,t,c)       timeoutSet(&(g)->timeoutData, t, a, c)
#define gpuCheckTimeout(g,t)         timeoutCheck(&(g)->timeoutData, t, __LINE__)
#define gpuScaleTimeout(g,a)         timeoutApplyScale(&(g)->timeoutData, a)
#define gpuTimeoutCondWait(g,a,b,t)  timeoutCondWait(&(g)->timeoutData, t, a, b, __LINE__)

//
// In SCSIM simulation platform, both CPU and GPU are simulated and the reg write/read itself
// takes more time. This helper macro handles it with increased timeout value.
//
#define GPU_ENG_RESET_TIMEOUT_VALUE(g, t) ((gpuIsSelfHosted(g) && IS_SIMULATION(g)) ? 1000 : (t))

#endif // _GPU_TIMEOUT_H_
