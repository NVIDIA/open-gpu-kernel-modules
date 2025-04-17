/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nv-memdbg.h"
#include "nv-linux.h"

/* track who's allocating memory and print out a list of leaked allocations at
 * teardown.
 */

#define NV_MEM_LOGGER_STACK_TRACE 0

#if defined(NV_STACK_TRACE_PRESENT) && defined(NV_MEM_LOGGER) && defined(DEBUG)
#define NV_MEM_LOGGER_STACK_TRACE 1
#endif

typedef struct {
    struct rb_node rb_node;
    void *addr;
    NvU64 size;
    NvU32 line;
    const char *file;
#if NV_MEM_LOGGER_STACK_TRACE == 1
    unsigned long stack_trace[32];
#endif
} nv_memdbg_node_t;

struct
{
    struct rb_root rb_root;
    NvU64 untracked_bytes;
    NvU64 num_untracked_allocs;
    nv_spinlock_t lock;
} g_nv_memdbg;

void nv_memdbg_init(void)
{
    NV_SPIN_LOCK_INIT(&g_nv_memdbg.lock);
    g_nv_memdbg.rb_root = RB_ROOT;
}

static nv_memdbg_node_t *nv_memdbg_node_entry(struct rb_node *rb_node)
{
    return rb_entry(rb_node, nv_memdbg_node_t, rb_node);
}

static void nv_memdbg_insert_node(nv_memdbg_node_t *new)
{
    nv_memdbg_node_t *node;
    struct rb_node **rb_node = &g_nv_memdbg.rb_root.rb_node;
    struct rb_node *rb_parent = NULL;

    while (*rb_node)
    {
        node = nv_memdbg_node_entry(*rb_node);

        WARN_ON(new->addr == node->addr);

        rb_parent = *rb_node;

        if (new->addr < node->addr)
            rb_node = &(*rb_node)->rb_left;
        else
            rb_node = &(*rb_node)->rb_right;
    }

    rb_link_node(&new->rb_node, rb_parent, rb_node);
    rb_insert_color(&new->rb_node, &g_nv_memdbg.rb_root);
}

static nv_memdbg_node_t *nv_memdbg_remove_node(void *addr)
{
    nv_memdbg_node_t *node = NULL;
    struct rb_node *rb_node = g_nv_memdbg.rb_root.rb_node;

    while (rb_node)
    {
        node = nv_memdbg_node_entry(rb_node);
        if (addr == node->addr)
            break;
        else if (addr < node->addr)
            rb_node = rb_node->rb_left;
        else
            rb_node = rb_node->rb_right;
    }

    WARN_ON(!node || node->addr != addr);

    rb_erase(&node->rb_node, &g_nv_memdbg.rb_root);
    return node;
}

void nv_memdbg_add(void *addr, NvU64 size, const char *file, int line)
{
    nv_memdbg_node_t *node;
    unsigned long flags;

    if (addr == NULL)
    {
        return;
    }

    /* If node allocation fails, we can still update the untracked counters */
    node = kmalloc(sizeof(*node),
                   NV_MAY_SLEEP() ? NV_GFP_KERNEL : NV_GFP_ATOMIC);
    if (node)
    {
        node->addr = addr;
        node->size = size;
        node->file = file;
        node->line = line;

#if NV_MEM_LOGGER_STACK_TRACE == 1
        memset(node->stack_trace, '\0', sizeof(node->stack_trace));

        stack_trace_save(node->stack_trace, NV_ARRAY_ELEMENTS(node->stack_trace), 0);
#endif
    }

    NV_SPIN_LOCK_IRQSAVE(&g_nv_memdbg.lock, flags);

    if (node)
    {
        nv_memdbg_insert_node(node);
    }
    else
    {
        ++g_nv_memdbg.num_untracked_allocs;
        g_nv_memdbg.untracked_bytes += size;
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&g_nv_memdbg.lock, flags);
}

void nv_memdbg_remove(void *addr, NvU64 size, const char *file, int line)
{
    nv_memdbg_node_t *node;
    unsigned long flags;

    if (addr == NULL)
    {
        return;
    }

    NV_SPIN_LOCK_IRQSAVE(&g_nv_memdbg.lock, flags);

    node = nv_memdbg_remove_node(addr);
    if (!node)
    {
        WARN_ON(g_nv_memdbg.num_untracked_allocs == 0);
        WARN_ON(g_nv_memdbg.untracked_bytes < size);
        --g_nv_memdbg.num_untracked_allocs;
        g_nv_memdbg.untracked_bytes -= size;
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&g_nv_memdbg.lock, flags);

    if (node)
    {
        if ((size != 0) && (node->size != size))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: size mismatch on free: %llu != %llu\n",
                size, node->size);
            if (node->file)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM:     allocation: 0x%p @ %s:%d\n",
                    node->addr, node->file, node->line);
            }
            else
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM:     allocation: 0x%p\n",
                    node->addr);
            }
            os_dbg_breakpoint();
        }

        kfree(node);
    }
}

void nv_memdbg_exit(void)
{
    nv_memdbg_node_t *node;
    NvU64 leaked_bytes = 0, num_leaked_allocs = 0;

    if (!RB_EMPTY_ROOT(&g_nv_memdbg.rb_root))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: list of leaked memory allocations:\n");
    }

    while (!RB_EMPTY_ROOT(&g_nv_memdbg.rb_root))
    {
        node = nv_memdbg_node_entry(rb_first(&g_nv_memdbg.rb_root));

        leaked_bytes += node->size;
        ++num_leaked_allocs;

        if (node->file)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM:    %llu bytes, 0x%p @ %s:%d\n",
                node->size, node->addr, node->file, node->line);
        }
        else
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM:    %llu bytes, 0x%p\n",
                node->size, node->addr);
        }

#if NV_MEM_LOGGER_STACK_TRACE == 1
        stack_trace_print(node->stack_trace, NV_ARRAY_ELEMENTS(node->stack_trace), 1);
#endif

        rb_erase(&node->rb_node, &g_nv_memdbg.rb_root);
        kfree(node);
    }

    /* If we failed to allocate a node at some point, we may have leaked memory
     * even if the tree is empty */
    if (num_leaked_allocs > 0 || g_nv_memdbg.num_untracked_allocs > 0)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: total leaked memory: %llu bytes in %llu allocations\n",
            leaked_bytes + g_nv_memdbg.untracked_bytes,
            num_leaked_allocs + g_nv_memdbg.num_untracked_allocs);

        if (g_nv_memdbg.num_untracked_allocs > 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM:                      %llu bytes in %llu allocations untracked\n",
                g_nv_memdbg.untracked_bytes, g_nv_memdbg.num_untracked_allocs);
        }
    }
}
