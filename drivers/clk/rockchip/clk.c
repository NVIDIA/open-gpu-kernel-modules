// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2014 MundoReader S.L.
 * Author: Heiko Stuebner <heiko@sntech.de>
 *
 * Copyright (c) 2016 Rockchip Electronics Co. Ltd.
 * Author: Xing Zheng <zhengxing@rock-chips.com>
 *
 * based on
 *
 * samsung/clk.c
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 * Copyright (c) 2013 Linaro Ltd.
 * Author: Thomas Abraham <thomas.ab@samsung.com>
 */

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/reboot.h>
#include <linux/rational.h>
#include "clk.h"

/*
 * Register a clock branch.
 * Most clock branches have a form like
 *
 * src1 --|--\
 *        |M |--[GATE]-[DIV]-
 * src2 --|--/
 *
 * sometimes without one of those components.
 */
static struct clk *rockchip_clk_register_branch(const char *name,
		const char *const *parent_names, u8 num_parents,
		void __iomem *base,
		int muxdiv_offset, u8 mux_shift, u8 mux_width, u8 mux_flags,
		int div_offset, u8 div_shift, u8 div_width, u8 div_flags,
		struct clk_div_table *div_table, int gate_offset,
		u8 gate_shift, u8 gate_flags, unsigned long flags,
		spinlock_t *lock)
{
	struct clk_hw *hw;
	struct clk_mux *mux = NULL;
	struct clk_gate *gate = NULL;
	struct clk_divider *div = NULL;
	const struct clk_ops *mux_ops = NULL, *div_ops = NULL,
			     *gate_ops = NULL;
	int ret;

	if (num_parents > 1) {
		mux = kzalloc(sizeof(*mux), GFP_KERNEL);
		if (!mux)
			return ERR_PTR(-ENOMEM);

		mux->reg = base + muxdiv_offset;
		mux->shift = mux_shift;
		mux->mask = BIT(mux_width) - 1;
		mux->flags = mux_flags;
		mux->lock = lock;
		mux_ops = (mux_flags & CLK_MUX_READ_ONLY) ? &clk_mux_ro_ops
							: &clk_mux_ops;
	}

	if (gate_offset >= 0) {
		gate = kzalloc(sizeof(*gate), GFP_KERNEL);
		if (!gate) {
			ret = -ENOMEM;
			goto err_gate;
		}

		gate->flags = gate_flags;
		gate->reg = base + gate_offset;
		gate->bit_idx = gate_shift;
		gate->lock = lock;
		gate_ops = &clk_gate_ops;
	}

	if (div_width > 0) {
		div = kzalloc(sizeof(*div), GFP_KERNEL);
		if (!div) {
			ret = -ENOMEM;
			goto err_div;
		}

		div->flags = div_flags;
		if (div_offset)
			div->reg = base + div_offset;
		else
			div->reg = base + muxdiv_offset;
		div->shift = div_shift;
		div->width = div_width;
		div->lock = lock;
		div->table = div_table;
		div_ops = (div_flags & CLK_DIVIDER_READ_ONLY)
						? &clk_divider_ro_ops
						: &clk_divider_ops;
	}

	hw = clk_hw_register_composite(NULL, name, parent_names, num_parents,
				       mux ? &mux->hw : NULL, mux_ops,
				       div ? &div->hw : NULL, div_ops,
				       gate ? &gate->hw : NULL, gate_ops,
				       flags);
	if (IS_ERR(hw)) {
		kfree(div);
		kfree(gate);
		return ERR_CAST(hw);
	}

	return hw->clk;
err_div:
	kfree(gate);
err_gate:
	kfree(mux);
	return ERR_PTR(ret);
}

struct rockchip_clk_frac {
	struct notifier_block			clk_nb;
	struct clk_fractional_divider		div;
	struct clk_gate				gate;

	struct clk_mux				mux;
	const struct clk_ops			*mux_ops;
	int					mux_frac_idx;

	bool					rate_change_remuxed;
	int					rate_change_idx;
};

#define to_rockchip_clk_frac_nb(nb) \
			container_of(nb, struct rockchip_clk_frac, clk_nb)

static int rockchip_clk_frac_notifier_cb(struct notifier_block *nb,
					 unsigned long event, void *data)
{
	struct clk_notifier_data *ndata = data;
	struct rockchip_clk_frac *frac = to_rockchip_clk_frac_nb(nb);
	struct clk_mux *frac_mux = &frac->mux;
	int ret = 0;

	pr_debug("%s: event %lu, old_rate %lu, new_rate: %lu\n",
		 __func__, event, ndata->old_rate, ndata->new_rate);
	if (event == PRE_RATE_CHANGE) {
		frac->rate_change_idx =
				frac->mux_ops->get_parent(&frac_mux->hw);
		if (frac->rate_change_idx != frac->mux_frac_idx) {
			frac->mux_ops->set_parent(&frac_mux->hw,
						  frac->mux_frac_idx);
			frac->rate_change_remuxed = 1;
		}
	} else if (event == POST_RATE_CHANGE) {
		/*
		 * The POST_RATE_CHANGE notifier runs directly after the
		 * divider clock is set in clk_change_rate, so we'll have
		 * remuxed back to the original parent before clk_change_rate
		 * reaches the mux itself.
		 */
		if (frac->rate_change_remuxed) {
			frac->mux_ops->set_parent(&frac_mux->hw,
						  frac->rate_change_idx);
			frac->rate_change_remuxed = 0;
		}
	}

	return notifier_from_errno(ret);
}

/*
 * fractional divider must set that denominator is 20 times larger than
 * numerator to generate precise clock frequency.
 */
static void rockchip_fractional_approximation(struct clk_hw *hw,
		unsigned long rate, unsigned long *parent_rate,
		unsigned long *m, unsigned long *n)
{
	struct clk_fractional_divider *fd = to_clk_fd(hw);
	unsigned long p_rate, p_parent_rate;
	struct clk_hw *p_parent;
	unsigned long scale;

	p_rate = clk_hw_get_rate(clk_hw_get_parent(hw));
	if ((rate * 20 > p_rate) && (p_rate % rate != 0)) {
		p_parent = clk_hw_get_parent(clk_hw_get_parent(hw));
		p_parent_rate = clk_hw_get_rate(p_parent);
		*parent_rate = p_parent_rate;
	}

	/*
	 * Get rate closer to *parent_rate to guarantee there is no overflow
	 * for m and n. In the result it will be the nearest rate left shifted
	 * by (scale - fd->nwidth) bits.
	 */
	scale = fls_long(*parent_rate / rate - 1);
	if (scale > fd->nwidth)
		rate <<= scale - fd->nwidth;

	rational_best_approximation(rate, *parent_rate,
			GENMASK(fd->mwidth - 1, 0), GENMASK(fd->nwidth - 1, 0),
			m, n);
}

static struct clk *rockchip_clk_register_frac_branch(
		struct rockchip_clk_provider *ctx, const char *name,
		const char *const *parent_names, u8 num_parents,
		void __iomem *base, int muxdiv_offset, u8 div_flags,
		int gate_offset, u8 gate_shift, u8 gate_flags,
		unsigned long flags, struct rockchip_clk_branch *child,
		spinlock_t *lock)
{
	struct clk_hw *hw;
	struct rockchip_clk_frac *frac;
	struct clk_gate *gate = NULL;
	struct clk_fractional_divider *div = NULL;
	const struct clk_ops *div_ops = NULL, *gate_ops = NULL;

	if (muxdiv_offset < 0)
		return ERR_PTR(-EINVAL);

	if (child && child->branch_type != branch_mux) {
		pr_err("%s: fractional child clock for %s can only be a mux\n",
		       __func__, name);
		return ERR_PTR(-EINVAL);
	}

	frac = kzalloc(sizeof(*frac), GFP_KERNEL);
	if (!frac)
		return ERR_PTR(-ENOMEM);

	if (gate_offset >= 0) {
		gate = &frac->gate;
		gate->flags = gate_flags;
		gate->reg = base + gate_offset;
		gate->bit_idx = gate_shift;
		gate->lock = lock;
		gate_ops = &clk_gate_ops;
	}

	div = &frac->div;
	div->flags = div_flags;
	div->reg = base + muxdiv_offset;
	div->mshift = 16;
	div->mwidth = 16;
	div->mmask = GENMASK(div->mwidth - 1, 0) << div->mshift;
	div->nshift = 0;
	div->nwidth = 16;
	div->nmask = GENMASK(div->nwidth - 1, 0) << div->nshift;
	div->lock = lock;
	div->approximation = rockchip_fractional_approximation;
	div_ops = &clk_fractional_divider_ops;

	hw = clk_hw_register_composite(NULL, name, parent_names, num_parents,
				       NULL, NULL,
				       &div->hw, div_ops,
				       gate ? &gate->hw : NULL, gate_ops,
				       flags | CLK_SET_RATE_UNGATE);
	if (IS_ERR(hw)) {
		kfree(frac);
		return ERR_CAST(hw);
	}

	if (child) {
		struct clk_mux *frac_mux = &frac->mux;
		struct clk_init_data init;
		struct clk *mux_clk;
		int ret;

		frac->mux_frac_idx = match_string(child->parent_names,
						  child->num_parents, name);
		frac->mux_ops = &clk_mux_ops;
		frac->clk_nb.notifier_call = rockchip_clk_frac_notifier_cb;

		frac_mux->reg = base + child->muxdiv_offset;
		frac_mux->shift = child->mux_shift;
		frac_mux->mask = BIT(child->mux_width) - 1;
		frac_mux->flags = child->mux_flags;
		frac_mux->lock = lock;
		frac_mux->hw.init = &init;

		init.name = child->name;
		init.flags = child->flags | CLK_SET_RATE_PARENT;
		init.ops = frac->mux_ops;
		init.parent_names = child->parent_names;
		init.num_parents = child->num_parents;

		mux_clk = clk_register(NULL, &frac_mux->hw);
		if (IS_ERR(mux_clk)) {
			kfree(frac);
			return mux_clk;
		}

		rockchip_clk_add_lookup(ctx, mux_clk, child->id);

		/* notifier on the fraction divider to catch rate changes */
		if (frac->mux_frac_idx >= 0) {
			pr_debug("%s: found fractional parent in mux at pos %d\n",
				 __func__, frac->mux_frac_idx);
			ret = clk_notifier_register(hw->clk, &frac->clk_nb);
			if (ret)
				pr_err("%s: failed to register clock notifier for %s\n",
						__func__, name);
		} else {
			pr_warn("%s: could not find %s as parent of %s, rate changes may not work\n",
				__func__, name, child->name);
		}
	}

	return hw->clk;
}

static struct clk *rockchip_clk_register_factor_branch(const char *name,
		const char *const *parent_names, u8 num_parents,
		void __iomem *base, unsigned int mult, unsigned int div,
		int gate_offset, u8 gate_shift, u8 gate_flags,
		unsigned long flags, spinlock_t *lock)
{
	struct clk_hw *hw;
	struct clk_gate *gate = NULL;
	struct clk_fixed_factor *fix = NULL;

	/* without gate, register a simple factor clock */
	if (gate_offset == 0) {
		return clk_register_fixed_factor(NULL, name,
				parent_names[0], flags, mult,
				div);
	}

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return ERR_PTR(-ENOMEM);

	gate->flags = gate_flags;
	gate->reg = base + gate_offset;
	gate->bit_idx = gate_shift;
	gate->lock = lock;

	fix = kzalloc(sizeof(*fix), GFP_KERNEL);
	if (!fix) {
		kfree(gate);
		return ERR_PTR(-ENOMEM);
	}

	fix->mult = mult;
	fix->div = div;

	hw = clk_hw_register_composite(NULL, name, parent_names, num_parents,
				       NULL, NULL,
				       &fix->hw, &clk_fixed_factor_ops,
				       &gate->hw, &clk_gate_ops, flags);
	if (IS_ERR(hw)) {
		kfree(fix);
		kfree(gate);
		return ERR_CAST(hw);
	}

	return hw->clk;
}

struct rockchip_clk_provider *rockchip_clk_init(struct device_node *np,
						void __iomem *base,
						unsigned long nr_clks)
{
	struct rockchip_clk_provider *ctx;
	struct clk **clk_table;
	int i;

	ctx = kzalloc(sizeof(struct rockchip_clk_provider), GFP_KERNEL);
	if (!ctx)
		return ERR_PTR(-ENOMEM);

	clk_table = kcalloc(nr_clks, sizeof(struct clk *), GFP_KERNEL);
	if (!clk_table)
		goto err_free;

	for (i = 0; i < nr_clks; ++i)
		clk_table[i] = ERR_PTR(-ENOENT);

	ctx->reg_base = base;
	ctx->clk_data.clks = clk_table;
	ctx->clk_data.clk_num = nr_clks;
	ctx->cru_node = np;
	spin_lock_init(&ctx->lock);

	ctx->grf = syscon_regmap_lookup_by_phandle(ctx->cru_node,
						   "rockchip,grf");

	return ctx;

err_free:
	kfree(ctx);
	return ERR_PTR(-ENOMEM);
}
EXPORT_SYMBOL_GPL(rockchip_clk_init);

void rockchip_clk_of_add_provider(struct device_node *np,
				  struct rockchip_clk_provider *ctx)
{
	if (of_clk_add_provider(np, of_clk_src_onecell_get,
				&ctx->clk_data))
		pr_err("%s: could not register clk provider\n", __func__);
}
EXPORT_SYMBOL_GPL(rockchip_clk_of_add_provider);

void rockchip_clk_add_lookup(struct rockchip_clk_provider *ctx,
			     struct clk *clk, unsigned int id)
{
	if (ctx->clk_data.clks && id)
		ctx->clk_data.clks[id] = clk;
}
EXPORT_SYMBOL_GPL(rockchip_clk_add_lookup);

void rockchip_clk_register_plls(struct rockchip_clk_provider *ctx,
				struct rockchip_pll_clock *list,
				unsigned int nr_pll, int grf_lock_offset)
{
	struct clk *clk;
	int idx;

	for (idx = 0; idx < nr_pll; idx++, list++) {
		clk = rockchip_clk_register_pll(ctx, list->type, list->name,
				list->parent_names, list->num_parents,
				list->con_offset, grf_lock_offset,
				list->lock_shift, list->mode_offset,
				list->mode_shift, list->rate_table,
				list->flags, list->pll_flags);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to register clock %s\n", __func__,
				list->name);
			continue;
		}

		rockchip_clk_add_lookup(ctx, clk, list->id);
	}
}
EXPORT_SYMBOL_GPL(rockchip_clk_register_plls);

void rockchip_clk_register_branches(struct rockchip_clk_provider *ctx,
				    struct rockchip_clk_branch *list,
				    unsigned int nr_clk)
{
	struct clk *clk = NULL;
	unsigned int idx;
	unsigned long flags;

	for (idx = 0; idx < nr_clk; idx++, list++) {
		flags = list->flags;

		/* catch simple muxes */
		switch (list->branch_type) {
		case branch_mux:
			clk = clk_register_mux(NULL, list->name,
				list->parent_names, list->num_parents,
				flags, ctx->reg_base + list->muxdiv_offset,
				list->mux_shift, list->mux_width,
				list->mux_flags, &ctx->lock);
			break;
		case branch_muxgrf:
			clk = rockchip_clk_register_muxgrf(list->name,
				list->parent_names, list->num_parents,
				flags, ctx->grf, list->muxdiv_offset,
				list->mux_shift, list->mux_width,
				list->mux_flags);
			break;
		case branch_divider:
			if (list->div_table)
				clk = clk_register_divider_table(NULL,
					list->name, list->parent_names[0],
					flags,
					ctx->reg_base + list->muxdiv_offset,
					list->div_shift, list->div_width,
					list->div_flags, list->div_table,
					&ctx->lock);
			else
				clk = clk_register_divider(NULL, list->name,
					list->parent_names[0], flags,
					ctx->reg_base + list->muxdiv_offset,
					list->div_shift, list->div_width,
					list->div_flags, &ctx->lock);
			break;
		case branch_fraction_divider:
			clk = rockchip_clk_register_frac_branch(ctx, list->name,
				list->parent_names, list->num_parents,
				ctx->reg_base, list->muxdiv_offset,
				list->div_flags,
				list->gate_offset, list->gate_shift,
				list->gate_flags, flags, list->child,
				&ctx->lock);
			break;
		case branch_half_divider:
			clk = rockchip_clk_register_halfdiv(list->name,
				list->parent_names, list->num_parents,
				ctx->reg_base, list->muxdiv_offset,
				list->mux_shift, list->mux_width,
				list->mux_flags, list->div_shift,
				list->div_width, list->div_flags,
				list->gate_offset, list->gate_shift,
				list->gate_flags, flags, &ctx->lock);
			break;
		case branch_gate:
			flags |= CLK_SET_RATE_PARENT;

			clk = clk_register_gate(NULL, list->name,
				list->parent_names[0], flags,
				ctx->reg_base + list->gate_offset,
				list->gate_shift, list->gate_flags, &ctx->lock);
			break;
		case branch_composite:
			clk = rockchip_clk_register_branch(list->name,
				list->parent_names, list->num_parents,
				ctx->reg_base, list->muxdiv_offset,
				list->mux_shift,
				list->mux_width, list->mux_flags,
				list->div_offset, list->div_shift, list->div_width,
				list->div_flags, list->div_table,
				list->gate_offset, list->gate_shift,
				list->gate_flags, flags, &ctx->lock);
			break;
		case branch_mmc:
			clk = rockchip_clk_register_mmc(
				list->name,
				list->parent_names, list->num_parents,
				ctx->reg_base + list->muxdiv_offset,
				list->div_shift
			);
			break;
		case branch_inverter:
			clk = rockchip_clk_register_inverter(
				list->name, list->parent_names,
				list->num_parents,
				ctx->reg_base + list->muxdiv_offset,
				list->div_shift, list->div_flags, &ctx->lock);
			break;
		case branch_factor:
			clk = rockchip_clk_register_factor_branch(
				list->name, list->parent_names,
				list->num_parents, ctx->reg_base,
				list->div_shift, list->div_width,
				list->gate_offset, list->gate_shift,
				list->gate_flags, flags, &ctx->lock);
			break;
		case branch_ddrclk:
			clk = rockchip_clk_register_ddrclk(
				list->name, list->flags,
				list->parent_names, list->num_parents,
				list->muxdiv_offset, list->mux_shift,
				list->mux_width, list->div_shift,
				list->div_width, list->div_flags,
				ctx->reg_base, &ctx->lock);
			break;
		}

		/* none of the cases above matched */
		if (!clk) {
			pr_err("%s: unknown clock type %d\n",
			       __func__, list->branch_type);
			continue;
		}

		if (IS_ERR(clk)) {
			pr_err("%s: failed to register clock %s: %ld\n",
			       __func__, list->name, PTR_ERR(clk));
			continue;
		}

		rockchip_clk_add_lookup(ctx, clk, list->id);
	}
}
EXPORT_SYMBOL_GPL(rockchip_clk_register_branches);

void rockchip_clk_register_armclk(struct rockchip_clk_provider *ctx,
				  unsigned int lookup_id,
				  const char *name, const char *const *parent_names,
				  u8 num_parents,
				  const struct rockchip_cpuclk_reg_data *reg_data,
				  const struct rockchip_cpuclk_rate_table *rates,
				  int nrates)
{
	struct clk *clk;

	clk = rockchip_clk_register_cpuclk(name, parent_names, num_parents,
					   reg_data, rates, nrates,
					   ctx->reg_base, &ctx->lock);
	if (IS_ERR(clk)) {
		pr_err("%s: failed to register clock %s: %ld\n",
		       __func__, name, PTR_ERR(clk));
		return;
	}

	rockchip_clk_add_lookup(ctx, clk, lookup_id);
}
EXPORT_SYMBOL_GPL(rockchip_clk_register_armclk);

void rockchip_clk_protect_critical(const char *const clocks[],
				   int nclocks)
{
	int i;

	/* Protect the clocks that needs to stay on */
	for (i = 0; i < nclocks; i++) {
		struct clk *clk = __clk_lookup(clocks[i]);

		clk_prepare_enable(clk);
	}
}
EXPORT_SYMBOL_GPL(rockchip_clk_protect_critical);

static void __iomem *rst_base;
static unsigned int reg_restart;
static void (*cb_restart)(void);
static int rockchip_restart_notify(struct notifier_block *this,
				   unsigned long mode, void *cmd)
{
	if (cb_restart)
		cb_restart();

	writel(0xfdb9, rst_base + reg_restart);
	return NOTIFY_DONE;
}

static struct notifier_block rockchip_restart_handler = {
	.notifier_call = rockchip_restart_notify,
	.priority = 128,
};

void
rockchip_register_restart_notifier(struct rockchip_clk_provider *ctx,
				   unsigned int reg,
				   void (*cb)(void))
{
	int ret;

	rst_base = ctx->reg_base;
	reg_restart = reg;
	cb_restart = cb;
	ret = register_restart_handler(&rockchip_restart_handler);
	if (ret)
		pr_err("%s: cannot register restart handler, %d\n",
		       __func__, ret);
}
EXPORT_SYMBOL_GPL(rockchip_register_restart_notifier);
