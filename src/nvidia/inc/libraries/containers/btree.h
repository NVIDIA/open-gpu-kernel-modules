/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _BTREE_H_
#define _BTREE_H_

/*********************** Balanced Tree data structure **********************\
*                                                                           *
* Module: BTREE.H                                                           *
*       API to BTREE routines.                                              *
*                                                                           *
\***************************************************************************/

//
// RED BLACK TREE structure.
//
#include "nvtypes.h"
#include "nvstatus.h"

typedef struct NODE
{
    // public:
    void             *Data;
    NvU64             keyStart;
    NvU64             keyEnd;

    // private:
    NvBool            isRed;            // !IsRed == IsBlack
    struct NODE      *parent;           // tree links
    struct NODE      *left;
    struct NODE      *right;

} NODE, *PNODE;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

NV_STATUS btreeInsert(PNODE, PNODE *);
NV_STATUS btreeUnlink(PNODE, PNODE *);
NV_STATUS btreeSearch(NvU64, PNODE *, PNODE);
NV_STATUS btreeEnumStart(NvU64, PNODE *, PNODE);
NV_STATUS btreeEnumNext(PNODE *, PNODE);
NV_STATUS btreeDestroyData(PNODE);
NV_STATUS btreeDestroyNodes(PNODE);

#endif // _BTREE_H_
