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
*   The GB100 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
#define NVOC_OBJTMR_H_PRIVATE_ACCESS_ALLOWED
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"

#include "published/blackwell/gb100/dev_top_zb.h"
#include "published/blackwell/gb100/dev_tmr.h"

/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
#define GET_DEVICE_ENTRY(gpu, type, instance, global, local, entry) \
    gpuGetOneDeviceEntry(gpu, type, instance, global, local, entry)

/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */
NvU32
tmrGetTmrBaseAddr_GB100
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    const DEVICE_INFO_ENTRY *pEntry;
    NV_STATUS status = GET_DEVICE_ENTRY(pGpu,
                                        NV_PTOP_ZB_DEVICE_INFO_DEV_TYPE_ENUM_TMR,
                                        DEVICE_INFO_DIELET_INSTANCE_ANY,
                                        0,
                                        DEVICE_INFO_DIE_LOCAL_INSTANCE_ID_ANY,
                                        &pEntry);

    NV_ASSERT_OR_RETURN(status == NV_OK, 0);

    return pEntry->devicePriBase;
}

NvU32
tmrGetNsecShiftMask_GB100
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU32 tmrBaseAddr = tmrGetTmrBaseAddr_HAL(pGpu, pTmr);

    return DRF_SHIFTMASK(tmrBaseAddr + NV_TMR_TIME_0_NSEC);
}

/*!
 * @brief Returns the BAR0 offset and size of the PTIMER range.
 *
 * @param[in]  pGpu
 * @param[in]  pTmr
 * @param[out] pBar0MapOffset
 * @param[out] pBar0MapSize
 *
 * @return NV_STATUS
 */
NV_STATUS
tmrGetTimerBar0MapInfo_GB100
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU64   *pBar0MapOffset,
    NvU32   *pBar0MapSize
)
{
    NvU32 tmrBaseAddr = tmrGetTmrBaseAddr_HAL(pGpu, pTmr);

    *pBar0MapOffset = tmrBaseAddr;
    *pBar0MapSize   = DRF_SIZE(NV_TMR);

    return NV_OK;
}