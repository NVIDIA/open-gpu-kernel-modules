/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/external_device/dev_p2060.h"
#include "gpu/external_device/dev_p2061.h"
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

// Write to the SyncSkew register
NV_STATUS
gsyncSetSyncSkew_P2061_V204
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NvU32              syncSkew
)
{
    DACP2060EXTERNALDEVICE *pThis = (DACP2060EXTERNALDEVICE *)pExtDev;
    NvU64 temp;

    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       syncSkew <= NV_P2061_V204_SYNC_SKEW_MAX_UNITS,
                       NV_ERR_INVALID_ARGUMENT);

    //
    // Cache the unmodified syncSkew value from the user.
    // We use this later to improve the stability of returned values in GetSyncSkew.
    //
    pThis->lastUserSkewSent = syncSkew;

    //
    // Fix the skew value which goes to HW, because we passed the resolution up
    // to NVAPI as 7us. However, the real resolution in hardware is
    // (1 / 131.072 MHz) = 7.6293945...
    // Fix it by multiplying by the ratio 7 / 7.6293945
    //
    temp = syncSkew;
    temp *= 70000000; // Safe because temp is 64 bits
    temp /= 76293945;
    syncSkew = (NvU32)temp; // Safe because we multiplied by less than 1.

    // update p2060 object
    pThis->SyncStartDelay = syncSkew;

    NV_ASSERT_OK_OR_RETURN(
        writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_LOW,
                                       syncSkew & DRF_MASK(NV_P2060_SYNC_SKEW_LOW_VAL)));
    NV_ASSERT_OK_OR_RETURN(
        writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_HIGH,
                                       (syncSkew >> 8) & DRF_MASK(NV_P2060_SYNC_SKEW_HIGH_VAL)));
    NV_ASSERT_OK_OR_RETURN(
        writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_UPPER,
                                       (syncSkew >> 16) & DRF_MASK(NV_P2060_SYNC_SKEW_UPPER_VAL)));

    return NV_OK;
}

// Read from the SyncSkew register
NV_STATUS
gsyncGetSyncSkew_P2061_V204
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NvU32             *pSyncSkew
)
{
    NvU8 data;
    NvU32 syncSkew;
    NvU64 temp;
    DACP2060EXTERNALDEVICE *pThis = (DACP2060EXTERNALDEVICE *)pExtDev;

    *pSyncSkew = 0;

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_LOW, &data));
    syncSkew = DRF_VAL(_P2060, _SYNC_SKEW_LOW, _VAL, data);

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_HIGH, &data));
    syncSkew |= DRF_VAL(_P2060, _SYNC_SKEW_HIGH, _VAL, data) << 8;

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_UPPER, &data));
    syncSkew |= DRF_VAL(_P2060, _SYNC_SKEW_UPPER, _VAL, data) << 16;

    // update p2060 object
    pThis->SyncStartDelay = syncSkew;

    //
    // Perform the opposite adjustment for returning the value to the user.
    // Multiply by the ratio 7.6293945 / 7
    //
    temp = syncSkew;
    temp *= 76293945; // Safe because temp is 64 bits
    temp /= 70000000;
    syncSkew = (NvU32)temp; // Safe because the HW register is only 24 bits wide to begin with

    //
    // Could be higher than maximum because we multiplied by greater than 1,
    // but this would only happen by someone manually programming the register
    //
    NV_CHECK_OR_ELSE(LEVEL_INFO,
                     syncSkew <= NV_P2061_V204_SYNC_SKEW_MAX_UNITS,
                     syncSkew = NV_P2061_V204_SYNC_SKEW_MAX_UNITS);

    //
    // Returning this value with stability to the user:
    // The programmed value can change slightly due to divisions, so if we read
    // from the hardware and find a value that is very close, just return the
    // last sent value. This assures stability even through multiple cycles of
    // set-get-set-get.
    //
    if (pThis->lastUserSkewSent != NV_P2061_V204_SYNC_SKEW_INVALID)
    {
        NvS32 difference;

        difference = syncSkew - pThis->lastUserSkewSent;
        if ((difference >= -2) && (difference <= 2))
        {
            syncSkew = pThis->lastUserSkewSent;
        }
    }
    *pSyncSkew = syncSkew;

    return NV_OK;
}

// Determine and write the proper RasterSync Decode Mode to the CONTROL5 register
NV_STATUS
gsyncSetRasterSyncDecodeMode_P2061_V300
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExtDev
)
{
    NV2080_CTRL_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE_PARAMS
            rasterSyncDecodeModeParams;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU8    old_data, data;

    //
    // Get the raster sync mode flag from the GPU
    // This is only used in P2061 v3.00+ for gsyncSetRasterDecodeMode()
    //
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient,
        pGpu->hInternalSubdevice, NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_RASTER_SYNC_DECODE_MODE,
        &rasterSyncDecodeModeParams, sizeof(rasterSyncDecodeModeParams)));

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2061_CONTROL5, &data));
    old_data = data;

    // Modify the register field with the corresponding mode
    switch(rasterSyncDecodeModeParams.rasterSyncDecodeMode)
    {
        case NV_P2061_CONTROL5_RASTER_SYNC_DECODE_MODE_VSYNC_SHORT_PULSE:
            data = FLD_SET_DRF(_P2061, _CONTROL5, _RASTER_SYNC_DECODE_MODE,
                               _VSYNC_SHORT_PULSE, data);
            break;

        case NV_P2061_CONTROL5_RASTER_SYNC_DECODE_MODE_VSYNC_SINGLE_PULSE:
            data = FLD_SET_DRF(_P2061, _CONTROL5, _RASTER_SYNC_DECODE_MODE,
                               _VSYNC_SINGLE_PULSE, data);
            break;
       default:
            return NV_ERR_INVALID_PARAMETER;
    };

    // Write if changed
    if (data != old_data)
    {
        NV_ASSERT_OK_OR_RETURN(
            writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2061_CONTROL5, data));
    }

    return NV_OK;
}

// Read the VRR setting from the CONTROL5 register
NV_STATUS
gsyncGetVRR_P2061_V300
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NvU32             *pVRR
)
{
    NvU8    data;

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2061_CONTROL5, &data));

    *pVRR =
        (DRF_VAL(_P2061, _CONTROL5, _SYNC_MODE, data) == NV_P2061_CONTROL5_SYNC_MODE_VARIABLE_REFRESH_RATE) ?
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MODE_VARIABLE_REFRESH_RATE :
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MODE_FIXED_REFRESH_RATE;

    return NV_OK;
}

// Read the VRR setting from the CONTROL5 register
NV_STATUS
gsyncSetVRR_P2061_V300
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NvU32              refreshRateMode
)
{
    NvU8    data, old_data;
    NvU32   new_setting;

    NV_ASSERT_OK_OR_RETURN(readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2061_CONTROL5, &data));
    old_data = data;

    switch (refreshRateMode)
    {
        case NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MODE_FIXED_REFRESH_RATE:
            new_setting = NV_P2061_CONTROL5_SYNC_MODE_FIXED_REFRESH_RATE;
            break;
        case NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MODE_VARIABLE_REFRESH_RATE:
            new_setting = NV_P2061_CONTROL5_SYNC_MODE_VARIABLE_REFRESH_RATE;
            break;
        default:
            return NV_ERR_INVALID_PARAMETER;
    }

    data = FLD_SET_DRF_NUM(_P2061, _CONTROL5, _SYNC_MODE, new_setting, data);
    if (data != old_data)
    {
        NV_ASSERT_OK_OR_RETURN(
            writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2061_CONTROL5, data));
    }

    return NV_OK;
}
