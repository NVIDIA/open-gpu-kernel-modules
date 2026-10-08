/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief TIME conversion functions implementations - GENERIC
 */

#include "nvport/nvport.h"


PORT_INLINE NvBool
_portTimeIsLeapYear(NvU32 year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}
PORT_INLINE NvU32
_portTimeDaysInYear(NvU32 year)
{
    return _portTimeIsLeapYear(year) ? 366 : 365;
}

static void
_portTimeUnixTimeToDate
(
    NvU64 t,
    NvU16 *pSeconds,
    NvU16 *pMinutes,
    NvU16 *pHours,
    NvU16 *pDay,
    NvU16 *pMonth,
    NvU16 *pYear
)
{
    NvU32 daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    *pSeconds = t % 60;
    t /= 60;
    *pMinutes = t % 60;
    t /= 60;
    *pHours = t % 24;
    t /= 24;

    *pYear = 1970;
    *pDay = 0;

    while (t >= _portTimeDaysInYear(*pYear))
    {
        t -= _portTimeDaysInYear(*pYear);
        (*pYear)++;
    }

    if (_portTimeIsLeapYear(*pYear))
        daysInMonth[1]++;

    for (*pMonth = 0; t >= daysInMonth[*pMonth]; (*pMonth)++)
        t -= daysInMonth[*pMonth];

    (*pMonth)++;
    *pDay = (NvU16)t+1;
}

PORT_WALLTIME portTimeGetLocalWallTime(void)
{
    PORT_WALLTIME w;
    NvU64 t = portTimeGetMilliseconds();

    w.millisecond  = t % 1000;
    t /= 1000;

    _portTimeUnixTimeToDate(t,
                            &w.second,
                            &w.minute,
                            &w.hour,
                            &w.day,
                            &w.month,
                            &w.year);
    return w;
}

PORT_WALLTIME portTimeConvertToWallTime(NvU64 msInUnixEpoch)
{
    PORT_WALLTIME w;

    w.millisecond  = msInUnixEpoch % 1000;
    msInUnixEpoch /= 1000;

    _portTimeUnixTimeToDate(msInUnixEpoch,
                            &w.second,
                            &w.minute,
                            &w.hour,
                            &w.day,
                            &w.month,
                            &w.year);
    return w;
}

NvU64 portTimeConvertToUnixMs(PORT_WALLTIME walltime)
{
    NvU64 unixms;
    NvU32 daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    NvU32 i;

    unixms  = (NvU64) walltime.millisecond;
    unixms += (NvU64) walltime.second * 1000;
    unixms += (NvU64) walltime.minute * 1000 * 60;
    unixms += (NvU64) walltime.hour   * 1000 * 60 * 60;
    unixms += (NvU64) (walltime.day-1)* 1000 * 60 * 60 * 24;

    for (i = 1970; i < walltime.year; i++)
    {
        unixms += (NvU64) _portTimeDaysInYear(i) * 1000ULL * 60 * 60 * 24;
    }

    if (_portTimeIsLeapYear(walltime.year))
        daysInMonth[1]++;

    for (i = 0; i+1 < walltime.month; i++)
    {
        unixms += (NvU64)daysInMonth[i] * 1000 * 60 * 60 * 24;
    }

    return unixms;
}

