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

#ifndef __UVM_FD_TYPE_H__
#define __UVM_FD_TYPE_H__

#include "nvstatus.h"

typedef enum
{
    UVM_FD_UNINITIALIZED,
    UVM_FD_INITIALIZING,
    UVM_FD_VA_SPACE,
    UVM_FD_MM,
    UVM_FD_TEST,
    UVM_FD_COUNT
} uvm_fd_type_t;

// This should be large enough to fit the valid values from uvm_fd_type_t above.
// Note we can't use order_base_2(UVM_FD_COUNT) to define this because our code
// coverage tool fails when the preprocessor expands that to a huge mess of
// ternary operators.
#define UVM_FD_TYPE_BITS 3
#define UVM_FD_TYPE_MASK ((1UL << UVM_FD_TYPE_BITS) - 1)

struct file;

// Returns the type of data filp->private_data contains and if ptr_val != NULL
// returns the value of the pointer.
uvm_fd_type_t uvm_fd_type(struct file *filp, void **ptr_val);

// Returns the pointer stored in filp->private_data if the type matches,
// otherwise returns NULL.
void *uvm_fd_get_type(struct file *filp, uvm_fd_type_t type);

// Does atomic CAS on filp->private_data, expecting UVM_FD_UNINITIALIZED and
// swapping in UVM_FD_INITIALIZING. Returns the old type regardless of CAS
// success.
uvm_fd_type_t uvm_fd_type_init_cas(struct file *filp);

// Like uvm_fd_type_init_cas() but returns NV_OK on CAS success and
// NV_ERR_IN_USE on CAS failure.
NV_STATUS uvm_fd_type_init(struct file *filp);

// Assigns {type, ptr} to filp. filp's current type must be UVM_FD_INITIALIZING.
// If the new type is UVM_FD_UNINITIALIZED, ptr must be NULL.
void uvm_fd_type_set(struct file *filp, uvm_fd_type_t type, void *ptr);

#endif // __UVM_FD_TYPE_H__
