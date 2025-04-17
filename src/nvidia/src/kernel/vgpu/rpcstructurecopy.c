/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Description:
//       Collection of functions to handle RPC structurecopy
//
//******************************************************************************

#include "vgpu/rpc.h"
#include "g_rpcstructurecopy_private.h"
#include "vgpu/sdk-structures.h"

#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

NV_STATUS deserialize_NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS_v15_03(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *fbLtcInfoForFbp, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{

    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    *offset += sizeof(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS_v15_03);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_FB_GET_LTC_INFO_FOR_FBP_v1A_0D(VGPU_FB_GET_LTC_INFO_FOR_FBP *vgpuFbGetLtcInfoForFbp, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{

    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuFbGetLtcInfoForFbp and buffer are valid, then copy data and return the offset
    if (vgpuFbGetLtcInfoForFbp && buffer)
    {
        VGPU_FB_GET_LTC_INFO_FOR_FBP_v1A_0D *vgpu_fb_get_ltc_info_for_fbp_v1A_0D = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_FB_GET_LTC_INFO_FOR_FBP_v1A_0D))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_fb_get_ltc_info_for_fbp_v1A_0D = (void*)(buffer + *offset);

        for (i = 0; i < MAX_FBPS; i++)
        {
            (*vgpuFbGetLtcInfoForFbp)[i].fbpIndex = vgpu_fb_get_ltc_info_for_fbp_v1A_0D->fbLtcInfoForFbp[i].fbpIndex;
            (*vgpuFbGetLtcInfoForFbp)[i].ltcMask  = vgpu_fb_get_ltc_info_for_fbp_v1A_0D->fbLtcInfoForFbp[i].ltcMask;
            (*vgpuFbGetLtcInfoForFbp)[i].ltcCount = vgpu_fb_get_ltc_info_for_fbp_v1A_0D->fbLtcInfoForFbp[i].ltcCount;
        }
    }

    *offset += sizeof(VGPU_FB_GET_LTC_INFO_FOR_FBP_v1A_0D);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_v16_00(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *mcEngineNotificationIntrVectors, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If mcEngineNotificationIntrVectors and buffer are valid, then copy data and return the offset
    if (mcEngineNotificationIntrVectors && buffer)
    {
        NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_v16_00 *mc_get_engine_notification_intr_vectors_v16_00 = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_v16_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        mc_get_engine_notification_intr_vectors_v16_00 = (void*)(buffer + *offset);

        mcEngineNotificationIntrVectors->numEntries = mc_get_engine_notification_intr_vectors_v16_00->numEntries;

        for (i = 0; i < mcEngineNotificationIntrVectors->numEntries; i++)
        {
            mcEngineNotificationIntrVectors->entries[i].nv2080EngineType       = mc_get_engine_notification_intr_vectors_v16_00->entries[i].nv2080EngineType;
            mcEngineNotificationIntrVectors->entries[i].notificationIntrVector = mc_get_engine_notification_intr_vectors_v16_00->entries[i].notificationIntrVector;
        }

    }

    *offset += sizeof(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_v16_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_v1E_09(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *mcStaticIntrTable, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If mcStaticIntrs and buffer are valid, then copy data and return the offset
    if (mcStaticIntrTable && buffer)
    {
        NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_v1E_09 *mc_get_static_intr_table_v1E_09 = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_v1E_09))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        mc_get_static_intr_table_v1E_09 = (void*)(buffer + *offset);

        mcStaticIntrTable->numEntries = mc_get_static_intr_table_v1E_09->numEntries;

        for (i = 0; i < mcStaticIntrTable->numEntries; i++)
        {
            mcStaticIntrTable->entries[i].nv2080IntrType = mc_get_static_intr_table_v1E_09->entries[i].nv2080IntrType;
            mcStaticIntrTable->entries[i].pmcIntrMask = mc_get_static_intr_table_v1E_09->entries[i].pmcIntrMask;
            mcStaticIntrTable->entries[i].intrVectorStall = mc_get_static_intr_table_v1E_09->entries[i].intrVectorStall;
            mcStaticIntrTable->entries[i].intrVectorNonStall = mc_get_static_intr_table_v1E_09->entries[i].intrVectorNonStall;
        }

    }

    *offset += sizeof(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_v1E_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_FIFO_GET_DEVICE_INFO_TABLE_v1A_07(VGPU_FIFO_GET_DEVICE_INFO_TABLE *fifoDeviceInfoTablePtr, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If fifoDeviceInfoTablePtr and buffer are valid, then copy data and return the offset
    if (fifoDeviceInfoTablePtr && buffer)
    {
        NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS *fifoDeviceInfoTable = *fifoDeviceInfoTablePtr;
        VGPU_FIFO_GET_DEVICE_INFO_TABLE_v1A_07 *vgpu_fifo_get_device_info_table_v1A_07 = NULL;
        NvU32 i, j;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_FIFO_GET_DEVICE_INFO_TABLE_v1A_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_fifo_get_device_info_table_v1A_07 = (void*)(buffer + *offset);

        for (i = 0; i < MAX_ITERATIONS_DEVICE_INFO_TABLE; i++)
        {
            fifoDeviceInfoTable[i].baseIndex  = vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].baseIndex;
            fifoDeviceInfoTable[i].numEntries = vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].numEntries;
            fifoDeviceInfoTable[i].bMore      = vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].bMore;

             for (j = 0; j < NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES; j++)
             {
                 portMemCopy(&fifoDeviceInfoTable[i].entries[j].engineData,
                             sizeof(fifoDeviceInfoTable[i].entries[j].engineData),
                             &vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].entries[j].engineData,
                             sizeof(fifoDeviceInfoTable[i].entries[j].engineData));
                 portMemCopy(&fifoDeviceInfoTable[i].entries[j].pbdmaIds,
                             sizeof(fifoDeviceInfoTable[i].entries[j].pbdmaIds),
                             &vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].entries[j].pbdmaIds,
                             sizeof(fifoDeviceInfoTable[i].entries[j].pbdmaIds));
                 portMemCopy(&fifoDeviceInfoTable[i].entries[j].pbdmaFaultIds,
                             sizeof(fifoDeviceInfoTable[i].entries[j].pbdmaFaultIds),
                             &vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].entries[j].pbdmaFaultIds,
                             sizeof(fifoDeviceInfoTable[i].entries[j].pbdmaFaultIds));
                 portMemCopy(&fifoDeviceInfoTable[i].entries[j].engineName,
                             sizeof(fifoDeviceInfoTable[i].entries[j].engineName),
                             &vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].entries[j].engineName,
                             sizeof(fifoDeviceInfoTable[i].entries[j].engineName));
                 fifoDeviceInfoTable[i].entries[j].numPbdmas = vgpu_fifo_get_device_info_table_v1A_07->fifoDeviceInfoTable[i].entries[j].numPbdmas;
             }
        }
    }

    *offset += sizeof(VGPU_FIFO_GET_DEVICE_INFO_TABLE_v1A_07);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES_v1A_07(VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES *fbDynamicBlacklistedPagesPtr, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If fbDynamicBlacklistedPagesPtr and buffer are valid, then copy data and return the offset
    if (fbDynamicBlacklistedPagesPtr && buffer)
    {
        NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *fbDynamicBlacklistedPages = *fbDynamicBlacklistedPagesPtr;
        VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES_v1A_07 *vgpu_fb_get_dynamic_blacklisted_pages_v1A_07 = NULL;
        NvU32 i, j;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES_v1A_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_fb_get_dynamic_blacklisted_pages_v1A_07 = (void*)(buffer + *offset);

        for (i = 0; i < MAX_ITERATIONS_DYNAMIC_BLACKLIST; i++)
        {
            fbDynamicBlacklistedPages[i].baseIndex    = vgpu_fb_get_dynamic_blacklisted_pages_v1A_07->fbDynamicBlacklistedPages[i].baseIndex;
            fbDynamicBlacklistedPages[i].validEntries = vgpu_fb_get_dynamic_blacklisted_pages_v1A_07->fbDynamicBlacklistedPages[i].validEntries;
            fbDynamicBlacklistedPages[i].bMore        = vgpu_fb_get_dynamic_blacklisted_pages_v1A_07->fbDynamicBlacklistedPages[i].bMore;

            for (j = 0; j < NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES; j++)
            {
                fbDynamicBlacklistedPages[i].offlined[j].pageNumber = vgpu_fb_get_dynamic_blacklisted_pages_v1A_07->fbDynamicBlacklistedPages[i].blackList[j].pageNumber;
                fbDynamicBlacklistedPages[i].offlined[j].source     = vgpu_fb_get_dynamic_blacklisted_pages_v1A_07->fbDynamicBlacklistedPages[i].blackList[j].source;
            }
        }
    }

    *offset += sizeof(VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES_v1A_07);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_v1C_09(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *busGetInfoV2, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If busGetInfoV2 and buffer are valid, then copy data and return the offset
    if (busGetInfoV2 && buffer)
    {
        NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_v1C_09 *bus_get_info_v2_v1C_09 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_v1C_09))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        bus_get_info_v2_v1C_09 = (void*)(buffer + *offset);

        busGetInfoV2->busInfoListSize = bus_get_info_v2_v1C_09->busInfoListSize;

        if (bus_get_info_v2_v1C_09->busInfoListSize > NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE_v1C_09)
        {
            return NV_ERR_OUT_OF_RANGE;
        }

        for (i = 0; i < bus_get_info_v2_v1C_09->busInfoListSize; i++)
        {
            busGetInfoV2->busInfoList[i].index = bus_get_info_v2_v1C_09->busInfoList[i].index;
            busGetInfoV2->busInfoList[i].data  = bus_get_info_v2_v1C_09->busInfoList[i].data;
        }
    }

    *offset += sizeof(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_v1C_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1A_1F(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *grSmIssueRateModifier, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If pVSI and buffer are valid, then copy data and return the offset
    if (grSmIssueRateModifier && buffer)
    {
        NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1A_1F *gr_get_sm_issue_rate_modifier_v1A_1F = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1A_1F))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gr_get_sm_issue_rate_modifier_v1A_1F = (void*)(buffer + *offset);

        grSmIssueRateModifier->grRouteInfo.flags     = gr_get_sm_issue_rate_modifier_v1A_1F->grRouteInfo.flags;
        grSmIssueRateModifier->grRouteInfo.route     = gr_get_sm_issue_rate_modifier_v1A_1F->grRouteInfo.route;

        grSmIssueRateModifier->imla0     = gr_get_sm_issue_rate_modifier_v1A_1F->imla0;
        grSmIssueRateModifier->fmla16    = gr_get_sm_issue_rate_modifier_v1A_1F->fmla16;
        grSmIssueRateModifier->dp        = gr_get_sm_issue_rate_modifier_v1A_1F->dp;
        grSmIssueRateModifier->fmla32    = gr_get_sm_issue_rate_modifier_v1A_1F->fmla32;
        grSmIssueRateModifier->ffma      = gr_get_sm_issue_rate_modifier_v1A_1F->ffma;
        grSmIssueRateModifier->imla1     = gr_get_sm_issue_rate_modifier_v1A_1F->imla1;
        grSmIssueRateModifier->imla2     = gr_get_sm_issue_rate_modifier_v1A_1F->imla2;
        grSmIssueRateModifier->imla3     = gr_get_sm_issue_rate_modifier_v1A_1F->imla3;
        grSmIssueRateModifier->imla4     = gr_get_sm_issue_rate_modifier_v1A_1F->imla4;
    }

    *offset += sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1A_1F);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *grSmIssueRateModifierV2, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If pVSI and buffer are valid, then copy data and return the offset
    if (grSmIssueRateModifierV2 && buffer)
    {
        NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09 *gr_get_sm_issue_rate_modifier_v2B_09 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gr_get_sm_issue_rate_modifier_v2B_09 = (void*)(buffer + *offset);

        grSmIssueRateModifierV2->smIssueRateModifierListSize = gr_get_sm_issue_rate_modifier_v2B_09->smIssueRateModifierListSize;

        if (gr_get_sm_issue_rate_modifier_v2B_09->smIssueRateModifierListSize >= NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE_v2B_09)
        {
            return NV_ERR_OUT_OF_RANGE;
        }

        for (i = 0; i < gr_get_sm_issue_rate_modifier_v2B_09->smIssueRateModifierListSize; i++)
        {
            grSmIssueRateModifierV2->smIssueRateModifierList[i].index = gr_get_sm_issue_rate_modifier_v2B_09->smIssueRateModifierList[i].index;
            grSmIssueRateModifierV2->smIssueRateModifierList[i].data  = gr_get_sm_issue_rate_modifier_v2B_09->smIssueRateModifierList[i].data;
        }
    }

    *offset += sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v24_07(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *grInfoParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (grInfoParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v24_07 *gr_info_v24_07 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v24_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gr_info_v24_07 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j < NV0080_CTRL_GR_INFO_MAX_SIZE_24_07; j++)
            {
                grInfoParams->engineInfo[i].infoList[j].index = gr_info_v24_07->engineInfo[i].infoList[j].index;
                grInfoParams->engineInfo[i].infoList[j].data = gr_info_v24_07->engineInfo[i].infoList[j].data;
            }
        }

    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v24_07);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v29_00(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS *grInfoParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (grInfoParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v29_00 *gr_info_v29_00 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v29_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gr_info_v29_00 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j < NV0080_CTRL_GR_INFO_MAX_SIZE_29_00; j++)
            {
                grInfoParams->engineInfo[i].infoList[j].index = gr_info_v29_00->engineInfo[i].infoList[j].index;
                grInfoParams->engineInfo[i].infoList[j].data = gr_info_v29_00->engineInfo[i].infoList[j].data;
            }
        }

    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS_v29_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v1F_01(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *smOrderParams, NvU8 *buffer,
                                                                                       NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (smOrderParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v1F_01 *sm_order_v1F_01 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v1F_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        sm_order_v1F_01 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j <NV2080_CTRL_INTERNAL_GR_MAX_SM_v1E_03; j++)
            {
                smOrderParams->globalSmOrder[i].globalSmId[j].gpcId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].gpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].localTpcId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].localTpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].localSmId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].localSmId;
                smOrderParams->globalSmOrder[i].globalSmId[j].globalTpcId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].globalTpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].virtualGpcId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].virtualGpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].migratableTpcId = sm_order_v1F_01->globalSmOrder[i].globalSmId[j].migratableTpcId;
            }

            smOrderParams->globalSmOrder[i].numSm = sm_order_v1F_01->globalSmOrder[i].numSm;
            smOrderParams->globalSmOrder[i].numTpc = sm_order_v1F_01->globalSmOrder[i].numTpc;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v1F_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v2A_02(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS *smOrderParams, NvU8 *buffer,
                                                                                       NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (smOrderParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v2A_02 *sm_order_v2A_02 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v2A_02))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        sm_order_v2A_02 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j <NV2080_CTRL_INTERNAL_GR_MAX_SM_v1E_03; j++)
            {
                smOrderParams->globalSmOrder[i].globalSmId[j].gpcId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].gpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].localTpcId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].localTpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].localSmId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].localSmId;
                smOrderParams->globalSmOrder[i].globalSmId[j].globalTpcId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].globalTpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].virtualGpcId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].virtualGpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].migratableTpcId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].migratableTpcId;
                smOrderParams->globalSmOrder[i].globalSmId[j].ugpuId = sm_order_v2A_02->globalSmOrder[i].globalSmId[j].ugpuId;
            }

            smOrderParams->globalSmOrder[i].numSm = sm_order_v2A_02->globalSmOrder[i].numSm;
            smOrderParams->globalSmOrder[i].numTpc = sm_order_v2A_02->globalSmOrder[i].numTpc;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS_v2A_02);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS_v1B_05(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS *ropInfoParams, NvU8 *buffer,
                                                                                NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (ropInfoParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS_v1B_05 *rop_info_v1B_05 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS_v1B_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        rop_info_v1B_05 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            ropInfoParams->engineRopInfo[i].ropUnitCount = rop_info_v1B_05->engineRopInfo[i].ropUnitCount;
            ropInfoParams->engineRopInfo[i].ropOperationsFactor = rop_info_v1B_05->engineRopInfo[i].ropOperationsFactor;
            ropInfoParams->engineRopInfo[i].ropOperationsCount = rop_info_v1B_05->engineRopInfo[i].ropOperationsCount;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS_v1B_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS_v1C_06(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS *ppcMaskParams, NvU8 *buffer,
                                                                                 NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (ppcMaskParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS_v1C_06 *ppc_mask_v1C_06 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS_v1C_06))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        ppc_mask_v1C_06 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j < NV2080_CTRL_INTERNAL_GR_MAX_GPC_v1C_03; j++)
            {
                ppcMaskParams->enginePpcMasks[i].mask[j] = ppc_mask_v1C_06->enginePpcMasks[i].mask[j];
            }
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS_v1C_06);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS_v25_07(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS* ctxBuffInfo, NvU8* buffer,
    NvU32 bufferSize, NvU32* offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (ctxBuffInfo && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS_v25_07* ctx_buff_info_v25_07 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS_v25_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        ctx_buff_info_v25_07 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j < NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT_v25_07; j++)
            {
                ctxBuffInfo->engineContextBuffersInfo[i].engine[j].size = ctx_buff_info_v25_07->engineContextBuffersInfo[i].engine[j].size;
                ctxBuffInfo->engineContextBuffersInfo[i].engine[j].alignment = ctx_buff_info_v25_07->engineContextBuffersInfo[i].engine[j].alignment;
            }
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS_v25_07);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_v25_0B
(
    NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS* pParams,
    NvU8 *buffer,
    NvU32 bufferSize,
    NvU32 *offset
)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if ((pParams != NULL) && (buffer != NULL))
    {
        NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_v25_0B *pParams_v25_0B = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_v25_0B))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v25_0B = (void*)(buffer + *offset);

        pParams->atomicsCaps = pParams_v25_0B->atomicsCaps;
    }

    *offset += sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_v25_0B);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1B_05(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS *smIssueRateModifier,NvU8 *buffer,
                                                                                            NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (smIssueRateModifier && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1B_05 *rate_modifier_v1B_05 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1B_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        rate_modifier_v1B_05 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            smIssueRateModifier->smIssueRateModifier[i].imla0 = rate_modifier_v1B_05->smIssueRateModifier[i].imla0;
            smIssueRateModifier->smIssueRateModifier[i].imla1 = rate_modifier_v1B_05->smIssueRateModifier[i].imla1;
            smIssueRateModifier->smIssueRateModifier[i].imla2 = rate_modifier_v1B_05->smIssueRateModifier[i].imla2;
            smIssueRateModifier->smIssueRateModifier[i].imla3 = rate_modifier_v1B_05->smIssueRateModifier[i].imla3;
            smIssueRateModifier->smIssueRateModifier[i].imla4 = rate_modifier_v1B_05->smIssueRateModifier[i].imla4;
            smIssueRateModifier->smIssueRateModifier[i].fmla16 = rate_modifier_v1B_05->smIssueRateModifier[i].fmla16;
            smIssueRateModifier->smIssueRateModifier[i].fmla32 = rate_modifier_v1B_05->smIssueRateModifier[i].fmla32;
            smIssueRateModifier->smIssueRateModifier[i].ffma = rate_modifier_v1B_05->smIssueRateModifier[i].ffma;
            smIssueRateModifier->smIssueRateModifier[i].dp = rate_modifier_v1B_05->smIssueRateModifier[i].dp;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS_v1B_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS *smIssueRateModifierV2, NvU8 *buffer,
                                                                                                 NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (smIssueRateModifierV2 && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09 *rate_modifier_v2B_09 = NULL;
        NvU32 i = 0;
        NvU32 j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        rate_modifier_v2B_09 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            smIssueRateModifierV2->smIssueRateModifierV2[i].smIssueRateModifierListSize = rate_modifier_v2B_09->smIssueRateModifierV2[i].smIssueRateModifierListSize;

            if (rate_modifier_v2B_09->smIssueRateModifierV2[i].smIssueRateModifierListSize >= NV2080_CTRL_GR_SM_ISSUE_RATE_MODIFIER_V2_MAX_LIST_SIZE_v2B_09)
            {
                return NV_ERR_OUT_OF_RANGE;
            }

            for (j = 0; j < rate_modifier_v2B_09->smIssueRateModifierV2[i].smIssueRateModifierListSize; j++)
            {
                smIssueRateModifierV2->smIssueRateModifierV2[i].smIssueRateModifierList[j].index = rate_modifier_v2B_09->smIssueRateModifierV2[i].smIssueRateModifierList[j].index;
                smIssueRateModifierV2->smIssueRateModifierV2[i].smIssueRateModifierList[j].data  = rate_modifier_v2B_09->smIssueRateModifierV2[i].smIssueRateModifierList[j].data;
            }
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_V2_PARAMS_v2B_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS_v1D_03(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS *floorsweepMaskParams, NvU8 *buffer,
                                                                                              NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (floorsweepMaskParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS_v1D_03 *floorsweep_mask_params_v1D_03 = NULL;
        NvU32 i = 0, j = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS_v1D_03))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        floorsweep_mask_params_v1D_03 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            for (j = 0; j < NV2080_CTRL_INTERNAL_GR_MAX_GPC_v1C_03; j++)
            {
                floorsweepMaskParams->floorsweepingMasks[i].tpcMask[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].tpcMask[j];
                floorsweepMaskParams->floorsweepingMasks[i].tpcCount[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].tpcCount[j];
                floorsweepMaskParams->floorsweepingMasks[i].numPesPerGpc[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].numPesPerGpc[j];
                floorsweepMaskParams->floorsweepingMasks[i].mmuPerGpc[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].mmuPerGpc[j];
                floorsweepMaskParams->floorsweepingMasks[i].zcullMask[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].zcullMask[j];
            }

            floorsweepMaskParams->floorsweepingMasks[i].gpcMask = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].gpcMask;
            floorsweepMaskParams->floorsweepingMasks[i].physGpcMask = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].physGpcMask;

            for (j = 0; j < NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT_v1C_03; j++)
            {
                floorsweepMaskParams->floorsweepingMasks[i].tpcToPesMap[j] = floorsweep_mask_params_v1D_03->floorsweepingMasks[i].tpcToPesMap[j];
            }
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS_v1D_03);

    return NVOS_STATUS_SUCCESS;

}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS_v1B_05(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS *zcullInfoParams, NvU8 *buffer,
                                                                                  NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (zcullInfoParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS_v1B_05 *zcull_info_v1B_05 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS_v1B_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        zcull_info_v1B_05 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            zcullInfoParams->engineZcullInfo[i].widthAlignPixels = zcull_info_v1B_05->engineZcullInfo[i].widthAlignPixels;
            zcullInfoParams->engineZcullInfo[i].heightAlignPixels = zcull_info_v1B_05->engineZcullInfo[i].heightAlignPixels;
            zcullInfoParams->engineZcullInfo[i].pixelSquaresByAliquots = zcull_info_v1B_05->engineZcullInfo[i].pixelSquaresByAliquots;
            zcullInfoParams->engineZcullInfo[i].aliquotTotal = zcull_info_v1B_05->engineZcullInfo[i].aliquotTotal;
            zcullInfoParams->engineZcullInfo[i].zcullRegionByteMultiplier = zcull_info_v1B_05->engineZcullInfo[i].zcullRegionByteMultiplier;
            zcullInfoParams->engineZcullInfo[i].zcullRegionHeaderSize = zcull_info_v1B_05->engineZcullInfo[i].zcullRegionHeaderSize;
            zcullInfoParams->engineZcullInfo[i].zcullSubregionHeaderSize = zcull_info_v1B_05->engineZcullInfo[i].zcullSubregionHeaderSize;
            zcullInfoParams->engineZcullInfo[i].subregionCount = zcull_info_v1B_05->engineZcullInfo[i].subregionCount;
            zcullInfoParams->engineZcullInfo[i].subregionWidthAlignPixels = zcull_info_v1B_05->engineZcullInfo[i].subregionWidthAlignPixels;
            zcullInfoParams->engineZcullInfo[i].subregionHeightAlignPixels = zcull_info_v1B_05->engineZcullInfo[i].subregionHeightAlignPixels;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS_v1B_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS_v1B_05(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS *fecsRecordSize, NvU8 *buffer,
                                                                                  NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (fecsRecordSize && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS_v1B_05 *fecs_record_size_v1B_05 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS_v1B_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        fecs_record_size_v1B_05 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            fecsRecordSize->fecsRecordSize[i].fecsRecordSize = fecs_record_size_v1B_05->fecsRecordSize[i].fecsRecordSize;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS_v1B_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS_v1D_04(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS *fecsTraceDefines, NvU8 *buffer,
                                                                                  NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (fecsTraceDefines && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS_v1D_04 *fecs_trace_defines_v1D_04 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS_v1D_04))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        fecs_trace_defines_v1D_04 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            fecsTraceDefines->fecsTraceDefines[i].fecsRecordSize = fecs_trace_defines_v1D_04->fecsTraceDefines[i].fecsRecordSize;
            fecsTraceDefines->fecsTraceDefines[i].timestampHiTagMask = fecs_trace_defines_v1D_04->fecsTraceDefines[i].timestampHiTagMask;
            fecsTraceDefines->fecsTraceDefines[i].timestampHiTagShift = fecs_trace_defines_v1D_04->fecsTraceDefines[i].timestampHiTagShift;
            fecsTraceDefines->fecsTraceDefines[i].timestampVMask = fecs_trace_defines_v1D_04->fecsTraceDefines[i].timestampVMask;
            fecsTraceDefines->fecsTraceDefines[i].numLowerBitsZeroShift = fecs_trace_defines_v1D_04->fecsTraceDefines[i].numLowerBitsZeroShift;
        }
    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS_v1D_04);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS_v1E_02(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS *grPdbPropertiesParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (grPdbPropertiesParams && buffer)
    {
        NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS_v1E_02 *gr_pdb_properties_v1E_02 = NULL;
        NvU32 i = 0;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS_v1E_02))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gr_pdb_properties_v1E_02 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04; i++)
        {
            grPdbPropertiesParams->pdbTable[i].bPerSubCtxheaderSupported = gr_pdb_properties_v1E_02->pdbTable[i].bPerSubCtxheaderSupported;
        }

    }

    *offset += sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS_v1E_02);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_DATA_v25_0E(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If pVSI and buffer are valid, then copy data and return the offset
    if (pVSI && buffer)
    {
        VGPU_STATIC_DATA_v25_0E *vgpu_static_data_v25_0E = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_DATA_v25_0E))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_static_data_v25_0E = (void*)(buffer + *offset);

        pVSI->fbTaxLength            = vgpu_static_data_v25_0E->fbTaxLength;
        pVSI->fbLength               = vgpu_static_data_v25_0E->fbLength;
        pVSI->fbBusWidth             = vgpu_static_data_v25_0E->fbBusWidth;
        pVSI->fbioMask               = vgpu_static_data_v25_0E->fbioMask;
        pVSI->fbpMask                = vgpu_static_data_v25_0E->fbpMask;
        pVSI->ltcMask                = vgpu_static_data_v25_0E->ltcMask;
        pVSI->ltsCount               = vgpu_static_data_v25_0E->ltsCount;
        pVSI->subProcessIsolation    = vgpu_static_data_v25_0E->subProcessIsolation;
        pVSI->engineList[0]          = vgpu_static_data_v25_0E->engineList;
        pVSI->sizeL2Cache            = vgpu_static_data_v25_0E->sizeL2Cache;
        pVSI->poisonFuseEnabled      = vgpu_static_data_v25_0E->poisonFuseEnabled;
        pVSI->guestManagedHwAlloc    = vgpu_static_data_v25_0E->guestManagedHwAlloc;

        // gpuname
        portMemCopy(pVSI->adapterName, sizeof(pVSI->adapterName),
                    vgpu_static_data_v25_0E->gpuName.adapterName, sizeof(pVSI->adapterName));
        portMemCopy(pVSI->adapterName_Unicode, sizeof(pVSI->adapterName_Unicode),
                    vgpu_static_data_v25_0E->gpuName.adapterName_Unicode, sizeof(pVSI->adapterName_Unicode));
        portMemCopy(pVSI->shortGpuNameString, sizeof(pVSI->shortGpuNameString),
                    vgpu_static_data_v25_0E->gpuName.shortGpuNameString, sizeof(pVSI->adapterName));

        pVSI->bSplitVasBetweenServerClientRm = vgpu_static_data_v25_0E->bSplitVasBetweenServerClientRm;
        pVSI->maxSupportedPageSize           = vgpu_static_data_v25_0E->maxSupportedPageSize;
        pVSI->bFlaSupported                  = vgpu_static_data_v25_0E->bFlaSupported;
        pVSI->bPerRunlistChannelRamEnabled   = vgpu_static_data_v25_0E->bPerRunlistChannelRamEnabled;
        pVSI->bAtsSupported                  = vgpu_static_data_v25_0E->bAtsSupported;

        pVSI->bPerSubCtxheaderSupported      = vgpu_static_data_v25_0E->bPerSubCtxheaderSupported;
        pVSI->bC2CLinkUp                     = vgpu_static_data_v25_0E->bC2CLinkUp;
        pVSI->bLocalEgmEnabled               = vgpu_static_data_v25_0E->bLocalEgmEnabled;
        pVSI->localEgmPeerId                 = vgpu_static_data_v25_0E->localEgmPeerId;
        pVSI->bSelfHostedMode                = vgpu_static_data_v25_0E->bSelfHostedMode;

        pVSI->ceFaultMethodBufferDepth       = vgpu_static_data_v25_0E->ceFaultMethodBufferDepth;
        pVSI->pcieGpuLinkCaps                = vgpu_static_data_v25_0E->pcieGpuLinkCaps;

        portMemCopy(pVSI->grCapsBits, sizeof(pVSI->grCapsBits),
                    vgpu_static_data_v25_0E->grCapsBits, sizeof(vgpu_static_data_v25_0E->grCapsBits));

        // Removed grBufferSize, gfxpBufferSize and gfxpBufferAlignment from VGPU_STATIC_INFO

        portMemCopy(pVSI->jpegCaps, sizeof(pVSI->jpegCaps),
                    vgpu_static_data_v25_0E->jpegCaps, sizeof(vgpu_static_data_v25_0E->jpegCaps));
    }

    *offset += sizeof(VGPU_STATIC_DATA_v25_0E);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_DATA_v27_00(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If pVSI and buffer are valid, then copy data and return the offset
    if (pVSI && buffer)
    {
        VGPU_STATIC_DATA_v27_00 *vgpu_static_data_v27_00 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_DATA_v27_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_static_data_v27_00 = (void*)(buffer + *offset);

        pVSI->fbTaxLength            = vgpu_static_data_v27_00->fbTaxLength;
        pVSI->fbLength               = vgpu_static_data_v27_00->fbLength;
        pVSI->fbBusWidth             = vgpu_static_data_v27_00->fbBusWidth;
        pVSI->fbioMask               = vgpu_static_data_v27_00->fbioMask;
        pVSI->fbpMask                = vgpu_static_data_v27_00->fbpMask;
        pVSI->ltcMask                = vgpu_static_data_v27_00->ltcMask;
        pVSI->ltsCount               = vgpu_static_data_v27_00->ltsCount;
        pVSI->subProcessIsolation    = vgpu_static_data_v27_00->subProcessIsolation;
        pVSI->engineList[0]          = vgpu_static_data_v27_00->engineList;
        pVSI->sizeL2Cache            = vgpu_static_data_v27_00->sizeL2Cache;
        pVSI->poisonFuseEnabled      = vgpu_static_data_v27_00->poisonFuseEnabled;
        pVSI->guestManagedHwAlloc    = vgpu_static_data_v27_00->guestManagedHwAlloc;

        // gpuname
        portMemCopy(pVSI->adapterName, sizeof(pVSI->adapterName),
                    vgpu_static_data_v27_00->gpuName.adapterName, sizeof(pVSI->adapterName));
        portMemCopy(pVSI->adapterName_Unicode, sizeof(pVSI->adapterName_Unicode),
                    vgpu_static_data_v27_00->gpuName.adapterName_Unicode, sizeof(pVSI->adapterName_Unicode));
        portMemCopy(pVSI->shortGpuNameString, sizeof(pVSI->shortGpuNameString),
                    vgpu_static_data_v27_00->gpuName.shortGpuNameString, sizeof(pVSI->adapterName));

        pVSI->bSplitVasBetweenServerClientRm = vgpu_static_data_v27_00->bSplitVasBetweenServerClientRm;
        pVSI->maxSupportedPageSize           = vgpu_static_data_v27_00->maxSupportedPageSize;
        pVSI->bFlaSupported                  = vgpu_static_data_v27_00->bFlaSupported;
        pVSI->bPerRunlistChannelRamEnabled   = vgpu_static_data_v27_00->bPerRunlistChannelRamEnabled;
        pVSI->bAtsSupported                  = vgpu_static_data_v27_00->bAtsSupported;

        pVSI->bPerSubCtxheaderSupported      = vgpu_static_data_v27_00->bPerSubCtxheaderSupported;
        pVSI->bC2CLinkUp                     = vgpu_static_data_v27_00->bC2CLinkUp;
        pVSI->bLocalEgmEnabled               = vgpu_static_data_v27_00->bLocalEgmEnabled;
        pVSI->localEgmPeerId                 = vgpu_static_data_v27_00->localEgmPeerId;
        pVSI->bSelfHostedMode                = vgpu_static_data_v27_00->bSelfHostedMode;

        pVSI->ceFaultMethodBufferDepth       = vgpu_static_data_v27_00->ceFaultMethodBufferDepth;
        pVSI->pcieGpuLinkCaps                = vgpu_static_data_v27_00->pcieGpuLinkCaps;

        portMemCopy(pVSI->grCapsBits, sizeof(pVSI->grCapsBits),
                    vgpu_static_data_v27_00->grCapsBits, sizeof(vgpu_static_data_v27_00->grCapsBits));

        // Removed grBufferSize, gfxpBufferSize and gfxpBufferAlignment from VGPU_STATIC_INFO

        portMemCopy(pVSI->jpegCaps, sizeof(pVSI->jpegCaps),
                    vgpu_static_data_v27_00->jpegCaps, sizeof(vgpu_static_data_v27_00->jpegCaps));
    }

    *offset += sizeof(VGPU_STATIC_DATA_v27_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_DATA_v27_01(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }
    // If pVSI and buffer are valid, then copy data and return the offset
    if (pVSI && buffer)
    {
        VGPU_STATIC_DATA_v27_01 *vgpu_static_data_v27_01 = NULL;
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_DATA_v27_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }
        vgpu_static_data_v27_01 = (void*)(buffer + *offset);
        pVSI->fbTaxLength            = vgpu_static_data_v27_01->fbTaxLength;
        pVSI->fbLength               = vgpu_static_data_v27_01->fbLength;
        pVSI->fbBusWidth             = vgpu_static_data_v27_01->fbBusWidth;
        pVSI->fbioMask               = vgpu_static_data_v27_01->fbioMask;
        pVSI->fbpMask                = vgpu_static_data_v27_01->fbpMask;
        pVSI->ltcMask                = vgpu_static_data_v27_01->ltcMask;
        pVSI->ltsCount               = vgpu_static_data_v27_01->ltsCount;
        pVSI->subProcessIsolation    = vgpu_static_data_v27_01->subProcessIsolation;
        pVSI->sizeL2Cache            = vgpu_static_data_v27_01->sizeL2Cache;
        pVSI->poisonFuseEnabled      = vgpu_static_data_v27_01->poisonFuseEnabled;
        pVSI->guestManagedHwAlloc    = vgpu_static_data_v27_01->guestManagedHwAlloc;

        // gpuname
        portMemCopy(pVSI->adapterName, sizeof(pVSI->adapterName),
                    vgpu_static_data_v27_01->gpuName.adapterName, sizeof(pVSI->adapterName));
        portMemCopy(pVSI->adapterName_Unicode, sizeof(pVSI->adapterName_Unicode),
                    vgpu_static_data_v27_01->gpuName.adapterName_Unicode, sizeof(pVSI->adapterName_Unicode));
        portMemCopy(pVSI->shortGpuNameString, sizeof(pVSI->shortGpuNameString),
                    vgpu_static_data_v27_01->gpuName.shortGpuNameString, sizeof(pVSI->adapterName));
        pVSI->bSplitVasBetweenServerClientRm = vgpu_static_data_v27_01->bSplitVasBetweenServerClientRm;
        pVSI->maxSupportedPageSize           = vgpu_static_data_v27_01->maxSupportedPageSize;
        pVSI->bFlaSupported                  = vgpu_static_data_v27_01->bFlaSupported;
        pVSI->bPerRunlistChannelRamEnabled   = vgpu_static_data_v27_01->bPerRunlistChannelRamEnabled;
        pVSI->bAtsSupported                  = vgpu_static_data_v27_01->bAtsSupported;
        pVSI->bPerSubCtxheaderSupported      = vgpu_static_data_v27_01->bPerSubCtxheaderSupported;
        pVSI->bC2CLinkUp                     = vgpu_static_data_v27_01->bC2CLinkUp;
        pVSI->bLocalEgmEnabled               = vgpu_static_data_v27_01->bLocalEgmEnabled;
        pVSI->localEgmPeerId                 = vgpu_static_data_v27_01->localEgmPeerId;
        pVSI->bSelfHostedMode                = vgpu_static_data_v27_01->bSelfHostedMode;
        pVSI->ceFaultMethodBufferDepth       = vgpu_static_data_v27_01->ceFaultMethodBufferDepth;
        pVSI->pcieGpuLinkCaps                = vgpu_static_data_v27_01->pcieGpuLinkCaps;
        portMemCopy(pVSI->grCapsBits, sizeof(pVSI->grCapsBits),
                    vgpu_static_data_v27_01->grCapsBits, sizeof(vgpu_static_data_v27_01->grCapsBits));

        // Removed grBufferSize, gfxpBufferSize and gfxpBufferAlignment from VGPU_STATIC_INFO

        for (i = 0; i < NVGPU_VGPU_ENGINE_LIST_MASK_ARRAY_MAX_v27_01; i++)
        {
            pVSI->engineList[i] = vgpu_static_data_v27_01->engineList[i];
        }
        portMemCopy(pVSI->jpegCaps, sizeof(pVSI->jpegCaps),
                    vgpu_static_data_v27_01->jpegCaps, sizeof(vgpu_static_data_v27_01->jpegCaps));
    }
    *offset += sizeof(VGPU_STATIC_DATA_v27_01);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_DATA_v2A_07(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }
    // If pVSI and buffer are valid, then copy data and return the offset
    if (pVSI && buffer)
    {
        VGPU_STATIC_DATA_v2A_07 *vgpu_static_data_v2A_07 = NULL;
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_DATA_v2A_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }
        vgpu_static_data_v2A_07 = (void*)(buffer + *offset);
        pVSI->fbTaxLength            = vgpu_static_data_v2A_07->fbTaxLength;
        pVSI->fbLength               = vgpu_static_data_v2A_07->fbLength;
        pVSI->fbBusWidth             = vgpu_static_data_v2A_07->fbBusWidth;
        pVSI->fbioMask               = vgpu_static_data_v2A_07->fbioMask;
        pVSI->fbpMask                = vgpu_static_data_v2A_07->fbpMask;
        pVSI->ltcMask                = vgpu_static_data_v2A_07->ltcMask;
        pVSI->ltsCount               = vgpu_static_data_v2A_07->ltsCount;
        pVSI->subProcessIsolation    = vgpu_static_data_v2A_07->subProcessIsolation;
        pVSI->sizeL2Cache            = vgpu_static_data_v2A_07->sizeL2Cache;
        pVSI->poisonFuseEnabled      = vgpu_static_data_v2A_07->poisonFuseEnabled;
        pVSI->guestManagedHwAlloc    = vgpu_static_data_v2A_07->guestManagedHwAlloc;

        // gpuname
        portMemCopy(pVSI->adapterName, sizeof(pVSI->adapterName),
                    vgpu_static_data_v2A_07->gpuName.adapterName, sizeof(pVSI->adapterName));
        portMemCopy(pVSI->adapterName_Unicode, sizeof(pVSI->adapterName_Unicode),
                    vgpu_static_data_v2A_07->gpuName.adapterName_Unicode, sizeof(pVSI->adapterName_Unicode));
        portMemCopy(pVSI->shortGpuNameString, sizeof(pVSI->shortGpuNameString),
                    vgpu_static_data_v2A_07->gpuName.shortGpuNameString, sizeof(pVSI->adapterName));
        pVSI->bSplitVasBetweenServerClientRm = vgpu_static_data_v2A_07->bSplitVasBetweenServerClientRm;
        pVSI->maxSupportedPageSize           = vgpu_static_data_v2A_07->maxSupportedPageSize;
        pVSI->bFlaSupported                  = vgpu_static_data_v2A_07->bFlaSupported;
        pVSI->bPerRunlistChannelRamEnabled   = vgpu_static_data_v2A_07->bPerRunlistChannelRamEnabled;
        pVSI->bAtsSupported                  = vgpu_static_data_v2A_07->bAtsSupported;
        pVSI->bPerSubCtxheaderSupported      = vgpu_static_data_v2A_07->bPerSubCtxheaderSupported;
        pVSI->bC2CLinkUp                     = vgpu_static_data_v2A_07->bC2CLinkUp;
        pVSI->bLocalEgmEnabled               = vgpu_static_data_v2A_07->bLocalEgmEnabled;
        pVSI->localEgmPeerId                 = vgpu_static_data_v2A_07->localEgmPeerId;
        pVSI->bSelfHostedMode                = vgpu_static_data_v2A_07->bSelfHostedMode;
        pVSI->ceFaultMethodBufferDepth       = vgpu_static_data_v2A_07->ceFaultMethodBufferDepth;
        pVSI->pcieGpuLinkCaps                = vgpu_static_data_v2A_07->pcieGpuLinkCaps;
        portMemCopy(pVSI->grCapsBits, sizeof(pVSI->grCapsBits),
                    vgpu_static_data_v2A_07->grCapsBits, sizeof(vgpu_static_data_v2A_07->grCapsBits));

        for (i = 0; i < NVGPU_VGPU_ENGINE_LIST_MASK_ARRAY_MAX_v27_01; i++)
        {
            pVSI->engineList[i] = vgpu_static_data_v2A_07->engineList[i];
        }
        portMemCopy(pVSI->jpegCaps, sizeof(pVSI->jpegCaps),
                    vgpu_static_data_v2A_07->jpegCaps, sizeof(vgpu_static_data_v2A_07->jpegCaps));
    }
    *offset += sizeof(VGPU_STATIC_DATA_v2A_07);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_v03_00(NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS *vgxSystemInfo,
                                                                            NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgxSystemInfo and buffer are valid, then copy data and return the offset
    if (vgxSystemInfo && buffer)
    {
        NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_v03_00 *vgx_system_info_v03_00 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_v03_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgx_system_info_v03_00 = (void*)(buffer + *offset);

        portMemCopy(vgxSystemInfo->szHostDriverVersionBuffer,
                    sizeof(vgxSystemInfo->szHostDriverVersionBuffer),
                    vgx_system_info_v03_00->szHostDriverVersionBuffer,
                    sizeof(vgx_system_info_v03_00->szHostDriverVersionBuffer));
        portMemCopy(vgxSystemInfo->szHostVersionBuffer,
                    sizeof(vgxSystemInfo->szHostVersionBuffer),
                    vgx_system_info_v03_00->szHostVersionBuffer,
                    sizeof(vgx_system_info_v03_00->szHostVersionBuffer));
        portMemCopy(vgxSystemInfo->szHostTitleBuffer,
                    sizeof(vgxSystemInfo->szHostTitleBuffer),
                    vgx_system_info_v03_00->szHostTitleBuffer,
                    sizeof(vgx_system_info_v03_00->szHostTitleBuffer));

        portMemCopy(vgxSystemInfo->szPluginTitleBuffer,
                    sizeof(vgxSystemInfo->szPluginTitleBuffer),
                    vgx_system_info_v03_00->szPluginTitleBuffer,
                    sizeof(vgx_system_info_v03_00->szPluginTitleBuffer));

        portMemCopy(vgxSystemInfo->szHostUnameBuffer,
                    sizeof(vgxSystemInfo->szHostUnameBuffer),
                    vgx_system_info_v03_00->szHostUnameBuffer,
                    sizeof(vgx_system_info_v03_00->szHostUnameBuffer));

        vgxSystemInfo->iHostChangelistNumber      = vgx_system_info_v03_00->iHostChangelistNumber;
        vgxSystemInfo->iPluginChangelistNumber    = vgx_system_info_v03_00->iPluginChangelistNumber;
    }

    *offset += sizeof(NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS_v03_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_v03_00(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *gidInfo,
                                                                 NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If gidInfo and buffer are valid, then copy data and return the offset
    if (gidInfo && buffer)
    {
        NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_v03_00 *gid_info_v03_00 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_v03_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gid_info_v03_00 = (void*)(buffer + *offset);

        gidInfo->index  = gid_info_v03_00->index;
        gidInfo->flags  = gid_info_v03_00->flags;
        gidInfo->length = gid_info_v03_00->length;
        portMemCopy(gidInfo->data, sizeof(gidInfo->data), gid_info_v03_00->data, sizeof(gid_info_v03_00->data));
    }

    *offset += sizeof(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS_v03_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_v03_00(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *skuInfo,
                                                                  NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_v25_0E(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS *skuInfo,
                                                                  NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If skuInfo and buffer are valid, then copy data and return the offset
    if (skuInfo && buffer)
    {
        NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_v25_0E *sku_info_v25_0E = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_v25_0E))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        sku_info_v25_0E = (void*)(buffer + *offset);

        skuInfo->BoardID = sku_info_v25_0E->BoardID;
        skuInfo->skuConfigVersion = sku_info_v25_0E->skuConfigVersion;
        portMemCopy(skuInfo->chipSKU, sizeof(skuInfo->chipSKU),
                    sku_info_v25_0E->chipSKU, sizeof(sku_info_v25_0E->chipSKU));
        portMemCopy(skuInfo->chipSKUMod, sizeof(skuInfo->chipSKUMod),
                    sku_info_v25_0E->chipSKUMod, sizeof(sku_info_v25_0E->chipSKUMod));
        portMemCopy(skuInfo->project, sizeof(skuInfo->project),
                    sku_info_v25_0E->project, sizeof(sku_info_v25_0E->project));
        portMemCopy(skuInfo->projectSKU, sizeof(skuInfo->projectSKU),
                    sku_info_v25_0E->projectSKU, sizeof(sku_info_v25_0E->projectSKU));
        portMemCopy(skuInfo->CDP, sizeof(skuInfo->CDP),
                    sku_info_v25_0E->CDP, sizeof(sku_info_v25_0E->CDP));
        portMemCopy(skuInfo->projectSKUMod, sizeof(skuInfo->projectSKUMod),
                    sku_info_v25_0E->projectSKUMod, sizeof(sku_info_v25_0E->projectSKUMod));
        skuInfo->businessCycle = sku_info_v25_0E->businessCycle;
    }

    *offset += sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_v25_0E);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_v03_00(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *fbRegionInfoParams,
                                                                          NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If fbRegionInfoParams and buffer are valid, then copy data and return the offset
    if (fbRegionInfoParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_v03_00 *fbRegionInfoParams_v03_00 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_v03_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        fbRegionInfoParams_v03_00 = (void*)(buffer + *offset);

        fbRegionInfoParams->numFBRegions = fbRegionInfoParams_v03_00->numFBRegions;

        for(i = 0; i < NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES; i++)
        {
            fbRegionInfoParams->fbRegion[i].base              = fbRegionInfoParams_v03_00->fbRegion[i].base;
            fbRegionInfoParams->fbRegion[i].limit             = fbRegionInfoParams_v03_00->fbRegion[i].limit;
            fbRegionInfoParams->fbRegion[i].reserved          = fbRegionInfoParams_v03_00->fbRegion[i].reserved;
            fbRegionInfoParams->fbRegion[i].performance       = fbRegionInfoParams_v03_00->fbRegion[i].performance;
            fbRegionInfoParams->fbRegion[i].supportCompressed = fbRegionInfoParams_v03_00->fbRegion[i].supportCompressed;
            fbRegionInfoParams->fbRegion[i].supportISO        = fbRegionInfoParams_v03_00->fbRegion[i].supportISO;
            fbRegionInfoParams->fbRegion[i].bProtected        = fbRegionInfoParams_v03_00->fbRegion[i].bProtected;
            portMemCopy(&fbRegionInfoParams->fbRegion[i].blackList,
                        sizeof(fbRegionInfoParams->fbRegion[i].blackList),
                        &fbRegionInfoParams_v03_00->fbRegion[i].blackList,
                        sizeof(fbRegionInfoParams_v03_00->fbRegion[i].blackList));
        }
    }

    *offset += sizeof(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_v03_00);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_v20_04(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS *ciProfiles,
                                                                         NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If ciProfiles and buffer are valid, then copy data and return the offset
    if (ciProfiles && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_v20_04 *ciProfiles_v20_04 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_v20_04))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        ciProfiles_v20_04 = (void*)(buffer + *offset);

        NV_ASSERT(ciProfiles_v20_04->profileCount <= NV_ARRAY_ELEMENTS(ciProfiles->profiles));

        ciProfiles->profileCount = ciProfiles_v20_04->profileCount;
        for (i = 0; i < ciProfiles->profileCount; i++)
        {
            ciProfiles->profiles[i].gfxGpcCount = ciProfiles_v20_04->profiles[i].gfxGpcCount;
            ciProfiles->profiles[i].computeSize = ciProfiles_v20_04->profiles[i].computeSize;
            ciProfiles->profiles[i].gpcCount    = ciProfiles_v20_04->profiles[i].gpcCount;
            ciProfiles->profiles[i].veidCount   = ciProfiles_v20_04->profiles[i].veidCount;
            ciProfiles->profiles[i].smCount     = ciProfiles_v20_04->profiles[i].smCount;
        }
    }

    *offset += sizeof(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS_v20_04);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v18_07(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *execPartitionInfo,
                                                                    NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v21_04(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *execPartitionInfo,
                                                                    NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v21_09(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *execPartitionInfo,
                                                                    NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v24_05(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *execPartitionInfo,
                                                                    NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If execPartitionInfo and buffer are valid, then copy data and return the offset
    if (execPartitionInfo && buffer)
    {
        NvU32 i;
        NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v24_05 *execPartitionInfo_v24_05 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v24_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        execPartitionInfo_v24_05 = (void*)(buffer + *offset);

        execPartitionInfo->execPartCount    = execPartitionInfo_v24_05->execPartCount;

        for (i = 0; i < execPartitionInfo->execPartCount; i++)
        {
            NVC637_CTRL_EXEC_PARTITIONS_INFO *dst = &(execPartitionInfo->execPartInfo[i]);

            execPartitionInfo->execPartId[i] = execPartitionInfo_v24_05->execPartId[i];

            dst->gpcCount       = execPartitionInfo_v24_05->execPartInfo[i].gpcCount;
            dst->gfxGpcCount    = execPartitionInfo_v24_05->execPartInfo[i].gfxGpcCount;
            dst->veidCount      = execPartitionInfo_v24_05->execPartInfo[i].veidCount;
            dst->ceCount        = execPartitionInfo_v24_05->execPartInfo[i].ceCount;
            dst->nvEncCount     = execPartitionInfo_v24_05->execPartInfo[i].nvEncCount;
            dst->nvDecCount     = execPartitionInfo_v24_05->execPartInfo[i].nvDecCount;
            dst->nvJpgCount     = execPartitionInfo_v24_05->execPartInfo[i].nvJpgCount;
            dst->ofaCount       = execPartitionInfo_v24_05->execPartInfo[i].ofaCount;
            dst->sharedEngFlag  = execPartitionInfo_v24_05->execPartInfo[i].sharedEngFlag;
            dst->smCount        = execPartitionInfo_v24_05->execPartInfo[i].smCount;
            dst->spanStart      = execPartitionInfo_v24_05->execPartInfo[i].spanStart;
            dst->computeSize    = execPartitionInfo_v24_05->execPartInfo[i].computeSize;
        }
    }

    *offset += sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_v24_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_v1A_07(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *zbcTableSizes,
                                                                         NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If zbcTableSizes and buffer are valid, then copy data and return the offset
    if (zbcTableSizes && buffer)
    {
        NvU32 i;
        NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_v1A_07 *zbcTableSizes_v1A_07 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_v1A_07))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        zbcTableSizes_v1A_07 = (void*)(buffer + *offset);

        for (i = 0; i < NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COUNT_v1A_07; i++) {
            zbcTableSizes[i].indexStart = zbcTableSizes_v1A_07[i].indexStart;
            zbcTableSizes[i].indexEnd   = zbcTableSizes_v1A_07[i].indexEnd;
        }
    }

    *offset += sizeof(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_v1A_07) * NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COUNT_v1A_07;

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v12_01(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v18_03(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v20_01(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v21_09(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v24_05(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If gpuPartitionInfo and buffer are valid, then copy data and return the offset
    if (gpuPartitionInfo && buffer)
    {
        NvU32 i;
        GPU_PARTITION_INFO_v24_05 *gpu_partition_info_v24_05 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(GPU_PARTITION_INFO_v24_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gpu_partition_info_v24_05 = (void*)(buffer + *offset);

        // GPU PARTITION INFO
        gpuPartitionInfo->swizzId         = gpu_partition_info_v24_05->swizzId;
        gpuPartitionInfo->grEngCount      = gpu_partition_info_v24_05->grEngCount;
        gpuPartitionInfo->veidCount       = gpu_partition_info_v24_05->veidCount;
        gpuPartitionInfo->ceCount         = gpu_partition_info_v24_05->ceCount;
        gpuPartitionInfo->gpcCount        = gpu_partition_info_v24_05->gpcCount;
        gpuPartitionInfo->virtualGpcCount = gpu_partition_info_v24_05->virtualGpcCount;
        gpuPartitionInfo->gfxGpcCount     = gpu_partition_info_v24_05->gfxGpcCount;
        gpuPartitionInfo->nvDecCount      = gpu_partition_info_v24_05->nvDecCount;
        gpuPartitionInfo->nvEncCount      = gpu_partition_info_v24_05->nvEncCount;
        gpuPartitionInfo->nvJpgCount      = gpu_partition_info_v24_05->nvJpgCount;
        gpuPartitionInfo->partitionFlag   = gpu_partition_info_v24_05->partitionFlag;
        gpuPartitionInfo->smCount         = gpu_partition_info_v24_05->smCount;
        gpuPartitionInfo->nvOfaCount      = gpu_partition_info_v24_05->nvOfaCount;
        gpuPartitionInfo->memSize         = gpu_partition_info_v24_05->memSize;
        gpuPartitionInfo->bValid          = gpu_partition_info_v24_05->bValid;
        gpuPartitionInfo->span.lo         = gpu_partition_info_v24_05->span.lo;
        gpuPartitionInfo->span.hi         = gpu_partition_info_v24_05->span.hi;
        gpuPartitionInfo->validCTSIdMask  = gpu_partition_info_v24_05->validCTSIdMask;

        for (i = 0; i < gpuPartitionInfo->grEngCount; i++)
        {
            gpuPartitionInfo->gpcsPerGr[i]        = gpu_partition_info_v24_05->gpcsPerGr[i];
            gpuPartitionInfo->veidsPerGr[i]       = gpu_partition_info_v24_05->veidsPerGr[i];
            gpuPartitionInfo->virtualGpcsPerGr[i] = gpu_partition_info_v24_05->virtualGpcsPerGr[i];
            gpuPartitionInfo->gfxGpcPerGr[i]      = gpu_partition_info_v24_05->gfxGpcPerGr[i];
        }
    }
    *offset += sizeof(GPU_PARTITION_INFO_v24_05);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_GPU_PARTITION_INFO_v28_02(GPU_PARTITION_INFO *gpuPartitionInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If gpuPartitionInfo and buffer are valid, then copy data and return the offset
    if (gpuPartitionInfo && buffer)
    {
        NvU32 i;
        GPU_PARTITION_INFO_v28_02 *gpu_partition_info_v28_02 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(GPU_PARTITION_INFO_v28_02))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        gpu_partition_info_v28_02 = (void*)(buffer + *offset);

        // GPU PARTITION INFO
        gpuPartitionInfo->swizzId         = gpu_partition_info_v28_02->swizzId;
        gpuPartitionInfo->grEngCount      = gpu_partition_info_v28_02->grEngCount;
        gpuPartitionInfo->veidCount       = gpu_partition_info_v28_02->veidCount;
        gpuPartitionInfo->ceCount         = gpu_partition_info_v28_02->ceCount;
        gpuPartitionInfo->gpcCount        = gpu_partition_info_v28_02->gpcCount;
        gpuPartitionInfo->virtualGpcCount = gpu_partition_info_v28_02->virtualGpcCount;
        gpuPartitionInfo->gfxGpcCount     = gpu_partition_info_v28_02->gfxGpcCount;
        gpuPartitionInfo->nvDecCount      = gpu_partition_info_v28_02->nvDecCount;
        gpuPartitionInfo->nvEncCount      = gpu_partition_info_v28_02->nvEncCount;
        gpuPartitionInfo->nvJpgCount      = gpu_partition_info_v28_02->nvJpgCount;
        gpuPartitionInfo->partitionFlag   = gpu_partition_info_v28_02->partitionFlag;
        gpuPartitionInfo->smCount         = gpu_partition_info_v28_02->smCount;
        gpuPartitionInfo->nvOfaCount      = gpu_partition_info_v28_02->nvOfaCount;
        gpuPartitionInfo->memSize         = gpu_partition_info_v28_02->memSize;
        gpuPartitionInfo->bValid          = gpu_partition_info_v28_02->bValid;
        gpuPartitionInfo->span.lo         = gpu_partition_info_v28_02->span.lo;
        gpuPartitionInfo->span.hi         = gpu_partition_info_v28_02->span.hi;
        gpuPartitionInfo->validCTSIdMask  = gpu_partition_info_v28_02->validCTSIdMask;
        gpuPartitionInfo->validGfxCTSIdMask  = gpu_partition_info_v28_02->validGfxCTSIdMask;

        for (i = 0; i < gpuPartitionInfo->grEngCount; i++)
        {
            gpuPartitionInfo->gpcsPerGr[i]        = gpu_partition_info_v28_02->gpcsPerGr[i];
            gpuPartitionInfo->veidsPerGr[i]       = gpu_partition_info_v28_02->veidsPerGr[i];
            gpuPartitionInfo->virtualGpcsPerGr[i] = gpu_partition_info_v28_02->virtualGpcsPerGr[i];
            gpuPartitionInfo->gfxGpcPerGr[i]      = gpu_partition_info_v28_02->gfxGpcPerGr[i];
        }
    }
    *offset += sizeof(GPU_PARTITION_INFO_v28_02);

    return NVOS_STATUS_SUCCESS;
}

// NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS deserialization is used starting with v24_06
NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v15_01(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v1A_04(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v1C_09(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v20_03(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v24_06(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If eccStatusParams and buffer are valid, then copy data and return the offset
    if (eccStatusParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v24_06 *eccStatusParams_v24_06 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v24_06))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        eccStatusParams_v24_06 = (void*)(buffer + *offset);

        eccStatusParams->bFatalPoisonError = eccStatusParams_v24_06->bFatalPoisonError;

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT_v24_06; i++) {
            eccStatusParams->units[i].enabled                = eccStatusParams_v24_06->units[i].enabled;
            eccStatusParams->units[i].scrubComplete          = eccStatusParams_v24_06->units[i].scrubComplete;
            eccStatusParams->units[i].supported              = eccStatusParams_v24_06->units[i].supported;
            eccStatusParams->units[i].dbe.count              = eccStatusParams_v24_06->units[i].dbe.count;
            eccStatusParams->units[i].dbeNonResettable.count = eccStatusParams_v24_06->units[i].dbeNonResettable.count;
            eccStatusParams->units[i].sbe.count              = eccStatusParams_v24_06->units[i].sbe.count;
            eccStatusParams->units[i].sbeNonResettable.count = eccStatusParams_v24_06->units[i].sbeNonResettable.count;
        }
    }
    *offset += sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v24_06);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v26_02(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If eccStatusParams and buffer are valid, then copy data and return the offset
    if (eccStatusParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v26_02 *eccStatusParams_v26_02 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v26_02))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        eccStatusParams_v26_02 = (void*)(buffer + *offset);

        eccStatusParams->bFatalPoisonError = eccStatusParams_v26_02->bFatalPoisonError;

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT_v26_02; i++) {
            eccStatusParams->units[i].enabled                = eccStatusParams_v26_02->units[i].enabled;
            eccStatusParams->units[i].scrubComplete          = eccStatusParams_v26_02->units[i].scrubComplete;
            eccStatusParams->units[i].supported              = eccStatusParams_v26_02->units[i].supported;
            eccStatusParams->units[i].dbe.count              = eccStatusParams_v26_02->units[i].dbe.count;
            eccStatusParams->units[i].dbeNonResettable.count = eccStatusParams_v26_02->units[i].dbeNonResettable.count;
            eccStatusParams->units[i].sbe.count              = eccStatusParams_v26_02->units[i].sbe.count;
            eccStatusParams->units[i].sbeNonResettable.count = eccStatusParams_v26_02->units[i].sbeNonResettable.count;
        }
    }
    *offset += sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v26_02);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v27_04(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If eccStatusParams and buffer are valid, then copy data and return the offset
    if (eccStatusParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v27_04 *eccStatusParams_v27_04 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v27_04))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        eccStatusParams_v27_04 = (void*)(buffer + *offset);

        eccStatusParams->bFatalPoisonError = eccStatusParams_v27_04->bFatalPoisonError;

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT_v27_04; i++) {
            eccStatusParams->units[i].enabled                = eccStatusParams_v27_04->units[i].enabled;
            eccStatusParams->units[i].scrubComplete          = eccStatusParams_v27_04->units[i].scrubComplete;
            eccStatusParams->units[i].supported              = eccStatusParams_v27_04->units[i].supported;
            eccStatusParams->units[i].dbe.count              = eccStatusParams_v27_04->units[i].dbe.count;
            eccStatusParams->units[i].dbeNonResettable.count = eccStatusParams_v27_04->units[i].dbeNonResettable.count;
            eccStatusParams->units[i].sbe.count              = eccStatusParams_v27_04->units[i].sbe.count;
            eccStatusParams->units[i].sbeNonResettable.count = eccStatusParams_v27_04->units[i].sbeNonResettable.count;
        }
    }
    *offset += sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v27_04);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_01(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If eccStatusParams and buffer are valid, then copy data and return the offset
    if (eccStatusParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_01 *eccStatusParams_v28_01 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        eccStatusParams_v28_01 = (void*)(buffer + *offset);

        eccStatusParams->bFatalPoisonError = eccStatusParams_v28_01->bFatalPoisonError;

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT_v28_01; i++) {
            eccStatusParams->units[i].enabled                = eccStatusParams_v28_01->units[i].enabled;
            eccStatusParams->units[i].scrubComplete          = eccStatusParams_v28_01->units[i].scrubComplete;
            eccStatusParams->units[i].supported              = eccStatusParams_v28_01->units[i].supported;
            eccStatusParams->units[i].dbe.count              = eccStatusParams_v28_01->units[i].dbe.count;
            eccStatusParams->units[i].dbeNonResettable.count = eccStatusParams_v28_01->units[i].dbeNonResettable.count;
            eccStatusParams->units[i].sbe.count              = eccStatusParams_v28_01->units[i].sbe.count;
            eccStatusParams->units[i].sbeNonResettable.count = eccStatusParams_v28_01->units[i].sbeNonResettable.count;
        }
    }
    *offset += sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_08(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS *eccStatusParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If eccStatusParams and buffer are valid, then copy data and return the offset
    if (eccStatusParams && buffer)
    {
        NvU32 i;
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_08 *eccStatusParams_v28_08 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_08))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        eccStatusParams_v28_08 = (void*)(buffer + *offset);

        eccStatusParams->bFatalPoisonError = eccStatusParams_v28_08->bFatalPoisonError;

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT_v28_08; i++) {
            eccStatusParams->units[i].enabled                = eccStatusParams_v28_08->units[i].enabled;
            eccStatusParams->units[i].scrubComplete          = eccStatusParams_v28_08->units[i].scrubComplete;
            eccStatusParams->units[i].supported              = eccStatusParams_v28_08->units[i].supported;
            eccStatusParams->units[i].dbe.count              = eccStatusParams_v28_08->units[i].dbe.count;
            eccStatusParams->units[i].dbeNonResettable.count = eccStatusParams_v28_08->units[i].dbeNonResettable.count;
            eccStatusParams->units[i].sbe.count              = eccStatusParams_v28_08->units[i].sbe.count;
            eccStatusParams->units[i].sbeNonResettable.count = eccStatusParams_v28_08->units[i].sbeNonResettable.count;
        }
    }
    *offset += sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS_v28_08);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_GPU_EXEC_SYSPIPE_INFO_v26_01(GPU_EXEC_SYSPIPE_INFO *execSyspipeInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If execSyspipeInfo and buffer are valid, then copy data and return the offset
    if (execSyspipeInfo && buffer)
    {
        NvU32 i;
        GPU_EXEC_SYSPIPE_INFO_v26_01 *execSyspipeInfo_v26_01 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(GPU_EXEC_SYSPIPE_INFO_v26_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        execSyspipeInfo_v26_01 = (void*)(buffer + *offset);

        execSyspipeInfo->execPartCount    = execSyspipeInfo_v26_01->execPartCount;

        for (i = 0; i < execSyspipeInfo_v26_01->execPartCount; i++)
        {
            execSyspipeInfo->execPartId[i] = execSyspipeInfo_v26_01->execPartId[i];
            execSyspipeInfo->syspipeId[i]  = execSyspipeInfo_v26_01->syspipeId[i];
        }
    }
    *offset += sizeof(GPU_EXEC_SYSPIPE_INFO_v26_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS_v29_05(NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS *ccuSampleInfoParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (ccuSampleInfoParams && buffer)
    {
        NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS_v29_05 *ccuSampleInfoParams_v29_05 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS_v29_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        ccuSampleInfoParams_v29_05 = (void*)(buffer + *offset);

        ccuSampleInfoParams->ccuSampleSize = ccuSampleInfoParams_v29_05->ccuSampleSize;
    }
    *offset += sizeof(NV2080_CTRL_INTERNAL_CCU_SAMPLE_INFO_PARAMS_v29_05);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_v12_01(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *grZcullInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If grZcullInfo and buffer are valid, then copy data and return the offset
    if (grZcullInfo && buffer)
    {
        NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_v12_01 *get_zcull_info_params_12_01 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_v12_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        get_zcull_info_params_12_01 = (void*)(buffer + *offset);

        grZcullInfo->widthAlignPixels           = get_zcull_info_params_12_01->widthAlignPixels;
        grZcullInfo->heightAlignPixels          = get_zcull_info_params_12_01->heightAlignPixels;
        grZcullInfo->pixelSquaresByAliquots     = get_zcull_info_params_12_01->pixelSquaresByAliquots;
        grZcullInfo->aliquotTotal               = get_zcull_info_params_12_01->aliquotTotal;
        grZcullInfo->zcullRegionByteMultiplier  = get_zcull_info_params_12_01->zcullRegionByteMultiplier;
        grZcullInfo->zcullRegionHeaderSize      = get_zcull_info_params_12_01->zcullRegionHeaderSize;
        grZcullInfo->zcullSubregionHeaderSize   = get_zcull_info_params_12_01->zcullSubregionHeaderSize;
        grZcullInfo->subregionCount             = get_zcull_info_params_12_01->subregionCount;
        grZcullInfo->subregionWidthAlignPixels  = get_zcull_info_params_12_01->subregionWidthAlignPixels;
        grZcullInfo->subregionHeightAlignPixels = get_zcull_info_params_12_01->subregionHeightAlignPixels;

    }

    *offset += sizeof(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_v12_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_PROPERTIES_v1B_01(VGPU_STATIC_PROPERTIES *vgpuStaticProperties, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuStaticProperties and buffer are valid, then copy data and return the offset
    if (vgpuStaticProperties && buffer)
    {
        VGPU_STATIC_PROPERTIES_v1B_01 *vgpu_static_properties_v1B_01 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_PROPERTIES_v1B_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_static_properties_v1B_01 = (void*)(buffer + *offset);

        // encSessionStatsReportingState
        vgpuStaticProperties->encSessionStatsReportingState = vgpu_static_properties_v1B_01->encSessionStatsReportingState;
        vgpuStaticProperties->bProfilingTracingEnabled      = vgpu_static_properties_v1B_01->bProfilingTracingEnabled;
        vgpuStaticProperties->bDebuggingEnabled             = vgpu_static_properties_v1B_01->bDebuggingEnabled;
        vgpuStaticProperties->channelCount                  = vgpu_static_properties_v1B_01->channelCount;
        vgpuStaticProperties->bPblObjNotPresent             = vgpu_static_properties_v1B_01->bPblObjNotPresent;
    }

    *offset += sizeof(VGPU_STATIC_PROPERTIES_v1B_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_PROPERTIES_v26_03(VGPU_STATIC_PROPERTIES *vgpuStaticProperties, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuStaticProperties and buffer are valid, then copy data and return the offset
    if (vgpuStaticProperties && buffer)
    {
        VGPU_STATIC_PROPERTIES_v26_03 *vgpu_static_properties_v26_03 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_PROPERTIES_v26_03))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_static_properties_v26_03 = (void*)(buffer + *offset);

        // encSessionStatsReportingState
        vgpuStaticProperties->encSessionStatsReportingState = vgpu_static_properties_v26_03->encSessionStatsReportingState;
        vgpuStaticProperties->bProfilingTracingEnabled      = vgpu_static_properties_v26_03->bProfilingTracingEnabled;
        vgpuStaticProperties->bDebuggingEnabled             = vgpu_static_properties_v26_03->bDebuggingEnabled;
        vgpuStaticProperties->channelCount                  = vgpu_static_properties_v26_03->channelCount;
        vgpuStaticProperties->bPblObjNotPresent             = vgpu_static_properties_v26_03->bPblObjNotPresent;
        vgpuStaticProperties->vmmuSegmentSize               = vgpu_static_properties_v26_03->vmmuSegmentSize;
    }

    *offset += sizeof(VGPU_STATIC_PROPERTIES_v26_03);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_PROPERTIES_v29_03(VGPU_STATIC_PROPERTIES *vgpuStaticProperties, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuStaticProperties and buffer are valid, then copy data and return the offset
    if (vgpuStaticProperties && buffer)
    {
        VGPU_STATIC_PROPERTIES_v29_03 *vgpu_static_properties_v29_03 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_STATIC_PROPERTIES_v29_03))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_static_properties_v29_03 = (void*)(buffer + *offset);

        // encSessionStatsReportingState
        vgpuStaticProperties->encSessionStatsReportingState = vgpu_static_properties_v29_03->encSessionStatsReportingState;
        vgpuStaticProperties->bProfilingTracingEnabled      = vgpu_static_properties_v29_03->bProfilingTracingEnabled;
        vgpuStaticProperties->bDebuggingEnabled             = vgpu_static_properties_v29_03->bDebuggingEnabled;
        vgpuStaticProperties->channelCount                  = vgpu_static_properties_v29_03->channelCount;
        vgpuStaticProperties->bPblObjNotPresent             = vgpu_static_properties_v29_03->bPblObjNotPresent;
        vgpuStaticProperties->vmmuSegmentSize               = vgpu_static_properties_v29_03->vmmuSegmentSize;
        vgpuStaticProperties->firstAsyncCEIdx               = vgpu_static_properties_v29_03->firstAsyncCEIdx;
    }

    *offset += sizeof(VGPU_STATIC_PROPERTIES_v29_03);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_BSP_GET_CAPS_v25_00(VGPU_BSP_GET_CAPS *vgpuBspCaps, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuBspCaps and buffer are valid, then copy data and return the offset
    if (vgpuBspCaps && buffer)
    {
        VGPU_BSP_GET_CAPS_v25_00 *vgpu_bsp_get_caps_v25_00 = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_BSP_GET_CAPS_v25_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_bsp_get_caps_v25_00 = (void*)(buffer + *offset);

        for (i = 0; i < MAX_NVDEC_ENGINES_V25_00; i++)
        {
            portMemCopy(&(*vgpuBspCaps)[i].capsTbl, NV0080_CTRL_BSP_CAPS_TBL_SIZE,
                        &vgpu_bsp_get_caps_v25_00->bspCaps[i].capsTblData, sizeof(vgpu_bsp_get_caps_v25_00->bspCaps[i].capsTblData));
        }
    }

    *offset += sizeof(VGPU_BSP_GET_CAPS_v25_00);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_GET_LATENCY_BUFFER_SIZE_v1C_09(VGPU_GET_LATENCY_BUFFER_SIZE *vgpu_get_latency_buffer_size, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpu_get_latency_buffer_size and buffer are valid, then copy data and return the offset
    if (vgpu_get_latency_buffer_size && buffer)
    {
        VGPU_GET_LATENCY_BUFFER_SIZE_v1C_09 *vgpu_get_latency_buffer_size_v1C_09 = NULL;
        NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *fifoLatencyBufferSize = *vgpu_get_latency_buffer_size;
        NvU32 i;

        if (bufferSize < (*offset + sizeof(VGPU_GET_LATENCY_BUFFER_SIZE_v1C_09)))
            return NV_ERR_NO_MEMORY;

        vgpu_get_latency_buffer_size_v1C_09 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_ENGINE_TYPE_LAST_v1C_09; i++)
        {
            fifoLatencyBufferSize[i].engineID  = vgpu_get_latency_buffer_size_v1C_09->fifoLatencyBufferSize[i].engineID;
            fifoLatencyBufferSize[i].gpEntries = vgpu_get_latency_buffer_size_v1C_09->fifoLatencyBufferSize[i].gpEntries;
            fifoLatencyBufferSize[i].pbEntries = vgpu_get_latency_buffer_size_v1C_09->fifoLatencyBufferSize[i].pbEntries;
        }
    }

    // If vgpu_get_latency_buffer_size or buffer is NULL, then this will be used to determine the size of the
    // static info buffer corresponding to the negotiated version
    *offset += sizeof(VGPU_GET_LATENCY_BUFFER_SIZE_v1C_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_GET_LATENCY_BUFFER_SIZE_v27_02(VGPU_GET_LATENCY_BUFFER_SIZE *vgpu_get_latency_buffer_size, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpu_get_latency_buffer_size and buffer are valid, then copy data and return the offset
    if (vgpu_get_latency_buffer_size && buffer)
    {
        VGPU_GET_LATENCY_BUFFER_SIZE_v27_02 *vgpu_get_latency_buffer_size_v27_02 = NULL;
        NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *fifoLatencyBufferSize = *vgpu_get_latency_buffer_size;
        NvU32 i;

        if (bufferSize < (*offset + sizeof(VGPU_GET_LATENCY_BUFFER_SIZE_v27_02)))
            return NV_ERR_NO_MEMORY;

        vgpu_get_latency_buffer_size_v27_02 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_ENGINE_TYPE_LAST_v27_02; i++)
        {
            fifoLatencyBufferSize[i].engineID  = vgpu_get_latency_buffer_size_v27_02->fifoLatencyBufferSize[i].engineID;
            fifoLatencyBufferSize[i].gpEntries = vgpu_get_latency_buffer_size_v27_02->fifoLatencyBufferSize[i].gpEntries;
            fifoLatencyBufferSize[i].pbEntries = vgpu_get_latency_buffer_size_v27_02->fifoLatencyBufferSize[i].pbEntries;
        }
    }

    // If vgpu_get_latency_buffer_size or buffer is NULL, then this will be used to determine the size of the
    // static info buffer corresponding to the negotiated version
    *offset += sizeof(VGPU_GET_LATENCY_BUFFER_SIZE_v27_02);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_CE_GET_CAPS_V2_v24_09(VGPU_CE_GET_CAPS_V2 *ceCapsPtr, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If ceCapsPtr and buffer are valid, then copy data and return the offset
    if (ceCapsPtr && buffer)
    {
        NV2080_CTRL_CE_GET_CAPS_V2_PARAMS *ceCaps = *ceCapsPtr;
        VGPU_CE_GET_CAPS_V2_v24_09 *vgpu_ce_get_caps_v2_v24_09 = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(VGPU_CE_GET_CAPS_V2_v24_09))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_ce_get_caps_v2_v24_09 = (void*)(buffer + *offset);

        for (i = 0; i < NV2080_ENGINE_TYPE_COPY_SIZE_v24_09; i++) {

            ceCaps[i].ceEngineType = vgpu_ce_get_caps_v2_v24_09->ceCaps[i].ceEngineType;
            portMemCopy(&ceCaps[i].capsTbl, NV2080_CTRL_CE_CAPS_TBL_SIZE,
                        &vgpu_ce_get_caps_v2_v24_09->ceCaps[i].capsTbl, NV2080_CTRL_CE_CAPS_TBL_SIZE);
        }
    }

    *offset += sizeof(VGPU_CE_GET_CAPS_V2_v24_09);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_v15_02(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *nvlinkCaps, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If nvlinkCaps and buffer are valid, then copy data and return the offset
    if (nvlinkCaps && buffer)
    {
        NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_v15_02 *get_nvlink_caps_v15_02 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_v15_02))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        get_nvlink_caps_v15_02 = (void*)(buffer + *offset);

        nvlinkCaps->capsTbl              = get_nvlink_caps_v15_02->capsTbl;
        nvlinkCaps->lowestNvlinkVersion  = get_nvlink_caps_v15_02->lowestNvlinkVersion;
        nvlinkCaps->highestNvlinkVersion = get_nvlink_caps_v15_02->highestNvlinkVersion;
        nvlinkCaps->lowestNciVersion     = get_nvlink_caps_v15_02->lowestNciVersion;
        nvlinkCaps->highestNciVersion    = get_nvlink_caps_v15_02->highestNciVersion;
        nvlinkCaps->discoveredLinkMask   = get_nvlink_caps_v15_02->discoveredLinkMask;
        nvlinkCaps->enabledLinkMask      = get_nvlink_caps_v15_02->enabledLinkMask;
    }

    *offset += sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS_v15_02);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_FLA_GET_RANGE_PARAMS_v1A_18(NV2080_CTRL_FLA_GET_RANGE_PARAMS *range_params, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If range_params and buffer are valid, then copy data and return the offset
    if (range_params && buffer)
    {
        NV2080_CTRL_FLA_GET_RANGE_PARAMS_v1A_18 *range_params_v1A_18 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_FLA_GET_RANGE_PARAMS_v1A_18))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        range_params_v1A_18 = (void*)(buffer + *offset);

        range_params->base = range_params_v1A_18->base;
        range_params->size = range_params_v1A_18->size;

    }

    *offset += sizeof(NV2080_CTRL_FLA_GET_RANGE_PARAMS_v1A_18);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_v21_0C(NVA080_CTRL_VGPU_GET_CONFIG_PARAMS *vgpuConfig, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If vgpuConfig and buffer are valid, then copy data and return the offset
    if (vgpuConfig && buffer)
    {
        NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_v21_0C *vgpu_get_config_params_v21_0C = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_v21_0C))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        vgpu_get_config_params_v21_0C = (void*)(buffer + *offset);

        // vgpuConfig
        vgpuConfig->frameRateLimiter              = vgpu_get_config_params_v21_0C->frameRateLimiter;
        vgpuConfig->swVSyncEnabled                = vgpu_get_config_params_v21_0C->swVSyncEnabled;
        vgpuConfig->cudaEnabled                   = vgpu_get_config_params_v21_0C->cudaEnabled;
        vgpuConfig->pluginPteBlitEnabled          = vgpu_get_config_params_v21_0C->pluginPteBlitEnabled;
        vgpuConfig->disableWddm1xPreemption       = vgpu_get_config_params_v21_0C->disableWddm1xPreemption;
        vgpuConfig->debugBufferSize               = vgpu_get_config_params_v21_0C->debugBufferSize;
        vgpuConfig->debugBuffer                   = vgpu_get_config_params_v21_0C->debugBuffer;
        vgpuConfig->guestFbOffset                 = vgpu_get_config_params_v21_0C->guestFbOffset;
        vgpuConfig->mappableCpuHostAperture       = vgpu_get_config_params_v21_0C->mappableCpuHostAperture;
        vgpuConfig->linuxInterruptOptimization    = vgpu_get_config_params_v21_0C->linuxInterruptOptimization;
        vgpuConfig->vgpuDeviceCapsBits            = vgpu_get_config_params_v21_0C->vgpuDeviceCapsBits;
        vgpuConfig->maxPixels                     = vgpu_get_config_params_v21_0C->maxPixels;
        vgpuConfig->uvmEnabledFeatures            = vgpu_get_config_params_v21_0C->uvmEnabledFeatures;
        vgpuConfig->enableKmdSysmemScratch        = vgpu_get_config_params_v21_0C->enableKmdSysmemScratch;
    }

   *offset += sizeof(NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_v21_0C);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_v1F_08(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *pcieSupportedGpuAtomics, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If pcieSupportedGpuAtomics and buffer are valid, then copy data and return the offset
    if (pcieSupportedGpuAtomics && buffer)
    {
        NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_v1F_08 *pcie_supported_gpu_atomics_v1F_08 = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_v1F_08))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pcie_supported_gpu_atomics_v1F_08 = (void*)(buffer + *offset);

        NV_ASSERT_OR_RETURN(NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT <= NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT_v1F_08,
                            NV_ERR_INSUFFICIENT_RESOURCES);

        for (i = 0; i < NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT_v1F_08; i++) {
            pcieSupportedGpuAtomics->atomicOp[i].bSupported = pcie_supported_gpu_atomics_v1F_08->atomicOp[i].bSupported;
            pcieSupportedGpuAtomics->atomicOp[i].attributes = pcie_supported_gpu_atomics_v1F_08->atomicOp[i].attributes;
        }
    }

    *offset += sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_v1F_08);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_v21_0A(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *cegetAllCaps, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If cegetAllCaps and buffer are valid, then copy data and return the offset
    if (cegetAllCaps && buffer)
    {
        NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_v21_0A *ce_get_all_caps_v21_0A = NULL;
        NvU32 i;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_v21_0A))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        ce_get_all_caps_v21_0A = (void*)(buffer + *offset);

        NV_ASSERT_OR_RETURN(NV2080_CTRL_MAX_PCES <= NV2080_CTRL_MAX_PCES_v21_0A,
                            NV_ERR_INSUFFICIENT_RESOURCES);

        NV_ASSERT_OR_RETURN(NV2080_CTRL_CE_CAPS_TBL_SIZE <=  NV2080_CTRL_CE_CAPS_TBL_SIZE_v21_0A,
                            NV_ERR_INSUFFICIENT_RESOURCES);

        cegetAllCaps->present = ce_get_all_caps_v21_0A->present;

        for (i = 0; i < NV2080_CTRL_MAX_PCES_v21_0A; i++) {
            portMemCopy(cegetAllCaps->capsTbl[i], (sizeof(NvU8) * NV2080_CTRL_CE_CAPS_TBL_SIZE_v21_0A),
                        ce_get_all_caps_v21_0A->ceCaps[i].capsTbl, (sizeof(NvU8) * NV2080_CTRL_CE_CAPS_TBL_SIZE_v21_0A));
        }
    }

    *offset += sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS_v21_0A);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_v22_01(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *c2cInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    // If c2cInfo and buffer are valid, then copy data and return the offset
    if (c2cInfo && buffer)
    {
        NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_v22_01 *c2c_info_v22_01 = NULL;

        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_v22_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        c2c_info_v22_01 = (void*)(buffer + *offset);

        c2cInfo->bIsLinkUp = c2c_info_v22_01->bIsLinkUp;
        c2cInfo->nrLinks = c2c_info_v22_01->nrLinks;
        c2cInfo->linkMask = c2c_info_v22_01->linkMask;
        c2cInfo->perLinkBwMBps = c2c_info_v22_01->perLinkBwMBps;
        c2cInfo->remoteType = c2c_info_v22_01->remoteType;
    }

    *offset += sizeof(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_v22_01);

    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS_v25_00(
    NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS_v25_00 *pParams_v25_00 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v25_00))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v25_00 = (void*)(buffer + *offset);
        NV_ASSERT_OR_RETURN(NV0080_CTRL_MSENC_CAPS_TBL_SIZE_V25_00 <=
                            NV0080_CTRL_MSENC_CAPS_TBL_SIZE,
                            NV_ERR_INSUFFICIENT_RESOURCES);

        pParams->instanceId = pParams_v25_00->instanceId;
        portMemCopy(pParams->capsTbl, sizeof(pParams->capsTbl),
            pParams_v25_00->capsTbl, sizeof(pParams_v25_00->capsTbl));
    }

    *offset += sizeof(*pParams_v25_00);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS_v25_01(
    NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS_v25_01 *pParams_v25_01 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v25_01))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v25_01 = (void*)(buffer + *offset);

        pParams->numConstructedFalcons = pParams_v25_01->numConstructedFalcons;
        for (i = 0; i < NV2080_CTRL_GPU_MAX_CONSTRUCTED_FALCONS; i++) {
            NV2080_CTRL_GPU_CONSTRUCTED_FALCON_INFO *dst = &(pParams->constructedFalconsTable[i]);
            NV2080_CTRL_GPU_CONSTRUCTED_FALCON_INFO_v25_01 *src = &(pParams_v25_01->constructedFalconsTable[i]);

            dst->engDesc = src->engDesc;
            dst->ctxAttr = src->ctxAttr;
            dst->ctxBufferSize = src->ctxBufferSize;
            dst->addrSpaceList = src->addrSpaceList;
            dst->registerBase = src->registerBase;
        }
    }

    *offset += sizeof(*pParams_v25_01);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_P2P_CAPABILITY_PARAMS_v25_03(
    VGPU_P2P_CAPABILITY_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    VGPU_P2P_CAPABILITY_PARAMS_v25_03 *pParams_v = NULL;

    if (offset == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v = (void*)(buffer + *offset);

        pParams->bGpuSupportsFabricProbe = pParams_v->bGpuSupportsFabricProbe;
    }

    *offset += sizeof(*pParams_v);
    return NV_OK;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v25_05(
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v25_05 *pParams_v25_05 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v25_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v25_05 = (void*)(buffer + *offset);

        pParams->numEntries = pParams_v25_05->numEntries;
        for (i = 0; i < NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES_V25_05; i++) {
            NV2080_CTRL_INTERNAL_DEVICE_INFO *dst = &(pParams->deviceInfoTable[i]);
            NV2080_CTRL_INTERNAL_DEVICE_INFO_v25_05 *src = &(pParams_v25_05->deviceInfoTable[i]);

            dst->faultId = src->faultId;
            dst->instanceId = src->instanceId;
            dst->typeEnum = src->typeEnum;
            dst->resetId = src->resetId;
            dst->devicePriBase = src->devicePriBase;
            dst->isEngine = src->isEngine;
            dst->rlEngId = src->rlEngId;
            dst->runlistPriBase = src->runlistPriBase;
            dst->groupId = src->groupId;
        }
    }

    *offset += sizeof(*pParams_v25_05);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v27_05(
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v27_05 *pParams_v27_05 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v27_05))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v27_05 = (void*)(buffer + *offset);

        pParams->numEntries = pParams_v27_05->numEntries;
        for (i = 0; i < NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES_V25_05; i++) {
            NV2080_CTRL_INTERNAL_DEVICE_INFO *dst = &(pParams->deviceInfoTable[i]);
            NV2080_CTRL_INTERNAL_DEVICE_INFO_v27_05 *src = &(pParams_v27_05->deviceInfoTable[i]);

            dst->faultId = src->faultId;
            dst->instanceId = src->instanceId;
            dst->typeEnum = src->typeEnum;
            dst->resetId = src->resetId;
            dst->devicePriBase = src->devicePriBase;
            dst->isEngine = src->isEngine;
            dst->rlEngId = src->rlEngId;
            dst->runlistPriBase = src->runlistPriBase;
            dst->groupId = src->groupId;
            dst->ginTargetId = src->ginTargetId;
            dst->deviceBroadcastPriBase = src->deviceBroadcastPriBase;
        }
    }

    *offset += sizeof(*pParams_v27_05);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v28_04(
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS_v28_04 *pParams_v28_04 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        NvU32 i;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v28_04))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v28_04 = (void*)(buffer + *offset);

        pParams->numEntries = pParams_v28_04->numEntries;
        for (i = 0; i < NV2080_CTRL_CMD_INTERNAL_DEVICE_INFO_MAX_ENTRIES_V28_04; i++) {
            NV2080_CTRL_INTERNAL_DEVICE_INFO *dst = &(pParams->deviceInfoTable[i]);
            NV2080_CTRL_INTERNAL_DEVICE_INFO_v28_04 *src = &(pParams_v28_04->deviceInfoTable[i]);

            dst->faultId = src->faultId;
            dst->instanceId = src->instanceId;
            dst->typeEnum = src->typeEnum;
            dst->resetId = src->resetId;
            dst->devicePriBase = src->devicePriBase;
            dst->isEngine = src->isEngine;
            dst->rlEngId = src->rlEngId;
            dst->runlistPriBase = src->runlistPriBase;
            dst->groupId = src->groupId;
            dst->ginTargetId = src->ginTargetId;
            dst->deviceBroadcastPriBase = src->deviceBroadcastPriBase;
            dst->groupLocalInstanceId = src->groupLocalInstanceId;
        }
    }

    *offset += sizeof(*pParams_v28_04);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS_v25_06(
    NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize,
    NvU32 *offset)
{
    NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS_v25_06 *pParams_v25_06 = NULL;

    if (offset == NULL)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }

    if (pParams != NULL && buffer != NULL)
    {
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(*pParams_v25_06))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pParams_v25_06 = (void*)(buffer + *offset);

        pParams->bOneToOneComptagLineAllocation = pParams_v25_06->bOneToOneComptagLineAllocation;
        pParams->bUseOneToFourComptagLineAllocation = pParams_v25_06->bUseOneToFourComptagLineAllocation;
        pParams->bUseRawModeComptaglineAllocation = pParams_v25_06->bUseRawModeComptaglineAllocation;
        pParams->bDisableCompbitBacking = pParams_v25_06->bDisableCompbitBacking;
        pParams->bDisablePostL2Compression = pParams_v25_06->bDisablePostL2Compression;
        pParams->bEnabledEccFBPA = pParams_v25_06->bEnabledEccFBPA;
        pParams->bL2PreFill = pParams_v25_06->bL2PreFill;
        pParams->l2CacheSize = pParams_v25_06->l2CacheSize;
        pParams->bFbpaPresent = pParams_v25_06->bFbpaPresent;
        pParams->comprPageSize = pParams_v25_06->comprPageSize;
        pParams->comprPageShift = pParams_v25_06->comprPageShift;
        pParams->ramType = pParams_v25_06->ramType;
        pParams->ltcCount = pParams_v25_06->ltcCount;
        pParams->ltsPerLtcCount = pParams_v25_06->ltsPerLtcCount;
    }

    *offset += sizeof(*pParams_v25_06);
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v25_13(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    if (!offset)
    {
        return NVOS_STATUS_ERROR_INVALID_ARGUMENT;
    }
    // If pParams and buffer are valid, then copy data and return the offset
    if (pParams && buffer)
    {
        NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v25_13 *pParams_v25_13  = NULL;
        if ((bufferSize < *offset) ||
            (bufferSize < (*offset + sizeof(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v25_13))))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }
        pParams_v25_13       = (void*)(buffer + *offset);
        pParams->eccMask     = pParams_v25_13->eccMask;
        pParams->nvlinkMask  = pParams_v25_13->nvlinkMask;
    }
    *offset += sizeof(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v25_13);
    return NVOS_STATUS_SUCCESS;
}

// Stub functions for earlier versions

NV_STATUS deserialize_VGPU_STATIC_INFO2_v18_07(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v18_0C(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v18_0F(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v1A_02(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v1A_04(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v1A_07(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v1C_09(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v20_03(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
   return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v21_04(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v21_09(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_VGPU_STATIC_INFO2_v24_05(VGPU_STATIC_INFO2 *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_SUCCESS;
}

NV_STATUS deserialize_NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS_v03_00(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS *grZcullInfo, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v07_00(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v09_04(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v12_00(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v12_01(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v12_06(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v12_0A(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v13_06(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v16_02(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v16_05(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v16_07(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v17_00(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v17_01(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v17_05(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_03(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_04(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_0E(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_10(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_11(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_13(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v18_16(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v19_00(VGPU_STATIC_INFO *vgpu_static_info, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1A_00(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1A_05(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1B_03(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1D_01(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1F_02(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v1F_09(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v20_01(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v20_04(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v21_09(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v23_01(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_VGPU_STATIC_INFO_v24_05(VGPU_STATIC_INFO *pVSI, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}

NV_STATUS deserialize_NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v18_0B(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams, NvU8 *buffer, NvU32 bufferSize, NvU32 *offset)
{
    return NVOS_STATUS_ERROR_NOT_SUPPORTED;
}
