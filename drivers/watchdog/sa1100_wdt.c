// SPDX-License-Identifier: GPL-2.0+
/*
 *	Watchdog driver for the SA11x0/PXA2xx
 *
 *	(c) Copyright 2000 Oleg Drokin <green@crimea.edu>
 *	    Based on SoftDog driver by Alan Cox <alan@lxorguk.ukuu.org.uk>
 *
 *	Neither Oleg Drokin nor iXcelerator.com admit liability nor provide
 *	warranty for any of this software. This material is provided
 *	"AS-IS" and at no charge.
 *
 *	(c) Copyright 2000           Oleg Drokin <green@crimea.edu>
 *
 *	27/11/2000 Initial release
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <linux/timex.h>

#ifdef CONFIG_ARCH_PXA
#include <mach/regs-ost.h>
#endif

#include <mach/reset.h>
#include <mach/hardware.h>

static unsigned long oscr_freq;
static unsigned long sa1100wdt_users;
static unsigned int pre_margin;
static int boot_status;

/*
 *	Allow only one person to hold it open
 */
static int sa1100dog_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(1, &sa1100wdt_users))
		return -EBUSY;

	/* Activate SA1100 Watchdog timer */
	writel_relaxed(readl_relaxed(OSCR) + pre_margin, OSMR3);
	writel_relaxed(OSSR_M3, OSSR);
	writel_relaxed(OWER_WME, OWER);
	writel_relaxed(readl_relaxed(OIER) | OIER_E3, OIER);
	return stream_open(inode, file);
}

/*
 * The watchdog cannot be disabled.
 *
 * Previous comments suggested that turning off the interrupt by
 * clearing OIER[E3] would prevent the watchdog timing out but this
 * does not appear to be true (at least on the PXA255).
 */
static int sa1100dog_release(struct inode *inode, struct file *file)
{
	pr_crit("Device closed - timer will not stop\n");
	clear_bit(1, &sa1100wdt_users);
	return 0;
}

static ssize_t sa1100dog_write(struct file *file, const char __user *data,
						size_t len, loff_t *ppos)
{
	if (len)
		/* Refresh OSMR3 timer. */
		writel_relaxed(readl_relaxed(OSCR) + pre_margin, OSMR3);
	return len;
}

static const struct watchdog_info ident = {
	.options	= WDIOF_CARDRESET | WDIOF_SETTIMEOUT
				| WDIOF_KEEPALIVEPING,
	.identity	= "SA1100/PXA255 Watchdog",
	.firmware_version	= 1,
};

static long sa1100dog_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	int ret = -ENOTTY;
	int time;
	void __user *argp = (void __user *)arg;
	int __user *p = argp;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user(argp, &ident,
				   sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, p);
		break;

	case WDIOC_GETBOOTSTATUS:
		ret = put_user(boot_status, p);
		break;

	case WDIOC_KEEPALIVE:
		writel_relaxed(readl_relaxed(OSCR) + pre_margin, OSMR3);
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(time, p);
		if (ret)
			break;

		if (time <= 0 || (oscr_freq * (long long)time >= 0xffffffff)) {
			ret = -EINVAL;
			break;
		}

		pre_margin = oscr_freq * time;
		writel_relaxed(readl_relaxed(OSCR) + pre_margin, OSMR3);
		fallthrough;

	case WDIOC_GETTIMEOUT:
		ret = put_user(pre_margin / oscr_freq, p);
		break;
	}
	return ret;
}

static const struct file_operations sa1100dog_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= sa1100dog_write,
	.unlocked_ioctl	= sa1100dog_ioctl,
	.compat_ioctl	= compat_ptr_ioctl,
	.open		= sa1100dog_open,
	.release	= sa1100dog_release,
};

static struct miscdevice sa1100dog_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &sa1100dog_fops,
};

static int margin __initdata = 60;		/* (secs) Default is 1 minute */
static struct clk *clk;

static int __init sa1100dog_init(void)
{
	int ret;

	clk = clk_get(NULL, "OSTIMER0");
	if (IS_ERR(clk)) {
		pr_err("SA1100/PXA2xx Watchdog Timer: clock not found: %d\n",
		       (int) PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	ret = clk_prepare_enable(clk);
	if (ret) {
		pr_err("SA1100/PXA2xx Watchdog Timer: clock failed to prepare+enable: %d\n",
		       ret);
		goto err;
	}

	oscr_freq = clk_get_rate(clk);

	/*
	 * Read the reset status, and save it for later.  If
	 * we suspend, RCSR will be cleared, and the watchdog
	 * reset reason will be lost.
	 */
	boot_status = (reset_status & RESET_STATUS_WATCHDOG) ?
				WDIOF_CARDRESET : 0;
	pre_margin = oscr_freq * margin;

	ret = misc_register(&sa1100dog_miscdev);
	if (ret == 0) {
		pr_info("SA1100/PXA2xx Watchdog Timer: timer margin %d sec\n",
			margin);
		return 0;
	}

	clk_disable_unprepare(clk);
err:
	clk_put(clk);
	return ret;
}

static void __exit sa1100dog_exit(void)
{
	misc_deregister(&sa1100dog_miscdev);
	clk_disable_unprepare(clk);
	clk_put(clk);
}

module_init(sa1100dog_init);
module_exit(sa1100dog_exit);

MODULE_AUTHOR("Oleg Drokin <green@crimea.edu>");
MODULE_DESCRIPTION("SA1100/PXA2xx Watchdog");

module_param(margin, int, 0);
MODULE_PARM_DESC(margin, "Watchdog margin in seconds (default 60s)");

MODULE_LICENSE("GPL");
