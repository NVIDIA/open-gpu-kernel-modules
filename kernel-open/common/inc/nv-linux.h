/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_LINUX_H_
#define _NV_LINUX_H_

#include "nvstatus.h"
#include "nv.h"
#include "nv-ioctl-numa.h"
#include "conftest.h"

#include "nv-lock.h"
#include "nv-pgprot.h"
#include "nv-mm.h"
#include "os-interface.h"
#include "nv-timer.h"
#include "nv-time.h"

#define NV_KERNEL_NAME "Linux"

#ifndef AUTOCONF_INCLUDED
#if defined(NV_GENERATED_AUTOCONF_H_PRESENT)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#endif

#if defined(NV_GENERATED_UTSRELEASE_H_PRESENT)
  #include <generated/utsrelease.h>
#endif

#if defined(NV_GENERATED_COMPILE_H_PRESENT)
  #include <generated/compile.h>
#endif

#include <linux/version.h>
#include <linux/utsname.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
#error "This driver does not support kernels older than 2.6.32!"
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 7, 0)
#  define KERNEL_2_6
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
#  define KERNEL_3
#else
#error "This driver does not support development kernels!"
#endif

#if defined (CONFIG_SMP) && !defined (__SMP__)
#define __SMP__
#endif

#if defined (CONFIG_MODVERSIONS) && !defined (MODVERSIONS)
#  define MODVERSIONS
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <asm/bug.h>

#include <linux/mm.h>

#if !defined(VM_RESERVED)
#define VM_RESERVED    0x00000000
#endif
#if !defined(VM_DONTEXPAND)
#define VM_DONTEXPAND  0x00000000
#endif
#if !defined(VM_DONTDUMP)
#define VM_DONTDUMP    0x00000000
#endif

#include <linux/init.h>             /* module_init, module_exit         */
#include <linux/types.h>            /* pic_t, size_t, __u32, etc        */
#include <linux/errno.h>            /* error codes                      */
#include <linux/list.h>             /* circular linked list             */
#include <linux/stddef.h>           /* NULL, offsetof                   */
#include <linux/wait.h>             /* wait queues                      */
#include <linux/string.h>           /* strchr(), strpbrk()              */

#include <linux/ctype.h>            /* isspace(), etc                   */
#include <linux/console.h>          /* acquire_console_sem(), etc       */
#include <linux/cpufreq.h>          /* cpufreq_get                      */

#include <linux/slab.h>             /* kmalloc, kfree, etc              */
#include <linux/vmalloc.h>          /* vmalloc, vfree, etc              */

#include <linux/poll.h>             /* poll_wait                        */
#include <linux/delay.h>            /* mdelay, udelay                   */

#include <linux/sched.h>            /* suser(), capable() replacement   */

#include <linux/random.h>           /* get_random_bytes()               */

#if defined(NV_LINUX_DMA_BUF_H_PRESENT)
#include <linux/dma-buf.h>
#endif

#if defined(NV_DRM_AVAILABLE)
#if defined(NV_DRM_DRM_DEVICE_H_PRESENT)
#include <drm/drm_device.h>
#endif

#if defined(NV_DRM_DRM_DRV_H_PRESENT)
#include <drm/drm_drv.h>
#endif

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_GEM_H_PRESENT)
#include <drm/drm_gem.h>
#endif
#endif /* NV_DRM_AVAILABLE */

/*
 * sched.h was refactored with this commit (as part of Linux 4.11)
 *   2017-03-03  1827adb11ad26b2290dc9fe2aaf54976b2439865
 */
#if defined(NV_LINUX_SCHED_SIGNAL_H_PRESENT)
#include <linux/sched/signal.h>     /* task_lock(), task_unlock()       */
#endif

#if defined(NV_LINUX_SCHED_TASK_H_PRESENT)
#include <linux/sched/task.h>       /* task_lock(), task_unlock()       */
#endif

/* task and signal-related items, for kernels < 4.11: */
#include <linux/sched.h>            /* task_lock(), task_unlock()       */

#include <linux/moduleparam.h>      /* module_param()                   */
#include <asm/tlbflush.h>           /* flush_tlb(), flush_tlb_all()     */

#include <linux/pci.h>              /* pci_find_class, etc              */
#include <linux/interrupt.h>        /* tasklets, interrupt helpers      */
#include <linux/timer.h>
#include <linux/file.h>             /* fget(), fput()                   */
#include <linux/rbtree.h>
#include <linux/cpu.h>              /* CPU hotplug support              */

#include <linux/pm_runtime.h>       /* pm_runtime_*                     */
#include <linux/fdtable.h>          /* files_fdtable, etc               */

#include <asm/div64.h>              /* do_div()                         */
#if defined(NV_ASM_SYSTEM_H_PRESENT)
#include <asm/system.h>             /* cli, sli, save_flags             */
#endif
#include <asm/io.h>                 /* ioremap, virt_to_phys            */
#include <asm/uaccess.h>            /* access_ok                        */
#include <asm/page.h>               /* PAGE_OFFSET                      */
#include <asm/pgtable.h>            /* pte bit definitions              */
#include <asm/bitops.h>             /* __set_bit()                      */

#if defined(NV_LINUX_TIME_H_PRESENT)
#include <linux/time.h>             /* FD_SET()                         */
#endif

#include "nv-list-helpers.h"

/*
 * Use current->cred->euid, instead of calling current_euid().
 * The latter can pull in the GPL-only debug_lockdep_rcu_enabled()
 * symbol when CONFIG_PROVE_RCU.  That is only used for debugging.
 *
 * The Linux kernel relies on the assumption that only the current process
 * is permitted to change its cred structure. Therefore, current_euid()
 * does not require the RCU's read lock on current->cred.
 */
#define NV_CURRENT_EUID() (__kuid_val(current->cred->euid))

#if !defined(NV_KUID_T_PRESENT)
static inline uid_t __kuid_val(uid_t uid)
{
    return uid;
}
#endif

#if defined(CONFIG_VGA_ARB)
#include <linux/vgaarb.h>
#endif

#include <linux/pagemap.h>
#include <linux/dma-mapping.h>

#if defined(NV_LINUX_DMA_MAP_OPS_H_PRESENT)
#include <linux/dma-map-ops.h>
#endif

#if defined(CONFIG_SWIOTLB) && defined(NVCPU_AARCH64)
#include <linux/swiotlb.h>
#endif

#include <linux/scatterlist.h>
#include <linux/completion.h>
#include <linux/highmem.h>

#include <linux/nodemask.h>

#include <linux/workqueue.h>        /* workqueue                        */
#include "nv-kthread-q.h"           /* kthread based queue              */

#if defined(NV_LINUX_EFI_H_PRESENT)
#include <linux/efi.h>              /* efi_enabled                      */
#endif

#include <linux/fb.h>               /* fb_info struct                   */
#include <linux/screen_info.h>      /* screen_info                      */

#if !defined(CONFIG_PCI)
#warning "Attempting to build driver for a platform with no PCI support!"
#include <asm-generic/pci-dma-compat.h>
#endif

#if defined(NV_EFI_ENABLED_PRESENT) && defined(NV_EFI_ENABLED_ARGUMENT_COUNT)
#if (NV_EFI_ENABLED_ARGUMENT_COUNT == 1)
#define NV_EFI_ENABLED() efi_enabled(EFI_BOOT)
#else
#error "NV_EFI_ENABLED_ARGUMENT_COUNT value unrecognized!"
#endif
#elif (defined(NV_EFI_ENABLED_PRESENT) || defined(efi_enabled))
#define NV_EFI_ENABLED() efi_enabled
#else
#define NV_EFI_ENABLED() 0
#endif

#if defined(CONFIG_CRAY_XT)
#include <cray/cray_nvidia.h>
NV_STATUS nvos_forward_error_to_cray(struct pci_dev *, NvU32,
        const char *, va_list);
#endif

#if defined(NVCPU_PPC64LE) && defined(CONFIG_EEH)
#include <asm/eeh.h>
#define NV_PCI_ERROR_RECOVERY_ENABLED() eeh_enabled()
#define NV_PCI_ERROR_RECOVERY
#endif

#if defined(NV_ASM_SET_MEMORY_H_PRESENT)
#include <asm/set_memory.h>
#endif

#if defined(NV_SET_MEMORY_UC_PRESENT)
#undef NV_SET_PAGES_UC_PRESENT
#endif

#if !defined(NVCPU_AARCH64) && !defined(NVCPU_PPC64LE)
#if !defined(NV_SET_MEMORY_UC_PRESENT) && !defined(NV_SET_PAGES_UC_PRESENT)
#error "This driver requires the ability to change memory types!"
#endif
#endif

/*
 * Traditionally, CONFIG_XEN indicated that the target kernel was
 * built exclusively for use under a Xen hypervisor, requiring
 * modifications to or disabling of a variety of NVIDIA graphics
 * driver code paths. As of the introduction of CONFIG_PARAVIRT
 * and support for Xen hypervisors within the CONFIG_PARAVIRT_GUEST
 * architecture, CONFIG_XEN merely indicates that the target
 * kernel can run under a Xen hypervisor, but not that it will.
 *
 * If CONFIG_XEN and CONFIG_PARAVIRT are defined, the old Xen
 * specific code paths are disabled. If the target kernel executes
 * stand-alone, the NVIDIA graphics driver will work fine. If the
 * kernels executes under a Xen (or other) hypervisor, however, the
 * NVIDIA graphics driver has no way of knowing and is unlikely
 * to work correctly.
 */
#if defined(CONFIG_XEN) && !defined(CONFIG_PARAVIRT)
#include <asm/maddr.h>
#include <xen/interface/memory.h>
#define NV_XEN_SUPPORT_FULLY_VIRTUALIZED_KERNEL
#endif

#ifdef CONFIG_KDB
#include <linux/kdb.h>
#include <asm/kdb.h>
#endif

#if defined(CONFIG_X86_REMOTE_DEBUG)
#include <linux/gdb.h>
#endif

#if defined(DEBUG) && defined(CONFIG_KGDB) && \
    defined(NVCPU_AARCH64)
#include <asm/kgdb.h>
#endif

#if defined(NVCPU_X86_64) && !defined(NV_XEN_SUPPORT_FULLY_VIRTUALIZED_KERNEL)
#define NV_ENABLE_PAT_SUPPORT
#endif

#define NV_PAT_MODE_DISABLED    0
#define NV_PAT_MODE_KERNEL      1
#define NV_PAT_MODE_BUILTIN     2

extern int nv_pat_mode;

#if defined(CONFIG_HOTPLUG_CPU)
#define NV_ENABLE_HOTPLUG_CPU
#include <linux/notifier.h>         /* struct notifier_block, etc       */
#endif

#if (defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE))
#include <linux/i2c.h>
#endif

#if defined(CONFIG_ACPI)
#include <linux/acpi.h>
#define NV_LINUX_ACPI_EVENTS_SUPPORTED 1
#endif

#if defined(NV_LINUX_ACPI_EVENTS_SUPPORTED)
#define NV_ACPI_WALK_NAMESPACE(type, start_object, max_depth, \
        user_function, args...) \
    acpi_walk_namespace(type, start_object, max_depth, \
            user_function, NULL, args)
#endif

#if defined(CONFIG_PREEMPT_RT) || defined(CONFIG_PREEMPT_RT_FULL)
#define NV_CONFIG_PREEMPT_RT 1
#endif

#if defined(NV_WRITE_CR4_PRESENT)
#define NV_READ_CR4()       read_cr4()
#define NV_WRITE_CR4(cr4)   write_cr4(cr4)
#else
#define NV_READ_CR4()       __read_cr4()
#define NV_WRITE_CR4(cr4)   __write_cr4(cr4)
#endif

#ifndef get_cpu
#define get_cpu() smp_processor_id()
#define put_cpu()
#endif

#if !defined(unregister_hotcpu_notifier)
#define unregister_hotcpu_notifier unregister_cpu_notifier
#endif
#if !defined(register_hotcpu_notifier)
#define register_hotcpu_notifier register_cpu_notifier
#endif

#if defined(NVCPU_X86_64)
#if !defined(pmd_large)
#define pmd_large(_pmd) \
    ((pmd_val(_pmd) & (_PAGE_PSE|_PAGE_PRESENT)) == (_PAGE_PSE|_PAGE_PRESENT))
#endif
#endif /* defined(NVCPU_X86_64) */

#define NV_PAGE_COUNT(page) \
  ((unsigned int)page_count(page))
#define NV_GET_PAGE_COUNT(page_ptr) \
  (NV_PAGE_COUNT(NV_GET_PAGE_STRUCT(page_ptr->phys_addr)))
#define NV_GET_PAGE_FLAGS(page_ptr) \
  (NV_GET_PAGE_STRUCT(page_ptr->phys_addr)->flags)

/*
 * Before the introduction of VM_PFNMAP, there was an VM_UNPAGED flag.
 * Drivers which wanted to call remap_pfn_range on normal pages had to use this
 * VM_UNPAGED flag *and* set PageReserved. With the introduction of VM_PFNMAP,
 * that restriction went away. This is described in commit
 *
 *   2005-10-28 6aab341e0a28aff100a09831c5300a2994b8b986
 *     ("mm: re-architect the VM_UNPAGED logic")
 *
 * , which added VM_PFNMAP and vm_normal_page. Therefore, if VM_PFNMAP is
 * defined, then we do *not* need to mark a page as reserved, in order to
 * call remap_pfn_range().
 */
#if !defined(VM_PFNMAP)
#define NV_MAYBE_RESERVE_PAGE(ptr_ptr) \
  SetPageReserved(NV_GET_PAGE_STRUCT(page_ptr->phys_addr))
#define NV_MAYBE_UNRESERVE_PAGE(page_ptr) \
  ClearPageReserved(NV_GET_PAGE_STRUCT(page_ptr->phys_addr))
#else
#define NV_MAYBE_RESERVE_PAGE(ptr_ptr)
#define NV_MAYBE_UNRESERVE_PAGE(page_ptr)
#endif /* defined(VM_PFNMAP) */

#if !defined(__GFP_COMP)
#define __GFP_COMP 0
#endif

#if !defined(DEBUG) && defined(__GFP_NOWARN)
#define NV_GFP_KERNEL (GFP_KERNEL | __GFP_NOWARN)
#define NV_GFP_ATOMIC (GFP_ATOMIC | __GFP_NOWARN)
#else
#define NV_GFP_KERNEL (GFP_KERNEL)
#define NV_GFP_ATOMIC (GFP_ATOMIC)
#endif

#if defined(GFP_DMA32)
/*
 * GFP_DMA32 is similar to GFP_DMA, but instructs the Linux zone
 * allocator to allocate memory from the first 4GB on platforms
 * such as Linux/x86-64; the alternative is to use an IOMMU such
 * as the one implemented with the K8 GART, if available.
 */
#define NV_GFP_DMA32 (NV_GFP_KERNEL | GFP_DMA32)
#else
#define NV_GFP_DMA32 (NV_GFP_KERNEL)
#endif

extern NvBool nvos_is_chipset_io_coherent(void);

#if defined(NVCPU_X86_64)
#define CACHE_FLUSH()  asm volatile("wbinvd":::"memory")
#define WRITE_COMBINE_FLUSH() asm volatile("sfence":::"memory")
#elif defined(NVCPU_AARCH64)
    static inline void nv_flush_cache_cpu(void *info)
    {
        if (!nvos_is_chipset_io_coherent())
        {
#if defined(NV_FLUSH_CACHE_ALL_PRESENT)
            flush_cache_all();
#else
            WARN_ONCE(0, "NVRM: kernel does not support flush_cache_all()\n");
#endif
        }
    }
#define CACHE_FLUSH()            nv_flush_cache_cpu(NULL)
#define CACHE_FLUSH_ALL()        on_each_cpu(nv_flush_cache_cpu, NULL, 1)
#define WRITE_COMBINE_FLUSH()    mb()
#elif defined(NVCPU_PPC64LE)
#define CACHE_FLUSH()            asm volatile("sync;  \n" \
                                              "isync; \n" ::: "memory")
#define WRITE_COMBINE_FLUSH()    CACHE_FLUSH()
#endif

typedef enum
{
    NV_MEMORY_TYPE_SYSTEM,      /* Memory mapped for ROM, SBIOS and physical RAM. */
    NV_MEMORY_TYPE_REGISTERS,
    NV_MEMORY_TYPE_FRAMEBUFFER,
    NV_MEMORY_TYPE_DEVICE_MMIO, /* All kinds of MMIO referred by NVRM e.g. BARs and MCFG of device */
} nv_memory_type_t;

#if defined(NVCPU_AARCH64) || defined(NVCPU_PPC64LE)
#define NV_ALLOW_WRITE_COMBINING(mt)    1
#elif defined(NVCPU_X86_64)
#if defined(NV_ENABLE_PAT_SUPPORT)
#define NV_ALLOW_WRITE_COMBINING(mt)    \
    ((nv_pat_mode != NV_PAT_MODE_DISABLED) && \
     ((mt) != NV_MEMORY_TYPE_REGISTERS))
#else
#define NV_ALLOW_WRITE_COMBINING(mt)    0
#endif
#endif

#if !defined(IRQF_SHARED)
#define IRQF_SHARED SA_SHIRQ
#endif

#define NV_MAX_RECURRING_WARNING_MESSAGES 10

/* various memory tracking/debugging techniques
 * disabled for retail builds, enabled for debug builds
 */

// allow an easy way to convert all debug printfs related to memory
// management back and forth between 'info' and 'errors'
#if defined(NV_DBG_MEM)
#define NV_DBG_MEMINFO NV_DBG_ERRORS
#else
#define NV_DBG_MEMINFO NV_DBG_INFO
#endif

#define NV_MEM_TRACKING_PAD_SIZE(size) \
    (size) = NV_ALIGN_UP((size + sizeof(void *)), sizeof(void *))

#define NV_MEM_TRACKING_HIDE_SIZE(ptr, size)            \
    if ((ptr != NULL) && (*(ptr) != NULL))              \
    {                                                   \
        NvU8 *__ptr;                                    \
        *(unsigned long *) *(ptr) = (size);             \
        __ptr = *(ptr); __ptr += sizeof(void *);        \
        *(ptr) = (void *) __ptr;                        \
    }
#define NV_MEM_TRACKING_RETRIEVE_SIZE(ptr, size)        \
    {                                                   \
        NvU8 *__ptr = (ptr); __ptr -= sizeof(void *);   \
        (ptr) = (void *) __ptr;                         \
        (size) = *(unsigned long *) (ptr);              \
    }

/* keep track of memory usage */
#include "nv-memdbg.h"

static inline void *nv_vmalloc(unsigned long size)
{
#if defined(NV_VMALLOC_HAS_PGPROT_T_ARG)
    void *ptr = __vmalloc(size, GFP_KERNEL, PAGE_KERNEL);
#else
    void *ptr = __vmalloc(size, GFP_KERNEL);
#endif
    if (ptr)
        NV_MEMDBG_ADD(ptr, size);
    return ptr;
}

static inline void nv_vfree(void *ptr, NvU32 size)
{
    NV_MEMDBG_REMOVE(ptr, size);
    vfree(ptr);
}

static inline void *nv_ioremap(NvU64 phys, NvU64 size)
{
    void *ptr = ioremap(phys, size);
    if (ptr)
        NV_MEMDBG_ADD(ptr, size);
    return ptr;
}

static inline void *nv_ioremap_nocache(NvU64 phys, NvU64 size)
{
    return nv_ioremap(phys, size);
}

static inline void *nv_ioremap_cache(NvU64 phys, NvU64 size)
{
#if defined(NV_IOREMAP_CACHE_PRESENT)
    void *ptr = ioremap_cache(phys, size);
    if (ptr)
        NV_MEMDBG_ADD(ptr, size);
    return ptr;
#elif defined(NVCPU_PPC64LE)
    //
    // ioremap_cache() has been only implemented correctly for ppc64le with
    // commit f855b2f544d6 in April 2017 (kernel 4.12+). Internally, the kernel
    // does provide a default implementation of ioremap_cache() that would be
    // incorrect for our use (creating an uncached mapping) before the
    // referenced commit, but that implementation is not exported and the
    // NV_IOREMAP_CACHE_PRESENT conftest doesn't pick it up, and we end up in
    // this #elif branch.
    //
    // At the same time, ppc64le have supported ioremap_prot() since May 2011
    // (commit 40f1ce7fb7e8, kernel 3.0+) and that covers all kernels we
    // support on power.
    //
    void *ptr = ioremap_prot(phys, size, pgprot_val(PAGE_KERNEL));
    if (ptr)
        NV_MEMDBG_ADD(ptr, size);
    return ptr;
#else
    return nv_ioremap(phys, size);
#endif
}

static inline void *nv_ioremap_wc(NvU64 phys, NvU64 size)
{
#if defined(NV_IOREMAP_WC_PRESENT)
    void *ptr = ioremap_wc(phys, size);
    if (ptr)
        NV_MEMDBG_ADD(ptr, size);
    return ptr;
#else
    return nv_ioremap_nocache(phys, size);
#endif
}

static inline void nv_iounmap(void *ptr, NvU64 size)
{
    NV_MEMDBG_REMOVE(ptr, size);
    iounmap(ptr);
}

static NvBool nv_numa_node_has_memory(int node_id)
{
    if (node_id < 0 || node_id >= MAX_NUMNODES)
        return NV_FALSE;
#if defined(NV_NODE_STATES_N_MEMORY_PRESENT)
    return node_state(node_id, N_MEMORY) ? NV_TRUE : NV_FALSE;
#else
    return node_state(node_id, N_HIGH_MEMORY) ? NV_TRUE : NV_FALSE;
#endif
}

#define NV_KMALLOC(ptr, size) \
    { \
        (ptr) = kmalloc(size, NV_GFP_KERNEL); \
        if (ptr) \
            NV_MEMDBG_ADD(ptr, size); \
    }

#define NV_KMALLOC_ATOMIC(ptr, size) \
    { \
        (ptr) = kmalloc(size, NV_GFP_ATOMIC); \
        if (ptr) \
            NV_MEMDBG_ADD(ptr, size); \
    }

#if defined(__GFP_RETRY_MAYFAIL)
#define NV_GFP_NO_OOM (NV_GFP_KERNEL | __GFP_RETRY_MAYFAIL)
#elif defined(__GFP_NORETRY)
#define NV_GFP_NO_OOM (NV_GFP_KERNEL | __GFP_NORETRY)
#else
#define NV_GFP_NO_OOM (NV_GFP_KERNEL)
#endif

#define NV_KMALLOC_NO_OOM(ptr, size) \
    { \
        (ptr) = kmalloc(size, NV_GFP_NO_OOM); \
        if (ptr) \
            NV_MEMDBG_ADD(ptr, size); \
    }

#define NV_KFREE(ptr, size) \
    { \
        NV_MEMDBG_REMOVE(ptr, size); \
        kfree((void *) (ptr)); \
    }

#define NV_ALLOC_PAGES_NODE(ptr, nid, order, gfp_mask) \
    { \
        (ptr) = (unsigned long)page_address(alloc_pages_node(nid, gfp_mask, order)); \
    }

#define NV_GET_FREE_PAGES(ptr, order, gfp_mask)      \
    {                                                \
        (ptr) = __get_free_pages(gfp_mask, order);   \
    }

#define NV_FREE_PAGES(ptr, order)                    \
    {                                                \
        free_pages(ptr, order);                      \
    }

#if defined(PAGE_KERNEL_NOENC)
#if defined(__pgprot_mask)
#define NV_PAGE_KERNEL_NOCACHE_NOENC __pgprot_mask(__PAGE_KERNEL_NOCACHE)
#elif defined(default_pgprot)
#define NV_PAGE_KERNEL_NOCACHE_NOENC default_pgprot(__PAGE_KERNEL_NOCACHE)
#elif defined( __pgprot)
#define NV_PAGE_KERNEL_NOCACHE_NOENC __pgprot(__PAGE_KERNEL_NOCACHE)
#else
#error "Unsupported kernel!!!"
#endif
#endif

static inline NvUPtr nv_vmap(struct page **pages, NvU32 page_count,
                             NvBool cached, NvBool unencrypted)
{
    void *ptr;
    pgprot_t prot = PAGE_KERNEL;
#if defined(NVCPU_X86_64)
#if defined(PAGE_KERNEL_NOENC)
    if (unencrypted)
    {
        prot = cached ? PAGE_KERNEL_NOENC : NV_PAGE_KERNEL_NOCACHE_NOENC;
    }
    else
#endif
    {
        prot = cached ? PAGE_KERNEL : PAGE_KERNEL_NOCACHE;
    }
#elif defined(NVCPU_AARCH64)
    prot = cached ? PAGE_KERNEL : NV_PGPROT_UNCACHED(PAGE_KERNEL);
#endif
    /* All memory cached in PPC64LE; can't honor 'cached' input. */
    ptr = vmap(pages, page_count, VM_MAP, prot);
    if (ptr)
        NV_MEMDBG_ADD(ptr, page_count * PAGE_SIZE);
    return (NvUPtr)ptr;
}

static inline void nv_vunmap(NvUPtr vaddr, NvU32 page_count)
{
    vunmap((void *)vaddr);
    NV_MEMDBG_REMOVE((void *)vaddr, page_count * PAGE_SIZE);
}

#if defined(NV_GET_NUM_PHYSPAGES_PRESENT)
#define NV_NUM_PHYSPAGES                get_num_physpages()
#else
#define NV_NUM_PHYSPAGES                num_physpages
#endif
#define NV_GET_CURRENT_PROCESS()        current->tgid
#define NV_IN_ATOMIC()                  in_atomic()
#define NV_LOCAL_BH_DISABLE()           local_bh_disable()
#define NV_LOCAL_BH_ENABLE()            local_bh_enable()
#define NV_COPY_TO_USER(to, from, n)    copy_to_user(to, from, n)
#define NV_COPY_FROM_USER(to, from, n)  copy_from_user(to, from, n)

#define NV_IS_SUSER()                   capable(CAP_SYS_ADMIN)
#define NV_PCI_DEVICE_NAME(pci_dev)     ((pci_dev)->pretty_name)
#define NV_CLI()                        local_irq_disable()
#define NV_SAVE_FLAGS(eflags)           local_save_flags(eflags)
#define NV_RESTORE_FLAGS(eflags)        local_irq_restore(eflags)
#define NV_MAY_SLEEP()                  (!irqs_disabled() && !in_interrupt() && !NV_IN_ATOMIC())
#define NV_MODULE_PARAMETER(x)          module_param(x, int, 0)
#define NV_MODULE_STRING_PARAMETER(x)   module_param(x, charp, 0)
#undef  MODULE_PARM

#define NV_NUM_CPUS()                   num_possible_cpus()

static inline dma_addr_t nv_phys_to_dma(struct device *dev, NvU64 pa)
{
#if defined(NV_PHYS_TO_DMA_PRESENT)
    return phys_to_dma(dev, pa);
#elif defined(NV_XEN_SUPPORT_FULLY_VIRTUALIZED_KERNEL)
    return phys_to_machine(pa);
#else
    return (dma_addr_t)pa;
#endif
}

#define NV_GET_PAGE_STRUCT(phys_page) virt_to_page(__va(phys_page))
#define NV_VMA_PGOFF(vma)             ((vma)->vm_pgoff)
#define NV_VMA_SIZE(vma)              ((vma)->vm_end - (vma)->vm_start)
#define NV_VMA_OFFSET(vma)            (((NvU64)(vma)->vm_pgoff) << PAGE_SHIFT)
#define NV_VMA_PRIVATE(vma)           ((vma)->vm_private_data)
#define NV_VMA_FILE(vma)              ((vma)->vm_file)

#define NV_DEVICE_MINOR_NUMBER(x)     minor((x)->i_rdev)
#define NV_CONTROL_DEVICE_MINOR       255

#define NV_PCI_DISABLE_DEVICE(pci_dev)                           \
    {                                                            \
        NvU16 __cmd[2];                                          \
        pci_read_config_word((pci_dev), PCI_COMMAND, &__cmd[0]); \
        pci_disable_device(pci_dev);                             \
        pci_read_config_word((pci_dev), PCI_COMMAND, &__cmd[1]); \
        __cmd[1] |= PCI_COMMAND_MEMORY;                          \
        pci_write_config_word((pci_dev), PCI_COMMAND,            \
                (__cmd[1] | (__cmd[0] & PCI_COMMAND_IO)));       \
    }

#define NV_PCI_RESOURCE_START(pci_dev, bar) pci_resource_start(pci_dev, (bar))
#define NV_PCI_RESOURCE_SIZE(pci_dev, bar)  pci_resource_len(pci_dev, (bar))
#define NV_PCI_RESOURCE_FLAGS(pci_dev, bar) pci_resource_flags(pci_dev, (bar))

#define NV_PCI_RESOURCE_VALID(pci_dev, bar)                                     \
    ((NV_PCI_RESOURCE_START(pci_dev, bar) != 0) &&                              \
     (NV_PCI_RESOURCE_SIZE(pci_dev, bar) != 0))

#define NV_PCI_DOMAIN_NUMBER(pci_dev) (NvU32)pci_domain_nr(pci_dev->bus)
#define NV_PCI_BUS_NUMBER(pci_dev)    (pci_dev)->bus->number
#define NV_PCI_DEVFN(pci_dev)         (pci_dev)->devfn
#define NV_PCI_SLOT_NUMBER(pci_dev)   PCI_SLOT(NV_PCI_DEVFN(pci_dev))

#if defined(CONFIG_X86_UV) && defined(NV_CONFIG_X86_UV)
#define NV_GET_DOMAIN_BUS_AND_SLOT(domain,bus,devfn)                        \
   ({                                                                       \
        struct pci_dev *__dev = NULL;                                       \
        while ((__dev = pci_get_device(PCI_VENDOR_ID_NVIDIA,                \
                    PCI_ANY_ID, __dev)) != NULL)                            \
        {                                                                   \
            if ((NV_PCI_DOMAIN_NUMBER(__dev) == domain) &&                  \
                (NV_PCI_BUS_NUMBER(__dev) == bus) &&                        \
                (NV_PCI_DEVFN(__dev) == devfn))                             \
            {                                                               \
                break;                                                      \
            }                                                               \
        }                                                                   \
        if (__dev == NULL)                                                  \
        {                                                                   \
            while ((__dev = pci_get_class((PCI_CLASS_BRIDGE_HOST << 8),     \
                        __dev)) != NULL)                                    \
            {                                                               \
                if ((NV_PCI_DOMAIN_NUMBER(__dev) == domain) &&              \
                    (NV_PCI_BUS_NUMBER(__dev) == bus) &&                    \
                    (NV_PCI_DEVFN(__dev) == devfn))                         \
                {                                                           \
                    break;                                                  \
                }                                                           \
            }                                                               \
        }                                                                   \
        if (__dev == NULL)                                                  \
        {                                                                   \
            while ((__dev = pci_get_class((PCI_CLASS_BRIDGE_PCI << 8),      \
                        __dev)) != NULL)                                    \
            {                                                               \
                if ((NV_PCI_DOMAIN_NUMBER(__dev) == domain) &&              \
                    (NV_PCI_BUS_NUMBER(__dev) == bus) &&                    \
                    (NV_PCI_DEVFN(__dev) == devfn))                         \
                {                                                           \
                    break;                                                  \
                }                                                           \
            }                                                               \
        }                                                                   \
        if (__dev == NULL)                                                  \
        {                                                                   \
            while ((__dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID,          \
                            __dev)) != NULL)                                \
            {                                                               \
                if ((NV_PCI_DOMAIN_NUMBER(__dev) == domain) &&              \
                    (NV_PCI_BUS_NUMBER(__dev) == bus) &&                    \
                    (NV_PCI_DEVFN(__dev) == devfn))                         \
                {                                                           \
                    break;                                                  \
                }                                                           \
            }                                                               \
        }                                                                   \
        __dev;                                                              \
    })
#elif defined(NV_PCI_GET_DOMAIN_BUS_AND_SLOT_PRESENT)
#define NV_GET_DOMAIN_BUS_AND_SLOT(domain,bus, devfn) \
    pci_get_domain_bus_and_slot(domain, bus, devfn)
#else
#define NV_GET_DOMAIN_BUS_AND_SLOT(domain,bus,devfn)               \
   ({                                                              \
        struct pci_dev *__dev = NULL;                              \
        while ((__dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID,     \
                    __dev)) != NULL)                               \
        {                                                          \
            if ((NV_PCI_DOMAIN_NUMBER(__dev) == domain) &&         \
                (NV_PCI_BUS_NUMBER(__dev) == bus) &&               \
                (NV_PCI_DEVFN(__dev) == devfn))                    \
            {                                                      \
                break;                                             \
            }                                                      \
        }                                                          \
        __dev;                                                     \
    })
#endif

#if defined(NV_PCI_STOP_AND_REMOVE_BUS_DEVICE_PRESENT)  // introduced in 3.4.9
#define NV_PCI_STOP_AND_REMOVE_BUS_DEVICE(pci_dev) pci_stop_and_remove_bus_device(pci_dev)
#elif defined(NV_PCI_REMOVE_BUS_DEVICE_PRESENT) // introduced in 2.6
#define NV_PCI_STOP_AND_REMOVE_BUS_DEVICE(pci_dev) pci_remove_bus_device(pci_dev)
#endif

#define NV_PRINT_AT(nv_debug_level,at)                                           \
    {                                                                            \
        nv_printf(nv_debug_level,                                                \
            "NVRM: VM: %s:%d: 0x%p, %d page(s), count = %d, flags = 0x%08x, "    \
            "page_table = 0x%p\n",  __FUNCTION__, __LINE__, at,                  \
            at->num_pages, NV_ATOMIC_READ(at->usage_count),                      \
            at->flags, at->page_table);                                          \
    }

#define NV_PRINT_VMA(nv_debug_level,vma)                                                 \
    {                                                                                    \
        nv_printf(nv_debug_level,                                                        \
            "NVRM: VM: %s:%d: 0x%lx - 0x%lx, 0x%08x bytes @ 0x%016llx, 0x%p, 0x%p\n",    \
            __FUNCTION__, __LINE__, vma->vm_start, vma->vm_end, NV_VMA_SIZE(vma),        \
            NV_VMA_OFFSET(vma), NV_VMA_PRIVATE(vma), NV_VMA_FILE(vma));                  \
    }

#ifndef minor
# define minor(x) MINOR(x)
#endif

#if defined(cpu_relax)
#define NV_CPU_RELAX() cpu_relax()
#else
#define NV_CPU_RELAX() barrier()
#endif

#ifndef IRQ_RETVAL
typedef void irqreturn_t;
#define IRQ_RETVAL(a)
#endif

#if !defined(PCI_COMMAND_SERR)
#define PCI_COMMAND_SERR            0x100
#endif
#if !defined(PCI_COMMAND_INTX_DISABLE)
#define PCI_COMMAND_INTX_DISABLE    0x400
#endif

#ifndef PCI_CAP_ID_EXP
#define PCI_CAP_ID_EXP 0x10
#endif

/*
 * On Linux on PPC64LE enable basic support for Linux PCI error recovery (see
 * Documentation/PCI/pci-error-recovery.txt). Currently RM only supports error
 * notification and data collection, not actual recovery of the device.
 */
#if defined(NVCPU_PPC64LE) && defined(CONFIG_EEH)
#include <asm/eeh.h>
#define NV_PCI_ERROR_RECOVERY
#endif

/*
 * If the host OS has page sizes larger than 4KB, we may have a security
 * problem. Registers are typically grouped in 4KB pages, but if there are
 * larger pages, then the smallest userspace mapping possible (e.g., a page)
 * may give more access than intended to the user.
 */
#define NV_4K_PAGE_ISOLATION_REQUIRED(addr, size)                       \
    ((PAGE_SIZE > NV_RM_PAGE_SIZE) &&                                   \
     ((size) <= NV_RM_PAGE_SIZE) &&                                     \
     (((addr) >> NV_RM_PAGE_SHIFT) ==                                   \
        (((addr) + (size) - 1) >> NV_RM_PAGE_SHIFT)))

/*
 * The kernel may have a workaround for this, by providing a method to isolate
 * a single 4K page in a given mapping.
 */
#if (PAGE_SIZE > NV_RM_PAGE_SIZE) && defined(NVCPU_PPC64LE) && defined(NV_PAGE_4K_PFN)
    #define NV_4K_PAGE_ISOLATION_PRESENT
    #define NV_4K_PAGE_ISOLATION_MMAP_ADDR(addr)                        \
        ((NvP64)((void*)(((addr) >> NV_RM_PAGE_SHIFT) << PAGE_SHIFT)))
    #define NV_4K_PAGE_ISOLATION_MMAP_LEN(size)     PAGE_SIZE
    #define NV_4K_PAGE_ISOLATION_ACCESS_START(addr)                     \
        ((NvP64)((void*)((addr) & ~NV_RM_PAGE_MASK)))
    #define NV_4K_PAGE_ISOLATION_ACCESS_LEN(addr, size)                 \
        ((((addr) & NV_RM_PAGE_MASK) + size + NV_RM_PAGE_MASK) &        \
         ~NV_RM_PAGE_MASK)
    #define NV_PROT_4K_PAGE_ISOLATION NV_PAGE_4K_PFN
#endif

static inline int nv_remap_page_range(struct vm_area_struct *vma,
    unsigned long virt_addr, NvU64 phys_addr, NvU64 size, pgprot_t prot)
{
    int ret = -1;

#if defined(NV_4K_PAGE_ISOLATION_PRESENT) && defined(NV_PROT_4K_PAGE_ISOLATION)
    if ((size == PAGE_SIZE) &&
        ((pgprot_val(prot) & NV_PROT_4K_PAGE_ISOLATION) != 0))
    {
        /*
         * remap_4k_pfn() hardcodes the length to a single OS page, and checks
         * whether applying the page isolation workaround will cause PTE
         * corruption (in which case it will fail, and this is an unsupported
         * configuration).
         */
#if defined(NV_HASH__REMAP_4K_PFN_PRESENT)
        ret = hash__remap_4k_pfn(vma, virt_addr, (phys_addr >> PAGE_SHIFT), prot);
#else
        ret = remap_4k_pfn(vma, virt_addr, (phys_addr >> PAGE_SHIFT), prot);
#endif
    }
    else
#endif
    {
        ret = remap_pfn_range(vma, virt_addr, (phys_addr >> PAGE_SHIFT), size,
            prot);
    }

    return ret;
}

static inline pgprot_t nv_adjust_pgprot(pgprot_t vm_prot, NvU32 extra)
{
    pgprot_t prot = __pgprot(pgprot_val(vm_prot) | extra);
#if defined(CONFIG_AMD_MEM_ENCRYPT) && defined(NV_PGPROT_DECRYPTED_PRESENT)
    /*
     * When AMD memory encryption is enabled, device memory mappings with the
     * C-bit set read as 0xFF, so ensure the bit is cleared for user mappings.
     *
     * If cc_mkdec() is present, then pgprot_decrypted() can't be used.
     */
#if defined(NV_CC_MKDEC_PRESENT)
    prot =  __pgprot(__sme_clr(pgprot_val(vm_prot)));
#else
    prot = pgprot_decrypted(prot);
#endif
#endif

    return prot;
}

static inline int nv_io_remap_page_range(struct vm_area_struct *vma,
    NvU64 phys_addr, NvU64 size, NvU32 extra_prot)
{
    int ret = -1;
#if !defined(NV_XEN_SUPPORT_FULLY_VIRTUALIZED_KERNEL)
    ret = nv_remap_page_range(vma, vma->vm_start, phys_addr, size,
        nv_adjust_pgprot(vma->vm_page_prot, extra_prot));
#else
    ret = io_remap_pfn_range(vma, vma->vm_start, (phys_addr >> PAGE_SHIFT),
        size, nv_adjust_pgprot(vma->vm_page_prot, extra_prot));
#endif
    return ret;
}

static inline vm_fault_t nv_insert_pfn(struct vm_area_struct *vma,
    NvU64 virt_addr, NvU64 pfn, NvU32 extra_prot)
{
    /*
     * vm_insert_pfn{,_prot} replaced with vmf_insert_pfn{,_prot} in Linux 4.20
     */
#if defined(NV_VMF_INSERT_PFN_PROT_PRESENT)
    return vmf_insert_pfn_prot(vma, virt_addr, pfn,
             __pgprot(pgprot_val(vma->vm_page_prot) | extra_prot));
#else
    int ret = -EINVAL;
    /*
     * Only PPC64LE (NV_4K_PAGE_ISOLATION_PRESENT) requires extra_prot to be
     * used when remapping.
     *
     * vm_insert_pfn_prot() was added in Linux 4.4, whereas POWER9 support
     * was added in Linux 4.8.
     *
     * Rather than tampering with the vma to make use of extra_prot with
     * vm_insert_pfn() on older kernels, for now, just fail in this case, as
     * it's not expected to be used currently.
     */
#if defined(NV_VM_INSERT_PFN_PROT_PRESENT)
    ret = vm_insert_pfn_prot(vma, virt_addr, pfn,
        __pgprot(pgprot_val(vma->vm_page_prot) | extra_prot));
#elif !defined(NV_4K_PAGE_ISOLATION_PRESENT)
    ret = vm_insert_pfn(vma, virt_addr, pfn);
#endif
    switch (ret)
    {
        case 0:
        case -EBUSY:
            /*
             * EBUSY indicates that another thread already handled
             * the faulted range.
             */
            return VM_FAULT_NOPAGE;
        case -ENOMEM:
            return VM_FAULT_OOM;
        default:
            break;
    }
#endif /* defined(NV_VMF_INSERT_PFN_PROT_PRESENT) */
    return VM_FAULT_SIGBUS;
}


#define NV_PAGE_MASK    (NvU64)(long)PAGE_MASK

extern void *nvidia_stack_t_cache;

/*
 * On Linux, when a kmem cache is created, a new sysfs entry is created for the
 * same unless it's merged with an existing cache. Upstream Linux kernel commit
 * 3b7b314053d021601940c50b07f5f1423ae67e21 (version 4.12+) made cache
 * destruction asynchronous which creates a race between cache destroy and
 * create. A new cache created with attributes as a previous cache, which is
 * scheduled for destruction, can try to create a sysfs entry with the same
 * conflicting name. Upstream Linux kernel commit
 * d50d82faa0c964e31f7a946ba8aba7c715ca7ab0 (4.18) fixes this issue by cleaning
 * up sysfs entry within slab_mutex, so the entry is deleted before a cache with
 * the same attributes could be created.
 *
 * To workaround this kernel issue, we take two steps:
 * - Create unmergeable caches: a kmem_cache with a constructor is unmergeable.
 *   So, we define an empty contructor for the same. Creating an unmergeable
 *   cache ensures that the kernel doesn't generate an internal name and always
 *   uses our name instead.
 *
 * - Generate a unique cache name by appending the current timestamp (ns). We
 *   wait for the timestamp to increment by at least one to ensure that we do
 *   not hit a name conflict in cache create -> destroy (async) -> create cycle.
 */
#if defined(NV_KMEM_CACHE_HAS_KOBJ_REMOVE_WORK) && !defined(NV_SYSFS_SLAB_UNLINK_PRESENT)
static inline void nv_kmem_ctor_dummy(void *arg)
{
    (void)arg;
}
#else
#define nv_kmem_ctor_dummy NULL
#endif

#define NV_KMEM_CACHE_CREATE(name, type)    \
    nv_kmem_cache_create(name, sizeof(type), 0)

/* The NULL pointer check is required for kernels older than 4.3 */
#define NV_KMEM_CACHE_DESTROY(kmem_cache)   \
    if (kmem_cache != NULL)                 \
    {                                       \
        kmem_cache_destroy(kmem_cache);     \
    }

#define NV_KMEM_CACHE_ALLOC(kmem_cache)     \
    kmem_cache_alloc(kmem_cache, GFP_KERNEL)
#define NV_KMEM_CACHE_FREE(ptr, kmem_cache) \
    kmem_cache_free(kmem_cache, ptr)

static inline void *nv_kmem_cache_zalloc(struct kmem_cache *k, gfp_t flags)
{
#if defined(NV_KMEM_CACHE_HAS_KOBJ_REMOVE_WORK) && !defined(NV_SYSFS_SLAB_UNLINK_PRESENT)
    /*
     * We cannot call kmem_cache_zalloc directly as it adds the __GFP_ZERO
     * flag. This flag together with the presence of a slab constructor is
     * flagged as a potential bug by the Linux kernel since it is the role
     * of a constructor to fill an allocated object with the desired
     * pattern. In our case, we specified a (dummy) constructor as a
     * workaround for a bug and not to zero-initialize objects. So, we take
     * the pain here to memset allocated object ourselves.
     */
    void *object = kmem_cache_alloc(k, flags);
    if (object)
        memset(object, 0, kmem_cache_size(k));
    return object;
#else
    return kmem_cache_zalloc(k, flags);
#endif
}

static inline int nv_kmem_cache_alloc_stack(nvidia_stack_t **stack)
{
    nvidia_stack_t *sp = NULL;
#if defined(NVCPU_X86_64)
    sp = NV_KMEM_CACHE_ALLOC(nvidia_stack_t_cache);
    if (sp == NULL)
        return -ENOMEM;
    sp->size = sizeof(sp->stack);
    sp->top = sp->stack + sp->size;
#endif
    *stack = sp;
    return 0;
}

static inline void nv_kmem_cache_free_stack(nvidia_stack_t *stack)
{
#if defined(NVCPU_X86_64)
    if (stack != NULL)
    {
        NV_KMEM_CACHE_FREE(stack, nvidia_stack_t_cache);
    }
#endif
}

#if defined(NVCPU_X86_64)
/*
 * RAM is cached on Linux by default, we can assume there's
 * nothing to be done here. This is not the case for the
 * other memory spaces: we will have made an attempt to add
 * a WC MTRR for the frame buffer.
 *
 * If a WC MTRR is present, we can't satisfy the WB mapping
 * attempt here, since the achievable effective memory
 * types in that case are WC and UC, if not it's typically
 * UC (MTRRdefType is UC); we could only satisfy WB mapping
 * requests with a WB MTRR.
 */
#define NV_ALLOW_CACHING(mt)            ((mt) == NV_MEMORY_TYPE_SYSTEM)
#else
#define NV_ALLOW_CACHING(mt)            ((mt) != NV_MEMORY_TYPE_REGISTERS)
#endif

typedef struct nvidia_pte_s {
    NvU64           phys_addr;
    unsigned long   virt_addr;
    NvU64           dma_addr;
#ifdef CONFIG_XEN
    unsigned int    guest_pfn;
#endif
    unsigned int    page_count;
} nvidia_pte_t;











typedef struct nv_alloc_s {
    struct nv_alloc_s *next;
    struct device     *dev;
    atomic_t       usage_count;
    struct {
        NvBool contig      : 1;
        NvBool guest       : 1;
        NvBool zeroed      : 1;
        NvBool aliased     : 1;
        NvBool user        : 1;
        NvBool node0       : 1;
        NvBool peer_io     : 1;
        NvBool physical    : 1;
        NvBool unencrypted : 1;
        NvBool coherent    : 1;
    } flags;
    unsigned int   cache_type;
    unsigned int   num_pages;
    unsigned int   order;
    unsigned int   size;
    nvidia_pte_t **page_table;          /* list of physical pages allocated */
    unsigned int   pid;
    struct page  **user_pages;
    NvU64         guest_id;             /* id of guest VM */
    void          *import_priv;
    struct sg_table *import_sgt;
} nv_alloc_t;

/**
 * nv_is_dma_direct - return true if direct_dma is enabled
 *
 * Starting with the 5.0 kernel, SWIOTLB is merged into
 * direct_dma, so systems without an IOMMU use direct_dma.  We
 * need to know if this is the case, so that we can use a
 * different check for SWIOTLB enablement.
 */
static inline NvBool nv_is_dma_direct(struct device *dev)
{
    NvBool is_direct = NV_FALSE;

#if defined(NV_DMA_IS_DIRECT_PRESENT)
    if (dma_is_direct(get_dma_ops(dev)))
        is_direct = NV_TRUE;
#endif

    return is_direct;
}

/**
 * nv_dma_maps_swiotlb - return NV_TRUE if swiotlb is enabled
 *
 * SWIOTLB creates bounce buffers for the DMA mapping layer to
 * use if a driver asks the kernel to map a DMA buffer that is
 * outside of the device's addressable range.  The driver does
 * not function correctly if bounce buffers are enabled for the
 * device.  So if SWIOTLB is enabled, we should avoid making
 * mapping calls.
 */
static inline NvBool
nv_dma_maps_swiotlb(struct device *dev)
{
    NvBool swiotlb_in_use = NV_FALSE;
#if defined(CONFIG_SWIOTLB)
  #if defined(NV_DMA_OPS_PRESENT) || defined(NV_GET_DMA_OPS_PRESENT) || \
      defined(NV_SWIOTLB_DMA_OPS_PRESENT)
    /*
     * We only use the 'dma_ops' symbol on older x86_64 kernels; later kernels,
     * including those for other architectures, have converged on the
     * get_dma_ops() interface.
     */
    #if defined(NV_GET_DMA_OPS_PRESENT)
    /*
     * The __attribute__ ((unused)) is necessary because in at least one
     * case, *none* of the preprocessor branches below are taken, and
     * so the ops variable ends up never being referred to at all. This can
     * happen with the (NV_IS_EXPORT_SYMBOL_PRESENT_swiotlb_map_sg_attrs == 1)
     * case.
     */
    const struct dma_map_ops *ops __attribute__ ((unused)) = get_dma_ops(dev);
    #else
    const struct dma_mapping_ops *ops __attribute__ ((unused)) = dma_ops;
    #endif

    /*
     * The switch from dma_mapping_ops -> dma_map_ops coincided with the
     * switch from swiotlb_map_sg -> swiotlb_map_sg_attrs.
     */
      #if defined(NVCPU_AARCH64) && \
          defined(NV_NONCOHERENT_SWIOTLB_DMA_OPS_PRESENT)
    /* AArch64 exports these symbols directly */
    swiotlb_in_use = ((ops == &noncoherent_swiotlb_dma_ops) ||
                      (ops == &coherent_swiotlb_dma_ops));
      #elif NV_IS_EXPORT_SYMBOL_PRESENT_swiotlb_map_sg_attrs != 0
    swiotlb_in_use = (ops->map_sg == swiotlb_map_sg_attrs);
      #elif NV_IS_EXPORT_SYMBOL_PRESENT_swiotlb_dma_ops != 0
    swiotlb_in_use = (ops == &swiotlb_dma_ops);
      #endif
      /*
       * The "else" case that is not shown
       * (for NV_IS_EXPORT_SYMBOL_PRESENT_swiotlb_map_sg_attrs == 0 ||
       * NV_IS_EXPORT_SYMBOL_PRESENT_swiotlb_dma_ops == 0) does
       * nothing, and ends up dropping us out to the last line of this function,
       * effectively returning false. The nearly-human-readable version of that
       * case is "struct swiotlb_dma_ops is present (NV_SWIOTLB_DMA_OPS_PRESENT
       * is defined) but neither swiotlb_map_sg_attrs nor swiotlb_dma_ops is
       * present".
       *
       * That can happen on kernels that fall within below range:
       *
       * 2017-12-24  4bd89ed39b2ab8dc4ac4b6c59b07d420b0213bec
       *     ("swiotlb: remove various exports")
       * 2018-06-28  210d0797c97d0e8f3b1a932a0dc143f4c57008a3
       *     ("swiotlb: export swiotlb_dma_ops")
       *
       * Related to this: Between above two commits, this driver has no way of
       * detecting whether or not the SWIOTLB is in use. Furthermore, the
       * driver cannot support DMA remapping. That leads to the following
       * point: "swiotlb=force" is not supported for kernels falling in above
       * range.
       *
       * The other "else" case that is not shown:
       * Starting with the 5.0 kernel, swiotlb is integrated into dma_direct,
       * which is used when there's no IOMMU.  In these kernels, ops == NULL,
       * swiotlb_dma_ops no longer exists, and we do not support swiotlb=force
       * (doing so would require detecting when swiotlb=force is enabled and
       * then returning NV_TRUE even when dma_direct is in use).  So for now,
       * we just return NV_FALSE and in nv_compute_gfp_mask() we check for
       * whether swiotlb could possibly be used (outside of swiotlb=force).
       */
  #endif

    /*
     * Commit 2017-11-07 d7b417fa08d ("x86/mm: Add DMA support for
     * SEV memory encryption") forces SWIOTLB to be enabled when AMD SEV 
     * is active in all cases.
     */
    if (os_sev_enabled)
        swiotlb_in_use = NV_TRUE;
#endif

    return swiotlb_in_use;
}

/*
 * TODO: Bug 1522381 will allow us to move these mapping relationships into
 *       common code.
 */

/*
 * Bug 1606851: the Linux kernel scatterlist code doesn't work for regions
 * greater than or equal to 4GB, due to regular use of unsigned int
 * throughout. So we need to split our mappings into 4GB-minus-1-page-or-less
 * chunks and manage them separately.
 */
typedef struct nv_dma_submap_s {
    NvU32 page_count;
    NvU32 sg_map_count;
    struct sg_table sgt;
    NvBool imported;
} nv_dma_submap_t;

typedef struct nv_dma_map_s {
    struct page **pages;
    NvU64 page_count;
    NvBool contiguous;
    NvU32 cache_type;
    struct sg_table *import_sgt;

    union
    {
        struct
        {
            NvU32 submap_count;
            nv_dma_submap_t *submaps;
        } discontig;

        struct
        {
            NvU64 dma_addr;
        } contig;
    } mapping;

    struct device *dev;
} nv_dma_map_t;

#define NV_FOR_EACH_DMA_SUBMAP(dm, sm, i)                                     \
    for (i = 0, sm = &dm->mapping.discontig.submaps[0];                       \
         i < dm->mapping.discontig.submap_count;                              \
         i++, sm = &dm->mapping.discontig.submaps[i])

#define NV_DMA_SUBMAP_MAX_PAGES           ((NvU32)(NV_U32_MAX >> PAGE_SHIFT))
#define NV_DMA_SUBMAP_IDX_TO_PAGE_IDX(s)  (s * NV_DMA_SUBMAP_MAX_PAGES)

/*
 * DO NOT use sg_alloc_table_from_pages on Xen Server, even if it's available.
 * This will glom multiple pages into a single sg element, which
 * xen_swiotlb_map_sg_attrs may try to route to the SWIOTLB. We must only use
 * single-page sg elements on Xen Server.
 */
#if defined(NV_SG_ALLOC_TABLE_FROM_PAGES_PRESENT) && \
    !defined(NV_DOM0_KERNEL_PRESENT)
    #define NV_ALLOC_DMA_SUBMAP_SCATTERLIST(dm, sm, i)                        \
        ((sg_alloc_table_from_pages(&sm->sgt,                                 \
            &dm->pages[NV_DMA_SUBMAP_IDX_TO_PAGE_IDX(i)],                     \
            sm->page_count, 0,                                                \
            sm->page_count * PAGE_SIZE, NV_GFP_KERNEL) == 0) ? NV_OK :        \
                NV_ERR_OPERATING_SYSTEM)
#else
    #define NV_ALLOC_DMA_SUBMAP_SCATTERLIST(dm, sm, i)                \
        ((sg_alloc_table(&sm->sgt, sm->page_count, NV_GFP_KERNEL)) == \
            0 ? NV_OK : NV_ERR_OPERATING_SYSTEM)
#endif

typedef struct nv_ibmnpu_info nv_ibmnpu_info_t;

typedef struct nv_work_s {
    struct work_struct task;
    void *data;
} nv_work_t;

#define NV_MAX_REGISTRY_KEYS_LENGTH   512

typedef enum
{
    NV_DEV_STACK_TIMER,
    NV_DEV_STACK_ISR,
    NV_DEV_STACK_ISR_BH,
    NV_DEV_STACK_ISR_BH_UNLOCKED,
    NV_DEV_STACK_GPU_WAKEUP,
    NV_DEV_STACK_COUNT
} nvidia_linux_dev_stack_t;

/* Linux version of the opaque type used for os_queue_work_item() */
struct os_work_queue {
    nv_kthread_q_t nvk;
};

/* Linux version of the opaque type used for os_wait_*() */
struct os_wait_queue {
    struct completion q;
};





























/*
 * To report error in msi/msix when unhandled count reaches a threshold
 */

typedef struct nv_irq_count_info_s
{
    int    irq;
    NvU64  unhandled;
    NvU64  total;
    NvU64  last_unhandled;
} nv_irq_count_info_t;

/* Linux-specific version of nv_dma_device_t */
struct nv_dma_device {
    struct {
        NvU64 start;
        NvU64 limit;
    } addressable_range;

    struct device *dev;
    NvBool nvlink;
};














/* linux-specific version of old nv_state_t */
/* this is a general os-specific state structure. the first element *must* be
   the general state structure, for the generic unix-based code */
typedef struct nv_linux_state_s {
    nv_state_t nv_state;

    atomic_t usage_count;
    NvU32    suspend_count;

    struct device  *dev;
    struct pci_dev *pci_dev;

    /* IBM-NPU info associated with this GPU */
    nv_ibmnpu_info_t *npu;






    /* NUMA node information for the platforms where GPU memory is presented
     * as a NUMA node to the kernel */
    struct {
        /* NUMA node id >=0  when the platform supports GPU memory as NUMA node
         * otherwise it holds the value of NUMA_NO_NODE */
        NvS32 node_id;

        /* NUMA online/offline status for platforms that support GPU memory as
         * NUMA node */
        atomic_t status;
    } numa_info;

    nvidia_stack_t *sp[NV_DEV_STACK_COUNT];

    char registry_keys[NV_MAX_REGISTRY_KEYS_LENGTH];

    nv_work_t work;

    /* get a timer callback every second */
    struct nv_timer rc_timer;

    /* lock for linux-specific data, not used by core rm */
    struct semaphore ldata_lock;

    /* proc directory information */
    struct proc_dir_entry *proc_dir;

    NvU32 minor_num;
    struct nv_linux_state_s *next;

    /* DRM private information */
    struct drm_device *drm;

    /* kthread based bottom half servicing queue and elements */
    nv_kthread_q_t bottom_half_q;
    nv_kthread_q_item_t bottom_half_q_item;

    /* Lock for unlocked bottom half protecting common allocated stack */
    void *isr_bh_unlocked_mutex;

    NvBool tce_bypass_enabled;

    NvU32 num_intr;

    /* Lock serializing ISRs for different MSI-X vectors */
    nv_spinlock_t msix_isr_lock;

    /* Lock serializing bottom halves for different MSI-X vectors */
    void *msix_bh_mutex;

    struct msix_entry *msix_entries;

    NvU64 numa_memblock_size;

    struct {
        struct backlight_device *dev;
        NvU32 displayId;
        const char *device_name;
    } backlight;

    /*
     * file handle for pci sysfs config file (/sys/bus/pci/devices/.../config)
     * which will be opened during device probe
     */
    struct file *sysfs_config_file;

    /* Per-GPU queue */
    struct os_work_queue queue;

    /* GPU user mapping revocation/remapping (only for non-CTL device) */
    struct semaphore mmap_lock; /* Protects all fields in this category */
    struct list_head open_files;
    NvBool all_mappings_revoked;
    NvBool safe_to_mmap;
    NvBool gpu_wakeup_callback_needed;

    /* Per-device notifier block for ACPI events */
    struct notifier_block acpi_nb;


















    /* Lock serializing ISRs for different SOC vectors */
    nv_spinlock_t soc_isr_lock;

    struct nv_timer snapshot_timer;
    nv_spinlock_t snapshot_timer_lock;
    void (*snapshot_callback)(void *context);

    /* count for unhandled, total and timestamp of irq */
    nv_irq_count_info_t *irq_count;

    /* Max number of irq triggered and are getting tracked */
    NvU16 current_num_irq_tracked;

    NvBool is_forced_shutdown;

    struct nv_dma_device dma_dev;
    struct nv_dma_device niso_dma_dev;
} nv_linux_state_t;

extern nv_linux_state_t *nv_linux_devices;

/*
 * Macros to protect operations on nv_linux_devices list
 * Lock acquisition order while using the nv_linux_devices list
 * 1. LOCK_NV_LINUX_DEVICES()
 * 2. Traverse the list
 *    If the list is traversed to search for an element say nvl,
 *    acquire the nvl->ldata_lock before step 3
 * 3. UNLOCK_NV_LINUX_DEVICES()
 * 4. Release nvl->ldata_lock after any read/write access to the
 *    nvl element is complete
 */
extern struct semaphore nv_linux_devices_lock;
#define LOCK_NV_LINUX_DEVICES()     down(&nv_linux_devices_lock)
#define UNLOCK_NV_LINUX_DEVICES()   up(&nv_linux_devices_lock)

/*
 * Lock to synchronize system power management transitions,
 * and to protect the global system PM state.  The procfs power
 * management interface acquires this lock in write mode for
 * the duration of the sleep operation, any other paths accessing
 * device state must acquire the lock in read mode.
 */
extern struct rw_semaphore nv_system_pm_lock;

extern NvBool nv_ats_supported;

#if defined(NV_LINUX_ACPI_EVENTS_SUPPORTED)
/*
 * acpi data storage structure
 *
 * This structure retains the pointer to the device,
 * and any other baggage we want to carry along
 *
 */
typedef struct
{
    nvidia_stack_t *sp;
    struct acpi_device *device;
    struct acpi_handle *handle;
    int notify_handler_installed;
} nv_acpi_t;

#endif

/*
 * file-private data
 * hide a pointer to our data structures in a file-private ptr
 * there are times we need to grab this data back from the file
 * data structure..
 */

typedef struct nvidia_event
{
    struct nvidia_event *next;
    nv_event_t event;
} nvidia_event_t;

typedef enum
{
    NV_FOPS_STACK_INDEX_MMAP,
    NV_FOPS_STACK_INDEX_IOCTL,
    NV_FOPS_STACK_INDEX_COUNT
} nvidia_entry_point_index_t;

typedef struct
{
    nv_file_private_t nvfp;

    nvidia_stack_t *sp;
    nvidia_stack_t *fops_sp[NV_FOPS_STACK_INDEX_COUNT];
    struct semaphore fops_sp_lock[NV_FOPS_STACK_INDEX_COUNT];
    nv_alloc_t *free_list;
    void *nvptr;
    nvidia_event_t *event_data_head, *event_data_tail;
    NvBool dataless_event_pending;
    nv_spinlock_t fp_lock;
    wait_queue_head_t waitqueue;
    nv_kthread_q_item_t deferred_close_q_item;
    NvU32 *attached_gpus;
    size_t num_attached_gpus;
    nv_alloc_mapping_context_t mmap_context;
    struct address_space mapping;

    struct list_head entry;
} nv_linux_file_private_t;

static inline nv_linux_file_private_t *nv_get_nvlfp_from_nvfp(nv_file_private_t *nvfp)
{
    return container_of(nvfp, nv_linux_file_private_t, nvfp);
}

#define NV_SET_FILE_PRIVATE(filep,data) ((filep)->private_data = (data))
#define NV_GET_LINUX_FILE_PRIVATE(filep) ((nv_linux_file_private_t *)(filep)->private_data)

/* for the card devices */
#define NV_GET_NVL_FROM_FILEP(filep)    (NV_GET_LINUX_FILE_PRIVATE(filep)->nvptr)
#define NV_GET_NVL_FROM_NV_STATE(nv)    ((nv_linux_state_t *)nv->os_state)

#define NV_STATE_PTR(nvl)   &(((nv_linux_state_t *)(nvl))->nv_state)


#define NV_ATOMIC_READ(data)            atomic_read(&(data))
#define NV_ATOMIC_SET(data,val)         atomic_set(&(data), (val))
#define NV_ATOMIC_INC(data)             atomic_inc(&(data))
#define NV_ATOMIC_DEC(data)             atomic_dec(&(data))
#define NV_ATOMIC_DEC_AND_TEST(data)    atomic_dec_and_test(&(data))

static inline struct kmem_cache *nv_kmem_cache_create(const char *name, unsigned int size,
                                                      unsigned int align)
{
    char *name_unique;
    struct kmem_cache *cache;
 
#if defined(NV_KMEM_CACHE_HAS_KOBJ_REMOVE_WORK) && !defined(NV_SYSFS_SLAB_UNLINK_PRESENT)
    size_t len;
    NvU64 tm_ns = nv_ktime_get_raw_ns();

    /*
     * Wait for timer to change at least once. This ensures
     * that the name generated below is always unique.
     */
    while (tm_ns == nv_ktime_get_raw_ns());
    tm_ns = nv_ktime_get_raw_ns();

    /* 20 is the max length of a 64-bit integer printed in decimal */
    len = strlen(name) + 20 + 1;
    name_unique = kzalloc(len, GFP_KERNEL);
    if (!name_unique)
        return NULL;

    if (snprintf(name_unique, len, "%s-%llu", name, tm_ns) >= len)
    {
        WARN(1, "kmem cache name too long: %s\n", name);
        kfree(name_unique);
        return NULL;
    }
#else
    name_unique = (char *)name;
#endif
    cache = kmem_cache_create(name_unique, size, align, 0, nv_kmem_ctor_dummy);
    if (name_unique != name)
        kfree(name_unique);
 
    return cache;
}


#if defined(CONFIG_PCI_IOV)
#define NV_PCI_SRIOV_SUPPORT
#endif /* CONFIG_PCI_IOV */


#define NV_PCIE_CFG_MAX_OFFSET 0x1000

#include "nv-proto.h"

/*
 * Check if GPU is present on the bus by checking flag
 * NV_FLAG_IN_SURPRISE_REMOVAL(set when eGPU is removed from TB3).
 */
static inline NV_STATUS nv_check_gpu_state(nv_state_t *nv)
{
#if !defined(NVCPU_PPC64LE)
    if (NV_IS_DEVICE_IN_SURPRISE_REMOVAL(nv))
    {
        return NV_ERR_GPU_IS_LOST;
    }
#endif

    return NV_OK;
}

extern NvU32 NVreg_EnableUserNUMAManagement;
extern NvU32 NVreg_RegisterPCIDriver;

extern NvU32 num_probed_nv_devices;
extern NvU32 num_nv_devices;

#define NV_FILE_INODE(file) (file)->f_inode

#if defined(NV_DOM0_KERNEL_PRESENT) || defined(NV_VGPU_KVM_BUILD)
#define NV_VGX_HYPER
#if defined(NV_XEN_IOEMU_INJECT_MSI)
#include <xen/ioemu.h>
#endif
#endif

static inline NvU64 nv_pci_bus_address(struct pci_dev *dev, NvU8 bar_index)
{
    NvU64 bus_addr = 0;
#if defined(NV_PCI_BUS_ADDRESS_PRESENT)
    bus_addr = pci_bus_address(dev, bar_index);
#elif defined(CONFIG_PCI)
    struct pci_bus_region region;

    pcibios_resource_to_bus(dev, &region, &dev->resource[bar_index]);
    bus_addr = region.start;
#endif
    return bus_addr;
}

/*
 * Decrements the usage count of the allocation, and moves the allocation to
 * the given nvlfp's free list if the usage count drops to zero.
 *
 * Returns NV_TRUE if the allocation is moved to the nvlfp's free list.
 */
static inline NvBool nv_alloc_release(nv_linux_file_private_t *nvlfp, nv_alloc_t *at)
{
    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    if (NV_ATOMIC_DEC_AND_TEST(at->usage_count))
    {
        NV_ATOMIC_INC(at->usage_count);

        at->next = nvlfp->free_list;
        nvlfp->free_list = at;
        return NV_TRUE;
    }

    return NV_FALSE;
}

/*
 * RB_EMPTY_ROOT was added in 2.6.18 by this commit:
 *   2006-06-21  dd67d051529387f6e44d22d1d5540ef281965fdd
 */
#if !defined(RB_EMPTY_ROOT)
#define RB_EMPTY_ROOT(root) ((root)->rb_node == NULL)
#endif

/*
 * Starting on Power9 systems, DMA addresses for NVLink are no longer
 * the same as used over PCIe.
 *
 * Power9 supports a 56-bit Real Address. This address range is compressed
 * when accessed over NVLink to allow the GPU to access all of memory using
 * its 47-bit Physical address.
 *
 * If there is an NPU device present on the system, it implies that NVLink
 * sysmem links are present and we need to apply the required address
 * conversion for NVLink within the driver.
 *
 * See Bug 1920398 for further background and details.
 *
 * Note, a deviation from the documented compression scheme is that the
 * upper address bits (i.e. bit 56-63) instead of being set to zero are
 * preserved during NVLink address compression so the orignal PCIe DMA
 * address can be reconstructed on expansion. These bits can be safely
 * ignored on NVLink since they are truncated by the GPU.
 *
 * Bug 1968345: As a performance enhancement it is the responsibility of
 * the caller on PowerPC platforms to check for presence of an NPU device
 * before the address transformation is applied.
 */
static inline NvU64 nv_compress_nvlink_addr(NvU64 addr)
{
    NvU64 addr47 = addr;

#if defined(NVCPU_PPC64LE)
    addr47 = addr & ((1ULL << 43) - 1);
    addr47 |= (addr & (0x3ULL << 45)) >> 2;
    WARN_ON(addr47 & (1ULL << 44));
    addr47 |= (addr & (0x3ULL << 49)) >> 4;
    addr47 |= addr & ~((1ULL << 56) - 1);
#endif

    return addr47;
}

static inline NvU64 nv_expand_nvlink_addr(NvU64 addr47)
{
    NvU64 addr = addr47;

#if defined(NVCPU_PPC64LE)
    addr = addr47 & ((1ULL << 43) - 1);
    addr |= (addr47 & (3ULL << 43)) << 2;
    addr |= (addr47 & (3ULL << 45)) << 4;
    addr |= addr47 & ~((1ULL << 56) - 1);
#endif

    return addr;
}

// Default flags for ISRs
static inline NvU32 nv_default_irq_flags(nv_state_t *nv)
{
    NvU32 flags = 0;

    /*
     * Request IRQs to be disabled in our ISRs to keep consistency across the
     * supported kernel versions.
     *
     * IRQF_DISABLED has been made the default in 2.6.35 with commit e58aa3d2d0cc
     * from March 2010. And it has been later completely removed in 4.1 with commit
     * d8bf368d0631 from March 2015. Add it to our flags if it's defined to get the
     * same behaviour on pre-2.6.35 kernels as on recent ones.
     */
#if defined(IRQF_DISABLED)
    flags |= IRQF_DISABLED;
#endif

    /*
     * For legacy interrupts, also allow sharing. Sharing doesn't make sense
     * for MSI(-X) as on Linux they are never shared across different devices
     * and we only register one ISR today.
     */
    if ((nv->flags & (NV_FLAG_USES_MSI | NV_FLAG_USES_MSIX)) == 0)
        flags |= IRQF_SHARED;

    return flags;
}

/*
 * From v3.7-rc1 kernel have stopped exporting get_unused_fd() and started
 * exporting get_unused_fd_flags(), as of this commit:
 * 2012-09-26 1a7bd2265fc ("make get_unused_fd_flags() a function")
 */
#if NV_IS_EXPORT_SYMBOL_PRESENT_get_unused_fd
    #define NV_GET_UNUSED_FD()  get_unused_fd()
#else
    #define NV_GET_UNUSED_FD()  get_unused_fd_flags(0)
#endif

#if NV_IS_EXPORT_SYMBOL_PRESENT_get_unused_fd_flags
    #define NV_GET_UNUSED_FD_FLAGS(flags)  get_unused_fd_flags(flags)
#else
    #define NV_GET_UNUSED_FD_FLAGS(flags)  (-1)
#endif

#if defined(NV_SET_CLOSE_ON_EXEC_PRESENT)
    #define NV_SET_CLOSE_ON_EXEC(fd, fdt) __set_close_on_exec(fd, fdt)
#elif defined(NV_LINUX_TIME_H_PRESENT) && defined(FD_SET)
    #define NV_SET_CLOSE_ON_EXEC(fd, fdt) FD_SET(fd, fdt->close_on_exec)
#else
    #define NV_SET_CLOSE_ON_EXEC(fd, fdt) __set_bit(fd, fdt->close_on_exec)
#endif

#define MODULE_BASE_NAME "nvidia"
#define MODULE_INSTANCE_NUMBER 0
#define MODULE_INSTANCE_STRING ""
#define MODULE_NAME MODULE_BASE_NAME MODULE_INSTANCE_STRING

NvS32 nv_request_soc_irq(nv_linux_state_t *, NvU32, nv_soc_irq_type_t, NvU32, NvU32);






static inline void nv_mutex_destroy(struct mutex *lock)
{
    mutex_destroy(lock);
}

static inline NvBool nv_platform_supports_numa(nv_linux_state_t *nvl)
{
    return nvl->numa_info.node_id != NUMA_NO_NODE;
}

static inline int nv_get_numa_status(nv_linux_state_t *nvl)
{
    if (!nv_platform_supports_numa(nvl))
    {
        return NV_IOCTL_NUMA_STATUS_DISABLED;
    }

    return NV_ATOMIC_READ(nvl->numa_info.status);
}

static inline int nv_set_numa_status(nv_linux_state_t *nvl, int status)
{
    if (!nv_platform_supports_numa(nvl))
    {
        return -EINVAL;
    }

    NV_ATOMIC_SET(nvl->numa_info.status, status);
    return 0;
}

typedef enum
{
    NV_NUMA_STATUS_DISABLED             = 0,
    NV_NUMA_STATUS_OFFLINE              = 1,
    NV_NUMA_STATUS_ONLINE_IN_PROGRESS   = 2,
    NV_NUMA_STATUS_ONLINE               = 3,
    NV_NUMA_STATUS_ONLINE_FAILED        = 4,
    NV_NUMA_STATUS_OFFLINE_IN_PROGRESS  = 5,
    NV_NUMA_STATUS_OFFLINE_FAILED       = 6,
    NV_NUMA_STATUS_COUNT
} nv_numa_status_t;

#if defined(NV_LINUX_PLATFORM_DEVICE_H_PRESENT)
#include <linux/platform_device.h>
#endif

#if defined(NV_LINUX_MUTEX_H_PRESENT)
#include <linux/mutex.h>
#endif

#if defined(NV_LINUX_RESET_H_PRESENT)
#include <linux/reset.h>
#endif

#if defined(NV_LINUX_DMA_BUF_H_PRESENT)
#include <linux/dma-buf.h>
#endif

#if defined(NV_LINUX_GPIO_H_PRESENT)
#include <linux/gpio.h>
#endif

#if defined(NV_LINUX_OF_GPIO_H_PRESENT)
#include <linux/of_gpio.h>
#endif

#if defined(NV_LINUX_OF_DEVICE_H_PRESENT)
#include <linux/of_device.h>
#endif

#if defined(NV_LINUX_OF_PLATFORM_H_PRESENT)
#include <linux/of_platform.h>
#endif

#if defined(NV_LINUX_INTERCONNECT_H_PRESENT)
#include <linux/interconnect.h>
#endif

#if defined(NV_LINUX_PM_RUNTIME_H_PRESENT)
#include <linux/pm_runtime.h>
#endif

#if defined(NV_LINUX_CLK_H_PRESENT)
#include <linux/clk.h>
#endif

#if defined(NV_LINUX_CLK_PROVIDER_H_PRESENT)
#include <linux/clk-provider.h>
#endif

#endif  /* _NV_LINUX_H_ */
