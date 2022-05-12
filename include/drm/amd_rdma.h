/* SPDX-License-Identifier: GPL-2.0 OR MIT */
/*
 * Copyright 2015-2021 Advanced Micro Devices, Inc.
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

/* @file This file defined kernel interfaces to communicate with amdkfd */

#ifndef AMD_RDMA_H_
#define AMD_RDMA_H_

/* API versions:
 * 1.0 Original API until ROCm 4.1, AMD_RDMA_MAJOR/MINOR undefined
 * 2.0 Added IOMMU (dma-mapping) support, removed p2p_info.kfd_proc
 *     Introduced AMD_RDMA_MAJOR/MINOR version definition
 */
#define AMD_RDMA_MAJOR 2
#define AMD_RDMA_MINOR 0

/**
 * Structure describing information needed to P2P access from another device
 * to specific location of GPU memory
 */
struct amd_p2p_info {
	uint64_t	va;		/**< Specify user virt. address
					  * which this page table
					  * described
					  */
	uint64_t	size;		/**< Specify total size of
					  * allocation
					  */
	struct pid	*pid;		/**< Specify process pid to which
					  * virtual address belongs
					  */
	struct sg_table *pages;		/**< Specify DMA/Bus addresses */
	void		*priv;		/**< Pointer set by AMD kernel
					  * driver
					  */
};

/**
 * Structure providing function pointers to support rdma/p2p requirements.
 * to specific location of GPU memory
 */
struct amd_rdma_interface {
	int (*get_pages)(uint64_t address, uint64_t length, struct pid *pid,
			 struct device *dma_dev,
			 struct amd_p2p_info **amd_p2p_data,
			 void  (*free_callback)(void *client_priv),
			 void  *client_priv);
	int (*put_pages)(struct amd_p2p_info **amd_p2p_data);
	int (*is_gpu_address)(uint64_t address, struct pid *pid);
	int (*get_page_size)(uint64_t address, uint64_t length, struct pid *pid,
				unsigned long *page_size);
};


int amdkfd_query_rdma_interface(const struct amd_rdma_interface **rdma);


#endif /* AMD_RDMA_H_ */
