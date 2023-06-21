/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"

#include "os/os.h"
#include "gpu/external_device/gsync.h"

#include "gpu/external_device/gsync_api.h"
#include "rmapi/rs_utils.h"
#include "resserv/rs_client.h"

NV_STATUS
gsyncapiConstruct_IMPL
(
    GSyncApi                     *pGSyncApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                status = NV_OK;
    NV30F1_ALLOC_PARAMETERS *pNv30f1AllocParams = pParams->pAllocParams;
    RmClient                *pClient            = dynamicCast(pCallContext->pClient, RmClient);
    NvU32                    eventNum = 0;

    RMCFG_FEATURE_ENABLED_OR_BAIL(EXTDEV_GSYNC);

    if (pClient == NULL)
    {
        return NV_ERR_INVALID_CLIENT;
    }

    // validate gsync instance
    if ( ! gsyncIsInstanceValid(pNv30f1AllocParams->gsyncInstance))
        return NV_ERR_INVALID_ARGUMENT;

    // init the gsync
    pGSyncApi->instance             = pNv30f1AllocParams->gsyncInstance;
    pGSyncApi->classNum             = NV30_GSYNC;
    pGSyncApi->notifyAction         = NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_DISABLE;
    pGSyncApi->lastEventNotified    = NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_LOSS;
    pGSyncApi->oldEventNotification = NV_FALSE;

    for (eventNum = 0; eventNum < NV30F1_CTRL_GSYNC_EVENT_TYPES; eventNum++)
    {
        pGSyncApi->pEventByType[eventNum] = NULL;
    }

    return status;
}

NV_STATUS
gsyncapiControl_IMPL
(
    GSyncApi                       *pGSyncApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    RMCFG_FEATURE_ENABLED_OR_BAIL(EXTDEV_GSYNC);

    if (gsyncIsInstanceValid(pGSyncApi->instance) == NV_FALSE)
        return NV_ERR_INVALID_ARGUMENT;

    return resControl_IMPL(staticCast(pGSyncApi, RsResource), pCallContext, pParams->pLegacyParams);
}

void
CliNotifyGsyncEvent
(
    NvU32 gsyncInst,
    NvU32 eventFlags
)
{
    RS_SHARE_ITERATOR it = serverutilShareIter(classId(NotifShare));

    // RMCONFIG: is GYSNC enabled?
    if ( ! RMCFG_FEATURE_EXTDEV_GSYNC)
    {
        NV_ASSERT(RMCFG_FEATURE_EXTDEV_GSYNC);
        return;
    }


    // search notifiers with events hooked up for this gpu
    while (serverutilShareIterNext(&it))
    {
        RsShared *pShared = it.pShared;
        GSyncApi *pGSyncApi;
        INotifier *pNotifier;
        NotifShare *pNotifierShare = dynamicCast(pShared, NotifShare);
        PEVENTNOTIFICATION pEventNotification;

        if ((pNotifierShare == NULL) || (pNotifierShare->pNotifier == NULL))
            continue;

        pNotifier = pNotifierShare->pNotifier;
        pGSyncApi = dynamicCast(pNotifier, GSyncApi);

        if (pGSyncApi == NULL)
            continue;

        // If this is the gsync instance we're looking for and
        // it's marked in use and the user has bound an event
        // to it and the user has enabled event notification for
        // it, then we're ready to signal the event...
        //
        // ...unless we're doing smart filtering, in which case
        // we only notify if the event intersects the action mask,
        // and is different from the last event we sent.
        //

        if (pGSyncApi->instance != gsyncInst)
            continue;

        pEventNotification = inotifyGetNotificationList(pNotifier);

        if (pGSyncApi->oldEventNotification)
        {
            // client is using old api
            OBJGPU *pMasterableGpu = gsyncGetMasterableGpuByInstance(gsyncInst);
            NvU32 oldEventFlags = gsyncConvertNewEventToOldEventNum(eventFlags);
            NvU32 tempMask = oldEventFlags;
            NvU32 eventNum = 0;

            // convert mask to array index (only one bit should be set)
            while (tempMask >>= 1)
            {
                eventNum++;
            }

            // now, look for an event to trigger
            if (eventNum < NV30F1_CTRL_GSYNC_EVENT_TYPES)
            {
                pEventNotification = pGSyncApi->pEventByType[eventNum];
            }

            //
            // Syncloss could also be a notifcation to reset the
            // event notification tracking when enabling framelock.
            //
            if ((NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_LOSS == oldEventFlags) &&
                (pEventNotification == NULL))
            {
                // update smart event notification tracking when
                // client is NOT registered for SYNC_LOSS
                // Ref. CL 14042432 and bug 200668208 for details
                pGSyncApi->lastEventNotified = oldEventFlags;
            }

            if (pMasterableGpu && pEventNotification &&
                (pGSyncApi->notifyAction & oldEventFlags) &&
                (pGSyncApi->lastEventNotified != oldEventFlags))
            {
                NV_PRINTF(LEVEL_INFO,
                          "gsync instance 0x%0x has had a status change: 0x%0x\n",
                          gsyncInst, oldEventFlags);

                osEventNotification(
                    pMasterableGpu,
                    pEventNotification,
                    OS_EVENT_NOTIFICATION_INDEX_ALL,
                    NULL,
                    0);

                // update smart event notification tracking
                pGSyncApi->lastEventNotified = oldEventFlags;
                continue;
            }
        }
        else
        {
            OBJGPU *pMasterableGpu = gsyncGetMasterableGpuByInstance(gsyncInst);

            if (pEventNotification == NULL)
                continue;

            //
            // Allowing NV30F1_GSYNC_NOTIFIERS_ALL to handle for all event types to make the
            // interface generic. Though for Windows, NV01_EVENT_OS_EVENT can only signal an
            // event but not handle over any information.
            //
            if (pEventNotification->NotifyType == NV01_EVENT_KERNEL_CALLBACK ||
                 (pEventNotification->NotifyIndex == NV30F1_GSYNC_NOTIFIERS_ALL))
            {
                NvNotification eventData;
                portMemSet((void *)&eventData, 0, sizeof(NvNotification));
                eventData.info32 = eventFlags;

                NV_PRINTF(LEVEL_INFO,
                          "gsync instance 0x%0x has had a status change: 0x%0x\n",
                          gsyncInst, eventFlags);

                osEventNotification(
                    pMasterableGpu,
                    pEventNotification,
                    OS_EVENT_NOTIFICATION_INDEX_ALL,
                    (void *)&eventData,
                    sizeof(eventData));
            }
            else
            {
                NvU32 notifyIndex;

                NV_ASSERT((pEventNotification->NotifyType == NV01_EVENT_KERNEL_CALLBACK_EX) ||
                          (pEventNotification->NotifyType == NV01_EVENT_OS_EVENT));

                for (notifyIndex = 0; eventFlags >> notifyIndex; notifyIndex++)
                {
                    if (eventFlags & NVBIT(notifyIndex))
                    {
                        NV_PRINTF(LEVEL_INFO,
                                  "gsync instance 0x%0x has had a status change: %d\n",
                                  gsyncInst, notifyIndex);

                        if (pEventNotification->NotifyType == NV01_EVENT_OS_EVENT)
                        {
                            osObjectEventNotification(
                                pNotifierShare->hNotifierClient,
                                pNotifierShare->hNotifierResource,
                                pGSyncApi->classNum, pEventNotification, notifyIndex, NULL, 0);
                        }
                        else
                        {
                            NvNotification eventData;
                            portMemSet((void *)&eventData, 0, sizeof(NvNotification));
                            eventData.info32 = notifyIndex;

                            NV_ASSERT(pEventNotification->NotifyType == NV01_EVENT_KERNEL_CALLBACK_EX);

                            osEventNotification(
                                pMasterableGpu,
                                pEventNotification,
                                notifyIndex,
                                (void *)&eventData,
                                sizeof(eventData));
                        }
                    }
                }
            }
        }
    }
}

