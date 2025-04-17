/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_watermark.h                                                    *
*    DP watermark IsModePossible calculations.                              *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_WATERMARK_H
#define INCLUDED_DP_WATERMARK_H

#include "displayport.h"

namespace DisplayPort
{
    class LinkConfiguration;

    struct ModesetInfo
    {
        unsigned twoChannelAudioHz;         // if you need 192khz stereo specify 192000 here
        unsigned eightChannelAudioHz;       // Same setting for multi channel audio.
                                            //  DisplayPort encodes 3-8 channel streams as 8 channel
        NvU64 pixelClockHz;                 // Requested pixel clock for the mode
        unsigned rasterWidth;
        unsigned rasterHeight;
        unsigned surfaceWidth;              // RasterBlankStartX - RasterBlankEndX
        unsigned surfaceHeight;             // Active region height
        unsigned depth;
        unsigned rasterBlankStartX;
        unsigned rasterBlankEndX;
        unsigned bitsPerComponent;          // Bits per component
        bool     bEnableDsc;                // bEnableDsc=1 indicates DSC would be enabled for the mode
        bool     bEnablePassThroughForPCON;

        DSC_MODE            mode;           // DSC Mode
        DP_COLORFORMAT      colorFormat;

        ModesetInfo(): twoChannelAudioHz(0),
                       eightChannelAudioHz(0),
                       pixelClockHz(0),
                       rasterWidth(0),
                       rasterHeight(0),
                       surfaceWidth(0),
                       surfaceHeight(0),
                       depth(0),
                       rasterBlankStartX(0),
                       rasterBlankEndX(0),
                       bitsPerComponent(0),
                       bEnableDsc(false),
                       bEnablePassThroughForPCON(false),
                       mode(DSC_SINGLE),
                       colorFormat(dpColorFormat_Unknown)  {}

        ModesetInfo(unsigned newTwoChannelAudioHz, unsigned newEightChannelAudioHz, NvU64 newPixelClockHz,
                    unsigned newRasterWidth, unsigned newRasterHeight,
                    unsigned newSurfaceWidth, unsigned newSurfaceHeight, unsigned newDepth,
                    unsigned newRasterBlankStartX=0, unsigned newRasterBlankEndX=0, bool newBEnableDsc = false,
                    DSC_MODE newMode = DSC_SINGLE, bool newBEnablePassThroughForPCON = false,
                    DP_COLORFORMAT dpColorFormat = dpColorFormat_Unknown):
           twoChannelAudioHz(newTwoChannelAudioHz),
           eightChannelAudioHz(newEightChannelAudioHz),
           pixelClockHz(newPixelClockHz),
           rasterWidth(newRasterWidth),
           rasterHeight(newRasterHeight),
           surfaceWidth(newSurfaceWidth),
           surfaceHeight(newSurfaceHeight),
           depth(newDepth),
           rasterBlankStartX(newRasterBlankStartX),
           rasterBlankEndX(newRasterBlankEndX),
           bitsPerComponent(0),
           bEnableDsc(newBEnableDsc),
           bEnablePassThroughForPCON(newBEnablePassThroughForPCON),
           mode(newMode),
           colorFormat(dpColorFormat) {}
    };

    struct Watermark
    {
        unsigned waterMark;
        unsigned tuSize;
        unsigned hBlankSym;
        unsigned vBlankSym;
        NvU32    effectiveBpp;
    };

    bool isModePossibleSST
    (
        const LinkConfiguration &linkConfig,
        const ModesetInfo &modesetInfo,
        Watermark  * dpInfo,
        bool bUseIncreasedWatermarkLimits = false
    );

    bool isModePossibleMST
    (
        const LinkConfiguration &linkConfig,
        const ModesetInfo &modesetInfo,
        Watermark  * dpInfo
    );

    bool isModePossibleSSTWithFEC
    (
        const LinkConfiguration &linkConfig,
        const ModesetInfo &modesetInfo,
        Watermark  * dpInfo,
        bool bUseIncreasedWatermarkLimits = false
    );

    bool isModePossibleMSTWithFEC
    (
        const LinkConfiguration &linkConfig,
        const ModesetInfo &modesetInfo,
        Watermark  * dpInfo
    );

    // Return Payload Bandwidth Number(PBN)for requested mode
    unsigned pbnForMode(const ModesetInfo &modesetInfo, bool bAccountSpread = true);
}

#endif //INCLUDED_DP_WATERMARK_H
