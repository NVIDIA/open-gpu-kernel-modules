/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmconfig.h"
#include "g_rmconfig_private.h"
#include "rmapi/rmapi_specific.h"
#include "rmapi/event.h"
#include "resource_desc.h"
#include "resserv/rs_server.h"

#include "class/cl0005.h" // NV01_EVENT
#include "class/clc574.h" // UVM_CHANNEL_RETAINER

#include "class/cl83de.h" // GT200_DEBUGGER

#include "ctrl/ctrl0002.h" // NV0002_CTRL_CMD_*_CONTEXTDMA

#include "ctrl/ctrl2080/ctrl2080bus.h" // NV2080_CTRL_CMD_BUS_SYSMEM_ACCESS
#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL
#include "ctrl/ctrl2080/ctrl2080rc.h" // NV2080_CTRL_CMD_RC_READ_VIRTUAL_MEM
#include "ctrl/ctrl2080/ctrl2080nvd.h" // NV2080_CTRL_CMD_NVD_SET_NOCAT_JOURNAL_DATA
#include "ctrl/ctrl2080/ctrl2080tmr.h" // NV2080_CTRL_CMD_TIMER_*
#include "ctrl/ctrl9010.h" //NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION
#include "ctrl/ctrl906f.h" // NV906F_CTRL_CMD_GET_MMU_FAULT_INFO
#include "ctrl/ctrlc370/ctrlc370chnc.h" // NVC370_CTRL_CMD_*

#include "ctrl/ctrl2080/ctrl2080gr.h"       // NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND
#include "ctrl/ctrl2080/ctrl2080internal.h" // NV2080_CTRL_CMD_INTERNAL_GR_CTXSW_ZCULL_BIND

NV_STATUS
rmapiFixupAllocParams
(
    RS_RESOURCE_DESC **ppResDesc,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    RS_RESOURCE_DESC *pResDesc = *ppResDesc;

    if ((pResDesc->pClassInfo != NULL) && (pResDesc->pClassInfo->classId == classId(Event)))
    {
        NV0005_ALLOC_PARAMETERS *pNv0005Params = pRmAllocParams->pAllocParams;

        //
        // This field isn't filled out consistently by clients. Some clients specify NV01_EVENT as the class
        // and then override it using the subclass in the event parameters, while other clients specify the
        // same subclass in both the RmAllocParams and event params. NV01_EVENT isn't a valid class to allocate
        // so overwrite it with the subclass from the event params.
        //
        if (pRmAllocParams->externalClassId == NV01_EVENT)
            pRmAllocParams->externalClassId = pNv0005Params->hClass;

        pNv0005Params->hSrcResource = pRmAllocParams->hParent;

        // No support for event and src resource that reside under different clients
        if (pNv0005Params->hParentClient != pRmAllocParams->hClient)
            pRmAllocParams->hParent = pRmAllocParams->hClient;

        // class id may have changed so refresh the resource descriptor, but make sure it is still an Event
        pResDesc = RsResInfoByExternalClassId(pRmAllocParams->externalClassId);
        if (pResDesc == NULL || pResDesc->pClassInfo == NULL || pResDesc->pClassInfo->classId != classId(Event))
            return NV_ERR_INVALID_CLASS;

        *ppResDesc = pResDesc;
    }

    return NV_OK;
}

NV_STATUS
serverAllocLookupSecondClient
(
    NvU32     externalClassId,
    void     *pAllocParams,
    NvHandle *phSecondClient
)
{
    RS_RESOURCE_DESC *pResDesc = RsResInfoByExternalClassId(externalClassId);

    *phSecondClient = NV01_NULL_OBJECT;

    if ((pAllocParams != NULL) && (pResDesc != NULL) &&
        (pResDesc->flags & RS_FLAGS_DUAL_CLIENT_LOCK))
    {
        switch (externalClassId)
        {
            case GT200_DEBUGGER:
            {
                *phSecondClient = ((NV83DE_ALLOC_PARAMETERS *) pAllocParams)->hAppClient;
                break;
            }
            case UVM_CHANNEL_RETAINER:
            {
                *phSecondClient = ((NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS *) pAllocParams)->hClient;
                break;
            }
            case NV01_EVENT:
            case NV01_EVENT_OS_EVENT:
            case NV01_EVENT_KERNEL_CALLBACK:
            case NV01_EVENT_KERNEL_CALLBACK_EX:
            {
                *phSecondClient = ((NV0005_ALLOC_PARAMETERS *) pAllocParams)->hParentClient;
                break;
            }
            default:
            {
                // RS_FLAGS_DUAL_CLIENT_LOCK flag requires adding a case statement here.
                NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
            }
        }
    }

    return NV_OK;
}

NV_STATUS
serverControlLookupSecondClient
(
    NvU32              cmd,
    void              *pControlParams,
    RS_CONTROL_COOKIE *pCookie,
    NvHandle          *phSecondClient
)
{
    *phSecondClient = NV01_NULL_OBJECT;

    if ((pControlParams != NULL) && (pCookie->ctrlFlags & RMCTRL_FLAGS_DUAL_CLIENT_LOCK))
    {
        switch (cmd)
        {
            case NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND:
            {
                *phSecondClient = ((NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *) pControlParams)->hClient;
                break;
            }
            default:
                // RMCTRL_FLAGS_DUAL_CLIENT_LOCK flag requires adding a case statement here.
                NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
        }
    }

    return NV_OK;
}

NvBool
rmapiRmControlCanBeRaisedIrql(NvU32 cmd)
{
    switch (cmd)
    {
        case NV2080_CTRL_CMD_TIMER_SCHEDULE:
        case NV2080_CTRL_CMD_TIMER_GET_TIME:
        // Below 2 control calls are used for flip canceling (HW Flip Queue)
        // We use TRASH/ABORT mode to discard queued hw commands in the push buffer (bug 200644346)
        case NVC370_CTRL_CMD_SET_ACCL:
        case NVC370_CTRL_CMD_GET_CHANNEL_INFO:
        case NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

NvBool
rmapiRmControlCanBeBypassLock(NvU32 cmd)
{
    switch (cmd)
    {
        case NV2080_CTRL_CMD_RC_READ_VIRTUAL_MEM:
        case NV2080_CTRL_CMD_TIMER_GET_TIME:
        case NV906F_CTRL_CMD_GET_MMU_FAULT_INFO:
        // Below 2 control calls are used for flip canceling (HW Flip Queue)
        // We use TRASH/ABORT mode to discard queued hw commands in the push buffer (bug 200644346)
        case NVC370_CTRL_CMD_SET_ACCL:
        case NVC370_CTRL_CMD_GET_CHANNEL_INFO:
        case NV2080_CTRL_CMD_BUS_SYSMEM_ACCESS:
        case NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION:
        case NV2080_CTRL_CMD_NVD_SET_NOCAT_JOURNAL_DATA:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}
