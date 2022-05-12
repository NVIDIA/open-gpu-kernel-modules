// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012, 2013, NVIDIA CORPORATION.  All rights reserved.
 */

#include <linux/io.h>
#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/delay.h>
#include <linux/export.h>
#include <linux/clk/tegra.h>

#include "clk.h"
#include "clk-id.h"

#define PLLX_BASE 0xe0
#define PLLX_MISC 0xe4
#define PLLX_MISC2 0x514
#define PLLX_MISC3 0x518

#define CCLKG_BURST_POLICY 0x368
#define CCLKLP_BURST_POLICY 0x370
#define SCLK_BURST_POLICY 0x028
#define SYSTEM_CLK_RATE 0x030
#define SCLK_DIVIDER 0x2c

static DEFINE_SPINLOCK(sysrate_lock);

enum tegra_super_gen {
	gen4 = 4,
	gen5,
};

struct tegra_super_gen_info {
	enum tegra_super_gen gen;
	const char **sclk_parents;
	const char **cclk_g_parents;
	const char **cclk_lp_parents;
	int num_sclk_parents;
	int num_cclk_g_parents;
	int num_cclk_lp_parents;
};

static const char *sclk_parents[] = { "clk_m", "pll_c_out1", "pll_p_out4",
			       "pll_p", "pll_p_out2", "unused",
			       "clk_32k", "pll_m_out1" };

static const char *cclk_g_parents[] = { "clk_m", "pll_c", "clk_32k", "pll_m",
					"pll_p", "pll_p_out4", "unused",
					"unused", "pll_x", "unused", "unused",
					"unused", "unused", "unused", "unused",
					"dfllCPU_out" };

static const char *cclk_lp_parents[] = { "clk_m", "pll_c", "clk_32k", "pll_m",
					 "pll_p", "pll_p_out4", "unused",
					 "unused", "pll_x", "pll_x_out0" };

static const struct tegra_super_gen_info tegra_super_gen_info_gen4 = {
	.gen = gen4,
	.sclk_parents = sclk_parents,
	.cclk_g_parents = cclk_g_parents,
	.cclk_lp_parents = cclk_lp_parents,
	.num_sclk_parents = ARRAY_SIZE(sclk_parents),
	.num_cclk_g_parents = ARRAY_SIZE(cclk_g_parents),
	.num_cclk_lp_parents = ARRAY_SIZE(cclk_lp_parents),
};

static const char *sclk_parents_gen5[] = { "clk_m", "pll_c_out1", "pll_c4_out3",
			       "pll_p", "pll_p_out2", "pll_c4_out1",
			       "clk_32k", "pll_c4_out2" };

static const char *cclk_g_parents_gen5[] = { "clk_m", "unused", "clk_32k", "unused",
					"pll_p", "pll_p_out4", "unused",
					"unused", "pll_x", "unused", "unused",
					"unused", "unused", "unused", "unused",
					"dfllCPU_out" };

static const char *cclk_lp_parents_gen5[] = { "clk_m", "unused", "clk_32k", "unused",
					"pll_p", "pll_p_out4", "unused",
					"unused", "pll_x", "unused", "unused",
					"unused", "unused", "unused", "unused",
					"dfllCPU_out" };

static const struct tegra_super_gen_info tegra_super_gen_info_gen5 = {
	.gen = gen5,
	.sclk_parents = sclk_parents_gen5,
	.cclk_g_parents = cclk_g_parents_gen5,
	.cclk_lp_parents = cclk_lp_parents_gen5,
	.num_sclk_parents = ARRAY_SIZE(sclk_parents_gen5),
	.num_cclk_g_parents = ARRAY_SIZE(cclk_g_parents_gen5),
	.num_cclk_lp_parents = ARRAY_SIZE(cclk_lp_parents_gen5),
};

static void __init tegra_sclk_init(void __iomem *clk_base,
				struct tegra_clk *tegra_clks,
				const struct tegra_super_gen_info *gen_info)
{
	struct clk *clk;
	struct clk **dt_clk;

	/* SCLK_MUX */
	dt_clk = tegra_lookup_dt_id(tegra_clk_sclk_mux, tegra_clks);
	if (dt_clk) {
		clk = tegra_clk_register_super_mux("sclk_mux",
						gen_info->sclk_parents,
						gen_info->num_sclk_parents,
						CLK_SET_RATE_PARENT,
						clk_base + SCLK_BURST_POLICY,
						0, 4, 0, 0, NULL);
		*dt_clk = clk;


		/* SCLK */
		dt_clk = tegra_lookup_dt_id(tegra_clk_sclk, tegra_clks);
		if (dt_clk) {
			clk = clk_register_divider(NULL, "sclk", "sclk_mux",
						CLK_IS_CRITICAL,
						clk_base + SCLK_DIVIDER, 0, 8,
						0, &sysrate_lock);
			*dt_clk = clk;
		}
	} else {
		/* SCLK */
		dt_clk = tegra_lookup_dt_id(tegra_clk_sclk, tegra_clks);
		if (dt_clk) {
			clk = tegra_clk_register_super_mux("sclk",
						gen_info->sclk_parents,
						gen_info->num_sclk_parents,
						CLK_SET_RATE_PARENT |
						CLK_IS_CRITICAL,
						clk_base + SCLK_BURST_POLICY,
						0, 4, 0, 0, NULL);
			*dt_clk = clk;
		}
	}

	/* HCLK */
	dt_clk = tegra_lookup_dt_id(tegra_clk_hclk, tegra_clks);
	if (dt_clk) {
		clk = clk_register_divider(NULL, "hclk_div", "sclk", 0,
				   clk_base + SYSTEM_CLK_RATE, 4, 2, 0,
				   &sysrate_lock);
		clk = clk_register_gate(NULL, "hclk", "hclk_div",
				CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
				clk_base + SYSTEM_CLK_RATE,
				7, CLK_GATE_SET_TO_DISABLE, &sysrate_lock);
		*dt_clk = clk;
	}

	/* PCLK */
	dt_clk = tegra_lookup_dt_id(tegra_clk_pclk, tegra_clks);
	if (!dt_clk)
		return;

	clk = clk_register_divider(NULL, "pclk_div", "hclk", 0,
				   clk_base + SYSTEM_CLK_RATE, 0, 2, 0,
				   &sysrate_lock);
	clk = clk_register_gate(NULL, "pclk", "pclk_div", CLK_SET_RATE_PARENT |
				CLK_IS_CRITICAL, clk_base + SYSTEM_CLK_RATE,
				3, CLK_GATE_SET_TO_DISABLE, &sysrate_lock);
	*dt_clk = clk;
}

static void __init tegra_super_clk_init(void __iomem *clk_base,
				void __iomem *pmc_base,
				struct tegra_clk *tegra_clks,
				struct tegra_clk_pll_params *params,
				const struct tegra_super_gen_info *gen_info)
{
	struct clk *clk;
	struct clk **dt_clk;

	/* CCLKG */
	dt_clk = tegra_lookup_dt_id(tegra_clk_cclk_g, tegra_clks);
	if (dt_clk) {
		if (gen_info->gen == gen5) {
			clk = tegra_clk_register_super_mux("cclk_g",
					gen_info->cclk_g_parents,
					gen_info->num_cclk_g_parents,
					CLK_SET_RATE_PARENT,
					clk_base + CCLKG_BURST_POLICY,
					TEGRA210_CPU_CLK, 4, 8, 0, NULL);
		} else {
			clk = tegra_clk_register_super_mux("cclk_g",
					gen_info->cclk_g_parents,
					gen_info->num_cclk_g_parents,
					CLK_SET_RATE_PARENT,
					clk_base + CCLKG_BURST_POLICY,
					0, 4, 0, 0, NULL);
		}
		*dt_clk = clk;
	}

	/* CCLKLP */
	dt_clk = tegra_lookup_dt_id(tegra_clk_cclk_lp, tegra_clks);
	if (dt_clk) {
		if (gen_info->gen == gen5) {
			/*
			 * TEGRA210_CPU_CLK flag is not needed for cclk_lp as
			 * cluster switching is not currently supported on
			 * Tegra210 and also cpu_lp is not used.
			 */
			clk = tegra_clk_register_super_mux("cclk_lp",
					gen_info->cclk_lp_parents,
					gen_info->num_cclk_lp_parents,
					CLK_SET_RATE_PARENT,
					clk_base + CCLKLP_BURST_POLICY,
					0, 4, 8, 0, NULL);
		} else {
			clk = tegra_clk_register_super_mux("cclk_lp",
					gen_info->cclk_lp_parents,
					gen_info->num_cclk_lp_parents,
					CLK_SET_RATE_PARENT,
					clk_base + CCLKLP_BURST_POLICY,
					TEGRA_DIVIDER_2, 4, 8, 9, NULL);
		}
		*dt_clk = clk;
	}

	tegra_sclk_init(clk_base, tegra_clks, gen_info);

#if defined(CONFIG_ARCH_TEGRA_114_SOC) || \
    defined(CONFIG_ARCH_TEGRA_124_SOC) || \
    defined(CONFIG_ARCH_TEGRA_210_SOC)
	/* PLLX */
	dt_clk = tegra_lookup_dt_id(tegra_clk_pll_x, tegra_clks);
	if (!dt_clk)
		return;

#if defined(CONFIG_ARCH_TEGRA_210_SOC)
	if (gen_info->gen == gen5)
		clk = tegra_clk_register_pllc_tegra210("pll_x", "pll_ref",
			clk_base, pmc_base, CLK_IGNORE_UNUSED, params, NULL);
	else
#endif
		clk = tegra_clk_register_pllxc("pll_x", "pll_ref", clk_base,
				pmc_base, CLK_IGNORE_UNUSED, params, NULL);

	*dt_clk = clk;

	/* PLLX_OUT0 */

	dt_clk = tegra_lookup_dt_id(tegra_clk_pll_x_out0, tegra_clks);
	if (!dt_clk)
		return;
	clk = clk_register_fixed_factor(NULL, "pll_x_out0", "pll_x",
					CLK_SET_RATE_PARENT, 1, 2);
	*dt_clk = clk;
#endif
}

void __init tegra_super_clk_gen4_init(void __iomem *clk_base,
				void __iomem *pmc_base,
				struct tegra_clk *tegra_clks,
				struct tegra_clk_pll_params *params)
{
	tegra_super_clk_init(clk_base, pmc_base, tegra_clks, params,
			     &tegra_super_gen_info_gen4);
}

void __init tegra_super_clk_gen5_init(void __iomem *clk_base,
				void __iomem *pmc_base,
				struct tegra_clk *tegra_clks,
				struct tegra_clk_pll_params *params)
{
	tegra_super_clk_init(clk_base, pmc_base, tegra_clks, params,
			     &tegra_super_gen_info_gen5);
}
