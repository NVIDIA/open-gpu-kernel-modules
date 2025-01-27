/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_DRM_PRIME_H_PRESENT)
#include <drm/drm_prime.h>
#endif

#include "nvidia-drm-gem-user-memory.h"
#include "nvidia-drm-helper.h"
#include "nvidia-drm-ioctl.h"

#include "linux/dma-buf.h"
#include "linux/mm.h"
#include "nv-mm.h"

#if defined(NV_LINUX_PFN_T_H_PRESENT)
#include "linux/pfn_t.h"
#endif

#if defined(NV_BSD)
#include <vm/vm_pageout.h>
#endif

static inline
void __nv_drm_gem_user_memory_free(struct nv_drm_gem_object *nv_gem)
{
    struct nv_drm_gem_user_memory *nv_user_memory = to_nv_user_memory(nv_gem);

    nv_drm_unlock_user_pages(nv_user_memory->pages_count,
                             nv_user_memory->pages);

    nv_drm_free(nv_user_memory);
}

static struct sg_table *__nv_drm_gem_user_memory_prime_get_sg_table(
    struct nv_drm_gem_object *nv_gem)
{
    struct nv_drm_gem_user_memory *nv_user_memory = to_nv_user_memory(nv_gem);
    struct drm_gem_object *gem = &nv_gem->base;

    return nv_drm_prime_pages_to_sg(gem->dev,
                                    nv_user_memory->pages,
                                    nv_user_memory->pages_count);
}

static void *__nv_drm_gem_user_memory_prime_vmap(
    struct nv_drm_gem_object *nv_gem)
{
    struct nv_drm_gem_user_memory *nv_user_memory = to_nv_user_memory(nv_gem);

    return nv_drm_vmap(nv_user_memory->pages,
                           nv_user_memory->pages_count,
                           true);
}

static void __nv_drm_gem_user_memory_prime_vunmap(
    struct nv_drm_gem_object *gem,
    void *address)
{
    nv_drm_vunmap(address);
}

static int __nv_drm_gem_user_memory_mmap(struct nv_drm_gem_object *nv_gem,
                                         struct vm_area_struct *vma)
{
    int ret = drm_gem_mmap_obj(&nv_gem->base,
                drm_vma_node_size(&nv_gem->base.vma_node) << PAGE_SHIFT, vma);

    if (ret < 0) {
        return ret;
    }

    /*
     * Enforce that user-memory GEM mappings are MAP_SHARED, to prevent COW
     * with MAP_PRIVATE and VM_MIXEDMAP
     */
    if (!(vma->vm_flags & VM_SHARED)) {
        return -EINVAL;
    }

    nv_vm_flags_clear(vma, VM_PFNMAP);
    nv_vm_flags_clear(vma, VM_IO);
    nv_vm_flags_set(vma, VM_MIXEDMAP);

    return 0;
}

#if defined(NV_LINUX) && !defined(NV_VMF_INSERT_MIXED_PRESENT)
static vm_fault_t __nv_vm_insert_mixed_helper(
    struct vm_area_struct *vma,
    unsigned long address,
    unsigned long pfn)
{
    int ret;

#if defined(NV_PFN_TO_PFN_T_PRESENT)
    ret = vm_insert_mixed(vma, address, pfn_to_pfn_t(pfn));
#else
    ret = vm_insert_mixed(vma, address, pfn);
#endif

    switch (ret) {
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
            WARN_ONCE(1, "Unhandled error in %s: %d\n", __FUNCTION__, ret);
            return VM_FAULT_SIGBUS;
    }
}
#endif

static vm_fault_t __nv_drm_gem_user_memory_handle_vma_fault(
    struct nv_drm_gem_object *nv_gem,
    struct vm_area_struct *vma,
    struct vm_fault *vmf)
{
    struct nv_drm_gem_user_memory *nv_user_memory = to_nv_user_memory(nv_gem);
    unsigned long address = nv_page_fault_va(vmf);
    struct drm_gem_object *gem = vma->vm_private_data;
    unsigned long page_offset;
    unsigned long pfn;

    page_offset = vmf->pgoff - drm_vma_node_start(&gem->vma_node);
    BUG_ON(page_offset >= nv_user_memory->pages_count);
    pfn = page_to_pfn(nv_user_memory->pages[page_offset]);

#if !defined(NV_LINUX)
    return vmf_insert_pfn(vma, address, pfn);
#elif defined(NV_VMF_INSERT_MIXED_PRESENT)
    return vmf_insert_mixed(vma, address, pfn_to_pfn_t(pfn));
#else
    return __nv_vm_insert_mixed_helper(vma, address, pfn);
#endif
}

static int __nv_drm_gem_user_create_mmap_offset(
    struct nv_drm_device *nv_dev,
    struct nv_drm_gem_object *nv_gem,
    uint64_t *offset)
{
    (void)nv_dev;
    return nv_drm_gem_create_mmap_offset(nv_gem, offset);
}

const struct nv_drm_gem_object_funcs __nv_gem_user_memory_ops = {
    .free = __nv_drm_gem_user_memory_free,
    .prime_get_sg_table = __nv_drm_gem_user_memory_prime_get_sg_table,
    .prime_vmap = __nv_drm_gem_user_memory_prime_vmap,
    .prime_vunmap = __nv_drm_gem_user_memory_prime_vunmap,
    .mmap = __nv_drm_gem_user_memory_mmap,
    .handle_vma_fault = __nv_drm_gem_user_memory_handle_vma_fault,
    .create_mmap_offset = __nv_drm_gem_user_create_mmap_offset,
};

int nv_drm_gem_import_userspace_memory_ioctl(struct drm_device *dev,
                                             void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    struct drm_nvidia_gem_import_userspace_memory_params *params = data;
    struct nv_drm_gem_user_memory *nv_user_memory;

    struct page **pages = NULL;
    unsigned long pages_count = 0;

    int ret = 0;

    if ((params->size % PAGE_SIZE) != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Userspace memory 0x%" NvU64_fmtx " size should be in a multiple of page "
            "size to create a gem object",
            params->address);
        return -EINVAL;
    }

    pages_count = params->size / PAGE_SIZE;

    ret = nv_drm_lock_user_pages(params->address, pages_count, &pages);

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lock user pages for address 0x%" NvU64_fmtx ": %d",
            params->address, ret);
        return ret;
    }

    if ((nv_user_memory =
            nv_drm_calloc(1, sizeof(*nv_user_memory))) == NULL) {
        ret = -ENOMEM;
        goto failed;
    }

    nv_user_memory->pages = pages;
    nv_user_memory->pages_count = pages_count;

    nv_drm_gem_object_init(nv_dev,
                           &nv_user_memory->base,
                           &__nv_gem_user_memory_ops,
                           params->size,
                           NULL /* pMemory */);

    return nv_drm_gem_handle_create_drop_reference(filep,
                                                   &nv_user_memory->base,
                                                   &params->handle);

failed:
    nv_drm_unlock_user_pages(pages_count, pages);

    return ret;
}

#endif
