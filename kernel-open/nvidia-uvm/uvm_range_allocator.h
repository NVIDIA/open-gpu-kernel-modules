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

#ifndef __UVM_RANGE_ALLOCATOR_H__
#define __UVM_RANGE_ALLOCATOR_H__

#include "uvm_range_tree.h"
#include "uvm_lock.h"

typedef struct {
    // Lock protecting the state of the range allocator
    uvm_spinlock_t lock;

    // Size of the range to allocate from
    NvU64 size;

    // Range tree tracking all the free ranges
    uvm_range_tree_t range_tree;
} uvm_range_allocator_t;

// A free range allocation
typedef struct {
    // The allocated start of the range
    NvU64 aligned_start;

    // A tree node allocated at the time of range allocation and used by the
    // range allocator when the range allocation is freed. This allows to
    // guarantee that uvm_range_allocator_free() always succeeds.
    uvm_range_tree_node_t *node;
} uvm_range_allocation_t;

// Initialize the range allocator with the given size
NV_STATUS uvm_range_allocator_init(NvU64 size, uvm_range_allocator_t *range_allocator);

// Deinitialize the range allocator
//
// All allocated ranges need to have been freed before the range allocator is deinitialized
void uvm_range_allocator_deinit(uvm_range_allocator_t *range_allocator);

// Alloc a free range of the given size and alignment
//
// Size needs to be greater or equal to 1.
// Alignment needs to be a power of 2 or 0. Alignment of 0 is converted into
// alignment of 1.
//
// On success, the start of the allocated range is returned in
// free_range_alloc->aligned_start.
NV_STATUS uvm_range_allocator_alloc(uvm_range_allocator_t *range_allocator, NvU64 size, NvU64 alignment, uvm_range_allocation_t *free_range_alloc);

// Free a previously allocated range
void uvm_range_allocator_free(uvm_range_allocator_t *range_allocator, uvm_range_allocation_t *range_alloc);

#endif // __UVM_RANGE_ALLOCATOR_H__
