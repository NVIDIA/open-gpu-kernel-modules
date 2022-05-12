// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Sean Wang <sean.wang@mediatek.com>
 *
 */

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt2701-clk.h>

#define GATE_G3D(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &g3d_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate_regs g3d_cg_regs = {
	.sta_ofs = 0x0,
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
};

static const struct mtk_gate g3d_clks[] = {
	GATE_G3D(CLK_G3DSYS_CORE, "g3d_core", "mfg_sel", 0),
};

static int clk_mt2701_g3dsys_init(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	struct device_node *node = pdev->dev.of_node;
	int r;

	clk_data = mtk_alloc_clk_data(CLK_G3DSYS_NR);

	mtk_clk_register_gates(node, g3d_clks, ARRAY_SIZE(g3d_clks),
			       clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		dev_err(&pdev->dev,
			"could not register clock provider: %s: %d\n",
			pdev->name, r);

	mtk_register_reset_controller(node, 1, 0xc);

	return r;
}

static const struct of_device_id of_match_clk_mt2701_g3d[] = {
	{
		.compatible = "mediatek,mt2701-g3dsys",
		.data = clk_mt2701_g3dsys_init,
	}, {
		/* sentinel */
	}
};

static int clk_mt2701_g3d_probe(struct platform_device *pdev)
{
	int (*clk_init)(struct platform_device *);
	int r;

	clk_init = of_device_get_match_data(&pdev->dev);
	if (!clk_init)
		return -EINVAL;

	r = clk_init(pdev);
	if (r)
		dev_err(&pdev->dev,
			"could not register clock provider: %s: %d\n",
			pdev->name, r);

	return r;
}

static struct platform_driver clk_mt2701_g3d_drv = {
	.probe = clk_mt2701_g3d_probe,
	.driver = {
		.name = "clk-mt2701-g3d",
		.of_match_table = of_match_clk_mt2701_g3d,
	},
};

builtin_platform_driver(clk_mt2701_g3d_drv);
