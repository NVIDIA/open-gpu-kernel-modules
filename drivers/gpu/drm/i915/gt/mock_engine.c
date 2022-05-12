// SPDX-License-Identifier: MIT
/*
 * Copyright © 2016 Intel Corporation
 */

#include "gem/i915_gem_context.h"
#include "gt/intel_ring.h"

#include "i915_drv.h"
#include "intel_context.h"
#include "intel_engine_pm.h"

#include "mock_engine.h"
#include "selftests/mock_request.h"

static int mock_timeline_pin(struct intel_timeline *tl)
{
	int err;

	if (WARN_ON(!i915_gem_object_trylock(tl->hwsp_ggtt->obj)))
		return -EBUSY;

	err = intel_timeline_pin_map(tl);
	i915_gem_object_unlock(tl->hwsp_ggtt->obj);
	if (err)
		return err;

	atomic_inc(&tl->pin_count);
	return 0;
}

static void mock_timeline_unpin(struct intel_timeline *tl)
{
	GEM_BUG_ON(!atomic_read(&tl->pin_count));
	atomic_dec(&tl->pin_count);
}

static struct intel_ring *mock_ring(struct intel_engine_cs *engine)
{
	const unsigned long sz = PAGE_SIZE / 2;
	struct intel_ring *ring;

	ring = kzalloc(sizeof(*ring) + sz, GFP_KERNEL);
	if (!ring)
		return NULL;

	kref_init(&ring->ref);
	ring->size = sz;
	ring->effective_size = sz;
	ring->vaddr = (void *)(ring + 1);
	atomic_set(&ring->pin_count, 1);

	ring->vma = i915_vma_alloc();
	if (!ring->vma) {
		kfree(ring);
		return NULL;
	}
	i915_active_init(&ring->vma->active, NULL, NULL, 0);
	__set_bit(I915_VMA_GGTT_BIT, __i915_vma_flags(ring->vma));
	__set_bit(DRM_MM_NODE_ALLOCATED_BIT, &ring->vma->node.flags);
	ring->vma->node.size = sz;

	intel_ring_update_space(ring);

	return ring;
}

static void mock_ring_free(struct intel_ring *ring)
{
	i915_active_fini(&ring->vma->active);
	i915_vma_free(ring->vma);

	kfree(ring);
}

static struct i915_request *first_request(struct mock_engine *engine)
{
	return list_first_entry_or_null(&engine->hw_queue,
					struct i915_request,
					mock.link);
}

static void advance(struct i915_request *request)
{
	list_del_init(&request->mock.link);
	i915_request_mark_complete(request);
	GEM_BUG_ON(!i915_request_completed(request));

	intel_engine_signal_breadcrumbs(request->engine);
}

static void hw_delay_complete(struct timer_list *t)
{
	struct mock_engine *engine = from_timer(engine, t, hw_delay);
	struct i915_request *request;
	unsigned long flags;

	spin_lock_irqsave(&engine->hw_lock, flags);

	/* Timer fired, first request is complete */
	request = first_request(engine);
	if (request)
		advance(request);

	/*
	 * Also immediately signal any subsequent 0-delay requests, but
	 * requeue the timer for the next delayed request.
	 */
	while ((request = first_request(engine))) {
		if (request->mock.delay) {
			mod_timer(&engine->hw_delay,
				  jiffies + request->mock.delay);
			break;
		}

		advance(request);
	}

	spin_unlock_irqrestore(&engine->hw_lock, flags);
}

static void mock_context_unpin(struct intel_context *ce)
{
}

static void mock_context_post_unpin(struct intel_context *ce)
{
}

static void mock_context_destroy(struct kref *ref)
{
	struct intel_context *ce = container_of(ref, typeof(*ce), ref);

	GEM_BUG_ON(intel_context_is_pinned(ce));

	if (test_bit(CONTEXT_ALLOC_BIT, &ce->flags)) {
		mock_ring_free(ce->ring);
		mock_timeline_unpin(ce->timeline);
	}

	intel_context_fini(ce);
	intel_context_free(ce);
}

static int mock_context_alloc(struct intel_context *ce)
{
	int err;

	ce->ring = mock_ring(ce->engine);
	if (!ce->ring)
		return -ENOMEM;

	ce->timeline = intel_timeline_create(ce->engine->gt);
	if (IS_ERR(ce->timeline)) {
		kfree(ce->engine);
		return PTR_ERR(ce->timeline);
	}

	err = mock_timeline_pin(ce->timeline);
	if (err) {
		intel_timeline_put(ce->timeline);
		ce->timeline = NULL;
		return err;
	}

	return 0;
}

static int mock_context_pre_pin(struct intel_context *ce,
				struct i915_gem_ww_ctx *ww, void **unused)
{
	return 0;
}

static int mock_context_pin(struct intel_context *ce, void *unused)
{
	return 0;
}

static void mock_context_reset(struct intel_context *ce)
{
}

static const struct intel_context_ops mock_context_ops = {
	.alloc = mock_context_alloc,

	.pre_pin = mock_context_pre_pin,
	.pin = mock_context_pin,
	.unpin = mock_context_unpin,
	.post_unpin = mock_context_post_unpin,

	.enter = intel_context_enter_engine,
	.exit = intel_context_exit_engine,

	.reset = mock_context_reset,
	.destroy = mock_context_destroy,
};

static int mock_request_alloc(struct i915_request *request)
{
	INIT_LIST_HEAD(&request->mock.link);
	request->mock.delay = 0;

	return 0;
}

static int mock_emit_flush(struct i915_request *request,
			   unsigned int flags)
{
	return 0;
}

static u32 *mock_emit_breadcrumb(struct i915_request *request, u32 *cs)
{
	return cs;
}

static void mock_submit_request(struct i915_request *request)
{
	struct mock_engine *engine =
		container_of(request->engine, typeof(*engine), base);
	unsigned long flags;

	i915_request_submit(request);

	spin_lock_irqsave(&engine->hw_lock, flags);
	list_add_tail(&request->mock.link, &engine->hw_queue);
	if (list_is_first(&request->mock.link, &engine->hw_queue)) {
		if (request->mock.delay)
			mod_timer(&engine->hw_delay,
				  jiffies + request->mock.delay);
		else
			advance(request);
	}
	spin_unlock_irqrestore(&engine->hw_lock, flags);
}

static void mock_reset_prepare(struct intel_engine_cs *engine)
{
}

static void mock_reset_rewind(struct intel_engine_cs *engine, bool stalled)
{
	GEM_BUG_ON(stalled);
}

static void mock_reset_cancel(struct intel_engine_cs *engine)
{
	struct mock_engine *mock =
		container_of(engine, typeof(*mock), base);
	struct i915_request *rq;
	unsigned long flags;

	del_timer_sync(&mock->hw_delay);

	spin_lock_irqsave(&engine->active.lock, flags);

	/* Mark all submitted requests as skipped. */
	list_for_each_entry(rq, &engine->active.requests, sched.link)
		i915_request_put(i915_request_mark_eio(rq));
	intel_engine_signal_breadcrumbs(engine);

	/* Cancel and submit all pending requests. */
	list_for_each_entry(rq, &mock->hw_queue, mock.link) {
		if (i915_request_mark_eio(rq)) {
			__i915_request_submit(rq);
			i915_request_put(rq);
		}
	}
	INIT_LIST_HEAD(&mock->hw_queue);

	spin_unlock_irqrestore(&engine->active.lock, flags);
}

static void mock_reset_finish(struct intel_engine_cs *engine)
{
}

static void mock_engine_release(struct intel_engine_cs *engine)
{
	struct mock_engine *mock =
		container_of(engine, typeof(*mock), base);

	GEM_BUG_ON(timer_pending(&mock->hw_delay));

	intel_breadcrumbs_free(engine->breadcrumbs);

	intel_context_unpin(engine->kernel_context);
	intel_context_put(engine->kernel_context);

	intel_engine_fini_retire(engine);
}

struct intel_engine_cs *mock_engine(struct drm_i915_private *i915,
				    const char *name,
				    int id)
{
	struct mock_engine *engine;

	GEM_BUG_ON(id >= I915_NUM_ENGINES);
	GEM_BUG_ON(!i915->gt.uncore);

	engine = kzalloc(sizeof(*engine) + PAGE_SIZE, GFP_KERNEL);
	if (!engine)
		return NULL;

	/* minimal engine setup for requests */
	engine->base.i915 = i915;
	engine->base.gt = &i915->gt;
	engine->base.uncore = i915->gt.uncore;
	snprintf(engine->base.name, sizeof(engine->base.name), "%s", name);
	engine->base.id = id;
	engine->base.mask = BIT(id);
	engine->base.legacy_idx = INVALID_ENGINE;
	engine->base.instance = id;
	engine->base.status_page.addr = (void *)(engine + 1);

	engine->base.cops = &mock_context_ops;
	engine->base.request_alloc = mock_request_alloc;
	engine->base.emit_flush = mock_emit_flush;
	engine->base.emit_fini_breadcrumb = mock_emit_breadcrumb;
	engine->base.submit_request = mock_submit_request;

	engine->base.reset.prepare = mock_reset_prepare;
	engine->base.reset.rewind = mock_reset_rewind;
	engine->base.reset.cancel = mock_reset_cancel;
	engine->base.reset.finish = mock_reset_finish;

	engine->base.release = mock_engine_release;

	i915->gt.engine[id] = &engine->base;
	i915->gt.engine_class[0][id] = &engine->base;

	/* fake hw queue */
	spin_lock_init(&engine->hw_lock);
	timer_setup(&engine->hw_delay, hw_delay_complete, 0);
	INIT_LIST_HEAD(&engine->hw_queue);

	intel_engine_add_user(&engine->base);

	return &engine->base;
}

int mock_engine_init(struct intel_engine_cs *engine)
{
	struct intel_context *ce;

	intel_engine_init_active(engine, ENGINE_MOCK);
	intel_engine_init_execlists(engine);
	intel_engine_init__pm(engine);
	intel_engine_init_retire(engine);

	engine->breadcrumbs = intel_breadcrumbs_create(NULL);
	if (!engine->breadcrumbs)
		return -ENOMEM;

	ce = create_kernel_context(engine);
	if (IS_ERR(ce))
		goto err_breadcrumbs;

	/* We insist the kernel context is using the status_page */
	engine->status_page.vma = ce->timeline->hwsp_ggtt;

	engine->kernel_context = ce;
	return 0;

err_breadcrumbs:
	intel_breadcrumbs_free(engine->breadcrumbs);
	return -ENOMEM;
}

void mock_engine_flush(struct intel_engine_cs *engine)
{
	struct mock_engine *mock =
		container_of(engine, typeof(*mock), base);
	struct i915_request *request, *rn;

	del_timer_sync(&mock->hw_delay);

	spin_lock_irq(&mock->hw_lock);
	list_for_each_entry_safe(request, rn, &mock->hw_queue, mock.link)
		advance(request);
	spin_unlock_irq(&mock->hw_lock);
}

void mock_engine_reset(struct intel_engine_cs *engine)
{
}
