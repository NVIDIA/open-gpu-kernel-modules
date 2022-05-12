// SPDX-License-Identifier: GPL-2.0-only
/*
 * File: sysctl.c
 *
 * Phonet /proc/sys/net/phonet interface implementation
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * Author: Rémi Denis-Courmont
 */

#include <linux/seqlock.h>
#include <linux/sysctl.h>
#include <linux/errno.h>
#include <linux/init.h>

#include <net/sock.h>
#include <linux/phonet.h>
#include <net/phonet/phonet.h>

#define DYNAMIC_PORT_MIN	0x40
#define DYNAMIC_PORT_MAX	0x7f

static DEFINE_SEQLOCK(local_port_range_lock);
static int local_port_range_min[2] = {0, 0};
static int local_port_range_max[2] = {1023, 1023};
static int local_port_range[2] = {DYNAMIC_PORT_MIN, DYNAMIC_PORT_MAX};
static struct ctl_table_header *phonet_table_hrd;

static void set_local_port_range(int range[2])
{
	write_seqlock(&local_port_range_lock);
	local_port_range[0] = range[0];
	local_port_range[1] = range[1];
	write_sequnlock(&local_port_range_lock);
}

void phonet_get_local_port_range(int *min, int *max)
{
	unsigned int seq;

	do {
		seq = read_seqbegin(&local_port_range_lock);
		if (min)
			*min = local_port_range[0];
		if (max)
			*max = local_port_range[1];
	} while (read_seqretry(&local_port_range_lock, seq));
}

static int proc_local_port_range(struct ctl_table *table, int write,
				 void *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	int range[2] = {local_port_range[0], local_port_range[1]};
	struct ctl_table tmp = {
		.data = &range,
		.maxlen = sizeof(range),
		.mode = table->mode,
		.extra1 = &local_port_range_min,
		.extra2 = &local_port_range_max,
	};

	ret = proc_dointvec_minmax(&tmp, write, buffer, lenp, ppos);

	if (write && ret == 0) {
		if (range[1] < range[0])
			ret = -EINVAL;
		else
			set_local_port_range(range);
	}

	return ret;
}

static struct ctl_table phonet_table[] = {
	{
		.procname	= "local_port_range",
		.data		= &local_port_range,
		.maxlen		= sizeof(local_port_range),
		.mode		= 0644,
		.proc_handler	= proc_local_port_range,
	},
	{ }
};

int __init phonet_sysctl_init(void)
{
	phonet_table_hrd = register_net_sysctl(&init_net, "net/phonet", phonet_table);
	return phonet_table_hrd == NULL ? -ENOMEM : 0;
}

void phonet_sysctl_exit(void)
{
	unregister_net_sysctl_table(phonet_table_hrd);
}
