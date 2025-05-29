/*******************************************************************************
    Copyright (c) 2013-2021 NVIDIA Corporation

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

#if defined(NV_LINUX_LOG2_H_PRESENT)
#include <linux/log2.h>
#endif
#if defined(NV_PRIO_TREE_PRESENT)
#include <linux/prio_tree.h>
#endif

#include <linux/jhash.h>
#include <linux/rwsem.h>
#include <linux/rbtree.h>
#include <linux/mm.h>

#if defined(NV_ASM_BARRIER_H_PRESENT)
#include <asm/barrier.h>
#endif

#if defined(NV_LINUX_ATOMIC_H_PRESENT)
#include <linux/atomic.h>
#endif

#include <asm/current.h>

#include <linux/random.h>           /* get_random_bytes()               */
#include <linux/radix-tree.h>       /* Linux kernel radix tree          */

#include <linux/file.h>             /* fget()                           */

#include <linux/percpu.h>

#if defined(NV_LINUX_PRINTK_H_PRESENT)
#include <linux/printk.h>
#endif

#if defined(NV_LINUX_RATELIMIT_H_PRESENT)
#include <linux/ratelimit.h>
#endif

#if defined(NV_LINUX_SCHED_TASK_STACK_H_PRESENT)
#include <linux/sched/task_stack.h>
#endif

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

    #if defined(NV_CPUMASK_OF_NODE_PRESENT)
        #define UVM_THREAD_AFFINITY_SUPPORTED() 1
    #else
        #define UVM_THREAD_AFFINITY_SUPPORTED() 0
    #endif

// The ARM arch lacks support for cpumask_of_node() until kernel 4.7. It was
// added via commit1a2db300348b ("arm64, numa: Add NUMA support for arm64
// platforms.") Callers should either check UVM_THREAD_AFFINITY_SUPPORTED()
// prior to calling this function of be prepared to deal with a NULL CPU
// mask.
static inline const struct cpumask *uvm_cpumask_of_node(int node)
{
#ifdef NV_CPUMASK_OF_NODE_PRESENT
    return cpumask_of_node(node);
#else
    return NULL;
#endif
}

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

// Various issues prevent us from using mmu_notifiers in older kernels. These
// include:
//  - ->release being called under RCU instead of SRCU: fixed by commit
//    21a92735f660eaecf69a6f2e777f18463760ec32, v3.7 (2012-10-08).
//  - Race conditions between mmu_notifier_release and mmu_notifier_unregister:
//    fixed by commit d34883d4e35c0a994e91dd847a82b4c9e0c31d83, v3.10
//    (2013-05-24).
//
// Unfortunately these issues aren't conftest-able, so instead we look for the
// presence of the invalidate_range callback in mmu_notifier_ops. This was added
// after all of the above issues were resolved, so we assume the fixes are
// present if we see the callback.
//
// The callback was added in commit 0f0a327fa12cd55de5e7f8c05a70ac3d047f405e,
// v3.19 (2014-11-13) and renamed in commit 1af5a8109904.
    #if defined(NV_MMU_NOTIFIER_OPS_HAS_INVALIDATE_RANGE) || \
        defined(NV_MMU_NOTIFIER_OPS_HAS_ARCH_INVALIDATE_SECONDARY_TLBS)
        #define UVM_CAN_USE_MMU_NOTIFIERS() 1
    #else
        #define UVM_CAN_USE_MMU_NOTIFIERS() 0
    #endif

// See bug 1707453 for further details about setting the minimum kernel version.
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 4, 0)
#  error This driver does not support kernels older than 4.4!
#endif

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

#if defined(NVCPU_X86)
/* Some old IA32 kernels don't have 64/64 division routines,
 * they only support 64/32 division with do_div(). */
static inline uint64_t NV_DIV64(uint64_t dividend, uint64_t divisor, uint64_t *remainder)
{
    /* do_div() only accepts a 32-bit divisor */
    *remainder = do_div(dividend, (uint32_t)divisor);

    /* do_div() modifies the dividend in-place */
    return dividend;
}
#else
/* All other 32/64-bit kernels we support (including non-x86 kernels) support
 * 64/64 division. */
static inline uint64_t NV_DIV64(uint64_t dividend, uint64_t divisor, uint64_t *remainder)
{
    *remainder = dividend % divisor;

    return dividend / divisor;
}
#endif

/* Return a nanosecond-precise value */
static inline NvU64 NV_GETTIME(void)
{
    struct timespec64 tm;

    ktime_get_raw_ts64(&tm);
    return (NvU64) timespec64_to_ns(&tm);
}

#if !defined(NV_FIND_NEXT_BIT_WRAP_PRESENT)
    static inline unsigned long find_next_bit_wrap(const unsigned long *addr, unsigned long size, unsigned long offset)
    {
        unsigned long bit = find_next_bit(addr, size, offset);

        if (bit < size)
            return bit;

        bit = find_first_bit(addr, offset);
        return bit < offset ? bit : size;
    }
#endif

// for_each_set_bit_wrap and __for_each_wrap were introduced in v6.1-rc1
// by commit 4fe49b3b97c2640147c46519c2a6fdb06df34f5f
#if !defined(for_each_set_bit_wrap)
static inline unsigned long __for_each_wrap(const unsigned long *bitmap,
                                            unsigned long size,
                                            unsigned long start,
                                            unsigned long n)
{
    unsigned long bit;

    if (n > start) {
        bit = find_next_bit(bitmap, size, n);
        if (bit < size)
            return bit;

        n = 0;
    }

    bit = find_next_bit(bitmap, start, n);
    return bit < start ? bit : size;
}

#define for_each_set_bit_wrap(bit, addr, size, start)                   \
    for ((bit) = find_next_bit_wrap((addr), (size), (start));           \
         (bit) < (size);                                                \
         (bit) = __for_each_wrap((addr), (size), (start), (bit) + 1))
#endif

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

static void uvm_init_radix_tree_preloadable(struct radix_tree_root *tree)
{
    // GFP_NOWAIT, or some combination of flags that avoids setting
    // __GFP_DIRECT_RECLAIM (__GFP_WAIT prior to commit
    // d0164adc89f6bb374d304ffcc375c6d2652fe67d from Nov 2015), is required for
    // using radix_tree_preload() for the tree.
    INIT_RADIX_TREE(tree, GFP_NOWAIT);
}

#if !defined(NV_RADIX_TREE_EMPTY_PRESENT)
static bool radix_tree_empty(struct radix_tree_root *tree)
{
    void *dummy;
    return radix_tree_gang_lookup(tree, &dummy, 0, 1) == 0;
}
#endif

// The radix tree root parameter was added to radix_tree_replace_slot in 4.10.
// That same change moved radix_tree_replace_slot from a header-only
// implementation to a .c file, but the symbol wasn't exported until later so
// we cannot use the function on 4.10. UVM uses this macro to ensure that
// radix_tree_replace_slot is not called when using that kernel.
#ifndef NV_RADIX_TREE_REPLACE_SLOT_PRESENT
    #define NV_RADIX_TREE_REPLACE_SLOT(...) \
        UVM_ASSERT_MSG(false, "radix_tree_replace_slot cannot be used in 4.10\n");
#else
#if (NV_RADIX_TREE_REPLACE_SLOT_ARGUMENT_COUNT == 2)
    #define NV_RADIX_TREE_REPLACE_SLOT(root, slot, entry) \
        radix_tree_replace_slot((slot), (entry))
#elif  (NV_RADIX_TREE_REPLACE_SLOT_ARGUMENT_COUNT == 3)
    #define NV_RADIX_TREE_REPLACE_SLOT(root, slot, entry) \
        radix_tree_replace_slot((root), (slot), (entry))
#else
#error "Unknown number of arguments"
#endif
#endif

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

// Added in 57bd1905b228f (acpi, x86/mm: Remove encryption mask from ACPI page
// protection type), v4.13
#if !defined(PAGE_KERNEL_NOENC)
  #define PAGE_KERNEL_NOENC PAGE_KERNEL
#endif

// uvm_pgprot_decrypted is a GPL-aware version of pgprot_decrypted that returns
// the given input when UVM cannot use GPL symbols, or pgprot_decrypted is not
// defined. Otherwise, the function is equivalent to pgprot_decrypted. UVM only
// depends on pgprot_decrypted when the driver is allowed to use GPL symbols:
// both AMD's SEV and Intel's TDX are only supported in conjunction with OpenRM.
//
// It is safe to invoke uvm_pgprot_decrypted in KVM + AMD SEV-SNP guests, even
// if the call is not required, because pgprot_decrypted(PAGE_KERNEL_NOENC) ==
// PAGE_KERNEL_NOENC.
//
// pgprot_decrypted was added by commit 21729f81ce8a ("x86/mm: Provide general
// kernel support for memory encryption") in v4.14 (2017-07-18)
static inline pgprot_t uvm_pgprot_decrypted(pgprot_t prot)
{
#if defined(pgprot_decrypted)
        return pgprot_decrypted(prot);
#endif

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
#define page_pgmap(page) (page)->pgmap
#endif

#endif // _UVM_LINUX_H
