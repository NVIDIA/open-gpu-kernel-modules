// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2010 Google, Inc.
 * Author: Erik Gilling <konkers@android.com>
 *
 * Copyright (C) 2011-2013 NVIDIA Corporation
 */

#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#include <linux/io.h>

#include "dev.h"
#include "debug.h"
#include "channel.h"

static DEFINE_MUTEX(debug_lock);

unsigned int host1x_debug_trace_cmdbuf;

static pid_t host1x_debug_force_timeout_pid;
static u32 host1x_debug_force_timeout_val;
static u32 host1x_debug_force_timeout_channel;

void host1x_debug_output(struct output *o, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(o->buf, sizeof(o->buf), fmt, args);
	va_end(args);

	o->fn(o->ctx, o->buf, len, false);
}

void host1x_debug_cont(struct output *o, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(o->buf, sizeof(o->buf), fmt, args);
	va_end(args);

	o->fn(o->ctx, o->buf, len, true);
}

static int show_channel(struct host1x_channel *ch, void *data, bool show_fifo)
{
	struct host1x *m = dev_get_drvdata(ch->dev->parent);
	struct output *o = data;

	mutex_lock(&ch->cdma.lock);
	mutex_lock(&debug_lock);

	if (show_fifo)
		host1x_hw_show_channel_fifo(m, ch, o);

	host1x_hw_show_channel_cdma(m, ch, o);

	mutex_unlock(&debug_lock);
	mutex_unlock(&ch->cdma.lock);

	return 0;
}

static void show_syncpts(struct host1x *m, struct output *o)
{
	struct list_head *pos;
	unsigned int i;

	host1x_debug_output(o, "---- syncpts ----\n");

	for (i = 0; i < host1x_syncpt_nb_pts(m); i++) {
		u32 max = host1x_syncpt_read_max(m->syncpt + i);
		u32 min = host1x_syncpt_load(m->syncpt + i);
		unsigned int waiters = 0;

		spin_lock(&m->syncpt[i].intr.lock);
		list_for_each(pos, &m->syncpt[i].intr.wait_head)
			waiters++;
		spin_unlock(&m->syncpt[i].intr.lock);

		if (!min && !max && !waiters)
			continue;

		host1x_debug_output(o,
				    "id %u (%s) min %d max %d (%d waiters)\n",
				    i, m->syncpt[i].name, min, max, waiters);
	}

	for (i = 0; i < host1x_syncpt_nb_bases(m); i++) {
		u32 base_val;

		base_val = host1x_syncpt_load_wait_base(m->syncpt + i);
		if (base_val)
			host1x_debug_output(o, "waitbase id %u val %d\n", i,
					    base_val);
	}

	host1x_debug_output(o, "\n");
}

static void show_all(struct host1x *m, struct output *o, bool show_fifo)
{
	unsigned int i;

	host1x_hw_show_mlocks(m, o);
	show_syncpts(m, o);
	host1x_debug_output(o, "---- channels ----\n");

	for (i = 0; i < m->info->nb_channels; ++i) {
		struct host1x_channel *ch = host1x_channel_get_index(m, i);

		if (ch) {
			show_channel(ch, o, show_fifo);
			host1x_channel_put(ch);
		}
	}
}

static int host1x_debug_show_all(struct seq_file *s, void *unused)
{
	struct output o = {
		.fn = write_to_seqfile,
		.ctx = s
	};

	show_all(s->private, &o, true);

	return 0;
}

static int host1x_debug_show(struct seq_file *s, void *unused)
{
	struct output o = {
		.fn = write_to_seqfile,
		.ctx = s
	};

	show_all(s->private, &o, false);

	return 0;
}

static int host1x_debug_open_all(struct inode *inode, struct file *file)
{
	return single_open(file, host1x_debug_show_all, inode->i_private);
}

static const struct file_operations host1x_debug_all_fops = {
	.open = host1x_debug_open_all,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int host1x_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, host1x_debug_show, inode->i_private);
}

static const struct file_operations host1x_debug_fops = {
	.open = host1x_debug_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void host1x_debugfs_init(struct host1x *host1x)
{
	struct dentry *de = debugfs_create_dir("tegra-host1x", NULL);

	/* Store the created entry */
	host1x->debugfs = de;

	debugfs_create_file("status", S_IRUGO, de, host1x, &host1x_debug_fops);
	debugfs_create_file("status_all", S_IRUGO, de, host1x,
			    &host1x_debug_all_fops);

	debugfs_create_u32("trace_cmdbuf", S_IRUGO|S_IWUSR, de,
			   &host1x_debug_trace_cmdbuf);

	host1x_hw_debug_init(host1x, de);

	debugfs_create_u32("force_timeout_pid", S_IRUGO|S_IWUSR, de,
			   &host1x_debug_force_timeout_pid);
	debugfs_create_u32("force_timeout_val", S_IRUGO|S_IWUSR, de,
			   &host1x_debug_force_timeout_val);
	debugfs_create_u32("force_timeout_channel", S_IRUGO|S_IWUSR, de,
			   &host1x_debug_force_timeout_channel);
}

static void host1x_debugfs_exit(struct host1x *host1x)
{
	debugfs_remove_recursive(host1x->debugfs);
}

void host1x_debug_init(struct host1x *host1x)
{
	if (IS_ENABLED(CONFIG_DEBUG_FS))
		host1x_debugfs_init(host1x);
}

void host1x_debug_deinit(struct host1x *host1x)
{
	if (IS_ENABLED(CONFIG_DEBUG_FS))
		host1x_debugfs_exit(host1x);
}

void host1x_debug_dump(struct host1x *host1x)
{
	struct output o = {
		.fn = write_to_printk
	};

	show_all(host1x, &o, true);
}

void host1x_debug_dump_syncpts(struct host1x *host1x)
{
	struct output o = {
		.fn = write_to_printk
	};

	show_syncpts(host1x, &o);
}
