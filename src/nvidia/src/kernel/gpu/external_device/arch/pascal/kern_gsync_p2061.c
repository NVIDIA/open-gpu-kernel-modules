/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "dev_p2060.h"
#include "dev_p2061.h"
#include "gpu/external_device/dac_p2060.h"
#include "gpu/external_device/dac_p2061.h"

/*
 * Get the house sync mode (input/output).
 */
NV_STATUS
gsyncGetHouseSyncMode_P2061
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU8*              houseSyncMode
)
{
    NvU8      regCtrl4;
    NV_STATUS status;

    status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                                           (NvU8)NV_P2061_CONTROL4, &regCtrl4);
    *houseSyncMode = DRF_VAL(_P2061, _CONTROL4, _HOUSE_SYNC_MODE, regCtrl4);

    return status;
}

/*
 * Set the house sync mode (input/output).
 */
NV_STATUS
gsyncSetHouseSyncMode_P2061
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU8               houseSyncMode
)
{
    NvU8      regStatus2;
    NvU8      regCtrl4;
    NV_STATUS status;

    if (houseSyncMode != NV_P2061_CONTROL4_HOUSE_SYNC_MODE_INPUT &&
        houseSyncMode != NV_P2061_CONTROL4_HOUSE_SYNC_MODE_OUTPUT)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                                           (NvU8)NV_P2060_STATUS2, &regStatus2);

    status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                                           (NvU8)NV_P2061_CONTROL4, &regCtrl4);

    //
    // Bailing out when the following 3 conditions are satisfied:
    // 1. house sync mode is currently set as input, and
    // 2. there is a BNC signal detected, and
    // 3. client wants to switch house sync mode to output.
    //
    // This helps avoid the case that user is trying to toggle the mode to output
    // when p2061 is inputting from a house sync device.
    //
    if (FLD_TEST_DRF(_P2061, _CONTROL4, _HOUSE_SYNC_MODE, _INPUT, regCtrl4) &&
        !FLD_TEST_DRF(_P2060, _STATUS2, _HS_DETECT, _NONE, regStatus2) &&
        houseSyncMode == NV_P2061_CONTROL4_HOUSE_SYNC_MODE_OUTPUT)
    {
        return NV_ERR_INVALID_STATE;
    }

    regCtrl4 = FLD_SET_DRF_NUM(_P2061, _CONTROL4, _HOUSE_SYNC_MODE, houseSyncMode, regCtrl4);
    status |= writeregu008_extdeviceTargeted(pGpu, pExtDev,
                                           (NvU8)NV_P2061_CONTROL4, regCtrl4);

    return status;
}

/*
 * Handle Get and Set queries related to CPL status.
 */
NV_STATUS
gsyncGetCplStatus_P2061
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCSTATUS CplStatus,
    NvU32 *pVal
)
{
    NV_STATUS status = NV_OK;
    NvU8 regStatus2;
    NvU8 regStatus6;

    switch (CplStatus)
    {
        // p2061-specific cases
        case gsync_Status_bInternalSlave:
            status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                     (NvU8)NV_P2060_STATUS2, &regStatus2);
            if (status != NV_OK)
            {
                break;
            }
            status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                     (NvU8)NV_P2061_STATUS6, &regStatus6);
            if (status == NV_OK)
            {
                *pVal = FLD_TEST_DRF(_P2060, _STATUS2, _PORT0, _OUTPUT, (NvU32)regStatus2) &&
                        FLD_TEST_DRF(_P2060, _STATUS2, _PORT1, _OUTPUT, (NvU32)regStatus2) &&
                        FLD_TEST_DRF(_P2061, _STATUS6, _INT_PORT_DIRECTION, _INPUT, (NvU32)regStatus6);
            }
            break;
        // common cases shared by both p2060 and p2061
        default:
            status = gsyncGetCplStatus_P2060(pGpu, pExtDev, CplStatus, pVal);
    }

    return status;
}

