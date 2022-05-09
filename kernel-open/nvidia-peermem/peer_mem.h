/* SPDX-License-Identifier: Linux-OpenIB */
/*
 * Copyright (c) 2014-2020,  Mellanox Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *  - Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 *  - Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
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
#ifndef RDMA_PEER_MEM_H
#define RDMA_PEER_MEM_H

#include <linux/scatterlist.h>

#define IB_PEER_MEMORY_NAME_MAX 64
#define IB_PEER_MEMORY_VER_MAX 16

/*
 * Prior versions used a void * for core_context, at some point this was
 * switched to use u64. Be careful if compiling this as 32 bit. To help the
 * value of core_context is limited to u32 so it should work OK despite the
 * type change.
 */
#define PEER_MEM_U64_CORE_CONTEXT

struct device;

/**
 *  struct peer_memory_client - registration information for user virtual
 *                              memory handlers
 *
 * The peer_memory_client scheme allows a driver to register with the ib_umem
 * system that it has the ability to understand user virtual address ranges
 * that are not compatible with get_user_pages(). For instance VMAs created
 * with io_remap_pfn_range(), or other driver special VMA.
 *
 * For ranges the interface understands it can provide a DMA mapped sg_table
 * for use by the ib_umem, allowing user virtual ranges that cannot be
 * supported by get_user_pages() to be used as umems.
 */
struct peer_memory_client {
    char name[IB_PEER_MEMORY_NAME_MAX];
    char version[IB_PEER_MEMORY_VER_MAX];

    /**
     * acquire - Begin working with a user space virtual address range
     *
     * @addr - Virtual address to be checked whether belongs to peer.
     * @size - Length of the virtual memory area starting at addr.
     * @peer_mem_private_data - Obsolete, always NULL
     * @peer_mem_name - Obsolete, always NULL
     * @client_context - Returns an opaque value for this acquire use in
     *                   other APIs
     *
     * Returns 1 if the peer_memory_client supports the entire virtual
     * address range, 0 or -ERRNO otherwise.  If 1 is returned then
     * release() will be called to release the acquire().
     */
    int (*acquire)(unsigned long addr, size_t size,
               void *peer_mem_private_data, char *peer_mem_name,
               void **client_context);
    /**
     * get_pages - Fill in the first part of a sg_table for a virtual
     *             address range
     *
     * @addr - Virtual address to be checked whether belongs to peer.
     * @size - Length of the virtual memory area starting at addr.
     * @write - Always 1
     * @force - 1 if write is required
     * @sg_head - Obsolete, always NULL
     * @client_context - Value returned by acquire()
     * @core_context - Value to be passed to invalidate_peer_memory for
     *                 this get
     *
     * addr/size are passed as the raw virtual address range requested by
     * the user, it is not aligned to any page size. get_pages() is always
     * followed by dma_map().
     *
     * Upon return the caller can call the invalidate_callback().
     *
     * Returns 0 on success, -ERRNO on failure. After success put_pages()
     * will be called to return the pages.
     */
    int (*get_pages)(unsigned long addr, size_t size, int write, int force,
             struct sg_table *sg_head, void *client_context,
             u64 core_context);
    /**
     * dma_map - Create a DMA mapped sg_table
     *
     * @sg_head - The sg_table to allocate
     * @client_context - Value returned by acquire()
     * @dma_device - The device that will be doing DMA from these addresses
     * @dmasync - Obsolete, always 0
     * @nmap - Returns the number of dma mapped entries in the sg_head
     *
     * Must be called after get_pages(). This must fill in the sg_head with
     * DMA mapped SGLs for dma_device. Each SGL start and end must meet a
     * minimum alignment of at least PAGE_SIZE, though individual sgls can
     * be multiples of PAGE_SIZE, in any mixture. Since the user virtual
     * address/size are not page aligned, the implementation must increase
     * it to the logical alignment when building the SGLs.
     *
     * Returns 0 on success, -ERRNO on failure. After success dma_unmap()
     * will be called to unmap the pages. On failure sg_head must be left
     * untouched or point to a valid sg_table.
     */
    int (*dma_map)(struct sg_table *sg_head, void *client_context,
               struct device *dma_device, int dmasync, int *nmap);
    /**
     * dma_unmap - Unmap a DMA mapped sg_table
     *
     * @sg_head - The sg_table to unmap
     * @client_context - Value returned by acquire()
     * @dma_device - The device that will be doing DMA from these addresses
     *
     * sg_head will not be touched after this function returns.
     *
     * Must return 0.
     */
    int (*dma_unmap)(struct sg_table *sg_head, void *client_context,
             struct device *dma_device);
    /**
     * put_pages - Unpin a SGL
     *
     * @sg_head - The sg_table to unpin
     * @client_context - Value returned by acquire()
     *
     * sg_head must be freed on return.
     */
    void (*put_pages)(struct sg_table *sg_head, void *client_context);
    /* Obsolete, not used */
    unsigned long (*get_page_size)(void *client_context);
    /**
     * release - Undo acquire
     *
     * @client_context - Value returned by acquire()
     *
     * If acquire() returns 1 then release() must be called. All
     * get_pages() and dma_map()'s must be undone before calling this
     * function.
     */
    void (*release)(void *client_context);
};

enum {
        PEER_MEM_INVALIDATE_UNMAPS = 1 << 0,
};

struct peer_memory_client_ex {
        struct peer_memory_client client;
        size_t ex_size;
        u32 flags;
};

/*
 * If invalidate_callback() is non-NULL then the client will only support
 * umems which can be invalidated. The caller may call the
 * invalidate_callback() after acquire() on return the range will no longer
 * have DMA active, and release() will have been called.
 *
 * Note: The implementation locking must ensure that get_pages(), and
 * dma_map() do not have locking dependencies with invalidate_callback(). The
 * ib_core will wait until any concurrent get_pages() or dma_map() completes
 * before returning.
 *
 * Similarly, this can call dma_unmap(), put_pages() and release() from within
 * the callback, or will wait for another thread doing those operations to
 * complete.
 *
 * For these reasons the user of invalidate_callback() must be careful with
 * locking.
 */
typedef int (*invalidate_peer_memory)(void *reg_handle, u64 core_context);

void *
ib_register_peer_memory_client(const struct peer_memory_client *peer_client,
                   invalidate_peer_memory *invalidate_callback);
void ib_unregister_peer_memory_client(void *reg_handle);

#endif
