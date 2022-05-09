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

#include <class/cl917a.h>

static void MoveCursor90(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                         NvS16 x, NvS16 y)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    GK104DispCursorControlPio *pEvoCursorControl =
        pEvoSubDev->cursorPio[head];

    pEvoCursorControl->SetCursorHotSpotPointsOut[NVKMS_LEFT] =
            DRF_NUM(917A, _SET_CURSOR_HOT_SPOT_POINTS_OUT, _X, x) |
            DRF_NUM(917A, _SET_CURSOR_HOT_SPOT_POINTS_OUT, _Y, y);

    pEvoCursorControl->Update =
            DRF_DEF(917A, _UPDATE, _INTERLOCK_WITH_CORE, _DISABLE);
}

NVEvoCursorHAL nvEvoCursor91 = {
    NV917A_CURSOR_CHANNEL_PIO,                  /* klass */
    MoveCursor90,                               /* MoveCursor */
    NULL,                                       /* ReleaseElv */
    {                                           /* caps */
        256,                                    /* maxSize */
    },
};
