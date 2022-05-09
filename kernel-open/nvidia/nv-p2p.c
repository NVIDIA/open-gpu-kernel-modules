/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nv-ibmnpu.h"
#include "nv-rsync.h"

#include "nv-p2p.h"
#include "rmp2pdefines.h"

typedef struct nv_p2p_dma_mapping {
    struct list_head list_node;
    struct nvidia_p2p_dma_mapping *dma_mapping;
} nv_p2p_dma_mapping_t;

typedef struct nv_p2p_mem_info {
    void (*free_callback)(void *data);
    void *data;
    struct nvidia_p2p_page_table page_table;
    struct {
        struct list_head list_head;
        struct semaphore lock;
    } dma_mapping_list;
    NvBool bPersistent;
    void *private;
} nv_p2p_mem_info_t;

int nvidia_p2p_cap_persistent_pages = 1;
EXPORT_SYMBOL(nvidia_p2p_cap_persistent_pages);

// declared and created in nv.c
extern void *nvidia_p2p_page_t_cache;

static struct nvidia_status_mapping {
    NV_STATUS status;
    int error;
} nvidia_status_mappings[] = {
    { NV_ERR_GENERIC,                -EIO      },
    { NV_ERR_INSUFFICIENT_RESOURCES, -ENOMEM   },
    { NV_ERR_NO_MEMORY,              -ENOMEM   },
    { NV_ERR_INVALID_ARGUMENT,       -EINVAL   },
    { NV_ERR_INVALID_OBJECT_HANDLE,  -EINVAL   },
    { NV_ERR_INVALID_STATE,          -EIO      },
    { NV_ERR_NOT_SUPPORTED,          -ENOTSUPP },
    { NV_ERR_OBJECT_NOT_FOUND,       -EINVAL   },
    { NV_ERR_STATE_IN_USE,           -EBUSY    },
    { NV_ERR_GPU_UUID_NOT_FOUND,     -ENODEV   },
    { NV_OK,                          0        },
};

#define NVIDIA_STATUS_MAPPINGS \
    (sizeof(nvidia_status_mappings) / sizeof(struct nvidia_status_mapping))

static int nvidia_p2p_map_status(NV_STATUS status)
{
    int error = -EIO;
    uint8_t i;

    for (i = 0; i < NVIDIA_STATUS_MAPPINGS; i++)
    {
        if (nvidia_status_mappings[i].status == status)
        {
            error = nvidia_status_mappings[i].error;
            break;
        }
    }
    return error;
}

static NvU32 nvidia_p2p_page_size_mappings[NVIDIA_P2P_PAGE_SIZE_COUNT] = {
    NVRM_P2P_PAGESIZE_SMALL_4K, NVRM_P2P_PAGESIZE_BIG_64K, NVRM_P2P_PAGESIZE_BIG_128K
};

static NV_STATUS nvidia_p2p_map_page_size(NvU32 page_size, NvU32 *page_size_index)
{
    NvU32 i;

    for (i = 0; i < NVIDIA_P2P_PAGE_SIZE_COUNT; i++)
    {
        if (nvidia_p2p_page_size_mappings[i] == page_size)
        {
            *page_size_index = i;
            break;
        }
    }

    if (i == NVIDIA_P2P_PAGE_SIZE_COUNT)
        return NV_ERR_GENERIC;

    return NV_OK;
}

static NV_STATUS nv_p2p_insert_dma_mapping(
    struct nv_p2p_mem_info *mem_info,
    struct nvidia_p2p_dma_mapping *dma_mapping
)
{
    NV_STATUS status;
    struct nv_p2p_dma_mapping *node;

    status = os_alloc_mem((void**)&node, sizeof(*node));
    if (status != NV_OK)
    {
        return status;
    }

    down(&mem_info->dma_mapping_list.lock);

    node->dma_mapping = dma_mapping;
    list_add_tail(&node->list_node, &mem_info->dma_mapping_list.list_head);

    up(&mem_info->dma_mapping_list.lock);

    return NV_OK;
}

static struct nvidia_p2p_dma_mapping* nv_p2p_remove_dma_mapping(
    struct nv_p2p_mem_info *mem_info,
    struct nvidia_p2p_dma_mapping *dma_mapping
)
{
    struct nv_p2p_dma_mapping *cur;
    struct nvidia_p2p_dma_mapping *ret_dma_mapping = NULL;

    down(&mem_info->dma_mapping_list.lock);

    list_for_each_entry(cur, &mem_info->dma_mapping_list.list_head, list_node)
    {
        if (dma_mapping == NULL || dma_mapping == cur->dma_mapping)
        {
            ret_dma_mapping = cur->dma_mapping;
            list_del(&cur->list_node);
            os_free_mem(cur);
            break;
        }
    }

    up(&mem_info->dma_mapping_list.lock);

    return ret_dma_mapping;
}

static void nv_p2p_free_dma_mapping(
    struct nvidia_p2p_dma_mapping *dma_mapping
)
{
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    NvU32 page_size;
    NV_STATUS status;
    NvU32 i;

    peer_dma_dev.dev = &dma_mapping->pci_dev->dev;
    peer_dma_dev.addressable_range.limit = dma_mapping->pci_dev->dma_mask;

    page_size = nvidia_p2p_page_size_mappings[dma_mapping->page_size_type];

    if (dma_mapping->private != NULL)
    {
        WARN_ON(page_size != PAGE_SIZE);

        status = nv_dma_unmap_alloc(&peer_dma_dev,
                                    dma_mapping->entries,
                                    dma_mapping->dma_addresses,
                                    &dma_mapping->private);
        WARN_ON(status != NV_OK);
    }
    else
    {
        for (i = 0; i < dma_mapping->entries; i++)
        {
            nv_dma_unmap_peer(&peer_dma_dev, page_size / PAGE_SIZE,
                              dma_mapping->dma_addresses[i]);
        }
    }

    os_free_mem(dma_mapping->dma_addresses);

    os_free_mem(dma_mapping);
}

static void nv_p2p_free_page_table(
    struct nvidia_p2p_page_table *page_table
)
{
    NvU32 i;
    struct nvidia_p2p_dma_mapping *dma_mapping;
    struct nv_p2p_mem_info *mem_info = NULL;

    mem_info = container_of(page_table, nv_p2p_mem_info_t, page_table);

    dma_mapping = nv_p2p_remove_dma_mapping(mem_info, NULL);
    while (dma_mapping != NULL)
    {
        nv_p2p_free_dma_mapping(dma_mapping);

        dma_mapping = nv_p2p_remove_dma_mapping(mem_info, NULL);
    }

    for (i = 0; i < page_table->entries; i++)
    {
        NV_KMEM_CACHE_FREE(page_table->pages[i], nvidia_p2p_page_t_cache);
    }

    if (page_table->gpu_uuid != NULL)
    {
        os_free_mem(page_table->gpu_uuid);
    }

    if (page_table->pages != NULL)
    {
        os_free_mem(page_table->pages);
    }

    os_free_mem(mem_info);
}

static NV_STATUS nv_p2p_put_pages(
    nvidia_stack_t * sp,
    uint64_t p2p_token,
    uint32_t va_space,
    uint64_t virtual_address,
    struct nvidia_p2p_page_table **page_table
)
{
    NV_STATUS status;
    struct nv_p2p_mem_info *mem_info = NULL;

    mem_info = container_of(*page_table, nv_p2p_mem_info_t, page_table);

    /*
     * rm_p2p_put_pages returns NV_OK if the page_table was found and
     * got unlinked from the RM's tracker (atomically). This ensures that
     * RM's tear-down path does not race with this path.
     *
     * rm_p2p_put_pages returns NV_ERR_OBJECT_NOT_FOUND if the page_table
     * was already unlinked.
     */
    if (mem_info->bPersistent)
    {
        status = rm_p2p_put_pages_persistent(sp, mem_info->private, *page_table);
    }
    else
    {
        status = rm_p2p_put_pages(sp, p2p_token, va_space,
                                  virtual_address, *page_table);
    }

    if (status == NV_OK)
    {
        nv_p2p_free_page_table(*page_table);
        *page_table = NULL;
    }
    else if (!mem_info->bPersistent && (status == NV_ERR_OBJECT_NOT_FOUND))
    {
        status = NV_OK;
        *page_table = NULL;
    }
    else
    {
        WARN_ON(status != NV_OK);
    }

    return status;
}

void NV_API_CALL nv_p2p_free_platform_data(
    void *data
)
{
    if (data == NULL)
    {
        WARN_ON(data == NULL);
        return;
    }

    nv_p2p_free_page_table((struct nvidia_p2p_page_table*)data);
}

int nvidia_p2p_init_mapping(
    uint64_t p2p_token,
    struct nvidia_p2p_params *params,
    void (*destroy_callback)(void *data),
    void *data
)
{
    return -ENOTSUPP;
}

EXPORT_SYMBOL(nvidia_p2p_init_mapping);

int nvidia_p2p_destroy_mapping(uint64_t p2p_token)
{
    return -ENOTSUPP;
}

EXPORT_SYMBOL(nvidia_p2p_destroy_mapping);

static void nv_p2p_mem_info_free_callback(void *data)
{
    nv_p2p_mem_info_t *mem_info = (nv_p2p_mem_info_t*) data;

    mem_info->free_callback(mem_info->data);

    nv_p2p_free_platform_data(&mem_info->page_table);
}

int nvidia_p2p_get_pages(
    uint64_t p2p_token,
    uint32_t va_space,
    uint64_t virtual_address,
    uint64_t length,
    struct nvidia_p2p_page_table **page_table,
    void (*free_callback)(void * data),
    void *data
)
{
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;
    struct nvidia_p2p_page *page;
    struct nv_p2p_mem_info *mem_info = NULL;
    NvU32 entries;
    NvU32 *wreqmb_h = NULL;
    NvU32 *rreqmb_h = NULL;
    NvU64 *physical_addresses = NULL;
    NvU32 page_count;
    NvU32 i = 0;
    NvBool bGetPages = NV_FALSE;
    NvBool bGetUuid = NV_FALSE;
    NvU32 page_size = NVRM_P2P_PAGESIZE_BIG_64K;
    NvU32 page_size_index;
    NvU64 temp_length;
    NvU8 *gpu_uuid = NULL;
    NvU8 uuid[NVIDIA_P2P_GPU_UUID_LEN] = {0};
    int rc;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return rc;
    }

    *page_table = NULL;
    status = os_alloc_mem((void **)&mem_info, sizeof(*mem_info));
    if (status != NV_OK)
    {
        goto failed;
    }

    memset(mem_info, 0, sizeof(*mem_info));

    INIT_LIST_HEAD(&mem_info->dma_mapping_list.list_head);
    NV_INIT_MUTEX(&mem_info->dma_mapping_list.lock);

    *page_table = &(mem_info->page_table);

    mem_info->bPersistent = (free_callback == NULL);

    //asign length to temporary variable since do_div macro does in-place division
    temp_length = length;
    do_div(temp_length, page_size);
    page_count = temp_length;

    if (length & (page_size - 1))
    {
        page_count++;
    }

    status = os_alloc_mem((void **)&physical_addresses,
            (page_count * sizeof(NvU64)));
    if (status != NV_OK)
    {
        goto failed;
    }
    status = os_alloc_mem((void **)&wreqmb_h, (page_count * sizeof(NvU32)));
    if (status != NV_OK)
    {
        goto failed;
    }
    status = os_alloc_mem((void **)&rreqmb_h, (page_count * sizeof(NvU32)));
    if (status != NV_OK)
    {
        goto failed;
    }

    if (mem_info->bPersistent)
    {
        void *gpu_info = NULL;

        if ((p2p_token != 0) || (va_space != 0))
        {
            status = -ENOTSUPP;
            goto failed;
        }

        status = rm_p2p_get_gpu_info(sp, virtual_address, length, &gpu_uuid, &gpu_info);
        if (status != NV_OK)
        {
            goto failed;
        }

        rc = nvidia_dev_get_uuid(gpu_uuid, sp);
        if (rc != 0)
        {
            status = NV_ERR_GPU_UUID_NOT_FOUND;
            goto failed;
        }

        os_mem_copy(uuid, gpu_uuid, NVIDIA_P2P_GPU_UUID_LEN);

        bGetUuid = NV_TRUE;

        status = rm_p2p_get_pages_persistent(sp, virtual_address, length, &mem_info->private,
                                             physical_addresses, &entries, *page_table, gpu_info);
        if (status != NV_OK)
        {
            goto failed;
        }
    }
    else
    {
        // Get regular old-style, non-persistent mappings
        status = rm_p2p_get_pages(sp, p2p_token, va_space,
                virtual_address, length, physical_addresses, wreqmb_h,
                rreqmb_h, &entries, &gpu_uuid, *page_table);
        if (status != NV_OK)
        {
            goto failed;
        }
    }

    bGetPages = NV_TRUE;
    (*page_table)->gpu_uuid = gpu_uuid;

    status = os_alloc_mem((void *)&(*page_table)->pages,
             (entries * sizeof(page)));
    if (status != NV_OK)
    {
        goto failed;
    }

    (*page_table)->version = NVIDIA_P2P_PAGE_TABLE_VERSION;

    for (i = 0; i < entries; i++)
    {
        page = NV_KMEM_CACHE_ALLOC(nvidia_p2p_page_t_cache);
        if (page == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto failed;
        }

        memset(page, 0, sizeof(*page));

        page->physical_address = physical_addresses[i];
        page->registers.fermi.wreqmb_h = wreqmb_h[i];
        page->registers.fermi.rreqmb_h = rreqmb_h[i];

        (*page_table)->pages[i] = page;
        (*page_table)->entries++;
    }

    status = nvidia_p2p_map_page_size(page_size, &page_size_index);
    if (status != NV_OK)
    {
        goto failed;
    }

    (*page_table)->page_size = page_size_index;

    os_free_mem(physical_addresses);
    os_free_mem(wreqmb_h);
    os_free_mem(rreqmb_h);

    if (free_callback != NULL)
    {
        mem_info->free_callback = free_callback;
        mem_info->data          = data;

        status = rm_p2p_register_callback(sp, p2p_token, virtual_address, length,
                                          *page_table, nv_p2p_mem_info_free_callback, mem_info);
        if (status != NV_OK)
        {
            goto failed;
        }
    }

    nv_kmem_cache_free_stack(sp);

    return nvidia_p2p_map_status(status);

failed:
    if (physical_addresses != NULL)
    {
        os_free_mem(physical_addresses);
    }
    if (wreqmb_h != NULL)
    {
        os_free_mem(wreqmb_h);
    }
    if (rreqmb_h != NULL)
    {
        os_free_mem(rreqmb_h);
    }

    if (bGetPages)
    {
        (void)nv_p2p_put_pages(sp, p2p_token, va_space,
                               virtual_address, page_table);
    }

    if (bGetUuid)
    {
        nvidia_dev_put_uuid(uuid, sp);
    }

    if (*page_table != NULL)
    {
        nv_p2p_free_page_table(*page_table);
    }

    nv_kmem_cache_free_stack(sp);

    return nvidia_p2p_map_status(status);
}

EXPORT_SYMBOL(nvidia_p2p_get_pages);

/*
 * This function is a no-op, but is left in place (for now), in order to allow
 * third-party callers to build and run without errors or warnings. This is OK,
 * because the missing functionality is provided by nv_p2p_free_platform_data,
 * which is being called as part of the RM's cleanup path.
 */
int nvidia_p2p_free_page_table(struct nvidia_p2p_page_table *page_table)
{
    return 0;
}

EXPORT_SYMBOL(nvidia_p2p_free_page_table);

int nvidia_p2p_put_pages(
    uint64_t p2p_token,
    uint32_t va_space,
    uint64_t virtual_address,
    struct nvidia_p2p_page_table *page_table
)
{
    struct nv_p2p_mem_info *mem_info = NULL;
    NvU8 uuid[NVIDIA_P2P_GPU_UUID_LEN] = {0};
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;
    int rc = 0;

    os_mem_copy(uuid, page_table->gpu_uuid, NVIDIA_P2P_GPU_UUID_LEN);

    mem_info = container_of(page_table, nv_p2p_mem_info_t, page_table);

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return -ENOMEM;
    }

    status = nv_p2p_put_pages(sp, p2p_token, va_space,
                              virtual_address, &page_table);

    if (mem_info->bPersistent)
    {
        nvidia_dev_put_uuid(uuid, sp);
    }

    nv_kmem_cache_free_stack(sp);

    return nvidia_p2p_map_status(status);
}

EXPORT_SYMBOL(nvidia_p2p_put_pages);

int nvidia_p2p_dma_map_pages(
    struct pci_dev *peer,
    struct nvidia_p2p_page_table *page_table,
    struct nvidia_p2p_dma_mapping **dma_mapping
)
{
    NV_STATUS status;
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    nvidia_stack_t *sp = NULL;
    NvU64 *dma_addresses = NULL;
    NvU32 page_count;
    NvU32 page_size;
    enum nvidia_p2p_page_size_type page_size_type;
    struct nv_p2p_mem_info *mem_info = NULL;
    NvU32 i;
    void *priv;
    int rc;

    if (peer == NULL || page_table == NULL || dma_mapping == NULL ||
        page_table->gpu_uuid == NULL)
    {
        return -EINVAL;
    }

    mem_info = container_of(page_table, nv_p2p_mem_info_t, page_table);

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return rc;
    }

    *dma_mapping = NULL;
    status = os_alloc_mem((void **)dma_mapping, sizeof(**dma_mapping));
    if (status != NV_OK)
    {
        goto failed;
    }
    memset(*dma_mapping, 0, sizeof(**dma_mapping));

    page_count = page_table->entries;

    status = os_alloc_mem((void **)&dma_addresses,
            (page_count * sizeof(NvU64)));
    if (status != NV_OK)
    {
        goto failed;
    }

    page_size_type = page_table->page_size;

    BUG_ON((page_size_type <= NVIDIA_P2P_PAGE_SIZE_4KB) ||
           (page_size_type >= NVIDIA_P2P_PAGE_SIZE_COUNT));

    peer_dma_dev.dev = &peer->dev;
    peer_dma_dev.addressable_range.limit = peer->dma_mask;

    page_size = nvidia_p2p_page_size_mappings[page_size_type];

    for (i = 0; i < page_count; i++)
    {
        dma_addresses[i] = page_table->pages[i]->physical_address;
    }

    status = rm_p2p_dma_map_pages(sp, &peer_dma_dev,
            page_table->gpu_uuid, page_size, page_count, dma_addresses, &priv);
    if (status != NV_OK)
    {
        goto failed;
    }

    (*dma_mapping)->version = NVIDIA_P2P_DMA_MAPPING_VERSION;
    (*dma_mapping)->page_size_type = page_size_type;
    (*dma_mapping)->entries = page_count;
    (*dma_mapping)->dma_addresses = dma_addresses;
    (*dma_mapping)->private = priv;
    (*dma_mapping)->pci_dev = peer;

    /*
     * All success, it is safe to insert dma_mapping now.
     */
    status = nv_p2p_insert_dma_mapping(mem_info, *dma_mapping);
    if (status != NV_OK)
    {
        goto failed_insert;
    }

    nv_kmem_cache_free_stack(sp);

    return 0;

failed_insert:
    nv_p2p_free_dma_mapping(*dma_mapping);
    dma_addresses = NULL;
    *dma_mapping = NULL;

failed:
    if (dma_addresses != NULL)
    {
        os_free_mem(dma_addresses);
    }

    if (*dma_mapping != NULL)
    {
        os_free_mem(*dma_mapping);
        *dma_mapping = NULL;
    }

    nv_kmem_cache_free_stack(sp);

    return nvidia_p2p_map_status(status);
}

EXPORT_SYMBOL(nvidia_p2p_dma_map_pages);

int nvidia_p2p_dma_unmap_pages(
    struct pci_dev *peer,
    struct nvidia_p2p_page_table *page_table,
    struct nvidia_p2p_dma_mapping *dma_mapping
)
{
    struct nv_p2p_mem_info *mem_info = NULL;

    if (peer == NULL || dma_mapping == NULL || page_table == NULL)
    {
        return -EINVAL;
    }

    mem_info = container_of(page_table, nv_p2p_mem_info_t, page_table);

    /*
     * nv_p2p_remove_dma_mapping returns dma_mapping if the dma_mapping was
     * found and got unlinked from the mem_info->dma_mapping_list (atomically).
     * This ensures that the RM's tear-down path does not race with this path.
     *
     * nv_p2p_remove_dma_mappings returns NULL if the dma_mapping was already
     * unlinked.
     */
    if (nv_p2p_remove_dma_mapping(mem_info, dma_mapping) == NULL)
    {
        return 0;
    }

    WARN_ON(peer != dma_mapping->pci_dev);

    BUG_ON((dma_mapping->page_size_type <= NVIDIA_P2P_PAGE_SIZE_4KB) ||
           (dma_mapping->page_size_type >= NVIDIA_P2P_PAGE_SIZE_COUNT));

    nv_p2p_free_dma_mapping(dma_mapping);

    return 0;
}

EXPORT_SYMBOL(nvidia_p2p_dma_unmap_pages);

/*
 * This function is a no-op, but is left in place (for now), in order to allow
 * third-party callers to build and run without errors or warnings. This is OK,
 * because the missing functionality is provided by nv_p2p_free_platform_data,
 * which is being called as part of the RM's cleanup path.
 */
int nvidia_p2p_free_dma_mapping(
    struct nvidia_p2p_dma_mapping *dma_mapping
)
{
    return 0;
}

EXPORT_SYMBOL(nvidia_p2p_free_dma_mapping);

int nvidia_p2p_register_rsync_driver(
    nvidia_p2p_rsync_driver_t *driver,
    void *data
)
{
    if (driver == NULL)
    {
        return -EINVAL;
    }

    if (!NVIDIA_P2P_RSYNC_DRIVER_VERSION_COMPATIBLE(driver))
    {
        return -EINVAL;
    }

    if (driver->get_relaxed_ordering_mode == NULL ||
        driver->put_relaxed_ordering_mode == NULL ||
        driver->wait_for_rsync == NULL)
    {
        return -EINVAL;
    }

    return nv_register_rsync_driver(driver->get_relaxed_ordering_mode,
                                    driver->put_relaxed_ordering_mode,
                                    driver->wait_for_rsync, data);
}

EXPORT_SYMBOL(nvidia_p2p_register_rsync_driver);

void nvidia_p2p_unregister_rsync_driver(
    nvidia_p2p_rsync_driver_t *driver,
    void *data
)
{
    if (driver == NULL)
    {
        WARN_ON(1);
        return;
    }

    if (!NVIDIA_P2P_RSYNC_DRIVER_VERSION_COMPATIBLE(driver))
    {
        WARN_ON(1);
        return;
    }

    if (driver->get_relaxed_ordering_mode == NULL ||
        driver->put_relaxed_ordering_mode == NULL ||
        driver->wait_for_rsync == NULL)
    {
        WARN_ON(1);
        return;
    }

    nv_unregister_rsync_driver(driver->get_relaxed_ordering_mode,
                               driver->put_relaxed_ordering_mode,
                               driver->wait_for_rsync, data);
}

EXPORT_SYMBOL(nvidia_p2p_unregister_rsync_driver);

int nvidia_p2p_get_rsync_registers(
    nvidia_p2p_rsync_reg_info_t **reg_info
)
{
    nv_linux_state_t *nvl;
    nv_state_t *nv;
    NV_STATUS status;
    void *ptr = NULL;
    NvU64 addr;
    NvU64 size;
    struct pci_dev *ibmnpu = NULL;
    NvU32 index = 0;
    NvU32 count = 0;
    nvidia_p2p_rsync_reg_info_t *info = NULL;
    nvidia_p2p_rsync_reg_t *regs = NULL;

    if (reg_info == NULL)
    {
        return -EINVAL;
    }

    status = os_alloc_mem((void**)&info, sizeof(*info));
    if (status != NV_OK)
    {
        return -ENOMEM;
    }

    memset(info, 0, sizeof(*info));

    info->version = NVIDIA_P2P_RSYNC_REG_INFO_VERSION;

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl; nvl = nvl->next)
    {
        count++;
    }

    status = os_alloc_mem((void**)&regs, (count * sizeof(*regs)));
    if (status != NV_OK)
    {
        nvidia_p2p_put_rsync_registers(info);
        UNLOCK_NV_LINUX_DEVICES();
        return -ENOMEM;
    }

    for (nvl = nv_linux_devices; nvl; nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);

        addr = 0;
        size = 0;

        status = nv_get_ibmnpu_genreg_info(nv, &addr, &size, (void**)&ibmnpu);
        if (status != NV_OK)
        {
            continue;
        }

        ptr = nv_ioremap_nocache(addr, size);
        if (ptr == NULL)
        {
            continue;
        }

        regs[index].ptr = ptr;
        regs[index].size = size;
        regs[index].gpu = nvl->pci_dev;
        regs[index].ibmnpu = ibmnpu;
        regs[index].cluster_id = 0;
        regs[index].socket_id = nv_get_ibmnpu_chip_id(nv);

        index++;
    }

    UNLOCK_NV_LINUX_DEVICES();

    info->regs = regs;
    info->entries = index;

    if (info->entries == 0)
    {
        nvidia_p2p_put_rsync_registers(info);
        return -ENODEV;
    }

    *reg_info = info;

    return 0;
}

EXPORT_SYMBOL(nvidia_p2p_get_rsync_registers);

void nvidia_p2p_put_rsync_registers(
    nvidia_p2p_rsync_reg_info_t *reg_info
)
{
    NvU32 i;
    nvidia_p2p_rsync_reg_t *regs = NULL;

    if (reg_info == NULL)
    {
        return;
    }

    if (reg_info->regs)
    {
        for (i = 0; i < reg_info->entries; i++)
        {
            regs = &reg_info->regs[i];

            if (regs->ptr)
            {
                nv_iounmap(regs->ptr, regs->size);
            }
        }

        os_free_mem(reg_info->regs);
    }

    os_free_mem(reg_info);
}

EXPORT_SYMBOL(nvidia_p2p_put_rsync_registers);
