/*
 * debugfs ops for process ASIDs
 *
 *  Copyright (C) 2000, 2001  Paolo Alberelli
 *  Copyright (C) 2003 - 2008  Paul Mundt
 *  Copyright (C) 2003, 2004  Richard Curnow
 *
 * Provides a debugfs file that lists out the ASIDs currently associated
 * with the processes.
 *
 * In the SH-5 case, if the DM.PC register is examined through the debug
 * link, this shows ASID + PC. To make use of this, the PID->ASID
 * relationship needs to be known. This is primarily for debugging.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>

#include <asm/processor.h>
#include <asm/mmu_context.h>

static int asids_debugfs_show(struct seq_file *file, void *iter)
{
	struct task_struct *p;

	read_lock(&tasklist_lock);

	for_each_process(p) {
		int pid = p->pid;

		if (unlikely(!pid))
			continue;

		if (p->mm)
			seq_printf(file, "%5d : %04lx\n", pid,
				   cpu_asid(smp_processor_id(), p->mm));
	}

	read_unlock(&tasklist_lock);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(asids_debugfs);

static int __init asids_debugfs_init(void)
{
	debugfs_create_file("asids", S_IRUSR, arch_debugfs_dir, NULL,
			    &asids_debugfs_fops);
	return 0;
}
device_initcall(asids_debugfs_init);
