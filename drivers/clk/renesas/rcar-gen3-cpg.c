// SPDX-License-Identifier: GPL-2.0
/*
 * R-Car Gen3 Clock Pulse Generator
 *
 * Copyright (C) 2015-2018 Glider bvba
 * Copyright (C) 2019 Renesas Electronics Corp.
 *
 * Based on clk-rcar-gen3.c
 *
 * Copyright (C) 2015 Renesas Electronics Corp.
 */

#include <linux/bug.h>
#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>

#include "renesas-cpg-mssr.h"
#include "rcar-cpg-lib.h"
#include "rcar-gen3-cpg.h"

#define CPG_PLL0CR		0x00d8
#define CPG_PLL2CR		0x002c
#define CPG_PLL4CR		0x01f4

#define CPG_RCKCR_CKSEL	BIT(15)	/* RCLK Clock Source Select */

/*
 * Z Clock & Z2 Clock
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is adjustable.  clk->rate = (parent->rate * mult / 32 ) / 2
 * parent - fixed parent.  No clk_set_parent support
 */
#define CPG_FRQCRB			0x00000004
#define CPG_FRQCRB_KICK			BIT(31)
#define CPG_FRQCRC			0x000000e0

struct cpg_z_clk {
	struct clk_hw hw;
	void __iomem *reg;
	void __iomem *kick_reg;
	unsigned long mask;
	unsigned int fixed_div;
};

#define to_z_clk(_hw)	container_of(_hw, struct cpg_z_clk, hw)

static unsigned long cpg_z_clk_recalc_rate(struct clk_hw *hw,
					   unsigned long parent_rate)
{
	struct cpg_z_clk *zclk = to_z_clk(hw);
	unsigned int mult;
	u32 val;

	val = readl(zclk->reg) & zclk->mask;
	mult = 32 - (val >> __ffs(zclk->mask));

	return DIV_ROUND_CLOSEST_ULL((u64)parent_rate * mult,
				     32 * zclk->fixed_div);
}

static int cpg_z_clk_determine_rate(struct clk_hw *hw,
				    struct clk_rate_request *req)
{
	struct cpg_z_clk *zclk = to_z_clk(hw);
	unsigned int min_mult, max_mult, mult;
	unsigned long prate;

	prate = req->best_parent_rate / zclk->fixed_div;
	min_mult = max(div64_ul(req->min_rate * 32ULL, prate), 1ULL);
	max_mult = min(div64_ul(req->max_rate * 32ULL, prate), 32ULL);
	if (max_mult < min_mult)
		return -EINVAL;

	mult = div64_ul(req->rate * 32ULL, prate);
	mult = clamp(mult, min_mult, max_mult);

	req->rate = div_u64((u64)prate * mult, 32);
	return 0;
}

static int cpg_z_clk_set_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long parent_rate)
{
	struct cpg_z_clk *zclk = to_z_clk(hw);
	unsigned int mult;
	unsigned int i;

	mult = DIV64_U64_ROUND_CLOSEST(rate * 32ULL * zclk->fixed_div,
				       parent_rate);
	mult = clamp(mult, 1U, 32U);

	if (readl(zclk->kick_reg) & CPG_FRQCRB_KICK)
		return -EBUSY;

	cpg_reg_modify(zclk->reg, zclk->mask,
		       ((32 - mult) << __ffs(zclk->mask)) & zclk->mask);

	/*
	 * Set KICK bit in FRQCRB to update hardware setting and wait for
	 * clock change completion.
	 */
	cpg_reg_modify(zclk->kick_reg, 0, CPG_FRQCRB_KICK);

	/*
	 * Note: There is no HW information about the worst case latency.
	 *
	 * Using experimental measurements, it seems that no more than
	 * ~10 iterations are needed, independently of the CPU rate.
	 * Since this value might be dependent of external xtal rate, pll1
	 * rate or even the other emulation clocks rate, use 1000 as a
	 * "super" safe value.
	 */
	for (i = 1000; i; i--) {
		if (!(readl(zclk->kick_reg) & CPG_FRQCRB_KICK))
			return 0;

		cpu_relax();
	}

	return -ETIMEDOUT;
}

static const struct clk_ops cpg_z_clk_ops = {
	.recalc_rate = cpg_z_clk_recalc_rate,
	.determine_rate = cpg_z_clk_determine_rate,
	.set_rate = cpg_z_clk_set_rate,
};

static struct clk * __init cpg_z_clk_register(const char *name,
					      const char *parent_name,
					      void __iomem *reg,
					      unsigned int div,
					      unsigned int offset)
{
	struct clk_init_data init = {};
	struct cpg_z_clk *zclk;
	struct clk *clk;

	zclk = kzalloc(sizeof(*zclk), GFP_KERNEL);
	if (!zclk)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &cpg_z_clk_ops;
	init.flags = 0;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	zclk->reg = reg + CPG_FRQCRC;
	zclk->kick_reg = reg + CPG_FRQCRB;
	zclk->hw.init = &init;
	zclk->mask = GENMASK(offset + 4, offset);
	zclk->fixed_div = div; /* PLLVCO x 1/div x SYS-CPU divider */

	clk = clk_register(NULL, &zclk->hw);
	if (IS_ERR(clk))
		kfree(zclk);

	return clk;
}

struct rpc_clock {
	struct clk_divider div;
	struct clk_gate gate;
	/*
	 * One notifier covers both RPC and RPCD2 clocks as they are both
	 * controlled by the same RPCCKCR register...
	 */
	struct cpg_simple_notifier csn;
};

static const struct clk_div_table cpg_rpcsrc_div_table[] = {
	{ 2, 5 }, { 3, 6 }, { 0, 0 },
};

static const struct clk_div_table cpg_rpc_div_table[] = {
	{ 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 }, { 0, 0 },
};

static struct clk * __init cpg_rpc_clk_register(const char *name,
	void __iomem *base, const char *parent_name,
	struct raw_notifier_head *notifiers)
{
	struct rpc_clock *rpc;
	struct clk *clk;

	rpc = kzalloc(sizeof(*rpc), GFP_KERNEL);
	if (!rpc)
		return ERR_PTR(-ENOMEM);

	rpc->div.reg = base + CPG_RPCCKCR;
	rpc->div.width = 3;
	rpc->div.table = cpg_rpc_div_table;
	rpc->div.lock = &cpg_lock;

	rpc->gate.reg = base + CPG_RPCCKCR;
	rpc->gate.bit_idx = 8;
	rpc->gate.flags = CLK_GATE_SET_TO_DISABLE;
	rpc->gate.lock = &cpg_lock;

	rpc->csn.reg = base + CPG_RPCCKCR;

	clk = clk_register_composite(NULL, name, &parent_name, 1, NULL, NULL,
				     &rpc->div.hw,  &clk_divider_ops,
				     &rpc->gate.hw, &clk_gate_ops,
				     CLK_SET_RATE_PARENT);
	if (IS_ERR(clk)) {
		kfree(rpc);
		return clk;
	}

	cpg_simple_notifier_register(notifiers, &rpc->csn);
	return clk;
}

struct rpcd2_clock {
	struct clk_fixed_factor fixed;
	struct clk_gate gate;
};

static struct clk * __init cpg_rpcd2_clk_register(const char *name,
						  void __iomem *base,
						  const char *parent_name)
{
	struct rpcd2_clock *rpcd2;
	struct clk *clk;

	rpcd2 = kzalloc(sizeof(*rpcd2), GFP_KERNEL);
	if (!rpcd2)
		return ERR_PTR(-ENOMEM);

	rpcd2->fixed.mult = 1;
	rpcd2->fixed.div = 2;

	rpcd2->gate.reg = base + CPG_RPCCKCR;
	rpcd2->gate.bit_idx = 9;
	rpcd2->gate.flags = CLK_GATE_SET_TO_DISABLE;
	rpcd2->gate.lock = &cpg_lock;

	clk = clk_register_composite(NULL, name, &parent_name, 1, NULL, NULL,
				     &rpcd2->fixed.hw, &clk_fixed_factor_ops,
				     &rpcd2->gate.hw, &clk_gate_ops,
				     CLK_SET_RATE_PARENT);
	if (IS_ERR(clk))
		kfree(rpcd2);

	return clk;
}


static const struct rcar_gen3_cpg_pll_config *cpg_pll_config __initdata;
static unsigned int cpg_clk_extalr __initdata;
static u32 cpg_mode __initdata;
static u32 cpg_quirks __initdata;

#define PLL_ERRATA	BIT(0)		/* Missing PLL0/2/4 post-divider */
#define RCKCR_CKSEL	BIT(1)		/* Manual RCLK parent selection */
#define SD_SKIP_FIRST	BIT(2)		/* Skip first clock in SD table */


static const struct soc_device_attribute cpg_quirks_match[] __initconst = {
	{
		.soc_id = "r8a7795", .revision = "ES1.0",
		.data = (void *)(PLL_ERRATA | RCKCR_CKSEL | SD_SKIP_FIRST),
	},
	{
		.soc_id = "r8a7795", .revision = "ES1.*",
		.data = (void *)(RCKCR_CKSEL | SD_SKIP_FIRST),
	},
	{
		.soc_id = "r8a7795", .revision = "ES2.0",
		.data = (void *)SD_SKIP_FIRST,
	},
	{
		.soc_id = "r8a7796", .revision = "ES1.0",
		.data = (void *)(RCKCR_CKSEL | SD_SKIP_FIRST),
	},
	{
		.soc_id = "r8a7796", .revision = "ES1.1",
		.data = (void *)SD_SKIP_FIRST,
	},
	{ /* sentinel */ }
};

struct clk * __init rcar_gen3_cpg_clk_register(struct device *dev,
	const struct cpg_core_clk *core, const struct cpg_mssr_info *info,
	struct clk **clks, void __iomem *base,
	struct raw_notifier_head *notifiers)
{
	const struct clk *parent;
	unsigned int mult = 1;
	unsigned int div = 1;
	u32 value;

	parent = clks[core->parent & 0xffff];	/* some types use high bits */
	if (IS_ERR(parent))
		return ERR_CAST(parent);

	switch (core->type) {
	case CLK_TYPE_GEN3_MAIN:
		div = cpg_pll_config->extal_div;
		break;

	case CLK_TYPE_GEN3_PLL0:
		/*
		 * PLL0 is a configurable multiplier clock. Register it as a
		 * fixed factor clock for now as there's no generic multiplier
		 * clock implementation and we currently have no need to change
		 * the multiplier value.
		 */
		value = readl(base + CPG_PLL0CR);
		mult = (((value >> 24) & 0x7f) + 1) * 2;
		if (cpg_quirks & PLL_ERRATA)
			mult *= 2;
		break;

	case CLK_TYPE_GEN3_PLL1:
		mult = cpg_pll_config->pll1_mult;
		div = cpg_pll_config->pll1_div;
		break;

	case CLK_TYPE_GEN3_PLL2:
		/*
		 * PLL2 is a configurable multiplier clock. Register it as a
		 * fixed factor clock for now as there's no generic multiplier
		 * clock implementation and we currently have no need to change
		 * the multiplier value.
		 */
		value = readl(base + CPG_PLL2CR);
		mult = (((value >> 24) & 0x7f) + 1) * 2;
		if (cpg_quirks & PLL_ERRATA)
			mult *= 2;
		break;

	case CLK_TYPE_GEN3_PLL3:
		mult = cpg_pll_config->pll3_mult;
		div = cpg_pll_config->pll3_div;
		break;

	case CLK_TYPE_GEN3_PLL4:
		/*
		 * PLL4 is a configurable multiplier clock. Register it as a
		 * fixed factor clock for now as there's no generic multiplier
		 * clock implementation and we currently have no need to change
		 * the multiplier value.
		 */
		value = readl(base + CPG_PLL4CR);
		mult = (((value >> 24) & 0x7f) + 1) * 2;
		if (cpg_quirks & PLL_ERRATA)
			mult *= 2;
		break;

	case CLK_TYPE_GEN3_SD:
		return cpg_sd_clk_register(core->name, base, core->offset,
					   __clk_get_name(parent), notifiers,
					   cpg_quirks & SD_SKIP_FIRST);

	case CLK_TYPE_GEN3_R:
		if (cpg_quirks & RCKCR_CKSEL) {
			struct cpg_simple_notifier *csn;

			csn = kzalloc(sizeof(*csn), GFP_KERNEL);
			if (!csn)
				return ERR_PTR(-ENOMEM);

			csn->reg = base + CPG_RCKCR;

			/*
			 * RINT is default.
			 * Only if EXTALR is populated, we switch to it.
			 */
			value = readl(csn->reg) & 0x3f;

			if (clk_get_rate(clks[cpg_clk_extalr])) {
				parent = clks[cpg_clk_extalr];
				value |= CPG_RCKCR_CKSEL;
			}

			writel(value, csn->reg);
			cpg_simple_notifier_register(notifiers, csn);
			break;
		}

		/* Select parent clock of RCLK by MD28 */
		if (cpg_mode & BIT(28))
			parent = clks[cpg_clk_extalr];
		break;

	case CLK_TYPE_GEN3_MDSEL:
		/*
		 * Clock selectable between two parents and two fixed dividers
		 * using a mode pin
		 */
		if (cpg_mode & BIT(core->offset)) {
			div = core->div & 0xffff;
		} else {
			parent = clks[core->parent >> 16];
			if (IS_ERR(parent))
				return ERR_CAST(parent);
			div = core->div >> 16;
		}
		mult = 1;
		break;

	case CLK_TYPE_GEN3_Z:
		return cpg_z_clk_register(core->name, __clk_get_name(parent),
					  base, core->div, core->offset);

	case CLK_TYPE_GEN3_OSC:
		/*
		 * Clock combining OSC EXTAL predivider and a fixed divider
		 */
		div = cpg_pll_config->osc_prediv * core->div;
		break;

	case CLK_TYPE_GEN3_RCKSEL:
		/*
		 * Clock selectable between two parents and two fixed dividers
		 * using RCKCR.CKSEL
		 */
		if (readl(base + CPG_RCKCR) & CPG_RCKCR_CKSEL) {
			div = core->div & 0xffff;
		} else {
			parent = clks[core->parent >> 16];
			if (IS_ERR(parent))
				return ERR_CAST(parent);
			div = core->div >> 16;
		}
		break;

	case CLK_TYPE_GEN3_RPCSRC:
		return clk_register_divider_table(NULL, core->name,
						  __clk_get_name(parent), 0,
						  base + CPG_RPCCKCR, 3, 2, 0,
						  cpg_rpcsrc_div_table,
						  &cpg_lock);

	case CLK_TYPE_GEN3_E3_RPCSRC:
		/*
		 * Register RPCSRC as fixed factor clock based on the
		 * MD[4:1] pins and CPG_RPCCKCR[4:3] register value for
		 * which has been set prior to booting the kernel.
		 */
		value = (readl(base + CPG_RPCCKCR) & GENMASK(4, 3)) >> 3;

		switch (value) {
		case 0:
			div = 5;
			break;
		case 1:
			div = 3;
			break;
		case 2:
			parent = clks[core->parent >> 16];
			if (IS_ERR(parent))
				return ERR_CAST(parent);
			div = core->div;
			break;
		case 3:
		default:
			div = 2;
			break;
		}
		break;

	case CLK_TYPE_GEN3_RPC:
		return cpg_rpc_clk_register(core->name, base,
					    __clk_get_name(parent), notifiers);

	case CLK_TYPE_GEN3_RPCD2:
		return cpg_rpcd2_clk_register(core->name, base,
					      __clk_get_name(parent));

	default:
		return ERR_PTR(-EINVAL);
	}

	return clk_register_fixed_factor(NULL, core->name,
					 __clk_get_name(parent), 0, mult, div);
}

int __init rcar_gen3_cpg_init(const struct rcar_gen3_cpg_pll_config *config,
			      unsigned int clk_extalr, u32 mode)
{
	const struct soc_device_attribute *attr;

	cpg_pll_config = config;
	cpg_clk_extalr = clk_extalr;
	cpg_mode = mode;
	attr = soc_device_match(cpg_quirks_match);
	if (attr)
		cpg_quirks = (uintptr_t)attr->data;
	pr_debug("%s: mode = 0x%x quirks = 0x%x\n", __func__, mode, cpg_quirks);

	spin_lock_init(&cpg_lock);

	return 0;
}
