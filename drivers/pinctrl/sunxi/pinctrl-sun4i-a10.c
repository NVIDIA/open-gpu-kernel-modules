/*
 * Allwinner A10 SoCs pinctrl driver.
 *
 * Copyright (C) 2014 Maxime Ripard
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-sunxi.h"

static const struct sunxi_desc_pin sun4i_a10_pins[] = {
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXD3 */
		  SUNXI_FUNCTION(0x3, "spi1"),		/* CS0 */
		  SUNXI_FUNCTION(0x4, "uart2"),		/* RTS */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXD3 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXD2 */
		  SUNXI_FUNCTION(0x3, "spi1"),		/* CLK */
		  SUNXI_FUNCTION(0x4, "uart2"),		/* CTS */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXD2 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXD1 */
		  SUNXI_FUNCTION(0x3, "spi1"),		/* MOSI */
		  SUNXI_FUNCTION(0x4, "uart2"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXD1 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXD0 */
		  SUNXI_FUNCTION(0x3, "spi1"),		/* MISO */
		  SUNXI_FUNCTION(0x4, "uart2"),		/* RX */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXD0 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXD3 */
		  SUNXI_FUNCTION(0x3, "spi1"),		/* CS1 */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXD3 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXD2 */
		  SUNXI_FUNCTION(0x3, "spi3"),		/* CS0 */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXD2 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXD1 */
		  SUNXI_FUNCTION(0x3, "spi3"),		/* CLK */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXD1 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXD0 */
		  SUNXI_FUNCTION(0x3, "spi3"),		/* MOSI */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXD0 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXCK */
		  SUNXI_FUNCTION(0x3, "spi3"),		/* MISO */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXERR */
		  SUNXI_FUNCTION(0x3, "spi3"),		/* CS1 */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GNULL / ERXERR */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x6, "i2s1",	/* MCLK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ERXDV */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GRXDV */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* EMDC */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* RX */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* EMDC */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* EMDIO */
		  SUNXI_FUNCTION(0x3, "uart6"),		/* TX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* RTS */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* EMDIO */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXEN */
		  SUNXI_FUNCTION(0x3, "uart6"),		/* RX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* CTS */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXCTL / ETXEN */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXCK */
		  SUNXI_FUNCTION(0x3, "uart7"),		/* TX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* DTR */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GNULL / ETXCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x6, "i2s1",	/* BCLK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ECRS */
		  SUNXI_FUNCTION(0x3, "uart7"),		/* RX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* DSR */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GTXCK / ECRS */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x6, "i2s1",	/* LRCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ECOL */
		  SUNXI_FUNCTION(0x3, "can"),		/* TX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* DCD */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GCLKIN / ECOL */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x6, "i2s1",	/* DO */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "emac"),		/* ETXERR */
		  SUNXI_FUNCTION(0x3, "can"),		/* RX */
		  SUNXI_FUNCTION(0x4, "uart1"),		/* RING */
		  SUNXI_FUNCTION_VARIANT(0x5, "gmac",	/* GNULL / ETXERR */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x6, "i2s1",	/* DI */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c0"),		/* SCK */
		  SUNXI_FUNCTION_VARIANT(0x3, "pll_lock_dbg",
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c0")),		/* SDA */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "pwm",	/* PWM0 */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_VARIANT(0x3, "pwm",	/* PWM0 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "ir0",	/* TX */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_VARIANT(0x3, "pwm",	/* PWM1 */
					 PINCTRL_SUN8I_R40),
		/*
		 * The SPDIF block is not referenced at all in the A10 user
		 * manual. However it is described in the code leaked and the
		 * pin descriptions are declared in the A20 user manual which
		 * is pin compatible with this device.
		 */
		  SUNXI_FUNCTION(0x4, "spdif")),        /* SPDIF MCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ir0")),		/* RX */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  /*
		   * On A10 there's only one I2S controller and the pin group
		   * is simply named "i2s". On A20 there's two and thus it's
		   * renamed to "i2s0". Deal with these name here, in order
		   * to satisfy existing device trees.
		   */
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* MCLK */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* MCLK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x3, "ac97")),		/* MCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* BCLK */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* BCLK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x3, "ac97")),		/* BCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* LRCK */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* LRCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x3, "ac97")),		/* SYNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* DO0 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* DO0 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x3, "ac97")),		/* DO */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* DO1 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* DO1 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x4, "pwm",	/* PWM6 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* DO2 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* DO2 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_VARIANT(0x4, "pwm",	/* PWM7 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* DO3 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* DO3 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s",	/* DI */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x2, "i2s0",	/* DI */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x3, "ac97"),		/* DI */
		/* Undocumented mux function on A10 - See SPDIF MCLK above */
		  SUNXI_FUNCTION_VARIANT(0x4, "spdif",	/* SPDIF IN */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi2"),		/* CS1 */
		/* Undocumented mux function on A10 - See SPDIF MCLK above */
		  SUNXI_FUNCTION(0x4, "spdif")),        /* SPDIF OUT */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi2"),		/* CS0 */
		  SUNXI_FUNCTION(0x3, "jtag")),		/* MS0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi2"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "jtag")),		/* CK0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi2"),		/* MOSI */
		  SUNXI_FUNCTION(0x3, "jtag")),		/* DO0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi2"),		/* MISO */
		  SUNXI_FUNCTION(0x3, "jtag")),		/* DI0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 18),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c1")),		/* SCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 19),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c1")),		/* SDA */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 20),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c2"),		/* SCK */
		  SUNXI_FUNCTION_VARIANT(0x4, "pwm",	/* PWM4 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 21),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "i2c2"),		/* SDA */
		  SUNXI_FUNCTION_VARIANT(0x4, "pwm",	/* PWM5 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 22),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart0"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x3, "ir1",	/* TX */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 23),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart0"),		/* RX */
		  SUNXI_FUNCTION(0x3, "ir1")),		/* RX */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NWE */
		  SUNXI_FUNCTION(0x3, "spi0")),		/* MOSI */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NALE */
		  SUNXI_FUNCTION(0x3, "spi0")),		/* MISO */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NCLE */
		  SUNXI_FUNCTION(0x3, "spi0")),		/* SCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0")),	/* NCE1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0")),	/* NCE0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NRE# */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* DS */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NRB0 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* CMD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NRB1 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ0 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ1 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* D1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ2 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* D2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ3 */
		  SUNXI_FUNCTION(0x3, "mmc2")),		/* D3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ4 */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* D4 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ5 */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* D5 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ6 */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* D6 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQ7 */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* D7 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0")),	/* NWP */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0")),	/* NCE2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 18),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0")),	/* NCE3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 19),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NCE4 */
		  SUNXI_FUNCTION(0x3, "spi2")),		/* CS0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 20),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NCE5 */
		  SUNXI_FUNCTION(0x3, "spi2")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 21),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NCE6 */
		  SUNXI_FUNCTION(0x3, "spi2")),		/* MOSI */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 22),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NCE7 */
		  SUNXI_FUNCTION(0x3, "spi2")),		/* MISO */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 23),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x3, "spi0")),		/* CS0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 24),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "nand0"),		/* NDQS */
		  SUNXI_FUNCTION_VARIANT(0x3, "mmc2",	/* RST */
					 PINCTRL_SUN8I_R40)),
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D0 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VP0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D1 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VN0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D2 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VP1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D3 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VN1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D4 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VP2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D5 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VN2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D6 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VPC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D7 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D8 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VP3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D9 */
		  SUNXI_FUNCTION(0x3, "lvds0")),	/* VM3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D10 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VP0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D11 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VN0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D12 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VP1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D13 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VN1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D14 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VP2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D15 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VN2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D16 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VPC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D17 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 18),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D18 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VP3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 19),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D19 */
		  SUNXI_FUNCTION(0x3, "lvds1")),	/* VN3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 20),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D20 */
		  SUNXI_FUNCTION(0x3, "csi1")),		/* MCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 21),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D21 */
		  SUNXI_FUNCTION(0x3, "sim")),		/* VPPEN */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 22),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D22 */
		  SUNXI_FUNCTION(0x3, "sim")),		/* VPPPP */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 23),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* D23 */
		  SUNXI_FUNCTION(0x3, "sim")),		/* DET */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 24),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "sim")),		/* VCCEN */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 25),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* DE */
		  SUNXI_FUNCTION(0x3, "sim")),		/* RST */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 26),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* HSYNC */
		  SUNXI_FUNCTION(0x3, "sim")),		/* SCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 27),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd0"),		/* VSYNC */
		  SUNXI_FUNCTION(0x3, "sim")),		/* SDA */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* PCK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* ERR */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* CK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* SYNC */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* HSYNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* DVLD */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* VSYNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D0 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D1 */
		  SUNXI_FUNCTION(0x3, "csi0"),		/* D1 */
		  SUNXI_FUNCTION(0x4, "sim")),		/* VPPEN */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D2 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D3 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D4 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D5 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D6 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts0"),		/* D7 */
		  SUNXI_FUNCTION(0x3, "csi0")),		/* D7 */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* D1 */
		  SUNXI_FUNCTION(0x4, "jtag")),		/* MSI */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* D0 */
		  SUNXI_FUNCTION(0x4, "jtag")),		/* DI1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* CLK */
		  SUNXI_FUNCTION(0x4, "uart0")),	/* TX */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* CMD */
		  SUNXI_FUNCTION(0x4, "jtag")),		/* DO1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* D3 */
		  SUNXI_FUNCTION(0x4, "uart0")),	/* RX */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc0"),		/* D2 */
		  SUNXI_FUNCTION(0x4, "jtag")),		/* CK1 */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* PCK */
		  SUNXI_FUNCTION(0x4, "mmc1")),		/* CMD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* ERR */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* CK */
		  SUNXI_FUNCTION(0x4, "mmc1")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* SYNC */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* HSYNC */
		  SUNXI_FUNCTION(0x4, "mmc1")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* DVLD */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* VSYNC */
		  SUNXI_FUNCTION(0x4, "mmc1")),		/* D1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D0 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D0 */
		  SUNXI_FUNCTION(0x4, "mmc1"),		/* D2 */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D1 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D1 */
		  SUNXI_FUNCTION(0x4, "mmc1"),		/* D3 */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D2 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D2 */
		  SUNXI_FUNCTION(0x4, "uart3"),		/* TX */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D3 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D3 */
		  SUNXI_FUNCTION(0x4, "uart3"),		/* RX */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D4 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D4 */
		  SUNXI_FUNCTION(0x4, "uart3"),		/* RTS */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D5 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D5 */
		  SUNXI_FUNCTION(0x4, "uart3"),		/* CTS */
		  SUNXI_FUNCTION(0x5, "csi0"),		/* D13 */
		  SUNXI_FUNCTION_VARIANT(0x6, "bist",	/* RESULT0 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D6 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D6 */
		  SUNXI_FUNCTION(0x4, "uart4"),		/* TX */
		  SUNXI_FUNCTION(0x5, "csi0"),		/* D14 */
		  SUNXI_FUNCTION_VARIANT(0x6, "bist",	/* RESULT1 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ts1"),		/* D7 */
		  SUNXI_FUNCTION(0x3, "csi1"),		/* D7 */
		  SUNXI_FUNCTION(0x4, "uart4"),		/* RX */
		  SUNXI_FUNCTION(0x5, "csi0")),		/* D15 */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D0 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAA0 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart3"),		/* TX */
		  SUNXI_FUNCTION_IRQ(0x6, 0),		/* EINT0 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D1 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAA1 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart3"),		/* RX */
		  SUNXI_FUNCTION_IRQ(0x6, 1),		/* EINT1 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D2 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAA2 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart3"),		/* RTS */
		  SUNXI_FUNCTION_IRQ(0x6, 2),		/* EINT2 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D3 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAIRQ */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart3"),		/* CTS */
		  SUNXI_FUNCTION_IRQ(0x6, 3),		/* EINT3 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D4 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD0 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart4"),		/* TX */
		  SUNXI_FUNCTION_IRQ(0x6, 4),		/* EINT4 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D5 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD1 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart4"),		/* RX */
		  SUNXI_FUNCTION_IRQ(0x6, 5),		/* EINT5 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D6 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD2 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart5"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* BS */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 6),		/* EINT6 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D7 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD3 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "uart5"),		/* RX */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* CLK */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 7),		/* EINT7 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D8 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD4 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXD3 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN0 */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* D0 */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 8),		/* EINT8 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D9 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD5 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXD2 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN1 */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* D1 */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 9),		/* EINT9 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D10 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD6 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXD1 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN2 */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* D2 */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 10),		/* EINT10 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D11 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD7 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXD0 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN3 */
		  SUNXI_FUNCTION_VARIANT(0x5, "ms",	/* D3 */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_IRQ(0x6, 11),		/* EINT11 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D12 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD8 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "ps2"),		/* SCK1 */
		  SUNXI_FUNCTION_IRQ(0x6, 12),		/* EINT12 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D13 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD9 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION(0x4, "ps2"),		/* SDA1 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* RST */
		  SUNXI_FUNCTION_IRQ(0x6, 13),		/* EINT13 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D14 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD10 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXD3 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN4 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* VPPEN */
		  SUNXI_FUNCTION_IRQ(0x6, 14),		/* EINT14 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D15 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD11 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXD2 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN5 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* VPPPP */
		  SUNXI_FUNCTION_IRQ(0x6, 15),		/* EINT15 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D15 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D16 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD12 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXD1 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN6 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* DET */
		  SUNXI_FUNCTION_IRQ(0x6, 16),		/* EINT16 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D16 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D17 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD13 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXD0 */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* IN7 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* VCCEN */
		  SUNXI_FUNCTION_IRQ(0x6, 17),		/* EINT17 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D17 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 18),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D18 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD14 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT0 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* SCK */
		  SUNXI_FUNCTION_IRQ(0x6, 18),		/* EINT18 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D18 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 19),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D19 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAD15 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXERR */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT1 */
		  SUNXI_FUNCTION(0x5, "sim"),		/* SDA */
		  SUNXI_FUNCTION_IRQ(0x6, 19),		/* EINT19 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D19 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 20),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D20 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAOE */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ERXDV */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "can"),		/* TX */
		  SUNXI_FUNCTION_IRQ(0x6, 20),		/* EINT20 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D20 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 21),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D21 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATADREQ */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* EMDC */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "can"),		/* RX */
		  SUNXI_FUNCTION_IRQ(0x6, 21),		/* EINT21 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D21 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 22),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D22 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATADACK */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* EMDIO */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT2 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* CMD */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D22 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 23),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* D23 */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATACS0 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXEN */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT3 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* CLK */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* D23 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 24),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* CLK */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATACS1 */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT4 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* D0 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* PCLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 25),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* DE */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAIORDY */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ECRS */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT5 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* D1 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* FIELD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 26),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* HSYNC */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAIOR */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ECOL */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT6 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* D2 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* HSYNC */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 27),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "lcd1"),		/* VSYNC */
		  SUNXI_FUNCTION_VARIANT(0x3, "pata",	/* ATAIOW */
					 PINCTRL_SUN4I_A10),
		  SUNXI_FUNCTION_VARIANT(0x3, "emac",	/* ETXERR */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION(0x4, "keypad"),	/* OUT7 */
		  SUNXI_FUNCTION(0x5, "mmc1"),		/* D3 */
		  SUNXI_FUNCTION(0x7, "csi1")),		/* VSYNC */
	/* Hole */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x3, "i2c3",	/* SCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x3, "i2c3",	/* SDA */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION_VARIANT(0x3, "i2c4",	/* SCK */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "pwm"),		/* PWM1 */
		  SUNXI_FUNCTION_VARIANT(0x3, "i2c4",	/* SDA */
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* CMD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* CLK */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* D0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* D1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* D2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "mmc3")),		/* D3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi0"),		/* CS0 */
		  SUNXI_FUNCTION(0x3, "uart5"),		/* TX */
		  SUNXI_FUNCTION_IRQ(0x6, 22)),		/* EINT22 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 11),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi0"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "uart5"),		/* RX */
		  SUNXI_FUNCTION_IRQ(0x6, 23)),		/* EINT23 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 12),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi0"),		/* MOSI */
		  SUNXI_FUNCTION(0x3, "uart6"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x4, "clk_out_a",
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_IRQ(0x6, 24)),		/* EINT24 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 13),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi0"),		/* MISO */
		  SUNXI_FUNCTION(0x3, "uart6"),		/* RX */
		  SUNXI_FUNCTION_VARIANT(0x4, "clk_out_b",
					 PINCTRL_SUN7I_A20 |
					 PINCTRL_SUN8I_R40),
		  SUNXI_FUNCTION_IRQ(0x6, 25)),		/* EINT25 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 14),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi0"),		/* CS1 */
		  SUNXI_FUNCTION(0x3, "ps2"),		/* SCK1 */
		  SUNXI_FUNCTION(0x4, "timer4"),	/* TCLKIN0 */
		  SUNXI_FUNCTION_IRQ(0x6, 26)),		/* EINT26 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 15),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi1"),		/* CS1 */
		  SUNXI_FUNCTION(0x3, "ps2"),		/* SDA1 */
		  SUNXI_FUNCTION(0x4, "timer5"),	/* TCLKIN1 */
		  SUNXI_FUNCTION_IRQ(0x6, 27)),		/* EINT27 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 16),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi1"),		/* CS0 */
		  SUNXI_FUNCTION(0x3, "uart2"),		/* RTS */
		  SUNXI_FUNCTION_IRQ(0x6, 28)),		/* EINT28 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 17),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi1"),		/* CLK */
		  SUNXI_FUNCTION(0x3, "uart2"),		/* CTS */
		  SUNXI_FUNCTION_IRQ(0x6, 29)),		/* EINT29 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 18),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi1"),		/* MOSI */
		  SUNXI_FUNCTION(0x3, "uart2"),		/* TX */
		  SUNXI_FUNCTION_IRQ(0x6, 30)),		/* EINT30 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 19),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "spi1"),		/* MISO */
		  SUNXI_FUNCTION(0x3, "uart2"),		/* RX */
		  SUNXI_FUNCTION_IRQ(0x6, 31)),		/* EINT31 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 20),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ps2"),		/* SCK0 */
		  SUNXI_FUNCTION(0x3, "uart7"),		/* TX */
		  SUNXI_FUNCTION_VARIANT(0x4, "hdmi",	/* HSCL */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_VARIANT(0x6, "pwm",	/* PWM2 */
					 PINCTRL_SUN8I_R40)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 21),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "ps2"),		/* SDA0 */
		  SUNXI_FUNCTION(0x3, "uart7"),		/* RX */
		  SUNXI_FUNCTION_VARIANT(0x4, "hdmi",	/* HSDA */
					 PINCTRL_SUN4I_A10 |
					 PINCTRL_SUN7I_A20),
		  SUNXI_FUNCTION_VARIANT(0x6, "pwm",	/* PWM3 */
					 PINCTRL_SUN8I_R40)),
};

static const struct sunxi_pinctrl_desc sun4i_a10_pinctrl_data = {
	.pins = sun4i_a10_pins,
	.npins = ARRAY_SIZE(sun4i_a10_pins),
	.irq_banks = 1,
	.irq_read_needs_mux = true,
	.disable_strict_mode = true,
};

static int sun4i_a10_pinctrl_probe(struct platform_device *pdev)
{
	unsigned long variant = (unsigned long)of_device_get_match_data(&pdev->dev);

	return sunxi_pinctrl_init_with_variant(pdev, &sun4i_a10_pinctrl_data,
					       variant);
}

static const struct of_device_id sun4i_a10_pinctrl_match[] = {
	{
		.compatible = "allwinner,sun4i-a10-pinctrl",
		.data = (void *)PINCTRL_SUN4I_A10
	},
	{
		.compatible = "allwinner,sun7i-a20-pinctrl",
		.data = (void *)PINCTRL_SUN7I_A20
	},
	{
		.compatible = "allwinner,sun8i-r40-pinctrl",
		.data = (void *)PINCTRL_SUN8I_R40
	},
	{}
};

static struct platform_driver sun4i_a10_pinctrl_driver = {
	.probe	= sun4i_a10_pinctrl_probe,
	.driver	= {
		.name		= "sun4i-pinctrl",
		.of_match_table	= sun4i_a10_pinctrl_match,
	},
};
builtin_platform_driver(sun4i_a10_pinctrl_driver);
