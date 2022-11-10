/*******************************************************************************
    Copyright (c) 2013 NVIDIA Corporation

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

#include "uvm_linux.h"
#if UVM_CGROUP_ACCOUNTING_SUPPORTED()
#include <linux/memcontrol.h>
#include <linux/sched/mm.h>
#endif

//
// uvm_linux.c
//
// This file, along with conftest.h and umv_linux.h, helps to insulate
// the (out-of-tree) UVM driver from changes to the upstream Linux kernel.
//

#if UVM_CGROUP_ACCOUNTING_SUPPORTED()
void uvm_memcg_context_start(uvm_memcg_context_t *context, struct mm_struct *mm)
{
    memset(context, 0, sizeof(*context));
    if (!mm)
        return;

    context->new_memcg = get_mem_cgroup_from_mm(mm);
    context->old_memcg = set_active_memcg(context->new_memcg);
}

void uvm_memcg_context_end(uvm_memcg_context_t *context)
{
    if (!context->new_memcg)
        return;

    set_active_memcg(context->old_memcg);
    mem_cgroup_put(context->new_memcg);
}
#endif
