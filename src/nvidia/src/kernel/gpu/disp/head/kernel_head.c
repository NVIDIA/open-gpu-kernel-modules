/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "gpu/timer/objtmr.h"

NV_STATUS
kheadConstruct_IMPL(KernelHead *pKernelHead)
{
    pKernelHead->Vblank.IntrState = NV_HEAD_VBLANK_INTR_UNAVAILABLE;
    return NV_OK;
}

NvU32
kheadGetVblankTotalCounter_IMPL
(
    KernelHead *pKernelHead
)
{
    return pKernelHead->Vblank.Counters.Total;
}

void
kheadSetVblankTotalCounter_IMPL
(
    KernelHead *pKernelHead,
    NvU32    counter
)
{
    pKernelHead->Vblank.Counters.Total = counter;
}

NvU32
kheadGetVblankLowLatencyCounter_IMPL
(
    KernelHead *pKernelHead
)
{
    return pKernelHead->Vblank.Counters.LowLatency;
}

void
kheadSetVblankLowLatencyCounter_IMPL
(
    KernelHead *pKernelHead,
    NvU32    counter
)
{
    pKernelHead->Vblank.Counters.LowLatency = counter;
}

NvU32
kheadGetVblankNormLatencyCounter_IMPL
(
    KernelHead *pKernelHead
)
{
    return pKernelHead->Vblank.Counters.NormLatency;
}

void
kheadSetVblankNormLatencyCounter_IMPL
(
    KernelHead *pKernelHead,
    NvU32    counter
)
{
    pKernelHead->Vblank.Counters.NormLatency = counter;
}

static NvBool
kheadIsVblankCallbackDue
(
    VBLANKCALLBACK *pCallback,
    NvU32           state,
    NvU64           time,
    NvU32           vblankCount
)
{
    if (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_TIMESTAMP)
    {
        //
        // Time stamp based call backs don't have a valid vblank count
        // vblank might be delayed and we might see only one vblank instead of two.
        // so, count doesn't make sense in case of TS.
        // and since the semantics is flip on vblank at TS >= TS specified, we can't
        // use tmrCallbacks (they might flip outside vblank)
        //
        return (time >= pCallback->TimeStamp);
    }
    else
    {
        //
        // These are now guaranteed to be sorted by VBlank
        // and, now all have a VBlankCount to make processing simpler
        // in this function, 'due' means "the next time the queue's counter is incremented,
        // will it be time to process this callback?"  This definition requires us to add 1 to
        // the current vblankCount during the comparison.
        //
        if (VBLANK_STATE_PROCESS_IMMEDIATE & state)
        {
            return NV_TRUE;
        }

        // Persistent callbacks that want to run every vblank
        if ((pCallback->Flags & VBLANK_CALLBACK_FLAG_PERSISTENT) && (pCallback->Flags & VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT))
        {
            return NV_TRUE;
        }

        // Every other callback whose time has come.
        if (pCallback->VBlankCount == 1+vblankCount)
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

NvU32
kheadCheckVblankCallbacksQueued_IMPL
(
    OBJGPU  *thisGpu,
    KernelHead *pKernelHead,
    NvU32    state,
    NvU32   *expiring
)
{
    OBJTMR           *pTmr;
    NvU64             time;
    NvU32             queues = 0;

    pTmr = GPU_GET_TIMER(thisGpu);
    tmrGetCurrentTime(pTmr, &time);

    if (expiring)
    {
        *expiring = 0;
    }
    //
    // return a union of queues (represented by VBLANK_STATE_PROCESS_XXX_LATENCY flags,)
    // that are nonempty, i.e. have at least one callback.
    // optionally, also return (via 'expiring', when non-NULL) which of those non-empty queues contain
    // callbacks that are due to be processed, the next time that queue's counter gets incremented.
    //
    if ( (pKernelHead->Vblank.Callback.pListLL) &&
         (state & VBLANK_STATE_PROCESS_LOW_LATENCY) )
    {
        queues |= VBLANK_STATE_PROCESS_LOW_LATENCY;

        if (expiring)
        {
            NvU32 vblankCount;
            VBLANKCALLBACK *pCallback;

            vblankCount = pKernelHead->Vblank.Counters.LowLatency;
            pCallback = pKernelHead->Vblank.Callback.pListLL;

            do
            {
                if (kheadIsVblankCallbackDue(pCallback, state, time, vblankCount))
                {
                    *expiring |= VBLANK_STATE_PROCESS_LOW_LATENCY;
                }
                pCallback = pCallback->Next;
            }
            while (pCallback && !(*expiring & VBLANK_STATE_PROCESS_LOW_LATENCY));
        }
    }

    if ( (pKernelHead->Vblank.Callback.pListNL) &&
         (state & VBLANK_STATE_PROCESS_NORMAL_LATENCY) )
    {
        queues |= VBLANK_STATE_PROCESS_NORMAL_LATENCY;

        if (expiring)
        {
            NvU32 vblankCount;
            VBLANKCALLBACK *pCallback;

            vblankCount = pKernelHead->Vblank.Counters.NormLatency;
            pCallback = pKernelHead->Vblank.Callback.pListNL;

            do
            {
                if (kheadIsVblankCallbackDue(pCallback, state, time, vblankCount))
                {
                    *expiring |= VBLANK_STATE_PROCESS_NORMAL_LATENCY;
                }

                pCallback = pCallback->Next;
            }
            while (pCallback && !(*expiring & VBLANK_STATE_PROCESS_NORMAL_LATENCY));
        }
    }

    return queues & state;
}
NvU32
kheadReadVblankIntrState_IMPL
(
    OBJGPU     *pGpu,
    KernelHead *pKernelHead
)
{
    // Check to make sure that our SW state grooves with the HW state
    if (kheadReadVblankIntrEnable_HAL(pGpu, pKernelHead) &&
            kheadGetDisplayInitialized_HAL(pGpu, pKernelHead))
    {
        // HW is enabled, check if SW state is not enabled
        if (pKernelHead->Vblank.IntrState != NV_HEAD_VBLANK_INTR_ENABLED)
        {
            pKernelHead->Vblank.IntrState = NV_HEAD_VBLANK_INTR_ENABLED;
        }
    }
    else
    {
        //
        // If HW is not enabled, SW state would depend on whether head is
        // driving display. Check for both the SW states and base the
        // SW state decision on head initialized state.
        // If head is initialized SW state should be AVAILABLE else
        // UNAVAILABLE.
        //
        if ((pKernelHead->Vblank.IntrState == NV_HEAD_VBLANK_INTR_ENABLED) ||
            (pKernelHead->Vblank.IntrState == NV_HEAD_VBLANK_INTR_UNAVAILABLE))
        {
            NvU32 state = NV_HEAD_VBLANK_INTR_UNAVAILABLE;

            //
            // We should say HW not enabled is AVAILABLE or UNAVAILABLE
            // So, we'll base the correct decision on whether or not
            // this head is driving any display.
            //
            if (kheadGetDisplayInitialized_HAL(pGpu, pKernelHead))
            {
                state = NV_HEAD_VBLANK_INTR_AVAILABLE;
            }

            if (state != pKernelHead->Vblank.IntrState)
            {
                pKernelHead->Vblank.IntrState = state;
            }
        }
        else if (pKernelHead->Vblank.IntrState == NV_HEAD_VBLANK_INTR_AVAILABLE)
        {
            //
            // If HW is not enabled and head is not driving any display then
            // the SW state should be UNAVAILABLE
            //
            if (!kheadGetDisplayInitialized_HAL(pGpu, pKernelHead))
            {
                pKernelHead->Vblank.IntrState = NV_HEAD_VBLANK_INTR_UNAVAILABLE;
            }
        }
    }

    return pKernelHead->Vblank.IntrState;
}

void
kheadWriteVblankIntrState_IMPL
(
    OBJGPU  *pGpu,
    KernelHead *pKernelHead,
    NvU32    newstate
)
{
    NvU32 previous;
    NvBool enablehw = NV_FALSE; // Dont update the hw by default
    NvBool updatehw = NV_FALSE; // Dont enable the hw by default

    // Get the previous state for various other stuff
    previous = pKernelHead->Vblank.IntrState;

    // Make sure we really support the requested next state
    if ( (newstate != NV_HEAD_VBLANK_INTR_UNAVAILABLE) &&
         (newstate != NV_HEAD_VBLANK_INTR_AVAILABLE)   &&
         (newstate != NV_HEAD_VBLANK_INTR_ENABLED) )
    {
        NV_PRINTF(LEVEL_ERROR, "Unknown state %x requested on head %d.\n",
                  newstate, pKernelHead->PublicId);
        return;
    }

    // Spew where we were and where we are going for tracking...
#if defined(DEBUG)

    NV_PRINTF(LEVEL_INFO, "Changing vblank state on pGpu=%p head %d: ", pGpu,
              pKernelHead->PublicId);

    switch(previous)
    {
        case NV_HEAD_VBLANK_INTR_UNAVAILABLE:
            NV_PRINTF(LEVEL_INFO, "UNAVAILABLE -> ");
            break;
        case NV_HEAD_VBLANK_INTR_AVAILABLE:
            NV_PRINTF(LEVEL_INFO, "AVAILABLE -> ");
            break;
        case NV_HEAD_VBLANK_INTR_ENABLED:
            NV_PRINTF(LEVEL_INFO, "ENABLED -> ");
            break;
        default:
            NV_PRINTF(LEVEL_INFO, "UNKNOWN -> ");
            break;
    }

    switch(newstate)
    {
        case NV_HEAD_VBLANK_INTR_UNAVAILABLE:
            NV_PRINTF(LEVEL_INFO, "UNAVAILABLE\n");
            break;
        case NV_HEAD_VBLANK_INTR_AVAILABLE:
            NV_PRINTF(LEVEL_INFO, "AVAILABLE\n");
            break;
        case NV_HEAD_VBLANK_INTR_ENABLED:
            NV_PRINTF(LEVEL_INFO, "ENABLED\n");
            break;
        default:
            NV_PRINTF(LEVEL_INFO, "UNKNOWN\n");
            break;
    }

#endif

    // Move to the new state
    switch(newstate)
    {
        // Move to the unavailable state. This has an implied disabled state.
        case NV_HEAD_VBLANK_INTR_UNAVAILABLE:

            // If the hw is on, turn it off
            if (previous == NV_HEAD_VBLANK_INTR_ENABLED)
            {
                enablehw = NV_FALSE;
                updatehw = NV_TRUE;
            }
            break;

        // Move to the available state. This has an implied disabled state.
        case NV_HEAD_VBLANK_INTR_AVAILABLE:

            // If the hw is on, turn it off
            if (previous == NV_HEAD_VBLANK_INTR_ENABLED)
            {
                enablehw = NV_FALSE;
                updatehw = NV_TRUE;
            }
            break;

        // Move to the enabled state. This has an implied available state.
        case NV_HEAD_VBLANK_INTR_ENABLED:

            // If the hw was off, turn it on
            if (previous != NV_HEAD_VBLANK_INTR_ENABLED)
            {
                enablehw = NV_TRUE;
                updatehw = NV_TRUE;
            }
            break;

        default:
            // We REALLY should never get here with the correct filtering above.
            NV_PRINTF(LEVEL_ERROR, "Unknown state %x requested on head %d.\n",
                      newstate, pKernelHead->PublicId);
            DBG_BREAKPOINT();
            return;
            break;
    }

    // Update the sw state
    pKernelHead->Vblank.IntrState = newstate;

    // Update the hw
    if (updatehw)
    {
        kheadWriteVblankIntrEnable_HAL(pGpu, pKernelHead, enablehw);

    }
}

void
kheadProcessRgLineCallbacks_KERNEL
(
    OBJGPU       *pGpu,
    KernelHead   *pKernelHead,
    NvU32         head,
    NvU32        *headIntrMask,
    NvU32        *clearIntrMask,
    NvBool        bIsIrqlIsr
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32 rgIntrMask[MAX_RG_LINE_CALLBACKS_PER_HEAD] = {headIntr_RgLineA, headIntr_RgLineB};
    NvU32 rgIntr;

    for (rgIntr = 0; rgIntr < MAX_RG_LINE_CALLBACKS_PER_HEAD; rgIntr++)
    {
        if (*headIntrMask & rgIntrMask[rgIntr])
        {
            kdispInvokeRgLineCallback(pKernelDisplay, head, rgIntr, bIsIrqlIsr);
            *clearIntrMask |= rgIntrMask[rgIntr];
        }
    }
}

