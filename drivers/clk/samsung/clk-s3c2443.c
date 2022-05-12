// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013 Heiko Stuebner <heiko@sntech.de>
 *
 * Common Clock Framework support for S3C2443 and following SoCs.
 */

#include <linux/clk-provider.h>
#include <linux/clk/samsung.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/reboot.h>

#include <dt-bindings/clock/s3c2443.h>

#include "clk.h"
#include "clk-pll.h"

/* S3C2416 clock controller register offsets */
#define LOCKCON0	0x00
#define LOCKCON1	0x04
#define MPLLCON		0x10
#define EPLLCON		0x18
#define EPLLCON_K	0x1C
#define CLKSRC		0x20
#define CLKDIV0		0x24
#define CLKDIV1		0x28
#define CLKDIV2		0x2C
#define HCLKCON		0x30
#define PCLKCON		0x34
#define SCLKCON		0x38
#define SWRST		0x44

/* the soc types */
enum supported_socs {
	S3C2416,
	S3C2443,
	S3C2450,
};

static void __iomem *reg_base;

/*
 * list of controller registers to be saved and restored during a
 * suspend/resume cycle.
 */
static unsigned long s3c2443_clk_regs[] __initdata = {
	LOCKCON0,
	LOCKCON1,
	MPLLCON,
	EPLLCON,
	EPLLCON_K,
	CLKSRC,
	CLKDIV0,
	CLKDIV1,
	CLKDIV2,
	PCLKCON,
	HCLKCON,
	SCLKCON,
};

PNAME(epllref_p) = { "mpllref", "mpllref", "xti", "ext" };
PNAME(esysclk_p) = { "epllref", "epll" };
PNAME(mpllref_p) = { "xti", "mdivclk" };
PNAME(msysclk_p) = { "mpllref", "mpll" };
PNAME(armclk_p) = { "armdiv" , "hclk" };
PNAME(i2s0_p) = { "div_i2s0", "ext_i2s", "epllref", "epllref" };

static struct samsung_mux_clock s3c2443_common_muxes[] __initdata = {
	MUX(0, "epllref", epllref_p, CLKSRC, 7, 2),
	MUX(ESYSCLK, "esysclk", esysclk_p, CLKSRC, 6, 1),
	MUX(0, "mpllref", mpllref_p, CLKSRC, 3, 1),
	MUX(MSYSCLK, "msysclk", msysclk_p, CLKSRC, 4, 1),
	MUX(ARMCLK, "armclk", armclk_p, CLKDIV0, 13, 1),
	MUX(0, "mux_i2s0", i2s0_p, CLKSRC, 14, 2),
};

static struct clk_div_table hclk_d[] = {
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 2 },
	{ .val = 3, .div = 4 },
	{ /* sentinel */ },
};

static struct clk_div_table mdivclk_d[] = {
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 3 },
	{ .val = 2, .div = 5 },
	{ .val = 3, .div = 7 },
	{ .val = 4, .div = 9 },
	{ .val = 5, .div = 11 },
	{ .val = 6, .div = 13 },
	{ .val = 7, .div = 15 },
	{ /* sentinel */ },
};

static struct samsung_div_clock s3c2443_common_dividers[] __initdata = {
	DIV_T(0, "mdivclk", "xti", CLKDIV0, 6, 3, mdivclk_d),
	DIV(0, "prediv", "msysclk", CLKDIV0, 4, 2),
	DIV_T(HCLK, "hclk", "prediv", CLKDIV0, 0, 2, hclk_d),
	DIV(PCLK, "pclk", "hclk", CLKDIV0, 2, 1),
	DIV(0, "div_hsspi0_epll", "esysclk", CLKDIV1, 24, 2),
	DIV(0, "div_fimd", "esysclk", CLKDIV1, 16, 8),
	DIV(0, "div_i2s0", "esysclk", CLKDIV1, 12, 4),
	DIV(0, "div_uart", "esysclk", CLKDIV1, 8, 4),
	DIV(0, "div_hsmmc1", "esysclk", CLKDIV1, 6, 2),
	DIV(0, "div_usbhost", "esysclk", CLKDIV1, 4, 2),
};

static struct samsung_gate_clock s3c2443_common_gates[] __initdata = {
	GATE(SCLK_HSMMC_EXT, "sclk_hsmmcext", "ext", SCLKCON, 13, 0, 0),
	GATE(SCLK_HSMMC1, "sclk_hsmmc1", "div_hsmmc1", SCLKCON, 12, 0, 0),
	GATE(SCLK_FIMD, "sclk_fimd", "div_fimd", SCLKCON, 10, 0, 0),
	GATE(SCLK_I2S0, "sclk_i2s0", "mux_i2s0", SCLKCON, 9, 0, 0),
	GATE(SCLK_UART, "sclk_uart", "div_uart", SCLKCON, 8, 0, 0),
	GATE(SCLK_USBH, "sclk_usbhost", "div_usbhost", SCLKCON, 1, 0, 0),
	GATE(HCLK_DRAM, "dram", "hclk", HCLKCON, 19, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_SSMC, "ssmc", "hclk", HCLKCON, 18, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_HSMMC1, "hsmmc1", "hclk", HCLKCON, 16, 0, 0),
	GATE(HCLK_USBD, "usb-device", "hclk", HCLKCON, 12, 0, 0),
	GATE(HCLK_USBH, "usb-host", "hclk", HCLKCON, 11, 0, 0),
	GATE(HCLK_LCD, "lcd", "hclk", HCLKCON, 9, 0, 0),
	GATE(HCLK_DMA5, "dma5", "hclk", HCLKCON, 5, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA4, "dma4", "hclk", HCLKCON, 4, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA3, "dma3", "hclk", HCLKCON, 3, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA2, "dma2", "hclk", HCLKCON, 2, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA1, "dma1", "hclk", HCLKCON, 1, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA0, "dma0", "hclk", HCLKCON, 0, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_GPIO, "gpio", "pclk", PCLKCON, 13, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_RTC, "rtc", "pclk", PCLKCON, 12, 0, 0),
	GATE(PCLK_WDT, "wdt", "pclk", PCLKCON, 11, 0, 0),
	GATE(PCLK_PWM, "pwm", "pclk", PCLKCON, 10, 0, 0),
	GATE(PCLK_I2S0, "i2s0", "pclk", PCLKCON, 9, 0, 0),
	GATE(PCLK_AC97, "ac97", "pclk", PCLKCON, 8, 0, 0),
	GATE(PCLK_ADC, "adc", "pclk", PCLKCON, 7, 0, 0),
	GATE(PCLK_SPI0, "spi0", "pclk", PCLKCON, 6, 0, 0),
	GATE(PCLK_I2C0, "i2c0", "pclk", PCLKCON, 4, 0, 0),
	GATE(PCLK_UART3, "uart3", "pclk", PCLKCON, 3, 0, 0),
	GATE(PCLK_UART2, "uart2", "pclk", PCLKCON, 2, 0, 0),
	GATE(PCLK_UART1, "uart1", "pclk", PCLKCON, 1, 0, 0),
	GATE(PCLK_UART0, "uart0", "pclk", PCLKCON, 0, 0, 0),
};

static struct samsung_clock_alias s3c2443_common_aliases[] __initdata = {
	ALIAS(MSYSCLK, NULL, "msysclk"),
	ALIAS(ARMCLK, NULL, "armclk"),
	ALIAS(MPLL, NULL, "mpll"),
	ALIAS(EPLL, NULL, "epll"),
	ALIAS(HCLK, NULL, "hclk"),
	ALIAS(HCLK_SSMC, NULL, "nand"),
	ALIAS(PCLK_UART0, "s3c2440-uart.0", "uart"),
	ALIAS(PCLK_UART1, "s3c2440-uart.1", "uart"),
	ALIAS(PCLK_UART2, "s3c2440-uart.2", "uart"),
	ALIAS(PCLK_UART3, "s3c2440-uart.3", "uart"),
	ALIAS(PCLK_UART0, "s3c2440-uart.0", "clk_uart_baud2"),
	ALIAS(PCLK_UART1, "s3c2440-uart.1", "clk_uart_baud2"),
	ALIAS(PCLK_UART2, "s3c2440-uart.2", "clk_uart_baud2"),
	ALIAS(PCLK_UART3, "s3c2440-uart.3", "clk_uart_baud2"),
	ALIAS(SCLK_UART, NULL, "clk_uart_baud3"),
	ALIAS(PCLK_PWM, NULL, "timers"),
	ALIAS(PCLK_RTC, NULL, "rtc"),
	ALIAS(PCLK_WDT, NULL, "watchdog"),
	ALIAS(PCLK_ADC, NULL, "adc"),
	ALIAS(PCLK_I2C0, "s3c2410-i2c.0", "i2c"),
	ALIAS(HCLK_USBD, NULL, "usb-device"),
	ALIAS(HCLK_USBH, NULL, "usb-host"),
	ALIAS(SCLK_USBH, NULL, "usb-bus-host"),
	ALIAS(PCLK_SPI0, "s3c2443-spi.0", "spi"),
	ALIAS(PCLK_SPI0, "s3c2443-spi.0", "spi_busclk0"),
	ALIAS(HCLK_HSMMC1, "s3c-sdhci.1", "hsmmc"),
	ALIAS(HCLK_HSMMC1, "s3c-sdhci.1", "mmc_busclk.0"),
	ALIAS(PCLK_I2S0, "samsung-i2s.0", "iis"),
	ALIAS(SCLK_I2S0, NULL, "i2s-if"),
	ALIAS(HCLK_LCD, NULL, "lcd"),
	ALIAS(SCLK_FIMD, NULL, "sclk_fimd"),
};

/* S3C2416 specific clocks */

static struct samsung_pll_clock s3c2416_pll_clks[] __initdata = {
	PLL(pll_6552_s3c2416, MPLL, "mpll", "mpllref", LOCKCON0, MPLLCON, NULL),
	PLL(pll_6553, EPLL, "epll", "epllref", LOCKCON1, EPLLCON, NULL),
};

PNAME(s3c2416_hsmmc0_p) = { "sclk_hsmmc0", "sclk_hsmmcext" };
PNAME(s3c2416_hsmmc1_p) = { "sclk_hsmmc1", "sclk_hsmmcext" };
PNAME(s3c2416_hsspi0_p) = { "hsspi0_epll", "hsspi0_mpll" };

static struct clk_div_table armdiv_s3c2416_d[] = {
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 3 },
	{ .val = 3, .div = 4 },
	{ .val = 5, .div = 6 },
	{ .val = 7, .div = 8 },
	{ /* sentinel */ },
};

static struct samsung_div_clock s3c2416_dividers[] __initdata = {
	DIV_T(ARMDIV, "armdiv", "msysclk", CLKDIV0, 9, 3, armdiv_s3c2416_d),
	DIV(0, "div_hsspi0_mpll", "msysclk", CLKDIV2, 0, 4),
	DIV(0, "div_hsmmc0", "esysclk", CLKDIV2, 6, 2),
};

static struct samsung_mux_clock s3c2416_muxes[] __initdata = {
	MUX(MUX_HSMMC0, "mux_hsmmc0", s3c2416_hsmmc0_p, CLKSRC, 16, 1),
	MUX(MUX_HSMMC1, "mux_hsmmc1", s3c2416_hsmmc1_p, CLKSRC, 17, 1),
	MUX(MUX_HSSPI0, "mux_hsspi0", s3c2416_hsspi0_p, CLKSRC, 18, 1),
};

static struct samsung_gate_clock s3c2416_gates[] __initdata = {
	GATE(0, "hsspi0_mpll", "div_hsspi0_mpll", SCLKCON, 19, 0, 0),
	GATE(0, "hsspi0_epll", "div_hsspi0_epll", SCLKCON, 14, 0, 0),
	GATE(0, "sclk_hsmmc0", "div_hsmmc0", SCLKCON, 6, 0, 0),
	GATE(HCLK_2D, "2d", "hclk", HCLKCON, 20, 0, 0),
	GATE(HCLK_HSMMC0, "hsmmc0", "hclk", HCLKCON, 15, 0, 0),
	GATE(HCLK_IROM, "irom", "hclk", HCLKCON, 13, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_PCM, "pcm", "pclk", PCLKCON, 19, 0, 0),
};

static struct samsung_clock_alias s3c2416_aliases[] __initdata = {
	ALIAS(HCLK_HSMMC0, "s3c-sdhci.0", "hsmmc"),
	ALIAS(HCLK_HSMMC0, "s3c-sdhci.0", "mmc_busclk.0"),
	ALIAS(MUX_HSMMC0, "s3c-sdhci.0", "mmc_busclk.2"),
	ALIAS(MUX_HSMMC1, "s3c-sdhci.1", "mmc_busclk.2"),
	ALIAS(MUX_HSSPI0, "s3c2443-spi.0", "spi_busclk2"),
	ALIAS(ARMDIV, NULL, "armdiv"),
};

/* S3C2443 specific clocks */

static struct samsung_pll_clock s3c2443_pll_clks[] __initdata = {
	PLL(pll_3000, MPLL, "mpll", "mpllref", LOCKCON0, MPLLCON, NULL),
	PLL(pll_2126, EPLL, "epll", "epllref", LOCKCON1, EPLLCON, NULL),
};

static struct clk_div_table armdiv_s3c2443_d[] = {
	{ .val = 0, .div = 1 },
	{ .val = 8, .div = 2 },
	{ .val = 2, .div = 3 },
	{ .val = 9, .div = 4 },
	{ .val = 10, .div = 6 },
	{ .val = 11, .div = 8 },
	{ .val = 13, .div = 12 },
	{ .val = 15, .div = 16 },
	{ /* sentinel */ },
};

static struct samsung_div_clock s3c2443_dividers[] __initdata = {
	DIV_T(ARMDIV, "armdiv", "msysclk", CLKDIV0, 9, 4, armdiv_s3c2443_d),
	DIV(0, "div_cam", "esysclk", CLKDIV1, 26, 4),
};

static struct samsung_gate_clock s3c2443_gates[] __initdata = {
	GATE(SCLK_HSSPI0, "sclk_hsspi0", "div_hsspi0_epll", SCLKCON, 14, 0, 0),
	GATE(SCLK_CAM, "sclk_cam", "div_cam", SCLKCON, 11, 0, 0),
	GATE(HCLK_CFC, "cfc", "hclk", HCLKCON, 17, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_CAM, "cam", "hclk", HCLKCON, 8, 0, 0),
	GATE(PCLK_SPI1, "spi1", "pclk", PCLKCON, 15, 0, 0),
	GATE(PCLK_SDI, "sdi", "pclk", PCLKCON, 5, 0, 0),
};

static struct samsung_clock_alias s3c2443_aliases[] __initdata = {
	ALIAS(SCLK_HSSPI0, "s3c2443-spi.0", "spi_busclk2"),
	ALIAS(SCLK_HSMMC1, "s3c-sdhci.1", "mmc_busclk.2"),
	ALIAS(SCLK_CAM, NULL, "camif-upll"),
	ALIAS(PCLK_SPI1, "s3c2410-spi.0", "spi"),
	ALIAS(PCLK_SDI, NULL, "sdi"),
	ALIAS(HCLK_CFC, NULL, "cfc"),
	ALIAS(ARMDIV, NULL, "armdiv"),
};

/* S3C2450 specific clocks */

PNAME(s3c2450_cam_p) = { "div_cam", "hclk" };
PNAME(s3c2450_hsspi1_p) = { "hsspi1_epll", "hsspi1_mpll" };
PNAME(i2s1_p) = { "div_i2s1", "ext_i2s", "epllref", "epllref" };

static struct samsung_div_clock s3c2450_dividers[] __initdata = {
	DIV(0, "div_cam", "esysclk", CLKDIV1, 26, 4),
	DIV(0, "div_hsspi1_epll", "esysclk", CLKDIV2, 24, 2),
	DIV(0, "div_hsspi1_mpll", "msysclk", CLKDIV2, 16, 4),
	DIV(0, "div_i2s1", "esysclk", CLKDIV2, 12, 4),
};

static struct samsung_mux_clock s3c2450_muxes[] __initdata = {
	MUX(0, "mux_cam", s3c2450_cam_p, CLKSRC, 20, 1),
	MUX(MUX_HSSPI1, "mux_hsspi1", s3c2450_hsspi1_p, CLKSRC, 19, 1),
	MUX(0, "mux_i2s1", i2s1_p, CLKSRC, 12, 2),
};

static struct samsung_gate_clock s3c2450_gates[] __initdata = {
	GATE(SCLK_I2S1, "sclk_i2s1", "div_i2s1", SCLKCON, 5, 0, 0),
	GATE(HCLK_CFC, "cfc", "hclk", HCLKCON, 17, 0, 0),
	GATE(HCLK_CAM, "cam", "hclk", HCLKCON, 8, 0, 0),
	GATE(HCLK_DMA7, "dma7", "hclk", HCLKCON, 7, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_DMA6, "dma6", "hclk", HCLKCON, 6, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_I2S1, "i2s1", "pclk", PCLKCON, 17, 0, 0),
	GATE(PCLK_I2C1, "i2c1", "pclk", PCLKCON, 16, 0, 0),
	GATE(PCLK_SPI1, "spi1", "pclk", PCLKCON, 14, 0, 0),
};

static struct samsung_clock_alias s3c2450_aliases[] __initdata = {
	ALIAS(PCLK_SPI1, "s3c2443-spi.1", "spi"),
	ALIAS(PCLK_SPI1, "s3c2443-spi.1", "spi_busclk0"),
	ALIAS(MUX_HSSPI1, "s3c2443-spi.1", "spi_busclk2"),
	ALIAS(PCLK_I2C1, "s3c2410-i2c.1", "i2c"),
};

static int s3c2443_restart(struct notifier_block *this,
			   unsigned long mode, void *cmd)
{
	__raw_writel(0x533c2443, reg_base + SWRST);
	return NOTIFY_DONE;
}

static struct notifier_block s3c2443_restart_handler = {
	.notifier_call = s3c2443_restart,
	.priority = 129,
};

/*
 * fixed rate clocks generated outside the soc
 * Only necessary until the devicetree-move is complete
 */
static struct samsung_fixed_rate_clock s3c2443_common_frate_clks[] __initdata = {
	FRATE(0, "xti", NULL, 0, 0),
	FRATE(0, "ext", NULL, 0, 0),
	FRATE(0, "ext_i2s", NULL, 0, 0),
	FRATE(0, "ext_uart", NULL, 0, 0),
};

static void __init s3c2443_common_clk_register_fixed_ext(
		struct samsung_clk_provider *ctx, unsigned long xti_f)
{
	s3c2443_common_frate_clks[0].fixed_rate = xti_f;
	samsung_clk_register_fixed_rate(ctx, s3c2443_common_frate_clks,
				ARRAY_SIZE(s3c2443_common_frate_clks));
}

void __init s3c2443_common_clk_init(struct device_node *np, unsigned long xti_f,
				    int current_soc,
				    void __iomem *base)
{
	struct samsung_clk_provider *ctx;
	int ret;
	reg_base = base;

	if (np) {
		reg_base = of_iomap(np, 0);
		if (!reg_base)
			panic("%s: failed to map registers\n", __func__);
	}

	ctx = samsung_clk_init(np, reg_base, NR_CLKS);

	/* Register external clocks only in non-dt cases */
	if (!np)
		s3c2443_common_clk_register_fixed_ext(ctx, xti_f);

	/* Register PLLs. */
	if (current_soc == S3C2416 || current_soc == S3C2450)
		samsung_clk_register_pll(ctx, s3c2416_pll_clks,
				ARRAY_SIZE(s3c2416_pll_clks), reg_base);
	else
		samsung_clk_register_pll(ctx, s3c2443_pll_clks,
				ARRAY_SIZE(s3c2443_pll_clks), reg_base);

	/* Register common internal clocks. */
	samsung_clk_register_mux(ctx, s3c2443_common_muxes,
			ARRAY_SIZE(s3c2443_common_muxes));
	samsung_clk_register_div(ctx, s3c2443_common_dividers,
			ARRAY_SIZE(s3c2443_common_dividers));
	samsung_clk_register_gate(ctx, s3c2443_common_gates,
		ARRAY_SIZE(s3c2443_common_gates));
	samsung_clk_register_alias(ctx, s3c2443_common_aliases,
		ARRAY_SIZE(s3c2443_common_aliases));

	/* Register SoC-specific clocks. */
	switch (current_soc) {
	case S3C2450:
		samsung_clk_register_div(ctx, s3c2450_dividers,
				ARRAY_SIZE(s3c2450_dividers));
		samsung_clk_register_mux(ctx, s3c2450_muxes,
				ARRAY_SIZE(s3c2450_muxes));
		samsung_clk_register_gate(ctx, s3c2450_gates,
				ARRAY_SIZE(s3c2450_gates));
		samsung_clk_register_alias(ctx, s3c2450_aliases,
				ARRAY_SIZE(s3c2450_aliases));
		fallthrough;	/* as s3c2450 extends the s3c2416 clocks */
	case S3C2416:
		samsung_clk_register_div(ctx, s3c2416_dividers,
				ARRAY_SIZE(s3c2416_dividers));
		samsung_clk_register_mux(ctx, s3c2416_muxes,
				ARRAY_SIZE(s3c2416_muxes));
		samsung_clk_register_gate(ctx, s3c2416_gates,
				ARRAY_SIZE(s3c2416_gates));
		samsung_clk_register_alias(ctx, s3c2416_aliases,
				ARRAY_SIZE(s3c2416_aliases));
		break;
	case S3C2443:
		samsung_clk_register_div(ctx, s3c2443_dividers,
				ARRAY_SIZE(s3c2443_dividers));
		samsung_clk_register_gate(ctx, s3c2443_gates,
				ARRAY_SIZE(s3c2443_gates));
		samsung_clk_register_alias(ctx, s3c2443_aliases,
				ARRAY_SIZE(s3c2443_aliases));
		break;
	}

	samsung_clk_sleep_init(reg_base, s3c2443_clk_regs,
			       ARRAY_SIZE(s3c2443_clk_regs));

	samsung_clk_of_add_provider(np, ctx);

	ret = register_restart_handler(&s3c2443_restart_handler);
	if (ret)
		pr_warn("cannot register restart handler, %d\n", ret);
}

static void __init s3c2416_clk_init(struct device_node *np)
{
	s3c2443_common_clk_init(np, 0, S3C2416, NULL);
}
CLK_OF_DECLARE(s3c2416_clk, "samsung,s3c2416-clock", s3c2416_clk_init);

static void __init s3c2443_clk_init(struct device_node *np)
{
	s3c2443_common_clk_init(np, 0, S3C2443, NULL);
}
CLK_OF_DECLARE(s3c2443_clk, "samsung,s3c2443-clock", s3c2443_clk_init);

static void __init s3c2450_clk_init(struct device_node *np)
{
	s3c2443_common_clk_init(np, 0, S3C2450, NULL);
}
CLK_OF_DECLARE(s3c2450_clk, "samsung,s3c2450-clock", s3c2450_clk_init);
