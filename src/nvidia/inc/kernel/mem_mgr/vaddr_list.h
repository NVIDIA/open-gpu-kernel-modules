/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *  Structure to track virtual memory mapping
 */

#ifndef VADDR_LIST_H
#define VADDR_LIST_H

#include "containers/map.h"
#include "mem_mgr/vaspace.h"

/*!
 *  Map info
 */
typedef struct
{
    /*!
     *  Indicate whether caller should release the VA.
     *  Default is set to TRUE. i.e caller should free the VA.
     *  Use vaListSetManaged() to change the value.
     */
    NvBool bRelease;
}VADDR_LIST_INFO;

/*!
 *  Virtual memory info
 */
typedef struct
{
    /*! virtual address */
    NvU64  vAddr;
    /*! No: of active references to the virtual address */
    NvU64  refCnt;
    /*!
     *  To track common properties.
     *  Key 0 (pVAS == NULL) is used to store this info
     */
    VADDR_LIST_INFO *pVaListInfo;
}VA_INFO;

/*!
 *  Dictionary that tracks active virtual memory mappings.
 *  Indexed by the vaspace object pointer.
 */
MAKE_MAP(VA_LIST, VA_INFO);

/*! Init the tracker object */
NV_STATUS vaListInit(VA_LIST *);
/*! Init the tracker object */
void vaListDestroy(VA_LIST *);
/*!
 *  Set VA lifecycle property.
 *  TRUE means caller should free the VA. e.g RM managed mappings
 *  FALSE means caller shouldn't free the VA. e.g UVM or KMD managed mappings.
 */
NV_STATUS vaListSetManaged(VA_LIST *, NvBool bManaged);
NvBool vaListGetManaged(VA_LIST *);
/*! Add a vas mapping to the tracker */
NV_STATUS vaListAddVa(VA_LIST *, OBJVASPACE *, NvU64 vaddr);
/*! Remove a vas mapping to the tracker */
NV_STATUS vaListRemoveVa(VA_LIST *, OBJVASPACE *);
/*! Get the vas mapping */
NV_STATUS vaListFindVa(VA_LIST *, OBJVASPACE *, NvU64 *vaddr);
/*! Get the vas refCount */
NV_STATUS vaListGetRefCount(VA_LIST *, OBJVASPACE *, NvU64 *refCount);
#endif // VADDR_LIST_H
