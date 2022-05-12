/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2018 Intel Corporation
 */

#ifndef _I915_SCHEDULER_TYPES_H_
#define _I915_SCHEDULER_TYPES_H_

#include <linux/list.h>

#include "gt/intel_engine_types.h"
#include "i915_priolist_types.h"

struct drm_i915_private;
struct i915_request;
struct intel_engine_cs;

struct i915_sched_attr {
	/**
	 * @priority: execution and service priority
	 *
	 * All clients are equal, but some are more equal than others!
	 *
	 * Requests from a context with a greater (more positive) value of
	 * @priority will be executed before those with a lower @priority
	 * value, forming a simple QoS.
	 *
	 * The &drm_i915_private.kernel_context is assigned the lowest priority.
	 */
	int priority;
};

/*
 * "People assume that time is a strict progression of cause to effect, but
 * actually, from a nonlinear, non-subjective viewpoint, it's more like a big
 * ball of wibbly-wobbly, timey-wimey ... stuff." -The Doctor, 2015
 *
 * Requests exist in a complex web of interdependencies. Each request
 * has to wait for some other request to complete before it is ready to be run
 * (e.g. we have to wait until the pixels have been rendering into a texture
 * before we can copy from it). We track the readiness of a request in terms
 * of fences, but we also need to keep the dependency tree for the lifetime
 * of the request (beyond the life of an individual fence). We use the tree
 * at various points to reorder the requests whilst keeping the requests
 * in order with respect to their various dependencies.
 *
 * There is no active component to the "scheduler". As we know the dependency
 * DAG of each request, we are able to insert it into a sorted queue when it
 * is ready, and are able to reorder its portion of the graph to accommodate
 * dynamic priority changes.
 *
 * Ok, there is now one active element to the "scheduler" in the backends.
 * We let a new context run for a small amount of time before re-evaluating
 * the run order. As we re-evaluate, we maintain the strict ordering of
 * dependencies, but attempt to rotate the active contexts (the current context
 * is put to the back of its priority queue, then reshuffling its dependents).
 * This provides minimal timeslicing and prevents a userspace hog (e.g.
 * something waiting on a user semaphore [VkEvent]) from denying service to
 * others.
 */
struct i915_sched_node {
	struct list_head signalers_list; /* those before us, we depend upon */
	struct list_head waiters_list; /* those after us, they depend upon us */
	struct list_head link;
	struct i915_sched_attr attr;
	unsigned int flags;
#define I915_SCHED_HAS_EXTERNAL_CHAIN	BIT(0)
	intel_engine_mask_t semaphores;
};

struct i915_dependency {
	struct i915_sched_node *signaler;
	struct i915_sched_node *waiter;
	struct list_head signal_link;
	struct list_head wait_link;
	struct list_head dfs_link;
	unsigned long flags;
#define I915_DEPENDENCY_ALLOC		BIT(0)
#define I915_DEPENDENCY_EXTERNAL	BIT(1)
#define I915_DEPENDENCY_WEAK		BIT(2)
};

#define for_each_waiter(p__, rq__) \
	list_for_each_entry_lockless(p__, \
				     &(rq__)->sched.waiters_list, \
				     wait_link)

#define for_each_signaler(p__, rq__) \
	list_for_each_entry_rcu(p__, \
				&(rq__)->sched.signalers_list, \
				signal_link)

#endif /* _I915_SCHEDULER_TYPES_H_ */
