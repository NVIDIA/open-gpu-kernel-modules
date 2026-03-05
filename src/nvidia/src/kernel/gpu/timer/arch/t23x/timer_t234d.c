/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/********************* Chip Specific HAL TMR Routines **********************\
*                                                                           *
*   The T234D specific HAL TMR routines reside in this file.                *
*   This implementation uses ARM architectural timer for T234D.             *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "os/os.h"
/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
static NvU64 tmrReadArchTimerNanoseconds(void);

/* ------------------------- Static Functions  ------------------------------ */

/*!
 * @brief Convert ARM architectural timer to nanoseconds
 *
 * @return ARM architectural timer time in nanoseconds
 */
static NvU64
tmrReadArchTimerNanoseconds(void)
{
    NvU64 timeNanoseconds;

#if PORT_IS_FUNC_SUPPORTED(portUtilExReadTimestampCounter) && PORT_IS_FUNC_SUPPORTED(portUtilExReadArchTimerFreq)
    NvU64 archTimerCount;
    NvU64 timerFreq;

    // Read ARM architectural timer counter and frequency
    archTimerCount = portUtilExReadTimestampCounter();
    timerFreq = portUtilExReadArchTimerFreq();

    if (timerFreq != 0) {
        //
        // Convert to nanoseconds: (count * 1000000000) / freq
        // Avoid overflow by checking if we can multiply safely
        //
        if (archTimerCount <= (NV_U64_MAX / 1000000000ULL)) {
            // Safe to multiply first
            timeNanoseconds = (archTimerCount * 1000000000ULL) / timerFreq;
        } else {
            //
            // Risk of overflow - use alternative calculation
            // timeNanoseconds = archTimerCount * (1000000000 / timerFreq) + remainder handling
            //
            NvU64 quotient = 1000000000ULL / timerFreq;
            NvU64 remainder = 1000000000ULL % timerFreq;
            timeNanoseconds = archTimerCount * quotient + (archTimerCount * remainder) / timerFreq;
        }
    } else {
        // Fallback to OS timestamp if frequency is 0
        timeNanoseconds = osGetTimestamp();
    }
#else
    // Fallback to OS timestamp if architectural timer not available
    timeNanoseconds = osGetTimestamp();
#endif

    return timeNanoseconds;
}

/* ------------------------- Public Functions  ------------------------------ */

/*!
 * @brief Read low 32 bits of ARM architectural timer in nanoseconds
 *
 * For T234D, the ARM architectural timer is used instead of GPU PTIMER.
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pTmr      Timer object pointer
 * @param[in] pThreadState Thread state (unused)
 *
 * @return Low 32 bits of ARM architectural timer time in nanoseconds
 */
NvU32
tmrReadTimeLoReg_T234D
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU64 timeNanoseconds;
    NvU32 timeLoValue;

    timeNanoseconds = tmrReadArchTimerNanoseconds();

    timeLoValue = NvU64_LO32(timeNanoseconds);

    return timeLoValue;
}

/*!
 * @brief Read high 32 bits of ARM architectural timer in nanoseconds
 *
 * For T234D, the ARM architectural timer is used instead of GPU PTIMER.
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pTmr      Timer object pointer
 * @param[in] pThreadState Thread state (unused)
 *
 * @return High 32 bits of ARM architectural timer time in nanoseconds
 */
NvU32
tmrReadTimeHiReg_T234D
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU64 timeNanoseconds;
    NvU32 timeHiValue;

    timeNanoseconds = tmrReadArchTimerNanoseconds();

    timeHiValue = NvU64_HI32(timeNanoseconds);

    return timeHiValue;
}
