/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2017 Broadcom. All Rights Reserved.
 * The term "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
 *
 * Contact Information:
 * linux-drivers@broadcom.com
 */

#ifndef BEISCSI_H
#define BEISCSI_H

#include <linux/pci.h>
#include <linux/if_vlan.h>
#include <linux/irq_poll.h>
#define FW_VER_LEN	32
#define MCC_Q_LEN	128
#define MCC_CQ_LEN	256
#define MAX_MCC_CMD	16
/* BladeEngine Generation numbers */
#define BE_GEN2 2
#define BE_GEN3 3
#define BE_GEN4	4
struct be_dma_mem {
	void *va;
	dma_addr_t dma;
	u32 size;
};

struct be_queue_info {
	struct be_dma_mem dma_mem;
	u16 len;
	u16 entry_size;		/* Size of an element in the queue */
	u16 id;
	u16 tail, head;
	bool created;
	u16 used;		/* Number of valid elements in the queue */
};

static inline u32 MODULO(u16 val, u16 limit)
{
	WARN_ON(limit & (limit - 1));
	return val & (limit - 1);
}

static inline void index_inc(u16 *index, u16 limit)
{
	*index = MODULO((*index + 1), limit);
}

static inline void *queue_head_node(struct be_queue_info *q)
{
	return q->dma_mem.va + q->head * q->entry_size;
}

static inline void *queue_get_wrb(struct be_queue_info *q, unsigned int wrb_num)
{
	return q->dma_mem.va + wrb_num * q->entry_size;
}

static inline void *queue_tail_node(struct be_queue_info *q)
{
	return q->dma_mem.va + q->tail * q->entry_size;
}

static inline void queue_head_inc(struct be_queue_info *q)
{
	index_inc(&q->head, q->len);
}

static inline void queue_tail_inc(struct be_queue_info *q)
{
	index_inc(&q->tail, q->len);
}

/*ISCSI */

struct be_aic_obj {		/* Adaptive interrupt coalescing (AIC) info */
	unsigned long jiffies;
	u32 eq_prev;		/* Used to calculate eqe */
	u32 prev_eqd;
#define BEISCSI_EQ_DELAY_MIN	0
#define BEISCSI_EQ_DELAY_DEF	32
#define BEISCSI_EQ_DELAY_MAX	128
};

struct be_eq_obj {
	u32 cq_count;
	struct be_queue_info q;
	struct beiscsi_hba *phba;
	struct be_queue_info *cq;
	struct work_struct mcc_work; /* Work Item */
	struct irq_poll	iopoll;
};

struct be_mcc_obj {
	struct be_queue_info q;
	struct be_queue_info cq;
};

struct beiscsi_mcc_tag_state {
	unsigned long tag_state;
#define MCC_TAG_STATE_RUNNING	0
#define MCC_TAG_STATE_TIMEOUT	1
#define MCC_TAG_STATE_ASYNC	2
#define MCC_TAG_STATE_IGNORE	3
	void (*cbfn)(struct beiscsi_hba *, unsigned int);
	struct be_dma_mem tag_mem_state;
};

struct be_ctrl_info {
	u8 __iomem *csr;
	u8 __iomem *db;		/* Door Bell */
	u8 __iomem *pcicfg;	/* PCI config space */
	struct pci_dev *pdev;

	/* Mbox used for cmd request/response */
	struct mutex mbox_lock;	/* For serializing mbox cmds to BE card */
	struct be_dma_mem mbox_mem;
	/* Mbox mem is adjusted to align to 16 bytes. The allocated addr
	 * is stored for freeing purpose */
	struct be_dma_mem mbox_mem_alloced;

	/* MCC Rings */
	struct be_mcc_obj mcc_obj;
	spinlock_t mcc_lock;	/* For serializing mcc cmds to BE card */

	wait_queue_head_t mcc_wait[MAX_MCC_CMD + 1];
	unsigned int mcc_tag[MAX_MCC_CMD];
	unsigned int mcc_tag_status[MAX_MCC_CMD + 1];
	unsigned short mcc_alloc_index;
	unsigned short mcc_free_index;
	unsigned int mcc_tag_available;

	struct beiscsi_mcc_tag_state ptag_state[MAX_MCC_CMD + 1];
};

#include "be_cmds.h"

/* WRB index mask for MCC_Q_LEN queue entries */
#define MCC_Q_WRB_IDX_MASK	CQE_STATUS_WRB_MASK
#define MCC_Q_WRB_IDX_SHIFT	CQE_STATUS_WRB_SHIFT
/* TAG is from 1...MAX_MCC_CMD, MASK includes MAX_MCC_CMD */
#define MCC_Q_CMD_TAG_MASK	((MAX_MCC_CMD << 1) - 1)

#define PAGE_SHIFT_4K		12
#define PAGE_SIZE_4K		(1 << PAGE_SHIFT_4K)

/* Returns number of pages spanned by the data starting at the given addr */
#define PAGES_4K_SPANNED(_address, size)				\
		((u32)((((size_t)(_address) & (PAGE_SIZE_4K - 1)) +	\
			(size) + (PAGE_SIZE_4K - 1)) >> PAGE_SHIFT_4K))

/* Returns bit offset within a DWORD of a bitfield */
#define AMAP_BIT_OFFSET(_struct, field)					\
		(((size_t)&(((_struct *)0)->field))%32)

/* Returns the bit mask of the field that is NOT shifted into location. */
static inline u32 amap_mask(u32 bitsize)
{
	return (bitsize == 32 ? 0xFFFFFFFF : (1 << bitsize) - 1);
}

static inline void amap_set(void *ptr, u32 dw_offset, u32 mask,
					u32 offset, u32 value)
{
	u32 *dw = (u32 *) ptr + dw_offset;
	*dw &= ~(mask << offset);
	*dw |= (mask & value) << offset;
}

#define AMAP_SET_BITS(_struct, field, ptr, val)				\
		amap_set(ptr,						\
			offsetof(_struct, field)/32,			\
			amap_mask(sizeof(((_struct *)0)->field)),	\
			AMAP_BIT_OFFSET(_struct, field),		\
			val)

static inline u32 amap_get(void *ptr, u32 dw_offset, u32 mask, u32 offset)
{
	u32 *dw = ptr;
	return mask & (*(dw + dw_offset) >> offset);
}

#define AMAP_GET_BITS(_struct, field, ptr)				\
		amap_get(ptr,						\
			offsetof(_struct, field)/32,			\
			amap_mask(sizeof(((_struct *)0)->field)),	\
			AMAP_BIT_OFFSET(_struct, field))

#define be_dws_cpu_to_le(wrb, len) swap_dws(wrb, len)
#define be_dws_le_to_cpu(wrb, len) swap_dws(wrb, len)
static inline void swap_dws(void *wrb, int len)
{
#ifdef __BIG_ENDIAN
	u32 *dw = wrb;
	WARN_ON(len % 4);
	do {
		*dw = cpu_to_le32(*dw);
		dw++;
		len -= 4;
	} while (len);
#endif /* __BIG_ENDIAN */
}
#endif /* BEISCSI_H */
