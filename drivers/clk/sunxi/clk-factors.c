// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 Emilio López <emilio@elopez.com.ar>
 *
 * Adjustable factor-based clock implementation
 */

#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/string.h>

#include "clk-factors.h"

/*
 * DOC: basic adjustable factor-based clock
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is adjustable.
 *        clk->rate = (parent->rate * N * (K + 1) >> P) / (M + 1)
 * parent - fixed parent.  No clk_set_parent support
 */

#define to_clk_factors(_hw) container_of(_hw, struct clk_factors, hw)

#define FACTORS_MAX_PARENTS		5

#define SETMASK(len, pos)		(((1U << (len)) - 1) << (pos))
#define CLRMASK(len, pos)		(~(SETMASK(len, pos)))
#define FACTOR_GET(bit, len, reg)	(((reg) & SETMASK(len, bit)) >> (bit))

#define FACTOR_SET(bit, len, reg, val) \
	(((reg) & CLRMASK(len, bit)) | (val << (bit)))

static unsigned long clk_factors_recalc_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	u8 n = 1, k = 0, p = 0, m = 0;
	u32 reg;
	unsigned long rate;
	struct clk_factors *factors = to_clk_factors(hw);
	const struct clk_factors_config *config = factors->config;

	/* Fetch the register value */
	reg = readl(factors->reg);

	/* Get each individual factor if applicable */
	if (config->nwidth != SUNXI_FACTORS_NOT_APPLICABLE)
		n = FACTOR_GET(config->nshift, config->nwidth, reg);
	if (config->kwidth != SUNXI_FACTORS_NOT_APPLICABLE)
		k = FACTOR_GET(config->kshift, config->kwidth, reg);
	if (config->mwidth != SUNXI_FACTORS_NOT_APPLICABLE)
		m = FACTOR_GET(config->mshift, config->mwidth, reg);
	if (config->pwidth != SUNXI_FACTORS_NOT_APPLICABLE)
		p = FACTOR_GET(config->pshift, config->pwidth, reg);

	if (factors->recalc) {
		struct factors_request factors_req = {
			.parent_rate = parent_rate,
			.n = n,
			.k = k,
			.m = m,
			.p = p,
		};

		/* get mux details from mux clk structure */
		if (factors->mux)
			factors_req.parent_index =
				(reg >> factors->mux->shift) &
				factors->mux->mask;

		factors->recalc(&factors_req);

		return factors_req.rate;
	}

	/* Calculate the rate */
	rate = (parent_rate * (n + config->n_start) * (k + 1) >> p) / (m + 1);

	return rate;
}

static int clk_factors_determine_rate(struct clk_hw *hw,
				      struct clk_rate_request *req)
{
	struct clk_factors *factors = to_clk_factors(hw);
	struct clk_hw *parent, *best_parent = NULL;
	int i, num_parents;
	unsigned long parent_rate, best = 0, child_rate, best_child_rate = 0;

	/* find the parent that can help provide the fastest rate <= rate */
	num_parents = clk_hw_get_num_parents(hw);
	for (i = 0; i < num_parents; i++) {
		struct factors_request factors_req = {
			.rate = req->rate,
			.parent_index = i,
		};
		parent = clk_hw_get_parent_by_index(hw, i);
		if (!parent)
			continue;
		if (clk_hw_get_flags(hw) & CLK_SET_RATE_PARENT)
			parent_rate = clk_hw_round_rate(parent, req->rate);
		else
			parent_rate = clk_hw_get_rate(parent);

		factors_req.parent_rate = parent_rate;
		factors->get_factors(&factors_req);
		child_rate = factors_req.rate;

		if (child_rate <= req->rate && child_rate > best_child_rate) {
			best_parent = parent;
			best = parent_rate;
			best_child_rate = child_rate;
		}
	}

	if (!best_parent)
		return -EINVAL;

	req->best_parent_hw = best_parent;
	req->best_parent_rate = best;
	req->rate = best_child_rate;

	return 0;
}

static int clk_factors_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct factors_request req = {
		.rate = rate,
		.parent_rate = parent_rate,
	};
	u32 reg;
	struct clk_factors *factors = to_clk_factors(hw);
	const struct clk_factors_config *config = factors->config;
	unsigned long flags = 0;

	factors->get_factors(&req);

	if (factors->lock)
		spin_lock_irqsave(factors->lock, flags);

	/* Fetch the register value */
	reg = readl(factors->reg);

	/* Set up the new factors - macros do not do anything if width is 0 */
	reg = FACTOR_SET(config->nshift, config->nwidth, reg, req.n);
	reg = FACTOR_SET(config->kshift, config->kwidth, reg, req.k);
	reg = FACTOR_SET(config->mshift, config->mwidth, reg, req.m);
	reg = FACTOR_SET(config->pshift, config->pwidth, reg, req.p);

	/* Apply them now */
	writel(reg, factors->reg);

	/* delay 500us so pll stabilizes */
	__delay((rate >> 20) * 500 / 2);

	if (factors->lock)
		spin_unlock_irqrestore(factors->lock, flags);

	return 0;
}

static const struct clk_ops clk_factors_ops = {
	.determine_rate = clk_factors_determine_rate,
	.recalc_rate = clk_factors_recalc_rate,
	.set_rate = clk_factors_set_rate,
};

static struct clk *__sunxi_factors_register(struct device_node *node,
					    const struct factors_data *data,
					    spinlock_t *lock, void __iomem *reg,
					    unsigned long flags)
{
	struct clk *clk;
	struct clk_factors *factors;
	struct clk_gate *gate = NULL;
	struct clk_mux *mux = NULL;
	struct clk_hw *gate_hw = NULL;
	struct clk_hw *mux_hw = NULL;
	const char *clk_name = node->name;
	const char *parents[FACTORS_MAX_PARENTS];
	int ret, i = 0;

	/* if we have a mux, we will have >1 parents */
	i = of_clk_parent_fill(node, parents, FACTORS_MAX_PARENTS);

	/*
	 * some factor clocks, such as pll5 and pll6, may have multiple
	 * outputs, and have their name designated in factors_data
	 */
	if (data->name)
		clk_name = data->name;
	else
		of_property_read_string(node, "clock-output-names", &clk_name);

	factors = kzalloc(sizeof(struct clk_factors), GFP_KERNEL);
	if (!factors)
		goto err_factors;

	/* set up factors properties */
	factors->reg = reg;
	factors->config = data->table;
	factors->get_factors = data->getter;
	factors->recalc = data->recalc;
	factors->lock = lock;

	/* Add a gate if this factor clock can be gated */
	if (data->enable) {
		gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
		if (!gate)
			goto err_gate;

		factors->gate = gate;

		/* set up gate properties */
		gate->reg = reg;
		gate->bit_idx = data->enable;
		gate->lock = factors->lock;
		gate_hw = &gate->hw;
	}

	/* Add a mux if this factor clock can be muxed */
	if (data->mux) {
		mux = kzalloc(sizeof(struct clk_mux), GFP_KERNEL);
		if (!mux)
			goto err_mux;

		factors->mux = mux;

		/* set up gate properties */
		mux->reg = reg;
		mux->shift = data->mux;
		mux->mask = data->muxmask;
		mux->lock = factors->lock;
		mux_hw = &mux->hw;
	}

	clk = clk_register_composite(NULL, clk_name,
			parents, i,
			mux_hw, &clk_mux_ops,
			&factors->hw, &clk_factors_ops,
			gate_hw, &clk_gate_ops, CLK_IS_CRITICAL);
	if (IS_ERR(clk))
		goto err_register;

	ret = of_clk_add_provider(node, of_clk_src_simple_get, clk);
	if (ret)
		goto err_provider;

	return clk;

err_provider:
	/* TODO: The composite clock stuff will leak a bit here. */
	clk_unregister(clk);
err_register:
	kfree(mux);
err_mux:
	kfree(gate);
err_gate:
	kfree(factors);
err_factors:
	return NULL;
}

struct clk *sunxi_factors_register(struct device_node *node,
				   const struct factors_data *data,
				   spinlock_t *lock,
				   void __iomem *reg)
{
	return __sunxi_factors_register(node, data, lock, reg, 0);
}

struct clk *sunxi_factors_register_critical(struct device_node *node,
					    const struct factors_data *data,
					    spinlock_t *lock,
					    void __iomem *reg)
{
	return __sunxi_factors_register(node, data, lock, reg, CLK_IS_CRITICAL);
}

void sunxi_factors_unregister(struct device_node *node, struct clk *clk)
{
	struct clk_hw *hw = __clk_get_hw(clk);
	struct clk_factors *factors;

	if (!hw)
		return;

	factors = to_clk_factors(hw);

	of_clk_del_provider(node);
	/* TODO: The composite clock stuff will leak a bit here. */
	clk_unregister(clk);
	kfree(factors->mux);
	kfree(factors->gate);
	kfree(factors);
}
