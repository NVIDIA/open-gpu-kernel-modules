// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 Linaro Ltd.
 * Author: Pi-Cheng Chen <pi-cheng.chen@linaro.org>
 */

#include <linux/clk-provider.h>
#include <linux/mfd/syscon.h>
#include <linux/slab.h>

#include "clk-mtk.h"
#include "clk-cpumux.h"

static inline struct mtk_clk_cpumux *to_mtk_clk_cpumux(struct clk_hw *_hw)
{
	return container_of(_hw, struct mtk_clk_cpumux, hw);
}

static u8 clk_cpumux_get_parent(struct clk_hw *hw)
{
	struct mtk_clk_cpumux *mux = to_mtk_clk_cpumux(hw);
	unsigned int val;

	regmap_read(mux->regmap, mux->reg, &val);

	val >>= mux->shift;
	val &= mux->mask;

	return val;
}

static int clk_cpumux_set_parent(struct clk_hw *hw, u8 index)
{
	struct mtk_clk_cpumux *mux = to_mtk_clk_cpumux(hw);
	u32 mask, val;

	val = index << mux->shift;
	mask = mux->mask << mux->shift;

	return regmap_update_bits(mux->regmap, mux->reg, mask, val);
}

static const struct clk_ops clk_cpumux_ops = {
	.get_parent = clk_cpumux_get_parent,
	.set_parent = clk_cpumux_set_parent,
};

static struct clk *
mtk_clk_register_cpumux(const struct mtk_composite *mux,
			struct regmap *regmap)
{
	struct mtk_clk_cpumux *cpumux;
	struct clk *clk;
	struct clk_init_data init;

	cpumux = kzalloc(sizeof(*cpumux), GFP_KERNEL);
	if (!cpumux)
		return ERR_PTR(-ENOMEM);

	init.name = mux->name;
	init.ops = &clk_cpumux_ops;
	init.parent_names = mux->parent_names;
	init.num_parents = mux->num_parents;
	init.flags = mux->flags;

	cpumux->reg = mux->mux_reg;
	cpumux->shift = mux->mux_shift;
	cpumux->mask = BIT(mux->mux_width) - 1;
	cpumux->regmap = regmap;
	cpumux->hw.init = &init;

	clk = clk_register(NULL, &cpumux->hw);
	if (IS_ERR(clk))
		kfree(cpumux);

	return clk;
}

int mtk_clk_register_cpumuxes(struct device_node *node,
			      const struct mtk_composite *clks, int num,
			      struct clk_onecell_data *clk_data)
{
	int i;
	struct clk *clk;
	struct regmap *regmap;

	regmap = syscon_node_to_regmap(node);
	if (IS_ERR(regmap)) {
		pr_err("Cannot find regmap for %pOF: %ld\n", node,
		       PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	for (i = 0; i < num; i++) {
		const struct mtk_composite *mux = &clks[i];

		clk = mtk_clk_register_cpumux(mux, regmap);
		if (IS_ERR(clk)) {
			pr_err("Failed to register clk %s: %ld\n",
			       mux->name, PTR_ERR(clk));
			continue;
		}

		clk_data->clks[mux->id] = clk;
	}

	return 0;
}
