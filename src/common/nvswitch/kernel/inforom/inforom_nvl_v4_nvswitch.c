/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "inforom/inforom_nvswitch.h"
#include "inforom/inforom_nvl_v4_nvswitch.h"
#include "ls10/ls10.h"

void
inforom_nvl_v4_update_correctable_error_rates
(
    INFOROM_NVL_CORRECTABLE_ERROR_RATE_STATE_V4S *pState,
    NvU8 link,
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *pCounts
)
{
    NvU32 i;
    NvU32 tempFlitCrc, tempRxLinkReplay, tempTxLinkReplay, tempLinkRecovery;
    NvU32 tempLaneCrc[4];

    //
    // If the registers have decreased from last reported, then
    // they must have been reset or have overflowed. Set the last
    // register value to 0.
    //
    if (pCounts->flitCrc < pState->lastRead[link].flitCrc)
    {
        pState->lastRead[link].flitCrc = 0;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(pState->lastRead[link].laneCrc); i++)
    {
        if (pCounts->laneCrc[i] < pState->lastRead[link].laneCrc[i])
        {
            pState->lastRead[link].laneCrc[i] = 0;
        }
    }

    // Get number of new errors since the last register read
    tempFlitCrc       = pCounts->flitCrc;
    pCounts->flitCrc -= pState->lastRead[link].flitCrc;

    // Update errors per minute with error delta
    m_inforom_nvl_get_new_errors_per_minute(pCounts->flitCrc,
            &pState->errorsPerMinute[link].flitCrc);

    // Save the current register value for the next callback
    pState->lastRead[link].flitCrc = tempFlitCrc;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pState->lastRead[link].laneCrc); i++)
    {
        tempLaneCrc[i] = pCounts->laneCrc[i];
        pCounts->laneCrc[i] -= pState->lastRead[link].laneCrc[i];
        m_inforom_nvl_get_new_errors_per_minute(pCounts->laneCrc[i],
                &pState->errorsPerMinute[link].laneCrc[i]);

        pState->lastRead[link].laneCrc[i] = tempLaneCrc[i];
    }

    //
    // We don't track rates for the following errors. We just need to stash
    // the current register value and update pCounts with the delta since
    // the last register read.
    //
    if (pCounts->rxLinkReplay < pState->lastRead[link].rxLinkReplay)
    {
        pState->lastRead[link].rxLinkReplay = 0;
    }
    tempRxLinkReplay = pCounts->rxLinkReplay;
    pCounts->rxLinkReplay -= pState->lastRead[link].rxLinkReplay;
    pState->lastRead[link].rxLinkReplay = tempRxLinkReplay;

    if (pCounts->txLinkReplay < pState->lastRead[link].txLinkReplay)
    {
        pState->lastRead[link].txLinkReplay = 0;
    }
    tempTxLinkReplay = pCounts->txLinkReplay;
    pCounts->txLinkReplay -= pState->lastRead[link].txLinkReplay;
    pState->lastRead[link].txLinkReplay = tempTxLinkReplay;

    if (pCounts->linkRecovery < pState->lastRead[link].linkRecovery)
    {
        pState->lastRead[link].linkRecovery = 0;
    }
    tempLinkRecovery = pCounts->linkRecovery;
    pCounts->linkRecovery -= pState->lastRead[link].linkRecovery;
    pState->lastRead[link].linkRecovery = tempLinkRecovery;
}
