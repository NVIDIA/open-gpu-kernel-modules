// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	SBC EPX C3 0.1	A Hardware Watchdog Device for the Winsystems EPX-C3
 *	single board computer
 *
 *	(c) Copyright 2006 Calin A. Culianu <calin@ajvar.org>, All Rights
 *	Reserved.
 *
 *	based on softdog.c by Alan Cox <alan@lxorguk.ukuu.org.uk>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/io.h>

static int epx_c3_alive;

#define WATCHDOG_TIMEOUT 1		/* 1 sec default timeout */

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
					__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

#define EPXC3_WATCHDOG_CTL_REG 0x1ee /* write 1 to enable, 0 to disable */
#define EPXC3_WATCHDOG_PET_REG 0x1ef /* write anything to pet once enabled */

static void epx_c3_start(void)
{
	outb(1, EPXC3_WATCHDOG_CTL_REG);
}

static void epx_c3_stop(void)
{

	outb(0, EPXC3_WATCHDOG_CTL_REG);

	pr_info("Stopped watchdog timer\n");
}

static void epx_c3_pet(void)
{
	outb(1, EPXC3_WATCHDOG_PET_REG);
}

/*
 *	Allow only one person to hold it open
 */
static int epx_c3_open(struct inode *inode, struct file *file)
{
	if (epx_c3_alive)
		return -EBUSY;

	if (nowayout)
		__module_get(THIS_MODULE);

	/* Activate timer */
	epx_c3_start();
	epx_c3_pet();

	epx_c3_alive = 1;
	pr_info("Started watchdog timer\n");

	return stream_open(inode, file);
}

static int epx_c3_release(struct inode *inode, struct file *file)
{
	/* Shut off the timer.
	 * Lock it in if it's a module and we defined ...NOWAYOUT */
	if (!nowayout)
		epx_c3_stop();		/* Turn the WDT off */

	epx_c3_alive = 0;

	return 0;
}

static ssize_t epx_c3_write(struct file *file, const char __user *data,
			size_t len, loff_t *ppos)
{
	/* Refresh the timer. */
	if (len)
		epx_c3_pet();
	return len;
}

static long epx_c3_ioctl(struct file *file, unsigned int cmd,
						unsigned long arg)
{
	int options, retval = -EINVAL;
	int __user *argp = (void __user *)arg;
	static const struct watchdog_info ident = {
		.options		= WDIOF_KEEPALIVEPING,
		.firmware_version	= 0,
		.identity		= "Winsystems EPX-C3 H/W Watchdog",
	};

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user(argp, &ident, sizeof(ident)))
			return -EFAULT;
		return 0;
	case WDIOC_GETSTATUS:
	case WDIOC_GETBOOTSTATUS:
		return put_user(0, argp);
	case WDIOC_SETOPTIONS:
		if (get_user(options, argp))
			return -EFAULT;

		if (options & WDIOS_DISABLECARD) {
			epx_c3_stop();
			retval = 0;
		}

		if (options & WDIOS_ENABLECARD) {
			epx_c3_start();
			retval = 0;
		}

		return retval;
	case WDIOC_KEEPALIVE:
		epx_c3_pet();
		return 0;
	case WDIOC_GETTIMEOUT:
		return put_user(WATCHDOG_TIMEOUT, argp);
	default:
		return -ENOTTY;
	}
}

static int epx_c3_notify_sys(struct notifier_block *this, unsigned long code,
				void *unused)
{
	if (code == SYS_DOWN || code == SYS_HALT)
		epx_c3_stop();		/* Turn the WDT off */

	return NOTIFY_DONE;
}

static const struct file_operations epx_c3_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= epx_c3_write,
	.unlocked_ioctl	= epx_c3_ioctl,
	.compat_ioctl	= compat_ptr_ioctl,
	.open		= epx_c3_open,
	.release	= epx_c3_release,
};

static struct miscdevice epx_c3_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &epx_c3_fops,
};

static struct notifier_block epx_c3_notifier = {
	.notifier_call = epx_c3_notify_sys,
};

static int __init watchdog_init(void)
{
	int ret;

	if (!request_region(EPXC3_WATCHDOG_CTL_REG, 2, "epxc3_watchdog"))
		return -EBUSY;

	ret = register_reboot_notifier(&epx_c3_notifier);
	if (ret) {
		pr_err("cannot register reboot notifier (err=%d)\n", ret);
		goto out;
	}

	ret = misc_register(&epx_c3_miscdev);
	if (ret) {
		pr_err("cannot register miscdev on minor=%d (err=%d)\n",
		       WATCHDOG_MINOR, ret);
		unregister_reboot_notifier(&epx_c3_notifier);
		goto out;
	}

	pr_info("Hardware Watchdog Timer for Winsystems EPX-C3 SBC: 0.1\n");

	return 0;

out:
	release_region(EPXC3_WATCHDOG_CTL_REG, 2);
	return ret;
}

static void __exit watchdog_exit(void)
{
	misc_deregister(&epx_c3_miscdev);
	unregister_reboot_notifier(&epx_c3_notifier);
	release_region(EPXC3_WATCHDOG_CTL_REG, 2);
}

module_init(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("Calin A. Culianu <calin@ajvar.org>");
MODULE_DESCRIPTION("Hardware Watchdog Device for Winsystems EPX-C3 SBC.  "
	"Note that there is no way to probe for this device -- "
	"so only use it if you are *sure* you are running on this specific "
	"SBC system from Winsystems!  It writes to IO ports 0x1ee and 0x1ef!");
MODULE_LICENSE("GPL");
