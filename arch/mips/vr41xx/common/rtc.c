// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  NEC VR4100 series RTC platform device.
 *
 *  Copyright (C) 2007	Yoichi Yuasa <yuasa@linux-mips.org>
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <asm/cpu.h>
#include <asm/vr41xx/irq.h>

static struct resource rtc_type1_resource[] __initdata = {
	{
		.start	= 0x0b0000c0,
		.end	= 0x0b0000df,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 0x0b0001c0,
		.end	= 0x0b0001df,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= ELAPSEDTIME_IRQ,
		.end	= ELAPSEDTIME_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= RTCLONG1_IRQ,
		.end	= RTCLONG1_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource rtc_type2_resource[] __initdata = {
	{
		.start	= 0x0f000100,
		.end	= 0x0f00011f,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 0x0f000120,
		.end	= 0x0f00013f,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= ELAPSEDTIME_IRQ,
		.end	= ELAPSEDTIME_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= RTCLONG1_IRQ,
		.end	= RTCLONG1_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static int __init vr41xx_rtc_add(void)
{
	struct platform_device *pdev;
	struct resource *res;
	unsigned int num;
	int retval;

	pdev = platform_device_alloc("RTC", -1);
	if (!pdev)
		return -ENOMEM;

	switch (current_cpu_type()) {
	case CPU_VR4111:
	case CPU_VR4121:
		res = rtc_type1_resource;
		num = ARRAY_SIZE(rtc_type1_resource);
		break;
	case CPU_VR4122:
	case CPU_VR4131:
	case CPU_VR4133:
		res = rtc_type2_resource;
		num = ARRAY_SIZE(rtc_type2_resource);
		break;
	default:
		retval = -ENODEV;
		goto err_free_device;
	}

	retval = platform_device_add_resources(pdev, res, num);
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
device_initcall(vr41xx_rtc_add);
