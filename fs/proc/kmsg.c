// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/proc/kmsg.c
 *
 *  Copyright (C) 1992  by Linus Torvalds
 *
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/syslog.h>

#include <linux/uaccess.h>
#include <asm/io.h>

extern wait_queue_head_t log_wait;

static int kmsg_open(struct inode * inode, struct file * file)
{
	return do_syslog(SYSLOG_ACTION_OPEN, NULL, 0, SYSLOG_FROM_PROC);
}

static int kmsg_release(struct inode * inode, struct file * file)
{
	(void) do_syslog(SYSLOG_ACTION_CLOSE, NULL, 0, SYSLOG_FROM_PROC);
	return 0;
}

static ssize_t kmsg_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	if ((file->f_flags & O_NONBLOCK) &&
	    !do_syslog(SYSLOG_ACTION_SIZE_UNREAD, NULL, 0, SYSLOG_FROM_PROC))
		return -EAGAIN;
	return do_syslog(SYSLOG_ACTION_READ, buf, count, SYSLOG_FROM_PROC);
}

static __poll_t kmsg_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &log_wait, wait);
	if (do_syslog(SYSLOG_ACTION_SIZE_UNREAD, NULL, 0, SYSLOG_FROM_PROC))
		return EPOLLIN | EPOLLRDNORM;
	return 0;
}


static const struct proc_ops kmsg_proc_ops = {
	.proc_flags	= PROC_ENTRY_PERMANENT,
	.proc_read	= kmsg_read,
	.proc_poll	= kmsg_poll,
	.proc_open	= kmsg_open,
	.proc_release	= kmsg_release,
	.proc_lseek	= generic_file_llseek,
};

static int __init proc_kmsg_init(void)
{
	proc_create("kmsg", S_IRUSR, NULL, &kmsg_proc_ops);
	return 0;
}
fs_initcall(proc_kmsg_init);
