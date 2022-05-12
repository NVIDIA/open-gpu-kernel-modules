// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * arch/arm/mach-at91/pm.c
 * AT91 Power Management
 *
 * Copyright (C) 2005 David Brownell
 */

#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/parser.h>
#include <linux/suspend.h>

#include <linux/clk/at91_pmc.h>
#include <linux/platform_data/atmel.h>

#include <soc/at91/pm.h>

#include <asm/cacheflush.h>
#include <asm/fncpy.h>
#include <asm/system_misc.h>
#include <asm/suspend.h>

#include "generic.h"
#include "pm.h"

struct at91_soc_pm {
	int (*config_shdwc_ws)(void __iomem *shdwc, u32 *mode, u32 *polarity);
	int (*config_pmc_ws)(void __iomem *pmc, u32 mode, u32 polarity);
	const struct of_device_id *ws_ids;
	struct at91_pm_data data;
};

static struct at91_soc_pm soc_pm = {
	.data = {
		.standby_mode = AT91_PM_STANDBY,
		.suspend_mode = AT91_PM_ULP0,
	},
};

static const match_table_t pm_modes __initconst = {
	{ AT91_PM_STANDBY,	"standby" },
	{ AT91_PM_ULP0,		"ulp0" },
	{ AT91_PM_ULP0_FAST,    "ulp0-fast" },
	{ AT91_PM_ULP1,		"ulp1" },
	{ AT91_PM_BACKUP,	"backup" },
	{ -1, NULL },
};

#define at91_ramc_read(id, field) \
	__raw_readl(soc_pm.data.ramc[id] + field)

#define at91_ramc_write(id, field, value) \
	__raw_writel(value, soc_pm.data.ramc[id] + field)

static int at91_pm_valid_state(suspend_state_t state)
{
	switch (state) {
		case PM_SUSPEND_ON:
		case PM_SUSPEND_STANDBY:
		case PM_SUSPEND_MEM:
			return 1;

		default:
			return 0;
	}
}

static int canary = 0xA5A5A5A5;

static struct at91_pm_bu {
	int suspended;
	unsigned long reserved;
	phys_addr_t canary;
	phys_addr_t resume;
} *pm_bu;

struct wakeup_source_info {
	unsigned int pmc_fsmr_bit;
	unsigned int shdwc_mr_bit;
	bool set_polarity;
};

static const struct wakeup_source_info ws_info[] = {
	{ .pmc_fsmr_bit = AT91_PMC_FSTT(10),	.set_polarity = true },
	{ .pmc_fsmr_bit = AT91_PMC_RTCAL,	.shdwc_mr_bit = BIT(17) },
	{ .pmc_fsmr_bit = AT91_PMC_USBAL },
	{ .pmc_fsmr_bit = AT91_PMC_SDMMC_CD },
	{ .pmc_fsmr_bit = AT91_PMC_RTTAL },
	{ .pmc_fsmr_bit = AT91_PMC_RXLP_MCE },
};

static const struct of_device_id sama5d2_ws_ids[] = {
	{ .compatible = "atmel,sama5d2-gem",		.data = &ws_info[0] },
	{ .compatible = "atmel,at91rm9200-rtc",		.data = &ws_info[1] },
	{ .compatible = "atmel,sama5d3-udc",		.data = &ws_info[2] },
	{ .compatible = "atmel,at91rm9200-ohci",	.data = &ws_info[2] },
	{ .compatible = "usb-ohci",			.data = &ws_info[2] },
	{ .compatible = "atmel,at91sam9g45-ehci",	.data = &ws_info[2] },
	{ .compatible = "usb-ehci",			.data = &ws_info[2] },
	{ .compatible = "atmel,sama5d2-sdhci",		.data = &ws_info[3] },
	{ /* sentinel */ }
};

static const struct of_device_id sam9x60_ws_ids[] = {
	{ .compatible = "atmel,at91sam9x5-rtc",		.data = &ws_info[1] },
	{ .compatible = "atmel,at91rm9200-ohci",	.data = &ws_info[2] },
	{ .compatible = "usb-ohci",			.data = &ws_info[2] },
	{ .compatible = "atmel,at91sam9g45-ehci",	.data = &ws_info[2] },
	{ .compatible = "usb-ehci",			.data = &ws_info[2] },
	{ .compatible = "atmel,at91sam9260-rtt",	.data = &ws_info[4] },
	{ .compatible = "cdns,sam9x60-macb",		.data = &ws_info[5] },
	{ /* sentinel */ }
};

static int at91_pm_config_ws(unsigned int pm_mode, bool set)
{
	const struct wakeup_source_info *wsi;
	const struct of_device_id *match;
	struct platform_device *pdev;
	struct device_node *np;
	unsigned int mode = 0, polarity = 0, val = 0;

	if (pm_mode != AT91_PM_ULP1)
		return 0;

	if (!soc_pm.data.pmc || !soc_pm.data.shdwc || !soc_pm.ws_ids)
		return -EPERM;

	if (!set) {
		writel(mode, soc_pm.data.pmc + AT91_PMC_FSMR);
		return 0;
	}

	if (soc_pm.config_shdwc_ws)
		soc_pm.config_shdwc_ws(soc_pm.data.shdwc, &mode, &polarity);

	/* SHDWC.MR */
	val = readl(soc_pm.data.shdwc + 0x04);

	/* Loop through defined wakeup sources. */
	for_each_matching_node_and_match(np, soc_pm.ws_ids, &match) {
		pdev = of_find_device_by_node(np);
		if (!pdev)
			continue;

		if (device_may_wakeup(&pdev->dev)) {
			wsi = match->data;

			/* Check if enabled on SHDWC. */
			if (wsi->shdwc_mr_bit && !(val & wsi->shdwc_mr_bit))
				goto put_device;

			mode |= wsi->pmc_fsmr_bit;
			if (wsi->set_polarity)
				polarity |= wsi->pmc_fsmr_bit;
		}

put_device:
		put_device(&pdev->dev);
	}

	if (mode) {
		if (soc_pm.config_pmc_ws)
			soc_pm.config_pmc_ws(soc_pm.data.pmc, mode, polarity);
	} else {
		pr_err("AT91: PM: no ULP1 wakeup sources found!");
	}

	return mode ? 0 : -EPERM;
}

static int at91_sama5d2_config_shdwc_ws(void __iomem *shdwc, u32 *mode,
					u32 *polarity)
{
	u32 val;

	/* SHDWC.WUIR */
	val = readl(shdwc + 0x0c);
	*mode |= (val & 0x3ff);
	*polarity |= ((val >> 16) & 0x3ff);

	return 0;
}

static int at91_sama5d2_config_pmc_ws(void __iomem *pmc, u32 mode, u32 polarity)
{
	writel(mode, pmc + AT91_PMC_FSMR);
	writel(polarity, pmc + AT91_PMC_FSPR);

	return 0;
}

static int at91_sam9x60_config_pmc_ws(void __iomem *pmc, u32 mode, u32 polarity)
{
	writel(mode, pmc + AT91_PMC_FSMR);

	return 0;
}

/*
 * Called after processes are frozen, but before we shutdown devices.
 */
static int at91_pm_begin(suspend_state_t state)
{
	switch (state) {
	case PM_SUSPEND_MEM:
		soc_pm.data.mode = soc_pm.data.suspend_mode;
		break;

	case PM_SUSPEND_STANDBY:
		soc_pm.data.mode = soc_pm.data.standby_mode;
		break;

	default:
		soc_pm.data.mode = -1;
	}

	return at91_pm_config_ws(soc_pm.data.mode, true);
}

/*
 * Verify that all the clocks are correct before entering
 * slow-clock mode.
 */
static int at91_pm_verify_clocks(void)
{
	unsigned long scsr;
	int i;

	scsr = readl(soc_pm.data.pmc + AT91_PMC_SCSR);

	/* USB must not be using PLLB */
	if ((scsr & soc_pm.data.uhp_udp_mask) != 0) {
		pr_err("AT91: PM - Suspend-to-RAM with USB still active\n");
		return 0;
	}

	/* PCK0..PCK3 must be disabled, or configured to use clk32k */
	for (i = 0; i < 4; i++) {
		u32 css;

		if ((scsr & (AT91_PMC_PCK0 << i)) == 0)
			continue;
		css = readl(soc_pm.data.pmc + AT91_PMC_PCKR(i)) & AT91_PMC_CSS;
		if (css != AT91_PMC_CSS_SLOW) {
			pr_err("AT91: PM - Suspend-to-RAM with PCK%d src %d\n", i, css);
			return 0;
		}
	}

	return 1;
}

/*
 * Call this from platform driver suspend() to see how deeply to suspend.
 * For example, some controllers (like OHCI) need one of the PLL clocks
 * in order to act as a wakeup source, and those are not available when
 * going into slow clock mode.
 *
 * REVISIT: generalize as clk_will_be_available(clk)?  Other platforms have
 * the very same problem (but not using at91 main_clk), and it'd be better
 * to add one generic API rather than lots of platform-specific ones.
 */
int at91_suspend_entering_slow_clock(void)
{
	return (soc_pm.data.mode >= AT91_PM_ULP0);
}
EXPORT_SYMBOL(at91_suspend_entering_slow_clock);

static void (*at91_suspend_sram_fn)(struct at91_pm_data *);
extern void at91_pm_suspend_in_sram(struct at91_pm_data *pm_data);
extern u32 at91_pm_suspend_in_sram_sz;

static int at91_suspend_finish(unsigned long val)
{
	flush_cache_all();
	outer_disable();

	at91_suspend_sram_fn(&soc_pm.data);

	return 0;
}

static void at91_pm_suspend(suspend_state_t state)
{
	if (soc_pm.data.mode == AT91_PM_BACKUP) {
		pm_bu->suspended = 1;

		cpu_suspend(0, at91_suspend_finish);

		/* The SRAM is lost between suspend cycles */
		at91_suspend_sram_fn = fncpy(at91_suspend_sram_fn,
					     &at91_pm_suspend_in_sram,
					     at91_pm_suspend_in_sram_sz);
	} else {
		at91_suspend_finish(0);
	}

	outer_resume();
}

/*
 * STANDBY mode has *all* drivers suspended; ignores irqs not marked as 'wakeup'
 * event sources; and reduces DRAM power.  But otherwise it's identical to
 * PM_SUSPEND_ON: cpu idle, and nothing fancy done with main or cpu clocks.
 *
 * AT91_PM_ULP0 is like STANDBY plus slow clock mode, so drivers must
 * suspend more deeply, the master clock switches to the clk32k and turns off
 * the main oscillator
 *
 * AT91_PM_BACKUP turns off the whole SoC after placing the DDR in self refresh
 */
static int at91_pm_enter(suspend_state_t state)
{
#ifdef CONFIG_PINCTRL_AT91
	/*
	 * FIXME: this is needed to communicate between the pinctrl driver and
	 * the PM implementation in the machine. Possibly part of the PM
	 * implementation should be moved down into the pinctrl driver and get
	 * called as part of the generic suspend/resume path.
	 */
	at91_pinctrl_gpio_suspend();
#endif

	switch (state) {
	case PM_SUSPEND_MEM:
	case PM_SUSPEND_STANDBY:
		/*
		 * Ensure that clocks are in a valid state.
		 */
		if (soc_pm.data.mode >= AT91_PM_ULP0 &&
		    !at91_pm_verify_clocks())
			goto error;

		at91_pm_suspend(state);

		break;

	case PM_SUSPEND_ON:
		cpu_do_idle();
		break;

	default:
		pr_debug("AT91: PM - bogus suspend state %d\n", state);
		goto error;
	}

error:
#ifdef CONFIG_PINCTRL_AT91
	at91_pinctrl_gpio_resume();
#endif
	return 0;
}

/*
 * Called right prior to thawing processes.
 */
static void at91_pm_end(void)
{
	at91_pm_config_ws(soc_pm.data.mode, false);
}


static const struct platform_suspend_ops at91_pm_ops = {
	.valid	= at91_pm_valid_state,
	.begin	= at91_pm_begin,
	.enter	= at91_pm_enter,
	.end	= at91_pm_end,
};

static struct platform_device at91_cpuidle_device = {
	.name = "cpuidle-at91",
};

/*
 * The AT91RM9200 goes into self-refresh mode with this command, and will
 * terminate self-refresh automatically on the next SDRAM access.
 *
 * Self-refresh mode is exited as soon as a memory access is made, but we don't
 * know for sure when that happens. However, we need to restore the low-power
 * mode if it was enabled before going idle. Restoring low-power mode while
 * still in self-refresh is "not recommended", but seems to work.
 */
static void at91rm9200_standby(void)
{
	asm volatile(
		"b    1f\n\t"
		".align    5\n\t"
		"1:  mcr    p15, 0, %0, c7, c10, 4\n\t"
		"    str    %2, [%1, %3]\n\t"
		"    mcr    p15, 0, %0, c7, c0, 4\n\t"
		:
		: "r" (0), "r" (soc_pm.data.ramc[0]),
		  "r" (1), "r" (AT91_MC_SDRAMC_SRR));
}

/* We manage both DDRAM/SDRAM controllers, we need more than one value to
 * remember.
 */
static void at91_ddr_standby(void)
{
	/* Those two values allow us to delay self-refresh activation
	 * to the maximum. */
	u32 lpr0, lpr1 = 0;
	u32 mdr, saved_mdr0, saved_mdr1 = 0;
	u32 saved_lpr0, saved_lpr1 = 0;

	/* LPDDR1 --> force DDR2 mode during self-refresh */
	saved_mdr0 = at91_ramc_read(0, AT91_DDRSDRC_MDR);
	if ((saved_mdr0 & AT91_DDRSDRC_MD) == AT91_DDRSDRC_MD_LOW_POWER_DDR) {
		mdr = saved_mdr0 & ~AT91_DDRSDRC_MD;
		mdr |= AT91_DDRSDRC_MD_DDR2;
		at91_ramc_write(0, AT91_DDRSDRC_MDR, mdr);
	}

	if (soc_pm.data.ramc[1]) {
		saved_lpr1 = at91_ramc_read(1, AT91_DDRSDRC_LPR);
		lpr1 = saved_lpr1 & ~AT91_DDRSDRC_LPCB;
		lpr1 |= AT91_DDRSDRC_LPCB_SELF_REFRESH;
		saved_mdr1 = at91_ramc_read(1, AT91_DDRSDRC_MDR);
		if ((saved_mdr1 & AT91_DDRSDRC_MD) == AT91_DDRSDRC_MD_LOW_POWER_DDR) {
			mdr = saved_mdr1 & ~AT91_DDRSDRC_MD;
			mdr |= AT91_DDRSDRC_MD_DDR2;
			at91_ramc_write(1, AT91_DDRSDRC_MDR, mdr);
		}
	}

	saved_lpr0 = at91_ramc_read(0, AT91_DDRSDRC_LPR);
	lpr0 = saved_lpr0 & ~AT91_DDRSDRC_LPCB;
	lpr0 |= AT91_DDRSDRC_LPCB_SELF_REFRESH;

	/* self-refresh mode now */
	at91_ramc_write(0, AT91_DDRSDRC_LPR, lpr0);
	if (soc_pm.data.ramc[1])
		at91_ramc_write(1, AT91_DDRSDRC_LPR, lpr1);

	cpu_do_idle();

	at91_ramc_write(0, AT91_DDRSDRC_MDR, saved_mdr0);
	at91_ramc_write(0, AT91_DDRSDRC_LPR, saved_lpr0);
	if (soc_pm.data.ramc[1]) {
		at91_ramc_write(0, AT91_DDRSDRC_MDR, saved_mdr1);
		at91_ramc_write(1, AT91_DDRSDRC_LPR, saved_lpr1);
	}
}

static void sama5d3_ddr_standby(void)
{
	u32 lpr0;
	u32 saved_lpr0;

	saved_lpr0 = at91_ramc_read(0, AT91_DDRSDRC_LPR);
	lpr0 = saved_lpr0 & ~AT91_DDRSDRC_LPCB;
	lpr0 |= AT91_DDRSDRC_LPCB_POWER_DOWN;

	at91_ramc_write(0, AT91_DDRSDRC_LPR, lpr0);

	cpu_do_idle();

	at91_ramc_write(0, AT91_DDRSDRC_LPR, saved_lpr0);
}

/* We manage both DDRAM/SDRAM controllers, we need more than one value to
 * remember.
 */
static void at91sam9_sdram_standby(void)
{
	u32 lpr0, lpr1 = 0;
	u32 saved_lpr0, saved_lpr1 = 0;

	if (soc_pm.data.ramc[1]) {
		saved_lpr1 = at91_ramc_read(1, AT91_SDRAMC_LPR);
		lpr1 = saved_lpr1 & ~AT91_SDRAMC_LPCB;
		lpr1 |= AT91_SDRAMC_LPCB_SELF_REFRESH;
	}

	saved_lpr0 = at91_ramc_read(0, AT91_SDRAMC_LPR);
	lpr0 = saved_lpr0 & ~AT91_SDRAMC_LPCB;
	lpr0 |= AT91_SDRAMC_LPCB_SELF_REFRESH;

	/* self-refresh mode now */
	at91_ramc_write(0, AT91_SDRAMC_LPR, lpr0);
	if (soc_pm.data.ramc[1])
		at91_ramc_write(1, AT91_SDRAMC_LPR, lpr1);

	cpu_do_idle();

	at91_ramc_write(0, AT91_SDRAMC_LPR, saved_lpr0);
	if (soc_pm.data.ramc[1])
		at91_ramc_write(1, AT91_SDRAMC_LPR, saved_lpr1);
}

struct ramc_info {
	void (*idle)(void);
	unsigned int memctrl;
};

static const struct ramc_info ramc_infos[] __initconst = {
	{ .idle = at91rm9200_standby, .memctrl = AT91_MEMCTRL_MC},
	{ .idle = at91sam9_sdram_standby, .memctrl = AT91_MEMCTRL_SDRAMC},
	{ .idle = at91_ddr_standby, .memctrl = AT91_MEMCTRL_DDRSDR},
	{ .idle = sama5d3_ddr_standby, .memctrl = AT91_MEMCTRL_DDRSDR},
};

static const struct of_device_id ramc_ids[] __initconst = {
	{ .compatible = "atmel,at91rm9200-sdramc", .data = &ramc_infos[0] },
	{ .compatible = "atmel,at91sam9260-sdramc", .data = &ramc_infos[1] },
	{ .compatible = "atmel,at91sam9g45-ddramc", .data = &ramc_infos[2] },
	{ .compatible = "atmel,sama5d3-ddramc", .data = &ramc_infos[3] },
	{ /*sentinel*/ }
};

static __init void at91_dt_ramc(void)
{
	struct device_node *np;
	const struct of_device_id *of_id;
	int idx = 0;
	void *standby = NULL;
	const struct ramc_info *ramc;

	for_each_matching_node_and_match(np, ramc_ids, &of_id) {
		soc_pm.data.ramc[idx] = of_iomap(np, 0);
		if (!soc_pm.data.ramc[idx])
			panic(pr_fmt("unable to map ramc[%d] cpu registers\n"), idx);

		ramc = of_id->data;
		if (!standby)
			standby = ramc->idle;
		soc_pm.data.memctrl = ramc->memctrl;

		idx++;
	}

	if (!idx)
		panic(pr_fmt("unable to find compatible ram controller node in dtb\n"));

	if (!standby) {
		pr_warn("ramc no standby function available\n");
		return;
	}

	at91_cpuidle_device.dev.platform_data = standby;
}

static void at91rm9200_idle(void)
{
	/*
	 * Disable the processor clock.  The processor will be automatically
	 * re-enabled by an interrupt or by a reset.
	 */
	writel(AT91_PMC_PCK, soc_pm.data.pmc + AT91_PMC_SCDR);
}

static void at91sam9_idle(void)
{
	writel(AT91_PMC_PCK, soc_pm.data.pmc + AT91_PMC_SCDR);
	cpu_do_idle();
}

static void __init at91_pm_sram_init(void)
{
	struct gen_pool *sram_pool;
	phys_addr_t sram_pbase;
	unsigned long sram_base;
	struct device_node *node;
	struct platform_device *pdev = NULL;

	for_each_compatible_node(node, NULL, "mmio-sram") {
		pdev = of_find_device_by_node(node);
		if (pdev) {
			of_node_put(node);
			break;
		}
	}

	if (!pdev) {
		pr_warn("%s: failed to find sram device!\n", __func__);
		return;
	}

	sram_pool = gen_pool_get(&pdev->dev, NULL);
	if (!sram_pool) {
		pr_warn("%s: sram pool unavailable!\n", __func__);
		goto out_put_device;
	}

	sram_base = gen_pool_alloc(sram_pool, at91_pm_suspend_in_sram_sz);
	if (!sram_base) {
		pr_warn("%s: unable to alloc sram!\n", __func__);
		goto out_put_device;
	}

	sram_pbase = gen_pool_virt_to_phys(sram_pool, sram_base);
	at91_suspend_sram_fn = __arm_ioremap_exec(sram_pbase,
					at91_pm_suspend_in_sram_sz, false);
	if (!at91_suspend_sram_fn) {
		pr_warn("SRAM: Could not map\n");
		goto out_put_device;
	}

	/* Copy the pm suspend handler to SRAM */
	at91_suspend_sram_fn = fncpy(at91_suspend_sram_fn,
			&at91_pm_suspend_in_sram, at91_pm_suspend_in_sram_sz);
	return;

out_put_device:
	put_device(&pdev->dev);
	return;
}

static bool __init at91_is_pm_mode_active(int pm_mode)
{
	return (soc_pm.data.standby_mode == pm_mode ||
		soc_pm.data.suspend_mode == pm_mode);
}

static int __init at91_pm_backup_init(void)
{
	struct gen_pool *sram_pool;
	struct device_node *np;
	struct platform_device *pdev = NULL;
	int ret = -ENODEV;

	if (!IS_ENABLED(CONFIG_SOC_SAMA5D2))
		return -EPERM;

	if (!at91_is_pm_mode_active(AT91_PM_BACKUP))
		return 0;

	np = of_find_compatible_node(NULL, NULL, "atmel,sama5d2-sfrbu");
	if (!np) {
		pr_warn("%s: failed to find sfrbu!\n", __func__);
		return ret;
	}

	soc_pm.data.sfrbu = of_iomap(np, 0);
	of_node_put(np);

	np = of_find_compatible_node(NULL, NULL, "atmel,sama5d2-securam");
	if (!np)
		goto securam_fail_no_ref_dev;

	pdev = of_find_device_by_node(np);
	of_node_put(np);
	if (!pdev) {
		pr_warn("%s: failed to find securam device!\n", __func__);
		goto securam_fail_no_ref_dev;
	}

	sram_pool = gen_pool_get(&pdev->dev, NULL);
	if (!sram_pool) {
		pr_warn("%s: securam pool unavailable!\n", __func__);
		goto securam_fail;
	}

	pm_bu = (void *)gen_pool_alloc(sram_pool, sizeof(struct at91_pm_bu));
	if (!pm_bu) {
		pr_warn("%s: unable to alloc securam!\n", __func__);
		ret = -ENOMEM;
		goto securam_fail;
	}

	pm_bu->suspended = 0;
	pm_bu->canary = __pa_symbol(&canary);
	pm_bu->resume = __pa_symbol(cpu_resume);

	return 0;

securam_fail:
	put_device(&pdev->dev);
securam_fail_no_ref_dev:
	iounmap(soc_pm.data.sfrbu);
	soc_pm.data.sfrbu = NULL;
	return ret;
}

static void __init at91_pm_use_default_mode(int pm_mode)
{
	if (pm_mode != AT91_PM_ULP1 && pm_mode != AT91_PM_BACKUP)
		return;

	if (soc_pm.data.standby_mode == pm_mode)
		soc_pm.data.standby_mode = AT91_PM_ULP0;
	if (soc_pm.data.suspend_mode == pm_mode)
		soc_pm.data.suspend_mode = AT91_PM_ULP0;
}

static const struct of_device_id atmel_shdwc_ids[] = {
	{ .compatible = "atmel,sama5d2-shdwc" },
	{ .compatible = "microchip,sam9x60-shdwc" },
	{ /* sentinel. */ }
};

static void __init at91_pm_modes_init(void)
{
	struct device_node *np;
	int ret;

	if (!at91_is_pm_mode_active(AT91_PM_BACKUP) &&
	    !at91_is_pm_mode_active(AT91_PM_ULP1))
		return;

	np = of_find_matching_node(NULL, atmel_shdwc_ids);
	if (!np) {
		pr_warn("%s: failed to find shdwc!\n", __func__);
		goto ulp1_default;
	}

	soc_pm.data.shdwc = of_iomap(np, 0);
	of_node_put(np);

	ret = at91_pm_backup_init();
	if (ret) {
		if (!at91_is_pm_mode_active(AT91_PM_ULP1))
			goto unmap;
		else
			goto backup_default;
	}

	return;

unmap:
	iounmap(soc_pm.data.shdwc);
	soc_pm.data.shdwc = NULL;
ulp1_default:
	at91_pm_use_default_mode(AT91_PM_ULP1);
backup_default:
	at91_pm_use_default_mode(AT91_PM_BACKUP);
}

struct pmc_info {
	unsigned long uhp_udp_mask;
	unsigned long mckr;
	unsigned long version;
};

static const struct pmc_info pmc_infos[] __initconst = {
	{
		.uhp_udp_mask = AT91RM9200_PMC_UHP | AT91RM9200_PMC_UDP,
		.mckr = 0x30,
		.version = AT91_PMC_V1,
	},

	{
		.uhp_udp_mask = AT91SAM926x_PMC_UHP | AT91SAM926x_PMC_UDP,
		.mckr = 0x30,
		.version = AT91_PMC_V1,
	},
	{
		.uhp_udp_mask = AT91SAM926x_PMC_UHP,
		.mckr = 0x30,
		.version = AT91_PMC_V1,
	},
	{	.uhp_udp_mask = 0,
		.mckr = 0x30,
		.version = AT91_PMC_V1,
	},
	{
		.uhp_udp_mask = AT91SAM926x_PMC_UHP | AT91SAM926x_PMC_UDP,
		.mckr = 0x28,
		.version = AT91_PMC_V2,
	},
};

static const struct of_device_id atmel_pmc_ids[] __initconst = {
	{ .compatible = "atmel,at91rm9200-pmc", .data = &pmc_infos[0] },
	{ .compatible = "atmel,at91sam9260-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,at91sam9261-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,at91sam9263-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,at91sam9g45-pmc", .data = &pmc_infos[2] },
	{ .compatible = "atmel,at91sam9n12-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,at91sam9rl-pmc", .data = &pmc_infos[3] },
	{ .compatible = "atmel,at91sam9x5-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,sama5d3-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,sama5d4-pmc", .data = &pmc_infos[1] },
	{ .compatible = "atmel,sama5d2-pmc", .data = &pmc_infos[1] },
	{ .compatible = "microchip,sam9x60-pmc", .data = &pmc_infos[4] },
	{ /* sentinel */ },
};

static void __init at91_pm_modes_validate(const int *modes, int len)
{
	u8 i, standby = 0, suspend = 0;
	int mode;

	for (i = 0; i < len; i++) {
		if (standby && suspend)
			break;

		if (modes[i] == soc_pm.data.standby_mode && !standby) {
			standby = 1;
			continue;
		}

		if (modes[i] == soc_pm.data.suspend_mode && !suspend) {
			suspend = 1;
			continue;
		}
	}

	if (!standby) {
		if (soc_pm.data.suspend_mode == AT91_PM_STANDBY)
			mode = AT91_PM_ULP0;
		else
			mode = AT91_PM_STANDBY;

		pr_warn("AT91: PM: %s mode not supported! Using %s.\n",
			pm_modes[soc_pm.data.standby_mode].pattern,
			pm_modes[mode].pattern);
		soc_pm.data.standby_mode = mode;
	}

	if (!suspend) {
		if (soc_pm.data.standby_mode == AT91_PM_ULP0)
			mode = AT91_PM_STANDBY;
		else
			mode = AT91_PM_ULP0;

		pr_warn("AT91: PM: %s mode not supported! Using %s.\n",
			pm_modes[soc_pm.data.suspend_mode].pattern,
			pm_modes[mode].pattern);
		soc_pm.data.suspend_mode = mode;
	}
}

static void __init at91_pm_init(void (*pm_idle)(void))
{
	struct device_node *pmc_np;
	const struct of_device_id *of_id;
	const struct pmc_info *pmc;

	if (at91_cpuidle_device.dev.platform_data)
		platform_device_register(&at91_cpuidle_device);

	pmc_np = of_find_matching_node_and_match(NULL, atmel_pmc_ids, &of_id);
	soc_pm.data.pmc = of_iomap(pmc_np, 0);
	of_node_put(pmc_np);
	if (!soc_pm.data.pmc) {
		pr_err("AT91: PM not supported, PMC not found\n");
		return;
	}

	pmc = of_id->data;
	soc_pm.data.uhp_udp_mask = pmc->uhp_udp_mask;
	soc_pm.data.pmc_mckr_offset = pmc->mckr;
	soc_pm.data.pmc_version = pmc->version;

	if (pm_idle)
		arm_pm_idle = pm_idle;

	at91_pm_sram_init();

	if (at91_suspend_sram_fn) {
		suspend_set_ops(&at91_pm_ops);
		pr_info("AT91: PM: standby: %s, suspend: %s\n",
			pm_modes[soc_pm.data.standby_mode].pattern,
			pm_modes[soc_pm.data.suspend_mode].pattern);
	} else {
		pr_info("AT91: PM not supported, due to no SRAM allocated\n");
	}
}

void __init at91rm9200_pm_init(void)
{
	if (!IS_ENABLED(CONFIG_SOC_AT91RM9200))
		return;

	/*
	 * Force STANDBY and ULP0 mode to avoid calling
	 * at91_pm_modes_validate() which may increase booting time.
	 * Platform supports anyway only STANDBY and ULP0 modes.
	 */
	soc_pm.data.standby_mode = AT91_PM_STANDBY;
	soc_pm.data.suspend_mode = AT91_PM_ULP0;

	at91_dt_ramc();

	/*
	 * AT91RM9200 SDRAM low-power mode cannot be used with self-refresh.
	 */
	at91_ramc_write(0, AT91_MC_SDRAMC_LPR, 0);

	at91_pm_init(at91rm9200_idle);
}

void __init sam9x60_pm_init(void)
{
	static const int modes[] __initconst = {
		AT91_PM_STANDBY, AT91_PM_ULP0, AT91_PM_ULP0_FAST, AT91_PM_ULP1,
	};

	if (!IS_ENABLED(CONFIG_SOC_SAM9X60))
		return;

	at91_pm_modes_validate(modes, ARRAY_SIZE(modes));
	at91_pm_modes_init();
	at91_dt_ramc();
	at91_pm_init(NULL);

	soc_pm.ws_ids = sam9x60_ws_ids;
	soc_pm.config_pmc_ws = at91_sam9x60_config_pmc_ws;
}

void __init at91sam9_pm_init(void)
{
	if (!IS_ENABLED(CONFIG_SOC_AT91SAM9))
		return;

	/*
	 * Force STANDBY and ULP0 mode to avoid calling
	 * at91_pm_modes_validate() which may increase booting time.
	 * Platform supports anyway only STANDBY and ULP0 modes.
	 */
	soc_pm.data.standby_mode = AT91_PM_STANDBY;
	soc_pm.data.suspend_mode = AT91_PM_ULP0;

	at91_dt_ramc();
	at91_pm_init(at91sam9_idle);
}

void __init sama5_pm_init(void)
{
	static const int modes[] __initconst = {
		AT91_PM_STANDBY, AT91_PM_ULP0, AT91_PM_ULP0_FAST,
	};

	if (!IS_ENABLED(CONFIG_SOC_SAMA5))
		return;

	at91_pm_modes_validate(modes, ARRAY_SIZE(modes));
	at91_dt_ramc();
	at91_pm_init(NULL);
}

void __init sama5d2_pm_init(void)
{
	static const int modes[] __initconst = {
		AT91_PM_STANDBY, AT91_PM_ULP0, AT91_PM_ULP0_FAST, AT91_PM_ULP1,
		AT91_PM_BACKUP,
	};

	if (!IS_ENABLED(CONFIG_SOC_SAMA5D2))
		return;

	at91_pm_modes_validate(modes, ARRAY_SIZE(modes));
	at91_pm_modes_init();
	at91_dt_ramc();
	at91_pm_init(NULL);

	soc_pm.ws_ids = sama5d2_ws_ids;
	soc_pm.config_shdwc_ws = at91_sama5d2_config_shdwc_ws;
	soc_pm.config_pmc_ws = at91_sama5d2_config_pmc_ws;
}

static int __init at91_pm_modes_select(char *str)
{
	char *s;
	substring_t args[MAX_OPT_ARGS];
	int standby, suspend;

	if (!str)
		return 0;

	s = strsep(&str, ",");
	standby = match_token(s, pm_modes, args);
	if (standby < 0)
		return 0;

	suspend = match_token(str, pm_modes, args);
	if (suspend < 0)
		return 0;

	soc_pm.data.standby_mode = standby;
	soc_pm.data.suspend_mode = suspend;

	return 0;
}
early_param("atmel.pm_modes", at91_pm_modes_select);
