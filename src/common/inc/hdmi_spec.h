/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _HDMI_SPEC_H_
#define _HDMI_SPEC_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: HDMI_SPEC.H                                                       *
*       Defines Common HDMI flags                                           *
*                                                                           *
\***************************************************************************/

/*
* RM will be moving to separate packet types for DP and HDMI
* since the SDP packet type differ between HDMI and DP. Going forward
* clients are expected to use the respective packet type. Once all the
* clients move to the new data types, we can remove the redundant
* PACKET_TYPE definition.
*/


typedef enum  
{
    pktType_AudioClkRegeneration           = 0x01,
    pktType_GeneralControl                 = 0x03,
    pktType_GamutMetadata                  = 0x0a,
    pktType_SRInfoFrame                    = 0x7f, // Self refresh infoframe for eDP enter/exit self refresh, SRS 1698
    pktType_Cea861BInfoFrame               = 0x80,
    pktType_VendorSpecInfoFrame            = 0x81,
    pktType_AviInfoFrame                   = 0x82,
    pktType_AudioInfoFrame                 = 0x84,
    pktType_SrcProdDescInfoFrame           = 0x83,
    pktType_MpegSrcInfoFrame               = 0x85,
    pktType_DynamicRangeMasteringInfoFrame = 0x87
} PACKET_TYPE; 

typedef enum
{
    hdmi_pktType_AudioClkRegeneration             = 0x01,
    hdmi_pktType_GeneralControl                   = 0x03,
    hdmi_pktType_GamutMetadata                    = 0x0a,
    hdmi_pktType_ExtendedMetadata                 = 0x7f,
    hdmi_pktType_Cea861BInfoFrame                 = 0x80,
    hdmi_pktType_VendorSpecInfoFrame              = 0x81,
    hdmi_pktType_AviInfoFrame                     = 0x82,
    hdmi_pktType_AudioInfoFrame                   = 0x84,
    hdmi_pktType_SrcProdDescInfoFrame             = 0x83,
    hdmi_pktType_MpegSrcInfoFrame                 = 0x85,
    hdmi_pktType_DynamicRangeMasteringInfoFrame   = 0x87
} HDMI_PACKET_TYPE;


#define HDMI_PKT_HDR_SIZE                      3

#define HDMI_PKT_AVI_NUM_DBYTES               14
#define HDMI_PKT_AUDIO_NUM_DBYTES             11
#define HDMI_PKT_GENCTRL_NUM_DBYTES            7
#define HDMI_PKT_ACR_NUM_DBYTES                7
#define HDMI_PKT_GAMUT_METADATA_NUM_DBYTES    28
#define HDMI_PKT_VS_MAX_NUM_DBYTES            28 

#define HDMI_GENCTRL_PACKET_MUTE_ENABLE     0x01
#define HDMI_GENCTRL_PACKET_MUTE_DISABLE    0x10

#endif // #ifndef _HDMI_SPEC_H_
