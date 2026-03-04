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

#endif // #ifndef KERN_DISP_TYPE_H
