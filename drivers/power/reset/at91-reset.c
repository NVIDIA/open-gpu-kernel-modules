/*
 * Atmel AT91 SAM9 & SAMA5 SoCs reset code
 *
 * Copyright (C) 2007 Atmel Corporation.
 * Copyright (C) BitBox Ltd 2010
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcosoft.com>
 * Copyright (C) 2014 Free Electrons
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>

#include <soc/at91/at91sam9_ddrsdr.h>
#include <soc/at91/at91sam9_sdramc.h>

#define AT91_RSTC_CR	0x00		/* Reset Controller Control Register */
#define AT91_RSTC_PROCRST	BIT(0)		/* Processor Reset */
#define AT91_RSTC_PERRST	BIT(2)		/* Peripheral Reset */
#define AT91_RSTC_EXTRST	BIT(3)		/* External Reset */
#define AT91_RSTC_KEY		(0xa5 << 24)	/* KEY Password */

#define AT91_RSTC_SR	0x04		/* Reset Controller Status Register */
#define AT91_RSTC_URSTS		BIT(0)		/* User Reset Status */
#define AT91_RSTC_RSTTYP	GENMASK(10, 8)	/* Reset Type */
#define AT91_RSTC_NRSTL		BIT(16)		/* NRST Pin Level */
#define AT91_RSTC_SRCMP		BIT(17)		/* Software Reset Command in Progress */

#define AT91_RSTC_MR	0x08		/* Reset Controller Mode Register */
#define AT91_RSTC_URSTEN	BIT(0)		/* User Reset Enable */
#define AT91_RSTC_URSTASYNC	BIT(2)		/* User Reset Asynchronous Control */
#define AT91_RSTC_URSTIEN	BIT(4)		/* User Reset Interrupt Enable */
#define AT91_RSTC_ERSTL		GENMASK(11, 8)	/* External Reset Length */

enum reset_type {
	RESET_TYPE_GENERAL	= 0,
	RESET_TYPE_WAKEUP	= 1,
	RESET_TYPE_WATCHDOG	= 2,
	RESET_TYPE_SOFTWARE	= 3,
	RESET_TYPE_USER		= 4,
	RESET_TYPE_CPU_FAIL	= 6,
	RESET_TYPE_XTAL_FAIL	= 7,
	RESET_TYPE_ULP2		= 8,
};

struct at91_reset {
	void __iomem *rstc_base;
	void __iomem *ramc_base[2];
	struct clk *sclk;
	struct notifier_block nb;
	u32 args;
	u32 ramc_lpr;
};

/*
* unless the SDRAM is cleanly shutdown before we hit the
* reset register it can be left driving the data bus and
* killing the chance of a subsequent boot from NAND
*/
static int at91_reset(struct notifier_block *this, unsigned long mode,
		      void *cmd)
{
	struct at91_reset *reset = container_of(this, struct at91_reset, nb);

	asm volatile(
		/* Align to cache lines */
		".balign 32\n\t"

		/* Disable SDRAM0 accesses */
		"	tst	%0, #0\n\t"
		"	beq	1f\n\t"
		"	str	%3, [%0, #" __stringify(AT91_DDRSDRC_RTR) "]\n\t"
		/* Power down SDRAM0 */
		"	str	%4, [%0, %6]\n\t"
		/* Disable SDRAM1 accesses */
		"1:	tst	%1, #0\n\t"
		"	beq	2f\n\t"
		"	strne	%3, [%1, #" __stringify(AT91_DDRSDRC_RTR) "]\n\t"
		/* Power down SDRAM1 */
		"	strne	%4, [%1, %6]\n\t"
		/* Reset CPU */
		"2:	str	%5, [%2, #" __stringify(AT91_RSTC_CR) "]\n\t"

		"	b	.\n\t"
		:
		: "r" (reset->ramc_base[0]),
		  "r" (reset->ramc_base[1]),
		  "r" (reset->rstc_base),
		  "r" (1),
		  "r" cpu_to_le32(AT91_DDRSDRC_LPCB_POWER_DOWN),
		  "r" (reset->args),
		  "r" (reset->ramc_lpr)
		: "r4");

	return NOTIFY_DONE;
}

static void __init at91_reset_status(struct platform_device *pdev,
				     void __iomem *base)
{
	const char *reason;
	u32 reg = readl(base + AT91_RSTC_SR);

	switch ((reg & AT91_RSTC_RSTTYP) >> 8) {
	case RESET_TYPE_GENERAL:
		reason = "general reset";
		break;
	case RESET_TYPE_WAKEUP:
		reason = "wakeup";
		break;
	case RESET_TYPE_WATCHDOG:
		reason = "watchdog reset";
		break;
	case RESET_TYPE_SOFTWARE:
		reason = "software reset";
		break;
	case RESET_TYPE_USER:
		reason = "user reset";
		break;
	case RESET_TYPE_CPU_FAIL:
		reason = "CPU clock failure detection";
		break;
	case RESET_TYPE_XTAL_FAIL:
		reason = "32.768 kHz crystal failure detection";
		break;
	case RESET_TYPE_ULP2:
		reason = "ULP2 reset";
		break;
	default:
		reason = "unknown reset";
		break;
	}

	dev_info(&pdev->dev, "Starting after %s\n", reason);
}

static const struct of_device_id at91_ramc_of_match[] = {
	{
		.compatible = "atmel,at91sam9260-sdramc",
		.data = (void *)AT91_SDRAMC_LPR,
	},
	{
		.compatible = "atmel,at91sam9g45-ddramc",
		.data = (void *)AT91_DDRSDRC_LPR,
	},
	{ /* sentinel */ }
};

static const struct of_device_id at91_reset_of_match[] = {
	{
		.compatible = "atmel,at91sam9260-rstc",
		.data = (void *)(AT91_RSTC_KEY | AT91_RSTC_PERRST |
				 AT91_RSTC_PROCRST),
	},
	{
		.compatible = "atmel,at91sam9g45-rstc",
		.data = (void *)(AT91_RSTC_KEY | AT91_RSTC_PERRST |
				 AT91_RSTC_PROCRST)
	},
	{
		.compatible = "atmel,sama5d3-rstc",
		.data = (void *)(AT91_RSTC_KEY | AT91_RSTC_PERRST |
				 AT91_RSTC_PROCRST)
	},
	{
		.compatible = "atmel,samx7-rstc",
		.data = (void *)(AT91_RSTC_KEY | AT91_RSTC_PROCRST)
	},
	{
		.compatible = "microchip,sam9x60-rstc",
		.data = (void *)(AT91_RSTC_KEY | AT91_RSTC_PROCRST)
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, at91_reset_of_match);

static int __init at91_reset_probe(struct platform_device *pdev)
{
	const struct of_device_id *match;
	struct at91_reset *reset;
	struct device_node *np;
	int ret, idx = 0;

	reset = devm_kzalloc(&pdev->dev, sizeof(*reset), GFP_KERNEL);
	if (!reset)
		return -ENOMEM;

	reset->rstc_base = devm_of_iomap(&pdev->dev, pdev->dev.of_node, 0, NULL);
	if (!reset->rstc_base) {
		dev_err(&pdev->dev, "Could not map reset controller address\n");
		return -ENODEV;
	}

	if (!of_device_is_compatible(pdev->dev.of_node, "atmel,sama5d3-rstc")) {
		/* we need to shutdown the ddr controller, so get ramc base */
		for_each_matching_node_and_match(np, at91_ramc_of_match, &match) {
			reset->ramc_lpr = (u32)match->data;
			reset->ramc_base[idx] = devm_of_iomap(&pdev->dev, np, 0, NULL);
			if (!reset->ramc_base[idx]) {
				dev_err(&pdev->dev, "Could not map ram controller address\n");
				of_node_put(np);
				return -ENODEV;
			}
			idx++;
		}
	}

	match = of_match_node(at91_reset_of_match, pdev->dev.of_node);
	reset->nb.notifier_call = at91_reset;
	reset->nb.priority = 192;
	reset->args = (u32)match->data;

	reset->sclk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(reset->sclk))
		return PTR_ERR(reset->sclk);

	ret = clk_prepare_enable(reset->sclk);
	if (ret) {
		dev_err(&pdev->dev, "Could not enable slow clock\n");
		return ret;
	}

	platform_set_drvdata(pdev, reset);

	if (of_device_is_compatible(pdev->dev.of_node, "microchip,sam9x60-rstc")) {
		u32 val = readl(reset->rstc_base + AT91_RSTC_MR);

		writel(AT91_RSTC_KEY | AT91_RSTC_URSTASYNC | val,
		       reset->rstc_base + AT91_RSTC_MR);
	}

	ret = register_restart_handler(&reset->nb);
	if (ret) {
		clk_disable_unprepare(reset->sclk);
		return ret;
	}

	at91_reset_status(pdev, reset->rstc_base);

	return 0;
}

static int __exit at91_reset_remove(struct platform_device *pdev)
{
	struct at91_reset *reset = platform_get_drvdata(pdev);

	unregister_restart_handler(&reset->nb);
	clk_disable_unprepare(reset->sclk);

	return 0;
}

static struct platform_driver at91_reset_driver = {
	.remove = __exit_p(at91_reset_remove),
	.driver = {
		.name = "at91-reset",
		.of_match_table = at91_reset_of_match,
	},
};
module_platform_driver_probe(at91_reset_driver, at91_reset_probe);

MODULE_AUTHOR("Atmel Corporation");
MODULE_DESCRIPTION("Reset driver for Atmel SoCs");
MODULE_LICENSE("GPL v2");
