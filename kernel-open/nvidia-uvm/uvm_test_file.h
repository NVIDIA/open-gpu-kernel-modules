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

#ifndef __UVM_TEST_FILE_H__
#define __UVM_TEST_FILE_H__

#include "uvm_lock.h"
#include "uvm_test_ioctl.h"
#include <linux/radix-tree.h>

// A UVM test file is a simple file type used for unit testing but not
// production. Currently it provides PFN-based mmap to system pages, but can be
// extended for other purposes.
//
// Each file contains a struct page per file offset, freshly allocated and
// zeroed when that offset is first mmap'd. The page(s) persist until file
// release.

typedef struct
{
    // This tree contains the struct page * for each file offset, if any
    struct radix_tree_root page_tree;

    // Lock protecting page_tree
    uvm_mutex_t mutex;
} uvm_test_file_t;


NV_STATUS uvm_test_file_initialize(UVM_TEST_FILE_INITIALIZE_PARAMS *params, struct file *filp);
int uvm_test_file_mmap(uvm_test_file_t *test_file, struct vm_area_struct *vma);
NV_STATUS uvm_test_file_unmap(UVM_TEST_FILE_UNMAP_PARAMS *params, struct file *filp);
void uvm_test_file_release(struct file *filp, uvm_test_file_t *test_file);

#endif // __UVM_TEST_FILE_H__
