/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl84A0_h_
#define _cl84A0_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/*
 * Class definitions for creating a memory descriptor from a list of page numbers
 * in RmAllocMemory.   No memory is allocated: only a memory descriptor and
 * memory object are created for later use in other calls.  These classes
 * are used by vGPU to create references to memory assigned to a guest VM.
 * In all cases, the list is passed as reference, in the pAddress argument
 * of RmAllocMemory, to a Nv01MemoryList structure (cast to a void **).
 */

/* List of system memory physical page numbers */
#define  NV01_MEMORY_LIST_SYSTEM                                   (0x00000081)
/* List of frame buffer physical page numbers */
#define  NV01_MEMORY_LIST_FBMEM                                    (0x00000082)
/* List of page numbers relative to the start of the specified object */
#define  NV01_MEMORY_LIST_OBJECT                                   (0x00000083)

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

/*
 * NV_MEMORY_LIST_ALLOCATION_PARAMS - Allocation params to create memory list
 * through NvRmAlloc.
 */
typedef struct
{
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
    NvU32   reserved_0;     /* reserved: must be 0 */
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
    NvP64   pageNumberList NV_ALIGN_BYTES(8);
    NvU64   limit NV_ALIGN_BYTES(8);
    NvU32   flagsOs02;
} NV_MEMORY_LIST_ALLOCATION_PARAMS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl84A0_h_ */
