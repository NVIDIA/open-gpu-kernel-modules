/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_watermark.cpp                                                  *
*    DP watermark IsModePossible calculations                               *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_watermark.h"
#include "dp_linkconfig.h"
#include "displayport.h"

#define FEC_TOTAL_SYMBOLS_PER_BLK(lanes)  ((NvU32)((lanes == 1) ? 512U : 256U))
#define FEC_PARITY_SYMBOLS_PER_BLK(lanes)  ((NvU32)((lanes == 1) ? 12U : 6U))
//return max number of FEC parity symbols in x link clock cycles 
#define FEC_PARITY_SYM_SST(lanes, x)   (DP_MIN((NvU32)(x) % FEC_TOTAL_SYMBOLS_PER_BLK(lanes), FEC_PARITY_SYMBOLS_PER_BLK(lanes)) + (NvU32)(x) / FEC_TOTAL_SYMBOLS_PER_BLK(lanes) * FEC_PARITY_SYMBOLS_PER_BLK(lanes) + FEC_PARITY_SYMBOLS_PER_BLK(lanes) + 1U)
#define FEC_PARITY_SYM_MST(lanes, x)   (DP_MIN((NvU32)(x) % FEC_TOTAL_SYMBOLS_PER_BLK(lanes), FEC_PARITY_SYMBOLS_PER_BLK(lanes)) + (NvU32)(x) / FEC_TOTAL_SYMBOLS_PER_BLK(lanes) * FEC_PARITY_SYMBOLS_PER_BLK(lanes) + 1U)


bool DisplayPort::isModePossibleMST
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    Watermark  * dpInfo
)
{
    //
    // For MST, use downspread 0.6%
    //
    NvU64 linkFreq = linkConfig.peakRate * 994 / 1000;

    //
    //  This function is for multistream only!
    //
    DP_ASSERT( linkConfig.multistream );

    if(!modesetInfo.pixelClockHz || !modesetInfo.depth)
    {
        DP_ASSERT(0 && "INVALID PIXEL CLOCK and DEPTH sent by the client ");
        return false;
    }

    // depth is multiplied by 16 in case of DSC enable
    unsigned DSC_FACTOR = modesetInfo.bEnableDsc ? 16 : 1;

    // Extra bits that we need to send
    //(hActiveDiv4Remainder > 0 ? (4- hActiveDiv4Remainder) : 0) -->
    // Number of extra pixels that we need to insert due to mapping pixels
    // to the DP lanes. (4 lanes for MS)
    //
    // 160 --> Extra bits that we need to send during horizontal blanking
    // (BS+VBID+MVID+MAUD+BE) => 5*8*num_lanes
    //
    // 6 * 4 --> Pixel padding worst case
    //
    NvU32 minHBlank   = ( ((modesetInfo.surfaceWidth % 4) > 0) ? ((4-(modesetInfo.surfaceWidth % 4)) * modesetInfo.depth)/ DSC_FACTOR : 0 ) + (160 +  6 * 4);

    // Rounding to nearest multiple of 32 since we always send 32 bits in one time slice
    minHBlank   = minHBlank + (32 - minHBlank % 32);

    // bpp - 1 --> Rounding
    minHBlank = ((minHBlank * DSC_FACTOR) + modesetInfo.depth - (1 * DSC_FACTOR))/modesetInfo.depth;

    if (minHBlank > modesetInfo.rasterWidth - modesetInfo.surfaceWidth)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Blanking Width is smaller than minimum permissible value."));
        return false;
    }

    // Bug 702290 - Active Width should be greater than 60
    if (modesetInfo.surfaceWidth <= 60)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Minimum Horizontal Active Width <= 60 not supported."));
        return false;
    }

    NvS32 vblank_symbols;
    NvS32 hblank_symbols = (NvS32)(((NvU64)(modesetInfo.rasterWidth - modesetInfo.surfaceWidth - minHBlank) * linkFreq) / modesetInfo.pixelClockHz);

    //reduce HBlank Symbols to account for secondary data packet
    hblank_symbols -= 1; //Stuffer latency to send BS
    hblank_symbols -= 3; //SPKT latency to send data to stuffer

    hblank_symbols -= linkConfig.lanes == 1 ? 9  : linkConfig.lanes == 2 ? 6 : 3;

    dpInfo->hBlankSym = (hblank_symbols < 0) ? 0 : hblank_symbols;


    //
    // Audio IMP calculations
    //   Perform the related audio calculation to determine the number of extra symbols needed.
    //
    NvU32 twoChannelAudio_symbols = 0;

    if (modesetInfo.twoChannelAudioHz != 0)
    {
        // 1-2 channel case
        NvU32 samples = (NvU32)divide_ceil(modesetInfo.twoChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz);

        // Round to the next even sample to account for stuffing (2 ch, 4 lanes)
        samples = samples + (2 - samples % 2);

        // Convert sample count to symbols
        twoChannelAudio_symbols = 10 * samples + 16;
    }

    NvU32 eightChannelAudio_symbols = 0;
    if (modesetInfo.eightChannelAudioHz != 0)
    {
        // 3-8 channel case
        NvU32 samples = (NvU32)divide_ceil(modesetInfo.eightChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz);

        // Convert sample count to symbols
        eightChannelAudio_symbols = 40 * samples + 16;
    }

    if (dpInfo->hBlankSym < DP_MAX(twoChannelAudio_symbols, eightChannelAudio_symbols))
    {
        return false;
    }

    // Refer to dev_disp.ref for more information.
    // # symbols/vblank = ((SetRasterBlankEnd.X + SetRasterSize.Width - SetRasterBlankStart.X - 40) * link_clk / pclk) - Y - 1;
    // where Y = (# lanes == 4) 12 : (# lanes == 2) ? 21 : 39
    if (modesetInfo.surfaceWidth < 40)
    {
        vblank_symbols = 0;
    }
    else
    {
        vblank_symbols = (NvS32)(((NvU64)(modesetInfo.surfaceWidth - 40) * linkFreq) /  modesetInfo.pixelClockHz) - 1;

        vblank_symbols -= linkConfig.lanes == 1 ? 39  : linkConfig.lanes == 2 ? 21 : 12;
    }

    dpInfo->vBlankSym = (vblank_symbols < 0) ? 0 : vblank_symbols;

    return true;
}


bool DisplayPort::isModePossibleSST
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    Watermark  * dpInfo,
    bool bUseIncreasedWatermarkLimits
)
{
    //
    //  This function is for single stream only!
    //
    DP_ASSERT( !linkConfig.multistream );

    unsigned watermarkAdjust = DP_CONFIG_WATERMARK_ADJUST;
    unsigned watermarkMinimum = DP_CONFIG_WATERMARK_LIMIT;
    // depth is multiplied by 16 in case of DSC enable
    unsigned DSC_FACTOR = modesetInfo.bEnableDsc ? 16 : 1;

    if(bUseIncreasedWatermarkLimits)
    {
        watermarkAdjust = DP_CONFIG_INCREASED_WATERMARK_ADJUST;
        watermarkMinimum = DP_CONFIG_INCREASED_WATERMARK_LIMIT;
    }

    if(!modesetInfo.pixelClockHz || !modesetInfo.depth)
    {
        DP_ASSERT(0 && "INVALID PIXEL CLOCK or DEPTH sent by the client ");
        return false;
    }
    // number of link clocks per line.
    int vblank_symbols      = 0;
    NvU64 PrecisionFactor, ratioF, watermarkF;

    NvU32 numLanesPerLink = linkConfig.lanes;

    DP_ASSERT(!linkConfig.multistream && "MST!");

    // Check if we have a valid laneCount as currently we support only up to 4-lanes
    if (!IS_VALID_LANECOUNT(linkConfig.lanes))
    {
        //
        // Print debug message and Assert. All calculations assume a max of 8 lanes
        // & any increase in lanes should cause these calculation to be updated
        //
        DP_LOG(("NVRM: %s: ERROR: LaneCount - %d is not supported for waterMark calculations.",
                __FUNCTION__, linkConfig.lanes));
        DP_LOG(("Current support is only up to 4-Lanes & any change/increase in supported lanes "
                "should be reflected in waterMark calculations algorithm. "
                "Ex: See calc for minHBlank variable below"));

        DP_ASSERT(0);
        return false;
    }

    if ((modesetInfo.pixelClockHz * modesetInfo.depth) >= (8 * linkConfig.minRate * linkConfig.lanes * DSC_FACTOR))
    {
        return false;
    }

    //
    // For DSC, if (pclk * bpp) < (1/64 * orclk * 8 * lanes) then some TU may end up with
    // 0 active symbols. This may cause HW hang. Bug 200379426
    //
    if ((modesetInfo.bEnableDsc) &&
        ((modesetInfo.pixelClockHz * modesetInfo.depth) < ((8 * linkConfig.minRate * linkConfig.lanes * DSC_FACTOR) / 64)))
    {
        return false;
    }

    //
    //  Perform the SST calculation.
    //    For auto mode the watermark calculation does not need to track accumulated error the
    //    formulas for manual mode will not work.  So below calculation was extracted from the DTB.
    //
    dpInfo->tuSize = 64;
    PrecisionFactor = 100000;
    ratioF = ((NvU64)modesetInfo.pixelClockHz * modesetInfo.depth * PrecisionFactor) / DSC_FACTOR;

    ratioF /= 8 * (NvU64) linkConfig.minRate * linkConfig.lanes;

    if (PrecisionFactor < ratioF) // Assert if we will end up with a negative number in below
        return false;

    watermarkF = ratioF * dpInfo->tuSize * (PrecisionFactor - ratioF)  / PrecisionFactor;
    dpInfo->waterMark = (unsigned)(watermarkAdjust + ((2 * (modesetInfo.depth * PrecisionFactor / (8 * numLanesPerLink * DSC_FACTOR)) + watermarkF) / PrecisionFactor));

    //
    //  Bounds check the watermark
    //
    NvU32 numSymbolsPerLine = (modesetInfo.surfaceWidth * modesetInfo.depth) / (8 * linkConfig.lanes * DSC_FACTOR);

    if (dpInfo->waterMark > 39 || dpInfo->waterMark > numSymbolsPerLine)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: watermark should not be greater than 39."));
        return false;
    }

    //
    //  Clamp the low side
    //
    if (dpInfo->waterMark < watermarkMinimum)
        dpInfo->waterMark = watermarkMinimum;

    //Bits to send BS/BE/Extra symbols due to pixel padding
    //Also accounts for enhanced framing.
    NvU32 BlankingBits = 3*8*numLanesPerLink + (linkConfig.enhancedFraming ? 3*8*numLanesPerLink : 0);

    //VBID/MVID/MAUD sent 4 times all the time
    BlankingBits += 3*8*4;

    NvU32 surfaceWidthPerLink = modesetInfo.surfaceWidth;

    //Extra bits sent due to pixel steering
    NvU32 PixelSteeringBits = (surfaceWidthPerLink % numLanesPerLink) ? (((numLanesPerLink - surfaceWidthPerLink % numLanesPerLink) * modesetInfo.depth) / DSC_FACTOR) : 0;

    BlankingBits += PixelSteeringBits;
    NvU64 NumBlankingLinkClocks = (NvU64)BlankingBits * PrecisionFactor / (8 * numLanesPerLink);
    NvU32 MinHBlank = (NvU32)(NumBlankingLinkClocks * modesetInfo.pixelClockHz/ linkConfig.minRate / PrecisionFactor);
    MinHBlank += 12;

    if (MinHBlank > modesetInfo.rasterWidth - modesetInfo.surfaceWidth)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Blanking Width is smaller than minimum permissible value."));
        return false;
    }

    // Bug 702290 - Active Width should be greater than 60
    if (modesetInfo.surfaceWidth <= 60)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Minimum Horizontal Active Width <= 60 not supported."));
        return false;
    }


    NvS32 hblank_symbols = (NvS32)(((NvU64)(modesetInfo.rasterWidth - modesetInfo.surfaceWidth - MinHBlank) * linkConfig.minRate) / modesetInfo.pixelClockHz);

    //reduce HBlank Symbols to account for secondary data packet
    hblank_symbols -= 1; //Stuffer latency to send BS
    hblank_symbols -= 3; //SPKT latency to send data to stuffer

    hblank_symbols -= numLanesPerLink == 1 ? 9  : numLanesPerLink == 2 ? 6 : 3;

    dpInfo->hBlankSym = (hblank_symbols < 0) ? 0 : hblank_symbols;

    //
    // Audio IMP calculations
    //

    // From dev_disp.ref:

    // The packet generation logic needs to know the length of the hblank period. If there is no room
    // in the current hblank for a new packet, it will be delayed until the next blanking period. This
    // field should be programmed during the second Supervisor interrupt based on the new raster
    // dimensions.

    // ...

    // --------------------------------------
    // The following formulas can be used to calculate the maximum audio sampling rate that can
    // be supported by DisplayPort given the current raster dimensions. DisplayPort has much more
    // bandwidth during blanking periods than HDMI has, so hblank size is less of an issue.

    // ...

    // Size of a packet for 2ch audio = 20 symbols (up to 2 samples)
    // Size of a packet for 8ch audio = 40 symbols
    // Size of an audio packet header plus control symbols = 2*#lanes + 8 symbols (assuming < 32 samples per line)
    // number of packets/hblank for 2ch audio = Floor ((number of free symbols/hblank - (2*#lanes + 8) / 20)
    // number of packets/hblank for 8ch audio = Floor ((number of free symbols/hblank - (2*#lanes + 8) / 40)

    // Maximum audio sample rate possible:
    // number of audio samples/line = SetRasterSize.Width * audio_fs / pclk
    // number of audio packets needed for 2ch audio = Ceiling(SetRasterSize.Width * audio_fs / (pclk*2))
    // number of audio packets needed for 3-8ch audio = SetRasterSize.Width * audio_fs / pclk

    // If number of audio packets needed > number of packets/hblank, then you cannot support that audio frequency

    // Note that the hBlankSym calculated is per lane. So the number of symbols available for audio is
    // (number of lanes * hBlankSym).
    // The calculation of audio packets per Hblank needs to account for the following -
    // 2 symbols for SS and SE; 8 symbols for header; and additional 2 symbols to account for actual values used by HW.
    // --------------------------------------

    if (modesetInfo.twoChannelAudioHz != 0)
    {
        if ((dpInfo->hBlankSym * numLanesPerLink) < (2 * numLanesPerLink + 8))
        {
            // There aren't enough symbols/hblank available.
            return false;
        }

        NvU32 twoChannelAudioPacketsPerHBlank = (NvU32)divide_floor(((dpInfo->hBlankSym * numLanesPerLink) - (2 * numLanesPerLink) - 8 - (2 * numLanesPerLink)), 20);

        NvU32 twoChannelAudioPackets = (NvU32)divide_ceil(modesetInfo.twoChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz * 2);

        if (twoChannelAudioPackets > twoChannelAudioPacketsPerHBlank)
        {
            // There aren't enough packets/hblank available.
            return false;
        }
    }

    if (modesetInfo.eightChannelAudioHz != 0)
    {
        if ((dpInfo->hBlankSym * numLanesPerLink) < (2 * numLanesPerLink + 8))
        {
            // There aren't enough symbols/hblank available.
            return false;
        }

        NvU32 eightChannelAudioPacketsPerHBlank = (NvU32)divide_floor(((dpInfo->hBlankSym * numLanesPerLink) - (2 * numLanesPerLink) - 8 - (2 * numLanesPerLink)), 40);

        NvU32 eightChannelAudioPackets = (NvU32)divide_ceil(modesetInfo.eightChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz);

        if (eightChannelAudioPackets > eightChannelAudioPacketsPerHBlank)
        {
            // There aren't enough packets/hblank available.
            return false;
        }
    }


    // Refer to dev_disp.ref for more information.
    // # symbols/vblank = ((SetRasterBlankEnd.X + SetRasterSize.Width - SetRasterBlankStart.X - 40) * link_clk / pclk) - Y - 1;
    // where Y = (# lanes == 4) 12 : (# lanes == 2) ? 21 : 39
    if (modesetInfo.surfaceWidth < 40)
    {
        vblank_symbols = 0;
    }
    else
    {
        vblank_symbols = (NvS32)(((NvU64)(modesetInfo.surfaceWidth - 40) * linkConfig.minRate) /  modesetInfo.pixelClockHz) - 1;

        vblank_symbols -= numLanesPerLink == 1 ? 39  : numLanesPerLink == 2 ? 21 : 12;
    }

    dpInfo->vBlankSym = (vblank_symbols < 0) ? 0 : vblank_symbols;

    return true;
}

bool DisplayPort::isModePossibleSSTWithFEC
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    Watermark  * dpInfo,
    bool bUseIncreasedWatermarkLimits
)
{
    //
    // This function is for single stream only!
    // Refer to Bug 200406501 and 200401850 for algorithm
    //
    DP_ASSERT( !linkConfig.multistream );

    unsigned watermarkAdjust = DP_CONFIG_WATERMARK_ADJUST;
    unsigned watermarkMinimum = DP_CONFIG_WATERMARK_LIMIT;
    // depth is multiplied by 16 in case of DSC enable
    unsigned DSC_FACTOR = modesetInfo.bEnableDsc ? 16 : 1;

    if(bUseIncreasedWatermarkLimits)
    {
        watermarkAdjust = DP_CONFIG_INCREASED_WATERMARK_ADJUST;
        watermarkMinimum = DP_CONFIG_INCREASED_WATERMARK_LIMIT;
    }

    if(!modesetInfo.pixelClockHz || !modesetInfo.depth)
    {
        DP_ASSERT(0 && "INVALID PIXEL CLOCK or DEPTH sent by the client ");
        return false;
    }
    // number of link clocks per line.
    int vblank_symbols = 0;
    NvU64 PrecisionFactor, ratioF, watermarkF;
    NvS32 w0, s;

    NvU32 numLanesPerLink = linkConfig.lanes;

    DP_ASSERT(!linkConfig.multistream && "MST!");

    // Check if we have a valid laneCount as currently we support only up to 4-lanes
    if (!IS_VALID_LANECOUNT(linkConfig.lanes))
    {
        //
        // Print debug message and Assert. All calculations assume a max of 8 lanes
        // & any increase in lanes should cause these calculation to be updated
        //
        DP_LOG(("NVRM: %s: ERROR: LaneCount - %d is not supported for waterMark calculations.",
                __FUNCTION__, linkConfig.lanes));
        DP_LOG(("Current support is only up to 4-Lanes & any change/increase in supported lanes "
                "should be reflected in waterMark calculations algorithm. "
                "Ex: See calc for minHBlank variable below"));

        DP_ASSERT(0);
        return false;
    }

    if ((modesetInfo.pixelClockHz * modesetInfo.depth) >= (8 * linkConfig.minRate * linkConfig.lanes * DSC_FACTOR))
    {
        return false;
    }

    //
    // For DSC, if (pclk * bpp) < (1/64 * orclk * 8 * lanes) then some TU may end up with
    // 0 active symbols. This may cause HW hang. Bug 200379426
    //
    if ((modesetInfo.bEnableDsc) &&
        ((modesetInfo.pixelClockHz * modesetInfo.depth) < ((8 * linkConfig.minRate * linkConfig.lanes * DSC_FACTOR) / 64)))
    {
        return false;
    }

    //
    //  Perform the SST calculation.
    //    For auto mode the watermark calculation does not need to track accumulated error the
    //    formulas for manual mode will not work.  So below calculation was extracted from the DTB.
    //
    dpInfo->tuSize = 64;
    PrecisionFactor = 100000;
    ratioF = ((NvU64)modesetInfo.pixelClockHz * modesetInfo.depth * PrecisionFactor) / DSC_FACTOR;

    ratioF /= 8 * (NvU64)linkConfig.minRate * linkConfig.lanes;

    if (PrecisionFactor < ratioF) // Assert if we will end up with a negative number in below
        return false;

    watermarkF = (ratioF * dpInfo->tuSize * (PrecisionFactor - ratioF)) / PrecisionFactor;

    w0 = (8 / linkConfig.lanes);
    if (linkConfig.bEnableFEC)
    {
        s = (linkConfig.lanes == 1) ? 15 : 10;
    }
    else
    {
        s = 3 - w0;
    }

    dpInfo->waterMark = (unsigned)(watermarkAdjust + ((3 * (modesetInfo.depth * PrecisionFactor / (8 * numLanesPerLink * DSC_FACTOR)) + watermarkF) / PrecisionFactor) + w0 + 3);

    s = ((NvS32)ratioF * s);

    dpInfo->waterMark = (unsigned)((NvS32)dpInfo->waterMark + (s / (NvS32)PrecisionFactor));

    //
    //  Bounds check the watermark
    //
    NvU32 numSymbolsPerLine = (modesetInfo.surfaceWidth * modesetInfo.depth) / (8 * linkConfig.lanes * DSC_FACTOR);

    if (dpInfo->waterMark > numSymbolsPerLine)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: watermark = %d should not be greater than numSymbolsPerLine = %d.", dpInfo->waterMark, numSymbolsPerLine));
        return false;
    }

    //
    //  Clamp the low side
    //
    if (dpInfo->waterMark < watermarkMinimum)
        dpInfo->waterMark = watermarkMinimum;

    unsigned MinHBlank = 0;
    unsigned MinHBlankFEC = 0;
    NvU32 BlankingBits = 0;
    NvU32 BlankingSymbolsPerLane = 0;

    BlankingBits = (3U * 8U * 4U) + (2U * 8U * numLanesPerLink);

    if (modesetInfo.bEnableDsc)
    {
        NvU32 sliceCount, sliceWidth, chunkSize;

        sliceCount = (modesetInfo.mode == DSC_DUAL) ? 8U : 4U;
        sliceWidth = (NvU32)divide_ceil(modesetInfo.surfaceWidth, sliceCount);
        chunkSize = (NvU32)divide_ceil(modesetInfo.depth * sliceWidth, 8U * DSC_FACTOR);

        if(((NvU64)(chunkSize + 1U) * sliceCount * modesetInfo.pixelClockHz) < (NvU64)(linkConfig.minRate * numLanesPerLink * modesetInfo.surfaceWidth))
        {
            // BW is plenty, this is common case.
            //EOC symbols, when BW enough, only last EOC needs to be considered.
            BlankingBits += 8U * numLanesPerLink; //+BlankingBits_DSC_EOC
            BlankingBits += (chunkSize * 8U) - (sliceWidth * modesetInfo.depth / DSC_FACTOR); //+BlankingBits_DSC_bytePadding, only need to consider last slice
            BlankingBits += (NvU32)(sliceCount * 8U * (divide_ceil(chunkSize, numLanesPerLink) * numLanesPerLink - chunkSize)); //+BlankingBits_DSC_lane_padding
        }
        else
        {   // no extra room in link BW
            //EOC symbols, EOC will be accumulated until hblank period.
            BlankingBits += (sliceCount * 8U * numLanesPerLink); //+BlankingBits_EOC
            //padding, can also use simplified but pessimistic version : BlankingBits += SliceNum * (logic_lanes *8-1);
            BlankingBits += (NvU32)(sliceCount * ((divide_ceil(chunkSize, numLanesPerLink) * numLanesPerLink * 8U) - (NvU32)(sliceWidth * modesetInfo.depth / DSC_FACTOR))); //+BlankingBits_DSC_padding
        }
    }
    else
    {
        NvU32 surfaceWidthPerLink = modesetInfo.surfaceWidth;
        NvU32 surfaceWidthPerLane = (NvU32)divide_ceil(surfaceWidthPerLink, numLanesPerLink);

        // Padding
        BlankingBits += (NvU32)divide_ceil(surfaceWidthPerLane * modesetInfo.depth, 8U) * 8U * numLanesPerLink - (NvU32)(surfaceWidthPerLink * modesetInfo.depth); //+BlankingBits_nonDSC_padding
    }

    BlankingSymbolsPerLane = (NvU32)divide_ceil(BlankingBits , (8U * numLanesPerLink)); //in symbols per lane
    BlankingSymbolsPerLane += (linkConfig.enhancedFraming ? 3U : 0U);

    if (linkConfig.bEnableFEC)
    {
        //
        // In worst case, FEC symbols fall into a narrow Hblank period, 
        // we have to consider this in HBlank checker, see bug 200496977
        // but we don't have to consider this in the calculation of hblank_symbols
        //

        MinHBlankFEC = FEC_PARITY_SYM_SST(numLanesPerLink, BlankingSymbolsPerLane); //in symbols
        BlankingSymbolsPerLane += MinHBlankFEC;
    }

    // BlankingSymbolsPerLane is the MinHBlank in link clock cycles,
    MinHBlank = (unsigned)(divide_ceil(BlankingSymbolsPerLane * modesetInfo.pixelClockHz, 
                                        linkConfig.peakRate)); //in pclk cycles
    MinHBlank += 3U; //add some margin

    NvU32 HBlank = (modesetInfo.rasterWidth - modesetInfo.surfaceWidth);

    if (MinHBlank > HBlank)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Blanking Width is smaller than minimum permissible value."));
        return false;
    }

    // Bug 702290 - Active Width should be greater than 60
    if (modesetInfo.surfaceWidth <= 60)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Minimum Horizontal Active Width <= 60 not supported."));
        return false;
    }

    NvU32 total_hblank_symbols = (NvS32)divide_ceil((HBlank * linkConfig.peakRate), modesetInfo.pixelClockHz);
    NvS32 hblank_symbols = (NvS32)(((NvU64)(HBlank - MinHBlank) * linkConfig.peakRate) / modesetInfo.pixelClockHz);

    if (linkConfig.bEnableFEC)
    {
        hblank_symbols -= (FEC_PARITY_SYM_SST(numLanesPerLink, total_hblank_symbols));
        hblank_symbols += MinHBlankFEC;
    }

    //reduce HBlank Symbols to account for secondary data packet
    hblank_symbols -= 1; //Stuffer latency to send BS
    hblank_symbols -= 3; //SPKT latency to send data to stuffer
    hblank_symbols -= 3; //add some margin

    dpInfo->hBlankSym = (hblank_symbols < 0) ? 0 : hblank_symbols;

    //
    // Audio IMP calculations
    //

    // From dev_disp.ref:

    // The packet generation logic needs to know the length of the hblank period. If there is no room
    // in the current hblank for a new packet, it will be delayed until the next blanking period. This
    // field should be programmed during the second Supervisor interrupt based on the new raster
    // dimensions.

    // ...

    // --------------------------------------
    // The following formulas can be used to calculate the maximum audio sampling rate that can
    // be supported by DisplayPort given the current raster dimensions. DisplayPort has much more
    // bandwidth during blanking periods than HDMI has, so hblank size is less of an issue.

    // ...

    // Size of a packet for 2ch audio = 20 symbols (up to 2 samples)
    // Size of a packet for 8ch audio = 40 symbols
    // Size of an audio packet header plus control symbols = 2*#lanes + 8 symbols (assuming < 32 samples per line)
    // number of packets/hblank for 2ch audio = Floor ((number of free symbols/hblank - (2*#lanes + 8) / 20)
    // number of packets/hblank for 8ch audio = Floor ((number of free symbols/hblank - (2*#lanes + 8) / 40)

    // Maximum audio sample rate possible:
    // number of audio samples/line = SetRasterSize.Width * audio_fs / pclk
    // number of audio packets needed for 2ch audio = Ceiling(SetRasterSize.Width * audio_fs / (pclk*2))
    // number of audio packets needed for 3-8ch audio = SetRasterSize.Width * audio_fs / pclk

    // If number of audio packets needed > number of packets/hblank, then you cannot support that audio frequency

    // Note that the hBlankSym calculated is per lane. So the number of symbols available for audio is
    // (number of lanes * hBlankSym).
    // The calculation of audio packets per Hblank needs to account for the following -
    // 2 symbols for SS and SE; 8 symbols for header; and additional 2 symbols to account for actual values used by HW.
    // --------------------------------------

    if (modesetInfo.twoChannelAudioHz != 0)
    {
        if ((dpInfo->hBlankSym * numLanesPerLink) < ((2 * numLanesPerLink) + 8))
        {
            // There aren't enough symbols/hblank available.
            return false;
        }

        NvU32 twoChannelAudioPacketsPerHBlank = (NvU32)divide_floor(((dpInfo->hBlankSym * numLanesPerLink) - (2 * numLanesPerLink) - 8 - (2 * numLanesPerLink)), 20);

        NvU32 twoChannelAudioPackets = (NvU32)divide_ceil(modesetInfo.twoChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz * 2);

        if (twoChannelAudioPackets > twoChannelAudioPacketsPerHBlank)
        {
            // There aren't enough packets/hblank available.
            return false;
        }
    }

    if (modesetInfo.eightChannelAudioHz != 0)
    {
        if ((dpInfo->hBlankSym * numLanesPerLink) < (2 * numLanesPerLink + 8))
        {
            // There aren't enough symbols/hblank available.
            return false;
        }

        NvU32 eightChannelAudioPacketsPerHBlank = (NvU32)divide_floor(((dpInfo->hBlankSym * numLanesPerLink) - (2 * numLanesPerLink) - 8 - (2 * numLanesPerLink)), 40);

        NvU32 eightChannelAudioPackets = (NvU32)divide_ceil(modesetInfo.eightChannelAudioHz * modesetInfo.rasterWidth, modesetInfo.pixelClockHz);

        if (eightChannelAudioPackets > eightChannelAudioPacketsPerHBlank)
        {
            // There aren't enough packets/hblank available.
            return false;
        }
    }

    // Refer to dev_disp.ref for more information.
    // # symbols/vblank = ((SetRasterBlankEnd.X + SetRasterSize.Width - SetRasterBlankStart.X - 40) * link_clk / pclk) - Y - 1;
    // where Y = (# lanes == 4) 12 : (# lanes == 2) ? 21 : 39
    if (modesetInfo.surfaceWidth < 40)
    {
        vblank_symbols = 0;
    }
    else
    {
        vblank_symbols = (NvS32)(((NvU64)(modesetInfo.surfaceWidth - 3) * linkConfig.peakRate) /  modesetInfo.pixelClockHz);

        //
        // The active region transmission is delayed because of lane fifo storage.
        // compare to the negedge of hblank, all the BE will be delayed by watermark/ratio cycles.
        // compare to the posedge of hblank(i.e. the time of sending out BS symbols in vblank period),
        // all the BS after active pixels will be delayed by maximum 1.5 TU cycles,
        // the delay of the BS will cause the 1st vblank line shorter than expected,
        // but it will squeeze hblank period first,
        // if hblank is short, the BS will be in hactive period and impact vblank_symbols.
        //

        NvS32 squeezed_symbols = (dpInfo->tuSize * 3 / 2) - hblank_symbols;
        squeezed_symbols = DP_MAX(squeezed_symbols, 0);
        NvS32 msa_symbols = (36 / numLanesPerLink) + 3;

        //
        // MSA can't be in the 1st vblank line, except v_front_porch=0
        // if we know v_front_porch != 0,
        // we can use MAX(squeezed_symbols, msa_symbols) instead of squeezed_symbols+msa_symbols
        //
        vblank_symbols -= (squeezed_symbols + msa_symbols);

        if (linkConfig.bEnableFEC)
        {
            vblank_symbols -= FEC_PARITY_SYM_SST(numLanesPerLink, vblank_symbols);
        }
        vblank_symbols -= 3U; //add some margin
    }

    dpInfo->vBlankSym = (vblank_symbols < 0) ? 0 : vblank_symbols;

    if (modesetInfo.bEnableDsc)
    {
        //
        // For DSC enabled case, the vblank_symbols must be large enough to accommodate DSC PPS SDP, see bug 2760673
        // For 1 lane, it requires at least 170+13 symbols
        // For 2 lane, it requires at least 86+3 symbols
        // For 4 lane, it requires at least 44+3 symbols
        // normally, no need to check this, except in some small resolution test case.
        //
        if ((numLanesPerLink == 1U) && (dpInfo->vBlankSym < 183U))
        {
            return false;
        }
        else if ((numLanesPerLink == 2U) && (dpInfo->vBlankSym < 89U))
        {
            return false;
        }
        if ((numLanesPerLink == 4U) && (dpInfo->vBlankSym <47U))
        {
            return false;
        }
    }

    return true;
}

bool DisplayPort::isModePossibleMSTWithFEC
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    Watermark  * dpInfo
)
{
    //
    // This function is for multistream only!
    // Refer to Bug 200406501 and 200401850 for algorithm
    //
    DP_ASSERT(linkConfig.multistream);

    if (!modesetInfo.pixelClockHz || !modesetInfo.depth)
    {
        DP_ASSERT(0 && "INVALID PIXEL CLOCK and DEPTH sent by the client ");
        return false;
    }

    if (linkConfig.lanes == 0)
    {
        DP_ASSERT(0 && "No Active link / link train failed ");
        return false;
    }

    // depth is multiplied by 16 in case of DSC enable
    unsigned DSC_FACTOR = modesetInfo.bEnableDsc ? 16 : 1;
    dpInfo->tuSize = 64;

    NvU32 BlankingBits, BlankingSymbolsPerLane;
    NvU32 numLanesPerLink = 4U;
    NvU32 MinHBlank;

    BlankingBits =  (3U * 8U * 4U) + (2U * 8U * numLanesPerLink);

    if(modesetInfo.bEnableDsc)
    {
        NvU32 sliceCount, sliceWidth, chunkSize;

        sliceCount = (modesetInfo.mode == DSC_DUAL) ? 8U : 4U;
        sliceWidth = (NvU32)divide_ceil(modesetInfo.surfaceWidth, sliceCount);
        chunkSize = (NvU32)divide_ceil(modesetInfo.depth * sliceWidth, 8U * DSC_FACTOR);

        //EOC symbols, EOC will be accumulated until hblank period.
        BlankingBits += (sliceCount * 8U * numLanesPerLink); //+BlankingBits_EOC
        //+BlankingBits_DSC_padding
        BlankingBits += (NvU32)(sliceCount * ((divide_ceil(chunkSize, numLanesPerLink) * numLanesPerLink * 8U) - (NvU32)(sliceWidth * modesetInfo.depth / DSC_FACTOR)));
    }
    else
    {
        NvU32 surfaceWidthPerLane = (NvU32)divide_ceil(modesetInfo.surfaceWidth, numLanesPerLink);

        //Extra bits sent due to pixel steering
        BlankingBits = (NvU32)divide_ceil(surfaceWidthPerLane * modesetInfo.depth, 8U) * 8U * numLanesPerLink - (NvU32)(modesetInfo.surfaceWidth * modesetInfo.depth); //+BlankingBits_nonDSC_padding
    }

    BlankingSymbolsPerLane = (NvU32)divide_ceil(BlankingBits, (8U * numLanesPerLink)); //in symbols per lane

    MinHBlank = (NvU32)divide_ceil(BlankingSymbolsPerLane * 8U * numLanesPerLink * DSC_FACTOR, modesetInfo.depth);
    MinHBlank += 3U; //add some margin

    NvU32 HBlank = (modesetInfo.rasterWidth - modesetInfo.surfaceWidth);

    if (MinHBlank > HBlank)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Blanking Width is smaller than minimum permissible value."));
        return false;
    }

    // Bug 702290 - Active Width should be greater than 60
    if (modesetInfo.surfaceWidth <= 60)
    {
        DP_LOG(("NVRM: %s:", __FUNCTION__));
        DP_LOG(("\t\tERROR: Minimum Horizontal Active Width <= 60 not supported."));
        return false;
    }

    // MST can do SDP splitting so all audio configuration are possible. 
    dpInfo->hBlankSym = 0U;
    dpInfo->vBlankSym = 0U;

    return true;
}

unsigned DisplayPort::pbnForMode(const ModesetInfo & modesetInfo)
{
    //
    // Calculate PBN in terms of 54/64 mbyte/sec
    // round up by .6% for spread de-rate. Note: if we're not spreading our link
    // this MUST still be counted.  It's also to allow downstream links to be spread.
    //
    unsigned pbnForMode = (NvU32)(divide_ceil(modesetInfo.pixelClockHz * modesetInfo.depth * 1006 * 64 / 8,
                                    (NvU64)54000000 *1000));

    if(modesetInfo.bEnableDsc)
    {
        //
        // When DSC is enabled consider depth will multiplied by 16 and also 3% FEC Overhead
        // as per DP1.4 spec
        pbnForMode = (NvU32)(divide_ceil(pbnForMode * 100, 97 * DSC_DEPTH_FACTOR));
    }

    return pbnForMode;
}
