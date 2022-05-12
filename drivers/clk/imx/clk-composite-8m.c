// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2018 NXP
 */

#include <linux/clk-provider.h>
#include <linux/errno.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/slab.h>

#include "clk.h"

#define PCG_PREDIV_SHIFT	16
#define PCG_PREDIV_WIDTH	3
#define PCG_PREDIV_MAX		8

#define PCG_DIV_SHIFT		0
#define PCG_CORE_DIV_WIDTH	3
#define PCG_DIV_WIDTH		6
#define PCG_DIV_MAX		64

#define PCG_PCS_SHIFT		24
#define PCG_PCS_MASK		0x7

#define PCG_CGC_SHIFT		28

static unsigned long imx8m_clk_composite_divider_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	unsigned long prediv_rate;
	unsigned int prediv_value;
	unsigned int div_value;

	prediv_value = readl(divider->reg) >> divider->shift;
	prediv_value &= clk_div_mask(divider->width);

	prediv_rate = divider_recalc_rate(hw, parent_rate, prediv_value,
						NULL, divider->flags,
						divider->width);

	div_value = readl(divider->reg) >> PCG_DIV_SHIFT;
	div_value &= clk_div_mask(PCG_DIV_WIDTH);

	return divider_recalc_rate(hw, prediv_rate, div_value, NULL,
				   divider->flags, PCG_DIV_WIDTH);
}

static int imx8m_clk_composite_compute_dividers(unsigned long rate,
						unsigned long parent_rate,
						int *prediv, int *postdiv)
{
	int div1, div2;
	int error = INT_MAX;
	int ret = -EINVAL;

	*prediv = 1;
	*postdiv = 1;

	for (div1 = 1; div1 <= PCG_PREDIV_MAX; div1++) {
		for (div2 = 1; div2 <= PCG_DIV_MAX; div2++) {
			int new_error = ((parent_rate / div1) / div2) - rate;

			if (abs(new_error) < abs(error)) {
				*prediv = div1;
				*postdiv = div2;
				error = new_error;
				ret = 0;
			}
		}
	}
	return ret;
}

static long imx8m_clk_composite_divider_round_rate(struct clk_hw *hw,
						unsigned long rate,
						unsigned long *prate)
{
	int prediv_value;
	int div_value;

	imx8m_clk_composite_compute_dividers(rate, *prate,
						&prediv_value, &div_value);
	rate = DIV_ROUND_UP(*prate, prediv_value);

	return DIV_ROUND_UP(rate, div_value);

}

static int imx8m_clk_composite_divider_set_rate(struct clk_hw *hw,
					unsigned long rate,
					unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	unsigned long flags;
	int prediv_value;
	int div_value;
	int ret;
	u32 val;

	ret = imx8m_clk_composite_compute_dividers(rate, parent_rate,
						&prediv_value, &div_value);
	if (ret)
		return -EINVAL;

	spin_lock_irqsave(divider->lock, flags);

	val = readl(divider->reg);
	val &= ~((clk_div_mask(divider->width) << divider->shift) |
			(clk_div_mask(PCG_DIV_WIDTH) << PCG_DIV_SHIFT));

	val |= (u32)(prediv_value  - 1) << divider->shift;
	val |= (u32)(div_value - 1) << PCG_DIV_SHIFT;
	writel(val, divider->reg);

	spin_unlock_irqrestore(divider->lock, flags);

	return ret;
}

static const struct clk_ops imx8m_clk_composite_divider_ops = {
	.recalc_rate = imx8m_clk_composite_divider_recalc_rate,
	.round_rate = imx8m_clk_composite_divider_round_rate,
	.set_rate = imx8m_clk_composite_divider_set_rate,
};

static u8 imx8m_clk_composite_mux_get_parent(struct clk_hw *hw)
{
	return clk_mux_ops.get_parent(hw);
}

static int imx8m_clk_composite_mux_set_parent(struct clk_hw *hw, u8 index)
{
	struct clk_mux *mux = to_clk_mux(hw);
	u32 val = clk_mux_index_to_val(mux->table, mux->flags, index);
	unsigned long flags = 0;
	u32 reg;

	if (mux->lock)
		spin_lock_irqsave(mux->lock, flags);

	reg = readl(mux->reg);
	reg &= ~(mux->mask << mux->shift);
	val = val << mux->shift;
	reg |= val;
	/*
	 * write twice to make sure non-target interface
	 * SEL_A/B point the same clk input.
	 */
	writel(reg, mux->reg);
	writel(reg, mux->reg);

	if (mux->lock)
		spin_unlock_irqrestore(mux->lock, flags);

	return 0;
}

static int
imx8m_clk_composite_mux_determine_rate(struct clk_hw *hw,
				       struct clk_rate_request *req)
{
	return clk_mux_ops.determine_rate(hw, req);
}


static const struct clk_ops imx8m_clk_composite_mux_ops = {
	.get_parent = imx8m_clk_composite_mux_get_parent,
	.set_parent = imx8m_clk_composite_mux_set_parent,
	.determine_rate = imx8m_clk_composite_mux_determine_rate,
};

struct clk_hw *imx8m_clk_hw_composite_flags(const char *name,
					const char * const *parent_names,
					int num_parents, void __iomem *reg,
					u32 composite_flags,
					unsigned long flags)
{
	struct clk_hw *hw = ERR_PTR(-ENOMEM), *mux_hw;
	struct clk_hw *div_hw, *gate_hw;
	struct clk_divider *div = NULL;
	struct clk_gate *gate = NULL;
	struct clk_mux *mux = NULL;
	const struct clk_ops *divider_ops;
	const struct clk_ops *mux_ops;

	mux = kzalloc(sizeof(*mux), GFP_KERNEL);
	if (!mux)
		goto fail;

	mux_hw = &mux->hw;
	mux->reg = reg;
	mux->shift = PCG_PCS_SHIFT;
	mux->mask = PCG_PCS_MASK;
	mux->lock = &imx_ccm_lock;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		goto fail;

	div_hw = &div->hw;
	div->reg = reg;
	if (composite_flags & IMX_COMPOSITE_CORE) {
		div->shift = PCG_DIV_SHIFT;
		div->width = PCG_CORE_DIV_WIDTH;
		divider_ops = &clk_divider_ops;
		mux_ops = &imx8m_clk_composite_mux_ops;
	} else if (composite_flags & IMX_COMPOSITE_BUS) {
		div->shift = PCG_PREDIV_SHIFT;
		div->width = PCG_PREDIV_WIDTH;
		divider_ops = &imx8m_clk_composite_divider_ops;
		mux_ops = &imx8m_clk_composite_mux_ops;
	} else {
		div->shift = PCG_PREDIV_SHIFT;
		div->width = PCG_PREDIV_WIDTH;
		divider_ops = &imx8m_clk_composite_divider_ops;
		mux_ops = &clk_mux_ops;
		flags |= CLK_SET_PARENT_GATE;
	}

	div->lock = &imx_ccm_lock;
	div->flags = CLK_DIVIDER_ROUND_CLOSEST;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		goto fail;

	gate_hw = &gate->hw;
	gate->reg = reg;
	gate->bit_idx = PCG_CGC_SHIFT;
	gate->lock = &imx_ccm_lock;

	hw = clk_hw_register_composite(NULL, name, parent_names, num_parents,
			mux_hw, mux_ops, div_hw,
			divider_ops, gate_hw, &clk_gate_ops, flags);
	if (IS_ERR(hw))
		goto fail;

	return hw;

fail:
	kfree(gate);
	kfree(div);
	kfree(mux);
	return ERR_CAST(hw);
}
EXPORT_SYMBOL_GPL(imx8m_clk_hw_composite_flags);
