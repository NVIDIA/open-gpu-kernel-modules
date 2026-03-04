/*******************************************************************************
    Copyright (c) 2024 NVIDIA Corporation

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

#include "uvm_common.h"
#include "uvm_fd_type.h"
#include "uvm_linux.h"
#include "uvm_lock.h"
#include "uvm_kvmalloc.h"
#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_test_file.h"

#include <linux/mm.h>
#include <linux/mm_types.h>

NV_STATUS uvm_test_file_initialize(UVM_TEST_FILE_INITIALIZE_PARAMS *params, struct file *filp)
{
    uvm_test_file_t *test_file;
    NV_STATUS status;

    status = uvm_fd_type_init(filp);
    if (status != NV_OK)
        return status;

    test_file = uvm_kvmalloc_zero(sizeof(*test_file));
    if (!test_file)
        goto error;

    INIT_RADIX_TREE(&test_file->page_tree, NV_UVM_GFP_FLAGS);
    uvm_mutex_init(&test_file->mutex, UVM_LOCK_ORDER_LEAF);

    uvm_fd_type_set(filp, UVM_FD_TEST, test_file);
    return NV_OK;

error:
    uvm_fd_type_set(filp, UVM_FD_UNINITIALIZED, NULL);
    return status;
}

// If index (page offset) already has a page allocated, return that page.
// Otherwise allocate, zero, insert, and return the page.
static struct page *test_file_get_page(uvm_test_file_t *test_file, NvU64 index)
{
    struct page *page;

    uvm_assert_mutex_locked(&test_file->mutex);

    page = radix_tree_lookup(&test_file->page_tree, index);
    if (!page) {
        page = alloc_page(NV_UVM_GFP_FLAGS | __GFP_ZERO);
        if (page && radix_tree_insert(&test_file->page_tree, index, page)) {
            __free_page(page);
            page = NULL;
        }
    }

    return page;
}

static struct vm_operations_struct uvm_vm_ops_empty;

int uvm_test_file_mmap(uvm_test_file_t *test_file, struct vm_area_struct *vma)
{
    NvU64 i;
    int ret = 0;

    uvm_mutex_lock(&test_file->mutex);

    // Unlike managed or semaphore vmas, we don't have any state to track
    // associated with the vma. We'll free the pages at file release time. If
    // the user wants to fork() or mremap(), we shouldn't care. The only reason
    // to assign vm_ops instead of leaving it NULL is to prevent
    // vma_is_anonymous() from returning true on this vma, which would cause
    // migrate_pageable_vma() to attempt to migrate instead of falling back to
    // uvm_populate_pageable_vma().
    vma->vm_ops = &uvm_vm_ops_empty;

    for (i = 0; i < vma_pages(vma); i++) {
        struct page *page = test_file_get_page(test_file, vma->vm_pgoff + i);
        if (!page) {
            ret = -ENOMEM;
            break;
        }

        // Let remap_pfn_range() set up our vma flags
        ret = remap_pfn_range(vma, vma->vm_start + i * PAGE_SIZE, page_to_pfn(page), 1, vma->vm_page_prot);
        if (ret) {
            // The kernel will unmap any already-mapped pages on mmap() failure
            break;
        }
    }

    uvm_mutex_unlock(&test_file->mutex);
    return ret;
}

NV_STATUS uvm_test_file_unmap(UVM_TEST_FILE_UNMAP_PARAMS *params, struct file *filp)
{
    if (uvm_fd_type(filp, NULL) != UVM_FD_TEST)
        return NV_ERR_ILLEGAL_ACTION;

    unmap_mapping_range(filp->f_mapping, params->offset, params->length, 1);
    return NV_OK;
}

void uvm_test_file_release(struct file *filp, uvm_test_file_t *test_file)
{
    void **slot;
    struct radix_tree_iter iter;

    radix_tree_for_each_slot(slot, &test_file->page_tree, &iter, 0) {
        void *ret;
        struct page *page = *slot;
        UVM_ASSERT(page);
        ret = radix_tree_delete(&test_file->page_tree, iter.index);
        UVM_ASSERT(ret == page);
        __free_page(page);
    }

    uvm_kvfree(test_file);

    // Allocated by uvm_open for all fd types
    uvm_kvfree(filp->f_mapping);
}
