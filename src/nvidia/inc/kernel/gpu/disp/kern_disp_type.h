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

#ifndef KERN_DISP_TYPE_H
#define KERN_DISP_TYPE_H

/******************************************************************************
*
*  Defines display type enums that can be used in the KernelDisplay object.
*
******************************************************************************/
#define NV_PDISP_CHN_NUM_ANY 0x7F
#define DISP_ACCL_NONE                        (0x00)
#define DISP_ACCL_IGNORE_PI                   NVBIT(0)
#define DISP_ACCL_SKIP_NOTIF                  NVBIT(1)
#define DISP_ACCL_SKIP_SEMA                   NVBIT(2)
#define DISP_ACCL_IGNORE_INTERLOCK            NVBIT(3)
#define DISP_ACCL_IGNORE_FLIPLOCK             NVBIT(4)
#define DISP_ACCL_TRASH_ONLY                  NVBIT(5)
#define DISP_ACCL_TRASH_AND_ABORT             NVBIT(6)
#define DISP_ACCL_SKIP_SYNCPOINT              NVBIT(7)
#define DISP_ACCL_IGNORE_TIMESTAMP            NVBIT(8)
#define DISP_ACCL_IGNORE_MGI                  NVBIT(9)
#define DISP_ACCL_DISABLE_PUTPTR_WRITE        NVBIT(16)
#define DISP_ACCL_LOCK_PIO_FIFO               NVBIT(16)
#define DISP_ACCL_DISABLE_INTR_DURING_SHTDWN  NVBIT(17)
#define DISP_ACCL_ALL                         ~(DISP_ACCL_NONE)

typedef enum
{
    dispChnClass_Curs,
    dispChnClass_Ovim,
    dispChnClass_Base,
    dispChnClass_Core,
    dispChnClass_Ovly,
    dispChnClass_Winim,
    dispChnClass_Win,
    dispChnClass_Any,
    dispChnClass_Supported
} DISPCHNCLASS;

typedef enum
{
    dispChnState_Idle,
    dispChnState_Wrtidle,
    dispChnState_Empty,
    dispChnState_Flushed,
    dispChnState_Busy,
    dispChnState_Dealloc,
    dispChnState_DeallocLimbo,
    dispChnState_Limbo1,
    dispChnState_Limbo2,
    dispChnState_Fcodeinit1,
    dispChnState_Fcodeinit2,
    dispChnState_Fcode,
    dispChnState_Vbiosinit1,
    dispChnState_Vbiosinit2,
    dispChnState_Vbiosoper,
    dispChnState_Unconnected,
    dispChnState_Initialize1,
    dispChnState_Initialize2,
    dispChnState_Shutdown1,
    dispChnState_Shutdown2,
    dispChnState_Supported
} DISPCHNSTATE;

enum DISPLAY_ICC_BW_CLIENT
{
    DISPLAY_ICC_BW_CLIENT_RM,
    DISPLAY_ICC_BW_CLIENT_EXT,  // DD or MODS
    NUM_DISPLAY_ICC_BW_CLIENTS
};

typedef enum
{
    dispMemoryTarget_physNVM,
    dispMemoryTarget_physPCI,
    dispMemoryTarget_physPCICoherent
} DISPMEMORYTARGET;

typedef struct
{
    NvU64               addr;
    DISPMEMORYTARGET    memTarget;
    NvBool              valid;
} VGAADDRDESC;

//
// Map HW channel state to SW channel state
//
typedef struct
{
    NvU32 hwChannelState;
    DISPCHNSTATE dispChnState;
} CHNSTATEMAP;

#endif // #ifndef KERN_DISP_TYPE_H
