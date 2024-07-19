//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//  SPDX-License-Identifier: MIT
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//  File:       nvt_util.c
//
//  Purpose:    provide the utility functions for timing library
//
//*****************************************************************************

#include "nvBinSegment.h"

#include "nvtiming_pvt.h"
#include "nvmisc.h" // NV_MAX

PUSH_SEGMENTS

CONS_SEGMENT(PAGE_CONS)

// The following table was generated w/ this program:
/*
#include <stdio.h>

#define CRC32_POLYNOMIAL 0xEDB88320

void main() 
{   
    unsigned int crc = 0, i = 0, j = 0;
    unsigned int CRCTable[256];
    
    for (i = 0; i < 256 ; i++) 
    {
        crc = i;
        for (j = 8; j > 0; j--) 
        {
            if (crc & 1)
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            else
                crc >>= 1;
        }
        CRCTable[i] = crc;
    }

    printf("static const NvU32 s_CRCTable[256] = {");
    for (i = 0; i < 256; i++)
    {
        printf("%s0x%08X%s",
               ((i % 10 == 0) ? "\n    " : ""),
               CRCTable[i],
               ((i != 255) ? ", " : " "));
    }
    printf("};\n");
}
*/
static const NvU32 s_CRCTable[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4,
    0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
    0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A,
    0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F,
    0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
    0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5,
    0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6,
    0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
    0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C,
    0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31,
    0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
    0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7,
    0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8,
    0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D };

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 a_div_b(NvU32 a, NvU32 b)
{
    if (b == 0)
        return 0xFFFFFFFF;

    return (a + b/2)/b;
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 axb_div_c(NvU32 a, NvU32 b, NvU32 c)
{
    NvU32 AhxBl, AlxBh;
    NvU32 AxB_high, AxB_low;
    NvU32 AxB_div_C_low;

    if (c==0)
        return 0xFFFFFFFF;

    // calculate a*b
    AhxBl  = (a>>16)*(b&0xFFFF);
    AlxBh  = (a&0xFFFF)*(b>>16);

    AxB_high =  (a>>16) * (b>>16);
    AxB_low  =  (a&0xFFFF) * (b&0xFFFF);

    AxB_high += AlxBh >> 16;
    AxB_high += AhxBl >> 16;
    
    if ((AxB_low + (AlxBh<<16))< AxB_low)
        AxB_high ++;
    AxB_low  += AlxBh << 16;

    if ((AxB_low + (AhxBl<<16)) < AxB_low)
        AxB_high ++;
    AxB_low  += AhxBl << 16;

    AxB_div_C_low = AxB_low/c;
    AxB_div_C_low += 0xFFFFFFFF / c * (AxB_high % c);
    AxB_div_C_low += ((0xFFFFFFFF % c + 1) * (AxB_high % c) + (AxB_low % c) + c/2) / c;


    return AxB_div_C_low;
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU64 axb_div_c_64(NvU64 a, NvU64 b, NvU64 c)
{
    // NvU64 arithmetic to keep precision and avoid floats
    // a*b/c = (a/c)*b + ((a%c)*b + c/2)/c
    return ((a/c)*b + ((a%c)*b + c/2)/c);
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 calculateCRC32(NvU8* pBuf, NvU32 bufsize)
{
    NvU32 crc32 = 0xFFFFFFFF, temp1, temp2, count = bufsize;

    if (bufsize == 0 || pBuf == NULL)
    {
        return 0;
    }

    while (count-- != 0)
    {
        temp1 = (crc32 >> 8) & 0x00FFFFFF;
        temp2 = s_CRCTable[(crc32 ^ *pBuf++) & 0xFF];
        crc32 = temp1 ^ temp2;
    }
    crc32 ^= 0xFFFFFFFF;

    return crc32;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvBool isChecksumValid(NvU8 *pBuf)
{
    NvU8 i;
    NvU8 checksum = 0;
    
    for (i= 0; i < NVT_EDID_BLOCK_SIZE; i++)
    {        
        checksum += pBuf[i];
    }

    if ((checksum & 0xFF) == 0)
    {
        return NV_TRUE;
    }
    
    return NV_FALSE;    
}

CODE_SEGMENT(PAGE_DD_CODE)
void patchChecksum(NvU8 *pBuf)
{
    NvU8 i;
    NvU8 chksum = 0;

    for (i = 0; i < NVT_EDID_BLOCK_SIZE; i++)
    {
        chksum += pBuf[i];
    }
    chksum &= 0xFF;

    // The 1-byte sum of all 128 bytes in this EDID block shall equal zero
    // The Checksum Byte (at address 7Fh) shall contain a value such that a checksum of the entire
    // 128-byte BASE EDID equals 00h.
    if (chksum)
    {
        pBuf[127] = 0xFF & (pBuf[127] + (0x100 - chksum));
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ComposeCustTimingString(NVT_TIMING *pT)
{
    if (pT == NULL) 
        return NVT_STATUS_ERR;

    NVT_SNPRINTF((char *)pT->etc.name, 40, "CUST:%dx%dx%d.%03dHz%s",pT->HVisible, (pT->interlaced ? 2 : 1)*pT->VVisible , pT->etc.rrx1k/1000, pT->etc.rrx1k%1000, (pT->interlaced ? "/i" : ""));
    pT->etc.name[39] = '\0';

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU16 NvTiming_CalcRR(NvU32 pclk, NvU16 interlaced, NvU16 HTotal, NvU16 VTotal)
{
    NvU16 rr = 0;

    if (interlaced)
    {
        NvU32 totalPixelsIn2Fields = (NvU32)HTotal * ((NvU32)VTotal * 2 + 1);

        if (totalPixelsIn2Fields != 0)
        {
            rr = (NvU16)axb_div_c_64((NvU64)pclk * 2, (NvU64)10000, (NvU64)totalPixelsIn2Fields);
        }
    }
    else
    {
        NvU32 totalPixels = (NvU32)HTotal * VTotal;

        if (totalPixels != 0)
        {
            rr = (NvU16)axb_div_c_64((NvU64)pclk, (NvU64)10000, (NvU64)totalPixels);
        }
    }
    return rr;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_CalcRRx1k(NvU32 pclk, NvU16 interlaced, NvU16 HTotal, NvU16 VTotal)
{
    NvU32 rrx1k = 0;

    if (interlaced)
    {
        NvU32 totalPixelsIn2Fields = (NvU32)HTotal * ((NvU32)VTotal * 2 + 1);

        if (totalPixelsIn2Fields != 0)
        {
            rrx1k = (NvU32)axb_div_c_64((NvU64)pclk * 2, (NvU64)10000000, (NvU64)totalPixelsIn2Fields);
        }
    }
    else
    {
        NvU32 totalPixels = (NvU32)HTotal * VTotal;

        if (totalPixels != 0)
        {
            rrx1k = (NvU32)axb_div_c_64((NvU64)pclk, (NvU64)10000000, (NvU64)totalPixels);
        }
    }
 
    return rrx1k;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_IsRoundedRREqual(NvU16 rr1, NvU32 rr1x1k, NvU16 rr2)
{
    return ((rr1 >= (rr1x1k/1000)) && (rr1 <= (rr1x1k + 500) / 1000) &&
            (rr2 >= (rr1x1k/1000)) && (rr2 <= (rr1x1k + 500) / 1000));
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 NvTiming_IsTimingExactEqual(const NVT_TIMING *pT1, const NVT_TIMING *pT2)
{
    if ((pT1 == NULL) || (pT2 == NULL))
    {
        return 0;
    }

    return ((  pT1->HVisible     ==   pT2->HVisible) &&
            (  pT1->HBorder      ==   pT2->HBorder) &&
            (  pT1->HFrontPorch  ==   pT2->HFrontPorch) &&
            (  pT1->HSyncWidth   ==   pT2->HSyncWidth) &&
            (  pT1->HSyncPol     ==   pT2->HSyncPol) &&
            (  pT1->HTotal       ==   pT2->HTotal) &&
            (  pT1->VVisible     ==   pT2->VVisible) &&
            (  pT1->VBorder      ==   pT2->VBorder) &&
            (  pT1->VFrontPorch  ==   pT2->VFrontPorch) &&
            (  pT1->VSyncWidth   ==   pT2->VSyncWidth) &&
            (  pT1->VSyncPol     ==   pT2->VSyncPol) &&
            (  pT1->VTotal       ==   pT2->VTotal) &&
            (  pT1->etc.rr       ==   pT2->etc.rr) &&
            (!!pT1->interlaced   == !!pT2->interlaced));
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 NvTiming_IsTimingExactEqualEx(const NVT_TIMING *pT1, const NVT_TIMING *pT2)
{
    NvU32 bIsTimingExactEqual = NvTiming_IsTimingExactEqual(pT1, pT2);
    return (bIsTimingExactEqual && (pT1->etc.rrx1k == pT2->etc.rrx1k));
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 NvTiming_IsTimingRelaxedEqual(const NVT_TIMING *pT1, const NVT_TIMING *pT2)
{
    if ((pT1 == NULL) || (pT2 == NULL))
    {
        return 0;
    } 

    return ((  pT1->HVisible     ==   pT2->HVisible) &&
            (  pT1->HBorder      ==   pT2->HBorder) &&
            (  pT1->HFrontPorch  ==   pT2->HFrontPorch) &&
            (  pT1->HSyncWidth   ==   pT2->HSyncWidth) &&
          //(  pT1->HSyncPol     ==   pT2->HSyncPol) &&   // skip the polarity check to tolerate mismatch h/v sync polarities in 18-byte DTD
            (  pT1->HTotal       ==   pT2->HTotal) &&
            (  pT1->VVisible     ==   pT2->VVisible) &&
            (  pT1->VBorder      ==   pT2->VBorder) &&
            (  pT1->VFrontPorch  ==   pT2->VFrontPorch) &&
            (  pT1->VSyncWidth   ==   pT2->VSyncWidth) &&
          //(  pT1->VSyncPol     ==   pT2->VSyncPol) &&   // skip the polarity check to tolerate mismatch h/v sync polarities in 18-byte DTD
            (  pT1->VTotal       ==   pT2->VTotal) &&
            (  pT1->etc.rr       ==   pT2->etc.rr) &&
            (!!pT1->interlaced   == !!pT2->interlaced));
}

CODE_SEGMENT(NONPAGE_DD_CODE)
NvU32 RRx1kToPclk (NVT_TIMING *pT)
{
    return axb_div_c(pT->HTotal * (pT->VTotal + ((pT->interlaced != 0) ? (pT->VTotal + 1) : 0)),
                     pT->etc.rrx1k,
                     1000 * ((pT->interlaced != 0) ? 20000 : 10000));
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU16 NvTiming_MaxFrameWidth(NvU16 HVisible, NvU16 repMask)
{
    NvU16 minPixelRepeat;

    if (repMask == 0)
    {
        return HVisible;
    }

    minPixelRepeat = 1;
    while ((repMask & 1) == 0)
    {
        repMask >>= 1;
        minPixelRepeat++;
    }

    return (HVisible / minPixelRepeat);
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_GetVrrFmin(
    const NVT_EDID_INFO *pEdidInfo,
    const NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo,
    NvU32 nominalRefreshRateHz,
    NVT_PROTOCOL sinkProtocol)
{
    NvU32 fmin = 0;

    // DP Adaptive Sync
    if (sinkProtocol == NVT_PROTOCOL_DP)
    {
        if (pEdidInfo)
        {
            if (pEdidInfo->ext_displayid.version)
            {
                fmin = pEdidInfo->ext_displayid.range_limits[0].vfreq_min;
            }

            if (pEdidInfo->ext_displayid20.version && pEdidInfo->ext_displayid20.range_limits.seamless_dynamic_video_timing_change)
            {
                fmin = pEdidInfo->ext_displayid20.range_limits.vfreq_min;
            }

            // DisplayID 2.0 extension
            if (pEdidInfo->ext_displayid20.version && pEdidInfo->ext_displayid20.total_adaptive_sync_descriptor != 0)
            {
                // Go through all the Adaptive Sync Data Blocks and pick the right frequency based on nominalRR
                NvU32 i;
                for (i = 0; i < pEdidInfo->ext_displayid20.total_adaptive_sync_descriptor; i++)
                {
                    if ((pEdidInfo->ext_displayid20.adaptive_sync_descriptor[i].max_rr == nominalRefreshRateHz) ||
                        (nominalRefreshRateHz == 0))
                    {
                        fmin = pEdidInfo->ext_displayid20.adaptive_sync_descriptor[i].min_rr;
                        break;
                    }
                }
            }

            if (!fmin)
            {
                NvU32 i;
                for (i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
                {
                    if (pEdidInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_DRL)
                    {
                        fmin = pEdidInfo->ldd[i].u.range_limit.min_v_rate;
                    }
                }
            }

            // Gsync
            if (pEdidInfo->nvdaVsdbInfo.valid)
            {
                fmin = NV_MAX(pEdidInfo->nvdaVsdbInfo.vrrData.v1.minRefreshRate, 10);
            }
        }

        // Display ID 2.0 Standalone
        if (pDisplayIdInfo)
        {
            // Go through all the Adaptive Sync Data Blocks and pick the right frequency based on nominalRR
            NvU32 i;
            for (i = 0; i < pDisplayIdInfo->total_adaptive_sync_descriptor; i++)
            {
                if ((pDisplayIdInfo->adaptive_sync_descriptor[i].max_rr == nominalRefreshRateHz) ||
                    (nominalRefreshRateHz == 0))
                {
                    fmin = pDisplayIdInfo->adaptive_sync_descriptor[i].min_rr;
                    break;
                }
            }
            // If unable to find the value, choose a fallback from DisplayId
            if (!fmin)
            {
                fmin = pDisplayIdInfo->range_limits.vfreq_min;
            }
        }
    }

    // HDMI 2.1 VRR
    else if (sinkProtocol == NVT_PROTOCOL_HDMI)
    {
        if (pEdidInfo)
        {
            fmin = pEdidInfo->hdmiForumInfo.vrr_min;
        }
    }

    return fmin;
}

POP_SEGMENTS
