// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2018 MediaTek Inc.
// Author: Weiyi Lu <weiyi.lu@mediatek.com>

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt8183-clk.h>

static const struct mtk_gate_regs vdec0_cg_regs = {
	.set_ofs = 0x0,
	.clr_ofs = 0x4,
	.sta_ofs = 0x0,
};

static const struct mtk_gate_regs vdec1_cg_regs = {
	.set_ofs = 0x8,
	.clr_ofs = 0xc,
	.sta_ofs = 0x8,
};

#define GATE_VDEC0_I(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &vdec0_cg_regs, _shift,	\
		&mtk_clk_gate_ops_setclr_inv)

#define GATE_VDEC1_I(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &vdec1_cg_regs, _shift,	\
		&mtk_clk_gate_ops_setclr_inv)

static const struct mtk_gate vdec_clks[] = {
	/* VDEC0 */
	GATE_VDEC0_I(CLK_VDEC_VDEC, "vdec_vdec", "mm_sel", 0),
	/* VDEC1 */
	GATE_VDEC1_I(CLK_VDEC_LARB1, "vdec_larb1", "mm_sel", 0),
};

static int clk_mt8183_vdec_probe(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_VDEC_NR_CLK);

	mtk_clk_register_gates(node, vdec_clks, ARRAY_SIZE(vdec_clks),
			clk_data);

	return of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
}

static const struct of_device_id of_match_clk_mt8183_vdec[] = {
	{ .compatible = "mediatek,mt8183-vdecsys", },
	{}
};

static struct platform_driver clk_mt8183_vdec_drv = {
	.probe = clk_mt8183_vdec_probe,
	.driver = {
		.name = "clk-mt8183-vdec",
		.of_match_table = of_match_clk_mt8183_vdec,
	},
};

builtin_platform_driver(clk_mt8183_vdec_drv);
