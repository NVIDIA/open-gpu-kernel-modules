/*******************************************************************************
    Copyright (c) 2016-2021 NVIDIA Corporation

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

#include "uvm_pte_batch.h"
#include "uvm_hal.h"

static bool uvm_gpu_phys_address_eq(uvm_gpu_phys_address_t pa1, uvm_gpu_phys_address_t pa2)
{
    return pa1.address == pa2.address && pa1.aperture == pa2.aperture;
}

void uvm_pte_batch_begin(uvm_push_t *push, uvm_pte_batch_t *batch)
{
    memset(batch, 0, sizeof(*batch));

    batch->membar = UVM_MEMBAR_GPU;
    batch->push = push;
}

static void uvm_pte_batch_flush_ptes_inline(uvm_pte_batch_t *batch)
{
    uvm_gpu_address_t inline_data_addr;
    uvm_gpu_t *gpu = uvm_push_get_gpu(batch->push);
    size_t ptes_size = batch->pte_count * batch->pte_entry_size;

    UVM_ASSERT(batch->pte_count != 0);
    UVM_ASSERT(batch->inlining);
    UVM_ASSERT_MSG(ptes_size == uvm_push_inline_data_size(&batch->inline_data), "ptes size %zd inline data %zd\n",
            ptes_size, uvm_push_inline_data_size(&batch->inline_data));

    batch->inlining = false;
    inline_data_addr = uvm_push_inline_data_end(&batch->inline_data);

    uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
    gpu->parent->ce_hal->memcopy(batch->push,
                                 uvm_mmu_gpu_address(gpu, batch->pte_first_address),
                                 inline_data_addr,
                                 ptes_size);
}

static void uvm_pte_batch_flush_ptes_memset(uvm_pte_batch_t *batch)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(batch->push);
    uvm_gpu_address_t addr = uvm_mmu_gpu_address(gpu, batch->pte_first_address);
    NvU32 i;

    UVM_ASSERT(batch->pte_count != 0);
    UVM_ASSERT(!batch->inlining);

    for (i = 0; i < batch->pte_count; ++i) {
        uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        gpu->parent->ce_hal->memset_8(batch->push, addr, batch->pte_bits_queue[i], sizeof(NvU64));
        addr.address += batch->pte_entry_size;
    }
}

static void uvm_pte_batch_flush_ptes(uvm_pte_batch_t *batch)
{
    if (batch->pte_count == 0)
        return;

    if (batch->inlining)
        uvm_pte_batch_flush_ptes_inline(batch);
    else
        uvm_pte_batch_flush_ptes_memset(batch);

    batch->pte_count = 0;
}

static void uvm_pte_batch_write_consecutive_inline(uvm_pte_batch_t *batch, NvU64 pte_bits)
{
    size_t extra_size = batch->pte_entry_size - sizeof(pte_bits);

    UVM_ASSERT(extra_size < batch->pte_entry_size);
    UVM_ASSERT(batch->inlining);

    // Add the PTE bits
    uvm_push_inline_data_add(&batch->inline_data, &pte_bits, sizeof(pte_bits));

    // And zero out the rest of the entry if anything remaining
    if (extra_size != 0)
        memset(uvm_push_inline_data_get(&batch->inline_data, extra_size), 0, extra_size);
}

static void uvm_pte_batch_write_consecutive(uvm_pte_batch_t *batch, NvU64 pte_bits)
{
    if (batch->inlining) {
        uvm_pte_batch_write_consecutive_inline(batch, pte_bits);
    }
    else {
        UVM_ASSERT_MSG(batch->pte_count < UVM_PTE_BATCH_MAX_PTES, "pte_count %u\n", batch->pte_count);
        batch->pte_bits_queue[batch->pte_count] = pte_bits;
    }
    ++batch->pte_count;
}

static void pte_batch_begin_inline(uvm_pte_batch_t *batch)
{
    NvU32 i;

    UVM_ASSERT(!batch->inlining);

    batch->inlining = true;
    uvm_push_inline_data_begin(batch->push, &batch->inline_data);

    for (i = 0; i < batch->pte_count; ++i)
        uvm_pte_batch_write_consecutive_inline(batch, batch->pte_bits_queue[i]);
}

void uvm_pte_batch_write_ptes(uvm_pte_batch_t *batch, uvm_gpu_phys_address_t first_pte, NvU64 *pte_bits, NvU32 entry_size, NvU32 entry_count)
{
    NvU32 max_entries = UVM_PUSH_INLINE_DATA_MAX_SIZE / entry_size;

    // Updating PTEs in sysmem requires a sysmembar after writing them and
    // before any TLB invalidates.
    if (first_pte.aperture == UVM_APERTURE_SYS)
        batch->membar = UVM_MEMBAR_SYS;

    while (entry_count > 0) {
        NvU32 entries_this_time;

        uvm_pte_batch_flush_ptes(batch);
        pte_batch_begin_inline(batch);

        entries_this_time = min(max_entries, entry_count);
        uvm_push_inline_data_add(&batch->inline_data, pte_bits, entries_this_time * entry_size);

        batch->pte_entry_size = entry_size;
        batch->pte_first_address = first_pte;
        batch->pte_count = entries_this_time;

        pte_bits += entries_this_time * (entry_size / sizeof(*pte_bits));
        first_pte.address += entries_this_time * entry_size;
        entry_count -= entries_this_time;
    }
}

void uvm_pte_batch_write_pte(uvm_pte_batch_t *batch, uvm_gpu_phys_address_t pte, NvU64 pte_bits, NvU32 pte_size)
{
    uvm_gpu_phys_address_t consecutive_pte_address = batch->pte_first_address;
    bool needs_flush = false;
    consecutive_pte_address.address += batch->pte_count * pte_size;

    // Updating PTEs in sysmem requires a sysmembar after writing them and
    // before any TLB invalidates.
    if (pte.aperture == UVM_APERTURE_SYS)
        batch->membar = UVM_MEMBAR_SYS;

    // Note that pte_count and pte_entry_size can be zero for the first PTE.
    // That's ok as the first PTE will never need a flush.
    if ((batch->pte_count + 1) * batch->pte_entry_size > UVM_PUSH_INLINE_DATA_MAX_SIZE)
        needs_flush = true;

    if (!uvm_gpu_phys_address_eq(pte, consecutive_pte_address))
        needs_flush = true;

    if (batch->pte_entry_size != pte_size)
        needs_flush = true;

    if (needs_flush) {
        uvm_pte_batch_flush_ptes(batch);
        batch->pte_first_address = pte;
        batch->pte_entry_size = pte_size;
    }

    if (!batch->inlining && batch->pte_count == UVM_PTE_BATCH_MAX_PTES)
        pte_batch_begin_inline(batch);

    uvm_pte_batch_write_consecutive(batch, pte_bits);
}

void uvm_pte_batch_clear_ptes(uvm_pte_batch_t *batch, uvm_gpu_phys_address_t first_pte, NvU64 empty_pte_bits, NvU32 entry_size, NvU32 entry_count)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(batch->push);

    // TODO: Bug 1767241: Allow small clears to batch
    uvm_pte_batch_flush_ptes(batch);

    uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
    uvm_push_set_flag(batch->push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    gpu->parent->ce_hal->memset_8(batch->push,
                                  uvm_mmu_gpu_address(gpu, first_pte),
                                  empty_pte_bits,
                                  entry_size * entry_count);

    if (first_pte.aperture == UVM_APERTURE_SYS)
        batch->membar = UVM_MEMBAR_SYS;
}

void uvm_pte_batch_end(uvm_pte_batch_t *batch)
{
    uvm_pte_batch_flush_ptes(batch);
    uvm_hal_wfi_membar(batch->push, batch->membar);
}
