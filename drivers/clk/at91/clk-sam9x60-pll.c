// SPDX-License-Identifier: GPL-2.0+
/*
 *  Copyright (C) 2019 Microchip Technology Inc.
 *
 */

#include <linux/bitfield.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

#define	PMC_PLL_CTRL0_DIV_MSK	GENMASK(7, 0)
#define	PMC_PLL_CTRL1_MUL_MSK	GENMASK(31, 24)
#define	PMC_PLL_CTRL1_FRACR_MSK	GENMASK(21, 0)

#define PLL_DIV_MAX		(FIELD_GET(PMC_PLL_CTRL0_DIV_MSK, UINT_MAX) + 1)
#define UPLL_DIV		2
#define PLL_MUL_MAX		(FIELD_GET(PMC_PLL_CTRL1_MUL_MSK, UINT_MAX) + 1)

#define FCORE_MIN		(600000000)
#define FCORE_MAX		(1200000000)

#define PLL_MAX_ID		7

struct sam9x60_pll_core {
	struct regmap *regmap;
	spinlock_t *lock;
	const struct clk_pll_characteristics *characteristics;
	const struct clk_pll_layout *layout;
	struct clk_hw hw;
	u8 id;
};

struct sam9x60_frac {
	struct sam9x60_pll_core core;
	u32 frac;
	u16 mul;
};

struct sam9x60_div {
	struct sam9x60_pll_core core;
	u8 div;
};

#define to_sam9x60_pll_core(hw)	container_of(hw, struct sam9x60_pll_core, hw)
#define to_sam9x60_frac(core)	container_of(core, struct sam9x60_frac, core)
#define to_sam9x60_div(core)	container_of(core, struct sam9x60_div, core)

static inline bool sam9x60_pll_ready(struct regmap *regmap, int id)
{
	unsigned int status;

	regmap_read(regmap, AT91_PMC_PLL_ISR0, &status);

	return !!(status & BIT(id));
}

static bool sam9x60_frac_pll_ready(struct regmap *regmap, u8 id)
{
	return sam9x60_pll_ready(regmap, id);
}

static unsigned long sam9x60_frac_pll_recalc_rate(struct clk_hw *hw,
						  unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_frac *frac = to_sam9x60_frac(core);

	return (parent_rate * (frac->mul + 1) +
		((u64)parent_rate * frac->frac >> 22));
}

static int sam9x60_frac_pll_prepare(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_frac *frac = to_sam9x60_frac(core);
	struct regmap *regmap = core->regmap;
	unsigned int val, cfrac, cmul;
	unsigned long flags;

	spin_lock_irqsave(core->lock, flags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, core->id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL1, &val);
	cmul = (val & core->layout->mul_mask) >> core->layout->mul_shift;
	cfrac = (val & core->layout->frac_mask) >> core->layout->frac_shift;

	if (sam9x60_frac_pll_ready(regmap, core->id) &&
	    (cmul == frac->mul && cfrac == frac->frac))
		goto unlock;

	/* Recommended value for PMC_PLL_ACR */
	if (core->characteristics->upll)
		val = AT91_PMC_PLL_ACR_DEFAULT_UPLL;
	else
		val = AT91_PMC_PLL_ACR_DEFAULT_PLLA;
	regmap_write(regmap, AT91_PMC_PLL_ACR, val);

	regmap_write(regmap, AT91_PMC_PLL_CTRL1,
		     (frac->mul << core->layout->mul_shift) |
		     (frac->frac << core->layout->frac_shift));

	if (core->characteristics->upll) {
		/* Enable the UTMI internal bandgap */
		val |= AT91_PMC_PLL_ACR_UTMIBG;
		regmap_write(regmap, AT91_PMC_PLL_ACR, val);

		udelay(10);

		/* Enable the UTMI internal regulator */
		val |= AT91_PMC_PLL_ACR_UTMIVR;
		regmap_write(regmap, AT91_PMC_PLL_ACR, val);

		udelay(10);
	}

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0,
			   AT91_PMC_PLL_CTRL0_ENLOCK | AT91_PMC_PLL_CTRL0_ENPLL,
			   AT91_PMC_PLL_CTRL0_ENLOCK | AT91_PMC_PLL_CTRL0_ENPLL);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	while (!sam9x60_pll_ready(regmap, core->id))
		cpu_relax();

unlock:
	spin_unlock_irqrestore(core->lock, flags);

	return 0;
}

static void sam9x60_frac_pll_unprepare(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct regmap *regmap = core->regmap;
	unsigned long flags;

	spin_lock_irqsave(core->lock, flags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, core->id);

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0, AT91_PMC_PLL_CTRL0_ENPLL, 0);

	if (core->characteristics->upll)
		regmap_update_bits(regmap, AT91_PMC_PLL_ACR,
				   AT91_PMC_PLL_ACR_UTMIBG | AT91_PMC_PLL_ACR_UTMIVR, 0);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	spin_unlock_irqrestore(core->lock, flags);
}

static int sam9x60_frac_pll_is_prepared(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);

	return sam9x60_pll_ready(core->regmap, core->id);
}

static long sam9x60_frac_pll_compute_mul_frac(struct sam9x60_pll_core *core,
					      unsigned long rate,
					      unsigned long parent_rate,
					      bool update)
{
	struct sam9x60_frac *frac = to_sam9x60_frac(core);
	unsigned long tmprate, remainder;
	unsigned long nmul = 0;
	unsigned long nfrac = 0;

	if (rate < FCORE_MIN || rate > FCORE_MAX)
		return -ERANGE;

	/*
	 * Calculate the multiplier associated with the current
	 * divider that provide the closest rate to the requested one.
	 */
	nmul = mult_frac(rate, 1, parent_rate);
	tmprate = mult_frac(parent_rate, nmul, 1);
	remainder = rate - tmprate;

	if (remainder) {
		nfrac = DIV_ROUND_CLOSEST_ULL((u64)remainder * (1 << 22),
					      parent_rate);

		tmprate += DIV_ROUND_CLOSEST_ULL((u64)nfrac * parent_rate,
						 (1 << 22));
	}

	/* Check if resulted rate is a valid.  */
	if (tmprate < FCORE_MIN || tmprate > FCORE_MAX)
		return -ERANGE;

	if (update) {
		frac->mul = nmul - 1;
		frac->frac = nfrac;
	}

	return tmprate;
}

static long sam9x60_frac_pll_round_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long *parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);

	return sam9x60_frac_pll_compute_mul_frac(core, rate, *parent_rate, false);
}

static int sam9x60_frac_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);

	return sam9x60_frac_pll_compute_mul_frac(core, rate, parent_rate, true);
}

static int sam9x60_frac_pll_set_rate_chg(struct clk_hw *hw, unsigned long rate,
					 unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_frac *frac = to_sam9x60_frac(core);
	struct regmap *regmap = core->regmap;
	unsigned long irqflags;
	unsigned int val, cfrac, cmul;
	long ret;

	ret = sam9x60_frac_pll_compute_mul_frac(core, rate, parent_rate, true);
	if (ret <= 0)
		return ret;

	spin_lock_irqsave(core->lock, irqflags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT, AT91_PMC_PLL_UPDT_ID_MSK,
			   core->id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL1, &val);
	cmul = (val & core->layout->mul_mask) >> core->layout->mul_shift;
	cfrac = (val & core->layout->frac_mask) >> core->layout->frac_shift;

	if (cmul == frac->mul && cfrac == frac->frac)
		goto unlock;

	regmap_write(regmap, AT91_PMC_PLL_CTRL1,
		     (frac->mul << core->layout->mul_shift) |
		     (frac->frac << core->layout->frac_shift));

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0,
			   AT91_PMC_PLL_CTRL0_ENLOCK | AT91_PMC_PLL_CTRL0_ENPLL,
			   AT91_PMC_PLL_CTRL0_ENLOCK |
			   AT91_PMC_PLL_CTRL0_ENPLL);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	while (!sam9x60_pll_ready(regmap, core->id))
		cpu_relax();

unlock:
	spin_unlock_irqrestore(core->lock, irqflags);

	return ret;
}

static const struct clk_ops sam9x60_frac_pll_ops = {
	.prepare = sam9x60_frac_pll_prepare,
	.unprepare = sam9x60_frac_pll_unprepare,
	.is_prepared = sam9x60_frac_pll_is_prepared,
	.recalc_rate = sam9x60_frac_pll_recalc_rate,
	.round_rate = sam9x60_frac_pll_round_rate,
	.set_rate = sam9x60_frac_pll_set_rate,
};

static const struct clk_ops sam9x60_frac_pll_ops_chg = {
	.prepare = sam9x60_frac_pll_prepare,
	.unprepare = sam9x60_frac_pll_unprepare,
	.is_prepared = sam9x60_frac_pll_is_prepared,
	.recalc_rate = sam9x60_frac_pll_recalc_rate,
	.round_rate = sam9x60_frac_pll_round_rate,
	.set_rate = sam9x60_frac_pll_set_rate_chg,
};

static int sam9x60_div_pll_prepare(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_div *div = to_sam9x60_div(core);
	struct regmap *regmap = core->regmap;
	unsigned long flags;
	unsigned int val, cdiv;

	spin_lock_irqsave(core->lock, flags);
	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, core->id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL0, &val);
	cdiv = (val & core->layout->div_mask) >> core->layout->div_shift;

	/* Stop if enabled an nothing changed. */
	if (!!(val & core->layout->endiv_mask) && cdiv == div->div)
		goto unlock;

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0,
			   core->layout->div_mask | core->layout->endiv_mask,
			   (div->div << core->layout->div_shift) |
			   (1 << core->layout->endiv_shift));

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	while (!sam9x60_pll_ready(regmap, core->id))
		cpu_relax();

unlock:
	spin_unlock_irqrestore(core->lock, flags);

	return 0;
}

static void sam9x60_div_pll_unprepare(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct regmap *regmap = core->regmap;
	unsigned long flags;

	spin_lock_irqsave(core->lock, flags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, core->id);

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0,
			   core->layout->endiv_mask, 0);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	spin_unlock_irqrestore(core->lock, flags);
}

static int sam9x60_div_pll_is_prepared(struct clk_hw *hw)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct regmap *regmap = core->regmap;
	unsigned long flags;
	unsigned int val;

	spin_lock_irqsave(core->lock, flags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, core->id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL0, &val);

	spin_unlock_irqrestore(core->lock, flags);

	return !!(val & core->layout->endiv_mask);
}

static unsigned long sam9x60_div_pll_recalc_rate(struct clk_hw *hw,
						 unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_div *div = to_sam9x60_div(core);

	return DIV_ROUND_CLOSEST_ULL(parent_rate, (div->div + 1));
}

static long sam9x60_div_pll_compute_div(struct sam9x60_pll_core *core,
					unsigned long *parent_rate,
					unsigned long rate)
{
	const struct clk_pll_characteristics *characteristics =
							core->characteristics;
	struct clk_hw *parent = clk_hw_get_parent(&core->hw);
	unsigned long tmp_rate, tmp_parent_rate, tmp_diff;
	long best_diff = -1, best_rate = -EINVAL;
	u32 divid;

	if (!rate)
		return 0;

	if (rate < characteristics->output[0].min ||
	    rate > characteristics->output[0].max)
		return -ERANGE;

	for (divid = 1; divid < core->layout->div_mask; divid++) {
		tmp_parent_rate = clk_hw_round_rate(parent, rate * divid);
		if (!tmp_parent_rate)
			continue;

		tmp_rate = DIV_ROUND_CLOSEST_ULL(tmp_parent_rate, divid);
		tmp_diff = abs(rate - tmp_rate);

		if (best_diff < 0 || best_diff > tmp_diff) {
			*parent_rate = tmp_parent_rate;
			best_rate = tmp_rate;
			best_diff = tmp_diff;
		}

		if (!best_diff)
			break;
	}

	if (best_rate < characteristics->output[0].min ||
	    best_rate > characteristics->output[0].max)
		return -ERANGE;

	return best_rate;
}

static long sam9x60_div_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);

	return sam9x60_div_pll_compute_div(core, parent_rate, rate);
}

static int sam9x60_div_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_div *div = to_sam9x60_div(core);

	div->div = DIV_ROUND_CLOSEST(parent_rate, rate) - 1;

	return 0;
}

static int sam9x60_div_pll_set_rate_chg(struct clk_hw *hw, unsigned long rate,
					unsigned long parent_rate)
{
	struct sam9x60_pll_core *core = to_sam9x60_pll_core(hw);
	struct sam9x60_div *div = to_sam9x60_div(core);
	struct regmap *regmap = core->regmap;
	unsigned long irqflags;
	unsigned int val, cdiv;

	div->div = DIV_ROUND_CLOSEST(parent_rate, rate) - 1;

	spin_lock_irqsave(core->lock, irqflags);
	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT, AT91_PMC_PLL_UPDT_ID_MSK,
			   core->id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL0, &val);
	cdiv = (val & core->layout->div_mask) >> core->layout->div_shift;

	/* Stop if nothing changed. */
	if (cdiv == div->div)
		goto unlock;

	regmap_update_bits(regmap, AT91_PMC_PLL_CTRL0,
			   core->layout->div_mask,
			   (div->div << core->layout->div_shift));

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_UPDATE | AT91_PMC_PLL_UPDT_ID_MSK,
			   AT91_PMC_PLL_UPDT_UPDATE | core->id);

	while (!sam9x60_pll_ready(regmap, core->id))
		cpu_relax();

unlock:
	spin_unlock_irqrestore(core->lock, irqflags);

	return 0;
}

static const struct clk_ops sam9x60_div_pll_ops = {
	.prepare = sam9x60_div_pll_prepare,
	.unprepare = sam9x60_div_pll_unprepare,
	.is_prepared = sam9x60_div_pll_is_prepared,
	.recalc_rate = sam9x60_div_pll_recalc_rate,
	.round_rate = sam9x60_div_pll_round_rate,
	.set_rate = sam9x60_div_pll_set_rate,
};

static const struct clk_ops sam9x60_div_pll_ops_chg = {
	.prepare = sam9x60_div_pll_prepare,
	.unprepare = sam9x60_div_pll_unprepare,
	.is_prepared = sam9x60_div_pll_is_prepared,
	.recalc_rate = sam9x60_div_pll_recalc_rate,
	.round_rate = sam9x60_div_pll_round_rate,
	.set_rate = sam9x60_div_pll_set_rate_chg,
};

struct clk_hw * __init
sam9x60_clk_register_frac_pll(struct regmap *regmap, spinlock_t *lock,
			      const char *name, const char *parent_name,
			      struct clk_hw *parent_hw, u8 id,
			      const struct clk_pll_characteristics *characteristics,
			      const struct clk_pll_layout *layout, u32 flags)
{
	struct sam9x60_frac *frac;
	struct clk_hw *hw;
	struct clk_init_data init;
	unsigned long parent_rate, irqflags;
	unsigned int val;
	int ret;

	if (id > PLL_MAX_ID || !lock || !parent_hw)
		return ERR_PTR(-EINVAL);

	frac = kzalloc(sizeof(*frac), GFP_KERNEL);
	if (!frac)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.parent_names = &parent_name;
	init.num_parents = 1;
	if (flags & CLK_SET_RATE_GATE)
		init.ops = &sam9x60_frac_pll_ops;
	else
		init.ops = &sam9x60_frac_pll_ops_chg;

	init.flags = flags;

	frac->core.id = id;
	frac->core.hw.init = &init;
	frac->core.characteristics = characteristics;
	frac->core.layout = layout;
	frac->core.regmap = regmap;
	frac->core.lock = lock;

	spin_lock_irqsave(frac->core.lock, irqflags);
	if (sam9x60_pll_ready(regmap, id)) {
		regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
				   AT91_PMC_PLL_UPDT_ID_MSK, id);
		regmap_read(regmap, AT91_PMC_PLL_CTRL1, &val);
		frac->mul = FIELD_GET(PMC_PLL_CTRL1_MUL_MSK, val);
		frac->frac = FIELD_GET(PMC_PLL_CTRL1_FRACR_MSK, val);
	} else {
		/*
		 * This means the PLL is not setup by bootloaders. In this
		 * case we need to set the minimum rate for it. Otherwise
		 * a clock child of this PLL may be enabled before setting
		 * its rate leading to enabling this PLL with unsupported
		 * rate. This will lead to PLL not being locked at all.
		 */
		parent_rate = clk_hw_get_rate(parent_hw);
		if (!parent_rate) {
			hw = ERR_PTR(-EINVAL);
			goto free;
		}

		ret = sam9x60_frac_pll_compute_mul_frac(&frac->core, FCORE_MIN,
							parent_rate, true);
		if (ret <= 0) {
			hw = ERR_PTR(ret);
			goto free;
		}
	}
	spin_unlock_irqrestore(frac->core.lock, irqflags);

	hw = &frac->core.hw;
	ret = clk_hw_register(NULL, hw);
	if (ret) {
		kfree(frac);
		hw = ERR_PTR(ret);
	}

	return hw;

free:
	spin_unlock_irqrestore(frac->core.lock, irqflags);
	kfree(frac);
	return hw;
}

struct clk_hw * __init
sam9x60_clk_register_div_pll(struct regmap *regmap, spinlock_t *lock,
			     const char *name, const char *parent_name, u8 id,
			     const struct clk_pll_characteristics *characteristics,
			     const struct clk_pll_layout *layout, u32 flags)
{
	struct sam9x60_div *div;
	struct clk_hw *hw;
	struct clk_init_data init;
	unsigned long irqflags;
	unsigned int val;
	int ret;

	if (id > PLL_MAX_ID || !lock)
		return ERR_PTR(-EINVAL);

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.parent_names = &parent_name;
	init.num_parents = 1;
	if (flags & CLK_SET_RATE_GATE)
		init.ops = &sam9x60_div_pll_ops;
	else
		init.ops = &sam9x60_div_pll_ops_chg;
	init.flags = flags;

	div->core.id = id;
	div->core.hw.init = &init;
	div->core.characteristics = characteristics;
	div->core.layout = layout;
	div->core.regmap = regmap;
	div->core.lock = lock;

	spin_lock_irqsave(div->core.lock, irqflags);

	regmap_update_bits(regmap, AT91_PMC_PLL_UPDT,
			   AT91_PMC_PLL_UPDT_ID_MSK, id);
	regmap_read(regmap, AT91_PMC_PLL_CTRL0, &val);
	div->div = FIELD_GET(PMC_PLL_CTRL0_DIV_MSK, val);

	spin_unlock_irqrestore(div->core.lock, irqflags);

	hw = &div->core.hw;
	ret = clk_hw_register(NULL, hw);
	if (ret) {
		kfree(div);
		hw = ERR_PTR(ret);
	}

	return hw;
}

