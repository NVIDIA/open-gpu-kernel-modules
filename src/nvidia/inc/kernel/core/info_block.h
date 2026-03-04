/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _INFO_BLOCK_H_
#define _INFO_BLOCK_H_

#include "nvtypes.h"

//
// HAL privata data management.
//
typedef struct ENG_INFO_LINK_NODE *PENG_INFO_LINK_NODE;
typedef struct ENG_INFO_LINK_NODE ENG_INFO_LINK_NODE;


struct ENG_INFO_LINK_NODE
{
    NvU32   dataId;
    void    *infoBlock;
    PENG_INFO_LINK_NODE next;
};

void* getInfoPtr(PENG_INFO_LINK_NODE pHead, NvU32 dataId);
void* addInfoPtr(PENG_INFO_LINK_NODE *ppHead, NvU32 dataId, NvU32 size);
void  deleteInfoPtr(PENG_INFO_LINK_NODE * ppHead, NvU32 dataId);
NvBool testInfoPtr(PENG_INFO_LINK_NODE, NvU32 dataId);

#endif // _INFO_BLOCK_H_
