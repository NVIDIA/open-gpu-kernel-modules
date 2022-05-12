// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/kernel/cpu/sh4a/clock-sh7722.c
 *
 * SH7722 clock framework support
 *
 * Copyright (C) 2009 Magnus Damm
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/clkdev.h>
#include <linux/sh_clk.h>
#include <asm/clock.h>
#include <cpu/sh7722.h>

/* SH7722 registers */
#define FRQCR		0xa4150000
#define VCLKCR		0xa4150004
#define SCLKACR		0xa4150008
#define SCLKBCR		0xa415000c
#define IRDACLKCR	0xa4150018
#define PLLCR		0xa4150024
#define MSTPCR0		0xa4150030
#define MSTPCR1		0xa4150034
#define MSTPCR2		0xa4150038
#define DLLFRQ		0xa4150050

/* Fixed 32 KHz root clock for RTC and Power Management purposes */
static struct clk r_clk = {
	.rate           = 32768,
};

/*
 * Default rate for the root input clock, reset this with clk_set_rate()
 * from the platform code.
 */
struct clk extal_clk = {
	.rate		= 33333333,
};

/* The dll block multiplies the 32khz r_clk, may be used instead of extal */
static unsigned long dll_recalc(struct clk *clk)
{
	unsigned long mult;

	if (__raw_readl(PLLCR) & 0x1000)
		mult = __raw_readl(DLLFRQ);
	else
		mult = 0;

	return clk->parent->rate * mult;
}

static struct sh_clk_ops dll_clk_ops = {
	.recalc		= dll_recalc,
};

static struct clk dll_clk = {
	.ops		= &dll_clk_ops,
	.parent		= &r_clk,
	.flags		= CLK_ENABLE_ON_INIT,
};

static unsigned long pll_recalc(struct clk *clk)
{
	unsigned long mult = 1;
	unsigned long div = 1;

	if (__raw_readl(PLLCR) & 0x4000)
		mult = (((__raw_readl(FRQCR) >> 24) & 0x1f) + 1);
	else
		div = 2;

	return (clk->parent->rate * mult) / div;
}

static struct sh_clk_ops pll_clk_ops = {
	.recalc		= pll_recalc,
};

static struct clk pll_clk = {
	.ops		= &pll_clk_ops,
	.flags		= CLK_ENABLE_ON_INIT,
};

struct clk *main_clks[] = {
	&r_clk,
	&extal_clk,
	&dll_clk,
	&pll_clk,
};

static int multipliers[] = { 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
static int divisors[] = { 1, 3, 2, 5, 3, 4, 5, 6, 8, 10, 12, 16, 20 };

static struct clk_div_mult_table div4_div_mult_table = {
	.divisors = divisors,
	.nr_divisors = ARRAY_SIZE(divisors),
	.multipliers = multipliers,
	.nr_multipliers = ARRAY_SIZE(multipliers),
};

static struct clk_div4_table div4_table = {
	.div_mult_table = &div4_div_mult_table,
};

#define DIV4(_reg, _bit, _mask, _flags) \
  SH_CLK_DIV4(&pll_clk, _reg, _bit, _mask, _flags)

enum { DIV4_I, DIV4_U, DIV4_SH, DIV4_B, DIV4_B3, DIV4_P, DIV4_NR };

struct clk div4_clks[DIV4_NR] = {
	[DIV4_I] = DIV4(FRQCR, 20, 0x1fef, CLK_ENABLE_ON_INIT),
	[DIV4_U] = DIV4(FRQCR, 16, 0x1fff, CLK_ENABLE_ON_INIT),
	[DIV4_SH] = DIV4(FRQCR, 12, 0x1fff, CLK_ENABLE_ON_INIT),
	[DIV4_B] = DIV4(FRQCR, 8, 0x1fff, CLK_ENABLE_ON_INIT),
	[DIV4_B3] = DIV4(FRQCR, 4, 0x1fff, CLK_ENABLE_ON_INIT),
	[DIV4_P] = DIV4(FRQCR, 0, 0x1fff, 0),
};

enum { DIV4_IRDA, DIV4_ENABLE_NR };

struct clk div4_enable_clks[DIV4_ENABLE_NR] = {
	[DIV4_IRDA] = DIV4(IRDACLKCR, 0, 0x1fff, 0),
};

enum { DIV4_SIUA, DIV4_SIUB, DIV4_REPARENT_NR };

struct clk div4_reparent_clks[DIV4_REPARENT_NR] = {
	[DIV4_SIUA] = DIV4(SCLKACR, 0, 0x1fff, 0),
	[DIV4_SIUB] = DIV4(SCLKBCR, 0, 0x1fff, 0),
};

enum { DIV6_V, DIV6_NR };

struct clk div6_clks[DIV6_NR] = {
	[DIV6_V] = SH_CLK_DIV6(&pll_clk, VCLKCR, 0),
};

static struct clk mstp_clks[HWBLK_NR] = {
	[HWBLK_URAM]  = SH_CLK_MSTP32(&div4_clks[DIV4_U], MSTPCR0, 28, CLK_ENABLE_ON_INIT),
	[HWBLK_XYMEM] = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR0, 26, CLK_ENABLE_ON_INIT),
	[HWBLK_TMU]   = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR0, 15, 0),
	[HWBLK_CMT]   = SH_CLK_MSTP32(&r_clk,		  MSTPCR0, 14, 0),
	[HWBLK_RWDT]  = SH_CLK_MSTP32(&r_clk,		  MSTPCR0, 13, 0),
	[HWBLK_FLCTL] = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR0, 10, 0),
	[HWBLK_SCIF0] = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR0, 7, 0),
	[HWBLK_SCIF1] = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR0, 6, 0),
	[HWBLK_SCIF2] = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR0, 5, 0),

	[HWBLK_IIC]   = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR1, 9, 0),
	[HWBLK_RTC]   = SH_CLK_MSTP32(&r_clk,		  MSTPCR1, 8, 0),

	[HWBLK_SDHI]  = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR2, 18, 0),
	[HWBLK_KEYSC] = SH_CLK_MSTP32(&r_clk,		  MSTPCR2, 14, 0),
	[HWBLK_USBF]  = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR2, 11, 0),
	[HWBLK_2DG]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 9, 0),
	[HWBLK_SIU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 8, 0),
	[HWBLK_JPU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 6, 0),
	[HWBLK_VOU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 5, 0),
	[HWBLK_BEU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 4, 0),
	[HWBLK_CEU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 3, 0),
	[HWBLK_VEU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 2, 0),
	[HWBLK_VPU]   = SH_CLK_MSTP32(&div4_clks[DIV4_B], MSTPCR2, 1, 0),
	[HWBLK_LCDC]  = SH_CLK_MSTP32(&div4_clks[DIV4_P], MSTPCR2, 0, 0),
};

static struct clk_lookup lookups[] = {
	/* main clocks */
	CLKDEV_CON_ID("rclk", &r_clk),
	CLKDEV_CON_ID("extal", &extal_clk),
	CLKDEV_CON_ID("dll_clk", &dll_clk),
	CLKDEV_CON_ID("pll_clk", &pll_clk),

	/* DIV4 clocks */
	CLKDEV_CON_ID("cpu_clk", &div4_clks[DIV4_I]),
	CLKDEV_CON_ID("umem_clk", &div4_clks[DIV4_U]),
	CLKDEV_CON_ID("shyway_clk", &div4_clks[DIV4_SH]),
	CLKDEV_CON_ID("bus_clk", &div4_clks[DIV4_B]),
	CLKDEV_CON_ID("b3_clk", &div4_clks[DIV4_B3]),
	CLKDEV_CON_ID("peripheral_clk", &div4_clks[DIV4_P]),
	CLKDEV_CON_ID("irda_clk", &div4_enable_clks[DIV4_IRDA]),
	CLKDEV_CON_ID("siua_clk", &div4_reparent_clks[DIV4_SIUA]),
	CLKDEV_CON_ID("siub_clk", &div4_reparent_clks[DIV4_SIUB]),

	/* DIV6 clocks */
	CLKDEV_CON_ID("video_clk", &div6_clks[DIV6_V]),

	/* MSTP clocks */
	CLKDEV_CON_ID("uram0", &mstp_clks[HWBLK_URAM]),
	CLKDEV_CON_ID("xymem0", &mstp_clks[HWBLK_XYMEM]),

	CLKDEV_ICK_ID("fck", "sh-tmu.0", &mstp_clks[HWBLK_TMU]),

	CLKDEV_ICK_ID("fck", "sh-cmt-32.0", &mstp_clks[HWBLK_CMT]),
	CLKDEV_DEV_ID("sh-wdt.0", &mstp_clks[HWBLK_RWDT]),
	CLKDEV_CON_ID("flctl0", &mstp_clks[HWBLK_FLCTL]),

	CLKDEV_DEV_ID("sh-sci.0", &mstp_clks[HWBLK_SCIF0]),
	CLKDEV_DEV_ID("sh-sci.1", &mstp_clks[HWBLK_SCIF1]),
	CLKDEV_DEV_ID("sh-sci.2", &mstp_clks[HWBLK_SCIF2]),

	CLKDEV_DEV_ID("i2c-sh_mobile.0", &mstp_clks[HWBLK_IIC]),
	CLKDEV_CON_ID("rtc0", &mstp_clks[HWBLK_RTC]),
	CLKDEV_DEV_ID("sh_mobile_sdhi.0", &mstp_clks[HWBLK_SDHI]),
	CLKDEV_DEV_ID("sh_keysc.0", &mstp_clks[HWBLK_KEYSC]),
	CLKDEV_CON_ID("usbf0", &mstp_clks[HWBLK_USBF]),
	CLKDEV_CON_ID("2dg0", &mstp_clks[HWBLK_2DG]),
	CLKDEV_DEV_ID("siu-pcm-audio", &mstp_clks[HWBLK_SIU]),
	CLKDEV_DEV_ID("sh-vou.0", &mstp_clks[HWBLK_VOU]),
	CLKDEV_CON_ID("jpu0", &mstp_clks[HWBLK_JPU]),
	CLKDEV_CON_ID("beu0", &mstp_clks[HWBLK_BEU]),
	CLKDEV_DEV_ID("renesas-ceu.0", &mstp_clks[HWBLK_CEU]),
	CLKDEV_CON_ID("veu0", &mstp_clks[HWBLK_VEU]),
	CLKDEV_CON_ID("vpu0", &mstp_clks[HWBLK_VPU]),
	CLKDEV_DEV_ID("sh_mobile_lcdc_fb.0", &mstp_clks[HWBLK_LCDC]),
};

int __init arch_clk_init(void)
{
	int k, ret = 0;

	/* autodetect extal or dll configuration */
	if (__raw_readl(PLLCR) & 0x1000)
		pll_clk.parent = &dll_clk;
	else
		pll_clk.parent = &extal_clk;

	for (k = 0; !ret && (k < ARRAY_SIZE(main_clks)); k++)
		ret = clk_register(main_clks[k]);

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	if (!ret)
		ret = sh_clk_div4_register(div4_clks, DIV4_NR, &div4_table);

	if (!ret)
		ret = sh_clk_div4_enable_register(div4_enable_clks,
					DIV4_ENABLE_NR, &div4_table);

	if (!ret)
		ret = sh_clk_div4_reparent_register(div4_reparent_clks,
					DIV4_REPARENT_NR, &div4_table);

	if (!ret)
		ret = sh_clk_div6_register(div6_clks, DIV6_NR);

	if (!ret)
		ret = sh_clk_mstp_register(mstp_clks, HWBLK_NR);

	return ret;
}
