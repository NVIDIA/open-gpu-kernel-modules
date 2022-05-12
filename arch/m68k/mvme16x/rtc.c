// SPDX-License-Identifier: GPL-2.0
/*
 *	Real Time Clock interface for Linux on the MVME16x
 *
 * Based on the PC driver by Paul Gortmaker.
 */

#define RTC_VERSION		"1.00"

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/capability.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/rtc.h>	/* For struct rtc_time and ioctls, etc */
#include <linux/bcd.h>
#include <asm/mvme16xhw.h>

#include <asm/io.h>
#include <linux/uaccess.h>
#include <asm/setup.h>

/*
 *	We sponge a minor off of the misc major. No need slurping
 *	up another valuable major dev number for this. If you add
 *	an ioctl, make sure you don't conflict with SPARC's RTC
 *	ioctls.
 */

static const unsigned char days_in_mo[] =
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static atomic_t rtc_ready = ATOMIC_INIT(1);

static long rtc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	volatile MK48T08ptr_t rtc = (MK48T08ptr_t)MVME_RTC_BASE;
	unsigned long flags;
	struct rtc_time wtime;
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case RTC_RD_TIME:	/* Read the time/date from RTC	*/
	{
		local_irq_save(flags);
		/* Ensure clock and real-time-mode-register are accessible */
		rtc->ctrl = RTC_READ;
		memset(&wtime, 0, sizeof(struct rtc_time));
		wtime.tm_sec =  bcd2bin(rtc->bcd_sec);
		wtime.tm_min =  bcd2bin(rtc->bcd_min);
		wtime.tm_hour = bcd2bin(rtc->bcd_hr);
		wtime.tm_mday =  bcd2bin(rtc->bcd_dom);
		wtime.tm_mon =  bcd2bin(rtc->bcd_mth)-1;
		wtime.tm_year = bcd2bin(rtc->bcd_year);
		if (wtime.tm_year < 70)
			wtime.tm_year += 100;
		wtime.tm_wday = bcd2bin(rtc->bcd_dow)-1;
		rtc->ctrl = 0;
		local_irq_restore(flags);
		return copy_to_user(argp, &wtime, sizeof wtime) ?
								-EFAULT : 0;
	}
	case RTC_SET_TIME:	/* Set the RTC */
	{
		struct rtc_time rtc_tm;
		unsigned char mon, day, hrs, min, sec, leap_yr;
		unsigned int yrs;

		if (!capable(CAP_SYS_ADMIN))
			return -EACCES;

		if (copy_from_user(&rtc_tm, argp, sizeof(struct rtc_time)))
			return -EFAULT;

		yrs = rtc_tm.tm_year;
		if (yrs < 1900)
			yrs += 1900;
		mon = rtc_tm.tm_mon + 1;   /* tm_mon starts at zero */
		day = rtc_tm.tm_mday;
		hrs = rtc_tm.tm_hour;
		min = rtc_tm.tm_min;
		sec = rtc_tm.tm_sec;

		leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));

		if ((mon > 12) || (day == 0))
			return -EINVAL;

		if (day > (days_in_mo[mon] + ((mon == 2) && leap_yr)))
			return -EINVAL;

		if ((hrs >= 24) || (min >= 60) || (sec >= 60))
			return -EINVAL;

		if (yrs >= 2070)
			return -EINVAL;

		local_irq_save(flags);
		rtc->ctrl     = RTC_WRITE;

		rtc->bcd_sec  = bin2bcd(sec);
		rtc->bcd_min  = bin2bcd(min);
		rtc->bcd_hr   = bin2bcd(hrs);
		rtc->bcd_dom  = bin2bcd(day);
		rtc->bcd_mth  = bin2bcd(mon);
		rtc->bcd_year = bin2bcd(yrs%100);

		rtc->ctrl     = 0;
		local_irq_restore(flags);
		return 0;
	}
	default:
		return -EINVAL;
	}
}

/*
 * We enforce only one user at a time here with the open/close.
 */
static int rtc_open(struct inode *inode, struct file *file)
{
	if( !atomic_dec_and_test(&rtc_ready) )
	{
		atomic_inc( &rtc_ready );
		return -EBUSY;
	}
	return 0;
}

static int rtc_release(struct inode *inode, struct file *file)
{
	atomic_inc( &rtc_ready );
	return 0;
}

/*
 *	The various file operations we support.
 */

static const struct file_operations rtc_fops = {
	.unlocked_ioctl	= rtc_ioctl,
	.open		= rtc_open,
	.release	= rtc_release,
	.llseek		= noop_llseek,
};

static struct miscdevice rtc_dev=
{
	.minor =	RTC_MINOR,
	.name =		"rtc",
	.fops =		&rtc_fops
};

static int __init rtc_MK48T08_init(void)
{
	if (!MACH_IS_MVME16x)
		return -ENODEV;

	pr_info("MK48T08 Real Time Clock Driver v%s\n", RTC_VERSION);
	return misc_register(&rtc_dev);
}
device_initcall(rtc_MK48T08_init);
