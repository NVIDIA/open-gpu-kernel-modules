/*******************************************************************************
    Copyright (c) 2013-2025 NVIDIA Corporation

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

//
// uvm_linux.h
//
// This file, along with conftest.h and umv_linux.c, helps to insulate
// the (out-of-tree) UVM driver from changes to the upstream Linux kernel.
//
//

#ifndef _UVM_LINUX_H
#define _UVM_LINUX_H

#include "nvtypes.h"

#include "nv-time.h"

#define NV_BUILD_MODULE_INSTANCES 0
#include "nv-linux.h"

#include <linux/log2.h>
#if defined(NV_PRIO_TREE_PRESENT)
#include <linux/prio_tree.h>
#endif

#include <linux/jhash.h>
#include <linux/rwsem.h>
#include <linux/rbtree.h>
#include <linux/mm.h>
#include <asm/barrier.h>
#include <linux/atomic.h>

#include <asm/current.h>

#include <linux/random.h>           /* get_random_bytes()               */
#include <linux/radix-tree.h>       /* Linux kernel radix tree          */

#include <linux/file.h>             /* fget()                           */

#include <linux/percpu.h>
#include <linux/printk.h>
#include <linux/ratelimit.h>
#include <linux/sched/task_stack.h>
#include "linux/bitmap.h"
#include "linux/bitops.h"
#include "linux/gfp.h"
#include "linux/pagemap.h"
#include "linux/types.h"

#if !defined(NV_SG_DMA_PAGE_ITER_PRESENT)
#include <linux/scatterlist.h>
#endif

#include <linux/cpumask.h>
#include <linux/topology.h>

#if defined(NV_LINUX_DMA_DIRECT_H_PRESENT)
#include <linux/dma-direct.h>
#else
#include <asm/dma-mapping.h>
#endif

#include "nv-kthread-q.h"

    #define UVM_THREAD_AFFINITY_SUPPORTED() 1

    #if defined(CONFIG_ZONE_DEVICE) && defined(NV_MEMORY_DEVICE_COHERENT_PRESENT)
        #define UVM_CDMM_PAGES_SUPPORTED() 1
    #else
        #define UVM_CDMM_PAGES_SUPPORTED() 0
    #endif

    #if defined(CONFIG_HMM_MIRROR) && defined(CONFIG_DEVICE_PRIVATE) && defined(NV_MIGRATE_DEVICE_RANGE_PRESENT)
        #define UVM_IS_CONFIG_HMM() 1
    #else
        #define UVM_IS_CONFIG_HMM() 0
    #endif

// ATS prefetcher uses hmm_range_fault() to query residency information.
// hmm_range_fault() needs CONFIG_HMM_MIRROR. To detect racing CPU invalidates
// of memory regions while hmm_range_fault() is being called, MMU interval
// notifiers are needed.
    #if defined(CONFIG_HMM_MIRROR) && defined(NV_MMU_INTERVAL_NOTIFIER)
        #define UVM_HMM_RANGE_FAULT_SUPPORTED() 1
    #else
        #define UVM_HMM_RANGE_FAULT_SUPPORTED() 0
    #endif

    #define UVM_CAN_USE_MMU_NOTIFIERS() 1

//
// printk.h already defined pr_fmt, so we have to redefine it so the pr_*
// routines pick up our version
//
#undef pr_fmt
#define NVIDIA_UVM_PRETTY_PRINTING_PREFIX "nvidia-uvm: "
#define pr_fmt(fmt) NVIDIA_UVM_PRETTY_PRINTING_PREFIX fmt

// Dummy printing function that maintains syntax and format specifier checking
// but doesn't print anything and doesn't evaluate the print parameters. This is
// roughly equivalent to the kernel's no_printk function. We use this instead
// because:
// 1) no_printk was not available until 2.6.36
// 2) Until 4.5 no_printk was implemented as a static function, meaning its
//    parameters were always evaluated
#define UVM_NO_PRINT(fmt, ...)          \
    do {                                \
        if (0)                          \
            printk(fmt, ##__VA_ARGS__); \
    } while (0)

#define NV_UVM_GFP_FLAGS (GFP_KERNEL | __GFP_NOMEMALLOC)

#if defined(NV_GET_DEV_PAGEMAP_HAS_PGMAP_ARG)
    #define NV_GET_DEV_PAGEMAP(pfn) get_dev_pagemap(pfn, NULL)
#else
    #define NV_GET_DEV_PAGEMAP get_dev_pagemap
#endif

/* Return a nanosecond-precise value */
static inline NvU64 NV_GETTIME(void)
{
    struct timespec64 tm;

    ktime_get_raw_ts64(&tm);
    return (NvU64) timespec64_to_ns(&tm);
}

// atomic_long_read_acquire and atomic_long_set_release were added in commit
// b5d47ef9ea5c5fe31d7eabeb79f697629bd9e2cb ("locking/atomics: Switch to
// generated atomic-long") in v5.1 (2019-05-05).
// TODO: Bug 3849079: We always use these definitions on newer kernels.
#define atomic_long_read_acquire uvm_atomic_long_read_acquire
static inline long uvm_atomic_long_read_acquire(atomic_long_t *p)
{
    long val = atomic_long_read(p);
    smp_mb();
    return val;
}

#define atomic_long_set_release uvm_atomic_long_set_release
static inline void uvm_atomic_long_set_release(atomic_long_t *p, long v)
{
    smp_mb();
    atomic_long_set(p, v);
}

typedef struct
{
    struct mem_cgroup *new_memcg;
    struct mem_cgroup *old_memcg;
} uvm_memcg_context_t;

    // cgroup support requires set_active_memcg(). set_active_memcg() is an
    // inline function that requires int_active_memcg per-cpu symbol when called
    // from interrupt context. int_active_memcg is only exported by commit
    // c74d40e8b5e2a on >= 5.14 kernels.
    #if NV_IS_EXPORT_SYMBOL_PRESENT_int_active_memcg
        #define UVM_CGROUP_ACCOUNTING_SUPPORTED() 1
        #define NV_UVM_GFP_FLAGS_ACCOUNT              (NV_UVM_GFP_FLAGS | __GFP_ACCOUNT)

        // Begin a Cgroup accounting context.
        // All sysmem page allocations done with NV_UVM_ACCOUNT_GFP_FLAGS will be
        // charged to the mm's memory control group.
        //
        // If mm is NULL, the accounting context will not be switched. Please, note
        // that in this case, any allocations which include NV_UVM_ACCOUNT_GFP_FLAGS
        // will be charged to the currently active context.
        //
        // Locking: uvm_memcg_context_t does not maintain its own locking. Callers must
        //          ensure that concurrent calls do not operate on the same context.
        void uvm_memcg_context_start(uvm_memcg_context_t *context, struct mm_struct *mm);

        // End the Cgroup accounting context started with uvm_mem_memcg_context_start().
        // After this call, the previously active memory control group will be restored.
        //
        // Locking: Callers must ensure that concurrent calls do not operate on the same
        //          context.
        void uvm_memcg_context_end(uvm_memcg_context_t *context);
    #else // !NV_IS_EXPORT_SYMBOL_PRESENT_int_active_memcg
        #define UVM_CGROUP_ACCOUNTING_SUPPORTED() 0
        #define NV_UVM_GFP_FLAGS_ACCOUNT              (NV_UVM_GFP_FLAGS)

        static inline void uvm_memcg_context_start(uvm_memcg_context_t *context, struct mm_struct *mm)
        {
            return;
        }

        static inline void uvm_memcg_context_end(uvm_memcg_context_t *context)
        {
            return;
        }
    #endif // NV_IS_EXPORT_SYMBOL_PRESENT_int_active_memcg

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
  #include <asm/pgtable.h>
  #include <asm/pgtable_types.h>
#endif

// PAGE_KERNEL_NOENC is only defined on x86. Define it for all architectures so
// we don't have to wrap uses in #ifdefs.
#if !defined(PAGE_KERNEL_NOENC)
  #define PAGE_KERNEL_NOENC PAGE_KERNEL
#endif

// uvm_pgprot_decrypted is a GPL-aware version of pgprot_decrypted that returns
// the given input when UVM cannot use GPL symbols. Otherwise, the function is
// equivalent to pgprot_decrypted. UVM only depends on pgprot_decrypted when the
// driver is allowed to use GPL symbols: both AMD's SEV and Intel's TDX are only
// supported in conjunction with OpenRM.
//
// It is safe to invoke uvm_pgprot_decrypted in KVM + AMD SEV-SNP guests, even
// if the call is not required, because pgprot_decrypted(PAGE_KERNEL_NOENC) ==
// PAGE_KERNEL_NOENC.
static inline pgprot_t uvm_pgprot_decrypted(pgprot_t prot)
{
    return pgprot_decrypted(prot);

   return prot;
}

#if !defined(NV_SG_DMA_PAGE_ITER_PRESENT)
    // Added by commit d901b2760dc6c ("lib/scatterlist: Provide a DMA page
    // iterator") v5.0
    struct sg_dma_page_iter {
        struct sg_page_iter base;
    };

    #define uvm_sg_page_iter_dma_address(dma_iter)      \
        sg_page_iter_dma_address(&((dma_iter)->base))
#else
    #define uvm_sg_page_iter_dma_address(dma_iter)      \
        sg_page_iter_dma_address((dma_iter))
#endif

#if !defined(NV_FOR_EACH_SGTABLE_DMA_PAGE_PRESENT)
    // Added by commit 709d6d73c756 ("scatterlist: add generic wrappers for
    // iterating over sgtable objects") v5.7.
    #define UVM_FOR_EACH_SGTABLE_DMA_PAGE_PRESENT() 0

    static int sg_dma_page_count(struct scatterlist *sg);
    bool __sg_page_iter_dma_next(struct sg_dma_page_iter *dma_iter);

    #define for_each_sg_dma_page(sglist, dma_iter, dma_nents, pgoffset)         \
        for (__sg_page_iter_start(&(dma_iter)->base, sglist, dma_nents,         \
                                  pgoffset);                                    \
            __sg_page_iter_dma_next(dma_iter);)

    #define for_each_sgtable_dma_page(sgt, dma_iter, pgoffset)                  \
            for_each_sg_dma_page((sgt)->sgl, dma_iter, (sgt)->nents, pgoffset)
#else
    #define UVM_FOR_EACH_SGTABLE_DMA_PAGE_PRESENT() 1
#endif

#ifndef NV_PAGE_PGMAP_PRESENT
static inline struct dev_pagemap *page_pgmap(const struct page *page)
{
    return page->pgmap;
}
#endif

#endif // _UVM_LINUX_H
