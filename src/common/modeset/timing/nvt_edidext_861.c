//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//  File:       nvt_edidext_861.c
//
//  Purpose:    the provide edid 861 extension related services
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "edid.h"



PUSH_SEGMENTS

#define EIA_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rrx1k,ip,aspect,rep,format) \
    {hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',(ip)=='i' ? NVT_INTERLACED:NVT_PROGRESSIVE,\
    0,{0,((rrx1k)+500)/1000,rrx1k,((1?aspect)<<16)|(0?aspect),rep,{0},{0},{0},{0},NVT_STATUS_EDID_861STn(format),"CEA-861B:#"#format""}}


#define NVT_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rrx1k,ip,aspect,rep,format,name) \
    {hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',(ip)=='i' ? NVT_INTERLACED:NVT_PROGRESSIVE,\
    0,{0,((rrx1k)+500)/1000,rrx1k,((1?aspect)<<16)|(0?aspect),rep,{0},{0},{0},{0},NVT_TYPE_NV_PREDEFINEDn(format),name}}

#define HDMI_EXT_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rrx1k,ip,aspect,rep,format,name) \
    {hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',(ip)=='i' ? NVT_INTERLACED:NVT_PROGRESSIVE,\
    0,{0,((rrx1k)+500)/1000,rrx1k,((1?aspect)<<16)|(0?aspect),rep,{0},{0},{0},{0},NVT_STATUS_HDMI_EXTn(format),name}}

DATA_SEGMENT(PAGE_DATA)
CONS_SEGMENT(PAGE_CONS)

static const NVT_TIMING EIA861B[]=
{
    // all 64 EIA/CEA-861E timings
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3,   0x1, 1),//640       x  480p @59.94/60   (Format 1)
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-', 59940,'p', 4:3,   0x1, 2),//720       x  480p @59.94/60   (Format 2) 
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-', 59940,'p',16:9,   0x1, 3),//720       x  480p @59.94/60   (Format 3)
    EIA_TIMING(1280, 110, 40,1650,'+', 720, 5,5, 750,'+', 59940,'p',16:9,   0x1, 4),//1280      x  720p @59.94/60   (Format 4)
    EIA_TIMING(1920,  88, 44,2200,'+', 540, 2,5, 562,'+', 59940,'i',16:9,   0x1, 5),//1920      x 1080i @59.94/60   (Format 5)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-', 59940,'i', 4:3,   0x2, 6),//720(1440) x  480i @59.94/60   (Format 6)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-', 59940,'i',16:9,   0x2, 7),//720(1440) x  480i @59.94/60   (Format 7)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 263,'-', 59940,'p', 4:3,   0x2, 8),//720(1440) x  240p @59.94/60   (Format 8)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 263,'-', 59940,'p',16:9,   0x2, 9),//720(1440) x  240p @59.94/60   (Format 9)
    EIA_TIMING(2880,  76,248,3432,'-', 240, 4,3, 262,'-', 59940,'i', 4:3, 0x3ff,10),//(2880)    x  480i @59.94/60   (Format 10)
    EIA_TIMING(2880,  76,248,3432,'-', 240, 4,3, 262,'-', 59940,'i',16:9, 0x3ff,11),//(2880)    x  480i @59.94/60   (Format 11)
    EIA_TIMING(2880,  76,248,3432,'-', 240, 5,3, 263,'-', 59940,'p', 4:3, 0x3ff,12),//(2880)    x  480p @59.94/60   (Format 12)
    EIA_TIMING(2880,  76,248,3432,'-', 240, 5,3, 263,'-', 59940,'p',16:9, 0x3ff,13),//(2880)    x  480p @59.94/60   (Format 13)
    EIA_TIMING(1440,  32,124,1716,'-', 480, 9,6, 525,'-', 59940,'p', 4:3,   0x3,14),//1440      x  480p @59.94/60   (Format 14)
    EIA_TIMING(1440,  32,124,1716,'-', 480, 9,6, 525,'-', 59940,'p',16:9,   0x3,15),//1440      x  480p @59.94/60   (Format 15)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4,5,1125,'+', 59940,'p',16:9,   0x1,16),//1920      x 1080p @59.94/60   (Format 16)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-', 50000,'p', 4:3,   0x1,17),//720       x  576p @50         (Format 17)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-', 50000,'p',16:9,   0x1,18),//720       x  576p @50         (Format 18)
    EIA_TIMING(1280, 440, 40,1980,'+', 720, 5,5, 750,'+', 50000,'p',16:9,   0x1,19),//1280      x  720p @50         (Format 19)
    EIA_TIMING(1920, 528, 44,2640,'+', 540, 2,5, 562,'+', 50000,'i',16:9,   0x1,20),//1920      x 1080i @50         (Format 20)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-', 50000,'i', 4:3,   0x2,21),//720(1440) x  576i @50         (Format 21)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-', 50000,'i',16:9,   0x2,22),//720(1440) x  576i @50         (Format 22)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-', 50000,'p', 4:3,   0x2,23),//720(1440) x  288p @50         (Format 23)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-', 50000,'p',16:9,   0x2,24),//720(1440) x  288p @50         (Format 24)
    EIA_TIMING(2880,  48,252,3456,'-', 288, 2,3, 312,'-', 50000,'i', 4:3, 0x3ff,25),//(2880)    x  576i @50         (Format 25)
    EIA_TIMING(2880,  48,252,3456,'-', 288, 2,3, 312,'-', 50000,'i',16:9, 0x3ff,26),//(2880)    x  576i @50         (Format 26)
    EIA_TIMING(2880,  48,252,3456,'-', 288, 2,3, 312,'-', 50000,'p', 4:3, 0x3ff,27),//(2880)    x  288p @50         (Format 27)
    EIA_TIMING(2880,  48,252,3456,'-', 288, 2,3, 312,'-', 50000,'p',16:9, 0x3ff,28),//(2880)    x  288p @50         (Format 28)
    EIA_TIMING(1440,  24,128,1728,'-', 576, 5,5, 625,'_', 50000,'p', 4:3,   0x3,29),//1440      x  576p @50         (Format 29)
    EIA_TIMING(1440,  24,128,1728,'-', 576, 5,5, 625,'_', 50000,'p',16:9,   0x3,30),//1440      x  576p @50         (Format 30)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4,5,1125,'+', 50000,'p',16:9,   0x1,31),//1920      x 1080p @50         (Format 31)
    EIA_TIMING(1920, 638, 44,2750,'+',1080, 4,5,1125,'+', 23976,'p',16:9,   0x1,32),//1920      x 1080p @23.97/24   (Format 32)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4,5,1125,'+', 25000,'p',16:9,   0x1,33),//1920      x 1080p @25         (Format 33)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4,5,1125,'+', 29970,'p',16:9,   0x1,34),//1920      x 1080p @29.97/30   (Format 34)
    EIA_TIMING(2880,  64,248,3432,'-', 480, 9,6, 525,'-', 59940,'p', 4:3,   0x7,35),//(2880)    x  480p @59.94/60   (Format 35)
    EIA_TIMING(2880,  64,248,3432,'-', 480, 9,6, 525,'-', 59940,'p',16:9,   0x7,36),//(2880)    x  480p @59.94/60   (Format 36)
    EIA_TIMING(2880,  48,256,3456,'-', 576, 5,5, 625,'-', 50000,'p', 4:3,   0x7,37),//(2880)    x  576p @50         (Format 37)
    EIA_TIMING(2880,  48,256,3456,'-', 576, 5,5, 625,'-', 50000,'p',16:9,   0x7,38),//(2880)    x  576p @50         (Format 38)
    EIA_TIMING(1920,  32,168,2304,'+', 540,23,5, 625,'-', 50000,'i',16:9,   0x1,39),//1920      x 1080i @50         (Format 39)
    EIA_TIMING(1920, 528, 44,2640,'+', 540, 2,5, 562,'+',100000,'i',16:9,   0x1,40),//1920      x 1080i @100        (Format 40)
    EIA_TIMING(1280, 440, 40,1980,'+', 720, 5,5, 750,'+',100000,'p',16:9,   0x1,41),//1280      x  720p @100        (Format 41)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-',100000,'p', 4:3,   0x1,42),//720       x  576p @100        (Format 42)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-',100000,'p',16:9,   0x1,43),//720       x  576p @100        (Format 43)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-',100000,'i', 4:3,   0x2,44),//720(1440) x  576i @100        (Format 44)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-',100000,'i',16:9,   0x2,45),//720(1440) x  576i @100        (Format 45)
    EIA_TIMING(1920,  88, 44,2200,'+', 540, 2,5, 562,'+',119880,'i',16:9,   0x1,46),//1920      x 1080i @119.88/120 (Format 46)
    EIA_TIMING(1280, 110, 40,1650,'+', 720, 5,5, 750,'+',119880,'p',16:9,   0x1,47),//1280      x  720p @119.88/120 (Format 47)
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-',119880,'p', 4:3,   0x1,48),//720       x  480p @119.88/120 (Format 48)
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-',119880,'p',16:9,   0x1,49),//720       x  480p @119.88/120 (Format 49)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-',119880,'i', 4:3,   0x2,50),//720(1440) x  480i @119.88/120 (Format 50)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-',119880,'i',16:9,   0x2,51),//720(1440) x  480i @119.88/120 (Format 51)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-',200000,'p', 4:3,   0x1,52),//720       x  576p @200        (Format 52)
    EIA_TIMING( 720,  12, 64, 864,'-', 576, 5,5, 625,'-',200000,'p',16:9,   0x1,53),//720       x  576p @200        (Format 53)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-',200000,'i', 4:3,   0x2,54),//720(1440) x  576i @200        (Format 54)
    EIA_TIMING(1440,  24,126,1728,'-', 288, 2,3, 312,'-',200000,'i',16:9,   0x2,55),//720(1440) x  576i @200        (Format 55)
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-',239760,'p', 4:3,   0x1,56),//720       x  480p @239.76/240 (Format 56)
    EIA_TIMING( 720,  16, 62, 858,'-', 480, 9,6, 525,'-',239760,'p',16:9,   0x1,57),//720       x  480p @239.76/240 (Format 57)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-',239760,'i', 4:3,   0x2,58),//720(1440) x  480i @239.76/240 (Format 58)
    EIA_TIMING(1440,  38,124,1716,'-', 240, 4,3, 262,'-',239760,'i',16:9,   0x2,59),//720(1440) x  480i @239.76/240 (Format 59)
    EIA_TIMING(1280,1760, 40,3300,'+', 720, 5,5, 750,'+',23976, 'p',16:9,   0x1,60),//1280      x  720p @23.97/24   (Format 60)
    EIA_TIMING(1280,2420, 40,3960,'+', 720, 5,5, 750,'+',25000, 'p',16:9,   0x1,61),//1280      x  720p @25         (Format 61)
    EIA_TIMING(1280,1760, 40,3300,'-', 720, 5,5, 750,'+',29970, 'p',16:9,   0x1,62),//1280      x  720p @29.97/30   (Format 62)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4,5,1125,'+',119880,'p',16:9,   0x1,63),//1920      x 1080p @119.88/120 (Format 63)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4,5,1125,'+',100000,'p',16:9,   0x1,64),//1920      x 1080p @100        (Format 64)
    // Following modes are from CEA-861F
    EIA_TIMING(1280,1760, 40,3300,'+', 720, 5, 5, 750,'+', 23976,'p',  64:27,   0x1, 65),//1280      x 720p @23.98/24   (Format 65)
    EIA_TIMING(1280,2420, 40,3960,'+', 720, 5, 5, 750,'+', 25000,'p',  64:27,   0x1, 66),//1280      x 720p @25         (Format 66)
    EIA_TIMING(1280,1760, 40,3300,'+', 720, 5, 5, 750,'+', 29970,'p',  64:27,   0x1, 67),//1280      x 720p @29.97/30   (Format 67)
    EIA_TIMING(1280, 440, 40,1980,'+', 720, 5, 5, 750,'+', 50000,'p',  64:27,   0x1, 68),//1280      x 720p @50         (Format 68)
    EIA_TIMING(1280, 110, 40,1650,'+', 720, 5, 5, 750,'+', 59940,'p',  64:27,   0x1, 69),//1280      x 720p @59.94/60   (Format 69)
    EIA_TIMING(1280, 440, 40,1980,'+', 720, 5, 5, 750,'+',100000,'p',  64:27,   0x1, 70),//1280      x 720p @100        (Format 70)
    EIA_TIMING(1280, 110, 40,1650,'+', 720, 5, 5, 750,'+',119880,'p',  64:27,   0x1, 71),//1280      x 720p @119.88/120 (Format 71)
    EIA_TIMING(1920, 638, 44,2750,'+',1080, 4, 5,1125,'+', 23976,'p',  64:27,   0x1, 72),//1920      x1080p @23.98/24   (Format 72)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4, 5,1125,'+', 25000,'p',  64:27,   0x1, 73),//1920      x1080p @25         (Format 73)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4, 5,1125,'+', 29970,'p',  64:27,   0x1, 74),//1920      x1080p @29.97/30   (Format 74)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4, 5,1125,'+', 50000,'p',  64:27,   0x1, 75),//1920      x1080p @50         (Format 75)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4, 5,1125,'+', 59940,'p',  64:27,   0x1, 76),//1920      x1080p @59.94/60   (Format 76)
    EIA_TIMING(1920, 528, 44,2640,'+',1080, 4, 5,1125,'+',100000,'p',  64:27,   0x1, 77),//1920      x1080p @100        (Format 77)
    EIA_TIMING(1920,  88, 44,2200,'+',1080, 4, 5,1125,'+',119880,'p',  64:27,   0x1, 78),//1920      x1080p @119.88/120 (Format 78)
    EIA_TIMING(1680,1360, 40,3300,'+', 720, 5, 5, 750,'+', 23976,'p',  64:27,   0x1, 79),//1680      x 720p @23.98/24   (Format 79)
    EIA_TIMING(1680,1228, 40,3168,'+', 720, 5, 5, 750,'+', 25000,'p',  64:27,   0x1, 80),//1680      x 720p @25         (Format 80)
    EIA_TIMING(1680, 700, 40,2640,'+', 720, 5, 5, 750,'+', 29970,'p',  64:27,   0x1, 81),//1680      x 720p @29.97/30   (Format 81)
    EIA_TIMING(1680, 260, 40,2200,'+', 720, 5, 5, 750,'+', 50000,'p',  64:27,   0x1, 82),//1680      x 720p @50         (Format 82)
    EIA_TIMING(1680, 260, 40,2200,'+', 720, 5, 5, 750,'+', 59940,'p',  64:27,   0x1, 83),//1680      x 720p @59.94/60   (Format 83)
    EIA_TIMING(1680,  60, 40,2000,'+', 720, 5, 5, 825,'+',100000,'p',  64:27,   0x1, 84),//1680      x 720p @100        (Format 84)
    EIA_TIMING(1680,  60, 40,2000,'+', 720, 5, 5, 825,'+',119880,'p',  64:27,   0x1, 85),//1680      x 720p @119.88/120 (Format 85)
    EIA_TIMING(2560, 998, 44,3750,'+',1080, 4, 5,1100,'+', 23976,'p',  64:27,   0x1, 86),//2560      x1080p @23.98/24   (Format 86)
    EIA_TIMING(2560, 448, 44,3200,'+',1080, 4, 5,1125,'+', 25000,'p',  64:27,   0x1, 87),//2560      x1080p @25         (Format 87)
    EIA_TIMING(2560, 768, 44,3520,'+',1080, 4, 5,1125,'+', 29970,'p',  64:27,   0x1, 88),//2560      x1080p @29.97/30   (Format 88)
    EIA_TIMING(2560, 548, 44,3300,'+',1080, 4, 5,1125,'+', 50000,'p',  64:27,   0x1, 89),//2560      x1080p @50         (Format 89)
    EIA_TIMING(2560, 248, 44,3000,'+',1080, 4, 5,1100,'+', 59940,'p',  64:27,   0x1, 90),//2560      x1080p @59.94/60   (Format 90)
    EIA_TIMING(2560, 218, 44,2970,'+',1080, 4, 5,1250,'+',100000,'p',  64:27,   0x1, 91),//2560      x1080p @100        (Format 91)
    EIA_TIMING(2560, 548, 44,3300,'+',1080, 4, 5,1250,'+',119880,'p',  64:27,   0x1, 92),//2560      x1080p @119.88/120 (Format 92)
    EIA_TIMING(3840,1276, 88,5500,'+',2160, 8,10,2250,'+', 23976,'p',   16:9,   0x1, 93),//3840      x2160p @23.98/24   (Format 93)
    EIA_TIMING(3840,1056, 88,5280,'+',2160, 8,10,2250,'+', 25000,'p',   16:9,   0x1, 94),//3840      x2160p @25         (Format 94)
    EIA_TIMING(3840, 176, 88,4400,'+',2160, 8,10,2250,'+', 29970,'p',   16:9,   0x1, 95),//3840      x2160p @29.97/30   (Format 95)
    EIA_TIMING(3840,1056, 88,5280,'+',2160, 8,10,2250,'+', 50000,'p',   16:9,   0x1, 96),//3840      x2160p @50         (Format 96)
    EIA_TIMING(3840, 176, 88,4400,'+',2160, 8,10,2250,'+', 59940,'p',   16:9,   0x1, 97),//3840      x2160p @59.94/60   (Format 97)
    EIA_TIMING(4096,1020, 88,5500,'+',2160, 8,10,2250,'+', 23976,'p',256:135,   0x1, 98),//4096      x2160p @23.98/24   (Format 98)
    EIA_TIMING(4096, 968, 88,5280,'+',2160, 8,10,2250,'+', 25000,'p',256:135,   0x1, 99),//4096      x2160p @25         (Format 99)
    EIA_TIMING(4096,  88, 88,4400,'+',2160, 8,10,2250,'+', 29970,'p',256:135,   0x1,100),//4096      x2160p @29.97/30   (Format 100)
    EIA_TIMING(4096, 968, 88,5280,'+',2160, 8,10,2250,'+', 50000,'p',256:135,   0x1,101),//4096      x2160p @50         (Format 101)
    EIA_TIMING(4096,  88, 88,4400,'+',2160, 8,10,2250,'+', 59940,'p',256:135,   0x1,102),//4096      x2160p @59.94/60   (Format 102)
    EIA_TIMING(3840,1276, 88,5500,'+',2160, 8,10,2250,'+', 23976,'p',  64:27,   0x1,103),//3840      x2160p @23.98/24   (Format 103)
    EIA_TIMING(3840,1056, 88,5280,'+',2160, 8,10,2250,'+', 25000,'p',  64:27,   0x1,104),//3840      x2160p @25         (Format 104)
    EIA_TIMING(3840, 176, 88,4400,'+',2160, 8,10,2250,'+', 29970,'p',  64:27,   0x1,105),//3840      x2160p @29.97/30   (Format 105)
    EIA_TIMING(3840,1056, 88,5280,'+',2160, 8,10,2250,'+', 50000,'p',  64:27,   0x1,106),//3840      x2160p @50         (Format 106)
    EIA_TIMING(3840, 176, 88,4400,'+',2160, 8,10,2250,'+', 59940,'p',  64:27,   0x1,107),//3840      x2160p @59.94/60   (Format 107)
    // VIC 108-127 timings are from CTA-861-G_FINAL_revised_2018_Errata_2.pdf
    EIA_TIMING(1280, 960, 40, 2500,'+', 720, 5, 5, 750,'+', 47950,'p',   16:9,  0x1,108),//1280 x  720p @47.95/48   (Format 108)
    EIA_TIMING(1280, 960, 40, 2500,'+', 720, 5, 5, 750,'+', 47950,'p',  64:27,  0x1,109),//1280 x  720p @47.95/48   (Format 109)
    EIA_TIMING(1680, 810, 40, 2750,'+', 720, 5, 5, 750,'+', 47950,'p',  64:27,  0x1,110),//1680 x  720p @47.95/48   (Format 110)
    EIA_TIMING(1920, 638, 44, 2750,'+',1080, 4, 5,1125,'+', 47950,'p',   16:9,  0x1,111),//1920 x 1080p @47.95/48   (Format 111)
    EIA_TIMING(1920, 638, 44, 2750,'+',1080, 4, 5,1125,'+', 47950,'p',  64:27,  0x1,112),//1920 x 1080p @47.95/48   (Format 112)
    EIA_TIMING(2560, 998, 44, 3750,'+',1080, 4, 5,1100,'+', 47950,'p',  64:27,  0x1,113),//2560 x 1080p @47.95/48   (Format 113)    
    EIA_TIMING(3840,1276, 88, 5500,'+',2160, 8,10,2250,'+', 47950,'p',   16:9,  0x1,114),//3840 x 2160p @47.95/48   (Format 114)
    EIA_TIMING(4096,1020, 88, 5500,'+',2160, 8,10,2250,'+', 47950,'p',256:135,  0x1,115),//4096 x 2160p @47.95/48   (Format 115)
    EIA_TIMING(3840,1276, 88, 5500,'+',2160, 8,10,2250,'+', 47950,'p',  64:27,  0x1,116),//3840 x 2160p @47.95/48   (Format 116)
    EIA_TIMING(3840,1056, 88, 5280,'+',2160, 8,10,2250,'+',100000,'p',   16:9,  0x1,117),//3840 x 2160p @100        (Format 117)
    EIA_TIMING(3840, 176, 88, 4400,'+',2160, 8,10,2250,'+',119880,'p',   16:9,  0x1,118),//3840 x 2160p @119.88/120 (Format 118)
    EIA_TIMING(3840,1056, 88, 5280,'+',2160, 8,10,2250,'+',100000,'p',  64:27,  0x1,119),//3840 x 2160p @100        (Format 119)
    EIA_TIMING(3840, 176, 88, 4400,'+',2160, 8,10,2250,'+',119880,'p',  64:27,  0x1,120),//3840 x 2160p @119.88/120 (Format 120)
    EIA_TIMING(5120,1996, 88, 7500,'+',2160, 8,10,2200,'+', 23976,'p',  64:27,  0x1,121),//5120 x 2160p @23.98/24   (Format 121)
    EIA_TIMING(5120,1696, 88, 7200,'+',2160, 8,10,2200,'+', 25000,'p',  64:27,  0x1,122),//5120 x 2160p @25         (Format 122)
    EIA_TIMING(5120, 664, 88, 6000,'+',2160, 8,10,2200,'+', 29970,'p',  64:27,  0x1,123),//5120 x 2160p @29.97/30   (Format 123)
    EIA_TIMING(5120, 746, 88, 6250,'+',2160, 8,10,2475,'+', 47950,'p',  64:27,  0x1,124),//5120 x 2160p @47.95/48   (Format 124)    
    EIA_TIMING(5120,1096, 88, 6600,'+',2160, 8,10,2250,'+', 50000,'p',  64:27,  0x1,125),//5120 x 2160p @50         (Format 125)    
    EIA_TIMING(5120, 164, 88, 5500,'+',2160, 8,10,2250,'+', 59940,'p',  64:27,  0x1,126),//5120 x 2160p @59.94/60   (Format 126)    
    EIA_TIMING(5120,1096, 88, 6600,'+',2160, 8,10,2250,'+',100000,'p',  64:27,  0x1,127),//5120 x 2160p @100        (Format 127)
    // VIC 128-192 are Forbidden and should be never used. But to simplify the SVD access, put a default timing here. 
    // We can remove these after adding a function to access CEA Timings.
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 128)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 129)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 130)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 131)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 132)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 133)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 134)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 135)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 136)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 137)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 138)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 139)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 140)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 141)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 142)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 143)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 144)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 145)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 146)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 147)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 148)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 149)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 150)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 151)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 152)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 153)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 154)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 155)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 156)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 157)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 158)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 159)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 160)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 161)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 162)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 163)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 164)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 165)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 166)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 167)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 168)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 169)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 170)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 171)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 172)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 173)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 174)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 175)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 176)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 177)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 178)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 179)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 180)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 181)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 182)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 183)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 184)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 185)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 186)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 187)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 188)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 189)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 190)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 191)
    EIA_TIMING( 640,  16, 96, 800,'-', 480,10,2, 525,'-', 59940,'p', 4:3, 0x1, 0),//640 x 480p @59.94/60 //Forbidden (Format 192)
    // VIC 193-219 timings are from CTA-861-G_FINAL_revised_2018_Errata_2.pdf
    EIA_TIMING( 5120, 164, 88, 5500,'+',2160, 8,10,2250,'+',120000,'p',  64:27,0x1,193),// 5120 x 2160p @119.88/120 (Format 193)
    EIA_TIMING( 7680,2552,176,11000,'+',4320,16,20,4500,'+', 23976,'p',   16:9,0x1,194),// 7680 x 4320p @23.98/24   (Format 194)
    EIA_TIMING( 7680,2352,176,10800,'+',4320,16,20,4400,'+', 25000,'p',   16:9,0x1,195),// 7680 x 4320p @25         (Format 195)
    EIA_TIMING( 7680, 552,176, 9000,'+',4320,16,20,4400,'+', 29970,'p',   16:9,0x1,196),// 7680 x 4320p @29.97/30   (Format 196)
    EIA_TIMING( 7680,2552,176,11000,'+',4320,16,20,4500,'+', 47950,'p',   16:9,0x1,197),// 7680 x 4320p @47.95/48   (Format 197)
    EIA_TIMING( 7680,2352,176,10800,'+',4320,16,20,4400,'+', 50000,'p',   16:9,0x1,198),// 7680 x 4320p @50         (Format 198)
    EIA_TIMING( 7680, 552,176, 9000,'+',4320,16,20,4400,'+', 59940,'p',   16:9,0x1,199),// 7680 x 4320p @59.94/60   (Format 199)
    EIA_TIMING( 7680,2112,176,10560,'+',4320,16,20,4500,'+',100000,'p',   16:9,0x1,200),// 7680 x 4320p @100        (Format 200)
    EIA_TIMING( 7680, 352,176, 8000,'+',4320,16,20,4500,'+',119880,'p',   16:9,0x1,201),// 7680 x 4320p @119.88/120 (Format 201)
    EIA_TIMING( 7680,2552,176,11000,'+',4320,16,20,4500,'+', 23976,'p',  64:27,0x1,202),// 7680 x 4320p @23.98/24   (Format 202)
    EIA_TIMING( 7680,2352,176,10800,'+',4320,16,20,4400,'+', 25000,'p',  64:27,0x1,203),// 7680 x 4320p @25         (Format 203)
    EIA_TIMING( 7680, 552,176, 9000,'+',4320,16,20,4400,'+', 29970,'p',  64:27,0x1,204),// 7680 x 4320p @29.97/30   (Format 204)
    EIA_TIMING( 7680,2552,176,11000,'+',4320,16,20,4500,'+', 47950,'p',  64:27,0x1,205),// 7680 x 4320p @47.95/48   (Format 205)
    EIA_TIMING( 7680,2352,176,10800,'+',4320,16,20,4400,'+', 50000,'p',  64:27,0x1,206),// 7680 x 4320p @50         (Format 206)
    EIA_TIMING( 7680, 552,176, 9000,'+',4320,16,20,4400,'+', 59940,'p',  64:27,0x1,207),// 7680 x 4320p @59.94/60   (Format 207)
    EIA_TIMING( 7680,2112,176,10560,'+',4320,16,20,4500,'+',100000,'p',  64:27,0x1,208),// 7680 x 4320p @100        (Format 208)
    EIA_TIMING( 7680, 352,176, 8800,'+',4500,16,20,4950,'+',119880,'p',  64:27,0x1,209),// 7680 x 4320p @119.88/120 (Format 209)
    EIA_TIMING(10240,1492,176,12500,'+',4320,16,20,4950,'+', 23976,'p',  64:27,0x1,210),//10240 x 4320p @23.98/24   (Format 210)
    EIA_TIMING(10240,2492,176,13500,'+',4320,16,20,4400,'+', 25000,'p',  64:27,0x1,211),//10240 x 4320p @25         (Format 211)
    EIA_TIMING(10240, 288,176,11000,'+',4320,16,20,4500,'+', 29970,'p',  64:27,0x1,212),//10240 x 4320p @29.97/30   (Format 212)
    EIA_TIMING(10240,1492,176,12500,'+',4320,16,20,4950,'+', 47950,'p',  64:27,0x1,213),//10240 x 4320p @47.95/48   (Format 213)
    EIA_TIMING(10240,2492,176,13500,'+',4320,16,20,4400,'+', 44000,'p',  64:27,0x1,214),//10240 x 4320p @50         (Format 214)
    EIA_TIMING(10240, 288,176,11000,'+',4320,16,20,4500,'+', 59940,'p',  64:27,0x1,215),//10240 x 4320p @59.94/60   (Format 215)
    EIA_TIMING(10240,2192,176,13200,'+',4320,16,20,4500,'+',100000,'p',  64:27,0x1,216),//10240 x 4320p @100        (Format 216)
    EIA_TIMING(10240, 288,176,11000,'+',4320,16,20,4500,'+',119880,'p',  64:27,0x1,217),//10240 x 4320p @119.88/120 (Format 217)
    EIA_TIMING( 4096, 800, 88, 5280,'+',2160, 8,10,2250,'+',100000,'p',256:135,0x1,218),// 4096 x 2160p @100        (Format 218)
    EIA_TIMING( 4096,  88, 88, 4400,'+',2160, 8,10,2250,'+',119880,'p',256:135,0x1,219),// 4096 x 2160p @119.88/120 (Format 219)
    // 220-255 Reserved for the Future
    // the end                                                         
    EIA_TIMING(0,0,0,0,'-',0,0,0,0,'-',0,'p',4:3,0,0)
};
static NvU32 MAX_CEA861B_FORMAT = sizeof(EIA861B)/sizeof(EIA861B[0]) - 1;

static const NvU32 EIA861B_DUAL_ASPECT_VICS[][2] =
{ 
    { 2, 3 },   // 720x480p         59.94Hz/60Hz
    { 4, 69 },  // 1280x720p        59.94Hz/60Hz
    { 6, 7 },   // 720(1440)x480i   59.94Hz/60Hz
    { 8, 9 },   // 720(1440)x240p   59.94Hz/60Hz

    { 10, 11 }, // 2880x480i        59.94Hz/60Hz
    { 12, 13 }, // 2880x240p        59.94Hz/60Hz
    { 14, 15 }, // 1440x480p        59.94Hz/60Hz
    { 16, 76 }, // 1920x1080p       59.94Hz/60Hz
    { 17, 18 }, // 720x576p         50Hz
    { 19, 68 }, // 1280x720p        50Hz

    { 21, 22 }, // 720(1440)x576i   50Hz
    { 23, 24 }, // 720(1440)x288p   50Hz
    { 25, 26 }, // 2880x576i        50Hz
    { 27, 28 }, // 2880x288p        50Hz
    { 29, 30 }, // 1440x576p        50Hz

    { 31, 75 }, // 1920x1080p       50Hz
    { 32, 72 }, // 1920x1080p       23.98Hz/24Hz
    { 33, 73 }, // 1920x1080p       25Hz
    { 34, 74 }, // 1920x1080p       29.97Hz/30Hz
    { 35, 36 }, // 2880x480p        59.94Hz/60Hz
    { 37, 38 }, // 2880x576p        50Hz

    { 41, 70 }, // 1280x720p        100Hz
    { 42, 43 }, // 720x576p         100Hz
    { 44, 45 }, // 720(1440)x576i   100Hz
    { 47, 71 }, // 1280x720p        119.88/120Hz
    { 48, 49 }, // 720x480p         119.88/120Hz

    { 50, 51 }, // 720(1440)x480i   119.88/120Hz
    { 52, 53 }, // 720x576p         200Hz
    { 54, 55 }, // 720(1440)x576i   200Hz 
    { 56, 57 }, // 720x480p         239.76/240Hz
    { 58, 59 }, // 720(1440)x480i   239.76/240Hz

    { 60, 65 }, // 1280x720p        23.98Hz/24Hz
    { 61, 66 }, // 1280x720p        25Hz
    { 62, 67 }, // 1280x720p        29.97Hz/30Hz
    { 63, 78 }, // 1920x1080p       119.88/120Hz
    { 64, 77 }, // 1920x1080p       100Hz

    { 93, 103 }, // 3840x2160p      23.98Hz/24Hz
    { 94, 104 }, // 3840x2160p      25Hz
    { 95, 105 }, // 3840x2160p      29.97Hz/30Hz
    { 96, 106 }, // 3840x2160p      50Hz
    { 97, 107 }, // 3840x2160p      59.94Hz/60Hz
};
static NvU32 MAX_EIA861B_DUAL_ASPECT_VICS = sizeof(EIA861B_DUAL_ASPECT_VICS) / sizeof(EIA861B_DUAL_ASPECT_VICS[0]);

static const NVT_TIMING PSF_TIMING[]=
{
    NVT_TIMING( 1920,600, 88,2750,'+', 540, 2,5,562,'+',47952,'i',16:9,       0x1, 1, "ITU-R BT.709-5:1080i/24Psf"),//1920x1080i @47.952Hz  | 24/PsF | ITU-R BT.709-5
    NVT_TIMING( 1920,488, 88,2640,'+', 540, 2,5,562,'+',49950,'i',16:9,       0x1, 2, "ITU-R BT.709-5:1080i/25Psf"),//1920x1080i @49.950Hz  | 25/PsF | ITU-R BT.709-5

    // the end
    EIA_TIMING(0,0,0,0,'-',0,0,0,0,'-',0,'p',4:3,0,0)
};
static NvU32 MAX_PSF_FORMAT = sizeof(PSF_TIMING)/sizeof(PSF_TIMING[0]) - 1;

static const NVT_TIMING HDMI_EXT_4Kx2K_TIMING[]=
{
    HDMI_EXT_TIMING( 3840, 176, 88,4400,'+', 2160, 8,10,2250,'+',29970,'p',16:9,       0x1, NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx30Hz,       "HDMI EXT: 3840x2160x29.97/30hz"),//3840x2160 @29.97/30Hz VIC: 0x01
    HDMI_EXT_TIMING( 3840,1056, 88,5280,'+', 2160, 8,10,2250,'+',25000,'p',16:9,       0x1, NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx25Hz,       "HDMI EXT: 3840x2160x25hz"),      //3840x2160 @25Hz       VIC: 0x02
    HDMI_EXT_TIMING( 3840,1276, 88,5500,'+', 2160, 8,10,2250,'+',23976,'p',16:9,       0x1, NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx24Hz,       "HDMI EXT: 3840x2160x23.98/24hz"),//3840x2160 @23.98/24Hz VIC: 0x03
    HDMI_EXT_TIMING( 4096,1020, 88,5500,'+', 2160, 8,10,2250,'+',24000,'p',16:9,       0x1, NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx24Hz_SMPTE, "HDMI EXT: 4096x2160x24hzSmpte"), //4096x2160 @24Hz       VIC: 0x04

    // the end
    EIA_TIMING(0,0,0,0,'-',0,0,0,0,'-',0,'p',4:3,0,0)
};
static NvU32 MAX_HDMI_EXT_4Kx2K_FORMAT = sizeof(HDMI_EXT_4Kx2K_TIMING)/sizeof(HDMI_EXT_4Kx2K_TIMING[0]) - 1;

// HDMI 1.4a mandatory 3D video formats. 
// From HDMI 1.4a specification page 147 of 201, table 8-15. And HDMI 1.4a Complaince test specification page 190.
static const HDMI3DDETAILS   HDMI_MANDATORY_3D_FORMATS[] = 
{
    {32, NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK | NVT_HDMI_3D_SUPPORTED_TOPBOTTOM_MASK, 0},       // 1920 x 1080p @ 24 Hz
    { 4, NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK | NVT_HDMI_3D_SUPPORTED_TOPBOTTOM_MASK, 0},       // 1280 x  720p @ 60 Hz
    {19, NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK | NVT_HDMI_3D_SUPPORTED_TOPBOTTOM_MASK, 0},       // 1280 x  720p @ 50 Hz
    { 5, NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEHALF_MASK, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH},       // 1920 x 1080i @ 60 Hz
    {20, NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEHALF_MASK, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH}        // 1920 x 1080i @ 50 Hz
};
static NvU32 MAX_HDMI_MANDATORY_3D_FORMAT = sizeof(HDMI_MANDATORY_3D_FORMATS) / sizeof(HDMI_MANDATORY_3D_FORMATS[0]);
static const NVT_VIDEO_INFOFRAME DEFAULT_VIDEO_INFOFRAME = {/*header*/2,2,13, /*byte1*/0, /*byte2*/0x8, /*byte3*/0, /*byte4*/0, /*byte5*/0, /*byte6~13*/0,0,0,0,0,0,0,0};
static const NVT_AUDIO_INFOFRAME DEFAULT_AUDIO_INFOFRAME = {/*header*/4,1,10, /*byte1*/0, /*byte2*/0,   /*byte3*/0, /*byte*/0,  /*byte5*/0, /*byte6~10*/0,0,0,0,0};

CODE_SEGMENT(PAGE_DD_CODE)
static NvU8 
getExistedCTATimingSeqNumber(
    NVT_EDID_INFO *pInfo, 
    enum NVT_TIMING_TYPE timingType)
{
    NvU8 count = 0;
    NvU8 i     = 0;

    switch (timingType)
    {
    case NVT_TYPE_CTA861_DID_T7:
    case NVT_TYPE_CTA861_DID_T8:
    case NVT_TYPE_CTA861_DID_T10:
        break;
    default:
        return count;
    }

    for (i = 0; i< pInfo->total_timings; i++)
    {
        if (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == timingType)
            ++count;
    }

    return count;
}

// parse the 861 detailed timing info
CODE_SEGMENT(PAGE_DD_CODE)
void parse861ExtDetailedTiming(NvU8 *pEdidExt,
                               NvU8 basicCaps,
                               NVT_EDID_INFO *pInfo)
{
    NvU32 count = 0;
    EIA861EXTENSION *pEIA861 = (EIA861EXTENSION *) pEdidExt;
    DETAILEDTIMINGDESCRIPTOR *pDTD;
    NVT_TIMING newTiming;

    // sanity check for CEA ext block
    if ((pEIA861->tag != 0x2) || (0 == pEIA861->offset) || (NVT_CEA861_REV_NONE == pEIA861->revision))
    {
        // no CEA ext block, return
        return;
    }

    // Get all detailed timings in CEA ext block
    pDTD = (DETAILEDTIMINGDESCRIPTOR *)&pEdidExt[pEIA861->offset];

    while((NvU8 *)pDTD + sizeof(DETAILEDTIMINGDESCRIPTOR) < (pEdidExt + sizeof(EDIDV1STRUC)) &&
          pDTD->wDTPixelClock != 0)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseEdidDetailedTimingDescriptor((NvU8 *)pDTD,
                                              &newTiming) == NVT_STATUS_SUCCESS)
        {
            NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name),
                         "CTA-861Long:%5dx%4dx%3d.%03dHz/%s",
                         (int)newTiming.HVisible,
                         (int)((newTiming.interlaced ? 2 : 1) * newTiming.VVisible),
                         (int)newTiming.etc.rrx1k/1000,
                         (int)newTiming.etc.rrx1k%1000,
                         (newTiming.interlaced ? "I":"P"));
            newTiming.etc.name[sizeof(newTiming.etc.name)-1] = '\0';
            newTiming.etc.status = NVT_STATUS_EDID_EXT_DTDn(++count);

            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }
        }
        pDTD ++;
    }
}

// parse the 861B short timing descriptor
CODE_SEGMENT(PAGE_DD_CODE)
void parse861bShortTiming(NVT_EDID_CEA861_INFO *pExt861,
                          void *pRawInfo,
                          NVT_CTA861_ORIGIN flag)
{
    NvU32 i;
    NvU32 vic, bytePos, bitPos;
    NVT_TIMING               newTiming;
    NVT_HDMI_FORUM_INFO     *pHfvs          = NULL;
    NVT_EDID_INFO           *pInfo          = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20   = NULL;

    NvU8                    *pVic           = pExt861->video;
    NvU32                    total_svd      = pExt861->total_svd;    
    NvU8                    *pYuv420Map     = pExt861->valid.y420cmdb ? pExt861->map_y420cmdb : NULL;
    NvU8                     yuv420MapCount = pExt861->total_y420cmdb;

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo    = (NVT_EDID_INFO *)pRawInfo;
        pHfvs    = &pInfo->hdmiForumInfo;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pHfvs    = &pDisplayID20->vendor_specific.hfvs;
    }
    else
    {
        return;
    }

    for (i = 0; i < total_svd; i++)
    {
        vic = NVT_GET_CTA_8BIT_VIC(pVic[i]);        
        
        if (vic == 0 || vic > MAX_CEA861B_FORMAT)
            continue;

        // assign corresponding CEA format's timing from pre-defined CE timing table, EIA861B
        newTiming = EIA861B[vic-1];
        newTiming.etc.status = NVT_STATUS_EDID_861STn(vic);

        // set CEA format to location of _CEA_FORMAT. _CEA_FORMAT isn't set in pre-defined CE timing from 
        // EIA861B table
        if (NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status) !=
            NVT_CEA861_640X480P_59940HZ_4X3)
        {
            // Although IT 640x480 video timing has a CE id, it is not a CE timing. See 3.1 
            // "General Video Format Requirements" section in CEA-861-E spec
            NVT_SET_CEA_FORMAT(newTiming.etc.status,
                               NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status));
        }

        // calculate the pixel clock
        newTiming.pclk  = RRx1kToPclk(&newTiming);
 
        if ((vic <= 64) && (pVic[i] & NVT_CTA861_VIDEO_NATIVE_MASK))
        {
            NVT_SET_NATIVE_TIMING_FLAG(newTiming.etc.status);
        }
        NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name),
                     "CTA-861G:#%3d:%5dx%4dx%3d.%03dHz/%s", (int)vic,
                     (int)newTiming.HVisible,
                     (int)((newTiming.interlaced ? 2 : 1)*newTiming.VVisible),
                     (int)newTiming.etc.rrx1k/1000, (int)newTiming.etc.rrx1k%1000,
                     (newTiming.interlaced ? "I":"P"));
        newTiming.etc.name[sizeof(newTiming.etc.name)-1] = '\0';

        // if yuv420 is supported in the video SVDs, it is indicated by CMDB bitmap
        bytePos = i / (8 * sizeof(NvU8));
        if (bytePos < yuv420MapCount)
        {
            bitPos  = 1 << (i % (8 * sizeof(NvU8)));
            if (pYuv420Map[bytePos] & bitPos)
            {
                // pHfvs->dcXXX are only for YCbCr420; when bitPos is set, 8bpc yuv420 always supported
                UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1,
                                           pHfvs->dc_30bit_420,
                                           pHfvs->dc_36bit_420, 0,
                                           pHfvs->dc_48bit_420);
            }
        }

        // Y420CMDB with L == 1, implies yuv420MapCount == 0 but all SVDs support 420
        if (pYuv420Map && yuv420MapCount == 0)
        {
            UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1,
                                       pHfvs->dc_30bit_420,
                                       pHfvs->dc_36bit_420, 0,
                                       pHfvs->dc_48bit_420);
        }

        if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
        {
            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }
        }
        else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
        {
           newTiming.etc.flag |= NVT_FLAG_DISPLAYID_2_0_TIMING;

           if (!assignNextAvailableDisplayId20Timing(pDisplayID20, &newTiming))
            {
                break;
            }
        }
    }
}

// parse the 861B short Yuv420 timing descriptor
CODE_SEGMENT(PAGE_DD_CODE)
void parse861bShortYuv420Timing(NVT_EDID_CEA861_INFO *pExt861,
                                void *pRawInfo,
                                NVT_CTA861_ORIGIN flag)
{
    NvU32 i;
    NvU8 vic;
    NVT_TIMING              newTiming;
    NVT_HDMI_FORUM_INFO    *pHfvs         = NULL;
    NVT_EDID_INFO          *pInfo         = NULL;
    NVT_DISPLAYID_2_0_INFO *pDisplayID20  = NULL;
    NvU8                   *pYuv420Vic    = pExt861->svd_y420vdb;
    NvU32                   total_y420vdb = pExt861->total_y420vdb;
    NvU8                   *pVdb          = pExt861->video;
    NvU32                   total_svd     = pExt861->total_svd;
    NvU32                   total_timings = 0;

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo = (NVT_EDID_INFO *)pRawInfo;
        pHfvs = &pInfo->hdmiForumInfo;
        total_timings = pInfo->total_timings;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pHfvs = &pDisplayID20->vendor_specific.hfvs;
        total_timings = pDisplayID20->total_timings;
    }
    else 
    {
        return;
    }

    if (total_timings == 0)
    {
        return;
    }


    for (i = 0; i < total_y420vdb; i++)
    {
        vic = NVT_GET_CTA_8BIT_VIC(pYuv420Vic[i]);
        
        if (vic == 0 || vic > MAX_CEA861B_FORMAT)
            continue;

        // assign corresponding CEA format's timing from pre-defined CE timing table, EIA861B
        newTiming = EIA861B[vic-1];
        
        // if yuv420 is supported in the video SVDs, it is indicated by yuv420vdb
        if(total_svd > 0)
        {
            NvU8 idx, j;
            NvBool bFound = NV_FALSE;
            for (idx=0; idx < total_svd; idx++)
            {
                if (pVdb[idx] == vic)
                {
                    for (j=0; j < total_timings; j++)
                    {
                        NVT_TIMING *timing = NULL;

                        if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
                        {
                            timing = &pInfo->timing[j];
                        }
                        else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
                        {
                            timing = &pDisplayID20->timing[j];
                        }

                        if (NvTiming_IsTimingExactEqual(timing, &newTiming))
                        {
                            bFound = NV_TRUE;
                            // we found one in pExt861->video[]. pHfvs->dcXXX are only for YCbCr420, so we can support:
                            // 1. 8bpc yuv420 always supported. 
                            // 2. only add yuv420 and its deep colour caps into Video Data Block
                            UPDATE_BPC_FOR_COLORFORMAT(timing->etc.yuv420, 0, 1,
                                                       pHfvs->dc_30bit_420,
                                                       pHfvs->dc_36bit_420, 0,
                                                       pHfvs->dc_48bit_420);
                            break;
                        }
                    }
                }
            }            
            if (bFound) continue;
        }

        newTiming.etc.status = NVT_STATUS_EDID_861STn(vic);

        // set CEA format to location of _CEA_FORMAT. _CEA_FORMAT isn't set in pre-defined CE timing from 
        // EIA861B table
        if (NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status) !=
            NVT_CEA861_640X480P_59940HZ_4X3)
        {
            // Although IT 640x480 video timing has a CE id, it is not a CE timing. See 3.1 
            // "General Video Format Requirements" section in CEA-861-E spec
            NVT_SET_CEA_FORMAT(newTiming.etc.status,
                               NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status));
        }

        // calculate the pixel clock
        newTiming.pclk = RRx1kToPclk(&newTiming);

        // From CTA-861-F: By default, Y420VDB SVDs, when present in the EDID, shall be less preferred than all regular Video Data Block SVDs. 
        // So it should use normal VIC code without native flag.
        //if ((vic <= 64) && (pVic[i] & NVT_CTA861_VIDEO_NATIVE_MASK))
        //{
        //     NVT_SET_NATIVE_TIMING_FLAG(newTiming.etc.status);
        //}
        NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name),
                     "CTA-861G:#%3d:%5dx%4dx%3d.%03dHz/%s", (int)vic,
                     (int)newTiming.HVisible,
                     (int)((newTiming.interlaced ? 2 : 1)*newTiming.VVisible),
                     (int)newTiming.etc.rrx1k/1000, (int)newTiming.etc.rrx1k%1000,
                     (newTiming.interlaced ? "I":"P"));
        newTiming.etc.name[sizeof(newTiming.etc.name)-1] = '\0';

        // update supported color space; any VICs enumerated in the Y420VDB are yuv420 only modes
        // update 8bpc supported color space; other bpc updated once VSDB is parsed

        // pHfvs->dcXXX are only for YCbCr420; when Vic enumerated here, 8bpc yuv420 always supported
        UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1,
                                   pHfvs->dc_30bit_420,
                                   pHfvs->dc_36bit_420, 0,
                                   pHfvs->dc_48bit_420);

        if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
        {
            if (!assignNextAvailableTiming(pInfo, &newTiming))
            {
                break;
            }
        }
        else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
        {
            newTiming.etc.flag |= NVT_FLAG_DISPLAYID_2_0_TIMING;

            if (assignNextAvailableDisplayId20Timing(pDisplayID20, &newTiming))
            {
                
                break;
            }
        }
    }
}

// Currently, the SVR both used in the NVRDB and VFPDB.
// "One particular application is a Sink that prefers a Video Format that is not listed as an SVD in a VDB
// but instead listed in a YCBCR 4:2:0 Video Data Block"
CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861NativeOrPreferredTiming(NVT_EDID_CEA861_INFO *pExt861,
                                           void *pRawInfo,
                                           NVT_CTA861_ORIGIN flag)
{
    NvU32 isMatch,i,j = 0;

    NVT_TIMING               preferTiming;
    NVT_EDID_INFO           *pInfo         = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20  = NULL;
    NvU8                     nativeSvr     = 0;
    NvU8                    *pSvr          = pExt861->svr_vfpdb;
    NvU8                     totalSvr      = pExt861->total_svr;
    NvU8                     kth           = 0;
    NvU8                     extKth        = 0;
    NvU8                     DTDCount      = 0;
    NvU8                     extDTDCount   = 0;
    NvU8                     DIDT7Count    = 0;
    NvU8                     DIDT10Count   = 0;

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo = (NVT_EDID_INFO *)pRawInfo;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
    }
    else 
    {
        return;
    }

    // finding all the DTD in Base 0 or CTA861
    if (flag == FROM_CTA861_EXTENSION)
    {
        // get the NVRDB, from the spec this native resolution has more high priority than others
        if (pExt861->valid.NVRDB == 1)
        {
            nativeSvr = pExt861->native_video_resolution_db.native_svr;
            totalSvr = 1;
        }

        for (j = 0; j < pInfo->total_timings; j++)
        {
            if (NVT_IS_DTD(pInfo->timing[j].etc.status))                  DTDCount++;
            else if (NVT_IS_EXT_DTD(pInfo->timing[j].etc.status))         extDTDCount++;
            else if (NVT_IS_CTA861_DID_T7(pInfo->timing[j].etc.status))   DIDT7Count++;
            else if (NVT_IS_CTA861_DID_T10(pInfo->timing[j].etc.status))  DIDT10Count++;
        }
    }

    // this only handle single SVR right now
    for (i = 0; i < totalSvr; i++)
    {
        NvU8 svr = 0;
        NvU8 vic = 0;

        if (pExt861->valid.NVRDB == 1)
            svr = nativeSvr;
        else
            svr = pSvr[i];

        // Reserved
        if (svr == 0 || svr == 128 || (svr >= 176 && svr <= 192) || svr == 255)
            continue;
        
        // Interpret as the Kth 18-byte DTD, where K = SVR - 128 (for K = 1 to 16) in both base0 and CTA block
        if (svr >= 129 && svr <= 144)
        {
            kth = svr - 128;
            // only base EDID and CTA861 can support 18bytes 
            if (flag == FROM_CTA861_EXTENSION)
            {
                for (j = 0; j < pInfo->total_timings; j++)
                {
                    if (kth <= DTDCount)
                    {
                        if (NVT_IS_DTDn(pInfo->timing[j].etc.status, kth))  
                            break;
                    }
                    else
                    {
                        extKth = kth - DTDCount;
                        if (NVT_IS_EXT_DTDn(pInfo->timing[j].etc.status, extKth)) 
                            break;
                    }
                }

                if (pExt861->valid.NVRDB == 1)
                    pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_NATIVE_TIMING;
                else  
                    pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_PREFERRED_TIMING;
            }
        }
        else if (svr >= 145 && svr <= 160)
        {
            // Interpret as the Nth 20-byte DTD or 6- or 7-byte CVT-based descriptor
            // where N = SVR – 144 (for N = 1 to 16)
            kth = svr - 144;

            if (flag == FROM_CTA861_EXTENSION)
            {
                for (j = 0; j < pInfo->total_timings; j++)
                {
                    if (kth <= DIDT7Count) // pick the Nth 20-byte first
                    {
                        if (NVT_IS_CTA861_DID_T7n(pInfo->timing[j].etc.status, kth))
                            break;
                    }
                    else
                    {
                        extKth = kth - DIDT7Count; // pick the T10 CVT-based timing then
                        if (NVT_IS_CTA861_DID_T10n(pInfo->timing[j].etc.status, extKth))
                            break;
                    } 
                }

                if (pExt861->valid.NVRDB == 1)
                    pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_NATIVE_TIMING;
                else  
                    pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_PREFERRED_TIMING;
            }            
        }
        else if (svr >= 161 && svr <= 175)
        {
            // Interpret as the video format indicated by the first VFD of the first VFDB with Frame Rates of Rate Index N
            // where N = SVR - 160 (for N = 1 to 15)
            break;
        }
        else if (svr == 254)
        {
            // Interpret as the timing format indicated by the first code of the first T8VTDB
            if (flag == FROM_CTA861_EXTENSION)
            {
                for (j = 0; j < pInfo->total_timings; j++)
                {
                    if (NVT_IS_CTA861_DID_T8_1(pInfo->timing[j].etc.status))
                    {
                        if (pExt861->valid.NVRDB == 1)
                            pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_NATIVE_TIMING;
                        else  
                            pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_PREFERRED_TIMING;
                        break;
                    }
                }
            }
            break;
        }
        else // assign corresponding CEA format's timing from pre-defined CE timing table, EIA861B
        {    
            // ( SVR >= 1 and SVR <= 127) and (SVR >= 193 and SVR <= 253)
            vic = NVT_GET_CTA_8BIT_VIC(svr);
            preferTiming = EIA861B[vic-1];

            if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
            {
                for (j = 0; j < pInfo->total_timings; j++)
                {            
                    isMatch = NvTiming_IsTimingExactEqual(&pInfo->timing[j], &preferTiming);
                    if (isMatch && (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[j].etc.status) == NVT_TYPE_EDID_861ST))
                    {
                        if (pExt861->valid.NVRDB == 1)
                            pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_NATIVE_TIMING;
                        else  
                            pInfo->timing[j].etc.flag |= NVT_FLAG_CTA_PREFERRED_TIMING;
                        break;
                    }
                }    
            }
            else if (flag == FROM_DISPLAYID_20_DATA_BLOCK) 
            {                
                for (j = 0; j < pDisplayID20->total_timings; j++)
                {
                    isMatch = NvTiming_IsTimingExactEqual(&pDisplayID20->timing[j], &preferTiming);
                    if (isMatch && (NVT_GET_TIMING_STATUS_TYPE(pDisplayID20->timing[j].etc.status) == NVT_TYPE_EDID_861ST))
                    {
                        if (pExt861->valid.NVRDB == 1)
                            pDisplayID20->timing[j].etc.flag |= NVT_FLAG_CTA_NATIVE_TIMING | NVT_FLAG_DISPLAYID_2_0_TIMING;
                        else  
                            pDisplayID20->timing[j].etc.flag |= NVT_FLAG_CTA_PREFERRED_TIMING | NVT_FLAG_DISPLAYID_2_0_TIMING;
                        break;
                    }
                }
            }
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCea861HdrStaticMetadataDataBlock(NVT_EDID_CEA861_INFO *pExt861,
                                           void *pRawInfo,
                                           NVT_CTA861_ORIGIN flag)
{
    NVT_EDID_INFO           *pInfo         = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20  = NULL;
    NVT_HDR_STATIC_METADATA *pHdrInfo      = NULL;

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo = (NVT_EDID_INFO *)pRawInfo;
        pHdrInfo = &pInfo->hdr_static_metadata_info;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pHdrInfo = &pDisplayID20->cta.hdrInfo;
    }
    else
    {
        return;
    }

    if (pExt861 == NULL || pHdrInfo == NULL)
    {
        return;
    }

    // Parse the EOTF capability information. It's possible to have multiple EOTF
    if (pExt861->hdr_static_metadata.byte1 & NVT_CEA861_EOTF_GAMMA_SDR)
    {
        pHdrInfo->supported_eotf.trad_gamma_sdr_eotf = 1;
    }
    if (pExt861->hdr_static_metadata.byte1 & NVT_CEA861_EOTF_GAMMA_HDR)
    {
        pHdrInfo->supported_eotf.trad_gamma_hdr_eotf = 1;
    }
    if (pExt861->hdr_static_metadata.byte1 & NVT_CEA861_EOTF_SMPTE_ST2084)
    {
        pHdrInfo->supported_eotf.smpte_st_2084_eotf = 1;
    }
    if (pExt861->hdr_static_metadata.byte1 & NVT_CEA861_EOTF_FUTURE)
    {
        pHdrInfo->supported_eotf.future_eotf = 1;
    }

    // Parse the static metadata descriptor
    if (pExt861->hdr_static_metadata.byte2)
    {
        pHdrInfo->static_metadata_type = 1;
    }
    else
    {
        pHdrInfo->static_metadata_type = 0;
    }

    pHdrInfo->max_cll  = pExt861->hdr_static_metadata.byte3 & NVT_CEA861_MAX_CLL_MASK;
    pHdrInfo->max_fall = pExt861->hdr_static_metadata.byte4 & NVT_CEA861_MAX_FALL_MASK;
    pHdrInfo->min_cll  = pExt861->hdr_static_metadata.byte5 & NVT_CEA861_MIN_CLL_MASK;
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCea861DvStaticMetadataDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag)
{
    NvU32 vsvdbVersion = 0;
    NVT_DV_STATIC_METADATA_TYPE0   *pDvType0 = NULL;
    NVT_DV_STATIC_METADATA_TYPE1   *pDvType1 = NULL;
    NVT_DV_STATIC_METADATA_TYPE1_1 *pvDvType1_1 = NULL;
    NVT_DV_STATIC_METADATA_TYPE2   *pDvType2 = NULL;

    NVT_EDID_INFO           *pInfo         = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20  = NULL;
    NVT_DV_STATIC_METADATA  *pDvInfo       = NULL;

    if (pExt861 == NULL || pRawInfo == NULL)
    {
        return;
    }

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo = (NVT_EDID_INFO *)pRawInfo;
        pDvInfo = &pInfo->dv_static_metadata_info;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pDvInfo = &pDisplayID20->cta.dvInfo;
    }
    else
    {
        return;
    }

    if(pExt861->vsvdb.ieee_id != NVT_CEA861_DV_IEEE_ID)
    {
        return;
    }


    //init
    NVMISC_MEMSET(pDvInfo, 0, sizeof(NVT_DV_STATIC_METADATA));

    // copy ieee id
    pDvInfo->ieee_id = pExt861->vsvdb.ieee_id;

    vsvdbVersion = (pExt861->vsvdb.vendor_data[0] & NVT_CEA861_VSVDB_VERSION_MASK) >> NVT_CEA861_VSVDB_VERSION_MASK_SHIFT;

    switch (vsvdbVersion)
    {
        case 0:
            if (pExt861->vsvdb.vendor_data_size < sizeof(NVT_DV_STATIC_METADATA_TYPE0))
            {
                return;
            }
            pDvType0 = (NVT_DV_STATIC_METADATA_TYPE0 *)(&pExt861->vsvdb.vendor_data);
            // copy the data
            pDvInfo->VSVDB_version               = pDvType0->VSVDB_version;
            pDvInfo->supports_2160p60hz          = pDvType0->supports_2160p60hz;
            pDvInfo->supports_YUV422_12bit       = pDvType0->supports_YUV422_12bit;
            pDvInfo->supports_global_dimming     = pDvType0->supports_global_dimming;
            pDvInfo->colorimetry                 = 0; // this field does not exist in type0
            pDvInfo->dm_version                  = (pDvType0->dm_version_major << 4)   | pDvType0->dm_version_minor;
            pDvInfo->target_min_luminance        = (pDvType0->target_min_pq_11_4 << 4) | pDvType0->target_min_pq_3_0;
            pDvInfo->target_max_luminance        = (pDvType0->target_max_pq_11_4 << 4) | pDvType0->target_max_pq_3_0;
            pDvInfo->cc_red_x                    = (pDvType0->cc_red_x_11_4 << 4)      | pDvType0->cc_red_x_3_0;
            pDvInfo->cc_red_y                    = (pDvType0->cc_red_y_11_4 << 4)      | pDvType0->cc_red_y_3_0;
            pDvInfo->cc_green_x                  = (pDvType0->cc_green_x_11_4 << 4)    | pDvType0->cc_green_x_3_0;
            pDvInfo->cc_green_y                  = (pDvType0->cc_green_y_11_4 << 4)    | pDvType0->cc_green_y_3_0;
            pDvInfo->cc_blue_x                   = (pDvType0->cc_blue_x_11_4 << 4)     | pDvType0->cc_blue_x_3_0;
            pDvInfo->cc_blue_y                   = (pDvType0->cc_blue_y_11_4 << 4)     | pDvType0->cc_blue_y_3_0;
            pDvInfo->cc_white_x                  = (pDvType0->cc_white_x_11_4 << 4)    | pDvType0->cc_white_x_3_0;
            pDvInfo->cc_white_y                  = (pDvType0->cc_white_y_11_4 << 4)    | pDvType0->cc_white_y_3_0;
            pDvInfo->supports_backlight_control  = 0;
            pDvInfo->backlt_min_luma             = 0;
            pDvInfo->interface_supported_by_sink = 0;
            pDvInfo->supports_10b_12b_444        = 0;
            break;
        case 1:
            if (pExt861->vsvdb.vendor_data_size == sizeof(NVT_DV_STATIC_METADATA_TYPE1))
            {
                pDvType1 = (NVT_DV_STATIC_METADATA_TYPE1 *)(&pExt861->vsvdb.vendor_data);
                // copy the data
                pDvInfo->VSVDB_version                  = pDvType1->VSVDB_version;
                pDvInfo->supports_2160p60hz             = pDvType1->supports_2160p60hz;
                pDvInfo->supports_YUV422_12bit          = pDvType1->supports_YUV422_12bit;
                pDvInfo->dm_version                     = pDvType1->dm_version;
                pDvInfo->supports_global_dimming        = pDvType1->supports_global_dimming;
                pDvInfo->colorimetry                    = pDvType1->colorimetry;
                pDvInfo->target_min_luminance           = pDvType1->target_min_luminance;
                pDvInfo->target_max_luminance           = pDvType1->target_max_luminance;
                pDvInfo->cc_red_x                       = pDvType1->cc_red_x;
                pDvInfo->cc_red_y                       = pDvType1->cc_red_y;
                pDvInfo->cc_green_x                     = pDvType1->cc_green_x;
                pDvInfo->cc_green_y                     = pDvType1->cc_green_y;
                pDvInfo->cc_blue_x                      = pDvType1->cc_blue_x;
                pDvInfo->cc_blue_y                      = pDvType1->cc_blue_y;
                pDvInfo->supports_backlight_control     = 0;
                pDvInfo->backlt_min_luma                = 0;
                pDvInfo->interface_supported_by_sink    = 0;
                pDvInfo->supports_10b_12b_444           = 0;
                pDvInfo->cc_white_x                     = 0;
                pDvInfo->cc_white_y                     = 0;
            }
            else if (pExt861->vsvdb.vendor_data_size == sizeof(NVT_DV_STATIC_METADATA_TYPE1_1))
            {
                pvDvType1_1 = (NVT_DV_STATIC_METADATA_TYPE1_1 *)(&pExt861->vsvdb.vendor_data);
                // copy the data
                pDvInfo->VSVDB_version                  = pvDvType1_1->VSVDB_version;
                pDvInfo->supports_2160p60hz             = pvDvType1_1->supports_2160p60hz;
                pDvInfo->supports_YUV422_12bit          = pvDvType1_1->supports_YUV422_12bit;
                pDvInfo->dm_version                     = pvDvType1_1->dm_version;
                pDvInfo->supports_global_dimming        = pvDvType1_1->supports_global_dimming;
                pDvInfo->colorimetry                    = pvDvType1_1->colorimetry;
                pDvInfo->target_min_luminance           = pvDvType1_1->target_min_luminance;
                pDvInfo->target_max_luminance           = pvDvType1_1->target_max_luminance;
                pDvInfo->cc_green_x                     = NVT_DOLBY_CHROMATICITY_MSB_GX | pvDvType1_1->unique_Gx;
                pDvInfo->cc_green_y                     = NVT_DOLBY_CHROMATICITY_MSB_GY | pvDvType1_1->unique_Gy;
                pDvInfo->cc_blue_x                      = NVT_DOLBY_CHROMATICITY_MSB_BX | pvDvType1_1->unique_Bx;
                pDvInfo->cc_blue_y                      = NVT_DOLBY_CHROMATICITY_MSB_BY | pvDvType1_1->unique_By;
                pDvInfo->cc_red_x                       = NVT_DOLBY_CHROMATICITY_MSB_RX | pvDvType1_1->unique_Rx;
                pDvInfo->cc_red_y                       = NVT_DOLBY_CHROMATICITY_MSB_RY | (pvDvType1_1->unique_Ry_bit_0 | (pvDvType1_1->unique_Ry_bit_1 <<1) | (pvDvType1_1->unique_Ry_bit_2_to_4 << 2));
                pDvInfo->supports_backlight_control     = 0;
                pDvInfo->backlt_min_luma                = 0;
                pDvInfo->interface_supported_by_sink    = pvDvType1_1->interface_supported_by_sink;
                pDvInfo->supports_10b_12b_444           = 0;
                pDvInfo->cc_white_x                     = 0;
                pDvInfo->cc_white_y                     = 0;
            }
            else
            {
                return;
            }
            
            break;
        case 2:
            if (pExt861->vsvdb.vendor_data_size < sizeof(NVT_DV_STATIC_METADATA_TYPE2))
            {
                return;
            }
            pDvType2 = (NVT_DV_STATIC_METADATA_TYPE2 *)(&pExt861->vsvdb.vendor_data);
            // copy the data
            pDvInfo->VSVDB_version               = pDvType2->VSVDB_version;
            pDvInfo->supports_backlight_control  = pDvType2->supports_backlight_control;
            pDvInfo->supports_YUV422_12bit       = pDvType2->supports_YUV422_12bit;
            pDvInfo->dm_version                  = pDvType2->dm_version;
            pDvInfo->supports_global_dimming     = pDvType2->supports_global_dimming;
            pDvInfo->target_min_luminance        = pDvType2->target_min_luminance;
            pDvInfo->interface_supported_by_sink = pDvType2->interface_supported_by_sink;
            pDvInfo->parity                      = pDvType2->parity;
            pDvInfo->target_max_luminance        = pDvType2->target_max_luminance;
            pDvInfo->cc_green_x                  = NVT_DOLBY_CHROMATICITY_MSB_GX | pDvType2->unique_Gx;
            pDvInfo->cc_green_y                  = NVT_DOLBY_CHROMATICITY_MSB_GY | pDvType2->unique_Gy;
            pDvInfo->cc_blue_x                   = NVT_DOLBY_CHROMATICITY_MSB_BX | pDvType2->unique_Bx;
            pDvInfo->cc_blue_y                   = NVT_DOLBY_CHROMATICITY_MSB_BY | pDvType2->unique_By;
            pDvInfo->cc_red_x                    = NVT_DOLBY_CHROMATICITY_MSB_RX | pDvType2->unique_Rx;
            pDvInfo->cc_red_y                    = NVT_DOLBY_CHROMATICITY_MSB_RY | pDvType2->unique_Ry;
            pDvInfo->supports_10b_12b_444        = pDvType2->supports_10b_12b_444_bit0 | (pDvType2->supports_10b_12b_444_bit1 << 1);
            pDvInfo->colorimetry                 = 0; 
            pDvInfo->supports_2160p60hz          = 0;
            pDvInfo->cc_white_x                  = 0;
            pDvInfo->cc_white_y                  = 0;
            break;
        default:
            break;
    }
}

// find both hdmi llc and hdmi forum vendor specific data block and return basic hdmi information
CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861VsdbBlocks(NVT_EDID_CEA861_INFO *pExt861,
                           void *pRawInfo, 
                           NVT_CTA861_ORIGIN flag
    )
{
    NvU32 i;
    
    NVT_EDID_INFO           *pInfo         = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20  = NULL;
    NVT_HDMI_LLC_INFO       *pHdmiLlc      = NULL;
    NVT_HDMI_FORUM_INFO     *pHfvs         = NULL;
    NVDA_VSDB_PARSED_INFO   *pNvVsdb       = NULL;
    MSFT_VSDB_PARSED_INFO   *pMsftVsdb     = NULL;
    
    if (pExt861 == NULL || pRawInfo == NULL)
    {
        return;
    }
    
    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo     = (NVT_EDID_INFO *)pRawInfo;
        pHdmiLlc  = &pInfo->hdmiLlcInfo;
        pHfvs     = &pInfo->hdmiForumInfo;
        pNvVsdb   = &pInfo->nvdaVsdbInfo;
        pMsftVsdb = &pInfo->msftVsdbInfo;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pHdmiLlc  = &pDisplayID20->vendor_specific.hdmiLlc;
        pHfvs     = &pDisplayID20->vendor_specific.hfvs;
        pNvVsdb   = &pDisplayID20->vendor_specific.nvVsdb;
        pMsftVsdb = &pDisplayID20->vendor_specific.msftVsdb;
    }
    else 
    {
        return;
    }

    if (pHdmiLlc == NULL || pHfvs == NULL || pNvVsdb == NULL || pMsftVsdb == NULL || (pExt861->total_vsdb == 0))
    {
        return;
    }

    for (i = 0; i < pExt861->total_vsdb; i++)
    {
        // Assumes each vsdb is unique for this CEA block, e.g., no two HDMI_IEEE_ID
        switch (pExt861->vsdb[i].ieee_id)
        {
            case NVT_CEA861_HDMI_IEEE_ID:
                // set any 3D timings and HDMI extended timing specified in the VSDB
                parseEdidHdmiLlcBasicInfo((VSDB_DATA *)(&pExt861->vsdb[i]), pHdmiLlc);
                pExt861->valid.H14B_VSDB = 1;
                break;
                
            case NVT_CEA861_HDMI_FORUM_IEEE_ID:
                parseEdidHdmiForumVSDB((VSDB_DATA *)(&pExt861->vsdb[i]), pHfvs);
                pExt861->valid.H20_HF_VSDB = 1;
                break;

            case NVT_CEA861_NVDA_IEEE_ID:
                parseEdidNvidiaVSDBBlock((VSDB_DATA *)(&pExt861->vsdb[i]), pNvVsdb);
                pExt861->valid.nvda_vsdb = 1;
                break;

            case NVT_CEA861_MSFT_IEEE_ID:
                parseEdidMsftVsdbBlock((VSDB_DATA *)(&pExt861->vsdb[i]), pMsftVsdb);
                pExt861->valid.msft_vsdb = 1;
                break;

        }
    }

    // H20_HF_VSDB shall be listed only if H14B_VSDB is also listed 
    // H20_HF_VSDB should not specify > 600MHz
    nvt_assert(!pExt861->valid.H20_HF_VSDB || (pExt861->valid.H14B_VSDB && (pHfvs->max_TMDS_char_rate <= 0x78)));

    // Done with reading CEA VSDB blocks, sanitize them now
    if (pExt861->valid.SCDB)
    {
        pHdmiLlc->effective_tmds_clock = pExt861->hfscdb[1];
    }
    else if (pExt861->valid.H14B_VSDB)
    {
        // HDMI 2.0 Spec -  section 10.3.2
        // The maximum Rate = Max_TMDS_Character_Rate * 5 MHz.
        // If the Sink does not support TMDS Character Rates > 340 Mcsc, then the Sink shall set this field to 0.
        // If the Sink supports TMDS Character Rates > 340 Mcsc, the Sink shall set Max_TMDS_Character_Rate appropriately and non - zero.

        // Pick updated TMDS clock rate
        pHdmiLlc->effective_tmds_clock = (pExt861->valid.H20_HF_VSDB) ?
                                            MAX(pHdmiLlc->max_tmds_clock, pHfvs->max_TMDS_char_rate) : 
                                            MIN(pHdmiLlc->max_tmds_clock, 0x44);
    }

}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861HfEeodb(NVT_EDID_CEA861_INFO *pExt861,
                        NvU32 *pTotalEdidExtensions)
{
    // *pTotalEdidExtensions set by the edid extension flag should be >= 1 for HFEEODB to be valid.
    if (pTotalEdidExtensions == NULL || pExt861 == NULL || !pExt861->valid.HF_EEODB || *pTotalEdidExtensions == 0)
    {
        return;
    }

    // HDMI 2.1 AmendmentA1 specifies that if EEODB is present sources shall ignore the Extension flag.
    // This effectively overrides the extension count from extension flag.
    *pTotalEdidExtensions = pExt861->hfeeodb;
}


CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861HfScdb(NVT_EDID_CEA861_INFO *pExt861,
                       void *pRawInfo,
                       NVT_CTA861_ORIGIN flag)
{
    NVT_EDID_INFO   *pInfo = (NVT_EDID_INFO *)pRawInfo;
    VSDB_DATA       vsdbData;

    if (pExt861 == NULL || pRawInfo == NULL)
    {
        return;
    }

    if (!pExt861->valid.SCDB || pExt861->valid.H20_HF_VSDB)
    {
        return;
    }
    NVMISC_MEMSET(&vsdbData, 0, sizeof(vsdbData));
    NVMISC_MEMCPY(&vsdbData.vendor_data, pExt861->hfscdb, sizeof(vsdbData.vendor_data));

    vsdbData.vendor_data_size = pExt861->hfscdbSize;

    parseEdidHdmiForumVSDB(&vsdbData, &pInfo->hdmiForumInfo);
}


CODE_SEGMENT(PAGE_DD_CODE)
void getEdidHDM1_4bVsdbTiming(NVT_EDID_INFO *pInfo)
{
    NvU32 i = 0, j = 0;

    for (i = 0; i < 2; ++i)
    {
        NVT_EDID_CEA861_INFO *pExt861 = (0 == i) ? &pInfo->ext861 : &pInfo->ext861_2;

        for (j = 0; j < pExt861->total_vsdb; ++j)
        {
            switch (pExt861->vsdb[j].ieee_id)
            {
                case NVT_CEA861_HDMI_IEEE_ID:
                {
                    NvU32 count = 0;
                    // set any 3D timings and HDMI extended timing specified in the VSDB
                    parseEdidHDMILLCTiming(pInfo, (VSDB_DATA *)(&pExt861->vsdb[j]), &count, &(pInfo->Hdmi3Dsupport));
                    pInfo->HDMI3DSupported = 0 < count;
                    break;
                }

                default:
                    break;
            }
        }
    }
}

// get the full EDID 861 extension info
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS get861ExtInfo(NvU8 *p, NvU32 size, NVT_EDID_CEA861_INFO *p861info)
{

    NvU32 dtd_offset;
    // sanity check
    if (p == NULL || size < sizeof(EDIDV1STRUC))
    {
        return NVT_STATUS_ERR;
    }

    // make sure we have 861 extension
    if (p[0] != 0x2 || p[1] < NVT_CEA861_REV_ORIGINAL) 
    {
        return NVT_STATUS_ERR;
    }

    // DTD offset sanity check 
    if (p[2] >= 1 && p[2] <= 3)
    {
        return NVT_STATUS_ERR;
    }

    // don't do anything further if p861info is NULL
    if (p861info == NULL)
    {
        return NVT_STATUS_SUCCESS;
    }

    // init
    NVMISC_MEMSET(p861info, 0, sizeof(NVT_EDID_CEA861_INFO));

    // get the revision number
    p861info->revision = p[1];
    
    // no extra info for 861-original, returning from here
    if (p861info->revision == NVT_CEA861_REV_ORIGINAL)
    {
        return NVT_STATUS_SUCCESS;
    }

    p861info->basic_caps = p[3];

    // no extra info for 861-A, returning from here
    if (p861info->revision == NVT_CEA861_REV_A)
    {
        return NVT_STATUS_SUCCESS;
    }

    dtd_offset = (NvU32)p[2];
    if (dtd_offset == 0 || dtd_offset == 4)
    {
        return NVT_STATUS_SUCCESS;
    }

    // resolve all short descriptors in the reserved block
    // reserved block starts from offset 04 to dtd_offset-1
    return parseCta861DataBlockInfo(&p[4], dtd_offset - 4, p861info);
}

// 1. get the  861 extension tags info
// 2. or validation purpose if p861info == NULL
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS parseCta861DataBlockInfo(NvU8 *p, 
                                    NvU32 size, 
                                    NVT_EDID_CEA861_INFO *p861info)
{
    NvU32 i, j;
    NvU32 video_index      = 0;
    NvU32 audio_index      = 0;
    NvU32 speaker_index    = 0;
    NvU32 vendor_index     = 0;
    NvU32 yuv420vdb_index  = 0;
    NvU32 yuv420cmdb_index = 0;
    NvU8  didT7_index      = 0;
    NvU8  didT8_index      = 0;
    NvU8  didT10_index     = 0;
    NvU8  svr_index        = 0;
    NvU32 ieee_id          = 0;
    NvU32 tag, ext_tag, payload;
    i= 0;

    while (i < size)
    {
        // get the descriptor's tag and payload size
        tag = NVT_CEA861_GET_SHORT_DESCRIPTOR_TAG(p[i]);
        payload = NVT_CEA861_GET_SHORT_DESCRIPTOR_SIZE(p[i]);

        /*don't allow data colleciton totally size larger than [127 - 5 (tag, revision, offset, describing native video format, checksum)]*/
        if ((i + payload > size) || (i + payload > 122))  
        {
            break;
        }
        // move the pointer to the payload section or extended Tag Code
        i++;
        
        // NvTiming_EDIDValidationMask will use the different tag/payload value to make sure each of cta861 data block legal
        if (p861info == NULL)
        {
            switch(tag)
            {
                case NVT_CEA861_TAG_AUDIO:
                case NVT_CEA861_TAG_VIDEO:
                case NVT_CEA861_TAG_SPEAKER_ALLOC:
                case NVT_CEA861_TAG_VESA_DTC:
                case NVT_CEA861_TAG_RSVD:
                break;
                case NVT_CEA861_TAG_VENDOR:
                    if (payload < 3) return NVT_STATUS_ERR;
                break;
                case NVT_CEA861_TAG_EXTENDED_FLAG:
                    if (payload >= 1)
                    {
                        ext_tag = p[i]; 
                        if      (ext_tag == NVT_CEA861_EXT_TAG_VIDEO_CAP && payload < 2)               return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_COLORIMETRY && payload < 3)             return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_VIDEO_FORMAT_PREFERENCE && payload < 2) return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_YCBCR420_VIDEO && payload < 2)          return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_YCBCR420_CAP && payload < 1)            return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_HDR_STATIC_METADATA && payload < 3)     return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CEA861_EXT_TAG_VENDOR_SPECIFIC_VIDEO && payload < 4)   return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_SCDB && payload < 7)                    return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_HF_EEODB && payload != 2)               return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VII && payload <= 2)           return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VIII && payload <= 2)          return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_X && payload <= 2)             return NVT_STATUS_ERR;
                        else if (ext_tag == NVT_CTA861_EXT_TAG_NATIVE_VIDEO_RESOLUTION)
                        {
                            if (payload != 2 && payload != 3 && payload != 7)                          return NVT_STATUS_ERR;
                        }
                            
                        if (payload > 2)
                        {
                            if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VII)
                            {
                                if ((payload-2) != 20)                                                 return NVT_STATUS_ERR;    // only support 20-bytes
                                if ((p[i+1] & 0x7) != 2)                                               return NVT_STATUS_ERR;    // Block Revision shall be 2
                                if ((p[i+1] & 0x70) >> 4 != 0)                                         return NVT_STATUS_ERR;    // Not allow extra byte
                            }
                            else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VIII)
                            {
                                 if ((payload-2) < 1)                                                  return NVT_STATUS_ERR;    // minimum one code supported
                                 if ((p[i+1] & 0x7) != 1)                                              return NVT_STATUS_ERR;    // Block Revision shall be 1
                                 if ((p[i+1] & 0xC0) >> 6 != 0)                                        return NVT_STATUS_ERR;    // Not allow others than DMT Timing
                            }
                            else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_X)
                            {
                                if ((p[i+1] & 0x7) != 0)                                               return NVT_STATUS_ERR;    // Block Revision shall be 0
                                if (((p[i+1] & 0x70) >> 4 == 0) && (payload-2) % 6)                    return NVT_STATUS_ERR;    // supported 6-bytes descriptors  
                                if (((p[i+1] & 0x70) >> 4 == 1) && (payload-2) % 7)                    return NVT_STATUS_ERR;    // supported 7-bytes descriptors
                            }
                        }
                    }
                break;
                default:
                break;       
            }
            return NVT_STATUS_SUCCESS;
        }

        // loop through all descriptors
        if (tag == NVT_CEA861_TAG_VIDEO)
        {
            // short video descriptor
            for (j = 0; j < payload; j ++, i ++, video_index ++)
            {
                if (video_index < NVT_CEA861_VIDEO_MAX_DESCRIPTOR)
                {
                    p861info->video[video_index] = p[i];
                }
                else
                {
                    break;
                }
            }
            p861info->total_svd = (NvU8)video_index;
        }
        else if (tag == NVT_CEA861_TAG_AUDIO)
        {
            // short audio descriptor
            for (j = 0; j < payload / 3; j ++, i += 3, audio_index ++)
            {
                if (audio_index < NVT_CEA861_AUDIO_MAX_DESCRIPTOR)
                {
                    p861info->audio[audio_index].byte1 = p[i];
                    p861info->audio[audio_index].byte2 = p[i+1];
                    p861info->audio[audio_index].byte3 = p[i+2];
                }
                else
                {
                    break;
                }
            }
            p861info->total_sad = (NvU8)audio_index;
        }
        else if (tag == NVT_CEA861_TAG_SPEAKER_ALLOC)
        {
            // speaker allocation descriptor
            for (j = 0; j < payload / 3; j ++, i += 3, speaker_index ++)
            {
                if (speaker_index < NVT_CEA861_SPEAKER_MAX_DESCRIPTOR)
                {
                    p861info->speaker[speaker_index].byte1 = p[i];
                    p861info->speaker[speaker_index].byte2 = p[i+1];
                    p861info->speaker[speaker_index].byte3 = p[i+2];
                }
                else
                {
                    break;
                }
            }
            p861info->total_ssd = (NvU8)speaker_index;
        }
        else if (tag == NVT_CEA861_TAG_VENDOR)
        {
            if (vendor_index < NVT_CEA861_VSDB_MAX_BLOCKS)
            {
                if (payload < 3)
                {
                    // This malformed payload will cause a hang below.
                    return NVT_STATUS_ERR;
                }

                p861info->vsdb[vendor_index].ieee_id = p[i];                 //IEEE ID low byte
                p861info->vsdb[vendor_index].ieee_id |= (p[i+1]) << 8;       //IEEE ID middle byte
                p861info->vsdb[vendor_index].ieee_id |= (p[i+2]) << 16;      //IEEE ID high byte

                p861info->vsdb[vendor_index].vendor_data_size = payload - 3;

                // move the pointer to the payload
                i += 3;

                // get the other vendor specific data
                for (j = 0; j < payload - 3; j ++, i ++)
                {
                    if (j < NVT_CEA861_VSDB_PAYLOAD_MAX_LENGTH)
                    {
                        p861info->vsdb[vendor_index].vendor_data[j] = p[i];
                    }
                }
                vendor_index++;
            }
        }
        else if (tag == NVT_CEA861_TAG_EXTENDED_FLAG)
        {
            if (payload >= 1)
            {
                ext_tag = p[i];
                if (ext_tag == NVT_CEA861_EXT_TAG_VIDEO_CAP && payload >= 2)
                {
                    p861info->video_capability = p[i + 1] & NVT_CEA861_VIDEO_CAPABILITY_MASK;
                    p861info->valid.VCDB = 1;
                    i += 2;
                }
                else if (ext_tag == NVT_CEA861_EXT_TAG_COLORIMETRY && payload >= 3)
                {
                    p861info->colorimetry.byte1 = p[i + 1] & NVT_CEA861_COLORIMETRY_MASK;
                    p861info->colorimetry.byte2 = p[i + 2] & NVT_CEA861_GAMUT_METADATA_MASK;
                    p861info->valid.colorimetry = 1;
                    i += 3;
                }
                else if (ext_tag == NVT_CEA861_EXT_TAG_VIDEO_FORMAT_PREFERENCE && payload >= 2)
                {
                    // when present, indicates the order of preference for selected Video Formats listed as DTDs and/or SVDs throughout Block 0 and the CTA Extensions of the
                    // order of SVD preferred modes shall take precedence over preferred modes defined elsewhere in the EDID/CEA861 blocks

                    // exclude the extended tag
                    i++; payload--;

                    for (j = 0; (j < payload) && (svr_index < NVT_CEA861_VFPDB_MAX_DESCRIPTOR); j++, i++, svr_index++)
                    {
                        p861info->svr_vfpdb[svr_index] = p[i];
                    }
                    p861info->total_svr = svr_index;
                }
                else if (ext_tag == NVT_CEA861_EXT_TAG_YCBCR420_VIDEO && payload >= 2)
                {
                    // when present, list SVDs that are only supported in YCbCr 4:2:0

                    // exclude the extended tag
                    i++; payload--;

                    for (j = 0; (j < payload) && (yuv420vdb_index < NVT_CEA861_Y420VDB_MAX_DESCRIPTOR); j++, i++, yuv420vdb_index++)
                    {
                        p861info->svd_y420vdb[yuv420vdb_index] = p[i];
                    }
                    p861info->total_y420vdb = (NvU8)yuv420vdb_index;
                }
                else if (ext_tag == NVT_CEA861_EXT_TAG_YCBCR420_CAP && payload >= 1)
                {
                    // when present, provides bitmap to video SVDs that also support YCbCr 4:2:0 in addition to RGB, YCbCr 4:4:4, and/or YCbCr 4: 2:0

                    // exclude the extended tag
                    i++; payload--;

                    for (j = 0; (j < payload) && (yuv420cmdb_index < NVT_CEA861_Y420CMDB_MAX_DESCRIPTOR); j++, i++, yuv420cmdb_index++)
                    {
                        p861info->map_y420cmdb[yuv420cmdb_index] = p[i];
                    }
                    p861info->total_y420cmdb = (NvU8)yuv420cmdb_index;

                    p861info->valid.y420cmdb = 1; // total_y420cmdb is not enough as this could be 0.  See CEA861-F 7.5.11
                }
                else if(ext_tag == NVT_CEA861_EXT_TAG_HDR_STATIC_METADATA && payload >= 3)
                {
                    p861info->hdr_static_metadata.byte1 = p[i + 1] & NVT_CEA861_EOTF_MASK;                            // This byte has bits which identify which EOTF supported by the sink.
                    p861info->hdr_static_metadata.byte2 = p[i + 2] & NVT_CEA861_STATIC_METADATA_DESCRIPTOR_MASK;      // This byte has bits which identify which Static Metadata descriptors are supported by the sink.

                    i += 3;

                    if (payload > 3)
                    {
                        p861info->hdr_static_metadata.byte3 = p[i];
                        i++;
                    }

                    if (payload > 4)
                    {
                        p861info->hdr_static_metadata.byte4 = p[i];
                        i++;
                    }

                    if (payload > 5)
                    {
                        p861info->hdr_static_metadata.byte5 = p[i];
                        i++;
                    }

                    p861info->valid.hdr_static_metadata = 1;
                }
                else if(ext_tag == NVT_CEA861_EXT_TAG_VENDOR_SPECIFIC_VIDEO)
                {
                    ieee_id  =  p[i + 1];           //IEEE ID low byte
                    ieee_id |= (p[i + 2]) << 8;     //IEEE ID middle byte
                    ieee_id |= (p[i + 3]) << 16;    //IEEE ID high byte

                    if ((ieee_id == NVT_CEA861_DV_IEEE_ID) || (ieee_id == NVT_CEA861_HDR10PLUS_IEEE_ID))
                    {
                        // exclude the extended tag
                        i++; payload--;

                        p861info->vsvdb.ieee_id = ieee_id;
                        p861info->vsvdb.vendor_data_size = payload - 3;

                        // move the pointer to the payload
                        i += 3;

                        // get the other vendor specific video data
                        for (j = 0; j < payload - 3; j++, i++)
                        {
                            if (j < NVT_CEA861_VSVDB_PAYLOAD_MAX_LENGTH)
                            {
                                p861info->vsvdb.vendor_data[j] = p[i];
                            }
                        }
                        
                        if (p861info->vsvdb.ieee_id == NVT_CEA861_DV_IEEE_ID)
                        {
                            p861info->valid.dv_static_metadata = 1;
                        }
                        else if (p861info->vsvdb.ieee_id == NVT_CEA861_HDR10PLUS_IEEE_ID)
                        {
                            p861info->valid.hdr10Plus = 1;
                        }
                    }
                    else
                    {
                        // skip the unsupported extended block
                        i += payload;
                    }
                }
                else if (ext_tag == NVT_CTA861_EXT_TAG_NATIVE_VIDEO_RESOLUTION)
                {
                    if (payload != 2 && payload != 3 && payload != 7) break;

                    i++; payload--;
                    p861info->native_video_resolution_db.native_svr = p[i];
                    p861info->valid.NVRDB = 1;

                    i++; payload--;
                    if (payload != 0)
                    {
                        p861info->native_video_resolution_db.option.img_size =  p[i] & 0x01;
                        p861info->native_video_resolution_db.option.sz_prec  = (p[i] & 0x80) >> 7;

                        i++; payload--;
                        if (p861info->native_video_resolution_db.option.img_size == 1)
                        {
                            for (j = 0; j< payload; j++, i++)
                            {
                                p861info->native_video_resolution_db.image_size[j] = p[i];
                            }
                        }
                    }
                }
                else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VII)
                {
                    if( payload != 22) break;

                    i++; payload--;
                    p861info->did_type7_data_block[didT7_index].version.revision =  p[i] & 0x07;
                    p861info->did_type7_data_block[didT7_index].version.dsc_pt   = (p[i] & 0x08) >> 3;
                    p861info->did_type7_data_block[didT7_index].version.t7_m     = (p[i] & 0x70) >> 4; 

                    //do not consider Byte 3
                    i++; payload--;

                    p861info->did_type7_data_block[didT7_index].total_descriptors = 
                        (NvU8)(payload / (NVT_CTA861_DID_TYPE7_DESCRIPTORS_LENGTH + p861info->did_type7_data_block[didT7_index].version.t7_m));
                    
                    for (j = 0; j < payload; j++, i++)
                    {
                        p861info->did_type7_data_block[didT7_index].payload[j] = p[i];
                    }
                    // next type7 data block if it exists
                    p861info->total_did_type7db = ++didT7_index;
                }
                else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_VIII)
                {
                    if (payload <= 2) break;

                    i++; payload--;
                    p861info->did_type8_data_block[didT8_index].version.revision  =  p[i] & 0x07;
                    p861info->did_type8_data_block[didT8_index].version.tcs       = (p[i] & 0x08) >> 3;
                    p861info->did_type8_data_block[didT8_index].version.t8y420    = (p[i] & 0x20) >> 5;
                    p861info->did_type8_data_block[didT8_index].version.code_type = (p[i] & 0xC0) >> 6;

                    //do not consider Byte 3
                    i++; payload--;

                    if (p861info->did_type8_data_block[didT8_index].version.tcs == 0)
                        p861info->did_type8_data_block[didT8_index].total_descriptors = (NvU8)payload;
                    else if (p861info->did_type8_data_block[didT8_index].version.tcs == 1)
                        p861info->did_type8_data_block[didT8_index].total_descriptors = (NvU8)(payload / 2);
                    
                    for (j = 0; j < payload; j++, i++)
                    {
                        p861info->did_type8_data_block[didT8_index].payload[j] = p[i];
                    }
                    // next type7 data block if it exists
                    p861info->total_did_type8db = ++didT8_index;
                }
                else if (ext_tag == NVT_CTA861_EXT_TAG_DID_TYPE_X)
                {
                    if (payload < 8 || ((payload-2) % 6 != 0 && (payload-2) % 7 != 0)) break;

                    i++; payload--;
                    p861info->did_type10_data_block[didT10_index].version.revision =  p[i] & 0x07;
                    p861info->did_type10_data_block[didT10_index].version.t10_m    = (p[i] & 0x70) >> 4;

                    // do not consider Byte 3
                    i++; payload--;

                    if (p861info->did_type10_data_block[didT10_index].version.t10_m == 0)
                        p861info->did_type10_data_block[didT10_index].total_descriptors = (NvU8)(payload / 6);
                    else if (p861info->did_type10_data_block[didT10_index].version.t10_m == 1)
                        p861info->did_type10_data_block[didT10_index].total_descriptors = (NvU8)(payload / 7);

                    for (j = 0; j < payload; j++, i++)
                    {
                        p861info->did_type10_data_block[didT10_index].payload[j] = p[i];
                    }
                    // next type10 data block if it exists
                    p861info->total_did_type10db = ++didT10_index;
                }
                else if(ext_tag == NVT_CTA861_EXT_TAG_SCDB && payload >= 7) // sizeof(HDMI Forum Sink Capability Data Block) ranges between 7 to 31 bytes
                {
                    // As per HDMI2.1 A1 amendment Sink Capability Data Structure(SCDS) can alternatively be included in HDMI Forum Sink Capability Data Block(HF-SCDB),
                    //  instead of HF-VSDB, to indicate HDMI2.1 capability.
                    // Sinks will expose HF-SCDB if they do not expose HF-VSDB.

                    // move pointer to SCDS
                    i += 3; 

                    // Copy SCDS over to p861info->vsdb[vendor_index]. Parsing will later be handled in parseEdidHdmiForumVSDB().
                    for (j = 0; (j < payload - 3) && (j < NVT_CTA861_EXT_SCDB_PAYLOAD_MAX_LENGTH); j ++, i ++)
                    {
                        p861info->hfscdb[j] = p[i];
                    }
                    p861info->hfscdbSize = MIN(payload - 3, NVT_CTA861_EXT_SCDB_PAYLOAD_MAX_LENGTH);
                    p861info->valid.SCDB = 1;
                }
                else if (ext_tag == NVT_CTA861_EXT_TAG_HF_EEODB && payload == 2)
                {
                    // Skip over extended tag
                    i++; payload--;

                    p861info->hfeeodb = p[i];
                    p861info->valid.HF_EEODB = 1;
                    i += payload;
                }
                else
                {
                    // skip the unrecognized extended block
                    i += payload;
                }
            }
        }
        else
        {
            // reserved block, just skip here
            i += payload;
        }
    }

    if (p861info) 
    {
        p861info->total_vsdb = (NvU8)vendor_index;
    }

    return NVT_STATUS_SUCCESS;
}

// enum the EIA/CEA 861B predefined timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumCEA861bTiming(NvU32 ceaFormat, NVT_TIMING *pT)
{
    if (pT == NULL || ceaFormat == 0 || ceaFormat > MAX_CEA861B_FORMAT)
    {
        return NVT_STATUS_ERR;
    }

    ceaFormat = NVT_GET_CTA_8BIT_VIC(ceaFormat);

    if (ceaFormat ==0)
        return NVT_STATUS_ERR;

    *pT = EIA861B[ceaFormat - 1];

    // calculate the pixel clock
    pT->pclk  = RRx1kToPclk (pT);
    NVT_SET_CEA_FORMAT(pT->etc.status, ceaFormat);

    NVT_SNPRINTF((char *)pT->etc.name, sizeof(pT->etc.name), "CTA-861G:#%3d:%dx%dx%3d.%03dHz/%s", (int)ceaFormat, (int)pT->HVisible, (int)((pT->interlaced ? 2 : 1)*pT->VVisible), (int)pT->etc.rrx1k/1000, (int)pT->etc.rrx1k%1000, (pT->interlaced ? "I":"P"));
    pT->etc.name[sizeof(pT->etc.name) - 1] = '\0';

    return NVT_STATUS_SUCCESS;
}


// Check whether the given timing is a CEA 861 timing. 
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 NvTiming_GetCEA861TimingIndex (NVT_TIMING *pT)
{
    NvU32 i = 0, j = 0;
    NvU32 ceaIndex = 0;
    NvU32 aspect_x;
    NvU32 aspect_y;

    if (pT == NULL)
    {
        return ceaIndex;
    }

    if (NVT_GET_CEA_FORMAT(pT->etc.status) != 0)
    {
        // CEA format has been set, done
        return NVT_GET_CEA_FORMAT(pT->etc.status);
    }

    aspect_x = nvt_aspect_x(pT->etc.aspect);
    aspect_y = nvt_aspect_y(pT->etc.aspect);

    // loop through the pre-defined CEA 861 table
    // Skip VIC1 - Although IT 640x480 video timing has a CE id, it is not a CE timing. See 3.1 
    // "General Video Format Requirements" section in CEA-861-E spec
    for (i = 1; i < MAX_CEA861B_FORMAT; i++)
    {
        if (NvTiming_IsTimingRelaxedEqual(pT, &EIA861B[i]))
        {
            // The timing matches with a CEA 861 timing. Set CEA format to NVT_TIMING.etc.status.
            ceaIndex = NVT_GET_TIMING_STATUS_SEQ(EIA861B[i].etc.status);

            if (!aspect_x || !aspect_y)
            {
                return ceaIndex;
            }
            
            // for the dual-aspect ratio timings we should further check the aspect ratio matching(16:9 or 4:3) based on the integer rounding error
            for (j = 0; j < MAX_EIA861B_DUAL_ASPECT_VICS; j++)
            {
                if (ceaIndex == EIA861B_DUAL_ASPECT_VICS[j][0])
                {
                    NvU32 ceaIndex1 = EIA861B_DUAL_ASPECT_VICS[j][1];

                    NvU32 format1 = axb_div_c(aspect_x, nvt_aspect_y(EIA861B[ceaIndex - 1].etc.aspect), aspect_y);
                    NvU32 format2 = axb_div_c(aspect_x, nvt_aspect_y(EIA861B[ceaIndex1 - 1].etc.aspect), aspect_y);

                    NvU32 format_1_diff = abs_delta(format1, nvt_aspect_x(EIA861B[ceaIndex - 1].etc.aspect));
                    NvU32 format_2_diff = abs_delta(format2, nvt_aspect_x(EIA861B[ceaIndex1 - 1].etc.aspect));

                    if (format_2_diff < format_1_diff)
                    {
                        ceaIndex = ceaIndex1;
                    }
                    break;
                }
                else if (ceaIndex < EIA861B_DUAL_ASPECT_VICS[j][0]) // not a dual-dspect ratio timing 
                {
                    break;
                }
            }
            break;
        }
    }
    return ceaIndex;
}

// calculate 861B based timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcCEA861bTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NvU32 pixelRepeatCount, NVT_TIMING *pT)

{
    NvU32 i = 0;
    NvU16 pixelRepeatMask = 1 << (pixelRepeatCount - 1);

    nvt_assert(pixelRepeatCount > 0 && pixelRepeatCount <= 10);

    if (pT == NULL)
    {
        return NVT_STATUS_ERR;
    }

    // loop through the table
    for (i = 0; i < MAX_CEA861B_FORMAT; i ++)
    {        
        if ((EIA861B[i].etc.rep & pixelRepeatMask) == 0) 
        {
            continue;
        }

        if (width == (NvU32)NvTiming_MaxFrameWidth(EIA861B[i].HVisible, pixelRepeatMask) &&
            height == frame_height(EIA861B[i])&&
            rr == EIA861B[i].etc.rr &&
            (!!(flag & NVT_PVT_INTERLACED_MASK)) == (!!EIA861B[i].interlaced))
        {
            *pT = EIA861B[i];

            // calculate the pixel clock
            pT->pclk  = RRx1kToPclk (pT);

            NVT_SET_CEA_FORMAT(pT->etc.status, NVT_GET_TIMING_STATUS_SEQ(pT->etc.status));

            NVT_SNPRINTF((char *)pT->etc.name, sizeof(pT->etc.name), "CTA-861G:#%3d:%dx%dx%3d.%03dHz/%s", (int)NVT_GET_TIMING_STATUS_SEQ(pT->etc.status), (int)pT->HVisible, (int)((pT->interlaced ? 2 : 1)*pT->VVisible), (int)pT->etc.rrx1k/1000, (int)pT->etc.rrx1k%1000, (pT->interlaced ? "I":"P"));
            pT->etc.name[sizeof(pT->etc.name) - 1] = '\0';

            return NVT_STATUS_SUCCESS;
        }
    }

    return NVT_STATUS_ERR;

}

// Assign fields in NVT_VIDEO_INFOFRAME_CTRL, using NVT_TIMING
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ConstructVideoInfoframeCtrl(const NVT_TIMING *pTiming, NVT_VIDEO_INFOFRAME_CTRL *pCtrl)
{
    // setup VIC code it is not specified
    if (pCtrl->video_format_id == NVT_INFOFRAME_CTRL_DONTCARE ||
        pCtrl->video_format_id == 0                           ||
        pCtrl->video_format_id >  NVT_CEA861_1920X1080P_29970HZ_16X9)
    {
        // setup video format ID
        pCtrl->video_format_id = (NvU8)NVT_GET_CEA_FORMAT(pTiming->etc.status);
        if (pCtrl->video_format_id < NVT_CEA861_640X480P_59940HZ_4X3 ||
            pCtrl->video_format_id > NVT_CTA861_4096x2160p_119880HZ_256X135)
        {
            // Prior RFE 543088
            if (pCtrl->video_format_id == 0 &&
                NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status) == NVT_TYPE_EDID_861ST)
            {
                pCtrl->video_format_id = (NvU8)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status);
            }
            if (pCtrl->video_format_id == 0 &&
                pTiming->HVisible == 640 &&
                pTiming->VVisible == 480 &&
                pTiming->interlaced == 0 &&
                pTiming->etc.rr == 60)
            {
                pCtrl->video_format_id = NVT_CEA861_640X480P_59940HZ_4X3;
            }
        }
    }

    // for HDMI_EXT timing, AVI VIC should be 0.
    if (NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status) == NVT_TYPE_HDMI_EXT)
    {
        pCtrl->video_format_id = 0;
    }

    // setup aspect ratio it is not specified
    if (pCtrl->pic_aspect_ratio == NVT_INFOFRAME_CTRL_DONTCARE            ||
        pCtrl->pic_aspect_ratio == NVT_VIDEO_INFOFRAME_BYTE2_M1M0_NO_DATA ||
        pCtrl->pic_aspect_ratio >  NVT_VIDEO_INFOFRAME_BYTE2_M1M0_FUTURE)
    {
        // extract the screen measurements from the DTD aspect ratio.
        // (we pack the height & width in a DWORD to form the aspect ratio)

        NvU32 x,y;
        x = (pTiming->etc.aspect         & 0x0fff);
        y = ((pTiming->etc.aspect >> 16) & 0x0fff);

        if (axb_div_c(y,3,x) == 4)
        {
            pCtrl->pic_aspect_ratio = NVT_VIDEO_INFOFRAME_BYTE2_M1M0_4X3;
        }
        else if (axb_div_c(y,9,x) == 16)
        {
            pCtrl->pic_aspect_ratio = NVT_VIDEO_INFOFRAME_BYTE2_M1M0_16X9;
        }
        else if (pCtrl->video_format_id == NVT_CEA861_640X480P_59940HZ_4X3)
        {
            pCtrl->pic_aspect_ratio = NVT_VIDEO_INFOFRAME_BYTE2_M1M0_4X3;
        }
        else
        {
            // default to no data, to cover other non-cea modes
            pCtrl->pic_aspect_ratio = NVT_VIDEO_INFOFRAME_BYTE2_M1M0_NO_DATA;
        }
    }

    if (pCtrl->it_content == NVT_INFOFRAME_CTRL_DONTCARE)
    {
        // Initialize ITC flag to NVT_VIDEO_INFOFRAME_BYTE3_ITC_IT_CONTENT
        pCtrl->it_content = NVT_VIDEO_INFOFRAME_BYTE3_ITC_IT_CONTENT;
        pCtrl->it_content_type = NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_GRAPHICS;
    }

    if (pCtrl->pixel_repeat == NVT_INFOFRAME_CTRL_DONTCARE)
    {
        // Initialize pixel repetitions
        NvU32 pixelRepeat = pTiming->etc.rep;
        LOWESTBITIDX_32(pixelRepeat);
        pCtrl->pixel_repeat = (NvU8)pixelRepeat;
    }

    return NVT_STATUS_SUCCESS;
}


// construct AVI video infoframe based on the user control and the current context state
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ConstructVideoInfoframe(NVT_EDID_INFO *pEdidInfo, NVT_VIDEO_INFOFRAME_CTRL *pCtrl, NVT_VIDEO_INFOFRAME *pContext, NVT_VIDEO_INFOFRAME *pInfoFrame)
{
    // parameter check
    if (pEdidInfo == NULL || pInfoFrame == NULL)
    {
        return NVT_STATUS_ERR;
    }

    // infoframe is only supported on 861A and later
    if (pEdidInfo->ext861.revision < NVT_CEA861_REV_A)
    {
        return NVT_STATUS_ERR;
    }

    // if context state is provided, use it to initialize the infoframe buffer
    if (pContext != NULL)
    {
        *pInfoFrame = *pContext;
    }
    else
    {
        *pInfoFrame = DEFAULT_VIDEO_INFOFRAME;
    }
 
    // init the header
    pInfoFrame->type = NVT_INFOFRAME_TYPE_VIDEO;
    
    // TODO : This is just to check the version, we still need to change lots of structure
    //        "NVT_VIDEO_INFOFRAME" / "VIDEO_INFOFRAME" / "DEFAULT_VIDEO_INFOFRAME" / "NVM_DISP_STATE" etc..
    //        to accept the new ACE0-3 bits supported in the future. Right now no any sink to support this.
    //
    // Based on the latest CTA-861-H.pdf file, we need to do following logic to get the correct CTA861 version
    // When Y=7, the IDO defines the C, EC and ACE fields, it shall use AVI InfoFrame Version 4.
    // When Y < 7, the following algorithm shall be used for AVI InfoFrame version selection:
    // if (C=3 and EC=7)
    //     Sources shall use AVI InfoFrame Version 4.
    // Else if (VIC>=128)
    //     Sources shall use AVI InfoFrame Version 3.
    // Else
    //     Sources shall use AVI InfoFrame Version 2.
    // End if
    //
    if (pCtrl)
    {
        if (nvt_get_bits(pInfoFrame->byte1, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_SHIFT) == NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_IDODEFINED)
        {
            pInfoFrame->version = NVT_VIDEO_INFOFRAME_VERSION_4;
        }
        else if (nvt_get_bits(pInfoFrame->byte1, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_SHIFT) < NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_IDODEFINED)
        {
            if ((nvt_get_bits(pInfoFrame->byte2, NVT_VIDEO_INFOFRAME_BYTE2_C1C0_MASK, NVT_VIDEO_INFOFRAME_BYTE2_C1C0_SHIFT) == NVT_VIDEO_INFOFRAME_BYTE2_C1C0_EXT_COLORIMETRY) &&
                //EC2-0 is based on the 7.5.5 at CTA861-G which DCI-P3 bit defined or notat byte4
                (nvt_get_bits(pInfoFrame->byte3, NVT_VIDEO_INFOFRAME_BYTE3_EC_MASK, NVT_VIDEO_INFOFRAME_BYTE3_EC_SHIFT)     == NVT_VIDEO_INFOFRAME_BYTE3_EC_AdditionalColorExt))  
            {
                 pInfoFrame->version = NVT_VIDEO_INFOFRAME_VERSION_4; // just put the logic to get the correct version 4, but it shall not be used at currently stage.
            }
            else
            {
                pInfoFrame->version = (((pCtrl->video_format_id & NVT_VIDEO_INFOFRAME_BYTE4_VIC7) == NVT_VIDEO_INFOFRAME_BYTE4_VIC7) ? NVT_VIDEO_INFOFRAME_VERSION_3 : 
                                       ((pEdidInfo->ext861.revision >= NVT_CEA861_REV_B) ? NVT_VIDEO_INFOFRAME_VERSION_2 : NVT_VIDEO_INFOFRAME_VERSION_1));
            }
        }
    }
    else
    {
        pInfoFrame->version = (pEdidInfo->ext861.revision >= NVT_CEA861_REV_B) ? NVT_VIDEO_INFOFRAME_VERSION_2 : NVT_VIDEO_INFOFRAME_VERSION_1;
    }
    pInfoFrame->length = sizeof(NVT_VIDEO_INFOFRAME) - sizeof(NVT_INFOFRAME_HEADER);
    
    if (pInfoFrame->version < NVT_VIDEO_INFOFRAME_VERSION_3)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte1, 0, NVT_VIDEO_INFOFRAME_BYTE1_RESERVED_MASK, NVT_VIDEO_INFOFRAME_BYTE1_RESERVED_SHIFT);
    }
    
    if (pInfoFrame->version == NVT_VIDEO_INFOFRAME_VERSION_2)
    {   
        nvt_nvu8_set_bits(pInfoFrame->byte4, 0, NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V2_MASK, NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V2_SHIFT);
    }
    else if (pInfoFrame->version == NVT_VIDEO_INFOFRAME_VERSION_1)
    {        
        nvt_nvu8_set_bits(pInfoFrame->byte3, 0, NVT_VIDEO_INFOFRAME_BYTE3_RESERVED_V1_MASK, NVT_VIDEO_INFOFRAME_BYTE3_RESERVED_V1_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->byte4, 0, NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V1_MASK, NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V1_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->byte5, 0, NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V1_MASK, NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V1_SHIFT);
    }

    // construct the desired infoframe contents based on the control
    if (pCtrl)
    {
        // byte 1
        if (pCtrl->color_space != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte1, pCtrl->color_space, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_SHIFT);
        }

        if (pCtrl->active_format_info_present != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte1, pCtrl->active_format_info_present, NVT_VIDEO_INFOFRAME_BYTE1_A0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_A0_SHIFT);
        }

        if (pCtrl->bar_info != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte1, pCtrl->bar_info, NVT_VIDEO_INFOFRAME_BYTE1_B1B0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_B1B0_SHIFT);
        }

        if (pCtrl->scan_info != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte1, pCtrl->scan_info, NVT_VIDEO_INFOFRAME_BYTE1_S1S0_MASK, NVT_VIDEO_INFOFRAME_BYTE1_S1S0_SHIFT);
        }

        // byte 2
        if (pCtrl->colorimetry != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte2, pCtrl->colorimetry, NVT_VIDEO_INFOFRAME_BYTE2_C1C0_MASK, NVT_VIDEO_INFOFRAME_BYTE2_C1C0_SHIFT);
        }

        if (pCtrl->pic_aspect_ratio != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte2, pCtrl->pic_aspect_ratio, NVT_VIDEO_INFOFRAME_BYTE2_M1M0_MASK, NVT_VIDEO_INFOFRAME_BYTE2_M1M0_SHIFT);
        }

        if (pCtrl->active_format_aspect_ratio != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte2, pCtrl->active_format_aspect_ratio, NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_MASK, NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_SHIFT);
        }

        // byte 3
        if (pCtrl->it_content != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte3, pCtrl->it_content, NVT_VIDEO_INFOFRAME_BYTE3_ITC_MASK, NVT_VIDEO_INFOFRAME_BYTE3_ITC_SHIFT);
        }

        if (pCtrl->extended_colorimetry != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte3, pCtrl->extended_colorimetry, NVT_VIDEO_INFOFRAME_BYTE3_EC_MASK, NVT_VIDEO_INFOFRAME_BYTE3_EC_SHIFT);
        }

        if (pCtrl->rgb_quantization_range != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte3, pCtrl->rgb_quantization_range, NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_MASK, NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_SHIFT);
        }

        if (pCtrl->nonuniform_scaling != NVT_INFOFRAME_CTRL_DONTCARE)
        {
            nvt_nvu8_set_bits(pInfoFrame->byte3, pCtrl->nonuniform_scaling, NVT_VIDEO_INFOFRAME_BYTE3_SC_MASK, NVT_VIDEO_INFOFRAME_BYTE3_SC_SHIFT);
        }

        // byte 4 and byte 5 only supported on InfoFrame 2.0 
        if (pInfoFrame->version >= NVT_VIDEO_INFOFRAME_VERSION_2)
        {
            // byte 4
            if (pCtrl->video_format_id != NVT_INFOFRAME_CTRL_DONTCARE)
            {
                nvt_nvu8_set_bits(pInfoFrame->byte4, pCtrl->video_format_id, NVT_VIDEO_INFOFRAME_BYTE4_VIC_MASK, NVT_VIDEO_INFOFRAME_BYTE4_VIC_SHIFT);
            }

            // byte 5
            if (pCtrl->pixel_repeat != NVT_INFOFRAME_CTRL_DONTCARE)
            {
                nvt_nvu8_set_bits(pInfoFrame->byte5, pCtrl->pixel_repeat, NVT_VIDEO_INFOFRAME_BYTE5_PR_MASK, NVT_VIDEO_INFOFRAME_BYTE5_PR_SHIFT);
            }

            // byte5
            if (pCtrl->it_content_type != NVT_INFOFRAME_CTRL_DONTCARE)
            {
                nvt_nvu8_set_bits(pInfoFrame->byte5, pCtrl->it_content_type, NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_MASK, NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_SHIFT);
            }
        }

        // byte 6~13, bar info
        if (pCtrl->top_bar != 0xFFFF)
        {
            pInfoFrame->top_bar_low  = (NvU8)(pCtrl->top_bar % 0x100);
            pInfoFrame->top_bar_high = (NvU8)(pCtrl->top_bar / 0x100);
        }
        if (pCtrl->bottom_bar != 0xFFFF)
        {
            pInfoFrame->bottom_bar_low  = (NvU8)(pCtrl->bottom_bar % 0x100);
            pInfoFrame->bottom_bar_high = (NvU8)(pCtrl->bottom_bar / 0x100);
        }
        if (pCtrl->left_bar != 0xFFFF)
        {
            pInfoFrame->left_bar_low  = (NvU8)(pCtrl->left_bar % 0x100);
            pInfoFrame->left_bar_high = (NvU8)(pCtrl->left_bar / 0x100);
        }
        if (pCtrl->right_bar != 0xFFFF)
        {
            pInfoFrame->right_bar_low  = (NvU8)(pCtrl->right_bar % 0x100);
            pInfoFrame->right_bar_high = (NvU8)(pCtrl->right_bar / 0x100);
        }
    }

    return NVT_STATUS_SUCCESS;
}

// construct AVI audio infoframe based on the user control and the current context state
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ConstructAudioInfoframe(NVT_AUDIO_INFOFRAME_CTRL *pUserCtrl, NVT_AUDIO_INFOFRAME *pContext, NVT_AUDIO_INFOFRAME *pInfoFrame)
{
    NVT_AUDIO_INFOFRAME_CTRL ctrl;

    // parameter check
    if (pInfoFrame == NULL)
    {
        return NVT_STATUS_ERR;
    }

    // use the user provided control if possible
    if (pUserCtrl)
    {
        ctrl = *pUserCtrl;
    }
    else
    {
        // otherwise use the default control
        NVMISC_MEMSET(&ctrl, NVT_INFOFRAME_CTRL_DONTCARE, sizeof(ctrl));
    }

    // if context state is provided, use it to initialize the infoframe buffer
    if (pContext != NULL)
    {
        *pInfoFrame = *pContext;
    }
    else
    {
        *pInfoFrame = DEFAULT_AUDIO_INFOFRAME;

        // if the context state is not provide, we should user EDID info to build a default ctrl
        //buildDefaultAudioInfoframeCtrl(pEdidInfo, &ctrl);
    }

    // init the header
    pInfoFrame->type = NVT_INFOFRAME_TYPE_AUDIO;
    pInfoFrame->version = NVT_VIDEO_INFOFRAME_VERSION_1;
    pInfoFrame->length = sizeof(NVT_AUDIO_INFOFRAME) - sizeof(NVT_INFOFRAME_HEADER);

    // init the reserved fields
    nvt_nvu8_set_bits(pInfoFrame->byte1, 0, NVT_AUDIO_INFOFRAME_BYTE1_RESERVED_MASK, NVT_AUDIO_INFOFRAME_BYTE1_RESERVED_SHIFT);
    nvt_nvu8_set_bits(pInfoFrame->byte2, 0, NVT_AUDIO_INFOFRAME_BYTE2_RESERVED_MASK, NVT_AUDIO_INFOFRAME_BYTE2_RESERVED_SHIFT);
    nvt_nvu8_set_bits(pInfoFrame->byte5, 0, NVT_AUDIO_INFOFRAME_BYTE5_RESERVED_MASK, NVT_AUDIO_INFOFRAME_BYTE5_RESERVED_SHIFT);
    pInfoFrame->rsvd_byte6  = 0;
    pInfoFrame->rsvd_byte7  = 0;
    pInfoFrame->rsvd_byte8  = 0;
    pInfoFrame->rsvd_byte9  = 0;
    pInfoFrame->rsvd_byte10 = 0;

    // byte 1
    if (ctrl.channel_count != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte1, ctrl.channel_count, NVT_AUDIO_INFOFRAME_BYTE1_CC_MASK, NVT_AUDIO_INFOFRAME_BYTE1_CC_SHIFT);
    }

    if (ctrl.coding_type != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte1, ctrl.coding_type, NVT_AUDIO_INFOFRAME_BYTE1_CT_MASK, NVT_AUDIO_INFOFRAME_BYTE1_CT_SHIFT);
    }

    // byte 2
    if (ctrl.sample_depth != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte2, ctrl.sample_depth, NVT_AUDIO_INFOFRAME_BYTE2_SS_MASK, NVT_AUDIO_INFOFRAME_BYTE2_SS_SHIFT);
    }

    if (ctrl.sample_rate != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte2, ctrl.sample_rate, NVT_AUDIO_INFOFRAME_BYTE2_SF_MASK, NVT_AUDIO_INFOFRAME_BYTE2_SF_SHIFT);
    }

    // byte 3
    pInfoFrame->byte3 = 0;

    // byte 4
    if (ctrl.speaker_placement != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte4, ctrl.speaker_placement, NVT_AUDIO_INFOFRAME_BYTE4_CA_MASK, NVT_AUDIO_INFOFRAME_BYTE4_CA_SHIFT);
    }

    // byte 5
    if (ctrl.level_shift != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte5, ctrl.level_shift, NVT_AUDIO_INFOFRAME_BYTE5_LSV_MASK, NVT_AUDIO_INFOFRAME_BYTE5_LSV_SHIFT);
    }

    if (ctrl.down_mix_inhibit != NVT_INFOFRAME_CTRL_DONTCARE)
    {
        nvt_nvu8_set_bits(pInfoFrame->byte5, ctrl.down_mix_inhibit, NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_MASK, NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_SHIFT);
    }


    return NVT_STATUS_SUCCESS;

}

// Construct Vendor Specific Infoframe
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ConstructVendorSpecificInfoframe(NVT_EDID_INFO *pEdidInfo, NVT_VENDOR_SPECIFIC_INFOFRAME_CTRL *pCtrl, NVT_VENDOR_SPECIFIC_INFOFRAME *pInfoFrame)
{
    NVT_STATUS RetCode = NVT_STATUS_SUCCESS;
    NvU8  optIdx  = 0;
    NvU8  HDMIFormat;

    // parameter check
    if (pEdidInfo == NULL || pInfoFrame == NULL || pCtrl == NULL)
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    // infoframe is only supported on 861A and later
    if (pEdidInfo->ext861.revision < NVT_CEA861_REV_A)
    {
        return NVT_STATUS_ERR;
    }


    // initialize the infoframe buffer
    nvt_nvu8_set_bits(pInfoFrame->Header.type, NVT_HDMI_VS_HB0_VALUE, NVT_HDMI_VS_HB0_MASK, NVT_HDMI_VS_HB0_SHIFT);
    nvt_nvu8_set_bits(pInfoFrame->Header.version, NVT_HDMI_VS_HB1_VALUE, NVT_HDMI_VS_HB1_MASK, NVT_HDMI_VS_HB1_SHIFT);
    nvt_nvu8_set_bits(pInfoFrame->Header.length, NVT_HDMI_VS_HB2_VALUE, NVT_HDMI_VS_HB2_MASK, NVT_HDMI_VS_HB2_SHIFT);

    if (pCtrl->HDMIRevision == 14)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_VS_BYTE1_OUI_VER_1_4, NVT_HDMI_VS_BYTE1_OUI_MASK, NVT_HDMI_VS_BYTE1_OUI_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->Data.byte2, NVT_HDMI_VS_BYTE2_OUI_VER_1_4, NVT_HDMI_VS_BYTE2_OUI_MASK, NVT_HDMI_VS_BYTE2_OUI_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->Data.byte3, NVT_HDMI_VS_BYTE3_OUI_VER_1_4, NVT_HDMI_VS_BYTE3_OUI_MASK, NVT_HDMI_VS_BYTE3_OUI_SHIFT);
    }
    else if (pCtrl->HDMIRevision >= 20)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_VS_BYTE1_OUI_VER_2_0, NVT_HDMI_VS_BYTE1_OUI_MASK, NVT_HDMI_VS_BYTE1_OUI_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->Data.byte2, NVT_HDMI_VS_BYTE2_OUI_VER_2_0, NVT_HDMI_VS_BYTE2_OUI_MASK, NVT_HDMI_VS_BYTE2_OUI_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->Data.byte3, NVT_HDMI_VS_BYTE3_OUI_VER_2_0, NVT_HDMI_VS_BYTE3_OUI_MASK, NVT_HDMI_VS_BYTE3_OUI_SHIFT);
    }

    // init the header (mostly done in default Infoframe)
    pInfoFrame->Header.length   = offsetof(NVT_VENDOR_SPECIFIC_INFOFRAME_PAYLOAD, optionalBytes);

    // construct the desired infoframe contents based on the control
    
    // clear all static reserved fields
    nvt_nvu8_set_bits(pInfoFrame->Data.byte4, 0, NVT_HDMI_VS_BYTE4_RSVD_MASK, NVT_HDMI_VS_BYTE4_RSVD_SHIFT);

    // setup the parameters
    nvt_nvu8_set_bits(pInfoFrame->Data.byte4, pCtrl->HDMIFormat, NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_MASK, NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_SHIFT);

    // determine what the format is -- if disabled, force the format to NONE.
    if (pCtrl->Enable)
    {
        HDMIFormat = pCtrl->HDMIFormat;
    }
    else
    {
        HDMIFormat = NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_NONE;
    }

    switch(HDMIFormat)
    {
    case NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_NONE:
        {
            nvt_nvu8_set_bits(pInfoFrame->Data.byte5, 0, NVT_HDMI_VS_BYTENv_RSVD_MASK, NVT_HDMI_VS_BYTENv_RSVD_SHIFT);
            break;
        }
    case NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_EXT:
        {
            // Note: extended resolution frames are not yet fully supported
            nvt_nvu8_set_bits(pInfoFrame->Data.byte5, pCtrl->HDMI_VIC, NVT_HDMI_VS_BYTE5_HDMI_VIC_MASK, NVT_HDMI_VS_BYTE5_HDMI_VIC_SHIFT);
            break;
        }
    case NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_3D:
        {
            nvt_nvu8_set_bits(pInfoFrame->Data.byte5, 0, NVT_HDMI_VS_BYTE5_HDMI_RSVD_MASK, NVT_HDMI_VS_BYTE5_HDMI_RSVD_SHIFT);
            nvt_nvu8_set_bits(pInfoFrame->Data.byte5, pCtrl->ThreeDStruc, NVT_HDMI_VS_BYTE5_HDMI_3DS_MASK, NVT_HDMI_VS_BYTE5_HDMI_3DS_SHIFT);

            // side by side half requires additional format data in the infoframe.
            if (NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF == pCtrl->ThreeDStruc)
            {
                nvt_nvu8_set_bits(pInfoFrame->Data.optionalBytes[optIdx], pCtrl->ThreeDDetail, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_MASK, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SHIFT);
                optIdx++;
            }
            if (pCtrl->MetadataPresent)
            {
                nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_PRES, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_MASK, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_SHIFT);

                switch(pCtrl->MetadataType)
                {
                case NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_PARALLAX:
                    {
                        if (sizeof(pCtrl->Metadata) >= NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX &&
                            sizeof(pInfoFrame->Data.optionalBytes) - (optIdx + 1) >= NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX)
                        {
                            nvt_nvu8_set_bits(pInfoFrame->Data.optionalBytes[optIdx], NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX, NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_MASK, NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_SHIFT);
                            nvt_nvu8_set_bits(pInfoFrame->Data.optionalBytes[optIdx], NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_PARALLAX, NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_MASK, NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_SHIFT);
                            ++optIdx;

                            NVMISC_MEMCPY(pCtrl->Metadata, &pInfoFrame->Data.optionalBytes[optIdx], NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX);
                            optIdx += NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX;
                        }
                        else
                        {
                            // not enough data in the control struct or not enough room in the infoframe -- BOTH compile time issues!!
                            // ignore metadata.
                            nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_NOTPRES, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_MASK, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_SHIFT);
                        }
                        break;
                    }
                default:
                    {
                        // unrecognised metadata, recover the best we can.
                        // note -- can not copy whatever is there because type implies length.
                        nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_NOTPRES, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_MASK, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_SHIFT);
                        RetCode = NVT_STATUS_ERR;
                    }
                }

            }
            else
            {
                nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_NOTPRES, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_MASK, NVT_HDMI_VS_BYTE5_3D_META_PRESENT_SHIFT);
            }
            break;
        }

    }

    if (pCtrl->ALLMEnable == 1)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_ALLM_MODE_EN, NVT_HDMI_VS_BYTE5_ALLM_MODE_MASK, NVT_HDMI_VS_BYTE5_ALLM_MODE_SHIFT);
    }
    else if (pCtrl->ALLMEnable == 0)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte5, NVT_HDMI_VS_BYTE5_ALLM_MODE_DIS, NVT_HDMI_VS_BYTE5_ALLM_MODE_MASK, NVT_HDMI_VS_BYTE5_ALLM_MODE_SHIFT);
    }

    // clear last byte of infoframe (reserved per spec).
    pInfoFrame->Header.length += optIdx + 1;
    for (; optIdx < sizeof(pInfoFrame->Data.optionalBytes); ++optIdx)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.optionalBytes[optIdx], NVT_HDMI_VS_BYTENv_RSVD, NVT_HDMI_VS_BYTENv_RSVD_MASK, NVT_HDMI_VS_BYTENv_RSVD_SHIFT);
    }
    
    return RetCode;
}

// Construct Extended Metadata Packet Infoframe
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_ConstructExtendedMetadataPacketInfoframe(
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME_CTRL *pCtrl,
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME *pInfoFrame)
{
    NVT_STATUS RetCode = NVT_STATUS_SUCCESS;
    if (!pCtrl || !pInfoFrame)
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    // Initialize the infoframe
    NVMISC_MEMSET(pInfoFrame, 0, sizeof(*pInfoFrame));

    // Construct an infoframe to enable or disable HDMI 2.1 VRR
    pInfoFrame->Header.type = NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET;
    pInfoFrame->Header.firstLast = NVT_EMP_HEADER_FIRST_LAST;
    pInfoFrame->Header.sequenceIndex = 0x00;

    if (pCtrl->EnableQMS)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_EMP_BYTE1_SYNC_ENABLE,
                                                  NVT_HDMI_EMP_BYTE1_SYNC_MASK,
                                                  NVT_HDMI_EMP_BYTE1_SYNC_SHIFT);
    }
        
    nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_EMP_BYTE1_VFR_ENABLE,
                                              NVT_HDMI_EMP_BYTE1_VFR_MASK,
                                              NVT_HDMI_EMP_BYTE1_VFR_SHIFT);

    nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_EMP_BYTE1_NEW_ENABLE,
                                              NVT_HDMI_EMP_BYTE1_NEW_MASK,
                                              NVT_HDMI_EMP_BYTE1_NEW_SHIFT);

    if (!pCtrl->EnableVRR && !pCtrl->EnableQMS)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.byte1, NVT_HDMI_EMP_BYTE1_END_ENABLE,
                                                  NVT_HDMI_EMP_BYTE1_END_MASK,
                                                  NVT_HDMI_EMP_BYTE1_END_SHIFT);
    }

    nvt_nvu8_set_bits(pInfoFrame->Data.byte3,
                      NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_SPEC_DEFINED,
                      NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_MASK,
                      NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_SHIFT);

    nvt_nvu8_set_bits(pInfoFrame->Data.byte5, 1,
                      NVT_HDMI_EMP_BYTE5_DATA_SET_TAG_LSB_MASK,
                      NVT_HDMI_EMP_BYTE5_DATA_SET_TAG_LSB_SHIFT);

    nvt_nvu8_set_bits(pInfoFrame->Data.byte7, ((pCtrl->EnableVRR || pCtrl->EnableQMS) ? 4 : 0),
                      NVT_HDMI_EMP_BYTE7_DATA_SET_LENGTH_LSB_MASK,
                      NVT_HDMI_EMP_BYTE7_DATA_SET_LENGTH_LSB_SHIFT);

    if (pCtrl->EnableVRR)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[0],
                          NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_ENABLE,
                          NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_MASK,
                          NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_SHIFT);
    }
    else if (pCtrl->EnableQMS)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[0], 1,
                          NVT_HDMI_EMP_BYTE8_MD0_M_CONST_MASK,
                          NVT_HDMI_EMP_BYTE8_MD0_M_CONST_SHIFT);
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[0],
                          NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_ENABLE,
                          NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_MASK,
                          NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_SHIFT);
    }

    if (pCtrl->ITTiming)
    {
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[1],
                          pCtrl->BaseVFP,
                          NVT_HDMI_EMP_BYTE8_MD1_BASE_VFRONT_MASK,
                          NVT_HDMI_EMP_BYTE8_MD1_BASE_VFRONT_SHIFT);
        
        // In HDMI2.1, MD2 bit 2 is set when RB timing is used.
        // In HDMI2.1A, MD2 bit 2 is RSVD as 0
        if (pCtrl->version == NVT_EXTENDED_METADATA_PACKET_INFOFRAME_VER_HDMI21)
        {
            nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[2],
                              pCtrl->ReducedBlanking,
                              NVT_HDMI_EMP_BYTE8_MD2_RB_MASK,
                              NVT_HDMI_EMP_BYTE8_MD2_RB_SHIFT);
        }

        // MSB for Base Refresh Rate
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[2],
                          pCtrl->BaseRefreshRate >> 8,
                          NVT_HDMI_EMP_BYTE8_MD2_BASE_RR_MSB_MASK,
                          NVT_HDMI_EMP_BYTE8_MD2_BASE_RR_MSB_SHIFT);

        // LSB for Base Refresh Rate
        nvt_nvu8_set_bits(pInfoFrame->Data.metadataBytes[3],
                          pCtrl->BaseRefreshRate,
                          NVT_HDMI_EMP_BYTE8_MD3_BASE_RR_LSB_MASK,
                          NVT_HDMI_EMP_BYTE8_MD3_BASE_RR_LSB_SHIFT);
    }

    return RetCode;
}

// Construct Adaptive Sync SDP
CODE_SEGMENT(PAGE_DD_CODE)
void NvTiming_ConstructAdaptiveSyncSDP(
    const NVT_ADAPTIVE_SYNC_SDP_CTRL    *pCtrl,
    NVT_ADAPTIVE_SYNC_SDP               *pSdp)
{
    if (!pCtrl || !pSdp)
    {
        return;
    }

    // Initialize the infoframe
    NVMISC_MEMSET(pSdp, 0, sizeof(*pSdp));

    // Construct an infoframe to enable or disable Adaptive Sync SDP
    pSdp->header.type = NVT_DP_ADAPTIVE_SYNC_SDP_PACKET_TYPE;
    pSdp->header.version = NVT_DP_ADAPTIVE_SYNC_SDP_VERSION; 
    pSdp->header.length = NVT_DP_ADAPTIVE_SYNC_SDP_LENGTH;

    // Payload
    if (pCtrl->bFixedVTotal)
    {
        nvt_nvu8_set_bits(pSdp->payload.db0, NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_FAVT_TARGET_REACHED,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_MASK,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_SHIFT);
        if (pCtrl->targetRefreshRate)
        {
            nvt_nvu8_set_bits(pSdp->payload.db3, pCtrl->targetRefreshRate & 0xff,
                                        NVT_DP_ADAPTIVE_SYNC_SDP_DB3_TARGET_RR_LSB_MASK,
                                        NVT_DP_ADAPTIVE_SYNC_SDP_DB3_TARGET_RR_LSB_SHIFT);

            nvt_nvu8_set_bits(pSdp->payload.db4, pCtrl->targetRefreshRate & 0x1,
                                        NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_MSB_MASK,
                                        NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_MSB_SHIFT);
        }
    }
    else
    {
        nvt_nvu8_set_bits(pSdp->payload.db0, NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_AVT_VARIABLE,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_MASK,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_SHIFT);
    }

    if (pCtrl->minVTotal)
    {
        nvt_nvu8_set_bits(pSdp->payload.db1, pCtrl->minVTotal & 0xff,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_LSB_MASK,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_LSB_SHIFT);
        nvt_nvu8_set_bits(pSdp->payload.db2, (pCtrl->minVTotal & 0xff00) >> 8,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB2_MIN_VTOTAL_MSB_MASK,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB2_MIN_VTOTAL_MSB_SHIFT);
    }

    if (pCtrl->bRefreshRateDivider)
    {
        nvt_nvu8_set_bits(pSdp->payload.db4, NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_ENABLE,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_MASK,
                                    NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_SHIFT);
    }
}

// Enumerate Psf Timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumNvPsfTiming(NvU32 nvPsfFormat, NVT_TIMING *pT)
{
    if (pT == NULL || nvPsfFormat == 0 || nvPsfFormat > MAX_PSF_FORMAT)
    {
        return NVT_STATUS_ERR;
    }

    *pT = PSF_TIMING[nvPsfFormat - 1];

    // calculate the pixel clock
    pT->pclk  = RRx1kToPclk (pT);

    return NVT_STATUS_SUCCESS;
}

// Set ActiveSpace for HDMI 3D stereo timing
CODE_SEGMENT(PAGE_DD_CODE)
void SetActiveSpaceForHDMI3DStereo(const NVT_TIMING *pTiming, NVT_EXT_TIMING *pExtTiming)
{
    // Note -- this assumes that the Timng is the 2D instance.
    NvU16 VBlank;

    // assume no active space to start.
    pExtTiming->HDMI3D.VActiveSpace[0] = 0;
    pExtTiming->HDMI3D.VActiveSpace[1] = 0;

    if (NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK == pExtTiming->HDMI3D.StereoStructureType)
    {
        VBlank = pTiming->VTotal - pTiming->VVisible;
        if (pTiming->interlaced)
        {
            //++++ This need to be revisited, not sure when active space 1 & 2 should be different.
            // (fortunately, we are not supporting any interlaced packed frame modes yet).
            pExtTiming->HDMI3D.VActiveSpace[0] = VBlank + 1;
            pExtTiming->HDMI3D.VActiveSpace[1] = VBlank - 1;
        }
        else
        {
            pExtTiming->HDMI3D.VActiveSpace[0] = VBlank;
        }
    }
    return;
}

// Generate HDMI stereo timing from 2D timing
CODE_SEGMENT(PAGE_DD_CODE)
void NvTiming_GetHDMIStereoTimingFrom2DTiming(const NVT_TIMING *pTiming, NvU8 StereoStructureType, NvU8 SideBySideHalfDetail, NVT_EXT_TIMING *pExtTiming)
{
    NvU16 VBlank;
    NvU16 HBlank;

    if ((NULL == pTiming) || (NULL == pExtTiming) || (!isHdmi3DStereoType(StereoStructureType)))
    {
        return;
    }
    // init the extended timing
    NVMISC_MEMSET(pExtTiming, 0, sizeof(NVT_EXT_TIMING));

    // copy the 2D timing to the 3D timing.
    pExtTiming->timing = *pTiming;

    // init the extension w/in the 3D timing
    pExtTiming->HDMI3D.StereoStructureType  = StereoStructureType;
    pExtTiming->HDMI3D.SideBySideHalfDetail = SideBySideHalfDetail;


    switch(StereoStructureType)
    {
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK:
        {
            // calculate VBlank
            VBlank = pTiming->VTotal - pTiming->VVisible;

            // Use the 2D timing to calculate the Active Space
            SetActiveSpaceForHDMI3DStereo(pTiming, pExtTiming);

            // Calculate the 3D VVisible size based on the 2D VVisible and the active space.
            if (pTiming->interlaced)
            {
                pExtTiming->timing.VVisible = ((pTiming->VVisible * 4) + (pExtTiming->HDMI3D.VActiveSpace[0]) * 2) + pExtTiming->HDMI3D.VActiveSpace[1];
            }
            else
            {
                pExtTiming->timing.VVisible = (pTiming->VVisible * 2) + pExtTiming->HDMI3D.VActiveSpace[0];
            }
            // Calculate the 3D VTotal from the 3D VVisible & the VBlank.
            pExtTiming->timing.VTotal = pExtTiming->timing.VVisible + VBlank;

            pExtTiming->timing.etc.status = NVT_SET_TIMING_STATUS_TYPE(pExtTiming->timing.etc.status, NVT_TYPE_HDMI_STEREO);

            break;
        }
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEFULL:
        {
            // calculate HBlank before calculating new HVisible
            HBlank = pTiming->HTotal - pTiming->HVisible;

            pExtTiming->timing.HVisible = pTiming->HVisible * 2;

            pExtTiming->timing.HTotal   = pExtTiming->timing.HVisible + HBlank;

            pExtTiming->timing.etc.status = NVT_SET_TIMING_STATUS_TYPE(pExtTiming->timing.etc.status, NVT_TYPE_HDMI_STEREO);

            break;
        }
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF:     // valid formats with no timing changes.
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM:
        {
            break;
        }
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_FIELD_ALT:          // formats we are not supporting.
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_LINE_ALT:
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTH:
    case NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTHGFX:
        {
            break;
        }
    }
    // calculate the pixel clock
    pExtTiming->timing.pclk  = RRx1kToPclk (&(pExtTiming->timing));
    return;
}

// Add mode to 3D stereo support map
CODE_SEGMENT(PAGE_DD_CODE)
void AddModeToSupportMap(HDMI3DSUPPORTMAP * pMap, NvU8 Vic, NvU8 StereoStructureType, NvU8 SideBySideHalfDetail)
{
    NvU32 i;

    if (0 < Vic)
    {
        // first check if the vic is already listed.
        for (i = 0; i < pMap->total; ++i)
        {
            if (pMap->map[i].Vic == Vic)
            {
                break;
            }
        }
        if (i == pMap->total)
        {
            // vic is not in the map.
            // add it.
            // note that we can't add the VIC to one of the 1st 16 entries.
            // 1st 16 entries in the map are reserved for the vics from the EDID.
            // if we add this VIC to the 1st 16, & there are any optional modes listed,
            // the optional mode(s) will be improperly applied to this VIC as well
            i = MAX(MAX_EDID_ADDRESSABLE_3D_VICS, pMap->total);
            if (i < MAX_3D_VICS_SUPPORTED)
            {
                pMap->map[i].Vic = Vic;
                pMap->total = i + 1;
            }
        }
        nvt_assert(pMap->total <= MAX_3D_VICS_SUPPORTED);
        if (i < pMap->total)
        {
            pMap->map[i].StereoStructureMask = pMap->map[i].StereoStructureMask | NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(StereoStructureType);
            if (NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF == StereoStructureType)
            {
                pMap->map[i].SideBySideHalfDetail = SideBySideHalfDetail;
            }
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidHdmiLlcBasicInfo(VSDB_DATA *pVsdb, NVT_HDMI_LLC_INFO *pHdmiLlc)
{
    NVT_HDMI_LLC_VSDB_PAYLOAD *p;
    if (pVsdb == NULL || pHdmiLlc == NULL)
    {
        return;
    }
    
    p = (NVT_HDMI_LLC_VSDB_PAYLOAD *)(&pVsdb->vendor_data);

    // Minimum vendor_data_size is 2
    pHdmiLlc->addrA = p->A;
    pHdmiLlc->addrB = p->B;
    pHdmiLlc->addrC = p->C;
    pHdmiLlc->addrD = p->D;

    // If more data is provided, we read it as well each field at a time up to video latency
    if (pVsdb->vendor_data_size >= 3)
    {
        pHdmiLlc->supports_AI  = p->Supports_AI;
        pHdmiLlc->dc_48_bit    = p->DC_48bit;
        pHdmiLlc->dc_36_bit    = p->DC_36bit;
        pHdmiLlc->dc_30_bit    = p->DC_30bit;
        pHdmiLlc->dc_y444      = p->DC_Y444;
        pHdmiLlc->dual_dvi     = p->DVI_Dual;

        if (pVsdb->vendor_data_size >= 4)
        {
            pHdmiLlc->max_tmds_clock        = p->Max_TMDS_Clock;

            if (pVsdb->vendor_data_size >= 5)
            {
                pHdmiLlc->latency_field_present     = p->Latency_Fields_Present;
                pHdmiLlc->i_latency_field_present   = p->I_Latency_Fields_Present;
                pHdmiLlc->hdmi_video_present        = p->HDMI_Video_present;
                pHdmiLlc->cnc3 = p->CNC3;
                pHdmiLlc->cnc2 = p->CNC2;
                pHdmiLlc->cnc1 = p->CNC1;
                pHdmiLlc->cnc0 = p->CNC0;
            }
        }
    }

}

// get HDMI 1.4 specific timing (3D stereo timings and extended mode timings)
CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidHDMILLCTiming(NVT_EDID_INFO *pInfo, VSDB_DATA *pVsdb, NvU32 *pMapSz, HDMI3DSUPPORTMAP * pM)
{
    NVT_HDMI_LLC_VSDB_PAYLOAD   *pHdmiLLC;
    NVT_HDMI_VIDEO              *pHDMIVideo;
    NvU32                       DataCnt = 0;
    NvU32                       DataSz;
    NvU16                       i, j, k;
    NvU16                       Supports50Hz;
    NvU16                       Supports60Hz;
    NvU32                       vendorDataSize;

    if ((NULL == pInfo) || (NULL == pVsdb) || (NULL == pM))
    {
        return;
    }

    // init the support map
    NVMISC_MEMSET(pM, 0, sizeof(HDMI3DSUPPORTMAP));
    Supports50Hz = 0;
    Supports60Hz = 0;

    nvt_assert(pInfo->total_timings <= COUNT(pInfo->timing));

    for (i = 0; i < pInfo->total_timings; ++i)
    {
        if (NVT_GET_TIMING_STATUS_TYPE(pInfo->timing[i].etc.status) == NVT_TYPE_EDID_861ST)
        {
            if (MAX_EDID_ADDRESSABLE_3D_VICS > pM->total)
            {
                // fill in the VICs from the EDID (up to the 1st 16). These are used for applying any 3D optional modes listed in the LLC
                // -- the optional modes are addressed based on their relative location within the EDID.
                pM->map[pM->total].Vic = (NvU8) NVT_GET_TIMING_STATUS_SEQ(pInfo->timing[i].etc.status);
                ++pM->total;
            }

            // since we are spinning through the timing array anyway,
            // check to see which refresh rates are supported.
            if (50 == pInfo->timing[i].etc.rr)
            {
                Supports50Hz = 1;
            }
            else if (60 == pInfo->timing[i].etc.rr)
            {
                Supports60Hz = 1;
            }
        }
    }

    if (0 == pM->total)
    {
        if (NULL != pMapSz)
        {
            *pMapSz = 0;
        }
    }

    vendorDataSize = pVsdb->vendor_data_size;
    if ((NVT_CEA861_HDMI_IEEE_ID == pVsdb->ieee_id) &&
        (offsetof(NVT_HDMI_LLC_VSDB_PAYLOAD, Data) < vendorDataSize))
    {
        pHdmiLLC = (NVT_HDMI_LLC_VSDB_PAYLOAD *)(&pVsdb->vendor_data);
        DataSz = (NvU32) MIN(vendorDataSize - offsetof(NVT_HDMI_LLC_VSDB_PAYLOAD, Data), sizeof(pHdmiLLC->Data));

        if (5 <= vendorDataSize)
        {
            if (pHdmiLLC->Latency_Fields_Present)
            {
                DataCnt += (NvU32) sizeof(NVT_CEA861_LATENCY);

                if (pHdmiLLC->I_Latency_Fields_Present)
                {
                    DataCnt += (NvU32) sizeof(NVT_CEA861_LATENCY);
                }
            }

            if ((pHdmiLLC->HDMI_Video_present) &&
                (DataSz > DataCnt) &&
                (DataSz - DataCnt >= sizeof(NVT_HDMI_VIDEO)))
            {
                pHDMIVideo = (NVT_HDMI_VIDEO *) &pHdmiLLC->Data[DataCnt];
                DataCnt += (NvU32) sizeof(NVT_HDMI_VIDEO);

                // If 3D is present, then add the basic 3D modes 1st.
                if (pHDMIVideo->ThreeD_Present)
                {
                    if ((0 != Supports50Hz) || (0 != Supports60Hz))
                    {
                        // 50 and / or 60 Hz is supported, add 1920 x 1080 @ 24Hz 3D modes.
                        AddModeToSupportMap(pM, 32, NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK, 0);       // 1920 x 1080p @ 24 Hz
                        AddModeToSupportMap(pM, 32, NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM, 0);       // 1920 x 1080p @ 24 Hz

                        if (0 != Supports50Hz)
                        {
                            // add the mandatory modes for 50 Hz
                            AddModeToSupportMap(pM, 19, NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK, 0);       // 1280 x  720p @ 50 Hz
                            AddModeToSupportMap(pM, 19, NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM, 0);       // 1280 x  720p @ 50 Hz
                                                                                                        // 1920 x 1080i @ 50 Hz
                            AddModeToSupportMap(pM, 20, NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH);
                        }

                        if (0 != Supports60Hz)
                        {
                            // add the mandatory modes for 60 Hz
                            AddModeToSupportMap(pM,  4, NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK, 0);       // 1280 x  720p @ 60 Hz
                            AddModeToSupportMap(pM,  4, NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM, 0);       // 1280 x  720p @ 60 Hz
                                                                                                        // 1920 x 1080i @ 60 Hz
                            AddModeToSupportMap(pM,  5, NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF, NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH);
                        }
                    }
                }

                if ((DataSz > DataCnt) &&
                    (DataSz - DataCnt >= pHDMIVideo->HDMI_VIC_Len))
                {
                    // handle HDMI VIC entries to add HDMI 1.4a 4kx2k extended modes
                    NVT_HDMI_VIC_LIST * pVicList = (NVT_HDMI_VIC_LIST *) &pHdmiLLC->Data[DataCnt];

                    for ( k = 0; k < pHDMIVideo->HDMI_VIC_Len; ++k)
                    {
                        NVT_TIMING newTiming;

                        // extended mode VIC code from 1 - 4.
                        if ((0 < pVicList->HDMI_VIC[k]) && (pVicList->HDMI_VIC[k] <= MAX_HDMI_EXT_4Kx2K_FORMAT))
                        {
                            NVMISC_MEMCPY(&newTiming,
                                          &HDMI_EXT_4Kx2K_TIMING[pVicList->HDMI_VIC[k] - 1],
                                          sizeof(newTiming));

                            // Fill in the pixel clock
                            newTiming.pclk = RRx1kToPclk(&newTiming);

                            if (!assignNextAvailableTiming(pInfo, &newTiming))
                            {
                                break;
                            }
                        }
                    }

                    DataCnt += pHDMIVideo->HDMI_VIC_Len;
                }

                // the following code implements parsing the HDMI 3D additional modes (all modes bitmap & additional vic modes)
                // Kepler and above support 3D secondary modes
                if ((pHDMIVideo->ThreeD_Present) &&
                    ((1 == pHDMIVideo->ThreeD_Multi_Present) || (2 == pHDMIVideo->ThreeD_Multi_Present)) &&
                    (0 < pHDMIVideo->HDMI_3D_Len) &&
                    (DataSz > (DataCnt + 1)) &&   //make sure pHdmiLLC->Data[DataCnt + 1] is valid
                    (DataSz - DataCnt >= pHDMIVideo->HDMI_3D_Len))
                {
                    NvU16   AllVicStructMask;
                    NvU16   AllVicIdxMask;
                    NvU8    AllVicDetail;

                    // determine which modes to apply to all VICs.
                    AllVicStructMask = (pHdmiLLC->Data[DataCnt] << 8) | pHdmiLLC->Data[DataCnt + 1];
                    AllVicStructMask = AllVicStructMask & NVT_ALL_HDMI_3D_STRUCT_SUPPORTED_MASK;
                    DataCnt += 2;

                    if ((2 == pHDMIVideo->ThreeD_Multi_Present) && (DataSz > (DataCnt+1)))   //make sure pHdmiLLC->Data[DataCnt + 1] is valid
                    {
                        AllVicIdxMask = pHdmiLLC->Data[DataCnt] << 8 | pHdmiLLC->Data[DataCnt + 1];
                        DataCnt += 2;
                    }
                    else
                    {
                        AllVicIdxMask = 0xffff;
                    }

                    // determine what the detail should be.
                    AllVicDetail = 0 != (AllVicStructMask & NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEHALF_MASK) ? NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH : 0;

                    // add the modes to the Support map for all the listed VICs.
                    for (k = 0; k < MIN(MAX_EDID_ADDRESSABLE_3D_VICS, pM->total); ++k)
                    {
                        if ((0 != (AllVicIdxMask & (1 << k))) && (0 != pM->map[k].Vic))
                        {
                            pM->map[k].StereoStructureMask  = pM->map[k].StereoStructureMask | AllVicStructMask;
                            pM->map[k].SideBySideHalfDetail = AllVicDetail;
                        }
                    }
                }

                // handle any additional per vic modes listed in the EDID
                while (DataSz > DataCnt)
                {
                    // get a pointer to the entry.
                    NVT_3D_MULTI_LIST * pMultiListEntry = (NVT_3D_MULTI_LIST *) &pHdmiLLC->Data[DataCnt];

                    // apply the specified structure to the Support Map
                    pM->map[pMultiListEntry->TwoD_VIC_order].StereoStructureMask = 
                        pM->map[pMultiListEntry->TwoD_VIC_order].StereoStructureMask | NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(pMultiListEntry->ThreeD_Structure);

                    // increment the Data count by 2 if this is side by side half,
                    // or 1 if it is any other structure.
                    if (NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF <= pMultiListEntry->ThreeD_Structure)
                    {
                        pM->map[pMultiListEntry->TwoD_VIC_order].SideBySideHalfDetail = pMultiListEntry->ThreeD_Detail;
                        DataCnt += 2;
                    }
                    else
                    {
                        pM->map[pMultiListEntry->TwoD_VIC_order].SideBySideHalfDetail = 0;
                        DataCnt += 1;
                    }
                }
            }
        }
    }
        

    // compress out entries where there is no 3D support.
    for (i = 0, j = 0; i < pM->total; ++i)
    {
        if (0 != pM->map[i].StereoStructureMask)
        {
            pM->map[j] = pM->map[i];
            ++j;
        }
    }

    pM->total = j;

    if (NULL != pMapSz)
    {
        *pMapSz = pM->total;
    }
}

// get HDMI 1.4 3D mandatory stereo format datail base on the input vic.
// If the vic is not in the mandatory format list, return error.
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetHDMIStereoMandatoryFormatDetail(const NvU8 vic, NvU16 *pStereoStructureMask, NvU8 *pSideBySideHalfDetail)
{
    NvU32   i;

    if ((vic < 1) || (vic > MAX_CEA861B_FORMAT))
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i < MAX_HDMI_MANDATORY_3D_FORMAT; i++)
    {
        if (vic == HDMI_MANDATORY_3D_FORMATS[i].Vic)
        {
            if (pStereoStructureMask != NULL)
            {
                *pStereoStructureMask  = HDMI_MANDATORY_3D_FORMATS[i].StereoStructureMask;
            }

            if (pSideBySideHalfDetail != NULL)
            {
                *pSideBySideHalfDetail = HDMI_MANDATORY_3D_FORMATS[i].SideBySideHalfDetail;
            }

            return NVT_STATUS_SUCCESS;
        }
    }

    return NVT_STATUS_ERR;
}
// return the aspect ratio of a given CEA/EIA 861 timing
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 getCEA861TimingAspectRatio(NvU32 vic)
{
    return (vic > 0 && vic < MAX_CEA861B_FORMAT + 1) ? EIA861B[vic-1].etc.aspect : 0;
}

// expose the HDMI extended video timing defined by the HDMI LLC VSDB
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumHdmiVsdbExtendedTiming(NvU32 hdmi_vic, NVT_TIMING *pT)
{
    if (hdmi_vic > MAX_HDMI_EXT_4Kx2K_FORMAT || hdmi_vic == 0 || pT == NULL)
    {
        return NVT_STATUS_ERR;
    }
    *pT = HDMI_EXT_4Kx2K_TIMING[hdmi_vic - 1];
    pT->pclk = RRx1kToPclk(pT);
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidNvidiaVSDBBlock(VSDB_DATA *pVsdb, NVDA_VSDB_PARSED_INFO *vsdbInfo)
{    
    NVT_NVDA_VSDB_PAYLOAD       *pNvda;

    if ((pVsdb == NULL) || (vsdbInfo == NULL))
    {
        return;
    }

    if ((NVT_CEA861_NVDA_IEEE_ID == pVsdb->ieee_id) &&
        (pVsdb->vendor_data_size >= sizeof(NVT_NVDA_VSDB_PAYLOAD)))
    {
        pNvda = (NVT_NVDA_VSDB_PAYLOAD *)(&pVsdb->vendor_data);

        // only version 0x1 is supported 
        if (pNvda->opcode == 0x1)
        {
            vsdbInfo->vsdbVersion = pNvda->opcode;
        }

        switch (vsdbInfo->vsdbVersion)
        {
            case 1:
                vsdbInfo->valid = NV_TRUE;
                vsdbInfo->vrrData.v1.supportsVrr = NV_TRUE;
                vsdbInfo->vrrData.v1.minRefreshRate = pNvda->vrrMinRefreshRate;
                break;

            default:
                break;
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidMsftVsdbBlock(VSDB_DATA *pVsdb, MSFT_VSDB_PARSED_INFO *pVsdbInfo)
{
    if ((pVsdb == NULL) || (pVsdbInfo == NULL))
    {
        return;
    }

    NVMISC_MEMSET(pVsdbInfo, 0, sizeof(MSFT_VSDB_PARSED_INFO));

    if ((NVT_CEA861_MSFT_IEEE_ID == pVsdb->ieee_id) &&
        (pVsdb->vendor_data_size >= sizeof(NVT_MSFT_VSDB_PAYLOAD)))
    {
        NvU32 i = 0;
        NVT_MSFT_VSDB_PAYLOAD *pMsftVsdbPayload = (NVT_MSFT_VSDB_PAYLOAD *)(&pVsdb->vendor_data);

        pVsdbInfo->version = pMsftVsdbPayload->version;

        if (pVsdbInfo->version >= 1) 
        {
            for (i = 0; i < MSFT_VSDB_CONTAINER_ID_SIZE; i++) 
            {
                pVsdbInfo->containerId[i] = pMsftVsdbPayload->containerId[i];
            }

            pVsdbInfo->desktopUsage = pMsftVsdbPayload->desktopUsage;
            pVsdbInfo->thirdPartyUsage = pMsftVsdbPayload->thirdPartyUsage;
            pVsdbInfo->valid = NV_TRUE;
        }
        // Version 3 is the latest version of MSFT VSDB at the time of writing this code
        // Any update from newer version will be ignored and be parsed as Version 3, till 
        // we have an explicit handling for newer version here.
        if (pVsdbInfo->version >= 3)
        {
            // Primary Use case is valid from Version 3 and is ignored on previous versions.
            pVsdbInfo->primaryUseCase = pMsftVsdbPayload->primaryUseCase;
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseEdidHdmiForumVSDB(VSDB_DATA *pVsdb, NVT_HDMI_FORUM_INFO *pHdmiInfo)
{
    NVT_HDMI_FORUM_VSDB_PAYLOAD *pHdmiForum;
    NvU32 remainingSize;

    if ((pVsdb == NULL) || pHdmiInfo == NULL)
    {
        return;
    }
    
    pHdmiForum = (NVT_HDMI_FORUM_VSDB_PAYLOAD *)(&pVsdb->vendor_data);
    switch(pHdmiForum->Version)
    {
        case 1:
            // From HDMI spec the payload data size is from 7 to 31
            // In parseCta861DataBlockInfo(), the payload size recorded in pHdmiForum is 
            //    subtracted by 3. Thus the expected range here is 4 - 28.
            // Assert if the the vendor_data_size < 4.
            nvt_assert(pVsdb->vendor_data_size >= 4);

            remainingSize  =  pVsdb->vendor_data_size;
            
            // second byte
            pHdmiInfo->max_TMDS_char_rate   = pHdmiForum->Max_TMDS_Character_Rate;
           
            // third byte
            pHdmiInfo->threeD_Osd_Disparity = pHdmiForum->ThreeD_Osd_Disparity;
            pHdmiInfo->dual_view            = pHdmiForum->Dual_View;
            pHdmiInfo->independent_View     = pHdmiForum->Independent_View;
            pHdmiInfo->lte_340Mcsc_scramble = pHdmiForum->Lte_340mcsc_Scramble;
            pHdmiInfo->ccbpci               = pHdmiForum->CCBPCI;
            pHdmiInfo->cable_status         = pHdmiForum->CABLE_STATUS;
            pHdmiInfo->rr_capable           = pHdmiForum->RR_Capable;
            pHdmiInfo->scdc_present         = pHdmiForum->SCDC_Present;
            
            // fourth byte
            pHdmiInfo->dc_30bit_420         = pHdmiForum->DC_30bit_420;
            pHdmiInfo->dc_36bit_420         = pHdmiForum->DC_36bit_420;
            pHdmiInfo->dc_48bit_420         = pHdmiForum->DC_48bit_420;
            pHdmiInfo->uhd_vic              = pHdmiForum->UHD_VIC;
            pHdmiInfo->max_FRL_Rate         = pHdmiForum->Max_FRL_Rate;

            remainingSize -= 4;
            
            // fifth byte
            if (!remainingSize--)
            {
                break;
            }
            pHdmiInfo->fapa_start_location  = pHdmiForum->FAPA_start_location;
            pHdmiInfo->allm                 = pHdmiForum->ALLM;
            pHdmiInfo->fva                  = pHdmiForum->FVA;
            pHdmiInfo->cnmvrr               = pHdmiForum->CNMVRR;
            pHdmiInfo->cinemaVrr            = pHdmiForum->CinemaVRR;
            pHdmiInfo->m_delta              = pHdmiForum->M_delta;
            pHdmiInfo->qms                  = pHdmiForum->QMS;
            pHdmiInfo->fapa_end_extended    = pHdmiForum->FAPA_End_Extended;
            
            // sixth byte
            if (!remainingSize--)
            {
                break;
            }
            pHdmiInfo->vrr_min              = pHdmiForum->VRR_min;
            pHdmiInfo->vrr_max              = ((NvU16)pHdmiForum->VRR_max_high) << 8;
            
            // seventh byte
            if (!remainingSize--)
            {
                break;
            }
            pHdmiInfo->vrr_max             |= (pHdmiForum->VRR_max_low);
           
            // eighth byte
            if (!remainingSize--)
            {
                break;
            }
            pHdmiInfo->dsc_10bpc            = pHdmiForum->DSC_10bpc;
            pHdmiInfo->dsc_12bpc            = pHdmiForum->DSC_12bpc;
            pHdmiInfo->dsc_16bpc            = pHdmiForum->DSC_16bpc;
            pHdmiInfo->dsc_All_bpp          = pHdmiForum->DSC_All_bpp;
            pHdmiInfo->dsc_Native_420       = pHdmiForum->DSC_Native_420;
            pHdmiInfo->dsc_1p2              = pHdmiForum->DSC_1p2;
            pHdmiInfo->qms_tfr_min          = pHdmiForum->QMS_TFR_min;
            pHdmiInfo->qms_tfr_max          = pHdmiForum->QMS_TFR_max;

            // ninth byte
            if (!remainingSize--)
            {
                break;
            }
            pHdmiInfo->dsc_MaxSlices          = 0;
            pHdmiInfo->dsc_MaxPclkPerSliceMHz = 0;
            switch(pHdmiForum->DSC_MaxSlices)
            {
            case 7: pHdmiInfo->dsc_MaxSlices = 16; pHdmiInfo->dsc_MaxPclkPerSliceMHz = 400; break;
            case 6: pHdmiInfo->dsc_MaxSlices = 12; pHdmiInfo->dsc_MaxPclkPerSliceMHz = 400; break;
            case 5: pHdmiInfo->dsc_MaxSlices = 8;  pHdmiInfo->dsc_MaxPclkPerSliceMHz = 400; break;
            case 4: pHdmiInfo->dsc_MaxSlices = 8;  pHdmiInfo->dsc_MaxPclkPerSliceMHz = 340; break;
            case 3: pHdmiInfo->dsc_MaxSlices = 4;  pHdmiInfo->dsc_MaxPclkPerSliceMHz = 340; break;
            case 2: pHdmiInfo->dsc_MaxSlices = 2;  pHdmiInfo->dsc_MaxPclkPerSliceMHz = 340; break;
            case 1: pHdmiInfo->dsc_MaxSlices = 1;  pHdmiInfo->dsc_MaxPclkPerSliceMHz = 340; break;
            default: break;
            }

            pHdmiInfo->dsc_Max_FRL_Rate     = pHdmiForum->DSC_Max_FRL_Rate;
          
            // tenth byte
            if (!remainingSize--)
            {
                break;
            }

            // Per spec, number of bytes has to be computed as 1024 x (1 + DSC_TotalChunkKBytes). 
            // For driver parser purposes, add 1 here so that the field means max num of KBytes in a link of chunks
            pHdmiInfo->dsc_totalChunkKBytes = (pHdmiForum->DSC_totalChunkKBytes == 0) ? 0 : pHdmiForum->DSC_totalChunkKBytes + 1;
            break;

        default:
            break;

    }    
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCea861Hdr10PlusDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag)
{
    NVT_EDID_INFO           *pInfo          = NULL;
    NVT_DISPLAYID_2_0_INFO  *pDisplayID20   = NULL;
    NVT_HDR10PLUS_INFO      *pHdr10PlusInfo = NULL;

    if (pExt861 == NULL || pRawInfo == NULL)
        return;

    if(pExt861->vsvdb.ieee_id != NVT_CEA861_HDR10PLUS_IEEE_ID) 
        return;

    if (flag == FROM_CTA861_EXTENSION || flag == FROM_DISPLAYID_13_DATA_BLOCK)
    {
        pInfo = (NVT_EDID_INFO *)pRawInfo;
        pHdr10PlusInfo = &pInfo->hdr10PlusInfo;
    }
    else if (flag == FROM_DISPLAYID_20_DATA_BLOCK)
    {
        pDisplayID20 = (NVT_DISPLAYID_2_0_INFO *)pRawInfo;
        pHdr10PlusInfo = &pDisplayID20->cta.hdr10PlusInfo;
    }
    else
    {
        return;
    }

    NVMISC_MEMSET(pHdr10PlusInfo, 0, sizeof(NVT_HDR10PLUS_INFO));

    if (pExt861->vsvdb.vendor_data_size < sizeof(NVT_HDR10PLUS_INFO))
        return;

    NVMISC_MEMCPY(pHdr10PlusInfo, &pExt861->vsvdb.vendor_data, sizeof(NVT_HDR10PLUS_INFO));
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861DIDType7VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo)
{
    NvU8 i = 0;
    NvU8 t7db_idx = 0;
    NvU8 startSeqNum = 0;

    NVT_TIMING newTiming;
    NVT_EDID_INFO *pInfo = (NVT_EDID_INFO *)pRawInfo;
    const DISPLAYID_2_0_TIMING_7_DESCRIPTOR *pT7Descriptor = NULL;
    NvU8 eachOfDescSize = sizeof(DISPLAYID_2_0_TIMING_7_DESCRIPTOR);

    for (t7db_idx = 0; t7db_idx < pExt861->total_did_type7db; t7db_idx++)
    {
        // 20 bytes
        eachOfDescSize += pExt861->did_type7_data_block[t7db_idx].version.t7_m;

        if (pExt861->did_type7_data_block[t7db_idx].total_descriptors != NVT_CTA861_DID_TYPE7_DESCRIPTORS_MAX)
        {
            nvt_assert(0 && "payload descriptor invalid. expect T7VTDB only 1 descriptor");
            continue;
        }

        if (pExt861->did_type7_data_block[t7db_idx].version.revision != 2 )
        {
            nvt_assert(0 && "The revision supported by CTA-861 is not 2");
        }

        startSeqNum = getExistedCTATimingSeqNumber(pInfo, NVT_TYPE_CTA861_DID_T7);

        for (i = 0; i < pExt861->did_type7_data_block[i].total_descriptors; i++)
        {
            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));
            if (NVT_STATUS_SUCCESS == parseDisplayId20Timing7Descriptor(&pExt861->did_type7_data_block[t7db_idx].payload[i*eachOfDescSize],
                                                                        &newTiming, 
                                                                        startSeqNum+i))
            {
                // T7VTDB shall not be used with video timing that can be expressed in an 18-byte DTD
                if (newTiming.HVisible < 4096 && newTiming.VVisible < 4096 && newTiming.pclk < 65536) 
                {
                    nvt_assert(0 && "The timing can be expressed in an 18-byte DTD");
                    continue;
                }

                pT7Descriptor = (const DISPLAYID_2_0_TIMING_7_DESCRIPTOR *)
                                    &pExt861->did_type7_data_block[t7db_idx].payload[i*eachOfDescSize];
                
                if (pT7Descriptor->options.is_preferred_or_ycc420 == 1 && newTiming.pclk > NVT_HDMI_YUV_420_PCLK_SUPPORTED_MIN)
                {
                    newTiming.etc.yuv420.bpcs = 0;
                    UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1,
                                               pInfo->hdmiForumInfo.dc_30bit_420,
                                               pInfo->hdmiForumInfo.dc_36bit_420, 0,
                                               pInfo->hdmiForumInfo.dc_48bit_420);
                }

                NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name), "CTA861-T7:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                     (int)NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status),
                                                                                     (int)newTiming.HVisible, 
                                                                                     (int)newTiming.VVisible,
                                                                                     (int)newTiming.etc.rrx1k/1000, 
                                                                                     (int)newTiming.etc.rrx1k%1000, 
                                                                                     (newTiming.interlaced ? "I":"P"));
                newTiming.etc.status = NVT_STATUS_CTA861_DID_T7N(NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status));
                newTiming.etc.name[sizeof(newTiming.etc.name) - 1] = '\0';
                newTiming.etc.rep = 0x1;

                if (!assignNextAvailableTiming(pInfo, &newTiming))
                {
                    break;
                }
            }
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861DIDType8VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo)
{
    NvU8 i = 0;
    NvU8 t8db_idx = 0;
    NvU8 startSeqNum = 0;
    NvU8 codeSize = 0;
    NvU8 codeType = 0;

    NVT_TIMING newTiming;
    NVT_EDID_INFO *pInfo = (NVT_EDID_INFO *)pRawInfo;

    for (t8db_idx = 0; t8db_idx < pExt861->total_did_type8db; t8db_idx++)
    {
        codeType = pExt861->did_type8_data_block[t8db_idx].version.code_type;
        codeSize = pExt861->did_type8_data_block[t8db_idx].version.tcs;

        if (codeType != 0 /*DMT*/)
        {
            nvt_assert(0 && "Not DMT code type!");
            continue;
        }

        startSeqNum = getExistedCTATimingSeqNumber(pInfo, NVT_TYPE_CTA861_DID_T8);

        for (i=0; i < pExt861->did_type8_data_block[t8db_idx].total_descriptors; i++)
        {
            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

            if (parseDisplayId20Timing8Descriptor(pExt861->did_type8_data_block[t8db_idx].payload, 
                                                  &newTiming, codeType, codeSize, i, startSeqNum + i) == NVT_STATUS_SUCCESS)
            {
                if (pExt861->did_type8_data_block[t8db_idx].version.t8y420 == 1 && newTiming.pclk > NVT_HDMI_YUV_420_PCLK_SUPPORTED_MIN)
                {
                    UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1,
                                                pInfo->hdmiForumInfo.dc_30bit_420,
                                                pInfo->hdmiForumInfo.dc_36bit_420, 0,
                                                pInfo->hdmiForumInfo.dc_48bit_420);
                }
                NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name), "CTA861-T8:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                    (int)NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status),
                                                                                    (int)newTiming.HVisible, (int)newTiming.VVisible,
                                                                                    (int)newTiming.etc.rrx1k/1000, (int)newTiming.etc.rrx1k%1000, 
                                                                                    (newTiming.interlaced ? "I":"P"));
                newTiming.etc.status = NVT_STATUS_CTA861_DID_T8N(NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status));
                newTiming.etc.name[sizeof(newTiming.etc.name) - 1] = '\0';
                newTiming.etc.rep = 0x1;

                if (!assignNextAvailableTiming(pInfo, &newTiming))
                {
                    break;
                }
            }
        }
    }
}

CODE_SEGMENT(PAGE_DD_CODE)
void parseCta861DIDType10VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo)
{
    NvU8 i = 0;    
    NvU8 t10db_idx = 0;
    NvU8 startSeqNum = 0;

    NVT_TIMING newTiming;
    NVT_EDID_INFO *pInfo = (NVT_EDID_INFO *)pRawInfo;
    const DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR *p6bytesDescriptor = NULL;
    NvU8 eachOfDescriptorsSize = sizeof (DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR);

    for (t10db_idx = 0; t10db_idx < pExt861->total_did_type10db; t10db_idx++)
    {
        startSeqNum = getExistedCTATimingSeqNumber(pInfo, NVT_TYPE_CTA861_DID_T10);

        // 6 or 7 bytes length
        eachOfDescriptorsSize += pExt861->did_type10_data_block[t10db_idx].version.t10_m;

        for (i = 0; i < pExt861->did_type10_data_block[t10db_idx].total_descriptors; i++)
        {
            if (pExt861->did_type10_data_block[t10db_idx].total_descriptors < NVT_CTA861_DID_TYPE10_DESCRIPTORS_MIN || 
                pExt861->did_type10_data_block[t10db_idx].total_descriptors > NVT_CTA861_DID_TYPE10_DESCRIPTORS_MAX)
            {
                nvt_assert(0 && "payload descriptor invalid. expect T10VTDB has minimum 1 descriptor, maximum 4 descriptors");
                continue;
            }

            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));
            if (NVT_STATUS_SUCCESS == parseDisplayId20Timing10Descriptor(&pExt861->did_type10_data_block[t10db_idx].payload[i*eachOfDescriptorsSize],
                                                                        &newTiming, 
                                                                        pExt861->did_type10_data_block[t10db_idx].version.t10_m, startSeqNum+i))
            {
                p6bytesDescriptor = (const DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR *)
                                    &pExt861->did_type10_data_block[t10db_idx].payload[i*eachOfDescriptorsSize];

                if (p6bytesDescriptor->options.ycc420_support && newTiming.pclk > NVT_HDMI_YUV_420_PCLK_SUPPORTED_MIN)
                {                 
                    UPDATE_BPC_FOR_COLORFORMAT(newTiming.etc.yuv420, 0, 1, 
                                               pInfo->hdmiForumInfo.dc_30bit_420, 
                                               pInfo->hdmiForumInfo.dc_36bit_420, 0,
                                               pInfo->hdmiForumInfo.dc_48bit_420);
                }

                if (p6bytesDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD)
                {
                    NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name), "CTA861-T10:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                         (int)NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status),
                                                                                         (int)newTiming.HVisible, 
                                                                                         (int)newTiming.VVisible,
                                                                                         (int)newTiming.etc.rrx1k/1000, 
                                                                                         (int)newTiming.etc.rrx1k%1000, 
                                                                                         (newTiming.interlaced ? "I":"P"));
                }
                else
                {
                    NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name), "CTA861-T10RB%d:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                         p6bytesDescriptor->options.timing_formula,
                                                                                         (int)NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status),
                                                                                         (int)newTiming.HVisible, 
                                                                                         (int)newTiming.VVisible,
                                                                                         (int)newTiming.etc.rrx1k/1000, 
                                                                                         (int)newTiming.etc.rrx1k%1000, 
                                                                                         (newTiming.interlaced ? "I":"P"));
                }
                newTiming.etc.status = NVT_STATUS_CTA861_DID_T10N(NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status));
                newTiming.etc.name[sizeof(newTiming.etc.name) - 1] = '\0';
                newTiming.etc.rep = 0x1;

                if (!assignNextAvailableTiming(pInfo, &newTiming))
                {
                    break;
                }
            }
            else
            {
                continue;
            }
        }
    }
}

POP_SEGMENTS
