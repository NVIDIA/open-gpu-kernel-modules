// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  c 2001 PPC 64 Team, IBM Corp
 *
 * scan-log-data driver for PPC64  Todd Inglett <tinglett@vnet.ibm.com>
 *
 * When ppc64 hardware fails the service processor dumps internal state
 * of the system.  After a reboot the operating system can access a dump
 * of this data using this driver.  A dump exists if the device-tree
 * /chosen/ibm,scan-log-data property exists.
 *
 * This driver exports /proc/powerpc/scan-log-dump which can be read.
 * The driver supports only sequential reads.
 *
 * The driver looks at a write to the driver for the single word "reset".
 * If given, the driver will reset the scanlog so the platform can free it.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/rtas.h>
#include <asm/prom.h>

#define MODULE_VERS "1.0"
#define MODULE_NAME "scanlog"

/* Status returns from ibm,scan-log-dump */
#define SCANLOG_COMPLETE 0
#define SCANLOG_HWERROR -1
#define SCANLOG_CONTINUE 1


static unsigned int ibm_scan_log_dump;			/* RTAS token */
static unsigned int *scanlog_buffer;			/* The data buffer */

static ssize_t scanlog_read(struct file *file, char __user *buf,
			    size_t count, loff_t *ppos)
{
	unsigned int *data = scanlog_buffer;
	int status;
	unsigned long len, off;
	unsigned int wait_time;

	if (count > RTAS_DATA_BUF_SIZE)
		count = RTAS_DATA_BUF_SIZE;

	if (count < 1024) {
		/* This is the min supported by this RTAS call.  Rather
		 * than do all the buffering we insist the user code handle
		 * larger reads.  As long as cp works... :)
		 */
		printk(KERN_ERR "scanlog: cannot perform a small read (%ld)\n", count);
		return -EINVAL;
	}

	if (!access_ok(buf, count))
		return -EFAULT;

	for (;;) {
		wait_time = 500;	/* default wait if no data */
		spin_lock(&rtas_data_buf_lock);
		memcpy(rtas_data_buf, data, RTAS_DATA_BUF_SIZE);
		status = rtas_call(ibm_scan_log_dump, 2, 1, NULL,
				   (u32) __pa(rtas_data_buf), (u32) count);
		memcpy(data, rtas_data_buf, RTAS_DATA_BUF_SIZE);
		spin_unlock(&rtas_data_buf_lock);

		pr_debug("scanlog: status=%d, data[0]=%x, data[1]=%x, " \
			 "data[2]=%x\n", status, data[0], data[1], data[2]);
		switch (status) {
		    case SCANLOG_COMPLETE:
			pr_debug("scanlog: hit eof\n");
			return 0;
		    case SCANLOG_HWERROR:
			pr_debug("scanlog: hardware error reading data\n");
			return -EIO;
		    case SCANLOG_CONTINUE:
			/* We may or may not have data yet */
			len = data[1];
			off = data[2];
			if (len > 0) {
				if (copy_to_user(buf, ((char *)data)+off, len))
					return -EFAULT;
				return len;
			}
			/* Break to sleep default time */
			break;
		    default:
			/* Assume extended busy */
			wait_time = rtas_busy_delay_time(status);
			if (!wait_time) {
				printk(KERN_ERR "scanlog: unknown error " \
				       "from rtas: %d\n", status);
				return -EIO;
			}
		}
		/* Apparently no data yet.  Wait and try again. */
		msleep_interruptible(wait_time);
	}
	/*NOTREACHED*/
}

static ssize_t scanlog_write(struct file * file, const char __user * buf,
			     size_t count, loff_t *ppos)
{
	char stkbuf[20];
	int status;

	if (count > 19) count = 19;
	if (copy_from_user (stkbuf, buf, count)) {
		return -EFAULT;
	}
	stkbuf[count] = 0;

	if (buf) {
		if (strncmp(stkbuf, "reset", 5) == 0) {
			pr_debug("scanlog: reset scanlog\n");
			status = rtas_call(ibm_scan_log_dump, 2, 1, NULL, 0, 0);
			pr_debug("scanlog: rtas returns %d\n", status);
		}
	}
	return count;
}

static int scanlog_open(struct inode * inode, struct file * file)
{
	unsigned int *data = scanlog_buffer;

	if (data[0] != 0) {
		/* This imperfect test stops a second copy of the
		 * data (or a reset while data is being copied)
		 */
		return -EBUSY;
	}

	data[0] = 0;	/* re-init so we restart the scan */

	return 0;
}

static int scanlog_release(struct inode * inode, struct file * file)
{
	unsigned int *data = scanlog_buffer;

	data[0] = 0;
	return 0;
}

static const struct proc_ops scanlog_proc_ops = {
	.proc_read	= scanlog_read,
	.proc_write	= scanlog_write,
	.proc_open	= scanlog_open,
	.proc_release	= scanlog_release,
	.proc_lseek	= noop_llseek,
};

static int __init scanlog_init(void)
{
	struct proc_dir_entry *ent;
	int err = -ENOMEM;

	ibm_scan_log_dump = rtas_token("ibm,scan-log-dump");
	if (ibm_scan_log_dump == RTAS_UNKNOWN_SERVICE)
		return -ENODEV;

	/* Ideally we could allocate a buffer < 4G */
	scanlog_buffer = kzalloc(RTAS_DATA_BUF_SIZE, GFP_KERNEL);
	if (!scanlog_buffer)
		goto err;

	ent = proc_create("powerpc/rtas/scan-log-dump", 0400, NULL,
			  &scanlog_proc_ops);
	if (!ent)
		goto err;
	return 0;
err:
	kfree(scanlog_buffer);
	return err;
}

static void __exit scanlog_cleanup(void)
{
	remove_proc_entry("powerpc/rtas/scan-log-dump", NULL);
	kfree(scanlog_buffer);
}

module_init(scanlog_init);
module_exit(scanlog_cleanup);
MODULE_LICENSE("GPL");
