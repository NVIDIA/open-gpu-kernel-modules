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
//  File:       nvt_edidext_displayid20.c
//
//  Purpose:    the provide displayID 2.0 related services
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "edid.h"

PUSH_SEGMENTS

/**
 *
 * @brief Parses a displayId20 EDID Extension block, with timings stored in p and
 *        other info stored in pInfo
 * @param p The EDID Extension Block (With a DisplayID in it)
 * @param size Size of the displayId Extension Block
 * @param pEdidInfo EDID struct containing DisplayID information and
 *                  the timings
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS
getDisplayId20EDIDExtInfo(
    NvU8 *p,
    NvU32 size,
    NVT_EDID_INFO *pEdidInfo)
{
    DISPLAYID_2_0_SECTION *extSection = NULL;

    if (p == NULL                            ||
        size < sizeof(EDIDV1STRUC)           ||
        size > sizeof(EDIDV1STRUC)           ||
        p[0] != NVT_EDID_EXTENSION_DISPLAYID ||
        pEdidInfo == NULL)
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    // Calculate the All DisplayID20 Extension checksum
    // The function name
    if (computeDisplayId20SectionCheckSum(p, sizeof(EDIDV1STRUC)) != 0)
    {
#ifdef DD_UNITTEST
         return NVT_STATUS_ERR;
#endif
    }

    extSection = (DISPLAYID_2_0_SECTION *)(p + 1);

    return parseDisplayId20EDIDExtSection(extSection, pEdidInfo);
}

/*
 *  @brief DisplayId20 as EDID extension block's "Section" entry point functions
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS
parseDisplayId20EDIDExtSection(
    DISPLAYID_2_0_SECTION * extSection,
    NVT_EDID_INFO *pEdidInfo)
{
    NvU8 datablock_location = 0;
    NvU8 datablock_length;
    NvU8 remaining_length;

    if ((extSection == NULL) ||
        (extSection->header.section_bytes != 121))
    {
        return NVT_STATUS_ERR;
    }

    // It is based on the DisplayID v2.0 Errata E7
    // First DisplayID2.0 section as EDID extension shall populate "Display Product Primary Use Case" byte with a value from 1h-8h based on the intended primary use case of the sink.
    // Any subsequent DisplayID2.0 section EDID extension shall set the "Display Product Primary Use Case" byte to 0h.
    pEdidInfo->total_did2_extensions++;

    if (extSection->header.version == DISPLAYID_2_0_VERSION)
    {
        if (((pEdidInfo->total_did2_extensions == 1) && (extSection->header.product_type == DISPLAYID_2_0_PROD_EXTENSION ||
                                                         extSection->header.product_type > DISPLAYID_2_0_PROD_HMD_AR  ||
                                                         extSection->header.extension_count != DISPLAYID_2_0_PROD_EXTENSION)) ||
            (pEdidInfo->total_did2_extensions > 1 && extSection->header.product_type != DISPLAYID_2_0_PROD_EXTENSION))
        {
#ifdef DD_UNITTEST
            return NVT_STATUS_ERR;
#endif
        }

        pEdidInfo->ext_displayid20.version = extSection->header.version;
        pEdidInfo->ext_displayid20.revision = extSection->header.revision;
        if (pEdidInfo->total_did2_extensions == 1)
        {
            pEdidInfo->ext_displayid20.primary_use_case = extSection->header.product_type;
        }
        pEdidInfo->ext_displayid20.as_edid_extension = NV_TRUE;
    }
    else
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    // validate for section checksum before processing the data block
    if (computeDisplayId20SectionCheckSum((const NvU8*)extSection, DISPLAYID_2_0_SECTION_SIZE_TOTAL(extSection->header)) != 0)
    {
        return NVT_STATUS_ERR;
    }

    remaining_length = extSection->header.section_bytes;

    while (datablock_location < extSection->header.section_bytes)
    {
        DISPLAYID_2_0_DATA_BLOCK_HEADER * dbHeader = (DISPLAYID_2_0_DATA_BLOCK_HEADER *) (extSection->data + datablock_location);
        NvU8 is_reserve = remaining_length > 3 && datablock_location == 0 && dbHeader->type == 0 && dbHeader->data_bytes > 0;
        NvU8 i;

        // Check the padding.
        if (dbHeader->type == 0 && !is_reserve)
        {
            for (i = 1 ; i < remaining_length; i++)
            {
                // All remaining bytes must all be 0.
                if (extSection->data[datablock_location + i] != 0)
                {
                    return NVT_STATUS_ERR;
                }
            }

            datablock_length = remaining_length;
        }
        else
        {
            if (parseDisplayId20EDIDExtDataBlocks((NvU8 *)(extSection->data + datablock_location),
                                                  extSection->header.section_bytes - datablock_location,
                                                  &datablock_length,
                                                  pEdidInfo) != NVT_STATUS_SUCCESS)
                return NVT_STATUS_ERR;
        }

        datablock_location += datablock_length;
        remaining_length -= datablock_length;
    }

    return NVT_STATUS_SUCCESS;
}

/*
 *  @brief DisplayId20 as EDID extension block's "Data Block" entry point functions
 *         For validation, passed the NULL pEdidInfo, and client will check the return value
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS
parseDisplayId20EDIDExtDataBlocks(
    NvU8 *pDataBlock,
    NvU8 RemainSectionLength,
    NvU8 *pCurrentDBLength,
    NVT_EDID_INFO *pEdidInfo)
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER * block_header = (DISPLAYID_2_0_DATA_BLOCK_HEADER *) pDataBlock;
    NVT_STATUS              status = NVT_STATUS_SUCCESS;
    NVT_DISPLAYID_2_0_INFO *pDisplayId20Info = NULL;

    NvU8 i;

    // size sanity checking
    if ((pDataBlock == NULL || RemainSectionLength <= NVT_DISPLAYID_DATABLOCK_HEADER_LEN) ||
        (block_header->data_bytes > RemainSectionLength - NVT_DISPLAYID_DATABLOCK_HEADER_LEN))
        return NVT_STATUS_ERR;

    if (block_header->type < DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY)
    {
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pEdidInfo != NULL)
    {
        pDisplayId20Info = &pEdidInfo->ext_displayid20;
    }

    *pCurrentDBLength = block_header->data_bytes + NVT_DISPLAYID_DATABLOCK_HEADER_LEN;

    status = parseDisplayId20DataBlock(block_header, pDisplayId20Info);

    if (pDisplayId20Info == NULL) return status;

    // TODO : All the data blocks shall sync the data from the datablock in DisplayID2_0 to pEdidInfo
    if (status == NVT_STATUS_SUCCESS && pDisplayId20Info->as_edid_extension == NV_TRUE)
    {
        switch (block_header->type)
        {
        case DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY:
            pDisplayId20Info->valid_data_blocks.product_id_present = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_INTERFACE_FEATURES:
            pDisplayId20Info->valid_data_blocks.interface_feature_present = NV_TRUE;

            // Supported - Color depth is supported for all supported timings.  Supported timing includes all Display-ID exposed timings
            // (that is timing exposed using DisplayID timing types and CTA VICs)
            if (IS_BPC_SUPPORTED_COLORFORMAT(pDisplayId20Info->interface_features.yuv444.bpcs))
            {
                pDisplayId20Info->basic_caps |= NVT_DISPLAY_2_0_CAP_YCbCr_444;
            }

            if (IS_BPC_SUPPORTED_COLORFORMAT(pDisplayId20Info->interface_features.yuv422.bpcs))
            {
                pDisplayId20Info->basic_caps |= NVT_DISPLAY_2_0_CAP_YCbCr_422;
            }

            if (pDisplayId20Info->interface_features.audio_capability.support_48khz   ||
                pDisplayId20Info->interface_features.audio_capability.support_44_1khz ||
                pDisplayId20Info->interface_features.audio_capability.support_32khz)
            {
                pDisplayId20Info->basic_caps |= NVT_DISPLAY_2_0_CAP_BASIC_AUDIO;
            }

            for (i = 0; i < pDisplayId20Info->interface_features.combination_count; i++)
            {
                if (pDisplayId20Info->interface_features.colorspace_eotf_combination[i].eotf == INTERFACE_EOTF_SMPTE_ST2084 &&
                    pDisplayId20Info->interface_features.colorspace_eotf_combination[i].color_space == INTERFACE_COLOR_SPACE_BT2020)
                {
                    pEdidInfo->hdr_static_metadata_info.static_metadata_type = 1;
                    pEdidInfo->hdr_static_metadata_info.supported_eotf.smpte_st_2084_eotf = 1;

                    pEdidInfo->ext861.hdr_static_metadata.byte1 |= NVT_CEA861_EOTF_SMPTE_ST2084;
                    pEdidInfo->ext861.colorimetry.byte1 |= NVT_CEA861_COLORIMETRY_BT2020RGB;

                    if (IS_BPC_SUPPORTED_COLORFORMAT(pDisplayId20Info->interface_features.yuv444.bpcs) ||
                        IS_BPC_SUPPORTED_COLORFORMAT(pDisplayId20Info->interface_features.yuv422.bpcs))
                    {
                        pEdidInfo->ext861.colorimetry.byte1 |= NVT_CEA861_COLORIMETRY_BT2020YCC;
                    }
                }
            }
        break;

        // DisplayID_v2.0 E5 defined
        // if inside CTA embedded block existed 420 VDB/CMDB, then we follow these two blocks only.
        // * support for 420 pixel encoding is limited to the timings exposed in the restricted set exposed in the CTA data block.
        // * field of "Mini Pixel Rate at YCbCr420" shall be set 00h
        case DISPLAYID_2_0_BLOCK_TYPE_CTA_DATA:
            pDisplayId20Info->valid_data_blocks.cta_data_present                   = NV_TRUE;

            // copy all the vendor specific data block from DisplayId20 to pEdidInfo
            // NOTE: mixed CTA extension block and DID2.0 extension block are not handled
            if (pEdidInfo->ext861.valid.H14B_VSDB == 0 && pEdidInfo->ext861_2.valid.H14B_VSDB == 0 && pDisplayId20Info->cta.cta861_info.valid.H14B_VSDB)
                NVMISC_MEMCPY(&pEdidInfo->hdmiLlcInfo,              &pDisplayId20Info->vendor_specific.hdmiLlc,  sizeof(NVT_HDMI_LLC_INFO));
            if (pEdidInfo->ext861.valid.H20_HF_VSDB == 0 && pEdidInfo->ext861_2.valid.H20_HF_VSDB == 0 && pDisplayId20Info->cta.cta861_info.valid.H20_HF_VSDB)
                NVMISC_MEMCPY(&pEdidInfo->hdmiForumInfo,            &pDisplayId20Info->vendor_specific.hfvs,     sizeof(NVT_HDMI_FORUM_INFO));
            if (pEdidInfo->ext861.valid.nvda_vsdb == 0 && pEdidInfo->ext861_2.valid.nvda_vsdb == 0 && pDisplayId20Info->cta.cta861_info.valid.nvda_vsdb)
                NVMISC_MEMCPY(&pEdidInfo->nvdaVsdbInfo,             &pDisplayId20Info->vendor_specific.nvVsdb,   sizeof(NVDA_VSDB_PARSED_INFO));
            if (pEdidInfo->ext861.valid.msft_vsdb == 0 && pEdidInfo->ext861_2.valid.msft_vsdb == 0 && pDisplayId20Info->cta.cta861_info.valid.msft_vsdb)
                NVMISC_MEMCPY(&pEdidInfo->msftVsdbInfo,             &pDisplayId20Info->vendor_specific.msftVsdb, sizeof(MSFT_VSDB_PARSED_INFO));
            if (pEdidInfo->ext861.valid.hdr_static_metadata == 0 && pEdidInfo->ext861_2.valid.hdr_static_metadata == 0 && pDisplayId20Info->cta.cta861_info.valid.hdr_static_metadata)
                NVMISC_MEMCPY(&pEdidInfo->hdr_static_metadata_info, &pDisplayId20Info->cta.hdrInfo,              sizeof(NVT_HDR_STATIC_METADATA));
            if (pEdidInfo->ext861.valid.dv_static_metadata == 0 && pEdidInfo->ext861_2.valid.dv_static_metadata == 0 && pDisplayId20Info->cta.cta861_info.valid.dv_static_metadata)
                NVMISC_MEMCPY(&pEdidInfo->dv_static_metadata_info,  &pDisplayId20Info->cta.dvInfo,               sizeof(NVT_DV_STATIC_METADATA));
            if (pEdidInfo->ext861.valid.hdr10Plus == 0 && pEdidInfo->ext861_2.valid.hdr10Plus == 0 && pDisplayId20Info->cta.cta861_info.valid.hdr10Plus)
                NVMISC_MEMCPY(&pEdidInfo->hdr10PlusInfo,            &pDisplayId20Info->cta.hdr10PlusInfo,        sizeof(NVT_HDR10PLUS_INFO));

            // If the CTA861 extension existed already, we need to synced the revision/basic_caps to CTA which is embedded in DID20
            if (pEdidInfo->ext861.revision >= NVT_CEA861_REV_B)
            {
                pDisplayId20Info->cta.cta861_info.revision   = pEdidInfo->ext861.revision;
                pDisplayId20Info->cta.cta861_info.basic_caps = pEdidInfo->ext861.basic_caps;
                pDisplayId20Info->basic_caps                 = pEdidInfo->ext861.basic_caps;
            }

            // this is the DisplayID20 Extension, just copy needed data block value here:
            if (pEdidInfo->ext861.revision == 0)
            {
                if (pDisplayId20Info->cta.cta861_info.valid.colorimetry)
                {
                    pEdidInfo->ext861.colorimetry.byte1 = pDisplayId20Info->cta.cta861_info.colorimetry.byte1;
                    pEdidInfo->ext861.colorimetry.byte2 = pDisplayId20Info->cta.cta861_info.colorimetry.byte2;
                }
            }
            else if (pEdidInfo->ext861_2.revision == 0)
            {
                if (pDisplayId20Info->cta.cta861_info.valid.colorimetry)
                {
                    pEdidInfo->ext861_2.colorimetry.byte1 = pDisplayId20Info->cta.cta861_info.colorimetry.byte1;
                    pEdidInfo->ext861_2.colorimetry.byte2 = pDisplayId20Info->cta.cta861_info.colorimetry.byte2;
                }
            }
        break;

        case DISPLAYID_2_0_BLOCK_TYPE_DISPLAY_PARAM:
            pDisplayId20Info->valid_data_blocks.parameters_present                 = NV_TRUE;

            // EDID only supported 10bits chromaticity to match the OS D3DKMDT_2DOFFSET 10bits, so we don't need to transfer it here.

            pEdidInfo->input.u.digital.bpc = NVT_COLORDEPTH_HIGHEST_BPC(pDisplayId20Info->display_param.native_color_depth);
            pEdidInfo->gamma = pDisplayId20Info->display_param.gamma_x100;

            if (pDisplayId20Info->display_param.audio_speakers_integrated == AUDIO_SPEAKER_INTEGRATED_SUPPORTED)
            {
                pDisplayId20Info->basic_caps |= NVT_DISPLAY_2_0_CAP_BASIC_AUDIO;
            }

            if (pDisplayId20Info->display_param.gamma_x100 != 0)
            {
                pEdidInfo->hdr_static_metadata_info.supported_eotf.trad_gamma_sdr_eotf = 1;
            }
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_STEREO:
            pDisplayId20Info->valid_data_blocks.stereo_interface_present           = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_TILED_DISPLAY:
            pDisplayId20Info->valid_data_blocks.tiled_display_present              = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_CONTAINER_ID:
            pDisplayId20Info->valid_data_blocks.container_id_present               = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_TIMING_7:
            pDisplayId20Info->valid_data_blocks.type7Timing_present                = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_TIMING_8:
            pDisplayId20Info->valid_data_blocks.type8Timing_present                = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_TIMING_9:
            pDisplayId20Info->valid_data_blocks.type9Timing_present                = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_TIMING_10:
            pDisplayId20Info->valid_data_blocks.type10Timing_present               = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_RANGE_LIMITS:
            pDisplayId20Info->valid_data_blocks.dynamic_range_limit_present        = NV_TRUE;
            break;
        case DISPLAYID_2_0_BLOCK_TYPE_ADAPTIVE_SYNC:
            pDisplayId20Info->valid_data_blocks.adaptive_sync_present              = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_BRIGHTNESS_LUMINANCE_RANGE:
            pDisplayId20Info->valid_data_blocks.brightness_luminance_range_present = NV_TRUE;
        break;
        case DISPLAYID_2_0_BLOCK_TYPE_VENDOR_SPEC:
            pDisplayId20Info->valid_data_blocks.vendor_specific_present            = NV_TRUE;
        break;
        default:
            break;
        }
    }

    return status;
}

/*  @brief Update the correct color format / attribute of timings from interface feature data block
 */
CODE_SEGMENT(PAGE_DD_CODE)
void
updateColorFormatForDisplayId20ExtnTimings(
    NVT_EDID_INFO *pInfo,
    NvU32 timingIdx)
{
    // pDisplayId20Info parsed displayID20 info
    NVT_DISPLAYID_2_0_INFO *pDisplayId20Info = &pInfo->ext_displayid20;
    NVT_TIMING *pT= &pInfo->timing[timingIdx];

    nvt_assert(timingIdx <= COUNT(pInfo->timing));

    if (pDisplayId20Info->as_edid_extension)
    {
        if ((pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_A_SUPPORTED ||
             pInfo->input.u.digital.video_interface == NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_B_SUPPORTED ||
             pInfo->ext861.valid.H14B_VSDB || pInfo->ext861.valid.H20_HF_VSDB) && pInfo->ext861.revision >= NVT_CEA861_REV_A)
        {
            UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 0,
                                                        1,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc10,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc12,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc14,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc16);
        }
        else
        {
            // rgb444 (always support 6bpc and 8bpc as per DP spec 5.1.1.1.1 RGB Colorimetry)
            UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 1,
                                                        1,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc10,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc12,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc14,
                                                        pDisplayId20Info->interface_features.rgb444.bpc.bpc16);
        }

        // yuv444
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv444, 0, /* yuv444 does not support 6bpc */
                                                    pDisplayId20Info->interface_features.yuv444.bpc.bpc8,
                                                    pDisplayId20Info->interface_features.yuv444.bpc.bpc10,
                                                    pDisplayId20Info->interface_features.yuv444.bpc.bpc12,
                                                    pDisplayId20Info->interface_features.yuv444.bpc.bpc14,
                                                    pDisplayId20Info->interface_features.yuv444.bpc.bpc16);
        // yuv422
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv422, 0, /* yuv422 does not support 6bpc */
                                                    pDisplayId20Info->interface_features.yuv422.bpc.bpc8,
                                                    pDisplayId20Info->interface_features.yuv422.bpc.bpc10,
                                                    pDisplayId20Info->interface_features.yuv422.bpc.bpc12,
                                                    pDisplayId20Info->interface_features.yuv422.bpc.bpc14,
                                                    pDisplayId20Info->interface_features.yuv422.bpc.bpc16);

        if (!NVT_DID20_TIMING_IS_CTA861(pInfo->timing[timingIdx].etc.flag, pInfo->timing[timingIdx].etc.status))
        {
            // yuv420
            UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv420, 0, /* yuv420 does not support 6bpc */
                                                        pDisplayId20Info->interface_features.yuv420.bpc.bpc8,
                                                        pDisplayId20Info->interface_features.yuv420.bpc.bpc10,
                                                        pDisplayId20Info->interface_features.yuv420.bpc.bpc12,
                                                        pDisplayId20Info->interface_features.yuv420.bpc.bpc14,
                                                        pDisplayId20Info->interface_features.yuv420.bpc.bpc16);
        }
    }
}

POP_SEGMENTS
