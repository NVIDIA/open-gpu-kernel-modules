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

#if defined(NV_PNV_NPU2_INIT_CONTEXT_PRESENT)
#include <asm/powernv.h>
#endif

#if defined(NV_LINUX_SCHED_TASK_STACK_H_PRESENT)
#include <linux/sched/task_stack.h>
#endif

#include <linux/cpumask.h>
#include <linux/topology.h>

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
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
#  error This driver does not support kernels older than 3.10!
#endif

#if !defined(VM_RESERVED)
#define VM_RESERVED    0x00000000
#endif
#if !defined(VM_DONTEXPAND)
#define VM_DONTEXPAND  0x00000000
#endif
#if !defined(VM_DONTDUMP)
#define VM_DONTDUMP    0x00000000
#endif
#if !defined(VM_MIXEDMAP)
#define VM_MIXEDMAP    0x00000000
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

// printk_ratelimited was added in 2.6.33 via commit
// 8a64f336bc1d4aa203b138d29d5a9c414a9fbb47. If not available, we prefer not
// printing anything since it's supposed to be rate-limited.
#if !defined(printk_ratelimited)
    #define printk_ratelimited UVM_NO_PRINT
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0)
    // Just too much compilation trouble with the rate-limiting printk feature
    // until about k3.8. Because the non-rate-limited printing will cause
    // surprises and problems, just turn it off entirely in this situation.
    //
    #undef pr_debug_ratelimited
    #define pr_debug_ratelimited UVM_NO_PRINT
#endif

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
#if !defined(pmd_large)
#define pmd_large(_pmd) \
    ((pmd_val(_pmd) & (_PAGE_PSE|_PAGE_PRESENT)) == (_PAGE_PSE|_PAGE_PRESENT))
#endif
#endif /* defined(NVCPU_X86) || defined(NVCPU_X86_64) */

#if !defined(GFP_DMA32)
/*
 * GFP_DMA32 is similar to GFP_DMA, but instructs the Linux zone
 * allocator to allocate memory from the first 4GB on platforms
 * such as Linux/x86-64; the alternative is to use an IOMMU such
 * as the one implemented with the K8 GART, if available.
 */
#define GFP_DMA32 0
#endif

#if !defined(__GFP_NOWARN)
#define __GFP_NOWARN 0
#endif

#if !defined(__GFP_NORETRY)
#define __GFP_NORETRY 0
#endif

#define NV_UVM_GFP_FLAGS (GFP_KERNEL)

// Develop builds define DEBUG but enable optimization
#if defined(DEBUG) && !defined(NVIDIA_UVM_DEVELOP)
  // Wrappers for functions not building correctly without optimizations on,
  // implemented in uvm_debug_optimized.c. Notably the file is only built for
  // debug builds, not develop or release builds.

  // Unoptimized builds of atomic_xchg() hit a BUILD_BUG() on arm64 as it relies
  // on __xchg being completely inlined:
  //   /usr/src/linux-3.12.19/arch/arm64/include/asm/cmpxchg.h:67:3: note: in expansion of macro 'BUILD_BUG'
  //
  // Powerppc hits a similar issue, but ends up with an undefined symbol:
  //   WARNING: "__xchg_called_with_bad_pointer" [...] undefined!
  int nv_atomic_xchg(atomic_t *val, int new);

  // Same problem as atomic_xchg() on powerppc:
  //   WARNING: "__cmpxchg_called_with_bad_pointer" [...] undefined!
  int nv_atomic_cmpxchg(atomic_t *val, int old, int new);

  // Same problem as atomic_xchg() on powerppc:
  //   WARNING: "__cmpxchg_called_with_bad_pointer" [...] undefined!
  long nv_atomic_long_cmpxchg(atomic_long_t *val, long old, long new);

  // This Linux kernel commit:
  // 2016-08-30  0d025d271e55f3de21f0aaaf54b42d20404d2b23
  // leads to build failures on x86_64, when compiling without optimization. Avoid
  // that problem, by providing our own builds of copy_from_user / copy_to_user,
  // for debug (non-optimized) UVM builds. Those are accessed via these
  // nv_copy_to/from_user wrapper functions.
  //
  // Bug 1849583 has further details.
  unsigned long nv_copy_from_user(void *to, const void __user *from, unsigned long n);
  unsigned long nv_copy_to_user(void __user *to, const void *from, unsigned long n);

#else
  #define nv_atomic_xchg            atomic_xchg
  #define nv_atomic_cmpxchg         atomic_cmpxchg
  #define nv_atomic_long_cmpxchg    atomic_long_cmpxchg
  #define nv_copy_to_user           copy_to_user
  #define nv_copy_from_user         copy_from_user
#endif

#ifndef NV_ALIGN_DOWN
#define NV_ALIGN_DOWN(v,g) ((v) & ~((g) - 1))
#endif

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

#if defined(CLOCK_MONOTONIC_RAW)
/* Return a nanosecond-precise value */
static inline NvU64 NV_GETTIME(void)
{
    struct timespec64 tm;

    ktime_get_raw_ts64(&tm);
    return (NvU64) timespec64_to_ns(&tm);
}
#else
/* We can only return a microsecond-precise value with the
 * available non-GPL symbols. */
static inline NvU64 NV_GETTIME(void)
{
    struct timespec64 tm;

    ktime_get_real_ts64(&tm);
    return (NvU64) timespec64_to_ns(&tm);
}
#endif

#if !defined(ilog2)
    static inline int NV_ILOG2_U32(u32 n)
    {
        return fls(n) - 1;
    }
    static inline int NV_ILOG2_U64(u64 n)
    {
        return fls64(n) - 1;
    }
    #define ilog2(n) (sizeof(n) <= 4 ? NV_ILOG2_U32(n) : NV_ILOG2_U64(n))
#endif

// for_each_bit added in 2.6.24 via commit 3e037454bcfa4b187e8293d2121bd8c0f5a5c31c
// later renamed in 2.6.34 via commit 984b3f5746ed2cde3d184651dabf26980f2b66e5
#if !defined(for_each_set_bit)
    #define for_each_set_bit(bit, addr, size) for_each_bit((bit), (addr), (size))
#endif

// for_each_set_bit_cont was added in 3.2 via 1e2ad28f80b4e155678259238f51edebc19e4014
// It was renamed to for_each_set_bit_from in 3.3 via 307b1cd7ecd7f3dc5ce3d3860957f034f0abe4df
#if !defined(for_each_set_bit_from)
    #define for_each_set_bit_from(bit, addr, size)              \
        for ((bit) = find_next_bit((addr), (size), (bit));      \
             (bit) < (size);                                    \
             (bit) = find_next_bit((addr), (size), (bit) + 1))
#endif

// for_each_clear_bit and for_each_clear_bit_from were added in 3.10 via
// 03f4a8226c2f9c14361f75848d1e93139bab90c4
#if !defined(for_each_clear_bit)
    #define for_each_clear_bit(bit, addr, size)                     \
        for ((bit) = find_first_zero_bit((addr), (size));           \
             (bit) < (size);                                        \
             (bit) = find_next_zero_bit((addr), (size), (bit) + 1))
#endif

#if !defined(for_each_clear_bit_from)
    #define for_each_clear_bit_from(bit, addr, size)                \
        for ((bit) = find_next_zero_bit((addr), (size), (bit));     \
             (bit) < (size);                                        \
             (bit) = find_next_zero_bit((addr), (size), (bit) + 1))
#endif

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

// Added in 2.6.24
#ifndef ACCESS_ONCE
  #define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))
#endif

// WRITE_ONCE/READ_ONCE have incompatible definitions across versions, which produces warnings.
// Therefore, we define our own macros
#define UVM_WRITE_ONCE(x, val) (ACCESS_ONCE(x) = (val))
#define UVM_READ_ONCE(x) ACCESS_ONCE(x)

// smp_mb__before_atomic was added in 3.16, provide a fallback
#ifndef smp_mb__before_atomic
  #if NVCPU_IS_X86 || NVCPU_IS_X86_64
    // That's what the kernel does for x86
    #define smp_mb__before_atomic() barrier()
  #else
    // That's what the kernel does for at least arm32, arm64 and powerpc as of 4.3
    #define smp_mb__before_atomic() smp_mb()
  #endif
#endif

// smp_mb__after_atomic was added in 3.16, provide a fallback
#ifndef smp_mb__after_atomic
  #if NVCPU_IS_X86 || NVCPU_IS_X86_64
    // That's what the kernel does for x86
    #define smp_mb__after_atomic() barrier()
  #else
    // That's what the kernel does for at least arm32, arm64 and powerpc as of 4.3
    #define smp_mb__after_atomic() smp_mb()
  #endif
#endif

// smp_load_acquire and smp_store_release were added in commit
// 47933ad41a86a4a9b50bed7c9b9bd2ba242aac63 ("arch: Introduce
// smp_load_acquire(), smp_store_release()") in v3.14 (2013-11-06).
#ifndef smp_load_acquire
    #define smp_load_acquire(p)                     \
        ({                                          \
            typeof(*(p)) __v = UVM_READ_ONCE(*(p)); \
            smp_mb();                               \
            __v;                                    \
        })
#endif

#ifndef smp_store_release
    #define smp_store_release(p, v)     \
        do {                            \
            smp_mb();                   \
            UVM_WRITE_ONCE(*(p), v);    \
        } while (0)
#endif

// atomic_read_acquire and atomic_set_release were added in commit
// 654672d4ba1a6001c365833be895f9477c4d5eab ("locking/atomics:
// Add _{acquire|release|relaxed}() variants of some atomic operations") in v4.3
// (2015-08-06).
// TODO: Bug 3849079: We always use this definition on newer kernels.
#ifndef atomic_read_acquire
    #define atomic_read_acquire(p) smp_load_acquire(&(p)->counter)
#endif

#ifndef atomic_set_release
    #define atomic_set_release(p, v) smp_store_release(&(p)->counter, v)
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

// Added in 3.11
#ifndef PAGE_ALIGNED
    #define PAGE_ALIGNED(addr) (((addr) & (PAGE_SIZE - 1)) == 0)
#endif

// Changed in 3.17 via commit 743162013d40ca612b4cb53d3a200dff2d9ab26e
#if (NV_WAIT_ON_BIT_LOCK_ARGUMENT_COUNT == 3)
    #define UVM_WAIT_ON_BIT_LOCK(word, bit, mode) \
        wait_on_bit_lock(word, bit, mode)
#elif (NV_WAIT_ON_BIT_LOCK_ARGUMENT_COUNT == 4)
    static __sched int uvm_bit_wait(void *word)
    {
        if (signal_pending_state(current->state, current))
            return 1;
        schedule();
        return 0;
    }
    #define UVM_WAIT_ON_BIT_LOCK(word, bit, mode) \
        wait_on_bit_lock(word, bit, uvm_bit_wait, mode)
#else
#error "Unknown number of arguments"
#endif

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

// Commit 1dff8083a024650c75a9c961c38082473ceae8cf (v4.7).
//
// Archs with CONFIG_MMU should have their own page.h, and can't include
// asm-generic/page.h. However, x86, powerpc, arm64 don't define page_to_virt()
// macro in their version of page.h.
#include <linux/mm.h>
#ifndef page_to_virt
  #include <asm/page.h>
  #define page_to_virt(x)    __va(PFN_PHYS(page_to_pfn(x)))
#endif

#endif // _UVM_LINUX_H
