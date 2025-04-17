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

#include "uvm_fd_type.h"
#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_va_space.h"
#include "uvm_test_file.h"

uvm_fd_type_t uvm_fd_type(struct file *filp, void **ptr_val)
{
    unsigned long uptr;
    uvm_fd_type_t type;
    void *ptr;

    UVM_ASSERT(uvm_file_is_nvidia_uvm(filp));

    uptr = atomic_long_read_acquire((atomic_long_t *) (&filp->private_data));
    type = (uvm_fd_type_t)(uptr & UVM_FD_TYPE_MASK);
    ptr = (void *)(uptr & ~UVM_FD_TYPE_MASK);
    BUILD_BUG_ON(UVM_FD_COUNT > UVM_FD_TYPE_MASK + 1);

    switch (type) {
        case UVM_FD_UNINITIALIZED:
        case UVM_FD_INITIALIZING:
            UVM_ASSERT(!ptr);
            break;

        case UVM_FD_VA_SPACE:
            UVM_ASSERT(ptr);
            BUILD_BUG_ON(__alignof__(uvm_va_space_t) < (1UL << UVM_FD_TYPE_BITS));
            break;

        case UVM_FD_MM:
            UVM_ASSERT(ptr);
            BUILD_BUG_ON(__alignof__(struct file) < (1UL << UVM_FD_TYPE_BITS));
            break;

        case UVM_FD_TEST:
            UVM_ASSERT(ptr);
            BUILD_BUG_ON(__alignof__(uvm_test_file_t) < (1UL << UVM_FD_TYPE_BITS));
            break;

        default:
            UVM_ASSERT(0);
    }

    if (ptr_val)
        *ptr_val = ptr;

    return type;
}

void *uvm_fd_get_type(struct file *filp, uvm_fd_type_t type)
{
    void *ptr;

    UVM_ASSERT(uvm_file_is_nvidia_uvm(filp));

    if (uvm_fd_type(filp, &ptr) == type)
        return ptr;
    else
        return NULL;
}

uvm_fd_type_t uvm_fd_type_init_cas(struct file *filp)
{
    long old = atomic_long_cmpxchg((atomic_long_t *)&filp->private_data, UVM_FD_UNINITIALIZED, UVM_FD_INITIALIZING);
    return (uvm_fd_type_t)(old & UVM_FD_TYPE_MASK);
}

NV_STATUS uvm_fd_type_init(struct file *filp)
{
    uvm_fd_type_t old = uvm_fd_type_init_cas(filp);

    if (old != UVM_FD_UNINITIALIZED)
        return NV_ERR_IN_USE;

    return NV_OK;
}

void uvm_fd_type_set(struct file *filp, uvm_fd_type_t type, void *ptr)
{
    void *tmp_ptr;
    UVM_ASSERT(uvm_fd_type(filp, &tmp_ptr) == UVM_FD_INITIALIZING);
    UVM_ASSERT(!tmp_ptr);

    if (type == UVM_FD_UNINITIALIZED)
        UVM_ASSERT(!ptr);

    UVM_ASSERT(((uintptr_t)ptr & type) == 0);

    atomic_long_set_release((atomic_long_t *)&filp->private_data, (uintptr_t)ptr | type);
}
