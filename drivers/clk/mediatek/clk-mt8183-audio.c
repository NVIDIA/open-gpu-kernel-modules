// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2018 MediaTek Inc.
// Author: Weiyi Lu <weiyi.lu@mediatek.com>

#include <linux/clk-provider.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt8183-clk.h>

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

#define GATE_AUDIO0(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &audio0_cg_regs, _shift,	\
		&mtk_clk_gate_ops_no_setclr)

#define GATE_AUDIO1(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &audio1_cg_regs, _shift,	\
		&mtk_clk_gate_ops_no_setclr)

static const struct mtk_gate audio_clks[] = {
	/* AUDIO0 */
	GATE_AUDIO0(CLK_AUDIO_AFE, "aud_afe", "audio_sel",
		2),
	GATE_AUDIO0(CLK_AUDIO_22M, "aud_22m", "aud_eng1_sel",
		8),
	GATE_AUDIO0(CLK_AUDIO_24M, "aud_24m", "aud_eng2_sel",
		9),
	GATE_AUDIO0(CLK_AUDIO_APLL2_TUNER, "aud_apll2_tuner", "aud_eng2_sel",
		18),
	GATE_AUDIO0(CLK_AUDIO_APLL_TUNER, "aud_apll_tuner", "aud_eng1_sel",
		19),
	GATE_AUDIO0(CLK_AUDIO_TDM, "aud_tdm", "apll12_divb",
		20),
	GATE_AUDIO0(CLK_AUDIO_ADC, "aud_adc", "audio_sel",
		24),
	GATE_AUDIO0(CLK_AUDIO_DAC, "aud_dac", "audio_sel",
		25),
	GATE_AUDIO0(CLK_AUDIO_DAC_PREDIS, "aud_dac_predis", "audio_sel",
		26),
	GATE_AUDIO0(CLK_AUDIO_TML, "aud_tml", "audio_sel",
		27),
	/* AUDIO1 */
	GATE_AUDIO1(CLK_AUDIO_I2S1, "aud_i2s1", "audio_sel",
		4),
	GATE_AUDIO1(CLK_AUDIO_I2S2, "aud_i2s2", "audio_sel",
		5),
	GATE_AUDIO1(CLK_AUDIO_I2S3, "aud_i2s3", "audio_sel",
		6),
	GATE_AUDIO1(CLK_AUDIO_I2S4, "aud_i2s4", "audio_sel",
		7),
	GATE_AUDIO1(CLK_AUDIO_PDN_ADDA6_ADC, "aud_pdn_adda6_adc", "audio_sel",
		20),
};

static int clk_mt8183_audio_probe(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	int r;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_AUDIO_NR_CLK);

	mtk_clk_register_gates(node, audio_clks, ARRAY_SIZE(audio_clks),
			clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		return r;

	r = devm_of_platform_populate(&pdev->dev);
	if (r)
		of_clk_del_provider(node);

	return r;
}

static const struct of_device_id of_match_clk_mt8183_audio[] = {
	{ .compatible = "mediatek,mt8183-audiosys", },
	{}
};

static struct platform_driver clk_mt8183_audio_drv = {
	.probe = clk_mt8183_audio_probe,
	.driver = {
		.name = "clk-mt8183-audio",
		.of_match_table = of_match_clk_mt8183_audio,
	},
};

builtin_platform_driver(clk_mt8183_audio_drv);
