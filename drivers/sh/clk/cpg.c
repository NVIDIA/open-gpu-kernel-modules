/*
 * Helper routines for SuperH Clock Pulse Generator blocks (CPG).
 *
 *  Copyright (C) 2010  Magnus Damm
 *  Copyright (C) 2010 - 2012  Paul Mundt
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/clk.h>
#include <linux/compiler.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/sh_clk.h>

#define CPG_CKSTP_BIT	BIT(8)

static unsigned int sh_clk_read(struct clk *clk)
{
	if (clk->flags & CLK_ENABLE_REG_8BIT)
		return ioread8(clk->mapped_reg);
	else if (clk->flags & CLK_ENABLE_REG_16BIT)
		return ioread16(clk->mapped_reg);

	return ioread32(clk->mapped_reg);
}

static void sh_clk_write(int value, struct clk *clk)
{
	if (clk->flags & CLK_ENABLE_REG_8BIT)
		iowrite8(value, clk->mapped_reg);
	else if (clk->flags & CLK_ENABLE_REG_16BIT)
		iowrite16(value, clk->mapped_reg);
	else
		iowrite32(value, clk->mapped_reg);
}

static int sh_clk_mstp_enable(struct clk *clk)
{
	sh_clk_write(sh_clk_read(clk) & ~(1 << clk->enable_bit), clk);
	if (clk->status_reg) {
		unsigned int (*read)(const void __iomem *addr);
		int i;
		void __iomem *mapped_status = (phys_addr_t)clk->status_reg -
			(phys_addr_t)clk->enable_reg + clk->mapped_reg;

		if (clk->flags & CLK_ENABLE_REG_8BIT)
			read = ioread8;
		else if (clk->flags & CLK_ENABLE_REG_16BIT)
			read = ioread16;
		else
			read = ioread32;

		for (i = 1000;
		     (read(mapped_status) & (1 << clk->enable_bit)) && i;
		     i--)
			cpu_relax();
		if (!i) {
			pr_err("cpg: failed to enable %p[%d]\n",
			       clk->enable_reg, clk->enable_bit);
			return -ETIMEDOUT;
		}
	}
	return 0;
}

static void sh_clk_mstp_disable(struct clk *clk)
{
	sh_clk_write(sh_clk_read(clk) | (1 << clk->enable_bit), clk);
}

static struct sh_clk_ops sh_clk_mstp_clk_ops = {
	.enable		= sh_clk_mstp_enable,
	.disable	= sh_clk_mstp_disable,
	.recalc		= followparent_recalc,
};

int __init sh_clk_mstp_register(struct clk *clks, int nr)
{
	struct clk *clkp;
	int ret = 0;
	int k;

	for (k = 0; !ret && (k < nr); k++) {
		clkp = clks + k;
		clkp->ops = &sh_clk_mstp_clk_ops;
		ret |= clk_register(clkp);
	}

	return ret;
}

/*
 * Div/mult table lookup helpers
 */
static inline struct clk_div_table *clk_to_div_table(struct clk *clk)
{
	return clk->priv;
}

static inline struct clk_div_mult_table *clk_to_div_mult_table(struct clk *clk)
{
	return clk_to_div_table(clk)->div_mult_table;
}

/*
 * Common div ops
 */
static long sh_clk_div_round_rate(struct clk *clk, unsigned long rate)
{
	return clk_rate_table_round(clk, clk->freq_table, rate);
}

static unsigned long sh_clk_div_recalc(struct clk *clk)
{
	struct clk_div_mult_table *table = clk_to_div_mult_table(clk);
	unsigned int idx;

	clk_rate_table_build(clk, clk->freq_table, table->nr_divisors,
			     table, clk->arch_flags ? &clk->arch_flags : NULL);

	idx = (sh_clk_read(clk) >> clk->enable_bit) & clk->div_mask;

	return clk->freq_table[idx].frequency;
}

static int sh_clk_div_set_rate(struct clk *clk, unsigned long rate)
{
	struct clk_div_table *dt = clk_to_div_table(clk);
	unsigned long value;
	int idx;

	idx = clk_rate_table_find(clk, clk->freq_table, rate);
	if (idx < 0)
		return idx;

	value = sh_clk_read(clk);
	value &= ~(clk->div_mask << clk->enable_bit);
	value |= (idx << clk->enable_bit);
	sh_clk_write(value, clk);

	/* XXX: Should use a post-change notifier */
	if (dt->kick)
		dt->kick(clk);

	return 0;
}

static int sh_clk_div_enable(struct clk *clk)
{
	if (clk->div_mask == SH_CLK_DIV6_MSK) {
		int ret = sh_clk_div_set_rate(clk, clk->rate);
		if (ret < 0)
			return ret;
	}

	sh_clk_write(sh_clk_read(clk) & ~CPG_CKSTP_BIT, clk);
	return 0;
}

static void sh_clk_div_disable(struct clk *clk)
{
	unsigned int val;

	val = sh_clk_read(clk);
	val |= CPG_CKSTP_BIT;

	/*
	 * div6 clocks require the divisor field to be non-zero or the
	 * above CKSTP toggle silently fails. Ensure that the divisor
	 * array is reset to its initial state on disable.
	 */
	if (clk->flags & CLK_MASK_DIV_ON_DISABLE)
		val |= clk->div_mask;

	sh_clk_write(val, clk);
}

static struct sh_clk_ops sh_clk_div_clk_ops = {
	.recalc		= sh_clk_div_recalc,
	.set_rate	= sh_clk_div_set_rate,
	.round_rate	= sh_clk_div_round_rate,
};

static struct sh_clk_ops sh_clk_div_enable_clk_ops = {
	.recalc		= sh_clk_div_recalc,
	.set_rate	= sh_clk_div_set_rate,
	.round_rate	= sh_clk_div_round_rate,
	.enable		= sh_clk_div_enable,
	.disable	= sh_clk_div_disable,
};

static int __init sh_clk_init_parent(struct clk *clk)
{
	u32 val;

	if (clk->parent)
		return 0;

	if (!clk->parent_table || !clk->parent_num)
		return 0;

	if (!clk->src_width) {
		pr_err("sh_clk_init_parent: cannot select parent clock\n");
		return -EINVAL;
	}

	val  = (sh_clk_read(clk) >> clk->src_shift);
	val &= (1 << clk->src_width) - 1;

	if (val >= clk->parent_num) {
		pr_err("sh_clk_init_parent: parent table size failed\n");
		return -EINVAL;
	}

	clk_reparent(clk, clk->parent_table[val]);
	if (!clk->parent) {
		pr_err("sh_clk_init_parent: unable to set parent");
		return -EINVAL;
	}

	return 0;
}

static int __init sh_clk_div_register_ops(struct clk *clks, int nr,
			struct clk_div_table *table, struct sh_clk_ops *ops)
{
	struct clk *clkp;
	void *freq_table;
	int nr_divs = table->div_mult_table->nr_divisors;
	int freq_table_size = sizeof(struct cpufreq_frequency_table);
	int ret = 0;
	int k;

	freq_table_size *= (nr_divs + 1);
	freq_table = kcalloc(nr, freq_table_size, GFP_KERNEL);
	if (!freq_table) {
		pr_err("%s: unable to alloc memory\n", __func__);
		return -ENOMEM;
	}

	for (k = 0; !ret && (k < nr); k++) {
		clkp = clks + k;

		clkp->ops = ops;
		clkp->priv = table;

		clkp->freq_table = freq_table + (k * freq_table_size);
		clkp->freq_table[nr_divs].frequency = CPUFREQ_TABLE_END;

		ret = clk_register(clkp);
		if (ret == 0)
			ret = sh_clk_init_parent(clkp);
	}

	return ret;
}

/*
 * div6 support
 */
static int sh_clk_div6_divisors[64] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64
};

static struct clk_div_mult_table div6_div_mult_table = {
	.divisors = sh_clk_div6_divisors,
	.nr_divisors = ARRAY_SIZE(sh_clk_div6_divisors),
};

static struct clk_div_table sh_clk_div6_table = {
	.div_mult_table	= &div6_div_mult_table,
};

static int sh_clk_div6_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk_div_mult_table *table = clk_to_div_mult_table(clk);
	u32 value;
	int ret, i;

	if (!clk->parent_table || !clk->parent_num)
		return -EINVAL;

	/* Search the parent */
	for (i = 0; i < clk->parent_num; i++)
		if (clk->parent_table[i] == parent)
			break;

	if (i == clk->parent_num)
		return -ENODEV;

	ret = clk_reparent(clk, parent);
	if (ret < 0)
		return ret;

	value = sh_clk_read(clk) &
		~(((1 << clk->src_width) - 1) << clk->src_shift);

	sh_clk_write(value | (i << clk->src_shift), clk);

	/* Rebuild the frequency table */
	clk_rate_table_build(clk, clk->freq_table, table->nr_divisors,
			     table, NULL);

	return 0;
}

static struct sh_clk_ops sh_clk_div6_reparent_clk_ops = {
	.recalc		= sh_clk_div_recalc,
	.round_rate	= sh_clk_div_round_rate,
	.set_rate	= sh_clk_div_set_rate,
	.enable		= sh_clk_div_enable,
	.disable	= sh_clk_div_disable,
	.set_parent	= sh_clk_div6_set_parent,
};

int __init sh_clk_div6_register(struct clk *clks, int nr)
{
	return sh_clk_div_register_ops(clks, nr, &sh_clk_div6_table,
				       &sh_clk_div_enable_clk_ops);
}

int __init sh_clk_div6_reparent_register(struct clk *clks, int nr)
{
	return sh_clk_div_register_ops(clks, nr, &sh_clk_div6_table,
				       &sh_clk_div6_reparent_clk_ops);
}

/*
 * div4 support
 */
static int sh_clk_div4_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk_div_mult_table *table = clk_to_div_mult_table(clk);
	u32 value;
	int ret;

	/* we really need a better way to determine parent index, but for
	 * now assume internal parent comes with CLK_ENABLE_ON_INIT set,
	 * no CLK_ENABLE_ON_INIT means external clock...
	 */

	if (parent->flags & CLK_ENABLE_ON_INIT)
		value = sh_clk_read(clk) & ~(1 << 7);
	else
		value = sh_clk_read(clk) | (1 << 7);

	ret = clk_reparent(clk, parent);
	if (ret < 0)
		return ret;

	sh_clk_write(value, clk);

	/* Rebiuld the frequency table */
	clk_rate_table_build(clk, clk->freq_table, table->nr_divisors,
			     table, &clk->arch_flags);

	return 0;
}

static struct sh_clk_ops sh_clk_div4_reparent_clk_ops = {
	.recalc		= sh_clk_div_recalc,
	.set_rate	= sh_clk_div_set_rate,
	.round_rate	= sh_clk_div_round_rate,
	.enable		= sh_clk_div_enable,
	.disable	= sh_clk_div_disable,
	.set_parent	= sh_clk_div4_set_parent,
};

int __init sh_clk_div4_register(struct clk *clks, int nr,
				struct clk_div4_table *table)
{
	return sh_clk_div_register_ops(clks, nr, table, &sh_clk_div_clk_ops);
}

int __init sh_clk_div4_enable_register(struct clk *clks, int nr,
				struct clk_div4_table *table)
{
	return sh_clk_div_register_ops(clks, nr, table,
				       &sh_clk_div_enable_clk_ops);
}

int __init sh_clk_div4_reparent_register(struct clk *clks, int nr,
				struct clk_div4_table *table)
{
	return sh_clk_div_register_ops(clks, nr, table,
				       &sh_clk_div4_reparent_clk_ops);
}

/* FSI-DIV */
static unsigned long fsidiv_recalc(struct clk *clk)
{
	u32 value;

	value = __raw_readl(clk->mapping->base);

	value >>= 16;
	if (value < 2)
		return clk->parent->rate;

	return clk->parent->rate / value;
}

static long fsidiv_round_rate(struct clk *clk, unsigned long rate)
{
	return clk_rate_div_range_round(clk, 1, 0xffff, rate);
}

static void fsidiv_disable(struct clk *clk)
{
	__raw_writel(0, clk->mapping->base);
}

static int fsidiv_enable(struct clk *clk)
{
	u32 value;

	value  = __raw_readl(clk->mapping->base) >> 16;
	if (value < 2)
		return 0;

	__raw_writel((value << 16) | 0x3, clk->mapping->base);

	return 0;
}

static int fsidiv_set_rate(struct clk *clk, unsigned long rate)
{
	int idx;

	idx = (clk->parent->rate / rate) & 0xffff;
	if (idx < 2)
		__raw_writel(0, clk->mapping->base);
	else
		__raw_writel(idx << 16, clk->mapping->base);

	return 0;
}

static struct sh_clk_ops fsidiv_clk_ops = {
	.recalc		= fsidiv_recalc,
	.round_rate	= fsidiv_round_rate,
	.set_rate	= fsidiv_set_rate,
	.enable		= fsidiv_enable,
	.disable	= fsidiv_disable,
};

int __init sh_clk_fsidiv_register(struct clk *clks, int nr)
{
	struct clk_mapping *map;
	int i;

	for (i = 0; i < nr; i++) {

		map = kzalloc(sizeof(struct clk_mapping), GFP_KERNEL);
		if (!map) {
			pr_err("%s: unable to alloc memory\n", __func__);
			return -ENOMEM;
		}

		/* clks[i].enable_reg came from SH_CLK_FSIDIV() */
		map->phys		= (phys_addr_t)clks[i].enable_reg;
		map->len		= 8;

		clks[i].enable_reg	= 0; /* remove .enable_reg */
		clks[i].ops		= &fsidiv_clk_ops;
		clks[i].mapping		= map;

		clk_register(&clks[i]);
	}

	return 0;
}
