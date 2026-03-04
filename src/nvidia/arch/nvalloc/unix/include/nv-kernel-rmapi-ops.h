/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_KERNEL_RMAPI_OPS_H_
#define _NV_KERNEL_RMAPI_OPS_H_

/*
 * Define the RMAPI provided to kernel-level RM clients.
 *
 * Kernel-level RM clients should populate nvidia_kernel_rmapi_ops_t
 * by assigning nvidia_kernel_rmapi_ops_t::op and the corresponding
 * parameter structure in nvidia_kernel_rmapi_ops_t's params union.
 * Then, pass a pointer to the nvidia_kernel_rmapi_ops_t to
 * rm_kernel_rmapi_op().
 */

#include "nvtypes.h"
#include "nvos.h"

typedef struct {
    NvU32 op; /* One of the NV0[14]_XXXX operations listed below. */

    union {
        NVOS00_PARAMETERS free;                /* NV01_FREE */
        NVOS02_PARAMETERS allocMemory64;       /* NV01_ALLOC_MEMORY */
        NVOS64_PARAMETERS alloc;               /* NV04_ALLOC */
        NVOS32_PARAMETERS *pVidHeapControl;    /* NV04_VID_HEAP_CONTROL */
        NVOS33_PARAMETERS mapMemory;           /* NV04_MAP_MEMORY */
        NVOS34_PARAMETERS unmapMemory;         /* NV04_UNMAP_MEMORY */
        NVOS39_PARAMETERS allocContextDma2;    /* NV04_ALLOC_CONTEXT_DMA */
        NVOS46_PARAMETERS mapMemoryDma;        /* NV04_MAP_MEMORY_DMA */
        NVOS47_PARAMETERS unmapMemoryDma;      /* NV04_UNMAP_MEMORY_DMA */
        NVOS49_PARAMETERS bindContextDma;      /* NV04_BIND_CONTEXT_DMA */
        NVOS54_PARAMETERS control;             /* NV04_CONTROL*/
        NVOS55_PARAMETERS dupObject;           /* NV04_DUP_OBJECT */
        NVOS57_PARAMETERS share;               /* NV04_SHARE */
        NVOS61_PARAMETERS addVblankCallback;   /* NV04_ADD_VBLANK_CALLBACK */
    } params;
} nvidia_kernel_rmapi_ops_t;

#endif /* _NV_KERNEL_RMAPI_OPS_H_ */
