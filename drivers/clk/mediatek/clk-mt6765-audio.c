// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt6765-clk.h>

static const struct mtk_gate_regs audio0_cg_regs = {
	.set_ofs = 0x0,
	.clr_ofs = 0x0,
	.sta_ofs = 0x0,
};

static const struct mtk_gate_regs audio1_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x4,
	.sta_ofs = 0x4,
};

#define GATE_AUDIO0(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &audio0_cg_regs,		\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_no_setclr,	\
	}

#define GATE_AUDIO1(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &audio1_cg_regs,		\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_no_setclr,	\
	}

static const struct mtk_gate audio_clks[] = {
	/* AUDIO0 */
	GATE_AUDIO0(CLK_AUDIO_AFE, "aud_afe", "audio_ck", 2),
	GATE_AUDIO0(CLK_AUDIO_22M, "aud_22m", "aud_engen1_ck", 8),
	GATE_AUDIO0(CLK_AUDIO_APLL_TUNER, "aud_apll_tuner",
		    "aud_engen1_ck", 19),
	GATE_AUDIO0(CLK_AUDIO_ADC, "aud_adc", "audio_ck", 24),
	GATE_AUDIO0(CLK_AUDIO_DAC, "aud_dac", "audio_ck", 25),
	GATE_AUDIO0(CLK_AUDIO_DAC_PREDIS, "aud_dac_predis",
		    "audio_ck", 26),
	GATE_AUDIO0(CLK_AUDIO_TML, "aud_tml", "audio_ck", 27),
	/* AUDIO1 */
	GATE_AUDIO1(CLK_AUDIO_I2S1_BCLK, "aud_i2s1_bclk",
		    "audio_ck", 4),
	GATE_AUDIO1(CLK_AUDIO_I2S2_BCLK, "aud_i2s2_bclk",
		    "audio_ck", 5),
	GATE_AUDIO1(CLK_AUDIO_I2S3_BCLK, "aud_i2s3_bclk",
		    "audio_ck", 6),
	GATE_AUDIO1(CLK_AUDIO_I2S4_BCLK, "aud_i2s4_bclk",
		    "audio_ck", 7),
};

static int clk_mt6765_audio_probe(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	int r;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_AUDIO_NR_CLK);

	mtk_clk_register_gates(node, audio_clks,
			       ARRAY_SIZE(audio_clks), clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r)
		pr_err("%s(): could not register clock provider: %d\n",
		       __func__, r);

	return r;
}

static const struct of_device_id of_match_clk_mt6765_audio[] = {
	{ .compatible = "mediatek,mt6765-audsys", },
	{}
};

static struct platform_driver clk_mt6765_audio_drv = {
	.probe = clk_mt6765_audio_probe,
	.driver = {
		.name = "clk-mt6765-audio",
		.of_match_table = of_match_clk_mt6765_audio,
	},
};

builtin_platform_driver(clk_mt6765_audio_drv);
