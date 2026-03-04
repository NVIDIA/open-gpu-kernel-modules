/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl84a0_deprecated_h_
#define _cl84a0_deprecated_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * List structure of NV01_MEMORY_LIST_* classes
 *
 * The pageNumber array is variable in length, with pageCount elements,
 * so the allocated size of the structure must reflect that.
 *
 * FBMEM items apply only to NV01_MEMORY_LIST_FBMEM and to
 * NV01_MEMORY_LIST_OBJECT when the underlying object is
 * FBMEM (must be zero for other cases)
 *
 * Nv01MemoryList is deprecated. NV_MEMORY_LIST_ALLOCATION_PARAMS should be used
 * instead.
 */
typedef struct Nv01MemoryListRec {
    NvHandle    hClient;    /* client to which object belongs
                             * (may differ from client creating the mapping).
                             * May be NV01_NULL_OBJECT, in which case client
                             * handle is used */
    NvHandle    hParent;    /* device with which object is associated.
                             * Must be NV01_NULL_OBJECT if hClient is NV01_NULL_OBJECT.
                             * Must not be NV01_NULL_OBJECT if hClient is
                             * not NV01_NULL_OBJECT. */
    NvHandle    hObject;    /* object to which pages are relative
                             * (NV01_NULL_OBJECT for NV01_MEMORY_LIST_SYSTEM
                             *  and NV01_MEMORY_LIST_FBMEM) */
    NvHandle   hHwResClient;/* client associated with the backdoor vnc surface*/
    NvHandle   hHwResDevice;/* device associated to the bacdoor vnc surface*/
    NvHandle   hHwResHandle;/* handle to hardware resources allocated to
                             * backdoor vnc surface*/
    NvU32   pteAdjust;      /* offset of data in first page */
    NvU32   type;           /* FBMEM: NVOS32_TYPE_* */
    NvU32   flags;          /* FBMEM: NVOS32_ALLOC_FLAGS_* */
    NvU32   attr;           /* FBMEM: NVOS32_ATTR_* */
    NvU32   attr2;          /* FBMEM: NVOS32_ATTR2_* */
    NvU32   height;         /* FBMEM: height in pixels */
    NvU32   width;          /* FBMEM: width in pixels */
    NvU32   format;         /* FBMEM: memory kind */
    NvU32   comprcovg;      /* FBMEM: compression coverage */
    NvU32   zcullcovg;      /* FBMEM: Z-cull coverage */
    NvU32   pageCount;      /* count of elements in pageNumber array */
    NvU32   heapOwner;      /* heap owner information from client */
    NvU32   reserved_1;     /* reserved: must be 0 */
    NvU64   NV_DECLARE_ALIGNED(guestId,8);
                            /* ID of the guest VM. e.g., domain ID in case of Xen */
    NvU64   NV_DECLARE_ALIGNED(rangeBegin,8);
                            /* preferred VA range start address */
    NvU64   NV_DECLARE_ALIGNED(rangeEnd,8);
                            /* preferred VA range end address */
    NvU32   pitch;
    NvU32   ctagOffset;
    NvU64   size;
    NvU64   align;
    NvU64   pageNumber[1];  /* variable length array of page numbers */
} Nv01MemoryList;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl84a0_deprecated_h_ */
