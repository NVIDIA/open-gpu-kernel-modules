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
//  File:       nvt_edid.c
//
//  Purpose:    the provide edid related services
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "edid.h"

PUSH_SEGMENTS

// Macro to declare a TIMING initializer for given parameters without border
#define EST_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rr,pclk,format) \
{hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',NVT_PROGRESSIVE,pclk,{0,rr,set_rrx1k(pclk,ht,vt),0,1,{0},{0},{0},{0},format,"VESA Established"}}

DATA_SEGMENT(PAGE_DATA)
#if !defined(NV_WSA)
CONS_SEGMENT(PAGE_CONS)
#endif // wsa

// There is a large table of strings that translate 3-character PNP vendor IDs to a more user-friendly name in the following header.
// Mark this constant table as pageable.
#include "nvPNPVendorIds.h"

static const NVT_TIMING EDID_EST[] =
{
    EST_TIMING( 720, 0,  0, 720,'-', 400, 0,0, 400,'-',70,    0,NVT_STATUS_EDID_EST), // 720x400x70Hz   (IBM, VGA)
    EST_TIMING( 720, 0,  0, 720,'-', 400, 0,0, 400,'-',88,    0,NVT_STATUS_EDID_EST), // 720x400x88Hz   (IBM, XGA2)
    {640,0,16,96,800,NVT_H_SYNC_NEGATIVE,480,0,10,2,525,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,2518,{0,60,60000,0,1,{0},{0},{0},{0},NVT_STATUS_EDID_EST,"EDID_Established"}},

    EST_TIMING( 640, 0,  0, 640,'-', 480, 0,0, 480,'-',67,    0,NVT_STATUS_EDID_EST), // 640x480x67Hz   (Apple, Mac II)

    // 640x480x72Hz (VESA) - this entry have borders
    {640,8,16,40,832,NVT_H_SYNC_NEGATIVE,480,8,1,3,520,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,3150,{0,72,72000,0,1,{0},{0},{0},{0},NVT_STATUS_EDID_EST,"EDID_Established"}},
    EST_TIMING( 640,16, 64, 840,'-', 480, 1,3, 500,'-',75, 3150,NVT_STATUS_EDID_EST), // 640x480x75Hz   (VESA)
    EST_TIMING( 800,24, 72,1024,'+', 600, 1,2, 625,'+',56, 3600,NVT_STATUS_EDID_EST), // 800x600x56Hz   (VESA)
    EST_TIMING( 800,40,128,1056,'+', 600, 1,4, 628,'+',60, 4000,NVT_STATUS_EDID_EST), // 800x600x60Hz   (VESA)

    EST_TIMING( 800,56,120,1040,'+', 600,37,6, 666,'+',72, 5000,NVT_STATUS_EDID_EST), // 800x600x72Hz   (VESA)
    EST_TIMING( 800,16, 80,1056,'+', 600, 1,3, 625,'+',75, 4950,NVT_STATUS_EDID_EST), // 800x600x75Hz   (VESA)
    EST_TIMING( 832, 0,  0, 832,'-', 624, 0,0, 624,'-',75,    0,NVT_STATUS_EDID_EST), // 832x624x75Hz   (Apple, Mac II)
    EST_TIMING(1024, 0,  0,1024,'-', 768, 0,0, 768,'-',87,    0,NVT_STATUS_EDID_EST), // 1024x768x87Hz  (IBM, Interlaced)

    EST_TIMING(1024,24,136,1344,'-', 768, 3,6, 806,'-',60, 6500,NVT_STATUS_EDID_EST), // 1024x768x60Hz  (VESA)
    EST_TIMING(1024,24,136,1328,'-', 768, 3,6, 806,'-',70, 7500,NVT_STATUS_EDID_EST), // 1024x768x70Hz  (VESA)
    EST_TIMING(1024,16, 96,1312,'+', 768, 1,3, 800,'+',75, 7875,NVT_STATUS_EDID_EST), // 1024x768x75Hz  (VESA)
    EST_TIMING(1280,16,144,1688,'+',1024, 1,3,1066,'+',75,13500,NVT_STATUS_EDID_EST), // 1280x1024x75Hz (VESA)

    // the end
    NVT_TIMING_SENTINEL
};

static NvU32 MAX_EST_FORMAT = sizeof(EDID_EST)/sizeof(EDID_EST[0]) - 1;

static const NVT_TIMING EDID_ESTIII[] =
{
    EST_TIMING( 640, 32, 64, 832,'+', 350,32,3, 445,'-',85, 3150,NVT_STATUS_EDID_EST), // 640x350x85Hz
    EST_TIMING( 640, 32, 64, 832,'-', 400, 1,3, 445,'+',85, 3150,NVT_STATUS_EDID_EST), // 640x400x85Hz
    EST_TIMING( 720, 36, 72, 936,'-', 400, 1,3, 446,'+',85, 3550,NVT_STATUS_EDID_EST), // 720x400x85Hz
    EST_TIMING( 640, 56, 56, 832,'-', 480, 1,3, 509,'-',85, 3600,NVT_STATUS_EDID_EST), // 640x480x85Hz
    EST_TIMING( 848, 16,112,1088,'+', 480, 6,8, 517,'+',60, 3375,NVT_STATUS_EDID_EST), // 848x480x60HZ
    EST_TIMING( 800, 32, 64,1048,'+', 600, 1,3, 631,'+',85, 5625,NVT_STATUS_EDID_EST), // 800x600x85Hz
    EST_TIMING(1024, 48, 96,1376,'+', 768, 1,3, 808,'+',85, 9450,NVT_STATUS_EDID_EST), // 1024x768x85Hz
    EST_TIMING(1152, 64,128,1600,'+', 864, 1,3, 900,'+',75,10800,NVT_STATUS_EDID_EST), // 1152x864x75Hz

    EST_TIMING(1280, 48, 32,1440,'+', 768, 3,7, 790,'-',60, 6825,NVT_STATUS_EDID_EST), // 1280x768x60Hz (RB)
    EST_TIMING(1280, 64,128,1664,'-', 768, 3,7, 798,'+',60, 7950,NVT_STATUS_EDID_EST), // 1280x768x60Hz
    EST_TIMING(1280, 80,128,1696,'-', 768, 3,7, 805,'+',75,10225,NVT_STATUS_EDID_EST), // 1280x768x75Hz
    EST_TIMING(1280, 80,136,1712,'-', 768, 3,7, 809,'+',85,11750,NVT_STATUS_EDID_EST), // 1280x768x85Hz
    EST_TIMING(1280, 96,112,1800,'+', 960, 1,3,1000,'+',60,10800,NVT_STATUS_EDID_EST), // 1280x960x60Hz
    EST_TIMING(1280, 64,160,1728,'+', 960, 1,3,1011,'+',85,14850,NVT_STATUS_EDID_EST), // 1280x960x85Hz
    EST_TIMING(1280, 48,112,1688,'+',1024, 1,3,1066,'+',60,10800,NVT_STATUS_EDID_EST), // 1280x1024x60Hz
    EST_TIMING(1280, 64,160,1728,'+',1024, 1,3,1072,'+',85,15750,NVT_STATUS_EDID_EST), // 1280x1024x85Hz

    EST_TIMING(1360, 64,112,1792,'+', 768, 3,6, 795,'+',60, 8550,NVT_STATUS_EDID_EST), // 1360x768x60Hz
    EST_TIMING(1440, 48, 32,1600,'+', 900, 3,6, 926,'-',60, 8875,NVT_STATUS_EDID_EST), // 1440x900x60Hz (RB)
    EST_TIMING(1440, 80,152,1904,'-', 900, 3,6, 934,'+',60,10650,NVT_STATUS_EDID_EST), // 1440x900x60Hz
    EST_TIMING(1440, 96,152,1936,'-', 900, 3,6, 942,'+',75,13675,NVT_STATUS_EDID_EST), // 1440x900x75Hz
    EST_TIMING(1440,104,152,1952,'-', 900, 3,6, 948,'+',85,15700,NVT_STATUS_EDID_EST), // 1440x900x85Hz
    EST_TIMING(1400, 48, 32,1560,'+',1050, 3,4,1080,'-',60,10100,NVT_STATUS_EDID_EST), // 1440x1050x60Hz (RB)
    EST_TIMING(1400, 88,144,1864,'-',1050, 3,4,1089,'+',60,12175,NVT_STATUS_EDID_EST), // 1440x1050x60Hz
    EST_TIMING(1400,104,144,1896,'-',1050, 3,4,1099,'+',75,15600,NVT_STATUS_EDID_EST), // 1440x1050x75Hz

    EST_TIMING(1400,104,152,1912,'-',1050, 3,4,1105,'+',85,17950,NVT_STATUS_EDID_EST), // 1440x1050x85Hz
    EST_TIMING(1680, 48, 32,1840,'+',1050, 3,6,1080,'-',60,11900,NVT_STATUS_EDID_EST), // 1680x1050x60Hz (RB)
    EST_TIMING(1680,104,176,2240,'-',1050, 3,6,1089,'+',60,14625,NVT_STATUS_EDID_EST), // 1680x1050x60Hz
    EST_TIMING(1680,120,176,2272,'-',1050, 3,6,1099,'+',75,18700,NVT_STATUS_EDID_EST), // 1680x1050x75Hz
    EST_TIMING(1680,128,176,2288,'-',1050, 3,6,1105,'+',85,21475,NVT_STATUS_EDID_EST), // 1680x1050x85Hz
    EST_TIMING(1600, 64,192,2160,'+',1200, 1,3,1250,'+',60,16200,NVT_STATUS_EDID_EST), // 1600x1200x60Hz
    EST_TIMING(1600, 64,192,2160,'+',1200, 1,3,1250,'+',65,17550,NVT_STATUS_EDID_EST), // 1600x1200x65Hz
    EST_TIMING(1600, 64,192,2160,'+',1200, 1,3,1250,'+',70,18900,NVT_STATUS_EDID_EST), // 1600x1200x70Hz

    EST_TIMING(1600, 64,192,2160,'+',1200, 1,3,1250,'+',75,20250,NVT_STATUS_EDID_EST), // 1600x1200x75Hz
    EST_TIMING(1600, 64,192,2160,'+',1200, 1,3,1250,'+',85,22950,NVT_STATUS_EDID_EST), // 1600x1200x85Hz
    EST_TIMING(1792,128,200,2448,'-',1344, 1,3,1394,'+',60,20475,NVT_STATUS_EDID_EST), // 1792x1344x60Hz
    EST_TIMING(1792, 96,216,2456,'-',1344, 1,3,1417,'+',75,26100,NVT_STATUS_EDID_EST), // 1792x1344x75Hz
    EST_TIMING(1856, 96,224,2528,'-',1392, 1,3,1439,'+',60,21825,NVT_STATUS_EDID_EST), // 1856x1392x60Hz
    EST_TIMING(1856,128,224,2560,'-',1392, 1,3,1500,'+',75,28800,NVT_STATUS_EDID_EST), // 1856x1392x75Hz
    EST_TIMING(1920, 48, 32,2080,'+',1200, 3,6,1235,'-',60,15400,NVT_STATUS_EDID_EST), // 1920x1200x60Hz (RB)
    EST_TIMING(1920,136,200,2592,'-',1200, 3,6,1245,'+',60,19325,NVT_STATUS_EDID_EST), // 1920x1200x60Hz

    EST_TIMING(1920,136,208,2608,'-',1200, 3,6,1255,'+',75,24525,NVT_STATUS_EDID_EST), // 1920x1200x75Hz
    EST_TIMING(1920,144,208,2624,'-',1200, 3,6,1262,'+',85,28125,NVT_STATUS_EDID_EST), // 1920x1200x85Hz
    EST_TIMING(1920,128,208,2600,'-',1440, 1,3,1500,'+',60,23400,NVT_STATUS_EDID_EST), // 1920x1440x60Hz
    EST_TIMING(1920,144,224,2640,'-',1440, 1,3,1500,'+',75,29700,NVT_STATUS_EDID_EST), // 1920x1440x75Hz

    NVT_TIMING_SENTINEL,
    NVT_TIMING_SENTINEL,
    NVT_TIMING_SENTINEL,
    NVT_TIMING_SENTINEL
};

static NvU32 MAX_ESTIII_FORMAT = sizeof(EDID_ESTIII)/sizeof(EDID_ESTIII[0]) - 1;

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumEST(NvU32 index, NVT_TIMING *pT)
{
    if ((pT == NULL) || (index > MAX_EST_FORMAT))
    {
        return NVT_STATUS_ERR;
    }

    *pT = EDID_EST[index];

    if (pT->HTotal == 0 || pT->VTotal == 0)
    {
        return NVT_STATUS_ERR;
    }

    pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk,
                              (NvU32)10000*(NvU32)1000,
                              (NvU32)pT->HTotal*(NvU32)pT->VTotal);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumESTIII(NvU32 index, NVT_TIMING *pT)
{
    if ((pT == NULL) || (index > MAX_ESTIII_FORMAT))
    {
        return NVT_STATUS_ERR;
    }

    *pT = EDID_ESTIII[index];

    if (pT->HTotal == 0 || pT->VTotal == 0)
    {
        return NVT_STATUS_ERR;
    }

    pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk,
                              (NvU32)10000*(NvU32)1000,
                              (NvU32)pT->HTotal*(NvU32)pT->VTotal);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 isHdmi3DStereoType(NvU8 StereoStructureType)
{
    return ((NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK      == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_FIELD_ALT      == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_LINE_ALT       == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEFULL == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTH         == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTHGFX      == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM      == StereoStructureType) ||
            (NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF == StereoStructureType));
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_GetVESADisplayDescriptorVersion(NvU8 *rawData, NvU32 *pVer)
{
    return getEdidVersion(rawData, pVer);
}

// get the EDID version
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 getEdidVersion(NvU8 *pEdid, NvU32 *pVer)
{
    EDIDV1STRUC *p = (EDIDV1STRUC *) pEdid;

    if (pEdid[0] == 0x00)
    {
        // For Version 1.x, first 8 bytes of EDID must be 00h, FFh, FFh, FFh, FFh, FFh, FFh, 00h.
        // Beware of Endian-ness and signed-ness.
        if (p->bHeader[1] != 0xFF || p->bHeader[2] != 0xFF || p->bHeader[3] != 0xFF ||
            p->bHeader[4] != 0xFF || p->bHeader[5] != 0xFF || p->bHeader[6] != 0xFF ||
            p->bHeader[7] != 0x00)
            return NVT_STATUS_ERR;

        *pVer = (((NvU32) p->bVersionNumber) << 8) + ((NvU32) p->bRevisionNumber);
    }
    else if ((pEdid[0] & 0xF0) == 0x20 && (pEdid[0] & 0x0F) >=0)
        *pVer = (((NvU32) (pEdid[0] & 0XF0) << 4) + (NvU32) (pEdid[0] & 0X0F)) ;  // DisplayID version 2.x
    else
        return NVT_STATUS_ERR;          // un-recongnized EDID version

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidCvt3ByteDescriptor(NvU8 *p, NVT_EDID_INFO *pInfo, NvU32 *vtbCount)
{
    NvU32 k;
    NvU32 width, height, aspect, rr = 0;
    NVT_EDID_DD_CVT_3BYTE_BLOCK *pTiming = (NVT_EDID_DD_CVT_3BYTE_BLOCK *)p;
    NVT_TIMING newTiming;
    NVT_STATUS status;


    if (pTiming->addressable_lines == 0)
        return;

    height = pTiming->addressable_lines;
    aspect = pTiming->aspect_ratio;

    if (aspect == NVT_EDID_CVT3_ASPECT_4X3)
        width = height * 4 / 3;
    else if (aspect == NVT_EDID_CVT3_ASPECT_16X9)
        width = height * 16 / 9;
    else if (aspect == NVT_EDID_CVT3_ASPECT_16X10)
        width = height * 16 / 10;
    else                         //15:9
        width = height * 15 / 9;

    width &= 0xFFFFFFF8; // round down to nearest 8

    // loop through bits4:0 of supported_vert_rate so we can add a timing
    // for each supported rate
    for (k=1; k<=0x10; k<<=1)
    {
        // skip if this bit indicate no support for the rate;
        if ( (pTiming->supported_vert_rates & (k)) == 0)
            continue;

        // find the correct refresh rate for this bit
        switch (k)
        {
            case NVT_EDID_CVT3_SUPPORTED_RATE_60HZ_REDUCED_BLANKING :
            case NVT_EDID_CVT3_SUPPORTED_RATE_60HZ :
                rr = 60;
                break;
            case NVT_EDID_CVT3_SUPPORTED_RATE_85HZ :
                rr = 85;
                break;
            case NVT_EDID_CVT3_SUPPORTED_RATE_75HZ :
                rr = 75;
                break;
            case NVT_EDID_CVT3_SUPPORTED_RATE_50HZ :
                rr = 50;
                break;
        }

        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if ( (k) != NVT_EDID_CVT3_SUPPORTED_RATE_60HZ_REDUCED_BLANKING) // standard blanking
        {
            status = NvTiming_CalcCVT(width, height, rr,
                                      NVT_PROGRESSIVE,
                                      &newTiming);
        }
        else // reduced blanking
        {
            status = NvTiming_CalcCVT_RB(width, height, rr,
                                         NVT_PROGRESSIVE,
                                         &newTiming);
        }

        if (status == NVT_STATUS_SUCCESS)
        {
            // For VTB timings, add additional information
            if (vtbCount)
            {
                (*vtbCount)++;
                newTiming.etc.status = NVT_STATUS_EDID_VTB_EXT_CVTn(*vtbCount);
                newTiming.etc.name[39] = '\0';
            }

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }
        }
    } // for (k=1; k<=0x10; k<<=1)

}

// parse the EDID 1.x based cvt timing info
CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidCvtTiming(NVT_EDID_INFO *pInfo)
{
    NvU32 i, j;

    // find display range limit with cvt, or cvt 3-byte LDDs
    for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        if ( pInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_CVT )
        {
            NVT_EDID_DD_CVT_3BYTE *pCVT = (NVT_EDID_DD_CVT_3BYTE *)&pInfo->ldd[i].u.cvt;

            // loop through cvt 3-byte blocks
            for (j=0; j<NVT_EDID_DD_MAX_CVT3_PER_DESCRITPOR; j++)
            {
                parseEdidCvt3ByteDescriptor((NvU8 *)(pCVT->block + j),
                                            pInfo, NULL);
            } // for(j=0; j<NVT_EDID_DD_CVT3_BLOCK_NUM; j++)
        } // else if CVT 3-byte block
    } // for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
}

// parse the EDID 1.x based established timing info
CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidEstablishedTiming(NVT_EDID_INFO *pInfo)
{
    NvU32 i, j, k;
    NvU32 count = 0;
    NVT_TIMING newTiming;

    for (i = 1 << (sizeof(pInfo->established_timings_1_2) * 8 - 1), j = 0; i != 0; i >>= 1, j ++)
    {
        if ((pInfo->established_timings_1_2 & i) != 0 && EDID_EST[j].pclk != 0)
        {
            // count the timing
            newTiming = EDID_EST[j];
            newTiming.etc.status = NVT_STATUS_EDID_ESTn(++count);
            NVT_SNPRINTF((char *)newTiming.etc.name, 40,
                         "EDID-EST(VESA):%dx%dx%dHz",
                         (int)newTiming.HVisible,
                         (int)newTiming.VVisible,
                         (int)newTiming.etc.rr);
            newTiming.etc.name[39] = '\0';

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }
        }
    }

    // ESTIII block in ldd only supported in EDID1.4 and above
    if (pInfo->version  < NVT_EDID_VER_1_4)
        return;

    for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        if ( pInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_ESTIII )
        {
            NVT_EDID_DD_EST_TIMING3* pEST = &pInfo->ldd[i].u.est3;

            for (j=0; j<NVT_EDID_DD_EST_TIMING3_NUM; j++) // loop through each byte
            {
                // loop through each bit in the byte
                for (k=7; k<=7; k--)
                {
                    // find if the bit is 1 and we have a valid associated timing
                    if (pEST->data[j] & (1<<k) && EDID_ESTIII[(j*8)+(7-k)].pclk != 0)
                    {
                        newTiming = EDID_ESTIII[(j*8)+(7-k)];
                        newTiming.etc.status = NVT_STATUS_EDID_ESTn(++count);
                        NVT_SNPRINTF((char *)newTiming.etc.name, 40,
                                     "EDID-EST(III):%dx%dx%dHz",
                                     (int)newTiming.HVisible,
                                     (int)newTiming.VVisible,
                                     (int)newTiming.etc.rr);
                        newTiming.etc.name[39] = '\0';

                        if (!assignNextAvailableTiming(pInfo, &newTiming))
                        {
                            break;
                        }
                    }
                }
            }
            break;
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidStandardTimingDescriptor(NvU16 timing, NVT_EDID_INFO *pInfo, NvU32 count, NVT_TIMING * pT)
{
    NvU32 aspect, width, height, rr;
    count++;

    // The value in the EDID = (Horizontal active pixels/8) - 31
    width = (timing & 0x0FF) + 31;
    width <<= 3;
    rr = ((timing >> 8) & 0x3F) + 60; // bits 5->0

    // get the height
    aspect = ((timing >> 8) & 0xC0); // aspect ratio at bit 7:6
    if (aspect == 0x00)
        height = (pInfo->version < 0x103) ? width : (width * 5 / 8); //16:10 per EDID1.3 and 1:1 with earlier EDID
    else if (aspect == 0x40)
        height = width * 3 / 4;                                    //4:3
    else if (aspect == 0x80)
        height = width * 4 / 5;                                    //5:4
    else
        height = width * 9 / 16;                                   //16:9

    // try to get the timing from DMT first
    if (NvTiming_CalcDMT(width, height, rr, 0, pT) == NVT_STATUS_SUCCESS)
    {
        pT->etc.status = NVT_STATUS_EDID_STDn(count);
        NVT_SNPRINTF((char *)pT->etc.name, 40, "EDID-STD(DMT):%dx%dx%dHz", (int)width, (int)height, (int)rr);
        pT->etc.name[39] = '\0';
    }
    else if (pInfo->version >= NVT_EDID_VER_1_4)
    {
        // EDID1.4 and above defaults to CVT, instead of GTF. GTF is deprecated as of 1.4.
        NvTiming_CalcCVT(width, height, rr, NVT_PROGRESSIVE, pT);
        pT->etc.status = NVT_STATUS_EDID_STDn(count);
        NVT_SNPRINTF((char *)pT->etc.name, 40, "EDID-STD(CVT):%dx%dx%dHz", (int)width, (int)height, (int)rr);
        pT->etc.name[39] = '\0';
    }
    else
    {
        // if the mode is not found in DMT, use GTF timing
        NvTiming_CalcGTF(width, height, rr, NVT_PROGRESSIVE, pT);
        pT->etc.status = NVT_STATUS_EDID_STDn(count);
        NVT_SNPRINTF((char *)pT->etc.name, 40, "EDID-STD(GTF):%dx%dx%dHz", (int)width, (int)height, (int)rr);
        pT->etc.name[39] = '\0';
    }
}

// parse the EDID 1.x based standard timing info
CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidStandardTiming(NVT_EDID_INFO *pInfo)
{
    NvU32 i, j;
    NVT_TIMING newTiming;
    NvU32 count = 0;

    // now check for standard timings
    for (i=0; i<NVT_EDID_MAX_STANDARD_TIMINGS; i++)
    {
        if (((pInfo->standard_timings[i] & 0x0FF) != 0x1) &&    //proper indication of unused field
             (pInfo->standard_timings[i] != 0x0))               //improper indication (bad edid)
        {
            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

            parseEdidStandardTimingDescriptor(pInfo->standard_timings[i],
                                              pInfo, count, &newTiming);

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }

            count++;
        }//if ((pInfo->standard_timings[i] & 0x0FF) != 0x1)
    } //for (i=0; i<NVT_EDID_MAX_STANDARD_TIMINGS; i++)

    // STI block in ldd only supported in EDID1.4 and above
    if (pInfo->version < NVT_EDID_VER_1_4)
        return;

    // now check for standard timings in long display descriptors
    for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        if ( pInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_STI )
        {
            NVT_EDID_DD_STD_TIMING* pSTI = &pInfo->ldd[i].u.std_timing;
            for (j=0; j<NVT_EDID_DD_STI_NUM; j++)
            {
                if ((pSTI->descriptor[j] & 0x0FF) != 0x00)
                {
                    NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

                    parseEdidStandardTimingDescriptor(pSTI->descriptor[j],
                                                      pInfo, count, &newTiming);

                    if (!assignNextAvailableTiming(pInfo, &newTiming))
                    {
                        break;
                    }

                    count++;
                } // if ((pSTI->std_timing[i] & 0x0FF) != 0x1)
            } //for (j=0; j<NVT_EDID_DD_STI_NUM; j++)
        } //if ( ((EDID_LONG_DISPLAY_DESCRIPTOR)p->DetailedTimingDesc[i]).tag = NVT_EDID_DISPLAY_DESCRIPTOR_STI )
    } //for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
}

// parse the signal detailed timing descriptor
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS parseEdidDetailedTimingDescriptor(NvU8 *p, NVT_TIMING *pT)
{
    DETAILEDTIMINGDESCRIPTOR *pDTD = (DETAILEDTIMINGDESCRIPTOR *)p;
    NvU32 dwTotalPixels;
    NvS32 hvisible, vvisible;

    // bypass input parameter check  in the private function


    // For EDID ver 1.1 & 1.2:
    // All bytes filled with 1 are considered as an empty block and should be skipped.
    // Here we just check first 5 bytes.
    // Release 5th byte zero check. Now we only check first 4 bytes. Some buggy EDID has
    // data in 5th byte.
    if(pDTD && (pDTD->wDTPixelClock !=0          || pDTD->bDTHorizontalActive !=0)
            && (pDTD->wDTPixelClock != 0x0101    || pDTD->bDTHorizontalActive != 1 ||
                pDTD->bDTHorizontalBlanking != 1 || pDTD->bDTHorizActiveBlank != 1))
    {
        // Note that hvisible and vvisible here correspond to the "Addressable Video" portion of the
        // "Active Video" defined in the EDID spec (see section 3.12: Note Regarding Borders)
        hvisible = (pDTD->bDTHorizontalActive + ((pDTD->bDTHorizActiveBlank & 0xF0) << 4)) - 2 * pDTD->bDTHorizontalBorder;
        vvisible = (pDTD->bDTVerticalActive + ((pDTD->bDTVertActiveBlank & 0xF0) << 4)) - 2 * pDTD->bDTVerticalBorder;

        // Sanity check since we are getting values from the monitor
        if (hvisible <= 0 || vvisible <= 0 || pDTD->wDTPixelClock == 0)
        {
            if (pT)
                pT->HVisible = 0;
            return NVT_STATUS_ERR;
        }

        // if the output timing buffer is not provide, simply return here to indicate a legal descriptor
        if (pT == NULL)
            return NVT_STATUS_SUCCESS;

        // horizontal timing parameters
        pT->HVisible    = (NvU16)hvisible;
        pT->HBorder     = (NvU16)pDTD->bDTHorizontalBorder;
        pT->HTotal      = (NvU16)hvisible + (NvU16)(pDTD->bDTHorizontalBlanking + ((pDTD->bDTHorizActiveBlank & 0x0F) << 8)) + pT->HBorder * 2;
        pT->HFrontPorch = (NvU16)(pDTD->bDTHorizontalSync + ((pDTD->bDTHorizVertSyncOverFlow & 0xC0) << 2));
        pT->HSyncWidth  = (NvU16)(pDTD->bDTHorizontalSyncWidth + ((pDTD->bDTHorizVertSyncOverFlow & 0x30) << 4));

        // vertical timing parameters
        pT->VVisible    = (NvU16)vvisible;
        pT->VBorder     = (NvU16)pDTD->bDTVerticalBorder;
        pT->VTotal      = (NvU16)vvisible + (NvU16)(pDTD->bDTVerticalBlanking + ((pDTD->bDTVertActiveBlank & 0x0F) << 8)) + pT->VBorder * 2;
        pT->VFrontPorch = (NvU16)(((pDTD->bDTVerticalSync & 0xF0) >> 4) + ((pDTD->bDTHorizVertSyncOverFlow & 0x0C) << 2));
        pT->VSyncWidth  = (NvU16)((pDTD->bDTVerticalSync & 0x0F) + ((pDTD->bDTHorizVertSyncOverFlow & 0x03) << 4));

        // pixel clock
        pT->pclk = (NvU32)pDTD->wDTPixelClock;

        // sync polarities
        if ((pDTD->bDTFlags & 0x18) == 0x18)
        {
            pT->HSyncPol = ((pDTD->bDTFlags & 0x2) != 0) ? NVT_H_SYNC_POSITIVE : NVT_H_SYNC_NEGATIVE;
            pT->VSyncPol = ((pDTD->bDTFlags & 0x4) != 0) ? NVT_V_SYNC_POSITIVE : NVT_V_SYNC_NEGATIVE;
        }
        else if ((pDTD->bDTFlags & 0x18) == 0x10)
        {
            pT->HSyncPol = ((pDTD->bDTFlags & 0x2) != 0) ? NVT_H_SYNC_POSITIVE : NVT_H_SYNC_NEGATIVE;
            pT->VSyncPol = NVT_V_SYNC_POSITIVE;
        }
        else
        {
            pT->HSyncPol = NVT_H_SYNC_NEGATIVE;
            pT->VSyncPol = NVT_V_SYNC_POSITIVE;
        }

        // interlaced
        if ((pDTD->bDTFlags & 0x80) == 0x80)
            pT->interlaced = 1;
        else
            pT->interlaced = 0;

        // Eizo split EDID case, using 0th bit to indicate split display capability
        if (((pDTD->bDTFlags & 1) == 1) && !(((pDTD->bDTFlags & 0x20) == 0x20) || ((pDTD->bDTFlags & 0x40) == 0x40)))
        {
            pT->etc.flag |= NVT_FLAG_EDID_DTD_EIZO_SPLIT;
        }
        if (pT->interlaced)
        {
            // Adjust for one extra blank line in every other frame.
            dwTotalPixels = (((NvU32)pT->HTotal * pT->VTotal) +
                            ((NvU32)pT->HTotal * (pT->VTotal + 1))) / 2;
        }
        else
        {
            dwTotalPixels = (NvU32)pT->HTotal * pT->VTotal;
        }

        pT->etc.rr = (NvU16)(((NvU32)pDTD->wDTPixelClock*10000+dwTotalPixels/2)/dwTotalPixels);
        // Using utility call to multiply and divide to take care of overflow and truncation of large values
        // How did we arrive at 10000000? It comes from the fact that Pixel clock mentioned in EDID is in mulitples of 10KHz = 10000
        // and the refresh rate is mentioned in 0.001Hz, that is 60Hz will be represented as 60000, which brings in the factor of 1000.
        // And hence 10000 * 1000 = 10000000
        pT->etc.rrx1k = axb_div_c(pDTD->wDTPixelClock, 10000000, dwTotalPixels);
        pT->etc.status = NVT_STATUS_EDID_DTD;
        NVT_SNPRINTF((char *)pT->etc.name, sizeof(pT->etc.name), "EDID-Detailed:%dx%dx%d.%03dHz%s", (int)pT->HVisible, (int)(pT->interlaced ? 2 : 1)*pT->VVisible , (int)pT->etc.rrx1k/1000, (int)pT->etc.rrx1k%1000, (pT->interlaced ? "/i" : ""));
        pT->etc.name[sizeof(pT->etc.name) - 1] = '\0';

        // aspect ratio
        pT->etc.aspect = (pDTD->bDTHorizVertImage & 0xF0) << 20 | pDTD->bDTHorizontalImage << 16 |
                         (pDTD->bDTHorizVertImage & 0x0F) << 8  | pDTD->bDTVerticalImage;

        pT->etc.rep = 0x1; // Bit mask for no pixel repetition.

        return NVT_STATUS_SUCCESS;
    }

    return NVT_STATUS_ERR;
}

// parse the EDID 1.x based standard timing info
CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidDetailedTiming(NvU8 *pEdid, NVT_EDID_INFO *pInfo)
{
    EDIDV1STRUC *p = (EDIDV1STRUC *) pEdid;
    NVT_TIMING newTiming;
    NvU32 i;
    NvBool found = NV_FALSE;

    for (i = 0; i < 4; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseEdidDetailedTimingDescriptor((NvU8 *)&p->DetailedTimingDesc[i],
                                              &newTiming) == NVT_STATUS_SUCCESS)
        {
            newTiming.etc.status = NVT_STATUS_EDID_DTDn(i+1);

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }

            found = NV_TRUE;
        }
    }

    if (found)
    {
        // if edid_ver 1.3, PTM flag should be set
        //nvt_assert(pInfo->version > 0x103 || (pInfo->u.feature &
        //           NVT_EDID_OTHER_FEATURES_PTM_INCLUDE_NATIVE));

        if (pInfo->u.feature & NVT_EDID_OTHER_FEATURES_PTM_INCLUDE_NATIVE)
        {
            pInfo->timing[0].etc.flag |= NVT_FLAG_DTD1_PREFERRED_TIMING;
        }
    }
}


// parse the EDID 1.x 18-byte long display descriptor
CODE_SEGMENT(PAGE_DD_CODE)
static void parseEdidLongDisplayDescriptor(EDID_LONG_DISPLAY_DESCRIPTOR *descriptor, NVT_EDID_18BYTE_DESCRIPTOR *p, NvU32 version)
{
    NvU32 i;

    // bypass the input pointer check in this private function

    // return if it's a detailed timing descriptor
    if (descriptor->prefix[0] != 0 || descriptor->prefix[1] != 0)
        return;

    // other sanity check for the input data
    if (descriptor->rsvd != 0)
        return;

    p->tag = descriptor->tag;

    // now translate the descriptor
    switch (descriptor->tag)
    {
        case NVT_EDID_DISPLAY_DESCRIPTOR_DPSN:    // display product serial number
        case NVT_EDID_DISPLAY_DESCRITPOR_DPN:     // display product name
        case NVT_EDID_DISPLAY_DESCRIPTOR_ADS:     // alphanumeric data string (ASCII)

            // copy the 13 characters payload from the 18-byte descriptor
            for (i = 0; i < NVT_PVT_EDID_LDD_PAYLOAD_SIZE; i++)
            {
                if (descriptor->data[i] == 0x0A)
                    p->u.serial_number.str[i] = '\0';
                else
                    p->u.serial_number.str[i] = descriptor->data[i];
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_DRL:     // display range limit
            {
                EDID_MONITOR_RANGE_LIMIT *pRangeLimit = (EDID_MONITOR_RANGE_LIMIT *)&descriptor->data[0];

                p->u.range_limit.min_v_rate   = pRangeLimit->minVRate;
                p->u.range_limit.max_v_rate   = pRangeLimit->maxVRate;
                p->u.range_limit.min_h_rate   = pRangeLimit->minHRate;
                p->u.range_limit.max_h_rate   = pRangeLimit->maxHRate;
                p->u.range_limit.max_pclk_MHz = pRangeLimit->maxPClock10M * 10;
                p->u.range_limit.timing_support = pRangeLimit->timing_support;

                // add 255Hz offsets if needed, use descriptor->rsvd2
                // to offset the min values their max MUST be offset as well
                if (version >= NVT_EDID_VER_1_4)
                {
                    if (descriptor->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_VER_MAX)
                    {
                        p->u.range_limit.max_v_rate += NVT_PVT_EDID_RANGE_OFFSET_AMOUNT;
                        if (descriptor->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_VER_MIN)
                        {
                            p->u.range_limit.min_v_rate += NVT_PVT_EDID_RANGE_OFFSET_AMOUNT;
                        }
                    }
                    if (descriptor->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_HOR_MAX)
                    {
                        p->u.range_limit.max_h_rate += NVT_PVT_EDID_RANGE_OFFSET_AMOUNT;
                        if (descriptor->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_HOR_MIN)
                        {
                            p->u.range_limit.min_h_rate += NVT_PVT_EDID_RANGE_OFFSET_AMOUNT;
                        }
                    }
                }

                if (p->u.range_limit.timing_support == NVT_EDID_RANGE_SUPPORT_GTF2)
                {
                    // descriptor->data[7]
                    // Start frequency for secondary curve, hor freq./2[kHz]
                    p->u.range_limit.u.gtf2.C     = pRangeLimit->u.gtf2.C / 2;       // 0 <= C <= 127
                    p->u.range_limit.u.gtf2.K     = pRangeLimit->u.gtf2.K;           // 0 <= K <= 255
                    p->u.range_limit.u.gtf2.J     = pRangeLimit->u.gtf2.J / 2;       // 0 <= J <= 127
                    p->u.range_limit.u.gtf2.M     = (pRangeLimit->u.gtf2.M_MSB << 8)
                                                  +  pRangeLimit->u.gtf2.M_LSB;      // 0 <= M <= 65535
                }
                else if (p->u.range_limit.timing_support == NVT_EDID_RANGE_SUPPORT_CVT)
                {
                    // the pixel clock adjustment is in cvt.pixel_clock @ bits7:2
                    // that number is in 0.25MHz, ie actual max clock is  max_pclk_MHz - (0.25 x cvt_pixel_clock)
                    // subtract the whole number part from max_pclk_MHz, save the remainder
                    p->u.range_limit.max_pclk_MHz -= (pRangeLimit->u.cvt.pixel_clock & NVT_PVT_EDID_CVT_PIXEL_CLOCK_MASK) >> NVT_PVT_EDID_CVT_PIXEL_CLOCK_SHIFT >> 2; // ie divide by 4 to get whole number
                    p->u.range_limit.u.cvt.pixel_clock_adjustment = ((pRangeLimit->u.cvt.pixel_clock & NVT_PVT_EDID_CVT_PIXEL_CLOCK_MASK) >> NVT_PVT_EDID_CVT_PIXEL_CLOCK_SHIFT) & 0x03; // ie modulus 4

                    p->u.range_limit.u.cvt.max_active_pixels_per_line = (pRangeLimit->u.cvt.pixel_clock & NVT_PVT_EDID_CVT_ACTIVE_MSB_MASK) << NVT_PVT_EDID_CVT_ACTIVE_MSB_SHIFT;
                    p->u.range_limit.u.cvt.max_active_pixels_per_line |= pRangeLimit->u.cvt.max_active;
                    p->u.range_limit.u.cvt.max_active_pixels_per_line <<= 3; // ie multiply 8

                    p->u.range_limit.u.cvt.aspect_supported = (pRangeLimit->u.cvt.aspect_supported & NVT_PVT_EDID_CVT_ASPECT_SUPPORTED_MASK) >> NVT_PVT_EDID_CVT_ASPECT_SUPPORTED_SHIFT;

                    p->u.range_limit.u.cvt.aspect_preferred = ( pRangeLimit->u.cvt.aspect_preferred_blanking & NVT_PVT_EDID_CVT_ASPECT_PREFERRED_MASK) >> NVT_PVT_EDID_CVT_ASPECT_PREFERRED_SHIFT;
                    p->u.range_limit.u.cvt.blanking_support = ( pRangeLimit->u.cvt.aspect_preferred_blanking & NVT_PVT_EDID_CVT_BLANKING_MASK) >> NVT_PVT_EDID_CVT_BLANKING_SHIFT;

                    p->u.range_limit.u.cvt.scaling_support  = (pRangeLimit->u.cvt.scaling_support & NVT_PVT_EDID_CVT_SCALING_MASK) >> NVT_PVT_EDID_CVT_SCALING_SHIFT;

                    p->u.range_limit.u.cvt.preferred_refresh_rate     = pRangeLimit->u.cvt.preferred_refresh_rate;
                }
            }

            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_CPD:     // color point data
            {
                EDID_COLOR_POINT_DATA *pColorPoint = (EDID_COLOR_POINT_DATA *)&descriptor->data[0];

                p->u.color_point.wp1_index = pColorPoint->wp1_index;
                p->u.color_point.wp1_x = pColorPoint->wp1_x << 2;
                p->u.color_point.wp1_x |= (pColorPoint->wp1_x_y & NVT_PVT_EDID_CPD_WP_X_MASK) >> NVT_PVT_EDID_CPD_WP_X_SHIFT;
                p->u.color_point.wp1_y = pColorPoint->wp1_y << 2;
                p->u.color_point.wp1_y |= (pColorPoint->wp1_x_y & NVT_PVT_EDID_CPD_WP_Y_MASK) >> NVT_PVT_EDID_CPD_WP_Y_SHIFT;
                p->u.color_point.wp1_gamma = pColorPoint->wp1_gamma + 100;

                p->u.color_point.wp2_index = pColorPoint->wp2_index;
                p->u.color_point.wp2_x = pColorPoint->wp2_x << 2;
                p->u.color_point.wp2_x |= (pColorPoint->wp2_x_y & NVT_PVT_EDID_CPD_WP_X_MASK) >> NVT_PVT_EDID_CPD_WP_X_SHIFT;
                p->u.color_point.wp2_y = pColorPoint->wp2_y << 2;
                p->u.color_point.wp2_y |= (pColorPoint->wp2_x_y & NVT_PVT_EDID_CPD_WP_Y_MASK) >> NVT_PVT_EDID_CPD_WP_Y_SHIFT;
                p->u.color_point.wp2_gamma = pColorPoint->wp2_gamma + 100;
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_STI:     // standard timing identification
            {
                EDID_STANDARD_TIMING_ID *pStdTiming = (EDID_STANDARD_TIMING_ID *)&descriptor->data[0];

                for(i=0; i<NVT_EDID_DD_STI_NUM; i++)
                {
                    p->u.std_timing.descriptor[i] = pStdTiming->std_timing[i];
                }
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_DCM:     // display color management
            {
                EDID_COLOR_MANAGEMENT_DATA *pColorMan = (EDID_COLOR_MANAGEMENT_DATA *)&descriptor->data[0];

                p->u.color_man.red_a3 = pColorMan->red_a3_lsb | (pColorMan->red_a3_msb << 8);
                p->u.color_man.red_a2 = pColorMan->red_a2_lsb | (pColorMan->red_a2_msb << 8);

                p->u.color_man.green_a3 = pColorMan->green_a3_lsb | (pColorMan->green_a3_msb << 8);
                p->u.color_man.green_a2 = pColorMan->green_a2_lsb | (pColorMan->green_a2_msb << 8);

                p->u.color_man.blue_a3 = pColorMan->blue_a3_lsb | (pColorMan->blue_a3_msb << 8);
                p->u.color_man.blue_a2 = pColorMan->blue_a2_lsb | (pColorMan->blue_a2_msb << 8);
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_CVT:     // CVT 3-byte timing code
            {
                EDID_CVT_3BYTE *pCVT_3byte = (EDID_CVT_3BYTE *)&descriptor->data[0];

                for (i=0; i<NVT_EDID_DD_MAX_CVT3_PER_DESCRITPOR; i++)
                {
                    if (pCVT_3byte->block[i].addressable_lines != 0)
                    {
                        p->u.cvt.block[i].addressable_lines = pCVT_3byte->block[i].addressable_lines;
                        p->u.cvt.block[i].addressable_lines |= (pCVT_3byte->block[i].lines_ratio & NVT_PVT_EDID_CVT3_LINES_MSB_MASK) << NVT_PVT_EDID_CVT3_LINES_MSB_SHIFT;
                        p->u.cvt.block[i].addressable_lines +=1;
                        p->u.cvt.block[i].addressable_lines <<= 1;

                        p->u.cvt.block[i].aspect_ratio = (pCVT_3byte->block[i].lines_ratio & NVT_PVT_EDID_CVT3_ASPECT_MASK) >> NVT_PVT_EDID_CVT3_ASPECT_SHIFT;

                        p->u.cvt.block[i].preferred_vert_rates = (pCVT_3byte->block[i].refresh_rates & NVT_PVT_EDID_CVT3_PREFERRED_RATE_MASK) >> NVT_PVT_EDID_CVT3_PREFERRED_RATE_SHIFT;
                        p->u.cvt.block[i].supported_vert_rates = (pCVT_3byte->block[i].refresh_rates & NVT_PVT_EDID_CVT3_SUPPORTED_RATE_MASK) >> NVT_PVT_EDID_CVT3_SUPPORTED_RATE_SHIFT;
                    }
                }
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_ESTIII:  // establishied timing III
            {
                EDID_EST_TIMINGS_III *pEstTiming = (EDID_EST_TIMINGS_III *)&descriptor->data[0];

                for(i=0; i<NVT_EDID_DD_EST_TIMING3_NUM; i++)
                {
                    p->u.est3.data[i] = pEstTiming->timing_byte[i];
                }
            }
            break;

        case NVT_EDID_DISPLAY_DESCRIPTOR_DUMMY:   // dummy descriptor
        default:
            // unresolved descriptor yet
            for (i = 0; i < NVT_PVT_EDID_LDD_PAYLOAD_SIZE; i++)
            {
                p->u.dummy.data[i] = descriptor->data[i];
            }
            break;
    }

}

// get generic EDID info
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NV_STDCALL NvTiming_ParseEDIDInfo(NvU8 *pEdid, NvU32 length, NVT_EDID_INFO *pInfo)
{
    NvU32 i, j, k, data;
    EDIDV1STRUC *p;
    NvU8 *pExt;
    NVT_EDID_CEA861_INFO *p861Info;

    // parameter check
    if (pEdid == NULL || length < 128 || pInfo == NULL)
    {
        return NVT_STATUS_ERR;
    }

    NVMISC_MEMSET(pInfo, 0, sizeof(NVT_EDID_INFO));

    // get the EDID version
    if (getEdidVersion(pEdid, &pInfo->version) == NVT_STATUS_ERR)
    {
        return NVT_STATUS_ERR;
    }

    p = (EDIDV1STRUC *) pEdid;

    // get the IDs
    pInfo->manuf_id = p->wIDManufName;
    pInfo->product_id = p->wIDProductCode;

    // translate the ID into manufacturer's name
    pInfo->manuf_name[0] = 'A' + (NvU8)((pInfo->manuf_id & 0x007c) >> 2) - 1;
    pInfo->manuf_name[1] = 'A' + (NvU8)((pInfo->manuf_id & 0x0003) << 3 | (pInfo->manuf_id & 0xe000) >> 13) - 1;
    pInfo->manuf_name[2] = 'A' + (NvU8)((pInfo->manuf_id & 0x1f00) >> 8) - 1;
    pInfo->manuf_name[3] = '\0';

    // get serial number
    pInfo->serial_number = p->dwIDSerialNumber;

    // get the week and year
    pInfo->week = p->bWeekManuf;
    pInfo->year = p->bYearManuf + 1990;

    // get the interface info
    pInfo->input.isDigital = (p->bVideoInputDef & NVT_PVT_EDID_INPUT_ISDIGITAL_MASK) >> NVT_PVT_EDID_INPUT_ISDIGITAL_SHIFT;

    if (pInfo->input.isDigital && pInfo->version > 0x103) // must be at least EDID1.4 to support the following fields
    {
        switch ( (p->bVideoInputDef & NVT_PVT_EDID_INPUT_BPC_MASK) >> NVT_PVT_EDID_INPUT_BPC_SHIFT)
        {
            case NVT_PVT_EDID_INPUT_BPC_6  :
                pInfo->input.u.digital.bpc = 6;
                break;
            case NVT_PVT_EDID_INPUT_BPC_8  :
                pInfo->input.u.digital.bpc = 8;
                break;
            case NVT_PVT_EDID_INPUT_BPC_10 :
                pInfo->input.u.digital.bpc = 10;
                break;
            case NVT_PVT_EDID_INPUT_BPC_12 :
                pInfo->input.u.digital.bpc = 12;
                break;
            case NVT_PVT_EDID_INPUT_BPC_14 :
                pInfo->input.u.digital.bpc = 14;
                break;
            case NVT_PVT_EDID_INPUT_BPC_16 :
                pInfo->input.u.digital.bpc = 16;
                break;
            default :
                pInfo->input.u.digital.bpc = 0;
                break;
        }
        pInfo->input.u.digital.video_interface = (p->bVideoInputDef & NVT_PVT_EDID_INPUT_INTERFACE_MASK) >> NVT_PVT_EDID_INPUT_INTERFACE_SHIFT;
    }
    else if (!pInfo->input.isDigital)
    {
        pInfo->input.u.analog_data = (p->bVideoInputDef & NVT_PVT_EDID_INPUT_ANALOG_ETC_MASK) >> NVT_PVT_EDID_INPUT_ANALOG_ETC_SHIFT;
    }

    // get the max image size and aspect ratio
    if (p->bMaxHorizImageSize != 0 && p->bMaxVertImageSize != 0)
    {
        pInfo->screen_size_x = p->bMaxHorizImageSize;
        pInfo->screen_size_y = p->bMaxVertImageSize;
        pInfo->screen_aspect_x = 0;
        pInfo->screen_aspect_y = 0;
    }
    else if (p->bMaxHorizImageSize != 0 && p->bMaxVertImageSize == 0)
    {
        pInfo->screen_size_x = 0;
        pInfo->screen_size_y = 0;
        pInfo->screen_aspect_x = 99 + p->bMaxHorizImageSize;
        pInfo->screen_aspect_y = 100;
    }
    else if (p->bMaxHorizImageSize == 0 && p->bMaxVertImageSize != 0)
    {
        pInfo->screen_size_x = 0;
        pInfo->screen_size_y = 0;
        pInfo->screen_aspect_x = 100;
        pInfo->screen_aspect_y = 99 + p->bMaxVertImageSize;
    }

    // get the gamma
    pInfo->gamma = p->bDisplayXferChar + 100;

    // get the features
    pInfo->u.feature = p->bFeatureSupport;

    // get chromaticity coordinates
    pInfo->cc_red_x =  p->Chromaticity[2] << 2;
    pInfo->cc_red_x |= (p->Chromaticity[0] & NVT_PVT_EDID_CC_RED_X1_X0_MASK) >> NVT_PVT_EDID_CC_RED_X1_X0_SHIFT;
    pInfo->cc_red_y =  p->Chromaticity[3] << 2;
    pInfo->cc_red_y |= (p->Chromaticity[0] & NVT_PVT_EDID_CC_RED_Y1_Y0_MASK) >> NVT_PVT_EDID_CC_RED_Y1_Y0_SHIFT;

    pInfo->cc_green_x =  p->Chromaticity[4] << 2;
    pInfo->cc_green_x |= (p->Chromaticity[0] & NVT_PVT_EDID_CC_GREEN_X1_X0_MASK) >> NVT_PVT_EDID_CC_GREEN_X1_X0_SHIFT;
    pInfo->cc_green_y =  p->Chromaticity[5] << 2;
    pInfo->cc_green_y |= (p->Chromaticity[0] & NVT_PVT_EDID_CC_GREEN_Y1_Y0_MASK) >> NVT_PVT_EDID_CC_GREEN_Y1_Y0_SHIFT;

    pInfo->cc_blue_x =  p->Chromaticity[6] << 2;
    pInfo->cc_blue_x |= (p->Chromaticity[1] & NVT_PVT_EDID_CC_BLUE_X1_X0_MASK) >> NVT_PVT_EDID_CC_BLUE_X1_X0_SHIFT;
    pInfo->cc_blue_y =  p->Chromaticity[7] << 2;
    pInfo->cc_blue_y |= (p->Chromaticity[1] & NVT_PVT_EDID_CC_BLUE_Y1_Y0_MASK) >> NVT_PVT_EDID_CC_BLUE_Y1_Y0_SHIFT;

    pInfo->cc_white_x =  p->Chromaticity[8] << 2;
    pInfo->cc_white_x |= (p->Chromaticity[1] & NVT_PVT_EDID_CC_WHITE_X1_X0_MASK) >> NVT_PVT_EDID_CC_WHITE_X1_X0_SHIFT;
    pInfo->cc_white_y =  p->Chromaticity[9] << 2;
    pInfo->cc_white_y |= (p->Chromaticity[1] & NVT_PVT_EDID_CC_WHITE_Y1_Y0_MASK) >> NVT_PVT_EDID_CC_WHITE_Y1_Y0_SHIFT;

    // copy established timings
    pInfo->established_timings_1_2  = (NvU16)p->bEstablishedTimings1 << 8;    
    pInfo->established_timings_1_2 |= (NvU16)p->bEstablishedTimings2;

    // copy manuf reserved timings
    pInfo->manufReservedTimings = p->bManufReservedTimings;

    // copy standard timings
    for (i = 0; i < NVT_EDID_MAX_STANDARD_TIMINGS; i++)
    {
        pInfo->standard_timings[i] = p->wStandardTimingID[i];
    }

    // get the number of extensions
    pInfo->total_extensions = p->bExtensionFlag;

    // check_sum
    for (i = 0, data = 0; i < length; i++)
    {
        data += pEdid[i];
    }
    pInfo->checksum_ok = !(data & 0xFF);
    pInfo->checksum = p->bChecksum;


    // now find out the total number of all of the timings in the EDID
    pInfo->total_timings = 0;

    // now find out the detailed timings
    parseEdidDetailedTiming(pEdid, pInfo);

    // now parse all 18-byte long display descriptors (not detailed timing)
    for (i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        parseEdidLongDisplayDescriptor((EDID_LONG_DISPLAY_DESCRIPTOR *)&p->DetailedTimingDesc[i], &pInfo->ldd[i], pInfo->version);
    }

    // now check the number of timings in the extension
    for (k = 0, j = 1; j <= pInfo->total_extensions && (j + 1) * sizeof(EDIDV1STRUC) <= length; j++)
    {
        pExt = pEdid + sizeof(EDIDV1STRUC) * j;

        // check for 861 extension first
        switch (*pExt)
        {
            case NVT_EDID_EXTENSION_CTA:
                p861Info = (k == 0) ? &pInfo->ext861 : &pInfo->ext861_2;

                get861ExtInfo(pExt, sizeof(EDIDV1STRUC), p861Info);
                
                // HF EEODB is present in edid v1.3 and v1.4 does not need this.Also, it is always present in the 1st CTA extension block.
                if (j == 1 && pInfo->version == NVT_EDID_VER_1_3)
                {
                    parseCta861HfEeodb(p861Info, &pInfo->total_extensions);
                }

                // update pInfo with basic hdmi info
                // assumes each edid will only have one such block across multiple cta861 blocks (otherwise may create declaration conflict)
                // In case of multiple such blocks, the last one takes precedence, except for SCDB

                // parseCta861VsdbBlocks() uses hfScdb info so need to be parsed first
                parseCta861HfScdb(p861Info, pInfo, FROM_CTA861_EXTENSION);
                parseCta861VsdbBlocks(p861Info, pInfo, FROM_CTA861_EXTENSION);

                // parse HDR related information from the HDR static metadata data block
                parseCea861HdrStaticMetadataDataBlock(p861Info, pInfo, FROM_CTA861_EXTENSION);

                // parse Dolby Vision related information from the DV vendor specific video data block
                parseCea861DvStaticMetadataDataBlock(p861Info, &pInfo->dv_static_metadata_info);

                // Timings are listed (or shall) be listed in priority order
                // So read SVD, yuv420 SVDs first before reading detailed timings

                // add the 861B short video timing descriptor
                if (p861Info->revision >= NVT_CEA861_REV_B)
                {
                    // base video
                    parse861bShortTiming(p861Info, pInfo, FROM_CTA861_EXTENSION);

                    // yuv420-only video
                    parse861bShortYuv420Timing(p861Info, pInfo, FROM_CTA861_EXTENSION);
                }

                // add the detailed timings in 18-byte long display descriptor
                parse861ExtDetailedTiming(pExt, p861Info->basic_caps, pInfo);

                // CEA861-F at 7.5.12 section about VFPDB block.
                if (p861Info->revision >= NVT_CEA861_REV_F && p861Info->total_vfpdb != 0)
                {
                    parse861bShortPreferredTiming(p861Info, pInfo, FROM_CTA861_EXTENSION);
                }

                k++;
                break;

            case NVT_EDID_EXTENSION_VTB:
                parseVTBExtension(pExt, pInfo);
                break;

            case NVT_EDID_EXTENSION_DISPLAYID:
                if ((pExt[1] & 0xF0) == 0x20) // displayID2.x as EDID extension
                {
                    if(getDisplayId20EDIDExtInfo(pExt, sizeof(EDIDV1STRUC),
                                                    pInfo) == NVT_STATUS_SUCCESS)
                    {
                        if (pInfo->ext861.total_y420vdb != 0 || pInfo->ext861.total_y420cmdb != 0)
                        {
                            pInfo->ext_displayid20.interface_features.yuv420_min_pclk = 0;
                        }

                        if (!pInfo->ext861.basic_caps)
                        {
                            pInfo->ext861.basic_caps = pInfo->ext_displayid20.basic_caps;
                        }
                    }
                }
                else // displayID13 as EDID extension
                {
                    //do not fail function based on return value of getDisplayIdEDIDExtInfo refer bug 3247180 where some rogue monitors don't provide correct DID13 raw data.
                    if (getDisplayIdEDIDExtInfo(pExt, sizeof(EDIDV1STRUC),
                                                   pInfo) == NVT_STATUS_SUCCESS)
                    {
                        // Check if YCbCr is supported in base block
                        // since it is mandatory if YCbCr is supported on any other display interface as per 5.1.1.1 Video Colorimetry
                        if(pInfo->u.feature_ver_1_4_digital.support_ycrcb_444)
                        {
                            if (!pInfo->ext_displayid.supported_displayId2_0)
                            {
                                pInfo->ext_displayid.u4.display_interface.ycbcr444_depth.support_8b = 1;
                            }
                            else
                            {
                                pInfo->ext_displayid.u4.display_interface_features.ycbcr444_depth.support_8b = 1;
                            }
                        }

                        if(pInfo->u.feature_ver_1_4_digital.support_ycrcb_422)
                        {
                            if (!pInfo->ext_displayid.supported_displayId2_0)
                            {
                                pInfo->ext_displayid.u4.display_interface.ycbcr422_depth.support_8b = 1;
                            }
                            else
                            {
                                pInfo->ext_displayid.u4.display_interface_features.ycbcr422_depth.support_8b = 1;
                            }
                        }
                    }
                }
                break;

            default:
                break;
        }
    }

    // Copy all the timings(could include type 7/8/9/10) from displayid20->timings[] to pEdidInfo->timings[] 
    for (i = 0; i < pInfo->ext_displayid20.total_timings; i++)
    {
        if (!assignNextAvailableTiming(pInfo, &(pInfo->ext_displayid20.timing[i])))
        {
            return NVT_STATUS_ERR;
        }
    }

    // check for cvt timings - in display range limits or cvt 3-byte LDD, only for EDID1.4 and above
    if (pInfo->version > 0x0103)
    {
        parseEdidCvtTiming(pInfo);
    }

    // now check for standard timings - base EDID and then the LDDs
    parseEdidStandardTiming(pInfo);

    // find out the total established timings - base EDID and then the LDDs
    parseEdidEstablishedTiming(pInfo);

    getEdidHDM1_4bVsdbTiming(pInfo);

    // Assert if no timings were found (due to a bad EDID) or if we mistakenly
    // assigned more timings than we allocated space for (due to bad logic above)
    nvt_assert(pInfo->total_timings &&
               (pInfo->total_timings <= COUNT(pInfo->timing)));

    // go through all timings and update supported color formats
    // consider the supported bpc per color format from parsed EDID / CTA861 / DisplayId
    updateColorFormatAndBpcTiming(pInfo);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
void updateColorFormatAndBpcTiming(NVT_EDID_INFO *pInfo)
{    
    NvU32 i, j, data;

    for (i = 0; i < pInfo->total_timings; i++)
    {
        data = NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status);
        switch (data)
        {
        case NVT_TYPE_HDMI_STEREO:
        case NVT_TYPE_HDMI_EXT:
            // VTB timing use the base EDID (block 0) to determine the color format support
        case NVT_TYPE_EDID_VTB_EXT:     
        case NVT_TYPE_EDID_VTB_EXT_STD: 
        case NVT_TYPE_EDID_VTB_EXT_DTD:
        case NVT_TYPE_EDID_VTB_EXT_CVT:
            // pInfo->u.feature_ver_1_3.color_type provides mono, rgb, rgy, undefined
            // assume RGB 8-bpc support only (VTB is pretty old edid standard)
            pInfo->timing[i].etc.rgb444.bpc.bpc8 = 1;
            break;
            // These are from the CTA block, and relies on
            // Since there could be multiple CEA blocks, these are adjusted when the blocks are parsed
        case NVT_TYPE_EDID_861ST:
        case NVT_TYPE_EDID_EXT_DTD:
            if (pInfo->ext_displayid20.as_edid_extension &&
                pInfo->ext_displayid20.valid_data_blocks.cta_data_present)
            {
                updateColorFormatForDisplayId20ExtnTimings(pInfo, i);
            }
            updateBpcForTiming(pInfo, i);
            break;
        default: 
            // * the displayID_v1.3/v2.0 EDID extension need to follow the EDID bpc definition.
            // * all other default to base edid
            updateBpcForTiming(pInfo, i);
        }

        // The timings[i] entries need to update the bpc values where are based on the different color format again
        // if displayId extension existed it's interface feature data block
        if (pInfo->ext_displayid.version == 0x12 || pInfo->ext_displayid.version == 0x13)
        {
            updateColorFormatForDisplayIdExtnTimings(pInfo, i);
        }
        else if (pInfo->ext_displayid20.valid_data_blocks.interface_feature_present)
        {
            // DisplayId2.0 spec has its own way of determining color format support which includes bpc + color format
            updateColorFormatForDisplayId20ExtnTimings(pInfo, i);
        }
    }

    // Go through all the timings and set CTA format accordingly. If a timing is a CTA 861b timing, store the
    // index of this CTA 861b standard in NVT_TIMING.etc.status field.
    // However parser needs to exclude the DTD timing in EDID base block where is shared same detailed timing in VIC/DTD_ext in CTA861
    for (i = 0; i < pInfo->total_timings; i++)
    {
        data = NvTiming_GetCEA861TimingIndex(&pInfo->timing[i]);
        // DisplayID block did not belong to CTA timing and it owned the deep color block itself
        if (data && !((NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_1) ||
                      (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_2) ||
                      (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_7) ||
                      (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_8) ||
                      (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_9) ||
                      (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_DISPLAYID_10)))
        {
            // CEA timings may be enumerated outside of SVD blocks -- the formats of these timings don't have CEA FORMAT (vic) set
            // before marking them CEA, make sure their color formats are updated too
            if (NVT_GET_CEA_FORMAT(pInfo->timing[i].etc.status) == 0 &&
                (!NVT_IS_DTD(pInfo->timing[i].etc.status) ||
                 isMatchedCTA861Timing(pInfo, &pInfo->timing[i])))
            {
                for (j = 0; j < pInfo->total_timings; j++)
                {
                    // It is assumed CTA timings that are repeated by the CTA block or different CTA blocks will
                    // announce the same color format for the same CTA timings
                    if (NVT_GET_CEA_FORMAT(pInfo->timing[j].etc.status) == data)
                    {
                        // There could be anomalies between EDID 1.4 base block color format vs CEA861 basic caps
                        // In this case we assume the union is supported
                        pInfo->timing[i].etc.rgb444.bpcs |= pInfo->timing[j].etc.rgb444.bpcs;
                        pInfo->timing[i].etc.yuv444.bpcs |= pInfo->timing[j].etc.yuv444.bpcs;
                        pInfo->timing[i].etc.yuv422.bpcs |= pInfo->timing[j].etc.yuv422.bpcs;
                        pInfo->timing[i].etc.yuv420.bpcs |= pInfo->timing[j].etc.yuv420.bpcs;
                        break;
                    }
                }

                // now update the VIC of this timing
                NVT_SET_CEA_FORMAT(pInfo->timing[i].etc.status, data);
            }
            // see the aspect ratio info if needed
            if (pInfo->timing[i].etc.aspect == 0)
            {
                pInfo->timing[i].etc.aspect = getCEA861TimingAspectRatio(data);
            }
        }
    }

}

CODE_SEGMENT(PAGE_DD_CODE)
NvBool isMatchedCTA861Timing(NVT_EDID_INFO *pInfo, NVT_TIMING *pT)
{
    NvU32 j;

    for (j = 0; j < pInfo->total_timings; j++)
    {
        if (NVT_GET_CEA_FORMAT(pInfo->timing[j].etc.status) && NvTiming_IsTimingExactEqual(&pInfo->timing[j], pT))
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

CODE_SEGMENT(PAGE_DD_CODE)
void updateBpcForTiming(NVT_EDID_INFO *pInfo, NvU32 index)
{
    NVT_EDID_CEA861_INFO *p861Info;

    // assume/prefer data from 1st CEA block if multiple exist
    p861Info = &pInfo->ext861;

    pInfo->timing[index].etc.rgb444.bpc.bpc8 = 1;

    if (pInfo->version >= NVT_EDID_VER_1_4 && pInfo->input.isDigital)
    {
        if (pInfo->u.feature_ver_1_4_digital.support_ycrcb_444)
        {
            pInfo->timing[index].etc.yuv444.bpc.bpc8 = 1;
        }
        if (pInfo->u.feature_ver_1_4_digital.support_ycrcb_422)
        {
            pInfo->timing[index].etc.yuv422.bpc.bpc8 = 1;
        }
        if (pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_DISPLAYPORT_SUPPORTED ||
            pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_UNDEFINED)
        {
            pInfo->timing[index].etc.rgb444.bpc.bpc6 = 1;

            // trust bpc claim in edid base block for DP only
            if (pInfo->input.u.digital.bpc >= NVT_EDID_VIDEOSIGNAL_BPC_10)
            {
                pInfo->timing[index].etc.rgb444.bpc.bpc10 = 1;
                pInfo->timing[index].etc.yuv444.bpc.bpc10 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_444 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_444);
                pInfo->timing[index].etc.yuv422.bpc.bpc10 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_422 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_422);
            }
            if (pInfo->input.u.digital.bpc >= NVT_EDID_VIDEOSIGNAL_BPC_12)
            {
                pInfo->timing[index].etc.rgb444.bpc.bpc12 = 1;
                pInfo->timing[index].etc.yuv444.bpc.bpc12 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_444 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_444);
                pInfo->timing[index].etc.yuv422.bpc.bpc12 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_422 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_422);
            }
            if (pInfo->input.u.digital.bpc >= NVT_EDID_VIDEOSIGNAL_BPC_16)
            {
                pInfo->timing[index].etc.rgb444.bpc.bpc16 = 1;
                pInfo->timing[index].etc.yuv444.bpc.bpc16 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_444 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_444);
                pInfo->timing[index].etc.yuv422.bpc.bpc16 = pInfo->u.feature_ver_1_4_digital.support_ycrcb_422 || (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_422);
            }
        }
        else if ((pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_A_SUPPORTED ||
                  pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_B_SUPPORTED || 
                  pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_UNDEFINED) &&
                 p861Info->revision >= NVT_CEA861_REV_A)
        {
            updateHDMILLCDeepColorForTiming(pInfo, index);
        }
    }
    else if (p861Info->revision >= NVT_CEA861_REV_A)
    {
        updateHDMILLCDeepColorForTiming(pInfo, index);
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_Get18ByteLongDescriptorIndex(NVT_EDID_INFO *pEdidInfo, NvU8 tag, NvU32 *pDtdIndex)
{
    NvU32 dtdIndex;

    if (!pEdidInfo || !pDtdIndex)
    {
        return NVT_STATUS_ERR;
    }

    for (dtdIndex = *pDtdIndex; dtdIndex < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; dtdIndex++)
    {
        if (pEdidInfo->ldd[dtdIndex].tag == tag)
        {
            *pDtdIndex = dtdIndex;
            return NVT_STATUS_SUCCESS;
        }
    }

    return NVT_STATUS_ERR;
}

// get the edid timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetEdidTimingEx(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_TIMING *pT, NvU32 rrx1k)
{
    NvU32 i, j;
    NvU32 preferred_cea, preferred_displayid_dtd, preferred_dtd1, dtd1, map0, map1, map2, map3, map4, ceaIndex, max, cvt;
    NVT_TIMING *pEdidTiming;
    NVT_EDID_DD_RANGE_CVT *pCVT = NULL;
    NVT_TIMING cvtTiming;

    // input check
    if (pEdidInfo == NULL || pEdidInfo->total_timings == 0 || pT == 0)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0) // rrx1k is optional, can be 0.
        return NVT_STATUS_ERR;

    pEdidTiming = pEdidInfo->timing;

    // the timing mapping index :
    //
    // preferred_cea           - the "prefer SVD" in CEA-861-F (i.e. A Sink that prefers a Video Format that is not listed as an SVD in Video Data Block, but instead listed in YCBCR 4:2:0 VDB)
    // preferred_displayid_dtd - the "prefer detailed timing of DispalyID" extension
    // preferred_dtd1          - the first deatiled timing and PTM flag is enable
    // dtd1 - the first detailed timing
    // map0 - the "perfect" match (the timing's H/V-visible and pixel clock(refresh rate) are the same as the asking "width", "height" and "rr".
    // map1 - the "closest" match with the honor of the interlaced flag
    // map2 - the "closest" match without the honor of the interlaced flag
    // map3 - the "closest" match to the panel's native timing (i.e. the first DTD timing or the short 861B/C/D timings with "native" flag).
    // map4 - the "closest" match with the same refresh rate
    // max  - the timing with the max visible area
    preferred_cea = preferred_displayid_dtd = preferred_dtd1 = dtd1 = map0 = map1 = map2 = map3 = map4 = ceaIndex = pEdidInfo->total_timings;
    max = cvt = 0;
    for (i = 0; i < pEdidInfo->total_timings; i++)
    {
        // if the client prefers _NATIVE timing, then don't select custom timing
        if ((flag & (NVT_FLAG_NATIVE_TIMING | NVT_FLAG_EDID_TIMING)) != 0 && NVT_IS_CUST_ENTRY(pEdidTiming[i].etc.status) != 0)
        {
            continue;
        }

        // find the perfect match is possible
        if ((flag & NVT_FLAG_MAX_EDID_TIMING) == 0 &&
            width  == pEdidTiming[i].HVisible &&
            height == frame_height(pEdidTiming[i]) &&
            rr     == pEdidTiming[i].etc.rr   &&
            ((rrx1k == 0) || (rrx1k == pEdidTiming[i].etc.rrx1k)) &&
            !!(flag & NVT_PVT_INTERLACED_MASK) == !!pEdidTiming[i].interlaced)
        {
            if (map0 >= pEdidInfo->total_timings)
            {
                // make sure we take the priority as "detailed>standard>established". (The array timing[] always have the detailed timings in the front and then the standard and established.)
                map0 = i;
            }

            if ( (NVT_PREFERRED_TIMING_IS_CEA(pEdidTiming[i].etc.flag)) ||
                 ((0 == (flag & NVT_FLAG_EDID_861_ST)) && NVT_PREFERRED_TIMING_IS_DTD1(pEdidTiming[i].etc.flag, pEdidTiming[i].etc.status)) ||
                 (NVT_PREFERRED_TIMING_IS_DISPLAYID(pEdidTiming[i].etc.flag)) ||
                 (NVT_IS_NATIVE_TIMING(pEdidTiming[i].etc.status)))
            {
                *pT = pEdidTiming[i];
                return NVT_STATUS_SUCCESS;
            }

            if (NVT_GET_TIMING_STATUS_TYPE(pEdidTiming[i].etc.status) == NVT_TYPE_EDID_861ST)
            {
                if (ceaIndex == pEdidInfo->total_timings)
                {
                    // Save the first entry found.
                    ceaIndex = i;
                }
                else
                {
                    if (((flag & NVT_FLAG_CEA_4X3_TIMING) && (pEdidTiming[i].etc.aspect == 0x40003)) ||
                        ((flag & NVT_FLAG_CEA_16X9_TIMING) && (pEdidTiming[i].etc.aspect == 0x160009)))
                    {
                        // Use preferred aspect ratio if specified.
                        ceaIndex = i;
                    }
                }
            }
        } // if ((flag & NVT_FLAG_MAX_EDID_TIMING) == 0 &&

        // bypass the custom timing to be select for the mismatch case
        if (NVT_GET_TIMING_STATUS_TYPE(pEdidTiming[i].etc.status) == NVT_TYPE_CUST ||
            NVT_IS_CUST_ENTRY(pEdidTiming[i].etc.status) != 0)
        {
            if (width != pEdidTiming[i].HVisible || height != frame_height(pEdidTiming[i]) || rr != pEdidTiming[i].etc.rr)
            {
                continue;
            }
        }

        // find out the preferred timing just in case of cea_vfpdb is existed
        if (preferred_cea == pEdidInfo->total_timings &&
            NVT_PREFERRED_TIMING_IS_CEA(pEdidTiming[i].etc.flag))
        {
            preferred_cea = i;
        }

        // find out the preferred timing just in case
        // Caller we will force rr value as 1 to select the DisplayID prefer timing in pEdidTiming if it existed
        // however, we can't assign the correct refresh rate we want if we had two and above rr values which shared the same timing.
        if (rr != 1)
        {
            if (pEdidTiming[i].etc.rr == rr && NVT_PREFERRED_TIMING_IS_DISPLAYID(pEdidTiming[i].etc.flag))
            {
                preferred_displayid_dtd = i;
            }
        }
        else if (preferred_displayid_dtd == pEdidInfo->total_timings &&
                NVT_PREFERRED_TIMING_IS_DISPLAYID(pEdidTiming[i].etc.flag))
        {
                preferred_displayid_dtd = i;
        }

        if (NVT_PREFERRED_TIMING_IS_DTD1(pEdidTiming[i].etc.flag, pEdidTiming[i].etc.status))
        {
            preferred_dtd1 = i;
        }

        if (NVT_IS_DTD1(pEdidTiming[i].etc.status))
        {
            dtd1 = i;
        }

        // find out the max mode just in case
        if (pEdidTiming[i].HVisible * pEdidTiming[i].VVisible > pEdidTiming[max].HVisible * pEdidTiming[max].VVisible)
            max = i;

        // if the requested timing is not in the EDID, try to find out the EDID entry with the same progressive/interlaced setting
        if (map1 >= pEdidInfo->total_timings)
        {
            if (!!(flag & NVT_PVT_INTERLACED_MASK) == !!pEdidTiming[i].interlaced &&
                width  <= pEdidTiming[i].HVisible &&
                height <= frame_height(pEdidTiming[i]))
            {
                map1 = i;
            }
        }
        else
        {
            if (!!(flag & NVT_PVT_INTERLACED_MASK) == !!pEdidTiming[i].interlaced &&
                width  <= pEdidTiming[i].HVisible &&
                height <= frame_height(pEdidTiming[i]) &&
                abs_delta(pEdidTiming[i].HVisible, width) <= abs_delta(pEdidTiming[map1].HVisible, width) &&
                abs_delta(frame_height(pEdidTiming[i]), height) <= abs_delta(frame_height(pEdidTiming[map1]), height))
            {
                // if there're 2 timings with the same visible size, choose the one with closer refresh rate
                if (pEdidTiming[i].HVisible == pEdidTiming[map1].HVisible &&
                    frame_height(pEdidTiming[i]) == frame_height(pEdidTiming[map1]))
                {
                    if (abs_delta(pEdidTiming[i].etc.rr, rr) < abs_delta(pEdidTiming[map1].etc.rr, rr))
                    {
                        map1 = i;
                    }
                }
                else
                {
                    map1 = i;
                }
            }
        }

        // if the requested timing is not in the EDID, try to find out the EDID entry without the progressive/interlaced setting
        if (map2 >= pEdidInfo->total_timings)
        {
            if (width  <= pEdidTiming[i].HVisible &&
                height <= frame_height(pEdidTiming[i]))
            {
                map2 = i;
            }
        }
        else
        {
            if (width  <= pEdidTiming[i].HVisible &&
                height <= frame_height(pEdidTiming[i]) &&
                abs_delta(pEdidTiming[i].HVisible, width) <= abs_delta(pEdidTiming[map2].HVisible, width) &&
                abs_delta(frame_height(pEdidTiming[i]), height) <= abs_delta(frame_height(pEdidTiming[map2]), height))
            {
                // if there're 2 timings with the same visible size, choose the one with closer refresh rate
                if (pEdidTiming[i].HVisible == pEdidTiming[map2].HVisible &&
                    frame_height(pEdidTiming[i]) == frame_height(pEdidTiming[map2]))
                {
                    if (abs_delta(pEdidTiming[i].etc.rr, rr) < abs_delta(pEdidTiming[map2].etc.rr, rr))
                    {
                        map2 = i;
                    }
                }
                else
                {
                    map2 = i;
                }
            }
        }

        // find out the native timing
        if (NVT_IS_NATIVE_TIMING(pEdidTiming[i].etc.status) || NVT_IS_DTD1(pEdidTiming[i].etc.status))
        {
            if (map3 >= pEdidInfo->total_timings)
            {
                if (width  <= pEdidTiming[i].HVisible &&
                    height <= frame_height(pEdidTiming[i]))
                {
                    map3 = i;
                }
            }
            else if(abs_delta(pEdidTiming[i].HVisible, width) <= abs_delta(pEdidTiming[map3].HVisible, width) &&
                abs_delta(frame_height(pEdidTiming[i]), height) <= abs_delta(frame_height(pEdidTiming[map3]), height) &&
                width  <= pEdidTiming[i].HVisible &&
                height <= frame_height(pEdidTiming[i]))
            {
                map3 = i;
            }
        }

        // find the edid timing with refresh rate matching
        if (map4 >= pEdidInfo->total_timings)
        {
            if (width  <= pEdidTiming[i].HVisible &&
                height <= pEdidTiming[i].VVisible &&
                rr == pEdidTiming[i].etc.rr)
            {
                map4 = i;
            }
        }
        else
        {
            if (width  <= pEdidTiming[i].HVisible &&
                height <= pEdidTiming[i].HVisible &&
                rr     == pEdidTiming[i].etc.rr   &&
                abs_delta(pEdidTiming[i].HVisible, width)  <= abs_delta(pEdidTiming[map4].HVisible, width) &&
                abs_delta(pEdidTiming[i].VVisible, height) <= abs_delta(pEdidTiming[map4].VVisible, height))
            {
                map4 = i;
            }
        }

    }//for (i = 0; i < pEdidInfo->total_timings; i++)

    if ( (preferred_displayid_dtd == preferred_dtd1) && (preferred_dtd1 == dtd1) &&
         (dtd1 == map0) &&
         (map0 == map1) &&
         (map1 == map2) &&
         (map2 == map3) &&
         (map3 == map4) &&
         (map4 == pEdidInfo->total_timings) &&
         pEdidInfo->version >= NVT_EDID_VER_1_4 &&
         pEdidInfo->u.feature_ver_1_4_digital.continuous_frequency &&
         !(flag & NVT_PVT_INTERLACED_MASK))
    {
        // try to find CVT timing that fits
        NvU32 maxHeight, minHeight, tempHeight;

        minHeight = ~0;
        maxHeight = tempHeight= 0;

        // looping through long display descriptors
        for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
        {
            if (pEdidInfo->ldd[i].tag != NVT_EDID_DISPLAY_DESCRIPTOR_DRL || pEdidInfo->ldd[i].u.range_limit.timing_support != NVT_EDID_RANGE_SUPPORT_CVT)
            {
                continue;
            }

            pCVT = &pEdidInfo->ldd[i].u.range_limit.u.cvt;

            if (width <= pCVT->max_active_pixels_per_line || (pCVT->scaling_support & NVT_EDID_CVT_SCALING_HOR_SHRINK))
            {
                for (j=0; j<NVT_EDID_CVT_ASPECT_SUPPORT_MAX && !cvt; j++)
                {
                    if ( !(pCVT->aspect_supported & (1<<j)))
                    {
                        continue;
                    }

                    switch (1<<j)
                    {
                        case NVT_EDID_CVT_ASPECT_SUPPORT_4X3:
                            tempHeight = axb_div_c(width, 3, 4);
                            if (axb_div_c(width, 3, height) ==  4) cvt = 1;
                            break;
                        case NVT_EDID_CVT_ASPECT_SUPPORT_16X9:
                            tempHeight = axb_div_c(width, 9, 16);
                            if (axb_div_c(width, 9, height) == 16) cvt = 1;
                            break;
                        case NVT_EDID_CVT_ASPECT_SUPPORT_16X10:
                            tempHeight = axb_div_c(width, 10, 16);
                            if (axb_div_c(width,10, height) == 16) cvt = 1;
                            break;
                        case NVT_EDID_CVT_ASPECT_SUPPORT_5X4:
                            tempHeight = axb_div_c(width, 4, 5);
                            if (axb_div_c(width, 4, height) ==  5) cvt = 1;
                            break;
                        case NVT_EDID_CVT_ASPECT_SUPPORT_15X9:
                            tempHeight = axb_div_c(width, 9, 15);
                            if (axb_div_c(width, 9, height) == 15) cvt = 1;
                            break;
                    }

                    //keep track of max and min in case NVT_EDID_CVT_SCALING_VER_STRETCH/SHRINK are true
                    if (minHeight > tempHeight)
                    {
                        minHeight = tempHeight;
                    }
                    if (maxHeight < tempHeight)
                    {
                        maxHeight = tempHeight;
                    }

                }//for (j=0; j<5; j++)
            }//if (width <= pCVT->max_active_pixels_per_line || (pCVT->scaling_support & NVT_EDID_CVT_SCALING_HOR_STRETCH))

            if ( ((minHeight < height) && (pCVT->scaling_support & NVT_EDID_CVT_SCALING_VER_SHRINK)) ||
                 ((maxHeight > height) && (pCVT->scaling_support & NVT_EDID_CVT_SCALING_VER_STRETCH)) )
            {
                cvt = 1;
            }

            if (cvt)
            {
                break;
            }
        }//for (i=0; i<NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)

        if (cvt)
        {
            //calculate the CVT timing
            // pclk is in 10KHz, max_pclk_MHz is in MHz, pixel_clock_adjustment is in .25MHz - make sure timing pclk is inside the max pclk declared in the EDID
            if (pCVT->blanking_support & NVT_EDID_CVT_BLANKING_REDUCED && NvTiming_CalcCVT_RB(width, height, rr, NVT_PROGRESSIVE, &cvtTiming) == NVT_STATUS_SUCCESS)
            {
                if ( cvtTiming.pclk > (NvU32)((pEdidInfo->ldd[i].u.range_limit.max_pclk_MHz * 100) - (pCVT->pixel_clock_adjustment * 25)) )
                {
                    cvt = 0;
                }
            }
            else if (pCVT->blanking_support & NVT_EDID_CVT_BLANKING_STANDARD && NvTiming_CalcCVT(width, height, rr, NVT_PROGRESSIVE, &cvtTiming) == NVT_STATUS_SUCCESS)
            {
                if ( cvtTiming.pclk > (NvU32)((pEdidInfo->ldd[i].u.range_limit.max_pclk_MHz * 100) - (pCVT->pixel_clock_adjustment * 25)) )
                {
                    cvt = 0;
                }
            }
            else
            {
                cvt = 0;
            }

        }
    }//(dtd1 == map0 == map1 == map2 == map3 == pEdidInfo->total_timings) && pEdidInfo->version >= NVT_EDID_VER_1_4 &&
     //    pEdidInfo->feature_ver_1_4_digital.continuous_frequency && !(flag & NVT_PVT_INTERLACED_MASK))

    // now return the mismatched EDID timing
    if (flag & NVT_FLAG_NV_PREFERRED_TIMING)
    {
        *pT = (preferred_displayid_dtd != pEdidInfo->total_timings) ? pEdidTiming[preferred_displayid_dtd] :
              (preferred_cea != pEdidInfo->total_timings)           ? pEdidTiming[preferred_cea]           :
              (preferred_dtd1 != pEdidInfo->total_timings)          ? pEdidTiming[preferred_dtd1]          :
                                                                      pEdidTiming[dtd1];
        // what if DTD1 itself is filtered out, in such case dtd1 index points to an invalid timing[]?
        // (dtd1 != pEdidInfo->total_timings) ? pEdidTiming[dtd1] : pEdidTiming[0];
    }
    else if (flag & NVT_FLAG_DTD1_TIMING)
    {
        *pT = pEdidTiming[dtd1];
    }
    else if ((flag & NVT_FLAG_MAX_EDID_TIMING) && (0 == (flag & NVT_FLAG_EDID_861_ST)))
    {
        *pT = pEdidTiming[max];
    }
    else if ((flag & (NVT_FLAG_CEA_4X3_TIMING | NVT_FLAG_CEA_16X9_TIMING | NVT_FLAG_EDID_861_ST)) && ceaIndex < (pEdidInfo->total_timings))
    {
        *pT = pEdidTiming[ceaIndex];
    }
    else if ((flag & NVT_FLAG_NATIVE_TIMING) != 0 && map3 < pEdidInfo->total_timings)
    {
        // Allow closest refresh rate match when EDID has detailed timing for different RR on native resolution.
        if (map0 < pEdidInfo->total_timings &&
            pEdidTiming[map0].HVisible == pEdidTiming[map3].HVisible &&
            pEdidTiming[map0].VVisible == pEdidTiming[map3].VVisible)
        {
            *pT = pEdidTiming[map0];
        }
        else
        {
            *pT = pEdidTiming[map3];
        }
    }
    else if (map0 < pEdidInfo->total_timings)
    {
        // use the exact mapped timing if possible
        *pT = pEdidTiming[map0];
    }
    else if ((flag & NVT_FLAG_EDID_TIMING_RR_MATCH) && map4 < pEdidInfo->total_timings)
    {
        *pT = pEdidTiming[map4];
    }
    else if (map1 < pEdidInfo->total_timings)
    {
        // use the mapped timing if possible
        *pT = pEdidTiming[map1];
    }
    else if (map2 < pEdidInfo->total_timings)
    {
        // use the 2nd mapped timing if possible
        *pT = pEdidTiming[map2];
    }
    else if (dtd1 < pEdidInfo->total_timings && width <= pEdidTiming[dtd1].HVisible && height <= pEdidTiming[dtd1].VVisible)
    {
        // use the 1st detailed timing if possible
        *pT = pEdidTiming[dtd1];
    }
    else if (cvt)
    {
        // use the cvt timing
        *pT = cvtTiming;
    }
    else
    {
        // use the max timing for all other cases
        *pT = pEdidTiming[max];
    }

    // set the mismatch status
    if (pT->HVisible != width || frame_height(*pT) != height)
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_SIZE);
    }
    if (!NvTiming_IsRoundedRREqual(pT->etc.rr, pT->etc.rrx1k, (NvU16)rr))
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_RR);
    }
    if (!!pT->interlaced != !!(flag & NVT_PVT_INTERLACED_MASK))
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_FORMAT);
    }

    return NVT_STATUS_SUCCESS;
}

// get the edid timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetEdidTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_TIMING *pT)
{
    return NvTiming_GetEdidTimingEx(width, height, rr, flag, pEdidInfo, pT, 0);
}
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetHDMIStereoExtTimingFromEDID(NvU32 width, NvU32 height, NvU32 rr, NvU8 StereoStructureType, NvU8 SideBySideHalfDetail, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_EXT_TIMING *pT)
{
    NVT_STATUS  status = NVT_STATUS_ERR;
    NvU8        Vic;
    NvU32         i;
    NVT_TIMING  Timing;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_EXT_TIMING));

    // adjust the flags --
    // need EDID timing with RR match,
    // not max timing,
    flag = flag | NVT_FLAG_EDID_TIMING | NVT_FLAG_EDID_TIMING_RR_MATCH | NVT_FLAG_EDID_861_ST;
    flag = flag & ~(NVT_FLAG_MAX_EDID_TIMING);

    status = NvTiming_GetEdidTiming(width, height, rr, flag, pEdidInfo, &Timing);
    if (NVT_STATUS_SUCCESS == status)
    {
        status = NVT_STATUS_ERR;

        // is this an exact match?
        if (0 == NVT_GET_TIMING_STATUS_MATCH(Timing.etc.status))
        {
            if (NVT_TYPE_EDID_861ST == NVT_GET_TIMING_STATUS_TYPE(Timing.etc.status))
            {
                // lookup the vic for this timing in the support map.
                Vic = (NvU8) NVT_GET_CEA_FORMAT(Timing.etc.status);
                for (i = 0; i < pEdidInfo->Hdmi3Dsupport.total; ++i)
                {
                    if (Vic == pEdidInfo->Hdmi3Dsupport.map[i].Vic)
                    {
                        break;
                    }
                }
                if (i < pEdidInfo->Hdmi3Dsupport.total)
                {
                    // does this vic support the requested structure type?
                    if (0 != (NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(StereoStructureType) & pEdidInfo->Hdmi3Dsupport.map[i].StereoStructureMask))
                    {
                        // if this is side-by-side(half) the detail needs to match also.
                        if ((NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF != StereoStructureType) || (SideBySideHalfDetail == pEdidInfo->Hdmi3Dsupport.map[i].SideBySideHalfDetail))
                        {
                            // convert the 2D timing to 3D.
                            NvTiming_GetHDMIStereoTimingFrom2DTiming(&Timing, StereoStructureType, SideBySideHalfDetail, pT);
                            status = NVT_STATUS_SUCCESS;
                        }
                    }
                }
            }
        }
    }
    return status;
}

// EDID based AspectRatio Timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetEDIDBasedASPRTiming( NvU16 width, NvU16 height, NvU16 rr, NVT_EDID_INFO *pEI, NVT_TIMING *pT)
{
    NvU32 i, dwStatus;
    NvU32 dwNativeIndex;
    NvU32 flag;
    NvU32 ret;

    // sanity check
    if( pEI == NULL || pEI->total_timings == 0 || pT == NULL )
    {
        return NVT_STATUS_ERR;
    }
    if( width == 0 || height == 0 )
    {
        return NVT_STATUS_ERR;
    }

    // get an EDID timing. Return err if it fails as we don't have any timing to tweak.
    flag = 0;
    ret = NvTiming_GetEdidTiming(width, height, rr, flag, pEI, pT);
    if( NVT_STATUS_SUCCESS != ret )
    {
        return NVT_STATUS_ERR;
    }
    // in case we have an exact match from EDID (in terms of Size), we return Success.
    else if ((NVT_GET_TIMING_STATUS_MATCH(pT->etc.status) & NVT_STATUS_TIMING_MISMATCH_SIZE) == 0)
    {
        return NVT_STATUS_SUCCESS;
    }

    // find the Native timing
    for (i = 0, dwNativeIndex = pEI->total_timings + 1; i < pEI->total_timings; i++)
    {
        dwStatus = pEI->timing[i].etc.status;

        if ((NVT_IS_NATIVE_TIMING(dwStatus)) || NVT_IS_DTD1(dwStatus))
        {
            dwNativeIndex = i;
            break;
        }
    }

    // we don't want to apply LogicScaling(Letterboxing) to Wide Mode on Wide Panel (or non-Wide Mode on non-Wide Panel)
    if( nvt_is_wideaspect(width, height) == nvt_is_wideaspect(pEI->timing[dwNativeIndex].HVisible, pEI->timing[dwNativeIndex].VVisible) )
    {
        return NVT_STATUS_ERR;
    }

    // Letterbox mode enabled by regkey LogicScalingMode
    // When we try to set modes not supported in EDID (eg. DFP over DSub) the display may not fit the screen.
    // If Logic Scaling is enabled (ie why we are here), we need to tweak the timing (for CRT) provided:
    //          1) the aspect ratio of native mode and requested mode differ
    //                                      eg. Native AR = 5:4,        1280x1024
    //                                          Requested AR = 16:10,   1280x800
    //          2) Both Width and Height do not mismatch together; If they do we shall go in for DMT/GTF timing
    //             by failing this call.
    if( pT->interlaced == 0 &&
        dwNativeIndex < pEI->total_timings &&
        (pEI->timing[dwNativeIndex].HVisible*height != pEI->timing[dwNativeIndex].VVisible*width) &&
        (width == pT->HVisible || height == pT->VVisible))
    {
        pT->HFrontPorch += (pT->HVisible - width) / 2;
        pT->VFrontPorch += (pT->VVisible - height) / 2;
        pT->HVisible = width;
        pT->VVisible = height;
        if(rr != pT->etc.rr)
        {
            pT->etc.rrx1k = rr * 1000;
            pT->pclk = RRx1kToPclk (pT);
        }

        pT->etc.status = NVT_STATUS_ASPR;
        return NVT_STATUS_SUCCESS;
    }

    return NVT_STATUS_ERR;
}

// check whether EDID is valid
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_EDIDValidationMask(NvU8 *pEdid, NvU32 length, NvBool bIsStrongValidation)
{
    NvU32                               i, j, version, checkSum;
    EDIDV1STRUC                         *p = (EDIDV1STRUC *)pEdid;
    EDID_LONG_DISPLAY_DESCRIPTOR        *pLdd;
    NvU8                                *pExt;
    DETAILEDTIMINGDESCRIPTOR            *pDTD;
    NvU32 ret = 0;

    // check the EDID base size to avoid accessing beyond the EDID buffer, do not proceed with
    // further validation.
    if (length < sizeof(EDIDV1STRUC))
    {
        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_SIZE);
        return ret;
    }

    // check the EDID version and signature    
    if (getEdidVersion(pEdid, &version) != NVT_STATUS_SUCCESS)
    {
        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_VERSION);
        return ret;
    }

    // check block 0 checksum value
    if (!isChecksumValid(pEdid))
    {
        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM);
        return ret;
    }

    // Strong validation to follow
    if (bIsStrongValidation == NV_TRUE)
    {
        // range limit check
        for (i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
        {
            pLdd = (EDID_LONG_DISPLAY_DESCRIPTOR *)&p->DetailedTimingDesc[i];
            if (pLdd->tag == NVT_EDID_DISPLAY_DESCRIPTOR_DRL && (version == 0x103 || (version == 0x104 && (p->bFeatureSupport & 1))))
            {
                EDID_MONITOR_RANGE_LIMIT *pRangeLimit = (EDID_MONITOR_RANGE_LIMIT *)pLdd->data;
                NvU8    max_v_rate_offset, min_v_rate_offset, max_h_rate_offset, min_h_rate_offset;

                // add 255Hz offsets as needed before doing the check, use descriptor->rsvd2
                nvt_assert(!(pLdd->rsvd2 & 0xF0));

                max_v_rate_offset = pLdd->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_VER_MAX ? NVT_PVT_EDID_RANGE_OFFSET_AMOUNT : 0;
                min_v_rate_offset = pLdd->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_VER_MIN ? NVT_PVT_EDID_RANGE_OFFSET_AMOUNT : 0;
                max_h_rate_offset = pLdd->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_HOR_MAX ? NVT_PVT_EDID_RANGE_OFFSET_AMOUNT : 0;
                min_h_rate_offset = pLdd->rsvd2 & NVT_PVT_EDID_RANGE_OFFSET_HOR_MIN ? NVT_PVT_EDID_RANGE_OFFSET_AMOUNT : 0;

                if ((pRangeLimit->minVRate + min_v_rate_offset) > (pRangeLimit->maxVRate + max_v_rate_offset) ||
                    (pRangeLimit->minHRate + min_h_rate_offset) > (pRangeLimit->maxHRate + max_h_rate_offset) ||
                    pRangeLimit->maxVRate == 0 ||
                    pRangeLimit->maxHRate == 0)
                {
                    ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_RANGE_LIMIT);
                }
                break;
            }
        }

        // extension and size check
        if ((NvU32)(p->bExtensionFlag + 1) * sizeof(EDIDV1STRUC) > length)
        {
            // Do not proceed with further validation if the size is invalid.
            ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_SIZE);
            return ret;
        }

        // validate Detailed Timing Descriptors, 4 blocks
        for (i = 0; i < 4; i++)
        {
            if (*((NvU16 *)&p->DetailedTimingDesc[i]) != 0)
            {
                // This block is not a Display Descriptor.
                // It must be a valid timing definition
                // validate the block by passing NULL as the NVTIMING parameter to parseEdidDetailedTimingDescriptor
                if (parseEdidDetailedTimingDescriptor((NvU8 *)&p->DetailedTimingDesc[i], NULL) != NVT_STATUS_SUCCESS)
                {
                    ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_DTD);
                }
            }
            else
            {
                // This block is a display descriptor, validate
                if (((EDID_LONG_DISPLAY_DESCRIPTOR *)&p->DetailedTimingDesc[i])->rsvd != 0)
                {
                    ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_DTD);
                }
            }
        }

        // validate extension blocks
        for (j = 1; j <= p->bExtensionFlag; j++)
        {
            pExt = pEdid + sizeof(EDIDV1STRUC) * j;

            // check for 861 extension
            switch (*pExt)
            {
                case NVT_EDID_EXTENSION_CTA:
                    // first sanity check on the extension block
                    if (get861ExtInfo(pExt, sizeof(EIA861EXTENSION), NULL) != NVT_STATUS_SUCCESS)
                    {
                        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT);
                    }

                    // check sum on CEA extension block
                    for (i = 0, checkSum = 0; i < sizeof(EIA861EXTENSION); i ++)
                    {
                        checkSum += pExt[i];
                    }

                    if ((checkSum & 0xFF) != 0)
                    {
                        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM);
                    }

                    // 0 indicates no DTD in this block
                    if (((EIA861EXTENSION*)pExt)->offset == 0)
                    {
                        continue;
                    }

                    // validate DTD blocks
                    pDTD = (DETAILEDTIMINGDESCRIPTOR *)&pExt[((EIA861EXTENSION *)pExt)->offset];
                    while (pDTD->wDTPixelClock != 0 &&
                            (NvU8 *)pDTD - pExt < (int)sizeof(EIA861EXTENSION))
                    {
                        if (parseEdidDetailedTimingDescriptor((NvU8 *)pDTD, NULL) != NVT_STATUS_SUCCESS)
                        {
                            ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT_DTD);
                        }
                        pDTD++;
                    }
                    break;
                case NVT_EDID_EXTENSION_VTB:
                    // perform a checksum on the VTB block
                    for (i = 0, checkSum = 0; i < sizeof(VTBEXTENSION); i++)
                    {
                        checkSum += pExt[i];
                    }
                    if ((checkSum & 0xFF) != 0)
                    {
                        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM);
                    }
                    break;
                case NVT_EDID_EXTENSION_DISPLAYID:
                    // perform a checksum on the VTB block
                    for (i = 0, checkSum = 0; i < sizeof(EIA861EXTENSION); i++)
                    {
                        checkSum += pExt[i];
                    }
                    if ((checkSum & 0xFF) != 0)
                    {
                        ret |= NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM);
                    }
                    break;
                default:
                    break;
            }
        }


    }

    return ret;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EDIDValidation (NvU8 *pEdid, NvU32 length, NvBool bIsStrongValidation)
{
    if (NvTiming_EDIDValidationMask(pEdid, length, bIsStrongValidation) != 0) {
        return NVT_STATUS_ERR;
    } else {
        return NVT_STATUS_SUCCESS;
    }
}

// Function Description: Get the first Detailed Timing Descriptor
//
// Parameters:
//        pEdidInfo:  IN - pointer to parsed EDID
//               pT:  OUT - pointer to where the DTD1 timing will be stored
//
// Return:
//             NVT_STATUS_SUCCESS: DTD1 was found in parsed EDID, pT is a valid result
//   NVT_STATUS_INVALID_PARAMETER: one or more parameter was invalid
//           NVT_STATUS_ERR: DTD1 was not found in parsed EDID, pT is invalid
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetDTD1Timing (NVT_EDID_INFO * pEdidInfo, NVT_TIMING * pT)
{
    NvU32 j;

    // check param
    if (pEdidInfo == NULL || pT == NULL)
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    // find the PTM mode
    for (j = 0; j < pEdidInfo->total_timings; j++)
    {
        if (NVT_PREFERRED_TIMING_IS_DTD1(pEdidInfo->timing[j].etc.flag, pEdidInfo->timing[j].etc.status))
        {
            *pT = pEdidInfo->timing[j];
            return NVT_STATUS_SUCCESS;
        }
    }

    // find DisplayID preferred
    for (j = 1; j < pEdidInfo->total_timings; j++)
    {
        if (NVT_PREFERRED_TIMING_IS_DISPLAYID(pEdidInfo->timing[j].etc.flag))
        {
            *pT = pEdidInfo->timing[j];
            return NVT_STATUS_SUCCESS;
        }
    }

    // DTD1 should exist, but if it doesn't, return not found
    for (j = 0; j < pEdidInfo->total_timings; j++)
    {
        NvU32 data = pEdidInfo->timing[j].etc.status;
        if (NVT_IS_DTD1(data))
        {
            *pT = pEdidInfo->timing[j];
            return NVT_STATUS_SUCCESS;
        }
    }

    // DTD1 should exist, but if it doesn't, return not found
    return NVT_STATUS_ERR;
}

// Description: Parses a VTB extension block into its associated timings
//
// Parameters:
//     pEdidExt: IN - pointer to the beginning of the extension block
//        pInfo: IN - The original block information, including the
//                    array of timings.
//
// NOTE: this function *really* should be in its own separate file, but a certain DVS test
// uses cross build makefiles which do not allow the specification of a new file.
CODE_SEGMENT(PAGE_DD_CODE)
void parseVTBExtension(NvU8 *pEdidExt, NVT_EDID_INFO *pInfo)
{
    NvU32 i;
    VTBEXTENSION *pExt = (VTBEXTENSION *)pEdidExt;
    NvU32 count;
    NvU32 bytes;
    NVT_TIMING newTiming;

    // Null = bad idea
    if (pEdidExt == NULL)
    {
        return;
    }

    // Sanity check for VTB extension block
    if (pExt->tag != NVT_EDID_EXTENSION_VTB ||
        pExt->revision == NVT_VTB_REV_NONE)
    {
        return;
    }

    // Sanity check - ensure that the # of descriptor does not exceed
    // byte size
    count = (NvU32)sizeof(EDID_LONG_DISPLAY_DESCRIPTOR) * pExt->num_detailed
          + (NvU32)sizeof(EDID_CVT_3BYTE_BLOCK) * pExt->num_cvt
          + (NvU32)sizeof(NvU16) * pExt->num_standard;
    if (count > NVT_VTB_MAX_PAYLOAD)
    {
        return;
    }

    count = 0;
    bytes = 0;

    // Process Detailed Timings
    for (i = 0; i < pExt->num_detailed; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseEdidDetailedTimingDescriptor((NvU8 *)(pExt->data + bytes),
                                              &newTiming) == NVT_STATUS_SUCCESS)
        {
            newTiming.etc.name[39] = '\0';
            newTiming.etc.status = NVT_STATUS_EDID_VTB_EXT_DTDn(++count);

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }

            bytes += (NvU32)(sizeof(EDID_LONG_DISPLAY_DESCRIPTOR));
        }
    }

    // Process CVT Timings
    for (i = 0; i < pExt->num_cvt; i++)
    {
        parseEdidCvt3ByteDescriptor((NvU8 *)(pExt->data + bytes), pInfo, &count);

        bytes += (NvU32)sizeof(EDID_CVT_3BYTE_BLOCK);
    }

    // Process Standard Timings
    for (i = 0; i < pExt->num_standard; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        parseEdidStandardTimingDescriptor(*(NvU16 *)(pExt->data + bytes),
                                          pInfo, count, &newTiming);
        newTiming.etc.name[39] = '\0';
        newTiming.etc.status = NVT_STATUS_EDID_VTB_EXT_STDn(++count);

        if (!assignNextAvailableTiming(pInfo, &newTiming))
        {
            break;
        }

        bytes += (NvU32)sizeof(NvU16);
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
static int IsPrintable(NvU8 c)
{
    return ((c >= ' ') && (c <= '~'));
}

CODE_SEGMENT(PAGE_DD_CODE)
static int IsWhiteSpace(NvU8 c)
{
    // consider anything unprintable or single space (ASCII 32)
    // to be whitespace
    return (!IsPrintable(c) || (c == ' '));
}

CODE_SEGMENT(PAGE_DD_CODE)
static void RemoveTrailingWhiteSpace(NvU8 *str, int len)
{
    int i;

    for (i = len; (i >= 0) && IsWhiteSpace(str[i]); i--)
    {
        str[i] = '\0';
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
static void RemoveNonPrintableCharacters(NvU8 *str)
{
    int i;

    // Check that all characters are printable.
    // If not, replace them with '?'
    for (i = 0; str[i] != '\0'; i++)
    {
        if (!IsPrintable(str[i]))
        {
            str[i] = '?';
        }
    }
}

/**
 * @brief Assigns this timing to the next available slot in pInfo->timing[] if
 *        possible.
 * @param pInfo EDID struct containing the parsed timings
 * @param pTiming New timing to be copied into pInfo->timing[]
 */
CODE_SEGMENT(PAGE_DD_CODE)
NvBool assignNextAvailableTiming(NVT_EDID_INFO *pInfo,
                                 const NVT_TIMING *pTiming)
{
    // Don't write past the end of
    // pInfo->timing[NVT_EDID_MAX_TOTAL_TIMING]
    if (pInfo->total_timings >= COUNT(pInfo->timing)) {
        return NV_FALSE;
    }

    pInfo->timing[pInfo->total_timings++] = *pTiming;
    return NV_TRUE;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetProductName(const NVT_EDID_INFO *pEdidInfo,
                                   NvU8 *pProductName,
                                   const NvU32 productNameLength)
{
    NvU32 i = 0, m = 0, n = 0;

    if( pEdidInfo == NULL || pProductName == NULL )
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    for ( i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        if (pEdidInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRITPOR_DPN)
        {
            for(n = 0; n < NVT_EDID_LDD_PAYLOAD_SIZE && pEdidInfo->ldd[i].u.product_name.str[n] != 0x0; n++)
            {
                pProductName[m++] = pEdidInfo->ldd[i].u.product_name.str[n];
                if ((m + 1) >= productNameLength)
                {
                    goto done;
                }
            }
        }
    }
done:
    pProductName[m] = '\0'; //Ensure a null termination at the end.

    RemoveTrailingWhiteSpace(pProductName, m);
    RemoveNonPrintableCharacters(pProductName);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_CalculateEDIDCRC32(NvU8* pEDIDBuffer, NvU32 edidsize)
{
    return calculateCRC32(pEDIDBuffer, edidsize);
}

//Calculates EDID's CRC after purging 'Week of Manufacture', 'Year of Manufacture',
//'Product ID String' & 'Serial Number' from EDID
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_CalculateCommonEDIDCRC32(NvU8* pEDIDBuffer, NvU32 edidVersion)
{
    NvU32 commonEDIDBufferSize = 0;
    NvU8 CommonEDIDBuffer[256];
    NvU32 edidBufferIndex = 0;

    if(pEDIDBuffer==NULL)
    {
        return 0;
    }

    // Transfer over the original EDID buffer
    NVMISC_MEMCPY(CommonEDIDBuffer, pEDIDBuffer, 256);

    // Wipe out the Serial Number, Week of Manufacture, and Year of Manufacture or Model Year
    NVMISC_MEMSET(CommonEDIDBuffer + 0x0C, 0, 6);

    // Wipe out the checksums
    CommonEDIDBuffer[0x7F] = 0;
    CommonEDIDBuffer[0xFF] = 0;

    // We also need to zero out any "EDID Other Monitor Descriptors" (http://en.wikipedia.org/wiki/Extended_display_identification_data)
    for (edidBufferIndex = 54; edidBufferIndex <= 108; edidBufferIndex += 18)
    {
        if (CommonEDIDBuffer[edidBufferIndex] == 0 && CommonEDIDBuffer[edidBufferIndex+1] == 0)
        {
            // Wipe this block out. It contains OEM-specific details that contain things like serial numbers
            NVMISC_MEMSET(CommonEDIDBuffer + edidBufferIndex, 0, 18);
        }
    }

    // Check what size we should do the compare against
    if ( edidVersion > NVT_EDID_VER_1_4 )
    {
        commonEDIDBufferSize = 256;
    }
    else
    {
        commonEDIDBufferSize = 128;
    }

    return NvTiming_CalculateEDIDCRC32(CommonEDIDBuffer, commonEDIDBufferSize);
} // NvTiming_CalculateCommonEDIDCRC32

// Calculate the minimum and maximum v_rate and h_rate, as well as
// maximum pclk; initialize with the range of values in the EDID mode
// list, but override with what is in the range limit descriptor section.
//
// based on drivers/modeset.nxt/CODE/edid.c:EdidGetMonitorLimits() and
// EdidBuildRangeLimits()
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalculateEDIDLimits(NVT_EDID_INFO *pEdidInfo, NVT_EDID_RANGE_LIMIT *pLimit)
{
    NvU32 i;

    NVMISC_MEMSET(pLimit, 0, sizeof(NVT_EDID_RANGE_LIMIT));

    // the below currently only supports 1.x EDIDs
    if ((pEdidInfo->version & 0xFF00) != 0x100)
    {
        return NVT_STATUS_ERR;
    }

    pLimit->min_v_rate_hzx1k = ~0;
    pLimit->max_v_rate_hzx1k = 0;
    pLimit->min_h_rate_hz = ~0;
    pLimit->max_h_rate_hz = 0;
    pLimit->max_pclk_10khz = 0;

    // find the ranges in the EDID mode list
    for (i = 0; i < pEdidInfo->total_timings; i++)
    {
        NVT_TIMING *pTiming = &pEdidInfo->timing[i];
        NvU32 h_rate_hz;

        if (pLimit->min_v_rate_hzx1k > pTiming->etc.rrx1k)
        {
            pLimit->min_v_rate_hzx1k = pTiming->etc.rrx1k;
        }
        if (pLimit->max_v_rate_hzx1k < pTiming->etc.rrx1k)
        {
            pLimit->max_v_rate_hzx1k = pTiming->etc.rrx1k;
        }

        h_rate_hz = axb_div_c(pTiming->pclk, 10000, (NvU32)pTiming->HTotal);

        if (pLimit->min_h_rate_hz > h_rate_hz)
        {
            pLimit->min_h_rate_hz = h_rate_hz;
        }
        if (pLimit->max_h_rate_hz < h_rate_hz)
        {
            pLimit->max_h_rate_hz = h_rate_hz;
        }

        if (pLimit->max_pclk_10khz < pTiming->pclk)
        {
            pLimit->max_pclk_10khz = pTiming->pclk;
        }
    }

    // use the range limit display descriptor, if available: these
    // override anything we found in the EDID mode list
    for (i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++)
    {
        if (pEdidInfo->ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_DRL)
        {
            NVT_EDID_DD_RANGE_LIMIT *pRangeLimit = &pEdidInfo->ldd[i].u.range_limit;
            NvU32 max_pclk_10khz;

            // {min,max}_v_rate is in hz
            if (pRangeLimit->min_v_rate != 0) {
                pLimit->min_v_rate_hzx1k = pRangeLimit->min_v_rate * 1000;
            }
            if (pRangeLimit->max_v_rate != 0) {
                pLimit->max_v_rate_hzx1k = pRangeLimit->max_v_rate * 1000;
            }

            // {min,max}_h_rate is in khz
            if (pRangeLimit->min_h_rate != 0) {
                pLimit->min_h_rate_hz = pRangeLimit->min_h_rate * 1000;
            }
            if (pRangeLimit->max_h_rate != 0) {
                pLimit->max_h_rate_hz = pRangeLimit->max_h_rate * 1000;
            }

            // EdidGetMonitorLimits() honored the pclk from the
            // modelist over what it found in the range limit
            // descriptor, so do the same here
            max_pclk_10khz = pRangeLimit->max_pclk_MHz * 100;
            if (pLimit->max_pclk_10khz < max_pclk_10khz) {
                pLimit->max_pclk_10khz = max_pclk_10khz;
            }

            break;
        }
    }

    return NVT_STATUS_SUCCESS;
}

// Build a user-friendly name:
//
// * get the vendor name:
//     * use the 3 character PNP ID from the EDID's manufacturer ID field
//     * expand, if possible, the PNP ID using the PNPVendorIds[] table
// * get the product name from the descriptor block(s)
// * prepend the vendor name and the product name, unless the product
//   name already contains the vendor name
// * if any characters in the string are outside the printable ASCII
//   range, replace them with '?'

#define tolower(c)      (((c) >= 'A' && (c) <= 'Z') ? (c) + ('a'-'A') : (c))

CODE_SEGMENT(PAGE_DD_CODE)
void NvTiming_GetMonitorName(NVT_EDID_INFO *pEdidInfo,
                             NvU8 monitor_name[NVT_EDID_MONITOR_NAME_STRING_LENGTH])
{
    NvU8 product_name[NVT_EDID_MONITOR_NAME_STRING_LENGTH];
    const NvU8 *vendor_name;
    NVT_STATUS status;
    NvU32 i, j;
    NvBool prepend_vendor;

    NVMISC_MEMSET(monitor_name, 0, NVT_EDID_MONITOR_NAME_STRING_LENGTH);

    // get vendor_name: it is either the manufacturer ID or the PNP vendor name
    vendor_name = pEdidInfo->manuf_name;

    for (i = 0; i < (sizeof(PNPVendorIds)/sizeof(PNPVendorIds[0])); i++)
    {
        if ((vendor_name[0] == PNPVendorIds[i].vendorId[0]) &&
            (vendor_name[1] == PNPVendorIds[i].vendorId[1]) &&
            (vendor_name[2] == PNPVendorIds[i].vendorId[2]))
        {
            vendor_name = (const NvU8 *) PNPVendorIds[i].vendorName;
            break;
        }
    }

    // get the product name from the descriptor blocks
    status = NvTiming_GetProductName(pEdidInfo, product_name, sizeof(product_name));

    if (status != NVT_STATUS_SUCCESS)
    {
        product_name[0] = '\0';
    }

    // determine if the product name already includes the vendor name;
    // if so, do not prepend the vendor name to the monitor name
    prepend_vendor = NV_TRUE;

    for (i = 0; i < NVT_EDID_MONITOR_NAME_STRING_LENGTH; i++)
    {
        if (vendor_name[i] == '\0')
        {
            prepend_vendor = NV_FALSE;
            break;
        }

        if (tolower(product_name[i]) != tolower(vendor_name[i]))
        {
            break;
        }
    }

    j = 0;

    // prepend the vendor name to the monitor name
    if (prepend_vendor)
    {
        for (i = 0; (i < NVT_EDID_MONITOR_NAME_STRING_LENGTH) && (vendor_name[i] != '\0'); i++)
        {
            monitor_name[j++] = vendor_name[i];
        }
    }

    // if we added the vendor name above, add a space between the
    // vendor name and the product name
    if ((j > 0) && (j < (NVT_EDID_MONITOR_NAME_STRING_LENGTH - 1)))
    {
        monitor_name[j++] = ' ';
    }

    // append the product name to the monitor string
    for (i = 0; (i < NVT_EDID_MONITOR_NAME_STRING_LENGTH) && (product_name[i] != '\0'); i++)
    {
        if (j >= (NVT_EDID_MONITOR_NAME_STRING_LENGTH - 1))
        {
            break;
        }
        monitor_name[j++] = product_name[i];
    }
    monitor_name[j] = '\0';

    RemoveTrailingWhiteSpace(monitor_name, j);
    RemoveNonPrintableCharacters(monitor_name);
}

CODE_SEGMENT(PAGE_DD_CODE)
void updateHDMILLCDeepColorForTiming(NVT_EDID_INFO *pInfo, NvU32 index)
{
    NVT_EDID_CEA861_INFO *p861Info = &pInfo->ext861;
    // NOTE: EDID and CEA861 does not have clear statement regarding this.
    // To be backward compatible with current Nvidia implementation, if not edid >= 1.4 and CEA block exists, follow color format declaration from CEA block.
    // update supported color space within each bpc
    // rgb 8bpc always supported

    UPDATE_BPC_FOR_COLORFORMAT(pInfo->timing[index].etc.rgb444, 0, 1,
                               pInfo->hdmiLlcInfo.dc_30_bit,
                               pInfo->hdmiLlcInfo.dc_36_bit,
                               0, pInfo->hdmiLlcInfo.dc_48_bit);

    if (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_444)
    {
        // pHdmiLlc->dc_y444 assumed basic cap is set; when base cap is set, 8bpc yuv444 always supported
        UPDATE_BPC_FOR_COLORFORMAT(pInfo->timing[index].etc.yuv444, 0, 1,
                                   pInfo->hdmiLlcInfo.dc_y444 && pInfo->hdmiLlcInfo.dc_30_bit,
                                   pInfo->hdmiLlcInfo.dc_y444 && pInfo->hdmiLlcInfo.dc_36_bit,
                                   0, pInfo->hdmiLlcInfo.dc_y444 && pInfo->hdmiLlcInfo.dc_48_bit);
    }
    if (p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_422)
    {
        // pHdmiLlc->dc_y444 assumed basic cap is set; when base cap is set, 8bpc yuv422 always supported
        // newer CEA861/HDMI specs suggest the base cap should support both or neither (Nvidia puts no limitations here)
        // HDMI1.4b spec  Section 6.2.4 Color Depth Requirements states that YCbCr 4:2:2 format is 36-bit mode, which means 8, 10 and 12bpc output is supported as soon as there is enough bandwidth
        UPDATE_BPC_FOR_COLORFORMAT(pInfo->timing[index].etc.yuv422, 0, 1, 1, 1, 0, 0);
    }
}

POP_SEGMENTS
