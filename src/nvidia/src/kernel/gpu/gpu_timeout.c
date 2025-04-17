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

/*!
 * @file
 * @brief   GPU Timeout related routines.
 */

/* ------------------------ Includes ---------------------------------------- */

#include "lib/base_utils.h"
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "nvrm_registry.h"
#include "core/thread_state.h"
#include "core/locks.h"
#include "gpu_mgr/gpu_mgr.h"

/* ------------------------ Public Functions  ------------------------------- */

/*!
 * @brief   Initializes default timeout values from a provided GPU.
 */
void
timeoutInitializeGpuDefault
(
    TIMEOUT_DATA *pTD,
    OBJGPU *pGpu
)
{
    NvU32               timeoutDefault;

    pTD->pGpu = pGpu;

    // Set default timeout mode before loading HAL state
    osGetTimeoutParams(pGpu, &timeoutDefault, &(pTD->scale), &(pTD->defaultFlags));
    if (!pTD->bDefaultOverridden)
    {
        pTD->defaultResetus = timeoutDefault;
        pTD->defaultus = timeoutDefault;
        pTD->bScaled = NV_FALSE;
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        //
        // vGPU:
        //
        // Since vGPU does all real hardware management in the host, use the OS
        // timer by default in the guest OS (where IS_VIRTUAL(pGpu) is true),
        // as it (hopefully) tracks a VM's actual time executing
        // (vs. reading the HW PTIMER which'll be too fast).
        // SOC NvDisplay:
        // SOC NvDisplay doesn't have HW timer so use OSTIMER as default
        //
        pTD->defaultFlags = GPU_TIMEOUT_FLAGS_OSTIMER;
    }

    // Using this boolean to ensure defaultus isn't scaled more than once.
    if (!pTD->bScaled)
    {
        pTD->defaultus = gpuScaleTimeout(pGpu, pTD->defaultus);
        pTD->bScaled = NV_TRUE;
    }

    //
    // Note we need to call threadStateResetTimeout() now that the timeout
    // mechanism and values are known to allow threadStateCheckTimeout()
    // to work after this point during init.
    //
    threadStateInitTimeout(pGpu, pTD->defaultus, pTD->defaultFlags);
    threadStateResetTimeout(pGpu);
}

/*!
 * @brief   Applies external timeout override based on registry values.
 */
void
timeoutRegistryOverride
(
    TIMEOUT_DATA *pTD,
    OBJGPU *pGpu
)
{
    NvU32 data32 = 0;

    // Override timeout value
    if ((osReadRegistryDword(pGpu,
                             NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT,
                             &data32) == NV_OK) && (data32 != 0))
    {
        // Handle 32-bit overflow.
        if (data32 > (NV_U32_MAX / 1000))
        {
            pTD->defaultus = NV_U32_MAX;
            pTD->defaultResetus = NV_U32_MAX;
        }
        else
        {
            // Convert to [us]
            pTD->defaultus = data32 * 1000;
            pTD->defaultResetus = data32 * 1000;
        }
        pTD->bDefaultOverridden = NV_TRUE;
        NV_PRINTF(LEVEL_ERROR, "Overriding default timeout to 0x%08x\n",
                  pTD->defaultus);
    }

    // Override timeout flag values
    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS,
                            &data32) == NV_OK)
    {
        switch (data32)
        {
            case NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS_OSDELAY:
            {
                pTD->defaultFlags = GPU_TIMEOUT_FLAGS_OSDELAY;
                break;
            }

            case NV_REG_STR_RM_OVERRIDE_DEFAULT_TIMEOUT_FLAGS_OSTIMER:
            {
                pTD->defaultFlags = GPU_TIMEOUT_FLAGS_OSTIMER;
                break;
            }

            default:
            {
                 NV_PRINTF(LEVEL_ERROR, "Unknown TIMEOUT_FLAGS value: 0x%08x\n",
                           data32);
                 NV_ASSERT(0);
            }
        }

        NV_PRINTF(LEVEL_ERROR, "Overriding default flags to 0x%08x\n",
                  pTD->defaultFlags);
    }
}

/*!
 * @brief   Applies external timeout override.
 */
void
timeoutOverride
(
    TIMEOUT_DATA *pTD,
    NvBool  bOverride,
    NvU32   timeoutMs
)
{
    pTD->bDefaultOverridden = bOverride;

    pTD->defaultus = bOverride ? (timeoutMs * 1000) : pTD->defaultResetus;
}

/*!
 * @brief   Initialize the RMTIMEOUT structure with the selected timeout scheme.
 */
void
timeoutSet
(
    TIMEOUT_DATA *pTD,
    RMTIMEOUT    *pTimeout,
    NvU32         timeoutUs,
    NvU32         flags
)
{
    OBJTMR             *pTmr;
    NvU64               timeInNs;
    NvU64               timeoutNs;

    portMemSet(pTimeout, 0, sizeof(*pTimeout));

    //
    // Note that if GPU_TIMEOUT_DEFAULT is used we will go through
    // threadStateCheckTimeout rather than timeoutCheck as we do
    // not want to have "stacked" gpuSetTimeouts.  The intent of
    // GPU_TIMEOUT_DEFAULT was to cover the *entire* RM API stack.
    // If GPU_TIMEOUT_DEFAULT was specified, this is essentially a
    // NULL operation other than setting the flags to route us to
    // threadStateCheckTimeout.  This can be overridden by
    // setting GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE.
    //
    pTimeout->flags = flags;
    if ((flags == 0) || (flags & GPU_TIMEOUT_FLAGS_DEFAULT) ||
        !(flags & (GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_OSDELAY |
                   GPU_TIMEOUT_FLAGS_TMR | GPU_TIMEOUT_FLAGS_TMRDELAY)))
    {
        pTimeout->flags |= pTD->defaultFlags;
    }

    if (timeoutUs == GPU_TIMEOUT_DEFAULT)
    {
        timeoutUs = pTD->defaultus;

        //
        // Use the ThreadState by default if GPU_TIMEOUT_DEFAULT was specified
        // unless we were told explicitly not to.
        // ThreadState only supports OSTIMER and OSDELAY
        //
        if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE) &&
             (pTimeout->flags & (GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_OSDELAY)))
        {
            pTimeout->flags |= GPU_TIMEOUT_FLAGS_USE_THREAD_STATE;
        }
    }

    // Set end time for elapsed time methods
    timeoutNs = (NvU64)timeoutUs * 1000;
    if (pTimeout->flags & GPU_TIMEOUT_FLAGS_OSTIMER)
    {
        //
        // For small timeouts (timeout durations on the order of magnitude of
        // the OS tick resolution), starting the timeout near the end of a tick
        // could cause a premature timeout since the start time is determined
        // by the start of the tick. Mitigate this by always padding the
        // timeout using the OS tick resolution, to bump us to the next tick.
        //
        timeoutNs += osGetTickResolution();

        timeInNs = osGetCurrentTick();

        pTimeout->pTmrGpu = NULL;
        pTimeout->timeout = timeInNs + timeoutNs;
    }
    else if ((pTimeout->flags & GPU_TIMEOUT_FLAGS_TMR) ||
             (pTimeout->flags & GPU_TIMEOUT_FLAGS_TMRDELAY))
    {
        OBJGPU *pGpu = pTD->pGpu;
        NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

        OBJGPU *pParentGpu = gpumgrGetParentGPU(pGpu);

        //
        // Set timer GPU to primary GPU for accurate timeout with SLI loop. But only
        // use the primary GPU if it is in full power mode or in the process of resuming.
        // Also don't use the primary if it is in full chip reset.
        //
        if (gpumgrIsParentGPU(pGpu) ||
            ((gpuIsGpuFullPower(pParentGpu) == NV_FALSE) &&
             !pParentGpu->getProperty(pParentGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH)) ||
            pParentGpu->getProperty(pParentGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET))
        {
            pTimeout->pTmrGpu = pGpu;
        }
        else
        {
            pTimeout->pTmrGpu = pParentGpu;
        }

        pTmr = GPU_GET_TIMER(pTimeout->pTmrGpu);
        NV_ASSERT_OR_RETURN_VOID(pTmr != NULL);

        if (pTimeout->flags & GPU_TIMEOUT_FLAGS_TMR)
        {

            // nanoseconds
            tmrGetCurrentTime(pTmr, &pTimeout->timeout);
            pTimeout->timeout += timeoutNs;
        }
        else // GPU_TIMEOUT_FLAGS_TMRDELAY
        {
            pTimeout->timeout = timeoutUs;
        }
    }
    else
    {
        pTimeout->pTmrGpu = NULL;
        pTimeout->timeout = timeoutUs;
    }
}

/*!
 * We typically only use this code if a time other than GPU_TIMEOUT_DEFAULT
 * was specified. For GPU_TIMEOUT_DEFAULT we use threadStateCheckTimeout.
 * The logic in the _threadNodeCheckTimeout() should closely resemble that
 * of the _checkTimeout().
 */
static NV_STATUS
_checkTimeout
(
    RMTIMEOUT  *pTimeout
)
{
    NV_STATUS status = NV_OK;
    OBJTMR   *pTmr;
    NvU64     current;
    NvU64     timeInNs;

    if (pTimeout->flags & GPU_TIMEOUT_FLAGS_OSTIMER)
    {
        timeInNs = osGetCurrentTick();
        if (timeInNs >= pTimeout->timeout)
        {
            if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG))
            {
                NV_PRINTF(LEVEL_INFO, "OS elapsed %llx >= %llx\n",
                          timeInNs, pTimeout->timeout);
            }
            status = NV_ERR_TIMEOUT;
        }
    }
    else if (pTimeout->flags & GPU_TIMEOUT_FLAGS_OSDELAY)
    {
        osDelayUs(100);

        //
        // TODO: Bug: 3312158 - Isolate the fix timeout logic to emulation.
        // This is because of the numerous timeout issues exposed in DVS
        // Emulation requires this to make sure we are not wasting emulation resources
        // by waiting for timeouts too long.
        // Once DVS issues are fixed, this fix will be enabled for all platforms.
        //
        if ((pTimeout->pTmrGpu != NULL) && (IS_EMULATION(pTimeout->pTmrGpu)))
        {
            //
            // Adjust the remaining time. Note that the remaining time is in nanoseconds unit
            // for GPU_TIMEOUT_FLAGS_OSDELAY
            //
            pTimeout->timeout -= NV_MIN(100ULL * 1000ULL, pTimeout->timeout);
        }
        else
        {
            pTimeout->timeout -= NV_MIN(100ULL , pTimeout->timeout);
        }

        if (pTimeout->timeout == 0)
        {
            if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG))
            {
                NV_PRINTF(LEVEL_INFO, "OS timeout == 0\n");
            }
            status =  NV_ERR_TIMEOUT;
        }
    }
    else if (pTimeout->flags & GPU_TIMEOUT_FLAGS_TMR)
    {
        NV_ASSERT_OR_RETURN(pTimeout->pTmrGpu != NULL, NV_ERR_INVALID_STATE);
        if (!API_GPU_ATTACHED_SANITY_CHECK(pTimeout->pTmrGpu))
            return NV_ERR_TIMEOUT;

        pTmr = GPU_GET_TIMER(pTimeout->pTmrGpu);
        NV_ASSERT_OR_RETURN(pTmr != NULL, NV_ERR_INVALID_STATE);

        tmrDelay(pTmr, 5ULL * 1000ULL);
        tmrGetCurrentTime(pTmr, &current);

        if (current >= pTimeout->timeout)
        {
            if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG))
            {
                NV_PRINTF(LEVEL_ERROR, "ptmr elapsed %llx >= %llx\n",
                          current, pTimeout->timeout);
            }
            status =  NV_ERR_TIMEOUT;
        }
    }
    else if (pTimeout->flags & GPU_TIMEOUT_FLAGS_TMRDELAY)
    {
        NV_ASSERT_OR_RETURN(pTimeout->pTmrGpu != NULL, NV_ERR_INVALID_STATE);
        if (!API_GPU_ATTACHED_SANITY_CHECK(pTimeout->pTmrGpu))
            return NV_ERR_TIMEOUT;

        pTmr = GPU_GET_TIMER(pTimeout->pTmrGpu);
        NV_ASSERT_OR_RETURN(pTmr != NULL, NV_ERR_INVALID_STATE);

        tmrDelay(pTmr, 5ULL * 1000ULL);
        pTimeout->timeout -= NV_MIN(5, pTimeout->timeout);

        if (pTimeout->timeout == 0)
        {
            if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG))
            {
                NV_PRINTF(LEVEL_INFO, "ptmr timeout == 0\n");
            }
            status =  NV_ERR_TIMEOUT;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid timeout flags 0x%08x\n",
                  pTimeout->flags);
        DBG_BREAKPOINT();
        status = NV_ERR_INVALID_STATE;
    }

    return status;
}

/*!
 * @brief   Check if the passed in RMTIMEOUT struct has expired.
 */
NV_STATUS
timeoutCheck
(
    TIMEOUT_DATA *pTD,
    RMTIMEOUT    *pTimeout,
    NvU32         lineNum
)
{
    OBJGPU    *pGpu = pTD->pGpu;
    NV_STATUS  status = NV_OK;

    NV_ASSERT(pTimeout != NULL);

    if ((pGpu != NULL) && API_GPU_IN_RESET_SANITY_CHECK(pGpu))
        return NV_ERR_TIMEOUT;

    if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_CPU_YIELD))
    {
        threadStateYieldCpuIfNecessary(pGpu, !!(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG));
    }

    //
    // Note that if GPU_TIMEOUT_DEFAULT is used we will go through
    // threadStateCheckTimeout rather than timeoutCheck as we do
    // not want to have "stacked" gpuSetTimeouts.  The intent of
    // GPU_TIMEOUT_DEFAULT is to cover the *entire* RM API stack.
    // If we are going through the case below, we should have just
    // called threadStateCheckTimeout directly rather than
    // timeoutCheck.
    //

    // If local timeout check was intended, check that first.
    if (!(pTimeout->flags & GPU_TIMEOUT_FLAGS_USE_THREAD_STATE))
    {
        status = _checkTimeout(pTimeout);
        if (status == NV_ERR_TIMEOUT)
        {
            // Mark that this Timeout is the result of a local timeout
            pTimeout->flags |= GPU_TIMEOUT_FLAGS_STATUS_LOCAL_TIMEOUT;
        }
    }

    //
    // Always check for the thread timeout in addition to any local timeout
    // unless we have EXPLICITLY been instructed not to by a timeout flag.
    //
    if ((status != NV_ERR_TIMEOUT) && !(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE))
    {
        status = threadStateCheckTimeout(pGpu, NULL /*pElapsedTime*/);

        if (status == NV_ERR_TIMEOUT)
        {
            // Mark that this Timeout is the result of ThreadState
            pTimeout->flags |= GPU_TIMEOUT_FLAGS_STATUS_THREAD_STATE_TIMEOUT;
        }
        else if (status != NV_OK)
        {
            // Try the local timeout as fallback, unless it was already checked.
            if (pTimeout->flags & GPU_TIMEOUT_FLAGS_USE_THREAD_STATE)
            {
                status = _checkTimeout(pTimeout);
                if (status == NV_ERR_TIMEOUT)
                {
                    // Mark that this Timeout is the result of a local timeout
                    pTimeout->flags |= GPU_TIMEOUT_FLAGS_STATUS_LOCAL_TIMEOUT;
                }
            }
        }
    }

    // Throttle priority of boosted threads if necessary
    threadPriorityThrottle();

    // Log the Timeout in the RM Journal
    if ( (status == NV_ERR_TIMEOUT) &&
        !(pTimeout->flags & GPU_TIMEOUT_FLAGS_BYPASS_JOURNAL_LOG))
    {
        NvU64 funcAddr = (NvU64) (NV_RETURN_ADDRESS());
        threadStateLogTimeout(pGpu, funcAddr, lineNum);
    }

    return status;
}

/*!
 * @brief   Wait for a condition function to return NV_TRUE or timeout.
 *
 * @param[in]   pTD         Timeout data
 * @param[in]   pTimeout    RM timeout structure to be used, or NULL to use default timeout
 * @param[in]   pCondFunc   Function implementing condition check to wait for
 * @param[in]   pCondData   An optional param to @ref pCondFunc (NULL if unused)
 *
 * @return  NV_OK           Condition met within the provided timeout period.
 * @return  NV_ERR_TIMEOUT  Timed out while waiting for the condition.
 *
 * @note    This interface addresses the recurring problem of reporting time-out
 *          when condition is actually met. That can happen since RM can get
 *          preempted by the OS any time during the execution. It is achieved by
 *          one additional condition check before the exit in case when timeout
 *          has been detected.
 */
NV_STATUS
timeoutCondWait
(
    TIMEOUT_DATA           *pTD,
    RMTIMEOUT              *pTimeout,
    GpuWaitConditionFunc   *pCondFunc,
    void                   *pCondData,
    NvU32                   lineNum
)
{
    OBJGPU    *pGpu = pTD->pGpu;
    NV_STATUS  status = NV_OK;
    RMTIMEOUT  timeout;

    if (pTimeout == NULL)
    {
        timeoutSet(pTD, &timeout, GPU_TIMEOUT_DEFAULT, 0);
        pTimeout = &timeout;
    }

    while (!pCondFunc(pGpu, pCondData))
    {
        osSpinLoop();

        status = timeoutCheck(pTD, pTimeout, lineNum);
        if (status != NV_OK)
        {
            if ((status == NV_ERR_TIMEOUT) &&
                pCondFunc(pGpu, pCondData))
            {
                status = NV_OK;
            }
            break;
        }
    }

    return status;
}
