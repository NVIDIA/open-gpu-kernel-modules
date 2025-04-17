/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/disp/vblank_callback/vblank.h"

#include "kernel/gpu/disp/head/kernel_head.h"
#include "kernel/gpu/disp/kern_disp.h"
#include "kernel/gpu/timer/objtmr.h"
#include "kernel/gpu/gpu.h"
#include "kernel/os/os.h"

void
kheadAddVblankCallback_IMPL
(
    OBJGPU         *pGpu,
    KernelHead     *pKernelHead,
    VBLANKCALLBACK *pCallback
)
{
    NvBool           OktoAdd = NV_TRUE;
    VBLANKCALLBACK  *pCheck  = NULL;
    VBLANKCALLBACK  *pNext   = NULL;
    VBLANKCALLBACK  *pPrev   = NULL;
    VBLANKCALLBACK  *pList   = NULL;
    NvU32            Count;
    NvBool           vblankIntrIsBeingGenerated = NV_FALSE;

    //
    // If callback needs vblank safety, make it low-latency, persistent and promote-to-front.
    // The callback is responsible for clearing its own persistence & safety flags,
    // once it achieves its raison d'etre, within it's own particular idiom.
    //
    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_GUARANTEE_SAFETY)
    {
        pCallback->Flags |= VBLANK_CALLBACK_FLAG_PERSISTENT;
        pCallback->Flags |= VBLANK_CALLBACK_FLAG_LOW_LATENCY;
        pCallback->Flags |= VBLANK_CALLBACK_FLAG_PROMOTE_TO_FRONT;
    }

    // Cache the requested queue and its current vblank count
    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_LOW_LATENCY)
    {
        pList = pKernelHead->Vblank.Callback.pListLL;
        Count = pKernelHead->Vblank.Counters.LowLatency;
    }
    else
    {
        pList = pKernelHead->Vblank.Callback.pListNL;
        Count = pKernelHead->Vblank.Counters.NormLatency;
    }

    //
    // If this callback is supposed to fire at a specific vblank count,
    // then that count (VBlankCount) better be in the future still.
    //
    NV_ASSERT(!(pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT) ||
               (pCallback->VBlankCount > Count) );

    NV_PRINTF(LEVEL_INFO, "headAddVblankCallback: pGpu=%p cb=%p\n", pGpu,
              pCallback);
    NV_PRINTF(LEVEL_INFO,
              "    cbproc=%p cbobj=%p p1=0x%x p2=0x%x count=0x%x flags=0x%x offset=0x%x\n",
              pCallback->Proc, pCallback->pObject, pCallback->Param1,
              pCallback->Param2, pCallback->VBlankCount, pCallback->Flags,
              pCallback->VBlankOffset);

    if (kheadReadVblankIntrState(pGpu, pKernelHead) != NV_HEAD_VBLANK_INTR_UNAVAILABLE)
        vblankIntrIsBeingGenerated = NV_TRUE;

    if ( vblankIntrIsBeingGenerated || (pCallback->Flags & VBLANK_CALLBACK_FLAG_PERSISTENT) )
    {
        pCheck = pList;

        //
        // Check that the list doesn't become a circular queue of one element, which can happen in multichip, if the a method
        // is called twice on multiple devices. If this happens, we'll be in an infinite loop in the while(Callback) below.
        //
        while (NULL != pCheck)
        {
            if (pCheck == pCallback)
            {
                //
                // It is expected that we may try to add the same callback again, as we may not get a
                // dacdisable (which deletes callbacks) between modesets and/or dacenables.
                //
                NV_PRINTF(LEVEL_INFO,
                          "headAddVblankCallback: VblankCallback already on the Callback List\n");
                OktoAdd = NV_FALSE;
            }
            pCheck = pCheck->Next;
        }

        if (OktoAdd)
        {
            //
            // Best-effort test to verify that this callback is not already part of any callback list
            // (the test won't detect callbacks added twice at the end of two lists)
            //
            NV_ASSERT(pCallback->Next == NULL);

            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT)
            {
                // We set the target to the one that the caller supplied.
                Count = pCallback->VBlankCount;
            }
            else if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_TIMESTAMP)
            {
                // We don't know which vblank would correspond to the timestamp, so just add it to end of list.
                Count = 0xFFFFFFFF;
            }
            else if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_OFFSET)
            {
                // We set the target to the current plus the offset that the caller supplied.
                Count += pCallback->VBlankOffset;
                pCallback->VBlankCount = Count;

                // If we are persistent, we should convert the vblank offset flag to a vblank count flag.
                if (pCallback->Flags & VBLANK_CALLBACK_FLAG_PERSISTENT)
                {
                    pCallback->Flags &= ~VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_OFFSET;
                    pCallback->Flags |=  VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT;
                }
            }
            else if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT)
            {
                // We set the target to the current plus one (the next vblank)
                Count += 1;
                pCallback->VBlankCount = Count;
            }
            else
            {
                //
                // We set the target to the current plus one (the next vblank).
                // We use this case when we dont know the request or legacy support.
                //
                Count += 1;
                pCallback->VBlankCount = Count;
            }

            // These are now guaranteed to be sorted by VBlank
            pPrev = NULL;
            pNext = pList;

            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_PROMOTE_TO_FRONT)
            {
                // To the front of the group that shares the same 'VBlankCount' value
                while ((NULL != pNext) && (Count > pNext->VBlankCount))
                {
                    pPrev = pNext;
                    pNext = pNext->Next;
                }
            }
            else
            {
                // To the back of the group that shares the same 'VBlankCount' value
                while ((NULL != pNext) && (Count >= pNext->VBlankCount))
                {
                    pPrev = pNext;
                    pNext = pNext->Next;
                }
            }

            // Are we at the head?
            if (pPrev == NULL)
            {
                pCallback->Next = pList;

                if (pCallback->Flags & VBLANK_CALLBACK_FLAG_LOW_LATENCY)
                {
                    pKernelHead->Vblank.Callback.pListLL = pCallback;
                }
                else
                {
                    pKernelHead->Vblank.Callback.pListNL = pCallback;
                }
            }
            else // In the middle or tail
            {
                pPrev->Next = pCallback;
                pCallback->Next = pNext;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "VBlankCallback discarded in dacCRTCAddVblankCallback to avoid infinite loop\n");
        }
    }
    else
    {
        // call it now
        if (pCallback->Proc)
        {
            NV_PRINTF(LEVEL_INFO,
                      "headAddVblankCallback: immediate invocation\n");
            pCallback->bImmediateCallback = NV_TRUE;

            // Force it to appear to be on the correct VBlankCount
            pCallback->VBlankCount = Count;
            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_USER)
            {
                // This is a user call back, they don't get a pointer to our pDev or Object data structs.
                pCallback->Proc(NULL,
                                NULL,
                                pCallback->Param1,
                                pCallback->Param2,
                                pCallback->Status);
            }
            else
            {
                //
                // this callback was scheduled when a trace was being conducted,
                // turn tracing back to make sure that we record this callback's
                // register operations too, so the trace will be complete
                // DON'T LOG USER CALLBACKS, not RM activity.  (plus the tracing system
                // requires a pDev ptr to find its own data structures)
                //
                pCallback->Proc(pGpu,
                                pCallback->pObject,
                                pCallback->Param1,
                                pCallback->Param2,
                                pCallback->Status);
            }
        }
    }

    // After all of that, if at least one callback is scheduled, head is enabled and the vblank is AVAILABLE, enable it now.
    if (vblankIntrIsBeingGenerated)
    {
        if ( (pKernelHead->Vblank.Callback.pListLL) ||
             (pKernelHead->Vblank.Callback.pListNL) )
        {
            if (kheadReadVblankIntrState(pGpu, pKernelHead) != NV_HEAD_VBLANK_INTR_ENABLED)
            {
                kheadWriteVblankIntrState(pGpu, pKernelHead, NV_HEAD_VBLANK_INTR_ENABLED);
                NV_PRINTF(LEVEL_INFO,
                          "headAddVblankCallback: Changed vblank stat to ENABLED\n");
            }
        }
    }
}

void
kheadPauseVblankCbNotifications_IMPL
(
    OBJGPU         *pGpu,
    KernelHead     *pKernelHead,
    VBLANKCALLBACK *pCallback
)
{
    VBLANKCALLBACK  *pList   = NULL;
    NvBool           bShouldDisable = NV_TRUE;

    // Cache the requested queue and its current vblank count
    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_LOW_LATENCY)
    {
        pList = pKernelHead->Vblank.Callback.pListLL;
    }
    else
    {
        pList = pKernelHead->Vblank.Callback.pListNL;
    }


    VBLANKCALLBACK *pPrev = pList;

    while (pPrev)
    {
        if (pPrev->bIsVblankNotifyEnable  == NV_TRUE)
        {
            bShouldDisable = NV_FALSE;
            break;
        }
        pPrev = pPrev->Next;
    }

    if(bShouldDisable)
    {
        kheadWriteVblankIntrState(pGpu, pKernelHead, NV_HEAD_VBLANK_INTR_AVAILABLE);
    }
}
void
kheadDeleteVblankCallback_IMPL
(
    OBJGPU         *pGpu,
    KernelHead     *pKernelHead,
    VBLANKCALLBACK *pCallback
)
{
    VBLANKCALLBACK  *pList   = NULL;
    NvBool           enabled = NV_FALSE;
    NvU32            Count;

    // Cache the requested queue and its current vblank count
    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_LOW_LATENCY)
    {
        pList = pKernelHead->Vblank.Callback.pListLL;
        Count = pKernelHead->Vblank.Counters.LowLatency;
    }
    else
    {
        pList = pKernelHead->Vblank.Callback.pListNL;
        Count = pKernelHead->Vblank.Counters.NormLatency;
    }

    // Disable VBlank (if it is even on) while we scan/process the callback list
    enabled = kheadReadVblankIntrEnable_HAL(pGpu, pKernelHead);

    if (enabled)
    {
        kheadWriteVblankIntrState(pGpu, pKernelHead, NV_HEAD_VBLANK_INTR_AVAILABLE);
    }

    // Search the list and remove this Callback entry
    if (pList == pCallback)
    {
        //
        // Found it.
        // Unlink it now. If we call it, it may try to add itself again, and wont be able to.
        if (pCallback->Flags & VBLANK_CALLBACK_FLAG_LOW_LATENCY)
        {
            pKernelHead->Vblank.Callback.pListLL = pCallback->Next;
        }
        else
        {
            pKernelHead->Vblank.Callback.pListNL = pCallback->Next;
        }

        //
        // Should the callback be executed as part of the object destroy?
        // (safe to do, since we already hold the necessary lock).
        //
        if ( (pCallback->Proc) &&
             (pCallback->Flags & VBLANK_CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP) )
        {
            // Force it to appear to be on the correct VBlankCount
            pCallback->VBlankCount = Count;

            // This is a user call back, they don't get a pointer to our pDev or Object data structs.
            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_USER)
            {
                pCallback->Proc(NULL,
                                NULL,
                                pCallback->Param1,
                                pCallback->Param2,
                                pCallback->Status);
            }
            else
            {
                pCallback->Proc(pGpu,
                                pCallback->pObject,
                                pCallback->Param1,
                                pCallback->Param2,
                                pCallback->Status);
            }
        }
    }
    else
    {
        VBLANKCALLBACK *pPrev = pList;

        while (pPrev)
        {
            if (pPrev->Next == pCallback)
            {
                //
                // Found it.
                // Should the callback be executed as part of the object destroy?
                // (safe to do, since we already hold the necessary lock).
                //
                if ( (pCallback->Proc) &&
                     (pCallback->Flags & VBLANK_CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP) )
                {
                    // Force it to appear to be on the correct VBlankCount
                    pCallback->VBlankCount = Count;

                    // This is a user call back, they don't get a pointer to our pDev or Object data structs.
                    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_USER)
                    {
                        pCallback->Proc(NULL,
                                        NULL,
                                        pCallback->Param1,
                                        pCallback->Param2,
                                        pCallback->Status);
                    }
                    else
                    {
                        pCallback->Proc(pGpu,
                                        pCallback->pObject,
                                        pCallback->Param1,
                                        pCallback->Param2,
                                        pCallback->Status);
                    }
                }

                pPrev->Next = pCallback->Next;
                break;
            }
            pPrev = pPrev->Next;
        }
    }
    pCallback->Next = NULL;

    // Check to see if there are no callbacks scheduled for this head
    if (!(pKernelHead->Vblank.Callback.pListLL) &&
        !(pKernelHead->Vblank.Callback.pListNL) )
    {
        //
        // Since there are no callbacks scheduled, then we don't need
        // to reenable anything.
        //
        enabled = NV_FALSE;
    }

    // Restore VBlank enable
    if (enabled)
    {
        kheadWriteVblankIntrState(pGpu, pKernelHead, NV_HEAD_VBLANK_INTR_ENABLED);
    }
}

void
kheadProcessVblankCallbacks_IMPL
(
    OBJGPU   *pGpu,
    KernelHead  *pKernelHead,
    NvU32     state
)
{
    KernelDisplay    *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    OBJTMR           *pTmr      = GPU_GET_TIMER(pGpu);
    VBLANKCALLBACK   *pCallback = NULL;
    VBLANKCALLBACK   *pNext     = NULL;
    VBLANKCALLBACK  **ppPrev    = NULL;
    NvBool            done      = NV_FALSE;
    NvBool            bQueueDpc = NV_FALSE;
    NvU32             newstate;
    NvU32             Count     = 0;
    NvU64             time      = 0;

    // If the caller failed to spec which queue, figure they wanted all of them
    if ((state & VBLANK_STATE_PROCESS_ALL_CALLBACKS) == 0)
    {
        state |= VBLANK_STATE_PROCESS_ALL_CALLBACKS;
    }

    // Keep a local copy we can mess with
    newstate = state;

    // We may have more then one queue to process, so this is the main loop.
    while (!done)
    {
        // Select the next queue to process. Give priority to the low latency folks.
        if (newstate & VBLANK_STATE_PROCESS_LOW_LATENCY)
        {
            // We dont want to come back here again.
            newstate &= ~VBLANK_STATE_PROCESS_LOW_LATENCY;

            // Grab the low latency queue and vblank count
            pCallback = pKernelHead->Vblank.Callback.pListLL;
            ppPrev    = &pKernelHead->Vblank.Callback.pListLL;
            Count     = pKernelHead->Vblank.Counters.LowLatency;
        }
        else if (newstate & VBLANK_STATE_PROCESS_NORMAL_LATENCY)
        {
            // We dont want to come back here again.
            newstate &= ~VBLANK_STATE_PROCESS_NORMAL_LATENCY;

            // Grab the normal latency queue and vblank count
            pCallback = pKernelHead->Vblank.Callback.pListNL;
            ppPrev    = &pKernelHead->Vblank.Callback.pListNL;
            Count     = pKernelHead->Vblank.Counters.NormLatency;
        }
        else
        {
            // We appear to have gone through all of the queues
            done = NV_TRUE;
        }

        // If we are not done, proces the next callback queue
        if (!done)
        {
            while (pCallback)
            {
                pNext = pCallback->Next;

                if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_TIMESTAMP)
                {
                    //
                    // Time stamp based call backs don't have a valid vblank count
                    // Vblank might be delayed and we might see only one vblank instead of two.
                    // So, count doesn't make sense in case of TS.
                    // And since the semantics is flip on vblank at TS >= TS specified, we can't
                    // use tmrCallbacks (they might flip outside vblank)
                    //

                    // Only re-read the time if we don't already know the result
                    if (time < pCallback->TimeStamp)
                    {
                        tmrGetCurrentTime(pTmr, &time);
                    }

                    if (time >= pCallback->TimeStamp)
                    {
                        //
                        // Unlink it before we call it.  Otherwise, it may
                        // try to add itself again, and wont be able to.
                        //
                        pCallback->Next = NULL;
                        *ppPrev = pNext;

                        // We better have something to do if we are wasting time reading TS
                        NV_ASSERT(pCallback->Proc);

                        //
                        // We need to avoid calling the _vblank_callback during Panel Replay
                        // as it will be taken care during _RG_VBLANK interrupt handling
                        //
                        if (pCallback != (VBLANKCALLBACK *)pKernelDisplay->pRgVblankCb && !pKernelDisplay->bIsPanelReplayEnabled)
                        {
                            pCallback->Proc(pGpu,
                                    pCallback->pObject,
                                    pCallback->Param1,
                                    pCallback->Param2,
                                    pCallback->Status);
                        }
                        bQueueDpc = NV_TRUE;
                    }
                    else
                    {
                        ppPrev = &pCallback->Next;
                    }
                }
                else
                {
                    if ( (pCallback->VBlankCount         == Count) ||
                         ((pCallback->VBlankCount + 1)   == Count) ||
                         (VBLANK_STATE_PROCESS_IMMEDIATE == state) )
                    {
                        pCallback->VBlankCount = Count;

                        //
                        // If this is not a persistent callback, unlink it.
                        // Otherwise, it may try to add itself again, and wont be able to add.
                        //

                        // Call the function now
                        if (pCallback->Proc)
                        {
                            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_USER)
                            {
                                //
                                // DON'T LOG USER CALLBACKS, not RM activity.  (plus the tracing system
                                // requires a pDev ptr to find its own data structures)
                                //

                                // This is a user call back, they don't get a pointer to our pDev or Object data structs.
                                pCallback->Proc(NULL,
                                                NULL,
                                                pCallback->Param1,
                                                pCallback->Param2,
                                                pCallback->Status);
                                bQueueDpc = NV_TRUE;
                            }
                            else
                            {
                                //
                                // This callback was scheduled when a trace was being conducted,
                                // turn tracing back to make sure that we record this callback's
                                // register operations too, so the trace will be complete
                                //
                                pCallback->Proc(pGpu,
                                                pCallback->pObject,
                                                pCallback->Param1,
                                                pCallback->Param2,
                                                pCallback->Status);
                                bQueueDpc = NV_TRUE;
                            }
                        }

                        // If this is a persistent callback make sure to updates its time to run if we are not multichip and not the last chip
                        if ( (pCallback->Flags & VBLANK_CALLBACK_FLAG_PERSISTENT) )
                        {
                            //
                            // So, it appears there are those that like to update vblank counts and such within the callback.
                            // This is fine I suppose, but we dont promise that this order is sorted then.
                            // Anyway, it may be that the callbacker updated the vblank offset also, so update that now.
                            // We should never see an OFFSET and PERSISTENT within the process loop.
                            //
                            if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_OFFSET)
                            {
                                // We set the target to the current plus the offset that the caller supplied.
                                pCallback->VBlankCount = Count + pCallback->VBlankOffset;

                                // We are persistent, so we should convert the vblank offset flag to a vblank count flag.
                                pCallback->Flags &= ~VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_OFFSET;
                                pCallback->Flags |=  VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT;
                            }

                            //
                            // If the vblank count has already been specified, we don't need to increment the
                            // the vblank count.
                            //
                            if ( !(pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT) )
                            {
                                pCallback->VBlankCount = Count + 1;
                            }

                            // Leave in callback chain.
                            ppPrev = &pCallback->Next;
                        }
                        else
                        {
                            //
                            // Yes, the proper way to terminate a persistent callback from within a callback is
                            // to make it non-persistant.  This is what the cursor functions do, and so we should
                            // check again after the callback.
                            //
                            pCallback->Next = NULL;
                            *ppPrev  = pNext;
                        }
                    }
                       // This condition arises at wrap time which is about every 331 days at 150 Hz
                    else
                    {
                        //
                        // MK: A callback may increase it's vblank count as part of it's execution. Since the
                        // callback list is only sorted at insertion time, this can render the list
                        // unsorted. So, we need to read the remaining entries in the list.
                        //
                        ppPrev = &pCallback->Next;

                    }
                }

                pCallback = pNext;
            }
        }
    }

    if (bQueueDpc)
    {
        osQueueDpc(pGpu);
    }

    // After all of that, if the callback lists are null and the vblank is ENABLED, move it to AVAILABLE now.
    if (!(pKernelHead->Vblank.Callback.pListLL) &&
        !(pKernelHead->Vblank.Callback.pListNL) )
    {
        if (kheadReadVblankIntrState(pGpu, pKernelHead) == NV_HEAD_VBLANK_INTR_ENABLED)
        {
            kheadWriteVblankIntrState(pGpu, pKernelHead, NV_HEAD_VBLANK_INTR_AVAILABLE);

            NV_PRINTF(LEVEL_INFO,
                      "Changed vblank state on head %d to AVAILABLE\n",
                      pKernelHead->PublicId);
        }
    }
}
