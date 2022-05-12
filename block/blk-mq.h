/* SPDX-License-Identifier: GPL-2.0 */
#ifndef INT_BLK_MQ_H
#define INT_BLK_MQ_H

#include "blk-stat.h"
#include "blk-mq-tag.h"

struct blk_mq_tag_set;

struct blk_mq_ctxs {
	struct kobject kobj;
	struct blk_mq_ctx __percpu	*queue_ctx;
};

/**
 * struct blk_mq_ctx - State for a software queue facing the submitting CPUs
 */
struct blk_mq_ctx {
	struct {
		spinlock_t		lock;
		struct list_head	rq_lists[HCTX_MAX_TYPES];
	} ____cacheline_aligned_in_smp;

	unsigned int		cpu;
	unsigned short		index_hw[HCTX_MAX_TYPES];
	struct blk_mq_hw_ctx 	*hctxs[HCTX_MAX_TYPES];

	/* incremented at dispatch time */
	unsigned long		rq_dispatched[2];
	unsigned long		rq_merged;

	/* incremented at completion time */
	unsigned long		____cacheline_aligned_in_smp rq_completed[2];

	struct request_queue	*queue;
	struct blk_mq_ctxs      *ctxs;
	struct kobject		kobj;
} ____cacheline_aligned_in_smp;

void blk_mq_exit_queue(struct request_queue *q);
int blk_mq_update_nr_requests(struct request_queue *q, unsigned int nr);
void blk_mq_wake_waiters(struct request_queue *q);
bool blk_mq_dispatch_rq_list(struct blk_mq_hw_ctx *hctx, struct list_head *,
			     unsigned int);
void blk_mq_add_to_requeue_list(struct request *rq, bool at_head,
				bool kick_requeue_list);
void blk_mq_flush_busy_ctxs(struct blk_mq_hw_ctx *hctx, struct list_head *list);
struct request *blk_mq_dequeue_from_ctx(struct blk_mq_hw_ctx *hctx,
					struct blk_mq_ctx *start);

/*
 * Internal helpers for allocating/freeing the request map
 */
void blk_mq_free_rqs(struct blk_mq_tag_set *set, struct blk_mq_tags *tags,
		     unsigned int hctx_idx);
void blk_mq_free_rq_map(struct blk_mq_tags *tags, unsigned int flags);
struct blk_mq_tags *blk_mq_alloc_rq_map(struct blk_mq_tag_set *set,
					unsigned int hctx_idx,
					unsigned int nr_tags,
					unsigned int reserved_tags,
					unsigned int flags);
int blk_mq_alloc_rqs(struct blk_mq_tag_set *set, struct blk_mq_tags *tags,
		     unsigned int hctx_idx, unsigned int depth);

/*
 * Internal helpers for request insertion into sw queues
 */
void __blk_mq_insert_request(struct blk_mq_hw_ctx *hctx, struct request *rq,
				bool at_head);
void blk_mq_request_bypass_insert(struct request *rq, bool at_head,
				  bool run_queue);
void blk_mq_insert_requests(struct blk_mq_hw_ctx *hctx, struct blk_mq_ctx *ctx,
				struct list_head *list);

/* Used by blk_insert_cloned_request() to issue request directly */
blk_status_t blk_mq_request_issue_directly(struct request *rq, bool last);
void blk_mq_try_issue_list_directly(struct blk_mq_hw_ctx *hctx,
				    struct list_head *list);

/*
 * CPU -> queue mappings
 */
extern int blk_mq_hw_queue_to_node(struct blk_mq_queue_map *qmap, unsigned int);

/*
 * blk_mq_map_queue_type() - map (hctx_type,cpu) to hardware queue
 * @q: request queue
 * @type: the hctx type index
 * @cpu: CPU
 */
static inline struct blk_mq_hw_ctx *blk_mq_map_queue_type(struct request_queue *q,
							  enum hctx_type type,
							  unsigned int cpu)
{
	return q->queue_hw_ctx[q->tag_set->map[type].mq_map[cpu]];
}

/*
 * blk_mq_map_queue() - map (cmd_flags,type) to hardware queue
 * @q: request queue
 * @flags: request command flags
 * @ctx: software queue cpu ctx
 */
static inline struct blk_mq_hw_ctx *blk_mq_map_queue(struct request_queue *q,
						     unsigned int flags,
						     struct blk_mq_ctx *ctx)
{
	enum hctx_type type = HCTX_TYPE_DEFAULT;

	/*
	 * The caller ensure that if REQ_HIPRI, poll must be enabled.
	 */
	if (flags & REQ_HIPRI)
		type = HCTX_TYPE_POLL;
	else if ((flags & REQ_OP_MASK) == REQ_OP_READ)
		type = HCTX_TYPE_READ;
	
	return ctx->hctxs[type];
}

/*
 * sysfs helpers
 */
extern void blk_mq_sysfs_init(struct request_queue *q);
extern void blk_mq_sysfs_deinit(struct request_queue *q);
extern int __blk_mq_register_dev(struct device *dev, struct request_queue *q);
extern int blk_mq_sysfs_register(struct request_queue *q);
extern void blk_mq_sysfs_unregister(struct request_queue *q);
extern void blk_mq_hctx_kobj_init(struct blk_mq_hw_ctx *hctx);

void blk_mq_release(struct request_queue *q);

static inline struct blk_mq_ctx *__blk_mq_get_ctx(struct request_queue *q,
					   unsigned int cpu)
{
	return per_cpu_ptr(q->queue_ctx, cpu);
}

/*
 * This assumes per-cpu software queueing queues. They could be per-node
 * as well, for instance. For now this is hardcoded as-is. Note that we don't
 * care about preemption, since we know the ctx's are persistent. This does
 * mean that we can't rely on ctx always matching the currently running CPU.
 */
static inline struct blk_mq_ctx *blk_mq_get_ctx(struct request_queue *q)
{
	return __blk_mq_get_ctx(q, raw_smp_processor_id());
}

struct blk_mq_alloc_data {
	/* input parameter */
	struct request_queue *q;
	blk_mq_req_flags_t flags;
	unsigned int shallow_depth;
	unsigned int cmd_flags;

	/* input & output parameter */
	struct blk_mq_ctx *ctx;
	struct blk_mq_hw_ctx *hctx;
};

static inline bool blk_mq_is_sbitmap_shared(unsigned int flags)
{
	return flags & BLK_MQ_F_TAG_HCTX_SHARED;
}

static inline struct blk_mq_tags *blk_mq_tags_from_data(struct blk_mq_alloc_data *data)
{
	if (data->q->elevator)
		return data->hctx->sched_tags;

	return data->hctx->tags;
}

static inline bool blk_mq_hctx_stopped(struct blk_mq_hw_ctx *hctx)
{
	return test_bit(BLK_MQ_S_STOPPED, &hctx->state);
}

static inline bool blk_mq_hw_queue_mapped(struct blk_mq_hw_ctx *hctx)
{
	return hctx->nr_ctx && hctx->tags;
}

unsigned int blk_mq_in_flight(struct request_queue *q,
		struct block_device *part);
void blk_mq_in_flight_rw(struct request_queue *q, struct block_device *part,
		unsigned int inflight[2]);

static inline void blk_mq_put_dispatch_budget(struct request_queue *q,
					      int budget_token)
{
	if (q->mq_ops->put_budget)
		q->mq_ops->put_budget(q, budget_token);
}

static inline int blk_mq_get_dispatch_budget(struct request_queue *q)
{
	if (q->mq_ops->get_budget)
		return q->mq_ops->get_budget(q);
	return 0;
}

static inline void blk_mq_set_rq_budget_token(struct request *rq, int token)
{
	if (token < 0)
		return;

	if (rq->q->mq_ops->set_rq_budget_token)
		rq->q->mq_ops->set_rq_budget_token(rq, token);
}

static inline int blk_mq_get_rq_budget_token(struct request *rq)
{
	if (rq->q->mq_ops->get_rq_budget_token)
		return rq->q->mq_ops->get_rq_budget_token(rq);
	return -1;
}

static inline void __blk_mq_inc_active_requests(struct blk_mq_hw_ctx *hctx)
{
	if (blk_mq_is_sbitmap_shared(hctx->flags))
		atomic_inc(&hctx->queue->nr_active_requests_shared_sbitmap);
	else
		atomic_inc(&hctx->nr_active);
}

static inline void __blk_mq_dec_active_requests(struct blk_mq_hw_ctx *hctx)
{
	if (blk_mq_is_sbitmap_shared(hctx->flags))
		atomic_dec(&hctx->queue->nr_active_requests_shared_sbitmap);
	else
		atomic_dec(&hctx->nr_active);
}

static inline int __blk_mq_active_requests(struct blk_mq_hw_ctx *hctx)
{
	if (blk_mq_is_sbitmap_shared(hctx->flags))
		return atomic_read(&hctx->queue->nr_active_requests_shared_sbitmap);
	return atomic_read(&hctx->nr_active);
}
static inline void __blk_mq_put_driver_tag(struct blk_mq_hw_ctx *hctx,
					   struct request *rq)
{
	blk_mq_put_tag(hctx->tags, rq->mq_ctx, rq->tag);
	rq->tag = BLK_MQ_NO_TAG;

	if (rq->rq_flags & RQF_MQ_INFLIGHT) {
		rq->rq_flags &= ~RQF_MQ_INFLIGHT;
		__blk_mq_dec_active_requests(hctx);
	}
}

static inline void blk_mq_put_driver_tag(struct request *rq)
{
	if (rq->tag == BLK_MQ_NO_TAG || rq->internal_tag == BLK_MQ_NO_TAG)
		return;

	__blk_mq_put_driver_tag(rq->mq_hctx, rq);
}

static inline void blk_mq_clear_mq_map(struct blk_mq_queue_map *qmap)
{
	int cpu;

	for_each_possible_cpu(cpu)
		qmap->mq_map[cpu] = 0;
}

/*
 * blk_mq_plug() - Get caller context plug
 * @q: request queue
 * @bio : the bio being submitted by the caller context
 *
 * Plugging, by design, may delay the insertion of BIOs into the elevator in
 * order to increase BIO merging opportunities. This however can cause BIO
 * insertion order to change from the order in which submit_bio() is being
 * executed in the case of multiple contexts concurrently issuing BIOs to a
 * device, even if these context are synchronized to tightly control BIO issuing
 * order. While this is not a problem with regular block devices, this ordering
 * change can cause write BIO failures with zoned block devices as these
 * require sequential write patterns to zones. Prevent this from happening by
 * ignoring the plug state of a BIO issuing context if the target request queue
 * is for a zoned block device and the BIO to plug is a write operation.
 *
 * Return current->plug if the bio can be plugged and NULL otherwise
 */
static inline struct blk_plug *blk_mq_plug(struct request_queue *q,
					   struct bio *bio)
{
	/*
	 * For regular block devices or read operations, use the context plug
	 * which may be NULL if blk_start_plug() was not executed.
	 */
	if (!blk_queue_is_zoned(q) || !op_is_write(bio_op(bio)))
		return current->plug;

	/* Zoned block device write operation case: do not plug the BIO */
	return NULL;
}

/*
 * For shared tag users, we track the number of currently active users
 * and attempt to provide a fair share of the tag depth for each of them.
 */
static inline bool hctx_may_queue(struct blk_mq_hw_ctx *hctx,
				  struct sbitmap_queue *bt)
{
	unsigned int depth, users;

	if (!hctx || !(hctx->flags & BLK_MQ_F_TAG_QUEUE_SHARED))
		return true;

	/*
	 * Don't try dividing an ant
	 */
	if (bt->sb.depth == 1)
		return true;

	if (blk_mq_is_sbitmap_shared(hctx->flags)) {
		struct request_queue *q = hctx->queue;
		struct blk_mq_tag_set *set = q->tag_set;

		if (!test_bit(QUEUE_FLAG_HCTX_ACTIVE, &q->queue_flags))
			return true;
		users = atomic_read(&set->active_queues_shared_sbitmap);
	} else {
		if (!test_bit(BLK_MQ_S_TAG_ACTIVE, &hctx->state))
			return true;
		users = atomic_read(&hctx->tags->active_queues);
	}

	if (!users)
		return true;

	/*
	 * Allow at least some tags
	 */
	depth = max((bt->sb.depth + users - 1) / users, 4U);
	return __blk_mq_active_requests(hctx) < depth;
}


#endif
