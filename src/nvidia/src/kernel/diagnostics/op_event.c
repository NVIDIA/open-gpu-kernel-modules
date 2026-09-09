/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "diagnostics/op_event.h"

#include "core/prelude.h"        // RMCFG_IS_PLATFORM (g_rmconfig_private.h)
#include "nvport/nvport.h"
#include "utils/nvassert.h"

static inline NvU64 _opevtNextInstanceId(void)
{
    // Multi-producer platforms use a thread-safe atomic counter for allocating instance IDs.
    static PORT_ATOMIC NvU64 g_opEventInstanceId;
    return portAtomicExIncrementU64(&g_opEventInstanceId);
}

NV_STATUS
opevtConstruct_IMPL
(
    OperationalEvent             *pEvent,
    EventBus                     *pEventBus,
    NvU32                         category,
    const char                   *pModuleSignature,
    NvU32                         eventCode,
    NvU32                         severity,
    NvU32                         attributes,
    NvU32                         logLevel
)
{
    pEvent->category = (NvU16)category;
    pEvent->eventCode = (NvU16)eventCode;
    portStringCopy(pEvent->moduleSignature, sizeof(pEvent->moduleSignature),
                   pModuleSignature, OPERATIONAL_EVENT_MODULE_SIGNATURE_LEN);

    pEvent->instanceId = _opevtNextInstanceId();
    pEvent->timestamp = portTimeGetMicroseconds();
    pEvent->severity = (OPERATIONAL_EVENT_SEVERITY)severity;
    pEvent->attributes = (OPERATIONAL_EVENT_ATTRIBUTES)attributes;
    pEvent->logLevel = (OPERATIONAL_EVENT_LOG_LEVEL)logLevel;
    pEvent->pContextList = NULL;

    return NV_OK;
}

void
opevtAddContext_IMPL
(
    OperationalEvent  *pEvent,
    EventContextHeader *pContext
)
{
    EventContextHeader **ppNext;

    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);
    NV_ASSERT_OR_RETURN_VOID(pContext != NULL);

    pContext->pNext = NULL;

    ppNext = &pEvent->pContextList;
    while (*ppNext != NULL)
    {
        ppNext = &(*ppNext)->pNext;
    }

    *ppNext = pContext;
}

void
opevtDestruct_IMPL
(
    OperationalEvent *pEvent
)
{
    EventContextHeader *pCtx;

    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);

    //
    // Walk the live context chain and free any nodes the producer marked as event-owned. Embedded
    // contexts (bOwnedByEvent == NV_FALSE, the default for producers that zero-init their context
    // storage) are skipped -- their lifetime is tied to the producer's allocation, not the event's.
    // The pNext walk uses a saved pointer because we may free the current node mid-loop.
    //
    pCtx = pEvent->pContextList;
    while (pCtx != NULL)
    {
        EventContextHeader *pNext = pCtx->pNext;
        if (pCtx->bOwnedByEvent)
            portMemFree(pCtx);
        pCtx = pNext;
    }
    pEvent->pContextList = NULL;
}

