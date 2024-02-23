/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_SEMAPHORE_H__
#define __NV_SEMAPHORE_H__

#include "nvtypes.h"
#include "nvCpuIntrinsics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    NvU32 payload;
    NvU32 reportValue;
    NvU64 timer;
} NvReportSemaphore32;

typedef volatile struct {
    NvU64 reportValue;
    NvU64 timer;
} NvReportSemaphore64;

typedef volatile union {
    NvReportSemaphore32 sema32;
    NvReportSemaphore64 sema64;
} NvReportSemaphore;

/*
 * These structures can't change size.  They map to the GPU and other driver
 * components expect the same size.
 */
ct_assert(sizeof(NvReportSemaphore32) == 16);
ct_assert(sizeof(NvReportSemaphore64) == 16);
ct_assert(sizeof(NvReportSemaphore)   == 16);

/*
 * Pre-Volta GPUs can only read/write a 32-bit semaphore.  Rather than try to
 * use multiple semaphore writes to emulate a full 64-bit write, which is prone
 * to race conditions when the value wraps, derive the full 64-bit value by
 * comparing the current GPU-accessible value with the the last value written by
 * the CPU or submitted to be written by the GPU, which is stashed in the
 * timestamp field of the semaphore by the CPU in both these cases.
 */
static inline void NvTimeSemFermiSetMaxSubmittedVal(
    volatile NvU64 *maxSubmittedPtr,
    const NvU64 value)
{
    NvU64 oldValue =
            (NvU64)__NVatomicCompareExchange64((volatile NvS64 *)maxSubmittedPtr,
                                               0, 0);

    // Atomically set report->timer to max(value, report->time).
    while (oldValue < value) {
        const NvU64 prevValue =
            (NvU64)__NVatomicCompareExchange64((volatile NvS64 *)maxSubmittedPtr,
                                               (NvS64)value,
                                               (NvS64)oldValue);
        if (prevValue == oldValue) {
            // The specified value was set.  Done.
            nvAssert(*maxSubmittedPtr >= value);
            break;
        }

        oldValue = prevValue;
    }
}

static inline void NvTimeSemFermiSetMaxSubmitted(
    NvReportSemaphore32 *report,
    const NvU64 value)
{
    NvTimeSemFermiSetMaxSubmittedVal(&report->timer, value);
}

static inline NvU64 NvTimeSemFermiGetPayloadVal(
    volatile void *payloadPtr,
    volatile void *maxSubmittedPtr)
{
    // The ordering of the two operations below is critical.  Other threads
    // may be submitting GPU work that modifies the semaphore value, or
    // modifying it from the CPU themselves.  Both of those operations first
    // set the 64-bit max submitted/timer value, then modify or submit work
    // to modify the 32-bit payload value.  Consider this hypothetical timeline
    // if the order of operations below is reversed:
    //
    //   thread1:
    //   -SetMaxSubmitted(0x1);
    //   -report->payload = 0x1;
    //
    //   thread2:
    //   -Reads 0x1 from report->timer
    //
    //   thread1:
    //   -SetMaxSubmitted(0x7fffffff);
    //   -report->payload = 0x7fffffff;
    //   -SetMaxSubmitted(0x100000000);
    //   -report->payload = 0x00000000;
    //
    //   thread2:
    //   -Reads 0x0 from report->payload
    //
    // The logic below would see 0 (payload) is less than 1 (max submitted) and
    // determine a wrap is outstanding, subtract one from the high 32-bits of
    // the max submitted value (0x00000000 - 0x1), overflow, and return the
    // current 64-bit value as 0xffffffff00000000 when the correct value is
    // 0x100000000.  To avoid this, we must read the payload prior to reading
    // the max submitted value from the timer field.  The logic can correctly
    // adjust the max submitted value back down if a wrap occurs between these
    // two operations, but has no way to bump the max submitted value up if a
    // wrap occurs with the opposite ordering.
    NvU64 current = *(volatile NvU32*)payloadPtr;
    // Use an atomic exchange to ensure the 64-bit read is atomic even on 32-bit
    // CPUs.
    NvU64 submitted = (NvU64)
        __NVatomicCompareExchange64((volatile NvS64 *)maxSubmittedPtr, 0ll, 0ll);

    nvAssert(!(current & 0xFFFFFFFF00000000ull));

    // The value is monotonically increasing, and differ by no more than
    // 2^31 - 1.  Hence, if the low word of the submitted value is less
    // than the low word of the current value, exactly one 32-bit wrap
    // occurred between the current value and the most recently
    // submitted value.  Walk back the high word to match the value
    // associated with the current GPU-visible value.
    if ((submitted & 0xFFFFFFFFull) < current) {
        submitted -= 0x100000000ull;
    }

    current |= (submitted & 0xFFFFFFFF00000000ull);

    return current;
}

static inline NvU64 NvTimeSemFermiGetPayload(
    NvReportSemaphore32 *report)
{
    return NvTimeSemFermiGetPayloadVal(&report->payload, &report->timer);
}

static inline void NvTimeSemFermiSetPayload(
    NvReportSemaphore32 *report,
    const NvU64 payload)
{
    // First save the actual value to the reserved/timer bits
    NvTimeSemFermiSetMaxSubmittedVal(&report->timer, payload);

    // Then write the low bits to the GPU-accessible semaphore value.
    report->payload = (NvU32)(payload & 0xFFFFFFFFULL);
}

/*
 * Volta and up.
 */

static inline NvU64 NvTimeSemVoltaGetPayloadVal(
    volatile void *payloadPtr)
{
    nvAssert(payloadPtr);
    return (NvU64)
        __NVatomicCompareExchange64((volatile NvS64 *)payloadPtr,
                                    0, 0);
}

static inline NvU64 NvTimeSemVoltaGetPayload(
    NvReportSemaphore64 *report)
{
    return NvTimeSemVoltaGetPayloadVal(&report->reportValue);
}

static inline void NvTimeSemVoltaSetPayload(
    NvReportSemaphore64 *report,
    const NvU64 payload)
{
    NvU64 oldPayload = 0;

    while (NV_TRUE) {
        NvU64 prevPayload = (NvU64)
            __NVatomicCompareExchange64((volatile NvS64 *)&report->reportValue,
                                        (NvS64)payload, (NvS64)oldPayload);

        if (prevPayload == oldPayload) {
            break;
        }

        nvAssert(prevPayload < payload);

        oldPayload = prevPayload;
    }
}

#ifdef __cplusplus
};
#endif

#endif /* __NV_SEMAPHORE_H__ */
