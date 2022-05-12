// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2013 Emilio López
 *
 * Emilio López <emilio@elopez.com.ar>
 */

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>

#define SUNXI_OSC24M_GATE	0

static DEFINE_SPINLOCK(hosc_lock);

static void __init sun4i_osc_clk_setup(struct device_node *node)
{
	struct clk *clk;
	struct clk_fixed_rate *fixed;
	struct clk_gate *gate;
	const char *clk_name = node->name;
	u32 rate;

	if (of_property_read_u32(node, "clock-frequency", &rate))
		return;

	/* allocate fixed-rate and gate clock structs */
	fixed = kzalloc(sizeof(struct clk_fixed_rate), GFP_KERNEL);
	if (!fixed)
		return;
	gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
	if (!gate)
		goto err_free_fixed;

	of_property_read_string(node, "clock-output-names", &clk_name);

	/* set up gate and fixed rate properties */
	gate->reg = of_iomap(node, 0);
	gate->bit_idx = SUNXI_OSC24M_GATE;
	gate->lock = &hosc_lock;
	fixed->fixed_rate = rate;

	clk = clk_register_composite(NULL, clk_name,
			NULL, 0,
			NULL, NULL,
			&fixed->hw, &clk_fixed_rate_ops,
			&gate->hw, &clk_gate_ops, 0);

	if (IS_ERR(clk))
		goto err_free_gate;

	of_clk_add_provider(node, of_clk_src_simple_get, clk);

	return;

err_free_gate:
	kfree(gate);
err_free_fixed:
	kfree(fixed);
}
CLK_OF_DECLARE(sun4i_osc, "allwinner,sun4i-a10-osc-clk", sun4i_osc_clk_setup);
