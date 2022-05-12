// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-omap1/board-generic.c
 *
 * Modified from board-innovator1510.c
 *
 * Code for generic OMAP board. Should work on many OMAP systems where
 * the device drivers take care of all the necessary hardware initialization.
 * Do not put any board specific code to this file; create a new machine
 * type if you need custom low-level initializations.
 */
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/mux.h>

#include <mach/usb.h>

#include "common.h"

/* assume no Mini-AB port */

#ifdef CONFIG_ARCH_OMAP15XX
static struct omap_usb_config generic1510_usb_config __initdata = {
	.register_host	= 1,
	.register_dev	= 1,
	.hmc_mode	= 16,
	.pins[0]	= 3,
};
#endif

#if defined(CONFIG_ARCH_OMAP16XX)
static struct omap_usb_config generic1610_usb_config __initdata = {
#ifdef CONFIG_USB_OTG
	.otg		= 1,
#endif
	.register_host	= 1,
	.register_dev	= 1,
	.hmc_mode	= 16,
	.pins[0]	= 6,
};
#endif

static void __init omap_generic_init(void)
{
#ifdef CONFIG_ARCH_OMAP15XX
	if (cpu_is_omap15xx()) {
		/* mux pins for uarts */
		omap_cfg_reg(UART1_TX);
		omap_cfg_reg(UART1_RTS);
		omap_cfg_reg(UART2_TX);
		omap_cfg_reg(UART2_RTS);
		omap_cfg_reg(UART3_TX);
		omap_cfg_reg(UART3_RX);

		omap1_usb_init(&generic1510_usb_config);
	}
#endif
#if defined(CONFIG_ARCH_OMAP16XX)
	if (!cpu_is_omap1510()) {
		omap1_usb_init(&generic1610_usb_config);
	}
#endif

	omap_serial_init();
	omap_register_i2c_bus(1, 100, NULL, 0);
}

MACHINE_START(OMAP_GENERIC, "Generic OMAP1510/1610/1710")
	/* Maintainer: Tony Lindgren <tony@atomide.com> */
	.atag_offset	= 0x100,
	.map_io		= omap16xx_map_io,
	.init_early	= omap1_init_early,
	.init_irq	= omap1_init_irq,
	.handle_irq	= omap1_handle_irq,
	.init_machine	= omap_generic_init,
	.init_late	= omap1_init_late,
	.init_time	= omap1_timer_init,
	.restart	= omap1_restart,
MACHINE_END
