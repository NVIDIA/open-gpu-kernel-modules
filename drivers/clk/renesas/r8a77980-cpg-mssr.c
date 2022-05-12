// SPDX-License-Identifier: GPL-2.0
/*
 * r8a77980 Clock Pulse Generator / Module Standby and Software Reset
 *
 * Copyright (C) 2018 Renesas Electronics Corp.
 * Copyright (C) 2018 Cogent Embedded, Inc.
 *
 * Based on r8a7795-cpg-mssr.c
 *
 * Copyright (C) 2015 Glider bvba
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/soc/renesas/rcar-rst.h>
#include <linux/sys_soc.h>

#include <dt-bindings/clock/r8a77980-cpg-mssr.h>

#include "renesas-cpg-mssr.h"
#include "rcar-gen3-cpg.h"

enum clk_ids {
	/* Core Clock Outputs exported to DT */
	LAST_DT_CORE_CLK = R8A77980_CLK_OSC,

	/* External Input Clocks */
	CLK_EXTAL,
	CLK_EXTALR,

	/* Internal Core Clocks */
	CLK_MAIN,
	CLK_PLL1,
	CLK_PLL2,
	CLK_PLL3,
	CLK_PLL1_DIV2,
	CLK_PLL1_DIV4,
	CLK_S0,
	CLK_S1,
	CLK_S2,
	CLK_S3,
	CLK_SDSRC,
	CLK_RPCSRC,
	CLK_OCO,

	/* Module Clocks */
	MOD_CLK_BASE
};

static const struct cpg_core_clk r8a77980_core_clks[] __initconst = {
	/* External Clock Inputs */
	DEF_INPUT("extal",  CLK_EXTAL),
	DEF_INPUT("extalr", CLK_EXTALR),

	/* Internal Core Clocks */
	DEF_BASE(".main",       CLK_MAIN, CLK_TYPE_GEN3_MAIN, CLK_EXTAL),
	DEF_BASE(".pll1",       CLK_PLL1, CLK_TYPE_GEN3_PLL1, CLK_MAIN),
	DEF_BASE(".pll2",       CLK_PLL2, CLK_TYPE_GEN3_PLL2, CLK_MAIN),
	DEF_BASE(".pll3",       CLK_PLL3, CLK_TYPE_GEN3_PLL3, CLK_MAIN),

	DEF_FIXED(".pll1_div2", CLK_PLL1_DIV2,	   CLK_PLL1,       2, 1),
	DEF_FIXED(".pll1_div4", CLK_PLL1_DIV4,	   CLK_PLL1_DIV2,  2, 1),
	DEF_FIXED(".s0",	CLK_S0,		   CLK_PLL1_DIV2,  2, 1),
	DEF_FIXED(".s1",	CLK_S1,		   CLK_PLL1_DIV2,  3, 1),
	DEF_FIXED(".s2",	CLK_S2,		   CLK_PLL1_DIV2,  4, 1),
	DEF_FIXED(".s3",	CLK_S3,		   CLK_PLL1_DIV2,  6, 1),
	DEF_FIXED(".sdsrc",	CLK_SDSRC,	   CLK_PLL1_DIV2,  2, 1),
	DEF_BASE(".rpcsrc",	CLK_RPCSRC, CLK_TYPE_GEN3_RPCSRC, CLK_PLL1),
	DEF_RATE(".oco",	CLK_OCO,           32768),

	DEF_BASE("rpc",		R8A77980_CLK_RPC, CLK_TYPE_GEN3_RPC,
		 CLK_RPCSRC),
	DEF_BASE("rpcd2",	R8A77980_CLK_RPCD2, CLK_TYPE_GEN3_RPCD2,
		 R8A77980_CLK_RPC),

	/* Core Clock Outputs */
	DEF_FIXED("ztr",	R8A77980_CLK_ZTR,   CLK_PLL1_DIV2,  6, 1),
	DEF_FIXED("ztrd2",	R8A77980_CLK_ZTRD2, CLK_PLL1_DIV2, 12, 1),
	DEF_FIXED("zt",		R8A77980_CLK_ZT,    CLK_PLL1_DIV2,  4, 1),
	DEF_FIXED("zx",		R8A77980_CLK_ZX,    CLK_PLL1_DIV2,  2, 1),
	DEF_FIXED("s0d1",	R8A77980_CLK_S0D1,  CLK_S0,         1, 1),
	DEF_FIXED("s0d2",	R8A77980_CLK_S0D2,  CLK_S0,         2, 1),
	DEF_FIXED("s0d3",	R8A77980_CLK_S0D3,  CLK_S0,         3, 1),
	DEF_FIXED("s0d4",	R8A77980_CLK_S0D4,  CLK_S0,         4, 1),
	DEF_FIXED("s0d6",	R8A77980_CLK_S0D6,  CLK_S0,         6, 1),
	DEF_FIXED("s0d12",	R8A77980_CLK_S0D12, CLK_S0,        12, 1),
	DEF_FIXED("s0d24",	R8A77980_CLK_S0D24, CLK_S0,        24, 1),
	DEF_FIXED("s1d1",	R8A77980_CLK_S1D1,  CLK_S1,         1, 1),
	DEF_FIXED("s1d2",	R8A77980_CLK_S1D2,  CLK_S1,         2, 1),
	DEF_FIXED("s1d4",	R8A77980_CLK_S1D4,  CLK_S1,         4, 1),
	DEF_FIXED("s2d1",	R8A77980_CLK_S2D1,  CLK_S2,         1, 1),
	DEF_FIXED("s2d2",	R8A77980_CLK_S2D2,  CLK_S2,         2, 1),
	DEF_FIXED("s2d4",	R8A77980_CLK_S2D4,  CLK_S2,         4, 1),
	DEF_FIXED("s3d1",	R8A77980_CLK_S3D1,  CLK_S3,         1, 1),
	DEF_FIXED("s3d2",	R8A77980_CLK_S3D2,  CLK_S3,         2, 1),
	DEF_FIXED("s3d4",	R8A77980_CLK_S3D4,  CLK_S3,         4, 1),

	DEF_GEN3_SD("sd0",	R8A77980_CLK_SD0,   CLK_SDSRC,	  0x0074),

	DEF_FIXED("cl",		R8A77980_CLK_CL,    CLK_PLL1_DIV2, 48, 1),
	DEF_FIXED("cp",		R8A77980_CLK_CP,    CLK_EXTAL,	    2, 1),
	DEF_FIXED("cpex",	R8A77980_CLK_CPEX,  CLK_EXTAL,	    2, 1),

	DEF_DIV6P1("canfd",	R8A77980_CLK_CANFD, CLK_PLL1_DIV4, 0x244),
	DEF_DIV6P1("csi0",	R8A77980_CLK_CSI0,  CLK_PLL1_DIV4, 0x00c),
	DEF_DIV6P1("mso",	R8A77980_CLK_MSO,   CLK_PLL1_DIV4, 0x014),

	DEF_GEN3_OSC("osc",	R8A77980_CLK_OSC,   CLK_EXTAL,     8),
	DEF_GEN3_MDSEL("r",	R8A77980_CLK_R, 29, CLK_EXTALR, 1, CLK_OCO, 1),
};

static const struct mssr_mod_clk r8a77980_mod_clks[] __initconst = {
	DEF_MOD("tmu4",			 121,	R8A77980_CLK_S0D6),
	DEF_MOD("tmu3",			 122,	R8A77980_CLK_S0D6),
	DEF_MOD("tmu2",			 123,	R8A77980_CLK_S0D6),
	DEF_MOD("tmu1",			 124,	R8A77980_CLK_S0D6),
	DEF_MOD("tmu0",			 125,	R8A77980_CLK_CP),
	DEF_MOD("scif4",		 203,	R8A77980_CLK_S3D4),
	DEF_MOD("scif3",		 204,	R8A77980_CLK_S3D4),
	DEF_MOD("scif1",		 206,	R8A77980_CLK_S3D4),
	DEF_MOD("scif0",		 207,	R8A77980_CLK_S3D4),
	DEF_MOD("msiof3",		 208,	R8A77980_CLK_MSO),
	DEF_MOD("msiof2",		 209,	R8A77980_CLK_MSO),
	DEF_MOD("msiof1",		 210,	R8A77980_CLK_MSO),
	DEF_MOD("msiof0",		 211,	R8A77980_CLK_MSO),
	DEF_MOD("sys-dmac2",		 217,	R8A77980_CLK_S0D3),
	DEF_MOD("sys-dmac1",		 218,	R8A77980_CLK_S0D3),
	DEF_MOD("cmt3",			 300,	R8A77980_CLK_R),
	DEF_MOD("cmt2",			 301,	R8A77980_CLK_R),
	DEF_MOD("cmt1",			 302,	R8A77980_CLK_R),
	DEF_MOD("cmt0",			 303,	R8A77980_CLK_R),
	DEF_MOD("tpu0",			 304,	R8A77980_CLK_S3D4),
	DEF_MOD("sdif",			 314,	R8A77980_CLK_SD0),
	DEF_MOD("pciec0",		 319,	R8A77980_CLK_S2D2),
	DEF_MOD("rwdt",			 402,	R8A77980_CLK_R),
	DEF_MOD("intc-ex",		 407,	R8A77980_CLK_CP),
	DEF_MOD("intc-ap",		 408,	R8A77980_CLK_S0D3),
	DEF_MOD("hscif3",		 517,	R8A77980_CLK_S3D1),
	DEF_MOD("hscif2",		 518,	R8A77980_CLK_S3D1),
	DEF_MOD("hscif1",		 519,	R8A77980_CLK_S3D1),
	DEF_MOD("hscif0",		 520,	R8A77980_CLK_S3D1),
	DEF_MOD("imp4",			 521,	R8A77980_CLK_S1D1),
	DEF_MOD("thermal",		 522,	R8A77980_CLK_CP),
	DEF_MOD("pwm",			 523,	R8A77980_CLK_S0D12),
	DEF_MOD("impdma1",		 526,	R8A77980_CLK_S1D1),
	DEF_MOD("impdma0",		 527,	R8A77980_CLK_S1D1),
	DEF_MOD("imp-ocv4",		 528,	R8A77980_CLK_S1D1),
	DEF_MOD("imp-ocv3",		 529,	R8A77980_CLK_S1D1),
	DEF_MOD("imp-ocv2",		 531,	R8A77980_CLK_S1D1),
	DEF_MOD("fcpvd0",		 603,	R8A77980_CLK_S3D1),
	DEF_MOD("vspd0",		 623,	R8A77980_CLK_S3D1),
	DEF_MOD("csi41",		 715,	R8A77980_CLK_CSI0),
	DEF_MOD("csi40",		 716,	R8A77980_CLK_CSI0),
	DEF_MOD("du0",			 724,	R8A77980_CLK_S2D1),
	DEF_MOD("lvds",			 727,	R8A77980_CLK_S2D1),
	DEF_MOD("etheravb",		 812,	R8A77980_CLK_S3D2),
	DEF_MOD("gether",		 813,	R8A77980_CLK_S3D2),
	DEF_MOD("imp3",			 824,	R8A77980_CLK_S1D1),
	DEF_MOD("imp2",			 825,	R8A77980_CLK_S1D1),
	DEF_MOD("imp1",			 826,	R8A77980_CLK_S1D1),
	DEF_MOD("imp0",			 827,	R8A77980_CLK_S1D1),
	DEF_MOD("imp-ocv1",		 828,	R8A77980_CLK_S1D1),
	DEF_MOD("imp-ocv0",		 829,	R8A77980_CLK_S1D1),
	DEF_MOD("impram",		 830,	R8A77980_CLK_S1D1),
	DEF_MOD("impcnn",		 831,	R8A77980_CLK_S1D1),
	DEF_MOD("gpio5",		 907,	R8A77980_CLK_CP),
	DEF_MOD("gpio4",		 908,	R8A77980_CLK_CP),
	DEF_MOD("gpio3",		 909,	R8A77980_CLK_CP),
	DEF_MOD("gpio2",		 910,	R8A77980_CLK_CP),
	DEF_MOD("gpio1",		 911,	R8A77980_CLK_CP),
	DEF_MOD("gpio0",		 912,	R8A77980_CLK_CP),
	DEF_MOD("can-fd",		 914,	R8A77980_CLK_S3D2),
	DEF_MOD("rpc-if",		 917,	R8A77980_CLK_RPCD2),
	DEF_MOD("i2c4",			 927,	R8A77980_CLK_S0D6),
	DEF_MOD("i2c3",			 928,	R8A77980_CLK_S0D6),
	DEF_MOD("i2c2",			 929,	R8A77980_CLK_S3D2),
	DEF_MOD("i2c1",			 930,	R8A77980_CLK_S3D2),
	DEF_MOD("i2c0",			 931,	R8A77980_CLK_S3D2),
};

static const unsigned int r8a77980_crit_mod_clks[] __initconst = {
	MOD_CLK_ID(402),	/* RWDT */
	MOD_CLK_ID(408),	/* INTC-AP (GIC) */
};

/*
 * CPG Clock Data
 */

/*
 *   MD		EXTAL		PLL2	PLL1	PLL3	OSC
 * 14 13	(MHz)
 * --------------------------------------------------------
 * 0  0		16.66 x 1	x240	x192	x192	/16
 * 0  1		20    x 1	x200	x160	x160	/19
 * 1  0		27    x 1	x148	x118	x118	/26
 * 1  1		33.33 / 2	x240	x192	x192	/32
 */
#define CPG_PLL_CONFIG_INDEX(md)	((((md) & BIT(14)) >> 13) | \
					 (((md) & BIT(13)) >> 13))

static const struct rcar_gen3_cpg_pll_config cpg_pll_configs[4] __initconst = {
	/* EXTAL div	PLL1 mult/div	PLL3 mult/div	OSC prediv */
	{ 1,		192,	1,	192,	1,	16,	},
	{ 1,		160,	1,	160,	1,	19,	},
	{ 1,		118,	1,	118,	1,	26,	},
	{ 2,		192,	1,	192,	1,	32,	},
};

static int __init r8a77980_cpg_mssr_init(struct device *dev)
{
	const struct rcar_gen3_cpg_pll_config *cpg_pll_config;
	u32 cpg_mode;
	int error;

	error = rcar_rst_read_mode_pins(&cpg_mode);
	if (error)
		return error;

	cpg_pll_config = &cpg_pll_configs[CPG_PLL_CONFIG_INDEX(cpg_mode)];

	return rcar_gen3_cpg_init(cpg_pll_config, CLK_EXTALR, cpg_mode);
}

const struct cpg_mssr_info r8a77980_cpg_mssr_info __initconst = {
	/* Core Clocks */
	.core_clks = r8a77980_core_clks,
	.num_core_clks = ARRAY_SIZE(r8a77980_core_clks),
	.last_dt_core_clk = LAST_DT_CORE_CLK,
	.num_total_core_clks = MOD_CLK_BASE,

	/* Module Clocks */
	.mod_clks = r8a77980_mod_clks,
	.num_mod_clks = ARRAY_SIZE(r8a77980_mod_clks),
	.num_hw_mod_clks = 12 * 32,

	/* Critical Module Clocks */
	.crit_mod_clks = r8a77980_crit_mod_clks,
	.num_crit_mod_clks = ARRAY_SIZE(r8a77980_crit_mod_clks),

	/* Callbacks */
	.init = r8a77980_cpg_mssr_init,
	.cpg_clk_register = rcar_gen3_cpg_clk_register,
};
