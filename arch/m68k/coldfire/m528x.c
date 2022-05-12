// SPDX-License-Identifier: GPL-2.0
/***************************************************************************/

/*
 *	m528x.c  -- platform support for ColdFire 528x based boards
 *
 *	Sub-architcture dependent initialization code for the Freescale
 *	5280, 5281 and 5282 CPUs.
 *
 *	Copyright (C) 1999-2003, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2001-2003, SnapGear Inc. (www.snapgear.com)
 */

/***************************************************************************/

#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfuart.h>
#include <asm/mcfclk.h>

/***************************************************************************/

DEFINE_CLK(pll, "pll.0", MCF_CLK);
DEFINE_CLK(sys, "sys.0", MCF_BUSCLK);
DEFINE_CLK(mcfpit0, "mcfpit.0", MCF_CLK);
DEFINE_CLK(mcfpit1, "mcfpit.1", MCF_CLK);
DEFINE_CLK(mcfpit2, "mcfpit.2", MCF_CLK);
DEFINE_CLK(mcfpit3, "mcfpit.3", MCF_CLK);
DEFINE_CLK(mcfuart0, "mcfuart.0", MCF_BUSCLK);
DEFINE_CLK(mcfuart1, "mcfuart.1", MCF_BUSCLK);
DEFINE_CLK(mcfuart2, "mcfuart.2", MCF_BUSCLK);
DEFINE_CLK(mcfqspi0, "mcfqspi.0", MCF_BUSCLK);
DEFINE_CLK(fec0, "fec.0", MCF_BUSCLK);
DEFINE_CLK(mcfi2c0, "imx1-i2c.0", MCF_BUSCLK);

struct clk *mcf_clks[] = {
	&clk_pll,
	&clk_sys,
	&clk_mcfpit0,
	&clk_mcfpit1,
	&clk_mcfpit2,
	&clk_mcfpit3,
	&clk_mcfuart0,
	&clk_mcfuart1,
	&clk_mcfuart2,
	&clk_mcfqspi0,
	&clk_fec0,
	&clk_mcfi2c0,
	NULL
};

/***************************************************************************/

static void __init m528x_qspi_init(void)
{
#if IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI)
	/* setup Port QS for QSPI with gpio CS control */
	__raw_writeb(0x07, MCFGPIO_PQSPAR);
#endif /* IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI) */
}

/***************************************************************************/

static void __init m528x_i2c_init(void)
{
#if IS_ENABLED(CONFIG_I2C_IMX)
	u16 paspar;

	/* setup Port AS Pin Assignment Register for I2C */
	/*  set PASPA0 to SCL and PASPA1 to SDA */
	paspar = readw(MCFGPIO_PASPAR);
	paspar |= 0xF;
	writew(paspar, MCFGPIO_PASPAR);
#endif /* IS_ENABLED(CONFIG_I2C_IMX) */
}

/***************************************************************************/

static void __init m528x_uarts_init(void)
{
	u8 port;

	/* make sure PUAPAR is set for UART0 and UART1 */
	port = readb(MCFGPIO_PUAPAR);
	port |= 0x03 | (0x03 << 2);
	writeb(port, MCFGPIO_PUAPAR);
}

/***************************************************************************/

static void __init m528x_fec_init(void)
{
	u16 v16;

	/* Set multi-function pins to ethernet mode for fec0 */
	v16 = readw(MCFGPIO_PASPAR);
	writew(v16 | 0xf00, MCFGPIO_PASPAR);
	writeb(0xc0, MCFGPIO_PEHLPAR);
}

/***************************************************************************/

#ifdef CONFIG_WILDFIRE
void wildfire_halt(void)
{
	writeb(0, 0x30000007);
	writeb(0x2, 0x30000007);
}
#endif

#ifdef CONFIG_WILDFIREMOD
void wildfiremod_halt(void)
{
	printk(KERN_INFO "WildFireMod hibernating...\n");

	/* Set portE.5 to Digital IO */
	writew(readw(MCFGPIO_PEPAR) & ~(1 << (5 * 2)), MCFGPIO_PEPAR);

	/* Make portE.5 an output */
	writeb(readb(MCFGPIO_PDDR_E) | (1 << 5), MCFGPIO_PDDR_E);

	/* Now toggle portE.5 from low to high */
	writeb(readb(MCFGPIO_PODR_E) & ~(1 << 5), MCFGPIO_PODR_E);
	writeb(readb(MCFGPIO_PODR_E) | (1 << 5), MCFGPIO_PODR_E);

	printk(KERN_EMERG "Failed to hibernate. Halting!\n");
}
#endif

void __init config_BSP(char *commandp, int size)
{
#ifdef CONFIG_WILDFIRE
	mach_halt = wildfire_halt;
#endif
#ifdef CONFIG_WILDFIREMOD
	mach_halt = wildfiremod_halt;
#endif
	mach_sched_init = hw_timer_init;
	m528x_uarts_init();
	m528x_fec_init();
	m528x_qspi_init();
	m528x_i2c_init();
}

/***************************************************************************/
