// SPDX-License-Identifier: GPL-2.0
/***************************************************************************/

/*
 *	clk.c -- general ColdFire CPU kernel clk handling
 *
 *	Copyright (C) 2009, Greg Ungerer (gerg@snapgear.com)
 */

/***************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/err.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfclk.h>

static DEFINE_SPINLOCK(clk_lock);

#ifdef MCFPM_PPMCR0
/*
 *	For more advanced ColdFire parts that have clocks that can be enabled
 *	we supply enable/disable functions. These must properly define their
 *	clocks in their platform specific code.
 */
void __clk_init_enabled(struct clk *clk)
{
	clk->enabled = 1;
	clk->clk_ops->enable(clk);
}

void __clk_init_disabled(struct clk *clk)
{
	clk->enabled = 0;
	clk->clk_ops->disable(clk);
}

static void __clk_enable0(struct clk *clk)
{
	__raw_writeb(clk->slot, MCFPM_PPMCR0);
}

static void __clk_disable0(struct clk *clk)
{
	__raw_writeb(clk->slot, MCFPM_PPMSR0);
}

struct clk_ops clk_ops0 = {
	.enable		= __clk_enable0,
	.disable	= __clk_disable0,
};

#ifdef MCFPM_PPMCR1
static void __clk_enable1(struct clk *clk)
{
	__raw_writeb(clk->slot, MCFPM_PPMCR1);
}

static void __clk_disable1(struct clk *clk)
{
	__raw_writeb(clk->slot, MCFPM_PPMSR1);
}

struct clk_ops clk_ops1 = {
	.enable		= __clk_enable1,
	.disable	= __clk_disable1,
};
#endif /* MCFPM_PPMCR1 */
#endif /* MCFPM_PPMCR0 */

struct clk *clk_get(struct device *dev, const char *id)
{
	const char *clk_name = dev ? dev_name(dev) : id ? id : NULL;
	struct clk *clk;
	unsigned i;

	for (i = 0; (clk = mcf_clks[i]) != NULL; ++i)
		if (!strcmp(clk->name, clk_name))
			return clk;
	pr_warn("clk_get: didn't find clock %s\n", clk_name);
	return ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL(clk_get);

int clk_enable(struct clk *clk)
{
	unsigned long flags;

	if (!clk)
		return -EINVAL;

	spin_lock_irqsave(&clk_lock, flags);
	if ((clk->enabled++ == 0) && clk->clk_ops)
		clk->clk_ops->enable(clk);
	spin_unlock_irqrestore(&clk_lock, flags);

	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
	unsigned long flags;

	if (!clk)
		return;

	spin_lock_irqsave(&clk_lock, flags);
	if ((--clk->enabled == 0) && clk->clk_ops)
		clk->clk_ops->disable(clk);
	spin_unlock_irqrestore(&clk_lock, flags);
}
EXPORT_SYMBOL(clk_disable);

void clk_put(struct clk *clk)
{
	if (clk->enabled != 0)
		pr_warn("clk_put %s still enabled\n", clk->name);
}
EXPORT_SYMBOL(clk_put);

unsigned long clk_get_rate(struct clk *clk)
{
	if (!clk)
		return 0;

	return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

/* dummy functions, should not be called */
long clk_round_rate(struct clk *clk, unsigned long rate)
{
	WARN_ON(clk);
	return 0;
}
EXPORT_SYMBOL(clk_round_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	WARN_ON(clk);
	return 0;
}
EXPORT_SYMBOL(clk_set_rate);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	WARN_ON(clk);
	return 0;
}
EXPORT_SYMBOL(clk_set_parent);

struct clk *clk_get_parent(struct clk *clk)
{
	WARN_ON(clk);
	return NULL;
}
EXPORT_SYMBOL(clk_get_parent);

/***************************************************************************/
