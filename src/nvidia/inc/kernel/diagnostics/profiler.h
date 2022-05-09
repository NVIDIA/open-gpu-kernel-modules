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
#ifndef _PROFILER_H_
#define _PROFILER_H_

/*!
 * @file  profiler.h
 * @brief Simple API to measure elapsed times in RM for profiling and statistics
 *
 * The primary goals of this API are to:
 *   1. Be lightweight and have little-to-no setup required (built into release drivers)
 *   2. Defer as much analysis as possible to the user of the data (keep it simple)
 *   3. Provide sub-millisecond resolution if possible (medium-high granularity)
 *
 * This is intended mainly for coarse measurements of time-critical software
 * sequences, such as GC6. For example, the measurements could be used to catch
 * major latency regressions in a particular timing module.
 *
 * For more sophisticated profiling (e.g. for prospective analysis), use of an
 * external profiling tool (e.g. xperf with ETW) is recommended instead.
 */

#include "core/core.h"

/*!
 * Record containing the statistics of a single time module to be profiled
 * periodically.
 *
 * This tracks the min/max elapsed time over all the measurement
 * cycles, as well as the total elapsed time and number of cycles.
 * To calculate the average elapsed time per cycle, divide total_ns by count.
 *
 * 64-bit precision integers are used to hold nanosecond resolution
 * over long periods of time (e.g. greater than 4 seconds).
 */
typedef struct
{
    NvU64   count;    //<! Number of cycles this time record has measured
    NvU64   start_ns; //<! Starting time of current cycle in nanoseconds
    NvU64   total_ns; //<! Total nanoseconds elapsed for this record
    NvU64   min_ns;   //<! Minimum nanoseconds elapsed in a single cycle
    NvU64   max_ns;   //<! Maximum nanoseconds elapsed in a single cycle
} RM_PROF_STATS;

/*!
 * A profiling group is a chain of continuous profiling modules. The purpose of
 * chaining the modules together is to reduce the number of timestamps taken
 * by approximately half and to prevent any time from "escaping" between the
 * end of one module and the start of another (e.g. due to preemption).
 */
typedef struct
{
    RM_PROF_STATS *pTotal; //<! Stats for the total time spent in the group.
    RM_PROF_STATS *pLast;  //<! Stats for the previous module of the group.
} RM_PROF_GROUP;

/*!
 * Start measuring time for the specified module stats (begin a new cycle).
 */
#define RM_PROF_START(pStats)            rmProfStart(pStats)

/*!
 * Stop measuring time for the specified module stats (end a cycle).
 * Must be called after RM_PROF_START - these two APIs are not reentrant.
 */
#define RM_PROF_STOP(pStats)             rmProfStop(pStats)

/*!
 * Lower-level API to record a cycle time manually. The provided time can
 * be derived from any timer source as long as it is converted to nanoseconds.
 */
#define RM_PROF_RECORD(pStats, time_ns)  rmProfRecord(pStats, time_ns)

/*!
 * Start measuring time for the specified profiling group (begin a new cycle).
 *   pTotal is the optional stats for the whole group.
 *   pFirst is the first module of the group.
 */
#define RM_PROF_GROUP_START(pGroup, pTotal, pFirst)  rmProfGroupStart(pGroup, pTotal, pFirst)

/*!
 * Continue profiling the next module of a profiling group.
 */
#define RM_PROF_GROUP_NEXT(pGroup, pNext)            rmProfGroupNext(pGroup, pNext)

/*!
 * Stop profiling a cycle of a profiling group (ends both the last and total modules).
 */
#define RM_PROF_GROUP_STOP(pGroup)                   rmProfGroupStop(pGroup)

// Underlying functions - use the wrapper macros instead.
void rmProfStart (RM_PROF_STATS *pStats);
void rmProfStop  (RM_PROF_STATS *pStats);
void rmProfRecord(RM_PROF_STATS *pStats, NvU64 time_ns);
void rmProfGroupStart(RM_PROF_GROUP *pGroup, RM_PROF_STATS *pTotal, RM_PROF_STATS *pFirst);
void rmProfGroupNext (RM_PROF_GROUP *pGroup, RM_PROF_STATS *pNext);
void rmProfGroupStop (RM_PROF_GROUP *pGroup);

#endif /* _PROFILER_H_ */
