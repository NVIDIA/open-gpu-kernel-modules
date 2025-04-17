/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "disp/v02_04/dev_disp.h"
#include "displayport/displayport.h"
#include "displayport/displayport2x.h"

NV_STATUS
kdispComputeDpModeSettings_v02_04
(
    OBJGPU             *pGpu,
    KernelDisplay      *pKernelDisplay,
    NvU32               headIndex, // not being used.
    DPMODESETDATA      *pDpmodesetData,
    DPIMPINFO          *dpInfo
)
{
    // number of link clocks per line.
    NvS32 vblank_symbols    = 0;
    NvS32 hblank_symbols    = 0;
    NvU64 PrecisionFactor, ratioF, watermarkF;
    NvU32 linkFreqHz        = 0;
    NvU32 dscFactor         = pDpmodesetData->bDscEnable ? DSC_BPP_FACTOR : 1;

    NvU32 numLanesPerLink   = pDpmodesetData->laneCount;

    NvS32 BlankingBits, PixelSteeringBits , NumBlankingLinkClocks;

    NvU32 hActive              = pDpmodesetData->SetRasterBlankStartX -
                                 pDpmodesetData->SetRasterBlankEndX;
    NvU32 minHBlank            = 0;
    NvU32 num_symbols_per_line = 0;
    NvU32 watermarkAdjust      = DP_CONFIG_WATERMARK_ADJUST;
    NvU32 watermarkMinimum     = DP_CONFIG_WATERMARK_LIMIT;

    dpInfo->bEnhancedFraming = pDpmodesetData->bEnhancedFraming;

    if (pDpmodesetData->bDscEnable)
    {
        if (pDpmodesetData->dp2LinkBw != 0)
        {
            linkFreqHz = (NvU32)((((NvU64)pDpmodesetData->dp2LinkBw) * DP_LINK_BW_FREQ_MULTI_10M_TO_MBPS * 97) / 100);
        }
        else if (pDpmodesetData->linkBw != 0)
        {
            linkFreqHz = (NvU32)((((NvU64)pDpmodesetData->linkBw) * DP_LINK_BW_FREQ_MULTI_MBPS * 97) / 100);
        }
    }
    else
    {
        if (pDpmodesetData->dp2LinkBw != 0)
        {
            linkFreqHz = (NvU32)((((NvU64)pDpmodesetData->dp2LinkBw) * DP_LINK_BW_FREQ_MULTI_10M_TO_MBPS * 995) / 1000);
        }
        else if (pDpmodesetData->linkBw != 0)
        {
            linkFreqHz = (NvU32)((((NvU64)pDpmodesetData->linkBw) * DP_LINK_BW_FREQ_MULTI_MBPS * 995) / 1000);
        }
    }

    if (!linkFreqHz || !pDpmodesetData->laneCount|| !pDpmodesetData->PClkFreqHz)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "One of linkFreq (%d Hz), pDpmodesetData->laneCount (%d Hz) or PClkFreq"
                  " (%lld Hz) came in as zero. Report issue to client (DD)\n",
                  linkFreqHz, pDpmodesetData->laneCount,
                  pDpmodesetData->PClkFreqHz);
        NV_ASSERT(linkFreqHz && pDpmodesetData->laneCount && pDpmodesetData->PClkFreqHz);
    }

    // Check if we have a valid laneCount as currently we support only upto 8-lanes
    if (!IS_VALID_LANECOUNT(pDpmodesetData->laneCount))
    {

        DBG_BREAKPOINT();
    }

    dpInfo->tuSize = 64;
    PrecisionFactor = 100000;
    ratioF  =
        ((NvU64)pDpmodesetData->PClkFreqHz * pDpmodesetData->bpp * PrecisionFactor)
        / dscFactor;
    ratioF /= 8 * (NvU64) linkFreqHz * pDpmodesetData->laneCount;

    {
        watermarkAdjust = DP_CONFIG_INCREASED_WATERMARK_ADJUST;
        watermarkMinimum = DP_CONFIG_INCREASED_WATERMARK_LIMIT;
    }

    watermarkF = ratioF * dpInfo->tuSize * (PrecisionFactor - ratioF)  / PrecisionFactor;
    dpInfo->waterMark =
        (NvU32)(watermarkAdjust +
        ((2 * (pDpmodesetData->bpp * PrecisionFactor / (8 * numLanesPerLink * dscFactor))
        + watermarkF) / PrecisionFactor));

    num_symbols_per_line = (hActive * pDpmodesetData->bpp) /
                           (8 * pDpmodesetData->laneCount * dscFactor);

    if (dpInfo->waterMark > 39)
    {
        dpInfo->waterMark = 39;
        DBG_BREAKPOINT();
    }
    else if (dpInfo->waterMark < watermarkMinimum)
    {
        dpInfo->waterMark = watermarkMinimum;
    }
    else if (dpInfo->waterMark > num_symbols_per_line)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "\n\t\twatermark greater than number of symbols in the line\n");
        DBG_BREAKPOINT();
        dpInfo->waterMark = num_symbols_per_line;
    }

    // minHBlank calculation.
    if (pDpmodesetData->bMultiStream)
    {
        minHBlank   =
            (((hActive % 4) > 0) ?
            (((4-(hActive % 4)) * pDpmodesetData->bpp) / dscFactor) : 0 )
            + (160 +  6 * 4);

        minHBlank   = minHBlank + (32 - minHBlank % 32);

        // bpp - 1 --> Rounding
        minHBlank   = (minHBlank + (pDpmodesetData->bpp/dscFactor) - 1)
                      / (pDpmodesetData->bpp/dscFactor);
    }
    else
    {
        BlankingBits = 3 * 8 * numLanesPerLink +
                      (pDpmodesetData->bEnhancedFraming ? 3*8*numLanesPerLink : 0);

        BlankingBits += 3*8*4;

        PixelSteeringBits = (hActive % numLanesPerLink) ?
                            ((numLanesPerLink - hActive % numLanesPerLink) * pDpmodesetData->bpp) / dscFactor : 0;

        BlankingBits += PixelSteeringBits;
        NumBlankingLinkClocks =
            (NvU32)(((NvU64)BlankingBits * PrecisionFactor) / (8 * numLanesPerLink));

        minHBlank =
            (NvU32)(NumBlankingLinkClocks * pDpmodesetData->PClkFreqHz / linkFreqHz / PrecisionFactor);

        minHBlank += 12;

        NV_ASSERT (hActive >= 60);
    }

    // Min hblank should be hit
    NV_ASSERT ((pDpmodesetData->SetRasterSizeWidth - hActive) >= minHBlank);

    hblank_symbols =
        (NvS32)(((NvU64)(pDpmodesetData->SetRasterSizeWidth - hActive - minHBlank)
        * linkFreqHz) / pDpmodesetData->PClkFreqHz);

    hblank_symbols -= 1;
    hblank_symbols -= 3;

    hblank_symbols -= numLanesPerLink == 1 ? 9  : numLanesPerLink == 2 ? 6 : 3;

    dpInfo->hBlankSym = (hblank_symbols < 0) ? 0 : hblank_symbols;

    if (hActive < 40)
    {
      vblank_symbols = 0;
    }
    else
    {
      vblank_symbols =
        (NvS32)(((NvU64)(hActive - 40) * linkFreqHz) /
        pDpmodesetData->PClkFreqHz) - 1;

      vblank_symbols -= numLanesPerLink == 1 ? 39 :
        numLanesPerLink == 2 ? 21 : 12;
    }

    dpInfo->vBlankSym = (vblank_symbols < 0) ? 0 : vblank_symbols;

    NV_PRINTF(LEVEL_INFO, "Minimum HBlank required is %d\n", minHBlank);

    // Change this to _ERRORS for now but eventually we'll back to _INFO
    NV_PRINTF(LEVEL_ERROR,
              "\n\t\t[IN]: PixelClockHz:%lld PixelDepth:%d LinkBw:%d LaneCount:%d "
              "TuSize: %d DSCEnable:%s\n"
              "\t\t[OUT]:WaterMark:%d\n"
              "\t\t[OUT]:VBlankSymbols:%d HBlankSymbols:%d\n",
              pDpmodesetData->PClkFreqHz, pDpmodesetData->bpp, linkFreqHz,
              pDpmodesetData->laneCount, dpInfo->tuSize,
              (pDpmodesetData->bDscEnable ? "NV_TRUE" : "NV_FALSE"),
              dpInfo->waterMark,
              dpInfo->vBlankSym, dpInfo->hBlankSym);

    return NV_OK;
}

//
// kdispSetAccel_v02_04: wrapper function for dispApplyAccelerators_v02_04
//
NV_STATUS
kdispSetAccel_v02_04
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvHandle     hClient,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance,
    NvU32        accelerators,
    NvU32        accelMask
)
{
    NvU32 currAccelerators;
    NV_STATUS status = NV_OK;

    //
    // The channel must be allocated before we try to do anything with it.
    // Note that we are checking sw state. The hw state is more appropriate
    // check but FCode doesn't alloc base channel. So, it should be fine
    // to use the sw state. In fact, it's better to have this in order to trap
    // any inadvertent alloc by someone else.
    //
    if ( !kdispIsChannelAllocatedSw_HAL(pGpu, pKernelDisplay, channelClass, channelInstance))
        return NV_ERR_INVALID_CHANNEL;

    if (channelClass == dispChnClass_Core)
    {
        NvU32             dispChannelNum;

        status = kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, &dispChannelNum);
        if (status != NV_OK)
            return status;

        if (pKernelDisplay->pClientChannelTable[dispChannelNum].pClient->hClient != hClient)
        return NV_ERR_INVALID_OWNER;
    
    }

    kdispGetAccelerators_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, NULL, &currAccelerators);

    //
    // change the bits indicated by the accel mask. This bitwise implements:
    // if (accelMask) then accelerators
    // else currAccelerators
    //
    accelerators = (accelMask & accelerators) | (~accelMask & currAccelerators);

    kdispApplyAccelerators_HAL(pGpu, pKernelDisplay, channelClass, channelInstance,
                                NULL, accelerators, NV_FALSE);
    return status;
}

//
//  dispGetAccel_v02_04: Wrapper function for dispGetAccelerators_v02_04
//
NV_STATUS
kdispGetAccel_v02_04
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvHandle     hClient,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance,
    NvU32        *accelerators
)
{
    NV_STATUS status = NV_OK;

    //
    // The channel must be allocated before we try to do anything with it.
    // Note that we are checking sw state. The hw state is more appropriate
    // check but FCode doesn't alloc base channel. So, it should be fine
    // to use the sw state. In fact, it's better to have this in order to trap
    // any inadvertent alloc by someone else.
    //
    if ( !kdispIsChannelAllocatedSw_HAL(pGpu, pKernelDisplay, channelClass, channelInstance))
        return NV_ERR_INVALID_CHANNEL;

    if (channelClass == dispChnClass_Core)
    {
        NvU32             dispChannelNum;

        status = kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, &dispChannelNum);
        if (status != NV_OK)
            return status;

        if (pKernelDisplay->pClientChannelTable[dispChannelNum].pClient->hClient != hClient)
            return NV_ERR_INVALID_OWNER;
    }

    kdispGetAccelerators_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, NULL, accelerators);

    return NV_OK;
}

NvBool
kdispIsChannelAllocatedSw_KERNEL_v02_04
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance
)
{
    NvU32 dispChannelNum;

    if (kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, &dispChannelNum) != NV_OK)
        return NV_FALSE;

    if (pKernelDisplay->pClientChannelTable[dispChannelNum].bInUse == NV_TRUE)
    {
        //
        // HW should also think that channel has been allocated or we have a
        // bookkeeping issue.
        //
        NV_ASSERT(kdispIsChannelAllocatedHw_HAL(pGpu, pKernelDisplay, channelClass, channelInstance) == NV_TRUE);

        return NV_TRUE;
    }

    return NV_FALSE;
}