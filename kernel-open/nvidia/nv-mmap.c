/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"
#include "nv_speculation_barrier.h"

/*
 * The 'struct vm_operations' open() callback is called by the Linux
 * kernel when the parent VMA is split or copied, close() when the
 * current VMA is about to be deleted.
 *
 * We implement these callbacks to keep track of the number of user
 * mappings of system memory allocations. This was motivated by a
 * subtle interaction problem between the driver and the kernel with
 * respect to the bookkeeping of pages marked reserved and later
 * mapped with mmap().
 *
 * Traditionally, the Linux kernel ignored reserved pages, such that
 * when they were mapped via mmap(), the integrity of their usage
 * counts depended on the reserved bit being set for as long as user
 * mappings existed.
 *
 * Since we mark system memory pages allocated for DMA reserved and
 * typically map them with mmap(), we need to ensure they remain
 * reserved until the last mapping has been torn down. This worked
 * correctly in most cases, but in a few, the RM API called into the
 * RM to free memory before calling munmap() to unmap it.
 *
 * In the past, we allowed nv_free_pages() to remove the 'at' from
 * the parent device's allocation list in this case, but didn't
 * release the underlying pages until the last user mapping had been
 * destroyed:
 *
 * In nvidia_vma_release(), we freed any resources associated with
 * the allocation (IOMMU mappings, etc.) and cleared the
 * underlying pages' reserved bits, but didn't free them. The kernel
 * was expected to do this.
 *
 * This worked in practise, but made dangerous assumptions about the
 * kernel's behavior and could fail in some cases. We now handle
 * this case differently (see below).
 */
static void
nvidia_vma_open(struct vm_area_struct *vma)
{
    nv_alloc_t *at = NV_VMA_PRIVATE(vma);

    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    if (at != NULL)
    {
        NV_ATOMIC_INC(at->usage_count);

        NV_PRINT_AT(NV_DBG_MEMINFO, at);
    }
}

/*
 * (see above for additional information)
 *
 * If the 'at' usage count drops to zero with the updated logic, the
 * the allocation is recorded in the free list of the private
 * data associated with the file pointer; nvidia_close() uses this
 * list to perform deferred free operations when the parent file
 * descriptor is closed. This will typically happen when the process
 * exits.
 *
 * Since this is technically a workaround to handle possible fallout
 * from misbehaving clients, we additionally print a warning.
 */
static void
nvidia_vma_release(struct vm_area_struct *vma)
{
    nv_alloc_t *at = NV_VMA_PRIVATE(vma);
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(NV_VMA_FILE(vma));
    static int count = 0;

    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    if (at != NULL && nv_alloc_release(nvlfp, at))
    {
        if ((at->pid == os_get_current_process()) &&
            (count++ < NV_MAX_RECURRING_WARNING_MESSAGES))
        {
            nv_printf(NV_DBG_MEMINFO,
                "NVRM: VM: %s: late unmap, comm: %s, 0x%p\n",
                __FUNCTION__, current->comm, at);
        }
    }
}

static int
nvidia_vma_access(
    struct vm_area_struct *vma,
    unsigned long addr,
    void *buffer,
    int length,
    int write
)
{
    nv_alloc_t *at = NULL;
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(NV_VMA_FILE(vma));
    nv_state_t *nv = NV_STATE_PTR(nvlfp->nvptr);
    NvU32 pageIndex, pageOffset;
    void *kernel_mapping;
    const nv_alloc_mapping_context_t *mmap_context = &nvlfp->mmap_context;
    NvU64 offsInVma = addr - vma->vm_start;

    pageIndex = (offsInVma >> PAGE_SHIFT);
    pageOffset = (offsInVma & ~PAGE_MASK);

    if (length < 0)
    {
        return -EINVAL;
    }

    if (!mmap_context->valid)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: VM: invalid mmap context\n");
        return -EINVAL;
    }

    if (write && !(mmap_context->prot & NV_PROTECT_WRITEABLE))
    {
        return -EACCES;
    }

    if (nv->flags & NV_FLAG_CONTROL)
    {
        at = NV_VMA_PRIVATE(vma);

        /*
         * at can be NULL for peer IO mem.
         */
        if (!at)
            return -EINVAL;

        if (pageIndex >= at->num_pages)
            return -EINVAL;

        pageIndex = nv_array_index_no_speculate(pageIndex, at->num_pages);
        kernel_mapping = (void *)(at->page_table[pageIndex]->virt_addr + pageOffset);
    }
    else
    {
        NvU64 idx = 0;
        NvU64 curOffs = 0;
        for(; idx < mmap_context->memArea.numRanges; idx++)
        {
            NvU64 nextOffs = mmap_context->memArea.pRanges[idx].size + curOffs;
            if (curOffs <= offsInVma && nextOffs > offsInVma)
            {
                NvU64 realAddr = offsInVma - curOffs + mmap_context->memArea.pRanges[idx].start;
                addr = realAddr & PAGE_MASK;
                goto found;
            }
            curOffs = nextOffs;
        }
        return -EINVAL;
found:
        kernel_mapping = os_map_kernel_space(addr, PAGE_SIZE, NV_MEMORY_UNCACHED);
        if (kernel_mapping == NULL)
            return -ENOMEM;

        kernel_mapping = ((char *)kernel_mapping + pageOffset);
    }

    length = NV_MIN(length, (int)(PAGE_SIZE - pageOffset));

    if (write)
        memcpy(kernel_mapping, buffer, length);
    else
        memcpy(buffer, kernel_mapping, length);

    if (at == NULL)
    {
        kernel_mapping = ((char *)kernel_mapping - pageOffset);
        os_unmap_kernel_space(kernel_mapping, PAGE_SIZE);
    }

    return length;
}

static vm_fault_t nvidia_fault(
#if !defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    struct vm_area_struct *vma,
#endif
    struct vm_fault *vmf
)
{
#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    struct vm_area_struct *vma = vmf->vma;
#endif
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(NV_VMA_FILE(vma));
    nv_linux_state_t *nvl = nvlfp->nvptr;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    vm_fault_t ret = VM_FAULT_NOPAGE;

    if (vma->vm_pgoff != 0)
    {
        return VM_FAULT_SIGBUS;
    }

    // Mapping revocation is only supported for GPU mappings.
    if (NV_IS_CTL_DEVICE(nv))
    {
        return VM_FAULT_SIGBUS;
    }


    // Wake up GPU and reinstate mappings only if we are not in S3/S4 entry
    if (!down_read_trylock(&nv_system_pm_lock))
    {
        return VM_FAULT_NOPAGE;
    }

    down(&nvl->mmap_lock);

    // Wake up the GPU if it is not currently safe to mmap.
    if (!nvl->safe_to_mmap)
    {
        NV_STATUS status;

        if (!nvl->gpu_wakeup_callback_needed)
        {
            // GPU wakeup callback already scheduled.
            up(&nvl->mmap_lock);
            up_read(&nv_system_pm_lock);
            return VM_FAULT_NOPAGE;
        }

        /*
         * GPU wakeup cannot be completed directly in the fault handler due to the
         * inability to take the GPU lock while mmap_lock is held.
         */
        status = rm_schedule_gpu_wakeup(nvl->sp[NV_DEV_STACK_GPU_WAKEUP], nv);
        if (status != NV_OK)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: VM: rm_schedule_gpu_wakeup failed: %x\n", status);
            up(&nvl->mmap_lock);
            up_read(&nv_system_pm_lock);
            return VM_FAULT_SIGBUS;
        }
        // Ensure that we do not schedule duplicate GPU wakeup callbacks.
        nvl->gpu_wakeup_callback_needed = NV_FALSE;

        up(&nvl->mmap_lock);
        up_read(&nv_system_pm_lock);
        return VM_FAULT_NOPAGE;
    }
    {
        NvU64 idx;
        NvU64 curOffs = 0;
        NvBool bRevoked = NV_TRUE;
        nv_alloc_mapping_context_t *mmap_context = &nvlfp->mmap_context;
        for(idx = 0; idx < mmap_context->memArea.numRanges; idx++)
        {
            NvU64 nextOffs = curOffs + mmap_context->memArea.pRanges[idx].size;
            NvU64 pfn = mmap_context->memArea.pRanges[idx].start >> PAGE_SHIFT;
            NvU64 numPages = mmap_context->memArea.pRanges[idx].size >> PAGE_SHIFT;
            while (numPages != 0)
            {
                ret = nv_insert_pfn(vma, curOffs + vma->vm_start, pfn);
                if (ret != VM_FAULT_NOPAGE)
                {
                    goto err;
                }
                bRevoked = NV_FALSE;
                curOffs += PAGE_SIZE;
                pfn++;
                numPages--;
            }
            curOffs = nextOffs;
        }
err:
        nvl->all_mappings_revoked &= bRevoked;
    }

    up(&nvl->mmap_lock);
    up_read(&nv_system_pm_lock);

    return ret;
}

static struct vm_operations_struct nv_vm_ops = {
    .open   = nvidia_vma_open,
    .close  = nvidia_vma_release,
    .fault  = nvidia_fault,
    .access = nvidia_vma_access,
};

int nv_encode_caching(
    pgprot_t *prot,
    NvU32     cache_type,
    nv_memory_type_t memory_type
)
{
    pgprot_t tmp;

    if (prot == NULL)
    {
        tmp = __pgprot(0);
        prot = &tmp;
    }

    switch (cache_type)
    {
        case NV_MEMORY_UNCACHED_WEAK:
#if defined(NV_PGPROT_UNCACHED_WEAK)
            *prot = NV_PGPROT_UNCACHED_WEAK(*prot);
            break;
#endif
        case NV_MEMORY_UNCACHED:
            *prot = (memory_type == NV_MEMORY_TYPE_SYSTEM) ?
                    NV_PGPROT_UNCACHED(*prot) :
                    NV_PGPROT_UNCACHED_DEVICE(*prot);
            break;
#if defined(NV_PGPROT_WRITE_COMBINED) && \
    defined(NV_PGPROT_WRITE_COMBINED_DEVICE)
        case NV_MEMORY_DEFAULT:
        case NV_MEMORY_WRITECOMBINED:
            if (NV_ALLOW_WRITE_COMBINING(memory_type))
            {
                *prot = (memory_type == NV_MEMORY_TYPE_FRAMEBUFFER) ?
                        NV_PGPROT_WRITE_COMBINED_DEVICE(*prot) :
                        NV_PGPROT_WRITE_COMBINED(*prot);
                break;
            }

            /*
             * If WC support is unavailable, we need to return an error
             * code to the caller, but need not print a warning.
             *
             * For frame buffer memory, callers are expected to use the
             * UC- memory type if we report WC as unsupported, which
             * translates to the effective memory type WC if a WC MTRR
             * exists or else UC.
             */
            return 1;
#endif
        case NV_MEMORY_CACHED:
            if (!NV_ALLOW_CACHING(memory_type))
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: VM: memory type %d does not allow caching!\n",
                    memory_type);
                return 1;
            }
            break;

        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: VM: cache type %d not supported for memory type %d!\n",
                cache_type, memory_type);
            return 1;
    }
    return 0;
}

static int nvidia_mmap_peer_io(
    struct vm_area_struct *vma,
    nv_alloc_t *at,
    NvU64 page_index,
    NvU64 pages
)
{
    int ret;
    NvU64 start;
    NvU64 size;

    BUG_ON(!at->flags.contig);

    start = at->page_table[page_index]->phys_addr;
    size = pages * PAGE_SIZE;

    ret = nv_io_remap_page_range(vma, start, size, vma->vm_start);

    return ret;
}

static int nvidia_mmap_sysmem(
    struct vm_area_struct *vma,
    nv_alloc_t *at,
    NvU64 page_index,
    NvU64 pages
)
{
    NvU64 j;
    int ret = 0;
    unsigned long start = 0;

    NV_ATOMIC_INC(at->usage_count);

    start = vma->vm_start;
    for (j = page_index; j < (page_index + pages); j++)
    {
        j = nv_array_index_no_speculate(j, (page_index + pages));

        if (
#if defined(NV_VGPU_KVM_BUILD)
            at->flags.guest ||
#endif
            at->flags.carveout)
        {
            ret = nv_remap_page_range(vma, start, at->page_table[j]->phys_addr,
                                      PAGE_SIZE, vma->vm_page_prot);
        }
        else
        {
            if (at->flags.unencrypted)
                vma->vm_page_prot = nv_adjust_pgprot(vma->vm_page_prot);

            ret = vm_insert_page(vma, start,
                                 NV_GET_PAGE_STRUCT(at->page_table[j]->phys_addr));
        }

        if (ret)
        {
            NV_ATOMIC_DEC(at->usage_count);
            return -EAGAIN;
        }
        start += PAGE_SIZE;
    }

    return ret;
}

static int nvidia_mmap_numa(
    struct vm_area_struct *vma,
    const nv_alloc_mapping_context_t *mmap_context)
{
    NvU64 start, addr;
    NvU64 pages;
    NvU64 i;

    pages = NV_VMA_SIZE(vma) >> PAGE_SHIFT;
    start = vma->vm_start;

    if (mmap_context->num_pages < pages)
    {
        return -EINVAL;
    }

    // Needed for the linux kernel for mapping compound pages
    nv_vm_flags_set(vma, VM_MIXEDMAP);

    for (i = 0, addr = mmap_context->page_array[0]; i < pages;
         addr = mmap_context->page_array[++i], start += PAGE_SIZE)
    {
        if (vm_insert_page(vma, start, NV_GET_PAGE_STRUCT(addr)) != 0)
        {
            return -EAGAIN;
        }
    }

    return 0;
}

int nvidia_mmap_helper(
    nv_state_t *nv,
    nv_linux_file_private_t *nvlfp,
    nvidia_stack_t *sp,
    struct vm_area_struct *vma,
    void *vm_priv
)
{
    NvU32 prot = 0;
    int ret;
    const nv_alloc_mapping_context_t *mmap_context = &nvlfp->mmap_context;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NV_STATUS status;

    if (nvlfp == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    /*
     * If mmap context is not valid on this file descriptor, this mapping wasn't
     * previously validated with the RM so it must be rejected.
     */
    if (!mmap_context->valid)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: VM: invalid mmap\n");
        return -EINVAL;
    }

    if (vma->vm_pgoff != 0)
    {
        return -EINVAL;
    }

    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    status = nv_check_gpu_state(nv);
    if (status != NV_OK)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv,
            "GPU is lost, skipping nvidia_mmap_helper\n");
        return status;
    }

    NV_VMA_PRIVATE(vma) = vm_priv;

    prot = mmap_context->prot;

    /*
     * Nvidia device node(nvidia#) maps device's BAR memory,
     * Nvidia control node(nvidiactrl) maps system memory.
     */
    if (!NV_IS_CTL_DEVICE(nv))
    {
        NvU64 access_start = mmap_context->access_start;
        NvU64 access_len = mmap_context->access_size;

        // Ensure size is correct.
        if (NV_VMA_SIZE(vma) != memareaSize(mmap_context->memArea))
        {
            return -ENXIO;
        }

        if (IS_REG_OFFSET(nv, access_start, access_len))
        {
            if (nv_encode_caching(&vma->vm_page_prot, NV_MEMORY_UNCACHED,
                        NV_MEMORY_TYPE_REGISTERS))
            {
                return -ENXIO;
            }
        }
        else if (IS_FB_OFFSET(nv, access_start, access_len))
        {
            if (IS_UD_OFFSET(nv, access_start, access_len))
            {
                if (nv_encode_caching(&vma->vm_page_prot, NV_MEMORY_UNCACHED,
                            NV_MEMORY_TYPE_FRAMEBUFFER))
                {
                    return -ENXIO;
                }
            }
            else
            {
                if (nv_encode_caching(&vma->vm_page_prot,
                        rm_disable_iomap_wc() ? NV_MEMORY_UNCACHED : mmap_context->caching, 
                        NV_MEMORY_TYPE_FRAMEBUFFER))
                {
                    if (nv_encode_caching(&vma->vm_page_prot,
                            NV_MEMORY_UNCACHED_WEAK, NV_MEMORY_TYPE_FRAMEBUFFER))
                    {
                        return -ENXIO;
                    }
                }
            }
        }

        down(&nvl->mmap_lock);
        if (nvl->safe_to_mmap)
        {
            nvl->all_mappings_revoked = NV_FALSE;

            //
            // This path is similar to the sysmem mapping code.
            // TODO: Refactor is needed as part of bug#2001704.
            //
            if ((nv_get_numa_status(nvl) == NV_NUMA_STATUS_ONLINE) &&
                !IS_REG_OFFSET(nv, access_start, access_len) &&
                (mmap_context->num_pages != 0))
            {
                ret = nvidia_mmap_numa(vma, mmap_context);
                if (ret)
                {
                    up(&nvl->mmap_lock);
                    return ret;
                }
            }
            else
            {
                NvU64 idx = 0;
                NvU64 curOffs = 0;
                for(; idx < mmap_context->memArea.numRanges; idx++)
                {
                    NvU64 nextOffs = curOffs + mmap_context->memArea.pRanges[idx].size;
                    if (nv_io_remap_page_range(vma,
                            mmap_context->memArea.pRanges[idx].start,
                            mmap_context->memArea.pRanges[idx].size,
                            vma->vm_start + curOffs) != 0)
                    {
                        up(&nvl->mmap_lock);
                        return -EAGAIN;
                    }
                    curOffs = nextOffs;
                }
            }
        }
        up(&nvl->mmap_lock);

        nv_vm_flags_set(vma, VM_IO | VM_PFNMAP | VM_DONTEXPAND);
    }
    else
    {
        nv_alloc_t *at;
        NvU64 page_index;
        NvU64 pages;
        NvU64 mmap_size;

        at = (nv_alloc_t *)mmap_context->alloc;
        page_index = mmap_context->page_index;
        mmap_size = NV_VMA_SIZE(vma);
        pages = mmap_size >> PAGE_SHIFT;

        if ((page_index + pages) > at->num_pages)
        {
            return -ERANGE;
        }

        /*
         * Callers that pass in non-NULL VMA private data must never reach this
         * code. They should be mapping on a non-control node.
         */
        BUG_ON(NV_VMA_PRIVATE(vma));

        if (at->flags.peer_io)
        {
            if (nv_encode_caching(&vma->vm_page_prot,
                                  at->cache_type,
                                  NV_MEMORY_TYPE_DEVICE_MMIO))
            {
                return -ENXIO;
            }

            /*
             * There is no need to keep 'peer IO at' alive till vma_release like
             * 'sysmem at' because there are no security concerns where a client
             * could free RM allocated sysmem before unmapping it. Hence, vm_ops
             * are NOP, and at->usage_count is never being used.
             */
            NV_VMA_PRIVATE(vma) = NULL;

            ret = nvidia_mmap_peer_io(vma, at, page_index, pages);

            BUG_ON(NV_VMA_PRIVATE(vma));

            if (ret)
            {
                return ret;
            }

            NV_PRINT_AT(NV_DBG_MEMINFO, at);

            nv_vm_flags_set(vma, VM_IO);
            nv_vm_flags_set(vma, VM_DONTEXPAND | VM_DONTDUMP);
        }
        else
        {
            if (nv_encode_caching(&vma->vm_page_prot,
                                  at->cache_type,
                                  NV_MEMORY_TYPE_SYSTEM))
            {
                return -ENXIO;
            }

            NV_VMA_PRIVATE(vma) = at;

            ret = nvidia_mmap_sysmem(vma, at, page_index, pages);

            if (ret)
            {
                return ret;
            }

            NV_PRINT_AT(NV_DBG_MEMINFO, at);

            //
            // VM_MIXEDMAP will be set by vm_insert_page() in nvidia_mmap_sysmem().
            // VM_SHARED is added to avoid any undesired copy-on-write effects.
            //
            nv_vm_flags_set(vma, VM_SHARED);
            nv_vm_flags_set(vma, VM_DONTEXPAND | VM_DONTDUMP);
        }
    }

    if ((prot & NV_PROTECT_WRITEABLE) == 0)
    {
        vma->vm_page_prot = NV_PGPROT_READ_ONLY(vma->vm_page_prot);
        nv_vm_flags_clear(vma, VM_WRITE);
        nv_vm_flags_clear(vma, VM_MAYWRITE);
    }

    vma->vm_ops = &nv_vm_ops;

    return 0;
}

int nvidia_mmap(
    struct file *file,
    struct vm_area_struct *vma
)
{
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);
    nv_linux_state_t *nvl;
    nv_state_t *nv;
    nvidia_stack_t *sp = NULL;
    int status;

    //
    // Do not allow mmap operation if this is a fd into
    // which rm objects have been exported.
    //
    if (nvlfp->nvfp.handles != NULL)
    {
        return -EINVAL;
    }

    if (!nv_is_control_device(NV_FILE_INODE(file)))
    {
        status = nv_wait_open_complete_interruptible(nvlfp);
        if (status != 0)
            return status;
    }

    nvl = nvlfp->nvptr;
    if (nvl == NULL)
        return -EIO;

    nv = NV_STATE_PTR(nvl);

    status = nv_kmem_cache_alloc_stack(&sp);
    if (status != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Unable to allocate altstack for mmap\n");
        return status;
    }

    status = nvidia_mmap_helper(nv, nvlfp, sp, vma, NULL);

    nv_kmem_cache_free_stack(sp);

    return status;
}

void
nv_revoke_gpu_mappings_locked(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_linux_file_private_t *nvlfp;

    /* Revoke all mappings for every open file */
    list_for_each_entry (nvlfp, &nvl->open_files, entry)
    {
        unmap_mapping_range(&nvlfp->mapping, 0, ~0, 1);
    }

    nvl->all_mappings_revoked = NV_TRUE;
}

NV_STATUS NV_API_CALL nv_revoke_gpu_mappings(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    // Mapping revocation is only supported for GPU mappings.
    if (NV_IS_CTL_DEVICE(nv))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    down(&nvl->mmap_lock);

    nv_revoke_gpu_mappings_locked(nv);

    up(&nvl->mmap_lock);

    return NV_OK;
}

void NV_API_CALL nv_acquire_mmap_lock(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    down(&nvl->mmap_lock);
}

void NV_API_CALL nv_release_mmap_lock(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    up(&nvl->mmap_lock);
}

NvBool NV_API_CALL nv_get_all_mappings_revoked_locked(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    // Caller must hold nvl->mmap_lock for all decisions based on this
    return nvl->all_mappings_revoked;
}

void NV_API_CALL nv_set_safe_to_mmap_locked(
    nv_state_t *nv,
    NvBool safe_to_mmap
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    // Caller must hold nvl->mmap_lock

    /*
     * If nvl->safe_to_mmap is transitioning from TRUE to FALSE, we expect to
     * need to schedule a GPU wakeup callback when we fault.
     *
     * nvl->gpu_wakeup_callback_needed will be set to FALSE in nvidia_fault()
     * after scheduling the GPU wakeup callback, preventing us from scheduling
     * duplicates.
     */
    if (!safe_to_mmap && nvl->safe_to_mmap)
    {
        nvl->gpu_wakeup_callback_needed = NV_TRUE;
    }

    nvl->safe_to_mmap = safe_to_mmap;
}

#if !NV_CAN_CALL_VMA_START_WRITE
static NvBool nv_vma_enter_locked(struct vm_area_struct *vma, NvBool detaching)
{
    NvU32 tgt_refcnt = VMA_LOCK_OFFSET;
    NvBool interrupted = NV_FALSE;
    if (!detaching)
    {
        tgt_refcnt++;
    }
    if (!refcount_add_not_zero(VMA_LOCK_OFFSET, &vma->vm_refcnt))
    {
        return NV_FALSE;
    }

    rwsem_acquire(&vma->vmlock_dep_map, 0, 0, _RET_IP_);
    prepare_to_rcuwait(&vma->vm_mm->vma_writer_wait);

    for (;;)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if (refcount_read(&vma->vm_refcnt) == tgt_refcnt)
            break;

        if (signal_pending_state(TASK_UNINTERRUPTIBLE, current))
        {
            interrupted = NV_TRUE;
            break;
        }

        schedule();
    }

    // This is an open-coded version of finish_rcuwait().
    rcu_assign_pointer(vma->vm_mm->vma_writer_wait.task, NULL);
    __set_current_state(TASK_RUNNING);

    if (interrupted)
    {
        // Clean up on error: release refcount and dep_map
        refcount_sub_and_test(VMA_LOCK_OFFSET, &vma->vm_refcnt);
        rwsem_release(&vma->vmlock_dep_map, _RET_IP_);
        return NV_FALSE;
    }

    lock_acquired(&vma->vmlock_dep_map, _RET_IP_);
    return NV_TRUE;
}

/*
 * Helper function to handle VMA locking and refcount management.
 */
void nv_vma_start_write(struct vm_area_struct *vma)
{
    NvU32 mm_lock_seq;
    NvBool locked;
    if (__is_vma_write_locked(vma, &mm_lock_seq))
        return;

    locked = nv_vma_enter_locked(vma, NV_FALSE);

    WRITE_ONCE(vma->vm_lock_seq, mm_lock_seq);
    if (locked)
    {
        NvBool detached;
        detached = refcount_sub_and_test(VMA_LOCK_OFFSET, &vma->vm_refcnt);
        rwsem_release(&vma->vmlock_dep_map, _RET_IP_);
        WARN_ON_ONCE(detached);
    }
}
EXPORT_SYMBOL(nv_vma_start_write);
#endif // !NV_CAN_CALL_VMA_START_WRITE
