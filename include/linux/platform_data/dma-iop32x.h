/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright © 2006, Intel Corporation.
 */
#ifndef IOP_ADMA_H
#define IOP_ADMA_H
#include <linux/types.h>
#include <linux/dmaengine.h>
#include <linux/interrupt.h>

#define IOP_ADMA_SLOT_SIZE 32
#define IOP_ADMA_THRESHOLD 4
#ifdef DEBUG
#define IOP_PARANOIA 1
#else
#define IOP_PARANOIA 0
#endif
#define iop_paranoia(x) BUG_ON(IOP_PARANOIA && (x))

#define DMA0_ID 0
#define DMA1_ID 1
#define AAU_ID 2

/**
 * struct iop_adma_device - internal representation of an ADMA device
 * @pdev: Platform device
 * @id: HW ADMA Device selector
 * @dma_desc_pool: base of DMA descriptor region (DMA address)
 * @dma_desc_pool_virt: base of DMA descriptor region (CPU address)
 * @common: embedded struct dma_device
 */
struct iop_adma_device {
	struct platform_device *pdev;
	int id;
	dma_addr_t dma_desc_pool;
	void *dma_desc_pool_virt;
	struct dma_device common;
};

/**
 * struct iop_adma_chan - internal representation of an ADMA device
 * @pending: allows batching of hardware operations
 * @lock: serializes enqueue/dequeue operations to the slot pool
 * @mmr_base: memory mapped register base
 * @chain: device chain view of the descriptors
 * @device: parent device
 * @common: common dmaengine channel object members
 * @last_used: place holder for allocation to continue from where it left off
 * @all_slots: complete domain of slots usable by the channel
 * @slots_allocated: records the actual size of the descriptor slot pool
 * @irq_tasklet: bottom half where iop_adma_slot_cleanup runs
 */
struct iop_adma_chan {
	int pending;
	spinlock_t lock; /* protects the descriptor slot pool */
	void __iomem *mmr_base;
	struct list_head chain;
	struct iop_adma_device *device;
	struct dma_chan common;
	struct iop_adma_desc_slot *last_used;
	struct list_head all_slots;
	int slots_allocated;
	struct tasklet_struct irq_tasklet;
};

/**
 * struct iop_adma_desc_slot - IOP-ADMA software descriptor
 * @slot_node: node on the iop_adma_chan.all_slots list
 * @chain_node: node on the op_adma_chan.chain list
 * @hw_desc: virtual address of the hardware descriptor chain
 * @phys: hardware address of the hardware descriptor chain
 * @group_head: first operation in a transaction
 * @slot_cnt: total slots used in an transaction (group of operations)
 * @slots_per_op: number of slots per operation
 * @idx: pool index
 * @tx_list: list of descriptors that are associated with one operation
 * @async_tx: support for the async_tx api
 * @group_list: list of slots that make up a multi-descriptor transaction
 *	for example transfer lengths larger than the supported hw max
 * @xor_check_result: result of zero sum
 * @crc32_result: result crc calculation
 */
struct iop_adma_desc_slot {
	struct list_head slot_node;
	struct list_head chain_node;
	void *hw_desc;
	struct iop_adma_desc_slot *group_head;
	u16 slot_cnt;
	u16 slots_per_op;
	u16 idx;
	struct list_head tx_list;
	struct dma_async_tx_descriptor async_tx;
	union {
		u32 *xor_check_result;
		u32 *crc32_result;
		u32 *pq_check_result;
	};
};

struct iop_adma_platform_data {
	int hw_id;
	dma_cap_mask_t cap_mask;
	size_t pool_size;
};

#define to_iop_sw_desc(addr_hw_desc) \
	container_of(addr_hw_desc, struct iop_adma_desc_slot, hw_desc)
#define iop_hw_desc_slot_idx(hw_desc, idx) \
	( (void *) (((unsigned long) hw_desc) + ((idx) << 5)) )
#endif
