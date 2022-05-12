// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
 */

#include <linux/clk-provider.h>
#include <linux/slab.h>
#include <linux/err.h>

#include "clk.h"

static unsigned long clk_sync_source_recalc_rate(struct clk_hw *hw,
						 unsigned long parent_rate)
{
	struct tegra_clk_sync_source *sync = to_clk_sync_source(hw);

	return sync->rate;
}

static long clk_sync_source_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *prate)
{
	struct tegra_clk_sync_source *sync = to_clk_sync_source(hw);

	if (rate > sync->max_rate)
		return -EINVAL;
	else
		return rate;
}

static int clk_sync_source_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct tegra_clk_sync_source *sync = to_clk_sync_source(hw);

	sync->rate = rate;
	return 0;
}

const struct clk_ops tegra_clk_sync_source_ops = {
	.round_rate = clk_sync_source_round_rate,
	.set_rate = clk_sync_source_set_rate,
	.recalc_rate = clk_sync_source_recalc_rate,
};

struct clk *tegra_clk_register_sync_source(const char *name,
					   unsigned long max_rate)
{
	struct tegra_clk_sync_source *sync;
	struct clk_init_data init;
	struct clk *clk;

	sync = kzalloc(sizeof(*sync), GFP_KERNEL);
	if (!sync) {
		pr_err("%s: could not allocate sync source clk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	sync->max_rate = max_rate;

	init.ops = &tegra_clk_sync_source_ops;
	init.name = name;
	init.flags = 0;
	init.parent_names = NULL;
	init.num_parents = 0;

	/* Data in .init is copied by clk_register(), so stack variable OK */
	sync->hw.init = &init;

	clk = clk_register(NULL, &sync->hw);
	if (IS_ERR(clk))
		kfree(sync);

	return clk;
}
