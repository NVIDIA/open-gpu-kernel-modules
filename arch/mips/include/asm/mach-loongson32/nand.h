/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2015 Zhang, Keguang <keguang.zhang@gmail.com>
 *
 * Loongson 1 NAND platform support.
 */

#ifndef __ASM_MACH_LOONGSON32_NAND_H
#define __ASM_MACH_LOONGSON32_NAND_H

#include <linux/dmaengine.h>
#include <linux/mtd/partitions.h>

struct plat_ls1x_nand {
	struct mtd_partition *parts;
	unsigned int nr_parts;

	int hold_cycle;
	int wait_cycle;
};

extern struct plat_ls1x_nand ls1b_nand_pdata;

bool ls1x_dma_filter_fn(struct dma_chan *chan, void *param);

#endif /* __ASM_MACH_LOONGSON32_NAND_H */
