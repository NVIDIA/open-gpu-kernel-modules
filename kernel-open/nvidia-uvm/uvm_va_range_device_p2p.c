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

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_types.h"
#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_kvmalloc.h"
#include "nv_uvm_interface.h"
#include "nv_uvm_types.h"

static struct kmem_cache *g_uvm_va_range_device_p2p_cache __read_mostly;

NV_STATUS uvm_va_range_device_p2p_init(void)
{
    g_uvm_va_range_device_p2p_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_device_p2p_t", uvm_va_range_device_p2p_t);
    if (!g_uvm_va_range_device_p2p_cache)
        return NV_ERR_NO_MEMORY;
    return NV_OK;
}

void uvm_va_range_device_p2p_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_va_range_device_p2p_cache);
}

static NvU64 p2p_mem_page_count(uvm_device_p2p_mem_t *p2p_mem)
{
    return (p2p_mem->pfn_count * p2p_mem->page_size) >> PAGE_SHIFT;
}

static struct page *p2p_mem_get_page(uvm_device_p2p_mem_t *p2p_mem, NvU64 cpu_page_index)
{
    NvU64 gpu_pfn_nr = cpu_page_index / (p2p_mem->page_size >> PAGE_SHIFT);
    NvU64 cpu_pfn_offset = cpu_page_index % (p2p_mem->page_size >> PAGE_SHIFT);

    UVM_ASSERT(gpu_pfn_nr < p2p_mem->pfn_count);
    if (gpu_pfn_nr >= p2p_mem->pfn_count)
        return NULL;

    return pfn_to_page(p2p_mem->pfns[gpu_pfn_nr] + cpu_pfn_offset);
}

// When pages are first allocated to a range they start out with a refcount of
// one. Therefore when the last range is destroyed we need to drop the page
// refcounts. If nothing is pinning the page(s) they will get freed and release
// their reference to the p2p_mem object. If there are pinned pages we wait
// until they drop their pin before freeing the p2p_mem object.
void uvm_va_range_free_device_p2p_mem(uvm_device_p2p_mem_t *p2p_mem)
{
    NvU64 i;
    uvm_gpu_t *gpu = p2p_mem->gpu;

    // In the coherent case we don't hold references on the page because RM does
    // via the duplicated handle.
    if (!uvm_parent_gpu_is_coherent(gpu->parent)) {
        uvm_mutex_lock(&gpu->device_p2p_lock);

        // It's possible that another range has been setup for the handle since
        // this was queued for deletion. In that case don't free the pages.
        if (nv_kref_read(&p2p_mem->va_range_count)) {
            uvm_mutex_unlock(&gpu->device_p2p_lock);
            return;
        }

        // No more va ranges are setup to map the physical range and none can be
        // established because we hold device_p2p_lock.
        for (i = 0; i < p2p_mem_page_count(p2p_mem); i++)
            put_page(p2p_mem_get_page(p2p_mem, i));

        // Wait for any pinners
        wait_event(p2p_mem->waitq, nv_kref_read(&p2p_mem->refcount) == 0);

        uvm_mutex_unlock(&gpu->device_p2p_lock);
    }

    // Unpin the GPU memory. On non-coherent systems this frees the memory from
    // the point of view of UVM and the kernel because we waited for all kernel
    // users to complete above. On coherent systems there may still be in-kernel
    // users however in this case the kernel is responsible for freeing the
    // pages when they are eventually unpinned.
    uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(gpu),
                                                     p2p_mem->rm_memory_handle));
    uvm_kvfree(p2p_mem->pfns);
    uvm_kvfree(p2p_mem);
}

static void put_device_p2p_mem(struct nv_kref *ref)
{
    uvm_device_p2p_mem_t *p2p_mem = container_of(ref, uvm_device_p2p_mem_t, va_range_count);

    // We need to call into RM which we can't do whilst holding va_space
    // lock so defer it. We free the device_p2p_range there as that holds the
    // queue item.
    uvm_deferred_free_object_add(p2p_mem->deferred_free_list,
                                 &p2p_mem->deferred_free,
                                 UVM_DEFERRED_FREE_OBJECT_TYPE_DEVICE_P2P_MEM);
}

static void deinit_device_p2p_mem(uvm_device_p2p_mem_t *p2p_mem, struct list_head *deferred_free_list)
{
    // We need to hold device_p2p_lock to serialise against the nv_kref_read
    // in free_device_p2p_mem but we already hold the va_space lock so we
    // need to use the nested variants. This is safe because we are explicitly
    // scheduling work which may not happen holding the va_space lock. Coherent
    // systems don't need to take the lock because the p2p_mem objects are not
    // shared between multiple va_ranges.
    if (!uvm_parent_gpu_is_coherent(p2p_mem->gpu->parent))
        uvm_mutex_lock_nested(&p2p_mem->gpu->device_p2p_lock);

    p2p_mem->deferred_free_list = deferred_free_list;
    nv_kref_put(&p2p_mem->va_range_count, put_device_p2p_mem);

    if (!uvm_parent_gpu_is_coherent(p2p_mem->gpu->parent))
        uvm_mutex_unlock_nested(&p2p_mem->gpu->device_p2p_lock);
}

void uvm_va_range_deinit_device_p2p(uvm_va_range_device_p2p_t *device_p2p_range, struct list_head *deferred_free_list)
{
    deinit_device_p2p_mem(device_p2p_range->p2p_mem, deferred_free_list);
    device_p2p_range->p2p_mem = NULL;
}

void uvm_va_range_destroy_device_p2p(uvm_va_range_device_p2p_t *device_p2p_range, struct list_head *deferred_free_list)
{
    if (device_p2p_range->p2p_mem)
        uvm_va_range_deinit_device_p2p(device_p2p_range, deferred_free_list);
    kmem_cache_free(g_uvm_va_range_device_p2p_cache, device_p2p_range);
}

static NV_STATUS uvm_va_range_create_device_p2p(uvm_va_space_t *va_space,
                                                struct mm_struct *mm,
                                                NvU64 start,
                                                NvU64 length,
                                                NvU64 offset,
                                                uvm_device_p2p_mem_t *p2p_mem,
                                                uvm_va_range_device_p2p_t **out_va_range)
{
    uvm_va_range_device_p2p_t *device_p2p_range = NULL;
    NV_STATUS status;

    device_p2p_range = nv_kmem_cache_zalloc(g_uvm_va_range_device_p2p_cache, NV_UVM_GFP_FLAGS);
    if (!device_p2p_range)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_initialize_reclaim(&device_p2p_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_DEVICE_P2P,
                                             va_space,
                                             start,
                                             start + length - 1);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_device_p2p_cache, device_p2p_range);
        return status;
    }

    status = uvm_range_tree_add(&va_space->va_range_tree, &device_p2p_range->va_range.node);
    if (status != NV_OK) {
        uvm_va_range_destroy(&device_p2p_range->va_range, NULL);
        return status;
    }

    device_p2p_range->p2p_mem = p2p_mem;
    device_p2p_range->gpu = p2p_mem->gpu;
    device_p2p_range->offset = offset;
    *out_va_range = device_p2p_range;

    return NV_OK;
}

static NV_STATUS get_gpu_pfns(uvm_gpu_t *gpu,
                              NvHandle duped_memory,
                              NvU64 *pfns,
                              NvU64 pfn_count,
                              NvLength size)
{
    UvmGpuExternalPhysAddrInfo ext_mapping_info;
    NV_STATUS status;
    NvU64 i;

    memset(&ext_mapping_info, 0, sizeof(ext_mapping_info));
    ext_mapping_info.physAddrBuffer = pfns;
    ext_mapping_info.physAddrBufferSize = pfn_count * sizeof(*pfns);

    status = uvm_rm_locked_call(nvUvmInterfaceGetExternalAllocPhysAddrs(gpu->rm_address_space,
                                                                        duped_memory,
                                                                        0,
                                                                        size,
                                                                        &ext_mapping_info));
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to read external address");
        return status;
    }

    if (ext_mapping_info.numWrittenPhysAddrs != pfn_count) {
        UVM_ERR_PRINT("Could not find GPU PFNs for device P2P memory\n");
        return NV_ERR_NO_MEMORY;
    }

    // get_gpu_pfns returns GPU physical addresses rather than absolute
    // pfn's so we need to convert these to absolute pfns by adding the BAR1
    // start address or system memory start address and right shifting by
    // PAGE_SHIFT.
    for (i = 0; i < ext_mapping_info.numWrittenPhysAddrs; i++)
        if (uvm_parent_gpu_is_coherent(gpu->parent)) {
            NvU64 last_pfn = gpu->parent->system_bus.memory_window_end >> PAGE_SHIFT;

            pfns[i] = (gpu->parent->system_bus.memory_window_start + pfns[i]) >> PAGE_SHIFT;
            UVM_ASSERT(pfns[i] <= last_pfn);
            if (pfns[i] > last_pfn)
                return NV_ERR_INVALID_ADDRESS;
        }
        else {
            NvU64 last_pfn = ((gpu->mem_info.static_bar1_start + gpu->mem_info.static_bar1_size) >> PAGE_SHIFT) - 1;

            pfns[i] = (gpu->mem_info.static_bar1_start + pfns[i]) >> PAGE_SHIFT;
            UVM_ASSERT(pfns[i] <= last_pfn);
            if (pfns[i] > last_pfn)
                return NV_ERR_INVALID_ADDRESS;
        }

    return NV_OK;
}

#if defined(CONFIG_PCI_P2PDMA) && defined(NV_STRUCT_PAGE_HAS_ZONE_DEVICE_DATA)
static bool pci_p2pdma_page_free(struct page *page) {
    return is_pci_p2pdma_page(page) && !page->zone_device_data && page_ref_count(page) == 1;
}

// page->zone_device_data does not exist in kernels versions older than v5.3
// which don't support CONFIG_PCI_P2PDMA. Therefore we need these accessor
// functions to ensure compilation succeeeds on older kernels.
static void pci_p2pdma_page_set_zone_device_data(struct page *page, void *zone_device_data)
{
    page->zone_device_data = zone_device_data;
}

static void *pci_p2pdma_page_get_zone_device_data(struct page *page)
{
    return page->zone_device_data;
}
#else
static bool pci_p2pdma_page_free(struct page *page) {
    UVM_ASSERT(0);
    return false;
}

static void pci_p2pdma_page_set_zone_device_data(struct page *page, void *zone_device_data)
{
    UVM_ASSERT(0);
}

static void *pci_p2pdma_page_get_zone_device_data(struct page *page)
{
    UVM_ASSERT(0);
    return NULL;
}
#endif

static NV_STATUS alloc_device_p2p_mem(uvm_gpu_t *gpu,
                                      NvHandle client,
                                      NvHandle memory,
                                      uvm_device_p2p_mem_t **p2p_mem_out)
{
    uvm_device_p2p_mem_t *p2p_mem;
    UvmGpuMemoryInfo mem_info;
    NvHandle dupped_memory;
    NV_STATUS status;
    NvU64 i;
    NvU64 *pfns = NULL;

    p2p_mem = uvm_kvmalloc_zero(sizeof(*p2p_mem));
    if (!p2p_mem)
        return NV_ERR_NO_MEMORY;

    status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(uvm_gpu_device_handle(gpu),
                                                        client,
                                                        memory,
                                                        &dupped_memory,
                                                        &mem_info));
    if (status != NV_OK)
        goto out_free;

    // GPU allocations typically use 2MB pages which is larger than any
    // supported CPU page size. However it's possible to create GPU allocations
    // which use 4K page sizes. We don't support that for P2P mappings so bail.
    if (mem_info.pageSize < PAGE_SIZE) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto out_release;
    }

    p2p_mem->gpu = gpu;
    p2p_mem->rm_memory_handle = dupped_memory;
    p2p_mem->pfn_count = mem_info.size/mem_info.pageSize;
    p2p_mem->page_size = mem_info.pageSize;
    init_waitqueue_head(&p2p_mem->waitq);
    nv_kref_init(&p2p_mem->refcount);
    nv_kref_init(&p2p_mem->va_range_count);

    pfns = uvm_kvmalloc_zero(p2p_mem->pfn_count * sizeof(*pfns));
    if (!pfns) {
        status = NV_ERR_NO_MEMORY;
        goto out_release;
    }

    status = get_gpu_pfns(gpu, dupped_memory, pfns, p2p_mem->pfn_count, mem_info.size);
    if (status != NV_OK)
        goto out_release;

    p2p_mem->pfns = pfns;

    for (i = 0; i < p2p_mem_page_count(p2p_mem); i++) {
        struct page *page = p2p_mem_get_page(p2p_mem, i);

        if (!pci_p2pdma_page_free(page)) {
            UVM_ASSERT(0);

            // This will leak the RM handle because we don't release it.
            // However we should never encounter this and it's unclear that
            // we can safely recover so the best thing is to leave the
            // memory pinned in case something is using it.
            return NV_ERR_INVALID_ARGUMENT;
        }

        pci_p2pdma_page_set_zone_device_data(page, p2p_mem);
        nv_kref_get(&p2p_mem->refcount);
    }

    // The pages hold the references now, so drop the one allocated at
    // initialisation.
    nv_kref_put(&p2p_mem->refcount, NULL);

    *p2p_mem_out = p2p_mem;

    return status;

out_release:
    uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(gpu), dupped_memory));
    uvm_kvfree(pfns);

out_free:
    uvm_kvfree(p2p_mem);

    return status;
}

static NV_STATUS alloc_pci_device_p2p(uvm_gpu_t *gpu,
                                      NvHandle client,
                                      NvHandle memory,
                                      uvm_device_p2p_mem_t **p2p_mem_out)
{
    NvU64 pfn;
    NV_STATUS status;
    NvHandle dupped_memory;
    UvmGpuMemoryInfo mem_info;
    uvm_device_p2p_mem_t *p2p_mem;

    status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(uvm_gpu_device_handle(gpu),
                                                        client,
                                                        memory,
                                                        &dupped_memory,
                                                        &mem_info));
    if (status != NV_OK)
        return status;

    // The client RM handle is associated with a range of pages, which if
    // initialised previously will all have their zone_device_data set to point
    // to the same p2p_mem struct. Therefore we only need to look up one page to
    // find the common p2p_mem struct to determine if the pages in the range are
    // free or not.
    status = get_gpu_pfns(gpu, dupped_memory, &pfn, 1, mem_info.pageSize);
    if (status != NV_OK)
        goto out_release;

    // Holding the device_p2p_lock ensures if we find a p2p_mem struct in
    // zone_device_data it will remain alive until we get a reference on it. It
    // also ensures if we don't find a p2p_mem object that we don't race with
    // some other thread assigning or clearing zone_device_data.
    uvm_mutex_lock(&gpu->device_p2p_lock);
    p2p_mem = pci_p2pdma_page_get_zone_device_data(pfn_to_page(pfn));
    if (!p2p_mem) {
        // We have not previously allocated p2pdma pages for this RM handle so do
        // so now.
        status = alloc_device_p2p_mem(gpu, client, memory, &p2p_mem);
        if (status != NV_OK) {
            uvm_mutex_unlock(&gpu->device_p2p_lock);
            goto out_release;
        }
    }
    else {
        // Get reference on the existing range.
        nv_kref_get(&p2p_mem->va_range_count);
    }
    uvm_mutex_unlock(&gpu->device_p2p_lock);

    // The p2p_mem will have have a pin on the memory.
    uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(gpu), dupped_memory));
    *p2p_mem_out = p2p_mem;

    return NV_OK;

out_release:
    uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(gpu), dupped_memory));

    return status;
}

static NV_STATUS alloc_coherent_device_p2p(uvm_gpu_t *gpu,
                                           NvHandle client,
                                           NvHandle memory,
                                           uvm_device_p2p_mem_t **p2p_mem_out)
{
    uvm_device_p2p_mem_t *p2p_mem;
    UvmGpuMemoryInfo mem_info;
    NvHandle dupped_memory;
    NV_STATUS status;
    NvU64 *pfns = NULL;

    p2p_mem = uvm_kvmalloc_zero(sizeof(*p2p_mem));
    if (!p2p_mem)
        return NV_ERR_NO_MEMORY;

    status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(uvm_gpu_device_handle(gpu),
                                                        client,
                                                        memory,
                                                        &dupped_memory,
                                                        &mem_info));
    if (status != NV_OK)
        goto out;

    // GPU allocations typically use 2MB pages which is larger than any
    // supported CPU page size. However it's possible to create GPU allocations
    // which use 4K page sizes. We don't support that for P2P mappings so bail.
    if (mem_info.pageSize < PAGE_SIZE) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto out_release;
    }

    p2p_mem->gpu = gpu;
    p2p_mem->rm_memory_handle = dupped_memory;
    p2p_mem->pfn_count = mem_info.size / mem_info.pageSize;
    p2p_mem->page_size = mem_info.pageSize;
    init_waitqueue_head(&p2p_mem->waitq);
    nv_kref_init(&p2p_mem->refcount);
    nv_kref_init(&p2p_mem->va_range_count);

    pfns = uvm_kvmalloc_zero(p2p_mem->pfn_count * sizeof(*pfns));
    if (!pfns) {
        status = NV_ERR_NO_MEMORY;
        goto out_release;
    }

    status = get_gpu_pfns(gpu, dupped_memory, pfns, p2p_mem->pfn_count, mem_info.size);
    if (status != NV_OK)
        goto out_release;

    // We don't need to get references on the underlying pages because RM should
    // hold a reference until we release the duplicated handle which doesn't
    // happen until the va_range is deinitialized.
    p2p_mem->pfns = pfns;
    *p2p_mem_out = p2p_mem;

    return NV_OK;

out_release:
    uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(gpu), dupped_memory));
    uvm_kvfree(pfns);

out:
    uvm_kvfree(p2p_mem);

    return status;
}

// Establish a mapping to p2p pages
NV_STATUS uvm_api_alloc_device_p2p(UVM_ALLOC_DEVICE_P2P_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_device_p2p_mem_t *p2p_mem;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_device_p2p_t *device_p2p_range;
    struct mm_struct *mm;
    uvm_gpu_t *gpu;
    LIST_HEAD(deferred_free_list);

    if (uvm_api_range_invalid(params->base + params->offset, params->length))
        return NV_ERR_INVALID_ARGUMENT;

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpuUuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    if (!gpu->device_p2p_initialised) {
        status = NV_ERR_NOT_SUPPORTED;
        goto out_release;
    }

    if (uvm_parent_gpu_is_coherent(gpu->parent)) {
        status = alloc_coherent_device_p2p(gpu, params->hClient, params->hMemory, &p2p_mem);
        if (status != NV_OK)
            goto out_release;
    }
    else {
        status = alloc_pci_device_p2p(gpu, params->hClient, params->hMemory, &p2p_mem);
        if (status != NV_OK)
            goto out_release;
    }

    if (params->offset + params->length > p2p_mem->pfn_count * p2p_mem->page_size) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto out_free;
    }

    // The mm needs to be locked in order to remove stale HMM va_blocks.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);
    status = uvm_va_range_create_device_p2p(va_space,
                                            mm,
                                            params->base,
                                            params->length,
                                            params->offset,
                                            p2p_mem,
                                            &device_p2p_range);
    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);

    if (status != NV_OK)
        goto out_free;

    uvm_gpu_release(gpu);
    return NV_OK;

out_free:
    deinit_device_p2p_mem(p2p_mem, &deferred_free_list);
    uvm_deferred_free_object_list(&deferred_free_list);

out_release:
    uvm_gpu_release(gpu);

    return status;
}

NV_STATUS uvm_va_range_device_p2p_map_cpu(uvm_va_space_t *va_space,
                                          struct vm_area_struct *vma,
                                          uvm_va_range_device_p2p_t *device_p2p_range)
{
    NvU64 i, va;
    NV_STATUS status = NV_OK;
    uvm_device_p2p_mem_t *p2p_mem = device_p2p_range->p2p_mem;

    uvm_assert_mmap_lock_locked(vma->vm_mm);

    for (i = device_p2p_range->offset >> PAGE_SHIFT, va = vma->vm_start; va < vma->vm_end; i++, va += PAGE_SIZE) {
        struct page *page = p2p_mem_get_page(p2p_mem, i);
        int ret;

        if (!page) {
            status = NV_ERR_FATAL_ERROR;
            goto error;
        }

        // Takes a reference on the page. We know the page can't be freed
        // (ie. have a refcount of 0) because the va_range exists and it has
        // a reference on the uvm_device_p2p_mem_t and hence the underlying
        // pages.
        ret = vm_insert_page(vma, va, page);
        if (ret) {
            status = errno_to_nv_status(ret);
            goto error;
        }
    }

    return NV_OK;

error:
    unmap_mapping_range(va_space->mapping, vma->vm_start, vma->vm_end - vma->vm_start, 1);

    return status;
}
