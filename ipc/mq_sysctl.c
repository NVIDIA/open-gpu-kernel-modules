// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2007 IBM Corporation
 *
 *  Author: Cedric Le Goater <clg@fr.ibm.com>
 */

#include <linux/nsproxy.h>
#include <linux/ipc_namespace.h>
#include <linux/sysctl.h>

#ifdef CONFIG_PROC_SYSCTL
static void *get_mq(struct ctl_table *table)
{
	char *which = table->data;
	struct ipc_namespace *ipc_ns = current->nsproxy->ipc_ns;
	which = (which - (char *)&init_ipc_ns) + (char *)ipc_ns;
	return which;
}

static int proc_mq_dointvec(struct ctl_table *table, int write,
			    void *buffer, size_t *lenp, loff_t *ppos)
{
	struct ctl_table mq_table;
	memcpy(&mq_table, table, sizeof(mq_table));
	mq_table.data = get_mq(table);

	return proc_dointvec(&mq_table, write, buffer, lenp, ppos);
}

static int proc_mq_dointvec_minmax(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos)
{
	struct ctl_table mq_table;
	memcpy(&mq_table, table, sizeof(mq_table));
	mq_table.data = get_mq(table);

	return proc_dointvec_minmax(&mq_table, write, buffer,
					lenp, ppos);
}
#else
#define proc_mq_dointvec NULL
#define proc_mq_dointvec_minmax NULL
#endif

static int msg_max_limit_min = MIN_MSGMAX;
static int msg_max_limit_max = HARD_MSGMAX;

static int msg_maxsize_limit_min = MIN_MSGSIZEMAX;
static int msg_maxsize_limit_max = HARD_MSGSIZEMAX;

static struct ctl_table mq_sysctls[] = {
	{
		.procname	= "queues_max",
		.data		= &init_ipc_ns.mq_queues_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_mq_dointvec,
	},
	{
		.procname	= "msg_max",
		.data		= &init_ipc_ns.mq_msg_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_mq_dointvec_minmax,
		.extra1		= &msg_max_limit_min,
		.extra2		= &msg_max_limit_max,
	},
	{
		.procname	= "msgsize_max",
		.data		= &init_ipc_ns.mq_msgsize_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_mq_dointvec_minmax,
		.extra1		= &msg_maxsize_limit_min,
		.extra2		= &msg_maxsize_limit_max,
	},
	{
		.procname	= "msg_default",
		.data		= &init_ipc_ns.mq_msg_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_mq_dointvec_minmax,
		.extra1		= &msg_max_limit_min,
		.extra2		= &msg_max_limit_max,
	},
	{
		.procname	= "msgsize_default",
		.data		= &init_ipc_ns.mq_msgsize_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_mq_dointvec_minmax,
		.extra1		= &msg_maxsize_limit_min,
		.extra2		= &msg_maxsize_limit_max,
	},
	{}
};

static struct ctl_table mq_sysctl_dir[] = {
	{
		.procname	= "mqueue",
		.mode		= 0555,
		.child		= mq_sysctls,
	},
	{}
};

static struct ctl_table mq_sysctl_root[] = {
	{
		.procname	= "fs",
		.mode		= 0555,
		.child		= mq_sysctl_dir,
	},
	{}
};

struct ctl_table_header *mq_register_sysctl_table(void)
{
	return register_sysctl_table(mq_sysctl_root);
}
