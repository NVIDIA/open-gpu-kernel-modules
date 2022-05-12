// SPDX-License-Identifier: GPL-2.0-or-later
/* FS-Cache worker operation management routines
 *
 * Copyright (C) 2008 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * See Documentation/filesystems/caching/operations.rst
 */

#define FSCACHE_DEBUG_LEVEL OPERATION
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include "internal.h"

atomic_t fscache_op_debug_id;
EXPORT_SYMBOL(fscache_op_debug_id);

static void fscache_operation_dummy_cancel(struct fscache_operation *op)
{
}

/**
 * fscache_operation_init - Do basic initialisation of an operation
 * @op: The operation to initialise
 * @release: The release function to assign
 *
 * Do basic initialisation of an operation.  The caller must still set flags,
 * object and processor if needed.
 */
void fscache_operation_init(struct fscache_cookie *cookie,
			    struct fscache_operation *op,
			    fscache_operation_processor_t processor,
			    fscache_operation_cancel_t cancel,
			    fscache_operation_release_t release)
{
	INIT_WORK(&op->work, fscache_op_work_func);
	atomic_set(&op->usage, 1);
	op->state = FSCACHE_OP_ST_INITIALISED;
	op->debug_id = atomic_inc_return(&fscache_op_debug_id);
	op->processor = processor;
	op->cancel = cancel ?: fscache_operation_dummy_cancel;
	op->release = release;
	INIT_LIST_HEAD(&op->pend_link);
	fscache_stat(&fscache_n_op_initialised);
	trace_fscache_op(cookie, op, fscache_op_init);
}
EXPORT_SYMBOL(fscache_operation_init);

/**
 * fscache_enqueue_operation - Enqueue an operation for processing
 * @op: The operation to enqueue
 *
 * Enqueue an operation for processing by the FS-Cache thread pool.
 *
 * This will get its own ref on the object.
 */
void fscache_enqueue_operation(struct fscache_operation *op)
{
	struct fscache_cookie *cookie = op->object->cookie;
	
	_enter("{OBJ%x OP%x,%u}",
	       op->object->debug_id, op->debug_id, atomic_read(&op->usage));

	ASSERT(list_empty(&op->pend_link));
	ASSERT(op->processor != NULL);
	ASSERT(fscache_object_is_available(op->object));
	ASSERTCMP(atomic_read(&op->usage), >, 0);
	ASSERTIFCMP(op->state != FSCACHE_OP_ST_IN_PROGRESS,
		    op->state, ==,  FSCACHE_OP_ST_CANCELLED);

	fscache_stat(&fscache_n_op_enqueue);
	switch (op->flags & FSCACHE_OP_TYPE) {
	case FSCACHE_OP_ASYNC:
		trace_fscache_op(cookie, op, fscache_op_enqueue_async);
		_debug("queue async");
		atomic_inc(&op->usage);
		if (!queue_work(fscache_op_wq, &op->work))
			fscache_put_operation(op);
		break;
	case FSCACHE_OP_MYTHREAD:
		trace_fscache_op(cookie, op, fscache_op_enqueue_mythread);
		_debug("queue for caller's attention");
		break;
	default:
		pr_err("Unexpected op type %lx", op->flags);
		BUG();
		break;
	}
}
EXPORT_SYMBOL(fscache_enqueue_operation);

/*
 * start an op running
 */
static void fscache_run_op(struct fscache_object *object,
			   struct fscache_operation *op)
{
	ASSERTCMP(op->state, ==, FSCACHE_OP_ST_PENDING);

	op->state = FSCACHE_OP_ST_IN_PROGRESS;
	object->n_in_progress++;
	if (test_and_clear_bit(FSCACHE_OP_WAITING, &op->flags))
		wake_up_bit(&op->flags, FSCACHE_OP_WAITING);
	if (op->processor)
		fscache_enqueue_operation(op);
	else
		trace_fscache_op(object->cookie, op, fscache_op_run);
	fscache_stat(&fscache_n_op_run);
}

/*
 * report an unexpected submission
 */
static void fscache_report_unexpected_submission(struct fscache_object *object,
						 struct fscache_operation *op,
						 const struct fscache_state *ostate)
{
	static bool once_only;
	struct fscache_operation *p;
	unsigned n;

	if (once_only)
		return;
	once_only = true;

	kdebug("unexpected submission OP%x [OBJ%x %s]",
	       op->debug_id, object->debug_id, object->state->name);
	kdebug("objstate=%s [%s]", object->state->name, ostate->name);
	kdebug("objflags=%lx", object->flags);
	kdebug("objevent=%lx [%lx]", object->events, object->event_mask);
	kdebug("ops=%u inp=%u exc=%u",
	       object->n_ops, object->n_in_progress, object->n_exclusive);

	if (!list_empty(&object->pending_ops)) {
		n = 0;
		list_for_each_entry(p, &object->pending_ops, pend_link) {
			ASSERTCMP(p->object, ==, object);
			kdebug("%p %p", op->processor, op->release);
			n++;
		}

		kdebug("n=%u", n);
	}

	dump_stack();
}

/*
 * submit an exclusive operation for an object
 * - other ops are excluded from running simultaneously with this one
 * - this gets any extra refs it needs on an op
 */
int fscache_submit_exclusive_op(struct fscache_object *object,
				struct fscache_operation *op)
{
	const struct fscache_state *ostate;
	unsigned long flags;
	int ret;

	_enter("{OBJ%x OP%x},", object->debug_id, op->debug_id);

	trace_fscache_op(object->cookie, op, fscache_op_submit_ex);

	ASSERTCMP(op->state, ==, FSCACHE_OP_ST_INITIALISED);
	ASSERTCMP(atomic_read(&op->usage), >, 0);

	spin_lock(&object->lock);
	ASSERTCMP(object->n_ops, >=, object->n_in_progress);
	ASSERTCMP(object->n_ops, >=, object->n_exclusive);
	ASSERT(list_empty(&op->pend_link));

	ostate = object->state;
	smp_rmb();

	op->state = FSCACHE_OP_ST_PENDING;
	flags = READ_ONCE(object->flags);
	if (unlikely(!(flags & BIT(FSCACHE_OBJECT_IS_LIVE)))) {
		fscache_stat(&fscache_n_op_rejected);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	} else if (unlikely(fscache_cache_is_broken(object))) {
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -EIO;
	} else if (flags & BIT(FSCACHE_OBJECT_IS_AVAILABLE)) {
		op->object = object;
		object->n_ops++;
		object->n_exclusive++;	/* reads and writes must wait */

		if (object->n_in_progress > 0) {
			atomic_inc(&op->usage);
			list_add_tail(&op->pend_link, &object->pending_ops);
			fscache_stat(&fscache_n_op_pend);
		} else if (!list_empty(&object->pending_ops)) {
			atomic_inc(&op->usage);
			list_add_tail(&op->pend_link, &object->pending_ops);
			fscache_stat(&fscache_n_op_pend);
			fscache_start_operations(object);
		} else {
			ASSERTCMP(object->n_in_progress, ==, 0);
			fscache_run_op(object, op);
		}

		/* need to issue a new write op after this */
		clear_bit(FSCACHE_OBJECT_PENDING_WRITE, &object->flags);
		ret = 0;
	} else if (flags & BIT(FSCACHE_OBJECT_IS_LOOKED_UP)) {
		op->object = object;
		object->n_ops++;
		object->n_exclusive++;	/* reads and writes must wait */
		atomic_inc(&op->usage);
		list_add_tail(&op->pend_link, &object->pending_ops);
		fscache_stat(&fscache_n_op_pend);
		ret = 0;
	} else if (flags & BIT(FSCACHE_OBJECT_KILLED_BY_CACHE)) {
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	} else {
		fscache_report_unexpected_submission(object, op, ostate);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	}

	spin_unlock(&object->lock);
	return ret;
}

/*
 * submit an operation for an object
 * - objects may be submitted only in the following states:
 *   - during object creation (write ops may be submitted)
 *   - whilst the object is active
 *   - after an I/O error incurred in one of the two above states (op rejected)
 * - this gets any extra refs it needs on an op
 */
int fscache_submit_op(struct fscache_object *object,
		      struct fscache_operation *op)
{
	const struct fscache_state *ostate;
	unsigned long flags;
	int ret;

	_enter("{OBJ%x OP%x},{%u}",
	       object->debug_id, op->debug_id, atomic_read(&op->usage));

	trace_fscache_op(object->cookie, op, fscache_op_submit);

	ASSERTCMP(op->state, ==, FSCACHE_OP_ST_INITIALISED);
	ASSERTCMP(atomic_read(&op->usage), >, 0);

	spin_lock(&object->lock);
	ASSERTCMP(object->n_ops, >=, object->n_in_progress);
	ASSERTCMP(object->n_ops, >=, object->n_exclusive);
	ASSERT(list_empty(&op->pend_link));

	ostate = object->state;
	smp_rmb();

	op->state = FSCACHE_OP_ST_PENDING;
	flags = READ_ONCE(object->flags);
	if (unlikely(!(flags & BIT(FSCACHE_OBJECT_IS_LIVE)))) {
		fscache_stat(&fscache_n_op_rejected);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	} else if (unlikely(fscache_cache_is_broken(object))) {
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -EIO;
	} else if (flags & BIT(FSCACHE_OBJECT_IS_AVAILABLE)) {
		op->object = object;
		object->n_ops++;

		if (object->n_exclusive > 0) {
			atomic_inc(&op->usage);
			list_add_tail(&op->pend_link, &object->pending_ops);
			fscache_stat(&fscache_n_op_pend);
		} else if (!list_empty(&object->pending_ops)) {
			atomic_inc(&op->usage);
			list_add_tail(&op->pend_link, &object->pending_ops);
			fscache_stat(&fscache_n_op_pend);
			fscache_start_operations(object);
		} else {
			ASSERTCMP(object->n_exclusive, ==, 0);
			fscache_run_op(object, op);
		}
		ret = 0;
	} else if (flags & BIT(FSCACHE_OBJECT_IS_LOOKED_UP)) {
		op->object = object;
		object->n_ops++;
		atomic_inc(&op->usage);
		list_add_tail(&op->pend_link, &object->pending_ops);
		fscache_stat(&fscache_n_op_pend);
		ret = 0;
	} else if (flags & BIT(FSCACHE_OBJECT_KILLED_BY_CACHE)) {
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	} else {
		fscache_report_unexpected_submission(object, op, ostate);
		ASSERT(!fscache_object_is_active(object));
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		ret = -ENOBUFS;
	}

	spin_unlock(&object->lock);
	return ret;
}

/*
 * queue an object for withdrawal on error, aborting all following asynchronous
 * operations
 */
void fscache_abort_object(struct fscache_object *object)
{
	_enter("{OBJ%x}", object->debug_id);

	fscache_raise_event(object, FSCACHE_OBJECT_EV_ERROR);
}

/*
 * Jump start the operation processing on an object.  The caller must hold
 * object->lock.
 */
void fscache_start_operations(struct fscache_object *object)
{
	struct fscache_operation *op;
	bool stop = false;

	while (!list_empty(&object->pending_ops) && !stop) {
		op = list_entry(object->pending_ops.next,
				struct fscache_operation, pend_link);

		if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags)) {
			if (object->n_in_progress > 0)
				break;
			stop = true;
		}
		list_del_init(&op->pend_link);
		fscache_run_op(object, op);

		/* the pending queue was holding a ref on the object */
		fscache_put_operation(op);
	}

	ASSERTCMP(object->n_in_progress, <=, object->n_ops);

	_debug("woke %d ops on OBJ%x",
	       object->n_in_progress, object->debug_id);
}

/*
 * cancel an operation that's pending on an object
 */
int fscache_cancel_op(struct fscache_operation *op,
		      bool cancel_in_progress_op)
{
	struct fscache_object *object = op->object;
	bool put = false;
	int ret;

	_enter("OBJ%x OP%x}", op->object->debug_id, op->debug_id);

	trace_fscache_op(object->cookie, op, fscache_op_cancel);

	ASSERTCMP(op->state, >=, FSCACHE_OP_ST_PENDING);
	ASSERTCMP(op->state, !=, FSCACHE_OP_ST_CANCELLED);
	ASSERTCMP(atomic_read(&op->usage), >, 0);

	spin_lock(&object->lock);

	ret = -EBUSY;
	if (op->state == FSCACHE_OP_ST_PENDING) {
		ASSERT(!list_empty(&op->pend_link));
		list_del_init(&op->pend_link);
		put = true;

		fscache_stat(&fscache_n_op_cancelled);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags))
			object->n_exclusive--;
		if (test_and_clear_bit(FSCACHE_OP_WAITING, &op->flags))
			wake_up_bit(&op->flags, FSCACHE_OP_WAITING);
		ret = 0;
	} else if (op->state == FSCACHE_OP_ST_IN_PROGRESS && cancel_in_progress_op) {
		ASSERTCMP(object->n_in_progress, >, 0);
		if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags))
			object->n_exclusive--;
		object->n_in_progress--;
		if (object->n_in_progress == 0)
			fscache_start_operations(object);

		fscache_stat(&fscache_n_op_cancelled);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;
		if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags))
			object->n_exclusive--;
		if (test_and_clear_bit(FSCACHE_OP_WAITING, &op->flags))
			wake_up_bit(&op->flags, FSCACHE_OP_WAITING);
		ret = 0;
	}

	if (put)
		fscache_put_operation(op);
	spin_unlock(&object->lock);
	_leave(" = %d", ret);
	return ret;
}

/*
 * Cancel all pending operations on an object
 */
void fscache_cancel_all_ops(struct fscache_object *object)
{
	struct fscache_operation *op;

	_enter("OBJ%x", object->debug_id);

	spin_lock(&object->lock);

	while (!list_empty(&object->pending_ops)) {
		op = list_entry(object->pending_ops.next,
				struct fscache_operation, pend_link);
		fscache_stat(&fscache_n_op_cancelled);
		list_del_init(&op->pend_link);

		trace_fscache_op(object->cookie, op, fscache_op_cancel_all);

		ASSERTCMP(op->state, ==, FSCACHE_OP_ST_PENDING);
		op->cancel(op);
		op->state = FSCACHE_OP_ST_CANCELLED;

		if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags))
			object->n_exclusive--;
		if (test_and_clear_bit(FSCACHE_OP_WAITING, &op->flags))
			wake_up_bit(&op->flags, FSCACHE_OP_WAITING);
		fscache_put_operation(op);
		cond_resched_lock(&object->lock);
	}

	spin_unlock(&object->lock);
	_leave("");
}

/*
 * Record the completion or cancellation of an in-progress operation.
 */
void fscache_op_complete(struct fscache_operation *op, bool cancelled)
{
	struct fscache_object *object = op->object;

	_enter("OBJ%x", object->debug_id);

	ASSERTCMP(op->state, ==, FSCACHE_OP_ST_IN_PROGRESS);
	ASSERTCMP(object->n_in_progress, >, 0);
	ASSERTIFCMP(test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags),
		    object->n_exclusive, >, 0);
	ASSERTIFCMP(test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags),
		    object->n_in_progress, ==, 1);

	spin_lock(&object->lock);

	if (!cancelled) {
		trace_fscache_op(object->cookie, op, fscache_op_completed);
		op->state = FSCACHE_OP_ST_COMPLETE;
	} else {
		op->cancel(op);
		trace_fscache_op(object->cookie, op, fscache_op_cancelled);
		op->state = FSCACHE_OP_ST_CANCELLED;
	}

	if (test_bit(FSCACHE_OP_EXCLUSIVE, &op->flags))
		object->n_exclusive--;
	object->n_in_progress--;
	if (object->n_in_progress == 0)
		fscache_start_operations(object);

	spin_unlock(&object->lock);
	_leave("");
}
EXPORT_SYMBOL(fscache_op_complete);

/*
 * release an operation
 * - queues pending ops if this is the last in-progress op
 */
void fscache_put_operation(struct fscache_operation *op)
{
	struct fscache_object *object;
	struct fscache_cache *cache;

	_enter("{OBJ%x OP%x,%d}",
	       op->object ? op->object->debug_id : 0,
	       op->debug_id, atomic_read(&op->usage));

	ASSERTCMP(atomic_read(&op->usage), >, 0);

	if (!atomic_dec_and_test(&op->usage))
		return;

	trace_fscache_op(op->object ? op->object->cookie : NULL, op, fscache_op_put);

	_debug("PUT OP");
	ASSERTIFCMP(op->state != FSCACHE_OP_ST_INITIALISED &&
		    op->state != FSCACHE_OP_ST_COMPLETE,
		    op->state, ==, FSCACHE_OP_ST_CANCELLED);

	fscache_stat(&fscache_n_op_release);

	if (op->release) {
		op->release(op);
		op->release = NULL;
	}
	op->state = FSCACHE_OP_ST_DEAD;

	object = op->object;
	if (likely(object)) {
		if (test_bit(FSCACHE_OP_DEC_READ_CNT, &op->flags))
			atomic_dec(&object->n_reads);
		if (test_bit(FSCACHE_OP_UNUSE_COOKIE, &op->flags))
			fscache_unuse_cookie(object);

		/* now... we may get called with the object spinlock held, so we
		 * complete the cleanup here only if we can immediately acquire the
		 * lock, and defer it otherwise */
		if (!spin_trylock(&object->lock)) {
			_debug("defer put");
			fscache_stat(&fscache_n_op_deferred_release);

			cache = object->cache;
			spin_lock(&cache->op_gc_list_lock);
			list_add_tail(&op->pend_link, &cache->op_gc_list);
			spin_unlock(&cache->op_gc_list_lock);
			schedule_work(&cache->op_gc);
			_leave(" [defer]");
			return;
		}

		ASSERTCMP(object->n_ops, >, 0);
		object->n_ops--;
		if (object->n_ops == 0)
			fscache_raise_event(object, FSCACHE_OBJECT_EV_CLEARED);

		spin_unlock(&object->lock);
	}

	kfree(op);
	_leave(" [done]");
}
EXPORT_SYMBOL(fscache_put_operation);

/*
 * garbage collect operations that have had their release deferred
 */
void fscache_operation_gc(struct work_struct *work)
{
	struct fscache_operation *op;
	struct fscache_object *object;
	struct fscache_cache *cache =
		container_of(work, struct fscache_cache, op_gc);
	int count = 0;

	_enter("");

	do {
		spin_lock(&cache->op_gc_list_lock);
		if (list_empty(&cache->op_gc_list)) {
			spin_unlock(&cache->op_gc_list_lock);
			break;
		}

		op = list_entry(cache->op_gc_list.next,
				struct fscache_operation, pend_link);
		list_del(&op->pend_link);
		spin_unlock(&cache->op_gc_list_lock);

		object = op->object;
		trace_fscache_op(object->cookie, op, fscache_op_gc);

		spin_lock(&object->lock);

		_debug("GC DEFERRED REL OBJ%x OP%x",
		       object->debug_id, op->debug_id);
		fscache_stat(&fscache_n_op_gc);

		ASSERTCMP(atomic_read(&op->usage), ==, 0);
		ASSERTCMP(op->state, ==, FSCACHE_OP_ST_DEAD);

		ASSERTCMP(object->n_ops, >, 0);
		object->n_ops--;
		if (object->n_ops == 0)
			fscache_raise_event(object, FSCACHE_OBJECT_EV_CLEARED);

		spin_unlock(&object->lock);
		kfree(op);

	} while (count++ < 20);

	if (!list_empty(&cache->op_gc_list))
		schedule_work(&cache->op_gc);

	_leave("");
}

/*
 * execute an operation using fs_op_wq to provide processing context -
 * the caller holds a ref to this object, so we don't need to hold one
 */
void fscache_op_work_func(struct work_struct *work)
{
	struct fscache_operation *op =
		container_of(work, struct fscache_operation, work);
	unsigned long start;

	_enter("{OBJ%x OP%x,%d}",
	       op->object->debug_id, op->debug_id, atomic_read(&op->usage));

	trace_fscache_op(op->object->cookie, op, fscache_op_work);

	ASSERT(op->processor != NULL);
	start = jiffies;
	op->processor(op);
	fscache_hist(fscache_ops_histogram, start);
	fscache_put_operation(op);

	_leave("");
}
