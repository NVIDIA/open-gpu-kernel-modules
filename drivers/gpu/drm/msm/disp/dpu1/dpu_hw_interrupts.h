/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 */

#ifndef _DPU_HW_INTERRUPTS_H
#define _DPU_HW_INTERRUPTS_H

#include <linux/types.h>

#include "dpu_hwio.h"
#include "dpu_hw_catalog.h"
#include "dpu_hw_util.h"
#include "dpu_hw_mdss.h"

/* When making changes be sure to sync with dpu_intr_set */
enum dpu_hw_intr_reg {
	MDP_SSPP_TOP0_INTR,
	MDP_SSPP_TOP0_INTR2,
	MDP_SSPP_TOP0_HIST_INTR,
	MDP_INTF0_INTR,
	MDP_INTF1_INTR,
	MDP_INTF2_INTR,
	MDP_INTF3_INTR,
	MDP_INTF4_INTR,
	MDP_AD4_0_INTR,
	MDP_AD4_1_INTR,
	MDP_INTF0_7xxx_INTR,
	MDP_INTF1_7xxx_INTR,
	MDP_INTF5_7xxx_INTR,
	MDP_INTR_MAX,
};

#define DPU_IRQ_IDX(reg_idx, offset)	(reg_idx * 32 + offset)

struct dpu_hw_intr;

/**
 * Interrupt operations.
 */
struct dpu_hw_intr_ops {

	/**
	 * enable_irq - Enable IRQ based on lookup IRQ index
	 * @intr:	HW interrupt handle
	 * @irq_idx:	Lookup irq index return from irq_idx_lookup
	 * @return:	0 for success, otherwise failure
	 */
	int (*enable_irq_locked)(
			struct dpu_hw_intr *intr,
			int irq_idx);

	/**
	 * disable_irq - Disable IRQ based on lookup IRQ index
	 * @intr:	HW interrupt handle
	 * @irq_idx:	Lookup irq index return from irq_idx_lookup
	 * @return:	0 for success, otherwise failure
	 */
	int (*disable_irq_locked)(
			struct dpu_hw_intr *intr,
			int irq_idx);

	/**
	 * clear_all_irqs - Clears all the interrupts (i.e. acknowledges
	 *                  any asserted IRQs). Useful during reset.
	 * @intr:	HW interrupt handle
	 * @return:	0 for success, otherwise failure
	 */
	int (*clear_all_irqs)(
			struct dpu_hw_intr *intr);

	/**
	 * disable_all_irqs - Disables all the interrupts. Useful during reset.
	 * @intr:	HW interrupt handle
	 * @return:	0 for success, otherwise failure
	 */
	int (*disable_all_irqs)(
			struct dpu_hw_intr *intr);

	/**
	 * dispatch_irqs - IRQ dispatcher will call the given callback
	 *                 function when a matching interrupt status bit is
	 *                 found in the irq mapping table.
	 * @intr:	HW interrupt handle
	 * @cbfunc:	Callback function pointer
	 * @arg:	Argument to pass back during callback
	 */
	void (*dispatch_irqs)(
			struct dpu_hw_intr *intr,
			void (*cbfunc)(void *arg, int irq_idx),
			void *arg);

	/**
	 * get_interrupt_status - Gets HW interrupt status, and clear if set,
	 *                        based on given lookup IRQ index.
	 * @intr:	HW interrupt handle
	 * @irq_idx:	Lookup irq index return from irq_idx_lookup
	 * @clear:	True to clear irq after read
	 */
	u32 (*get_interrupt_status)(
			struct dpu_hw_intr *intr,
			int irq_idx,
			bool clear);

	/**
	 * lock - take the IRQ lock
	 * @intr:	HW interrupt handle
	 * @return:	irq_flags for the taken spinlock
	 */
	unsigned long (*lock)(
			struct dpu_hw_intr *intr);

	/**
	 * unlock - take the IRQ lock
	 * @intr:	HW interrupt handle
	 * @irq_flags:  the irq_flags returned from lock
	 */
	void (*unlock)(
			struct dpu_hw_intr *intr, unsigned long irq_flags);
};

/**
 * struct dpu_hw_intr: hw interrupts handling data structure
 * @hw:               virtual address mapping
 * @ops:              function pointer mapping for IRQ handling
 * @cache_irq_mask:   array of IRQ enable masks reg storage created during init
 * @save_irq_status:  array of IRQ status reg storage created during init
 * @total_irqs: total number of irq_idx mapped in the hw_interrupts
 * @irq_lock:         spinlock for accessing IRQ resources
 */
struct dpu_hw_intr {
	struct dpu_hw_blk_reg_map hw;
	struct dpu_hw_intr_ops ops;
	u32 *cache_irq_mask;
	u32 *save_irq_status;
	u32 total_irqs;
	spinlock_t irq_lock;
	unsigned long irq_mask;
};

/**
 * dpu_hw_intr_init(): Initializes the interrupts hw object
 * @addr: mapped register io address of MDP
 * @m :   pointer to mdss catalog data
 */
struct dpu_hw_intr *dpu_hw_intr_init(void __iomem *addr,
		struct dpu_mdss_cfg *m);

/**
 * dpu_hw_intr_destroy(): Cleanup interrutps hw object
 * @intr: pointer to interrupts hw object
 */
void dpu_hw_intr_destroy(struct dpu_hw_intr *intr);
#endif
