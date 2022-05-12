/*
 * Copyright 2014 Advanced Micro Devices, Inc.
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
 *
 */

#ifndef KFD_IPC_H_
#define KFD_IPC_H_

#include <linux/types.h>
#include <linux/dma-buf.h>

/* avoid including kfd_priv.h */
struct kfd_dev;
struct kfd_process;

struct kfd_ipc_obj {
	struct hlist_node node;
	struct kref ref;
	struct dma_buf *dmabuf;
	uint32_t share_handle[4];
	uint32_t flags;
};

int kfd_ipc_import_handle(struct kfd_dev *dev, struct kfd_process *p,
			  uint32_t gpu_id, uint32_t *share_handle,
			  uint64_t va_addr, uint64_t *handle,
			  uint64_t *mmap_offset, uint32_t *pflags);
int kfd_ipc_import_dmabuf(struct kfd_dev *kfd, struct kfd_process *p,
			  uint32_t gpu_id, int dmabuf_fd,
			  uint64_t va_addr, uint64_t *handle,
			  uint64_t *mmap_offset);
int kfd_ipc_export_as_handle(struct kfd_dev *dev, struct kfd_process *p,
			     uint64_t handle, uint32_t *ipc_handle,
			     uint32_t flags);

int kfd_ipc_store_insert(struct dma_buf *dmabuf, struct kfd_ipc_obj **ipc_obj,
			 uint32_t flags);
void kfd_ipc_obj_put(struct kfd_ipc_obj **obj);

#endif /* KFD_IPC_H_ */
