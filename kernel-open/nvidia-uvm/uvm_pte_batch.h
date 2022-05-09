/*******************************************************************************
    Copyright (c) 2016 NVIDIA Corporation

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
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_PTE_BATCH_H__
#define __UVM_PTE_BATCH_H__

#include "uvm_forward_decl.h"
#include "uvm_hal_types.h"
#include "uvm_push.h"

// Max PTEs to queue up for memsets before switching to inline memcopy.
// Currently inline memcopy has to read the data from sysmem (that's where the
// pushbuffer is) adding some latency so it's not obvious that it's better than
// a memset that can just write the PTE bits to local vidmem. On the other hand,
// launching a CE memset operation per each PTE also adds latency and takes a
// lot of pushbuffer space.
//
// TODO: Bug 1767241: Benchmark the two approaches to see whether the current
//       number makes sense.
//
// TODO: Bug 1767241: If pushbuffer is ever moved to vidmem the tradeoffs can
//       change as inline memcopy would have lower latency.
#define UVM_PTE_BATCH_MAX_PTES 4

struct uvm_pte_batch_struct
{
    uvm_push_t *push;

    uvm_push_inline_data_t inline_data;
    bool inlining;

    uvm_gpu_phys_address_t pte_first_address;
    NvU32 pte_entry_size;
    NvU64 pte_bits_queue[UVM_PTE_BATCH_MAX_PTES];
    NvU32 pte_count;

    // A membar to be applied after all the PTE writes.
    // Starts out as UVM_MEMBAR_GPU and is promoted to UVM_MEMBAR_SYS if any of
    // the written PTEs are in sysmem.
    uvm_membar_t membar;
};

// Begin a PTE batch
void uvm_pte_batch_begin(uvm_push_t *push, uvm_pte_batch_t *batch);

// End a PTE batch
//
// This pushes all the queued up PTE operations, followed by a membar.
// The membar is chosen based on the aperture of all the written PTEs.
//
// This is guaranteed to push a Host WFI, so it can be followed immediately by a
// TLB invalidate.
void uvm_pte_batch_end(uvm_pte_batch_t *batch);

// Queue up a write of PTEs from a buffer
void uvm_pte_batch_write_ptes(uvm_pte_batch_t *batch,
        uvm_gpu_phys_address_t first_pte, NvU64 *pte_bits, NvU32 entry_size, NvU32 entry_count);

// Queue up a single PTE write
void uvm_pte_batch_write_pte(uvm_pte_batch_t *batch,
        uvm_gpu_phys_address_t pte, NvU64 pte_bits, NvU32 entry_size);

// Queue up a clear of PTEs
void uvm_pte_batch_clear_ptes(uvm_pte_batch_t *batch,
        uvm_gpu_phys_address_t first_pte, NvU64 pte_bits, NvU32 entry_size, NvU32 entry_count);

// A helper for a single call to uvm_pte_batch_write_ptes() that begin and ends the PTE batch internally
static void uvm_pte_batch_single_write_ptes(uvm_push_t *push,
        uvm_gpu_phys_address_t first_pte, NvU64 *pte_bits, NvU32 entry_size, NvU32 entry_count)
{
    uvm_pte_batch_t batch;
    uvm_pte_batch_begin(push, &batch);
    uvm_pte_batch_write_ptes(&batch, first_pte, pte_bits, entry_size, entry_count);
    uvm_pte_batch_end(&batch);
}

#endif // __UVM_PTE_BATCH_H__
