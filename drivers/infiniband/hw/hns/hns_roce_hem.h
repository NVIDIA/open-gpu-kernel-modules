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

#ifndef _HNS_ROCE_HEM_H
#define _HNS_ROCE_HEM_H

#define HW_SYNC_SLEEP_TIME_INTERVAL	20
#define HW_SYNC_TIMEOUT_MSECS           (25 * HW_SYNC_SLEEP_TIME_INTERVAL)
#define BT_CMD_SYNC_SHIFT		31

enum {
	/* MAP HEM(Hardware Entry Memory) */
	HEM_TYPE_QPC = 0,
	HEM_TYPE_MTPT,
	HEM_TYPE_CQC,
	HEM_TYPE_SRQC,
	HEM_TYPE_SCCC,
	HEM_TYPE_QPC_TIMER,
	HEM_TYPE_CQC_TIMER,
	HEM_TYPE_GMV,

	 /* UNMAP HEM */
	HEM_TYPE_MTT,
	HEM_TYPE_CQE,
	HEM_TYPE_SRQWQE,
	HEM_TYPE_IDX,
	HEM_TYPE_IRRL,
	HEM_TYPE_TRRL,
};

#define HNS_ROCE_HEM_CHUNK_LEN	\
	 ((256 - sizeof(struct list_head) - 2 * sizeof(int)) /	 \
	 (sizeof(struct scatterlist) + sizeof(void *)))

#define check_whether_bt_num_3(type, hop_num) \
	(type < HEM_TYPE_MTT && hop_num == 2)

#define check_whether_bt_num_2(type, hop_num) \
	((type < HEM_TYPE_MTT && hop_num == 1) || \
	(type >= HEM_TYPE_MTT && hop_num == 2))

#define check_whether_bt_num_1(type, hop_num) \
	((type < HEM_TYPE_MTT && hop_num == HNS_ROCE_HOP_NUM_0) || \
	(type >= HEM_TYPE_MTT && hop_num == 1) || \
	(type >= HEM_TYPE_MTT && hop_num == HNS_ROCE_HOP_NUM_0))

enum {
	 HNS_ROCE_HEM_PAGE_SHIFT = 12,
	 HNS_ROCE_HEM_PAGE_SIZE  = 1 << HNS_ROCE_HEM_PAGE_SHIFT,
};

struct hns_roce_hem_chunk {
	struct list_head	 list;
	int			 npages;
	int			 nsg;
	struct scatterlist	 mem[HNS_ROCE_HEM_CHUNK_LEN];
	void			 *buf[HNS_ROCE_HEM_CHUNK_LEN];
};

struct hns_roce_hem {
	struct list_head	 chunk_list;
	int			 refcount;
};

struct hns_roce_hem_iter {
	struct hns_roce_hem		 *hem;
	struct hns_roce_hem_chunk	 *chunk;
	int				 page_idx;
};

struct hns_roce_hem_mhop {
	u32	hop_num;
	u32	buf_chunk_size;
	u32	bt_chunk_size;
	u32	ba_l0_num;
	u32	l0_idx; /* level 0 base address table index */
	u32	l1_idx; /* level 1 base address table index */
	u32	l2_idx; /* level 2 base address table index */
};

void hns_roce_free_hem(struct hns_roce_dev *hr_dev, struct hns_roce_hem *hem);
int hns_roce_table_get(struct hns_roce_dev *hr_dev,
		       struct hns_roce_hem_table *table, unsigned long obj);
void hns_roce_table_put(struct hns_roce_dev *hr_dev,
			struct hns_roce_hem_table *table, unsigned long obj);
void *hns_roce_table_find(struct hns_roce_dev *hr_dev,
			  struct hns_roce_hem_table *table, unsigned long obj,
			  dma_addr_t *dma_handle);
int hns_roce_init_hem_table(struct hns_roce_dev *hr_dev,
			    struct hns_roce_hem_table *table, u32 type,
			    unsigned long obj_size, unsigned long nobj,
			    int use_lowmem);
void hns_roce_cleanup_hem_table(struct hns_roce_dev *hr_dev,
				struct hns_roce_hem_table *table);
void hns_roce_cleanup_hem(struct hns_roce_dev *hr_dev);
int hns_roce_calc_hem_mhop(struct hns_roce_dev *hr_dev,
			   struct hns_roce_hem_table *table, unsigned long *obj,
			   struct hns_roce_hem_mhop *mhop);
bool hns_roce_check_whether_mhop(struct hns_roce_dev *hr_dev, u32 type);

void hns_roce_hem_list_init(struct hns_roce_hem_list *hem_list);
int hns_roce_hem_list_calc_root_ba(const struct hns_roce_buf_region *regions,
				   int region_cnt, int unit);
int hns_roce_hem_list_request(struct hns_roce_dev *hr_dev,
			      struct hns_roce_hem_list *hem_list,
			      const struct hns_roce_buf_region *regions,
			      int region_cnt, unsigned int bt_pg_shift);
void hns_roce_hem_list_release(struct hns_roce_dev *hr_dev,
			       struct hns_roce_hem_list *hem_list);
void *hns_roce_hem_list_find_mtt(struct hns_roce_dev *hr_dev,
				 struct hns_roce_hem_list *hem_list,
				 int offset, int *mtt_cnt, u64 *phy_addr);

static inline void hns_roce_hem_first(struct hns_roce_hem *hem,
				      struct hns_roce_hem_iter *iter)
{
	iter->hem = hem;
	iter->chunk = list_empty(&hem->chunk_list) ? NULL :
				 list_entry(hem->chunk_list.next,
					    struct hns_roce_hem_chunk, list);
	iter->page_idx = 0;
}

static inline int hns_roce_hem_last(struct hns_roce_hem_iter *iter)
{
	return !iter->chunk;
}

static inline void hns_roce_hem_next(struct hns_roce_hem_iter *iter)
{
	if (++iter->page_idx >= iter->chunk->nsg) {
		if (iter->chunk->list.next == &iter->hem->chunk_list) {
			iter->chunk = NULL;
			return;
		}

		iter->chunk = list_entry(iter->chunk->list.next,
					 struct hns_roce_hem_chunk, list);
		iter->page_idx = 0;
	}
}

static inline dma_addr_t hns_roce_hem_addr(struct hns_roce_hem_iter *iter)
{
	return sg_dma_address(&iter->chunk->mem[iter->page_idx]);
}

#endif /* _HNS_ROCE_HEM_H */
