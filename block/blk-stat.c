// SPDX-License-Identifier: GPL-2.0
/*
 * Block stat tracking code
 *
 * Copyright (C) 2016 Jens Axboe
 */
#include <linux/kernel.h>
#include <linux/rculist.h>
#include <linux/blk-mq.h>

#include "blk-stat.h"
#include "blk-mq.h"
#include "blk.h"

struct blk_queue_stats {
	struct list_head callbacks;
	spinlock_t lock;
	bool enable_accounting;
};

void blk_rq_stat_init(struct blk_rq_stat *stat)
{
	stat->min = -1ULL;
	stat->max = stat->nr_samples = stat->mean = 0;
	stat->batch = 0;
}

/* src is a per-cpu stat, mean isn't initialized */
void blk_rq_stat_sum(struct blk_rq_stat *dst, struct blk_rq_stat *src)
{
	if (!src->nr_samples)
		return;

	dst->min = min(dst->min, src->min);
	dst->max = max(dst->max, src->max);

	dst->mean = div_u64(src->batch + dst->mean * dst->nr_samples,
				dst->nr_samples + src->nr_samples);

	dst->nr_samples += src->nr_samples;
}

void blk_rq_stat_add(struct blk_rq_stat *stat, u64 value)
{
	stat->min = min(stat->min, value);
	stat->max = max(stat->max, value);
	stat->batch += value;
	stat->nr_samples++;
}

void blk_stat_add(struct request *rq, u64 now)
{
	struct request_queue *q = rq->q;
	struct blk_stat_callback *cb;
	struct blk_rq_stat *stat;
	int bucket, cpu;
	u64 value;

	value = (now >= rq->io_start_time_ns) ? now - rq->io_start_time_ns : 0;

	blk_throtl_stat_add(rq, value);

	rcu_read_lock();
	cpu = get_cpu();
	list_for_each_entry_rcu(cb, &q->stats->callbacks, list) {
		if (!blk_stat_is_active(cb))
			continue;

		bucket = cb->bucket_fn(rq);
		if (bucket < 0)
			continue;

		stat = &per_cpu_ptr(cb->cpu_stat, cpu)[bucket];
		blk_rq_stat_add(stat, value);
	}
	put_cpu();
	rcu_read_unlock();
}

static void blk_stat_timer_fn(struct timer_list *t)
{
	struct blk_stat_callback *cb = from_timer(cb, t, timer);
	unsigned int bucket;
	int cpu;

	for (bucket = 0; bucket < cb->buckets; bucket++)
		blk_rq_stat_init(&cb->stat[bucket]);

	for_each_online_cpu(cpu) {
		struct blk_rq_stat *cpu_stat;

		cpu_stat = per_cpu_ptr(cb->cpu_stat, cpu);
		for (bucket = 0; bucket < cb->buckets; bucket++) {
			blk_rq_stat_sum(&cb->stat[bucket], &cpu_stat[bucket]);
			blk_rq_stat_init(&cpu_stat[bucket]);
		}
	}

	cb->timer_fn(cb);
}

struct blk_stat_callback *
blk_stat_alloc_callback(void (*timer_fn)(struct blk_stat_callback *),
			int (*bucket_fn)(const struct request *),
			unsigned int buckets, void *data)
{
	struct blk_stat_callback *cb;

	cb = kmalloc(sizeof(*cb), GFP_KERNEL);
	if (!cb)
		return NULL;

	cb->stat = kmalloc_array(buckets, sizeof(struct blk_rq_stat),
				 GFP_KERNEL);
	if (!cb->stat) {
		kfree(cb);
		return NULL;
	}
	cb->cpu_stat = __alloc_percpu(buckets * sizeof(struct blk_rq_stat),
				      __alignof__(struct blk_rq_stat));
	if (!cb->cpu_stat) {
		kfree(cb->stat);
		kfree(cb);
		return NULL;
	}

	cb->timer_fn = timer_fn;
	cb->bucket_fn = bucket_fn;
	cb->data = data;
	cb->buckets = buckets;
	timer_setup(&cb->timer, blk_stat_timer_fn, 0);

	return cb;
}

void blk_stat_add_callback(struct request_queue *q,
			   struct blk_stat_callback *cb)
{
	unsigned int bucket;
	unsigned long flags;
	int cpu;

	for_each_possible_cpu(cpu) {
		struct blk_rq_stat *cpu_stat;

		cpu_stat = per_cpu_ptr(cb->cpu_stat, cpu);
		for (bucket = 0; bucket < cb->buckets; bucket++)
			blk_rq_stat_init(&cpu_stat[bucket]);
	}

	spin_lock_irqsave(&q->stats->lock, flags);
	list_add_tail_rcu(&cb->list, &q->stats->callbacks);
	blk_queue_flag_set(QUEUE_FLAG_STATS, q);
	spin_unlock_irqrestore(&q->stats->lock, flags);
}

void blk_stat_remove_callback(struct request_queue *q,
			      struct blk_stat_callback *cb)
{
	unsigned long flags;

	spin_lock_irqsave(&q->stats->lock, flags);
	list_del_rcu(&cb->list);
	if (list_empty(&q->stats->callbacks) && !q->stats->enable_accounting)
		blk_queue_flag_clear(QUEUE_FLAG_STATS, q);
	spin_unlock_irqrestore(&q->stats->lock, flags);

	del_timer_sync(&cb->timer);
}

static void blk_stat_free_callback_rcu(struct rcu_head *head)
{
	struct blk_stat_callback *cb;

	cb = container_of(head, struct blk_stat_callback, rcu);
	free_percpu(cb->cpu_stat);
	kfree(cb->stat);
	kfree(cb);
}

void blk_stat_free_callback(struct blk_stat_callback *cb)
{
	if (cb)
		call_rcu(&cb->rcu, blk_stat_free_callback_rcu);
}

void blk_stat_enable_accounting(struct request_queue *q)
{
	unsigned long flags;

	spin_lock_irqsave(&q->stats->lock, flags);
	q->stats->enable_accounting = true;
	blk_queue_flag_set(QUEUE_FLAG_STATS, q);
	spin_unlock_irqrestore(&q->stats->lock, flags);
}
EXPORT_SYMBOL_GPL(blk_stat_enable_accounting);

struct blk_queue_stats *blk_alloc_queue_stats(void)
{
	struct blk_queue_stats *stats;

	stats = kmalloc(sizeof(*stats), GFP_KERNEL);
	if (!stats)
		return NULL;

	INIT_LIST_HEAD(&stats->callbacks);
	spin_lock_init(&stats->lock);
	stats->enable_accounting = false;

	return stats;
}

void blk_free_queue_stats(struct blk_queue_stats *stats)
{
	if (!stats)
		return;

	WARN_ON(!list_empty(&stats->callbacks));

	kfree(stats);
}
