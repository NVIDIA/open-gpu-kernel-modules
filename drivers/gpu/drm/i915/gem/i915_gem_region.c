// SPDX-License-Identifier: MIT
/*
 * Copyright © 2019 Intel Corporation
 */

#include "intel_memory_region.h"
#include "i915_gem_region.h"
#include "i915_drv.h"
#include "i915_trace.h"

void i915_gem_object_init_memory_region(struct drm_i915_gem_object *obj,
					struct intel_memory_region *mem)
{
	obj->mm.region = intel_memory_region_get(mem);

	if (obj->base.size <= mem->min_page_size)
		obj->flags |= I915_BO_ALLOC_CONTIGUOUS;

	mutex_lock(&mem->objects.lock);

	if (obj->flags & I915_BO_ALLOC_VOLATILE)
		list_add(&obj->mm.region_link, &mem->objects.purgeable);
	else
		list_add(&obj->mm.region_link, &mem->objects.list);

	mutex_unlock(&mem->objects.lock);
}

void i915_gem_object_release_memory_region(struct drm_i915_gem_object *obj)
{
	struct intel_memory_region *mem = obj->mm.region;

	mutex_lock(&mem->objects.lock);
	list_del(&obj->mm.region_link);
	mutex_unlock(&mem->objects.lock);

	intel_memory_region_put(mem);
}

struct drm_i915_gem_object *
i915_gem_object_create_region(struct intel_memory_region *mem,
			      resource_size_t size,
			      unsigned int flags)
{
	struct drm_i915_gem_object *obj;
	int err;

	/*
	 * NB: Our use of resource_size_t for the size stems from using struct
	 * resource for the mem->region. We might need to revisit this in the
	 * future.
	 */

	GEM_BUG_ON(flags & ~I915_BO_ALLOC_FLAGS);

	if (!mem)
		return ERR_PTR(-ENODEV);

	size = round_up(size, mem->min_page_size);

	GEM_BUG_ON(!size);
	GEM_BUG_ON(!IS_ALIGNED(size, I915_GTT_MIN_ALIGNMENT));

	if (i915_gem_object_size_2big(size))
		return ERR_PTR(-E2BIG);

	obj = i915_gem_object_alloc();
	if (!obj)
		return ERR_PTR(-ENOMEM);

	err = mem->ops->init_object(mem, obj, size, flags);
	if (err)
		goto err_object_free;

	trace_i915_gem_object_create(obj);
	return obj;

err_object_free:
	i915_gem_object_free(obj);
	return ERR_PTR(err);
}
