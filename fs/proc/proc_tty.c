// SPDX-License-Identifier: GPL-2.0
/*
 * proc_tty.c -- handles /proc/tty
 *
 * Copyright 1997, Theodore Ts'o
 */

#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/tty.h>
#include <linux/seq_file.h>
#include <linux/bitops.h>
#include "internal.h"

/*
 * The /proc/tty directory inodes...
 */
static struct proc_dir_entry *proc_tty_driver;

/*
 * This is the handler for /proc/tty/drivers
 */
static void show_tty_range(struct seq_file *m, struct tty_driver *p,
	dev_t from, int num)
{
	seq_printf(m, "%-20s ", p->driver_name ? p->driver_name : "unknown");
	seq_printf(m, "/dev/%-8s ", p->name);
	if (p->num > 1) {
		seq_printf(m, "%3d %d-%d ", MAJOR(from), MINOR(from),
			MINOR(from) + num - 1);
	} else {
		seq_printf(m, "%3d %7d ", MAJOR(from), MINOR(from));
	}
	switch (p->type) {
	case TTY_DRIVER_TYPE_SYSTEM:
		seq_puts(m, "system");
		if (p->subtype == SYSTEM_TYPE_TTY)
			seq_puts(m, ":/dev/tty");
		else if (p->subtype == SYSTEM_TYPE_SYSCONS)
			seq_puts(m, ":console");
		else if (p->subtype == SYSTEM_TYPE_CONSOLE)
			seq_puts(m, ":vtmaster");
		break;
	case TTY_DRIVER_TYPE_CONSOLE:
		seq_puts(m, "console");
		break;
	case TTY_DRIVER_TYPE_SERIAL:
		seq_puts(m, "serial");
		break;
	case TTY_DRIVER_TYPE_PTY:
		if (p->subtype == PTY_TYPE_MASTER)
			seq_puts(m, "pty:master");
		else if (p->subtype == PTY_TYPE_SLAVE)
			seq_puts(m, "pty:slave");
		else
			seq_puts(m, "pty");
		break;
	default:
		seq_printf(m, "type:%d.%d", p->type, p->subtype);
	}
	seq_putc(m, '\n');
}

static int show_tty_driver(struct seq_file *m, void *v)
{
	struct tty_driver *p = list_entry(v, struct tty_driver, tty_drivers);
	dev_t from = MKDEV(p->major, p->minor_start);
	dev_t to = from + p->num;

	if (&p->tty_drivers == tty_drivers.next) {
		/* pseudo-drivers first */
		seq_printf(m, "%-20s /dev/%-8s ", "/dev/tty", "tty");
		seq_printf(m, "%3d %7d ", TTYAUX_MAJOR, 0);
		seq_puts(m, "system:/dev/tty\n");
		seq_printf(m, "%-20s /dev/%-8s ", "/dev/console", "console");
		seq_printf(m, "%3d %7d ", TTYAUX_MAJOR, 1);
		seq_puts(m, "system:console\n");
#ifdef CONFIG_UNIX98_PTYS
		seq_printf(m, "%-20s /dev/%-8s ", "/dev/ptmx", "ptmx");
		seq_printf(m, "%3d %7d ", TTYAUX_MAJOR, 2);
		seq_puts(m, "system\n");
#endif
#ifdef CONFIG_VT
		seq_printf(m, "%-20s /dev/%-8s ", "/dev/vc/0", "vc/0");
		seq_printf(m, "%3d %7d ", TTY_MAJOR, 0);
		seq_puts(m, "system:vtmaster\n");
#endif
	}

	while (MAJOR(from) < MAJOR(to)) {
		dev_t next = MKDEV(MAJOR(from)+1, 0);
		show_tty_range(m, p, from, next - from);
		from = next;
	}
	if (from != to)
		show_tty_range(m, p, from, to - from);
	return 0;
}

/* iterator */
static void *t_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&tty_mutex);
	return seq_list_start(&tty_drivers, *pos);
}

static void *t_next(struct seq_file *m, void *v, loff_t *pos)
{
	return seq_list_next(v, &tty_drivers, pos);
}

static void t_stop(struct seq_file *m, void *v)
{
	mutex_unlock(&tty_mutex);
}

static const struct seq_operations tty_drivers_op = {
	.start	= t_start,
	.next	= t_next,
	.stop	= t_stop,
	.show	= show_tty_driver
};

/*
 * This function is called by tty_register_driver() to handle
 * registering the driver's /proc handler into /proc/tty/driver/<foo>
 */
void proc_tty_register_driver(struct tty_driver *driver)
{
	struct proc_dir_entry *ent;
		
	if (!driver->driver_name || driver->proc_entry ||
	    !driver->ops->proc_show)
		return;

	ent = proc_create_single_data(driver->driver_name, 0, proc_tty_driver,
			       driver->ops->proc_show, driver);
	driver->proc_entry = ent;
}

/*
 * This function is called by tty_unregister_driver()
 */
void proc_tty_unregister_driver(struct tty_driver *driver)
{
	struct proc_dir_entry *ent;

	ent = driver->proc_entry;
	if (!ent)
		return;
		
	remove_proc_entry(ent->name, proc_tty_driver);
	
	driver->proc_entry = NULL;
}

/*
 * Called by proc_root_init() to initialize the /proc/tty subtree
 */
void __init proc_tty_init(void)
{
	if (!proc_mkdir("tty", NULL))
		return;
	proc_mkdir("tty/ldisc", NULL);	/* Preserved: it's userspace visible */
	/*
	 * /proc/tty/driver/serial reveals the exact character counts for
	 * serial links which is just too easy to abuse for inferring
	 * password lengths and inter-keystroke timings during password
	 * entry.
	 */
	proc_tty_driver = proc_mkdir_mode("tty/driver", S_IRUSR|S_IXUSR, NULL);
	proc_create_seq("tty/ldiscs", 0, NULL, &tty_ldiscs_seq_ops);
	proc_create_seq("tty/drivers", 0, NULL, &tty_drivers_op);
}
