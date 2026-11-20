/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "nvport/nvport.h"

static NvU64 testClockMs;
static unsigned int testCount;

NvU64 portTimeGetMilliseconds(void)
{
    return testClockMs;
}

static void
checkWallTime
(
    const char *pApi,
    NvU64 timestampMs,
    PORT_WALLTIME actual,
    PORT_WALLTIME expected
)
{
    if (actual.year != expected.year || actual.month != expected.month ||
        actual.day != expected.day || actual.hour != expected.hour ||
        actual.minute != expected.minute || actual.second != expected.second ||
        actual.millisecond != expected.millisecond)
    {
        fprintf(stderr,
                "%s at %" PRIu64 " ms: expected "
                "%04u-%02u-%02u %02u:%02u:%02u.%03u, got "
                "%04u-%02u-%02u %02u:%02u:%02u.%03u\n",
                pApi, (uint64_t)timestampMs,
                expected.year, expected.month, expected.day,
                expected.hour, expected.minute, expected.second,
                expected.millisecond,
                actual.year, actual.month, actual.day,
                actual.hour, actual.minute, actual.second, actual.millisecond);
        exit(EXIT_FAILURE);
    }

    if (portTimeConvertToUnixMs(actual) != timestampMs)
    {
        fprintf(stderr, "%s round trip failed at %" PRIu64 " ms\n",
                pApi, (uint64_t)timestampMs);
        exit(EXIT_FAILURE);
    }
}

static void
checkTime(NvU64 timestampMs, PORT_WALLTIME expected)
{
    testClockMs = timestampMs;
    checkWallTime("portTimeConvertToWallTime", timestampMs,
                  portTimeConvertToWallTime(timestampMs), expected);
    checkWallTime("portTimeGetLocalWallTime", timestampMs,
                  portTimeGetLocalWallTime(), expected);
    testCount++;
}

static void
checkAgainstUtc(NvU64 timestampMs)
{
    time_t seconds = (time_t)(timestampMs / 1000);
    struct tm utc;
    PORT_WALLTIME expected;

    if ((NvU64)seconds != timestampMs / 1000 ||
        gmtime_r(&seconds, &utc) == NULL)
    {
        fprintf(stderr, "UTC reference cannot represent %" PRIu64 " ms\n",
                (uint64_t)timestampMs);
        exit(EXIT_FAILURE);
    }

    expected.year = utc.tm_year + 1900;
    expected.month = utc.tm_mon + 1;
    expected.day = utc.tm_mday;
    expected.hour = utc.tm_hour;
    expected.minute = utc.tm_min;
    expected.second = utc.tm_sec;
    expected.millisecond = timestampMs % 1000;
    checkTime(timestampMs, expected);
}

int main(void)
{
    static const struct
    {
        NvU64 timestampMs;
        PORT_WALLTIME expected;
    } cases[] =
    {
        {0,                 {1970,  1,  1,  0,  0,  0,   0}},
        {1790813192000ULL,  {2026, 10,  1,  0,  6, 32,   0}},
        {1790852555000ULL,  {2026, 10,  1, 11,  2, 35,   0}},
        {31536000000ULL,    {1971,  1,  1,  0,  0,  0,   0}},
        {946684800000ULL,   {2000,  1,  1,  0,  0,  0,   0}},
        {1798761600000ULL,  {2027,  1,  1,  0,  0,  0,   0}},
        {4102444800000ULL,  {2100,  1,  1,  0,  0,  0,   0}},
        {13569465600000ULL, {2400,  1,  1,  0,  0,  0,   0}},
        {951868799999ULL,   {2000,  2, 29, 23, 59, 59, 999}},
        {951868800000ULL,   {2000,  3,  1,  0,  0,  0,   0}},
        {4107542399999ULL,  {2100,  2, 28, 23, 59, 59, 999}},
        {4107542400000ULL,  {2100,  3,  1,  0,  0,  0,   0}},
        {13574649599999ULL, {2400,  2, 29, 23, 59, 59, 999}},
        {13574649600000ULL, {2400,  3,  1,  0,  0,  0,   0}}
    };
    static const NvU64 offsetsMs[] = {0, 43200123, 86399999};
    unsigned int day;
    size_t i;

    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
        checkTime(cases[i].timestampMs, cases[i].expected);

    /* Check each day in one complete 400-year Gregorian cycle. */
    for (day = 0; day < 146097; day++)
    {
        for (i = 0; i < sizeof(offsetsMs) / sizeof(offsetsMs[0]); i++)
            checkAgainstUtc((NvU64)day * 86400000 + offsetsMs[i]);
    }

    printf("PASS: %u timestamps, both wall-time APIs and round trips\n",
           testCount);
    return EXIT_SUCCESS;
}
