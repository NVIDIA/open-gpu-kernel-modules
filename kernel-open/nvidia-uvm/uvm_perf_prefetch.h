/*******************************************************************************
    Copyright (c) 2016-2019 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN hint OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_PERF_PREFETCH_H__
#define __UVM_PERF_PREFETCH_H__

#include "uvm_linux.h"
#include "uvm_processors.h"
#include "uvm_va_block_types.h"

typedef struct
{
    const uvm_page_mask_t *prefetch_pages_mask;

    uvm_processor_id_t residency;
} uvm_perf_prefetch_hint_t;

// Global initialization/cleanup functions
NV_STATUS uvm_perf_prefetch_init(void);
void uvm_perf_prefetch_exit(void);

// VA space Initialization/cleanup functions
NV_STATUS uvm_perf_prefetch_load(uvm_va_space_t *va_space);
void uvm_perf_prefetch_unload(uvm_va_space_t *va_space);

// Obtain a hint with the pages that may be prefetched in the block
uvm_perf_prefetch_hint_t uvm_perf_prefetch_get_hint(uvm_va_block_t *va_block,
                                                    const uvm_page_mask_t *new_residency_mask);

// Notify that the given mask of pages within region is going to migrate to
// the given residency. The caller must hold the va_space lock.
void uvm_perf_prefetch_prenotify_fault_migrations(uvm_va_block_t *va_block,
                                                  uvm_va_block_context_t *va_block_context,
                                                  uvm_processor_id_t new_residency,
                                                  const uvm_page_mask_t *migrate_pages,
                                                  uvm_va_block_region_t region);

#define UVM_PERF_PREFETCH_HINT_NONE()                       \
    (uvm_perf_prefetch_hint_t){ NULL, UVM_ID_INVALID }

#endif
