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

#ifndef __UVM_KVMALLOC_H__
#define __UVM_KVMALLOC_H__

#include "uvm_linux.h"
#include "uvm_test_ioctl.h"

// kmalloc is faster than vmalloc because it doesn't have to remap kernel
// virtual memory, but for that same reason it requires physically-contiguous
// memory. It also supports a native krealloc function which is missing in
// vmalloc.
//
// Therefore the uvm_kvmalloc APIs use kmalloc when possible, but will fall back
// to vmalloc when the allocation size exceeds this UVM_KMALLOC_THRESHOLD.
//
// This value is somewhat arbitrary. kmalloc can support allocations much larger
// than PAGE_SIZE, but the larger the size the higher the chances of allocation
// failure.
//
// This is in the header so callers can use it to inform their allocation sizes
// if they wish.
#define UVM_KMALLOC_THRESHOLD (4*PAGE_SIZE)

NV_STATUS uvm_kvmalloc_init(void);
void uvm_kvmalloc_exit(void);

// Allocating a size of 0 with any of these APIs returns ZERO_SIZE_PTR
void *__uvm_kvmalloc(size_t size, const char *file, int line, const char *function);
void *__uvm_kvmalloc_zero(size_t size, const char *file, int line, const char *function);

#define uvm_kvmalloc(__size) __uvm_kvmalloc(__size, __FILE__, __LINE__, __FUNCTION__)
#define uvm_kvmalloc_zero(__size) __uvm_kvmalloc_zero(__size, __FILE__, __LINE__, __FUNCTION__)

void uvm_kvfree(void *p);

// Follows standard realloc semantics:
// - uvm_kvrealloc(NULL, size) and uvm_kvrealloc(ZERO_SIZE_PTR, size) are each
//   equivalent to uvm_kvmalloc(size)
// - uvm_kvrealloc(p, 0) is the same as uvm_kvfree(p), and returns ZERO_SIZE_PTR
void *__uvm_kvrealloc(void *p, size_t new_size, const char *file, int line, const char *function);

#define uvm_kvrealloc(__p, __new_size) __uvm_kvrealloc(__p, __new_size, __FILE__, __LINE__, __FUNCTION__)

// Returns the allocation size for a prior allocation from uvm_kvmalloc,
// uvm_kvmalloc_zero, or uvm_kvrealloc. This may be more than the size requested
// in those calls, in which case the extra memory is safe to use.
//
// p must not be NULL.
size_t uvm_kvsize(void *p);

NV_STATUS uvm_test_kvmalloc(UVM_TEST_KVMALLOC_PARAMS *params, struct file *filp);

#endif // __UVM_KVMALLOC_H__
