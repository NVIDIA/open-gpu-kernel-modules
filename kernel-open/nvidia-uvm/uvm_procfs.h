/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

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

#ifndef __UVM_PROCFS_H__
#define __UVM_PROCFS_H__

#include "uvm_extern_decl.h"
#include "uvm_forward_decl.h"
#include "uvm_linux.h"
#include "nv-procfs.h"
#include "conftest.h"

NV_STATUS uvm_procfs_init(void);
void uvm_procfs_exit(void);

// Is procfs enabled at all?
static bool uvm_procfs_is_enabled(void)
{
#if defined(CONFIG_PROC_FS)
    return true;
#else
    return false;
#endif
}

// Is debug procfs enabled? This indicates that debug procfs files should be
// created.
static bool uvm_procfs_is_debug_enabled(void)
{
    return uvm_enable_debug_procfs != 0;
}

struct proc_dir_entry *uvm_procfs_get_gpu_base_dir(void);
struct proc_dir_entry *uvm_procfs_get_cpu_base_dir(void);

int uvm_procfs_open_callback(void);
void uvm_procfs_close_callback(void);

// Helper for printing into a seq_file if it's not NULL and UVM_DBG_PRINT
// otherwise. Useful when sharing a print function for both debug output and
// procfs output.
#define UVM_SEQ_OR_DBG_PRINT(seq_file, format, ...)             \
    do {                                                        \
        if (seq_file != NULL)                                   \
            seq_printf(seq_file, format, ##__VA_ARGS__);        \
        else                                                    \
            UVM_DBG_PRINT(format, ##__VA_ARGS__);               \
    } while (0)

#if defined(CONFIG_PROC_FS)

// Defer PM lock acquisition until the respective read() callback
// is invoked, to ensure the lock is acquired and released by
// the same thread.  Else the lock tracking validation code must
// be disabled for this lock, which is undesirable. As a result, 
// lockless macro is used below. See bug 2594854 for additional
// information.
#define UVM_DEFINE_SINGLE_PROCFS_FILE(name) \
    NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY_WITHOUT_LOCK(name)
#endif

#endif // __UVM_PROCFS_H__
