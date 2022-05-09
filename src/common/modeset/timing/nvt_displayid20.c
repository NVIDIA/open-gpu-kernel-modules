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
//  File:       nvt_displayid20.c
//
//  Purpose:    the provide displayID 2.0 related services
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "nvtiming.h"
#include "nvtiming_pvt.h"
#include "displayid20.h"

PUSH_SEGMENTS

// DisplayID20 Entry point functions
static NVT_STATUS parseDisplayId20BaseSection(const DISPLAYID_2_0_SECTION *pSection, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);
static NVT_STATUS parseDisplayId20SectionDataBlocks(const DISPLAYID_2_0_SECTION *pSection, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);
static NVT_STATUS parseDisplayId20ExtensionSection(const DISPLAYID_2_0_SECTION *pSection, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);

// DisplayID20 Data Block Tag Alloction
static NVT_STATUS parseDisplayId20ProductIdentity(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_PRODUCT_IDENTITY *pProductIdentity);               // 0x20 Product Identificaton                                                                                                                    Block Tag
static NVT_STATUS parseDisplayId20DisplayParam(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_DISPLAY_PARAMETERS *pDisplayParam);                   // 0x21 Display Parameters
static NVT_STATUS parseDisplayId20Timing7(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                 // 0x22 Type VII Timing - Detailed Timing
static NVT_STATUS parseDisplayId20Timing8(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                 // 0x23 Type VIII Timing - Enumerated Timing
static NVT_STATUS parseDisplayId20Timing9(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                 // 0x24 Type IX Timing - Formula-based
static NVT_STATUS parseDisplayId20Timing10(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                // 0x24 Type X Timing - Formula-based RR up to 1024Hz
static NVT_STATUS parseDisplayId20RangeLimit(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_RANGE_LIMITS *pRangeLimits);                            // 0x25 Dynamic Video Timing Range Limits
static NVT_STATUS parseDisplayId20DisplayInterfaceFeatures(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_INTERFACE_FEATURES *pInterfaceFeatures);  // 0x26 Display Interface Features
static NVT_STATUS parseDisplayId20Stereo(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                  // 0x27 Stereo Display Interface
static NVT_STATUS parseDisplayId20TiledDisplay(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_TILED_DISPLAY_TOPOLOGY *pTileTopo);                   // 0x28 Tiled Display Topology
static NVT_STATUS parseDisplayId20ContainerId(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_CONTAINERID *pContainerId);                            // 0x29 ContainerID
static NVT_STATUS parseDisplayId20VendorSpecific(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_VENDOR_SPECIFIC *pVendorSpecific);                  // 0x7E Vendor-specific
static NVT_STATUS parseDisplayId20CtaData(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);                                 // 0x81 CTA DisplayID

// Helper function
static NVT_STATUS getPrimaryUseCase(NvU8 product_type, NVT_DISPLAYID_PRODUCT_PRIMARY_USE_CASE *primary_use_case);
static NVT_STATUS parseDisplayId20Timing7Descriptor(const DISPLAYID_2_0_TIMING_7_DESCRIPTOR *pDescriptor, NVT_TIMING *pTiming, NvU8 revision, NvU8 count);
static NVT_STATUS parseDisplayId20Timing9Descriptor(const DISPLAYID_2_0_TIMING_9_DESCRIPTOR *pDescriptor, NVT_TIMING *pTiming, NvU8 count);
static NVT_STATUS parseDisplayId20Timing10Descriptor(const void *pDescriptor, NVT_TIMING *pTiming, NvU8 payloadbytes, NvU8 count);
static NvU32      greatestCommonDenominator(NvU32 x, NvU32 y);
static NvU8       getExistedTimingSeqNumber(NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, enum NVT_TIMING_TYPE);

/*
 *  The Second-generation version of VESA DisplayID Standard
 *                    DisplayID v2.0
 *
 *  @brief Parses a displayID20 section
 *
 *  @param pDisplayId     The DisplayId20 Section Block ()
 *  @param length         Size of the displayId section Block
 *  @param pDisplayIdInfo Need to parse the raw data to store as NV structure
 *
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NV_STDCALL
NvTiming_parseDisplayId20Info(
    const NvU8 *pDisplayId,
    NvU32 length,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS             status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_SECTION *pSection = NULL;
    NvU32                  offset = 0;
    NvU32                  extensionIndex = 0;
    NvU32                  idx = 0;

    // parameter check
    if ((pDisplayId == NULL) ||
        (pDisplayIdInfo == NULL))
    {
        return NVT_STATUS_ERR;
    }

    pSection = (const DISPLAYID_2_0_SECTION *)pDisplayId;

    if ((pSection->header.version < DISPLAYID_2_0_VERSION) ||
        (DISPLAYID_2_0_SECTION_SIZE_TOTAL(pSection->header) > length))
    {
        return NVT_STATUS_ERR;
    }

    NVMISC_MEMSET(pDisplayIdInfo, 0, sizeof(NVT_DISPLAYID_2_0_INFO));

    status = parseDisplayId20BaseSection(pSection, pDisplayIdInfo);
    if (status != NVT_STATUS_SUCCESS)
    {
        return status;
    }

    pDisplayIdInfo->extension_count = pSection->header.extension_count;
    for (extensionIndex = 0; extensionIndex < pDisplayIdInfo->extension_count; extensionIndex++)
    {
        // Get offset to the next section.
        offset += DISPLAYID_2_0_SECTION_SIZE_TOTAL(pSection->header);

        // validate the next section buffer is valid
        pSection = (const DISPLAYID_2_0_SECTION *)(pDisplayId + offset);
        if ((offset + DISPLAYID_2_0_SECTION_SIZE_TOTAL(pSection->header)) > length)
        {
            return NVT_STATUS_ERR;
        }

        // process the section
        status = parseDisplayId20ExtensionSection(pSection, pDisplayIdInfo);
        if (status != NVT_STATUS_SUCCESS)
        {
            return status;
        }
    }

    for (idx = 0; idx < pDisplayIdInfo->total_timings; idx++)
    {
        updateColorFormatForDisplayId20Timings(pDisplayIdInfo, idx);
    }

    return status;
}

NvU32 NvTiming_DisplayID2ValidationMask(
    NVT_DISPLAYID_2_0_INFO *pDisplayId20Info,
    NvBool bIsStrongValidation)
{
    NvU32 j;
    NvU32 ret = 0;

    // check the DisplayId2 version and signature
    if (pDisplayId20Info->version != 0x2)
    {
        ret |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_VERSION);
    }
    
    if (!pDisplayId20Info->valid_data_blocks.product_id_present)
    {
        ret |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_PRODUCT_ID);
    }
    
    if (pDisplayId20Info->primary_use_case >= PRODUCT_PRIMARY_USE_GENERIC_DISPLAY && 
        pDisplayId20Info->primary_use_case <= PRODUCT_PRIMARY_USE_HEAD_MOUNT_AUGMENTED_REALITY)
    {
        if (!(pDisplayId20Info->valid_data_blocks.parameters_present        &&
              pDisplayId20Info->valid_data_blocks.interface_feature_present &&
              pDisplayId20Info->valid_data_blocks.type7Timing_present       &&
              pDisplayId20Info->total_timings))
        {
            ret |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_NO_DATA_BLOCK);
        }
    }

    // Strong validation to follow
    if (bIsStrongValidation == NV_TRUE)
    {
        // TODO : For each of the Data Block limitation
        // Type 7 Timings data block
        for (j = 0; j <= pDisplayId20Info->total_timings; j++)
        {
            if ( NVT_PREFERRED_TIMING_IS_DISPLAYID(pDisplayId20Info->timing[j].etc.flag) && 
                 (pDisplayId20Info->display_param.h_pixels != 0) &&
                 (pDisplayId20Info->display_param.v_pixels != 0))
            {
                if ( pDisplayId20Info->timing[j].HVisible != pDisplayId20Info->display_param.h_pixels ||
                     pDisplayId20Info->timing[j].VVisible != pDisplayId20Info->display_param.v_pixels )
                {
                    ret |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_NO_DATA_BLOCK);
                    break;
                }
            }
        }
        // TODO : go on the next data block validation if it existed.
        // TODO : validate extension blocks
    }

    return ret;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS 
NvTiming_DisplayID2ValidationDataBlocks(
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, 
    NvBool bIsStrongValidation)
{
    if (NvTiming_DisplayID2ValidationMask(pDisplayIdInfo, bIsStrongValidation) != 0) 
    {
        return NVT_STATUS_ERR;
    }
    else
    {
        return NVT_STATUS_SUCCESS;
    }
}

//  DisplayID20 Entry point functions
CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20BaseSection(
    const DISPLAYID_2_0_SECTION *pSection,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS    status = NVT_STATUS_SUCCESS;

    // validate for section checksum before processing the data block
    if (computeDisplayId20SectionCheckSum((const NvU8 *)pSection, DISPLAYID_2_0_SECTION_SIZE_TOTAL(pSection->header)) != 0)
    {
        status |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_CHECKSUM);
        return status;
    }

    pDisplayIdInfo->revision = pSection->header.revision;
    pDisplayIdInfo->version = pSection->header.version;

    status = getPrimaryUseCase(pSection->header.product_type,
        &pDisplayIdInfo->primary_use_case);
    if (status != NVT_STATUS_SUCCESS)
    {
        return status;
    }

    status = parseDisplayId20SectionDataBlocks(pSection, pDisplayIdInfo);

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20ExtensionSection(
    const DISPLAYID_2_0_SECTION *pSection,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS    status = NVT_STATUS_SUCCESS;

    // validate for section checksum before processing the data block
    if (computeDisplayId20SectionCheckSum((const NvU8 *)pSection, DISPLAYID_2_0_SECTION_SIZE_TOTAL(pSection->header)) != 0)
    {
        status |= NVT_DID2_VALIDATION_ERR_MASK(NVT_DID2_VALIDATION_ERR_CHECKSUM);
        return status;
    }

    nvt_assert(pSection->header.version >= DISPLAYID_2_0_VERSION);
    nvt_assert(pSection->header.extension_count == 0);
    nvt_assert(pSection->header.product_type == DISPLAYID_2_0_PROD_EXTENSION);

    status = parseDisplayId20SectionDataBlocks(pSection, pDisplayIdInfo);

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20SectionDataBlocks(
    const DISPLAYID_2_0_SECTION *pSection,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NvU32                            i = 0;
    NvU32                            offset = 0;
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock = NULL;
    NVT_STATUS                       status = NVT_STATUS_SUCCESS;

    while (offset < pSection->header.section_bytes)
    {
        // Get current block
        pDataBlock = (const DISPLAYID_2_0_DATA_BLOCK_HEADER *)(pSection->data + offset);

        // detected zero padding
        if (pDataBlock->type == 0)
        {
            for (i = offset; i < pSection->header.section_bytes; i++)
            {
                // validate that all paddings are zeros
                nvt_assert(pSection->data[i] == 0);
            }
            break;
        }

        // check data block is valid. 
        if ((offset + DISPLAYID_2_0_DATA_BLOCK_SIZE_TOTAL(pDataBlock)) > pSection->header.section_bytes)
        {
            return NVT_STATUS_ERR;
        }

        // parse the data block
        status = parseDisplayId20DataBlock(pDataBlock, pDisplayIdInfo);
        if (status != NVT_STATUS_SUCCESS)
        {
            return status;
        }
        
        switch (pDataBlock->type)
        {
            case DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY:
                pDisplayIdInfo->valid_data_blocks.product_id_present             = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_DISPLAY_PARAM:
                pDisplayIdInfo->valid_data_blocks.parameters_present             = NV_TRUE;
                if (pDisplayIdInfo->display_param.audio_speakers_integrated == AUDIO_SPEAKER_INTEGRATED_SUPPORTED)
                {
                    pDisplayIdInfo->basic_caps |= NVT_DISPLAY_2_0_CAP_BASIC_AUDIO;
                }
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_TIMING_7:
                pDisplayIdInfo->valid_data_blocks.type7Timing_present            = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_TIMING_8:
                pDisplayIdInfo->valid_data_blocks.type8Timing_present            = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_TIMING_9:
                pDisplayIdInfo->valid_data_blocks.type9Timing_present            = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_RANGE_LIMITS:
                pDisplayIdInfo->valid_data_blocks.dynamic_range_limit_present    = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_INTERFACE_FEATURES:
                pDisplayIdInfo->valid_data_blocks.interface_feature_present      = NV_TRUE;

                // Supported - Color depth is supported for all supported timings.  Supported timing includes all Display-ID exposed timings 
                // (that is timing exposed using DisplayID timing types and CTA VICs)
                if (IS_BPC_SUPPORTED_COLORFORMAT(pDisplayIdInfo->interface_features.yuv444.bpcs))
                {
                    pDisplayIdInfo->basic_caps |= NVT_DISPLAY_2_0_CAP_YCbCr_444;
                }

                if (IS_BPC_SUPPORTED_COLORFORMAT(pDisplayIdInfo->interface_features.yuv422.bpcs))
                {
                    pDisplayIdInfo->basic_caps |= NVT_DISPLAY_2_0_CAP_YCbCr_422;
                }

                if (pDisplayIdInfo->interface_features.audio_capability.support_48khz   || 
                    pDisplayIdInfo->interface_features.audio_capability.support_44_1khz || 
                    pDisplayIdInfo->interface_features.audio_capability.support_32khz)
                {
                    pDisplayIdInfo->basic_caps |= NVT_DISPLAY_2_0_CAP_BASIC_AUDIO;
                }
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_STEREO:
                pDisplayIdInfo->valid_data_blocks.stereo_interface_present       = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_TILED_DISPLAY:
                pDisplayIdInfo->valid_data_blocks.tiled_display_present          = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_CONTAINER_ID:
                pDisplayIdInfo->valid_data_blocks.container_id_present           = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_VENDOR_SPEC:
                pDisplayIdInfo->valid_data_blocks.vendor_specific_present        = NV_TRUE;
                break;
            case DISPLAYID_2_0_BLOCK_TYPE_CTA_DATA:
                pDisplayIdInfo->valid_data_blocks.cta_data_present               = NV_TRUE;
                break;
            default:
                status = NVT_STATUS_ERR;
        }

        // advance to the next block
        offset += DISPLAYID_2_0_DATA_BLOCK_SIZE_TOTAL(pDataBlock);
    }

    return status;
}
 
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS 
parseDisplayId20DataBlock(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    
    switch (pDataBlock->type)
    {
    case DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY:
        status = parseDisplayId20ProductIdentity(pDataBlock, &pDisplayIdInfo->product_identity);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_DISPLAY_PARAM:
        status = parseDisplayId20DisplayParam(pDataBlock, &pDisplayIdInfo->display_param);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_TIMING_7:
        status = parseDisplayId20Timing7(pDataBlock, pDisplayIdInfo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_TIMING_8:
        status = parseDisplayId20Timing8(pDataBlock, pDisplayIdInfo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_TIMING_9:
        status = parseDisplayId20Timing9(pDataBlock, pDisplayIdInfo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_TIMING_10:
        status = parseDisplayId20Timing10(pDataBlock, pDisplayIdInfo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_RANGE_LIMITS:
        status = parseDisplayId20RangeLimit(pDataBlock, &pDisplayIdInfo->range_limits);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_INTERFACE_FEATURES:
        status = parseDisplayId20DisplayInterfaceFeatures(pDataBlock, &pDisplayIdInfo->interface_features);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_STEREO:
        status = parseDisplayId20Stereo(pDataBlock, pDisplayIdInfo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_TILED_DISPLAY:
        status = parseDisplayId20TiledDisplay(pDataBlock, &pDisplayIdInfo->tile_topo);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_CONTAINER_ID:
        status = parseDisplayId20ContainerId(pDataBlock, &pDisplayIdInfo->container_id);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_VENDOR_SPEC:
        status = parseDisplayId20VendorSpecific(pDataBlock, &pDisplayIdInfo->vendor_specific);
        break;
    case DISPLAYID_2_0_BLOCK_TYPE_CTA_DATA:
        status = parseDisplayId20CtaData(pDataBlock, pDisplayIdInfo);
        break;
    default:
        status = NVT_STATUS_ERR;
    }
    return status;
}

// All Data Blocks Parsing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS
parseDisplayId20ProductIdentity(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_PRODUCT_IDENTITY *pProductIdentity)
{
    NVT_STATUS                                status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_PROD_IDENTIFICATION_BLOCK  *pProductIdBlock = NULL;

    if (pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY)
    {
        pProductIdBlock = (const DISPLAYID_2_0_PROD_IDENTIFICATION_BLOCK *)pDataBlock;

        pProductIdentity->vendor_id = (pProductIdBlock->vendor[0] << 16) |
            (pProductIdBlock->vendor[1] << 8) |
            (pProductIdBlock->vendor[2]);
        pProductIdentity->product_id = (pProductIdBlock->product_code[0]) |
            (pProductIdBlock->product_code[1] << 8);
        pProductIdentity->serial_number = (pProductIdBlock->serial_number[0]) |
            (pProductIdBlock->serial_number[1] << 8) |
            (pProductIdBlock->serial_number[2] << 16) |
            (pProductIdBlock->serial_number[3] << 24);
        pProductIdentity->week = (pProductIdBlock->model_tag >= 1 && pProductIdBlock->model_tag <= 52) ?
            pProductIdBlock->model_tag : 0;
        pProductIdentity->year = (pProductIdBlock->model_tag == 0xFF) ?
            pProductIdBlock->model_year :
            pProductIdBlock->model_year + 2000;

        if (pProductIdBlock->product_name_string_size != 0)
        {
            NVMISC_STRNCPY((char *)pProductIdentity->product_string,
                (const char *)pProductIdBlock->product_name_string,
                pProductIdBlock->product_name_string_size);
        }
        pProductIdentity->product_string[pProductIdBlock->product_name_string_size] = '\0';
    }
    else
    {
        return NVT_STATUS_ERR;
    }

    return status;
}


CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20DisplayParam(    
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_DISPLAY_PARAMETERS *pDisplayParam)
{
    NVT_STATUS                          status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_DISPLAY_PARAM_BLOCK  *pDisplayParamBlock = NULL;

    if ((pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_DISPLAY_PARAM) &&
        (pDataBlock->data_bytes == DISPLAYID_2_0_DISPLAY_PARAM_BLOCK_PAYLOAD_LENGTH))
    {
        pDisplayParamBlock = (const DISPLAYID_2_0_DISPLAY_PARAM_BLOCK *)pDataBlock;

        pDisplayParam->revision = pDisplayParamBlock->header.revision;
        pDisplayParam->h_image_size_micro_meter = (pDisplayParamBlock->horizontal_image_size[1] << 8 |
            pDisplayParamBlock->horizontal_image_size[0]) *
            (pDisplayParamBlock->header.image_size_multiplier ? 1000 : 100);
        pDisplayParam->v_image_size_micro_meter = (pDisplayParamBlock->vertical_image_size[1] << 8 |
            pDisplayParamBlock->vertical_image_size[0]) *
            (pDisplayParamBlock->header.image_size_multiplier ? 1000 : 100);
        pDisplayParam->h_pixels = pDisplayParamBlock->horizontal_pixel_count[1] << 8 |
            pDisplayParamBlock->horizontal_pixel_count[0];
        pDisplayParam->v_pixels = pDisplayParamBlock->vertical_pixel_count[1] << 8 |
            pDisplayParamBlock->vertical_pixel_count[0];

        pDisplayParam->scan_orientation          = pDisplayParamBlock->feature.scan_orientation;
        pDisplayParam->audio_speakers_integrated = pDisplayParamBlock->feature.audio_speaker_information ? AUDIO_SPEAKER_INTEGRATED_NOT_SUPPORTED : AUDIO_SPEAKER_INTEGRATED_SUPPORTED;
        pDisplayParam->color_map_standard        = pDisplayParamBlock->feature.color_information ? COLOR_MAP_CIE_1976 : COLOR_MAP_CIE_1931;

        // 12 bits Binary Fraction Representations
        pDisplayParam->primaries[0].x = pDisplayParamBlock->primary_color_1_chromaticity.color_bits_mid.color_x_bits_high << 8 |
            pDisplayParamBlock->primary_color_1_chromaticity.color_x_bits_low;
        pDisplayParam->primaries[0].y = pDisplayParamBlock->primary_color_1_chromaticity.color_y_bits_high << 4 |
            pDisplayParamBlock->primary_color_1_chromaticity.color_bits_mid.color_y_bits_low;
        pDisplayParam->primaries[1].x = pDisplayParamBlock->primary_color_2_chromaticity.color_bits_mid.color_x_bits_high << 8 |
            pDisplayParamBlock->primary_color_2_chromaticity.color_x_bits_low;
        pDisplayParam->primaries[1].y = pDisplayParamBlock->primary_color_2_chromaticity.color_y_bits_high << 4 |
            pDisplayParamBlock->primary_color_2_chromaticity.color_bits_mid.color_y_bits_low;
        pDisplayParam->primaries[2].x = pDisplayParamBlock->primary_color_3_chromaticity.color_bits_mid.color_x_bits_high << 8 |
            pDisplayParamBlock->primary_color_3_chromaticity.color_x_bits_low;
        pDisplayParam->primaries[2].y = pDisplayParamBlock->primary_color_3_chromaticity.color_y_bits_high << 4 |
            pDisplayParamBlock->primary_color_3_chromaticity.color_bits_mid.color_y_bits_low;
        pDisplayParam->white.x = pDisplayParamBlock->white_point_chromaticity.color_bits_mid.color_x_bits_high << 8 |
            pDisplayParamBlock->white_point_chromaticity.color_x_bits_low;
        pDisplayParam->white.y = pDisplayParamBlock->white_point_chromaticity.color_y_bits_high << 4 |
            pDisplayParamBlock->white_point_chromaticity.color_bits_mid.color_y_bits_low;

        // IEEE 754 half-precision binary floating-point format
        pDisplayParam->native_max_luminance_full_coverage = pDisplayParamBlock->max_luminance_full_coverage[1] << 8 |
            pDisplayParamBlock->max_luminance_full_coverage[0];
        pDisplayParam->native_max_luminance_1_percent_rect_coverage = pDisplayParamBlock->max_luminance_1_percent_rectangular_coverage[1] << 8 |
            pDisplayParamBlock->max_luminance_1_percent_rectangular_coverage[0];
        pDisplayParam->native_min_luminance = pDisplayParamBlock->min_luminance[1] << 8 |
            pDisplayParamBlock->min_luminance[0];

        if (pDisplayParamBlock->feature.luminance_information == 0)
        {
            pDisplayParam->native_luminance_info = NATIVE_LUMINANCE_INFO_MIN_GURANTEE_VALUE;
        }
        else if (pDisplayParamBlock->feature.luminance_information == 1)
        {
            pDisplayParam->native_luminance_info = NATIVE_LUMINANCE_INFO_SOURCE_DEVICE_GUIDANCE;
        }
        else
        {
            return NVT_STATUS_ERR;
        }

        UPDATE_BPC_FOR_COLORFORMAT(pDisplayParam->native_color_depth,
            pDisplayParamBlock->color_depth_and_device_technology.color_depth == NATIVE_COLOR_BPC_6,
            pDisplayParamBlock->color_depth_and_device_technology.color_depth == NATIVE_COLOR_BPC_8,
            pDisplayParamBlock->color_depth_and_device_technology.color_depth == NATIVE_COLOR_BPC_10,
            pDisplayParamBlock->color_depth_and_device_technology.color_depth == NATIVE_COLOR_BPC_12,
            0,
            pDisplayParamBlock->color_depth_and_device_technology.color_depth == NATIVE_COLOR_BPC_16);

        pDisplayParam->device_technology = pDisplayParamBlock->color_depth_and_device_technology.device_technology;
        if (pDisplayParam->revision == 1)
        {
            pDisplayParam->device_theme_Preference = pDisplayParamBlock->color_depth_and_device_technology.device_theme_preference;
        }
        pDisplayParam->gamma_x100 = (pDisplayParamBlock->gamma_EOTF + 100);
    }
    else
    {
        return NVT_STATUS_ERR;
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE) 
static NVT_STATUS
parseDisplayId20Timing7(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS                          status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TIMING_7_BLOCK *pTiming7Block    = NULL;
    NvU32                               descriptorCount  = 0;
    NvU8                                revision         = 0;
    NvU8                                i                = 0;
    NvU8                                startSeqNumber   = 0;

    NVT_TIMING                          newTiming;

    if (pDataBlock->type != DISPLAYID_2_0_BLOCK_TYPE_TIMING_7)
    {
        nvt_assert(0);
        return NVT_STATUS_ERR; 
    }

    pTiming7Block = (const DISPLAYID_2_0_TIMING_7_BLOCK *)pDataBlock;

    // Based on the DisplayID_2_0_E7 spec:
    // the Future descriptor can be defined with more than 20 Byte per descriptor without creating a new timing type
    if (pTiming7Block->header.payload_bytes_len == 0)
    {
        if (pDataBlock->data_bytes % sizeof(DISPLAYID_2_0_TIMING_7_DESCRIPTOR) != 0)
        {
            nvt_assert(0);
            return NVT_STATUS_ERR;
        }

        descriptorCount = pDataBlock->data_bytes / (sizeof(DISPLAYID_2_0_TIMING_7_DESCRIPTOR) + pTiming7Block->header.payload_bytes_len);

        if (descriptorCount < 1 || descriptorCount > DISPLAYID_2_0_TIMING_7_MAX_DESCRIPTORS)
        {
            nvt_assert(0);
            return NVT_STATUS_ERR;
        }

        startSeqNumber = getExistedTimingSeqNumber(pDisplayIdInfo, NVT_TYPE_DISPLAYID_7);

        for (i = 0; i < descriptorCount; i++)
        {
            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));
            if (parseDisplayId20Timing7Descriptor(&pTiming7Block->descriptors[i], &newTiming, revision, startSeqNumber+i) == NVT_STATUS_SUCCESS)
            {
                if (!assignNextAvailableDisplayId20Timing(pDisplayIdInfo, &newTiming))
                {
                    break;
                }
            }
        }
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE) 
static  NVT_STATUS
parseDisplayId20Timing8(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TIMING_8_BLOCK *pTiming8Block = NULL;
    NVT_TIMING newTiming;
    NvU8       codeType       = DISPLAYID_2_0_TIMING_CODE_RSERVED;
    NvU8       codeCount      = 0;
    NvU8       startSeqNumber = 0;
    NvU8       i;


    if (pDataBlock->type != DISPLAYID_2_0_BLOCK_TYPE_TIMING_8)
    {
        nvt_assert(0);
        return NVT_STATUS_ERR;
    }

    pTiming8Block = (const DISPLAYID_2_0_TIMING_8_BLOCK *)pDataBlock;

    // 1-byte descriptor timing code
    if (pTiming8Block->header.timing_code_size == DISPLAYID_2_0_TIMING_CODE_SIZE_1_BYTE)
    {
        if (pDataBlock->data_bytes % sizeof(DISPLAYID_2_0_TIMING_8_ONE_BYTE_CODE) != 0)
        {   
            return NVT_STATUS_ERR;
        }

        codeCount = pDataBlock->data_bytes / sizeof(DISPLAYID_2_0_TIMING_8_ONE_BYTE_CODE);
        if (codeCount < 1 || codeCount > DISPLAYID_2_0_TIMING_8_MAX_CODES)
        {
            return NVT_STATUS_ERR;
        }

        codeType = pTiming8Block->header.timing_code_type;
        startSeqNumber = getExistedTimingSeqNumber(pDisplayIdInfo, NVT_TYPE_DISPLAYID_8);

        for (i = 0; i < codeCount; i++)
        {
            NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

            if (codeType == DISPLAYID_2_0_TIMING_CODE_DMT)
            {
                if (NvTiming_EnumDMT((NvU32)(pTiming8Block->timing_code_1[i].timing_code),
                                     &newTiming) != NVT_STATUS_SUCCESS)
                {
                    break;
                }
            }
            else if (codeType == DISPLAYID_2_0_TIMING_CODE_CTA_VIC)
            {
                if (NvTiming_EnumCEA861bTiming((NvU32)(pTiming8Block->timing_code_1[i].timing_code), 
                                               &newTiming) != NVT_STATUS_SUCCESS)
                {
                    break;
                }
            }
            else if (codeType == DISPLAYID_2_0_TIMING_CODE_HDMI_VIC)
            {    
                if (NvTiming_EnumHdmiVsdbExtendedTiming((NvU32)(pTiming8Block->timing_code_1[i].timing_code), 
                                                    &newTiming) != NVT_STATUS_SUCCESS)
                {
                    break;
                }
            }
            else
            {
                // RESERVED
                break;
            }

            newTiming.etc.flag |= ((pTiming8Block->header.revision >= 1) && pTiming8Block->header.is_support_yuv420) ? NVT_FLAG_DISPLAYID_2_0_EXPLICT_YUV420 : 0;
            newTiming.etc.status = NVT_STATUS_DISPLAYID_8N(++startSeqNumber);
            
            NVT_SNPRINTF((char *)newTiming.etc.name, sizeof(newTiming.etc.name), "DID20-Type8:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                  (int)NVT_GET_TIMING_STATUS_SEQ(newTiming.etc.status),
                                                                                  (int)newTiming.HVisible, (int)newTiming.VVisible,
                                                                                  (int)newTiming.etc.rrx1k/1000, (int)newTiming.etc.rrx1k%1000, 
                                                                                  (newTiming.interlaced ? "I":"P"));
            newTiming.etc.name[sizeof(newTiming.etc.name) - 1] = '\0';
            
            if (!assignNextAvailableDisplayId20Timing(pDisplayIdInfo, &newTiming))
            {
                break;
            }
        }
    }
    else
    {
        nvt_assert(0);
        // TODO : 2-byte descriptor timing code did not define yet in DID20.
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE) 
static NVT_STATUS
parseDisplayId20Timing9(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS                          status           = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TIMING_9_BLOCK *pTiming9Block    = NULL;
    NVT_TIMING                          newTiming;
    NvU32                               descriptorCount  = 0;
    NvU8                                startSeqNumber   = 0;
    NvU8                                i                = 0;
    
    if (pDataBlock->type != DISPLAYID_2_0_BLOCK_TYPE_TIMING_9)
    {
        nvt_assert(0);
        return NVT_STATUS_ERR;
    }

    descriptorCount = pDataBlock->data_bytes / sizeof(DISPLAYID_2_0_TIMING_9_DESCRIPTOR);
    if (descriptorCount < 1 || descriptorCount > DISPLAYID_2_0_TIMING_9_MAX_DESCRIPTORS)
    {
        nvt_assert(0);
        return NVT_STATUS_ERR;
    }

    pTiming9Block = (const DISPLAYID_2_0_TIMING_9_BLOCK *)pDataBlock;

    startSeqNumber = getExistedTimingSeqNumber(pDisplayIdInfo, NVT_TYPE_DISPLAYID_9);

    for (i = 0; i < descriptorCount; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (parseDisplayId20Timing9Descriptor(&pTiming9Block->descriptors[i], &newTiming, startSeqNumber+i) == NVT_STATUS_SUCCESS)
        {
            if (!assignNextAvailableDisplayId20Timing(pDisplayIdInfo, &newTiming))
            {
                break;
            }
        }
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE) 
static NVT_STATUS
parseDisplayId20Timing10(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TIMING_10_BLOCK *pTiming10Block = NULL;
    NvU32  descriptorCount                              = 0;
    NvU8   startSeqNumber                               = 0;
    NvU8   i                                            = 0;

    NVT_TIMING   newTiming;

    pTiming10Block = (const DISPLAYID_2_0_TIMING_10_BLOCK *)pDataBlock;

    if (pTiming10Block->header.type != DISPLAYID_2_0_BLOCK_TYPE_TIMING_10)
    {
        nvt_assert(0);
        return NVT_STATUS_ERR;
    }

    if (pTiming10Block->header.payload_bytes_len == DISPLAYID_2_0_TIMING_10_PAYLOAD_BYTES_6)
    {
        descriptorCount = pDataBlock->data_bytes / sizeof(DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR);

        if (descriptorCount < 1 || descriptorCount > DISPLAYID_2_0_TIMING_10_MAX_6BYTES_DESCRIPTORS)
        {
            nvt_assert(0);
            return NVT_STATUS_ERR;
        }
    }
    else if (pTiming10Block->header.payload_bytes_len == DISPLAYID_2_0_TIMING_10_PAYLOAD_BYTES_7)
    {
        descriptorCount = pDataBlock->data_bytes / sizeof(DISPLAYID_2_0_TIMING_10_7BYTES_DESCRIPTOR);

        if (descriptorCount < 1 || descriptorCount > DISPLAYID_2_0_TIMING_10_MAX_7BYTES_DESCRIPTORS)
        {
            nvt_assert(0);
            return NVT_STATUS_ERR;
        }
    }

    startSeqNumber = getExistedTimingSeqNumber(pDisplayIdInfo, NVT_TYPE_DISPLAYID_10);

    for (i = 0; i < descriptorCount; i++)
    {
        NVMISC_MEMSET(&newTiming, 0, sizeof(newTiming));

        if (NVT_STATUS_SUCCESS == parseDisplayId20Timing10Descriptor(&pTiming10Block->descriptors[i], &newTiming, pTiming10Block->header.payload_bytes_len, startSeqNumber+i))
        {
            if (!assignNextAvailableDisplayId20Timing(pDisplayIdInfo, &newTiming))
            {
                break;
            }
        }
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20RangeLimit(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_RANGE_LIMITS *pRangeLimits)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_RANGE_LIMITS_BLOCK *pRangeLimitsBlock = NULL;

    if ((pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_RANGE_LIMITS) &&
        (pDataBlock->data_bytes == DISPLAYID_2_0_RANGE_LIMITS_BLOCK_PAYLOAD_LENGTH))
    {
        pRangeLimitsBlock = (const DISPLAYID_2_0_RANGE_LIMITS_BLOCK *)pDataBlock;

        pRangeLimits->revision = pDataBlock->revision;

        pRangeLimits->pclk_min = (pRangeLimitsBlock->pixel_clock_min[2] << 16 |
            pRangeLimitsBlock->pixel_clock_min[1] << 8 |
            pRangeLimitsBlock->pixel_clock_min[0]) + 1;
        pRangeLimits->pclk_max = (pRangeLimitsBlock->pixel_clock_max[2] << 16 |
            pRangeLimitsBlock->pixel_clock_max[1] << 8 |
            pRangeLimitsBlock->pixel_clock_max[0]) + 1;
        pRangeLimits->vfreq_min = pRangeLimitsBlock->vertical_frequency_min;
        if (pRangeLimits->revision == 1)
        {
            pRangeLimits->vfreq_max = pRangeLimitsBlock->dynamic_video_timing_range_support.vertical_frequency_max_9_8 << 8 | pRangeLimitsBlock->vertical_frequency_max_7_0;   
        }
        else 
        {
            pRangeLimits->vfreq_max = pRangeLimitsBlock->vertical_frequency_max_7_0;
        }

        pRangeLimits->seamless_dynamic_video_timing_change = pRangeLimitsBlock->dynamic_video_timing_range_support.seamless_dynamic_video_timing_change;
    }
    else
    {
        return NVT_STATUS_ERR;
    }

    return status;
}

#define ADD_COLOR_SPACE_EOTF_COMBINATION(_pInterfaceFeatures, _color_space, _eotf) do {                                                        \
                    (_pInterfaceFeatures)->colorspace_eotf_combination[(_pInterfaceFeatures)->combination_count].color_space = (_color_space); \
                    (_pInterfaceFeatures)->colorspace_eotf_combination[(_pInterfaceFeatures)->combination_count].eotf        = (_eotf);        \
                    (_pInterfaceFeatures)->combination_count++;                                                                                \
                } while(0)

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20DisplayInterfaceFeatures(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_INTERFACE_FEATURES *pInterfaceFeatures)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    NvU32       i = 0;
    const DISPLAYID_2_0_INTERFACE_FEATURES_BLOCK *pInterfaceFeaturesBlock = NULL;

    if (pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_INTERFACE_FEATURES &&
        pDataBlock->data_bytes >= DISPLAYID_2_0_INTERFACE_FEATURES_BLOCK_PAYLOAD_LENGTH_MIN)
    {
        pInterfaceFeaturesBlock = (const DISPLAYID_2_0_INTERFACE_FEATURES_BLOCK *)pDataBlock;
        pInterfaceFeatures->revision = pDataBlock->revision;

        UPDATE_BPC_FOR_COLORFORMAT(pInterfaceFeatures->rgb444,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_6,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_8,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_10,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_12,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_14,
            pInterfaceFeaturesBlock->interface_color_depth_rgb.bit_per_primary_16);
        UPDATE_BPC_FOR_COLORFORMAT(pInterfaceFeatures->yuv444,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_6,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_8,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_10,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_12,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_14,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr444.bit_per_primary_16);
        UPDATE_BPC_FOR_COLORFORMAT(pInterfaceFeatures->yuv422,
            0,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr422.bit_per_primary_8,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr422.bit_per_primary_10,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr422.bit_per_primary_12,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr422.bit_per_primary_14,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr422.bit_per_primary_16);
        UPDATE_BPC_FOR_COLORFORMAT(pInterfaceFeatures->yuv420,
            0,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr420.bit_per_primary_8,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr420.bit_per_primary_10,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr420.bit_per_primary_12,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr420.bit_per_primary_14,
            pInterfaceFeaturesBlock->interface_color_depth_ycbcr420.bit_per_primary_16);

        // * 74.25MP/s 
        pInterfaceFeatures->yuv420_min_pclk = pInterfaceFeaturesBlock->min_pixel_rate_ycbcr420 *
                                              7425;

        pInterfaceFeatures->audio_capability.support_48khz =
            pInterfaceFeaturesBlock->audio_capability.sample_rate_48_khz;
        pInterfaceFeatures->audio_capability.support_44_1khz =
            pInterfaceFeaturesBlock->audio_capability.sample_rate_44_1_khz;
        pInterfaceFeatures->audio_capability.support_32khz =
            pInterfaceFeaturesBlock->audio_capability.sample_rate_32_khz;

        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_srgb_eotf_srgb)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_SRGB,
                INTERFACE_EOTF_SRGB);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_bt601_eotf_bt601)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_BT601,
                INTERFACE_EOTF_BT601);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_bt709_eotf_bt1886)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_BT709,
                INTERFACE_EOTF_BT1886);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_adobe_rgb_eotf_adobe_rgb)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_ADOBE_RGB,
                INTERFACE_EOTF_ADOBE_RGB);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_dci_p3_eotf_dci_p3)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_DCI_P3,
                INTERFACE_EOTF_DCI_P3);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_bt2020_eotf_bt2020)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_BT2020,
                INTERFACE_EOTF_BT2020);
        }
        if (pInterfaceFeaturesBlock->color_space_and_eotf_1.color_space_bt2020_eotf_smpte_st2084)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                INTERFACE_COLOR_SPACE_BT2020,
                INTERFACE_EOTF_SMPTE_ST2084);
        }

        for (i = 0; i < pInterfaceFeaturesBlock->additional_color_space_and_eotf_count.count; i++)
        {
            ADD_COLOR_SPACE_EOTF_COMBINATION(pInterfaceFeatures,
                pInterfaceFeaturesBlock->additional_color_space_and_eotf[i].color_space,
                pInterfaceFeaturesBlock->additional_color_space_and_eotf[i].eotf);
        }
    }
    else
    {
        return NVT_STATUS_ERR;
    }
    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20Stereo(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;

    // TODO: Implement the parsing here.

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20TiledDisplay(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_TILED_DISPLAY_TOPOLOGY *pTileTopo)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TILED_DISPLAY_BLOCK *pTiledDisplayBlock = NULL;

    if ((pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_TILED_DISPLAY) &&
        (pDataBlock->data_bytes == DISPLAYID_2_0_TILED_DISPLAY_BLOCK_PAYLOAD_LENGTH))
    {
        pTiledDisplayBlock = (const DISPLAYID_2_0_TILED_DISPLAY_BLOCK *)pDataBlock;

        pTileTopo->revision = pDataBlock->revision;

        pTileTopo->capability.bSingleEnclosure = pTiledDisplayBlock->capability.single_enclosure;
        pTileTopo->capability.bHasBezelInfo = pTiledDisplayBlock->capability.has_bezel_info;
        pTileTopo->capability.multi_tile_behavior = pTiledDisplayBlock->capability.multi_tile_behavior;
        pTileTopo->capability.single_tile_behavior = pTiledDisplayBlock->capability.single_tile_behavior;

        pTileTopo->topology.row = ((pTiledDisplayBlock->topo_loc_high.row << 5) |
            (pTiledDisplayBlock->topo_low.row)) + 1;
        pTileTopo->topology.col = ((pTiledDisplayBlock->topo_loc_high.col << 5) |
            (pTiledDisplayBlock->topo_low.col)) + 1;
        pTileTopo->location.x = ((pTiledDisplayBlock->topo_loc_high.x << 5) |
            (pTiledDisplayBlock->loc_low.x));
        pTileTopo->location.y = ((pTiledDisplayBlock->topo_loc_high.y << 5) |
            (pTiledDisplayBlock->loc_low.y));

        pTileTopo->native_resolution.width = ((pTiledDisplayBlock->native_resolution.width_high << 8) |
            pTiledDisplayBlock->native_resolution.width_low) + 1;
        pTileTopo->native_resolution.height = ((pTiledDisplayBlock->native_resolution.height_high << 8) |
            pTiledDisplayBlock->native_resolution.height_low) + 1;

        pTileTopo->bezel_info.top = (pTiledDisplayBlock->bezel_info.top *
            pTiledDisplayBlock->bezel_info.pixel_density) / 10;
        pTileTopo->bezel_info.bottom = (pTiledDisplayBlock->bezel_info.bottom *
            pTiledDisplayBlock->bezel_info.pixel_density) / 10;
        pTileTopo->bezel_info.right = (pTiledDisplayBlock->bezel_info.right *
            pTiledDisplayBlock->bezel_info.pixel_density) / 10;
        pTileTopo->bezel_info.left = (pTiledDisplayBlock->bezel_info.left *
            pTiledDisplayBlock->bezel_info.pixel_density) / 10;

        pTileTopo->tile_topology_id.vendor_id = pTiledDisplayBlock->topo_id.vendor_id[0] << 16 |
            pTiledDisplayBlock->topo_id.vendor_id[1] << 8 |
            pTiledDisplayBlock->topo_id.vendor_id[2];
        pTileTopo->tile_topology_id.product_id = pTiledDisplayBlock->topo_id.product_id[1] << 8 |
            pTiledDisplayBlock->topo_id.product_id[0];
        pTileTopo->tile_topology_id.serial_number = pTiledDisplayBlock->topo_id.serial_number[3] << 24 |
            pTiledDisplayBlock->topo_id.serial_number[2] << 16 |
            pTiledDisplayBlock->topo_id.serial_number[1] << 8 |
            pTiledDisplayBlock->topo_id.serial_number[0];
    }
    else
    {
        return NVT_STATUS_ERR;
    }
    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20ContainerId(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_CONTAINERID *pContainerId)
{
    NVT_STATUS status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_CONTAINERID_BLOCK  *pContainerIdBlock = NULL;

    if ((pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_CONTAINER_ID) &&
        (pDataBlock->data_bytes == DISPLAYID_2_0_CONTAINERID_BLOCK_PAYLOAD_LENGTH))
    {
        pContainerIdBlock = (const DISPLAYID_2_0_CONTAINERID_BLOCK *)pDataBlock;

        pContainerId->revision = pDataBlock->revision;
        pContainerId->data1 = pContainerIdBlock->container_id[0] << 24 |
            pContainerIdBlock->container_id[1] << 16 |
            pContainerIdBlock->container_id[2] << 8 |
            pContainerIdBlock->container_id[3];
        pContainerId->data2 = pContainerIdBlock->container_id[4] << 8 |
            pContainerIdBlock->container_id[5];
        pContainerId->data3 = pContainerIdBlock->container_id[6] << 8 |
            pContainerIdBlock->container_id[7];
        pContainerId->data4 = pContainerIdBlock->container_id[8] << 8 |
            pContainerIdBlock->container_id[9];
        pContainerId->data5[0] = pContainerIdBlock->container_id[10];
        pContainerId->data5[1] = pContainerIdBlock->container_id[11];
        pContainerId->data5[2] = pContainerIdBlock->container_id[12];
        pContainerId->data5[3] = pContainerIdBlock->container_id[13];
        pContainerId->data5[4] = pContainerIdBlock->container_id[14];
        pContainerId->data5[5] = pContainerIdBlock->container_id[15];
    }
    else
    {
        status = NVT_STATUS_ERR;
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20VendorSpecific(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, 
    NVT_DISPLAYID_VENDOR_SPECIFIC *pVendorSpecific)
{
    NVT_STATUS status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_VENDOR_SPECIFIC_BLOCK *block = NULL;
    NvU32 ieee_oui = 0;

    if (pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_VENDOR_SPEC)
    {
        block = (const DISPLAYID_2_0_VENDOR_SPECIFIC_BLOCK*)pDataBlock;
        ieee_oui = (NvU32)((block->vendor_id[0] << 16) | 
                           (block->vendor_id[1] << 8)  | 
                           (block->vendor_id[2])); 

        switch (ieee_oui)
        {
        case NVT_VESA_VENDOR_SPECIFIC_IEEE_ID:
            // TODO: below parser shall be updated if DID21 changed in the future
            if (pDataBlock->data_bytes == NVT_VESA_VENDOR_SPECIFIC_LENGTH)
            {
                pVendorSpecific->vesaVsdb.data_struct_type.type = block->vendor_specific_data[3] & NVT_VESA_ORG_VSDB_DATA_TYPE_MASK;
                pVendorSpecific->vesaVsdb.data_struct_type.color_space_and_eotf = (block->vendor_specific_data[3] & NVT_VESA_ORG_VSDB_COLOR_SPACE_AND_EOTF_MASK) >> NVT_VESA_ORG_VSDB_COLOR_SPACE_AND_EOTF_SHIFT;

                pVendorSpecific->vesaVsdb.overlapping.pixels_overlapping_count = block->vendor_specific_data[4] & NVT_VESA_ORG_VSDB_PIXELS_OVERLAPPING_MASK;
                pVendorSpecific->vesaVsdb.overlapping.multi_sst = (block->vendor_specific_data[4] & NVT_VESA_ORG_VSDB_MULTI_SST_MODE_MASK) >> NVT_VESA_ORG_VSDB_MULTI_SST_MODE_SHIFT;

                pVendorSpecific->vesaVsdb.pass_through_integer.pass_through_integer_dsc = block->vendor_specific_data[5] & NVT_VESA_ORG_VSDB_PASS_THROUGH_INTEGER_MASK;
                pVendorSpecific->vesaVsdb.pass_through_fractional.pass_through_fraction_dsc = block->vendor_specific_data[6] & NVT_VESA_ORG_VSDB_PASS_THROUGH_FRACTIOINAL_MASK;
            }
            else
            {
                status = NVT_STATUS_ERR;
            }
        break;

        default:
        break;
        }
    }

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20CtaData(
    const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock,
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo)
{
    NVT_STATUS status = NVT_STATUS_SUCCESS;
    
    const DISPLAYID_2_0_CTA_BLOCK * ctaBlock = NULL;
    NVT_EDID_CEA861_INFO *p861Info = NULL;
    NvU8 *pcta_data = NULL;

    if (pDataBlock->type == DISPLAYID_2_0_BLOCK_TYPE_CTA_DATA)
    {
        ctaBlock  = (const DISPLAYID_2_0_CTA_BLOCK *)pDataBlock;

        // WAR here to add a (size_t) cast for casting member from const to non-const in order to avoid Linux old compiler failed in DVS.
        pcta_data = (NvU8 *)(size_t)ctaBlock->cta_data;

        status = parseCta861DataBlockInfo(pcta_data, pDataBlock->data_bytes, &pDisplayIdInfo->cta.cta861_info);
        if (status != NVT_STATUS_SUCCESS)
        {
            return status;
        }
        
        p861Info = &pDisplayIdInfo->cta.cta861_info;

        parseCta861VsdbBlocks(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);

        parseCta861HfScdb(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);

        // This CTA 861 function to parse 861 part
        parse861bShortTiming(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);

        // yuv420-only video
        parse861bShortYuv420Timing(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);

        parseCea861HdrStaticMetadataDataBlock(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);

        // CEA861-F at 7.5.12 section about VFPDB block.
        if (p861Info->total_vfpdb != 0)
        {
            parse861bShortPreferredTiming(p861Info, pDisplayIdInfo, FROM_DISPLAYID_20_DATA_BLOCK);
        }

        return status;

    }
    else
    {
        return NVT_STATUS_ERR;
    }
}

// Helper function
CODE_SEGMENT(PAGE_DD_CODE)
static NvU32
greatestCommonDenominator(
    NvU32 x,
    NvU32 y)
{
    NvU32 g = 0;

    while (x > 0)
    {
        g = x;
        x = y % x;
        y = g;
    }
    return g;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS 
getPrimaryUseCase(
    NvU8 product_type, 
    NVT_DISPLAYID_PRODUCT_PRIMARY_USE_CASE *primary_use_case)
{
    NVT_STATUS status = NVT_STATUS_SUCCESS;

    switch (product_type)
    {
    case DISPLAYID_2_0_PROD_TEST:
        *primary_use_case = PRODUCT_PRIMARY_USE_TEST_EQUIPMENT;
        break;
    case DISPLAYID_2_0_PROD_GENERIC_DISPLAY:
        *primary_use_case = PRODUCT_PRIMARY_USE_GENERIC_DISPLAY;
        break;
    case DISPLAYID_2_0_PROD_TELEVISION:
        *primary_use_case = PRODUCT_PRIMARY_USE_TELEVISION;
        break;
    case DISPLAYID_2_0_PROD_DESKTOP_PRODUCTIVITY_DISPLAY:
        *primary_use_case = PRODUCT_PRIMARY_USE_DESKTOP_PRODUCTIVITY;
        break;
    case DISPLAYID_2_0_PROD_DESKTOP_GAMING_DISPLAY:
        *primary_use_case = PRODUCT_PRIMARY_USE_DESKTOP_GAMING;
        break;
    case DISPLAYID_2_0_PROD_PRESENTATION_DISPLAY:
        *primary_use_case = PRODUCT_PRIMARY_USE_PRESENTATION;
        break;
    case DISPLAYID_2_0_PROD_HMD_VR:
        *primary_use_case = PRODUCT_PRIMARY_USE_HEAD_MOUNT_VIRTUAL_REALITY;
        break;
    case DISPLAYID_2_0_PROD_HMD_AR:
        *primary_use_case = PRODUCT_PRIMARY_USE_HEAD_MOUNT_AUGMENTED_REALITY;
        break;
    case DISPLAYID_2_0_PROD_EXTENSION:
        status = NVT_STATUS_ERR;
        break;
    default:
        status = NVT_STATUS_ERR;
    }

    return status;
}

// used in DID20 and DID20ext
CODE_SEGMENT(PAGE_DD_CODE)
NvU8
computeDisplayId20SectionCheckSum(
    const NvU8 *pSectionBytes,
    NvU32 length)
{

    NvU32 i = 0;
    NvU32 checkSum = 0;

    // Each DisplayID section composed of five mandatory bytes:
    // DisplayID Structure Version and Revision
    // Section Size
    // Product Primary Use Case
    // Extension Count
    // Checksum
    for (i = 0, checkSum = 0; i < length; i++)
    {
        checkSum += pSectionBytes[i];
    }

    return (checkSum & 0xFF);
}

CODE_SEGMENT(PAGE_DD_CODE)
NvBool
assignNextAvailableDisplayId20Timing(
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, 
    const NVT_TIMING *pTiming)
{
    if (pDisplayIdInfo->total_timings >= COUNT(pDisplayIdInfo->timing))
    {
        return NV_FALSE;
    }

    pDisplayIdInfo->timing[pDisplayIdInfo->total_timings] = *pTiming;
    pDisplayIdInfo->total_timings++;

    return NV_TRUE;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20Timing7Descriptor(
    const DISPLAYID_2_0_TIMING_7_DESCRIPTOR *pDescriptor,
    NVT_TIMING *pTiming,
    NvU8 revision,
    NvU8 count)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    NvU32       gdc = 0;

    // pclk is in 10Khz
    // pixel_clock is in kHz
    pTiming->pclk = ((pDescriptor->pixel_clock[2] << 16 |
        pDescriptor->pixel_clock[1] << 8 |
        pDescriptor->pixel_clock[0]) + 1) / 10;

    pTiming->HBorder = 0;
    pTiming->VBorder = 0;

    pTiming->HVisible = ((pDescriptor->horizontal.active_image_pixels[1] << 8) |
        (pDescriptor->horizontal.active_image_pixels[0])) + 1;
    pTiming->VVisible = ((pDescriptor->vertical.active_image_lines[1] << 8) |
        (pDescriptor->vertical.active_image_lines[0])) + 1;

    pTiming->HTotal = (((pDescriptor->horizontal.blank_pixels[1] << 8) |
        (pDescriptor->horizontal.blank_pixels[0])) + 1) +
        pTiming->HVisible;
    pTiming->VTotal = (((pDescriptor->vertical.blank_lines[1] << 8) |
        (pDescriptor->vertical.blank_lines[0])) + 1) +
        pTiming->VVisible;

    pTiming->HFrontPorch = ((pDescriptor->horizontal.front_porch_pixels_high << 8) |
        (pDescriptor->horizontal.front_porch_pixels_low)) + 1;
    pTiming->VFrontPorch = ((pDescriptor->vertical.front_porch_lines_high << 8) |
        (pDescriptor->vertical.front_porch_lines_low)) + 1;

    pTiming->HSyncWidth = ((pDescriptor->horizontal.sync_width_pixels[1] << 8) |
        (pDescriptor->horizontal.sync_width_pixels[0])) + 1;
    pTiming->VSyncWidth = ((pDescriptor->vertical.sync_width_lines[1] << 8) |
        (pDescriptor->vertical.sync_width_lines[0])) + 1;

    pTiming->HSyncPol = pDescriptor->horizontal.sync_polarity ? NVT_H_SYNC_POSITIVE :
        NVT_H_SYNC_NEGATIVE;
    pTiming->VSyncPol = pDescriptor->vertical.sync_polarity ? NVT_V_SYNC_POSITIVE :
        NVT_V_SYNC_NEGATIVE;
        
    // EDID used in DP1.4 Compliance test had incorrect HBlank listed, leading to wrong raster sizes being set by driver (bug 2714607)
    // Filter incorrect timings here. HTotal must cover sufficient blanking time
    if (pTiming->HTotal < (pTiming->HVisible + pTiming->HFrontPorch + pTiming->HSyncWidth))
    {
        return NVT_STATUS_ERR;
    }

    pTiming->interlaced = pDescriptor->options.interface_frame_scanning_type;

    switch (pDescriptor->options.aspect_ratio)
    {
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_1_1:
        pTiming->etc.aspect = (1 << 16) | 1;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_5_4:
        pTiming->etc.aspect = (5 << 16) | 4;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_4_3:
        pTiming->etc.aspect = (4 << 16) | 3;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_15_9:
        pTiming->etc.aspect = (15 << 16) | 9;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_16_9:
        pTiming->etc.aspect = (16 << 16) | 9;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_16_10:
        pTiming->etc.aspect = (16 << 16) | 10;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_64_27:
        pTiming->etc.aspect = (64 << 16) | 27;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_256_135:
        pTiming->etc.aspect = (256 << 16) | 135;
        break;
    case DISPLAYID_2_0_TIMING_ASPECT_RATIO_CALCULATE:
        gdc = greatestCommonDenominator(pTiming->HVisible, pTiming->VVisible);
        if (gdc != 0)
        {
            pTiming->etc.aspect = ((pTiming->HVisible / gdc) << 16) |
                (pTiming->VVisible / gdc);
        }
        else
        {
            pTiming->etc.aspect = 0;
        }
        break;
    default:
        pTiming->etc.aspect = 0;
    }

    pTiming->etc.rr = NvTiming_CalcRR(pTiming->pclk,
        pTiming->interlaced,
        pTiming->HTotal,
        pTiming->VTotal);
    pTiming->etc.rrx1k = NvTiming_CalcRRx1k(pTiming->pclk,
        pTiming->interlaced,
        pTiming->HTotal,
        pTiming->VTotal);

    pTiming->etc.flag |= (revision >= DISPLAYID_2_0_TYPE7_DSC_PASSTHRU_REVISION ) ? NVT_FLAG_DISPLAYID_7_DSC_PASSTHRU : 0;
    if (revision >= DISPLAYID_2_0_TYPE7_YCC420_SUPPORT_REVISION)
    {
        pTiming->etc.flag |= pDescriptor->options.is_preferred_or_ycc420 ? NVT_FLAG_DISPLAYID_2_0_EXPLICT_YUV420 : 0;

        if (pDescriptor->options.is_preferred_or_ycc420) // YCC 420 support
        {
            UPDATE_BPC_FOR_COLORFORMAT(pTiming->etc.yuv420, 0, 1, 1, 1, 0, 1);
        }
    }
    else
    {
        pTiming->etc.flag |= pDescriptor->options.is_preferred_or_ycc420 ? NVT_FLAG_DISPLAYID_DTD_PREFERRED_TIMING : 0;
    }

    pTiming->etc.status = NVT_STATUS_DISPLAYID_7N(++count); 

    NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type7:#%2d:%dx%dx%3d.%03dHz/%s", 
                                                                        (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                        (int)pTiming->HVisible, 
                                                                        (int)((pTiming->interlaced ? 2 : 1)*pTiming->VVisible),
                                                                        (int)pTiming->etc.rrx1k/1000,
                                                                        (int)pTiming->etc.rrx1k%1000,
                                                                        (pTiming->interlaced ? "I":"P"));
    pTiming->etc.name[sizeof(pTiming->etc.name) - 1] = '\0';

    pTiming->etc.rep = 0x1;

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20Timing9Descriptor(
    const DISPLAYID_2_0_TIMING_9_DESCRIPTOR *pDescriptor,
    NVT_TIMING *pTiming,
    NvU8 count)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    NvU32       width  = 0;
    NvU32       height = 0;
    NvU32       rr     = 0;

    width  = (pDescriptor->horizontal_active_pixels[1] << 8 | pDescriptor->horizontal_active_pixels[0]) + 1;
    height = (pDescriptor->vertical_active_lines[1] << 8    | pDescriptor->vertical_active_lines[0]) + 1;
    rr     = pDescriptor->refresh_rate + 1;

    switch (pDescriptor->options.timing_formula)
    {
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD:
        status = NvTiming_CalcCVT(width, height, rr, NVT_PROGRESSIVE, pTiming);
    break;        
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_1:
        status = NvTiming_CalcCVT_RB(width, height, rr, NVT_PROGRESSIVE, pTiming);
    break;
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_2:
        status = NvTiming_CalcCVT_RB2(width, height, rr, pDescriptor->options.fractional_refresh_rate_support, pTiming);
    break;
    default:
        status = NVT_STATUS_ERR;
    break;
    }

    if (status == NVT_STATUS_SUCCESS)
    {        
        NVMISC_MEMSET(pTiming->etc.name, 0, sizeof(pTiming->etc.name));
        pTiming->etc.status = NVT_STATUS_DISPLAYID_9N(++count);

        if ( pDescriptor->options.timing_formula== DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type9:#%3d:%dx%dx%3d.%03dHz/%s", 
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible, 
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000, 
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
        else if (pDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_1)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type9-RB1:#%3d:%dx%dx%3d.%03dHz/%s", 
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible,
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000, 
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
        else if (pDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_2)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type9-RB2:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible, 
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000, 
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
    }

    pTiming->etc.name[sizeof(pTiming->etc.name) - 1] = '\0';

    return status;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NVT_STATUS
parseDisplayId20Timing10Descriptor(
    const void *pDescriptor,
    NVT_TIMING *pTiming,
    NvU8 payloadbytes, 
    NvU8 count)
{
    NVT_STATUS  status = NVT_STATUS_SUCCESS;
    const DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR* p6bytesDescriptor = NULL;
    const DISPLAYID_2_0_TIMING_10_7BYTES_DESCRIPTOR* p7bytesDescriptor = NULL;
    NvU32 width  = 0;
    NvU32 height = 0;
    NvU32 rr     = 0;

    p6bytesDescriptor = (const DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR *)pDescriptor;

    width  = (p6bytesDescriptor->horizontal_active_pixels[1] << 8 | p6bytesDescriptor->horizontal_active_pixels[0]) + 1;
    height = (p6bytesDescriptor->vertical_active_lines[1] << 8    | p6bytesDescriptor->vertical_active_lines[0]) + 1;
    rr     = p6bytesDescriptor->refresh_rate + 1;

    if (payloadbytes == DISPLAYID_2_0_TIMING_10_PAYLOAD_BYTES_7)
    {
        p7bytesDescriptor = (const DISPLAYID_2_0_TIMING_10_7BYTES_DESCRIPTOR *)pDescriptor;
                       rr = (p7bytesDescriptor->descriptor_6_bytes.refresh_rate | p7bytesDescriptor->refresh_rate_high << 8) + 1;
    }

    switch (p6bytesDescriptor->options.timing_formula)
    {
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD:
        status = NvTiming_CalcCVT(width, height, rr, NVT_PROGRESSIVE, pTiming);
    break;        
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_1:
        status = NvTiming_CalcCVT_RB(width, height, rr, NVT_PROGRESSIVE, pTiming);
    break;
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_2:
        status = NvTiming_CalcCVT_RB2(width, height, rr, p6bytesDescriptor->options.vrr_or_hblank, pTiming);
    break;
    case DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_3:
        {
            /* Will uncomment this if we used the new RB3 interface.
            NvU32 hBlankValue = 0;

            if (p6bytesDescriptor->options.vrr_or_hblank == 0)     // Horizontal Blank in Pixels = [Field Value] * 8 + 80
            {
                hBlankValue = p7bytesDescriptor->delta_hblank * 8 + 80;
            }
            else if (p6bytesDescriptor->options.vrr_or_hblank == 1)
            {
                if (p7bytesDescriptor->delta_hblank <= 5)
                    hBlankValue = p7bytesDescriptor->delta_hblank * 8 + 160;
                else // if 5 < Field Value <=7
                    hBlankValue = 160 - (p7bytesDescriptor->delta_hblank * 8);
            }
            */

            // TODO : Need to handle  7:5 bit at 6 byte -Additional Vertical Blank Time in % of frame time for the defined Refresh Rate
            //status = NvTiming_CalcCVT_RB3(width, height, rr, hBlankValue, pTiming);
            break;
        }
    }

    if ( status == NVT_STATUS_SUCCESS )
    {
        NVMISC_MEMSET(pTiming->etc.name, 0, sizeof(pTiming->etc.name));
        pTiming->etc.status = NVT_STATUS_DISPLAYID_10N(++count);

        if (p6bytesDescriptor->options.ycc420_support)
        {
            // YCC 420 support
            UPDATE_BPC_FOR_COLORFORMAT(pTiming->etc.yuv420, 0, 1, 1, 1, 0, 1);
        }

        if (p6bytesDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type10:#%3d:%dx%dx%3d.%03dHz/%s", 
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible, 
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000, 
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));

        }
        else if (p6bytesDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_1)
        {
             NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type10RB1:#%3d:%dx%dx%3d.%03dHz/%s",
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible, 
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000, 
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
        else if (p6bytesDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_2)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type10RB2:#%3d:%dx%dx%3d.%03dHz/%s", 
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible,
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000,
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
        else if (p6bytesDescriptor->options.timing_formula == DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_3)
        {
            NVT_SNPRINTF((char *)pTiming->etc.name, sizeof(pTiming->etc.name), "DID20-Type10RB3:#%3d:%dx%dx%3d.%03dHz/%s", 
                                                                                (int)NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status),
                                                                                (int)pTiming->HVisible,
                                                                                (int)pTiming->VVisible,
                                                                                (int)pTiming->etc.rrx1k/1000,
                                                                                (int)pTiming->etc.rrx1k%1000, 
                                                                                (pTiming->interlaced ? "I":"P"));
        }
    }

    pTiming->etc.name[sizeof(pTiming->etc.name) - 1] = '\0';

    return status;
}

// get the existed stored timing sequence number
CODE_SEGMENT(PAGE_DD_CODE)
static NvU8 
getExistedTimingSeqNumber(
    NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, 
    enum NVT_TIMING_TYPE timingType)
{
    NvU8 count = 0;
    NvU8 i     = 0;

    switch (timingType)
    {
    case NVT_TYPE_DISPLAYID_7:               
    case NVT_TYPE_DISPLAYID_8:
    case NVT_TYPE_DISPLAYID_9:
    case NVT_TYPE_DISPLAYID_10:
        break;
    default:
        return count;
    }

    for (i = 0; i< pDisplayIdInfo->total_timings; i++)
    {
        if (NVT_GET_TIMING_STATUS_TYPE(pDisplayIdInfo->timing[i].etc.status) == timingType)
        {
            ++count;
        }
    }

    return count;
}

// get the version
CODE_SEGMENT(PAGE_DD_CODE)
NvU32 getDID2Version(NvU8 *pData, NvU32 *pVer)
{
    const DISPLAYID_2_0_SECTION *pSection = (const DISPLAYID_2_0_SECTION*)pData;

    *pVer = 0;
    if (pSection->header.version == 0x2)
    {
        *pVer = (((NvU32)pSection->header.version) << 8) + ((NvU32)pSection->header.revision);
    }
    else
        return NVT_STATUS_ERR;          // un-recongnized DisplayID20 version

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
void 
updateColorFormatForDisplayId20Timings(
    NVT_DISPLAYID_2_0_INFO *pDisplayId20Info, 
    NvU32 timingIdx)
{
    // pDisplayId20Info parsed displayID20 info    
    NVT_TIMING *pT= &pDisplayId20Info->timing[timingIdx];
    
    nvt_assert(timingIdx <= COUNT(pDisplayId20Info->timing));
  
    // rgb444 (always support 6bpc and 8bpc as per DP spec 5.1.1.1.1 RGB Colorimetry)
    UPDATE_BPC_FOR_COLORFORMAT(pT->etc.rgb444, 1,
                                               1,
                                               pDisplayId20Info->interface_features.rgb444.bpc.bpc10,
                                               pDisplayId20Info->interface_features.rgb444.bpc.bpc12,
                                               pDisplayId20Info->interface_features.rgb444.bpc.bpc14,
                                               pDisplayId20Info->interface_features.rgb444.bpc.bpc16);

    // yuv444
    UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv444, 0, // yuv444 does not support 6bpc 
                                               pDisplayId20Info->interface_features.yuv444.bpc.bpc8,
                                               pDisplayId20Info->interface_features.yuv444.bpc.bpc10,
                                               pDisplayId20Info->interface_features.yuv444.bpc.bpc12,
                                               pDisplayId20Info->interface_features.yuv444.bpc.bpc14,
                                               pDisplayId20Info->interface_features.yuv444.bpc.bpc16);
    // yuv422
    UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv422, 0, // yuv422 does not support 6bpc 
                                               pDisplayId20Info->interface_features.yuv422.bpc.bpc8,
                                               pDisplayId20Info->interface_features.yuv422.bpc.bpc10,
                                               pDisplayId20Info->interface_features.yuv422.bpc.bpc12,
                                               pDisplayId20Info->interface_features.yuv422.bpc.bpc14,
                                               pDisplayId20Info->interface_features.yuv422.bpc.bpc16);

    if (!NVT_DID20_TIMING_IS_CTA861(pT->etc.flag, pT->etc.status))
    {
        // yuv420
        UPDATE_BPC_FOR_COLORFORMAT(pT->etc.yuv420, 0,  // yuv420 does not support 6bpc 
                                                   pDisplayId20Info->interface_features.yuv420.bpc.bpc8,
                                                   pDisplayId20Info->interface_features.yuv420.bpc.bpc10,
                                                   pDisplayId20Info->interface_features.yuv420.bpc.bpc12,
                                                   pDisplayId20Info->interface_features.yuv420.bpc.bpc14,
                                                   pDisplayId20Info->interface_features.yuv420.bpc.bpc16);
    }
}
POP_SEGMENTS

