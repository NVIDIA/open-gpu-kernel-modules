// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk-provider.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

#include <dt-bindings/clock/qcom,gcc-ipq806x.h>
#include <dt-bindings/reset/qcom,gcc-ipq806x.h>

#include "common.h"
#include "clk-regmap.h"
#include "clk-pll.h"
#include "clk-rcg.h"
#include "clk-branch.h"
#include "clk-hfpll.h"
#include "reset.h"

static struct clk_pll pll0 = {
	.l_reg = 0x30c4,
	.m_reg = 0x30c8,
	.n_reg = 0x30cc,
	.config_reg = 0x30d4,
	.mode_reg = 0x30c0,
	.status_reg = 0x30d8,
	.status_bit = 16,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "pll0",
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_regmap pll0_vote = {
	.enable_reg = 0x34c0,
	.enable_mask = BIT(0),
	.hw.init = &(struct clk_init_data){
		.name = "pll0_vote",
		.parent_names = (const char *[]){ "pll0" },
		.num_parents = 1,
		.ops = &clk_pll_vote_ops,
	},
};

static struct clk_pll pll3 = {
	.l_reg = 0x3164,
	.m_reg = 0x3168,
	.n_reg = 0x316c,
	.config_reg = 0x3174,
	.mode_reg = 0x3160,
	.status_reg = 0x3178,
	.status_bit = 16,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "pll3",
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_regmap pll4_vote = {
	.enable_reg = 0x34c0,
	.enable_mask = BIT(4),
	.hw.init = &(struct clk_init_data){
		.name = "pll4_vote",
		.parent_names = (const char *[]){ "pll4" },
		.num_parents = 1,
		.ops = &clk_pll_vote_ops,
	},
};

static struct clk_pll pll8 = {
	.l_reg = 0x3144,
	.m_reg = 0x3148,
	.n_reg = 0x314c,
	.config_reg = 0x3154,
	.mode_reg = 0x3140,
	.status_reg = 0x3158,
	.status_bit = 16,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "pll8",
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_regmap pll8_vote = {
	.enable_reg = 0x34c0,
	.enable_mask = BIT(8),
	.hw.init = &(struct clk_init_data){
		.name = "pll8_vote",
		.parent_names = (const char *[]){ "pll8" },
		.num_parents = 1,
		.ops = &clk_pll_vote_ops,
	},
};

static struct hfpll_data hfpll0_data = {
	.mode_reg = 0x3200,
	.l_reg = 0x3208,
	.m_reg = 0x320c,
	.n_reg = 0x3210,
	.config_reg = 0x3204,
	.status_reg = 0x321c,
	.config_val = 0x7845c665,
	.droop_reg = 0x3214,
	.droop_val = 0x0108c000,
	.min_rate = 600000000UL,
	.max_rate = 1800000000UL,
};

static struct clk_hfpll hfpll0 = {
	.d = &hfpll0_data,
	.clkr.hw.init = &(struct clk_init_data){
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.name = "hfpll0",
		.ops = &clk_ops_hfpll,
		.flags = CLK_IGNORE_UNUSED,
	},
	.lock = __SPIN_LOCK_UNLOCKED(hfpll0.lock),
};

static struct hfpll_data hfpll1_data = {
	.mode_reg = 0x3240,
	.l_reg = 0x3248,
	.m_reg = 0x324c,
	.n_reg = 0x3250,
	.config_reg = 0x3244,
	.status_reg = 0x325c,
	.config_val = 0x7845c665,
	.droop_reg = 0x3314,
	.droop_val = 0x0108c000,
	.min_rate = 600000000UL,
	.max_rate = 1800000000UL,
};

static struct clk_hfpll hfpll1 = {
	.d = &hfpll1_data,
	.clkr.hw.init = &(struct clk_init_data){
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.name = "hfpll1",
		.ops = &clk_ops_hfpll,
		.flags = CLK_IGNORE_UNUSED,
	},
	.lock = __SPIN_LOCK_UNLOCKED(hfpll1.lock),
};

static struct hfpll_data hfpll_l2_data = {
	.mode_reg = 0x3300,
	.l_reg = 0x3308,
	.m_reg = 0x330c,
	.n_reg = 0x3310,
	.config_reg = 0x3304,
	.status_reg = 0x331c,
	.config_val = 0x7845c665,
	.droop_reg = 0x3314,
	.droop_val = 0x0108c000,
	.min_rate = 600000000UL,
	.max_rate = 1800000000UL,
};

static struct clk_hfpll hfpll_l2 = {
	.d = &hfpll_l2_data,
	.clkr.hw.init = &(struct clk_init_data){
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.name = "hfpll_l2",
		.ops = &clk_ops_hfpll,
		.flags = CLK_IGNORE_UNUSED,
	},
	.lock = __SPIN_LOCK_UNLOCKED(hfpll_l2.lock),
};

static struct clk_pll pll14 = {
	.l_reg = 0x31c4,
	.m_reg = 0x31c8,
	.n_reg = 0x31cc,
	.config_reg = 0x31d4,
	.mode_reg = 0x31c0,
	.status_reg = 0x31d8,
	.status_bit = 16,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "pll14",
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

static struct clk_regmap pll14_vote = {
	.enable_reg = 0x34c0,
	.enable_mask = BIT(14),
	.hw.init = &(struct clk_init_data){
		.name = "pll14_vote",
		.parent_names = (const char *[]){ "pll14" },
		.num_parents = 1,
		.ops = &clk_pll_vote_ops,
	},
};

#define NSS_PLL_RATE(f, _l, _m, _n, i) \
	{  \
		.freq = f,  \
		.l = _l, \
		.m = _m, \
		.n = _n, \
		.ibits = i, \
	}

static struct pll_freq_tbl pll18_freq_tbl[] = {
	NSS_PLL_RATE(550000000, 44, 0, 1, 0x01495625),
	NSS_PLL_RATE(733000000, 58, 16, 25, 0x014b5625),
};

static struct clk_pll pll18 = {
	.l_reg = 0x31a4,
	.m_reg = 0x31a8,
	.n_reg = 0x31ac,
	.config_reg = 0x31b4,
	.mode_reg = 0x31a0,
	.status_reg = 0x31b8,
	.status_bit = 16,
	.post_div_shift = 16,
	.post_div_width = 1,
	.freq_tbl = pll18_freq_tbl,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "pll18",
		.parent_names = (const char *[]){ "pxo" },
		.num_parents = 1,
		.ops = &clk_pll_ops,
	},
};

enum {
	P_PXO,
	P_PLL8,
	P_PLL3,
	P_PLL0,
	P_CXO,
	P_PLL14,
	P_PLL18,
};

static const struct parent_map gcc_pxo_pll8_map[] = {
	{ P_PXO, 0 },
	{ P_PLL8, 3 }
};

static const char * const gcc_pxo_pll8[] = {
	"pxo",
	"pll8_vote",
};

static const struct parent_map gcc_pxo_pll8_cxo_map[] = {
	{ P_PXO, 0 },
	{ P_PLL8, 3 },
	{ P_CXO, 5 }
};

static const char * const gcc_pxo_pll8_cxo[] = {
	"pxo",
	"pll8_vote",
	"cxo",
};

static const struct parent_map gcc_pxo_pll3_map[] = {
	{ P_PXO, 0 },
	{ P_PLL3, 1 }
};

static const struct parent_map gcc_pxo_pll3_sata_map[] = {
	{ P_PXO, 0 },
	{ P_PLL3, 6 }
};

static const char * const gcc_pxo_pll3[] = {
	"pxo",
	"pll3",
};

static const struct parent_map gcc_pxo_pll8_pll0[] = {
	{ P_PXO, 0 },
	{ P_PLL8, 3 },
	{ P_PLL0, 2 }
};

static const char * const gcc_pxo_pll8_pll0_map[] = {
	"pxo",
	"pll8_vote",
	"pll0_vote",
};

static const struct parent_map gcc_pxo_pll8_pll14_pll18_pll0_map[] = {
	{ P_PXO, 0 },
	{ P_PLL8, 4 },
	{ P_PLL0, 2 },
	{ P_PLL14, 5 },
	{ P_PLL18, 1 }
};

static const char * const gcc_pxo_pll8_pll14_pll18_pll0[] = {
	"pxo",
	"pll8_vote",
	"pll0_vote",
	"pll14",
	"pll18",
};

static struct freq_tbl clk_tbl_gsbi_uart[] = {
	{  1843200, P_PLL8, 2,  6, 625 },
	{  3686400, P_PLL8, 2, 12, 625 },
	{  7372800, P_PLL8, 2, 24, 625 },
	{ 14745600, P_PLL8, 2, 48, 625 },
	{ 16000000, P_PLL8, 4,  1,   6 },
	{ 24000000, P_PLL8, 4,  1,   4 },
	{ 32000000, P_PLL8, 4,  1,   3 },
	{ 40000000, P_PLL8, 1,  5,  48 },
	{ 46400000, P_PLL8, 1, 29, 240 },
	{ 48000000, P_PLL8, 4,  1,   2 },
	{ 51200000, P_PLL8, 1,  2,  15 },
	{ 56000000, P_PLL8, 1,  7,  48 },
	{ 58982400, P_PLL8, 1, 96, 625 },
	{ 64000000, P_PLL8, 2,  1,   3 },
	{ }
};

static struct clk_rcg gsbi1_uart_src = {
	.ns_reg = 0x29d4,
	.md_reg = 0x29d0,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x29d4,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi1_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi1_uart_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x29d4,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi1_uart_clk",
			.parent_names = (const char *[]){
				"gsbi1_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi2_uart_src = {
	.ns_reg = 0x29f4,
	.md_reg = 0x29f0,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x29f4,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi2_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi2_uart_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 8,
	.clkr = {
		.enable_reg = 0x29f4,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi2_uart_clk",
			.parent_names = (const char *[]){
				"gsbi2_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi4_uart_src = {
	.ns_reg = 0x2a34,
	.md_reg = 0x2a30,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x2a34,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi4_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi4_uart_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 26,
	.clkr = {
		.enable_reg = 0x2a34,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi4_uart_clk",
			.parent_names = (const char *[]){
				"gsbi4_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi5_uart_src = {
	.ns_reg = 0x2a54,
	.md_reg = 0x2a50,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x2a54,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi5_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi5_uart_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 22,
	.clkr = {
		.enable_reg = 0x2a54,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi5_uart_clk",
			.parent_names = (const char *[]){
				"gsbi5_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi6_uart_src = {
	.ns_reg = 0x2a74,
	.md_reg = 0x2a70,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x2a74,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi6_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi6_uart_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 18,
	.clkr = {
		.enable_reg = 0x2a74,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi6_uart_clk",
			.parent_names = (const char *[]){
				"gsbi6_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi7_uart_src = {
	.ns_reg = 0x2a94,
	.md_reg = 0x2a90,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_uart,
	.clkr = {
		.enable_reg = 0x2a94,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi7_uart_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi7_uart_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 14,
	.clkr = {
		.enable_reg = 0x2a94,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi7_uart_clk",
			.parent_names = (const char *[]){
				"gsbi7_uart_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct freq_tbl clk_tbl_gsbi_qup[] = {
	{  1100000, P_PXO,  1, 2, 49 },
	{  5400000, P_PXO,  1, 1,  5 },
	{ 10800000, P_PXO,  1, 2,  5 },
	{ 15060000, P_PLL8, 1, 2, 51 },
	{ 24000000, P_PLL8, 4, 1,  4 },
	{ 25000000, P_PXO,  1, 0,  0 },
	{ 25600000, P_PLL8, 1, 1, 15 },
	{ 48000000, P_PLL8, 4, 1,  2 },
	{ 51200000, P_PLL8, 1, 2, 15 },
	{ }
};

static struct clk_rcg gsbi1_qup_src = {
	.ns_reg = 0x29cc,
	.md_reg = 0x29c8,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x29cc,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi1_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi1_qup_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 11,
	.clkr = {
		.enable_reg = 0x29cc,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi1_qup_clk",
			.parent_names = (const char *[]){ "gsbi1_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi2_qup_src = {
	.ns_reg = 0x29ec,
	.md_reg = 0x29e8,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x29ec,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi2_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi2_qup_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 6,
	.clkr = {
		.enable_reg = 0x29ec,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi2_qup_clk",
			.parent_names = (const char *[]){ "gsbi2_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi4_qup_src = {
	.ns_reg = 0x2a2c,
	.md_reg = 0x2a28,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x2a2c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi4_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi4_qup_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 24,
	.clkr = {
		.enable_reg = 0x2a2c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi4_qup_clk",
			.parent_names = (const char *[]){ "gsbi4_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi5_qup_src = {
	.ns_reg = 0x2a4c,
	.md_reg = 0x2a48,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x2a4c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi5_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi5_qup_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 20,
	.clkr = {
		.enable_reg = 0x2a4c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi5_qup_clk",
			.parent_names = (const char *[]){ "gsbi5_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi6_qup_src = {
	.ns_reg = 0x2a6c,
	.md_reg = 0x2a68,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x2a6c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi6_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi6_qup_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 16,
	.clkr = {
		.enable_reg = 0x2a6c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi6_qup_clk",
			.parent_names = (const char *[]){ "gsbi6_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gsbi7_qup_src = {
	.ns_reg = 0x2a8c,
	.md_reg = 0x2a88,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_gsbi_qup,
	.clkr = {
		.enable_reg = 0x2a8c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi7_qup_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	},
};

static struct clk_branch gsbi7_qup_clk = {
	.halt_reg = 0x2fd0,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x2a8c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi7_qup_clk",
			.parent_names = (const char *[]){ "gsbi7_qup_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch gsbi1_h_clk = {
	.hwcg_reg = 0x29c0,
	.hwcg_bit = 6,
	.halt_reg = 0x2fcc,
	.halt_bit = 13,
	.clkr = {
		.enable_reg = 0x29c0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch gsbi2_h_clk = {
	.hwcg_reg = 0x29e0,
	.hwcg_bit = 6,
	.halt_reg = 0x2fcc,
	.halt_bit = 9,
	.clkr = {
		.enable_reg = 0x29e0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi2_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch gsbi4_h_clk = {
	.hwcg_reg = 0x2a20,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd0,
	.halt_bit = 27,
	.clkr = {
		.enable_reg = 0x2a20,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi4_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch gsbi5_h_clk = {
	.hwcg_reg = 0x2a40,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd0,
	.halt_bit = 23,
	.clkr = {
		.enable_reg = 0x2a40,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi5_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch gsbi6_h_clk = {
	.hwcg_reg = 0x2a60,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd0,
	.halt_bit = 19,
	.clkr = {
		.enable_reg = 0x2a60,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi6_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch gsbi7_h_clk = {
	.hwcg_reg = 0x2a80,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd0,
	.halt_bit = 15,
	.clkr = {
		.enable_reg = 0x2a80,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gsbi7_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_gp[] = {
	{ 12500000, P_PXO,  2, 0, 0 },
	{ 25000000, P_PXO,  1, 0, 0 },
	{ 64000000, P_PLL8, 2, 1, 3 },
	{ 76800000, P_PLL8, 1, 1, 5 },
	{ 96000000, P_PLL8, 4, 0, 0 },
	{ 128000000, P_PLL8, 3, 0, 0 },
	{ 192000000, P_PLL8, 2, 0, 0 },
	{ }
};

static struct clk_rcg gp0_src = {
	.ns_reg = 0x2d24,
	.md_reg = 0x2d00,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_cxo_map,
	},
	.freq_tbl = clk_tbl_gp,
	.clkr = {
		.enable_reg = 0x2d24,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gp0_src",
			.parent_names = gcc_pxo_pll8_cxo,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_PARENT_GATE,
		},
	}
};

static struct clk_branch gp0_clk = {
	.halt_reg = 0x2fd8,
	.halt_bit = 7,
	.clkr = {
		.enable_reg = 0x2d24,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gp0_clk",
			.parent_names = (const char *[]){ "gp0_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gp1_src = {
	.ns_reg = 0x2d44,
	.md_reg = 0x2d40,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_cxo_map,
	},
	.freq_tbl = clk_tbl_gp,
	.clkr = {
		.enable_reg = 0x2d44,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gp1_src",
			.parent_names = gcc_pxo_pll8_cxo,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	}
};

static struct clk_branch gp1_clk = {
	.halt_reg = 0x2fd8,
	.halt_bit = 6,
	.clkr = {
		.enable_reg = 0x2d44,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gp1_clk",
			.parent_names = (const char *[]){ "gp1_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg gp2_src = {
	.ns_reg = 0x2d64,
	.md_reg = 0x2d60,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_cxo_map,
	},
	.freq_tbl = clk_tbl_gp,
	.clkr = {
		.enable_reg = 0x2d64,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "gp2_src",
			.parent_names = gcc_pxo_pll8_cxo,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	}
};

static struct clk_branch gp2_clk = {
	.halt_reg = 0x2fd8,
	.halt_bit = 5,
	.clkr = {
		.enable_reg = 0x2d64,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "gp2_clk",
			.parent_names = (const char *[]){ "gp2_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch pmem_clk = {
	.hwcg_reg = 0x25a0,
	.hwcg_bit = 6,
	.halt_reg = 0x2fc8,
	.halt_bit = 20,
	.clkr = {
		.enable_reg = 0x25a0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pmem_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_rcg prng_src = {
	.ns_reg = 0x2e80,
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.clkr = {
		.enable_reg = 0x2e80,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "prng_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
		},
	},
};

static struct clk_branch prng_clk = {
	.halt_reg = 0x2fd8,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 10,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(10),
		.hw.init = &(struct clk_init_data){
			.name = "prng_clk",
			.parent_names = (const char *[]){ "prng_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_sdc[] = {
	{    200000, P_PXO,   2, 2, 125 },
	{    400000, P_PLL8,  4, 1, 240 },
	{  16000000, P_PLL8,  4, 1,   6 },
	{  17070000, P_PLL8,  1, 2,  45 },
	{  20210000, P_PLL8,  1, 1,  19 },
	{  24000000, P_PLL8,  4, 1,   4 },
	{  48000000, P_PLL8,  4, 1,   2 },
	{  64000000, P_PLL8,  3, 1,   2 },
	{  96000000, P_PLL8,  4, 0,   0 },
	{ 192000000, P_PLL8,  2, 0,   0 },
	{ }
};

static struct clk_rcg sdc1_src = {
	.ns_reg = 0x282c,
	.md_reg = 0x2828,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_sdc,
	.clkr = {
		.enable_reg = 0x282c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "sdc1_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
		},
	}
};

static struct clk_branch sdc1_clk = {
	.halt_reg = 0x2fc8,
	.halt_bit = 6,
	.clkr = {
		.enable_reg = 0x282c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "sdc1_clk",
			.parent_names = (const char *[]){ "sdc1_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg sdc3_src = {
	.ns_reg = 0x286c,
	.md_reg = 0x2868,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_sdc,
	.clkr = {
		.enable_reg = 0x286c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "sdc3_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
		},
	}
};

static struct clk_branch sdc3_clk = {
	.halt_reg = 0x2fc8,
	.halt_bit = 4,
	.clkr = {
		.enable_reg = 0x286c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "sdc3_clk",
			.parent_names = (const char *[]){ "sdc3_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch sdc1_h_clk = {
	.hwcg_reg = 0x2820,
	.hwcg_bit = 6,
	.halt_reg = 0x2fc8,
	.halt_bit = 11,
	.clkr = {
		.enable_reg = 0x2820,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sdc1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch sdc3_h_clk = {
	.hwcg_reg = 0x2860,
	.hwcg_bit = 6,
	.halt_reg = 0x2fc8,
	.halt_bit = 9,
	.clkr = {
		.enable_reg = 0x2860,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sdc3_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_tsif_ref[] = {
	{ 105000, P_PXO,  1, 1, 256 },
	{ }
};

static struct clk_rcg tsif_ref_src = {
	.ns_reg = 0x2710,
	.md_reg = 0x270c,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 16,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_map,
	},
	.freq_tbl = clk_tbl_tsif_ref,
	.clkr = {
		.enable_reg = 0x2710,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "tsif_ref_src",
			.parent_names = gcc_pxo_pll8,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
		},
	}
};

static struct clk_branch tsif_ref_clk = {
	.halt_reg = 0x2fd4,
	.halt_bit = 5,
	.clkr = {
		.enable_reg = 0x2710,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "tsif_ref_clk",
			.parent_names = (const char *[]){ "tsif_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch tsif_h_clk = {
	.hwcg_reg = 0x2700,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd4,
	.halt_bit = 7,
	.clkr = {
		.enable_reg = 0x2700,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "tsif_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch dma_bam_h_clk = {
	.hwcg_reg = 0x25c0,
	.hwcg_bit = 6,
	.halt_reg = 0x2fc8,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x25c0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "dma_bam_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch adm0_clk = {
	.halt_reg = 0x2fdc,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(2),
		.hw.init = &(struct clk_init_data){
			.name = "adm0_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch adm0_pbus_clk = {
	.hwcg_reg = 0x2208,
	.hwcg_bit = 6,
	.halt_reg = 0x2fdc,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 11,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(3),
		.hw.init = &(struct clk_init_data){
			.name = "adm0_pbus_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pmic_arb0_h_clk = {
	.halt_reg = 0x2fd8,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 22,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(8),
		.hw.init = &(struct clk_init_data){
			.name = "pmic_arb0_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pmic_arb1_h_clk = {
	.halt_reg = 0x2fd8,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 21,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "pmic_arb1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pmic_ssbi2_clk = {
	.halt_reg = 0x2fd8,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 23,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(7),
		.hw.init = &(struct clk_init_data){
			.name = "pmic_ssbi2_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch rpm_msg_ram_h_clk = {
	.hwcg_reg = 0x27e0,
	.hwcg_bit = 6,
	.halt_reg = 0x2fd8,
	.halt_check = BRANCH_HALT_VOTED,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x3080,
		.enable_mask = BIT(6),
		.hw.init = &(struct clk_init_data){
			.name = "rpm_msg_ram_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_pcie_ref[] = {
	{ 100000000, P_PLL3,  12, 0, 0 },
	{ }
};

static struct clk_rcg pcie_ref_src = {
	.ns_reg = 0x3860,
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll3_map,
	},
	.freq_tbl = clk_tbl_pcie_ref,
	.clkr = {
		.enable_reg = 0x3860,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_ref_src",
			.parent_names = gcc_pxo_pll3,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch pcie_ref_src_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 30,
	.clkr = {
		.enable_reg = 0x3860,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_ref_src_clk",
			.parent_names = (const char *[]){ "pcie_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch pcie_a_clk = {
	.halt_reg = 0x2fc0,
	.halt_bit = 13,
	.clkr = {
		.enable_reg = 0x22c0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_a_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie_aux_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 31,
	.clkr = {
		.enable_reg = 0x22c8,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_aux_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie_h_clk = {
	.halt_reg = 0x2fd4,
	.halt_bit = 8,
	.clkr = {
		.enable_reg = 0x22cc,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie_phy_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 29,
	.clkr = {
		.enable_reg = 0x22d0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie_phy_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_rcg pcie1_ref_src = {
	.ns_reg = 0x3aa0,
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll3_map,
	},
	.freq_tbl = clk_tbl_pcie_ref,
	.clkr = {
		.enable_reg = 0x3aa0,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_ref_src",
			.parent_names = gcc_pxo_pll3,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch pcie1_ref_src_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 27,
	.clkr = {
		.enable_reg = 0x3aa0,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_ref_src_clk",
			.parent_names = (const char *[]){ "pcie1_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch pcie1_a_clk = {
	.halt_reg = 0x2fc0,
	.halt_bit = 10,
	.clkr = {
		.enable_reg = 0x3a80,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_a_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie1_aux_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 28,
	.clkr = {
		.enable_reg = 0x3a88,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_aux_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie1_h_clk = {
	.halt_reg = 0x2fd4,
	.halt_bit = 9,
	.clkr = {
		.enable_reg = 0x3a8c,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie1_phy_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 26,
	.clkr = {
		.enable_reg = 0x3a90,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie1_phy_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_rcg pcie2_ref_src = {
	.ns_reg = 0x3ae0,
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll3_map,
	},
	.freq_tbl = clk_tbl_pcie_ref,
	.clkr = {
		.enable_reg = 0x3ae0,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_ref_src",
			.parent_names = gcc_pxo_pll3,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch pcie2_ref_src_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 24,
	.clkr = {
		.enable_reg = 0x3ae0,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_ref_src_clk",
			.parent_names = (const char *[]){ "pcie2_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch pcie2_a_clk = {
	.halt_reg = 0x2fc0,
	.halt_bit = 9,
	.clkr = {
		.enable_reg = 0x3ac0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_a_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie2_aux_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 25,
	.clkr = {
		.enable_reg = 0x3ac8,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_aux_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie2_h_clk = {
	.halt_reg = 0x2fd4,
	.halt_bit = 10,
	.clkr = {
		.enable_reg = 0x3acc,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch pcie2_phy_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 23,
	.clkr = {
		.enable_reg = 0x3ad0,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "pcie2_phy_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_sata_ref[] = {
	{ 100000000, P_PLL3,  12, 0, 0 },
	{ }
};

static struct clk_rcg sata_ref_src = {
	.ns_reg = 0x2c08,
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll3_sata_map,
	},
	.freq_tbl = clk_tbl_sata_ref,
	.clkr = {
		.enable_reg = 0x2c08,
		.enable_mask = BIT(7),
		.hw.init = &(struct clk_init_data){
			.name = "sata_ref_src",
			.parent_names = gcc_pxo_pll3,
			.num_parents = 2,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch sata_rxoob_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 20,
	.clkr = {
		.enable_reg = 0x2c0c,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_rxoob_clk",
			.parent_names = (const char *[]){ "sata_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch sata_pmalive_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 19,
	.clkr = {
		.enable_reg = 0x2c10,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_pmalive_clk",
			.parent_names = (const char *[]){ "sata_ref_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch sata_phy_ref_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 18,
	.clkr = {
		.enable_reg = 0x2c14,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_phy_ref_clk",
			.parent_names = (const char *[]){ "pxo" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch sata_a_clk = {
	.halt_reg = 0x2fc0,
	.halt_bit = 12,
	.clkr = {
		.enable_reg = 0x2c20,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_a_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch sata_h_clk = {
	.halt_reg = 0x2fdc,
	.halt_bit = 21,
	.clkr = {
		.enable_reg = 0x2c00,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch sfab_sata_s_h_clk = {
	.halt_reg = 0x2fc4,
	.halt_bit = 14,
	.clkr = {
		.enable_reg = 0x2480,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sfab_sata_s_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch sata_phy_cfg_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 14,
	.clkr = {
		.enable_reg = 0x2c40,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "sata_phy_cfg_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_usb30_master[] = {
	{ 125000000, P_PLL0,  1, 5, 32 },
	{ }
};

static struct clk_rcg usb30_master_clk_src = {
	.ns_reg = 0x3b2c,
	.md_reg = 0x3b28,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll0,
	},
	.freq_tbl = clk_tbl_usb30_master,
	.clkr = {
		.enable_reg = 0x3b2c,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_master_ref_src",
			.parent_names = gcc_pxo_pll8_pll0_map,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch usb30_0_branch_clk = {
	.halt_reg = 0x2fc4,
	.halt_bit = 22,
	.clkr = {
		.enable_reg = 0x3b24,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_0_branch_clk",
			.parent_names = (const char *[]){ "usb30_master_ref_src", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch usb30_1_branch_clk = {
	.halt_reg = 0x2fc4,
	.halt_bit = 17,
	.clkr = {
		.enable_reg = 0x3b34,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_1_branch_clk",
			.parent_names = (const char *[]){ "usb30_master_ref_src", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static const struct freq_tbl clk_tbl_usb30_utmi[] = {
	{ 60000000, P_PLL8,  1, 5, 32 },
	{ }
};

static struct clk_rcg usb30_utmi_clk = {
	.ns_reg = 0x3b44,
	.md_reg = 0x3b40,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll0,
	},
	.freq_tbl = clk_tbl_usb30_utmi,
	.clkr = {
		.enable_reg = 0x3b44,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_utmi_clk",
			.parent_names = gcc_pxo_pll8_pll0_map,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch usb30_0_utmi_clk_ctl = {
	.halt_reg = 0x2fc4,
	.halt_bit = 21,
	.clkr = {
		.enable_reg = 0x3b48,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_0_utmi_clk_ctl",
			.parent_names = (const char *[]){ "usb30_utmi_clk", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch usb30_1_utmi_clk_ctl = {
	.halt_reg = 0x2fc4,
	.halt_bit = 15,
	.clkr = {
		.enable_reg = 0x3b4c,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb30_1_utmi_clk_ctl",
			.parent_names = (const char *[]){ "usb30_utmi_clk", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static const struct freq_tbl clk_tbl_usb[] = {
	{ 60000000, P_PLL8,  1, 5, 32 },
	{ }
};

static struct clk_rcg usb_hs1_xcvr_clk_src = {
	.ns_reg = 0x290C,
	.md_reg = 0x2908,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll0,
	},
	.freq_tbl = clk_tbl_usb,
	.clkr = {
		.enable_reg = 0x2968,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "usb_hs1_xcvr_src",
			.parent_names = gcc_pxo_pll8_pll0_map,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch usb_hs1_xcvr_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 17,
	.clkr = {
		.enable_reg = 0x290c,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "usb_hs1_xcvr_clk",
			.parent_names = (const char *[]){ "usb_hs1_xcvr_src" },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch usb_hs1_h_clk = {
	.hwcg_reg = 0x2900,
	.hwcg_bit = 6,
	.halt_reg = 0x2fc8,
	.halt_bit = 1,
	.clkr = {
		.enable_reg = 0x2900,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb_hs1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_rcg usb_fs1_xcvr_clk_src = {
	.ns_reg = 0x2968,
	.md_reg = 0x2964,
	.mn = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.p = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.s = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll0,
	},
	.freq_tbl = clk_tbl_usb,
	.clkr = {
		.enable_reg = 0x2968,
		.enable_mask = BIT(11),
		.hw.init = &(struct clk_init_data){
			.name = "usb_fs1_xcvr_src",
			.parent_names = gcc_pxo_pll8_pll0_map,
			.num_parents = 3,
			.ops = &clk_rcg_ops,
			.flags = CLK_SET_RATE_GATE,
		},
	},
};

static struct clk_branch usb_fs1_xcvr_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 17,
	.clkr = {
		.enable_reg = 0x2968,
		.enable_mask = BIT(9),
		.hw.init = &(struct clk_init_data){
			.name = "usb_fs1_xcvr_clk",
			.parent_names = (const char *[]){ "usb_fs1_xcvr_src", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch usb_fs1_sys_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 18,
	.clkr = {
		.enable_reg = 0x296c,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb_fs1_sys_clk",
			.parent_names = (const char *[]){ "usb_fs1_xcvr_src", },
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_branch usb_fs1_h_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 19,
	.clkr = {
		.enable_reg = 0x2960,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "usb_fs1_h_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch ebi2_clk = {
	.hwcg_reg = 0x3b00,
	.hwcg_bit = 6,
	.halt_reg = 0x2fcc,
	.halt_bit = 1,
	.clkr = {
		.enable_reg = 0x3b00,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "ebi2_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static struct clk_branch ebi2_aon_clk = {
	.halt_reg = 0x2fcc,
	.halt_bit = 0,
	.clkr = {
		.enable_reg = 0x3b00,
		.enable_mask = BIT(8),
		.hw.init = &(struct clk_init_data){
			.name = "ebi2_always_on_clk",
			.ops = &clk_branch_ops,
		},
	},
};

static const struct freq_tbl clk_tbl_gmac[] = {
	{ 133000000, P_PLL0, 1,  50, 301 },
	{ 266000000, P_PLL0, 1, 127, 382 },
	{ }
};

static struct clk_dyn_rcg gmac_core1_src = {
	.ns_reg[0] = 0x3cac,
	.ns_reg[1] = 0x3cb0,
	.md_reg[0] = 0x3ca4,
	.md_reg[1] = 0x3ca8,
	.bank_reg = 0x3ca0,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_gmac,
	.clkr = {
		.enable_reg = 0x3ca0,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core1_src",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
		},
	},
};

static struct clk_branch gmac_core1_clk = {
	.halt_reg = 0x3c20,
	.halt_bit = 4,
	.hwcg_reg = 0x3cb4,
	.hwcg_bit = 6,
	.clkr = {
		.enable_reg = 0x3cb4,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core1_clk",
			.parent_names = (const char *[]){
				"gmac_core1_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_dyn_rcg gmac_core2_src = {
	.ns_reg[0] = 0x3ccc,
	.ns_reg[1] = 0x3cd0,
	.md_reg[0] = 0x3cc4,
	.md_reg[1] = 0x3cc8,
	.bank_reg = 0x3ca0,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_gmac,
	.clkr = {
		.enable_reg = 0x3cc0,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core2_src",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
		},
	},
};

static struct clk_branch gmac_core2_clk = {
	.halt_reg = 0x3c20,
	.halt_bit = 5,
	.hwcg_reg = 0x3cd4,
	.hwcg_bit = 6,
	.clkr = {
		.enable_reg = 0x3cd4,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core2_clk",
			.parent_names = (const char *[]){
				"gmac_core2_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_dyn_rcg gmac_core3_src = {
	.ns_reg[0] = 0x3cec,
	.ns_reg[1] = 0x3cf0,
	.md_reg[0] = 0x3ce4,
	.md_reg[1] = 0x3ce8,
	.bank_reg = 0x3ce0,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_gmac,
	.clkr = {
		.enable_reg = 0x3ce0,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core3_src",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
		},
	},
};

static struct clk_branch gmac_core3_clk = {
	.halt_reg = 0x3c20,
	.halt_bit = 6,
	.hwcg_reg = 0x3cf4,
	.hwcg_bit = 6,
	.clkr = {
		.enable_reg = 0x3cf4,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core3_clk",
			.parent_names = (const char *[]){
				"gmac_core3_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_dyn_rcg gmac_core4_src = {
	.ns_reg[0] = 0x3d0c,
	.ns_reg[1] = 0x3d10,
	.md_reg[0] = 0x3d04,
	.md_reg[1] = 0x3d08,
	.bank_reg = 0x3d00,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_gmac,
	.clkr = {
		.enable_reg = 0x3d00,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core4_src",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
		},
	},
};

static struct clk_branch gmac_core4_clk = {
	.halt_reg = 0x3c20,
	.halt_bit = 7,
	.hwcg_reg = 0x3d14,
	.hwcg_bit = 6,
	.clkr = {
		.enable_reg = 0x3d14,
		.enable_mask = BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "gmac_core4_clk",
			.parent_names = (const char *[]){
				"gmac_core4_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static const struct freq_tbl clk_tbl_nss_tcm[] = {
	{ 266000000, P_PLL0, 3, 0, 0 },
	{ 400000000, P_PLL0, 2, 0, 0 },
	{ }
};

static struct clk_dyn_rcg nss_tcm_src = {
	.ns_reg[0] = 0x3dc4,
	.ns_reg[1] = 0x3dc8,
	.bank_reg = 0x3dc0,
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 4,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_nss_tcm,
	.clkr = {
		.enable_reg = 0x3dc0,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "nss_tcm_src",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
		},
	},
};

static struct clk_branch nss_tcm_clk = {
	.halt_reg = 0x3c20,
	.halt_bit = 14,
	.clkr = {
		.enable_reg = 0x3dd0,
		.enable_mask = BIT(6) | BIT(4),
		.hw.init = &(struct clk_init_data){
			.name = "nss_tcm_clk",
			.parent_names = (const char *[]){
				"nss_tcm_src",
			},
			.num_parents = 1,
			.ops = &clk_branch_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static const struct freq_tbl clk_tbl_nss[] = {
	{ 110000000, P_PLL18, 1, 1, 5 },
	{ 275000000, P_PLL18, 2, 0, 0 },
	{ 550000000, P_PLL18, 1, 0, 0 },
	{ 733000000, P_PLL18, 1, 0, 0 },
	{ }
};

static struct clk_dyn_rcg ubi32_core1_src_clk = {
	.ns_reg[0] = 0x3d2c,
	.ns_reg[1] = 0x3d30,
	.md_reg[0] = 0x3d24,
	.md_reg[1] = 0x3d28,
	.bank_reg = 0x3d20,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_nss,
	.clkr = {
		.enable_reg = 0x3d20,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "ubi32_core1_src_clk",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
		},
	},
};

static struct clk_dyn_rcg ubi32_core2_src_clk = {
	.ns_reg[0] = 0x3d4c,
	.ns_reg[1] = 0x3d50,
	.md_reg[0] = 0x3d44,
	.md_reg[1] = 0x3d48,
	.bank_reg = 0x3d40,
	.mn[0] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.mn[1] = {
		.mnctr_en_bit = 8,
		.mnctr_reset_bit = 7,
		.mnctr_mode_shift = 5,
		.n_val_shift = 16,
		.m_val_shift = 16,
		.width = 8,
	},
	.s[0] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.s[1] = {
		.src_sel_shift = 0,
		.parent_map = gcc_pxo_pll8_pll14_pll18_pll0_map,
	},
	.p[0] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.p[1] = {
		.pre_div_shift = 3,
		.pre_div_width = 2,
	},
	.mux_sel_bit = 0,
	.freq_tbl = clk_tbl_nss,
	.clkr = {
		.enable_reg = 0x3d40,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "ubi32_core2_src_clk",
			.parent_names = gcc_pxo_pll8_pll14_pll18_pll0,
			.num_parents = 5,
			.ops = &clk_dyn_rcg_ops,
			.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE,
		},
	},
};

static struct clk_regmap *gcc_ipq806x_clks[] = {
	[PLL0] = &pll0.clkr,
	[PLL0_VOTE] = &pll0_vote,
	[PLL3] = &pll3.clkr,
	[PLL4_VOTE] = &pll4_vote,
	[PLL8] = &pll8.clkr,
	[PLL8_VOTE] = &pll8_vote,
	[PLL14] = &pll14.clkr,
	[PLL14_VOTE] = &pll14_vote,
	[PLL18] = &pll18.clkr,
	[GSBI1_UART_SRC] = &gsbi1_uart_src.clkr,
	[GSBI1_UART_CLK] = &gsbi1_uart_clk.clkr,
	[GSBI2_UART_SRC] = &gsbi2_uart_src.clkr,
	[GSBI2_UART_CLK] = &gsbi2_uart_clk.clkr,
	[GSBI4_UART_SRC] = &gsbi4_uart_src.clkr,
	[GSBI4_UART_CLK] = &gsbi4_uart_clk.clkr,
	[GSBI5_UART_SRC] = &gsbi5_uart_src.clkr,
	[GSBI5_UART_CLK] = &gsbi5_uart_clk.clkr,
	[GSBI6_UART_SRC] = &gsbi6_uart_src.clkr,
	[GSBI6_UART_CLK] = &gsbi6_uart_clk.clkr,
	[GSBI7_UART_SRC] = &gsbi7_uart_src.clkr,
	[GSBI7_UART_CLK] = &gsbi7_uart_clk.clkr,
	[GSBI1_QUP_SRC] = &gsbi1_qup_src.clkr,
	[GSBI1_QUP_CLK] = &gsbi1_qup_clk.clkr,
	[GSBI2_QUP_SRC] = &gsbi2_qup_src.clkr,
	[GSBI2_QUP_CLK] = &gsbi2_qup_clk.clkr,
	[GSBI4_QUP_SRC] = &gsbi4_qup_src.clkr,
	[GSBI4_QUP_CLK] = &gsbi4_qup_clk.clkr,
	[GSBI5_QUP_SRC] = &gsbi5_qup_src.clkr,
	[GSBI5_QUP_CLK] = &gsbi5_qup_clk.clkr,
	[GSBI6_QUP_SRC] = &gsbi6_qup_src.clkr,
	[GSBI6_QUP_CLK] = &gsbi6_qup_clk.clkr,
	[GSBI7_QUP_SRC] = &gsbi7_qup_src.clkr,
	[GSBI7_QUP_CLK] = &gsbi7_qup_clk.clkr,
	[GP0_SRC] = &gp0_src.clkr,
	[GP0_CLK] = &gp0_clk.clkr,
	[GP1_SRC] = &gp1_src.clkr,
	[GP1_CLK] = &gp1_clk.clkr,
	[GP2_SRC] = &gp2_src.clkr,
	[GP2_CLK] = &gp2_clk.clkr,
	[PMEM_A_CLK] = &pmem_clk.clkr,
	[PRNG_SRC] = &prng_src.clkr,
	[PRNG_CLK] = &prng_clk.clkr,
	[SDC1_SRC] = &sdc1_src.clkr,
	[SDC1_CLK] = &sdc1_clk.clkr,
	[SDC3_SRC] = &sdc3_src.clkr,
	[SDC3_CLK] = &sdc3_clk.clkr,
	[TSIF_REF_SRC] = &tsif_ref_src.clkr,
	[TSIF_REF_CLK] = &tsif_ref_clk.clkr,
	[DMA_BAM_H_CLK] = &dma_bam_h_clk.clkr,
	[GSBI1_H_CLK] = &gsbi1_h_clk.clkr,
	[GSBI2_H_CLK] = &gsbi2_h_clk.clkr,
	[GSBI4_H_CLK] = &gsbi4_h_clk.clkr,
	[GSBI5_H_CLK] = &gsbi5_h_clk.clkr,
	[GSBI6_H_CLK] = &gsbi6_h_clk.clkr,
	[GSBI7_H_CLK] = &gsbi7_h_clk.clkr,
	[TSIF_H_CLK] = &tsif_h_clk.clkr,
	[SDC1_H_CLK] = &sdc1_h_clk.clkr,
	[SDC3_H_CLK] = &sdc3_h_clk.clkr,
	[ADM0_CLK] = &adm0_clk.clkr,
	[ADM0_PBUS_CLK] = &adm0_pbus_clk.clkr,
	[PCIE_A_CLK] = &pcie_a_clk.clkr,
	[PCIE_AUX_CLK] = &pcie_aux_clk.clkr,
	[PCIE_H_CLK] = &pcie_h_clk.clkr,
	[PCIE_PHY_CLK] = &pcie_phy_clk.clkr,
	[SFAB_SATA_S_H_CLK] = &sfab_sata_s_h_clk.clkr,
	[PMIC_ARB0_H_CLK] = &pmic_arb0_h_clk.clkr,
	[PMIC_ARB1_H_CLK] = &pmic_arb1_h_clk.clkr,
	[PMIC_SSBI2_CLK] = &pmic_ssbi2_clk.clkr,
	[RPM_MSG_RAM_H_CLK] = &rpm_msg_ram_h_clk.clkr,
	[SATA_H_CLK] = &sata_h_clk.clkr,
	[SATA_CLK_SRC] = &sata_ref_src.clkr,
	[SATA_RXOOB_CLK] = &sata_rxoob_clk.clkr,
	[SATA_PMALIVE_CLK] = &sata_pmalive_clk.clkr,
	[SATA_PHY_REF_CLK] = &sata_phy_ref_clk.clkr,
	[SATA_A_CLK] = &sata_a_clk.clkr,
	[SATA_PHY_CFG_CLK] = &sata_phy_cfg_clk.clkr,
	[PCIE_ALT_REF_SRC] = &pcie_ref_src.clkr,
	[PCIE_ALT_REF_CLK] = &pcie_ref_src_clk.clkr,
	[PCIE_1_A_CLK] = &pcie1_a_clk.clkr,
	[PCIE_1_AUX_CLK] = &pcie1_aux_clk.clkr,
	[PCIE_1_H_CLK] = &pcie1_h_clk.clkr,
	[PCIE_1_PHY_CLK] = &pcie1_phy_clk.clkr,
	[PCIE_1_ALT_REF_SRC] = &pcie1_ref_src.clkr,
	[PCIE_1_ALT_REF_CLK] = &pcie1_ref_src_clk.clkr,
	[PCIE_2_A_CLK] = &pcie2_a_clk.clkr,
	[PCIE_2_AUX_CLK] = &pcie2_aux_clk.clkr,
	[PCIE_2_H_CLK] = &pcie2_h_clk.clkr,
	[PCIE_2_PHY_CLK] = &pcie2_phy_clk.clkr,
	[PCIE_2_ALT_REF_SRC] = &pcie2_ref_src.clkr,
	[PCIE_2_ALT_REF_CLK] = &pcie2_ref_src_clk.clkr,
	[USB30_MASTER_SRC] = &usb30_master_clk_src.clkr,
	[USB30_0_MASTER_CLK] = &usb30_0_branch_clk.clkr,
	[USB30_1_MASTER_CLK] = &usb30_1_branch_clk.clkr,
	[USB30_UTMI_SRC] = &usb30_utmi_clk.clkr,
	[USB30_0_UTMI_CLK] = &usb30_0_utmi_clk_ctl.clkr,
	[USB30_1_UTMI_CLK] = &usb30_1_utmi_clk_ctl.clkr,
	[USB_HS1_H_CLK] = &usb_hs1_h_clk.clkr,
	[USB_HS1_XCVR_SRC] = &usb_hs1_xcvr_clk_src.clkr,
	[USB_HS1_XCVR_CLK] = &usb_hs1_xcvr_clk.clkr,
	[USB_FS1_H_CLK] = &usb_fs1_h_clk.clkr,
	[USB_FS1_XCVR_SRC] = &usb_fs1_xcvr_clk_src.clkr,
	[USB_FS1_XCVR_CLK] = &usb_fs1_xcvr_clk.clkr,
	[USB_FS1_SYSTEM_CLK] = &usb_fs1_sys_clk.clkr,
	[EBI2_CLK] = &ebi2_clk.clkr,
	[EBI2_AON_CLK] = &ebi2_aon_clk.clkr,
	[GMAC_CORE1_CLK_SRC] = &gmac_core1_src.clkr,
	[GMAC_CORE1_CLK] = &gmac_core1_clk.clkr,
	[GMAC_CORE2_CLK_SRC] = &gmac_core2_src.clkr,
	[GMAC_CORE2_CLK] = &gmac_core2_clk.clkr,
	[GMAC_CORE3_CLK_SRC] = &gmac_core3_src.clkr,
	[GMAC_CORE3_CLK] = &gmac_core3_clk.clkr,
	[GMAC_CORE4_CLK_SRC] = &gmac_core4_src.clkr,
	[GMAC_CORE4_CLK] = &gmac_core4_clk.clkr,
	[UBI32_CORE1_CLK_SRC] = &ubi32_core1_src_clk.clkr,
	[UBI32_CORE2_CLK_SRC] = &ubi32_core2_src_clk.clkr,
	[NSSTCM_CLK_SRC] = &nss_tcm_src.clkr,
	[NSSTCM_CLK] = &nss_tcm_clk.clkr,
	[PLL9] = &hfpll0.clkr,
	[PLL10] = &hfpll1.clkr,
	[PLL12] = &hfpll_l2.clkr,
};

static const struct qcom_reset_map gcc_ipq806x_resets[] = {
	[QDSS_STM_RESET] = { 0x2060, 6 },
	[AFAB_SMPSS_S_RESET] = { 0x20b8, 2 },
	[AFAB_SMPSS_M1_RESET] = { 0x20b8, 1 },
	[AFAB_SMPSS_M0_RESET] = { 0x20b8, 0 },
	[AFAB_EBI1_CH0_RESET] = { 0x20c0, 7 },
	[AFAB_EBI1_CH1_RESET] = { 0x20c4, 7 },
	[SFAB_ADM0_M0_RESET] = { 0x21e0, 7 },
	[SFAB_ADM0_M1_RESET] = { 0x21e4, 7 },
	[SFAB_ADM0_M2_RESET] = { 0x21e8, 7 },
	[ADM0_C2_RESET] = { 0x220c, 4 },
	[ADM0_C1_RESET] = { 0x220c, 3 },
	[ADM0_C0_RESET] = { 0x220c, 2 },
	[ADM0_PBUS_RESET] = { 0x220c, 1 },
	[ADM0_RESET] = { 0x220c, 0 },
	[QDSS_CLKS_SW_RESET] = { 0x2260, 5 },
	[QDSS_POR_RESET] = { 0x2260, 4 },
	[QDSS_TSCTR_RESET] = { 0x2260, 3 },
	[QDSS_HRESET_RESET] = { 0x2260, 2 },
	[QDSS_AXI_RESET] = { 0x2260, 1 },
	[QDSS_DBG_RESET] = { 0x2260, 0 },
	[SFAB_PCIE_M_RESET] = { 0x22d8, 1 },
	[SFAB_PCIE_S_RESET] = { 0x22d8, 0 },
	[PCIE_EXT_RESET] = { 0x22dc, 6 },
	[PCIE_PHY_RESET] = { 0x22dc, 5 },
	[PCIE_PCI_RESET] = { 0x22dc, 4 },
	[PCIE_POR_RESET] = { 0x22dc, 3 },
	[PCIE_HCLK_RESET] = { 0x22dc, 2 },
	[PCIE_ACLK_RESET] = { 0x22dc, 0 },
	[SFAB_LPASS_RESET] = { 0x23a0, 7 },
	[SFAB_AFAB_M_RESET] = { 0x23e0, 7 },
	[AFAB_SFAB_M0_RESET] = { 0x2420, 7 },
	[AFAB_SFAB_M1_RESET] = { 0x2424, 7 },
	[SFAB_SATA_S_RESET] = { 0x2480, 7 },
	[SFAB_DFAB_M_RESET] = { 0x2500, 7 },
	[DFAB_SFAB_M_RESET] = { 0x2520, 7 },
	[DFAB_SWAY0_RESET] = { 0x2540, 7 },
	[DFAB_SWAY1_RESET] = { 0x2544, 7 },
	[DFAB_ARB0_RESET] = { 0x2560, 7 },
	[DFAB_ARB1_RESET] = { 0x2564, 7 },
	[PPSS_PROC_RESET] = { 0x2594, 1 },
	[PPSS_RESET] = { 0x2594, 0 },
	[DMA_BAM_RESET] = { 0x25c0, 7 },
	[SPS_TIC_H_RESET] = { 0x2600, 7 },
	[SFAB_CFPB_M_RESET] = { 0x2680, 7 },
	[SFAB_CFPB_S_RESET] = { 0x26c0, 7 },
	[TSIF_H_RESET] = { 0x2700, 7 },
	[CE1_H_RESET] = { 0x2720, 7 },
	[CE1_CORE_RESET] = { 0x2724, 7 },
	[CE1_SLEEP_RESET] = { 0x2728, 7 },
	[CE2_H_RESET] = { 0x2740, 7 },
	[CE2_CORE_RESET] = { 0x2744, 7 },
	[SFAB_SFPB_M_RESET] = { 0x2780, 7 },
	[SFAB_SFPB_S_RESET] = { 0x27a0, 7 },
	[RPM_PROC_RESET] = { 0x27c0, 7 },
	[PMIC_SSBI2_RESET] = { 0x280c, 12 },
	[SDC1_RESET] = { 0x2830, 0 },
	[SDC2_RESET] = { 0x2850, 0 },
	[SDC3_RESET] = { 0x2870, 0 },
	[SDC4_RESET] = { 0x2890, 0 },
	[USB_HS1_RESET] = { 0x2910, 0 },
	[USB_HSIC_RESET] = { 0x2934, 0 },
	[USB_FS1_XCVR_RESET] = { 0x2974, 1 },
	[USB_FS1_RESET] = { 0x2974, 0 },
	[GSBI1_RESET] = { 0x29dc, 0 },
	[GSBI2_RESET] = { 0x29fc, 0 },
	[GSBI3_RESET] = { 0x2a1c, 0 },
	[GSBI4_RESET] = { 0x2a3c, 0 },
	[GSBI5_RESET] = { 0x2a5c, 0 },
	[GSBI6_RESET] = { 0x2a7c, 0 },
	[GSBI7_RESET] = { 0x2a9c, 0 },
	[SPDM_RESET] = { 0x2b6c, 0 },
	[SEC_CTRL_RESET] = { 0x2b80, 7 },
	[TLMM_H_RESET] = { 0x2ba0, 7 },
	[SFAB_SATA_M_RESET] = { 0x2c18, 0 },
	[SATA_RESET] = { 0x2c1c, 0 },
	[TSSC_RESET] = { 0x2ca0, 7 },
	[PDM_RESET] = { 0x2cc0, 12 },
	[MPM_H_RESET] = { 0x2da0, 7 },
	[MPM_RESET] = { 0x2da4, 0 },
	[SFAB_SMPSS_S_RESET] = { 0x2e00, 7 },
	[PRNG_RESET] = { 0x2e80, 12 },
	[SFAB_CE3_M_RESET] = { 0x36c8, 1 },
	[SFAB_CE3_S_RESET] = { 0x36c8, 0 },
	[CE3_SLEEP_RESET] = { 0x36d0, 7 },
	[PCIE_1_M_RESET] = { 0x3a98, 1 },
	[PCIE_1_S_RESET] = { 0x3a98, 0 },
	[PCIE_1_EXT_RESET] = { 0x3a9c, 6 },
	[PCIE_1_PHY_RESET] = { 0x3a9c, 5 },
	[PCIE_1_PCI_RESET] = { 0x3a9c, 4 },
	[PCIE_1_POR_RESET] = { 0x3a9c, 3 },
	[PCIE_1_HCLK_RESET] = { 0x3a9c, 2 },
	[PCIE_1_ACLK_RESET] = { 0x3a9c, 0 },
	[PCIE_2_M_RESET] = { 0x3ad8, 1 },
	[PCIE_2_S_RESET] = { 0x3ad8, 0 },
	[PCIE_2_EXT_RESET] = { 0x3adc, 6 },
	[PCIE_2_PHY_RESET] = { 0x3adc, 5 },
	[PCIE_2_PCI_RESET] = { 0x3adc, 4 },
	[PCIE_2_POR_RESET] = { 0x3adc, 3 },
	[PCIE_2_HCLK_RESET] = { 0x3adc, 2 },
	[PCIE_2_ACLK_RESET] = { 0x3adc, 0 },
	[SFAB_USB30_S_RESET] = { 0x3b54, 1 },
	[SFAB_USB30_M_RESET] = { 0x3b54, 0 },
	[USB30_0_PORT2_HS_PHY_RESET] = { 0x3b50, 5 },
	[USB30_0_MASTER_RESET] = { 0x3b50, 4 },
	[USB30_0_SLEEP_RESET] = { 0x3b50, 3 },
	[USB30_0_UTMI_PHY_RESET] = { 0x3b50, 2 },
	[USB30_0_POWERON_RESET] = { 0x3b50, 1 },
	[USB30_0_PHY_RESET] = { 0x3b50, 0 },
	[USB30_1_MASTER_RESET] = { 0x3b58, 4 },
	[USB30_1_SLEEP_RESET] = { 0x3b58, 3 },
	[USB30_1_UTMI_PHY_RESET] = { 0x3b58, 2 },
	[USB30_1_POWERON_RESET] = { 0x3b58, 1 },
	[USB30_1_PHY_RESET] = { 0x3b58, 0 },
	[NSSFB0_RESET] = { 0x3b60, 6 },
	[NSSFB1_RESET] = { 0x3b60, 7 },
	[UBI32_CORE1_CLKRST_CLAMP_RESET] = { 0x3d3c, 3},
	[UBI32_CORE1_CLAMP_RESET] = { 0x3d3c, 2 },
	[UBI32_CORE1_AHB_RESET] = { 0x3d3c, 1 },
	[UBI32_CORE1_AXI_RESET] = { 0x3d3c, 0 },
	[UBI32_CORE2_CLKRST_CLAMP_RESET] = { 0x3d5c, 3 },
	[UBI32_CORE2_CLAMP_RESET] = { 0x3d5c, 2 },
	[UBI32_CORE2_AHB_RESET] = { 0x3d5c, 1 },
	[UBI32_CORE2_AXI_RESET] = { 0x3d5c, 0 },
	[GMAC_CORE1_RESET] = { 0x3cbc, 0 },
	[GMAC_CORE2_RESET] = { 0x3cdc, 0 },
	[GMAC_CORE3_RESET] = { 0x3cfc, 0 },
	[GMAC_CORE4_RESET] = { 0x3d1c, 0 },
	[GMAC_AHB_RESET] = { 0x3e24, 0 },
	[NSS_CH0_RST_RX_CLK_N_RESET] = { 0x3b60, 0 },
	[NSS_CH0_RST_TX_CLK_N_RESET] = { 0x3b60, 1 },
	[NSS_CH0_RST_RX_125M_N_RESET] = { 0x3b60, 2 },
	[NSS_CH0_HW_RST_RX_125M_N_RESET] = { 0x3b60, 3 },
	[NSS_CH0_RST_TX_125M_N_RESET] = { 0x3b60, 4 },
	[NSS_CH1_RST_RX_CLK_N_RESET] = { 0x3b60, 5 },
	[NSS_CH1_RST_TX_CLK_N_RESET] = { 0x3b60, 6 },
	[NSS_CH1_RST_RX_125M_N_RESET] = { 0x3b60, 7 },
	[NSS_CH1_HW_RST_RX_125M_N_RESET] = { 0x3b60, 8 },
	[NSS_CH1_RST_TX_125M_N_RESET] = { 0x3b60, 9 },
	[NSS_CH2_RST_RX_CLK_N_RESET] = { 0x3b60, 10 },
	[NSS_CH2_RST_TX_CLK_N_RESET] = { 0x3b60, 11 },
	[NSS_CH2_RST_RX_125M_N_RESET] = { 0x3b60, 12 },
	[NSS_CH2_HW_RST_RX_125M_N_RESET] = { 0x3b60, 13 },
	[NSS_CH2_RST_TX_125M_N_RESET] = { 0x3b60, 14 },
	[NSS_CH3_RST_RX_CLK_N_RESET] = { 0x3b60, 15 },
	[NSS_CH3_RST_TX_CLK_N_RESET] = { 0x3b60, 16 },
	[NSS_CH3_RST_RX_125M_N_RESET] = { 0x3b60, 17 },
	[NSS_CH3_HW_RST_RX_125M_N_RESET] = { 0x3b60, 18 },
	[NSS_CH3_RST_TX_125M_N_RESET] = { 0x3b60, 19 },
	[NSS_RST_RX_250M_125M_N_RESET] = { 0x3b60, 20 },
	[NSS_RST_TX_250M_125M_N_RESET] = { 0x3b60, 21 },
	[NSS_QSGMII_TXPI_RST_N_RESET] = { 0x3b60, 22 },
	[NSS_QSGMII_CDR_RST_N_RESET] = { 0x3b60, 23 },
	[NSS_SGMII2_CDR_RST_N_RESET] = { 0x3b60, 24 },
	[NSS_SGMII3_CDR_RST_N_RESET] = { 0x3b60, 25 },
	[NSS_CAL_PRBS_RST_N_RESET] = { 0x3b60, 26 },
	[NSS_LCKDT_RST_N_RESET] = { 0x3b60, 27 },
	[NSS_SRDS_N_RESET] = { 0x3b60, 28 },
};

static const struct regmap_config gcc_ipq806x_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.max_register	= 0x3e40,
	.fast_io	= true,
};

static const struct qcom_cc_desc gcc_ipq806x_desc = {
	.config = &gcc_ipq806x_regmap_config,
	.clks = gcc_ipq806x_clks,
	.num_clks = ARRAY_SIZE(gcc_ipq806x_clks),
	.resets = gcc_ipq806x_resets,
	.num_resets = ARRAY_SIZE(gcc_ipq806x_resets),
};

static const struct of_device_id gcc_ipq806x_match_table[] = {
	{ .compatible = "qcom,gcc-ipq8064" },
	{ }
};
MODULE_DEVICE_TABLE(of, gcc_ipq806x_match_table);

static int gcc_ipq806x_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct regmap *regmap;
	int ret;

	ret = qcom_cc_register_board_clk(dev, "cxo_board", "cxo", 25000000);
	if (ret)
		return ret;

	ret = qcom_cc_register_board_clk(dev, "pxo_board", "pxo", 25000000);
	if (ret)
		return ret;

	ret = qcom_cc_probe(pdev, &gcc_ipq806x_desc);
	if (ret)
		return ret;

	regmap = dev_get_regmap(dev, NULL);
	if (!regmap)
		return -ENODEV;

	/* Setup PLL18 static bits */
	regmap_update_bits(regmap, 0x31a4, 0xffffffc0, 0x40000400);
	regmap_write(regmap, 0x31b0, 0x3080);

	/* Set GMAC footswitch sleep/wakeup values */
	regmap_write(regmap, 0x3cb8, 8);
	regmap_write(regmap, 0x3cd8, 8);
	regmap_write(regmap, 0x3cf8, 8);
	regmap_write(regmap, 0x3d18, 8);

	return of_platform_populate(pdev->dev.of_node, NULL, NULL, &pdev->dev);
}

static struct platform_driver gcc_ipq806x_driver = {
	.probe		= gcc_ipq806x_probe,
	.driver		= {
		.name	= "gcc-ipq806x",
		.of_match_table = gcc_ipq806x_match_table,
	},
};

static int __init gcc_ipq806x_init(void)
{
	return platform_driver_register(&gcc_ipq806x_driver);
}
core_initcall(gcc_ipq806x_init);

static void __exit gcc_ipq806x_exit(void)
{
	platform_driver_unregister(&gcc_ipq806x_driver);
}
module_exit(gcc_ipq806x_exit);

MODULE_DESCRIPTION("QCOM GCC IPQ806x Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:gcc-ipq806x");
