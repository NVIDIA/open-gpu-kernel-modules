/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "disp/v05_01/dev_disp.h"
#include "displayport/displayport.h"
#include "displayport/displayport2x.h"


// Local help functions and strctures/macros for NVD5 DP IMP.

// All bpp / effective bpp are multiplied by this scaler to avoid floating operation.
#define BPPX256_SCALER       256U
#define BPPX16_SCALER        16U
#define BPPX32_SCALER        32U
#define LOGICAL_LANES         4U

//
// DP1: 8b/10b, we have 1 byte per logic lane.
// DP2: 128b/132b, we have only 4 bytes per lane.
//
#define DP1_BYTE_PER_LOGIC_LANE    1U
#define DP2_BYTES_PER_LOGIC_LANE   4U

//
// DP1: 8b/10b,    1 symbol is 8 bits.
// DP2: 128b/132b, 1 symbol is 32 bits.
//
#define DP1_SYMBOL_SIZE       8U
#define DP2_SYMBOL_SIZE      32U

// DP1: 8b/10b, symbol size after channel coding.
#define DP1_CODING_SIZE      10U

#define GET_SYMBOL_SIZE(bIsDp2xChannelCoding) ((bIsDp2xChannelCoding) ? DP2_SYMBOL_SIZE : DP1_SYMBOL_SIZE)
#define BYTES_PER_LOGIC_LANE(bIsDp2xChannelCoding) ((bIsDp2xChannelCoding) ? DP2_BYTES_PER_LOGIC_LANE : DP1_BYTE_PER_LOGIC_LANE)

#define FEC_PARITY_SYM(lanes, x) ((lanes) == 1 ? (NV_MIN(((x) - 1) % 512, 12) + ((x) - 1) / 512 * 12 + 1) : \
                                                 (NV_MIN(((x) - 1) % 256,  6) + ((x) - 1) / 256 *  6 + 1))
#define HBLANK_SYMBOLS_SCALE 64U

typedef enum
{
    RG_PACKET_MODE_SINGLE = 1,
    RG_PACKET_MODE_DOUBLE = 2,
    RG_PACKET_MODE_QUAD   = 4
} RG_PACKET_MODE;

#define DEFAULT_RG_PACKET_MODE          4U
#define DEFAULT_PCLK_FACTOR_VALUE       8U

typedef enum
{
    TWO_CHANNELS   = 2,
    EIGHT_CHANNELS = 8
} DP_AUDIO_CHANNELS;

static NvU32 _calcEffectiveBppxScalerNonDsc
(
    NvU32   hActive,
    NvU32   sourceBpp,
    NvBool  bIsDp2xChannelCoding,
    NvBool  bMultiStream,
    NvU32   laneCount
)
{
    NvU32 totalSymbols          = 0U;
    NvU32 effectiveBppxScaler   = 0U;
    NvU32 totalSymbolsPerLane   = 0U;
    NvU32 logicalLanes          = LOGICAL_LANES;

    NvU32 symbolSize   = GET_SYMBOL_SIZE(bIsDp2xChannelCoding);

     //
    // Logic lane count values for :
    // 1. 128b/132b : 4
    // 2. 8b/10 :
    //     a. MST : 4
    //     B. SST : 1, 2 or 4 based on lane Count
    //
    if (!bMultiStream && !bIsDp2xChannelCoding)
    {
        logicalLanes = laneCount;
    }

    NvU32 bitsPerLane   = (NvU32)NV_CEIL(hActive, logicalLanes) * sourceBpp;
    totalSymbolsPerLane = (NvU32)NV_CEIL(bitsPerLane, symbolSize);
    totalSymbols        = totalSymbolsPerLane * logicalLanes;
    effectiveBppxScaler = (NvU32)NV_CEIL((totalSymbols * symbolSize * BPPX256_SCALER), hActive);

    return effectiveBppxScaler;
}

static NvU32 _calcDpMinHBlankMST
(
    NvU32 bppXScaler,
    NvBool bIsDp2xChannelCoding
)
{
    NvU32 symbolCounts = bIsDp2xChannelCoding ? 3U : 5U;
    NvU32 symbolSize   = GET_SYMBOL_SIZE(bIsDp2xChannelCoding);
    return NV_CEIL(symbolCounts * symbolSize * LOGICAL_LANES * BPPX256_SCALER, bppXScaler);
}

static NvU32 _getSdpSymbolsMST
(
    NvU32   sdpPayloadSize,
    NvBool  bIsDp2xChannelCoding
)
{
    if (sdpPayloadSize <= 0)
        return 0;
    if (bIsDp2xChannelCoding)
        return (4 + ((sdpPayloadSize / 16) * 5));
    else
        return (2 + (NV_CEIL((sdpPayloadSize / 16 * 5) + 2, 4)));
}

static NvU32 _getDpAudioSymbolMST
(
    DP_AUDIO_CHANNELS   channels,
    NvU32               samples_x1000,
    NvBool              bIsDp2xChannelCoding
)
{
    //
    // Two channel : samples per packets = 2.
    // Eight Channel : samples per packets = 1
    //
    NvU32 packets_x1000     = samples_x1000 / ((channels == 2) ? 2 : 1);
    NvU32 packetPerLineOff  = packets_x1000 % 1000;
    NvU32 packetPerLineBase = packets_x1000 / 1000;

    NvU32 sdpPayloadSize    = (channels == 2) ? 16U : 32U;
    NvU32 symbolPerLineLo   = _getSdpSymbolsMST(sdpPayloadSize, bIsDp2xChannelCoding) * packetPerLineBase;
    NvU32 symbolPerLineHi   = _getSdpSymbolsMST(sdpPayloadSize, bIsDp2xChannelCoding) * (packetPerLineBase + 1);

    NvU32 symbolPerLine_x1000   = symbolPerLineLo * (1000 - packetPerLineOff) +
                                  symbolPerLineHi * packetPerLineOff;

    return NV_CEIL(symbolPerLine_x1000, 1000);
}

// Returns Audio_Symbols_per_Line_per_Lane.
static NvU32 _getDpAudioSymbolSST
(
    DP_AUDIO_CHANNELS   channels,
    NvU32               samplesPerLine_x1000,
    NvU32               laneCount,
    NvU32               surfaceWidth
)
{
    NvU32 cyclesPerPacket       = 0U;
    NvU32 cyclesPerPacketInc    = 0U;
    NvU32 cyclesOverHead        = (laneCount == 1) ? 12 : 2;

    NvU32 packetsPerLine        = 0U;

    switch (channels)
    {
        case TWO_CHANNELS:
        {
            switch (laneCount)
            {
                case 1:
                    cyclesPerPacket     = 30;
                    cyclesPerPacketInc  = 20;
                    break;
                case 2:
                    cyclesPerPacket     = 16;
                    cyclesPerPacketInc  = 10;
                    break;
                case 4:
                    cyclesPerPacket     = 9;
                    cyclesPerPacketInc  = 5;
                    break;
            }
        }
        case EIGHT_CHANNELS:
        {
            switch (laneCount)
            {
                case 1:
                    cyclesPerPacket     = 50;
                    cyclesPerPacketInc  = 40;
                    break;
                case 2:
                    cyclesPerPacket     = 26;
                    cyclesPerPacketInc  = 20;
                    break;
                case 4:
                    cyclesPerPacket     = 14;
                    cyclesPerPacketInc  = 10;
                    break;
            }
        }
    }

    //
    // Two channel : samples per packets = 2.
    // Eight Channel : samples per packets = 1
    //
    packetsPerLine       = NV_CEIL(samplesPerLine_x1000,
                                   1000 * ((channels == TWO_CHANNELS) ? 2 : 1));

    if (packetsPerLine == 0)
        return 0;

    return (cyclesPerPacket + (packetsPerLine - 1) * cyclesPerPacketInc + cyclesOverHead);
}

static NV_STATUS _calcPClkFactorAndRgPacketMode
(
    OBJGPU         *pGpu,
    DPMODESETDATA  *pDpModesetData,
    NvU32           headIndex,
    NvU32          *PClkFactor,
    NvU32          *rgPacketMode
)
{

    NvU64 rawPixelFreq      = pDpModesetData->PClkFreqHz;
    NvU32 hTotal            = pDpModesetData->SetRasterSizeWidth;

    NvU32 pClkCap           = GPU_REG_RD32(pGpu, NV_PDISP_FE_SW_HEAD_CLK_CAP(headIndex));
    NvU32 pClkMax           = DRF_VAL(_PDISP, _FE_SW_HEAD_CLK_CAP, _PCLK_MAX, pClkCap);
    NvU32 pClkMin           = DRF_VAL(_PDISP, _FE_SW_HEAD_CLK_CAP, _PCLK_MIN, pClkCap);

    NV_ASSERT_OR_RETURN((PClkFactor != NULL && rgPacketMode != NULL), NV_ERR_INVALID_ARGUMENT);

    // Init the value to 1;
    *PClkFactor = 1;
    *rgPacketMode = RG_PACKET_MODE_SINGLE;
    if (pDpModesetData->colorFormat == dpColorFormat_YCbCr420)
        *PClkFactor = 2;

    if ((((hTotal / *PClkFactor) % 4)  == 0) &&
        ((rawPixelFreq / (*PClkFactor * 4)) > (pClkMin * 1000000)))
    {
        *PClkFactor <<= 2;
        *rgPacketMode = RG_PACKET_MODE_QUAD;
    }
    else if ((((hTotal / *PClkFactor) % 2)  == 0) &&
             ((rawPixelFreq / (*PClkFactor * 2)) > (pClkMin * 1000000)))
    {
        *PClkFactor <<= 1;
        *rgPacketMode = RG_PACKET_MODE_DOUBLE;
    }

    if (((rawPixelFreq / *PClkFactor) > (pClkMax * 10000000)) &&
        (pDpModesetData->bDscEnable && pDpModesetData->bpp <= 16))
    {
        *PClkFactor <<= 1;
    }

    return NV_OK;
}

static NvU32 _calcWatermark8b10bSST
(
    NvU32   pClkHz,
    NvU64   linkRateHz,
    NvU32   laneCount,
    NvBool  bFecEnabled,
    NvU32   scaledBpp,
    NvBool  bIsDscEnabled
)
{
    NvU32 linkRateMHz   = linkRateHz / 1000000;
    NvU32 pClkKhz       = pClkHz / 1000;

    NvS32 ratio_x1000   = 1000;
    NvU32 w0            = (8 / laneCount);
    NvS32 s             = 0U;
    NvS32 watermark     = 0U;
    NvU32 bppScaler     = 1U;

    if (bFecEnabled)
    {
        s = (laneCount == 1) ? 18 : 13;
    }
    else
    {
        s = 3 - w0;
    }

    if (bIsDscEnabled)
    {
        bppScaler = BPPX16_SCALER;
    }

    ratio_x1000 = ((NvS64) pClkKhz * scaledBpp * (bFecEnabled ? 1024 : 1000)) /
                   (linkRateMHz * laneCount * (bppScaler * 1000 * DP1_SYMBOL_SIZE / DP1_CODING_SIZE));

    // 15625 = 1000 * 1000 / 64
    watermark =  NV_CEIL(3 * 15625 * scaledBpp, 8 * bppScaler * laneCount);
    watermark += (ratio_x1000 * (1000 - ratio_x1000));
    watermark =  watermark * 1000 + ratio_x1000 * s * 15625;
    watermark =  NV_CEIL(watermark, 15625 * 1000);
    watermark += (w0 + 3);

    return (NvU32) watermark;
}

static NvU64 _convertLinkRateToDataRate
(
    NvU32   linkRate10M,
    NvBool  bIs128b132bChannelCoding,
    NvBool  bEnableFEC
)
{
    NvU64 dataRateHz    = 0U;
    NvU64 minRate       = DP_LINK_RATE_10M_TO_BPS(((NvU64)linkRate10M));

    if (bIs128b132bChannelCoding)
    {
        // 128b/132b Total Data BW efficiency considers FEC overhead
        dataRateHz  = DP_LINK_RATE_BITSPS_TO_BYTESPS(DATA_BW_EFF_128B_132B(minRate));
    }
    else
    {
        if (bEnableFEC)
        {
            minRate     = minRate - 3 * minRate / 100;
        }
        else
        {
            minRate     = minRate - 6 * minRate / 1000;
        }
        dataRateHz = DP_LINK_RATE_BITSPS_TO_BYTESPS(OVERHEAD_8B_10B(minRate));
    }
    return dataRateHz;
}

/*!
 * @brief Get the LOADV counter
 *
 * @param[in]  pGpu                    OBJGPU pointer
 * @param[in]  pKernelHead             KernelHead object pointer
 *
 * @return the current LOADV counter
 */
NvU32
kheadGetLoadVCounter_v05_01
(
    OBJGPU                 *pGpu,
    KernelHead             *pKernelHead
)
{
    return GPU_REG_RD32(pGpu, NV_PDISP_RG_IN_LOADV_COUNTER(pKernelHead->PublicId));
}

NvU32
kdispGetPBTargetAperture_v05_01
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         memAddrSpace,
    NvU32         cacheSnoop
)
{
    NvU32 pbTargetAperture = PHYS_NVM;

    /* On T264 Tegra SoC, only valid options are IOVA if NISO SMMU is enabled,
     * or, PHYS_NVM if NISO SMMU is disabled/bypassed.
     */
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS))
    {
        pbTargetAperture = IOVA;
    }
    else
    {
        pbTargetAperture = PHYS_NVM;
    }

    return pbTargetAperture;
}

/*!
 * @brief Reads the non-notification kernel interrupt vector # from the interrupt control register
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pDisp         OBJDISP pointer
 *
 */
void kdispIntrRetrigger_v05_01
(
    OBJGPU            *pGpu,
    KernelDisplay     *pKernelDisplay,
    NvU32              index,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 val = 0;
    val = FLD_SET_DRF(_PDISP_FE, _INTR_RETRIGGER, _TRIGGER, _TURE, val);
    GPU_REG_WR32_EX(pGpu, NV_PDISP_FE_INTR_RETRIGGER(index), val, pThreadState);
}


NV_STATUS
kdispComputeDpModeSettings_v05_01
(
    OBJGPU             *pGpu,
    KernelDisplay      *pKernelDisplay,
    NvU32               headIndex,
    DPMODESETDATA      *pDpModesetData,
    DPIMPINFO          *dpInfo
)
{
    NvU32 hActive               = pDpModesetData->SetRasterBlankStartX -
                                  pDpModesetData->SetRasterBlankEndX;

    NvU32 hBlank                = pDpModesetData->SetRasterSizeWidth - hActive;

    NvU32 effectiveBppxScaler   = 0U;
    NvU32 symbolSize            = GET_SYMBOL_SIZE(pDpModesetData->bDP2xChannelCoding);
    NvU64 linkFreqHz            = pDpModesetData->dp2LinkBw ?
                                  DP_LINK_RATE_10M_TO_BPS((NvU64)pDpModesetData->dp2LinkBw) :
                                  DP_LINK_RATE_270M_TO_BPS((NvU64)pDpModesetData->linkBw);

    NvU32 sliceCount            = 0U;
    NvU32 chunkSize             = 0U;
    NvU32 sliceWidth            = 0U;

    NvU32 rgPacketMode          = 0U;
    NvU32 pclkFactor            = 0U;

    // depth is multiplied by 16 in case of DSC enable
    NvU32  dscFactor            = pDpModesetData->bDscEnable ? 16U : 1U;
    NvU64  linkDataRate         = 0U;
    NvS32  hBlankSym            = 0U;
    NvS32  vBlankSym            = 0U;
    NvU32  msaSym               = 0U;

    dpInfo->bEnhancedFraming = pDpModesetData->bEnhancedFraming;

    if (!linkFreqHz || !pDpModesetData->laneCount|| !pDpModesetData->PClkFreqHz)
    {
        NV_ASSERT(linkFreqHz && pDpModesetData->laneCount && pDpModesetData->PClkFreqHz);
    }

    // Check if we have a valid laneCount as currently we support only upto 4-lanes
    if (!IS_VALID_LANECOUNT(pDpModesetData->laneCount))
    {
        DBG_BREAKPOINT();
    }

    dpInfo->tuSize = 64;

    // Init dpInfo here
    dpInfo->vBlankSym = 0U;
    dpInfo->twoChannelAudioSymbols = 0U;
    dpInfo->eightChannelAudioSymbols = 0U;

    if (pDpModesetData->bDscEnable)
    {
        // Default to 4 if client does not speicify the slice count.
        sliceCount = (pDpModesetData->sliceCount ? pDpModesetData->sliceCount : 4U);
        sliceWidth = (NvU32)NV_CEIL(pDpModesetData->SetRasterSizeWidth, sliceCount);
        chunkSize  = (NvU32)NV_CEIL(pDpModesetData->bpp * sliceWidth, 8U * dscFactor);
    }
    linkDataRate = _convertLinkRateToDataRate(pDpModesetData->dp2LinkBw,
                                             pDpModesetData->bDP2xChannelCoding,
                                             pDpModesetData->bFecEnable);

    dpInfo->linkTotalDataRate = 8 * linkDataRate * pDpModesetData->laneCount;

    // SST
    if (!(pDpModesetData->bMultiStream))
    {
        if (pDpModesetData->bDscEnable)
        {
            effectiveBppxScaler = pDpModesetData->bpp * BPPX16_SCALER;
        }
        else
        {
            if (pDpModesetData->bDisableEffBppSST8b10b && !pDpModesetData->bDP2xChannelCoding)
            {
                effectiveBppxScaler = pDpModesetData->bpp * BPPX256_SCALER;
            }
            else
            {
                effectiveBppxScaler = _calcEffectiveBppxScalerNonDsc(hActive, pDpModesetData->bpp, pDpModesetData->bDP2xChannelCoding,
                                                                     pDpModesetData->bMultiStream, pDpModesetData->laneCount);        
            }
        }
    }
    else
    {
        effectiveBppxScaler = pDpModesetData->bpp;
    }

    dpInfo->effectiveBppxScaler = effectiveBppxScaler;

    if (pDpModesetData->bDP2xChannelCoding || pDpModesetData->bMultiStream)
    {

        dpInfo->minHBlank   = _calcDpMinHBlankMST(effectiveBppxScaler, pDpModesetData->bDP2xChannelCoding);

        if (pDpModesetData->bMultiStream)
        {
            dpInfo->hBlankSym = (hBlank * pDpModesetData->bpp) / (LOGICAL_LANES * symbolSize * BPPX256_SCALER);
        }
        else
        {
            dpInfo->hBlankSym = (hBlank * pDpModesetData->bpp) / (LOGICAL_LANES * symbolSize * dscFactor);
        }

        if (pDpModesetData->bDP2xChannelCoding)
        {
            // exclude BS/BE
            dpInfo->hBlankSym -= 2U;
        }
        else
        {
            // Better to leave some margin for hblank
            dpInfo->hBlankSym -= 6U;
        }

        if (pDpModesetData->twoChannelAudioHz != 0)
        {
            NvU32 samples_x1000 = (NvU32)NV_CEIL((NvU64)pDpModesetData->twoChannelAudioHz *
                                                        pDpModesetData->SetRasterSizeWidth,
                                                  pDpModesetData->PClkFreqHz * 1000);

            dpInfo->twoChannelAudioSymbols = _getDpAudioSymbolMST(TWO_CHANNELS, samples_x1000,
                                                                  pDpModesetData->bDP2xChannelCoding);
        }

        // 3-8 channel case
        if (pDpModesetData->eightChannelAudioHz != 0)
        {
            NvU32 samples_x1000 = (NvU32)NV_CEIL((NvU64)pDpModesetData->eightChannelAudioHz *
                                                        pDpModesetData->SetRasterSizeWidth,
                                                  pDpModesetData->PClkFreqHz * 1000);

            dpInfo->eightChannelAudioSymbols = _getDpAudioSymbolMST(EIGHT_CHANNELS, samples_x1000,
                                                                    pDpModesetData->bDP2xChannelCoding);
        }
    }
    else
    {
        // 8b/10b SST.

        // (VBID+MVID+MAUD)
        const NvU32 symbolCount             = 3U;
        // Per spec, each symbol will be repeated 4 times.
        const NvU32 repeatCount             = 4U;
        // enhanced_framing and 3cycle dummy
        const NvU32 enhancedFramingSymbols  = 8U;
        NvU32 blankingBits = symbolCount * repeatCount * DP1_SYMBOL_SIZE +
                             enhancedFramingSymbols * DP1_SYMBOL_SIZE * pDpModesetData->laneCount;

        if (pDpModesetData->bDscEnable)
        {
            blankingBits += DP1_SYMBOL_SIZE * pDpModesetData->laneCount;
            blankingBits += sliceCount * (chunkSize * 8  - (sliceWidth * pDpModesetData->bpp / dscFactor));
            blankingBits +=
                NV_CEIL(chunkSize, pDpModesetData->laneCount) * pDpModesetData->laneCount * 8 - chunkSize * 8;
        }
        else
        {
            NvU32 HActivePerLane = NV_CEIL(hActive, pDpModesetData->laneCount);
            //padding
            blankingBits += NV_CEIL(HActivePerLane * pDpModesetData->bpp * BPPX256_SCALER, DP1_SYMBOL_SIZE * BPPX256_SCALER) * DP1_SYMBOL_SIZE * pDpModesetData->laneCount -
                            (hActive * pDpModesetData->bpp); //+blankingBits_nonDSC_padding
        }

        // 1006 / 1000 for downspread.
        dpInfo->minHBlank = (NvU32)NV_CEIL((NvU64)blankingBits * pDpModesetData->PClkFreqHz * DP1_CODING_SIZE * 1006,
                                           (NvU64)DP1_SYMBOL_SIZE * pDpModesetData->laneCount * linkFreqHz * 1000);
        // 1-2 channel case
        if (pDpModesetData->twoChannelAudioHz != 0)
        {
            NvU32 samplesPerLine_x1000     = (NvU32)NV_CEIL((NvU64)pDpModesetData->twoChannelAudioHz *
                                                             pDpModesetData->SetRasterSizeWidth,
                                                             pDpModesetData->PClkFreqHz);

            dpInfo->twoChannelAudioSymbols =
                _getDpAudioSymbolSST(TWO_CHANNELS, samplesPerLine_x1000, pDpModesetData->laneCount, hActive);
        }

        // 3-8 channel case
        if (pDpModesetData->eightChannelAudioHz != 0)
        {
            NvU32 samplesPerLine_x1000     = (NvU32)NV_CEIL((NvU64)pDpModesetData->eightChannelAudioHz *
                                                                   pDpModesetData->SetRasterSizeWidth,
                                                             pDpModesetData->PClkFreqHz);

            dpInfo->eightChannelAudioSymbols =
                _getDpAudioSymbolSST(EIGHT_CHANNELS, samplesPerLine_x1000, pDpModesetData->laneCount, hActive);
        }

        dpInfo->hBlankSym = 0;

        if (_calcPClkFactorAndRgPacketMode(pGpu, pDpModesetData, headIndex,
                                           &pclkFactor, &rgPacketMode) != NV_OK)
        {
            pclkFactor   = DEFAULT_PCLK_FACTOR_VALUE;
            rgPacketMode = DEFAULT_RG_PACKET_MODE;
        }

        hBlankSym = (hBlank - dpInfo->minHBlank - 3 * (1 << pclkFactor)) * HBLANK_SYMBOLS_SCALE;
        if (pDpModesetData->bDscEnable)
        {
            hBlankSym -= ((1 << pclkFactor) * 32 * dscFactor * HBLANK_SYMBOLS_SCALE / pDpModesetData->bpp) - 1;
        }
        hBlankSym = hBlankSym * linkFreqHz * 994 / (DP1_CODING_SIZE * HBLANK_SYMBOLS_SCALE * pDpModesetData->PClkFreqHz * 1000);
        if (pDpModesetData->bFecEnable)
        {
            NvU32 totalHBlankSymbols =
                (NvU32)NV_CEIL((NvU64)(hBlank * linkFreqHz * 994),
                               ((NvU64)DP1_CODING_SIZE * pDpModesetData->PClkFreqHz * 1000));
            hBlankSym -= FEC_PARITY_SYM(pDpModesetData->laneCount, totalHBlankSymbols);
            hBlankSym -= 1;
            hBlankSym -= 3;
            hBlankSym -= 3;

        }

        // Bug 5042450 clamp min_hBlankSym value
        NvS32 hBlankSymMin = pDpModesetData->laneCount==4 ? 12 : ( pDpModesetData->laneCount==2 ? 19 : 43 );
        if (hBlankSym < hBlankSymMin)
        {
            hBlankSym = hBlankSymMin;
        }

        if (hBlankSym > 0)
        {
            dpInfo->hBlankSym = hBlankSym;
        }

        // Bug - 4300218
        // Programmed the vBlankSym based on the alogirthm mentioned in the bug 4300218
        vBlankSym = (NvS32)(((hActive - 3*pclkFactor) * linkFreqHz * 994) / (pDpModesetData->PClkFreqHz*1000));
        msaSym    = (36 / pDpModesetData->laneCount) + 3;
        vBlankSym -= msaSym;

        if (pDpModesetData->bFecEnable)
        {
            vBlankSym -= FEC_PARITY_SYM(LOGICAL_LANES, vBlankSym);
        }

        //added some margin
        vBlankSym -= 3;

        if(vBlankSym > 0)
        {
            dpInfo->vBlankSym = vBlankSym;
        }
        
        dpInfo->waterMark = _calcWatermark8b10bSST(pDpModesetData->PClkFreqHz,
                                                   linkFreqHz,
                                                   pDpModesetData->laneCount,
                                                   pDpModesetData->bFecEnable,
                                                   pDpModesetData->bpp,
                                                   pDpModesetData->bDscEnable);
    }

    return NV_OK;
}


/*!
 * @brief Get the crash lock counter vertical
 *
 * @param[in]  pGpu                    OBJGPU pointer
 * @param[in]  pKernelHead             KernelHead object pointer
 *
 * @return the current crash lock counter
 */
NvU32
kheadGetCrashLockCounterV_v05_01
(
    OBJGPU                 *pGpu,
    KernelHead             *pKernelHead
)
{
    NvU32 counter =  GPU_REG_RD32(pGpu, NV_PDISP_RG_CRASHLOCK_COUNTER(pKernelHead->PublicId));

    return DRF_VAL(_PDISP, _RG_CRASHLOCK_COUNTER, _V, counter);
}

