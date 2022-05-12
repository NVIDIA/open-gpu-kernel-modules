// SPDX-License-Identifier: GPL-2.0-only
/*
 * helper functions for physically contiguous capture buffers
 *
 * The functions support hardware lacking scatter gather support
 * (i.e. the buffers must be linear in physical memory)
 *
 * Copyright (c) 2008 Magnus Damm
 *
 * Based on videobuf-vmalloc.c,
 * (c) 2007 Mauro Carvalho Chehab, <mchehab@kernel.org>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <media/videobuf-dma-contig.h>

struct videobuf_dma_contig_memory {
	u32 magic;
	void *vaddr;
	dma_addr_t dma_handle;
	unsigned long size;
};

#define MAGIC_DC_MEM 0x0733ac61
#define MAGIC_CHECK(is, should)						    \
	if (unlikely((is) != (should)))	{				    \
		pr_err("magic mismatch: %x expected %x\n", (is), (should)); \
		BUG();							    \
	}

static int __videobuf_dc_alloc(struct device *dev,
			       struct videobuf_dma_contig_memory *mem,
			       unsigned long size, gfp_t flags)
{
	mem->size = size;
	mem->vaddr = dma_alloc_coherent(dev, mem->size,
					&mem->dma_handle, flags);

	if (!mem->vaddr) {
		dev_err(dev, "memory alloc size %ld failed\n", mem->size);
		return -ENOMEM;
	}

	dev_dbg(dev, "dma mapped data is at %p (%ld)\n", mem->vaddr, mem->size);

	return 0;
}

static void __videobuf_dc_free(struct device *dev,
			       struct videobuf_dma_contig_memory *mem)
{
	dma_free_coherent(dev, mem->size, mem->vaddr, mem->dma_handle);

	mem->vaddr = NULL;
}

static void videobuf_vm_open(struct vm_area_struct *vma)
{
	struct videobuf_mapping *map = vma->vm_private_data;

	dev_dbg(map->q->dev, "vm_open %p [count=%u,vma=%08lx-%08lx]\n",
		map, map->count, vma->vm_start, vma->vm_end);

	map->count++;
}

static void videobuf_vm_close(struct vm_area_struct *vma)
{
	struct videobuf_mapping *map = vma->vm_private_data;
	struct videobuf_queue *q = map->q;
	int i;

	dev_dbg(q->dev, "vm_close %p [count=%u,vma=%08lx-%08lx]\n",
		map, map->count, vma->vm_start, vma->vm_end);

	map->count--;
	if (0 == map->count) {
		struct videobuf_dma_contig_memory *mem;

		dev_dbg(q->dev, "munmap %p q=%p\n", map, q);
		videobuf_queue_lock(q);

		/* We need first to cancel streams, before unmapping */
		if (q->streaming)
			videobuf_queue_cancel(q);

		for (i = 0; i < VIDEO_MAX_FRAME; i++) {
			if (NULL == q->bufs[i])
				continue;

			if (q->bufs[i]->map != map)
				continue;

			mem = q->bufs[i]->priv;
			if (mem) {
				/* This callback is called only if kernel has
				   allocated memory and this memory is mmapped.
				   In this case, memory should be freed,
				   in order to do memory unmap.
				 */

				MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

				/* vfree is not atomic - can't be
				   called with IRQ's disabled
				 */
				dev_dbg(q->dev, "buf[%d] freeing %p\n",
					i, mem->vaddr);

				__videobuf_dc_free(q->dev, mem);
				mem->vaddr = NULL;
			}

			q->bufs[i]->map = NULL;
			q->bufs[i]->baddr = 0;
		}

		kfree(map);

		videobuf_queue_unlock(q);
	}
}

static const struct vm_operations_struct videobuf_vm_ops = {
	.open	= videobuf_vm_open,
	.close	= videobuf_vm_close,
};

/**
 * videobuf_dma_contig_user_put() - reset pointer to user space buffer
 * @mem: per-buffer private videobuf-dma-contig data
 *
 * This function resets the user space pointer
 */
static void videobuf_dma_contig_user_put(struct videobuf_dma_contig_memory *mem)
{
	mem->dma_handle = 0;
	mem->size = 0;
}

/**
 * videobuf_dma_contig_user_get() - setup user space memory pointer
 * @mem: per-buffer private videobuf-dma-contig data
 * @vb: video buffer to map
 *
 * This function validates and sets up a pointer to user space memory.
 * Only physically contiguous pfn-mapped memory is accepted.
 *
 * Returns 0 if successful.
 */
static int videobuf_dma_contig_user_get(struct videobuf_dma_contig_memory *mem,
					struct videobuf_buffer *vb)
{
	unsigned long untagged_baddr = untagged_addr(vb->baddr);
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long prev_pfn, this_pfn;
	unsigned long pages_done, user_address;
	unsigned int offset;
	int ret;

	offset = untagged_baddr & ~PAGE_MASK;
	mem->size = PAGE_ALIGN(vb->size + offset);
	ret = -EINVAL;

	mmap_read_lock(mm);

	vma = find_vma(mm, untagged_baddr);
	if (!vma)
		goto out_up;

	if ((untagged_baddr + mem->size) > vma->vm_end)
		goto out_up;

	pages_done = 0;
	prev_pfn = 0; /* kill warning */
	user_address = untagged_baddr;

	while (pages_done < (mem->size >> PAGE_SHIFT)) {
		ret = follow_pfn(vma, user_address, &this_pfn);
		if (ret)
			break;

		if (pages_done == 0)
			mem->dma_handle = (this_pfn << PAGE_SHIFT) + offset;
		else if (this_pfn != (prev_pfn + 1))
			ret = -EFAULT;

		if (ret)
			break;

		prev_pfn = this_pfn;
		user_address += PAGE_SIZE;
		pages_done++;
	}

out_up:
	mmap_read_unlock(current->mm);

	return ret;
}

static struct videobuf_buffer *__videobuf_alloc(size_t size)
{
	struct videobuf_dma_contig_memory *mem;
	struct videobuf_buffer *vb;

	vb = kzalloc(size + sizeof(*mem), GFP_KERNEL);
	if (vb) {
		vb->priv = ((char *)vb) + size;
		mem = vb->priv;
		mem->magic = MAGIC_DC_MEM;
	}

	return vb;
}

static void *__videobuf_to_vaddr(struct videobuf_buffer *buf)
{
	struct videobuf_dma_contig_memory *mem = buf->priv;

	BUG_ON(!mem);
	MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

	return mem->vaddr;
}

static int __videobuf_iolock(struct videobuf_queue *q,
			     struct videobuf_buffer *vb,
			     struct v4l2_framebuffer *fbuf)
{
	struct videobuf_dma_contig_memory *mem = vb->priv;

	BUG_ON(!mem);
	MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

	switch (vb->memory) {
	case V4L2_MEMORY_MMAP:
		dev_dbg(q->dev, "%s memory method MMAP\n", __func__);

		/* All handling should be done by __videobuf_mmap_mapper() */
		if (!mem->vaddr) {
			dev_err(q->dev, "memory is not allocated/mmapped.\n");
			return -EINVAL;
		}
		break;
	case V4L2_MEMORY_USERPTR:
		dev_dbg(q->dev, "%s memory method USERPTR\n", __func__);

		/* handle pointer from user space */
		if (vb->baddr)
			return videobuf_dma_contig_user_get(mem, vb);

		/* allocate memory for the read() method */
		if (__videobuf_dc_alloc(q->dev, mem, PAGE_ALIGN(vb->size),
					GFP_KERNEL))
			return -ENOMEM;
		break;
	case V4L2_MEMORY_OVERLAY:
	default:
		dev_dbg(q->dev, "%s memory method OVERLAY/unknown\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int __videobuf_mmap_mapper(struct videobuf_queue *q,
				  struct videobuf_buffer *buf,
				  struct vm_area_struct *vma)
{
	struct videobuf_dma_contig_memory *mem;
	struct videobuf_mapping *map;
	int retval;

	dev_dbg(q->dev, "%s\n", __func__);

	/* create mapping + update buffer list */
	map = kzalloc(sizeof(struct videobuf_mapping), GFP_KERNEL);
	if (!map)
		return -ENOMEM;

	buf->map = map;
	map->q = q;

	buf->baddr = vma->vm_start;

	mem = buf->priv;
	BUG_ON(!mem);
	MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

	if (__videobuf_dc_alloc(q->dev, mem, PAGE_ALIGN(buf->bsize),
				GFP_KERNEL | __GFP_COMP))
		goto error;

	/* Try to remap memory */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	/* the "vm_pgoff" is just used in v4l2 to find the
	 * corresponding buffer data structure which is allocated
	 * earlier and it does not mean the offset from the physical
	 * buffer start address as usual. So set it to 0 to pass
	 * the sanity check in vm_iomap_memory().
	 */
	vma->vm_pgoff = 0;

	retval = vm_iomap_memory(vma, mem->dma_handle, mem->size);
	if (retval) {
		dev_err(q->dev, "mmap: remap failed with error %d. ",
			retval);
		dma_free_coherent(q->dev, mem->size,
				  mem->vaddr, mem->dma_handle);
		goto error;
	}

	vma->vm_ops = &videobuf_vm_ops;
	vma->vm_flags |= VM_DONTEXPAND;
	vma->vm_private_data = map;

	dev_dbg(q->dev, "mmap %p: q=%p %08lx-%08lx (%lx) pgoff %08lx buf %d\n",
		map, q, vma->vm_start, vma->vm_end,
		(long int)buf->bsize, vma->vm_pgoff, buf->i);

	videobuf_vm_open(vma);

	return 0;

error:
	kfree(map);
	return -ENOMEM;
}

static struct videobuf_qtype_ops qops = {
	.magic		= MAGIC_QTYPE_OPS,
	.alloc_vb	= __videobuf_alloc,
	.iolock		= __videobuf_iolock,
	.mmap_mapper	= __videobuf_mmap_mapper,
	.vaddr		= __videobuf_to_vaddr,
};

void videobuf_queue_dma_contig_init(struct videobuf_queue *q,
				    const struct videobuf_queue_ops *ops,
				    struct device *dev,
				    spinlock_t *irqlock,
				    enum v4l2_buf_type type,
				    enum v4l2_field field,
				    unsigned int msize,
				    void *priv,
				    struct mutex *ext_lock)
{
	videobuf_queue_core_init(q, ops, dev, irqlock, type, field, msize,
				 priv, &qops, ext_lock);
}
EXPORT_SYMBOL_GPL(videobuf_queue_dma_contig_init);

dma_addr_t videobuf_to_dma_contig(struct videobuf_buffer *buf)
{
	struct videobuf_dma_contig_memory *mem = buf->priv;

	BUG_ON(!mem);
	MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

	return mem->dma_handle;
}
EXPORT_SYMBOL_GPL(videobuf_to_dma_contig);

void videobuf_dma_contig_free(struct videobuf_queue *q,
			      struct videobuf_buffer *buf)
{
	struct videobuf_dma_contig_memory *mem = buf->priv;

	/* mmapped memory can't be freed here, otherwise mmapped region
	   would be released, while still needed. In this case, the memory
	   release should happen inside videobuf_vm_close().
	   So, it should free memory only if the memory were allocated for
	   read() operation.
	 */
	if (buf->memory != V4L2_MEMORY_USERPTR)
		return;

	if (!mem)
		return;

	MAGIC_CHECK(mem->magic, MAGIC_DC_MEM);

	/* handle user space pointer case */
	if (buf->baddr) {
		videobuf_dma_contig_user_put(mem);
		return;
	}

	/* read() method */
	if (mem->vaddr) {
		__videobuf_dc_free(q->dev, mem);
		mem->vaddr = NULL;
	}
}
EXPORT_SYMBOL_GPL(videobuf_dma_contig_free);

MODULE_DESCRIPTION("helper module to manage video4linux dma contig buffers");
MODULE_AUTHOR("Magnus Damm");
MODULE_LICENSE("GPL");
