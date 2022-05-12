// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2013 Boris BREZILLON <b.brezillon@overkiz.com>
 */

#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
#include <linux/mfd/syscon.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/syscore_ops.h>

#include <asm/proc-fns.h>

#include <dt-bindings/clock/at91.h>

#include "pmc.h"

#define PMC_MAX_IDS 128
#define PMC_MAX_PCKS 8

int of_at91_get_clk_range(struct device_node *np, const char *propname,
			  struct clk_range *range)
{
	u32 min, max;
	int ret;

	ret = of_property_read_u32_index(np, propname, 0, &min);
	if (ret)
		return ret;

	ret = of_property_read_u32_index(np, propname, 1, &max);
	if (ret)
		return ret;

	if (range) {
		range->min = min;
		range->max = max;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(of_at91_get_clk_range);

struct clk_hw *of_clk_hw_pmc_get(struct of_phandle_args *clkspec, void *data)
{
	unsigned int type = clkspec->args[0];
	unsigned int idx = clkspec->args[1];
	struct pmc_data *pmc_data = data;

	switch (type) {
	case PMC_TYPE_CORE:
		if (idx < pmc_data->ncore)
			return pmc_data->chws[idx];
		break;
	case PMC_TYPE_SYSTEM:
		if (idx < pmc_data->nsystem)
			return pmc_data->shws[idx];
		break;
	case PMC_TYPE_PERIPHERAL:
		if (idx < pmc_data->nperiph)
			return pmc_data->phws[idx];
		break;
	case PMC_TYPE_GCK:
		if (idx < pmc_data->ngck)
			return pmc_data->ghws[idx];
		break;
	case PMC_TYPE_PROGRAMMABLE:
		if (idx < pmc_data->npck)
			return pmc_data->pchws[idx];
		break;
	default:
		break;
	}

	pr_err("%s: invalid type (%u) or index (%u)\n", __func__, type, idx);

	return ERR_PTR(-EINVAL);
}

struct pmc_data *pmc_data_allocate(unsigned int ncore, unsigned int nsystem,
				   unsigned int nperiph, unsigned int ngck,
				   unsigned int npck)
{
	unsigned int num_clks = ncore + nsystem + nperiph + ngck + npck;
	struct pmc_data *pmc_data;

	pmc_data = kzalloc(struct_size(pmc_data, hwtable, num_clks),
			   GFP_KERNEL);
	if (!pmc_data)
		return NULL;

	pmc_data->ncore = ncore;
	pmc_data->chws = pmc_data->hwtable;

	pmc_data->nsystem = nsystem;
	pmc_data->shws = pmc_data->chws + ncore;

	pmc_data->nperiph = nperiph;
	pmc_data->phws = pmc_data->shws + nsystem;

	pmc_data->ngck = ngck;
	pmc_data->ghws = pmc_data->phws + nperiph;

	pmc_data->npck = npck;
	pmc_data->pchws = pmc_data->ghws + ngck;

	return pmc_data;
}

#ifdef CONFIG_PM
static struct regmap *pmcreg;

static u8 registered_ids[PMC_MAX_IDS];
static u8 registered_pcks[PMC_MAX_PCKS];

static struct
{
	u32 scsr;
	u32 pcsr0;
	u32 uckr;
	u32 mor;
	u32 mcfr;
	u32 pllar;
	u32 mckr;
	u32 usb;
	u32 imr;
	u32 pcsr1;
	u32 pcr[PMC_MAX_IDS];
	u32 audio_pll0;
	u32 audio_pll1;
	u32 pckr[PMC_MAX_PCKS];
} pmc_cache;

/*
 * As Peripheral ID 0 is invalid on AT91 chips, the identifier is stored
 * without alteration in the table, and 0 is for unused clocks.
 */
void pmc_register_id(u8 id)
{
	int i;

	for (i = 0; i < PMC_MAX_IDS; i++) {
		if (registered_ids[i] == 0) {
			registered_ids[i] = id;
			break;
		}
		if (registered_ids[i] == id)
			break;
	}
}

/*
 * As Programmable Clock 0 is valid on AT91 chips, there is an offset
 * of 1 between the stored value and the real clock ID.
 */
void pmc_register_pck(u8 pck)
{
	int i;

	for (i = 0; i < PMC_MAX_PCKS; i++) {
		if (registered_pcks[i] == 0) {
			registered_pcks[i] = pck + 1;
			break;
		}
		if (registered_pcks[i] == (pck + 1))
			break;
	}
}

static int pmc_suspend(void)
{
	int i;
	u8 num;

	regmap_read(pmcreg, AT91_PMC_SCSR, &pmc_cache.scsr);
	regmap_read(pmcreg, AT91_PMC_PCSR, &pmc_cache.pcsr0);
	regmap_read(pmcreg, AT91_CKGR_UCKR, &pmc_cache.uckr);
	regmap_read(pmcreg, AT91_CKGR_MOR, &pmc_cache.mor);
	regmap_read(pmcreg, AT91_CKGR_MCFR, &pmc_cache.mcfr);
	regmap_read(pmcreg, AT91_CKGR_PLLAR, &pmc_cache.pllar);
	regmap_read(pmcreg, AT91_PMC_MCKR, &pmc_cache.mckr);
	regmap_read(pmcreg, AT91_PMC_USB, &pmc_cache.usb);
	regmap_read(pmcreg, AT91_PMC_IMR, &pmc_cache.imr);
	regmap_read(pmcreg, AT91_PMC_PCSR1, &pmc_cache.pcsr1);

	for (i = 0; registered_ids[i]; i++) {
		regmap_write(pmcreg, AT91_PMC_PCR,
			     (registered_ids[i] & AT91_PMC_PCR_PID_MASK));
		regmap_read(pmcreg, AT91_PMC_PCR,
			    &pmc_cache.pcr[registered_ids[i]]);
	}
	for (i = 0; registered_pcks[i]; i++) {
		num = registered_pcks[i] - 1;
		regmap_read(pmcreg, AT91_PMC_PCKR(num), &pmc_cache.pckr[num]);
	}

	return 0;
}

static bool pmc_ready(unsigned int mask)
{
	unsigned int status;

	regmap_read(pmcreg, AT91_PMC_SR, &status);

	return ((status & mask) == mask) ? 1 : 0;
}

static void pmc_resume(void)
{
	int i;
	u8 num;
	u32 tmp;
	u32 mask = AT91_PMC_MCKRDY | AT91_PMC_LOCKA;

	regmap_read(pmcreg, AT91_PMC_MCKR, &tmp);
	if (pmc_cache.mckr != tmp)
		pr_warn("MCKR was not configured properly by the firmware\n");
	regmap_read(pmcreg, AT91_CKGR_PLLAR, &tmp);
	if (pmc_cache.pllar != tmp)
		pr_warn("PLLAR was not configured properly by the firmware\n");

	regmap_write(pmcreg, AT91_PMC_SCER, pmc_cache.scsr);
	regmap_write(pmcreg, AT91_PMC_PCER, pmc_cache.pcsr0);
	regmap_write(pmcreg, AT91_CKGR_UCKR, pmc_cache.uckr);
	regmap_write(pmcreg, AT91_CKGR_MOR, pmc_cache.mor);
	regmap_write(pmcreg, AT91_CKGR_MCFR, pmc_cache.mcfr);
	regmap_write(pmcreg, AT91_PMC_USB, pmc_cache.usb);
	regmap_write(pmcreg, AT91_PMC_IMR, pmc_cache.imr);
	regmap_write(pmcreg, AT91_PMC_PCER1, pmc_cache.pcsr1);

	for (i = 0; registered_ids[i]; i++) {
		regmap_write(pmcreg, AT91_PMC_PCR,
			     pmc_cache.pcr[registered_ids[i]] |
			     AT91_PMC_PCR_CMD);
	}
	for (i = 0; registered_pcks[i]; i++) {
		num = registered_pcks[i] - 1;
		regmap_write(pmcreg, AT91_PMC_PCKR(num), pmc_cache.pckr[num]);
	}

	if (pmc_cache.uckr & AT91_PMC_UPLLEN)
		mask |= AT91_PMC_LOCKU;

	while (!pmc_ready(mask))
		cpu_relax();
}

static struct syscore_ops pmc_syscore_ops = {
	.suspend = pmc_suspend,
	.resume = pmc_resume,
};

static const struct of_device_id sama5d2_pmc_dt_ids[] = {
	{ .compatible = "atmel,sama5d2-pmc" },
	{ /* sentinel */ }
};

static int __init pmc_register_ops(void)
{
	struct device_node *np;

	np = of_find_matching_node(NULL, sama5d2_pmc_dt_ids);
	if (!np)
		return -ENODEV;

	pmcreg = device_node_to_regmap(np);
	of_node_put(np);
	if (IS_ERR(pmcreg))
		return PTR_ERR(pmcreg);

	register_syscore_ops(&pmc_syscore_ops);

	return 0;
}
/* This has to happen before arch_initcall because of the tcb_clksrc driver */
postcore_initcall(pmc_register_ops);
#endif
