// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spu aware cpufreq governor for the cell processor
 *
 * © Copyright IBM Corporation 2006-2008
 *
 * Author: Christian Krafft <krafft@de.ibm.com>
 */

#include <linux/cpufreq.h>
#include <linux/sched.h>
#include <linux/sched/loadavg.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/atomic.h>
#include <asm/machdep.h>
#include <asm/spu.h>

#define POLL_TIME	100000		/* in µs */
#define EXP		753		/* exp(-1) in fixed-point */

struct spu_gov_info_struct {
	unsigned long busy_spus;	/* fixed-point */
	struct cpufreq_policy *policy;
	struct delayed_work work;
	unsigned int poll_int;		/* µs */
};
static DEFINE_PER_CPU(struct spu_gov_info_struct, spu_gov_info);

static int calc_freq(struct spu_gov_info_struct *info)
{
	int cpu;
	int busy_spus;

	cpu = info->policy->cpu;
	busy_spus = atomic_read(&cbe_spu_info[cpu_to_node(cpu)].busy_spus);

	info->busy_spus = calc_load(info->busy_spus, EXP, busy_spus * FIXED_1);
	pr_debug("cpu %d: busy_spus=%d, info->busy_spus=%ld\n",
			cpu, busy_spus, info->busy_spus);

	return info->policy->max * info->busy_spus / FIXED_1;
}

static void spu_gov_work(struct work_struct *work)
{
	struct spu_gov_info_struct *info;
	int delay;
	unsigned long target_freq;

	info = container_of(work, struct spu_gov_info_struct, work.work);

	/* after cancel_delayed_work_sync we unset info->policy */
	BUG_ON(info->policy == NULL);

	target_freq = calc_freq(info);
	__cpufreq_driver_target(info->policy, target_freq, CPUFREQ_RELATION_H);

	delay = usecs_to_jiffies(info->poll_int);
	schedule_delayed_work_on(info->policy->cpu, &info->work, delay);
}

static void spu_gov_init_work(struct spu_gov_info_struct *info)
{
	int delay = usecs_to_jiffies(info->poll_int);
	INIT_DEFERRABLE_WORK(&info->work, spu_gov_work);
	schedule_delayed_work_on(info->policy->cpu, &info->work, delay);
}

static void spu_gov_cancel_work(struct spu_gov_info_struct *info)
{
	cancel_delayed_work_sync(&info->work);
}

static int spu_gov_start(struct cpufreq_policy *policy)
{
	unsigned int cpu = policy->cpu;
	struct spu_gov_info_struct *info = &per_cpu(spu_gov_info, cpu);
	struct spu_gov_info_struct *affected_info;
	int i;

	if (!cpu_online(cpu)) {
		printk(KERN_ERR "cpu %d is not online\n", cpu);
		return -EINVAL;
	}

	if (!policy->cur) {
		printk(KERN_ERR "no cpu specified in policy\n");
		return -EINVAL;
	}

	/* initialize spu_gov_info for all affected cpus */
	for_each_cpu(i, policy->cpus) {
		affected_info = &per_cpu(spu_gov_info, i);
		affected_info->policy = policy;
	}

	info->poll_int = POLL_TIME;

	/* setup timer */
	spu_gov_init_work(info);

	return 0;
}

static void spu_gov_stop(struct cpufreq_policy *policy)
{
	unsigned int cpu = policy->cpu;
	struct spu_gov_info_struct *info = &per_cpu(spu_gov_info, cpu);
	int i;

	/* cancel timer */
	spu_gov_cancel_work(info);

	/* clean spu_gov_info for all affected cpus */
	for_each_cpu (i, policy->cpus) {
		info = &per_cpu(spu_gov_info, i);
		info->policy = NULL;
	}
}

static struct cpufreq_governor spu_governor = {
	.name = "spudemand",
	.start = spu_gov_start,
	.stop = spu_gov_stop,
	.owner = THIS_MODULE,
};
cpufreq_governor_init(spu_governor);
cpufreq_governor_exit(spu_governor);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Krafft <krafft@de.ibm.com>");
