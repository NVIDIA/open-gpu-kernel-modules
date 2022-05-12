/*
 * OMAP gate clock support
 *
 * Copyright (C) 2013 Texas Instruments, Inc.
 *
 * Tero Kristo <t-kristo@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/clk-provider.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/clk/ti.h>

#include "clock.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s: " fmt, __func__

static int omap36xx_gate_clk_enable_with_hsdiv_restore(struct clk_hw *clk);

static const struct clk_ops omap_gate_clkdm_clk_ops = {
	.init		= &omap2_init_clk_clkdm,
	.enable		= &omap2_clkops_enable_clkdm,
	.disable	= &omap2_clkops_disable_clkdm,
	.restore_context = clk_gate_restore_context,
};

const struct clk_ops omap_gate_clk_ops = {
	.init		= &omap2_init_clk_clkdm,
	.enable		= &omap2_dflt_clk_enable,
	.disable	= &omap2_dflt_clk_disable,
	.is_enabled	= &omap2_dflt_clk_is_enabled,
	.restore_context = clk_gate_restore_context,
};

static const struct clk_ops omap_gate_clk_hsdiv_restore_ops = {
	.init		= &omap2_init_clk_clkdm,
	.enable		= &omap36xx_gate_clk_enable_with_hsdiv_restore,
	.disable	= &omap2_dflt_clk_disable,
	.is_enabled	= &omap2_dflt_clk_is_enabled,
	.restore_context = clk_gate_restore_context,
};

/**
 * omap36xx_gate_clk_enable_with_hsdiv_restore - enable clocks suffering
 *         from HSDivider PWRDN problem Implements Errata ID: i556.
 * @hw: DPLL output struct clk_hw
 *
 * 3630 only: dpll3_m3_ck, dpll4_m2_ck, dpll4_m3_ck, dpll4_m4_ck,
 * dpll4_m5_ck & dpll4_m6_ck dividers gets loaded with reset
 * valueafter their respective PWRDN bits are set.  Any dummy write
 * (Any other value different from the Read value) to the
 * corresponding CM_CLKSEL register will refresh the dividers.
 */
static int omap36xx_gate_clk_enable_with_hsdiv_restore(struct clk_hw *hw)
{
	struct clk_omap_divider *parent;
	struct clk_hw *parent_hw;
	u32 dummy_v, orig_v;
	int ret;

	/* Clear PWRDN bit of HSDIVIDER */
	ret = omap2_dflt_clk_enable(hw);

	/* Parent is the x2 node, get parent of parent for the m2 div */
	parent_hw = clk_hw_get_parent(clk_hw_get_parent(hw));
	parent = to_clk_omap_divider(parent_hw);

	/* Restore the dividers */
	if (!ret) {
		orig_v = ti_clk_ll_ops->clk_readl(&parent->reg);
		dummy_v = orig_v;

		/* Write any other value different from the Read value */
		dummy_v ^= (1 << parent->shift);
		ti_clk_ll_ops->clk_writel(dummy_v, &parent->reg);

		/* Write the original divider */
		ti_clk_ll_ops->clk_writel(orig_v, &parent->reg);
	}

	return ret;
}

static struct clk *_register_gate(struct device *dev, const char *name,
				  const char *parent_name, unsigned long flags,
				  struct clk_omap_reg *reg, u8 bit_idx,
				  u8 clk_gate_flags, const struct clk_ops *ops,
				  const struct clk_hw_omap_ops *hw_ops)
{
	struct clk_init_data init = { NULL };
	struct clk_hw_omap *clk_hw;
	struct clk *clk;

	clk_hw = kzalloc(sizeof(*clk_hw), GFP_KERNEL);
	if (!clk_hw)
		return ERR_PTR(-ENOMEM);

	clk_hw->hw.init = &init;

	init.name = name;
	init.ops = ops;

	memcpy(&clk_hw->enable_reg, reg, sizeof(*reg));
	clk_hw->enable_bit = bit_idx;
	clk_hw->ops = hw_ops;

	clk_hw->flags = clk_gate_flags;

	init.parent_names = &parent_name;
	init.num_parents = 1;

	init.flags = flags;

	clk = ti_clk_register_omap_hw(NULL, &clk_hw->hw, name);

	if (IS_ERR(clk))
		kfree(clk_hw);

	return clk;
}

static void __init _of_ti_gate_clk_setup(struct device_node *node,
					 const struct clk_ops *ops,
					 const struct clk_hw_omap_ops *hw_ops)
{
	struct clk *clk;
	const char *parent_name;
	struct clk_omap_reg reg;
	u8 enable_bit = 0;
	u32 val;
	u32 flags = 0;
	u8 clk_gate_flags = 0;

	if (ops != &omap_gate_clkdm_clk_ops) {
		if (ti_clk_get_reg_addr(node, 0, &reg))
			return;

		if (!of_property_read_u32(node, "ti,bit-shift", &val))
			enable_bit = val;
	}

	if (of_clk_get_parent_count(node) != 1) {
		pr_err("%pOFn must have 1 parent\n", node);
		return;
	}

	parent_name = of_clk_get_parent_name(node, 0);

	if (of_property_read_bool(node, "ti,set-rate-parent"))
		flags |= CLK_SET_RATE_PARENT;

	if (of_property_read_bool(node, "ti,set-bit-to-disable"))
		clk_gate_flags |= INVERT_ENABLE;

	clk = _register_gate(NULL, node->name, parent_name, flags, &reg,
			     enable_bit, clk_gate_flags, ops, hw_ops);

	if (!IS_ERR(clk))
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
}

static void __init
_of_ti_composite_gate_clk_setup(struct device_node *node,
				const struct clk_hw_omap_ops *hw_ops)
{
	struct clk_hw_omap *gate;
	u32 val = 0;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return;

	if (ti_clk_get_reg_addr(node, 0, &gate->enable_reg))
		goto cleanup;

	of_property_read_u32(node, "ti,bit-shift", &val);

	gate->enable_bit = val;
	gate->ops = hw_ops;

	if (!ti_clk_add_component(node, &gate->hw, CLK_COMPONENT_TYPE_GATE))
		return;

cleanup:
	kfree(gate);
}

static void __init
of_ti_composite_no_wait_gate_clk_setup(struct device_node *node)
{
	_of_ti_composite_gate_clk_setup(node, NULL);
}
CLK_OF_DECLARE(ti_composite_no_wait_gate_clk, "ti,composite-no-wait-gate-clock",
	       of_ti_composite_no_wait_gate_clk_setup);

#if defined(CONFIG_ARCH_OMAP2) || defined(CONFIG_ARCH_OMAP3)
static void __init of_ti_composite_interface_clk_setup(struct device_node *node)
{
	_of_ti_composite_gate_clk_setup(node, &clkhwops_iclk_wait);
}
CLK_OF_DECLARE(ti_composite_interface_clk, "ti,composite-interface-clock",
	       of_ti_composite_interface_clk_setup);
#endif

static void __init of_ti_composite_gate_clk_setup(struct device_node *node)
{
	_of_ti_composite_gate_clk_setup(node, &clkhwops_wait);
}
CLK_OF_DECLARE(ti_composite_gate_clk, "ti,composite-gate-clock",
	       of_ti_composite_gate_clk_setup);


static void __init of_ti_clkdm_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clkdm_clk_ops, NULL);
}
CLK_OF_DECLARE(ti_clkdm_gate_clk, "ti,clkdm-gate-clock",
	       of_ti_clkdm_gate_clk_setup);

static void __init of_ti_hsdiv_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clk_hsdiv_restore_ops,
			      &clkhwops_wait);
}
CLK_OF_DECLARE(ti_hsdiv_gate_clk, "ti,hsdiv-gate-clock",
	       of_ti_hsdiv_gate_clk_setup);

static void __init of_ti_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clk_ops, NULL);
}
CLK_OF_DECLARE(ti_gate_clk, "ti,gate-clock", of_ti_gate_clk_setup);

static void __init of_ti_wait_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clk_ops, &clkhwops_wait);
}
CLK_OF_DECLARE(ti_wait_gate_clk, "ti,wait-gate-clock",
	       of_ti_wait_gate_clk_setup);

#ifdef CONFIG_ARCH_OMAP3
static void __init of_ti_am35xx_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clk_ops,
			      &clkhwops_am35xx_ipss_module_wait);
}
CLK_OF_DECLARE(ti_am35xx_gate_clk, "ti,am35xx-gate-clock",
	       of_ti_am35xx_gate_clk_setup);

static void __init of_ti_dss_gate_clk_setup(struct device_node *node)
{
	_of_ti_gate_clk_setup(node, &omap_gate_clk_ops,
			      &clkhwops_omap3430es2_dss_usbhost_wait);
}
CLK_OF_DECLARE(ti_dss_gate_clk, "ti,dss-gate-clock",
	       of_ti_dss_gate_clk_setup);
#endif
