// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Registration of Cobalt LED platform device.
 *
 *  Copyright (C) 2007	Yoichi Yuasa <yuasa@linux-mips.org>
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <cobalt.h>

static struct resource cobalt_led_resource __initdata = {
	.start	= 0x1c000000,
	.end	= 0x1c000000,
	.flags	= IORESOURCE_MEM,
};

static __init int cobalt_led_add(void)
{
	struct platform_device *pdev;
	int retval;

	if (cobalt_board_id == COBALT_BRD_ID_QUBE1 ||
	    cobalt_board_id == COBALT_BRD_ID_QUBE2)
		pdev = platform_device_alloc("cobalt-qube-leds", -1);
	else
		pdev = platform_device_alloc("cobalt-raq-leds", -1);

	if (!pdev)
		return -ENOMEM;

	retval = platform_device_add_resources(pdev, &cobalt_led_resource, 1);
	if (retval)
		goto err_free_device;

	retval = platform_device_add(pdev);
	if (retval)
		goto err_free_device;

	return 0;

err_free_device:
	platform_device_put(pdev);

	return retval;
}
device_initcall(cobalt_led_add);
