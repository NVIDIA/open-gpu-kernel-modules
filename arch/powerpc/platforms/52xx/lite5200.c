// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Freescale Lite5200 board support
 *
 * Written by: Grant Likely <grant.likely@secretlab.ca>
 *
 * Copyright (C) Secret Lab Technologies Ltd. 2006. All rights reserved.
 * Copyright 2006 Freescale Semiconductor, Inc. All rights reserved.
 *
 * Description:
 */

#undef DEBUG

#include <linux/init.h>
#include <linux/pci.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/root_dev.h>
#include <linux/initrd.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/prom.h>
#include <asm/mpc52xx.h>

/* ************************************************************************
 *
 * Setup the architecture
 *
 */

/* mpc5200 device tree match tables */
static const struct of_device_id mpc5200_cdm_ids[] __initconst = {
	{ .compatible = "fsl,mpc5200-cdm", },
	{ .compatible = "mpc5200-cdm", },
	{}
};

static const struct of_device_id mpc5200_gpio_ids[] __initconst = {
	{ .compatible = "fsl,mpc5200-gpio", },
	{ .compatible = "mpc5200-gpio", },
	{}
};

/*
 * Fix clock configuration.
 *
 * Firmware is supposed to be responsible for this.  If you are creating a
 * new board port, do *NOT* duplicate this code.  Fix your boot firmware
 * to set it correctly in the first place
 */
static void __init
lite5200_fix_clock_config(void)
{
	struct device_node *np;
	struct mpc52xx_cdm  __iomem *cdm;
	/* Map zones */
	np = of_find_matching_node(NULL, mpc5200_cdm_ids);
	cdm = of_iomap(np, 0);
	of_node_put(np);
	if (!cdm) {
		printk(KERN_ERR "%s() failed; expect abnormal behaviour\n",
		       __func__);
		return;
	}

	/* Use internal 48 Mhz */
	out_8(&cdm->ext_48mhz_en, 0x00);
	out_8(&cdm->fd_enable, 0x01);
	if (in_be32(&cdm->rstcfg) & 0x40)	/* Assumes 33Mhz clock */
		out_be16(&cdm->fd_counters, 0x0001);
	else
		out_be16(&cdm->fd_counters, 0x5555);

	/* Unmap the regs */
	iounmap(cdm);
}

/*
 * Fix setting of port_config register.
 *
 * Firmware is supposed to be responsible for this.  If you are creating a
 * new board port, do *NOT* duplicate this code.  Fix your boot firmware
 * to set it correctly in the first place
 */
static void __init
lite5200_fix_port_config(void)
{
	struct device_node *np;
	struct mpc52xx_gpio __iomem *gpio;
	u32 port_config;

	np = of_find_matching_node(NULL, mpc5200_gpio_ids);
	gpio = of_iomap(np, 0);
	of_node_put(np);
	if (!gpio) {
		printk(KERN_ERR "%s() failed. expect abnormal behavior\n",
		       __func__);
		return;
	}

	/* Set port config */
	port_config = in_be32(&gpio->port_config);

	port_config &= ~0x00800000;	/* 48Mhz internal, pin is GPIO	*/

	port_config &= ~0x00007000;	/* USB port : Differential mode	*/
	port_config |=  0x00001000;	/*            USB 1 only	*/

	port_config &= ~0x03000000;	/* ATA CS is on csb_4/5		*/
	port_config |=  0x01000000;

	pr_debug("port_config: old:%x new:%x\n",
	         in_be32(&gpio->port_config), port_config);
	out_be32(&gpio->port_config, port_config);

	/* Unmap zone */
	iounmap(gpio);
}

#ifdef CONFIG_PM
static void lite5200_suspend_prepare(void __iomem *mbar)
{
	u8 pin = 1;	/* GPIO_WKUP_1 (GPIO_PSC2_4) */
	u8 level = 0;	/* wakeup on low level */
	mpc52xx_set_wakeup_gpio(pin, level);

	/*
	 * power down usb port
	 * this needs to be called before of-ohci suspend code
	 */

	/* set ports to "power switched" and "powered at the same time"
	 * USB Rh descriptor A: NPS = 0, PSM = 0 */
	out_be32(mbar + 0x1048, in_be32(mbar + 0x1048) & ~0x300);
	/* USB Rh status: LPS = 1 - turn off power */
	out_be32(mbar + 0x1050, 0x00000001);
}

static void lite5200_resume_finish(void __iomem *mbar)
{
	/* USB Rh status: LPSC = 1 - turn on power */
	out_be32(mbar + 0x1050, 0x00010000);
}
#endif

static void __init lite5200_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("lite5200_setup_arch()", 0);

	/* Map important registers from the internal memory map */
	mpc52xx_map_common_devices();

	/* Some mpc5200 & mpc5200b related configuration */
	mpc5200_setup_xlb_arbiter();

	/* Fix things that firmware should have done. */
	lite5200_fix_clock_config();
	lite5200_fix_port_config();

#ifdef CONFIG_PM
	mpc52xx_suspend.board_suspend_prepare = lite5200_suspend_prepare;
	mpc52xx_suspend.board_resume_finish = lite5200_resume_finish;
	lite5200_pm_init();
#endif
}

static const char * const board[] __initconst = {
	"fsl,lite5200",
	"fsl,lite5200b",
	NULL,
};

/*
 * Called very early, MMU is off, device-tree isn't unflattened
 */
static int __init lite5200_probe(void)
{
	return of_device_compatible_match(of_root, board);
}

define_machine(lite5200) {
	.name 		= "lite5200",
	.probe 		= lite5200_probe,
	.setup_arch 	= lite5200_setup_arch,
	.discover_phbs	= mpc52xx_setup_pci,
	.init		= mpc52xx_declare_of_platform_devices,
	.init_IRQ 	= mpc52xx_init_irq,
	.get_irq 	= mpc52xx_get_irq,
	.restart	= mpc52xx_restart,
	.calibrate_decr	= generic_calibrate_decr,
};
