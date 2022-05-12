/*
 * drivers/char/watchdog/ixp4xx_wdt.c
 *
 * Watchdog driver for Intel IXP4xx network processors
 *
 * Author: Deepak Saxena <dsaxena@plexity.net>
 *
 * Copyright 2004 (c) MontaVista, Software, Inc.
 * Based on sa1100 driver, Copyright (C) 2000 Oleg Drokin <green@crimea.edu>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

static bool nowayout = WATCHDOG_NOWAYOUT;
static int heartbeat = 60;	/* (secs) Default is 1 minute */
static unsigned long wdt_status;
static unsigned long boot_status;
static DEFINE_SPINLOCK(wdt_lock);

#define WDT_TICK_RATE (IXP4XX_PERIPHERAL_BUS_CLOCK * 1000000UL)

#define	WDT_IN_USE		0
#define	WDT_OK_TO_CLOSE		1

static void wdt_enable(void)
{
	spin_lock(&wdt_lock);
	*IXP4XX_OSWK = IXP4XX_WDT_KEY;
	*IXP4XX_OSWE = 0;
	*IXP4XX_OSWT = WDT_TICK_RATE * heartbeat;
	*IXP4XX_OSWE = IXP4XX_WDT_COUNT_ENABLE | IXP4XX_WDT_RESET_ENABLE;
	*IXP4XX_OSWK = 0;
	spin_unlock(&wdt_lock);
}

static void wdt_disable(void)
{
	spin_lock(&wdt_lock);
	*IXP4XX_OSWK = IXP4XX_WDT_KEY;
	*IXP4XX_OSWE = 0;
	*IXP4XX_OSWK = 0;
	spin_unlock(&wdt_lock);
}

static int ixp4xx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(WDT_IN_USE, &wdt_status))
		return -EBUSY;

	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);
	wdt_enable();
	return stream_open(inode, file);
}

static ssize_t
ixp4xx_wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;
				if (c == 'V')
					set_bit(WDT_OK_TO_CLOSE, &wdt_status);
			}
		}
		wdt_enable();
	}
	return len;
}

static const struct watchdog_info ident = {
	.options	= WDIOF_CARDRESET | WDIOF_MAGICCLOSE |
			  WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity	= "IXP4xx Watchdog",
};


static long ixp4xx_wdt_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	int ret = -ENOTTY;
	int time;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info *)arg, &ident,
				   sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, (int *)arg);
		break;

	case WDIOC_GETBOOTSTATUS:
		ret = put_user(boot_status, (int *)arg);
		break;

	case WDIOC_KEEPALIVE:
		wdt_enable();
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(time, (int *)arg);
		if (ret)
			break;

		if (time <= 0 || time > 60) {
			ret = -EINVAL;
			break;
		}

		heartbeat = time;
		wdt_enable();
		fallthrough;

	case WDIOC_GETTIMEOUT:
		ret = put_user(heartbeat, (int *)arg);
		break;
	}
	return ret;
}

static int ixp4xx_wdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_OK_TO_CLOSE, &wdt_status))
		wdt_disable();
	else
		pr_crit("Device closed unexpectedly - timer will not stop\n");
	clear_bit(WDT_IN_USE, &wdt_status);
	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

	return 0;
}


static const struct file_operations ixp4xx_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= ixp4xx_wdt_write,
	.unlocked_ioctl	= ixp4xx_wdt_ioctl,
	.compat_ioctl	= compat_ptr_ioctl,
	.open		= ixp4xx_wdt_open,
	.release	= ixp4xx_wdt_release,
};

static struct miscdevice ixp4xx_wdt_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &ixp4xx_wdt_fops,
};

static int __init ixp4xx_wdt_init(void)
{
	int ret;

	/*
	 * FIXME: we bail out on device tree boot but this really needs
	 * to be fixed in a nicer way: this registers the MDIO bus before
	 * even matching the driver infrastructure, we should only probe
	 * detected hardware.
	 */
	if (of_have_populated_dt())
		return -ENODEV;
	if (!(read_cpuid_id() & 0xf) && !cpu_is_ixp46x()) {
		pr_err("Rev. A0 IXP42x CPU detected - watchdog disabled\n");

		return -ENODEV;
	}
	boot_status = (*IXP4XX_OSST & IXP4XX_OSST_TIMER_WARM_RESET) ?
			WDIOF_CARDRESET : 0;
	ret = misc_register(&ixp4xx_wdt_miscdev);
	if (ret == 0)
		pr_info("timer heartbeat %d sec\n", heartbeat);
	return ret;
}

static void __exit ixp4xx_wdt_exit(void)
{
	misc_deregister(&ixp4xx_wdt_miscdev);
}


module_init(ixp4xx_wdt_init);
module_exit(ixp4xx_wdt_exit);

MODULE_AUTHOR("Deepak Saxena <dsaxena@plexity.net>");
MODULE_DESCRIPTION("IXP4xx Network Processor Watchdog");

module_param(heartbeat, int, 0);
MODULE_PARM_DESC(heartbeat, "Watchdog heartbeat in seconds (default 60s)");

module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started");

MODULE_LICENSE("GPL");
