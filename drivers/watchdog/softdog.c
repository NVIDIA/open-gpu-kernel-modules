// SPDX-License-Identifier: GPL-2.0+
/*
 *	SoftDog:	A Software Watchdog Device
 *
 *	(c) Copyright 1996 Alan Cox <alan@lxorguk.ukuu.org.uk>,
 *							All Rights Reserved.
 *
 *	Neither Alan Cox nor CymruNet Ltd. admit liability nor provide
 *	warranty for any of this software. This material is provided
 *	"AS-IS" and at no charge.
 *
 *	(c) Copyright 1995    Alan Cox <alan@lxorguk.ukuu.org.uk>
 *
 *	Software only watchdog driver. Unlike its big brother the WDT501P
 *	driver this won't always recover a failed machine.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/workqueue.h>

#define TIMER_MARGIN	60		/* Default is 60 seconds */
static unsigned int soft_margin = TIMER_MARGIN;	/* in seconds */
module_param(soft_margin, uint, 0);
MODULE_PARM_DESC(soft_margin,
	"Watchdog soft_margin in seconds. (0 < soft_margin < 65536, default="
					__MODULE_STRING(TIMER_MARGIN) ")");

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		"Watchdog cannot be stopped once started (default="
				__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static int soft_noboot;
module_param(soft_noboot, int, 0);
MODULE_PARM_DESC(soft_noboot,
	"Softdog action, set to 1 to ignore reboots, 0 to reboot (default=0)");

static int soft_panic;
module_param(soft_panic, int, 0);
MODULE_PARM_DESC(soft_panic,
	"Softdog action, set to 1 to panic, 0 to reboot (default=0)");

static char *soft_reboot_cmd;
module_param(soft_reboot_cmd, charp, 0000);
MODULE_PARM_DESC(soft_reboot_cmd,
	"Set reboot command. Emergency reboot takes place if unset");

static bool soft_active_on_boot;
module_param(soft_active_on_boot, bool, 0000);
MODULE_PARM_DESC(soft_active_on_boot,
	"Set to true to active Softdog on boot (default=false)");

static struct hrtimer softdog_ticktock;
static struct hrtimer softdog_preticktock;

static int reboot_kthread_fn(void *data)
{
	kernel_restart(soft_reboot_cmd);
	return -EPERM; /* Should not reach here */
}

static void reboot_work_fn(struct work_struct *unused)
{
	kthread_run(reboot_kthread_fn, NULL, "softdog_reboot");
}

static enum hrtimer_restart softdog_fire(struct hrtimer *timer)
{
	static bool soft_reboot_fired;

	module_put(THIS_MODULE);
	if (soft_noboot) {
		pr_crit("Triggered - Reboot ignored\n");
	} else if (soft_panic) {
		pr_crit("Initiating panic\n");
		panic("Software Watchdog Timer expired");
	} else {
		pr_crit("Initiating system reboot\n");
		if (!soft_reboot_fired && soft_reboot_cmd != NULL) {
			static DECLARE_WORK(reboot_work, reboot_work_fn);
			/*
			 * The 'kernel_restart' is a 'might-sleep' operation.
			 * Also, executing it in system-wide workqueues blocks
			 * any driver from using the same workqueue in its
			 * shutdown callback function. Thus, we should execute
			 * the 'kernel_restart' in a standalone kernel thread.
			 * But since starting a kernel thread is also a
			 * 'might-sleep' operation, so the 'reboot_work' is
			 * required as a launcher of the kernel thread.
			 *
			 * After request the reboot, restart the timer to
			 * schedule an 'emergency_restart' reboot after
			 * 'TIMER_MARGIN' seconds. It's because if the softdog
			 * hangs, it might be because of scheduling issues. And
			 * if that is the case, both 'schedule_work' and
			 * 'kernel_restart' may possibly be malfunctional at the
			 * same time.
			 */
			soft_reboot_fired = true;
			schedule_work(&reboot_work);
			hrtimer_add_expires_ns(timer,
					(u64)TIMER_MARGIN * NSEC_PER_SEC);

			return HRTIMER_RESTART;
		}
		emergency_restart();
		pr_crit("Reboot didn't ?????\n");
	}

	return HRTIMER_NORESTART;
}

static struct watchdog_device softdog_dev;

static enum hrtimer_restart softdog_pretimeout(struct hrtimer *timer)
{
	watchdog_notify_pretimeout(&softdog_dev);

	return HRTIMER_NORESTART;
}

static int softdog_ping(struct watchdog_device *w)
{
	if (!hrtimer_active(&softdog_ticktock))
		__module_get(THIS_MODULE);
	hrtimer_start(&softdog_ticktock, ktime_set(w->timeout, 0),
		      HRTIMER_MODE_REL);

	if (IS_ENABLED(CONFIG_SOFT_WATCHDOG_PRETIMEOUT)) {
		if (w->pretimeout)
			hrtimer_start(&softdog_preticktock,
				      ktime_set(w->timeout - w->pretimeout, 0),
				      HRTIMER_MODE_REL);
		else
			hrtimer_cancel(&softdog_preticktock);
	}

	return 0;
}

static int softdog_stop(struct watchdog_device *w)
{
	if (hrtimer_cancel(&softdog_ticktock))
		module_put(THIS_MODULE);

	if (IS_ENABLED(CONFIG_SOFT_WATCHDOG_PRETIMEOUT))
		hrtimer_cancel(&softdog_preticktock);

	return 0;
}

static struct watchdog_info softdog_info = {
	.identity = "Software Watchdog",
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops softdog_ops = {
	.owner = THIS_MODULE,
	.start = softdog_ping,
	.stop = softdog_stop,
};

static struct watchdog_device softdog_dev = {
	.info = &softdog_info,
	.ops = &softdog_ops,
	.min_timeout = 1,
	.max_timeout = 65535,
	.timeout = TIMER_MARGIN,
};

static int __init softdog_init(void)
{
	int ret;

	watchdog_init_timeout(&softdog_dev, soft_margin, NULL);
	watchdog_set_nowayout(&softdog_dev, nowayout);
	watchdog_stop_on_reboot(&softdog_dev);

	hrtimer_init(&softdog_ticktock, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	softdog_ticktock.function = softdog_fire;

	if (IS_ENABLED(CONFIG_SOFT_WATCHDOG_PRETIMEOUT)) {
		softdog_info.options |= WDIOF_PRETIMEOUT;
		hrtimer_init(&softdog_preticktock, CLOCK_MONOTONIC,
			     HRTIMER_MODE_REL);
		softdog_preticktock.function = softdog_pretimeout;
	}

	if (soft_active_on_boot)
		softdog_ping(&softdog_dev);

	ret = watchdog_register_device(&softdog_dev);
	if (ret)
		return ret;

	pr_info("initialized. soft_noboot=%d soft_margin=%d sec soft_panic=%d (nowayout=%d)\n",
		soft_noboot, softdog_dev.timeout, soft_panic, nowayout);
	pr_info("             soft_reboot_cmd=%s soft_active_on_boot=%d\n",
		soft_reboot_cmd ?: "<not set>", soft_active_on_boot);

	return 0;
}
module_init(softdog_init);

static void __exit softdog_exit(void)
{
	watchdog_unregister_device(&softdog_dev);
}
module_exit(softdog_exit);

MODULE_AUTHOR("Alan Cox");
MODULE_DESCRIPTION("Software Watchdog Device Driver");
MODULE_LICENSE("GPL");
