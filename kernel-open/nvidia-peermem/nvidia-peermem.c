/*
 * Copyright (c) 2006, 2007 Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2007, 2008 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/hugetlb.h>
#include <linux/pci.h>

#include "nv-p2p.h"
#include "peer_mem.h"
#include "conftest.h"

#define DRV_NAME    "nv_mem"
#define DRV_VERSION NV_VERSION_STRING

MODULE_AUTHOR("Yishai Hadas");
MODULE_DESCRIPTION("NVIDIA GPU memory plug-in");

MODULE_LICENSE("Dual BSD/GPL");

MODULE_VERSION(DRV_VERSION);
enum {
        NV_MEM_PEERDIRECT_SUPPORT_DEFAULT = 0,
        NV_MEM_PEERDIRECT_SUPPORT_LEGACY = 1,
};
static int peerdirect_support = NV_MEM_PEERDIRECT_SUPPORT_DEFAULT;
module_param(peerdirect_support, int, S_IRUGO);
MODULE_PARM_DESC(peerdirect_support, "Set level of support for Peer-direct, 0 [default] or 1 [legacy, for example MLNX_OFED 4.9 LTS]");

enum {
        NV_MEM_PERSISTENT_API_SUPPORT_LEGACY = 0,
        NV_MEM_PERSISTENT_API_SUPPORT_DEFAULT = 1,
};
static int persistent_api_support = NV_MEM_PERSISTENT_API_SUPPORT_DEFAULT;
module_param(persistent_api_support, int, S_IRUGO);
MODULE_PARM_DESC(persistent_api_support, "Set level of support for persistent APIs, 0 [legacy] or 1 [default]");

#define peer_err(FMT, ARGS...) printk(KERN_ERR "nvidia-peermem" " %s:%d ERROR " FMT, __FUNCTION__, __LINE__, ## ARGS)
#ifdef NV_MEM_DEBUG
#define peer_trace(FMT, ARGS...) printk(KERN_DEBUG "nvidia-peermem" " %s:%d TRACE " FMT, __FUNCTION__, __LINE__, ## ARGS)
#else
#define peer_trace(FMT, ARGS...) do {} while (0)
#endif

#if defined(NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT)

#ifndef READ_ONCE
#define READ_ONCE(x) ACCESS_ONCE(x)
#endif

#ifndef WRITE_ONCE
#define WRITE_ONCE(x, val) ({ ACCESS_ONCE(x) = (val); })
#endif

#define GPU_PAGE_SHIFT   16
#define GPU_PAGE_SIZE    ((u64)1 << GPU_PAGE_SHIFT)
#define GPU_PAGE_OFFSET  (GPU_PAGE_SIZE-1)
#define GPU_PAGE_MASK    (~GPU_PAGE_OFFSET)

invalidate_peer_memory mem_invalidate_callback;
static void *reg_handle = NULL;
static void *reg_handle_nc = NULL;

#define NV_MEM_CONTEXT_MAGIC ((u64)0xF1F4F1D0FEF0DAD0ULL)

struct nv_mem_context {
    u64 pad1;
    struct nvidia_p2p_page_table *page_table;
    struct nvidia_p2p_dma_mapping *dma_mapping;
    u64 core_context;
    u64 page_virt_start;
    u64 page_virt_end;
    size_t mapped_size;
    unsigned long npages;
    unsigned long page_size;
    struct task_struct *callback_task;
    int sg_allocated;
    struct sg_table sg_head;
    u64 pad2;
};

#define NV_MEM_CONTEXT_CHECK_OK(MC) ({                                  \
    struct nv_mem_context *mc = (MC);                                   \
    int rc = ((0 != mc) &&                                              \
              (READ_ONCE(mc->pad1) == NV_MEM_CONTEXT_MAGIC) &&          \
              (READ_ONCE(mc->pad2) == NV_MEM_CONTEXT_MAGIC));           \
    if (!rc) {                                                          \
        peer_trace("invalid nv_mem_context=%px pad1=%016llx pad2=%016llx\n", \
                   mc,                                                  \
                   mc?mc->pad1:0,                                       \
                   mc?mc->pad2:0);                                      \
    }                                                                   \
    rc;                                                                 \
})

static void nv_get_p2p_free_callback(void *data)
{
    int ret = 0;
    struct nv_mem_context *nv_mem_context = (struct nv_mem_context *)data;
    struct nvidia_p2p_page_table *page_table = NULL;
    struct nvidia_p2p_dma_mapping *dma_mapping = NULL;

    __module_get(THIS_MODULE);

    if (!NV_MEM_CONTEXT_CHECK_OK(nv_mem_context)) {
        peer_err("detected invalid context, skipping further processing\n");
        goto out;
    }

    if (!nv_mem_context->page_table) {
        peer_err("nv_get_p2p_free_callback -- invalid page_table\n");
        goto out;
    }

    /* Save page_table locally to prevent it being freed as part of nv_mem_release
     *  in case it's called internally by that callback.
     */
    page_table = nv_mem_context->page_table;

    if (!nv_mem_context->dma_mapping) {
        peer_err("nv_get_p2p_free_callback -- invalid dma_mapping\n");
        goto out;
    }
    dma_mapping = nv_mem_context->dma_mapping;

    /* For now don't set nv_mem_context->page_table to NULL,
     * confirmed by NVIDIA that inflight put_pages with valid pointer will fail gracefully.
     */

    nv_mem_context->callback_task = current;
    (*mem_invalidate_callback) (reg_handle, nv_mem_context->core_context);
    nv_mem_context->callback_task = NULL;

    ret = nvidia_p2p_free_dma_mapping(dma_mapping);
    if (ret)
        peer_err("nv_get_p2p_free_callback -- error %d while calling nvidia_p2p_free_dma_mapping()\n", ret);

    ret = nvidia_p2p_free_page_table(page_table);
    if (ret)
        peer_err("nv_get_p2p_free_callback -- error %d while calling nvidia_p2p_free_page_table()\n", ret);

out:
    module_put(THIS_MODULE);
    return;

}

/* At that function we don't call IB core - no ticket exists */
static void nv_mem_dummy_callback(void *data)
{
    struct nv_mem_context *nv_mem_context = (struct nv_mem_context *)data;
    int ret = 0;

    __module_get(THIS_MODULE);

    ret = nvidia_p2p_free_page_table(nv_mem_context->page_table);
    if (ret)
        peer_err("nv_mem_dummy_callback -- error %d while calling nvidia_p2p_free_page_table()\n", ret);

    module_put(THIS_MODULE);
    return;
}

/* acquire return code: 1 mine, 0 - not mine */
static int nv_mem_acquire(unsigned long addr, size_t size, void *peer_mem_private_data,
                          char *peer_mem_name, void **client_context)
{

    int ret = 0;
    struct nv_mem_context *nv_mem_context;

    nv_mem_context = kzalloc(sizeof *nv_mem_context, GFP_KERNEL);
    if (!nv_mem_context)
        /* Error case handled as not mine */
        return 0;

    nv_mem_context->pad1 = NV_MEM_CONTEXT_MAGIC;
    nv_mem_context->page_virt_start = addr & GPU_PAGE_MASK;
    nv_mem_context->page_virt_end   = (addr + size + GPU_PAGE_SIZE - 1) & GPU_PAGE_MASK;
    nv_mem_context->mapped_size  = nv_mem_context->page_virt_end - nv_mem_context->page_virt_start;
    nv_mem_context->pad2 = NV_MEM_CONTEXT_MAGIC;

    ret = nvidia_p2p_get_pages(0, 0, nv_mem_context->page_virt_start, nv_mem_context->mapped_size,
                               &nv_mem_context->page_table, nv_mem_dummy_callback, nv_mem_context);

    if (ret < 0)
        goto err;

    ret = nvidia_p2p_put_pages(0, 0, nv_mem_context->page_virt_start,
                               nv_mem_context->page_table);
    if (ret < 0) {
        /* Not expected, however in case callback was called on that buffer just before
            put pages we'll expect to fail gracefully (confirmed by NVIDIA) and return an error.
        */
        peer_err("nv_mem_acquire -- error %d while calling nvidia_p2p_put_pages()\n", ret);
        goto err;
    }

    /* 1 means mine */
    *client_context = nv_mem_context;
    __module_get(THIS_MODULE);
    return 1;

err:
    memset(nv_mem_context, 0, sizeof(*nv_mem_context));
    kfree(nv_mem_context);

    /* Error case handled as not mine */
    return 0;
}

static int nv_dma_map(struct sg_table *sg_head, void *context,
                      struct device *dma_device, int dmasync,
                      int *nmap)
{
    int i, ret;
    struct scatterlist *sg;
    struct nv_mem_context *nv_mem_context =
        (struct nv_mem_context *) context;
    struct nvidia_p2p_page_table *page_table = nv_mem_context->page_table;
    struct nvidia_p2p_dma_mapping *dma_mapping;
    struct pci_dev *pdev = to_pci_dev(dma_device);

    if (page_table->page_size != NVIDIA_P2P_PAGE_SIZE_64KB) {
        peer_err("nv_dma_map -- assumption of 64KB pages failed size_id=%u\n",
                    nv_mem_context->page_table->page_size);
        return -EINVAL;
    }

    if (!pdev) {
        peer_err("nv_dma_map -- invalid pci_dev\n");
        return -EINVAL;
    }

    ret = nvidia_p2p_dma_map_pages(pdev, page_table, &dma_mapping);
    if (ret) {
        peer_err("nv_dma_map -- error %d while calling nvidia_p2p_dma_map_pages()\n", ret);
        return ret;
    }

    if (!NVIDIA_P2P_DMA_MAPPING_VERSION_COMPATIBLE(dma_mapping)) {
        peer_err("error, incompatible dma mapping version 0x%08x\n",
                 dma_mapping->version);
        nvidia_p2p_dma_unmap_pages(pdev, page_table, dma_mapping);
        return -EINVAL;
    }

    nv_mem_context->npages = dma_mapping->entries;

    ret = sg_alloc_table(sg_head, dma_mapping->entries, GFP_KERNEL);
    if (ret) {
        nvidia_p2p_dma_unmap_pages(pdev, page_table, dma_mapping);
        return ret;
    }

    nv_mem_context->dma_mapping = dma_mapping;
    nv_mem_context->sg_allocated = 1;
    for_each_sg(sg_head->sgl, sg, nv_mem_context->npages, i) {
        sg_set_page(sg, NULL, nv_mem_context->page_size, 0);
        sg_dma_address(sg) = dma_mapping->dma_addresses[i];
        sg_dma_len(sg) = nv_mem_context->page_size;
    }
    nv_mem_context->sg_head = *sg_head;
    *nmap = nv_mem_context->npages;

    return 0;
}

static int nv_dma_unmap(struct sg_table *sg_head, void *context,
               struct device  *dma_device)
{
    struct pci_dev *pdev = to_pci_dev(dma_device);
    struct nv_mem_context *nv_mem_context =
        (struct nv_mem_context *)context;

    if (!nv_mem_context) {
        peer_err("nv_dma_unmap -- invalid nv_mem_context\n");
        return -EINVAL;
    }

    if (WARN_ON(0 != memcmp(sg_head, &nv_mem_context->sg_head, sizeof(*sg_head))))
        return -EINVAL;

    if (nv_mem_context->callback_task == current)
        goto out;

    if (nv_mem_context->dma_mapping)
        nvidia_p2p_dma_unmap_pages(pdev, nv_mem_context->page_table,
                                   nv_mem_context->dma_mapping);

out:
    return 0;
}

static void nv_mem_put_pages_common(int nc,
                                    struct sg_table *sg_head,
                                    void *context)
{
    int ret = 0;
    struct nv_mem_context *nv_mem_context =
        (struct nv_mem_context *) context;

    if (!nv_mem_context) {
        peer_err("nv_mem_put_pages -- invalid nv_mem_context\n");
        return;
    }

    if (WARN_ON(0 != memcmp(sg_head, &nv_mem_context->sg_head, sizeof(*sg_head))))
        return;

    if (nv_mem_context->callback_task == current)
        return;

    if (nc) {
#ifdef NVIDIA_P2P_CAP_GET_PAGES_PERSISTENT_API
        ret = nvidia_p2p_put_pages_persistent(nv_mem_context->page_virt_start,
                                              nv_mem_context->page_table, 0);
#else
        ret = nvidia_p2p_put_pages(0, 0, nv_mem_context->page_virt_start,
                                   nv_mem_context->page_table);
#endif
    } else {
        ret = nvidia_p2p_put_pages(0, 0, nv_mem_context->page_virt_start,
                                   nv_mem_context->page_table);
    }

#ifdef _DEBUG_ONLY_
    /* Here we expect an error in real life cases that should be ignored - not printed.
      * (e.g. concurrent callback with that call)
    */
    if (ret < 0) {
        printk(KERN_ERR "error %d while calling nvidia_p2p_put_pages, page_table=%p \n",
               ret,  nv_mem_context->page_table);
    }
#endif

    return;
}

static void nv_mem_put_pages(struct sg_table *sg_head, void *context)
{
    nv_mem_put_pages_common(0, sg_head, context);
}

static void nv_mem_put_pages_nc(struct sg_table *sg_head, void *context)
{
    nv_mem_put_pages_common(1, sg_head, context);
}

static void nv_mem_release(void *context)
{
    struct nv_mem_context *nv_mem_context =
        (struct nv_mem_context *) context;
    if (nv_mem_context->sg_allocated) {
        sg_free_table(&nv_mem_context->sg_head);
        nv_mem_context->sg_allocated = 0;
    }
    memset(nv_mem_context, 0, sizeof(*nv_mem_context));
    kfree(nv_mem_context);
    module_put(THIS_MODULE);
    return;
}

static int nv_mem_get_pages(unsigned long addr,
                            size_t size, int write, int force,
                            struct sg_table *sg_head,
                            void *client_context,
                            u64 core_context)
{
    int ret;
    struct nv_mem_context *nv_mem_context;

    nv_mem_context = (struct nv_mem_context *)client_context;
    if (!nv_mem_context)
        return -EINVAL;

    nv_mem_context->core_context = core_context;
    nv_mem_context->page_size = GPU_PAGE_SIZE;

    ret = nvidia_p2p_get_pages(0, 0, nv_mem_context->page_virt_start, nv_mem_context->mapped_size,
                               &nv_mem_context->page_table, nv_get_p2p_free_callback, nv_mem_context);
    if (ret < 0) {
        peer_err("error %d while calling nvidia_p2p_get_pages()\n", ret);
        return ret;
    }

    /* No extra access to nv_mem_context->page_table here as we are
        called not under a lock and may race with inflight invalidate callback on that buffer.
        Extra handling was delayed to be done under nv_dma_map.
     */
    return 0;
}

static unsigned long nv_mem_get_page_size(void *context)
{
    struct nv_mem_context *nv_mem_context =
                (struct nv_mem_context *)context;

    return nv_mem_context->page_size;
}


static struct peer_memory_client_ex nv_mem_client_ex = { .client = {
    .acquire        = nv_mem_acquire,
    .get_pages  = nv_mem_get_pages,
    .dma_map    = nv_dma_map,
    .dma_unmap  = nv_dma_unmap,
    .put_pages  = nv_mem_put_pages,
    .get_page_size  = nv_mem_get_page_size,
    .release        = nv_mem_release,
}};

static int nv_mem_get_pages_nc(unsigned long addr,
                            size_t size, int write, int force,
                            struct sg_table *sg_head,
                            void *client_context,
                            u64 core_context)
{
    int ret;
    struct nv_mem_context *nv_mem_context;

    nv_mem_context = (struct nv_mem_context *)client_context;
    if (!nv_mem_context)
        return -EINVAL;

    nv_mem_context->core_context = core_context;
    nv_mem_context->page_size = GPU_PAGE_SIZE;

#ifdef NVIDIA_P2P_CAP_GET_PAGES_PERSISTENT_API
    ret = nvidia_p2p_get_pages_persistent(nv_mem_context->page_virt_start,
                                          nv_mem_context->mapped_size,
                                          &nv_mem_context->page_table, 0);
#else
    ret = nvidia_p2p_get_pages(0, 0, nv_mem_context->page_virt_start, nv_mem_context->mapped_size,
                               &nv_mem_context->page_table, NULL, NULL);
#endif

    if (ret < 0) {
        peer_err("error %d while calling nvidia_p2p_get_pages() with NULL callback\n", ret);
        return ret;
    }

    return 0;
}

static struct peer_memory_client nv_mem_client_nc = {
    .acquire        = nv_mem_acquire,
    .get_pages      = nv_mem_get_pages_nc,
    .dma_map        = nv_dma_map,
    .dma_unmap      = nv_dma_unmap,
    .put_pages      = nv_mem_put_pages_nc,
    .get_page_size  = nv_mem_get_page_size,
    .release        = nv_mem_release,
};

static int nv_mem_legacy_client_init(void)
{
    // off by one, to leave space for the trailing '1' which is flagging
    // the new client type
    BUG_ON(strlen(DRV_NAME) > IB_PEER_MEMORY_NAME_MAX-1);
    strcpy(nv_mem_client_ex.client.name, DRV_NAME);

    // [VER_MAX-1]=1 <-- last byte is used as flag
    // [VER_MAX-2]=0 <-- version string terminator
    BUG_ON(strlen(DRV_VERSION) > IB_PEER_MEMORY_VER_MAX-2);
    strcpy(nv_mem_client_ex.client.version, DRV_VERSION);

    nv_mem_client_ex.client.version[IB_PEER_MEMORY_VER_MAX-1] = 1;

    if (peerdirect_support != NV_MEM_PEERDIRECT_SUPPORT_LEGACY) {
        nv_mem_client_ex.ex_size = sizeof(struct peer_memory_client_ex);
        // PEER_MEM_INVALIDATE_UNMAPS allow clients to opt out of
        // unmap/put_pages during invalidation, i.e. the client tells the
        // infiniband layer that it does not need to call
        // unmap/put_pages in the invalidation callback
        nv_mem_client_ex.flags = PEER_MEM_INVALIDATE_UNMAPS;
    } else {
        nv_mem_client_ex.ex_size = 0;
        nv_mem_client_ex.flags = 0;
    }

    reg_handle = ib_register_peer_memory_client(&nv_mem_client_ex.client,
                         &mem_invalidate_callback);
    if (!reg_handle) {
        peer_err("nv_mem_client_init -- error while registering traditional client\n");
        return -EINVAL;
    }
    return 0;
}

static int nv_mem_nc_client_init(void)
{
    // The nc client enables support for persistent pages.
    if (persistent_api_support == NV_MEM_PERSISTENT_API_SUPPORT_LEGACY)
    {
        //
        // If legacy behavior is forced via module param,
        // both legacy and persistent clients are registered and are named
        // "nv_mem"(legacy) and "nv_mem_nc"(persistent).
        //
        strcpy(nv_mem_client_nc.name, DRV_NAME "_nc");
    }
    else
    {
        //
        // With default persistent behavior, the client name shall be "nv_mem"
        // so that libraries can use the persistent client under the same name.
        //
        strcpy(nv_mem_client_nc.name, DRV_NAME);
    }

    strcpy(nv_mem_client_nc.version, DRV_VERSION);
    reg_handle_nc = ib_register_peer_memory_client(&nv_mem_client_nc, NULL);
    if (!reg_handle_nc) {
        peer_err("nv_mem_client_init -- error while registering nc client\n");
        return -EINVAL;
    }
    return 0;
}

#endif /* NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT */

static int nv_mem_param_peerdirect_conf_check(void)
{
    int rc = 0;
    switch (peerdirect_support) {
    case NV_MEM_PEERDIRECT_SUPPORT_DEFAULT:
    case NV_MEM_PEERDIRECT_SUPPORT_LEGACY:
        break;
    default:
        peer_err("invalid peerdirect_support param value %d\n", peerdirect_support);
        rc = -EINVAL;
        break;
    }
    return rc;
}

static int nv_mem_param_persistent_api_conf_check(void)
{
    int rc = 0;
    switch (persistent_api_support) {
    case NV_MEM_PERSISTENT_API_SUPPORT_DEFAULT:
    case NV_MEM_PERSISTENT_API_SUPPORT_LEGACY:
        break;
    default:
        peer_err("invalid persistent_api_support param value %d\n", persistent_api_support);
        rc = -EINVAL;
        break;
    }
    return rc;
}

static int __init nv_mem_client_init(void)
{
#if defined (NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT)
    int rc;
    rc = nv_mem_param_peerdirect_conf_check();
    if (rc) {
        return rc;
    }

    rc = nv_mem_param_persistent_api_conf_check();
    if (rc) {
        return rc;
    }

    if (persistent_api_support == NV_MEM_PERSISTENT_API_SUPPORT_LEGACY) {
        rc = nv_mem_legacy_client_init();
        if (rc)
            goto out;
    }

    rc = nv_mem_nc_client_init();
    if (rc)
        goto out;

out:
    if (rc) {
        if (reg_handle) {
            ib_unregister_peer_memory_client(reg_handle);
            reg_handle = NULL;
        }

        if (reg_handle_nc) {
            ib_unregister_peer_memory_client(reg_handle_nc);
            reg_handle_nc = NULL;
        }
    }

    return rc;
#else
    return -EINVAL;
#endif
}

static void __exit nv_mem_client_cleanup(void)
{
#if defined (NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT)
    if (reg_handle)
        ib_unregister_peer_memory_client(reg_handle);

    if (reg_handle_nc)
        ib_unregister_peer_memory_client(reg_handle_nc);
#endif
}

module_init(nv_mem_client_init);
module_exit(nv_mem_client_cleanup);
