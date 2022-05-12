/*
 * Copyright (c) 2016 Hisilicon Limited.
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

#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include "hns_roce_device.h"
#include <rdma/ib_umem.h>

int hns_roce_bitmap_alloc(struct hns_roce_bitmap *bitmap, unsigned long *obj)
{
	int ret = 0;

	spin_lock(&bitmap->lock);
	*obj = find_next_zero_bit(bitmap->table, bitmap->max, bitmap->last);
	if (*obj >= bitmap->max) {
		bitmap->top = (bitmap->top + bitmap->max + bitmap->reserved_top)
			       & bitmap->mask;
		*obj = find_first_zero_bit(bitmap->table, bitmap->max);
	}

	if (*obj < bitmap->max) {
		set_bit(*obj, bitmap->table);
		bitmap->last = (*obj + 1);
		if (bitmap->last == bitmap->max)
			bitmap->last = 0;
		*obj |= bitmap->top;
	} else {
		ret = -EINVAL;
	}

	spin_unlock(&bitmap->lock);

	return ret;
}

void hns_roce_bitmap_free(struct hns_roce_bitmap *bitmap, unsigned long obj,
			  int rr)
{
	hns_roce_bitmap_free_range(bitmap, obj, 1, rr);
}

int hns_roce_bitmap_alloc_range(struct hns_roce_bitmap *bitmap, int cnt,
				int align, unsigned long *obj)
{
	int ret = 0;
	int i;

	if (likely(cnt == 1 && align == 1))
		return hns_roce_bitmap_alloc(bitmap, obj);

	spin_lock(&bitmap->lock);

	*obj = bitmap_find_next_zero_area(bitmap->table, bitmap->max,
					  bitmap->last, cnt, align - 1);
	if (*obj >= bitmap->max) {
		bitmap->top = (bitmap->top + bitmap->max + bitmap->reserved_top)
			       & bitmap->mask;
		*obj = bitmap_find_next_zero_area(bitmap->table, bitmap->max, 0,
						  cnt, align - 1);
	}

	if (*obj < bitmap->max) {
		for (i = 0; i < cnt; i++)
			set_bit(*obj + i, bitmap->table);

		if (*obj == bitmap->last) {
			bitmap->last = (*obj + cnt);
			if (bitmap->last >= bitmap->max)
				bitmap->last = 0;
		}
		*obj |= bitmap->top;
	} else {
		ret = -EINVAL;
	}

	spin_unlock(&bitmap->lock);

	return ret;
}

void hns_roce_bitmap_free_range(struct hns_roce_bitmap *bitmap,
				unsigned long obj, int cnt,
				int rr)
{
	int i;

	obj &= bitmap->max + bitmap->reserved_top - 1;

	spin_lock(&bitmap->lock);
	for (i = 0; i < cnt; i++)
		clear_bit(obj + i, bitmap->table);

	if (!rr)
		bitmap->last = min(bitmap->last, obj);
	bitmap->top = (bitmap->top + bitmap->max + bitmap->reserved_top)
		       & bitmap->mask;
	spin_unlock(&bitmap->lock);
}

int hns_roce_bitmap_init(struct hns_roce_bitmap *bitmap, u32 num, u32 mask,
			 u32 reserved_bot, u32 reserved_top)
{
	u32 i;

	if (num != roundup_pow_of_two(num))
		return -EINVAL;

	bitmap->last = 0;
	bitmap->top = 0;
	bitmap->max = num - reserved_top;
	bitmap->mask = mask;
	bitmap->reserved_top = reserved_top;
	spin_lock_init(&bitmap->lock);
	bitmap->table = kcalloc(BITS_TO_LONGS(bitmap->max), sizeof(long),
				GFP_KERNEL);
	if (!bitmap->table)
		return -ENOMEM;

	for (i = 0; i < reserved_bot; ++i)
		set_bit(i, bitmap->table);

	return 0;
}

void hns_roce_bitmap_cleanup(struct hns_roce_bitmap *bitmap)
{
	kfree(bitmap->table);
}

void hns_roce_buf_free(struct hns_roce_dev *hr_dev, struct hns_roce_buf *buf)
{
	struct hns_roce_buf_list *trunks;
	u32 i;

	if (!buf)
		return;

	trunks = buf->trunk_list;
	if (trunks) {
		buf->trunk_list = NULL;
		for (i = 0; i < buf->ntrunks; i++)
			dma_free_coherent(hr_dev->dev, 1 << buf->trunk_shift,
					  trunks[i].buf, trunks[i].map);

		kfree(trunks);
	}

	kfree(buf);
}

/*
 * Allocate the dma buffer for storing ROCEE table entries
 *
 * @size: required size
 * @page_shift: the unit size in a continuous dma address range
 * @flags: HNS_ROCE_BUF_ flags to control the allocation flow.
 */
struct hns_roce_buf *hns_roce_buf_alloc(struct hns_roce_dev *hr_dev, u32 size,
					u32 page_shift, u32 flags)
{
	u32 trunk_size, page_size, alloced_size;
	struct hns_roce_buf_list *trunks;
	struct hns_roce_buf *buf;
	gfp_t gfp_flags;
	u32 ntrunk, i;

	/* The minimum shift of the page accessed by hw is HNS_HW_PAGE_SHIFT */
	if (WARN_ON(page_shift < HNS_HW_PAGE_SHIFT))
		return ERR_PTR(-EINVAL);

	gfp_flags = (flags & HNS_ROCE_BUF_NOSLEEP) ? GFP_ATOMIC : GFP_KERNEL;
	buf = kzalloc(sizeof(*buf), gfp_flags);
	if (!buf)
		return ERR_PTR(-ENOMEM);

	buf->page_shift = page_shift;
	page_size = 1 << buf->page_shift;

	/* Calc the trunk size and num by required size and page_shift */
	if (flags & HNS_ROCE_BUF_DIRECT) {
		buf->trunk_shift = ilog2(ALIGN(size, PAGE_SIZE));
		ntrunk = 1;
	} else {
		buf->trunk_shift = ilog2(ALIGN(page_size, PAGE_SIZE));
		ntrunk = DIV_ROUND_UP(size, 1 << buf->trunk_shift);
	}

	trunks = kcalloc(ntrunk, sizeof(*trunks), gfp_flags);
	if (!trunks) {
		kfree(buf);
		return ERR_PTR(-ENOMEM);
	}

	trunk_size = 1 << buf->trunk_shift;
	alloced_size = 0;
	for (i = 0; i < ntrunk; i++) {
		trunks[i].buf = dma_alloc_coherent(hr_dev->dev, trunk_size,
						   &trunks[i].map, gfp_flags);
		if (!trunks[i].buf)
			break;

		alloced_size += trunk_size;
	}

	buf->ntrunks = i;

	/* In nofail mode, it's only failed when the alloced size is 0 */
	if ((flags & HNS_ROCE_BUF_NOFAIL) ? i == 0 : i != ntrunk) {
		for (i = 0; i < buf->ntrunks; i++)
			dma_free_coherent(hr_dev->dev, trunk_size,
					  trunks[i].buf, trunks[i].map);

		kfree(trunks);
		kfree(buf);
		return ERR_PTR(-ENOMEM);
	}

	buf->npages = DIV_ROUND_UP(alloced_size, page_size);
	buf->trunk_list = trunks;

	return buf;
}

int hns_roce_get_kmem_bufs(struct hns_roce_dev *hr_dev, dma_addr_t *bufs,
			   int buf_cnt, int start, struct hns_roce_buf *buf)
{
	int i, end;
	int total;

	end = start + buf_cnt;
	if (end > buf->npages) {
		dev_err(hr_dev->dev,
			"failed to check kmem bufs, end %d + %d total %u!\n",
			start, buf_cnt, buf->npages);
		return -EINVAL;
	}

	total = 0;
	for (i = start; i < end; i++)
		bufs[total++] = hns_roce_buf_page(buf, i);

	return total;
}

int hns_roce_get_umem_bufs(struct hns_roce_dev *hr_dev, dma_addr_t *bufs,
			   int buf_cnt, int start, struct ib_umem *umem,
			   unsigned int page_shift)
{
	struct ib_block_iter biter;
	int total = 0;
	int idx = 0;
	u64 addr;

	if (page_shift < HNS_HW_PAGE_SHIFT) {
		dev_err(hr_dev->dev, "failed to check umem page shift %u!\n",
			page_shift);
		return -EINVAL;
	}

	/* convert system page cnt to hw page cnt */
	rdma_umem_for_each_dma_block(umem, &biter, 1 << page_shift) {
		addr = rdma_block_iter_dma_address(&biter);
		if (idx >= start) {
			bufs[total++] = addr;
			if (total >= buf_cnt)
				goto done;
		}
		idx++;
	}

done:
	return total;
}

void hns_roce_cleanup_bitmap(struct hns_roce_dev *hr_dev)
{
	if (hr_dev->caps.flags & HNS_ROCE_CAP_FLAG_XRC)
		hns_roce_cleanup_xrcd_table(hr_dev);

	if (hr_dev->caps.flags & HNS_ROCE_CAP_FLAG_SRQ)
		hns_roce_cleanup_srq_table(hr_dev);
	hns_roce_cleanup_qp_table(hr_dev);
	hns_roce_cleanup_cq_table(hr_dev);
	hns_roce_cleanup_mr_table(hr_dev);
	hns_roce_cleanup_pd_table(hr_dev);
	hns_roce_cleanup_uar_table(hr_dev);
}
