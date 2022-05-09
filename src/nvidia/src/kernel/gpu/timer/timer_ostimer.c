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

/********************* Non-Chip Specific HAL TMR Routines ******************\
*                                                                           *
*   This file contains TMR method implementations using OSTIMER             *
*                                                                           *
\***************************************************************************/

#include "objtmr.h"

//
// This function returns current time from OS timer
//
NvU64
tmrGetTimeEx_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU32   seconds;                // Time since 1970 in seconds
    NvU32   useconds;               //  and uSeconds.
    NvU64 timeNs; // Time since 1970 in ns.

    //
    // Get current time from operating system.
    //
    // We get the time in seconds and microseconds since 1970
    // Note that we don't really need the real time of day
    //
    osGetCurrentTime(&seconds, &useconds);

    //
    // Calculate ns since 1970.
    //
    timeNs = ((NvU64)seconds * 1000000 + useconds) * 1000;

    return timeNs;
}

/*!
 *  Creates OS timer event
 *
 *  @param[in]  pTmr Pointer to Timer Object
 *  @param[in]  pEvent pointer to timer event information
 *  @param[out]  NV_STATUS
 */
NV_STATUS tmrEventCreateOSTimer_OSTIMER
(
    OBJTMR     *pTmr,
    PTMR_EVENT  pEventPublic
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    PTMR_EVENT_PVT pEvent = (PTMR_EVENT_PVT)pEventPublic;

    status = osCreateNanoTimer(pGpu->pOsGpuInfo, pEvent, &(pEvent->super.pOSTmrCBdata));

    if (status != NV_OK)
    {
        pEvent->super.pOSTmrCBdata = NULL;
        NV_PRINTF(LEVEL_ERROR, "OS create timer failed\n");
    }

    return status;
}

/*!
 * This function Starts or Schedules OS Timer
 *
 *   @param[in]  pTmr Pointer to Timer Object
 *   @param[in]  pEvent pointer to timer event information
 *   @param[in]  absolute time in nano seconds
 *
 *  @returns  NV_ERR_INVALID_REQUEST failed to create timer
*/
NV_STATUS tmrEventScheduleAbsOSTimer_OSTIMER
(
    OBJTMR     *pTmr,
    PTMR_EVENT  pPublicEvent,
    NvU64       timeNs
)
{
    NV_STATUS status= NV_OK;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    PTMR_EVENT_PVT pEvent = (PTMR_EVENT_PVT) pPublicEvent;

    if (pEvent->super.pOSTmrCBdata == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "OS Timer not created\n");
        return NV_ERR_INVALID_REQUEST;
    }

    status = osStartNanoTimer(pGpu->pOsGpuInfo, pEvent->super.pOSTmrCBdata, timeNs);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "OS Start timer FAILED!\n");
    }

    pEvent->super.flags |= TMR_FLAG_OS_TIMER_QUEUED;
    return status;
}

/*!
 * This function runs OS timer callback
 *
*   @param[in]  pGpu Pointer to GPU object
*   @param[in]  pTmr Pointer to Timer Object
*   @param[in]  pEvent pointer to timer event information
*
 *  @returns  NV_ERR_INVALID_REQUEST if callback not found
 */
NV_STATUS tmrEventServiceOSTimerCallback_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    PTMR_EVENT          pPublicEvent
)
{
    PTMR_EVENT_PVT  pEvent = (PTMR_EVENT_PVT)pPublicEvent;
    NV_STATUS status = NV_OK;

    if (pEvent && (pEvent->super.pTimeProc != NULL))
    {
        pEvent->super.pTimeProc(pGpu, pTmr, (PTMR_EVENT)pEvent);
        pEvent->super.flags &= ~TMR_FLAG_OS_TIMER_QUEUED;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR No Timer event callback found, invalid timer SW state\n");
        status = NV_ERR_INVALID_REQUEST;
    }

    return status;
}

/*!
 * This function cancels OS timer callback
 *
 *   @param[in]  pTmr Pointer to Timer Object
 *   @param[in]  pEvent pointer to timer event information
 *   @returns  NV_ERR_INVALID_REQUEST if callback entry not found
 */
NV_STATUS tmrEventCancelOSTimer_OSTIMER
(
    OBJTMR             *pTmr,
    PTMR_EVENT          pPublicEvent
)
{
    NV_STATUS status= NV_OK;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    PTMR_EVENT_PVT  pTmrEvent = (PTMR_EVENT_PVT) pPublicEvent;

    if (pTmrEvent != NULL && pTmrEvent->super.pOSTmrCBdata != NULL)
    {
        // Cancel the callback of OS timer
        status = osCancelNanoTimer(pGpu->pOsGpuInfo, pTmrEvent->super.pOSTmrCBdata);
        pTmrEvent->super.flags &= ~TMR_FLAG_OS_TIMER_QUEUED;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR No Timer event callback found, invalid timer SW state\n");
        status = NV_ERR_INVALID_REQUEST;
    }

    return status;
}

/*!
 * This function cancels OS timer callback
 *
 *   @param[in]  pTmr Pointer to Timer Object
 *   @param[in]  pEvent pointer to timer event information
 *
 *  @returns  NV_ERR_INVALID_REQUEST if callback entry not found
 */
NV_STATUS tmrEventDestroyOSTimer_OSTIMER
(
    OBJTMR             *pTmr,
    PTMR_EVENT          pPublicEvent
)
{
    NV_STATUS status= NV_OK;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    PTMR_EVENT_PVT  pTmrEvent = (PTMR_EVENT_PVT) pPublicEvent;

    if (pTmrEvent != NULL && pTmrEvent->super.pOSTmrCBdata != NULL)
    {
        // Cancel the callback of OS timer
        status = osDestroyNanoTimer(pGpu->pOsGpuInfo, pTmrEvent->super.pOSTmrCBdata);
        pTmrEvent->super.flags &= ~TMR_FLAG_OS_TIMER_QUEUED;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No Timer event callback found, invalid timer SW state\n");
        status = NV_ERR_INVALID_REQUEST;
    }

    return status;
}

NV_STATUS
tmrGetIntrStatus_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    NvU32              *pStatus,
    THREAD_STATE_NODE  *pThreadState
)
{
    *pStatus = 0;
    return NV_OK;
}

//
// For functions that only need a short delta of time elapsed (~ 4.29 seconds)
// NOTE: Since it wraps around every 4.29 seconds, for general GetTime purposes,
//       it's better to use tmrGetTime().
//
NvU32
tmrGetTimeLo_OSTIMER
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    return NvU64_LO32(tmrGetTimeEx_HAL(pGpu, pTmr, NULL));
}

NvU64
tmrGetTime_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr
)
{
    return tmrGetTimeEx_HAL(pGpu, pTmr, NULL);
}

NvU32
tmrReadTimeLoReg_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    return NvU64_LO32(tmrGetTimeEx_HAL(pGpu, pTmr, pThreadState));
}

NvU32
tmrReadTimeHiReg_OSTIMER
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    return NvU64_HI32(tmrGetTimeEx_HAL(pGpu, pTmr, pThreadState));
}

NV_STATUS
tmrGetGpuAndCpuTimestampPair_OSTIMER
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU64   *pGpuTime,
    NvU64   *pCpuTime
)
{
#if PORT_IS_FUNC_SUPPORTED(portUtilExReadTimestampCounter)
    *pGpuTime = tmrGetTimeEx_HAL(pGpu, pTmr, NULL);
    *pCpuTime = portUtilExReadTimestampCounter();
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS
tmrDelay_OSTIMER
(
     OBJTMR    *pTmr,
     NvU32      nsec
)
{
    if (nsec > 50000000)    // 50 ms.
    {
        osDelay(nsec / 1000000);
    }
    else if (nsec > 0)
    {
        osDelayNs(nsec);
    }

    return NV_OK;
}

