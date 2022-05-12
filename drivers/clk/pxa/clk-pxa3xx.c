// SPDX-License-Identifier: GPL-2.0-only
/*
 * Marvell PXA3xxx family clocks
 *
 * Copyright (C) 2014 Robert Jarzmik
 *
 * Heavily inspired from former arch/arm/mach-pxa/pxa3xx.c
 *
 * For non-devicetree platforms. Once pxa is fully converted to devicetree, this
 * should go away.
 */
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/of.h>
#include <mach/smemc.h>
#include <mach/pxa3xx-regs.h>

#include <dt-bindings/clock/pxa-clock.h>
#include "clk-pxa.h"

#define KHz 1000
#define MHz (1000 * 1000)

enum {
	PXA_CORE_60Mhz = 0,
	PXA_CORE_RUN,
	PXA_CORE_TURBO,
};

enum {
	PXA_BUS_60Mhz = 0,
	PXA_BUS_HSS,
};

/* crystal frequency to HSIO bus frequency multiplier (HSS) */
static unsigned char hss_mult[4] = { 8, 12, 16, 24 };

/* crystal frequency to static memory controller multiplier (SMCFS) */
static unsigned int smcfs_mult[8] = { 6, 0, 8, 0, 0, 16, };
static unsigned int df_clkdiv[4] = { 1, 2, 4, 1 };

static const char * const get_freq_khz[] = {
	"core", "ring_osc_60mhz", "run", "cpll", "system_bus"
};

/*
 * Get the clock frequency as reflected by ACSR and the turbo flag.
 * We assume these values have been applied via a fcs.
 * If info is not 0 we also display the current settings.
 */
unsigned int pxa3xx_get_clk_frequency_khz(int info)
{
	struct clk *clk;
	unsigned long clks[5];
	int i;

	for (i = 0; i < 5; i++) {
		clk = clk_get(NULL, get_freq_khz[i]);
		if (IS_ERR(clk)) {
			clks[i] = 0;
		} else {
			clks[i] = clk_get_rate(clk);
			clk_put(clk);
		}
	}
	if (info) {
		pr_info("RO Mode clock: %ld.%02ldMHz\n",
			clks[1] / 1000000, (clks[0] % 1000000) / 10000);
		pr_info("Run Mode clock: %ld.%02ldMHz\n",
			clks[2] / 1000000, (clks[1] % 1000000) / 10000);
		pr_info("Turbo Mode clock: %ld.%02ldMHz\n",
			clks[3] / 1000000, (clks[2] % 1000000) / 10000);
		pr_info("System bus clock: %ld.%02ldMHz\n",
			clks[4] / 1000000, (clks[4] % 1000000) / 10000);
	}
	return (unsigned int)clks[0] / KHz;
}

static unsigned long clk_pxa3xx_ac97_get_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	unsigned long ac97_div, rate;

	ac97_div = AC97_DIV;

	/* This may loose precision for some rates but won't for the
	 * standard 24.576MHz.
	 */
	rate = parent_rate / 2;
	rate /= ((ac97_div >> 12) & 0x7fff);
	rate *= (ac97_div & 0xfff);

	return rate;
}
PARENTS(clk_pxa3xx_ac97) = { "spll_624mhz" };
RATE_RO_OPS(clk_pxa3xx_ac97, "ac97");

static unsigned long clk_pxa3xx_smemc_get_rate(struct clk_hw *hw,
					      unsigned long parent_rate)
{
	unsigned long acsr = ACSR;
	unsigned long memclkcfg = __raw_readl(MEMCLKCFG);

	return (parent_rate / 48)  * smcfs_mult[(acsr >> 23) & 0x7] /
		df_clkdiv[(memclkcfg >> 16) & 0x3];
}
PARENTS(clk_pxa3xx_smemc) = { "spll_624mhz" };
RATE_RO_OPS(clk_pxa3xx_smemc, "smemc");

static bool pxa3xx_is_ring_osc_forced(void)
{
	unsigned long acsr = ACSR;

	return acsr & ACCR_D0CS;
}

PARENTS(pxa3xx_pbus) = { "ring_osc_60mhz", "spll_624mhz" };
PARENTS(pxa3xx_32Khz_bus) = { "osc_32_768khz", "osc_32_768khz" };
PARENTS(pxa3xx_13MHz_bus) = { "osc_13mhz", "osc_13mhz" };
PARENTS(pxa3xx_ac97_bus) = { "ring_osc_60mhz", "ac97" };
PARENTS(pxa3xx_sbus) = { "ring_osc_60mhz", "system_bus" };
PARENTS(pxa3xx_smemcbus) = { "ring_osc_60mhz", "smemc" };

#define CKEN_AB(bit) ((CKEN_ ## bit > 31) ? &CKENB : &CKENA)
#define PXA3XX_CKEN(dev_id, con_id, parents, mult_lp, div_lp, mult_hp,	\
		    div_hp, bit, is_lp, flags)				\
	PXA_CKEN(dev_id, con_id, bit, parents, mult_lp, div_lp,		\
		 mult_hp, div_hp, is_lp,  CKEN_AB(bit),			\
		 (CKEN_ ## bit % 32), flags)
#define PXA3XX_PBUS_CKEN(dev_id, con_id, bit, mult_lp, div_lp,		\
			 mult_hp, div_hp, delay)			\
	PXA3XX_CKEN(dev_id, con_id, pxa3xx_pbus_parents, mult_lp,	\
		    div_lp, mult_hp, div_hp, bit, pxa3xx_is_ring_osc_forced, 0)
#define PXA3XX_CKEN_1RATE(dev_id, con_id, bit, parents)			\
	PXA_CKEN_1RATE(dev_id, con_id, bit, parents,			\
		       CKEN_AB(bit), (CKEN_ ## bit % 32), 0)

static struct desc_clk_cken pxa3xx_clocks[] __initdata = {
	PXA3XX_PBUS_CKEN("pxa2xx-uart.0", NULL, FFUART, 1, 4, 1, 42, 1),
	PXA3XX_PBUS_CKEN("pxa2xx-uart.1", NULL, BTUART, 1, 4, 1, 42, 1),
	PXA3XX_PBUS_CKEN("pxa2xx-uart.2", NULL, STUART, 1, 4, 1, 42, 1),
	PXA3XX_PBUS_CKEN("pxa2xx-i2c.0", NULL, I2C, 2, 5, 1, 19, 0),
	PXA3XX_PBUS_CKEN("pxa27x-udc", NULL, UDC, 1, 4, 1, 13, 5),
	PXA3XX_PBUS_CKEN("pxa27x-ohci", NULL, USBH, 1, 4, 1, 13, 0),
	PXA3XX_PBUS_CKEN("pxa3xx-u2d", NULL, USB2, 1, 4, 1, 13, 0),
	PXA3XX_PBUS_CKEN("pxa27x-pwm.0", NULL, PWM0, 1, 6, 1, 48, 0),
	PXA3XX_PBUS_CKEN("pxa27x-pwm.1", NULL, PWM1, 1, 6, 1, 48, 0),
	PXA3XX_PBUS_CKEN("pxa2xx-mci.0", NULL, MMC1, 1, 4, 1, 24, 0),
	PXA3XX_PBUS_CKEN("pxa2xx-mci.1", NULL, MMC2, 1, 4, 1, 24, 0),
	PXA3XX_PBUS_CKEN("pxa2xx-mci.2", NULL, MMC3, 1, 4, 1, 24, 0),

	PXA3XX_CKEN_1RATE("pxa27x-keypad", NULL, KEYPAD,
			  pxa3xx_32Khz_bus_parents),
	PXA3XX_CKEN_1RATE("pxa3xx-ssp.0", NULL, SSP1, pxa3xx_13MHz_bus_parents),
	PXA3XX_CKEN_1RATE("pxa3xx-ssp.1", NULL, SSP2, pxa3xx_13MHz_bus_parents),
	PXA3XX_CKEN_1RATE("pxa3xx-ssp.2", NULL, SSP3, pxa3xx_13MHz_bus_parents),
	PXA3XX_CKEN_1RATE("pxa3xx-ssp.3", NULL, SSP4, pxa3xx_13MHz_bus_parents),

	PXA3XX_CKEN(NULL, "AC97CLK", pxa3xx_ac97_bus_parents, 1, 4, 1, 1, AC97,
		    pxa3xx_is_ring_osc_forced, 0),
	PXA3XX_CKEN(NULL, "CAMCLK", pxa3xx_sbus_parents, 1, 2, 1, 1, CAMERA,
		    pxa3xx_is_ring_osc_forced, 0),
	PXA3XX_CKEN("pxa2xx-fb", NULL, pxa3xx_sbus_parents, 1, 1, 1, 1, LCD,
		    pxa3xx_is_ring_osc_forced, 0),
	PXA3XX_CKEN("pxa2xx-pcmcia", NULL, pxa3xx_smemcbus_parents, 1, 4,
		    1, 1, SMC, pxa3xx_is_ring_osc_forced, CLK_IGNORE_UNUSED),
};

static struct desc_clk_cken pxa300_310_clocks[] __initdata = {

	PXA3XX_PBUS_CKEN("pxa3xx-gcu", NULL, PXA300_GCU, 1, 1, 1, 1, 0),
	PXA3XX_PBUS_CKEN("pxa3xx-nand", NULL, NAND, 1, 2, 1, 4, 0),
	PXA3XX_CKEN_1RATE("pxa3xx-gpio", NULL, GPIO, pxa3xx_13MHz_bus_parents),
};

static struct desc_clk_cken pxa320_clocks[] __initdata = {
	PXA3XX_PBUS_CKEN("pxa3xx-nand", NULL, NAND, 1, 2, 1, 6, 0),
	PXA3XX_PBUS_CKEN("pxa3xx-gcu", NULL, PXA320_GCU, 1, 1, 1, 1, 0),
	PXA3XX_CKEN_1RATE("pxa3xx-gpio", NULL, GPIO, pxa3xx_13MHz_bus_parents),
};

static struct desc_clk_cken pxa93x_clocks[] __initdata = {

	PXA3XX_PBUS_CKEN("pxa3xx-gcu", NULL, PXA300_GCU, 1, 1, 1, 1, 0),
	PXA3XX_PBUS_CKEN("pxa3xx-nand", NULL, NAND, 1, 2, 1, 4, 0),
	PXA3XX_CKEN_1RATE("pxa93x-gpio", NULL, GPIO, pxa3xx_13MHz_bus_parents),
};

static unsigned long clk_pxa3xx_system_bus_get_rate(struct clk_hw *hw,
					    unsigned long parent_rate)
{
	unsigned long acsr = ACSR;
	unsigned int hss = (acsr >> 14) & 0x3;

	if (pxa3xx_is_ring_osc_forced())
		return parent_rate;
	return parent_rate / 48 * hss_mult[hss];
}

static u8 clk_pxa3xx_system_bus_get_parent(struct clk_hw *hw)
{
	if (pxa3xx_is_ring_osc_forced())
		return PXA_BUS_60Mhz;
	else
		return PXA_BUS_HSS;
}

PARENTS(clk_pxa3xx_system_bus) = { "ring_osc_60mhz", "spll_624mhz" };
MUX_RO_RATE_RO_OPS(clk_pxa3xx_system_bus, "system_bus");

static unsigned long clk_pxa3xx_core_get_rate(struct clk_hw *hw,
					      unsigned long parent_rate)
{
	return parent_rate;
}

static u8 clk_pxa3xx_core_get_parent(struct clk_hw *hw)
{
	unsigned long xclkcfg;
	unsigned int t;

	if (pxa3xx_is_ring_osc_forced())
		return PXA_CORE_60Mhz;

	/* Read XCLKCFG register turbo bit */
	__asm__ __volatile__("mrc\tp14, 0, %0, c6, c0, 0" : "=r"(xclkcfg));
	t = xclkcfg & 0x1;

	if (t)
		return PXA_CORE_TURBO;
	return PXA_CORE_RUN;
}
PARENTS(clk_pxa3xx_core) = { "ring_osc_60mhz", "run", "cpll" };
MUX_RO_RATE_RO_OPS(clk_pxa3xx_core, "core");

static unsigned long clk_pxa3xx_run_get_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	unsigned long acsr = ACSR;
	unsigned int xn = (acsr & ACCR_XN_MASK) >> 8;
	unsigned int t, xclkcfg;

	/* Read XCLKCFG register turbo bit */
	__asm__ __volatile__("mrc\tp14, 0, %0, c6, c0, 0" : "=r"(xclkcfg));
	t = xclkcfg & 0x1;

	return t ? (parent_rate / xn) * 2 : parent_rate;
}
PARENTS(clk_pxa3xx_run) = { "cpll" };
RATE_RO_OPS(clk_pxa3xx_run, "run");

static unsigned long clk_pxa3xx_cpll_get_rate(struct clk_hw *hw,
	unsigned long parent_rate)
{
	unsigned long acsr = ACSR;
	unsigned int xn = (acsr & ACCR_XN_MASK) >> 8;
	unsigned int xl = acsr & ACCR_XL_MASK;
	unsigned int t, xclkcfg;

	/* Read XCLKCFG register turbo bit */
	__asm__ __volatile__("mrc\tp14, 0, %0, c6, c0, 0" : "=r"(xclkcfg));
	t = xclkcfg & 0x1;

	pr_info("RJK: parent_rate=%lu, xl=%u, xn=%u\n", parent_rate, xl, xn);
	return t ? parent_rate * xl * xn : parent_rate * xl;
}
PARENTS(clk_pxa3xx_cpll) = { "osc_13mhz" };
RATE_RO_OPS(clk_pxa3xx_cpll, "cpll");

static void __init pxa3xx_register_core(void)
{
	clk_register_clk_pxa3xx_cpll();
	clk_register_clk_pxa3xx_run();

	clkdev_pxa_register(CLK_CORE, "core", NULL,
			    clk_register_clk_pxa3xx_core());
}

static void __init pxa3xx_register_plls(void)
{
	clk_register_fixed_rate(NULL, "osc_13mhz", NULL,
				CLK_GET_RATE_NOCACHE,
				13 * MHz);
	clkdev_pxa_register(CLK_OSC32k768, "osc_32_768khz", NULL,
			    clk_register_fixed_rate(NULL, "osc_32_768khz", NULL,
						    CLK_GET_RATE_NOCACHE,
						    32768));
	clk_register_fixed_rate(NULL, "ring_osc_120mhz", NULL,
				CLK_GET_RATE_NOCACHE,
				120 * MHz);
	clk_register_fixed_rate(NULL, "clk_dummy", NULL, 0, 0);
	clk_register_fixed_factor(NULL, "spll_624mhz", "osc_13mhz", 0, 48, 1);
	clk_register_fixed_factor(NULL, "ring_osc_60mhz", "ring_osc_120mhz",
				  0, 1, 2);
}

#define DUMMY_CLK(_con_id, _dev_id, _parent) \
	{ .con_id = _con_id, .dev_id = _dev_id, .parent = _parent }
struct dummy_clk {
	const char *con_id;
	const char *dev_id;
	const char *parent;
};
static struct dummy_clk dummy_clks[] __initdata = {
	DUMMY_CLK(NULL, "pxa93x-gpio", "osc_13mhz"),
	DUMMY_CLK(NULL, "sa1100-rtc", "osc_32_768khz"),
	DUMMY_CLK("UARTCLK", "pxa2xx-ir", "STUART"),
	DUMMY_CLK(NULL, "pxa3xx-pwri2c.1", "osc_13mhz"),
};

static void __init pxa3xx_dummy_clocks_init(void)
{
	struct clk *clk;
	struct dummy_clk *d;
	const char *name;
	int i;

	for (i = 0; i < ARRAY_SIZE(dummy_clks); i++) {
		d = &dummy_clks[i];
		name = d->dev_id ? d->dev_id : d->con_id;
		clk = clk_register_fixed_factor(NULL, name, d->parent, 0, 1, 1);
		clk_register_clkdev(clk, d->con_id, d->dev_id);
	}
}

static void __init pxa3xx_base_clocks_init(void)
{
	struct clk *clk;

	pxa3xx_register_plls();
	pxa3xx_register_core();
	clk_register_clk_pxa3xx_system_bus();
	clk_register_clk_pxa3xx_ac97();
	clk_register_clk_pxa3xx_smemc();
	clk = clk_register_gate(NULL, "CLK_POUT",
				"osc_13mhz", 0, OSCC, 11, 0, NULL);
	clk_register_clkdev(clk, "CLK_POUT", NULL);
	clkdev_pxa_register(CLK_OSTIMER, "OSTIMER0", NULL,
			    clk_register_fixed_factor(NULL, "os-timer0",
						      "osc_13mhz", 0, 1, 4));
}

int __init pxa3xx_clocks_init(void)
{
	int ret;

	pxa3xx_base_clocks_init();
	pxa3xx_dummy_clocks_init();
	ret = clk_pxa_cken_init(pxa3xx_clocks, ARRAY_SIZE(pxa3xx_clocks));
	if (ret)
		return ret;
	if (cpu_is_pxa320())
		return clk_pxa_cken_init(pxa320_clocks,
					 ARRAY_SIZE(pxa320_clocks));
	if (cpu_is_pxa300() || cpu_is_pxa310())
		return clk_pxa_cken_init(pxa300_310_clocks,
					 ARRAY_SIZE(pxa300_310_clocks));
	return clk_pxa_cken_init(pxa93x_clocks, ARRAY_SIZE(pxa93x_clocks));
}

static void __init pxa3xx_dt_clocks_init(struct device_node *np)
{
	pxa3xx_clocks_init();
	clk_pxa_dt_common_init(np);
}
CLK_OF_DECLARE(pxa_clks, "marvell,pxa300-clocks", pxa3xx_dt_clocks_init);
