// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * clk-h32mx.c
 *
 *  Copyright (C) 2014 Atmel
 *
 * Alexandre Belloni <alexandre.belloni@free-electrons.com>
 */

#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

#include "pmc.h"

#define H32MX_MAX_FREQ	90000000

struct clk_sama5d4_h32mx {
	struct clk_hw hw;
	struct regmap *regmap;
};

#define to_clk_sama5d4_h32mx(hw) container_of(hw, struct clk_sama5d4_h32mx, hw)

static unsigned long clk_sama5d4_h32mx_recalc_rate(struct clk_hw *hw,
						 unsigned long parent_rate)
{
	struct clk_sama5d4_h32mx *h32mxclk = to_clk_sama5d4_h32mx(hw);
	unsigned int mckr;

	regmap_read(h32mxclk->regmap, AT91_PMC_MCKR, &mckr);
	if (mckr & AT91_PMC_H32MXDIV)
		return parent_rate / 2;

	if (parent_rate > H32MX_MAX_FREQ)
		pr_warn("H32MX clock is too fast\n");
	return parent_rate;
}

static long clk_sama5d4_h32mx_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *parent_rate)
{
	unsigned long div;

	if (rate > *parent_rate)
		return *parent_rate;
	div = *parent_rate / 2;
	if (rate < div)
		return div;

	if (rate - div < *parent_rate - rate)
		return div;

	return *parent_rate;
}

static int clk_sama5d4_h32mx_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct clk_sama5d4_h32mx *h32mxclk = to_clk_sama5d4_h32mx(hw);
	u32 mckr = 0;

	if (parent_rate != rate && (parent_rate / 2) != rate)
		return -EINVAL;

	if ((parent_rate / 2) == rate)
		mckr = AT91_PMC_H32MXDIV;

	regmap_update_bits(h32mxclk->regmap, AT91_PMC_MCKR,
			   AT91_PMC_H32MXDIV, mckr);

	return 0;
}

static const struct clk_ops h32mx_ops = {
	.recalc_rate = clk_sama5d4_h32mx_recalc_rate,
	.round_rate = clk_sama5d4_h32mx_round_rate,
	.set_rate = clk_sama5d4_h32mx_set_rate,
};

struct clk_hw * __init
at91_clk_register_h32mx(struct regmap *regmap, const char *name,
			const char *parent_name)
{
	struct clk_sama5d4_h32mx *h32mxclk;
	struct clk_init_data init;
	int ret;

	h32mxclk = kzalloc(sizeof(*h32mxclk), GFP_KERNEL);
	if (!h32mxclk)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &h32mx_ops;
	init.parent_names = parent_name ? &parent_name : NULL;
	init.num_parents = parent_name ? 1 : 0;
	init.flags = CLK_SET_RATE_GATE;

	h32mxclk->hw.init = &init;
	h32mxclk->regmap = regmap;

	ret = clk_hw_register(NULL, &h32mxclk->hw);
	if (ret) {
		kfree(h32mxclk);
		return ERR_PTR(ret);
	}

	return &h32mxclk->hw;
}
