/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** ODB State Routines **************************\
*                                                                           *
*            Simple API to measure elapsed times in RM for profiling        *
*                                                                           *
\***************************************************************************/

#include "diagnostics/profiler.h"
#include "os/os.h"

static void _rmProfStopTime(RM_PROF_STATS *pStats, NvU64 stop_ns);

/*!
 * @brief Start measuring elapsed time for a specific profiling module.
 *
 * @param[in,out]  pStats  Profiling stats for the module
 */
void
rmProfStart
(
    RM_PROF_STATS *pStats
)
{
    NV_ASSERT_OR_RETURN_VOID(pStats != NULL);

    if (pStats->start_ns != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Starting time measurement that is already started\n");
        //
        // No breakpoint since this isn't fatal by itself.
        // Most likely there was an error that propagated up the stack before
        // the measurement was stopped on the last cycle.
        //
        // In that case, restarting the measurement is probably the right thing
        // to do anyway.
        //
    }
    osGetPerformanceCounter(&pStats->start_ns);
}

/*!
 * @brief Stop measuring elapsed time for a specific profiling module and
 *        update the module's statistics.
 *
 * @param[in,out]  pStats  Profiling stats for the module
 */
void
rmProfStop
(
    RM_PROF_STATS *pStats
)
{
    NvU64 stop_ns;

    NV_ASSERT_OR_RETURN_VOID(pStats != NULL);

    osGetPerformanceCounter(&stop_ns);
    _rmProfStopTime(pStats, stop_ns);
}

/*!
 * Same as #rmProfStop but parameterized by the stop time.
 */
static void
_rmProfStopTime
(
    RM_PROF_STATS *pStats,
    NvU64          stop_ns
)
{
    NV_ASSERT_OR_RETURN_VOID(pStats != NULL);

    if (pStats->start_ns == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Stopping time measurement that is already stopped\n");
        DBG_BREAKPOINT();
        //
        // Breakpoint since this case is more serious - something is likely
        // wrong with the profiling code. Also return early so the bogus
        // measurement is not recorded.
        //
        return;
    }
    RM_PROF_RECORD(pStats, stop_ns - pStats->start_ns);
    pStats->start_ns = 0;
}

/*!
 * @brief Manually update the statistics for one cycle of a specific profiling
 *        module.
 *
 * @param[in,out]  pStats  Profiling stats for the module
 * @param[in]      time_ns Elapsed time in nanoseconds for this cycle.
 */
void
rmProfRecord
(
    RM_PROF_STATS *pStats,
    NvU64          time_ns
)
{
    NV_ASSERT_OR_RETURN_VOID(pStats != NULL);

    if (pStats->count == 0 || time_ns < pStats->min_ns)
    {
        pStats->min_ns = time_ns;
    }
    if (pStats->count == 0 || time_ns > pStats->max_ns)
    {
        pStats->max_ns = time_ns;
    }
    pStats->total_ns += time_ns;
    pStats->count += 1;
}

/*!
 * @brief Start measuring time for the specified profiling group (begin a new cycle).
 *
 * @param[out]    pGroup  Profiling group structure to be used.
 * @param[in/out] pTotal  Optional stats for the whole group duration (may be NULL).
 * @param[in/out] pFirst  First module of the group.
 */
void
rmProfGroupStart
(
    RM_PROF_GROUP *pGroup,
    RM_PROF_STATS *pTotal,
    RM_PROF_STATS *pFirst
)
{
    NV_ASSERT_OR_RETURN_VOID(pGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(pFirst != NULL);

    // Start profiling the first module.
    RM_PROF_START(pFirst);

    // Reuse the first modules' start time for the total module.
    if (pTotal != NULL)
    {
        pTotal->start_ns = pFirst->start_ns;
    }

    // Initialize the group structure.
    pGroup->pTotal = pTotal;
    pGroup->pLast  = pFirst;
}

/*!
 * @brief Continue profiling the next module of a profiling group.
 *
 * @param[in/out]  pGroup  Profiling group.
 * @param[in/out]  pNext   Next module of the group.
 */
void
rmProfGroupNext
(
    RM_PROF_GROUP *pGroup,
    RM_PROF_STATS *pNext
)
{
    NV_ASSERT_OR_RETURN_VOID(pGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(pGroup->pLast != NULL);
    NV_ASSERT_OR_RETURN_VOID(pNext != NULL);

    // Start profiling the next module.
    RM_PROF_START(pNext);

    // Reuse the next modules' start time for the last module's stop time.
    _rmProfStopTime(pGroup->pLast, pNext->start_ns);

    // Update the group structure.
    pGroup->pLast = pNext;
}

/*!
 * @brief Stop profiling a cycle of a profiling group (ends both the last and total modules).
 *
 * @param[in]  pGroup  Profiling group.
 */
void
rmProfGroupStop
(
    RM_PROF_GROUP *pGroup
)
{
    NvU64 stop_ns;

    NV_ASSERT_OR_RETURN_VOID(pGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(pGroup->pLast != NULL);

    osGetPerformanceCounter(&stop_ns);

    // Reuse the same stop time for both last and total module.
    _rmProfStopTime(pGroup->pLast, stop_ns);
    if (pGroup->pTotal != NULL)
    {
        _rmProfStopTime(pGroup->pTotal, stop_ns);
    }

    // Clear the group structure.
    pGroup->pTotal = NULL;
    pGroup->pLast  = NULL;
}
