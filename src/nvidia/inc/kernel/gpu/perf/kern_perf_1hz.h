/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_PERF_1HZ_H
#define KERNEL_PERF_1HZ_H

/* ------------------------ Includes --------------------------------------- */
#include "gpu/gpu_resource.h"
#include "objtmr.h"

/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Datatypes -------------------------------------- */
/*!
 * This structure represents data for managing 1HZ Callback timer
 */
typedef struct
{
    /*!
     * NV_TRUE if 1Hz callback is in progress
     */
    NvBool                  b1HzTimerCallback;

    /*!
     * TRUE if AllowMaxPerf and not in Hibernate/Standby
     */
    NvBool                  bEnableTimerUpdates;
} KERNEL_PERF_1HZ;

/* -------------------- Function Prototypes -------------------------------- */
/*!
 * @brief Handle 1Hz timer callback from SW interrupts
 *
 * @param[in]  pGpu OBJGPU pointer
 * @param[in]  pTmr OBJTMR pointer
 * @param[in] *ptr  timer callback ID
 *
 * @returns Always return NV_OK
 *
 */
NV_STATUS kperfTimerProc(OBJGPU *pGpu, OBJTMR *pTmr, void *ptr);

/*!
 * Since the function tmrCancelCallback() needs a distinct value for POBJECT,
 * we can not just use any value in the POBJECT field if we intend to use
 * tmrCancelCallback() function. For scheduling Kernel Perf related callbacks we 
 * will use the unique value for the Kernel Perf by using the address of the function
 * that will be called when timer elapses.
 */
#define TMR_POBJECT_KERNEL_PERF_1HZ ((void *)(kperfTimerProc))

#endif // KERNEL_PERF_1HZ_H
