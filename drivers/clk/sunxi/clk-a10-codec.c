// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2013 Emilio López
 *
 * Emilio López <emilio@elopez.com.ar>
 */

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>

#define SUN4I_CODEC_GATE	31

static void __init sun4i_codec_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name, *parent_name;
	void __iomem *reg;

	reg = of_io_request_and_map(node, 0, of_node_full_name(node));
	if (IS_ERR(reg))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);
	parent_name = of_clk_get_parent_name(node, 0);

	clk = clk_register_gate(NULL, clk_name, parent_name,
				CLK_SET_RATE_PARENT, reg,
				SUN4I_CODEC_GATE, 0, NULL);

	if (!IS_ERR(clk))
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
}
CLK_OF_DECLARE(sun4i_codec, "allwinner,sun4i-a10-codec-clk",
	       sun4i_codec_clk_setup);
