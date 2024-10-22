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
//  File:       nvt_edidext_displayid.c
//
//  Purpose:    the provide edid related services
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "edid.h"

PUSH_SEGMENTS

static NVT_STATUS parseDisplayIdSection(DISPLAYID_SECTION * section,
                                        NvU32 max_length,
                                        NVT_EDID_INFO *pEdidInfo);

// Specific blocks that can be parsed based on DisplayID
static NVT_STATUS parseDisplayIdProdIdentityBlock(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdParam(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdColorChar(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdTiming1(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTiming2(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTiming3(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTiming4(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTiming5(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTimingVesa(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdTimingEIA(NvU8 * block, NVT_EDID_INFO *pEdidInfo);
static NVT_STATUS parseDisplayIdRangeLimits(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdSerialNumber(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdAsciiString(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdDeviceData(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdInterfacePower(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdTransferChar(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdDisplayInterface(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdStereo(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdTiledDisplay(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);
static NVT_STATUS parseDisplayIdCtaData(NvU8 * block, NVT_EDID_INFO *pInfo);
static NVT_STATUS parseDisplayIdDisplayInterfaceFeatures(NvU8 * block, NVT_DISPLAYID_INFO *pInfo);

static NVT_STATUS parseDisplayIdTiming1Descriptor(DISPLAYID_TIMING_1_DESCRIPTOR * desc, NVT_TIMING *pT);
static NVT_STATUS parseDisplayIdTiming2Descriptor(DISPLAYID_TIMING_2_DESCRIPTOR * desc, NVT_TIMING *pT);
static NVT_STATUS parseDisplayIdTiming3Descriptor(DISPLAYID_TIMING_3_DESCRIPTOR * desc, NVT_TIMING *pT);
static NVT_STATUS parseDisplayIdTiming5Descriptor(DISPLAYID_TIMING_5_DESCRIPTOR * desc, NVT_TIMING *pT);

/**
 * @brief Parses a displayID Extension block, with timings stored in pT and
 *        other info stored in pInfo
 * @param p The EDID Extension Block (With a DisplayID in it)
 * @param size Size of the displayID Extension Block
 * @param pEdidInfo EDID struct containing DisplayID information and
 *                  the timings
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS getDisplayIdEDIDExtInfo(NvU8 *p, NvU32 size,
                                   NVT_EDID_INFO *pEdidInfo)
{
    DISPLAYID_SECTION * section;

    if (p == NULL || size < sizeof(EDIDV1STRUC))
        return NVT_STATUS_ERR;
    if (p[0] != NVT_EDID_EXTENSION_DISPLAYID)
        return NVT_STATUS_ERR;

    section = (DISPLAYID_SECTION *)(p + 1);
    pEdidInfo->ext_displayid.version = section->version;
    if (section->product_type > NVT_DISPLAYID_PROD_MAX_NUMBER)
        return NVT_STATUS_ERR;

    return parseDisplayIdSection(section, sizeof(EDIDV1STRUC) - 1, pEdidInfo);
}

/**
 * @brief updates the color format for each bpc for each timing
 * @param pInfo EDID struct containing DisplayID information and
 *              the timings
 * @param timingIdx Index of the first display ID timing in the
 *                              pInfo->timing[] timing array.
  */
CODE_SEGMENT(PAGE_DD_CODE)
void updateColorFormatForDisplayIdExtnTimings(NVT_EDID_INFO *pInfo,
                                              NvU32 timingIdx)
{
    // pDisplayIdInfo is the parsed display ID info
    NVT_DISPLAYID_INFO *pDisplayIdInfo = &pInfo->ext_displayid;
    NVT_TIMING *pT = &pInfo->timing[timingIdx];

    nvt_assert((timingIdx) <= COUNT(pInfo->timing));

    if ((pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_A_SUPPORTED ||
         pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_B_SUPPORTED ||
         pInfo->ext861.valid.H14B_VSDB || pInfo->ext861.valid.H20_HF_VSDB) && pInfo->ext861.revision >= NVT_CEA861_REV_A)
    {
         if (!pInfo->ext_displayid.supported_displayId2_0)
         {
             UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 0,
                                                        1,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_10b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_12b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_14b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_16b);
         }
         else
         {
             // rgb444 (always support 6bpc and 8bpc as per DP spec 5.1.1.1.1 RGB Colorimetry)
             UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 0,
                                                        1,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_10b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_12b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_14b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_16b);
         }
    }
    else // DP
    {
         if (!pInfo->ext_displayid.supported_displayId2_0)
         {
             UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 1,
                                                        1,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_10b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_12b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_14b,
                                                        pDisplayIdInfo->u4.display_interface.rgb_depth.support_16b);
         }
         else
         {
            // rgb444 (always support 6bpc and 8bpc as per DP spec 5.1.1.1.1 RGB Colorimetry)
             UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 1,
                                                        1,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_10b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_12b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_14b,
                                                        pDisplayIdInfo->u4.display_interface_features.rgb_depth.support_16b);
         }
    }

    if (!pInfo->ext_displayid.supported_displayId2_0)
    {
        // yuv444
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv444, 0, /* yuv444 does not support 6bpc */
                                                    pDisplayIdInfo->u4.display_interface.ycbcr444_depth.support_8b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr444_depth.support_10b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr444_depth.support_12b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr444_depth.support_14b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr444_depth.support_16b);
        // yuv422
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv422, 0, /* yuv422 does not support 6bpc */
                                                    pDisplayIdInfo->u4.display_interface.ycbcr422_depth.support_8b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr422_depth.support_10b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr422_depth.support_12b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr422_depth.support_14b,
                                                    pDisplayIdInfo->u4.display_interface.ycbcr422_depth.support_16b);
    }
    else
    {
        // yuv444
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv444, 0, /* yuv444 does not support 6bpc */
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr444_depth.support_8b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr444_depth.support_10b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr444_depth.support_12b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr444_depth.support_14b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr444_depth.support_16b);
        // yuv422
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv422, 0, /* yuv422 does not support 6bpc */
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr422_depth.support_8b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr422_depth.support_10b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr422_depth.support_12b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr422_depth.support_14b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr422_depth.support_16b);
        // yuv420
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv420, 0, /* yuv420 does not support 6bpc */
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr420_depth.support_8b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr420_depth.support_10b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr420_depth.support_12b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr420_depth.support_14b,
                                                    pDisplayIdInfo->u4.display_interface_features.ycbcr420_depth.support_16b);
    }
}

/**
 * @brief Parses a displayID Section
 * @param section The DisplayID Section to parse
 * @param max_length The indicated total length of the displayID as given (or
 *                   sizeof(EDIDV1STRUCT) for an extension block)
 * @param pEdidInfo EDID struct containing DisplayID information and
 *                  the timings
 */
CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdSection(DISPLAYID_SECTION * section,
                                        NvU32 max_length,
                                        NVT_EDID_INFO *pEdidInfo)
{
    NvU8 block_location = 0;
    NvU8 section_length;
    NvU8 remaining_length;

    if (section == NULL || max_length <= NVT_DISPLAYID_SECTION_HEADER_LEN)
        return NVT_STATUS_ERR;
    if (section->section_bytes > max_length - NVT_DISPLAYID_SECTION_HEADER_LEN)
        return NVT_STATUS_ERR;

    remaining_length = section->section_bytes;

    while (block_location < section->section_bytes)
    {
        DISPLAYID_DATA_BLOCK_HEADER * hdr = (DISPLAYID_DATA_BLOCK_HEADER *) (section->data + block_location);
        NvBool is_prod_id = remaining_length > 3 && block_location == 0 && hdr->type == 0 && hdr->data_bytes > 0;
        NvU8 i;

        // Check the padding.
        if (hdr->type == 0 && !is_prod_id)
        {
            for (i = 1 ; i < remaining_length; i++)
            {
                // All remaining bytes must all be 0.
                if (section->data[block_location + i] != 0)
                {
                    return NVT_STATUS_ERR;
                }
            }

            section_length = remaining_length;
        }
        else
        {
            if (parseDisplayIdBlock((NvU8 *)(section->data + block_location),
                                    section->section_bytes - block_location,
                                    &section_length,
                                    pEdidInfo) != NVT_STATUS_SUCCESS)
                return NVT_STATUS_ERR;
        }

        block_location += section_length;
        remaining_length -= section_length;
    }

    return NVT_STATUS_SUCCESS;
}

/**
 * @brief Parses a displayID data block
 * @param block The DisplayID data block to parse
 * @param max_length The indicated total length of the each data block for checking
 * @param pLength return the indicated length of the each data block
 * @param pEdidInfo EDID struct containing DisplayID information and
 *                  the timings or validation purpose if it is NULL
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS parseDisplayIdBlock(NvU8* pBlock,
                               NvU8 max_length,
                               NvU8* pLength,
                               NVT_EDID_INFO *pEdidInfo)
{
    DISPLAYID_DATA_BLOCK_HEADER * hdr = (DISPLAYID_DATA_BLOCK_HEADER *) pBlock;
    NVT_STATUS ret = NVT_STATUS_SUCCESS;
    NVT_DISPLAYID_INFO *pInfo;

    if (pBlock == NULL || max_length <= NVT_DISPLAYID_DATABLOCK_HEADER_LEN)
        return NVT_STATUS_ERR;

    if (hdr->data_bytes > max_length - NVT_DISPLAYID_DATABLOCK_HEADER_LEN)
        return NVT_STATUS_ERR;

    pInfo = pEdidInfo == NULL ? NULL : &pEdidInfo->ext_displayid;

    *pLength = hdr->data_bytes + NVT_DISPLAYID_DATABLOCK_HEADER_LEN;

    switch (hdr->type)
    {
        case NVT_DISPLAYID_BLOCK_TYPE_PRODUCT_IDENTITY:
            ret = parseDisplayIdProdIdentityBlock(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_PARAM:
            ret = parseDisplayIdParam(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_COLOR_CHAR:
            ret = parseDisplayIdColorChar(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_1:
            ret = parseDisplayIdTiming1(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_2:
            ret = parseDisplayIdTiming2(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_3:
            ret = parseDisplayIdTiming3(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_4:
            ret = parseDisplayIdTiming4(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_5:
            ret = parseDisplayIdTiming5(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_VESA:
            ret = parseDisplayIdTimingVesa(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TIMING_CEA:
            ret = parseDisplayIdTimingEIA(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_RANGE_LIMITS:
            ret = parseDisplayIdRangeLimits(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_SERIAL_NUMBER:
            ret = parseDisplayIdSerialNumber(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_ASCII_STRING:
            ret = parseDisplayIdAsciiString(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_DEVICE_DATA:
            ret = parseDisplayIdDeviceData(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_INTERFACE_POWER:
            ret = parseDisplayIdInterfacePower(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TRANSFER_CHAR:
            ret = parseDisplayIdTransferChar(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_INTERFACE:
            ret = parseDisplayIdDisplayInterface(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_STEREO:
            ret = parseDisplayIdStereo(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_TILEDDISPLAY:
            ret = parseDisplayIdTiledDisplay(pBlock, pInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_CTA_DATA:
            ret = parseDisplayIdCtaData(pBlock, pEdidInfo);
            break;
        case NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_INTERFACE_FEATURES:
            ret = parseDisplayIdDisplayInterfaceFeatures(pBlock, pInfo);
            break;
        default:
            ret = NVT_STATUS_ERR;
            break;
    }

    if (pEdidInfo == NULL) return ret;

    return NVT_STATUS_SUCCESS;
}
CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdColorChar(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    NvU32 i, j;
    NvU16 x_p, y_p;
    DISPLAYID_COLOR_CHAR_BLOCK * blk = (DISPLAYID_COLOR_CHAR_BLOCK *)block;

    /** unused flag - uncomment if you wish to use it in the future
    NvU8 isTemp = DRF_VAL(T_DISPLAYID, _COLOR, _TEMPORAL, blk->point_info);
    */
    NvU8 wp_num = DRF_VAL(T_DISPLAYID, _COLOR, _WHITE_POINTS, blk->point_info);
    NvU8 prim_num = DRF_VAL(T_DISPLAYID, _COLOR, _PRIMARIES, blk->point_info);

    if ((prim_num + wp_num) * sizeof(DISPLAYID_COLOR_POINT) + 1 != blk->header.data_bytes)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    for (i = 0; i < prim_num; i++)
    {
        x_p = (blk->points)[i].color_x_bits_low +
            (DRF_VAL(T_DISPLAYID, _COLOR, _POINT_X, (blk->points)[i].color_bits_mid) << 8);
        y_p = DRF_VAL(T_DISPLAYID, _COLOR, _POINT_Y, (blk->points)[i].color_bits_mid) +
            ((blk->points)[i].color_y_bits_high << 4);
        pInfo->primaries[i].x = x_p;
        pInfo->primaries[i].y = y_p;
    }

    for (j = 0; j < wp_num; j++)
    {
        x_p = (blk->points)[i].color_x_bits_low +
            (DRF_VAL(T_DISPLAYID, _COLOR, _POINT_X, (blk->points)[i].color_bits_mid) << 8);
        y_p = DRF_VAL(T_DISPLAYID, _COLOR, _POINT_Y, (blk->points)[i].color_bits_mid) +
            ((blk->points)[i].color_y_bits_high << 4);
        pInfo->white_points[pInfo->total_primaries + j].x = x_p;
        pInfo->white_points[pInfo->total_primaries + j].y = y_p;

        i++;
    }
    pInfo->total_primaries = prim_num;
    pInfo->total_white_points += wp_num;
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdProdIdentityBlock(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_PROD_IDENTIFICATION_BLOCK * blk = (DISPLAYID_PROD_IDENTIFICATION_BLOCK *)block;
    if (blk->header.data_bytes - blk->productid_string_size != NVT_DISPLAYID_PRODUCT_IDENTITY_MIN_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    pInfo->vendor_id = (blk->vendor)[2] | ((blk->vendor)[1] << 8) | ((blk->vendor)[0] << 16);
    pInfo->product_id = blk->product_code;
    pInfo->serial_number = blk->serial_number;
    pInfo->week = blk->model_tag;
    pInfo->year = blk->model_year;

    if (blk->productid_string_size != 0)
        NVMISC_STRNCPY((char *)pInfo->product_string, (const char *)blk->productid_string, blk->productid_string_size);
    pInfo->product_string[blk->productid_string_size] = '\0';

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdParam(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_DISPLAY_PARAM_BLOCK * blk = (DISPLAYID_DISPLAY_PARAM_BLOCK *)block;
    if (blk->header.data_bytes != NVT_DISPLAYID_DISPLAY_PARAM_BLOCK_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    pInfo->horiz_size         = blk->horizontal_image_size;
    pInfo->vert_size          = blk->vertical_image_size;
    pInfo->horiz_pixels       = blk->horizontal_pixel_count;
    pInfo->vert_pixels        = blk->vertical_pixel_count;

    pInfo->support_audio      = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _SUPPORT_AUDIO,        blk->feature);
    pInfo->separate_audio     = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _SEPARATE_AUDIO,       blk->feature);
    pInfo->audio_override     = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _AUDIO_INPUT_OVERRIDE, blk->feature);
    pInfo->power_management   = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _POWER_MANAGEMENT,     blk->feature);
    pInfo->fixed_timing       = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _FIXED_TIMING,         blk->feature);
    pInfo->fixed_pixel_format = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _FIXED_PIXEL_FORMAT,   blk->feature);
    pInfo->deinterlace        = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _DEINTERLACING,        blk->feature);

    pInfo->gamma              = (NvU16)(blk->transfer_char_gamma - 1) * 100;
    pInfo->aspect_ratio       = blk->aspect_ratio;

    pInfo->depth_overall      = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _DEPTH_OVERALL, blk->color_bit_depth);
    pInfo->depth_native       = DRF_VAL(T_DISPLAYID, _DISPLAY_PARAM, _DEPTH_NATIVE,  blk->color_bit_depth);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming1(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU16 i;
    NVT_TIMING newTiming;
    DISPLAYID_TIMING_1_BLOCK * blk = (DISPLAYID_TIMING_1_BLOCK *)block;
    if (blk->header.data_bytes % sizeof(DISPLAYID_TIMING_1_DESCRIPTOR) != 0)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i * sizeof(DISPLAYID_TIMING_1_DESCRIPTOR) < blk->header.data_bytes; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseDisplayIdTiming1Descriptor(blk->descriptors + i,
                                            &newTiming) == NVT_STATUS_SUCCESS)
        {
            if (pEdidInfo == NULL) continue;

            if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
            {
                break;
            }
        }
        else
        {
            if (pEdidInfo == NULL) return NVT_STATUS_ERR;
        }
    }
    return NVT_STATUS_SUCCESS;
}
CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming1Descriptor(DISPLAYID_TIMING_1_DESCRIPTOR * type1, NVT_TIMING *pT)
{
    if (type1 == NULL || pT == NULL)
        return NVT_STATUS_ERR;

    // the pixel clock
    pT->pclk = (NvU32)((type1->pixel_clock_high << 16) + (type1->pixel_clock_mid << 8) + type1->pixel_clock_low_minus_0_01MHz + 1);

    // the DisplayID spec does not support border
    pT->HBorder = pT->VBorder = 0;

    // get horizontal timing parameters
    pT->HVisible    = (NvU16)((type1->horizontal.active_image_pixels_high << 8) + type1->horizontal.active_image_pixels_low_minus_1 + 1);
    pT->HTotal      = (NvU16)((type1->horizontal.blank_pixels_high        << 8) + type1->horizontal.blank_pixels_low_minus_1        + 1) + pT->HVisible;
    pT->HFrontPorch = (NvU16)((type1->horizontal.front_porch_high         << 8) + type1->horizontal.front_porch_low_minus_1         + 1);
    pT->HSyncWidth  = (NvU16)((type1->horizontal.sync_width_high          << 8) + type1->horizontal.sync_width_low_minus_1          + 1);
    pT->HSyncPol    = type1->horizontal.sync_polarity ? NVT_H_SYNC_POSITIVE : NVT_H_SYNC_NEGATIVE;

    // get vertical timings
    pT->VVisible    = (NvU16)((type1->vertical.active_image_lines_high << 8) + type1->vertical.active_image_lines_low_minus_1 + 1);
    pT->VTotal      = (NvU16)((type1->vertical.blank_lines_high        << 8) + type1->vertical.blank_lines_low_minus_1        + 1) + pT->VVisible;
    pT->VFrontPorch = (NvU16)((type1->vertical.front_porch_lines_high  << 8) + type1->vertical.front_porch_lines_low_minus_1  + 1);
    pT->VSyncWidth  = (NvU16)((type1->vertical.sync_width_lines_high   << 8) + type1->vertical.sync_width_lines_low_minus_1   + 1);
    pT->VSyncPol    = type1->vertical.sync_polarity ? NVT_V_SYNC_POSITIVE : NVT_V_SYNC_NEGATIVE;

    // EDID used in DP1.4 Compliance test had incorrect HBlank listed, leading to wrong raster sizes being set by driver (bug 2714607)
    // Filter incorrect timings here. HTotal must cover sufficient blanking time
    if (pT->HTotal < (pT->HVisible + pT->HFrontPorch + pT->HSyncWidth))
    {
        return NVT_STATUS_ERR;
    }

    // the frame scanning type
    pT->interlaced = type1->options.interface_frame_scanning_type;

    // the aspect ratio
    switch (type1->options.aspect_ratio)
    {
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_1_1:
            pT->etc.aspect = (1 << 16) | 1;
            break;
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_5_4:
            pT->etc.aspect = (5 << 16) | 4;
            break;
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_4_3:
            pT->etc.aspect = (4 << 16) | 3;
            break;
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_15_9:
            pT->etc.aspect = (15 << 16) | 9;
            break;
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_16_9:
            pT->etc.aspect = (16 << 16) | 9;
            break;
        case NVT_DISPLAYID_TIMING_ASPECT_RATIO_16_10:
            pT->etc.aspect = (16 << 16) | 10;
            break;
        default:
            pT->etc.aspect = 0;
            break;
    }

    // the refresh rate
    pT->etc.rr = NvTiming_CalcRR(pT->pclk, pT->interlaced, pT->HTotal, pT->VTotal);
    pT->etc.rrx1k = NvTiming_CalcRRx1k(pT->pclk, pT->interlaced, pT->HTotal, pT->VTotal);
    pT->etc.name[39] = '\0';
    pT->etc.rep = 0x1; // bit mask for no pixel repetition

    pT->etc.status = NVT_STATUS_DISPLAYID_1;
    // Unlike the PTM in EDID base block, DisplayID type I/II preferred timing does not have  dependency on sequence
    // so we'll just update the preferred flag, not sequence them
    //pT->etc.status = NVT_STATUS_DISPLAYID_1N(1);
    pT->etc.flag |= type1->options.is_preferred_detailed_timing ? NVT_FLAG_DISPLAYID_DTD_PREFERRED_TIMING : 0;

    /* Fields currently not used. Uncomment them for future use
    type1->options.stereo_support;
    */

    // the DisplayID spec covers the timing parameter(Visible/FrontPorch/SyncWidth/Total) range from 1~65536 while our NVT_TIMING structure which is mostly based on NvU16 only covers 0~65535
    nvt_assert(pT->HVisible != 0);
    nvt_assert(pT->HFrontPorch != 0);
    nvt_assert(pT->HSyncWidth != 0);
    nvt_assert(pT->VVisible != 0);
    nvt_assert(pT->VFrontPorch != 0);
    nvt_assert(pT->VSyncWidth != 0);

    // cover the possible overflow
    nvt_assert(pT->HTotal > pT->HVisible);
    nvt_assert(pT->VTotal > pT->VVisible);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming2(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU16 i;
    DISPLAYID_TIMING_2_BLOCK * blk = (DISPLAYID_TIMING_2_BLOCK *)block;
    NVT_TIMING newTiming;

    if (blk->header.data_bytes % sizeof(DISPLAYID_TIMING_2_DESCRIPTOR) != 0)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i * sizeof(DISPLAYID_TIMING_2_DESCRIPTOR) < blk->header.data_bytes; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseDisplayIdTiming2Descriptor(blk->descriptors + i,
                                            &newTiming) == NVT_STATUS_SUCCESS)
        {
            if (pEdidInfo == NULL) continue;

            if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
            {
                break;
            }
        }
        else
        {
            if (pEdidInfo == NULL) return NVT_STATUS_ERR;
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming2Descriptor(DISPLAYID_TIMING_2_DESCRIPTOR * type2, NVT_TIMING *pT)
{
    if (type2 == NULL || pT == NULL)
        return NVT_STATUS_ERR;

    // the pixel clock
    pT->pclk = (NvU32)((type2->pixel_clock_high << 16) + (type2->pixel_clock_mid << 8) + type2->pixel_clock_low_minus_0_01MHz + 1);

    // the DisplayID spec does not support border
    pT->HBorder = pT->VBorder = 0;

    // get horizontal timing parameters
    pT->HVisible    = (NvU16)((type2->horizontal.active_image_in_char_high << 8) + type2->horizontal.active_image_in_char_minus_1 + 1) * NVT_DISPLAYID_CHAR_WIDTH_IN_PIXELS;
    pT->HTotal      = (NvU16)(type2->horizontal.blank_in_char_minus_1 + 1) * NVT_DISPLAYID_CHAR_WIDTH_IN_PIXELS + pT->HVisible;
    pT->HFrontPorch = (NvU16)(type2->horizontal.front_porch_in_char_minus_1 + 1) * NVT_DISPLAYID_CHAR_WIDTH_IN_PIXELS;
    pT->HSyncWidth  = (NvU16)(type2->horizontal.sync_width_in_char_minus_1 + 1) * NVT_DISPLAYID_CHAR_WIDTH_IN_PIXELS;
    pT->HSyncPol    = type2->options.hsync_polarity ? NVT_H_SYNC_POSITIVE : NVT_H_SYNC_NEGATIVE;

    // get vertical timing parameters
    pT->VVisible    = (NvU16)((type2->vertical.active_image_lines_high << 8) + type2->vertical.active_image_lines_low_minus_1 + 1);
    pT->VTotal      = (NvU16)(type2->vertical.blank_lines_minus_1 + 1) + pT->VVisible;
    pT->VFrontPorch = (NvU16)(type2->vertical.front_porch_lines_minus_1 + 1);
    pT->VSyncWidth  = (NvU16)(type2->vertical.sync_width_lines_minus_1 + 1);
    pT->VSyncPol    = type2->options.vsync_polarity ? NVT_V_SYNC_POSITIVE : NVT_V_SYNC_NEGATIVE;

    // the frame scanning type
    pT->interlaced = type2->options.interface_frame_scanning_type;

    // the refresh rate
    pT->etc.rr = NvTiming_CalcRR(pT->pclk, pT->interlaced, pT->HTotal, pT->VTotal);
    pT->etc.rrx1k = NvTiming_CalcRRx1k(pT->pclk, pT->interlaced, pT->HTotal, pT->VTotal);

    pT->etc.aspect = 0;
    pT->etc.name[39] = '\0';
    pT->etc.rep = 0x1; // Bit mask for no pixel repetition

    pT->etc.status = NVT_STATUS_DISPLAYID_2;
    // Unlike the PTM in EDID base block, DisplayID type I/II preferred timing does not have dependency on sequence
    // so we'll just update the preferred flag, not sequence them
    //pT->etc.status = NVT_STATUS_DISPLAYID_1N(1);
    pT->etc.flag |= type2->options.is_preferred_detailed_timing ? NVT_FLAG_DISPLAYID_DTD_PREFERRED_TIMING : 0;

    /* Fields currently not used. Uncomment them for future use
    type1->options.stereo_support;
    */

    // the DisplayID spec covers the timing parameter(Visible/FrontPorch/SyncWidth/Total) range from 1~65536 while our NVT_TIMING structure which is mostly based on NvU16 only covers 0~65535
    nvt_assert(pT->HVisible != 0);
    nvt_assert(pT->HFrontPorch != 0);
    nvt_assert(pT->HSyncWidth != 0);
    nvt_assert(pT->VVisible != 0);
    nvt_assert(pT->VFrontPorch != 0);
    nvt_assert(pT->VSyncWidth != 0);

    // cover the possible overflow
    nvt_assert(pT->HTotal > pT->HVisible);
    nvt_assert(pT->VTotal > pT->VVisible);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming3Descriptor(DISPLAYID_TIMING_3_DESCRIPTOR * desc,
                                                  NVT_TIMING *pT)
{
    NvU8 formula, aspect;
    NvU32 horiz, vert, rr;
    NvU32 interlace;
    if (desc == NULL || pT == NULL)
        return NVT_STATUS_ERR;

    formula = DRF_VAL(T_DISPLAYID, _TIMING_3, _FORMULA, desc->optns);
    /* Fields currently not used, uncomment for use
    preferred = DRF_VAL(T_DISPLAYID, _TIMING, _PREFERRED, desc->optns);
    */
    aspect = DRF_VAL(T_DISPLAYID, _TIMING_3, _ASPECT_RATIO, desc->optns);
    interlace = DRF_VAL(T_DISPLAYID, _TIMING_3, _INTERLACE, desc->transfer) ? NVT_INTERLACED : NVT_PROGRESSIVE;
    rr = (NvU32)(DRF_VAL(T_DISPLAYID, _TIMING_3, _REFRESH_RATE, desc->transfer) + 1);

    horiz = (NvU32)((desc->horizontal_active_pixels + 1) << 3);

    switch (aspect)
    {
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_1_1:
            vert = horiz;
            break;
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_5_4:
            vert = horiz * 4 / 5;
            break;
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_4_3:
            vert = horiz * 3 / 4;
            break;
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_15_9:
            vert = horiz * 9 / 15;
            break;
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_16_9:
            vert = horiz * 9 / 16;
            break;
        case NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_16_10:
            vert = horiz * 10 / 16;
            break;
        default:
            return NVT_STATUS_ERR;
    }

    switch (formula)
    {
        case NVT_DISPLAYID_TIMING_3_FORMULA_STANDARD:
            if (NvTiming_CalcCVT(horiz, vert, rr, interlace, pT) != NVT_STATUS_SUCCESS)
                return NVT_STATUS_ERR;
            break;
        case NVT_DISPLAYID_TIMING_3_FORMULA_REDUCED_BLANKING:
            if (NvTiming_CalcCVT_RB(horiz, vert, rr, interlace, pT) != NVT_STATUS_SUCCESS)
                return NVT_STATUS_ERR;
            break;
        default:
            return NVT_STATUS_ERR;
    }

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming3(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU16 i;
    DISPLAYID_TIMING_3_BLOCK * blk = (DISPLAYID_TIMING_3_BLOCK *)block;
    NVT_TIMING newTiming;

    if (blk->header.data_bytes % sizeof(DISPLAYID_TIMING_3_DESCRIPTOR) != 0)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i * sizeof(DISPLAYID_TIMING_3_DESCRIPTOR) < blk->header.data_bytes; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseDisplayIdTiming3Descriptor(blk->descriptors + i,
                                            &newTiming) == NVT_STATUS_SUCCESS)
        {
            if (pEdidInfo == NULL) continue;

            if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
            {
                break;
            }
        }
        else
        {
            if (pEdidInfo == NULL) return NVT_STATUS_ERR;
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming4(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU16 i;
    NVT_TIMING newTiming;
    DISPLAYID_TIMING_4_BLOCK * blk = (DISPLAYID_TIMING_4_BLOCK *)block;
    if (blk->header.data_bytes < 1 || blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i < blk->header.data_bytes; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (NvTiming_EnumDMT((NvU32)(blk->timing_codes[i]),
                             &newTiming) == NVT_STATUS_SUCCESS)
        {
            if (pEdidInfo == NULL) continue;

            if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
            {
                break;
            }
        }
        else
        {
            if (pEdidInfo == NULL) return NVT_STATUS_ERR;
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming5Descriptor(DISPLAYID_TIMING_5_DESCRIPTOR * desc, NVT_TIMING *pT)
{
    NvU32 width, height, rr;
    NvBool is1000div1001 = NV_FALSE;

    // we don't handle stereo type nor custom reduced blanking yet
    //NvU8 stereoType, formula;
    //stereoType = (desc->optns & NVT_DISPLAYID_TIMING_5_STEREO_SUPPORT_MASK);
    //formula = desc->optns & NVT_DISPLAYID_TIMING_5_FORMULA_SUPPORT_MASK;

    if (desc->optns & NVT_DISPLAYID_TIMING_5_FRACTIONAL_RR_SUPPORT_MASK)
    {
        is1000div1001 = NV_TRUE;
    }
    width = ((desc->horizontal_active_pixels_high << 8) | desc->horizontal_active_pixels_low) + 1;
    height = ((desc->vertical_active_pixels_high << 8) | desc->vertical_active_pixels_low) + 1;
    rr = desc->refresh_rate + 1;
    return NvTiming_CalcCVT_RB2(width, height, rr, is1000div1001, NV_FALSE, pT);
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiming5(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU16 i;
    NVT_TIMING newTiming;
    DISPLAYID_TIMING_5_BLOCK * blk = (DISPLAYID_TIMING_5_BLOCK *)block;
    if (blk->header.data_bytes < 1 || blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }
    for (i = 0; i * sizeof(DISPLAYID_TIMING_5_DESCRIPTOR) < blk->header.data_bytes; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseDisplayIdTiming5Descriptor(blk->descriptors + i, &newTiming) == NVT_STATUS_SUCCESS)
        {
            if (pEdidInfo == NULL) continue;

            if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
            {
                 break;
            }
        }
        else
        {
            if (pEdidInfo == NULL) return NVT_STATUS_ERR;
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTimingVesa(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU8 i, j;
    NVT_TIMING newTiming;
    DISPLAYID_TIMING_MODE_BLOCK * blk = (DISPLAYID_TIMING_MODE_BLOCK *)block;
    if (blk->header.data_bytes != DISPLAYID_TIMING_VESA_BLOCK_SIZE)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i < DISPLAYID_TIMING_VESA_BLOCK_SIZE; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (blk->timing_modes[i] & (1 << j))
            {
                NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

                if (NvTiming_EnumDMT((NvU32)(i * 8 + j + 1),
                                     &newTiming) == NVT_STATUS_SUCCESS)
                {
                    if (pEdidInfo == NULL) continue;

                    if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
                    {
                        break;
                    }
                }
                else
                {
                    if (pEdidInfo == NULL) return NVT_STATUS_ERR;
                }
            }
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTimingEIA(NvU8 * block, NVT_EDID_INFO *pEdidInfo)
{
    NvU8 i, j;
    NVT_TIMING newTiming;
    DISPLAYID_TIMING_MODE_BLOCK * blk = (DISPLAYID_TIMING_MODE_BLOCK *)block;
    if (blk->header.data_bytes != DISPLAYID_TIMING_CEA_BLOCK_SIZE)
    {
        return NVT_STATUS_ERR;
    }

    for (i = 0; i < DISPLAYID_TIMING_CEA_BLOCK_SIZE; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (blk->timing_modes[i] & (1 << j))
            {
                NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

                if (NvTiming_EnumCEA861bTiming((NvU32)(i * 8 + j + 1),
                                               &newTiming) == NVT_STATUS_SUCCESS)
                {
                    if (pEdidInfo == NULL) continue;

                    if (!assignNextAvailableTiming(pEdidInfo, &newTiming))
                    {
                        break;
                    }
                }
                else
                {
                    if (pEdidInfo == NULL) return NVT_STATUS_ERR;
                }
            }
        }
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdRangeLimits(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    NVT_DISPLAYID_RANGE_LIMITS * rl;
    DISPLAYID_RANGE_LIMITS_BLOCK * blk = (DISPLAYID_RANGE_LIMITS_BLOCK *)block;
    NVT_STATUS status = NVT_STATUS_SUCCESS;
    NvU32 minPclk = 0;
    NvU32 maxPclk = 0;

    if (blk->header.data_bytes != DISPLAYID_RANGE_LIMITS_BLOCK_LEN)
    {
        return NVT_STATUS_ERR;
    }

    minPclk = blk->pixel_clock_min[0] | (blk->pixel_clock_min[1] << 8) | (blk->pixel_clock_min[2] << 16);
    maxPclk = blk->pixel_clock_max[0] | (blk->pixel_clock_max[1] << 8) | (blk->pixel_clock_max[2] << 16);

    if (blk->vertical_refresh_rate_min == 0 || blk->vertical_refresh_rate_max == 0 ||
        blk->vertical_refresh_rate_min > blk->vertical_refresh_rate_max ||
        minPclk > maxPclk)
    {
        // wrong range limit
        status = NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return status;

    if (pInfo->rl_num >= NVT_DISPLAYID_RANGE_LIMITS_MAX_COUNT)
    {
        return NVT_STATUS_ERR;
    }

    rl = pInfo->range_limits + pInfo->rl_num;
    (pInfo->rl_num)++;

    rl->pclk_min = minPclk;
    rl->pclk_max = maxPclk;

    rl->interlaced = DRF_VAL(T_DISPLAYID, _RANGE_LIMITS, _INTERLACE, blk->optns);
    rl->cvt = DRF_VAL(T_DISPLAYID, _RANGE_LIMITS, _CVT_STANDARD, blk->optns);
    rl->cvt_reduced = DRF_VAL(T_DISPLAYID, _RANGE_LIMITS, _CVT_REDUCED, blk->optns);
    rl->dfd = DRF_VAL(T_DISPLAYID, _RANGE_LIMITS, _DFD, blk->optns);

    rl->hfreq_min = blk->horizontal_frequency_min;
    rl->hfreq_max = blk->horizontal_frequency_max;
    rl->hblank_min = blk->horizontal_blanking_min;
    rl->vfreq_min = blk->vertical_refresh_rate_min;
    rl->vfreq_max = blk->vertical_refresh_rate_max;
    rl->vblank_min = blk->vertical_blanking_min;

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdSerialNumber(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_ASCII_STRING_BLOCK * blk = (DISPLAYID_ASCII_STRING_BLOCK *)block;
    if (blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    // Nothing is currently done to store any ASCII Serial Number, if it is
    // required. Code here may need to be modified sometime in the future, along
    // with NVT_DISPLAYID_INFO struct
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdAsciiString(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_ASCII_STRING_BLOCK * blk = (DISPLAYID_ASCII_STRING_BLOCK *)block;
    if (blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    // Nothing is currently done to store any ASCII String Data, if it is
    // required. Code here may need to be modified sometime in the future, along
    // with NVT_DISPLAYID_INFO struct
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdDeviceData(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_DEVICE_DATA_BLOCK * blk = (DISPLAYID_DEVICE_DATA_BLOCK *)block;
    if (blk->header.data_bytes != DISPLAYID_DEVICE_DATA_BLOCK_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    pInfo->tech_type = blk->technology;

    pInfo->device_op_mode = DRF_VAL(T_DISPLAYID, _DEVICE, _OPERATING_MODE, blk->operating_mode);
    pInfo->support_backlight = DRF_VAL(T_DISPLAYID, _DEVICE, _BACKLIGHT, blk->operating_mode);
    pInfo->support_intensity = DRF_VAL(T_DISPLAYID, _DEVICE, _INTENSITY, blk->operating_mode);

    pInfo->horiz_pixel_count = blk->horizontal_pixel_count;
    pInfo->vert_pixel_count = blk->vertical_pixel_count;

    pInfo->orientation = DRF_VAL(T_DISPLAYID, _DEVICE, _ORIENTATION, blk->orientation);
    pInfo->rotation = DRF_VAL(T_DISPLAYID, _DEVICE, _ROTATION, blk->orientation);
    pInfo->zero_pixel = DRF_VAL(T_DISPLAYID, _DEVICE, _ZERO_PIXEL, blk->orientation);
    pInfo->scan_direction = DRF_VAL(T_DISPLAYID, _DEVICE, _SCAN, blk->orientation);

    pInfo->subpixel_info = blk->subpixel_info;
    pInfo->horiz_pitch = blk->horizontal_pitch;
    pInfo->vert_pitch = blk->vertical_pitch;

    pInfo->color_bit_depth = DRF_VAL(T_DISPLAYID, _DEVICE, _COLOR_DEPTH, blk->color_bit_depth);
    pInfo->white_to_black = DRF_VAL(T_DISPLAYID, _DEVICE, _WHITE_BLACK, blk->response_time);
    pInfo->response_time = DRF_VAL(T_DISPLAYID, _DEVICE, _RESPONSE_TIME, blk->response_time);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdInterfacePower(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_INTERFACE_POWER_BLOCK * blk = (DISPLAYID_INTERFACE_POWER_BLOCK *)block;
    if (blk->header.data_bytes != DISPLAYID_INTERFACE_POWER_BLOCK_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    // Note specifically that the data inside T1/T2 variables are the exact
    // interface power data. the millisecond increments are dependent on the
    // DisplayID specification.
    pInfo->t1_min = DRF_VAL(T_DISPLAYID, _POWER, _T1_MIN, blk->power_sequence_T1);
    pInfo->t1_max = DRF_VAL(T_DISPLAYID, _POWER, _T1_MAX, blk->power_sequence_T1);
    pInfo->t2_max = DRF_VAL(T_DISPLAYID, _POWER, _T2, blk->power_sequence_T2);
    pInfo->t3_max = DRF_VAL(T_DISPLAYID, _POWER, _T3, blk->power_sequence_T3);
    pInfo->t4_min = DRF_VAL(T_DISPLAYID, _POWER, _T4_MIN, blk->power_sequence_T4_min);
    pInfo->t5_min = DRF_VAL(T_DISPLAYID, _POWER, _T5_MIN, blk->power_sequence_T5_min);
    pInfo->t6_min = DRF_VAL(T_DISPLAYID, _POWER, _T6_MIN, blk->power_sequence_T6_min);

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTransferChar(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    // Transfer Characteristics are currently not supported, but parsing of the
    // block should be added in the future when more specifications on monitors
    // that require this information is located here.
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdDisplayInterface(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_INTERFACE_DATA_BLOCK * blk = (DISPLAYID_INTERFACE_DATA_BLOCK *)block;
    if (blk->header.data_bytes != DISPLAYID_INTERFACE_DATA_BLOCK_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    pInfo->supported_displayId2_0 = 0;

    // Type/Link Info
    pInfo->u4.display_interface.interface_type = DRF_VAL(T_DISPLAYID, _INTERFACE, _TYPE, blk->info);
    pInfo->u4.display_interface.u1.digital_num_links = DRF_VAL(T_DISPLAYID, _INTERFACE, _NUMLINKS, blk->info);
    pInfo->u4.display_interface.interface_version = blk->version;

    // Color Depths
    pInfo->u4.display_interface.rgb_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB16, blk->color_depth_rgb);
    pInfo->u4.display_interface.rgb_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB14, blk->color_depth_rgb);
    pInfo->u4.display_interface.rgb_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB12, blk->color_depth_rgb);
    pInfo->u4.display_interface.rgb_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB10, blk->color_depth_rgb);
    pInfo->u4.display_interface.rgb_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB8, blk->color_depth_rgb);
    pInfo->u4.display_interface.rgb_depth.support_6b = DRF_VAL(T_DISPLAYID, _INTERFACE, _RGB6, blk->color_depth_rgb);
    pInfo->u4.display_interface.ycbcr444_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_16, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr444_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_14, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr444_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_12, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr444_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_10, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr444_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_8, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr444_depth.support_6b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR444_6, blk->color_depth_ycbcr444);
    pInfo->u4.display_interface.ycbcr422_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR422_16, blk->color_depth_ycbcr422);
    pInfo->u4.display_interface.ycbcr422_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR422_14, blk->color_depth_ycbcr422);
    pInfo->u4.display_interface.ycbcr422_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR422_12, blk->color_depth_ycbcr422);
    pInfo->u4.display_interface.ycbcr422_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR422_10, blk->color_depth_ycbcr422);
    pInfo->u4.display_interface.ycbcr422_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE, _YCBCR422_8, blk->color_depth_ycbcr422);

    // Content Protection
    pInfo->u4.display_interface.content_protection = DRF_VAL(T_DISPLAYID, _INTERFACE, _CONTENT, blk->content_protection);
    pInfo->u4.display_interface.content_protection_version = blk->content_protection_version;

    // Spread
    pInfo->u4.display_interface.spread_spectrum = DRF_VAL(T_DISPLAYID, _INTERFACE, _SPREAD_TYPE, blk->spread);
    pInfo->u4.display_interface.spread_percent = DRF_VAL(T_DISPLAYID, _INTERFACE, _SPREAD_PER, blk->spread);

    // Proprietary Information
    switch (pInfo->u4.display_interface.interface_type)
    {
        case NVT_DISPLAYID_INTERFACE_TYPE_LVDS:
            pInfo->u2.lvds.color_map = DRF_VAL(T_DISPLAYID, _LVDS, _COLOR, blk->interface_attribute_1);
            pInfo->u2.lvds.support_2_8v = DRF_VAL(T_DISPLAYID, _LVDS, _2_8, blk->interface_attribute_1);
            pInfo->u2.lvds.support_12v = DRF_VAL(T_DISPLAYID, _LVDS, _12, blk->interface_attribute_1);
            pInfo->u2.lvds.support_5v = DRF_VAL(T_DISPLAYID, _LVDS, _5, blk->interface_attribute_1);
            pInfo->u2.lvds.support_3_3v = DRF_VAL(T_DISPLAYID, _LVDS, _3_3, blk->interface_attribute_1);
            pInfo->u2.lvds.DE_mode = DRF_VAL(T_DISPLAYID, _INTERFACE, _DE, blk->interface_attribute_2);
            pInfo->u2.lvds.polarity = DRF_VAL(T_DISPLAYID, _INTERFACE, _POLARITY, blk->interface_attribute_2);
            pInfo->u2.lvds.data_strobe = DRF_VAL(T_DISPLAYID, _INTERFACE, _STROBE, blk->interface_attribute_2);
            break;
        case NVT_DISPLAYID_INTERFACE_TYPE_PROPRIETARY:
            pInfo->u2.proprietary.DE_mode = DRF_VAL(T_DISPLAYID, _INTERFACE, _DE, blk->interface_attribute_1);
            pInfo->u2.proprietary.polarity = DRF_VAL(T_DISPLAYID, _INTERFACE, _POLARITY, blk->interface_attribute_1);
            pInfo->u2.proprietary.data_strobe = DRF_VAL(T_DISPLAYID, _INTERFACE, _STROBE, blk->interface_attribute_1);
            break;
        default:
            break;
    }
    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdStereo(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    NvU8 * sub;

    DISPLAYID_STEREO_INTERFACE_METHOD_BLOCK * blk = (DISPLAYID_STEREO_INTERFACE_METHOD_BLOCK *)block;
    if (blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    sub = blk->timing_sub_block;

    pInfo->stereo_code = blk->stereo_code;
    switch (blk->stereo_code)
    {
        case NVT_DISPLAYID_STEREO_FIELD_SEQUENTIAL:
            pInfo->u3.field_sequential.stereo_polarity = sub[0];
            break;
        case NVT_DISPLAYID_STEREO_SIDE_BY_SIDE:
            pInfo->u3.side_by_side.view_identity = sub[0];
            break;
        case NVT_DISPLAYID_STEREO_PIXEL_INTERLEAVED:
            NVMISC_MEMCPY(pInfo->u3.pixel_interleaved.interleave_pattern, sub, 8);
            break;
        case NVT_DISPLAYID_STEREO_DUAL_INTERFACE:
            pInfo->u3.left_right_separate.mirroring = DRF_VAL(T_DISPLAYID, _STEREO, _MIRRORING, sub[0]);
            pInfo->u3.left_right_separate.polarity = DRF_VAL(T_DISPLAYID, _STEREO, _POLARITY, sub[0]);
            break;
        case NVT_DISPLAYID_STEREO_MULTIVIEW:
            pInfo->u3.multiview.num_views = sub[0];
            pInfo->u3.multiview.code = sub[1];
            break;
        case NVT_DISPLAYID_STEREO_PROPRIETARY:
            break;
        default:
            return NVT_STATUS_ERR;
    }

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdTiledDisplay(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    DISPLAYID_TILED_DISPLAY_BLOCK * blk = (DISPLAYID_TILED_DISPLAY_BLOCK *)block;
    if (blk->header.data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    // For revision 0, we only allow one tiled display data block.
    if (!blk->header.revision && pInfo->tile_topology_id.vendor_id)
        return NVT_STATUS_SUCCESS;

    pInfo->tiled_display_revision           = blk->header.revision;

    pInfo->tile_capability.bSingleEnclosure     = blk->capability.single_enclosure;
    pInfo->tile_capability.bHasBezelInfo        = blk->capability.has_bezel_info;
    pInfo->tile_capability.multi_tile_behavior  = blk->capability.multi_tile_behavior;
    pInfo->tile_capability.single_tile_behavior = blk->capability.single_tile_behavior;

    pInfo->tile_topology.row                = ((blk->topo_loc_high.row << 5) | blk->topology_low.row) + 1;
    pInfo->tile_topology.col                = ((blk->topo_loc_high.col << 5) | blk->topology_low.col) + 1;

    pInfo->tile_location.x                  = (blk->topo_loc_high.x << 5) | blk->location_low.x;
    pInfo->tile_location.y                  = (blk->topo_loc_high.y << 5) | blk->location_low.y;

    pInfo->native_resolution.width          = ((blk->native_resolution.width_high<<8)|blk->native_resolution.width_low) + 1;
    pInfo->native_resolution.height         = ((blk->native_resolution.height_high<<8)|blk->native_resolution.height_low) + 1;

    pInfo->bezel_info.pixel_density         = blk->bezel_info.pixel_density;
    pInfo->bezel_info.top                   = (blk->bezel_info.top * blk->bezel_info.pixel_density) / 10;
    pInfo->bezel_info.bottom                = (blk->bezel_info.bottom * blk->bezel_info.pixel_density) / 10;
    pInfo->bezel_info.right                 = (blk->bezel_info.right * blk->bezel_info.pixel_density) / 10;
    pInfo->bezel_info.left                  = (blk->bezel_info.left * blk->bezel_info.pixel_density) / 10;

    pInfo->tile_topology_id.vendor_id       = (blk->topology_id.vendor_id[2] << 16) |
                                              (blk->topology_id.vendor_id[1] << 8 ) |
                                              blk->topology_id.vendor_id[0];

    pInfo->tile_topology_id.product_id      = (blk->topology_id.product_id[1] << 8) | blk->topology_id.product_id[0];

    pInfo->tile_topology_id.serial_number   = (blk->topology_id.serial_number[3] << 24) |
                                              (blk->topology_id.serial_number[2] << 16) |
                                              (blk->topology_id.serial_number[1] << 8 ) |
                                              blk->topology_id.serial_number[0];

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdCtaData(NvU8 * block, NVT_EDID_INFO *pInfo)
{
    DISPLAYID_DATA_BLOCK_HEADER * blk = (DISPLAYID_DATA_BLOCK_HEADER*)block;
    NVT_EDID_CEA861_INFO *p861info;
    if (blk->data_bytes > NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN)
    {
        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    p861info = &pInfo->ext861;

    pInfo->ext_displayid.cea_data_block_present = 1;
    p861info->revision = blk->revision;

    //parse CEA tags which starts at 3rd byte from block
    parseCta861DataBlockInfo(&block[3], blk->data_bytes, p861info);

    // update pInfo with basic hdmi info
    // assumes each edid will only have one such block across multiple cta861 blocks (otherwise may create declaration conflict)
    // in case of multiple such blocks, the last one takes precedence
    parseCta861VsdbBlocks(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);

    parseCta861HfScdb(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);

    //parse HDR related information from the HDR static metadata data block
    if (p861info->valid.hdr_static_metadata != 0)
    {
        parseCta861HdrStaticMetadataDataBlock(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);
    }

    // base video
    parse861bShortTiming(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);
    // yuv420-only video
    parse861bShortYuv420Timing(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);
    // CEA861-F at 7.5.12 section about VFPDB block.
    if (p861info->total_svr != 0)
    {
        parseCta861NativeOrPreferredTiming(p861info, pInfo, FROM_DISPLAYID_13_DATA_BLOCK);
    }

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS parseDisplayIdDisplayInterfaceFeatures(NvU8 * block, NVT_DISPLAYID_INFO *pInfo)
{
    NvU8 i;
    DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK * blk = (DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK *)block;
    if (blk->header.data_bytes > DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK_MAX_LEN)
    {

        return NVT_STATUS_ERR;
    }

    if (pInfo == NULL) return NVT_STATUS_SUCCESS;

    pInfo->supported_displayId2_0 = 1;

    // Color Depths
    pInfo->u4.display_interface_features.rgb_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB16, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.rgb_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB14, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.rgb_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB12, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.rgb_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB10, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.rgb_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB8, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.rgb_depth.support_6b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _RGB6, blk->supported_color_depth_rgb);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_16, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_14, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_12, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_10, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_8, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr444_depth.support_6b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR444_6, blk->supported_color_depth_ycbcr444);
    pInfo->u4.display_interface_features.ycbcr422_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR422_16, blk->supported_color_depth_ycbcr422);
    pInfo->u4.display_interface_features.ycbcr422_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR422_14, blk->supported_color_depth_ycbcr422);
    pInfo->u4.display_interface_features.ycbcr422_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR422_12, blk->supported_color_depth_ycbcr422);
    pInfo->u4.display_interface_features.ycbcr422_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR422_10, blk->supported_color_depth_ycbcr422);
    pInfo->u4.display_interface_features.ycbcr422_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR422_8, blk->supported_color_depth_ycbcr422);
    pInfo->u4.display_interface_features.ycbcr420_depth.support_16b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR420_16, blk->supported_color_depth_ycbcr420);
    pInfo->u4.display_interface_features.ycbcr420_depth.support_14b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR420_14, blk->supported_color_depth_ycbcr420);
    pInfo->u4.display_interface_features.ycbcr420_depth.support_12b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR420_12, blk->supported_color_depth_ycbcr420);
    pInfo->u4.display_interface_features.ycbcr420_depth.support_10b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR420_10, blk->supported_color_depth_ycbcr420);
    pInfo->u4.display_interface_features.ycbcr420_depth.support_8b = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _YCBCR420_8, blk->supported_color_depth_ycbcr420);

    // Minimum Pixel Rate at Which YCbCr 4:2:0 Encoding Is Supported
    pInfo->u4.display_interface_features.minimum_pixel_rate_ycbcr420 = blk->minimum_pixel_rate_ycbcr420;

    // Audio capability
    pInfo->u4.display_interface_features.audio_capability.support_32khz = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _AUDIO_SUPPORTED_32KHZ, blk->supported_audio_capability);
    pInfo->u4.display_interface_features.audio_capability.support_44_1khz = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _AUDIO_SUPPORTED_44_1KHZ, blk->supported_audio_capability);
    pInfo->u4.display_interface_features.audio_capability.support_48khz = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _AUDIO_SUPPORTED_48KHZ, blk->supported_audio_capability);

    // Colorspace and EOTF combination
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_bt2020_eotf_smpte_st2084 = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_BT2020_EOTF_SMPTE_ST2084, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_bt2020_eotf_bt2020 = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_BT2020_EOTF_BT2020, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_dci_p3_eotf_dci_p3 = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_DCI_P3_EOTF_DCI_P3, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_adobe_rgb_eotf_adobe_rgb = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_ADOBE_RGB_EOTF_ADOBE_RGB, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_bt709_eotf_bt1886 = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_BT709_EOTF_BT1886, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_bt601_eotf_bt601 = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_BT601_EOTF_BT601, blk->supported_colorspace_eotf_combination_1);
    pInfo->u4.display_interface_features.colorspace_eotf_combination_1.support_colorspace_srgb_eotf_srgb = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _COLORSPACE_SRGB_EOTF_SRGB, blk->supported_colorspace_eotf_combination_1);

    // Additional support Colorspace and EOTF
    pInfo->u4.display_interface_features.total_additional_colorspace_eotf.total = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _ADDITIONAL_SUPPORTED_COLORSPACE_EOTF_TOTAL, blk->additional_supported_colorspace_eotf_total);

    for (i = 0; i < pInfo->u4.display_interface_features.total_additional_colorspace_eotf.total; i++)
    {
        pInfo->u4.display_interface_features.additional_colorspace_eotf[i].support_colorspace = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _ADDITIONAL_SUPPORTED_COLORSPACE, blk->additional_supported_colorspace_eotf[i]);
        pInfo->u4.display_interface_features.additional_colorspace_eotf[i].support_eotf = DRF_VAL(T_DISPLAYID, _INTERFACE_FEATURES, _ADDITIONAL_SUPPORTED_EOTF, blk->additional_supported_colorspace_eotf[i]);

    }
    return NVT_STATUS_SUCCESS;
}

POP_SEGMENTS
