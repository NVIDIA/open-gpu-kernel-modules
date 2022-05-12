/*
 * Copyright 2016 Advanced Micro Devices, Inc.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/* NOTE:
 *
 * This file contains logic to dynamically detect and enable PeerDirect
 * suppor. PeerDirect support is delivered e.g. as part of OFED
 * from Mellanox. Because we are not able to rely on the fact that the
 * corresponding OFED will be installed we should:
 *  - copy PeerDirect definitions locally to avoid dependency on
 *    corresponding header file
 *  - try dynamically detect address of PeerDirect function
 *    pointers.
 *
 * If dynamic detection failed then PeerDirect support should be
 * enabled using the standard PeerDirect bridge driver from:
 * https://github.com/RadeonOpenCompute/ROCnRDMA
 *
 *
 * Logic to support PeerDirect relies only on official public API to be
 * non-intrusive as much as possible.
 *
 **/

#include <linux/device.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/module.h>

#include "kfd_priv.h"

/* ----------------------- PeerDirect interface ------------------------------*/

/*
 * Copyright (c) 2013,  Mellanox Technologies. All rights reserved.
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
#define IB_PEER_MEMORY_NAME_MAX 64
#define IB_PEER_MEMORY_VER_MAX 16

struct peer_memory_client {
	char	name[IB_PEER_MEMORY_NAME_MAX];
	char	version[IB_PEER_MEMORY_VER_MAX];
	/* acquire return code: 1-mine, 0-not mine */
	int (*acquire)(unsigned long addr, size_t size,
			void *peer_mem_private_data,
					char *peer_mem_name,
					void **client_context);
	int (*get_pages)(unsigned long addr,
			  size_t size, int write, int force,
			  struct sg_table *sg_head,
			  void *client_context, void *core_context);
	int (*dma_map)(struct sg_table *sg_head, void *client_context,
			struct device *dma_device, int dmasync, int *nmap);
	int (*dma_unmap)(struct sg_table *sg_head, void *client_context,
			   struct device  *dma_device);
	void (*put_pages)(struct sg_table *sg_head, void *client_context);
	unsigned long (*get_page_size)(void *client_context);
	void (*release)(void *client_context);
	void* (*get_context_private_data)(u64 peer_id);
	void (*put_context_private_data)(void *context);
};

typedef int (*invalidate_peer_memory)(void *reg_handle,
					  void *core_context);

void *ib_register_peer_memory_client(struct peer_memory_client *peer_client,
				  invalidate_peer_memory *invalidate_callback);
void ib_unregister_peer_memory_client(void *reg_handle);


/*------------------- PeerDirect bridge driver ------------------------------*/

#define AMD_PEER_BRIDGE_DRIVER_VERSION	"1.0"
#define AMD_PEER_BRIDGE_DRIVER_NAME	"amdkfd"

static char rdma_name[] = "AMD RDMA";


static void* (*pfn_ib_register_peer_memory_client)(struct peer_memory_client
							*peer_client,
					invalidate_peer_memory
							*invalidate_callback);

static void (*pfn_ib_unregister_peer_memory_client)(void *reg_handle);

static void *ib_reg_handle;

struct amd_mem_context {
	uint64_t	va;
	uint64_t	size;
	unsigned long	offset;
	struct amdgpu_bo *bo;
	struct kfd_dev	*dev;

	struct sg_table *pages;
	struct device *dma_dev;


	/* Context received from PeerDirect call */
	void *core_context;

	pid_t pid;
	uint32_t flags;
};

/* Workaround: Mellanox peerdirect driver expects sg lists at
 * page granularity. This causes failures when an application tries
 * to register size < PAGE_SIZE or addr starts at some offset. Fix
 * it by aligning the size to page size and addr to page boundary.
 */
static void align_addr_size(unsigned long *addr, size_t *size)
{
	unsigned long end = ALIGN(*addr + *size, PAGE_SIZE);

	*addr = ALIGN_DOWN(*addr, PAGE_SIZE);
	*size = end - *addr;
}

static int amd_acquire(unsigned long addr, size_t size,
			void *peer_mem_private_data,
			char *peer_mem_name, void **client_context)
{
	struct kfd_process *p;
	struct kfd_bo *buf_obj;
	struct amd_mem_context *mem_context;

	if (peer_mem_name == rdma_name) {
		p = peer_mem_private_data;
	} else {
		p = kfd_get_process(current);
		if (!p) {
			pr_debug("Not a KFD process\n");
			return 0;
		}
	}

	align_addr_size(&addr, &size);

	mutex_lock(&p->mutex);
	buf_obj = kfd_process_find_bo_from_interval(p, addr,
			addr + size - 1);
	if (!buf_obj) {
		pr_debug("Cannot find a kfd_bo for the range\n");
		goto out_unlock;
	}

	/* Initialize context used for operation with given address */
	mem_context = kzalloc(sizeof(*mem_context), GFP_KERNEL);
	if (!mem_context)
		goto out_unlock;

	mem_context->pid = p->lead_thread->pid;

	pr_debug("addr: %#lx, size: %#lx, pid: %d\n",
		 addr, size, mem_context->pid);

	mem_context->va     = addr;
	mem_context->size   = size;
	mem_context->offset = addr - buf_obj->it.start;

	mem_context->bo = amdgpu_amdkfd_gpuvm_get_bo_ref(buf_obj->mem,
							 &mem_context->flags);
	mem_context->dev = buf_obj->dev;

	mutex_unlock(&p->mutex);

	pr_debug("Client context: 0x%p\n", mem_context);

	/* Return pointer to allocated context */
	*client_context = mem_context;

	/* Return 1 to inform that this address which will be handled
	 * by AMD GPU driver
	 */
	return 1;

out_unlock:
	mutex_unlock(&p->mutex);
	return 0;
}

static int amd_get_pages(unsigned long addr, size_t size, int write, int force,
			  struct sg_table *sg_head,
			  void *client_context, void *core_context)
{
	int ret;
	struct amd_mem_context *mem_context =
		(struct amd_mem_context *)client_context;

	align_addr_size(&addr, &size);

	pr_debug("addr: %#lx, size: %#lx, core_context: 0x%p\n",
		 addr, size, core_context);

	if (!mem_context || !mem_context->bo || !mem_context->dev) {
		pr_warn("Invalid client context");
		return -EINVAL;
	}

	pr_debug("pid: %d\n", mem_context->pid);

	if (addr != mem_context->va) {
		pr_warn("Context address (%#llx) is not the same\n",
			mem_context->va);
		return -EINVAL;
	}

	if (size != mem_context->size) {
		pr_warn("Context size (%#llx) is not the same\n",
			mem_context->size);
		return -EINVAL;
	}

	ret = amdgpu_amdkfd_gpuvm_pin_bo(mem_context->bo,
				mem_context->bo->kfd_bo->domain);
	if (ret) {
		pr_err("Pinning of buffer failed.\n");
		return ret;
	}

	/* Mark the device as active */
	kfd_inc_compute_active(mem_context->dev);

	mem_context->core_context = core_context;

	return 0;
}


static int amd_dma_map(struct sg_table *sg_head, void *client_context,
			struct device *dma_device, int dmasync, int *nmap)
{
	struct sg_table *sg_table_tmp;
	int ret;

	/*
	 * NOTE/TODO:
	 * We could have potentially three cases for real memory
	 *	location:
	 *		- all memory in the local
	 *		- all memory in the system (RAM)
	 *		- memory is spread (s/g) between local and system.
	 *
	 *	In the case of all memory in the system we could use
	 *	iommu driver to build DMA addresses but not in the case
	 *	of local memory because currently iommu driver doesn't
	 *	deal with local/device memory addresses (it requires "struct
	 *	page").
	 *
	 *	Accordingly returning assumes that iommu funcutionality
	 *	should be disabled so we can assume that sg_table already
	 *	contains DMA addresses.
	 *
	 */
	struct amd_mem_context *mem_context =
		(struct amd_mem_context *)client_context;

	pr_debug("Client context: 0x%p, sg_head: 0x%p\n",
			client_context, sg_head);

	if (!mem_context || !mem_context->bo || !mem_context->dev) {
		pr_warn("Invalid client context");
		return -EINVAL;
	}

	pr_debug("pid: %d, address: %#llx, size: %#llx\n",
			mem_context->pid,
			mem_context->va,
			mem_context->size);

	/* Build sg_table for buffer being exported, including DMA mapping */
	ret = amdgpu_amdkfd_gpuvm_get_sg_table(
		mem_context->dev->adev, mem_context->bo, mem_context->flags,
		mem_context->offset, mem_context->size,
		dma_device, DMA_BIDIRECTIONAL, &sg_table_tmp);
	if (ret) {
		pr_err("Building of sg_table failed\n");
		return ret;
	}

	/* Maintain a copy of the handle to sg_table */
	mem_context->pages = sg_table_tmp;
	mem_context->dma_dev = dma_device;

	/* Copy information about previosly allocated sg_table */
	*sg_head = *mem_context->pages;

	/* Return number of pages */
	*nmap = mem_context->pages->nents;

	return ret;
}

static int amd_dma_unmap(struct sg_table *sg_head, void *client_context,
			   struct device  *dma_device)
{
	struct amd_mem_context *mem_context =
		(struct amd_mem_context *)client_context;

	pr_debug("Client context: 0x%p, sg_table: 0x%p\n",
			client_context, sg_head);

	if (!mem_context || !mem_context->bo || !mem_context->dma_dev) {
		pr_warn("Invalid client context");
		return -EINVAL;
	}

	pr_debug("pid: %d, address: %#llx, size: %#llx\n",
			mem_context->pid,
			mem_context->va,
			mem_context->size);

	/* Release the mapped pages of buffer */
	amdgpu_amdkfd_gpuvm_put_sg_table(mem_context->bo,
					 mem_context->dma_dev,
					 DMA_BIDIRECTIONAL,
					 mem_context->pages);
	mem_context->pages = NULL;

	return 0;
}

static void amd_put_pages(struct sg_table *sg_head, void *client_context)
{
	struct amd_mem_context *mem_context =
		(struct amd_mem_context *)client_context;

	pr_debug("Client context: 0x%p, sg_head: 0x%p\n",
			client_context, sg_head);
	pr_debug("pid: %d, address: %#llx, size: %#llx\n",
			mem_context->pid,
			mem_context->va,
			mem_context->size);

	amdgpu_amdkfd_gpuvm_unpin_bo(mem_context->bo);
	kfd_dec_compute_active(mem_context->dev);
}

static unsigned long amd_get_page_size(void *client_context)
{
	return PAGE_SIZE;
}

static void amd_release(void *client_context)
{
	struct amd_mem_context *mem_context =
		(struct amd_mem_context *)client_context;

	pr_debug("Client context: 0x%p\n", client_context);
	pr_debug("pid: %d, address: %#llx, size: %#llx\n",
			mem_context->pid,
			mem_context->va,
			mem_context->size);

	amdgpu_amdkfd_gpuvm_put_bo_ref(mem_context->bo);

	kfree(mem_context);
}


static struct peer_memory_client amd_mem_client = {
	.acquire = amd_acquire,
	.get_pages = amd_get_pages,
	.dma_map = amd_dma_map,
	.dma_unmap = amd_dma_unmap,
	.put_pages = amd_put_pages,
	.get_page_size = amd_get_page_size,
	.release = amd_release,
	.get_context_private_data = NULL,
	.put_context_private_data = NULL,
};

/** Initialize PeerDirect interface with RDMA Network stack.
 *
 *  Because network stack could potentially be loaded later we check
 *  presence of PeerDirect when HSA process is created. If PeerDirect was
 *  already initialized we do nothing otherwise try to detect and register.
 */
void kfd_init_peer_direct(void)
{
	if (pfn_ib_unregister_peer_memory_client) {
		pr_debug("PeerDirect support was already initialized\n");
		return;
	}

	pr_debug("Try to initialize PeerDirect support\n");

	pfn_ib_register_peer_memory_client =
		(void *(*)(struct peer_memory_client *,
			  invalidate_peer_memory *))
		symbol_request(ib_register_peer_memory_client);

	pfn_ib_unregister_peer_memory_client = (void (*)(void *))
		symbol_request(ib_unregister_peer_memory_client);

	if (!pfn_ib_register_peer_memory_client ||
		!pfn_ib_unregister_peer_memory_client) {
		pr_debug("PeerDirect interface was not detected\n");
		/* Do cleanup */
		kfd_close_peer_direct();
		return;
	}

	strcpy(amd_mem_client.name,    AMD_PEER_BRIDGE_DRIVER_NAME);
	strcpy(amd_mem_client.version, AMD_PEER_BRIDGE_DRIVER_VERSION);

	ib_reg_handle = pfn_ib_register_peer_memory_client(&amd_mem_client, NULL);

	if (!ib_reg_handle) {
		pr_err("Cannot register peer memory client\n");
		/* Do cleanup */
		kfd_close_peer_direct();
		return;
	}

	pr_info("PeerDirect support was initialized successfully\n");
}

/**
 * Close connection with PeerDirect interface with RDMA Network stack.
 *
 */
void kfd_close_peer_direct(void)
{
	if (pfn_ib_unregister_peer_memory_client) {
		if (ib_reg_handle)
			pfn_ib_unregister_peer_memory_client(ib_reg_handle);

		symbol_put(ib_unregister_peer_memory_client);
	}

	if (pfn_ib_register_peer_memory_client)
		symbol_put(ib_register_peer_memory_client);


	/* Reset pointers to be safe */
	pfn_ib_unregister_peer_memory_client = NULL;
	pfn_ib_register_peer_memory_client   = NULL;
	ib_reg_handle = NULL;
}

/* ------------------------- AMD RDMA wrapper --------------------------------*/

#include "drm/amd_rdma.h"

struct rdma_p2p_data {
	struct amd_p2p_info p2p_info;
	void (*free_callback)(void *client_priv);
	void *client_priv;
};

/**
 * This function makes the pages underlying a range of GPU virtual memory
 * accessible for DMA operations from another PCIe device
 *
 * \param   address       - The start address in the Unified Virtual Address
 *			    space in the specified process
 * \param   length        - The length of requested mapping
 * \param   pid           - Pointer to structure pid to which address belongs.
 *			    Could be NULL for current process address space.
 * \param   p2p_data    - On return: Pointer to structure describing
 *			    underlying pages/locations
 * \param   free_callback - Pointer to callback which will be called when access
 *			    to such memory must be stopped immediately: Memory
 *			    was freed, GECC events, etc.
 *			    Client should  immediately stop any transfer
 *			    operations and returned as soon as possible.
 *			    After return all resources associated with address
 *			    will be release and no access will be allowed.
 * \param   client_priv   - Pointer to be passed as parameter on
 *			    'free_callback;
 *
 * \return  0 if operation was successful
 */
static int rdma_get_pages(uint64_t address, uint64_t length, struct pid *pid,
			  struct device *dma_dev,
			  struct amd_p2p_info **amd_p2p_data,
			  void  (*free_callback)(void *client_priv),
			  void  *client_priv)
{
	struct rdma_p2p_data *p2p_data;
	struct kfd_process *p;
	struct sg_table sg_head;
	struct amd_mem_context *mem_context;
	int nmap;
	int r;

	p2p_data = kzalloc(sizeof(*p2p_data), GFP_KERNEL);
	if (!p2p_data)
		return -ENOMEM;

	p = kfd_lookup_process_by_pid(pid);
	if (!p) {
		pr_debug("pid lookup failed\n");
		r = -ESRCH;
		goto err_lookup_process;
	}

	r = amd_acquire(address, length, p, rdma_name, (void **)&mem_context);
	kfd_unref_process(p);
	if (r == 0) {
		pr_debug("acquire failed: %d\n", r);
		goto err_acquire;
	}

	r = amd_get_pages(address, length, 1, 0, &sg_head,
			  mem_context, p2p_data);
	if (r) {
		pr_debug("get_pages failed: %d\n", r);
		goto err_get_pages;
	}

	r = amd_dma_map(&sg_head, mem_context, dma_dev, 0, &nmap);
	if (r) {
		pr_debug("dma_map failed: %d\n", r);
		goto err_dma_map;
	}


	p2p_data->free_callback = free_callback;
	p2p_data->client_priv = client_priv;
	p2p_data->p2p_info.va = address;
	p2p_data->p2p_info.size = length;
	p2p_data->p2p_info.pid = pid;
	p2p_data->p2p_info.pages = mem_context->pages;
	p2p_data->p2p_info.priv = mem_context;

	*amd_p2p_data = &p2p_data->p2p_info;

	return 0;

err_dma_map:
	amd_put_pages(&sg_head, mem_context);
err_get_pages:
	amd_release(mem_context);
err_acquire:
err_lookup_process:
	kfree(p2p_data);

	return r;
}

/**
 *
 * This function release resources previously allocated by get_pages() call.
 *
 * \param   p_p2p_data - A pointer to pointer to amd_p2p_info entries
 *			allocated by get_pages() call.
 *
 * \return  0 if operation was successful
 */
static int rdma_put_pages(struct amd_p2p_info **p_p2p_data)
{
	struct rdma_p2p_data *p2p_data =
		container_of(*p_p2p_data, struct rdma_p2p_data, p2p_info);
	int r;

	r = amd_dma_unmap(p2p_data->p2p_info.pages,
			  p2p_data->p2p_info.priv,
			  NULL);
	if (r)
		return r;
	amd_put_pages(p2p_data->p2p_info.pages,
		      p2p_data->p2p_info.priv);
	amd_release(p2p_data->p2p_info.priv);
	kfree(p2p_data);

	*p_p2p_data = NULL;

	return 0;
}

/**
 * Check if given address belongs to GPU address space.
 *
 * \param   address - Address to check
 * \param   pid     - Process to which given address belongs.
 *		      Could be NULL if current one.
 *
 * \return  0  - This is not GPU address managed by AMD driver
 *	    1  - This is GPU address managed by AMD driver
 */
static int rdma_is_gpu_address(uint64_t address, struct pid *pid)
{
	struct kfd_bo *buf_obj;
	struct kfd_process *p;

	p = kfd_lookup_process_by_pid(pid);
	if (!p) {
		pr_debug("Could not find the process\n");
		return 0;
	}

	buf_obj = kfd_process_find_bo_from_interval(p, address, address);

	kfd_unref_process(p);
	if (!buf_obj)
		return 0;

	return 1;
}

/**
 * Return the single page size to be used when building scatter/gather table
 * for given range.
 *
 * \param   address   - Address
 * \param   length    - Range length
 * \param   pid       - Process id structure. Could be NULL if current one.
 * \param   page_size - On return: Page size
 *
 * \return  0 if operation was successful
 */
static int rdma_get_page_size(uint64_t address, uint64_t length,
			      struct pid *pid, unsigned long *page_size)
{
	/*
	 * As local memory is always consecutive, we can assume the local
	 * memory page size to be arbitrary.
	 * Currently we assume the local memory page size to be the same
	 * as system memory, which is 4KB.
	 */
	*page_size = PAGE_SIZE;

	return 0;
}

/**
 * Singleton object: rdma interface function pointers
 */
static const struct amd_rdma_interface rdma_ops = {
	.get_pages = rdma_get_pages,
	.put_pages = rdma_put_pages,
	.is_gpu_address = rdma_is_gpu_address,
	.get_page_size = rdma_get_page_size
};

/**
 * amdkfd_query_rdma_interface - Return interface (function pointers table) for
 *				 rdma interface
 *
 *
 * \param interace     - OUT: Pointer to interface
 *
 * \return 0 if operation was successful.
 */
int amdkfd_query_rdma_interface(const struct amd_rdma_interface **ops)
{
	*ops  = &rdma_ops;

	return 0;
}
EXPORT_SYMBOL(amdkfd_query_rdma_interface);
