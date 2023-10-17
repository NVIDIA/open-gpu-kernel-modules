/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvkms-types.h>
#include <nvkms-utils.h>

#include <class/clc37a.h>
#include <class/clc57a.h>
#include <class/clc67a.h>

static void WaitForFreeSpace(NVDevEvoPtr pDevEvo,
                             NVC37ADispCursorImmControlPio *pEvoCursorControl)
{
    /*
     * Wait for Free to be non-zero, indicating there is space to push a method.
     * The only case where Free is expected to be zero is when display
     * frontend (FE) hardware is processing a previous method.
     * .1s should be more than enough time to wait for that.
     */
    NvU64 startTime = 0;
    const NvU64 timeout = 100000; /* 0.1 seconds */

    do {
        if (pEvoCursorControl->Free != 0) {
            return;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            break;
        }

        nvkms_yield();

    } while (TRUE);

    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                     "Timed out waiting for cursor PIO space");
}

static void MoveCursorC3(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                         NvS16 x, NvS16 y)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    NVC37ADispCursorImmControlPio *pEvoCursorControl =
        pEvoSubDev->cursorPio[head];

    WaitForFreeSpace(pDevEvo, pEvoCursorControl);
    pEvoCursorControl->SetCursorHotSpotPointOut[0] =
            DRF_NUM(C37A, _SET_CURSOR_HOT_SPOT_POINT_OUT, _X, x) |
            DRF_NUM(C37A, _SET_CURSOR_HOT_SPOT_POINT_OUT, _Y, y);

    WaitForFreeSpace(pDevEvo, pEvoCursorControl);
    pEvoCursorControl->Update =
            DRF_DEF(C37A, _UPDATE, _FLIP_LOCK_PIN, _LOCK_PIN_NONE);
}

static void ReleaseElvC3(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    NVC37ADispCursorImmControlPio *pEvoCursorControl =
        pEvoSubDev->cursorPio[head];

    WaitForFreeSpace(pDevEvo, pEvoCursorControl);
    pEvoCursorControl->Update =
            DRF_DEF(C37A, _UPDATE, _FLIP_LOCK_PIN, _LOCK_PIN_NONE) |
            DRF_DEF(C37A, _UPDATE, _RELEASE_ELV, _TRUE);
}

NVEvoCursorHAL nvEvoCursorC3 = {
    NVC37A_CURSOR_IMM_CHANNEL_PIO,              /* klass */
    MoveCursorC3,                               /* MoveCursor */
    ReleaseElvC3,                               /* ReleaseElv */
    {                                           /* caps */
        256,                                    /* maxSize */
    },
};

NVEvoCursorHAL nvEvoCursorC5 = {
    NVC57A_CURSOR_IMM_CHANNEL_PIO,              /* klass */
    MoveCursorC3,                               /* MoveCursor */
    ReleaseElvC3,                               /* ReleaseElv */
    {                                           /* caps */
        256,                                    /* maxSize */
    },
};

NVEvoCursorHAL nvEvoCursorC6 = {
    NVC67A_CURSOR_IMM_CHANNEL_PIO,              /* klass */
    MoveCursorC3,                               /* MoveCursor */
    ReleaseElvC3,                               /* ReleaseElv */
    {                                           /* caps */
        256,                                    /* maxSize */
    },
};

