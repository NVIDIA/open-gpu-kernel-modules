/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Declarations for VGPU PMA Guest Scrubber Shared memory structures.
//
//   Description:
//       This module declares the shared memory structures for VGPU PMA Guest
//       Scrubber.
//
//******************************************************************************

#ifndef __vgpu_pma_guest_scrubber_h__
#define __vgpu_pma_guest_scrubber_h__

#include "gpu/mem_mgr/rm_page_size.h"

#define VGPU_GUEST_PMA_MAX_SCRUB_ITEMS                  4096
#define VGPU_GUEST_PMA_SCRUBBER_SHARED_BUFFER_SIZE      ((sizeof(VGPU_SCRUB_NODE) * VGPU_GUEST_PMA_MAX_SCRUB_ITEMS) + RM_PAGE_SIZE)
#define VGPU_GUEST_PMA_SCRUBBER_SHARED_BUFFER_PFNS      (NV_DIV_AND_CEIL(VGPU_GUEST_PMA_SCRUBBER_SHARED_BUFFER_SIZE, RM_PAGE_SIZE))

typedef struct VGPU_SCRUB_NODE {
    volatile NvU32                              workId;  // The 32 bit ID assigned to each work
    volatile NvU64                              base;    // The base address from which the scrub to start
    volatile NvU64                              size;    // The size of a scrub work
} VGPU_SCRUB_NODE;

typedef struct VGPU_GUEST_PMA_SCRUB_BUFFER_RING_HEADER {
    volatile NvU32                              lastSubmittedWorkId;
    volatile NvU32                              lastSWSemaphoreDone;
    volatile NvU64                              scrubberGetIdx;
    volatile NvU64                              scrubberPutIdx;
} VGPU_GUEST_PMA_SCRUB_BUFFER_RING_HEADER;

typedef struct VGPU_GUEST_PMA_SCRUB_BUFFER_RING {
    VGPU_GUEST_PMA_SCRUB_BUFFER_RING_HEADER    *pScrubBuffRingHeader;
    VGPU_SCRUB_NODE                            *pScrubList;
} VGPU_GUEST_PMA_SCRUB_BUFFER_RING;

#endif // __vgpu_pma_guest_scrubber_h__

