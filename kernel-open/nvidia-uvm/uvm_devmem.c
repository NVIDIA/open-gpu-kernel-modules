/*******************************************************************************
    Copyright (c) 2015-2026 NVIDIA Corporation

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

#include "uvm_forward_decl.h"
#include "uvm_api.h"
#include "uvm_linux.h"
#include "uvm_types.h"
#include "uvm_va_space.h"
#include "uvm_devmem.h"
#include "uvm_pmm_gpu.h"

#if defined(CONFIG_PCI_P2PDMA) && defined(NV_STRUCT_PAGE_HAS_ZONE_DEVICE_DATA)
#include <linux/pci-p2pdma.h>
#endif

#if UVM_IS_CONFIG_HMM() || UVM_CDMM_PAGES_SUPPORTED()

uvm_gpu_chunk_t *uvm_devmem_page_to_chunk(struct page *page)
{
    return page_get_zone_device_chunk_data(page);
}
#endif

#if UVM_IS_CONFIG_HMM()
uvm_va_space_t *uvm_devmem_page_to_va_space(struct page *page)
{
    uvm_gpu_chunk_t *gpu_chunk = uvm_devmem_page_to_chunk(page);

    // uvm_hmm_unregister_gpu() needs to do a racy check here so
    // page->zone_device_data might be NULL.
    if (!gpu_chunk || !gpu_chunk->va_space)
        return NULL;

    // Use the cached va_space rather than gpu_chunk->va_block->hmm.va_space
    // since the va_block may have been freed while the va_space is still valid.
    return gpu_chunk->va_space;
}

// Check there are no orphan pages. This should be only called as part of
// removing a GPU: after all work is stopped and all va_blocks have been
// destroyed. By now there should be no device-private page references left as
// there are no va_space's left on this GPU and orphan pages should be removed
// by va_space destruction or unregistration from the GPU.
bool uvm_devmem_check_orphan_pages(uvm_pmm_gpu_t *pmm)
{
    size_t i;
    bool ret = true;
    uvm_gpu_t *gpu = uvm_pmm_to_gpu(pmm);
    unsigned long devmem_start;
    unsigned long devmem_end;
    unsigned long pfn;

    if (!pmm->initialized || !uvm_hmm_is_enabled_system_wide() || !gpu->parent->devmem)
        return ret;

    devmem_start = gpu->parent->devmem->pagemap.range.start + gpu->mem_info.phys_start;
    devmem_end = devmem_start + gpu->mem_info.size;

    // Scan all the root chunks looking for subchunks which are still
    // referenced.
    for (i = 0; i < pmm->root_chunks.count; i++) {
        uvm_gpu_root_chunk_t *root_chunk = &pmm->root_chunks.array[i];

        uvm_pmm_gpu_root_chunk_lock(pmm, root_chunk);
        if (root_chunk->chunk.state == UVM_PMM_GPU_CHUNK_STATE_IS_SPLIT)
            ret = false;
        uvm_pmm_gpu_root_chunk_unlock(pmm, root_chunk);
    }

    for (pfn = __phys_to_pfn(devmem_start); pfn <= __phys_to_pfn(devmem_end); pfn++) {
        struct page *page = pfn_to_page(pfn);

        if (!uvm_is_device_page(page)) {
            ret = false;
            break;
        }

        if (page->zone_device_data) {
            ret = false;
            break;
        }

        if (page_count(page)) {
            ret = false;
            break;
        }
    }

    return ret;
}
#endif

#if UVM_IS_CONFIG_HMM() || UVM_CDMM_PAGES_SUPPORTED()
static void devmem_page_free_gpu_chunk(uvm_gpu_chunk_t *chunk)
{
    uvm_gpu_t *gpu = uvm_gpu_chunk_get_gpu(chunk);

    UVM_ASSERT(chunk->va_space);

    // Use the cached va_space rather than chunk->va_block->hmm.va_space. The
    // va_block may already have been freed by the time this page_free() callback
    // runs, but the va_space is guaranteed to outlive the device page.
    if (chunk->va_space) {
        uvm_va_space_t *va_space = chunk->va_space;

        atomic64_dec(&va_space->hmm.allocated_page_count);
        UVM_ASSERT(atomic64_read(&va_space->hmm.allocated_page_count) >= 0);
    }

    // We should be calling free_chunk() except that it acquires a mutex and
    // we may be in an interrupt context where we can't do that. Instead,
    // do a lazy free. Note that we have to use a "normal" spin lock because
    // the UVM context is not available.
    spin_lock(&gpu->pmm.list_lock.lock);

    UVM_ASSERT(chunk->is_referenced);

    chunk->va_block = NULL;
    chunk->va_space = NULL;
    chunk->is_referenced = false;

    if (chunk->state == UVM_PMM_GPU_CHUNK_STATE_ALLOCATED) {
        list_del_init(&chunk->list);
        uvm_pmm_gpu_chunk_pin(&gpu->pmm, chunk);
    }
    else {
        UVM_ASSERT(chunk->state == UVM_PMM_GPU_CHUNK_STATE_TEMP_PINNED);
        UVM_ASSERT(list_empty(&chunk->list));
    }

    list_add_tail(&chunk->list, &gpu->pmm.root_chunks.va_block_lazy_free);

    spin_unlock(&gpu->pmm.list_lock.lock);

    nv_kthread_q_schedule_q_item(&gpu->parent->lazy_free_q,
                                 &gpu->pmm.root_chunks.va_block_lazy_free_q_item);
}
#endif

#if UVM_IS_CONFIG_HMM()
static void devmem_page_free(struct page *page)
{
    uvm_gpu_chunk_t *chunk = uvm_devmem_page_to_chunk(page);

    page->zone_device_data = NULL;
    devmem_page_free_gpu_chunk(chunk);
}

#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
static void devmem_folio_free(struct folio *folio)
{
    devmem_page_free(&folio->page);
}
#endif

// This is called by HMM when the CPU faults on a ZONE_DEVICE private entry.
static vm_fault_t devmem_fault(struct vm_fault *vmf)
{
    uvm_va_space_t *va_space = uvm_devmem_page_to_va_space(vmf->page);

    if (!va_space)
        return VM_FAULT_SIGBUS;

    return uvm_va_space_cpu_fault_hmm(va_space, vmf);
}

static vm_fault_t devmem_fault_entry(struct vm_fault *vmf)
{
    UVM_ENTRY_RET(devmem_fault(vmf));
}

static const struct dev_pagemap_ops uvm_devmem_ops =
{
#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
    .folio_free = devmem_folio_free,
#else
    .page_free = devmem_page_free,
#endif
    .migrate_to_ram = devmem_fault_entry,
};

// Allocating and initialising device private pages takes a significant amount
// of time on very large systems. So rather than do that everytime a GPU is
// registered we do it once and keep track of the range when the GPU is
// unregistered for later reuse.
//
// This function tries to find an exsiting range of device private pages and if
// available allocates and returns it for reuse.
static uvm_pmm_gpu_devmem_t *devmem_reuse_pagemap(unsigned long size)
{
    uvm_pmm_gpu_devmem_t *devmem;

    list_for_each_entry(devmem, &g_uvm_global.devmem_ranges.list, list_node) {
        if (devmem->size == size) {
            list_del(&devmem->list_node);
            return devmem;
        }
    }

    return NULL;
}

static uvm_pmm_gpu_devmem_t *devmem_alloc_pagemap(unsigned long size)
{
    uvm_pmm_gpu_devmem_t *devmem;
    struct resource *res;
    void *ptr;
    NV_STATUS status;

    res = request_free_mem_region(&iomem_resource, size, "nvidia-uvm-hmm");
    if (IS_ERR(res)) {
        UVM_ERR_PRINT("request_free_mem_region() err %ld\n", PTR_ERR(res));
        status = errno_to_nv_status(PTR_ERR(res));
        return NULL;
    }

    devmem = kzalloc(sizeof(*devmem), GFP_KERNEL);
    if (!devmem)
        goto err;

    devmem->size = size;
    devmem->pagemap.type = MEMORY_DEVICE_PRIVATE;
    devmem->pagemap.range.start = res->start;
    devmem->pagemap.range.end = res->end;
    devmem->pagemap.nr_range = 1;
    devmem->pagemap.ops = &uvm_devmem_ops;
    devmem->pagemap.owner = &g_uvm_global;

    // Numa node ID doesn't matter for ZONE_DEVICE private pages.
    ptr = memremap_pages(&devmem->pagemap, NUMA_NO_NODE);
    if (IS_ERR(ptr)) {
        UVM_ERR_PRINT("memremap_pages() err %ld\n", PTR_ERR(ptr));
        status = errno_to_nv_status(PTR_ERR(ptr));
        goto err_free;
    }

    return devmem;

err_free:
    kfree(devmem);

err:
    release_mem_region(res->start, resource_size(res));
    return NULL;
}

unsigned long uvm_devmem_get_gpu_pfn(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk)
{
    uvm_gpu_t *gpu = uvm_pmm_to_gpu(pmm);
    unsigned long devmem_start = gpu->parent->devmem->pagemap.range.start;

    return (devmem_start + chunk->address) >> PAGE_SHIFT;
}
#else // UVM_IS_CONFIG_HMM()
static void *devmem_alloc_pagemap(unsigned long size) { return NULL; }
static void *devmem_reuse_pagemap(unsigned long size) { return NULL; }
#endif // UVM_IS_CONFIG_HMM()

#if defined(CONFIG_PCI_P2PDMA)
// PCI P2PDMA device caching functions
// Similar to devmem caching, we cache PCI devices to avoid expensive
// registration/unregistration cycles during GPU add/remove operations.

// This function tries to find an existing PCI device registration and if
// available returns it for reuse.
static uvm_pmm_gpu_pci_dev_list_t *uvm_devmem_pci_p2pdma_reuse_device(struct pci_dev *pdev)
{
    uvm_pmm_gpu_pci_dev_list_t *pci_dev_entry;

    list_for_each_entry(pci_dev_entry, &g_uvm_global.pci_p2pdma_devices.list, list_node) {
        if (pci_dev_entry->pdev == pdev) {
            struct dev_pagemap *pgmap = NV_GET_DEV_PAGEMAP(pci_dev_entry->dev_start_pfn);

            // If a valid pagemap is present for the start pfn, the p2pdma registration
            // is valid and can be reused.
            if (pgmap) {
                put_dev_pagemap(pgmap);
                return pci_dev_entry;
            }
            else {
                // The device was present, but hotplug or another event such as unbind
                // removed the device from underneath, start fresh.
                list_del_init(&pci_dev_entry->list_node);
                uvm_kvfree(pci_dev_entry);
                break;
            }
        }
    }

    return NULL;
}

// This function caches a PCI device registration for later reuse.
static void uvm_devmem_pci_p2pdma_cache_device(struct pci_dev *pdev, unsigned long dev_start_pfn)
{
    uvm_pmm_gpu_pci_dev_list_t *pci_dev_entry;

    pci_dev_entry = uvm_kvmalloc_zero(sizeof(*pci_dev_entry));
    if (!pci_dev_entry) {
        // If we can't allocate memory for caching, we'll just skip caching
        // This is not a fatal error, just means we'll have to re-register next time
        return;
    }

    pci_dev_entry->pdev = pdev;
    pci_dev_entry->func = NV_PCI_DEVFN(pdev);
    pci_dev_entry->bus = NV_PCI_BUS_NUMBER(pdev);
    pci_dev_entry->domain = NV_PCI_DOMAIN_NUMBER(pdev);
    pci_dev_entry->dev_start_pfn = dev_start_pfn;

    list_add_tail(&pci_dev_entry->list_node, &g_uvm_global.pci_p2pdma_devices.list);
}

// PCI P2PDMA cleanup function
void uvm_devmem_pci_p2pdma_cache_exit(void)
{
    uvm_pmm_gpu_pci_dev_list_t *pci_dev_entry, *pci_dev_next;

    list_for_each_entry_safe(pci_dev_entry, pci_dev_next, &g_uvm_global.pci_p2pdma_devices.list, list_node) {

        // If there was a race with PCI hotplug, the devm release action would free the
        // memory associated with the pci p2p dma memory. Otherwise get a reference
        // to the device and ensure it's still present before freeing pages
        if (pci_device_is_present(pci_dev_entry->pdev)) {
            struct pci_dev *pdev = NV_GET_DOMAIN_BUS_AND_SLOT(pci_dev_entry->domain, pci_dev_entry->bus, pci_dev_entry->func);

            if (pdev) {
                struct dev_pagemap *pgmap;

                UVM_ASSERT(pdev == pci_dev_entry->pdev);

                pgmap = NV_GET_DEV_PAGEMAP(pci_dev_entry->dev_start_pfn);
                if (pgmap) {
                    put_dev_pagemap(pgmap);
                    devm_memunmap_pages(&pdev->dev, pgmap);
                }

                pci_dev_put(pdev);
            }

            list_del(&pci_dev_entry->list_node);
            uvm_kvfree(pci_dev_entry);
        }
    }
}
#else // CONFIG_PCI_P2PDMA
void uvm_devmem_pci_p2pdma_cache_exit(void)
{

}
#endif // CONFIG_PCI_P2PDMA

#if (UVM_CDMM_PAGES_SUPPORTED() || defined(CONFIG_PCI_P2PDMA)) && defined(NV_STRUCT_PAGE_HAS_ZONE_DEVICE_DATA)
struct kmem_cache *g_uvm_coherent_devmem_page_cache;

static void device_p2p_page_free_wake(struct nv_kref *ref)
{
    uvm_device_p2p_mem_t *p2p_mem = container_of(ref, uvm_device_p2p_mem_t, refcount);
    wake_up(&p2p_mem->waitq);
}

static void device_p2p_page_free(struct page *page)
{
    uvm_device_p2p_mem_t *p2p_mem = page->zone_device_data;

    if (!page->zone_device_data)
        return;

    page->zone_device_data = NULL;
    nv_kref_put(&p2p_mem->refcount, device_p2p_page_free_wake);
}

#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
static void device_p2p_folio_free(struct folio *folio)
{
    device_p2p_page_free(&folio->page);
}
#endif

NV_STATUS uvm_devmem_global_init(void)
{
    g_uvm_coherent_devmem_page_cache = NV_KMEM_CACHE_CREATE("uvm_coherent_devmem_page_cache",
                                                            uvm_coherent_devmem_page_t);
    if (!g_uvm_coherent_devmem_page_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_devmem_global_deinit(void)
{
    kmem_cache_destroy_safe(&g_uvm_coherent_devmem_page_cache);
}
#else // (UVM_CDMM_PAGES_SUPPORTED || defined(CONFIG_PCI_P2PDMA)) && defined(NV_STRUCT_PAGE_HAS_ZONE_DEVICE_DATA)
NV_STATUS uvm_devmem_global_init(void) { return NV_OK; }
void uvm_devmem_global_deinit(void) {}
#endif

#if UVM_CDMM_PAGES_SUPPORTED()
static void device_coherent_page_free(struct page *page)
{
    uvm_gpu_chunk_t *chunk = uvm_devmem_page_to_chunk(page);
    uvm_device_p2p_mem_t *p2p_mem = page_get_zone_device_p2p_data(page);

    // DEVICE_COHERENT pages used for managed memory have no zone_device_data
    // at all.
    if (!page->zone_device_data) {
        return;
    }

    kmem_cache_free(g_uvm_coherent_devmem_page_cache, page->zone_device_data);
    page->zone_device_data = NULL;

    if (chunk)
        devmem_page_free_gpu_chunk(chunk);

    if (p2p_mem)
        nv_kref_put(&p2p_mem->refcount, device_p2p_page_free_wake);
}

#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
static void device_coherent_folio_free(struct folio *folio)
{
    device_coherent_page_free(&folio->page);
}
#endif

static const struct dev_pagemap_ops uvm_device_coherent_pgmap_ops =
{
#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
    .folio_free = device_coherent_folio_free,
#else
    .page_free = device_coherent_page_free,
#endif
};

static NV_STATUS uvm_devmem_cdmm_init(uvm_parent_gpu_t *parent_gpu)
{
    uvm_pmm_gpu_devmem_t *devmem;
    void *ptr;
    NV_STATUS status;

    UVM_ASSERT(g_uvm_coherent_devmem_page_cache);

    list_for_each_entry(devmem, &g_uvm_global.devmem_ranges.list, list_node) {
        if (devmem->pagemap.range.start == parent_gpu->system_bus.memory_window_start) {
            UVM_ASSERT(devmem->pagemap.type == MEMORY_DEVICE_COHERENT);
            UVM_ASSERT(devmem->pagemap.range.end ==
                       SUBSECTION_ALIGN_UP(parent_gpu->system_bus.memory_window_end >> PAGE_SHIFT) << PAGE_SHIFT);
            list_del(&devmem->list_node);
            parent_gpu->devmem = devmem;
            parent_gpu->device_p2p_initialised = true;
            return NV_OK;
        }
    }

    devmem = kzalloc(sizeof(*devmem), GFP_KERNEL);
    if (!devmem)
        goto err;

    devmem->size = parent_gpu->system_bus.memory_window_end - parent_gpu->system_bus.memory_window_start;
    devmem->pagemap.type = MEMORY_DEVICE_COHERENT;
    devmem->pagemap.range.start = parent_gpu->system_bus.memory_window_start;
    devmem->pagemap.range.end = SUBSECTION_ALIGN_UP(parent_gpu->system_bus.memory_window_end >> PAGE_SHIFT) << PAGE_SHIFT;
    devmem->pagemap.nr_range = 1;
    devmem->pagemap.ops = &uvm_device_coherent_pgmap_ops;
    devmem->pagemap.owner = &g_uvm_global;

    // Numa node ID doesn't matter for ZONE_DEVICE coherent pages.
    ptr = memremap_pages(&devmem->pagemap, NUMA_NO_NODE);
    if (IS_ERR(ptr)) {
        UVM_ERR_PRINT("memremap_pages() err %ld\n", PTR_ERR(ptr));
        status = errno_to_nv_status(PTR_ERR(ptr));
        goto err_free;
    }

    parent_gpu->devmem = devmem;
    parent_gpu->device_p2p_initialised = true;

    return NV_OK;

err_free:
    kfree(devmem);

err:
    return NV_ERR_NOT_SUPPORTED;
}

static void uvm_devmem_cdmm_deinit(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->device_p2p_initialised = false;
    list_add_tail(&parent_gpu->devmem->list_node, &g_uvm_global.devmem_ranges.list);
    parent_gpu->devmem = NULL;
}
#else // UVM_CDMM_PAGES_SUPPORTED
static NV_STATUS uvm_devmem_cdmm_init(uvm_parent_gpu_t *parent_gpu) { return NV_OK; }
static void uvm_devmem_cdmm_deinit(uvm_parent_gpu_t *parent_gpu) {}
#endif // UVM_CDMM_PAGES_SUPPORTED

#if UVM_IS_CONFIG_HMM() || UVM_CDMM_PAGES_SUPPORTED()
NV_STATUS uvm_devmem_init(uvm_parent_gpu_t *gpu)
{
    // Create a DEVICE_PRIVATE page for every GPU page available on the parent.
    unsigned long size = gpu->max_allocatable_address;

    if (gpu->cdmm_enabled)
        return uvm_devmem_cdmm_init(gpu);

    if (!uvm_hmm_is_enabled_system_wide() ||
        !UVM_CAN_USE_DEVICE_PRIVATE_MEMREMAP_PAGES()) {
        gpu->devmem = NULL;
        return NV_OK;
    }

    if (gpu->test.inject_error.disable_devmem)
        return NV_ERR_BUSY_RETRY;

    gpu->devmem = devmem_reuse_pagemap(size);
    if (!gpu->devmem)
        gpu->devmem = devmem_alloc_pagemap(size);

    if (!gpu->devmem)
        return NV_ERR_BUSY_RETRY;

    return NV_OK;
}

void uvm_devmem_deinit(uvm_parent_gpu_t *gpu)
{
    if (gpu->cdmm_enabled && gpu->devmem) {
        uvm_devmem_cdmm_deinit(gpu);
        return;
    }

    if (!gpu->devmem)
        return;

    list_add_tail(&gpu->devmem->list_node, &g_uvm_global.devmem_ranges.list);
    gpu->devmem = NULL;
}

void uvm_devmem_exit(void)
{
    uvm_pmm_gpu_devmem_t *devmem, *devmem_next;

    list_for_each_entry_safe(devmem, devmem_next, &g_uvm_global.devmem_ranges.list, list_node) {
        list_del(&devmem->list_node);
        memunmap_pages(&devmem->pagemap);
        if (devmem->pagemap.type == MEMORY_DEVICE_PRIVATE)
            release_mem_region(devmem->pagemap.range.start, range_len(&devmem->pagemap.range));
        kfree(devmem);
    }
}
#else
NV_STATUS uvm_devmem_init(uvm_parent_gpu_t *gpu) { return NV_OK; }
void uvm_devmem_deinit(uvm_parent_gpu_t *gpu) {}
void uvm_devmem_exit(void) {}
#endif

#if !UVM_IS_CONFIG_HMM()
bool uvm_devmem_check_orphan_pages(uvm_pmm_gpu_t *pmm)
{
    return true;
}
#endif // UVM_IS_CONFIG_HMM()

// PCI P2PDMA pages are not well supported by the kernel/architecture on all
// ARM64 based systems so disable support for those systems.
// TODO: Bug 5303506: ARM64: P2PDMA pages cannot be accessed from the CPU on
// ARM
#if defined(CONFIG_PCI_P2PDMA) && defined(NV_STRUCT_PAGE_HAS_ZONE_DEVICE_DATA) && !defined(NVCPU_AARCH64)

static const struct dev_pagemap_ops uvm_device_p2p_pgmap_ops =
{
#if defined(NV_PAGEMAP_OPS_HAS_FOLIO_FREE)
    .folio_free = device_p2p_folio_free,
#else
    .page_free = device_p2p_page_free,
#endif
};

void uvm_devmem_device_p2p_init(uvm_parent_gpu_t *parent_gpu)
{
    unsigned long pci_start_pfn = pci_resource_start(parent_gpu->pci_dev,
                                                     uvm_device_p2p_static_bar(parent_gpu)) >> PAGE_SHIFT;
    unsigned long pci_end_pfn = pci_start_pfn + (parent_gpu->static_bar1_size >> PAGE_SHIFT);
    struct page *p2p_page;
    uvm_pmm_gpu_pci_dev_list_t *cached_pci_dev;

    if (uvm_parent_gpu_is_coherent(parent_gpu)) {
        // P2PDMA support with CDMM enabled requires special
        // MEMORY_DEVICE_COHERENT pages to have been allocated which will have
        // also set the p2p initialised state if successful.
        if (parent_gpu->cdmm_enabled)
            return;

        parent_gpu->device_p2p_initialised = true;
        return;
    }

    parent_gpu->device_p2p_initialised = false;

    // RM sets static_bar1_size when it has created a contiguous BAR mapping
    // large enough to cover all of GPU memory that will be allocated to
    // userspace buffers. This is required to support the P2PDMA feature to
    // ensure we have a P2PDMA page available for every mapping.
    //
    // Due to current limitations in the Linux kernel we can only create
    // the P2PDMA pages if the BAR1 region has not already been mapped
    // write-combined. By default RM maps the region write-combined, but this
    // can be disabled by setting the RmForceDisableIomapWC regkey which allows
    // creation of the P2PDMA pages.
    // TODO: Bug 5044562: P2PDMA pages require the PCIe BAR to be mapped UC
    if (!parent_gpu->static_bar1_size || parent_gpu->static_bar1_write_combined)
        return;

    // Try to reuse a cached PCI device registration first
    cached_pci_dev = uvm_devmem_pci_p2pdma_reuse_device(parent_gpu->pci_dev);

    // If we didn't find a cached registration or couldn't reuse it, register now
    if (!cached_pci_dev) {
        if (pci_p2pdma_add_resource(parent_gpu->pci_dev, uvm_device_p2p_static_bar(parent_gpu), 0, 0)) {
            UVM_ERR_PRINT("Unable to initialse PCI P2PDMA pages\n");
            return;
        }
        // Cache once and reuse till the driver is unloaded
        uvm_devmem_pci_p2pdma_cache_device(parent_gpu->pci_dev, pci_start_pfn);
    }

    // The current upstream PCIe P2PDMA architecture does not allow drivers to
    // specify a page_free callback. We plan to work with upstream maintainers
    // to resolve this but in the mean time we can work around the issue by
    // overwriting the existing dev_pagemap_ops struct with our own.
    // TODO: Bug 4672502: [Linux Upstream][UVM] Allow drivers to manage and
    // allocate PCI P2PDMA pages directly
    p2p_page = pfn_to_page(pci_start_pfn);
    page_pgmap(p2p_page)->ops = &uvm_device_p2p_pgmap_ops;
    for (; page_to_pfn(p2p_page) < pci_end_pfn; p2p_page++)
        p2p_page->zone_device_data = NULL;

    parent_gpu->device_p2p_initialised = true;
}

void uvm_devmem_device_p2p_deinit(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->device_p2p_initialised = false;
}
#else // CONFIG_PCI_P2PDMA

// Coherent platforms can do P2PDMA without CONFIG_PCI_P2PDMA
void uvm_devmem_device_p2p_init(uvm_parent_gpu_t *parent_gpu)
{
    if (uvm_parent_gpu_is_coherent(parent_gpu)) {
        if (parent_gpu->cdmm_enabled)
            return;

        // A coherent system uses normal struct pages.
        parent_gpu->device_p2p_initialised = true;
        return;
    }
}

void uvm_devmem_device_p2p_deinit(uvm_parent_gpu_t *parent_gpu)
{
    parent_gpu->device_p2p_initialised = false;
}
#endif // CONFIG_PCI_P2PDMA
