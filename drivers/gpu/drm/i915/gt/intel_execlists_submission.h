/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2014 Intel Corporation
 */

#ifndef __INTEL_EXECLISTS_SUBMISSION_H__
#define __INTEL_EXECLISTS_SUBMISSION_H__

#include <linux/llist.h>
#include <linux/types.h>

struct drm_printer;

struct i915_request;
struct intel_context;
struct intel_engine_cs;
struct intel_gt;

enum {
	INTEL_CONTEXT_SCHEDULE_IN = 0,
	INTEL_CONTEXT_SCHEDULE_OUT,
	INTEL_CONTEXT_SCHEDULE_PREEMPTED,
};

int intel_execlists_submission_setup(struct intel_engine_cs *engine);

void intel_execlists_show_requests(struct intel_engine_cs *engine,
				   struct drm_printer *m,
				   void (*show_request)(struct drm_printer *m,
							const struct i915_request *rq,
							const char *prefix,
							int indent),
				   unsigned int max);

struct intel_context *
intel_execlists_create_virtual(struct intel_engine_cs **siblings,
			       unsigned int count);

struct intel_context *
intel_execlists_clone_virtual(struct intel_engine_cs *src);

int intel_virtual_engine_attach_bond(struct intel_engine_cs *engine,
				     const struct intel_engine_cs *master,
				     const struct intel_engine_cs *sibling);

#endif /* __INTEL_EXECLISTS_SUBMISSION_H__ */
